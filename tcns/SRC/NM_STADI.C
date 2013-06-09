/************************************************************************/
/*  Asea Brown Boveri Research Center, CH-5405 Baden, Switzerland       */
/*  Copyright 1993                                                      */
/*                                                                      */
/*  NAME:                                                               */
/*    NM_STADI.C                                                        */
/*    NM = Network interface for Messages (Routing/Mapping)             */
/*    STADI = Station Directory, station is an alias for device.        */
/*                                                                      */
/*  ABSTRACT:                                                           */
/*    TCN Message Transport Protocol, part of Network layer.            */
/*    This station directory is implemented as a table.                 */
/*    Use '#define SIMPLE' for simple routing version.                  */
/*                                                                      */
/************************************************************************/
/*                                                                      */
/*  AUTHOR  :   07-Sep-1993     CRBC1   Hoepli                          */
/*  MODIFIED:   05-Jan-1994     CRBC1   Hoepli    char -> unsigned char */
/*              07-Mar-1994     CRBC1   Hoepli    define DU_MODULE_NAME */
/*  MODIFIED:   15-Nov-1995     BATL    Flum      changed AM_ST_ENTRY   */
/*                                                                      */
/************************************************************************/

#define DU_MODULE_NAME "NM_STAD"
/* for hamster calls which are now mapped to JDP event recorder.
   DU_MODULE_NAME must be defined before "du_sys.h" is included.
*/
#include "stdio.h"
#include "nm_stadi.h"
#include "am_sys.h"
#include "pi_sys.h"
#include "md_ifce.h"

/* DEFINES: ----------------------------------------------------------- */

/*
#define SIMPLE
compiles version for simple routing, final_station = next_station = link_address
*/
#define STATION_IS_PHYA
/* compiles version for link_address = next_station (!= final station)
*/


#define STADI_TABLE_SIZE   256
#define STADI_LAST_IX      (STADI_TABLE_SIZE - 1)

/* TYPES: ------------------------------------------------------------- */

#ifdef STATION_IS_PHYA
typedef struct
        {
            unsigned char next_station;
            unsigned char bus_id;
        } AM_ST_NEXT;
#endif

/* VARIABLES: --------------------------------------------------------- */

#ifdef SIMPLE
#elif defined (STATION_IS_PHYA)
static AM_ST_NEXT  stadi[STADI_TABLE_SIZE];
#else
static AM_ST_ENTRY stadi[STADI_TABLE_SIZE];
#endif
static NM_ST_CHECK st_check_fct;
static short       stadi_mutex; /* semaphore */

/* FUNCTIONS: --------------------------------------------------------- */

AM_RESULT _am_st_write (AM_ST_ENTRY entries[],
		       unsigned int nr_of_entries)
/* Do not lock the mutex semaphore during the checks to prevent deadlocks! */
{
    AM_ST_ENTRY *next;
    AM_RESULT    error;
#ifndef SIMPLE
    short        pi_result;
#endif

    error = AM_OK;
    for (next =(AM_ST_ENTRY*) &entries[--nr_of_entries]; next >= entries; next--)
    {
        if (st_check_fct (next) == AM_OK)
        {
#ifdef SIMPLE
            if ((next->final_station != next->next_station) ||
                (next->final_station != next->link_address))
            {
                error = AM_FAILURE;
            }
#elif defined (STATION_IS_PHYA)
            if (next->next_station == next->link_address)
            {
                pi_pend_semaphore (stadi_mutex, PI_FOREVER, &pi_result);
                stadi[next->final_station].next_station = next->next_station;
                stadi[next->final_station].bus_id = next->bus_id;
                pi_post_semaphore (stadi_mutex, &pi_result);
            }
            else
            {
                error = AM_FAILURE;
            }
#else
            pi_pend_semaphore (stadi_mutex, PI_FOREVER, &pi_result);
            stadi[next->final_station] = *next;
            pi_post_semaphore (stadi_mutex, &pi_result);
#endif
        }
        else
        {
            error = AM_FAILURE;
        }
    }
    return error;
}

/************************************************************************/

void _am_st_read (AM_ST_ENTRY entries[],
                 unsigned int nr_of_entries)
{
    AM_ST_ENTRY *next;
    short        pi_result;

    pi_pend_semaphore (stadi_mutex, PI_FOREVER, &pi_result);
    for (next = &entries[--nr_of_entries]; next >= entries; next--)
    {
#ifdef SIMPLE
        next->next_station = next->final_station;
        next->link_address = next->final_station;
        next->bus_id = AM_MY_VB_NR;
#elif defined (STATION_IS_PHYA)
        next->link_address =
        next->next_station = stadi[next->final_station].next_station;
        next->bus_id  = stadi[next->final_station].bus_id;
#else
        *next = stadi[next->final_station];
#endif
    }
    pi_post_semaphore (stadi_mutex, &pi_result);
}

/************************************************************************/

AM_RESULT nm_st_connect_check (NM_ST_CHECK check_fct)
{
    if (st_check_fct == NULL)
    {
        st_check_fct = check_fct;
        return AM_OK;
    }
    else
    {
        return AM_FAILURE;
    }
}

/************************************************************************/

unsigned char nm_st_find_phya (LINK_ADDRESS link_address, unsigned char bus_nr)
{
    unsigned char next_station = AM_UNKNOWN_DEVICE;

#if (!defined (SIMPLE) && !defined (STATION_IS_PHYA))
    AM_ST_ENTRY  *next;
    short         pi_result;
#endif

#if (defined (SIMPLE) || defined (STATION_IS_PHYA))
    bus_nr = bus_nr; /* suppress warning 'parameter not used' */
    next_station = link_address;
#else
    pi_pend_semaphore (stadi_mutex, PI_FOREVER, &pi_result);
    for (next = stadi; next < &stadi[STADI_TABLE_SIZE]; next++)
    {
        if ((next->link_address == link_address) && (next->bus_id == bus_nr))
        {
            next_station = next->next_station;
            break;
        }
    }
    pi_post_semaphore (stadi_mutex, &pi_result);
#endif
    return next_station;
}

/************************************************************************/

unsigned char nm_st_get_bus_nr (unsigned char *station /* inout */)
{
    unsigned char bus_nr;

#ifndef SIMPLE
    unsigned char next_station;
#endif

#ifdef SIMPLE
    *station = *station; /* suppress warning 'parameter not used' */
    bus_nr = AM_MY_VB_NR;
#else
    next_station = stadi[*station].next_station;
    if (next_station != AM_UNKNOWN_DEVICE)
    {
        *station = next_station;
    }
    bus_nr = stadi[*station].bus_id;
#endif
    return bus_nr;
}

/************************************************************************/

unsigned char nm_st_get_next (unsigned char station)
{
#ifdef SIMPLE
    return station;
#else
    return stadi[station].next_station;
#endif
}

/************************************************************************/

void nm_st_system_init (void)
{
#ifdef SIMPLE
#elif defined (STATION_IS_PHYA)
    AM_ST_NEXT *entry;
#else
    AM_ST_ENTRY *entry;
#endif
    short       pi_result;

    st_check_fct = NULL;
#ifndef SIMPLE
    for (entry = &stadi[STADI_LAST_IX]; entry >= stadi; entry--)
    {
        entry->next_station = AM_UNKNOWN_DEVICE;
        entry->bus_id       = AM_UNKNOWN_BUS_NR;
    }
#endif
    stadi_mutex = pi_create_semaphore (1, PI_ORD_FCFS, &pi_result);
    hamster_halt (pi_result != PI_RET_OK);
}
