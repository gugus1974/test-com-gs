/****************************************************************************/
/*          Copyright ( C ) Siemens AG 1995 All Rights Reserved             */
/****************************************************************************/

#ifndef c_defs_h        /* [ */
#define c_defs_h

#include        <stddef.h>

/****************************************************************************/
/*      Common C Definitions and Declarations                               */
/****************************************************************************/

/* Ausgleich von ANSI C und K&R C */

#ifdef __STDC__ /* [[ ANSI C */

#define if_ANSI_C_( ANSI_sequence, nonANSI_sequence )   ANSI_sequence

#ifndef NULL
#define NULL    (void *)0
#endif

#define Void    void

#define Func( parlist ) parlist

#define va_init         va_start

#define Testf( parlist ) \
        {printf("++ "__FILE__" %d: ", __LINE__);printf parlist;fflush(stdout);}

#else   /* ][ K&R C */

#define if_ANSI_C_( ANSI_sequence, nonANSI_sequence )   nonANSI_sequence

#ifndef __FILE__
#define __FILE__        ""
#endif

#ifndef NULL
#define NULL    0
#endif

#define Void    unsigned char

#define const
#define signed
#define volatile
#define sig_atomic_t    int

#define Func( parlist ) ()

#define va_init(l,p)    va_start(l)
#ifndef va_dcl  /* [ in <varargs.h> */
#define va_dcl  int va_alist;
#define Testf( parlist )        {printf parlist;}
#endif  /* ] */
#endif  /* ]] */
/*==========================================================================*/

typedef signed int      Int;
typedef unsigned int    Ord;

typedef signed char     Int8;
typedef unsigned char   Ord8;

typedef signed short    Int16;
typedef unsigned short  Ord16;

typedef signed long     Int32;
typedef unsigned long   Ord32;

/*--------------------------------------------------------------------------*/

#define IGN( anything_to_ignore )

#define Clear( ptr, size )      memset( ptr, '\0', size )

#define Shift( v, e ) ( (e)>0 ? (v)<<(e) : (e)<0 ? (v)>>-(e) : (v) )

/*--------------------------------------------------------------------------*/

#ifndef offsetof /* [ in <stddef.h> */
#define offsetof( type, comp )  ((int) & (((type *)0)->comp ))
#endif  /* ] */

#define structof( type, comp, ptr ) \
        ((type *)((char *)(ptr) - offsetof( type, comp )))

    /* (ptr) zeigt auf die Komponente 'comp' einer Struktur vom Typ 'type'!
       'structof' liefert als Ergebnis die Anfangs-Adresse dieser Struktur.
    */
/*==========================================================================*/
#endif  /* ] c_defs_h */
