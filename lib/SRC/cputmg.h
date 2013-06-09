/****************************************************************/
/* 2004/nov/08,lun 08:40                \SWISV\LIB\SRC\cputmg.h */
/****************************************************************/
/*      Utility per la misura del tempo di calcolo 16x          */
/****************************************************************/
/*
    $Date: 2004/11/08 08:54:53 $          $Revision: 1.3 $
    $Author: mungiguerrav $
*/


#ifndef CPUTMG_INCLUSO
#define CPUTMG_INCLUSO

void cputmg_init(float tick);
void cputmg_avg(void);

struct cputmgType
{
    unsigned short startAux;
    unsigned short start16x;
    unsigned short TAux;
    unsigned short T16x;
    unsigned short TDsp;
    unsigned short TCap;
    unsigned short TSlw;
};

struct dsptmgType
{
    unsigned short Dsp1Min;
    unsigned short Dsp2Min;
    unsigned short Dsp3Min;
    unsigned short Dsp4Min;
    unsigned short Dsp5Min;
    unsigned short Dsp6Min;
    unsigned short Dsp7Min;

    unsigned short Dsp1Max;
    unsigned short Dsp2Max;
    unsigned short Dsp3Max;
    unsigned short Dsp4Max;
    unsigned short Dsp5Max;
    unsigned short Dsp6Max;
    unsigned short Dsp7Max;

    unsigned short Dsp1Avg;
    unsigned short Dsp2Avg;
    unsigned short Dsp3Avg;
    unsigned short Dsp4Avg;
    unsigned short Dsp5Avg;
    unsigned short Dsp6Avg;
    unsigned short Dsp7Avg;
};

extern volatile struct cputmgType cputmg;
extern volatile struct dsptmgType tmgD;

#define CPUTMG_16x_START(T)     cputmg.start16x = T;
#define CPUTMG_DSP_START(T)     cputmg.T16x     = T - cputmg.start16x;
#define CPUTMG_CAP_START(T)     cputmg.TDsp     = T - cputmg.start16x - cputmg.T16x;
#define CPUTMG_SLW_START(T)     cputmg.TCap     = T - cputmg.start16x - cputmg.T16x - cputmg.TDsp;
#define CPUTMG_SLW_STOP(T)      cputmg.TSlw     = T - cputmg.start16x - cputmg.T16x - cputmg.TDsp - cputmg.TCap;

#define CPUTMG_AUX_START(T)     cputmg.startAux = T;
#define CPUTMG_AUX_STOP(T)      cputmg.TAux     = T - cputmg.startAux;

#endif


/* ****************************************************************************
    $Log: cputmg.h,v $
    Revision 1.3  2004/11/08 08:54:53  mungiguerrav
    2004/nov/08,lun 08:40          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.2        2004/06/15 11:20:465    buscob
01.04   2003.nov.21,ven             Napoli              Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso.

    Revision 1.1        2002/03/27 18:14:50     accardol
    - Inserimento in CVS della versione in lib07m
    Revision 1.1.1.1    2002/03/27 18:14:50     accardol
    - Inserimento in CVS in branch STP della versione in lib07m
01.03   2002.mar.27,mer             Napoli              Accardo
        - Si agg campo  cputmgType.TSlw
        - Si rinom.     CPUTMG_CAP_STOP(T)     in      CPUTMG_SLW_START(T)
        - Si agg.       define CPUTMG_SLW_STOP(T) :
            cputmg.TSlw     = T - cputmg.start16x - cputmg.T16x - cputmg.TDsp - cputmg.TCap;

 ------------------------   Prima di CVS    ------------------------
01.02   2001.nov.12,lun             Napoli              Mungi
        - Si elim. parametro: unsigned short far *tmgdpr  in cputmg_init()
        - Si elim. prototipo cputmg_dprlet()
        - Si agg. struct dsptmgType con i campi DspXMin,Max,Avg  che erano
          nella static struct tmgP
        - Si agg. extern volatile struct dsptmgType tmgD;
01.01   2001.nov.09,ven             Napoli              Mungi
        - Si riducono i campi della struct cputmgType: si lasciano solo
          i campi della struttura usati dalle macro
01.00   2001/nov/08,gio             Napoli              Mungi\Busco
        - Cambia nome da timing.h  a  cputmg.h
        - Si mod. la struct timingType in cputmgType, aggiungendo
          i campi:  timerTick; tmgCount; tmgdpr;
                    AUXacc; T66acc; DSPacc; CAPacc;
        - Si mod. nomi macro da TMING_XXXX   a    CPUTMG_XXXX
        - Si agg. i prototipi void cputmg_avg(void);
                              void cputmg_dprlet(void);
*/

