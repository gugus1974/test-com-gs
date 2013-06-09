/********************************************************************/
/* 2004/nov/30,mar 16:20                \SWISV\LIB\SRC\nvramDev.c   */
/********************************************************************/
/*      Driver per memorizzazioni su Non Volatile RAM               */
/********************************************************************/
/*
    $Date: 2004/12/13 11:56:02 $          $Revision: 1.3 $
    $Author: mungiguerrav $
*/


#include "string.h"

#include "consolle.h"
#include "nvramDev.h"


#define     SECTORSIZE      0x400L
#define     FILLCHAR        0xFF


short   nvramDev_read( char XHUGE* ptSrc, char XHUGE* ptDst, unsigned short size )
{
    unsigned short i;

    /* Raddoppia l'indirizzo per accedere alla NVRAM che e' a soli 8 bit */
    ptSrc = (char XHUGE*) ((((unsigned long)ptSrc & 0x3FFFFL) << 1) | ((unsigned long)ptSrc & 0xFC0000L));

    /* Copia la zona di memoria leggendo nei soli byte bassi */
    for(i=0; i<size; i++)
    {
        *((char XHUGE*)ptDst+i) = (char)(*((short XHUGE*)ptSrc+i));
    }

    return ( 0 );
}

short   nvramDev_write( char XHUGE* ptDst, char XHUGE* ptSrc, unsigned short size )
{
    unsigned short i;

    /* Raddoppia l'indirizzo per accedere alla NVRAM che e' a soli 8 bit */
    ptDst = (char XHUGE*) ((((unsigned long)ptDst & 0x3FFFFL) << 1) | ((unsigned long)ptDst & 0xFC0000L));

    /* Copia la zona di memoria scrivendo nei soli byte bassi */
    for(i=0; i<size; i++)
    {
        *((short XHUGE*)ptDst+i) = (short)(*((char XHUGE*)ptSrc+i));
    }

    return ( 0 );
}

short   nvramDev_SectorErase( char XHUGE* ptDst, unsigned short sector )
{
    void XHUGE*     base;
    unsigned short  i;

    /* Raddoppia l'indirizzo per accedere alla NVRAM che e' a soli 8 bit */
    ptDst = (char XHUGE*) ((((unsigned long)ptDst & 0x3FFFFL) << 1) | ((unsigned long)ptDst & 0xFC0000L));

    /* calcola l'indirizzo di inizio del settore a cui appartiene l'indirizzo ptDst fornito */
    base    = (void XHUGE*) (((long) ptDst & ~(2*SECTORSIZE-1L)) + sector * 2*SECTORSIZE);

    /* azzera la memoria scrivendo a 16 bit */
    for(i=0; i<SECTORSIZE; i++)
    {
        *((short XHUGE *)base+i) = FILLCHAR;
    }

    return ( 0 );
}

/******************************************************************/
/* Restituisce la dimensione fisica di un settore del dispositivo */
/******************************************************************/
unsigned long nvramDev_SectorSize( void )
{
    return(SECTORSIZE);
}


/* ****************************************************************************
    $Log: nvramDev.c,v $
    Revision 1.3  2004/12/13 11:56:02  mungiguerrav
    2004/nov/30,mar 16:20           Napoli      Mungi
    - Si cambia il nome del modulo da   nvramdev   a   nvramDev
    - Per rendere possibile la compilazione a scopo di debug anche in BorlandC,
      si utilizza la macro XHUGE, definita in nvramDev.h

    Revision 1.2  2004/11/08 15:58:58  mungiguerrav
    2004/nov/08,lun 16:50          Napoli       Mungi
    - Si aggiungono keyword CVS e breve storia


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.1        2002/05/17 08:57:08     accardol
    - Inserimento in CVS della versione in lib07n
    Revision 1.1.1.1    2002/05/20 08:57:08     accardol
    - Inserimento in CVS in branch STP della versione in lib07n
01.00	2002/mag/17,ven             Napoli              Busco
        - Creazione fatta il 2001/lug/23,lun
*/

