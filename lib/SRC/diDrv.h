/****************************************************************/
/* 2004/ott/22,ven 16:50                \SWISV\LIB\SRC\didrv.h  */
/****************************************************************/
/*
    $Date: 2004/10/22 15:11:14 $          $Revision: 1.2 $
    $Author: mungiguerrav $
*/


short           didrv_add(  unsigned short diNumber,    /* numero della scheda Di                       */
                            void huge *BaseRd,          /* registro di lettura                          */
                            void huge *BaseWr,          /* registro di scrittura                        */
                            const char *HighName,       /* nome della variabile con i 16 canali alti    */
                            const char *LowName);       /* nome della variabile con i 16 canali bassi   */

void            didrv_init(char *buff, unsigned short size);        /* inizializza il buffer per gestire le informazioni sulle schede di */
unsigned short  didrv_numDi(void);                                  /* restituisce il numero di schede di allocate */
void            didrv_input(unsigned short diNumber);
void            didrv_output(unsigned short diNumber);
void            didrv_test(unsigned short diNumber);
void            didrv_reset(unsigned short diNumber);
unsigned short  didrv_alarm(unsigned short diNumber);
void            didrv_led(unsigned short diNumber,short on_off);
unsigned long   didrv_read(unsigned short diNumber);

void            didrv_SetChannelForcing(unsigned short diNumber,unsigned short ch, unsigned short frc);
short           didrv_GetChannelInput(unsigned short diNumber,unsigned short ch);
short           didrv_GetChannelStatus(unsigned short diNumber,unsigned short ch);
short           didrv_GetChannelFailure(unsigned short diNumber,unsigned short ch);
short           didrv_GetChannelForcing(unsigned short diNumber,unsigned short ch);
void            didrv_StatusPrint(unsigned short diNumber);
unsigned short  didrv_status(unsigned short diNumber);

#define DIDRV_STATUSVMEFAIL     0x8000
#define DIDRV_STATUSSUPPLYFAIL  0x0001
#define DIDRV_STATUSCHFAIL1     0x0002
#define DIDRV_STATUSCHFAIL0     0x0004


/* ****************************************************************************
    $Log: diDrv.h,v $
    Revision 1.2  2004/10/22 15:11:14  mungiguerrav
    2004/ott/22,ven 16:50          Napoli       Mungi
    - Si riporta sul tronco principale la correzione effettuata in branch
      didrv_nappoc  su gestione più schede
    - Si aggiungono keyword CVS e storia passata


    Revision 1.1.1.1.4.2    2004/10/01 09:59:16     nappoc
    2003/set/29,mer 11:40    Napoli    Nappo
    - Si agg. 2 parametri nella funzione didrv_add per poter aggiungere due variabili in vardb
      che mappano parte alta e bassa del buffer della DI

    Revision 1.1.1.1.4.1    2004/09/07 07:33:29     nappoc
    2004/set/07         Napoli              Nappo
    - Si modificano i prototipi delle funzioni aggiungendo il parametro relativo al numero della scheda DI
    - Si modifica didrv_init passandogli puntatore al buffer per la gstione delle DI e dimensione
    - Si aggiunge didrv_add per aggiungere una scheda DI nel buffer


 ---------------    Fase transitoria tra *.mod e CVS    -------------
    Revision 1.1      2002/05/21 09:50:14   accardol
    - Inserimento in CVS
    Revision 1.1.1.1  2002/05/21 09:50:14   accardol
    - Inserimento in CVS in branch STP
02.06   2002.mag.21,mar         Napoli                  Busco\Mungi
        - Si distingue:
            DIDRV_STATUSCHFAIL1     0x0002
            DIDRV_STATUSCHFAIL0     0x0004
 ----------------    Prima di CVS    -----------------------------
02.05   2002.mag.20,lun         Napoli                  Busco
        - Si sposta da AIACE\SRC a LIB\SRC e si inserisce la 02.04bs
        - Si agg. prot. di funz. per forcing e lettura stato
02.04bs 2002.mar.06,mer         Napoli                  Busco
        - Si elim. prototipi di didrv_rdbitbuffer e _wr... perche' statiche
        - Si cambiano prototipi _error() in _status() e _errorprint() in _StatusPrint()
02.03   2001.mag.10,gio         Napoli                  Accardo
        - Si sost.  didrv_init(void far  *BaseRd,void far  *BaseWr)
          con       didrv_init(void huge *BaseRd,void huge *BaseWr)
02.02   2001.apr.28,sab         Napoli                  Accardo
        - Si mod.      _init(void far *Base)
                    in _init(void far *BaseRd,void far*BaseWr);
02.01   2001.feb.28,mer         Napoli                  Accardo
        - Si sposta da \SWISV\TLRV\SRC in \SWISV\AIACE\SRC.
----------------------Storia precedente di TLRV-----------------------
02.01   2001/gen/19,gio         Napoli                  Busco
        - Si agg. prototipo funzione didrv_errorstr(..)
02.00   2001/gen/04,gio         Napoli                  Busco
        - Si cambia il nome del file da di.h a didrv.h
01.03   1998/nov/05,gio - Si agg. prototipo di_read()
                        - Si mod prototipo di_input()
01.02   1998/mag/25,lun - Si agg. prototipi di di_alarm() e di_led()
                        - Si agg. prototipi di di_rdbitbuffer() e di_output()
01.01   1998/mar/30,lun - Si cambia il prototipo di di_input()
01.00   1997/ott/17,ven - Creazione a partire da DO.H
*/
