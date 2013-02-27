/********************************************************************/
/* 2004/nov/30,mar 16:25                \SWISV\LIB\SRC\nvramDev.h   */
/********************************************************************/
/*      Driver per memorizzazioni su Non Volatile RAM               */
/********************************************************************/
/*
    $Date: 2004/12/13 11:59:05 $          $Revision: 1.3 $
    $Author: mungiguerrav $
*/


#ifndef _NVRAMDEV_
#define _NVRAMDEV_

/*==================================================================*/
/* Macros */
#ifdef __C166__
    #define XHUGE xhuge
#else
    #define XHUGE huge
#endif


short           nvramDev_read(          char XHUGE  *ptSrc, char XHUGE      *ptDst, unsigned short  size );

short           nvramDev_write(         char XHUGE  *ptDst, char XHUGE      *ptSrc, unsigned short  size );

short           nvramDev_SectorErase(   char XHUGE  *ptDst, unsigned short  sector );

unsigned long   nvramDev_SectorSize(    void );

#endif


/* ****************************************************************************
    $Log: nvramDev.h,v $
    Revision 1.3  2004/12/13 11:59:05  mungiguerrav
    2004/nov/30,mar 16:25           Napoli      Mungi
    - Si cambia il nome del modulo da   nvramdev   a   nvramDev
    - Si aggiunge la macro di protezione _NVRAMDEV_
    - Per rendere possibile la compilazione a scopo di debug anche in BorlandC,
      si definisce la macro condizionata XHUGE

    Revision 1.2  2004/11/08 15:59:24  mungiguerrav
    2004/nov/08,lun 16:55          Napoli       Mungi
    - Si aggiungono keyword CVS e breve storia


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.1        2002/05/17 08:57:46     accardol
    - Inserimento in CVS della versione in lib07n
    Revision 1.1.1.1    2002/05/20 08:57:46     accardol
    - Inserimento in CVS in branch STP della versione in lib07n
01.00	2002/mag/17,ven             Napoli              Busco
        - Creazione fatta il 2001/lug/23,lun
*/

