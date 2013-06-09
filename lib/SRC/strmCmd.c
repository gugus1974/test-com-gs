/********************************************************************/
/* 2004/nov/11,gio 09:25                \SWISV\LIB\SRC\strmCmd.c    */
/********************************************************************/
/*      Comandi per gestire gli stream dalla linea di comando       */
/********************************************************************/
/*
    $Date: 2004/11/11 09:53:52 $          $Revision: 1.3 $
    $Author: mungiguerrav $
*/


#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "consolle.h"
#include "stream.h"

#include "strmcmd.h"

/* prototipi delle funzioni locali */
static void gest_stream(        short argc, char* argv[]          );
static void printstreamdata(    hStream stream                  );
static void printstreamheader(  void                            );
static void printhexbuffer(     char *buf, unsigned short size  );


void strmcmd_init(void)
{
    consolle_addcmd("stream","[nStream  [init]|[ls [firstRec] [lastRec]] ]", "Gestione Stream dati", gest_stream);
}

void gest_stream(short argc, char* argv[])
{
    short i;

    /* Se non ci sono argomenti visualizza lo stato di tutti gli stream */
    if (argc == 1)
    {
        printstreamheader();
        for (i=1; i<=stream_nstream(); i++)
        {
            printstreamdata(i);
        }
    }

    /* Se c'e' un solo argomento specificato esso costituisce il numero */
    /* dello stream di cui si vogliono le informazioni                  */
    if (argc == 2)
    {
        i = atoi(argv[1]);
        if ((i > 0) && (i <= stream_nstream()))
        {
            printstreamheader();
            printstreamdata(i);
        }
        else
            consolle_print("Stream %d not found\n",i);
    }



    /* Se ci sono almeno due argomenti il primo deve essere un numero */
    /* di stream valido                                               */
    if (argc >= 3)
    {
        unsigned short stream;

        stream = atoi(argv[1]);
        if ((stream > 0) && (stream <= stream_nstream()))
        {
            /* Visualizza i record selezionati */
            if (strcmp(argv[2],"ls") == 0)
            {
                static char  buf[0x1000];
                short nbyte;
                long first_record;
                long last_record;
                unsigned long nrec_printed;


                first_record = 1;
                if(argc>3)
                {
                    first_record = atol(argv[3]);
                }

                if(first_record<1)
                    first_record = 1;

                last_record = 0x7FFFFFFFL;
                if(argc>4)
                {
                    last_record = atol(argv[4]);
                }

                if(last_record<first_record)
                    last_record = first_record;

                /* Si porta sul primo record richiesto */
                stream_seek(stream,first_record);

                nbyte = stream_read(stream, buf, 0x1000);
                nrec_printed = 0;
                while(nbyte > 0 && ((nrec_printed+first_record) <= last_record))
                {
                    consolle_print("Rec. %lu\n",nrec_printed+first_record);
                    printhexbuffer(buf, nbyte);
                    nrec_printed++;
                    nbyte = stream_read(stream, buf, 0x1000);
                }
                consolle_print("End Of Stream %d\n",stream);
            }

            /* Inizializza lo stream */
            if (strcmp(argv[2],"init") == 0)
            {
                consolle_print("Initializing Stream %d...",stream);
                stream_erase(stream);
                consolle_print("\tOK\n");
            }

        }
        else
            consolle_print("Stream %d not found\n",stream);


    }

}

static void printstreamdata(hStream stream)
{
    consolle_print("%2u %-10s %-10s 0x%08lX 0x%08lX 0x%08lX 0x%08lX 0x%08lX %6lu\n",
                    stream,
                    stream_streamName(stream),
                    stream_deviceName(stream_device(stream)),
                    stream_ptStart(stream),
                    stream_size(stream),
                    stream_ptFirst(stream),
                    stream_ptLast(stream),
                    stream_ptRead(stream),
                    stream_recCount(stream)
                  );
}

static void printstreamheader(void)
{
    consolle_print("N. Name       Device     Start      Size       ptFirst    ptLast     ptRead       nRec\n");
}


static void printhexbuffer(char *buf, unsigned short size)
{
    short rig,col;

    if (size > 0)
    {
        #define NC 16
        #define NR ((size-1)/NC+1)
        for(rig=0; rig<NR; rig++)
        {
            consolle_print("%4u - ",rig);
            for(col=0; col<NC; col++)
            {
                if (rig*NC+col < size)
                    consolle_print("%02X ",(unsigned char) *(buf+rig*NC+col));
            }
            consolle_print("\n");
        }
        consolle_print("\n");
    }
}


/* ****************************************************************************
    $Log: strmCmd.c,v $
    Revision 1.3  2004/11/11 09:53:52  mungiguerrav
    2004/nov/11,gio 09:25          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.2        2004/06/15 11:56:15     buscob
01.04   2003/nov/21,ven             Napoli              Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso.

    Revision 1.1        2002/05/20 13:15:00     accardol
    - Inserimento in CVS della versione in lib07m
    Revision 1.1.1.1    2002/05/20 13:15:00     accardol
    - Inserimento in CVS in branch STP della versione in lib07m
        La data è arretrata con un artificio
01.03   2002/mag/20,lun             Napoli              Busco\Mungi
        - La v.01.03 entra ufficialmente in libreria UFAP

 ------------------------   Prima di CVS    ------------------------
01.02   2001/ago/27,lun             Napoli              Busco
        - Si elimina stringa "stream" dall'help
        - Con il comando "stream" si ottengono anche l'indirizzo di partenza
          e la dimensione degli stream
01.01   1999/dic/13,lun             Napoli              Busco/Mungi
        - Si agg. help al comando [stream ...]
01.00   1999/set/28,mar             Napoli              Busco
        - Creazione per agg. comando [stream]
*/

