/*=============================================================================================*/
/* Serial standard input/output routines using the 68360 CPM    					           */
/* Implementation file (sio_cpm.c)													           */
/*																					           */
/* Version 1.0																		           */
/*																					           */
/* (c)1997 Ansaldo Trasporti														           */
/* Written by Giuseppe Carnevale													           */
/*=============================================================================================*/


/*=============================================================================================*/
/* Includes */

#include <stdio.h>
#include <string.h>

#include "c_utils.h"
#include "atr_hw.h"
#include "hw_com.h"
#include "cpmdrv.h"
#include "pi_sys.h"


/*=============================================================================================*/
/* Defines */

#define OUT_RING_BUF_SIZE	2048	/* output ring buffer size                      */
#define IN_RING_BUF_SIZE	2048	/* input ring buffer size                       */

#define IN_CPM_NUM			16		/* number of RX active at the same time         */
#define IN_CPM_BUF_SIZE		1		/* input CPM buffer size (!!! only 1 supported) */


/*=============================================================================================*/
/* Globals */

static int		 the_sxc;	/* serial channel to use */
static int		 flag_id;	/* event flag id 1=output, 2=input */
static int		 err;		/* VRTX error code */

static char		 	 out_ring_buffer[OUT_RING_BUF_SIZE];	/* buffer used to hold the output data */
static CRingDesc 	 out_ring = {out_ring_buffer, sizeof(out_ring_buffer), 0, 0};
static int		 	 out_mutex;								/* mutex used to access the output     */
static char		 	 out_put_in_progress;					/* if TRUE a put is in progress        */
static volatile char out_send_in_progress;					/* if TRUE a send is in progress       */

static unsigned short in_request = 1;							/* number of bytes in request          */
static char			  in_buffer[IN_CPM_NUM][IN_CPM_BUF_SIZE];	/* buffers used by the CPM             */
static char			  in_ring_buffer[IN_RING_BUF_SIZE];			/* buffer used to hold the input data  */
static CRingDesc	  in_ring = {in_ring_buffer, sizeof(in_ring_buffer), 0, 0};


/*=============================================================================================*/
/* Output functions */

/*---------------------------------------------------------------------------------------------*/
/* Output callback */

static void put_callback(int sxc, const void *buff, unsigned short size, unsigned short status)
{
	/* remove the sent data from the ring and signal this */
	if (size) {
		c_ring_get(&out_ring, 0, size);
		sc_fpost(flag_id, 1, &err);
	}

	/* send the next data */
	size = c_ring_ready_size_cont(&out_ring);
	if (size) {
		out_send_in_progress = 1;
		cpm_sxc_send(sxc, out_ring.ring + out_ring.rp, size);
	}
	else out_send_in_progress = 0;
}


/*---------------------------------------------------------------------------------------------*/
/* Buffer output routine */

void sio_cpm_put(const char *buf, short req_num)
{
	Cpu_Sr	sr;
	int		i;

	/* check if we are in a task or in an ISR */
	if (!get_ilvl()) {
		
		/* we are in a task: wait on the mutex */
		sc_mpend(out_mutex, 0, &err);
		out_put_in_progress = 1;
	}
	else {
		
		/* we are in an ISR: check if we can enter or abort */
		int_disable(sr);
		if (out_put_in_progress) {
			int_enable(sr);
			return;
		}
		out_put_in_progress = 1;
		int_enable(sr);
	}

	while (req_num) {

		/* clear the output flag */
		sc_fclear(flag_id, 1, &err);

		/* send as much data as we can */
		i = c_ring_put(&out_ring, buf, req_num);
		buf += i;
		req_num -= i;

		/* start the transfer if needed */
		if (!cpm_sxc_send_num(the_sxc)) put_callback(the_sxc, 0, 0, 0);

		/* always exit if we are in an ISR */
		if (get_ilvl()) break;

		/* wait if we have other characters to output */
		if (req_num) sc_fpend(flag_id, 0, 1, 0, &err);
	}

	/* signal to the ISRs that we are free and let enter other tasks*/
	out_put_in_progress = 0;
	if (!get_ilvl()) sc_mpost(out_mutex, &err);
}


/*---------------------------------------------------------------------------------------------*/
/* Output flush routine */

void sio_cpm_put_flush(void)
{
	if (!get_ilvl()) while (out_send_in_progress);
}


/*=============================================================================================*/
/* Input functions */

/*---------------------------------------------------------------------------------------------*/
/* Input callback */

static void get_callback(int sxc, const void *buff, unsigned short size, unsigned short status)
{
//	unsigned short act_request;		/* number of bytes to request */

	/* put the received data in the input ring and wake up the task */
	c_ring_put(&in_ring, buff, size);
//	if (in_request > size) in_request -= size;
//	else in_request = 1;
	sc_fpost(flag_id, 2, &err);

	/* set the new buffer request and abort a previous transfer if different */
//	act_request = (in_request > IN_CPM_BUF_SIZE ? IN_CPM_BUF_SIZE : in_request);
//	if (cpm_sxc_uart_receive_set_size(sxc, act_request) != act_request) cpm_sxc_receive_close(sxc);

	/* start a new transfer */
	cpm_sxc_receive(sxc, (void*)buff);
}


/*---------------------------------------------------------------------------------------------*/
/* Buffer input routine; call it from 1 task ONLY! */

void sio_cpm_get(char *buf, short req_num, short timeout, short *act_num)
{
	unsigned short	i;
	int				pend_err = 0;

	/* initialize the number of transferred bytes */
	if (act_num) *act_num = 0;

	while (req_num) {

		/* get as much data as we can */
		i = c_ring_get(&in_ring, buf, req_num);
		buf += i;
		req_num -= i;
		if (act_num) *act_num += i;
		if (!req_num || timeout == PI_NO_WAIT || pend_err == ER_TMO) break;

		/* wait if we have other characters to input */
		sc_fpend(flag_id, timeout, 2, 0, &pend_err);
		sc_fclear(flag_id, 2, &err);
	}
}


/*=============================================================================================*/
/* Initialization routine */

void sio_cpm_init(int sxc, int brg_clk, unsigned long baud_rate)
{
	int i;
	
	/* save the serial channel to use, initialize the flag word and the output control mutex */
	the_sxc = sxc;
	flag_id = sc_fcreate(&err);
	out_mutex = sc_mcreate(2,&err);

	/* initialize the serial channel */
	if (baud_rate) cpm_brg_conf(brg_clk, baud_rate, 16);
	if (sxc == CPM_SMC1 || sxc == CPM_SMC2) {
		cpm_si_smc_link(sxc, brg_clk);
		cpm_smc_uart_conf(sxc, 1, IN_CPM_NUM, TRUE);
	}
	else {
		cpm_si_scc_link(sxc, brg_clk, brg_clk);
		cpm_scc_uart_conf(sxc, 1, IN_CPM_NUM, TRUE);
	}
	cpm_sxc_pin_conf(sxc, CPM_TX_PIN+CPM_RX_PIN);
	cpm_sxc_send_enable(sxc, put_callback);
	cpm_sxc_receive_enable(sxc, get_callback);

	/* start the first receive transfers */
	cpm_sxc_uart_receive_set_size(sxc, in_request);
	for (i = 0; i < IN_CPM_NUM; i ++) cpm_sxc_receive(sxc, in_buffer[i]);
}


/*=============================================================================================*/

