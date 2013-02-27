#if !(defined (NM_1VB) || defined (NM_1VB_SIMPLE) || defined (NM_GW_NO_TB))
/************************************************************************/
/*  Asea Brown Boveri Research Center, CH-5405 Baden, Switzerland       */
/*  Copyright 1993                                                      */
/*                                                                      */
/*  NAME:                                                               */
/*    AM_GROUP.C                                                        */
/*    Groups of vehicles, membership of the own vehicle.                */
/*    Interface to application and messenger.                           */
/*                                                                      */
/*  ABSTRACT: see interface module 'AM_GROUP.H'                         */
/*                                                                      */
/************************************************************************/
/*                                                                      */
/*  AUTHOR  :   06-Sep-1993     CRBC1   Hoepli                          */
/*  MODIFIED:   25-Nov-1993     CRBC1   Hoepli  char -> unsigned char   */
/*              31-Jan-1994     CRBC1   Hoepli  #ifdef for whole module */
/*                                                                      */
/************************************************************************/

#include "am_sys.h"
#include "am_group.h"


/* variables: */
static unsigned char is_member[NR_OF_GROUPS + 1];
/* index = group number, last entry is dummy for invalid group numbers */

/* local prototypes: */
static unsigned char valid_index (unsigned char group);

/*************************************************************************/

static unsigned char valid_index (unsigned char group)
{
unsigned char result = NR_OF_GROUPS;

  group &= ~(AM_PHYS_ADDR | AM_GROUP_BIT);
  if (group < NR_OF_GROUPS) {
    result = group;
  }
  return result;
} /* valid_index */

/*************************************************************************/

void am_join_groups (unsigned char group_list[], 
                     unsigned short nr_of_entries)
{
unsigned char ix;

  for (; nr_of_entries > 0; nr_of_entries--) {
    ix = valid_index (*group_list++);
    is_member[ix] = !0;
  }
} /* am_join_groups */

/*************************************************************************/

void am_leave_groups (unsigned char group_list[], 
                      unsigned short nr_of_entries)
{
unsigned char ix;

  for (; nr_of_entries > 0; nr_of_entries--) {
    ix = valid_index (*group_list++);
    is_member[ix] = 0;
  }
} /* am_leave_groups */

/*************************************************************************/

void am_leave_all_groups (void)
{
short ix;
  for (ix = 0; ix < NR_OF_GROUPS; ix++) {
    is_member[ix] = 0;
  }
} /* am_leave_all_groups */

/*************************************************************************/

unsigned char am_is_member_of (unsigned char vehicle_group)
{
unsigned char ix;

  ix = valid_index (vehicle_group);
  if (ix < NR_OF_GROUPS) {
    return is_member[ix];
  } else {
    return 0;
  }
} /* nm_is_member_of */
#endif
