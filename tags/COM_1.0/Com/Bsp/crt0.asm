
				LLEN	1000					; our lines are very long

*************************************************************************************************
*                                                                                               *
* COM board initialization for VRTX boot                                                        *
*                                                                                               *
* (c)1997 Ansaldo Trasporti                                                                     *
* Written by Giuseppe Carnevale                                                                 *
*                                                                                               *
*************************************************************************************************


				CHIP	68360					; we are using a 68360 chip
				OPT		BRB						; forward references use the 8 bit displacement

*************************************************************************************************

CODE_SECTION	MACRO							; switch to the code section
				SECTION 9
				ENDM

DATA_SECTION	MACRO							; switch to the data section
				SECTION 14
				ENDM

*************************************************************************************************

				XREF	_boot_sys_crt0			; boot code initialization entry point
				XREF	_boot_warm_start		; this is the warm start entry point

				XREF	_boot_item_target
				XREF	_boot_segv_ok
				XREF	_cpu_target

*************************************************************************************************

				XDEF	_crt0_init_stack		; initialization stack
				XDEF	_crt0_init_stktop		; top of initialization stack

				XDEF	_crt0_cold_start		; cold start entry point
				XDEF	_crt0_warm_start		; warm start entry point

*************************************************************************************************

				DATA_SECTION

_crt0_init_stack:								; stack used during the initialization
				DS.L	$1000					; = 16KB
_crt0_init_stktop:

*************************************************************************************************

				ORG	$0,m68k_crt0_vec

        		DC.L	_crt0_init_stktop			; reset SP
        		DC.L	_crt0_cold_start+$400000	; reset PC
        		DC.L	_crt0_warm_start			; this is the warm entry point (for the monitor)
boot_sys_vec:	DC.L	_boot_sys_crt0				; this is the cold board bootstrap entry point

*************************************************************************************************

use_watchdog	SET		1						; set to 1 to enable the HW watchdog

*************************************************************************************************

				ORG	$400,m68k_crt0

_crt0_cold_start:
				INCLUDE	com_init.src			; initialize the board hardware
				INCLUDE	com_rora.src			; move ourself from the ROM to the RAM
				move.w	#$2700,SR				; disable interrupts and switch to interrupt mode
				movea.l	#_crt0_init_stktop,SP	; set up initialization stack
				move.l	boot_sys_vec,A0			; load the boot address and fix it
				move.l	#_boot_sys_crt0,boot_sys_vec
				jmp     (A0)					; do the board bootstrap

_crt0_warm_start:
				move.w	#$2700,SR				; disable interrupts and switch to interrupt mode
				movea.l	#_crt0_init_stktop,SP	; set up initialization stack
				move.l	_boot_item_target,_cpu_target
				clr.l	_boot_segv_ok
				jmp     _boot_warm_start		; do the board warm bootstrap

*************************************************************************************************

				END

