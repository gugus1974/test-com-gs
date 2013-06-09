/*
*****************************************************************************
*                                                                           *
*    Property of  :   ABB Verkehrssysteme AG, CH-8050 Z*rich / Switzerland  *
*    COPYRIGHT    :   (c) 1991 ABB Verkehrssysteme AG                       *
*                                                                           *
*****************************************************************************
*                                                                           *
*    PROJECT      : Micas Operating System                                  *
*    SUBPROJECT   : Memory Manager, Pool administration                     *
*                                                                           *
*****************************************************************************
*                                                                           *
*    MODULE       : MM_POOL.c                                               *
*                                                                           *
*    DOCUMENT     :                                                         *
*                                                                           *
*    ABSTRACT     : Administration of a pool of buffer.                     *
*                                                                           *
*    REMARKS      :                                                         *
*                                                                           *
*    DEPENDENCIES :                                                         *
*                                                                           *
*    ACCEPTED     :                                                         *
*                                                                           *
*****************************************************************************
*                                                                           *
*    HISTORY:                                                               *
*                                                                           *
*    Version  YY-MM-DD  Name         Dept.    Ref   Comments                *
*    -------  --------  ----------   ------   ----  --------------------    *
*    0.1      91-10-02  E. Nantier   BAEI-2    --   created                 *
*                                                                           *
*    0.2      91-10-29  WHK          BAEI-3    --   changed                 *
*                                                                           *
*    0.3      92-03-09  E. Hoepli    CRBC-1         mm_pool_exchange        *
*    0.4      95-02-13  R. Flum      BATL           Switch O_SUN inserted   *
*    0.5      95-02-15  R. Flum      BATL           Switch O_MD_STDA to     *
*                                                   compile the MD sources  *
*                                                   without C standard lib  *
*                                                                           *
*****************************************************************************
*/

/*
*****************************************
* include files                         *
*****************************************
*/


#if defined (O_MD_STDA)
#define SHRT_MAX 0x7FFF
#define NULL     0
#else
#include <stdio.h>
#include <limits.h>
#endif

#if defined (O_PC) || defined (O_SUN)
#include <stdlib.h>
#endif

#include "pi_sys.h"
#include "am_sys.h"
#include "basetype.h"
#include "md_layer.h"
#include "mm_pool.h"

/*
*****************************************
* Procedures                            *
*****************************************
*/

void mm_pool_create( int packet_nr, int packet_size,
		     int locked, void * * pool_id )
{
int             i ;
MD_PACKET     * p ;
void          * p_id ;
unsigned long   size ;

    size = packet_nr * packet_size;
    if (size > SHRT_MAX)               /* Mr */
    {
	*pool_id = NULL;
    }
    else
    {
      *pool_id = pi_alloc (size);
      if ( *pool_id  != NULL )
      {
	if ( locked == 0 )
	{
		p_id = pool_id ;

	}
	else
	{
		p_id = NULL ;
	}


	p =(MD_PACKET*)(*pool_id);
	for( i = 1 /* should it not be 0 ? */ ; i < packet_nr ; i++ )
	{
		p->next = p + 1 ;
		p->identifier = p_id ;
		p++ ;
	}

	p->next = NULL ;
	p->identifier = p_id ;
      }
    }
}

void mm_pool_get_pack_from_ISR( void * * pool_id, MD_PACKET * * new_pack )
{

	if ( ( *new_pack = (MD_PACKET*)(*pool_id) ) != NULL )
	{
                *pool_id = (*new_pack)->next ;
        }
}

void mm_pool_put_pack_from_ISR( MD_PACKET * used_pack )
{
   if ( used_pack != NULL )
   {
      if( used_pack->identifier != NULL )
      {
         used_pack->next = *(void * *)used_pack->identifier ;
         *(void * *)(used_pack->identifier) = used_pack ;
      }
   }
}

void mm_pool_exchange( void * * next_pool_id, MD_PACKET * * full_pack )
{
MD_PACKET * ptr ;

	if( (*full_pack)->identifier == NULL ) {
/* !!! WHK 22.10.1991 - the code need not work properly */
		return;
	}
        mm_pool_get_pack( next_pool_id, &ptr ) ;
        if ( ptr == NULL )
        {
                mm_pool_put_pack( *full_pack ) ;
                *full_pack = NULL ;
        }
        else
        {
                ptr->identifier = (*full_pack)->identifier ;
                (*full_pack)->identifier = next_pool_id ;
                mm_pool_put_pack( ptr );
        }
}
