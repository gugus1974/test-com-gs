/********************************************************************/
/* 2004/nov/08,lun 09:35                \SWISV\LIB\SRC\am29dev.h    */
/********************************************************************/
/*                                                                  */
/********************************************************************/
/*
    $Date: 2004/11/08 09:28:43 $          $Revision: 1.2 $
    $Author: mungiguerrav $
*/


short   am29dev_read(  char *ptSrc, char *ptDst, unsigned short size );

short   am29dev_write( char *ptDst, char *ptSrc, unsigned short size );

short   am29dev_sector_erase( char *ptDst, unsigned short sector );

unsigned long am29dev_SectorSize( void );


/* ****************************************************************************
    $Log: am29Dev.h,v $
    Revision 1.2  2004/11/08 09:28:43  mungiguerrav
    2004/nov/08,lun 09:35          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.1        2002/05/20 14:15:36     accardol
    - Inserimento in CVS della versione in lib07n
    Revision 1.1.1.1    2002/05/20 14:15:36     accardol
    - Inserimento in CVS in branch STP della versione in lib07n
01.01   2002/mag/20,lun             Napoli              Busco/Mungi
        - Si agg. il prot. unsigned short am29dev_SectorSize( void )

 ------------------------   Prima di CVS    ------------------------
01.00   1999/ago/30,lun             Napoli              Busco
        - Creazione
*/

