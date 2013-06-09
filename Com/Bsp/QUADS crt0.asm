***************************************************************************
*
*		Copyright (c) 1993 READY SYSTEMS CORPORATION.
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
*	Module Name:		crt0.asm
*
*	Identification:		@(#) 1.8 crt0.asm
*
*	Date:			1/25/94  15:08:26
*
****************************************************************************
*

        OPT     NOLC
        LIST

CODE_SECTION	MACRO
	SECTION 9
	ENDM

DATA_SECTION	MACRO
	SECTION 14
	ENDM

	XREF	_boot_sys_crt0

	XDEF	_crt0_init_stack
	XDEF	_crt0_init_stktop

	XDEF	_crt0_cold_start
	XDEF	_crt0_warm_start
	XDEF	_crt0_common_start

*	XDEF	_my_spurious_int

	XDEF	_irq_level4
	XDEF	_irq_level3

	DATA_SECTION
*SPURIOUS_COUNTER 
*	ds.l	$01

*SAVED_SR
*	ds.w	$01
_crt0_init_stack:
	ds.l	$1000
_crt0_init_stktop:

        CODE_SECTION
        dc.l    _crt0_init_stktop        * reset SP
        dc.l    _crt0_cold_start         * reset PC
        dc.l    _crt0_warm_start
*        dc.l    _my_spurious_int

_crt0_cold_start:


	MOVEA.L  #$20700,A0 
	MOVEA.L  A0,A7
	MOVEA.L  #$7,A3
	MOVEC    A3,SFC	                * SFC & DFC must be set first to idicate
	MOVEC    A3,DFC	                * cpu space and before accessing master MBAR
	MOVEA.L  #$20001,A3            	* to obtain the mapping described in 
	MOVES.L  A3,($3FF00).L         	* the m68360QUADS user manual, init. master MBAR
	MOVEA.W  #$BFFF,A3	        * to obtain access to slave MBAR, init. 
	MOVES.W  A3,($3FF08).L		* 0x3ff08 with slave's MBAR
	MOVEA.W  #$2,A3			
	MOVES.W  A3,($3FF04).L	        * init slave MBAR
	MOVEA.W  #$BFFF,A3
	MOVES.W  A3,($3FF08).L
	MOVEA.W  #$2001,A3
	MOVES.W  A3,($3FF06).L
	JSR      INIT_QUADS

        moveq.l #0,D0          	        * signal cold reset
        bra.s   _crt0_common_start

_crt0_warm_start:
        moveq.l #1,D0           	* signal warm reset

_crt0_common_start:

        move.w	#$2700,SR		* disable interrupts and switch to interrupt mode
        movea.l	#_crt0_init_stktop,SP	* set up initialization stack

	jmp     _boot_sys_crt0
        illegal

INIT_QUADS:

	MOVE.L   #$21009,D6
	MOVEA.L  D6,A0            	* determine if we got a software or hardware 
	MOVE.B   (A0),D0		* reset, if software we are done else
	ANDI.L   #$FE,D0		* re-init. master & slave QUICC
	BEQ.L    INIT_QUADS_14
	MOVEA.L  #$21000,A5	        * init. the master 
	ORI.L    #$10000,(A5)
	MOVE.L   #$2200,($21040).L      * init. GMR; configure memory controller
	MOVE.L   #$5FFE0004,($21054).L	* init. OR0
	MOVEQ    #$3,D7
	MOVE.L   D7,($21050).L	        * init. BR0

	MOVE.B   #$84,($21008).L        * init. AVR; generate auto vector for interrupt level 7 & 2
	MOVE.B   #$AB,($2100C).L	* init. CLKOCR; CLKO1 disable, CLKO2 enabled
	MOVEA.L  #$21010,A5	        * write protect PLLCR
	ORI.W    #$4000,(A5)        	* init. SYPCR; disable software watchdog 
	MOVE.B   #$37,($21022).L 	* and enable bus monitor 
                                        

	MOVEA.L  #$23000,A5             * init. the slave
	MOVEQ    #$0,D7
	OR.L     D7,(A5)
	MOVE.B   #$AF,($2300C).L	* inti. CLKOCR; CLKO1 disable, CLKO2 enabled
	MOVEA.L  #$23010,A5		* write protect PLLCR
	ORI.W    #$4000,(A5)		*
	MOVE.B   #$37,($23022).L	* init. SYPCR; disabled software watchdog and enable bus monitor
	MOVEA.L  #$23040,A3		* init. GMR; init the DRAM Simm that is on the QUADS (type is mcm36256)
	MOVE.L   #$17A44380,(A3)
	CLR.W    ($23554).L		* init. PAODR; select as active drivers
	MOVE.L   #$23556,D7		* init. PADAT
	MOVEA.L  D7,A0
	MOVE.W   #$FFFF,(A0)
	MOVE.W   #$3C00,($23550).L	* init. PADIR
	MOVE.W   #$33F,($23552).L	* init. PAPAR
	CLR.W    ($236C0).L		* init. PBODR; select as active drivers
	MOVE.L   #$3FFFF,($236C4).L	* init. PBDAT
	MOVEQ    #$7F,D1
	MOVE.L   D1,($236B8).L		* init. PBDIR
	MOVEQ    #$E,D1
	MOVE.L   D1,($236BC).L		* init. PBPAR
	MOVE.L   #$FFFE002,($23094).L	* init. OR4; 
	MOVE.L   #$24003,($23090).L	* init. BR4
	MOVE.L   #$FFFE004,($230A4).L	* init. OR5
	MOVE.L   #$26001,($230A0).L	* init. BR5
	CLR.B    ($26000).L		
	MOVE.L   #$FFFE004,($230B4).L	* init. OR6
	MOVEA.L  #$230B0,A4		* init. BR6
	MOVE.L   #$28001,(A4)

	MOVEA.L  #$24000,A5	 	* Determine if QUICC A(31:28) pin is 
	MOVE.W   (A5),D0                * write enable or configured as address lines
	BTST.L   #$3,D0
	BEQ.B    INIT_QUADS_1
	MOVEA.L  #$21016,A2		* master PEPAR: must be write enable (WE3-WE0)
	ORI.W    #$80,(A2)		* select write enable on PEPAR
	MOVE.L   #$28000,(A4)
	BRA.B    INIT_QUADS_2
INIT_QUADS_1:


	MOVEA.L  #$21016,A2	   	* master PEPAR: must be configred 
	ANDI.W   #$FF7F,(A2)            * as address lines (A31-A28).
	MOVE.L   (A4),D0
	ANDI.L   #-$800,D0
	MOVEA.L  D0,A0
	CLR.B    (A0)
INIT_QUADS_2:
	MOVE.W   #$3760,($23016).L	* init. slave PEPAR
	MOVEA.L  D7,A0
	MOVE.W   (A0),D0
	BTST.L   #$7,D0			* determine if FLASH memory switch is on/off
	BNE.B    INIT_QUADS_3		* if off skip init. of OR3 & BR3
	MOVE.L   #$3FF80000,($23084).L 	* must be on so : init. slave OR3
	MOVE.L   #$80009,($23080).L	* init. slave BR3
INIT_QUADS_3:
	MOVEA.L  D7,A0			* check if enable RAM switch is on/off
	MOVE.W   (A0),D0
	BTST.L   #$6,D0			* if off skip init. of OR1 & BR1
	BNE.B    INIT_QUADS_7
	MOVEA.L  #$23064,A4		* init. slave OR1
	MOVE.L   #$2FC00009,(A4)
	MOVE.W   (A5),D0		* determine what type of DRAM Simm
	ANDI.L   #$C0,D0		* that is installed on the QUADS
	BNE.B    INIT_QUADS_4
	ORI.L    #$30000000,(A4)
INIT_QUADS_4:
	MOVE.W   (A5),D0
	MOVEQ    #$30,D1
	AND.L    D1,D0
	MOVEQ    #$10,D1
	CMP.L    D1,D0
	BEQ.B    INIT_QUADS_5
	MOVE.W   (A5),D0
	MOVEQ    #$30,D1
	AND.L    D1,D0
	MOVEQ    #$20,D1
	CMP.L    D1,D0
	BNE.B    INIT_QUADS_6
INIT_QUADS_5:
	ORI.L    #$FF00000,(A4)
INIT_QUADS_6:
	MOVE.L   #$400005,($23060).L	* init. slave BR1
INIT_QUADS_7:
	MOVEA.L  D7,A0			* check if enable DRAM switch is on/off
	MOVE.W   (A0),D0
	BTST.L   #$6,D0			* if off skip init. of OR2 & BR2
	BNE.B    INIT_QUADS_10		
	MOVE.W   (A5),D0
	MOVEQ    #$30,D7
	AND.L    D7,D0
	MOVEQ    #$10,D7
	CMP.L    D7,D0
	BEQ.B    INIT_QUADS_9
	MOVE.W   (A5),D0
	MOVEQ    #$30,D7
	AND.L    D7,D0
	CMP.L    D7,D0
	BNE.B    INIT_QUADS_10
INIT_QUADS_9:
	MOVEA.L  #$23074,A4		* init. slave OR2
	MOVE.L   #$2FC00009,(A4)	
	MOVE.W   (A5),D0		* determine what kind of DRAM Simm
	ANDI.L   #$C0,D0		* that is installed on the QUADS
	BNE.B    INIT_QUADS_8
	ORI.L    #$30000000,(A4)
	MOVE.W   (A5),D0
	MOVEQ    #$30,D7
	AND.L    D7,D0
	MOVEQ    #$10,D7
	CMP.L    D7,D0
	BNE.B    INIT_QUADS_8
	ORI.L    #$FF00000,(A4)
	MOVE.L   #$500005,($23070).L
	BRA.B    INIT_QUADS_10
INIT_QUADS_8:
	MOVE.W   (A5),D0
	MOVEQ    #$30,D7
	AND.L    D7,D0
	CMP.L    D7,D0
	BNE.B    INIT_QUADS_10

	MOVE.L   #$800005,($23070).L	* init. slave BR2
INIT_QUADS_10:
	MOVE.W   (A5),D0
	ANDI.L   #$C0,D0
	CMPI.L   #$80,D0
	BNE.B    INIT_QUADS_11
	MOVE.L   #$17840380,(A3)
	BRA.B    INIT_QUADS_13
INIT_QUADS_11:
	MOVE.W   (A5),D0
	ANDI.L   #$C0,D0
	MOVEQ    #$40,D7
	CMP.L    D7,D0
	BNE.B    INIT_QUADS_12
	MOVE.L   #$17A40380,(A3)
	BRA.B    INIT_QUADS_13
INIT_QUADS_12:
	MOVE.W   (A5),D0
	ANDI.L   #$C0,D0
	BNE.B    INIT_QUADS_13
	MOVE.L   #$17A44380,(A3)
INIT_QUADS_13:
	MOVE.L   ($23070).L,D0		* init. slave BR2
	BTST.L   #$0,D0
	BEQ.B    INIT_QUADS_13b
	MOVE.L   (A3),D0
	ANDI.L   #$FFFFFF,D0
	ORI.L    #$A000000,D0
	MOVE.L   D0,(A3)
INIT_QUADS_13b:
	MOVE.L   #-$1FFFA,($23054).L	* init. slave OR0
	MOVEQ    #$3,D7
	MOVE.L   D7,($23050).L		* init. slave BR0
	MOVE.L   #-$FFFFFFA,($230C4).L  * init. slave OR7
	MOVEQ    #$1,D7
	MOVE.L   D7,($230C0).L		* init. salve BR7
	MOVE.W   #$750,($2351E).L
INIT_QUADS_14:

	RTS


*_my_spurious_int:

*	move     SR,SAVED_SR
*	addq.l   #1,SPURIOUS_COUNTER

*	RTE


_irq_level4:
        move.w	#$2400,SR		* change irq level to 4 
	RTS

_irq_level3:
        move.w	#$2300,SR		* change irq level to 3
	RTS

	END








