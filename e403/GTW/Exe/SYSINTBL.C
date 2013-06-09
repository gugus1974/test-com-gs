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
*	Module Name:		sysintbl.tpl
*
*	Identification:		 1.5 sysintbl.tpl
*
*	Date:			8/11/94  09:12:20
*
****************************************************************************
*/


#include <syskind.h>

extern void sys_get_bootitem(ANSIPROT0);
extern void sys_initialize_console(ANSIPROT0);
extern void sys_initialize_syshooks(ANSIPROT0);
extern void sys_initialize_mmu_cache(ANSIPROT0);
extern void user_init1(ANSIPROT0);
extern void sys_initialize_vrtxsa_cfg(ANSIPROT0);
extern void sys_initialize_vrtx(ANSIPROT0);
extern void sys_initialize_device_vectors(ANSIPROT0);
extern void sys_enable_device_ints(ANSIPROT0);
extern void sys_initialize_timer(ANSIPROT0);
extern void sys_initialize_clock(ANSIPROT0);
extern void sys_initialize_rtl(ANSIPROT0);
extern void sys_initialize_environment(ANSIPROT0);
extern void user_init2(ANSIPROT0);
extern void v90k_go(ANSIPROT0);

const sys_tentry_t sys_init_table[] = {
/*0*/	sys_get_bootitem,
/*1*/	sys_initialize_console,
/*2*/	sys_initialize_syshooks,
/*3*/	sys_initialize_mmu_cache,
/*4*/	user_init1,
/*5*/	sys_initialize_vrtxsa_cfg,
/*6*/	sys_initialize_vrtx,
/*7*/	sys_initialize_device_vectors,
/*8*/	sys_enable_device_ints,
/*9*/	sys_initialize_timer,
/*10*/	sys_initialize_clock,
/*11*/	sys_initialize_rtl,
/*12*/	sys_initialize_environment,
/*13*/	user_init2,
/*14*/	v90k_go,
	0
};
