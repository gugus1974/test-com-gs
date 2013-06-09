/********************************************************************/
/* 2004/nov/05,ven 14:50                \SWISV\LIB\SRC\timeDate.h   */
/********************************************************************/
/*
    $Date: 2004/11/05 13:58:09 $          $Revision: 1.3 $
    $Author: mungiguerrav $
*/


#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

#ifndef _TIMEDATE_
#define _TIMEDATE_

typedef struct                      /* per l'interfacciamneto con il rtc    */
{
    unsigned char       ora ;
    unsigned char       min;
    unsigned char       sec;
} time;

typedef struct                      /* per l'interfacciamento con il rtc    */
{
    unsigned char       giorno;
    unsigned char       mese;
    unsigned char       anno;
} date;


typedef struct
{
     unsigned long      sec;        /* UCT   time_date                      */
     unsigned short     ticks;      /* 65535 ticks per sec                  */
} timedate48;


void timedate_cod(      date            *dateIn,
                        time            *timeIn,
                        timedate48      *timedateOut);

void timedate_decod(    timedate48      *timedateIn,
                        date            *dateOut,
                        time            *timeOut);

#endif


/* ****************************************************************************
    $Log: timeDate.h,v $
    Revision 1.3  2004/11/05 13:58:09  mungiguerrav
    2004/nov/05,ven 14:50          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.2            2004/08/31 14:25:15     mungiguerrav
    2004/ago/31,mar 16:25        Napoli          Mungi\Busco
    - Si effettua Merge da branch makefile

    Revision 1.1.1.1.2.1    2004/07/14 15:34:14     buscob
    - Inserimento su branch makefile di version9i esterne
01.03   2004/apr/07,mer             Napoli              Mungi
        - Si applicano le regole su nomi, parentesi e incolonnamenti

    Revision 1.1        2000/05/30 09:42:08     accardol
    - Inserimento in CVS della versione in lib07n
    Revision 1.1.1.1    2000/05/30 09:42:08     accardol
    - Inserimento in CVS in branch STP della versione in lib07n
        (la data è arretrata con un artificio)
01.02   2000/mag/30,mar             Napoli              Mungi
        - Si incolonna secondo regole SWISV

 ------------------------   Prima di CVS    ------------------------
01.01   2000/gen/24,lun             ???                 Schiavo
        - Creazione del file di tipo modulo \SWISV\LIB
01.00   1998/ott/XX,yyy             Napoli              Ausiello
        - Creazione algoritmo in libfunc.h (immagino, MNG)
*/

