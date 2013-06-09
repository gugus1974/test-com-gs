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
*    SUBPROJECT   : Link-Layer Message (LM)                                 *
*                                                                           *
*****************************************************************************
*                                                                           *
*    MODULE       : LM_LAYER.C                                              *
*                                                                           *
*    DOCUMENT     :                                                         *
*                                                                           *
*    ABSTRACT     : Interface Alignment for MVB and WTB Link Layer Func's   *
*                                                                           *
*    REMARKS      :                                                         *
*                                                                           *
*    DEPENDENCIES :                                                         *
*                                                                           *
*    ACCEPTED     :                                                         *
*                                                                           *
*****************************************************************************
*                                                                           *
*    HISTORY:                                                               *
*                                                                           *
*     Version  YY-MM-DD  Name         Dept.    Ref   Comments               *
*    -------  --------  ----------   ------   ----  --------------------    *
*    4.3.0.0  95-02-28  zur Bonsen   BATC      --   created                 *
*                                                                           *
*                                                                           *
*****************************************************************************
*/

#include "lm_layer.h"


#if (defined (NM_1TB)) || (defined (NM_GW_SIMPLE))

   /* Declare these functions only if a target system with */
   /* link to 1 WTB is supported                           */


/*
*****************************************************************************
* name          lm_m_t_init                                                 *
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
* description   WTB Initialization Funciton                                 *
*                                                                           *
*****************************************************************************
* history       1   95-02-26   BG Created                                   *
*****************************************************************************
*/

void lm_m_t_init( unsigned short ts_id,
                  void (* receive_confirm)(void),
                  void (* get_pack)(void **, MD_PACKET ** ),
                  void ** ident,
                  void (* put_pack)( MD_PACKET * ),
                  void (* status_indicate)(int ) )

{
    ts_id = ts_id; /* Avoids warning */
    lm_t_init( receive_confirm, get_pack, ident, put_pack, status_indicate );
}


/*
*****************************************************************************
* name          lm_m_t_get_status                                           *
*                                                                           *
* param. in     ts_id            Traffic Store  Identification              *
*               selector         Selects one of contants                    *
*               reset            Indicates whether to reset status or not   *
*                                                                           *
* param. out    status           Status flags defined by 'selector'         *
*                                                                           *
* return value  -                                                           *
*****************************************************************************
* description   WTB Message Status Retrieval                                *
*                                                                           *
*****************************************************************************
* history       1   95-02-28   BG Created                                   *
*****************************************************************************
*/

void lm_m_t_get_status ( unsigned short ts_id, unsigned short selector,
                         unsigned short reset, unsigned short *status )
{
    ts_id = ts_id; /* Avoids warning */
    lm_t_get_status( selector, reset, status );
}

/*
*****************************************************************************
* name          lm_m_t_send_request                                         *
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
* description   WTB Send Message Request                                    *
*                                                                           *
*****************************************************************************
* history       1   95-02-28   BG Created                                   *
*****************************************************************************
*/
void lm_m_t_send_request( unsigned short   ts_id,
                          unsigned char    src_dev,
                          unsigned char    dst_dev,
                          MD_PACKET      * packet,
                          int            * status   )
{
    ts_id = ts_id; /* Avoids warning */
    lm_t_send_request( src_dev, dst_dev, packet, status );
}

/*
*****************************************************************************
* name          lm_m_t_send_queue_flush                                     *
*                                                                           *
* param. in     ts_id            Traffic Store  Identification              *
*                                                                           *
* return value  -                                                           *
*****************************************************************************
* description   WTB Flush Send Queue                                        *
*                                                                           *
*****************************************************************************
* history       1   93-08-31   BG Created                                   *
*****************************************************************************
*/
void lm_m_t_send_queue_flush( unsigned short ts_id )
{
  ts_id = ts_id; /* Avoids warning */
  lm_t_send_queue_flush();
}


/*
*****************************************************************************
* name          lm_m_t_receive_poll                                         *
*                                                                           *
* param. in     ts_id            Traffic Store  Identification              *
*               source_device         source device                         *
*               destination_device    destination device                    *
*               packet                pointer on the packet to send         *
*               status                status of this request.               *
*                                                                           *
* return value  -                                                           *
*****************************************************************************
* description   WTB Receive Message Poll                                    *
*****************************************************************************
* history       1   93-08-31   BG Created                                   *
*****************************************************************************
*/

void lm_m_t_receive_poll( unsigned  short ts_id,
                          unsigned  char *src_dev,
                          unsigned  char *dst_dev,
                          MD_PACKET **packet, int *status )
{
    ts_id = ts_id; /* Avoids warning */
    lm_t_receive_poll( src_dev, dst_dev, packet, status );
}


#endif /* WTB support available */


#if (defined (O_BAP)) && (    (defined (NM_1VB_SIMPLE) ) \
                           || (defined (NM_1VB)        ) \
                           || (defined (NM_GW_SIMPLE)  ) \
                           || (defined (NM_GW_NO_TB)   ) \
                         )


   /* Necessary to model lm_m_v_... functions for BAP-based targets */


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
* description   BAP Initialization Funciton                                 *
*                                                                           *
*****************************************************************************
* history       1   95-02-26   BG Created                                   *
*****************************************************************************
*/

void lm_m_v_init( unsigned short ts_id,
                  void (* receive_confirm)(void),
                  void (* get_pack)(void **, MD_PACKET ** ),
                  void ** ident,
                  void (* put_pack)( MD_PACKET * ),
                  void (* status_indicate)(int ) )

{
    ts_id = ts_id; /* Avoids warning */
    lm_v_init( receive_confirm, get_pack, ident, put_pack, status_indicate );
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
* description   BAP Message Status Retrieval                                *
*                                                                           *
*****************************************************************************
* history       1   95-02-28   BG Created                                   *
*****************************************************************************
*/

void lm_m_v_get_status ( unsigned short ts_id, unsigned short selector,
                         unsigned short reset, unsigned short *status )
{
    ts_id = ts_id; /* Avoids warning */
    lm_v_get_status( selector, reset, status );
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
* description   BAP Send Message Request                                    *
*                                                                           *
*****************************************************************************
* history       1   95-02-28   BG Created                                   *
*****************************************************************************
*/
void lm_m_v_send_request( unsigned short   ts_id,
                          unsigned char    src_dev,
                          unsigned char    dst_dev,
                          MD_PACKET      * packet,
                          int            * status   )
{
    ts_id = ts_id; /* Avoids warning */
    lm_v_send_request( src_dev, dst_dev, packet, status );
}

/*
*****************************************************************************
* name          lm_m_v_send_queue_flush                                     *
*                                                                           *
* param. in     ts_id            Traffic Store  Identification              *
*                                                                           *
* return value  -                                                           *
*****************************************************************************
* description   BAP Flush Send Queue                                        *
*                                                                           *
*****************************************************************************
* history       1   93-08-31   BG Created                                   *
*****************************************************************************
*/
void lm_m_v_send_queue_flush( unsigned short ts_id )
{
  ts_id = ts_id; /* Avoids warning */
  lm_v_send_queue_flush();
}


/*
*****************************************************************************
* name          lm_m_v_receive_poll                                         *
*                                                                           *
* param. in     ts_id            Traffic Store  Identification              *
*               source_device         source device                         *
*               destination_device    destination device                    *
*               packet                pointer on the packet to send         *
*               status                status of this request.               *
*                                                                           *
* return value  -                                                           *
*****************************************************************************
* description   BAP Receive Message Poll                                    *
*****************************************************************************
* history       1   93-08-31   BG Created                                   *
*****************************************************************************
*/

void lm_m_v_receive_poll( unsigned  short ts_id,
                          unsigned  char *src_dev,
                          unsigned  char *dst_dev,
                          MD_PACKET **packet, int *status )
{
    ts_id = ts_id; /* Avoids warning */
    lm_v_receive_poll( src_dev, dst_dev, packet, status );
}

#endif /* O_BAP used for MVB support */
