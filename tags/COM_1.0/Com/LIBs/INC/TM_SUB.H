#ifndef	TM_SUB_H
#define	TM_SUB_H

/************************************************************************/
/*  Asea Brown Boveri Research Center, CH-5405 Baden, Switzerland       */
/*  Copyright 1991                                                      */
/*                                                                      */
/*  NAME:                                                               */
/*    TM_SUB.H                                                          */
/*    TM = Transport layer interface for Messages                       */
/*                                                                      */
/*  ABSTRACT:                                                           */
/*    TCN Message Transport Protocol, Transport sublayer.                  */
/*    This is an internal interface of the transport layer which was    */
/*    introduced to separate the messenger part from the application    */
/*    interface (code that is called from application or timeout task). */
/*                                                                      */
/************************************************************************/
/*                                                                      */
/*  AUTHOR  :   24-Sep-1992     CRBC1   Hoepli                          */
/*  MODIFIED:   14-Jun-1994     CRBC1   Hoepli       HUGE               */
/*  MODIFIED:   13-Feb-1995     BATL    Flum         Switch O_SUN       */
/************************************************************************/

#include "nm_layer.h"

#include "md_layer.h"


/* CONST ------------------------------------------------------------------ */

#if defined (O_EKR) || defined (O_PC)
#define HUGE      huge
#else
#define HUGE
#endif


#define	SEQ_NR_SIZE	8

/* TYPES ------------------------------------------------------------------- */

typedef	struct {
MD_PACKET *		pack[SEQ_NR_SIZE];  /* static packets for send conn */
unsigned char		size;		/* agreed packet size */
unsigned char		last_size;	/* of last packet in active window */
unsigned int		send_not_yet;	/* upper window edge */
unsigned int            next_send;      /* not sent but allowed to be sent */
} SEND_INFO;

enum  CONN_STATE { DISC, SETUP, SEND, LISTEN, RECEIVE, PEND_ACK, FROZEN,
		   LISTEN_FROZEN, RCV_CANC, SEND_CANC, CLOSED };

typedef	struct CONN_DESCR {
MD_LIST_HEADER		list;	/* link in 'timedOutConn' */
int			timer_running, timed_out;
short    		timer;
int			type;
AM_RESULT		error;
unsigned int		credit;
int			eot;
int                     upd_pend; /* packets must be updated for this conn */
CARDINAL16              conn_run_nr;    /* big-endian */
unsigned char           credit_and_pack_size; /* proposed, for rcv conn only */
TM_MSG_DESCR *		msg;	/* static for rcv, dynamic for send conn */
enum CONN_STATE		state;
SET8			my_sap;	/* channel field in mtc */
unsigned int		expected;	/* ack or data, lower window edge */
union {
	unsigned int	neu;		/* unacknowledged received */
	unsigned int	rep;		/* repetitions */
	} cnt;
unsigned char HUGE *	msg_pos;
union {
	MD_PACKET *	reply;		/* static packet for rcv conn */
	SEND_INFO *	send_ext;	/* static sender extension */
	} ref;
struct CONN_DESCR *	next;		/* link in open or idle connections */
} CONN_DESCR;

/* FUNCTIONS: -------------------------------------------------------------- */

void pend_mutex (void);
void post_mutex (void);
void signal_tmo (void * param);
AM_RESULT default_put (void);
void default_deliver (void);

/* VARIABLES: -------------------------------------------------------------- */

extern unsigned int my_credit;

extern MD_LIST_HEADER * timed_out_conn;	/* elements: CONN_DESCR */
/* list of timedout connections */

extern MD_LIST_HEADER * send_msg[TM_USER_TYPE_SIZE]; /* elements: TM_MSG_DESCR */
extern MD_LIST_HEADER * idle_msg;		     /* elements: TM_MSG_DESCR */
/* message descriptors are allocated when necessary, 'msg_que_size' is the sum
   of all message descriptors in 'send_msg' and 'idle_msg' lists together. */

extern int halt_condition;
/* initialization result, value != 0 forces a call to 'hamster' in
   'tm_messenger'. 'hamster' cannot be called during initialization! 
*/

extern short cancel_mutex;

extern short delay_only;


#endif
