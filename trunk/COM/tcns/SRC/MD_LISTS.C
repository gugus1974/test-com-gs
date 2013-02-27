/************************************************************************/
/*  Asea Brown Boveri Research Center, CH-5405 Baden, Switzerland       */
/*  Copyright 1991                                                      */
/*                                                                      */
/*  NAME:                                                               */
/*    MD_LISTS.C                                                        */
/*                                                                      */
/*  ABSTRACT:                                                           */
/*    An element with 'next' = NULL is not connected in any list.       */
/*    All lists are FIFO order. The elements are inserted at the list   */
/*    end and taken at the begining.                                    */
/*    The descriptor element is the root which points itself when the   */
/*    queue is empty. The root is an unremovable element of the queue.  */
/*                                                                      */
/*  DEPENDENCIES:                                                       */
/*    All functions on the queues are independently called from         */
/*    different tasks. Therefore the care must be taken if loading,     */
/*    unloading and removing elements. As the protection mechanism      */
/*    lock( ) and unlock( ) functions are used.                         */
/*                                                                      */
/************************************************************************/
/*                                                                      */
/*  AUTHOR  :   13-Feb-1991     CRBC1   Hoepli                          */
/*  APPROVED:   <dd-mmm-yy>  <dep.>   <name>                            */
/*  MODIFIED:   <dd-mmm-yy>  <dep.>   <name>                            */
/*              <reason>                                                */
/*                                                                      */
/*  MODIFIED:   1991.08.22   BAEI-3   Krzeslowski ( WHK )               */
/*              translated to C                                         */
/*              The old Modula 2 names are left as comments at the very */
/*              end of every function.                                  */
/*              BOOLEAN changed to 'int'.                               */
/*              CARDINAL changed to 'size_t'.                           */
/*              The list is created statically. No elements are         */
/*              returned to memory manager!!                            */
/*                                                                      */
/*  MODIFIED:   1991.10.07   BAEI-3   Krzeslowski ( WHK )               */
/*              Module name changed to MD_LISTS.C ( previous name       */
/*              LISTS.C )                                               */
/*                                                                      */
/*  MODIFIED:   1991.10.11   BAEI-3   Krzeslowski ( WHK )               */
/*              corrections to the list has been made.                  */
/*                                                                      */
/*  MODIFIED:   1991.10.17   BAEI-3   Krzeslowski ( WHK )               */
/*              lock & unlock functions have been implemented.          */
/*  MODIFIED:   1995.02.13   BATL     Flum                              */
/*              inserted Switch O_SUN                                   */
/*                                                                      */
/*  MODIFIED:   1995.02.15   BATL     Flum                              */
/*              Switch O_MD_STDA to compile source without C standard   */
/*              Lib                                                     */
/************************************************************************/


#if defined (O_MD_STDA)
#define NULL       0
typedef unsigned size_t;
#else
#include <stddef.h>		/* size_t */
#include <stdio.h>		/* NULL */
#endif

#if defined (O_PC) || defined (O_SUN)
#include "stdlib.h"
#endif

#include "pi_sys.h"
#include "am_sys.h"
#include "basetype.h"
#include "md_layer.h"

/* ------------------------------------------------------------------------- */

void md_init_ele( MD_LIST_HEADER * ele )
{
  if( ele != NULL ) {
    ele->next = NULL;
  }
} /* init_ele( ) */

/* ------------------------------------------------------------------------- */

void md_put_ele( MD_LIST_HEADER * root, MD_LIST_HEADER * ele )
{
  if( ele == NULL ) {
    return;
  }

  if( ele->next != NULL ) {
    return;
  }

  pi_lock_task( );

  ele->next = root;
  ele->prev = root->prev;
  root->prev->next = ele;
  root->prev = ele;

  pi_unlock_task( );
} /* put_ele( ) */

/* ------------------------------------------------------------------------- */
  
void md_get_ele( MD_LIST_HEADER * root, MD_LIST_HEADER * * ele )
{
  if( root->next == root ) {
    *ele = NULL;
  } 
  else {
    pi_lock_task( );

    *ele = root->next;
    root->next = (*ele)->next;
    (*ele)->next->prev = root;
    (*ele)->next = NULL;

    pi_unlock_task( );
  } /* if */
} /* get_ele( ) */
  
/* ------------------------------------------------------------------------- */

void md_cancel_ele( MD_LIST_HEADER * ele )
{
  if( ele == NULL ) {
    return;
  }

  if( ele->next == NULL ) {
    return;
  }

  pi_lock_task( );

  ele->next->prev = ele->prev;
  ele->prev->next = ele->next;
  ele->next = NULL;

  pi_unlock_task( );
} /* cancel_ele( ) */

/* ------------------------------------------------------------------------- */

void md_init_list( MD_LIST_HEADER * * root)
{
/* root is a pointer to the head and to the tail of the created queue
*/
  *root = (MD_LIST_HEADER *)pi_alloc( sizeof( MD_LIST_HEADER ) );
  (*root)->next = *root;
  (*root)->prev = *root;

} /* init_list( ) */

/* ------------------------------------------------------------------------- */
