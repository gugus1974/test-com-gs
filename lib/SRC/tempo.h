/****************************************************************/
/* 2004/nov/05,ven 15:20                \SWISV\LIB\SRC\tempo.h  */
/****************************************************************/
/*      Utility per la misura del tempo trascorso               */
/****************************************************************/
/*
    $Date: 2004/11/05 14:30:37 $          $Revision: 1.3 $
    $Author: mungiguerrav $
*/


struct typeTempo
{
    unsigned short      tot;
    unsigned short      cen;
    unsigned short      sec;
    unsigned short      min;
    unsigned short      ora;
    unsigned short      incr;
    unsigned short      timer;
};

extern struct typeTempo tempo;

void tempo_init(    unsigned char   incremento,
                    unsigned char   ora0,
                    unsigned char   min0,
                    unsigned char   sec0,
                    unsigned short  timer       );

void tempo_incr(    void                        );


/* ****************************************************************************
    $Log: tempo.h,v $
    Revision 1.3  2004/11/05 14:30:37  mungiguerrav
    2004/nov/05,ven 15:20          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.2            2004/06/15 11:57:59     buscob
01.04   2003/nov/30,dom             Madrid                  Mungi
        - Si cambiano struct typeTempo e prototipo tempo_init(),
          aggiungendo   unsigned short timer
01.03   2003/nov/21,ven             Napoli                  Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso

    Revision 1.1            2000/05/24 14:54:44     accardol
    - Inserimento in CVS della versione in lib07m
    Revision 1.1.1.1        2000/05/24 14:54:44     accardol
    - Inserimento in CVS in branch STP della versione in lib07m
        (la data è arretrata con un artificio)
01.02   2000/mag/24,mer             Napoli              Mungi
        - Si mod. prot. void tempo_init(Uchar incremento,
                                        Uchar ora0,
                                        Uchar min0,
                                        Uchar sec0  );

 ------------------------   Prima di CVS    ------------------------
01.01   1998/feb/21,sab
        - Si mod. tempo.tot da Ulong fino a 360000 = 60 min
                            in Uint  fino a  60000 = 10 min
01.00   1998/feb/21,sab     - Creazione
*/

