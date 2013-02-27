/****************************************************************/
/* 2004/nov/05,ven 15:55                \SWISV\LIB\SRC\consio.c */
/****************************************************************/
/*      Gestione dello Standard I/O per consolle                */
/****************************************************************/
/*
    $Date: 2004/11/05 15:08:46 $          $Revision: 1.2 $
    $Author: mungiguerrav $
*/


#include "stdio.h"
#include "consio.h"
#include "query.h"

static consio_tGetFunPtr ptGetFun = NULL;
static consio_tPutFunPtr ptPutFun = NULL;

consio_tGetFunPtr  consio_SetGetFun(consio_tGetFunPtr funPtr)
{
	consio_tGetFunPtr tmp;

	tmp      = ptGetFun;
	ptGetFun = funPtr;

	return (tmp);
}

consio_tPutFunPtr  consio_SetPutFun(consio_tPutFunPtr funPtr)
{
	consio_tPutFunPtr tmp;

	tmp      = ptPutFun;
	ptPutFun = funPtr;

	return (tmp);
}

char consio_getkey(void)
{
    char    c;

	if (ptGetFun)
	{
	    c = query_filter((*ptGetFun)());
	}
	else
	{
	    c = EOF;
	}

	return (c);
}

char consio_putchar(char c)
{
	/* add a CR before a LF */
	if (c == 10) consio_putchar(13);

	if (ptPutFun)
	{
	    (*ptPutFun)(c);
	}

	return c;
}


/* ****************************************************************************
    $Log: consio.c,v $
    Revision 1.2  2004/11/05 15:08:46  mungiguerrav
    2004/nov/05,ven 15:55          Napoli       Mungi
    - Si aggiungono keyword CVS e breve storia


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.1            2001/11/08 18:28:58     accardol
    - Inserimento in CVS della versione in lib07n
    Revision 1.1.1.1        2001/11/08 18:28:58     accardol
    - Inserimento in CVS in branch STP della versione in lib07n
        (la data è arretrata con un artificio)
01.00   2001/nov/08,gio             Napoli              Mungi\Busco
        - Creazione (a partire da idee in sio.c di Carnevale)
*/

