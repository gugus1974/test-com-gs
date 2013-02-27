/*=====================================================================================*/
/* TM layer trace routines   														   */
/* Header file (traceenb.h)															   */
/* 																					   */
/* Version 1.0																		   */
/*																					   */
/* (c)1996 Ansaldo Trasporti														   */
/* Written by Giuseppe Carnevale													   */
/*=====================================================================================*/


/*=====================================================================================*/
/* Includes */

#include "am_sys.h"
#include "nm_layer.h"
#include "tm_layer.h"
#include "tm_sub.h"
#include "md_layer.h"
#include <stdio.h>


/*=====================================================================================*/
/* Pragmas */

#ifdef __C166__
#pragma WL(3)
#endif


/*=====================================================================================*/
/* Globals */

char nm_trace_enable = 0;	/* if 1 the tracing is enabled */


/*=====================================================================================*/
/* Routines */

/*-------------------------------------------------------------------------------------*/
static void __dump_pack(MD_PACKET *pack, unsigned int size)
{
    unsigned int	i;
    unsigned char 	mtc = pack->port.lk.mtc;

	/* link layer header */
	printf("(");
	if (pack->port.lk.version.mvb.prot_hsrc == 0x80) {		/* MVB link header */
		printf("V:");
		i = pack->port.lk.version.mvb.mode_hdest;
		if ((i & 0xF0) == 0x10)
			printf("%02x", ((i & 0x0F) << 8) | pack->port.lk.version.mvb.dest);
		else if ((i & 0xF0) == 0xF0) printf("--");
		else printf("??");
		printf(",%02x", ((pack->port.lk.version.mvb.prot_hsrc & 0x0F) << 8) |
						pack->port.lk.version.mvb.src);
	}
	else {															/* WTB link header */
		printf("T:%02x,%02x", pack->port.lk.version.wtb.dest, pack->port.lk.version.wtb.src);
	}
	printf(",%02x)(", pack->port.lk.size);

	/* network layer header */
	if (pack->port.lk.final_vehicle & 0x80) printf("S");
	else printf("U");
	printf("%x,", (pack->port.lk.final_vehicle & 0x80) >> 6 |
				  (pack->port.lk.origin_vehicle & 0x80)>> 7);
	printf("%02x,%02x,%02x,%02x)", (int)(pack->port.lk.final_vehicle & 0x3F),
		   (int)pack->port.lk.final_fct_or_dev,(int)(pack->port.lk.origin_vehicle & 0x3F),
		   (int)pack->port.lk.origin_fct_or_dev);

	/* print the flags */
	if (mtc & 0x80) printf("C");
	else printf("R");
	if (mtc & 0x40) printf("C: ");
	else printf("P: ");
	mtc &= 0x3F;

	if ((mtc & 0x30) == 0x10) {			/* DT */
		i = mtc & 7;
		printf("DT%u", i);
 		if (mtc & 0x08) printf("E");
  		else printf(" ");
   	}
	else if ((mtc & 0x38) == 0x20) {	/* AK */
		i = mtc & 7;
		printf("AK%u ", i);
   	}
	else if ((mtc & 0x38) == 0x30) {	/* NK */
		i = mtc & 7;
		printf("NK%u ", i);
   	}
	else if ((mtc & 0x3F) == 0x00) {	/* CR */
		printf("CR  ");
   	}
	else if ((mtc & 0x3F) == 0x01) {	/* CC */
		printf("CC  ");
   	}
	else if ((mtc & 0x3F) == 0x02) {	/* DR */
		printf("DR  ");
   	}
	else if ((mtc & 0x3F) == 0x03) {	/* DC */
		printf("DC  ");
   	}
   	else printf("??  ");
        
    printf(" (");
    for (i = 0; i < size - 5; i++) {
    	printf("%02x", (int)pack->port.lk.data[i]);
    }
    printf(")\n");
}              


/*-------------------------------------------------------------------------------------*/
static void __nm_send_pack(MD_PACKET *pack, unsigned int size, const TM_LINK_ID *src,
						   const TM_LINK_ID *dest, AM_RESULT *error)
{
	nm_send_pack(pack, size, src, dest, error);
	if (nm_trace_enable & 1) {
		if (*error) printf("!%d", (int)*error);
		printf("<<");
		__dump_pack(pack, size);
 	}
}


/*-------------------------------------------------------------------------------------*/
static int __nm_rcv_pack(MD_PACKET **pack, unsigned int *size, TM_LINK_ID *src,
						 TM_LINK_ID *dest, AM_RESULT *error)
{
	int result = nm_rcv_pack(pack, size, src, dest, error);

	if (result && (nm_trace_enable & 1)) {
		if (*error) printf("!%d", (int)*error);
		printf(">>");
		__dump_pack(*pack, *size);
	}

	return result;
}


/*-------------------------------------------------------------------------------------*/
void __signal_tmo(void *param)
{
	signal_tmo(param);

	if (nm_trace_enable & 2) {
		CONN_DESCR	*conn = (CONN_DESCR*)param;
 		char		valid = 0;

		printf("<TMO> ");
		printf(conn->my_sap & 0x80 ? "C: " : "R: ");
		switch(conn->state) {
			case SETUP:			printf("setup");		valid = 1;	break;
  			case SEND:			printf("send");			valid = 1;	break;
			case LISTEN:		printf("listen");					break;
			case LISTEN_FROZEN:	printf("listen_frozen");			break;
  			case RECEIVE:		printf("receive");					break;
  			case PEND_ACK:		printf("pend_ack");					break;
  			case FROZEN:		printf("frozen");					break;
  			case RCV_CANC:		printf("rcv_canc");		valid = 1;	break;
  			case SEND_CANC:		printf("send_canc");	valid = 1;	break;
  			case CLOSED:		printf("closed");					break;
  			default:			printf("invalid option");
		}
  		if (valid) printf(" %3.3u", conn->cnt.rep);
  		printf("\n");
	}
}


/*=====================================================================================*/
/* Function intercept */

#define nm_send_pack(pack, size, src, dest, error) __nm_send_pack(pack, size, src, dest, error)
#define nm_rcv_pack(pack, size, src, dest, error) __nm_rcv_pack(pack, size, src, dest, error)
#define signal_tmo __signal_tmo


/*=====================================================================================*/
/* Cleanup */

#ifdef __C166__
#pragma WL(0)
#endif
#undef TRACE_ENB
#undef MULTICAST

