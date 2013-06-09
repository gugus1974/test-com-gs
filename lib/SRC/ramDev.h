/****************************************************************/
/* 2004/nov/30,mar 16:40                \SWISV\LIB\SRC\ramdev.h */
/****************************************************************/
/*          Driver per memorizzazioni su RAM                    */
/****************************************************************/
/*
    $Date: 2004/12/13 12:06:37 $          $Revision: 1.3 $
    $Author: mungiguerrav $
*/


short           ramDev_read(        char    *ptSrc, char            *ptDst, unsigned short  size );

short           ramDev_write(       char    *ptDst, char            *ptSrc, unsigned short  size );

short           ramDev_SectorErase( char    *ptDst, unsigned short  sector );

unsigned long   ramDev_SectorSize( void );


/* ****************************************************************************
    $Log: ramDev.h,v $
    Revision 1.3  2004/12/13 12:06:37  mungiguerrav
    2004/nov/30,mar 16:40           Napoli      Mungi
    - Si cambia il nome del modulo      da  ramdev          a   ramDev
    - Si cambia nome routine pubblica   da  _sector_erase   a   _SectorErase

    Revision 1.2  2004/11/09 14:59:30  mungiguerrav
    2004/nov/09,mar 15:50          Napoli       Mungi
    - Si aggiungono keyword CVS e breve storia


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.1        2002/05/20 14:11:50     accardol
    - Inserimento in CVS della versione in lib07n
    Revision 1.1.1.1    2002/05/20 14:11:50     accardol
    - Inserimento in CVS in branch STP della versione in lib07n
01.01   2002/mag/20,lun             Napoli              Busco/Mungi
        - Si agg. il prot.  unsigned long ramdev_SectorSize( void )

 ------------------------   Prima di CVS    ------------------------
01.00   1999/ago/31,mar             Napoli              Busco/Mungi
        - Creazione
*/

