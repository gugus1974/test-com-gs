/*   LP_INDT.C
 ---------------------------------------------------------------------------- 
                                                                              
     Property of  :    ABB Verkehrssysteme AG, CH-8050 ZÅrich / Switzerland   
     COPYRIGHT    :    (c) 1991 ABB Verkehrssysteme AG                        
                                                                              
 ---------------------------------------------------------------------------- 
                                                                              
     PROJECT      :    MicOS Version 2                                        
                                                                              
     SUBPROJECT   :    Process Data (LP)                                      
                                                                              
 ---------------------------------------------------------------------------- 
                                                                              
     MODULE       :    lp_indt.c                                              
                                                                              
     DOCUMENT     :                                                           
                                                                              
     ABSTRACT     : 1. Definition of contents of indirect call table          
                       for indirect calling mechanism                         

                    2. Version verification                                   
                                                                              
     REMARKS      :                                                           
                                                                              
     DEPENDENCIES :                                                           
                                                                              
     ACCEPTED     :                                                           
                                                                              
     HISTORY      :   See end of file                                         
                                                                              
 ---------------------------------------------------------------------------- 

    LP_EVOLUTION is incremented upon each new release.
    LP_UPDATE    is reserved (will be defined later)

    These constants are used for the project version management.
*/


#define	 LP_INDT_C

#if defined (O_PCMCIA)
#define O_LE
#endif
#include "apd_incl.h"
#include "lp_sys.h"		/*   Structure TYPE_LP_IND_CALL decl	    */

#if !defined (O_EKR)

TYPE_PD_RESULT  _lpm_mbox_req_create    (const  struct STR_LP_TS_CFG * p_ts_cfg)
{
    p_ts_cfg = p_ts_cfg;
	return (0);
}
#endif


/* new page
 -----------------------------------
  constant table for indirect call
 -----------------------------------
*/

const TYPE_PD_IND_CALL pd_ind_call_table =
{
 {PD_VERSION, PD_RELEASE, PD_UPDATE, PD_EVOLUTION},
 _apd_init,
 _apd_put_variable,
 _apd_get_variable,
 _apd_put_set     ,
 _apd_put_set_fast,
 _apd_get_set     ,
 _apd_get_set_fast,
 _apd_put_cluster ,
 _apd_get_cluster ,
 _apd_force_variable,
 _apd_unforce_variable,
 _apd_unforce_all ,
 _apd_subscribe   ,
 _apd_desubscribe ,
 _apd_pass_subscription ,
 _apd_pack        ,
 _apd_unpack      ,
 _lpd_gen_pv_set  ,
 _lpd_mod_pv_set
};


#if defined (O_BAP)
const TYPE_LP_IND_CALL lp_ind_call_table =
{
    {LP_VERSION, LP_RELEASE, LP_UPDATE, LP_EVOLUTION},
    _lp_init,
    _lpm_mbox_req_create,
    _lp_create,
    _lp_cluster,
    _lp_cluster,
    _lp_get_data_set,
    _lp_put_data_set,
    _lp_get_variable,
    _lp_put_variable,
    _lpa_pack,
    _lpa_unpack,
    _lp_frc_single,
    _lp_unfrc_all,
    _lpc_frc_flag_sum,
    _lp_tmo_clr_state,
    _lp_tmo_get_state,
    _lp_tmo_set_state,
    _lp_tmo_counter,
    _lp_subscribe,
    _lp_pv_size,
    _ap_get_variable,
    _ap_put_variable,
    _ap_frc_single,
    _lpx_gen_clus_ts,
    _lpx_gen_clus_ds,
    _lpx_gen_ds_set,
    _lpc_enquiry,
    _lp_desubscribe,
    _lpl_get_pv_x,
    _lpl_put_pv_x
};
#else
const TYPE_LP_IND_CALL lp_ind_call_table =
{
    {LP_VERSION, LP_RELEASE, LP_UPDATE, LP_EVOLUTION},
    _lp_init,
    _lp_create,
    _lp_cluster,
    _lp_get_data_set,
    _lp_put_data_set,
    _lp_get_variable,
    _lp_put_variable,
    _lpa_pack,
    _lpa_unpack,
    _lp_frc_single,
    _lp_unfrc_all,
    _lpc_frc_flag_sum,
    _lp_pv_size,
    _ap_get_variable,
    _ap_put_variable,
    _ap_frc_single,
    _lpx_gen_clus_ts,
    _lpx_gen_clus_ds,
    _lpx_gen_ds_set,
    _lpc_enquiry,
    _lpl_get_pv_x,
    _lpl_put_pv_x
};
#endif

/*
 -----------------------------------
  Loc. for ptr to function table     
 ----------------------------------- 
*/

/* Required if pointer to indirect call table cannot be disposed to a        */
/* pre-defined memory location.  In this case, a memory location is allo-    */
/* instead.  Initialization is accomplished with am_connect(), as usual.     */

    TYPE_LP_IND_CALL*  lp_pc_ptr_to_table;
    TYPE_PD_IND_CALL*  pd_pc_ptr_to_table;


/*
 ----------------------------------------------------------------------------
|
|   M O D I F I C A T I O N S
|   - - - - - - - - - - - - -
|
 ----------------------------------------------------------------------------

    file name: LP_INDT.C	 	Nr:	1	dated:	92-03-24

    name of object modified		Nr:	?  	dated:	9?-??-??

    description of what was done to object (not functionality!)

 ----------------------------------------------------------------------------

    file name: LP_INDT.C 	 	Nr:	2	dated:	93-01-27

    version management			Nr:	1  	dated:	93-01-27

    constants were moved from LP_SYS.H to this file

 ----------------------------------------------------------------------------

    file name: LP_INDT.C 	 	Nr:	3	dated:	93-05-26

    Increment of LP_EVOLUTION

    Correction made to module LP_INIT.C

 ----------------------------------------------------------------------------

    file name: LP_INDT.C 	   Evol-Nr:	7	dated:	93-06-15

    Increment of LP_EVOLUTION

    Correction made to module LP_AS2C.C
    Switch added to headerfile LP_SYS.H

    Addition made to module   LP_IRPT.C

    New function: _lp_desubscribe
 ----------------------------------------------------------------------------

    file name: LP_INDT.C 	   Evol-Nr:	8	dated:	93-07-30

    Increment of LP_EVOLUTION

    Correction made to module LP_FRCE.C
 ----------------------------------------------------------------------------

    file name: LP_INDT.C 	   Evol-Nr:	9	dated:	93-12-02

    Increment of LP_EVOLUTION

    Correction made to module LP_INIT.C
 ----------------------------------------------------------------------------

    file name: LP_INDT.C 	   Evol-Nr:	10	dated:	93-12-13

    Increment of LP_EVOLUTION

    Correction made to module LP_SYS.H

 ----------------------------------------------------------------------------

    file name: LP_INDT.C 	   Evol-Nr:	11	dated:	94-01-14

    Increment of LP_EVOLUTION

    Correction made to module LPA_AS2C.C
    Correction made to module LPA_ASXX.H
    Correction made to module LPL_LACS.H
    Correction made to module LPL_4TCN.H

 ----------------------------------------------------------------------------

    file name: LP_INDT.C 	   Evol-Nr:	13	dated:	94-03-16

    Increment of LP_EVOLUTION

    MicOS release 

 ----------------------------------------------------------------------------

    file name: LP_INDT.C 	   Evol-Nr:	14	dated:	94-03-16

    Increment of LP_EVOLUTION

    JDP release with MVBC for little and big endian representation
 ----------------------------------------------------------------------------

    file name: LP_INDT.C 	   Evol-Nr:	15	dated:	94-06-14

    Increment of LP_EVOLUTION

    Correction to Force Table for MVBC: Mask and Data Swapped
    See files: LPL_4TCN.H / LP_VAR.C / LPA_AS2C.C

 ----------------------------------------------------------------------------

    file name: LP_INDT.C 	   Evol-Nr:	16	dated:	94-09-23

    Increment of LP_EVOLUTION
    Modification of freshness counter
    See files: LPL_4TCN.H / LP_TMO.C
    New structure PV_NAME for train bus
    See file:  LP_SYS.H
    new JDP function lp_get_version
 ----------------------------------------------------------------------------

    file name: LP_INDT.C 	   Evol-Nr:      17	dated:	94-09-23

    Increment of LP_EVOLUTION
    Modification of subscription mechanism
    See files: APD_LAYER.C / APD_INCL.H

*/
