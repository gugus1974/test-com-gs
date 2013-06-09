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
*	Module Name:		vosintbl.tpl
*
*	Identification:		 1.18 vosintbl.tpl
*
*	Date:			9/28/94  08:52:07
*
****************************************************************************
*/

/* Kernel-independent system configuration options */

#include <syskind.h>

/* Console output buffer */

char sys_console_buffer[64];
const unsigned short sys_console_size = sizeof(sys_console_buffer);

/* Interrupt-driven console output function */

extern void sc_putc(_ANSIPROT1(char));
void (*  sys_console_putc_interrupt)(_ANSIPROT1(char)) = sc_putc;

/* Name of console logical I/O device */

const char sys_console_name[] = "CONSOLE";
/*** for backwards compatibility with ESH, should be removed ***/
char *  sys_console = "CONSOLE";

/* Console callouts */

extern void sys_ttya_txrdy_handler(_ANSIPROT4(logio_device_id_t, logio_int_event_t, void *, cpu_context_ptr));
extern void sys_ttya_rxchr_handler(_ANSIPROT4(logio_device_id_t, logio_int_event_t, void *, cpu_context_ptr));
extern void sys_ttya_error_handler(_ANSIPROT4(logio_device_id_t, logio_int_event_t, void *, cpu_context_ptr));
void (*  sys_console_txrdy_callout)(_ANSIPROT4(logio_device_id_t, logio_int_event_t, void *, cpu_context_ptr)) = sys_ttya_txrdy_handler;
void (*  sys_console_rxchr_callout)(_ANSIPROT4(logio_device_id_t, logio_int_event_t, void *, cpu_context_ptr)) = sys_ttya_rxchr_handler;
void (*  sys_console_ttyerr_callout)(_ANSIPROT4(logio_device_id_t, logio_int_event_t, void *, cpu_context_ptr)) = sys_ttya_error_handler;

/* Timer callouts */

extern void sys_timer_handler(_ANSIPROT4(logio_device_id_t, logio_int_event_t, void *, cpu_context_ptr));
void (*  sys_timer_callout)(_ANSIPROT4(logio_device_id_t, logio_int_event_t, void *, cpu_context_ptr)) = sys_timer_handler;

/* Cache and MMU */

#if defined(MC68020) || defined(MC68040) || defined(I386) || defined(SPARC) || defined(AMD29K)
const char sys_cache_enabled = 1;
const cpu_cm_cd_t sys_cache_designator = CPU_CACHE_ALL_CACHE;
const unsigned sys_cache_mode = BOOTOS_MEMORY_COPYBACK;
#endif /* MC68020 || MC68040 || I386 || SPARC */
#if defined(MC68020) || defined(MC68040) || defined(I386)
const char sys_mmu_enabled = 1;
const unsigned sys_mmu_page_table_size = 0x10000;
const cpu_cm_mmu_mode_t sys_mmu_mode =CPU_MMU_MODE_USER;
#endif /* MC68020 || MC68040 || I386 */

#ifdef SPARC
const unsigned long cpu_nwindows = 8;
#endif

/* System initialization verbosity */

const char sys_verbose_major = 0;
const char sys_verbose_minor = 0;
const char sys_verbose_panic = 0;
