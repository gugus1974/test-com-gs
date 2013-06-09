/*   LP_INIT.C
 ---------------------------------------------------------------------------- 
                                                                              
     Property of  :   ABB Verkehrssyteme AG, CH-8050 Zrich / Switzerland     
     COPYRIGHT    :   (c) 1991 ABB Verkehrssysteme AG                         
                                                                              
 ---------------------------------------------------------------------------- 
                                                                              
     Project      :   MicOS                                                   
     Subproject   :   Link Process                                            
                                                                              
 ---------------------------------------------------------------------------- 
                                                                              
     File         :   LP_INIT.C                                               
                                                                              
     Document     :   [1]: IEC Document WG 22-5: "Upper Layers", V 1.3        
                      [2]: Link Layer Process Specification                   
                      [3]: Link Layer Process Design                          
                                                                              
     Abstract     :   Initialisation  Routines, Interface Operating System    
                                                                              
     Remarks      :
                                                                              
     Dependencies :                                                           

     Accepted     :                                                           
                                                                              
     HISTORY      :   See end of file                                         
                                                                              
 ---------------------------------------------------------------------------- 
*/

#define   LP_INIT_C
#define   LPI_MODULE_NAME "lp_init"

#if defined (O_PCMCIA)
#define O_LE
#endif
#include "lp_sys.h"             /*   LP interface to user                   */
#include "lc_sys.h"             /*   LP interface to user                   */
#include "pi_sys.h"             /*   PIL                                    */

#include "lpc_blck.h"           /*   control block functionality            */
#include "lpi_ifce.h"           /*   LP interface to others                 */
#include "lpl_4tcn.h"           /*   TS access routines                     */

#include "lp_tmo.h"             /*   timeout surveillance                   */
#if defined (O_BAP)
#include "lp_irpt.h"            /*   indication functionality               */
#endif

#ifdef O_PCMCIA
  #include "PCNODE.H"
  extern unsigned short lp_pcmcia_node;
#endif


#define LP_MINUS_1_POINTER (void *) -1L


/* new page
 ---------------------------------------------------------------------------- 
| 
| prototypes
|
 ---------------------------------------------------------------------------- 
*/


void            lp_hw_enable          (unsigned short               hw_type,
                                       const struct STR_LP_TS_CFG * p_ts_cfg);

int             lp_ts_create          (void *                 cb_handle,
                                       const struct STR_LP_TS_CFG * p_ts_cfg);

int             lp_ts_create_pit        (void *                 cb_handle,
                                         const struct STR_LP_TS_CFG * p_ts_cfg);

int             lp_ts_create_pcs        (void *                 cb_handle,
                                         const struct STR_LP_TS_CFG * p_ts_cfg);

int             lp_ts_create_prt        (void *                 cb_handle,
                                         const struct STR_LP_TS_CFG * p_ts_cfg);

int             lp_ts_create_frc        (void *                 cb_handle,
                                         const struct STR_LP_TS_CFG * p_ts_cfg);

void            lp_ts_free              (void *                 cb_handle);

int             lp_ts_open_port         (void *                 p_cb,
                                         const struct STR_LP_PRT_CFG *p_cfg);

/* new page
 ---------------------------------------------------------------------------- 
| 
| function      lp_init
|                                                                             
| param. in     -                                                             
|        out    -                                                             
|                                                                             
| return value  result                  LPS_OK
|
 ------------   A B S T R A C T   ------------------------------------------- 


  Is called once to reset the LP software by

        initialising the control block

        initialising the refresh timeout static variables

*/

TYPE_PD_RESULT   _lp_init                 (void)

{
        lpc_init  ();
        lp_tmo_init ();
        return (LPS_OK);

}

/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  1.0  BAEI-2  91-09-27  created  LPDBR4.TXT  4.1  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lp_init   */



/* new page
 ---------------------------------------------------------------------------- 
|
| function      lp_create
| 
| param. in     ts_id                   Traffic Store identifier
|               hw_type                 LP_HW_TYPE_NONE
|                                       LP_HW_TYPE_BAP_1
|                                       LP_HW_TYPE_HDLC
|               p_ts_cfg                Pointer to traffic store configuration
|               p_hw_cfg                Pointer to hardware configuration
|
|        out    -
| 
| return value  result                  LPS_OK
|                                       LPS_CONFIG
|                                       LPS_MEMORY
|
 ------------   A B S T R A C T   ------------------------------------------- 


  See TYPE_LP_TS_CONFIG in lp_sys.h for a detailed description of the traffic
  store attributes!

  Traffic Store already created?
  YES:                                  NO:
  return old dynamic buffers

  create new traffic store (function call lp_ts_create)

  Traffic Store created successfully?

  YES:
  Configure Traffic Store

  The hardware type is evaluated by the "put_size" function, as not all hard-
  wares support the same size range.

  For each data set in list p_cfg:

    PIT:
    Checks whether port index is already in use (error)
    Writes port index into port index table under byte offset of port address

    PCS:
    Checks whether port is already in use (error)
    Checks data set type (source, sink, source and sink = sink)
    Writes data set type
    Checks data set size (error)
    Writes data set size

  Successfully configured?
  YES:
  Announce traffic store to control
  block
  start the bus hardware
  (function call lp_hwxx_start)

  Anything unsuccessful?
  YES:
  Clear port index table
  Deallocate dynamic memory components

*/

TYPE_PD_RESULT _lp_create       (unsigned short                 ts_id,
                                 unsigned short                 hw_type,
                                 const struct STR_LP_TS_CFG *   p_ts_cfg,
                                 const union UNN_LP_HW_CFG *    p_hw_cfg)
{
void *                  p_cb;
unsigned int            prt_count;
const struct STR_LP_PRT_CFG * p_prt_cfg;
short                   result = LPS_OK;

        hw_type  = hw_type;
        p_hw_cfg = p_hw_cfg;

        result   = lpc_open ((unsigned int) ts_id, &p_cb);

        lpi_hamster_cond (result == LPS_UNKNOWN_TS_ID, LPI_DU_ERR, LPI_DU_RESET, "traffic store id"); /*$H. traffic store id ??? */
                                        #if !defined (O_TCN)
        lpi_hamster_cond (hw_type == LP_HW_TYPE_HDLC, LPI_DU_ERR, LPI_DU_RESET, "hw type         ");  /*$H. traffic store id ??? */
                                        #endif
        lpc_kill_ts (p_cb);

        lpi_hw_enable (hw_type, &p_hw_cfg->mic_1);

		lpc_put_hw_type  (p_cb, hw_type);

        lpc_put_p_ts_cfg (p_cb, p_ts_cfg);

		result = lp_ts_create (p_cb, p_ts_cfg);

		prt_count = p_ts_cfg->prt_count;
        p_prt_cfg = p_ts_cfg->p_prt_cfg;

        while   ((prt_count-- != 0) && (result == LPS_OK))
        {
                result    = lp_ts_open_port (p_cb, p_prt_cfg);

                p_prt_cfg = p_prt_cfg + 1;

        }

        if (result == LPS_OK)
        {
            lpc_put_state (p_cb, LPC_TS_IN_USE);

            lpi_mbox_activate (p_ts_cfg);

            lp_hw_enable (hw_type, p_ts_cfg);
		}
		else
		{
			if (p_ts_cfg->prt_count != 0)
			{
				lpi_clr_pit (p_cb);
			}

			lpc_kill_ts (p_cb);
		}

		return (result);
}

/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Name     Reference
| ----  ------  --------  -------  -------  ---------
|   1   BAEI-2  91-09-03  created  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lp_create                                                   */




/* new page
 ---------------------------------------------------------------------------- 
|
| function      lp_hw_start
|
| param. in     p_hw_cfg
|               hw_type
|        out    -
|
| return value  -
|
 ------------   A B S T R A C T   ------------------------------------------- 

  
  See TYPE_LP_HW_CFG for details of hardware configuration.

  The sink timeout supervision is commented out because of bug in MVBC.

*/

void           lp_hw_enable     (unsigned short               hw_type,
                                 const struct STR_LP_TS_CFG * p_ts_cfg)
{
        hw_type  = hw_type;
        p_ts_cfg = p_ts_cfg;
                                        #if defined (O_TCN)
		if (hw_type == LP_HW_TYPE_HDLC)
		{
	 		lpi_tb_announce_ts_cfg (p_ts_cfg);
    	}
                                        #endif
                                        #if defined (O_BAP)
        if  (hw_type == LP_HW_TYPE_BAP)
        {
			lpi_hw_bap_1_start ();
        }
                                        #else
                                        #if (0)
        if  (hw_type == LP_HW_TYPE_MVBC)
        {
            TM_TYPE_SERVICE_AREA * p_sa;
            unsigned short rge = p_ts_cfg->prt_indx_max < LPI_TM_STSR_RGE_MAX ?
                                 p_ts_cfg->prt_indx_max : LPI_TM_STSR_RGE_MAX ;

            p_sa   = lpi_lc_get_p_sa ();

            tm_put_sa (p_sa, int_regs.stsr.w, LPI_TM_STSR_CYC_MSK,
                                              LPI_TM_STSR_CYC_OFF,
                                              LPI_TM_STSR_CYC_VAL );

            tm_put_sa (p_sa, int_regs.stsr.w, LPI_TM_STSR_RGE_MSK,
                                              LPI_TM_STSR_RGE_OFF,
                                              rge );
        }
                                        #endif
                                        #endif
}

/*
 ----------------------------------------------------------------------------
| Vers  Dept.   YY-MM-DD  Status   Name     Reference
| ----  ------  --------  -------  -------  ---------
|  1.0  BAEI-2  91-10-01  created  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lp_hw_start  */


/* new page
 ---------------------------------------------------------------------------- 
|
| function      lp_ts_create
| 
| param. in     ts_id
|               p_ts_cfg
|        out    -
| 
| return value  result
|                                                                             |
 ------------   A B S T R A C T   ------------------------------------------- 


  Conditional checking of
        specified maximum port address
        specified maximum port index

  Write traffic store type (TYPE A, B or AB) to control block
  Save the maximum port address and port index (TS boundaries)

  Create the PIT - Port Index Table
  Create the PCS - Port Communication and Status Register Array
  Create the PRT - Port Data Array
  Create the FRC - Force Table
  Write  ownership and pb_mwd fields to control block

*/

int             lp_ts_create            (void *                         p_cb,
										 const struct STR_LP_TS_CFG *   p_cfg)
{

int             result = LPS_OK;

    lpi_hamster_cond (lpl_chk_prt_addr_max (p_cb, p_cfg->prt_addr_max) != LPS_OK,
                      LPI_DU_ERR, LPI_DU_RESET, "port address max"); /*$H. port address max ??? */
    lpi_hamster_cond (lpl_chk_prt_indx_max (p_cb, p_cfg->prt_indx_max) != LPS_OK,
                      LPI_DU_ERR, LPI_DU_RESET, "port index max"); /*$H. port index max ??? */

	lpc_put_max (p_cb, p_cfg->prt_addr_max, p_cfg->prt_indx_max);

	if      (lpc_put_ts_type  (p_cb, p_cfg->ts_type)  != LPS_OK) {result = LPS_ERROR;}

	else if (lp_ts_create_pit (p_cb, p_cfg) != LPS_OK) {result = LPS_ERROR;}

	else if (lp_ts_create_pcs (p_cb, p_cfg) != LPS_OK) {result = LPS_ERROR;}

	else if (lp_ts_create_prt (p_cb, p_cfg) != LPS_OK) {result = LPS_ERROR;}

	else if (lp_ts_create_frc (p_cb, p_cfg) != LPS_OK) {result = LPS_ERROR;}

	else
	{
		lpc_put_ts_owner (p_cb, p_cfg->ownership);
		lpc_put_pb_mwd   (p_cb, p_cfg->pb_mwd);

		lpi_mbox_create  (p_cfg);
	}

	return (result);
}

/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  1.0  BAEI-2  91-09-03  created  LPDBR4.TXT  4.3  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lp_ts_create                                                */

/* new page
 ----------------------------------------------------------------------------
|
| function      lp_ts_create_pit
|
| param. in     ts_id                   Traffic Store Id
|               p_ts_cfg                Pointer to TS configuration
|        out    -
| 
| return value  result
|                                                                             |
 ------------   A B S T R A C T   ------------------------------------------- 


  This routine creates a zero-initialised port index table.

  !!! Attention: lpc_put_max must have stored the prt_addr_max beforehand!

  Convert prt_addr_max and pit_type to size of pit array.

  NULL pointer configured?
  YES:                                  NO:
  Allocate traffic store component      Use port index table base pointer

  Clear traffic store component
  Store location in control block
  Store memory type in control block
  Store port index size (8/16 bits) in control block

*/

int             lp_ts_create_pit        (void *                         p_cb,
                                         const struct STR_LP_TS_CFG *   p_cfg)
{
void *          p_pit;
int             result = LPS_OK;

#if defined (O_PCMCIA)   /* a NULL ptr. is a valid TS addr.  (access to TS is done by special fcts.)*/

    p_pit = p_cfg->pb_pit;

#else
int pit_size = lpl_gen_pit_offset (p_cb, lpc_get_prt_addr_max (p_cb) + 1);

    if ((p_pit = p_cfg->pb_pit) == NULL)
    {
        if ((p_pit = pi_alloc (pit_size)) != NULL)
        {
                lpc_remember (p_cb, p_pit);
        }
        else
        {
                result = LPS_MEMORY;
        }
    }
#endif

    lpc_put_pb_pit (p_cb, p_pit);

    if ((p_cfg->ownership == LP_CFG_TS_OWNED) &&
        (result           == LPS_OK))
    {
                lpi_clr_pit (p_cb);
    }

    return (result);
}
/*
 ----------------------------------------------------------------------------
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  1.0  BAEI-2  91-09-03  created  LPDBR4.TXT  4.2  Marsden      --
 ----------------------------------------------------------------------------
End of function lp_ts_create_pit                                            */

/* new page
 ---------------------------------------------------------------------------- 
| 
| function      lp_ts_create_pcs
|
| param. in     ts_id
|               p_ts_cfg
|        out    -
| 
| return value  result
|                                                                            |
 ------------   A B S T R A C T   -------------------------------------------


  This routine creates a zero-initialised pcs array

  !!! Attention: lpc_put_max must have stored the prt_indx_max beforehand!

  Convert prt_indx_max to size of pcs array.

  NULL pointer configured?
  YES:                                  NO:
  Allocate traffic store component      Use configured base pointer

  Clear traffic store component
  Store location in control block
  Store memory type in control block

*/

int             lp_ts_create_pcs        (void *                         p_cb,
                                         const struct STR_LP_TS_CFG *   p_cfg)
{
void *          p_pcs;

int             pcs_size;

int             result = LPS_OK;



    pcs_size   = lpl_gen_pcs_offset (p_cb,
                                     lpc_get_prt_indx_max (p_cb) + 1);

    if ((p_pcs = p_cfg->pb_pcs) == NULL)
    {
        if ((p_pcs = pi_alloc (pcs_size)) != NULL)
        {
                lpc_remember (p_cb, p_pcs);
        }
        else
        {
                result = LPS_MEMORY;
        }
    }

    lpc_put_pb_pcs (p_cb, p_pcs);

    if ((p_cfg->ownership == LP_CFG_TS_OWNED) &&
        (result           == LPS_OK))
    {
        lpi_set_ts_array (p_cb, p_pcs, 0, pcs_size);
    }

    return (result);
}

/*
 ----------------------------------------------------------------------------
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  1.0  BAEI-2  91-09-03  created  LPDBR4.TXT  4.2  Marsden      --
 ----------------------------------------------------------------------------
End of function lp_ts_create_pcs                                            */

/* new page
 ----------------------------------------------------------------------------
|
| function      lp_ts_create_prt
|
| param. in     ts_id
|               p_ts_cfg
|        out    -
|
| return value  result
|                                                                            |
 ------------   A B S T R A C T   -------------------------------------------


  This routine creates a zero-initialised prt data buffer array

  !!! Attention: lpc_put_max must have stored the prt_indx_max beforehand!

  Convert prt_indx_max to size of prt array.

  NULL pointer configured?
  YES:                                  NO:
  Allocate traffic store component      Use configured base pointer

  Clear traffic store component
  Store location in control block
  Store memory type in control block

*/

int             lp_ts_create_prt        (void *                         p_cb,
                                         const struct STR_LP_TS_CFG *   p_cfg)
{
void *          p_prt;

unsigned short  prt_size;

int    result = LPS_OK;


    prt_size    = lpl_gen_prt_offset (p_cb,
                                      lpc_get_prt_indx_max (p_cb) + 1);

    if ((p_prt = p_cfg->pb_prt) == NULL)
    {
        if ((p_prt = pi_alloc (prt_size)) != NULL)
        {
                lpc_remember (p_cb, p_prt);
        }
        else
        {
                result = LPS_MEMORY;
        }
    }

    lpc_put_pb_prt (p_cb, p_prt);

    if ((p_cfg->ownership == LP_CFG_TS_OWNED) &&
        (result           == LPS_OK))
    {
        if (p_cfg->pb_def != NULL)
        {
            lpl_put_def_values (p_cb, p_cfg->pb_def);
        }
        else
        {
            lpi_set_ts_array (p_cb, p_prt, 0, prt_size);
        }
    }

    return (result);
}

/*
 ----------------------------------------------------------------------------
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  1.0  BAEI-2  91-09-03  created  LPDBR4.TXT  4.2  Marsden      --
 ----------------------------------------------------------------------------
End of function lp_ts_create_prt                                            */

/* new page
 ----------------------------------------------------------------------------
|
| function      lp_ts_create_frc
|
| param. in     ts_id
|               p_ts_cfg
|        out    -
|
| return value  result
|                                                                             |
 ------------   A B S T R A C T   -------------------------------------------


  This routine creates a zero-initialised force table array

  !!! Attention: lpc_put_max must have stored the prt_indx_max beforehand!

  Convert prt_indx_max to size of force table array.

  NULL pointer configured?
  YES:                                  NO:
  Allocate traffic store component      Use configured base pointer

  Clear traffic store component
  Store location in control block
  Store memory type in control block

*/


int             lp_ts_create_frc        (void *                         p_cb,
                                         const struct STR_LP_TS_CFG *   p_cfg)
{
void *          p_frc;

unsigned short  frc_size;
int             result = LPS_OK;

    p_frc = p_cfg->pb_frc;

    if (p_frc != LP_MINUS_1_POINTER)
    {
        frc_size = lpl_gen_prt_offset (p_cb,
                                       lpc_get_prt_indx_max (p_cb) + 1);

        if (p_frc == NULL)
        {
                if ((p_frc = pi_alloc (frc_size)) != NULL)
                {
                        lpc_remember (p_cb, p_frc);
                }
                else
                {
                        result = LPS_MEMORY;
                }
        }

        lpc_put_pb_frc (p_cb, p_frc);

        if (result == LPS_OK)
        {
            lpi_set_ts_array (p_cb, p_frc, 0, frc_size);
		}
                                       #if defined (O_PCMCIA)
        {
         void * p_frc_shadow;
                if ((p_frc_shadow = pi_alloc (frc_size)) != NULL)
                {
                    lpc_put_pb_frc_shadow (p_cb, p_frc_shadow);
                    lpi_memset (p_frc_shadow, 0, frc_size);
                }
                else
                {
                    lpi_hamster_uncond (LPI_DU_ERR, LPI_DU_RESET, "pi_alloc failed");  /*$H. prt_indx_max too big */
                }
        }
                                       #endif
    }
    return (result);
}

/*
 ----------------------------------------------------------------------------
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  1.0  BAEI-2  91-09-03  created  LPDBR4.TXT  4.2  Marsden      --
 ----------------------------------------------------------------------------
End of function lp_ts_create_frc                                            */

/* new page
 ---------------------------------------------------------------------------- 
|
| function      lp_ts_open_port
|
| param. in     p_cb                            control block handle
|               p_cfg                           pointer port configuration
|
|        out    -
|
| return value  result                          LPS_OK
|                                               LPS_CONFIG
|                                                                            |
 ------------   A B S T R A C T   ------------------------------------------- 
  

  For each data set in list p_cfg:

    PIT:
    Checks whether port index is already in use (error)
    Writes port index into port index table under byte offset of port address

    PCS:
    Checks whether port is already in use (error)
    Checks data set type (source, sink, source and sink = sink)
    Writes data set type
    Checks data set size (error)
    Writes data set size

*/

int             lp_ts_open_port         (void *                         p_cb,
                                         const struct STR_LP_PRT_CFG *  p_cfg)
{

unsigned char * p_pcs;                  /*   temp pointer pcs           */
LC_TYPE_TS_PIT *pb_pit;
unsigned short  prt_addr;
unsigned short  prt_indx;
unsigned short  prt_type;
unsigned short  prt_size;
unsigned short  ix_tmp;
unsigned int    tmp;

int             result = LPS_OK;

		prt_addr = p_cfg->prt_addr;
		prt_indx = p_cfg->prt_indx;
		prt_type = p_cfg->type;
		prt_size = p_cfg->size;

		pb_pit   = lpc_get_pb_pit (p_cb);
		p_pcs    = (((unsigned char *)  lpc_get_pb_pcs (p_cb)) +
							   lpl_gen_pcs_offset (p_cb, prt_indx));

										#ifndef O_TCN
        lpi_hamster_cond (!(prt_addr <= lpc_get_prt_addr_max (p_cb)) &&
						 (prt_indx  <= lpc_get_prt_indx_max (p_cb)) &&
						 (prt_indx  != 0)                          &&
						 (prt_addr  != 0),
                          LPI_DU_ERR, LPI_DU_RESET, "prt addr or indx"); /*$H. prt addr or indx ??? */

        lpi_hamster_cond (lpl_chk_prt_size (p_cb, prt_indx, prt_size) != LPS_OK,
                          LPI_DU_ERR, LPI_DU_RESET, "prt size"); /*$H. prt size ??? */

                                        #else
        lpi_hamster_cond (!(prt_addr <= lpc_get_prt_addr_max (p_cb)) &&
                         (prt_indx <= lpc_get_prt_indx_max (p_cb)),
                          LPI_DU_ERR, LPI_DU_RESET, "prt addr or indx"); /*$H. prt addr or indx ??? */
                                        #endif

        lpi_get_prt_indx (p_cb, &ix_tmp, pb_pit, prt_addr);

        lpi_hamster_cond (ix_tmp != 0,
                          LPI_DU_ERR, LPI_DU_RESET, "double prt addr"); /*$H. double prt addr ??? */

        lpl_get_prt_type (p_cb, &tmp, p_pcs);

        lpi_hamster_cond (tmp != LPL_PCS_PASSIVE,
                          LPI_DU_ERR, LPI_DU_RESET, "double prt indx"); /*$H. double prt indx ??? */

        lpi_hamster_cond (((prt_type != LPL_PCS_PASSIVE) &&
						  (prt_type != LPL_PCS_SINK   ) &&
						  (prt_type != LPL_PCS_SRCE   ) &&
						  (prt_type != LPL_PCS_BDIR   )),
                          LPI_DU_ERR, LPI_DU_RESET, "prt type"); /*$H. port type ??? */

												#if (LPI_DB == 2)
		if (result == LPS_OK)
												#endif

		{
			lpi_put_prt_indx (p_cb, pb_pit, prt_addr, prt_indx);
			lpl_put_prt_size (p_cb, p_pcs, prt_size);
			lpl_put_prt_type (p_cb, p_pcs, prt_type);
			lpl_num_enable   (p_cb, p_pcs, LC_PCS_VAL_NUM_SET);
		}

		return (result);
}

/* new page
 ----------------------------------------------------------------------------
|
| function      lp_rd_prt_cfg
|
 ------------   A B S T R A C T   -------------------------------------------

  LME (Layer Management Entity)
  Searches the traffic store identified by ts_id for configured ports
  Returns the configuration of the configured ports
  Works also for Train Bus Traffic Stores
  p_rd_prt_cfg: Memory for array of c_prts_max elements of rd_prt_cfg
  p_c_prts    : Count of found configured ports, not more than c_prts_max
  ts_id       : traffic store identifier (o....15)
  c_prts_max  : Caller can not store more than this amount of port configs

*/

TYPE_PD_RESULT _lp_rd_prt_cfg       (unsigned int               ts_id,
                                     struct STR_LP_RD_PRT_CFG * p_rd_prt_cfg,
                                     unsigned int *             p_c_prts,
                                     unsigned int               c_prts_max)
{
void       *    p_cb;
LC_TYPE_TS_PIT *pb_pit;
unsigned char * p_pcs;                  /*   temp pointer pcs           */
unsigned int    c_prts;

unsigned int    type;
unsigned int    fe;
unsigned int    fcode;
unsigned int    twcs;

         short  result;
unsigned short  prt_addr;
unsigned short  prt_addr_max;
unsigned short  prt_ix;
unsigned short  size;

unsigned char   hw_type;

  *p_c_prts = 0;

  if (((result = lpc_open (ts_id, &p_cb)) == LPS_OK) &&
      (p_rd_prt_cfg != NULL))
  {
    pb_pit = lpc_get_pb_pit (p_cb);

    prt_addr_max = lpc_get_prt_addr_max(p_cb);

    hw_type = lpc_get_hw_type (p_cb);

    for (prt_addr  = 0, c_prts = 0;
        (prt_addr <= prt_addr_max) & (c_prts < c_prts_max);
         prt_addr++)
    {
        lpi_get_prt_indx (p_cb, &prt_ix, pb_pit, prt_addr);
        if ((prt_ix != 0) | (hw_type == LP_HW_TYPE_HDLC))
        {
            p_pcs = (((unsigned char *) lpc_get_pb_pcs (p_cb)) +
                                        lpl_gen_pcs_offset (p_cb, prt_ix));

            lpi_get_pcs (p_cb, &fcode, p_pcs, LC_PCS_MSK_FCODE, LC_PCS_OFF_FCODE);
            lpi_get_pcs (p_cb, &type , p_pcs, LC_PCS_MSK_TYPE , LC_PCS_OFF_TYPE );
                                        #if !defined (O_BAP)
            lpi_get_pcs (p_cb, &fe   , p_pcs, LC_PCS_MSK_FE   , LC_PCS_OFF_FE   );
            lpi_get_pcs (p_cb, &twcs , p_pcs, LC_PCS_MSK_TWCS , LC_PCS_OFF_TWCS );
                                        #else
            fe   = 0;
            twcs = 0;
                                        #endif
            if  (hw_type == LP_HW_TYPE_HDLC)
            {
                lpl_tb_get_pcs_size(p_cb, &size, p_pcs);
            }
            else
            {
                size = 0;
            }
            if (type != LPL_PCS_PASSIVE)
            {
                p_rd_prt_cfg->port_address = prt_addr;
                p_rd_prt_cfg->port_config  = 16 * fcode +
                                              4 * type  +
                                              2 * twcs  +
                                              1 * fe    ;
                p_rd_prt_cfg->port_size    =  size;

                c_prts++;
                p_rd_prt_cfg++;
            }
        }
    }
    *p_c_prts = c_prts;
  }
  return (result);
}


/* new page
 ----------------------------------------------------------------------------
|
| function      lp_redundancy
|
 ------------   A B S T R A C T   -------------------------------------------

   The ports which were passive during their configuration are

   param == ON    switched active source
   param == OFF   switched passive

*/

TYPE_PD_RESULT _lp_redundancy      (unsigned int  ts_id,
                                    unsigned char action)
{
void *                        p_cb;
const struct STR_LP_PRT_CFG * p_prt_cfg;
const struct STR_LP_TS_CFG *  p_ts_cfg;
unsigned char *               p_pcs;        /*   temp pointer pcs           */
unsigned int    prt_count;
unsigned int    i;
unsigned short  type;
         short  result = LPS_OK;

    if ((result = lpc_open (ts_id, &p_cb)) == LPS_OK)
    {
        lpc_get_p_ts_cfg (p_cb, &p_ts_cfg);

	    prt_count = p_ts_cfg->prt_count;
        p_prt_cfg = p_ts_cfg->p_prt_cfg;

        type = action == LP_RED_PASSIVE ? LPL_PCS_PASSIVE : LPL_PCS_SRCE;

        for (i = 0; i < prt_count; i++, p_prt_cfg++)
        {
            if (p_prt_cfg->type == LP_CFG_PASSIVE)
            {
		        p_pcs = (((unsigned char *) lpc_get_pb_pcs (p_cb)) +
			                  lpl_gen_pcs_offset (p_cb, p_prt_cfg->prt_indx));

			    lpl_put_prt_type (p_cb, p_pcs, type);
            }
        }
    }
    return (result);
}

/*
 ----------------------------------------------------------------------------
|
|   M O D I F I C A T I O N S
|   - - - - - - - - - - - - -
|
 ----------------------------------------------------------------------------

    file name: LP_INIT.C                Nr:     1       dated:  91-09-03

    name of object modified

    description of what was done to object

 ----------------------------------------------------------------------------

    file name: LP_INIT.C                Nr:     2       dated:  93-01-27

    hamster texts added to hamster call

 ----------------------------------------------------------------------------

    file name: LP_INIT.C                Nr:     3       dated:  93-05-26

    lp_ts_create_pit                    

    fault in pit_size initialisation

 ----------------------------------------------------------------------------

    file name: LP_INIT.C                Nr:     4       dated:  93-05-27

    lp_ts_open_port                     

	extended to accomodate the wire train bus (switch O_TCN)

 ----------------------------------------------------------------------------

	file name: LP_INIT.C                Nr:     5       dated:  93-12-02

	lp_ts_open_port

	prt_size written to "tack" bit in pcs + 1

 ----------------------------------------------------------------------------

	file name: LP_INIT.C                Nr:     6       dated:  94-02-09

	lp_create

	lpc_put_hw_type() before lp_ts_create() as hw_type newly evalued by
	lpc_put_ts_type() which is called by lp_ts_create()

 ----------------------------------------------------------------------------

	file name: LP_INIT.C                Nr:     7       dated:  95-12-06

	lp_create_ts_frce

    new shadow force table created for pcmcia
*/
