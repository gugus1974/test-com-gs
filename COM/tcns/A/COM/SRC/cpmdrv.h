/*=============================================================================================*/
/* Drivers for the 68360 CPM                                    					           */
/* Header file (cpmdrv.h)													                   */
/*																					           */
/* Version 1.0																		           */
/*																					           */
/* (c)1997 Ansaldo Trasporti														           */
/* Written by Giuseppe Carnevale													           */
/*=============================================================================================*/

#ifndef _CPMDRV_
#define _CPMDRV_


/*=============================================================================================*/
/* Includes */

#include "hw_360.h"


/*=============================================================================================*/
/* Definitions */

/* channels */
#define CPM_SCC1	0	/* UART & HDLC */
#define CPM_SCC2	4	/* UART & HDLC */
#define CPM_SCC3	8	/* UART & HDLC */
#define CPM_SCC4	12	/* UART & HDLC */
#define CPM_SMC1	9	/* UART only   */
#define CPM_SMC2	13	/* UART only   */
#define CPM_SPI     5	/* master only */

/* buadrate generators */
#define CPM_BRG1	0
#define CPM_BRG2 	1
#define CPM_BRG3	2
#define CPM_BRG4	3

/* external clock pins */
#define CPM_CLK15	4	/* CLK1 or CLK5 */
#define CPM_CLK26	5	/* CLK2 or CLK6 */
#define CPM_CLK37	6	/* CLK3 or CLK7 */
#define CPM_CLK48	7	/* CLK4 or CLK8 */

/* general-purpose timers */
#define CPM_TIMER1	0
#define CPM_TIMER2	1
#define CPM_TIMER3	2
#define CPM_TIMER4	3

/* port C pins interrupt modes */
#define CPM_PORTC_INT_CHANGE	0
#define CPM_PORTC_INT_HI2LO		1

/* channel pins */
#define CPM_TX_PIN		1
#define CPM_RX_PIN		2
#define CPM_RTS_B_PIN	4
#define CPM_RTS_C_PIN	8
#define CPM_CTS_PIN		16
#define CPM_CD_PIN		32
#define CPM_CLK15_PIN	64
#define CPM_CLK26_PIN	128
#define CPM_CLK37_PIN	256
#define CPM_CLK48_PIN	512

/* HDLC protocol options (they go directly in the PSMR register) */
#define CPM_HDLC_DRT_OPT	(1<<6)	/* disable receiver while transmitting */

/* SPI protocol options (they go directly in the SPMODE register) */
#define CPM_SPI_CI_OPT		(1<<13)	/* invert the clock                                  */
#define CPM_SPI_CP_OPT		(1<<12)	/* clock phase selection                             */
#define CPM_SPI_REV_OPT		(1<<10)	/* if selected MSB is transmitted and received first */
#define CPM_SPI_LEN_4_OPT	(3<<4)	/* 4 bits per character                              */
#define CPM_SPI_LEN_5_OPT	(4<<4)	/* 5 bits per character                              */
#define CPM_SPI_LEN_6_OPT	(5<<4)	/* 6 bits per character                              */
#define CPM_SPI_LEN_7_OPT	(6<<4)	/* 7 bits per character                              */
#define CPM_SPI_LEN_8_OPT	(7<<4)	/* 8 bits per character                              */
#define CPM_SPI_LEN_9_OPT	(8<<4)	/* 9 bits per character                              */
#define CPM_SPI_LEN_10_OPT	(9<<4)	/* 10 bits per character                             */
#define CPM_SPI_LEN_11_OPT	(10<<4)	/* 11 bits per character                             */
#define CPM_SPI_LEN_12_OPT	(11<<4)	/* 12 bits per character                             */
#define CPM_SPI_LEN_13_OPT	(12<<4)	/* 13 bits per character                             */
#define CPM_SPI_LEN_14_OPT	(13<<4)	/* 14 bits per character                             */
#define CPM_SPI_LEN_15_OPT	(14<<4)	/* 15 bits per character                             */
#define CPM_SPI_LEN_16_OPT	(15<<4)	/* 16 bits per character                             */


/*=============================================================================================*/
/* Macros */

/* get the indexes from the channel number */
#define cpm_ch_scc(x) (x >> 2)
#define cpm_ch_smc(x) ((x >> 2) & 1)

/* get the event and status registers */
#define cpm_scc_status(scc) (quicc->scc_regs[cpm_ch_scc(scc)].scc_sccs)
#define cpm_scc_event(scc)  (quicc->scc_regs[cpm_ch_scc(scc)].scc_scce)
#define cpm_scc_mask(scc)   (quicc->scc_regs[cpm_ch_scc(scc)].scc_sccm)
#define cpm_smc_event(smc)  (quicc->smc_regs[cpm_ch_smc(smc)].smc_smce)
#define cpm_smc_mask(smc)   (quicc->smc_regs[cpm_ch_smc(smc)].smc_smcm)

/* get the ports value */
#define cpm_porta() (quicc->pio_padat)
#define cpm_portb() (quicc->pip_pbdat)
#define cpm_portc() (quicc->pio_pcdat)


/*=============================================================================================*/
/* Callbacks */

typedef void (*Cpm_event_fptr)(unsigned short event_mask);
typedef void (*Cpm_txrx_fptr)(int sxc, const void *buff, unsigned short size,
							  unsigned short status);
typedef void (*Cpm_timer_fptr)(void);
typedef void (*Cpm_portc_fptr)(void);


/*=============================================================================================*/
/* Prototypes */

/*---------------------------------------------------------------------------------------------*/

void           cpm_init(int cicr);
void           cpm_cpic_hp(int int_vec);

/*---------------------------------------------------------------------------------------------*/

void           cpm_brg_conf(int brg, unsigned long baud_rate, int oversampling);

/*---------------------------------------------------------------------------------------------*/

void           cpm_si_scc_link(int scc, int tx_brg_clk, int rx_brg_clk);
void           cpm_si_smc_link(int smc, int brg_clk);

/*---------------------------------------------------------------------------------------------*/

void           cpm_smc_uart_conf(int smc, int tx_num, int rx_num, char allow_nesting);
void           cpm_scc_uart_conf(int scc, int tx_num, int rx_num, char allow_nesting);
void           cpm_scc_hdlc_conf(int scc, int max_frame_size, short opt,
								 int tx_num, int rx_num, char allow_nesting);
void           cpm_sxc_pin_conf(int sxc, int pins);
void           cpm_sxc_event_enable(int sxc, unsigned short event_mask, Cpm_event_fptr event_func);

/*---------------------------------------------------------------------------------------------*/

void           cpm_sxc_send_enable(int sxc, Cpm_txrx_fptr cpm_send_confirm_func);
void           cpm_sxc_send_disable(int sxc);
char           cpm_sxc_send(int sxc, const void *buf, unsigned short size);
unsigned short cpm_sxc_send_num(int sxc);

/*---------------------------------------------------------------------------------------------*/

void           cpm_sxc_receive_enable(int sxc, Cpm_txrx_fptr cpm_receive_func);
void           cpm_sxc_receive_disable(int sxc);
char           cpm_sxc_receive(int sxc, void *buf);
void           cpm_sxc_receive_close(int sxc);
unsigned short cpm_sxc_receive_num(int sxc);

unsigned short cpm_sxc_uart_receive_set_size(int sxc, unsigned short size);
void           cpm_scc_hdlc_receive_set_address(int scc, unsigned short hmask,
												unsigned short haddr1, unsigned short haddr2,
												unsigned short haddr3, unsigned short haddr4);

/*---------------------------------------------------------------------------------------------*/

void           cpm_spi_conf(unsigned long baud_rate, unsigned short opt,
							Cpm_txrx_fptr cpm_receive_func, char allow_nesting);
void           cpm_spi_send_receive(const void *send_buf, void *receive_buf, unsigned short size);

/*---------------------------------------------------------------------------------------------*/

void           cpm_timer_init(int timer, Cpm_timer_fptr f, char allow_nesting);
void           cpm_timer_set_periodic(int timer, unsigned short time_us);

/*---------------------------------------------------------------------------------------------*/

void		   cpm_risctimer_init(unsigned long risc_timer_us, char allow_nesting);
void		   cpm_risctimer_subscribe(int timer, Cpm_timer_fptr f);
void		   cpm_risctimer_start(int timer, unsigned short ticks, char reload);
void		   cpm_risctimer_stop(int timer);
unsigned long  cpm_risctimer_counter(void);

/*---------------------------------------------------------------------------------------------*/

void           cpm_portc_connect(int mask, int int_mode, Cpm_portc_fptr f, char allow_nesting);
void           cpm_portc_enable(int mask);
void           cpm_portc_disable(int mask);

/*---------------------------------------------------------------------------------------------*/

/*=============================================================================================*/


#endif

