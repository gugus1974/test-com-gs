/********************************************************************/
/* 2004/nov/26,ven 12:05                \SWISV\TCNS\SRC\malloc.c    */
/********************************************************************/
/* Memory allocation routines                                       */
/********************************************************************/
/*
    $Date: 2004/11/26 11:12:57 $          $Revision: 1.2 $
    $Author: mungiguerrav $
*/


#ifndef _MALLOC_
#define _MALLOC_

/*==========================================================================*/
/* Prototypes */

void            init_mempool(   char *p,    unsigned int size);
char            *malloc(        char *pool, unsigned int size);
void            free(           char *p);
unsigned short  mempool_use(    char *pool);

#endif


/* ****************************************************************************
    $Log: malloc.h,v $
    Revision 1.2  2004/11/26 11:12:57  mungiguerrav
    2004/nov/26,ven 12:05           Napoli      Mungi
    - Si aggiungono keyword CVS e storia invertita
    - Si sostituiscono TAB con 4 spazi


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision            1.1         2004/11/26 09:45:54     mungiguerrav
    2004/nov/26,ven 10:48           Napoli      Mungi
    - Si aggiungono immodificati dalle directory inferiori TCNS\A\XXX\SRC,
      prima di iniziare il processo di ripulitura e adeguamento alle regole
      di codifica e alle regole di gestione CVS

 --------------  Spostamento da A\XXX\SRC a SRC  ---------------------

    Revision            1.2         2004/09/02 11:31:18     mungiguerrav
    2004/set/02,gio 13:25   Napoli          Mungi\Busco
    - Merge da branch di sviluppo  makefile
    - E' in effetti la creazione dei file nelle nuove directory dedicate alle
      singole schede registrate (DEA,DEV,STA,STB,STR)
    - Non sono ancora inseriti i token di gestioni automatiche CVS (DATE, FILE, AUTHOR, LOG)

    Revision [makefile] 1.1.2.1     2004/07/09 16:24:25     buscob
    - Inserimento in CVS in branch makefile
    - Addio monolite !!
01.04   1997/apr/22,mar             Napoli              Carnevale
        - Modifica non documentata

 ------------------------   Prima di CVS    ------------------------
01.03   199X/mmm/gg                 Napoli              Carnevale
        - Modifica non documentata
01.02   199X/mmm/gg                 Napoli              Carnevale
        - Modifica non documentata
01.01   199X/mmm/gg                 Napoli              Carnevale
        - Modifica non documentata
01.00   1996/mmm/gg                 Napoli              Carnevale
        - Creazione
*/

