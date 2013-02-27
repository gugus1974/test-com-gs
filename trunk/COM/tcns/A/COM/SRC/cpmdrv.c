/*=============================================================================================*/
/* Drivers for the 68360 CPM                                    					           */
/* Implementation file (cpmdrv.c)													           */
/*																					           */
/* Version 1.1																		           */
/*																					           */
/* (c)1997 Ansaldo Trasporti														           */
/* Written by Giuseppe Carnevale													           */
/*                                                                                             */ 
/* Versione 1.1 del 31 luglio 1999                                                             */
/* modifica in hdlc_conf : evita ritrasmissione dopo CTS lost                                  */
/*
/*   07/09/2001     Napoli     Schiavo
/*    - si agg. commento per il calcolo del baud rate della SPI (non si compila)
/*=============================================================================================*/


/*=============================================================================================*/
/* Includes */

#include <stdio.h>
#include <string.h>

#include "atr_hw.h"
#include "hw_com.h"
#include "cpmdrv.h"


/*=============================================================================================*/
/* Module globals */

static unsigned short	 tx_bd_flags[16];	/* value for the tx bds (wrap bit excluded) */
static unsigned short	 rx_bd_flags[16];	/* value for the rx bds (wrap bit excluded) */

static unsigned short	 tx_bd_num[16];		/* number of transmit bds */
static unsigned short	 rx_bd_num[16];		/* number of receive bds */

static unsigned short	 tx_bd_used[16];	/* number of used transmit bds */
static unsigned short	 rx_bd_used[16];	/* number of used receive bds */

static volatile QUICC_BD *tx_bd_head[16];	/* first transmit buffer descriptor for each ch. */
static volatile QUICC_BD *rx_bd_head[16];	/* first receive buffer descriptor for each ch.  */

static volatile QUICC_BD *tx_bd_hw[16];	/* next transmit bd the hardware will use for each ch. */
static volatile QUICC_BD *rx_bd_hw[16];	/* next receive bd the hardware will use for each ch.  */

static volatile QUICC_BD *tx_bd_sw[16];	/* next transmit bd the software will use for each ch. */
static volatile QUICC_BD *rx_bd_sw[16];	/* next receive bd the software will use for each ch.  */

static unsigned short ev_mask[16];			/* events to be reported                */
static Cpm_event_fptr ev_func[16];			/* callback functions for the events    */
static Cpm_txrx_fptr  tx_func[16];			/* callback functions for the 'send'    */
static Cpm_txrx_fptr  rx_func[16];			/* callback functions for the 'receive' */
static Cpm_timer_fptr timer_func[4];		/* timer callback functions             */
static Cpm_timer_fptr risctimer_func[16];	/* RISC timer callback functions        */
static Cpm_portc_fptr portc_func[12];		/* port C callback functions            */

static short first_int_vec;				/* first interrupt vector for the CPM */

static short sr_cpm_int_disabled;		/* SR register to use to disable CPM interrupts */
static short sr_cpm_int_enabled;		/* SR register to use to enable CPM interrupts  */

static unsigned long intr_nest;			/* mask of ISRs that allow the interrupt nesting */


/*=============================================================================================*/
/* Macros */

#define cpm_fail() if (1) asm(void, "	illegal"); else

#define enable_cpm_ints() asm(void, "	move.w	`sr_cpm_int_enabled`,SR")

#define enter_cpm_int(x) if (x & intr_nest) enable_cpm_ints(); else
#define exit_cpm_int(x) if (1) {enable_cpm_ints(); quicc->intr_cisr = x;} else

/* call the next call-back function associated to a channel in a specific direction */
#define cb(dir, sxc) {																		\
	volatile QUICC_BD *bd = dir##_bd_hw[sxc]; 												\
	while (dir##_bd_used[sxc] && !(bd->status & CPM_BD_R)) {								\
		if (dir##_func[sxc]) (*dir##_func[sxc])(sxc, bd->buf, bd->length, bd->status);		\
		dir##_bd_used[sxc]--;																\
		bd = dir##_bd_hw[sxc] = ((bd->status & CPM_BD_W) ? dir##_bd_head[sxc] : bd+1);		\
	}																						\
}


/*=============================================================================================*/
/* Interrupt handlers */

/*---------------------------------------------------------------------------------------------*/
/* CPM error handler - just return */

static void cpm_error_handler(void) {}


/*---------------------------------------------------------------------------------------------*/
/* CPM SDMA channel bus error handler - we are in trouble */

static void cpm_sdma_bus_error_handler(void)
{
	asm(void, "	illegal");
}


/*---------------------------------------------------------------------------------------------*/
/* SMC UART interrupt handlers */

/* generic SMC UART handler */
#define cpm_smc_uart_handler(n) {					\
	volatile struct smc_regs *r;					\
	unsigned short           ev;					\
	enter_cpm_int(INTR_SMC##n);						\
	r = &quicc->smc_regs[cpm_ch_smc(CPM_SMC##n)];	\
	if (r->smc_smce & CPM_E_TX) {					\
		r->smc_smce = CPM_E_TX;						\
		cb(tx, CPM_SMC##n);							\
	}												\
	if (r->smc_smce & CPM_E_RX) {					\
		r->smc_smce = CPM_E_RX;						\
		cb(rx, CPM_SMC##n);							\
	}												\
	if (ev = (r->smc_smce & ev_mask[CPM_SMC##n])) {	\
		r->smc_smce = ev;							\
		(*ev_func[CPM_SMC##n])(ev);					\
	}												\
	exit_cpm_int(INTR_SMC##n);						\
}

static void cpm_smc1_uart_handler(void) { cpm_smc_uart_handler(1); }
static void cpm_smc2_uart_handler(void) { cpm_smc_uart_handler(2); }


/*---------------------------------------------------------------------------------------------*/
/* SCC UART interrupt handlers */

/* generic SCC UART handler */
#define cpm_scc_uart_handler(n) {					\
	volatile struct scc_regs *r;					\
	unsigned short           ev;					\
	enter_cpm_int(INTR_SCC##n);						\
	r = &quicc->scc_regs[cpm_ch_scc(CPM_SCC##n)];	\
	if (r->scc_scce & CPM_E_TX) {					\
		r->scc_scce = CPM_E_TX;						\
		cb(tx, CPM_SCC##n);							\
	}												\
	if (r->scc_scce & CPM_E_RX) {					\
		r->scc_scce = CPM_E_RX;						\
		cb(rx, CPM_SCC##n);							\
	}												\
	if (ev = (r->scc_scce & ev_mask[CPM_SCC##n])) {	\
		r->scc_scce = ev;							\
		(*ev_func[CPM_SCC##n])(ev);					\
	}												\
	exit_cpm_int(INTR_SCC##n);						\
}

static void cpm_scc1_uart_handler(void) { cpm_scc_uart_handler(1); }
static void cpm_scc2_uart_handler(void) { cpm_scc_uart_handler(2); }
static void cpm_scc3_uart_handler(void) { cpm_scc_uart_handler(3); }
static void cpm_scc4_uart_handler(void) { cpm_scc_uart_handler(4); }


/*---------------------------------------------------------------------------------------------*/
/* SCC HDLC interrupt handlers */

/* generic SCC HDLC handler */
#define cpm_scc_hdlc_handler(n) {					\
	volatile struct scc_regs *r;					\
	unsigned short           ev;					\
	enter_cpm_int(INTR_SCC##n);						\
	r = &quicc->scc_regs[cpm_ch_scc(CPM_SCC##n)];	\
	if (r->scc_scce & (CPM_E_TXB+CPM_E_TXE)) {		\
		r->scc_scce = CPM_E_TXB+CPM_E_TXE;			\
		cb(tx, CPM_SCC##n);							\
	}												\
	if (r->scc_scce & (CPM_E_RXF+CPM_E_RXB)) {		\
		r->scc_scce = CPM_E_RXF+CPM_E_RXB;			\
		cb(rx, CPM_SCC##n);							\
	}												\
	if (ev = (r->scc_scce & ev_mask[CPM_SCC##n])) {	\
		r->scc_scce = ev;							\
		(*ev_func[CPM_SCC##n])(ev);					\
	}												\
	exit_cpm_int(INTR_SCC##n);						\
}

static void cpm_scc1_hdlc_handler(void) { cpm_scc_hdlc_handler(1); }
static void cpm_scc2_hdlc_handler(void) { cpm_scc_hdlc_handler(2); }
static void cpm_scc3_hdlc_handler(void) { cpm_scc_hdlc_handler(3); }
static void cpm_scc4_hdlc_handler(void) { cpm_scc_hdlc_handler(4); }


/*---------------------------------------------------------------------------------------------*/
/* SPI interrupt handler */

static void cpm_spi_handler(void)
{
	enter_cpm_int(INTR_SCP);
	if (quicc->spi_spie & CPM_E_RXB) {
		quicc->spi_spie = CPM_E_RXB;
		cb(tx, CPM_SPI);
		cb(rx, CPM_SPI);
	}
	exit_cpm_int(INTR_SCP);
}


/*---------------------------------------------------------------------------------------------*/
/* General purpose timers interrupt handlers */

/* generic timer handler */
#define cpm_timer_handler(n) {				\
	enter_cpm_int(INTR_TIMER##n);			\
	quicc->timer_ter##n = 0xFFFF;			\
	(*timer_func[CPM_TIMER##n])();			\
	exit_cpm_int(INTR_TIMER##n);			\
}

static void cpm_timer1_handler(void) { cpm_timer_handler(1); }
static void cpm_timer2_handler(void) { cpm_timer_handler(2); }
static void cpm_timer3_handler(void) { cpm_timer_handler(3); }
static void cpm_timer4_handler(void) { cpm_timer_handler(4); }


/*---------------------------------------------------------------------------------------------*/
/* RISC timers interrupt handler */

static void cpm_risctimer_handler(void)
{
	volatile QUICC	*q = quicc;
	unsigned short	rter, i;

	enter_cpm_int(INTR_CP_TIMER);
	while (rter = q->cp_rter) {
		for (i = 0; i < 16; i++) if (rter & (1<<i)) {
			q->cp_rter |= 1<<i;
			if (risctimer_func[i]) (*risctimer_func[i])();
		}
	}
	exit_cpm_int(INTR_CP_TIMER);
}


/*---------------------------------------------------------------------------------------------*/
/* Port C interrupt handlers */

/* generic port C pin handler */
#define cpm_portc_handler(n) {				\
	enter_cpm_int(INTR_PIO_PC##n);			\
	(*portc_func[n])();						\
	exit_cpm_int(INTR_PIO_PC##n);			\
}

static void cpm_portc0_handler(void)  { cpm_portc_handler(0);  }
static void cpm_portc1_handler(void)  { cpm_portc_handler(1);  }
static void cpm_portc2_handler(void)  { cpm_portc_handler(2);  }
static void cpm_portc3_handler(void)  { cpm_portc_handler(3);  }
static void cpm_portc4_handler(void)  { cpm_portc_handler(4);  }
static void cpm_portc5_handler(void)  { cpm_portc_handler(5);  }
static void cpm_portc6_handler(void)  { cpm_portc_handler(6);  }
static void cpm_portc7_handler(void)  { cpm_portc_handler(7);  }
static void cpm_portc8_handler(void)  { cpm_portc_handler(8);  }
static void cpm_portc9_handler(void)  { cpm_portc_handler(9);  }
static void cpm_portc10_handler(void) { cpm_portc_handler(10); }
static void cpm_portc11_handler(void) { cpm_portc_handler(11); }


/*=============================================================================================*/
/* Internal routines */

/*---------------------------------------------------------------------------------------------*/
/* Send a command to the CPM  */

static void cpm_command(int ch, int cmd)
{
	Cpu_Sr			sr;
	volatile QUICC	*q = quicc;
	unsigned short	cr = (ch << 4) | (cmd << 8) | 1;

	/* wait for the previous command completion and send a new one */
	int_disable(sr);
	while (q->cp_cr & 1);
	q->cp_cr = cr;
	int_enable(sr);
}


/*---------------------------------------------------------------------------------------------*/
/* Allocate and initialize the specified number of Buffer Descriptors with the flags */

static QUICC_BD *allocate_bds(int n)
{
	int		 i;
	QUICC_BD *bd = 0;
	
	if (n) {

		/* allocate the memory for the BDs */
		bd = (QUICC_BD*)alloc_quicc_dp_ram(n * sizeof(QUICC_BD));
		if (!bd) cpm_fail();

		/* initialize the BDs fields (including the Wrap bit) */
		for (i = 0; i < n; i++) {
			bd[i].status = 0;
			bd[i].length = 0;
			bd[i].buf    = 0;
		}
		bd[n-1].status = CPM_BD_W;
	}

	return bd;
}


static void setup_bds(int sxc, int tx_n, unsigned short tx_flags,
							   int rx_n, unsigned short rx_flags)
{
	tx_bd_flags[sxc] = tx_flags | CPM_BD_R | CPM_BD_I;
	rx_bd_flags[sxc] = rx_flags | CPM_BD_E | CPM_BD_I;

	tx_bd_num[sxc] = tx_n;
	rx_bd_num[sxc] = rx_n;

	tx_bd_used[sxc] = 0;
	rx_bd_used[sxc] = 0;

	tx_bd_head[sxc] = tx_bd_hw[sxc] = tx_bd_sw[sxc] = allocate_bds(tx_n);
	rx_bd_head[sxc] = rx_bd_hw[sxc] = rx_bd_sw[sxc] = allocate_bds(rx_n);
}


/*---------------------------------------------------------------------------------------------*/
/* Reset BDs */

static void reset_bds(volatile QUICC_BD *bd_head)
{
	for (;;) {
		bd_head->status &= ~CPM_BD_R;
		if (bd_head->status & CPM_BD_W) break;
		bd_head++;
	}
}


/*=============================================================================================*/
/* Generic routines */

/*---------------------------------------------------------------------------------------------*/
/* CPM global initialization                                                                   */
/* cicr          -> value for the CICR register                                                */

void cpm_init(int cicr)
{
	volatile QUICC	*q = quicc;
	int				ilvl;

	/* recover some useful information from the specified cicr */
	first_int_vec = cicr & 0xE0;
	ilvl = (cicr >> 13) & 7;
	sr_cpm_int_disabled = 0x2000 + (ilvl << 8);
	sr_cpm_int_enabled = 0x2000 + ((ilvl-1) << 8);

	/* initialize the CPM Interrupt Controller */
	q->intr_cicr = cicr;
	q->intr_cipr = 0xFFFFFFFF;

	/* install the error handler for the CPM */
	install_isr(first_int_vec + CPIC_VEC_ERROR, cpm_error_handler);

	/* initialize the SDMA */
	q->sdma_sdcr  = 0x0740;
	q->intr_cimr |= INTR_SDMA_BERR;
	install_isr(first_int_vec + CPIC_VEC_SDMA_BERR, cpm_sdma_bus_error_handler);
}


/*---------------------------------------------------------------------------------------------*/
/* Set the interrupt number with the highest priority                                          */
/* int_vec -> vector number with the highest priority                                          */

void cpm_cpic_hp(int int_vec)
{
	quicc->intr_cicr = (quicc->intr_cicr & ~(0x1F << 8)) | (int_vec << 8);
}


/*=============================================================================================*/
/* Baud Rate Generator */

/*---------------------------------------------------------------------------------------------*/
/* Configure the specified BRG (use BRGCLK, no autobaud, no prescaler)                         */
/* brg          -> select the BRG from CPM_BRG1, CPM_BRG2, CPM_BRG3 or CPM_BRG4                */
/* baud_rate    -> desired baud rate [bps]                                                     */
/* oversampling -> desired oversampling multiplication factor                                  */

void cpm_brg_conf(int brg, unsigned long baud_rate, int oversampling)
{
	quicc->brgc[brg].l = (1 << 16) |											// EN
						 ((atr_cpu_clock / (baud_rate * oversampling) - 1) << 1);
}


/*=============================================================================================*/
/* Serial Interface */

/*---------------------------------------------------------------------------------------------*/
/* Configure the specified SCC to use the specified BRG or CLK pin                             */
/* scc        -> selected SCC from CPM_SCC1, CPM_SCC2, CPM_SCC3 or CPM_SCC4                    */
/* tx_brg_clk -> BRG or CLK selection for transmission                                         */
/* rx_brg_clk -> BRG or CLK selection for reception                                            */

void cpm_si_scc_link(int scc, int tx_brg_clk, int rx_brg_clk)
{
	volatile QUICC				  *q = quicc;

	switch (scc) {
		case CPM_SCC1:
			q->si_sicr |= (tx_brg_clk << 0) | (rx_brg_clk << 3);
			break;
		case CPM_SCC2:
			q->si_sicr |= (tx_brg_clk << 8) | (rx_brg_clk << 11);
			break;
		case CPM_SCC3:
			q->si_sicr |= (tx_brg_clk << 16) | (rx_brg_clk << 19);
			break;
		case CPM_SCC4:
			q->si_sicr |= (tx_brg_clk << 24) | (rx_brg_clk << 27);
			break;
	}
}


/*---------------------------------------------------------------------------------------------*/
/* Configure the specified SMC to use the specified BRG or CLK pin                             */
/* smc     -> selected SMC from CPM_SMC1 or CPM_SMC2                                           */
/* brg_clk -> BRG or CLK selection                                                             */

void cpm_si_smc_link(int smc, int brg_clk)
{
	switch (smc) {
		case CPM_SMC1:
			quicc->si_simode |= brg_clk << 12;
			break;
		case CPM_SMC2:
			quicc->si_simode |= brg_clk << 28;
			break;
	}
}


/*=============================================================================================*/
/* SxC configuration routines */

/*---------------------------------------------------------------------------------------------*/
/* Configure the selected SMC for UART operation                                               */
/* smc           -> selected SMC to be used for UART                                           */
/* tx_num        -> maximum number of transmit BDs active (minimum 1)                          */
/* rx_num        -> maximum number of receive BDs active (minimum 2)                           */
/* allow_nesting -> if TRUE the associated ISR allows the nesting of other interrupts          */

void cpm_smc_uart_conf(int smc, int tx_num, int rx_num, char allow_nesting)
{
	volatile QUICC				  *q = quicc;
	int							  ch = cpm_ch_smc(smc);
	volatile struct smc_uart_pram *p = &q->pram[2+ch].scc.pothers.idma_smc.psmc.u;
	volatile struct smc_regs	  *r = &q->smc_regs[ch];

	/* allocate the buffer descriptors */
	setup_bds(smc, tx_num + 1, CPM_BD_I, rx_num + 1, CPM_BD_I);

	/* initialize the parameters */
	p->rbase = (char*)rx_bd_head[smc] - (char*)q;
	p->tbase = (char*)tx_bd_head[smc] - (char*)q;
	cpm_command(smc, CPM_CMD_INIT_RXTX_PARAMS);
	p->rfcr = p->tfcr = 0x18;
	p->mrblr = p->max_idl = p->brkec = 0;
	p->brkcr = 1;

	/* clear any previous event and enable the supported interrupts */
	r->smc_smce = 0xFF;
	r->smc_smcm = CPM_E_RX + CPM_E_TX;

	/* install the isr and enable the interrupts */
	if (smc == CPM_SMC1) {
		if (allow_nesting) intr_nest |= INTR_SMC1;
		install_isr(first_int_vec + CPIC_VEC_SMC1, cpm_smc1_uart_handler);
		q->intr_cimr |= INTR_SMC1;
	}
	else {
		if (allow_nesting) intr_nest |= INTR_SMC1;
		install_isr(first_int_vec + CPIC_VEC_SMC2_PIP, cpm_smc2_uart_handler);
		q->intr_cimr |= INTR_SMC2;
	}
	
	/* configure for 8 bit, no parity, 1 stop bit and enable the tx and rx */
	r->smc_smcmr = 0x4820;
}


/*---------------------------------------------------------------------------------------------*/
/* Configure the selected SCC for UART operation                                               */
/* scc           -> selected SMC to be used for UART                                           */
/* tx_num        -> maximum number of transmit BDs active (minimum 1)                          */
/* rx_num        -> maximum number of receive BDs active (minimum 2)                           */
/* allow_nesting -> if TRUE the associated ISR allows the nesting of other interrupts          */

void cpm_scc_uart_conf(int scc, int tx_num, int rx_num, char allow_nesting)
{
	volatile QUICC			  *q = quicc;
	int						  ch = cpm_ch_scc(scc);
	volatile struct uart_pram *p = &q->pram[ch].scc.pscc.u;
	volatile struct scc_regs  *r = &q->scc_regs[ch];

	/* allocate the buffer descriptors */
	setup_bds(scc, tx_num + 1, CPM_BD_I, rx_num + 1, CPM_BD_I);

	/* initialize the parameters */
	p->rbase = (char*)rx_bd_head[scc] - (char*)q;
	p->tbase = (char*)tx_bd_head[scc] - (char*)q;
	cpm_command(scc, CPM_CMD_INIT_RXTX_PARAMS);
	p->rfcr = p->tfcr = 0x18;
	p->mrblr = p->max_idl = 0;
	p->brkcr = 1;
	p->parec = p->frmer = p->nosec = p->brkec = p->uaddr1 = p->uaddr2 = p->toseq = 0;
	p->cc[0] = p->cc[1] = p->cc[2] = p->cc[3] = p->cc[4] = p->cc[5] = p->cc[6] = p->cc[7] = 0x8000;
	p->rccm = 0xC0FF;

	/* clear any previous event and enable the supported interrupts */
	r->scc_scce = 0xFFFF;
	r->scc_sccm = CPM_E_RX + CPM_E_TX;

	/* install the isr and enable the interrupts */
	switch (scc) {
		case CPM_SCC1:
			if (allow_nesting) intr_nest |= INTR_SCC1;
			install_isr(first_int_vec + CPIC_VEC_SCC1, cpm_scc1_uart_handler);
			q->intr_cimr |= INTR_SCC1;
			break;
		case CPM_SCC2:
			if (allow_nesting) intr_nest |= INTR_SCC2;
			install_isr(first_int_vec + CPIC_VEC_SCC2, cpm_scc2_uart_handler);
			q->intr_cimr |= INTR_SCC2;
			break;
		case CPM_SCC3:
			if (allow_nesting) intr_nest |= INTR_SCC3;
			install_isr(first_int_vec + CPIC_VEC_SCC3, cpm_scc3_uart_handler);
			q->intr_cimr |= INTR_SCC3;
			break;
		case CPM_SCC4:
			if (allow_nesting) intr_nest |= INTR_SCC4;
			install_isr(first_int_vec + CPIC_VEC_SCC4, cpm_scc4_uart_handler);
			q->intr_cimr |= INTR_SCC4;
			break;
	}

	/* small rx FIFO len, 16x sampling for tx and rx, UART mode */
	r->scc_gsmr.w.high = 0x00000020;
	r->scc_gsmr.w.low  = 0x00028004;

	/* configure the UART: asynchronous flow control, 8N1 */
	r->scc_psmr = 0xB000;
}


/*---------------------------------------------------------------------------------------------*/
/* Configure the selected SCC for HDLC operation                                               */
/* scc            -> selected SMC to be used for HDLC                                          */
/* max_frame_size -> maximum frame size                                                        */
/* opt            -> option flags                                                              */
/* tx_num         -> maximum number of transmit BDs active (minimum 1)                         */
/* rx_num         -> maximum number of receive BDs active (minimum 2)                          */
/* allow_nesting  -> if TRUE the associated ISR allows the nesting of other interrupts         */

void cpm_scc_hdlc_conf(int scc, int max_frame_size, short opt,
					   int tx_num, int rx_num, char allow_nesting)
{
	volatile QUICC			  *q = quicc;
	int						  ch = cpm_ch_scc(scc);
	volatile struct hdlc_pram *p = &q->pram[ch].scc.pscc.h;
	volatile struct scc_regs  *r = &q->scc_regs[ch];

	/* allocate the buffer descriptors */
	setup_bds(scc, tx_num + 1, CPM_BD_I+CPM_BD_L+CPM_BD_TC, rx_num + 1, CPM_BD_I);

	/* initialize the parameters */
	p->rbase = (char*)rx_bd_head[scc] - (char*)q;
	p->tbase = (char*)tx_bd_head[scc] - (char*)q;
	cpm_command(scc, CPM_CMD_INIT_RXTX_PARAMS);
	p->rfcr = p->tfcr = 0x18;
	p->mrblr = max_frame_size;
	p->c_mask = 0x0000F0B8;
	p->c_pres = 0x0000FFFF;
	p->disfc = p->crcec = p->abtsc = p->nmarc = p->retrc = 0;
	p->mflr = max_frame_size;
	p->rfthr = 1;
	p->hmask = p->haddr1 = p->haddr2 = p->haddr3 = p->haddr4 = 0;

	/* clear any previous event and enable the supported interrupts */
	r->scc_scce = 0xFFFF;
	r->scc_sccm = CPM_E_RXB | CPM_E_TXB | CPM_E_RXF | CPM_E_TXE;

	/* install the isr and enable the interrupts */
	switch (scc) {
		case CPM_SCC1:
			if (allow_nesting) intr_nest |= INTR_SCC1;
			install_isr(first_int_vec + CPIC_VEC_SCC1, cpm_scc1_hdlc_handler);
			q->intr_cimr |= INTR_SCC1;
			break;
		case CPM_SCC2:
			if (allow_nesting) intr_nest |= INTR_SCC2;
			install_isr(first_int_vec + CPIC_VEC_SCC2, cpm_scc2_hdlc_handler);
			q->intr_cimr |= INTR_SCC2;
			break;
		case CPM_SCC3:
			if (allow_nesting) intr_nest |= INTR_SCC3;
			install_isr(first_int_vec + CPIC_VEC_SCC3, cpm_scc3_hdlc_handler);
			q->intr_cimr |= INTR_SCC3;
			break;
		case CPM_SCC4:
			if (allow_nesting) intr_nest |= INTR_SCC4;
			install_isr(first_int_vec + CPIC_VEC_SCC4, cpm_scc4_hdlc_handler);
			q->intr_cimr |= INTR_SCC4;
			break;
	}

	// modifica in hdlc_conf : evita ritrasmissione dopo CTS lost
	/* configure for HDLC */
    // test set bit 41	
	r->scc_gsmr.w.high = (1<<9); // set CTSP  //0x00000000;
	r->scc_gsmr.w.low  = 0x00000000;
	r->scc_psmr = opt;

	
	
}


/*---------------------------------------------------------------------------------------------*/
/* Configure the selected SCC or SMC communication pins                                        */
/* sxc  -> SCC or SMC to configure                                                             */
/* pins -> pins to use                                                                         */

void cpm_sxc_pin_conf(int sxc, int pins)
{
	volatile QUICC			  *q = quicc;

	switch (sxc) {
		case CPM_SMC1:
			if (pins & CPM_TX_PIN) q->pip_pbpar |= 1 << 6;
			if (pins & CPM_RX_PIN) q->pip_pbpar |= 1 << 7;
			break;
		case CPM_SMC2:
			if (pins & CPM_TX_PIN) q->pip_pbpar |= 1 << 10;
			if (pins & CPM_RX_PIN) q->pip_pbpar |= 1 << 11;
			break;
		case CPM_SCC1:
			if (pins & CPM_TX_PIN)     q->pio_papar |= 1 << 1;
			if (pins & CPM_RX_PIN)     q->pio_papar |= 1 << 0;
			if (pins & CPM_RTS_B_PIN) {q->pip_pbpar |= 1 << 12; q->pip_pbdir |= 1 << 12;}
			if (pins & CPM_RTS_C_PIN)  q->pio_pcpar |= 1 << 0;
			if (pins & CPM_CTS_PIN)    q->pio_pcso  |= 1 << 4;
			if (pins & CPM_CD_PIN)     q->pio_pcso  |= 1 << 5;
			break;
		case CPM_SCC2:
			if (pins & CPM_TX_PIN)     q->pio_papar |= 1 << 3;
			if (pins & CPM_RX_PIN)     q->pio_papar |= 1 << 2;
			if (pins & CPM_RTS_B_PIN) {q->pip_pbpar |= 1 << 13; q->pip_pbdir |= 1 << 13;}
			if (pins & CPM_RTS_C_PIN)  q->pio_pcpar |= 1 << 1;
			if (pins & CPM_CTS_PIN)    q->pio_pcso  |= 1 << 6;
			if (pins & CPM_CD_PIN)     q->pio_pcso  |= 1 << 7;
			break;
		case CPM_SCC3:
			if (pins & CPM_TX_PIN)     q->pio_papar |= 1 << 5;
			if (pins & CPM_RX_PIN)     q->pio_papar |= 1 << 4;
			if (pins & CPM_RTS_B_PIN) {q->pip_pbpar |= 1 << 14; q->pip_pbdir |= 1 << 14;}
			if (pins & CPM_RTS_C_PIN)  q->pio_pcpar |= 1 << 2;
			if (pins & CPM_CTS_PIN)    q->pio_pcso  |= 1 << 8;
			if (pins & CPM_CD_PIN)     q->pio_pcso  |= 1 << 9;
			break;
		case CPM_SCC4:
			if (pins & CPM_TX_PIN)     q->pio_papar |= 1 << 7;
			if (pins & CPM_RX_PIN)     q->pio_papar |= 1 << 6;
			if (pins & CPM_RTS_B_PIN) {q->pip_pbpar |= 1 << 15; q->pip_pbdir |= 1 << 15;}
			if (pins & CPM_RTS_C_PIN)  q->pio_pcpar |= 1 << 3;
			if (pins & CPM_CTS_PIN)    q->pio_pcso  |= 1 << 10;
			if (pins & CPM_CD_PIN)     q->pio_pcso  |= 1 << 11;
			break;
	}

	switch (sxc) {
		case CPM_SMC1:
		case CPM_SCC1:
		case CPM_SCC2:
			if (pins & CPM_CLK15_PIN) q->pio_papar |= 1 << 8;
			if (pins & CPM_CLK26_PIN) q->pio_papar |= 1 << 9;
			if (pins & CPM_CLK37_PIN) q->pio_papar |= 1 << 10;
			if (pins & CPM_CLK48_PIN) q->pio_papar |= 1 << 11;
			break;
		case CPM_SMC2:
		case CPM_SCC3:
		case CPM_SCC4:
			if (pins & CPM_CLK15_PIN) q->pio_papar |= 1 << 12;
			if (pins & CPM_CLK26_PIN) q->pio_papar |= 1 << 13;
			if (pins & CPM_CLK37_PIN) q->pio_papar |= 1 << 14;
			if (pins & CPM_CLK48_PIN) q->pio_papar |= 1 << 15;
			break;
	}
}


/*---------------------------------------------------------------------------------------------*/
/* Register a function to the specified SCC or SMC events (only one function supported)        */
/* sxc        -> SCC or SMC to configure                                                       */
/* event_mask -> events to look for                                                            */
/* event_func -> function to call                                                              */

void cpm_sxc_event_enable(int sxc, unsigned short event_mask, Cpm_event_fptr event_func)
{
	/* save the function and the corresponding mask */
	ev_mask[sxc] = event_mask;
	ev_func[sxc] = event_func;

	/* clear any previous event and enable the required interrupts */
	switch (sxc) {
		case CPM_SCC1:
		case CPM_SCC2:
		case CPM_SCC3:
		case CPM_SCC4:
			{
				volatile struct scc_regs *r = &quicc->scc_regs[cpm_ch_scc(sxc)];
				r->scc_scce  = event_mask;
				r->scc_sccm |= event_mask;
			}
			break;

		case CPM_SMC1:
		case CPM_SMC2:
			{
				volatile struct smc_regs *r = &quicc->smc_regs[cpm_ch_smc(sxc)];
				r->smc_smce  = event_mask;
				r->smc_smcm |= event_mask;
			}
			break;
	}
}


/*=============================================================================================*/
/* "Send" routines */

/*---------------------------------------------------------------------------------------------*/
/* Set the TX enable/disable status                                                            */
/* sxc                   -> SCC or SMC to use                                                  */
/* cpm_send_confirm_func -> send confirmation function                                         */

void cpm_sxc_send_enable(int sxc, Cpm_txrx_fptr cpm_send_confirm_func)
{
	tx_func[sxc] = cpm_send_confirm_func;

	switch (sxc) {
		case CPM_SCC1: case CPM_SCC2: case CPM_SCC3: case CPM_SCC4:
			quicc->scc_regs[cpm_ch_scc(sxc)].scc_gsmr.w.low |= 0x10;
			break;
		case CPM_SMC1: case CPM_SMC2:
			quicc->smc_regs[cpm_ch_smc(sxc)].smc_smcmr |= 2;
			break;
	}
}

void cpm_sxc_send_disable(int sxc)
{
	/* disable the transmitter */
	switch (sxc) {
		case CPM_SCC1: case CPM_SCC2: case CPM_SCC3: case CPM_SCC4:
			quicc->scc_regs[cpm_ch_scc(sxc)].scc_gsmr.w.low &= ~0x10;
			break;
		case CPM_SMC1: case CPM_SMC2:
			quicc->smc_regs[cpm_ch_smc(sxc)].smc_smcmr &= ~2;
			break;
	}

	/* abort the pending transfers and reset the transmitter */
	reset_bds(tx_bd_head[sxc]);
	tx_bd_used[sxc] = 0;
	cpm_command(sxc, CPM_CMD_INIT_TX_PARAMS);
}


/*---------------------------------------------------------------------------------------------*/
/* Send data using the first free BD                                                           */
/* sxc  -> SCC or SMC to use (could specify SPI but for internal use only)                     */
/* buf  -> buffer to send                                                                      */
/* size -> number of bytes to send                                                             */

char cpm_sxc_send(int sxc, const void *buf, unsigned short size)
{
	volatile QUICC_BD *bd = tx_bd_sw[sxc]; /* buffer descriptor to use */

	/* check if there are free BDs */
	if (tx_bd_used[sxc] == tx_bd_num[sxc]) return 1;
	tx_bd_used[sxc]++;

	/* update the next BD to use */
	if (bd->status & CPM_BD_W) tx_bd_sw[sxc] = tx_bd_head[sxc];
	else tx_bd_sw[sxc] = bd + 1;

	/* program the BD and start the transfer */
	bd->length = size;
	bd->buf    = (void*)buf;
	bd->status = tx_bd_flags[sxc] | (bd->status & CPM_BD_W);
	
	/* on the SCCs set the transmit on demand register to decrease latency */
	if (sxc == CPM_SCC1 || sxc == CPM_SCC2 || sxc == CPM_SCC3 || sxc == CPM_SCC4)
		quicc->scc_regs[cpm_ch_scc(sxc)].scc_todr = 0x8000;

	return 0;
}


/*---------------------------------------------------------------------------------------------*/
/* Return the number of used transmit BDs                                                      */
/* sxc  -> SCC or SMC                                                                          */
/* ()  <-  number of used transmit BDs for the specified SCC or SMC                            */

unsigned short cpm_sxc_send_num(int sxc)
{
	return tx_bd_used[sxc];
}


/*=============================================================================================*/
/* "Receive" routines */

/*---------------------------------------------------------------------------------------------*/
/* Set the RX enable/disable status                                                            */
/* sxc              -> SCC or SMC to use                                                       */
/* cpm_receive_func -> receive function                                                        */

void cpm_sxc_receive_enable(int sxc, Cpm_txrx_fptr cpm_receive_func)
{
	rx_func[sxc] = cpm_receive_func;

	switch (sxc) {
		case CPM_SCC1: case CPM_SCC2: case CPM_SCC3: case CPM_SCC4:
			quicc->scc_regs[cpm_ch_scc(sxc)].scc_gsmr.w.low |= 0x20;
			break;
		case CPM_SMC1: case CPM_SMC2:
			quicc->smc_regs[cpm_ch_smc(sxc)].smc_smcmr |= 1;
			break;
	}
}

void cpm_sxc_receive_disable(int sxc)
{
	/* disable the receiver */
	switch (sxc) {
		case CPM_SCC1: case CPM_SCC2: case CPM_SCC3: case CPM_SCC4:
			quicc->scc_regs[cpm_ch_scc(sxc)].scc_gsmr.w.low &= ~0x20;
			break;
		case CPM_SMC1: case CPM_SMC2:
			quicc->smc_regs[cpm_ch_smc(sxc)].smc_smcmr &= ~1;
			break;
	}

	/* abort the pending transfers and reset the receiver */
	reset_bds(rx_bd_head[sxc]);
	rx_bd_used[sxc] = 0;
	cpm_command(sxc, CPM_CMD_INIT_RX_PARAMS);
}


/*---------------------------------------------------------------------------------------------*/
/* Receive data using the first free BD                                                        */
/* sxc  -> SCC or SMC to use (could specify SPI but for internal use only)                     */
/* buf  -> buffer to use for the data                                                          */

char cpm_sxc_receive(int sxc, void *buf)
{
	volatile QUICC_BD *bd = rx_bd_sw[sxc]; /* buffer descriptor to use */

	/* check if there are free BDs */
	if (rx_bd_used[sxc] == rx_bd_num[sxc]) return 1;
	rx_bd_used[sxc]++;

	/* update the next BD to use */
	if (bd->status & CPM_BD_W) rx_bd_sw[sxc] = rx_bd_head[sxc];
	else rx_bd_sw[sxc] = bd + 1;

	/* program the BD and start the transfer */
	bd->length  = 0;
	bd->buf     = (void*)buf;
	bd->status = tx_bd_flags[sxc] | (bd->status & CPM_BD_W);

	return 0;
}


/*---------------------------------------------------------------------------------------------*/
/* Abort the current receive                                                                   */
/* sxc  -> SCC or SMC                                                                          */

void cpm_sxc_receive_close(int sxc)
{
	/* close the current BD */
	cpm_command(sxc, CPM_CMD_CLOSE_RX_BD);
}


/*---------------------------------------------------------------------------------------------*/
/* Return the number of used receive BDs                                                       */
/* sxc  -> SCC or SMC                                                                          */
/* ()  <-  number of used receive BDs for the specified SCC or SMC                             */

unsigned short cpm_sxc_receive_num(int sxc)
{
	return rx_bd_used[sxc];
}


/*---------------------------------------------------------------------------------------------*/
/* Set the size of the receive BDs (useful only for UART transmissions)                        */
/* sxc  -> SCC or SMC                                                                          */
/* size -> number of bytes to receive                                                          */

unsigned short cpm_sxc_uart_receive_set_size(int sxc, unsigned short size)
{
	unsigned short *mrblr, x;
	Cpu_Sr		   sr;

	/* get the pointer to the MRBLR register */
	switch (sxc) {
		case CPM_SCC1:
		case CPM_SCC2:
		case CPM_SCC3:
		case CPM_SCC4:
			mrblr = &quicc->pram[cpm_ch_scc(sxc)].scc.pscc.u.mrblr;
			break;

		case CPM_SMC1:
		case CPM_SMC2:
			mrblr = &quicc->pram[2+cpm_ch_smc(sxc)].scc.pothers.idma_smc.psmc.u.mrblr;
			break;
	}
	
	/* set the new value and return the old one */
	int_disable(sr);
	x = *mrblr;
	*mrblr = size;
	int_enable(sr);
	return x;
}


/*---------------------------------------------------------------------------------------------*/
/* Configure the addresses to be recognized in HDLC reception                                  */
/* scc              -> SCC to configure                                                        */
/* hmask            -> address mask                                                            */
/* haddr1 .. haddr4 -> addresses                                                               */

void cpm_scc_hdlc_receive_set_address(int scc, unsigned short hmask,
									  unsigned short haddr1, unsigned short haddr2,
									  unsigned short haddr3, unsigned short haddr4)
{
	volatile struct hdlc_pram *p = &quicc->pram[cpm_ch_scc(scc)].scc.pscc.h;

	p->hmask  = hmask;
	p->haddr1 = haddr1;
	p->haddr2 = haddr2;
	p->haddr3 = haddr3;
	p->haddr4 = haddr4;
}


/*=============================================================================================*/
/* SPI routines */

/*---------------------------------------------------------------------------------------------*/
/* Initialize the SPI interface                                                                */
/* baud_rate        -> desired baud rate [bps]                                                 */
/* opt              -> option flags                                                            */
/* cpm_receive_func -> receive function                                                        */
/* allow_nesting    -> if TRUE the associated ISR allows the nesting of other interrupts       */

void cpm_spi_conf(unsigned long baud_rate, unsigned short opt, Cpm_txrx_fptr cpm_receive_func,
				  char allow_nesting)
{
	volatile QUICC				*q = quicc;
	volatile struct spi_pram	*p = &q->pram[1].scc.pothers.timer_spi.spi;

	/* we use the SPICLK, SPIMOSI and SPIMISO pins */
	q->pip_pbpar |= 0xE;
	q->pip_pbdir |= 0xE;

	/* allocate the buffer descriptors */
	setup_bds(CPM_SPI, 1, CPM_BD_I+CPM_BD_L, 1, CPM_BD_I);

	/* initialize the parameters */
	p->rbase = (char*)rx_bd_head[CPM_SPI] - (char*)q;
	p->tbase = (char*)tx_bd_head[CPM_SPI] - (char*)q;
	cpm_command(CPM_SPI, CPM_CMD_INIT_RXTX_PARAMS);
	p->rfcr = p->tfcr = 0x18;
	p->mrblr = 0xFFFE;

	/* clear any previous event and enable the supported interrupts */
	q->spi_spie = 0xFF;
	q->spi_spim = CPM_E_RXB;

	/* install the isr and enable the interrupts */
	if (allow_nesting) intr_nest |= INTR_SCP;
	install_isr(first_int_vec + CPIC_VEC_SPI, cpm_spi_handler);
	q->intr_cimr |= INTR_SCP;

	/* install the user callback function */
	rx_func[CPM_SPI] = cpm_receive_func;

	/* configure the SPI and enable it */
    /* la corretta formula per il baud rate Š 0xF&(atr_cpu_clock/(4*baud_rate)-1)*/
	q->spi_spmode = 0x0300 | opt | (0xF & (((atr_cpu_clock+(4*baud_rate-1)) / (4*baud_rate)) - 1));
}


/*---------------------------------------------------------------------------------------------*/
/* Send and receive data                                                                       */
/* send_buf     -> buffer of data to send                                                      */
/* receive_buf  -> buffer where to receive the data                                            */
/* size         -> number of bytes to transmit/receive                                         */

void cpm_spi_send_receive(const void *send_buf, void *receive_buf, unsigned short size)
{
	/* link the recive and transmit buffers */
	cpm_sxc_receive(CPM_SPI, receive_buf);
	cpm_sxc_send(CPM_SPI, send_buf, size);

	/* start the transfer */
	quicc->spi_spcom = 0x80;
}


/*=============================================================================================*/
/* General purpose timers */

static unsigned short tmr;	/* cached TRM register value */


/*---------------------------------------------------------------------------------------------*/
/* Initialize the selected timer                                                               */
/* timer         -> timer number                                                               */
/* allow_nesting -> if TRUE the associated ISR allows the nesting of other interrupts          */

#define cpm_timer_install(n) {															\
	tmr = ((atr_cpu_clock / 1000000) << 8) | (1<<4) | (1<<3) | (1<<1);					\
	timer_func[CPM_TIMER##n] = f;														\
	if (allow_nesting) intr_nest |= INTR_TIMER##n;										\
	install_isr(first_int_vec + CPIC_VEC_TIMER##n, cpm_timer##n##_handler);				\
	quicc->intr_cimr |= INTR_TIMER##n;													\
}

void cpm_timer_init(int timer, Cpm_timer_fptr f, char allow_nesting)
{
	switch (timer) {
		case CPM_TIMER1: cpm_timer_install(1); break;
		case CPM_TIMER2: cpm_timer_install(2); break;
		case CPM_TIMER3: cpm_timer_install(3); break;
		case CPM_TIMER4: cpm_timer_install(4); break;
	}
}


/*---------------------------------------------------------------------------------------------*/
/* Re/program or disable the selected timer to generate a periodic interrupt                   */
/* timer   -> timer number                                                                     */
/* time_us -> timer run time in [us] (0 to disable the timer)                                  */
/* f       -> function to call each timer period                                               */

#define cpm_timer_enable(n) {														\
	int_disable(sr);																\
																					\
	/* disable the timer and clear a previous event */								\
	q->timer_tgcr &= ~(1 << ((n-1)*4));												\
	q->timer_tmr##n = tmr;															\
	q->timer_tcn##n = 0;															\
	q->timer_trr##n = time_us;														\
	q->timer_ter##n = 0xFFFF;														\
	q->intr_cisr = INTR_TIMER##n;													\
																					\
	/* enable the timer if a nonzero time was specified */							\
	if (time_us) q->timer_tgcr |= 1 << ((n-1)*4);									\
																					\
	int_enable(sr);																	\
}

void cpm_timer_set_periodic(int timer, unsigned short time_us)
{
	Cpu_Sr		   sr;
	volatile QUICC *q = quicc;

	switch (timer) {
		case CPM_TIMER1: cpm_timer_enable(1); break;
		case CPM_TIMER2: cpm_timer_enable(2); break;
		case CPM_TIMER3: cpm_timer_enable(3); break;
		case CPM_TIMER4: cpm_timer_enable(4); break;
	}
}


/*=============================================================================================*/
/* RISC timers */

/*---------------------------------------------------------------------------------------------*/
/* RISC timers intialization                                                                   */
/* risc_timer_us -> RISC timer tick time [us] (better if a power of 2)                         */
/* allow_nesting -> if TRUE the associated ISR allows the nesting of other interrupts          */

void cpm_risctimer_init(unsigned long risc_timer_us, char allow_nesting)
{
	volatile QUICC				*q = quicc;
	volatile struct timer_pram	*p = &q->pram[1].scc.pothers.timer_spi.timer;

	/* initialize the RISC timer tick period */
	q->cp_rccr = ((((risc_timer_us * (atr_cpu_clock / 1000)) / 1024000) - 1) & 0x3F) << 8;

	/* initialize the timer parameters */
	p->tm_base = (char*)alloc_quicc_dp_ram(64) - (char*)q;

	/* install the RISC timer interrupt handler */
	if (allow_nesting) intr_nest |= INTR_CP_TIMER;
	install_isr(first_int_vec + CPIC_VEC_RISC_TIMER, cpm_risctimer_handler);
	q->intr_cimr |= INTR_CP_TIMER;

	/* start the timer processing */
	q->cp_rccr |= 0x8000;
}


/*---------------------------------------------------------------------------------------------*/
/* RISC timer subscription                                                                     */
/* timer  -> RISC timer number (0..15)                                                         */
/* f      -> function to call each timer period                                                */

void cpm_risctimer_subscribe(int timer, Cpm_timer_fptr f)
{
	risctimer_func[timer] = f;
}


/*---------------------------------------------------------------------------------------------*/
/* RISC timer start                                                                            */
/* timer   -> RISC timer number (0..15)                                                        */
/* ticks   -> number of RISC ticks before timer elapsing                                       */
/* restart -> if TRUE the timer is automatically restarted                                     */

void cpm_risctimer_start(int timer, unsigned short ticks, char restart)
{
	volatile QUICC				*q = quicc;
	volatile struct timer_pram	*p = &q->pram[1].scc.pothers.timer_spi.timer;
	Cpu_Sr						sr;
	unsigned long				cmd;

	/* enable the specified timer */
	q->cp_rtmr |= 1 << timer;
	cmd = ticks | (timer << 16) | (restart << 30) | (1 << 31);
	int_disable(sr);
	p->tm_cmd = cmd;
	while (q->cp_cr & 1);
	q->cp_cr = (5 << 4) | (CPM_CMD_SET_TIMER << 8) | 1;
	int_enable(sr);
}


/*---------------------------------------------------------------------------------------------*/
/* RISC timer stop                                                                             */
/* timer   -> RISC timer number (0..15)                                                        */

void cpm_risctimer_stop(int timer)
{
	volatile QUICC				*q = quicc;
	volatile struct timer_pram	*p = &q->pram[1].scc.pothers.timer_spi.timer;
	Cpu_Sr						sr;
	unsigned short				timer_bit = 1 << timer;

	/* stop the specified timer and disable it */
	int_disable(sr);
	p->tm_cmd = 0;
	while (q->cp_cr & 1);
	q->cp_cr = (5 << 4) | (SET_TIMER << 8) | 1;
	q->cp_rter |= timer_bit;
	q->cp_rtmr &= ~timer_bit;
	int_enable(sr);
}


/*---------------------------------------------------------------------------------------------*/
/* RISC timer ticks counter value                                                              */

unsigned long cpm_risctimer_counter(void)
{
	return quicc->pram[1].scc.pothers.timer_spi.timer.tm_cnt;
}


/*=============================================================================================*/
/* Port C interrupt */

static const unsigned char portc_vec[] = {CPIC_VEC_PC0, CPIC_VEC_PC1,  CPIC_VEC_PC2,
										  CPIC_VEC_PC3, CPIC_VEC_PC4,  CPIC_VEC_PC5,
										  CPIC_VEC_PC6, CPIC_VEC_PC7,  CPIC_VEC_PC8,
										  CPIC_VEC_PC9, CPIC_VEC_PC10, CPIC_VEC_PC11};
static const void *portc_handler[] = {cpm_portc0_handler, cpm_portc1_handler,  cpm_portc2_handler,
									  cpm_portc3_handler, cpm_portc4_handler,  cpm_portc5_handler,
									  cpm_portc6_handler, cpm_portc7_handler,  cpm_portc8_handler,
									  cpm_portc9_handler, cpm_portc10_handler, cpm_portc11_handler};
static const unsigned long portc_intr[] = {INTR_PIO_PC0, INTR_PIO_PC1, INTR_PIO_PC2,
									 	  INTR_PIO_PC3, INTR_PIO_PC4, INTR_PIO_PC5,
									 	  INTR_PIO_PC6, INTR_PIO_PC7, INTR_PIO_PC8,
									 	  INTR_PIO_PC9, INTR_PIO_PC10, INTR_PIO_PC11};


/*---------------------------------------------------------------------------------------------*/
/* Connect an interrupt handler to the specified port C pin                                    */
/* pin           -> port C pin mask                                                            */
/* int_mode      -> interrupt mode                                                             */
/* f             -> function to call on each interrupt occurrence                              */
/* allow_nesting -> if TRUE the associated ISR allows the nesting of other interrupts          */

void cpm_portc_connect(int mask, int int_mode, Cpm_portc_fptr f, char allow_nesting)
{
	int pin;

	for (pin = 0; pin < 16; pin++) if (mask & (1<<pin)) {
		/* set the pin interrupt mode, save the associated function and enable the interrupts */
		quicc->pio_pcint |= int_mode << pin;
		portc_func[pin] = f;
		if (allow_nesting) intr_nest |= portc_intr[pin];
		install_isr(first_int_vec + portc_vec[pin], portc_handler[pin]);
	}
}


/*---------------------------------------------------------------------------------------------*/
/* Enable the interrupt handlers for the specified port C pins                                 */
/* pin           -> port C pin mask                                                            */

void cpm_portc_enable(int mask)
{
	int 			pin;
	unsigned long	cimr_mask = 0;
	Cpu_Sr			sr;

	/* build the corresponding CIMR register mask */
	for (pin = 0; pin < 16; pin++) if (mask & (1<<pin)) cimr_mask |= portc_intr[pin];

	/* update the CIMR register */
	int_disable(sr);
	quicc->intr_cimr |= cimr_mask;
	int_enable(sr);
}


/*---------------------------------------------------------------------------------------------*/
/* Disable the interrupt handlers for the specified port C pins                                */
/* pin           -> port C pin mask                                                            */

void cpm_portc_disable(int mask)
{
	int 			pin;
	unsigned long	cimr_mask = 0;
	Cpu_Sr			sr;

	/* build the corresponding CIMR register mask */
	for (pin = 0; pin < 16; pin++) if (mask & (1<<pin)) cimr_mask |= portc_intr[pin];

	/* update the CIMR register */
	int_disable(sr);
	quicc->intr_cimr &= ~cimr_mask;
	int_enable(sr);
}


/*=============================================================================================*/

