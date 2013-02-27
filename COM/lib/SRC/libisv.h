/****************************************************************/
/* 2004/nov/08,lun 11:25                \SWISV\LIB\SRC\libisv.h */
/****************************************************************/
/*
    $Date: 2004/11/08 11:19:24 $          $Revision: 1.3 $
    $Author: mungiguerrav $
*/


#ifndef _LIBISV_
#define _LIBISV_

typedef signed short        ABOOL;

typedef unsigned char       Uchar;
typedef unsigned long       Ulong;
typedef unsigned short      Ushort;

union LongShort
{
    long b32;
    struct
    {
        short l;
        short h;
    } b16;
};

union UlongUsh
{
    Ulong b32;
    struct
    {
        Ushort l;
        Ushort h;
    } b16;
};

union UshUch
{
    Ushort b16;
    struct
    {
        Uchar l;
        Uchar h;
    } b8;
};


#define VERO   1
#define FALSO  0

#endif


/* ****************************************************************************
    $Log: libisv.h,v $
    Revision 1.3  2004/11/08 11:19:24  mungiguerrav
    2004/nov/08,lun 11:25          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.2        2004/06/15 11:33:51     buscob
02.01   2003/nov/29,sab             Madrid              Mungi
        - Si corregge l'errata sostituzione di union LongInt  con union UlongUsh
          in quanto non si e' cambiato solo il nome, ma anche i tipi
          da signed ad unsigned:
            - Si introduce      union LongShort
            - Si lascia la      union UlongUsh      come nuovo tipo
02.00   2003/nov/21,ven             Napoli              Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso
        - Si modifica anche     union LongInt   in      UlongUsh

    Revision 1.1        2002/01/24 14:39:24     accardol
    - Inserimento in CVS della versione in lib07m
    Revision 1.1.1.1    2002/01/24 14:39:24     accardol
    - Inserimento in CVS in branch STP della versione in lib07m
01.20   2002/gen/24,gio             Napoli              Mungi\Accardo
        - Per gestire l'accesso ai campi 16/8H+8L bit del componente SSC interno
          al 167, ossia l'accesso sia come Ushort che come doppio Uchar,
          si agg. la union UshUch (unsigned short - unsigned ch),
          sull'esempio della storica union LongInt 32/16H+16L
                union UshUch
                {
                    Ushort b16;
                    struct
                    {
                        Uchar l;
                        Uchar h;
                    } b8;
                };

 ------------------------   Prima di CVS    ------------------------
01.19   2001/dic/14,ven             Napoli              Mungi
        - Dopo altri tentativi infruttuosi, si decide ad effettuare una
          modifica a forte impatto sui moduli di applicativo:
            - Per compatibilita' con il SW TCN ed il suo porting su S.O. RTX,
              si mod.:  typedef unsigned int    BOOLEAN
              in:       typedef signed short    ABOOL   (AnsaldoBreda BOOL)
        - Visto che il punto precedente richiede la modifica di molti file di
          libreria e applicativi, si decide di fare anche il passo di eliminare
          tutte le macro di interfaccia con le funzioni di libreria introdotte
          quando si volevano utilizzare i puntatori intermedi, avendo gia'
          eliminato il passaggio attraverso i puntatori indiretti.
          Le sostituzioni da effettuare negli applicativi sono:
                SET_ISR         cpudrv_setIsr
                SET_TIMER       cpudrv_setTimer
                START_TIMER     cpudrv_startTimer
                START_ADC16x    cpudrv_socAD16x
                WAIT_ADC16x     cpudrv_waitAD16x
                OUT_DAC         cpudrv_outDAc
                DSP_INT         cpudrv_dspInt
01.18   2001/dic/10,lun             Napoli              Mungi\Busco
        - In un tentativo di rendere compatibile la doppia definizione di tipo:
            \SWISV\LIB\SRC\libisv.h
                typedef unsigned int    BOOLEAN
          usata nel SW di origine Controlli Azionamento, e
            \KEIL403\C166\INC\rtx166.h
                typedef enum {FALSE, TRUE} BOOLEAN;
          usata nel SW TCN e nel suo porting su S.O. RTX realizzata da Carnevale,
          si cerca di proteggere la dichiarazione con:
                #ifndef _PI_RTX_
          ma i risultati sono scarsi in quanto dipendono dall'ordine
          con cui si eseguono gli include !!
01.17   2001/nov/08,gio             Napoli              Mungi\Busco
        - Si elim. macro SET_CAPCOM
01.16   2001/ott/26,ven             Napoli              Mungi\Busco
        - Si elim. puntatore a _sysReset()
01.15   2001/ott/22,lun             Napoli              Mungi\Busco
        - Si elim. macro e puntatori per ADC veloci
        - Si elim. extern puntatori a vme e dpr
01.14   2001/ott/19,ven             Napoli              Mungi\Busco
        - Si riporta la modifica di Mungi del 2001/giu/25,lun:
            - Si elim. tutte le linee che riguardano l'accesso alle schede
              (reali o simulate) spostate nei driver.h delle directory SRC
              o SIM delle singole schede
            - Si elim. define SIM 100 (diventato define ALL4 100 in DAU\SIM\driver)
01.13   2001/lug/24,mar             Napoli              Accardo
        - Si elim. extern char (*consIn)();
                   extern void (*consOut)(char c);
        - Si elim. #define CONS_in  (*consIn)
                   #define CONS_out (*consOut)
01.12   2001/lug/19,gio             Napoli              Accardo
        - Si sost.  #include "reg16x.h"
                    #include "bit16x.h"
                    #include "intnum.h"
          con un unico
                    #include "regdef.h"
01.11   2001/mag/30,mer             Napoli              Busco/Mungi
        - Si dichiara il puntatore a sysReset() come far:
          da  void (*sysReset)()        a  void (*far sysReset)()
01.10   2001/mag/29,mar             Napoli              Busco/Mungi
        - Si agg. puntatore a _sysReset
01.09   2001/mag/10,gio             Napoli              Accardo
        - Si mod. vme da far a huge
01.08   2001/mag/03,gio             Napoli              Accardo
        - Si sost. dprBaseAddr con dpr.
        - vme e dpr diventano unsigned int*
        - Si elim. _setlevel() e _resetlevel()
        - Si eliminano puntatori agli AD veloci, ai DAC ed ai buffer Lin ed Lout.
        - Si rinomina HoldDSP    in holdDSP
        - Si rinomina ReleaseDSP in releaseDSP
        - Si rinomina ReadADC    in readADvel
        - Si ridefinisce vme(indir)=vme[indir/2]
        - In simulazione dpr(indir)=dpr[indir]
01.07   2001/feb/28,mer             Napoli              Accardo
        - vme diventa Ushort far*
01.06   2000/dic/11,lun             Napoli              Busco
        - Si rinomina puntatore dpr in DprBaseAddr
        - Si definisce macro dpr()
01.05   2000/dic/07,gio             Napoli              Busco
        - Si agg. prototipo ReadADC()
01.04   2000/dic/04,lun             Napoli              Busco
        - Si aggiungono macro per l'accesso diretto alla RAM del DSP su scheda PCA
        - Si agg. definizione funzioni di interfaccia HoldDSP e ReleaseDSP
        - Si agg. extern variabili per gestire le stringhe di versione
01.03   2000/ott/30,lun             Napoli              Accardo
        - Si separa definizione di pectype tra scheda reale e simulazione.
01.02   2000/ott/11,mer             Napoli              Accardo
        - Si elim. in simulazione:
                #include "reg16s.h"
                #include "bit16s.h"
        - Si agg. in simulazione:
                #include "SWISV\LIB\A\SIM\dan.h"
01.01   2000/ott/04,mer             Napoli              Accardo
        - Si ripristinano vecchi puntatori a risorse hw veloci.
        - Si agg. in simulazione:
                #include "reg16s.h"
                #include "bit16s.h"
        - Si agg. per scheda reale:
                #include "reg16x.h"
                #include "bit16x.h"
01.00   2000/set/05,mar             Napoli              Accardo
        - Si estrae da libdan6.h 3.07 la parte non legata alla scheda.
        - Si agg. macro LSC_ISR(N_TRAP,NOME_FUN,NOME_REG) per la definizione
          delle ISR e si eliminano tutte le altre.
        - Si ridefiniscono i puntatori alle periferiche hw veloci
          (AD converter, DA converter, latch in , latch out).
        - Si introducono puntatori a dpr e vme.
        - Si elimina definizione di P5_RD e P5_WR.
        - Si introducono macro e puntatori a funzioni di libreria.
        - Si unifica la definizione di pectype:
                typedef unsigned int pectype
--------------------Storia precedente di libda6.h----------------------
03.07   1998/giu/25,gio             Napoli              Mungi
        - Si agg. macro LD6_ISRLVL per assegnare lo stesso banco a ISR dello stesso Livello
*/

