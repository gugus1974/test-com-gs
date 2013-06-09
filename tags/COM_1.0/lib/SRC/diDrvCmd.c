/********************************************************************/
/* 2004/ott/22,ven 17:40                \SWISV\LIB\SRC\didrvcmd.c   */
/********************************************************************/
/*
    $Date: 2004/10/22 15:46:38 $          $Revision: 1.5 $
    $Author: mungiguerrav $
*/


#include "string.h"
#include "stdlib.h"
#include "consolle.h"
#include "didrv.h"
#include "didrvcmd.h"

static tPrintChDescrFunc PrintChDescrFunc;

static void cmdDi(short argc, char* argv[]);

void didrvcmd_init(tPrintChDescrFunc pcdf)
{
    consolle_addcmd("di","[diNumber] [reset|test]","gestione schede DI", cmdDi);
    PrintChDescrFunc = pcdf;
}


/************************/
/* gestione comando di  */
/************************/
static void cmdDi(short argc, char* argv[])
{
    if(argc > 2)
    {
        unsigned short numDi;
        /* ricava il num della scheda Di */
        numDi = atoi(argv[1]) - 1;          /* l'utente numera le DI come 1, 2, etc... */
        if (strcmp(argv[2],"reset") == 0)
        {
            /* Reset scheda */
            didrv_reset(numDi);
            consolle_print("DI %d board reset\n",numDi);
        }
        else if (strcmp(argv[2],"frc") == 0)
        {
            /* Forzamento canale */
            if (argc > 4)
            {
                short nbit;
                short statobit;

                nbit     = atoi(argv[3]);
                statobit = atoi(argv[4]);

                if ((nbit>=1) && (nbit<=31) && (statobit>=0) && (statobit<=1))
                {
                    didrv_SetChannelForcing(numDi,nbit,statobit);
                    consolle_print("DI %d CHAN: %d forced to %d\n",numDi,nbit,statobit);
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

                nbit = atoi(argv[3]);
                if ((nbit>=1) && (nbit<=31))
                {
                    didrv_SetChannelForcing(numDi,nbit,-1);
                    consolle_print("DI %d CHAN: %d forcing cleared\n",numDi,nbit);
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
        else if (strcmp(argv[2],"h") == 0)
        {
            consolle_print("DI COMMAND SYNTAX:\n");
            consolle_print("di diNumber               - Show the DI board status\n");
            consolle_print("di diNumber reset         - Reset the DI board\n");
            consolle_print("di diNumber frc <ch> <st> - Force the DI channel <ch> to <st>\n");
            consolle_print("di diNumber rel <ch>      - Release forcing on channel <ch>\n");
        }
        else
        {
            /* Comando errato */
            consolle_print("Wrong parameters\n");
        }
    }
    else
    {
        unsigned short i,j;
        for (j=0;j<didrv_numDi();j++)
        {
            didrv_input(j);
            consolle_print("DI %d STATUS: ",j);
            didrv_StatusPrint(j);

            consolle_print("CHAN. IN FRC STATUS FAIL  DESCRIPTION\n");

            for (i=0;i<31;i++)
            {
                short temp;

                /* Stampa lo stato dell'ingresso scheda */
                consolle_print("%2d    %d   ",i+1,didrv_GetChannelInput(j,i+1));

                /* Stampa lo stato di forcing */
                temp = didrv_GetChannelForcing(j,i+1);
                if (temp == -1)
                {
                    consolle_print(" ");
                }
                else
                {
                    consolle_print("%d",temp);
                }

                /* Stampa lo stato del canale */
                consolle_print("   %2d     ",didrv_GetChannelStatus(j,i+1));

                /* Stampa lo stato di fail */
                temp = didrv_GetChannelFailure(j,i+1);
                switch(temp)
                {
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

                if (PrintChDescrFunc != NULL)
                {
                    consolle_print(" - %s\n",PrintChDescrFunc(j,i+1));
                }
                else
                {
                    consolle_print("\n");
                }
            }
        }
    }
}


/* ****************************************************************************
    $Log: diDrvCmd.c,v $
    Revision 1.5  2004/10/22 15:46:38  mungiguerrav
    2004/ott/22,ven 17:40          Napoli       Mungi\Nappo
    - Come fatto da Calabrese su dodrvcmd, si numerano le schede DI
      da 1 a N, invece che da 0 a N-1, come sono abituati a fare gli utenti
      sui veicoli

    Revision 1.4  2004/10/22 15:36:46  mungiguerrav
    2004/ott/22,ven 17:15          Napoli       Mungi
    - Si riporta sul tronco principale la correzione effettuata in branch
      didrv_nappoc  su gestione più schede
    - Si aggiungono keyword CVS e storia passata


    Revision 1.3.4.1      2004/09/07 07:34:10     nappoc
    2004/set/07         Napoli              Nappo
    - Si adegua il modulo alle modifiche apportate a didrv
      (MNG: mi sembra un po' vago)

 ---------------    Fase transitoria tra *.mod e CVS    -------------
    Revision 1.3      2004/06/15 11:24:33     buscob
02.04   2003.dic.02,mar         Madrid                  Mungi
        - Si reinserisce la modifica 02.02-2003.lug.16,mer, sul numero canali,
          persa da Nappo nella versione 02.03,
          nonostante tutti gli "scenziamienti" del CVS
                        Un arrabbiato Mungi
02.03   2003.nov.24,lun         Napoli                  Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso

    Revision 1.2      2004/06/15 10:17:06     buscob
02.02   2003.lug.16,mer         Napoli                  Mungi
        - Si incrementa numero canali gestiti da 19 a 31

    Revision 1.1      2002/05/21 15:39:40   accardol
    - Inserimento in CVS
    Revision 1.1.1.1  2002/05/21 15:39:40   accardol
    - Inserimento in CVS in branch STP
02.01   2002.mag.21,mar         Napoli                  Busco\Mungi
        - Si agg. gestione stampa descrizione canale ID con funz. di applicativo
        - Si cambia nome    da  gest_di()   a  cmdDi()
 ----------------    Prima di CVS    -----------------------------
02.00   2002.mag.20,lun         Napoli                  Busco
        - Si sposta da AIACE\SRC    a   LIB\SRC
        - Si agg. sottocomando frc,rel,h    per     forcing,release,help
01.02    2001.nov.17,sab         Pistoia                 Accardo/Mungi
        - Si sost. printf() con consolle_print().
        - Si elim. #include "stdio.h"
01.01    2001.feb.28,mer        Napoli                  Accardo
        - Si sposta da \SWISV\TLRV\SRC in \SWISV\AIACE\SRC.
----------------------Storia precedente di TLRV-----------------------
01.01   2001.gen.22,lun         Napoli                  Busco
        - Si usano le funzioni didrv_alarm() e _GetChannel(..)
01.00   2000/feb/07,lun - Creazione: si usa la funzione di_stato per accedere a di_ok
*/
