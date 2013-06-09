/********************************************************************/
/* 2004/nov/05,ven 17:10                \SWISV\LIB\SRC\consolle.h   */
/********************************************************************/
/*          Header file per il gestore dei comandi da terminale     */
/********************************************************************/
/*
    $Date: 2004/11/05 16:33:11 $          $Revision: 1.3 $
    $Author: mungiguerrav $
*/


void consolle_init(     void);

void consolle_print(    const char* format, ...);

void consolle_parsestr( char*       stringa,
                        short*      numToken,
                        char*       token[]);
long consolle_parsenum( const char* s);

void consolle(          void);
void consolle_prompt(   char*       promptIn);

void consolle_addcmd(   char*       nomeCmd,
                        char*       sintassiCmd,
                        char*       helpCmd,
                        void        (*fGest)());
void consolle_cmdon(    char*       nomeCmd);
void consolle_cmdoff(   char*       nomeCmd);
void consolle_comando(  char*       comando);


/* ****************************************************************************
    $Log: consolle.h,v $
    Revision 1.3  2004/11/05 16:33:11  mungiguerrav
    2004/nov/05,ven 17:10          Napoli       Mungi
    - Si aggiungono keyword CVS e monumentale storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.2        2004/06/15 11:17:25     buscob
02.06   2003.nov.29,sab             Madrid                  Mungi
        - Si elimina finalmente prototipo   consolle_addvar(..),
          abolito sin da    consolle.c-v.5.05-Sab.05.feb.2000-Napoli-Mungi
        - Si applicano le regole sulle parentesi graffe e sui nomi dal Manuale del SW
02.05   2003.nov.21,ven             Napoli                  Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso.

    Revision 1.1        2000/02/05 14:33:10     accardol
    - Inserimento in CVS della versione in lib07m
    Revision 1.1.1.1    2000/02/05 14:33:10     accardol
    - Inserimento in CVS in branch STP della versione in lib07m
02.04   2000/feb/05,sab             Napoli              Mungi
        - Si elim. include query.h a cascata

 ------------------------   Prima di CVS    ------------------------
02.03   2000/feb/02,mer             Napoli              Busco/Mungi
        - Si elim. i prototipi consolle_view()
02.02   2000/feb/01,mar             Napoli              Mungi
        - Si elim. i prototipi consolle_brk_test() e consolle_trace()
02.01   1999/dic/06,lun             Napoli              Mungi/Busco
        - Si elim. i prototipi consolle_query() e consolle_cercavar()
02.00   1999/dic/03,ven             Napoli              Busco/Mungi
        - Si elim. struct var_entry
        - Si elim. prot. consolle_addvar e _cercavar
        - Temporaneamente si reiseriscono _addvar e _cercavar per
          compatibilita' con applicativi attuali
01.10   1998/feb/21,sab             Napoli              Mungi/Busco
        - Si mod. prototipo consolle_addvar(..,void far *pvar,...)
01.09   1998/feb/11,mer             Napoli              Busco/Mungi
        - Si agg. prototipo consolle_query()
01.08   1997/mar/14,ven
        - Si agg. prototipi di consolle_parsestr e _parsenum;
        - Si elimina prot. di consolle_dac
01.07   1997/gen/28,mar
        - Si agg. prototipo di consolle_print()
01.06   1997/gen/02,gio
        - Si sposta struct var_entry in consolle.h
01.05   1996/ott/17,gio
        - Si sposta struct var_entry in consolle.c
        - Si elim. prototipo di consolle_var()
01.04   1996/lug/18,gio
        - Si agg. define BRKTRACE  e  BRKPROGRAMMA
01.03   1996/mar/07,gio
        - Cambia protot. da:  _init(int baud)  in  _init(void)
01.02   1995/ott/18,mer
        - Si agg. var. int tipo alla struct var_entry:
                   tipo = 0 --> variabile int
                   tipo = 1 --> variabile unsigned int
        - Si agg. var. int tipo al prototipo consolle_addvar(..)
01.01   1995/ott/17,mar
        - Si agg. comando dac
01.00   1995/set/27,mer - Creazione
*/
