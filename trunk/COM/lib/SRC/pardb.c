/****************************************************************************************/
/* 2004/nov/12,ven 17:05                                         \SWISV\LIB\SRC\pardb.c */
/*                                                                                      */
/* Module: PARDB                                                                        */
/* Description: Gestione dei parametri di applicativo                                   */
/*                                                                  IMPLEMENTATION FILE */
/****************************************************************************************/
/*
    $Date: 2004/11/12 16:41:43 $          $Revision: 1.3 $
    $Author: mungiguerrav $
*/


/****************************************************************************************/
/* Include files                                                                        */
/****************************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "stream.h"

#include "pardb.h"
#include "consolle.h"
#include "strparse.h"


/****************************************************************************************/
/* Symbol definitions                                                                   */
/****************************************************************************************/
#define MAXWRITERETRY       3   /* Num. max. di riscritture di un record nello stream */

                                /* Costanti di selezione comando di shell */
#define CMDLIST             1
#define CMDSET              2
#define CMDWRITE            3
#define CMDDEFAULT          4
#define CMDREAD             5


/****************************************************************************************/
/* Extern variables references                                                          */
/****************************************************************************************/



/****************************************************************************************/
/* Static variables type definitions                                                    */
/****************************************************************************************/
struct ParEntry                 /* Informazioni relative ad un parametro */
{
    char*   name;               /* Nome identificativo del parametro */
    void*   pVar;               /* Puntatore alla variabile di applicativo */
    char    size;               /* Dimensione del Parametro */
    pardb_tPfValidate validate; /* Puntatore alla funzione di validazione */
    char    ParType;            /* Codice identificativo del tipo del Parametro */
    unsigned char   ParGroup;   /* Codice identificativo del gruppo del paramentro */
    unsigned short  flags;      /* Flags di stato vari   */
};

#define FLAGMODIFIED    0x0001  /* Il par. e' stato modificato rispetto al DB */
#define FLAGSCANNED     0x0002  /* Il par. e' stato elaborato (list etc.) */

struct ParRecord                /* Struttura record nello stream */
{
    char    size;                       /* Dimensione parametro */
    char    ParType;                    /* Tipo parametro */
    char    name[PARDB_NAMEMAXLEN];     /* Nome del parametro */
    char    value[PARDB_VALUEMAXLEN];   /* Valore del parametro */
};

/****************************************************************************************/
/* Static variables                                                                     */
/****************************************************************************************/
struct                                  /* Raggruppa tutte le variabili di stato del modulo PARDB. */
{
    struct ParEntry *StartAddress;      /* E' il puntatore al database dei parametri.  */
    unsigned short MaxSize;             /* Dim. massima del database dei parametri     */
    unsigned short NItems;              /* Numero di parametri presenti nel database   */
    hStream stream;                     /* Handler allo stream da utilizzare           */
    short error;                        /* Stato di errore del modulo                  */
    pardb_tPfClassDescr ClassDescr;     /* Funz. per la decodifica delle classi        */
                                        /* modificati rispetto a quelli memorizzati    */
} DbStatus;


/****************************************************************************************/
/* Local functions prototypes                                                           */
/****************************************************************************************/
static struct ParEntry* SearchParamInDb(char* name);
static unsigned short   ReadParFromStream(struct ParEntry* pEntry, struct ParRecord* rec);
static void             gest_par(short argc, char* argv[]);
static void             parlist(char *Name);
static void             string2bin(struct ParEntry* par, char* str, char* BinBuffer);
static unsigned short   CalcChecksum(void *buf, unsigned short size);
static unsigned short   CalcGroupChecksum(unsigned char GroupCode);
static unsigned char    GetNextGroupCode(unsigned char Code2Search);



/****************************************************************************************/
/* Extern functions implementation                                                      */
/****************************************************************************************/

/****************************************************************************************/
/* Nome: dbpar_init                                                                     */
/*                                                                                      */
/* Descrizione:                                                                         */
/*      Inizializza il modulo DBPAR portando tutte le variabili statiche nel loro stato */
/*      iniziale                                                                        */
/*                                                                                      */
/* Variabili di ingresso:                                                               */
/*      hstream stream  -   Handler allo stream da utilizzare per la memorizzazione dei */
/*                          parametri.                                                  */
/*      char* BufDb     -   Puntatore al buffer da utilizzare per contenere il database */
/*                          dei parametri.                                              */
/*      unsigned short BufSize                                                          */
/*                      -   E' la dimensione del buffer espressa in byte.               */
/*      dbpar_PfClassDescr ClassDescr                                                   */
/*                      -   Puntatore alla funzione per la decodifica delle classi      */
/*                                                                                      */
/* Variabili di uscita:                                                                 */
/*      Nessuno                                                                         */
/*                                                                                      */
/* Ritorno funzione:                                                                    */
/*      0 - Errore di inizializzazione                                                  */
/*      1 - OK                                                                          */
/****************************************************************************************/
short pardb_init(hStream stream, char* BufDb, unsigned short BufSize, pardb_tPfClassDescr ClassDescr)
{
    short RetVal;

    RetVal = 1;
    /* Controlla se i valori passati sono validi */
    if((stream > 0) && (BufDb != NULL) && (BufSize > 0))
    {
        RetVal = 0;
        DbStatus.stream       = stream;
        DbStatus.StartAddress = (struct ParEntry *)BufDb;
        DbStatus.MaxSize      = BufSize;
        DbStatus.NItems       = 0;
        DbStatus.ClassDescr   = ClassDescr;
        DbStatus.error        = PARDB_NOERR;
    }


    consolle_addcmd("par", "list|set|write|default|read", "Parameters Database", gest_par);

    return(RetVal);
}



/****************************************************************************************/
/* Nome: dbpar_addpar                                                                   */
/*                                                                                      */
/* Descrizione:                                                                         */
/*      Aggiunge un parametro al database dei parametri                                 */
/*                                                                                      */
/* Variabili di ingresso:                                                               */
/*      char* parname   -   Stringa di identificazione parametri.                       */
/*      void* pvar      -   Puntatore alla variabile di applicativo da gestire come par.*/
/*      char  varsize   -   Dimensione della variabile di applicativo in byte.          */
/*      pardb_pfvalidate validate   -   Puntatore alla funzione di validazione.         */
/*      pardb_partype partype       -   Tipo del parametro.                             */
/*                                                                                      */
/* Variabili di uscita:                                                                 */
/*      Nessuna                                                                         */
/*                                                                                      */
/* Ritorno funzione:                                                                    */
/*      0 : Nessun problema                                                             */
/*      1 : Si e' riscontrato un errore                                                 */
/****************************************************************************************/

short pardb_addpar(char* name, void* pVar, char size, pardb_tPfValidate validate,
                   char  ParType, unsigned char ParGroup)
{
    DbStatus.error = PARDB_NOERR;

    /* Controlla che il puntatore al nome non sia nullo */
    if(name == NULL)
    {
        DbStatus.error = PARDB_ERRBADNAME;
    }

    /* Controlla che il nome non sia una stringa vuota o troppo lunga */
    else if ((name[0] == '\0') || (strlen(name) > (PARDB_NAMEMAXLEN-1)))
    {
        DbStatus.error = PARDB_ERRBADNAME;
    }

    /* Controlla che la dimensione sia corretta */
    else if ((size <= 0) || (size > PARDB_VALUEMAXLEN))
    {
        DbStatus.error = PARDB_ERRBADSIZE;
    }

    /* Controlla che il nome non contenga spazi, "#" e "@" */
    else if (strchr(name,' ') || strchr(name,'#') || strchr(name,'@'))
    {
        DbStatus.error = PARDB_ERRBADNAME;
    }

    /* Controlla che il tipo di parametro sia tra quelli consentiti */
    else if (( ParType != PARDB_TYPECHAR)  && ( ParType != PARDB_TYPEUCHAR) &&
             ( ParType != PARDB_TYPEXCHAR) && ( ParType != PARDB_TYPESHORT) &&
             ( ParType != PARDB_TYPEUSHORT)&& ( ParType != PARDB_TYPEXSHORT)&&
             ( ParType != PARDB_TYPELONG)  && ( ParType != PARDB_TYPEULONG) &&
             ( ParType != PARDB_TYPEXLONG) && ( ParType != PARDB_TYPEFLOAT) &&
             ( ParType != PARDB_TYPESTRING))
    {
        DbStatus.error = PARDB_ERRBADTYPE;
    }

    /* Controlla che ci sia spazio nel database */
    else if (DbStatus.MaxSize < ( (DbStatus.NItems+1) * sizeof(struct ParEntry)))
    {
        DbStatus.error = PARDB_ERROUTOFMEM;
    }

    /* Controlla che il nome non sia gia' presente nel database */
    else if (SearchParamInDb(name) != NULL)
    {
        DbStatus.error = PARDB_ERRDUPNAME;
    }

    /* Controlla che il codice di gruppo non sia nullo */
    else if ((ParGroup == 0) || (ParGroup == 0xFF))
    {
        DbStatus.error = PARDB_ERRBADGROUP;
    }

    /* Se i controlli sono andati a buon fine aggiunge un nuovo parametro al database */
    if (!DbStatus.error)
    {
        (DbStatus.StartAddress[DbStatus.NItems]).name     = name;
        (DbStatus.StartAddress[DbStatus.NItems]).pVar     = pVar;
        (DbStatus.StartAddress[DbStatus.NItems]).size     = size;
        (DbStatus.StartAddress[DbStatus.NItems]).validate = validate;
        (DbStatus.StartAddress[DbStatus.NItems]).ParType  = ParType;
        (DbStatus.StartAddress[DbStatus.NItems]).ParGroup = ParGroup;
        (DbStatus.StartAddress[DbStatus.NItems]).flags = 0;
        DbStatus.NItems++;

    }

    return(DbStatus.error);
}


short pardb_write(void)
{
    short            i;
    struct ParRecord rec;

    DbStatus.error = PARDB_NOERR;

    /* Azzera lo stream */
    if(stream_erase(DbStatus.stream))
    {
        DbStatus.error = PARDB_ERRSTREAMERASE;
    }
    else
    {
        for(i=0; i<DbStatus.NItems; i++)
        {

            /* forma il record da memorizzare nello stream */
            rec.size    = (DbStatus.StartAddress[i]).size;
            rec.ParType = (DbStatus.StartAddress[i]).ParType;
            memset(rec.name, 0,PARDB_NAMEMAXLEN);
            memset(rec.value,0,PARDB_VALUEMAXLEN);

            memccpy(rec.name,(DbStatus.StartAddress[i]).name, 0, PARDB_NAMEMAXLEN);
            memcpy(rec.value,(DbStatus.StartAddress[i]).pVar, (DbStatus.StartAddress[i]).size);

            /* Azzera il flag di modificato per indicare che il valore e' allineato */
            /* con quello memorizzato in memoria permanente  */
            (DbStatus.StartAddress[i]).flags &= ~FLAGMODIFIED;

            /* aggiunge il record allo stream */
            if(stream_append(DbStatus.stream,&rec,sizeof(rec)))
            {
                DbStatus.error = PARDB_ERRSTREAMAPPEND;
            }
        }


        /**************************************************************/
        /* inserire qui la gestione della verifica dei record scritti */
        /**************************************************************/




    }

    /* esce segnalando se si e' verificato un errore */
    return(DbStatus.error != PARDB_NOERR);
}

short pardb_read(char *ParName)
{
    short  i;
    struct ParRecord rec;

    DbStatus.error = PARDB_NOERR;

    if(ParName == NULL)
    {
        /* deve leggere TUTTI i parametri dichiarati */
        i=0;
        while((i<DbStatus.NItems) && (DbStatus.error == PARDB_NOERR))
        {
            short temp;

            temp = ReadParFromStream(&(DbStatus.StartAddress[i]), &rec);

            if(temp)
            {
                /* si e' verificato un errore */
                DbStatus.error = temp;
            }
            else
            {
                /* la lettura e' andata a buon fine scrive nella var di applicativo */
                memcpy((DbStatus.StartAddress[i]).pVar,&(rec.value[0]),(DbStatus.StartAddress[i]).size);

                /* Azzera il flag di modificato */
                (DbStatus.StartAddress[i]).flags &= ~FLAGMODIFIED;

            }
            i++;
        }
    }
    else
    {
        struct ParEntry* pEntry;

        /* deve leggere solo il parametro specificato */
        pEntry=SearchParamInDb(ParName);
        if(pEntry != NULL)
        {
            short temp;

            temp = ReadParFromStream(pEntry, &rec);
            if(temp)
            {
                /* si e' verificato un errore */
                DbStatus.error = temp;
            }
            else
            {
                /* la lettura e' andata a buon fine scrive nella var di applicativo */
                memcpy(pEntry->pVar,rec.value,pEntry->size);
            }
        }
        else
        {
            /* se non lo trova segnala errore */
            DbStatus.error = PARDB_ERRUNKNOWNNAME;
        }
    }
    return(DbStatus.error != PARDB_NOERR);
}

unsigned short pardb_error(void)
{
    unsigned short RetVal;

    RetVal         = DbStatus.error;
    DbStatus.error = PARDB_NOERR;
    return(RetVal);
}


short pardb_SetParam(char* ParName, char* ParValue)
{
    ParName=ParName;
    ParValue=ParValue;
    return(0);
}

void pardb_ResetToDefault(void)
{
    short i;

    /* Azzera lo stream per la memorizzazione dei parametri */
    stream_erase(DbStatus.stream);

    /* Il valore attuale dei parametri non e' piu' uguale a quello che si otterra' */
    /* dopo il reset.                                                              */
    for(i=0; i<DbStatus.NItems; i++)
    {
        (DbStatus.StartAddress[i]).flags |= FLAGMODIFIED;
    }
}

unsigned short pardb_NParam(void)
{
    return(DbStatus.NItems);
}

/****************************************************************************************/
/* Local functions implementation                                                       */
/****************************************************************************************/
static struct ParEntry* SearchParamInDb(char* name)
{
    struct ParEntry* RetVal;
    short i;
    short found;

    RetVal = NULL;

    /* Ricerca il nome del parametro nel database */
    i = 0;
    found = 0;
    while((i<DbStatus.NItems) && !found)
    {
        if(strcmp((DbStatus.StartAddress[i]).name, name)==0)
        {
            found = 1;
        }
        i++;
    }
    if(found)
    {
        RetVal = &(DbStatus.StartAddress[i-1]);
    }
    return(RetVal);
}

/* Calcola il checksum di un buffer di memoria */
static unsigned short CalcChecksum(void *buf, unsigned short size)
{
    unsigned short RetVal;
    unsigned short i;

    RetVal = 0;
    for(i=0; i<size; i++)
    {
        RetVal += ((unsigned char*)buf)[i];
    }
    return(RetVal);
}

/* Calcola il checksum di tutti i parametri di un gruppo */
static unsigned short CalcGroupChecksum(unsigned char GroupCode)
{
    unsigned short RetVal;
    unsigned short i;

    RetVal = 0;
    for(i=0; i<DbStatus.NItems; i++)
    {
        if((DbStatus.StartAddress[i]).ParGroup == GroupCode)
        {
            RetVal += CalcChecksum(DbStatus.StartAddress[i].pVar,DbStatus.StartAddress[i].size);
        }

    }
    return(RetVal);
}

static unsigned short ReadParFromStream(struct ParEntry* pEntry, struct ParRecord* rec)
{
    struct ParRecord tempRec;
    short            nbyte;
    short            found;
    short            RetVal;

    RetVal = 0;

    /* Inizia a leggere lo stream dall'inizio */
    stream_seek(DbStatus.stream,1);
    found = 0;
    do
    {
        nbyte = stream_read(DbStatus.stream, &tempRec, sizeof(tempRec));
        if(nbyte>0)
        {
            found = (strcmp(pEntry->name,tempRec.name) == 0);
        }

    } while((nbyte>0) && (!found));

    if(!found)
    {
        /* Se il parametro non e' stato trovato nello stream lo segnala */
        RetVal = PARDB_ERRNOTFOUND;
    }
    else
    {
        /* controlla la corrispondenza del tipo e dimensione */
        if((tempRec.ParType != pEntry->ParType) ||
           (tempRec.size    != pEntry->size))
        {
            /* se non c'e' corrispondenza lo segnala */
            RetVal = PARDB_ERRDATAMISMATCH;
        }
        else
        {
            /* Tutti i controlli sono andati a buon fine */
            /* copia i dati nel record da passare al chiamante */
            memcpy(rec,&tempRec,sizeof(tempRec));
        }
    }

    return(RetVal);

}


/* Elenca il valore attuale dei parametri */
static void parlist(char *Name)
{
    short i;
    unsigned short cs;
    unsigned short GlobCs;

    if(!Name)
    {
        unsigned char ParGroup;

        /* Deve elencare tutti i parametri */
        ParGroup = 0;
        consolle_print("*** Parameter database ***\n");

        /* Azzera il checksum totale */
        GlobCs=0;

        /* Stampa i parametri per gruppi */
        while((ParGroup=GetNextGroupCode(ParGroup)) != 0xFF)
        {
            cs = CalcGroupChecksum(ParGroup);
            /* Calcola il checksum totale */
            GlobCs += cs;

            consolle_print("\n");
            /* Se l'applicativo ha fornito la funzione di descrizione gruppo la stampa */
            if(DbStatus.ClassDescr != NULL)
            {
                consolle_print("Group: %s - ",DbStatus.ClassDescr(ParGroup));
            }
            consolle_print("CheckSum:0x%04X\n",cs);

            for(i=0; i<pardb_NParam();i++)
            {
                if((DbStatus.StartAddress[i]).ParGroup == ParGroup)
                {
                    /* Stampa il flag di modificato */
                    if((DbStatus.StartAddress[i]).flags & FLAGMODIFIED)
                    {
                        consolle_print("M ");
                    }
                    else
                    {
                        consolle_print("  ");
                    }

                    /* Stampa le informazioni generiche sul parametro */
                    consolle_print("%-10s = ", DbStatus.StartAddress[i].name);

                    /* Stampa le informazioni funzione del tipo */
                    switch(DbStatus.StartAddress[i].ParType)
                    {
                        case PARDB_TYPECHAR:
                            consolle_print("%d\n",(*(char*)DbStatus.StartAddress[i].pVar));
                            break;

                        case PARDB_TYPESHORT:
                            consolle_print("%d\n",(*(short*)DbStatus.StartAddress[i].pVar));
                            break;

                        case PARDB_TYPEUCHAR:
                            consolle_print("%u\n",(*(unsigned char*)DbStatus.StartAddress[i].pVar));
                            break;

                        case PARDB_TYPEUSHORT:
                            consolle_print("%u\n",(*(unsigned short*)DbStatus.StartAddress[i].pVar));
                            break;

                        case PARDB_TYPEXCHAR:
                            consolle_print("0x%02X\n",(*(unsigned char*)DbStatus.StartAddress[i].pVar));
                            break;

                        case PARDB_TYPEXSHORT:
                            consolle_print("0x%04X\n",(*(unsigned short*)DbStatus.StartAddress[i].pVar));
                            break;

                        case PARDB_TYPELONG:
                            consolle_print("%ld\n",(*(long*)DbStatus.StartAddress[i].pVar));
                            break;

                        case PARDB_TYPEULONG:
                            consolle_print("%lu\n",(*(unsigned long*)DbStatus.StartAddress[i].pVar));
                            break;

                        case PARDB_TYPEXLONG:
                            consolle_print("0x%08lX\n",(*(unsigned long*)DbStatus.StartAddress[i].pVar));
                            break;

                        case PARDB_TYPEFLOAT:
                            consolle_print("%f\n",(*(float*)DbStatus.StartAddress[i].pVar));
                            break;

                        case PARDB_TYPESTRING:
                            consolle_print("%s\n",(char*)DbStatus.StartAddress[i].pVar);
                            break;
                    }
                }
            }
        }
        consolle_print("\nGlobal Checksum: 0x%04X\n",GlobCs);
    }
    else
    {
        /* Deve visualizzare solo il parametro indicato da Name */
    }
}

static void string2bin(struct ParEntry* par, char* str, char* BinBuffer)
{
    switch(par->ParType)
    {
        case PARDB_TYPECHAR:
        case PARDB_TYPEUCHAR:
        case PARDB_TYPEXCHAR:
            *(char*)BinBuffer = (char)parse_number(str);

            break;

        case PARDB_TYPESHORT:
        case PARDB_TYPEUSHORT:
        case PARDB_TYPEXSHORT:
            *(short*)BinBuffer = (short)parse_number(str);
            break;

        case PARDB_TYPELONG:
        case PARDB_TYPEULONG:
        case PARDB_TYPEXLONG:
            *(long*)BinBuffer = (long)parse_number(str);
            break;

        case PARDB_TYPEFLOAT:
            *(float*)BinBuffer = (float)atof(str);
            break;

        case PARDB_TYPESTRING:
            strncpy(BinBuffer,str,par->size-1);
            break;
    }

}

static void gest_par(short argc, char* argv[])
{
    char                cmd;
    struct ParEntry*    par;
    char                parValue[PARDB_VALUEMAXLEN];

    /* Individua il comando da eseguire */
    cmd = 0;
    if(argc<2)
    {
        /* non sono presenti parametri          */
        /* assume per default il comando "list" */
        cmd = CMDLIST;
    }
    else
    {
        if(strcmp(argv[1],"list") == 0)
        {
            cmd = CMDLIST;
        }
        else if(strcmp(argv[1],"set") == 0)
        {
            cmd = CMDSET;
        }
        else if(strcmp(argv[1],"write") == 0)
        {
            cmd = CMDWRITE;
        }
        else if(strcmp(argv[1],"default") == 0)
        {
            cmd = CMDDEFAULT;
        }
        else if(strcmp(argv[1],"read") == 0)
        {
            cmd = CMDREAD;
        }
    }

    switch(cmd)
    {
        case CMDLIST:
            if(argc>=2)
            {
                /* E' stato specificato un nome di parametro */
                parlist(argv[2]);
            }
            else
            {
                /* Non si e' specificato un nome di parametro */
                /* lista tutti i parametri                    */
                parlist(NULL);
            }
        break;

        case CMDSET:
            if(argc>3)
            {
                par = SearchParamInDb(argv[2]);
                if (par)
                {
                    short ValueIsOK;

                    /* Il parametro e' stato trovato nel database */
                    ValueIsOK = 1;

                    /* Trasforma la stringa di valore posta sulla linea di comando nel suo valore */
                    /* binario */
                    memset(parValue,0,PARDB_VALUEMAXLEN);
                    string2bin(par,argv[3],parValue);

                    /* Se e' prevista la funzione di validazione controlla il valore del parametro*/
                    if(par->validate)
                    {
                        ValueIsOK = par->validate(parValue);
                    }

                    if(ValueIsOK)
                    {
                        /* Il valore assegnato al parametro è corretto lo trasferisce nella */
                        /* variabile associata al parametro                                 */
                        memcpy(par->pVar, parValue, par->size);
                        consolle_print("Parameter %s updated\n",argv[2]);

                        /* Segna il parametro come modificato */
                        par->flags |= FLAGMODIFIED;

                    }
                    else
                    {
                        /* La funzione di validazione ha scartato il valore */
                        consolle_print("Value %s not allowed for %s\n",argv[3],argv[2]);
                    }
                }
                else
                {
                    /* Il parametro NON e' stato trovato nel database */
                    consolle_print("Parameter %s not found\n",argv[2]);
                }
            }
            else
            {
                /* Non sono stati inseriti i parametri sufficienti */
                consolle_print("Syntax error: par set <ParName> <NewValue>\n");
            }

        break;

        case CMDWRITE:
            if(pardb_write())
            {
                consolle_print("Write Error: %d\n",pardb_error());
            }
            else
            {
                consolle_print("ParDB copied to NV memory\n");
            }

        break;

        case CMDDEFAULT:
            pardb_ResetToDefault();
            consolle_print("NV memory ParDB erased. RESET to restore default values.\n");
        break;

        case CMDREAD:
            if(pardb_read(NULL))
            {
                consolle_print("Read Error: %d\n",pardb_error());
            }
            else
            {
                consolle_print("ParDB copied from NV memory\n");
            }
        break;

        default:
            /* E' stato immesso un comando sconosciuto */
            consolle_print("--PAR: Wrong command\n");
        break;
    }
//    return(0);
}

/* Effettua una scansione dei parametri e restituisce il codice di gruppo immediatamente */
/* successivo a quello fornito.                                                          */
/* Restituisce 0xFF se non viene trovato un codice valido                                */
static unsigned char GetNextGroupCode(unsigned char Code2Search)
{
    short i;
    unsigned char CodeFound;

    CodeFound = 0xFF;
    for(i=0;i<DbStatus.NItems;i++)
    {
        if((DbStatus.StartAddress[i].ParGroup > Code2Search) && (DbStatus.StartAddress[i].ParGroup < CodeFound))
        {
            CodeFound = DbStatus.StartAddress[i].ParGroup;
        }
    }
    return(CodeFound);
}


/* ****************************************************************************
    $Log: pardb.c,v $
    Revision 1.3  2004/11/12 16:41:43  mungiguerrav
    2004/nov/12,ven 17:05          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.2        2004/06/15 11:43:36     buscob
01.04   2003/nov/21,ven         Napoli                  Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso.

    Revision 1.1        2002/05/17 09:17:08     accardol
    - Inserimento in CVS della versione in lib07m
    Revision 1.1.1.1    2002/05/17 09:17:08     accardol
    - Inserimento in CVS in branch STP della versione in lib07m
        La data è arretrata con un artificio
01.03   2002/mag/17,ven             Napoli              Busco
        - Inserimento in libreria UFAP
        - Si corregge tipo da char a unsigned char per param. ParGroup di _addpar()

 ------------------------   Prima di CVS    ------------------------
01.02   2001/lug/31,mar             Napoli              Busco
        - Si elimina gestione valori di default
        - Il codice di gruppo non puo' essere nullo
        - Si elimina il comando "par checksum" perche' accorpato con "par list"
01.01   2001/lug/30,lun             Napoli              Busco
        - Si usa consolle invece di ushell per compatibilita' con PCA
        - Si implementa comando par checksum
        - Corretta gestione parametri stinga con lunghezza > MAX
        - Corretta gestione comando set senza valore parametro
        - Si elimina generazione e controllo del checksum del record perche' lo si e'
          trasferito in stream
01.00   2001/giu/04,lun             Napoli              Busco
        - Creazione
*/

