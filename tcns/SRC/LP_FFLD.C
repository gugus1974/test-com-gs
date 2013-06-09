/* LP_FFLD.C
 ---------------------------------------------------------------------------- 
                                                                              
     Property of  :   ABB Verkehrssyteme AG, CH-8050 ZÅrich / Switzerland     
     COPYRIGHT    :   (c) 1991 ABB Verkehrssysteme AG                         
                                                                              
 ---------------------------------------------------------------------------- 
                                                                              
     Project      :   MicOS                                                   
     Subproject   :   Process Data                                            
                                                                              
 ---------------------------------------------------------------------------- 
                                                                              
     File         :   LP_FFLD.C   (FORCE FIELD, Constant Definitions          
                                                                              
     Document     :   [1]: IEC WG 22-5, Upper Layers, Version 1.3             
                      [2]: Link Layer Process Specification                   
                      [3]: Link Layer Process Design                          
                                                                              
     Abstract     :   Separate File for force auxiliary constants 	      
                                                                              
     Remarks      :                                                           
                                                                              
     Dependencies :                                                           
                                                                              
     Accepted     :                                                           
                                                                              
 ---------------------------------------------------------------------------- 
                                                                              
     HISTORY:                                                                 
                                                                              
     Version    YY-MM-DD     Name           Dept.      Reference     Status   
     -------    --------     ----------     ------     ---------     -------  
        1       91-08-19     W. Marsden     BAEI-2         --                 
                                                                              
 ---------------------------------------------------------------------------- 
*/

#define  LP_FFLD_C

#if defined (O_PCMCIA)
#define O_LE
#endif
#include "lp_sys.h"			/*   needed by lpl_lacs.h	    */

#include "lpl_4tcn.h"			/*   TYPE_LPL_FRCE_PAGE...	    */

#include "lp_ffld.h"			/*   export declaration		    */


/* new page
 ---------------------------------------------------------------------------- 
|
| static var    lp_frc_field
|
 ------------   A B S T R A C T   ------------------------------------------- 
  

  Modifications to the force table bit mask are done by using these values as
  the application variable and calling a "write port" function. 
  As the maximum application variable size is 16 words, the force field is 
  256 bits.

  The lp_frc_field is passed to force a variable, the lp_unfrc_field is
  passed to unforce a variable (clear mask, clear value in table).

*/

#if defined (O_TCN)
const	LC_TYPE_TS_PGE_WTB      lp_frc_field =
        {
             0xffff, 0xffff, 0xffff, 0xffff,
             0xffff, 0xffff, 0xffff, 0xffff,
             0xffff, 0xffff, 0xffff, 0xffff,
             0xffff, 0xffff, 0xffff, 0xffff,
             0xffff, 0xffff, 0xffff, 0xffff,
             0xffff, 0xffff, 0xffff, 0xffff,
             0xffff, 0xffff, 0xffff, 0xffff,
             0xffff, 0xffff, 0xffff, 0xffff,
             0xffff, 0xffff, 0xffff, 0xffff,
             0xffff, 0xffff, 0xffff, 0xffff,
             0xffff, 0xffff, 0xffff, 0xffff,
             0xffff, 0xffff, 0xffff, 0xffff,
             0xffff, 0xffff, 0xffff, 0xffff,
             0xffff, 0xffff, 0xffff, 0xffff,
             0xffff, 0xffff, 0xffff, 0xffff,
             0xffff, 0xffff, 0xffff, 0xffff
        };
const	LC_TYPE_TS_PGE_WTB      lp_unfrc_field =
        {
             0,      0,      0,      0     ,
             0,      0,      0,      0     ,
             0,      0,      0,      0     ,
             0,      0,      0,      0     ,
             0,      0,      0,      0     ,
             0,      0,      0,      0     ,
             0,      0,      0,      0     ,
             0,      0,      0,      0     ,
             0,      0,      0,      0     ,
             0,      0,      0,      0     ,
             0,      0,      0,      0     ,
             0,      0,      0,      0     ,
             0,      0,      0,      0     ,
             0,      0,      0,      0     ,
             0,      0,      0,      0     ,
             0,      0,      0,      0
        };


#else

const	LC_TYPE_TS_PGE      lp_frc_field =
        {
            { 0xffff, 0xffff, 0xffff, 0xffff},
            { 0xffff, 0xffff, 0xffff, 0xffff},
            { 0xffff, 0xffff, 0xffff, 0xffff},
            { 0xffff, 0xffff, 0xffff, 0xffff}
        };

const	LC_TYPE_TS_PGE      lp_unfrc_field =
        {
            { 0,      0,      0,      0     },
            { 0,      0,      0,      0     },
            { 0,      0,      0,      0     },
            { 0,      0,      0,      0     }
        };

#endif

/*
 ----------------------------------------------------------------------------- 
| 
| Version 0.2   Modifications
| 
 ----------------------------------------------------------------------------- 



*/
