/*=============================================================================================*/
/* ATR boards hardware routines for the COM board                                              */
/* Implementation file (hw_com.c)													           */
/* 																					           */
/* Version 1.1																		           */
/*																					           */
/* (c)1997/8 Ansaldo Trasporti														           */
/* Written by Giuseppe Carnevale													           */
/*																					           */
/* Hardware resources reserved for an external monitor:								           */
/* - BRGC1 baud rate generator														           */
/* - the first 2 BDs																           */
/* - SCC1																			           */
/*																					           */
/* The shell I/O is directed to SCC1 if it not yet initialized (no external monitor) or to     */
/* SMC1 otherwise.																	           */
/*
/*   07/09/2001     Napoli     Schiavo
/*    - si agg. commento per la config. della SPI (non si compila)
/*                                                                                            */
/*=============================================================================================*/


/*=============================================================================================*/
/* Includes */

#include "lc_tgt.h"	   /* Hardware-dependent items */

#include <string.h>

#include "sio.h"
#include "sio_cpm.h"
#include "atr_hw.h"
#include "am29f040.h"
#include "x25642.h"

#include "hw_com.h"
#include "cpmdrv.h"
#include "sio_cpm.h"


/*=============================================================================================*/
/* Globals */

/*---------------------------------------------------------------------------------------------*/

volatile unsigned short laco_counter = 0;		/* MVBD LACO rising edge counter */
volatile unsigned short lbco_counter = 0;		/* MVBD LBCO rising edge counter */
volatile unsigned short lajo_counter = 0;		/* MVBD LAJO both edges counter  */
volatile unsigned short lbjo_counter = 0;		/* MVBD LBJO both edges counter  */
volatile unsigned short shd_counter  = 0;		/* MVBD SHD both edges counter   */

tcn_event_handler 		laco_handler = 0;		/* MVBD LACO rising edge handler */
tcn_event_handler 		lbco_handler = 0;		/* MVBD LBCO rising edge handler */
tcn_event_handler 		lajo_handler = 0;		/* MVBD LAJO both edges handler  */
tcn_event_handler 		lbjo_handler = 0;		/* MVBD LBJO both edges handler  */
tcn_event_handler 		shd_handler  = 0;		/* MVBD SHD both edges handler   */

led_handler				mvb_ba_led_handler = 0;	/* MVB BA led blinker            */

volatile unsigned short	vme_berr_counter = 0;	/* VME bus error counter         */
bus_error_handler		vme_berr_handler = 0;	/* VME bus error handler         */

const unsigned long		atr_hw_code   = COM_CODE;
const char	        	*atr_hw_name  = "COM";
const unsigned long 	atr_cpu_clock = 24000000;


/*---------------------------------------------------------------------------------------------*/

volatile QUICC	*quicc;						/* pointer to the QUICC DP-RAM structure */
unsigned short	saved_sr;					/* SR saved during interrupt disable */
unsigned long	out_port_shadow[2];			/* output ports shadow */
unsigned long	out_port_xor[2] = {0, 0};	/* output ports XOR mask */


/*=============================================================================================*/
/* MVBC interrupt handlers */

/*---------------------------------------------------------------------------------------------*/
static void mvbc_int0_kernel_isr(void)
{
//	set_out_port(1, DOP1_OUTD19, DOP1_OUTD19);
	_lc_intr_hdlr_0_k(0);
//	set_out_port(1, DOP1_OUTD19, 0);
}

/*---------------------------------------------------------------------------------------------*/
static void mvbc_int1_kernel_isr(void)
{
//	set_out_port(1, DOP1_OUTD20, DOP1_OUTD20);
	_lc_intr_hdlr_1_k(0);
//	set_out_port(1, DOP1_OUTD20, 0);
}

/*---------------------------------------------------------------------------------------------*/


/*=============================================================================================*/
/* MVBD interrupt handlers */

/*---------------------------------------------------------------------------------------------*/
static void mvbd_laco_isr(void)
{
	laco_counter++;
	if (laco_handler) (*laco_handler)();
}


/*---------------------------------------------------------------------------------------------*/
static void mvbd_lbco_isr(void)
{
	lbco_counter++;
	if (lbco_handler) (*lbco_handler)();
}


/*---------------------------------------------------------------------------------------------*/
static void mvbd_lajo_isr(unsigned short param)
{
	param = param;
	lajo_counter++;
	if (lajo_handler) (*lajo_handler)();
}


/*---------------------------------------------------------------------------------------------*/
static void mvbd_lbjo_isr(unsigned short param)
{
	param = param;
	lbjo_counter++;
	if (lbjo_handler) (*lbjo_handler)();
}


/*---------------------------------------------------------------------------------------------*/


/*=============================================================================================*/
/* X25642 serial E2PROM support functions. Warning: call them only from a task! */

static short spi_sem;	/* semaphore used to sync with the SPI reception */
static short spi_mutex;	/* semaphore used as a mutex on SPI accesses     */


/*---------------------------------------------------------------------------------------------*/
static void spi_rx_cb(int sxc, const void *buff, unsigned short size, unsigned short status)
{
	pi_post_semaphore(spi_sem, 0);
}


/*---------------------------------------------------------------------------------------------*/
short x25642_select(char select_flag)
{
	/* move the /CS pin to select the X25642 and get or release the mutex */
	if (select_flag) {
		pi_pend_semaphore(spi_mutex, PI_FOREVER, 0);
		quicc->pip_pbdat &= ~1;
	}
	else {
		quicc->pip_pbdat |= 1;
		pi_post_semaphore(spi_mutex, 0);
	}

	return X25642_OK;
}


/*---------------------------------------------------------------------------------------------*/
static short x25642_transfer(void *buf, unsigned short size)
{
	/* do the transfer */
	cpm_spi_send_receive(buf, buf, size);
	pi_pend_semaphore(spi_sem, PI_FOREVER, 0);

	return X25642_OK;
}


/*---------------------------------------------------------------------------------------------*/


/*=============================================================================================*/
/* Private routine used to reset the external peripherals */

static void reset_ext_peri(void)
{
//	/* reset the MVBC */
//	*(volatile unsigned short *)(TM0_BASE + 0xFF80L) = 0;
}


/*=============================================================================================*/
/* Initializes the hardware */

void hw_init(unsigned long baud_rate)
{
	volatile QUICC 	*q;		/* QUICC pointer   */
	int			   	i;		/* generic integer */
	short			err;	/* error code      */

	/* first at all we need to recover the pointer to the QUICC DP-RAM */
	q = quicc = asm(volatile QUICC*, 
		"	move.l  #7,d0           ; '0111' is the function code for CPU Space		",
		"	movec   d0,SFC          ; Store '0111' into both the Src Fcn Code Reg	",
		"	movec   d0,DFC          ; and the Dest. Fcn Code Reg					",
		"	moves.l $03FF00,a0      ; Move MBAR into a0								",
		"	move.l  a0,d0															",
		"	andi.l  #$ffffe000,d0   ; Extract the base address from MBAR			"
	);

	/* This is a good place to enable interrupts if for some reason this is not yet done */
	asm(void, "	move.w	#$2000,SR");

	/* Serial monitor compatibility: reserve space for 2 buffer descriptors */
	alloc_quicc_dp_ram(2 * sizeof(QUICC_BD));

	/* shut off all the leds */
	set_out_port(1, DOP1_LD4a_|DOP1_LD4b_|DOP1_LD5a_|DOP1_LD5b_|DOP1_LD6a_|DOP1_LD6b_,
					DOP1_LD4a_|DOP1_LD4b_|DOP1_LD5a_|DOP1_LD5b_|DOP1_LD6a_|DOP1_LD6b_);

	/* initialize the CPM:                                                              */
	/* use the SCC grouped mode with SCCa = SCC3, SCCb = SCC4, SCCc = SCC1, SCCd = SCC2 */
	/* IRL = 6, Highest Priority = SCCa, interrupt vector = 0x80                        */
	/* RISC timer tick time = 1ms                                                       */
	cpm_init(CICR_SCCa_SCC3 | CICR_SCCb_SCC4 | CICR_SCCc_SCC1 | CICR_SCCd_SCC2 |
			 CICR_IRQ(CPM_ILVL) | CICR_HP(0x1E) | CICR_VB(0x80 >> 5));

	/* initialize the RISC Timers */
	cpm_risctimer_init(RISC_TIMERS_TICK_US, FALSE);

	/* reset the external peripherals */
	reset_ext_peri();

	/* initialize the serial interface if required */
	if (baud_rate) {
		int scc1_used = q->scc_regs[cpm_ch_scc(CPM_SCC1)].scc_gsmr.w.low != 0;
		sio_cpm_init(scc1_used ? CPM_SMC1 : CPM_SCC1, CPM_BRG2, baud_rate);
		sio_set_get(sio_cpm_get);
		sio_set_put(sio_cpm_put);
	}
	
	/* link the TIMER1 to the VRTX timer tick */
	cpm_timer_init(CPM_TIMER1, ui_timer, TRUE);
	cpm_timer_set_periodic(CPM_TIMER1, PI_TICK*1000);

	/* connect to the MVB line */
	set_out_port(0, DOP0_KMA|DOP0_KMB, DOP0_KMA|DOP0_KMB);

	/* attach the MVBC/D interrupt handlers */
	install_isr(24+MVBC_0_ILVL, mvbc_int0_kernel_isr);
	install_isr(24+MVBC_1_ILVL, mvbc_int1_kernel_isr);
	cpm_portc_connect(PORTC_LACO, CPM_PORTC_INT_HI2LO, mvbd_laco_isr, TRUE);
	cpm_portc_connect(PORTC_LBCO, CPM_PORTC_INT_HI2LO, mvbd_lbco_isr, TRUE);
	cpm_portc_enable(PORTC_LACO | PORTC_LBCO);

	/* install the serial E2PROM if present (check PB17 high) */
	if (q->pip_pbdat & (1<<17)) {

		/* create the semaphores used to syncronize with the SPI */
		spi_sem   = pi_create_semaphore(0, PI_ORD_FCFS, &err);
		spi_mutex = pi_create_semaphore(1, PI_ORD_FCFS, &err);

		if (!err) {
			/* use PB0 as /CS */
			q->pip_pbdat |= 1;
			q->pip_pbdir |= 1;
		
			/* initialize the SPI interface */
            /* il baud rate non pu• essere < di cpu_clk/64 */
			cpm_spi_conf(500000 /*2000000*/, CPM_SPI_CI_OPT+CPM_SPI_CP_OPT+CPM_SPI_REV_OPT+CPM_SPI_LEN_8_OPT,
						 spi_rx_cb, TRUE);

			/* install the driver providing the required functions */
			x25642_init(x25642_select, x25642_transfer);
		}
	}
}


/*=====================================================================================*/
/* Install the PIL timeout routines */

static void hw_background(void)
{
	static unsigned long	last_port0 = DIN0_SHD_;
	unsigned long			new_port0 = get_in_port(0);
	unsigned short			dr;
    static unsigned char toggle=0;	
    
    /* watchdog service */
	quicc->sim_swsr = 0x55;
	quicc->sim_swsr = 0xAA;

	/* SHD monitoring */
	if ((new_port0 ^ last_port0) & DIN0_SHD_) {
		/* close or re-open the MVB connection */
		set_mvb_connection(MVB_TS_ID, 0, shd_counter&1);
		set_mvb_connection(MVB_TS_ID, 1, shd_counter&1);

		/* signal the event to the application */
		shd_counter++;
		if (shd_handler) (*shd_handler)();
	}
	last_port0 = new_port0;

    led_user5(toggle & 0x02);
    toggle++;

//	/* RLD cleaning */
//	dr = *(unsigned short *)(TM0_BASE + 0xFF88UL);
//	if (!(dr & 1)) *(unsigned short *)(TM0_BASE + 0xFF88UL) = 0;
}

short hw_install_timeouts(void)
{
	short err, id;
	Check(pi_create_timeout(&id, hw_background, 0, PI_FOREVER));
	Check(pi_enable_timeout(id, ms2tick(100 /*500*/)));
error:
	return err;
}


/*=============================================================================================*/
/* Install the MVBC sampled interrupt routines */

short hw_install_lc_interrupts(void)
{
	short err;

	Check(lc_m_int_connect(MVB_TS_ID, mvbd_lajo_isr, LC_INT_EXT_3, 0, LC_KERNEL));
	Check(lc_m_int_connect(MVB_TS_ID, mvbd_lbjo_isr, LC_INT_EXT_2, 0, LC_KERNEL));

error:
	return err;
}


/*=============================================================================================*/
/* Line connection control routines */

/*---------------------------------------------------------------------------------------------*/
short get_mvb_connection(short ts_id, short line)
{
	if (line == 0) return (get_out_port(0) & DOP0_KMA) != 0;
	return (get_out_port(0) & DOP0_KMB) != 0;
}


/*---------------------------------------------------------------------------------------------*/
void set_mvb_connection(short ts_id, short line, short value)
{
	if (line == 0) set_out_port(0, DOP0_KMA, value ? DOP0_KMA : 0);
	else set_out_port(0, DOP0_KMB, value ? DOP0_KMB : 0);
}


/*---------------------------------------------------------------------------------------------*/


/*=============================================================================================*/
/* Reset and NMI routines */

/*---------------------------------------------------------------------------------------------*/
void hw_reset(void)
{
	/* wait for the output flush */
	sio_cpm_put_flush();

	/* disable interrupts, reset the external peripherals and reboot */
	pi_disable();
	reset_ext_peri();
	asm(void, "	reset");
}


/*---------------------------------------------------------------------------------------------*/
void hw_halt(void)
{
	/* wait for the output flush */
	sio_cpm_put_flush();

	/* disable everything else and wait for the watchdog */
	pi_disable();
	while (1);
}


/*---------------------------------------------------------------------------------------------*/
void hw_nmi(void)
{
	asm(void, "	illegal");
}


/*-------------------------------------------------------------------------------------*/
void hw_watchdog_service(void)
{
	/* watchdog service */
	quicc->sim_swsr = 0x55;
	quicc->sim_swsr = 0xAA;
}


/*=============================================================================================*/
/* Flash management */

/*---------------------------------------------------------------------------------------------*/
void hw_ram2rom(void)
{
	short	err;
	int		i;

	/* disable any interrupt source (just to be sure) */
	asm(void, "	move.w	#$2700,SR");

	/* write the memory contents to flash */
	Check(am29f040_chip_erase((void*)0x400000));
	for (i = 0; i < 64; i++) {
		led_user1(i & 1);
		led_user2(i & 2);
		led_user3(i & 4);
		led_user4(i & 8);
		led_user5(i & 16);
		led_user6(i & 32);
		Check(am29f040_write((void*)0x400000, i * (0x100000 / 64), (void*)(i * (0x100000 / 64)), 0x100000 / 64));
	}

error:
	if (err) asm(void, "	bgnd");
	hw_reset();
}


/*==============================================================================================*/
/* External data management */

static char ext_data_initialized = FALSE;	/* if TRUE we have been initialized */
static long ext_data_pos = -1;				/* position in the "file"           */

/*----------------------------------------------------------------------------------------------*/
short hw_ext_data_init(const void **data, long *size)
{
	/* fill the return fields */
	if (data) *data = (void*)0x0E0000;		/* the data is already copied to RAM */
	if (size) *size = 0x020000;				/* this is not the real size         */

	/* signal the initialization */
	ext_data_initialized = TRUE;

	return 0;
}


/*----------------------------------------------------------------------------------------------*/
short hw_ext_data_open(char erase_flag)
{
	short err = 0;	/* error code */

	/* proceed only if we have been initialized */
	if (!ext_data_initialized) return -2;

	/* critical section start */
	pi_disable();
	
	/* do not "open" the "file" if already used */
	if (ext_data_pos != -1) {
		pi_enable();
		return -1;
	}

	/* initialize the position and end the critical section */
	ext_data_pos = 0x020000L * 7;
	pi_enable();

	/* clear the memory if requested */
	if (erase_flag) Check(am29f040_sector_erase((void*)0x400000, 7));

error:
	if (err) ext_data_pos = -1;
	return err;
}


/*----------------------------------------------------------------------------------------------*/
short hw_ext_data_write(void *data, unsigned short data_len)
{
	short err = 0;	/* error code */

	/* do not write if closed */
	if (ext_data_pos < 0) return -1;

	/* write the data if specified */
	if (data) Check(am29f040_write((void*)0x400000, ext_data_pos, data, data_len));

error:
	/* update the position */
	if (err) ext_data_pos = -1;
	else ext_data_pos += data_len;

	return err;
}


/*----------------------------------------------------------------------------------------------*/
short hw_ext_data_close(void)
{
	/* set the pointer to a void value to close the "file" */
	ext_data_pos = -1;
	return 0;
}


/*=============================================================================================*/
/* I/O routines */

/*---------------------------------------------------------------------------------------------*/
char _getkey(void)
{
	return sio_get_key();
}


/*---------------------------------------------------------------------------------------------*/
#undef putc
int putc(int c, FILE *stream)
{
	return sio_put_char(c);
}

int fputc(int c, FILE *stream)
{
	return sio_put_char(c);
}


/*=============================================================================================*/
/* COM specific routines */

/*---------------------------------------------------------------------------------------------*/
/* Allocate a buffer in the QUICC DP-RAM (that memory cannot be freed) */

void *alloc_quicc_dp_ram(int size)
{
	static int p = 0;	/* position inside the DP-RAM      */
	int        v = p;	/* position of the allocated block */
	
	/* check for enough space */
	if (p + size > ((char*)&quicc->RESERVED1 - (char*)quicc)) return 0;
	
	/* update our position and return the pointer to it */
	p += size;
	return ((char*)quicc) + v;
}


/*---------------------------------------------------------------------------------------------*/
/* Install an interrupt handler */

#ifdef USE_VRTX_ISR_HANDLING

void **sys_load_vrtx_isr(int vector, void *isr_address);

void install_interrupt_handler(int vector, void *isr_address)
{
	sys_load_vrtx_isr(vector, isr_address);
}

#else

#pragma asm
	XREF	_v90k_interrupt_enter
	XREF	_v90k_interrupt_exit

hw_isr_start:
	jsr		_v90k_interrupt_enter
	movem.l	D0/D1/A0/A1,-(A7)
hw_isr_jsr:
	jsr		$12345678
	movem.l	(A7)+,D0/D1/A0/A1
	jmp		_v90k_interrupt_exit
hw_isr_end:
#pragma endasm

void install_isr(int vector, void *isr_address)
{
	void			*isr_start = asm(void *,       "	move.l	#hw_isr_start,D0");
	unsigned long	isr_len = asm(unsigned long,   "	move.l	#(hw_isr_end-hw_isr_start),D0");
	unsigned long	isr_jsr = asm(unsigned long,   "	move.l	#(hw_isr_jsr-hw_isr_start),D0");

	char *isr_code = pi_alloc(isr_len);
	pi_copy8(isr_code, isr_start, isr_len);
	*(void **)(isr_code + isr_jsr + 2) = isr_address;

	((long*)cpu_get_physical_vbr())[vector] = (long)isr_code;
}

#endif


/*---------------------------------------------------------------------------------------------*/
/* Map the user boot vectors */

#pragma asm

	ORG	$100					; Application
	move.l	$0,A7
	move.l	$4,A0
    jmp		(-$400000,A0)

	ORG	$110					; RAM to ROM copy
	move.l	#_hw_ram2rom,$C
	jmp		$100

	ORG	$120					; Monitor
	move.l	$3000,A7
	move.l	$3004,A0
    jmp		(A0)

#pragma endasm


/*---------------------------------------------------------------------------------------------*/

