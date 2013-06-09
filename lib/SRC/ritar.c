/****************************************************************/
/* 2004/nov/11,gio 08:45                \SWISV\LIB\SRC\ritar.c  */
/****************************************************************/
/*              Gestione ritardi                                */
/****************************************************************/
/*
    $Date: 2004/11/11 09:44:47 $          $Revision: 1.3 $
    $Author: mungiguerrav $
*/


#include "stdlib.h"
#include "stdio.h"

#include "ritar.h"

void ritar_init( struct timerStruct *delay,
                 unsigned short max,
                 unsigned short cnt,
                 unsigned short buf)
{
    delay->massimo = max;
    delay->cont    = cnt;
    delay->buffer  = buf;
}

unsigned char ritar_fisso(struct timerStruct *delay)
{
    if (delay->cont == 0)
    {
        delay->buffer=delay->cont;
        return 0;
    }
    else
    {
        delay->buffer=((delay->cont)-1);
        return 1;
    }
}

unsigned char ritar_ciclico(struct timerStruct *delay)
{
    if (delay->cont == 0)
    {
        return 0;
    }
    else
    {
        delay->buffer=((delay->cont)-1);
        return 1;
    }
}

void ritar_ricar(struct timerStruct *delay)
{
    delay->cont=delay->buffer;
    delay->buffer=delay->massimo;
}

void ritar_reinit(struct timerStruct *delay)
{
    delay->buffer=delay->massimo;
}

void ritar_remax(struct timerStruct *delay, unsigned short new_max)
{
    delay->massimo=new_max;
}


/* ****************************************************************************
    $Log: ritar.c,v $
    Revision 1.3  2004/11/11 09:44:47  mungiguerrav
    2004/nov/11,gio 08:45          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.2        2004/06/15 11:49:30     buscob
02.02   2003/dic/02,mar             Madrid              Mungi
        - Si elim. un warning del Borland, cancellando l'inutile    include intrins.h
02.01   2003/nov/21,ven             Napoli              Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso

    Revision 1.1        2000/03/03 13:35:34     accardol
    - Inserimento in CVS della versione in lib07m
    Revision 1.1.1.1    2000/03/03 13:35:34     accardol
    - Inserimento in CVS in branch STP della versione in lib07m
        La data è arretrata con un artificio
02.00   2000/mar/03,ven             Napoli              Mungi/Balbi/Speda
        - Diventa un modulo di libreria con nome ritar.c
        - Si agg. la routine _init, che permette il posizionamento
          run-time dei campi del contatore

 ------------------------   Prima di CVS    ------------------------
01.00   1996/apr/15,lun             Napoli              Spedaliere
        - Creazione con nome gestcont.c
        - Parametri in ingresso : Indirizzo della struttura del contatore
                                : Routine da eseguire dopo il ritardo
        - Parametri in uscita   : Un intero 0 o 1 se eseguito il conteggio
        - Gestione contatori ONTD: sono definiti in varcon.h come globali
          e ridefiniti in esterne.h come extern. Sono di due tipi:
                - ad 8 bit per tempi fino a 5 sec.
                - a 16 bit per tempi oltre i 5 sec.
          Sono suddivisi inoltre in timer periodici o fissi (1 o 2).
             - Il timer periodico si azzera e si ripristina con un tempo di
               ciclo prestabilito
             - Il timer fisso una volta azzerato non si ripristina piu'
               finche' e' chiamato.
        - E' un modulo di applicativo, per cui si deve modificare ogni volta che si
          aggiungono contatori (Mungi)
*/

