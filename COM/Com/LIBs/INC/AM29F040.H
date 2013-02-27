/********************************************************************/
/* 2004/nov/12,ven 09:10                \SWISV\LIB\SRC\am29f040.h   */
/********************************************************************/
/*                  Am29F040 write routines                         */
/********************************************************************/
/*
    $Date: 2004/11/12 10:01:14 $          $Revision: 1.2 $
    $Author: mungiguerrav $
*/


#ifndef _AM29F040_
#define _AM29F040_

/*==================================================================*/
/* Constants */

/* Am29F040 error codes */
#define AM29F040_OK             0
#define AM29F040_FAILURE_ERR    1

/*==================================================================*/
/* Prototypes */

short   am29f040_check_code(    void    *base);

short   am29f040_chip_erase(    void    *base);

short   am29f040_sector_erase(  void    *base,
                                short   sector);

short   am29f040_write(         void    *base,
                                long    offset,
                                void    *buff,
                                long    buff_len);

#endif


/* ****************************************************************************
    $Log: am29f040.h,v $
    Revision 1.2  2004/11/12 10:01:14  mungiguerrav
    2004/nov/12,ven 09:10          Napoli       Mungi
        - Si aggiungono keyword CVS e breve storia documentata
        - Si sostituiscono i TAB con 4 spazi
        - Si applica regole di codifica per incolonnamento e parentesi
        - Non si applicano ancora regole di codifica per nomi routine e variabili


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.1        1997/04/16 11:25:34     accardol
    - Inserimento in CVS della versione in lib07n
    Revision 1.1.1.1    1997/04/16 11:25:34     accardol
    - Inserimento in CVS in branch STP della versione in lib07n
        La data è arretrata con un artificio
01.10   1997/apr/16,mer             Napoli              Carnevale
        - Modifiche non documentate

 ------------------------   Prima di CVS    ------------------------
01.01   1997/xxx/XX,xxx             Napoli              Carnevale
        - Modifiche non documentate
01.00   1996/xxx/XX,xxx             Napoli              Carnevale
        - Creazione
*/

