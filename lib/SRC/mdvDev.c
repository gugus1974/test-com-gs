/****************************************************************/
/* 2004/nov/08,lun 12:40                \SWISV\LIB\SRC\mdvDev.c */
/****************************************************************/
/*      Driver per memorizzazioni su scheda MDV ed MDV-02       */
/****************************************************************/
/*
    $Date: 2004/11/08 11:52:24 $          $Revision: 1.2 $
    $Author: mungiguerrav $
*/


#include "string.h"

#include "am29f040.h"
#include "mdvdev.h"

#define BUSY_MEM    0xC40000
#define SECTORSIZE  0x20000L
#define MAXRETRY    5       /* Numero massimo di tentativi da effettuare          */
                            /* in caso di insuccesso delle operazioni sulla flash */

typedef unsigned short  Ushort;
typedef unsigned long   Ulong;

short   mdvdev_read( char *ptSrc, char *ptDst, Ushort size )
{
    xmemcpy( ptDst, ptSrc, size );
    return ( 0 );
}

short   mdvdev_write( char *ptDst, char *ptSrc, Ushort size )
{
    char xhuge *base;
    long       offset;
    short      dummy;
    short      WriteError;
    char       NumRetry;

    dummy = *((volatile unsigned short xhuge*)(BUSY_MEM));

    base    = (char *) ((long) ptDst & 0xFFF00000L);
    offset  =           (long) ptDst & 0x000FFFFFL;

    WriteError = 0;
    NumRetry   = 0;
    do
    {
        WriteError = am29f040_write( base, offset, ptSrc, size );
        NumRetry++;
    } while((NumRetry < MAXRETRY) && WriteError);

    return ( WriteError );

}

short   mdvdev_sector_erase( char *ptDst, Ushort sector )
{
    char xhuge *base;
    long       absSector;
    short      dummy;
    short      EraseError;
    char       NumRetry;

    dummy = *((volatile unsigned short xhuge*)(BUSY_MEM));

    base    = (char *) ((long) ptDst & 0xFFF00000L);

    absSector   = ( ( (long) ptDst - (long) base ) >> 17) + sector;

    EraseError = 0;
    NumRetry   = 0;
    do
    {
        EraseError = am29f040_sector_erase( base, (short) absSector );
    } while((NumRetry < MAXRETRY) && EraseError);


    return ( EraseError );
}

/******************************************************************/
/* Restituisce la dimensione fisica di un settore del dispositivo */
/******************************************************************/
unsigned long mdvdev_SectorSize( void )
{
    return(SECTORSIZE);
}


/* ****************************************************************************
    $Log: mdvDev.c,v $
    Revision 1.2  2004/11/08 11:52:24  mungiguerrav
    2004/nov/08,lun 12:40          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.1        2002/05/20 14:15:06     accardol
    - Inserimento in CVS della versione in lib07n
    Revision 1.1.1.1    2002/05/20 14:15:06     accardol
    - Inserimento in CVS in branch STP della versione in lib07n
02.01   2002/mag/20,lun             Napoli              Busco\Mungi
        - Si agg. funz. unsigned long mdvdev_SectorSize( void );
        - Si agg. define SECTORSIZE  0x20000L

 ------------------------   Prima di CVS    ------------------------
02.00   2002/mar/11,lun             Napoli              Busco
        - Si effettuano fino a MAXRETRY tentativi successivi per le operazioni di
          scrittura e cancellazione della flash per aumentare la robustezza del sistema.
        - Si attivano i ritorni diagnostici delle funzioni
01.00   1999/ott/21,gio             Napoli              Busco
        - Creazione a partire da am29dev.c vers.1.01
*/

