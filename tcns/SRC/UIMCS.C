
/*************************************************************************
**        Copyright (C) Siemens AG 1997 All Rights Reserved. Confidential    
**========================================================================
**
**    PROJECT::    TCN Train Communication Network
                   JDP Joint Development Project

**    MODULE::     UIC Multicast Server

**    WORKFILE::   $Workfile:   UIMCS.C  $
**************************************************************************
**    TASK::       UIC-Agent

**************************************************************************
**    AUTHOR::     P. Just ()  
**    CREATED::    20.10.97
**========================================================================
**    HISTORY::    AUTHOR:: $Author:   HEINDEL_AR  $
          REVISION::            $Revision:   1.27  $  
          MODTIME::             $Date:   10 Dec 1998 13:28:50  $
      $Log:   O:\PRJ\JDP\FUE\SW\PVCS\ARC\TCN_FW\UIC\SRC\UIMCS.Cv_  $
 * 
 *    Rev 1.27   10 Dec 1998 13:28:50   HEINDEL_AR
 * removed '&' from function pointers
 * 
 *    Rev 1.26   04 Aug 1998 08:13:58   Just_Pete
 * Die Länge der Reply-Message wird genau
 * für den Bytestream berechnet.
 * 
 *    Rev 1.25   17 Jul 1998 14:02:24   Just_Pete
 * Die Reply des Multicast-Create-Requests
 * enthält nun die Replies aller Adressaten: 
 * Dienst UIC_MCP_COLLECT_REPLY.
 * 
 *    Rev 1.24   09 Jul 1998 12:13:24   Just_Pete
 * UIMCS fuer synchronen Aufruf um Dienst
 * UIC-MCP_SYNC erweitert.
 * 
 *    Rev 1.23   10 Jun 1998 15:13:02   Just_Pete
 * Bei owner UIC_O_DB uns dem command 
 * FC_WRITE_KFTCORR wird auch eine Taufe
 * automatisch ausgelöst
 * 
 *    Rev 1.22   05 Jun 1998 16:21:46   Just_Pete
 * bei OK war state_cmd in der reply undefiniert
 * 
 *    Rev 1.21   03 Jun 1998 12:07:32   Just_Pete
 * Wegen warnings bei iC-86 wurden enums 
 * in defines geändert.
 * 
 *    Rev 1.20   20 May 1998 10:32:24   Just_Pete
 * get_next_job_processed(): loop end 
 * condition corrected
 * 
 *    Rev 1.19   20 May 1998 10:00:02   Just_Pete
 * counters for the dynamic debugging of the
 * uimcs handler
 * 
 *    Rev 1.18   19 May 1998 10:19:56   Just_Pete
 * updated uimsc.h
 * 
 *    Rev 1.17   19 May 1998 09:18:32   Just_Pete
 * correction owner_of_this_vehicle
 * 
 *    Rev 1.16   19 May 1998 08:54:10   Just_Pete
 * extern owner_of_this_vehicle removed
 * 
 *    Rev 1.15   15 May 1998 14:00:46   Just_Pete
 * uimcs_handler(): changes in state actions
 * 
 *    Rev 1.14.1.0   14 May 1998 16:05:48   Just_Pete
 * extended multicast server
**************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "profile.h"
#include "tcntypes.h"
#include "wtb.h"
#include "bmi.h"
#include "ums.h"
#include "umsconv.h" 

#include "am_sys.h"
#include "pi_sys.h"
#include "uwtm.h"
#include "ungs.h"
#include "uagt.h"

#include "uimcs.h"
#ifdef O_KFT
#include "kft.h"
#endif

#define UWTM_T_RESULT int

/*
 * state of multicast service
 */
#define MC_S_UNDEFINED      0
#define MC_S_DEFINED        1
#define MC_S_IN_PROGRESS    2
#define MC_S_SERVED         3

/*
 * return values of local functions
 */
typedef int T_RETURN_VAL;
#define MC_OK               0
#define MC_WAITING          1
#define MC_DONE             2
#define MC_INCOMPLETE       3
#define MC_INAUGURATION     4
#define MC_ERROR            5

/*
 * list of multicast requests processed
 * the information is stored in a job description
 */

/* definition of a multicast job */
typedef struct STR_MC_JOB
{ 
   UNSIGNED16                 identifier;  /* job id         */
   UNSIGNED16                 mc_code;     /* cmd_code to be multicasted */
   UNSIGNED8                  state;       /* state of job    */
   UNSIGNED8                  service;     /* type of service */
   UNSIGNED8                  retry_max;   /* retry counter   */
   UNSIGNED8                  owner;       /* owner eg. UIC or DB */
   UNSIGNED8                  dest_veh;
   UNSIGNED8                  dest_fct;
   UNSIGNED8                  src_veh;
   UNSIGNED8                  src_fct;
   UNSIGNED8                  topo_cnt;
   void                       *p_msg;       /* msg to be multicasted */
   CARDINAL32                 msg_size;     /* size of this message  */
   UNSIGNED8                  *p_collect_reply;    /* ptr to collect reply message */
   CARDINAL32                 collect_reply_size;  /* size of collect reply message */
   unsigned char              replier_fct_from_uagt;  /* function id forwarded by uagt */
   uagt_reference             *reference_from_uagt;   /* uagt reference */
   struct STR_MC_JOB*         next_job;

} T_MC_JOB;

/*
 * definitions for the actual call processing
 * the information is stored in a "call" structure;
 * it contains more detailed information like the 
 * destination nodes.
 */

/* state of a destination */
#define DEST_UNDEFINED          0
#define DEST_ADDRESSED          1
#define DEST_CALLED             2
#define DEST_SERVED             3

/* list of destinations */
typedef struct
{
   UNSIGNED8                  wtb_addr;
   UNSIGNED8                  state;

} T_DEST_LIST;

#define STATE_IDLE                              0
#define STATE_CALLING                           1
#define STATE_WAITING_FOR_REPLY                 2
#define STATE_WAITING_FOR_START_INAUGURATION    3

typedef void (*T_JOB_DONE)(void);

/* control structure for the job processing */
typedef struct
{
   UNSIGNED8                  state;        /* MC_STATE */
   UNSIGNED8                  retries;      /* number of reties */
   UNSIGNED8                  instances;    /* active call instances */
   UNSIGNED8                  dest_index;
   UNSIGNED8                  topo_counter;
   BOOLEAN                    inaug_error;
   T_MC_JOB*                  p_job;        /* description of the multicast job */
   AM_ADDRESS                 replier;
   AM_CALL_CONF               confirm_hdl;
   T_JOB_DONE                 job_done;
   T_DEST_LIST                dest[MC_DEST_MAX];

} T_UIMCS_CTRL;

/****************************************************************************
 * global variables
 */
extern void uic_inaug_enf_requ (UNSIGNED8*);
extern unsigned char replier_function;
extern uagt_reference * reference;

/****************************************************************************
 * local variables
 */

static T_UIMCS_CTRL                  uimcs_ctrl;
static UIMCS_T_CONFIGURATION         uimcs_config;
static short                         uimcs_semid;
static T_MC_JOB*                     job_queue_head;
static T_MC_JOB*                     job_free_queue;
static UIC_Header                    uimcs_topo_call;
static UNSIGNED8                     uimcs_inaug_dummy_state;

#ifdef O_166
#pragma hold(near,0)
#endif

UNSIGNED32                    multicast_state_idle;
UNSIGNED32                    multicast_state_calling;
UNSIGNED32                    multicast_state_waiting_for_reply;
UNSIGNED32                    multicast_state_inauguration;
UNSIGNED32                    multicast_state_default;
UNSIGNED32                    multicast_complete;
UNSIGNED32                    multicast_incomplete;
UNSIGNED32                    multicast_wrong_service;
UNSIGNED32                    multicast_retries;
UNSIGNED32                    multicast_send_am_ok;
UNSIGNED32                    multicast_send;
UNSIGNED32                    multicast_handler;
UNSIGNED32                    multicast_inauguration_detected;

/*********************************************************************/
/*  Name:       free_mc_callmsg()                                        */
/*  Function:   free the memory previously allocated for the         */
/*              multicast message                                    */
/*  returns :   void                                                 */
/*********************************************************************/
static void free_mc_callmsg(T_MC_JOB*   p_job)
{
   p_job->state = MC_S_SERVED;

   /*
    * free the call message
    */
   if (p_job->p_msg != NULL)
   {
      PI_FREE(p_job->p_msg);
      p_job->p_msg = NULL;
   }

   return;
}

/*********************************************************************/
/*  Name:       free_mc_collect_replymsg()                           */
/*  Function:   free the memory previously allocated for the         */
/*              multicast collect reply message                      */
/*  returns :   void                                                 */
/*********************************************************************/
static void free_mc_collect_replymsg(T_MC_JOB*   p_job)
{
   if (p_job->p_collect_reply != 0)
   {
      PI_FREE(p_job->p_collect_reply);
      p_job->p_collect_reply = NULL;
   }

   return;
}

/*********************************************************************/
/*  Name:       free_job()                                           */
/*  Function:   free memory of a multicast message and put the       */
/*              multicast job descriptor back into the free-list     */
/*  returns :   void                                                 */
/*********************************************************************/
static void free_job(T_MC_JOB* p_job)
{
    /* free the call message*/
    free_mc_callmsg(p_job);
    /* free the collect reply  message*/
    free_mc_collect_replymsg(p_job);
   
    p_job->state = MC_S_UNDEFINED;

    /* put the job descriptor back into the free list */
   
    pi_lock_task();
    p_job->next_job   = job_free_queue;
    job_free_queue    = p_job;
    pi_unlock_task();
    return;
}

/*********************************************************************/
/*  Name:       remove_job_from_queue()                              */
/*  Function:   removes a jobdescriptor from the active queue        */
/*  returns :   MC_OK or MC_ERROR when pend semaphore failed         */
/*********************************************************************/
static T_RETURN_VAL remove_job_from_queue(T_MC_JOB* p_job)
{
   T_MC_JOB       **pptr, *p_next;
   short            retval;
   
   pi_pend_semaphore( uimcs_semid, PI_FOREVER, &retval);
   if (retval != PI_RET_OK)
   {
      p_job->state = MC_S_UNDEFINED;
      return MC_ERROR;
   }
   for (pptr = &job_queue_head ; (*pptr != p_job) && (*pptr != NULL); 
                                                pptr = &(*pptr)->next_job)
   {
   }
   if (*pptr != NULL)
   {
      p_next = (*pptr)->next_job;
      *pptr  = p_next;
   }
   pi_post_semaphore( uimcs_semid, &retval);

   return MC_OK;
}

/*********************************************************************/
/*  Name:       remove_and_free_job()                                */
/*  Function:   cleans up after a call_confirm was received for the  */
/*              multicast service "simple". Removes job from the     */
/*              queue, frees memory from the job's message and puts  */
/*              the job descriptor into the job_free_queue           */
/*  returns :   void                                                 */
/*********************************************************************/
static void remove_and_free_job(void)
{
   T_RETURN_VAL             retval;

   retval = remove_job_from_queue( uimcs_ctrl.p_job);
   if (retval == MC_OK)
   {
      free_job( uimcs_ctrl.p_job);
   }

   return;
}

/*********************************************************************/
/*  Name:       clear_job_queue()                                    */
/*  Function:   clears all queued for the service UIC_MCP_SIMPLE     */
/*  returns :   MC_OK or MC_ERROR when pend semaphore failed         */
/*********************************************************************/
static T_RETURN_VAL clear_job_queue( void)
{
   
   T_MC_JOB**       pptr;
   T_MC_JOB*        p_job;
   short            retval;

   pi_pend_semaphore( uimcs_semid, PI_FOREVER, &retval);
   if (retval != PI_RET_OK)
   {
      return MC_ERROR;
   }

   pptr = &job_queue_head;
   while ((*pptr) != NULL)
   {
      p_job = *pptr;
      if (((*pptr)->service == UIC_MCP_SIMPLE) || 
          ((*pptr)->service == UIC_MCP_SYNC) ||
          ((*pptr)->service == UIC_MCP_COLLECT_REPLY))
      {
         *pptr     = p_job->next_job;
         free_job(p_job);
      }
      else
      {
         pptr = &(p_job->next_job);
      }
   }
   pi_post_semaphore( uimcs_semid, &retval);

   return MC_OK;
}

/*********************************************************************/
/*  Name:       get_new_job_id()                                     */
/*  Function:   creates an identifier for a new multicast job        */
/*  returns :   job identifier                                       */
/*********************************************************************/
static UNSIGNED16 get_new_job_id( void)
{
   static UNSIGNED16    job_id_cnt;    /* may be initialized to any value */
   UNSIGNED16           new_job_id;

   pi_lock_task();
   if (job_id_cnt++ == 0) 
   {
      job_id_cnt = 1;    
   }
   new_job_id = job_id_cnt;
   pi_unlock_task();

   return new_job_id;  
}

/*********************************************************************/
/*  Name:       set_new_job()                                        */
/*  Function:   initialize a new multicast job descriptor            */
/*  returns :   NULL or pointer to a multicast job descriptor        */
/*********************************************************************/
static T_MC_JOB* set_new_job( UIC_MCreateRequ*  p_callmsg, 
                              CARDINAL32        callmsg_size,
                              UNSIGNED8         topo_counter)
{
   T_MC_JOB*        p_job;
   UIC_Msg*         p_msg;
   CARDINAL32       msg_len = callmsg_size - UIC_MCCREATE_CTRL_BYTES;
   UIC_Msg          msg_to_convert;

  
   /*
    * get a free multicast job descriptor
    */
   pi_lock_task();
   p_job = job_free_queue;
   if (p_job == NULL)
   {
       pi_unlock_task();
       return NULL;
   }
   job_free_queue = p_job->next_job;
   pi_unlock_task();
   
   /*
    * allocate memory for message which will be multicasted 
    */
   p_msg = PI_ALLOC( msg_len);
   if (p_msg == NULL)
   {
       free_job(p_job);
       return NULL;
   }
  
   /*
    * initialize the job description
    */
   p_job->identifier  = get_new_job_id();
   p_job->mc_code     = p_callmsg->mc_ctrl.mc_code;
   p_job->state       = MC_S_DEFINED;
   p_job->service     = p_callmsg->mc_ctrl.mc_service;
   p_job->retry_max   = p_callmsg->mc_ctrl.mc_retry_counter;
   p_job->dest_veh    = p_callmsg->mc_ctrl.mc_destination_veh;
   p_job->dest_fct    = p_callmsg->mc_ctrl.mc_destination_fct;
   p_job->owner       = p_callmsg->hdr.owner;
   p_job->src_veh     = p_callmsg->hdr.src_veh;
   p_job->src_fct     = p_callmsg->hdr.src_fct;
   p_job->p_msg       = p_msg;
   p_job->msg_size    = msg_len;
   p_job->topo_cnt    = topo_counter;
   p_job->next_job    = NULL;

   /*
    * initialize the message which will be multicasted 
    */
   msg_to_convert.hdr.owner                = p_callmsg->hdr.owner;
   msg_to_convert.hdr.reserved1            = 0;
   msg_to_convert.hdr.dest_veh             = p_callmsg->mc_ctrl.mc_destination_veh;
   msg_to_convert.hdr.dest_fct             = p_callmsg->mc_ctrl.mc_destination_fct;
   msg_to_convert.hdr.src_veh              = p_callmsg->hdr.dest_veh;
   msg_to_convert.hdr.src_fct              = p_callmsg->hdr.dest_fct;
   msg_to_convert.hdr.code                 = p_callmsg->mc_ctrl.mc_code;
   msg_to_convert.hdr.state_cmd            = p_callmsg->mc_ctrl.mc_status;
   msg_to_convert.hdr.reserved2            = 0;
   wtbconv_00_01__HTON((UNSIGNED8*)&p_msg->hdr, &msg_to_convert.hdr);
   pi_copy8( ((char*)p_msg) + UIC_HEADER_BYTES, p_callmsg->mc_msg_data.mc_message, 
      (unsigned short)(msg_len - UIC_HEADER_BYTES));

   /*
    * preset the variables for synchronous reply
    */

   switch (p_job->service)
   {
   case UIC_MCP_COLLECT_REPLY:
       /* allocate memory for the maximum of collect replies */
       p_job->collect_reply_size = UIC_HEADER_BYTES + 
              2 + (MC_DEST_MAX * SIZE_OF_COLLECT_REPLY_ELEMENT);
       if ((p_job->p_collect_reply = PI_ALLOC (p_job->collect_reply_size)) == NULL)
       {
           free_job(p_job);
           return NULL;
       }
       /* clear num_replies byte */
       *(p_job->p_collect_reply + UIC_HEADER_BYTES) = 0;
       /* clear reserved byte */
       *(p_job->p_collect_reply + UIC_HEADER_BYTES+1) = 0;
       p_job->replier_fct_from_uagt =  replier_function;
       p_job->reference_from_uagt   =  reference;
       break;

   case UIC_MCP_SYNC:
       p_job->replier_fct_from_uagt =  replier_function;
       p_job->reference_from_uagt   =  reference;
       break;
     
   case UIC_MCP_SIMPLE:
       p_job->replier_fct_from_uagt =  0;
       p_job->reference_from_uagt   =  NULL;
       break;
     
   default:
       /* wrong service */
       free_job(p_job);
       return NULL;
       break;
   }
   
   return p_job;
}

/*********************************************************************/
/*  Name:       append_job_to_queue()                                */
/*  Function:   inserts job into active queue                        */
/*  returns :   MC_OK or MC_ERROR, when pend semaphore failed        */
/*********************************************************************/
static T_RETURN_VAL append_job_to_queue(T_MC_JOB* p_job)
{
   short retval;

   pi_pend_semaphore( uimcs_semid, PI_FOREVER, &retval);
   if (retval != PI_RET_OK)
   {
      return MC_ERROR;
   }
   p_job->next_job = job_queue_head;
   job_queue_head      = p_job;
   pi_post_semaphore( uimcs_semid, &retval);

   return MC_OK;
}

/*********************************************************************/
/*  Name:       get_next_job_processed()                             */
/*  Function:   get next multicast job descriptor from the queue     */
/*  returns :   NULL or pointer to the last multicast job descriptor */
/*              in the queue (FIFO)                                  */
/*********************************************************************/
static T_MC_JOB* get_next_job_processed( void)
{
   T_MC_JOB*        ptr;
   T_MC_JOB*        p_job = NULL;

   for (ptr = job_queue_head; (ptr != NULL)&&(p_job == NULL); ptr = ptr->next_job)
   {
      if (ptr->state == MC_S_DEFINED)
      {
         p_job = ptr;
      }
   }
   return p_job;
}

/*********************************************************************/
/*  Name:       confirm_procedure_simple_sync()                      */
/*  Function:   confirm procedure for the multicast service          */
/*              "simple" and "sync". A reply message has been        */
/*              received. Assuming a UIC-Telegram.                   */
/*  returns :   void                                                 */
/*********************************************************************/
static void confirm_procedure_simple_sync(  UNSIGNED8          caller_fct,
                                            void*              p_call_ref,
                                            const AM_ADDRESS*  p_replier,
                                            void*              p_inmsg,
                                            CARDINAL32         inmsg_size,
                                            AM_RESULT          status )
{
  UNSIGNED8*   p_ds =  p_call_ref;
  UIC_Msg      msg_converted, *p_msg;

  UNREFERENCED (inmsg_size);
  UNREFERENCED (p_replier);
  UNREFERENCED (caller_fct);


  switch (status)
  {
     case AM_OK:
        if (p_inmsg != NULL)
        {
           wtbconv_00_01__NTOH(p_inmsg, &msg_converted.hdr);
           p_msg = &msg_converted;
           if ((p_msg->hdr.state_cmd <= UIC_S_OK_LAST)&&(p_ds != NULL))
           {
              *p_ds = DEST_SERVED;
              multicast_send_am_ok++;
           }
        }
        break;
       
     case AM_INAUG_ERR:
        uimcs_ctrl.inaug_error = TRUE;
        break;
        
     default:
        break;
  }
  if (p_inmsg != NULL)
  {
      am_buffer_free(p_inmsg, inmsg_size);
  }

  pi_lock_task();
  uimcs_ctrl.instances--;
  pi_unlock_task();
  
  return;
}

/*********************************************************************/
/*  Name:       confirm_procedure_collect()                          */
/*  Function:   confirm procedure for the multicast service collect  */
/*              reply. A reply message has been received. Assuming   */
/*              a UIC-Telegram.                                      */
/*              The telegram will be copied as a bytestream into the */
/*              data buffer of the reply message.                    */
/*  returns :   void                                                 */
/*********************************************************************/
static void confirm_procedure_collect(UNSIGNED8          caller_fct,
                                      void*              p_call_ref,
                                      const AM_ADDRESS*  p_replier,
                                      void*              p_inmsg,
                                      CARDINAL32         inmsg_size,
                                      AM_RESULT          status )
{
  UNSIGNED8    *p_ds =  p_call_ref;
  UIC_Msg      msg_converted, *p_msg;
  UNSIGNED8    *data;

  UNREFERENCED (inmsg_size);
  UNREFERENCED (p_replier);
  UNREFERENCED (caller_fct);


  switch (status)
  {
  case AM_OK:
      if (p_inmsg != NULL)
      {
          /* p_inmsg is a pointer to a received bytestream message */
          /* compare and copy replies */
          /* TO DO */
          wtbconv_00_01__NTOH(p_inmsg, &msg_converted.hdr);
          p_msg = &msg_converted;
          if ((p_msg->hdr.state_cmd <= UIC_S_OK_LAST)&&(p_ds != NULL))
          {
              if (*p_ds != DEST_SERVED) 
              {
                  *p_ds = DEST_SERVED;
                  /* base address */
                  data = uimcs_ctrl.p_job->p_collect_reply + UIC_HEADER_BYTES + 2;
                  data = data + *(uimcs_ctrl.p_job->p_collect_reply + UIC_HEADER_BYTES)
                         * SIZE_OF_COLLECT_REPLY_ELEMENT;
                  pi_copy8(data, p_inmsg, SIZE_OF_COLLECT_REPLY_ELEMENT);
                  *(uimcs_ctrl.p_job->p_collect_reply + UIC_HEADER_BYTES) += 1;
              }
              multicast_send_am_ok++;
          }
        }
        break;
       
     case AM_INAUG_ERR:
        uimcs_ctrl.inaug_error = TRUE;
        break;
        
     default:
        break;
  }
  if (p_inmsg != NULL)
  {
      am_buffer_free(p_inmsg, inmsg_size);
  }

  pi_lock_task();
  uimcs_ctrl.instances--;
  pi_unlock_task();
  
  return;
}

/*********************************************************************/
/*  Name:       save_mc_request_params()                             */
/*  Function:   save all parameters of the multicast request call    */
/*  returns :   MC_OK, MC_ERROR in case of a wrong service or        */
/*              MC_INAUGURATION when a inauguration has been detected*/
/*********************************************************************/
static T_RETURN_VAL save_mc_request_params( void)
{
   uimcs_ctrl.retries            = uimcs_ctrl.p_job->retry_max;
   uimcs_ctrl.replier.fct_or_sta = uimcs_ctrl.p_job->dest_fct;
   uimcs_ctrl.replier.next_sta   = 0;
   uimcs_ctrl.replier.tc         = uimcs_ctrl.topo_counter;
   /* replier.vehicle will be initialized just before the am_call_requ() */

   uimcs_ctrl.instances          = 0;
   uimcs_ctrl.dest_index         = 0;
   uimcs_ctrl.inaug_error        = FALSE;

   switch (uimcs_ctrl.p_job->service)
   {
   case UIC_MCP_COLLECT_REPLY:
       uimcs_ctrl.confirm_hdl  = confirm_procedure_collect;
       break;
   case UIC_MCP_SYNC:
   case UIC_MCP_SIMPLE:
       uimcs_ctrl.confirm_hdl  = confirm_procedure_simple_sync;
       break;
     
   default:
       /* wrong service */
       return MC_ERROR;
       break;
   }
       
   uimcs_ctrl.job_done     = remove_and_free_job;
       
   if ((uimcs_ctrl.p_job->topo_cnt != 0)
           && (uimcs_ctrl.p_job->topo_cnt != uimcs_ctrl.topo_counter))
   { 
       return MC_INAUGURATION;
   }
   return MC_OK;
}

/*********************************************************************/
/*  Name:       send_multicast()                                     */
/*  Function:   performs am_call_requ() for each destination         */
/*              adressed.                                            */
/*  returns :   MC_DONE, when the message is sent to all destinations*/
/*              MC_INAUGURATION, when an inauguration occurred       */
/*********************************************************************/
static T_RETURN_VAL send_multicast( void)
{

   while (  (uimcs_ctrl.instances < uimcs_config.open_calls_max) 
         && (uimcs_ctrl.dest_index < MC_DEST_MAX)
         && (uimcs_ctrl.inaug_error != TRUE)
         )
   {
      /* loop all destinations */
      if (uimcs_ctrl.dest[uimcs_ctrl.dest_index].state == DEST_ADDRESSED)
      {
         pi_lock_task();
         uimcs_ctrl.instances++;
         pi_unlock_task();
         uimcs_ctrl.dest[uimcs_ctrl.dest_index].state = DEST_CALLED;
         uimcs_ctrl.replier.vehicle = uimcs_ctrl.dest[uimcs_ctrl.dest_index].wtb_addr;
         multicast_send++;

         am_call_requ( UIC_F_UMS, &(uimcs_ctrl.replier), 
              uimcs_ctrl.p_job->p_msg, uimcs_ctrl.p_job->msg_size,
              NULL, uimcs_config.reply_len_max,
              uimcs_config.reply_timeout,
              uimcs_ctrl.confirm_hdl,
              &(uimcs_ctrl.dest[uimcs_ctrl.dest_index].state));
      }
      uimcs_ctrl.dest_index++;
   }
  
   if (uimcs_ctrl.inaug_error == TRUE)
   {
      return MC_INAUGURATION;
   }
   else
   {
      return MC_DONE;
   }
}

/*********************************************************************/
/*  Name:       look_for_replies()                                   */
/*  Function:   Die Funktion soll die zwischenzeitlich eingetroffenen*/
/*              Antworten von den Zielstationen eines Multicast      */
/*              durchsehen.                                          *
/*  returns :   MC_WAITING, wenn nicht alle replies vorhanden sind   */
/*              MC_INAUGURATION, wenn seit dem Sendezeitpunkt eine   */
/*                              Taufe stattgefunden hat              */
/*              MC_DONE, wenn alle replies eingetroffen sind         */
/*              MC_INCOMPLETE, wenn noch nicht alle Zielstationen    */
/*                             bedient, bzw. ein retry läuft         */
/*********************************************************************/
static T_RETURN_VAL look_for_replies( void)
{
   BOOLEAN              done = TRUE;
   UNSIGNED8            i;


   if (uimcs_ctrl.instances != 0)    
   {
      return MC_WAITING;
   }
   
   if (uimcs_ctrl.inaug_error == TRUE)
   {
      return MC_INAUGURATION;
   }
   
   for (i = 0; i < MC_DEST_MAX; i++)
   {
      switch (uimcs_ctrl.dest[i].state)
      {
         case DEST_CALLED:
            /* This may initiate a retry of transmission */
            uimcs_ctrl.dest[i].state = DEST_ADDRESSED;
            done = FALSE;
            break;
         case DEST_ADDRESSED:
            done = FALSE;
            break;
         case DEST_UNDEFINED:
         case DEST_SERVED:
            break;
      }
   }

   if (done)
   {
      return MC_DONE;
   }
   else
   {
      return MC_INCOMPLETE;
   }
}

/*********************************************************************/
/*  Name:       get_destination_converted()                          */
/*  Function:   converts UIC into TCN address by calling the UNGS    */
/*  returns :   MC_OK                                                */
/*              MC_ERROR, when address conversion failed             */
/*********************************************************************/

static T_RETURN_VAL get_destination_converted(UNSIGNED8    uicAddress,
                                              T_UIMCS_CTRL *p_uimcs_ctrl)
{
    UIC_CvtUicToTcnList  *p_reply;
    UNSIGNED32           reply_size;
    int                  i;
    UNSIGNED8            former_address;
    T_RETURN_VAL         retval = MC_OK;

    /* calling the ungs at this point is allowed, since this is still the */
    /* procedure call from the uagt. There is no concurrent read from the */
    /* NADI.                                                              */
    if (ungs_UIC_TCN_convert(uicAddress, &p_reply, &reply_size) == UIC_S_OK)
    {
        if ((p_reply->num_tcnaddr != 0) && (p_reply->num_tcnaddr <= MC_DEST_MAX))
        {
            p_uimcs_ctrl->topo_counter = p_reply->topo_counter;
            former_address = INVALID_TCN_ADDRESS;
            for (i=0 ; i < p_reply->num_tcnaddr; i++)
            {
                if ( p_reply->tcn_addr[i] != former_address)
                {
                    p_uimcs_ctrl->dest[i].wtb_addr = p_reply->tcn_addr[i];
                    former_address = p_reply->tcn_addr[i];
                    p_uimcs_ctrl->dest[i].state    = DEST_ADDRESSED;
                }
                else 
                {
                    p_uimcs_ctrl->dest[i].wtb_addr = INVALID_TCN_ADDRESS;
                    p_uimcs_ctrl->dest[i].state    = DEST_UNDEFINED;
                }
            }
        
            for (    ; i < MC_DEST_MAX; i++ )
            {
                p_uimcs_ctrl->dest[i].wtb_addr = INVALID_TCN_ADDRESS;
                p_uimcs_ctrl->dest[i].state    = DEST_UNDEFINED;
            }
        }
        else
        {
            retval = MC_ERROR;
        }
    }
    else 
    {
        retval = MC_ERROR;
    }
    PI_FREE (p_reply);
    return retval;
}

/*********************************************************************/
/*  Name:       create_multicast_job()                               */
/*  Function:   enters a request into the multicast job processing   */
/*              queue.                                               */
/*  returns :   UIC_S_ERR_MC_SERVICE on wrong service                */
/*              UIC_S_ERR_OVERLOAD, when no new job can be created   */
/*              UIC_S_ERR_MC_DEST on a invalid destination address   */
/*              UIMCS_OK                                             */
/*********************************************************************/
static UIMCS_T_RESULT create_multicast_job(
           UIC_MCreateRequ*                  p_msg, 
           CARDINAL32                        msg_size, 
           UNSIGNED8                         topo_counter,
           UIC_MCreateReply*                 p_reply
          )
{
   T_MC_JOB*        p_job = NULL;
   int              retval = UIMCS_OK;


   /*
    * initialize reply parameters
    */
   p_reply->call_id = 0; 
   switch ( p_msg->mc_ctrl.mc_service )
   {
      case UIC_MCP_SIMPLE:
          break;
      case UIC_MCP_SYNC:
      case UIC_MCP_COLLECT_REPLY:
          retval = UIMCS_OK_SYNC;
          break;
      default:
          p_reply->hdr.state_cmd = (UNSIGNED8)UIC_S_ERR_MC_SERVICE;
          return retval;
          break;
   }

   /*
    * preset a new multicast job descriptor and append it to the queue
    */
   if ((p_job = set_new_job(p_msg, msg_size, topo_counter)) == NULL)
   {
      p_reply->hdr.state_cmd = (UNSIGNED8)UIC_S_ERR_OVERLOAD;
      return UIMCS_OK;
   }
   
   if (append_job_to_queue( p_job) != MC_OK)
   {
      free_mc_collect_replymsg(p_job);
      free_job(p_job);
      p_reply->hdr.state_cmd = (UNSIGNED8)UIC_S_ERR_OVERLOAD;
      return UIMCS_OK;
   }

   p_reply->call_id = p_job->identifier;

   if (get_destination_converted(p_msg->mc_ctrl.mc_destination_veh,&uimcs_ctrl)!= MC_OK)
   {
       p_reply->hdr.state_cmd = (UNSIGNED8)UIC_S_ERR_MC_DEST;
       if (remove_job_from_queue(p_job) == MC_OK)
       {
           free_mc_collect_replymsg(p_job);
           free_job( p_job);
       }
       return UIMCS_OK;
   }
   return retval;  
}

/*********************************************************************/
/*  Name:       send_reply_to_caller()                               */
/*  Function:   In case of service UIC_MCP_SYNC the function sends   */
/*              the reply to the calling function.                   */
/*  returns :   void                                                 */
/*********************************************************************/
void send_reply_to_caller(T_RETURN_VAL ret)
{
    int size;
    void * p_uic_replymsg;
    UIC_Msg msg_out;

    switch (uimcs_ctrl.p_job->service)
    {
    case UIC_MCP_SYNC:
        size = UIC_HEADER_BYTES;
        size += size & 0x01;
        if ((p_uic_replymsg = PI_ALLOC (size)) == NULL) return;
        break;
    case UIC_MCP_COLLECT_REPLY:
        /* recalculate the size to be transmitted */
        size = UIC_HEADER_BYTES + 2 + 
            (SIZE_OF_COLLECT_REPLY_ELEMENT * (*(uimcs_ctrl.p_job->p_collect_reply + UIC_HEADER_BYTES)));
        p_uic_replymsg = uimcs_ctrl.p_job->p_collect_reply;
        /* ptr must be NULL; otherwise p_collect_reply would be free'd */
        uimcs_ctrl.p_job->p_collect_reply = NULL;
        break;
    default:
        return;
        break;
    }

    msg_out.hdr.owner       = uimcs_ctrl.p_job->owner;
    msg_out.hdr.reserved1   = 0;
    msg_out.hdr.dest_veh    = uimcs_ctrl.p_job->src_veh;
    msg_out.hdr.dest_fct    = uimcs_ctrl.p_job->src_fct;
    msg_out.hdr.src_veh     = uwtm_my_uic_addr();
    msg_out.hdr.src_fct     = UIC_F_UMS;
    msg_out.hdr.code        = UIC_FR_MULTICAST_CREATE;
    msg_out.hdr.reserved2   = 0;
    
    if (ret == MC_DONE)
    {
        msg_out.hdr.state_cmd   = UIC_S_OK;
    }
    else 
    {
        msg_out.hdr.state_cmd   = UIC_S_ERR_MC_SEND;
    }
        
    wtbconv_00_01__HTON(p_uic_replymsg, &msg_out.hdr);
    
    /* Save pointer to reply message and pointer to the free function */
    /* returned by the provider ungs for the specific instance of the */
    /* uagt.                                                          */
    uimcs_ctrl.p_job->reference_from_uagt->p_buffer = p_uic_replymsg;
    uimcs_ctrl.p_job->reference_from_uagt->p_buffer_free_fct = PI_FREE;
    
    am_reply_requ(  uimcs_ctrl.p_job->replier_fct_from_uagt,
                    p_uic_replymsg,     /* out_msg_addr */
                    size              , /* out_msg_size */
                    uimcs_ctrl.p_job->reference_from_uagt, /* extern reference */
                    AM_OK );

    return;
}

/*********************************************************************/
/*  Name:       uimcs_handler()                                      */
/*  Function:   The handler has to be activated cyclic in order to   */
/*              process all jobs queued.                             */
/*  returns :   void                                                 */
/*********************************************************************/
void uimcs_handler( void)
{
    T_RETURN_VAL             retval;
    UNSIGNED8 *p_owner;
    multicast_handler++;
   
    switch (uimcs_ctrl.state)
    {
    case STATE_IDLE:
        multicast_state_idle++;
        /* initial state; look for a new job to be processed */
        if ((uimcs_ctrl.p_job = get_next_job_processed()) != NULL)
        {
            if ((retval = save_mc_request_params()) == MC_OK)
            {
                /* new job prepared */
                uimcs_ctrl.p_job->state = MC_S_IN_PROGRESS;
                retval = send_multicast();
                if ((retval == MC_DONE)||(retval == MC_INAUGURATION))
                {
                    uimcs_ctrl.state = STATE_WAITING_FOR_REPLY;
                }
            }
            else
            {
                multicast_wrong_service++;
                free_mc_collect_replymsg(uimcs_ctrl.p_job);
                /* wrong service or inauguration happened */
                retval = remove_job_from_queue( uimcs_ctrl.p_job);
                if (retval == MC_OK)
                {
                    /* free just the specific job */
                    free_job( uimcs_ctrl.p_job);
                }
                else
                {
                    /* clear the whole job queue */
                    clear_job_queue();
                }
            }
        }
        break;

      
    case STATE_CALLING:
        multicast_state_calling++;
        retval = send_multicast();
        if ( (retval == MC_DONE) || (retval == MC_INAUGURATION))
        {
            uimcs_ctrl.state = STATE_WAITING_FOR_REPLY;
        }
        break;

      
    case STATE_WAITING_FOR_REPLY:
        multicast_state_waiting_for_reply++;
        retval = look_for_replies();
        switch (retval)
        {
        case MC_WAITING:
            /* do not change this state */
            break;
            
        case MC_DONE:
            multicast_complete++;
            /* message is sent to all destinations */
            if ((uimcs_ctrl.p_job->mc_code == UIC_FC_DEL_CONFIG) ||
                (uimcs_ctrl.p_job->mc_code == UIC_FC_WRITE_CORRECTION) ||
                (uimcs_ctrl.p_job->mc_code == UIC_FC_WRITE_VEH_RESNUM))
            {
                uimcs_ctrl.state = STATE_WAITING_FOR_START_INAUGURATION;
                send_reply_to_caller(MC_DONE);
                break;
            }
            send_reply_to_caller(MC_DONE);
            p_owner = (UNSIGNED8*)(uimcs_ctrl.p_job->p_msg);
#ifdef O_KFT
            if ((uimcs_ctrl.p_job->mc_code == FC_WRITE_KFTCORR)&&
                ( *(p_owner + UIC_HEADER_OWNER_OFFSET) == UIC_O_DB))
            {
                uimcs_ctrl.state = STATE_WAITING_FOR_START_INAUGURATION;
                break;
            }
#endif
            uimcs_ctrl.state = STATE_IDLE;
            free_mc_callmsg( uimcs_ctrl.p_job);
            uimcs_ctrl.job_done();
            break;
            
        case MC_INCOMPLETE: 
            uimcs_ctrl.dest_index = 0;
            if (uimcs_ctrl.retries > 0)
            {
                multicast_retries++;
                uimcs_ctrl.retries--;
                uimcs_ctrl.state = STATE_CALLING;
            }
            else
            {
                multicast_incomplete++;
                send_reply_to_caller(MC_INCOMPLETE);
                free_mc_callmsg( uimcs_ctrl.p_job);
                uimcs_ctrl.job_done();
                uimcs_ctrl.state = STATE_IDLE;
            }
            break;
            
        case MC_INAUGURATION:
        default:
            multicast_inauguration_detected++;
            free_mc_collect_replymsg(uimcs_ctrl.p_job);
            /* reset state to IDLE*/
            clear_job_queue();
            uimcs_ctrl.state = STATE_IDLE;
            break;
        }
        break;

          
        case STATE_WAITING_FOR_START_INAUGURATION:
            multicast_state_inauguration++;
            uimcs_ctrl.state = STATE_IDLE;
            free_mc_callmsg( uimcs_ctrl.p_job);
            uimcs_ctrl.job_done();
            uimcs_inaug_dummy_state = UIC_INAUG;
            uic_inaug_enf_requ(&uimcs_inaug_dummy_state);
            break;
         
          
        default:
            multicast_state_default++;
            free_mc_collect_replymsg(uimcs_ctrl.p_job);
            uimcs_ctrl.state = STATE_IDLE;
            break;
    
}
return;
}

/*********************************************************************/
/*  Name:       compute_message_size()                               */
/*  Function:   compute the effective size of the message for the    */
/*              multicast request                                    */
/*  returns :   even size of the message                             */
/*********************************************************************/
static UNSIGNED32 compute_message_size(UIC_MCreateRequ *p_mc_msg,
                                       UNSIGNED32      uic_callmsg_size)
{
    UNSIGNED32  size, last_address;
    UNSIGNED8   index;
            
    switch (p_mc_msg->mc_ctrl.mc_code)
    {
    case UIC_FC_WRITE_CORRECTION:
        if ( p_mc_msg->mc_ctrl.mc_status)
        {
            last_address = (UNSIGNED32)&p_mc_msg->mc_msg_data.correction_data;
        }
        else
        {
            index = p_mc_msg->mc_msg_data.correction_data.num_veh;
            last_address = (UNSIGNED32)&p_mc_msg->mc_msg_data.correction_data.corr_data[index];
        }
        break;
           
    case UIC_FC_WRITE_VEH_RESNUM:
        index = p_mc_msg->mc_msg_data.res_num_data.num_veh;
        last_address = (UNSIGNED32)&p_mc_msg->mc_msg_data.res_num_data.res_data[index];
        break;
           
    case UIC_FC_DEL_CONFIG:
        last_address = ((UNSIGNED32)&p_mc_msg->mc_msg_data);
        break;
           
    case UIC_FC_WRITE_GROUP:
        index = p_mc_msg->mc_msg_data.group.GroupDescr.num_entries;
        last_address = (UNSIGNED32)&p_mc_msg->mc_msg_data.group.GroupVeh[index];
        break;
           
    case UIC_FC_WRITE_ALL_GROUPS:
        index = p_mc_msg->mc_msg_data.group_list.num_groups;
        last_address = (UNSIGNED32)&p_mc_msg->mc_msg_data.group_list.group[index];
        break;
           
    case UIC_FC_DELETE_GROUP:
        last_address = ((UNSIGNED32)&p_mc_msg->mc_msg_data.group_number+1);
        break;
           
    case UIC_FC_DELETE_ALL_GROUPS:
    case UIC_FC_SLEEP:
    default:
        last_address = (UNSIGNED32)&p_mc_msg->mc_msg_data + uic_callmsg_size 
                       - UIC_HEADER_BYTES - UIC_MCCREATE_CTRL_BYTES;
        break;
    }
               
    size = last_address - (UNSIGNED32) &p_mc_msg->hdr;
    size += size & 0x01;           
    return size;
}
   

/*********************************************************************/
/*  Name:       job_free_queue_init()                                */
/*  Function:   allocates memory for the job descripor queue         */
/*  returns :   UIMCS_OK or UIMCS_ERR_ALLOC when alloc failed        */
/*********************************************************************/
static UIMCS_T_RESULT job_free_queue_init( void)
{
   T_MC_JOB*        ptr;
   int              i;

   job_free_queue = PI_ALLOC( sizeof(T_MC_JOB) * uimcs_config.mc_requests_max);
   ptr = job_free_queue;

   if (ptr != NULL)
   {
      for ( i = 0; i < uimcs_config.mc_requests_max; i++, ptr++)
      {
         ptr->p_msg = NULL;
         ptr->p_collect_reply = NULL;
         ptr->reference_from_uagt = NULL;
         ptr->next_job = ptr + 1;
      }
      ptr->next_job = NULL;
   }
   else 
   {
       return UIMCS_ERR_ALLOC;
   }

   return UIMCS_OK;
}

/*********************************************************************/
/*  Name:       job_queue_head_init()                                      */
/*  Function:   initialization of the multicast job description queue*/
/*  returns :   UIMCS_OK or UIMCS_R_FATAL_ALLOC when semaphore failed*/
/*********************************************************************/
static UIMCS_T_RESULT job_queue_head_init( void)
{
   short          retval;
   UIMCS_T_RESULT result = UIMCS_OK;


   job_queue_head = NULL;

   uimcs_semid = pi_create_semaphore( 1, PI_ORD_FCFS, &retval);
   if (retval != PI_RET_OK)
   {
      uimcs_semid = 0;
      result = UIMCS_ERR_ALLOC;
      
   }

   return result;
}

/*********************************************************************/
/*  Name:       uimcs_init()                                         */
/*  Function:   initialization of the multicast server               */
/*  returns :   whatever the functions job_descr_init, job_list_init */
/*              did return                                           */
/*********************************************************************/
UIMCS_T_RESULT uimcs_init(UIMCS_T_CONFIGURATION*   p_config_param)
{  
   UIMCS_T_RESULT retval;
   if (p_config_param != NULL)
   {
      uimcs_config = *p_config_param;
   }
   else
   {
      uimcs_config.mc_requests_max = 0;
      uimcs_config.open_calls_max  = 0;
      uimcs_config.reply_len_max   = 0;
      uimcs_config.reply_timeout   = 0;
   }

   if ( (retval = job_free_queue_init()) == UIMCS_OK)
   {
       if ( (retval = job_queue_head_init()) == UIMCS_OK)
       {
           uimcs_ctrl.state = STATE_IDLE;
       }
   }

   multicast_state_idle                = 0;
   multicast_state_calling             = 0;
   multicast_state_waiting_for_reply   = 0;
   multicast_state_inauguration        = 0;
   multicast_state_default             = 0;
   multicast_complete                  = 0;
   multicast_incomplete                = 0;
   multicast_wrong_service             = 0;
   multicast_retries                   = 0;
   multicast_send_am_ok                = 0;
   multicast_send                      = 0;
   multicast_handler                   = 0;
   multicast_inauguration_detected     = 0;

   return retval;

}

/*********************************************************************/
/*  Name:       uimcs_request()                                      */
/*  Function:   appends a multicast request to the uimcs input queue */
/*  returns :   UIMCS_OK           on success                        */
/*              UIMCS_ERR_CODE   function code unknown               */
/*              UIMCS_ERR_PTR    incorrect pointer                   */
/*              UIMCS_ERR_MSG    wrong message format                */
/*              UIMCS_ERR_REPLY  insufficient memory for reply       */
/*********************************************************************/
UIMCS_T_RESULT uimcs_request(
           void*       p_uic_callmsg,         /* pointer to the call message */
           CARDINAL32  uic_callmsg_size,      /* size of the call message */
           AM_ADDRESS* caller,                /* handle to the caller */
           void**      pp_uic_replymsg,       /* pointer to reply message */
           CARDINAL32* p_uic_replymsg_size,   /* pointer to size of the reply */
           REPLY_FREE* p_reply_free_fct       /* function to free the 
                                                 uic_replymsg memory */
          )
{
   UIC_Header           *p_msg    = (UIC_Header*) p_uic_callmsg;
   UIC_Header           *p_reply  = NULL;
   CARDINAL32           reply_len;
   UNSIGNED8            topo_counter = 0;

   UIC_MCreateRequ      *p_UicMCreateRequCall;
   UIC_MCreateReply     *p_UicMCreateReply;
   UNSIGNED8            *p_byte_in = (UNSIGNED8 *)p_uic_callmsg;
   UNSIGNED32           size;
   int                  retval = UIMCS_OK;

   /*
    * check input parameters: errors are "fatal" (no reply) 
    * (most likely some programming error)
    */
   if (  (p_uic_callmsg       == NULL)
      || (pp_uic_replymsg     == NULL)
      || (p_uic_replymsg_size == NULL)
      || (p_reply_free_fct    == NULL)
      )
   {
      return UIMCS_ERR_PTR;
   }
   
   if (UIC_HEADER_BYTES > uic_callmsg_size)
   {
      *p_uic_replymsg_size = 0;
      return UIMCS_ERR_MSG;
   }

   switch ( NTOH16(p_byte_in + UIC_HEADER_CMD_OFFSET ) )
   {
      case UIC_FC_MULTICAST_CREATE:
               reply_len    = sizeof(UIC_MCreateReply);
               break;
      default:
               return UIMCS_ERR_CODE;
   }

   /* message conversion required */
   /* The conversion of the call  message  */
   /* compute enough size                  */
   size = sizeof(UIC_MCreateRequ) + uic_callmsg_size;
   size += size & 1;
   if ((p_UicMCreateRequCall = (UIC_MCreateRequ*)PI_ALLOC(size)) == NULL) {
       *p_uic_replymsg_size =0;
       retval = UIMCS_ERR_REPLY;
   }
   else {
       /* Convert the call message from a bytestream */
       wtbconv_15_07__NTOH ((UNSIGNED8*)p_uic_callmsg, uic_callmsg_size, 
                                                        p_UicMCreateRequCall);
       /*
        * dispatcher for the services offered
        */
       switch ( p_UicMCreateRequCall->hdr.code )
           {
       case UIC_FC_MULTICAST_CREATE:
           if (caller != NULL)
           {
               topo_counter = caller->tc;
           }

           /* prepare the basic reply message header */
           if ((p_UicMCreateReply = (UIC_MCreateReply*)PI_ALLOC( reply_len)) == NULL ) {
               *p_uic_replymsg_size =0;
               return UIMCS_ERR_REPLY;
           }

           
           /* recalculate the size to be multicasted */
           size = compute_message_size(p_UicMCreateRequCall, uic_callmsg_size);
           retval = create_multicast_job( p_UicMCreateRequCall, size, 
                                          topo_counter, p_UicMCreateReply);
           break;
       default:
           PI_FREE (p_UicMCreateRequCall);
           *p_uic_replymsg_size =0;
           return UIMCS_ERR_CODE;
           break;
       }
       if (retval == UIMCS_OK ) {
           /* prepare the uic reply header */
           exchange_UIC_HEADER((UIC_Msg*)p_UicMCreateReply,
                                                (UIC_Msg*)p_UicMCreateRequCall);
           /* Allocate memory for the bytestream reply */
           if ((*pp_uic_replymsg = PI_ALLOC(sizeof(UIC_Msg))) == NULL) {
               retval = UIMCS_ERR_REPLY;
               *p_uic_replymsg_size =0;
               p_reply_free_fct = NULL;
           }
           else {
               /* Convert the structured message into a bytestream */
               p_UicMCreateReply->hdr.state_cmd = UIMCS_OK;
               wtbconv_15_07A_HTON ((UNSIGNED8*)(*pp_uic_replymsg), p_UicMCreateReply);
               *p_uic_replymsg_size = UIC_SIZEOF_UIC_MCREATEREPLY;
               *p_reply_free_fct    = PI_FREE;
           }
           /* free the structured message allocated formerly in this function */
           PI_FREE (p_UicMCreateReply);
       }
       else {
           if (retval == UIMCS_OK_SYNC) {
               PI_FREE (p_UicMCreateReply);
           }
           *p_uic_replymsg_size =0;
           *pp_uic_replymsg = NULL;    
           p_reply_free_fct = NULL;
       }
       PI_FREE (p_UicMCreateRequCall);
   }   
   return retval;
}
