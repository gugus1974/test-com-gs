/****************************************************************************************/
/* 2004/nov/15,lun 08:50                                         \SWISV\LIB\SRC\query.c */
/*                                                                                      */
/* Module: QUERY                                                                        */
/* Description: Sistema per l'interrogazione del sistema                                */
/*                                                                  IMPLEMENTATION FILE */
/****************************************************************************************/
/*
    $Date: 2004/11/16 09:45:14 $          $Revision: 1.5 $
    $Author: mungiguerrav $
*/


/* definendo il simbolo qui sotto si ottiene la compatibilita' con ATRterm 4.x */
#define ATRTERM4

#include "string.h"     /* gestione stringhe */
#include "ctype.h"
#include "stdlib.h"
#include "stdio.h"
#include "query.h"

#define BUFFERLEN     256  /* Massima lunghezza della stringa di query  */
#define NSTRTSTPCHAR  2    /* Numero di start ed end char da inserire   */


/* Stati interni possibili */
#define STATUSDISABLED  0    /* Query disabilitate  */
#define STATUSWAIT      1    /* Il sistema e' in attesa di nuova stringa di query */
#define STATUSGET       2    /* E' in corso una acquisizione di stringa di query */

/*****************************************/
/* struttura per la gestione dei comandi */
/*****************************************/
struct CmdEntry
{
    char   *nome_cmd;       /* puntatore alla stringa comando      */
    void   (*f_gest)(char* arg);
};

/***************************************/
/* Prototipi funzioni locali di modulo */
/***************************************/
static  void    strCheckSum(char bytCheckSum, char *strCS);
static  short   calcCheckSum(char *str, short len);
static  short   SearchCommand(char *strcmd);

#ifdef ATRTERM4
static void parsestr(char* stringa, unsigned short* numToken, char* token[], short maxnumToken);
#endif

static struct CmdEntry *CmdList;        /* Puntatore alla lista di comandi query */
static unsigned short   CmdListSize;    /* Spazio a disposizione per la lista di comandi */
static unsigned short   CmdListNumItem; /* Numero di comandi registrati */
static char             startchar;      /* Carattere da usare come START del pacchetto */
static char             endchar;        /* Carattere da usare come STOP del pacchetto */
static char             sepchar;        /* Carattere da usare per separare il checksum dai dati */
static char             status;         /* Stato attuale di query */
static char             querybuf[BUFFERLEN];
static short            querybuflen;
static char             OutputBuffer[BUFFERLEN];/* Buffer di output */

#ifdef ATRTERM4
static short   mblnMode;                      /* abilitazione funzionalita' avanzate */
#endif

/******************************************/
/* Inizializzazione dell'interprete query */
/******************************************/
void query_init(char *list, unsigned short size)
{
    CmdList             = (struct CmdEntry *) list;
    CmdListSize         = size;
    CmdListNumItem      = 0;

    startchar           = '@';
    endchar             = '#';
    sepchar             = '>';
    status              = STATUSDISABLED;
    querybuflen         = 0;

#ifdef ATRTERM4
    mblnMode            = 0;
#endif

}

/******************************************/
/* Aggiunta di un comando all'interprete  */
/******************************************/
short query_addcmd(char *nome, void (*f_gest)(char* arg))
{
    if (sizeof(struct CmdEntry) * (CmdListNumItem+1) < CmdListSize)
    {
        CmdList[CmdListNumItem].nome_cmd = nome;
        CmdList[CmdListNumItem].f_gest   = f_gest;
        CmdListNumItem++;
        return(1);
    }
    else
        return(0);
}

/*************************************************************************/
/* Abilita il filtraggio dei caratteri per l'interpretazione delle query */
/*************************************************************************/
void query_enable(void)
{
    querybuf[0]  = '\0';
    querybuflen  = 0;
    status       = STATUSWAIT;
}

/****************************************************************************/
/* Disabilita il filtraggio dei caratteri per l'interpretazione delle query */
/****************************************************************************/
void query_disable(void)
{
    status      = STATUSDISABLED;
}

/****************************************************************************/
/* Imposta il modo di funzionamento della query:                            */
/* 0: compatibile con le vers. di ATRterm antecedenti alla 5.0              */
/* 1: funzioni di checksum e carattere start/stop multiplo abilitate        */
/****************************************************************************/
void query_setmode(short mode)
{
#ifdef ATRTERM4
    mblnMode = mode;
#else
    mode = mode;
#endif
}

/****************************************************************************/
/* Restituisce lo stato del modulo query                                    */
/****************************************************************************/
short query_status(void)
{
    short retval;

    if (status == STATUSDISABLED)
    {
        retval = 0;
    }
    else
    {
#ifdef ATRTERM4
        if (mblnMode)
        {
#endif
            retval = 2;

#ifdef ATRTERM4
        }
        else
        {
            retval = 1;
        }
#endif

    }
    return(retval);
}

/*******************************************************************/
/* Filtra i caratteri che costituiscono una query.                 */
/* Restituisce il medesimo carattere se questo non fa parte di una */
/* query o altrimenti EOF                                          */
/*******************************************************************/
char query_filter(char c)
{
    if (c != EOF)
    {
        char  retchar;

        switch(status)
        {
            case STATUSDISABLED:
            /* Le query sono disabilitate                */
            /*   - Non esegue alcun filtro sui caratteri */

                retchar = c;
            break;

            case STATUSWAIT:
            /* E' in attesa di una nuova stringa di query */
                if (c == startchar)
                {
                    /* Reinizializza la stringa query e forza lo stato STATUSGET */
                    querybuf[0] = '\0';
                    querybuflen = 0;
                    status      = STATUSGET;
                    retchar     = EOF;
                }
                else if (c == endchar)
                {
#ifdef ATRTERM4
                    /* Mantiene attivo l'output                      */
                    /* si autogenera un comando VV ogni 20 caratteri */
                    /* di fine query                                 */

                    static short counter = 0;
                    short        numcmd;

                    if (!mblnMode && !counter)
                    {
                        counter = 20;
                        numcmd = SearchCommand("VV");

                        /* Se e' stato trovato esegue il comando */
                        if (numcmd > -1)
                        {
                            CmdList[numcmd].f_gest("");
                        }
                    }

                    if (counter)
                        counter--;
#endif
                    retchar    = EOF;
                }
                else
                    retchar = c;   /* E' arrivato un normale carattere */
            break;

            case STATUSGET:
            /* E' in corso l'acquisizione di una nuova stringa di query */
                if (c == startchar)
                {
                    /* Reinizializza la stringa query  */
                    querybuf[0] = '\0';
                    querybuflen = 0;
                }
                else if (c == endchar)
                {
                    /**********************************/
                    /* E' arrivato un pacchetto query */
                    /**********************************/
#ifdef ATRTERM4
                    if (mblnMode)
                    {
#endif
                        short CheckSumOK;

                        CheckSumOK = 0;

                        /* Calcola il checksum del pacchetto arrivato */
                        if (!calcCheckSum(querybuf,-1))
                        {
                            char *pEnd;

                            /* Il CheckSum e' OK, controlla la presenza */
                            /* del separatore dati>checksum             */
                            pEnd = strrchr(querybuf,sepchar);

                            if (pEnd)
                            {
                                /* separa i dati dal checksum */
                                *pEnd = '\0';
                                CheckSumOK = 1;
                            }
                        }


                        if (CheckSumOK)
                        {
                            char *pStartParam;

                            /* Cerca lo spazio dopo il nome comando */
                            pStartParam = strchr(&querybuf[0],' ');

                            if (pStartParam)
                            {
                                short numcmd;

                                /* Se lo trova lo sost. con termine stringa */
                                *pStartParam = '\0';

                                /* Modifica il puntatore per puntare all'inizio */
                                /* della stringa di parametri */
                                pStartParam++;

                                numcmd = SearchCommand(querybuf);

                                /* Se e' stato trovato esegue il comando */
                                if (numcmd > -1)
                                {
                                    CmdList[numcmd].f_gest(pStartParam);
                                }
                            }
                        }
#ifdef ATRTERM4
                    }
                    else
                    {
                        /* Se Mode == 0 per compatibilita' con ATRterm attuale */
                        /* Questa parte di codice verra' eliminata dopo la     */
                        /* transizione definitiva ad ATRterm 5.0               */

                        unsigned short  temp_numvarquery;
                        char*           Tokens[100];        /* puntatori ai Tokens  */
                        short           numcmd;

                        /* Individua i singoli token nella stringa di query */
                        parsestr(querybuf,&temp_numvarquery,&Tokens[0],100);

                        if ( temp_numvarquery > 0 )
                        {
                            short iStep;

                            /* C'e' almeno un token */
                            iStep = atoi(Tokens[0]);
                            if (iStep>0 && iStep<=1000)
                            {
                                /* Il primo token e' un numero -> lo interpreta   */
                                /* come query step                                */
                                /* si autogenera un comando VT per l'impostazione */
                                /* del tempo di campionamento                     */

                                numcmd = SearchCommand("VT");

                                /* Se e' stato trovato esegue il comando */
                                if (numcmd > -1)
                                {
                                    CmdList[numcmd].f_gest(Tokens[0]);
                                }

                            }
                            else
                            {
                                char tempstr[30];

                                /* Si autogenera il comando per Disattivare tutte le variabili */
                                numcmd = SearchCommand("VOFF");

                                /* Se e' stato trovato esegue il comando */
                                if (numcmd > -1)
                                {
                                    CmdList[numcmd].f_gest("");
                                }

                                /* imposta i nomi delle variabili */
                                /* si autogenera un comando VS */
                                numcmd = SearchCommand("VS");

                                /* Se e' stato trovato esegue il comando */
                                if (numcmd > -1)
                                {
                                    short i;

                                    /* Genera i comandi elementari di */
                                    /* impostazione variabili         */
                                    for(i=0;i<temp_numvarquery;i++)
                                    {
                                        sprintf(tempstr,"%u %s",i+1,Tokens[i]);
                                        CmdList[numcmd].f_gest(tempstr);
                                    }
                                }
                            }
                        }
                    }
#endif

                    /* Riporta lo stato a quello normale */
                    status = STATUSWAIT;
                }
                else
                {
                    /* Aggiunge il carattere alla stringa di query */
                    if (querybuflen < BUFFERLEN-1)
                    {
                        querybuf[querybuflen++] = c;
                        querybuf[querybuflen] = '\0';
                    }
                }
                retchar     = EOF;
            break;
            default:
                retchar     = EOF;
            break;
        }
        return(retchar);
    }
    else
        return(c);
}

/***********************************/
/* Restituisce il buffer di output */
/***********************************/
char* query_replystr(void)
{
    char* RetVal;

    if (OutputBuffer[0])
        RetVal = OutputBuffer;
    else
        RetVal = NULL;

    return(RetVal);
}

/***********************************/
/* Azzera il buffer di output */
/***********************************/
void query_ResetReplyStr(void)
{
    OutputBuffer[0] = '\0';
}

/********************************************************/
/* Accoda una stringa di risposta nel buffer di uscita  */
/* correndandola dei caratteri di start/stop e checksum */
/* Restituisce l'esito dell'append                      */
/* 0 = Buffer pieno, append non effettuato              */
/* 1 = Append OK                                        */
/********************************************************/
short query_AppendReply(char *replystr)
{
    short   RetVal;
    char    strstart[NSTRTSTPCHAR+2];
    char    strstop[NSTRTSTPCHAR+2];
    char    strcheck[10];

    RetVal = 0;
    if (replystr != NULL)
    {
#ifdef ATRTERM4
        if (mblnMode)
        {
#endif
            short intAuxStrLen;
            short intReplyStrLen;

            /* C'e' una stringa da inviare */
            /* Compone la stringa di risposta */

            /* Calcola la lunghezza della stringa da appendere al buffer */

            /* deve aggiungere NSTRTSTPCHAR caratteri di start e stop */
            /* piu' due caratteri di checksum e uno di separazione    */
            intAuxStrLen = 2*NSTRTSTPCHAR + 3;
            memset(strstart,startchar,NSTRTSTPCHAR);
            strstart[NSTRTSTPCHAR] = '\0';
            memset(strstop,endchar,NSTRTSTPCHAR);
            strstop[NSTRTSTPCHAR] = '\0';

            /* Aggiunge la lunghezza della stringa di dati */
            intReplyStrLen = strlen(replystr);

            if (BUFFERLEN-strlen(OutputBuffer) > intReplyStrLen+intAuxStrLen+2)
            {
                char CheckSum;

                /* Calcola il checksum */
                CheckSum = calcCheckSum(replystr,intReplyStrLen);
                CheckSum -= sepchar;

                /* Calcola la stringa di chechsum */
                strCheckSum(CheckSum,strcheck);

                strcat(OutputBuffer,strstart);  /* Appende lo start */
                strcat(OutputBuffer,replystr);  /* Appende i dati */
                strcat(OutputBuffer,strcheck);  /* Appende il checksum */
                strcat(OutputBuffer,strstop);   /* Appende lo stop */

                RetVal = 1;
            }
#ifdef ATRTERM4
        }
        else
        {
            short intAuxStrLen;
            short intReplyStrLen;

            /* C'e' una stringa da inviare */
            /* Compone la stringa di risposta */

            /* Calcola la lunghezza della stringa da appendere al buffer */

            /* deve aggiungere 1 carattere di start e stop */
            intAuxStrLen = 2;
            memset(strstart,startchar,1);
            strstart[1] = '\0';
            memset(strstop,endchar,1);
            strstop[1] = '\0';

            /* Aggiunge la lunghezza della stringa di dati */
            intReplyStrLen = strlen(replystr);

            if (BUFFERLEN-strlen(OutputBuffer) > intReplyStrLen+intAuxStrLen+2)
            {
                strcat(OutputBuffer,strstart);  /* Appende lo start */
                strcat(OutputBuffer,replystr);  /* Appende i dati */
                strcat(OutputBuffer,strstop);   /* Appende lo stop */

                RetVal = 1;
            }
        }
#endif

    }
    return(RetVal);
}

/************************************************************************/
/* Calcola il checksum per i primi len caratteri della stringa str      */
/* Se si incontra la fine della stringa viene terminato il calcolo      */
/* Se si specifica -1 come len si esegue il calcolo su tutta la stringa */
/************************************************************************/
static short calcCheckSum(char *str, short len)
{
    char    bytCheckSum;
    short   i;

    /* Calcolo del CheckSum */
    bytCheckSum = 0;
    if (len != -1)
    {
        for(i=0;(i<len) && str[i];i++)
            bytCheckSum += str[i];
    }
    else
    {
        for(i=0;str[i];i++)
            bytCheckSum += str[i];
    }

    bytCheckSum = 0x0-bytCheckSum;
    return(bytCheckSum);
}

/**************************************************************/
/* Calcola la stringa di caratteri di CheckSum effettuando un */
/* opportuno filtraggio                                       */
/**************************************************************/
static void strCheckSum(char bytCheckSum, char *strCS)
{
    short   len;

    len = 0;

    /* Aggiunge il separatore di checksum */
    strCS[len++] = sepchar;

    /* Deve aggiungere il checksum solo se diverso da 0 */
    if (bytCheckSum != 0)
    {
        /* I caratteri vietati vengono spezzati in due */
        if ((bytCheckSum == startchar) ||
            (bytCheckSum == endchar)   ||
            (bytCheckSum == sepchar)   ||
            (bytCheckSum == 0x0A)      ||
            (bytCheckSum == -1)          )
        {
            strCS[len++] = bytCheckSum-1;
            strCS[len++] = 1;
        }
        else
            /* In tutti gli altri casi puo' aggiungere il checksum */
            strCS[len++] = bytCheckSum;
    }

    /* Termina la stringa */
    strCS[len] = '\0';
}

#ifdef ATRTERM4
/*************************************************************************/
/* Scandisce una stringa, restituisce il numero di token ed un array di  */
/* puntatori agli stessi                                                 */
/* N.B. la stringa originale viene modoficata ed continua ad essere      */
/*      puntata dai puntatori restituiti                                 */
/*************************************************************************/
static void parsestr(char* stringa, unsigned short* numToken, char* token[], short maxnumToken)
{
    short   i;                  /* indice al carattere della stringa    */
    short   stato;              /* si trova su token = 1; spazio = 0    */
    char    c;

    i         = 0;     /* inizia dal primo carattere della stringa */
    *numToken = 0;     /* non ha trovato ancora nessun token       */
    stato     = 0;     /* parte dallo stato spazio                 */

    while( ( (c=stringa[i]) != '\0') && (*numToken < maxnumToken) )
    {
        if (stato)
        {
            if (isspace(c))
            {
                stringa[i] = '\0';
                stato   = 0;
            }
        }
        else
        {
            if (isgraph(c))
            {
                token[(*numToken)++] = &stringa[i];
                stato   = 1;
            }
        }
        i++;
    }
}
#endif

/* Cerca il comando nel database */
/* Restituisce -1 se non trovato */

static short SearchCommand(char *strcmd)
{
    short   found;
    short   n;
    short   retval;

    found  = 0;
    n = 0;

    while((n < CmdListNumItem) && !found)
    {
        if (strcmp(strcmd,CmdList[n].nome_cmd) == 0)
            found = 1;
        else
            n++;
    }

    if (found)
    {
        retval = n;
    }
    else
    {
        retval = -1;
    }

    return(retval);
}


/* ****************************************************************************
    $Log: query.c,v $
    Revision 1.5  2004/11/16 09:45:14  mungiguerrav
    2004/nov/15,lun 08:50          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.4        2004/08/31 14:25:15     mungiguerrav
    2004/ago/31,mar 16:25        Napoli          Mungi\Busco
    - Si effettua Merge da branch makefile

    Revision 1.3.2.1    2004/07/14 15:30:38     buscob
03.03   2004/apr/05,lun             Napoli                  Nappo/Busco
        - Si elimina l'inutile inclusione di vardb.h

    Revision 1.3        2004/06/15 11:45:15     buscob
03.02   2003/nov/21,ven             Napoli              Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso

    Revision 1.2        2003/03/03 14:38:46     buscob
03.01   2003/mar/03,lun             Napoli              Busco
        - Nella modalita' ATRterm 4 si genera una chiamata fittizia al comando VOFF
          quando si cambiano le variabili richieste.
          Questo per evitare che Query continui a mandare le variabili anche se
          vengono disattivate da ATRterm
          Questo bug e' stato introdotto nella versione 03.00 ed in particolare con
          la gestione del vardb separato

    Revision 1.1        2002/05/17 09:44:26     accardol
    - Inserimento in CVS della versione in lib07d
    Revision 1.1.1.1    2002/05/17 09:44:26     accardol
    - Inserimento in CVS in branch STP della versione in lib07d
        La data è arretrata con un artificio
03.00   2002/mag/17,ven             Napoli              Busco
        - Fatta il 2001/nov/28,mer
        - Si scorpora la gestione del database portandola nei moduli vardb e vardbcmd.
          Ora query gestisce solo i comandi query
        - Si rende personalizzabile dall'applicativo il numero di comandi
        - Si cambia QUERY_BUFLEN in BUFFERLEN
        - Avendolo abolito in vardb.h, si sost. VARDB_MAXVAR con 100
        - Si sost. test con assegnazione con assegnazione + test,
          come imposto dalle regole di codifica
        - Si sost. bytChheckSum == 0xFF con == -1, perche' il tipo e' char

 ------------------------   Prima di CVS    ------------------------
02.03   2001/mag/05,sab             Napoli              Oliviero
	    - Corretto errore di mancata dichiarazione variabili
02.02   2001/apr/27,ven             Napoli              Oliviero
	    - Nella modifica 2.01 non erano state eliminate le inizializzazioni
          implicite che quindi vengono eliminate
02.01   2000/nov/29,mer             Napoli              Busco/Oliviero
        - Si agg. funzione query_init() e si eliminano le inizializzazioni
          implicite che contravvengono ad una regola di programmazione.
02.00   2000/lug/24,lun             Napoli              Busco
        - Si agg. funzione  query_setmode(int mode)     che attiva la modalita' di funzionamento
          con caratteri di start/stop query multipli e checksum.
          Questa modalita' non e' compatibile con le versioni di ATRterm antecedenti alla 5.0
        - query_replystr(...)
          se non riconosce una variabile chiesta dalla PTU non risponde con il nome ma con "??".
          Questa modifica si e' resa necessaria per risolvere il problema del blocco della scheda
          sulla nuova richiesta delle variabili
        - Si elimina la variabile queryNSample ed il numero di campione nella risposta verso la PTU
01.04   2000/gen/28,ven             Napoli              Busco/Mungi
        - Si agg. la gestione di queryNSample, aggiunta in coda alla stringa
          di _replystr() per permettere ad ATRterm di ricostruire il tempo che scorre
        - Si utilizza (xxx far *) nei cast per campionare varSample[i]
01.03   2000/gen/24,lun             Napoli              Busco/Balbi
        - Si agg. la possibilita' di gestire le variabili char
        - Si eliminano i formati di stampa dal dbvar
01.02   2000/gen/19,mer             Napoli              Balbi/Busco/Mungi
        - Si rende  retchar  locale ad un ramo di  if(c != EOF) in query_filter
        - Si agg. ramo default allo switch (warning del compilatore KEIL4_3)
01.01   2000/gen/07,ven             Napoli              Busco/Accardo
        - BugFix: provocava a volte il blocco del programma sull'attivazione della Query
        - BugFix: creava un campione con una sola variabile quando ATRterm inviava
                  la stringa di impostazione query_step
        - BugFix: impostava su query_step+1 l'output dei campioni
01.00   1999/dic/03,ven             Napoli              Busco
        - Creazione a partire da LIB\HC\consolle.c v.4.05 del 15.apr.99
        - Si trasforma querybuflen da char a short
*/

