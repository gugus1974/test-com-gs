/****************************************************************/
/* 2004/nov/05,ven 17:35                \SWISV\LIB\SRC\hw16x.h  */
/****************************************************************/
/*
    $Date: 2004/11/05 16:42:04 $          $Revision: 1.2 $
    $Author: mungiguerrav $
*/


#ifndef _HW16X_
#define _HW16X_

/*======================================================================*/
/* Prototypes */

// void hw_init(unsigned long baud_rate);

// void hw_reset(void);
// void hw_halt(void);

void hw167_nmi(void);
void hw167_wdServ(void);

/*----------------------------------------------------------------------*/

#endif


/* ****************************************************************************
    $Log: hw16x.h,v $
    Revision 1.2  2004/11/05 16:42:04  mungiguerrav
    2004/nov/05,ven 17:35          Napoli       Mungi
    - Si aggiungono keyword CVS e breve storia


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.1            2000/06/05 09:27:08     accardol
    - Inserimento in CVS della versione in lib07n
    Revision 1.1.1.1        2000/06/05 09:27:08     accardol
    - Inserimento in CVS in branch STP della versione in lib07n
        (la data è arretrata con un artificio)
01.00   2000/giu/05,lun             Napoli              Mungi
        - Creaz. a partire da LBATR\SRC\atr_hw.h v.01.10-1999/lug/13
*/

