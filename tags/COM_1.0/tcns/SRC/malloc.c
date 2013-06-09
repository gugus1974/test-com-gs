/********************************************************************/
/* 2004/nov/26,ven 11:35                \SWISV\TCNS\SRC\malloc.c    */
/********************************************************************/
/* Memory allocation routines                                       */
/********************************************************************/
/*
    $Date: 2004/11/26 11:08:35 $          $Revision: 1.2 $
    $Author: mungiguerrav $
*/


/*==========================================================================*/
/* Includes */

#include <intrins.h>
#include "malloc.h"

/*==========================================================================*/
/* Compilation switches */
/* define FAST_ALLOC to enable the fast allocation mode (wastes memory) */
#define FAST_ALLOC


/*==========================================================================*/
/* Pragmas */
#pragma HLARGE
#pragma WL(3)


/*==========================================================================*/
/* Typedefs */

/* memory block status words */
#define USED        0xABCD          /* used block */
#define NOT_USED    0xF1F0          /* free block */

#define fail()      _trap_(2)

#ifdef FAST_ALLOC
    typedef struct mem_header           /**** Memory block header structure ****/
    {
        struct mem_header *prev;        /* ptr to the previous block header */
        struct mem_header *next;        /* ptr to the next block header */
        int                used;        /* NOT_USED if free, USED if used */
    } mem_header;
#else
    typedef struct mem_header           /**** Memory block header structure ****/
    {
        struct mem_header *next;        /* ptr to the next block header */
        struct mem_header *prev;        /* ptr to the previous block header */
        int                used;        /* NOT_USED if free, USED if used */
    } mem_header;
#endif

typedef unsigned int uint;


/*==========================================================================*/
/* Public Routines */

/*--------------------------------------------------------------------------*/
/* init_mempool

   Call it to specify the allocation pool memory

   pool -> ptr to the memory pool
   size -> size of the memory pool
*/

void init_mempool(char *pool, uint size)
{
    mem_header  *fp = (mem_header *)pool;
    mem_header  *lp = (mem_header *)(pool + size - sizeof(*lp));

    /* initialize the first memory block */
    fp->prev = lp;
    fp->next = lp;
    fp->used = NOT_USED;

    /* initialize the last memory block */
    lp->prev = fp;
    lp->next = fp;
    lp->used = USED;
}


/*--------------------------------------------------------------------------*/
/* malloc

   Allocates a block of memory

   pool      -> pool of memory where to allocate the data
   size      -> size of the block
   malloc() <-  address of the allocated block
*/

char *malloc (char *pool, uint size)
{
    mem_header  *bp;        /* ptr to the block header */
    mem_header  *np;        /* ptr to the new block header */
    mem_header  *xp;        /* ptr to the next block header */
    mem_header  *yp;        /* ptr to the extra block header */
    uint        block_size; /* size of the block we are examining */

    if (!pool) return 0;

    /* round the size to the int size */
    size += (sizeof(int) - (size % sizeof(int))) % sizeof(int);

    /* load the ptr to the first block */
    bp = (mem_header*)pool;

    /* loop searching for a free block */
#ifdef FAST_ALLOC
    for (xp = bp->prev, bp = xp->prev; xp != (mem_header*)pool; xp = bp, bp = bp->prev)
    {
#else
    for (; (xp = bp->next) != (mem_header*)pool; bp = xp)
    {
#endif

        if (bp->used == NOT_USED)
        {
            /* calculate the current block size */
            block_size = (char*)xp - (char*)bp - sizeof(*bp);

            /* check for enough space */
            if (block_size > size)
            {
                /* check for the space for another block header */
                if (block_size < size + sizeof(*bp))
                {
                    /* no space, just return the found block */
                    bp->used = USED;
                    return (char *)(bp+1);
                }

                /* calculate the pointer to the new block header */
                np = (mem_header*) ((char *)(bp+1) + size);

                /* set the block headers */
                bp->next = np;
                bp->used = USED;
                np->prev = bp;
                np->used = NOT_USED;

                /* remove xp from the chain if it is free */
                if (xp->used == NOT_USED)
                {
                    yp = xp->next;
                    np->next = yp;
                    yp->prev = np;
                }
                else if (xp->used == USED)
                {
                    np->next = xp;
                    xp->prev = np;
                }
                else
                {
                    fail();
                }

                return (char *)(bp+1);
            }
        }
        else if (bp->used != USED)
        {
            fail();
        }
    }

    return 0;
}


/*--------------------------------------------------------------------------*/
/* free

   Deallocates a block of memory

   p -> pointer the block
*/

void free(char *p)
{
    mem_header *bp;     /* ptr to the block header */
    mem_header *xp;     /* ptr to the extra block header */

    /* return if trying to deallocate a null ptr */
    if (!p)
    {
        return;
    }

    /* find the ptr to the block header and mark the block as unused */
    bp = ((mem_header *)p) - 1;
    if (bp->used != USED)
    {
        fail();
    }

    bp->used = NOT_USED;

    /* merge with the next block if it is unused */
    xp = bp->next;
    if (xp->used == NOT_USED)
    {
        bp->next = xp->next;
        xp->next->prev = bp;
    }
    else if (xp->used != USED)
    {
        fail();
    }

    /* merge with the previous block if it is unused */
    xp = bp->prev;
    if (xp->used == NOT_USED)
    {
        xp->next = bp->next;
        bp->next->prev = xp;
    }
    else if (xp->used != USED)
    {
        fail();
    }
}


/*--------------------------------------------------------------------------*/
/* mempool_use

   Returns the pool usage in bytes

   pool             -> pool of memory where to collect the statistic
   mempool_usage() <-  number of bytes used in the specified pool
*/

unsigned short mempool_use(char *pool)
{
    mem_header  *bp;            /* ptr to the block header */
    uint        all_size = 0;   /* allocated size */

    if (!pool)
    {
        return 0;
    }

    /* load the start of the block list */
    bp = (mem_header*)pool;

    /* loop searching for a free block */
    while (bp->next != (mem_header*)pool)
    {
        if      (bp->used == USED)
        {
            all_size += (char*)bp->next - (char*)bp;
        }
        else if (bp->used == NOT_USED)
        {
            all_size += sizeof(*bp);
        }
        else
        {
            fail();
        }

        bp = bp->next;
    }
    all_size += sizeof(*bp);

    return all_size;
}


/* ****************************************************************************
    $Log: malloc.c,v $
    Revision 1.2  2004/11/26 11:08:35  mungiguerrav
    2004/nov/26,ven 11:35           Napoli      Mungi
    - Si aggiungono keyword CVS e storia invertita
    - Si sostituiscono TAB con 4 spazi
    - Si applicano le regole di codifica sulle parentesi


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision            1.1         2004/11/26 09:45:54     mungiguerrav
    2004/nov/26,ven 10:48           Napoli      Mungi
    - Si aggiungono immodificati dalle directory inferiori TCNS\A\XXX\SRC,
      prima di iniziare il processo di ripulitura e adeguamento alle regole
      di codifica e alle regole di gestione CVS

 --------------  Spostamento da A\XXX\SRC a SRC  ---------------------

    Revision            1.2         2004/09/02 11:31:18     mungiguerrav
    2004/set/02,gio 13:25   Napoli          Mungi\Busco
    - Merge da branch di sviluppo  makefile
    - E' in effetti la creazione dei file nelle nuove directory dedicate alle
      singole schede registrate (DEA,DEV,STA,STB,STR)
    - Non sono ancora inseriti i token di gestioni automatiche CVS (DATE, FILE, AUTHOR, LOG)

    Revision [makefile] 1.1.2.1     2004/07/09 16:24:25     buscob
    - Inserimento in CVS in branch makefile
    - Addio monolite !!
01.04   1997/apr/22,mar             Napoli              Carnevale
        - Support for 32-bit processors

 ------------------------   Prima di CVS    ------------------------
01.03   199X/mmm/gg                 Napoli              Carnevale
        - Added the fast allocation mode enabled with the FAST_ALLOC switch
01.02   199X/mmm/gg                 Napoli              Carnevale
        - Minor optimizations
01.01   199X/mmm/gg                 Napoli              Carnevale
        - Corrected a bug in malloc
01.00   1996/mmm/gg                 Napoli              Carnevale
        - Creazione
*/

