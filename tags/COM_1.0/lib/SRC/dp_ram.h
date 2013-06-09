/****************************************************************/
/* 2004/nov/08,lun 11:15                \SWISV\LIB\SRC\dp_ram.c */
/****************************************************************/
/*      Struttura fittizia di dp_ram condivisa                  */
/****************************************************************/
/*
    $Date: 2004/11/08 10:19:28 $          $Revision: 1.2 $
    $Author: mungiguerrav $
*/


#ifndef _DP_RAM_
#define _DP_RAM_

/* Includes */

#include "dp_mbx.h"


/*==================================================================*/
/* mailboxes */
#define MBX_NUM         4           /* number of defined mailboxes  */


/*==================================================================*/
/* Typedefs */

typedef struct                      /**** Dual-Port RAM structure ****/
{
    short   reservedDAU[1000];      /* riservate per utenti DAU */
    short   mbx[MBX_NUM];           /* mailboxes */
} DpRam;


/*==================================================================*/
/* Globals */

extern volatile DpRam huge      dp_ram;     /* DP-RAM structure */


#endif


/* ****************************************************************************
    $Log: dp_ram.h,v $
    Revision 1.2  2004/11/08 10:19:28  mungiguerrav
    2004/nov/08,lun 11:15          Napoli       Mungi
    - Si aggiungono keyword CVS e breve storia


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.1            1997/07/16 13:52:06     accardol
    - Inserimento in CVS della versione in lib07n
    Revision 1.1.1.1        1997/07/16 13:52:06     accardol
    - Inserimento in CVS in branch STP della versione in lib07n
        La data è arretrata con un artificio
01.00   1997/lug/16,mer             Napoli              Mungi
        - Creazione a partire da \CARN01\STAR\PRJS\TEST_DP\dp_ram.h
        - Si agg. campo reservedDAU[1000] per area di lavoro utenti DAU
        - Si elim. campi relativi a dp_comm
        - Si dichiara huge la dp_ram, per compat. DAU
*/

