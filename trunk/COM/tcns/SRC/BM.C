/*
BUSMANAGER WTB V1.9
iMB, Mike Becker
  3. Oktober 1995,  9:06
*/
/*
GENERAL TOP OF FILE
*/
/*
;=========================================================================
;                  copyright 1994 by ABB HENSCHEL
;-------------------------------------------------------------------------
; project   : TRAIN COMMUNICATION NETWORK
;-------------------------------------------------------------------------
; department: AHE/STL3      author: Mike Becker           date: 28.11.1994
;-------------------------------------------------------------------------
; filename  : \TCN\MOD\BM.30\SOURCE\BM.X
;-------------------------------------------------------------------------
; function  : busmanager of TCN link layer  version 3.0
;=========================================================================
*/
/*
NOTES
*/
/*
;= 1 =====================================================================
;  this C-source should be ANSI-C-compatible
;  it was compiled with INTEL-C-compiler V4.1,
;  ANSI-C-compatibility was checked with BORLAND-C-compiler V3.1
;=========================================================================
*/
/*
LIST OF CHANGES SINCE DATE OF CREATION
*/
/*
;-------------------------------------------------------------------------
;- nr.  0 -- author: Mike Becker ----- date: xx.xx.1994 -- version: 0.0  -
;  completed
;-------------------------------------------------------------------------
; see #Br/#Kvr                                                           -
;=========================================================================



 040495  Le  1.0  created
 300595  Le  1.1  md_free_packets() moved to MAC
 300695  Le  1.2  Change User Report added,
                  BMI/MMI-Reports now via ZAS
 100795  Le  2.0  support for statistical data
 200995  Le  2.1  - pointer to "bmi_busy" passed in "tbm_init"
                  - SetSleep,... now treated in tbm_general_events
 300796  cs  4.0  follow up of bug fixes that have been detected for version v3.1 and v3.2
                  (3) changes to BM to avoid open bus_switch problem:
                      new command MAC_BM_CloseSwitch_Req and
                      indication  BM_MAC_CloseSwitch_Con and
                      new state SMS_W4_CLOSE_CON_GO_CONFIGURED
 151096  cs  4.1  (22)messenger is now informed before the control interface about a
                      successful naming
 051196  cs  4.1  (26)messenger needs to be informed after each new inauguration
 270197  cs  4.1  (41)consistency for topography information enhanced
                  (42)local function for copying topography frame
 270197  cs       (44)diag-entries now in diag.h defined
 270197  cs       (45)resetting of busy flag in case that the BM_BMI_xxx messages are ignored
*/
/*
COMPILER SWITCHES
*/
#ifdef O_960
#include "coco.h"
#endif
/*
INCLUDEFILES
*/
#include "tcntypes.h"   /*4.0*/
#include "bmi.h"
#include "zas.h"
#include "diagwtb.h"
#if defined(O_960) || defined(O_VCU6U_GW)
#include "pi_sys.h"
#include "pd_jdp.h"
#else
#include "pil.h"
#include "pd_hand.h"
#endif
#include "mmi.h"
#include "ll.h"
#include "nvm.h"
/*
IMPLEMENTATION
*/
/*
DEFINES
*/
#define MD_OK             0   /* very bad style !! use md_layer.h */
#define MD_INAUGURATION   2
/*
states
*/
#define SMS_INITIALIZED                               0x01
#define SMS_W4_CONFIG_CON                             0x02
#define SMS_CONFIGURED                                0x03
#define SMS_W4_STOP_CON_GO_MASTER                     0x04
#define SMS_W4_STOP_CON_GO_CONFIGURED_VIA_UNNAME      0x05
#define SMS_W4_STOP_CON_GO_SLAVE                      0x06
#define SMS_W4_STOP_CON_GO_CONFIGURED                 0x07
#define SMS_W4_UNNAME_CON_GO_MASTER                   0x08
#define SMS_W4_UNNAME_CON_GO_CONFIGURED               0x09
#define SMS_W4_UNNAME_CON_GO_SLAVE                    0x0A
#define SMS_W4_CLOSE_CON_GO_CONFIGURED                0x0B     /*4.0-3*/
/*4.0-3: for the following states the value has been incremented by 1 */

#define SS_INCLUSION                                  0x0C
#define SS_INHIBITED                                  0x0D
#define SS_W4_NAMING                                  0x0E
#define SS_W4_END_OF_NAMING                           0x0F
#define SS_REGULAR_OPERATION                          0x10

#define SM_READY_TO_NAME                              0x11
#define SM_W4_FIRST_NODE                              0x12
#define SM_W4_NAME_CON                                0x13
#define SM_W4_TOPO_IND                                0x14
#define SM_REG_OPERATION_STRONG                       0x15
#define SM_REG_OPERATION_WEAK                         0x16
#define SM_W4_UNNAME_CON_AFTER_FAILED_NAMING          0x17
#define SM_W4_UNNAME_CON_AFTER_FAILED_MASTER          0x18
/*
TYPEDEFS
*/

/*
DECLARATIONS
*/
/*
trace variables
*/
#ifdef O_BM_TRACE
unsigned char  tbm_trace[256] ;
unsigned char  tbm_trace_ndx  ;
#endif
static unsigned char tbm_state  ; /* actual state of BM state machine */
static unsigned char tbm_event  ; /* event, which caused activation of BM */
static Type_ZASElement tbm_msg2mac ; /* message to MAC */
static Type_ZASElement tbm_msg2bmi ; /* message to BMI */
static unsigned char * bmi_busy ;
static Type_Configuration tbm_configuration ;
static Type_SaveData tbm_SV ;
static unsigned short       tbm_ms ;
static unsigned short       tbm_inhibit ;
static unsigned short       tbm_detection_result ;
static unsigned short       tbm_inclusion_result ;
static unsigned short       tbm_master_reason ;
static unsigned short       tbm_slave_reason ;
static Type_NodeDescriptor  tbm_desc ;
static unsigned short       tbm_name_result ;
static Type_TopographyBuffer * p_topo_buffer ;
static Type_WTBStatus        * p_WTB_status ;
/*
PROTOTYPES OF LOCAL FUNCTIONS
*/
static void tbm_error          (void) ;
static void bm_zas_entry       (Type_ZASElement * msg) ;
static void bmi_report         (l_report report) ;
static void tbm_general_events (void);
static void bm_copy_topography (Type_Topography *p_new_topo); /*4.1-42*/
/*
void bm_zas_entry (Type_ZASElement * msg)

                     B M   S T A T E   M A C H I N E
*/
void bm_zas_entry (Type_ZASElement * msg)   /* WTB - BUSMANAGER  */
{
  tbm_event = msg->enr ;
  /*
  trace
  */
#ifdef O_BM_TRACE
  tbm_trace[tbm_trace_ndx++] = tbm_event ;
  tbm_trace[tbm_trace_ndx++] = tbm_state ;
#endif
  switch (tbm_state)
  {
    /*
    common states
    */
    /*
    SMS_INITIALIZED
    */
    case SMS_INITIALIZED:
    {
      if (tbm_event == BM_BMI_Configure)
      {
        tbm_configuration = *((Type_Configuration*)(msg->p34)) ;
        tbm_msg2mac.enr = MAC_BM_Configure_Req ;
        tbm_msg2mac.p34 = &tbm_configuration ;
        zas_send(WSNR_MAC, &tbm_msg2mac) ;
        tbm_state = SMS_W4_CONFIG_CON ;
      }
      else
      {
        tbm_general_events() ;
      }
      break;
    }
    /*
    SMS_W4_CONFIG_CON
    */
    case SMS_W4_CONFIG_CON:
    {
      if (tbm_event == BM_MAC_Configure_Con)
      {
        p_WTB_status->link_layer_state = LS_CONFIGURED ;
        *bmi_busy = FALSE ;
        bmi_report (LR_CONFIGURED) ;
        tbm_state = SMS_CONFIGURED ;
      }
      else
      {
        tbm_general_events() ;
      }
      break;
    }
    /*
    SMS_CONFIGURED
    */
    case SMS_CONFIGURED:
    {
      switch (tbm_event)
      {
        case BM_BMI_SetStrong:
        {
          flush_recovery_data();
          tbm_ms = STRONG_STRENGTH ;
          p_WTB_status->link_layer_state = LS_READY_TO_NAME ;
          *bmi_busy = FALSE ;
          bmi_report(LR_STRONG) ;
          tbm_state = SM_READY_TO_NAME ;
          break;
        }
        case BM_BMI_SetSlave:
        {
          tbm_ms = SLAVE_STRENGTH ;
          /*
          try to include slave
          */
          if ( restore_recovery_data (&tbm_SV) == 0 )
          {
            tbm_msg2mac.p34 = &tbm_SV ;
          }
          else
          {
            /* no recovery data available */
            tbm_msg2mac.p34 = (void*)0 ;
          }
          tbm_msg2mac.enr = MAC_BM_Inclusion_Req ;
          zas_send(WSNR_MAC, &tbm_msg2mac) ;
          tbm_state = SS_INCLUSION ;
          break;
        }
        case BM_BMI_SetWeak:
        {
          tbm_ms = WEAK_STRENGTH ;
          /*
          try to include weak master
          */
          if ( restore_recovery_data (&tbm_SV) == 0 )
          {
            tbm_msg2mac.p34 = &tbm_SV ;
          }
          else
          {
            tbm_msg2mac.p34 = (void*)0 ;
          }
          tbm_msg2mac.enr = MAC_BM_Inclusion_Req ;
          zas_send(WSNR_MAC, &tbm_msg2mac) ;
          tbm_state = SS_INCLUSION ;
          break;
        }
        default:
        {
          tbm_general_events() ;
        }
      }
      break;
    }
    /*
    SMS_W4_STOP_CON_GO_CONFIGURED
    */
    case SMS_W4_STOP_CON_GO_CONFIGURED:
    {
      if (tbm_event == BM_MAC_Stop_Con)
      {
        /*4.0-3...
        p_WTB_status->link_layer_state = LS_CONFIGURED;
        *bmi_busy = FALSE;
        bmi_report(LR_REMOVED);
        tbm_state = SMS_CONFIGURED;
        ...4.0-3...*/
        tbm_msg2mac.enr = MAC_BM_CloseSwitch_Req ;
        zas_send(WSNR_MAC, &tbm_msg2mac) ;
        tbm_state = SMS_W4_CLOSE_CON_GO_CONFIGURED ;
        /*...4.0-3*/
      }
      else
      {
        tbm_general_events() ;
      }
      break;
    }
    /*
    SMS_W4_STOP_CON_GO_CONFIGURED_VIA_UNNAME
    */
    case SMS_W4_STOP_CON_GO_CONFIGURED_VIA_UNNAME:
    {
      if (tbm_event == BM_MAC_Stop_Con)
      {
        md_status_indicate (MD_INAUGURATION) ;
        tbm_msg2mac.enr = MAC_BM_Unname_Req ;
        zas_send(WSNR_MAC, &tbm_msg2mac) ;
        tbm_state = SMS_W4_UNNAME_CON_GO_CONFIGURED ;
      }
      else
      {
        tbm_general_events() ;
      }
      break;
    }
    /*
    SMS_W4_UNNAME_CON_GO_CONFIGURED
    */
    case SMS_W4_UNNAME_CON_GO_CONFIGURED:
    {
      if (tbm_event == BM_MAC_Unname_Con)
      {
        /*4.0-3...
        p_WTB_status->link_layer_state = LS_CONFIGURED;
        *bmi_busy = FALSE;
        bmi_report(LR_REMOVED);
        tbm_state = SMS_CONFIGURED ;
        ...4.0-3...*/
        tbm_msg2mac.enr = MAC_BM_CloseSwitch_Req ;
        zas_send(WSNR_MAC, &tbm_msg2mac) ;
        tbm_state = SMS_W4_CLOSE_CON_GO_CONFIGURED ;
        /*...4.0-3*/
      }
      else
      {
        tbm_general_events() ;
      }
      break;
    }
    /*
    4.0-3:
    SMS_W4_CLOSE_CON_GO_CONFIGURED
    */
    /*4.0-3...*/
    case SMS_W4_CLOSE_CON_GO_CONFIGURED:
    {
      if (tbm_event == BM_MAC_CloseSwitch_Con)
      {
        p_WTB_status->link_layer_state = LS_CONFIGURED ;
        *bmi_busy = FALSE ;
        bmi_report(LR_REMOVED) ;
        tbm_state = SMS_CONFIGURED ;
      }
      else
      {
        tbm_general_events() ;
      }
      break;
      /*4.0-3*/
    }

    /*
    SMS_W4_STOP_CON_GO_SLAVE
    */
    case SMS_W4_STOP_CON_GO_SLAVE:
    {
      if (tbm_event == BM_MAC_Stop_Con)
      {
        md_status_indicate (MD_INAUGURATION) ;
        tbm_msg2mac.enr = MAC_BM_Unname_Req ;
        zas_send(WSNR_MAC, &tbm_msg2mac) ;
        tbm_state = SMS_W4_UNNAME_CON_GO_SLAVE ;
      }
      else
      {
        if (tbm_event == BM_BMI_Remove)
        {
          tbm_state = SMS_W4_STOP_CON_GO_CONFIGURED_VIA_UNNAME ;
        }
        else
        {
          tbm_general_events() ;
        }
      }
      break;
    }
    /*
    SMS_W4_UNNAME_CON_GO_SLAVE
    */
    case SMS_W4_UNNAME_CON_GO_SLAVE:
    {
      if (tbm_event == BM_MAC_Unname_Con)
      {
        tbm_msg2mac.enr = MAC_BM_Detect_Req ;
        tbm_msg2mac.p1  = SLAVE_STRENGTH ;
        zas_send(WSNR_MAC, &tbm_msg2mac) ;
        tbm_state = SS_W4_NAMING ;
      }
      else
      {
        if (tbm_event == BM_BMI_Remove)
        {
          tbm_state = SMS_W4_UNNAME_CON_GO_CONFIGURED ;
        }
        else
        {
          tbm_general_events() ;
        }
      }
      break;
    }

    /*
    SMS_W4_STOP_CON_GO_MASTER
    */
    case SMS_W4_STOP_CON_GO_MASTER:
    {
      if (tbm_event == BM_MAC_Stop_Con)
      {
        md_status_indicate (MD_INAUGURATION) ;
        tbm_msg2mac.enr = MAC_BM_Unname_Req ;
        zas_send(WSNR_MAC, &tbm_msg2mac) ;
        tbm_state = SMS_W4_UNNAME_CON_GO_MASTER ;
      }
      else
      {
        if (tbm_event == BM_BMI_Remove)
        {
          tbm_state = SMS_W4_STOP_CON_GO_CONFIGURED_VIA_UNNAME ;
        }
        else
        {
          tbm_general_events() ;
        }
      }
      break;
    }
    /*
    SMS_W4_UNNAME_CON_GO_MASTER
    */
    case SMS_W4_UNNAME_CON_GO_MASTER:
    {
      if (tbm_event == BM_MAC_Unname_Con)
      {
        tbm_msg2mac.enr = MAC_BM_Detect_Req ;
        tbm_msg2mac.p1  = tbm_ms ;
        zas_send(WSNR_MAC, &tbm_msg2mac) ;
        *bmi_busy = FALSE ;
        tbm_state = SM_W4_FIRST_NODE ;
      }
      else
      {
        if (tbm_event == BM_BMI_Remove)
        {
          tbm_state = SMS_W4_UNNAME_CON_GO_CONFIGURED ;
        }
        else
        {
          tbm_general_events() ;
        }
      }
      break;
    }
    /*
    slave states
    */
    /*
    SS_INCLUSION
    */
    case SS_INCLUSION :
    {
      if (tbm_event == BM_MAC_Inclusion_Con)
      {
        switch (tbm_inclusion_result = msg->p1)
        {
          /*
          MAC_RESULT_INC_DISRUPT
          */
          case MAC_RESULT_INC_DISRUPT:
          {
            flush_recovery_data();
            p_WTB_status->link_layer_state = LS_READY_TO_BE_NAMED ;
            *bmi_busy = FALSE ;
            bmi_report(LR_SLAVE) ;
            tbm_msg2mac.enr = MAC_BM_Detect_Req ;
            tbm_msg2mac.p1  = SLAVE_STRENGTH ;
            zas_send(WSNR_MAC, &tbm_msg2mac) ;
            tbm_state = SS_W4_NAMING ;
            break;
          }
          /*
          MAC_RESULT_INC_INHIBIT
          */
          case MAC_RESULT_INC_INHIBIT:
          {
            flush_recovery_data();
            p_WTB_status->link_layer_state = LS_INHIBITED ;
            *bmi_busy = FALSE ;
            bmi_report(LR_INHIBITED) ;
            tbm_msg2mac.enr = MAC_BM_Inhibit_Req ;
            zas_send(WSNR_MAC, &tbm_msg2mac) ;
            tbm_state = SS_INHIBITED ;
            break;
          }
          /*
          MAC_RESULT_INC_INCLUDED
          */
          case MAC_RESULT_INC_INCLUDED:
          {
            bm_copy_topography( &tbm_SV.topo); /*4.1-42*/
            p_WTB_status->link_layer_state = LS_REG_OPERATION_SLAVE ;
            dl_enable_pdi
               (
            tbm_configuration.p_traffic_store,
            p_topo_buffer->p_valid_topo -> node_address,
            tbm_configuration.sink_port_count,
            tbm_configuration.source_port_count,tbm_configuration.port_size);
            md_status_indicate (MD_OK) ;
            *bmi_busy = FALSE ;
            bmi_report(LR_INCLUDED) ;
            tbm_state = SS_REGULAR_OPERATION ;
            break;
          }
          default:
          {
            tbm_error() ;
          }
        }
      }
      else
      {
        tbm_general_events() ;
      }
      break;
    }
    /*
    SS_INHIBITED
    */
    case SS_INHIBITED:
    {
      if (tbm_event == BM_MAC_Inhibit_Con)
      {
        tbm_inhibit = msg->p1 ;
        if (tbm_inhibit)
        {
          /*
          request inhibit
          */
          tbm_msg2mac.enr = MAC_BM_Inhibit_Req ;
          zas_send(WSNR_MAC, &tbm_msg2mac ) ;
          /* tbm_state = SS_INHIBITED ; */
        }
        else
        {
          p_WTB_status->link_layer_state = LS_READY_TO_BE_NAMED ;
          bmi_report(LR_ALLOWED) ;
          /*
          start detection
          */
          tbm_msg2mac.enr = MAC_BM_Detect_Req ;
          tbm_msg2mac.p1  = SLAVE_STRENGTH ;
          zas_send(WSNR_MAC, &tbm_msg2mac) ;
          tbm_state = SS_W4_NAMING ;
        }
      }
      else
      {
        if (tbm_event == BM_BMI_Remove)
        {
          /*
          stop MAC
          */
          tbm_msg2mac.enr = MAC_BM_Stop_Req ;
          zas_send(WSNR_MAC, &tbm_msg2mac ) ;
          tbm_state = SMS_W4_STOP_CON_GO_CONFIGURED ;
        }
        else
        {
          tbm_general_events() ;
        }
      }
      break;
    }
    /*
    SS_W4_NAMING
    */
    case SS_W4_NAMING:
    {
      if (tbm_event == BM_MAC_Detect_Con)
      {
        tbm_detection_result = msg->p1 ;
        if (tbm_detection_result == MAC_RESULT_DET_NOTHING)
        {
          /*
          start detection
          */
          tbm_msg2mac.enr = MAC_BM_Detect_Req ;
          tbm_msg2mac.p1  = tbm_ms ;
          zas_send(WSNR_MAC, &tbm_msg2mac) ;
          if (tbm_ms == SLAVE_STRENGTH)
          {
            /* tbm_state = SS_W4_NAMING */
          }
          else
          {
            bmi_report(LR_PROMOTED) ;
            /* promoted to weak  master */
            tbm_state = SM_W4_FIRST_NODE ;
          }
        }
        else
        {
          /*
          start slave
          */
          tbm_msg2mac.enr = MAC_BM_StartSlave_Cmd ;
          zas_send(WSNR_MAC, &tbm_msg2mac) ;
          tbm_state = SS_W4_END_OF_NAMING ;
        }
      }
      else
      {
        if (tbm_event == BM_BMI_Remove)
        {
          /*
          stop MAC
          */
          tbm_msg2mac.enr = MAC_BM_Stop_Req ;
          zas_send(WSNR_MAC, &tbm_msg2mac ) ;
          tbm_state = SMS_W4_STOP_CON_GO_CONFIGURED ;
        }
        else
        {
          tbm_general_events() ;
        }
      }
      break;
    }
    /*
    SS_W4_END_OF_NAMING
    */
    case SS_W4_END_OF_NAMING:
    {
      if (tbm_event == BM_MAC_SlaveStop_Ind)
      {
        /* tbm_slave_reason is ignored */
        /*
        start detection
        */
        tbm_msg2mac.enr = MAC_BM_Detect_Req ;
        tbm_msg2mac.p1  = SLAVE_STRENGTH ;
        zas_send(WSNR_MAC, &tbm_msg2mac) ;
        tbm_state = SS_W4_NAMING ;
      }
      else
      {
        if (tbm_event == BM_MAC_Topo_Ind)
        {
          /*
          save_recovery_data
          */
          tbm_SV = *((Type_SaveData*)(msg->p34)) ;
          save_recovery_data (&tbm_SV) ;
          bm_copy_topography(& tbm_SV.topo); /*4.1-42*/
          p_WTB_status->link_layer_state = LS_REG_OPERATION_SLAVE ;
          dl_enable_pdi
             (
          tbm_configuration.p_traffic_store,
          p_topo_buffer->p_valid_topo -> node_address,
          tbm_configuration.sink_port_count,
          tbm_configuration.source_port_count,tbm_configuration.port_size);
          md_status_indicate (MD_OK) ;
          p_WTB_status->link_layer_state = LS_REG_OPERATION_SLAVE ;
          bmi_report(
#ifdef UMS
                     msg->p1 ? LR_TOPO_FAIL :
#endif
                     LR_NAMED) ;
          tbm_state = SS_REGULAR_OPERATION ;
        }
        else
        {
          if (tbm_event == BM_BMI_Remove)
          {
            /*
            stop MAC
            */
            tbm_msg2mac.enr = MAC_BM_Stop_Req ;
            zas_send(WSNR_MAC, &tbm_msg2mac ) ;
            tbm_state = SMS_W4_STOP_CON_GO_CONFIGURED ;
          }
          else
          {
            tbm_general_events() ;
          }
        }
      }
      break;
    }
    /*
    SS_REGULAR_OPERATION
    */
    case SS_REGULAR_OPERATION:
    {
      switch (tbm_event)
      {
        /*
        messages from MAC while regular operation slave
        */
        /*
        BM_MAC_SlaveStop_Ind
        */
        case BM_MAC_SlaveStop_Ind:
        {
          md_status_indicate (MD_INAUGURATION) ;
          tbm_slave_reason = msg->p1 ;
          flush_recovery_data() ;
          tbm_msg2mac.enr = MAC_BM_Detect_Req ;
          tbm_msg2mac.p1  = SLAVE_STRENGTH ;
          zas_send(WSNR_MAC, &tbm_msg2mac) ;
          p_WTB_status->link_layer_state = LS_READY_TO_BE_NAMED ;
          bmi_report(LR_DISCONNECTION) ;
          tbm_state = SS_W4_NAMING ;
          break;
        }
        /*
        BM_MAC_Topo_Ind
        */
        case BM_MAC_Topo_Ind:
        {
          /*
          save_recovery_data
          */
          tbm_SV = *((Type_SaveData*)(msg->p34)) ;
          save_recovery_data (&tbm_SV) ;
          bm_copy_topography( &tbm_SV.topo);  /*4.1-42*/
          md_status_indicate(MD_OK );   /*4.1-26*/
          bmi_report(
#ifdef UMS
                     msg->p1 ? LR_TOPO_FAIL :
#endif
                     LR_NEW_TOPOGRAPHY) ;
          break;
        }
        /*
        messages from BMI while regular operation slave
        */
        /*
        BM_BMI_Remove
        */
        case BM_BMI_Remove:
        {
          flush_recovery_data() ;
          tbm_msg2mac.enr = MAC_BM_Stop_Req ;
          zas_send(WSNR_MAC, &tbm_msg2mac) ;
          tbm_state = SMS_W4_STOP_CON_GO_CONFIGURED ;
          break;
        }
        /*
        BM_BMI_ChgNodeDesc
        */
        case BM_BMI_ChgNodeDesc:
        {
          tbm_desc = *((Type_NodeDescriptor*)(msg->p34)) ;
          tbm_msg2mac.enr = MAC_BM_ChgNodeDesc_Cmd ;
          tbm_msg2mac.p34  = &tbm_desc ;
          zas_send(WSNR_MAC, &tbm_msg2mac) ;
          break;
        }
        /*
        BM_BMI_ChgUserReport
        */
        case BM_BMI_ChgUserReport:
        {
          tbm_msg2mac.enr = MAC_BM_ChgUserReport_Cmd ;
          tbm_msg2mac.p1  = msg->p1 ;
          tbm_msg2mac.p2  = msg->p2 ;
          zas_send(WSNR_MAC, &tbm_msg2mac) ;
          break;
        }
        default:
        {
          tbm_general_events() ;
        }
      }
      break;
    }
    /*
    master states
    */
    /*
    SM_READY_TO_NAME
    */
    case SM_READY_TO_NAME:
    {
      if (tbm_event == BM_BMI_StartNaming)
      {
        p_WTB_status->link_layer_state = LS_TRANSIENT ;
        *bmi_busy = FALSE ;
        /*
        start detection
        */
        tbm_msg2mac.enr = MAC_BM_Detect_Req ;
        tbm_msg2mac.p1  = tbm_ms ;
        zas_send(WSNR_MAC, &tbm_msg2mac) ;
        tbm_state = SM_W4_FIRST_NODE ;
      }
      else
      {
        if (tbm_event == BM_BMI_Remove)
        {
          p_WTB_status->link_layer_state = LS_CONFIGURED ;
          *bmi_busy = FALSE ;
          bmi_report(LR_REMOVED) ;
          tbm_state = SMS_CONFIGURED ;
        }
        else
        {
          tbm_general_events() ;
        }
      }
      break;
    }
    /*
    SM_W4_FIRST_NODE
    */
    case SM_W4_FIRST_NODE:
    {
      if (tbm_event == BM_MAC_Detect_Con)
      {
        tbm_detection_result = msg->p1 ;
        switch (tbm_detection_result)
        {
          case MAC_RESULT_DET_SMC :
          {
            bmi_report(LR_MASTER_CONFLICT) ;
            /*
            start detection
            */
            tbm_msg2mac.enr = MAC_BM_Detect_Req ;
            tbm_msg2mac.p1  = tbm_ms ;
            zas_send(WSNR_MAC, &tbm_msg2mac) ;
            /* tbm_state = SM_W4_FIRST_NODE ; */
            break;
          }
          case MAC_RESULT_DET_YIELD :
          {
            /*
            start slave
            */
            tbm_msg2mac.enr = MAC_BM_StartSlave_Cmd ;
            zas_send(WSNR_MAC, &tbm_msg2mac) ;
            bmi_report(LR_DEMOTED) ;
            tbm_state = SS_W4_END_OF_NAMING ;
            break;
          }
          case MAC_RESULT_DET_INSIST :
          {
            /*
            name composition
            */
            if (tbm_ms == STRONG_STRENGTH)
            {
              tbm_msg2mac.p1  = STRONG ;
            }
            else
            {
              tbm_msg2mac.p1  = WEAK ;
            }
            tbm_msg2mac.enr = MAC_BM_Name_Req ;
            zas_send(WSNR_MAC, &tbm_msg2mac) ;
            tbm_state = SM_W4_NAME_CON ;
            break;
          }
        }
      }
      else
      {
        if (tbm_event == BM_BMI_Remove)
        {
          /*
          stop MAC
          */
          tbm_msg2mac.enr = MAC_BM_Stop_Req ;
          zas_send(WSNR_MAC, &tbm_msg2mac ) ;
          tbm_state = SMS_W4_STOP_CON_GO_CONFIGURED ;
        }
        else
        {
          tbm_general_events() ;
        }
      }
      break;
    }
    /*
    SM_W4_NAME_CON
    */
    case SM_W4_NAME_CON:
    {
      if (tbm_event == BM_MAC_Name_Con)
      {
        switch (tbm_name_result = msg->p1)
        {
          /*
          MAC_RESULT_NAME_YIELD
          */
          case MAC_RESULT_NAME_YIELD:
          {
            p_WTB_status->link_layer_state = LS_READY_TO_BE_NAMED ;
            bmi_report(LR_DEMOTED) ;
            tbm_msg2mac.enr = MAC_BM_Unname_Req ;
            zas_send(WSNR_MAC, &tbm_msg2mac) ;
            tbm_state = SMS_W4_UNNAME_CON_GO_SLAVE ;
            break;
          }
          /*
          MAC_RESULT_NAME_AGAIN
          */
          case MAC_RESULT_NAME_AGAIN :
          {
            tbm_msg2mac.enr = MAC_BM_Detect_Req ;
            tbm_msg2mac.p1  = tbm_ms ;
            zas_send(WSNR_MAC, &tbm_msg2mac) ;
            tbm_state = SM_W4_FIRST_NODE ;
            break;
          }
          /*
          MAC_RESULT_NAME_OK
          */
          case MAC_RESULT_NAME_OK :
          {
            tbm_msg2mac.enr = MAC_BM_StartMaster_Cmd ;
            zas_send(WSNR_MAC, &tbm_msg2mac) ;
            tbm_state = SM_W4_TOPO_IND ;
            break;
          }
          /*
          MAC_RESULT_NAME_FAILURE
          */
          case MAC_RESULT_NAME_FAILURE:
          {
            tbm_msg2mac.enr = MAC_BM_Unname_Req ;
            zas_send(WSNR_MAC, &tbm_msg2mac) ;
            tbm_state = SM_W4_UNNAME_CON_AFTER_FAILED_NAMING ;
            break;
          }
          default:
          {
            tbm_error() ;
          }
        }
      }
      else
      {
        if (tbm_event == BM_BMI_Remove)
        {
          /*
          stop MAC
          */
          tbm_msg2mac.enr = MAC_BM_Stop_Req ;
          tbm_msg2mac.p1  = tbm_ms ;
          zas_send(WSNR_MAC, &tbm_msg2mac) ;
          tbm_state = SMS_W4_STOP_CON_GO_CONFIGURED_VIA_UNNAME;
        }
        else
        {
          tbm_general_events() ;
        }
      }
      break;
    }
    /*
    SM_W4_TOPO_IND
    */
    case SM_W4_TOPO_IND:
    {
      if (tbm_event == BM_MAC_Topo_Ind)
      {
        bm_copy_topography( &((Type_SaveData*)(msg->p34))->topo);  /*4.1-42*/
        /* ignore SV */
        dl_enable_pdi
           (
        tbm_configuration.p_traffic_store,
        p_topo_buffer->p_valid_topo -> node_address,
        tbm_configuration.sink_port_count,
        tbm_configuration.source_port_count,tbm_configuration.port_size);
        md_status_indicate (MD_OK) ;    /*4.1-22*/
        switch (tbm_ms)
        {
          case WEAK_STRENGTH:
          {
            p_WTB_status->link_layer_state = LS_REG_OPERATION_WEAK ;
            bmi_report(
#ifdef UMS
                       msg->p1 ? LR_TOPO_FAIL :
#endif
                       LR_NAMING_SUCCESSFUL) ;
            tbm_state = SM_REG_OPERATION_WEAK ;
            break;
          }
          case STRONG_STRENGTH:
          {
            p_WTB_status->link_layer_state = LS_REG_OPERATION_STRONG ;
            *bmi_busy = FALSE ;
            bmi_report(
#ifdef UMS
                       msg->p1 ? LR_TOPO_FAIL :
#endif
                       LR_NAMING_SUCCESSFUL) ;
            tbm_state = SM_REG_OPERATION_STRONG ;
            break;
          }
          default:
          {
            tbm_error() ;
          }
        }
        /*md_status_indicate (MD_OK) ;    4.1-22*/
      }
      else
      {
        if (tbm_event == BM_MAC_MasterStop_Ind)
        {
          tbm_master_reason = msg->p1 ;
          /*
          unname composition
          */
          tbm_msg2mac.enr = MAC_BM_Unname_Req ;
          zas_send(WSNR_MAC, &tbm_msg2mac) ;
          tbm_state = SM_W4_UNNAME_CON_AFTER_FAILED_MASTER ;
        }
        else
        {
          if (tbm_event == BM_BMI_Remove)
          {
            /*
            stop MAC
            */
            tbm_msg2mac.enr = MAC_BM_Stop_Req ;
            zas_send(WSNR_MAC, &tbm_msg2mac ) ;
            tbm_state = SMS_W4_STOP_CON_GO_CONFIGURED_VIA_UNNAME;

          }
          else
          {
            tbm_general_events() ;
          }
        }
      }
      break;
    }
    /*
    SM_REG_OPERATION_STRONG
    */
    case SM_REG_OPERATION_STRONG:
    {
      switch (tbm_event)
      {
        /*
        messages from MAC while regular operation strong
        */
        /*
        BM_MAC_MasterEndNodeFail_Ind
        */
        case BM_MAC_MasterEndNodeFail_Ind:
        {
          bmi_report(LR_DISRUPTION) ;
          break;
        }
        /*
        BM_MAC_Topo_Ind
        */
        case BM_MAC_Topo_Ind:
        {
          bm_copy_topography( &(((Type_SaveData*)(msg->p34))->topo) ); /*4.1-42*/
          /* ignore SV */
          md_status_indicate(MD_OK );     /*4.1-26*/
          bmi_report(
#ifdef UMS
                     msg->p1 ? LR_TOPO_FAIL :
#endif
                     LR_NEW_TOPOGRAPHY) ;
          break;
        }
        /*
        BM_MAC_MasterSMC_Ind
        */
        case BM_MAC_MasterSMC_Ind:
        {
          bmi_report(LR_MASTER_CONFLICT) ;
          break;
        }
        /*
        BM_MAC_WTBNodes_Ind
        */
        case BM_MAC_WTBNodes_Ind:
        {
          bmi_report(LR_NODE_STATUS) ;
          break;
        }
        /*
        BM_MAC_MasterLengthening_Ind
        */
        case BM_MAC_MasterLengthening_Ind:
        {
          bmi_report(LR_LENGTHENING) ;
          break;
        }
        /*
        BM_MAC_MasterStop_Ind
        */
        case BM_MAC_MasterStop_Ind:
        {
          md_status_indicate (MD_INAUGURATION) ;
          tbm_master_reason = msg->p1 ;
          /*
          unname composition
          */
          tbm_msg2mac.enr = MAC_BM_Unname_Req ;
          zas_send(WSNR_MAC, &tbm_msg2mac) ;
          tbm_state = SM_W4_UNNAME_CON_AFTER_FAILED_MASTER ;
          break;
        }
        /*
        messages from BMI while regular operation strong
        */
        /*
        BM_BMI_SetWeak
        */
        case BM_BMI_SetWeak:
        {
          tbm_msg2mac.enr = MAC_BM_SetWeak_Cmd ;
          zas_send(WSNR_MAC, &tbm_msg2mac) ;
          tbm_ms = WEAK_STRENGTH ;
          p_WTB_status->link_layer_state = LS_REG_OPERATION_WEAK ;
          *bmi_busy = FALSE ;
          bmi_report(LR_WEAK) ;
          tbm_state = SM_REG_OPERATION_WEAK ;
          break;
        }
        /*
        BM_BMI_ChgNodeDesc
        */
        case BM_BMI_ChgNodeDesc:
        {
          tbm_desc = *((Type_NodeDescriptor*)(msg->p34)) ;
          tbm_msg2mac.enr = MAC_BM_ChgNodeDesc_Cmd ;
          tbm_msg2mac.p34  = &tbm_desc ;
          zas_send(WSNR_MAC, &tbm_msg2mac) ;
          break;
        }
        /*
        BM_BMI_ChgUserReport
        */
        case BM_BMI_ChgUserReport:
        {
          tbm_msg2mac.enr = MAC_BM_ChgUserReport_Cmd ;
          tbm_msg2mac.p1  = msg->p1 ;
          tbm_msg2mac.p2  = msg->p2 ;
          zas_send(WSNR_MAC, &tbm_msg2mac) ;
          break;
        }
        /*
        BM_BMI_StartNaming
        */
        case BM_BMI_StartNaming:
        {
          tbm_msg2mac.enr = MAC_BM_Stop_Req ;
          zas_send(WSNR_MAC, &tbm_msg2mac) ;
          tbm_state = SMS_W4_STOP_CON_GO_MASTER ;
          break;
        }
        /*
        BM_BMI_Remove
        */
        case BM_BMI_Remove:
        {
          tbm_msg2mac.enr = MAC_BM_Stop_Req ;
          zas_send(WSNR_MAC, &tbm_msg2mac) ;
          tbm_state = SMS_W4_STOP_CON_GO_CONFIGURED_VIA_UNNAME ;
          break;
        }
        default:
        {
          tbm_general_events() ;
        }
      }
      break;
    }
    /*
    SM_REG_OPERATION_WEAK
    */
    case SM_REG_OPERATION_WEAK:
    {
      switch (tbm_event)
      {
        /*
        messages from MAC while regular operation weak
        */
        /*
        BM_MAC_MasterEndNodeFail_Ind
        */
        case BM_MAC_MasterEndNodeFail_Ind:
        {
          bmi_report(LR_DISRUPTION) ;
          tbm_msg2mac.enr = MAC_BM_Stop_Req ;
          zas_send(WSNR_MAC, &tbm_msg2mac) ;
          tbm_state = SMS_W4_STOP_CON_GO_MASTER ;
          break;
        }
        /*
        BM_MAC_Topo_Ind
        */
        case BM_MAC_Topo_Ind:
        {
          bm_copy_topography( &(((Type_SaveData*)(msg->p34))->topo) ); /*4.1-42*/
          /* ignore SV */
          md_status_indicate(MD_OK );    /*4.1-26*/
          bmi_report(
#ifdef UMS
                     msg->p1 ? LR_TOPO_FAIL :
#endif
                     LR_NEW_TOPOGRAPHY) ;
          break;
        }
        /*
        BM_MAC_WTBNodes_Ind
        */
        case BM_MAC_WTBNodes_Ind:
        {
          bmi_report(LR_NODE_STATUS) ;
          break;
        }
        /*
        BM_MAC_MasterLengthening_Ind
        */
        case BM_MAC_MasterLengthening_Ind:
        {
          bmi_report(LR_LENGTHENING) ;
          tbm_msg2mac.enr = MAC_BM_Stop_Req ;
          zas_send(WSNR_MAC, &tbm_msg2mac) ;
          tbm_state = SMS_W4_STOP_CON_GO_MASTER ;
          break;
        }
        /*
        BM_MAC_MasterYield_Ind
        */
        case BM_MAC_MasterYield_Ind:
        {
          p_WTB_status->link_layer_state = LS_READY_TO_BE_NAMED ;
          bmi_report(LR_DEMOTED) ;
          tbm_msg2mac.enr = MAC_BM_Stop_Req ;
          zas_send(WSNR_MAC, &tbm_msg2mac) ;
          tbm_state = SMS_W4_STOP_CON_GO_SLAVE ;
          break;
        }
        /*
        BM_MAC_MasterStop_Ind
        */
        case BM_MAC_MasterStop_Ind:
        {
          md_status_indicate (MD_INAUGURATION) ;
          tbm_master_reason = msg->p1 ;
          /*
          unname composition
          */
          tbm_msg2mac.enr = MAC_BM_Unname_Req ;
          zas_send(WSNR_MAC, &tbm_msg2mac) ;
          tbm_state = SM_W4_UNNAME_CON_AFTER_FAILED_MASTER ;
          break;
        }
        /*
        messages from BMI while regular operation weak
        */
        /*
        BM_BMI_SetStrong
        */
        case BM_BMI_SetStrong:
        {
          tbm_msg2mac.enr = MAC_BM_SetStrong_Cmd ;
          zas_send(WSNR_MAC, &tbm_msg2mac) ;
          tbm_ms = STRONG_STRENGTH ;
          p_WTB_status->link_layer_state = LS_REG_OPERATION_STRONG ;
          *bmi_busy = FALSE ;
          bmi_report(LR_STRONG) ;
          tbm_state = SM_REG_OPERATION_STRONG ;
          break;
        }
        /*
        BM_BMI_ChgNodeDesc
        */
        case BM_BMI_ChgNodeDesc:
        {
          tbm_desc = *((Type_NodeDescriptor*)(msg->p34)) ;
          tbm_msg2mac.enr = MAC_BM_ChgNodeDesc_Cmd ;
          tbm_msg2mac.p34  = &tbm_desc ;
          zas_send(WSNR_MAC, &tbm_msg2mac) ;
          break;
        }
        /*
        BM_BMI_ChgUserReport
        */
        case BM_BMI_ChgUserReport:
        {
          tbm_msg2mac.enr = MAC_BM_ChgUserReport_Cmd ;
          tbm_msg2mac.p1  = msg->p1 ;
          tbm_msg2mac.p2  = msg->p2 ;
          zas_send(WSNR_MAC, &tbm_msg2mac) ;
          break;
        }
        /*
        BM_BMI_StartNaming
        */
        case BM_BMI_StartNaming:
        {
          tbm_msg2mac.enr = MAC_BM_Stop_Req ;
          zas_send(WSNR_MAC, &tbm_msg2mac) ;
          tbm_state = SMS_W4_STOP_CON_GO_MASTER ;
          break;
        }
        /*
        BM_BMI_Remove
        */
        case BM_BMI_Remove:
        {
          tbm_msg2mac.enr = MAC_BM_Stop_Req ;
          zas_send(WSNR_MAC, &tbm_msg2mac) ;
          tbm_state = SMS_W4_STOP_CON_GO_CONFIGURED_VIA_UNNAME ;
          break;
        }
        default:
        {
          tbm_general_events() ;
        }
      }
      break;
    }
    /*
    SM_W4_UNNAME_CON_AFTER_FAILED_NAMING
    */
    case SM_W4_UNNAME_CON_AFTER_FAILED_NAMING:
    {
      if (tbm_event == BM_MAC_Unname_Con)
      {
        if (tbm_ms == STRONG_STRENGTH)
        {
          p_WTB_status->link_layer_state = LS_READY_TO_NAME ;
          bmi_report(LR_NAMING_FAILED) ;
          tbm_state = SM_READY_TO_NAME ;
        }
        else
        {
          /*
          start detection
          */
          tbm_msg2mac.enr = MAC_BM_Detect_Req ;
          tbm_msg2mac.p1  = SLAVE_STRENGTH ;
          zas_send(WSNR_MAC, &tbm_msg2mac) ;
          p_WTB_status->link_layer_state = LS_READY_TO_BE_NAMED ;
          bmi_report(LR_NAMING_FAILED) ;
          tbm_state = SS_W4_NAMING ;
        }
      }
      else
      {
        if (tbm_event == BM_BMI_Remove)
        {
          tbm_state = SMS_W4_UNNAME_CON_GO_CONFIGURED ;
        }
        else
        {
          tbm_general_events() ;
        }
      }
      break;
    }
    /*
    SM_W4_UNNAME_CON_AFTER_FAILED_MASTER
    */
    case SM_W4_UNNAME_CON_AFTER_FAILED_MASTER:
    {
      if (tbm_event == BM_MAC_Unname_Con)
      {
        if (tbm_ms == STRONG_STRENGTH)
        {
          p_WTB_status->link_layer_state = LS_READY_TO_NAME ;
          tbm_state = SM_READY_TO_NAME ;
        }
        else
        {
          /*
          start detection
          */
          tbm_msg2mac.enr = MAC_BM_Detect_Req ;
          tbm_msg2mac.p1  = SLAVE_STRENGTH ;
          zas_send(WSNR_MAC, &tbm_msg2mac) ;
          p_WTB_status->link_layer_state = LS_READY_TO_BE_NAMED ;
          tbm_state = SS_W4_NAMING ;
        }
        switch (tbm_master_reason)
        {
          /*
          MAC_MASTER_STOP_FAILURE
          */
          case MAC_MASTER_STOP_FAILURE:
          {
            bmi_report(LR_NAMING_FAILED) ;
            break;
          }
          /*
          MAC_MASTER_STOP_POLLLISTOVF
          */
          case MAC_MASTER_STOP_POLLLISTOVF:
          {
            bmi_report(LR_POLL_LIST_OVF) ;
            break;
          }
          default:
          {
            tbm_error() ;
          }
        }
      }
      else
      {
        if (tbm_event == BM_BMI_Remove)
        {
          tbm_state = SMS_W4_UNNAME_CON_GO_CONFIGURED ;
        }
        else
        {
          tbm_general_events() ;
        }
      }
      break;
    }
    default:
    {
      tbm_error() ;
    }
  }
}
/*
functions for BM
*/
/*
void tbm_init(Type_WTBStatus        * WTB_status,
              Type_WTBNodes         * WTB_nodes,
              Type_TopographyBuffer * topo,
              Type_MACStatistic     * statistic_data,
              unsigned char         * p_bmi_busy)
*/
void tbm_init(Type_WTBStatus        * WTB_status,
              Type_WTBNodes         * WTB_nodes,
              Type_TopographyBuffer * topo,
              Type_MACStatistic     * statistic_data,
              unsigned char         * p_bmi_busy)
{
  bmi_busy = p_bmi_busy ;
  p_topo_buffer = topo ;
  p_WTB_status = WTB_status ;
  zas_subscribe (PNR_BM, ( void (*)(void*) )bm_zas_entry) ;
  tbm_state = SMS_INITIALIZED ;
  tbm_event = NO_EVENT ;
  p_topo_buffer->p_valid_topo = &p_topo_buffer->topo_a;
  p_WTB_status->link_layer_state = LS_INITIALIZED ;
  /*
  intialize trace buffer
  */
#ifdef O_BM_TRACE
  for (tbm_trace_ndx = 0 ; tbm_trace_ndx < 0xFF ; tbm_trace_ndx++)
  {
   tbm_trace[tbm_trace_ndx] = 0x88 ;
  }
  tbm_trace[0xFF] = 0x88 ;
  tbm_trace_ndx = 0x00 ;
#endif
  tbm_msg2mac.pnr = PNR_MAC ;
  tbm_msg2bmi.pnr = PNR_BMI ;
  tbm_msg2bmi.enr = BMI_BM_Report ;
  /* initialize medium access */
  mac_init(WTB_status,
           WTB_nodes,
           statistic_data) ;
}
/*
static void bm_copy_topography()   4.1-42
*/
static void bm_copy_topography( Type_Topography * p_new_topo)
{
  if (p_topo_buffer->p_valid_topo == &p_topo_buffer->topo_a)
  {
    p_topo_buffer->topo_b = *p_new_topo;
    pi_disable() ;
    p_topo_buffer->p_valid_topo = &p_topo_buffer->topo_b ;
    p_topo_buffer->valid_topo_counter = p_new_topo->topo_counter; /*4.1-41*/
    pi_enable() ;
  }
  else
  {
    p_topo_buffer->topo_a = *p_new_topo;
    pi_disable() ;
    p_topo_buffer->p_valid_topo = &p_topo_buffer->topo_a ;
    p_topo_buffer->valid_topo_counter = p_new_topo->topo_counter; /*4.1-41*/
    pi_enable() ;
  }
}
/*
void tbm_error()
*/
void tbm_error(void)
{
  diagwtb_entry(
              DIAG_BM_ERR,
              (unsigned short) tbm_state,
              (unsigned short) tbm_event,
              0x8888,
              (void*)0
            ) ;
  /*4.1-45: workaround: bmi_busy is reset */
  switch (tbm_event)
  {
    case BM_BMI_SetSlave:
    case BM_BMI_SetStrong:
    case BM_BMI_StartNaming:
    case BM_BMI_Remove:
    case BM_BMI_SetSleep:
    case BM_BMI_CancelSleep:
    case BM_BMI_ChgNodeDesc:
    case BM_BMI_Inhibit:
    case BM_BMI_Allow:
    case BM_BMI_ChgUserReport:
    {
      *bmi_busy = FALSE ;
      break;
    }
  }
}
/*
void bmi_report(l_report report)
*/
void bmi_report(l_report report)
{
  /* report is passed as ZAS message to BMI */
  tbm_msg2bmi.p1  = report ;
  zas_send(WSNR_BMI, &tbm_msg2bmi) ;
}
/*
void tbm_general_events (void)
*/
void tbm_general_events (void)
{
  switch (tbm_event)
  {
    /*
    BM_BMI_SetSleep
    */
    case BM_BMI_SetSleep:
    {
      tbm_msg2mac.enr = MAC_BM_Sleep_Cmd ;
      zas_send(WSNR_MAC, &tbm_msg2mac) ;
      break;
    }
    /*
    BM_BMI_CancelSleep
    */
    case BM_BMI_CancelSleep:
    {
      tbm_msg2mac.enr = MAC_BM_CancelSleep_Cmd ;
      zas_send(WSNR_MAC, &tbm_msg2mac) ;
      break;
    }
    /*
    BM_BMI_Inhibit
    */
    case BM_BMI_Inhibit:
    {
      tbm_msg2mac.enr = MAC_BM_Inhibit_Cmd ;
      zas_send(WSNR_MAC, &tbm_msg2mac) ;
      break;
    }
    /*
    BM_BMI_Allow
    */
    case BM_BMI_Allow:
    {
      tbm_msg2mac.enr = MAC_BM_Allow_Cmd ;
      zas_send(WSNR_MAC, &tbm_msg2mac) ;
      break;
    }
    default:
    {
      tbm_error() ;
    }
  }
}
