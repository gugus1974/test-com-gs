/************************************************************************/
/*  Asea Brown Boveri Research Center, CH-5405 Baden, Switzerland       */
/*  Copyright 1991                                                      */
/*                                                                      */
/*  NAME:                                                               */
/*    TM_I_LAY.C                                                        */
/*                                                                      */
/*  ABSTRACT:                                                           */
/*    TCN Message Transport Protocol, Transport layer                   */
/*    Provides simplex connections between a caller and a replier.      */
/*    A LinkId together with the channel( SAP ) identify a connection.  */
/*    This file contains the interface to the application layer.        */
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
/*              04-Feb-1992   CRBC1    Hoepli                           */
/*              11-Mar-1992   CRBC1    Hoepli     NET_DELAY changed     */
/*              12-Mar-1992   CRBC1    Hoepli     DR reason passing     */
/*              17-Mar-1992   CRBC1    Hoepli     alignment in CR, CC   */
/*              22-Apr-1992   CRBC1    Hoepli     tm_deliver_xx added   */
/*              19-May-1992   CRBC1    Hoepli     TM_INVOKE_MSG_IND chg */
/*              28-Jul-1992   CRBC1    Hoepli     run_time (limitation) */
/*              24-Sep-1992   BAEI-2   Muenger    TM_I_LAY, TM_M_LAY    */
/*              25-Nov-1992   BAEI-2   Muenger    tm_delay added.       */
/*              07-Mar-1994   CRBC1    Hoepli     define DU_MODULE_NAME */
/*              22-Jun-1994   CRBC1    Hoepli     early descr. release  */
/*              15-Feb-1995   BATL     Flum       Switch O_MD_STDA to   */
/*                                                compile the sources   */
/*                                                without C standard lib*/
/************************************************************************/

#define DU_MODULE_NAME "TM_I_LA"
/* for hamster calls which are now mapped to JDP event recorder.
   DU_MODULE_NAME must be defined before "du_sys.h" is included.
*/

#if defined (O_MD_STDA)
#define NULL      0
#else
#include <stdio.h>         /* NULL */
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

/* VARIABLES --------------------------------------------------------------- */
short cancel_mutex;
/* semaphore for mutual exclusive access to the 'cancelled' field.
   The message/header buffer must not be accessed anymore after successful
   cancel!
*/

short delay_only; /* sema_id */
/* semaphore for delays only, does never get a signal.
*/

TM_GET_MSG_IND tm_put_call; /* execute only! */
/* Sends a call message to a local replier.
   A 'result' !== 'AM_OK' signals that the replier did not get the message.
   The replier is not allowed to access the passed call message buffer
   after return from this procedure, therefore this buffer may be reused 
   ( overwritten ) by the caller immediately. The call is not yet allowed
   to be executed! */

TM_DELIVER_MSG_IND tm_deliver_call; /* execute only! */
/* Gives a local call free for execution (abort with 'result' != 'AM_OK').
   The call must have been passed with a preceeding 'tm_put_call' which
   has returned 'AM_OK' together with the same 'param'. */

TM_GET_MSG_IND tm_put_reply;   /* execute only! */
/* Sends a reply message back to a local caller.
   A 'result' !== 'AM_OK' signals that the caller did not get the message.
   The caller is not allowed to access the passed reply message buffer
   after return from this procedure, therefore this buffer may be reused 
   ( overwritten ) by the replier immediately. The reply is not yet allowed
   to be evaluated! */

TM_DELIVER_MSG_IND tm_deliver_reply; /* execute only! */
/* Gives a local reply free for evaluation (abort with 'result' != 'AM_OK').
   The reply must have been passed with a preceeding 'tm_put_reply' which
   has returned 'AM_OK' together with the same 'param'. */



/* FUNCTIONS --------------------------------------------------------------- */

void pend_mutex (void)
{
    short result;

    pi_pend_semaphore (cancel_mutex, PI_FOREVER, &result);
}

void post_mutex (void)
{
    short result;

    pi_post_semaphore (cancel_mutex, &result);
}

void tm_delay (unsigned short time)
{
    short status;

    pi_pend_semaphore (delay_only, time, &status);
}


/* shortcut: --------------------------------------------------------------*/

AM_RESULT default_put (void)
{
  return AM_UNKNOWN_DEST_ERR;
} /* default_put */

/**********************************************************************/

void default_deliver (void)
{
  hamster_halt (TRUE);
} /* default_deliver */

/* timeout: ***********************************************************/

void signal_tmo( void * param )
/* called from the external Timeout handler */
{
  md_put_ele( timed_out_conn, (MD_LIST_HEADER *)param );
  ((CONN_DESCR *)param)->timed_out = 1;
  ((CONN_DESCR *)param)->timer_running = 0;
} /* signal_tmo */
  
/* exported: **********************************************************/

AM_RESULT tm_send_msg_requ ( int who, const TM_LINK_ID * link,
                             void * msg_addr, CARDINAL32 msg_len,
                             void * hdr_addr, unsigned int hdr_len,
			     void * param, TM_MSG_DESCR * * cancel_ref,
                             unsigned char my_vehicle,
                             unsigned char mode)   /* added EH 22-Jun-1994 */
{
TM_MSG_DESCR * msg;

  if ((msg_addr == NULL) && (msg_len != 0))
  {
    return AM_FAILURE;
  }
  if ((hdr_addr == NULL) && (hdr_len != 0))
  {
    return AM_FAILURE;
  }
  if (my_credit == 0)
  {
    return AM_FAILURE; /* call 'tm_init' first! */
  }
  md_get_ele( idle_msg, (MD_LIST_HEADER * *)&msg );
  if( msg == NULL )
  {
    msg = (TM_MSG_DESCR *)pi_alloc( sizeof( TM_MSG_DESCR ) );
    if( msg == NULL )
    {
      return AM_NO_LOC_MEM_ERR;
    }
    md_init_ele((MD_LIST_HEADER *) msg );
  } /* if */
  msg->cancelled = 0;
  msg->cancel_enb = 1;
  msg->link = *link;
  msg->msg_addr = msg_addr;
  msg->msg_len = msg_len;
  msg->hdr_addr = hdr_addr;
  msg->hdr_len = hdr_len;
  msg->param = param;
  msg->my_vehicle = my_vehicle;
  msg->send_mode = mode;          /* added EH 22-Jun-1994 */

  md_put_ele( send_msg[who], (MD_LIST_HEADER *) msg );
  *cancel_ref = msg;
  return AM_OK;
} /* tm_send_msg_requ */

/**********************************************************************/

int tm_cancel_msg ( TM_MSG_DESCR * cancel_ref )
{
int     result;

  result = 0;
  hamster_halt (halt_condition);
  if( cancel_ref != NULL )
  {
	pend_mutex ();
	if (cancel_ref->cancel_enb)
	{
	  cancel_ref->cancelled = !0;
	  result = !0;
	}
	post_mutex ();
  }
  return result;
} /* tm_cancel_msg */
