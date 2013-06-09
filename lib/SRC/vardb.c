/****************************************************************************************/
/* 2004/ott/25,lun 13:05                                         \SWISV\LIB\SRC\vardb.c */
/*                                                                                      */
/* Module: VARDB                                                                        */
/* Description: Database delle variabili da monitorare                                  */
/*                                                                  IMPLEMENTATION FILE */
/****************************************************************************************/
/*
    $Date: 2004/10/25 11:29:49 $          $Revision: 1.8 $
    $Author: mungiguerrav $
*/


#include "string.h"     /* gestione stringhe */
#include "ctype.h"
#include "stdlib.h"
#include "stdio.h"
#include "vardb.h"
#include "query.h"

#define ATRTERM4

/**********************************/
/* formati di stampa per la query */
/**********************************/
#define FRMTQUERY_INT   "%d "     /* 'i' */
#define FRMTQUERY_UINT  "%u "     /* 'u' */
#define FRMTQUERY_HEX   "%u "     /* 'x' */
#define FRMTQUERY_CHAR  "%d "     /* 'c' */

#define VARDB_MAXREPLYLEN 256
#define RESETTIMEOUT      500

struct  varEntry        *vardb;             /* Database delle variabili             */
static  unsigned short  vardbSize;          /* Dimensione memoria per Database      */
static  unsigned short  vardbUsed;          /* Memoria usata                        */
static  unsigned short  vardbNitem;         /* Numero di variabili presenti         */
static  unsigned short  cntTimeOut;         /* Timer per disattivazione output      */
static  unsigned short  vardbBitNitem;      /* Num. di variabili a bit presenti     */

static  char     flgPrint = 0;
static  unsigned short  SampleStep = 10;    /* Fattore di decimazione campioni      */
static  unsigned short  numvarquery = 0;    /* Num. di var. richieste nella query   */
static  unsigned short  dtSample = 10;      /* Passo di campionamento base in ms    */

/*********************************************/
/* Prototipi delle funzioni locali di modulo */
/*********************************************/
static void gestquery_VN(char* arg);
static void gestquery_VS(char* arg);
static void gestquery_VT(char* arg);
static void gestquery_VI(char* arg);
static void gestquery_VV(char* arg);
static void gestquery_VSDT(char* arg);
static void gestquery_VOFF(char* arg);
static void gestquery_VBN(char* arg);
static void gestquery_VBI(char* arg);

/* Prototipi delle funzioni locali di modulo */
static void parsestr(char* stringa, unsigned short* numToken, char* token[], short maxnumToken);

/* Temporaneamente il DB per la variabili bit e' statico */
#define MAXVARBIT   200

struct  varBitEntry     vardbBit[MAXVARBIT];

/*********************************************************/
/* Inizializza il gestore del database delle variabili   */
/* - Restituisce 1 se tutto OK                           */
/* - Restituisce 0 se non riesce ad aggiungere i comandi */
/*********************************************************/
short vardb_init(char *buff, unsigned short size, unsigned short dt)
{
    short RetVal;

    RetVal          = 1;
    vardb           = (struct varEntry *) buff;
    vardbSize       = size;
    vardbUsed       = 0;
    vardbNitem      = 0;
    cntTimeOut      = 0;
    dtSample        = dt;

    vardbBitNitem   = 0;

    RetVal = RetVal && query_addcmd("VN",   gestquery_VN);
    RetVal = RetVal && query_addcmd("VS",   gestquery_VS);
    RetVal = RetVal && query_addcmd("VT",   gestquery_VT);
    RetVal = RetVal && query_addcmd("VI",   gestquery_VI);
    RetVal = RetVal && query_addcmd("VV",   gestquery_VV);
    RetVal = RetVal && query_addcmd("VSDT", gestquery_VSDT);
    RetVal = RetVal && query_addcmd("VOFF", gestquery_VOFF);
    RetVal = RetVal && query_addcmd("VBN",  gestquery_VBN);
    RetVal = RetVal && query_addcmd("VBI",  gestquery_VBI);
    return(RetVal);
}

/************************************************************/
/* Restituisce il numero di variabili presenti nel database */
/************************************************************/
unsigned short vardb_numvar(void)
{
    return(vardbNitem);
}

/************************************************************************/
/* Gestione del comando: VN                                             */
/* Richiesta numero di variabili presenti nel database                  */
/* Richiesta:  "VN"                                                     */
/* Risposta:   "VN OK n" (dove n e' il numero di var. presenti nel DB   */
/************************************************************************/
static void gestquery_VN(char* arg)
{
    char tempstr[20];

    arg = arg;
    sprintf(tempstr,"VN OK %u",vardbNitem);

    query_AppendReply(tempstr);
}

/***************************************************************************/
/* Gestione del comando: VBN                                               */
/* Richiesta numero di variabili bit presenti nel database                 */
/* Richiesta:  "VBN"                                                       */
/* Risposta:   "VBN OK n" (dove n e' il numero di var. bit presenti nel DB */
/***************************************************************************/
static void gestquery_VBN(char* arg)
{
    char tempstr[20];

    arg = arg;
    sprintf(tempstr,"VBN OK %u",vardbBitNitem);

    query_AppendReply(tempstr);
}

/*********************************************************************/
/* Gestione del comando: VS                                          */
/* Impostazione variabili da campionare                              */
/* Richiesta:  "VS i varname_i"                                      */
/* Risposta1:  "VS OK i"                                             */
/* Risposta2:  "VS ??"                                               */
/*                                                                   */
/* Nota: nella richiesta i e' il numero della variabile che si vuole */
/*       impostare                                                   */
/*********************************************************************/
static void gestquery_VS(char* arg)
{
    char            *Tokens[4];             /* puntatori ai nomi delle var query    */
    unsigned short  NumToken;               /* Numero dei token presenti in arg     */
    short           ReplyOK;
    short           NumVar;
    char            tempstr[20];

    ReplyOK     = 0;
    NumVar      = 0;

    /* Individua i singoli token nella stringa di query */
    parsestr(arg,&NumToken,&Tokens[0],4);

    if ( NumToken == 2 )
    {
        /* Ci sono due argomenti (numero della var e nome) */

        /* Interpreta il primo parametro come numero della variabile */
        NumVar  = atoi(Tokens[0]);
        if (NumVar>0 && NumVar<=vardbNitem)
        {
            short NotFound;

            NotFound    = 0;

            /* Cerca nel database la variabile richiesta */
            if ((vardb[NumVar-1].queryvar = vardb_findvar(Tokens[1])) == NULL)
            {
                NotFound = 1;
            }

            /* Se l'indice della variabile e' maggiore del numero di variabili */
            /* da restituire aggiorna quest'ultimo */
            if (NumVar > numvarquery)
            {
                numvarquery = NumVar;
            }

            if (!NotFound)
            {
                ReplyOK = 1;
            }
        }
        else
        {
            /* deve disabilitare tutte le variabili */
            numvarquery = 0;
            ReplyOK = 1;
        }
    }

    if (ReplyOK)
    {
        sprintf(tempstr,"VS OK %u",NumVar);
    }
    else
    {
        sprintf(tempstr,"VS ?? %u",NumVar);
    }

#ifdef ATRTERM4
    if (query_status() == 2)
    {
#endif
        query_AppendReply(tempstr);
#ifdef ATRTERM4
    }
#endif
}


/*********************************************************************/
/* Gestione del comando: VT                                          */
/* Impostazione tempo di campionamento                               */
/* Richiesta:  "VT n"                                                */
/* Risposta:   "VT n"                                                */
/*                                                                   */
/* Nota: nella richiesta n e' il numero di chiamate vardb_sample da  */
/*       aspettare tra due campionamenti                             */
/*       nella risposta n e' il valore attuale                       */
/*********************************************************************/
static void gestquery_VT(char* arg)
{
    short   tempSampleStep;
    char    tempStr[30];

    if (arg != NULL)
    {
        tempSampleStep = atoi(arg);

        if (tempSampleStep > 0 && tempSampleStep <= 1000)
        {
            SampleStep = tempSampleStep;
        }
    }

#ifdef ATRTERM4
    if (query_status() == 2)
    {
#endif
        sprintf(tempStr,"VT %u",SampleStep);
        query_AppendReply(tempStr);
#ifdef ATRTERM4
    }
#endif
}

/*********************************************************************/
/* Gestione del comando: VV                                          */
/* Richiede l'output dei campioni                                    */
/* Richiesta:  "VV"                                                  */
/* Risposta:   "VV Val1 Val2 ... ValN"                               */
/*********************************************************************/
static void gestquery_VV(char* arg)
{
    arg         = arg;
    vardb_ResetTimeout();
}

/*********************************************************************/
/* Gestione del comando: VOFF                                        */
/* Interrompe l'output dei campioni                                  */
/* Richiesta:  "VOFF"                                                */
/* Risposta:   "VOFF"                                                */
/*********************************************************************/
static void gestquery_VOFF(char* arg)
{
    arg         = arg;
    cntTimeOut  = 0;
    numvarquery = 0;
    query_AppendReply("VOFF");
}

/*********************************************************************/
/* Gestione del comando: VSVT                                        */
/* Interrogazione passo di campionamento base                        */
/* Richiesta:  "VSDT"                                                */
/* Risposta:   "VSDT ms"                                             */
/*********************************************************************/
static void gestquery_VSDT(char* arg)
{
    char    tempStr[30];

    arg         = arg;
    sprintf(tempStr,"VSDT %u",dtSample);
    query_AppendReply(tempStr);
}

/***************************************************************************/
/* Gestione del comando: VI                                                */
/* Richiesta di informazioni su una variabile                              */
/* Richiesta:  "VI n"                                                      */
/* Risposta1:  "VI n OK nome unit… scala descr flags"                      */
/* Risposta2:  "VI ??"                                                     */
/*                                                                         */
/* Nota: nella richiesta n e' il numero della variabile di cui si vogliono */
/*       le info                                                           */
/*       I dati sono separati da tab                                       */
/***************************************************************************/
static void gestquery_VI(char* arg)
{
    char    tempstr[100];
    short   NumVar;
    short   ReplyOK;

    ReplyOK         = 0;

    if (arg != NULL)
    {
        NumVar = atoi(arg);

        /* Controlla se il numero della variabile richiesta esiste */
        if (NumVar > 0 && NumVar <= vardbNitem)
        {
            /* Forma la stringa di dati */
            sprintf(tempstr,"VI %u OK %.16s\t%.8s\t%d\t%.50s\t%u",
                    NumVar,
                    vardb[NumVar-1].nomeVar,
                    vardb[NumVar-1].unit,
                    vardb[NumVar-1].scale,
                    vardb[NumVar-1].descr,
                    vardb[NumVar-1].flags);

            /* La invia a query */
            query_AppendReply(tempstr);
            ReplyOK = 1;
        }
    }

    if(!ReplyOK)
    {
        query_AppendReply("VI ??");
    }
}

/***************************************************************************/
/* Gestione del comando: VBI                                               */
/* Richiesta di informazioni su una variabile bit                          */
/* Richiesta:  "VBI n"                                                     */
/* Risposta1:  "VBI n OK nome descr BWIndex Nbit"                          */
/* Risposta2:  "VBI ??"                                                    */
/*                                                                         */
/* Nota: nella richiesta n e' il numero della variabile di cui si vogliono */
/*       le info                                                           */
/*       I dati sono separati da tab                                       */
/***************************************************************************/
static void gestquery_VBI(char* arg)
{
    char    tempstr[100];
    short   NumVar;
    short   ReplyOK;

    ReplyOK     = 0;

    if (arg != NULL)
    {
        NumVar = atoi(arg);

        /* Controlla se il numero della variabile richiesta esiste */
        if (NumVar > 0 && NumVar <= vardbBitNitem)
        {
            /* Forma la stringa di dati */
            sprintf(tempstr,"VBI %u OK %.8s\t%.50s\t%u\t%u",
                    NumVar,
                    vardbBit[NumVar-1].nomeVar,
                    vardbBit[NumVar-1].descr,
                    vardbBit[NumVar-1].BWIndex,
                    vardbBit[NumVar-1].flags
                    );

            /* La invia a query */
            query_AppendReply(tempstr);
            ReplyOK = 1;
        }
    }

    if(!ReplyOK)
    {
        query_AppendReply("VBI ??");
    }
}

/*******************************************************************/
/* Campiona le variabili selezionate                               */
/* Restituisce:                                                    */
/* 1 se il campionamento e' stato effettuato,                      */
/* 0 se il timer non e' ancora scattato                            */
/*******************************************************************/
short vardb_sample(void)
{
            short           i;
            short           retval;
    static  unsigned short  cntQuerySample = 10;    /* Contatore per Decimazione campioni   */

    /* Decrementa il timer per la disattivazione del query output */
    if (cntTimeOut)
    {
        cntTimeOut--;
    }

    if (cntQuerySample>0)
    {
        cntQuerySample--;
    }

    if (!cntQuerySample)
    {
        cntQuerySample = SampleStep;

        /* Campiona le variabili selezionate */
        for(i=0; i < numvarquery; i++)
        {
            if (vardb[i].queryvar != NULL)
            {
                switch(vardb[i].queryvar->flags & VARDB_TYPEMASK)
                {
                    case VARDB_TYPEUSHORT:
                    case VARDB_TYPESHORT:
                    case VARDB_TYPEUHEX:
                        vardb[i].queryvar->Sample = *((short far*)vardb[i].queryvar->pvar);
                    break;

                    case VARDB_TYPECHAR:
                        vardb[i].queryvar->Sample = *((char far*)vardb[i].queryvar->pvar);
                    break;
                    default:
					break;
                }
            }
        }

        flgPrint    = 1;
        retval      = 1;
    }
    else
    {
        retval = 0;
        }

    return(retval);
}

/***********************************************************************/
/* Questa funzione deve essere chiamata dall'applicativo in Background */
/*   - Controlla se deve essere emessa una reply ed in caso positivo   */
/*     la passa a query                                                */
/***********************************************************************/
short vardb_replystr(void)
{
    char    replystr[VARDB_MAXREPLYLEN];
    char    tempstr[20];

    /* Controlla se sono stati prodotti campioni da inviare */
    if(flgPrint && cntTimeOut)
    {
        short i;
        short intReplyStrLen;
        short len;

        flgPrint = 0;

#ifdef ATRTERM4
        if(query_status() == 2)
        {
#endif
            replystr[0] = 'V';
            replystr[1] = 'V';
            replystr[2] = ' ';
            replystr[3] = '\0';
            intReplyStrLen = 3;
#ifdef ATRTERM4
        }
        else
        {
            replystr[0] = '\0';
            intReplyStrLen = 0;
        }
#endif

        for(i=0; i < numvarquery; i++)
        {
            if (vardb[i].queryvar != NULL)
            {
                switch(vardb[i].queryvar->flags & VARDB_TYPEMASK)
                {
                    case VARDB_TYPESHORT:
                    case VARDB_TYPECHAR:
                         len = sprintf(tempstr,FRMTQUERY_INT,vardb[i].queryvar->Sample);
                    break;
                    case VARDB_TYPEUHEX:
                    case VARDB_TYPEUSHORT:
                         len = sprintf(tempstr,FRMTQUERY_UINT,vardb[i].queryvar->Sample);
                    break;
                    default:
                        len = 0;
                        tempstr[0] = '\0';
					break;
                }
            }
            else
            {
                len = sprintf(tempstr,"?? ");
            }

            /* Ricostruisce la lunghezza della risposta */
            intReplyStrLen += len;

            /* Controlla che non sfondi la lunghezza massima      */
            /* e che ci sia spazio per il carattere di fine query */
            if(intReplyStrLen < VARDB_MAXREPLYLEN)
            {
                strcat(replystr,tempstr);
            }
        }

        query_AppendReply(replystr);
        return(1);
    }
    else
    {
        return(0);
    }
}

/**************************************/
/* Aggiunge una variabile alla lista  */
/* Restituisce:                       */
/*   index - se OK                    */
/*   -1 - se non c'e' piu' spazio     */
/*   -2 - se la variabile esiste gia' */
/**************************************/
short vardb_addvar( const char      *nomeVar,
                    const char      *unit,
                    const char      *descr,
                    short           scale,
                    void far        *pvar,
                    unsigned short  flags )
{
    short   retval;

    /*Controlla se la variabile e' gia' presente nel database */
    if (vardb_findvar((char *)nomeVar) != NULL)
    {
        /* E' stata trovata segnala l'errore */
        retval = -2;
    }
    /* Non e' stata trovata controlla se c'e' spazio per aggiungerla */
    else if (sizeof(struct varEntry) * (vardbNitem+1) < vardbSize)
    {
        vardb_setvar(&(vardb[vardbNitem]),
                     (char *) nomeVar,
                     (char *) unit,
                     (char *) descr,
                     scale,
                     pvar,
                     flags);

        retval = vardbNitem;
        vardbNitem++;
    }
    else
    {
        /* Non c'e' piu' spazio, lo segnala */
        retval = -1;
    }

    return(retval);
}

short vardb_addvarbit(unsigned short BWIndex, char NBit, const char *nomeVar, const char *descr, unsigned short flags)
{
    short           RetVal;
    unsigned short  FinalFlags; /* ricostruisce la word flags combinando il numero di bit con la word flags passatagli */
    unsigned short  tempNBit;

    if (vardbBitNitem < MAXVARBIT)
    {
        /* ricostruisce la word Flags */
        FinalFlags          = flags & 0x0FFF;
        tempNBit            = NBit & 0x0F;
        tempNBit            = tempNBit << 12;
        FinalFlags          = FinalFlags | tempNBit;
        /* aggiunge le informazioni   */
        vardbBit[vardbBitNitem].nomeVar = nomeVar;
        vardbBit[vardbBitNitem].descr   = descr;
        vardbBit[vardbBitNitem].BWIndex = BWIndex;
        vardbBit[vardbBitNitem].flags   = FinalFlags;

        vardbBitNitem++;
        RetVal  = vardbBitNitem;
    }
    else
    {
        /* Non c'e' spazio */
        RetVal = -1;
    }
    return(RetVal);
}


/*************************************************************************/
/* Scandisce una stringa, restituisce il numero di token ed un array di  */
/* puntatori agli stessi                                                 */
/* N.B. la stringa originale viene modoficata ed continua ad essere      */
/*      puntata dai puntatori restituiti                                 */
/*************************************************************************/
static void parsestr(char* stringa, unsigned short* numToken, char* token[], short maxnumToken)
{
    short   i;                      /* indice al carattere della stringa */
    short   stato;                  /* si trova su token = 1; spazio = 0 */
    char    c;

    i           = 0;                /* inizia dal primo carattere della stringa */
    *numToken   = 0;                /* non ha trovato ancora nessun token       */
    stato       = 0;                /* parte dallo stato spazio                 */

    while( ( (c=stringa[i]) != '\0') && (*numToken < maxnumToken) )
    {
        if(stato)
        {
            if(isspace(c))
            {
                stringa[i] = '\0';
                stato   = 0;
            }
        }
        else
        {
            if(isgraph(c))
            {
                token[(*numToken)++] = &stringa[i];
                stato   = 1;
            }
        }
        i++;
    }
}

/*******************************************************************/
/* Cerca una variabile nella lista e restituisce il puntatore alla */
/* struct relativa (NULL se la variabile non e' stato trovata)     */
/*******************************************************************/
struct varEntry* vardb_findvar(char *nomeVar)
{
    short               i;
    struct varEntry*    result;
    short               found;
    short               indexFound;

    i           = 0;
    found       = 0;
    indexFound  = 0;

    while(i<vardbNitem && !found)
    {
        if(strcmp(nomeVar,vardb[i].nomeVar) == 0)
        {
            found       = 1;
            indexFound  = i;
        }

        i++;
    }

    if (found)
    {
        result  = &vardb[indexFound];
    }
    else
    {
        result  = NULL;
    }

    return(result);
}


/*******************************************************************/
/* Cerca una variabile nella lista e restituisce l'indice ad essa  */
/* relativo.                                                       */
/* Restituisce -1 se la variabile non e' stata trovata             */
/*******************************************************************/
short vardb_VarIndex(char *nomeVar)
{
    struct varEntry*    pvar;
    short               RetVal;

    pvar    = vardb_findvar(nomeVar);

    if (pvar == NULL)
    {
        /* La variabile non e' stata trovata nel database */
        RetVal = -1;
    }
    else
    {
        /* Costruisce l'indice a partire dal puntatore */
        RetVal = (pvar-vardb)/sizeof(struct varEntry);
    }
    return(RetVal);
}

void vardb_setvar( struct varEntry* entry,
                     char *nomeVar,
                     char *unit,
                     char *descr,
                    short scale,
                 void far *pvar,
           unsigned short flags)
{
    entry->nomeVar  = nomeVar;
    entry->pvar     = pvar;
    entry->unit     = unit;
    entry->descr    = descr;
    entry->scale    = scale;
    entry->flags    = flags;
}

void vardb_ResetTimeout(void)
{
    cntTimeOut  = RESETTIMEOUT;
}


struct varEntry* query_findvar(char *nomeVar)
{
    return(vardb_findvar(nomeVar));
}


/* ****************************************************************************
    $Log: vardb.c,v $
    Revision 1.8  2004/10/25 11:29:49  mungiguerrav
    2004/ott/25,lun 13:05          Napoli       Mungi
    - Si aggiungono keyword CVS e storia passata
    - Si cita la modifica fatta e ritirata su branch didrv_nappoc solo per suggerire
      che in casi del genere sarebbe preferibile effettuare prima i test e solo
      dopo i commit, che potrebbero rivelarsi inutili (MNG)


    Revision 1.7.2.2  2004/10/01 09:53:03  nappoc
    2004/set/30,gio 13:10  Napoli    Nappo
    - Si torna indietro con le direttive di compilazione aggiunte nella versione del file precedente

    Revision 1.7.2.1  2004/09/28 12:12:03  nappoc
    2004/set/27,lun 14:10    Napoli    Busco\Nappo
    - Si aggiungono direttive di compilazione per la versione di ATRterm nei comandi VOFF e VSDT

    Revision 1.7      2004/08/31 14:25:15     mungiguerrav
    2004/ago/31,mar 16:25        Napoli          Mungi\Busco
    - Si effettua Merge da branch makefile

 ---------------    Fase transitoria tra *.mod e CVS    -------------
    Revision 1.6.2.1  2004/07/14 15:44:25     buscob
02.00   2004.apr.02,ven         Napoli                  Nappo/Busco
        - Si cambia la gestione delle informazioni all'interno della struttura varDB e varDbBit
        - Eliminata la funzione query_addvar presente finora per compatibilita' con le vecchie
          applicazioni.
        - Si incrementa la dimensione della stringa contenente il nome della variabile
          all'interno del comando VI

    Revision 1.6      2004/06/15 12:06:25     buscob
01.09   2003.nov.29,sab             Madrid                  Mungi
        - Si introducono le regole su parentesi e nomi dal manuale SW
        - Si inserisce il suggerimento di Busco su:
                abolizione di "unit"
                sostituzione di "Description Not Available yet" con "Description N/A"
          nella routine di interfaccia query_addvar(..)
01.08   2003.nov.21,ven             Napoli                  Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso

    Revision 1.5      2004/06/15 10:37:01     buscob
01.07   2003.nov.19,mer             Napoli              Nappo
        - si aumenta il numero max di var bit per testare AtrTerm5.0 su variabili bit fittizie

    Revision 1.4      2003/03/03 16:21:09     buscob
    Revision 1.3      2003/03/03 15:15:08     buscob
    (Commit per sperimentare le keyword CVS, come Revision)
    Revision 1.2      2003/03/03 14:40:13     buscob
01.06   2003.feb.03,lun             Napoli              Busco
        - il comando VOFF azzera anche numvarquery in modo da resettare le variabili richieste
01.05   2003.gen.23,gio             Napoli              Busco
        - vardb_addvar restituisce l'indice della variabile nel DB se OK altrimenti un codice
          di errore < 0. Questo serve per poter avere un riferimento alla variabile BitWord
          per poterci associare le variabili bit.
        - Si agg. funzione vardb_addvarbit(..) per la definizione delle variabili bit
01.04   2003.gen.22,mer             Napoli              Busco
        - Si mod. risposta al comando query "VI"
        - Si mod. risposta al comando query "VT"
        - Si agg. comando query "VOFF"

    Revision 1.1      2002/05/17 10:02:14   accardol
    - Inserimento in CVS
    Revision 1.1.1.1  2002/05/17 10:02:14   accardol
    - Inserimento in CVS in branch STP
01.03   2002.mag.17,ven             Napoli              Busco
        - Si agg. funzione vardb_VarIndex(..)

 ----------------    Prima di CVS    -----------------------------
01.02   2002.apr.15,lun             Napoli              Busco
        - Bugfix: gestquery_VS(char* arg) - Si evita di azzerare numvarquery quando non si
          trova una variabile.
        - Si elimina codice commentato con #ifdef NONCOMPILARE rimasto inutilizzato per
          un tempo sufficiente
01.01   2001.dic.03,lun             Napoli              Busco
        - Si corr. bug in gestquery_VS(). Se si chiedevano TUTTE le var registrate da ATRterm
          non se ne otteneva nessuna.
01.00   2001.lug.24,lun             Napoli              Busco
        - Creazione
*/

