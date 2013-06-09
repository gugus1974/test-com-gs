/*   LPI_IFCE.H
 ----------------------------------------------------------------------------- 
                                                                               
     Property of  :   ABB Verkehrssyteme AG, CH-8050 ZÅrich / Switzerland      
     COPYRIGHT    :   (c) 1991 ABB Verkehrssysteme AG                          
                                                                               
 ----------------------------------------------------------------------------- 
                                                                               
     Project      :   IEC - TCN                                                
     Subproject   :   Link Process                                             
                                                                               
 ----------------------------------------------------------------------------- 
                                                                               
     File         :   LPP_PCMC.H   (Link Process Procedures)                   
                                                                               
     Document     :   [1]: IEC WG 22-5, Upper Layers, Version 1.3              
                      [2]: Link Layer Process Specification                    
                      [3]: Link Layer Process Design                           
                                                                               
     Abstract     :   Import interface not covered by PIL
                      functionality implemented differently for TCN
                      than for MicOS (for hardware reasons)                    
                                                                               
     Remarks      :                                                            
                                                                               
     Dependencies :                                                            
                                                                               
     Accepted     :                                                            

 -----------------------------------------------------------------------------

     HISTORY:

     Version    YY-MM-DD     Name           Dept.      Reference     Status
     -------    --------     ----------     ------     ---------     -------
        1       91-09-16     W. Marsden     BAEI-2         --

 -----------------------------------------------------------------------------
*/

#ifndef  LPP_PCMC_H
#define  LPP_PCMC_H

#if defined (O_PCMCIA)

#include "lp_sys.h"
#include "lc_sys.h"

#include "pcnode.h"

#define LPP_OK SUCCESS


int      dummy  ();

void   lpp_put_ts_short     (void *            p_cb,
                             void *            p_ts,
                             void *            p_ap);

void   lpp_get_ts_short     (void *            p_cb,
                             void *            p_ap,
                             void *            p_ts);

void   lpp_put_ts_array     (void *            p_cb,
                             void *            p_ts,
                             void *            p_ap,
                             unsigned short    size_x_2);

void   lpp_get_ts_array     (void *            p_cb,
                             void *            p_ap,
                             void *            p_ts,
                             unsigned short    size_x_2);

void   lpp_set_ts_array     (void *            p_cb,
                             void *            p_ts,
                             unsigned short    value,
                             unsigned short    size_of);

void   lpp_put_pit          (void *            p_cb,
                             LC_TYPE_TS_PIT *  p_pit,
                             unsigned short    prt_addr,
                             unsigned short    prt_indx);

void   lpp_get_pit          (void *            p_cb,
                             unsigned short *  p_ix,
                             LC_TYPE_TS_PIT *  p_pit,
                             unsigned short    prt_addr);

void   lpp_put_pcs          (void *            p_cb,
                             void *            p_pcs,
                             unsigned short    mask,
                             unsigned short    offset,
                             unsigned short    value);

void   lpp_get_pcs          (void *            p_cb,
                             unsigned int   *  p_val,
                             void *            p_pcs,
                             unsigned short    mask,
                             unsigned short    offset);

void   lpp_tgl_pcs          (void *            p_cb,
                             void *            p_pcs,
                             unsigned short    mask,
                             unsigned short    offset);

void   lpp_inc_pcs          (void *            p_cb,
                             void *            p_pcs,
                             unsigned short    mask,
                             unsigned short    offset);

void   lpp_set_pc_node      (int               node_no);


#endif






/*
 ----------------------------------------------------------------------------- 
|
| Version 2     Modifications
|
 ----------------------------------------------------------------------------- 



*/

#endif
