/****************************************************************************************/
/* 2004/ott/25,lun 13:35                                         \SWISV\LIB\SRC\vardb.h */
/*                                                                                      */
/* Module: VARDB                                                                        */
/* Description: Database delle variabili da monitorare                                  */
/*                                                                          HEADER FILE */
/****************************************************************************************/
/*
    $Date: 2004/10/25 11:52:53 $          $Revision: 1.6 $
    $Author: mungiguerrav $
*/


#ifndef VARDB_INCLUDED
#define VARDB_INCLUDED

/* Formati per la stampa delle variabili */
#define FRMTVAR_INT    "%6d"     /* 'i' */
#define FRMTVAR_UINT   "%6u"     /* 'u' */
#define FRMTVAR_HEX    " %04Xh"  /* 'x' */
#define FRMTVAR_CHAR   "%6d"     /* 'c' */

/* Codici per l'accesso al campo flags della struct varEntry */

#define VARDB_ENABLED     0x8000
#define VARDB_VARINSCOPE  0x4000

#define VARDB_TYPEMASK    0x3800
#define VARDB_TYPEUSHORT  0x0000
#define VARDB_TYPESHORT   0x0800
#define VARDB_TYPEUHEX    0x1000
#define VARDB_TYPECHAR    0x1800

#define VARDB_DSPTYPEDEC  0x0000
#define VARDB_DSPTYPEHEX  0x0100
#define VARDB_DSPTYPELEDR 0x0200
#define VARDB_DSPTYPELEDG 0x0300
#define VARDB_DSPTYPELEDY 0x0400

#define VARDB_DSPCOL      0x0001
#define VARDB_DSPROW      0x0010

/* Codici per l'accesso al campo flags della struct varBitEntry */
#define VARDB_BITNUMBERMASK  0xF000
#define VARDB_BITINSCOPE     0x0800

#define VARDB_BITDSPTYPELEDR 0x0100
#define VARDB_BITDSPTYPELEDG 0x0200
#define VARDB_BITDSPTYPELEDY 0x0300

#define VARDB_BITDSPCOL      0x0001
#define VARDB_BITDSPROW      0x0010

/**************************************************/
/* struttura dati per la gestione delle variabili */
/**************************************************/
struct varEntry
{
    const char      *nomeVar;       /* puntatore alla stringa nome var          */
    const char      *unit;          /* unita' di misura                         */
    const char      *descr;         /* descrizione                              */
    short           scale;          /* scala espressa in 1/1000                 */
    unsigned short  flags;          /* bit 15     Enabled                       */
                                    /* bit 14     Var in scope                  */
                                    /* bit 13..11 Var type            0=Ushort, 1=Short, 2=UHex, 3=Char    */
                                    /* bit 10..8  Display type        0=Dec, 1=Hex, 2=LedR, 3=LedG, 4=LedY */
                                    /* bit 7..4   Display row                   */
                                    /* bit 3..0   Display col                   */
    void far        *pvar;          /* puntatore alla variabile                 */
    short           Sample;         /* Valore dell'ultimo Campione acquisito    */
    struct varEntry *queryvar;      /* Puntatori al DB per le var. richieste    */
};

/******************************************************/
/* struttura dati per la gestione delle variabili bit */
/******************************************************/
struct varBitEntry
{
    const char      *nomeVar;       /* puntatore alla stringa nome var                */
    const char      *descr;         /* descrizione                                    */
    unsigned short  BWIndex;        /* Indice della variabile principale (BitWord)*/
    unsigned short  flags;          /* word per la gestione delle info sulle var. bit */
};

short               vardb_init(         char            *buff,
                                        unsigned short  size,
                                        unsigned short  dt);

short               vardb_sample(       void);

short               vardb_addvar(       const char      *nomeVar,
                                        const char      *unit,
                                        const char      *descr,
                                        short           scale,
                                        void far        *pvar,
                                        unsigned short  flags);

short               vardb_addvarbit(    unsigned short  BWindex,
                                        char            NBit,
                                        const char      *nomeVar,
                                        const char      *descr,
                                        unsigned short  flags);

struct varEntry*    vardb_findvar(      char            *nomeVar);

short               vardb_VarIndex(     char            *nomeVar);

unsigned short      vardb_numvar(       void);

void                vardb_setvar(       struct varEntry *entry,
                                        char            *nomeVar,
                                        char            *unit,
                                        char            *descr,
                                        short           scale,
                                        void far        *pvar,
                                        unsigned short  flags);

short               vardb_replystr(     void);
void                vardb_ResetTimeout( void);

#endif


/* ****************************************************************************
    $Log: vardb.h,v $
    Revision 1.6  2004/10/25 11:52:53  mungiguerrav
    2004/ott/25,lun 13:35          Napoli       Mungi
    - Si aggiungono keyword CVS e storia passata


    Revision 1.5      2004/08/31 14:25:15     mungiguerrav
    2004/ago/31,mar 16:25        Napoli          Mungi\Busco
    - Si effettua Merge da branch makefile

 ---------------    Fase transitoria tra *.mod e CVS    -------------
    Revision 1.4.2.1  2004/07/14 15:45:10     buscob
02.00   2004.apr.02,ven             Napoli                  Nappo/Busco
        - Si cambia la gestione delle informazioni all'interno della struct varEntry:
            - Si abolisce il campo char tipo, trasferito in un sottocampo di flags
            - I singoli bit del campo flags assumono il singificato:
                15          variabile abilitata
                14          variabile sullo scope
                13..11      tipo di variabile
                                000 = Ushort
                                001 = Short
                                010 = UHex
                                011 = Char
                                1xx = (future espansioni)
                10..8       tipo di visualizzazione  su Atrterm5
                                000 = Dec
                                001 = Hex
                                010 = LedRed
                                011 = LedGreen
                                100 = LedYellow
                                1xx = (future espansioni)
                7..4       riga     sul pannello delle variabili
                3..0       colonna  sul pannello delle variabili
        - Si cambia la gestione delle informazioni all'interno della struct varBitEntry:
            - Si sostituisce il campo char Nbit con il campo Ushort flags
            - I singoli bit del campo flags assumono il singificato:
                15..12	    numero del bit
                11        	variabile sullo scope
                9..8   	    colore del led
                                00  = (future espansioni)
                                01  = LedRed
                                10  = LedGreen
                                11  = LedYellow
                7..4       	riga    sul pannello delle variabili
                3..0       	colonna sul pannello delle variabili
        - Si aggiungono i codici per l'accesso al campo flags delle struct varEntry e varBitEntry

        - I prototipi   vardb_addvar e vardb_setvar     sostituiscono:
            i parametri     tipo, state, defAtten
            con             flags
        - Il prototipo  vardb_addvarbit     aggiunge il parametro   flags
        - Eliminata la funzione query_addvar presente finora per compatibilita'
          con le vecchie applicazioni

    Revision 1.4      2004/06/15 12:07:45     buscob
01.04   2003.nov.29,sab             Madrid                  Mungi
        - Si introducono le regole su parentesi e nomi dal manuale SW
01.03   2003.nov.21,ven             Napoli                  Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso

    Revision 1.3      2003/03/03 16:14:49     buscob
01.02   2003.gen.23,gio             Napoli              Busco
        - Si agg. struttura dati    struct varbit_entry     per la gestione delle variabili bit
        - Si agg. prototipo
                int vardb_addvarbit(int BWindex, char NBit, const char *nome_var, const char *descr);

    Revision 1.2      2003/03/03 14:40:13     buscob
    (Commit per sperimentare le keyword CVS)
    Revision 1.1      2002/05/17 09:54:52   accardol
    - Inserimento in CVS
    Revision 1.1.1.1  2002/05/17 09:54:52   accardol
    - Inserimento in CVS in branch STP
01.01   2002.mag.17,ven             Napoli              Busco
        - Si agg. prototipo funzione vardb_VarIndex(..)

 ----------------    Prima di CVS    -----------------------------
01.00   2001.lug.24,lun             Napoli              Busco
        - Creazione
*/

