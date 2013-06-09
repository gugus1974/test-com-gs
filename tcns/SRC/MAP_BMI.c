/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> write_serv() protype removed                                                        */
/* <ATR:02> p_err_text type fixed                                                               */
/* <ATR:03> (unmarked) SPrint_F() replaced with sprintf()                                       */
/* <ATR:04> removed the O_960 pragmas                                                           */
/* <ATR:05> p_event_text type fixed                                                             */
/* <ATR:06> RECORD_M_ERR() and RECORD_M_EVENT() macros fixed                                    */
/* <ATR:08> Include "atr_map.h" with UIC Mapping Server spacific patches                        */
/* <ATR:09> pi_enable() corrected to pi_unlock_task()                                           */
/* <ATR:10> map_user_ls_Report called if specified                                              */
/* <ATR:11> do not insert the MAP_FUNCTION_NR and FUNC_PROMOTION in the function directory      */
/* <ATR:12> added the fritting_disabled variable initialization                                 */
/* <ATR:13> can go back to LS_CONFIGURED with a LR_REMOVED indication                           */
/* <ATR:14> transmission_rate calculated from the BITRATE global                                */
/* <ATR:15> LL.H included for the BITRATE global definition                                     */
/* <ATR:16> pdm_wtb_topo_ind() routed on pdm_new_wtb_topo()                                     */
/* <ATR:17> PDM mode initialized from MAP configuration                                         */
/*==============================================================================================*/

#include "atr_map.h"			/* <ATR:08> */

/*
   000000408
   MAP_BMI
   Wolfgang Landwehr
   11. Oktober 1995, 16:01
   3

 */

/*
   GENERAL TOP OF FILE
 */

   /*
      Busmanagement interface for UIC mapping server.
    */

   /*
      17.03.95 M. Becker, W. Landwehr
      Start of developement.
    */

   /*
      Copyright (c) 1995 by ABB-Henschel
    */


/*
   MODUL HISTORY
 */

   /*
      2.01 17.03.95  M. Becker
      Adopted from version 2.0 of control interface MAP_TCN.C
      2.02 30.03.95  W. Landwehr
      Extended with queueing for parallel calls from WTB and MAP.
      2.03 05.04.95  W. Landwehr
      Separate cases for MAP_NEW_INAUGURATION in LS_REG_OPERATION_WEAK/STRONG.
      2.04 06.04.95  W. Landwehr
      Supplemented with several PDM calls {pdm_wtb_topo_ind}.
      2.05 10.04.95  W. Landwehr
      mbi_type added by STOP mapping.
      2.06 03.05.95  W. Landwehr
      Several adaptions to set NADI invalid.
      2.07 01.06.95  W. Landwehr
      Optimized some Task disable by change pi_disable with pi_lock_task
      after adaptions in RTP.
      2.08 11.10.95  W. Landwehr
      adoptions for MyFriend, remove DOS test features.
    */


/*
   COMPILER SWITCHES
 */

/* <ATR:04> */
// #ifdef O_960    
// #pragma noalign    
// #endif

/*
   INCLUDEFILES
 */
#include "AM_SYS.H"				/* RTP interface */
#include "PI_SYS.H"				/* PIL interface */
#include "BMI.H"				/* WTB busmanager interface */
#include "MAP.H"
#include "MAP_BMI.H"
#include "MAP_UMS.H"

/* <ATR:15> */
#include "ll.h"

#ifdef RECORD

#include "RECORDER.H"

#endif

#ifdef PDM

#include "LP_SYS.H"
#include "PDM_MAIN.H"

/* <ATR:16> */
#include "atr_pdm.h"
#define pdm_wtb_topo_ind pdm_new_wtb_topo

#endif

#ifdef EH

#include "eh_api.h"

#endif


/*
   IMPLEMENTATION
 */

   /*
      DEFINES
    */
#define MBI_WRONG_REPORT     1
#define MBI_WRONG_TCNTYPE    2
#define MBI_WRONG_STATE      3
#define MBI_MASTER_CONFLICT  4
#define MBI_ERROR_RETURN     5
#define MBI_ERROR_LSTGETTOPO 6

#define MAX_BMI_QUEUE        5

#ifdef O_960

	/* <ATR:06> */
#define RECORD_M_ERR( p_msg ) \
                            nse_record( "MAP_BMI", __LINE__, \
                            NSE_EVENT_TYPE_ERROR, \
                            NSE_ACTION_CONTINUE, \
                            strlen ( p_msg ) + 1, \
                            ( unsigned char *) p_msg )

	/* <ATR:06> */
#define RECORD_M_EVENT( p_msg ) \
                              nse_record( "MAP_BMI", __LINE__, \
                              NSE_EVENT_TYPE_SPECIAL, \
                              NSE_ACTION_CONTINUE, \
                              strlen ( p_msg ) + 1, \
                              ( unsigned char *) p_msg )

#endif


   /*
      EXTERNALS
    */
extern s_CommInfo map_ci;		/* interface map_npt.c, map_bmi.c */
extern map_TrainTopography NADI;	/* NADI addr. to set invalid */


   /*
      GLOBAL DECLARATIONS
    */
map_ctrl_field map_CTRL;		/* node configuration table */

Type_Configuration SVI_ConfigField;
Type_WTBStatus SVI_StatusField;
Type_Topography SVI_Topography;

unsigned short map_v_greater_5kmh;	/* 0xCCCC means each action disabled */

unsigned char map_mbi_ll_state;
unsigned char map_bmi_function_running;
short map_queue_ndx;
l_report map_bmi_queue[MAX_BMI_QUEUE];	/* Report queue for map_bmi */
l_result map_ls_result;

Type_Topography map_wtb_topo;	/* for terminal_report function */
l_report map_old_reason = 0x80;

unsigned char map_mbi_trace[256];
unsigned char map_mbi_trace_ndx;


extern char mvbon_flag;   /**** GWD 17/01/2001 */


   /*
      PROTOTYPES
    */
//    void       write_serv           (const char *);   /* <ATR:01> */

map_result map_init (void);
void map_CtrlReport (l_report);

void map_terminal_report (l_report);
void map_event_buf (l_report);

void map_ls_error (l_result);
void map_bmi_report (l_report);
void map_bmi_error (unsigned short);

void map_que_in_report (l_report);
void map_que_out_report (l_report *);


   /*
      SAMPLE OF FUNCTIONS
    */

	  /*
	     MAP_CTRLERRORREPORT
	   */

void 
map_CtrlErrorReport (l_report ergebnis)
{

#ifdef O_960

	RECORD_M_ERR ("Fatal ERROR - LinkLayer ErrorMessage");

#endif
}


	  /*
	     MAP_BMI_ERROR
	   */

void 
map_bmi_error (unsigned short error_code)
{

	switch (error_code) {

	case MBI_WRONG_REPORT:
		{
			write_serv ("^MAP_BMI err: wrong report !!!^");

			break;
		}

	case MBI_WRONG_TCNTYPE:
		{
			write_serv ("^MAP_BMI err: wrong TCNType !!!^");

			break;
		}

	case MBI_WRONG_STATE:
		{
			write_serv ("^MAP_BMI err: wrong state !!!^");

			break;
		}

	case MBI_MASTER_CONFLICT:
		{
			write_serv ("^MAP_BMI err: !! MASTER CONFLICT !! ^");

			break;
		}

	case MBI_ERROR_RETURN:
		{
			write_serv ("^MAP_BMI err: system configuration error^");

			break;
		}

	case MBI_ERROR_LSTGETTOPO:
		{
			write_serv ("^MAP_BMI err: ls_t_GetTopography error^");

			break;
		}
	}
	return;
}


	  /*
	     MAP_LS_ERROR
	   */

void 
map_ls_error (l_result error_code)
{
	char *p_err_text;			/* <ATR:02> */

	switch (error_code) {

	case L_BUSY:
		{
			write_serv ("^MAP_BMI err: L_BUSY");

			p_err_text = "L_BUSY";
			break;
		}

	case L_CALLING_SEQUENCE:
		{
			write_serv ("^MAP_BMI err: L_CALLING_SEQUENCE");

			p_err_text = "L_CALLING_SEQUENCE";
			break;
		}

	case L_MISSING_UDF:
		{
			write_serv ("^MAP_BMI err: L_MISSING_UDF");

			p_err_text = "L_MISSING_UDF";
			break;
		}

	case L_CONFIGURATION_INVALID:
		{
			write_serv ("^MAP_BMI err: L_CONFIGURATION_INVALID");

			p_err_text = "L_CONFIGURATION_INVALID";
			break;
		}
	default:{
			/* L_OK */
			p_err_text = 0;
		}
	}

	if (p_err_text != 0) {
		RECORD_M_EVENT (p_err_text);
	}
}


	  /*
	     MAP_TERMINAL_REPORT
	   */

void 
map_terminal_report (l_report reason)
{
	char buf[100] =
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	if (reason != map_old_reason) {
		map_old_reason = reason;

		switch (reason) {

		case LR_CONFIGURED:
			{
				write_serv ("^MAP_BMI report: LR_CONFIGURED^");
				break;
			}

		case LR_STRONG:
			{
				write_serv ("^MAP_BMI report: LR_STRONG (Strong Master)^");
				break;
			}

		case LR_WEAK:
			{
				write_serv ("^MAP_BMI report: LR_WEAK (weakened) ^");
				break;
			}

		case LR_SLAVE:
			{
				write_serv ("^MAP_BMI report: LR_SLAVE^");
				break;
			}

		case LR_PROMOTED:
			{
				write_serv ("^MAP_BMI report: LR_PROMOTED (Weak Master) ^");
				break;
			}

		case LR_NAMING_SUCCESSFUL:
			{

				if (ls_t_GetTopography (&map_wtb_topo) == 0) {
					sprintf (buf, "^MAP_BMI report: LR_NAMING_SUCCESSFUL ("
							 "Own addr.: %2d,  Number of nodes: %2d)^\n\r",
					map_wtb_topo.node_address, map_wtb_topo.number_of_nodes);

					write_serv (buf);
				}
				break;
			}

		case LR_NAMED:
			{

				if (ls_t_GetTopography (&map_wtb_topo) == 0) {
					sprintf (buf, "^MAP_BMI report: LR_NAMING_SUCCESSFUL ("
							 "Own addr.: %2d,  Number of nodes: %2d)^\n\r",
					map_wtb_topo.node_address, map_wtb_topo.number_of_nodes);
				}
				write_serv (buf);

				break;
			}

		case LR_REMOVED:
			{
				write_serv ("^MAP_BMI report: LR_REMOVED (configured) ^");
				break;
			}

		case LR_DEMOTED:
			{
				write_serv ("^MAP_BMI report: LR_DEMOTED (weak->slave) ^");
				break;
			}

		case LR_DISCONNECTION:
			{
				write_serv ("^MAP_BMI report: LR_DISCONNECTION (end node lost) ^");
				break;
			}

		case LR_INCLUDED:
			{
				write_serv ("^MAP_BMI report: LR_INCLUDED (inclusion) ^");
				break;
			}

		case LR_LENGTHENING:
			{
				write_serv ("^MAP_BMI report: LR_LENGTHENING ^");
				break;
			}

		case LR_DISRUPTION:
			{
				write_serv ("^MAP_BMI report: LR_DISRUPTION (end node lost) ^");
				break;
			}

		case LR_MASTER_CONFLICT:
			{
				write_serv ("^MAP_BMI report: LR_MASTER_CONFLICT ^");
				break;
			}

		case LR_NODE_STATUS:
			{
				write_serv ("^MAP_BMI report: LR_NODE_STATUS ^");
				break;
			}

		case LR_NAMING_FAILED:
			{
				write_serv ("^MAP_BMI report: LR_NAMING_FAILED ^");
				break;
			}

		case LR_NEW_TOPOGRAPHY:
			{

				if (ls_t_GetTopography (&map_wtb_topo) == 0) {
					sprintf (buf,
							 "^MAP_BMI report: LR_NAMING_SUCCESSFUL ("
							 "Own addr.: %2d,  Number of nodes: %2d)^\n\r",
					map_wtb_topo.node_address, map_wtb_topo.number_of_nodes);

					write_serv (buf);
				} else {
					write_serv (
								   "^MAP_BMI report: LR_NEW_TOPOGRAPHY ^");
				}
				break;
			}

		case LR_POLL_LIST_OVF:
			{
				write_serv ("^MAP_BMI report: LR_POLL_LIST_OVF ^");
				break;
			}

		case LR_ALLOWED:
			{
				write_serv ("^MAP_BMI report: LR_ALLOWED (can join network) ^");
				break;
			}

		case LR_INHIBITED:
			{
				write_serv ("^MAP_BMI report: LR_INHIBITED (can't join network) ^");
				break;
			}

		case MAP_NEW_INAUGURATION:
			{
				write_serv ("^MAP_BMI report: MAP_NEW_INAUGURATION ^");
				break;
			}
		default:{
				write_serv ("^MAP_BMI report: UNEXPECTED REPORT^");
				break;
			}
		}
	}
}


	  /*
	     MAP_EVENT_BUF
	   */

void 
map_event_buf (l_report report)
{
	char *p_event_text;			/* <ATR:05> */

#ifdef RECORD

	map_mbi_trace[map_mbi_trace_ndx++] = map_mbi_ll_state;

	map_mbi_trace[map_mbi_trace_ndx++] = report;

	switch (map_mbi_ll_state) {

	case LS_INITIALIZED:
		{
			p_event_text = "LS_INITIALIZED";
			break;
		}

	case LS_CONFIGURED:
		{
			p_event_text = "LS_CONFIGURED";
			break;
		}

	case LS_READY_TO_NAME:
		{
			p_event_text = "LS_READY_TO_NAME";
			break;
		}

	case LS_READY_TO_BE_NAMED:
		{
			p_event_text = "LS_READY_TO_BE_NAMED";
			break;
		}

	case LS_REG_OPERATION_SLAVE:
		{
			p_event_text = "LS_REG_OPERATION_SLAVE";
			break;
		}

	case LS_REG_OPERATION_WEAK:
		{
			p_event_text = "LS_REG_OPERATION_WEAK";
			break;
		}

	case LS_REG_OPERATION_STRONG:
		{
			p_event_text = "LS_REG_OPERAION_STRONG";
			break;
		}

	case LS_INHIBITED:
		{
			p_event_text = "LS_INHIBITED";
			break;
		}
	default:{
			p_event_text = "WRONG_STATE";
		}
	}
	RECORD_M_EVENT (p_event_text);

	switch (report) {

	case LR_CONFIGURED:
		{
			p_event_text = "LR_CONFIGURED";
			break;
		}

	case LR_STRONG:
		{
			p_event_text = "LR_STRONG";
			break;
		}

	case LR_SLAVE:
		{
			p_event_text = "LR_SLAVE";
			break;
		}

	case LR_WEAK:
		{
			p_event_text = "LR_WEAK";
			break;
		}

	case LR_PROMOTED:
		{
			p_event_text = "LR_PROMOTED";
			break;
		}

	case LR_NAMING_SUCCESSFUL:
		{
			p_event_text = "LR_NAMING_SUCCESSFUL";
			break;
		}

	case LR_NAMED:
		{
			p_event_text = "LR_NAMED";
			break;
		}

	case LR_REMOVED:
		{
			p_event_text = "LR_REMOVED";
			break;
		}

	case LR_DEMOTED:
		{
			p_event_text = "LR_DEMOTED";
			break;
		}

	case LR_DISCONNECTION:
		{
			p_event_text = "LR_DISCONNECTION";
			break;
		}

	case LR_INHIBITED:
		{
			p_event_text = "LR_INHIBITED";
			break;
		}

	case LR_LENGTHENING:
		{
			p_event_text = "LR_LENGTHENING";
			break;
		}

	case LR_INCLUDED:
		{
			p_event_text = "LR_INCLUDED";
			break;
		}

	case LR_DISRUPTION:
		{
			p_event_text = "LR_DISRUPTION";
			break;
		}

	case LR_MASTER_CONFLICT:
		{
			p_event_text = "LR_MASTER_CONFLICT";
			break;
		}

	case LR_NAMING_FAILED:
		{
			p_event_text = "LR_NAMING_FAILED";
			break;
		}

	case LR_NEW_TOPOGRAPHY:
		{
			p_event_text = "LR_NEW_TOPOGRAPHY";
			break;
		}

	case LR_POLL_LIST_OVF:
		{
			p_event_text = "LR_POLL_LIST_OVF";
			break;
		}

	case LR_ALLOWED:
		{
			p_event_text = "LR_ALLOWED";
			break;
		}

	case LR_NODE_STATUS:
		{
			p_event_text = "LR_NODE_STATUS";
			break;
		}

	case MAP_NEW_INAUGURATION:
		{
			p_event_text = "MAP_NEW_INAUGURATION";
			break;
		}
	default:{
			p_event_text = "WRONG_REPORT";
		}
	}
	RECORD_M_EVENT (p_event_text);

#endif
}



   /*
      FUNCTIONS OF THE CONTROL INTERFACE
    */

	  /*
	     MAP_INIT
	   */

map_result 
map_init (void)
{
/* <ATR:11> */
// #define       FUNC_PROMOTION   14 /* #kör */
//             AM_DIR_ENTRY aw_dir_entries[2];
//             aw_dir_entries[ 0 ].function = MAP_FUNCTION_NR;
//             aw_dir_entries[ 0 ].device   = AM_SAME_DEVICE;
//             aw_dir_entries[ 1 ].function = FUNC_PROMOTION;
//             aw_dir_entries[ 1 ].device   = AM_SAME_DEVICE;
//             am_insert_dir_entries( aw_dir_entries, 2 );    

	/* common interface between MAP_BMI.C and MAP_NPT.C */
	map_ci.bmi_event = 0;
	map_ci.bmi_type = 0;
	map_ci.p_VD = &map_CTRL.map_config.vehicle_descriptor;
	map_ci.p_TT = &SVI_Topography;
	map_ci.NodeKonf = &map_CTRL.map_config;

	for (map_queue_ndx = 0; map_queue_ndx < MAX_BMI_QUEUE; map_queue_ndx++) {
		map_bmi_queue[map_queue_ndx] = 0;	/* Report values */
	}
	return MAP_OK;
}


	  /*
	     MAP_INSTALL
	   */

map_result 
map_install (map_report new_proc_rep,
			 map_report * old_proc_rep,
			 map_error new_proc_err,
			 map_error * old_proc_err)
{

	if (new_proc_rep != 0) {
		*old_proc_rep = map_CTRL.map_reportF;
		map_CTRL.map_reportF = new_proc_rep;
	}
	if (new_proc_err != 0) {
		*old_proc_err = map_CTRL.map_errorF;
		map_CTRL.map_errorF = new_proc_err;
	}
	return MAP_OK;
}


	  /*
	     MAP_CONFIGURE
	   */

map_result 
map_configure (map_NodeConfigurationTable * p_ConfigField)
{
	
	
	
	map_CTRL.map_config = *p_ConfigField;	/* copy user configuration */

	/*
	   initialize configuration field for link layer (SVI_ConfigField)
	 */

	/* <ATR:14> */ SVI_ConfigField.transmission_rate = BITRATE / 1000; /* WTB */

	SVI_ConfigField.basic_period = map_CTRL.map_config.configuration.basic_period;

/* <ATR:12> */ SVI_ConfigField.fritting_disabled = 0;

	SVI_ConfigField.node_descriptor.node_frame_size =
		map_CTRL.map_config.configuration.node_descriptor.node_frame_size;

	SVI_ConfigField.node_descriptor.node_period =
		map_CTRL.map_config.configuration.node_descriptor.node_period;

	SVI_ConfigField.node_descriptor.node_key.node_type =
		map_CTRL.map_config.configuration.node_descriptor.node_key.node_type;

	SVI_ConfigField.node_descriptor.node_key.node_version =
		map_CTRL.map_config.configuration.node_descriptor.node_key.node_version;

	SVI_ConfigField.poll_md_when_idle = 0;
	SVI_ConfigField.sink_port_count = 22;
	SVI_ConfigField.source_port_count = 1;
	SVI_ConfigField.port_size = 128;

	SVI_ConfigField.p_traffic_store =
		(void *) (map_CTRL.map_config.configuration.ts_descriptor);

	SVI_ConfigField.Report = map_CtrlReport;
	SVI_ConfigField.ErrorReport = map_CtrlErrorReport;

	map_mbi_ll_state = LS_INITIALIZED;

	/*
	   call REPORT function
	 */

/* <ATR:17> */
#ifdef PDM
	pdm_set_node_mode(node_mode_from_vhdes(&p_ConfigField->vehicle_descriptor));
#endif

	if (map_CTRL.map_reportF != 0) {
		map_CTRL.map_reportF (MAP_NODE_STARTED);
	}
	
	if (!mvbon_flag)   /**** GWD 17/01/2001 */
	{
	    if (ls_t_Configure (&SVI_ConfigField) != L_OK) {
		   map_bmi_error (MBI_ERROR_RETURN);
	    }
	}
	else printf("MVB SENZA WTB !\n"); 
	
	return MAP_OK;
}


	  /*
	     MAP_RESET
	   */

map_result 
map_reset (void)
{
	l_result result;

	/* result = ls_t_Reset();   not allowed in this case */

	if (map_CTRL.map_reportF != 0) {
		/* map_CTRL.map_reportF( MAP_TCN_CONFIGURED ); */
	}
	return MAP_NP;
}


	  /*
	     MAP_GET_TOPOGRAPHY
	   */

map_result 
map_get_topography (Type_Topography * p_topography,
					unsigned short *own_address)
{
	l_result result;
	Type_WTBStatus SVI_Status;
	Type_WTBStatus *p_SVI_STAT;
	map_result m_result;

	p_SVI_STAT = &SVI_Status;

	if ((map_mbi_ll_state == LS_REG_OPERATION_STRONG) ||
		(map_mbi_ll_state == LS_REG_OPERATION_WEAK) ||
		(map_mbi_ll_state == LS_REG_OPERATION_SLAVE)) {
		result = ls_t_GetTopography (p_topography);

		m_result = ls_t_GetStatus (p_SVI_STAT);

		if ((result == L_OK) && (m_result == L_OK)) {
			*own_address = p_SVI_STAT->node_address;

			m_result = MAP_OK;
		} else {
			m_result = MAP_NP;
		}
	} else {
		m_result = MAP_NP;
	}
	return m_result;
}



   /*
      MAP_BMI_REPORT
    */

void 
map_bmi_report (l_report report)
{
	unsigned char my_type;
	unsigned char *p_event_text;

	if (report == LR_REMOVED)
		map_mbi_ll_state = LS_INITIALIZED;	/* <ATR:13> */

	/*
	   terminal output, trac state and write to event recorder
	 */

#ifdef TERM_OUT

	map_terminal_report (report);

#endif

#ifdef RECORD

	map_event_buf (report);

#endif


	switch (map_mbi_ll_state) {

		/*
		   LS_INITIALIZED
		 */

	case LS_INITIALIZED:
		{

			if (report == LR_CONFIGURED || report == LR_REMOVED)	/* <ATR:13> */
//          if (report == LR_CONFIGURED)
			{
				map_mbi_ll_state = LS_CONFIGURED;

				my_type = map_CTRL.map_config.vehicle_descriptor.TCNType;

				/*
				   PDM connection 0 nodes
				 */

#ifdef PDM

				map_ci.p_TT->number_of_nodes = 0;

				pdm_wtb_topo_ind (map_ci.p_TT);

#endif


				if (my_type == L_STRONG) {
					ls_t_SetStrong ();
				} else {

					if (my_type == L_WEAK) {
						ls_t_SetWeak ();
					} else {

						if (my_type == L_SLAVE) {
							ls_t_SetSlave ();
						} else {
							map_bmi_error (MBI_WRONG_TCNTYPE);
						}
					}
				}
			} else {
				map_bmi_error (MBI_WRONG_REPORT);
			}
			break;
		}


		/*
		   LS_CONFIGURED
		 */

	case LS_CONFIGURED:
		{

			switch (report) {

				/*
				   strong, slave, inhibited, included
				 */

			case LR_STRONG:
				{
					map_mbi_ll_state = LS_READY_TO_NAME;

					ls_t_StartNaming ();

					break;
				}

			case LR_SLAVE:
				{
					map_mbi_ll_state = LS_READY_TO_BE_NAMED;

					break;
				}

			case LR_INHIBITED:
				{
					map_mbi_ll_state = LS_INHIBITED;

					break;
				}

			case LR_INCLUDED:
				{

					if ((ls_t_GetTopography (map_ci.p_TT) == L_OK) &&
						(ls_t_GetStatus (&SVI_StatusField) == L_OK)) {
						map_ci.NODEstate = SVI_StatusField;

						map_ci.bmi_type = L_SLAVE;
						map_ci.bmi_event = START_INCLUDE;

						/* NADI is invalid now! */
						NADI.state.validity = MAP_NADI_INVALID;

						/*
						   PDM connection
						 */

#ifdef PDM

						pdm_wtb_topo_ind (map_ci.p_TT);

#endif


						/*
						   call REPORT function
						 */

						if (map_CTRL.map_reportF != 0) {
							map_CTRL.map_reportF (MAP_TCN_CONFIGURED);
						}
					} else {
						map_bmi_error (MBI_ERROR_LSTGETTOPO);
					}
					map_mbi_ll_state = LS_REG_OPERATION_SLAVE;

					break;
				}

			default:{
					map_bmi_error (MBI_WRONG_REPORT);
				}
			}
			break;
		}


		/*
		   LS_READY_TO_NAME
		 */

	case LS_READY_TO_NAME:
		{

			if (report == LR_NAMING_SUCCESSFUL) {

				/*
				 */
				map_mbi_ll_state = LS_REG_OPERATION_STRONG;

				if ((ls_t_GetTopography (map_ci.p_TT) == L_OK) &&
					(ls_t_GetStatus (&SVI_StatusField) == L_OK)) {
					map_ci.NODEstate = SVI_StatusField;

					/* activate MAPPING SERVER */
					map_ci.bmi_type = L_STRONG;
					map_ci.bmi_event = START_INAUGURATION;

					/* NADI is invalid now! */
					NADI.state.validity = MAP_NADI_INVALID;

					/*
					   PDM connection
					 */

#ifdef PDM

					pdm_wtb_topo_ind (map_ci.p_TT);

#endif


					/*
					   call REPORT function
					 */

					if (map_CTRL.map_reportF != 0) {
						map_CTRL.map_reportF (MAP_TCN_CONFIGURED);
					}
				}
			} else {

				if (report == LR_NAMING_FAILED) {
					/* NADI is invalid now! */
					NADI.state.validity = MAP_NADI_INVALID;

					/*
					   PDM connection: 0 nodes
					 */

#ifdef PDM

					map_ci.p_TT->number_of_nodes = 0;

					pdm_wtb_topo_ind (map_ci.p_TT);

#endif


					/*
					   call REPORT function
					 */

					if (map_CTRL.map_reportF != 0) {
						map_CTRL.map_reportF (MAP_NEW_CONFIGURATION);
					}
					map_ls_result = ls_t_StartNaming ();

					/* error report to term and event buffer */
					map_ls_error (map_ls_result);
				} else {

					if (report == LR_MASTER_CONFLICT) {
						/* if necessary with timeout counter */
						map_bmi_error (MBI_MASTER_CONFLICT);
					} else {
						map_bmi_error (MBI_WRONG_REPORT);
					}
				}
			}
			break;
		}


		/*
		   LS_READY_TO_BE_NAMED
		 */

	case LS_READY_TO_BE_NAMED:
		{

			switch (report) {

				/*
				   named, 
				   promoted, 
				   naming successful,
				   demoted, 
				   naming failed  
				 */

			case LR_NAMED:
				{
					map_mbi_ll_state = LS_REG_OPERATION_SLAVE;

					if ((ls_t_GetTopography (map_ci.p_TT) == L_OK) &&
						(ls_t_GetStatus (&SVI_StatusField) == L_OK)) {
						map_ci.NODEstate = SVI_StatusField;

						map_ci.bmi_type = L_SLAVE;
						map_ci.bmi_event = START_INAUGURATION;	/* activate MAPPING SERVER */

						/* NADI is invalid now! */
						NADI.state.validity = MAP_NADI_INVALID;

						/*
						   PDM connection
						 */

#ifdef PDM

						pdm_wtb_topo_ind (map_ci.p_TT);

#endif


						/*
						   call REPORT function
						 */

						if (map_CTRL.map_reportF != 0) {
							map_CTRL.map_reportF (MAP_TCN_CONFIGURED);
						}
					}
					break;
				}

			case LR_PROMOTED:
				{
					/* weak master node starts naming, this needs a certain time */

					break;
				}

			case LR_NAMING_SUCCESSFUL:
				{
					map_mbi_ll_state = LS_REG_OPERATION_WEAK;

					if ((ls_t_GetTopography (map_ci.p_TT) == L_OK) &&
						(ls_t_GetStatus (&SVI_StatusField) == L_OK)) {
						map_ci.NODEstate = SVI_StatusField;

						/* activate MAPPING SERVER */
						map_ci.bmi_type = L_WEAK;
						map_ci.bmi_event = START_INAUGURATION;

						/* NADI is invalid now! */
						NADI.state.validity = MAP_NADI_INVALID;

						/*
						   PDM connection
						 */

#ifdef PDM

						pdm_wtb_topo_ind (map_ci.p_TT);

#endif


						/*
						   call REPORT function
						 */

						if (map_CTRL.map_reportF != 0) {
							map_CTRL.map_reportF (MAP_TCN_CONFIGURED);
						}
					}
					break;
				}

			case LR_DEMOTED:
				{
					/* weak master detected a stronger master while naming */

					break;
				}

			case LR_NAMING_FAILED:
				{
					NADI.state.validity = MAP_NADI_INVALID;		/* NADI is invalid now! */

					/*
					   PDM connection 0 nodes
					 */

#ifdef PDM

					map_ci.p_TT->number_of_nodes = 0;

					pdm_wtb_topo_ind (map_ci.p_TT);

#endif


					/*
					   call REPORT function
					 */

					if (map_CTRL.map_reportF != 0) {
						map_CTRL.map_reportF (MAP_TCN_CONFIGURED);
					}
					break;
				}

			default:{
					map_bmi_error (MBI_WRONG_REPORT);
				}
			}
			break;
		}


		/*
		   LS_REG_OPERATION_SLAVE
		 */

	case LS_REG_OPERATION_SLAVE:
		{

			switch (report) {

				/*
				   new topography,
				   disconnection
				 */

			case LR_NEW_TOPOGRAPHY:
				{

					if ((ls_t_GetTopography (map_ci.p_TT) == L_OK) &&
						(ls_t_GetStatus (&SVI_StatusField) == L_OK)) {
						map_ci.NODEstate = SVI_StatusField;

						/* activate MAPPING SERVER */
						map_ci.bmi_type = L_SLAVE;
						map_ci.bmi_event = START_INAUGURATION;

						/* NADI is invalid now */
						NADI.state.validity = MAP_NADI_INVALID;

						/*
						   PDM connection
						 */

#ifdef PDM

						pdm_wtb_topo_ind (map_ci.p_TT);

#endif


						/*
						   call REPORT function
						 */

						if (map_CTRL.map_reportF != 0) {
							map_CTRL.map_reportF (MAP_TCN_CONFIGURED);
						}
					} else {
						map_bmi_error (MBI_ERROR_LSTGETTOPO);
					}
					break;
				}

			case LR_DISCONNECTION:
				{
					map_mbi_ll_state = LS_READY_TO_BE_NAMED;

					/* deactivate MAPPING SERVER */
					map_ci.bmi_type = L_SLAVE;
					map_ci.bmi_event = STOP;

					/* NADI is invalid now */
					NADI.state.validity = MAP_NADI_INVALID;

					/*
					   PDM connection 0 nodes
					 */

#ifdef PDM

					map_ci.p_TT->number_of_nodes = 0;

					pdm_wtb_topo_ind (map_ci.p_TT);

#endif


					/*
					   call REPORT function
					 */

					if (map_CTRL.map_reportF != 0) {
						map_CTRL.map_reportF (MAP_NEW_CONFIGURATION);
					}
					break;
				}

			default:{
					map_bmi_error (MBI_WRONG_REPORT);
				}
			}
			break;
		}


		/*
		   LS_REG_OPERATION_WEAK
		 */

	case LS_REG_OPERATION_WEAK:
		{

			switch (report) {

				/*
				   new topography,
				   lengthening, disruption,
				   naming failed
				   demoted,
				   naming successful,
				   node status,
				   new_inauguration
				 */

			case LR_NEW_TOPOGRAPHY:
				{

					if ((ls_t_GetTopography (map_ci.p_TT) == L_OK) &&
						(ls_t_GetStatus (&SVI_StatusField) == L_OK)) {
						map_ci.NODEstate = SVI_StatusField;

						map_ci.bmi_type = L_WEAK;
						map_ci.bmi_event = START_INAUGURATION;	/* activate MAPPING SERVER */
						NADI.state.validity = MAP_NADI_INVALID;		/* NADI is invalid now! */

						/*
						   PDM connection
						 */

#ifdef PDM

						pdm_wtb_topo_ind (map_ci.p_TT);

#endif


						/*
						   call REPORT function
						 */

						if (map_CTRL.map_reportF != 0) {
							map_CTRL.map_reportF (MAP_TCN_CONFIGURED);
						}
					} else {
						map_bmi_error (MBI_ERROR_LSTGETTOPO);
					}
					break;
				}

			case LR_LENGTHENING:
			case LR_DISRUPTION:
				{
					map_ci.bmi_type = L_WEAK;
					map_ci.bmi_event = STOP;
					NADI.state.validity = MAP_NADI_INVALID;		/* NADI is invalid now! */

					/*
					   PDM connected
					 */

#ifdef PDM

					map_ci.p_TT->number_of_nodes = 0;

					pdm_wtb_topo_ind (map_ci.p_TT);

#endif


					/*
					   call REPORT function
					 */

					if (map_CTRL.map_reportF != 0) {
						map_CTRL.map_reportF (MAP_NEW_CONFIGURATION);
					}
					break;
				}

			case LR_NAMING_FAILED:
				{
					map_mbi_ll_state = LS_READY_TO_BE_NAMED;

					break;
				}

			case LR_DEMOTED:
				{

					/*
					   PDM connection 0 nodes
					 */

#ifdef PDM

					map_ci.p_TT->number_of_nodes = 0;

					pdm_wtb_topo_ind (map_ci.p_TT);

#endif


					/*
					   call REPORT function
					 */

					if (map_CTRL.map_reportF != 0) {
						map_CTRL.map_reportF (MAP_NEW_CONFIGURATION);
					}
					map_mbi_ll_state = LS_READY_TO_BE_NAMED;

					break;
				}

			case LR_NAMING_SUCCESSFUL:
				{

					if ((ls_t_GetTopography (map_ci.p_TT) == L_OK) &&
						(ls_t_GetStatus (&SVI_StatusField) == L_OK)) {
						map_ci.NODEstate = SVI_StatusField;

						/* activate MAPPING SERVER */
						map_ci.bmi_type = L_WEAK;
						map_ci.bmi_event = START_INAUGURATION;
						/* NADI is invalid now! */
						NADI.state.validity = MAP_NADI_INVALID;


						/*
						   PDM connection
						 */

#ifdef PDM

						pdm_wtb_topo_ind (map_ci.p_TT);

#endif


						/*
						   call REPORT function
						 */

						if (map_CTRL.map_reportF != 0) {
							map_CTRL.map_reportF (MAP_TCN_CONFIGURED);
						}
					} else {
						map_bmi_error (MBI_ERROR_LSTGETTOPO);
					}
					break;
				}

			case LR_NODE_STATUS:
				{
					break;
				}

			case MAP_NEW_INAUGURATION:
				{

					/*
					   This report type is delivered by map_npt (MAPPING SERVER)
					 */
					map_ci.bmi_type = L_WEAK;
					map_ci.bmi_event = STOP;
					NADI.state.validity = MAP_NADI_INVALID;		/* NADI is invalid now! */

					/*
					   PDM connection 0 nodes
					 */

#ifdef PDM

					map_ci.p_TT->number_of_nodes = 0;

					pdm_wtb_topo_ind (map_ci.p_TT);

#endif


					/*
					   call REPORT function
					 */

					if (map_CTRL.map_reportF != 0) {
						/* only if the report function is installed */
						map_CTRL.map_reportF (MAP_NEW_CONFIGURATION);
					}
					map_ls_result = ls_t_StartNaming ();	/* later with timeout counter */

					map_ls_error (map_ls_result);	/* error report to term and event buffer */

					break;
				}

			default:{
					map_bmi_error (MBI_WRONG_REPORT);
				}
			}
			break;
		}


		/*
		   LS_REG_OPERATION_STRONG
		 */

	case LS_REG_OPERATION_STRONG:
		{

			switch (report) {

				/*
				   lengthening, disruption,
				   naming successful,
				   new topography,
				   node status,
				   demoted
				   naming failed
				 */

			case LR_LENGTHENING:
			case LR_DISRUPTION:
				{
					map_ci.bmi_type = L_STRONG;
					map_ci.bmi_event = STOP;

					/* NADI is invalid now! */
					NADI.state.validity = MAP_NADI_INVALID;

					/*
					   PDM connection 0 nodes
					 */

#ifdef PDM

					map_ci.p_TT->number_of_nodes = 0;

					pdm_wtb_topo_ind (map_ci.p_TT);

#endif


					/*
					   call REPORT function
					 */

					if (map_CTRL.map_reportF != 0) {
						map_CTRL.map_reportF (MAP_NEW_CONFIGURATION);
					}
					map_ls_result = ls_t_StartNaming ();

					map_ls_error (map_ls_result);	/* error report to term and event buffer */

					break;
				}

			case LR_NEW_TOPOGRAPHY:
				{

					if ((ls_t_GetTopography (map_ci.p_TT) == L_OK) &&
						(ls_t_GetStatus (&SVI_StatusField) == L_OK)) {
						map_ci.NODEstate = SVI_StatusField;

						/* activate MAPPING SERVER */
						map_ci.bmi_type = L_STRONG;
						map_ci.bmi_event = START_INAUGURATION;

						/* NADI is invalid now! */
						NADI.state.validity = MAP_NADI_INVALID;

						/*
						   PDM connection
						 */

#ifdef PDM

						pdm_wtb_topo_ind (map_ci.p_TT);

#endif


						/*
						   call REPORT function
						 */

						if (map_CTRL.map_reportF != 0) {
							map_CTRL.map_reportF (MAP_TCN_CONFIGURED);
						}
					} else {
						map_bmi_error (MBI_ERROR_LSTGETTOPO);
					}
					break;
				}

			case LR_MASTER_CONFLICT:
				{
					map_bmi_error (MBI_MASTER_CONFLICT);

					break;
				}

			case LR_NAMING_SUCCESSFUL:
				{

					if ((ls_t_GetTopography (map_ci.p_TT) == L_OK) &&
						(ls_t_GetStatus (&SVI_StatusField) == L_OK)) {
						map_ci.NODEstate = SVI_StatusField;

						/* activate MAPPING SERVER */
						map_ci.bmi_type = L_STRONG;
						map_ci.bmi_event = START_INAUGURATION;

						/* NADI is invalid now! */
						NADI.state.validity = MAP_NADI_INVALID;

						/*
						   PDM connection
						 */

#ifdef PDM

						pdm_wtb_topo_ind (map_ci.p_TT);

#endif


						/*
						   call REPORT function
						 */

						if (map_CTRL.map_reportF != 0) {
							map_CTRL.map_reportF (MAP_TCN_CONFIGURED);
						}
					} else {
						map_bmi_error (MBI_ERROR_LSTGETTOPO);
					}
					break;
				}

			case LR_NODE_STATUS:
				{
					break;
				}

			case LR_NAMING_FAILED:
				{
					map_ci.bmi_type = L_STRONG;
					map_ci.bmi_event = STOP;

					/* NADI is invalid now! */
					NADI.state.validity = MAP_NADI_INVALID;

					/*
					   PDM connection 0 nodes
					 */

#ifdef PDM

					map_ci.p_TT->number_of_nodes = 0;

					pdm_wtb_topo_ind (map_ci.p_TT);

#endif


					/*
					   call REPORT function
					 */

					if (map_CTRL.map_reportF != 0) {
						map_CTRL.map_reportF (MAP_NEW_CONFIGURATION);
					}
					map_ls_result = ls_t_StartNaming ();	/* later with timeout counter */

					map_ls_error (map_ls_result);	/* error report to term and event buffer */

					break;
				}

			case MAP_NEW_INAUGURATION:
				{
					map_ci.bmi_type = L_STRONG;
					map_ci.bmi_event = STOP;

					/* NADI is invalid now! */
					NADI.state.validity = MAP_NADI_INVALID;

					/*
					   PDM connection 0 nodes
					 */

#ifdef PDM

					map_ci.p_TT->number_of_nodes = 0;

					pdm_wtb_topo_ind (map_ci.p_TT);

#endif


					/*
					   call REPORT function
					 */

					if (map_CTRL.map_reportF != 0) {
						map_CTRL.map_reportF (MAP_NEW_CONFIGURATION);
					}
					map_ls_result = ls_t_StartNaming ();

					map_ls_error (map_ls_result);	/* error report to term and event buffer */

					break;
				}

			default:{
					map_bmi_error (MBI_WRONG_REPORT);
				}
			}
			break;
		}


		/*
		   LS_INHIBITED
		 */

	case LS_INHIBITED:
		{

			if (report == LR_ALLOWED) {
				map_mbi_ll_state = LS_READY_TO_BE_NAMED;
			} else {
				map_bmi_error (MBI_WRONG_REPORT);
			}
			break;
		}

	default:{
			map_bmi_error (MBI_WRONG_STATE);
		}
	}
}


   /*
      MAP_CTRLREPORT
    */

void 
map_CtrlReport (l_report report)
{
	l_report x_report;

	/* <ATR:10> */
	if (map_user_ls_Report)
		(*map_user_ls_Report) (report);

	if (map_v_greater_5kmh != 0xCCCC) {

		/*
		   HANDLE THE PENDING REPORTS
		 */
		pi_lock_task ();		/* disable Task-IR */

		/*
		   QUEUEING IN A REPORT
		 */
		map_queue_ndx = map_bmi_queue[0];

		if (++map_queue_ndx >= MAX_BMI_QUEUE) {
			map_queue_ndx = 1;	/* 0 is write index */
		}
		map_bmi_queue[map_queue_ndx] = report;

		/* index pointed always onto the current element */
		map_bmi_queue[0] = map_queue_ndx;


		if (map_bmi_function_running) {
			/* queue will worked through in else path */
			pi_unlock_task ();
		} else {
			map_bmi_function_running = TRUE;

			pi_unlock_task ();	/* <ATR:09> pi_enable(); */

			while (TRUE) {		/* until no report is pending */
				pi_lock_task ();	/* disable Task-IR */

				/*
				   QUEUEING OUT A REPORT
				 */

				/*
				   Atentione:
				   this function must unconditional run uneder pi_disable/pi_enable.
				 */

				for (map_queue_ndx = 1; map_queue_ndx < MAX_BMI_QUEUE; map_queue_ndx++) {
					x_report = map_bmi_queue[map_queue_ndx];

					if (x_report != 0) {
						map_bmi_queue[map_queue_ndx] = 0;

						break;
					}
				}


				if (x_report != 0) {
					pi_unlock_task ();

					/* the real report function */
					map_bmi_report (x_report);
				} else {
					map_bmi_function_running = FALSE;

					/* queueing in starts always from bottom */
					map_queue_ndx = MAX_BMI_QUEUE;

					pi_unlock_task ();

					break;		/* leave now */
				}
			}
		}

	} else {
		RECORD_M_EVENT ("v > 5kmh, no BMI control allowed!");

		map_event_buf (report);
	}
}
