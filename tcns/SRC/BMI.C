/*
BUSMANAGEMENT INTERFACE WTB V1.9
iMB, Mike Becker
  3. Oktober 1995,  9:09
*/
/*
GENERAL TOP OF FILE
*/
/*
;=========================================================================
;                    copyright 1994 by ABB Henschel
;-------------------------------------------------------------------------
; project   : TRAIN COMMUNICATION NETWORK
;-------------------------------------------------------------------------
; department: AHE/STL3      author: Ch.Leeb, M.Becker    date: 09.12.1994
;-------------------------------------------------------------------------
; filename  : \TCN\MOD\BMI.30\SOURCE\BMI.X
;-------------------------------------------------------------------------
; function  : busmanagement interface for TCN link layer V1.9
;=========================================================================
*/
/*
NOTES
*/

/*
LIST OF CHANGES SINCE DATE OF CREATION
*/
/*
;== LIST OF CHANGES SINCE DATE OF CREATION ===============================
;-------------------------------------------------------------------------

 010195  Le  0.1  created
 300495  Le  0.11 bug fix: the global variable
                  Type_Topography  * temp_p_topo is
                  now defined locally instead in
                  the function ls_t_GetTopography
 300695  Le  0.12 Change User Report now via ZAS
                  BMI/MMI-Reports now via ZAS
 100795  Le  0.13 ls_t_GetStatisticData
 200995  Le  1.0  - pointer to "bmi_busy" passed in "tbm_init"
                  - mmi_zas_entry moved to MMI, new function
                    bmi_md_status_ind
                  - ls_t_SetSleep now legal in LS_READY_TO_NAME and
                    LS_READY_TO_BE_NAMED
 311095 iMB  3.00 ported for ABB 12MX02
 190896  cs  4.0  (10) ls_t_SetSleep now accepted in LS_CONFIGURED
 151096  cs  4.1  (22) O_BMI_PIL_QUEUE
 080197  cs  4.1  (36) BMI_MBX_SIZE increased
 270197  cs  4.1  (41) check for topography consistency
 270197  cs  4.1  (24) support for recovery data in EEPROM
 270197           (44) BMI_ERROR -> DIAG_BMI_ERR now in diag.h
 110297           (48) diag_init() now in ls_t_Init()
*/

#define WTB_VERSION "name=WTB 1.9 Link Layer-V:1.00-D:21.09.95 -C: "
/*
COMPILER SWITCHES
*/
#ifdef O_960
#include "coco.h"
#endif
/*
INCLUDEFILES
*/
#include "tcntypes.h"    /*4.0*/
#include "bmi.h"
#include "wtbdrv.h"
#include "zas.h"
#if defined(O_960) || defined(O_VCU6U_GW)
#include "pi_sys.h"
#else
#include "pil.h"
#endif
#include "mmi.h"
#include "diagwtb.h"
#include "ll.h"
#ifdef UMS
#include "ums.h"
#include "adid.h"
#endif
/*
IMPLEMENTATION
*/
/*
DEFINES
*/
#if defined(UMS) && ! defined(O_BMI_PIL_QUEUE)
#define O_BMI_PIL_QUEUE /* G. Carnevale 98/09/07: force for UMS */
#endif
#define BMI_MAX_REPORT 20
#define MD_OK             0   /* very bad style !! use md_layer.h */
#define MD_INAUGURATION   2
#ifndef O_BMI_PIL_QUEUE       /*4.1-23*/
#define BMI_MBX_EMPTY          0x0000
#define BMI_MBX_DATA_AVAILABLE 0x0001

#define BMI_MBX_DATA_ACCEPTED  0x0002
#define BMI_MBX_OVERFLOW       0x0003

#define BMI_MBX_SIZE               40 /* 0x000A 4.1-36*/
#endif
/*
MODUL GLOBAL VARIABLES
*/
static Type_ZASElement    bmi_order_fld ;
static unsigned char      bmi_busy ;
static Type_Configuration bmi_config ;
#ifndef UMS
static /* G. Carnevale 98/09/07: not static only for UMS */
#endif
void (*Report)(l_report) ;
static void (*ErrorReport)(l_report) ; /* is never used */
#ifndef UMS
static  /* G. Carnevale 98/09/07: not static for UMS */
#endif
void (*lm_status_indicate) (int result);
#ifndef O_BMI_PIL_QUEUE    /*4.1-23*/
static short bmi_mbx[BMI_MBX_SIZE];
static short bmi_mbx_lz ;
static short bmi_mbx_sz ;
#else
#ifndef UMS
static /* G. Carnevale 98/09/07: not static for UMS */
#endif
short bmi_mbx_id;
#endif
static Type_WTBStatus        mac_WTB_status ;
static Type_WTBNodes         mac_WTB_nodes ;
static Type_TopographyBuffer topo ;
static Type_MACStatistic     mac_statistic_data ;
/*
internal prototypes
*/
static void bmi_msg2bm(void) ;
static void bmi_diag_entry(unsigned short negative_result,
                    unsigned short fun_nr) ;
static void bmi_zas_entry (Type_ZASElement * msg);
#ifndef O_BMI_PIL_QUEUE    /*4.1-23*/
static void            bmi_send_queue (unsigned short, unsigned short *) ;
static unsigned short  bmi_receive_queue(unsigned short *) ;
#endif
/*
bmi functions (LS-interface)
*/
void ls_t_Init(void)
{
  /*
  system initialisation of train bus link layer
   (must be called exactly once)
  */
#ifndef O_BMI_PIL_QUEUE   /*4.1-23*/
  unsigned short i;
#else
  short result;
#endif
  diagwtb_sysinit() ;       /* 4.1-48: initialize diagnosis */
#ifndef O_BMI_PIL_QUEUE   /*4.1-23*/
  /* initialize BMI mailbox  */
  bmi_mbx_lz = 0 ;
  bmi_mbx_sz = 1 ;
  for (i=0; i<BMI_MBX_SIZE; i++)
  {
    bmi_mbx[i]= 0x0000 ;
  }
#else
  bmi_mbx_id = pi_create_queue(BMI_MAX_REPORT,   /* entries */
                               PI_ORD_FCFS,      /* message ordering */
                               &result     );
  if (result == PI_RET_OK)
  {

  }
  else
  {
    bmi_diag_entry(DIAG_BMI_ERR, 0) ;
  }
#endif
#ifdef O_NVM_EEPROM /*4.1-24*/
  init_recovery_data();
#endif
}
l_result ls_t_Reset(void)
{
  /*
  initialize train bus link layer an set variables to predefined values
  */
  zas_init() ;           /* initialize ZAS */
  tbm_init(&mac_WTB_status,
           &mac_WTB_nodes,
           &topo,
           &mac_statistic_data,
           &bmi_busy) ;           /* initialize busmanager */
  /*diag_sysinit() ;       4.1-48 initialize diagnosis */
  lm_status_indicate  = (void (*) (int))  0;
  lm_t_system_init() ;   /* messenger <-> MAC interface on messenger module*/
  zas_subscribe (PNR_BMI, ( void(*)(void*) )bmi_zas_entry) ;
  bmi_busy = FALSE ;
  mac_WTB_status.link_layer_state = LS_INITIALIZED ;
  bmi_order_fld.pnr = PNR_BM ;
  return(L_OK) ;
}
l_result ls_t_Configure(Type_Configuration * p_config)
{
  /*
  configure train bus link layer
  */
  pi_disable() ;
  if (bmi_busy)
  {
    pi_enable() ;
    bmi_diag_entry(L_BUSY,FNR_CONFIGURE) ;
    return(L_BUSY) ;
  }
  else
  {
    if (mac_WTB_status.link_layer_state != LS_INITIALIZED)
    {
      pi_enable() ;
      bmi_diag_entry(L_CALLING_SEQUENCE, FNR_CONFIGURE) ;
      return(L_CALLING_SEQUENCE) ;
    }
    else
    {
      bmi_busy = TRUE ;
      pi_enable() ;
    }
  }
  /*
  check if configuration valid
  */
  if ( p_config -> transmission_rate != (BITRATE / 1000))
  {
    bmi_diag_entry(L_CONFIGURATION_INVALID, FNR_CONFIGURE) ;
    bmi_busy = FALSE ;
    return(L_CONFIGURATION_INVALID) ;
  }
  if (p_config -> basic_period != BASIC_CYCLE)
  {
    bmi_diag_entry(L_CONFIGURATION_INVALID, FNR_CONFIGURE) ;
    bmi_busy = FALSE ;
    return(L_CONFIGURATION_INVALID) ;
  }
  if (p_config -> node_descriptor.node_frame_size > D_MAX_DATA_SIZE)
  {
    bmi_diag_entry(L_CONFIGURATION_INVALID, FNR_CONFIGURE) ;
    bmi_busy = FALSE ;
    return(L_CONFIGURATION_INVALID) ;
  }
  if (p_config -> node_descriptor.node_period > MAX_NODE_PERIOD)
  {
    bmi_diag_entry(L_CONFIGURATION_INVALID, FNR_CONFIGURE) ;
    bmi_busy = FALSE ;
    return(L_CONFIGURATION_INVALID) ;
  }
  if (p_config -> poll_md_when_idle) /* not yet supported */
  {
    bmi_diag_entry(L_CONFIGURATION_INVALID, FNR_CONFIGURE) ;
    bmi_busy = FALSE ;
    return(L_CONFIGURATION_INVALID) ;
  }
  if ( (Report=p_config->Report) != (void(*)(l_report))0)
  {

  }
  else
  {
    bmi_diag_entry(L_MISSING_UDF, FNR_CONFIGURE) ;
    bmi_busy = FALSE ;
    return(L_MISSING_UDF) ;
  }
  if ( (ErrorReport=p_config->ErrorReport) != (void(*)(l_report))0)
  {

  }
  else
  {
    bmi_diag_entry(L_MISSING_UDF, FNR_CONFIGURE) ;
    bmi_busy = FALSE ;
    return(L_MISSING_UDF) ;
  }
  bmi_config = *p_config ; /* copy the whole config for MAC */
  bmi_order_fld.enr = BM_BMI_Configure ;
  bmi_order_fld.p34 = &bmi_config ;
  bmi_msg2bm() ;
  return(L_OK) ;
}
l_result ls_t_SetSlave(void)
{
  /*
  set train bus node to slave mode
  */
  pi_disable() ;
  if ( bmi_busy )
  {
    pi_enable() ;
    bmi_diag_entry(L_BUSY,FNR_SET_SLAVE) ;
    return(L_BUSY) ;
  }
  else
  {
    if (mac_WTB_status.link_layer_state != LS_CONFIGURED)
    {
      pi_enable() ;
      bmi_diag_entry(L_CALLING_SEQUENCE,FNR_SET_SLAVE) ;
      return(L_CALLING_SEQUENCE) ;
    }
    else
    {
      bmi_busy = TRUE ;
      pi_enable() ;
    }
  }
  bmi_order_fld.enr = BM_BMI_SetSlave ; /* message number for BM */
  /* no parameters */
  bmi_msg2bm() ;
  return(L_OK) ;
}
l_result ls_t_SetWeak(void)
{
  /*
  set train bus node to weak master mode
  */
  pi_disable() ;
  if ( bmi_busy )
  {
    pi_enable() ;
    bmi_diag_entry(L_BUSY,FNR_SET_WEAK) ;
    return(L_BUSY) ;
  }
  else
  {
    if ((mac_WTB_status.link_layer_state != LS_CONFIGURED)           &&
        (mac_WTB_status.link_layer_state != LS_REG_OPERATION_STRONG)    )
    {
      pi_enable() ;
      bmi_diag_entry(L_CALLING_SEQUENCE,FNR_SET_WEAK) ;
      return(L_CALLING_SEQUENCE) ;
    }
    else
    {
      bmi_busy = TRUE ;
      pi_enable() ;
    }
  }
  bmi_order_fld.enr = BM_BMI_SetWeak ; /* messagenumber for BM */
  /* no parameters */
  bmi_msg2bm() ;
  return(L_OK) ;
}
l_result ls_t_SetStrong(void)
{
  /*
  set train bus node to strong master mode
  */
  pi_disable() ;
  if ( bmi_busy )
  {
    pi_enable() ;
    bmi_diag_entry(L_BUSY,FNR_SET_STRONG) ;
    return(L_BUSY) ;
  }
  else
  {
    if ((mac_WTB_status.link_layer_state != LS_CONFIGURED)         &&
        (mac_WTB_status.link_layer_state != LS_REG_OPERATION_WEAK)    )
    {
      pi_enable() ;
      bmi_diag_entry(L_CALLING_SEQUENCE,FNR_SET_STRONG) ;
      return(L_CALLING_SEQUENCE) ;
    }
    else
    {
      bmi_busy = TRUE ;
      pi_enable() ;
    }
  }
  bmi_order_fld.enr = BM_BMI_SetStrong ; /* messagenumber for BM */
  /* no parameters */
  bmi_msg2bm() ;
  return(L_OK) ;
}
l_result ls_t_StartNaming(void)
{
  /*
  start train bus naming
  */
  pi_disable() ;
  if (bmi_busy )
  {
    pi_enable() ;
    bmi_diag_entry(L_BUSY,FNR_START_NAMING) ;
    return(L_BUSY) ;
  }
  else
  {
    if ( (mac_WTB_status.link_layer_state !=
          LS_READY_TO_NAME)                    &&
         (mac_WTB_status.link_layer_state !=
          LS_REG_OPERATION_WEAK)               &&
         (mac_WTB_status.link_layer_state !=
          LS_REG_OPERATION_STRONG)
       )
    {
      pi_enable() ;
      bmi_diag_entry(L_CALLING_SEQUENCE,FNR_START_NAMING) ;
      return(L_CALLING_SEQUENCE) ;
    }
    else
    {
      bmi_busy = TRUE ;
      pi_enable() ;
    }
  }
  bmi_order_fld.enr = BM_BMI_StartNaming ;
  /* no parameters */
  bmi_msg2bm() ;
  return(L_OK) ;
}
l_result ls_t_Inhibit(void)
{
  /*
  inhibit
  */
  pi_disable() ;
  if (bmi_busy )
  {
    pi_enable() ;
    bmi_diag_entry(L_BUSY,FNR_INHIBIT) ;
    return(L_BUSY) ;
  }
  else
  {
    if ((mac_WTB_status.link_layer_state != LS_REG_OPERATION_SLAVE) &&
        (mac_WTB_status.link_layer_state != LS_REG_OPERATION_WEAK) &&
        (mac_WTB_status.link_layer_state != LS_REG_OPERATION_STRONG))
    {
      pi_enable() ;
      bmi_diag_entry(L_CALLING_SEQUENCE,FNR_INHIBIT) ;
      return(L_CALLING_SEQUENCE) ;
    }
    else
    {
      bmi_busy = TRUE ;
      pi_enable() ;
    }
  }
  bmi_order_fld.enr = BM_BMI_Inhibit ;
  /* no parameters */
  bmi_msg2bm() ;
  bmi_busy = FALSE ;
  return(L_OK) ;
}
l_result ls_t_Allow(void)
{
  /*
  allow
  */
  pi_disable() ;
  if (bmi_busy )
  {
    pi_enable() ;
    bmi_diag_entry(L_BUSY,FNR_ALLOW) ;
    return(L_BUSY) ;
  }
  else
  {
    if ((mac_WTB_status.link_layer_state != LS_REG_OPERATION_SLAVE) &&
        (mac_WTB_status.link_layer_state != LS_REG_OPERATION_WEAK) &&
        (mac_WTB_status.link_layer_state != LS_REG_OPERATION_STRONG))
    {
      pi_enable() ;
      bmi_diag_entry(L_CALLING_SEQUENCE,FNR_ALLOW) ;
      return(L_CALLING_SEQUENCE) ;
    }
    else
    {
      bmi_busy = TRUE ;
      pi_enable() ;
    }
  }
  bmi_order_fld.enr = BM_BMI_Allow ;
  /* no parameters */
  bmi_msg2bm() ;
  bmi_busy = FALSE ;
  return(L_OK) ;
}
l_result ls_t_SetSleep(void)
{
  /*
  sleep request
  */
  pi_disable() ;
  if (bmi_busy )
  {
    pi_enable() ;
    bmi_diag_entry(L_BUSY,FNR_SET_SLEEP) ;
    return(L_BUSY) ;
  }
  else
  {
    if ((mac_WTB_status.link_layer_state != LS_REG_OPERATION_SLAVE)   &&
        (mac_WTB_status.link_layer_state != LS_REG_OPERATION_WEAK)    &&
        (mac_WTB_status.link_layer_state != LS_REG_OPERATION_STRONG)  &&
        (mac_WTB_status.link_layer_state != LS_READY_TO_NAME)         &&
        (mac_WTB_status.link_layer_state != LS_READY_TO_BE_NAMED)     &&
        (mac_WTB_status.link_layer_state != LS_TRANSIENT)     /*4.0-10*/
       )
    {
      pi_enable() ;
      bmi_diag_entry(L_CALLING_SEQUENCE,FNR_SET_SLEEP) ;
      return(L_CALLING_SEQUENCE) ;
    }
    else
    {
      bmi_busy = TRUE ;
      pi_enable() ;
    }
  }
  bmi_order_fld.enr = BM_BMI_SetSleep ;
  /* no parameters */
  bmi_msg2bm() ;
  bmi_busy = FALSE ;
  return(L_OK) ;
}
l_result ls_t_CancelSleep(void)
{
  /*
  cancel sleep request
  */
  pi_disable() ;
  if (bmi_busy)
  {
    pi_enable() ;
    bmi_diag_entry(L_BUSY,FNR_CANCEL_SLEEP) ;
    return(L_BUSY) ;
  }
  else
  {
    if ((mac_WTB_status.link_layer_state != LS_REG_OPERATION_SLAVE) &&
        (mac_WTB_status.link_layer_state != LS_REG_OPERATION_WEAK)  &&
        (mac_WTB_status.link_layer_state != LS_REG_OPERATION_STRONG))
    {
      pi_enable() ;
      bmi_diag_entry(L_CALLING_SEQUENCE,FNR_CANCEL_SLEEP) ;
      return(L_CALLING_SEQUENCE) ;
    }
    else
    {
      bmi_busy = TRUE ;
      pi_enable() ;
    }
  }
  bmi_order_fld.enr = BM_BMI_CancelSleep ;
  /* no parameters */
  bmi_msg2bm() ;
  bmi_busy = FALSE ;
  return(L_OK) ;
}
l_result ls_t_Remove(void)
{
  /*
  switch link layer to state Configured
  */
  pi_disable() ;
  if (bmi_busy)
  {
    pi_enable() ;
    bmi_diag_entry(L_BUSY, FNR_REMOVE) ;
    return(L_BUSY) ;
  }
  else
  {
    if
      (
     (mac_WTB_status.link_layer_state ==
                             LS_INITIALIZED)   ||
     (mac_WTB_status.link_layer_state ==
                             LS_CONFIGURED)
      )
    {
      pi_enable() ;
      bmi_diag_entry(L_CALLING_SEQUENCE,FNR_REMOVE) ;
      return(L_CALLING_SEQUENCE) ;
    }
    else
    {
      bmi_busy = TRUE ;
      pi_enable() ;
    }
  }
  bmi_order_fld.enr = BM_BMI_Remove ;
  /* no parameters */
  bmi_msg2bm() ;
  return(L_OK) ;
}
l_result ls_t_ChgUserReport(unsigned char set_mask,
                            unsigned char clear_mask)
{
  /*
  change user report
  */
  pi_disable() ;
  if (bmi_busy)
  {
    pi_enable() ;
    bmi_diag_entry(L_BUSY,FNR_CHG_USER_REP) ;
    return(L_BUSY) ;
  }
  else
  {
    if ((mac_WTB_status.link_layer_state != LS_REG_OPERATION_SLAVE) &&
        (mac_WTB_status.link_layer_state != LS_REG_OPERATION_WEAK)  &&
        (mac_WTB_status.link_layer_state != LS_REG_OPERATION_STRONG))
    {
      pi_enable() ;
      bmi_diag_entry(L_CALLING_SEQUENCE,FNR_CHG_USER_REP) ;
      return(L_CALLING_SEQUENCE) ;
    }
    else
    {
      bmi_busy = TRUE ;
      pi_enable() ;
    }
  }
  bmi_order_fld.enr = BM_BMI_ChgUserReport ;
  /* fill in parameters */
  bmi_order_fld.p1  = (unsigned short) set_mask ;
  bmi_order_fld.p2  = (unsigned short) clear_mask ;
  bmi_msg2bm() ;
  bmi_busy = FALSE ;
  return(L_OK) ;
}
l_result ls_t_GetStatus(Type_WTBStatus * p_status)
{
  /*
  serve user status request
  */
  /* draw a copy of the MAC status */
  * p_status = mac_WTB_status ;
  return(L_OK) ;
}
l_result ls_t_GetWTBNodes(Type_WTBNodes * p_nodes)
{
  /*
  serve user WTB Nodes request
  */
  if ((mac_WTB_status.link_layer_state != LS_REG_OPERATION_WEAK)   &&
      (mac_WTB_status.link_layer_state != LS_REG_OPERATION_STRONG))
  {
    bmi_diag_entry(L_CALLING_SEQUENCE, FNR_GET_WTB_NODES) ;
    return(L_CALLING_SEQUENCE) ;
  }
  /* draw a copy of the MAC's variable mac_WTB_nodes */
  * p_nodes = mac_WTB_nodes ;
  return(L_OK) ;
}
l_result ls_t_GetTopography(Type_Topography * p_topo)
{
  /*
  serve user topography request
  */
  Type_Topography  * temp_p_topo ;
  UNSIGNED8        temp_topo_counter; /*4.1-41*/
  if ((mac_WTB_status.link_layer_state != LS_REG_OPERATION_SLAVE)  &&
      (mac_WTB_status.link_layer_state != LS_REG_OPERATION_WEAK)   &&
      (mac_WTB_status.link_layer_state != LS_REG_OPERATION_STRONG))
  {
    bmi_diag_entry(L_CALLING_SEQUENCE, FNR_GET_TOPOGRAPHY) ;
    return(L_CALLING_SEQUENCE) ;
  }
  /*
  draw a copy of topo referenced by topo.p_valid_topo
  */
  /* copy topo pointer as critical operation */
  pi_disable() ;
  temp_p_topo       = topo.p_valid_topo;
  temp_topo_counter = topo.valid_topo_counter; /*4.1-41*/
  pi_enable() ;
  /* now we have a save pointer and we can copy the topography */
  * p_topo = * temp_p_topo;
  /*4.1-41*/
  if (temp_topo_counter != topo.valid_topo_counter)
  {
    return(L_BUSY);
  }
#ifdef UMS
  /* access IDTS (only if topo is valid) */
  li_t_read_nk(p_topo);
#endif
  return(L_OK) ;
}
l_result ls_t_ChgNodeDesc(Type_NodeDescriptor descriptor)
{
  /*
  change node descriptor
  */
  pi_disable() ;
  if (bmi_busy)
  {
    pi_enable() ;
    bmi_diag_entry(L_BUSY,FNR_CHG_NODE_DESC) ;
    return(L_BUSY) ;
  }
  else
  {
    if ((mac_WTB_status.link_layer_state != LS_REG_OPERATION_SLAVE) &&
        (mac_WTB_status.link_layer_state != LS_REG_OPERATION_WEAK)  &&
        (mac_WTB_status.link_layer_state != LS_REG_OPERATION_STRONG)   )
    {
      pi_enable() ;
      bmi_diag_entry(L_CALLING_SEQUENCE,FNR_CHG_NODE_DESC) ;
      return(L_CALLING_SEQUENCE) ;
    }
    else
    {
      bmi_busy = TRUE ;
      pi_enable() ;
    }
  }
  bmi_order_fld.enr = BM_BMI_ChgNodeDesc ;
  bmi_order_fld.p34 = & descriptor ;
  bmi_msg2bm() ;
  bmi_busy = FALSE ;
  return(L_OK) ;
}
l_result ls_t_GetStatisticData(Type_LLStatisticData * p_statistic_data)
{
  /*
  get statistic data
  */
  type_StatisticData * p_driver_data ;
  if (d_read_statistic  (&p_driver_data) == D_OK)
  {
    p_statistic_data->A1.NoTxF = p_driver_data->A1.NoTxF;
    p_statistic_data->A1.NoRxF = p_driver_data->A1.NoRxF;
    p_statistic_data->A1.NoFE  = p_driver_data->A1.NoFE;
    p_statistic_data->A1.NoTO  = p_driver_data->A1.NoTO;
    p_statistic_data->A2.NoTxF = p_driver_data->A2.NoTxF;
    p_statistic_data->A2.NoRxF = p_driver_data->A2.NoRxF;
    p_statistic_data->A2.NoFE  = p_driver_data->A2.NoFE;
    p_statistic_data->A2.NoTO  = p_driver_data->A2.NoTO;
    p_statistic_data->B1.NoTxF = p_driver_data->B1.NoTxF;
    p_statistic_data->B1.NoRxF = p_driver_data->B1.NoRxF;
    p_statistic_data->B1.NoFE  = p_driver_data->B1.NoFE;
    p_statistic_data->B1.NoTO  = p_driver_data->B1.NoTO;
    p_statistic_data->B2.NoTxF = p_driver_data->B2.NoTxF;
    p_statistic_data->B2.NoRxF = p_driver_data->B2.NoRxF;
    p_statistic_data->B2.NoFE  = p_driver_data->B2.NoFE;
    p_statistic_data->B2.NoTO  = p_driver_data->B2.NoTO;
    p_statistic_data->LineSwitch = p_driver_data->LineSwitch;
    p_statistic_data->basic_period_cnt = mac_statistic_data.basic_period_cnt;
    p_statistic_data->inaug_cnt = mac_statistic_data.inaug_cnt;
    p_statistic_data->topo_cnt  = mac_statistic_data.topo_cnt;
    p_statistic_data->NoTMD     = mac_statistic_data.NoTMD;
    p_statistic_data->NoRMD     = mac_statistic_data.NoRMD;
  }
  else
  {
    return(L_CALLING_SEQUENCE) ;
  }
  return(L_OK) ;
}
/*
bmi auxiliary functions
*/
void bmi_bind_mmi ( void (*status_indication) (int result) )
{
  lm_status_indicate  = status_indication;
}
void bmi_msg2bm(void)
{
  /*
  send message to busmanager
  */
  zas_send( WSNR_BM, &bmi_order_fld ) ; /* send message */
  zas_activate() ;
}
void bmi_diag_entry(unsigned short negative_result, unsigned short fun_nr)
{
  /*
  record negative return value
  */
  diagwtb_entry(DIAG_BMI_ERR, mac_WTB_status.link_layer_state, negative_result, fun_nr, (void*)0) ;
}
#ifndef O_BMI_PIL_QUEUE        /*4.1-23*/
void bmi_send_queue (unsigned short message, unsigned short * result)
{
  /*
  put a message to the BMI mailbox
  */
  pi_disable() ;
  if (bmi_mbx_sz == bmi_mbx_lz)
  {
    pi_enable();
    *result = BMI_MBX_OVERFLOW ;
  }
  else
  {
    bmi_mbx[bmi_mbx_sz++] = message ;
    bmi_mbx_sz %= BMI_MBX_SIZE ;
    pi_enable() ;
    *result = BMI_MBX_DATA_ACCEPTED ;
  }
  return ;
}
unsigned short  bmi_receive_queue(unsigned short * result)
{
  /*
  get a message from the BMI mailbox
  */
  unsigned short tmp_lz ; /* used as read pointer and return value */
  pi_disable() ;
  tmp_lz = bmi_mbx_lz ;
  tmp_lz++ ;
  tmp_lz %= BMI_MBX_SIZE ;
  if (tmp_lz == bmi_mbx_sz)
  {
    pi_enable() ;
    *result = BMI_MBX_EMPTY ;
  }
  else
  {
    bmi_mbx_lz = tmp_lz ; /* set new read pointer */
    tmp_lz = bmi_mbx[bmi_mbx_lz] ; /* get message */
    pi_enable() ;
    *result = BMI_MBX_DATA_AVAILABLE ;
  }
  return (tmp_lz) ;
}
#endif
/*
void bmi_zas_entry (Type_ZASElement * msg)

#* ZAS call-back (reception) function *#
*/
void bmi_zas_entry (Type_ZASElement * msg)
{
  unsigned short result;
#ifndef O_BMI_PIL_QUEUE   /*4.1-23*/
  bmi_send_queue (msg->p1, &result);
  if (result == BMI_MBX_DATA_ACCEPTED)
  {

  }
  else
  {
    bmi_diag_entry(0xE002,0) ;
  }
#else
  pi_send_queue (bmi_mbx_id, (char *) ((l_report)msg->p1), &result);
  if (result == PI_RET_OK)
  {

  }
  else
  {
    bmi_diag_entry(0xE002,0) ;
  }
#endif
}
/*
void bmi_md_status_ind (unsigned short status)

#* called by upper MMI when it receives a status indication *#
*/
void bmi_md_status_ind (unsigned short status)
{
  unsigned short result;
#ifndef O_BMI_PIL_QUEUE   /*4.1-23*/
  bmi_send_queue (status, &result);
  if (result == BMI_MBX_DATA_ACCEPTED)
  {

  }
  else
  {
    bmi_diag_entry(0xE002,1) ;
  }
#else
  pi_send_queue (bmi_mbx_id, (char *) status, &result);
  if (result == PI_RET_OK)
  {

  }
  else
  {
    bmi_diag_entry(0xE002,0) ;
  }
#endif
}
/*
bmi_report_task

#* set up this function as task in the operating system *#
*/
/*
All reports (user reports to the LS-Interface and status indications to the LM-Interface) are issued via this task. The
task connects the link layer interrupt level with the operating system task level via a PIL mailbox. The reports can
therefore use all kernel operations.
The both ZAS entry functions bmi_zas_entry and mmi_zas_entry post the reports into the BMI mailbox.
The report task runs cyclically, polling for arrived mails. It then sorts out LS-Interface and LM-Interface reports and
calls the appropriate callback function.
*/
#ifdef UMS
void bmi_old_report_task (void)  /* G. Carnevale 98/09/07: rename for UMS */
#else
void bmi_report_task (void)
#endif
{
  unsigned short result;
  unsigned short tmp_report ;
#ifndef O_BMI_PIL_QUEUE        /*4.1-23*/
  while (1)
  {
    tmp_report = bmi_receive_queue(&result);
    if (result == BMI_MBX_DATA_AVAILABLE)
    {
      switch (tmp_report)
      {
        case MD_OK:
        case MD_INAUGURATION:
        {
          /*
          */
          /* function installed ? */
          if (lm_status_indicate != (void (*) (int)) 0)
          {
            lm_status_indicate ((int) tmp_report);
          }
          break;
        }
        default:
        {
          Report (tmp_report) ;
        }
      }
    }
    else
    {
      /* mailbox is empty */
      break ;
    }
  }
#else
  /*4.1-23*/
  while (1)
  {
    tmp_report = (l_report) pi_receive_queue(bmi_mbx_id, PI_FOREVER, &result);
    if (result == PI_RET_OK)
    {
      switch (tmp_report)
      {
        case MD_OK:
        case MD_INAUGURATION:
        {
          /*
          */
          /* function installed ? */
          if (lm_status_indicate != (void (*) (int)) 0)
          {
            lm_status_indicate ((int) tmp_report);
          }
          break;
        }
        default:
        {
          Report (tmp_report) ;
        }
      }
    }
    else
    {
      bmi_diag_entry(0xE001,0) ;
    }
  }
#endif
}
/*
char * wtb_get_version()
*/
char * wtb_get_version()
{
  /* static char * p_wtb_version = WTB_VERSION; */
  return ((char*)0) ;
}
