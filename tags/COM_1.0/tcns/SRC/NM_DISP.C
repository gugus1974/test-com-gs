/************************************************************************/
/*  Asea Brown Boveri Research Center, CH-5405 Baden, Switzerland       */
/*  Copyright 1993                                                      */
/*                                                                      */
/*  NAME:                                                               */
/*    NM_DISP.C                                                         */
/*                                                                      */
/*  ABSTRACT:                                                           */
/*    TCN Message Transport Protocol, bus dispatcher for Network Layer. */
/*    Maintains an indirect jump table for the link layer functions.    */
/*                                                                      */
/************************************************************************/
/*                                                                      */
/*  AUTHOR  :   13-Oct-1993     CRBC1   Hoepli                          */
/*  MODIFIED:   25-Nov-1993     CRBC1   Hoepli   char -> unsigned char  */
/*                                                                      */
/************************************************************************/

#include "basetype.h"
#include "md_layer.h"
#include "nm_disp.h"

/* VARIABLES: --------------------------------------------------------- */

NM_LINK_LAYER_FCTS    nm_link_fcts[AM_MAX_BUSSES];   /* index is bus_nr */                                                   
static unsigned char  is_connected[AM_MAX_BUSSES];      /* boolean */

/* FUNCTIONS --------------------------------------------------------- */

void _am_disp_show_busses (unsigned char *nr_of_busses,
			  unsigned char bus_nr[AM_MAX_BUSSES])
{
unsigned char ix;

 *nr_of_busses = 0;
 for (ix = 0; ix < AM_MAX_BUSSES; ix++) {
   if (is_connected[ix] && (ix != AM_TB_NR)) {
     bus_nr[*nr_of_busses] = ix;
     (*nr_of_busses)++;
   } /* if */
 } /* for */
 if (is_connected[AM_TB_NR]) {
     bus_nr[*nr_of_busses] = AM_TB_NR;
     (*nr_of_busses)++;
 }
} /* am_disp_show_busses */

/**********************************************************************/

unsigned char nm_disp_connect_bus (unsigned char bus_nr, 
                                   const NM_LINK_LAYER_FCTS *bus_fcts)
{
  if ((bus_nr < AM_MAX_BUSSES) && !is_connected[bus_nr]) {
    nm_link_fcts[bus_nr] = *bus_fcts;
    is_connected[bus_nr] = !0;
    return 0;
  } else {
    return !0;
  }
} /* nm_disp_connect_bus */

/**********************************************************************/

void nm_disp_system_init (void)
{
unsigned char bus_nr;

  for (bus_nr = 0; bus_nr < AM_MAX_BUSSES; bus_nr++) {
    is_connected[bus_nr] = 0;
  }
} /* nm_disp_system_init */
