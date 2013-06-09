/*==============================================================================================*/
/* ATR TCN Network Management extensions											            */
/* Header file (atr_nma.h)      													            */
/* 																					            */
/* Version 1.3																		            */
/*																					            */
/* (c)1997 Ansaldo Trasporti														            */
/* Written by Giuseppe Carnevale													            */
/*==============================================================================================*/


/*==============================================================================================*/
/* Includes */

#include "conf_tcn.h"


/*==============================================================================================*/
/* Defines */

/* Error codes */
#define ATR_NMA_OK		0
#define ATR_NMA_BUSY	1
#define ATR_NMA_FAILURE	-1

/* NMA services supported by ATR */
#define ATR_NMA_READ_NCDB_SIF			128
#define ATR_NMA_START_NCDB_UPDATE_SIF	129
#define ATR_NMA_WRITE_NCDB_SIF			130
#define ATR_NMA_END_NCDB_UPDATE_SIF		131
#define ATR_NMA_SHELL_SIF				132

/* XMODEM NCDB error codes (the standard XMODEM codes could also be returned) */
#define XMODEM_NCDB_WRITE_ERR	1000	/* got an error writing the NCDB             */
#define XMODEM_NCDB_FORMAT_ERR	1001	/* got an error in the NCDB we are receiving */
#define XMODEM_NCDB_MEMORY_ERR	1002	/* not enough memory for this NCDB           */


/*==============================================================================================*/
/* Typedefs */

typedef void (*FilterNcdbObjFptr)(NcObjHeader *header);


/*==============================================================================================*/
/* Prototypes */

short start_ext_ncdb_update(void);
short write_ext_ncdb_object(const NcObjHeader *header);
short end_ext_ncdb_update(void);

short xmodem_ext_ncdb_rcv(FilterNcdbObjFptr filter_ncdb_obj_func);

short atr_nma_init(void);
short atr_nma_call(unsigned char veh, unsigned char sta, unsigned char tc,
				   unsigned char sif, const char *data_out, unsigned short data_out_len,
				   char **data_in, unsigned short *data_in_len_p);


/*==============================================================================================*/

