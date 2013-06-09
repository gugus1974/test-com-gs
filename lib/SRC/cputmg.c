/********************************************************************/
/* 2004/nov/08,lun 08:30                \SWISV\LIB\SRC\cputmg.c     */
/********************************************************************/
/*      Utility per la misura del tempo di calcolo 16x e DSP        */
/********************************************************************/
/*
    $Date: 2004/11/08 08:54:28 $          $Revision: 1.3 $
    $Author: mungiguerrav $
*/



#include "libisv.h"

#include "consolle.h"

#include "cputmg.h"

volatile struct cputmgType cputmg;
volatile struct dsptmgType tmgD;

static struct
{
    float        tick;          /* Valore del tick del timer espresso in microsecondi   */
    unsigned short count;         /* Contatore numero di dati accumulati per medie        */

    long         AuxAcc;
    long         T6xAcc;
    long         DspAcc;
    long         CapAcc;
    long         SlwAcc;

    unsigned short AuxMin;
    unsigned short T6xMin;
    unsigned short DspMin;
    unsigned short CapMin;
    unsigned short SlwMin;

    unsigned short AuxMax;
    unsigned short T6xMax;
    unsigned short DspMax;
    unsigned short CapMax;
    unsigned short SlwMax;

    unsigned short AuxAvg;
    unsigned short T6xAvg;
    unsigned short DspAvg;
    unsigned short CapAvg;
    unsigned short SlwAvg;
} tmgL;

static struct
{
    unsigned short AuxMin;
    unsigned short T6xMin;
    unsigned short DspMin;
    unsigned short CapMin;
    unsigned short SlwMin;

    unsigned short AuxMax;
    unsigned short T6xMax;
    unsigned short DspMax;
    unsigned short CapMax;
    unsigned short SlwMax;

    unsigned short AuxAvg;
    unsigned short T6xAvg;
    unsigned short DspAvg;
    unsigned short CapAvg;
    unsigned short SlwAvg;
} tmgP;

/* prototipi funzioni locali */
static void resetTiming(void);
static void gest_timing(short argc, char* argv[]);

void cputmg_init(float tick)
{
    consolle_addcmd("timing","","Misura i timing dell'applicazione", gest_timing);

    tmgL.tick   = tick*1.0e6;
    tmgL.count  = 0;

    cputmg.TAux = 0;
    cputmg.T16x = 0;
    cputmg.TDsp = 0;
    cputmg.TCap = 0;
    cputmg.TSlw = 0;

    resetTiming();
}

void cputmg_avg(void)
{
    unsigned short AUX;
    unsigned short T6x;
    unsigned short DSP;
    unsigned short CAP;
    unsigned short SLW;

    tmgL.count++;

    /*------------ campionamento 166 -------------*/
    AUX  = cputmg.TAux;
    T6x  = cputmg.T16x;
    DSP  = cputmg.TDsp;
    CAP  = cputmg.TCap;
    SLW  = cputmg.TSlw;

    /*------ accumula per valor medio 166 --------*/
    tmgL.AuxAcc += AUX;
    tmgL.T6xAcc += T6x;
    tmgL.DspAcc += DSP;
    tmgL.CapAcc += CAP;
    tmgL.SlwAcc += SLW;

    /*-------- aggiorna max e min 166 ------------*/
    if(AUX > tmgL.AuxMax)       tmgL.AuxMax = AUX;
    if(AUX < tmgL.AuxMin)       tmgL.AuxMin = AUX;

    if(T6x > tmgL.T6xMax)       tmgL.T6xMax = T6x;
    if(T6x < tmgL.T6xMin)       tmgL.T6xMin = T6x;

    if(DSP > tmgL.DspMax)       tmgL.DspMax = DSP;
    if(DSP < tmgL.DspMin)       tmgL.DspMin = DSP;

    if(CAP > tmgL.CapMax)       tmgL.CapMax = CAP;
    if(CAP < tmgL.CapMin)       tmgL.CapMin = CAP;

    if(SLW > tmgL.SlwMax)       tmgL.SlwMax = SLW;
    if(SLW < tmgL.SlwMin)       tmgL.SlwMin = SLW;

    /* Controlla se la finestra di calcolo timing e' terminata */
    if (tmgL.count >= 1024)
    {
        /* calcola i valori average */
        tmgL.AuxAvg = (unsigned short)(tmgL.AuxAcc>>10);
        tmgL.T6xAvg = (unsigned short)(tmgL.T6xAcc>>10);
        tmgL.DspAvg = (unsigned short)(tmgL.DspAcc>>10);
        tmgL.CapAvg = (unsigned short)(tmgL.CapAcc>>10);
        tmgL.SlwAvg = (unsigned short)(tmgL.SlwAcc>>10);

        tmgL.count  = 0;

        tmgP.AuxMin = tmgL.AuxMin;
        tmgP.T6xMin = tmgL.T6xMin;
        tmgP.DspMin = tmgL.DspMin;
        tmgP.CapMin = tmgL.CapMin;
        tmgP.SlwMin = tmgL.SlwMin;

        tmgP.AuxMax = tmgL.AuxMax;
        tmgP.T6xMax = tmgL.T6xMax;
        tmgP.DspMax = tmgL.DspMax;
        tmgP.CapMax = tmgL.CapMax;
        tmgP.SlwMax = tmgL.SlwMax;

        tmgP.AuxAvg = tmgL.AuxAvg;
        tmgP.T6xAvg = tmgL.T6xAvg;
        tmgP.DspAvg = tmgL.DspAvg;
        tmgP.CapAvg = tmgL.CapAvg;
        tmgP.SlwAvg = tmgL.SlwAvg;

        resetTiming();
    }
}

static void resetTiming(void)
{
    tmgL.AuxAcc  = 0;
    tmgL.T6xAcc  = 0;
    tmgL.DspAcc  = 0;
    tmgL.CapAcc  = 0;
    tmgL.SlwAcc  = 0;

    tmgL.AuxMin  = 0xFFFF;
    tmgL.T6xMin  = 0xFFFF;
    tmgL.DspMin  = 0xFFFF;
    tmgL.CapMin  = 0xFFFF;
    tmgL.SlwMin  = 0xFFFF;

    tmgL.AuxMax  = 0;
    tmgL.T6xMax  = 0;
    tmgL.DspMax  = 0;
    tmgL.CapMax  = 0;
    tmgL.SlwMax  = 0;
}

/***************************/
/* gestione comando timing */
/***************************/
static void gest_timing(short argc, char* argv[])
{
    argc = argc;
    argv = argv;

    consolle_print("    TASK         MIN      MAX      AVG\n");
    consolle_print("Task 16x    :   %4u     %4u     %4u (uS)\n",
            (Ushort) ((float)tmgP.T6xMin * tmgL.tick +.5),
            (Ushort) ((float)tmgP.T6xMax * tmgL.tick +.5),
            (Ushort) ((float)tmgP.T6xAvg * tmgL.tick +.5) );
    consolle_print("Task DSP    :   %4u     %4u     %4u (uS)\n",
            (Ushort) ((float)tmgP.DspMin * tmgL.tick +.5),
            (Ushort) ((float)tmgP.DspMax * tmgL.tick +.5),
            (Ushort) ((float)tmgP.DspAvg * tmgL.tick +.5) );
    consolle_print("Task CAPINT :   %4u     %4u     %4u (uS)\n",
            (Ushort) ((float)tmgP.CapMin * tmgL.tick +.5),
            (Ushort) ((float)tmgP.CapMax * tmgL.tick +.5),
            (Ushort) ((float)tmgP.CapAvg * tmgL.tick +.5) );
    consolle_print("Task SLOW   :   %4u     %4u     %4u (uS)\n",
            (Ushort) ((float)tmgP.SlwMin * tmgL.tick +.5),
            (Ushort) ((float)tmgP.SlwMax * tmgL.tick +.5),
            (Ushort) ((float)tmgP.SlwAvg * tmgL.tick +.5) );

    consolle_print("-------------------------------------------\n");
    consolle_print("    TOTALE     %5u    %5u    %5u\n",
     (Ushort) (((float)tmgP.T6xMin+(float)tmgP.DspMin+(float)tmgP.CapMin+(float)tmgP.SlwMin) * tmgL.tick +.5),
     (Ushort) (((float)tmgP.T6xMax+(float)tmgP.DspMax+(float)tmgP.CapMax+(float)tmgP.SlwMax) * tmgL.tick +.5),
     (Ushort) (((float)tmgP.T6xAvg+(float)tmgP.DspAvg+(float)tmgP.CapAvg+(float)tmgP.SlwAvg) * tmgL.tick +.5) );

    consolle_print("-------------------------------------------\n");
    consolle_print("Task AUX    :   %4u     %4u     %4u (uS)\n",
            (Ushort) ((float)tmgP.AuxMin*tmgL.tick +.5),
            (Ushort) ((float)tmgP.AuxMax*tmgL.tick +.5),
            (Ushort) ((float)tmgP.AuxAvg*tmgL.tick +.5) );
    consolle_print("-------------------------------------------\n\n");

    consolle_print("Task DSP 1  :   %4u     %4u     %4u (uS)\n",tmgD.Dsp1Min,tmgD.Dsp1Max,tmgD.Dsp1Avg);
    consolle_print("Task DSP 2  :   %4u     %4u     %4u (uS)\n",tmgD.Dsp2Min,tmgD.Dsp2Max,tmgD.Dsp2Avg);
    consolle_print("Task DSP 3  :   %4u     %4u     %4u (uS)\n",tmgD.Dsp3Min,tmgD.Dsp3Max,tmgD.Dsp3Avg);
    consolle_print("Task DSP 4  :   %4u     %4u     %4u (uS)\n",tmgD.Dsp4Min,tmgD.Dsp4Max,tmgD.Dsp4Avg);
    consolle_print("Task DSP 5  :   %4u     %4u     %4u (uS)\n",tmgD.Dsp5Min,tmgD.Dsp5Max,tmgD.Dsp5Avg);
    consolle_print("Task DSP 6  :   %4u     %4u     %4u (uS)\n",tmgD.Dsp6Min,tmgD.Dsp6Max,tmgD.Dsp6Avg);
    consolle_print("Task DSP 7  :   %4u     %4u     %4u (uS)\n",tmgD.Dsp7Min,tmgD.Dsp7Max,tmgD.Dsp7Avg);
    consolle_print("-------------------------------------------\n");
    consolle_print("    TOTALE     %5u    %5u    %5u\n",
        tmgD.Dsp1Min+tmgD.Dsp2Min+tmgD.Dsp3Min+tmgD.Dsp4Min+tmgD.Dsp5Min+tmgD.Dsp6Min+tmgD.Dsp7Min,
        tmgD.Dsp1Max+tmgD.Dsp2Max+tmgD.Dsp3Max+tmgD.Dsp4Max+tmgD.Dsp5Max+tmgD.Dsp6Max+tmgD.Dsp7Max,
        tmgD.Dsp1Avg+tmgD.Dsp2Avg+tmgD.Dsp3Avg+tmgD.Dsp4Avg+tmgD.Dsp5Avg+tmgD.Dsp6Avg+tmgD.Dsp7Avg );
}


/* ****************************************************************************
    $Log: cputmg.c,v $
    Revision 1.3  2004/11/08 08:54:28  mungiguerrav
    2004/nov/08,lun 08:30          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.2        2004/06/15 11:20:12     buscob
01.05   2003/nov/21,ven         Napoli                  Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso.

    Revision 1.1        2002/03/27 18:09:52     accardol
    - Inserimento in CVS della versione in lib07m
    Revision 1.1.1.1    2002/03/27 18:09:52     accardol
    - Inserimento in CVS in branch STP della versione in lib07m
01.04   2002/mar/27,mer         Napoli                  Accardo
        - Si agg. gestione  TASK SLOW   per misurare la durata della parte "lenta" di CAPINT.

 ------------------------   Prima di CVS    ------------------------
01.03   2001/nov/12,lun         Napoli                  Mungi
        - Si agg. la:   volatile struct dsptmgType tmgD;
        - Si abol. il campo Ulong far    *ptDpr;   di  struct  tmgL,
          in quanto ora l'accesso a dpr e' fatto da cpudrv_dsptmg()
        - Si aboliscono i campi DspXMin,Max,Avg  dalla struct tmgP
        - In cputmg_init() non si passa piu' l'indice della dpr
        - Si abolisce la cputmg_dprlet()
        - Nella stampa dei dati DSP si usa la struct pubblica tmgD dedicata
          al trasferimento da DSP, invece della struct privata tmgP di stampa
01.02   2001/nov/09,ven         Napoli                  Mungi
        - Si elim. include cpudrv.h
        - Si agg. struct tmgL, con i campi locali della vecchia struct timing
        - Si mod. nome campi:   tmgL.timerTick  in  .tick
                                tmgL.tmgdpr     in  .ptDpr
                                tmgL.tmgCount   in  .count
        - Si elim. gli accessi tramite macro dpr, sostituendoli con
          accesso tramite puntatore locale tmgL.ptDpr
        - Si elim. la macro TICK, sostituendola con il campo tmgL.tick
        - La  volatile struct pubblica cambia nome da timing a cputmg
01.01   2001/nov/09,ven         Napoli                  Mungi
        - Si allineano gli indici di dpr alla situazione di dsptmg
01.00   2001/nov/08,gio         Napoli                  Mungi\Busco
        - Cambia nome da timing.c  a  cputmg.c
        - Il timing è sempre in corso e si stampa su richiesta del comando
        - La gestione dei timing DSP e' spostata in dsptmg
----------------------fine storia timing.c-----------------------------
01.06   2001/giu/25,lun         Napoli                  Mungi
        - Si agg. include driver.h
01.05   2000/dic/11,lun         Napoli                  Busco
        - Si sost. dpr[..] con dpr(..)
01.04   2000/ott/30,lun         Napoli                  Accardo
        - Si sost. #include "libdan6.h" con #include "libisv.h"
01.03   1999/mag/17,lun             Napoli              Mungi
        - Si sost.  "\r\n"  con     "\n"
01.02   1998/feb/26,gio
        - Si agg. stampa TOTALE DSP
01.01   1998/feb/24,mar
        - Si agg. timing relativi a 7 pezzi su DSP
01.00   1997/nov/25,mar
        - Creazione
*/
