/****************************************************************************/
/*  Copyright ( C ) Siemens AG 1996 All Rights Reserved. Confidential       */
/****************************************************************************/

#define PDM_MODULE	/* access to PDM internal definitions */


#include <stdio.h>

#ifdef  __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#include "c_defs.h"
#include "pdmdebug.h"

#ifdef DEBUG
int tcndebug = 1;
#endif

#ifdef  __STDC__
void pr_err( char *fmt, ... )
#else
void pr_err( fmt, va_alist )
    char        *fmt;
    va_dcl
#endif
{
    va_list  ap;

    va_init( ap, fmt );
    vfprintf( stderr, fmt, ap );
    va_end( ap );

    fflush( stderr );
    return;
}
