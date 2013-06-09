/* LP_FFLD.H
 ---------------------------------------------------------------------------- 
                                                                              
     Property of  :   ABB Verkehrssyteme AG, CH-8050 ZÅrich / Switzerland     
     COPYRIGHT    :   (c) 1991 ABB Verkehrssysteme AG                         
                                                                              
 ---------------------------------------------------------------------------- 
                                                                              
     Project      :   IEC - TCN                                               
     Subproject   :   Process Data                                            
                                                                              
 ---------------------------------------------------------------------------- 
                                                                              
     File         :   LP_FFLD.H   (FORCE FIELD, Constant Definitions          
                                                                              
     Document     :   [1]: IEC WG 22-5, Upper Layers, Version 1.3             
                      [2]: Link Layer Process Specification                   
                      [3]: Link Layer Process Design                          
                                                                              
     Abstract     :   Separate File for constants (RP II)                     
                                                                              
     Remarks      :                                                           
                                                                              
     Dependencies :                                                           
                                                                              
     Accepted     :                                                           
                                                                              
 ---------------------------------------------------------------------------- 
                                                                              
     HISTORY:                                                                 
                                                                              
     Version    YY-MM-DD     Name           Dept.      Reference     Status   
     -------    --------     ----------     ------     ---------     -------  
        1       92-05-29     W. Marsden     BAEI-2         --                 
                                                                              
 ---------------------------------------------------------------------------- 
*/

#ifndef	 LP_FFLD_H
#define  LP_FFLD_H

#include "lpl_4tcn.h"
#include "lc_sys.h"

/* new page
 ---------------------------------------------------------------------------- 
|
| static var    lp_frc_field
|
 ------------   A B S T R A C T   ------------------------------------------- 
  

  The force state bit field is set by the lp_frc_variable routine and cleared
  by the lp_unfrc_variable routine.
  These routines call the lpl_put_variable routine to write the bit field.

  The lpl_put_variable writes a variable into a traffic store like memory
  area. The lp_frc_field is the variable taken for writing when "forcing",
  the lp_unfrc_field is the variable taken for writing when "unforcing".
  The size is the "worst case": an array of 256 bit.

*/
#if defined O_TCN
extern	const	LC_TYPE_TS_PGE_WTB    lp_frc_field;
extern	const	LC_TYPE_TS_PGE_WTB    lp_unfrc_field;
#else
extern	const	LC_TYPE_TS_PGE	      lp_frc_field;
extern	const	LC_TYPE_TS_PGE	      lp_unfrc_field;
#endif
/*
 ----------------------------------------------------------------------------- 
| 
| Version 0.2   Modifications
| 
 ----------------------------------------------------------------------------- 



*/
#endif
