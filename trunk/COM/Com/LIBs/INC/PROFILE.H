/*==============================================================================================*/
/* UIC Mapping Server patches file													            */
/* Implementation file (profile.h)													            */
/* 																					            */
/* Version 1.0																		            */
/*																					            */
/* (c)1998 Ansaldo Trasporti														            */
/* Written by Giuseppe Carnevale													            */
/*==============================================================================================*/


/*==============================================================================================*/
/* Includes */

#include "atr_sys.h"	/* global definitions */


/*==============================================================================================*/
/* Defines */

/*----------------------------------------------------------------------------------------------*/
/* Compilation switches */

#define PDM				/* we use the PDM */
#ifndef UMS
#define UMS             /* new UIC Mapping Server */
#endif
// #define O_GATEWAY       /* work as Gateway (defined in atr_sys.h) */
// #define WTB
// #define GATEWAY
// #define WAG


/*----------------------------------------------------------------------------------------------*/
/* Constants */

#define ALL_OK 0	/* system-wide "no error" value */


/*----------------------------------------------------------------------------------------------*/
/* Macros */

#define UNREFERENCED(v) (v = v)


/*----------------------------------------------------------------------------------------------*/
/* Function remapping */

/* memory allocation */
#define PI_ALLOC pi_alloc
#define PI_FREE  pi_free

/* C166 watchdog */
#define SRVWDT()

/* selective task disabling routines */
#define SPLDECL
#define SPLMAC()    pi_disable()
// #define SPLMSG()    pi_disable()
// #define SPLCI()     pi_disable()
// #define SPLMON()    pi_disable()
// #define SPLISR()    pi_disable()
// #define SPLTMO()    pi_disable()
// #define SPLOS()     pi_disable()
// #define SPLSTS()    pi_disable()
// #define SPLAPL()    pi_disable()
// #define SPLNS()     pi_disable()
// #define SPLBA()     pi_disable()
#define SPLX()      pi_enable()

/* event recording routines */
#define RECORD_M_REBOOT(p_msg) (nse_record(__FILE__,__LINE__,  \
                                        NSE_EVENT_TYPE_ERROR,\
                                        NSE_ACTION_REBOOT, \
                                        (strlen(p_msg)+1),   \
                                        (UNSIGNED8 *)(p_msg)))
#define RECORD_M_NSDB(p_msg) (nse_record(__FILE__,__LINE__,  \
                                        NSE_EVENT_TYPE_ERROR,\
                                        NSE_ACTION_HALT, \
                                        (strlen(p_msg)+1),   \
                                        (UNSIGNED8 *)(p_msg)))
#define RECORD_M_EVENT(p_msg) (nse_record(__FILE__,__LINE__,  \
                                        NSE_EVENT_TYPE_SPECIAL,\
                                        NSE_ACTION_CONTINUE,   \
                                        (strlen(p_msg)+1),     \
                                        (UNSIGNED8 *)(p_msg)))


/*==============================================================================================*/

