/****************************************************************/
/* 2004/nov/09,mar 15:00                \SWISV\LIB\SRC\msf.c    */
/****************************************************************/
/*      Implementazione di macchine a stati finiti              */
/****************************************************************/
/*
    $Date: 2004/11/09 14:31:15 $          $Revision: 1.3 $
    $Author: mungiguerrav $
*/


#include "stdlib.h"
#include "msf.h"

static char msf_errcode;    /* Codice di errore */

/* Tabella dei messaggi di errore */
char *msf_errmsg[] =
{
/* 0 */  "OK",
/* 1 */  "Mem. insuff.",
/* 2 */  "Stato duplic.",
/* 3 */  "Stato inesist."
};


static p_MSF_STATO trova_stato(p_MSF msf, char codice);

static unsigned short memdisp;
static char *pt_fine;


void msf_init(char *mem_msf,unsigned short mem_msf_size)
{
    /* inizializza i puntatori al buffer ed il codice di errore */
    memdisp = mem_msf_size;
    pt_fine = mem_msf;
    msf_errcode = 0;
}

void *msf_malloc(unsigned short size)
{
    char *pt_temp;

    if(memdisp >= size)
    {
        memdisp -= size;
        pt_temp = pt_fine;
        pt_fine += size;
        return(pt_temp);
    }
    else
    {
        /* segnala memoria insufficiente */
        msf_errcode = 1;
        return(NULL);
    }
}

/******************************************************************/
/* allocazione ed inizializzazione di una macchina a stati finiti */
/******************************************************************/
p_MSF msf_new()
{
    p_MSF pt;

    if(msf_errcode == 0)
    {
        if((pt = msf_malloc(sizeof(struct MSF))) != NULL)
        {
            pt->stato_iniz  = NULL;
            pt->stato_corr  = NULL;
            pt->primo_stato = NULL;
        }
        return(pt);
    }
    else
        return(NULL);
}

/**********************************************************/
/* allocazione ed aggiunta ad una macchina a stati finiti */
/* di un nuovo stato                                      */
/**********************************************************/
p_MSF_STATO msf_addstate(p_MSF msf, char cod, short iniziale, void (*st_fun)())
{
    p_MSF_STATO pt, pt_temp;

    if(msf_errcode == 0)
    {
        if((pt = msf_malloc(sizeof(struct MSF_STATO))) != NULL)
        {
            if(trova_stato(msf,cod) == NULL)
            {
                if(msf->primo_stato == NULL)
                    /* aggiunge il primo stato alla MSF */
                    msf->primo_stato = pt;
                else
                {
                    /* si porta alla fine della lista degli stati */
                    pt_temp = msf->primo_stato;
                    while(pt_temp->altro_stato != NULL)
                        pt_temp = pt_temp->altro_stato;

                    /* pt_temp e' il puntatore all'ultimo stato della lista */

                    /* accoda il nuovo stato */
                    pt_temp->altro_stato = pt;
                }

                /* inizializza il nuovo stato */
                pt->codice      = cod;
                pt->altro_stato = NULL;
                pt->prima_trans = NULL;
                pt->stato_fun   = st_fun;

                /* se lo stato e' lo stato iniziale lo imposta nella MSF */
                if(iniziale)
                {
                    msf->stato_iniz = pt;
                    msf->stato_corr = pt;
                }
            }
            else
            {
                /* segnala stato duplicato */
                msf_errcode = 2;
                pt = NULL;
            }
        }
        return(pt);
    }
    else
        return(NULL);
}

static p_MSF_STATO trova_stato(p_MSF msf, char codice)
{
    short cod_trovato = 0;
    p_MSF_STATO pt_stato = 0;

    if(msf->primo_stato != NULL)
    {
        pt_stato = msf->primo_stato;

        while((pt_stato->codice != codice) && (pt_stato->altro_stato != NULL))
            pt_stato = pt_stato->altro_stato;

        if(pt_stato->codice == codice)
            cod_trovato = 1;
    }

    if(cod_trovato)
        return(pt_stato);
    else
        return(NULL);
}

/*************************************************************/
/* allocazione ed aggiunta ad uno stato (specificato da cod) */
/* di una nuova transizione                                  */
/*************************************************************/
p_MSF_TRANS msf_addtrans(p_MSF msf, char cod, char st_pros, short (*cond)(), short nc_vera, void (*tr_fun)())
{
    p_MSF_TRANS pt, pt_temp;
    p_MSF_STATO pt_stato, pt_st_pr;

    if(msf_errcode == 0)
    {
        if((pt = msf_malloc(sizeof(struct MSF_TRANS))) != NULL) ;
        {
            pt_stato = trova_stato(msf,cod);
            pt_st_pr = trova_stato(msf,st_pros);

            if((pt_stato != NULL) && (pt_st_pr != NULL))
            {
                /* pt_stato e' il puntatore allo stato a cui si vuole */
                /* aggiungere la transizione                          */

                if(pt_stato->prima_trans == NULL)
                    /* aggiunge la prima transizione allo stato */
                    pt_stato->prima_trans = pt;
                else
                {
                    /* si porta alla fine della lista delle transizioni dello stato */
                    pt_temp = pt_stato->prima_trans;
                    while(pt_temp->altra_trans != NULL)
                        pt_temp = pt_temp->altra_trans;

                    /* pt_temp e' il puntatore all'ultima transizione dello stato */

                    /* accoda la nuova transizione */
                    pt_temp->altra_trans = pt;
                }

                /* inizializza la nuova transizione */
                pt->condiz      = cond;
                pt->nc_vera     = nc_vera;
                pt->st_prossimo = pt_st_pr;
                pt->altra_trans = NULL;
                pt->cont_nc     = 0;
                pt->trans_fun   = tr_fun;
            }
            else
            {
                /* segnala stato inesistente */
                msf_errcode = 3;
                pt = NULL;
            }
        }
        return(pt);
    }
    else
        return(NULL);
}

/*************************************/
/* esecuzione della macchina a stati */
/*************************************/
short msf_run(p_MSF msf)
{
    p_MSF_TRANS temp_trans;
    short transitato = 0;

    if(msf_errcode == 0)
    {
        temp_trans = msf->stato_corr->prima_trans;

        while((temp_trans != NULL) && !transitato)
        {
            if(temp_trans->condiz())
            {
                temp_trans->cont_nc++;

                /* la condizione e' rimasta vera per nc_vera cicli */
                /* esegue la transizione                           */
                if(temp_trans->cont_nc >= temp_trans->nc_vera)
                {
                    /* se presente esegue una funzione */
                    /* associata alla transizione      */
                    if(temp_trans->trans_fun != NULL)
                        temp_trans->trans_fun();

                    /* imposta lo stato prossimo */
                    msf->stato_corr = temp_trans->st_prossimo;

                    /* azzera il contatore di num cicli a vero delle */
                    /* transizioni del nuovo stato                   */
                    {
                        p_MSF_TRANS   t_trans;

                        t_trans = msf->stato_corr->prima_trans;
                        while(t_trans != NULL)
                        {
                            t_trans->cont_nc = 0;
                            t_trans = t_trans->altra_trans;
                        }
                    }
                    /* segnala che ha effettuato la transizione */
                    transitato = 1;
                }
            }
            else
                /* azzera il contatore di num cicli a vero della var */
                temp_trans->cont_nc = 0;

            temp_trans = temp_trans->altra_trans;
        }

        if(msf->stato_corr->stato_fun != NULL)
            msf->stato_corr->stato_fun();
    }
    return(transitato);
}

/********************************/
/* reset della macchina a stati */
/********************************/
void msf_reset(p_MSF msf)
{
    p_MSF_STATO temp_stato;
    p_MSF_TRANS temp_trans;

    if(msf_errcode == 0)
    {
        /* imposta lo stato corrente allo stato iniziale */
        msf->stato_corr = msf->stato_iniz;

        /* azzera i contatori di tutte le transizioni */
        temp_stato = msf->stato_corr;
        while(temp_stato != NULL)
        {
            temp_trans = temp_stato->prima_trans;
            while(temp_trans != NULL)
            {
                temp_trans->cont_nc = 0;
                temp_trans = temp_trans->altra_trans;
            }
            temp_stato = temp_stato->altro_stato;
        }

        /* esegue la funzione di stato */
        if(msf->stato_corr->stato_fun != NULL)
            msf->stato_corr->stato_fun();
    }
}

/**********************************************/
/* restituisce il codice dello stato corrente */
/**********************************************/
char msf_stato(p_MSF msf)
{
    return(msf->stato_corr->codice);
}

/***********************************/
/* restituisce il codice di errore */
/***********************************/
char msf_errore(void)
{
    return(msf_errcode);
}

/************************************/
/* restituisce la stringa di errore */
/************************************/
char *msf_errstr(void)
{
    return(msf_errmsg[msf_errcode]);
}


/* ****************************************************************************
    $Log: msf.c,v $
    Revision 1.3  2004/11/09 14:31:15  mungiguerrav
    2004/nov/09,mar 15:00          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.2        2004/06/15 11:38:52     buscob
02.04   2003/nov/21,ven         Napoli                  Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso.

    Revision 1.1        2000/02/01 15:02:00     accardol
    - Inserimento in CVS della versione in lib07m
    Revision 1.1.1.1    2000/02/01 15:02:00     accardol
    - Inserimento in CVS in branch STP della versione in lib07m
        La data è arretrata con un artificio
02.03   2000/feb/01,mar             Napoli              D'Andria\Mungi
        - Si elim. warning di level 3, inizializzando pt_stato=0 in trova_stato(..)

 ------------------------   Prima di CVS    ------------------------
02.02   1997/mag/30,ven             Napoli              Busco
        - msf_run(): si elim. bug.
          Si usa un'altra var t_trans per inizializzare i contatori delle trans. del nuovo
          stato e non si altera quindi il valore di temp_trans che e' utilizzato dopo.
02.01   1997/mag/08,gio             Napoli              Busco
        - msf_run(): si sposta   return(transitato)
02.00   1996/dic/10,mar             Napoli              Busco
        - Si gestisce la segnalazione di errore tramite le funzioni
          msf_errore() e msf_errstr()
01.02   1996/lug/19,ven             Napoli              Busco
        - La funzione trova_stato() accetta un char come codice
        - Le funzioni add_state() e add_trans() accettano char come codici
01.01   1996/lug/08,lun             Napoli              Busco
        - Si agg. esecuzione della funzione di stato al reset
01.00   1996/lug/04,gio             Napoli              Busco
        - Creazione
*/

