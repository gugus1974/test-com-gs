/****************************************************************/
/* 2004/ott/29,ven 10:25                \SWISV\LIB\SRC\versio.c */
/****************************************************************/
/*      Gestione versioni applicativi e librerie software       */
/****************************************************************/
/*
    $Date: 2004/10/29 08:35:16 $          $Revision: 1.8 $
    $Author: mungiguerrav $
*/


#include "stdio.h"
#include "stdlib.h"
#include "consolle.h"
#include "query.h"
#include "versio.h"

/****************************************/
/*      Variabili locali di modulo      */
/****************************************/
static  unsigned char       *schName;
static  unsigned short      numSW;
static  unsigned short      id;
static  struct versio       *arrayVers[MAX_SW_VERS];

/*********************************************/
/* Prototipi delle funzioni locali di modulo */
/*********************************************/
static  void    cmdVersio(      void);
static  void    gestquery_VER(  char* arg);
static  void    gestquery_PING( char* arg);
static  void    gestquery_BRD(  char* arg);

void versio_init(   unsigned char   *scheda,
                    unsigned short  nSW,
                    struct versio   *arVrs[])
{
    unsigned short i;

    schName = scheda;

    if (nSW > MAX_SW_VERS)
    {
        numSW = MAX_SW_VERS;
        consolle_print("Troppi moduli: gestiti i primi %d",numSW);
    }
    else
    {
        numSW = nSW;
    }

    for (i=0; i<numSW; i++)
    {
        arrayVers[i]=arVrs[i];
    }

    consolle_addcmd("versio",   "", "versioni SW applicativo, librerie e firmware", cmdVersio);

    /* All'inizializzazione visualizza la versione dell'applicativo */
    consolle_comando("versio");
    query_addcmd("VER", gestquery_VER);
    query_addcmd("PING",gestquery_PING);
    query_addcmd("BRD", gestquery_BRD);

    id = 0;
}

/************************************************/
/* stampa delle versioni applicativo e librerie */
/************************************************/
static void cmdVersio( void )
{
    unsigned short      i;
    const unsigned char *astk="*******************************************************************";
    const unsigned char *trtt="**  -----------------------------------------------------------  **";

    consolle_print("\n%s\n",astk);
    consolle_print("**                          Scheda  %-6s                       **\n",schName);
    consolle_print("%s\n",astk);
    consolle_print("**  Name              Vers      Date         Time      Status    **\n");
    consolle_print("%s\n",trtt);

                /* "*******************************************************************"  */
                /* "**  Name              Vers      Date         Time      Status    **"  */
                /* "**  -----------------------------------------------------------  **"  */
                /* "**  1234567890123456  12345678  12345678901  12345678  12345678  **"  */

    for (i=0; i<numSW; i++)
    {
        consolle_print("**  %-16s", arrayVers[i]->nome);
        consolle_print("  %-8s",    arrayVers[i]->vers);
        consolle_print("  %11s",    arrayVers[i]->date);
        consolle_print("  %8s",     arrayVers[i]->time);
        consolle_print("  %8s",     arrayVers[i]->stat);
        consolle_print("  **\n");
    }

    consolle_print("%s\n",trtt);
    consolle_print("**                          ANSALDOBREDA                         **\n");
    consolle_print("%s\n",astk);
}

/************************************************************************/
/* Gestione del comando: VER                                            */
/* Richiesta versione applicativo                                       */
/* Richiesta:  "VER i"                                                  */
/* Risposta:   "VER i OK sss" (dove sss e' la stringa di versione)      */
/************************************************************************/
static void gestquery_VER(char* arg)
{
    char            tempstr[100];
    unsigned char   strindex;

    /* determina l'indice della stringa di versione richiesta */
    strindex = atoi(arg);

    if (strindex < numSW)
    {
        if (strindex > 0)
        {
            sprintf(tempstr,"VER %d OK %s\t%s\t%s\t%s",strindex,
                            arrayVers[strindex-1]->nome,
                            arrayVers[strindex-1]->vers,
                            arrayVers[strindex-1]->date,
                            arrayVers[strindex-1]->time);
        }
        else
        {
            sprintf(tempstr,"VER 0 OK %s",schName);
        }
    }
    else
    {
        /* Segnala stringa non disponibile */
        sprintf(tempstr,"VER %d ??",strindex);
    }

    query_AppendReply(tempstr);
}

/************************************************************************/
/* Gestione del comando: PING                                           */
/* Richiesta di segnale di vita ed identificativo                       */
/* Richiesta:  "PING id" (dove id e' la proposta di identificativo)     */
/* Risposta:   "PONG id" (dove id e' l'identificativo accettato)        */
/************************************************************************/
static void gestquery_PING(char* arg)
{
    char    tempstr[40];

    /* Se l'id non e' stato ancora assegnato gli assegna quello proposto da ATRterm */
    if (!id)
    {
        id = atoi(arg);
    }

    sprintf(tempstr,"PO %u",id);

    query_AppendReply(tempstr);
}

/************************************************************************/
/* Gestione del comando: BRD                                            */
/* Richiesta nome della scheda                                          */
/* Richiesta:  "BRD"                                                    */
/* Risposta:   "BRD boardname" (dove boardname e' il nome della scheda) */
/************************************************************************/
static void gestquery_BRD(char* arg)
{
    char tempstr[40];

    arg = arg;
    sprintf(tempstr,"BRD %s",schName);
    query_AppendReply(tempstr);
}

/* ****************************************************************************
    $Log: versio.c,v $
    Revision 1.8  2004/10/29 08:35:16  mungiguerrav
    2004/ott/29,ven 10:25          Napoli       Mungi
    - Si incremento la spazio riservato alla stringa di Status da 6 a 8,
      per poter accogliere anche il nome del rilascio

    Revision 1.7  2004/10/25 13:13:00  mungiguerrav
    2004/ott/25,lun 15:05          Napoli       Mungi
    - Si riporta sul tronco principale la modifica effettuata nel branch didrv_nappoc
    - Lo spazio riservato allo stato era salito da 4 a 6

    Revision 1.6.2.1  2004/09/28 12:09:59  nappoc
    2004/set/20,lun 14:00    Napoli    Mungi\Formato
    - Si migliora la stampa delle stringe di versione nel comando versio

    Revision 1.6  2004/08/31 14:25:16  mungiguerrav
    2004/ago/31,mar 16:25        Napoli          Mungi\Busco
    - Si effettua Merge da branch makefile

    Revision 1.5.2.1  2004/07/22 16:28:31  buscob
    2004/07/22,gio 16:30     Napoli      Busco/Mungi
    - Si agg. stampa dello stato dei moduli
    - Si incorpora .MOD


    --------    PRIMA DI CVS    --------
01.06   2003.nov.30,dom             Madrid                  Mungi
        - Si spera di correggere per l'ultima volta
                numero di versione e file .mod
          sbagliati nuovamente nell'inserire (spero non tramite CVS !!!)
          la versione precedente
        - Si applicano le regole di parentesi e nomi dal manuale del SW,
          rinominando   gest_versio()       in      cmdVersio()
        - Si modifica la stringa alla base della finestra delle versioni
          da:   "**              Ansaldo Trasporti  s.p.a.              **\n"
          a:    "**                     ANSALDOBREDA                    **\n"
            -------------------   CHE TRISTEZZA !!!!   -------------------
01.05   2003.nov.19,mer             Napoli                  Busco\Nappo
        - Si agg. comando query BRD per interrogazione da ATRterm 5
          (fornisce il nome della scheda)
01.04   2003.lug.18,ven             Napoli                  Mungi
        - Versione fittizia creata per allineare date e modifiche:
          la versione 01.03 era infatti datata 2003.lug.02,mer invece
          di 2003.gen.20,lun e non era neppure compilata
          SPERO CHE QUESTA STORIA SULLA GESTIONE COMPATIBILE TRA
          CVS E FOTO SI CONCLUDA QUANTO PRIMA !!!!!!    Mungi
1.03    2003.gen.20,lun         Napoli                  Busco
        - Si agg. comando query VER per interrogazione da ATRterm 5
        - Si agg. comando query PING
        - Si agg. parametro idp alla funzione versio_init
1.02    2000.ott.15,dom         Copenaghen              Mungi
        - Si protegge dal caso di un numero di versioni maggiore
          di MAX_SW_VERS, con stampa di avviso
1.01    Gio.24.feb.2000         Napoli                  Mungi
        - Si scrivono tutte le informazioni di un Exec o di una
          Library su una sola riga
1.00    Lun.21.feb.2000         Napoli                  Mungi
        - Creazione a partire da pezzi di libda6.c-03.feb.2000
*/

