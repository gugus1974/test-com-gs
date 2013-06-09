/*=============================================================================================*/
/* UMS utility functions           													           */
/* Header file (atr_ums.h)															           */
/* 																					           */
/* Version 1.0																		           */
/*																					           */
/* (c)1998 Ansaldo Trasporti														           */
/* Written by Giuseppe Carnevale													           */
/*=============================================================================================*/


#ifndef _UMS_UTIL_
#define _UMS_UTIL_


/*=============================================================================================*/
/* Includes */

#include "ums.h"


/*=============================================================================================*/
/* Defines */


/*=============================================================================================*/
/* Typedefs */

typedef void (*UmsUicInd)(AM_RESULT status, UIC_Header *msg, void *user_ref);


/*=============================================================================================*/
/* Prototypes */

short ums_util_init(void);

AM_RESULT ums_uic_read_nadi(UIC_ReadNadi *p_NadiMsg, char fct, UmsUicInd ind);
AM_RESULT ums_uic_read_uwtm_state();
AM_RESULT ums_uic_change_omode();
AM_RESULT ums_uic_read_uwtm_topography();
AM_RESULT ums_uic_read_group();
AM_RESULT ums_uic_read_group_list();
AM_RESULT ums_uic_write_group();
AM_RESULT ums_uic_write_group_all();
AM_RESULT ums_uic_delete_group();
AM_RESULT ums_uic_delete_group_all();
AM_RESULT ums_uic_invalidate_nadi();
AM_RESULT ums_uic_write_correction();
AM_RESULT ums_uic_write_res_nr();
AM_RESULT ums_uic_sleep();
AM_RESULT ums_uic_inauguration_control();
AM_RESULT ums_uic_inauguration_enforce();
AM_RESULT ums_uic_multicast_create();
AM_RESULT ums_uic_leading();
AM_RESULT ums_uic_convert_uic_tcn();


/*=============================================================================================*/

#endif

