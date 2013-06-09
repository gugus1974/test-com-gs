/********************************************************************/
/* 2004/nov/05,ven 14:40                \SWISV\LIB\SRC\timeDate.c   */
/********************************************************************/
/*      Codifica e decodifica dello standard TCN timedate48         */
/********************************************************************/
/*
    $Date: 2004/11/05 13:57:55 $          $Revision: 1.3 $
    $Author: mungiguerrav $
*/


#include "timedate.h"

const unsigned short days[]={31,28,31,30,31,30,31,31,30,31,30,31};

void timedate_cod(date *dateIn, time *timeIn, timedate48 *timedateOut)
{
    unsigned long   anno = 1900;
    unsigned short  index;
    unsigned long   allsec;     /* tutti i secondi trascorsi dal 1/1/1970   */

    anno        += dateIn->anno;
    if (anno < 1996)
    {
        anno    = anno + 100;
    }

    /* alla mezzanotte del 31/12/ dell'anno "scorso" a Greenwich            */
    /* dal giorno 1/1/70 ,se dopo 2000 correzione *100!                     */
    allsec      = (anno-1970)*365;                      /* giorni               */

    /* correzione : num_anni_bisestili_trascorsi                            */
    for (index=1970; index<anno; index++)           /* aggiunge giorni      */
    {
        allsec      += (index%4==0 && index%100!=0 ||index%400==0);
    }

    /* aggiunta dei giorni trascorsi da capodanno alla mezzanotte di ieri   */
    for (index=1;index < dateIn->mese;index++)
    {
        allsec      += days[index-1];
    }

    /* correzione se anno corrente bisestile ed e'                          */
    /* trascorso il 29 febbraio                                             */
    if ( ( (anno%4==0 && anno%100!=0) || anno%400==0) && dateIn->mese>2)
    {
        allsec      += 1;
    }

    allsec      += (dateIn->giorno-1);

    allsec      = allsec*24L*3600L;     /* trasforma giorni in secondi          */

    /* aggiunge secondi trascorsi a Greenwich fino all'ora corrente         */
    /* (ora-1 poiche' siamo a "Roma" )                                      */
    allsec      += timeIn->ora*3600L+timeIn->min*60+timeIn->sec;
    allsec      = allsec - 3600;

    timedateOut->sec    = allsec;
    timedateOut->ticks  = 0;    /* puo' anche essere calibrato (valore basso)*/
}


void timedate_decod(timedate48 *timedateIn, date *dateOut, time *timeOut)
{
    unsigned short  locdays[12];
    unsigned long   allsec, anno;
    unsigned long   giorniTot, giorniCap;

    unsigned char   annoStimato;
    unsigned char   mese, ora, min, sec;
    unsigned short  index;

    unsigned char   isBis   = FALSE;

    giorniTot   = (timedateIn->sec+3600) / (24*3600L);

    allsec      = timedateIn->sec -giorniTot* (24*3600L)+3600;

    annoStimato = (unsigned char)(giorniTot/365+70);
    anno        = 1900 + annoStimato;

    giorniCap   = (anno-1970)*365;

    /* correzione : num_anni_bisestili_trascorsi                            */
    for (index=1970; index<anno; index++)            /* aggiunge giorni     */
    {
        giorniCap   += (index%4==0 && index%100!=0 ||index%400==0);
    }

    /* correzione errore di stima                                           */
    if (giorniCap > giorniTot)
    {
       annoStimato--;
       anno         = 1900 + annoStimato;
    }

    if (anno%4==0 && anno%100!=0 || anno%400==0)
    {
        isBis       = TRUE;
    }

    giorniCap   = (anno-1970)*365;

    /* correzione : num_anni_bisestili_trascorsi                            */
    for (index=1970; index<anno; index++)           /* aggiunge giorni      */
    {
       giorniCap += (index%4==0 && index%100!=0 ||index%400==0);
    }

    mese        = 0;
    giorniTot   -= giorniCap;

    for (index=0; index<12; index++)
    {
        locdays[index]  = days[index];
    }

    if (isBis==TRUE)
    {
        locdays[1]++;
    }

    while (giorniTot >= locdays[mese])
    {
        giorniTot   = giorniTot- locdays[mese];
        mese++;
    }
    mese++;

    ora         = (unsigned char)( allsec / 3600L);
    min         = (unsigned char)((allsec-ora*3600L) / 60L);
    sec         = (unsigned char)( allsec-ora*3600L-min*60L);

    dateOut->giorno = (unsigned char)(giorniTot+1);
    dateOut->mese   = mese;
    dateOut->anno   = annoStimato % 100;

    timeOut->ora    = ora;
    timeOut->min    = min;
    timeOut->sec    = sec;
}


/* ****************************************************************************
    $Log: timeDate.c,v $
    Revision 1.3  2004/11/05 13:57:55  mungiguerrav
    2004/nov/05,ven 14:40          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.2            2004/08/31 14:25:15     mungiguerrav
    2004/ago/31,mar 16:25        Napoli          Mungi\Busco
    - Si effettua Merge da branch makefile

    Revision 1.1.1.1.2.1    2004/07/14 15:33:43     buscob
    - Inserimento su branch makefile di version9i esterne
01.04   2004/apr/07,mer             Napoli              Mungi
        - Si applicano le regole su nomi, parentesi e incolonnamenti
01.03   2004/gen/21,mer             Napoli              Calabrese/Porzio
        - Si corregge algoritmo di timedate_cod, errore sull'inizio anni bisestili

    Revision 1.1        2000/05/30 09:42:22     accardol
    - Inserimento in CVS della versione in lib07n
    Revision 1.1.1.1    2000/05/30 09:42:22     accardol
    - Inserimento in CVS in branch STP della versione in lib07n
        (la data è arretrata con un artificio)
01.02   2000/mag/30,mar             Napoli              Mungi
        - Si elim. stampe di debug
        - Si incolonna secondo regole SWISV

 ------------------------   Prima di CVS    ------------------------
01.01   2000/gen/24,lun             ???                 Schiavo
        - Creazione del file di tipo modulo \SWISV\LIB
01.00   1998/ott/XX,yyy             Napoli              Ausiello
        - Creazione algoritmo in libfunc.c
*/

