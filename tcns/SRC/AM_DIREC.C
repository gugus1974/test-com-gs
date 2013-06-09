/************************************************************************/
/*  Asea Brown Boveri Research Center, CH-5405 Baden, Switzerland       */
/*  Copyright 1991                                                      */
/*                                                                      */
/*  NAME:                                                               */
/*    AM_DIREC.C                                                        */
/*                                                                      */
/*  ABSTRACT:                                                           */
/*    TCN Message Transport Protocol, MVB directory table.              */
/*                                                                      */
/*  DEPENDENCIES:                                                       */
/*    <text>                                                            */
/*                                                                      */
/************************************************************************/
/*                                                                      */
/*  AUTHOR  :   30-Apr-1991     CRBC1   Hoepli                          */
/*  APPROVED:   <dd-mmm-yy>  <dep.>   <name>                            */
/*  MODIFIED:   <dd-mmm-yy>  <dep.>   <name>                            */
/*              <reason>                                                */
/*  MODIFIED:   1991.08.22   BAEI-3   Krzeslowski ( WHK )               */
/*              translated to C                                         */
/*              The old Modula 2 names are left as comments at the very */
/*              end of every function.                                  */
/*              BOOLEAN changed to 'int'.                               */
/*              CARDINAL changed to 'size_t'.                           */
/*                                                                      */
/*  MODIFIED:   1991.10.08   BAEI-3   Krzeslowski ( WHK )               */
/*              Module name changed to AM_DIREC.C ( previous name       */
/*              MTPDIREC.C )                                            */
/*              18-Mar-1992  CRBC1    E. Hoepli    index type cast      */
/*              93-10-01     BATC     R. Muenger   double index type    */
/*                                                 cast for 96 compiler */
/*  MODIFIED:   1995.02.15   BATL     R.Flum       Switch O_MD_STDA to  */
/*                                                 compile the sources  */
/*                                                 without c standard   */
/*                                                 lib                  */
/************************************************************************/


#if defined (O_MD_STDA)
typedef unsigned size_t;
#define NULL    0
#else
#include <stddef.h>		/* size_t */
#include <stdio.h>		/* NULL */
#endif


#include "am_sys.h"

/* definitions ------------------------------------------------------------- */

#define	DIR_TABLE_SIZE	256

/* static variables -------------------------------------------------------- */

static unsigned char /* device */ dir_table[ DIR_TABLE_SIZE /* function */ ];

/* function bodies --------------------------------------------------------- */

void _am_insert_dir_entries( AM_DIR_ENTRY * list, unsigned nr_of_entries )
/* inserts a list of entries into the directory table( overwrite ). */
{
size_t ix;

  for( ix = 0 ; ix < nr_of_entries ; ix++ ) {
    dir_table[list[ix].function] =
      list[ix].device;
  } /* for */
} /* _am_insert_dir_entries( ) */

void _am_remove_dir_entries( AM_DIR_ENTRY * list, unsigned nr_of_entries )
/* removes a list of entries from the directory table. */
{
size_t ix;

  for(  ix = 0 ; ix < nr_of_entries ; ix++ ) {
    dir_table [list[ix].function] =
      AM_UNKNOWN_DEVICE;
  } /* for */
} /* _am_remove_dir_entries( ) */

void _am_get_dir_entry( unsigned char function, unsigned char * device )
/* returns 'AM_UNKNOWN_DEVICE' if there is no known device address for the 
   queried function. */
{
  *device = dir_table[function];
} /* _am_get_dir_entry( ) */

void _am_clear_dir ( void )
/* clears the whole directory table.
   The function has to be called in the initialisation routine
*/
{
size_t ix;

  for( ix = 0 ; ix < DIR_TABLE_SIZE ; ix++ ) { 
    dir_table[ix] = AM_UNKNOWN_DEVICE;
  } /* for */
} /* _am_clear_dir( ) */

