/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> Corrected a bug in convert_big_endian16() and convert_big_endian32() for BE machines*/
/* <ATR:02> Included "atr_sys.h"                                                                */
/*==============================================================================================*/

/* <ATR:02> */
#include "atr_sys.h"


/*
********************************************************************************
*                                                                              *
*    Property of  :   ABB Verkehrssysteme AG, CH-8050 Z*rich / Switzerland     *
*    COPYRIGHT    :   (c) 1990 ABB Verkehrssysteme AG                          *
*                                                                              *
********************************************************************************
*                                                                              *
*    PROJECT      : MicOS Version 2                                            *
*                                                                              *
*    SUBPROJECT   : Message Data (Application Layer)                           *
*                                                                              *
********************************************************************************
*                                                                              *
*    MODULE       : basetype.h                                                 *
*                                                                              *
*    DOCUMENT     :                                                            *
*                                                                              *
*    ABSTRACT     : Definition of basic constants and types                    *
*                                                                              *
*    REMARKS      : -                                                          *
*                                                                              *
*    DEPENDENCIES : -                                                          *
*                                                                              *
*    ACCEPTED     :                                                            *
*                                                                              *
********************************************************************************
*                                                                              *
*    HISTORY:                                                                  *
*                                                                              *
*    Version  YY-MM-DD  Name         Dept.   Ref   Status                      *
*    -------  --------  -----        ------  ----  -------                     *
*     1.0     92-06-29  E. Hoepli    CRBC-1   --   created                     *
*     1.1     94-01-31  zur Bonsen   BATL     --   removed non-ASCII chars     *
*     1.2     95-04-20  R.Flum       BATL     --   Compiler Switch O_LE(Little *
*                                                  Endian) added, to           *
*                                                  distuingish between a       *
*                                                  little and big endian device*
********************************************************************************
*/

#ifndef         BASETYPE_H
    #define     BASETYPE_H
/*
*****************************************
* basic constants, types and functions  *
*****************************************
*/

typedef unsigned short CARDINAL16;

typedef unsigned long  SET32;
typedef unsigned short SET16;
typedef unsigned char  SET8;

#define	BIT0	0x0001
#define	BIT1	0x0002
#define	BIT2	0x0004
#define	BIT3	0x0008
#define	BIT4	0x0010
#define	BIT5	0x0020
#define	BIT6	0x0040
#define	BIT7	0x0080
#define	BIT8	0x0100
#define	BIT9	0x0200
#define	BITA	0x0400
#define	BITB	0x0800
#define	BITC	0x1000
#define	BITD	0x2000
#define	BITE	0x4000
#define	BITF	0x8000

#define	BITPOS( x )	( (unsigned short)1 << (unsigned short)( x ) )

#ifdef O_LE

  #define convert_big_endian16( dest_addr, src_addr )\
  {typedef struct {char byte_0, byte_1;} b2;\
    ((b2 *)(dest_addr))->byte_0 = ((b2 *)(src_addr))->byte_1;\
    ((b2 *)(dest_addr))->byte_1 = ((b2 *)(src_addr))->byte_0;}

  #define convert_big_endian32( dest_addr, src_addr )\
  {typedef struct {char byte_0, byte_1, byte_2, byte_3;} b4;\
    ((b4 *)(dest_addr))->byte_0 = ((b4 *)(src_addr))->byte_3;\
    ((b4 *)(dest_addr))->byte_1 = ((b4 *)(src_addr))->byte_2;\
    ((b4 *)(dest_addr))->byte_2 = ((b4 *)(src_addr))->byte_1;\
    ((b4 *)(dest_addr))->byte_3 = ((b4 *)(src_addr))->byte_0;}

#else

/* <ATR:01> */
  #define convert_big_endian16( dest_addr, src_addr )\
  { *(unsigned short *) (dest_addr) = *(unsigned short *)(src_addr); }

  #define convert_big_endian32( dest_addr, src_addr )\
  { *(unsigned long *) (dest_addr) = *(unsigned long *)(src_addr); }

#endif


/* functions: ------------------------------------------------------------*/

#if(0)
void convert_big_endian (char * dest       /* out */,
			 const void * src  /* in  */,
			 unsigned len);
#endif
/* Copies 'len' bytes starting from 'src' address to 'dest' address.
   Copies in reverse order (i.e. inverts the sequence of bytes in the
   whole block) if the local format is little-endian.
   The arrays at 'src' and 'dest' must not overlap. */

#endif  /* BASETYPE_H */
