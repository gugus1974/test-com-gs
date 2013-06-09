/********************************************************************/
/* 2004/nov/15,lun 08:30                \SWISV\LIB\SRC\queryCmd.c   */
/********************************************************************/
/*
    $Date: 2004/11/16 10:10:15 $          $Revision: 1.3 $
    $Author: mungiguerrav $
*/


#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "consolle.h"
#include "querycmd.h"
#include "query.h"
#include "ctype.h"

/****************************************************/
/* prototipi delle funzioni di gestione dei comandi */
/****************************************************/
static void gest_query (short argc,  char* argv[]);


/******************/
/* parametri vari */
/******************/
void querycmd_init(void)
{
    consolle_addcmd("query", "[enable]", "enable/disable query ",      gest_query);
}

/****************************/
/* gestione comando query   */
/****************************/
static void gest_query(short argc, char* argv[])
{
    short intParam;

    if (argc>1)
    {
        intParam = atoi(argv[1]);
        switch(intParam)
        {
            case 1:
                query_enable();
                query_setmode(0);
                break;
            case 2:
                query_enable();
                query_setmode(1);
                break;
            default:
                query_disable();
                query_setmode(0);
                break;
        }
    }

    consolle_print("Query mode: %d\n",query_status());
}


/* ****************************************************************************
    $Log: queryCmd.c,v $
    Revision 1.3  2004/11/16 10:10:15  mungiguerrav
    2004/nov/15,lun 08:30          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.2        2004/06/15 11:47:34     buscob
02.01   2003/nov/21,ven             Napoli              Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso.

    Revision 1.1        2002/05/17 09:49:08     accardol
    - Inserimento in CVS della versione in lib07m
    Revision 1.1.1.1    2002/05/17 09:49:08     accardol
    - Inserimento in CVS in branch STP della versione in lib07m
        La data è arretrata con un artificio
02.00   2002/mag/17,ven             Napoli              Busco
        - Fatta il 2001/nov/26,lun
        - Il comando query usa un unico parametro:
            0        disabilita query
            1 (def)  abilita query in modo compatibile con ATRterm 4.2
            2        abilita query in modo compatibile con ATRterm 5.x
        - Si elim. tutti i riferimenti alla gestione delle variabili spostata in varDBcmd.*

 ------------------------   Prima di CVS    ------------------------
01.06   2000/lug/18,mer             Napoli              Busco
        - Si gestisce il secondo parametro nel comando di consolle query per impostare il query mode
01.05   2000/gen/28,ven             Napoli              Mungi/Busco
        - Si utilizza (xxx far *) nei cast per stampare variabili
01.04   2000/gen/27,gio             Napoli              Mungi/Busco
        - Si castizza in is_aux la variabile varauxnames in (char *) ossia si elimina huge
01.03   2000/gen/24,lun             Napoli              Balbi/Busco
        - Si gestiscono le var. char
        - Si agg. la funzione printvar
01.02   2000/gen/19,mer             Napoli              Balbi/Busco/Mungi
        - Si castizza in is_aux la variabile varauxnames in char huge *
01.01   1999/dic/03,ven             Napoli              Busco/Mungi
        - Si traducono le stampe in inglese
        - Si agg. gest_query
        - Si agg. indirizzo nella stampa vars
        - Si elim. bug in gest_query (disabilitava se non c'erano parametri nella linea di comando)
        - Si stampa "Var not found"
01.00   1999/dic/03,ven             Napoli              Busco
        - Creazione
*/

