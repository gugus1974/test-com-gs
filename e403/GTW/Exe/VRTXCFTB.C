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
*	Module Name:		vrtxcftb.tpl
*
*	Identification:		 6.43 vrtxcftb.tpl
*
*	Date:			10/11/94  15:15:42
*
****************************************************************************
*/

/* VRTX configuration table */

#include <vrtxil.h>
#include <logio.h>
#include <sysvrtx.h>

extern CVT sys_vrtx_cvt;
extern void sys_ttya_xtxrdy(_ANSIPROT1(int));
extern void sys_tcreate_hook(_ANSIPROT2(TCB *, TCB *));
extern void sys_tdelete_hook(_ANSIPROT2(TCB *, TCB *));
extern void sys_tswitch_hook_null(_ANSIPROT2(TCB *, TCB *));



 CFTBL sys_vrtx_config_rom =
       {
        0,
        /* workspace size */
        310288,
        /* system stack size */
        8192,
        /* ISR stack size */
        0,
        /* control block count */
        48,
        /* partition count */
        17,
        /* idle task stack size */
        8192,
        /* queue count */
        16,
        /* component disable level */
        7,
        /* user stack size */
        16384,
        /* maximum task ID */
        0,
        /* target type */
        target_unspecified,
        /* user task count */
        12,
        /* Configuration options */
        0,
        /* transmit ready driver address */
        sys_ttya_xtxrdy,
        /* task create hook routine */
        sys_tcreate_hook,
        /* task delete hook routine */
        sys_tdelete_hook,
        /* task switch hook routine */
        sys_tswitch_hook_null,
        /* component vector table address */
        &sys_vrtx_cvt,
        /* ARTX reserved field */
        0,
        /* ARTX number of entries allocated per task */
        0,
        /* ARTX number of tasks using entry tables */
        0,
        /* ARTX master frame control block count */
        0
        };
        

 int sys_os_mode = 1992;      /* trap_handler == 1965 else 1992 */

struct v32_config *sys_vrtx_config_ptr;


extern void main(_ANSIPROT0);
void (*  sys_entry_pt1)(_ANSIPROT0) = 0;
void (*  sys_entry_pt2)(_ANSIPROT0) = main;
void (*  sys_usr_entry_pt)(_ANSIPROT0) = (void (* const)())0;

extern void sys_load_nk_isr(_ANSIPROT3(int, void *, char *));
void (*  sys_interrupt_handler_installer)(_ANSIPROT3(int, void *, char *)) = sys_load_nk_isr;






const sys_vrtx_type_t sys_vrtx_type = sys_vrtxsa;

extern void *(sys_load_vrtx_isr)(_ANSIPROT2(int, void *)); 
void *(*sys_load_vrtxsa_isr)(_ANSIPROT2(int, void *)) = sys_load_vrtx_isr;
