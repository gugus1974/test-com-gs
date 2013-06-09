/****************************************************************/
/* 2004/nov/08,lun 11:10                \SWISV\LIB\SRC\dp_ram.c */
/****************************************************************/
/*      Struttura fittizia di dp_ram condivisa                  */
/****************************************************************/
/*
    $Date: 2004/11/08 10:18:54 $          $Revision: 1.2 $
    $Author: mungiguerrav $
*/


/* Includes */

#include "dp_ram.h"


/*----------------------------------------------------------*/
/* Dual-Port RAM allocation */

#pragma HL(NEAR 0)              /* force the default allocation type */
#pragma NOINIT                  /* do not clear the Dual-Port RAM */
volatile DpRam huge dp_ram;     /* Dual-Port RAM structure */


/* ****************************************************************************
    $Log: dp_ram.c,v $
    Revision 1.2  2004/11/08 10:18:54  mungiguerrav
    2004/nov/08,lun 11:10          Napoli       Mungi
    - Si aggiungono keyword CVS e breve storia


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.1            1997/07/16 13:52:00     accardol
    - Inserimento in CVS della versione in lib07n
    Revision 1.1.1.1        1997/07/16 13:52:00     accardol
    - Inserimento in CVS in branch STP della versione in lib07n
        La data è arretrata con un artificio
01.00   1997/lug/16,mer             Napoli              Mungi
        - Creazione a partire da \CARN01\STAR\PRJS\TEST_DP\dp_ram.c
        - Si elim. parti relative a dp_comm
        - Si dichiara huge la dp_ram, per compat. DAU
*/

