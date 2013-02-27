/********************************************************************/
/* 2004/dic/15,mer 12:00                \SWISV\LIB\SRC\stream.c     */
/********************************************************************/
/*                  Gestione stream di dati in memoria              */
/********************************************************************/
/*
    $Date: 2004/12/15 11:53:46 $          $Revision: 1.7 $
    $Author: mungiguerrav $
*/


#include <string.h>
/* debug  #include <stdio.h>*/

#include "libisv.h"
#include "stream.h"
#include "consolle.h"

typedef short           (*tReadFunc )(char XHUGE *ptSrc, char XHUGE *ptDst, Ushort size);
typedef short           (*tWriteFunc)(char XHUGE *ptDst, char XHUGE *ptSrc, Ushort size);
typedef short           (*tEraseFunc)(char XHUGE *ptDst, Ushort sector);

typedef char XHUGE      *ptCluster;


/* Struttura contenente le informazioni su di un device */
struct streamDev
{
    char        name[STREAM_NAMEMAXLENGTH+1];
    tReadFunc   readFunc;
    tWriteFunc  writeFunc;
    tEraseFunc  eraseFunc;
    Ulong       sectorSize;
};

/* Struttura contenente le informazioni su di uno stream */
struct stream
{
    char        name[STREAM_NAMEMAXLENGTH+1];
    ptCluster   start;
    Ulong       size;
    Ushort      clusterSize;
    Ushort      device;
    Ushort      mode;
    ptCluster   ptRead;
    ptCluster   ptFirst;
    ptCluster   ptLast;
    Ushort      numSector;
    Ushort      RecCount;                           // Numero di record nello stream
    char        SectorErased;                       // Flag che indica che e' stato cancellato un settore e che quindi
                                                    // si deve ricalcolare il numero di records
    char        idString[STREAM_IDSTRINGMAXSIZE+1];
};

/* struttura di intestazione per i cluster */
struct  clusterHeader
{
    char        index;
    char        nCluster;
    char        usedByte;
    char        checksum;
};

static struct streamDev TabStreamDev[STREAM_MAXDEVICE+1];
static struct stream    TabStream[   STREAM_MAXSTREAM+1];
static Ushort nDevice;  /* Numero di device definiti */
static Ushort nStream;  /* Numero di stream definiti */

/*------ prototipi delle funzioni locali -----------------------*/
static  void        appendCluster(          hStream stream, void *buffer);
static  Ushort      sectorCluster(          hStream stream, ptCluster cluster);
static  Ushort      nextSector   (          hStream stream, Ushort sector);
static  Ushort      clusterIsFirstOfSector( hStream stream, ptCluster cluster);
static  Ushort      clusterIsEmpty(         hStream stream, ptCluster cluster);
static  ptCluster   nextCluster(            hStream stream, ptCluster cluster);

static  ptCluster   firstClusterOfSector(   hStream stream, Ushort sector);
static  void        readCluster(            hStream stream, ptCluster cluster, void *buffer);
static  void        writeCluster(           hStream stream, ptCluster cluster, void *buffer);
static  void        markAsFirstCluster(     hStream stream, ptCluster cluster);
static  Ushort      clusterIsFirstMarked(   hStream stream, ptCluster cluster);
static  void        createFirstMark(        hStream stream, char *buffer);
static  ptCluster   searchFirstMark(        hStream stream);
static  ptCluster   searchLastCluster(      hStream stream);
static  char        CalcChecksum(           void *buf,      unsigned short size);
static  void        UpdateRecCount(         hStream stream );


hStreamDev      stream_devicedef(   char            *name,
                                    short           (*readFunc)(),
                                    short           (*writeFunc)(),
                                    short           (*eraseFunc)(),
                                    unsigned long   (*SecSizeFunc)(void)
                                )
{
    if (nDevice >= STREAM_MAXDEVICE)
    {
        return (0);
    }
    else
    {
        nDevice++;

        strncpy(TabStreamDev[nDevice].name,name,STREAM_NAMEMAXLENGTH);

        TabStreamDev[nDevice].readFunc      = readFunc;
        TabStreamDev[nDevice].writeFunc     = writeFunc;
        TabStreamDev[nDevice].eraseFunc     = eraseFunc;
        TabStreamDev[nDevice].sectorSize    = SecSizeFunc();

        return(nDevice);
    }
}


hStream         stream_open(    char            *name,
                                void XHUGE      *start,
                                Ulong           size,
                                Ushort          clusterSize,
                                hStreamDev      device,
                                Ushort          mode
                                )
{
    if (nStream == STREAM_MAXSTREAM)
    {
        return (0);
    }
    else
    {
        nStream++;

        strncpy(TabStream[nStream].name,name,STREAM_NAMEMAXLENGTH);

        TabStream[nStream].idString[0]      = '\0';
        TabStream[nStream].start            = start;
        TabStream[nStream].size             = size;

        if (clusterSize >= STREAM_CLUSTERMINSIZE)
        {
            TabStream[nStream].clusterSize  = clusterSize;
        }
        else
        {
            TabStream[nStream].clusterSize  = STREAM_CLUSTERMINSIZE;
        }

        TabStream[nStream].device           = device;
        TabStream[nStream].mode             = mode;
        TabStream[nStream].numSector        = size/TabStreamDev[device].sectorSize;
        TabStream[nStream].ptFirst          = nextCluster(nStream,searchFirstMark(nStream));
        TabStream[nStream].ptRead           = TabStream[nStream].ptFirst;
        TabStream[nStream].ptLast           = searchLastCluster(nStream);

        TabStream[nStream].SectorErased     = 0;
        UpdateRecCount(nStream);

        /* Legge la IdString */
        stream_GetIdString(nStream);

        return(nStream);
    }
}


Ushort stream_append( hStream   stream,
                      void      *buffer,
                      Ushort    bufferSize )
{
    Ushort  i;
    Ushort  nCluster;
    Ushort  headSize;
    Ushort  dataSize;
    char    bufCluster[STREAM_CLUSTERMAXSIZE];
    struct clusterHeader    clHd;

    headSize    = sizeof(struct clusterHeader);
    dataSize    = TabStream[stream].clusterSize - headSize;

    /* Calcola il numero di cluster necessari a memorizzare il buffer */
    nCluster    = ((short)bufferSize - 1) / dataSize + 1;

    /* Memorizza tutti i cluster tranne l'ultimo */
    for (i = 0; i<nCluster-1; i++)
    {
        clHd.index      = i+1;
        clHd.nCluster   = nCluster;
        clHd.usedByte   = dataSize;
        clHd.checksum   = 0;

        memcpy(bufCluster, &clHd,              headSize);
        memcpy(bufCluster+(sizeof clHd),buffer,dataSize);

        /* Calcola ed aggiunge il checksum nel cluster */
        clHd.checksum   = -CalcChecksum(bufCluster,TabStream[stream].clusterSize);
        memcpy(bufCluster, &clHd, headSize);

        appendCluster(stream,bufCluster);

        buffer  = (char *)buffer + dataSize;
    }

    /* Memorizza l'ultimo cluster */
    dataSize        = bufferSize - (nCluster-1)*dataSize;

    clHd.index      = nCluster;
    clHd.nCluster   = nCluster;
    clHd.usedByte   = dataSize;
    clHd.checksum   = 0;

    memset(bufCluster, 0xFF, TabStream[stream].clusterSize);

    memcpy(bufCluster, &clHd,              headSize);
    memcpy(bufCluster+(sizeof clHd),buffer,dataSize);

    /* Calcola ed aggiunge il checksum nel cluster */
    clHd.checksum   = -CalcChecksum(bufCluster,TabStream[stream].clusterSize);

    memcpy(bufCluster, &clHd, headSize);

    appendCluster(stream,bufCluster);

    if (TabStream[stream].SectorErased)
    {
        /* E' stato cancellato un settore, ricalcola il numero di record riscandendo tutto lo stream */
        UpdateRecCount(stream);
    }
    else
    {
        /* Non si e' cancellato nessun settore, incrementa il numero di record */
        TabStream[stream].RecCount++;
    }
    TabStream[stream].SectorErased = 0;

    TabStream[stream].ptRead    = TabStream[stream].ptFirst;

    return ( 0 );
}


short          stream_read( hStream     stream,
                            void        *buffer,
                            Ushort      bufferSize      )
{
    Ushort                  nCluster;
    Ushort                  headSize;
    Ushort                  dataSize;
    char                    bufCluster[STREAM_CLUSTERMAXSIZE];
    struct clusterHeader    *ptClHd;
    Ushort                  clusterIndex;
    Ushort                  clusterFirst;
    Ushort                  clusterValid;
    Ushort                  clusterNull;
    Ushort                  clusterLast;
    Ushort                  inRecord;
    Ushort                  readByte;
    Ushort                  endOfStream;
    Ushort                  endOfBuffer;
    ptCluster               pt_temp;
    char                    *ptBufRecord;

    headSize        = sizeof(struct clusterHeader);
    dataSize        = TabStream[stream].clusterSize - headSize;

    pt_temp         = TabStream[stream].ptRead;
    ptClHd          = (struct clusterHeader *) bufCluster;

    ptBufRecord     = buffer;
    readByte        = 0;
    clusterIndex    = 0;
    clusterLast     = 0;
    inRecord        = 0;
    nCluster        = 0;

    endOfStream     = (pt_temp == TabStream[stream].ptLast );
    endOfBuffer     = 0;

    while( !clusterLast &&  !endOfStream && !endOfBuffer)
    {
        readCluster(stream, pt_temp, bufCluster);

            /* Controlla se il cluster e' il primo del record */
            clusterFirst =
                ( ( ptClHd->index   == 1 )
                  &&
                  ( ptClHd->nCluster > 0 )
                  &&
                  ( ptClHd->usedByte <= dataSize)
                );

            /* Controlla se il cluster e' valido */
            clusterValid =
                (CalcChecksum(bufCluster,TabStream[stream].clusterSize) == 0)
                &&
                ( clusterFirst
                  ||
                  ( ( ptClHd->index    == clusterIndex )
                    &&
                    ( ptClHd->nCluster == nCluster)
                    &&
                    ( ptClHd->usedByte <= dataSize)
                  )
                );

            /* Controlla se il cluster e' stato annullato */
            clusterNull =
                ( ( ptClHd->index    == 0 )
                  &&
                  ( ptClHd->nCluster == 0 )
                );

            /* Controlla se il cluster e' l'ultimo del record */
            clusterLast  =
                ( clusterValid
                  &&
                  ( ptClHd->index == ptClHd->nCluster )
                );

            {

            if ( clusterFirst)
            {
                ptBufRecord     = buffer;
                clusterIndex    = 1;
                inRecord        = 1;
                readByte        = 0;
                nCluster        = ptClHd->nCluster;
            }

            if ( clusterValid && inRecord)
            {
                if ( (bufferSize - readByte) >= ptClHd->usedByte)
                {
                    memcpy(ptBufRecord,
                           bufCluster+headSize,
                           ptClHd->usedByte);

                    ptBufRecord += ptClHd->usedByte;
                    readByte    += ptClHd->usedByte;
                    clusterIndex++;
                    endOfBuffer = 0;
                }
                else
                {
                    endOfBuffer = 1;
                }
            }
            else if ( !clusterNull )
            {
                inRecord    = 0;
            }

            pt_temp         = nextCluster(stream, pt_temp);
            if ( clusterIsFirstOfSector(stream,pt_temp) )
            {
                pt_temp = nextCluster(stream, pt_temp);
            }

            endOfStream     = (pt_temp == TabStream[stream].ptLast );
        }
    }

    TabStream[stream].ptRead =pt_temp;

    if ( endOfBuffer )
    {
        return ( -2);
    }
    else if ( clusterLast )
    {
        return ( readByte );
    }
    else
    {
        return ( -1);
    }
}


Ushort  stream_seek( hStream stream, Ulong nRecord )
{
    short i;
    char  buf[0x10];

    /* Si porta all'inizio dello Stream */
    TabStream[stream].ptRead = TabStream[stream].ptFirst;
    if (nRecord > 1)
    {
        for (i=1; i<nRecord; i++)
        {
            stream_read(stream, buf, 0x10);
        }
    }

    return(0);
}


Ushort          stream_lock(    hStream         stream)
{
    stream=stream;
    return(0);
}


Ushort          stream_unlock(  hStream         stream)
{
    stream=stream;
    return(0);
}


Ushort stream_erase( hStream stream )
{
    Ushort      i;
    hStreamDev  dev;

    dev = TabStream[stream].device;

    for (i=0; i<TabStream[stream].numSector; i++)
    {
        TabStreamDev[dev].eraseFunc(TabStream[stream].start, i);
    }

    markAsFirstCluster(stream, TabStream[stream].start);

    TabStream[stream].ptFirst = nextCluster(stream,TabStream[stream].start);
    TabStream[stream].ptRead  = TabStream[stream].ptFirst;
    TabStream[stream].ptLast  = searchLastCluster(stream);

    UpdateRecCount(stream);
    return ( 0 );
}

static void UpdateRecCount( hStream stream )
{
    static char  buf[0x10];
    short nbyte;
    unsigned long nrec;

    /* Si porta sul primo record */
    stream_seek(stream,1);

    nbyte = stream_read(stream, buf, 0x10);
    nrec  = 0;
    while(nbyte != -1)
    {
        nrec++;
        nbyte = stream_read(stream, buf, 0x10);
    }
    TabStream[stream].RecCount = nrec;
}

Ulong stream_recCount( hStream stream )
{
    return(TabStream[stream].RecCount);
}

Ulong stream_ptFirst( hStream stream )
{
    return((Ulong)TabStream[stream].ptFirst);
}

Ulong stream_ptLast( hStream stream )
{
    return((Ulong)TabStream[stream].ptLast);
}

Ulong stream_ptRead( hStream stream )
{
    return((Ulong)TabStream[stream].ptRead);
}

Ulong stream_ptStart( hStream stream )
{
    return((Ulong)TabStream[stream].start);
}

Ulong stream_size( hStream stream )
{
    return((Ulong)TabStream[stream].size);
}

char *stream_streamName( hStream stream )
{
    return(TabStream[stream].name);
}

char *stream_deviceName( hStreamDev device )
{
    return(TabStreamDev[device].name);
}

unsigned short  stream_device( hStream stream )
{
    return(TabStream[stream].device);
}

unsigned short  stream_nstream( void )
{
    return(nStream);
}

unsigned short  stream_ndevice( void )
{
    return(nStream);
}

Ulong stream_byteCount( hStream stream )
{
    long nbyte;

    nbyte = (long)(TabStream[stream].ptLast) -
            (long)(TabStream[stream].ptFirst);

    if(nbyte < 0)
        nbyte += TabStream[stream].size;

    return(nbyte);
}

char *stream_GetIdString( hStream stream )
{
    char        bufClus[STREAM_CLUSTERMAXSIZE];
    ptCluster   p_clust;

    /* Cerca il cluster con il marcatore di Start */
    p_clust = searchFirstMark( stream );

    /* Lo legge nel buffer */
    readCluster(stream, p_clust, bufClus);

    /* Azzera la idString */
    memset(TabStream[nStream].idString, 0x00, STREAM_IDSTRINGMAXSIZE+1);

    /* Copia la idString nel campo della struttura stream */
    strncpy(TabStream[stream].idString,
            &bufClus[TabStream[stream].clusterSize-STREAM_IDSTRINGMAXSIZE],
            STREAM_IDSTRINGMAXSIZE);

    return(TabStream[stream].idString);
}

/* Imposta una stringa di ID ed inizializza lo stream */
void stream_SetIdString( hStream stream, char *idString)
{
    /* Azzera la stringa */
    memset(TabStream[nStream].idString, 0x00, STREAM_IDSTRINGMAXSIZE+1);

    /* Copia la stringa passata */
    strncpy(TabStream[nStream].idString,idString,STREAM_IDSTRINGMAXSIZE);
    stream_erase( stream );
}

/*------------------------------------------------------*/
/*                      Funzioni locali                 */
/*------------------------------------------------------*/

static void     appendCluster( hStream stream, void *buffer)
{
    char XHUGE      *p_next;
    struct stream   *ptStream;
    hStreamDev      dev;

    /* Crea il puntatore alla struttura di definizione stream */
    ptStream    = &(TabStream[stream]);

    /* Trova il device a cui e' collegato lo stream */
    dev         = ptStream->device;

    /* Calcola il puntatore al prossimo cluster */
    p_next      = nextCluster(stream,ptStream->ptLast);

    /* Se il cluster e' l'ultimo di un sector, cancella il sector successivo */
    if ( clusterIsFirstOfSector(stream,p_next) )
    {
/* debug  printf("Cancellazione settore per scavalcamento %u\n",
                   sectorCluster(stream,p_next));
*/
        TabStreamDev[dev].eraseFunc( ptStream->start,
                                     sectorCluster(stream,p_next)   );

        p_next = nextCluster(stream,p_next);

        /* Segnala che si e' cancellato un settore e che si deve quindi ricalcolare il numero di records */
        TabStream[stream].SectorErased = 1;
    }

    /* Scrittura del cluster */
    TabStreamDev[dev].writeFunc( ptStream->ptLast,
                                 buffer,
                                 ptStream->clusterSize);

    ptStream->ptLast = p_next;

    /* Se i puntatori first e last coincidono significa che si devono       */
    /* cancellare logicamente i cluster di un sector e quindi si sposta     */
    /* il puntatore first all'inizio del sector successivo                  */
    if ( ptStream->ptFirst == ptStream->ptLast)
    {
        ptCluster p_temp;

        p_temp = firstClusterOfSector(stream,
                           nextSector(stream,
                        sectorCluster(stream,ptStream->ptFirst)));

        markAsFirstCluster(stream, p_temp);

        ptStream->ptFirst   = nextCluster(stream, p_temp);
    }
}


static Ushort   sectorCluster( hStream stream, ptCluster cluster)
{
    return
    (
        ( (long)cluster - (long)TabStream[stream].start )
        /
        TabStreamDev[TabStream[stream].device].sectorSize

    );
}


static Ushort   nextSector( hStream stream, Ushort sector)
{
    sector++;

    if (sector>=TabStream[stream].numSector)
        sector = 0;

    return ( sector );
}


static Ushort   clusterIsFirstOfSector( hStream stream, ptCluster cluster)
{
    Ushort num_settore;

    num_settore     = sectorCluster(stream, cluster);
    return
    (
        ( (long)cluster - (long)TabStream[stream].start )
        ==
        TabStreamDev[TabStream[stream].device].sectorSize * num_settore
    );
}


static Ushort   clusterIsEmpty( hStream stream, ptCluster cluster)
{
    char        bufClus[STREAM_CLUSTERMAXSIZE];
    char XHUGE  *pTemp;
    short       i;

    readCluster(stream, cluster, bufClus);

    pTemp   = bufClus;
    i       = 0;

    while ((*pTemp==(char)0xFF) && (i<TabStream[stream].clusterSize))
    {
        i++;
        pTemp++;
    }

    return( i==TabStream[stream].clusterSize );
}


static ptCluster nextCluster(   hStream stream, ptCluster cluster)
{
    cluster += TabStream[stream].clusterSize;
    if ( cluster >= (TabStream[stream].start + TabStream[stream].size) )
        cluster = TabStream[stream].start;

    return( cluster );
}


/*
static ptCluster prevCluster(   hStream stream, ptCluster cluster)
{
    cluster -= TabStream[stream].clusterSize;
    if ( cluster < TabStream[stream].start)
        cluster = TabStream[stream].start + TabStream[stream].size + 1L -
                 TabStream[stream].clusterSize;

    return( cluster );
}
*/

static ptCluster firstClusterOfSector( hStream stream, Ushort sector)
{
    return
    ( (ptCluster)
        ( (long)TabStream[stream].start
        +
        TabStreamDev[TabStream[stream].device].sectorSize * sector )
    );
}



static void     readCluster( hStream stream, ptCluster cluster, void* buffer)
{
    TabStreamDev[TabStream[stream].device].readFunc(
                                            cluster,
                                            buffer,
                                            TabStream[stream].clusterSize);
}


static void     writeCluster( hStream stream, ptCluster cluster, void* buffer)
{
    TabStreamDev[TabStream[stream].device].writeFunc(
                                            cluster,
                                            buffer,
                                            TabStream[stream].clusterSize);
}


static void     markAsFirstCluster( hStream stream, ptCluster cluster)
{
    char    buff[STREAM_CLUSTERMAXSIZE];

    createFirstMark(stream, buff);

    writeCluster(stream, cluster, buff);
}


static Ushort   clusterIsFirstMarked( hStream stream, ptCluster cluster)
{
    char    bufMark[STREAM_CLUSTERMAXSIZE];
    char    bufClus[STREAM_CLUSTERMAXSIZE];

    createFirstMark(stream, bufMark);

    readCluster(stream, cluster, bufClus);

//    return( memcmp(bufMark,bufClus,TabStream[stream].clusterSize) == 0 );
    return( memcmp(bufMark,bufClus,TabStream[stream].clusterSize-STREAM_IDSTRINGMAXSIZE) == 0 );
}


/* Riempie il buffer assegnato con la sequenza di codici da usare come */
/* marcatore di primo settore.                                         */
static void     createFirstMark( hStream stream, char *buffer)
{
    Ushort  i;

    /* Azzera il buffer */
    memset(buffer, 0x00, TabStream[stream].clusterSize);

    /* Riempe la parte iniziale del cluster con i codici sequenziali */
    for (i=0;i<TabStream[stream].clusterSize-STREAM_IDSTRINGMAXSIZE;i++)
        buffer[i]=TabStream[stream].clusterSize - i;

    /* Copia la idString nella parte rimanente del buffer */
    strncpy(&buffer[TabStream[stream].clusterSize-STREAM_IDSTRINGMAXSIZE],TabStream[stream].idString,STREAM_IDSTRINGMAXSIZE);
}

static ptCluster searchFirstMark( hStream stream )
{
    Ushort      i, trovato;
    ptCluster   p_temp, p_result;

    trovato     = 0;
    p_result    = NULL;

    for (i=0; i<TabStream[stream].numSector; i++)
    {
        p_temp = firstClusterOfSector(stream, i);

        if ( clusterIsFirstMarked( stream, p_temp) )
        {
            trovato     = 1;
            p_result    = p_temp;
        }
    }

    if (!trovato)
    {
        p_result    = TabStream[stream].start;

        stream_erase(stream);
    }

    return ( p_result );
}

static ptCluster searchLastCluster( hStream stream )
{
    ptCluster   p_temp;
    Ushort      numTestedSector;

    numTestedSector = 0;

    p_temp          = TabStream[stream].ptFirst;

    while ( (!clusterIsEmpty(stream, p_temp))
            &&
            (numTestedSector<(TabStream[stream].numSector*2-1) ) )
    {
        p_temp  = nextCluster( stream, p_temp);

        if ( clusterIsFirstOfSector(stream, p_temp) )
        {
            p_temp  = nextCluster( stream, p_temp);
            numTestedSector++;
        }
    }

    return ( p_temp );
}

static char CalcChecksum(void *buf, unsigned short size)
{
    unsigned char   RetVal;
    unsigned short  i;

    RetVal = 0;
    for (i=0; i<size; i++)
    {
        RetVal += ((unsigned char*)buf)[i];
    }
    return(RetVal);
}


/* ****************************************************************************
    $Log: stream.c,v $
    Revision 1.7  2004/12/15 11:53:46  mungiguerrav
    2004/dic/15,mer 12:00           Napoli      Mungi
    - Si elimina la definizione di XHUGE, spostata in stream.h per usarla in prototipo
    - Si reinseriscono da stream.h 3 delle 4 typedef di function, abolendo tSecSizeFunc:
      infatti nella struct streamDev TabStreamDev solo le prime 3 sono usate come tipo funzione,
      mentre la 4^ funzione è direttamente chiamata per riempire un campo Ulong
    - Nel prototipo di stream_devicedef(..) si indicano i tipi di ritorno delle 4 funzioni
      e l'argomento (void) solo della 4^ funzione per definire il prototipo prima della chiamata

    Revision 1.6  2004/12/13 13:10:29  mungiguerrav
    2004/dic/07,mar 11:40           Napoli      Mungi
    - Per rendere possibile la compilazione a scopo di debug anche in BorlandC,
      si aggiunge la definizione condizionata di XHUGE
    - stream_devicedef():  si utilizzano i typedef delle funzione da ricevere,
                           (i typedef sono spostati in  stream.h)
    - Si applicano le regole di codifica sulle parentesi

    Revision 1.5  2004/11/16 15:30:39  mungiguerrav
    2004/nov/15,lun 10:20          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.4        2004/08/31 14:25:15     mungiguerrav
    2004/ago/31,mar 16:25        Napoli          Mungi\Busco
    - Si effettua Merge da branch makefile

    Revision 1.3.2.1    2004/07/14 15:32:09     buscob
01.13   2004/apr/05,lun             Napoli              Mungi\Nappo
        - Per eliminare un warning BorlandC 3.0 si cambia il tipo del parametro 5 di
          stream_devicedef() in modo che sia noto il prototipo della funzione SecSizeFunc()
        - Si sposta l'operatore di indirezione *
            da      tipo*  nomeVar      (stile Busco)
            a       tipo   *nomeVar     (stile Resto del Mondo)
        - Si gestisce in maniera diversa l'incremento del puntatore (void *) buffer:
          invece di dichiararlo fittiziamente come (char *) nel parametro 2 di stream_append() e _read()
          si usa il cast (char *) nell'operazione effettiva:    buffer  = (char *)buffer + dataSize

    Revision 1.3        2004/06/15 11:55:24     buscob
01.12   2003/nov/21,ven             Napoli              Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso.

    Revision 1.2        2004/06/15 10:35:36     buscob
01.11   2003/nov/19,mer             Napoli              Busco
        - Si agg. gestione del numero di record come variabile in modo da non dover
          scorrere sempre tutto lo stream ogni volta che si vuole conoscerlo

    Revision 1.1        2002/05/20 14:10:54     accardol
    - Inserimento in CVS della versione in lib07h
    Revision 1.1.1.1    2002/05/20 14:10:54     accardol
    - Inserimento in CVS in branch STP della versione in lib07h
        La data è arretrata con un artificio
01.10   2002/mag/20,lun             Napoli              Mungi\Accardo
        - Si elim. typedef Ushort e Ulong, agg. include libisv.h

 ------------------------   Prima di CVS    ------------------------
01.09   2001/ago/27,lun             Napoli              Busco
        - Si agg. funzione stream_ptStart(...) che restituisce l'indirizzo di inizio
          dell'area di memoria riservata ad uno stream
01.08   2001/lug/30,lun             Napoli              Busco
        - Si agg. byte di checksum per il controllo del cluster
01.07   2001/lug/30,lun             Napoli              Busco
        - Si agg. typedef della funzione SecSizeFunc per consentire ai device driver
          di definire una funzione che restituisce l'ampiezza del settore fisico
        - stream_devicedef(...) accetta un puntatore alla funzione che restituisce
          l'ampiezza del settore invece che l'ampiezza direttamente
        - La funzione clusterIsEmpty(...) utilizza la funzione readCluster per accedere
          ai dati invece di accedervi direttamente
01.06   2001/lug/23,lun             Napoli              Busco
        - stream_open(...): - si mod. void* start in void xhuge * start per poter
                              essere compilato con modello SMALL
01.05   2000/ott/30,lun             Napoli              Busco
        - stream_open(...): - si limita dimensione minima del cluster a STREAM_CLUSTERMINSIZE
                            - si inizializza idString
                            - si agg. stream_GetIdString(nStream)
        - createFirstMark():- negli ultimi STREAM_IDSTRINGMAXSIZE del marcatore
                              si inserisce la idString
        - clusterIsFirstMarked():- si confrontano solo i primi caratteri
                                   escludendo quelli che contengono la idString
        - Si creano le funzioni stream_GetIdString() e stream_SetIdString()
01.04   2000/feb/05,sab             Napoli              Mungi
        - Si sost.  include consolle.h  con     include stdio.h
        - Si sost.  consolle_print(..)  con     printf(..)
01.03   2000/gen/28,ven             Napoli              Busco/Mungi
        - Si agg. inizializzazione a 0 di nCluster in stream_read(...)
        - Si agg. inizializzazione a NULL di p_result in searchFirstMark(...)
01.02   1999/dic/14,mar             Napoli              Busco/Mungi
        - Si agg. funzione stream_byteCount(...) che restituisce il numero di byte usati nello stream
        - Si agg. funzione stream_size(...) che restituisce la dimensione totale dello stream
01.01   1999/set/28,mar             Napoli              Busco
        - Si agg. funzioni stream_nstream(), stream_ndevice(), stream_device()
        - Si rendono le var. nDevice e nStream statiche di modulo
        - Si rinomina la funzione stream_device() preesistente  in stream_devicedef()
        - Si agg. la funzione stream_deviceName()
        - Si cambia nome da stream_name() a stream_streamName() e _deviceName()
        - Si agg. funzione stream_recCount()
        - Si scrive la funzione stream_seek()
01.00   1999/ago/30,lun             Napoli              Busco/Mungi
        - Creazione
*/

