/***************************************************************************
*
*		Copyright (c) 1993 MICROTEC RESEARCH INCORPORATED.
*
*	All rights reserved. MICROTEC RESEARCH's source code is an unpublished
*	work and the use of a copyright notice does not imply otherwise.
*	This source code contains confidential, trade secret material of
*	MICROTEC RESEARCH. Any attempt or participation in deciphering, decoding,
*	reverse engineering or in any way altering the source code is
*	strictly prohibited, unless the prior written consent of
*	MICROTEC RESEARCH is obtained.
*
*
*	Module Name:		vrtxcvt.tpl
*
*	Identification:		 6.5 vrtxcvt.tpl
*
*	Date:			8/11/94  09:12:30
*
****************************************************************************
*/


#include <vrtx90/v32cftbl.h>

struct v32_cvt sys_vrtx_cvt;			/* in data section */
 struct v32_cvt sys_vrtx_cvt_rom =               /* in constant section */
	{
	/* maximum Ready Systems Components */
	10,		
	/* maximum user defined components */
	0,
	/* reserved */
	0, 0, 0, 0, 
	/* iox codespace and workspace address */
	0, 0, 
	/* fmx codespace and workspace address */
	0, 0, 
	/* rtl codespace and workspace address */
	0, 0, 
	/* component5 codespace and workspace address */
	0, 0, 
	/* mpv codespace and workspace address */
	0, 0, 
	/* ifx codespace and workspace address */
	0, 0, 
	/* component8 codespace and workspace address */
	0, 0, 
	/* component9 codespace and workspace address */
	0, 0, 
	/* tnx codespace and workspace address */
	0, 0
	}; 			
