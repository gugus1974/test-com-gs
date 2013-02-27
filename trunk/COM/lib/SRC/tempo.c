/****************************************************************/
/* 2004/nov/05,ven 15:10                \SWISV\LIB\SRC\tempo.c  */
/****************************************************************/
/*      Utility per la gestione del tempo trascorso             */
/****************************************************************/
/*
    $Date: 2004/11/05 14:30:26 $          $Revision: 1.3 $
    $Author: mungiguerrav $
*/


#include "tempo.h"

struct typeTempo tempo;

void tempo_init(    unsigned char   incremento,
                    unsigned char   ora0,
                    unsigned char   min0,
                    unsigned char   sec0,
                    unsigned short  timer
               )
{
    tempo.tot   = (unsigned short)( ( (unsigned long)(min0*60+sec0)*100 ) % 60000L );
    tempo.cen   = 0;
    tempo.sec   = sec0;
    tempo.min   = min0;
    tempo.ora   = ora0;
    tempo.incr  = incremento;
    tempo.timer = timer;
}

void tempo_incr()
{
    tempo.tot   += tempo.incr;
    if (tempo.tot >= 60000U)
    {
        tempo.tot -=60000U;
    }

    tempo.cen   += tempo.incr;
    if (tempo.cen >= 100)
    {
        tempo.cen   -= 100;

        tempo.sec++;
        if (tempo.sec>=60)
        {
            tempo.sec   = 0;

            tempo.min++;
            if (tempo.min>=60)
            {
                tempo.min   = 0;

                tempo.ora++;
                if (!tempo.timer && (tempo.ora>=24) )
                {
                    tempo.ora   = 0;
                }
            }
        }
    }
}


/* ****************************************************************************
    $Log: tempo.c,v $
    Revision 1.3  2004/11/05 14:30:26  mungiguerrav
    2004/nov/05,ven 15:10          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.2            2004/06/15 11:59:04     buscob
01.04   2003/nov/30,dom             Madrid              Mungi
        - Si introduce la gestione del flag timer, per stabilire se trascorsa l'ora 24
          si debba riazzerare il campo tempo.ora
        - Si trasforma la seconda costante 60000 in 60000U, per un warning del BorlandC 3.0

    Revision 1.1            2001/11/23 16:06:14     accardol
    - Inserimento in CVS della versione in lib07m
    Revision 1.1.1.1        2001/11/23 16:06:14     accardol
    - Inserimento in CVS in branch STP della versione in lib07m
        (la data è arretrata con un artificio)
01.03   2001/nov/23,ven             Napoli              Mungi
        - Si trasforma la costante 60000 in 60000L, per un warning del Borland,
          ma non sono convinto che sia nececessario
 ------------------------   Prima di CVS    ------------------------
01.02   2000/mag/24,mer             Napoli              Mungi
        - Si agg. la possibilita' di inizializzare i campi ora, min, sec
01.01   1998/feb/21,sab
        - Si mod. tempo.tot da Ulong fino a 360000 = 60 min
                            in Uint  fino a  60000 = 10 min
          perche' consolle non sa ancora gestire addvar long
01.00   1998/feb/21,sab
        - Creazione con gestione tempo.incr definito in inizializz.
          e Ulong tempo.tot fino a 360000 = 1 ora
*/

