#ifndef	AM_SUB_H
#define	AM_SUB_H
/************************************************************************/
/*  Asea Brown Boveri Research Center, CH-5405 Baden, Switzerland       */
/*  Copyright 1993                                                      */
/*                                                                      */
/*  NAME:                                                               */
/*    AM_SUB.H                                                          */
/*                                                                      */
/*  ABSTRACT:                                                           */
/*    internal interface of the session layer which has been introduced */
/*    to make some session layer functions available for the multicast  */
/*    protocol.                                                         */
/*                                                                      */
/************************************************************************/
/*                                                                      */
/*  AUTHOR  :   14-Jun-1993     CRBC1   Hoepli                          */
/*  MODIFIED:   02-Mar-1994     CRBC1   Hoepli  reply_tmo: int -> long  */     
/*              14-Jul-1994     CRBC1   Hoepli  am_new_call added       */
/*                 (replaces am_get_call and am_get_link_id)            */
/************************************************************************/

#include "am_sys.h"
#include "tm_layer.h"

/* CONSTANTS: --------------------------------------------------------- */

#define AM_PLEASE_REPLY         0 /* reply transmission enabled */
#define AM_NO_REPLY             1 /* no reply transmission */

/* TYPES: ------------------------------------------------------------- */

enum AM_CALL_STATES {
AM_IDLE,        /* descriptor is not in use */
AM_CALLING,     /* call transmission started, messenger or caller process
                   got a reference */
AM_WAIT_REPLY,  /* call transmission completed, timeout process got a ref.,
                   reply timeout is running */
AM_RECEIVING,   /* reply reception started, messenger or replier process
                   got a reference */
AM_CLOSING      /* the call will be closed unconditionally by the one process
                   that forced transition to this state */
/* the one process that has got a reference to the call descriptor is
   allowed to access the descriptor and to close the call */
};
                            
typedef union {             
AM_CALL_CONF    call;      
AM_INVOKE_CONF  invoke;     
} AM_ANY_CONF;             

typedef struct AM_CALL_DESCR {
struct AM_CALL_DESCR * next; /* in 'idle_calls' or 'open_calls' list */
unsigned long        reply_tmo;    /* EH, 02-Mar-1994: in ticks [PI_TICK] */
short                reply_timer;
AM_RESULT            result;
void               * ext_ref;
TM_MSG_DESCR       * canc_ref;
/* 'canc_ref' != NULL in states 'AM_CALLING' or 'AM_RECEIVING' only,
   reference to cancel message communication */
AM_ANY_CONF          conf_proc;     
TM_LINK_ID           link;
void               * in_msg_addr;
CARDINAL32           in_msg_size;
enum AM_CALL_STATES  call_state;
unsigned char        caller_vehicle;
unsigned char        my_buffer; /* messenger allocated reply buffer */
unsigned char        msg_hdr; /* call message header */
unsigned char        caller_fct;  /* EH 14-Jul-1994 added for cancel */
AM_ADDRESS           replier;     /* EH 14-Jul-1994 added for cancel */
} AM_CALL_DESCR;

/* PROTOTYPES: -------------------------------------------------------- */

/* caller's site: */
AM_RESULT am_new_call (unsigned char caller_fct, const AM_ADDRESS *replier,
                       AM_CALL_DESCR * * call, unsigned char *local);
void am_close_call( void * call, AM_RESULT error );
void am_c_send_conf( void * param, AM_RESULT err, 
                     unsigned char caller_vehicle );
void am_pend_call_mutex (void);
void am_post_call_mutex (void);

/* replier's site: */
void am_r_invoke_ind (const TM_LINK_ID * link,
                      void * * in_msg_addr, CARDINAL32 in_msg_size,
                      void * * in_hdr_addr, unsigned int * in_hdr_size,
                      TM_MSG_DESCR * canc_ref, void * * param,
                      AM_RESULT * err, unsigned char replier_vehicle);
void am_r_rcv_ind (void * param, AM_RESULT err);

#endif
