/********************************************************************/
/* 2004/nov/15,lun 10:10                \SWISV\LIB\SRC\spartan.h    */
/********************************************************************/
/*
    $Date: 2004/11/16 15:20:05 $          $Revision: 1.3 $
    $Author: mungiguerrav $
*/


#ifndef _SPARTAN_
#define _SPARTAN_

typedef void  (*tSetPinFunc )  (short value);
typedef short (*tGetPinFunc )  (void);

short   spartan_load(   unsigned char huge  *p,
                        unsigned short      numbyte);

void    spartan_init(   tSetPinFunc         SetDINFunc,
                        tSetPinFunc         SetCCLKFunc,
                        tSetPinFunc         SetPROGFunc,
                        tGetPinFunc         GetDONEFunc,
                        tGetPinFunc         GetINITFunc );

#endif


/* ****************************************************************************
    $Log: spartan.h,v $
    Revision 1.3  2004/11/16 15:20:05  mungiguerrav
    2004/nov/15,lun 10:10          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita
    - Si migliora incolonnamento


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.2        2004/06/15 11:53:21     buscob
02.03   2003/nov/21,ven             Napoli              Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso.

    Revision 1.1        2001/12/21 13:19:32     accardol
    - Inserimento in CVS della versione in lib07m
    Revision 1.1.1.1    2001/12/21 13:19:32     accardol
    - Inserimento in CVS in branch STP della versione in lib07m
        La data è arretrata con un artificio
02.02   2001/dic/21,ven             Napoli              Mungi
        - Si agg. la protezione da inclusione multipla  ifndef _SPARTAN_

 ------------------------   Prima di CVS    ------------------------
02.01   2001/gen/26,ven             Napoli              Busco/Accardo
        - spartan_load(): si dichiara huge il puntatore *p
02.00   2000/dic/06,mer             Napoli              Busco
        - Si agg. typedef per le funzioni di pilotaggio pin
        - Si modifica fpga_init(...)
        - Si rinomina in startan.h
01.03   2000/dic/05,mar             Napoli              Busco
        - Si elimina include libpca7.h
01.02   2000/giu/05,lun             Napoli              Mungi
        - Si incl. libpca7.h invece di libsta7.h
01.01   1999/xxx/XX,xxx             Napoli              Busco
        - Modifiche non documentate
01.00   1999/apr/19,lun             Napoli              Busco
        - Creazione a partire da ETR 500P con nome fpgaload.h
*/

