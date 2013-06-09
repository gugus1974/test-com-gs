/****************************************************************/
/* 2004/dic/15,mer 15:35                \SWISV\LIB\SRC\ramDev.c */
/****************************************************************/
/*          Driver per memorizzazioni su RAM                    */
/****************************************************************/
/*
    $Date: 2004/12/15 14:48:27 $          $Revision: 1.4 $
    $Author: mungiguerrav $
*/


#include "string.h"

#include "ramDev.h"


#define     SECTORSIZE      0x400
#define     FILLCHAR        0xFF


short   ramDev_read( char* ptSrc, char* ptDst, unsigned short size )
{
    memcpy( ptDst, ptSrc, size );
    return ( 0 );
}

short   ramDev_write( char* ptDst, char* ptSrc, unsigned short size )
{
    memcpy( ptDst, ptSrc, size );

    return ( 0 );
}

short   ramDev_SectorErase( char* ptDst, unsigned short sector )
{
    void*   base;

    base    = (void *) (((long) ptDst & ~(SECTORSIZE-1L)) + sector * SECTORSIZE);

    memset(base, FILLCHAR, SECTORSIZE);

    return ( 0 );
}

/******************************************************************/
/* Restituisce la dimensione fisica di un settore del dispositivo */
/******************************************************************/
unsigned long ramDev_SectorSize( void )
{
    return(SECTORSIZE);
}


/* ****************************************************************************
    $Log: ramDev.c,v $
    Revision 1.4  2004/12/15 14:48:27  mungiguerrav
    2004/dic/15,mer 15:35           Napoli      Mungi\Formato
    - BugFix: dopo prime prove di Formato su Aiace/Madrid,
              si corregge   ramdev_SectorSize
              in            ramDev_SectorSize

    Revision 1.3  2004/12/13 12:05:58  mungiguerrav
    2004/nov/30,mar 16:35           Napoli      Mungi
    - Si cambia il nome del modulo      da  ramdev          a   ramDev
    - Si cambia nome routine pubblica   da  _sector_erase   a   _SectorErase

    Revision 1.2  2004/11/09 14:59:06  mungiguerrav
    2004/nov/09,mar 15:40          Napoli       Mungi
    - Si aggiungono keyword CVS e breve storia


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.1        2002/05/20 14:13:28     accardol
    - Inserimento in CVS della versione in lib07n
    Revision 1.1.1.1    2002/05/20 14:13:28     accardol
    - Inserimento in CVS in branch STP della versione in lib07n
01.01   2002/mag/20,lun             Napoli              Busco/Mungi
        - Si agg. la funz. unsigned long ramdev_SectorSize( void )

 ------------------------   Prima di CVS    ------------------------
01.00   1999/ago/31,mar             Napoli              Busco/Mungi
        - Creazione
*/

