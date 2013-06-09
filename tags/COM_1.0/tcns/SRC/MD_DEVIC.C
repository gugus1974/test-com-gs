/************************************************************************/
/*  Asea Brown Boveri Research Center, CH-5405 Baden, Switzerland       */
/*  Copyright 1991                                                      */
/*                                                                      */
/*  NAME:                                                               */
/*    MD_DEVIC.C                                                        */
/*                                                                      */
/*  ABSTRACT:                                                           */
/*                                                                      */
/*  DEPENDENCIES:                                                       */
/*                                                                      */
/************************************************************************/
/*                                                                      */
/*  AUTHOR  :   17-Oct-1991  CRBC1    Hoepli                            */
/*  MODIFIED:   02-Mar-1992  CRBC1    Hoepli				*/
/*              01-Dec-1992  BAEI-2   roe     lm_dummy_init for MSC	*/
/*              14-Jul-1994  CRBC1    Hoepli  md_my_tc added            */              
/*                                                                      */
/************************************************************************/

#include "am_sys.h"
#include "basetype.h"
#include "md_layer.h"


unsigned char md_my_vehicle, md_my_device, md_my_tc;

#ifdef _MSC_VER    /* Workaround for MSC, allows to link this file out  */
		   /* of a library					*/
void lm_dummy_init (void)
{
}
#endif

