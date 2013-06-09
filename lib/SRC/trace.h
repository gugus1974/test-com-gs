/********************************************************************/
/* 2004/nov/15,lun 11:10                \SWISV\LIB\SRC\trace.h      */
/********************************************************************/
/*
    $Date: 2004/11/16 15:38:46 $          $Revision: 1.3 $
    $Author: mungiguerrav $
*/


#ifndef TRACE_INCLUDED
#define TRACE_INCLUDED

#include "stream.h"

void trace_init(        hStream         Stream,
                        char            *TraceBuffer,
                        unsigned short  size);

void trace_sample(      void );
void trace_ForceTrigger(void );
void trace_write(       void );

#endif


/* ****************************************************************************
    $Log: trace.h,v $
    Revision 1.3  2004/11/16 15:38:46  mungiguerrav
    2004/nov/15,lun 11:10          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.2        2004/06/15 12:01:03     buscob
01.02   2003/nov/21,ven             Napoli              Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso

    Revision 1.1        2002/05/17 10:12:14     accardol
    - Inserimento in CVS della versione in lib07m
    Revision 1.1.1.1    2002/05/17 10:12:14     accardol
    - Inserimento in CVS in branch STP della versione in lib07m
        La data è arretrata con un artificio
01.01   2002/mag/17,ven             Napoli              Busco
        - Fatta il 2001.ago.29,mer
        - Si elim. prototipo funzione trace_print()

 ------------------------   Prima di CVS    ------------------------
01.00   2001/lug/31,mar             Napoli              Busco
        - Creazione a partire da trcbk.h versione 1.04
*/

