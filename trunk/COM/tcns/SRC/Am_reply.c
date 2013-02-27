/************************************************************************/
/*  Asea Brown Boveri Research Center, CH-5405 Baden, Switzerland       */
/*  Copyright 1991                                                      */
/*                                                                      */
/*  NAME:                                                               */
/*    AM_REPLY.C                                                        */
/*    The prototypes for the public functions are contained in the      */
/*    header module 'AM_SYS.H'.                                         */
/*                                                                      */
/*  ABSTRACT:                                                           */
/*    TCN Message Transport Protocol, Session layer for replier.        */
/*                                                                      */
/************************************************************************/
/*                                                                      */
/*  AUTHOR  :   16-May-1991     CRBC1   Hoepli                          */
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
/*              06-Feb-1992   CRBC1      Hoepli                         */
/*              22-Apr-1992   CRBC1    Hoepli     tm_deliver_xx added   */
/*              18-May-1992  CRBC1    Hoepli      r_invoke_ind changed  */
/*              28-Sep-1992  BAEI-2   Muenger     pend/post_mutex       */
/*              17-Nov-1992  CRBC1    Hoepli      am_get_replier_status */
/*              18-Nov-1993  CRBC1    Hoepli      am_sub.h generated    */
/*              24-Feb-1994  CRBC1    Hoepli      am_reply_requ hamster */
/*              07-Mar-1994   CRBC1    Hoepli     define DU_MODULE_NAME */
/*              14-Jun-1994   CRBC1    Hoepli     HUGE in get_int_call  */
/*              22-Jun-1994  CRBC1    Hoepli      early descr. release  */
/*              20-Jul-1994  CRBC1    Hoepli      topo count in am_ if  */
/*              03-Aug-1994  CRBC1    Hoepli      next_sta in AM_ADDRESS*/
/*              13-Feb-1995  BATL     Flum        inserted SWITCH O_SUN */
/*              15-Feb-1995  BATL     Flum        Switch O_MD_STDA to   */
/*                                                compile MD sources    */
/*                                                without C standard lib*/
/*              05-Oct-1995  BATL     Flum        inserted counters     */
/*                                                am_messages_received  */
/*                                                am_messages_sent      */
/************************************************************************/

#define DU_MODULE_NAME "AM_REPL"
/* for hamster calls which are now mapped to JDP event recorder.
   DU_MODULE_NAME must be defined before "du_sys.h" is included.
*/

#if defined (O_MD_STDA)
#define NULL     0
#define SHRT_MAX 0x7FFF
#else
#include <stdio.h>      /* NULL */
#include <string.h>     /* memcpy( ) */
#include <limits.h>     /* SHRT_MAX */
#endif

#if defined (O_PC) || defined (O_SUN)
#include "stdlib.h"
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


#define COPY_BLOCK_SIZE       512
/* maximal number of bytes to copy before releasing the processor which
   allows other processes to run */
#define CANCEL_RETRY_CYC        1
/* cycle time in ticks which is awaited before a cancel operation which
   failed due to glitches is internally retried */
#define SYNC_RETRY_CYC         1
/* cycle time in ticks which is awaited before a failed operation is
   internally retried to regard delays due to processor sharing
   (shortcut only). */

#if defined (O_RP) || defined (O_KHW)
#define BORDER_REG  (* (unsigned char *) 0xCC00)

#define RECV_CONFIRM(proc, rf, caller, in_msg, in_size, ref)  {\
        unsigned char border = BORDER_REG; \
        BORDER_REG           = BC_BORDER_DEFAULT; \
        proc (rf, caller, in_msg, in_size, ref); \
        BORDER_REG           = border; \
      }

#define REPL_CONFIRM(proc, rf, ref)  {\
        unsigned char border = BORDER_REG; \
        BORDER_REG           = BC_BORDER_DEFAULT; \
        proc (rf, ref); \
        BORDER_REG           = border; \
      }
#else
#define RECV_CONFIRM(proc, rf, caller, in_msg, in_size, ref)  {\
        proc (rf, caller, in_msg, in_size, ref); \
      }

#define REPL_CONFIRM(proc, rf, ref)  {\
        proc (rf, ref); \
      }
#endif
/* !!! On the RPII & KHWII hardware the border register must be changed
       before calling a user specified confirmation procedure           */


/* TYPES: ------------------------------------------------------------- */

/* states of a bind descriptor: */
enum BIND_STATES
{
  BOUND=AM_RS_BOUND,
  UNBOUND=AM_RS_UNBOUND,
  UNBINDING=AM_RS_UNBINDING
};
/* 'UNBINDING': the bind descriptor is still in use, but new references
   to it must not be created anymore.
   'UNBOUND'  : the bind descriptor is not in use (other fields invalid).
 */

typedef struct {
unsigned char replier_fct;
unsigned char bind_state; /* BIND_STATES */
AM_RCV_CONF   rcv_conf_proc;
AM_REPLY_CONF reply_conf_proc;
struct INST_DESCR  * active_inst;
/* list of active instances (ready or involved in a call) for this function */
} BIND_DESCR;


/* states of an instance (descriptor): */
enum INST_STATES {
IDLE,
READY=AM_RS_READY,
RECEIVING=AM_RS_RECEIVING,
IN_EXECUTION=AM_RS_RPC_INEXEC,
SENDING=AM_RS_SENDING,
CLOSING=AM_RS_CLOSING
};
/*
IDLE:          descriptor is not in use
READY:         instance is ready to accept an incoming call
RECEIVING:     call reception in progress, messenger or caller process
               got a reference
IN_EXECUTION:  call reception completed, outstanding reply request
SENDING:       reply transmission in progress, messenger or replier process
               got a reference
CLOSING:       the instance will be closed unconditionally by the one
               process that forced transition to this state

The one process that has got a reference to the instance descriptor is
allowed to access the descriptor and to close the call
*/


typedef struct INST_DESCR {
struct INST_DESCR * next;
BIND_DESCR        * bind_ref;
void              * ext_ref; /* external instance reference */
void              * msg_addr;
CARDINAL32          msg_size;
/* 'inst_state' == ['READY' | 'RECEIVING']: buffer for incoming call,
   'inst_state' == 'SENDING': buffer with outgoing reply message */
CARDINAL32          max_in_msg_size;
TM_MSG_DESCR      * canc_ref;
/* 'canc_ref' != NULL in states 'RECEIVING' or 'SENDING' only,
   reference to cancel message communication */
TM_LINK_ID          link;
enum INST_STATES    inst_state;
unsigned char       abort; /* disables reply transmission */
unsigned char       shortcut; /* internal call, caller is on same device */
unsigned char       my_buffer;
/* messenger allocated call buffer and buffer was not yet passed to user */
AM_RESULT           status; /* reply message header */
unsigned char       replier_vehicle;
unsigned char       msg_hdr; /* call message header */
} INST_DESCR;

/* PROTOTYPES: -------------------------------------------------------- */

/* the following declarations are renamed with am_ prefix and included
   from am_sub.h:
*/
#define r_invoke_ind    am_r_invoke_ind
#define r_rcv_ind       am_r_rcv_ind

static void cancel_inst (INST_DESCR * inst,
                         unsigned char /* boolean */ * cancelled);
static void find_bind_ref (unsigned char replier_fct,
                           unsigned char * bind_state,
                           BIND_DESCR * * bind_ref);
static void get_inst (/* out */ INST_DESCR * * inst, /* out */ AM_RESULT * err);
static void close_inst (INST_DESCR * inst, unsigned char do_confirm);
static void find_inst (const BIND_DESCR * bind_ref, const void * ext_ref,
                       /* out */ INST_DESCR * * inst);
static AM_RESULT ready_to_rcv_inst (unsigned char replier_fct,
                                    CARDINAL32 in_msg_size,
                                    TM_MSG_DESCR * canc_ref,
                                    const TM_LINK_ID * link,
                                    /* out */ INST_DESCR * * inst);
static void r_send_conf (void * param, AM_RESULT err,
                         unsigned char replier_vehicle);
static AM_RESULT get_int_call (unsigned char caller_fct,
                               unsigned char replier_fct,
                               void * in_msg_addr, CARDINAL32 in_msg_size,
                               void * in_hdr_addr, unsigned int in_hdr_size,
                               void * * param);

/* VARIABLES: --------------------------------------------------------- */


/* number of received messages by an application */
extern unsigned long    am_messages_received;

/* number of sent messages by an application */
extern unsigned long    am_messages_sent;

/* number of instances for all repliers */
extern unsigned char   am_instances;

static unsigned int    max_inst;        /* allowed number of instances */
static unsigned int    nr_of_inst;      /* current number of instances */

static short           inst_mutex;      /* sema_id */

/* semaphore for mutual exclusive access to instance lists, instance
   descriptors and other global variables */

static INST_DESCR   * idle_inst; /* list of unused instance descriptors */
static BIND_DESCR   * bind_table; /* index range will be == 'max_inst' */
static unsigned int   cur_bind_size; /* current 'bind_table' index range,
'bind_table'[index] with index >= 'cur_bind_size' is currently not in use. */

static int halt_condition;
/* initialization result, value != 0 forces a call to 'hamster' in
   'am_bind_replier'. 'hamster' cannot be called during initialization! */

/* FUNCTIONS: --------------------------------------------------------- */

static void pend_mutex (void)
{
    short result;

    pi_pend_semaphore (inst_mutex, PI_FOREVER, &result);
}


static void post_mutex (void)
{
    short result;

    pi_post_semaphore (inst_mutex, &result);
}


void cancel_inst (INST_DESCR * inst, unsigned char /* boolean */ * cancelled)
/* precondition: 'inst_mutex' must be awaited before this function is called!
   If the instance's 'canc_ref' != NULL then cancels message communication.
   If this cancel operation is successful or if the 'inst_state' is 'READY'
   or 'IN_EXECUTION' then {'inst_state' = 'CLOSING', 'canc_ref' = NULL and
   'cancelled' = !0}. */
{
  if (inst->canc_ref != NULL)
  {
    *cancelled = (tm_cancel_msg (inst->canc_ref) != 0);
  }
  else
  {
    *cancelled = ((inst->inst_state == READY) ||
                  (inst->inst_state == IN_EXECUTION));
  } /* if */
  if (*cancelled)
  {
    inst->inst_state = CLOSING;
    inst->canc_ref = NULL;
  } /* if */
} /* cancel_inst */

/* some access to bind table: ********************************************/

void am_init_replier (unsigned int allowed_inst)
/* Defines the maximal number of instances which is allowed on this device.
   Allocates memory for 'allowed_inst' entries in the 'bind_table'. */
{
  pend_mutex ();
  max_inst = allowed_inst;
  am_instances = (unsigned char) max_inst;
  bind_table = (BIND_DESCR *) pi_alloc (max_inst * sizeof(BIND_DESCR));
  cur_bind_size = 0;
  post_mutex ();
} /* am_init_replier */

/**********************************************************************/

void _am_bind_replier (unsigned char replier_fct,
                       AM_RCV_CONF rcv_conf,
                       AM_REPLY_CONF reply_conf,
                       AM_RESULT * status)
/* Connects the confirmation procedures for a replier function.
   No action if 'status' is returned != 'AM_OK',
   'AM_NO_LOC_MEM_ERR': there was no memory for the 'bind_table';
   'AM_ALREADY_USED'  : this replier function is already bound;
   'AM_FAILURE'       : the 'bind_table' is full; */
{
unsigned int ix, free_ix;
BIND_DESCR * new_entry;

  *status = AM_FAILURE;
  hamster_halt (halt_condition);
      /*$H.Error creating semaphore */

  pend_mutex ();
  if (max_inst == 0) {}
  else if (bind_table != NULL)
  {
    *status = AM_OK;
    free_ix = cur_bind_size;
    for (ix = 0; ix < cur_bind_size; ix++)
    {
      if (bind_table[ix].bind_state == (unsigned char)UNBOUND)
      {
        free_ix = ix;
      }
      else if (bind_table[ix].replier_fct == replier_fct)
      {
        *status = AM_ALREADY_USED;
      } /* if */
    } /* for */
    if (*status == AM_OK)
    {
      if (free_ix == cur_bind_size)
      {
        if (free_ix < max_inst)
        {
          cur_bind_size++;
        }
        else
        {
          *status = AM_FAILURE;
        } /* if */
      } /* if */
      if (*status == AM_OK)
      {
        new_entry = &(bind_table[free_ix]);
        new_entry->replier_fct = replier_fct;
        new_entry->rcv_conf_proc = rcv_conf;
        new_entry->reply_conf_proc = reply_conf;
        new_entry->active_inst = NULL;
        new_entry->bind_state = (unsigned char)BOUND;
      } /* if */
    } /* if */
  }
  else
  {
    *status = AM_NO_LOC_MEM_ERR;
  } /* if (bind_table != NULL) */
  post_mutex ();
} /* _am_bind_replier */

/**********************************************************************/

void _am_unbind_replier (unsigned char replier_fct)
/* Cancels all instances of the specified function, retries until there
   are no active instances. Removes the function from the 'bind_table'.
   The bind state of the function is first set to 'UNBINDING', which
   prohibits that further instances are inserted into the active list. */
{
unsigned char  bind_state; /* BIND_STATES */
BIND_DESCR   * bind_ref;
unsigned char  locked, close; /* boolean */

  locked = 0;
  do /* while (locked); */
  {
    if (locked)
    {
      tm_delay (CANCEL_RETRY_CYC);
      locked = 0;
    }
    do /* while (close); */
    {
      close = 0;
      pend_mutex ();
      find_bind_ref (replier_fct, &bind_state, &bind_ref);
      if (bind_state != (unsigned char)UNBOUND)
      {
        if (bind_ref->active_inst != NULL)
        {
          bind_ref->bind_state = (unsigned char)UNBINDING;
          cancel_inst (bind_ref->active_inst, &close);
          locked = !close;
        }
        else
        {
          bind_ref->bind_state = (unsigned char)UNBOUND;
        } /* if (bind_ref->active_inst != NULL) */
      } /* if (bind_state != UNBOUND) */
      post_mutex ();
      if (close)
      {
        close_inst (bind_ref->active_inst, 0);
      } /* if */
    } while (close);
  } while (locked);
} /* _am_unbind_replier */

/**********************************************************************/

void find_bind_ref (unsigned char replier_fct,
                    unsigned char /* BIND_STATES */ * bind_state,
                    BIND_DESCR * * bind_ref)
/* precondition: 'inst_mutex' must be awaited before this function is called!
   Finds the index into the 'bind_table' for the specified function.
   '*bind_ref' is undefined if 'UNBOUND' is returned as '*bind_state'. */
{
unsigned int ix;

  for (ix = 0; ix < cur_bind_size; ix++)
  {
    if (bind_table[ix].replier_fct == replier_fct)
    {
      if ((*bind_state = bind_table[ix].bind_state) != (unsigned char)UNBOUND)
      {
        *bind_ref = &bind_table[ix];
        return;
      } /* if */
    } /* if */
  } /* for */
  *bind_state = (unsigned char)UNBOUND;
} /* find_bind_ref */

/* some access to instance lists: ****************************************/

void get_inst (/* out */ INST_DESCR * * inst, /* out */ AM_RESULT * err)
/* precondition: 'inst_mutex' must be awaited before this function is called!
   Removes an instance descriptor from the idle list, or allocates a new
   instance descriptor if the 'max_inst' limit is not yet reached.
   Otherwise returns '*inst' as NULL and 'err' != AM_OK. */
{
  *err = AM_OK;
  *inst = idle_inst;
  if (idle_inst != NULL)
  {
    idle_inst = idle_inst->next;
  }
  else if (nr_of_inst < max_inst)
  {
    if ((*inst = (INST_DESCR *) pi_alloc (sizeof (INST_DESCR))) != NULL)
    {
      nr_of_inst++;
    }
    else
    {
      *err = AM_NO_LOC_MEM_ERR;
    }
  }
  else
  {
    *err = AM_NR_OF_INST_OVF;
  } /* if */
} /* get_inst */

/**********************************************************************/

void close_inst (INST_DESCR * inst, unsigned char do_confirm)
/* Closes an instance descriptor.
   Removes the instance from the active list in the 'bind_table',
   inserts it into the idle list and conditionally calls the reply
   confirmation procedure. 'inst' must not be NULL! */
{
AM_REPLY_CONF proc;
unsigned char replier_fct;
void        * ext_ref;
INST_DESCR ** p_chained_from;

  pend_mutex ();
  if (inst->my_buffer)
  {
    pi_free (inst->msg_addr);
  }
  ext_ref = inst->ext_ref;
  replier_fct = inst->bind_ref->replier_fct;
  proc = inst->bind_ref->reply_conf_proc;
  for (p_chained_from = &(inst->bind_ref->active_inst);
       ;
       p_chained_from = &((*p_chained_from)->next))
  {
    if ((*p_chained_from) == inst)
    {
      (*p_chained_from) = inst->next;
      break;
    }
    else
    {
      hamster_halt ((*p_chained_from) == NULL);
	/*$H.close_inst: inst descriptor not found in bind table */
    } /* if */
  } /* for */
  inst->next = idle_inst;
  idle_inst = inst;
  post_mutex ();
  if (do_confirm)
  {
    REPL_CONFIRM (proc, replier_fct, ext_ref);
  } /* if */
} /* close_inst */

/**********************************************************************/

void find_inst (const BIND_DESCR * bind_ref, const void * ext_ref,
                /* out */ INST_DESCR * * inst)
/* precondition: 'inst_mutex' must be awaited before this function is called!
   Finds the instance descriptor with the specified 'ext_ref' field in the
   active list which belongs to the 'bind_ref'.
   '*inst' is returned 'NULL' if the instance is not in the list. */
{
  *inst = bind_ref->active_inst;
  while (*inst != NULL)
  {
    if ((*inst)->ext_ref == ext_ref)
    {
      break;
    }
    else
    {
      *inst = (*inst)->next;
    }
  } /* while */
} /* find_inst */

/**********************************************************************/

AM_RESULT ready_to_rcv_inst (unsigned char replier_fct, CARDINAL32 in_msg_size,
                             TM_MSG_DESCR * canc_ref,
                             const TM_LINK_ID * link,
                             /* out */ INST_DESCR * * inst)
/* Checks if there is already such an open link and sets 'abort' = !0 for
   the involved instance descriptor. Otherwise:
   Finds, in the list of active instances which serve the specified function,
   an instance with 'inst_state' == 'READY' which accepts the specified call
   message size. Allocates a buffer for the call message if the replier did
   not supply a buffer. Changes the 'inst_state' to 'RECEIVING' and saves
   'canc_ref', 'in_msg_size' and 'link'. Modifies 'msg_addr' and 'my_buffer'
   in case of successful memory allocation.
   On any problem '*inst' is returned NULL and the function result != 'AM_OK'
   ( no action ). */
{
    BIND_DESCR  * bind_ref;
    unsigned char bind_state; /* BIND_STATES */
    AM_RESULT     err;
    INST_DESCR  * find, * found;

    *inst = NULL;
    found = NULL;
    err = AM_NO_READY_INST_ERR;
    pend_mutex ();
    find_bind_ref (replier_fct, &bind_state, &bind_ref);
    if (bind_state == (unsigned char)BOUND)
    { /* check if already such a link and find ready instance: */
        for (find = bind_ref->active_inst; find != NULL; find = find->next)
        {
            if (find->inst_state == READY)
            {
                if (find->max_in_msg_size >= in_msg_size)
                {
                    found = find;
                    err = AM_OK;
                }
                else if (err != AM_OK)
                {
                    err = AM_CALL_LEN_OVF;
                } /* if (find->max_in_msg_size >= in_msg_size) */
            }
            /* !!! The following comparison is not portable, but asserted by
               'am_system_init_replier'. */
            else if (*(CARDINAL32 *)&(find->link) == *(CARDINAL32 *)link)
            {
                find->abort = !0;
                err = AM_DUPL_LINK_ERR;
                break;
            } /* if (find->inst_state == READY) */
        } /* for */

        if (err == AM_OK)
        { /* allocate a buffer if necessary: */
            if ((found->msg_addr == NULL) && (in_msg_size != 0))
            {
                if (in_msg_size > SHRT_MAX)
                {
                    err = AM_NO_REM_MEM_ERR;
                }
                else if ((found->msg_addr = pi_alloc (in_msg_size)) != NULL)
                {
                    found->my_buffer = !0;
                }
                else
                {
                    err = AM_NO_REM_MEM_ERR;
                } /* if (in_msg_size > SHRT_MAX) */
            } /* if */

            if (err == AM_OK)
            {
                *inst = found;
                found->inst_state = RECEIVING;
                found->msg_size = in_msg_size;
                found->link = *link;
                found->canc_ref = canc_ref;
            } /* if (err == AM_OK) */
        } /* if (err == AM_OK) */
    }
    else
    {
        err = AM_UNKNOWN_DEST_ERR;
    } /* if (bind_state == BOUND) */
    post_mutex ();
    return err;
} /* ready_to_rcv_inst */

/* connected as notification procedures: *********************************/

void r_invoke_ind (const TM_LINK_ID * link,
                   void * * in_msg_addr, CARDINAL32 in_msg_size,
                   void * * in_hdr_addr, unsigned int * in_hdr_size,
                   TM_MSG_DESCR * canc_ref, void * * param,
                   AM_RESULT * err, unsigned char replier_vehicle)
/* Is called on reception of an invoke packet from a caller.
   The call message header must be empty.
   Calls 'ready_to_rcv_inst' to get a ready instance which will receive the
   call message. Saves 'replier_vehicle' in the instance descriptor.
   Returns 'param' as reference to the instance descriptor or rejects the
   call with '*err' != 'AM_OK'. */
{
unsigned char         replier_fct;

  if (AM_PHYS_ADDR & replier_vehicle)
  {
    replier_fct = AM_SERVICE_FCT;
  }
  else
  {
    replier_fct = (*link).my_fct_or_dev;
  } /* if */
  *err = ready_to_rcv_inst (replier_fct, in_msg_size, canc_ref, link,
                            (INST_DESCR * *)param);
  if (*err == AM_OK)
  {
    *in_msg_addr = ((INST_DESCR *)(*param))->msg_addr;
    *in_hdr_addr = &((INST_DESCR *)(*param))->msg_hdr;
    *in_hdr_size = sizeof (((INST_DESCR *)(*param))->msg_hdr);
    ((INST_DESCR *)(*param))->replier_vehicle = replier_vehicle;
    ((INST_DESCR *)(*param))->shortcut = 0;
  } /* if (*err == AM_OK) */
} /* r_invoke_ind */

/**********************************************************************/
/* added EH 22-Jun-1994: */
void r_rcv_ind_rel (void * param, AM_RESULT err, unsigned char *release)
{
  /* request early release in case of RPI */
  *release = (((INST_DESCR *)param)->msg_hdr == AM_NO_REPLY);
  r_rcv_ind (param, err);
} /* r_rcv_ind_rel */

/**********************************************************************/

void r_rcv_ind (void * param, AM_RESULT err)
/* Called when a complete call message has been received.
   'param' is the reference to the instance descriptor.
   Normally the call is passed to the replier instance by calling the
   confirmation procedure which was bound for this replier function.
   On any other error the call is discarded, and the instance is not
   notified but returned to the ready state to wait for another call. */
{
AM_RCV_CONF     proc;
unsigned char   replier_fct;
AM_ADDRESS      caller;   /* EH 20-Jul-1994, old type was AM_NET_ADDR */
void          * ext_ref, * in_msg_addr;
CARDINAL32      in_msg_size;

  if (err == AM_OK)
  {
    proc = ((INST_DESCR *)param)->bind_ref->rcv_conf_proc;
    replier_fct = ((INST_DESCR *)param)->bind_ref->replier_fct;
    /* EH 20-Jul-1994, assign caller.tc: */
    caller.tc = ((INST_DESCR *)param)->replier_vehicle & ~AM_PHYS_ADDR;
    caller.vehicle = ((INST_DESCR *)param)->link.vehicle;
    /* EH 03-Aug-1994, new AM_ADDRESS format containing next station: */
    caller.fct_or_sta = ((INST_DESCR *)param)->link.fct_or_dev;
    caller.next_sta = ((INST_DESCR *)param)->link.link_dev;
/*  if (AM_PHYS_ADDR & caller.vehicle)
    {
      caller.function = AM_MANAGER_FCT;
      caller.device = ((INST_DESCR *)param)->link.fct_or_dev;
    }
    else
    {
      caller.function = ((INST_DESCR *)param)->link.fct_or_dev;
      if (caller.vehicle == AM_SAME_VEHICLE)
      {
	caller.device = ((INST_DESCR *)param)->link.link_dev;
      }
      else
      {
        caller.device = AM_UNKNOWN_DEVICE;
      }
    }
*/  ext_ref = ((INST_DESCR *)param)->ext_ref;
    in_msg_addr = ((INST_DESCR *)param)->msg_addr;
    in_msg_size = ((INST_DESCR *)param)->msg_size;

    pend_mutex ();
    ((INST_DESCR *)param)->canc_ref = NULL;
    ((INST_DESCR *)param)->inst_state = IN_EXECUTION;
    ((INST_DESCR *)param)->my_buffer = 0;
    post_mutex ();
    am_messages_received = ((am_messages_received+1) % 0xFFFFFFFFL);
    RECV_CONFIRM (proc,replier_fct,&caller,in_msg_addr,in_msg_size,ext_ref);
  }
  else
  {
    pend_mutex ();
    ((INST_DESCR *)param)->canc_ref = NULL;
    ((INST_DESCR *)param)->inst_state = READY;
    if (((INST_DESCR *)param)->my_buffer)
    {
      pi_free (((INST_DESCR *)param)->msg_addr);
      ((INST_DESCR *)param)->msg_addr = NULL;
      ((INST_DESCR *)param)->my_buffer = 0;
    } /* if */
    post_mutex ();
  } /* if (err == AM_OK) */
} /* r_rcv_ind */

/**********************************************************************/

void r_send_conf (void * param, AM_RESULT err, unsigned char replier_vehicle)
/* Called on completion of a reply message transmission.
   'param' is the reference to the instance descriptor.
   Tries to send the same message once more if 'err' == 'AM_TRY_LATER_ERR'
   while the 'abort' field of the instance descriptor is not set. */
{
  replier_vehicle = replier_vehicle;
  if (((err == AM_TRY_LATER_ERR) || (err == AM_REM_CONN_OVF))
      && (!((INST_DESCR *)param)->abort))
  {
    pend_mutex ();
    err = tm_send_msg_requ (TM_REPLIER_USER, &((INST_DESCR *)param)->link,
                            ((INST_DESCR *)param)->msg_addr,
                            ((INST_DESCR *)param)->msg_size,
                            &((INST_DESCR *)param)->status, sizeof (AM_RESULT),
                            param, &((INST_DESCR *)param)->canc_ref,
                            ((INST_DESCR *)param)->replier_vehicle,
                            TM_MODE_RELEASE);  /* added EH 22-Jun-1994 */
    post_mutex ();
    if (err == AM_OK)
    {
      return;
    } /* if (err == AM_OK) */
  } /* if */
  close_inst ((INST_DESCR *)param, !0);
} /* r_send_conf */

/**********************************************************************/

AM_RESULT get_int_call
                  (unsigned char caller_fct, unsigned char replier_fct,
                   void * in_msg_addr, CARDINAL32 in_msg_size,
                   void * in_hdr_addr, unsigned int in_hdr_size,
                   /* out */ void * * param)
/* Called through 'tm_put_call' by an internal caller to dispatch a call.
   First calls 'ready_to_rcv_inst' to find a ready instance. Then, if the
   call is to be accepted, copies the call message to the own buffer. */
{
INST_DESCR    * inst;
TM_LINK_ID      link;
short           last_size;
unsigned char   *dest,  *src;
AM_RESULT       err;

  if (in_hdr_size == sizeof (inst->msg_hdr))
  {
    link.my_fct_or_dev = replier_fct;
    link.vehicle = AM_SAME_VEHICLE;
    link.fct_or_dev = caller_fct;
    link.link_dev = AM_SAME_DEVICE;
    err = ready_to_rcv_inst (replier_fct, in_msg_size, NULL, &link, &inst);
    if (err == AM_OK)
    {
      inst->replier_vehicle = AM_ANY_TC;  /* EH 20-Jul-1994 */
      inst->msg_hdr = * (unsigned char *) in_hdr_addr;
      last_size = in_msg_size % COPY_BLOCK_SIZE;
      dest = (unsigned char *) inst->msg_addr;
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
      inst->shortcut = !0;
      *param = inst;
    } /* if (err == AM_OK) */
  }
  else
  {
    err = AM_FAILURE;
  } /* if (in_hdr_size == 0) */
  return err;
} /* get_int_call */

/* exported procedures: **************************************************/

void _am_rcv_requ (unsigned char replier_fct,
                   void * in_msg_addr,  /* buffer for call message */
                   CARDINAL32 in_msg_size,
                   void * ext_ref,
                   AM_RESULT * status)
/* An instance announces that it is ready to accept an incoming call of
   the specified replier function and waits for such a call.
   Checks if the replier function is bound and if there is already such
   an instance. Gets a free instance descriptor (from the idle list),
   initializes it and inserts it into the bind_table's active list.
   On any problem 'status' is returned != 'AM_OK' (no action). */
{
BIND_DESCR   * bind_ref;
unsigned char  bind_state; /* BIND_STATES */
INST_DESCR   * inst;

  pend_mutex ();
  find_bind_ref (replier_fct, &bind_state, &bind_ref);
  if (bind_state == (unsigned char)BOUND)
  {
    find_inst (bind_ref, ext_ref, &inst);
    if (inst == NULL)
    {
      get_inst (&inst, status);
      if (inst != NULL)
      {
        inst->inst_state = READY;
        inst->my_buffer = 0;
        inst->abort = 0;
        inst->ext_ref = ext_ref;
        inst->canc_ref = NULL;
        inst->bind_ref = bind_ref;
        inst->msg_addr = in_msg_addr;
        inst->max_in_msg_size = in_msg_size;
        inst->next = bind_ref->active_inst;
        bind_ref->active_inst = inst;
      } /* if (inst != NULL) */
    }
    else
    {
      *status = AM_ALREADY_USED;
    } /* if (inst == NULL) */
  }
  else
  {
    *status = AM_FAILURE;
  } /* if (bind_state == BOUND) */
  post_mutex ();
} /* _am_rcv_requ */

/************************************************************************/

AM_REPLIER_STATES _am_get_replier_status (unsigned char replier_fct,
                                          void * ext_ref)
{
unsigned char  state;
BIND_DESCR   * bind_ref;
INST_DESCR   * inst;

  pend_mutex ();
  find_bind_ref (replier_fct, &state, &bind_ref);
  if (state == (unsigned char)BOUND)
  {
    find_inst (bind_ref, ext_ref, &inst);
    if (inst != NULL)
    {
      state = (unsigned char)(inst->inst_state);
      if ((state == (unsigned char)IN_EXECUTION)
          && (inst->msg_hdr == AM_NO_REPLY))
      {
        state = (unsigned char)AM_RS_RPI_INEXEC;
      }
    }
  }
  post_mutex ();
  return state;
} /* _am_get_replier_status */

/*****************************************************************************/

void _am_reply_requ (unsigned char replier_fct,
                     void * out_msg_addr,       /* reply message */
                     CARDINAL32 out_msg_size,
                     void * ext_ref,
                     AM_RESULT status)
/* An instance which executed a call returns a reply message for that call.
   Searches this instance in the bind_table's active list and retrieves the
   caller identification from the instance descriptor. Updates the instance's
   state to 'SENDING'. Passes 'status' with use of the reply message header.
   The reply message is not passed to the caller if the 'abort' field in the
   instance descriptor is set. In this case or in case of an internal call
   (shortcut, caller on same device: retry after delay on 'AM_TRY_LATER_ERR')
   the instance descriptor is closed immediately by calling 'close_inst'. */
{
    unsigned char bind_state; /* BIND_STATES */
    BIND_DESCR  * bind_ref;
    INST_DESCR  * inst;
    AM_RESULT     err;
    unsigned char close, deliver; /* boolean */
    AM_REPLY_CONF proc;
    void        * param;

	am_messages_sent = ((am_messages_sent+1) % 0xFFFFFFFFL);
	deliver = 0;
	close   = 0;
	pend_mutex ();
	find_bind_ref (replier_fct, &bind_state, &bind_ref);
	if (bind_state == (unsigned char)BOUND)
	{
        find_inst (bind_ref, ext_ref, &inst);
        close = (inst != NULL);
        if (close)
        {
            close = (inst->inst_state == IN_EXECUTION);
        }
        if (!close)
        {
            proc = bind_ref->reply_conf_proc;
            post_mutex ();
            REPL_CONFIRM (proc, replier_fct, ext_ref);
            hamster_continue (TRUE);     /* 24-Feb-1994, EH: was hamster_halt */
	        /*$H.am_reply_requ: unexpected request, no outstanding reply */
        }
        inst->inst_state = SENDING;
        if ((inst->abort) || (inst->msg_hdr == AM_NO_REPLY))
        {
            inst->inst_state = CLOSING;
        }
        else
        {
            inst->status = status;
            if (!inst->shortcut)
            {
                inst->msg_addr = out_msg_addr; /* save for retrial */
                inst->msg_size = out_msg_size;
                err = tm_send_msg_requ (TM_REPLIER_USER, &inst->link,
                                        out_msg_addr, out_msg_size,
                                        &inst->status, sizeof (AM_RESULT),
                                        inst, &inst->canc_ref, inst->replier_vehicle,
                                        TM_MODE_RELEASE);  /* added EH 22-Jun-1994 */
                close = (err != AM_OK);
            }
            else
            {
                post_mutex ();
	            err = tm_put_reply (replier_fct, inst->link.fct_or_dev,
                                    out_msg_addr, out_msg_size,
                                    &inst->status, sizeof (AM_RESULT), &param);
                deliver = (err == AM_OK);
                pend_mutex ();
            }
        }
    }
    else
    {
        hamster_halt (TRUE);
        /*$H.am_reply_requ: Function has not been bound (no am_bind_replier?) */
        /* there will be no confirmation to return the reply message buffer! */
    }
    post_mutex ();
    if (close)
    {
        close_inst (inst, !0);
    }
    if (deliver)
    {
        tm_deliver_reply (param, AM_OK);
    }
}

/**********************************************************************/

void _am_rcv_cancel (unsigned char replier_fct,
                     void * ext_ref, AM_RESULT * status)
/* Cancels an instance.
   Does not return as long as there is such an active instance.
   If 'cancel_inst' is successful then returns 'AM_OK' as '*status'.
   Otherwise tries again after a delay. Returns 'AM_FAILURE' as '*status'
   as soon as there is no such active instance.*/
{
unsigned char bind_state; /* BIND_STATES */
BIND_DESCR  * bind_ref;
INST_DESCR  * inst;
unsigned char inactive; /* boolean */

  *status = AM_FAILURE;
  inactive = !0;
  do /* while (!inactive) */
  {
    if (!inactive)
    {
      tm_delay (CANCEL_RETRY_CYC);
      inactive = !0;
    }
    pend_mutex ();
    find_bind_ref (replier_fct, &bind_state, &bind_ref);
    if (bind_state != (unsigned char)UNBOUND)
    {
      find_inst (bind_ref, ext_ref, &inst);
      if (inst != NULL)
      {
        cancel_inst (inst, &inactive);
        if (inactive)
        {
          *status = AM_OK;
        } /* if (inactive) */
      } /* if (inst != NULL) */
    } /* if (bind_state != UNBOUND) */
    post_mutex ();
  } while (!inactive);
  if (*status == AM_OK)
  {
    close_inst (inst, 0);
  } /* if (*status == AM_OK) */
} /* _am_rcv_cancel */

/**********************************************************************/

void am_system_init_replier (void)
{
short status;

  halt_condition = nr_of_inst = max_inst = cur_bind_size = 0;
  idle_inst = NULL;
  bind_table = NULL;
  inst_mutex = pi_create_semaphore (1, PI_ORD_FCFS, &status);
  if (status != PI_RET_OK)
  {
    halt_condition = 1;
  }
  tm_define_user (TM_REPLIER_USER, r_invoke_ind, r_rcv_ind_rel, r_send_conf);
  tm_conn_get_call (get_int_call, r_rcv_ind);
} /* am_system_init_replier */
