/* LP_TMO.H
 ---------------------------------------------------------------------------- 
                                                                              
     Property of  :   ABB Verkehrssyteme AG, CH-8050 ZÅrich / Switzerland     
     COPYRIGHT    :   (c) 1991 ABB Verkehrssysteme AG                         
                                                                              
 ---------------------------------------------------------------------------- 
                                                                              
     Project      :   IEC-TCN                                                 
     Subproject   :   Link Process                                            
                                                                              
 ---------------------------------------------------------------------------- 
                                                                              
     File Name    :   LP_TMO.H 	                                              
                                                                              
     Document     :   see source file                                         
                                                                              
     Abstract     :         "                                                 
                                                                              
     Remarks      :         "                                                 
                                                                              
     Dependencies :         "                                                 
                                                                              
     Accepted     :         "

 ----------------------------------------------------------------------------

     HISTORY:

     Version    YY-MM-DD     Name           Dept.      Reference     Status
     -------    --------     ----------     ------     ---------     -------
       0.1      91-11-26     W. Marsden     BAEI-2         --        created

 ----------------------------------------------------------------------------
*/

#ifndef   LP_TMO_H
#define   LP_TMO_H

#include  "lp_sys.h"
/*
 ----------------------------------------------------------------------------
|
| constant      LP_TMO_CYCLE
|
 ------------   A B S T R A C T   -------------------------------------------


  The timeout counter cycle is defined here. This constant is used to convert
  the tick count into ms. LP cannot guarantee, that the application will
  actually update the timeout counter with this cycle time.

  !!!Attention: LPLACS.lpl_refresh() shifts 6. Modifications of this constant
		will have a side affect upon this function.

*/
  
#define		LP_TMO_CYCLE		64


/*
 -----------------------------------------------------------------------------
|
| Constants
|
 ------------   A B S T R A C T   --------------------------------------------

  LP_TMO_MAX_COUNT specifies the maximum count for lp_counter. The task_cycle
  in the equation refers to the cyclic call of lp_counter.

  The constant LP_TMO_MAX_TIME is defined in lp_sys.h
  The constant LP_TMO_CYCLE is defined in lp_tmo.h

*/

#define		LP_TMO_MAX_COUNT	((LP_TMO_MAX_TIME / LP_TMO_CYCLE))



/*
 ----------------------------------------------------------------------------
|
| prototype     lp_tmo_init
|
 ---------------------------------------------------------------------------- 
*/

void            lp_tmo_init		(void);

#endif
