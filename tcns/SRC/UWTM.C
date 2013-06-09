/*************************************************************************
**        Copyright (C) Siemens AG 1997 All Rights Reserved. Confidential
**========================================================================
**
**    PROJECT::    TCN Train Communication Network
                   JDP Joint Development Project

**    MODULE::     UWTM UIC - WTB - Manager

**    WORKFILE::   $Workfile:   UWTM.C  $
**************************************************************************
**    TASK::       Implementation of WTB Manager

**************************************************************************
**    AUTHOR::     Reinwald (Rei)
**    CREATED::    06.11.97
**========================================================================
**    HISTORY::    AUTHOR::   $Author:   HEINDEL_AR  $
                   REVISION:: $Revision:   1.43  $
                   MODTIME::  $Date:   10 Dec 1998 14:00:38  $
      $Log:   O:\PRJ\JDP\FUE\SW\PVCS\ARC\TCN_FW\UIC\SRC\UWTM.Cv_  $
 *
 *    Rev 1.43   10 Dec 1998 14:00:38   HEINDEL_AR
 * call Report() for non O_166
 * do not signal to rtp_task for non O_166
 * do not globber callbacks if already set
 *
 *    Rev 1.42   04 Dec 1998 10:07:54   REINWALD_LU
 * -new kft: call uwtm_report also if PDM is stopped
 * -allow leading requ, if my uic addr is 0 (checked by utbc instead)
 *
 *    Rev 1.40   05 Nov 1998 10:32:48   REINWALD_LU
 * single: start timer not in standby
 *
 *    Rev 1.39   04 Nov 1998 12:12:24   REINWALD_LU
 * single: order of procedure calls changed
 *
 *    Rev 1.38   28 Oct 1998 16:07:42   REINWALD_LU
 * single: suppress events, when already tcn-inaugurated
 *
 *    Rev 1.38   28 Oct 1998 14:48:32   REINWALD_LU
 * stopp single event in queue, if already inaugurated
 *
 *    Rev 1.37   27 Oct 1998 12:27:54   REINWALD_LU
 * single mode: call order after topo change after new wtb inaug changed
 *
 *    Rev 1.36   21 Oct 1998 08:31:16   REINWALD_LU
 * ns_start_names added; d_reset_ts moved to pdm
 *
 *    Rev 1.35   15 Oct 1998 16:19:00   REINWALD_LU
 * show begin and end of UIC-inaug in single to CI
 *
 *    Rev 1.34   14 Oct 1998 13:00:00   REINWALD_LU
 * switch GW_KFT_DEPOT for send_topo...
 * if UIC-ID is zero, do not set leading
 *
 *    Rev 1.33   13 Oct 1998 14:17:00   REINWALD_LU
 * O_STANDBY added
 *
 *    Rev 1.32   12 Oct 1998 08:48:18   REINWALD_LU
 * Single node behaviour changed ,
 * no pdm when uic address 0
 *
 *    Rev 1.31   01 Oct 1998 16:36:44   REINWALD_LU
 * new functions: uwtm_get_topo and uwtm_send_topo_disable
 * Single node behaviour improved
 *
 *    Rev 1.30   11 Sep 1998 09:48:38   REINWALD_LU
 * enable leading for single node
 *
 *    Rev 1.29   28 Aug 1998 13:26:44   HEINDEL_AR
 * li_t_Sync() not only for O_166
 *
 *    Rev 1.28   05 Aug 1998 15:58:16   REINWALD_LU
 * added writig of own TCN address into idts in case of single node
 *
 *    Rev 1.27   04 Aug 1998 09:00:22   REINWALD_LU
 * Msg reply lengthes with fixes consts, shall avoid padding problems with sizeof()
 *
 *    Rev 1.26   29 Jul 1998 17:32:48   REINWALD_LU
 * d_reset_ts inserted
 *
 *    Rev 1.25   14 Jul 1998 11:58:10   REINWALD_LU
 * Master conflict functionality moved to uwtm,
 * event texts shortened
 *
 *    Rev 1.24   26 Jun 1998 13:27:10   REINWALD_LU
 * topo received bit inserted
 *
 *    Rev 1.23   24 Jun 1998 12:25:50   REINWALD_LU
 * added elin report function
 *
 *    Rev 1.22   24 Jun 1998 07:55:42   REINWALD_LU
 * var use changed
 *
 *    Rev 1.21   22 Jun 1998 10:52:58   REINWALD_LU
 * initializing of global variables moved to uwtm_init()
 *
 *    Rev 1.20   10 Jun 1998 12:06:34   REINWALD_LU
 * automat changed in ready_to_name: start naming
 *
 *    Rev 1.19   08 Jun 1998 09:00:04   REINWALD_LU
 * behaviour during inauguration corrected: build invalid NADI
 *
 *    Rev 1.18   29 May 1998 08:06:04   REINWALD_LU
 * single node mode removed
 *
 *    Rev 1.17   27 May 1998 16:23:58   REINWALD_LU
 * call order in single mode changed;
 * topo response improved
 *
 *    Rev 1.16   26 May 1998 13:22:28   REINWALD_LU
 * - leading request corrected,
 * - redundancy handling inserted,
 * - interface cleaned
 *
 *    Rev 1.15   19 May 1998 09:26:10   REINWALD_LU
 * Adaption for non-166 systems
 *
 *    Rev 1.14   18 May 1998 14:42:26   REINWALD_LU
 * preparation for "restricted" mode;
 * more compiler switches
 *
 *    Rev 1.13   14 May 1998 07:52:10   REINWALD_LU
 * call messager immediately after 'MD_OK'
 *
 *    Rev 1.12   12 May 1998 08:18:46   REINWALD_LU
 * topo telegram reworked
 *
 *    Rev 1.11   04 May 1998 09:50:02   REINWALD_LU
 * set end uic inaug bit
 *
 *    Rev 1.10   29 Apr 1998 12:27:30   REINWALD_LU
 * new: report inclusion to utbc, automatic inauguration after leading request
 *
 *    Rev 1.9   28 Apr 1998 09:56:28   REINWALD_LU
 * bugfix: twice set leading request
 *
 *    Rev 1.8   27 Apr 1998 14:02:24   REINWALD_LU
 * bugfixes: trac_attr + PI_FREE
 *
 *    Rev 1.8   27 Apr 1998 14:01:38   REINWALD_LU
 * bugfixes: trac_attr + PI_FREE
 *
 *    Rev 1.7   22 Apr 1998 12:21:28   REINWALD_LU
 * ld_exists type changed
 *
 *    Rev 1.6   22 Apr 1998 11:05:42   REINWALD_LU
 * EX-master handling,
 * automatic inauguration after leading requ.
 *
 *    Rev 1.6   22 Apr 1998 11:02:20   REINWALD_LU
 * automatic inaug. after lead requ for kft,
 * EX-Master handling
 *
 *    Rev 1.5   31 Mar 1998 08:34:06   Reinwald_Lu
 * statistics, single node mode inserted
 *
 *    Rev 1.4   23 Mar 1998 08:37:22   REINWALD_LU
 * masterconflict interface,
 * INCLUSION: PD-frame length 0
 *
 *    Rev 1.3   18 Mar 1998 16:44:40   REINWALD_LU
 * bugfixes, functional enhancement, names changed
 *
 *    Rev 1.0   27 Nov 1997 15:45:12   REINWALD_LU
 * First Release
**************************************************************************/
#include "profile.h"
#include "tcntypes.h"
#include <string.h>

#include "pi_sys.h"
#include "am_sys.h"

#include "bmi.h"
#include "ums.h"
#include "uwtm.h"
#include "adid.h"
#include "utbc.h"
#include "umsconv.h"

#include "wtbdrv.h"

#ifdef O_CTRLIF
#include "ctrlif.h"
#endif

#ifdef O_STANDBY
#include "c165_hw.h"
#endif

#include "utbc.h"
#include "md_layer.h"
#include "recorder.h"
#ifdef PDM
#include "atr_pdm.h"
#endif
#ifdef O_166
#include "rtp_task.h" /* G. Carnevale 98/09/07: Siemens only */
#endif

#ifdef GW_KFT_DEPOT
#include "ns_names.h"
#endif

#define RECORD
#define SIN /* support single node mode */
/*----------------------- global defines ------------------------------------*/

#define MAX_TOPO_COUNT      63 /* see mac.c / maccase.[ch] */

#define SINGLE_NODE_TIMEOUT 5000  /* ms */

#define SINGLE_NODE_STATE    0x00FF /* Must be different from LR_STATES */
#define UWTM_INT_PRIO       241
#define UWTM_CYCLE          64ul

#define SMC_BEGIN 1
#define SMC_END   2
#define UMS_SMC

/*--------------------- global variables ------------------------------------*/

#ifndef O_166
extern void (*Report)(l_report); /* G. Carnevale 98/09/07: call Report function */
#endif

extern UNSIGNED32 uic_e_count;

/* internal modi */
UNSIGNED8          tcn_node_type;
BOOLEAN            redundancy_start;
BOOLEAN            send_topo;
UNSIGNED8          single_node;
UNSIGNED8          main_state; /* idle, regular, restricted to MD or single */
UNSIGNED8          master_conflict;
#ifdef SIN
UNSIGNED8          single_topo;
#endif
BOOLEAN            UIC_started; /* new ums is started */
BOOLEAN            uwtm_inclusion_flag;
int                map_wtb_is_inaugurated;
int                smc_monoflop;

unsigned char      node_state; /* WTB-LL state */

static Type_Topography    u_topo;
static Type_WTBStatus     status;
static Type_NodeDescriptor nd;

UNSIGNED8 cmd_sleep, state_sleep;
UNSIGNED8 cmd_omode;
UNSIGNED8 cmd_inhibit, state_inhibit;
UNSIGNED8 cmd_inaug;

UNSIGNED8 is_leading;
UNSIGNED8 leading_dir;
UNSIGNED8 ld_requ;


UNSIGNED8 my_uic_addr;
UNSIGNED8 uwtm_NADI_state;
BOOLEAN only_new_topo;

UNSIGNED8 trac_attr; /* attributes for traction, if 1 bit (attr.) is set,
                      * the vehicle can make traction */
#ifdef SIN
short sn_tid; /* timeout for single node */
#endif

UWTM_REPORT_TYPE UWTM_Report;

#ifdef O_166
static int eur_status;
#pragma hold(near,0)
#else
short eur_status;
#endif

UNSIGNED8 uwtm_inaug_reason;
UNSIGNED32 uwtm_wtb_inaug_count;
UNSIGNED32 uwtm_uic_inaug_count;
UNSIGNED32 uwtm_inaug_err_count;
UNSIGNED32 uwtm_cmd_inaug_count;

unsigned long SMC_time;

/******************************************************************************
*   name:         uwtm_red_indicate
*   function:     calls utbc_red_indicate and sets own redundancy state
******************************************************************************/
void uwtm_red_indicate(void)
{
  utbc_red_indicate();
  redundancy_start = TRUE;
  uwtm_inaug_reason = UIC_IR_RED;
#ifdef SIN
  if (single_node) {
    pi_enable_timeout(sn_tid,SINGLE_NODE_TIMEOUT);
  }
#endif
}

#ifdef GW_KFT_DEPOT
/******************************************************************************
*   name:         uwtm_send_topo_disable
*   function:
******************************************************************************/
void uwtm_send_topo_disable(void)
{
  ls_t_SendTopoDisable();
  send_topo = FALSE;
}
#endif

/******************************************************************************
*   name:         uwtm_get_topo
*   function:     calls ls_t_gettopo and handles single node
******************************************************************************/
l_result uwtm_get_topo(Type_Topography * topo)
{
  l_result ret;

  if (main_state == UIC_M_SINGLE) {
    topo->node_address = 1;
    topo->node_orient = L_SAME;
    topo->individual_period = 0;
#ifdef SIN
    topo->topo_counter = single_topo;
#else
    topo->topo_counter = 255;
#endif
    topo->is_strong = (tcn_node_type == L_STRONG);
    topo->number_of_nodes = 1;
    topo->bottom_address = 1;
    topo->top_address = 1;
    ret = UIC_OK;
  } else {
    ret = ls_t_GetTopography(topo);
  }

  return ret;
}

/******************************************************************************
*   name:         map_wtb_inaugurated
*   function:     shows inauguration state to the CI
*   return value: TRUE: wtb is inaugurated
******************************************************************************/
int map_wtb_inaugurated(void)
{
    return map_wtb_is_inaugurated;
}

UNSIGNED8 uwtm_is_leading(void)
{
  return is_leading;
}

UNSIGNED8 uwtm_leading_dir(void)
{
  return leading_dir;
}

UNSIGNED8 uwtm_get_state(void)
{
  return main_state;
}

UNSIGNED8 uwtm_my_uic_addr(void)
{
  return my_uic_addr;
}

UNSIGNED8 uwtm_get_nadi_state(void)
{
   return uwtm_NADI_state;
}


int uwtm_master_conflict(void)
{
  return (master_conflict == SMC_BEGIN);
}

/*****************************************************************************
 *
 * interface for WTB LL  to signal begin and end of Strong Master Confict
 *
 *****************************************************************************/
void uwtm_begin_master_conflict(void)
{
  master_conflict = SMC_BEGIN;
  SMC_time = d_tc_get_systime();
}

void uwtm_end_master_conflict(void)
{
  master_conflict |= SMC_END;
}

/*****************************************************************************
 *
 *  internal functions
 *
 *****************************************************************************/


#ifdef SIN
/******************************************************************************
*   name:         uwtm_timeout
*   function:     is called after 5 secs. without inauguration to start
*                 single node mode
******************************************************************************/
void uwtm_timeout(void* dummy)
{
  UNREFERENCED(dummy);
#ifdef O_166
  isr_mbx_send(bmi_mbx_id,
    (char *)(0x10000L | SINGLE_NODE_STATE),
    UWTM_INT_PRIO,
    &eur_status);
#else
  pi_send_queue(bmi_mbx_id,(char *)(0x10000L | SINGLE_NODE_STATE),&eur_status);
#endif
} /* uwtm_timeout */
#endif

/******************************************************************************
*   name:         uwtm_placebo_error_func
*   function:     there must be an error report function connected to WTB-LL
******************************************************************************/
void uwtm_placebo_error_func( l_report ergebnis )
{
  UNREFERENCED(ergebnis);
  RECORD_M_REBOOT("Fatal ERROR - WTB-LL ErrMsg");
}

/******************************************************************************
*   name:         uwtm_placebo_report_func
*   function:     there must be an error report function connected to WTB-LL
******************************************************************************/
void uwtm_placebo_report_func( l_report ergebnis )
{
  UNREFERENCED(ergebnis);
}


/******************************************************************************
*   name:         uwtm_cyclic_handler
*   function:     cyclic look-up for state changes
******************************************************************************/
void uwtm_cyclic_handler(void)
{
  SPLDECL
  /* check inhibit requests */
  pi_lock_task();
  if (cmd_inhibit != state_inhibit) {

    if (cmd_inhibit == UIC_INHIBIT) {
      if (ls_t_Inhibit() == UWTM_OK)
        state_inhibit = UIC_INHIBIT;
    } else {
      if (ls_t_Allow() == UWTM_OK)
        state_inhibit = UIC_ALLOWED;
    }
  } /* sleep */
  pi_unlock_task();

  if ((node_state == LS_REG_OPERATION_SLAVE) ||
      (node_state == LS_REG_OPERATION_WEAK) ||
      (node_state == LS_REG_OPERATION_STRONG) ||
      (main_state == UIC_M_SINGLE)) {


    /* check sleep reuqests */
    pi_lock_task();
    if (cmd_sleep != state_sleep) {

      if (cmd_sleep == UIC_SLEEP) {
        if (ls_t_SetSleep() == UWTM_OK)
          state_sleep = UIC_SLEEP;
      } else {
        if (ls_t_CancelSleep() == UWTM_OK)
          state_sleep = UIC_NO_SLEEP;
      }
    } /* sleep */
    pi_unlock_task();


    /* check inauguation enforce */
    pi_lock_task();
    if (cmd_inaug == UIC_INAUG) {
      if (main_state == UIC_M_SINGLE) {
#ifdef SIN
        map_wtb_is_inaugurated = FALSE;
        pi_send_queue(bmi_mbx_id,(char *)(0x10000L | SINGLE_NODE_STATE),(short *)&eur_status);
        cmd_inaug = UIC_NO_INAUG;
#endif
      } else {
        if (ls_t_ChgNodeDesc(nd) == UWTM_OK) {
          cmd_inaug = UIC_NO_INAUG;
        }
      }
    }
    pi_unlock_task();
  } /* if reg operation */

  /* check omode request - in any state of LL */
  pi_lock_task();
  if (tcn_node_type != cmd_omode) {

    if ((node_state == LS_REG_OPERATION_STRONG) && (cmd_omode == UWTM_WEAK)) {
#ifndef O_STANDBY
      if (ls_t_SetWeak() == UWTM_OK) {
        tcn_node_type = cmd_omode;
      }
#else
      if (hw_standby) {
        ci_defer_setmode(CI_WEAKMODE);
        tcn_node_type = cmd_omode;
      } else {
        if (ls_t_SetWeak() == UWTM_OK) {
          tcn_node_type = cmd_omode;
        }
      }
#endif
    } else if ((node_state == LS_REG_OPERATION_WEAK) && (cmd_omode == UWTM_STRONG)) {
#ifndef O_STANDBY
      if (ls_t_SetStrong() == UWTM_OK) {
        tcn_node_type = cmd_omode;
      }
#else
      if (hw_standby) {
        ci_defer_setmode(CI_STRONGMODE);
        tcn_node_type = cmd_omode;
      } else {
        if (ls_t_SetStrong() == UWTM_OK) {
          tcn_node_type = cmd_omode;
        }
      }
#endif
    } else if (ls_t_Remove() == UWTM_OK) {
      tcn_node_type = cmd_omode;
    }

    pi_unlock_task();
  } /* if chg omode */
#ifdef UMS_SMC


  if (smc_monoflop > 0) {
    smc_monoflop--;
  } else {
    SPLMAC();
    if (master_conflict & SMC_END) {
      master_conflict = 0;
    }
    SPLX();

    if (master_conflict == SMC_BEGIN) {
      SPLMAC();
      if ((d_tc_get_systime() - SMC_time) > 1100) {
        master_conflict = 0;
      }
      SPLX();
    }
    if (master_conflict == SMC_BEGIN) {
      smc_monoflop = 1;
    }
  }
#elif defined O_CTRLIF
  master_conflict = (ci_get_master_conflict() ? UIC_CONFLICT : UIC_NO_CONFLICT);
#else
  master_conflict = UIC_NO_CONFLICT;
#endif

} /* uwtm_cyclic_handler */

/******************************************************************************
*   name:         uwtm_set_omode
*   function:     sets operational mode of WTB LL
******************************************************************************/
void uwtm_set_omode(void)
{
  BOOLEAN ld_exists = FALSE;
  l_result wtb_result;

  cmd_inhibit = UIC_ALLOWED;
  state_inhibit = UIC_ALLOWED; /* reset due to new inauguration */
  main_state = UIC_M_RESTRICTED;
  utbc_build_nadi( &uwtm_NADI_state,&my_uic_addr,&is_leading,
                   &leading_dir,&ld_exists, SET_TEMP_INVALID);
#ifdef PDM
  pdm_control(PDM_STOP,0);
#endif
#ifndef O_STANDBY
  switch (tcn_node_type) {
    case L_SLAVE     :
      wtb_result = ls_t_SetSlave();
      break;
    case L_STRONG    :
      wtb_result = ls_t_SetStrong();
      break;
    case L_UNDEFINED : /* no break */
    case L_WEAK      : /* no break */
    default          :
      wtb_result = ls_t_SetWeak();
      break;
  } /* Ende swich */
#else
  switch (tcn_node_type) {
    case L_SLAVE     :
      if (hw_standby) {
        ci_defer_setmode(CI_SLAVEMODE);
      } else {
        wtb_result = ls_t_SetSlave();
      }
      break;
    case L_STRONG    :
      if (hw_standby) {
        ci_defer_setmode(CI_STRONGMODE);
      } else {
        wtb_result = ls_t_SetStrong();
      }
      break;
    case L_UNDEFINED : /* no break */
    case L_WEAK      : /* no break */
    default          :
      if (hw_standby) {
        ci_defer_setmode(CI_WEAKMODE);
      } else {
        wtb_result = ls_t_SetWeak();
      }
      break;
  } /* Ende swich */
#endif

} /* uwtm_set_omode */

/******************************************************************************
*   name:         uwtm_start_node
*   function:     calls build_nadi and starts pdm
******************************************************************************/
void uwtm_start_node(void)
{
#ifdef PDM
  UNSIGNED16 u_pdm_mode;
#endif
  BOOLEAN ld_exists = FALSE;
  if ((uwtm_get_topo(&u_topo) == L_OK) &&
      (ls_t_GetStatus(&status) == L_OK) ) {
    main_state = UIC_M_RESTRICTED; /* end single node mode */
    main_state = (utbc_build_nadi(
                    &uwtm_NADI_state,
                    &my_uic_addr,
                    &is_leading,
                    &leading_dir,
                    &ld_exists,
                    (uwtm_inclusion_flag ? NODE_REINSERTION : 0)|
                    (only_new_topo ? NEW_TOPO_RCVD : 0))
      == UWTM_OK) ? UIC_M_REGULAR : UIC_M_RESTRICTED;
    single_topo = u_topo.topo_counter;
  }
#ifdef PDM
  if ((main_state == UIC_M_REGULAR) && (my_uic_addr != 0)) {
#ifdef O_CTRLIF
    u_pdm_mode = ci_get_pdm_mode() & 0x000f; /* only low nibble is significant */
#else
    u_pdm_mode = 0x000f;
#endif
    if (u_pdm_mode == 0x000f) {

      /* == 000f means, UWTM controls the marshalling mode:
       * LEADING, LED or WAGGON */
      UWTM_Report(PDM_START,(is_leading == UIC_LEADING) ? PDM_LEADING :
        (trac_attr ? PDM_LED : PDM_WAGGON));

    } else {

      /* != 000f means, CI (= application) controls pdm_mode.
       * */

      if (u_pdm_mode == PDM_LEADING) {
        /* CI (= Appl) wants to be the one and only leading veh. */

        /* look for the leading veh. If it is not myself, ignore the PDM_LEADING */
        if ((ld_exists) && !(is_leading)) {
          UWTM_Report(PDM_START, (trac_attr ? PDM_LED : PDM_WAGGON));

        } else { /* no leading veh or I am leading */
          UWTM_Report(PDM_START,PDM_LEADING);
        }
      } else {
        /* no leading veh., does not matter which mode */
        UWTM_Report(PDM_START,u_pdm_mode);
      }
    }
  } else {
    UWTM_Report(PDM_STOP,0);
  }
#endif
#ifdef O_CTRLIF
  ci_end_uic_inauguration(); /* begin is controlled by WTB-LL */
#endif

} /* uwtm_start_node */

/******************************************************************************
*   name:         uwtm_stop_node
*   function:     stops pdm and resets flags
******************************************************************************/
void uwtm_stop_node(void)
{
  BOOLEAN ld_exists = FALSE;

  cmd_inhibit = UIC_ALLOWED;
  state_inhibit = UIC_ALLOWED;
  my_uic_addr = 0;
  main_state = UIC_M_RESTRICTED;
  utbc_build_nadi( &uwtm_NADI_state,&my_uic_addr,&is_leading,
                   &leading_dir,&ld_exists, SET_TEMP_INVALID);
#ifdef PDM
  pdm_control(PDM_STOP,0);
#endif

} /* uwtm_stop_node */

#if 0
/******************************************************************************
*   name:         uwtm_start_restricted
*   function:     calls stops pdm and sets restricted mode
******************************************************************************/
void uwtm_start_restricted(void)
{
  BOOLEAN ld_exists = FALSE;

  main_state = UIC_M_RESTRICTED;
  utbc_build_nadi( &uwtm_NADI_state,&my_uic_addr,&is_leading,
                   &leading_dir,&ld_exists, 0);

#ifdef PDM
  UWTM_Report(PDM_STOP,0);
#endif

#ifdef O_CTRLIF
  ci_end_uic_inauguration(); /* begin is controlled by WTB-LL */
#endif
} /* uwtm_start_restricted */
#else
#define uwtm_start_restricted  uwtm_start_node
#endif

/******************************************************************************
*   name:         uwtm_task
*   function:     cyclic handler for WTB state control, includes bmi_report_task:
   All reports (user reports to the LS-Interface and status indications
   to the LM-Interface) are issued via this task. The task connects the link
   layer interrupt level with the operating system task level via a PIL mailbox.
   The reports can therefore use all kernel operations.

   The both ZAS entry functions bmi_zas_entry and mmi_zas_entry post the
   reports into the PIL mailbox.

   The report task runs event-driven whenever mail arrives. It then sorts
   out LS-Interface and LM-Interface reports and calls the appropriate callback
   function.
******************************************************************************/
void uwtm_task(void)
{
  static BOOLEAN is_weak;
  short result;
  l_result wtb_result;
  l_report report;
#ifdef RECORD
  char txt[32];
#endif
#ifdef SIN
  BOOLEAN ld_exists = FALSE;
#endif

  while (1) {
    only_new_topo = FALSE;
    report = (l_report)(unsigned long)pi_receive_queue(bmi_mbx_id,
      UWTM_CYCLE,&result);
#ifdef O_STANDBY
    {
#endif
      if (result == PI_RET_OK) {
        /* Msg received */

#ifdef O_ELIN /* ELIN report function */
        ll_event(report);
#endif

#ifndef O_166
        Report(report); /* G. Carnevale 98/09/07: call Report function */
#endif
        switch (report) {
#ifdef SIN
          case SINGLE_NODE_STATE:
#ifdef O_STANDBY
            if ((!hw_standby) && (!map_wtb_is_inaugurated)) {
#endif
              pi_disable_timeout(sn_tid);
#ifdef O_CTRLIF
              ci_begin_uic_inauguration();
#endif
              map_wtb_is_inaugurated = FALSE;
              main_state = UIC_M_SINGLE;
              if (--single_topo < 1) {
                single_topo = MAX_TOPO_COUNT;
              }
              RECORD_M_EVENT("Single node mode started");
              RECORD_M_EVENT("MD_OK in Single");
              if (send_topo) {
                li_t_ChgInaugDataSD(0x81); /* direction same, own address 1 as single */
                li_t_Sync();
                utbc_build_nadi(&uwtm_NADI_state,&my_uic_addr,&is_leading,
                 &leading_dir,&ld_exists,SINGLE_NODE_MODE);
#ifdef PDM
                UWTM_Report(PDM_START,(is_leading == UIC_LEADING) ? PDM_LEADING :
                  (trac_attr ? PDM_LED : PDM_WAGGON));
              }else {
                UWTM_Report(PDM_STOP,0);
#endif /* PDM */
              }
              /* function installed ? */
              if (lm_status_indicate != (void (*) (int)) 0) {
                short pi_err;
                lm_status_indicate ((int) MD_OK);
#ifdef O_166 /* G. Carnevale 98/12/17: Siemens only */
                pi_send_queue(rtp_mbx_id,(char *)&pi_err,&pi_err);
#endif
                /* activate tm_messenger immediately, because abort_TB_conn() is set !0 */
                /* all tasks which want to send messages have to wait until the messanger */
               /* is ready with this run and has called abortTB_conn() !!! */
              }
              map_wtb_is_inaugurated = TRUE;
#ifdef O_CTRLIF
              ci_end_uic_inauguration();
#endif
              break;
#ifdef O_STANDBY
            } else {
              break;
            }
#endif /* O_STANDBY */
#endif /* SIN */
          case MD_OK:  /* nobreak */
          case MD_INAUGURATION:
            main_state = UIC_M_RESTRICTED;
            RECORD_M_EVENT(report == MD_OK ? "MD_OK" : "MD_INAUGURATION");
            /* function installed ? */
            if (lm_status_indicate != (void (*) (int)) 0) {
#ifdef O_166 /* G. Carnevale 98/09/07: Siemens only */
              short pi_err;
#endif
              lm_status_indicate ((int) report);
#ifdef O_166 /* G. Carnevale 98/09/07: Siemens only */
              pi_send_queue(rtp_mbx_id,(char *)&pi_err,&pi_err);
#endif
              /* activate tm_messenger immediately, because abort_TB_conn() is set !0 */
              /* all tasks which want to send messages have to wait until the messanger */
              /* is ready with this run and has called abortTB_conn() !!! */
            }
            break;

          default:

#ifdef RECORD
            switch (node_state) {
              case LS_INITIALIZED:          strcpy(txt,"S: INITIALIZED "); break;
              case LS_CONFIGURED:           strcpy(txt,"S: CONFIGURED  "); break;
              case LS_READY_TO_NAME:        strcpy(txt,"S: R_TO_NAME   "); break;
              case LS_READY_TO_BE_NAMED:    strcpy(txt,"S: R_TOBE_NAMED"); break;
              case LS_REG_OPERATION_SLAVE:  strcpy(txt,"S: REG_OP_SLAVE"); break;
              case LS_REG_OPERATION_WEAK:   strcpy(txt,"S: REG_OP_WEAK "); break;
              case LS_REG_OPERATION_STRONG: strcpy(txt,"S: REGOP_STRONG"); break;
              case LS_INHIBITED:            strcpy(txt,"S: INHIBITED   "); break;
              default:                      strcpy(txt,"S: WRONG       "); break;
            }

            switch (report) {
              case LR_CONFIGURED:          strcat(txt,"R: CONFIGURED  "); break;
              case LR_STRONG:              strcat(txt,"R: STRONG      "); break;
              case LR_SLAVE:               strcat(txt,"R: SLAVE       "); break;
              case LR_WEAK:                strcat(txt,"R: WEAK        "); break;
              case LR_PROMOTED:            strcat(txt,"R: PROMOTED    "); break;
              case LR_NAMING_SUCCESSFUL:   strcat(txt,"R: NAME_SUCCESS"); break;
              case LR_NAMED:               strcat(txt,"R: NAMED       "); break;
              case LR_TOPO_FAIL:           strcat(txt,"R: TOPO_FAIL   "); break;
              case LR_REMOVED:             strcat(txt,"R: REMOVED     "); break;
              case LR_DEMOTED:             strcat(txt,"R: DEMOTED     "); break;
              case LR_DISCONNECTION:       strcat(txt,"R: DISCONNECT. "); break;
              case LR_INHIBITED:           strcat(txt,"R: INHIBITED   "); break;
              case LR_LENGTHENING:         strcat(txt,"R: LENGTHENING "); break;
              case LR_INCLUDED:            strcat(txt,"R: INCLUDED    "); break;
              case LR_DISRUPTION:          strcat(txt,"R: DISRUPTION  "); break;
              case LR_MASTER_CONFLICT:     strcat(txt,"R: S_M_CONFLICT"); break;
              case LR_NAMING_FAILED:       strcat(txt,"R: NAMING_FAIL "); break;
              case LR_NEW_TOPOGRAPHY:      strcat(txt,"R: NEW_TOPO    "); break;
              case LR_POLL_LIST_OVF:       strcat(txt,"R: POLLLIST_OVF"); break;
              case LR_ALLOWED:             strcat(txt,"R: ALLOWED     "); break;
              case LR_NODE_STATUS:         strcat(txt,"R: NODE_STATE  "); break;
              default:                     strcat(txt,"R: WRONG       "); break;
            }
            RECORD_M_EVENT( txt );
#endif

            switch (report) {
              case LR_CONFIGURED:
              case LR_REMOVED:
              case LR_DISCONNECTION:
              case LR_NAMING_FAILED:
              case LR_DEMOTED:
              case LR_DISRUPTION:
              case LR_LENGTHENING:
#ifdef SIN
                if (single_node
#ifdef O_STANDBY
                && !hw_standby
#endif
                )
                  pi_enable_timeout(sn_tid,SINGLE_NODE_TIMEOUT);
#endif
                map_wtb_is_inaugurated = FALSE;
                break;

              case LR_TOPO_FAIL:
                uwtm_inaug_err_count++;
                /* NOBREAK */
              case LR_NAMED:
              case LR_NAMING_SUCCESSFUL:
              case LR_INCLUDED:
                map_wtb_is_inaugurated = TRUE;
#ifdef SIN
                if (single_node)
                  pi_disable_timeout(sn_tid);
#endif
                uwtm_wtb_inaug_count++;
                uwtm_uic_inaug_count++;
                break;

              case LR_NEW_TOPOGRAPHY:
                uwtm_uic_inaug_count++;
                /* NOBREAK */
              case LR_STRONG:
              case LR_SLAVE:
              case LR_PROMOTED:
              case LR_WEAK:
              case LR_INHIBITED:
              case LR_MASTER_CONFLICT:
              case LR_NODE_STATUS:
              case LR_POLL_LIST_OVF:
              case LR_ALLOWED:
              default:
                /* state unchanged */
                break;
            } /* Ende switch report */

            switch (node_state) {
/**************************************/
              case LS_INITIALIZED:
                if (report == LR_CONFIGURED) {
                  node_state = LS_CONFIGURED ;
                  uwtm_set_omode();
                }
                break;
/**************************************/
              case LS_CONFIGURED:
                switch (report) {
                  /*
                   * strong, slave, inhibited, included
                   */
                  case LR_STRONG:
                    node_state = LS_READY_TO_NAME ;
                    ls_t_StartNaming() ;
                    break;

                  case LR_SLAVE:
                    node_state = LS_READY_TO_BE_NAMED ;
                    is_weak = FALSE;
                    break;

                  case LR_INHIBITED:
                    node_state = LS_INHIBITED ;
                    break;

                  case LR_INCLUDED:
#ifdef PDM
                    pdm_control(PDM_STOP,0);
#endif
                    ls_t_ChgNodeDesc(nd);
                    uwtm_inclusion_flag = TRUE;
                    node_state = LS_REG_OPERATION_SLAVE ;
                    break;

                  case LR_REMOVED:
                    /* just in case */
                    break;

                  default:
                    break;

                } /* Ende switch report */
                break;

/*****************************************/
              case LS_READY_TO_NAME:
                switch (report) {
                  case LR_TOPO_FAIL:
                    uwtm_start_restricted();
                    break;

                  case LR_NAMING_SUCCESSFUL:
                    node_state = LS_REG_OPERATION_STRONG ;
                    uwtm_start_node();
                    break;

                  case LR_NAMING_FAILED:
                    my_uic_addr = 0;
#ifdef PDM
                    pdm_control(PDM_STOP,0);
#endif
                    result = ls_t_StartNaming();
                    break;

                  case LR_DISRUPTION:
                    uwtm_inaug_reason = UIC_IR_DISCON;
                    result = ls_t_StartNaming();
                    break;

                  case LR_LENGTHENING:
                    uwtm_inaug_reason = UIC_IR_LENGTH;
                    result = ls_t_StartNaming();
                    break;

                  case LR_MASTER_CONFLICT:
                    break;

                  case LR_REMOVED:
                    node_state = LS_CONFIGURED;
                    uwtm_set_omode();
                    break;

                  default:
                    break;
                }
                break;

/****************************************/
              case LS_READY_TO_BE_NAMED:
                switch (report) {
                  case LR_TOPO_FAIL:
                    node_state = (is_weak) ? LS_REG_OPERATION_WEAK : LS_REG_OPERATION_SLAVE;
                    uwtm_start_restricted();
                    break;

                  case LR_NAMED:
                    node_state = LS_REG_OPERATION_SLAVE;
                    uwtm_start_node();
                    break;

                  case LR_PROMOTED:
                    /* weak master node starts naming, this needs a certain time */
                    is_weak = TRUE;
                    break;

                  case LR_NAMING_SUCCESSFUL:
                    node_state = LS_REG_OPERATION_WEAK ;
                    uwtm_start_node();
                    break;

                  case LR_DEMOTED:
                    /* weak master detected a stronger master while naming */
                    is_weak = FALSE;
                    break;

                  case LR_NAMING_FAILED:
                    break;

                  case LR_REMOVED:
                    node_state = LS_CONFIGURED;
                    uwtm_set_omode();
                    break;

                  default:
                    break;
                }
                break;
/****************************************/
              case LS_REG_OPERATION_SLAVE:
                 switch (report) {
                  case LR_TOPO_FAIL:
                    only_new_topo = TRUE;
                    uwtm_start_restricted();
                    break;
                  case LR_NEW_TOPOGRAPHY:
                    only_new_topo = TRUE;
                    uwtm_start_node();
                    break;

                  case LR_DISCONNECTION: /* nobreak */
                    uwtm_inaug_reason = UIC_IR_DISCON;
                  case LR_NAMING_FAILED:
                    uwtm_inclusion_flag = FALSE;
                    uwtm_stop_node();
                    node_state = LS_READY_TO_BE_NAMED ;
                    break;

                  case LR_REMOVED:
                    uwtm_inclusion_flag = FALSE;
                    cmd_inhibit = UIC_ALLOWED;
                    state_inhibit = UIC_ALLOWED;
                    node_state = LS_CONFIGURED;
                    uwtm_set_omode();
                    break;

                  default:
                    break;
                }
                break;
/******************************************/
              case LS_REG_OPERATION_WEAK:
                switch (report) {
                  case LR_TOPO_FAIL:
                    only_new_topo = TRUE;
                    uwtm_start_restricted();
                    break;

                  case LR_NEW_TOPOGRAPHY:
                    only_new_topo = TRUE;
                    uwtm_start_node();
                    break;

                  case LR_NAMING_FAILED:
                  case LR_LENGTHENING:
                    uwtm_inaug_reason = UIC_IR_LENGTH;
                    uwtm_stop_node();
                    node_state = LS_READY_TO_BE_NAMED ;
                    break;

                  case LR_DISRUPTION:
                    uwtm_inaug_reason = UIC_IR_DISCON;
                  case LR_DEMOTED:
                    uwtm_stop_node();
                    node_state = LS_READY_TO_BE_NAMED ;
                    break;
#if 0
                  case LR_NAMING_SUCCESSFUL:
                    uwtm_start_node();
                    break;
#endif
                  case LR_NODE_STATUS:
                    break;

                  case LR_STRONG:
                    node_state = LS_REG_OPERATION_STRONG;
                    break;

                  case LR_REMOVED:
                    cmd_inhibit = UIC_ALLOWED;
                    state_inhibit = UIC_ALLOWED;
                    node_state = LS_CONFIGURED;
                    uwtm_set_omode();
                    break;

                  default:
                    break;
                }
                break;
/*********************************************/
              case LS_REG_OPERATION_STRONG:
                switch (report) {
                  case LR_TOPO_FAIL:
                    only_new_topo = TRUE;
                    uwtm_start_restricted();
                    break;

                  case LR_NEW_TOPOGRAPHY:
                    only_new_topo = TRUE;
                    uwtm_start_node();
                    break;

                  case LR_MASTER_CONFLICT:
                    break;
#if 0
                  case LR_NAMING_SUCCESSFUL:
                    uwtm_start_node();
                    break;
#endif
                  case LR_NODE_STATUS:
                    break;

                  case LR_LENGTHENING:
                    uwtm_inaug_reason = UIC_IR_LENGTH;
                    uwtm_stop_node();
                    /* AH: new state due to WTB link layer state diagram (?) */
                    node_state = LS_READY_TO_NAME;
                    wtb_result = ls_t_StartNaming(); /* later with timeout counter */
                    break;

                  case LR_DISRUPTION:
                    uwtm_inaug_reason = UIC_IR_DISCON;
                  case LR_NAMING_FAILED:
                    uwtm_stop_node();
                    /* AH: new state due to WTB link layer state diagram (?) */
                    node_state = LS_READY_TO_NAME;
                    wtb_result = ls_t_StartNaming(); /* later with timeout counter */
                    break;

                  case LR_WEAK:
                    node_state = LS_REG_OPERATION_WEAK;
                    break;

                  case LR_REMOVED:
                    cmd_inhibit = UIC_ALLOWED;
                    state_inhibit = UIC_ALLOWED;
                    node_state = LS_CONFIGURED;
                    uwtm_set_omode();
                    break;

                  default:
                    break;
                }
                break;
/*****************************************/
              case LS_INHIBITED:
                if (report == LR_ALLOWED) {
                  node_state = LS_READY_TO_BE_NAMED ;
                } else if (report == LR_REMOVED) {
                  node_state = LS_CONFIGURED;
                }
                break;

              default:
                break;
            }

            break;
        } /* Ende switch report */
      } /* msg reception */

      uwtm_cyclic_handler();

#ifdef O_STANDBY
    }
#endif
  } /* Ende while */
} /* uwtm_task */

void bmi_report_task(void)
{

  if (UIC_started) {
    uwtm_task();
  } else { /* no UIC, but old MAP */
    bmi_old_report_task();
  } /* Ende if UIC */
} /* Ende bmi_report_task */


/*****************************************************************************
 *
 *  public functions
 *
 *****************************************************************************/
#ifdef O_166
/******************************************************************************
*   name:         uwtm_stop_old_map
*   function:     stops old MAP-task if UMS is started
******************************************************************************/
void uwtm_stop_old_map(void)
{
  if (UIC_started) {

    tsk_sleep(PI_FOREVER);
  }
} /* uwtm_stop_old_map */
#endif
/******************************************************************************
*   name:         uwtm_init
*   function:     initializes the internal variables of uwtm
******************************************************************************/
void uwtm_init(void)
{
  uwtm_inclusion_flag = FALSE;
  main_state = UIC_M_IDLE;
#ifdef UMS_SMC
  master_conflict = 0;
#else
  master_conflict = UIC_NO_CONFLICT;
#endif
  node_state = LS_INITIALIZED;

  cmd_sleep = UIC_NO_SLEEP;
  state_sleep = UIC_NO_SLEEP;

  cmd_omode = UWTM_PASSIVE;

  cmd_inhibit = UIC_ALLOWED;
  state_inhibit = UIC_ALLOWED;

  is_leading = UIC_NOT_LEADING;
  leading_dir = UIC_DIR1;
  ld_requ = UIC_NOT_LEADING;

  cmd_inaug = UIC_NO_INAUG;
  only_new_topo = FALSE;

  my_uic_addr = 0;
  uwtm_NADI_state = UIC_NADI_STATE_INVALID;
  trac_attr = 0; /* no traction */
  uwtm_inaug_reason = UIC_IR_UNKNOWN;
  uwtm_wtb_inaug_count = 0;
  uwtm_uic_inaug_count = 0;
  uwtm_inaug_err_count = 0;
  uwtm_cmd_inaug_count = 0;

  map_wtb_is_inaugurated = FALSE;
  redundancy_start = FALSE;
  UIC_started = FALSE;
  send_topo = TRUE;
  smc_monoflop = 0;
  SMC_time = 0;
  tcn_node_type = 0;
} /* uwtm_init */

/******************************************************************************
*   name:         uwtm_configure
*   function:     init's the WTB LL
*   used globals:
*   return value: ALL_OK or error code
******************************************************************************/

#define TRAC_CONTROL 20
#define TRAC_MASK    0x06  /* Filter traction modes */

int uwtm_configure(UWTM_NODE_CONFIG_TABLE_T *nct, UWTM_REPORT_TYPE report)
{
  int result;
  UNSIGNED8 inaug_data = 0;

  tcn_node_type = nct->omode;
  cmd_omode = tcn_node_type;
  single_node = nct->single_node;
#ifndef SIN
  /* do nothing with single */
  UNREFERENCED(single_node);
#endif
  nd = nct->wtb_config.node_descriptor;
  /* G. Carnevale 98/09/14: do not clobber already set report functions */
  if (nct->wtb_config.Report == NULL) {
      nct->wtb_config.Report = uwtm_placebo_report_func;
  }
  if (nct->wtb_config.ErrorReport == NULL) {
      nct->wtb_config.ErrorReport = uwtm_placebo_error_func;
  }
  result = ls_t_Configure(&(nct->wtb_config));
#ifdef SIN
  if (PI_RET_OK != pi_create_timeout(&sn_tid,uwtm_timeout,NULL,PI_FOREVER)) {
    result = UWTM_ERR_MEM;
  }
#endif
  UWTM_Report = report;

  li_t_GetInaugDataByte(TRAC_CONTROL,&inaug_data);
  trac_attr = inaug_data & TRAC_MASK;
#ifdef SIN
  if (single_node
#ifdef O_STANDBY
  && !hw_standby
#endif
  ) {
    pi_enable_timeout(sn_tid,SINGLE_NODE_TIMEOUT);
  }
  single_topo = MAX_TOPO_COUNT;
#endif
  UIC_started = TRUE;
  return(result);
} /* uwtm_configure */


/******************************************************************************
*   name:         uic_fc_change_omode_request
*   function:
*   return value:
******************************************************************************/
void uic_ch_omode_requ(UNSIGNED8 *status)
{
  if ((*status == UWTM_WEAK)  ||
      (*status == UWTM_SLAVE) ||
      (*status == UWTM_STRONG)||
      (*status == UWTM_PASSIVE)) {
    cmd_omode = *status;
    uwtm_cmd_inaug_count++;
    uwtm_inaug_reason = UIC_IR_CMD;
    *status = UWTM_OK;
  } else {
    *status = UWTM_ERR_PARAM;
  }
}

int uic_fc_change_omode_request(
  UIC_TC_CHANGE_OMODE *call_msg,
  UIC_TR_CHANGE_OMODE **reply_msg,
  REPLY_FREE *free_reply_msg)
{
  if ((*reply_msg = (UIC_TR_CHANGE_OMODE*)PI_ALLOC(sizeof(UIC_TR_CHANGE_OMODE))) != NULL) {

    exchange_UIC_HEADER((UIC_Msg *)*reply_msg,(UIC_Msg *)call_msg);

    (*reply_msg)->state_cmd = call_msg->state_cmd;
    uic_ch_omode_requ(&((*reply_msg)->state_cmd));

    *free_reply_msg = PI_FREE;
    return UWTM_OK;
  } else { /* PI_ALLOC failed */
    *reply_msg = NULL;
    *free_reply_msg = NULL;
    return UWTM_ERR_BUF;
  }
  return UWTM_ERR_BUF;
} /* uic_fc_change_omode_request */

/******************************************************************************
*   name:         uic_fc_inauguration_control_request
*   function:
*   return value:
******************************************************************************/
void uic_inaug_ctrl_requ(UNSIGNED8 *status)
{
  if ((*status == UIC_INHIBIT) ||
      (*status == UIC_ALLOWED)) {
    cmd_inhibit = *status;
    *status = UWTM_OK;
  } else {
    *status = UWTM_ERR_PARAM;
  }
}

int uic_fc_inauguration_control_request(
  UIC_TC_INAUGURATION_CONTROL *call_msg,
  UIC_TR_INAUGURATION_CONTROL **reply_msg,
  REPLY_FREE *free_reply_msg)
{
  if ((*reply_msg = (UIC_TR_INAUGURATION_CONTROL*)PI_ALLOC(sizeof(UIC_TR_INAUGURATION_CONTROL))) != NULL) {

    exchange_UIC_HEADER((UIC_Msg *)*reply_msg,(UIC_Msg *)call_msg);

    (*reply_msg)->state_cmd = call_msg->state_cmd;
    uic_inaug_ctrl_requ(&((*reply_msg)->state_cmd));

    *free_reply_msg = PI_FREE;
    return UWTM_OK;
  } else { /* PI_ALLOC failed */
    *reply_msg = NULL;
    *free_reply_msg = NULL;
    return UWTM_ERR_BUF;
  }
  return UWTM_ERR_BUF;
} /* uic_fc_inauguration_control_request */

/******************************************************************************
*   name:         uic_fc_inauguration_enforce_request
*   function:
*   return value:
******************************************************************************/
void uic_inaug_enf_requ (UNSIGNED8 *status)
{
    if (*status == UIC_INAUG) {
      cmd_inaug = UIC_INAUG;
      uwtm_cmd_inaug_count++;
      *status = UWTM_OK;
    } else {
      *status = UWTM_ERR_PARAM;
    }
}

int uic_fc_inauguration_enforce_request(
  UIC_TC_INAUGURATION_ENFORCE *call_msg,
  UIC_TR_INAUGURATION_ENFORCE **reply_msg,
  REPLY_FREE *free_reply_msg)
{
  if ((*reply_msg = (UIC_TR_INAUGURATION_ENFORCE*)PI_ALLOC(sizeof(UIC_TR_INAUGURATION_ENFORCE))) != NULL) {

    exchange_UIC_HEADER((UIC_Msg *)*reply_msg,(UIC_Msg *)call_msg);

    (*reply_msg)->state_cmd = UIC_INAUG;
    uic_inaug_enf_requ(&((*reply_msg)->state_cmd));

    *free_reply_msg = PI_FREE;
    return UWTM_OK;
  } else { /* PI_ALLOC failed */
    *reply_msg = NULL;
    *free_reply_msg = NULL;
    return UWTM_ERR_BUF;
  }
  return UWTM_ERR_BUF;
} /* uic_fc_inauguration_enforce_request */

/******************************************************************************
*   name:         uic_fc_leading_request
*   function:
*   return value:
******************************************************************************/
void uic_lead_requ(UNSIGNED8 *status, UNSIGNED8 direc)
{
  UNSIGNED8 temp_ld = *status;
  if (((temp_ld == UIC_LEADING) ||
      (temp_ld == UIC_NOT_LEADING)) &&
      ((direc == UIC_DIR1) ||
      (direc == UIC_DIR2))) {
    utbc_set_leading_req(*status,direc);
    *status = UWTM_OK;
  } else {
    *status = UWTM_ERR_PARAM;
  }
  if ((temp_ld != ld_requ)
      || ((leading_dir != direc)&&(temp_ld == UIC_LEADING))) {
    UNSIGNED8 state = UIC_INAUG;
    uic_inaug_enf_requ(&state);
  }
  ld_requ = temp_ld;
}

int uic_fc_leading_request(
  UIC_TC_LEADING *call_msg,
  UIC_TR_LEADING **reply_msg,
  REPLY_FREE *free_reply_msg)
{
  if ((*reply_msg = (UIC_TR_LEADING*)PI_ALLOC(sizeof(UIC_TR_LEADING))) != NULL) {

    exchange_UIC_HEADER((UIC_Msg *)*reply_msg,(UIC_Msg *)call_msg);

    (*reply_msg)->state_cmd = call_msg->hdr.state_cmd;
    uic_lead_requ(&((*reply_msg)->state_cmd),call_msg->ld_direc);

    *free_reply_msg = PI_FREE;
    return UWTM_OK;

  } else { /* PI_ALLOC failed */
    *reply_msg = NULL;
    *free_reply_msg = NULL;
    return UWTM_ERR_BUF;
  }
  return UWTM_ERR_BUF;
} /* uic_fc_leading_request */

/******************************************************************************
*   name:         uic_fc_sleep_request
*   function:
*   return value:
******************************************************************************/
void uic_sleep_requ(UNSIGNED8 *status)
{
    if ((*status == UIC_SLEEP) ||
        (*status == UIC_NO_SLEEP)) {
      cmd_sleep = *status;
      *status = UWTM_OK;
    } else {
      *status = UWTM_ERR_PARAM;
    }
}

int uic_fc_sleep_request(
  UIC_TC_SLEEP *call_msg,
  UIC_TR_SLEEP **reply_msg,
  REPLY_FREE *free_reply_msg)
{
  if ((*reply_msg = (UIC_TR_SLEEP*)PI_ALLOC(sizeof(UIC_TR_SLEEP))) != NULL) {

    exchange_UIC_HEADER((UIC_Msg *)*reply_msg,(UIC_Msg *)call_msg);

    (*reply_msg)->state_cmd = call_msg->state_cmd;
    uic_sleep_requ(&((*reply_msg)->state_cmd));

    *free_reply_msg = PI_FREE;
    return UIC_OK;
  } else { /* PI_ALLOC failed */
    *reply_msg = NULL;
    *free_reply_msg = NULL;
    return UWTM_ERR_BUF;
  }
  return UWTM_ERR_BUF;
} /* uic_fc_sleep_request */

/******************************************************************************
*   name:         uic_fc_read_uwtm_state_request
*   function:
*   return value:
******************************************************************************/
int uic_fc_read_uwtm_state_request(
  UIC_TC_READ_UWTM_STATE *call_msg,
  UIC_TR_READ_UWTM_STATE **reply_msg,
  REPLY_FREE *free_reply_msg)
{
  if ((*reply_msg = (UIC_TR_READ_UWTM_STATE*)PI_ALLOC(sizeof(UIC_TR_READ_UWTM_STATE))) != NULL) {

/*    memset(*reply_msg,0,sizeof(UIC_TR_READ_UWTM_STATE)); /* fill with zeroes per default */
    exchange_UIC_HEADER((UIC_Msg *)*reply_msg,(UIC_Msg *)call_msg);

    (*reply_msg)->hdr.state_cmd     = (UNSIGNED8)ls_t_GetStatus(&status);
    (*reply_msg)->uwtm_state        = main_state;
    (*reply_msg)->master_conflict   =
#ifdef UMS_SMC
    uwtm_master_conflict();
#else
    master_conflict;
#endif
    (*reply_msg)->red_state         = redundancy_start ? UIC_DEAD : UIC_ALIVE;
    (*reply_msg)->inaug_reason      = uwtm_inaug_reason;
    (*reply_msg)->inaug_result      = (main_state == UIC_M_REGULAR) ? UIC_I_OK : UIC_I_ERR;
    (*reply_msg)->res1              = 0;
    (*reply_msg)->rel_time          = d_tc_get_systime();
    (*reply_msg)->tcn_inaug_count   = uwtm_wtb_inaug_count;
    (*reply_msg)->uic_inaug_count   = uwtm_uic_inaug_count;
    (*reply_msg)->uic_inaug_error_count = uwtm_inaug_err_count;
    (*reply_msg)->ll_inaug_count    = uwtm_uic_inaug_count - uwtm_cmd_inaug_count;
    (*reply_msg)->cmd_inaug_count   = uwtm_cmd_inaug_count;
    (*reply_msg)->e_count           = uic_e_count;
    (*reply_msg)->res2              = 0;
    (*reply_msg)->WTB_hardware      = status.WTB_hardware;
    (*reply_msg)->WTB_software      = status.WTB_software;
    (*reply_msg)->hardware_state    = status.hardware_state;
    (*reply_msg)->link_layer_state  = status.link_layer_state;
    (*reply_msg)->net_inhibit       = status.net_inhibit;
    (*reply_msg)->node_address      = status.node_address;
    (*reply_msg)->node_orient       = status.node_orient;
    (*reply_msg)->node_strength     = status.node_strength;
    (*reply_msg)->node_frame_size   = status.node_descriptor.node_frame_size;
    (*reply_msg)->node_period       = status.node_descriptor.node_period;
    (*reply_msg)->node_type         = status.node_descriptor.node_key.node_type;
    (*reply_msg)->node_version      = status.node_descriptor.node_key.node_version;
    (*reply_msg)->node_report       = status.node_status.node_report;
    (*reply_msg)->user_report       = status.node_status.user_report;

    *free_reply_msg = PI_FREE;
    return UWTM_OK;
  } else { /* PI_ALLOC failed */
    *reply_msg = NULL;
    *free_reply_msg = NULL;
    return UWTM_ERR_BUF;
  }
  return UWTM_ERR_BUF;
} /* uic_fc_read_uwtm_state_request */


/******************************************************************************
*   name:         uic_fc_read_uwtm_topography_request
*   function:
*   return value:
******************************************************************************/
int uic_fc_read_uwtm_topography_request(
  UIC_TC_READ_UWTM_TOPOGRAPHY *call_msg,
  UIC_TR_READ_UWTM_TOPOGRAPHY **reply_msg,
  REPLY_FREE *free_reply_msg)
{
  if ((*reply_msg = (UIC_TR_READ_UWTM_TOPOGRAPHY*)PI_ALLOC(sizeof(UIC_TR_READ_UWTM_TOPOGRAPHY))) != NULL) {

    exchange_UIC_HEADER((UIC_Msg *)*reply_msg,(UIC_Msg *)call_msg);

    if ((((*reply_msg)->hdr.state_cmd = (UNSIGNED8)uwtm_get_topo(&u_topo)) == L_OK)
        && map_wtb_is_inaugurated){
#if 0 /* Adtranz doesn't want it */
      (*reply_msg)->node_address      = u_topo.node_address;
      (*reply_msg)->node_orient       = u_topo.node_orient;
      (*reply_msg)->individual_period = u_topo.individual_period;
      (*reply_msg)->is_strong         = u_topo.is_strong;
#endif
      (*reply_msg)->topo_counter      = u_topo.topo_counter;
      (*reply_msg)->number_of_nodes   = u_topo.number_of_nodes;
      (*reply_msg)->bottom_address    = u_topo.bottom_address;
      (*reply_msg)->top_address       = u_topo.top_address;
      (*reply_msg)->uic_address       = my_uic_addr;
    } else {
#if 0 /* Adtranz doesn't want it */
      (*reply_msg)->node_address      = 0;
      (*reply_msg)->node_orient       = 0;
      (*reply_msg)->individual_period = 0;
      (*reply_msg)->is_strong         = 0;
#endif
      (*reply_msg)->topo_counter      = 0;
      (*reply_msg)->number_of_nodes   = 0;
      (*reply_msg)->bottom_address    = 0;
      (*reply_msg)->top_address       = 0;
      (*reply_msg)->uic_address       = 0;
      if ((*reply_msg)->hdr.state_cmd == 0) {
        (*reply_msg)->hdr.state_cmd = UIC_S_ERR_FUNCTION_DISABLED;
      }
    }
    (*reply_msg)->res1              = 0;

    *free_reply_msg = PI_FREE;
    return UWTM_OK;
  } else { /* PI_ALLOC failed */
    *reply_msg = NULL;
    *free_reply_msg = NULL;
    return UWTM_ERR_BUF;
  }
  return UWTM_ERR_BUF;
} /* uic_fc_read_uwtm_topography_request */


/******************************************************************************
*   name:         uwtm_request  (global)
*   function:
*   return value:
******************************************************************************/
int uwtm_request(void *p_uic_callmsg,    /* pointer to input message    */
                 WORD32   uic_call_size,    /* size of message in bytes    */
                 AM_ADDRESS *caller,        /* address of caller */
                 void **p_uic_replymsg,   /* pointer to reply message    */
                 WORD32  *p_uic_reply_size, /* size of reply message       */
                 REPLY_FREE *free_replymsg) /* pointer to mem free function */
{
  int code = NTOH16(((UNSIGNED8 *)p_uic_callmsg+UIC_HEADER_CMD_OFFSET));

  UNREFERENCED(uic_call_size);
  UNREFERENCED(caller);

  *free_replymsg = PI_FREE;
  *p_uic_replymsg = NULL;

  /* First test for valid ptrs   */
  /* and sizes                   */
  if ((NULL == p_uic_callmsg)||(NULL == p_uic_replymsg)) {
    return UWTM_ERR_PTR;
  } else {

    switch(code) {

      case UIC_FC_CHANGE_OMODE:
        {
          REPLY_FREE free_reply_msg;
          UIC_TR_CHANGE_OMODE *reply_t_msg;

          UIC_TC_CHANGE_OMODE  *call_t_msg = (UIC_TC_CHANGE_OMODE *)
            PI_ALLOC(sizeof(UIC_TC_CHANGE_OMODE));
          if (NULL == call_t_msg) {
            return UWTM_ERR_BUF;
          }

          NTOH_uic_fc_change_omode((UNSIGNED8 *)p_uic_callmsg,call_t_msg);

          if (uic_fc_change_omode_request(call_t_msg,&reply_t_msg,&free_reply_msg) != UWTM_OK) {
            PI_FREE(call_t_msg);
            return UWTM_ERR_BUF;
          }

          PI_FREE(call_t_msg);

          *p_uic_replymsg = PI_ALLOC(sizeof(UIC_TR_CHANGE_OMODE));
            /* Byte stream cannot be longer than struct */
          if (*p_uic_replymsg == NULL) {
            return UWTM_ERR_BUF;
          }

          HTON_uic_fr_change_omode(reply_t_msg,(UNSIGNED8 *)*p_uic_replymsg);

          free_reply_msg(reply_t_msg);
          *p_uic_reply_size = UIC_HEADER_BYTES;
        } break;

      case UIC_FC_INAUGURATION_CONTROL:
        {
          REPLY_FREE free_reply_msg;
          UIC_TR_INAUGURATION_CONTROL *reply_t_msg;

          UIC_TC_INAUGURATION_CONTROL  *call_t_msg = (UIC_TC_INAUGURATION_CONTROL *)
            PI_ALLOC(sizeof(UIC_TC_INAUGURATION_CONTROL));
          if (NULL == call_t_msg) {
            return UWTM_ERR_BUF;
          }

          NTOH_uic_fc_inauguration_control((UNSIGNED8 *)p_uic_callmsg,call_t_msg);

          if (uic_fc_inauguration_control_request(call_t_msg,&reply_t_msg,&free_reply_msg) != UWTM_OK) {
            PI_FREE(call_t_msg);
            return UWTM_ERR_BUF;
          }

          PI_FREE(call_t_msg);

          *p_uic_replymsg = PI_ALLOC(sizeof(UIC_TR_INAUGURATION_CONTROL));
            /* Byte stream cannot be longer than struct */
          if (*p_uic_replymsg == NULL) {
            return UWTM_ERR_BUF;
          }

          HTON_uic_fr_inauguration_control(reply_t_msg,(UNSIGNED8 *)*p_uic_replymsg);

          free_reply_msg(reply_t_msg);
          *p_uic_reply_size = UIC_HEADER_BYTES;
        } break;

      case UIC_FC_INAUGURATION_ENFORCE:
        {
          REPLY_FREE free_reply_msg;
          UIC_TR_INAUGURATION_ENFORCE *reply_t_msg;

          UIC_TC_INAUGURATION_ENFORCE  *call_t_msg = (UIC_TC_INAUGURATION_ENFORCE *)
            PI_ALLOC(sizeof(UIC_TC_INAUGURATION_ENFORCE));
          if (NULL == call_t_msg) {
            return UWTM_ERR_BUF;
          }

          NTOH_uic_fc_inauguration_enforce((UNSIGNED8 *)p_uic_callmsg,call_t_msg);

          if (uic_fc_inauguration_enforce_request(call_t_msg,&reply_t_msg,&free_reply_msg) != UWTM_OK) {
            PI_FREE(call_t_msg);
            return UWTM_ERR_BUF;
          }

          PI_FREE(call_t_msg);

          *p_uic_replymsg = PI_ALLOC(sizeof(UIC_TR_INAUGURATION_ENFORCE));
            /* Byte stream cannot be longer than struct */
          if (*p_uic_replymsg == NULL) {
            return UWTM_ERR_BUF;
          }

          HTON_uic_fr_inauguration_enforce(reply_t_msg,(UNSIGNED8 *)*p_uic_replymsg);

          free_reply_msg(reply_t_msg);
          *p_uic_reply_size = UIC_HEADER_BYTES;
        } break;

      case UIC_FC_SET_LEADING_REQU:
        {
          REPLY_FREE free_reply_msg;
          UIC_TR_LEADING *reply_t_msg;

          UIC_TC_LEADING  *call_t_msg = (UIC_TC_LEADING *)
            PI_ALLOC(sizeof(UIC_TC_LEADING));
          if (NULL == call_t_msg) {
            return UWTM_ERR_BUF;
          }

          NTOH_uic_fc_leading((UNSIGNED8 *)p_uic_callmsg,call_t_msg);

          if (uic_fc_leading_request(call_t_msg,&reply_t_msg,&free_reply_msg) != UWTM_OK) {
            PI_FREE(call_t_msg);
            return UWTM_ERR_BUF;
          }

          PI_FREE(call_t_msg);

          *p_uic_replymsg = PI_ALLOC(sizeof(UIC_TR_LEADING));
            /* Byte stream cannot be longer than struct */
          if (*p_uic_replymsg == NULL) {
            return UWTM_ERR_BUF;
          }

          HTON_uic_fr_leading(reply_t_msg,(UNSIGNED8 *)*p_uic_replymsg);

          free_reply_msg(reply_t_msg);
          *p_uic_reply_size = UIC_HEADER_BYTES;
        } break;

      case UIC_FC_SLEEP:
        {
          REPLY_FREE free_reply_msg;
          UIC_TR_SLEEP *reply_t_msg;

          UIC_TC_SLEEP  *call_t_msg = (UIC_TC_SLEEP *)
            PI_ALLOC(sizeof(UIC_TC_SLEEP));
          if (NULL == call_t_msg) {
            return UWTM_ERR_BUF;
          }

          NTOH_uic_fc_sleep((UNSIGNED8 *)p_uic_callmsg,call_t_msg);

          if (uic_fc_sleep_request(call_t_msg,&reply_t_msg,&free_reply_msg) != UWTM_OK) {
            PI_FREE(call_t_msg);
            return UWTM_ERR_BUF;
          }

          PI_FREE(call_t_msg);

          *p_uic_replymsg = PI_ALLOC(sizeof(UIC_TR_SLEEP));
            /* Byte stream cannot be longer than struct */
          if (*p_uic_replymsg == NULL) {
            return UWTM_ERR_BUF;
          }

          HTON_uic_fr_sleep(reply_t_msg,(UNSIGNED8 *)*p_uic_replymsg);

          free_reply_msg(reply_t_msg);
          *p_uic_reply_size = UIC_HEADER_BYTES;
        } break;

      case UIC_FC_READ_UWTM_STATE:
        {
          REPLY_FREE free_reply_msg;
          UIC_TR_READ_UWTM_STATE *reply_t_msg;

          UIC_TC_READ_UWTM_STATE  *call_t_msg = (UIC_TC_READ_UWTM_STATE *)
            PI_ALLOC(sizeof(UIC_TC_READ_UWTM_STATE));
          if (NULL == call_t_msg) {
            return UWTM_ERR_BUF;
          }

          NTOH_uic_fc_read_uwtm_state((UNSIGNED8 *)p_uic_callmsg,call_t_msg);

          if (uic_fc_read_uwtm_state_request(call_t_msg,&reply_t_msg,&free_reply_msg) != UWTM_OK) {
            PI_FREE(call_t_msg);
            return UWTM_ERR_BUF;
          }

          PI_FREE(call_t_msg);

          *p_uic_replymsg = PI_ALLOC(sizeof(UIC_TR_READ_UWTM_STATE));
          if (*p_uic_replymsg == NULL) {
            return UWTM_ERR_BUF;
          }
            /* Byte stream cannot be longer than struct */

          HTON_uic_fr_read_uwtm_state(reply_t_msg,(UNSIGNED8 *)*p_uic_replymsg);

          free_reply_msg(reply_t_msg);
          *p_uic_reply_size = UIC_TR_READ_UWTM_STATE_LENGTH;
          } break;

      case UIC_FC_READ_UWTM_TOPOGRAPHY:
        {
          REPLY_FREE free_reply_msg;
          UIC_TR_READ_UWTM_TOPOGRAPHY *reply_t_msg;

          UIC_TC_READ_UWTM_TOPOGRAPHY  *call_t_msg = (UIC_TC_READ_UWTM_TOPOGRAPHY *)
            PI_ALLOC(sizeof(UIC_TC_READ_UWTM_TOPOGRAPHY));
          if (NULL == call_t_msg) {
            return UWTM_ERR_BUF;
          }

          NTOH_uic_fc_read_uwtm_topography((UNSIGNED8 *)p_uic_callmsg,call_t_msg);

          if (uic_fc_read_uwtm_topography_request(call_t_msg,&reply_t_msg,&free_reply_msg) != UWTM_OK) {
            PI_FREE(call_t_msg);
            return UWTM_ERR_BUF;
          }

          PI_FREE(call_t_msg);

          *p_uic_replymsg = PI_ALLOC(sizeof(UIC_TR_READ_UWTM_TOPOGRAPHY));
            /* Byte stream cannot be longer than struct */
          if (*p_uic_replymsg == NULL) {
            return UWTM_ERR_BUF;
          }

          HTON_uic_fr_read_uwtm_topography(reply_t_msg,(UNSIGNED8 *)*p_uic_replymsg);

          free_reply_msg(reply_t_msg);
          *p_uic_reply_size = UIC_TR_READ_UWTM_TOPOGRAPHY_LENGTH;
        } break;

      default:
        return UWTM_ERR_MSG_CODE;
        break;

    } /* Ende switch */
  }
  return UWTM_OK;
} /* uwtm_request */

