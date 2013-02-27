/********************************************************************/
/* 2004/nov/30,mar 15:35                \SWISV\LIB\SRC\dp8570a.h    */
/********************************************************************/
/*
    $Date: 2004/12/13 11:51:55 $          $Revision: 1.3 $
    $Author: mungiguerrav $
*/


struct dp8570a_datetime
{
   unsigned char anno;
   unsigned char mese;
   unsigned char giorno;
   unsigned char ora;
   unsigned char minuti;
   unsigned char secondi;
};

typedef struct dp8570a_datetime *p_dp8570a_date_time;


short dp8570a_start(    void *base);
short dp8570a_startRead(void *base);
short dp8570a_stop(     void *base);
short dp8570a_clkRead(  void *base, p_dp8570a_date_time time);
short dp8570a_clkWrite( void *base, p_dp8570a_date_time time);
short dp8570a_memRead(  void *base, short offset, short *data);
short dp8570a_memWrite( void *base, short offset, short  data);
short dp8570a_lowbat(   void *base);
short dp8570a_tsEnable( void *base);
short dp8570a_tsDisable(void *base);
short dp8570a_tsRead(   void *base);


/* ****************************************************************************
    $Log: dp8570a.h,v $
    Revision 1.3  2004/12/13 11:51:55  mungiguerrav
    2004/nov/30,mar 15:35           Napoli      Mungi
    - Per adeguarsi a macro dp8570a(base,offset):
        - _memRead():   void *data      ==>    short *data
        - _memWrite():  char data       ==>    short data

    Revision 1.2  2004/11/08 10:08:56  mungiguerrav
    2004/nov/08,lun 11:05          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.1        2002/05/20 12:59:22     accardol
    - Inserimento in CVS della versione in lib07n
    Revision 1.1.1.1    2002/05/20 12:59:22     accardol
    - Inserimento in CVS in branch STP della versione in lib07n
01.02   2002/mag/20,lun             Napoli              Busco\Mungi
        - La v.01.01 entra ufficialmente in libreria UFAP

 ------------------------   Prima di CVS    ------------------------
01.01   2000/mag/30,mar             Napoli              Mungi
        - Si inserisce in \SWISV\LIB
        - Si mod. nomi prototipi sost. "_" con lettera Maiuscola
01.00   1999/lug/29,gio             Napoli              Busco
        - Creazione in TLRV\A\STAF di FINA
*/

