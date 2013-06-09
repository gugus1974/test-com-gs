/********************************************************************/
/* 2004/nov/08,lun 10:40                \SWISV\LIB\SRC\dp_mbx.c     */
/********************************************************************/
/*                                                                  */
/********************************************************************/
/*
    $Date: 2004/11/08 09:53:16 $          $Revision: 1.3 $
    $Author: mungiguerrav $
*/


/*==============================================================================================*/
/* Dual port RAM mailbox manager       												            */
/* Implementation file (dp_mbx.c)               									            */
/* 																					            */
/* Version 1.0																		            */
/*																					            */
/* (c)1996 Ansaldo Trasporti														            */
/* Written by Giuseppe Carnevale													            */
/*==============================================================================================*/


/*==============================================================================================*/
/* Includes */

#include "dp_mbx.h"


/*==============================================================================================*/
/* Globals */

static volatile short huge	*mbx;							/* mailboxes ptr */
static short				mbx_size;						/* number of mailboxes */
static DpMbxHandler			handler[DP_MBX_MAX_MAILBOXES];	/* handlers function ptrs */


/*==============================================================================================*/
/* Public routines */

/*----------------------------------------------------------------------------------------------*/
void dp_mbx_init(volatile short huge *mbx_p, short mbx_num)
{
	/* save the address and the number of the mailboxes */
	mbx = mbx_p;
	mbx_size = mbx_num;
}


/*----------------------------------------------------------------------------------------------*/
void dp_mbx_register_handler(short request_id, DpMbxHandler handler_p)
{
	/* clear the corresponding mailbox */
	mbx[request_id] = 0;

	/* register the handler function overwriting the old one */
	handler[request_id] = handler_p;
}


/*----------------------------------------------------------------------------------------------*/
short dp_mbx_send_request(short request_id, short request_data)
{
	/* return error if the handler has not yet processed the last request */
	if (mbx[request_id]) return 0;

	if (request_data) {
		/* save the request */
		mbx[request_id] = request_data;

		/* send the signal to the server */
		dp_mbx_set_signal(request_id);
	}

	return 1;
}


/*----------------------------------------------------------------------------------------------*/
void dp_mbx_clear_request(short request_id)
{
	/* clear a request */
	mbx[request_id] = 0;
}


/*----------------------------------------------------------------------------------------------*/
void dp_mbx_poll()
{
	short	i;	/* generic index */

	/* clear the external signal */
	dp_mbx_clear_signal();

	/* examine the request data calling the corresponding handlers */
	for (i = 0; i < mbx_size; i++) if (handler[i]){
		short x = mbx[i];
		if (x) (*handler[i])(i, x);
	}
}


/* ****************************************************************************
    $Log: dp_mbx.c,v $
    Revision 1.3  2004/11/08 09:53:16  mungiguerrav
    2004/nov/08,lun 10:40          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.2            2004/06/15 11:29:32     buscob
01.01   2003/nov/21,ven             Napoli              Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso

    Revision 1.1            1997/05/13 12:46:50     accardol
    - Inserimento in CVS della versione in lib07m
    Revision 1.1.1.1        1997/05/13 12:46:50     accardol
    - Inserimento in CVS in branch STP della versione in lib07m
        La data è arretrata con un artificio
01.00   1997/mag/13,mar             Napoli              Carnevale
        - Versione di Carnevale
*/

