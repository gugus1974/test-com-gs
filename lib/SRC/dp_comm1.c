/********************************************************************/
/* 2004/nov/12,ven 14:30                \SWISV\LIB\SRC\dp_comm1.c   */
/********************************************************************/
/*          Dual port RAM TCN communication: server                 */
/********************************************************************/
/*
    $Date: 2004/11/12 15:09:28 $          $Revision: 1.2 $
    $Author: mungiguerrav $
*/


/*==================================================================*/
/* Includes */
#include "dp_comm.h"
#include "lp_sys.h"
#include "am_sys.h"
#include "pi_sys.h"

/*==================================================================*/
/* Pragmas */
#pragma WL(3)

/*==================================================================*/
/* Globals */
static  DpComm1CfgEntry *cfg;                                   /* configuration list */

static  short           updated_ds_count[DP_COMM_MAX_HOSTS];    /* number of dataset updates to be confirmed        */
static  short           snt_msg_queue[DP_COMM_MAX_HOSTS];       /* queues holding the sent messages (to be acked)   */
static  short           rcv_msg_queue[DP_COMM_MAX_HOSTS];       /* queues holding the received messages             */

/*==================================================================*/
/* Generic private routines */

/*------------------------------------------------------------------*/
/* Find an entry in the configuration list using the server request id */
static DpComm1CfgEntry *find_cfg(short request_id)
{
    DpComm1CfgEntry     *cfgP   = cfg;

    while (cfgP->cfg)
    {
        if (cfgP->cfg->server_req_id == request_id || cfgP->cfg->server_ack_id == request_id)
        {
            return cfgP;
        }

        cfgP++;
    }
    return 0;
}


/*==================================================================*/
/* TCN confirmation routines */

/*------------------------------------------------------------------*/
static void call_conf(          unsigned char   caller_fct,
                                char            *ext_ref,
                        const   AM_ADDRESS      *replier,
                                char            *in_msg_addr,
                                CARDINAL32      in_msg_size,
                                AM_RESULT       status)
{
    short       err     = 0;                        /* error code   */
    DpMsgDesc   *m      = (DpMsgDesc*)ext_ref;      /* sent message */

    /* avoid warnings about unused parameters */
    caller_fct = caller_fct; replier = replier; in_msg_size = in_msg_size;

    /* deallocate the received buffer if created (we don't use it) */
    if (in_msg_addr)
    {
        am_buffer_free(in_msg_addr, in_msg_size);
    }

    /* save the call status */
    m->status   = status;

    /* put the message in the sent messages queue (deallocating it on error) */
    pi_send_queue(snt_msg_queue[m->host_id], (void*)m, &err);
    if (err)
    {
        pi_free((void*)m);
    }
    else
    {
        dp_mbx_send_request(cfg[m->host_id].cfg->server_ack_id, DP_COMM_NULL_BIT);
    }
}

/*------------------------------------------------------------------*/
static void rcv_conf(           unsigned char   replier_fct,
                        const   AM_ADDRESS      *caller,
                                char            *in_msg_addr,
                                CARDINAL32      in_msg_size,
                                char            *ext_ref)
{
    DpComm1CfgEntry     *cfgP   = (DpComm1CfgEntry*)ext_ref;    /* configuration ptr*/
    short               err     = 0;                            /* error code       */

    /* allocate the space for a copy of the message plus header rounded even */
    DpMsgDesc *m = (DpMsgDesc*)pi_alloc((sizeof(DpMsgDesc) + in_msg_size + 1) & 0xFFFE);

    if (m)
    {
        /* fill the header and copy the message */
        m->size     = in_msg_size;
        m->our_fct  = replier_fct;
        m->ext_addr = *(AM_Address*)caller;
        m->host_id  = cfgP - cfg;
        m->ref      = 0;
        m->status   = AM_OK;
        pi_copy16((void*)(m+1), (void*)in_msg_addr, (in_msg_size+1) / 2);

        /* put the message in the received messages queue */
        pi_send_queue(rcv_msg_queue[m->host_id], (void*)m, &err);
        if (err)
        {
            /* the queue is full: discard the message */
            am_reply_requ(replier_fct, 0, 0, ext_ref, AM_NO_READY_INST_ERR);
            pi_free((void*)m);
        }
        else
        {
            /* message queued: reply it and wake up the host sender */
            am_reply_requ(replier_fct, 0, 0, ext_ref, AM_OK);
            dp_mbx_send_request(cfgP->cfg->server_ack_id, DP_COMM_NULL_BIT);
        }
    }
    else
    {
        am_reply_requ(replier_fct, 0, 0, ext_ref, AM_NO_REM_MEM_ERR);   /* out of memory    */
    }

    /* deallocate the received message */
    am_buffer_free(in_msg_addr, in_msg_size);
}

/*------------------------------------------------------------------*/
static void reply_conf(     unsigned char   replier_fct,
                            char            *ext_ref)
{
    DpComm1CfgEntry     *cfgP   = (DpComm1CfgEntry*)ext_ref;    /* configuration ptr*/
    AM_RESULT           status  = 0;                            /* AM error code    */

    /* request another message */
    am_rcv_requ(replier_fct, 0, cfgP->cfg->rcv_buff_size, ext_ref, &status);
}


/*==================================================================*/
/* Mailbox handlers routine */

/*------------------------------------------------------------------*/
static void server_request_handler( short   request_id,
                                    short   request_data)
{
    DpComm1CfgEntry         *cfgP   = find_cfg(request_id); /* configuration ptr    */
    short                   host_id = cfgP - cfg;           /* host id              */
    struct STR_LP_DS_SET    **dsH;                          /* handle to the dataset*/

    if (cfgP)
    {   /* just to be sure */

        if (request_data & DP_COMM_UPDATE_DS_BIT)
        {   /* dataset update request */

            /* update the configured datasets */
            if (cfgP->put_ds_list)
            {
                for (dsH = cfgP->put_ds_list; *dsH != 0; dsH++)
                {
                    lp_put_data_set(*dsH);
                }
            }

            if (cfgP->get_ds_list)
            {
                for (dsH = cfgP->get_ds_list; *dsH != 0; dsH++)
                {
                    lp_get_data_set(*dsH);
                }
            }

            /* increment the count of dataset updates to be confirmed*/
            updated_ds_count[host_id]++;

            /* wake up our sender */
            dp_mbx_send_request(cfgP->cfg->server_ack_id, DP_COMM_NULL_BIT);
        }

        if (request_data & DP_COMM_SND_MSG_BIT)
        {   /* send message request */

            /* calculate the size of the message plus header rounded even */
            short       s   = (sizeof(DpMsgDesc) + cfgP->cfg->snd_msg_desc->size + 1) & 0xFFFE;

            /* allocate the space for a copy of it */
            DpMsgDesc   *m  = (DpMsgDesc*)pi_alloc(s);

            if (m)
            {
                /* copy the message with the header and update it */
                pi_copy16((void*)m, (void*)cfgP->cfg->snd_msg_desc, s/2);
                m->host_id  = host_id;

                /* send the message */
                am_call_requ(m->our_fct, (AM_ADDRESS*)&m->ext_addr, m+1, m->size, 0, 0, 0, call_conf, m);
            }
        }

        /* clear the mailbox before sending the acknoweledge */
        dp_mbx_clear_request(request_id);

        /* send back the acknoweledge (it could be lost but this is not a real problem) */
        dp_mbx_send_request(cfgP->cfg->host_ack_id, request_data);
    }
}

/*------------------------------------------------------------------*/
static void server_acknoweledge_handler(    short   request_id,
                                            short   unused)
{
    DpComm1CfgEntry     *cfgP       = find_cfg(request_id); /* configuration ptr    */
    short               host_id     = cfgP - cfg;           /* host id              */
    short               err         = 0;                    /* error code           */
    DpMsgDesc           *m;                                 /* message descriptors  */
    short               request_data= 0;                    /* host request flags   */

    unused  = unused;

    if (cfgP)
    {   /* just to be sure */

        /* clear the mailbox before sending the next request */
        dp_mbx_clear_request(request_id);

        /* check that the host mailbox is free */
        if (!dp_mbx_send_request(cfgP->cfg->host_req_id, 0))
        {
            return;
        }

        /* check the count of dataset updates to be confirmed */
        if (updated_ds_count[host_id])
        {
            updated_ds_count[host_id]--;
            request_data    |= DP_COMM_UPDATE_DS_DONE_BIT;
        }

        /* check the sent messages queue */
        m   = (DpMsgDesc*)pi_accept_queue(snt_msg_queue[host_id], &err);
        if (!err)
        {
            pi_copy16((void*)cfgP->cfg->snd_msg_done_desc, (void*)m, sizeof(*m)/2);
            pi_free((void*)m);
            request_data    |= DP_COMM_SND_MSG_DONE_BIT;
        }

        /* check the received messages queue */
        m   = (DpMsgDesc*)pi_accept_queue(rcv_msg_queue[host_id], &err);
        if (!err)
        {
            pi_copy16((void*)cfgP->cfg->rcv_msg_desc, (void*)m, (sizeof(*m) + m->size + 1)/2);
            pi_free((void*)m);
            request_data    |= DP_COMM_RCV_MSG_BIT;
        }

        /* send the requests if any */
        if (request_data)
        {
            dp_mbx_send_request(cfgP->cfg->host_req_id, request_data);
        }
    }
}


/*==================================================================*/
/* Communication server routines */

/*------------------------------------------------------------------*/
short dp_comm1_init(DpComm1CfgEntry *cfg_p)
{
    short               err     = 0;    /* error code                       */
    DpComm1CfgEntry     *cfgP;          /* pointer inside the configuration */
    short               host_id;        /* host id                          */
    AM_RESULT           status  = 0;    /* AM error code                    */
    char                *cP;            /* generic char pointer             */

    /* save the configuration */
    cfg     = cfg_p;

    for (cfgP = cfg, host_id = 0; cfgP->cfg != 0; cfgP++, host_id++)
    {
        /* register the mailbox handlers */
        dp_mbx_register_handler(cfgP->cfg->server_req_id, server_request_handler);
        dp_mbx_register_handler(cfgP->cfg->server_ack_id, server_acknoweledge_handler);

        /* create the queues */
        snt_msg_queue[host_id]  = pi_create_queue(cfgP->max_snd_msg_num, PI_ORD_FCFS, &err);
        Check(err);
        rcv_msg_queue[host_id]  = pi_create_queue(cfgP->max_rcv_msg_num, PI_ORD_FCFS, &err);
        Check(err);

        /* subscribe the required TCN functions */
        if (cfgP->fct_list)
        {
            for (cP = cfgP->fct_list; cP != 0; cP++)
            {
                am_bind_replier(*cP, rcv_conf, reply_conf, &status);
                Check(status);
                am_rcv_requ(*cP, 0, cfgP->cfg->rcv_buff_size, cfgP, &status);
                Check(status);
            }
        }
    }

error:
    return err;
}


/* ****************************************************************************
    $Log: dp_comm1.c,v $
    Revision 1.2  2004/11/12 15:09:28  mungiguerrav
    2004/nov/12,ven 14:30          Napoli       Mungi
            - Si aggiungono keyword CVS e breve storia documentata
            - Si sostituiscono i TAB con 4 spazi
            - Si applica regole di codifica per incolonnamento e parentesi
            - Non si applicano ancora regole di codifica per nomi routine e variabili


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.1        1997/12/17 12:43:14     accardol
    - Inserimento in CVS della versione in lib07n
    Revision 1.1.1.1    1997/12/17 12:43:14     accardol
    - Inserimento in CVS in branch STP della versione in lib07n
        La data è arretrata con un artificio
01.01   1997/dic/17,mer             Napoli              Carn\Mungi
        - Modifica non documentata

 ------------------------   Prima di CVS    ------------------------
01.00   1997/lug/23,mer             Napoli              Carn\Mungi
        - Creazione a partire da \CARN02\?????????????????\dp_comm1.c
*/
