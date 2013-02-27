/******************************************************************************
*         Copyright (C) Siemens AG 1994 All Rights Reserved                   *
*******************************************************************************
*
*    project:     TCN Train Communication Network
*                 JDP Joint Development Project
*
*    title:       Network Management Agent
*
*
*******************************************************************************
*
*    description: the agent part of TRAIN NETWORK MANAGEMENT
*
*    department:  VT 596 Communication
*    version:     0.0
*    language:    ANSI-'C'
*    system:
*
*    author:      E. Renner
*    history:     date        sign   remark
*                 17.10.1994  Re     created
*                 29.03.1996  Hil    adapted to new norm
*
******************************************************************************/


/******************************************************************************
*
*   name:         ma_wtb.c
*
*   function:     Serve Request PDU's for WTB Objects
*
*
*
*   input:        -
*
*   output:       -
*
*   return value: -
*
******************************************************************************/



#include <stdio.h>  

#include "tcntypes.h"
#include "tnm.h"
#include "lc_sys.h"
#include "bmi.h"

/* translated only when mode = O_GATEWAY is used */
#ifdef O_GATEWAY     

#define ReadWTBStatus         20
#define WriteWTBControl       21
#define ReadWTBNodes          22
#define ReadTopography        24

#define NM_RESET_NODE            0
#define NM_CONFIGURE_NODE        2
#define NM_SET_SLAVE             4
#define NM_SET_WEAK              8
#define NM_SET_STRONG			 16
#define NM_START_NAMING          32
#define NM_REMOVE                64
#define NM_INHIBIT				 128
#define NM_ALLOW                 256
#define NM_SET_SLEEP			 512
#define NM_CANCEL_SLEEP          1024


static int ma_createReadWTBStatusResponsePDU(struct TNMPDU *PDU, UNSIGNED8 link_id, Type_WTBStatus *wtb_status)
{

//  PDU->TNM_code = TNM;
//	PDU->SIF      = 20;
	ma_encodeUNSIGNED8(&PDU->Data[0],link_id);
	ma_encodeUNSIGNED8(&PDU->Data[1],wtb_status->node_address);
	ma_encodeUNSIGNED8(&PDU->Data[2],wtb_status->WTB_hardware);
	ma_encodeUNSIGNED8(&PDU->Data[3],wtb_status->WTB_software);
	ma_encodeUNSIGNED8(&PDU->Data[4],wtb_status->link_layer_state);
	ma_encodeUNSIGNED8(&PDU->Data[5],0);
	/*ma_encodeUNSIGNED32(&PDU->Data[6],topography_counter);     how to determine? */
	ma_encodeUNSIGNED8(&PDU->Data[10],wtb_status->node_orient);
	ma_encodeUNSIGNED8(&PDU->Data[11],wtb_status->node_strength);
	ma_encodeUNSIGNED8(&PDU->Data[12],wtb_status->node_descriptor.node_frame_size);
	ma_encodeUNSIGNED8(&PDU->Data[13],wtb_status->node_descriptor.node_period);
	ma_encodeUNSIGNED8(&PDU->Data[14],wtb_status->node_descriptor.node_key.node_type);
	ma_encodeUNSIGNED8(&PDU->Data[15],wtb_status->node_descriptor.node_key.node_version);
	/*ma_encodeUNSIGNED8(&PDU->Data[16],node_status);  how to determine ? */
	ma_encodeUNSIGNED8(&PDU->Data[17],wtb_status->node_strength);
	/* information missing:
	ma_encodeUNSIGNED32(&PDU->Data[18],line_a1_errors);
    ma_encodeUNSIGNED32(&PDU->Data[22],line_a2_errors);
	ma_encodeUNSIGNED32(&PDU->Data[26],line_b1_errors);
	ma_encodeUNSIGNED32(&PDU->Data[30],line_b2_errors);
	*/

    return(36);
}




int ma_WTBObjects(struct TNMPDU *InPDU,struct TNMPDU * OutPDU,int * OutPDULen)
{

	int  ReturnCode = OK;
	UNSIGNED8 link_id;
	BITSET16  res;
	int i;

    switch (InPDU->SIF) {

	case ReadWTBStatus:
	{

		/*
		Type_WTBStatus wtb_status;
		link_id = ma_decodeUNSIGNED8(&InPDU->Data[0]);
		res = ls_t_GetStatus(&wtb_status);
		*/

		return(MM_SIF_NOT_SUPPORTED);
	
		/*  *OutPDULen = ma_createReadWTBStatusResponsePDU(OutPDU, link_id, &wtb_status); */
	};
	break;


	case WriteWTBControl:
	{
		UNSIGNED16 command = ma_decodeUNSIGNED16(&InPDU->Data[2]);
		switch (command){
		  case NM_RESET_NODE:
	    	{
              ls_t_Reset();
			};
			break;

		  case NM_CONFIGURE_NODE:
			{
              return(MM_SIF_NOT_SUPPORTED);
			};
			break;

		  case NM_SET_SLAVE:
			{
			  ls_t_SetSlave();
			};
			break;

		  case NM_SET_WEAK:
			{
			  ls_t_SetWeak();
			};
			break;

		  case NM_SET_STRONG:
			{
			  ls_t_SetStrong();
			};
			break;

		  case NM_START_NAMING:
			{
			  ls_t_StartNaming();
			};
			break;
		
		  case NM_REMOVE:
			{
		      ls_t_Remove();
			};
			break;

		  case NM_INHIBIT:
			{
			  ls_t_Inhibit();
			};
			break;
		
		  case NM_ALLOW:
			{
			  ls_t_Allow();
			};
			break;

		  case NM_SET_SLEEP:
			{
			  ls_t_SetSleep();
			};
			break;

		  case NM_CANCEL_SLEEP:
			{
			  ls_t_CancelSleep();
			};
			break;

		  default:
			{
			  return(MM_SIF_NOT_SUPPORTED);
			}
		}
	 };
	 break;


	case ReadWTBNodes:
	 {
		Type_Topography wtb_topography;
		Type_WTBNodes   wtb_nodes;
		UNSIGNED8 nb_nodes;
		UNSIGNED8 node_addr;
	
		res = ls_t_GetTopography(&wtb_topography);
		if (res != L_OK){
			 return(MM_CMD_NOT_EXECUTED);
		}

			nb_nodes  = wtb_topography.number_of_nodes;
			node_addr = wtb_topography.node_address;

			res = ls_t_GetWTBNodes(&wtb_nodes);
			if (res != L_OK){
				 return(MM_CMD_NOT_EXECUTED);
			}
			
			link_id = ma_decodeUNSIGNED8(&InPDU->Data[0]);
			ma_encodeUNSIGNED8(&OutPDU->Data[0],link_id);
			ma_encodeUNSIGNED8(&OutPDU->Data[1],node_addr);
			ma_encodeUNSIGNED8(&OutPDU->Data[2],0);   /* reserved */
			ma_encodeUNSIGNED8(&OutPDU->Data[3],(nb_nodes & 0x3F) << 1);
			ma_encodeUNSIGNED8(&OutPDU->Data[4],wtb_nodes.bottom_address);
			ma_encodeUNSIGNED8(&OutPDU->Data[5],wtb_nodes.top_address);
			/*** node status list ***/
			for (i=1;i<nb_nodes;i++) {
					 ma_encodeUNSIGNED8(&OutPDU->Data[(i*2)+4],wtb_nodes.node_status_list[i-1].node_report);
					 ma_encodeUNSIGNED8(&OutPDU->Data[(i*2)+5],wtb_nodes.node_status_list[i-1].user_report);
				 }
			*OutPDULen = 2*nb_nodes + 8;
		 };
		 break;



      case ReadTopography:
		 {

			UNSIGNED8 tmp = 0;
			UNSIGNED8 nb_nodes;
			Type_Topography wtb_topography;
			
			res = ls_t_GetTopography(&wtb_topography);
			if (res != L_OK){
				 return(MM_CMD_NOT_EXECUTED);
			}	 

			link_id = ma_decodeUNSIGNED8(&InPDU->Data[0]);

			ma_encodeUNSIGNED8(&OutPDU->Data[0],link_id);
			ma_encodeUNSIGNED8(&OutPDU->Data[1],wtb_topography.node_address);
			ma_encodeUNSIGNED8(&OutPDU->Data[2],wtb_topography.individual_period);

			nb_nodes = wtb_topography.number_of_nodes;
			tmp |= (UNSIGNED8)wtb_topography.is_strong << 7;
			tmp |= ((nb_nodes & 0x3F) << 1);

			ma_encodeUNSIGNED8(&OutPDU->Data[3],tmp);
			ma_encodeUNSIGNED8(&OutPDU->Data[4],wtb_topography.bottom_address);
			ma_encodeUNSIGNED8(&OutPDU->Data[5],wtb_topography.topo_counter);
			/*** node-key list ***/
			for (i=1;i<nb_nodes;i++) {
/* <ATR:02> */
#ifndef UMS
				  ma_encodeUNSIGNED8(&OutPDU->Data[(i*2)+4],wtb_topography.NK[i-1].node_type);
				  ma_encodeUNSIGNED8(&OutPDU->Data[(i*2)+5],wtb_topography.NK[i-1].node_version);
#else
				  ma_encodeUNSIGNED8(&OutPDU->Data[(i*2)+4],0);
				  ma_encodeUNSIGNED8(&OutPDU->Data[(i*2)+5],0);
#endif
              }

			*OutPDULen = (nb_nodes*2) + 8;

		 };
		 break;


		default:
		 {
			return(MM_SIF_NOT_SUPPORTED);
		 }
    }
	 return(ReturnCode);
}  


#endif
