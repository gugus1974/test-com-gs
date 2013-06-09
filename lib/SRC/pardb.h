/****************************************************************************************/
/* 2004/nov/12,ven 17:10                                         \SWISV\LIB\SRC\pardb.h */
/*                                                                                      */
/* Module: PARDB                                                                        */
/* Description: Gestione dei parametri di applicativo                                   */
/*                                                                          HEADER FILE */
/****************************************************************************************/
/*
    $Date: 2004/11/12 16:42:46 $          $Revision: 1.2 $
    $Author: mungiguerrav $
*/


#ifndef PARDB_INCLUDED
#define PARDB_INCLUDED

/****************************************************************************************/
/* Include files                                                                        */
/****************************************************************************************/
#include "stream.h"

/****************************************************************************************/
/* Symbol definitions                                                                   */
/****************************************************************************************/

#define PARDB_TYPECHAR          1   /* Codici di tipo parametro */
#define PARDB_TYPEUCHAR         2
#define PARDB_TYPEXCHAR         3
#define PARDB_TYPESHORT         4
#define PARDB_TYPEUSHORT        5
#define PARDB_TYPEXSHORT        6
#define PARDB_TYPELONG          7
#define PARDB_TYPEULONG         8
#define PARDB_TYPEXLONG         9
#define PARDB_TYPEFLOAT         10
#define PARDB_TYPESTRING        11

#define PARDB_NAMEMAXLEN        10  /* Lunghezza massima del nome di un parametro */

#define PARDB_VALUEMAXLEN       16  /* Lunghezza massima del valore di un parametro */

#define PARDB_NOERR             0   /* Codici di errore */
#define PARDB_ERRBADNAME        1
#define PARDB_ERRDUPNAME        2
#define PARDB_ERRBADTYPE        3
#define PARDB_ERROUTOFMEM       4
#define PARDB_ERRVALIDATE       5
#define PARDB_ERRWRITE          6
#define PARDB_ERRUNKNOWNNAME    7
#define PARDB_ERRNOTFOUND       8
#define PARDB_ERRBADGROUP       9
#define PARDB_ERRDATAMISMATCH   10
#define PARDB_ERRBADSIZE        11
#define PARDB_ERRSTREAMERASE    12
#define PARDB_ERRSTREAMAPPEND   13


/****************************************************************************************/
/* Type definitions                                                                     */
/****************************************************************************************/

/* Serve all'applicativo per dichiarare le funzioni di validazione. */
typedef short (*pardb_tPfValidate ) (void* ParamValue);

/* Serve all'applicativo per dichiarare la funzione che restituisce la descrizione  */
/* di una classe di parametri.                                                      */
typedef  char* (*pardb_tPfClassDescr ) (char ClassCode);

/****************************************************************************************/
/* Interface variables                                                                  */
/****************************************************************************************/

/****************************************************************************************/
/* Functions prototypes                                                                 */
/****************************************************************************************/
short           pardb_init(     hStream                 stream,
                                char                    *BufDb,
                                unsigned short          BufSize,
                                pardb_tPfClassDescr     ClassDescr);

short           pardb_addpar(   char                    *name,
                                void                    *pVar,
                                char                    size,
                                pardb_tPfValidate       validate,
                                char                    ParType,
                                unsigned char           ParGroup);

short           pardb_write(    void);

short           pardb_read(     char                    *ParName);

unsigned short  pardb_error(    void);

unsigned short  pardb_NParam(   void);

short           pardb_SetParam( char                    *ParName,
                                char                    *ParValue);

void            pardb_ResetToDefault(   void);

#endif


/* ****************************************************************************
    $Log: pardb.h,v $
    Revision 1.2  2004/11/12 16:42:46  mungiguerrav
    2004/nov/12,ven 17:10          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.1        2002/05/17 09:18:22     accardol
    - Inserimento in CVS della versione in lib07n
    Revision 1.1.1.1    2002/05/17 09:18:22     accardol
    - Inserimento in CVS in branch STP della versione in lib07n
        La data è arretrata con un artificio
01.03   2002/mag/17,ven             Napoli              Busco
        - Inserimento in libreria UFAP
        - Si corregge tipo da char a unsigned char per param. ParGroup di _addpar()

 ------------------------   Prima di CVS    ------------------------
01.02   2001/lug/31,mar             Napoli              Busco
        - Si elimina parametro DefVal nel prototipo della funzione pardb_addpar(...)
        - Si aggiunge PARDB_ERRBADGROUP
01.01   2001/lug/30,lun             Napoli              Busco
        - Si elimina PARDB_ERRCHECKSUM
01.00   2001/giu/04,lun             Napoli              Busco
        - Creazione
*/

