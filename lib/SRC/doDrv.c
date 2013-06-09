/****************************************************************/
/* 2004/ott/29,ven 15:50                \SWISV\LIB\SRC\dodrv.c  */
/****************************************************************/
/*
    $Date: 2004/11/04 15:21:46 $          $Revision: 1.5 $
    $Author: mungiguerrav $
*/

#include "string.h"
#include "stdlib.h"

#include "consolle.h"
#include "vardb.h"

#include "dodrv.h"

extern volatile short vme_ko;

/*********************************************************************/
/* Struttura locale per la gestione dei buffer di una singola scheda */
/*********************************************************************/
static struct doBuffer
{
    unsigned long           buffer_in_do;           /* Immagine buffer di ingresso                  */
    unsigned long           buffer_out_do;          /* Immagine buffer di uscita                    */
volatile unsigned short huge *BaseAddressRd;        /* Indirizzo base della scheda in lettura       */
volatile unsigned short huge *BaseAddressWr;        /* Indirizzo base della scheda in scrittura     */
    unsigned long           Force0Buffer;           /* Ogni bit a 1 indica canale forzato a 0       */
    unsigned long           Force1Buffer;           /* Ogni bit a 1 indica canale forzato a 1       */
    unsigned long           Fail0Buffer;            /* Ogni bit a 1 indica fail del canale a 0      */
    unsigned long           Fail1Buffer;            /* Ogni bit a 1 indica fail del canale a 1      */
    unsigned short          BoardStatus;
    unsigned long           AppOutBuffer;           /* Buffer di uscita per comandi da applicativo  */
    unsigned long           FailEnable;             /* Ogni bit a 1 indica Fail detect abilitato    */
    unsigned long           ResetBuffer;            /* Ogni bit a 1 indica che il canale deve essere resettato */
};


/*****************************/
/* Variabili interne         */
/*****************************/
static  short           dummy;
struct  doBuffer        *doBuf;                 /* puntatore all'area di memoria in cui sono allocate le schede do */
static  unsigned short  doBufSize;              /* Dimensione memoria per array di schede                          */
static  unsigned short  doNitem;                /* Numero di schede presenti                                       */


#define DELAY {unsigned short i; for(i=0;i<0x2000;i++); }

/****************************************/
/* Prototipi delle funzione locali      */
/****************************************/
static void wrbitbuffer(unsigned short doNumber,unsigned short nbit, unsigned short stato);
static long rdbitbuffer(unsigned short doNumber,unsigned short nbit);


/******************************************************************************************/
/* Inizializza la scheda DO                                                               */
/* questa funzione consente all'applicativo di inizializzare il driver                    */
/* definendo il puntatore all'area di memoria dedicata alle informazioni relative alle DI */
/* e la grandezza di tale area di memoria                                                 */
/******************************************************************************************/
void dodrv_init(char *buff, unsigned short size)
{
    doBuf           = (struct doBuffer *) buff;  /* assegna il puntatore all'area di memoria DO */
    doBufSize       = size;                      /* assegna l'ampiezza dell'area di memoria     */
    doNitem         = 0;                         /* inizializza il numero di schede             */
}

/************************************************************/
/* Aggiunge ed imposta la scheda selezionata aggiungendo    */
/* quelle che non sono state ancora allocate                */
/* Restituisce:                                             */
/*   index - se OK                                          */
/*   -1 - se non c'e' piu' spazio                           */
/*   -2 - se la scheda esiste gia'                          */
/************************************************************/
short dodrv_add(unsigned short doNumber,void huge *BaseRd,void huge *BaseWr,const char *HighName,const char *LowName)
{
    short   retval;

    /*Controlla se la scheda e' gia' presente */
    if (doNumber<doNitem)
    {
        /* E' stata trovata e segnala che era già presente */
        retval = -2;
        consolle_print("di number %d already configured\n",doNumber);
    }
    /* Non e' stata trovata controlla se c'e' spazio per aggiungerla */
    else if (sizeof(struct doBuffer) * (doNitem+1) < doBufSize)
    {
        consolle_print("configured do number %d\n",doNumber);
        doBuf[doNumber].BaseAddressRd = BaseRd;
        doBuf[doNumber].BaseAddressWr = BaseWr;
        doBuf[doNumber].buffer_in_do  = 0L;
        doBuf[doNumber].buffer_out_do = 0L;
        doBuf[doNumber].Force0Buffer  = 0L;
        doBuf[doNumber].Force1Buffer  = 0L;
        doBuf[doNumber].Fail0Buffer   = 0L;
        doBuf[doNumber].Fail1Buffer   = 0L;
        doBuf[doNumber].BoardStatus   = 0;
        doBuf[doNumber].AppOutBuffer  = 0L;
        doBuf[doNumber].ResetBuffer   = 0L;
        doBuf[doNumber].FailEnable    = 0;
        if (HighName != NULL)
        {
            vardb_addvar(HighName,  "", "High 16 di ch", 1000, (short far *) (&doBuf[doNumber].buffer_in_do+1),  VARDB_TYPEUHEX);
        }
        if (LowName != NULL)
        {
            vardb_addvar(LowName,   "", "Low 16 di ch",  1000, (short *) &doBuf[doNumber].buffer_in_do, VARDB_TYPEUHEX);
        }
        retval = doNitem;
        doNitem++;
    }
    else
    {
        /* Non c'e' piu' spazio, lo segnala */
        retval = -1;
        consolle_print("no space to configure di number %d\n",doNumber);
    }
    return(retval);
}

/***********************************************/
/* Restituisce il numero di schede DO allocate */
/***********************************************/
unsigned short dodrv_numDo(void)
{
    return(doNitem);
}


/************************/
/* Resetta la scheda DO */
/************************/
void dodrv_reset(unsigned short doNumber)
{
    if(doNumber < doNitem)
    {
        doBuf[doNumber].buffer_in_do  = 0;
        doBuf[doNumber].buffer_out_do = 0;
        dodrv_ResetChannels(doNumber,0xFFFFFFFFL);

        wrbitbuffer(doNumber,2,1);              /* tutti i canali spenti e scheda spenta */
        dodrv_output(doNumber);
        DELAY;
        wrbitbuffer(doNumber,2,0);              /* tutti i canali spenti e scheda accesa */
        dodrv_output(doNumber);
    }
    else
    {
        consolle_print("\n Accesso a scheda inesistente\n");
    }
}

/**************************************************/
/* Esegue i test di funzionamento della scheda DO */
/**************************************************/
void dodrv_test(unsigned short doNumber)
{
    if(doNumber < doNitem)
    {
        doBuf[doNumber].BoardStatus = 0;
        /* Test di colloquio VME */
        vme_ko = 0;

        dummy = *doBuf[doNumber].BaseAddressRd;
        dummy = *doBuf[doNumber].BaseAddressRd;
        dummy = *doBuf[doNumber].BaseAddressRd;
        dummy = *doBuf[doNumber].BaseAddressRd;

        if (vme_ko)
        {
            doBuf[doNumber].BoardStatus |= DODRV_STATUSVMEFAIL;
        }

        dodrv_reset(doNumber);
    }
    else
    {
        consolle_print("\n Accesso a scheda inesistente funzione test\n");
    }
}

/***********************/
/* Posiziona un canale */
/***********************/
void dodrv_SetChannelCommand(unsigned short doNumber,unsigned short ch, unsigned short cmd)
{
    if(doNumber < doNitem)
    {
          if(ch>0 && ch<20)
          {
              if(cmd)
              {
                  doBuf[doNumber].AppOutBuffer |=  (0x1L << (ch+3));
              }
              else
              {
                  doBuf[doNumber].AppOutBuffer &= ~(0x1L << (ch+3));
              }
          }
    }
    else
    {
        consolle_print("\n Accesso a scheda inesistente \n");
    }
}

/****************************************/
/* Posiziona il forzamento ad un canale */
/* state = 0 -> Forza a 0               */
/* state = 1 -> Forza a 1               */
/* state =-1 -> Elimina forzamento      */
/****************************************/
void dodrv_SetChannelForcing(unsigned short doNumber,unsigned short ch, unsigned short frc)
{
    if(doNumber < doNitem)
    {
         if(ch>0 && ch<20)
         {
             unsigned long ltemp;
             ltemp = (0x1L << (ch+3));

             switch(frc)
             {
                 case 0:
                     /* Deve forzare a 0 */
                     doBuf[doNumber].Force0Buffer |=  ltemp;
                     doBuf[doNumber].Force1Buffer &= ~ltemp;
                 break;

                 case 1:
                     /* Deve forzare a 0 */
                     doBuf[doNumber].Force0Buffer &= ~ltemp;
                     doBuf[doNumber].Force1Buffer |=  ltemp;
                 break;

                 default:
                     /* Deve eliminare il forzamento */
                     doBuf[doNumber].Force0Buffer &= ~ltemp;
                     doBuf[doNumber].Force1Buffer &= ~ltemp;
                 break;
             }
         }
    }
    else
    {
        consolle_print("\n Accesso a scheda inesistente \n");
    }
}

/*******************************/
/* Legge lo stato di un canale */
/****************************************************/
/* RetVal = 0 -> Stato del canale = 0               */
/* RetVal = 1 -> Stato del canale = 1               */
/* state  =-1 -> Errore: scheda inesistente         */
/****************************************************/
short dodrv_GetChannelStatus(unsigned short doNumber,unsigned short ch)
{
    short RetVal;

    if(doNumber < doNitem)
    {
        if(ch>0 && ch<20)
        {
            RetVal = ((doBuf[doNumber].buffer_in_do & (0x1L << (ch+3))) == 0);
        }
        else
        {
            RetVal = 0;
        }
     }
     else
     {
        consolle_print("\n Accesso a scheda inesistente \n");
        RetVal = -1;
     }
     return(RetVal);
}

/*****************************************/
/* Legge il comando inviato ad un canale */
/*****************************************/
short dodrv_GetChannelCommand(unsigned short doNumber,unsigned short ch)
{
    short RetVal;
    if(doNumber < doNitem)
    {
        if(ch>0 && ch<20)
        {
            RetVal = ((doBuf[doNumber].AppOutBuffer & (0x1L << (ch+3))) != 0);
        }
        else
        {
            RetVal = 0;
        }
    }
    else
    {
        consolle_print("\n Accesso a scheda inesistente \n");
        RetVal = -1;
    }
    return(RetVal);
}

/*********************************************/
/* Legge lo stato di forzamento di un canale */
/* Restituisce:                              */
/* 0  = Forzato a 0                          */
/* 1  = Forzato a 1                          */
/* -1 = Non Forzato                          */
/*********************************************/
short dodrv_GetChannelForcing(unsigned short doNumber,unsigned short ch)
{
    short RetVal;

    RetVal = -1;
    if(doNumber < doNitem)
    {
           if(ch>0 && ch<20)
           {
               if ((doBuf[doNumber].Force0Buffer & (0x1L << (ch+3))) != 0)
               {
                   RetVal = 0;
               }
               if ((doBuf[doNumber].Force1Buffer & (0x1L << (ch+3))) != 0)
               {
                   RetVal = 1;
               }
           }
    }
    else
    {
        consolle_print("\n Accesso a scheda inesistente \n");
        RetVal = -1;
    }
    return(RetVal);
}

/************************************/
/* Scrive il buffer sulla scheda DO */
/************************************/
void dodrv_output(unsigned short doNumber)
{
    if(doNumber < doNitem)
    {
       if (!(doBuf[doNumber].BoardStatus & DODRV_STATUSVMEFAIL))
       {
           /* Calcola la word di uscita considerando anche i forzamenti */
           doBuf[doNumber].buffer_out_do = ~doBuf[doNumber].ResetBuffer &
                                            (doBuf[doNumber].AppOutBuffer | doBuf[doNumber].Force1Buffer)&
                                            ~doBuf[doNumber].Force0Buffer;
           doBuf[doNumber].ResetBuffer = 0L;

           *(doBuf[doNumber].BaseAddressWr)   =  doBuf[doNumber].buffer_out_do & 0xFFL;
           *(doBuf[doNumber].BaseAddressWr+1) = (doBuf[doNumber].buffer_out_do & 0xFF00L) >> 8;
           *(doBuf[doNumber].BaseAddressWr+2) = (doBuf[doNumber].buffer_out_do & 0xFF0000L) >> 16;
       }
    }
    else
    {
        consolle_print("\n Accesso a scheda inesistente \n");
    }
}

/***********************************/
/* Legge il buffer dalla scheda DO */
/***********************************/
void dodrv_input(unsigned short doNumber)
{
    unsigned long temp;
    unsigned long inDo = 0;
    static unsigned long inDoPrec;
    static unsigned long fail1prec;
    unsigned long fail1 = 0;
    if(doNumber < doNitem)
    {
        if (!(doBuf[doNumber].BoardStatus & DODRV_STATUSVMEFAIL))
        {
            temp = *(doBuf[doNumber].BaseAddressRd) & 0xFF;
            inDo |= (unsigned long)temp;
            temp = *(doBuf[doNumber].BaseAddressRd+1) & 0xFF;
            inDo |= (temp << 8);
            temp = *(doBuf[doNumber].BaseAddressRd+2) & 0xFF;
            inDo |= (temp << 16);
            doBuf[doNumber].buffer_in_do = (doBuf[doNumber].buffer_in_do & (inDo | inDoPrec)) |
                                           (inDo & inDoPrec);          /* funzione di stabilità del canale di uscita     */
            inDoPrec = inDo;                                           /* se il buffer è già 1 prenso l'OR tra lo stato  */
                                                                       /* attuale e quello precedente                    */
                                                                       /* se il buffer è zero, prendo l'AND tra lo stato */
                                                                       /* attuale e quello precedente                    */
                                                                       /* cioè richiedo che il bit alto persista per due */
                                                                       /* cicli                                          */
            /* Calcola le word di fail */
            doBuf[doNumber].Fail0Buffer |= doBuf[doNumber].FailEnable &
                                          (~doBuf[doNumber].buffer_out_do & ~doBuf[doNumber].buffer_in_do);
            fail1 |= doBuf[doNumber].FailEnable &
                     (doBuf[doNumber].buffer_out_do  &  doBuf[doNumber].buffer_in_do);
            doBuf[doNumber].Fail1Buffer = (doBuf[doNumber].Fail1Buffer & (fail1 | fail1prec)) |
                                        (fail1 & fail1prec);/* funzione di stabilità del canale di uscita     */
                                                            /* se il buffer è già 1 prenso l'OR tra lo stato  */
                                                            /* attuale e quello precedente                    */
                                                            /* se il buffer è zero, prendo l'AND tra lo stato */
                                                            /* attuale e quello precedente                    */
                                                            /* cioè richiedo che il bit alto persista per due */
                                                            /* cicli                                          */
            fail1prec = fail1;
            if (doBuf[doNumber].Fail0Buffer || doBuf[doNumber].Fail1Buffer)
            {
                doBuf[doNumber].BoardStatus |= DODRV_STATUSCHFAIL;
            }

            /* Controlla l'allarme alimentatore */
            if (rdbitbuffer(doNumber,0))
            {
                doBuf[doNumber].BoardStatus |= DODRV_STATUSSUPPLYFAIL;
            };
        }
    }
    else
    {
        consolle_print("\n Accesso a scheda inesistente \n");
    }
}

/*************************************************************/
/* Imposta un filtro di selezione su cui controllare il FAIL */
/*************************************************************/
void dodrv_SetChannelFailEnable(unsigned short doNumber,unsigned short ch, unsigned short enable)
{
    if(doNumber < doNitem)
    {
         if(ch>0 && ch<20)
         {
             if(enable)
             {
                 doBuf[doNumber].FailEnable |=  (0x1L << (ch+3));
             }
             else
             {
                 doBuf[doNumber].FailEnable &= ~(0x1L << (ch+3));
             }
         }
    }
     else
    {
        consolle_print("\n Accesso a scheda inesistente \n");
    }
}

/********************************************************/
/* Legge lo stato di abilitazione del FAIL di un canale */
/********************************************************/
short dodrv_GetChannelFailEnable(unsigned short doNumber,unsigned short ch)
{
    short RetVal;
    if(doNumber < doNitem)
        {
        if(ch>0 && ch<20)
        {
            RetVal = ((doBuf[doNumber].FailEnable & (0x1L << (ch+3))) != 0);
        }
        else
        {
            RetVal = 0;
        }
    }
    else
    {
        consolle_print("\n Accesso a scheda inesistente \n");
        RetVal = -1;
    }
    return(RetVal);
}


/*********************************/
/* Imposta i canali da resettare */
/*********************************/
void dodrv_ResetChannels(unsigned short doNumber,unsigned long lReset)
{
   if(doNumber < doNitem)
   {
        doBuf[doNumber].ResetBuffer = lReset & 0x007FFFF0L;
        doBuf[doNumber].Fail0Buffer &= ~doBuf[doNumber].ResetBuffer;
        doBuf[doNumber].Fail1Buffer &= ~doBuf[doNumber].ResetBuffer;
        doBuf[doNumber].BoardStatus &= ~DODRV_STATUSCHFAIL;
   }
   else
   {
        consolle_print("\n Accesso a scheda inesistente \n");
   }
}

/*************************************/
/* Lettura immagine interna stato DO */
/*************************************/
unsigned long dodrv_read(unsigned short doNumber)
{
   unsigned long retval;
   if(doNumber < doNitem)
   {
       retval = doBuf[doNumber].buffer_in_do;
   }
   else
   {
        consolle_print("\n Accesso a scheda inesistente \n");
        retval = -1;
   }
   return(retval);
}

/*************************************/
/* Restituisce lo stato della scheda */
/*************************************/
short dodrv_status(unsigned short doNumber)
{
   short retval;
   if(doNumber < doNitem)
   {
       retval = doBuf[doNumber].BoardStatus;
   }
   else
   {
        consolle_print("\n Accesso a scheda inesistente \n");
        retval = -1;
   }
   return(retval);
}

/**************************************/
/* Stampa a video il codice di errore */
/**************************************/
void dodrv_StatusPrint(unsigned short doNumber)
{
   if(doNumber < doNitem)
   {
        if(!doBuf[doNumber].BoardStatus)
        {
            consolle_print("OK\n");
        }

        if(doBuf[doNumber].BoardStatus & DODRV_STATUSVMEFAIL)
        {
            consolle_print("VME ACCESS FAILED\n");
        }

        if(doBuf[doNumber].BoardStatus & DODRV_STATUSSUPPLYFAIL)
        {
            consolle_print("SUPPLY FAIL\n");
        }

        if(doBuf[doNumber].BoardStatus & DODRV_STATUSCHFAIL)
        {
            consolle_print("CH FAIL\n");
        }
   }
   else
   {
        consolle_print("\n Accesso a scheda inesistente \n");
   }
}

/************************************************/
/* Restituisce il buffer delle protezioni Fail0 */
/************************************************/
unsigned long dodrv_Fail0(unsigned short doNumber)
{
    unsigned long retval;
   if(doNumber < doNitem)
   {
       retval = doBuf[doNumber].Fail0Buffer;
   }
   else
   {
        consolle_print("\n Accesso a scheda inesistente \n");
        retval = -1;
   }
   return(retval);

}

/************************************************/
/* Restituisce il buffer delle protezioni Fail1 */
/************************************************/
unsigned long dodrv_Fail1(unsigned short doNumber)
{
        unsigned long retval;
   if(doNumber < doNitem)
   {
       retval = doBuf[doNumber].Fail1Buffer;
   }
   else
   {
        consolle_print("\n Accesso a scheda inesistente \n");
        retval = -1;
   }
   return(retval);
}

/***************************************/
/* Legge lo stato di Fail di un canale */
/* Restituisce:                        */
/* 0 = Nessun Fail                     */
/* 1 = Fail 0                          */
/* 2 = Fail 1                          */
/* 3 = Fail 0 + 1                      */
/***************************************/
short dodrv_GetChannelFailure(unsigned short doNumber,unsigned short ch)
{
    short RetVal;

    RetVal = 0;
   if(doNumber < doNitem)
   {
         if(ch>0 && ch<20)
         {
             if ((doBuf[doNumber].Fail0Buffer & (0x1L << (ch+3))) != 0)
             {
                 RetVal |= 1;
             }
             if ((doBuf[doNumber].Fail1Buffer & (0x1L << (ch+3))) != 0)
             {
                 RetVal |= 2;
             }
         }
   }
   else
   {
        RetVal = -1;
   }
   return(RetVal);

}



/***********************************/
/* Imposta lo stato on/off del LED */
/***********************************/
void dodrv_led(unsigned short doNumber,short on_off)
{
    wrbitbuffer(doNumber,0,on_off);
}


/****************************************************/
/* Scrive lo stato di un bit nel buffer immagine DO */
/****************************************************/
static void wrbitbuffer(unsigned short doNumber,unsigned short nbit, unsigned short stato)
{
   if (doNumber < doNitem)
   {
        if (stato)
        {
            doBuf[doNumber].AppOutBuffer |= (0x1L << nbit);
        }
        else
        {
            doBuf[doNumber].AppOutBuffer &= ~(0x1L << nbit);
        }
    }
}

/****************************************************/
/* Legge lo stato di un bit nel buffer di input DO  */
/****************************************************/
static long rdbitbuffer(unsigned short doNumber,unsigned short nbit)
{
    long retval;

    if (doNumber < doNitem)
    {
        retval = (doBuf[doNumber].buffer_in_do & (0x1L << nbit)) != 0;
    }
    else
    {
        retval = -1;
    }
    return(retval);
}


/* ****************************************************************************
    $Log: doDrv.c,v $
    Revision 1.5  2004/11/04 15:21:46  mungiguerrav
    2004/ott/29,ven 15:50          Napoli       Mungi\Nappo
    - Si definisce localmente e non in dodrv.h la macro DELAY (2000 cicli)
    - Si rendono static le routine wrbitbuffer e rdbitbuffer:
        - Vi si applicano le regole di codifica su parentesi e incolonnamento
        - Si elimina il +3 a nbit introdotto da Calabrese anche in queste
          routine di servizio, mentre il +3 serve solo nelle routine di interfaccia
          verso l'applicativo, il quale passa il canale 'ch' e non 'nbit'

    Revision 1.4  2004/10/25 09:48:32  mungiguerrav
    2004/ott/25,lun 11:00          Napoli       Mungi
    - Si effettuano correzioni di incolonnamento e di parentesi
    - Si corregge errore su doppio test su HighName: il secondo è su LowName

    Revision 1.3  2004/10/25 09:43:44  mungiguerrav
    2004/ott/25,lun 10:15          Napoli       Mungi
    - Si riporta sul tronco principale le correzioni effettuate nei branch
      didrv_nappoc e work_calabresea  su gestione più schede do
    - Si aggiungono keyword CVS e storia passata

    Revision 1.2.6.1  2004/10/13 11:47:48  calabresea
    introduzione DO multipla

    Revision 1.2.4.1  2004/10/01 10:01:31  nappoc
    2004/set/29,mer 14:35    Napoli    Nappo
    - Si include vardb.h
    - Si agg. due variabili in vardb che mappano parte alta e bassa del buffer della DO

 ---------------    Fase transitoria tra *.mod e CVS    -------------
    Revision 1.2      2004/06/15 11:26:25     buscob
    02.08   2003/nov/21,ven         Napoli                  Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso.

    Revision 1.1      2002/05/21 14:52:24   accardol
    - Inserimento in CVS
    Revision 1.1.1.1  2002/05/21 14:52:24   accardol
    - Inserimento in CVS in branch STP
02.07   2002/mag/21,mar         Napoli                  Busco/Mungi
        - Si elim. azzeramenti di Fail0Buffer, Fail1Buffer e BoardStatus nella funzione dodrv_reset();
        - Si inverte retval di _GetChannelStatus()
        - Si sost. funz. _SetFailEnable() con   _SetChannelFailEnable()
                                          e     _GetChannelFailEnable()
 ----------------    Prima di CVS    -----------------------------
02.06   2002/mag/20,lun         Napoli                  Busco
        - Nell'operazione di spostamento da AIACE\SRC a LIB\SRC, si
          inseriscono le modifiche 02.04bs e 02.05bs fatte da Busco fuori UFAP
        - Si elim. warning Borland, agg. L a 0xFFFFFFFF
02.04   2002/mar/28,gio         Napoli                  Accardo
        - Si corregge errore segnalato da Busco in _input() sostituendo le righe
                temp = *(BaseAddressRd+1) & 0xFF;
          con   temp = *(BaseAddressRd+2) & 0xFF;
02.05bs 2002/mar/06,mer         Napoli                  Busco
        - Si cambia int in short
        - Si rinominano le funzioni esportate per maggiore chiarezza
02.04bs 2002/feb/05,mar         Napoli                  Busco
        - Si elim.  include libisv.h    perche' inutile
        - Si gestiscono i forzamenti dei canali di uscita indipendentemente dall'applicativo
        - Si agg. vars Force0Buffer e Force1Buffer
        - Si agg. funzione dodrv_ForceChannel(...) per forzamento canali
        - Si agg. funzione dodrv_GetChannelCmd(..)
        - Si agg. funzione dodrv_GetChannelForcing(..)
        - Si sost. word Fail0Buffer e Fail1Buffer a buffer_prz per gestire il fail0 e 1
        - Si elim. funzioni dodrv_prz(..), dodrv_alarm(..)
        - Si agg. funzioni dodrv_Fail0(..) e _Fail1(..)
        - Si agg. funzione dodrv_GetFail(..)
        - Si sost. var BoardStatus a ErrorWord e si agg. codifica di ALARM e CHFAIL
        - Si agg. funzione dodrv_SetFailEnable(..)
        - Si agg. funzione dodrv_ResetChannels(..)
        - Si sost. funzione _StatusPrint(..)  a  _errorprint(..)
        - Si sost. funzione _status(void)     a  _error(..)
02.03   2001/dic/14,ven         Napoli                  Mungi
        - Si riordinano gli include secondo le regole
2.02    2001/mag/10,gio         Napoli                  Accardo
        - Si mod. BaseAddressRd e BaseAddressWr da far a huge.
        - Si sost.  dodrv_init(void far  *BaseRd,void far  *BaseWr)
          con       dodrv_init(void huge *BaseRd,void huge *BaseWr)
2.01    2001/apr/28,sab         Napoli                  Accardo
        - Si sostituisce puntatore unico all'indirizzo base della scheda
          BaseAddress con due puntatori:
                BaseAddressRd per accesso in lettura
                BaseAddressWr per accesso in scrittura
          I due indirizzi coincidono per la scheda reale, mentre sono diversi
          per la scheda simulata.
2.00    2001/feb/28,mer         Napoli                  Accardo
        - Si sposta da \SWISV\TLRV\SRC in \SWISV\AIACE\SRC.
----------------------Storia precedente di TLRV-----------------------
2.00    2001/gen/22,lun         Napoli                  Busco
        - Si cambia nome da do.c a dodrv.c e quindi a tutte le funzioni esportate
        - Si elim. funzioni dodrv_rdbitbuffer() e dodrv_rdbitbuffe()
        - Si agg. funzioni dodrv_SetChannel(..) e dodrv_GetChannel(..)
01.07   2000/feb/07,lun - Si sposta il comando do nel nuovo modulo docmd.c
01.06   1998/nov/05,gio - Si agg. funzione do_read() per lettura immagine stato DO
01.05   1998/lug/31,mar - Si inizializza do_ok = 0
01.04   1998/lug/14,mar - Si agg. funzione do_prz()
01.03   1998/giu/09,mar - Il comando do reset chiama do_reset() e non piu' do_init()
01.02   1998/mag/25,lun - Si agg. funzione do_alarm() per lettura stato alarm
                        - Si agg. funzione do_led(int) per accensione/spegnimento led
01.01   1998/mar/30,lun - La funzione do_input restituisce la parola letta
                        - Si agg. visualizzazione dello stato di allarme
01.00   1997/ott/17,ven - Creazione a partire da DIDO.C
*/

