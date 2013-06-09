/****************************************************************************************/
/* 2004/nov/15,lun 09:20                                         \SWISV\LIB\SRC\query.h */
/*                                                                                      */
/* Module: QUERY                                                                        */
/* Description: Sistema per l'interrogazione del sistema                                */
/*                                                                          HEADER FILE */
/****************************************************************************************/
/*
    $Date: 2004/11/16 09:45:52 $          $Revision: 1.3 $
    $Author: mungiguerrav $
*/


#ifndef QUERY_INCLUDED
#define QUERY_INCLUDED

#define QUERY_STENABLED     100    /* Query abilitate  */
#define QUERY_STDISABLED    101    /* Query disabilitate  */

void    query_init(char *list, unsigned short size);
short   query_addcmd(char *nome, void (*f_gest)(char* arg));
char    query_filter(  char c);
void    query_enable(  void);
void    query_disable( void);
short   query_status(  void);
char*   query_replystr(void);
short   query_AppendReply(char *replystr);
void    query_ResetReplyStr(void);
void    query_setmode(short mode);

#endif


/* ****************************************************************************
    $Log: query.h,v $
    Revision 1.3  2004/11/16 09:45:52  mungiguerrav
    2004/nov/15,lun 09:20          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.2        2004/06/15 11:46:08     buscob
01.06   2003/nov/21,ven             Napoli              Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso

    Revision 1.1        2002/05/17 08:17:14     accardol
    - Inserimento in CVS della versione in lib07m
    Revision 1.1.1.1    2002/05/17 08:17:14     accardol
    - Inserimento in CVS in branch STP della versione in lib07m
        La data è arretrata con un artificio
01.05   2002/mag/17,ven             Napoli              Busco\Mungi
        - Si corregge versione alla Accardo

 ------------------------   Prima di CVS    ------------------------
01.04   2001/nov/28,mer             Napoli              Busco
        - Si sposta definizione tipo qcmd_entry in query.c
01.03   2000/nov/29,mer             Napoli              Busco/Oliviero
        - Si agg. prototipo query_init()
01.02   2000/lug/24,lun             Napoli              Busco
        - Si agg. prototipo di query_setmode(...) per specificare il modo di funzionamento della query
01.01   2000/gen/24,lun             Napoli              Busco/Balbi
        - Si eliminano i campi di formato nella struttura var_entry
01.00   1999/dic/03,ven             Napoli              Busco
        - Creazione
*/

