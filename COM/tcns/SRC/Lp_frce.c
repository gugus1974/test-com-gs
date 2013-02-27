/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* Removed byte accesses to the TS                                                              */
/*==============================================================================================*/


#ifdef __C166__
#pragma WL(0)
#endif

/*   LP_FRCE.C
 ---------------------------------------------------------------------------- 
                                                                              
     Property of  :   ABB Verkehrssyteme AG, CH-8050 Zrich / Switzerland     
     COPYRIGHT    :   (c) 1991 ABB Verkehrssysteme AG                         
                                                                              
 ---------------------------------------------------------------------------- 
                                                                              
     Project      :   MicOS                                                   
     Subproject   :   Process Data                                            
                                                                              
 ---------------------------------------------------------------------------- 
                                                                              
     File         :   LP_FRCE.C   (Link Process Get and Put)                  
                                                                              
     Document     :   [1]: IEC WG 22-5, Upper Layers, Version 1.3             
                      [2]: Link Layer Process Specification                   
                      [3]: Link Layer Process Design                          
                                                                              
     Abstract     :   Application Interface Link Process                      
                      Force and Unforce Variable, Unforce All                 
                      Management of force state per traffic store             
                                                                              
     Remarks      :                                                           
                                                                              
     Dependencies :                                                           
                                                                              
     Accepted     :                                                           
                                                                              
     HISTORY      :   See end of file                                         
                                                                              
 ---------------------------------------------------------------------------- 
*/

#define  LP_FRCE_C

#if defined (O_PCMCIA)
#define O_LE
#endif
#include "lp_sys.h"             /*   LP interface to user                   */
#include "lc_sys.h"             /*   LC interface to user                   */
#include "pi_sys.h"             /*   PIL                                    */

#include "lpi_ifce.h"           /*   LP Interface to others                 */
#include "lpl_4tcn.h"           /*   TS access routines (low access)        */
#include "lpc_blck.h"           /*   Control Block functionality            */

#include "lp_ffld.h"            /*   Constants for modifying bit mask       */



/* new page
 ---------------------------------------------------------------------------- 
|
| name          ap_frc_single
|
| param. in     pv_name
|               p_variable
|               operation               LP_FORCE
|                                       LP_UNFORCE
|        out    -
|               
| return value  result
| 
 ------------   A B S T R A C T   ------------------------------------------- 


  The user interface of this function is

        - ap_frc_variable   ()
        - ap_unfrc_variable ()

  The function forces one bus process data variable by converting the input
  parameters into an lp_frc_single compatible structure and calling
  lp_frc_single (). Please refer to the struct STR_PV_NAME definition for a 
  description of the input variable pv_name.

  Call lp_frc_single () directly if efficiency is a requirement.


*/

TYPE_PD_RESULT        _ap_frc_single   (const  struct STR_PV_NAME      pv_name,
                                        void *                  p_var,
                                        short                   operation)
{
struct  STR_LP_PV_SET pv_set;

        pv_set.pv_name  = pv_name;
        pv_set.p_var    = p_var;

        return (lp_frc_single (&pv_set, operation));
}


/* new page
 ---------------------------------------------------------------------------- 
|
| name          lp_frc_single
|
| param. in     pv_set
|               operation               LP_FORCE
|                                       LP_UNFORCE
|        out    -
|               
| return value  result
| 
 ------------   A B S T R A C T   ------------------------------------------- 


  The user interface of this function is
        - lp_frc_variable   ()
        - lp_unfrc_variable ()

  The function forces or unforces one bus process data variable. 

  The traffic store id is first tested whether it is known to the
  control block and whether the configuration was successfully completed.
  Failure here leads to an immediate routine termination with the function
  value LPS_UNKNOWN_TS_ID or LPS_CONFIG.

  Get force table base pointer from control block.
  Get page offset (0 / 20H) write, add to force table base pointer

  Force operation?
  YES:                                  NO:
  Mask appl pointer is                  Mask appl pointer is
  force constant one array              force constant zero array
  Val  appl pointer is                  Val  appl pointer is
  passed by caller                      force constant zero array

  Disable interrupt
  Write force table value, size as specified by pv_name
  Enable interrupt

  Disable interrupt
  Write force table mask, size as specified by pv_name
  Enable interrupt

*/

TYPE_PD_RESULT _lp_frc_single   (const  struct STR_LP_PV_SET *  p_pv_set,
								 short                          operation)
{
short           result = LPS_OK;

void *          		p_cb;
void *          		p_frc_var;
void *          		p_frc_msk;

struct  STR_LP_DS_X     ds_x;
struct  STR_LP_PV_X     pv_x;
LC_TYPE_TS_PRT *        p_prt;
LC_TYPE_TS_PRT *        p_frc;
                                        #if !defined (O_BAP)
LC_TYPE_TS_PCS * 			p_pcs;
TM_TYPE_SERVICE_AREA *  p_sa;
unsigned int	        pcs_type;
unsigned short		    is_prt_addr;      /* in service port address */
unsigned short		    my_prt_addr;      /* target port address     */
                                        #endif
struct  STR_DS_NAME ds_name;

    ds_name.ts_id    = lp_get_pvn_dsn (&p_pv_set->pv_name, ts_id);
    ds_name.prt_addr = lp_get_pvn_dsn (&p_pv_set->pv_name, prt_addr);

	if ((result = lpc_open (ds_name.ts_id, &p_cb)) == LPS_OK)
	{
		lpx_gen_ds_x (&ds_x, &ds_name);
		p_frc = (LC_TYPE_TS_PRT *) ds_x.p_frc;
        p_prt = ds_x.p_prt;
		lpx_gen_pv_x (&pv_x, p_pv_set, 1);
										#if !defined (O_BAP)
		p_pcs = ds_x.p_pcs;
										#endif
		if (operation == LP_FORCE)              /*   First subst. value   */
		{
				lpl_frc_enable (p_cb, p_pcs, LC_PCS_VAL_FE_SET);
				lpc_frc_flag_set (p_cb);
				p_frc_var =  p_pv_set->p_var;
				p_frc_msk =  (void *) &lp_frc_field;
		}
		else
		{
				p_frc_var =  (void *) &lp_unfrc_field;
				p_frc_msk =  (void *) &lp_unfrc_field;
		}

		lpi_lock ();

                                        #if defined (O_TCN)
		if  (lpc_get_hw_type (p_cb) == LP_HW_TYPE_HDLC)
		{
			pv_x.p_appl = p_frc_var;
			_lpl_tb_put_pv_x (&p_frc[0][LPI_FRC_DTA]    , &pv_x, 1);

			pv_x.p_appl = p_frc_msk;
			_lpl_tb_put_pv_x (&p_frc[0][LPI_FRC_MSK_WTB], &pv_x, 1);
		}
		else
										#endif
		{

LC_TYPE_TS_PGE tmp_prt_pge;

														#if !defined (O_BAP)
			lpl_get_prt_type (p_cb, &pcs_type, p_pcs);
			lpl_put_prt_type (p_cb, p_pcs, LPL_PCS_PASSIVE);
			p_sa        = lpi_lc_get_p_sa ();

			is_prt_addr = lpi_lc_get_mfr_pa (p_sa);
			my_prt_addr = ds_name.prt_addr;

			if (my_prt_addr == is_prt_addr)             /* wait till over */
			{
				lpi_lc_wait (20);
			}
														#endif
			pv_x.p_appl = p_frc_var;
//			_lpl_put_pv_x (&p_frc[0][LPI_FRC_DTA], &pv_x, 1);
lpi_get_ts_array (p_cb, &tmp_prt_pge, &p_frc[0][LPI_FRC_DTA], ds_x.prt_sze);
_lpl_put_pv_x (&tmp_prt_pge, &pv_x, 1);
lpi_put_ts_array (p_cb, &p_frc[0][LPI_FRC_DTA], &tmp_prt_pge, ds_x.prt_sze);

			pv_x.p_appl = p_frc_msk;
//			_lpl_put_pv_x (&p_frc[0][LPI_FRC_MSK], &pv_x, 1);
lpi_get_ts_array (p_cb, &tmp_prt_pge, &p_frc[0][LPI_FRC_MSK], ds_x.prt_sze);
_lpl_put_pv_x (&tmp_prt_pge, &pv_x, 1);
lpi_put_ts_array (p_cb, &p_frc[0][LPI_FRC_MSK], &tmp_prt_pge, ds_x.prt_sze);

	        if (operation == LP_FORCE)              /*   First subst. value   */
		    {
			    pv_x.p_appl = p_frc_var;
//			    _lpl_put_pv_x (&p_prt[0][0], &pv_x, 1);
lpi_get_ts_array (p_cb, &tmp_prt_pge, &p_prt[0][0], ds_x.prt_sze);
_lpl_put_pv_x (&tmp_prt_pge, &pv_x, 1);
lpi_put_ts_array (p_cb, &p_prt[0][0], &tmp_prt_pge, ds_x.prt_sze);
//			    _lpl_put_pv_x (&p_prt[0][1], &pv_x, 1);
lpi_get_ts_array (p_cb, &tmp_prt_pge, &p_prt[0][1], ds_x.prt_sze);
_lpl_put_pv_x (&tmp_prt_pge, &pv_x, 1);
lpi_put_ts_array (p_cb, &p_prt[0][1], &tmp_prt_pge, ds_x.prt_sze);
            }
														#if !defined (O_BAP)
            lpl_put_prt_type (p_cb, p_pcs, pcs_type);
														#endif
		}
		lpi_unlock ();
	}
	return (result);
}

/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Name     Reference
| ----  ------  --------  -------  -------  ---------
|  1    BAEI-2  91-09-03  created  Marsden      --
|  2    BAEI-2  92-11-10  modif'd  Marsden      --
|  3    BATC    93-07-30  modif'd  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lp_frc_single                                               */

/* new page
 ---------------------------------------------------------------------------- 
|
| name          lp_unfrc_all
|
| param. in     ts_id
|
|        out    -
|
| return value  result
| 
 ------------   A B S T R A C T   ------------------------------------------- 


  The function unforces all bus process data variables of the target traffic
  store passed in the parameter "ts_id".

  The traffic store id is first tested whether it is known to the
  control block and whether the configuration was successfully completed.
  Failure here leads to an immediate routine termination with the function
  value LPS_UNKNOWN_TS_ID or LPS_CONFIG.

  The force table base pointer is read from the control block.
  The port max is read from the control block. The force table size is
  the byte offset to the "theoretical" port after port max.

  Disable the interrupts

  The force table is cleared.

  Enable the interrupts

  Clear the force flag for the target traffic store in the control block

*/


TYPE_PD_RESULT   _lp_unfrc_all            (unsigned short         ts_id)

{
void *          pb_frc;
void *          p_cb;
#if !defined (O_BAP)
LC_TYPE_TS_PCS *p_pcs;
int				i;
#endif

unsigned short  prt_indx;
unsigned short  byte_size;

TYPE_PD_RESULT  result = LPS_OK;

	if  ((result = lpc_open ((unsigned int) ts_id, &p_cb)) == LPS_OK)
	{
		prt_indx = lpc_get_prt_indx_max (p_cb) + 1;
													#if !defined (O_BAP)
		for (i = 0, p_pcs = lpc_get_pb_pcs (p_cb);
			 i < prt_indx;
			 i++, p_pcs++)
		{
			lpl_frc_enable (p_cb, p_pcs, LC_PCS_VAL_FE_CLR);
		}
													#endif
		pb_frc     = lpc_get_pb_frc (p_cb);

		byte_size  = lpl_gen_prt_offset (p_cb, prt_indx);

		lpi_lock ();
        lpi_set_ts_array (p_cb, pb_frc, 0, byte_size);
		lpi_unlock ();
                                        #if defined (O_PCMCIA)
        {
        LC_TYPE_TS_PRT * p_frc_shadow;
            p_frc_shadow = lpc_get_pb_frc_shadow (p_cb);
    		lpi_memset (p_frc_shadow, 0, byte_size);
        }
                                        #endif
		lpc_frc_flag_clr (p_cb);
	}
	return (result);
}

/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|   1   BAEI-2  91-09-03  created  LPDBR5.TXT  5.4  Marsden      --
|   2   BATC    93-07-30  modif'd  Marsden      --
 ----------------------------------------------------------------------------
End of function lp_unfrc_all                                                */


/*
 ----------------------------------------------------------------------------
|
|   M O D I F I C A T I O N S
|   - - - - - - - - - - - - -
|
 ----------------------------------------------------------------------------

    file name: LP_FRCE.C                Nr:     1       dated:  91-08-19

    name of object modified             Nr:     ?       dated:  9?-??-??

    description of what was done to object

 ----------------------------------------------------------------------------

    file name: LP_FRCE.C                Nr:     2       dated:  92-10-05

    lp_frc_single                       Nr:     2       dated:  92-10-05

    lp_frc_single: The force table value is cleared when unforced

 ----------------------------------------------------------------------------

    file name: LP_FRCE.C                Nr:     3       dated:  93-01-27

	lp_frc_single                       Nr:     3       dated:  93-01-27

    hamster texts added to hamster call

 ----------------------------------------------------------------------------

	file name: LP_FRCE.C                Nr:     4       dated:  93-07-30

	lp_frc_single                       Nr:     3       dated:  93-07-30
	lp_unfrc_all                        Nr:     2       dated:  93-07-30

	Evaluation of traffic store id not hamster but return value
	Functionality executed only upon defined ts_id

 ----------------------------------------------------------------------------

	file name: LP_FRCE.C                Nr:     5       dated:  94-02-02

	lp_frc_single
	lp_unfrc_all

	Modification of force table structure to support TRAIN BUS

	pi_disable/enable replaced by lpi_lock/unlock

 ----------------------------------------------------------------------------

	file name: LP_FRCE.C                Nr:     6       dated:  94-02-21

	lp_frc_single

	New for MVBC:   Support of PCS bit "FE"
					Test for active transfer, wait when necessary

	lp_unfrc_all

	New for MVBC: Support of PCS bit "FE"


*/
