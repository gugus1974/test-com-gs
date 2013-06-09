/***************************************************************************
*
*		Copyright (c) 1992 MICROTEC RESEARCH INCORPORATED.
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
*	Module Name:		%M%
*
*	Identification:		%Z% %I% %M%
*
*	Date:			%G%  %U%
*
****************************************************************************
*/

#include <ansiprot.h>








int rtl_heap_size = 10000;
extern void sc_call();
void (* const sys_sc_call_dummy)() = (void (*)())sc_call;



void (* const sys_mpv_poll_ptr)() = (void (*)())0;

extern void sys_rtl_tcreate_hook();
extern void sys_rtl_tdelete_hook();
extern void sys_rtl_tswitch_hook();

void (* const sys_rtl_hooks_tab[3])() = {
 	sys_rtl_tcreate_hook,
        sys_rtl_tdelete_hook,
        sys_rtl_tswitch_hook
};



