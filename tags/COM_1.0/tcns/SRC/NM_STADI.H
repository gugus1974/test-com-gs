#ifndef	NM_STADI_H
#define	NM_STADI_H

/************************************************************************/
/*  Asea Brown Boveri Research Center, CH-5405 Baden, Switzerland       */
/*  Copyright 1993                                                      */
/*                                                                      */
/*  NAME:                                                               */
/*    NM_STADI.H                                                        */
/*    NM = Network interface for Messages (Routing/Mapping)             */
/*    STADI = Station Directory, station is an alias for device.        */
/*                                                                      */
/*  ABSTRACT:                                                           */
/*    TCN Message Transport Protocol, part of Network layer.            */
/*    The station directory is necessary if there is more than 1 VB     */
/*    in a vehicle, or if the physical address is not identical with    */
/*    the station address. A station address is unique within a         */
/*    vehicle. The STADI is used by the link layer to map physical      */
/*    addresses to station addresses, and by the network layer to map   */
/*    a station to its bus number and to the next_station which is the  */
/*    one station to be used as next hop.                               */
/*    This is not the complete interface to the STADI. Some functions   */
/*    and definitions are accessible from applications and are put into */
/*    a separate header module AM_STADI.H which then is included here.  */
/*    The function directory in module AM_DIREC.C maps functions to     */
/*    next_stations.                                                    */                                    
/*                                                                      */
/************************************************************************/
/*                                                                      */
/*  AUTHOR  :   23-Aug-1993     CRBC1   Hoepli                          */
/*  MODIFIED:   15-Nov-1993     CRBC1   Hoepli  char -> unsigned char   */
/*  MODIFIED:   15-Nov-1995     BATL    Flum    AM_ST_PHYA ->           */
/*                                              LINK_ADDRESS            */
/*                                                                      */
/************************************************************************/

#include "am_sys.h"

/* DEFINES: ----------------------------------------------------------- */

#define NM_ST_TB_STATION   255    
/* This entry in the STADI is used to save the physical address for the
   router to the TB if the router station is not defined in the STADI.
*/                                             

/* TYPES: ------------------------------------------------------------- */

typedef AM_RESULT (*NM_ST_CHECK) ( AM_ST_ENTRY *);
/* function type to check one new STADI entry. */

/* FUNCTIONS: --------------------------------------------------------- */

AM_RESULT nm_st_connect_check (NM_ST_CHECK check_fct);
/* Connects a check function which will be called before a new entry
   is written into the STADI. The entry is discarded if the check fails.
   Returns AM_OK on success, or AM_FAILURE if the check function cannot
   be connected.
*/

unsigned char nm_st_find_phya (LINK_ADDRESS link_address, unsigned char bus_nr);
/* Finds a link address in the STADI and returns the attached
   next_station or AM_UNKNOWN_DEVICE as result.
*/

unsigned char nm_st_get_bus_nr (unsigned char *station /* inout */);
/* Returns as result the bus number which must be used to reach the
   specified station or AM_UNKNOWN_BUS_NR.
   If next_station is known, then next_station is returned as output
   value for station.
*/

unsigned char nm_st_get_next (unsigned char station);
/* Returns next_station for station (next hop to reach station). 
   The result may be AM_UNKNOWN_DEVICE.
*/

void nm_st_system_init (void);
/* Must be called once before any other STADI function. */

#endif
