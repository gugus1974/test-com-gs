/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> added t_bitfield and t_boolean types required by the ABB WTB LL                     */
/* <ATR:02> added MAX and MIN macros required by the ABB WTB LL                                 */
/* <ATR:03> header macro renamed from TCNTYPES_H to TCNTYPES_H_                                 */
/* <ATR:04> added the WORD32 definition                                                         */
/* <ATR:05> added BOOLEAN2 values definition required by the PDM                                */
/*==============================================================================================*/


/*************************************************************************
**        Copyright (C) Siemens AG 1994 All Rights Reserved. Confidential    
**========================================================================
**
**    PROJECT::    TCN Train Communication Network
                   JDP Joint Development Project

**    MODULE::     General Data Types

**    WORKFILE::   $Workfile:   tcntypes.h  $
**************************************************************************
**    TASK::       Machine dependent data types byte, word, double word
                   PV_Name 48 bit unsigned integer big-endian
                   Datatypes defined in Real-Time Protocols 1.4.0/4.2.9 
                   page 133
**************************************************************************
**    AUTHOR::     E. Renner
**    CREATED::    04.10.94
**========================================================================
**    HISTORY::    AUTHOR::   $Author: mungiguerrav $
                   REVISION:: $Revision: 1.2 $  
                   MODTIME::  $Date: 2004/09/02 10:01:32 $
      $Log: TCNTYPES.H,v $
      Revision 1.2  2004/09/02 10:01:32  mungiguerrav
      2004/set/02,gio 11:50   Napoli          Mungi
      - Merge da branch di sviluppo  makefile
      - In effetti su tale branch si sono spostate in  TCNS\SRC tutti i componenti
        che nella organizzazione di Carnevale erano sotto SW\TCN\JDP\XX: la
        posizione originaria puo' essere ricavata dall'inizio del nome del componente
        o dal commento nel file TCNS\makefile
      - Si spera quanto prima di inserire i token di gestioni automatiche CVS
        (DATE, FILE, AUTHOR, LOG), per abolire i file .mod o i pezzi di storia
        inseriti senza regole nei componenti stessi
      - Si spera soprattutto che qualcuno con la nuova organizzazione si prenda
        la briga di migliorare la qualita' di questo modulo, sia in termini formali che
        prestazionali (MNG)

      Revision 1.1.2.1  2004/07/09 16:24:38  buscob
      Addio Monolite !!

  
     Rev 1.1   Feb 23 1996 12:34:06   HEINDEL_AR
  header comment changed
  48 bit PV_Name data type
**************************************************************************/



/* CAUTION !!! these types may depend on your targed system */

/* <ATR:03> */
#ifndef TCNTYPES_H_
#define TCNTYPES_H_

/* <ATR:01> */
typedef unsigned int    t_bitfield; /* basetype for bitfields */
typedef unsigned char   t_boolean;  /* TRUE or FALSE */

/* <ATR:02> */
#ifndef MAX
#define MAX( a, b)      ( (a>=b) ? (a) : (b) )
#endif
#ifndef MIN
#define MIN( a, b)      ( (a<=b) ? (a) : (b) )
#endif

/* <ATR:04> */
#define WORD32        UNSIGNED32

/* <ATR:05> */
#define BOOLEAN2_FALSE (1)
#define BOOLEAN2_TRUE  (2)

typedef struct timedate { 
    unsigned short times[3];
} timedate;

typedef struct unsigned64 {
    unsigned long longs[2];
} unsigned64;

#define BOOLEAN       unsigned char
#define BITSET8       unsigned char
#define UNSIGNED8     unsigned char
#define INTEGER8      char
#define CHARACTER     char
#define ENUM8         unsigned char
#define ENUM16        unsigned short
#define BITSET16      unsigned short
#define UNSIGNED16    unsigned short
#define INTEGER16     short
#define UNSIGNED32    unsigned long
#define INTEGER32     long
#define REAL32        float
#define TIMEDATE48    timedate
#define UNSIGNED64    unsigned64


/* A PV_Name shall be treated as a 48-bit unsigned integer. It is 
 * constructed by 6 byte objects. The order of the parts of
 * the 48 bit unsigned integer is processor independent BIG-ENDIAN
 * (the norm is silent about the endianess!)
 */
typedef struct {
    UNSIGNED8 b0;
    UNSIGNED8 b1;
    UNSIGNED8 b2;
    UNSIGNED8 b3;
    UNSIGNED8 b4;
    UNSIGNED8 b5;
} TYPE_PV_NAME_48;

/* Macro to initialize a PV_Name,
 * refer to TCN Standard Part 2, PV_Name format 
 */
#define PVN48INIT(tsid,port,size,varoff,type,chkoff) \
    { ((tsid) << 4) | ((port) >> 8), 	\
      (port) & 0xff, 			\
      ((size) << 2) | ((varoff) >> 8), 	\
      (varoff) & 0xff, 			\
      ((type) << 2) | ((chkoff) >> 8), 	\
      (chkoff) & 0xff }


/* Macros to access (get and set) parts of a PV_NAME */
#define PVN48GETTSID(pvn)   ((pvn).b0 >> 4)
#define PVN48GETPORT(pvn)   ((((pvn).b0 & 0xf) << 8) | (pvn).b1)
#define PVN48GETDSNAME(pvn) (((pvn).b0 << 8) | (pvn).b1)
#define PVN48GETSIZE(pvn)   ((pvn).b2 >> 2)
#define PVN48GETVAROFF(pvn) ((((pvn).b2 & 0x3) << 8) | (pvn).b3)
#define PVN48GETTYPE(pvn)   ((pvn).b4 >> 2)
#define PVN48GETCHKOFF(pvn) ((((pvn).b4 & 0x3) << 8) | (pvn).b5)

/* NOTE: the following macros evaluate their first argument more than once! */
/* NOTE: the macros check not for overflow: if values does not fit in
 *       the available space, then the PV_Name is messed!
 */
#define PVN48SETTSID(pvn,val) ((pvn).b0 = (pvn).b0 & 0x0f | ((val) << 4))
#define PVN48SETPORT(pvn,val)   \
    ((pvn).b0 = (pvn).b0 & 0xf0 | ((val) >> 8), (pvn).b1 = (val) & 0xff)
#define PVN48SETDSNAME(pvn,val) ((pvn).b0 = (val) >> 8, (pvn).b1 = (val) & 0xff)
#define PVN48SETSIZE(pvn,val) ((pvn).b2 = (pvn).b2 & 0x03 | ((val) << 2))
#define PVN48SETVAROFF(pvn,val) \
    ((pvn).b2 = (pvn).b2 & 0xfc | ((val) >> 8), (pvn).b3 = (val) & 0xff)
#define PVN48SETTYPE(pvn,val)  ((pvn).b4 = (pvn).b4 & 0x03 | ((val) << 2))
#define PVN48SETCHKOFF(pvn,val) \
    ((pvn).b4 = (pvn).b4 & 0xfc | ((val) >> 8), (pvn).b5 = (val) & 0xff)


#endif  /* _TCNTypes */

