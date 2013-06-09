/********************************************************************/
/* 2004/nov/15,lun 11:00                \SWISV\LIB\SRC\trace.c      */
/********************************************************************/
/*
    $Date: 2004/11/16 15:38:15 $          $Revision: 1.4 $
    $Author: mungiguerrav $
*/


#include "string.h"     /* gestione stringhe */
#include "ctype.h"
#include "stdlib.h"
#include "stdio.h"

#include "consio.h"
#include "consolle.h"
#include "vardb.h"
#include "trace.h"
#include "stream.h"

/******************/
/* parametri vari */
/******************/
#define MAXNUMVAR           16      /* numero massimo di variabili da campionare    */

/* Codice di stato */
#define STOP                0       /* trace disabilitato                           */
#define RUN                 1       /* in attesa del trigger                        */
#define TRIGGERED           2       /* e' arrivato il trigger,                      */
                                    /*      aspetta che si completi il buffer       */
#define WRITING             3       /* trace disabilitato,                          */
                                    /*      sta eseguendo la scrittura in memoria   */
#define DONE                4       /* ha finito di scrivere il trace in memoria    */
#define TRACENUMSTATUS      5

/* Codici di trigger */
#define TRIGGER_EQUAL       0       /* Variabile  == TriggerVal                     */
#define TRIGGER_NOTEQUAL    1       /* Variabile  != TriggerVal                     */
#define TRIGGER_LESSTHEN    2       /* Variabile   < TriggerVal                     */
#define TRIGGER_GREATERTHEN 3       /* Variabile   > TriggerVal                     */
#define TRIGGER_ONEBIT0     4       /* (~Variabile & TriggerVal)                    */
#define TRIGGER_ONEBIT1     5       /* (Variabile  & TriggerVal)                    */
#define TRIGGER_ALLBIT0     6       /* (~Variabile & TriggerVal) == TriggerVal      */
#define TRIGGER_ALLBIT1     7       /* (Variabile & TriggerVal)  == TriggerVal      */
#define TRIGGERNUMRELATIONS 8

/* Modo di funzionamento del trigger */
#define AUTO                0       /* Passa nello stato RUN automaticamente        */
#define SINGLE              1       /* Si ferma nello stato STOP                    */
#define TRIGGERNUMMODE      2

struct t_datetime
{
    unsigned short      year;
    unsigned char       month;
    unsigned char       day;
    unsigned char       hour;
    unsigned char       min;
    unsigned char       sec;
    unsigned char       csec;
};

struct t_TraceHeader
{
    struct t_datetime   Time;                   /* Data/ora del trace                   */
    char                NumVar;                 /* Numero di variabili memorizzate      */
    char                NumSample;              /* Numero di campioni                   */
    char                TriggerSample;          /* Campione corrispondente al trigger   */
    char                FirstSample;            /* Primo campione della registrazione   */
    short               VarIndex[MAXNUMVAR];    /* Indici delle variabili memorizzate   */
};


extern  struct varEntry     *vardb;             /* Database delle variabili             */


/********************************/
/* variabili per gestione trace */
/********************************/
static  char* TriggerRelStr[TRIGGERNUMRELATIONS]=
{
    "=","!=","<",">","b0","b1","bb0","bb1"
};

static  char* TriggerModeStr[TRIGGERNUMMODE]=
{
    "auto","single"
};

static  char* TraceStatusStr[TRACENUMSTATUS]=
{
    "STOP","RUN","TRIGGERED","WRITING","DONE"
};

static  unsigned short      traceIstBrk;
static  unsigned short      TraceStep;
static  unsigned short      TraceCount;         /* contatore per rallentamento trace                */

static  unsigned char       NumVar;             /* Numero di variabili di cui si desidera il trace  */
static  unsigned short      NumSample;          /* Numero di campioni                               */
static  unsigned short      SampleCount;        /* Numero di campioni da memorizzare dopo il trigger*/
static  unsigned char       TriggerPos;         /* Posizione del trigger (in %)                     */
static  unsigned short      TriggerPosAbs;      /* Posizione del trigger (in valore assoluto)       */
static  unsigned short      TraceBufferSize;    /* Dimensione dell'intero buffer                    */
static  unsigned short      TraceMatrixSize;    /* Dimensione della matrice di memorizzazione       */
static  unsigned char       Status;             /* Stato di funzionamento                           */
static  unsigned short      TriggerMode;        /* Modo di funzionamento trigger                    */
static  unsigned char       ForceTrigger;       /* Flag utilizzato per forzare il trigger           */
static  struct varEntry*    TriggerVar;         /* puntatore alla variabile trigger                 */
static  unsigned short      TriggerVal;         /* Valore di trigger                                */
static  char                TriggerRelation;    /* Codice operatore di relaz. da usare nel trigger  */

static  struct varEntry*    varTrace[MAXNUMVAR];/* puntatori alle var (alle strutture in vardb)     */
                                                /* di cui si richiede il trace                      */
static  hStream             TraceStream;        /* handler allo stream da usare per la memorizzaz.  */

static  unsigned short      *TraceMatrix;       /* Puntat. all'inizio della zona campioni del buffer*/
static  char                *TraceBuffer;       /* Puntatore all'inizio del buffer                  */

/****************************************************/
/* prototipi delle funzioni di gestione dei comandi */
/****************************************************/
static  void    cmdTrace(           short argc, char* argv[]);
static  void    ResetTrace(         void);
static  short   CheckBufSize(       short nVar, short nSample);
static  void    SetTriggerPosAbs(   void);
static  void    tprint(             void);


/***************************/
/* inizializzazione modulo */
/***************************/
void trace_init(hStream Stream, char *Buffer, unsigned short size)
{
    unsigned short  i;

    TraceStream     = Stream;
    TraceMatrix     = (unsigned short*)(Buffer+sizeof(struct t_TraceHeader));
    TraceBuffer     = Buffer;
    TraceMatrixSize = size-sizeof(struct t_TraceHeader);
    TraceBufferSize = size;
    NumVar          = 0;
    NumSample       = 10;
    TriggerPos      = 50;
    SetTriggerPosAbs();
    Status          = STOP;
    ForceTrigger    = 0;
    TriggerMode     = AUTO;

    traceIstBrk     = 0;
    TraceStep       = 1;
    TraceCount      = 0;
    TriggerRelation = -1;

    for(i=0; i<MAXNUMVAR; i++)
    {
        varTrace[i]     = (struct varEntry*) 0;
    }

    TriggerVar      = (struct varEntry*) 0;

    consolle_addcmd("tr", "","variables trace",       cmdTrace);
}


/* Azzera il buffer di memorizzazione */
static void ResetTrace(void)
{
    short   i,j;

    for (i=0; i<NumVar; i++)
    {
        for (j=0; i<NumSample; i++)
        {
            TraceMatrix[j*NumVar+i] = 0;
        }
    }
}

/* Restituisce la stringa descrittiva dello stato del trace */
static char* StatusStr(void)
{
    char* RetVal;

    if (Status<TRACENUMSTATUS)
    {
            RetVal = TraceStatusStr[Status];
    }
    else
    {
            RetVal = "UNKNOWN";
    }
    return(RetVal);
}

/* Restituisce la stringa descrittiva del modo di trigger */
static char* ModeStr(void)
{
    char* RetVal;

    if (TriggerMode<TRIGGERNUMMODE)
    {
            RetVal = TriggerModeStr[TriggerMode];
    }
    else
    {
            RetVal = "UNKNOWN";
    }
    return(RetVal);
}

/* Controlla se la dimensione del buffer e' sufficiente per il numero       */
/* di variabili e per il numero di campioni richiesto                       */
static short CheckBufSize(short nVar, short nSample)
{
    return((nVar*nSample*sizeof(short))<=TraceMatrixSize);
}

/* Calcola il numero massimo di variabili che e' possibile campionare       */
/* in base alle dimensioni del buffer ed al numero di campioni desiderato   */
static short MaxNumVar( void )
{
    short   RetVal;

    RetVal = TraceMatrixSize/(NumSample*sizeof(short));
    if (RetVal > MAXNUMVAR)
    {
        RetVal = MAXNUMVAR;
    }
    return(RetVal);
}

/* Calcola il numero massimo di campioni che e' possibile campionare        */
/* in base alle dimensioni del buffer ed al numero di variabili desiderato  */
static short MaxNumSample( void )
{
    return(TraceMatrixSize/(NumVar*sizeof(short)));
}

/* Imposta il valore assoluto del trigger in funzione della percentuale e   */
/* del numero di campioni                                                   */
static void SetTriggerPosAbs(void)
{
    TriggerPosAbs = (NumSample*TriggerPos)/100;
}

/* Controlla se il trigger e' scattato */
short CheckTrigger(void)
{
            short   RetVal;
            short   TriggerNow;
    static  short   TriggerPrevious;

    TriggerNow  = 0;

    if (ForceTrigger)
    {
        ForceTrigger    = 0;
        TriggerNow      = 1;
    }

    if (TriggerVar != NULL)
    {
        switch(TriggerRelation)
        {
            case TRIGGER_EQUAL:
                if ((short)*( (short far *)(TriggerVar->pvar) ) == TriggerVal)
                {
                    TriggerNow  = 1;
                }
            break;

            case TRIGGER_NOTEQUAL:
                if ((short)*( (short far *)(TriggerVar->pvar) ) != TriggerVal)
                {
                    TriggerNow  = 1;
                }
            break;

            case TRIGGER_LESSTHEN:
                if ( ( (TriggerVar->flags & VARDB_TYPEMASK) == VARDB_TYPESHORT ) ||
                     ( (TriggerVar->flags & VARDB_TYPEMASK) == VARDB_TYPECHAR  )    )
                {
                    /* Variabile con segno */
                    if ((short)*( (short far *)(TriggerVar->pvar) ) < TriggerVal)
                    {
                        TriggerNow  = 1;
                    }
                }
                else
                {
                    if ((unsigned short)*( (short far *)(TriggerVar->pvar) ) < TriggerVal)
                    {
                        TriggerNow  = 1;
                    }
                }
            break;

            case TRIGGER_GREATERTHEN:
                if ( ( (TriggerVar->flags & VARDB_TYPEMASK) == VARDB_TYPESHORT ) ||
                     ( (TriggerVar->flags & VARDB_TYPEMASK) == VARDB_TYPECHAR  )    )
                {
                    /* Variabile con segno */
                    if ((short)*( (short far *)(TriggerVar->pvar) ) > TriggerVal)
                    {
                        TriggerNow  = 1;
                    }
                }
                else
                {
                    if ((unsigned short)*( (short far *)(TriggerVar->pvar) ) > TriggerVal)
                    {
                        TriggerNow  = 1;
                    }
                }
            break;

            case TRIGGER_ONEBIT0:
                if ( ~((short)*( (short far *)(TriggerVar->pvar))) & TriggerVal)
                {
                    TriggerNow  = 1;
                }
            break;

            case TRIGGER_ONEBIT1:
                if ( ((short)*( (short far *)(TriggerVar->pvar))) & TriggerVal)
                {
                    TriggerNow  = 1;
                }
            break;

            case TRIGGER_ALLBIT0:
                if ( (~((short)*( (short far *)(TriggerVar->pvar))) & TriggerVal) == TriggerVal)
                {
                    TriggerNow  = 1;
                }
            break;

            case TRIGGER_ALLBIT1:
                if ( ((short)*( (short far *)(TriggerVar->pvar)) & TriggerVal) == TriggerVal)
                {
                    TriggerNow  = 1;
                }
            break;

            default:
            break;
        }
    }

    /* Deve segnalare trigger attivo solo sui fronti positivi della condizione */
    RetVal          = (TriggerNow && !TriggerPrevious);
    TriggerPrevious = TriggerNow;

    return(RetVal);
}

/* Campionamento delle variabili */
/* Questa funzione deve essere chiamata dall'applicativo nel task ad alta priorita' */
void trace_sample(void)
{
    if (++TraceCount >= TraceStep)
    {
        short           tmpMat, i;
        struct varEntry *ptEntry;

        TraceCount  = 0;

        switch(Status)
        {
            case RUN:
                /* Controlla il trigger */
                if (CheckTrigger())
                {
                    /* E' scattato il trigger: */
                    /* Imposta il numero di campioni da eseguire dopo il trigger */
                    SampleCount = NumSample-TriggerPosAbs-1;

                    /* Si porta nello stato TRIGGERED */
                    Status = TRIGGERED;
                }

            break;

            case TRIGGERED:
                if (SampleCount)
                {
                    SampleCount--;
                }
                else
                {
                    Status = WRITING;
                }
            break;

            case DONE:
                /* Se e' in modo AUTO riarma il trigger */
                if (TriggerMode == AUTO)
                {
                    Status = RUN;
                }
                else
                {
                    Status = STOP;
                }
            break;

            case STOP:
            break;
        }

        /* Nello stato RUN e TRIGGERED deve effettuare il campionamento */
        if ((Status == RUN) || (Status == TRIGGERED))
        {
            for (i=0; i<NumVar; i++)
            {
                ptEntry = varTrace[i];

                if ( (ptEntry->flags & VARDB_TYPEMASK) == VARDB_TYPECHAR)
                {
                    tmpMat = (short)*( (char  far *)(ptEntry->pvar) );
                }
                else
                {
                    tmpMat = (short)*( (short far *)(ptEntry->pvar) );
                }

                TraceMatrix[traceIstBrk*NumVar+i]   = tmpMat;
            }

            /* Gestisce il buffer in modo CIRCOLARE */
            if (++traceIstBrk >= NumSample)
            {
                traceIstBrk = 0;
            }
        }
    }
}

/* Questa funzione deve essere chiamata continuamente dall'applicativo in background */
/* o in un task a bassa priorita'. Essa provvede alla memorizzazione di un trace */
void trace_write(void)
{
    /* Se il sistema e' arrivato in modo WRITING effettua la memorizzazione */
    if (Status == WRITING)
    {
        short   i;
        /* Riempie l'header con i dati opportuni */

        ((struct t_TraceHeader *)TraceBuffer)->NumVar        = NumVar;
        ((struct t_TraceHeader *)TraceBuffer)->NumSample     = NumSample;
        ((struct t_TraceHeader *)TraceBuffer)->TriggerSample = TriggerPosAbs;
        ((struct t_TraceHeader *)TraceBuffer)->FirstSample   = traceIstBrk;

        for (i=0;i<NumVar;i++)
        {
            ((struct t_TraceHeader *)TraceBuffer)->VarIndex[i] = vardb_VarIndex((char *)varTrace[i]->nomeVar);
        }

        stream_append(TraceStream,TraceBuffer,sizeof(struct t_TraceHeader)+(NumSample*NumVar*2));

        consolle_print("\n*** Trace occurred ***\n");
        /* Si porta in modo DONE */
        Status  = DONE;
    }
}

/* Stampa il contenuto del buffer */
static void tprint(void)
{
    short i,j;
    short index;
    unsigned short delay;

    index = ((struct t_TraceHeader *)TraceBuffer)->FirstSample;

    for (i=0;i<((struct t_TraceHeader *)TraceBuffer)->NumVar;i++)
    {
        consolle_print("%s ",vardb[((struct t_TraceHeader *)TraceBuffer)->VarIndex[i]].nomeVar);
    }

    consolle_print("\n");

    for (i=0;i<((struct t_TraceHeader *)TraceBuffer)->NumSample;i++)
    {
        consolle_print("%3d ",i+1);

        for (j=0;j<((struct t_TraceHeader *)TraceBuffer)->NumVar;j++)
        {
            consolle_print(FRMTVAR_INT,TraceMatrix[index*((struct t_TraceHeader *)TraceBuffer)->NumVar+j]);
        }

        if (((struct t_TraceHeader *)TraceBuffer)->TriggerSample == i)
        {
            consolle_print(" <---\n");
        }
        else
        {
            consolle_print("\n");
        }
        index++;
        if (index>=((struct t_TraceHeader *)TraceBuffer)->NumSample)
        {
            index = 0;
        }

        /* Provvisorio */
        /* Questo ritardo e' necessario per non perdere caratteri durante la stampa */
        /* Per risolvere il problema si deve modificare la funzione putchar         */
        for (delay=0;delay<0xF000;delay++);
        for (delay=0;delay<0xF000;delay++);
        for (delay=0;delay<0xF000;delay++);
    }
}

/**************************/
/* gestione comando trace */
/**************************/
static void cmdTrace(short argc, char* argv[])
{
    char    i;
    char    VarNotFound;
    char    TooManyVars;

    if ( argc > 1 )
    {
        /* Si sono specificati dei parameteri */
        if ((strcmp(argv[1],"var") == 0) || (strcmp(argv[1],"var+") == 0))
        {
            /* Si devono impostare le variabili da campionare */
            short           i;
            struct varEntry *ptEntry;

            if (argc > 2)
            {
                VarNotFound     = -1;
                TooManyVars     = 0;

                /* Si porta nello stato di STOP */
                Status          = STOP;
                ResetTrace();

                /* Se si e' usato il comando var deve azzerare l'elenco variabili */
                /* altrimenti deve aggiungere */
                if (strcmp(argv[1],"var") == 0)
                {
                    NumVar = 0;
                }

                /* Scandisce i nomi delle variabili forniti */
                for (i=0; i<argc-2; i++)
                {
                    /* Controlla se c'e' spazio per un'altra variabile */
                    if (NumVar<MaxNumVar())
                    {
                        /* Controlla se la variabile e' presente in archivio */
                        if ((ptEntry = vardb_findvar(argv[i+2])) != NULL)
                        {
                            varTrace[NumVar]    = ptEntry;
                            NumVar++;
                        }
                        else
                        {
                            VarNotFound = i+2;
                        }
                    }
                    else
                    {
                        TooManyVars = 1;
                    }
                }

                /* Stampa dei messaggi diagnostici */
                if (TooManyVars)
                {
                    consolle_print("Too many variables (truncated to max: %d)\n",MaxNumVar());
                }

                if (VarNotFound != -1)
                {
                    consolle_print("Variable not found: %s\n",argv[VarNotFound]);
                }
            }
            else
            {
                consolle_print("Trace ERROR: specify variables to add\n");
            }
        }
        else if (strcmp(argv[1],"step") == 0)
        {
            /* Si deve impostare il passo di campionamento */
            if (argc > 2)
            {
                short   temp;

                temp    = atoi(argv[2]);
                if (temp > 0)
                {
                    /* Si porta nello stato di STOP */
                    Status      = STOP;
                    ResetTrace();

                    TraceStep   = temp;
                    TraceCount  = 0;
                }
                else
                {
                    consolle_print("Trace ERROR: bad step value\n");
                }
            }
            else
            {
                consolle_print("Trace ERROR: specify a sampling step\n");
            }
        }
        else if (strcmp(argv[1],"len") == 0)
        {
            /* Si deve impostare la lunghezza del buffer di campionamento */
            if (argc > 2)
            {
                long    temp;

                temp    = atol(argv[2]);
                if ((temp > 0) && CheckBufSize(NumVar,temp))
                {
                    /* Si porta nello stato di STOP */
                    Status      = STOP;
                    ResetTrace();

                    NumSample   = temp;
                    SetTriggerPosAbs();
                }
                else
                {
                    consolle_print("Trace ERROR: bad buffer length value (max: %d)\n",MaxNumSample());
                }
            }
            else
            {
                consolle_print("Trace ERROR: specify a buffer length\n");
            }
        }
        else if (strcmp(argv[1],"pos") == 0)
        {
            /* Si deve impostare la posizione del trigger */
            if (argc > 2)
            {
                long    temp;

                temp    = atol(argv[2]);
                if ((temp > 0) && (temp < 100))
                {
                    /* Si porta nello stato di STOP */
                    Status      = STOP;
                    ResetTrace();

                    TriggerPos  = temp;
                    SetTriggerPosAbs();
                }
                else
                {
                    consolle_print("Trace ERROR: bad trigger position\n");
                }
            }
            else
            {
                consolle_print("Trace ERROR: specify a trigger position\n");
            }
        }
        else if (strcmp(argv[1],"ft") == 0)
        {
            /* Deve forzare il trigger */
            consolle_print("Trigger forced\n");
            ForceTrigger    = 1;
        }
        else if (strcmp(argv[1],"mode") == 0)
        {
            /* Deve impostare il modo di funzionamento  */

            if (argc == 3)
            {
                char    mode;

                mode    = -1;
                for (i=0;i<TRIGGERNUMMODE;i++)
                {
                    if (strcmp(argv[2],TriggerModeStr[i])==0)
                    {
                        mode = i;
                    }
                }
                if (mode != -1)
                {
                    TriggerMode = mode;
                }
            }
        }
        else if (strcmp(argv[1],"run") == 0)
        {
            if (Status == STOP)
            {
                Status  = RUN;
                consolle_print("Trace running...\n");
            }
        }
        else if (strcmp(argv[1],"stop") == 0)
        {
            Status  = STOP;
            consolle_print("Trace stopped\n");
        }
        else if (strcmp(argv[1],"clear") == 0)
        {
            /* Deve cancellare tutte le memorizzazioni */
            stream_erase(TraceStream);
        }
        else if (strcmp(argv[1],"trig") == 0)
        {
            /* Si devono impostare la condizione di trigger */
            struct varEntry *ptEntry;
            char            relation;

            relation = -1;
            if (argc == 5)
            {
                /* Controlla se la variabile specificata esiste */
                if ((ptEntry = vardb_findvar(argv[2])) != NULL)
                {
                    for (i=0;i<TRIGGERNUMRELATIONS;i++)
                    {
                        if (strcmp(argv[3],TriggerRelStr[i])==0)
                        {
                            relation = i;
                        }
                    }

                    if (relation == -1)
                    {
                        consolle_print("Trace ERROR: specify a valid operator (");
                        for (i=0;i<TRIGGERNUMRELATIONS-1;i++)
                        {
                            consolle_print("%s,",TriggerRelStr[i]);
                        }
                        consolle_print("%s)\n",TriggerRelStr[TRIGGERNUMRELATIONS-1]);
                    }
                }
                else
                {
                    consolle_print("Trace ERROR: variable not found\n");
                }

                /* Se tutti i parametri sono corretti li attiva */
                if (ptEntry && (relation != -1))
                {
                    /* Forza lo stato di STOP */
                    Status          = STOP;

                    TriggerVar      = ptEntry;
                    TriggerVal      = atoi(argv[4]);
                    TriggerRelation = relation;
                }
            }
            else
            {
                consolle_print("Trace ERROR: syntax error\n");
            }
        }
        else if ( (strcmp(argv[1],"ll") == 0) ||    /*Long  list */
                  (strcmp(argv[1],"sl") == 0))      /*Short list */
        {
            short           nbyte;
            unsigned long   nRecPrinted;
            char            longList;
            short           firstRecord;
            short           lastRecord;
            short           ListAbort;

            /* Forza lo stato di STOP */
            Status  = STOP;

            if (strcmp(argv[1],"ll") == 0)
            {
                longList    = 1;
            }
            else
            {
                longList    = 0;
            }

            firstRecord = 1;
            if (argc>2)
            {
                firstRecord = atoi(argv[2]);
            }

            lastRecord  = 0x7FFF;
            if (argc>3)
            {
                lastRecord  = atoi(argv[3]);
            }

            if (firstRecord<0 || lastRecord<0)
            {
                unsigned long   nRec;

                nRec    = stream_recCount(TraceStream);

                if (firstRecord<0)
                {
                    firstRecord     = nRec + firstRecord + 1;
                }

                if (lastRecord<0)
                {
                    lastRecord      = nRec + lastRecord + 1;
                }
            }

            if (firstRecord<1)
            {
                firstRecord = 1;
            }

            if (lastRecord<1)
            {
                lastRecord  = 0x7FFF;
            }

            if (lastRecord<firstRecord)
            {
                lastRecord = firstRecord;
            }

            /* Si porta sul primo record richiesto */
            stream_seek(TraceStream,firstRecord);

            /* Legge un record dallo stream nel buffer di lavoro */
            nbyte       = stream_read(TraceStream, TraceBuffer, TraceBufferSize);
            nRecPrinted = 0;
//            ptH = (struct t_diagHeader *) buf;

            consolle_print("Trace memory ");
            if (longList)
            {
                consolle_print("long list:\n");
            }
            else
            {
                consolle_print("short list:\n");
            }

            if (firstRecord != 1)
            {
                consolle_print("Record range start: %u\n",firstRecord);
            }

            if (lastRecord != 0x7FFF)
            {
                consolle_print("Record range end: %u\n",lastRecord);
            }

            if (!longList)
            {
                consolle_print("Rec Date       Time\n");
            }

//            while(nbyte > 0 && ((nrec_printed+first_record) <= last_record))

            ListAbort = 0;
            while(nbyte > 0 && ((nRecPrinted+firstRecord) <= lastRecord) && !ListAbort)
            {
                if (longList)
                {
                    consolle_print("\nRec: %-2u Date:%02u/%02u/%04u Time:%02u:%02u:%02u.%02u\n",
                                   nRecPrinted+firstRecord,
                                   0,0,0,0,0,0,0
/*
                                   ptH->Time.day,
                                   ptH->Time.month,
                                   ptH->Time.year,
                                   ptH->Time.hour,
                                   ptH->Time.min,
                                   ptH->Time.sec,
                                   ptH->Time.csec
*/
                                   );

                    tprint();
                }
                else
                {
                    consolle_print("%-3u %02u/%02u/%04u %02u:%02u:%02u.%02u\n",
                                   nRecPrinted+firstRecord,
                                   0,0,0,0,0,0,0
                                   );
                }
                nRecPrinted++;
                nbyte   = stream_read(TraceStream, TraceBuffer, TraceBufferSize);


                /* Svuota la coda di caratteri di input e controlla la pressione dello SPAZIO */
                {
                    char c;

                    c = consio_getkey();
                    while(c != EOF)
                    {
                        if (c == ' ')
                        {
                            ListAbort = 1;
                        }
                        c = consio_getkey();
                    }
                }
            }
            if (ListAbort)
            {
                consolle_print("List aborted\n");
            }
        }
        else if (strcmp(argv[1],"help") == 0)
        {
            /* Stampa pagina di help */
            consolle_print("Syntax:\ntr [[[command] par1] par2]...\ncommands available:\n");
            consolle_print("help                   - print this page\n");
            consolle_print("run                    - set trace in RUN status\n");
            consolle_print("stop                   - set trace in STOP status\n");
            consolle_print("clear                  - clear all trace in memory\n");
            consolle_print("ft                     - force a trigger\n");
            consolle_print("var <v1> <v2>..<vn>    - select vars to trace\n");
            consolle_print("var+ <v1> <v2>..<vn>   - add vars to trace\n");
            consolle_print("step <n>               - set the sampling step\n");
            consolle_print("len <n>                - set the number of sampling\n");
            consolle_print("pos <n%%>              - set the trigger position\n");
            consolle_print("sl [[<first>] <last>]  - short list of trace in memory\n");
            consolle_print("ll [[<first>] <last>]  - long list of trace in memory\n");
            consolle_print("trig <var> <rel> <val> - set the trigger condition\n");
            consolle_print("mode auto|single       - set the trigger mode\n\n");
            consolle_print("  * To view the trace settings/status use: \"tr\"\n");
            consolle_print("  * To view the last trace in memory use: \"tr ll -1\"\n");
            consolle_print("  * Press space to abort listings\n");
        }
    }
    else
    {
        short   i;

        /* Visualizza le impostazioni del trace */
        consolle_print("***** Trace settings *****\n");
        consolle_print(" status:            %s\n",StatusStr());
        consolle_print(" records:           %-d\n",stream_recCount(TraceStream));
        consolle_print(" sampling STEP:     %-d\n",TraceStep);
        consolle_print(" buffer LENgth:     %-d\n",NumSample);
        consolle_print(" VARs selected:     %-d\n",NumVar);
        consolle_print(" trigger MODE:      %s\n",ModeStr());

        if (NumVar)
        {
            consolle_print(" (");
            for(i=0; i<NumVar-1; i++)
            {
                if (varTrace[i]==NULL)
                {
                    consolle_print("(Null)\n");
                }
                else
                {
                    consolle_print("%s,",varTrace[i]->nomeVar);
                }
            }
            consolle_print("%s)\n",varTrace[NumVar-1]->nomeVar);
        }

        if ((TriggerRelation != -1) && TriggerVar)
        {
            consolle_print(" TRIGGER condition: \"%s %s %d\"\n",TriggerVar->nomeVar,
                                                                TriggerRelStr[TriggerRelation],
                                                                TriggerVal);
        }
        else
        {
            consolle_print(" TRIGGER condition: NONE\n");
        }
        consolle_print(" trigger POSition:  %-d%%\n",TriggerPos);
    }
}


/* ****************************************************************************
    $Log: trace.c,v $
    Revision 1.4  2004/11/16 15:38:15  mungiguerrav
    2004/nov/15,lun 11:00          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.3        2004/08/31 14:25:15     mungiguerrav
    2004/ago/31,mar 16:25        Napoli          Mungi\Busco
    - Si effettua Merge da branch makefile

    Revision 1.2.2.1    2004/07/14 15:38:57     buscob
01.07   2004/apr/05,lun             Napoli              Mungi\Nappo
        - Nella struct t_TraceHeader si mod. il campo VarIndex[MAXNUMVAR] da char a short
          in quanto la funzione vardb_VarIndex che la inizializza in trace_write()
          puo' restituire anche un indice superiore a 127
01.06   2003/dic/17,mer             Napoli              Nappo/Busco
        - Si modifica l'utilizzo della struttura vaEntry in conformità con le modifiche
          apportate alla stessa in vardb.h

    Revision 1.2        2004/06/15 12:00:22     buscob
01.05   2003/nov/30,dom             Madrid              Mungi
        - Si applicano le regole su parentesi e nomi dal manuale del SW
01.04   2003/nov/21,ven             Napoli              Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso.

    Revision 1.1        2002/05/17 10:19:30     accardol
    - Inserimento in CVS della versione in lib07m
    Revision 1.1.1.1    2002/05/17 10:19:30     accardol
    - Inserimento in CVS in branch STP della versione in lib07m
        La data è arretrata con un artificio
01.03   2002/mag/17,ven             Napoli              Busco
        - Si elim. funzione temporanea VarIndex perche' si e' aggiunta in query
        - Si mod. TriggerRelation da Uchar a char (per warning Borland)
        - Si agg. include consio.h e si usa consio_getkey()

 ------------------------   Prima di CVS    ------------------------
01.02   2001/ago/29,mer             Napoli              Busco
        - Si elim. funzione trace_print()
01.01   2001/ago/27,lun             Napoli              Busco
        - Si rende modificabile da comando il numero di variabili da campionare
        - Si rende modificabile da comando il numero di campioni
01.00   2001/lug/31,mar             Napoli              Busco
        - Creazione a partire da trcbk.c versione 1.03
*/

