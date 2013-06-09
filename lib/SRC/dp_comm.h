/********************************************************************/
/* 2004/nov/12,ven 13:55                \SWISV\LIB\SRC\dp_comm.h    */
/********************************************************************/
/*              Dual port RAM TCN communication                     */
/********************************************************************/
/*
    $Date: 2004/11/12 15:08:03 $          $Revision: 1.2 $
    $Author: mungiguerrav $
*/


#ifndef _DP_COMM_
#define _DP_COMM_

/*==================================================================*/
/* Includes */
#include "dp_mbx.h"

/*==================================================================*/
/* Constants */

/* maximum number of hosts */
#define DP_COMM_MAX_HOSTS               8

/* request data bits */
#define DP_COMM_NULL_BIT                1
#define DP_COMM_UPDATE_DS_BIT           2
#define DP_COMM_SND_MSG_BIT             4
#define DP_COMM_UPDATE_DS_DONE_BIT      8
#define DP_COMM_SND_MSG_DONE_BIT        16
#define DP_COMM_RCV_MSG_BIT             32


/*==================================================================*/
/* Typedefs */

/*------------------------------------------------------------------*/
typedef struct
{
    unsigned char   vehicle;            /* physical or logical addressing           */
    unsigned char   fct_or_sta;         /* station if phys., function if log.       */
    unsigned char   next_sta;           /* next station used on the way             */
    unsigned char   tc;                 /* topo count for which vehicle is valid    */
} AM_Address;

/*------------------------------------------------------------------*/
typedef struct
{               /**** Message header (message follows) ****/
    unsigned short  size;               /* size of the message in bytes             */
    short           our_fct;            /* our function                             */
    AM_Address      ext_addr;           /* external address                         */
    short           host_id;            /* host id                                  */
    long            ref;                /* application reference for this message   */
    short           status;             /* message status                           */
    /* message */
} DpMsgDesc;

/*------------------------------------------------------------------*/
typedef struct
{               /**** Communication configuration ****/
    short           server_req_id;      /* server request mailbox id                */
    short           server_ack_id;      /* server acknoweledge mailbox id           */
    short           host_req_id;        /* host request mailbox id                  */
    short           host_ack_id;        /* host acknoweledge mailbox id             */
    DpMsgDesc huge  *snd_msg_desc;      /* ptr to the send message header           */
    DpMsgDesc huge  *snd_msg_done_desc; /* ptr to the send message done descriptor  */
    DpMsgDesc huge  *rcv_msg_desc;      /* ptr to the receive message header        */
    short           snd_buff_size;      /* size of the send buffer                  */
    short           rcv_buff_size;      /* size of the receive bufer                */
} DpCommCfg;

/*------------------------------------------------------------------*/
typedef struct
{               /**** Communication server configuration entry **/
    DpCommCfg       *cfg;               /* ptr to the communication configuration   */
    void            **put_ds_list;      /* ptr to the list of datasets to put (struct STR_LP_DS_SET) */
    void            **get_ds_list;      /* ptr to the list of datasets to get (struct STR_LP_DS_SET) */
    char            *fct_list;          /* ptr to the list of subscribed functions  */
    short           max_snd_msg_num;    /* maximum number of pending send calls     */
    short           max_rcv_msg_num;    /* maximum number of pending receive calls  */
} DpComm1CfgEntry;


/*==================================================================*/
/* Prototypes */

/*------------------------------------------------------------------*/
/* Communication server functions */
short   dp_comm1_init(DpComm1CfgEntry *cfg_p);

/*------------------------------------------------------------------*/
/* Communication host functions */
void    dp_comm2_init(  DpCommCfg huge  *cfg_p);

void    dp_comm2_check_server_free( void);

short   dp_comm2_call(  char            update_ds,
                        char            sender_fct,
                        AM_Address huge *dest,
                        void huge       *msg,
                        unsigned short  msg_size,
                        long            ref);

/*------------------------------------------------------------------*/
/* Communication host user provided functions */
extern  void dp_comm2_server_free_handler(      void);

extern  void dp_comm2_update_ds_done_handler(   void);

extern  void dp_comm2_snd_msg_done_handler(     long            ref,
                                                short           status);

extern  void dp_comm2_rcv_msg_handler(          char            dest_fct,
                                                AM_Address huge *sender,
                                                void huge       *msg,
                                                unsigned short  msg_size);

#endif


/* ****************************************************************************
    $Log: dp_comm.h,v $
    Revision 1.2  2004/11/12 15:08:03  mungiguerrav
    2004/nov/12,ven 13:55          Napoli       Mungi
            - Si aggiungono keyword CVS e breve storia documentata
            - Si sostituiscono i TAB con 4 spazi
            - Si applica regole di codifica per incolonnamento
            - Non si applicano ancora regole di codifica per nomi routine e variabili


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.1        2000/01/20 09:23:20     accardol
    - Inserimento in CVS della versione in lib07n
    Revision 1.1.1.1    2000/01/20 09:23:20     accardol
    - Inserimento in CVS in branch STP della versione in lib07n
        La data è arretrata con un artificio
01.02   2001/nov/22,gio         Napoli                  Mungi
        - Si agg. i void negli argomenti dei prototipi:
                    void dp_comm2_check_server_free(        void );
            extern  void dp_comm2_server_free_handler(      void );
            extern  void dp_comm2_update_ds_done_handler(   void );

 ------------------------   Prima di CVS    ------------------------
01.01   1997/ago/26,mar             Napoli              Carn\Mungi
        - Modifica non documentata
01.00   1997/lug/23,mer             Napoli              Carn\Mungi
        - Creazione a partire da \CARN02\?????????????????\dp_comm.h
*/
