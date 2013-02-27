/*==============================================================================================*/
/* UIC Mapping Server patches file													            */
/* Implementation file (atr_map2.c)													            */
/* 																					            */
/* Version 1.0																		            */
/*																					            */
/* (c)1998 Ansaldo Trasporti														            */
/* Written by Giuseppe Carnevale													            */
/*==============================================================================================*/


/*==============================================================================================*/
/* Includes */

#include <stdio.h>
#include "pi_sys.h"
#include "bmi.h"
#include "am_sys.h"
#include "c_utils.h"
#include "ushell.h"

#include "ums.h"
#include "adid.h"

void PrintNADI(void);


/*==============================================================================================*/
/* Globals */


/*==============================================================================================*/
/* Routines */

/*----------------------------------------------------------------------------------------------*/
/* UMS NADI display */

extern UNSIGNED8 *p_idts;

static short nadi_cmd(short argc, char *argv[])
{
//	UNSIGNED8		idts_number_nodes;	/* number of nodes entered in IDTS */
//	UNSIGNED16		idts_handle;		/* access handle of IDTS           */
//	UNSIGNED8		idts_index;			/* internal index in IDTS          */
//	TYPE_InaugData  idts_inaug_data;	/* inauguration data               */
	int				i, j;				/* generic indexes                  */
	TYPE_InaugData	*inaug_frame;		/* pointer to an inauguration frame */

	PrintNADI();

//	if (li_t_IDTSOpen(&idts_number_nodes, &idts_handle) == LI_OK) {
//		for (i = 0, idts_index = 0; i < idts_number_nodes; i++) {
//			if (li_t_IDTSReadNext(idts_handle, &idts_index, &idts_inaug_data) == LI_OK) {
//				printf("%02X %02X %02X %02X ", idts_inaug_data.UIC_Ind, idts_inaug_data.UIC_Ver,
//						idts_inaug_data.src_dev, idts_inaug_data.size);
//				for (j = 0; j < idts_inaug_data.size; j++) printf("%02X ", idts_inaug_data.Data[j]);
//				printf("\n");
//			}
//			else break;
//		}
//	}

	for (i = 0, inaug_frame = (TYPE_InaugData *)p_idts; i < LI_INAUG_ENTRIES_MAX; i++, inaug_frame++) {
		printf("%02X %02X %02X %02X ", inaug_frame->UIC_Ind, inaug_frame->UIC_Ver,
				inaug_frame->src_dev, inaug_frame->size);
		for (j = 0; j < inaug_frame->size; j++) printf("%02X ", inaug_frame->Data[j]);
		printf("\n");
		
	}

	return 0;
}


/*----------------------------------------------------------------------------------------------*/
/* Installs the UIC Mapping Server monitor(s) commands */

void ums_monitor(void)
{
	ushell_register("nadi", "", "Print the NADI", nadi_cmd);
}


/*----------------------------------------------------------------------------------------------*/


/*==============================================================================================*/

