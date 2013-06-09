/********************************************************************/
/* 2004/nov/05,ven 09:45                \SWISV\LIB\SRC\vardbcmd.c   */
/********************************************************************/
/*
    $Date: 2004/11/05 09:03:36 $          $Revision: 1.4 $
    $Author: mungiguerrav $
*/


#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "consolle.h"
#include "vardbcmd.h"
#include "vardb.h"
#include "ctype.h"

/******************/
/* parametri vari */
/******************/
#define MAXNVARAUX   10
#define MAXLENVARAUX 10

extern  struct varEntry     *vardb;     /* Database delle variabili */

static  char                varauxnames[MAXNVARAUX][MAXLENVARAUX+1];
static  short               nvaraux;

/****************************************************/
/* prototipi delle funzioni di gestione dei comandi */
/****************************************************/
static void gest_var   ( short argc, char* argv[]);
static void gest_addvar( short argc, char* argv[]);

/***********************************************/
/* prototipi delle funzioni statiche di modulo */
/***********************************************/
static short    isAux(      struct varEntry* var);
static long     parsenum(   const char *s);
static void     printvar(   struct varEntry* ptentry);

void vardbcmd_init(void)
{
    nvaraux     = 0;
    consolle_addcmd("var",   "[<var>]",        "display var list",        gest_var);
    consolle_addcmd("addvar","name addr. type","add run-time vars",   gest_addvar);
}

/************************/
/* gestione comando var */
/************************/
static void gest_var(short argc, char* argv[])
{
    struct varEntry*    ptentry;

    if (argc > 1)
    {
        if ((ptentry = vardb_findvar(argv[1])) != NULL)
        {
            /* se trova la variabile nella lista */
            if (argc == 2)
            /* se c'e' solo il primo parametro stampa la variabile */
            {
                printvar(ptentry);
            }
        }
        else
            consolle_print("Var not found\n");
    }
    else
    {
        short i;

        consolle_print("%d vars in database.\n",vardb_numvar());
        for(i=0; i < vardb_numvar(); i++)
        {
            printvar(&vardb[i]);
        }
    }
}

/***************************/
/* gestione comando addvar */
/***************************/
static void gest_addvar(short argc, char* argv[])
{
    short               temp;
    struct varEntry     *tempvar;

    if (argc>2)
    {
        /* Controlla se la variabile e' gia' presente nel DB */
        tempvar = vardb_findvar(argv[1]);

        if(tempvar != NULL)
        {   /* La variabile e' gia' presente nel DB */
            if(isAux(tempvar))
            {   /* La variabile e' una run-time, la puo' sostituire */
                vardb_setvar( tempvar,
                              (char *)tempvar->nomeVar,
                              "",
                              "",
                              1000,
                              (unsigned short *)parsenum(argv[2]),
                              VARDB_TYPESHORT);

                consolle_print("Var replaced\n");
            }
            else
            {   /* La var. non e' una run-time, non la puo' sostituire */
                consolle_print("Var already defined\n");
            }
        }
        else
        {   /* La variabile non e' presente nel DB */
            if(nvaraux < MAXNVARAUX)
            {   /* C'e' posto per un'altra variabile run-time */

                /* Copia il nome della var. nell'array */
                strncpy(&(varauxnames[nvaraux][0]),argv[1],MAXLENVARAUX);

                temp = vardb_addvar(&(varauxnames[nvaraux][0]),
                                     "unit",
                                     "descr",
                                     1000,
                                     (unsigned short *)parsenum(argv[2]),
                                     VARDB_TYPESHORT);

                /* Controllare l'esito dell'aggiunta */
                if (!temp)
                {
                    /* E' riuscito ad aggiungere la variabile */
                    nvaraux++;
                    consolle_print("Var added\n");
                }
                else
                {
                    /* Non e' riuscito ad aggiungere la variabile */
                    consolle_print("Too many vars\n");
                }
            }
            else
            {   /* Non c'e' piu' posto per le var run-time */
                consolle_print("Too many run-time vars\n");
            }
        }
    }
}

static short isAux( struct varEntry* var)
{
    short i;
    short retval;

    retval = 0;
    for (i=0; i<MAXNVARAUX; i++)
    {
        if((char *)(&(varauxnames[i][0])) ==  var->nomeVar)
           retval = 1;
    }
    return(retval);
}

/******************/
/* Parse a number */
/******************/
static long parsenum(const char *s)
{
    long l = 0;

    if (s[0] == '$')                                sscanf(s+1, "%lx", &l);
    else if (s[0] == '0' && toupper(s[1]) == 'X')   sscanf(s+2, "%lx", &l);
    else                                            sscanf(s,   "%ld", &l);

    return l;
}


/**************************************/
/* Stampa le info relative ad una var */
/**************************************/
static void printvar(struct varEntry* ptentry)
{
    consolle_print("%-10s (0x%08lX) = ",
                   ptentry->nomeVar, (long)(ptentry->pvar));
    switch(ptentry->flags & VARDB_TYPEMASK)
    {
        case VARDB_TYPECHAR:
            consolle_print(FRMTVAR_CHAR,(short)*((char far *)ptentry->pvar));
        break;
        case VARDB_TYPEUSHORT:
            consolle_print(FRMTVAR_UINT,(short)*(( short far *)ptentry->pvar));
        break;
        case VARDB_TYPESHORT:
            consolle_print(FRMTVAR_INT, (short)*(( short far *)ptentry->pvar));
        break;
        case VARDB_TYPEUHEX:
            consolle_print(FRMTVAR_HEX, (short)*(( short far *)ptentry->pvar));
        break;
        default:
        break;
    }
    consolle_print("\n");
}


/* ****************************************************************************
    $Log: vardbCmd.c,v $
    Revision 1.4  2004/11/05 09:03:36  mungiguerrav
    2004/nov/05,ven 09:45          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.3        2004/08/31 14:25:15     mungiguerrav
    2004/ago/31,mar 16:25           Napoli              Mungi\Busco
    - Si effettua Merge da branch makefile

    Revision 1.2.2.1    2004/07/14 15:45:45     buscob
01.03   2003.dic.17,mer             Napoli              Nappo/Busco
        - Si modifica l'utilizzo della struttura varEntry in conformità con le
          modifiche apportate alla stessa in vardb.h

    Revision 1.2        2004/06/15 12:08:54     buscob
01.02   2003/dic/02,mar             Madrid              Mungi
        - Si introducono le regole su parentesi e nomi dal manuale SW
        - Si aggiunge il cast (char *)tempvar->nomeVar,
          nella chiamata a vardb_setvar(), in cui si usa il campo di
          struct varEntry:    const char *nomeVar
01.01   2003/nov/21,ven             Napoli              Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso

    Revision 1.1      2002/05/17 10:06:34   accardol
    - Inserimento in CVS della versione in lib07m
    Revision 1.1.1.1  2002/05/17 10:06:34   accardol
    - Inserimento in CVS in branch STP della versione in lib07m
01.00   2002/mag/17,ven             Napoli              Busco
        - Creazione a partire da querycmd.c v.01.06
*/

