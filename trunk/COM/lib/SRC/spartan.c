/********************************************************************/
/* 2004/nov/15,lun 10:00                \SWISV\LIB\SRC\spartan.c    */
/********************************************************************/
/*
    $Date: 2004/11/16 15:19:04 $          $Revision: 1.3 $
    $Author: mungiguerrav $
*/


#include "hw16x.h"
#include "spartan.h"

/*****************************/
/* Prototipi funzioni locali */
/*****************************/
static void send_byte(char valore);

static tSetPinFunc SetDIN;
static tSetPinFunc SetCCLK;
static tSetPinFunc SetPROG;
static tGetPinFunc GetDONE;
static tGetPinFunc GetINIT;

void spartan_init(tSetPinFunc SetDINFunc,
                  tSetPinFunc SetCCLKFunc,
                  tSetPinFunc SetPROGFunc,
                  tGetPinFunc GetDONEFunc,
                  tGetPinFunc GetINITFunc
                 )
{
    SetDIN  = SetDINFunc;
    SetCCLK = SetCCLKFunc;
    SetPROG = SetPROGFunc;
    GetDONE = GetDONEFunc;
    GetINIT = GetINITFunc;

    /* posiziona il buffer in input */
    GetDONE();
    GetINIT();

    SetCCLK(0);
    SetDIN(0);
    SetPROG(1);

    /* Invia dati sporchi per fare uscire le fpga dallo stato di boot */
    {
        short i;

        for (i=0;i<10;i++)
            send_byte(0x0F);
    }
}

static void send_byte(char valore)
{
    short j;

    for(j=0;j<8;j++)
    {
        SetCCLK(0);

        SetDIN(valore & 1);

        valore = valore >> 1;

        /* invia l'impulso di clock */
        SetCCLK(1);
    }
}


short spartan_load(unsigned char huge *p, unsigned short numbyte)
{
    unsigned short    i,k;
    unsigned long   tout;
    short           LoadError;

    LoadError = 0;

    /**************************************/
    /* Invia il comando di programmazione */
    /**************************************/
    SetPROG(0);
    for(k=0;k<100;k++);
    SetPROG(1);

    /****************************/
    /* Attende che INIT si alzi */
    /****************************/
    tout = 0x2000L;
    while(!GetINIT() && (--tout>0));

    /********************************************/
    /* Se e' andato in timeout segnala errore 1 */
    /********************************************/
    if(!tout)
    {
        LoadError |= 1;
    }

    /**********************************/
    /* invia il programma bit per bit */
    /**********************************/
    for(i=0;i<numbyte;i++)
    {
        send_byte(p[i]);

        if (i<(numbyte-1))
        {
            /*****************************************/
            /* Durante il caricamento devono essere: */
            /* INIT = alto                           */
            /* DONE = basso                          */
            /*****************************************/
            if (!GetINIT())
            {
                LoadError |= 2;
            }

            if (GetDONE())
            {
                LoadError |= 4;
            }
        }

        if((i & 0xFFF) == 0)
            hw167_wdServ();
    }

    for(k=0;k<100;k++);

    /******************************************/
    /* Alla fine del caricamento deve essere: */
    /* DONE = alto                            */
    /******************************************/
    if(!GetDONE())
    {
        LoadError |= 8;
    }

    return(LoadError);
}


/* ****************************************************************************
    $Log: spartan.c,v $
    Revision 1.3  2004/11/16 15:19:04  mungiguerrav
    2004/nov/15,lun 10:00          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.2        2004/06/15 11:53:13     buscob
02.03   2003/nov/21,ven             Napoli              Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso.

    Revision 1.1        2001/12/21 13:21:18     accardol
    - Inserimento in CVS della versione in lib07m
    Revision 1.1.1.1    2001/12/21 13:21:18     accardol
    - Inserimento in CVS in branch STP della versione in lib07m
        La data è arretrata con un artificio
02.02   2001/dic/21,ven             Napoli              Mungi\Accardo
        - Si elim. include libisv.h
        - Si sost. Ulong con unsigned long

 ------------------------   Prima di CVS    ------------------------
02.01   2001/gen/26,ven             Napoli              Busco/Accardo
        - spartan_load(): si dichiara huge il puntatore *p
02.00   2000/dic/06,mer             Napoli              Busco
        - Si rende static la funzione send_byte()
        - Si gestiscono i pin di collegamento con la FPGA mediante
          callback functions definite dall'applicatico e passate nella
          funzione fpgaload_init(...)
        - Si rinomina in startan.c
01.04   2000/dic/05,mar             Napoli              Busco
        - Si aggiunge include libisv.h
01.03   2000/giu/05,lun             Napoli              Mungi
        - Si elim. include libXXX7.h, gia' inserito in fpgaload.h
        - Si elim. le def. dei bit, gia' inserite in bitfield.h
        - Si incl.  hw16x.h          invece di  atr_hw.h
        - Si chiama hw16x_wdServ()   invece di  hw_watchdog_service()
01.02   2000/feb/07,lun             Napoli              De Marco/Busco
        - Si iniz. tout fuori dallo switch per warning level 3
01.01   1999/xxx/XX,xxx             Napoli              Busco
        - Modifiche non documentate
01.00   1999/apr/19,lun             Napoli              Busco
        - Creazione a partire da ETR 500P con nome fpgaload.c
*/

