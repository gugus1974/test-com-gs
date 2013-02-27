/********************************************************************/
/* 2004/nov/05,ven 16:10                \SWISV\LIB\SRC\consolle.c   */
/********************************************************************/
/*
    $Date: 2004/11/05 16:32:56 $          $Revision: 1.5 $
    $Author: mungiguerrav $
*/


#include "string.h"
#include "ctype.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdarg.h"
#include "consolle.h"
#include "query.h"

#include "consio.h"

/************************/
/*      Parametri       */
/************************/
#define MAX_CMD             70      /* numero massimo di comandi gestibili          */
#define MAX_TOKEN           30      /* numero massimo dei token su linea di comando */
#define MAX_LEN_PROMPT      14      /* lunghezza massima del prompt                 */
#define MAX_LEN_RIGA        80      /* massima lunghezza della riga di comando      */

/************************************************/
/*      Struttura per la gestione dei comandi   */
/************************************************/
struct cmdEntry
{
    char        *nomeCmd;           /* puntatore alla stringa comando               */
    char        *sintassiCmd;       /* puntatore alla stringa di sintassi           */
    char        *helpCmd;           /* puntatore alla stringa di help               */
    void        (*fGest)(short argc, char* argv[]);
                                    /* puntatore alla funzione di gestione          */
    short       attivo;             /* flag di comando disponibile                  */
} cmdList[MAX_CMD];

/************************************************/
/*      Variabili per la gestione dei comandi   */
/************************************************/
static  short   numCmd  = 0;        /* numero di comandi nella lista                */
static  short   numParam;           /* numero di par. sulla linea di comando        */
static  char    *param[MAX_TOKEN];  /* puntatori alle stringhe parametro            */


/************************************************/
/*      Variabili per gestione riga di comando  */
/************************************************/
static  char    riga[MAX_LEN_RIGA+5];       /* buffer per la riga di comando        */
static  short   lenRiga;                    /* lunghezza effettiva della riga       */
static  char    prompt[MAX_LEN_PROMPT+1];   /* stringa di prompt                    */

/************************************************/
/*      prototipi delle funzioni statiche       */
/************************************************/
static  void    cmdH(               short argc, char* argv[]);

static  void    interpretaComando(  void);
static  short   cercaCmd(           char *nome_cmd);

/****************************************/
/* inizializzazione consolle di comando */
/****************************************/
void consolle_init(void)
{
    riga[0]         = '\0';
    lenRiga         = 0;
    consolle_prompt("");

    consolle_addcmd("h",     "[comando]",              "vis. help",             cmdH);
}

/************************************************************************/
/* gestione dell'output formattato sulla seriale                        */
/************************************************************************/
void consolle_print(const char *format, ...)
{
    va_list    args;
    char       s[256];
    short      i;

    va_start(args,format);
    vsprintf(s,format,args);
    for(i=0; (s[i]!=0) && (i<256); i++)
    {
        consio_putchar(s[i]);
    }

    va_end(args);
}

/*************************************************************************/
/* scandisce una stringa e restituisce il numero di token ed un array di */
/* puntatori agli stessi                                                 */
/*************************************************************************/
void consolle_parsestr(char* stringa, short* numToken, char* token[])
{
    short   i;                      /* indice al carattere della stringa            */
    short   stato;                  /* si trova su token = 1; spazio = 0            */
    char    c;

    i           = 0;                /* inizia dal primo carattere della stringa     */
    *numToken   = 0;                /* non ha trovato ancora nessun token           */
    stato       = 0;                /* parte dallo stato spazio                     */

    while( ( (c=stringa[i]) != '\0') && (*numToken < MAX_TOKEN) )
    {
        if(stato)
        {
            if(isspace(c))
            {
                stringa[i]  = '\0';
                stato       = 0;
            }
        }
        else
        {
            if(isgraph(c))
            {
                token[(*numToken)++]    = &stringa[i];
                stato                   = 1;
            }
        }
        i++;
    }
}

/******************/
/* Parse a number */
/******************/
long consolle_parsenum(const char *s)
{
    long l = 0;

    if      (s[0] == '$')
    {
        sscanf(s+1, "%lx", &l);
    }
    else if (s[0] == '0' && toupper(s[1]) == 'X')
    {
        sscanf(s+2, "%lx", &l);
    }
    else
    {
        sscanf(s,   "%ld", &l);
    }

    return l;
}

/************************************************************/
/* gestione della riga di comando ed esecuzione dei comandi */
/************************************************************/
void consolle( void )
{
    char c;

    /* preleva un caratteri mediante la call-back function */
    /* definita dalla libreria di scheda                   */
    c   = consio_getkey();

    switch(c)
    {
        case EOF:
        break;

        case '\r':
            lenRiga     = 0;
            consolle_print("\n");
            /* interpreta il comando contenuto in riga */
            interpretaComando();
            consolle_print("%s",prompt);
        break;

        case '\b':
            if (lenRiga > 0)
            {
                riga[--lenRiga]     = '\0';
                consolle_print("\r%s%s \r%s%s",prompt,riga,prompt,riga);
            }
        break;

        case '\t':
            riga[0]     = '\0';
            lenRiga     = 0;
            consolle_print("\n%s",prompt);
        break;

        default:
            /* filtra i codici di controllo  */
            if (isprint(c))
            {
                if (lenRiga < MAX_LEN_RIGA)
                {
                    riga[lenRiga++]     = c;
                    riga[lenRiga]       = '\0';
                    consio_putchar(c);
                }
            }
        break;
    }
}

/***********************************/
/* imposta il prompt del terminale */
/***********************************/
void consolle_prompt(char *promptIn)
{
    strncpy(prompt,promptIn,MAX_LEN_PROMPT);
    consolle_print("%s",prompt);
}

/**********************************/
/* aggiunge un comando alla lista */
/**********************************/
void consolle_addcmd( char *nomeCmd,char *sintassiCmd, char *helpCmd, void (*fGest)() )
{
    if (numCmd < MAX_CMD-1)
    {
        cmdList[numCmd].nomeCmd     = nomeCmd;
        cmdList[numCmd].sintassiCmd = sintassiCmd;
        cmdList[numCmd].helpCmd     = helpCmd;
        cmdList[numCmd].fGest       = fGest;
        cmdList[numCmd].attivo      = 1;
        numCmd++;
    }
}

/*********************/
/* attiva un comando */
/*********************/
void consolle_cmdon( char *nomeCmd )
{
    short i;

    if(nomeCmd == NULL)
    {
        for (i=0;i<numCmd;i++)
        {
            cmdList[i].attivo   = 1;
        }
    }
    else
    {
        if (( i = cercaCmd(nomeCmd) ) != -1)
        {
            cmdList[i].attivo   = 1;
        }
    }

}

/************************/
/* disattiva un comando */
/************************/
void consolle_cmdoff( char *nomeCmd )
{
    short i;

    if(nomeCmd == NULL)
    {
        for (i=0;i<numCmd;i++)
        {
            cmdList[i].attivo   = 0;
        }
    }
    else
    {
        if (( i = cercaCmd(nomeCmd) ) != -1)
        {
            cmdList[i].attivo   = 0;
        }
    }
}

void consolle_comando(char* rigaComando)
{
    strcpy(riga,rigaComando);
    interpretaComando();
}

static void interpretaComando(void)
{
    short   numComando;

    /***********************************************/
    /* effettua la scansione della riga di comando */
    /***********************************************/
    consolle_parsestr(riga, &numParam, &param[0]);

    /***********************************/
    /* ricerca il comando nella lista  */
    /***********************************/

    if (numParam > 0)
    {
        if ((numComando = cercaCmd(param[0])) == -1)
        {
            consolle_print("ERRORE: Comando Sconosciuto\n");
            riga[0] = 'h';
            riga[1] = '\0';
        }
        else
        {
            /*************************************/
            /* se il comando e' attivo lo esegue */
            /*************************************/
            if (cmdList[numComando].attivo)
            {
                cmdList[numComando].fGest(numParam,param);
            }
            else
            {
                consolle_print("Comando disabilitato\n");
            }
        }
    }
}

/*******************************************************************/
/* cerca un comando nella lista e restituisce l'indice della lista */
/* (restituisce -1 se il comando non e' stato trovato)             */
/*******************************************************************/
static short cercaCmd(char *nomeCmd)
{
    short i = 0;

    while ((i < numCmd) && (strcmp(nomeCmd,cmdList[i].nomeCmd) != 0))
    {
        i++;
    }

    if (i == numCmd)
    {
        i = -1;
    }

    return(i);
}

/*************************/
/* gestione comando help */
/*************************/
static void cmdH(short argc, char* argv[])
{
    short   i;
    char    tmp[128];

    if ((argc == 2) && (i = cercaCmd(argv[1])) != -1)
    {
        sprintf(tmp,"%s %s",cmdList[i].nomeCmd,cmdList[i].sintassiCmd);
        consolle_print("%-30s = %s\n",tmp,cmdList[i].helpCmd);
    }
    else
    {
        consolle_print("COMANDI A DISPOSIZIONE:\n");
        consolle_print("[TAB]                          = cancella riga\n");
        consolle_print("[BackSpace]                    = cancella carattere\n");
        for (i=0; i < numCmd; i++)
        {
            if (cmdList[i].attivo)
            {
                sprintf(tmp,"%s %s",cmdList[i].nomeCmd,cmdList[i].sintassiCmd);
                consolle_print("%-30s = %s\n",tmp,cmdList[i].helpCmd);
            }
        }
    }
}


/* ****************************************************************************
    $Log: consolle.c,v $
    Revision 1.5  2004/11/05 16:32:56  mungiguerrav
    2004/nov/05,ven 16:10          Napoli       Mungi
    - Si aggiungono keyword CVS e monumentale storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.4        2004/06/15 11:17:04     buscob
05.10   2003/nov/29,sab             Madrid              Mungi
        - Si applicano le regole sulle parentesi graffe e sui nomi dal Manuale del SW
05.09   2003/nov/21,ven             Napoli              Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso.

    Revision 1.3        2003/03/03 16:18:34     accardol
    Revision 1.2        2003/03/03 15:39:50     accardol
05.08   2003/mar/03,lun             Napoli              Accardo/Busco
        - Si elim. dich. inutile:
            extern struct var_entry vardb[QUERY_MAXVAR];

    Revision 1.1        2001/11/22 12:08:18     accardol
    - Inserimento in CVS della versione in lib07d
    Revision 1.1.1.1    2001/11/22 12:08:18     accardol
    - Inserimento in CVS in branch STP della versione in lib07d
05.07   2001/nov/22,gio             Napoli              Mungi
        - Si elim. la inutile static nulla

 ------------------------   Prima di CVS    ------------------------
05.06   2001/nov/08,gio             Napoli              Mungi\Busco
        - Si agg. include consio.h
        - Si sostituiscono le callback CONS_in() e CONS_out()
          rispettivam. con cpudrv_consIn() e cpudrv_consOut()
05.05   2000/feb/05,sab             Napoli              Mungi
        - Si elim. interf. di compatibilita' tra consolle_addavar(..) e
          query_addvar(..): e' meglio che gli applicativi si adeguino subito
05.04   2000/feb/02,mer             Napoli              Busco/Mungi
        - Si sposta in modulo view.* il ramo di stato_consolle=ST_VIEWTERM
          e si abolisce stato_consolle e lo switch relativo
05.03   2000/feb/02,mer             Napoli              Busco/Mungi
        - Si elimina la gest. in bkg di query_replystr e trcbk_print
05.02   2000/feb/01,mar             Napoli              Mungi
        - Si spostano in modulo trcbk.* le gestioni di trace e brk
        - Si spostano in modulo memd.*  le gestioni di mem, dump e '.'
        - Si adeguano le stampe a nuova struct var_entry di query.h v.1.01-2000/gen/24
05.01   1999/dic/06,lun             Napoli              Mungi/Busco
        - Si elim. i remapping consolle_query() e consolle_cercavar()
05.00   1999/dic/03,ven             Napoli              Busco/Mungi
        - Si spostano in modulo query.* le gestioni di:
               - query vera e propria
               - variabili
        - Si usano i formati di stampa .frmtaux invece di .frmt nelle stampe di view e trace
04.07   1999/nov/19,ven             Napoli              Schiavo/Mungi
        - Si assorbono modifiche di E500P\LIB\HC  del 26.lug.99
          Si aumenta:   MAX_VAR         da  70      a    80
                        MAX_LEN_QUERY   da 160      a   200
04.06   1999/giu/10,gio             Napoli              Mungi
        - Il comando view fa partire la stampa solo se si inserisce senza parametri,
          eliminando quindi la partenza automatica allo start-up delle applicazioni
          che definiscono le variabili e lo step associato al view
04.05   1999/apr/15,gio             Napoli              Mungi
        - Si continua ad eliminare i "." ad inizio righe di stampa
04.04   1999/apr/08,gio             Napoli              Mungi
        - Si sost.         "\r\n"          con     "\n"
        - Si eliminano i "." ad inizio righe di stampa
04.03   1999/feb/25,gio             Napoli              Mungi
        - Si incr. matrice trace da 3 x 100   a   9 x 100
        - La stampa di matrice trace e' tempificata da query_prn
          gest_brk(), consolle_brk_test(): si aggiunge pretrigger
04.02   1998/nov/05,gio             Caserta             Mungi
        - Si aum.   MAX_VAR e MAX_CMD     da  60     a  70
04.01   1998/feb/21,sab             Napoli              Mungi/Busco
        - Si mod. _addvar(..,void far *pvar,...)
04.00   1998/feb/13,ven             Napoli              Busco/Mungi
        - Si intr. consolle_query() per gest. query_step controllata da scheda e non da ATRterm
        - Si intr. timeout di invio query
        - Si sost. vartoken   con   vartoken[MAX_TOKEN]
        - Si sost.:     static send_query()     (che inviava dati a comando ATRterm)
          con:          static init_query()     (che inizializza vartoken[x] e gestisce timeout da ATRterm)
        - Si intr. varCamp[MAX_TOKEN] per migliorare campionamento
        - Si agg. alla fine di consolle() l'invio effettivo dei dati verso ATRterm
        - Si corregge baco su gest. MAX_LEN_QUERY (\0 in query[MAX],
          ossia fuori dal campo di validita') e incr. da 80 a 160
03.13   1998/feb/10,mar             Napoli              Mungi
        - Si aum. MAX_LEN_PROMPT da 10 a 14
        - Si intr. gestione frmt per esadecimali
03.12   1997/nov/21,ven             Napoli              Mungi/Busco
        - Si aum. MAX_VAR e MAX_CMD a 60
03.11   1997/nov/19,mer             Napoli              Mungi
        - Si agg. print_view0 ad inizio e fine view
        - Si agg. stampa '. ' ad inizio messaggi
        - Si agg. gest_punto() per inserire commenti
        - Si modif. view in modo che se non vi sono errori, la stampa
          parta subito dopo l'immissione di nuove var o di nuovo step
03.10   1997/ago/26,mar
        - Si aum. MAX_VAR da 30 a 40  e MAX_CMD da 30 a 50
03.09   1997/mag/07,mer
        - In gest_mem si modifica cast per adeg. nuovo ambiente
03.08   1997/mar/26,mer
        - Si agg. possibilita' di scrittura al comando dump
        - Si controlla 1=<size=<0x100
        - 'mem' ristampa valore ultimo indirizzo
03.07   1997/mar/14,ven
        - Si agg. comando dump;
        - Si agg. routine consolle_parsenum per input numeri esadecimali nei comandi mem e dump;
        - Si rinomina parse_str in consolle_parsestr
03.06   1997/feb/19,mer
        - Si elim var. tmp non piu' utilizzata in consolle_init()
        - Si agg. prototipi di cerca_cmd,COS_in e CONS_out
        - Si eliminano nomi dei par. in gest_vers per evitare warniong par. non usato
        - Si sost. &tokenquery[0] a &tokenquery nella chiamata a parse_str() per compatibilita' ANSI
        - Si setta il prompt di default a stringa nulla
        - L'help stampa il nome del comando oltre che la sintassi
        - Si eliminano i nomi dei comandi dalle stringhe di sintassi dei comandi di consolle
        - consolle_cmdon() e consolle_cmdoff() abilitano/disabilitano tutti i
          comandi se chiamate con parametro NULL
        - Si agg. parametro nome_comando in help
        - Si elim. 'Errore comando sconosciuto' su linea di comando vuota
03.05   1997/feb/17,lun
        - Si sost.   \n\r     con    \r\n    per compatib. con editor
        - Si cambia consolle_print_view()  in   print_view()
03.04   1997/gen/28,mar
        - In send_query() si restituisce il token se non e' un nome var
        - Si rende consolle_print() non statica
03.03   1997/gen/27,lun
        - Si agg. la funzione print()
        - Si sostituiscono le chiamate a printf con print per
          rimandare alla libreria la gestione dell'output
        - Si sost. &param[0] a &param e &tokenquery[0] a &tokenquery
          nella chiamata a parse_str() per compatibilita' ANSI
03.02   1997/gen/24,ven
        - Si utilizza la call-back function CONS_in()
03.01   1997/gen/07,mar
        - Si rendono static tutte le funzioni gest_*** di consolle
        - Si cambiano le variabili di tipo bit in unsigned int per compatibilita' ANSI
        - Si agg. view *
        - Si agg. gestione query
03.00   1997/gen/02,gio
        - Si aumenta la lunghezza della riga di comando ad 80 caratteri
        - La funzione estrai_var diventa static
        - Si agg. la define MAX_PAR per il numero massimo di parametri
        - Si eliminano le var. com, arg1, arg2, arg3
        - Si agg. le var nparam e param[]
        - Si modificano i par. di scambio delle funz. di gestione comandi
        - Si gestisce correttamente il quarto par del comando view
        - Si filtrano tutti i caratteri non stampabili
        - Si spostano i comandi monitor e dac in libdan6
        - Si sposta l'interpretazione della riga di comando nella funzione
          consolle_comando richiamabile dall'esterno di consolle
        - Si elimina la funzione consolle_var
        - Alla funzione consolle_cercavar si fa ritornare il puntatore alla var nella lista
02.13   1996/ott/17,gio
        - Si filtra il carattere 0x11 inviato periodicamente dal monitor
        - Si agg. messaggio di comando disabilitato
        - Si inverte ordine test nei while di cerca_cmd() e consolle_var()
          per problemi di accesso a vme bus in background
        - Si rende static di modulo consolle_var()
02.12   1996/ago/28,mer
        - Si agg. 4^ colonna in stampa view
02.11   1996/ago/28,mer
        - Si agg. stampa step alla fine del view
02.10   1996/lug/26,ven
        - Si rendono com,arg1,arg2,arg3,riga e riga_old static
02.09   1996/lug/19,ven
        - Si agg. ramo else in gest_view
02.08   1996/lug/18,gio
        - Si agg. gest. brk_tipo per BRK trace e BRK PROGRAMMA
02.07   1996/lug/15,lun
        - Si rende il view multiplo a tre variabili
02.06   1996/mag/29,mer
        - Si agg. comando vers per 166 e DSP
02.05   1996/mag/24,ven
        - Si agg. stampe sul comando monitor
02.04   1996/mag/16,gio
        - Si agg. comando monitor
02.03   1996/mar/21,gio
        - Si agg. var. nulla per inizializz. puntatori
        - Si elim. near nella dichiaraz. puntatori
02.02   1996/mar/07,gio
        - Elim. include "serbuf.h" e chiamata serbuf_init(baud)
02.01   1996/mar/01,ven
        - Si agg. funzione estrai_var per ottimizzazione
02.00   1996/feb/23,ven
        - Si ottimizza consolle_dac aggiungendo le variabili struct var_entry near var_dac_a,var_dac_b
        - Si ottimizza consolle_trace aggiungendo le variabili struct var_entry near var_trace1..3
        - Si ottimizza consolle_view e consolle aggiungendo le variabili struct var_entry near var_view
        - Si ottimizza consolle_brk_test
01.06   1996/feb/19,lun
        - Si elimina include di "reg166.h" e "bitfield.h"
        - Si elimina bug in gest_trace che comportava il blocco di trace.
        - Nel trace l'indice parte ora sempre da 0
01.05   1995/ott/18,mer
        - Si elimina inizializzazione a NULL della var_list[]
        - Si aggiunge alla struct var_entry l'informazione del tipo di variabile: 0 = int, 1 = unsigned int;
          questa info e' utile alla stampa della variabile
        - Nella struct trace_entry si sostituisce l'array indice_var[3]
          alle tre variabili indice_var1,..,indice_var3
        - Eliminato bug nel comando trace (si verificava quando si richiedeva il trace di una
          variabile non esistente)
01.04   1995/ott/17,mar
        - Si agg. comando dac
        - Si agg. view_com.attivo=0 in consolle()
        - Si riduce brk.command da 100 a 50 caratteri
01.03   1995/ott/09,lun
        - Si aggiunge comando sfr166;
        - Si sposta printf del view in consolle()
01.02   1995/ott/06,ven
        - Si aumentano MAX_CMD e MAX_VAR da 20 a 30
01.01   1995/ott/03,mar
        - Si rende val_list visibile dall'esterno per gli altri comandi
        - Si cambia cerca_var in consolle_var visibile dall'esterno
01.00   1995/set/28,gio - Creazione
*/
