/********************************************************************/
/* 2004/nov/09,mar 15:15                \SWISV\LIB\SRC\msf.h        */
/********************************************************************/
/*      Implementazione di macchine a stati finiti                  */
/********************************************************************/
/*
    $Date: 2004/11/09 14:31:45 $          $Revision: 1.4 $
    $Author: mungiguerrav $
*/


typedef struct MSF_TRANS *p_MSF_TRANS;
typedef struct MSF_STATO *p_MSF_STATO;
typedef struct MSF       *p_MSF;

struct MSF_TRANS
{
    short           (*condiz)(void);/* condizione logica da valutare           */
    unsigned short  nc_vera;       /* numero di cicli di logica per cui la    */
                                 /* condizione deve essere verificata prima */
                                 /* di diventare attiva                     */
    unsigned short  cont_nc;       /* contatore del numero di cicli           */
    p_MSF_STATO   st_prossimo;   /* stato di arrivo della transizione       */
    void          (*trans_fun)(void);/* funzione da eseguire nella transizione  */
    p_MSF_TRANS   altra_trans;   /* puntatore alla prossima transizione da  */
                                 /* valutare                                */
};

struct MSF_STATO
{
    unsigned char codice;        /* codice numerico dello stato                */
    void          (*stato_fun)(void);/* funzione da eseguire nello stato           */
    p_MSF_STATO   altro_stato;   /* puntatore al prossimo stato della lista    */
    p_MSF_TRANS   prima_trans;   /* puntatore alla prima transizione di uscita */
                                 /* dallo stato                                */
};

struct MSF
{
    p_MSF_STATO   stato_iniz;    /* stato iniziale della macchina a stati finiti */
    p_MSF_STATO   stato_corr;    /* stato corrente                               */
    p_MSF_STATO   primo_stato;   /* primo stato della lista stati                */

};


/* allocazione ed inizializzazione di una macchina a stati finiti */
p_MSF msf_new(void);

/* allocazione ed aggiunta ad una macchina a stati finiti */
/* di un nuovo stato                                      */
p_MSF_STATO msf_addstate(p_MSF msf, char cod, short iniziale, void (*st_fun)());


/* allocazione ed aggiunta ad uno stato (specificato da cod) */
/* di una nuova transizione                                  */
p_MSF_TRANS msf_addtrans(p_MSF msf, char cod, char st_pros, short (*cond)(), short nc_vera, void (*tr_fun)());


/* esecuzione della macchina a stati */
short msf_run(p_MSF msf);

/* reset della macchina a stati */
void msf_reset(p_MSF msf);

/* restituisce il codice dello stato corrente */
char msf_stato(p_MSF msf);

/* inizializzazione modulo MSF */
void msf_init(char *mem_msf, unsigned short mem_msf_size);

/* restituisce il codice di errore */
char msf_errore(void);

/* restituisce il codice di errore */
char *msf_errstr(void);


/* ****************************************************************************
    $Log: msf.h,v $
    Revision 1.4  2004/11/09 14:31:45  mungiguerrav
    2004/nov/09,mar 15:15          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.3        2004/08/31 14:25:15     mungiguerrav
    2004/ago/31,mar 16:25        Napoli          Mungi\Busco
    - Si effettua Merge da branch makefile

    Revision 1.2.2.1    2004/07/14 15:29:37     buscob
01.07   2003/dic/22,lun             Napoli                  Calabrese
        - Si aggiunge void al prototipo di msf_new

    Revision 1.2        2004/06/15 11:41:22     buscob
01.06   2003/nov/21,ven             Napoli                  Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso.

    Revision 1.1        2001/11/22 12:23:38     accardol
    - Inserimento in CVS della versione in lib07m
    Revision 1.1.1.1    2001/11/22 12:23:38     accardol
    - Inserimento in CVS in branch STP della versione in lib07m
        La data è arretrata con un artificio
01.05   2001/nov/22,gio             Napoli              Mungi
        - Si agg. i void negli argomenti dei puntatori a funzione nelle strutture:
                    int     (*condiz)(      void );
                    void    (*trans_fun)(   void );
                    void    (*stato_fun)(   void );

 ------------------------   Prima di CVS    ------------------------
01.04   1997/giu/02,mar             Napoli              Busco
        - Si cambiano i contatori delle transizioni da char a unsigned int
01.03   1996/dic/10,mar             Napoli              Busco
        - Si cambia prototipo msf_init(...)
        - Si agg. prototipo di msf_errore() e msf_errstr()
01.02   1996/lug/19,ven             Napoli              Busco
        - I typedef dei puntatori ritornano normali (near)
        - cont_nc e nc_vera nella struct MSF diventano char
        - Il codice dello stato diventa char
        - Si cambiano i prototipi delle funzioni per gestire lo stato char
01.01   1996/lug/11,gio             Napoli              Busco
        - I typedef dei puntatori diventano far
01.00   1996/lug/04,gio             Napoli              Busco
        - Creazione
*/

