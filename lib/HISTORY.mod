2004/ago/31,mar 16:00              \SWISV\LIB\history.mod
****************************************************************
            Storia globale del modulo LIB
****************************************************************
$Date: 2005/01/26 13:05:41 $          $Revision: 9.1 $
$Author: mungiguerrav $

****************************************************************************/
$Log: HISTORY.mod,v $
Revision 9.1  2005/01/26 13:05:41  mungiguerrav
----------------------------------------------------------------------
        Release: rel_09a    2005/01/26   13:49:30
- L'unica modifica operativa riguarda l'introduzione dei driver per DI e DO che
  gestiscono più schede: occorre adeguare gli applicativi
- Altra modifca secondaria che richiede l'adeguamento degli applicativi è il cambio dei
  nomi delle public routine in nvramDev e ramDev
- I makefile gestiscono gli include mutlipli

- Per tutti i file si aggiungono keyword CVS e storia invertita

HCD\
    default.obc
        - Si cambia il test da  if NOT errorlevel 1
                            a   if "%risXX%"=="0"
          per compatibilta' tra win98 e win2000

SRC\
    - am29dev.obc, hw16x.obc, mdvDev.obc
        - Essendo impossibili le compilazioni per BorlandC e modello 167 medium,
          si impone la compilazione solo per modello 167 Large

    - default.obc
        - BugFix:   Su segnalazione di Formato e Cascone si scopre che in caso di
                    errore di compilazione Borland l'avviso di errore ingannava, in quanto
                    utilizzava %dObj% (e quindi OBJ7M) invece del corretto OBJL

    - dp_comm1.obc
            - Adegua il test di errore a Win2000        (if "%risXX%"=="0")
            - Migliora le informazioni fornite in caso di errore di compilazione

    - Per i file di origine Carnevale:  am29f040, dp_commX, sio, strparse, ushell:
        - Si sostituiscono i TAB con 4 spazi
        - Si applica regole di codifica per incolonnamento e parentesi
        - Non si applicano ancora regole di codifica per nomi routine e variabili

    - am29f040.c,.h
        - Per rendere possibile la compilazione a scopo di debug anche in BorlandC,
          si definisce XHUGE pari a huge nel caso non sia definito __C166__

    - diDrv.c,.h
        - Si modifica il drv della scheda DI per poter gestire più schede
        - Si modifica didrv_init passandogli puntatore al buffer per la gstione delle DI e dimensione
        - Si aggiunge didrv_add per aggiungere una scheda DI nel buffer
        - Si agg. 2 parametri nella funzione didrv_add per poter aggiungere due variabili in vardb
          che mappano parte alta e bassa del buffer della DI
        - Si corregge errore su doppio test su HighName: il secondo è su LowName

    - diDrvCmd.c,.h
        - Si adegua il modulo alle modifiche apportate a didrv su gestione piu' schede
        - Come fatto da Calabrese su dodrvcmd, si numerano le schede DI da 1 a N,
          invece che da 0 a N-1, come sono abituati a fare gli utenti sui veicoli

    - doDrv.c
        - Si agg. due variabili in vardb che mappano parte alta e bassa del buffer della DO
        - Introduzione DO multipla
        - Si corregge errore su doppio test su HighName: il secondo è su LowName
        - Si definisce localmente e non in dodrv.h la macro DELAY (2000 cicli)
        - Si rendono static le routine wrbitbuffer e rdbitbuffer:
            - Si elimina il +3 a nbit introdotto da Calabrese anche in queste
              routine di servizio, mentre il +3 serve solo nelle routine di interfaccia
              verso l'applicativo, il quale passa il canale 'ch' e non 'nbit'

    - doDrv.h
        - In mancanza del file utilizzato da Calabrese e perso prima dell'inserimento in CVS,
          si riscrive l'adeguamento alla gestione di più schede DO:
            - Si aggiunge a quasi tutti i prototipi il parametro doNumber, che individua
              su quale schede intende applicarsi la routine
            - Si distingue l'operazione di  _init del modulo dall'operazione di  _add
              di una scheda DO da gestire
            - Nel prototipo di _add si aggiungono 2 parametri per 2 variabili
              in vardb  che mappano parte alta e bassa del buffer della relativa DO
            - Si agg. la routine _numDo, che restituisce il numero di DO registrate

    - doDrvCmd.c,.h
        - Introduzione DO multipla

    - dp8570a.c,.h
        - Per rendere possibile la compilazione a scopo di debug anche in BorlandC,
          si aggiunge la definizione condizionata di XHUGE
        - Per adeguarsi a macro dp8570a(base,offset):
            - _memRead():   void *data      ==>    short *data
            - _memWrite():  char data       ==>    short data

    - nvRamDev.c,.h
        - Si cambia il nome del modulo da   nvramdev   a   nvramDev     (anche delle public routine)
        - Per rendere possibile la compilazione a scopo di debug anche in BorlandC,
          si utilizza la macro XHUGE, definita in nvramDev.h
        - Si aggiunge la macro di protezione _NVRAMDEV_

    - ramDev.c,.h
        - Si cambia il nome del modulo da   ramdev      a   ramDev     (anche delle public routine)
        - Si cambia nome routine pubblica   da  _sector_erase   a   _SectorErase

    - rtcCmd.c
        - Per adeguarsi al nuovo tipo del 3° parametro di dp8570a_memRead,
          si cambia    temp    da  unsigned char     a   short

    - stream.c
        - Per rendere possibile la compilazione a scopo di debug anche in BorlandC,
          si aggiunge la definizione condizionata di XHUGE
        - stream_devicedef():  si utilizzano i typedef delle funzione da ricevere,
                               (i typedef sono spostati in  stream.h)
        - Si applicano le regole di codifica sulle parentesi
        - Si elimina la definizione di XHUGE, spostata in stream.h per usarla in prototipo
        - Si reinseriscono da stream.h 3 delle 4 typedef di function, abolendo tSecSizeFunc:
          infatti nella struct streamDev TabStreamDev solo le prime 3 sono usate come tipo funzione,
          mentre la 4^ funzione è direttamente chiamata per riempire un campo Ulong
        - Nel prototipo di stream_devicedef(..) si indicano i tipi di ritorno delle 4 funzioni
          e l'argomento (void) solo della 4^ funzione per definire il prototipo prima della chiamata

    - stream.h
        - Si abolisce la macro condizionata xhuge (sostituita da XHUGE definita in stream.c)
        - Si agg. i typedef delle funzione da ricevere in stream_devicedef()
        - Si inserisce la definizione di XHUGE che era in stream.c perché usata
          anche in un prototipo e quindi necessaria ai file che includono stream.h
        - Si riportano in stream.c 3 delle 4 typedef di function (si abolisce tSecSizeFunc),
          in quanto utilizzate solo da stream.c: infatti nel prototipo di stream_devicedef(..)
          si indicano solo i tipi di ritorno delle 4 funzioni e non gli argomenti

    - versio.c
        - Si migliora la stampa delle stringhe di versione nel comando versio
        - Si incremento la spazio riservato alla stringa di Status da 4 a 8,
          per poter accogliere anche il nome del rilascio

    - versio.h
        - Si mod. MAXLENVRS da 14 a 15 per avere sempre l'accesso ad indirizzi di memoria pari, visto che
          la famiglia delle schede DEA, DEV, STA, STB non gestisce l'accesso ad indirizzi dispari.
          Senza questa modifica le schede della famiglia STx  e  DEx    si bloccano

 \
    - incfile
        - Si crea per permettere ai mekefile di gestire gli include a piu' livelli:
          in questo file sono create le variabili con gli include a cascade dei file
          header presenti nella dir SRC e HCD

    - makefile
        - Utilizzando i target multipli, si unificano le chiamate di compilazione per tutti gli
          oggetti derivati da un solo sorgente
        - Si agg.   include incfile di LIB,  necessario alla gestione degli include a piu' livelli
        - Nei prerequisiti dei target oggetto si aggiungono le variabili di tipo I_xxxx,  definite negli incfile
        - Per evitare messaggi di errore, nel target 'del' si aggiunge il test    if exist  DIR\*.xxx
----------------------------------------------------------------------

Revision 1.7  2004/08/31 14:53:39  mungiguerrav
2004/ago/31,mar 16:35        Napoli          Mungi\Busco
- REL_09
- Si effettua Merge da branch makefile
- Si inserisce file makefile
- Si aboliscono i file *.doc antichi e il tag *.flb è gestito internamente
- Si elimina gestione OBJ6 per scheda DAU
- Porzio/Calabrese correggono errore sull'inizio anni bisestili
  nell'algoritmo di timedate_cod
- Si elimina funzione fittizia di compatibilità query_addvar
- Si agg. parametro flags alla routine vardb_addvarbit
- Si sposta componente fixt da modulo lib a l16x
- Si cambia directory target per scheda PCA da OBJ7S a OBJ7M


****************************************************************
Per la storia precedente di LIB, vedi foto Lib07n del 2003/12/02,mar
