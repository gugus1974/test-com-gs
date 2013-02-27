*---------------------------------------------------------------------------
*	%W%  %G%
* monvisi.inc - XRAYMON visible declarations
*
* $Author:   Kjell Christenson, DIGITAILOR AB  $
*
* $Log:   F:/develop/xray/xhm/asm68k/monvisi.inv  $
*
* Copyright (c) 1991-1996  Microtec Research, Inc.
* All rights reserved
*
*	Rev 8.4		Ray ,MRI, Nov 9 1992
*			More MTD merge
*	Rev 7.2		Paul, MRI, Mar 04, 1992
*			Added SRV_ equs
*
*	Rev 7.2		Paul, MRI, Feb 12, 1992
*			UTL_TRACE added for rtos trace testing. Also CORTOS
*			now two bits and CORTOS_EX one of them. Added
*			MON_MTASK return code.
*
*	Rev 7.1		G. Boehm, MRI, November 12, 1991
*			Digitailor Merge (RTOS)
*			(merge Digitailor Revs 1.6-1.7)
*
*	Rev 6.2		G. Boehm, MRI, November 1, 1991
*			Put in copyright notice
*
*	Rev 4.2		G. Boehm, MRI
*			implement 68332
*
*	New Rev# 1.1  	G. Boehm, MRI	Oct 29, 1990 	Put under SCCS
* 
*    Rev 1.7   19 Jul 1991 10:20:30   kjell
* New breakpoint status defined: STAT_IA, used to report hardware breakpoints
*   when the execution is stopped *after* the execution of the instruction.
* 
*    Rev 1.6   27 Feb 1991 12:55:28   Kjell_Christenson,_DIGITAILOR_AB
* New constants: UTL_SETREG, UTL_GETREG, UTL_RESET, UTL_REINIT, EXE_EVENT, 
*   CORTOS, CH_FIRST, EXE_GO_MODE, EXE_TRACE_MODE, MODE_NORM, MODE_RTOS, 
*   IDLE_RX, IDLE_TX, IDLE_SUM, IDLE_INI, ER_TOUT, MON_MESSAGE, ER_MESSAGE, 
*   MON_PARSE, ER_PARSE, MON_VALUE, ER_VALUE.
* New fields in config-table: cfutlhk, cfsushk (replaced cfrmbz2, cfrmbz3)
* The definitions for all register numbers (REG_xx) are now available.
* 
*    Rev 1.5   06 Jun 1990 17:00:04   Paul_Kimelman,_MRI
* New config field CFEXTHK for extension hook (RMBZ2).
* New config field CFUSRCODE (RMBZ3) to supply start address of monitor-
*  support-code. This is only provided if it is immediately before mon_start.
*  It is used to protect writes into the Monitor and support code by XRAY.
* Two new reserved field added to config table (RMBZ2 & RMBZ3).
* 
*    Rev 1.4   25 May 1990 15:47:28   Kjell Christenson, DIGITAILOR AB
* New config field: CFENALVL to supply an enable level, (previous CFRMBZ1)
* 
*    Rev 1.3   20 May 1990 21:19:38   Kjell Christenson, DIGITAILOR AB
* New constants: MOREINIT, COCALLRST.
* 
*    Rev 1.2   31 Jan 1990  8:35:44   Kjell Christenson, DIGITAILOR AB
* New breakpoint type FLAG_IT, for temporary breakpoint.
* 
*    Rev 1.1   13 Dec 1989 15:05:18   Kjell Christenson, DIGITAILOR AB
* Floating point support option added: COFPU
* 
*    Rev 1.0   01 Nov 1989 16:20:18   Kjell Christenson, DIGITAILOR AB
* New constants: ER_SUM, EXT_ERR, MON_SUM_CODE, MON_SUM_WSP
* 
*    Rev 0.4   13 Oct 1989 17:26:56   Kjell Christenson, DIGITAILOR AB
* New configuration table entries: CFAUXVECT, CFEXECHK
* New configuration option: CFAUX2CH
* New exception type: EXEC_BRP, unknown breakpoint
* 
*    Rev 0.3   22 Sep 1989  9:18:26   Kjell Christenson, DIGITAILOR AB
* New constants: EXCP_ALIN,EXCP_FLIN,EXCP_FORM,MOALINE,MOFLINE,MOFORMAT
* The 80x86 entrys MODBLE & MONMI is set equal to 68k MOBUSERR & MOADRERR
* New in config table: CFABVECT, holds the abort vector number (68k only)
* The cpu family (68k=100H & 80x86=200H) is now included in the cpu type 
* 
*    Rev 0.2   15 Sep 1989 17:25:12   Kjell Christenson, DIGITAILOR AB
* New constants: COAUXMON, MORESTART
* 
*    Rev 0.1   13 Jul 1989 18:40:24   Kjell Christenson, DIGITAILOR AB
* Initial revision.
*
*---------------------------------------------------------------------------

* configuration table
		offset	0
CFWORKSP 	ds.l	1	* pointer to workspace
CFWSSIZ		ds.l	1	* workspace size (bytes)
CFCODESP 	ds.l	1	* pointer to codespace
CFCSSIZ 	ds.l	1	* codespace size
CFOPTS 		ds.w	1	* options
CFCPUTYP 	ds.w	1	* cpu type
CFDELAY 	ds.w	1	* delay loops before timeout
CFMAXBP 	ds.w	1	* max number of breakpoints
CFBPINSTR 	ds.w	1	* breakpoint instruction
CFBPVECT 	ds.w	1	* breakpoint vector
CFAUXVECT 	ds.w	1	* auxiliary monitor invoke vector
CFENALVL	ds.b	1	* enable level
CFRMBZ1		ds.b	1	* reserved, must be zero
CFMBUFSIZ 	ds.w	1	* message buffer size
CFIBUFSIZ 	ds.w	1	* input buffer size
CFSCHEME 	ds.w	1	* max encoding scheme
CFABVECT	ds.w	1	* abort vector
CFOUTPOLL 	ds.l	1	* output poll routine
CFINPOLL 	ds.l	1	* input poll routine
CFSETBPHK 	ds.l	1	* set breakpoint hook
CFAFFBPHK 	ds.l	1	* affect breakpoint hook
CFINITHK 	ds.l	1	* initialization hook
CFIDLEHK 	ds.l	1	* idle hook
CFINTRHK 	ds.l	1	* interrupt enable/disable hook
CFAUXMHK	ds.l	1	* auxiliary monitor hook
CFEXECHK	ds.l	1	* start/stop execution hook
CFHUMAN 	ds.l	1	* human component pointer
CFEXTHK		ds.l	1	* extension hook
CFUSRCODE 	ds.l	1	* start of user mon code area (for protect)
CFUTLHK		ds.l	1	* utility hook
CFSUSHK		ds.l	1	* suspend hook

* configuration options
COIBPSP	  equ	$01		* call bp-hook for instr bp
COIBPROM  equ	$02		* call bp-hook is instr bp set in ROM
COINIVECT equ	$04		* initialize the IVT (all vectors)
COHUMAN	  equ	$08		* start in human mode (not impl)
COCHKSUM  equ	$10		* perform checksum calculations (not impl)
COAUXMON  equ	$20		* auxiliary monitor used
COAUX2CH  equ	$40		* auxiliary monitor on separate channel
COFPU	  equ	$80		* floating point unit avail. (68881/68882)
COXIDLE	  equ	$100		* call idle hook during init & checksum
COCALLRST equ	$200		* call instruction used to reach MORESTART
CORTOS	  equ	$800		* running in RTOS (multitaksing) mode
CORTOS_EX equ	$C00		* partially in RTOS mode (until go)
COPAKSEQ  equ   $1000		* use packet numbering
* cpu types
CCPU68000  equ	$101		* 68000 
CCPU68008  equ	$102		* 68008 
CCPU68010  equ	$103		* 68010 
CCPU68070  equ	$104		* 68070 (Signetics)
CCPU68012  equ	$105		* 68012 
CCPU68020  equ	$106		* 68020 
CCPU68030  equ	$107		* 68030 
CCPU68040  equ	$108		* 68040 
CCPU68332  equ	$109		* 68332 
CCPU68360  equ	$10A		* 68360 


* monitor call return codes
RET_OK	equ	0		* ok
ER_IHK	equ	1		* bad return from init hook
ER_NEW	equ	2		* not enough workspace
ER_CFG	equ	3		* bad config table, missing or bad entries
ER_HUM	equ	4		* bad or missing human component
ER_TMB	equ	5		* too many breakpoints reported
ER_HLT	equ	6		* halt message was received (from MORXCHR)
ER_IBF  equ	7		* interrupt input buffer full
ER_SUM	equ	8		* bad monitor code checksum found at init

* input codes to some monitor calls and hooks
CH_NORM	   equ	0		* normal chr in a msg (polled input/output)
CH_LAST	   equ	1		* last chr in a message (polled input/output)
CH_FIRST   equ	2		* first chr in a message (polled input/output)
AFF_CLR	   equ	0		* clear breakpoint (affect brp hook)
AFF_ARM	   equ	1		* arm breakpoints (affect brp hook)
AFF_DIS	   equ	2		* disarm breakpoints (affect brp hook)
INT_DIS	   equ	0		* disable interrupts (intr hook)
INT_ENA	   equ	1		* enable interrupts (intr hook)
AUX_ENTER  equ	0		* enter auxiliary monitor (auxmon hook)
AUX_EXIT   equ	1		* exit auxiliary monitor (auxmon hook)
EXE_STOP   equ	0		* stop execution (exec hook)
EXE_START  equ	1		* start execution (exec hook)
EXE_EVENT  equ	2		* start execution + wait for event (exec hook)
EXE_GO_MODE equ 0		* full speed execution (exec hook)
EXE_TRACE_MODE equ 1		* single step execution (exec hook)
UTL_GETREG equ	0		* get register (utility hook)
UTL_SETREG equ	1		* set register (utility hook)
UTL_RESET  equ	2		* reset command issued (utility hook)
UTL_REINIT equ	3		* monitor re-initialized (utility hook)
UTL_TRACE  equ  4		* trace test for rtos mode (utility hook)
EXT_REPEAT equ	0		* repeat previous command (extension hook)
EXT_STAT   equ	1		* show status (extension hook)
EXT_SS	   equ	2		* set status (extension hook)
EXT_BC 	   equ	3		* break complex (extension hook)
IDLE_RX	   equ  0		* waiting for rec-char (idle hook)
IDLE_TX	   equ	1		* waiting for xmit-char (idle hook)
IDLE_SUM   equ  2		* calculating checksum (idle hook)
IDLE_INI   equ	3		* initializing (idle hook)

* requests to MOSERVICE entry
SRV_STEP   equ	1		* request to perform a step (on some targets)
SRV_IO     equ	2		* monio entry. This passes a MON_IO request
SRV_BRKSET equ	3		* define breakpoint (from RTOS stuff)

* output codes, returned from user hooks
ER_TBF	equ	1		* output: UART transmit buffer full, try again
ER_REC	equ	1		* input: rec-err,bad-chr,pty,overrun etc.
ER_NCA  equ	2		* input: no character available
ER_TOUT equ	9		* input: timeout (in RTOS mode only)
ER_BAD_DATA equ 2		* extension: bad data
ER_LIMIT    equ	4		* extension: monitor limitation
ER_MESSAGE  equ	12 		* extension: message returned
ER_PARSE    equ	13		* extension: parse & invoke again
ER_VALUE    equ 14		* extension: value returned

* modes returned from MORXCHR
MODE_NORM equ	0		* normal mode
MODE_RTOS equ	1		* in RTOS mode

* breakpoint types (input to set/affect breakpoint hooks)
FLAG_I	equ	$01		* instruction breakpoint
FLAG_IT	equ	$11		* temporary instruction breakpoint
FLAG_R	equ	$02		* read breakpoint
FLAG_W	equ	$04		* write breakpoint
PORT_I	equ	$08		* port input breakpoint
PORT_O	equ	$80		* port output breakpoinT
* breakpoint types (to report special breakpoints with the MOSPBREAK call)
STAT_I	equ	$10		* break on instruction fetch
STAT_IA	equ	$11		* break on instruction fetch,when stop after BP
STAT_R	equ	$20		* break on read
STAT_W	equ	$40		* break on write
STAT_PI	equ	$08		* break on port input
STAT_PO	equ	$80		* break on port output

* get_status status bits
COMINCNF equ 	$8000		* minimum monitor conf (output option bit)

* stop info status
STAT_NOINFO equ 0		* no more info
STAT_BRKP   equ 1		* breakpoint info
STAT_EXCP   equ 2		* exception info

* return codes, from monitor commands
MON_OK	     equ  0		* ok result
MON_BAD_ADDR equ  1		* bad address
MON_BAD_DATA equ  2		* bad data
MON_ROM_ADDR equ  3		* dest address is rom
MON_LIMIT    equ  4		* monitor limitation
MON_BAD_REG  equ  5		* bad register
MON_BUF_OFL  equ  6		* I/O buffer overflow, too much data
MON_HALT     equ  7		* exec stopped due to user halt
MON_EXCP     equ  8		* exec stopped due to exception
MON_BRKP     equ  9		* exec stopped due to instr breakp
MON_SUM_CODE equ  10		* code checksum error
MON_SUM_WSP  equ  11		* workspace checksum error
MON_MESSAGE  equ  12		* message returned (ext cmd)
MON_PARSE    equ  13		* parse string & invoke again (ext cmd)
MON_VALUE    equ  14		* value returned (ext cmd)
MON_IO       equ  15		* request for I/O services
MON_MTASK    equ  16		* multitasking mode enabled

* exception types (STAT_EXCP codes)
EXCP_UNKN    equ  $8000		* unknown exception 
EXCP_BUS     equ  $8001		* bus exception (68K) 
EXCP_ADDR    equ  $8002		* address exception (68K) 
EXCP_OPC     equ  $8003		* illegal opcode 
EXCP_DIVZ    equ  $8004		* division with zero 
EXCP_BND     equ  $8005		* bound/chk exception 
EXCP_OVFL    equ  $8006		* overflow exception 
EXCP_PRIV    equ  $8007		* privilege exception 
EXCP_TRC     equ  $8008		* trace (single step) exception 
EXCP_COPR    equ  $8009		* coprocessor exception 
EXCP_DBLE    equ  $800A		* double fault exception 
EXCP_NMI     equ  $800B		* nmi trap 
EXCP_ALIN    equ  $800C		* A-line exception (68K)
EXCP_FLIN    equ  $800D		* F-line exception (68K)
EXCP_FORM    equ  $800E		* Format exception (68K)
EXCP_BRP     equ  $800F		* unknown breakpoint exception
EXCP_BASE    equ  $8100		* unknown exception vector base (+0..+FF)

* XRAY register numbers (same definitions as in i682dec.h) 
REG_D0   equ	1
REG_D1   equ	2
REG_D2   equ	3
REG_D3   equ	4
REG_D4   equ	5
REG_D5   equ	6
REG_D6   equ	7
REG_D7   equ	8
REG_A0   equ	9
REG_A1   equ	10
REG_A2   equ	11
REG_A3   equ	12
REG_A4   equ	13
REG_A5   equ	14
REG_A6   equ	15
REG_A7   equ	16
REG_USP	 equ	17	; user stack pointer
REG_SSP  equ	18	; equivalent to REG_ISP (68000,08,10,12)
REG_MSP  equ	19	; master stack pointer (68020,30,40...)
REG_ISP  equ	20	; interrupt stack pointer (68020,30,40...)
REG_SR   equ	21	; status register
REG_VBR  equ	22	; vector base register (68010,20,30,40...)
REG_MYPC equ	23	; saved PC
REG_SFC  equ	25	; Alternate function code reg (68020,30,40...)
REG_DFC  equ	26	; Alternate function code reg (68020,30,40...)
REG_CACR equ	27	; cache control register (68020,30,40...)
REG_CAAR equ	28	; cache addrees register (68020,30,40...)
* 29-63 reserved
REG_FP0	  equ	48	; floating point registers
REG_FP1	  equ	49
REG_FP2	  equ	50
REG_FP3	  equ	51
REG_FP4	  equ	52
REG_FP5	  equ	53
REG_FP6	  equ	54
REG_FP7	  equ	55
REG_FPCR  equ	56	; floating point control reg
REG_FPSR  equ	57	; floating point status reg
REG_FPIAR equ	58	; floating point address reg


* monitor calls (offset from start)
MOINIT	  equ	4		* initialize
MORXCHR	  equ	8		* receive char ready
MOHALT	  equ	$0C		* user halt
MOSPBRK	  equ	$10		* report special breakpoint
MOSPEXCP  equ	$14		* report special exception
MOBREAK	  equ	$18		* stop after breakpoint/exception
MOUNEXCP  equ	$1C		* unknown exception
MOBUSERR  equ	$20		* bus error exception (68k) or ..
MODOUBLE  equ	 $20		* ..double fault exception (80x86)
MOADDRERR equ	$24		* address error exception (68k) or ..
MONMI	  equ	 $24		* ..nmi exception (80x86)
MOILLOPC  equ	$28		* illegal opcode
MODIVZ	  equ	$2C		* division by zero
MOBOUND	  equ	$30		* bound/chk exception
MOOVFL	  equ	$34		* overflow exception
MOPRIV	  equ	$38		* privilege/protection exception
MOTRACE	  equ	$3C		* trace/single-step exception
MOCOPROC  equ	$40		* coprocessor exception
MOALINE	  equ	$44		* A-line exception (68k)
MOFLINE	  equ	$48		* F-line exception (68k)
MOFORMAT  equ	$4C		* format exception (68k)
MORESTART equ	$50		* restart from auxiliary monitor
MOREINIT  equ	$54		* monitor re-initialize
