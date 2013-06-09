* ====================================================================
*
* m360qdser.ieq.IEQ - I/O Equate File for 68360 QUADS
*
* Module Name:  BOARDSUP
* Module Desc:  include file (as hwequ.inc) for board.src or moto.src
* Created:      August 1990
*
*	placed in CLEARCASE - this file his heavily based on the
*	303 SCM driver Apr 95 Modifications from 302 to QUADS by
*	Steve Clevenger
*
*       New rev #5.1    G. Boehm, MRI, April 24, 1991
*                       Put under SCCS
*			Move many board specific equates to moto.src
*
* Revision:     1.0
* --------------------------------------------------------------------
* THIS INFORMATION IS PROPRIETARY TO
* MICROTEC RESEARCH, INC.
* --------------------------------------------------------------------
* Copyright (c) 1990-1996  Microtec Research, Inc.
* All rights reserved
* --------------------------------------------------------------------
*
* General Information:
*
* Note:  The Motorola MC68302 processor can accomplish I/O via an on-
*        chip peripheral.  This file configures these  resources as a
*        serial interface to the XRAY Debugger-Monitor. The 302 device
*	 is almost identiacl to the one used in the 360 QUADs unit.
*
* ====================================================================

* ====================================================================

   ifeq UFLAG		; MCT sets UFLAG to desired channel #
   			; If it didn't, default to channel #2
SCCx    equ     2       ; SCC1 = 1, SCC2 = 2, SCC3 = 3
   elsec
SCCx	equ	UFLAG
   endc

* ====================================================================
*  Equating BASE_ADDR_CAL to 1 allows conditional assembly of code
*  which calculates the BAR peripheral base address whenever an IO
*  routine is called.  This allows the user to be unconcerned with
*  any changes their application may make to this register, since
*  XHM68K IO routines (CON_IN, CON_OUT, or RX_INT) will be unaffected.
*  In addition, foregoing this calculation will allow for better
*  performance from Monitor IO routines.
*
* MCT assumes "BASE_ADDR_CAL equ 1", ie, it will not set up IOBASE by
* default.
* ====================================================================

BASE_ADDR_CAL   equ     1

SLAVE1_MBARE_BIT  equ   $bfff

* --------------------------------------------------------------------
*  SCC Parameter RAM:  Equates are specified as relative offsets
*                      from SCC_PB
* --------------------------------------------------------------------
*QUADS SPECIFIC SUPPORT
* hard code DP RAM base - this is specific for the QAUDS

				IFNDEF	DPRBASE
DPRBASE			EQU		$FFFF8000				; 68360 DP-RAM base address
				ENDC
REGB    equ  DPRBASE+$1000	;i.e. 4K up
SCC_PB  equ  DPRBASE+$c00+(SCCx-1)*$100 ; SCC parameter RAM base




  ifeq BASE_ADDR_CAL       ; For:   BASE_ADDR_CAL   equ  0
Base    equ  IOBASE
  endc

* -----------------  Clock settings for SCC baudrate  ----------------
*
*  Based on EXTAL Frequency of 16.0 MHz  (DIV = TIMECONST)
*
*   1200 baud:  TIMECONST = 832, DIV4 = 0
*   2400 baud:  TIMECONST = 416, DIV4 = 0
*   4800 baud:  TIMECONST = 207, DIV4 = 0
*   9600 baud:  TIMECONST = 103, DIV4 = 0
*  19200 baud:  TIMECONST =  51, DIV4 = 0
*  38400 baud:  TIMECONST =  25, DIV4 = 0
*
* --------------------------------------------------------------------

DIV4    equ     0       ; In SCC Configuration Register

*  If BAUD is undefined, we assume TIMECONST was defined.  Therefore,
*  calculation of the baudrate uses TIMECONST.
*  MCT sets up BAUD by default, and TIMECONST=0.

*  Baud rate=(System Clock or TIN1 Clock)/(Clock Divider+1)/(1 or 4)/16
*  Clock Divider=((System Clock or TIN1 Clock)/((1 or 4)*16*Baud rate)) - 1

*  Baudrate = ((System Clock or TIN1 Clock) / (DIV4 * 16)) * (1 / (DIV+1))
*  DIV = ((System Clock or TIN1 Clock)/(DIV4 * 16 * Baudrate)) - 1


  ifne BAUD                 ; Baudrate determined by BAUD equate
  ifeq BAUD-IO19200
DIV     equ     51
  endc
  ifeq BAUD-IO9600
DIV     equ     103
  endc
  ifeq BAUD-IO4800
DIV     equ     207
  endc
  ifeq BAUD-IO2400
DIV     equ     416
  endc
  ifeq BAUD-IO1200
DIV     equ     832
  endc
  elsec                     ; Baudrate determined by TIMECONST equate
DIV     equ     TIMECONST
  endc

BAUDRATE    equ     (DIV<<1)+DIV4

*QUADS*
* --------------------------------------------------------------------
*  Buffer descriptor offsets from Parameter RAM base
* --------------------------------------------------------------------
RxBD      equ     SCC_PB+$0
TxBD      equ     SCC_PB+$2


* --------------------------------------------------------------------
*  Buffer Descriptor structure (both rx and tx)
* --------------------------------------------------------------------
BD_STAT   equ     $0     ; BD status register
BD_LEN    equ     $02    ; BD Buffer Length
BD_PTR    equ     $04    ; BD Data Pointer

* --------------------------------------------------------------------
*  SCC Parameter RAM
* --------------------------------------------------------------------
RFCR    equ     SCC_PB+$4    ; Rx Function Code
TFCR    equ     SCC_PB+$5    ; Tx Function Code
MRBLR   equ     SCC_PB+$6    ; Max Rx Buffer Length

* --------------------------------------------------------------------
* UART protocol specific parameters
* --------------------------------------------------------------------
MAX_IDL     equ     SCC_PB+$38    ; Max IDLE Characters
IDLC        equ     SCC_PB+$3a    ; Temporary Receive IDLE Counter
BRKCR       equ     SCC_PB+$3c    ; Break Count Register
PAREC       equ     SCC_PB+$3e    ; Receive Parity Error Counter
FRMEC       equ     SCC_PB+$40    ; Receive Framing Error Counter
NOSEC       equ     SCC_PB+$42    ; Receive Noise Counter
BRKEC       equ     SCC_PB+$44    ; Receive Break Character Counter
UADDR1      equ     SCC_PB+$48    ; UART ADDRESS Character 1
UADDR2      equ     SCC_PB+$4a    ; UART ADDRESS Character 2
RCCR1       equ     SCC_PB+$50    ; Receive Control Character 1 Register
RCCR2       equ     SCC_PB+$52    ; Receive Control Character 2 Register
RCCR3       equ     SCC_PB+$54    ; Receive Control Character 3 Register
RCCR4       equ     SCC_PB+$56    ; Receive Control Character 4 Register
RCCR5       equ     SCC_PB+$58    ; Receive Control Character 5 Register
RCCR6       equ     SCC_PB+$5a    ; Receive Control Character 6 Register
RCCR7       equ     SCC_PB+$5c    ; Receive Control Character 7 Register
RCCR8       equ     SCC_PB+$5e    ; Receive Control Character 8 Register
RCCM        equ     SCC_PB+$60

* --------------------------------------------------------------------
* SCC Internal Registers:  Equates are specified as relative offsets
*                          from Base
* --------------------------------------------------------------------

SDCR	EQU		REGB+$51E
PAPAR	EQU		REGB+$552

BRGC1   equ     REGB+$5f0       ; BRG1 Configuration Register
BRGC2   equ     REGB+$5f4       ; BRG2 Configuration Register
BRGC3   equ     REGB+$5f8       ; BRG3 Configuration Register
BRGC4   equ     REGB+$5fc       ; BRG4 Configuration Register

GSMR_L  equ     REGB+$600+(SCCx-1)*$20     ; General Mode Reg Low 32 bits
GSMR_H  equ     REGB+$600+(SCCx-1)*$20+$04 ; General Mode Reg Hi  32 bits
PSMR    equ     REGB+$600+(SCCx-1)*$20+$08 ; Protocol Specific Mode Register
TODR    equ     REGB+$600+(SCCx-1)*$20+$0c ; Transmit on demand
DSR     equ     REGB+$600+(SCCx-1)*$20+$0e ; SCC Data Sync Register
SCCE    equ     REGB+$600+(SCCx-1)*$20+$10 ; SCC Event Register
SCCM    equ     REGB+$600+(SCCx-1)*$20+$14 ; SCC Mask Register
SCCS    equ     REGB+$600+(SCCx-1)*$20+$18 ; SCC Status Register

SIMODE  equ     REGB+$06e0      ; SI Mode
SIGMR   equ     REGB+$06e4      ; SI Global Mode
SISTR   equ     REGB+$06e6      ; SI Status
SICMR   equ     REGB+$06e7      ; SI Command
SICR    equ     REGB+$06ec      ; SI Clock Routing
SIRP    equ     REGB+$06f0      ; SI RAM Pointer
SIRAM   equ     REGB+$0700      ; SI Routing RAM

* --------------------------------------------------------------------
    section 14,,D

Rx_BUF  ds.b    1        ; External Receive Buffer
Tx_BUF  ds.b    1        ; External Tranmit Buffer

*    section 15,,D
RBASE   ds.l    1        ; store RXBD base
TBASE   ds.l    1        ; store TXBD base
* --------------------------------------------------------------------

