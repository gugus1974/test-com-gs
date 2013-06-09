/*   LP_CLUS.C
 ---------------------------------------------------------------------------- 
                                                                              
     Property of  :   ABB Verkehrssyteme AG, CH-8050 Zrich / Switzerland     
     COPYRIGHT    :   (c) 1991 ABB Verkehrssysteme AG                         
                                                                              
 ---------------------------------------------------------------------------- 
                                                                              
     Project      :   MicOS                                                   
     Subproject   :   Process Data                                            
                                                                              
 ---------------------------------------------------------------------------- 
                                                                              
     File         :   LP_CLUS.C         (Link Process Get and Put)            
                                                                              
     Document     :   [1]: IEC WG 22-5, Upper Layers, Version 1.3             
                      [2]: Link Layer Process Specification                   
                      [3]: Link Layer Process Design                          
                                                                              
     Abstract     :   Application Interface Link Process                      
                                                                              
     Remarks      :   Order of Include: lptype.h                              
                                        lpapex.h                              
                                                                              
     Dependencies :                                                           
                                                                              
     Accepted     :                                                           
                                                                              
     HISTORY      :   See end of file                                         
                                                                              
 ---------------------------------------------------------------------------- 
*/

#define  LP_CLUS_C
#define  LPI_MODULE_NAME "lp_clus"

#if defined (O_PCMCIA)
#define O_LE
#endif
#include "lp_sys.h"                     /*   LP interface to user           */
#include "pi_sys.h"                     /*   PIL                            */

#include "lpi_ifce.h"                   /*   LP interface to others         */
#include "lpl_4tcn.h"                   /*   TS access routines             */
#include "lpc_blck.h"                   /*   control block functionality    */
#include "lpa_as2c.h"                   /*   possible ASM implementation    */


/*
 ---------------------------------------------------------------------------- 
|
| name          lp_cluster
|
| param. in     p                       Pointer to cluster copy list
|               data_flow               LP_PUT
|                                       LP_GET
|        out    -
|               
| return value  result                  LPS_OK
|                                       LPS_UNKNOWN_TS_ID
|                                       LPS_CONFIG
|
 ------------   A B S T R A C T   ------------------------------------------- 


  Please refer to the structure STR_PV_CLUS definition for a description of 
  the input variable p_pv_clus.

  The function updates process variables as described by the input list. The
  input list is sorted into separate traffic store lists which are sorted into
  separate port lists. The port lists are organised in bus variables to
  update.

  Each separate traffic store id is first tested whether it is known to the
  control block and whether the configuration was successfully completed.
  Failure here leads to an immediate routine termination with the function
  value LPS_UNKNOWN_TS_ID or LPS_CONFIG.

  For each traffic store id, the traffic store base pointers for the
  components port index table (pb_pit), port communication and status register
  (pb_pcs) and force table (pb_frc) is read from the control block.
  The port count is read from the input list.

  The routine now discriminates between LP_PUT and LP_GET:

  LP_PUT:
  Get page offset (0 / 20H) write, add to port pointer
  For all variables in cluster list: write from appl. to port
  For size of port: Merge force Table with process variables to consider the
                    force state of individual bus variables
  Toggle the page pointer in the pcs
  Set the update field to "updated" in the pcs

  LP_GET:
  Disable the task switch mechanism (lock or interrupt disable, see lpi_lock)
  Get page offset (0 / 20H) read, add to port pointer
  For size of port: Copy port into stack, at the same time merge with the
                    force table to consider the force state of the individual
                    bus variables
  Enable the interrupts
  For all variables in cluster list:
  Get variable from traffic store
  write from stack to application
  If refresh variable provided: write time since last port update (refresh
  time) into refresh variable

*/

TYPE_PD_RESULT _lp_cluster      (const  struct STR_PV_CLUS *    p_pv_clus,
                                 short                          data_flow)

{

short                   result = LPS_OK;

int                     ts_count;

char *                  p_pcs;
char *                  p_frc;

void *                  p_cb;

struct LPI_LC_STR_TS_PGE * p_prt;
struct LPI_LC_STR_TS_PGE * p_tmp;
struct LPI_LC_STR_TS_PGE   prt_image;

struct STR_LP_CLUS_TS **p_clus_ts;
struct STR_LP_CLUS_TS * p_clts;

unsigned short          prt_sze;

struct STR_LP_CLUS_DS * p_clp;

	p_tmp  = &prt_image;

    ts_count  = p_pv_clus->ts_count;

    p_clus_ts = (struct STR_LP_CLUS_TS **) &(p_pv_clus->p_clus_ts[0]);

    while (ts_count-- > 0)
    {
        p_clts          = *p_clus_ts++;

		result          = lpc_open ((unsigned int) p_clts->ts_id, &p_cb);

        lpi_hamster_cond (result, LPI_DU_ERR, LPI_DU_RESET, "traffic store id"); /*$H. traffic store id ??? */

		if ((p_clp = (struct STR_LP_CLUS_DS *) p_clts->p_clus_ds) != NULL)
		{
		  if (lpi_mbox_slave_lock (p_cb) == LPS_OK)
		  {
			lpa_get_ts_seg (p_pcs, p_prt, p_frc, p_clp);

			if (data_flow == LP_GET)
			{
				do
				{
					lpa_copy_ds_x (p_pcs, p_prt, p_frc, prt_sze, p_clp);

					lpi_lock ();

					lpa_add_pge_offset_rd (p_cb, p_pcs, p_prt);
					                    #if defined (O_BAP)
					lpa_get_frcd_prt  (p_tmp, p_prt, p_frc, prt_sze);
					                    #else
                    {
                    unsigned int my_type;
                      lpl_get_prt_type (p_cb, &my_type, p_pcs)
					  if (my_type == LP_CFG_SRCE)
					  {
					   	lpa_get_frcd_prt  (p_tmp, p_prt, p_frc, prt_sze);
					  }
					  else
					  {
						*p_tmp = *p_prt;
					  }
                    }
					                    #endif
					lpi_unlock ();

					_lpl_get_pv_x  (p_tmp, p_clp->p_dta_lst,
										   p_clp->ds_x.pv_x_cnt);

					if (p_clp->ds_x.p_refresh != NULL)
					{
						lpl_refresh (p_cb, p_pcs, p_clp->ds_x.p_refresh);
					}
				}
				while ((p_clp = p_clp->p_nxt_lst) != NULL);
			}

			else                                /*  a PUT cluster       */
			{
			  do
			  {
												#ifndef LPD_DBBM2
              unsigned int my_type;
												#endif
				lpa_copy_ds_x (p_pcs, p_prt, p_frc, prt_sze, p_clp);
												#ifndef LPD_DBBM2
                lpl_get_prt_type (p_cb, &my_type, p_pcs);
				if (my_type != LP_CFG_SINK)
												#endif
				{
					lpa_add_pge_offset_wt (p_cb, p_pcs, p_prt);

					_lpl_put_pv_x  (p_prt, p_clp->p_dta_lst,
										   p_clp->ds_x.pv_x_cnt);

					#if defined (O_BAP)
					lpa_put_frcd_prt (p_prt, p_frc, prt_sze);
					#endif
					lpa_tgl_prt_page (p_cb, p_pcs);

					lpl_put_prt_tack (p_cb, p_pcs, LPL_PCS_UPDATED);
				}
			  }
			  while ((p_clp = p_clp->p_nxt_lst) != NULL);
			}
			lpi_mbox_slave_unlock (p_cb);
          }
        }
    }

    return (result);

}

/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Name     Reference
| ----  ------  --------  -------  -------  ---------
|  1    BAEI-2  91-09-03  created  Marsden      --
|  2    BAEI-2  92-11-10  modif'd  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lp_cluster                                                  */


/*
 ----------------------------------------------------------------------------
|
|   M O D I F I C A T I O N S
|   - - - - - - - - - - - - -
|
 ----------------------------------------------------------------------------

    file name: LP_CLUS.C                Nr:     1       dated:  91-08-19

    name of object modified             Nr:     ?       dated:  9?-??-??

    description of what was done to object

 ----------------------------------------------------------------------------

    file name: LP_CLUS.C                Nr:     2       dated:  93-01-27

    lp_cluster                          Nr:     2       dated:  93-01-27

    hamster texts added to hamster call

*/
