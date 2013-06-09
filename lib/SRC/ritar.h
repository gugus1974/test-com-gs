/****************************************************************/
/* 2003/nov/11,gio 08:55                \SWISV\LIB\SRC\ritar.h  */
/****************************************************************/
/* Prototipi funzioni definite nel modulo ritar.c               */
/* Struttura del timer che gestisce il ritardo                  */
/****************************************************************/
/*
    $Date: 2004/11/11 09:45:26 $          $Revision: 1.3 $
    $Author: mungiguerrav $
*/


#ifndef  _RITAR_H_
#define  _RITAR_H_

typedef struct timerStruct
{
    unsigned short  massimo;    /* Massimo   del timer  */
    unsigned short  cont;       /* Contatore del timer  */
    unsigned short  buffer;     /* Buffer    del timer  */
};

void            ritar_init(     struct timerStruct      *delay,
                                unsigned short          max,
                                unsigned short          cnt,
                                unsigned short          buf);
unsigned char   ritar_fisso(    struct timerStruct      *delay);
unsigned char   ritar_ciclico(  struct timerStruct      *delay);
void            ritar_ricar(    struct timerStruct      *delay);
void            ritar_reinit(   struct timerStruct      *delay);
void            ritar_remax(    struct timerStruct      *delay,
                                unsigned short          new_max);

#endif


/* ****************************************************************************
    $Log: ritar.h,v $
    Revision 1.3  2004/11/11 09:45:26  mungiguerrav
    2004/nov/11,gio 08:55          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.2        2004/06/15 11:50:06     buscob
02.01   2003/nov/21,ven             Napoli              Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso

    Revision 1.1        2000/03/03 13:35:38     accardol
    - Inserimento in CVS della versione in lib07m
    Revision 1.1.1.1    2000/03/03 13:35:38     accardol
    - Inserimento in CVS in branch STP della versione in lib07m
        La data è arretrata con un artificio
02.00   2000/mar/03,ven             Napoli              Mungi/Balbi/Speda
        - Diventa un modulo di libreria con nome ritar.h
        - Si definiscono la         struct timerStruct      e i prototipi:
                void            ritar_init(..)
                unsigned char   ritar_fisso(..)
                unsigned char   ritar_ciclico(..)
                void            ritar_ricar(..)
                void            ritar_reinit(..)
                void            ritar_remax(..)

 ------------------------   Prima di CVS    ------------------------
01.00   1996/apr/15,lun             Napoli              Spedaliere
        - Creazione con nome gestcont.h
*/

