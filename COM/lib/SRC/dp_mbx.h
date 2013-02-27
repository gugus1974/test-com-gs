/********************************************************************/
/* 2004/nov/08,lun 10:45                \SWISV\LIB\SRC\dp_mbx.h     */
/********************************************************************/
/*                                                                  */
/********************************************************************/
/*
    $Date: 2004/11/08 09:54:15 $          $Revision: 1.2 $
    $Author: mungiguerrav $
*/


/*==============================================================================================*/
/* Dual port RAM mailbox manager			    									            */
/* Header file (dp_mbx.h)         													            */
/* 																					            */
/* Version 1.0																		            */
/*																					            */
/* (c)1996 Ansaldo Trasporti														            */
/* Written by Giuseppe Carnevale													            */
/*==============================================================================================*/

#ifndef _DP_MBX_
#define _DP_MBX_


/*==============================================================================================*/
/* Typedefs */

#define DP_MBX_MAX_MAILBOXES	32		/* maximum number of mailboxes */


/*==============================================================================================*/
/* Typedefs */

/* Mailbox handler function */
typedef void (*DpMbxHandler)(short request_id, short request_data);


/*==============================================================================================*/
/* Prototypes */

/*----------------------------------------------------------------------------------------------*/
/* Library functions */

void    dp_mbx_init(                volatile short huge *mbx_p, short mbx_num );
void    dp_mbx_register_handler(    short request_id, DpMbxHandler handler_p );
short   dp_mbx_send_request(        short request_id, short request_data );
void    dp_mbx_clear_request(       short request_id );
void    dp_mbx_poll(                void );


/*----------------------------------------------------------------------------------------------*/
/* User provided functions */

extern void dp_mbx_clear_signal( void );
extern void dp_mbx_set_signal(short request_id);


/*==============================================================================================*/

#endif


/* ****************************************************************************
    $Log: dp_mbx.h,v $
    Revision 1.2  2004/11/08 09:54:15  mungiguerrav
    2004/nov/08,lun 10:45          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.1            2001/11/22 12:19:20     accardol
    - Inserimento in CVS della versione in lib07n
    Revision 1.1.1.1        2001/11/22 12:19:20     accardol
    - Inserimento in CVS in branch STP della versione in lib07n
        La data è arretrata con un artificio
01.01   2001/nov/22,gio             Napoli              Mungi
        - Si agg. i void negli argomenti dei prototipi:
                    void    dp_mbx_poll(            void );
            extern  void    dp_mbx_clear_signal(    void );

 ------------------------   Prima di CVS    ------------------------
01.00   1997/mag/13,mar             Napoli              Carnevale
        - Versione di Carnevale
*/

