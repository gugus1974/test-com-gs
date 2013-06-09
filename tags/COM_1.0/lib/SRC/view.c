/*****************************************************************/
/* 2004/nov/04,gio 13:15                \SWISV\LIB\SRC\view.c    */
/*****************************************************************/
/*
    $Date: 2004/11/04 12:40:43 $          $Revision: 1.4 $
    $Author: mungiguerrav $
*/


#include "string.h"     /* gestione stringhe */
#include "ctype.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdarg.h"
#include "consolle.h"
#include "vardb.h"

#define MAX_VARVIEW    12           /* numero massimo di variabili per il view  */

extern  struct varEntry     vardb[];                /* Database delle variabili */

/**********************************************/
/* prototipi delle funzioni interne al modulo */
/**********************************************/
static  void    cmdView(        short argc, char* argv[]);

static  void    printView(      void);
static  void    printView0(     void);
static  void    printNomiVar(   void);

/*******************************/
/* variabili per gestione view */
/*******************************/
static  short           numVarView;             /* num. var. di cui si vuole il view    */
static  struct varEntry *varView[MAX_VARVIEW];  /* punt.var. di cui si vuole il view    */

static  unsigned short  viewEnable;
static  unsigned short  viewCnt;    /* contatore per rallentamento view         */
static  unsigned short  viewStep;   /* numero di chiamate di consolle_view      */
                                    /* tra due visualizzazioni successive       */
static  unsigned short  viewPrn;    /* flag di stampa view in corso             */
static  unsigned short  viewStato;

/********************/
/* inizializzazione */
/********************/
void view_init(void)
{
    unsigned short i;

    viewCnt     = 0;
    viewStep    = 100;
    viewPrn     = 0;
    viewEnable  = 0;
    viewStato   = 0;

    for(i=0; i<MAX_VARVIEW; i++)
    {
        varView[i]  = (struct varEntry*) 0;
    }

    consolle_addcmd("view",  "[<step>|<var1>..]",      "vis. continua variab.", cmdView);
}

void view_count(void)
{
    if (++viewCnt == viewStep)
    {
        viewCnt     = 0;
        viewPrn     = 1;
    }
}

void view_print(void)
{
    switch(viewStato)
    {
        case 0:
            if (viewEnable)
            {
                viewStato   = 1;
            }
        break;

        case 1:
            printView0();
            viewStato   = 2;
        break;

        case 2:
            printView();
            if (!viewEnable)
            {
                viewStato   = 3;
            }
        break;

        case 3:
            printView0();
            consolle_print("\n");

            printNomiVar();

            consolle_print("\nstep %u\n",viewStep);
            viewStato   = 0;
        break;

        default:
            viewStato   = 0;
        break;
    }
}

/* stampa la riga di view */
static void printView(void)
{
    if (viewPrn && viewEnable)
    {
        short i;

        consolle_print("\n");
        for(i = 0; i<numVarView; i++)
        {
            struct varEntry*    ptentry;

            ptentry     = varView[i];
            if (ptentry==NULL)
            {
                consolle_print("     0");
            }
            else
            {
                switch(ptentry->flags & VARDB_TYPEMASK)
                {
                    case VARDB_TYPECHAR:   consolle_print(FRMTVAR_CHAR,(short)*( (char  far *)ptentry->pvar));  break;
                    case VARDB_TYPEUSHORT: consolle_print(FRMTVAR_UINT,(short)*( (short far *)ptentry->pvar));  break;
                    case VARDB_TYPESHORT:  consolle_print(FRMTVAR_INT, (short)*( (short far *)ptentry->pvar));  break;
                    case VARDB_TYPEUHEX:   consolle_print(FRMTVAR_HEX, (short)*( (short far *)ptentry->pvar));  break;
                    default:
                    break;
                }
            }
        }

        viewPrn     = 0;
    }
}

/* stampa la riga di view con 0*/
static void printView0(void)
{
    short   i;

    consolle_print("\n");
    consolle_print(FRMTVAR_UINT, (short)*( (short far *)(varView[0]->pvar) ));

    for(i=1; i<numVarView; i++)
    {
        consolle_print(FRMTVAR_UINT, 0);
    }
}

/* stampa la riga dei nomi var */
static void printNomiVar(void)
{
    short   i;

    for(i=0; i<numVarView; i++)
    {
        if (varView[i]==NULL)
        {
            consolle_print("(Null)");
        }
        else
        {
            consolle_print("%s  ",varView[i]->nomeVar);
        }
    }
}

/*************************/
/* gestione comando view */
/*************************/
static void cmdView( short argc, char* argv[] )
{
    short   varNonTrovata;

    varNonTrovata   = 0;

    if ( argc > 1 )
    {
        if (isdigit(argv[1][0]))
        {
           viewStep     = atol(argv[1]);
           viewCnt      = 0;
        }
        else
        {
            short               i;
            struct varEntry*    ptentry;

            numVarView      = 0;

            for(i=1; i<argc && numVarView<MAX_VARVIEW; i++)
            {
                if ((ptentry=vardb_findvar(argv[i])) != NULL)
                {
                    varView[numVarView++]   = ptentry;
                }
                else if (argv[i][0] == '.')         /* attiva la var. precedente    */
                {
                    numVarView++;
                }
                else if (argv[i][0] == '*')         /* attiva tutte le var. preced. */
                {
                    numVarView              = MAX_VARVIEW;
                }
                else
                {
                    varNonTrovata           = 1;
                    varView[numVarView++]   = NULL;
                }
            }

            if (varNonTrovata)
            {
                consolle_print("ERRORE: variabile non trovata");

                if(numVarView > 0)
                {
                    consolle_print("\nVariabili attuali:\n ");
                    printNomiVar();
                }

                consolle_print("\nstep %u\n",viewStep);
                consolle_comando(". ");
            }
        }
    }
    else if ( numVarView > 0 )
    {
        viewEnable  = !viewEnable;
    }
}


/* ****************************************************************************
    $Log: view.c,v $
    Revision 1.4  2004/11/04 12:40:43  mungiguerrav
    2004/nov/04,gio 13:15          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


    Revision 1.3  2004/08/31 14:25:16  mungiguerrav
    2004/ago/31,mar 16:25        Napoli          Mungi\Busco
    - Si effettua Merge da branch makefile

 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.2.2.1  2004/07/14 15:46:20  buscob
01.07   2003/dic/17,mer         Napoli                  Nappo/Busco
        - Si modifica l'utilizzo della struttura varEntry in conformita' con le
          modifiche apportate alla stessa in vardb.h

    Revision 1.2      2004/06/15 12:11:38     buscob
01.06   2003/nov/29,sab             Madrid                  Mungi
        - Si applicano le regole su parentesi e nomi dal manuale SW
01.05   2003/nov/21,ven             Napoli                  Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso.

    Revision 1.1      2002/05/17 10:08:56   accardol
    - Inserimento in CVS della versione in lib07m
    Revision 1.1.1.1  2002/05/17 10:08:56   accardol
    - Inserimento in CVS in branch STP della versione in lib07m
01.04   2002/mag/17,ven         Napoli                  Busco
        - Fatta il 2000.ott.30,lun
        - Si sost. include "query.h" con "vardb.h"
        - Si sost. query_findvar() con vardb_findvar()

 ------------------------   Prima di CVS    ------------------------
01.03   2000/feb/07,lun         Napoli                  Mungi
        - Se var non trovata si pone var_view[++]=NULL e non &vardb[0]
01.02   2000/feb/05,sab         Napoli                  Mungi
        - Si elim. gest. con numVarViewOld e si agg. test su
          var_view[i]==NULL, che stampano 0 o (Null)
        - Si agg. la static void print_nomivar(void)
01.01   2000/feb/02,mer         Napoli                  Mungi
        - Si elim. blocco SW su view *, attivando le colonne fino a
          numVarViewOld e non fino a MAX
01.00   2000/feb/01,mar         Napoli                  Busco/Mungi
        - Creazione a partire da consolle.c v.5.01 del 06.dic.1999
          estraendo i pezzi relativi a view
*/
