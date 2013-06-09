#ifndef	NM_DISP_H
#define	NM_DISP_H

/************************************************************************/
/*  Asea Brown Boveri Research Center, CH-5405 Baden, Switzerland       */
/*  Copyright 1993                                                      */
/*                                                                      */
/*  NAME:                                                               */
/*    NM_DISP.H                                                         */
/*    NM = Network interface for Messages                               */
/*    DISP = Dispatcher for available link layers (busses).             */
/*                                                                      */
/*  ABSTRACT:                                                           */
/*    TCN Message Transport Protocol, part of Network layer.            */
/*    This module decouples the network layer from the underlying link  */
/*    layers. The link layers connect themselves to the dispatcher, and */
/*    the network layer calls the link layer functions indirectly       */
/*    through the dispatcher.                                           */
/*    Some part of the dispatcher interface is also used by applica-    */
/*    tions and therefore declared in the header file AM_STADI.H which  */
/*    is included here.                                                 */
/*                                                                      */
/************************************************************************/
/*                                                                      */
/*  AUTHOR  :   12-Oct-1993     CRBC1   Hoepli                          */
/*  MODIFIED:   25-Nov-1993     CRBC1   Hoepli  char -> unsigned char   */
/*  MODIFIED:   02-Ma -1995     BATL    Flum    additional parameter in */
/*                                              the jump table functions*/
/*                                                                      */
/*  MODIFIED:   15-Sep-1995     BATL    Flum    additional parameters to*/
/*                                              funcitons               */
/*                                              lm_m_v_get_own_phya     */
/*                                              and lm_m_v_get_src_phya */
/*  MODIFIED:   15-Sep-1995     BATL    Flum    T_ID removed            */
/************************************************************************/

#include "md_layer.h"
#include "am_sys.h"

/* DEFINES: ----------------------------------------------------------- */

#define NM_ST_TB_STATION   255    
/* This entry in the STADI is used to save the physical address for the
   router to the TB if the router station is not defined in the STADI.
*/                                             



/* TYPES: ------------------------------------------------------------- */

typedef struct {
void (*lm_init)(unsigned short dummy, void (* receive_confirm)(void),
			void (* get_pack)(void **, MD_PACKET ** ),
			void ** ident,
			void (* put_pack)( MD_PACKET * ),
			void (* status_indicate)(int ) );
void (*lm_get_status)(unsigned short, unsigned short, unsigned short, unsigned short * );
void (*lm_send_request)(unsigned short, unsigned char, unsigned char, MD_PACKET *, int *);
void (*lm_send_queue_flush)(unsigned short);
void (*lm_receive_poll)(unsigned short, unsigned char *, unsigned char *, MD_PACKET **,
			int *);
unsigned short (*lm_get_own_phya)(unsigned short, int *);
unsigned short (*lm_get_src_phya)(MD_PACKET *, int *);
} NM_LINK_LAYER_FCTS;

/* VARIABLES: --------------------------------------------------------- */

/* indirect jump table for link layer functions, execute access only! */
extern NM_LINK_LAYER_FCTS nm_link_fcts[AM_MAX_BUSSES]; /* index is bus_nr */                                                   

/* FUNCTIONS: --------------------------------------------------------- */

unsigned char nm_disp_connect_bus (unsigned char bus_nr, 
                                   const NM_LINK_LAYER_FCTS *bus_fcts);
/* Connects the link layer interface functions for a specified bus number.
   The bus number must be < AM_MAX_BUSSES.
   Returns 0 on successful connection (no overwrite).
*/

void nm_disp_system_init (void);
/* Must be called once before any other dispatcher function. */

#endif
