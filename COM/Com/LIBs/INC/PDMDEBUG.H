/*********************************************************************
 *  Copyright (C) Siemens AG 1996 All rights reserved. Confidential
 *
 *  PDM - DEBUG interface    
 *                           
 *  Module:      pdmdebug.h  
 *********************************************************************/

#ifndef _PDMDEBUG_H
#define _PDMDEBUG_H

#if defined(__cplusplus)
extern "C" {
#endif


#ifdef DEBUG
extern int tcndebug;

void pr_err( char *fmt, ... );

#define DEBUGprintf(flag,x) { if (flag & tcndebug) { pr_err x ; }}
#define STATIC
#else
#define DEBUGprintf(flag,x) 
#define STATIC static
#endif


#define DEBUGPDM       0x00000001


#define PDM_Dbg(x)     DEBUGprintf(DEBUGPDM, x)

#ifdef KRALLOC
#define malloc( size ) 		kralloc( 1, (size), __FILE__, __LINE__)
#define calloc( n, size ) 	kralloc((n), (size), __FILE__, __LINE__)
#define realloc( p, size ) 	krealloc((p), (size), __FILE__, __LINE__)
#define free( ptr ) 		krfree((ptr))
#endif

#if defined(__cplusplus)
}
#endif
                                         

#endif /* _PDMDEBUG_H */                     

