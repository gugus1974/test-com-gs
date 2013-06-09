/********************************************************************/
/* 2004/dic/15,mer 12:10                \SWISV\LIB\SRC\stream.h     */
/********************************************************************/
/*                  Gestione stream di dati in memoria              */
/********************************************************************/
/*
    $Date: 2004/12/15 11:43:23 $          $Revision: 1.5 $
    $Author: mungiguerrav $
*/


#ifndef STREAM_INCLUDED
#define STREAM_INCLUDED

/*==================================================================*/
/* Macros */
#ifdef __C166__
    #define XHUGE xhuge
#else
    #define XHUGE huge
#endif


#define STREAM_MAXDEVICE        5
#define STREAM_MAXSTREAM        5
#define STREAM_NAMEMAXLENGTH    10
#define STREAM_CLUSTERMAXSIZE   256
#define STREAM_CLUSTERMINSIZE   32
#define STREAM_IDSTRINGMAXSIZE  16


typedef unsigned short  hStreamDev;
typedef unsigned short  hStream;

hStreamDev      stream_devicedef(   char            *name,
                                    short           (*readFunc)(),
                                    short           (*writeFunc)(),
                                    short           (*eraseFunc)(),
                                    unsigned long   (*SecSizeFunc)()
                                );
hStream         stream_open(    char            *name,
                                void XHUGE      *start,
                                unsigned long   size,
                                unsigned short  clusterSize,
                                hStreamDev      device,
                                unsigned short  mode
                                );
unsigned short  stream_append(  hStream         stream,
                                void            *buffer,
                                unsigned short  bufferSize
                                );
short           stream_read(    hStream         stream,
                                void            *buffer,
                                unsigned short  bufferSize
                                );
unsigned short  stream_seek(    hStream         stream,
                                unsigned long   nRecord
                                );
unsigned short  stream_lock(    hStream         stream
                                );
unsigned short  stream_unlock(  hStream         stream
                                );
unsigned short  stream_erase(   hStream         stream
                                );
unsigned long   stream_ptFirst( hStream         stream
                                );
unsigned long   stream_ptLast(  hStream         stream
                                );
unsigned long   stream_ptRead(  hStream         stream
                                );
unsigned long   stream_ptStart( hStream         stream
                                );
unsigned long   stream_size(    hStream         stream
                                );
char            *stream_streamName(    hStream stream
                                );
char            *stream_deviceName(    hStreamDev device
                                );
unsigned short  stream_device(  hStream stream
                                );
unsigned short  stream_nstream( void
                                );
unsigned short  stream_ndevice( void
                                );
unsigned long   stream_recCount( hStream stream
                                );
unsigned long   stream_byteCount(hStream stream
                                );
char            *stream_GetIdString(hStream stream );
void            stream_SetIdString( hStream stream, char *idString);

#endif


/* ****************************************************************************
    $Log: stream.h,v $
    Revision 1.5  2004/12/15 11:43:23  mungiguerrav
    2004/dic/15,mer 12:10           Napoli      Mungi
    - Si inserisce la definizione di XHUGE che era in stream.c perché usata
      anche in un prototipo e quindi necessaria ai file che includono stream.h
    - Si riportano in stream.c 3 delle 4 typedef di function (si abolisce tSecSizeFunc),
      in quanto utilizzate solo da stream.c: infatti nel prototipo di stream_devicedef(..)
      si indicano solo i tipi di ritorno delle 4 funzioni e non gli argomenti

    Revision 1.4  2004/12/13 13:12:56  mungiguerrav
    2004/dic/07,mar 11:50           Napoli      Mungi
    - Si abolisce la macro condizionata xhuge (sostituita da XHUGE definita in stream.c)
    - Si agg. i typedef delle funzione da ricevere in stream_devicedef()

    Revision 1.3  2004/11/16 15:31:21  mungiguerrav
    2004/nov/15,lun 10:30          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.2        2004/08/31 14:25:15     mungiguerrav
    2004/ago/31,mar 16:25        Napoli          Mungi\Busco
    - Si effettua Merge da branch makefile

    Revision 1.1.1.1.2.1    2004/07/14 15:33:00     buscob
01.09   2004/apr/05,lun             Napoli              Mungi\Nappo
        - Per consentire la compilazione in BC30 per simulazione, si introduce il test
                #ifndef __C166__
                    #define xhuge huge
                #endif
        - Si sposta l'operatore di indirezione *
            da      tipo*  nomeVar      (stile Busco)
            a       tipo   *nomeVar     (stile Resto del Mondo)

    Revision 1.1        2002/05/20 14:10:34     accardol
    - Inserimento in CVS della versione in lib07n
    Revision 1.1.1.1    2002/05/20 14:10:34     accardol
    - Inserimento in CVS in branch STP della versione in lib07n
        La data è arretrata con un artificio
01.08   2002/mag/20,lun             Napoli              Busco\Mungi
        - Entra in libreria UFAP
        - stream_open(.):   Nell'ultimp parametro, si sost. Ulong sectorSize
                            con     Ulong  (*SecSizeFunc)()

 ------------------------   Prima di CVS    ------------------------
01.07   2001/ago/27,lun             Napoli              Busco
        - Si agg. prototipo stream_ptStart(...)
01.06   2001/lug/31,mar             Napoli              Busco
        - Si porta STREAM_CLUSTERMINSIZE da 64 a 32 e STREAM_IDSTRINGMAXSIZE da 32 a 16
          per poter avere cluster piu' piccoli
01.05   2001/lug/23,lun             Napoli              Busco
        - stream_open(...): - si mod. void* start in void xhuge * start per poter
                              essere compilato con modello SMALL
01.04   2001/apr/18,mer             Napoli                  Busco
        - Si agg. #ifdef STREAM_INCLUDED #define STREAM_INCLUDED
01.03   2000/ott/27,ven             Napoli              Busco
        - Si agg. define STREAM_CLUSTERMINSIZE e STREAM_IDSTRINGMAXSIZE che
          definisce la minima lunghezza del cluster.
          Il valore fornito nello stream_open(...) viene cimato con questo parametro.
          Serve per avere un cluster di dimensioni tali da ospitare la idString.
        - Si agg. prototipi stream_GetIdString() e stream_SetIdString()
01.02   1999/dic/14,mar             Napoli              Busco/Mungi
        - Si agg. prot. Ulong stream_byteCount(..)
        - Si agg. prot. Ulong stream_size(..)
01.01   1999/set/27,lun             Napoli              Busco
        - Si agg. prototipi delle funzioni stream_nstream(), stream_ndevice(), stream_device()
        - Si rinomina la funzione stream_device() preesistente in stream_devicedef()
01.00   1999/ago/30,lun             Napoli              Busco/Mungi
        - Creazione
*/

