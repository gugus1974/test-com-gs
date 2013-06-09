/****************************************************************/
/* 2004/nov/08,lun 12:45                \SWISV\LIB\SRC\mdvDev.h */
/****************************************************************/
/*      Driver per memorizzazioni su scheda MDV ed MDV-02       */
/****************************************************************/
/*
    $Date: 2004/11/08 11:52:56 $          $Revision: 1.2 $
    $Author: mungiguerrav $
*/


short   mdvdev_read(  char *ptSrc, char *ptDst, unsigned short size );

short   mdvdev_write( char *ptDst, char *ptSrc, unsigned short size );

short   mdvdev_sector_erase( char *ptDst, unsigned short sector );

unsigned long mdvdev_SectorSize( void );


/* ****************************************************************************
    $Log: mdvDev.h,v $
    Revision 1.2  2004/11/08 11:52:56  mungiguerrav
    2004/nov/08,lun 12:45          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.1        2002/05/20 14:14:52     accardol
    - Inserimento in CVS della versione in lib07n
    Revision 1.1.1.1    2002/05/20 14:14:52     accardol
    - Inserimento in CVS in branch STP della versione in lib07n
01.01   2002/mag/20,lun             Napoli              Busco\Mungi
        - Si agg. prot. unsigned long mdvdev_SectorSize( void );

 ------------------------   Prima di CVS    ------------------------
01.00   1999/ott/21,gio             Napoli              Busco
        - Creazione a partire da am29dev.h vers.1.00
*/

