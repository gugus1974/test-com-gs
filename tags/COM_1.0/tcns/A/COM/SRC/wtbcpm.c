/*=============================================================================================*/
/* WTB Driver for the Motorola 68360                                                           */
/* Implementation file (wtbcpm.c)                                                              */
/*                                                                                             */
/* Version 1.1                                                                                 */
/*                                                                                             */
/* (c)1997 Ansaldo Trasporti                                                                   */
/* Written by Giuseppe Carnevale                                                               */
/*                                                                                             */
/* Version history:                                                                            */
/*   1.0 (xx/xx/xx) : first release                                                              */
/*=============================================================================================*/


/*=============================================================================================*/
/* Speed booster option: if you are really short of response time, the "cpmdrv.c" source file  */
/* could be included here (and of course excluded from the linking). This gives an extra speed */
/* because of the function inlining.                                                            */

// #include "cpmdrv.c"


/*=============================================================================================*/
/* Include files */

#include "wtbcpm.h"
#include "cpmdrv.h"
#include "pi_sys.h"
#include "hw_com.h"


/*=============================================================================================*/
/* Defines */

/* define the following macros to enable the use of the debug pins */
// #define DEBUG_PIN_CH_1	DOP1_OUTD19
// #define DEBUG_PIN_CH_2	DOP1_OUTD20

/* the following definitions set the hardware resources association for each channel */
#define SCC_CH_1		CPM_SCC3
#define SCC_CH_2		CPM_SCC4
#define CPIC_VEC_CH_1	CPIC_VEC_SCC3
#define CPIC_VEC_CH_2	CPIC_VEC_SCC4
#define TIMER_CH_1		CPM_TIMER3
#define TIMER_CH_2		CPM_TIMER4
#define CLK_RX_CH_1		CPM_CLK15
#define CLK_TX_CH_1		CPM_CLK26
#define CLK_RX_CH_2		CPM_CLK37
#define CLK_TX_CH_2		CPM_CLK48
#define CLK_PIN_RX_CH_1	CPM_CLK15_PIN
#define CLK_PIN_TX_CH_1	CPM_CLK26_PIN
#define CLK_PIN_RX_CH_2	CPM_CLK37_PIN
#define CLK_PIN_TX_CH_2	CPM_CLK48_PIN

/* this is the timer used internally by this driver to periodically check some signals */
#define D_PERIODIC_TIMER_NUM		3
#define D_PERIODIC_TIMER_PERIOD		100

/* this is the WTB* fritting control timer */
#define D_FRITTING_TIMER_NUM		4

/* TLD and OLD logical signals */
#define TLD1 ((cpm_portc() & PORTC_TLD1_) == 0)
#define TLD2 ((cpm_portc() & PORTC_TLD2_) == 0)
#define OLD1 ((cpm_portb() & (1<<11)) == 0)
#define OLD2 ((cpm_porta() & (1<<11)) == 0)


/*=============================================================================================*/
/* Typedefs */

typedef struct {				/* ### WTB frame structure ### */
	type_idu_drv	data;		/* data used by the link layer */
	unsigned short	crc;		/* CRC-16                      */
} FrameWTB;

/* ### callback functions types ### */
typedef void (*T_p_udf_send_confirm)(type_idu_drv *p_idu, unsigned char send_state);
typedef void (*T_p_udf_rcv_event)(type_idu_drv *pdu, unsigned char line, unsigned char status);


/*=============================================================================================*/
/* Globals */

static const unsigned long def_pin_mask[]  = {	/* default pin mask   */
	DOP0_FRLOAD1_ | DOP0_FRLOAD2 | DOP0_KFB_ | DOP0_KFA_ | DOP0_K4B_ | DOP0_K4A_ | DOP0_K3B_ |
	DOP0_K3A_ | DOP0_K1B_ | DOP0_K1A_ | DOP0_SEL2A | DOP0_SEL1A | DOP0_GFONA_ | DOP0_GFONB_ |
	DOP0_RESETB_ | DOP0_RESETA_, DOP1_SLEEPM_ };
static const unsigned long def_pin_value[] = {	/* default pin values */
	DOP0_KFB_ | DOP0_KFA_ | DOP0_K4B_ | DOP0_K4A_ | DOP0_K3B_ |
	DOP0_K3A_ | DOP0_K1B_ | DOP0_K1A_ | DOP0_GFONA_ | DOP0_GFONB_,
	DOP1_SLEEPM_ };

int				mau_type = D_MODE_MAU_UNK;		/* MAU interface we are using        */
unsigned long	BITRATE  = 1000000;				/* bit rate in bits/s used WTB-LL    */
static int		av_lines = D_LINE_A | D_LINE_B;	/* available WTB lines               */

static T_p_udf_send_confirm	send_confirm[2];	/* send confirmation procedures      */
static X_p_udf_send_confirm	trace_send_confirm;	/* send confirmation trace procedure */
static T_p_udf_rcv_event	receive_event[2];	/* receive procedures                */
static X_p_udf_rcv_event	trace_receive_event;/* receive trace procedure           */
static long					rx_frame[2][2][(sizeof(FrameWTB)+2)/sizeof(long)]; /* rx frames */
static type_idu_drv			*tx_frame[2];		/* tx frame pointer                  */
static type_StatisticData	stat;				/* statistic info                    */
static WTBDriverStatus		d;					/* driver status                     */


/*=============================================================================================*/
/* State machine */

/* Each channel has a state that depends on what is happening on it: */
typedef enum {
	state_A, /* the channel is not initialized                                   */
	state_B, /* the channel is free                                              */
	state_C, /* receiving something not requested                                */
	state_D, /* started a TX with ACK, waiting start of RX                       */
	state_E, /* transmitting and receiving a frame with ACK                      */
	state_F, /* frame with ACK transmitted, waiting for echo                     */
	state_G, /* frame with ACK transmitted and received, waiting for reply start */
	state_H, /* receiving the reply                                              */
	state_I, /* started a TX, waiting start of RX                                */
	state_J, /* transmitting and receiving a frame                               */
	state_K, /* frame transmitted, waiting for echo                              */
	state_L, /* waiting for minimum interframe time                              */
	state_X  /* the channel is not working                                       */
} State;

/* The transitions among these states are made when one of these events occours: */
/* event_configure : the channel has been configured                             */
/* event_rx_start  : receive started on the channel                              */
/* event_rx_end    : receive ended on the channel                                */
/* event_tx_start  : transmission started on the channel                         */
/* event_tx_end    : transmission ended on the channel                           */
/* event_timeout   : got a timeout                                               */

/* To ease the task of this driver the following applies:                        */
/* - minimum interframe spacing requirements is done in hardware delaying CTS;   */
/* - event_rx_start is not detected;                                             */
/* - the reception of the sent frames is disabled.                               */
/* This implies that state_C/D/F/I/K/L are no longer present.                    */

/* The following "picture" gives an idea of when the state transitions happen:   */
/*    AAABBB BBBCCCCCCCCLLLBBB BBBDEEEEEEEFGGGHHHHHHHHLLLBBB BBBIJJJJJJJKLLLBBB  */
/*    AAABBB BBBBBBBBBBBBBBBBB BBBEEEEEEEEGGGGGGGHHHHHBBBBBB BBBJJJJJJJJBBBBBBB  */
/* TX ???--- ----------------- ---XXXXXXXX------------------ ---XXXXXXXX-------  */
/* RX ???--- ---XXXXXXXX------ ----xxxxxxxx---XXXXXXXX------ ----xxxxxxxx------  */

State state[2] = {state_A, state_A};

#define set_X_state(X, new_state) if (1) {													\
	state[X-1] = new_state;																	\
	if (new_state == state_B) cpm_timer_set_periodic(TIMER_CH_##X, 0);						\
	else if (new_state == state_G) cpm_timer_set_periodic(TIMER_CH_##X, d.ch[X-1].timeout);	\
	else cpm_timer_set_periodic(TIMER_CH_##X, d.ch[X-1].panic_timeout);						\
} else


/*=============================================================================================*/
/* Private functions */

/*---------------------------------------------------------------------------------------------*/
/* Toggle the debug pins */

#if defined(DEBUG_PIN_CH_1) && defined(DEBUG_PIN_CH_2)
#define d_X_debug_pin(X, status) set_out_port(1, DEBUG_PIN_CH_##X, status ? DEBUG_PIN_CH_##X : 0)
#else
#define d_X_debug_pin(X, status)
#endif


/*---------------------------------------------------------------------------------------------*/
/* Signal one of the counted events */

#define inc_X_counter(X, counter) {												\
	if (d.ch[X-1].line == D_LINE_A) stat.A##X.counter++, d.stat.A##X.counter++;	\
	else stat.B##X.counter++, d.stat.B##X.counter++;							\
}


/*---------------------------------------------------------------------------------------------*/
/* Set the trusted line for each channel */

#define d_X_set_trusted(X, new_line) if (1) {										\
	if (new_line == D_LINE_A) set_out_port(0, DOP0_SEL##X##A, DOP0_SEL##X##A);		\
	else set_out_port(0, DOP0_SEL##X##A, 0);										\
	d.ch[X-1].line = new_line;														\
} else


/*---------------------------------------------------------------------------------------------*/
/* Update the line disturbed statuses from the hardware signals */

#define d_X_update_disturbance(X) if (1) {											\
	char tld = TLD##X;																\
	char old = OLD##X;																\
	if (d.ch[X-1].line == D_LINE_A) d.ch[X-1].ald = tld, d.ch[X-1].i_ald |= tld,	\
									d.ch[X-1].bld = old, d.ch[X-1].i_bld |= old;	\
	else                            d.ch[X-1].ald = old, d.ch[X-1].i_ald |= old,	\
									d.ch[X-1].bld = tld, d.ch[X-1].i_bld |= tld;	\
} else


/*---------------------------------------------------------------------------------------------*/
/* Do the automatic trusted line switching if applicable (tld_sw is the one-shot software TLD) */

#define d_X_check_line_switch(X, tld_sw) if (1) {									\
	char tld = TLD##X;																\
	char old = OLD##X;																\
	d_X_update_disturbance(X);														\
	if (d.ch[X-1].auto_sw && (tld || tld_sw) && !old) {								\
		stat.LineSwitch++, d.stat.LineSwitch++;										\
		if (d.ch[X-1].line == D_LINE_A) d_X_set_trusted(X, D_LINE_B);				\
		else							d_X_set_trusted(X, D_LINE_A);				\
		d.ch[X-1].ignore_rx_err = TRUE;												\
	}																				\
} else


/*---------------------------------------------------------------------------------------------*/
/* Call the user confirmation procedures */

#define p_udf_X_send_confirm(X, p_idu, send_state) if (1) {						\
	if (send_confirm[X-1]) (*send_confirm[X-1])(p_idu, send_state);				\
	if (trace_send_confirm) (*trace_send_confirm)(X, p_idu, send_state);		\
} else

#define p_udf_X_receive_event(X, p_idu, status)	if (1) {									\
	unsigned char line = d.ch[X-1].line | 													\
						 (((av_lines & D_LINE_A) && d.ch[X-1].ald) ? D_LINE_A_ERR : 0) |	\
						 (((av_lines & D_LINE_B) && d.ch[X-1].bld) ? D_LINE_B_ERR : 0);		\
	if (receive_event[X-1]) (*receive_event[X-1])(p_idu, line, status);			\
	if (trace_receive_event) (*trace_receive_event)(X, p_idu, line, status);	\
} else


/*=============================================================================================*/
/* Callbacks                                                                                   */
/* These function are called at interrupt time. We assume that the timer has a lower priority  */
/* than the TX/RX/event interrupts.                                                            */

/*---------------------------------------------------------------------------------------------*/
#define tx_X_callback(X, buff, size, status) {										\
	d_X_debug_pin(X, 1);															\
																					\
	/* check the line status */														\
	d_X_update_disturbance(X);														\
																					\
	/* increment the counter of transmitted frames */								\
	inc_X_counter(X, NoTxF);														\
																					\
	/* change the driver state */													\
	switch (state[X-1]) {															\
		case state_E:																\
			/* start the reply waiting and confirm the sent frame */				\
			set_X_state(X, state_G);												\
			p_udf_X_send_confirm(X, tx_frame[X-1], D_OK);							\
			break;																	\
		case state_J:																\
			/* go to idle state and confirm the sent frame */						\
			set_X_state(X, state_B);												\
			p_udf_X_send_confirm(X, tx_frame[X-1], D_OK);							\
			break;																	\
	}																				\
																					\
	d_X_debug_pin(X, 0);															\
}

static void tx_1_callback(int sxc, const void *buff, unsigned short size, unsigned short status)
{ tx_X_callback(1, buff, size, status); }
static void tx_2_callback(int sxc, const void *buff, unsigned short size, unsigned short status)
{ tx_X_callback(2, buff, size, status); }


/*---------------------------------------------------------------------------------------------*/
#define rx_X_callback(X, buff, size, status) {													\
	int	valid;	/* if TRUE the received frame is valid */										\
																								\
	/* if the receiver is disabled just ignore the received frame */							\
	if (!d.ch[X-1].rx)                                                                          \
	{																		                    \
		cpm_sxc_receive(SCC_CH_##X, (void*)buff);												\
    	return;																					\
	}																							\
																								\
	d_X_debug_pin(X, 1);																		\
																								\
	if (status & (CPM_BD_DE|CPM_BD_LG|CPM_BD_NO|CPM_BD_AB|CPM_BD_CR|CPM_BD_OV|CPM_BD_CD) ||		\
		size < 6 || ((FrameWTB*)buff)->data.size + 6 != size) {									\
																								\
		/* we received a frame with error */													\
		inc_X_counter(X, NoFE);																	\
		valid = FALSE;																			\
																								\
		/* set the trusted line as disturbed (if we do not allow this error) */					\
		if (d.ch[X-1].ignore_rx_err) {															\
			d.ch[X-1].ignore_rx_err = FALSE;													\
			d_X_update_disturbance(X);															\
		}																						\
		else d_X_check_line_switch(X, TRUE);													\
																								\
		/* start a new transfer */																\
		cpm_sxc_receive(sxc, (void*)buff);														\
	}																							\
	else {																						\
		/* we received a valid frame */															\
		inc_X_counter(X, NoRxF);																\
		valid = TRUE;																			\
		d_X_update_disturbance(X);																\
	}																							\
																								\
	/* change the driver state */																\
	switch (state[X-1]) {																		\
		case state_B:																			\
			/* we received a new frame */														\
			set_X_state(X, state_B);															\
			if (valid) p_udf_X_receive_event(X, (void*)buff, D_RPT_FRAME_RECEIVED);				\
			break;																				\
		case state_G:																			\
		case state_H:																			\
			/* we received the reply */															\
			set_X_state(X, state_B);															\
			if (valid) p_udf_X_receive_event(X, (void*)buff, D_RPT_FRAME_RECEIVED);				\
			else p_udf_X_receive_event(X, 0, D_RPT_TIMEOUT);									\
			break;																				\
		default:																				\
			/* we have a "virtual" collision; discard the received frame */						\
			if (valid) cpm_sxc_receive(sxc, (void*)buff);										\
			break;																				\
	}																							\
																								\
	d_X_debug_pin(X, 0);																		\
}

static void rx_1_callback(int sxc, const void *buff, unsigned short size, unsigned short status)
{ rx_X_callback(1, buff, size, status); }
static void rx_2_callback(int sxc, const void *buff, unsigned short size, unsigned short status)
{ rx_X_callback(2, buff, size, status); }


/*---------------------------------------------------------------------------------------------*/
#define event_X_callback(X, event_mask) {											\
	d_X_debug_pin(X, 1);															\
																					\
	d_X_debug_pin(X, 0);															\
}

static void event_1_callback(unsigned short event_mask) { event_X_callback(1, event_mask); }
static void event_2_callback(unsigned short event_mask) { event_X_callback(2, event_mask); }


/*---------------------------------------------------------------------------------------------*/
#define timer_X_callback(X) {														\
	d_X_debug_pin(X, 1);															\
																					\
	switch (state[X-1]) {															\
		case state_G:																\
			/* inspect the line to check its busy condition */						\
			if ((cpm_scc_event(SCC_CH_##X) & cpm_scc_mask(SCC_CH_##X)) || 			\
					!(cpm_scc_status(SCC_CH_##X) & CPM_S_ID)) {						\
				set_X_state(X, state_H);											\
				break;																\
			}																		\
		case state_H:																\
			/* no valid reception detected */										\
			inc_X_counter(X, NoTO);													\
			set_X_state(X, state_B);												\
			p_udf_X_receive_event(X, 0, D_RPT_TIMEOUT);								\
			break;																	\
		default:																	\
			/* this is a "panic" timeout; signal hardware fault */					\
			set_out_port(0, def_pin_mask[0], def_pin_value[0]);						\
			set_out_port(1, def_pin_mask[1], def_pin_value[1]);						\
			cpm_portc_disable(PORTC_TLD##X##_);										\
			cpm_sxc_send_disable(SCC_CH_##X);										\
			cpm_sxc_receive_disable(SCC_CH_##X);									\
			set_X_state(X, state_X);												\
			d.mauhf = TRUE;															\
	}																				\
																					\
	d_X_debug_pin(X, 0);															\
}

static void timer_1_callback(void) { timer_X_callback(1); }
static void timer_2_callback(void) { timer_X_callback(2); }


/*---------------------------------------------------------------------------------------------*/

static void tld_1_callback(void) { d_X_check_line_switch(1, FALSE); }
static void tld_2_callback(void) { d_X_check_line_switch(2, FALSE); }


/*=============================================================================================*/
/* Periodic signal monitoring */

/*---------------------------------------------------------------------------------------------*/
/* Update the hardware fault statuses from the hardware signals */

#define d_check_hw_fault_L(L) if (1) {										\
	static unsigned short HF##L##c;		/* counter for the line */			\
																			\
	if (HF##L##c) HF##L##c--;												\
	else {																	\
		if ((get_in_port(0) & DIN0_HWFLT##L##_) == 0) {						\
			set_out_port(0, DOP0_RESET##L##_, 0);							\
			set_out_port(0, DOP0_RESET##L##_, DOP0_RESET##L##_);			\
			if (d.HF##L##p == 0) d.HF##L##p = 1;							\
			else if (d.HF##L##p < 32768) d.HF##L##p *= 2;					\
			else d.HF##L##p = 65535;										\
			HF##L##c = d.HF##L##p;											\
		}																	\
		else if (d.HF##L##p) d.HF##L##p--;									\
	}																		\
} else

static void d_periodic_timer(void)
{
	d_check_hw_fault_L(A);
	d_check_hw_fault_L(B);
}


/*=============================================================================================*/
/* WTB* fritting control timer */

static char fritting_state = 0;	/* the current state of the "fritting machine" */


/*---------------------------------------------------------------------------------------------*/
/* Switch the fritting on the WTB* directions */

static void d_fritting_timer(void)
{
	Cpu_Sr		sr;

	switch (fritting_state) {
		case 1: /* fritting ON on A2 */
			set_out_port(0, DOP0_KFA_ | DOP0_GFONA_ | DOP0_FRLOAD1_ | DOP0_FRLOAD2, 0);
			fritting_state = 2;
			break;

		case 2: /* fritting ON on A1 */
			set_out_port(0, DOP0_KFA_ | DOP0_GFONA_ | DOP0_FRLOAD1_ | DOP0_FRLOAD2,
							DOP0_FRLOAD1_ | DOP0_FRLOAD2);
			fritting_state = 3;
			break;

		case 3: /* go to the OFF position */
			fritting_state = 0;

		default: /* fritting OFF */
			set_out_port(0, DOP0_KFA_ | DOP0_GFONA_ | DOP0_FRLOAD1_ | DOP0_FRLOAD2,
								DOP0_KFA_ | DOP0_GFONA_ | DOP0_FRLOAD2);
	}

	if (fritting_state != 0) d_tc_timer_reset(D_FRITTING_TIMER_NUM, 50 + 2 * d_get_rand());
}


/*---------------------------------------------------------------------------------------------*/
/* Start the fritting on the WTB* directions */

static d_fritting_timer_on(void)
{
	if (fritting_state == 0) {
		fritting_state = 1;
		d_fritting_timer();
	}
}


/*---------------------------------------------------------------------------------------------*/
/* Stop the fritting on the WTB* directions */

static d_fritting_timer_off(void)
{
	d_tc_timer_reset(D_FRITTING_TIMER_NUM, 0);
	fritting_state = 0;
	d_fritting_timer();
}


/*=============================================================================================*/
/* Driver initialization */

/*---------------------------------------------------------------------------------------------*/
/* This one is called by the application *before* the WTB LL starts */

unsigned char d_configure(int mau_kind, unsigned long bit_rate, int lines_avail)
{
	/* if we are using the ATR's MAU we need to fix a few signals */
	if (mau_kind == D_MODE_MAU_ATR)
		set_out_port_xor(0,
			DOP0_KFB_|DOP0_KFA_|DOP0_K4B_|DOP0_K4A_|DOP0_K3B_|DOP0_K3A_|DOP0_K1B_|DOP0_K1A_|
			DOP0_GFONA_|DOP0_GFONB_,
			DOP0_KFB_|DOP0_KFA_|DOP0_K4B_|DOP0_K4A_|DOP0_K3B_|DOP0_K3A_|DOP0_K1B_|DOP0_K1A_|
			DOP0_GFONA_|DOP0_GFONB_);

	/* save the specified parameters */
	mau_type = mau_kind;
	BITRATE  = bit_rate;
	av_lines = lines_avail;

	/* fix the fritting loads for WTB* */
	if (mau_type == D_MODE_MAU_ATR && av_lines == D_LINE_A)
		set_out_port(0, DOP0_FRLOAD1_ | DOP0_FRLOAD2, DOP0_FRLOAD2);

	/* pre-configure the output pins to avoid problems if the MAU is not used */
	cpm_sxc_pin_conf(SCC_CH_1, CPM_TX_PIN | CPM_RX_PIN | CPM_RTS_B_PIN | CPM_CTS_PIN |
					 CPM_CD_PIN | CLK_PIN_RX_CH_1 | CLK_PIN_TX_CH_1);
	cpm_sxc_pin_conf(SCC_CH_2, CPM_TX_PIN | CPM_RX_PIN | CPM_RTS_B_PIN | CPM_CTS_PIN |
					 CPM_CD_PIN | CLK_PIN_RX_CH_2 | CLK_PIN_TX_CH_2);

	return D_OK;
}


/*---------------------------------------------------------------------------------------------*/
/* This one is called by the WTB LL */

unsigned char d_sysinit(void)
{
	/* initialize the MAU pins and remove the reset */
	set_out_port(0, def_pin_mask[0], def_pin_value[0]);
	set_out_port(1, def_pin_mask[1], def_pin_value[1]);
	set_out_port(0, DOP0_RESETB_ | DOP0_RESETA_, DOP0_RESETB_ | DOP0_RESETA_);

	/* initialize the driver statistics */
	d_clear_statistic();

	/* initialize the driver status */
	d.direction      = D_DIRECT_1;
	d.interruption_A = D_OFF;
	d.interruption_B = D_OFF;
	d.ch[0].switch_A = d.ch[1].switch_A = D_OPEN;
	d.ch[0].switch_B = d.ch[1].switch_B = D_OPEN;
	d.ch[0].line     = d.ch[1].line     = D_LINE_A;
	d.ch[0].auto_sw  = d.ch[1].auto_sw  = FALSE;
	d.ch[0].rx       = d.ch[1].rx       = FALSE;
	d.ch[0].tx       = d.ch[1].tx       = FALSE;

	/* start the periodic timer */
	d_tc_timer_subscribe(D_PERIODIC_TIMER_NUM, d_periodic_timer, D_MODE_CYCLIC);
	d_tc_timer_control(D_PERIODIC_TIMER_NUM, D_TC_PROC);
	d_tc_timer_reset(D_PERIODIC_TIMER_NUM, D_PERIODIC_TIMER_PERIOD);

	/* install the fritting timer */
	d_tc_timer_subscribe(D_FRITTING_TIMER_NUM, d_fritting_timer, D_MODE_NONCYCLIC);
	d_tc_timer_control(D_FRITTING_TIMER_NUM, D_TC_PROC);

	return D_OK;
}


/*=============================================================================================*/
/* Install the specified channel */

#define d_X_install(X, p_udf_send_confirm, p_udf_rcv_event, baudrate, timeout) {				\
	/* initialize the line parameters with the user's supplied ones */							\
	send_confirm[X-1]       = p_udf_send_confirm;												\
	receive_event[X-1]      = p_udf_rcv_event;													\
	d.ch[X-1].baudrate      = baudrate;															\
	d.ch[X-1].timeout       = timeout * 10;														\
	d.ch[X-1].panic_timeout = 1000000;															\
																								\
	if (state[X-1] == state_X) return D_ERR_MAU_FAILURE;										\
																								\
	if (state[X-1] == state_A) {																\
		void *p;																				\
																								\
		/* initialize the SCCs and the timer used by the channel */ 							\
		cpm_si_scc_link(SCC_CH_##X, CLK_TX_CH_##X, CLK_RX_CH_##X);								\
		cpm_scc_hdlc_conf(SCC_CH_##X, sizeof(FrameWTB), CPM_HDLC_DRT_OPT, 1, 2, FALSE);			\
		cpm_sxc_pin_conf(SCC_CH_##X, CPM_TX_PIN | CPM_RX_PIN | CPM_RTS_B_PIN | CPM_CTS_PIN |	\
						 CPM_CD_PIN | CLK_PIN_RX_CH_##X | CLK_PIN_TX_CH_##X);					\
		cpm_sxc_event_enable(SCC_CH_##X, HDLC_EVENT_BSY, event_##X##_callback);					\
		cpm_timer_init(TIMER_CH_##X, timer_##X##_callback, FALSE);								\
																								\
		/* attach the line monitoring interrupt */												\
		cpm_portc_connect(PORTC_TLD##X##_, CPM_PORTC_INT_HI2LO, tld_##X##_callback, FALSE);		\
		cpm_portc_enable(PORTC_TLD##X##_);														\
																								\
		/* queue the reception of 2 frames */													\
		cpm_sxc_receive(SCC_CH_##X, (void*)&rx_frame[X-1][0]);									\
		cpm_sxc_receive(SCC_CH_##X, (void*)&rx_frame[X-1][1]);									\
																								\
		/* enable the transmitter and the receiver */											\
		cpm_sxc_send_enable(SCC_CH_##X, tx_##X##_callback);										\
		cpm_sxc_receive_enable(SCC_CH_##X, rx_##X##_callback);									\
	}																							\
	set_X_state(X, state_B);																	\
																								\
	return D_OK;																				\
}

unsigned char d_1_install(T_p_udf_send_confirm p_udf_send_confirm,
						  T_p_udf_rcv_event p_udf_rcv_event,
                          unsigned long baudrate, unsigned short timeout)
{ d_X_install(1, p_udf_send_confirm, p_udf_rcv_event, baudrate, timeout); }

unsigned char d_2_install(T_p_udf_send_confirm p_udf_send_confirm,
						  T_p_udf_rcv_event p_udf_rcv_event,
                          unsigned long baudrate, unsigned short timeout)
{ d_X_install(2, p_udf_send_confirm, p_udf_rcv_event, baudrate, timeout); }



/*=============================================================================================*/
/* Enable the transmitter of the specified channel */

#define d_X_tx_enable(X) {								\
	d.ch[X-1].tx = TRUE;								\
	return D_OK;										\
}

unsigned char d_1_tx_enable(void) { d_X_tx_enable(1); }
unsigned char d_2_tx_enable(void) { d_X_tx_enable(2); }


/*=============================================================================================*/
/* Disable the transmitter of the specified channel */

#define d_X_tx_disable(X) {				\
	d.ch[X-1].tx = FALSE;				\
	return D_OK;						\
}

unsigned char d_1_tx_disable(void) { d_X_tx_disable(1); }
unsigned char d_2_tx_disable(void) { d_X_tx_disable(2); }


/*=============================================================================================*/
/* Send a frame to the specified channel */

#define d_X_send_request(X, p_idu, mode) {												\
	/* check that we are in the correct state to send out something */					\
	if (state[X-1] == state_X) return D_ERR_MAU_FAILURE;								\
	if (state[X-1] != state_B)								                            \
	{								                                                    \
		d.maubusy = 1;			                                                        \
	    return D_ERR_NOT_ALLOWED;		                         						\
	}								                                                    \
	else 	                                                                            \
	    d.maubusy = 0;	                                                                \
																						\
	/* enable the transmitter if not yet done */										\
	if (!d.ch[X-1].tx) d_##X##_tx_enable();												\
																						\
	/* enable the receiver if not yet done */											\
	if (!d.ch[X-1].rx) d_##X##_rx_enable(D_NONSELECTIVE);								\
																						\
	/* save the frame pointer */														\
	tx_frame[X-1] = p_idu;																\
																						\
	/* update the status and start the transfer */										\
	if (mode == D_MODE_ACK) set_X_state(X, state_E);									\
	else set_X_state(X, state_J);														\
	cpm_sxc_send(SCC_CH_##X, p_idu, p_idu->size + 4);									\
	d.ch[X-1].ignore_rx_err = FALSE;													\
																						\
	return D_OK;																		\
}

unsigned char d_send_request(unsigned char dir_code, type_idu_drv *p_idu, unsigned char mode)
{
	if (!dir_code) d_X_send_request(1, p_idu, mode);
	d_X_send_request(2, p_idu, mode);
}


/*=============================================================================================*/
/* Enable the receiver of the specified channel */

#define d_X_rx_enable(X, rx_address) {														\
	if (rx_address == D_NONSELECTIVE)														\
		cpm_scc_hdlc_receive_set_address(SCC_CH_##X, 0, 0xFF, 0xFF, 0xFF, 0xFF);			\
	else cpm_scc_hdlc_receive_set_address(SCC_CH_##X, 0xFF, rx_address, 0xFF, 0xFF, 0xFF);	\
	d.ch[X-1].rx = TRUE;																	\
	d.ch[X-1].rx_addr = rx_address;															\
	return D_OK;																			\
}

unsigned char d_1_rx_enable(unsigned char rx_address) { d_X_rx_enable(1, rx_address); }
unsigned char d_2_rx_enable(unsigned char rx_address) { d_X_rx_enable(2, rx_address); }


/*=============================================================================================*/
/* Disable the receiver of the specified channel */

#define d_X_rx_disable(X) {					\
	d.ch[X-1].rx = FALSE;					\
	return D_OK;							\
}

unsigned char d_1_rx_disable(void) { d_X_rx_disable(1); }
unsigned char d_2_rx_disable(void) { d_X_rx_disable(2); }


/*=============================================================================================*/
/* Quit the indicated frame processing */

#define d_X_receive_quit(X, p_idu) {								\
	if (p_idu) cpm_sxc_receive(SCC_CH_##X, (void*)p_idu);			\
	return D_OK;													\
}

unsigned char d_1_receive_quit(type_idu_drv *p_idu) { d_X_receive_quit(1, p_idu); }
unsigned char d_2_receive_quit(type_idu_drv *p_idu) { d_X_receive_quit(2, p_idu); }


/*=============================================================================================*/
/* Line status information */

unsigned char d_get_cur_line_status(unsigned char *status)
{
	Cpu_Sr		  sr;
	unsigned char s = 0;

	/* build the status flags */
	int_disable(sr);
	if (av_lines & D_LINE_A) {
		if (d.ch[0].ald) s |= D_DA1;
		if (d.ch[1].ald) s |= D_DA2;
	}
	if (av_lines & D_LINE_B) {
		if (d.ch[0].bld) s |= D_DB1;
		if (d.ch[1].bld) s |= D_DB2;
	}
	int_enable(sr);

	*status = s;
	return D_OK;
}

unsigned char d_get_int_line_status(unsigned char *status)
{
	Cpu_Sr		  sr;
	unsigned char s = 0;

	/* build the status flags and reset the integrated line stats */
	int_disable(sr);
	if (av_lines & D_LINE_A) {
		if (d.ch[0].i_ald) s |= D_DA1;
		if (d.ch[1].i_ald) s |= D_DA2;
	}
	if (av_lines & D_LINE_B) {
		if (d.ch[0].i_bld) s |= D_DB1;
		if (d.ch[1].i_bld) s |= D_DB2;
	}
	d.ch[0].i_ald = d.ch[1].i_ald = d.ch[0].i_bld = d.ch[1].i_bld = FALSE;
	int_enable(sr);

	*status = s;
	return D_OK;
}


/*=============================================================================================*/
/* Statistic counters */

unsigned char d_read_statistic(type_StatisticData **pp_StatisticCnt)
{
	stat.p_detailed_state = (void*)&d;
	*pp_StatisticCnt = &stat;
	return D_OK;
}

unsigned char d_clear_statistic(void)
{
	/* reset the driver statistics */
	pi_zero8((void*)&stat, sizeof(stat));
	stat.p_detailed_state = (void*)&d;

	return D_OK;
}


/*=============================================================================================*/
/* Timing control */

/* Warning: the systime is max 2^28 * RISC_TIMERS_TICK_US (38.177 hours) */

#define TIMER_K		32768L
#define SYSTIME_K	16L

static struct {
	void			(*p_timeout_handler)(void);
	unsigned char	mode;
} d_timer[16];

unsigned char d_tc_timer_subscribe(unsigned char timer_no, void (*p_timeout_handler)(void),
								   unsigned char mode)
{
	d_timer[timer_no].p_timeout_handler = p_timeout_handler;
	d_timer[timer_no].mode              = mode;
	return D_OK;
}

unsigned char d_tc_timer_release(unsigned char timer_no)
{
	d_timer[timer_no].p_timeout_handler = 0;
	return D_OK;
}

unsigned char d_tc_timer_reset(unsigned char timer_no, unsigned short time)
{
	if (time) cpm_risctimer_start(timer_no, (time * ((TIMER_K * 1000) / RISC_TIMERS_TICK_US)) / TIMER_K,
								  d_timer[timer_no].mode == D_MODE_CYCLIC ? 1 : 0);
	else cpm_risctimer_stop(timer_no);
	return D_OK;
}

unsigned char d_tc_timer_control(unsigned char timer_no, unsigned char control)
{
	cpm_risctimer_subscribe(timer_no, control == D_TC_PROC ? d_timer[timer_no].p_timeout_handler : 0);
	return D_OK;
}

unsigned long d_tc_get_systime(void)
{
	return (cpm_risctimer_counter() * SYSTIME_K) / ((SYSTIME_K * 1000) / RISC_TIMERS_TICK_US);
}


/*=============================================================================================*/
/* Physical layer switches */

unsigned char d_sw_direction(unsigned char direction)
{
	/* save the main channel direction information and change the interrupt priorities */
	d.direction = direction;
	cpm_cpic_hp(direction == D_DIRECT_1 ? CPIC_VEC_CH_1 : CPIC_VEC_CH_2);
	return D_OK;
}

unsigned char d_sw_interruption(unsigned char line, unsigned char position)
{
	unsigned char err = D_OK;
	unsigned long mask = 0, value = 0;

	if (position != D_ON && position != D_OFF) Check(D_ERR_PARAM);

	if (line == D_LINE_A) {
		mask |= DOP0_K1A_;
		if (position == D_OFF) value |= DOP0_K1A_;
	}
	else if (line == D_LINE_B) {
		mask |= DOP0_K1B_;
		if (position == D_OFF) value |= DOP0_K1B_;
	}
	else Check(D_ERR_PARAM);

	if (line == D_LINE_A) d.interruption_A = position;
	else                  d.interruption_B = position;

	set_out_port(0, mask, value);

error:
	return err;
}

unsigned char d_sw_line_attachment(unsigned char switch_1A, unsigned char switch_1B,
                                   unsigned char switch_2A, unsigned char switch_2B)
{
	unsigned char err = D_OK;
	unsigned long mask = 0, value = 0;

	if ((switch_1A != D_OPEN && switch_1A != D_CLOSE) ||
	    (switch_1B != D_OPEN && switch_1B != D_CLOSE) ||
	    (switch_2A != D_OPEN && switch_2A != D_CLOSE) ||
	    (switch_2B != D_OPEN && switch_2B != D_CLOSE)) Check(D_ERR_PARAM);

	mask |= DOP0_K3A_ | DOP0_K3B_ | DOP0_K4A_ | DOP0_K4B_;

	if (switch_1A == D_OPEN)  value |= DOP0_K3A_;
	if (switch_1B == D_OPEN)  value |= DOP0_K3B_;
	if (switch_2A == D_OPEN)  value |= DOP0_K4A_;
	if (switch_2B == D_OPEN)  value |= DOP0_K4B_;

	d.ch[0].switch_A = switch_1A;
	d.ch[0].switch_B = switch_1B;
	d.ch[1].switch_A = switch_2A;
	d.ch[1].switch_B = switch_2B;

	set_out_port(0, mask, value);

error:
	return err;
}

unsigned char d_sw_line_receive(unsigned char line_ch1, unsigned char line_ch2)
{
	unsigned char err = D_OK;
	unsigned char auto_ch1 = FALSE, auto_ch2 = FALSE;
	unsigned long mask = 0, value = 0;

	/* fix request for single-line systems */
	if      (av_lines == D_LINE_A) line_ch1 = line_ch2 = D_LINE_A;
	else if (av_lines == D_LINE_B) line_ch1 = line_ch2 = D_LINE_B;

	/* decode the request */
	if (line_ch1 == D_LINE_AUTO) auto_ch1 = TRUE, line_ch1 == D_LINE_A;
	if (line_ch2 == D_LINE_AUTO) auto_ch2 = TRUE, line_ch2 == D_LINE_A;

	/* set the bits for line A */
	if      (line_ch1 == D_LINE_A) mask |= DOP0_SEL1A, value |= DOP0_SEL1A;
	else if (line_ch1 == D_LINE_B) mask |= DOP0_SEL1A;
	else Check(D_ERR_PARAM);

	/* set the bits for line B */
	if      (line_ch2 == D_LINE_A) mask |= DOP0_SEL2A, value |= DOP0_SEL2A;
	else if (line_ch2 == D_LINE_B) mask |= DOP0_SEL2A;
	else Check(D_ERR_PARAM);

	/* save the request */
	d.ch[0].line    = line_ch1;
	d.ch[1].line    = line_ch2;
	d.ch[0].auto_sw = auto_ch1;
	d.ch[1].auto_sw = auto_ch2;

	/* output the bits */
	set_out_port(0, mask, value);

error:
	return err;
}


/*=============================================================================================*/
/* Fritting */

unsigned char d_sw_frittung_control(unsigned char command, unsigned char direction,
									unsigned char line)
{
	/* special case for WTB* */
	if (mau_type == D_MODE_MAU_ATR && av_lines == D_LINE_A) switch (command) {
		case D_FRITTING_PASSIVE:
		case D_FRITTING_OFF:
			d_fritting_timer_off();
			if (direction == D_DIRECT_2) d.ch[1].fritting_on = FALSE;
			else if (direction == D_DIRECT_1) d.ch[0].fritting_on = FALSE;
			break;
		case D_FRITTING_ACTIVE:
			if (direction == D_DIRECT_2) {
				if (!d.ch[0].fritting_on && !d.ch[1].fritting_on) d_fritting_timer_on();
				d.ch[1].fritting_on = TRUE;
			}
			else if (direction == D_DIRECT_1) {
				if (!d.ch[0].fritting_on && !d.ch[1].fritting_on) d_fritting_timer_on();
				d.ch[0].fritting_on = TRUE;
			}
	}
	else switch (command) {
		case D_FRITTING_PASSIVE:
		case D_FRITTING_OFF:
			if (line == D_LINE_A && direction == D_DIRECT_2) {
				d.ch[1].fritting_on = FALSE;
				set_out_port(0, DOP0_KFA_ | DOP0_GFONA_, DOP0_KFA_ | DOP0_GFONA_);
			}
			else if (line == D_LINE_B && direction == D_DIRECT_1) {
				d.ch[0].fritting_on = FALSE;
				set_out_port(0, DOP0_KFB_ | DOP0_GFONB_, DOP0_KFB_ | DOP0_GFONB_);
			}
			break;
		case D_FRITTING_ACTIVE:
			if (line == D_LINE_A && direction == D_DIRECT_2) {
				d.ch[1].fritting_on = TRUE;
				set_out_port(0, DOP0_KFA_ | DOP0_GFONA_, 0);
			}
			else if (line == D_LINE_B && direction == D_DIRECT_1) {
				d.ch[0].fritting_on = TRUE;
				set_out_port(0, DOP0_KFB_ | DOP0_GFONB_, 0);
			}
	}
	return D_OK;
}


/*=============================================================================================*/
/* Sleep mode */

void d_sleep(void)
{
	Cpu_Sr			sr;
	unsigned long	timeout;

	/* disable anything else */
	int_disable(sr);

	/* put UNIM and UNIS to 0 and wait a second */
	set_out_port(1, DOP1_OUTD15 | DOP1_OUTD16, 0);
	timeout = d_tc_get_systime() + 1000;
	while (timeout > d_tc_get_systime()) hw_watchdog_service();

	/* start the sleep (if allowed) */
	set_out_port(1, DOP1_SLEEPM_, 0);

	/* after 1 second reset ourself if still alive */
	timeout = d_tc_get_systime() + 1000;
	while (timeout > d_tc_get_systime()) hw_watchdog_service();
	hw_reset();
}


/*=============================================================================================*/
/* Random number generator */

unsigned char d_get_rand(void)
{
	/* WARNING: this isn't a real random number; could just be used as seed */
	return cpm_risctimer_counter() & 0x7F;
}


/*=============================================================================================*/
/* Trace functions installer */

void d_trace_install(X_p_udf_send_confirm p_udf_send_confirm, X_p_udf_rcv_event p_udf_rcv_event)
{
	/* save the trace function pointers */
	trace_send_confirm  = p_udf_send_confirm;
	trace_receive_event = p_udf_rcv_event;
}


/*=============================================================================================*/

