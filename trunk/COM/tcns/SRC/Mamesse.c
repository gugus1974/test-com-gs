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
*						29.03.1996  Hil    adapted to new norm:
*												 use of new services
*												 read_messenger_status(),
* 												 write_messenger_control()
*                                    NOTE: these services are defined in
*												 RTP, Version 4.0
*                                    When using this services, you must
*											    adapt 'ma_main.c': AM_ADDR instead of
*												 AM_NET_ADDRESS is to use  !!! 
*
*
******************************************************************************/


/******************************************************************************
*
*   name:         mamesse.c
*
*   function:     Serve Request PDU's for Messenger Objects
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


#include <string.h>
#include "tcntypes.h"
#include "am_sys.h"
#ifdef O_GATEWAY
#include "am_group.h"  /* join/remove groups: am_is_member_of(), am_join_groups() */
#endif  
#include "tnm.h"

/* note: 'am_stadi.h' contains an old definition of AM_ST_ENTRY */
/* this old definition is not used here */

#define RTP_4

#ifdef RTP_4    /* when using RTP, Version 4.0, this switch must be set in 'tnm.h' */

extern unsigned long    am_messages_sent;
extern unsigned long    am_messages_received;
extern unsigned long    tm_packets_sent;
extern unsigned long    tm_packet_retries;
extern unsigned long    tm_multicast_retries;

#endif

/* should be stored in NSDB for future */
static char messenger_name[31] = "1234567890123456789012345678901";    /* exactly 31 characters */


#define ReadMessengerStatus      40
#define WriteMessengerControl    41
#define ReadFunctionDirectory    42
#define WriteFunctionDirectory   43
#define ReadStationDirectory     44
#define WriteStationDirectory    45
#define ReadGroupDirectory       46
#define WriteGroupDirectory      47

/*	NOTE: 0 <= service identifier <= 7, since 3 Bits are used */ 
/*
#define ReadNodeDirectory		 8
#define WriteNodeDirectory		 9
*/


int ma_MessengerObjects(struct TNMPDU *InPDU,struct TNMPDU * OutPDU,int * OutPDULen)
	 {

	 int        ReturnCode = OK;
	 int        len = 0;
	 AM_RESULT  res;

     switch (InPDU->SIF) {

		case ReadMessengerStatus:
		  {

#ifdef RTP_4
			  struct STR_AM_RD_MSNGR_STATUS msngr_status;
			  res = am_read_messenger_status( &msngr_status );
			  if (res != AM_OK) {
					return(MM_CMD_NOT_EXECUTED);
			  }
			  
			  len += ma_encodeString((char*)&OutPDU->Data[len],messenger_name);
			  len += ma_encodeUNSIGNED8(&OutPDU->Data[len],msngr_status.send_time_out);
			  len += ma_encodeUNSIGNED8(&OutPDU->Data[len],msngr_status.alive_time_out);
			  len += ma_encodeUNSIGNED8(&OutPDU->Data[len],msngr_status.ack_time_out);
			  len += ma_encodeUNSIGNED8(&OutPDU->Data[len],msngr_status.credit);
			  len += ma_encodeUNSIGNED8(&OutPDU->Data[len],0);   /* reserved */
			  len += ma_encodeUNSIGNED8(&OutPDU->Data[len],msngr_status.packet_size);
			  len += ma_encodeUNSIGNED8(&OutPDU->Data[len],msngr_status.instances);
			  len += ma_encodeUNSIGNED8(&OutPDU->Data[len],msngr_status.multicast_window);
			  len += ma_encodeUNSIGNED32(&OutPDU->Data[len],msngr_status.messages_sent);
			  len += ma_encodeUNSIGNED32(&OutPDU->Data[len],msngr_status.messages_received);
			  len += ma_encodeUNSIGNED32(&OutPDU->Data[len],msngr_status.packets_sent);
			  len += ma_encodeUNSIGNED32(&OutPDU->Data[len],msngr_status.packet_retries);
			  len += ma_encodeUNSIGNED32(&OutPDU->Data[len],msngr_status.multicast_retries);

			  *OutPDULen = len+2;
#else
			  return(MM_SIF_NOT_SUPPORTED);

#endif

		  };
		  break;

		case WriteMessengerControl:
		 {

#ifdef RTP_4

			UNSIGNED8 clear;
			struct STR_AM_WT_MSNGR_CTRL msngr_ctrl;

			len += ma_decodeString(messenger_name,(char*)&InPDU->Data[0]);

			msngr_ctrl.send_time_out     = ma_decodeUNSIGNED8(&InPDU->Data[len]);
			msngr_ctrl.alive_time_out    = ma_decodeUNSIGNED8(&InPDU->Data[len+1]);
			msngr_ctrl.ack_time_out      = ma_decodeUNSIGNED8(&InPDU->Data[len+2]);
			msngr_ctrl.credit            = ma_decodeUNSIGNED8(&InPDU->Data[len+3]);
			msngr_ctrl.packet_size       = ma_decodeUNSIGNED8(&InPDU->Data[len+5]);
			clear                        = ma_decodeUNSIGNED8(&InPDU->Data[len+6]);
			msngr_ctrl.multicast_window  = ma_decodeUNSIGNED8(&InPDU->Data[len+7]);

			/* clear counters, if desired */
			if(clear & 1)
				 am_messages_sent = 0;
			if(clear & 2)
				 am_messages_received = 0;
//			if(clear & 4)
//				 tm_packets_sent = 0;
//			if(clear & 8)
//				 tm_packet_retries = 0;
//			if(clear & 16)
//				 tm_multicast_retries = 0;

			res = am_write_messenger_control(&msngr_ctrl);
			if (res != AM_OK) {
				 return(NOTOK);
			}	 

#else
			return(MM_SIF_NOT_SUPPORTED);

#endif			

		 };
		 break;

		case ReadFunctionDirectory:
		 {
			unsigned char station;
			int i;

			len += ma_encodeUNSIGNED8(&OutPDU->Data[0],0);     /* padding zero  */
			len += ma_encodeUNSIGNED8(&OutPDU->Data[1],254);   /* fixed forever */
			for (i=1;i<255;i++) {
				 am_get_dir_entry((unsigned char)i,&station);
				 len += ma_encodeUNSIGNED8(&OutPDU->Data[i+2],(unsigned char)i);
				 len += ma_encodeUNSIGNED8(&OutPDU->Data[i+3],station);
             }
		    *OutPDULen = len+2;

		 };
		 break;


		case WriteFunctionDirectory:
		 {
            int i;
			AM_DIR_ENTRY dir;

			int clear_directory  = ma_decodeUNSIGNED8(&InPDU->Data[0]); /* for fun only */
			int nb_functions     = ma_decodeUNSIGNED8(&InPDU->Data[1]);

			for(i=0;i<nb_functions;i++) {
				dir.function    = ma_decodeUNSIGNED8(&InPDU->Data[(i*2)+2]);
				dir.device      = ma_decodeUNSIGNED8(&InPDU->Data[(i*2)+3]);
				am_insert_dir_entries(&dir,(unsigned int)1);
			}
			/* default response */
		 };
		 break;


		case ReadStationDirectory:
		 {
#ifdef RTP_4
			 AM_ST_ENTRY entry;
			 int  i;
			 len += ma_encodeUNSIGNED8(&OutPDU->Data[0],0);     /* padding zero  */
			 len += ma_encodeUNSIGNED8(&OutPDU->Data[1],254);   /* fixed forever */

			 for (i=1;i<255;i++) {
				entry.final_station = (unsigned char)i;
				(void) am_st_read(&entry,(unsigned char)1);
				len += ma_encodeUNSIGNED8(&OutPDU->Data[(i*6)+2],entry.final_station);
				len += ma_encodeUNSIGNED8(&OutPDU->Data[(i*6)+3],entry.next_station);
				len += ma_encodeUNSIGNED32(&OutPDU->Data[(i*6)+4],entry.link_address);
				}

			 *OutPDULen = len+2;
#else
			 return(MM_SIF_NOT_SUPPORTED);
#endif			 

		  };
		  break;


		case WriteStationDirectory:
		  {
#ifdef RTP_4
			 AM_ST_ENTRY entry;
			 int  i;
			 int clear_directory  = ma_decodeUNSIGNED8(&InPDU->Data[0]);
			 int nb_stations      = ma_decodeUNSIGNED8(&InPDU->Data[1]);

			 for(i=0;i<nb_stations;i++) {
				  entry.final_station   = ma_decodeUNSIGNED8(&InPDU->Data[(i*4)+2]);
				  entry.next_station    = ma_decodeUNSIGNED8(&InPDU->Data[(i*4)+3]);
				  entry.link_address    = ma_decodeUNSIGNED16(&InPDU->Data[(i*4)+4]);
				  am_st_write(&entry,(unsigned char)1);
				 }
#else
			 return(MM_SIF_NOT_SUPPORTED);
#endif			 

		  };
		  break;


		case ReadGroupDirectory:
		 { 
#ifdef O_GATEWAY			 
			int i;
			long tmp = 1;
			long upper_32bits = 0;
			long lower_32bits = 0;

			/* lower 32 bits */
			for(i=0;i<32;i++)  if(am_is_member_of((unsigned char)i) != 0) lower_32bits |= (tmp<<i);

			/* upper 32 bits */
			for(i=32;i<64;i++) if(am_is_member_of((unsigned char)i) != 0) upper_32bits |= (tmp<<i);

			ma_encodeUNSIGNED32(&OutPDU->Data[0],upper_32bits);
			ma_encodeUNSIGNED32(&OutPDU->Data[4],lower_32bits);
			*OutPDULen = 10; 
#else
			return(MM_SIF_NOT_SUPPORTED);
#endif			 		

		 };
		 break;


		case WriteGroupDirectory:    
		 {
#ifdef O_GATEWAY		 
			int i;
			long tmp = 1;
			long upper_32bits = ma_decodeUNSIGNED32(&InPDU->Data[0]);
			long lower_32bits = ma_decodeUNSIGNED32(&InPDU->Data[4]);

			/* lower 32 bits */
			for(i=0;i<32;i++)  if(lower_32bits & (tmp<<i)) am_join_groups((unsigned char*)&i,(unsigned short)1);

			/* upper 32 bits */
			for(i=32;i<64;i++) if(upper_32bits & (tmp<<i)) am_join_groups((unsigned char*)&i,(unsigned short)1);

			/* default response */
#else
			return(MM_SIF_NOT_SUPPORTED);
#endif			 					
		 };
		 break;

		default: 
		{
         return(MM_SIF_NOT_SUPPORTED);
        }
    }
    return(ReturnCode);
}
