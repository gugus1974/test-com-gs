#ifndef	NM_LAYER_H
#define	NM_LAYER_H

/************************************************************************/
/*  Asea Brown Boveri Research Center, CH-5405 Baden, Switzerland       */
/*  Copyright 1991                                                      */
/*                                                                      */
/*  NAME:                                                               */
/*    NM_LAYER.H                                                        */
/*    NM = Network interface for Messages                               */
/*                                                                      */
/*  ABSTRACT:                                                           */
/*    TCN Message Transport Protocol, Network layer.                    */
/*    VB devices normally do not know their unique vehicle address      */
/*    and therefore they only support 'AM_SAME_VEHICLE'.                */
/*                                                                      */
/************************************************************************/
/*                                                                      */
/*  AUTHOR  :   30-Apr-1991     CRBC1   Hoepli                          */
/*  APPROVED:   <dd-mmm-yy>  <dep.>   <name>                            */
/*  MODIFIED:   <dd-mmm-yy>  <dep.>   <name>                            */
/*              <reason>                                                */
/*  MODIFIED:   1991.08.22   BAEI-3   Krzeslowski                       */
/*              translated to C                                         */
/*              19-Nov-1991  CRBC1    Hoepli                            */
/*              05-Mar-1992  CRBC1    Hoepli     new 'nm_config'        */
/*              16-Jun-1995  BATL     Flum       new param at "nm_init" */
/*              11-Okt-1995  BATL     Flum    nm_change_packet_lifetime */
/*                                                                      */
/************************************************************************/

#include "md_layer.h"

/* TYPE --------------------------------------------------------------- */




/* FUNCTIONS ---------------------------------------------------------- */

int nm_rcv_pack( MD_PACKET ** pack /* out */,
		 unsigned int * size /* out */,
		 /* size of link data unit without size field */
		 TM_LINK_ID * src /* out */,
		 TM_LINK_ID * dest /* out */,
		 AM_RESULT * error /* out */ );
/* Receive: returns 0 if there is no packet available.
   A gateway may return an error if the packet is addressed to
   some other destination device and cannot be forwarded.
   The priority in the control field of the packet is not valid.
   A received packet must be recycled to its pool after use. */

void nm_send_pack( MD_PACKET * pack,
		   unsigned int size,
		   /* size of link data unit without size field */
		   const TM_LINK_ID * src,  /* in */
		   const TM_LINK_ID * dest, /* in */
		   AM_RESULT * error /* out */ );
/* Sends a packet to the specified destination.
   Fills in the net header and the size field. Sets the physical 
   addressing bit for the source in the net header to the same value as 
   specified in 'dest'.
   Defines the priority in the control field of the packet, high
   priority for physical addressing.
   Returns asynchronously. The packet is recycled to its pool after
   completion of transmission or immediatley in case of an error. */

void nm_init (unsigned int nr_of_pack,void (*abort_TB_conn) (void));
/* Creates at most 'nr_of_pack' free packets in the receive pool
   for the link layer (in an end-system) and calls 'lm_init'.
   'nr_of_pack' has no significance in a gateway. */

void nm_system_init (void);
/* Must be called at system initialization. Calls 'md_set_my_device',
   'md_set_my_vehicle' and 'lm_system_init'. */

#ifndef NM_1_TB
short nm_change_packet_lifetime(unsigned long);
    /* Used for networkmanagement. The procedure changes the bus supervision
       intervall in which the underlay link layers will verified */
#endif

#endif
