/*************************************************************************
**        Copyright (C) Siemens AG 1997 All Rights Reserved. Confidential    
**========================================================================
**
**    PROJECT::    TCN Train Communication Network
                   JDP Joint Development Project

**    MODULE::     UWTM UIC - WTB - Manager

**    WORKFILE::   $Workfile:   UWTM.H  $
**************************************************************************
**    TASK::       Headerfile for WTB Manager 

**************************************************************************
**    AUTHOR::     Reinwald (Rei)  
**    CREATED::    06.11.97
**========================================================================
**    HISTORY::    AUTHOR::   $Author: schiavor $
                   REVISION:: $Revision: 1.1.2.1 $  
                   MODTIME::  $Date: 2006/12/01 10:41:46 $
      $Log: UWTM.H,v $
      Revision 1.1.2.1  2006/12/01 10:41:46  schiavor
      01/12/2006                         Napoli                            SchiavoR
      Primo inserimento per utilizzo libreria nel GTW

 * 
 *    Rev 1.8   01 Oct 1998 16:35:14   REINWALD_LU
 * new functions: uwtm_get_topo and uwtm_send_topo_disable
 * 
 *    Rev 1.7   14 Jul 1998 11:49:18   REINWALD_LU
 * Master conflict functionality moved to uwtm
 * 
 *    Rev 1.6   26 May 1998 13:15:28   REINWALD_LU
 * interface cleaned
 * 
 *    Rev 1.5   19 May 1998 15:51:02   REINWALD_LU
 * kft_used removed
 * 
 *    Rev 1.4   04 May 1998 09:49:20   REINWALD_LU
 * complete interface of UWTM inserted
 * 
 *    Rev 1.3   20 Apr 1998 12:21:34   REINWALD_LU
 * OCTET replaced by UNSIGNED8
 * 
 *    Rev 1.2   18 Mar 1998 16:49:14   REINWALD_LU
 * consts moved to ums.h, interface changed
 * 
 *    Rev 1.0   27 Nov 1997 15:43:22   REINWALD_LU
 * First Release
**************************************************************************/

#ifndef UWTM_H_
#define UWTM_H_

#ifndef TCNTYPES_H_
#error include "tcntypes.h" first 
#endif


#ifndef UMS_H_
#error include "ums.h" first
#endif

#ifndef BMI_H_
#error include "bmi.h" first
#endif

/* Macro defines */
/* Single node */
#define UIC_SINGLE                       99

/* Types */
typedef int (* UWTM_REPORT_TYPE) (UNSIGNED8,UNSIGNED8);

typedef struct {
  UNSIGNED8             omode;
  UNSIGNED8             single_node;
  Type_Configuration    wtb_config;
} UWTM_NODE_CONFIG_TABLE_T;

/* Variables */

/* Functions */

void uwtm_init(void);
/*{ init procedure, to be called first of all }*/ 

int uwtm_configure(UWTM_NODE_CONFIG_TABLE_T *nct, UWTM_REPORT_TYPE report);
/*{ configuration of the node parameters (WTB), to be called once after init }*/ 

int uwtm_request(void      *p_uic_callmsg,   /* pointer to input message    */
                 WORD32   uic_call_size,     /* size of message in bytes    */
                 AM_ADDRESS *caller,         /* address of caller */
                 void      **p_uic_replymsg, /* pointer to reply message    */
                 WORD32   *p_uic_reply_size, /* size of reply message       */
                 REPLY_FREE *free_replymsg); /* pointer to mem free function */
/*{ handles UIC message calls according to UIC LL 556. Attention: messages   
    must be in bytestream format ! }*/ 

void uwtm_stop_old_map(void);
/*{ stops mapping server task in map_task }*/

int map_wtb_inaugurated(void);
/*{ delivers TRUE after an UIC inauguration is completed }*/

l_result uwtm_get_topo(Type_Topography *topo);
/*{ Replacement of ls_t_GetTopography() which also provides a valid topo 
    in single node mode }*/

void uwtm_red_indicate(void);
/*{ delivers redundancy state to utbc. Call this procedure, if the startup was
    after a redundancy switch. Do not call otherwise. }*/

void uwtm_send_topo_disable(void);
/*{ disables sending topo frames and stops pdm in single node mode }*/

UNSIGNED8 uwtm_get_state(void);
/*{ delivers uwtm state }*/

UNSIGNED8 uwtm_is_leading(void);
/*{ delivers, if vehicle or trainset is leading }*/

UNSIGNED8 uwtm_leading_dir(void);
/*{ delivers direction of leading }*/

UNSIGNED8 uwtm_my_uic_addr(void);
/*{ delivers own UIC range number }*/

UNSIGNED8 uwtm_get_nadi_state(void);
/*{ delivers NADI state }*/

int uwtm_master_conflict(void);
/*{ TRUE: Strong master conflict; FALSE: otherwise }*/

void uwtm_begin_master_conflict(void);
void uwtm_end_master_conflict(void);
/*{ signal begin and end of master conflict to the uwtm, called only by WTB-LL }*/

void uic_ch_omode_requ(UNSIGNED8* status);

void uic_inaug_ctrl_requ(UNSIGNED8* status);

void uic_inaug_enf_requ(UNSIGNED8* status);

void uic_lead_requ(UNSIGNED8* status, UNSIGNED8 direc);

void uic_sleep_requ(UNSIGNED8* status);

int uic_fc_read_uwtm_topography_request(
  UIC_TC_READ_UWTM_TOPOGRAPHY *call_msg,
  UIC_TR_READ_UWTM_TOPOGRAPHY **reply_msg,
  REPLY_FREE *free_reply_msg);

int uic_fc_read_uwtm_state_request(
  UIC_TC_READ_UWTM_STATE *call_msg,
  UIC_TR_READ_UWTM_STATE **reply_msg,
  REPLY_FREE *free_reply_msg);

int uic_fc_sleep_request(
  UIC_TC_SLEEP *call_msg,
  UIC_TR_SLEEP **reply_msg,
  REPLY_FREE *free_reply_msg);

int uic_fc_leading_request(
  UIC_TC_LEADING *call_msg,
  UIC_TR_LEADING **reply_msg,
  REPLY_FREE *free_reply_msg);

int uic_fc_inauguration_enforce_request(
  UIC_TC_INAUGURATION_ENFORCE *call_msg,
  UIC_TR_INAUGURATION_ENFORCE **reply_msg,
  REPLY_FREE *free_reply_msg);

int uic_fc_inauguration_control_request(
  UIC_TC_INAUGURATION_CONTROL *call_msg,
  UIC_TR_INAUGURATION_CONTROL **reply_msg,
  REPLY_FREE *free_reply_msg);

int uic_fc_change_omode_request(
  UIC_TC_CHANGE_OMODE *call_msg,
  UIC_TR_CHANGE_OMODE **reply_msg,
  REPLY_FREE *free_reply_msg);

#endif /* _UWTM_H */


