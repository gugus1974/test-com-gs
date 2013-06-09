/****************************************************************/
/* 2004/ott/25,lun 11:45                \SWISV\LIB\SRC\didrv.c  */
/****************************************************************/
/*
    $Date: 2004/10/25 09:49:23 $          $Revision: 1.4 $
    $Author: mungiguerrav $
*/


#include "string.h"
#include "consolle.h"
#include "vardb.h"
#include "didrv.h"

/**********************************/
/* Variabili globali di programma */
/**********************************/
extern volatile short vme_ko;

/*********************************************************************/
/* Struttura locale per la gestione dei buffer di una singola scheda */
/*********************************************************************/
static struct diBuffer
{
    unsigned long buffer_in_di;                     /* Immagine buffer di ingresso              */
    unsigned long buffer_out_di;                    /* Immagine buffer di uscita                */
    volatile unsigned short huge *BaseAddressRd;    /* Indirizzo base della scheda in lettura   */
    volatile unsigned short huge *BaseAddressWr;    /* Indirizzo base della scheda in scrittura */
    unsigned long Force0Buffer;                     /* Ogni bit posto a 1 indica canale forzato a 0 */
    unsigned long Force1Buffer;                     /* Ogni bit posto a 1 indica canale forzato a 1 */
    unsigned long Fail0Buffer;                      /* Ogni bit posto a 1 indica fail del canale a 0 */
    unsigned long Fail1Buffer;                      /* Ogni bit posto a 1 indica fail del canale a 1 */
    unsigned short BoardStatus;
};

/******************************/
/* Variabili locali di modulo */
/******************************/
struct  diBuffer        *diBuf;                 /* puntatore all'area di memoria in cui sono allocate le schede di */
static  unsigned short  diBufSize;              /* Dimensione memoria per array di schede                          */
static  unsigned short  diNitem;                /* Numero di schede presenti                                       */
static short dummy;

#define DELAY {unsigned short i; for(i=0;i<0xffff;i++); }

/*****************************/
/* Funzioni locali di modulo */
/*****************************/
static void            wrbitbuffer(unsigned short diNumber,unsigned short nbit, unsigned short stato);
static unsigned short  rdbitbuffer(unsigned short diNumber,unsigned short nbit);

/****************************/
/* Inizializza la scheda DI */
/****************************/
void didrv_init(char *buff, unsigned short size)
{
    diBuf           = (struct diBuffer *) buff;
    diBufSize       = size;
    diNitem         = 0;
}

/************************************************************/
/* Aggiunge ed imposta la scheda selezionata aggiungendo    */
/* quelle che non sono state ancora allocate                */
/* Restituisce:                                             */
/*   index - se OK                                          */
/*   -1 - se non c'e' piu' spazio                           */
/*   -2 - se la scheda esiste gia'                          */
/************************************************************/
short didrv_add(unsigned short diNumber,void huge *BaseRd,void huge *BaseWr,const char *HighName,const char *LowName)
{
    short   retval;

    /*Controlla se la scheda e' gia' presente */
    if (diNumber<diNitem)
    {
        /* E' stata trovata e segnala che era già presente */
        retval = -2;
        consolle_print("di number %d already configured\n",diNumber);
    }
    /* Non e' stata trovata controlla se c'e' spazio per aggiungerla */
    else if (sizeof(struct diBuffer) * (diNitem+1) < diBufSize)
    {
        consolle_print("configured di number %d\n",diNumber);
        diBuf[diNumber].BaseAddressRd = BaseRd;
        diBuf[diNumber].BaseAddressWr = BaseWr;
        diBuf[diNumber].buffer_in_di  = 0L;
        diBuf[diNumber].buffer_out_di = 0L;
        diBuf[diNumber].Force0Buffer  = 0L;
        diBuf[diNumber].Force1Buffer  = 0L;
        diBuf[diNumber].Fail0Buffer   = 0L;
        diBuf[diNumber].Fail1Buffer   = 0L;
        diBuf[diNumber].BoardStatus   = 0;

        if (HighName != NULL)
        {
            vardb_addvar(HighName,  "", "High 16 di ch", 1000, (short far *) (&diBuf[diNumber].buffer_in_di)+1,  VARDB_TYPEUHEX);
        }
        if (LowName != NULL)
        {
            vardb_addvar(LowName,   "", "Low 16 di ch",  1000, (short *) &diBuf[diNumber].buffer_in_di, VARDB_TYPEUHEX);
        }
        retval = diNitem;
        diNitem++;
    }
    else
    {
        /* Non c'e' piu' spazio, lo segnala */
        retval = -1;
        consolle_print("no space to configure di number %d\n",diNumber);
    }
    return(retval);
}

/***********************************************/
/* Restituisce il numero di schede DI allocate */
/***********************************************/
unsigned short didrv_numDi(void)
{
    return(diNitem);
}

/************************/
/* Resetta la scheda DI */
/************************/
void didrv_reset(unsigned short diNumber)
{
    if(diNumber < diNitem)
    {
        diBuf[diNumber].buffer_in_di  = 0;
        diBuf[diNumber].buffer_out_di = 0;

        wrbitbuffer(diNumber,0,1);   /* scheda spenta */
        didrv_output(diNumber);
        DELAY;
        wrbitbuffer(diNumber,0,0);   /* scheda accesa */
        didrv_output(diNitem);
    }
}

/**************************************************/
/* Esegue i test di funzionamento della scheda DI */
/**************************************************/
void didrv_test(unsigned short diNumber)
{
    if(diNumber < diNitem)
    {
        diBuf[diNumber].BoardStatus = 0;
        /* Test di colloquio VME */
        vme_ko = 0;

        dummy  = *diBuf[diNumber].BaseAddressRd;
        dummy  = *diBuf[diNumber].BaseAddressRd;
        dummy  = *diBuf[diNumber].BaseAddressRd;
        dummy  = *diBuf[diNumber].BaseAddressRd;
        dummy  = *diBuf[diNumber].BaseAddressRd;

        if (vme_ko)
        {
            diBuf[diNumber].BoardStatus |= DIDRV_STATUSVMEFAIL;
        }

        /* TEST 1 */
        didrv_reset(diNumber);

        wrbitbuffer(diNumber,7,1);   /* test1=1 */
        wrbitbuffer(diNumber,5,0);   /* test0=0 */
        wrbitbuffer(diNumber,0,0);   /* abil=0  */
        didrv_output(diNumber);
        DELAY;
        didrv_input(diNumber);

        diBuf[diNumber].Fail1Buffer = ~(diBuf[diNumber].buffer_in_di | 0x80000000L);
        if (diBuf[diNumber].Fail1Buffer)
        {
            diBuf[diNumber].BoardStatus |= DIDRV_STATUSCHFAIL1;
        }

        /* TEST 0 */
        didrv_reset(diNumber);

        wrbitbuffer(diNumber,7,0);   /* test1=0 */
        wrbitbuffer(diNumber,5,1);   /* test0=1 */
        wrbitbuffer(diNumber,0,0);   /* abil=0  */
        didrv_output(diNumber);
        DELAY;
        didrv_input(diNumber);

        diBuf[diNumber].Fail0Buffer = diBuf[diNumber].buffer_in_di;
        if (diBuf[diNumber].Fail0Buffer)
        {
            diBuf[diNumber].BoardStatus |= DIDRV_STATUSCHFAIL0;
        }

        didrv_reset(diNumber);
    }
}

/******************************************************/
/* restituisce l'immagine della scheda DI selezionata */
/* restituisce 0 se la scheda non è presente          */
/******************************************************/
unsigned long didrv_read(unsigned short diNumber)
{
    unsigned long RetVal;
    RetVal = 0L;
    if(diNumber < diNitem)
    {
        RetVal = diBuf[diNumber].buffer_in_di;
    }
    return RetVal;
}

/***********************************/
/* Legge il buffer dalla scheda DI */
/***********************************/
void didrv_input(unsigned short diNumber)
{
    unsigned short temp1;
    unsigned short temp2;
    unsigned short temp3;
    unsigned short temp4;

    if(diNumber < diNitem)
    {

        if (!(diBuf[diNumber].BoardStatus & DIDRV_STATUSVMEFAIL))
        {
            temp1 = (*(diBuf[diNumber].BaseAddressRd+0));
            temp2 = (*(diBuf[diNumber].BaseAddressRd+1));
            temp3 = (*(diBuf[diNumber].BaseAddressRd+2));
            temp4 = (*(diBuf[diNumber].BaseAddressRd+3));

            temp1 = temp1 & 0x00FF;
            temp2 = temp2 & 0x00FF;
            temp3 = temp3 & 0x00FF;
            temp4 = temp4 & 0x00FF;

            temp1 = temp1 +  (temp2 << 8);
            temp3 = temp3 +  (temp4 << 8);
            diBuf[diNumber].buffer_in_di = temp1 + (((unsigned long)temp3) << 16);
        }
    }
}

/************************************/
/* Scrive il buffer sulla scheda DI */
/************************************/
void didrv_output(unsigned short diNumber)
{
    if(diNumber < diNitem)
    {
        if (!(diBuf[diNumber].BoardStatus & DIDRV_STATUSVMEFAIL))
        {
            *diBuf[diNumber].BaseAddressWr = (diBuf[diNumber].buffer_out_di & 0xFFL);
        }
    }
}

/*************************************/
/* Restituisce lo stato della scheda */
/*************************************/
unsigned short didrv_status(unsigned short diNumber)
{
    unsigned short RetVal;
    RetVal = 0;
    if(diNumber < diNitem)
    {
        RetVal = diBuf[diNumber].BoardStatus;
    }
    return RetVal;
}

/**************************************/
/* Stampa a video il codice di errore */
/**************************************/
void didrv_StatusPrint(unsigned short diNumber)
{
    if(diNumber < diNitem)
    {
        if (!diBuf[diNumber].BoardStatus)
        {
            consolle_print("DI NUMBER %d: OK\n",diNumber);
        }

        if (diBuf[diNumber].BoardStatus & DIDRV_STATUSVMEFAIL)
        {
            consolle_print("DI NUMBER %d: VME ACCESS FAILED\n",diNumber);
        }

        if (diBuf[diNumber].BoardStatus & DIDRV_STATUSSUPPLYFAIL)
        {
            consolle_print("DI NUMBER %d: SUPPLY FAIL\n",diNumber);
        }

        if (diBuf[diNumber].BoardStatus & DIDRV_STATUSCHFAIL1)
        {
            consolle_print("DI NUMBER %d: CH FAIL 1\n",diNumber);
        }

        if (diBuf[diNumber].BoardStatus & DIDRV_STATUSCHFAIL0)
        {
            consolle_print("DI NUMBER %d: CH FAIL 0\n",diNumber);
        }
    }
}


/****************************************/
/* Posiziona il forzamento ad un canale */
/* state = 0 -> Forza a 0               */
/* state = 1 -> Forza a 1               */
/* state =-1 -> Elimina forzamento      */
/****************************************/
void didrv_SetChannelForcing(unsigned short diNumber,unsigned short ch, unsigned short frc)
{
    if((diNumber<diNitem)&&(ch>0 && ch<32))
    {
        unsigned long ltemp;
        ltemp = (0x1L << (ch-1));

        switch(frc)
        {
            case 0:
                /* Deve forzare a 0 */
                diBuf[diNumber].Force0Buffer |=  ltemp;
                diBuf[diNumber].Force1Buffer &= ~ltemp;
            break;

            case 1:
                /* Deve forzare a 0 */
                diBuf[diNumber].Force0Buffer &= ~ltemp;
                diBuf[diNumber].Force1Buffer |=  ltemp;
            break;

            default:
                /* Deve eliminare il forzamento */
                diBuf[diNumber].Force0Buffer &= ~ltemp;
                diBuf[diNumber].Force1Buffer &= ~ltemp;
            break;
        }
    }
}

/*********************************************/
/* Legge lo stato di forzamento di un canale */
/* Restituisce:                              */
/* 0  = Forzato a 0                          */
/* 1  = Forzato a 1                          */
/* -1 = Non Forzato                          */
/*********************************************/
short didrv_GetChannelForcing(unsigned short diNumber,unsigned short ch)
{
    short RetVal;

    RetVal = -1;
    if((diNumber<diNitem)&&(ch>0 && ch<32))
    {
        if ((diBuf[diNumber].Force0Buffer & (0x1L << (ch-1))) != 0)
        {
            RetVal = 0;
        }
        if ((diBuf[diNumber].Force1Buffer & (0x1L << (ch-1))) != 0)
        {
            RetVal = 1;
        }
    }
    return(RetVal);
}

/*********************************************/
/* Restituisce lo stato del bit Alarm        */
/* restituisce 2 se la scheda non è presente */
/*********************************************/
unsigned short didrv_alarm(unsigned short diNumber)
{
    unsigned short RetVal;

    RetVal = 2;
    if(diNumber < diNitem)
    {
        RetVal = rdbitbuffer(diNumber,31);
    }
    return(RetVal);
}

/***********************************/
/* Imposta lo stato on/off del LED */
/***********************************/
void didrv_led(unsigned short diNumber,short on_off)
{
    if(diNumber < diNitem)
    {
        wrbitbuffer(diNumber,2,on_off);
    }
}

/***************************************************/
/* Restituisce lo stato dell'ingresso di un canale */
/* (Indipendentemente dal forcing)                 */
/***************************************************/
short didrv_GetChannelInput(unsigned short diNumber,unsigned short ch)
{
    short RetVal;

    RetVal = 0;
    if ((diNumber<diNitem)&&(ch>0 && ch<32))
    {
        RetVal = ((diBuf[diNumber].buffer_in_di & (0x1L << (ch-1))) != 0);
    }
    return(RetVal);
}

/************************************************************************************/
/* Restituisce lo stato del canale calcolato come il valore in ingresso alla scheda */
/* condizionato dallo stato di forcing                                              */
/************************************************************************************/
short didrv_GetChannelStatus(unsigned short diNumber,unsigned short ch)
{
    short RetVal;

    RetVal = 0;
    if ((diNumber<diNitem)&&(ch>0 && ch<32))
    {
        RetVal = ((((diBuf[diNumber].buffer_in_di | diBuf[diNumber].Force1Buffer) & ~diBuf[diNumber].Force0Buffer) & (0x1L << (ch-1))) != 0);
    }
    return(RetVal);
}

/***************************************/
/* Legge lo stato di Fail di un canale */
/* Restituisce:                        */
/* 0 = Nessun Fail                     */
/* 1 = Fail 0                          */
/* 2 = Fail 1                          */
/* 3 = Fail 0 + 1                      */
/***************************************/
short didrv_GetChannelFailure(unsigned short diNumber,unsigned short ch)
{
    short RetVal;

    RetVal = 0;
    if((diNumber<diNitem)&&(ch>0 && ch<32))
    {
        if ((diBuf[diNumber].Fail0Buffer & (0x1L << (ch-1))) != 0)
        {
            RetVal |= 1;
        }
        if ((diBuf[diNumber].Fail1Buffer & (0x1L << (ch-1))) != 0)
        {
            RetVal |= 2;
        }
    }
    return(RetVal);
}


/****************************************************/
/* Scrive lo stato di un bit nel buffer immagine DI */
/****************************************************/
static void wrbitbuffer(unsigned short diNumber,unsigned short nbit, unsigned short stato)
{
    if (stato)
    {
        diBuf[diNumber].buffer_out_di |= (0x1L << nbit);
    }
    else
    {
        diBuf[diNumber].buffer_out_di &= ~(0x1L << nbit);
    }
}

/***************************************************/
/* Legge lo stato di un bit dal buffer immagine DI */
/***************************************************/
static unsigned short rdbitbuffer(unsigned short diNumber,unsigned short nbit)
{
    return((diBuf[diNumber].buffer_in_di & (0x1L << nbit)) != 0);
}


/* ****************************************************************************
    $Log: diDrv.c,v $
    Revision 1.4  2004/10/25 09:49:23  mungiguerrav
    2004/ott/25,lun 11:45          Napoli       Mungi
    - Si corregge errore su doppio test su HighName: il secondo è su LowName

    Revision 1.3  2004/10/22 15:10:36  mungiguerrav
    2004/ott/22,ven 16:10          Napoli       Mungi
    - Si riporta sul tronco principale la correzione effettuata in branch
      didrv_nappoc  su gestione più schede
    - Si aggiungono keyword CVS e storia passata




    Revision 1.2.4.2      2004/10/01 09:57:06     nappoc
    2003/set/29,mer     Napoli              Nappo
    - Si agg. 2 parametri nella funzione didrv_add per poter aggiungere due variabili in vardb
      che mappano parte alta e bassa del buffer della DI

    Revision 1.2.4.1      2004/09/07 07:31:09     nappoc
    2004/set/07         Napoli              Nappo
    - Si modifica il drv della scheda DI per poter gestire più schede

 ---------------    Fase transitoria tra *.mod e CVS    -------------
    Revision 1.2      2004/06/15 11:23:04     buscob
02.09   2003.nov.21,ven         Napoli                  Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso.

    Revision 1.1      2002/05/21 10:40:04   accardol
    - Inserimento in CVS
    Revision 1.1.1.1  2002/05/21 10:40:04   accardol
    - Inserimento in CVS in branch STP
02.08   2002.mag.21,mar         Napoli                  Busco\Mungi
        - Si distingue il caso DIDRV_STATUSCHFAIL1 dal caso DIDRV_STATUSCHFAIL0
        - Si elim. azzeramenti di Fail0Buffer, Fail1Buffer e BoardStatus nella
          funzione dodrv_reset();
 ----------------    Prima di CVS    -----------------------------
02.07   2002.mag.20,lun         Napoli                  Busco
        - Si sposta da AIACE\SRC a LIB\SRC e si inserisce la 02.06bs
        - Si agg. funzioni per forcing e lettura stato
02.06bs 2002.mar.06,mer         Napoli                  Busco
        - Si cambia nome alle funzioni didrv_wrbitbuffer e _rd.. perche' statiche
        - Si elim. include "libisv.h"
        - Si rinomina _error() in _status() e _errorprint() in _StatusPrint()
02.05   2002.gen.30,mer         Napoli                  Mungi
        - Si agg. uno spazio tra  if e il suo test (..)
        - Si agg. L finale dopo costante 0x7FFFFFFF per elim. warning del BorlandC
2.04    2001.mag.15,mar         Napoli                  Accardo
        - Si allunga attesa per test 1 e 0 da 0x2000 cicli a 0xffff cicli
2.03    2001.mag.10,gio         Napoli                  Accardo
        - Si mod. BaseAddressRd e BaseAddressWr da far a huge.
        - Si sost.  didrv_init(void far  *BaseRd,void far  *BaseWr)
          con       didrv_init(void huge *BaseRd,void huge *BaseWr)
2.02    2001.apr.28,sab         Napoli                  Accardo
        - Si sostituisce puntatore unico all'indirizzo base della scheda
          BaseAddress con due puntatori:
                BaseAddressRd per accesso in lettura
                BaseAddressWr per accesso in scrittura
          I due indirizzi coincidono per la scheda reale, mentre sono diversi
          per la scheda simulata.
02.01   2001/feb/28,mer         Napoli                  Accardo
        - Si sposta da \SWISV\TLRV\SRC in \SWISV\AIACE\SRC.
----------------------Storia precedente di TLRV-----------------------
02.01   2001/gen/19,gio         Napoli                  Busco
        - Si agg. funzione didrv_errorstr(..)
02.00   2001/gen/04,gio         Napoli                  Busco
        - Si cambia il nome del file da di.c a didrv.c
01.05   2000/feb/27,dom - Si sposta il comando di nel nuovo modulo dicmd.c
01.04   1998/nov/05,gio - Si agg. di_read()
                        - di_input restituisce ora void (aggiorna solo l'immagine interna)
01.03   1998/lug/31,ven - Si inizializza di_ok = 0
01.02   1998/mag/25,lun - Si agg. funzione di_alarm() per lettura stato alarm
                        - Si agg. funzione di_led(int) per accensione/spegnimento led
                        - Si agg. funzione di_wrbitbuffer() e di_output()
01.01   1998/mar/30,lun - La funzione di_input restituisce la parola letta
                        - Si agg. visualizzazione dello stato di allarme
01.00   1997/ott/17,ven - Creazione a partire da DIDO.C
*/
