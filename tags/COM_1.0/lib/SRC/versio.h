/****************************************************************/
/* 2004/ott/25,gio 15:10                \SWISV\LIB\SRC\versio.h */
/****************************************************************/
/*      Gestione versioni applicativi e librerie software       */
/****************************************************************/
/*
   $Date: 2004/10/25 13:16:53 $          $Revision: 1.3 $
   $Author: mungiguerrav $
*/

#ifndef VERSIO_INCLUDED
#define VERSIO_INCLUDED

#define MAXLENVRS       15
#define MAX_SW_VERS     10

struct versio
{
    unsigned char       stat[MAXLENVRS+1];
    unsigned char       nome[MAXLENVRS+1];
    unsigned char       vers[MAXLENVRS+1];
    unsigned char       date[MAXLENVRS+1];
    unsigned char       time[MAXLENVRS+1];
};

void versio_init(   unsigned char   *scheda,
                    unsigned short  nSW,
                    struct versio   *arVrs[] );

#endif

/* ****************************************************************************
   $Log: versio.h,v $
   Revision 1.3  2004/10/25 13:16:53  mungiguerrav
   2004/ott/25,lun 15:10          Napoli       Mungi
   - Si riporta sul tronco principale la modifica fondamentale effettuata nel
     branch didrv_nappoc: senza essa le schede della famiglia STx  e  DEx
     si bloccano

   Revision 1.2.2.1  2004/09/21 14:44:53  nappoc
   2004/09/21,lun 16:35    Napoli      Nappo
    - Si mod. MAXLENVRS da 14 a 15 per avere sempre l'accesso ad indirizzi di memoria pari, visto che
      la famiglia delle schede DEA, DEV, STA, STB non gestisce l'accesso ad indirizzi dispari.

   Revision 1.2  2004/08/31 14:25:16  mungiguerrav
   2004/ago/31,mar 16:25        Napoli          Mungi\Busco
   - Si effettua Merge da branch makefile

   Revision 1.1.1.1.2.1  2004/07/22 12:13:59  buscob
   2004/lug/22.gio 14:04      Napoli       Busco/Mungi
   - Si agg. il campo stat per accogliere elaborazioni di checkmod


    --- PRIMA DI CVS -----
1.01    2000.ott.15,dom         Copenaghen              Mungi
        - Si gestisce l'inclusione multipla
        - Si aumenta MAX_SW_VERS    da  8   a   10
1.00    Lun.21.feb.2000         Napoli                  Mungi
        - Creazione con prototipo:
                void versio_init(   unsigned char   *scheda,
                                    unsigned short  nSW,
                                    struct versio   *arVrs[] )
*/

