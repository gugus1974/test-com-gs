/********************************************************************/
/* 2004/nov/11,gio 16:20                \SWISV\LIB\SRC\strParse.h   */
/********************************************************************/
/*              String parsing utility routines                     */
/********************************************************************/
/*
    $Date: 2004/11/12 11:15:43 $          $Revision: 1.2 $
    $Author: mungiguerrav $
*/


#ifndef _STRPARSE_
#define _STRPARSE_

/*==================================================================*/
/* Prototypes */

void        dump_buffer(            char            absolute_addr,
                            const   unsigned char   *buf,
                                    short           size);

short       parse_buffer(   const   char            *s,
                                    unsigned char   *buf,
                                    short           el_size);

long        parse_number(   const   char            *s);

#endif


/* ****************************************************************************
    $Log: strParse.h,v $
    Revision 1.2  2004/11/12 11:15:43  mungiguerrav
    2004/nov/11,gio 16:20          Napoli       Mungi
        - Si aggiungono keyword CVS e breve storia documentata
        - Si sostituiscono i TAB con 4 spazi
        - Si applica regole di codifica per incolonnamento
        - Non si applicano ancora regole di codifica per nomi routine e variabili


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.1        1997/08/27 08:05:50     accardol
    - Inserimento in CVS della versione in lib07n
    Revision 1.1.1.1    1997/08/27 08:05:50     accardol
    - Inserimento in CVS in branch STP della versione in lib07n
        La data è arretrata con un artificio
01.10   1997/ago/27,mer             Napoli              Carnevale
        - Modifiche non documentate

 ------------------------   Prima di CVS    ------------------------
01.01   1997/xxx/XX,xxx             Napoli              Carnevale
        - Modifiche non documentate
01.00   1996/xxx/XX,xxx             Napoli              Carnevale
        - Creazione
*/

