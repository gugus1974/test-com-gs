/******************************************************************************
*         Copyright (C) Siemens AG 1994 All Rights Reserved                   *
*******************************************************************************
*
*    project:     TCN Train Communication Network
*                 JDP Joint Development Project
*
*    title:       Network Management Agent
*
*
*******************************************************************************
*
*    description: the agent part of TRAIN NETWORK MANAGEMENT
*
*    department:  VT 596 Communication
*    version:     0.0
*    language:    ANSI-'C'
*    system:
*
*    author:      E. Renner
*    history:     date        sign   remark
*                 05.10.1994  Re     created
*                 29.03.1996  Hil    'ma_encodeString()' modified
*
*
******************************************************************************/


/******************************************************************************
*
*   name:         MMutil.h
*
*   function:     Utilities for the Network Management Agent.
*
*
*
*   input:        -
*
*   output:       -
*
*   return value: -
*
******************************************************************************/




#include "tcntypes.h"
#include "tnm.h"
#include <stddef.h>


int ma_encodeString(char * dest, char * source)   
{
  int i;

  if (source == NULL) return 0;

  for (i = 0; *source != 0 ; i++) *dest++ = *source++;

  /* now place the terminating zero */
  *dest++ = 0;
  i++;

  /* padding to long address */
  while (i & 3) {
  	*dest++ = 0;
  	i++;
  }
  
  return i;
}

int ma_encodeUNSIGNED8(unsigned char * dest, UNSIGNED8 source)
{
	*dest    =  source;      /* place low-order byte  */
	return(1);               /* Laenge in Bytes zurueckliefern */
}

int ma_encodeUNSIGNED16(unsigned char * dest, UNSIGNED16 source)
{
   *dest =  source>>8;     /* place high-order byte  */
    dest++;
   *dest    =  source;     /* place low-order byte  */
	return(2);
}

int ma_encodeUNSIGNED32(unsigned char * dest, UNSIGNED32 source)
{
  *dest =  source>>24;  /* place high-order byte of high-order word */
	dest++;
  *dest =  source>>16;  /* place low-order  byte of high-order word */
   dest++;
  *dest =  source>>8;   /* place high-order byte of low-order  word */
   dest++;
  *dest =  source;      /* place low-order  byte of low-order  word */
  return(4);
}


int ma_decodeString(char * dest, char * source)
{
	int i;

	if (source == NULL) return 0;

    for (i = 0; *source != 0 ; i++) *dest++ = *source++;

	/* now place the terminating zero */
    *dest++ = 0;
    i++;

	/* if there was alignement, increment len to decode furtheron correctly */
	return i + (4 - (i & 3)) & 3;
}

UNSIGNED8 ma_decodeUNSIGNED8(unsigned char * source)
{
  return(*source);
}


UNSIGNED16 ma_decodeUNSIGNED16(unsigned char * source)
{
UNSIGNED16 tmp = 0;
UNSIGNED16 dest = 0;
  tmp         = *source++;
  tmp         = tmp & 0xff;
  dest        = tmp<<8;
  tmp         = *source;
  tmp         = tmp & 0xff;
  dest       |= tmp;

  return(dest);
}


UNSIGNED32 ma_decodeUNSIGNED32(unsigned char * source)
{
UNSIGNED32 tmp  = 0;
UNSIGNED32 dest = 0;

	tmp   =  *source++;
   tmp   = tmp & 0xff;
	dest |=  tmp<<24;
	tmp   =  *source++;
	tmp   = tmp & 0xff;
	dest |=  tmp <<16;
	tmp   =  *source++;
	tmp   = tmp & 0xff;
	dest |=  tmp<<8;
	tmp   =  *source;
	tmp   = tmp & 0xff;
	dest |=  tmp;
	return(dest);
}

