/*
*****************************************************************************
*                                                                           *
*    Property of  :   ABB Verkehrssysteme AG, CH-8050 Z*rich / Switzerland  *
*    COPYRIGHT    :   (c) 1991 ABB Verkehrssysteme AG                       *
*                                                                           *
*****************************************************************************
*                                                                           *
*    PROJECT      : IEC TCN                                                 *
*                                                                           *
*    SUBPROJECT   : TCN Link-Layer Message (LM)                             *
*                                                                           *
*****************************************************************************
*                                                                           *
*    MODULE       : LM_LAYER.C                                              *
*                                                                           *
*    DOCUMENT     : TCN Link-Layer Common (LC) / Link-Layer Message (LM)    *
*                   Migration Document, 3EHT 420 015                        *
*                                                                           *
*                   TCN Link-Layer Common (LC) / Link-Layer Message (LM)    *
*                   User's Manual, 3EHL 420 332                             *
*                                                                           *
*                   MVBC ASIC Data Sheet                                    *
*                   Data Sheet, 3EHL 420 441                                *
*                                                                           *
*    ABSTRACT     : Link-Layer Message Module (Everything)                  *
*                                                                           *
*    REMARKS      :                                                         *
*                                                                           *
*    DEPENDENCIES : Link-Layer Common, Programmer's Interface Library       *
*                                                                           *
*    ACCEPTED     :                                                         *
*                                                                           *
*****************************************************************************
*                                                                           *
*    HISTORY:                                                               *
*                                                                           *
*    Version  YY-MM-DD  Name         Dept.    Ref   Comments                *
*    -------  --------  ----------   ------   ----  --------------------    *
*    4.1.0.0  94-01-20  zur Bonsen   BATC      --   Created                 *
*                                                   (100% written new)      *
*    4.1.0.1  94-05-26  zur Bonsen   BATC      --   char -> unsigned char   *
*    4.2.0.0  94-11-11  zur Bonsen   BATC      --   Adapted to new LC 4.2   *
*    4.2.0.0  94-11-11  (no changes in this file)                           *
*    4.3.0.0  95-01-31  zur Bonsen   BATL      --   See below               *
*    4.3.1.0  95-01-31  zur Bonsen   BATL      --   Changes in lm_head.h    *
*    4.3.1.1  95-04-09  zur Bonsen   BATL      --   LM_IEC_PROTOCOL introdu-*
*                           ced.            *
*    4.3.2.0  95-09-15  zur Bonsen   BATL      --   lm_m_v_get_own_phya and *
*                           lm_m_v_get_src_phya new *
*                                                                           *
*****************************************************************************
* Updates in release 4.3.0.0:                                               *
*                                                                           *
* Adaption to targets with MC 68360 Processors (Big Endian)                 *
* Non-ASCII characters removed for use on non-PC systems                    *
*                                                                           *
*****************************************************************************
*/

#include "basetype.h"
#include "md_layer.h"
#include "lm_layer.h"
#include "mvbc.h"
#include "lm_head.h"

/*
*****************************************
* Global Variable                       *
*****************************************
*/

LMI_TYPE_CTRL_BLK lmi_ctrl_blk[LC_CTRL_BLK_SIZE];

/*
*****************************************
* Important Remarks                     *
*****************************************
*/

/* The prefix "p16_", found in variable names of type 'unsigned short',      */
/* represents a 16-bit pointer format used by the MVBC to access message     */
/* queues.  This format is used in the QDT and LLR.  The memory address can  */
/* be obtained by following formula:                                         */

/* addr = (((unsigned long) p16_...) << 2) | (((unsigned long) qo) << 18)    */

/* Legend: qo = queue offset (part of MCM)                                   */


/*
*****************************************
* Functions implementation              *
*****************************************
*/


/*
*****************************************************************************
* name          lmi_check_ts_valid                                          *
*                                                                           *
* param. in     ts_id            Traffic Store Identification               *
* return value  status           MD_OK or MD_REJECT                         *
*****************************************************************************
* description   Checks if ts_id is within specified boundaries and          *
*               that the TS is properly configured.  If both conditions     *
*               are met, then MD_OK is returnecd.                           *
*                                                                           *
*               This function is fully reentrant                            *
*                                                                           *
* globals       lmi_ctrl_blk[ts_id]                                         *
*****************************************************************************
* history       1   93-08-20   BG Created                                   *
*****************************************************************************
*/

short lmi_check_ts_valid(unsigned short ts_id);
short lmi_check_ts_valid(unsigned short ts_id)
{
    short ret_val = (ts_id < LC_CTRL_BLK_SIZE) ? MD_OK : MD_REJECT;

    if (ret_val == MD_OK)
    {
        ret_val = (lmi_ctrl_blk[ts_id].checklist == LMI_CHK_OPERATIONAL) ? MD_OK : MD_REJECT;
    }

    if (ret_val != MD_OK)
    {
        lmx_hamster( LMX_POSITION, LMX_CONTINUE, 1, ts_id, 0, 0 );
        /* $H. Invalid ts_id, ts_id = %d (decimal) */
    }
    return (ret_val);
}

/*
*****************************************************************************
* name          lmi_report_rq_overflow                                      *
*                                                                           *
* param. in     param            Parameter, = Traffic Store ID used         *
*                                                                           *
* return value  -                                                           *
*****************************************************************************
* description   Indicates that receive queue overflow occurred              *
*                                                                           *
*               This function serves as an Interrupt Service Routine (ISR)  *
*                                                                           *
* globals       lmi_ctrl_blk[ts_id]                                         *
*****************************************************************************
* history       1   93-08-31   BG Created                                   *
*               2   94-22-11   ts_id parameter removed, param used instead  *
*****************************************************************************
*/

void lmi_report_rq_overflow(unsigned short param);
void lmi_report_rq_overflow(unsigned short param)
{
    TM_TYPE_SERVICE_AREA *p_sa;         /* Pointer to Service Area           */

    lmi_ctrl_blk[param].rq_overflow = MD_RECV_OVF;

    p_sa = lmi_ctrl_blk[param].p_tm_service_area;

    lmx_hamster_continue( p_sa->qdt.rcve_q == 0 );
    /* $H. Nonexistent receive queue encountered */

}


/*
*****************************************************************************
* name          lmi_install_q                                               *
*                                                                           *
* param. in     p_space          Memory Fragment Description                *
*                                                                           *
* param. out    p16_q            Software pointer to queue                  *
*                                                                           *
* return value  ret_val          MD_OK or MD_REJECT (bad configuration)     *
*****************************************************************************
* description   Installation of one queue structure                         *
*                                                                           *
*               This function is reentrant as long different MVBC/TS are    *
*               processed.                                                  *
*                                                                           *
* globals       lmi_ctrl_blk[ts_id]                                         *
*****************************************************************************
* history       1.0 93-08-31   BG Created                                   *
*               1.1 94-05-26   BG total_llr_blk_cnt -> total_data_blk_cnt   *
*****************************************************************************
*/

short lmi_install_q( struct   LM_STR_FRAGMENTS *p_space,
                    TM_TYPE_LLR **p16_q );
short lmi_install_q( struct   LM_STR_FRAGMENTS *p_space,
                    TM_TYPE_LLR **p16_q )
{
    unsigned short ret_val = MD_OK; /* Return Value                          */

    TM_TYPE_LLR     llr_1st;        /* First pointer to LLR                  */
    TM_TYPE_LLR  *p_llr_prev;       /* Pointer to previous LLR               */
    TM_TYPE_LLR  *p_llr     ;       /* Pointer to current LLR                */

    TM_TYPE_PAGE    *p_data;        /* Pointer to data area -> cleared       */
    unsigned short p16_data;        /* Pointer to 16Wd Data Block            */

    unsigned short llr_blk_cnt;     /* # 32Byte blocks needed for LLR        */
    unsigned short total_data_blk_cnt = 0; /* Accumulation thereof           */

    unsigned short i;               /* Loop Index                            */
    unsigned short j;               /* Inner Loop Index                      */
    unsigned short first = !0;      /* Indicates first time                  */

    struct LM_STR_FRAGMENTS *p_current = p_space; /* Ptr to frag. descr.     */


    p_llr_prev = &llr_1st;          /* Holder for p16 to first LLR           */

    while (p_current != NULL)
    {
        /*
        ******************************
        * Build 1st LLR in fragment  *
        ******************************
        */

        /* Compute block count needed by LLR, unit: 32-byte blocks */
        llr_blk_cnt = ((p_current->size-1) / LMI_Q_BLK_P16U_SIZE)+1;

        /* Pointer to top of fragment: -> 1st LLR */
        p_llr  = (TM_TYPE_LLR *) p_current->p_tm_fragment;

        /* Chain LLR.NEXT-pointer up to next fragment */
        p_llr_prev->p16_next = lmx_ptr_to_p16(p_llr);

        /* Pointer to data area, is a TM_TYPE_PAGE type */
        p_data = (TM_TYPE_PAGE *) lmx_ptr_add_ofs(p_current->p_tm_fragment, llr_blk_cnt*LMI_Q_BLK_BYTE_SIZE);

        p16_data =  lmx_ptr_to_p16(p_data);

        total_data_blk_cnt += p_current->size;

        for (i=0; i<p_current->size; i++)
        {
            /*
            ******************************
            * Install queue in fragment  *
            ******************************
            */

            /* Mark 1st data block as NULL: Queue boundary marker */
            if (first != 0)
            {
                p_llr->p16_data  = 0;
                first            = 0;
            }
            else /* Install pointer to data block */
            {
                p_llr->p16_data  = p16_data;
            }
            p16_data += LMI_Q_BLK_P16U_SIZE;
            for (j = 0; j < LMI_MSG_SIZE_WD; j++)
            {
                ((unsigned short *) p_data)[j] = 0;
            }
            p_data++;

            /* Install pointer to next LLR */

            p_llr_prev = p_llr++;
            p_llr_prev->p16_next = lmx_ptr_to_p16(p_llr);

        }   /* for */

        p_current = p_current->p_next; /* Next fragment */

    }   /* while */

    /*
    ******************************
    * Tilt Queue to Ring Buffer  *
    ******************************
    */

    p_llr_prev->p16_next = llr_1st.p16_next;
    *p16_q               = (TM_TYPE_LLR*) p_space->p_tm_fragment;

    if (total_data_blk_cnt < LMI_MIN_QUEUE_SIZE)      /* Check minimum Q size */
    {
        lmx_hamster( LMX_POSITION, LMX_CONTINUE, 1, total_data_blk_cnt, 0, 0 );
        /* $H. Number of locations per queue fragment too low: %d (decimal) */

        ret_val = MD_REJECT;
    }
    return (ret_val);
}


/*
*****************************************************************************
* name          lm_v_system_init                                            *
*                                                                           *
* param. in     -                                                           *
*                                                                           *
* return value  -                                                           *
*****************************************************************************
* description   Initialization of Link Layer Message                        *
*                                                                           *
*               This function is reentrant as long different MVBC/TS are    *
*               processed.                                                  *
*                                                                           *
*                                                                           *
* globals       lmi_ctrl_blk (all)                                          *
*****************************************************************************
* history       1   93-08-31   BG Created                                   *
*****************************************************************************
*/

void lm_v_system_init( void )
{
    short i = lmx_half(sizeof(lmi_ctrl_blk));
                                        /* Loop Index, Init=size in words    */
                                        /* Numeric pointer of control block  */

    unsigned short *cb = (unsigned short *) lmi_ctrl_blk;

    while (i-- > 0)
    {
        *cb++ = 0;
    }
}


/*
*****************************************************************************
* name          lm_m_v_config                                               *
*                                                                           *
* param. in     ts_id            Traffic Store  Identification              *
*               p_xq0_space      Memory Fragment Description for xmit q 0   *
*               p_xq1_space      Memory Fragment Description for xmit q 1   *
*               p_rq_space       Memory Fragment Description for rcve queue *
*                                                                           *
* return value  status           MD_OK or MD_REJECT                         *
*                                MD_REJECT occurs if control block is not   *
*                                initialized or DA > 255 (DA not suitable)  *
*****************************************************************************
* description   Installation of queue structures                            *
*                                                                           *
*               This function is reentrant as long different MVBC/TS are    *
*               processed.                                                  *
*                                                                           *
* globals       lmi_ctrl_blk[ts_id]                                         *
*****************************************************************************
* history       1   93-08-31   BG Created                                   *
*****************************************************************************
*/

short lm_m_v_config( unsigned short ts_id,
                     struct LM_STR_FRAGMENTS *p_xq0_space,
                     struct LM_STR_FRAGMENTS *p_xq1_space,
                     struct LM_STR_FRAGMENTS *p_rq_space )
{
    TM_TYPE_SERVICE_AREA *p_sa;         /* Pointer to Service Area           */
    TM_TYPE_PCS          *p_pcs;        /* Pointer to a selected PCS         */

    unsigned short da;                  /* Device Address                    */
    unsigned short ret_val = MD_REJECT; /* Return Value, for Hamster         */


    /*
    ******************************
    * Validity Check             *
    ******************************
    */

    if (ts_id < LC_CTRL_BLK_SIZE)       /* ts_id for LM is equivalent to LC  */
    {

        if (lc_m_get_device_address(ts_id, &da)==LC_OK)
        {

            if (da <= LMI_HIGHEST_DA)   /* DA must be 255 or less            */
            {
                ret_val = MD_OK;        /* Assume OK for time being          */

                lmi_ctrl_blk[ts_id].p_tm_service_area = (void *) lc_m_get_service_area_addr(ts_id);

                p_sa = lmi_ctrl_blk[ts_id].p_tm_service_area;

                /*
                ******************************
                * Install/Init. Msg. Queues  *
                ******************************
                */

                ret_val |= lmi_install_q( p_xq0_space, &lmi_ctrl_blk[ts_id].p_xq[LMI_XQ0] );
                ret_val |= lmi_install_q( p_xq1_space, &lmi_ctrl_blk[ts_id].p_xq[LMI_XQ1] );
                ret_val |= lmi_install_q( p_rq_space , &lmi_ctrl_blk[ts_id].p_rq    );

                if (ret_val == MD_OK)
                {

                    /* Specify queue pointers in Service Area                    */

                    /* Xmit Q's: They wil be defined at the 1st call of lm_m_v_- */
                    /* in order to avoid duplicate code.  Rcve Q's are declared  */
                    /* as empty queues - for the time being                      */

                    p_sa->qdt.xmit_q[LMI_XQ0] = 0;
                    p_sa->qdt.xmit_q[LMI_XQ1] = 0;
                    p_sa->qdt.rcve_q          = lmi_ctrl_blk[ts_id].p_rq->p16_next;

                    /*
                    ******************************
                    * Cfg. Message Source Port   *
                    ******************************
                    */

                    p_pcs = &(p_sa->pp_pcs[TM_PP_MSRC]);

                    p_pcs->word0.w =
                          W_FC12          /* F-Code 12                               */
                        | TM_PCS0_SRC     /* Active Source                           */
                        | TM_PCS0_CPE1    /* Clears Events of Event Type 1           */
                        | TM_PCS0_QA;     /* Queues attached.  MD is Non-Numeric.    */

                    p_pcs->word1.w =
                          TM_PCS1_VP0;    /* Guarantees a correct value in PCS 1     */

                    p_pcs->tack = 0;      /* No message has been transferred before  */
                                                                                                         /*
                    ******************************
                    * Cfg. Message Sink Port     *
                    ******************************
                    */

                    p_pcs = &(p_sa->pp_pcs[TM_PP_MSNK]);

                    p_pcs->word0.w =
                          W_FC12          /* F-Code 12                               */
                        | TM_PCS0_SINK    /* Active Sink                             */
                        | TM_PCS0_QA;     /* Queues attached.  MD is Non-Numeric.    */

                    p_pcs->word1.w =
                          TM_PCS1_VP0;    /* Guarantees a correct value in PCS 1     */
                    p_pcs->tack = 0;      /* No message has been transferred before  */

                    /*
                    ******************************
                    * Cfg. Event Port for ET=1   *
                    ******************************
                    */

                    p_pcs = &(p_sa->pp_pcs[TM_PP_EF1]);

                    p_pcs->word0.w =
                          W_FC9           /* F-Code 9 (MVBC does not check FC here)  */
                        | TM_PCS0_SRC     /* Active Source                           */
                        | TM_PCS0_NUM;    /* Ev. Frames are contiguous 16-bit words  */

                    p_pcs->word1.w =
                          TM_PCS1_VP0;    /* Guarantees a correct value in PCS 1     */


                    /*
                    ******************************
                    * Connect Intr. Serv. Rout.  *
                    ******************************
                    */

                    /* Receive Queue Exception (Overflow) */

                    lc_m_int_connect( ts_id, lmi_report_rq_overflow, LC_INT_RQE, LC_NO_KERNEL, ts_id);


                    /* Mark that configuration has taken place */

                    lmi_ctrl_blk[ts_id].checklist = LMI_CHK_CONFIGURED;

                    /*
                    ******************************
                    * Declare Device Type in DSW *
                    ******************************
                    */

                    lc_m_set_device_status_word( ts_id, LC_DSW_TYPE_CLASS_2_3_MSK,
                                                        LC_DSW_TYPE_CLASS_2_3_SET );

                } /* if (ret_val==MD_OK) ...      */
            }     /* if (da <= ...                */
        }         /* if (lc_get_device_address... */
    }             /* if (ts_id < LC_CTRL_BLK_SIZE */

    return (ret_val);
}

/*
*****************************************************************************
* name          lm_m_v_init                                                 *
*                                                                           *
* param. in     ts_id            Traffic Store  Identification              *
*               receive_confirm  Receive Confirm Function !!! not used !!!  *
*               get_pack         Get packet Function                        *
*               ident            MD Packet Pool Identification              *
*               put_pack         Put packet function                        *
*               status_indicate  Function to call to indicate that LM is OK *
*                                                                           *
* return value  -                                                           *
*****************************************************************************
* description   Final activation of message queue system                    *
*                                                                           *
*               This function is reentrant as long different MVBC/TS are    *
*               processed.                                                  *
*                                                                           *
* globals       lmi_ctrl_blk[ts_id]                                         *
*****************************************************************************
* history       1   93-08-31   BG Created                                   *
*****************************************************************************
*/

void lm_m_v_init( unsigned short ts_id,
                  void (* receive_confirm)(void),
                  void (* get_pack)(void **, MD_PACKET ** ),
                  void ** ident,
                  void (* put_pack)( MD_PACKET * ),
                  void (* status_indicate)(int ) )

{
    if (ts_id < LC_CTRL_BLK_SIZE)
    {
        if (lmi_ctrl_blk[ts_id].checklist == LMI_CHK_CONFIGURED)
        {
            lmi_ctrl_blk[ts_id].get_pack = get_pack;
            lmi_ctrl_blk[ts_id].put_pack = put_pack;
            lmi_ctrl_blk[ts_id].ident    = ident   ;

            status_indicate(MD_READY);   /* Report to upper level */

            lmi_ctrl_blk[ts_id].checklist = LMI_CHK_OPERATIONAL;
        }
        else
        {
            status_indicate(MD_REJECT);
        }
    }
    else
    {
        status_indicate(MD_REJECT);
    }
    receive_confirm = receive_confirm; /* !!! to avoid warning */
}


/*
*****************************************************************************
* name          lm_m_v_get_status                                           *
*                                                                           *
* param. in     ts_id            Traffic Store  Identification              *
*               selector         Selects one of contants                    *
*               reset            Indicates whether to reset status or not   *
*                                                                           *
* param. out    status           Status flags defined by 'selector'         *
*                                                                           *
* return value  -                                                           *
*****************************************************************************
* description   Returns message status                                      *
*                                                                           *
*               !!! MVBC migration: MD_IRQ_WITHOUT_MESSAGE and              *
*                   MD_INDICATE_MESSAGE are no longer used by this code.    *
*                                                                           *
*               This function is fully reentrant                            *
*                                                                           *
* globals       lmi_ctrl_blk[ts_id]                                         *
*****************************************************************************
* history       1   93-08-31   BG Created                                   *
*****************************************************************************
*/

void lm_m_v_get_status ( unsigned short ts_id, unsigned short selector,
                         unsigned short reset, unsigned short *status )
{
    TM_TYPE_SERVICE_AREA *p_sa;         /* Pointer to Service Area           */
    volatile unsigned short       *p_msrc_tack;  /* Pointer to MSRC Tack bits         */
    volatile unsigned short       *p_msnk_tack;  /* Pointer to MSNK Tack bits         */

    unsigned short       int_status;    /* Internal status bits              */


    if (lmi_check_ts_valid(ts_id) == MD_OK)
    {
        int_status = lmi_ctrl_blk[ts_id].rq_overflow;

        p_sa = lmi_ctrl_blk[ts_id].p_tm_service_area;

        p_msrc_tack = &(p_sa->pp_pcs[TM_PP_MSRC].tack);
        if (*p_msrc_tack == LMI_TACK_ACTIVE)
        {
            int_status |= MD_SEND_ACTIVE;
        }

        p_msnk_tack = &(p_sa->pp_pcs[TM_PP_MSNK].tack);
        if (*p_msnk_tack == LMI_TACK_ACTIVE)
        {
            int_status |= MD_RECV_ACTIVE;
        }

        *status = int_status & selector;                /* Filter option     */

        int_status = int_status & reset;                /* Reset  option     */

        if ((int_status & MD_SEND_ACTIVE) != 0)
        {
            *p_msrc_tack = LMI_TACK_PASSIVE;
        }

        if ((int_status & MD_RECV_ACTIVE) != 0)
        {
            *p_msnk_tack = LMI_TACK_PASSIVE;
        }

        if ((int_status & MD_RECV_OVF) != 0)
        {
            lmi_ctrl_blk[ts_id].rq_overflow = 0;
        }

    }   /* if (lmi_check_ts_valid... */
}

/*
*****************************************************************************
* name          lm_m_v_send_request                                         *
*                                                                           *
* param. in     ts_id            Traffic Store  Identification              *
*               src_dev          Source Device                              *
*               dst_dev          Destination Device                         *
*               packet           Pointer to packet to be sent               *
*                                                                           *
* param. out    status           MD_OK or MD_REJECT                         *
*                                                                           *
* return value  -                                                           *
*****************************************************************************
* description   Returns message status                                      *
*                                                                           *
*               !!! MVBC migration: MD_IRQ_WITHOUT_MESSAGE and              *
*                   MD_INDICATE_MESSAGE are no longer used by this code.    *
*                                                                           *
* globals       lmi_ctrl_blk[ts_id]                                         *
*****************************************************************************
* history       1   93-08-31   BG Created                                   *
*****************************************************************************
*/
void lm_m_v_send_request( unsigned short   ts_id,
                          unsigned char    src_dev,
                          unsigned char    dst_dev,
                          MD_PACKET      * packet,
                          int            * status   )
{

    TM_TYPE_SERVICE_AREA *p_sa;         /* Pointer to Service Area           */
    unsigned short       dev_addr_temp; /* Temp. storage for Dev. Address    */

    unsigned short       p16_next;      /* LLR Next Pointer (MVBC format)    */
    unsigned short       p16_data;      /* LLR Data Pointer (MVBC format)    */

    TM_TYPE_LLR          *p_this;       /* Pointer to current LLR            */
    TM_TYPE_LLR          *p_next;       /* Pointer to next    LLR            */
    TM_TYPE_PAGE         *p_data;       /* Pointer to current queue data     */

    unsigned short       xq_nr;         /* Transmit Queue Number: 0 or 1     */


    if (lmi_check_ts_valid(ts_id) == MD_OK)
    {
        p_sa = lmi_ctrl_blk[ts_id].p_tm_service_area;

        /* Identify Priority and select Transmit Queue: 0 or 1 */

        xq_nr = ( (packet->control & MD_HIGH_PRIO) != 0 ) ? LMI_XQ0 : LMI_XQ1;

        /*
        ******************************
        * Construct Msg Packet Header*
        ******************************
        */

        if ( src_dev == 0 ) /* References own device */
        {
            lc_m_get_device_address(ts_id, &dev_addr_temp);
            src_dev = (char) dev_addr_temp ;  /* It's the most up-to-date DA */
        }

        packet->port.lk.version.mvb.mode_hdest = LMI_PT_TO_PT << 4; /* 22.10.1991 EH */
        packet->port.lk.version.mvb.src        = src_dev ;
        packet->port.lk.version.mvb.prot_hsrc  = LM_IEC_PROTOCOL ;
        packet->port.lk.version.mvb.dest       = dst_dev ;
        packet->next                           = NULL ;

        /*
        ******************************
        * Check state of Xmit Queue  *
        ******************************
        */

        /* Check if Transmit Queue was flushed or reset, then reinitalize them */

        if ((p_sa->qdt.xmit_q[LMI_XQ0]==0) || (p_sa->qdt.xmit_q[LMI_XQ1]==0))
        {
            p_sa->qdt.xmit_q[LMI_XQ0] = lmx_ptr_to_p16(lmi_ctrl_blk[ts_id].p_xq[LMI_XQ0] );
            p_sa->qdt.xmit_q[LMI_XQ1] = lmx_ptr_to_p16(lmi_ctrl_blk[ts_id].p_xq[LMI_XQ1] );
        }

        /* Check if Transmit Queue is already full */

        p_this   = lmi_ctrl_blk[ts_id].p_xq[xq_nr];
        p16_next = p_this->p16_next;

        if (p16_next == p_sa->qdt.xmit_q[xq_nr])
        {
            /* Transmit Queue Full */
            *status = MD_REJECT;
        }
        else
        {
            *status = MD_OK;

            /*
            ******************************
            * Put Msg Packet into Queue  *
            ******************************
            */

            /* Copy data from MD_PACKET to Transmit Queue */

            lmx_disable();
            p_next   = lmx_p16_to_ptr( p16_next, p_this );
            p16_data = p_next->p16_data;
            p_data   = lmx_p16_to_ptr( p16_data, p_this );

            lmx_copy16( (void*)p_data, (void*)&(packet->port), LMI_MSG_SIZE_WD );

            p_next->p16_data = 0;
            p_this->p16_data = p16_data;
            lmi_ctrl_blk[ts_id].p_xq[xq_nr] = p_next;
            lmx_enable();

            packet->status = MD_SENT ;  /* Indicate Message as 'sent'        */

            lmx_disable();              /* Put Packet back into pool         */
            lmi_ctrl_blk[ts_id].put_pack(packet);
            lmx_enable();
        } /* else (where Xmit Q not full) */

        /*
        ******************************
        * Announce Event with ET=1   *
        ******************************
        */

        tm_1_data_wd((p_sa->pp_data),TM_PP_EF1,TM_PAGE_0,0)
          = W_FC12                      /* F-Code for Message Data           */
          | ((unsigned short) src_dev); /* Own Device Address                */
        p_sa->int_regs.mr.w = TM_MR_EA1;

    }   /* if (lmi_check_ts_valid... */
    else
    {
        *status = MD_REJECT;
    }
}

/*
*****************************************************************************
* name          lm_m_v_send_queue_flush                                     *
*                                                                           *
* param. in     ts_id            Traffic Store  Identification              *
*                                                                           *
* return value  -                                                           *
*****************************************************************************
* description   Flushes Send Queue                                          *
*                                                                           *
* globals       lmi_ctrl_blk[ts_id]                                         *
*****************************************************************************
* history       1   93-08-31   BG Created                                   *
*****************************************************************************
*/

void lm_m_v_send_queue_flush( unsigned short ts_id )
{
    TM_TYPE_SERVICE_AREA *p_sa;         /* Pointer to Service Area           */

    if (lmi_check_ts_valid(ts_id) == MD_OK)
    {
        p_sa = lmi_ctrl_blk[ts_id].p_tm_service_area;


        /* Deactivate event arbitration participation at MVBC */

        p_sa->int_regs.mr.w = TM_MR_EC1;  /* Cancel Event 1                  */

        /* Flush both transmit queues */
        p_sa->qdt.xmit_q[LMI_XQ0] = 0; /* Marks 'just flushed'               */
        p_sa->qdt.xmit_q[LMI_XQ1] = 0;


        /* !!! Attention: Following cases have bene taken care of.           */

        /* If the Master has already polled the event and is about to ini-   */
        /* tiate the message transfer, then the MVBC will flag a TRANSMIT    */
        /* QUEUE EXCEPTION which requires no further attention here.         */

        /* If queue preprocessing took place while no post processing has    */
        /* occurred yet, then a 0 will be encountered and END-OF-QUEUE       */
        /* occurs which requires no further software attention.              */

    }   /* if (lmi_check_ts_valid... */
}


/*
*****************************************************************************
* name          lm_m_v_receive_poll                                         *
*                                                                           *
* param. in     ts_id                 Traffic Store  Identification         *
*               source_device         source device                         *
*               destination_device    destination device                    *
*               packet                pointer on the packet to send         *
*                                                                           *
* param. out    status                status of this request.               *
*                                                                           *
* return value  -                                                           *
*****************************************************************************
* description   Return the pointer of the first packet.                     *
*               The pointer is null when the chain is leer.                 *
*                                                                           *
* globals       lmi_ctrl_blk[ts_id]                                         *
*****************************************************************************
* history       1   93-08-31   BG Created                                   *
*****************************************************************************
*/

void lm_m_v_receive_poll( unsigned  short ts_id,
                          unsigned  char *src_dev,
                          unsigned  char *dst_dev,
                          MD_PACKET **packet, int *status )
{
    TM_TYPE_SERVICE_AREA *p_sa;         /* Pointer to Service Area          */
    MD_PACKET            *p;            /* Incoming Message Packet from pool*/

    unsigned short       p16_next;      /* LLR Next Pointer (MVBC format)   */
    unsigned short       p16_data;      /* LLR Data Pointer (MVBC format)   */

    TM_TYPE_LLR          *p_this;       /* Pointer to current LLR           */
    TM_TYPE_LLR          *p_next;       /* Pointer to next    LLR           */
    TM_TYPE_PAGE         *p_data;       /* Pointer to current queue data    */

    if (lmi_check_ts_valid(ts_id) == MD_OK)
    {
        p_sa = lmi_ctrl_blk[ts_id].p_tm_service_area;

        /*
        ******************************
        * Check if MVBC received Msgs*
        ******************************
        */

        p_this   = lmi_ctrl_blk[ts_id].p_rq;
        p16_next = p_this->p16_next;

        if (p_sa->qdt.rcve_q == p16_next) /* Queue empty */
        {
            *status = MD_QUEUEVOID;
        }
        else
        {
            lmi_ctrl_blk[ts_id].get_pack( lmi_ctrl_blk[ts_id].ident , &p ) ;

            if ( p == NULL )
            {
                lmx_hamster_continue (TRUE);
                /* $H. Receive overflow */
                *status = MD_REJECT ;        /* Packet Pool Mgt */
            }
            else
            {
                /*
                ******************************
                * Get Msg Packet from Queue  *
                ******************************
                */

                lmx_disable();
                p_next   = lmx_p16_to_ptr( p16_next, p_this );
                p16_data = p_next->p16_data;
                p_data   = lmx_p16_to_ptr( p16_data, p_this );

                lmx_copy16( (void*)&(p->port), (void*)p_data, LMI_MSG_SIZE_WD );

                p_next->p16_data = 0;
                p_this->p16_data = p16_data;
                lmi_ctrl_blk[ts_id].p_rq = p_next;
                lmx_enable();

                /*
                ******************************
                * Extract Msg Packet Header  *
                ******************************
                */

                *packet  = p;
                *src_dev = p->port.lk.version.mvb.src;
                *dst_dev = p->port.lk.version.mvb.dest;

                /* 0x00FF: No const: Masks away Communication Mode (CM) */

                *status = MD_OK;

            } /* else (where p != NULL)         */
        }     /* else (where queue is not empty */
    }         /* if (lmi_check_ts_valid ...     */
    else
    {
        *status = MD_REJECT;
    }
}


/*
*****************************************************************************
* name          lm_m_v_get_own_phya                                         *
*                                                                           *
* param. in     ts_id            Traffic Store  Identification              *
*                                                                           *
* param. out    status           MD_OK or MD_REJECT                         *
*                                                                           *
* return value  own_phya         Own physical address                       *
*                                (zero if MD_REJECT is indicated in status) *
*                                                                           *
*****************************************************************************
* description   Returns physical address of the link layer of own device    *
*                                                                           *
* globals       lmi_ctrl_blk[ts_id] (read, not modified)                    *
*****************************************************************************
* history       1   95-09-15   BG Created                                   *
*****************************************************************************
*/

unsigned short lm_m_v_get_own_phya( unsigned  short  ts_id,
                                    int             *status   )
{
    unsigned short        own_phya = 0; /* Own physical address             */

    if (lmi_check_ts_valid(ts_id) == MD_OK)
    {
        own_phya = lc_m_get_device_status_word(ts_id);
        *status = MD_OK;
    }         /* if (lmi_check_ts_valid ...     */
    else
    {
        *status = MD_REJECT;
    }
    return (own_phya);
}

/*
*****************************************************************************
* name          lm_m_v_get_src_phya                                         *
*                                                                           *
* param. in     packet           Pointer on the packet to check             *
*                                                                           *
* param. out    status           MD_OK or MD_REJECT                         *
*                                                                           *
* return value  src_phya         Physical address of specified source dev.  *
*                                (zero if MD_REJECT is indicated in status) *
*                                                                           *
*****************************************************************************
* description   Returns physical address of the link layer of source device *
*                                                                           *
* globals       lmi_ctrl_blk[ts_id] (read, not modified)                    *
*****************************************************************************
* history       1   95-09-15   BG Created                                   *
*****************************************************************************
*/

unsigned short lm_m_v_get_src_phya( MD_PACKET * packet,
                                    int       * status   )
{
    unsigned short        src_phya = 0; /* Own physical address             */

    if (packet != NULL)
    {
        src_phya = packet->port.lk.version.mvb.src
                 + ((packet->port.lk.version.mvb.prot_hsrc & 0xF) <<8);
        *status  = MD_OK;
    }
    else
    {
        *status = MD_REJECT;
    }
    return (src_phya);
}

