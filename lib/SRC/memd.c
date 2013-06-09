/****************************************************************/
/* 2004/nov/08,lun 16:30                \SWISV\LIB\SRC\memd.c   */
/****************************************************************/
/*      Gestore dei comandi di accesso a memoria                */
/****************************************************************/
/*
    $Date: 2004/11/08 15:49:38 $          $Revision: 1.3 $
    $Author: mungiguerrav $
*/


#include "stdio.h"
#include "consolle.h"

/******************************************************/
/* prototipi delle funzioni di gestione della memoria */
/******************************************************/
static void gest_mem(    short argc, char* argv[]);
static void gest_dump(   short argc, char* argv[]);
static void gest_punto(  short argc, char* argv[]);

/******************/
/* parametri vari */
/******************/
#define BYTES_PER_ROW  16   /* byte per riga da stampare in dump        */

/****************************************/
/* inizializzazione consolle di comando */
/****************************************/
void memd_init(void)
{
    consolle_addcmd("mem",   "[indir]",                "vis. memoria",          gest_mem);
    consolle_addcmd("dump",  "[indir [lung]]",         "vis. area memoria",     gest_dump);
    consolle_addcmd(".",     "[commento]",             "per inserire commenti", gest_punto);
}

/************************/
/* gestione comando mem */
/************************/
static void gest_mem(short argc, char* argv[])
{
    static unsigned short far *mem_ptr;

    if ( argc > 1 )
    {
        mem_ptr = (unsigned short far *) ((consolle_parsenum(argv[1])>>1)<<1);
        if ( argc > 2 )
        {
            *mem_ptr = (unsigned short) consolle_parsenum(argv[2]);
        }
    }
    consolle_print("0x%08lX = 0x%X\n",(long)mem_ptr,*mem_ptr);
}

/*************************/
/* gestione comando dump */
/*************************/
static void gest_dump(short argc, char *argv[])
{
    unsigned short        i, j, k, size;
    unsigned char       c;
    unsigned char far  *buf;

    if(argc > 2)
    {
        size = (unsigned short) consolle_parsenum(argv[2]);
        size = (size>0x100) ? 0x100 : size;
        size = (size==0x0)  ? 0x1   : size;
    }
    else
        size=1;

    if (argc > 1 && argc < 4)
    {
        buf=(unsigned char far *)consolle_parsenum(argv[1]);
        for (i = 0; i <= (size-1) / BYTES_PER_ROW; i++)
        {
            consolle_print("%08lx:", (long)buf + (long)(i * BYTES_PER_ROW));
            for (j = 0; j < BYTES_PER_ROW; j++)
            {
                k = i*BYTES_PER_ROW + j;
                if (k < size)           consolle_print(" %02x", buf[k]);
                else                    consolle_print("   ");
            }
            consolle_print(" '");
            for (j = 0; j < BYTES_PER_ROW; j++)
            {
                k = i*BYTES_PER_ROW + j;
                if (k < size)
                {
                    c = buf[k];
                    if (c >= ' ')       consolle_print("%c", c);
                    else                consolle_print(".");
                }
                else
                    consolle_print(" ");
            }
            consolle_print("'\n");
        }
    }
    else if (argc>3)
    {
        if (size==(argc-3))
        {
            buf = (unsigned char far *)consolle_parsenum(argv[1]);
            for (i = 0; i < size; i++)
                *(buf+i) =(unsigned char)consolle_parsenum(argv[3+i]);
            for (i = 0; i <= (size-1) / BYTES_PER_ROW; i++)
            {
                consolle_print("%08lx:", (long)buf + (long)(i * BYTES_PER_ROW));
                for (j = 0; j < BYTES_PER_ROW; j++)
                {
                    k = i*BYTES_PER_ROW + j;
                    if (k < size)           consolle_print(" %02x", buf[k]);
                    else                    consolle_print("   ");
                }
                consolle_print(" '");
                for (j = 0; j < BYTES_PER_ROW; j++)
                {
                    k = i*BYTES_PER_ROW + j;
                    if (k < size)
                    {
                        c = buf[k];
                        if (c >= ' ')       consolle_print("%c", c);
                        else                consolle_print(".");
                    }
                    else
                        consolle_print(" ");
                }
                consolle_print("'\n");
            }
        }
        else
            consolle_print("ERRORE: numero parametri errato\n");
    }
}

/************************/
/* gestione comando .   */
/************************/
static void gest_punto(short argc, char* argv[])
{
    argc = argc;
    argv[0] = argv[0];
}


/* ****************************************************************************
    $Log: memd.c,v $
    Revision 1.3  2004/11/08 15:49:38  mungiguerrav
    2004/nov/08,lun 16:30          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.2        2004/06/15 11:36:43     buscob
01.04   2003/nov/21,ven             Napoli              Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso

    Revision 1.1        2001/07/30 14:34:50     accardol
    - Inserimento in CVS della versione in lib07m
    Revision 1.1.1.1    2001/07/30 14:34:50     accardol
    - Inserimento in CVS in branch STP della versione in lib07m
        La data è arretrata con un artificio
01.03   2001/lug/30,lun             Napoli              Accardo
        - Si sost. in consolle_print() di gest_mem()
            (unsigned int far *)mem_ptr con
            (long)mem_ptr
          per correggere scritta a video dell'indirizzo.

 ------------------------   Prima di CVS    ------------------------
01.02   2001/gen/26,ven             Napoli              Busco,Mungi
        - Si stampa indirizzo puntato in memoria invece della stringa inserita con il comando
01.01   2000/feb/01,mar             Napoli              Mungi
        - Si elim. include stdlib.h, ctype.h, string.h, stdarg.h
01.00   2000/gen/28,ven             Napoli              Mungi
        - Creazione a partire da consolle.c v.5.01 del 06.dic.1999
          estraendo i pezzi relativi a mem, dump e '.'
    /---------------storia di consolle.c relativa a questi comandi------------------\
    |   3.07 14.mar.97  - Si agg. comando dump;                                     |
    |                   - Si agg. routine consolle_parsenum per input numeri        |
    |                     esadecimali nei comandi mem e dump;                       |
    |   3.08 26.mar.96  - Si agg. possibilita' di scrittura al comando dump         |
    |                   - Si controlla 1=<size=<0x100                               |
    |                   - 'mem' ristampa valore ultimo indirizzo                    |
    |   3.09 07.mag.97  - In gest_mem si modifica cast per adeg. nuovo ambiente     |
    |   3.11    Mer.19.nov.97           Napoli                  Mungi               |
    |           - Si agg. stampa '. ' ad inizio messaggi                            |
    |           - Si agg. gest_punto() per inserire commenti                        |
    |   4.04    Gio.08.apr.99           Napoli                  Mungi               |
    |           - Si eliminano i "." ad inizio righe di stampa                      |
    |   4.05    Gio.15.apr.99           Napoli                  Mungi               |
    |           - Si continua ad eliminare i "." ad inizio righe di stampa          |
    \-------------------------------------------------------------------------------/
*/

