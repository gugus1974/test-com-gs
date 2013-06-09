/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> The freed connection is put at the end of the list to avoid its early re-usage      */
/* <ATR:02> fixup to avoid C166 bug                                                             */
/*==============================================================================================*/

/************************************************************************/
/*  Asea Brown Boveri Research Center, CH-5405 Baden, Switzerland       */
/*  Copyright 1991                                                      */
/*                                                                      */
/*  NAME:                                                               */
/*    TM_M_LAY.C                                                        */
/*                                                                      */
/*  ABSTRACT:                                                           */
/*    TCN Message Transport Protocol, Transport layer                   */
/*    Provides simplex connections between a caller and a replier.      */
/*    A LinkId together with the channel( SAP ) identify a connection.  */
/*    This file contains the messenger part of the transport layer.     */
/************************************************************************/
/*                                                                      */
/*  AUTHOR  :   29-Apr-1991     CRBC1   Hoepli                          */
/*  APPROVED:   <dd-mmm-yy>  <dep.>   <name>                            */
/*  MODIFIED:   <dd-mmm-yy>  <dep.>   <name>                            */
/*              <reason>                                                */
/*  MODIFIED:   1991.08.22   BAEI-3   Krzeslowski ( WHK )               */
/*              the module was originally written in Modula 2 and then  */
/*              translated to C                                         */
/*                                                                      */
/*       04-Feb-1992   CRBC1    Hoepli                                  */
/*       11-Mar-1992   CRBC1    Hoepli     NET_DELAY changed            */
/*       12-Mar-1992   CRBC1    Hoepli     DR reason passing            */
/*       17-Mar-1992   CRBC1    Hoepli     alignment in CR, CC          */
/*       22-Apr-1992   CRBC1    Hoepli     tm_deliver_xx added          */
/*       19-May-1992   CRBC1    Hoepli     TM_INVOKE_MSG_IND chg        */
/*       28-Jul-1992   CRBC1    Hoepli     run_time (limitation)        */
/*       24-Sep-1992   BAEI-2   Muenger    TM_I_LAY, TM_M_LAY           */
/*       03-May-1993   CRBC1    Hoepli     bug handle_rcv_pack          */
/*       21-Jun-1993   BATC     Muenger    hamster_continue             */
/*       21-Jun-1993   BATC     Muenger    MAX_SEND_CONN in sys.h       */
/*       04-Nov-1993   BATC     Muenger    variable pack timeout        */
/*       04-Nov-1993   CRBC1    Hoepli     tm_messenger                 */
/*      (check both send_msg queues for pending requests)               */
/*       11-Nov-1993   CRBC1    Hoepli   DR for own protocl only        */
/*       18-Nov-1993   CRBC1    Hoepli     #ifdef MULTICAST             */
/*       22-Dec-1993   CRBC1    Hoepli     changed for new nm           */
/*       01-Mar-1994   CRBC1    Hoepli  tm_define_ext_xx, PI_TICK       */
/*       07-Mar-1994   CRBC1    Hoepli     define DU_MODULE_NAME        */
/*       19-May-1994   BATC     Schomisch  Different ACK_TMO            */
/*                                          for MVB and WTB             */
/*       14-Jun-1994   CRBC1    Hoepli     msg_pos is now HUGE          */
/*       15-Jun-1994   CRBC1    Hoepli     alloc descr at init.         */
/*       22-Jun-1994   CRBC1    Hoepli     early descr. release         */
/*       04-Aug-1994   CRBC1    Hoepli     link_id: next station        */
/*       24-Jan-1995   BATL     Flum       add changes by               */
/*       10-Feb-1995   BATL     Flum       changed name of              */
/*                                         variables new, try           */
/*                                         to neu, tries                */
/*                                         (new,try=BC++keywords)       */
/*       13-Feb-1995   BATL     Flum       Switch O_SUN inserted        */
/*       15-Feb-1995   BATL     Flum       Switch O_MD_STDA to          */
/*                                         compile the source           */
/*                                         without the C standard       */
/*                                         lib                          */
/*       27-Fev-1995   BATL     Flum       MD_NET_PACKET cleared        */
/*                                         up                           */
/*       28-March-1995 BATL     Flum       inserted                     */
/*                                         SIZEOF_CR_PARAM              */
/*                                         SIZEOF_CC_PARAM              */
/*                                         SIZEOF_DR_PARAM              */
/*                                         bcause of alligment          */
/*                                         problems                     */
/*       16-Jun-1995   BATL    Flum        tm_abort_TB_conn,            */
/*                                         accept CR in all recv        */
/*                                         connection states            */
/*                                         ALIVE_TMO shortened          */
/*                                         for mess. with >=1 DT        */
/*                                                                      */
/*       13-Jul-1995 BATL      Flum       if rcv_DR(AM_INAUG_ERR)       */
/*                                        from VB, send	                */
/*                                        DR(AM_INAUG_ERR)              */
/*                                        and cancel all open           */
/*                                        connections which shall       */
/*                                        be transmitted over the       */
/*                                        TB                            */
/*       03-Aug-1995 BATL      Flum       inserted                      */
/*                                        tm_read_messenger_status      */
/*                                        and statistical counters      */
/*       09-Oct-1995 BATL      Flum       tm_write_messenger_control    */
/*                                        added                         */
/*       11-Oct-1995 BATL      Flum       MAX_CREDIT instead credit in  */
/*                                        procedure nm_init             */
/************************************************************************/


#define DU_MODULE_NAME "TM_M_LA"
/* for hamster calls which are now mapped to JDP event recorder.
   DU_MODULE_NAME must be defined before "du_sys.h" is included.
*/
                                                    
#define MULTICAST
/* enables connection of an external protocol, can be undefined for
   code optimization if the multicast protocol is not used.
*/


#ifdef TRACE_ENB
#include "traceenb.h"
#endif
/* enables tracing printout for tests */

#if defined (O_PC) || defined (O_SUN)
#include "stdlib.h"
#endif

#ifndef O_MD_STDA
#include <stdio.h>			/* printf( ) */
#include <string.h>                     /* memcpy    */
#endif

#include "pi_sys.h"
#include "md_ifce.h"
#include "am_sys.h"
#include "basetype.h"
#include "md_layer.h"
#include "nm_layer.h"
#include "mm_pool.h"
#include "tm_layer.h"
#include "tm_sub.h"

/* CONST: system parameters: ---------------------------------------------- */

/* system parameters, must be identical for all nodes: */
/* EH 01-Mar-1994, times in units [ms] instead of ticks */
#define	MAX_REP_CNT	2	/* number of retransmissions */
/* does not include first trial, number of trials = 'MAX_REP_CNT' + 1 */
#define	CONN_TMO_INC	0UL	 /* unit: [ms]; difference between
                                   SEND_TMO and CONN_TMO */
#define ACK_TMO_V_MS      (4*64UL) /* unit: [ms] */
#define ACK_TMO_T_MS      (4*64UL) /* unit: [ms] */
/* 93-11-04 Mr: separate timeout values for local and trainbus connections */
#define NET_DELAY_V     (8*64UL) /* unit: [ms] */
#define NET_DELAY_T     (16*64UL) /* unit: [ms] */
#define	PROC_TIME       (1*64UL) /* unit: [ms]; processing time */
#define PACK_LIFE_TIME (80*64UL) /* unit: [ms]; max. life-time of a packet */

/* The packet sizes are coded for negociation in the CR/CC packets.
   The sizes in bytes for code 0..15 must be increasing and are
   defined as follows: */
#define CODE_0_PACK_SIZE     27
#define CODE_1_PACK_SIZE     32
#define CODE_2_PACK_SIZE     38
#define CODE_3_PACK_SIZE     44
#define CODE_4_PACK_SIZE     50
#define CODE_5_PACK_SIZE     56
#define CODE_6_PACK_SIZE     62
#define CODE_7_PACK_SIZE     68
#define CODE_8_PACK_SIZE     74
#define CODE_9_PACK_SIZE     80
#define CODE_10_PACK_SIZE    87
#define CODE_11_PACK_SIZE    94
#define CODE_12_PACK_SIZE   101
#define CODE_13_PACK_SIZE   108
#define CODE_14_PACK_SIZE   115
#define CODE_15_PACK_SIZE   128
/* The message header is transmitted in the CR packet.
   The CR frame must not be longer than the shortest bus frame.
   This is asserted by 'tm_system_init' in debug mode. */

/* CONST: node parameters: ------------------------------------------------ */

/* node parameters: */

/* CONST ------------------------------------------------------------------ */

#define NR_OF_CODES          16

/* protocol definitions: */
/* 93-11-04 Mr: separate timeout values for local and trainbus connections */
/* EH 01-Mar-1994, times in units [ms] with identifier suffix "_MS": */
#define SEND_TMO_V_MS   ( 2 * ( NET_DELAY_V + PROC_TIME ) + ACK_TMO_V_MS )
#define CONN_TMO_V_MS   ( SEND_TMO_V_MS + CONN_TMO_INC )
#define SHRT_ALIVE_TMO_V_MS  (( MAX_REP_CNT + 1 ) * ( CONN_TMO_V_MS + PROC_TIME ))
#define LONG_ALIVE_TMO_V_MS  (( MAX_REP_CNT + 1 ) * ( CONN_TMO_V_MS + PROC_TIME ) \
                             + PACK_LIFE_TIME )

#define SEND_TMO_T_MS   ( 2 * ( NET_DELAY_T + PROC_TIME ) + ACK_TMO_T_MS )
#define CONN_TMO_T_MS   ( SEND_TMO_T_MS + CONN_TMO_INC )
#define SHRT_ALIVE_TMO_T_MS  (( MAX_REP_CNT + 1 ) * ( CONN_TMO_T_MS + PROC_TIME ))
#define LONG_ALIVE_TMO_T_MS  (( MAX_REP_CNT + 1 ) * ( CONN_TMO_T_MS + PROC_TIME ) \
                             + PACK_LIFE_TIME )


/* times in ticks, 1 tick is the unit for the PIL timer values [PI_TICK]: */
#define ACK_TMO_V     (ACK_TMO_V_MS / PI_TICK)    /* rounded to next lower */
#define ACK_TMO_T     (ACK_TMO_T_MS / PI_TICK)
/* the following times in ticks are rounded to the next higher value: */
#define SEND_TMO_V    ((SEND_TMO_V_MS  + PI_TICK - 1) / PI_TICK)
#define CONN_TMO_V    ((CONN_TMO_V_MS  + PI_TICK - 1) / PI_TICK)
#define SHRT_ALIVE_TMO_V ((SHRT_ALIVE_TMO_V_MS + PI_TICK - 1) / PI_TICK)
#define LONG_ALIVE_TMO_V ((LONG_ALIVE_TMO_V_MS + PI_TICK - 1) / PI_TICK)
#define SEND_TMO_T    ((SEND_TMO_T_MS  + PI_TICK - 1) / PI_TICK)
#define CONN_TMO_T    ((CONN_TMO_T_MS  + PI_TICK - 1) / PI_TICK)
#define SHRT_ALIVE_TMO_T ((SHRT_ALIVE_TMO_T_MS + PI_TICK - 1) / PI_TICK)
#define LONG_ALIVE_TMO_T ((LONG_ALIVE_TMO_T_MS + PI_TICK - 1) / PI_TICK)


#define MAX_CREDIT  7

MD_LINK_PACKET md_link_packet;

#define MY_PACK_SIZE    (sizeof(MD_LINK_PACKET) - sizeof(md_link_packet.version) - sizeof(md_link_packet.size))

#define PACK_HDR_SIZE   ( MY_PACK_SIZE - MD_DATA_SIZE )
/* size of the common packet header up to and including 'mtc' */

/* mtc: message transport control field: */
#define	CR		0		                         	/* Connect Request */
#define	CC		BIT0	                    		/* Connect Confirm */
#define	DR		BIT1		                    	/* Disconnect Request */
#define DC    ( BIT0 | BIT1 )               /* Disconnect Confirm */
#define	DT		BIT4	                      	/* Data, 16 .. 31: EOT + SeqNr */
#define	AK		BIT5	                      	/* Acknowledge, 32 .. 39: SeqNr */
#define	EOT		BIT3            	/* End Of Transmission bit for Data packets */
#define	MASK_SEQ_NR	( BIT0 | BIT1 | BIT2 )
#define MAX_CONTROL ( BIT0 | BIT1 )         /* Maximum of CR, CC, DR, DC */

#define	CALLER_BIT	BIT7
#define	RCV_BIT		BIT6

/* mask for channel field in mtc */
#define	MASK_SAP	( CALLER_BIT | RCV_BIT )
/* caller send channel */
#define	CSEND_SAP	CALLER_BIT
/* caller receive channel */
#define	CRCV_SAP	( CALLER_BIT | RCV_BIT )
/* replier send channel */
#define	RSEND_SAP	0
/* replier receive channel */
#define	RRCV_SAP	RCV_BIT

#define SIZEOF_DR_PARAM          1
#define SIZEOF_CC_PARAM          3
#define SIZEOF_CR_PARAM         (7 + TM_MSG_HDR_SIZE)


#define DR_PACK_SIZE		( PACK_HDR_SIZE + SIZEOF_DR_PARAM )
#define CC_PACK_SIZE		( PACK_HDR_SIZE + SIZEOF_CC_PARAM )
#define CR_MIN_PACK_SIZE	( PACK_HDR_SIZE + SIZEOF_CR_PARAM -\
				  TM_MSG_HDR_SIZE )

#define MAX_RUN_TIME              10  /* limits 'tm_messenger' run-time */
#define SEND_RUN_TIME              1  /* approx. for 'send_data_pack' */
#define RCV_RUN_TIME               1  /* approx. for 'rcv_data' */
#define ACCEPT_RUN_TIME            2  /* approx. for 'new_conn' */
#define CLOSE_RUN_TIME             1  /* approx. for 'close_xxx' */

/* TYPES ------------------------------------------------------------------- */

#if defined (O_960)
#pragma noalign str_cr_param
#endif

typedef	struct {
TM_INVOKE_MSG_IND	invoke_notif;
TM_RCV_MSG_IND		data_notif;
TM_SEND_MSG_CONF	sent_notif;
} USER_DESCR;

typedef struct str_cr_param {
CARDINAL16              conn_run_nr;    /* big-endian */
CARDINAL32              msg_size;       /* big-endian */
unsigned char           credit_and_pack_size;
unsigned char           hdr_data[TM_MSG_HDR_SIZE];
} CR_PARAM;  /* parameters for CR = Connect Request packet */

typedef	struct {
CARDINAL16              conn_run_nr;    /* big-endian */
unsigned char		credit_and_pack_size;
} CC_PARAM;  /* parameters for CC = Connect Confirm packet */
                  
typedef	struct {
AM_RESULT		reason;
} DR_PARAM;  /* parameters for DR = Disconnect Request packet */

typedef union {
CR_PARAM                cr_par;
CC_PARAM                cc_par;
DR_PARAM                dr_par;
} PACK_PARAM;

/* DC and AK packets have no parameters, DT packets contain data. */

/* FUNCTIONS --------------------------------------------------------------- */

static CONN_DESCR * new_rcv_descr (void);  /* EH, 15-Jun-1994 */
static CONN_DESCR * new_send_descr (void);  /* " */

static void restart_tmo( CONN_DESCR * conn, unsigned int time );
static void reset_tmo( CONN_DESCR * conn );
static void get_tmo_event( CONN_DESCR ** conn );

#ifdef TRACE_ENB

extern unsigned long total_msg_cnt, total_err_cnt;
extern AM_RESULT     last_err;

static void print_status( void );
static void trace( int caller, SET8 mtc, int eot, int nr );
static void trace_tmo( int caller, int state, unsigned int rep_cnt );

extern int trace_enable;

static /*unsigned*/ char pb[128];

#endif

static void free_conn( int is_sender, CONN_DESCR * conn );
static void find_conn( SET8 sap, const TM_LINK_ID * link,
		       CONN_DESCR * * conn );
static void close_send( CONN_DESCR * conn, AM_RESULT err );
static void close_rcv( CONN_DESCR * conn, AM_RESULT err );
static int  send_data_pack( CONN_DESCR * conn, int ix );
static void reply_disc_requ( AM_RESULT err );
static void reply_disc_conf( void );
static void send_ack( CONN_DESCR * conn );
static void request_ack( CONN_DESCR * conn );
static void rcv_data( void );
static void send_data_or_cancel( CONN_DESCR * conn );
static void handle_rcv_pack( void );
static unsigned char get_CR_data (unsigned int hdr_len);
static void new_conn( void );
static void repeat( CONN_DESCR * conn );
static void start_send( int who, TM_MSG_DESCR * m );
static void handle_tmo( CONN_DESCR * conn );
static void send_CR (CONN_DESCR * conn);
static void send_CC (CONN_DESCR * conn);
static void send_DR (MD_PACKET * packet, CONN_DESCR * conn);
static unsigned int encode_pack_size (unsigned int pack_size);
static unsigned int decode_pack_size (unsigned int code);
static void tm_abort_TB_conn (void);


/* EXTERNAL VARIABLES: ----------------------------------------------------- */

unsigned int my_credit;		/* my proposed, maximum credit */

MD_LIST_HEADER * timed_out_conn;	/* elements: CONN_DESCR */
/* list of timedout connections */

MD_LIST_HEADER * send_msg[TM_USER_TYPE_SIZE]; /* elements: TM_MSG_DESCR */
MD_LIST_HEADER * idle_msg;		     /* elements: TM_MSG_DESCR */
/* message descriptors are allocated when necessary, 'msg_que_size' is the sum
   of all message descriptors in 'send_msg' and 'idle_msg' lists together. */

int halt_condition;
/* initialization result, value != 0 forces a call to 'hamster' in
   'tm_messenger'. 'hamster' cannot be called during initialization! 
*/

/* VARIABLES: -------------------------------------------------------------- */

static unsigned char tm_packet_size = 0; /* for network management */

static unsigned long tm_packets_sent = 0;    /* counting the number of packets
					       sent to MVB or WTB */
static unsigned long tm_packet_retries = 0; /* counting the number of retried
					       packets in the single cast proto */

static unsigned long tm_multicast_retries = 0; /* counting the number of retried
					      packets in the multicast
					      protocol */

static unsigned int tm_multicast_window = 0; /* size of the multicast window */

static unsigned int run_time;
/* run-time approximation to limit the number of received packets in the
   current call of 'tm_messenger'. 
*/

static void * locked_pool;
/* locked pool for transmission of packets; each send connection has
   'SEQ_NR_SIZE' packets and each receive connection has 1 packet.
   The packets belong to the connection and are attached to the
   connection descriptor once. */

static const unsigned char code_table[NR_OF_CODES] =
{
    CODE_0_PACK_SIZE,
    CODE_1_PACK_SIZE,
    CODE_2_PACK_SIZE,
    CODE_3_PACK_SIZE,
    CODE_4_PACK_SIZE,
    CODE_5_PACK_SIZE,
    CODE_6_PACK_SIZE,
    CODE_7_PACK_SIZE,
    CODE_8_PACK_SIZE,
    CODE_9_PACK_SIZE,
    CODE_10_PACK_SIZE,
    CODE_11_PACK_SIZE,
    CODE_12_PACK_SIZE,
    CODE_13_PACK_SIZE,
    CODE_14_PACK_SIZE,
    CODE_15_PACK_SIZE
};

/* 93-11-04 Mr: separate timeout values for local and trainbus connections */
#define NB_BUSSES       2   /* constants for vehicle bus and train bus     */
#define NB_TIMEOUTS     5   /* number of timeout consts that depend on bus */

#define SEND_TMO_INDEX  0
#define CONN_TMO_INDEX  1
#define SHRT_ALIVE_TMO_INDEX 2
#define LONG_ALIVE_TMO_INDEX 3
#define ACK_TMO_INDEX   4
#define TMO_MVB_INDEX   0
#define TMO_WTB_INDEX   1

/* EH 01-Mar-1994, type for 'time_table' changed from short to long */
static unsigned long time_table [NB_BUSSES] [NB_TIMEOUTS] =
{
    {       /* constants for vehicle bus */
        SEND_TMO_V,
        CONN_TMO_V,
	SHRT_ALIVE_TMO_V,
	LONG_ALIVE_TMO_V,
	ACK_TMO_V
    },
    {       /* constants for train bus */
        SEND_TMO_T,
        CONN_TMO_T,
	SHRT_ALIVE_TMO_T,
	LONG_ALIVE_TMO_T,
	ACK_TMO_T
    }
};


static unsigned int   my_pack_size_code;
static CARDINAL16     conn_run_nr; /* local-endian */

/* variables for handling of a received packet: */
static CONN_DESCR *   rcv_conn;
static MD_PACKET *    rcv_packet;
static unsigned int   rcv_size;
static TM_LINK_ID     rcv_link;
static TM_LINK_ID     rcv_dest;
static SET8           rcv_mtc, rcv_sap;
static int            put_to_pool, rcv_eot;
static unsigned int   rcv_seq_nr;
static PACK_PARAM *   rcv_par_ptr;

static TM_LINK_ID     my_full_addr;
/* This variable may be used as source for calls to 'nm_send_pack'.
   The field 'my_full_addr.link_dev' is initialized once to the constant
   'AM_SAME_DEVICE' and should never be changed. */

static unsigned int   msg_que_size;
/* maximal number of queued send messages that ever occured up to now */

static USER_DESCR users[TM_USER_TYPE_SIZE];

static CONN_DESCR * idle_rcv_conn,   /* free connection descriptors */
                  * idle_send_conn,
                  * open_rcv_conn,   /* open connections, state != 'DISC' */
                  * open_send_conn;
static unsigned int nr_of_send_conn; 
/* number of elements in 'open_send_conn' */
static unsigned int nr_of_rcv_conn;  
/* number of elements in 'open_rcv_conn' */

static int      send_reply; /* replier has priority for next send */
static int      fetch_next, tries;
static int      do_abort_TB_conn;

/* connected functions for external protocol: */
static TM_POLL_FCT ext_poll;
#ifdef MULTICAST
static NM_RCV_FCT ext_rcv_pack;
#endif

/* optional tracing: -------------------------------------------------------*/

#ifdef	TRACE_ENB

static void print_status( void )
{
static int cnt;

   if( ! ( ( cnt++ ) % 256 ) ) {
      sprintf( pb, "\nSTATUS: total msg = %lu, total err = %lu, last error = %d\n",
               total_msg_cnt, total_err_cnt, last_err );
      bprintf( pb );
   }
}


void trace( int caller, SET8 mtc, int eot, int nr )
{
  if( ! trace_enable )
     return;

  print_status( );

  sprintf( pb, "%d rcv> ", system_counter( 0 ) );
  bprintf( pb );

  if( caller != 0 ) {
    sprintf( pb, "C <-- R: " );
    bprintf( pb );
  } 
  else {
    sprintf( pb, "C --> R: " );
    bprintf( pb );
  }

  if( mtc == CR ) {
    sprintf( pb, "CR" );
    bprintf( pb );
  } 
  else if ( mtc == CC ) {
    sprintf( pb, "CC" );
    bprintf( pb );
  } 
  else if ( mtc == DR ) {
    sprintf( pb, "DR" );
    bprintf( pb );
  } 
  else if ( mtc == DC ) {
    sprintf( pb, "DC" );
    bprintf( pb );
  }
  else if ( mtc == DT ) {
    sprintf( pb, "DT, %5.5u", nr );
    bprintf( pb );
    if( eot != 0 ) {
       sprintf( pb, ", EOT" );
       bprintf( pb );
    }
  } 
  else if ( mtc == AK ) {
    sprintf( pb, "AK, %5.5u", nr );
    bprintf( pb );
  } /* if */
  sprintf( pb, "\r\n" );
  bprintf( pb );
} /* Trace */

/**********************************************************************/

void trace_tmo( int caller, int state, unsigned int rep_cnt )
{
int valid;

  valid = 0;
  sprintf( pb, "%d TMO> ", system_counter( 0 ) );
  bprintf( pb );
  if( caller != 0 ) {
    sprintf( pb, "C: " );
    bprintf( pb );
  } 
  else {
    sprintf( pb, "R: " );
    bprintf( pb );
  }
  switch( state ) {
  case SETUP :
    sprintf( pb, "setup " );
    valid = 1;
    break;
  case SEND :
    sprintf( pb, "send " );
    valid = 1;
    break;
  case LISTEN :
    sprintf( pb, "listen " );
    break;
  case LISTEN_FROZEN :
    sprintf( pb, "listen_frozen " );
    break;
  case RECEIVE :
    sprintf( pb, "receive " );
    break;
  case PEND_ACK :
    sprintf( pb, "pendAck " );
    break;
  case FROZEN:
    sprintf( pb, "frozen " );
    break;
  case RCV_CANC :
    sprintf( pb, "rcvCanc " );
    valid = 1;
    break;
  case SEND_CANC :
    sprintf( pb, "sendCanc " );
    valid = 1;
    break;
  case CLOSED :
    sprintf( pb, "closed " );
    break;
  default :
    sprintf( pb, "invalid option " );
    break;
  } /* switch( state ) */
  bprintf( pb );
  if( valid != 0 ) {
    sprintf( pb, "%3.3u", rep_cnt );
    bprintf( pb );
  }
  sprintf( pb, "\r\n" );
  bprintf( pb );
} /* trace_tmo */
#endif          

/* shortcut: --------------------------------------------------------------*/

void tm_conn_get_call  ( TM_GET_MSG_IND get_ind, 
                         TM_DELIVER_MSG_IND deliver_ind )
{
    tm_put_call = get_ind;
    tm_deliver_call = deliver_ind;
} /* tm_conn_get_call */

/**********************************************************************/

void tm_conn_get_reply ( TM_GET_MSG_IND get_ind,
                         TM_DELIVER_MSG_IND deliver_ind )
{
    tm_put_reply = get_ind;
    tm_deliver_reply = deliver_ind;
} /* tm_conn_get_reply */

/* timeout: ---------------------------------------------------------------*/

void restart_tmo( CONN_DESCR * conn, unsigned int time_index )
/* called from the 'tm_messenger' process */
{
#ifdef MD_DEBUG
	if (time_index == 0)
		printf(" start send_tmo  ");
	if (time_index == 1)
		printf(" start conn_tmo  ");
	if (time_index == 2)
		printf(" start shrt_alive_tmo  ");
	if (time_index == 3)
		printf(" start long_alive_tom  ");
	if (time_index == 4)
		printf(" start ack_tmo  ");
#endif

  if ( conn->timer_running != 0 )
  {
    pi_disable_timeout (conn->timer);
  }
  if ( conn->timed_out != 0 )
  {
    md_cancel_ele( (MD_LIST_HEADER *) conn );
    conn->timed_out = 0;
  } /* if */
  conn->timer_running = 1;

  pi_enable_timeout (conn->timer,
    time_table [((conn->msg->link.vehicle & ~AM_PHYS_ADDR) ==
		  AM_SAME_VEHICLE) ? 0 : 1]
	       [time_index]);
} /* restart_tmo */

/**********************************************************************/

void reset_tmo( CONN_DESCR * conn )
/* called from the 'tm_messenger' process */
{
  if ( conn->timer_running != 0 )
  {
    pi_disable_timeout (conn->timer);
    conn->timer_running = 0;
  } /* if */
  if ( conn->timed_out != 0 )
  {
    md_cancel_ele( (MD_LIST_HEADER *) conn );
    conn->timed_out = 0;
  } /* if */;
} /* reset_tmo */

/**********************************************************************/

void get_tmo_event( CONN_DESCR ** conn )
/* called from the 'tm_messenger' process */
{
  md_get_ele( timed_out_conn, (MD_LIST_HEADER * *)conn );
  if(*conn != NULL )
  {
    (*conn)->timed_out = 0;
  }
} /* get_tmo_event */


/* over bus: --------------------------------------------------------------*/
/* exported: **********************************************************/

void tm_define_user( int who, TM_INVOKE_MSG_IND listen,
                     TM_RCV_MSG_IND rcv, TM_SEND_MSG_CONF done)
{
  users[who].invoke_notif = listen;
  users[who].data_notif = rcv;
  users[who].sent_notif = done;
} /* tm_define_user */

/**********************************************************************/

void tm_abort_TB_conn (void)
{
	do_abort_TB_conn = !0;
}
/**********************************************************************/

void tm_init ( unsigned int credit, int max_conn )
{
  if (credit > MAX_CREDIT) 
  {
    credit = MAX_CREDIT;
  }
  else if (credit < 1)
  {
    credit = 1;
	}
	/* (FL) MAX_CREDIT instead of credit because my_credit may be changed
		 dynamical by the network management.
		 So the recources must be reserved for the max. possible credit    */
	nm_init (MAX_CREDIT * max_conn, tm_abort_TB_conn);
  my_credit = credit;
  mm_pool_create (max_conn + (SEQ_NR_SIZE * AM_MAX_SEND_CONN),
		  sizeof (MD_PACKET), !0, &locked_pool);
  { /* EH 15-Jun-1994, create all receive and send connection resources: */
  short ix;
  CONN_DESCR *conn;

    for (ix = 0; ix < max_conn; ix++) {
      conn = new_rcv_descr ();
      hamster_halt (conn == NULL);
      conn->next = idle_rcv_conn;
      idle_rcv_conn = conn;
    } /* for */
    for (ix = 0; ix < AM_MAX_SEND_CONN; ix++) {
      conn = new_send_descr ();
      if (conn != NULL) {
        conn->next = idle_send_conn;
        idle_send_conn = conn;
      } else {
        hamster_continue (!0);
        break;
      } /* if (conn != NULL) */
    } /* for */
    hamster_halt (idle_send_conn == NULL);
  }
#ifdef TRACE_ENB
  trace_enb_init ();
#endif
} /* tm_init */

/* internal: **********************************************************/

static CONN_DESCR * new_rcv_descr (void)  /* added EH, 15-Jun-1994 */
/* creates a new receive connection descriptor with all its attached 
   elements, which are a message descriptor, a timer and one packet.
*/
{
CONN_DESCR * conn;
short        pi_result;

  conn = (CONN_DESCR *)pi_alloc( sizeof( CONN_DESCR ) ); 
  if( conn != NULL )
  {
    md_init_ele( (MD_LIST_HEADER *) conn );
    conn->timed_out = 0;
    conn->timer_running = 0;
    conn->msg = (TM_MSG_DESCR *)pi_alloc( sizeof( TM_MSG_DESCR ) );
    if( conn->msg == NULL )
    {
      pi_free( conn );
      conn = NULL;
    }
  } /* if */
  if( conn != NULL )
  {
    pi_result  = pi_create_timeout (&conn->timer, signal_tmo, conn, 1);
    if (pi_result == PI_RET_OK)
    {
      mm_pool_get_pack (&locked_pool, &conn->ref.reply);
      if (conn->ref.reply != NULL)
      {
        conn->ref.reply->status = MD_SENT;
      }
      else
      {
	pi_delete_timeout (conn->timer);
        pi_result = ~PI_RET_OK;
      }
    }
    if (pi_result != PI_RET_OK)
    {
      pi_free (conn->msg);
      pi_free (conn);
      conn = NULL;
    } /* if */
  } /* if */
  return conn;
} /* new_rcv_desrc */

/**********************************************************************/

static CONN_DESCR * new_send_descr (void)  /* added EH, 15-Jun-1994 */
/* creates a new send connection descriptor with all its attached 
   elements, which are a send extension descriptor with 8 packets and
   a timer.
*/
{
CONN_DESCR * conn;
MD_PACKET ** my_packets;
short        ix;
short        pi_result;

  conn = (CONN_DESCR *)pi_alloc( sizeof( CONN_DESCR ) );
  if( conn != NULL )
  {
    md_init_ele( (MD_LIST_HEADER *) conn );
    conn->timed_out = 0;
    conn->timer_running = 0;
    conn->ref.send_ext = (SEND_INFO *)pi_alloc( sizeof( SEND_INFO ) );
    if( conn->ref.send_ext != NULL )
    {
      pi_result = pi_create_timeout (&conn->timer, signal_tmo, conn, 1);
      if (pi_result == PI_RET_OK)
      {
        my_packets = conn->ref.send_ext->pack;
        for (ix = 0; ix < SEQ_NR_SIZE; ix++)
        {
          mm_pool_get_pack (&locked_pool, &my_packets[ix]);
          if (my_packets[ix] != NULL)
          {
            my_packets[ix]->status = MD_SENT;
          }
          else
          {
            /* ??? !!! ??? some packets are lost, they should be freed */
            pi_delete_timeout (conn->timer);
	    pi_result = ~PI_RET_OK;
            break;
          } /* if */
        } /* for */
      } /* if */
      if (pi_result != PI_RET_OK)
      {
	pi_free (conn->ref.send_ext);
        conn->ref.send_ext = NULL;
      }
    } /* if */
    if( conn->ref.send_ext == NULL )
    {
      pi_free( conn );
      conn = NULL;
    } /* if */
  } /* if */
  return conn;
} /* new_send_descr */

/**********************************************************************/

void free_conn( int is_sender, CONN_DESCR * conn )
/* recycles the connection descriptor, and for send connections also
   the message descriptor. */
{
CONN_DESCR * * root, * * idle;

  if( is_sender != 0 )
  {
    md_put_ele( idle_msg, (MD_LIST_HEADER *) (conn->msg) );
    root = &open_send_conn;
    idle = &idle_send_conn;
    nr_of_send_conn--;
  } 
  else 
  {
    root = &open_rcv_conn; 
    idle = &idle_rcv_conn;
    nr_of_rcv_conn--;
  } /* if */
  
/* remove from root: */
  while ( *root != conn )
  {
    hamster_halt (*root == NULL);
    root = &( (*root)->next );
  } /* while */
  *root = conn->next;
  
/* insert in idle: */
/* <ATR:01> */
//  conn->next = *idle;
//  *idle = conn;
{ /* WARNING: the following code should be optimized */
	CONN_DESCR *p = *idle;

	conn->next = NULL;
	if (p) {
		while (p->next) p = p->next;
		p->next = conn;
	}
	else *idle = conn;
}

} /* free_conn */

/**********************************************************************/

void find_conn( SET8 sap, const TM_LINK_ID * link, CONN_DESCR * * conn )
/* finds the open connection which belongs to the specified 'link' and
   'sap'. */
{
int me;

  me = TM_REPLIER_USER;
  if( CALLER_BIT & sap )
  {
    me = TM_CALLER_USER;
  }
  if( ! ( RCV_BIT & sap ) )
  { 
    *conn = open_send_conn;
  } 
  else
  {
    *conn = open_rcv_conn;
  } /* if */
/* !!! The following comparison is not portable, but asserted by
   'tm_system_init' in debug mode. */
  while( *conn != NULL )
  {
    if( (*conn)->type == me &&
	*(CARDINAL32 *)link == *(CARDINAL32 *)&((*conn)->msg->link) )
    {
      return;
    } 
    else
    {
      (*conn) = (*conn)->next;
    }
  } /* while */
} /* find_conn */

/**********************************************************************/
static TM_MSG_DESCR * next_msg;
static SET8           next_sap;
static int            sender;

void tm_messenger( void )
/* messenger is the protocol handler that runs as the process */
/* defined as the cyclic task */
{
CONN_DESCR *          tmp_conn;
AM_RESULT             rcv_error;
unsigned char         nr_empty_que;    /* EH, 04-Nov-1993 */
SET8                  tmp;
unsigned char         own_protocol;    /* 11-Nov-1993 */
TM_POLL_FCT           cur_poll;        /* EH 28-Feb-1994 */

#ifdef MULTICAST    /* EH 28-Feb-1994 */
NM_RCV_FCT            cur_rcv_pack;
#endif

#ifdef TRACE_ENB
static int xcnt;

   if( ( xcnt++ ) & 0x1 )
      led_on( );
   else
      led_off( );
#endif

    hamster_halt (halt_condition);

/* start next send connection if possible: */
    if( fetch_next != 0 )
    {
      tries = 0;
      if( idle_send_conn != NULL )
      {
        for (nr_empty_que = 0; nr_empty_que < TM_USER_TYPE_SIZE; )
        { /* for loop added to check both queues, EH 4-Nov-1993 */
          if( send_reply != 0 ) {
            sender = TM_REPLIER_USER;
            next_sap = RSEND_SAP;
          } 
          else
          { 
            sender = TM_CALLER_USER;
            next_sap = CSEND_SAP;
          }
          if (nr_empty_que == 0)
          {                              /* EH 4-Nov-1993 */
            send_reply = ! send_reply;
          }
          md_get_ele( send_msg[sender], (MD_LIST_HEADER * *)&next_msg );
          if( next_msg != NULL )
          {
            if( next_msg->cancelled )
            { 
	      md_put_ele( idle_msg, (MD_LIST_HEADER *) next_msg );
            } 
            else
            {
	      tries = 1;
              break;   /* EH 4-Nov-1993 */
            } /* if */
          } else {
            nr_empty_que++;   /* EH 4-Nov-1993 */
          } /* if */
        } /* for */    /* EH 4-Nov-1993 */
      } /* if */
    } /* if */
    if( tries )
    {
      /* wait if there is already such a connection! */
      find_conn( next_sap, &(next_msg->link), &tmp_conn );
      if( tmp_conn != NULL )
      {
        fetch_next = 0;
	tries = 0;
      } 
      else
      { 
        nr_of_send_conn++;
        start_send( sender, next_msg );
        fetch_next = 1;
      } /* if */
    } /* if */

/* handle received packets until a limited run-time is consumed: */
    run_time = 0;
#ifdef MULTICAST
    pi_disable ();
    cur_rcv_pack = ext_rcv_pack;
    pi_enable ();
    while( cur_rcv_pack( &rcv_packet, &rcv_size, &(rcv_link),
			 &rcv_dest, &rcv_error ) )
#else
    while( nm_rcv_pack( &rcv_packet, &rcv_size, &(rcv_link),
                        &rcv_dest, &rcv_error ) )
#endif
    {
      rcv_par_ptr = (PACK_PARAM *) rcv_packet->port.lk.data;
      put_to_pool = !0;
      rcv_link.my_fct_or_dev = rcv_dest.fct_or_dev;
      rcv_mtc = rcv_packet->port.lk.mtc;
			rcv_sap = ( rcv_mtc & MASK_SAP ) ^ MASK_SAP;
			rcv_mtc = rcv_mtc & ~MASK_SAP;
#ifdef MD_DEBUG
	if (rcv_mtc == CR)
		printf("\nCR :  ");
	if (rcv_mtc == CC)
		printf("\nCC :  ");
	if (rcv_mtc == DR)
		printf("\nDR :  ");
	if (rcv_mtc == DC)
		printf("\nDC :  ");
	if (rcv_mtc == DT)
		printf("\nDT :  ");
	if (rcv_mtc == AK)
		printf("\nACK : ");
#endif
			/* 11-Nov-1993, next lines moved from handle_rcv_pack */
			own_protocol = !0;
			rcv_seq_nr = rcv_mtc & MASK_SEQ_NR;
      rcv_eot = EOT & rcv_mtc;
      tmp = (rcv_mtc & ~MASK_SEQ_NR);
      if( (tmp & ~EOT) == DT )
      {
        rcv_mtc = DT;
      }
      else if ( tmp == AK )
      {
        rcv_mtc = AK;
      }
      else if (tmp > MAX_CONTROL)
      {
        own_protocol = 0;
      }
      if (!own_protocol) {}
      else if( rcv_error != AM_OK )
      {
	if ((rcv_error == AM_INAUG_ERR) && (rcv_mtc == DR)) /* FL */
	{                                                   /* FL */
	  reply_disc_requ (AM_INAUG_ERR);                   /* FL */
	  tm_abort_TB_conn();                               /* FL */
	}                                                   /* FL */
	else if ((rcv_mtc != DC) && (rcv_mtc != DR))
        {
          reply_disc_requ (rcv_error);
        }
      }
      else
      {
        /* 22-Dec-1993, EH: link_dev != next_hop for phys. addr. at caller */
        /* EH 04-Aug-1994: exception removed due to next_sta in AM_ADDRESS! 
	if ((rcv_link.vehicle
            == (AM_PHYS_ADDR | AM_SAME_VEHICLE)) && (rcv_sap & CALLER_BIT))
        {
	  rcv_link.link_dev = rcv_link.fct_or_dev;
        }
        */
        find_conn( rcv_sap, &rcv_link, &rcv_conn );
        if( rcv_conn != NULL )
        {
          handle_rcv_pack ();
        } 
        else 
        {
          new_conn ();
        } /* if */
      } /* if */
      if (put_to_pool)
      {
	mm_pool_put_pack (rcv_packet);
      } /* if */
      if (run_time > MAX_RUN_TIME)
      {
        break;
      }
    } /* while */

/* handle all timeouts: */
    do
    {
      get_tmo_event( &tmp_conn );
      if( tmp_conn != NULL ) 
      {
        handle_tmo( tmp_conn );
      }
    } while( tmp_conn != NULL );

    pi_disable ();
    cur_poll = ext_poll;
    pi_enable ();
    cur_poll ();
    if (do_abort_TB_conn)
    {
      /* close TB send conn */
      tmp_conn = open_send_conn;
      while( tmp_conn != NULL )
      {
	if((tmp_conn->msg->link.vehicle & ~AM_PHYS_ADDR)
						!= AM_SAME_VEHICLE)
	{
		/* TB connection -> close it */
		close_send( tmp_conn, AM_INAUG_ERR );
		/* open_send_conn has been changed by close_send */
		tmp_conn = open_send_conn; /* restart at beginning */
	} /* if */
	else
	{ /* VB connection -> don't touch it */
	  tmp_conn = tmp_conn -> next;
	} /* else */
      } /* while */
      do_abort_TB_conn = 0;
   } /* if */
} /* tm_messenger */

/**********************************************************************/

void close_send( CONN_DESCR * conn, AM_RESULT err )
/* Notifies send completion, ensures that no timeout is running for
   this connection and frees the connection */
{
unsigned char  cancelled;

   pend_mutex();
   conn->msg->cancel_enb = 0;
   cancelled = conn->msg->cancelled;
   post_mutex ();
   if (!cancelled)
   {
     run_time += CLOSE_RUN_TIME;
     users[conn->type].sent_notif
       ( conn->msg->param, err, conn->msg->my_vehicle );
   }
   reset_tmo( conn );
   free_conn( 1, conn );
   tries = 1;
} /* close_send */

/**********************************************************************/

void close_rcv( CONN_DESCR * conn, AM_RESULT err )
/* Receive completion: issues a data indication. Enters state 'frozen',
   or 'listen_frozen' resp. when currently in state 'listen', in case
   of normal completion( in which case the connection remains open
   to repeat an AK ), or frees the connection immediately after an error.
*/
{
unsigned char  cancelled;
unsigned char  release = 0;     /* added EH 22-Jun-1994 */

   pend_mutex();
   conn->msg->cancel_enb = 0;
   cancelled = conn->msg->cancelled;
   post_mutex ();
   if (!cancelled)
   {
     run_time += CLOSE_RUN_TIME;
     users[conn->type].data_notif( conn->msg->param, err, &release );
   }
   if (( err != AM_OK ) || release )
   {
      reset_tmo( conn );
      free_conn( 0, conn );
   } 
   else
   {
     if (conn->state == LISTEN)
     {
       conn->state = LISTEN_FROZEN;
       restart_tmo( conn, LONG_ALIVE_TMO_INDEX );
     }
     else
     {
       conn->state = FROZEN;
       restart_tmo( conn, SHRT_ALIVE_TMO_INDEX );
     }
   } /* if */
} /* close_rcv */

/**********************************************************************/

int send_data_pack( CONN_DESCR * conn, int ix )
/* sends 1 packet[ix] containing next data from the message.
   'eot', 'last_size' and 'error' in the connection descriptor are updated. 
   No action if the packet is pending. Returns 'MD_OK' or 'MD_PENDING'. */
{
MD_PACKET *       net_pack;
unsigned int      copy_size;

  net_pack = conn->ref.send_ext->pack[ix];
  if (net_pack->status != MD_PENDING)
  {
    run_time += SEND_RUN_TIME;
    net_pack->port.lk.mtc = conn->my_sap | DT | ix;
    conn->ref.send_ext->last_size = conn->ref.send_ext->size;
    copy_size = conn->ref.send_ext->last_size - PACK_HDR_SIZE;
    if ( conn->msg->msg_len <= (CARDINAL32) copy_size )
    {
      conn->eot = 1;
      net_pack->port.lk.mtc |= EOT;
      copy_size = (unsigned int) conn->msg->msg_len;
      conn->ref.send_ext->last_size = 
        (unsigned char)( copy_size + PACK_HDR_SIZE );
    } /* if */
    if (conn->msg_pos != NULL)
    {
      pi_copy8( (void *)net_pack->port.lk.data, (void *)conn->msg_pos,
		(short) copy_size );
      conn->msg_pos += copy_size;
    } /* if */
    conn->msg->msg_len -= copy_size;
    my_full_addr.vehicle = conn->msg->my_vehicle;
    my_full_addr.fct_or_dev = conn->msg->link.my_fct_or_dev;
    nm_send_pack( net_pack, conn->ref.send_ext->last_size, &my_full_addr,
		  &conn->msg->link, &conn->error );
    tm_packets_sent = ((tm_packets_sent+1) % 0xFFFFFFFFL);
    return MD_OK;
  }
  else
  {
    return MD_PENDING;
  }
} /* send_data_pack */

/**********************************************************************/

void reply_disc_requ( AM_RESULT err )
/* replies DR without using a connection; clears 'put_to_pool'.
   uses 'rcv_packet', reads 'rcv_link', 'rcv_par_ptr' and 'rcv_dest' */
{
  rcv_packet->port.lk.mtc
    = ((rcv_packet->port.lk.mtc & MASK_SAP) ^ MASK_SAP) | DR;
  rcv_par_ptr->dr_par.reason = err;
  nm_send_pack (rcv_packet, DR_PACK_SIZE, &rcv_dest,
		&rcv_link, &err );
  tm_packets_sent = ((tm_packets_sent+1) % 0xFFFFFFFFL);
  put_to_pool = 0;
} /* reply_disc_requ */

/**********************************************************************/

void reply_disc_conf( void )
/* replies DC without using a connection; clears 'put_to_pool'.
   uses 'rcv_packet', reads 'rcv_link' and 'rcv_dest' */
{
AM_RESULT err;

  rcv_packet->port.lk.mtc
    = ((rcv_packet->port.lk.mtc & MASK_SAP) ^ MASK_SAP) | DC;
  nm_send_pack (rcv_packet, PACK_HDR_SIZE, &rcv_dest,
		&rcv_link, &err );
  tm_packets_sent = ((tm_packets_sent+1) % 0xFFFFFFFFL);
  put_to_pool = 0;
} /* reply_disc_conf */

/**********************************************************************/

void send_ack( CONN_DESCR * conn )
/* uses the conn->ref.reply packet to send AK.
   No action if the packet is pending. */
{
  if (conn->ref.reply->status != MD_PENDING)
  {
    conn->ref.reply->port.lk.mtc = conn->my_sap | AK | conn->expected;
    my_full_addr.vehicle = conn->msg->my_vehicle;
    my_full_addr.fct_or_dev = conn->msg->link.my_fct_or_dev;
    nm_send_pack( conn->ref.reply, PACK_HDR_SIZE, &my_full_addr,
		  &conn->msg->link, &conn->error );
    tm_packets_sent = ((tm_packets_sent+1) % 0xFFFFFFFFL);
  }
} /* send_ack */

/**********************************************************************/

void request_ack (CONN_DESCR * conn )
/* Requests transmission of an AK by calling 'send_ack'.
   Updates the state, resets the neu count and starts a timeout. */
{
  send_ack( conn );
  conn->cnt.neu = 0;
  restart_tmo( conn, SHRT_ALIVE_TMO_INDEX );
  conn->state = RECEIVE;
} /* request_ack */

/**********************************************************************/

void rcv_data( void )
/* A destination in state 'RECEIVE' or 'PEND_ACK' receives a DT or 
   a DR packet. 
   Task switches must be disabled before testing 'cancelled' during
   write access to the message buffer. */
{
unsigned int data_size;

  run_time += RCV_RUN_TIME;
  if( rcv_mtc == DT )
  {
    pend_mutex();
    if (rcv_conn->msg->cancelled)
    {
      post_mutex ();
      send_DR (rcv_conn->ref.reply, rcv_conn);
      rcv_conn->cnt.rep = 0;
      restart_tmo (rcv_conn, SEND_TMO_INDEX);
      rcv_conn->state = RCV_CANC;
    }
    else
    {
      if( rcv_seq_nr == rcv_conn->expected )
      {
        data_size = rcv_size - PACK_HDR_SIZE;
        if (rcv_conn->msg_pos != NULL)
        {
	  pi_copy8 ((void *) rcv_conn->msg_pos, rcv_packet->port.lk.data,
		    (short) data_size );
          rcv_conn->msg_pos += data_size;
        } /* if */
        post_mutex ();
	rcv_conn->expected = (rcv_conn->expected + 1) % SEQ_NR_SIZE;
	rcv_conn->cnt.neu++;
        if( rcv_eot )
        {
          send_ack( rcv_conn );
	  close_rcv( rcv_conn, AM_OK );
        } 
	else if( rcv_conn->cnt.neu == rcv_conn->credit )
        {
          request_ack( rcv_conn );
        }
        else if( rcv_conn->state == RECEIVE )
        {
          restart_tmo( rcv_conn, ACK_TMO_INDEX );
	  rcv_conn->state = PEND_ACK;
        } /* if */
      } 
      else
      {
        post_mutex ();
        request_ack( rcv_conn );
      } /* if (rcv_seq_nr == rcv_conn->expected) */
    } /* if (rcv_conn->msg->cancelled) */
  }
  else if ( rcv_mtc == DR )
  {
    reply_disc_conf ();
    close_rcv( rcv_conn, rcv_par_ptr->dr_par.reason );
  } /* if (rcv_mtc == DT) */
} /* rcv_data */
    
/**********************************************************************/

void send_data_or_cancel( CONN_DESCR * conn )
/* A source sends as many DT packets as allowed (i.e. starting from
   'next_send' packet number up to but excluding 'send_not_yet')
   or {sends DR, resets 'cnt.rep' and restarts the timeout} if cancel 
   has been requested. 
   Task switches must be disabled before testing 'cancelled' during
   read access to the message buffer.
   Updates 'next_send' and 'state'. */
{
MD_PACKET **        my_packets;
unsigned int *      ix_ptr;
unsigned int        ix_not_yet;

  ix_ptr = &conn->ref.send_ext->next_send;
  my_packets = conn->ref.send_ext->pack;
  ix_not_yet = conn->ref.send_ext->send_not_yet;
  if (ix_not_yet == *ix_ptr)
  {
    return;
  }
  if (my_packets[*ix_ptr]->status != MD_PENDING)
  {
    pend_mutex();
    if (conn->msg->cancelled)
    {
      conn->state = SEND_CANC;
      send_DR (my_packets[0], conn);
      conn->cnt.rep = 0;
      restart_tmo( conn, SEND_TMO_INDEX );
    }
    else
    {
      for( ; ; )
      {
        if (send_data_pack( conn, *ix_ptr ) == MD_OK)
        {
          *ix_ptr = ( *ix_ptr + 1 ) % SEQ_NR_SIZE;
	  if ((*ix_ptr == ix_not_yet) ||  (conn->eot == !0))
          {
            break;
          } /* if */
        }
        else
        {
          break;
        } /* if */
      } /* for( ; ; ) */
    } /* if (conn->msg->cancelled) */
    post_mutex ();
  } /* if !(...) */
} /* send_data_or_cancel */

/**********************************************************************/

void handle_rcv_pack()
/* A packet has been received for an opened connection */
{
unsigned int        cp;     /* received 'credit_and_pack_size' */
int                 accept;

#ifdef TRACE_ENB
  if( trace_enable )
     trace( CALLER_BIT & rcv_conn->my_sap, rcv_mtc, rcv_eot, rcv_seq_nr );
#endif
  switch( rcv_conn->state )
  {
    case RECEIVE : case PEND_ACK :
      if( rcv_mtc == CR )                    /* CL 260595 */
      {
	close_rcv (rcv_conn, AM_FAILURE);
	rcv_conn = NULL;
	new_conn ();
      }
	else
	{
	  rcv_data();
	}
	break;
    case SEND :
      if( rcv_mtc == DR )
      {
        rcv_conn->error = rcv_par_ptr->dr_par.reason;
        reply_disc_conf ();
        rcv_conn->state = CLOSED;
	restart_tmo( rcv_conn, SHRT_ALIVE_TMO_INDEX );
      }
      else if ( rcv_mtc == AK )
      {
        if( rcv_conn->ref.send_ext->send_not_yet > rcv_conn->expected )
        {
          accept = rcv_seq_nr > rcv_conn->expected &&
                   rcv_seq_nr <= rcv_conn->ref.send_ext->send_not_yet;
        }
        else
        {
          accept = rcv_seq_nr > rcv_conn->expected ||
                   rcv_seq_nr <= rcv_conn->ref.send_ext->send_not_yet;
        } /* if */
        if( accept )
        {
          rcv_conn->expected = rcv_seq_nr;
	  rcv_conn->ref.send_ext->send_not_yet 
            = (rcv_seq_nr + rcv_conn->credit) % SEQ_NR_SIZE;
	  if (!rcv_conn->eot)
          {
            send_data_or_cancel( rcv_conn );
            if (rcv_conn->state == SEND)
            {
              rcv_conn->cnt.rep = 0;
              restart_tmo (rcv_conn, SEND_TMO_INDEX);
            }
          }
          else if (rcv_conn->expected == rcv_conn->ref.send_ext->next_send)
          {
            close_send (rcv_conn, AM_OK);
          }
        } /* if */
      } /* if */
      break;
    case LISTEN :
      if( rcv_mtc == CR )
      {
        if (rcv_conn->conn_run_nr == rcv_par_ptr->cr_par.conn_run_nr)
        {
          send_CC (rcv_conn);
          restart_tmo( rcv_conn, SHRT_ALIVE_TMO_INDEX );
        }
        else
        {
	  close_rcv (rcv_conn, AM_FAILURE);
	  rcv_conn = NULL;
	  new_conn ();                         /* CL 260595 */
	}
      }
      else
      {
        rcv_conn->state = RECEIVE;
        rcv_data();
      } /* if */
      break;
    case LISTEN_FROZEN :
      if( rcv_mtc == CR )
      {
        if (rcv_conn->conn_run_nr == rcv_par_ptr->cr_par.conn_run_nr)
        {
          send_CC (rcv_conn);
        }
        else
        {
          reset_tmo (rcv_conn);
          new_conn ();
        }
      } /* if */
      break;
    case SETUP :
      if( rcv_mtc == DR )
      { 
        close_send( rcv_conn, rcv_par_ptr->dr_par.reason );
      } 
      else if ( rcv_mtc == CC )
      {
        if (rcv_conn->conn_run_nr == rcv_par_ptr->cc_par.conn_run_nr)
	{
          /* added EH 22-Jun-1994: release rcv conn on same link if requ. */
          if (rcv_conn->msg->send_mode & TM_MODE_RELEASE)
          {
          CONN_DESCR *conn;

            find_conn (rcv_sap ^ RCV_BIT, &rcv_link, &conn);
            if (conn != NULL)
            {
              hamster_halt ((conn->state != FROZEN) 
                            && (conn->state != LISTEN_FROZEN));
              reset_tmo (conn);
              free_conn (0, conn);
            } /* if (conn != NULL) */
          } /* if (rcv_conn->msg->send_mode & TM_MODE_RELEASE) */
          /*  EH, 03-May-1993: next line was in if statement (bug)! */
	  rcv_conn->msg->my_vehicle = rcv_dest.vehicle;
          if (!rcv_conn->eot)
          {
						cp = rcv_par_ptr->cc_par.credit_and_pack_size;
						tm_packet_size = (unsigned char)cp;
						rcv_conn->credit = cp / BIT4;
						rcv_conn->ref.send_ext->next_send = 0;
						rcv_conn->ref.send_ext->send_not_yet = rcv_conn->credit;
						rcv_conn->ref.send_ext->size
							= (unsigned char) (decode_pack_size (cp % BIT4));
						rcv_conn->state = SEND;
						send_data_or_cancel( rcv_conn );
						rcv_conn->error = AM_OK;
						if (rcv_conn->state == SEND)
            {
              rcv_conn->cnt.rep = 0;
              restart_tmo (rcv_conn, SEND_TMO_INDEX);
            } /* if */
          }
          else
          {
            close_send (rcv_conn, AM_OK);
          } /* if (!rcv_conn->eot) */
        } /* if */
      } /* if */
      break;
    case FROZEN :
      if( rcv_mtc == DT )
      {
        send_ack( rcv_conn );
      } 
      else if ( rcv_mtc == CR )
      {
        reset_tmo( rcv_conn );
        new_conn();
      } /* if */
      break;
    case SEND_CANC :
      if( rcv_mtc == DC )
      {
        close_send( rcv_conn, AM_FAILURE );
      } 
      else if ( rcv_mtc == DR )
      {
        rcv_conn->state = CLOSED;
	restart_tmo( rcv_conn, SHRT_ALIVE_TMO_INDEX );
      } /* if */
      break;
    case RCV_CANC :
      if( rcv_mtc == DC || rcv_mtc == DR )
      {
	close_rcv( rcv_conn, AM_FAILURE );
       }
       else
       if ( rcv_mtc == CR )                   /* CL 260595 */
       {
       close_rcv (rcv_conn, AM_FAILURE);
       rcv_conn = NULL;
       new_conn ();
       }
       break;
    case CLOSED : /* no action */
      break;
    default:
      free_conn( ! (RCV_BIT & rcv_conn->my_sap ), rcv_conn );
      break;
  } /* switch */
} /* handle_rcv_pack */

/**********************************************************************/

unsigned char get_CR_data (unsigned int hdr_len)
/* copies the data in the CR packet into the header and message buffer.
   Returns 0 if the message is not yet completely received (eot).
*/
{
unsigned int data_size;

  data_size = rcv_size - CR_MIN_PACK_SIZE;
  if (hdr_len > data_size)
  {
    hdr_len = data_size;
  }
  rcv_conn->msg->hdr_len = hdr_len;
  data_size -= hdr_len;
  pend_mutex();
  if (!rcv_conn->msg->cancelled)
  {
    if (rcv_conn->msg->hdr_addr != NULL)
    {
      pi_copy8 (rcv_conn->msg->hdr_addr, rcv_par_ptr->cr_par.hdr_data,
                (short) hdr_len);
    }
    if (rcv_conn->msg_pos != NULL)
    {
      pi_copy8 ((void *) rcv_conn->msg_pos,
                rcv_par_ptr->cr_par.hdr_data + hdr_len, (short) data_size);
      rcv_conn->msg_pos += data_size;
    }
  }
  post_mutex ();
  return (rcv_conn->msg->msg_len <= (CARDINAL32) data_size);
} /* get_CR_data */

/**********************************************************************/

void new_conn()
/* Called on reception of a packet for which there is no open( in this
   case 'rcv_conn' has value NULL ) or a frozen connection.
   A received DR is always answered with a DC. */
{
int              dest_type;
unsigned int     hdr_len;

#ifdef TRACE_ENB
  if( trace_enable )
    if( rcv_conn == NULL ) {
       trace( CALLER_BIT & rcv_sap, rcv_mtc, 0 /* dummy */, 0 /* dummy */ );
  } /* if */
#endif

  if( rcv_mtc == DR )
  {
    reply_disc_conf ();
  } 
  else if ( rcv_mtc == CR )
  {
    run_time += ACCEPT_RUN_TIME;
    if( CALLER_BIT & rcv_sap )
    {
      dest_type = TM_CALLER_USER;
    }
    else
    {
      dest_type = TM_REPLIER_USER;
    } /* if */
    if (users[dest_type].invoke_notif == NULL)
    {
      reply_disc_requ( AM_UNKNOWN_DEST_ERR );
      return;
    } /* if */
    
/* get a connection descriptor if possible: */
    if( rcv_conn == NULL )
    {
      if( idle_rcv_conn == NULL )
      { 
	reply_disc_requ( AM_REM_CONN_OVF);
        return;
      }
      else
      {
        rcv_conn = idle_rcv_conn;
        idle_rcv_conn = rcv_conn->next;
      } /* if */
      rcv_conn->next = open_rcv_conn;
      open_rcv_conn = rcv_conn;
      nr_of_rcv_conn++;
    } /* if */

/* initialize the connection descriptor: */
    rcv_conn->type = dest_type;
    rcv_conn->msg->link = rcv_link;
    rcv_conn->msg->cancelled = 0;
    rcv_conn->msg->cancel_enb = !0;
    convert_big_endian32(&(rcv_conn->msg->msg_len),
			 &(rcv_par_ptr->cr_par.msg_size));
    rcv_conn->msg->my_vehicle = rcv_dest.vehicle;
    users[dest_type].invoke_notif
      ( &rcv_conn->msg->link,
        &rcv_conn->msg->msg_addr, rcv_conn->msg->msg_len,
        &rcv_conn->msg->hdr_addr, &hdr_len,
        rcv_conn->msg,
	&rcv_conn->msg->param, &rcv_conn->error,
	rcv_conn->msg->my_vehicle );
    rcv_conn->msg_pos = (unsigned char *) rcv_conn->msg->msg_addr;
    rcv_conn->msg_pos += 0; /* force pointer normalization */
    if( rcv_conn->error != AM_OK )
    {
      reply_disc_requ( rcv_conn->error );
      free_conn( 0, rcv_conn );
      return;
    } /* if */
    rcv_conn->conn_run_nr = rcv_par_ptr->cr_par.conn_run_nr;
    rcv_conn->state = LISTEN;
    rcv_conn->expected = 0;
    rcv_conn->cnt.neu = 0;
    rcv_conn->my_sap = rcv_sap;
    rcv_conn->credit_and_pack_size = rcv_par_ptr->cr_par.credit_and_pack_size;
    rcv_eot = get_CR_data (hdr_len);
    send_CC (rcv_conn);
    if (rcv_eot)
    {
      close_rcv (rcv_conn, AM_OK);
    }
    else
    {
      restart_tmo( rcv_conn, SHRT_ALIVE_TMO_INDEX );
    }
  } /* if */
} /* new_conn */

/**********************************************************************/

void repeat( CONN_DESCR * conn )
{
unsigned int    time, pack_size, next_send, nr, next_nr;
MD_PACKET **    my_packets;

  time = SEND_TMO_INDEX;
  if( conn->state == RCV_CANC )
  {
    send_DR (conn->ref.reply, conn);
    tm_packet_retries = ((tm_packet_retries +1) % 0xFFFFFFFFL);
  }
  else if (conn->state == SEND_CANC)
  {
    send_DR (conn->ref.send_ext->pack[0], conn);
    tm_packet_retries = ((tm_packet_retries +1) % 0xFFFFFFFFL);
  }
  else if (conn->state == SETUP)
  {
    time = CONN_TMO_INDEX;
    send_CR (conn);
    tm_packet_retries = ((tm_packet_retries +1) % 0xFFFFFFFFL);
  }
  else /* if (conn->state == SEND) */
  {
    my_packets = conn->ref.send_ext->pack;
    next_send = conn->ref.send_ext->next_send;
    pack_size = conn->ref.send_ext->size;
    my_full_addr.vehicle = conn->msg->my_vehicle;
    my_full_addr.fct_or_dev = conn->msg->link.my_fct_or_dev;
    for (next_nr = conn->expected; next_nr != next_send; )
    {
      nr = next_nr;
      next_nr = ( next_nr + 1 ) % SEQ_NR_SIZE;
      if (next_nr == next_send)
      {
        pack_size = conn->ref.send_ext->last_size;
      } /* if */
      if (my_packets[nr]->status != MD_PENDING)
      {
        nm_send_pack (my_packets[nr], pack_size, &my_full_addr,
		      &conn->msg->link, &conn->error );
       	tm_packets_sent = ((tm_packets_sent+1) % 0xFFFFFFFFL);
        tm_packet_retries = ((tm_packet_retries +1) % 0xFFFFFFFFL);
      }
      else
      {
        break;
      } /* if */
    } /* for */
  } /* if */
  restart_tmo( conn, time );
  conn->cnt.rep++;
} /* repeat */

/**********************************************************************/

void handle_tmo( CONN_DESCR * conn )
/* A timeout expired for an open connection */
{
#ifdef TRACE_ENB
  if( trace_enable )
     trace_tmo( CALLER_BIT & conn->my_sap, conn->state, conn->cnt.rep );
#endif

  switch( conn->state )
  {
  case LISTEN : case RECEIVE :
    if( conn->error == AM_OK )
    {
      conn->error = AM_ALIVE_TMO_ERR;
    }
    close_rcv( conn, conn->error );
    break;
  case PEND_ACK :
    request_ack( conn );
    break;
  case FROZEN : case LISTEN_FROZEN :
    free_conn( 0, conn );
    break;
  case CLOSED :
    close_send( conn, conn->error );
    break;
  case DISC :
    hamster_continue (TRUE);                  /* Mr : 93-06-21 */
    break;
  case SETUP : case SEND : case SEND_CANC : case RCV_CANC :
    if ((conn->cnt.rep < MAX_REP_CNT ) &&    /* extended EH 22-Jun-1994 */
        !((conn->eot == !0) && (conn->state == SETUP) && 
          (conn->msg->send_mode & TM_MODE_PREVENT_DUPL)))
    {
      repeat( conn );
    } 
    else
    {
      if( conn->state == RCV_CANC )
      {
        close_rcv( conn, AM_FAILURE );
      } 
      else
      {
	if( conn->error == AM_OK )
        {
          if( conn->state == SETUP )
	  {
	    if (conn->ref.send_ext->pack[0]->status == MD_SENT)
	    {
	      conn->error = AM_CONN_TMO_ERR;
	    }
	    else
	    {
	      conn->error = AM_BUS_ERR;
	    }
	  }
          else if ( conn->state == SEND )
          {
	    conn->error = AM_SEND_TMO_ERR;
          } 
          else
          {
            conn->error = AM_FAILURE;
          }
        } /* if */
        close_send( conn, conn->error );
      } /* if */
    } /* if */
  default :
    break;
  } /* switch */
} /* handle_tmo */

/**********************************************************************/

void start_send( int who, TM_MSG_DESCR * msg_ref )
/* opens a send connection and sends CR.
   entry condition: idle_send_conn != NULL.
*/
{
CONN_DESCR *     conn;
    
/* get a send connection descriptor: */
  hamster_halt ( idle_send_conn == NULL );
  conn = idle_send_conn;
  idle_send_conn = conn->next;
  conn->next = open_send_conn;
  open_send_conn = conn;

/* initialize the connection descriptor: */
  if( who == TM_CALLER_USER )
  {
    conn->my_sap = CSEND_SAP;
  } 
  else
  {
    conn->my_sap = RSEND_SAP;
  }
  conn->msg = msg_ref;
  conn->type = who;
  conn->state = SETUP;
  conn->expected = 0;
  conn->cnt.rep = 0;
  conn->eot = 0;
  conn->error = AM_BUS_ERR;

  conn->upd_pend = !0;
  conn->msg_pos = (unsigned char *) conn->msg->msg_addr;
  conn->msg_pos += 0; /* force pointer normalization */
  send_CR (conn);
  if (conn->error == AM_INAUG_ERR)             /* CL 060695 */
  {
	 close_send (conn, AM_INAUG_ERR);
  }
  else
  {
	 restart_tmo( conn, CONN_TMO_INDEX );
  }
} /* start_send */

/**********************************************************************/

void send_CR (CONN_DESCR * conn)
/* Sends a connect request packet. If 'upd_pend' then builds the packet
   before transmission, updates 'eot' and clears 'upd_pend'.
   Uses the static variable 'conn_run_nr' to build a packet and increments
   it afterwards.
   No action if the packet is pending. */
{
CR_PARAM *     param;
MD_PACKET *    packet;
unsigned int * hdr_len_ptr;
AM_RESULT      error;
unsigned int   copy_size;

  packet = conn->ref.send_ext->pack[0];
  if (packet->status != MD_PENDING)
  {
    hdr_len_ptr = &conn->msg->hdr_len;
		if (conn->upd_pend)
    {
      conn->error = AM_OK;
      packet->port.lk.mtc = conn->my_sap | CR;
      param = (CR_PARAM *) packet->port.lk.data;
      convert_big_endian16(&conn->conn_run_nr, &conn_run_nr);
      param->conn_run_nr = conn->conn_run_nr;
      conn_run_nr++;
      convert_big_endian32(&(param->msg_size), &(conn->msg->msg_len));
			param->credit_and_pack_size = (unsigned char) ((BIT4 * MAX_CREDIT) + my_pack_size_code);
      if (*hdr_len_ptr > TM_MSG_HDR_SIZE)
      {
        *hdr_len_ptr = TM_MSG_HDR_SIZE;
      }
      pend_mutex();
      if (!conn->msg->cancelled)
      {
        if (*hdr_len_ptr != 0)
        {
          pi_copy8 (param->hdr_data, conn->msg->hdr_addr,
                    (short) *hdr_len_ptr);
        }
        copy_size = TM_MSG_HDR_SIZE - *hdr_len_ptr;
        conn->eot = (conn->msg->msg_len <= (CARDINAL32) copy_size);
        if (conn->eot)
        {
          copy_size = (unsigned int) conn->msg->msg_len;
        }
        if (copy_size > 0)
        {
          pi_copy8 (&param->hdr_data[*hdr_len_ptr], (void *)conn->msg_pos,
                    (short) copy_size);
          conn->msg->msg_len -= copy_size;
          conn->msg_pos += copy_size;
          *hdr_len_ptr += copy_size;
        }
      }
      post_mutex ();
      conn->upd_pend = 0;
    } /* if (conn->upd_pend) */
    if (!conn->msg->cancelled)
    {
      my_full_addr.vehicle = conn->msg->my_vehicle;
      my_full_addr.fct_or_dev = conn->msg->link.my_fct_or_dev;
      nm_send_pack (packet, CR_MIN_PACK_SIZE + *hdr_len_ptr,
                    &my_full_addr,&conn->msg->link, &error);
      tm_packets_sent = ((tm_packets_sent+1) % 0xFFFFFFFFL);

      if (error != AM_OK)
      {
        conn->error = error;
      }
    } /* if (!conn->msg->cancelled) */
  } /* if */
} /* send_CR */

/**********************************************************************/

void send_CC (CONN_DESCR * conn)
/* Sends a connect confirm packet. No action if the packet is pending. 
*/
{
CC_PARAM *     param;
MD_PACKET *    packet;
unsigned int   ps;

  packet = conn->ref.reply;
  if (packet->status != MD_PENDING)
  {
    packet->port.lk.mtc = conn->my_sap | CC;
    param = (CC_PARAM *) packet->port.lk.data;
    param->conn_run_nr = conn->conn_run_nr;
    ps = conn->credit_and_pack_size;
    conn->credit = ps / BIT4;
    ps %= BIT4;
    if (my_pack_size_code < ps)
		{
      ps = my_pack_size_code;
    }
    if (conn->credit > my_credit)
    {
      conn->credit = my_credit;
    }
    param->credit_and_pack_size 
      = (unsigned char) (BIT4 * conn->credit + ps);
    my_full_addr.vehicle = conn->msg->my_vehicle;
    my_full_addr.fct_or_dev = conn->msg->link.my_fct_or_dev;
    nm_send_pack (packet, CC_PACK_SIZE, &my_full_addr,
		  &conn->msg->link, &conn->error);
    tm_packets_sent = ((tm_packets_sent+1) % 0xFFFFFFFFL);
  } /* if */
} /* send_CC */

/**********************************************************************/

void send_DR (MD_PACKET * packet, CONN_DESCR * conn)
/* Sends a disconnect request packet with reason = "AM_REM_CANC_ERR".
   No action if the packet is pending. */
{
DR_PARAM * param;

  if (packet->status != MD_PENDING)
  {
    packet->port.lk.mtc = conn->my_sap | DR;
    param = (DR_PARAM *) packet->port.lk.data;
    param->reason = AM_REM_CANC_ERR;
    my_full_addr.vehicle = conn->msg->my_vehicle;
    my_full_addr.fct_or_dev = conn->msg->link.my_fct_or_dev;
    nm_send_pack (packet, DR_PACK_SIZE, &my_full_addr,
		  &conn->msg->link, &conn->error);
    tm_packets_sent = ((tm_packets_sent+1) % 0xFFFFFFFFL);
  } /* if */
} /* send_DR */

/**********************************************************************/

unsigned int decode_pack_size (unsigned int code)
{
  return ((unsigned int) code_table [code]);
} /* decode_pack_size */
      
/**********************************************************************/

unsigned int encode_pack_size (unsigned int pack_size)
{
unsigned int ix;
  for (ix = 1; ix < NR_OF_CODES; ix++)
  {
    if (pack_size < (unsigned int) code_table [ix])
    {
      break;
    }
  }
  return (ix-1);
} /* encode_pack_size */

/* support of external protocol or poll function: *********************/
/* <ATR:02> */
#ifdef __C166__
AM_RESULT tm_define_ext_poll (TM_POLL_FCT new_poll,  /* EH 28-Feb-1994 */
                              TM_POLL_FCT huge *old_poll)
#else
AM_RESULT tm_define_ext_poll (TM_POLL_FCT new_poll,  /* EH 28-Feb-1994 */
                              TM_POLL_FCT *old_poll)                              
#endif
{
  if ((new_poll != NULL) && (old_poll != NULL)) {
    pi_disable ();
    *old_poll = ext_poll;
    ext_poll  = new_poll;
    pi_enable ();
    return AM_OK;
  } else {
    return AM_FAILURE;
  }
} /* tm_define_ext_poll */

/**********************************************************************/
#ifdef MULTICAST
AM_RESULT tm_define_ext_rcv_pack (NM_RCV_FCT new_rcv,
                                  NM_RCV_FCT *old_rcv)
{
  if ((new_rcv != NULL) && (old_rcv != NULL)) {
    pi_disable ();
    *old_rcv = ext_rcv_pack;
    ext_rcv_pack = new_rcv;
    pi_enable ();
    return AM_OK;
  } else {
    return AM_FAILURE;
  }
} /* tm_define_ext_rcv_pack */
#endif
/**********************************************************************/

void own_poll (void) {} 

/**********************************************************************/

void tm_system_init( void )
{
short status;

  do_abort_TB_conn = 0;
  ext_poll = own_poll;
#ifdef MULTICAST
  ext_rcv_pack = nm_rcv_pack;
#endif
  nm_system_init ();
  md_init_list( &idle_msg);
  msg_que_size = nr_of_send_conn = nr_of_rcv_conn = 0;
  idle_send_conn = idle_rcv_conn = open_rcv_conn = open_send_conn = NULL;
  md_init_list( &send_msg[TM_CALLER_USER]);
  tm_define_user (TM_CALLER_USER, NULL, NULL, NULL);
  md_init_list( &send_msg[TM_REPLIER_USER]);
  tm_define_user (TM_REPLIER_USER, NULL, NULL, NULL);
  send_reply = fetch_next = 1;
  my_credit = 0; /* invalid value, checked in tm_send_msg_requ */
  tm_put_call = tm_put_reply = (TM_GET_MSG_IND) default_put;
  tm_deliver_call = tm_deliver_reply 
    = (TM_DELIVER_MSG_IND) default_deliver;
  conn_run_nr = 0;
	my_full_addr.link_dev = AM_SAME_DEVICE;
	my_pack_size_code = encode_pack_size (MY_PACK_SIZE);
	md_init_list( &timed_out_conn);
  halt_condition = ( sizeof( CARDINAL32 ) != sizeof( TM_LINK_ID ) );
  if (halt_condition)
  /* change function 'find_conn' if this error comes. */
  {
    halt_condition = 1;
  }
  else
  {
#ifndef O_SUN  /* Alligment Problem with SUN ACC COMPILER */
    halt_condition =
      ( CODE_0_PACK_SIZE < ( PACK_HDR_SIZE + sizeof ( CR_PARAM ) ) );
#endif
    if (halt_condition)
    {
      halt_condition = 2;
    }
  }
  cancel_mutex = pi_create_semaphore (1, PI_ORD_FCFS, &status);
  if (status != PI_RET_OK)
  {
    halt_condition = 3;
  }
  delay_only = pi_create_semaphore (0, PI_ORD_FCFS, &status);
  if (status != PI_RET_OK)
  {
    halt_condition = 4;
  }
} /* tm_system_init */
/*-----------------------------------------------------------------------------

	 FUNCTION: tm_read_messenger_status

	 INPUT:

	 OUTPUT:   see function

	 RETURN:   -

	 ABSTRACT: Read status informations of the messengers transport layer

-----------------------------------------------------------------------------*/
void tm_read_messenger_status(unsigned long  *p_packets_sent,
                              unsigned long  *p_packet_retries,
                              unsigned long  *p_multicast_retries,
                              unsigned char  *p_send_time_out,
                              unsigned char  *p_ack_time_out,
                              unsigned char  *p_alive_time_out,
                              unsigned char  *p_credit,
                              unsigned char  *p_packet_size,
                              unsigned char  *p_multicast_window)
{
  /* Timeouts on the WTB */
  /* The timeouts in the timeout table are specified in PI_TICKS. */
  /* PI_TICK is dependent  from the OS (VCOS 16ms, Micos 64ms).   */
  /* To retrive the timeouts in multiples of 64 ms follow formula */
  /* is used: ((timeout(Ticks) * PI_TICK(dev. dep.)/64UL)         */
  /* timeout in multiple of 64ms */

  *p_send_time_out     = (unsigned char) ((time_table[TMO_WTB_INDEX] [SEND_TMO_INDEX]
												 * PI_TICK) / 64UL);
  /* timeout in multiple of 64ms */
  *p_ack_time_out      = (unsigned char) ((time_table[TMO_WTB_INDEX] [ACK_TMO_INDEX]
												 * PI_TICK) / 64UL);
  /* timeout in sec. */
  *p_alive_time_out    = (unsigned char) ((time_table[TMO_WTB_INDEX] [SHRT_ALIVE_TMO_INDEX]
												 * PI_TICK) / 1000UL);
  /* round if neccessary */

  if (((time_table[TMO_WTB_INDEX] [SHRT_ALIVE_TMO_INDEX] * PI_TICK)
       % 1000UL) >= 500UL)
  {
    *p_alive_time_out = *p_alive_time_out + 1;
  }

  *p_credit            =  0x0F & ((unsigned char)my_credit);
  *p_packet_size       =  0x0F & ((unsigned char)tm_packet_size);
  *p_packets_sent      =  tm_packets_sent ;
  *p_packet_retries    =  tm_packet_retries;

  /* multicast is not yet supported */

  *p_multicast_window  =  tm_multicast_window;
  *p_multicast_retries =  tm_multicast_retries;
}

/*-----------------------------------------------------------------------------

	 FUNCTION: tm_write_messenger_control

	 INPUT:    see function

	 OUTPUT:   -

	 RETURN:   -

	 ABSTRACT: Set parameters in the Messengers transport layer

-----------------------------------------------------------------------------*/


short tm_write_messenger_control(unsigned char send_time_out,
                                 unsigned char ack_time_out,
                                 unsigned char alive_time_out,
                                 unsigned char packet_size,
                                 unsigned char packet_lifetime,
                                 unsigned char multicast_window,
                                 unsigned char credit)
{
  int tm_write_error = 0;

  /* Timeouts for the WTB                                                 */
  /* The value of the input parameters send_time_out and ack_timeout      */
  /* specifies a multiples of 64ms.                                        */
  /* The timeout table for the messenger expected the time in PI_TICK.    */
  /* PI_TICK is OS dependent, so follow formula is used:                  */
  /* timeout_table.time_out = ((.._time_out * 64UL) / PI_TICK(16|64ms))   */

  /* timeout in ticks */
  time_table[TMO_WTB_INDEX] [SEND_TMO_INDEX] =
														((send_time_out * 64UL)/PI_TICK);
  /* timeout in ticks */
  time_table[TMO_WTB_INDEX] [ACK_TMO_INDEX] =
														((ack_time_out * 64UL)/PI_TICK);
  /* timeout in ticks */
  time_table[TMO_WTB_INDEX] [SHRT_ALIVE_TMO_INDEX] =
														((alive_time_out/*sec*/ * 1000UL)/PI_TICK);

#ifndef NM_1TB
  /* write a new bus timeout interval */
  tm_write_error = nm_change_packet_lifetime((packet_lifetime *64UL)/PI_TICK);
#else
  packet_lifetime = packet_lifetime;
#endif

  /* set a new credit, which is accepted by this station */

  if (credit > MAX_CREDIT)
  {
    my_credit = MAX_CREDIT;
  }
  else if(credit < 1)
  {
    my_credit = 1;
  }
  /* multicast is not yet supported */
  multicast_window = multicast_window;
  /* packet_size is fixed during the initialization time.
  Now it is not possible to change the packet size dynamic.
  Only the COM960 is able to transmit 128 bytes packets. The
  LL MVBC and LL BAP are not able to transmit 128 byte packets */

  /* avoid warnings */
  packet_size = packet_size;

  return tm_write_error;
}
