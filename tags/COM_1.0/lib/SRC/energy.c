/****************************************************************************************/
/* 2004/nov/05,ven 17:40                                        \SWISV\LIB\SRC\energy.c */
/*                                                                                      */
/* Module: ENERGY                                                                       */
/* Description: Calcolo energia assorbita e restituita                                  */
/*                                                                  IMPLEMENTATION FILE */
/****************************************************************************************/
/*
    $Date: 2004/11/05 16:54:59 $          $Revision: 1.2 $
    $Author: mungiguerrav $
*/


/****************************************************************************************/
/* Include files                                                                        */
/****************************************************************************************/
#include "energy.h"


/****************************************************************************************/
/* Static variables                                                                     */
/****************************************************************************************/
static unsigned long    kwh1;                       /* Rappresenta 1kwh                 */
static unsigned short   erCode;                     /* Codice di errore                 */


/****************************************************************************************/
/* Nome: energy_init                                                                    */
/*                                                                                      */
/* Descrizione:                                                                         */
/*      - Funzione di inizializzazione                                                  */
/*                                                                                      */
/* Variabili di ingresso:                                                               */
/*      unsigned short  tms         - Passo di campionamento in msec                    */
/*                                                                                      */
/* Variabili di uscita:                                                                 */
/*      Nessuna                                                                         */
/*                                                                                      */
/* Ritorno funzione:                                                                    */
/*      ENERGY_TCAMPOK   - Passo di campionamento valido                                */
/*      ENERGY_TCAMPFAIL - Passo di campionamento troppo piccolo                        */
/*                                                                                      */
/****************************************************************************************/
unsigned short energy_init(unsigned short tms)
{
    kwh1        = 0;
    erCode      = ENERGY_TCAMPFAIL;

    if (tms >= ENERGY_TMSMIN)
    {
        kwh1    = (unsigned long)(3600.0*1000.0/1e-3/(float)tms + 0.5);
        erCode  = ENERGY_TCAMPOK;
    }

    return(erCode);
}


/****************************************************************************************/
/* Nome: energy_calc                                                                    */
/*                                                                                      */
/* Descrizione:                                                                         */
/*      - Calcola energia assorbita e restituita.                                       */
/*      - La funzione deve essere chiamata a tempo fisso. Esegue i calcoli ed aggiorna  */
/*        le variabili di uscita solo ogni maxNumCicli chiamate.                        */
/*                                                                                      */
/* Variabili di ingresso:                                                               */
/*      long   pot      - Potenza in Watt                                               */
/*                                                                                      */
/* Variabili di uscita:                                                                 */
/*      Nessuna                                                                         */
/*                                                                                      */
/* Ritorno funzione:                                                                    */
/*      Nessuno                                                                         */
/****************************************************************************************/
void energy_calc(long pot, struct energy_counter *pt)
{
    if (erCode == ENERGY_TCAMPOK)
    {
        if (pot > 0)
        {
            pt->sumPotAss += (unsigned long)(+pot);

            while (pt->sumPotAss  >= kwh1)
            {
                pt->enerAss++;
                pt->sumPotAss -= kwh1;
            }
        }
        else if (pot < 0)
        {
            pt->sumPotRes += (unsigned long)(-pot);

            while (pt->sumPotRes  >= kwh1)
            {
                pt->enerRes++;
                pt->sumPotRes -= kwh1;
            }
        }
    }
}


/* ****************************************************************************
    $Log: energy.c,v $
    Revision 1.2  2004/11/05 16:54:59  mungiguerrav
    2004/nov/05,ven 17:40          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.1        2003/02/12 10:19:22     accardol
    - Inserimento in CVS della versione in lib07n
01.06   2003/feb/12,mer             Napoli              Accardo
        - Si elim. routine energy_calcAss()
        - Si elim. var. globali enerAss, enerRes.
        - Si elim. var. statiche sumPotAss, sumPotRes.
        - L'applicativo passa a energy_calc() il puntatore ad un struttura del tipo energy_counter
          contenente le variabili di energia e di accumulo della potenza:
                    struct energy_counter
                    {
                        unsigned short  enerAss;
                        unsigned short  enerRes;
                        unsigned long   sumPotAss;
                        unsigned long   sumPotRes;
                    };
          Si sost.      energy_calc(long pot)
          con           energy_calc(long pot, struct energy_counter *pt)
          L'applicativo, istanziando n strutture del tipo energy_counter puo' effettuare
          n calcoli indipendenti di energia.

 ------------------------   Prima di CVS    ------------------------
01.05   2003/feb/11,mar             Napoli              Accardo
        - Si agg. inizializzazione di sumPotAss, sumPotRes, kwh1 a 0.
01.04   2003/feb/10,lun             Napoli              Accardo
        - L'applicativo passa a energy_calc() ed energy_calcAss() la potenza invece di tensione e
          corrente. Si sost. pertanto:
                energy_calc   (short tens, short corr)     con     energy_calc   (long pot)
                energy_calcAss(short tens, short corr)     con     energy_calcAss(long pot)
01.03   2003/feb/10,lun             Napoli              Accardo
        - Si agg. la routine    energy_calcAss()  per il calcolo della sola energia assorbita.
01.02   2003/gen/27,lun             Napoli              Accardo
        - L'applicativo passa ad energy_init() i valori iniziali di energia assorbita
          e restituita:     enerAssIni, enerResIni.
01.01   2003/gen/24,ven             Napoli              Accardo
        - L'applicativo passa a energy_init() il tempo di calcolo tCalc.
        - energy_init() restituisce un codice di errore:
                ENERGY_TCALCFAIL    tempo di calcolo troppo piccolo (<  ENERGY_TCALCMIN)
                ENERGY_TCALCOK      tempo di calcolo valido         (>= ENERGY_TCALCMIN)
        - Se tCalc non e' valido la routine energy_calc() non aggiorna i valori di enerAss ed enerRes.
01.00   2003/gen/22,mer             Napoli              Accardo
        - Creazione
*/

