/****************************************************************/
/* 2004/ott/28,gio 15:10                \SWISV\LIB\SRC\dodrv.h  */
/****************************************************************/
/*
    $Date: 2004/11/04 14:56:51 $          $Revision: 1.3 $
    $Author: mungiguerrav $
*/

void            dodrv_init(char *buff, unsigned short size);

short           dodrv_add(  unsigned short  doNumber,
                            void huge       *BaseRd,
                            void huge       *BaseWr,
                            const char      *HighName,
                            const char      *LowName);

unsigned short  dodrv_numDo(    void);


void            dodrv_test(     unsigned short doNumber);
void            dodrv_output(   unsigned short doNumber);
void            dodrv_input(    unsigned short doNumber);
short           dodrv_status(   unsigned short doNumber);
void            dodrv_reset(    unsigned short doNumber);
void            dodrv_led(      unsigned short doNumber, short on_off);
unsigned long   dodrv_read(     unsigned short doNumber);
void            dodrv_StatusPrint(unsigned short doNumber);

unsigned long   dodrv_Fail0(    unsigned short doNumber);
unsigned long   dodrv_Fail1(    unsigned short doNumber);
void            dodrv_SetChannelFailEnable( unsigned short doNumber, unsigned short ch, unsigned short enable);
short           dodrv_GetChannelFailEnable( unsigned short doNumber, unsigned short ch);

void            dodrv_ResetChannels(        unsigned short doNumber, unsigned long lReset);

void            dodrv_SetChannelCommand(    unsigned short doNumber, unsigned short ch, unsigned short cmd);
short           dodrv_GetChannelCommand(    unsigned short doNumber, unsigned short ch);
short           dodrv_GetChannelStatus(     unsigned short doNumber, unsigned short ch);
short           dodrv_GetChannelFailure(    unsigned short doNumber, unsigned short ch);
void            dodrv_SetChannelForcing(    unsigned short doNumber, unsigned short ch, unsigned short frc);
short           dodrv_GetChannelForcing(    unsigned short doNumber, unsigned short ch);


#define DODRV_STATUSVMEFAIL     0x8000
#define DODRV_STATUSSUPPLYFAIL  0x0001
#define DODRV_STATUSCHFAIL      0x0002


/* ****************************************************************************
    $Log: doDrv.h,v $
    Revision 1.3  2004/11/04 14:56:51  mungiguerrav
    2004/ott/28,gio 15:10          Napoli       Mungi\Nappo
    - In mancanza del file utilizzato da Calabrese e perso prima dell'inserimento in CVS,
      si riscrive l'adeguamento alla gestione di più schede DO:
        - Si aggiunge a quasi tutti i prototipi il parametro doNumber, che individua
          su quale schede intende applicarsi la routine
        - Si distingue l'operazione di  _init del modulo dall'operazione di  _add
          di una scheda DO da gestire
        - Nel prototipo di _add si aggiungono 2 parametri per 2 variabili
          in vardb  che mappano parte alta e bassa del buffer della relativa DO
        - Si agg. la routine _numDo, che restituisce il numero di DO registrate

    Revision 1.2  2004/10/25 10:12:33  mungiguerrav
    2004/ott/25,lun 12:00          Napoli       Mungi
    - Si aggiungono keyword CVS e storia passata


 ---------------    Fase transitoria tra *.mod e CVS    -------------
    Revision 1.1      2002/05/21 14:52:22   accardol
    - Inserimento in CVS
    Revision 1.1.1.1  2002/05/21 14:52:22   accardol
    - Inserimento in CVS in branch STP
02.07   2002/mag/21,mar         Napoli                  Busco\Mungi
        - Si sost. prot.    void  _SetFailEnable(unsigned long lFail)
                    con     void  _SetChannelFailEnable(Ushort ch, Ushort enable)
                    e       short _GetChannelFailEnable(unsigned short ch)

 ----------------    Prima di CVS    -----------------------------
02.06   2002/mag/20,lun         Napoli                  Busco
        - Nell'operazione di spostamento da AIACE\SRC a LIB\SRC, si
          inseriscono le modifiche 02.04bs e 02.05bs fatte da Busco fuori UFAP
02.05bs 2002/mar/06,mer         Napoli                  Busco
        - Si cambia int in short
        - Si rinominano le funzioni esportate per maggiore chiarezza
02.04bs 2002/feb/05,mar         Napoli                  Busco
        - Si agg. prot.   dodrv_ForceChannel(..)
        - Si agg. prot.   dodrv_GetChannelCmd(..)
        - Si agg. prot.   dodrv_GetChannelForcing(..)
        - Si agg. prot.   dodrv_Fail0(..) e _Fail1(..)
        - Si agg. prot.   dodrv_GetFail(..)
        - Si modificano codici di stato scheda
        - Si agg. prot.   dodrv_SetFailEnable(..)
        - Si agg. prot.   dodrv_ResetChannels(..)
        - Si sost. prot.  _StatusPrint(..) a  _errorprint(..)
        - Si sost. prot.  _status(void)    a  _error(..)
        - Si elim. prot.  dodrv_prz(..), dodrv_alarm(..);
02.03   2001/dic/20,gio         Napoli                  Mungi\Accardo
        - Si agg. void in prototipo void dodrv_errorprint(void);
        - Si incolonnano i parametri dei prototipi
02.02   2001/mag/10,gio         Napoli                  Accardo
        - Si sost.  dodrv_init(void far  *BaseRd,void far  *BaseWr)
          con       dodrv_init(void huge *BaseRd,void huge *BaseWr)
02.01   2001/apr/28,sab         Napoli                  Accardo
        - Si mod.      _init(void far *Base)
                    in _init(void far *BaseRd,void far*BaseWr);
02.00   2001/feb/28,mer         Napoli                  Accardo
        - Si sposta da \SWISV\TLRV\SRC in \SWISV\AIACE\SRC.
----------------------Storia precedente di TLRV-----------------------
02.00   2001/gen/22,lun         Napoli                  Busco
        - Si cambia nome da do.h a dodrv.h e quindi a tutte le funzioni esportate
        - Si elim. prototipi dodrv_rdbitbuffer() e dodrv_rdbitbuffer()
        - Si agg. prototipi dodrv_SetChannel(..) e dodrv_GetChannel(..)
01.07   2000/feb/07,lun - Si sposta il comando do nel nuovo modulo docmd.c
01.04   1998/nov/05,gio - Si agg. prototipo funzione do_read()
01.03   1998/lug/14,mar - Si agg. prot. funzione do_prz()
01.02   1998/mag/25,lun - Si agg. prototipi di do_alarm() e do_led()
01.01   1998/mar/30,lun - Si cambia il prototipo di do_input()
01.00   1997/ott/17,ven - Creazione a partire da DIDO.h
*/

