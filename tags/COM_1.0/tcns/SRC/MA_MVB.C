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
*         			29.03.1996  Hil    adapted to new norm
*
*
******************************************************************************/


/******************************************************************************
*
*   name:         ma_mvb.c
*
*   function:     Serve Request PDU's for MVB Objects
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
#include "bax_incl.h"   

#define ReadMVBStatus          10
#define WriteMVBControl        11
#define ReadMVBDevices         12
#define WriteMVBConfiguration  13


static int ma_createReadMVBStatusResponsePDU(struct TNMPDU *PDU, UNSIGNED8 link_id, struct LC_STR_MVB_STATUS *p_mvb_status)
{
	int len = 0;

//	PDU->TNM_code = TNM;
//	PDU->SIF      = 10;
	ma_encodeUNSIGNED8(&PDU->Data[0],link_id);
	ma_encodeUNSIGNED8(&PDU->Data[1],0);
	ma_encodeUNSIGNED16(&PDU->Data[2],(p_mvb_status->link_address)& 0x0FFF);   /* lower 12 bits */
	len += ma_encodeString((char*)&PDU->Data[4],(char*)&p_mvb_status->hardware_id);
	len += ma_encodeString((char*)&PDU->Data[len+4],(char*)&p_mvb_status->software_id);
	ma_encodeUNSIGNED16(&PDU->Data[len+4],p_mvb_status->device_status_word);
	ma_encodeUNSIGNED8(&PDU->Data[len+6],0);
	ma_encodeUNSIGNED8(&PDU->Data[len+7],p_mvb_status->t_reply);
	ma_encodeUNSIGNED32(&PDU->Data[len+8],p_mvb_status->lineA_errors);
	ma_encodeUNSIGNED32(&PDU->Data[len+12],p_mvb_status->lineB_errors);

    return(len+18);
}



extern struct BA_STR_DIAGNOSIS *ba_status;

int ma_MVBObjects(struct TNMPDU *InPDU,struct TNMPDU * OutPDU,int * OutPDULen)
{

	 int ReturnCode = OK;
	 int len = 0;
	 int res, i;

 switch (InPDU->SIF) {

	case ReadMVBStatus:
	{
		 struct LC_STR_MVB_STATUS mvb_status;
		 UNSIGNED8 link_id = ma_decodeUNSIGNED8(&InPDU->Data[0]);
		 res = lc_read_mvb_status(link_id,&mvb_status);
		 if (res == LC_REJECT) {
			  return(MM_CMD_NOT_EXECUTED);
		 }
	     *OutPDULen = ma_createReadMVBStatusResponsePDU(OutPDU,link_id,&mvb_status);
    };
	break;


	case WriteMVBControl:
	 {

		 int link_id         =  ma_decodeUNSIGNED8(&InPDU->Data[0]);
		 int device_address  = (ma_decodeUNSIGNED16(&InPDU->Data[2]) & 0x0fff);
		 int t_reply         =  ma_decodeUNSIGNED16(&InPDU->Data[4]);
		 int command         = (ma_decodeUNSIGNED16(&InPDU->Data[6]) & 0x00ff);

		 /* mask upper 4 command bits; the command may be: */
		 /* switch to line A (SLA), switch to line B (SLB) */
		 /* clear error counters on line A/B (CLA, CLB) */

		 command = command & 0x0078;
		 res = lc_mvb_line(link_id, command);
		 if (res == LC_REJECT) {
			  ReturnCode = MM_CMD_NOT_EXECUTED;
		 }	  
		 /*  NOTE: 'WriteMVBControl' not complete */
	};
	break;


	case ReadMVBDevices:
	{
	    unsigned short address;			/* address of this device */
	    unsigned short nr_devices = 0;	/* number of devices      */

		int link_id = ma_decodeUNSIGNED8(&InPDU->Data[0]);

	    if (link_id != MVB_TS_ID || lc_m_get_device_address(link_id, &address) != LC_OK)
	    	ReturnCode = NOTOK;
	    else {

			/* save the bus id and device address */
			OutPDU->Data[len++] = link_id;
			OutPDU->Data[len++] = 0;
			len += ma_encodeUNSIGNED16(&OutPDU->Data[len], address);

			/* skip the number of devices (will be filled later on) */
			len += 2;
				
			/* dump the devices counting them */
			if (ba_status) {
					
				/* class 2+ devices */
				for (i = 1; i < 256; i++) {
					if (ba_status->p_scan_rslt_cl_2p->sn_rslt[i].rsp == 0) {
						len += ma_encodeUNSIGNED16(&OutPDU->Data[len], i);
						len += ma_encodeUNSIGNED16(&OutPDU->Data[len],
							ba_status->p_scan_rslt_cl_2p->sn_rslt[i].dsw);
						nr_devices++;
					}
				}

				/* class 1 devices */
				for (i = 16; i < 256; i++) {
					if (ba_status->p_scan_rslt_cl_1->sn_rslt[i].rsp == 0) {
						len += ma_encodeUNSIGNED16(&OutPDU->Data[len], i*16);
						len += ma_encodeUNSIGNED16(&OutPDU->Data[len],
							ba_status->p_scan_rslt_cl_1->sn_rslt[i].dsw);
						nr_devices++;
					}
				}
			}

			/* insert the number of devices */
			ma_encodeUNSIGNED16(&OutPDU->Data[4], nr_devices);

			/* set the total len */
			*OutPDULen = len + 2;
		}
	}
	break;


	case WriteMVBConfiguration:
	{
		  ReturnCode = MM_SIF_NOT_SUPPORTED;
	};
	break;

    default: 
	{
	      return(MM_SIF_NOT_SUPPORTED);
	}
 
 } /* end 'switch' */

    return(ReturnCode);
}
