/****************************************************************/
/* 2004/nov/05,ven 14:15                \SWISV\LIB\SRC\trcbk.h  */
/****************************************************************/
/*      Header file per il gestore di trace e brk               */
/****************************************************************/
/*
    $Date: 2004/11/05 13:24:55 $          $Revision: 1.3 $
    $Author: mungiguerrav $
*/


#define BRKTRACE        1
#define BRKPROGRAMMA    2

void    trcbk_init(   void );
void    trcbk_print(  void );
short   trcbk_test(   void );
void    trcbk_sample( void );


/* ****************************************************************************
    $Log: trcbk.h,v $
    Revision 1.3  2004/11/05 13:24:55  mungiguerrav
    2004/nov/05,ven 14:15          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.2        2004/06/15 12:03:38     buscob
01.01   2003/nov/21,ven             Napoli              Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso

    Revision 1.1      2000/02/01 15:04:00   accardol
    - Inserimento in CVS della versione in lib07m
    Revision 1.1.1.1  2000/02/01 15:04:00   accardol
    - Inserimento in CVS in branch STP della versione in lib07m
        (la data è arretrata con un artificio)
01.00   2000/feb/01,mar             Napoli              Mungi
        - Creazione a partire da consolle.h v.2.01 del 06.dic.1999
          estraendo i pezzi relativi a trace e brk
    /----------------storia di consolle.h relativa a questi comandi-------------\
    |   1.04 18.lug.96  - Si agg. define BRKTRACE  e  BRKPROGRAMMA              |
    \---------------------------------------------------------------------------/
*/

