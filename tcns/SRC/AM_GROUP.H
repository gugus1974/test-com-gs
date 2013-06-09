#ifndef	AM_GROUP_H
#define	AM_GROUP_H

/************************************************************************/
/*  Asea Brown Boveri Research Center, CH-5405 Baden, Switzerland       */
/*  Copyright 1993                                                      */
/*                                                                      */
/*  NAME:                                                               */
/*    AM_GROUP.H                                                        */
/*    Groups of vehicles, membership of the own vehicle.                */
/*    Interface to application and messenger.                           */
/*                                                                      */
/*  ABSTRACT:                                                           */
/*    This module supplies all information about vehicle groups that    */
/*    the TCN messenger needs to know for routing of multicast packets. */
/*    The messenger does not initialize the group module, he just calls */
/*    function 'am_is_member_of' on a VB/TB gateway or a TB end-system. */
/*    The 2 bits 'AM_PHYS_ADDR' and 'AM_GROUP_BIT' are discarded in all */
/*    parameters that specify a group.                                  */
/*                                                                      */
/************************************************************************/
/*                                                                      */
/*  AUTHOR  :   01-Sep-1993     CRBC1   Hoepli                          */
/*  MODIFIED:   25-Nov-1993     CRBC1   Hoepli   char -> unsigned char  */
/*                                                                      */
/************************************************************************/

#define NR_OF_GROUPS    64

/* FUNCTIONS ---------------------------------------------------------- */

unsigned char am_is_member_of (unsigned char vehicle_group);
/* Checks whether the own vehicle is member of the specified vehicle group.
   Returns a boolean result.
*/

void am_join_groups (unsigned char group_list[], 
                     unsigned short nr_of_entries);
/* Inserts the own vehicle as member of all groups in array 'group_list',
   'nr_of_entries' specifies the number of elements in 'group_list'.
*/

void am_leave_groups (unsigned char group_list[], 
                      unsigned short nr_of_entries);
/* Removes the own vehicle from all groups in array 'group_list',
   'nr_of_entries' specifies the number of elements in 'group_list'.
*/

void am_leave_all_groups (void);

#endif
