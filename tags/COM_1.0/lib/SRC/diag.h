/****************************************************************/
/* 2004/nov/08,lun 09:10                \SWISV\LIB\SRC\diag.h   */
/****************************************************************/
/*          Memorizzazione eventi diagnostici                   */
/****************************************************************/
/*
    $Date: 2004/11/08 09:21:25 $          $Revision: 1.5 $
    $Author: mungiguerrav $
*/


#define DIAG_EVENTDATAMAXSIZE    50 //Dimensioni massime della struttura
                                    //EventData per i dati da memorizzare
                                    //una sola volta per evento

typedef void  (*tPrintSampleFunc )  (char *ptSample);
typedef void  (*tPrintEventDataFunc)(char *ptSample);
typedef void  (*tPrintVarNamesFunc )(void);
typedef void  (*tVarNamesStringFunc )(char *buffer, short BufferSize);  //Restituisce una stringa con i nomi delle
                                                                        //variabili presenti nei record diagnostici
typedef void  (*tSampleStringFunc )(char *ptSample, char *buffer, short BufferSize); //Restituisce una stringa con i
                                                                        //valori dei campioni delle variabili
typedef char* (*tPrintCodDescrFunc )(unsigned short DiagCode);
typedef short (*tPrintCodEnabledFunc )(unsigned short DiagCode);
typedef void  (*tOnNewPacketWrittenFunc)(char *pt);

void diag_init( tPrintSampleFunc     psf,
                tPrintEventDataFunc  pedf,
                tPrintVarNamesFunc   pvnf,
                tPrintCodDescrFunc   pcdf,
                tPrintCodEnabledFunc pcef,
                tVarNamesStringFunc  pvnsf,
                tSampleStringFunc    pssf,
                char  *SampleBuffer,
                char  *EventDataBuffer,
                char  *TotalBuffer,
                short SampleSize,
                short EventDataSize,
                short MaxSample,
                short Stream
              );

void diag_append( unsigned short diagCode,
                  unsigned short n_ante,
                  unsigned short n_post,
                  unsigned short year,
                  unsigned char  month,
                  unsigned char  day,
                  unsigned char  hour,
                  unsigned char  min,
                  unsigned char  sec,
                  unsigned char  csec
                );

void diag_updateSample(void);


/* ****************************************************************************
    $Log: diag.h,v $
    Revision 1.5  2004/11/08 09:21:25  mungiguerrav
    2004/nov/08,lun 09:10          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.4        2004/06/15 11:22:01     buscob
01.07   2003/nov/21,ven             Napoli              Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso.

    Revision 1.3        2004/06/15 10:34:05     buscob
01.06   2003/ott/20,dom             Napoli              Busco
        - Si cambia prototipo di diag_init
        - Si agg. tipi delle funzioni che deve definire l'applicativo per fornire
          i nomi delle variabili ed i valori ad ATRterm

    Revision 1.2        2003/02/12 10:27:49     accardol
01.05   2002/nov/04,lun             Napoli              Busco
        - Si cambia prototipo di diag_init
        - Si agg. DIAG_EVENTDATAMAXSIZE

    Revision 1.1        2001/07/25 22:15:00     accardol
    - Inserimento in CVS della versione in lib07c
    Revision 1.1.1.1    2001/07/25 22:15:00     accardol
    - Inserimento in CVS in branch STP della versione in lib07c
01.04   2001/lug/26,gio             Napoli              Accardo
        - Si corregge data del file.

 ------------------------   Prima di CVS    ------------------------
01.03   2000/lug/14,ven             Napoli              Schiavo
        - Si agg. parametro PrintEnabledDescrFunc in diag_init(..)
01.02   2000/mar/03,ven             Napoli              Busco/Schiavo
        - Si agg. parametro pcdf in diagno_init(...)
01.01   1999/dic/14,mar             Napoli              Busco/Mungi
        - _init(): Si cambia param. formale     da MaxAnteSample
                                                a  MaxSample
                   Si agg. parametro formale    char*  TotalBuffer
01.00   1999/set/29,mer             Napoli              Busco
        - Creazione
*/

