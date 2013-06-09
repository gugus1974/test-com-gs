/*   LP_VAR.C
 ----------------------------------------------------------------------------

     Property of  :   ABB Verkehrssyteme AG, CH-8050 Zrich / Switzerland
     COPYRIGHT    :   (c) 1991 ABB Verkehrssysteme AG

 ----------------------------------------------------------------------------

     Project      :   MicOS
     Subproject   :   Process Data

 ----------------------------------------------------------------------------

     File         :   LP_VAR.C          (Link Process Get and Put)

     Document     :   [1]: IEC WG 22-5, Upper Layers, Version 1.3
                      [2]: Link Layer Process Specification
                      [3]: Link Layer Process Design

     Abstract     :   Application Interface Link Process

     Remarks      :

     Dependencies :

     Accepted     :

     HISTORY      :   See end of file

 ----------------------------------------------------------------------------
*/

#define  LP_VAR_C
#define  LPI_MODULE_NAME "lp_var"

#if defined (O_PCMCIA)
#define O_LE
#endif
#include "lp_sys.h"             /*   LP user interface                      */
#include "pi_sys.h"             /*   PIL                                    */

#include "lpl_4tcn.h"           /*   TS access routines                     */
#include "lpc_blck.h"           /*   control block functionality            */
#include "lpi_ifce.h"           /*   LP import interface                    */
#include "lpa_as2c.h"           /*   Assembler tuning for these possible    */
#include "lp_ffld.h"

/*
 ----------------------------------------------------------------------------
|
| name          lp_put_data_set
|
| param. in     pv_name
|               data_flow               put or get variable
|        out    p_variable
|               p_refresh
|
| return value  result
|
 ------------   A B S T R A C T   -------------------------------------------


  The function updates the process variable passed in the parameter.

  Please refer to the struct STR_PV_NAME definition for a description of the
  input variable pv_name.

  Each separate traffic store id is first tested whether it is known to the
  control block and whether the configuration was successfully completed.
  Failure here leads to an immediate routine termination with the function
  value LPS_UNKNOWN_TS_ID or LPS_CONFIG.
  Use lpi_dblevel defined in lpifce for personalised error handling.

  The traffic store base pointers for the components port index table (pb_pit)
  port communication and status register (pb_pcs) and force table (pb_frc) are
  read from the control block.

  The port index stored under the port address is fetched from the control
  block and compared with passive (port index 0). If active, the traffic store
  component offsets are generated / fetched and added to
        the pcs base pointer
        the prt base pointer
        the frc base pointer.

  Disable the interrupts
  Get page offset (0 / 20H) read, add to port pointer
  Get page offset (0 / 20H) write, add to port pointer
  Copy from read page into write page
  Put process variable into write page
  Merge write page and force table
  Enable the interrupts

  The interface contains a return code, but no checks are made (efficiency
  reasons). The return code is a feature for the future.

*/

TYPE_PD_RESULT  _lp_put_data_set (const struct STR_LP_DS_SET *  p_ds_set)
{
#if defined (O_TCN) || defined (O_PCMCIA)
    void * p_cb;
#endif

#if !defined (O_TCN)
    unsigned int     prt_type;
#endif

#if defined (O_BAP)
    LC_TYPE_TS_PRT * p_frc;
#endif

    int    lpi_lock_active = LP_PASSIVE;
    unsigned short  prt_sze;

#if !defined (O_BAP)
    LC_TYPE_TS_PGE tmp_prt_pge;
#endif

#if defined (O_TCN) || defined (O_PCMCIA)
    p_cb = lpc_turbo_open (p_ds_set->ds_y.ts_id);
#endif

#if !defined (O_TCN)
    lpl_get_prt_type (p_cb, &prt_type, p_ds_set->ds_x.p_pcs);

    if (prt_type != LP_CFG_SINK)
#endif
    {
        LC_TYPE_TS_PCS * p_pcs;
        LC_TYPE_TS_PRT * p_prt;
        LC_TYPE_TS_PRT * p_prt_read;

        p_prt   = p_ds_set->ds_x.p_prt;
        p_pcs   = p_ds_set->ds_x.p_pcs;
        prt_sze = p_ds_set->ds_x.prt_sze;

        if (p_ds_set->ds_y.with_else == LP_VAR_WITH_ELSE)
        {
            p_prt_read      = p_prt;
            lpi_lock_active = LP_ACTIVE;
            lpi_lock();

#if defined (O_TCN)
            if (lpc_get_hw_type(p_cb) == LP_HW_TYPE_HDLC)
            {
                lpa_tb_add_pge_offset_wt (p_cb, p_pcs, p_prt);
                lpa_tb_add_pge_offset_rd (p_cb, p_pcs, p_prt_read);
                pi_copy16 ((void*)p_prt, (void*)p_prt_read, prt_sze);
                _lpl_tb_put_pv_x (p_prt, p_ds_set->p_pv_x,
                                         p_ds_set->ds_x.pv_x_cnt);
            }
            else
#endif
            {
                lpa_add_pge_offset_wt (p_cb, p_pcs, p_prt);
                lpa_add_pge_offset_rd (p_cb, p_pcs, p_prt_read);

#if defined (O_BAP)
                pi_copy16 (p_prt, p_prt_read, prt_sze);
               _lpl_put_pv_x (p_prt, p_ds_set->p_pv_x,
                                            p_ds_set->ds_x.pv_x_cnt);
#else
                lpi_get_ts_array (p_cb, &tmp_prt_pge, p_prt_read, prt_sze);
               _lpl_put_pv_x (&tmp_prt_pge, p_ds_set->p_pv_x,
                                            p_ds_set->ds_x.pv_x_cnt);
#endif
            }
        }
        else
        {
            lpi_lock_active = LP_PASSIVE;

#if defined (O_TCN) && !defined (O_MVB)
            if (lpc_get_hw_type(p_cb) == LP_HW_TYPE_HDLC)
            {
                lpa_tb_add_pge_offset_wt (p_cb, p_pcs, p_prt);
                _lpl_tb_put_pv_x  (p_prt, p_ds_set->p_pv_x,
                                   p_ds_set->ds_x.pv_x_cnt);
            }
            else
#endif
            {
                lpa_add_pge_offset_wt (p_cb, p_pcs, p_prt);

#if defined (O_BAP)
                _lpl_put_pv_x (p_prt, p_ds_set->p_pv_x, p_ds_set->ds_x.pv_x_cnt);
#else
                lpi_memset (&tmp_prt_pge, 0, 2 * prt_sze);
                _lpl_put_pv_x (&tmp_prt_pge, p_ds_set->p_pv_x,
                                             p_ds_set->ds_x.pv_x_cnt);
#endif
            }
        }
#if defined (O_TCN) && !defined (O_MVB)
        if (lpc_get_hw_type(p_cb) == LP_HW_TYPE_HDLC)
        {
            if (p_ds_set->ds_y.with_frce == LP_VAR_WITH_FRCE)
            {
                LC_TYPE_TS_PRT * p_frc;
                p_frc   = p_ds_set->ds_x.p_frc;
                lpl_get_prt_size (p_cb, &prt_sze, p_pcs);
                lpa_tb_put_frcd_prt (p_prt, p_frc, prt_sze);
            }
        }
        else
#endif
        {
#if defined (O_BAP)
            if (p_ds_set->ds_y.with_frce == LP_VAR_WITH_FRCE)
            {
                p_frc   = p_ds_set->ds_x.p_frc;
                lpa_put_frcd_prt (p_prt, p_frc, prt_sze);
            }
#else
            {
                lpi_put_ts_array (p_cb, p_prt, &tmp_prt_pge, prt_sze);
            }
#endif
        }
        lpa_tgl_prt_page (p_cb, p_pcs);
    }

    if (lpi_lock_active == LP_ACTIVE)
    {
        lpi_unlock();
    }

    lpl_put_prt_tack (p_cb, p_ds_set->ds_x.p_pcs, LPL_PCS_UPDATED);
    return (LPS_OK);
}

/*
 ----------------------------------------------------------------------------
| Version Info: See end of file
 ----------------------------------------------------------------------------
End of function lp_put_data_set */

/*
 ----------------------------------------------------------------------------
|
| name          lp_get_data_set
|
| param. in     pv_name
|               data_flow               put or get variable
|        out    p_variable
|               p_refresh
|
| return value  result
|
 ------------   A B S T R A C T   -------------------------------------------


  The function updates the process variable passed in the parameter.

  Please refer to the struct STR_PV_NAME definition for a description of the
  input variable pv_name.

  Each separate traffic store id is first tested whether it is known to the
  control block and whether the configuration was successfully completed.
  Failure here leads to an immediate routine termination with the function
  value LPS_UNKNOWN_TS_ID or LPS_CONFIG.
  Use lpi_dblevel defined in lpifce for personalised error handling.

  The traffic store base pointers for the components port index table (pb_pit)
  port communication and status register (pb_pcs) and force table (pb_frc) are
  read from the control block.

  The port index stored under the port address is fetched from the control
  block and compared with passive (port index 0). If active, the traffic store
  component offsets are generated / fetched and added to
        the pcs base pointer
        the prt base pointer
        the frc base pointer.

  data_flow = LP_GET

  Disable the interrupts
  Get page offset (0 / 20H) read, add to port pointer
  Merge traffic store and force table to temp image
  Get process variable or force value from image
  Enable the interrupts
  Update the refresh variable with time since last port update

  The interface contains a return code, but no checks are made (efficiency
  reasons). The return code is a feature for the future.


*/
struct          MY_DCK
{
    unsigned char data[32];
};

struct          MY_PRT
{
    struct MY_DCK dck_32[4];
};


TYPE_PD_RESULT    _lp_get_data_set (const struct STR_LP_DS_SET * p_ds_set)

{
                                        #if defined (O_TCN) || defined (O_PCMCIA)
void *          p_cb;
                                        #endif
LC_TYPE_TS_PCS *p_pcs;
                                        #if !defined (O_PCMCIA)
LC_TYPE_TS_PRT *p_frc;
                                        #endif
struct MY_PRT * p_prt;
struct MY_PRT * p_tmp;

struct MY_PRT   prt_image;
unsigned short  prt_cnt;
                                        #if !defined (O_BAP) && !defined (O_PCMCIA)
unsigned short  prt_type;
                                        #endif
                                        #if defined (O_TCN) || defined (O_PCMCIA)
        p_cb = lpc_turbo_open (p_ds_set->ds_y.ts_id);
                                        #endif
                                        #if !defined (O_PCMCIA)
        p_frc = p_ds_set->ds_x.p_frc;
                                        #endif
        p_pcs = p_ds_set->ds_x.p_pcs;
        p_prt = p_ds_set->ds_x.p_prt;
        p_tmp = &prt_image;

                                        #if defined (O_TCN)
        if  (lpc_get_hw_type (p_cb) == LP_HW_TYPE_HDLC)
        {
            lpl_get_prt_size (p_cb, &prt_cnt, p_pcs);

            lpi_lock ();
            lpa_tb_add_pge_offset_rd (p_cb, p_pcs, p_prt);

            *p_tmp = *p_prt;

            lpi_unlock ();

            if  (p_ds_set->ds_y.with_frce == LP_VAR_WITH_FRCE)
            {
                p_frc    = p_ds_set->ds_x.p_frc;

                lpa_tb_put_frcd_prt (p_tmp, p_frc, prt_cnt);

            }
            _lpl_tb_get_pv_x (&prt_image,
                              p_ds_set->p_pv_x,
                              p_ds_set->ds_x.pv_x_cnt);
        }
        else
                                        #endif
        {
            prt_cnt = p_ds_set->ds_x.prt_sze;

            lpi_lock ();
            lpa_add_pge_offset_rd (p_cb, p_pcs, p_prt);
                                        #if   defined (O_BAP)
            if  (p_ds_set->ds_y.with_frce == LP_VAR_WITH_FRCE)
                                        #elif !defined (O_PCMCIA)
            lpl_get_prt_type (p_cb, &prt_type, p_pcs);

            if  ((p_ds_set->ds_y.with_frce == LP_VAR_WITH_FRCE) &&
                (prt_type == LPL_PCS_SRCE))
                                        #endif
                                        #if !defined (O_PCMCIA)
            {
                lpa_get_frcd_prt (p_tmp, p_prt, p_frc, prt_cnt);
            }
            else
                                        #endif
            {
                lpi_get_ts_array (p_cb, p_tmp, p_prt, prt_cnt);
            }

            lpi_unlock ();

            _lpl_get_pv_x   (p_tmp, p_ds_set->p_pv_x,
                             p_ds_set->ds_x.pv_x_cnt);
        }

        if (p_ds_set->ds_x.p_refresh != NULL)
        {
            lpl_refresh (p_cb, p_ds_set->ds_x.p_pcs,p_ds_set->ds_x.p_refresh);
        }

        return (LPS_OK);
}

/*
 ----------------------------------------------------------------------------
| Version Info: See end of file
 ----------------------------------------------------------------------------
End of function lp_get_data_set */


/*
 ----------------------------------------------------------------------------
|
| name          lp_put_variable
|
| (in)          p_pv_set
|
| return value  result
|
 ------------   A B S T R A C T   -------------------------------------------


  The function updates the process variable passed in the parameter.

  Please refer to the structure pv_set  definition for a description of the
  input structure STR_LP_PV_SET (lp_sys.h)

  Each separate traffic store id is first tested whether it is known to the
  control block and whether the configuration was successfully completed.

  A data set ds_set is generated from the pv_set. The function  put_ds_set
  is called.

*/

TYPE_PD_RESULT    _lp_put_variable (const struct STR_LP_PV_SET * p_pv_set)

{

short                   result = LPS_OK;

struct STR_LP_DS_SET    p_ds_set;
struct STR_LP_PV_X      pv_x;
void *                  p_cb;

        result = lpc_open (lp_get_pvn_dsn(&p_pv_set->pv_name, ts_id), &p_cb);

        lpi_hamster_cond (result, LPI_DU_ERR, LPI_DU_RESET, "traffic store id"); /*$H. traffic store id ??? */

        lp_gen_ds_set   (&p_ds_set, &pv_x, p_pv_set, NULL, 1);

        lp_put_data_set (&p_ds_set);

        return (result);

}

TYPE_PD_RESULT  _ap_put_variable       (const  struct STR_PV_NAME      pv_name,
                                        void *                  p_var)

{
struct  STR_LP_PV_SET   pv_set;

        pv_set.pv_name = pv_name;
        pv_set.p_var   = p_var;

        return (_lp_put_variable (&pv_set));

}


/*
 ----------------------------------------------------------------------------
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|   1   BAEI-2  91-09-03  created  LPDBR5.TXT  5.1  Marsden      --
|   2   BAEI-2  92-10-05  modif'd                   Marsden      --
 ----------------------------------------------------------------------------
End of function lp_put_variable */

/*
 ----------------------------------------------------------------------------
|
| name          lp_get_variable
|
| (in)          p_pv_set
|
| return value  result
|
 ------------   A B S T R A C T   -------------------------------------------


  The function updates the process variable passed in the parameter.

  Please refer to the structure pv_set  definition for a description of the
  input structure STR_LP_PV_SET (lp_sys.h)

  Each separate traffic store id is first tested whether it is known to the
  control block and whether the configuration was successfully completed.

  A data set ds_set is generated from the pv_set. The function  put_ds_set
  is called.

*/

TYPE_PD_RESULT _lp_get_variable (const  struct STR_LP_PV_SET *  p_pv_set,
                                        unsigned short *        p_refresh)

{

short                   result = LPS_OK;

struct STR_LP_DS_SET    p_ds_set;
struct STR_LP_PV_X      pv_x;
void *                  p_cb;


        result = lpc_open (lp_get_pvn_dsn(&p_pv_set->pv_name, ts_id), &p_cb);

        lpi_hamster_cond (result, LPI_DU_ERR, LPI_DU_RESET, "traffic store id"); /*$H. traffic store id ??? */

        lp_gen_ds_set   (&p_ds_set, &pv_x, p_pv_set, p_refresh, 1);

        lp_get_data_set (&p_ds_set);

        return (result);

}


TYPE_PD_RESULT _ap_get_variable (const  struct STR_PV_NAME      pv_name,
                                        void *                  p_var,
                                        unsigned short *        p_refresh)

{
struct  STR_LP_PV_SET   pv_set;

        pv_set.pv_name = pv_name;
        pv_set.p_var   = p_var;

        return (_lp_get_variable (&pv_set, p_refresh));

}

/*
 ----------------------------------------------------------------------------
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|   1   BAEI-2  91-09-03  created  LPDBR5.TXT  5.1  Marsden      --
|   2   BAEI-2  92-10-05  modif'd                   Marsden      --
 ----------------------------------------------------------------------------
End of function lp_variable */


/*
 ----------------------------------------------------------------------------
|
|   M O D I F I C A T I O N S
|   - - - - - - - - - - - - -
|
 ----------------------------------------------------------------------------

    file name: LP_VAR.C                 Nr:     1       dated:  91-08-19

        name of object modified         Nr:     ?       dated:  9?-??-??

        description of what was done to object

 ----------------------------------------------------------------------------

        file name: LP_VAR.C             Nr:     2       dated:  93-01-27

        hamster texts added to hamster call

 ----------------------------------------------------------------------------

        lp_get_data_set                 Nr:     3       dated:  93-06-17

        time under lpi_lock shortened

 ----------------------------------------------------------------------------

        lp_get_data_set                 Nr:     4       dated:  93-06-22

        extended for WTB

 ----------------------------------------------------------------------------

        lp_put_data_set                 Nr:     5       dated:  94-02-15

        forcing removed for MVBC, as done by hardware controller
        error correction in branch "with else": lpi_unlock() called too early

        lp_get_data_set

        forcing of sinks removed for MVBC, as done by hardware controller

 ----------------------------------------------------------------------------

                                        Nr:     6       dated:  94-11-01

        New switch O_MVB for only Vehicle Bus
        Capabilities of TB extended to match capabilities of VB


 ----------------------------------------------------------------------------
*/

