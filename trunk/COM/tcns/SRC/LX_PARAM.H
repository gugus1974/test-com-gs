#ifndef LM_PARAM_H
#define LM_PARAM_H

/************************************************************************/
/*  Asea Brown Boveri Research Center, CH-5405 Baden, Switzerland       */
/*  Copyright 1993                                                      */
/*                                                                      */
/*  NAME:                                                               */
/*    LX_PARAM.H                                                        */
/*                                                                      */
/*  ABSTRACT:                                                           */
/*    Link Layer Parametrization for TCN Message Data.                  */
/*    This module is used by the network layer to initialize all        */
/*    underlying link layers and to provide a hardware defined station  */
/*    address.                                                          */
/*                                                                      */
/************************************************************************/
/*                                                                      */
/*  AUTHOR  :   23-Aug-1993     CRBC1   Hoepli                          */
/*  APPROVED:   <dd-mmm-yy>  <dep.>   <name>                            */
/*  MODIFIED:   <dd-mmm-yy>  <dep.>   <name>                            */
/*              <reason>                                                */
/*                                                                      */
/************************************************************************/
/*                                                                      */
/*  CHANGES:                                                            */
/*                                                                      */
/*  1.      95-02-28  BATL  zur Bonsen  Renamed to LX_PARAM.H           */
/*                                                                      */
/*                                                                      */
/************************************************************************/

void lm_system_init (void);
/* Initializes all underlying link layers and connects them to the
   bus dispatcher.
*/

void lm_get_own_station (unsigned short *own_station);
/* Returns the own, hardware defined station address. Does not inspect
   the STADI, but only reads the hardware configuration if any.
   The following special values are possible:
   - AM_SAME_DEVICE means that the own station address may be defined as
     part of the STADI. If it is not, then any station address can be
     used to reach the station with physical addressing in case there is
     a VB connected. The own station address can be defined by writing into
     the STADI. If there is no VB, then only station AM_SAME_DEVICE
     is accepted.  
   - >= AM_UNKNOWN_DEVICE means that physical addressing is not possible,
     all physically addressed packets are discarded (neither consumed
     nor forwarded). Additionally STADI entries which would define the
     own station address are deleted, and such entries cannot be written
     into the STADI because they do not pass the checks. This value allows
     to clear the own station address in systems where it is defined
     by the STADI (and not by hardware). In such systems the station
     address must be known to communicate, but it cannot be read without
     communication! Thus a device is not accessible if its station address
     is forgotten. In this case AM_UNKNOWN_DEVICE allows to delete the
     old station address, and a restart with AM_SAME_DEVICE allows access
     using any station address.
   Any other value defines a constant own station address which neither
   changes nor allows conflicts with STADI entries.
*/

#endif
