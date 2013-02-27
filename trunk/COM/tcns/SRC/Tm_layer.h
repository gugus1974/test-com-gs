#ifndef TM_LAYER_H
#define TM_LAYER_H

/************************************************************************/
/*  Asea Brown Boveri Research Center, CH-5405 Baden, Switzerland       */
/*  Copyright 1991                                                      */
/*                                                                      */
/*  NAME:                                                               */
/*    TM_LAYER.H                                                        */
/*    TM = Transport layer interface for Messages                       */
/*                                                                      */
/*  ABSTRACT:                                                           */
/*    TCN Message Transport Protocol, Transport layer.                  */
/*    Provides message transfer over the bus or local between a caller  */
/*    and a replier function.                                           */
/*    A simplex bus connection is established for each message.         */
/*                                                                      */
/************************************************************************/
/*                                                                      */
/*  AUTHOR  :   24-Apr-1991     CRBC1   Hoepli                          */
/*  MODIFIED:   1991.08.22   BAEI-3   Krzeslowski                       */
/*              translated to C                                         */
/*              ??? !!! ??? ist used to show that the code could be     */
/*              improved.                                               */
/*              ADDRESS is translated to 'void *'                       */
/*              TO is changed to TMO within the names for timeouts.     */
/*              All constants are changed to dec or hex.                */
/*              04-Dec-1991  CRBC1    Hoepli                            */
/*              22-Apr-1992  CRBC1    Hoepli      tm_deliver_xx added   */
/*              18-May-1992  CRBC1    Hoepli      TM_INVOKE_MSG_IND     */
/*              26-Sep-1992  CRBC1    Hoepli      extrn var. (shortcut) */
/*              25-Nov-1992  BAEI-2   Muenger     tm_delay added        */
/*              18-Nov-1993  CRBC1    Hoepli      external protocol     */
/*              28-Feb-1994  CRBC1    Hoepli      " " -> poll/rcv_pack  */
/*              22-Jun-1994  CRBC1    Hoepli      early descr. release  */
/*              06-Sep-1995  BATL     Flum    tm_read_messenger_status  */
/*                                            added                     */
/*              12-Oct-1995  BATL     Flum    tm_write_messenger_control*/
/*                                            added                     */
/************************************************************************/


#include "nm_layer.h"

#include "md_layer.h"

/* CONST -------------------------------------------------------------- */

#define TM_MSG_HDR_SIZE    15
/* This is the maximal message header size. A longer message header 
   is cut to this limit without any warning. */
   

#define TM_CALLER_USER    0
#define TM_REPLIER_USER   1
/* These are the user types of the transport layer interface ('who') */
#define TM_USER_TYPE_SIZE 2

/* added EH 22-Jun-1994: values for 'mode' param. in 'tm_send_msg_requ': */
#define TM_MODE_NORMAL        0
#define TM_MODE_RELEASE       BIT0  /* release rcv conn. with incoming CC */ 
#define TM_MODE_PREVENT_DUPL  BIT1  /* send CR containing eot once only */

/* TYPES -------------------------------------------------------------- */


typedef struct {
MD_LIST_HEADER    list;
TM_LINK_ID        link;
unsigned char     my_vehicle;
void            * msg_addr;
CARDINAL32        msg_len;
void            * hdr_addr;
unsigned int      hdr_len;
void            * param;
int               cancelled;
int               cancel_enb;
unsigned char     send_mode;    /* added EH 22-Jun-1994 */
} TM_MSG_DESCR;
/* This type should not be known outside of this module, it should be
   hidden. So do not access this descriptor!
 */

typedef void ( * TM_SEND_MSG_CONF )  ( void * /* param */,
                                       AM_RESULT /* error */,
                                       unsigned char /* my_vehicle */ );

typedef void ( * TM_INVOKE_MSG_IND ) ( /* in: link */ const TM_LINK_ID *,
                                       /* out: msg_addr */ void * *,
                                       /* msg_len */ CARDINAL32,
                                       /* out: hdr_addr */ void * *,
                                       /* out: hdr_len */ unsigned int *,
                                       /* in: canc_ref */ TM_MSG_DESCR *,
                                       /* out: param */ void * *,
                                       /* out: error */ AM_RESULT *,
                                       /* my_vehicle */ unsigned char );

typedef void ( * TM_RCV_MSG_IND ) ( /* param */ void *,
                                    /* error */ AM_RESULT,
                                    /* boolean: release */ unsigned char * );
/* 'release' added EH 22-Jun-1994: allows to release transport layer
   internal receive connection descriptors immediately.
*/

typedef AM_RESULT ( * TM_GET_MSG_IND )
                     ( /* from_fct, to_fct */ unsigned char, unsigned char,
                       /* msg_addr */ void *, 
                       /* msg_len */ CARDINAL32,
                       /* hdr_addr */ void *, 
                       /* hdr_len */ unsigned int,
                       /* out: param */ void * *);

typedef void (* TM_DELIVER_MSG_IND)( void * param, AM_RESULT result);

/* EH 28-Feb-1994, function types for 'tm_define_ext_xx' parameters: */
typedef void (* TM_POLL_FCT) (void);
typedef int  (* NM_RCV_FCT)  (MD_PACKET **pack,
                              unsigned int *size,
			      TM_LINK_ID *src,
			      TM_LINK_ID *dest,
                              AM_RESULT *error);

/* FUNCTIONS ---------------------------------------------------------- */
  
void tm_define_user ( int who, 
                      TM_INVOKE_MSG_IND listen, 
                      TM_RCV_MSG_IND rcv,
                      TM_SEND_MSG_CONF done);
/* 'who' must be either 'TM_CALLER_USER' or 'TM_REPLIER_USER'.
   The 'listen' procedure is called after reception of an invoke
   packet. A connection is only accepted if 'listen' returns
   'AM_OK' as error, any other error is interpreted as reject reason.
   'cancel_ref' of 'listen' is just valid until 'rcv' will be called and
   must no more be used afterwards to cancel an accepted message reception.
   'rcv' will be called for every connection which has been accepted
   unless it has been cancelled successfully by this site. It passes the
   completely received message or an error != 'AM_OK' on failure.
   It will return 'param' from the related previous 'listen'.
   The messenger uses the buffer which is supplied externally with
   'listen' until it calls the related 'rcv' or until the connection is
   cancelled successfully by this site. */

AM_RESULT tm_send_msg_requ ( int who, 
                             const TM_LINK_ID * link,
                             void * msg_addr, CARDINAL32 msg_len,
                             void * hdr_addr, unsigned int hdr_len,
                             void * param, 
                             /* out */ TM_MSG_DESCR * * cancel_ref,
                             unsigned char my_vehicle,
                             unsigned char mode);  /* added EH 22-Jun-1994 */
/* 'who' must be either 'TM_CALLER_USER' or 'TM_REPLIER_USER'.
   Returns a result !== 'AM_OK' if the message cannot be queued( no action ).
   Otherwise the procedure will return immediately and complete later.
   The 'done' procedure of a previous 'tm_define_user' will be called 
   on completion and will return 'param' which is not inspected but
   just offered as possibility for an external reference.
   In '*cancel_ref' a value is returned which later may be used for cancel.
   '*cancel_ref' is just valid until 'done' will be called and must no more
   be used afterwards to cancel a transmission.
   The message header size 'hdr_len' is restricted and cut if necessary
   to 'TM_MSG_HDR_SIZE'.
   The field 'link.link_dev' must not be unknown.
   'mode' with values TM_MODE_xxx allows early release of transport layer
   internal connection descriptors (TM_MODE_RELEASE), or prevents retrans-
   mission of a CR which contains a full (short) message (this is necessary
   to prevent duplication of messages when a receive connection is released
   immediately with receive indication, mode TM_MODE_PREVENT_DUPL).
   The messenger accesses the send buffer until it calls the related 'done'
   or until the message is cancelled successfully by this site,
   but no longer after the destination has received the message. */

int tm_cancel_msg ( TM_MSG_DESCR * cancel_ref );
/* Cancels a previous 'tm_send_msg_requ' or an accepted( receive ) connection.
   Returns 0 if the connection cannot be cancelled. The confirmation procedure
   of a successfully cancelled message will never be called. Regard the
   restricted validity of 'cancel_ref'! 
   'cancel_ref' is no more valid after a successful cancel operation. */

void tm_init ( unsigned int credit, int max_conn );
/* Defines the proposed( maximum ) credit for all connections and
   the maximal number of connections. 
   Must be called before any send message request. */

/* shortcut: ---------------------------------------------------------- */
/*    Decouples the caller session layer from the replier session layer.*/
/*    Provides message transfer within the same device between a caller */
/*    and a replier function without using the bus.                     */
/*    The passed messages always are copied to avoid buffer management. */
/*                                                                      */
/*  RESTRICTIONS:                                                       */
/*    It must be allowed that the connected get procedures wait for     */
/*    some resources. Therefore no resources should be locked before    */
/*    calling a put procedure to prevent deadlocks!                     */

void tm_conn_get_call (TM_GET_MSG_IND get_ind, 
                       TM_DELIVER_MSG_IND deliver_ind); 
/* Assigns the procedures which will be called with 'tm_put_call' and
   'tm_deliver_call' respectively.
   Connects 'get_ind' as notification procedure to pass a local call
   message and 'deliver_ind' as notification to deliver a successfully passed
   local call for execution. If 'get_ind' returns 'AM_OK' then 'deliver_ind'
   is called with the same 'param' as has been returned with 'get_ind'. */

void tm_conn_get_reply ( TM_GET_MSG_IND get_ind, 
                         TM_DELIVER_MSG_IND deliver_ind); 
/* Assigns the procedure which will be called with 'tm_put_reply' and
   'tm_deliver_reply' respectively.
   Connects 'get_ind' as notification procedure to receive a local reply
   message and 'deliver_ind' as notification to deliver a successfully passed
   local reply for evaluation. If 'get_ind' returns 'AM_OK' then 'deliver_ind'
   is called with the same 'param' as has been returned with 'get_ind'. */

extern TM_GET_MSG_IND tm_put_call; /* execute only! */
/* Sends a call message to a local replier.
   A 'result' !== 'AM_OK' signals that the replier did not get the message.
   The replier is not allowed to access the passed call message buffer
   after return from this procedure, therefore this buffer may be reused 
   ( overwritten ) by the caller immediately. The call is not yet allowed
   to be executed! */

extern TM_DELIVER_MSG_IND tm_deliver_call; /* execute only! */
/* Gives a local call free for execution (abort with 'result' != 'AM_OK').
   The call must have been passed with a preceeding 'tm_put_call' which
   has returned 'AM_OK' together with the same 'param'. */

extern TM_GET_MSG_IND tm_put_reply;   /* execute only! */
/* Sends a reply message back to a local caller.
   A 'result' !== 'AM_OK' signals that the caller did not get the message.
   The caller is not allowed to access the passed reply message buffer
   after return from this procedure, therefore this buffer may be reused 
   ( overwritten ) by the replier immediately. The reply is not yet allowed
   to be evaluated! */

extern TM_DELIVER_MSG_IND tm_deliver_reply; /* execute only! */
/* Gives a local reply free for evaluation (abort with 'result' != 'AM_OK').
   The reply must have been passed with a preceeding 'tm_put_reply' which
   has returned 'AM_OK' together with the same 'param'. */

/* external protocol connection: -------------------------------------- */

/* EH 28-Feb-1994, functions replacing previous 'tm_define_ext_protocol': */
#ifdef __C166__
AM_RESULT tm_define_ext_poll (TM_POLL_FCT new_poll,    /* in  */
                              TM_POLL_FCT huge *old_poll);  /* out */
#else
AM_RESULT tm_define_ext_poll (TM_POLL_FCT new_poll,    /* in  */
                              TM_POLL_FCT *old_poll);  /* out */
#endif
/* Allows to connect an external function 'new_poll' that shall be polled 
   from the messenger task. The output parameter '*old_poll' shows the 
   previous poll function which !must! be called from 'new_poll'. Thus
   several poll functions can be connected, and each connector is obliged
   to invoke the previous poll function in addition to the own poll function.
   Returns AM_OK in case of success.
   Returns AM_FAILURE if either 'new_poll' or 'old_poll' is NULL (no action).
*/

AM_RESULT tm_define_ext_rcv_pack (NM_RCV_FCT new_rcv,    /* in  */
                                  NM_RCV_FCT *old_rcv);  /* out */
/* Allows to replace the current function which is called to receive a packet 
   from the network layer. Thus a received packet can be consumed externally
   by another protocol which acts like a filter and passes all packets that
   cannot be interpreted. 
   In future the messenger task will call 'new_rcv' to get the next received
   packet. The output parameter '*old_rcv' shows the previous receive filter
   function which was called up to now to receive a packet. The function
   'new_rcv' is obliged to call '*old_rcv' to get the next received packet.
   Thus several filters for different protocols can be chained, each filter
   consuming its own packets and passing all other packets.
*/
   
/* system requirements: ------------------------------------------------ */

void tm_system_init (void);
/* Must be called at system initialization. Calls 'nm_system_init'. */

void tm_messenger (void);
/* Must be started as a cyclic task which has lower priority than the
   timeout process. */

void tm_delay (unsigned short);
/* Delays the calling task. The length of the delay is specified in the
   parameter (in ticks). */

void tm_read_messenger_status(unsigned long *,
                              unsigned long *,
                              unsigned long *,
                              unsigned char *,
                              unsigned char *,
                              unsigned char *,
                              unsigned char *,
                              unsigned char *,
                              unsigned char *
                              );
/* read status variables from the Messengers transport layer */

short tm_write_messenger_control(unsigned char,
                                 unsigned char,
                                 unsigned char,
                                 unsigned char,
                                 unsigned char,
                                 unsigned char,
                                 unsigned char);
/* write parameters in the Messenger */

#endif
