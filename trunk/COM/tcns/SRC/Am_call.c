/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> the QNX compiler prefers 960-like definitions                                       */
/*==============================================================================================*/


/************************************************************************/
/*  Asea Brown Boveri Research Center, CH-5405 Baden, Switzerland       */
/*  Copyright 1991                                                      */
/*                                                                      */
/*  NAME:                                                               */
/*    AM_CALL.C                                                         */
/*    The prototypes for the public functions are contained in the      */
/*    header module 'AM_SYS.H'.                                         */
/*                                                                      */
/*  ABSTRACT:                                                           */
/*    TCN Message Transport Protocol, Session layer for caller.         */
/*                                                                      */
/************************************************************************/
/*                                                                      */
/*  AUTHOR  :   23-May-1991     CRBC1   Hoepli                          */
/*  MODIFIED:   1991.08.22   BAEI-3   Krzeslowski ( WHK )               */
/*              the module was originally written in Modula 2 and then  */
/*              translated to C                                         */
/*                                                                      */
/*              The old Modula 2 names are left as comments at the very */
/*              end of every function.                                  */
/*                                                                      */
/*              BOOLEAN changed to 'int'.                               */
/*              CARDINAL changed to 'size_t'.                           */
/*                                                                      */
/*              14-Jan-1992  CRBC1    Hoepli                            */
/*              18-Mar-1992  CRBC1    Hoepli    - get_link_id (service) */
/*              22-Apr-1992  CRBC1    Hoepli      tm_deliver_xx added   */
/*              18-May-1992  CRBC1    Hoepli      c_invoke_ind changed  */
/*              28-Sep-1992  BAEI-2   Muenger     pend/post_mutex       */
/* @JDP         18-Nov-1992  CRBC1    Hoepli      calls with no reply   */
/* @JDP: lines containing "@JDP" must be deleted to get the JDP version */
/*              04-May-1993  CRBC1    Hoepli      bug in get_link_id    */
/*              18-Nov-1993  CRBC1    Hoepli      am_sub.h generated    */
/*              02-Mar-1994  CRBC1    Hoepli      PIL timeout interface */
/*                      (no pi_pend_semaphore in tmo handler, PI_TICK)  */
/*              07-Mar-1994   CRBC1    Hoepli     define DU_MODULE_NAME */
/*              14-Jun-1994   CRBC1    Hoepli     HUGE in get_int_reply */
/*              22-Jun-1994  CRBC1    Hoepli      early descr. release  */
/*              19-Jul-1994  CRBC1    Hoepli      topo count in am_ if  */
/*              27-Jul-1994  CRBC1    Hoepli      != 254 can call 253   */
/*              04-Aug-1994  CRBC1    Hoepli      next_sta in AM_ADDRESS*/
/*              13-Feb-1995  BATL     Flum        inserted SWITCH O_SUN */
/*              15-Feb-1995  BATL     Flum        insertes SWITCH       */
/*                                                O_MD_STDA             */
/*                                                to compile source     */
/*                                                without standard c lib*/
/*              05-Oct-1995  BATL     Flum	  inserted am_messages_sent,*/
/*                                                 am_messages_received */
/*                                                                      */
/* CONFIGURATION DEFINES:                                               */
/*    since version of 04-Aug-1994, the caller session layer accesses   */
/*    the station directory table (StaDi) if there is no switch defined */
/*    which indicates simple routing. The same compilation defines are  */
/*    used as for the network layer module NM_LAYER.C.                  */
/*    One of the following defines can be used to skip Stadi access:    */
/*    - NM_1VB_SIMPLE: there is just 1 VB and no TB, simple routing     */
/*    - NM_1TB       : there is 1 TB and no VB, same as simple routing  */
/*    - NM_GW_SIMPLE : there is 1 VB and 1 TB, simple routing           */
/************************************************************************/

#define DU_MODULE_NAME "AM_CALL"
/* for hamster calls which are now mapped to JDP event recorder.
   DU_MODULE_NAME must be defined before "du_sys.h" is included.
*/

/* constructed configuration define for conditional compilation: */
/* <ATR:01> */
#if defined(O_960) || defined(__QNX__)
/* changed syntax of compiler switches (deleted: defined () ) */
#define NM_SIMPLE   (NM_1VB_SIMPLE || NM_GW_SIMPLE || NM_1TB)
#else
#define NM_SIMPLE   (defined ( ## NM_1VB_SIMPLE ## ) || defined ( ## NM_GW_SIMPLE ## ) || \
                     defined ( ## NM_1TB ## ))
#endif

/* INCLUDES: ---------------------------------------------------------- */

#if !NM_SIMPLE
#include "nm_stadi.h"
#endif

#if defined (O_MD_STDA)
#define SHRT_MAX 0x7FFF
#define NULL     0
#else
#include <stdio.h>      /* NULL */
#include <limits.h>     /* SHRT_MAX */
#endif

#if defined (O_PC) || defined (O_SUN)
#include "stdlib.h"
#include "string.h"
#endif

#include "pi_sys.h"
#include "md_ifce.h"
#include "am_sys.h"
#include "basetype.h"
#include "md_layer.h"
#include "nm_layer.h"
#include "tm_layer.h"

#include "am_sub.h"
/* some types and functions are renamed with am_ prefix and exported
   because they are also used for the multicast protocol
*/


/* CONSTANTS, MACROS -------------------------------------------------- */

#if defined (O_EKR) || defined (O_PC)
#define HUGE      huge
#else
#define HUGE
#endif


#define NO_CONFIRM              (AM_MAX_ERR + 1)
/* see close_call, the messenger never returns an error > AM_MAX_ERR */

#define COPY_BLOCK_SIZE       512
/* maximal number of bytes to copy before releasing the processor which
   allows other processes to run */
#define CANCEL_RETRY_CYC        1
/* cycle time in ticks which is awaited before a cancel operation which
   failed due to glitches is internally retried */


#if defined (O_RP) || defined (O_KHW)
#define BORDER_REG  (* (unsigned char *) 0xCC00)

#define CALL_CONFIRM(proc)  {\
        unsigned char border = BORDER_REG; \
        BORDER_REG           = BC_BORDER_DEFAULT; \
        proc; \
        BORDER_REG           = border; \
      }
#else
#define CALL_CONFIRM(proc)  {\
        proc; \
      }
#endif
/* !!! On the RPII & KHWII hardware the border register must be changed
       before calling a user specified confirmation procedure           */

/* EH 02-Mar-1994: new Macro TICKS to convert a time from units [64ms]
   to tick units [PI_TICK]. Rounds up to the next higher value.
*/
#define TICKS( time) ((((unsigned long) time * 64) + PI_TICK - 1) / PI_TICK)


/* TYPES: ------------------------------------------------------------- */

/* the following declarations are renamed with am_ prefix and included
   from am_sub.h:
*/
#define CALL_STATES     AM_CALL_STATES
#define IDLE            AM_IDLE
#define CALLING         AM_CALLING
#define WAIT_REPLY      AM_WAIT_REPLY
#define RECEIVING       AM_RECEIVING
#define CLOSING         AM_CLOSING
#define ANY_CONF        AM_ANY_CONF
#define CALL_DESCR      AM_CALL_DESCR

/* PROTOTYPES: -------------------------------------------------------- */

/* the following declarations are renamed with am_ prefix and included
   from am_sub.h:
*/
#define new_call    am_new_call
#define close_call  am_close_call
#define c_send_conf am_c_send_conf
#define pend_mutex  am_pend_call_mutex
#define post_mutex  am_post_call_mutex

static AM_RESULT get_link_id ( unsigned char caller_fct,
                               const AM_ADDRESS * replier,
                               /* out */ TM_LINK_ID * link,
                               /* out */ unsigned char * local,
/* tc added EH 19-Jul-1994 */  /* out */ unsigned char * tc);
static AM_RESULT get_call( const TM_LINK_ID * link,
                           /* out */ AM_CALL_DESCR * * call );
static void find_call( const TM_LINK_ID * link,
                       /* out */ CALL_DESCR * * call );
static void find_call_from_appl (unsigned char caller_fct,
                                 const AM_ADDRESS *replier,
                                 /* out */ CALL_DESCR **call);
static void alloc_buffer (CALL_DESCR * call, CARDINAL32 in_msg_size,
                          AM_RESULT * err);
static void c_invoke_ind( const TM_LINK_ID * link,
                          void * * in_msg_addr, CARDINAL32 in_msg_size,
                          void * * in_hdr_addr, unsigned int * in_hdr_size,
                          TM_MSG_DESCR * canc, void * * param,
                          AM_RESULT * err, unsigned char caller_vehicle);
static void handle_no_reply( void * param );
static void own_poll (void);
static AM_RESULT get_int_reply
                         ( unsigned char replier_fct,
                           unsigned char caller_fct,
                           void * in_msg_addr, CARDINAL32 in_msg_size,
                           void * in_hdr_addr, unsigned int in_hdr_size,
                           void * * param);

/* VARIABLES: --------------------------------------------------------- */

/* counters for LME */

/* number of messages received by an application */
extern unsigned long am_messages_received;

/* number of messages sent by an application */
extern unsigned long am_messages_sent;

/* EH, 02-Mar-1994: 'def_reply_tmo' was of type unsigned int before */
static unsigned long def_reply_tmo; /* default reply timeout in ticks */
static unsigned int  max_calls;     /* allowed number of idle or open calls */
static unsigned int  nr_of_calls;   /* current number "   "   "   "     "   */

static short call_mutex; /* sema_id */
/* semaphore for mutual exclusive access to call lists, call descriptors
   and other global variables */
static CALL_DESCR * open_calls; /* list of open calls */
static CALL_DESCR * idle_calls; /* list of idle (=unused) call descriptors */
static int halt_condition;
/* initialization result, value != 0 forces a call to 'hamster' in
   'am_call_cancel' or 'am_call_request'. 'hamster' cannot be called
   during initialization! */

/* added 02-Mar-1994: a PIL timeout handler is not allowed to call
   'pi_pend_semaphore', therefore a queue has been added to pass the job
   'close_call' to a poll function that is attached to the messenger task.
   A chaining strategy must be obeyed for the poll function calling
   sequence as described in module 'tm_layer.h', fct. 'tm_define_ext_poll'.
*/
static TM_POLL_FCT  old_poll;   /* previous poll function in chain */
static short        tmo_queue;  /* PIL queue id for reply timeout messages. */
/* A message in 'tmo_queue' is a reference to a call descriptor, which is
   also used as timeout parameter.
*/

static unsigned char current_tc = AM_ANY_TC; /* added EH 19-Jul-1994 */
/* current topo count as indicated by an application, call requests with
   another tc are rejected (except AM_ANY_TC). Read by 'get_link_id',
   written by 'am_set_current_tc'. Initialised to have no function.
*/

/* FUNCTIONS: --------------------------------------------------------- */

void pend_mutex (void)
{
    short result;

    pi_pend_semaphore (call_mutex, PI_FOREVER, &result);
}

void post_mutex (void)
{
    short result;

    pi_post_semaphore (call_mutex, &result);
}

/**********************************************************************/

AM_RESULT get_link_id( unsigned char caller_fct,
                       const AM_ADDRESS * replier,
                       /* out */ TM_LINK_ID * link,
                       /* out */ unsigned char * local,
                       /* out */ unsigned char * tc)
/* the out parameters are only valid if the function returns 'AM_OK'.
   'tc' returns the topo count to use for this call, added EH 19-Jul-1994.
   'tc' is AM_ANY_TC for local calls, else it is replier.tc, or "my_tc" if
   replier.tc is AM_ANY_TC (because comparison with "my_vehicle" depends
   on "my_tc").
   'local' means that the replier is on the same device, logical addr.
   is always used in this case.
   'link' always contains 'AM_SAME_VEHICLE' instead of "my_vehicle"
   and "my_device" instead of 'AM_SAME_DEVICE'.
   Returned error codes:
   - 'AM_DEST_NOT_REG_ERR' if there is no entry in the station or function
     directory table;
   - 'AM_ADDR_FMT_ERR' if 'replier' is not valid or if another caller
     than the manager function requires physical addressing;
   - 'AM_MY_DEV_UNKNOWN_ERR' if "my_device" = 'AM_UNKNOWN_DEVICE';
   - 'AM_INAUG_ERR' if replier.tc != current_tc & none of both is AM_ANY_TC;
   - 'AM_FAILURE' if replier->tc is out of range (> AM_MAX_TC);
*/
{
unsigned char find_fct, my_device;
unsigned char my_veh, my_tc;            /* EH 19-Jul-1994 */
unsigned char next_station, is_phys;    /* EH 04-Aug-1994 */

  hamster_halt (halt_condition);
      /*$H.Error creating semaphore */

  if (replier->tc > AM_MAX_TC) {
    return AM_FAILURE;
  }
  /* EH 19-Jul-1994, check against 'current_tc': */
  if ((replier->tc != AM_ANY_TC) && (current_tc != AM_ANY_TC) &&
      (replier->tc != current_tc)) {
    return AM_INAUG_ERR;
  }
  my_device = md_get_my_device ();
  if (my_device == AM_UNKNOWN_DEVICE)
  {
    return AM_MY_DEV_UNKNOWN_ERR;
  }
  link->vehicle = (*replier).vehicle;
  md_get_my_veh_and_tc (&my_veh, &my_tc);  /* EH 19-Jul-1994*/
  if (my_veh != AM_SAME_VEHICLE)
  {
    if (((*replier).vehicle & ~AM_PHYS_ADDR) == my_veh)
    {
      if ((replier->tc == AM_ANY_TC) || (replier->tc == my_tc))
      { /* condition added EH 19-Jul-1994*/
	link->vehicle
          = (((*replier).vehicle & AM_PHYS_ADDR) | AM_SAME_VEHICLE);
      } /* if */
    } /* if */
  } /* if */
  *local = ((link->vehicle & ~AM_PHYS_ADDR)
            == AM_SAME_VEHICLE);
  if (*local) {  /* EH 19-Jul-1994*/
    *tc = AM_ANY_TC;
  } else if (replier->tc != AM_ANY_TC) {
    *tc = replier->tc;
  } else {
    *tc = my_tc;
  } /* if (*local) */

  /* EH 04-Aug-1994, major mod. for new AM_ADDRESS containing next station: */
  is_phys = replier->vehicle & AM_PHYS_ADDR;
  if (is_phys) {
    if (caller_fct != AM_MANAGER_FCT) {
      return AM_ADDR_FMT_ERR;
    }
    link->my_fct_or_dev = my_device;
  } else {
    link->my_fct_or_dev = caller_fct;
  }

  /* get 'next_station', use fct. and station directory tables if unknown */
  next_station =
    (replier->next_sta == AM_SAME_DEVICE) ? my_device : replier->next_sta;
  if (next_station == AM_UNKNOWN_DEVICE) {
    if (*local && is_phys) {
      next_station = (replier->fct_or_sta != AM_UNKNOWN_DEVICE)
                     ? replier->fct_or_sta : my_device;
    } else {
      if (*local) {
        find_fct = replier->fct_or_sta; /* is a function */
      } else {
        find_fct = AM_ROUTER_FCT;
      }
      am_get_dir_entry (find_fct, &next_station);
      if (next_station == AM_UNKNOWN_DEVICE) {
        return AM_DEST_NOT_REG_ERR;
      }
    } /* if (*local && is_phys) */
    if (next_station == AM_SAME_DEVICE) {
      next_station = my_device;
#if !NM_SIMPLE
    } else if (next_station != my_device) {
      next_station = nm_st_get_next (next_station);
      if (next_station == AM_UNKNOWN_DEVICE) {
        return AM_DEST_NOT_REG_ERR;
      }
#endif
    } /* if (next_station == AM_SAME_DEVICE) */
  } /* if (next_station == AM_UNKNOWN_DEVICE) */

  link->fct_or_dev = replier->fct_or_sta;
  if (*local) {
    *local = (next_station == my_device);
    if (*local && is_phys) {
      if ((replier->fct_or_sta == my_device) ||
          (replier->fct_or_sta == AM_SAME_DEVICE) ||
          (replier->fct_or_sta == AM_UNKNOWN_DEVICE))
      { /* always use logical addressing in case of shortcut */
	link->vehicle = AM_SAME_VEHICLE;
	link->fct_or_dev = AM_SERVICE_FCT;
        link->my_fct_or_dev = AM_MANAGER_FCT;
      } else {
        return AM_ADDR_FMT_ERR;
      }
    } /* if (*local && is_phys) */
  } /* if (*local) */
  link->link_dev = next_station;
  return AM_OK;
} /* get_link_id */

/**********************************************************************/

void alloc_buffer (CALL_DESCR * call, CARDINAL32 in_msg_size, AM_RESULT * err)
/* Allocates a buffer for the reply message if the caller did not supply
   a buffer. Checks if the required 'in_msg_size' is acceptable.
   Modifies the fields 'in_msg_addr' and 'my_buffer' of the call descriptor
   in case of successful memory allocation, and modifies 'in_msg_size' =
   'in_msg_size' if 'err' is returned as 'AM_OK'. */
{
  *err = AM_OK;
  if (call->in_msg_size >= in_msg_size)
  {
    if ((call->in_msg_addr == NULL) && (in_msg_size != 0))
    {
      if (in_msg_size > SHRT_MAX)
      {
        *err = AM_NO_REM_MEM_ERR;
      }
      else
      {
        if ((call->in_msg_addr = pi_alloc (in_msg_size)) != NULL)
        {
          call->in_msg_size = in_msg_size;
          call->my_buffer = !0;
        }
        else
        {
          *err = AM_NO_REM_MEM_ERR;
        }
      } /* if (in_msg_size > SHRT_MAX) */
    }
    else
    {
      call->in_msg_size = in_msg_size;
    } /* if (call->in_msg_addr != NULL) */
  }
  else
  {
    *err = AM_REPLY_LEN_OVF;
  } /* if (call->in_msg_size >= in_msg_size) */
} /* alloc_buffer */

/* some access to call lists: ********************************************/

AM_RESULT get_call( const TM_LINK_ID * link,
                    /* out */ CALL_DESCR * * call )
/* precondition: 'call_mutex' must be awaited before this procedure is called!
   Checks if there is already an open call with the same 'link'.
   Removes an entry from the list of idle calls or allocates a new call descr.
   Initializes the 'reply_tmo' field in the descriptor to the default value,
   the 'call_state' field to 'CALLING', 'my_buffer' to 0, 'canc_ref' to NULL
   and the 'link' field.
   Inserts the call descriptor into the list of open calls.
   May return a function result != 'AM_OK' together with 'call' == NULL. */
{
    AM_RESULT    error;
    CALL_DESCR * new_call;

    error = AM_OK;
    find_call (link, &new_call);
    if (new_call != NULL)
    {
        error = AM_DUPL_LINK_ERR;
    }
    else if (idle_calls != NULL)
    {
        new_call = idle_calls;
        idle_calls = idle_calls->next;
    }
    else if (nr_of_calls < max_calls)
    {
        new_call = (CALL_DESCR *) pi_alloc (sizeof(CALL_DESCR));
        if (new_call != NULL)
        {
            if (pi_create_timeout (&(new_call->reply_timer), handle_no_reply,
                                     new_call, 1) != PI_RET_OK)
            {
                pi_free ((void*)new_call);
                error = AM_NO_LOC_MEM_ERR;  /* EH 19-Jul-1994, was AM_FAILURE */
            }
            else
            {
                nr_of_calls++;
            }
        }
        else
        {
            error = AM_NO_LOC_MEM_ERR;
        }
    }
    else
    {
        error = AM_NR_OF_CALLS_OVF;
    }
    *call = NULL;
    if (error == AM_OK)
    {
        new_call->reply_tmo = def_reply_tmo;
        new_call->call_state = CALLING;
        new_call->link = *link;
        new_call->my_buffer = 0;
        new_call->canc_ref = NULL;
        new_call->next = open_calls;
        *call = open_calls = new_call;
    }
    return error;
}

/**********************************************************************/

AM_RESULT new_call (unsigned char caller_fct, const AM_ADDRESS *replier,
                    AM_CALL_DESCR * * call, unsigned char *local)
/* precondition: 'call_mutex' must be awaited before this procedure is called!
   Calls 'get_link_id' to build a link identification and 'get_call' to
   instantiate a call. Initializes additionally the fields 'caller_fct',
   'replier' (used for cancel) and 'caller_vehicle' in the call descriptor.
   May return a function result != 'AM_OK' together with 'call' == NULL.
   Added EH 19-Jul-1994 to simplify interface to multicast 'mc_trans.c'.
*/
{
unsigned char tc;
TM_LINK_ID    link;
AM_RESULT      err;

  err = get_link_id (caller_fct, replier, &link, local, &tc);
  if (err == AM_OK) {
    err = get_call (&link, call);
    if (*call != NULL) {
      (*call)->caller_fct = caller_fct;
      (*call)->replier = *replier;
      (*call)->caller_vehicle = tc;
    }
  } else {
    *call = NULL;
  }
  return err;
} /* new_call */

/**********************************************************************/

void close_call (void * call, AM_RESULT error)
/* Closes a call. 'call' is the reference to the call descriptor.
   Called when the complete reply message has been received, therefore
   'call' is of type void * instead of CALL_DESCR *.
   Removes from list of open calls, inserts into list of idle calls and
   conditionally if 'error' != NO_CONFIRM calls the confirmation procedure.
   Reads 'in_msg_addr' and 'my_buffer' fields of the call descriptor,
   and conditionally if 'error' == AM_OK also 'result' and 'in_msg_size'.
   Confirms with 0 as reply message size if 'error' != AM_OK.
   Deallocates the reply buffer if {'error' != AM_OK and 'my_buffer' != 0}
   and confirms with NULL as reply message address in this case. */
{
CALL_DESCR  * find;
AM_ADDRESS    replier;
unsigned char caller_fct;
AM_RESULT     err;       /* returned with confirmation */
void        * addr, * ref;
CARDINAL32    len;
ANY_CONF      proc;
unsigned char call_mode; /* boolean */

  replier.tc = ((CALL_DESCR *)call)->caller_vehicle; /* EH 19-Jul-1994 */
  replier.vehicle = ((CALL_DESCR *)call)->link.vehicle;
  /* EH 04-Aug-1994, new AM_ADDRESS format containing next station: */
  replier.fct_or_sta = ((CALL_DESCR *)call)->link.fct_or_dev;
  replier.next_sta = ((CALL_DESCR *)call)->link.link_dev;
  caller_fct = ((CALL_DESCR *)call)->caller_fct;
  /*
  if (AM_PHYS_ADDR & replier.vehicle)
  {
    replier.function = AM_SERVICE_FCT;
    caller_fct = AM_MANAGER_FCT;
    replier.device = ((CALL_DESCR *)call)->link.fct_or_dev;
  }
  else
  {
    replier.function = ((CALL_DESCR *)call)->link.fct_or_dev;
    caller_fct = ((CALL_DESCR *)call)->link.my_fct_or_dev;
    if (replier.vehicle == AM_SAME_VEHICLE)
    {
      replier.device = ((CALL_DESCR *)call)->link.link_dev;
    }
    else
    {
      replier.device = AM_UNKNOWN_DEVICE;
    }
  }
  */
  ref = ((CALL_DESCR *)call)->ext_ref;
  proc = ((CALL_DESCR *)call)->conf_proc;
  call_mode = ((CALL_DESCR *)call)->msg_hdr;
  addr = ((CALL_DESCR *)call)->in_msg_addr;
  if (error == AM_OK)
  {
    err = ((CALL_DESCR *)call)->result;
    len = ((CALL_DESCR *)call)->in_msg_size;
  }
  else
  {
    err = error;
    len = 0;
    if (((CALL_DESCR *)call)->my_buffer)
    {
      pi_free (addr);
      addr = NULL;
    }
  }
  pend_mutex ();
  if (open_calls == ((CALL_DESCR *)call))
  {
    open_calls = ((CALL_DESCR *)call)->next;
  }
  else
  {
    for (find = open_calls; ; find = find->next)
    {
      hamster_halt (find == NULL);
	/*$H.close_call: call descriptor not found in list of open calls */

      if (find->next == ((CALL_DESCR *)call))
      {
        find->next = ((CALL_DESCR *)call)->next;
        break;
      } /* if */
    } /* for */
  } /* if */
  ((CALL_DESCR *)call)->next = idle_calls;
  idle_calls = ((CALL_DESCR *)call);
  post_mutex ();
  if (error != NO_CONFIRM)
  {
    if (err == AM_OK)
    {
       /* number of successfull calls sent by an application */
			 am_messages_received = ((am_messages_received+1) % 0xFFFFFFFFL);
    }
    if (call_mode == AM_NO_REPLY)
    {
      CALL_CONFIRM (proc.invoke (caller_fct, ref, &replier, err));
    }
    else
    {
      CALL_CONFIRM (proc.call (caller_fct, ref, &replier, addr, len, err));
    }
  }
} /* close_call */

/**********************************************************************/

void find_call( const TM_LINK_ID * link, /* out */ CALL_DESCR * * call )
/* precondition: 'call_mutex' must be awaited before this procedure is called!
   Finds the open call with the specified link identification.
   NULL may be returned. */
{
  *call = open_calls;
  while( *call != NULL )
  {
    /* !!! The following comparison is not portable, but asserted by
       'am_system_init_caller'. */
    if( *(CARDINAL32 *)&(*call)->link == *(CARDINAL32 *)link )
    {
      return;
    }
    else
    {
      *call = (*call)->next;
    }
  } /* while */
} /* find_call */

/**********************************************************************/

static void find_call_from_appl (unsigned char caller_fct,
                                 const AM_ADDRESS *replier,
                                 /* out */ CALL_DESCR **call)
/* precondition: 'call_mutex' must be awaited before this procedure is called!
   Finds the open call with the specified caller_fct and replier.
   NULL may be returned. */
{
  *call = open_calls;
  while (*call != NULL)
  {
    if (((*call)->caller_fct == caller_fct) &&
        ((*call)->replier.vehicle == replier->vehicle) &&
        ((*call)->replier.fct_or_sta == replier->fct_or_sta) &&
        ((*call)->replier.next_sta == replier->next_sta) &&
        ((*call)->replier.tc == replier->tc))
    {
      return;
    }
    else
    {
      *call = (*call)->next;
    }
  } /* while */
} /* find_call_from_appl */

/* connected as notification procedures: *********************************/

void c_send_conf( void * param, AM_RESULT err, unsigned char caller_vehicle )
/* Called on completion of a call message transmission.
   'param' is the reference to the call descriptor.
   If 'err' != 'AM_OK' then {closes the call} else {updates the call state
   to 'WAIT_REPLY', remembers the 'caller_vehicle' in the call descriptor,
   clears 'canc_ref' to NULL and starts the reply timeout}. */
{
  if ((err == AM_OK)
      && (((CALL_DESCR *)param)->msg_hdr == AM_PLEASE_REPLY))
  {
    pend_mutex ();
    ((CALL_DESCR *)param)->canc_ref = NULL;
    ((CALL_DESCR *)param)->call_state = WAIT_REPLY;
    pi_enable_timeout (((CALL_DESCR *)param)->reply_timer,
                       ((CALL_DESCR *)param)->reply_tmo);
    /* EH 19-Jul-1994, mask and conditional assignment: */
    caller_vehicle &= ~AM_PHYS_ADDR;
    if (((CALL_DESCR *)param)->caller_vehicle == AM_ANY_TC) {
      ((CALL_DESCR *)param)->caller_vehicle = caller_vehicle;
    } else {
      hamster_halt (caller_vehicle != ((CALL_DESCR *)param)->caller_vehicle);
    }
    post_mutex ();
  }
  else
  {
    ((CALL_DESCR *)param)->result = err;
    close_call( param, err );
  } /* if */
} /* c_send_conf */

/**********************************************************************/

void c_invoke_ind (const TM_LINK_ID * link,
                   void * * in_msg_addr, CARDINAL32 in_msg_size,
                   void * * in_hdr_addr, unsigned int * in_hdr_size,
                   TM_MSG_DESCR * canc, void * * param,
                   AM_RESULT * err, unsigned char caller_vehicle)
/* Called on reception of an invoke packet from a replier.
   The reply is rejected if either there is no open call for this link,
   or the 'caller_vehicle' is not the same as in the call descriptor
   (inauguration), or the call is not in state 'WAIT_REPLY', or the reply
   is longer than acceptable by the caller, or the caller did not supply a
   buffer and there is not enough memory. An open call is closed if a reply
   to the right 'caller_vehicle' is rejected in state 'WAIT_REPLY'.
   If the reply is accepted then the call state is updated to 'RECEIVING',
   'in_msg_size' as well as 'canc' are stored in the call descriptor
   and 'param' is returned as reference to the call descriptor. */
{
CALL_DESCR * call;

  *err = AM_OK;
  pend_mutex ();
  find_call( link, &call );
  if (call != NULL)
  {
    caller_vehicle &= ~AM_PHYS_ADDR; /* EH 19-Jul-1994 */
    if (call->caller_vehicle == caller_vehicle)
    {
      if (call->call_state == WAIT_REPLY)
      {
        if (pi_disable_timeout (call->reply_timer) == PI_RET_OK)
        {
          call->call_state = RECEIVING;
          *in_hdr_addr = &(call->result);
          *in_hdr_size = sizeof (AM_RESULT);
          alloc_buffer (call, in_msg_size, err);
          if (*err == AM_OK)
          {
            call->canc_ref = canc;
          }
          *param = call;
          *in_msg_addr = call->in_msg_addr;
        }
        else /* the timeout process will close the call */
        {
          *err = AM_NO_REPLY_EXP_ERR;
          call = NULL;
        } /* if (pi_disable_timeout (call->reply_timer) == PI_RET_OK) */
      }
      else
      {
        *err = AM_TRY_LATER_ERR;
        call = NULL;
      } /* if (call->call_state == WAIT_REPLY) */
    }
    else
    {
      *err = AM_INAUG_ERR;
      call = NULL;
    } /* if (call->caller_vehicle == caller_vehicle) */
  }
  else
  {
    *err = AM_NO_REPLY_EXP_ERR;
  } /* if (call != NULL) */
  post_mutex ();
  if ((*err != AM_OK) && (call != NULL))
  {
    close_call (call, (*err == AM_NO_REM_MEM_ERR) ? AM_NO_LOC_MEM_ERR : *err);
  }
} /* c_invoke_ind */

/**********************************************************************/
/* added EH 22-Jun-1994: unconditional early release */
void c_rcv_ind (void * call, AM_RESULT error, unsigned char *release)
{
  *release = !0;
  close_call (call, error);
} /* c_rcv_ind */

/**********************************************************************/

void handle_no_reply( void * param )
/* Called on reply timeout by the timeout process.
   Inserts message 'param' into 'tmo_queue', does not close the call
   immediately because 'close_call' pends for a semaphore which is
   not an allowed operation for a timeout handler (changed 02-Mar-1994).
   'param' is the reference to the call descriptor. */
{
short status;

  pi_send_queue (tmo_queue, (char*) param, &status);
  hamster_halt (status != PI_RET_OK);
} /* handle_no_reply */

/**********************************************************************/
/* Function added 02-Mar-1994: */

static void own_poll (void)
/* Closes all calls for which the reply timeout elapsed. A reference to
   these calls has been inserted as message into the 'tmo_queue' by the
   timeout handler function 'handle_no_reply'.
   This function is connected to the tm_messenger task as poll function,
   because there is no own task in this layer. Therefore 'old_poll' is
   called before the own task is done as required by the poll chaining
   strategy.
*/
{
short     status;
void     *param;

  old_poll ();
  for (;;) {
    param = pi_accept_queue (tmo_queue, &status);
    if (status == PI_RET_OK) {
      close_call( param, AM_REPLY_TMO_ERR);
    } else {
      break;
    }
  } /* for */
} /* own_poll */

/**********************************************************************/

AM_RESULT get_int_reply( unsigned char replier_fct,
                         unsigned char caller_fct,
                         void * in_msg_addr, CARDINAL32 in_msg_size,
                         void * in_hdr_addr, unsigned int in_hdr_size,
                         void * * param)
/* Called through 'tm_put_reply' by an internal replier to return a reply
   message. The reply is rejected if either there is no open call for the
   related link identification, or the call is not in state 'WAIT_REPLY', or
   the reply is longer than acceptable by the caller, or the caller did not
   supply a buffer and there is not enough memory. An open call is closed if
   a reply is rejected in state 'WAIT_REPLY'.
   If the reply is accepted then the reply message is copied to the caller's
   buffer and a reference to the call descriptor is returned as 'param'. */
{
CALL_DESCR * call;
TM_LINK_ID   link;
short        last_size;
unsigned char *dest, *src;
AM_RESULT    err;

  /* build the related link identification: */
  link.link_dev = md_get_my_device();
  /* EH 27-Jul-1994, always use logical addressing for shortcut:
  if (replier_fct == AM_SERVICE_FCT)
  {
    if (caller_fct != AM_MANAGER_FCT)
    {
      return AM_ADDR_FMT_ERR;
    }
    link.my_fct_or_dev = link.link_dev;
    link.fct_or_dev = link.my_fct_or_dev;
    link.vehicle = (AM_SAME_VEHICLE | AM_PHYS_ADDR);
  }
  else
  */
  {
    link.my_fct_or_dev = caller_fct;
    link.fct_or_dev = replier_fct;
    link.vehicle = AM_SAME_VEHICLE;
  } /* if */

  /* find the call descriptor and update its state to 'RECEIVING'.
     Do not lock 'call_mutex' during the message copy operation! */
  err = AM_NO_REPLY_EXP_ERR;
  pend_mutex ();
  find_call (&link, &call);
  if (call != NULL)
  {
    if (call->call_state == WAIT_REPLY)
    {
      if (pi_disable_timeout (call->reply_timer) == PI_RET_OK)
      {
        call->call_state = RECEIVING;
        if ((in_hdr_size == sizeof (AM_RESULT)) && (in_hdr_addr != NULL)
            && !((in_msg_addr == NULL) && (in_msg_size != 0)))
        {
          call->result = *(AM_RESULT *)in_hdr_addr;
          err = AM_OK;
        }
        else
        {
          err = AM_FAILURE;
        } /* if (in_hdr_size == sizeof (AM_RESULT)) */
      }
      else /* the timeout process will close the call */
      {
        call = NULL;
      } /* if (pi_disable_timeout (call->reply_timer) == PI_RET_OK) */
    }
    else
    {
      call = NULL;
    } /* if (call->call_state == WAIT_REPLY) */
  } /* if (call != NULL) */
  post_mutex ();

  /* conditionally allocate a buffer for the reply message, copy and close: */
  if (err == AM_OK)
  {
    alloc_buffer (call, in_msg_size, &err);
  }
  if (err == AM_OK)
  {
    last_size = in_msg_size % COPY_BLOCK_SIZE;
    dest = (unsigned char *) call->in_msg_addr;
    src = (unsigned char *) in_msg_addr;
    dest += 0; /* assignment does not normalize, so force normalization */
    src  += 0; /* " */
    for (in_msg_size /= COPY_BLOCK_SIZE; in_msg_size > 0; in_msg_size--)
    {
      pi_copy8 ((void *) dest, (void *) src, COPY_BLOCK_SIZE);
      src = src + COPY_BLOCK_SIZE;
      dest = dest + COPY_BLOCK_SIZE;
      tm_delay (1);
    }
    if (last_size != 0)
    {
      pi_copy8 ((void *) dest, (void *) src, last_size);
    }
  } /* if (err == AM_OK) */
  if (call != NULL)
  {
    if (err != AM_OK)
    {
      close_call (call, (err == AM_NO_REM_MEM_ERR) ? AM_NO_LOC_MEM_ERR : err);
    }
  }
  *param = call;
  return err;
} /* get_int_reply */

/* global functions **********************************************************/

void am_init_caller( unsigned int reply_tmo, unsigned int nr_of_calls )
/* defines the default value for the reply timeout and the maximum number of
   simultaneously open calls.
   Extended 02-Mar-1994: creates 'tmo_queue' and connects function
   'own_poll' as poll function to the messenger task. 'reply_tmo' is given
   in units [64ms] and converted to tick units [PI_TICK].
*/
{
short status;

  tmo_queue = pi_create_queue (nr_of_calls, PI_ORD_FCFS, &status);
  hamster_halt (status != PI_RET_OK);
  hamster_halt (tm_define_ext_poll (own_poll, &old_poll) != AM_OK);
  pend_mutex ();
  def_reply_tmo = TICKS( reply_tmo);   /* EH, 02-Mar-1994: TICKS */
  max_calls = nr_of_calls;
  post_mutex ();
} /* am_init_caller */

/**********************************************************************/

/* @JDP
void _am_call_requ
@JDP */
void _am_send_requ /* @JDP */
                  (unsigned char caller_fct,
                   const AM_ADDRESS * replier,
                   void * out_msg_addr,         /* call message */
                   CARDINAL32 out_msg_size,
                   void * in_msg_addr,          /* reply message */
                   CARDINAL32 in_msg_size,
                   unsigned int reply_tmo,      /* 0 if default */
                   AM_CALL_CONF confirm,
                   AM_INVOKE_CONF invoke_conf,  /* @JDP */
                   void * ext_ref)
/* Gets the link identification for 'caller_fct' and 'replier', checks
   if there is already an open call with the same link and gets an idle
   call descriptor. If the replier is not internal (not on the same device)
   then requests call message transmission. Otherwise the call message is
   directly put to the replier (shortcut), the reply timeout is started
   and the call's state is updated to 'WAIT_REPLY'.
   EH 02-Mar-1994: 'reply_tmo' is given in units [64ms] and converted to
   tick units [PI_TICK].
*/
{
CALL_DESCR *  call;
unsigned char short_cut, do_post; /* boolean */
unsigned char no_reply; /* @JDP boolean */
AM_RESULT     err;
/* TM_LINK_ID    link; EH 19-Jul-1994 */
void *        param;

	am_messages_sent = ((am_messages_sent+1) % 0xFFFFFFFFL); /* Fl 95.10.05 */
	err = AM_OK;
	/* EH 19-Jul-1994: new_call instead of get_link_id, get_call */
	/* err = get_link_id (caller_fct, replier, &link, &short_cut); */
	no_reply = (invoke_conf != (AM_INVOKE_CONF) NULL);    /* @JDP */
	if (confirm != (AM_CALL_CONF) NULL)                   /* @JDP */
  {                                                     /* @JDP */
    if (no_reply)                                       /* @JDP */
    {                                                   /* @JDP */
      err = AM_FAILURE;                                 /* @JDP */
    }                                                   /* @JDP */
  }                                                     /* @JDP */
  else                                                  /* @JDP */
  {                                                     /* @JDP */
    if (!no_reply)                                      /* @JDP */
    {                                                   /* @JDP */
      return;                                           /* @JDP */
    }                                                   /* @JDP */
  }                                                     /* @JDP */
  if (err == AM_OK)
  {
    pend_mutex ();
    do_post = !0;
    if ((err = new_call (caller_fct, replier, &call, &short_cut)) == AM_OK)
    {
      if (reply_tmo != 0)
      {
        call->reply_tmo = TICKS (reply_tmo);  /* EH, 02-Mar-1994: TICKS */
      }
      call->ext_ref = ext_ref;
      if (no_reply)                            /* @JDP */
      {                                        /* @JDP */
        call->conf_proc.invoke = invoke_conf;  /* @JDP */
        call->msg_hdr = AM_NO_REPLY;           /* @JDP */
      }                                        /* @JDP */
      else                                     /* @JDP */
      {                                        /* @JDP */
        call->conf_proc.call = confirm;
        call->in_msg_addr = in_msg_addr;
        call->in_msg_size = in_msg_size;
        call->msg_hdr = AM_PLEASE_REPLY;
      } /* if (no_reply) */                    /* @JDP */
      if ( ! short_cut )
      {
        err = tm_send_msg_requ (TM_CALLER_USER, &(call->link),
                                out_msg_addr, out_msg_size,
                                &call->msg_hdr, sizeof (call->msg_hdr),
                                call, &(call->canc_ref),
              /* EH 19-Jul-1994 AM_SAME_VEHICLE */ call->caller_vehicle,
/* @JDP EH 22-Jun-1994 */       no_reply ? TM_MODE_PREVENT_DUPL :
                                TM_MODE_NORMAL);
      }
      else
      {
        /* do not lock the semaphore while copying, prevent deadlocks ! */
        post_mutex ();
        do_post = 0;
        /* EH 04-Aug-1994, ...fct_or_dev always is a function if local! */
	err = tm_put_call (caller_fct, call->link.fct_or_dev,
                           out_msg_addr, out_msg_size,
                           &call->msg_hdr, sizeof (call->msg_hdr), &param);
        if (err == AM_OK)
        {
          if (!no_reply)     /* @JDP */
          {                  /* @JDP */
            pend_mutex ();
            pi_enable_timeout (call->reply_timer, call->reply_tmo);
            call->call_state = WAIT_REPLY;
            post_mutex ();
          }                  /* @JDP */
          tm_deliver_call (param, AM_OK);
        }
      } /* if ( ! short_cut ) */
    } /* if ((err = new_call (...)) == AM_OK) */
    if (do_post)
    {
      post_mutex ();
    }
  }
  else
  {
    call = NULL;
  } /* if (err == AM_OK) */
/* @JDP
  if (err != AM_OK)
@JDP */
  if ((err != AM_OK) || (no_reply && short_cut))            /* @JDP */
  {
    if (call != NULL)
    {
      call->result = err;
      close_call (call, err);
    }
    else if (no_reply)                                      /* @JDP */
    {                                                       /* @JDP */
      invoke_conf  (caller_fct, ext_ref, replier, err);     /* @JDP */
    }                                                       /* @JDP */
    else
    {
      confirm (caller_fct, ext_ref, replier, in_msg_addr, 0L, err);
    }
  } /* if (err != AM_OK) */
} /* _am_call_requ */

/**********************************************************************/

void _am_call_cancel (unsigned char caller_fct,
                      const AM_ADDRESS * replier,
                      AM_RESULT * status)
/* Gets the link identification for 'caller_fct' and 'replier' and checks
   if there is an open call with this link. If the 'call_state' ==
   'WAIT_REPLY' then cancels the timeout, else if the call's 'canc_ref' !=
   NULL then cancels message communication. If one of these cancel operations
   is successful then closes the call without confirmation, else (glitch,
   the call cannot be closed because the call descriptor will be accessed
   unconditionally by another process) tries again later after a delay.
   EH 19-Jul-1994, modified:
   does not search a call by the link id, but by caller_fct and replier
   instead. This modification is necessary because conversion to a link id
   is time dependant, an intermediate inauguration does not lead to the
   same link id for identical calling parameters. So a call issued before
   an inauguration could possibly not be cancelled afterwards if the own
   vehicle number changed! Caller_fct and replier must therefore be saved
   in the call descriptor.
*/
{
unsigned char locked; /* boolean */
CALL_DESCR  * call;

  *status = AM_FAILURE;
  /* if (get_link_id( caller_fct, replier, &link, &dummy ) == AM_OK) */
  {
    locked = 0;
    do
    {
      if (locked)
      {
	tm_delay (CANCEL_RETRY_CYC);
	locked = 0;
      }
      pend_mutex ();
      find_call_from_appl (caller_fct, replier, &call);
      if (call != NULL)
      {
        if (call->call_state == WAIT_REPLY)
        {
          if (pi_disable_timeout (call->reply_timer) == PI_RET_OK)
          {
            *status = AM_OK;
            call->call_state = CLOSING;
          }
          else
          {
            locked = !0;
          }
        }
        else if (call->canc_ref != NULL)
        {
          if (tm_cancel_msg (call->canc_ref) != 0)
          {
            *status = AM_OK;
            call->canc_ref = NULL;
            call->call_state = CLOSING;
          }
          else
          {
            locked = !0;
          }
        }
        else
        {
          locked = !0;
        } /* if (call->call_state == WAIT_REPLY) */
      } /* if (call != NULL) */
      post_mutex ();
    }
    while (locked);
    if (*status == AM_OK)
    {
      close_call (call, NO_CONFIRM);
    }
  } /* if */
} /* _am_call_cancel*/

/**********************************************************************/

AM_RESULT _am_set_current_tc (unsigned char tc)  /* added EH 19-Jul-1994*/
{
  if (tc <= AM_MAX_TC) {
    current_tc = tc;
    return AM_OK;
  } else {
    return AM_FAILURE;
  }
} /* _am_set_current_tc */

/**********************************************************************/

void am_system_init_caller (void)
{
short  status;

  halt_condition = max_calls = nr_of_calls = 0;
  call_mutex = pi_create_semaphore (1, PI_ORD_FCFS, &status);
  if (status != PI_RET_OK)
  {
    halt_condition = 1;
  }
  idle_calls = open_calls = NULL;
  tm_define_user (TM_CALLER_USER, c_invoke_ind, c_rcv_ind, c_send_conf);
  tm_conn_get_reply (get_int_reply, close_call);
} /* am_system_init_caller */
