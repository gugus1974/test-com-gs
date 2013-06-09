/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> pdjdp_my_device always set to 0 for PDM compatibility                               */
/* <ATR:02> disable feature for MS 10 implemented: do not receive, transmit invalid data        */
/* <ATR:03> protection against frame requests before initialization (with UMS it could happen)  */
/* <ATR:04> copy only the bytes actually transferred, skipping null frames                      */
/* <ATR:05> do not import frames with a FTF changed from the previous one                       */
/*==============================================================================================*/

/*
HEADER
*/
/*****************************************************************
*   AAA   BBBB   BBBB   |                                        *
*  A   A  B   B  B   B  |                                        *
*  AAAAA  BBBB   BBBB   |                                        *
*  A   A  B   B  B   B  |                                        *
*  A   A  BBBB   BBBB   |       TRAIN COMMUNICATION NETWORK      *
*                       |                                        *
*  ASEA  BROWN  BOVERI  |      Copyright 1991 by ABB Henschel    *
*-----------------------+----------------------------------------*
*                                                                *
* function      : processdata handler called by the MAC-modul,   *
*                 serving a traffic store structure as defined   *
*                 for MVB                                        *
*                                                                *
* date          : 18.02.93                                       *
* author        : P. Gehring                                     *
* released      : XX.XX.XX                                       *
* compiler      : ANSI-C                                         *
*                                                                *
*----------------------------------------------------------------*
*                          description                           *
*                                                                *
*  all processdata transmitted on the bus is available in a      *
*  common data-aerea called trafficstore. For JDP the traffic-   *
*  store-structure as defined for the MVB is used                *
*                                                                *
*                                                                *
*----------------------------------------------------------------*
* file                 : PD_JDP.X                                *
*----------------------------------------------------------------*
* called functions     : vgl. PD_JDP.H                           *
*                                                                *
*                                                                *
*----------------------------------------------------------------*
* used functions       : vgl. include directives                 *
*                                                                *
*                                                                *
*----------------------------------------------------------------*
* used files           : vgl. include directives                 *
*                                                                *
*                                                                *
*----------------------------------------------------------------*
* author: Peter Gehring WAG/NTL2  date: 18.02.93  version: 1.0   *
*                                                                *
* changes:                                                       *
*================================================================*
*----------------------------------------------------------------*
* author: Gg                      date: 18.02.93   version: 1.0  *
* description :                                                  *
*                                                                *
*----------------------------------------------------------------*
*----------------------------------------------------------------*
* author: Gg                      date: 18.02.93   version: 1.1  *
* description :                                                  *
*----------------------------------------------------------------*
* author: Gg                      date: 18.02.93   version: 1.0  *
* description :                                                  *
*               data consistency over a whole WTB-Port           *
*----------------------------------------------------------------*
*----------------------------------------------------------------*
* author: Cl                      date: 10.03.53   version: 1.2  *
* description :                                                  *
*               dl_enable_pdi: 1 diag entry removed,             *
*                              1 diag entry changed              *
*----------------------------------------------------------------*
*                                                                *
*----------------------------------------------------------------*
* 4.0-5  01.08.96  cs  process data in poll frames               *
* 4.0-6  01.08.96  cs  synchronisation of process data           *
* 4.1-18 26.09.96  cs  compiler problem: additional brakets      *
*                      substitution of "p_dest_dev"              *
* 4.1-20 26.09.96  cs  further portability changes               *
* 4.1-21 15.10.96  cs  send buffer is only copied if not empty   *
* 4.1-44 27.01.97  cs  constant for diag_entry defined -> diag.h *
*****************************************************************/
/* #include tcn.h #CL removed 10.02.95 */
#ifndef COCO_H
#include "coco.h" /* #Br 17.02.95 */
#endif
#include "wtb.h"         /*4.0-5*/
#include "wtb_pd.h"      /*4.0-5*/
#include "diagwtb.h"
#include "pd_jdp.h"

/* <ATR:02> */
#ifndef UMS
#include "bmi.h"
extern Type_Configuration mac_configuration;	/* MAC configuration structure used to get our NodeDesc */
extern Type_Topography    mac_topo;			 	/* MAC topography                                       */
unsigned char             pd_jdp_topo_counter;	/* if different from the MAC one the WTB TS is disabled */
#endif

type_mvb_descriptor *pdjdp_ts_descriptor; /* pointer to the traffic store descriptor */
unsigned char       pdjdp_my_device;      /* my own device address, result of the train inauguration process */
type_idu            send_frame;           /* the memory of the real WTB-PD-frame */


/*
void dl_system_init_pdi(void)
*/
void dl_system_init_pdi(void)
{
  pdjdp_ts_descriptor = (type_mvb_descriptor *) 0x00;
  pdjdp_my_device     = 0x00;
}


/*
void dl_enable_pdi (void *        poi2dpr,
               unsigned char my_own_device,
               unsigned char no_rec_ports,
               unsigned char no_send_ports,
               unsigned char max_pd_data)
*/
void dl_enable_pdi (
               void          *poi2dpr,
               unsigned char my_own_device,
               unsigned char no_rec_ports,
               unsigned char no_send_ports,
               unsigned char max_pd_data)
{
  /* initialize the traffic store descriptor pointer */
  pdjdp_ts_descriptor = (type_mvb_descriptor *) poi2dpr;

  /* <ATR:01> pdjdp_my_device = my_own_device; */

  /* check the descriptor parameter */
  if ( pdjdp_ts_descriptor->ts_type !=  LP_TS_TYPE_D )
  {
    diagwtb_entry(DIAG_PDH_ERR,0,0,0,(void*) 0);
  }
  if ( pdjdp_ts_descriptor->prt_addr_max <  WTB_PORT_MAX ) /* 4.0-5: instead of 0x3F */
                                                           /* 4.1-43: < instead of != */
  {
    diagwtb_entry(DIAG_PDH_ERR,1,0,0,(void*) 0);
  }
}


/*
void pd_receive_indicate (type_idu *poi2idu)
*/
/* !!! MODIFIED: No more prt_indx division by 16 !!! */
/* !!! MODIFIED: port structure modified !!! */
/* !!! MODIFIED: location of "size" now member of PCS structure !!! */
/* !!! MODIFIED: PCS structure now 16 bytes !!! */
/* !!! MODIFIED: function parameter now from type_idu !!! */
/* !!! MOFIFIED: TACK now -1 (in preparation for MVBC type structure) */
/* !!! MODIFIED: structure copy of 128 bytes, size independant */

void pd_receive_indicate (type_idu *poi2idu)
{
  /* declarations */
  unsigned short          act_port_index;
  struct STR_TB_PCS      *p_pcs;
  struct STR_TB_PRT      *p_prt;
  struct STR_TB_PRT_PGE  *p_dck_pge;
  type_idu *              p_2_idu = (type_idu *) poi2idu;
  int                     write_pge;
  unsigned char           port_addr;

  /* <ATR:03> */
  if (!pdjdp_ts_descriptor) return;

  /* <ATR:04> */
  if (!p_2_idu->size) return;

  /* <ATR:02> */
#ifndef UMS
  if (pd_jdp_topo_counter != mac_topo.topo_counter) return;
#endif

  /* map logical port address and physical device address */
  /*4.0-5..*/
  if (p_2_idu->dd != WTB_ADR_BROADCAST)
  {
    /* this is process_data in a PD_Request frame */
    port_addr = WTB_PORT_PIGGY_OFFSET;
  }
  else
  {
    port_addr = p_2_idu->sd;
  }
  act_port_index = (pdjdp_ts_descriptor->pb_pit)[port_addr];
  /*..4.0-5*/
  p_pcs = (struct STR_TB_PCS *) pdjdp_ts_descriptor->pb_pcs;
  p_pcs = p_pcs + act_port_index;
  p_prt = (struct STR_TB_PRT *) pdjdp_ts_descriptor->pb_prt;
  p_prt = p_prt + act_port_index;
  p_pcs->size = p_2_idu->size;     /* new feature: save data size in PCS */
  write_pge = p_pcs->page == 0 ? 1 : 0;

  /* <ATR:05> */
  p_dck_pge = &(p_prt->pge[p_pcs->page]);
  if (p_dck_pge->tb_dck[0] != p_2_idu->data.tb_dck[0] ||
      p_dck_pge->tb_dck[1] != p_2_idu->data.tb_dck[1]) return;

  p_dck_pge = &(p_prt->pge[write_pge]);

  /* <ATR:04> */
  memcpy(p_dck_pge, &p_2_idu->data, p_2_idu->size);
//*p_dck_pge = p_2_idu->data;         /* !!! structure copy, 128 bytes */

  p_pcs->tack = -1;                     /* set sink-time-counter*/
  p_pcs->page++;                        /* set valid page bit   */
  /*4.0-6..*/
  /* testing if there is a data_set subscription */
  if ((p_pcs->function != NULL) && (p_2_idu->size != 0) )
  {
    p_pcs->function(p_pcs->instance);
  }
  /*..4.0-6*/
}


/*
type_idu *   pd_request_to_send         (unsigned char destination_device)
*/
/* !!! MODIFIED: type of function value modified to type_idu !!! */
/* !!! MODIFIED: port structure modified !!! */
/* !!! MODIFIED: location of "size" now member of PCS structure !!! */
/* !!! MODIFIED: PCS structure now 16 bytes !!! */
/* !!! MODIFIED: function parameter now from type_idu !!! */
/* !!! MODIFIED: structure copy of 128 bytes, size independant */

type_idu *   pd_request_to_send         (unsigned char destination_device)
{
  /* local declarations */
  type_idu               *p_ret           = NULL  /*4.1-21*/;
  struct STR_TB_PCS      *p_pcs;
  struct STR_TB_PRT      *p_prt;
  struct STR_TB_PRT_PGE  *p_dck_pge;
  int                     read_pge;
  unsigned short          act_port_index;

  /* <ATR:03> */
  if (!pdjdp_ts_descriptor) {
  	send_frame.size = 0;
  	return &send_frame;
  }

  /* <ATR:02> */
#ifndef UMS
if (pd_jdp_topo_counter != mac_topo.topo_counter && destination_device == WTB_ADR_BROADCAST) {
  	int i;
  	send_frame.size           = mac_configuration.node_descriptor.node_frame_size;
  	send_frame.data.tb_dck[0] = mac_configuration.node_descriptor.node_key.node_type;
  	send_frame.data.tb_dck[1] = mac_configuration.node_descriptor.node_key.node_version;
  	for (i = 2; i < send_frame.size; i++) send_frame.data.tb_dck[i] = 0xFF;
  	return &send_frame;
  }
#endif

  /*4.0-5..*/
  /* map logical port address and physical device address */
  if (destination_device !=  WTB_ADR_BROADCAST )
  {
    act_port_index = (pdjdp_ts_descriptor->pb_pit)[ WTB_PORT_PIGGY_OFFSET + destination_device ];  /*4.0-6*/
  }
  else
  {
    act_port_index = (pdjdp_ts_descriptor->pb_pit)[ pdjdp_my_device ];  /*4.0-6: instead of 0x00*/
  }
  p_pcs = (struct STR_TB_PCS *) pdjdp_ts_descriptor->pb_pcs;
  p_pcs = p_pcs + act_port_index;
  p_prt = (struct STR_TB_PRT *) pdjdp_ts_descriptor->pb_prt;
  p_prt = p_prt + act_port_index;
  if ((send_frame.size = p_pcs->size) != 0)       /*4.1-21*/
  {
    read_pge = p_pcs->page;
    p_dck_pge = &(p_prt->pge[read_pge]);
    send_frame.data = *p_dck_pge; /* copy data from TS--->HDLC */
    p_ret = &send_frame;
  }
  return (p_ret);
}


/*
void pd_send_confirm (void *poi2idu)
*/
void pd_send_confirm (void *poi2idu)
{
  /*4.0-6..*/
  unsigned short     send_port_index;    /*4.1-20:type*/
  struct STR_TB_PCS  *p_pcs;             /* port constrol structure */
  unsigned char      port_addr;
  if (poi2idu == NULL)
  {
    return;
  }
  if (((type_idu*)poi2idu)->dd != WTB_ADR_BROADCAST)            /*4.1-18*/
  {
    /* for process data in poll frames */
    port_addr = ((type_idu*)poi2idu)->dd + WTB_PORT_PIGGY_OFFSET; /*4.1-18*/
  }
  else
  {
     port_addr = pdjdp_my_device;
  }
  send_port_index = (pdjdp_ts_descriptor->pb_pit)[port_addr];
  p_pcs = (struct STR_TB_PCS *) pdjdp_ts_descriptor->pb_pcs;
  p_pcs = p_pcs + send_port_index;
  /* testing if there is a data_set subscription */
  if ((p_pcs->function != NULL) && ( ((type_idu*)poi2idu)->size != 0) ) /*4.1-18*/
  {
    p_pcs->function(p_pcs->instance);
  }
  /*..4.0-6*/
}


/*
void pd_actualize_time(void)
*/
void pd_actualize_time(void)
{
  /* void */
}


/*
LP_RESULT ds_t_subscribe(...)               4.0-6
*/
/*4.0-6*/
LP_RESULT ds_t_subscribe
                  ( DS_NAME*    data_set  /* name of the data_set */
                  , DS_EVENT    function  /* subscribed function */
                  , UNSIGNED16  instance  /* application instance */
                  )
/* purpose:   enters a subscription request into the port control structure
 * returns:   L_RANGE          if <data_set> is invalid
 *            L_OK             if the function has been executed correctly
 * remark:    a WTB subscription is automatically being erased after
 *            a train bus inauguration
 */
{
  unsigned short         port_index;    /*4.1-20*/
  struct STR_TB_PCS      *p_pcs;
  port_index = (pdjdp_ts_descriptor->pb_pit)[data_set->ds_port_addr  ];
  p_pcs = (struct STR_TB_PCS *) pdjdp_ts_descriptor->pb_pcs;
  p_pcs = p_pcs + port_index;
  p_pcs->instance = instance;
  p_pcs->function = function;
  return LP_OK;
}


/*
LP_RESULT ds_t_unsubscribe(...)             4.0-6
*/
/*4.0-6*/
LP_RESULT ds_t_unsubscribe
                  ( DS_NAME*    data_set  /* name of the data_set */
                  )
/* purpose:   removes a data_set from the subscription list
 * returns:   LP_OK                   if the subscription has been successful
 *            LP_SUBSCRIPTION_UNKNOWN if the subscription does not exist
 */
{
  unsigned short         port_index;      /*4.1-20*/
  struct STR_TB_PCS      *p_pcs;          /* port control structure */
  port_index = (pdjdp_ts_descriptor->pb_pit)[data_set->ds_port_addr  ];
  p_pcs = (struct STR_TB_PCS *) pdjdp_ts_descriptor->pb_pcs;
  p_pcs = p_pcs + port_index;
  p_pcs->function = NULL;
  p_pcs->instance = 0;
  return LP_OK;
}

