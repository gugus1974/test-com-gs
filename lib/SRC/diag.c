/****************************************************************/
/* 2004/nov/08,lun 08:50                \SWISV\LIB\SRC\diag.c   */
/****************************************************************/
/*          Memorizzazione eventi diagnostici                   */
/****************************************************************/
/*
    $Date: 2004/11/08 09:20:38 $          $Revision: 1.4 $
    $Author: mungiguerrav $
*/


/* DA FARE:
   1) Utilizzare variabili registrate in vardb eliminando cosi' le
      funzioni PrintSampleFunc e PrintVarNamesFunc
   2) Rendere indipendente dal sistema operativo
   3) Velocizzare lo scarico dei dati (download in binario?)
*/


#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "consolle.h"
#include "stream.h"
#include "query.h"

#include "pi_rtx.h"
#include "strt_tcn.h"

#include "diag.h"

#define MAXCACHEENTRY   8        /* Modulo 2 per semplif. coda circolare    */
#define MAXSUMENTRY    50

#define MEMO_TASK_ID   30
#define MEMO_TASK_PRIO 5
#define MEMO_QUEUE_DIM 30
short   queue_memo;
short   esito_pil = PI_RET_OK;

struct t_datetime
{
    unsigned short     year;
    unsigned char      month;
    unsigned char      day;
    unsigned char      hour;
    unsigned char      min;
    unsigned char      sec;
    unsigned char      csec;
};

struct t_diagno
{
    tPrintSampleFunc     PrintSampleFunc;
    tPrintEventDataFunc  PrintEventDataFunc;
    tPrintVarNamesFunc   PrintVarNamesFunc;
    tPrintCodDescrFunc   PrintCodDescrFunc;
    tPrintCodEnabledFunc PrintCodEnabledFunc;
    tVarNamesStringFunc  VarNamesStringFunc;
    tSampleStringFunc    SampleStringFunc;
    char                 *SampleBuffer;
    char                 *EventDataBuffer;
    char                 *TotalBuffer;
    short                SampleSize;
    short                EventDataSize;
    short                MaxSample;
    short                Stream;
    short                actualSample;
    unsigned short       FirstEventIndex;
    unsigned short       LastEventIndex;
    unsigned short       CurrentRecNumber;
    unsigned short       CurrentIndex;
};

struct t_diagHeader
{
    unsigned short     DiagCode;        // Codice dell'evento diagnostico
    unsigned short     Index;           // Indice progressivo dell'evento diagnostico
//    short              nAnteSample;     // Numero di campioni Ante
//    short              nPostSample;     // Numero di campioni Post
    char      nAnteSample;     // Numero di campioni Ante
    char      nPostSample;     // Numero di campioni Post
    struct t_datetime  Time;            // Timestamp dell'evento
};

struct t_cache
{
    struct t_diagHeader header;
    short               waitCycle;
    short               trigger;
    char                EventData[DIAG_EVENTDATAMAXSIZE];
};

struct t_summaryEntry
{
    unsigned short     DiagCode;
    unsigned short     Occurrence;
    unsigned short     RecFirst;
    struct t_datetime  TimeFirst;
    unsigned short     RecLast;
    struct t_datetime  TimeLast;
};


static struct t_diagno          diagno;
static struct t_cache           cache[MAXCACHEENTRY];
static struct t_summaryEntry    summary[MAXSUMENTRY];
static char                     buf[0x1000];
static char                     bufQuery[0x1000]; /* Buf per contenere record selezionato */
static short                    appendEnabled;
static unsigned short           firstCache;     /* prossimo da cui leggere  */
static unsigned short           lastCache;      /* prossimo in cui scrivere */

/* Prototipi delle funzioni locali */
static char     *BufferedSample(unsigned short base, short offset);
static void     gest_diag(short argc, char* argv[]);
static void     writeRec(unsigned short index);
unsigned short  GotoIndex(unsigned short Index);
static void     gestquery_DIDS(char* arg);
static void     gestquery_DRC(char* arg);
static void     gestquery_DVN(char* arg);
static void     gestquery_DRS(char* arg);
static void     gestquery_DRD(char* arg);
static void     gestquery_DENDX(char* arg);


/* Inizializza il sistema di memorizzazione dati diagnostici */
void diag_init( tPrintSampleFunc     psf,
                tPrintEventDataFunc  pedf,
                tPrintVarNamesFunc   pvnf,
                tPrintCodDescrFunc   pcdf,
                tPrintCodEnabledFunc pcef,
                tVarNamesStringFunc  pvnsf,
                tSampleStringFunc    pssf,
                char  *SampleBuffer,
                char  *EventDataBuffer,
                char  *TotalBuffer,
                short SampleSize,
                short EventDataSize,
                short MaxSample,
                short Stream
              )
{
    struct t_diagHeader *ptH;
    unsigned long nRec;

    diagno.PrintSampleFunc     = psf;
    diagno.PrintEventDataFunc  = pedf;
    diagno.PrintVarNamesFunc   = pvnf;
    diagno.PrintCodDescrFunc   = pcdf;
    diagno.PrintCodEnabledFunc = pcef;
    diagno.VarNamesStringFunc  = pvnsf;
    diagno.SampleStringFunc    = pssf;


    diagno.SampleBuffer      = SampleBuffer;
    diagno.EventDataBuffer   = EventDataBuffer;
    diagno.SampleSize        = SampleSize;

    /* Limita la dimensione dell' EventDataBuffer alla massima consentita */
    /* Se il puntatore al buffer fornito e' NULL forza la dimensione a 0 */
    diagno.EventDataSize = EventDataSize;
    if (diagno.EventDataSize > DIAG_EVENTDATAMAXSIZE)
    {
        diagno.EventDataSize = DIAG_EVENTDATAMAXSIZE;
    }
    if (!diagno.EventDataBuffer)
    {
        diagno.EventDataSize = 0;
    }


    diagno.MaxSample         = MaxSample;
    diagno.actualSample      = 0;
    diagno.TotalBuffer       = TotalBuffer;
    diagno.Stream            = Stream;
    diagno.CurrentRecNumber  = 0;
    diagno.CurrentIndex      = 0;

    /* Preleva l'indice progressivo del primo e dell'ultimo record */
    nRec = stream_recCount(diagno.Stream);
    if (nRec > 0)
    {
        /* Se ci sono record diagnostici ne legge il primo ...*/
        stream_seek(diagno.Stream,1);

        /* Legge il primo record nel buffer Query per renderlo selezionato */
        stream_read(diagno.Stream, bufQuery, 0x1000);
        ptH = (struct t_diagHeader *) bufQuery;
        diagno.FirstEventIndex = ptH->Index;
        diagno.CurrentRecNumber = 1;
        diagno.CurrentIndex = diagno.FirstEventIndex;

        /* ... e l'ultimo */
        stream_seek(diagno.Stream,nRec);
        stream_read(diagno.Stream, buf, 0x1000);
        ptH = (struct t_diagHeader *) buf;
        diagno.LastEventIndex = ptH->Index;
    }
    else
    {
        /* Altrimenti gli indici vengono posti a 0 */
        diagno.FirstEventIndex = 0;
        diagno.LastEventIndex = 0;
    }

    appendEnabled   = 1;
    firstCache      = 0;
    lastCache       = 0;

    pi_task_create(memo_task, MEMO_TASK_ID, MEMO_TASK_PRIO);

    consolle_addcmd("diag","INIT| [sl|ll|sm [start|0 [end|0 [code]]]]", "Gestione Dati diagnostici", gest_diag);
    query_addcmd("DENDX",  gestquery_DENDX);
    query_addcmd("DIDS",   gestquery_DIDS);
    query_addcmd("DRC",    gestquery_DRC);
    query_addcmd("DVN",    gestquery_DVN);
    query_addcmd("DRS",    gestquery_DRS);
    query_addcmd("DRD",    gestquery_DRD);

}

/* Aggiunge un nuovo campione al buffer circolare */
void diag_updateSample(void)
{
    char*           p_dest;
    unsigned short  index;
    unsigned short  fineWrite;

    diagno.actualSample++;
    if(diagno.actualSample >= diagno.MaxSample)
        diagno.actualSample = 0;

    /* Calcola il puntatore al buffer circolare in cui scrivere */
    p_dest = diagno.TotalBuffer + diagno.actualSample * diagno.SampleSize;

    /* Copia il campione nel buffer circolare */
    memcpy(p_dest, diagno.SampleBuffer, diagno.SampleSize);

    index = firstCache;             /* Scansione per decremento */
    while (index!=lastCache)
    {
        if (cache[index].waitCycle>0)
            cache[index].waitCycle--;

        index = (index + 1 ) & (MAXCACHEENTRY-1);
    }

    if (appendEnabled)
    {
        fineWrite = 0;                  /* Scansione per scrittura */
        while ( (firstCache != lastCache) && !fineWrite )
        {
            if (cache[firstCache].waitCycle == 0)
            {
                writeRec(firstCache);
                firstCache = (firstCache + 1 ) & (MAXCACHEENTRY-1);
            }
            else
                fineWrite=1;
        }
    }
}


/* Aggiunta di un nuovo pacchetto diagnostico */
void diag_append( unsigned short diagCode,
                  unsigned short n_ante,
                  unsigned short n_post,
                  unsigned short year,
                  unsigned char  month,
                  unsigned char  day,
                  unsigned char  hour,
                  unsigned char  min,
                  unsigned char  sec,
                  unsigned char  csec
                )
{
    unsigned short  tempCache;

    if (n_ante<0)   n_ante = 0;
    if (n_post<0)   n_post = 0;


    tempCache = (lastCache + 1 ) & (MAXCACHEENTRY-1);
    if (firstCache != tempCache)    /* FIFO non piena */
    {
        /* Riempie la cache degli header */
        cache[lastCache].header.DiagCode    = diagCode;
        cache[lastCache].header.nAnteSample = n_ante;
        cache[lastCache].header.nPostSample = n_post;
        cache[lastCache].header.Time.year   = year;
        cache[lastCache].header.Time.month  = month;
        cache[lastCache].header.Time.day    = day;
        cache[lastCache].header.Time.hour   = hour;
        cache[lastCache].header.Time.min    = min;
        cache[lastCache].header.Time.sec    = sec;
        cache[lastCache].header.Time.csec   = csec;

        cache[lastCache].waitCycle          = n_post;
        cache[lastCache].trigger            = diagno.actualSample;

        /* Se e' stato definito un record EventData lo memorizza nella cache. */
        if (diagno.EventDataSize > 0)
        {
            memcpy(&(cache[lastCache].EventData), diagno.EventDataBuffer, diagno.EventDataSize);
        }

        lastCache = tempCache;
    }
    else
        consolle_print("\nToo many diagnostic events to cache !!!\n");
}


static void writeRec(unsigned short index)
{
    unsigned short        BufSize;
    char xhuge         *p_temp;
    char xhuge         *p_buffer;
    short esito = PI_RET_OK;


    /* Calcola la dimensione del buffer di memoria necessario   */
    /* a contenere i dati diagnostici                           */
    BufSize = sizeof(struct t_diagHeader)  +
              diagno.EventDataSize         +
              diagno.SampleSize *
              (cache[index].header.nAnteSample +
               cache[index].header.nPostSample + 1);

    /* Alloca il buffer */
    p_buffer = pi_alloc(BufSize);
    p_temp   = p_buffer;

    if (p_temp == NULL)
        consolle_print("Errore di allocazione");
    else
    {
        short i;

        /* Incrementa l'EventIndex corrente e lo inserisce nell'header */
        /* N.B. questa operazione viene fatta qui e non nella funzione _append perche' a causa della cache */
        /* l'ordine di memorizzazione su flash potrebbe non coincidere con l'ordine temporale degli eventi */
        /* (se hanno nPost campioni differenti)                                                            */
        diagno.LastEventIndex++;
        cache[index].header.Index = diagno.LastEventIndex;

        /* Copia l'header nel buffer */
        memcpy(p_temp, &(cache[index].header), sizeof(struct t_diagHeader));
        p_temp += sizeof(struct t_diagHeader);

        /* Copia l'EventDataBuffer nel buffer */
        memcpy(p_temp, cache[index].EventData, diagno.EventDataSize);
        p_temp += diagno.EventDataSize;

        /* Copia n_ante campioni nel buffer */
        for (i=-cache[index].header.nAnteSample;
             i<=cache[index].header.nPostSample;
             i++)
        {
            p_temp = memcpy(p_temp,
                            BufferedSample(cache[index].trigger,i),
                            diagno.SampleSize);

            p_temp += diagno.SampleSize;
        }

        pi_send_queue(queue_memo,p_buffer,&esito);
        if(esito!=0)
            consolle_print("Send error %d\n",esito);
    }
}

static char *BufferedSample(unsigned short base, short offset)
{
    short buffer_index;

    buffer_index = base+offset;

    if (offset<0)   /* caso ante */
    {
        if (buffer_index < 0)
            buffer_index += diagno.MaxSample;
    }
    else            /* caso post */
    {
        if (buffer_index >= diagno.MaxSample)
            buffer_index -= diagno.MaxSample;
    }

    return(diagno.TotalBuffer + diagno.SampleSize * buffer_index);
}


/* Task per la memorizzazione dei dati diagnostici */
PI_TASK(memo_task, MEMO_TASK_ID, MEMO_TASK_PRIO)
{
    char huge  *buf_ptr;
    struct     t_diagHeader *pt;
    short      recsize;

    queue_memo = pi_create_queue(MEMO_QUEUE_DIM,PI_ORD_FCFS,&esito_pil);

    while (1)
    {  /* Attende un buffer di dati  */

//       consolle_print("Memo task in attesa\n");

       buf_ptr =
         (char huge *) pi_receive_queue (queue_memo,PI_FOREVER,&esito_pil);
//*debug*/         consolle_print("Arrivata coda\n");

       /* Preleva dal buffer l'header per poter risalire alla dimensione */
       /* del record da scrivere                                         */
       pt = (struct t_diagHeader *) buf_ptr;
       recsize = sizeof(struct t_diagHeader) +
                 diagno.EventDataSize        +
                 (pt->nAnteSample + pt->nPostSample + 1) * diagno.SampleSize;

        /* Si appresta ad aggiungere un record, invalida i puntatori al record selezionato */
        diagno.CurrentRecNumber  = 0;
        diagno.CurrentIndex      = 0;

       /* Memorizza il buffer ricevuto  */
       stream_append(diagno.Stream, buf_ptr, recsize);


       /* Rilascia il buffer ricevuto  */
       pi_free ((char *) buf_ptr);
    }
}

/******************************************************/
/* Comando per la gestione della diagnostica da shell */
/******************************************************/
static void gest_diag(short argc, char* argv[])
{
    short i;

    /* Stampa sempre la stringa di identificazione */
    consolle_print("Diagnostic ID string: %s\n",stream_GetIdString(diagno.Stream));

    if (argc >= 2)
    {
        /* Inizializzazione */
        if ( (strcmp(argv[1],"INIT") == 0) )
        {
            appendEnabled = 0;

            consolle_print("Initializing Diagnostic Memory ...");

            /* controlla se e' stata specificata una ID string */
            if (argc > 2)
                stream_SetIdString(diagno.Stream,argv[2]);
            else
                stream_SetIdString(diagno.Stream,"");

            diagno.FirstEventIndex = 0;
            diagno.LastEventIndex  = 0;
            consolle_print("\tOK\n");

            appendEnabled = 1;
        }

        /* Visualizzazione dati diagnostici */
        if ( (strcmp(argv[1],"ll") == 0) ||    /*Long  list */
             (strcmp(argv[1],"sl") == 0))      /*Short list */
        {
            short               nbyte;
            long                first_record;
            long                last_record;
            unsigned long       nrec_printed;
            struct t_diagHeader *ptH;
            char*               pt_temp;
            unsigned short      srcCode;
            char                longlist;

            /* disabilita la memorizzazione di nuovi eventi durante la lettura */
            appendEnabled = 0;

            if (strcmp(argv[1],"ll") == 0)
                longlist = 1;
            else
                longlist = 0;

            first_record = 1;
            if (argc>2)
                first_record = atol(argv[2]);

            last_record = 0x7FFFFFFFL;
            if (argc>3)
                last_record = atol(argv[3]);

            if (first_record<0 || last_record<0)
            {
                unsigned long nRec;

                nRec = stream_recCount(diagno.Stream);

                if (first_record<0)
                    first_record = nRec + first_record + 1;
                if (last_record<0)
                    last_record  = nRec + last_record + 1;
            }

            if (first_record<1)
                first_record = 1;
            if (last_record<1)
                last_record  = 0x7FFFFFFFL;

            if (last_record<first_record)
                last_record = first_record;

            /* Legge un eventuale codice richiesto dalla riga di comando */
            if (argc>4)
                srcCode = atoi(argv[4]);
            else
                srcCode = 0;

            /* Si porta sul primo record richiesto */
            stream_seek(diagno.Stream,first_record);

            nbyte = stream_read(diagno.Stream, buf, 0x1000);
            nrec_printed = 0;
            ptH = (struct t_diagHeader *) buf;

            if(longlist)
                consolle_print("Diagnostics long list:\n");
            else
                consolle_print("Diagnostics short list:\n");

            if (srcCode != 0)
                consolle_print("Searching for code: %u\n",srcCode);

            if (first_record != 1)
                consolle_print("Record range start: %u\n",first_record);

            if (last_record != 0x7FFFFFFFL)
                consolle_print("Record range end: %u\n",last_record);

            if(!longlist)
                consolle_print("Rec    Code  Date\n");

            while(nbyte > 0 && ((nrec_printed+first_record) <= last_record))
            {
                short f_print;

                /* Se e' definita la funzione di filtraggio della visualizzazione
                   dei codici diagnostici la chiama */
                f_print = 1;
                if (diagno.PrintCodEnabledFunc != NULL)
                {
                    if (!diagno.PrintCodEnabledFunc(ptH->DiagCode))
                    {
                        f_print = 0;
                    }
                }


                if( ((srcCode == 0) || (srcCode == ptH->DiagCode)) && f_print)
                {
                    if(longlist)
                    {
                        consolle_print("Rec: %6lu  Code: %5u  Date:%02u/%02u/%04u %02u:%02u:%02u.%02u",
                                       nrec_printed+first_record,
                                       ptH->DiagCode,
                                       ptH->Time.day,
                                       ptH->Time.month,
                                       ptH->Time.year,
                                       ptH->Time.hour,
                                       ptH->Time.min,
                                       ptH->Time.sec,
                                       ptH->Time.csec
                                       );

                        /* Se e' definita la funzione di stampa della descrizione
                           del codice diagnostico la chiama */
                        if (diagno.PrintCodDescrFunc != NULL)
                        {
                            consolle_print(" - %s\n",diagno.PrintCodDescrFunc(ptH->DiagCode));
                        }
                        else
                        {
                            consolle_print("\n");
                        }

                        /* Se e' definita la funzione di stampa degli EventData la chiama */
                        if (diagno.PrintEventDataFunc != NULL)
                        {
                            pt_temp = buf + sizeof(struct t_diagHeader);
                            diagno.PrintEventDataFunc(pt_temp);
                        }
                        consolle_print("\n");

                        consolle_print("N.   ");
                        diagno.PrintVarNamesFunc();
                        pt_temp = buf + sizeof(struct t_diagHeader) +
                                  diagno.EventDataSize;

                        /* Stampa i campioni ante e post */
                        for(i= - ptH->nAnteSample; i<=ptH->nPostSample; i++)
                        {
                            consolle_print("%4d ",i);
                            diagno.PrintSampleFunc(pt_temp);
                            pt_temp += diagno.SampleSize;
                        }
                        consolle_print("\n");
                    }
                    else
                    {
                        consolle_print("%6lu %5u %02u/%02u/%04u %02u:%02u:%02u.%02u",
                                       nrec_printed+first_record,
                                       ptH->DiagCode,
                                       ptH->Time.day,
                                       ptH->Time.month,
                                       ptH->Time.year,
                                       ptH->Time.hour,
                                       ptH->Time.min,
                                       ptH->Time.sec,
                                       ptH->Time.csec
                                       );

                        /* Se e' definita la funzione di stampa degli EventData la chiama */
                        if (diagno.PrintEventDataFunc != NULL)
                        {
                            pt_temp = buf + sizeof(struct t_diagHeader);
                            consolle_print(" - ");
                            diagno.PrintEventDataFunc(pt_temp);
                        }

                        if (diagno.PrintCodDescrFunc != NULL)
                            consolle_print(" - %s\n",diagno.PrintCodDescrFunc(ptH->DiagCode));
                        else
                            consolle_print("\n");

                    }
                }
                nrec_printed++;
                nbyte = stream_read(diagno.Stream, buf, 0x1000);
            }
            consolle_print("End Of Stream %d\n",diagno.Stream);

            appendEnabled = 1;
        }

        /* Summary */
        if (strcmp(argv[1],"sm") == 0)
        {
            short                   nbyte;
            long                    first_record;
            long                    last_record;
            unsigned long           nrec_printed;
            struct t_diagHeader     *ptH;
            unsigned short          nsumentry;
            long                    ntotoccur;
            unsigned short          srcCode;

            appendEnabled = 0;

            first_record = 1;
            if (argc>2)
                first_record = atol(argv[2]);

            if (first_record<1)
                first_record = 1;

            last_record = 0x7FFFFFFFL;
            if (argc>3)
                last_record = atol(argv[3]);

            if (last_record<1)
                last_record = 0x7FFFFFFFL;

            if (last_record<first_record)
                last_record = first_record;

            /* Legge un eventuale codice richiesto dalla riga di comando */
            if (argc>4)
                srcCode = atoi(argv[4]);
            else
                srcCode = 0;

            /* Si porta sul primo record richiesto */
            stream_seek(diagno.Stream,first_record);

            nbyte = stream_read(diagno.Stream, buf, 0x1000);
            nrec_printed = 0;
            ptH = (struct t_diagHeader *) buf;
            nsumentry = 0;
            while(nbyte > 0 && ((nrec_printed+first_record) <= last_record))
            {
                unsigned short sumentry;
                unsigned char  entryfound;
                short f_print;

                sumentry = 1;
                entryfound = 0;

                f_print = 1;
                if (diagno.PrintCodEnabledFunc != NULL)
                   if (!diagno.PrintCodEnabledFunc(ptH->DiagCode))
                       f_print = 0;

                if ( ((srcCode == 0) || (srcCode == ptH->DiagCode)) && f_print)
                {
                    while((sumentry<=nsumentry) && !entryfound)
                    {
                        if(summary[sumentry-1].DiagCode == ptH->DiagCode)
                            entryfound = 1;
                        else
                            sumentry++;
                    }

                    if (entryfound)
                    {
                        /* Ha trovato il codice del record attuale nel sommario */
                        summary[sumentry-1].Occurrence++;
                        summary[sumentry-1].RecLast = nrec_printed+first_record;
                        memcpy(&(summary[sumentry-1].TimeLast), &(ptH->Time), sizeof(struct t_datetime));
                    }
                    else
                    {
                        /* Non ha trovato il codice nel sommario, se c'e' posto lo aggiunge */
                        if (nsumentry < MAXSUMENTRY-1)
                        {
                            /* Aggiunge una voce al sommario e la inizializza */
                            nsumentry++;
                            summary[nsumentry-1].DiagCode = ptH->DiagCode;
                            summary[nsumentry-1].Occurrence = 1;
                            summary[nsumentry-1].RecFirst = nrec_printed+first_record;
                            memcpy(&(summary[nsumentry-1].TimeFirst), &(ptH->Time), sizeof(struct t_datetime));
                            summary[nsumentry-1].RecLast = nrec_printed+first_record;
                            memcpy(&(summary[nsumentry-1].TimeLast), &(ptH->Time), sizeof(struct t_datetime));
                        }
                        else
                            consolle_print("Too many codes to summarize (%d)\n",ptH->DiagCode);
                    }
                }

                nrec_printed++;
                nbyte = stream_read(diagno.Stream, buf, 0x1000);
            }
            consolle_print("Diagnostics summary:\n");
            if (nsumentry)
            {
                if (srcCode != 0)
                    consolle_print("Searching for code: %u\n",srcCode);
                if (first_record != 1)
                    consolle_print("Record range start: %lu\n",first_record);
                if (last_record != 0x7FFFFFFFL)
                    consolle_print("Record range end: %lu\n",last_record);
                consolle_print("Code  Nrec First Date of First rec.     Last  Date of Last rec.\n");
            }

            ntotoccur = 0;
            for(i=0; i<nsumentry; i++)
            {
                consolle_print("%5u %4u ",
                               summary[i].DiagCode,
                               summary[i].Occurrence);

                consolle_print("%5u %02u/%02u/%04u %02u:%02u:%02u.%02u %5u %02u/%02u/%04u %02u:%02u:%02u.%02u",
                               summary[i].RecFirst,
                               summary[i].TimeFirst.day,
                               summary[i].TimeFirst.month,
                               summary[i].TimeFirst.year,
                               summary[i].TimeFirst.hour,
                               summary[i].TimeFirst.min,
                               summary[i].TimeFirst.sec,
                               summary[i].TimeFirst.csec,

                               summary[i].RecLast,
                               summary[i].TimeLast.day,
                               summary[i].TimeLast.month,
                               summary[i].TimeLast.year,
                               summary[i].TimeLast.hour,
                               summary[i].TimeLast.min,
                               summary[i].TimeLast.sec,
                               summary[i].TimeLast.csec
                               );

                if (diagno.PrintCodDescrFunc != NULL)
                    consolle_print(" - %s\n",diagno.PrintCodDescrFunc(summary[i].DiagCode));
                else
                    consolle_print("\n");

                ntotoccur += summary[i].Occurrence;
            }
            consolle_print("Total summarized records: %lu\n",ntotoccur);
            consolle_print("Used bytes: %lu on %lu\n",
                           stream_byteCount(diagno.Stream),
                           stream_size(diagno.Stream));

            appendEnabled = 1;
        }
    }
}

/************************************************************************/
/* Gestione del comando: DIDS                                           */
/* Richiesta Identification string                                      */
/* Richiesta:  "DIDS"                                                   */
/* Risposta:   "DIDS OK s" (dove s e' la identification string)         */
/************************************************************************/
static void gestquery_DIDS(char* arg)
{
    char tempstr[50];

    arg = arg;
    sprintf(tempstr,"DIDS OK %s",stream_GetIdString(diagno.Stream));

    query_AppendReply(tempstr);
}

/************************************************************************/
/* Gestione del comando: DRC                                            */
/* Richiesta Record Count                                               */
/* Richiesta:  "DRC"                                                    */
/* Risposta:   "DRC OK n" (dove n e' il numero di record)               */
/************************************************************************/
static void gestquery_DRC(char* arg)
{
    char tempstr[20];

    arg = arg;
    sprintf(tempstr,"DRC OK %ld",stream_recCount(diagno.Stream));

    query_AppendReply(tempstr);
}

/************************************************************************/
/* Gestione del comando: DRS                                            */
/* - Record Select -                                                    */
/* Richiesta:  "DRS n" (dove n e' il numero di record da selezionare)   */
/* Risposta1:  "DRS OK n header"                                        */
/* Risposta2:  "DRS ?? n"                                               */
/************************************************************************/
static void gestquery_DRS(char* arg)
{
    char                    tempstr[200];
    struct t_diagHeader     *ptH;
    long                    RecordIndex;

    appendEnabled = 0;
    RecordIndex = atol(arg);

    if (GotoIndex(RecordIndex))
    {
        ptH = (struct t_diagHeader *) bufQuery;
        if (diagno.PrintCodDescrFunc != NULL)
        {
            sprintf(tempstr,"DRS OK %lu\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%s",
                            RecordIndex,
                            ptH->nAnteSample,
                            ptH->nPostSample,
                            ptH->DiagCode,
                            ptH->Time.day,
                            ptH->Time.month,
                            ptH->Time.year,
                            ptH->Time.hour,
                            ptH->Time.min,
                            ptH->Time.sec,
                            ptH->Time.csec,
                            diagno.PrintCodDescrFunc(ptH->DiagCode)
                            );
        }
        else
        {
            sprintf(tempstr,"DRS OK %lu\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%s",
                            RecordIndex,
                            ptH->nAnteSample,
                            ptH->nPostSample,
                            ptH->DiagCode,
                            ptH->Time.day,
                            ptH->Time.month,
                            ptH->Time.year,
                            ptH->Time.hour,
                            ptH->Time.min,
                            ptH->Time.sec,
                            ptH->Time.csec,
                            "N/A"
                            );
        }
    }
    else
    {
        sprintf(tempstr,"DRS ?? %lu",RecordIndex);
    }
    query_AppendReply(tempstr);
    appendEnabled = 1;
}

/*******************************************************************************************************/
/* Gestione del comando: DVN                                                                           */
/* Richiesta Var Names                    //Si deve implementare                                                             */
/* Richiesta:  "DVN n"                                                                                 */
/* Risposta:   "DVN OK n s" (dove n è il record selezionato ed s e' la stringa contenente i nomi var)  */
/*******************************************************************************************************/
static void gestquery_DVN(char* arg)
{
    char                    tempstr[200];
    char                    namestr[200];
    struct t_diagHeader     *ptH;
    long                    RecordIndex;

    appendEnabled = 0;
    RecordIndex = atol(arg);

    if (GotoIndex(RecordIndex))
    {
        ptH = (struct t_diagHeader *) bufQuery;
	if (diagno.VarNamesStringFunc != NULL)
    	{
            diagno.VarNamesStringFunc(namestr,100);
	    sprintf(tempstr,"DVN OK %lu %s",RecordIndex,namestr);
        }
        else
        {
	    sprintf(tempstr,"DVN ?? %lu",RecordIndex);
        }
    }
    else
    {
        sprintf(tempstr,"DVN ?? %lu",RecordIndex);
    }
    query_AppendReply(tempstr);
    appendEnabled = 1;
}

/************************************************************************/
/* Gestione del comando: DRD                                            */
/* Richiesta Dati sul record selezionato                                */
/* Richiesta:  "DRD n"    (n=numero linea)                              */
/* Risposta1:  "DRD OK n s" (s=stringa dati)                            */
/* Risposta2:  "DRD ?? n"                                               */
/************************************************************************/
static void gestquery_DRD(char* arg)
{
    char                    tempstr[200];
    char                    samplestr[200];
    struct t_diagHeader     *ptH;
    short                   LineNumber;
    char                    LineOK;
    char*                   pt_temp;

    LineNumber = atoi(arg);

    LineOK = 0;
    ptH = (struct t_diagHeader *) bufQuery;
    if(diagno.CurrentRecNumber > 0)
    {
        if((LineNumber >= -ptH->nAnteSample) && (LineNumber <= ptH->nPostSample))
        {
            LineOK = 1;
        }
    }

    if(LineOK)
    {
    	if (diagno.SampleStringFunc)
	    {
            pt_temp = bufQuery + sizeof(struct t_diagHeader) +
                      diagno.EventDataSize + diagno.SampleSize*(LineNumber+ptH->nAnteSample);

	        diagno.SampleStringFunc(pt_temp,samplestr,200);
            sprintf(tempstr,"DRD OK %d %s",LineNumber,samplestr);
    	}
	    else
	    {
            sprintf(tempstr,"DRD ?? %d",LineNumber);
	    }
    }
    else
    {
        sprintf(tempstr,"DRD ?? %d",LineNumber);
    }

    query_AppendReply(tempstr);
}


/*****************************************************************************/
/* Gestione del comando: DENDX                                               */
/* Richiesta Event Index                                                     */
/* Richiesta:  "DENDX"                                                       */
/* Risposta:   "DENDX OK first last" (dove first e last sono gli indici      */
/*             progressivi del primo ed ultimo record                        */
/*****************************************************************************/
static void gestquery_DENDX(char* arg)
{
    char        tempstr[20];
    struct      t_diagHeader *ptH;
    static unsigned short PreviousLastEventIndex;

    arg = arg;
    if (diagno.LastEventIndex != PreviousLastEventIndex)
    {
        /* L'indice dell'ultimo evento e' cambiato dall'ultima richiesta, controlla se e' cambiato */
        /* anche l'indice del primo evento */
        stream_seek(diagno.Stream,1);
        stream_read(diagno.Stream, buf, 0x100);
        ptH = (struct t_diagHeader *) buf;

        diagno.FirstEventIndex = ptH->Index;

        PreviousLastEventIndex = diagno.LastEventIndex;
    }

    sprintf(tempstr,"DENDX OK %u %u",diagno.FirstEventIndex,diagno.LastEventIndex);

    query_AppendReply(tempstr);
}

/* Cerca un record che contiene l'index cercato, il record viene restituito in bufQuery */
unsigned short GotoIndex(unsigned short Index)
{
    unsigned short RecordCount;
    volatile struct t_diagHeader *ptH;
    char   Found;
    unsigned short i;

    RecordCount = stream_recCount(diagno.Stream);

    printf("Requested:%u\n",Index);
    printf("RecCount: %u\n",RecordCount);
    printf("Current:  %u\n",diagno.CurrentIndex);

    ptH = (volatile struct t_diagHeader *) bufQuery;
    Found = 1;
    if (Index != diagno.CurrentIndex)
    {
        /* Il record richiesto non e' gia' il corrente record selezionato */
        printf("non corrente\n");
        if ((Index == (diagno.CurrentIndex+1)) && (diagno.CurrentRecNumber != 0) )
        {
            printf("successivo\n");
            /* deve passare al record successivo */
            stream_read(diagno.Stream, bufQuery, 0x1000);
            Found = (Index == ptH->Index);
            diagno.CurrentIndex = ptH->Index;
            diagno.CurrentRecNumber++;
            printf("Index: %u\n",diagno.CurrentIndex);
        }
        else
        {
            printf("ricerca dall'inizio\n");
            /* cerca il record con l'indice richiesto */
            stream_seek(diagno.Stream,1);
            i = 1;
            Found = 0;
            while ((i<=RecordCount) && !Found)
            {
                stream_read(diagno.Stream, bufQuery, 0x1000);
                Found = (Index == ptH->Index);
                diagno.CurrentIndex = ptH->Index;
                diagno.CurrentRecNumber=i;
                i++;
            }
        }
    }
    return(Found);
}


/* ****************************************************************************
    $Log: diag.c,v $
    Revision 1.4  2004/11/08 09:20:38  mungiguerrav
    2004/nov/08,lun 08:50          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.3        2004/06/15 10:27:29     buscob
05.00   2003/nov/19,mer             Napoli              Busco
        - Si agg. gestione delle funzioni per la restituzione ad ATRterm dei nomi
          e dei valori delle variabili memorizzate. Queste devono essere definite
          dall'applicativo e devono essere passate a diag_init(...)
        - Si sostituiscono int con short
04.01   2003/lug/03,gio             Napoli              Busco
        - Si agg. gestione dell'indice progressivo di evento diagnostico
        - nAnte ed nPost si riducono da short ad unsigned char per evitare
          di aumentare le dimensioni dell'header.

    Revision 1.2        2003/02/12 10:27:29     buscob
04.00   2002/nov/04,lun             Napoli              Busco
        - Si agg. gestione di un record EventData per la memorizzazione
          di un unico campione per evento.

    Revision 1.1        2002/05/17 08:45:50     accardol
    - Inserimento in CVS della versione in lib07c
    Revision 1.1.1.1    2002/05/17 08:45:50     accardol
    - Inserimento in CVS in branch STP della versione in lib07c
03.00   2002/mag/17,ven             Napoli              Busco
        - Fatta il 2000/nov/20,lun
        - Si agg. comandi query per futura versione ATRterm:
          gestquery_
          _DIDS     Richiesta IDentification String
          _DRC      Richiesta Record Count
          _DRI      Richiesta Record Info sul record selezionato
          _DVN      Richiesta Var Names
          _DRS      Record Select
          _DDL      Richiesta Data Line sul record selezionato

 ------------------------   Prima di CVS    ------------------------
02.06   2000/ott/30,lun             Napoli              Busco
        - Il comando diag ora gestisce indici di record negativi
          (diag sl -5) elenca gli ultimi 5 eventi
02.05   2000/ott/18,mer             Velim               Busco
        - Si gestisce la ID string
02.04   2000/lug/14,ven             Napoli              Schiavo
        - Si agg. funzione tPrintCodEnabledFunc PrintEnabledDescrFunc
          per abilitare stampa del codice
02.03   2000/mar/07,mar             Arezzo              Schiavo
        - Si corr. stampa della descrizione codice diagnostico  nel caso sm
02.02   2000/mar/03,ven             Napoli              Busco/Schiavo
        - Si agg. stampa della descrizione codice diagnostico
02.01   1999/dic/14,mar             Napoli              Busco/Mungi
        - Si agg. stampa dei byte usati e totali nel comando [diag sm]
02.00   1999/dic/14,mar             Napoli              Busco/Mungi
        - Si agg. gestione nPost
        - Si agg. cache FIFO per gestione ultimi trigger
        - writeRec(): Si ottimizza gestione cache[index].header
        - Il buffer totale e' passato dall'applicativo e non piu'
          allocato dinamicamente
01.03   1999/dic/13,lun             Napoli              Busco/Mungi
        - Si agg. l'opzione [diag INIT]
        - Si disabilita l'append mentre e' in corso un comando di lettura DIAG
01.02   1999/dic/07,mar             Napoli              Busco/Mungi
        - Si promuove   struct t_summaryEntry   summary[MAXSUMENTRY];
          da locale a statica di modulo per inspiegati problemi
          di blocco programma (stack overflow ?)
01.01   1999/dic/07,mer             Napoli              Busco/Mungi
        - Si promuove char buf[0x1000]  da locale a static di modulo
        - Si agg. opzioni stampa lu per due variabili long
01.00   1999/ott/01,ven             Napoli              Busco
        - Creazione
*/

