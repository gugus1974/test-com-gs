/****************************************************************/
/* 2004/nov/05,ven 16:00                \SWISV\LIB\SRC\consio.h */
/****************************************************************/
/*      Gestione dello Standard I/O per consolle                */
/****************************************************************/
/*
    $Date: 2004/11/05 15:08:59 $          $Revision: 1.2 $
    $Author: mungiguerrav $
*/


#ifndef _CONSIO_
#define _CONSIO_

#include <stdio.h>

typedef             char (*consio_tGetFunPtr)(void);
typedef             char (*consio_tPutFunPtr)(char c);

consio_tGetFunPtr   consio_SetGetFun(consio_tGetFunPtr funPtr);
consio_tPutFunPtr   consio_SetPutFun(consio_tPutFunPtr funPtr);
char                consio_getkey(void);
char                consio_putchar(char c);

#endif


/* ****************************************************************************
    $Log: consio.h,v $
    Revision 1.2  2004/11/05 15:08:59  mungiguerrav
    2004/nov/05,ven 16:00          Napoli       Mungi
    - Si aggiungono keyword CVS e breve storia


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.1            2001/11/08 18:28:52     accardol
    - Inserimento in CVS della versione in lib07n
    Revision 1.1.1.1        2001/11/08 18:28:52     accardol
    - Inserimento in CVS in branch STP della versione in lib07n
        (la data è arretrata con un artificio)
01.00   2001/nov/08,gio             Napoli              Mungi\Busco
        - Creazione (a partire da idee in sio.h di Carnevale)
*/

