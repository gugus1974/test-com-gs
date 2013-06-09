/********************************************************************/
/* 2004/nov/30,mar 15:30                \SWISV\LIB\SRC\dp8570a.c    */
/********************************************************************/
/*
    $Date: 2004/12/13 11:49:20 $          $Revision: 1.3 $
    $Author: mungiguerrav $
*/


#include "dp8570a.h"

/*==================================================================*/
/* Macros */
#ifdef __C166__
    #define XHUGE xhuge
#else
    #define XHUGE huge
#endif


/* Definizione mappa dei registri (spiazzati per indizzamento a 16 bit) */
#define DP8570A_MSR         0x00
#define DP8570A_RTMR        0x01
#define DP8570A_IRR         0x04

#define DP8570A_CENSECS     0x05
#define DP8570A_SECONDS     0x06
#define DP8570A_MINUTES     0x07
#define DP8570A_HOURS       0x08
#define DP8570A_DAY         0x09
#define DP8570A_MONTHS      0x0A
#define DP8570A_YEARS       0x0B

#define DP8570A_SECONDS_TS  0x19
#define DP8570A_MINUTES_TS  0x1A
#define DP8570A_HOURS_TS    0x1B
#define DP8570A_DAY_TS      0x1C
#define DP8570A_MONTHS_TS   0x1D

/* Bit di selezione mappe di memoria */
#define MSR_SEL_RAM         0x80
#define MSR_SEL_REG0        0x00
#define MSR_SEL_REG1        0x40

#define RTMR_START          0x08

#define IRR_LOWBAT          0x40
#define IRR_TS_ENABLE       0x80


#define dp8570a(base, offset) *(((volatile unsigned short XHUGE *)(base)) + (offset))

/****************************************************************/
/* Prototipi delle funzioni statiche di modulo                  */
/****************************************************************/
static unsigned char  bcd2bin(unsigned char valbcd);
static unsigned char  bin2bcd(unsigned char valbin);
static void           LeapBitsWrite(void* base, short Bits);

short dp8570a_start(void *base)
{
    short temp;

    /* Alza il bit di start senza alterare gli atri bit del registro */
    dp8570a(base, DP8570A_MSR)  = MSR_SEL_REG1;
    temp                        = dp8570a(base, DP8570A_RTMR);
    temp = temp | RTMR_START;
    dp8570a(base, DP8570A_RTMR) = temp;
    return(1);
}

short dp8570a_stop(void *base)
{
    short temp;

    /* Abbassa il bit di start senza alterare gli altri bit del registro */
    dp8570a(base, DP8570A_MSR)  = MSR_SEL_REG1;
    temp                        = dp8570a(base, DP8570A_RTMR);
    temp = temp & ~RTMR_START;
    dp8570a(base, DP8570A_RTMR) = temp;
    return(1);
}

short dp8570a_startRead(void *base)
{
    short temp;

    dp8570a(base, DP8570A_MSR)  = MSR_SEL_REG1;
    temp                        = dp8570a(base, DP8570A_RTMR);

    return((temp & RTMR_START) != 0);
}

short dp8570a_clkRead(void *base, p_dp8570a_date_time time)
{
    dp8570a_tsDisable(base);    /* Blocca l'aggiornamento dei registri SAVE */

    dp8570a(base, DP8570A_MSR)  = MSR_SEL_REG0;

    time->anno    = bcd2bin(dp8570a(base, DP8570A_YEARS));
    time->mese    = bcd2bin(dp8570a(base, DP8570A_MONTHS_TS));
    time->giorno  = bcd2bin(dp8570a(base, DP8570A_DAY_TS));
    time->ora     = bcd2bin(dp8570a(base, DP8570A_HOURS_TS));
    time->minuti  = bcd2bin(dp8570a(base, DP8570A_MINUTES_TS));
    time->secondi = bcd2bin(dp8570a(base, DP8570A_SECONDS_TS));

    dp8570a_tsEnable(base);     /* Riabilita l'aggiornamento dei registri SAVE */
    return(1);
}

short dp8570a_clkWrite(void *base, p_dp8570a_date_time time)
{
    dp8570a_stop(base);

    dp8570a(base, DP8570A_MSR)          = MSR_SEL_REG0;
    dp8570a(base, DP8570A_YEARS)        = bin2bcd(time->anno);
    dp8570a(base, DP8570A_MONTHS)       = bin2bcd(time->mese);
    dp8570a(base, DP8570A_DAY)          = bin2bcd(time->giorno);
    dp8570a(base, DP8570A_HOURS)        = bin2bcd(time->ora);
    dp8570a(base, DP8570A_MINUTES)      = bin2bcd(time->minuti);
    dp8570a(base, DP8570A_SECONDS)      = bin2bcd(time->secondi);
    dp8570a(base, DP8570A_CENSECS)      = 0;

    dp8570a(base, DP8570A_MONTHS_TS)    = 0;
    dp8570a(base, DP8570A_DAY_TS)       = 0;
    dp8570a(base, DP8570A_MINUTES_TS)   = 0;
    dp8570a(base, DP8570A_SECONDS_TS)   = 0;

    /* Posiziona i bit di anno bisestile */
    LeapBitsWrite(base, ((time->anno) % 4));

    dp8570a_start(base);
    return(1);
}

short dp8570a_memRead(void *base, short offset, short *data)
{
    if (offset > 0 && offset < 0x20)
    {
        dp8570a(base, DP8570A_MSR)  = MSR_SEL_RAM;
        *data                       = dp8570a(base, offset);
    }
    return(1);
}

short dp8570a_memWrite(void *base, short offset, short data)
{
    if (offset > 0 && offset < 0x20)
    {
        dp8570a(base, DP8570A_MSR)  = MSR_SEL_RAM;
        dp8570a(base, offset)       = data;
    }
    return(1);
}

short dp8570a_lowbat(void *base)
{
    short temp;

    dp8570a(base, DP8570A_MSR)  = MSR_SEL_REG0;
    temp = dp8570a(base, DP8570A_IRR);

    return((temp & IRR_LOWBAT) != 0);
}

short dp8570a_tsEnable(void *base)
{
    dp8570a(base, DP8570A_MSR)  = MSR_SEL_REG0;
    dp8570a(base, DP8570A_IRR)  = IRR_TS_ENABLE;
    return(1);
}

short dp8570a_tsDisable(void *base)
{
    dp8570a(base, DP8570A_MSR)  = MSR_SEL_REG0;
    dp8570a(base, DP8570A_IRR)  = 0;
    return(1);
}

short dp8570a_tsRead(void *base)
{
    short temp;

    dp8570a(base, DP8570A_MSR)  = MSR_SEL_REG0;
    temp                        = dp8570a(base, DP8570A_IRR);

    return((temp & IRR_TS_ENABLE) != 0);
}

static unsigned char bcd2bin(unsigned char valbcd)
{
    return((valbcd >> 4) * 10 + (valbcd & 0x0F));
}

static unsigned char bin2bcd(unsigned char valbin)
{
    return(((valbin/10) << 4) + (valbin%10));
}

/* Posiziona i bit di anno bisestile senza alterare gli atri bit del registro */
static void LeapBitsWrite(void* base, short Bits)
{
    short temp;

    dp8570a(base, DP8570A_MSR)  = MSR_SEL_REG1;
    temp                        = dp8570a(base, DP8570A_RTMR);
    temp = temp & 0xFC;
    temp = temp | (Bits & 0x03);
    dp8570a(base, DP8570A_RTMR) = temp;
}


/* ****************************************************************************
    $Log: dp8570a.c,v $
    Revision 1.3  2004/12/13 11:49:20  mungiguerrav
    2004/nov/30,mar 15:30           Napoli      Mungi
    - Per rendere possibile la compilazione a scopo di debug anche in BorlandC,
      si aggiunge la definizione condizionata di XHUGE
    - _memWrite():  si modifica da      char data   a       short data,
                    per adeguarsi a macro dp8570a(base,offset)

    Revision 1.2  2004/11/08 10:08:26  mungiguerrav
    2004/nov/08,lun 10:55          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.1        2002/05/20 12:59:24     accardol
    - Inserimento in CVS della versione in lib07n
    Revision 1.1.1.1    2002/05/20 12:59:24     accardol
    - Inserimento in CVS in branch STP della versione in lib07n
01.04   2002/mag/20,lun             Napoli              Busco\Mungi
        - La v.01.03 entra ufficialmente in libreria UFAP
 ------------------------   Prima di CVS    ------------------------
01.03   2002/mar/02,gio             Napoli              Busco
        - Si agg. gestione dei bit per gli anni bistestili
01.02   2000/lug/21,mar             Napoli              Mungi
        - Si mod.  dp8570a_clk_read    in    dp8570a_clkRead,
          per distrazione non fatta nella v.1.01
01.01   2000/mag/30,mar             Napoli              Mungi
        - Si inserisce in \SWISV\LIB
        - Si mod. nomi routine sost. "_" con lettera Maiuscola
        - Si elim. prototipi dp8570a_tsEnable e dp8570a_tsDisable,
          erroneamente dichiarati static
01.00   1999/lug/29,gio             Napoli              Busco
        - Creazione in TLRV\A\STAF di FINA
*/

