/****************************************************************/
/* 2004/dic/15,mer 12:25                \SWISV\LIB\SRC\rtcCmd.c */
/****************************************************************/
/*          Comandi per le gestione del Real Time Clock         */
/****************************************************************/
/*
    $Date: 2004/12/15 11:57:04 $          $Revision: 1.4 $
    $Author: mungiguerrav $
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dp8570a.h"
#include "consolle.h"
#include "rtccmd.h"
#include "strparse.h"

/************************************/
/* Variabili statiche di modulo     */
/************************************/
static void *rtcBase;

/************************************************/
/* Prototipi delle funzioni statiche di modulo  */
/************************************************/
static void rtcShowStatus(void);

/****************************************************/
/* Prototipi delle funzioni di gestione dei comandi */
/****************************************************/
static void gest_rtc(short argc, char* argv[]);


void rtccmd_init(void *base)
{
    rtcBase = base;

    dp8570a_start(rtcBase);
    consolle_addcmd("rtc","[aa mm gg hh mm ss]", "Impostazioni Real Time Clock", gest_rtc);
}


static void rtcShowStatus(void)
{
    short i;
    struct dp8570a_datetime time;

    dp8570a_clkRead(rtcBase, &time);
    consolle_print("Actual time:  ");
    consolle_print("%02u/%02u/%02u ",time.giorno,time.mese,time.anno);
    consolle_print("%02u:%02u:%02u\n",time.ora,time.minuti,time.secondi);
    consolle_print("RTC status:   ");
    if (dp8570a_startRead(rtcBase))
    {
        consolle_print("Running\n");
    }
    else
    {
        consolle_print("Stopped\n");
    }
    consolle_print("Base address: 0x%lX\n",(unsigned long)rtcBase);
    consolle_print("Time Save:    ");
    if (dp8570a_tsRead(rtcBase))
    {
        consolle_print("Enabled\n");
    }
    else
    {
        consolle_print("Disabled\n");
    }
    consolle_print("Battery:      ");
    if (dp8570a_lowbat(rtcBase))
    {
        consolle_print("Low\n");
    }
    else
    {
        consolle_print("OK\n");
    }

    consolle_print("Memory dump:\n   _0 _1 _2 _3 _4 _5 _6 _7 _8 _9 _A _B _C _D _E _F\n0_    ");
    for(i=1; i<=0x0F; i++)
    {
        short   temp;
        dp8570a_memRead(rtcBase,i,&temp);
        consolle_print("%2X ",temp);
    }
    consolle_print("\n1_ ");
    for(i=0x10; i<=0x1F; i++)
    {
        short   temp;
        dp8570a_memRead(rtcBase,i,&temp);
        consolle_print("%2X ",temp);
    }
    consolle_print("\n");
}

/************************/
/* gestione comando rtc */
/************************/
static void gest_rtc(short argc, char* argv[])
{
    struct dp8570a_datetime time;

    if(argc == 7)
    {
        time.anno    = (unsigned char) atoi(argv[1]);
        time.mese    = (unsigned char) atoi(argv[2]);
        time.giorno  = (unsigned char) atoi(argv[3]);
        time.ora     = (unsigned char) atoi(argv[4]);
        time.minuti  = (unsigned char) atoi(argv[5]);
        time.secondi = (unsigned char) atoi(argv[6]);

        if((time.giorno>=1)&&(time.giorno<=31)&&
           (time.mese>=1)&& (time.mese<=12)&&(time.anno<=99)&&
           (time.ora<=24) &&(time.minuti<=59)&&(time.secondi<=59))
        {
            dp8570a_clkWrite(rtcBase, &time);
        }
        else
        {
            consolle_print("\nParametri errati");
        }
    }
    else
    {
        if (argc >= 2)
        {
            if(strcmp(argv[1],"base") == 0)
            {
                rtcBase = (void *)parse_number(argv[2]);
                consolle_print("RTC base address: 0x%lX\n",(unsigned long)rtcBase);
            }
            else if(strcmp(argv[1],"start") == 0)
            {
                dp8570a_start(rtcBase);
                consolle_print("RTC started\n");
            }
            else if(strcmp(argv[1],"stop") == 0)
            {
                dp8570a_stop(rtcBase);
                consolle_print("RTC stopped\n");
            }
            else if(strcmp(argv[1],"ts_enable") == 0)
            {
                dp8570a_tsEnable(rtcBase);
                consolle_print("RTC Time Save enabled\n");
            }
            else if(strcmp(argv[1],"ts_disable") == 0)
            {
                dp8570a_tsDisable(rtcBase);
                consolle_print("RTC Time Save disabled\n");
            }
            else if(strcmp(argv[1],"sm") == 0)
            {
                if (argc >= 4)
                {
                    dp8570a_memWrite(rtcBase,(unsigned char)parse_number(argv[2]),(unsigned char)parse_number(argv[3]));
                }
            }
        }
        else
        {
            rtcShowStatus();
        }
    }
}


/* ****************************************************************************
    $Log: rtcCmd.c,v $
    Revision 1.4  2004/12/15 11:57:04  mungiguerrav
    2004/dic/15,mer 12:25           Napoli      Mungi
    - Per adeguarsi al nuovo tipo del 3° parametro di dp8570a_memRead,
      si cambia    temp    da  unsigned char     a   short
    - Si applicano le regole di codifica sulle parentesi

    Revision 1.3  2004/11/10 10:07:42  mungiguerrav
    2004/nov/09,mar 17:05          Napoli       Mungi
    - Si aggiungono keyword CVS e breve storia


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.2        2004/06/15 11:51:27     buscob
01.02   2003/nov/21,ven             Napoli              Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso

    Revision 1.1        2000/05/30 09:17:02     accardol
    - Inserimento in CVS della versione in lib07m
    Revision 1.1.1.1    2000/05/30 09:17:02     accardol
    - Inserimento in CVS in branch STP della versione in lib07m
        La data è arretrata con un artificio
01.01   2000/mag/30,mar             Napoli              Mungi
        - Si inserisce in \SWISV\LIB
        - Si mod. nomi routine e var. sost. "_" con lettera Maiuscola

 ------------------------   Prima di CVS    ------------------------
01.00   1999/lug/29,gio             Napoli              Busco
        - Creazione in TLRV\A\STAF di FINA
*/

