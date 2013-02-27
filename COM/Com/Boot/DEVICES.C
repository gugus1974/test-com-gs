/***************************************************************************
*
*		Copyright (c) 1992 READY SYSTEMS CORPORATION.
*
*	All rights reserved. READY SYSTEMS' source code is an unpublished
*	work and the use of a copyright notice does not imply otherwise.
*	This source code contains confidential, trade secret material of
*	READY SYSTEMS. Any attempt or participation in deciphering, decoding,
*	reverse engineering or in any way altering the source code is
*	strictly prohibited, unless the prior written consent of
*	READY SYSTEMS is obtained.
*
*
*	Module Name:		devices.tpl
*
*	Identification:		 1.4 devices.tpl
*
*	Date:			4/25/94  14:52:36
*
****************************************************************************
*/




#include <boot.h>

extern logio_method_t logio_board_method;


const boot_device_t boot_device_table[] = {
	"DEV_BOARD",	&logio_board_method,
	"",	0
};
