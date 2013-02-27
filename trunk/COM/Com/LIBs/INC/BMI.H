/*



*/
/*
 * filename : bmi.h 
 * by       : M.Becker AHE/STL3, Ch.Leeb CHCRC C1
 * date     : 11.07.95
 * 
 * bus management interface version 3.0
 *
 * changes: 
 *
 * 4.1    26.09.96  cs        portability changes
 */
#ifndef BMI_H_ /* G. Carnevale 98/09/07: prevent multiple inclusion */
#define BMI_H_

#include "tcntypes.h"         /*4.1*/
#define HARDWARE_STATE_OK      0
#define HARDWARE_STATE_FAILURE 1
#define WTB_HARDWARE_VERSION 1
#define WTB_SOFTWARE_VERSION 1
#define MAX_NODES 62
/* typedefinitions */
typedef unsigned short l_result ;
#define L_OK                    0x0000     /* possible results */
#define L_BUSY                  0x0001
#define L_CALLING_SEQUENCE      0x0002
#define L_MISSING_UDF           0x0003
#define L_CONFIGURATION_INVALID 0x0004
/* 0x0000 and 0x0001 are reserved !!! */
#define LR_CONFIGURED           0x0010
#define LR_STRONG               0x0011
#define LR_SLAVE                0x0012
#define LR_PROMOTED             0x0013
#define LR_NAMING_SUCCESSFUL    0x0014
#define LR_NAMED                0x0015
#define LR_WEAK                 0x0016
#define LR_REMOVED              0x0017
#define LR_DEMOTED              0x0018
#define LR_DISCONNECTION        0x0019
#define LR_INHIBITED            0x001A
#define LR_INCLUDED             0x001B
#define LR_LENGTHENING          0x001C
#define LR_DISRUPTION           0x001D
#define LR_MASTER_CONFLICT      0x001E
#define LR_NAMING_FAILED        0x001F
#define LR_NEW_TOPOGRAPHY       0x0020
#define LR_NODE_STATUS          0x0021
#define LR_POLL_LIST_OVF        0x0022
#define LR_ALLOWED              0x0023
#ifdef UMS
#define LR_TOPO_FAIL           0x0024
#endif
typedef unsigned short l_report ;
#define FNR_RESET               0x0001 /* function numbers for diagnostic */
#define FNR_CONFIGURE           0x0002
#define FNR_SET_SLAVE           0x0003
#define FNR_SET_WEAK            0x0004
#define FNR_SET_STRONG          0x0005
#define FNR_START_NAMING        0x0006
#define FNR_INHIBIT             0x0007
#define FNR_ALLOW               0x0008
#define FNR_SET_SLEEP           0x0009
#define FNR_CANCEL_SLEEP        0x000A
#define FNR_REMOVE              0x000B
#define FNR_GET_TOPOGRAPHY      0x000C
#define FNR_GET_STATUS          0x000D
#define FNR_CHG_NODE_DESC       0x000D
#define FNR_CHG_USER_REP        0x000F
#define FNR_GET_WTB_NODES       0x0010
#define LS_INITIALIZED          0x00   /* possible values of state */ 
#define LS_CONFIGURED           0x01
#define LS_READY_TO_NAME        0x02
#define LS_READY_TO_BE_NAMED    0x03
#define LS_INHIBITED            0x04
#define LS_REG_OPERATION_STRONG 0x05
#define LS_REG_OPERATION_WEAK   0x06
#define LS_REG_OPERATION_SLAVE  0x07
#define LS_TRANSIENT            0xFF  
#define L_UNKNOWN               0    /* possible values of direction */
#define L_SAME                  1
#define L_INVERSE               2
#define L_UNDEFINED             0x00   /* possible values of operational_mode */
#define L_SLAVE                 0x01 
#define L_STRONG                0x02
#define L_WEAK                  0x03
#define SAM_BIT                 0x01
#define SLP_BIT                 0x02
#define DSC_BIT                 0x04
#define INT_BIT                 0x08
#define DB2_BIT                 0x10
#define DB1_BIT                 0x20
#define DA2_BIT                 0x40
#define DA1_BIT                 0x80
#define MAX_NODE_PERIOD         7   /* max.indiv. pollt. = 2^7 * base period */
/* node status */
typedef struct
{
  BITSET8                      node_report;
  BITSET8                      user_report;
}
Type_NodeStatus;
/* node key */
typedef struct
{
  UNSIGNED8                    node_type;
  UNSIGNED8                    node_version;
} 
Type_NodeKey;
/* node descriptor */
typedef struct
{
 UNSIGNED8                     node_frame_size;
 UNSIGNED8                     node_period;
 Type_NodeKey                  node_key;
}
Type_NodeDescriptor;
typedef struct
{
  UNSIGNED8                    WTB_hardware;
  UNSIGNED8                    WTB_software;
  UNSIGNED8                    hardware_state;
  UNSIGNED8                    link_layer_state;
  ENUM8                        net_inhibit;
  UNSIGNED8                    node_address;
  UNSIGNED8                    node_orient;
  UNSIGNED8                    node_strength;
  Type_NodeDescriptor          node_descriptor;
  Type_NodeStatus              node_status;
}
Type_WTBStatus;
typedef Type_NodeKey Type_NodeIx2NodeKeyArray[MAX_NODES] ;
typedef struct
{
  UNSIGNED8                    node_address ;
  UNSIGNED8                    node_orient ;
  UNSIGNED8                    topo_counter ;
  UNSIGNED8                    individual_period ;
  UNSIGNED8                    is_strong ;
  UNSIGNED8                    number_of_nodes ;
  UNSIGNED8                    bottom_address ;
  UNSIGNED8                    top_address ;
  Type_NodeIx2NodeKeyArray     NK ;
}
Type_Topography;
typedef Type_NodeStatus Type_NodeIx2NodeStatusArray [MAX_NODES];
typedef struct
{
  UNSIGNED8                    bus_identifier; 
  UNSIGNED8                    number_of_nodes;
  UNSIGNED8                    bottom_address;
  UNSIGNED8                    top_address; 
  Type_NodeIx2NodeStatusArray  node_status_list;
}
Type_WTBNodes;
typedef struct 
{
  UNSIGNED16                   transmission_rate ;
  UNSIGNED16                   basic_period ;
  UNSIGNED16                   reserved4 ;       
  UNSIGNED16                   fritting_disabled ;  
  Type_NodeDescriptor          node_descriptor;
  UNSIGNED8                    poll_md_when_idle ;
  UNSIGNED16                   reserved7 ;         
  UNSIGNED16                   sink_port_count ;
  UNSIGNED16                   source_port_count ;
  UNSIGNED16                   reserved10 ;
  UNSIGNED16                   reserved11 ;
  UNSIGNED16                   reserved12 ;    
  UNSIGNED16                   port_size ;
  UNSIGNED16                   reserved14 ;
  UNSIGNED16                   reserved15 ;
  void                         *p_traffic_store;
  void                         (*Report)(l_report) ;
  void                         (*ErrorReport)(l_report) ; 
} Type_Configuration ;
typedef struct
{
  UNSIGNED32                   NoTxF; /* counter of transmitted frames */
  UNSIGNED32                   NoRxF; /* counter of frames received without error */
  UNSIGNED16                   NoFE;  /* counter of frames received with error */
  UNSIGNED16                   NoTO;  /* counter of timeouts (ACK mode only) */
}Type_LineStatus;
typedef struct
{
 UNSIGNED32                    basic_period_cnt;
 UNSIGNED8                     inaug_cnt;
 UNSIGNED8                     topo_cnt;
 UNSIGNED16                    reserved0;
 UNSIGNED32                    NoTMD;
 UNSIGNED32                    NoRMD;
 Type_LineStatus               A1;   /* counters for line A in direction 1             */
 Type_LineStatus               A2;   /* counters for line A in direction 1             */
 Type_LineStatus               B1;   /* counters for line A in direction 1             */
 Type_LineStatus               B2;   /* counters for line A in direction 1             */
 UNSIGNED16                    LineSwitch; /* counter switching between redundant lines */
 UNSIGNED16                    reserved1;
 UNSIGNED16                    reserved2;
 UNSIGNED16                    reserved3;
}Type_LLStatisticData;


extern short bmi_mbx_id;

extern void (*lm_status_indicate) (int result);

 /* 
 * train bus control interface 
 */
void ls_t_Init(void) ;
l_result ls_t_Reset(void) ;
l_result ls_t_Configure(Type_Configuration *) ;
l_result ls_t_SetSlave(void) ;
l_result ls_t_SetWeak(void) ;
l_result ls_t_SetStrong(void) ;
l_result ls_t_StartNaming(void) ;
l_result ls_t_Remove(void) ;
l_result ls_t_Inhibit(void) ;
l_result ls_t_Allow(void) ;
l_result ls_t_SetSleep(void) ;
l_result ls_t_CancelSleep(void) ;
l_result ls_t_GetStatus(Type_WTBStatus * p_status) ;
l_result ls_t_GetWTBNodes(Type_WTBNodes * p_nodes) ;
l_result ls_t_GetTopography(Type_Topography * p_topography) ;
l_result ls_t_ChgNodeDesc(Type_NodeDescriptor node_descriptor) ;
l_result ls_t_ChgUserReport(UNSIGNED8  set_mask, UNSIGNED8  clear_mask) ;
l_result ls_t_GetStatisticData (Type_LLStatisticData * p_statistic_data);
/********************************** E O F ************************************/
#endif
