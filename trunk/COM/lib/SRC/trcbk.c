/****************************************************************/
/* 2004/nov/05,ven 14:00                \SWISV\LIB\SRC\trcbk.c  */
/****************************************************************/
/*
    $Date: 2004/11/05 13:24:17 $          $Revision: 1.4 $
    $Author: mungiguerrav $
*/


#include "string.h"     /* gestione stringhe */
#include "ctype.h"
#include "stdlib.h"
#include "stdio.h"
#include "consolle.h"
#include "vardb.h"
#include "trcbk.h"

/******************/
/* parametri vari */
/******************/
#define TRACE_NCAMP   100   /* numero di campioni per il trace          */
#define MAX_VARTRACE    9   /* numero massimo di variabili per il trace */

/********************************/
/* variabili per gestione trace */
/********************************/
static  unsigned short  trcIstBrk;
static  unsigned short  trcPrn;
static  unsigned short  trcStep;
static  unsigned short  trcCnt;                     /* contatore per rallentamento  */
static  unsigned short  trcI;                       /* indice di stampa             */
static  unsigned short  trcK;                       /* indice di stampa             */
static  unsigned short  trcMat[TRACE_NCAMP][MAX_VARTRACE];
static  struct varEntry *varTrace[MAX_VARTRACE];    /* puntatori alle variabili     */
                                                    /* di cui si richiede il trace  */

/******************************/
/* variabili per gestione brk */
/******************************/
static  char            brkComando[50];     /* contiene la stringa esplicativa del break    */
static  char            brkTest;            /* tipo di test da eseguire                     */
static  short           brkValore;          /* valore di confronto del test                 */
static  short           brkTipo;            /* tipo di break da eseguire                    */
static  short           brkPretrigger;      /* numero di campioni dopo break trace          */
static  struct varEntry *varBreak;          /* puntatore alla var per break                 */

/****************************************************/
/* prototipi delle funzioni di gestione dei comandi */
/****************************************************/
static  void    cmdBrk(    short argc, char* argv[]);
static  void    cmdTrace(  short argc, char* argv[]);

/***************************/
/* inizializzazione modulo */
/***************************/
void trcbk_init(void)
{
    unsigned short i;

    trcIstBrk   = 0;
    trcStep     = 1;
    trcCnt      = 0;
    trcI        = 0;
    trcK        = 0;
    trcPrn      = 0;

    for(i=0; i<MAX_VARTRACE; i++)
    {
        varTrace[i] = (struct varEntry*) 0;
    }

    consolle_addcmd("trace",    "[<step>|<var1>..<var9>]",  "trace variabili",          cmdTrace);

    strcpy(brkComando,"Break disabilitato");
    brkTest         = 'x';
    brkValore       = 0;
    brkTipo         = BRKTRACE;
    brkPretrigger   = 0;

    varBreak        = (struct varEntry*) 0;

    consolle_addcmd("brk",      "[condiz [pretrig]]",       "break trace e/o programma",cmdBrk);
}

/*******************************************************/
/* restituisce 1 se la condizione di brk e' verificata */
/*******************************************************/
short trcbk_test(void)
{
    static  unsigned short  brkCont;
            unsigned short  brkTrue;
            short           brkValAtt;

    brkTrue     = 0;
    if (varBreak!=NULL)
    {
        if ((varBreak->flags & VARDB_TYPEMASK) == VARDB_TYPECHAR)
        {
            brkValAtt   = (short)*((char far  *)varBreak->pvar);
        }
        else
        {
            brkValAtt   = (short)*((short far *)varBreak->pvar);
        }

        switch(brkTest)
        {
            case '=':
                if (brkValAtt == brkValore)
                    brkTrue=1;
            break;
            case '>':
                if (brkValAtt  > brkValore)
                    brkTrue=1;
            break;
            case '<':
                if (brkValAtt  < brkValore)
                   brkTrue=1;
            break;
        }
    }

    if (brkTrue)
    {
        if (brkCont == brkPretrigger)
        {
            return(brkTipo);
        }
        else
        {
            brkCont++;
            return(0);
        }
    }
    else
    {
        brkCont=0;
        return(0);
    }
}

void trcbk_sample(void)
{
    if (!trcPrn)
    {
        if (++trcCnt == trcStep)
        {
            short           tmpMat, i;
            struct varEntry *ptEntry;

            trcCnt = 0;
            for (i=0; i<MAX_VARTRACE; i++)
            {
                ptEntry     = varTrace[i];
                if      (ptEntry==NULL)
                {
                    tmpMat = 0;
                }
                else if ((ptEntry->flags & VARDB_TYPEMASK) == VARDB_TYPECHAR)
                {
                    tmpMat = (short)*( (char  far *)(ptEntry->pvar) );
                }
                else
                {
                    tmpMat = (short)*( (short far *)(ptEntry->pvar) );
                }

                trcMat[trcIstBrk][i] = tmpMat;
            }

            if(++trcIstBrk == TRACE_NCAMP)
            {
                trcIstBrk=0;
            }
        }
    }
}

/*******************************************************************/
/* stampa a terminale il contenuto del buffer di trace             */
/*******************************************************************/
void trcbk_print()
{
    if (trcPrn)
    {
        if (trcK==0)
        {
            consolle_print("\n");
        }

        if (trcK++ < TRACE_NCAMP)
        {
            register short j;

            if (trcI == TRACE_NCAMP)
            {
                trcI = 0;
            }

            consolle_print("%2d",trcK);

            for (j=0;j<MAX_VARTRACE;j++)
            {
                consolle_print(FRMTVAR_INT,trcMat[trcI][j]);
            }
            consolle_print("\n");

            trcI++;
        }
        else
        {
            short   k;

            consolle_print("ncamp");
            for (k=0; k<MAX_VARTRACE; k++)
                if (varTrace[k]==NULL)
                {
                    consolle_print(",(Null)");
                }
                else
                {
                    consolle_print(",%s",varTrace[k]->nomeVar);
                }

            consolle_print("\n");
            consolle_print("step %u,  pretrig %u\n",trcStep, brkPretrigger);

            trcPrn  = 0;
            trcK    = 0;
        }
    }
}

/************************/
/* gestione comando brk */
/************************/
static void cmdBrk(short argc, char* argv[])
{
    if ( argc > 4 )
    {
        /* si sono specificati almeno 3 parametri */
        struct varEntry *ptEntry;

        if((ptEntry = vardb_findvar(argv[1])) != NULL)
        {
            varBreak    = ptEntry;
            if (brkTipo==BRKPROGRAMMA)
            {
                strcpy(brkComando,"Break PROGRAMMA per ");
            }
            else
            {
                strcpy(brkComando,"Break trace     per ");
            }

            strcat(strcat(strcat(brkComando,argv[1]),argv[2]),argv[3]);
            brkValore       = atoi(argv[3]);
            brkTest         = argv[2][0];

            if ( argc == 5)
            {
                brkPretrigger   = atoi(argv[4]);
            }
            else
            {
                brkPretrigger   = 0;
            }
        }
    }
    else if ( strcmp(argv[1],"TOTALE") == 0)
    {
        brkTipo         = BRKPROGRAMMA;
        brkPretrigger   = 0;
        brkComando[6]   ='P';
        brkComando[7]   ='R';
        brkComando[8]   ='O';
        brkComando[9]   ='G';
        brkComando[10]  ='R';
        brkComando[11]  ='A';
        brkComando[12]  ='M';
        brkComando[13]  ='M';
        brkComando[14]  ='A';
    }
    else
    {
        consolle_print("%s,   pretrig=%u\n",brkComando,brkPretrigger);
    }
}

/**************************/
/* gestione comando trace */
/**************************/
static void cmdTrace(short argc, char* argv[])
{
    if ( argc > 1 )
    {
        if (isdigit(argv[1][0]))
        {
           trcStep  = atoi(argv[1]);
           trcCnt   = 0;
        }
        else
        {
            short           i;
            struct varEntry *ptEntry;

            for(i=0; i<MAX_VARTRACE; i++)
            {
                if (argc > i+1)
                {
                    if ((ptEntry = vardb_findvar(argv[i+1])) != NULL)
                    {
                        varTrace[i]     = ptEntry;
                    }
                }
            }
        }
    }
    else
    {
        trcPrn  = 1;
        trcI    = trcIstBrk;
    }
}


/* ****************************************************************************
    $Log: trcbk.c,v $
    Revision 1.4  2004/11/05 13:24:17  mungiguerrav
    2004/nov/05,ven 14:00          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.3  2004/08/31 14:25:15  mungiguerrav
    2004/ago/31,mar 16:25        Napoli          Mungi\Busco
    - Si effettua Merge da branch makefile

    Revision 1.2.2.1    2004/07/14 15:39:52     buscob
01.07   2003/dic/17,mer         Napoli                  Nappo/Busco
        - Si modifica l'utilizzo della struttura varEntry in conformita' con le
          modifiche apportate alla stessa in vardb.h

    Revision 1.2        2004/06/15 12:02:39     buscob
01.06   2003/nov/30,dom             Madrid              Mungi
        - Si applicano le regole su parentesi e nomi dal manuale SW
01.05   2003/nov/21,ven             Napoli              Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso

    Revision 1.1      2002/05/17 09:53:28   accardol
    - Inserimento in CVS della versione in lib07m
    Revision 1.1.1.1  2002/05/17 09:53:28   accardol
    - Inserimento in CVS in branch STP della versione in lib07m
01.04   2002/mag/17,ven             Napoli              Busco
        - Si sost. include query.h   con   include vardb.h
        - Si sost. query_findvar     con   vardb_findvar
        - Si sost. atol con atoi nella lettura di trace_step

 ------------------------   Prima di CVS    ------------------------
01.03   2000/feb/08,mar             Napoli              DeMarco
        - Si mod. stampa della riga finale del trace
01.02   2000/feb/05,sab             Napoli              Mungi/Busco
        - Si definisce il comportamento di trcbk_sample anche quando
          var_trace[i] non e' definita
        - Si elim. include ctype.h e stdarg.h
01.01   2000/feb/02,mer             Napoli              Busco/Mungi
        - Si definisce il comportamento di trcbk_test anche quando
          var_break non e' definita
01.00   2000/feb/01,mar             Napoli              Mungi
        - Creazione a partire da consolle.c v.5.01 del 06.dic.1999
          estraendo i pezzi relativi a trace e brk
    /---------------storia di consolle.c relativa a questi comandi--------------------------\
    |   1.06 19.feb.96  - Si elimina bug in gest_trace che comportava il blocco di trace    |
    |                   - Nel trace l'indice parte ora sempre da 0                          |
    |   2.00 23.feb.96  - Si ottimizza consolle_trace aggiungendo le variabili              |
    |                     struct var_entry near var_trace1..3                               |
    |                   - Si ottimizza consolle_brk_test                                    |
    |   2.08 18.lug.96  - Si agg. gest. brk_tipo per BRK trace e BRK PROGRAMMA              |
    |   4.03    Gio.25.feb.99           Napoli                  Mungi                       |
    |           - Si incr. matrice trace da 3 x 100   a   9 x 100                           |
    |           - La stampa di matrice trace e' tempificata da query_prn                    |
    |             gest_brk(), consolle_brk_test(): si aggiunge pretrigger                   |
    |   5.00    Ven.03.dic.99           Napoli                  Busco/Mungi                 |
    |           - Si usano i formati di stampa .frmtaux invece di .frmt nelle               |
    |             stampe di view e trace                                                    |
    \---------------------------------------------------------------------------------------/
*/

