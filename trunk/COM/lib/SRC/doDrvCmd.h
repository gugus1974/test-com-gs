/********************************************************************/
/* 2004/ott/28,gio 15:45                \SWISV\LIB\SRC\dodrvcmd.h   */
/********************************************************************/
/*
    $Date: 2004/10/28 13:54:01 $          $Revision: 1.4 $
    $Author: mungiguerrav $
*/


typedef char* (*tPrnChDescFun)(unsigned short doNumber, unsigned short ch);

void dodrvcmd_init( tPrnChDescFun pcdf );


/* ****************************************************************************
    $Log: doDrvCmd.h,v $
    Revision 1.4  2004/10/28 13:54:01  mungiguerrav
    2004/ott/28,gio 15:45          Napoli       Mungi\Nappo
    - Si riporta sul tronco principale la modifica sulle do multiple da
      work_calabresea, erroneamente non effettuata nella versione precedente

    Revision 1.3  2004/10/25 10:49:39  mungiguerrav
    2004/ott/25,lun 12:40          Napoli       Mungi
    - Si riporta sul tronco principale la modifica effettuata nel branch
      work_calabresea  su gestione più schede do
    - Si aggiungono keyword CVS e storia passata


    Revision 1.2.6.1  2004/10/13 11:49:19  calabresea
    introduzione DO multipla

 ---------------    Fase transitoria tra *.mod e CVS    -------------
    Revision 1.2      2004/06/15 10:19:34     buscob
01.02   2003.lug.16,mer             Napoli                  Mungi
        - Si agg. tipo di funzione applicativo per gestione stampa descrizione canale OD:
            typedef char* (*tPrnChDescFun)(unsigned short ch)
        - Si aggiunge parametro a:
            void dodrvcmd_init( tPrnChDescFun pcdf )

    Revision 1.1      2002/05/20 14:45:22   accardol
    - Inserimento in CVS
    Revision 1.1.1.1  2002/05/20 14:45:22   accardol
    - Inserimento in CVS in branch STP
01.01   2002/mag/20,lun         Napoli                  Busco
        - Si sposta da AIACE\SRC    a   LIB\SRC

 ----------------    Prima di CVS    -----------------------------
01.00    2001/feb/28,mer         Napoli                  Accardo
        - Si sposta da \SWISV\TLRV\SRC in \SWISV\AIACE\SRC.
----------------------Storia precedente di TLRV-----------------------
01.00   2000/feb/07,lun - Creazione
*/

