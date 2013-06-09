/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> md_layer.h definitions used                                                         */
/*==============================================================================================*/

/*



*/
/*
HEADER
*/
/*
document header
*/
   /*****************************************************************
      *   AAA   BBBB   BBBB   |                                        *
      *  A   A  B   B  B   B  |           M o d u l k o p f            *
      *  AAAAA  BBBB   BBBB   |                                        *
      *  A   A  B   B  B   B  |                                        *
      *  A   A  BBBB   BBBB   |      TRAIN COMMUNICATION NETWORK       *
      *                       |                                        *
      *  ASEA  BROWN  BOVERI  |      Copyright 1991 by ABB Henschel    *
      *-----------------------+----------------------------------------*
      *                                                                *
      * Funktionalitaet: MAC - Messenger -Interface                    *
      *                                                                *
      * Erstellt am:     15.12.92                                      *
      * Autor(en):       Peter Gehring                                 *
      * Version:         1.1                                           *
      * Freigegeben am:  XX.XX.92                                      *
      * Abhdngikeiten:   ANSI C                                        *
      *                                                                *
      *----------------------------------------------------------------*
      *                       Kurzbeschreibung                         *
      *                                                                *
      * Diese Modul bietet zum Messenger eine Normkonforme Schnitt-    *
      * stelle.  Der MAC greift |ber Functions auf die zu sendenden    *
      * MD-Daten zu, die in diesem Modul gespeichert werden.           *
      * Bei diesen Functionen nimmt die Laufzeit direkt Einflu_ auf die*
      * response-time auf den Bus.                                     *
      *                                                                *
      *                                                                *
      *----------------------------------------------------------------*
      * Ablage:                MMI.X                                   *
      *----------------------------------------------------------------*
      * Enthaltene Prozeduren: vgl. MMI.H                              *
      *                                                                *
      *----------------------------------------------------------------*
      * Gerufene Prozeduren:   vgl. Includes                           *
      *                                                                *
      *----------------------------------------------------------------*
      * Benvtigten Files:     vgl. Includes directives                 *
      *                                                                *
      *----------------------------------------------------------------*
      * Aenderungen:                                                   *
      *                                                                *
      * version   yy-mm-dd   name             dept.                    *
      * -------   --------   ----             ----                     *
      *   1.0     XX-XX-XX    Gg            WAG/NTL2                   *
      *   1.1     92-12-15    Gg            WAG/NTL2                   *
      *           adaption to new project structure                    *
      *   1.2     93-02-01    Gg            WAG/NTL2                   *
      *           messenger interface with conditional compiler        *
      *   1.2     93-04-14    Gg            WAG/NTL2                   *
      *           md_free_packet added                                 *
      *                                                                *
      *   1.3     93-12-12 Diagnoseeintrag DIAG_QUEUE_FLUSH unterdr|cken,da*
      *           der Messenger alle 3/4 Zyklen die Funktion aufruft   *
      *                                                                *
      *   1.4.    93-12-12 Kvr lm_X_send_request() erweitert (Hvpli)   *
      *                                                                *
      *   1.5.    94-02-01 Kvr MD_PACKET2 in MD_PACKET umbenannt       *
      *                                                                *
      *   1.6.    94-02-09 Kvr lm_X_send_request() gedndert            *
      *                                                                *
      *   1.7.    94-02-21 Kvr alle INCLUDES ohne Pfadangabe           *
      *                        #ifdef lm_v_system_init an richtige Stelle*
      *----------------------------------------------------------------*
      *   3.0     95-02-09 Br transform for XTOOLS                     *
      *                       - lm_v_system_init deleted               *
      *                       - condit. comp. for tcn_VB deleted       *
      *                       - ctrl.h replaced with bmi.h             *
      *                       - tcn.h changed to ll.h                  *
      *                       - coco.h added                           *
      *                       - type_zas_element -> Type_ZASElement    *
      *                       - messenger_zas revised to new struct    *
      *                       - mac_md_zas revised to new struct       *
      *                       - zas_data revised to new struct         *
      *                       - ls_t_GetNodeStatus -> ls_t_GetStatus   *
      *                       - WTB_status revised to new struct       *
      *                       - checking of comp.sw. deleted           *
      *   3.1     95-04-04 Le - ZAS removed                            *
      *                       - subscription with BMI added            *
      *                       - bug in md_flush fixed                  *
      *   3.2     95-04-30 Le - bug in lm_get_packet removed           *
      *                       - pi_enable/disable added when           *
      *                       - accessing pool                         *
      *                       - cleaned up                             *
      *   3.3     95-05-30 Le - lm_t_send_request: counter bug         *
      *                         corrected                              *
      *   3.4     95-07-05 Le - md_status_indicate via ZAS to BMI      *
      *   3.5     95-09-20 Le - mmi_zas_entry moved to MMI             *
      *                       - function bmi_md_status_ind             *
      *                                                                *
      *----------------------------------------------------------------*
      *           96-04-10 Kvr- adapted to Norm 495                    *
      *                         new lm-interface with changed names    *
      *                         lm_t -> lm_m_t because messenger has   *
      *                         changed names, and new parameter bus_id*
      *           96-04-11 Kvr- packet structures in mmi.h deleted and *
      *                         am_sys.h and wtbdrv.h therefore included*
      *                         (MD_PACKET + type_idu_drv)             *
      *   4.1     26.09.96 cs   (19) undefined return value            *
      *   4.1     27.01.97 cs   (44) diag-error codes now in diag.h    *
    ******************************************************************/
/*
include files
*/
#ifdef O_960
#include "coco.h"
#endif
/*
includes
*/
#include "tcntypes.h"
#include "bmi.h"  /* busmanagement interface headerfile */
#include "ll.h"   /* TCN-WTB link layer header*/
#if defined(O_960) || defined(O_VCU6U_GW)
#include "pi_sys.h"
#else
#include "pil.h"
#endif
#include "diagwtb.h"      /* diagnostic headerfile */
#include "wtbdrv.h"    /* before mmi.h */
#define  WAG           /* for MD_PACKET in am_sys.h */
#include "am_sys.h"    /* before mmi.h */
#include "mmi.h"
#include "zas.h"
/*
defines
*/
#define MD_MAX_PACKET     0x80   /* maximale MD-Packet Anzahl zum Senden */
#define MD_MAX_REC_PACKET 0x200   /* Ringbufferldnge Empfangsliste */
     /* !!! very bad style !!! use md_layer.h for the following definitions */
/* <ATR:01> */
// #define MD_RECEIVE_ACTIVE       0x01   /* status-bits */
// #define MD_RECEIVE_OVERFLOW     0x02
// #define MD_SEND_ACTIVE          0x04
// #define MD_OK                   0x00   /* return values */
// #define MD_FAILURE              0x01
// #define MD_HIGH_PRIO               1
// #define MD_PENDING                 2
// #define MD_SENT                    3
// #define MD_FLUSHED                 4
#define MD_RECEIVE_ACTIVE       MD_RECV_ACTIVE   /* status-bits */
#define MD_RECEIVE_OVERFLOW     MD_RECV_OVF
/*
call-back function prototypes
*/
/* function-addresses indirect-calls to the messenger */
void (*lm_get_packet) (void **owner, MD_PACKET **received_packet);
void (*lm_receive_indicate) (void);
void (*lm_send_confirm) (MD_PACKET *sent_packet);
/*
global variables
*/
void  **pool_identifier; /* address of a pointer to the memeory pool */
unsigned short md_rec_ind_count; /* counter for the received packets */
unsigned char  mac_status; /* MAC (MMI) - statusvariable */
unsigned short md_packet_count;
/* Zaehler f|r die PACKET Sendezahl \berwachung ;
   <= wortgroesse der CPU */
unsigned short md_rec_overflow;
 /* counter for received MD-packets that can't be queued */
unsigned short  md_rec_lz; /* "lesezeiger" of the received-MD-packet-queue */
unsigned short  md_rec_sz; /* "schreibzeiger" of the received-MD-packet-queue */
MD_PACKET *md_rec_list[MD_MAX_REC_PACKET];
/* of the received-MD-packet-queue */
  /*
  variables declaration at MAC side
  */
        unsigned char send_enable;                /* flag, which enables MD data transmission or not */
        unsigned short md_rec_count;              /* counter for the received MD-packets */
        unsigned short md_send_low_count;         /* counter for the sended MD-packets (low priority)  */
        unsigned short md_send_high_count;        /* counter for the sended MD-packets (high priority) */
        unsigned short md_send_low_lz;             /* messagedata send list low priority lesezeiger */
        unsigned short md_send_low_sz;             /* messagedata send list low priority schreibzeiger */
        unsigned short md_send_high_lz;            /* messagedata send list high priority lesezeiger */
        unsigned short md_send_high_sz;            /* messagedata send list high priority schreibzeiger */
        MD_PACKET *md_send_low_list [MD_MAX_PACKET]; /* messagedata send list low priority */
        MD_PACKET *md_send_high_list[MD_MAX_PACKET]; /* messagedata send list high priority */
        unsigned short md_low_lost;               /* counter for the lost low priority frames */
        unsigned short md_high_lost;              /* counter for the lost high priority frames */
        MD_PACKET *poi2sendedpacket;                 /* store sended packet for confirmation */
        unsigned short md_conf_count;             /* counter for the confirmation calls */
     Type_ZASElement msg2mmi ; /* message to MMI */
/*
local function prototypes
*/
  static void mmi_zas_entry (Type_ZASElement * msg);
/*
void lm_t_system_init (void)
*/
/*
link layer interface for message data
*/

   /*
   void lm_t_system_init (..)
   */

      void lm_t_system_init (void)
{
   lm_receive_indicate = (void (*) (void)) 0;
   lm_get_packet       = (void (*) (void **, MD_PACKET **)) 0;
   lm_send_confirm     = (void (*) (MD_PACKET *)) 0;;
   pool_identifier     = NULL;
   md_rec_overflow     = 0;
   /* Empfangs-Paket - Queue initialisieren */
   md_rec_lz           = 0;
   md_rec_sz           = 0;
   md_packet_count     = 0; /* Packetzaehlerueberwachung RESET */
   mac_status          = 0;
   md_rec_ind_count    = 0;
   zas_subscribe (PNR_MMI, ( void(*)(void*) )mmi_zas_entry) ; /* csXXX*/
}
/*
void lm_m_t_init(unsigned short bus_id,
                    void (*rec_confirm)(void),
                    void (*get_a_packet)(void** owner, MD_PACKET **a_packet),
                    void **owner,
                    void (*put_a_packet)  (MD_PACKET *a_packet),
                    void (*status_changed) (int result)
                  )
*/
/*
void lm_m_t_init(...)
*/

   void lm_m_t_init(unsigned short bus_id,
                    void (*rec_confirm)(void),
                    void (*get_a_packet)(void** owner, MD_PACKET **a_packet),
                    void **owner,
                    void (*put_a_packet)  (MD_PACKET *a_packet),
                    void (*status_changed) (int result)
                  )
{
   /*
   install the messenger functions
   */
         bus_id              = bus_id; /* avoid warnings*/
         lm_receive_indicate = rec_confirm;
         lm_get_packet       = get_a_packet;
         lm_send_confirm     = put_a_packet;
         pool_identifier     = owner;         /* reference to the memory-pool */
         send_enable         = TRUE; /* the messagedata transfer is permitted */
   bmi_bind_mmi (status_changed) ;
   /*zas_subscribe (PNR_MMI, ( void(*)(void*) )mmi_zas_entry) ;  csXXX*/
}
/*
void lm_m_t_send_request(  unsigned short bus_id,
                              unsigned char sourcedevice,
                              unsigned char destinationdevice,
                              MD_PACKET *packet_to_send2,
                              int *result )
*/
/*
void lm_m_t_send_request(...)
*/

   /* #pragma argsused #iMB */
   void lm_m_t_send_request(  unsigned short bus_id,
                              unsigned char sourcedevice,
                              unsigned char destinationdevice,
                              MD_PACKET *packet_to_send2,
                              int *result )
{
   bus_id = bus_id; /* avoid warnings */
   /* send a MD-packet */
   /* ACHTUNG nicht unendlich viele Pakete an den MAC verschicken */
   if (md_packet_count < (MD_MAX_PACKET - 1))
   {
      /* packet_to_send->sendframe.sd  set by MAC */
      packet_to_send2->port.lk.version.wtb.dest  = destinationdevice; /* #kvr 11.04.96 */
      /* enter the send packet into the right send-list */
      if (packet_to_send2->control == MD_HIGH_PRIO)
      {
         if ( (md_send_high_sz+1) % MD_MAX_PACKET != md_send_high_lz)
         {
            md_send_high_list[md_send_high_sz] = ((MD_PACKET *) packet_to_send2);
            md_send_high_sz = (md_send_high_sz + 1) % MD_MAX_PACKET;
            /* Zaehler fuer die max. Zahl der Packete im MAC ueberwachen */
            md_packet_count++;
            /* set the packet status */
            packet_to_send2->status = MD_PENDING;
            *result = MD_OK;
         }
         else
         {
            /* the packet can't queued - packet lost */
            md_high_lost++;
            diagwtb_entry(DIAG_MMI_ERR_LOW_WS_FULL,0,0,0,packet_to_send2);
            *result = MD_FAILURE;
         }
      }
      else
      {
         /* send with low priority */
         if ( (md_send_low_sz+1) % MD_MAX_PACKET != md_send_low_lz)
         {
            md_send_low_list[md_send_low_sz] = ((MD_PACKET *) packet_to_send2);
            md_send_low_sz = (md_send_low_sz + 1) % MD_MAX_PACKET;
            /* Zaehler fuer die max. Zahl der Packete im MAC ueberwachen */
            md_packet_count++;
            /* set the packet status */
            packet_to_send2->status = MD_PENDING;
            *result = MD_OK;
         }
         else
         {
            /* the packet can't queued - packet lost */
            md_low_lost++;
            diagwtb_entry(DIAG_MMI_ERR_HIGH_WS_FULL,0,0,0,packet_to_send2);
            *result = MD_FAILURE;
         }
      }
   }
   else
   {
      *result = MD_FAILURE;
   }
}
/*
void lm_m_t_receive_poll(unsigned short bus_id,
                            unsigned char  *sourcedevice,
                            unsigned char  *destinationdevice,
                            MD_PACKET      **receive_packet2,
                            int            *result )
*/
/*
void lm_m_t_receive_poll(...)
*/

   void lm_m_t_receive_poll(unsigned short bus_id,
                            unsigned char  *sourcedevice,
                            unsigned char  *destinationdevice,
                            MD_PACKET      **receive_packet2,
                            int            *result )
{
   bus_id = bus_id; /* avoid warnings */
   /* request for received MD-packets */
   /* test for entries in the rec-list */
   if (md_rec_sz != md_rec_lz)
   {
      /* read the next packet from the rec_list */
      *receive_packet2 = (MD_PACKET*) md_rec_list[md_rec_lz];
      *sourcedevice   = md_rec_list[md_rec_lz]->port.lk.version.wtb.src;
      *destinationdevice = md_rec_list[md_rec_lz]->port.lk.version.wtb.dest;
      *result = MD_OK;
      /* increment the read index */
      md_rec_lz = (md_rec_lz + 1) % MD_MAX_REC_PACKET;
   }
   else
   {
      /* no packet received */
      *result = MD_FAILURE;
      *receive_packet2 = NULL;
   }
}
/*
void lm_m_t_get_status(unsigned short bus_id,
                          unsigned short status_selector,
                          unsigned short status_reset,
                          unsigned short *status_result    )
*/
/*
void lm_m_t_get_status(...)
*/

   void lm_m_t_get_status(unsigned short bus_id,
                          unsigned short status_selector,
                          unsigned short status_reset,
                          unsigned short *status_result    )
{
   bus_id = bus_id; /* avoid warnings */
   /* function for MMI-status request */
   /* R|ckgabeparameter ermitteln */
   *status_result = mac_status & status_selector;
   /* RESET-Bits lvschen */
   /*                     AND             XOR                   */
   mac_status = mac_status & (status_reset ^ 0xFF);
}
/*
void lm_m_t_send_queue_flush ( unsigned short bus_id )
*/
/*
void lm_m_t_send_queue_flush(void)
*/

   void lm_m_t_send_queue_flush ( unsigned short bus_id )
{
   bus_id = bus_id; /* avoid warnings */
}
/*
void mmi_zas_entry (Type_ZASElement * msg)
*/
/*
void mmi_zas_entry (Type_ZASElement * msg)
*/

   void mmi_zas_entry (Type_ZASElement * msg)
{
   bmi_md_status_ind ((unsigned short)msg->p1) ;
}
/*
void mac_md_system_init (void)
*/
/*
mac - messenger - interface at the MAC-modul
*/
   /* functions used by the MAC-module */

   /*
   void mac_md_system_init (void)
   */

      void mac_md_system_init (void)
{
   send_enable        = FALSE;
   md_rec_count       = 0;
   md_send_low_count  = 0;
   md_send_high_count = 0;
   md_low_lost        = 0;
   md_high_lost       = 0;
   poi2sendedpacket   = (MD_PACKET *) 0;
   md_conf_count      = 0;
   /* initialize the index of the send-queues */
   md_send_low_sz  = 0;
   md_send_low_lz  = 0;
   md_send_high_sz = 0;
   md_send_high_lz = 0;
   msg2mmi.pnr = PNR_MMI ;
   msg2mmi.enr = MMI_BM_Report ;
}
/*
unsigned char md_check_for_data (void)
*/
/*
unsigned char md_check_for_data (void)
*/

   unsigned char md_check_for_data (void)
{
   /* check if MD-frame's available */
   return (  ((md_send_low_lz != md_send_low_sz) || (md_send_high_lz != md_send_high_sz)) && (send_enable));
}
/*
void md_receive_indicate (void *receive_frame )
*/
/*
void md_receive_indicate (void *receive_frame )
*/

   void md_receive_indicate (void *receive_frame )
{
   /* indicate a MD frame */
      MD_PACKET     *mypacket;
   if (send_enable)
   {
      /* increment the receive-packet counter */
      md_rec_count++;
      /* lm_get_packet installed ? */
      if ( lm_get_packet !=  (void (*) (void **, MD_PACKET **)) 0)
      {
         /* get an empty packet from the pool */
         pi_disable();
         lm_get_packet(pool_identifier, &mypacket);
         pi_enable();
         if (mypacket != NULL)
         {
            /* copy frame into the packet */
            /*pi_copy8 ( (void*) & ( (PACKET *)mypacket)->sendframe, receive_frame, ((unsigned short) ((type_md_idu *)
            receive_frame)
            ->size) + 4);*/
            /* #kvr 11.04.96 - MD_PACKET from am_sys.h  */
            pi_copy8 ( (void*) &mypacket->port.lk.version.wtb.dest, receive_frame, ((unsigned short) ((type_idu_drv *
            )receive_frame
            )->size) + 4);
            /* initialize NEXT-parameter */
            mypacket->next = NULL;
            /* Packet queuen */
            md_rec_ind_count++;
            /* Ist in der Empfangspakete-Queue noch Platz */
            if ( (md_rec_sz+1) % MD_MAX_REC_PACKET != md_rec_lz )
            {
               md_rec_list[md_rec_sz] = ((MD_PACKET*)mypacket);
               md_rec_sz = (md_rec_sz+1) % MD_MAX_REC_PACKET;
            }
            else
            {
               /* !!! Achtung in diesem Fall bleibt das Paket auf ewig blockiert */
               /* Datenverlust   !!! must not occur */
               md_rec_overflow++;
               /* diagnostic message */
               diagwtb_entry(DIAG_MMI_ERR_MD_PACKET_LOST,0,0,0, (void*)0);
            }
#ifdef NEVERDEF
            /* Transportebene aktivieren */
            if (lm_receive_indicate != (void (*) (void)) 0)
            {
               lm_receive_indicate();
            }
#endif
         }
         else
         {
            /* diagnostic message */
            diagwtb_entry(DIAG_MMI_ERR_MD_FRAME_LOST,0,0,0,receive_frame);
         }
      }
      else
      {
         /* Fehlermeldung */
         mac_status = mac_status | MD_RECEIVE_OVERFLOW;
         /* diagnostic message */
         diagwtb_entry(DIAG_MMI_ERR_MD_FRAME_LOST,0,0,0,receive_frame);
      }
   }
}
/*
void * md_request_to_send (unsigned char *destination)
*/
/*
void * md_request_to_send (unsigned char *destination)
*/

   void * md_request_to_send (unsigned char *destination)
{
   /* request for one MD-frame to send */
   type_idu_drv * myframe;
   if (send_enable)
   {
      if (poi2sendedpacket == (MD_PACKET *) 0)
      {
         if ( md_send_high_lz != md_send_high_sz )
         {
            /*
            send high priority MD frame
            */
               /* store pointer to the actual send frame for confirmation */
                  poi2sendedpacket = md_send_high_list[md_send_high_lz];
               /* set the return value */
                  myframe = (type_idu_drv *) &(poi2sendedpacket->port.lk.version.wtb.dest); /* #kvr 11.04.96 */
               /* read the destination address from the sendframe */
                  *destination = myframe->dd;
               /* increment send-packet-counter for high priority */
                  md_send_high_count++;
               /* set the read index to the next element in the send list */
                  md_send_high_lz = (md_send_high_lz + 1) % MD_MAX_PACKET;
         }
         else
         {
            if ( md_send_low_lz != md_send_low_sz )
            {
               /*
               send low priority MD frame
               */
                  /* store pointer to the actual send frame for confirmation */
                     poi2sendedpacket = md_send_low_list[md_send_low_lz];
                  /* set the return value */
                     myframe = (type_idu_drv *) &(poi2sendedpacket->port.lk.version.wtb.dest); /* #kvr 11.04.96 */
                  /* read the destination address from the send frame */
                     *destination = myframe->dd;
                  /* increment the send -packet-counter */
                     md_send_low_count++;
                  /* increment the read index of the low priority send list */
                     md_send_low_lz = (md_send_low_lz + 1) % MD_MAX_PACKET;
            }
            else
            {
               /* nothing to send */
               myframe = NULL;
            }
         }
      }
      else
      {
         diagwtb_entry(DIAG_MMI_ERR_MD_CONFIRM,0,0,
         0x0001, (void*)0);
         myframe = NULL; /*4.1-19*/
      }
   }
   else
   {
      /* nothing to send */
      myframe = NULL;
   }
   return ((void*) myframe);
}
/*
void md_send_confirm (void)
*/
/*
void md_send_confirm (void)
*/

   void md_send_confirm (void)
{
   /* confirm a sended MD-frame */
   if (poi2sendedpacket != (MD_PACKET *) 0)
   {
      /* sign STATUS-flag in the sended packet as sended */
      poi2sendedpacket->status = MD_SENT ;
      md_conf_count++;
      /* set the mac-status of send-active */
      mac_status |= MD_SEND_ACTIVE;
      /* Packetzaehlerueberwachung dekrementieren */
      /* Zaehler fuer die max. Zahl der Packete im MAC ueberwachen */
      md_packet_count--;
      /* CONFIRM-function installed ? */
      if ( lm_send_confirm != (void (*) (MD_PACKET *)) 0 )
      {
         pi_disable();
         lm_send_confirm( (MD_PACKET *) poi2sendedpacket);
         pi_enable();
      }
   }
   else
   {
      diagwtb_entry(DIAG_MMI_ERR_MD_CONFIRM,0,0,0x0002, (void*)0);
   }
   /* reset pointer */
   poi2sendedpacket = (MD_PACKET *) 0;
}
/*
void md_free_packets (void)
*/
/*
void md_free_packets (void)
*/

   void md_free_packets (void)
{
   /* function which confirms all stored MD-packets to the messenger in case of a communication breakdown */
   /* lock sending */
   send_enable = FALSE;
   /* flush high priority queue */
   while (md_send_high_sz != md_send_high_lz)
   {
      /* sign packet FLUSHED */
      md_send_high_list[md_send_high_lz]->status = MD_FLUSHED;
      /* Packetzaehlerueberwachung dekrementieren */
      /* Zaehler fuer die max. Zahl der Packete im MAC ueberwachen */
      md_packet_count--;
      /* CONFIRM-function installed ? */
      if ( lm_send_confirm != (void (*) (MD_PACKET *)) 0 )
      {
         pi_disable();
         lm_send_confirm( (MD_PACKET *) md_send_high_list[md_send_high_lz]);
         pi_enable();
      }
      /* next element */
      md_send_high_lz = (md_send_high_lz + 1) % MD_MAX_PACKET;
   }
   /* flush low priority queue */
   while (md_send_low_sz != md_send_low_lz)
   {
      /* sign packet FLUSHED */
      md_send_low_list[md_send_low_lz]->status = MD_FLUSHED;
      /* Packetzaehlerueberwachung dekrementieren */
      /* Zaehler fuer die max. Zahl der Packete im MAC ueberwachen */
      md_packet_count--;
      /* CONFIRM-function installed ? */
      if ( lm_send_confirm != (void (*) (MD_PACKET *)) 0 )
      {
         pi_disable();
         lm_send_confirm( (MD_PACKET *) md_send_low_list[md_send_low_lz]);
         pi_enable();
      }
      /* next element */
      md_send_low_lz = (md_send_low_lz + 1) % MD_MAX_PACKET;
   }
   /* release send-flag */
   send_enable = TRUE;
}
/*
void md_status_indicate(unsigned short result)
*/
/*
void md_status_indicate(unsigned short result)
*/

   void md_status_indicate(unsigned short result)
{
   msg2mmi.p1 = result ;
   zas_send(WSNR_MMI, &msg2mmi) ;
}

