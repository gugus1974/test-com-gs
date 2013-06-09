/*=============================================================================================*/
/* WTB Driver for the Motorola CPM                                                             */
/* Header file (wtbcpm.h)                                                                      */
/*                                                                                             */
/* Version 1.1                                                                                 */
/*                                                                                             */
/* (c)1997 Ansaldo Trasporti                                                                   */
/* Written by Giuseppe Carnevale                                                               */
/*                                                                                             */
/* Version history:                                                                            */
/*   1.0 (xx/xx/xx) : first release                                                            */
/*=============================================================================================*/

#ifndef _WTBCPM_
#define _WTBCPM_


/*=============================================================================================*/
/* Includes */

#include "wtbdrv.h"


/*=============================================================================================*/
/* Private section */

/*---------------------------------------------------------------------------------------------*/
/* Defines */

/* Available MAUs */
#define D_MODE_MAU_UNK		0	/* unknown MAU, use "standard" interface */
#define D_MODE_MAU_ATR		1	/* MAU made by ATR                       */

/* Error codes */
#define D_ERR_PARAM			1 	/* invalid parameter         */
#define D_ERR_NOT_ALLOWED	2 	/* command not allowed now   */
#define D_ERR_MAU_FAILURE	3 	/* the MAU is not responding */


/*---------------------------------------------------------------------------------------------*/
/* Typedefs */

typedef struct {
    unsigned long	baudrate;		/* line baudrate                                          */
    unsigned long	timeout;		/* reply frame timeout value [us]                         */
    unsigned long	panic_timeout;	/* generic panic timeout value [us]                       */
	unsigned char	switch_A,		/* K3A/K4A switches status (D_OPEN,D_CLOSE)               */
					switch_B;		/* K3B/K4A switches status (D_OPEN,D_CLOSE)               */
	unsigned char	line;			/* line associated to each channel (D_LINE_A,D_LINE_B)    */
	unsigned char	auto_sw;		/* automatic switch for each channel (FALSE,TRUE)         */
	unsigned char	rx, tx;			/* rx and tx enable status for each channel (FALSE,TRUE)  */
	unsigned char	rx_addr;		/* rx address for each channel                            */
	unsigned char	fritting_on;	/* if TRUE the fritting is requested on that line         */
	unsigned char	ald, bld;		/* A and B line disturbed flags                           */
	unsigned char	i_ald, i_bld;	/* integrated A and B line disturbed flags                */
	unsigned char	ignore_rx_err;	/* ignore the next RX error (we switched the channel)     */
} WTBChStatus;

typedef struct {
	type_StatisticData	stat;			/* statistic data copy (never cleared)                */
	unsigned char		mauhf;			/* MAU hardware failure flag (not recoverable)        */
	unsigned char		maubusy;        /* MAU not allowed flag (not recoverable)             */
	unsigned char		direction;		/* main channel direction (D_DIRECT_1,D_DIRECT_2)     */
	unsigned char		interruption_A,	/* K1A switch status (D_OFF,D_ON)                     */
						interruption_B;	/* K1B switch status (D_OFF,D_ON)                     */
	unsigned short		HFAp, HFBp;		/* hardware fault sampling period for each line       */
	WTBChStatus			ch[2];			/* channel status                                     */
} WTBDriverStatus;

/* trace callback functions types */
typedef void (*X_p_udf_send_confirm)(int X, type_idu_drv *p_idu, unsigned char send_state);
typedef void (*X_p_udf_rcv_event)(int X, type_idu_drv *pdu, unsigned char line, unsigned char status);


/*---------------------------------------------------------------------------------------------*/
/* Prototypes */

unsigned char d_configure(int mau_type, unsigned long bit_rate, int lines_avail);
void d_trace_install(X_p_udf_send_confirm p_udf_send_confirm, X_p_udf_rcv_event p_udf_rcv_event);


/*=============================================================================================*/

#endif

