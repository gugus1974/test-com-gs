/*   LP_IRPT.C
 ----------------------------------------------------------------------------- 
                                                                               
     Property of  :   ABB Verkehrssyteme AG, CH-8050 ZÅrich / Switzerland      
     COPYRIGHT    :   (c) 1991 ABB Verkehrssysteme AG                          
                                                                               
 ----------------------------------------------------------------------------- 
                                                                               
     Project      :   MicOS                                                    
     Subproject   :   Process Data                                             
                                                                               
 ----------------------------------------------------------------------------- 
                                                                               
     File         :   LP_IRPT.C         (Link Process Interrupt Handling)      
                                                                               
     Document     :   [1]: IEC WG 22-5, Upper Layers, Version 1.3              
                      [2]: Link Layer Process Specification                    
                      [3]: Link Layer Process Design                           
                                                                               
     Abstract     :   Interrupts may be subscribed and desubscribed to         
                      an individual port for each traffic store.               
                                                                               
     Remarks      :                                                            
                                                                               
     Dependencies :                                                            
                                                                               
     Accepted     :                                                            
                                                                               
     HISTORY      :   See end of file                                          
                                                                               
 ----------------------------------------------------------------------------- 
*/

#define  LP_IRPT_C

#if defined (O_PCMCIA)
#define O_LE
#endif

#if !defined (WTB_ONLY)
#if  defined (O_BAP)

#define  LPI_MODULE_NAME "lp_irpt"

#include "lp_sys.h"             /*   LP user interface                      */
#include "lc_sys.h"             /*   TCN common layer                       */
#include "pi_sys.h"             /*   PIL                                    */

#include "lpi_ifce.h"           /*   LP import interface                    */
#include "lpl_4tcn.h"           /*   TS access routines                     */
#include "lpc_blck.h"           /*   control block functionality            */

#include "lp_irpt.h"            /*   indication functionality               */


/* new page
 ---------------------------------------------------------------------------- 
| 
| name          lp_subscribe
|
| param. in     ds_name
|               p_procedure
|               ds_type
|               
|        out    -
|               
| return value  result                  LPS_OK
|                                       LPS_UNKNOWN_TS_ID
|                                       LPS_CONFIG
| 
 ------------   A B S T R A C T   ------------------------------------------- 
  

  The address of an indication procedure to be called upon a send or receive
  (ds_type) update of the data set (port) defined by ds_name is connected
  to the appropriate interrupt handler.

*/


TYPE_PD_RESULT  _lp_subscribe   (unsigned short                 ds_type,
                                       struct STR_DS_NAME       ds_name,
                                 void   (*p_proc) (struct STR_DS_NAME))
{
short           result = LPS_OK;
unsigned char   lc_int;
unsigned short  prt_indx;
int             ie_x;
char *          p_pcs;
void *          p_cb;
LC_TYPE_TS_PIT* pb_pit;


        result  = lpc_open (ds_name.ts_id, &p_cb);

        lpi_hamster_cond (result, LPI_DU_ERR, LPI_DU_RESET, "traffic store id"); /*$H. traffic store id ??? */

        if (ds_type == LP_CFG_SRCE)
        {
                if (lpc_irpt_srce_query (p_cb) != LPC_IRPT_FREE)
                {
                        lpi_hamster_uncond (LPI_DU_ERR, LPI_DU_RESET, "int error"); /*$H. int error ??? */
												/*$H. interrupt double use */
				}
				else
				{
						lpc_irpt_srce_in_use (p_cb);
						ie_x   = LPL_PCS_IE0;
						lc_int = LC_INT_LP_SEND;
				}
        }
        else
		{
                if (lpc_irpt_sink_query (p_cb) != LPC_IRPT_FREE)
                {
                        result     = LPS_CONFIG;
                }
                else
                {
                        lpc_irpt_sink_in_use (p_cb);
						ie_x   = LPL_PCS_IE2;
						lc_int = LC_INT_LP_RECV;
                }

        }
        if (result == LPS_OK)
        {
			pb_pit   = lpc_get_pb_pit (p_cb);
			lpi_get_prt_indx_via_pvn (p_cb, &prt_indx, pb_pit, ds_name.prt_addr);

			p_pcs    = ((char *) lpc_get_pb_pcs (p_cb)) +
					   lpl_gen_pcs_offset (p_cb, prt_indx);

			lpl_put_prt_ie_x (p_cb, p_pcs, ie_x);

			result   = lpi_lc_int_connect ((void (*) (unsigned short)) p_proc,
						     			   lc_int,
									       LC_KERNEL,
									       * ((unsigned short *) &ds_name))
					 == LC_OK ? LPS_OK : LPS_ERROR;
		}

		return (result);

}
/*
 ----------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-10-22  created                   Marsden      --   
 ----------------------------------------------------------------------------- 
End of function lp_subscribe                                                 */


/*  new page
 ---------------------------------------------------------------------------- 
| 
|   name        lp_desubscribe
|
|   par. in     ds_type
|               ds_name
|        out    -
|               
|   ret val     -
| 
 ------------   A B S T R A C T   ------------------------------------------- 
  

	 Undo what was done by lp_subscribe

*/

TYPE_PD_RESULT  _lp_desubscribe    (unsigned short      ds_type,
									struct STR_DS_NAME  ds_name)
{
char *          p_pcs;
void *          p_cb;
LC_TYPE_TS_PIT* pb_pit;
short           result = LPS_OK;
int             prt_indx;
unsigned char   lc_int;


		result  = lpc_open (ds_name.ts_id, &p_cb);

        lpi_hamster_cond (result, LPI_DU_ERR, LPI_DU_RESET, "traffic store id"); /*$H. traffic store id ??? */

		if (result == LPS_OK)
		{
			pb_pit   = lpc_get_pb_pit (p_cb);
			lpi_get_prt_indx_via_pvn (p_cb, &prt_indx, pb_pit, ds_name.prt_addr);

			p_pcs    = ((char *) lpc_get_pb_pcs (p_cb)) +
					   lpl_gen_pcs_offset (p_cb, prt_indx);

			if (ds_type == LP_CFG_SRCE)
			{
				if (lpc_irpt_srce_query (p_cb) == LPC_IRPT_IN_USE)
				{
					lpl_put_prt_ie_x (p_cb, p_pcs, LPL_PCS_IE_OFF);
					lc_int = LC_INT_LP_SEND;
					lpc_irpt_srce_free (p_cb);
				}
			}
			else
			{
				if (lpc_irpt_sink_query (p_cb) == LPC_IRPT_IN_USE)
				{
					lpl_put_prt_ie_x (p_cb, p_pcs, LPL_PCS_IE_OFF);
					lc_int = LC_INT_LP_RECV;
					lpc_irpt_sink_free (p_cb);
				}

			}

			result = lpi_lc_int_disconnect (lc_int) == LC_OK ?
                                                       LPS_OK : LPS_ERROR;
		}

		return (result);

}
/*
 ----------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|   1   BATC    93-06-17  created                   Marsden      --   
 ----------------------------------------------------------------------------- 
End of function lp_desubscribe                                               */


/*
 ----------------------------------------------------------------------------
|
|   M O D I F I C A T I O N S
|   - - - - - - - - - - - - -
|
 ----------------------------------------------------------------------------

	file name: LP_IRPT.C                Nr:     1       dated:  91-10-25

	name of object modified             Nr:     ?       dated:  9?-??-??

	description of what was done to object

 ----------------------------------------------------------------------------

	file name: LP_IRPT.C                Nr:     2       dated:  93-01-27

	hamster texts added to hamster call

 ----------------------------------------------------------------------------

	func name: lp_desubscribe           Nr:     3       dated:  93-06-16

	new function added to module

 ----------------------------------------------------------------------------

	func name: all                      Nr:     4       dated:  94-03-15

	new switch WTB_ONLY added to mask module

*/

#endif
#endif
