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
*	Module Name:		vrtxcals.tpl
*
*	Identification:		 6.13 vrtxcals.tpl
*
*	Date:			8/11/94  09:12:24
*
****************************************************************************
*/

#include <vrtxil.h>

/* contains reference to all the VRTX system calls that are configured to be in OS */

static void (* const sys_syscall_table[])() =
{
	(void(*)())sc_tcreate,
	(void(*)())sc_tecreate,
	(void(*)())sc_tdelete,
	(void(*)())sc_tsuspend,
	(void(*)())sc_tresume,
	(void(*)())sc_tpriority,
	(void(*)())sc_tinquiry,
	(void(*)())sc_gblock,
	(void(*)())sc_rblock,
	(void(*)())sc_post,
	(void(*)())sc_pend,
	(void(*)())sc_gtime,
	(void(*)())sc_stime,
	(void(*)())sc_delay,
	(void(*)())sc_getc,
	(void(*)())sc_putc,
	(void(*)())sc_waitc,
	(void(*)())ui_timer,
	(void(*)())ui_rxchr,
	(void(*)())ui_txrdy,
	(void(*)())sc_tslice,
	(void(*)())sc_fcreate,
	(void(*)())sc_fdelete,
	(void(*)())sc_fpend,
	(void(*)())sc_fpost,
	(void(*)())sc_fclear,
	(void(*)())sc_finquiry,
	(void(*)())sc_qjam,
	(void(*)())sc_qecreate,
	(void(*)())sc_lock,
	(void(*)())sc_unlock,
	(void(*)())sc_pcreate,
	(void(*)())sc_pextend,
	(void(*)())sc_accept,
	(void(*)())sc_qpost,
	(void(*)())sc_qpend,
	(void(*)())sc_qaccept,
	(void(*)())sc_qinquiry,
	(void(*)())sc_screate,
	(void(*)())sc_sdelete,
	(void(*)())sc_spend,
	(void(*)())sc_spost,
	(void(*)())sc_sinquiry,
	(void(*)())vrtx_init,
	(void(*)())vrtx_go,
	(void(*)())sc_acceptc,
	(void(*)())sc_gclock,
	(void(*)())sc_sclock,
	(void(*)())sc_adelay,
	(void(*)())sc_halloc,
	(void(*)())sc_hcreate,
	(void(*)())sc_hdelete,
	(void(*)())sc_hfree,
	(void(*)())sc_hinquiry,
	(void(*)())sc_maccept,
	(void(*)())sc_mcreate,
	(void(*)())sc_mdelete,
	(void(*)())sc_minquiry,
	(void(*)())sc_mpend,
	(void(*)())sc_mpost,
	(void(*)())sc_pdelete,
	(void(*)())sc_pinquiry,
	(void(*)())sc_qbrdcst,
	(void(*)())sc_qdelete,
	(void(*)())sc_saccept,
	(void(*)())sc_gversion,
	(void(*)())sc_call,
	0
};

void v32_trap_handler()
{
}
