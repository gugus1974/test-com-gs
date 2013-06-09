/*=============================================================================================*/
/* X25642 serial E2PROM access routines                                                        */
/* Header file (x25642.h)       													           */
/* 																					           */
/* Version 1.0																		           */
/*																					           */
/* (c)1998 Ansaldo Trasporti														           */
/* Written by Giuseppe Carnevale													           */
/*=============================================================================================*/

#ifndef _X25642_
#define _X25642_


/*=============================================================================================*/
/* Constants */

/* X25642 error codes */
#define X25642_OK					0
#define X25642_UNINITIALIZED_ERR	1
#define X25642_FAILURE_ERR			2


/*=============================================================================================*/
/* Typedefs */

typedef short (*X25642_select_fptr)(char select_flag);
typedef short (*X25642_transfer_fptr)(void *buf, unsigned short size);


/*=============================================================================================*/
/* Prototypes */

short x25642_init(X25642_select_fptr select_fptr, X25642_transfer_fptr transfer_fptr);
short x25642_read_status(unsigned char *status);
short x25642_write_status(unsigned char status);
short x25642_read(unsigned short address, char *buf, unsigned short size);
short x25642_write(unsigned short address, const char *buf, unsigned short size);


/*=============================================================================================*/

#endif

