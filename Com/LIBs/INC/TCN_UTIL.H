/*=============================================================================================*/
/* TCN utility functions													                   */
/* Header file (tcn_util.h)															           */
/* 																					           */
/* Version 1.0																		           */
/*																					           */
/* (c)1999 Ansaldo Trasporti														           */
/* Written by Giuseppe Carnevale													           */
/*=============================================================================================*/


#ifndef _TCN_UTIL_
#define _TCN_UTIL_


/*=============================================================================================*/
/* Typedefs */

#include "tcntypes.h"
#include "apd_incl.h"


/*=============================================================================================*/
/* Prototypes */

void conv_apdpvname_pvn32(const TYPE_APD_PV_NAME *apd_pv_name, struct STR_PV_NAME *pvn32);
void conv_pvn48_pvn32(const TYPE_PV_NAME_48 *pvn48, struct STR_PV_NAME *pvn32);
void conv_apdpvname_pvn48(const TYPE_APD_PV_NAME *apd_pv_name, TYPE_PV_NAME_48 *pvn48);
void conv_pvn48_apdpvname(const TYPE_PV_NAME_48 *pvn48, TYPE_APD_PV_NAME *apd_pv_name);

int size_pvn48(const TYPE_PV_NAME_48 *pvn48);
int size_apdpvname(const TYPE_APD_PV_NAME *apd_pv_name);


/*=============================================================================================*/

#endif

