/****************************************************************/
/* 2004/nov/11,gio 15:25                \SWISV\LIB\SRC\sio.h    */
/****************************************************************/
/*          Gestione dello Standard I/O                         */
/****************************************************************/
/*
    $Date: 2004/11/11 14:49:06 $          $Revision: 1.2 $
    $Author: mungiguerrav $
*/


#ifndef _SIO_
#define _SIO_

/*==============================================================*/
/* Includes */
#include <stdio.h>

/*==============================================================*/
/* Prototypes */
typedef void (*SioGetFuncPtr)(char *buf, short req_num, short timeout, short *act_num);
typedef void (*SioPutFuncPtr)(const char *buf, short req_num);

/*==============================================================*/
/* Prototypes */
SioGetFuncPtr   sio_set_get(    SioGetFuncPtr get_func_ptr);
SioPutFuncPtr   sio_set_put(    SioPutFuncPtr put_func_ptr);

void            sio_get(              char *buf, short req_num, short timeout, short *act_num);
void            sio_put(        const char *buf, short req_num);

char            sio_poll_key(   short timeout);
char            sio_get_key(    void);
char            sio_put_char(   char c);

#endif


/* ****************************************************************************
    $Log: sio.h,v $
    Revision 1.2  2004/11/11 14:49:06  mungiguerrav
    2004/nov/11,gio 15:25          Napoli       Mungi
    - Si aggiungono keyword CVS e breve storia documentata
    - Si applica regole di codifica per incolonnamento
    - Non si applicano ancora regole di codifica per nomi routine e variabili


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.1        1997/09/03 14:02:24     accardol
    - Inserimento in CVS della versione in lib07n
    Revision 1.1.1.1    1997/09/03 14:02:24     accardol
    - Inserimento in CVS in branch STP della versione in lib07n
        La data è arretrata con un artificio
01.01   1997/set/03,mer             Napoli              Carnevale
        - Modifiche non documentate
01.00   1996/xxx/XX,xxx             Napoli              Carnevale
        - Creazione
*/
