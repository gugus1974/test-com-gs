/********************************************************************/
/* 2004/nov/12,ven 15:30                \SWISV\LIB\SRC\dp_comm2.c   */
/********************************************************************/
/*          Dual port RAM TCN communication: host                   */
/********************************************************************/
/*
    $Date: 2004/11/12 15:10:11 $          $Revision: 1.3 $
    $Author: mungiguerrav $
*/


/*==================================================================*/
/* Includes */
#include "dp_comm.h"

/*==================================================================*/
/* Pragmas */
#pragma WL(3)

/*==================================================================*/
/* Globals */
static DpCommCfg huge   *cfg;                   /* configuration list */

/*==================================================================*/
/* Private routines */

/*------------------------------------------------------------------*/
static void pi_copy16(  short huge      *target,
                        short huge      *source,
                        unsigned short  size)
{
    for (; size > 0; size--)
    {
        *target++ = *source++;
    }
}


/*==================================================================*/
/* Mailbox handler routines */

/*------------------------------------------------------------------*/
static void host_request_handler(   short   request_id,
                                    short   request_data)
{
    /* handle a dataset update done message */
    if (request_data & DP_COMM_UPDATE_DS_DONE_BIT)
    {
        dp_comm2_update_ds_done_handler();
    }

    /* handle a send done message */
    if (request_data & DP_COMM_SND_MSG_DONE_BIT)
    {
        dp_comm2_snd_msg_done_handler(cfg->snd_msg_done_desc->ref,
                                      cfg->snd_msg_done_desc->status);
    }

    /* handle a received message */
    if (request_data & DP_COMM_RCV_MSG_BIT)
    {
        dp_comm2_rcv_msg_handler(cfg->rcv_msg_desc->our_fct, &cfg->rcv_msg_desc->ext_addr,
                                 cfg->rcv_msg_desc + 1, cfg->rcv_msg_desc->size);
    }

    /* clear the request after the processing */
    dp_mbx_clear_request(request_id);
}

/*------------------------------------------------------------------*/
static void host_acknoweledge_handler(  short   request_id,
                                        short   request_data)
{
    request_data    = request_data;

    /* clear the request as soon as possible */
    dp_mbx_clear_request(request_id);

    /* if the server is free call the user handler */
    if (dp_mbx_send_request(cfg->server_req_id, 0))
    {
        dp_comm2_server_free_handler();
    }
}


/*==================================================================*/
/* Communication host routines */

/*------------------------------------------------------------------*/
void dp_comm2_init(DpCommCfg huge *cfg_p)
{
    /* save the configuration */
    cfg             = cfg_p;

    /* initialize the mailbox handlers */
    dp_mbx_register_handler(cfg->host_req_id, host_request_handler);
    dp_mbx_register_handler(cfg->host_ack_id, host_acknoweledge_handler);
}

/*------------------------------------------------------------------*/
void dp_comm2_check_server_free(void)
{
    /* send an ack to ourself */
    dp_mbx_send_request(cfg->host_ack_id, DP_COMM_NULL_BIT);
}


/*------------------------------------------------------------------*/
short dp_comm2_call(    char            update_ds,
                        char            sender_fct,
                        AM_Address huge *dest,
                        void huge       *msg,
                        unsigned short  msg_size,
                        long            ref)
{
    DpMsgDesc huge  *m          = cfg->snd_msg_desc;
    short           request_id  = DP_COMM_NULL_BIT;

    /* check that the mailbox is not busy */
    if (!dp_mbx_send_request(cfg->server_req_id, 0))
    {
        return 0;
    }

    /* ask the dataset update if requested */
    if (update_ds)
    {
        request_id  |= DP_COMM_UPDATE_DS_BIT;
    }

    /* ask the message sending if requested */
    if (sender_fct)
    {
        /* fix the message length */
        if (msg_size > cfg->snd_buff_size)
        {
            msg_size    = cfg->snd_buff_size;
        }

        /* copy the parameters */
        m->size     = msg_size;
        m->our_fct  = sender_fct;
        pi_copy16((void*)&m->ext_addr, (void*)dest, sizeof(*dest) / 2);
        m->host_id  = 0;
        m->ref      = ref;
        m->status   = 0;
        pi_copy16((void*)(m+1), msg, (msg_size+1) / 2);

        /* set the flag */
        request_id  |= DP_COMM_SND_MSG_BIT;
    }

    /* send the call using the mailbox */
    return dp_mbx_send_request(cfg->server_req_id, request_id);
}


/* ****************************************************************************
    $Log: dp_comm2.c,v $
    Revision 1.3  2004/11/12 15:10:11  mungiguerrav
    2004/nov/12,ven 15:30          Napoli       Mungi
            - Si aggiungono keyword CVS e breve storia documentata
            - Si sostituiscono i TAB con 4 spazi
            - Si applica regole di codifica per incolonnamento e parentesi
            - Non si applicano ancora regole di codifica per nomi routine e variabili


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.2        2004/06/15 11:28:54     buscob
01.03   2003/nov/21,ven             Napoli                  Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso.

    Revision 1.1        1997/08/26 09:23:10     accardol
    - Inserimento in CVS della versione in lib07m
    Revision 1.1.1.1    1997/08/26 09:23:10     accardol
    - Inserimento in CVS in branch STP della versione in lib07m
        La data è arretrata con un artificio
01.02   1997/ago/26,mar             Napoli              Carn\Mungi
        - Modifica non documentata

 ------------------------   Prima di CVS    ------------------------
01.01   1997/lug/24,gio             Napoli              Carn\Mungi
        - Si agg.   dummy=dummy     in
          static void host_acknoweledge_handler(short request_id, short dummy)
01.00   1997/lug/23,mer             Napoli              Carn\Mungi
        - Creazione a partire da \CARN02\?????????????????\dp_comm2.c
        - Si dichiara huge la dp_ram, per compatibilita' DAU
*/
