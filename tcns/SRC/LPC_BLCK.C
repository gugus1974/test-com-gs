/*   LPC_BLCK.C
 ----------------------------------------------------------------------------- 
                                                                               
     Property of  :   ABB Verkehrssyteme AG, CH-8050 ZÅrich / Switzerland      
     COPYRIGHT    :   (c) 1991 ABB Verkehrssysteme AG                          
                                                                               
 ----------------------------------------------------------------------------- 
                                                                               
     Project      :   MicOS                                                    
     Subproject   :   Link Layer Process                                       
                                                                               
 ----------------------------------------------------------------------------- 
                                                                               
     File Name    :   LPC_BLCK.C                                                 
                                                                               
     Document     :   [1]. IEC WG 22-5 "Upper Layers", Version 1.3             
                      [2]. Link Layer Process Specification                    
                      [3]. Link Layer Process Design                           
                                                                               
     Abstract     :   Link Layer Process                                       
                      Control Block data definition and access                 
                                                                               
     Remarks      :   This module is INCLUDED in LPAPEX.C to allow fast        
                      macro function access to the control block               
                                                                               
     Dependencies :                                                            
                                                                               
     Accepted     :                                                            
                                                                               
 ----------------------------------------------------------------------------- 
                                                                               
     HISTORY:     :   See end of file                                          
                                                                               
 ----------------------------------------------------------------------------- 
*/

#define  LPC_BLCK_C

#if defined (O_PCMCIA)
#define O_LE
#endif
#include "lp_sys.h"		/*   LP user interface                      */
#include "pi_sys.h"		/*   PIL                                    */

#include "lpa_as2c.h"		/*   possibly tuned by ASM implementation   */
#include "lpi_ifce.h"		/*   LP import interface                    */
#include "lpl_4tcn.h"		/*   TS access routines                     */

#include "lpc_blck.h"		/*   control block functionality            */


/*
 ---------------------------------------------------------------------------- 
|
| macro         P_CB
|
 ------------   A B S T R A C T   ------------------------------------------- 
  
  
  Macro to abbreviate the structure access.

*/

#define		P_CB			((TYPE_LP_CB *) p_cb)


/* new page
 ---------------------------------------------------------------------------- 
|
| static data   lp_cb
| 
 ------------   A B S T R A C T   ------------------------------------------- 
  
  
  Static Memory is defined for the link process control block. The control
  block will store the traffic store control information for up to 16
  independant traffic stores.
  The control block size may be reduced by limiting the total of traffic
  stores (LPC_SIZE).
  Each traffic store is identified by the index "ts_id" (0....LPC_SIZE-1).

*/

static      	TYPE_LP_CB      lp_cb[LPC_SIZE];

/* new page
 ---------------------------------------------------------------------------- 
|
| function      lpc_open
|
| (in )         ts_id			traffic store identifier
| (out)         p_cb                    handle for cb macro functions
|
| return value  result			LPS_OK
|					LPS_UNKNOWN_TS_ID
|
 ------------   A B S T R A C T   ------------------------------------------- 

  This function must be called prior to any control block function call. It
  checks whether the traffic store identifier is defined and within range.
  The return value is the pointer to the control block of the target traffic
  store.
  
  This function was defined for the following reasons:

  One central checking mechanism defined/undefined ts_id
  Fast access to control block
  Access to control block from outside of this module

*/

short	lpc_open		(unsigned int  ts_id,
    					 void * *	   p_cb)
{
short result ;

	*p_cb  		= (void *) &lp_cb[ts_id];

	if (ts_id < LPC_SIZE)
	{
		if (lpc_get_state (*p_cb) != LPC_TS_IN_USE)
		{
			result     = LPS_CONFIG ;
		}
		else
		{
			result     = LPS_OK ;
		}
	}
	else 
	{
		result = LPS_UNKNOWN_TS_ID ;
	}
	return (result);
}
/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-09-24  created  LPDBR1.TXT  1.1  Marsden      --
|  0.1  BAEI-2  92-08-19  modi'fd                   Nantier      --
 ---------------------------------------------------------------------------- 
End of function lpc_open                                                  */

/* new page
 ---------------------------------------------------------------------------- 
|
| function      lpc_turbo_open
|
| (in )         ts_id			traffic store identifier
| (out)         p_cb                    handle for cb macro functions
|
| return value  result			LPS_OK
|					LPS_UNKNOWN_TS_ID
|
 ------------   A B S T R A C T   ------------------------------------------- 

  This function must be called prior to any control block function call. It
  is a fast conversion of the traffic store identifier ts_id to the pointer
  handle required by cb functions.

  Use lpc_open for a more leisurly and integer control block approach.

*/

void *		lpc_turbo_open		(unsigned short	ts_id)

{

	return (&lp_cb[ts_id]);

}

/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|   1   BAEI-2  92-09-24  created  LPDBR1.TXT  1.1  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_open                                                  */

/* new page
 ---------------------------------------------------------------------------- 
|
| function      lpc_enquiry
|
| (in )         ts_id			traffic store identifier
| (out)         p_cb_struct             write control block here
|
| return value  result			LPS_OK
|					LPS_UNKNOWN_TS_ID
|
 ------------   A B S T R A C T   ------------------------------------------- 

  This function copies the data of one control block to the memory provided 
  by the caller.

*/

TYPE_PD_RESULT		_lpc_enquiry		(unsigned short	ts_id,
					 void *		p_cb_struct)

{
void *	p_cb;
short	result;

	if ((result = lpc_open ((unsigned int) ts_id, &p_cb)) == LPS_OK)
	{
		pi_copy8 (p_cb_struct, p_cb, sizeof (struct STR_LP_CB));
	}		

	return (result);

}

/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|   1   BAEI-2  92-12-16  created                   Marsden  see end of file
 ---------------------------------------------------------------------------- 
End of function lpc_open                                                  */

/* new page
 ---------------------------------------------------------------------------- 
|
| function      lpc_init
|                                                                             
| return value  -
| 
 ------------   A B S T R A C T   ------------------------------------------- 


  The control block is cleared. All information is lost.

*/

void            lpc_init          (void)

{
int i;

	lpi_memset (lp_cb, 0, sizeof(lp_cb));

    for (i = 0; i < LPC_SIZE; i++)
    {
        lp_cb[i].ts_id = i;
    }

}

/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-08-20  created  LPDBR1.TXT  1.1  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_init                                                  */

/* new page
 ---------------------------------------------------------------------------- 
|
| function      lpc_kill_ts
|
| (in )         p_cb
|               
| return value  -
| 
 ------------   A B S T R A C T   ------------------------------------------- 


  The control block for the opened ts_id is cleared. Any dynamic memory
  parts are recycled to the pool.

*/

void            lpc_kill_ts		(void * 	p_cb)

{
	lpc_recycle (p_cb);
	lpi_memset  (p_cb, 0, sizeof(struct STR_LP_CB));
}

/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|   1   BAEI-2  92-09-28  created                   Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_init                                                  */

/* new page
 ---------------------------------------------------------------------------- 
|
| function      lpc_frc_flag_clr
|
| (in )         p_cb
|               
| return value  -
|		
 ------------   A B S T R A C T   ------------------------------------------- 

  The force flag state shows whether any process data in the target traffic
  store are forced. This function resets this state. It will be called by the
  "unforce_all" function after all data in the traffic store have been
  unforced.

*/

void        	lpc_frc_flag_clr	(void * 	p_cb)

{
		P_CB->frc_flag = LP_UNFORCED;
}

/*  
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-10-01  created  LPDBR1.TXT  1.6  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_frc_flag_clr                                          */

/* new page
 ---------------------------------------------------------------------------- 
|
| function      lpc_frc_flag_set
|
| (in )         p_cb
|               
| return value  -
|		
 ------------   A B S T R A C T   ------------------------------------------- 


  The force flag state shows whether any process data in the target traffic
  store are forced. This function sets this state to "forced". It will be
  called by the "force_variable" function upon forcing a variable in the
  traffic store.

*/

void        	lpc_frc_flag_set	(void * 	p_cb)

{
		P_CB->frc_flag = LP_FORCED;
}

/*  
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-10-07  created  LPDBR1.TXT  1.6  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_frc_flag_set                                          */

/* new page
 ---------------------------------------------------------------------------- 
| 
| function      _lpc_frc_flag_sum
|
| (in )         p_cb
|               
| return value  result			LP_UNFORCED
|					LP_FORCED
|
 ------------   A B S T R A C T   -------------------------------------------

  The force flag states show whether any process data in the individual
  traffic stores are forced. This function reads the force flag of each
  traffic store and provides the value "forced" if at least one traffic
  store contains forced data, "unforced" if no traffic store contains forced
  data.

*/

TYPE_PD_RESULT     	_lpc_frc_flag_sum		(void)

{
int	i      	= 0;
short	result 	= LP_UNFORCED;

	for (i = 0; i < LPC_SIZE; i++)
	{
		if (lp_cb[i].frc_flag == LP_FORCED)
		{
			result = LP_FORCED;
			break;
		}
	}
	return (result);
}

/*  
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-10-07  created  LPDBR1.TXT  1.1  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lp_frc_flag_sum                                             */


/* new page
 ---------------------------------------------------------------------------- 
| 
| function      lpc_put_max
|
| (in )         p_cb
| (in )         pit_max
| (in )         prt_max
|               
| return value  result			LPS_OK
| 					LPS_RANGE
| 
 ------------   A B S T R A C T   ------------------------------------------- 

    pit_max     is used for range checking purposes
    prt_max     is used for range checking and to derive size of force table
		!!! Attention, prt_max must be the most significant port of
		    a 4 port dock set ( x x x x x x 1 1 ).

*/

void  		lpc_put_max      	(void *		    p_cb,
					             unsigned short	prt_addr_max,
					             unsigned short prt_indx_max)
{
	P_CB->prt_addr_max  = prt_addr_max;
	P_CB->prt_indx_max  = prt_indx_max;
}

/*  
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-09-23  created  LPDBR1.TXT  1.4  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_put_max                                               */

/* new page
 ---------------------------------------------------------------------------- 
| 
| function      lpc_put_state
|
| (in )         p_cb
| (in )         state
|
| return value  -
|
 ------------   A B S T R A C T   -------------------------------------------

  The process link layer checks whether a traffic store was successfully
  configured before performing an on-line operation upon it. This function
  is used during initialisation to declare a traffic store "in use".

  The traffic store is either

		LPC_TS_NOT_USED
		LPC_TS_USED
		LPC_TS_ERROR

*/

void		lpc_put_state		(void         * p_cb,
                				 short  		state)

{
    if (state <= LPC_STATE_MAX)
    {
	P_CB->state = state;
    }
    else
    {
	P_CB->state = LPC_TS_ERROR;
    }
}

/*
 ----------------------------------------------------------------------------
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-10-07  created  LPDBR1.TXT  1.4  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_put_state                                             */

/* new page
 ---------------------------------------------------------------------------- 
|
| function      lpc_put_ts_type
|
| (in )         p_cb
| (in )         ts_type
|               
| return value  -
|		
 ------------   A B S T R A C T   ------------------------------------------- 

  The traffic store type is decoded and stored as type of the individual
  traffic store elements pit, pcs and prt
  The pcs_power_of_2 is the coded size of the pcs in bytes (4/8 Bytes)

*/

short  	lpc_put_ts_type		(void * 	     p_cb,
					         unsigned short  ts_type)

{
short	result = LPS_OK;

	P_CB->ts_type = ts_type;

#if defined	(O_RP) || defined (LPD_RP2)

	if (ts_type == LP_TS_TYPE_A)
	{
		P_CB->pcs_power_of_2 = LPL_PCS_A_SIZE_IN_PWR2;
	}
	else
	{
		P_CB->pcs_power_of_2 = LPL_PCS_C_SIZE_IN_PWR2;
	}

#elif defined 	(O_KHW) || defined (O_PC) || defined (O_EKR)

	P_CB->pcs_power_of_2 = LPL_PCS_A_SIZE_IN_PWR2;

#else

	if       (ts_type == LP_TS_TYPE_A)
	{
		#if defined (O_BAP)
			P_CB->pcs_power_of_2 = LPL_PCS_A_SIZE_IN_PWR2;
		#else
			result = LPS_ERROR;
		#endif
	}
	else if  (ts_type == LP_TS_TYPE_B)
	{
		#if !defined (O_BAP)
		P_CB->pcs_power_of_2 = LPL_PCS_B_SIZE_IN_PWR2;
		#else
			result = LPS_ERROR;
		#endif
	}
	else if  (ts_type == LP_TS_TYPE_D)
	{
		P_CB->pcs_power_of_2 = LPL_PCS_D_SIZE_IN_PWR2;
	}
	else
	{
		result = LPS_ERROR;
	}

#endif

	return (result);

}

/*  
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  92-07-01  created                   Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_put_ts_type                                           */

/*
 ----------------------------------------------------------------------------
|
|   M O D I F I C A T I O N S
|   - - - - - - - - - - - - -
|
 ----------------------------------------------------------------------------

    file name: LPC_BLCK.C	 	Nr:	1	dated:	91-07-22

    name of object modified		Nr:	?  	dated:	9?-??-??

    description of what was done to object (not functionality!)

 ----------------------------------------------------------------------------

    file name: LPC_BLCK.C	 	Nr:	2	dated:	92-12-16

    lpc_enquiry       			Nr:	1  	dated:	92-12-16

    new function added to module

*/
