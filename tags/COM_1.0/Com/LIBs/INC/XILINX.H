/*=============================================================================================*/
/* Xilinx FPGAs utilities                                                                      */
/* Header file (xilinx.h)         													           */
/* 																					           */
/* Version 1.0																		           */
/*																					           */
/* (c)1999 Ansaldo Trasporti														           */
/* Written by Giuseppe Carnevale													           */
/*=============================================================================================*/

#ifndef _XILINX_
#define _XILINX_


/*=============================================================================================*/
/* Constants */

/* error codes */
#define XILINX_FAILURE_ERR	1	/* device failure during programming */
#define XILINX_FORMAT_ERR	2	/* bad program format                */


/*=============================================================================================*/
/* Typedefs */

/* function used to send serial data program to an FPGA */
typedef void (*XilinxSend)(const char *data, long size);


/*=============================================================================================*/
/* Prototypes */

short xilinx_program(const unsigned char **program_list, XilinxSend send_func, long max_size);


/*=============================================================================================*/

#endif

