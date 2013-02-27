* ====================================================================
*
* M360qdser.INC - SERIAL I/O driver for Motorola 360 QUADS  BOARD *
* Module Name:  BOARDSUP
* Module Desc:  include file (as hwdrv.inc) for board.src or moto.src
* Author:       Stephen Clevenger
* Created:      November 1990, Revision: 1.0
*
*	Cleanup and add to clearcase Apr 95 ..Ray
*
*       New rev #5.1    G. Boehm, MRI, April 24, 1991
*                       Put under SCCS
*
*	Rev	5.2     Modified for QUAD360 support. 
*			these modification were carried out after
*			product release and were incorporateed from 
*			the Tech support organisation.  ...Ray
*
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
* Note:  The Motorola 360 QUICC  can accomplish I/O via an on-
*        chip peripheral.  This file contains I/O routines which use
*        these resources as a serial interface to the XRAY Debugger-
*        Monitor. This driver is specific to the QUADS.
*
* ====================================================================

*---------------------------------------------------------------------
* RxBD / TxBD Register Bit Masks 
*---------------------------------------------------------------------
* TxBD status bit 15 - buffer ready 0=data sent
TDRE    equ $F      ; Transmit Data Register Empty Flag

* RxBD status bit 15 - buffer empty 0=buf full
RDRF    equ $F      ; Receive Data Register Full Flag
*---------------------------------------------------------------------
* RxBD Error Masks
*---------------------------------------------------------------------
BR  equ 32          ; Break Received
FR  equ 16          ; Framing Error 
PR  equ 8           ; Parity Error
OV  equ 2           ; Overrun Error 
CD  equ 1           ; Carrier Detect Lost 
ERR equ OV+FR+PR    ; Detect Parity, Framing, and Overrun Errors


*---------------------------------------------------------------------
* Interrupt Level equate for Serial IO
*---------------------------------------------------------------------
*  All INRQ interrupts are level 4.  Monitor SR mask level must be < 4.

*---------------------------------------------------------------------
*   External References
*---------------------------------------------------------------------
*   None
    
* ====================================================================
        section 9,,C
* ====================================================================
*
*  QUICC Initialization Routine
*
* ====================================================================

IO_INIT: 
    movem.l D0/D1/d2/A0/A1,-(SP)   ;

  ifne BASE_ADDR_CAL
* Read MBAR for slave 1
*    movec      sfc,d1   ; save function codes
*    movec      dfc,d2
*    movea.l    #$7,a0   ; new FC value
*    movec      a0,sfc   ; set new FC's
*    movec      a0,dfc
*    move.w     #SLAVE1_MBARE_BIT,a0 ; slave 1 select bit
*    moves.w    a0,($3ff08).l  ; select slave 1
*    moves.l    ($3ff04).l,a0  ; get base address
*    movec      d1,sfc   ; restore FC's
*    movec      d2,dfc
*    move.l     a0,d1
*    andi.w     #$e000,d1   ; get rid of non-address bits
*    move.l     d1,d2       ; keep a copy of base address
*    addi.l     #SCC_PB,d2  ; get PB address for this SCC
*    move.l     d2,a0       ; save for addressing, 
*    clr.l      d2
*    move.w     (RxBD,a0),d2; move RxBD base to a0
*    or.l       d1,d2       ; or in high order address bits
*    move.l     d2,RBASE    ; save it
*    clr.l      d2
*    move.w     (TxBD,a0),d2; move TxBD base to a0
*    or.l       d1,d2       ; or in high order address bits
*    move.l     d2,TBASE

	move.l	#DPRBASE+$0,RBASE
	move.l	#DPRBASE+$8,TBASE

  elsec
    move.l  #Base,A0        ; Move Base address to A0
  endc
  
    cmp.l   #0,D0
    bne INIT_RESET       ; NOT FIRST INIT (call INIT_RESET with A0=base adr)
  
**  Register Initializations

	move.w	#$0740,SDCR
	move.w	#$0000,RxBD
	move.w	#$0008,TxBD
	move.w	#$0001,CR
	move.b	#$15,RFCR
	move.b	#$15,TFCR
	move.w	#$0001,MRBLR
	move.w	#$0000,MAX_IDL
	move.w	#$0001,BRKCR
	clr.w	PAREC
	clr.w	FRMEC
	clr.w	NOSEC
	clr.w	BRKEC
	clr.w	UADDR1
	clr.w	UADDR2
	move.w	#$8000,RCCR1
	move.w	#$8000,RCCR2
	move.w	#$8000,RCCR3
	move.w	#$8000,RCCR4
	move.w	#$8000,RCCR5
	move.w	#$8000,RCCR6
	move.w	#$8000,RCCR7
	move.w	#$8000,RCCR8
	move.w	#$C0FF,RCCM

    move.l  #$00000020,GSMR_H     ; Rx FIFO set to 8 bit wide
    move.l  #$00028004,GSMR_L     ; TDCR ='10', txclk is 16* clock mode
	move.w	#$3000,PSMR

	or.w	#$0003,PAPAR				; Valid only for SCC1
    clr.l   SICR            			; route brg1 to all the SCCs
	move.l	#$010000+(38*2),BRGC1		; set BRGC1 to 38400 (x16) bps
*	move.l	#$010000+(12*2),BRGC1		; set BRGC1 to 115200 (x16) bps
*	move.l	#$010000+(155*2),BRGC1		; set BRGC1 to 9600 (x16) bps

                            
* Init Rx Buffer Descriptor
    move.l  RBASE,a0
    move.w  #$E000,(BD_STAT,A0)    ; E = 1, X = 1, W = 1, I = 0, C = 0, A = 0
    move.w  #$01,(BD_LEN,A0)
    lea     Rx_BUF,A1           ; Input Buffer
    move.l  A1,(BD_PTR,A0)

* Init Tx Buffer Descriptor
    move.l  TBASE,a0
    move.w  #$6800,(BD_STAT,A0)    ; R = 0, X = 0, W = 1, I = 0, CR = 0, A = 0,
    move.w  #$01,(BD_LEN,A0)  ; P = 0
    lea     Tx_BUF,A1           ; Output Buffer
    move.l  A1,(BD_PTR,A0)

    move.b  #$00,(SCCM)      ; Mask out SCC Interrupts
    move.b  #$FF,(SCCE)      ; Clear SCC Event Register

  ifne CONIO&INTRPT
    ori.w   #$1000,(BD_STAT,A0); Interrupt when Buffer Full (I = 1)
    bset.b  #0,(SCCM,A0)    ; Unmask Rx Interrupt
    bsr INT_INIT_LOCAL      ; Write IMR to enable SCCx interrupt
    move.w  (GIMR,A0),D1    ; Assumes GIMR already set-up
    andi.l  #$E0,D1         ; Mask V7-V5 bits
   ifeq SCCx-1              ; Concatenate bits of vector
    ori.w   #$0D,D1         ; SCC1 vector $0D supplied by controller
   endc 
   ifeq SCCx-2  
    ori.w   #$0A,D1         ; SCC2 vector $0A supplied by controller
   endc 
   ifeq SCCx-3  
    ori.w   #$08,D1         ; SCC3 vector $08 supplied by controller
   endc 
    lsl.l   #2,D1           ; Calculate Vector Offset
    move.l  D1,A1           ;
    move.l  #RX_INT,(A1)    ; 
  endc				; CONIO&INTRPT

    move.l  #$00028034,GSMR_L   ; enable transmitter and receiver

    movem.l (SP)+,D0/D1/d2/A0/A1   ;                  

    rts
    
    

INIT_RESET:                 ; reset port from aux-mon (interrupt, etc)
			    ; enter with A0=base address
  ifne	CONIO&INTRPT		; skip Int enable if int supported
    bset.b  #0,SCCM    ; Unmask Rx Interrupt
  endc
    movem.l (SP)+,D0/D1/A0/A1   ;                  
    rts



IO_TERM:			;reset port to aux-mon (disable int, etc)
      ifne	CONIO&INTRPT		; skip if int not supported

	movem.l	D0/A0,-(SP)
	
*For QUADS
* for now avoid base addr calculation, just hard code BD base

        ifne BASE_ADDR_CAL
          move.l  TBASE,a0        ; get TX BD address for this SCC
        elsec
          move.l  #Base,A0        ; Move Base address to A0
        endc
        
        bclr.b  #0,(SCCM)    ; Disable Rx Interrupt
        movem.l (SP)+,D0/A0                  

      endc			; CONIO&INTRPT
	
      rts
 
 
 
IO_BAUD:
    RTS             ; allows selection of baud change
  
* ====================================================================
*
* Polled output
*
*  -> D1 = character to transmit
*  <- D0 = RET_OK (transmitted ok)
*      ER_TBF (transmit buffer full)
*
* ====================================================================

CON_OUT:                    ; Non-packetized output
    movem.l D1/D2/A0/A1,-(SP)  ; Save D2/A0/A1
*QUADS*
* for now avoid base addr calculation, just hard code BD base

  ifne BASE_ADDR_CAL
    move.l  TBASE,a0        ; get TX BD address for this SCC
  elsec
    move.l  #Base,A0        ; Move Base address to A0
  endc
    move.w  (BD_STAT,A0),D2    ; Read status into D2
    btst    #TDRE,D2        ; Buffer Empty?
    bne CON_OUT_FAIL        ; If not, ER_TBF condition exists
                            ; Note: Other Tx conditions may be checked here 

    move.l  (BD_PTR,A0),A1; Move address of Tx_BUF to A1
    move.b  D1,(A1)         ; Write D1 character to Output Buffer
    ori.w   #$8000,(BD_STAT,A0); Signal Tx Buffer Ready
                            ; Wrap bit set on this TxBD?
                            
    moveq   #RET_OK,D0      ; Presume RET_OK Condition
    bra CON_OUT_RET

CON_OUT_FAIL:
    moveq   #ER_TBF,D0      ; Set ER_TBF Condition (buffer full)

CON_OUT_RET:
    movem.l (SP)+,D1/D2/A0/A1  ; Restore D2/A0/A1
    rts                     ; Return

* ====================================================================
*
* Polled input
*
*  <- D1 = received character
*     D0 = RET_OK (received ok)
*      ER_NCA (no character available)
*      ER_REC (receive error)
*
* ====================================================================

  ifne  CONIO&POLLED
CON_IN:                     ; Character polling routine
    movem.l D2/D3/A0/A1,-(SP)  ; Save D2/D3/A0/A1
*QUADS*
* for now avoid base addr calculation, just hard code BD base

  ifne BASE_ADDR_CAL
    move.l  RBASE,a0        ; get RX BD address for this SCC
  elsec
    move.l  #Base,A0        ; Move Base address to A0
  endc
    moveq   #ER_NCA,D0      ; Presume ER_NCA condition (no character)

    move.w  (BD_STAT,A0),D2    ; Read status into D2
    btst    #RDRF,D2        ; Character available?
    bne CON_IN_RET          ; No, so exit (CON_IN_RET)

    move.l  (BD_PTR,A0),A1; Move address of Rx_BUF to A1
    move.b  (A1),D1         ; Read character from Input Buffer into D1
    ori.w   #$8000,(BD_STAT,A0); Signal Rx Buffer Emptied

    andi.w  #ERR,D2         ; Mask Receive Error(s):  Overrun (OV), Parity 
                            ; (PR), and Frame (FR).
    bne CON_IN_FAIL         ; Errors?
                            ; Wrap bit set on this RxBD?

    moveq   #RET_OK,D0      ; Set RET_OK condition
    bra CON_IN_RET

CON_IN_FAIL: 
    moveq   #ER_REC,D0      ; Set ER_REC condition 
CON_IN_RET:
    movem.l (SP)+,D2/D3/A0/A1   ; Restore D2/D3/A0/A1
    rts                     ; Return
  endc                      ; Polled Routine


* ====================================================================
*
* Interrupt driven input
*
*  <- D1 = received character
*     D0 = RET_OK (received ok)
*      ER_REC (receive error)
*      ER_HLT (halt code)
*
* ====================================================================

  ifne CONIO&INTRPT
RX_INT:                     ; Receive interrupt routine
    movem.l D0/D1/A0/A1,-(SP)   ; Save D0/D1/A0/A1
*QUADS*
* for now avoid base addr calculation, just hard code BD base

  ifne BASE_ADDR_CAL
    move.l  RBASE,a0        ; get RX BD address for this SCC
  elsec
    move.l  #Base,A0        ; Move Base address to A0
  endc
    btst.b  #0,(SCCE)    ; Valid Rx interrupt?
                            ; Note:  Other SCC interrupts may be 
                            ; supported here.
    beq RX_END              ; No, so exit (RX_END)
    bset.b  #0,(SCCE)    ; Clear Rx Event in SCCE 
    move.w  (BD_STAT,A0),D0    ; Read status into D0
    btst    #RDRF,D0        ; Character Available?
    bne RX_END              ; No, so exit (RX_END)

    move.l  (BD_PTR,A0),A1    ; Move address of Rx_BUF to A1
    move.b  (A1),D1             ; Read character from Input Buffer into D1
    ori.w   #$8000,(BD_STAT,A0)    ; Signal Rx Buffer Emptied

    andi.w  #ERR,D0         ; Mask Receive Error(s):  Overrun (OV), Parity 
                            ; (PR), and Frame (FR).
    bne RX_ERROR            ; Errors?
                            ; Wrap bit set on this RxBD?
                            
    moveq   #RET_OK,D0      ; Set RET_OK condition
    bsr MO_START+MORXCHR    ; Call Monitor to read character

    cmp.b   #ER_HLT,D0      ; Check for Halt (Break/Abort)
    bne RX_END              ; No Halt, so exit (RX_END) 

    bsr INT_CLR_LOCAL           ; Interrupt controller support
    movem.l (SP)+,D0/D1/A0/A1   ; Restore D0/D1/A0/A1
    bra MO_START+MOHALT         ; Call Monitor @ MOHALT

RX_ERROR:
    moveq   #ER_REC,D0      ; Set ER_REC condition 
    bsr MO_START+MORXCHR    ; Call Monitor to read character

RX_END:
    bsr INT_CLR_LOCAL           ; Interrupt controller support
    movem.l (SP)+,D0/D1/A0/A1   ; Restore D0/D1/A0/A1
    rte
  

*-============================================================
*
* INT_INIT_LOCAL and INT_CLR_LOCAL are local substitutes for
* INT_INIT and INT_CLR, which are normally located in board.src
* (or moto.src).  They are local here because they are so 
* specific to the SCC I/O, not to the board.  Since SCC I/O
* cannot be used by other boards, there is no need to call the
* board specific versions.  Furthermore, the INT_INIT and INT_CLR
* routines in moto.src support the 68681 DUART, not the SCC, so
* they would be bad to call here.
*
* On entry, A0 contains the peripheral base address which is
* represented in the BAR.
*
*-============================================================

INT_INIT_LOCAL:				
   ifeq SCCx-1
	ori.w	#$2000,(IMR,A0); Set SCC1 interrupt in IMR (@ $816)
   endc
   ifeq SCCx-2
	ori.w	#$400,(IMR,A0)	; Set SCC2 interrupt in IMR (@ $816)
   endc
   ifeq SCCx-3
	ori.w	#$100,(IMR,A0)	; Set SCC3 interrupt in IMR (@ $816)
   endc
	rts			; normal init ok

INT_CLR_LOCAL:				
   ifeq SCCx-1
	ori.w	#$2000,(ISR,A0); Clear SCC1 interrupt in ISR (@ $818)
   endc
   ifeq SCCx-2
	ori.w	#$400,(ISR,A0)	; Clear SCC2 interrupt in ISR (@ $818)
   endc
   ifeq SCCx-3
	ori.w	#$100,(ISR,A0)	; Clear SCC3 interrupt in ISR (@ $818)
   endc
	rts			; normal interrupt handling ok


  endc                      ; CONIO&INTRPT 
