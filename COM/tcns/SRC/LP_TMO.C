/*   LP_TMO.C
 ---------------------------------------------------------------------------- 
                                                                              
     Property of  :   ABB Verkehrssyteme AG, CH-8050 Zrich / Switzerland     
     COPYRIGHT    :   (c) 1991 ABB Verkehrssysteme AG                         
                                                                              
 ---------------------------------------------------------------------------- 
                                                                              
     Project      :   MicOS                                                   
     Subproject   :   Process Data                                            
                                                                              
 ---------------------------------------------------------------------------- 
                                                                              
     File         :   LP_TMO_.C   	(Link Process Timeout)                
                                                                              
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

#define  LP_TMO_C
#define  LPI_MODULE_NAME "lp_tmo"

#if defined (O_PCMCIA)
#define O_LE
#endif
#include "lp_sys.h"		/*   LP user interface                      */
#include "pi_sys.h"		/*   PIL                                    */

#include "lpl_4tcn.h"		/*   TS access routines                     */
#include "lpc_blck.h"		/*   control block functionality            */
#include "lpi_ifce.h"		/*   LP import interface                    */

#include "lp_tmo.h"		/*   timeout surveillance                   */



/*
 ---------------------------------------------------------------------------- 
|
| static data	lp_tmo_state
|
 ------------   A B S T R A C T   ------------------------------------------- 
 
 
  Static Memory is defined to save information which is required only once
  for the whole link process subproject.

	lp_tmo_state		A port was not updated within the specified
				time band.

				This variable is accessed by
					- lp_tmo_clr_state
					- lp_tmo_get_state
					- lp_tmo_set_state

*/
  
static  int	lp_tmo_state;



/* new page
 ---------------------------------------------------------------------------- 
|
| function      lp_tmo_init
|
| param. in     -
|        out    -
|               
| return value  -
| 
 ------------   A B S T R A C T   ------------------------------------------- 


  The static data

	- lpc_tmo_state

  is cleared.

*/

void            lp_tmo_init          (void)

{

	lp_tmo_state = 0;

}

/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-08-20  created  LPDBR5.TXT  5.2  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lp_tmo_init                                                 */

/*
 ---------------------------------------------------------------------------- 
|
| name          lp_tmo_counter
|
| param. in     p_clus_tmo
|               
|        out    -
|               
| return value  -
| 
 ------------   A B S T R A C T   ------------------------------------------- 


  Please refer to the structure STR_LP_TMO_CLUS definition for a description 
  of the input variable p_clus_tmo.

  The function increments an 8-bit counter which is reset to 255 by an update. 
  The actual time since the last update is

	Cycle Time of function call * (counter + 1)

  The counter is limited to LP_TMO_MAX_COUNT

  When the limit is reached, the timeout is reported to the lp control block.

		 No updates: count 0.....LP_TMO_MAX_COUNT
		 Updates   : count -1....LP_TMO_MAX_COUNT

  !!! Attention: The read modify write access is not exclusive. Thus, in
		 a very seldom cases the update reset may be lost. This is
		 not a problem, as the probability of a second loss is
		 extremely small.

*/

TYPE_PD_RESULT  _lp_tmo_counter		(const struct STR_LP_TMO_CLUS *	p_tmo)

{

struct STR_LP_TMO_CLUS_TS **	p_clus_ts;

struct STR_LP_TMO_CLUS_TS *	p_clts;
struct STR_DS_NAME *	    p_clp;

char *			pb_pcs;
char *			p_pcs;
void *			p_cb;
LC_TYPE_TS_PIT *pb_pit;

int		        ts_count;
int		        prt_count;
unsigned short	prt_indx;

short   result = LPS_OK;


  ts_count  	= p_tmo->ts_count;

  p_clus_ts 	= (struct STR_LP_TMO_CLUS_TS **) &(p_tmo->p_clus_ts[0]);

  while (ts_count-- > 0)
  {
	p_clts 		= *p_clus_ts++;

	result 		= lpc_open ((unsigned int) p_clts->ts_id, &p_cb);

    lpi_hamster_cond (result, LPI_DU_ERR, LPI_DU_RESET, "traffic store id"); /*$H. traffic store id ??? */

	pb_pcs		= lpc_get_pb_pcs (p_cb);

	prt_count 	= p_clts->prt_count;

	p_clp      	= p_clts->p_ds_name;

	pb_pit	        = lpc_get_pb_pit (p_cb);

	while (prt_count-- > 0)
	{
    unsigned int tack;
	    lpi_get_prt_indx (p_cb, &prt_indx, pb_pit, p_clp->prt_addr);

	    p_pcs	= pb_pcs + lpl_gen_pcs_offset (p_cb, prt_indx);

        lpl_get_prt_tack (p_cb, &tack, p_pcs);

	    if (tack == LP_TMO_MAX_COUNT)
	    {
	        _lp_tmo_set_state ();
	    }
	    else
	    {
		    pi_disable ();                  /*   !!! does not protect  */

		    lpl_inc_prt_tack (p_cb, p_pcs);

		    pi_enable ();                   /*   ... against BAP       */
	    }

		p_clp += 1;
	}
  }
  return (result);
}
/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-10-22  created  LPDBR5.TXT  5.2  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lp_tmo_counter                                              */

/* new page
 ---------------------------------------------------------------------------- 
| 
| function      lp_tmo_..._state
|
| param. in     -
|
|        out    -
|
| return value  -
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The tmo_state provides the information, whether all ports in all traffic
  stores were updated within their time band.

  clr: This function will be called after reading the value to support a
       dynamic tracing of this value.

  get: Read the tmo_state variable.

  set: At least one port was found not updated within the allowed time band.

*/

void  	       _lp_tmo_clr_state	(void)

{
	lp_tmo_state = LP_TMO_PASSIVE;
}


TYPE_PD_RESULT   _lp_tmo_get_state      (void)

{
	return (lp_tmo_state);
}


void  	       _lp_tmo_set_state	(void)

{
	lp_tmo_state = LP_TMO_ACTIVE;
}


/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-10-18  created  LPDBR5.TXT  5.2  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lp_tmo_..._state                                            */





/*
 ----------------------------------------------------------------------------
|
|   M O D I F I C A T I O N S
|   - - - - - - - - - - - - -
|
 ----------------------------------------------------------------------------

    file name: LP_TMO.C  	 	Nr:	1	dated:	91-10-21

    name of object modified		Nr:	?  	dated:	9?-??-??

    description of what was done to object

 ----------------------------------------------------------------------------

    file name: LP_TMO.C  	 	Nr:	2	dated:	93-01-27

    hamster texts added to hamster call

 ----------------------------------------------------------------------------

    file name: LP_TMO.C  	 	Nr:	3	dated:	94-09-24

    LP_TMO_MAX_COUNT modified to accomodate new lpl_refresh()

*/
