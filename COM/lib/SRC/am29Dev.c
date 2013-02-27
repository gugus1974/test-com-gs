/********************************************************************/
/* 2004/nov/08,lun 09:30                \SWISV\LIB\SRC\am29dev.c    */
/********************************************************************/
/*                                                                  */
/********************************************************************/
/*
    $Date: 2004/11/08 09:27:54 $          $Revision: 1.2 $
    $Author: mungiguerrav $
*/


#include "string.h"

#include "am29f040.h"
#include "am29dev.h"


#define     SECTORSIZE      0x20000L    /* Dimensione fisica del settore                */
#define     MAXRETRY        5           /* Numero massimo di tentativi da effettuare in */
                                        /* caso di insuccesso delle operaz. sulla flash */

typedef unsigned short  Ushort;
typedef unsigned long   Ulong;

short   am29dev_read( char *ptSrc, char *ptDst, Ushort size )
{
    xmemcpy( ptDst, ptSrc, size );
    return ( 0 );
}

short   am29dev_write( char *ptDst, char *ptSrc, Ushort size )
{
    char xhuge *base;
    long       offset;
    short      WriteError;
    char       NumRetry;

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

short   am29dev_sector_erase( char *ptDst, Ushort sector )
{
    char xhuge *base;
    long       absSector;
    short      EraseError;
    char       NumRetry;

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
unsigned long am29dev_SectorSize( void )
{
    return(SECTORSIZE);
}


/* ****************************************************************************
    $Log: am29Dev.c,v $
    Revision 1.2  2004/11/08 09:27:54  mungiguerrav
    2004/nov/08,lun 09:30          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.1        2002/05/20 14:15:54     accardol
    - Inserimento in CVS della versione in lib07n
    Revision 1.1.1.1    2002/05/20 14:15:54     accardol
    - Inserimento in CVS in branch STP della versione in lib07n
02.01   2002/mag/20,lun             Napoli              Busco/Mungi
        - Si agg. la funz. unsigned long am29dev_SectorSize( void )
        - Si agg. define SECTORSIZE  0x20000L

 ------------------------   Prima di CVS    ------------------------
02.00   2002/mag/17,ven             Napoli              Busco
        - Fatta il 2002.mar.11,lun
        - Si effettuano fino a MAXRETRY tentativi successivi per le operazioni di
          scrittura e cancellazione della flash per aumentare la robustezza del sistema.
        - Si attivano i ritorni diagnostici delle funzioni
01.01   1999/set/28,mar             Napoli              Busco
        - Si dichiarano i puntatori base come xhuge
        - Si usa xmemcpy invece di memcpy
        - Si shifta a DX di 17 bit invece che di 16 per calcolare l'absSector
01.00   1999/ago/30,lun             Napoli              Busco
        - Creazione
*/

