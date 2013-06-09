/****************************************************************/
/* 2004/nov/05,ven 17:30                \SWISV\LIB\SRC\hw16x.c  */
/****************************************************************/
/*      Hardware routines for 164/166/167 boards                */
/****************************************************************/
/*
    $Date: 2004/11/05 16:41:52 $          $Revision: 1.2 $
    $Author: mungiguerrav $
*/


#include <intrins.h>

/*==================================================================*/
/* NMI e watchdog routines */

/*------------------------------------------------------------------*/
void hw167_nmi()
{
    _trap_(2);
}

/*------------------------------------------------------------------*/
void hw167_wdServ(void)
{
    /* service the watchdog timer */
    _srvwdt_();
}


/* ****************************************************************************
    $Log: hw16x.c,v $
    Revision 1.2  2004/11/05 16:41:52  mungiguerrav
    2004/nov/05,ven 17:30          Napoli       Mungi
    - Si aggiungono keyword CVS e breve storia


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.1            2000/06/05 09:26:44     accardol
    - Inserimento in CVS della versione in lib07n
    Revision 1.1.1.1        2000/06/05 09:26:44     accardol
    - Inserimento in CVS in branch STP della versione in lib07n
        (la data è arretrata con un artificio)
01.00   2000/giu/05,lun             Napoli              Mungi
        - Creaz. a partire da pezzo di LIB\SRC\hw_star.c v.01.20-2000/mar/09
*/

