#ifndef	NM_MANAG_H
#define	NM_MANAG_H

/************************************************************************/
/*  Asea Brown Boveri Research Center, CH-5405 Baden, Switzerland       */
/*  Copyright 1991                                                      */
/*                                                                      */
/*  NAME:                                                               */
/*    NM_MANAG.H                                                        */
/*    NM = Network interface for Messages                               */
/*                                                                      */
/*  ABSTRACT:                                                           */
/*    TCN Message Transport Protocol, Network layer management.         */
/*    This module is used in the VB/TB gateway or a TB end-system only. */
/*    It contains the train configuration part of the network layer.    */
/*                                                                      */
/************************************************************************/
/*                                                                      */
/*  AUTHOR  :   11-Mar-1992     CRBC1   Hoepli                          */
/*  MODIFIED:   01-Sep-1993     CRBC1   Hoepli   nm_vehicle_exists chg. */
/*                                                                      */
/************************************************************************/

#include "am_sys.h"

/* FUNCTIONS ---------------------------------------------------------- */

void nm_set_vehicle (unsigned char vehicle);
/* Defines my constant vehicle address, must be called before
   'am_announce_device'.
   !!! This function will not exist anymore as soon as dynamic train
   configuration management is implemented.
*/

void nm_status_ind ( int status);
/* Treats status indications from the train bus link layer. */

AM_RESULT nm_vehicle_exists (unsigned char *vehicle);
/* Checks if the specified vehicle is available on the train bus.
   Returns AM_INAUG_ERR during inauguration, AM_UNKNOWN_DEST_ERR
   if this vehicle is not connected or AM_OK on success.
   Changes vehicle to AM_ALL_VEH in case it is a vehicle group.
*/

void nm_mng_system_init (void);
/* Must be called at system initialization. */

#endif
