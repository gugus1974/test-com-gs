/*=============================================================================================*/
/* TCN utility functions         													           */
/* Implementation file (tcn_util.c)													           */
/* 																					           */
/* Version 1.0																		           */
/*																					           */
/* (c)1999 Ansaldo Trasporti														           */
/* Written by Giuseppe Carnevale													           */
/*=============================================================================================*/


/*=============================================================================================*/
/* Includes */

#include "pi_sys.h"
#include "lc_sys.h"
#include "lp_sys.h"

#include "tcn_util.h"


/*=============================================================================================*/
/* Convertion routines */

/*---------------------------------------------------------------------------------------------*/
/* Process variables name conversion from APD format to 32-bit */

void conv_apdpvname_pvn32(const TYPE_APD_PV_NAME *apd_pv_name, struct STR_PV_NAME *pvn32)
{
	pvn32->type       = apd_pv_name->type;
	pvn32->size       = apd_pv_name->size;
	pvn32->bit_offset = apd_pv_name->bit_offset;
	pvn32->prt_addr   = apd_pv_name->prt_addr;
	pvn32->ts_id      = apd_pv_name->ts_id;
}


/*---------------------------------------------------------------------------------------------*/
/* Process variables name conversion from 48-bit to 32-bit */

void conv_pvn48_pvn32(const TYPE_PV_NAME_48 *pvn48, struct STR_PV_NAME *pvn32)
{
	pvn32->type       = PVN48GETTYPE(*pvn48);
	pvn32->size       = PVN48GETSIZE(*pvn48);
	pvn32->bit_offset = PVN48GETVAROFF(*pvn48);
	pvn32->prt_addr   = PVN48GETPORT(*pvn48);
	pvn32->ts_id      = PVN48GETTSID(*pvn48);
}


/*---------------------------------------------------------------------------------------------*/
/* Process variables name conversion from APD format (endianess dependent) to 48-bit */

void conv_apdpvname_pvn48(const TYPE_APD_PV_NAME *apd_pv_name, TYPE_PV_NAME_48 *pvn48)
{
	PVN48SETTSID(*pvn48, apd_pv_name->ts_id);
	PVN48SETPORT(*pvn48, apd_pv_name->prt_addr);
	PVN48SETSIZE(*pvn48, apd_pv_name->size);
	PVN48SETVAROFF(*pvn48, apd_pv_name->bit_offset);
	PVN48SETTYPE(*pvn48, apd_pv_name->type);
	PVN48SETCHKOFF(*pvn48, apd_pv_name->ctl_offset);
}


/*---------------------------------------------------------------------------------------------*/
/* Process variables name conversion from 48-bit to APD format (endianess dependent) */

void conv_pvn48_apdpvname(const TYPE_PV_NAME_48 *pvn48, TYPE_APD_PV_NAME *apd_pv_name)
{
	apd_pv_name->ts_id      = PVN48GETTSID(*pvn48);
	apd_pv_name->prt_addr   = PVN48GETPORT(*pvn48);
	apd_pv_name->size       = PVN48GETSIZE(*pvn48);
	apd_pv_name->bit_offset = PVN48GETVAROFF(*pvn48);
	apd_pv_name->type       = PVN48GETTYPE(*pvn48);
	apd_pv_name->ctl_offset = PVN48GETCHKOFF(*pvn48);
}


/*=============================================================================================*/
/* Size routines */

/*---------------------------------------------------------------------------------------------*/
/* Return the size of the variable needed to store a 48-bit named variable */

int size_pvn48(const TYPE_PV_NAME_48 *pvn48)
{
	struct STR_PV_NAME pvn32;
	conv_pvn48_pvn32(pvn48, &pvn32);
	return lp_pv_size(&pvn32);
}


/*---------------------------------------------------------------------------------------------*/
/* Return the size of the variable needed to store an APD named variable */

int size_apdpvname(const TYPE_APD_PV_NAME *apd_pv_name)
{
	struct STR_PV_NAME pvn32;
	conv_apdpvname_pvn32(apd_pv_name, &pvn32);
	return lp_pv_size(&pvn32);
}



/*=============================================================================================*/

