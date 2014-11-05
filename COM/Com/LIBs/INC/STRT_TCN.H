/*==============================================================================================*/
/* TCN startup			    														            */
/* Header file (strt_tcn.h)     													            */
/* 																					            */
/* Version 3.9																		            */
/*																					            */
/* (c)1996-9 Ansaldo Trasporti														            */
/* Written by Giuseppe Carnevale													            */
/*																					            */
/* Version history:																	            */
/* 1.2 (17/10/96) : added the STRT_SIO_ASC0_BG macro								            */
/* 1.3 (30/10/96) : changed the file name											            */
/* 1.4 (13/11/96) : updated to the changes in conf_mvb; removed the STRT_HAS_BA switch          */
/* 1.5 (05/12/96) : added the STRT_DEFINE_NULL_MVBC_TSK_CONF and                                */
/*                    STRT_RTX166_ROUND_ROBIN switches                                          */
/* 1.6 (16/12/96) : removed: START_MVBC_MONITOR_TASK_ID, STRT_DEFINE_NULL_MVBC_TSK_CONF;        */
/*                  added  : STRT_SHELL_TASK_ID, STRT_SHELL_TASK_PRIO,                          */
/*                           STRT_SHELL_PROMPT, STRT_INSTALL_MVBC_MONITOR                       */
/* 2.0 (14/12/97) : WTB compatibility (Network Configuration)                                   */
/*                  removed: STRT_DA_OVERRIDE, STRT_SHELL_PROMPT, get_tcn_address(),            */
/*							 STRT_BA_EVENT_POLL_STRATEGY, STRT_BA_MACROS_IN_TURN,               */
/*							 STRT_BA_SCAN_STRATEGY, STRT_INSTALL_MVBC_MONITOR,                  */
/*                           STRT_AM_MAX_CALL_NUMBER, STRT_AM_MAX_INST_NUMBER,                  */
/*                           STRT_AM_DEFAULT_REPLY_TMO, STRT_AM_MY_CREDIT                       */
/*                  added  : get_station_id(), STRT_AUTO_ST_ID, STRT_INSTALL_TCN_MONITOR,       */
/*                           STRT_ROUTINE_NAME                                                  */
/* 2.1 (12/02/97) : Multiple configuration compatibility                                        */
/*                  added  : STRT_EXTERN_NCDB_ADDRESS                                           */
/* 2.2 (02/04/97) : Network Management compatibility                                            */
/* 2.3 (03/04/97) : Version string added                                                        */
/*                  added  : STRT_PROJECT_STRING                                                */
/* 2.4 (16/05/97) : Improved the error management                                               */
/* 2.5 (23/05/97) : Better support for no-TCN systems                                           */
/*                  Renamed STRT_INSTALL_TCN_MONITOR to STRT_INSTALL_MONITORS                   */
/* 2.6 (05/06/97) : RTX-166 user error handler moved in its PIL                                 */
/* 2.7 (25/08/97) : STRT_SIO_ASC0_BG replaced with STRT_SIO_BAUD_RATE                           */
/* 3.0 (27/08/97) : VRTX compatibility                                                          */
/*                  added: STRT_APPLICATION_TASK_PRIO                                           */
/* 3.1 (23/12/97) : fixed the STRT_PIL_SYSPOOL_SIZE for VRTX; modified for WTB support          */
/*                  added: STRT_BMI_TASK_ID, STRT_PDM_TASK_ID, STRT_MAP_TASK_ID,                */
/*                         STRT_BMI_TASK_PRIO, STRT_PDM_TASK_PRIO, STRT_MAP_TASK_PRIO           */
/* 3.2 (18/03/98) : added: STRT_GTW_MODE_ID, STRT_NO_USER_LS_REPORT, STRT_NO_USER_MAP_REPORT,   */
/*                         get_gateway_mode_id(), user_ls_report(), user_map_report()           */
/* 3.3 (25/03/98) : added: STRT_MAP                                                             */
/* 3.4 (27/04/98) : added: STRT_PDM_EXPORT_MODE, STRT_PDM_IMPORT_MODE, STRT_PDM_INTERNAL_MODE   */
/*                  removed: get_gateway_mode_id(), STRT_GTW_MODE_ID                            */
/* 3.5 (11/05/98) : added: application_version, library_version                                 */
/*                  removed: version_string()                                                   */
/* 3.6 (30/09/98) : added: STRT_EXT_NCDB_ENABLED; UMS compatibility                             */
/*                  removed: STRT_PDM_EXPORT_MODE, STRT_PDM_IMPORT_MODE, STRT_PDM_INTERNAL_MODE */
/* 3.8 (04/03/99) : added: STRT_RTX166_NO_ALLOC_IN_ISR, strt_tcn_configured, strt_tcn_failure,  */
/*                         STRT_NO_USER_BA_REPORT                                               */
/* 3.9 (17/05/99) : modified the application_task definition                                    */
/*==============================================================================================*/


/*==============================================================================================*/
/* Defines */

/* define this with the "project" name string */
#define STRT_PROJECT_STRING			"test com 0.0"

/* define this with the name to use for the strt routine ("main" if it will be the entry point) */
#define STRT_ROUTINE_NAME			main

/* number of application tasks */
#define STRT_USER_TASK_NUM			10

/* define this if you want the TCN to be configured */
#define STRT_CONFIGURE_TCN

/* these macros define the various tasks ids */
#define STRT_APPLICATION_TASK_ID	2	/* this can be commented out */
#define STRT_SINK_TMO_TASK_ID		3
#define STRT_MESSENGER_TASK_ID		4
#define STRT_SHELL_TASK_ID			5	/* this can be commented out */
#define STRT_BMI_TASK_ID			6	/* WTB */
#define STRT_PDM_TASK_ID			7	/* WTB */
#define STRT_MAP_TASK_ID			8	/* WTB && !UMS || !WTB && MAP */

/* these macros define the tasks priorities */
#define STRT_BMI_TASK_PRIO			55	/* WTB */
#define STRT_BA_TASK_PRIO			54
#define STRT_MESSENGER_TASK_PRIO	53
#define STRT_PDM_TASK_PRIO			52	/* WTB */
#define STRT_MAP_TASK_PRIO			51	/* WTB && !UMS || !WTB && MAP */
#define STRT_SINK_TMO_TASK_PRIO		50
#define STRT_APPLICATION_TASK_PRIO  1
//#define STRT_SHELL_TASK_PRIO        0
#define STRT_SHELL_TASK_PRIO        51

/* this macro enables the external NCDB */
#define STRT_EXT_NCDB_ENABLED

/* RTX-166 configuration parameters */
#define STRT_RTX166_ROUND_ROBIN		TRUE
#define STRT_RTX166_MAX_MAILBOXES	0
#define STRT_RTX166_MAX_SEMAPHORES	0
#define STRT_RTX166_MAX_MEM_POOLS	0

/* RTX-166 system mempool size */
#define STRT_RTX166_SYSPOOL_SIZE	0x1000

/* RTX-166 workspace bytes to use for fast tasks (standard tasks add 32 + stack context) */
#define STRT_RTX166_WSPSIZE_FAST	800

/* define this to disable dynamic allocation in ISRs on RTX166 platforms */
#define STRT_RTX166_NO_ALLOC_IN_ISR

/* PIL constants */
#ifdef __C166__
#define STRT_PIL_SYSPOOL_SIZE		0xF000		/* system mempool size */
#endif
#ifdef _MRI
#define STRT_PIL_SYSPOOL_SIZE		-(16*1024)	/* system mempool size */
#endif
#define STRT_PIL_SEM_NUM			64		/* max number of semaphores */
#define STRT_PIL_MQ_NUM				32		/* max number of message queues */
#define STRT_PIL_TMO_NUM			64		/* max number of timeouts */

/* define this to have automatic station ID configuration */
#define STRT_AUTO_ST_ID				0xF0000000

/* define this with the baud rate for the serial interface */
#define STRT_SIO_BAUD_RATE			38400

/* define this to install the monitor commands in the shell */
#define STRT_INSTALL_MONITORS

/* define this if you want an nse_record implementation that uses printf */
#define STRT_PROVIDES_NSE_RECORD

/* define this if you do not want user_ba_report() to be called when the BA diagnosis changes */
#define STRT_NO_USER_BA_REPORT

#ifdef ATR_WTB

/* define this to start the UIC Mapping Server; could be used in an MVB-only system */
//#define STRT_MAP

/* define this if you do not want user_ls_report() to be called when a WTB LL Report occurs */
#define STRT_NO_USER_LS_REPORT

/* define this if you do not want user_map_report() to be called when a UIC Mapping Server Report occurs */
#define STRT_NO_USER_MAP_REPORT

#endif


/*==============================================================================================*/
/* Include the user definitions */

#include "strt_usr.h"


/*==============================================================================================*/
/* Do not touch this section */

/*----------------------------------------------------------------------------------------------*/
/* Includes */

#include "pi_sys.h"
#include "conf_tcn.h"


/*----------------------------------------------------------------------------------------------*/
/* Macros */

#ifdef __C166__
/* size of the workspace for each task */
#define WSPSIZE (32+(RTX_STKSIZE_ST*2) + STRT_RTX166_WSPSIZE_FAST)
#endif

/* save an error */
#define Error(act, list) \
	nse_record(__FILE__, __LINE__, NSE_EVENT_TYPE_ERROR, act, sizeof(list), (UINT8*)&list)


/*----------------------------------------------------------------------------------------------*/
/* External functions */

NcEleID get_station_id(unsigned short mvb_hw_address);	/* user station ID calculation */
void user_ba_report(void *p_diagnosis);					/* user MVB BA report          */
void user_ls_report(unsigned short r);					/* user WTB link layer report  */
void user_map_report(unsigned short r);					/* user Mapping Server report  */
PI_TASK_DEF(application_task);							/* application task proc name  */


/*----------------------------------------------------------------------------------------------*/
/* Provided globals */

extern const char application_version[];
extern const char library_version[];

extern char strt_tcn_configured;	/* if TRUE the configuration was succesful          */
extern char strt_tcn_failure;		/* if TRUE there was a failure in the configuration */


/*==============================================================================================*/

