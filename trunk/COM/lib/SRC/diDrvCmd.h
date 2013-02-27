/********************************************************************/
/* 2004/ott/22,ven 17:25                \SWISV\LIB\SRC\didrvcmd.h   */
/********************************************************************/
/*
    $Date: 2004/10/22 15:38:23 $          $Revision: 1.2 $
    $Author: mungiguerrav $
*/


typedef char* (*tPrintChDescrFunc )(unsigned short diNumber,unsigned short ch);

void didrvcmd_init( tPrintChDescrFunc pcdf );


/* ****************************************************************************
    $Log: diDrvCmd.h,v $
    Revision 1.2  2004/10/22 15:38:23  mungiguerrav
    2004/ott/22,ven 17:25          Napoli       Mungi
    - Si riporta sul tronco principale la correzione effettuata in branch
      didrv_nappoc  su gestione più schede
    - Si aggiungono keyword CVS e storia passata


 ---------------    Fase transitoria tra *.mod e CVS    -------------
    Revision 1.1.1.1.4.1    2004/09/07 07:34:10     nappoc
    2004/set/07         Napoli              Nappo
    - Si adegua il modulo alle modifiche apportate a didrv
      (MNG: mi sembra un po' vago: in pratica si aggiunge parametro diNumber)

    Revision 1.1      2002/05/21 15:35:26   accardol
    - Inserimento in CVS
    Revision 1.1.1.1  2002/05/21 15:35:26   accardol
    - Inserimento in CVS in branch STP
01.02   2002/mag/21,mar         Napoli                  Busco\Mungi
        - Si agg. tipo di funzione applicativo per gestione stampa descrizione canale ID
 ----------------    Prima di CVS    -----------------------------
01.01   2002/mag/20,lun         Napoli                  Busco
        - Si sposta da AIACE\SRC    a   LIB\SRC
01.00   2001/feb/28,mer         Napoli                  Accardo
        - Si sposta da \SWISV\TLRV\SRC in \SWISV\AIACE\SRC.
----------------------Storia precedente di TLRV-----------------------
01.00   2000/feb/07,lun - Creazione
*/
