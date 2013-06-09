/********************************************************************/
/* 2004/ott/25,lun 12:15                \SWISV\LIB\SRC\dodrvcmd.c   */
/********************************************************************/
/*
    $Date: 2004/10/25 10:43:58 $          $Revision: 1.4 $
    $Author: mungiguerrav $
*/



#include "string.h"
#include "stdlib.h"

#include "consolle.h"

#include "dodrv.h"
#include "dodrvcmd.h"

static tPrnChDescFun PrnChDescFun;

static void cmdDo(short argc, char* argv[]);

/****************************/
/* Inizializza il modulo DO */
/****************************/
void dodrvcmd_init(tPrnChDescFun pcdf)
{
    consolle_addcmd("do","[reset] [nbit stato]","gestione scheda DO", cmdDo);
    PrnChDescFun = pcdf;
}

/************************/
/* gestione comando do  */
/************************/
static void cmdDo(short argc, char* argv[])
{
    short j;

    if(argc > 2)
    {
        unsigned short numDo;
        /* ricava il num della scheda Di */
        numDo = atoi(argv[1]) - 1;      /* l'utente numera le DO come 1, 2, etc... */

        if (strcmp(argv[2],"reset") == 0)
        {
            /* Reset scheda */
            dodrv_reset(numDo);
            consolle_print("DO %d board reset\n", numDo);
        }
        else if (strcmp(argv[2],"frc") == 0)
        {
            /* Forzamento canale */
            if (argc > 4)
            {
                short nbit;
                short statobit;

                nbit     = argv[3][0]-'A'+1; /* i canali della DO sono indicati dalle lettere maiuscole */
                statobit = atoi(argv[4]);

                if ((nbit>=1) && (nbit<=19) && (statobit>=0) && (statobit<=1))
                {
                    dodrv_SetChannelForcing(numDo,nbit,statobit);
                    consolle_print("DO %d CHAN: %d forced to %d\n",numDo, nbit,statobit);
                }
                else
                {
                    /* Parametri errati */
                    consolle_print("Wrong parameters\n");
                }
            }
            else
            {
                /* Parametri mancanti */
                consolle_print("Missing parameters\n");
            }
        }
        else if (strcmp(argv[2],"rel") == 0)
        {
            /* Clear Forzamento canale */
            if (argc > 3)
            {
                short nbit;

                nbit     = argv[3][0]-'A'+1; /* i canali della DO sono indicati dalle lettere maiuscole */
                if ((nbit>=1) && (nbit<=19))
                {
                    dodrv_SetChannelForcing(numDo, nbit,-1);
                    consolle_print("DO %d CHAN: %d forcing cleared\n", numDo, nbit);
                }
                else
                {
                    /* Parametri errati */
                    consolle_print("Wrong parameters\n");
                }
            }
            else
            {
                /* Parametri mancanti */
                consolle_print("Missing parameters\n");
            }
        }
        else if (strcmp(argv[2],"failenable") == 0)
        {
            /* Forzamento canale */
            if (argc > 4)
            {
                short nbit;
                short statobit;

                nbit     = argv[3][0]-'A'+1; /* i canali della DO sono indicati dalle lettere maiuscole */
                statobit = atoi(argv[4]);

                if ((nbit>=1) && (nbit<=19) && (statobit>=0) && (statobit<=1))
                {
                    dodrv_SetChannelFailEnable(numDo,nbit,statobit);
                    consolle_print("DO %d CHAN: %d ability to detect failures %d\n",numDo, nbit,statobit);
                }
                else
                {
                    /* Parametri errati */
                    consolle_print("Wrong parameters\n");
                }
            }
            else
            {
                /* Parametri mancanti */
                consolle_print("Missing parameters\n");
            }
        }
        else if (strcmp(argv[2],"chres") == 0)
        {
            dodrv_ResetChannels(numDo, 0xFFFFFFFFL);
            consolle_print("Channels reset\n DO %d",numDo);
        }
        else if (strcmp(argv[1],"h") == 0)
        {
            consolle_print("DO COMMAND SYNTAX:\n");
            consolle_print("do               - Show the DO board status\n");
            consolle_print("do reset         - Reset the DO board\n");
            consolle_print("do frc <ch> <st> - Force the DO channel <ch> to <st>\n");
            consolle_print("do failenable <ch> <st> - Force the DO channel <ch> ability to detect fails to <st>\n");
            consolle_print("do rel <ch>      - Release forcing on channel <ch>\n");
            consolle_print("do chres         - Reset channel failures\n");
        }
        else
        {
            /* Comando errato */
            consolle_print("Wrong parameters\n");
        }
    }
    else for (j=0;j<dodrv_numDo();j++)
    {
        short i;

        dodrv_input(j);
        consolle_print("DO STATUS: %d ",j+1);
        dodrv_StatusPrint(j);

        consolle_print("CHAN. CMD FRC STATUS FAIL\n");

        for (i=0;i<19;i++)
        {
            short temp;

            consolle_print("%1c      %d   ",'A'+i,dodrv_GetChannelCommand(j,i+1));

            /* Stampa lo stato di forcing */
            temp = dodrv_GetChannelForcing(j,i+1);
            if (temp == -1)
            {
                consolle_print(" ");
            }
            else
            {
                consolle_print("%d",temp);
            }

            /* Stampa lo stato del canale */
            consolle_print("   %2d     ",dodrv_GetChannelStatus(j, i+1));

            /* Stampa lo stato di fail */
            if ( dodrv_GetChannelFailEnable(j, i+1) )
            {
                temp = dodrv_GetChannelFailure(j, i+1);
            }
            else
            {
                temp = -1;
            }

            switch(temp)
            {
                case -1:
                    consolle_print("d");
                break;

                case 1:
                    consolle_print("0");
                break;

                case 2:
                    consolle_print("1");
                break;

                case 3:
                    consolle_print("X");
                break;

                default:
                    consolle_print(" ");
                break;

            }

            if (PrnChDescFun != NULL)
            {
                consolle_print(" - %s\n",PrnChDescFun(j, i+1));
            }
            else
            {
                consolle_print("\n");
            }
        }
    }
}


/* ****************************************************************************
    $Log: doDrvCmd.c,v $
    Revision 1.4  2004/10/25 10:43:58  mungiguerrav
    2004/ott/25,lun 12:15          Napoli       Mungi
    - Si riporta sul tronco principale la modifica effettuata nel branch
      work_calabresea  su gestione più schede do
    - Si aggiungono keyword CVS e storia passata
    - Si migliorano incolonnamenti e parentesi


    Revision 1.3.6.1  2004/10/13 11:49:19  calabresea
    introduzione DO multipla

 ---------------    Fase transitoria tra *.mod e CVS    -------------
    Revision 1.3      2004/06/15 11:27:46     buscob
02.03   2003/nov/21,ven         Napoli                  Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso.

    Revision 1.2      2004/06/15 10:19:12     buscob
02.02   2003/lug/16,mer             Napoli                  Mungi
        - Si agg.   include dodrvcmd.h  per nuovo tipo funzione stampa
        - Si def.   static tPrnChDescFun PrnChDescFun
        - Si cambia _init   con aggiunta parametro      tPrnChDescFun pcdf
          e si pone       PrnChDescFun = pcdf
        - Si cambia nome    gest_do     in      cmdDo
          e si aggiunge     consolle_print(" - %s\n",PrnChDescFun(i+1))

    Revision 1.1      2002/05/21 14:58:12   accardol
    - Inserimento in CVS
    Revision 1.1.1.1  2002/05/21 14:58:12   accardol
    - Inserimento in CVS in branch STP
02.01   2002/mag/21,mar         Napoli                  Busco\Mungi
        - Si stampa anche il caso di FAIL disabilitato ('d')

 ----------------    Prima di CVS    -----------------------------
02.00   2002/mag/20,lun         Napoli                  Busco
        - Si sposta da AIACE\SRC    a   LIB\SRC
        - Si agg. sottocomando frc,rel,h    per     forcing,release,help
01.05   2002/feb/05,mar         Napoli                  Busco
        - Si agg. stampa dello stato dei canali
01.04   2002/feb/05,mar         Napoli                  Busco
        - Si aggiungono i sottocomandi fc, clf, h
        - Si migliora la stampa dello stato della scheda includendo i forzamenti
        - Si usa la funzione dodrv_StatusPrint per la stampa dello stato scheda
01.03   2002/gen/27,dom         Napoli                  Accardo/Mungi
        - Si modifica ordine degli #include
01.02   2001/nov/17,sab         Pistoia                 Accardo/Mungi
        - Si sost. printf() con consolle_print().
        - Si elim. #include "stdio.h"
01.01    2001/feb/28,mer         Napoli                  Accardo
        - Si sposta da \SWISV\TLRV\SRC in \SWISV\AIACE\SRC.
----------------------Storia precedente di TLRV-----------------------
01.01   2001/gen/22,lun         Napoli                  Busco
        - Si usano funzioni dodrv_alarm() e _GetChannel()
01.00   2000/feb/07,lun - Creazione
*/

