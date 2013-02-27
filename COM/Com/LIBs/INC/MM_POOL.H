#ifndef MM_POOL_H
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
*    MODULE       : MM_POOL.H                                               *
*                                                                           *
*    DOCUMENT     :                                                         *
*                                                                           *
*    ABSTRACT     : Administartion of a pool of buffer.                     *
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
*    0.2      92-03-09  E. Hoepli    CRBC-1         mm_pool_exchange        *
*                                                                           *
*****************************************************************************
*/

#define MM_POOL_H

/*
*****************************************
* Includes                              *
*****************************************
*/

#include "md_layer.h"

/*
*****************************************
* Procedures                            *
*****************************************
*/

#define mm_pool_put_pack( pack )\
{  pi_disable();\
   mm_pool_put_pack_from_ISR( pack );\
   pi_enable();}

#define mm_pool_get_pack( pool, pack )\
{  pi_disable();\
   mm_pool_get_pack_from_ISR( pool, pack );\
   pi_enable();}

void mm_pool_create( int, int, int, void * * ) ;
void mm_pool_get_pack_from_ISR( void * *, MD_PACKET * * ) ;
void mm_pool_put_pack_from_ISR( MD_PACKET * ) ;
void mm_pool_exchange( void * *, MD_PACKET * * ) ;

#endif
