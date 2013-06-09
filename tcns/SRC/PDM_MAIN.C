/*************************************************************************
**        Copyright (C) Siemens AG 1993 All Rights Reserved. Confidential    
**========================================================================
**
**    PROJECT::    TCN Train Communication Network
                   JDP Joint Development Project

**    MODULE::     PDM Process Data Marshalling

**    WORKFILE::   $Workfile$
**************************************************************************
**    TASK::       This module includes all the interface procedures and 
                   the main task of PDM.
**************************************************************************
**    AUTHOR::     Krafft (kra)    
                   Heindel (Hei)  
**    CREATED::    13.08.93
**========================================================================
**    HISTORY::    AUTHOR::   $Author: schiavor $
                   REVISION:: $Revision: 1.1.2.1 $  
                   MODTIME::  $Date: 2006/12/01 10:40:06 $
      previous history:
                   date      sign   remark
                   16.08.93  kra    start of development
                   10.02.95  kra    Modification for WTB 1.9
                                    Topography has different structure!!

      $Log: PDM_MAIN.C,v $
      Revision 1.1.2.1  2006/12/01 10:40:06  schiavor
      01/12/2006                         Napoli                            SchiavoR
      Primo inserimento per utilizzo libreria nel GTW

*************************************************************************/

#define PDM_MODULE	/* access to PDM internal definitions */
#define PDM_MAIN_C_	/* braindead IC960 compiler           */

#ifdef O_960
#pragma noalign
#endif

#include <string.h>
#include "lp_sys.h"
#include "pi_sys.h"
#include "apd_incl.h"
#include "pdmdebug.h"
#include "pdmtypes.h"
#include "pdm_cp.h"
#include "pdm_lcfg.h"
#include "pdm_main.h"
#include "recorder.h"

/* the last nibble of PDM_INTERNAL, PDM_EXPORT and PDM_IMPORT builds an index
 * int the copy list data structure array
 */
#define PDM_TYPE_MASK   0x0003    /* mask out index from PDM_INTERNAL
				   * PDM_EXPORT, PDM_IMPORT
				   */

#define PDM_INT         (PDM_INTERNAL & PDM_TYPE_MASK)
#define PDM_EXP         (PDM_EXPORT & PDM_TYPE_MASK)
#define PDM_IMP         (PDM_IMPORT & PDM_TYPE_MASK)

/* messages to be send if marshalling cycle elapse */
#define TOPO_IND 0x0010

typedef struct pdm_msg
{
  UNSIGNED16 msg_class;
  UNSIGNED16 param1;
  void   *param2;
} T_PDM_MSG;

/* mailbox queue size */
#define QUEUE_SIZE 10

#define RECORD_M_ERR(p_msg) (nse_record("PDMMAIN",__LINE__,  \
                                        NSE_EVENT_TYPE_ERROR,\
                                        NSE_ACTION_CONTINUE, \
                                        (strlen(p_msg)+1),   \
                                        (UINT8 *)p_msg))

#define RECORD_M_EVENT(p_msg) (nse_record("PDMMAIN",__LINE__,  \
                                        NSE_EVENT_TYPE_SPECIAL,\
                                        NSE_ACTION_CONTINUE,   \
                                        (strlen(p_msg)+1),     \
                                        (UINT8 *)p_msg))


/*****************************************************************************/
/*---------- global variables of PDM ----------------------------------------*/

T_PDM_STATUS pdm_stat = {0};

void *        p_pdm_database;   /* pointer to actual database              */

T_PDM_COPY_LIST pdm_copy_list[PDM_MARSH_TYPES];

short pdm_sema;                    /* id of semaphore to synchronise access
				      to pdm_stat structure                   */

short pdm_mailbox;                   /* mailbox id for marshalling             */

/* shadow buffer for WTB source port */
UNSIGNED8 pdm_shadow_buf[PDM_SHADOW_BUF_LEN];

UNSIGNED16 pdm_ftf_size = 1;

/* global vars to store WTB topography data */
unsigned char  wtb_tsid;
Type_Topography * p_wtb_topo;



/*new page*/
/******************************************************************************
*
*   name:          pdm_timeout
*
*   function:      Called if timout for internal, export or import marshalling
*                  occurs. it then
*                  sends a message to main procedure to signal it. this mail
*                  triggers the corresponding marshalling kind.
*
*   input:         message to be send
*
*   used globals:  mailbox id
*

******************************************************************************/
STATIC T_PDM_MSG pdm_msg[PDM_MARSH_TYPES + 1];

void pdm_timeout(void *arg)
{
    short status;
    T_PDM_COPY_LIST *p_cpl = (T_PDM_COPY_LIST *)arg;
    T_PDM_MSG *msg = &pdm_msg[p_cpl->type & PDM_TYPE_MASK];
    
    msg->msg_class = p_cpl->type;
    
    pi_send_queue(pdm_mailbox, (char *)msg, &status);
}




/*new page*/
/******************************************************************************
*
*   name:         config_marsh
*
*   function:     configures copy lists out of database with specified mode
*                 depending on kind of marshalling. if the specified mode
*                 leads to an error, the default mode is used to configure.
*                 if the default mode leads to an error too, marshalling for
*                 that kind (internal, export, import) is set to disabled.
*                 after successful configuration the timeout is started.
*
*   used globals: pdm_copy_list
*
*   return value: 
*
******************************************************************************/
int config_marsh(UNSIGNED16 type)
{
    int result;
    T_PDM_COPY_LIST *p_cpl;

    p_cpl = &pdm_copy_list[type & PDM_TYPE_MASK];
    p_cpl->type = type;
    result = build_copy_list(p_cpl, p_pdm_database);
    if ((result != PDM_OK) && (p_cpl->mode != PDM_DEFAULT_MODE)) {
	RECORD_M_ERR("Can't config mode");
	/* try it with default mode. If this fails marshalling for
	 * this copy list is disabled
	 */
	p_cpl->mode = PDM_DEFAULT_MODE;
	(void)build_copy_list(p_cpl, p_pdm_database);
    }
    return(result);
}

/*new page*/
/******************************************************************************
*
*   name:         pdm_init
*
*   function:     Initialises internal structurs and parameters
*                 Must be called before marshalling work can be done
*                 (pd_marshall).
*
*   input:        none
*
*   used globals: pdm_stat structure
*
*   output:       none
*
*   return value: PDM_OK
*                 Return values of PIL
*
******************************************************************************/
/*
 * NOTE: in case of error resources are not released. Since further pdm_init
 * calls are ignored, this can be tolerated
 */
int pdm_init (void)
{
    static short status;
    unsigned long i;

    if (pdm_stat.initialised) {
	return (status);
    }
    pdm_stat.initialised = TRUE;

    /*---------------- get needed operating system objects --------------------*/
    pdm_sema = pi_create_semaphore(1, PI_ORD_FCFS, &status);
    if (status != PI_RET_OK) {
	RECORD_M_ERR("Can't get interface semaphore");
	return (status);
    }

    pdm_mailbox = pi_create_queue(QUEUE_SIZE, PI_ORD_FCFS, &status);
    if (status != PI_RET_OK) {
	RECORD_M_ERR("Can't create queue");
	return (status);
    }

    for (i = 0; i < PDM_MARSH_TYPES; i++) {
	status = pi_create_timeout((short*)&pdm_copy_list[i].timeout_id, 
				   pdm_timeout, &pdm_copy_list[i], PI_FOREVER);
	if (status != PI_RET_OK) {
	    RECORD_M_ERR("Can't create timeout");
	    return (status);
	}
    }


    /*--------------- set pdm internal status ---------------------------------*/
    p_pdm_database = NULL;
    
    pdm_stat.pdm_sw_version = PDM_SW_VERSION;
    pdm_stat.configured     = FALSE;
    pdm_stat.wtb_ts_created = FALSE;
    pdm_stat.topo_indicated = FALSE;

    for (i = 0; i < PDM_MARSH_TYPES; i++) {
	pdm_copy_list[i].status = PDM_INIT;
	pdm_copy_list[i].result = PDM_OK;
	pdm_copy_list[i].configured = FALSE;
	pdm_copy_list[i].shadow_buf = NULL;
    }
    pdm_copy_list[PDM_EXP].shadow_buf = pdm_shadow_buf;

  return (PDM_OK);
}

/*new page*/
/******************************************************************************
*
*   name:          pdm_config
*
*   function:      Hand over the database for marshalling
*
*   input:         pointer to database
*
*   used globals:  pdm_stat
*                  wtb_tsid
*
*   output:        none
*
*   return value:  PDM_OK
*
******************************************************************************/
int pdm_config (void * p_pdm_db,
                UNSIGNED16 internal_mode,
                UNSIGNED16 export_mode,
                UNSIGNED16 import_mode)
{
    int result = PDM_OK;
    short status;

    /* lock sema to access pdm_stat */
    pi_pend_semaphore(pdm_sema, PI_FOREVER, &status);
    p_pdm_database = p_pdm_db;    /* pointer to actual database */
    pdm_stat.configured = TRUE;                  /* mark: pdm_config called    */
    
    pdm_stat.mdb_version = ((T_MDB_DB_HDR *)p_pdm_db)->db_version;
    /* if PDM has been reconfigured, internal status must be reset */

    pdm_copy_list[PDM_INT].status = PDM_INIT;
    pdm_copy_list[PDM_INT].mode = internal_mode;
    pdm_copy_list[PDM_EXP].status = PDM_INIT;
    pdm_copy_list[PDM_EXP].mode = export_mode;
    pdm_copy_list[PDM_IMP].status = PDM_INIT;
    pdm_copy_list[PDM_IMP].mode = import_mode;

    /* configure cp lists for internal marshalling */
    result = config_marsh(PDM_INTERNAL);

    pi_post_semaphore(pdm_sema, &status);
    return (result);
}

/*new page*/
/******************************************************************************
*
*   name:          pdm_get_status
*
*   function:      returns content of statusstructure of pdm. Memory must be
*                  provided by the caller!
*
*   input:         pointer to status structure
*
*   used globals:  pdm_stat_list
*
******************************************************************************/
void 
pdm_get_status (T_PDM_STATUS * p_pdm_st)
{
    *p_pdm_st = pdm_stat;
    p_pdm_st->ex_status = pdm_copy_list[PDM_EXP].status;
    p_pdm_st->ex_mode   = pdm_copy_list[PDM_EXP].mode;
    p_pdm_st->ex_cycle  = pdm_copy_list[PDM_EXP].cycle;
    p_pdm_st->ex_result = pdm_copy_list[PDM_EXP].result;
    p_pdm_st->in_status = pdm_copy_list[PDM_INT].status;
    p_pdm_st->in_mode   = pdm_copy_list[PDM_INT].mode;
    p_pdm_st->in_cycle  = pdm_copy_list[PDM_INT].cycle;
    p_pdm_st->in_result = pdm_copy_list[PDM_INT].result;
    p_pdm_st->im_status = pdm_copy_list[PDM_IMP].status;
    p_pdm_st->im_mode   = pdm_copy_list[PDM_IMP].mode;
    p_pdm_st->im_cycle  = pdm_copy_list[PDM_IMP].cycle;
    p_pdm_st->im_result = pdm_copy_list[PDM_IMP].result;
}

/*new page*/
/******************************************************************************
*
*   name:         pdm_wtb_ts_crea_ind
*
*   function:     This proc must be called when train bus traffic store
*                 is initialised. Beginning from this moment, it is possible
*                 to configure copy list for export.
*
*   input:        traffic store id of wtb ts
*
*   used globals: wtb_tsid which holds the traffic store id of the WTB
*
*   output:       none
*
*   return value: none
*
******************************************************************************/
void pdm_wtb_ts_crea_ind (unsigned char wtb_ts_id)
{
    short status;

    /* lock sema to access pdm_stat                */
    pi_pend_semaphore(pdm_sema, PI_FOREVER, &status);

    /* do not call this procedure before PDM initialised (sema don't exist)*/
    wtb_tsid = wtb_ts_id;
    pdm_stat.wtb_ts_created = TRUE;

    /* after train bus ts is created export can be started */
    if (pdm_stat.configured) {
	config_marsh(PDM_EXPORT);
    }
    pi_post_semaphore(pdm_sema, &status);
}


/*new page*/
/******************************************************************************
*
*   name:         wtb_topo_indication
*
*   function:     Must be called if the topography of the WTB has changed. It
*                 then (re)configures the marshalling for the import, after
*                 analysing the topography.
*
*   input:        pointer to topography generated by WTB Link Layer Version 1.9
*
*   used globals: !!! own_node_address for WTB 1.9 not needed and therefor
*                     deleted!
*                 p_wtb_topo
*
*   output:       none
*
*   return value: none
*
*
*
******************************************************************************/
void 
wtb_topo_ind (Type_Topography *p_topography)
{
    p_wtb_topo = p_topography;

    pdm_stat.topo_indicated = TRUE;
    
    /* after train inauguration import can be started */
    if (pdm_stat.wtb_ts_created) {
	config_marsh(PDM_IMPORT);
    }
}

/*new page*/
/******************************************************************************
*
*   name:         pdm_wtb_topo_indication
*
*   function:     Must be called if the topography of the WTB has changed. It
*                 then (re)configures the marshalling for the import, after
*                 analysing the topography.
*                 This procedure is called by an external module
*
*   input:        pointer to topography generated by WTB Link Layer
*                 Own physical TCN address
*
*   used globals:
*
*   output:       none
*
*   return value: none
*
******************************************************************************/
void 
pdm_wtb_topo_ind (Type_Topography *p_topography)
{
    short status;
    T_PDM_MSG *p_msg;
    static T_PDM_MSG topo_msg[2];
    static unsigned short toggle = 0;

    /* toggle index to handle two indications coming in a short interval */
    toggle = !toggle;

    p_msg = &topo_msg[toggle];
    p_msg->msg_class = TOPO_IND;
    p_msg->param2 = p_topography;

    pi_send_queue(pdm_mailbox, (char *)p_msg, &status);
}

/*new page*/
/******************************************************************************
*
*   name:         pdm_reconfig_marsh
*
*   function:     if the application chooses to change marshlling mode
*                 this procedure should be called (e.g. vehicle application
*                 changes from leaded to leading vehicle).
*                 procedure reconfigures corresponding mode of specified type
*                 (internal, import or export).
*
*   input:        type: One of the following INTERNAL, EXPORT, IMPORT
*                 mode: Specified in MDB, user defined mode of marshalling
*
*   used globals: pdm_stat
*
*   output:       none
*
*   return value: PDM_OK
*                 PDM_ERR_CONF_MODE can not configure new or default mode
*                                   specified marshalling type is switched off
*                 PDM_WAR_DEF_USED  can not configure specified type, but
*                                   DEFAULT_MODE was used instead.
*                 PDM_ERR_CALL_SEQ  Wrong call sequence!
*
******************************************************************************/
int 
pdm_reconfig_marsh(UNSIGNED16 type, UNSIGNED16 mode)
{
    short status;
    int result = PDM_OK;
    T_PDM_COPY_LIST *p_cpl;
    
    if (!pdm_stat.configured) {
	return PDM_ERR_CALL_SEQ;
    }

    /* lock sema to access pdm_stat              */
    pi_pend_semaphore(pdm_sema, PI_FOREVER, &status);
    
    p_cpl = &pdm_copy_list[type & PDM_TYPE_MASK];
    p_cpl->type = type;
    p_cpl->mode = mode;
    /* configure export only, if 
     * - PDM_INTERNAL
     * - PDM_EXPORT and WTB TS created
     * - PDM_IMPORT and WTB TS created and topography indicated */
    if (type == PDM_INTERNAL 
	|| 
	(pdm_stat.wtb_ts_created 
	 &&
	 (type == PDM_EXPORT || pdm_stat.topo_indicated))) {
	config_marsh(type);
    }
    if ((p_cpl->status == PDM_DISABLED) && (mode != PDM_DO_NOT_MARSH)) {
	/* configuration failed completely? */
        result = PDM_ERR_CONF_MODE;
    }
    else if (mode != p_cpl->mode) {
	/* mode changed during configuration? -> DEFAULT mode used */
        result = PDM_WAR_DEF_USED;
    }
    pi_post_semaphore(pdm_sema, &status);
    return(result);
}



/*new page*/
/******************************************************************************
*
*   name:          pdm_wtb_put_variable
*
*   function:      Puts a variable to the wtb source port since the normal
*                  ap_put_variable can not be used to do it. the variable
*                  is not written in the real traffic store, because the
*                  source port is under the control of PDM.
*
*   input:         pv_name of the variable to be put
*                  p_var - pointer to application variable
*                  p_chk - pointer to application check variable
*                  pd_type - expected pd_type of the source port or
*                            0xff for don't care for PDtype.
*
*   used globals:  pdm_shadow_buf - shadow buffer of wtb source port (port 0)
*                  wtb_ts_id  - traffic store id of the wtb traffic store
*
*   output:        none
*
*   return value:  PDM_OK         if everything is ok
*                  PDM_INVAL_PV_N pv_name not belonging to WTB source port
*                  PDM_INVAL_PD_TYPE Expected pd_type not not present!
*
******************************************************************************/
short 
pdm_wtb_put_variable (const TYPE_PV_NAME_48 pv_name,
		      void  *p_var,
		      UNSIGNED8 *p_chk,
		      const UNSIGNED16 pd_type)
{
    short  status;
    UNSIGNED16 ftf;
    struct STR_APD_PV_DESCRIPTOR pvd;
    char    pv_set[lpd_sizeof_pv_set(2)];
    struct STR_APD_PV_SET *p_pv_set = (struct STR_APD_PV_SET *)pv_set;

    /* check whether the pv_name belongs to the wtb source port */
    if (PVN48GETTSID(pv_name) != wtb_tsid || PVN48GETPORT(pv_name) != 0) {
	return(PDM_INVAL_PV_N);
    }
    
    if (pd_type != PDM_DONT_CARE_OCTET) {
	/* do not access pd_type position if pd_type != dont care! */
	if (PVN48GETVAROFF(pv_name) < pdm_ftf_size * 8) {
	    return(PDM_INVAL_PV_N);
	}
    
	/* read pd_type out of shadow source port */
	if (pdm_ftf_size == 1) { 
	    ftf = pdm_shadow_buf[0];  
	}
	else {
	    ftf = pdm_shadow_buf[0] << 8 | pdm_shadow_buf[1];
	}

	/* check pd type field */
	if (ftf != pd_type) { 
	    return(PDM_INVAL_PD_TYPE);
	}
    }
    pvd.pv_name.ts_id       = PVN48GETTSID(pv_name);
    pvd.pv_name.prt_addr    = PVN48GETPORT(pv_name);
    pvd.pv_name.size        = PVN48GETSIZE(pv_name);
    pvd.pv_name.bit_offset  = PVN48GETVAROFF(pv_name);
    pvd.pv_name.type        = PVN48GETTYPE(pv_name);
    pvd.pv_name.ctl_offset  = PVN48GETCHKOFF(pv_name);
    pvd.p_variable = p_var;
    pvd.p_validity = p_chk;

    if (lpd_gen_pv_set (p_pv_set, &pvd, NULL, 1) != APD_OK) {
	return(PDM_INVAL_PV_N);
    }

    p_pv_set->multiple_access = APD_ACTIVE;
	
    /* lock sema to access pdm_shadow_buf            */
    pi_pend_semaphore(pdm_sema, PI_FOREVER, &status); 
    
    /* write variable to shadow source port */
    /* there is no chance to avoid the illegal cast. The interfaces of RTP
     * cause the inconsistence!
     */
    lp_put_pv_x(pdm_shadow_buf, 
		(struct STR_LP_PV_X *)p_pv_set->p_pv_list, p_pv_set->c_pv_list);

    pi_post_semaphore(pdm_sema, &status);


    return(PDM_OK);
}


/*new page*/
/******************************************************************************
 *
 *   pdm_main:      Does the marshalling work
 *
 *   used globals:  pdm_stat
 *                  pdm_copy_list
 *                  pdm_mailbox
 *
 *   return never 
 *
 ******************************************************************************/
void 
pdm_main (void)
{
    short status;
    T_PDM_MSG * p_msg;
    T_PDM_COPY_LIST *pcl;
    
    while (1) {
	p_msg = (T_PDM_MSG *)pi_receive_queue(pdm_mailbox, PI_FOREVER, &status);
	/* lock sema to access pdm_stat */
	pi_pend_semaphore(pdm_sema, PI_FOREVER, &status); 

	switch(p_msg->msg_class) {
	  case PDM_INTERNAL:
	  case PDM_EXPORT:
	    (void)pdm_copy(&pdm_copy_list[p_msg->msg_class & PDM_TYPE_MASK]);
	    break;
	    
	  case PDM_IMPORT:
	    /*do import marshalling*/
	    pcl = &pdm_copy_list[PDM_IMPORT & PDM_TYPE_MASK];
	    if (pdm_copy(pcl) != PDM_OK) {
		pi_disable_timeout(pcl->timeout_id);
		pcl->status = PDM_INIT;
		RECORD_M_EVENT("PDType changed, reconfig import");
		config_marsh(PDM_IMPORT);  /* reconfigure if e.g. ftf has changed */
	    }
	    break;

	  case TOPO_IND:
	    /* Topograhy indication */
	    RECORD_M_EVENT("Topography indication");
	    wtb_topo_ind((Type_Topography *)p_msg->param2);
	    break;
	    
	  default:
	    RECORD_M_EVENT("unknown message ignored");
	    break;
	}
	pi_post_semaphore(pdm_sema, &status);
    }
}



/*new page*/
/******************************************************************************
*
*   name:          pd_marshall
*
*   function:      Does the marshalling work for test purpose only!
*
*   input:         none
*
*   used globals:  pdm_stat
*                  internal_copy_list
*                  export_copy_list
*
*   output:        none
*
*   return value:  none
*
******************************************************************************/
void 
pd_marshall (void)
{
    short status;

    pi_pend_semaphore(pdm_sema, PI_FOREVER, &status); /* lock sema to access
							 pdm_stat            */
    if (pdm_stat.configured == FALSE)  {
	/*fatal error: pd_marshall can not be called before pdm is configured!*/
	pi_post_semaphore(pdm_sema, &status);
	return;
    }

    /*----------- INTERNAL ----------------------------------------------------*/
    (void)pdm_copy(&pdm_copy_list[PDM_INT]);
    
    /*---------------------------------- EXPORT -------------------------------*/
    (void)pdm_copy(&pdm_copy_list[PDM_EXP]);
    
    /*---------------------------------- IMPORT -------------------------------*/
    if (pdm_copy(&pdm_copy_list[PDM_IMP]) != PDM_OK)  {
	pi_disable_timeout(pdm_copy_list[PDM_IMP].timeout_id);
	pdm_copy_list[PDM_IMP].status = PDM_INIT;
	RECORD_M_EVENT("PDType changed, reconfig import");
	config_marsh(PDM_IMPORT);   /* reconfigure if e.g. ftf has changed */
    }
    pi_post_semaphore(pdm_sema, &status);
}


