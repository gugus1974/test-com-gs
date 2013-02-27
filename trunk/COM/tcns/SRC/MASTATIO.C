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
*                 04.10.1994  Re     created
*                 29.03.1996  Hil    adapted to new norm
*                                    'ma_PIL_gettime()' and 'ma_PIL_settime()'
*                                    replaced by 'get_sys_time()'
*                                    service 'read_inventory' modified
*                                    'STATIC' replaced by 'static'
*
******************************************************************************/


/******************************************************************************
*
*   name:         ma_station.c
*
*   function:     Serve Request PDU's for Station Objects
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


#include "tcntypes.h"
#include "lc_sys.h"
#include "am_sys.h"   

#include "tnm.h" /* think for OK */

#ifdef O_166
#include "c165_hw.h"    /* get_sys_time() */
#elif defined O_DOS        
#include "pi_sys.h"   
#undef OK
#elif defined O_W31
#include "pi_sys.h"
#undef OK
#else
/* <ATR:02> */
// #error get_sys_time
#include "pi_sys.h"
#endif

/* service identifiers */
#define ReadStatus            0
#define WriteControl          1
#define ReadInventory         2
#define Reserve               3
#define ReadDescriptor        4
#define ReadLinksDescriptor   6	  /* not supported at the moment */
#define WriteLinksDescriptor  7	  /* not supported at the moment */

#define RESERVE   1
#define RELEASE   2
#define RESTART   3
#define OVERWRITE 1

#define STATION_RESERVED  1
#define SER_MASK_BIT  1  /* 17.02.97 mask-bit for 'station reserved' defined (K. Hilbrich) */



/* Station Control (may be stored in nonvolatile memory	*/
/* or in NSDB for future)  **/


static char agent_version[] = {"Ansaldo TCN Management, 01/99"};

/* <ATR:03> */
// static char manufact_name[] = {"Siemens AG VT 596 "};
// static char device_type[]   = {"Prototype"};
// static char station_name[31] = "1234567890123456789012345678901";
// static char station_identifier = 0;
char *manufact_name;
char *device_type;
char *station_name;
char station_identifier;

static unsigned short own_adr;
static unsigned short station_status_word;


/** Agent Reservation Management **/


static long manager_identifier = 0;
static long manager_modified   = 0;
static long agent_reserved     = 0;



static void ma_restartNode(void) 
{
/* <ATR:01> */
//  sw_reset();
	hw_reset();
}


static void ma_refreshReservationTimer(void)
{
   if (manager_identifier != 0)                  /* reserved ? */
       manager_modified = get_sys_time();       /* set actual time */
}


UNSIGNED32 ma_getmanager_identifier(void)
{
   long act_time;
   if (manager_identifier != 0)  /* reserved or timeout? */
   {
       act_time = get_sys_time();
       if(((act_time - manager_modified) > agent_reserved)  || ((act_time - manager_modified) > AGT_RES_TIMEOUT))
            manager_identifier = agent_reserved = 0;
            /* time out -> clear reservation */
   }
   
   return manager_identifier;

}


static int ma_reserveAgent(UNSIGNED32 identifier, UNSIGNED16 time_out)
{
    if ((ma_getmanager_identifier() == 0) ||
        (ma_getmanager_identifier() == identifier))  /* reserved ? */
    {
       manager_identifier = identifier;
       agent_reserved     = time_out;
       ma_refreshReservationTimer();
       /* update station_status_word */
       station_status_word |= STATION_RESERVED;

	   /* 17.02.97 inserted (K. Hilbrich) */
	   lc_set_device_status_word( SER_MASK_BIT, STATION_RESERVED ); 

       return OK;
    }
	
	return NOTOK;
}


static int ma_forceReserveAgent(UNSIGNED32 identifier, UNSIGNED16 time_out)
{

    manager_identifier = identifier;
    agent_reserved     = time_out;
    ma_refreshReservationTimer();
    /* update station_status_word */
    station_status_word |= STATION_RESERVED;
    
	/* 17.02.97 inserted (K. Hilbrich) */
	lc_set_device_status_word( SER_MASK_BIT, STATION_RESERVED ); 

	return OK;
}


static int ma_releaseAgent(UNSIGNED32 identifier)
{
   if ((ma_getmanager_identifier() == 0) ||
       (ma_getmanager_identifier() == identifier))  /* reserved ? */
   {
      manager_identifier = 0;
      agent_reserved     = 0;
      /* update station_status_word */
	  station_status_word &= ~STATION_RESERVED;

	  /* 17.02.97 inserted (K. Hilbrich) */
	  lc_set_device_status_word( SER_MASK_BIT, ~STATION_RESERVED ); 
      
	  return OK;
   }

   return NOTOK;
}




static int ma_ReserveAgent(char* PDU) {

UNSIGNED16 command;
UNSIGNED16 access_type;
UNSIGNED16 time_out;
UNSIGNED32 manager_identifier;

/* parse parameters */
command               = ma_decodeUNSIGNED16((unsigned char*)&PDU[0]);
access_type           = ma_decodeUNSIGNED16((unsigned char*)&PDU[2]);
time_out              = ma_decodeUNSIGNED16((unsigned char*)&PDU[4]);
manager_identifier    = ma_decodeUNSIGNED32((unsigned char*)&PDU[6]);


switch (command) {

		 case RESERVE: 
			 {
                if (access_type == OVERWRITE)
                    return(ma_forceReserveAgent(manager_identifier,time_out));
                else
                    return(ma_reserveAgent(manager_identifier,time_out));
             };
         case RELEASE: 
			 {
                    return(ma_releaseAgent(manager_identifier));
             };
         case RESTART: 
			 {
                    ma_restartNode();
                    return(ma_releaseAgent(manager_identifier));  /* may not return */
             };
         default: 
			 {
                    return(MM_CMD_NOT_EXECUTED);
             }

         }
}



/************** serve request PDUs *****************/


static int ma_createReadStationStatusResponsePDU(struct TNMPDU *PDU, UNSIGNED16 station_status_word)
{
//    PDU->TNM_code = TNM;
//    PDU->SIF      = 0;
      ma_encodeUNSIGNED8(&PDU->Data[0],0);  /* how to determine the exact link_id ? use default-value 0*/
      ma_encodeUNSIGNED8(&PDU->Data[1],0);  /* unused */
      ma_encodeUNSIGNED16(&PDU->Data[2],own_adr);
      ma_encodeUNSIGNED16(&PDU->Data[4],station_status_word);

      return(8);
}



static int ma_createWriteStationControlResponsePDU(struct TNMPDU *PDU, UNSIGNED8 link_id)
{
//    PDU->TNM_code = TNM;
//    PDU->SIF      = 1;
      ma_encodeUNSIGNED8(&PDU->Data[0],link_id);
      ma_encodeUNSIGNED8(&PDU->Data[1],0);  /* unused */
      ma_encodeUNSIGNED16(&PDU->Data[2],own_adr);

      return(6);
}


static int ma_createReadInventoryResponsePDU(struct TNMPDU *PDU,int *ReturnCode)
{
      int           len = 0;
      int           i   = 0;
      int           j   = 0;
      UNSIGNED16    tmp = 0;
      unsigned char erg;

      lc_get_device_address(&own_adr);

//    PDU->TNM_code = TNM;
//    PDU->SIF      = 2;

      len += ma_encodeUNSIGNED16(&PDU->Data[len], 0);
      len += ma_encodeString((char*)&PDU->Data[len], agent_version);
      len += ma_encodeString((char*)&PDU->Data[len], manufact_name);
      len += ma_encodeString((char*)&PDU->Data[len], device_type);

       /* service list */
       for (i=15;i>=0;i--) {
            tmp = 0;
            for(j=15;j>=0;j--) {
                am_get_dir_entry((unsigned char)((i<<4)+j),&erg);
                     if(erg == own_adr) {
                          tmp = tmp | (1<<j);
                     }
            }
            len += ma_encodeUNSIGNED16(&PDU->Data[len], tmp);
       }
	   
	   /* link set */
#ifdef O_GATEWAY
	   tmp = 3;
#else
	   tmp = 1;
#endif
       len += ma_encodeUNSIGNED16(&PDU->Data[len], tmp);

       len += ma_encodeUNSIGNED8(&PDU->Data[len],0); /* align */
	   len += ma_encodeUNSIGNED8(&PDU->Data[len],station_identifier);
       len += ma_encodeString((char*)&PDU->Data[len],station_name);
       len += ma_encodeUNSIGNED16(&PDU->Data[len],station_status_word);
	   
	   *ReturnCode = OK;

       return(len + 2);
 }



static int ma_createReadDescriptorResponsePDU(struct TNMPDU *PDU,UNSIGNED8 SIF_code,char *service_description)
{
//   PDU->TNM_code = TNM;
//   PDU->SIF      = 4;
     PDU->Data[0]  = 0;
     PDU->Data[1]  = SIF_code;
     return(ma_encodeString((char*)&PDU->Data[2],service_description) + 4);
}


/* READ LINK DESCRIPTOR NOT AVAILABLE YET */



/***************** STATION OBJECT **********************/


int ma_StationObjects(struct TNMPDU *InPDU,struct TNMPDU *OutPDU,int *OutPDULen)
{

      int  ReturnCode = OK;
      lc_get_device_address(&own_adr);
      station_status_word |= lc_get_device_status_word();

	  /* <ATR:04> */
      switch (InPDU->SIF) {

      case ReadStatus:
           {
                *OutPDULen = ma_createReadStationStatusResponsePDU(OutPDU,station_status_word);
           };
           break;

      case WriteControl:
           {
                UNSIGNED8 command = ma_decodeUNSIGNED8(&InPDU->Data[0]);
                UNSIGNED8 st_id  =  ma_decodeUNSIGNED8(&InPDU->Data[1]);

                if (command & 0x01)
                        ma_restartNode();  /* SW-reset --> function does not return */

                /* if st_id = 0 the identifier of the station is not changed */
                if (st_id != 0)
                    station_identifier = st_id;

                /* if station-name is zero, the station name is not changed */
                if (((char *)(&InPDU->Data[2])) != NULL)
                    ma_decodeString(station_name,(char*)&InPDU->Data[2]);

                 /* how to determine the exact link-id? use default-value 0 */
                 *OutPDULen = ma_createWriteStationControlResponsePDU(OutPDU, 0);

            };
            break;

      case ReadInventory:
           {	
			   *OutPDULen = ma_createReadInventoryResponsePDU(OutPDU,&ReturnCode);
           };
           break;

      case Reserve:   /* agent reservation */
           {
               ReturnCode = ma_ReserveAgent((char*)&InPDU->Data[0]);  /* may not return in case of restart */
               ma_encodeUNSIGNED32(&OutPDU->Data[0],ma_getmanager_identifier());
               *OutPDULen = 6;
           };
           break;

      case ReadDescriptor:
           {
               char *descr = NULL;
	  
			   if ((descr = ma_getGroupDescription(ma_decodeUNSIGNED8(&InPDU->Data[1]))) != NULL)
                   *OutPDULen = ma_createReadDescriptorResponsePDU(OutPDU,InPDU->Data[1],descr);
               else ReturnCode = MM_CMD_NOT_EXECUTED;
           };
           break;

      case ReadLinksDescriptor:
           {
             /* LME not available yet */
             ReturnCode = MM_SIF_NOT_SUPPORTED;
           }
           break;

      case WriteLinksDescriptor:
           {
             /* LME not available yet */
             ReturnCode = MM_SIF_NOT_SUPPORTED;
           }
           break;

      default: {
          return(MM_SIF_NOT_SUPPORTED);
      }
	
	}

    return(ReturnCode);
}
