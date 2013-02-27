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
*                 03.10.1994  Re     created
*                 29.03.96    Hil    service- and utility-functions
*												 prototypes defined here
*												 new return-codes 43 and 44 added
*                                    constant 'AGENT_RES_TIMEOUT' defined
*
*
******************************************************************************/


/******************************************************************************
*
*   name:         TNM.H
*
*   function:     common include .
*
*
*   input:        -
*
*   output:       -
*
*   return value: -
*
******************************************************************************/

#ifndef _TNM_
#define _TNM_   1


#ifndef OK
#define OK      0
#define NOTOK  -1
#endif  

/* #define NMM_MANAGER   only define for manager */


/* Return Codes for TNM - procedures */ 

#define MM_OK					0

#define MM_SIF_NOT_SUPPORTED    33         /* SIF_code not supported */
#define MM_RDONLY_ACCESS        34         /* read only access permittted */
#define MM_CMD_NOT_EXECUTED     35         /* service failed */
#define MM_DNLD_NO_FLASH        36         /* no flash EPROM at that location*/
#define MM_DNLD_FLASH_HW_ERR    37         /* hardware error during download*/
#define MM_BAD_CHECKSUM         38         /* domain has an incorrect checksum */
#define MM_INT_ERROR            39         /* internal error */
#define MM_ER_VERS              40         /* erroneous version */
#define MM_BUS_HW_BAD           41         /* link damaged */
#define MM_BUS_HW_NO_CONFIG     42         /* unconfigured hardware */
#define MM_LP_ERROR				43         /* failed access to traffic store */
#define MM_VERSION_CONFLICT     44			 /* version conflict */

#define TNM 				    0x17       /* token for management-message header */

#define TNM_NEW_CALL			0x02	   /* new token for call messages */
#define TNM_NEW_REPLY			0x82	   /* new token for reply messages */

#define MAX_TNM_PDU             1024       /* largest size of data in frame */
#define AGT_RES_TIMEOUT         10000      /* agent reservation-timeout in ms */
										   /* res.-timeout should not be more than 3600s ! */ 
														 
 struct TNMPDU {
      UNSIGNED8 TNM_code;
      UNSIGNED8 SIF;
      UNSIGNED8 Data[MAX_TNM_PDU];
 }; 
			  
/* function prototypes */ 

/* void            ma_printPDU(struct TNMPDU *PDU,int PDULen); */

int				ma_init(void);
unsigned long   ma_getmanager_identifier(void);
int             ma_BadSIFGroup(struct TNMPDU *InPDU,struct TNMPDU *OutPDU, int * OutPDULen);
int        	  	ma_processPDU(struct TNMPDU *InPDU,struct TNMPDU *OutPDU,int * OutPDULen);
int        	  	ma_StationObjects(struct TNMPDU *InPDU,struct TNMPDU *OutPDU,int * OutPDULen);
int        	    ma_MVBObjects(struct TNMPDU *InPDU,struct TNMPDU *OutPDU,int * OutPDULen);
#ifdef O_GATEWAY
int        	    ma_WTBObjects(struct TNMPDU *InPDU,struct TNMPDU *OutPDU,int * OutPDULen);  
#endif
int        	    ma_VariablesObjects(struct TNMPDU *InPDU,struct TNMPDU *OutPDU,int * OutPDULen);
int        	    ma_MessengerObjects(struct TNMPDU *InPDU,struct TNMPDU *OutPDU,int * OutPDULen);
int        	    ma_DomainsObjects(struct TNMPDU *InPDU,struct TNMPDU *OutPDU,int * OutPDULen);
int        	    ma_TasksObjects(struct TNMPDU *InPDU,struct TNMPDU *OutPDU,int * OutPDULen);
int        	    ma_ClockObjects(struct TNMPDU *InPDU,struct TNMPDU *OutPDU,int * OutPDULen);
int        	    ma_UserDefinedObjects(struct TNMPDU *InPDU,struct TNMPDU *OutPDU,int * OutPDULen);
int        	    ma_BadSIFUser(void *InPDU,void  * OutPDU, int *OutPDULen);
char* 			ma_getGroupDescription(unsigned char SIF);

/** utility function prototypes **/

int              ma_decodeString(char * dest, char * source);
unsigned char    ma_decodeUNSIGNED8(unsigned char * source);
unsigned short   ma_decodeUNSIGNED16(unsigned char * source);
unsigned long    ma_decodeUNSIGNED32(unsigned char * source);

int 			 ma_encodeString(char * dest, char * source);
int 			 ma_encodeUNSIGNED8(unsigned char * dest, unsigned char source);
int 			 ma_encodeUNSIGNED16(unsigned char * dest, unsigned short source);
int 			 ma_encodeUNSIGNED32(unsigned char * dest, unsigned long source);  


//** function prototypes for network-management-manager **/ 

#ifdef NMM_MANAGER


/* Station Group */
int  mm_createReadStationStatusRequestPDU(struct TNMPDU *PDU);
int  mm_createWriteStationControlRequestPDU(struct TNMPDU *PDU,UNSIGNED8 cmd,UNSIGNED8 StationIdentifier, char* StationName);
int  mm_createReadInventoryRequestPDU(struct TNMPDU *PDU);
int  mm_createReservationRequestPDU(struct TNMPDU *PDU,UNSIGNED16 cmd,UNSIGNED16 access_type,UNSIGNED16 reservation_timeout,UNSIGNED32 manager_identifier);
int  mm_createReadDescriptorRequestPDU(struct TNMPDU *PDU,UNSIGNED8 SIF_code);
int  mm_createReadLinksDescriptorRequestPDU(struct TNMPDU *PDU);
int  mm_createWriteLinksDescriptorRequestPDU(struct TNMPDU *PDU,UNSIGNED8 link_id,UNSIGNED8 link_type,char* link_name); 

/* MVB Link Group */
int mm_createReadMVBStatusRequestPDU(struct TNMPDU *PDU, UNSIGNED8 link_id);
int mm_createWriteMVBControlRequestPDU(struct TNMPDU *PDU, UNSIGNED8 link,UNSIGNED16 device,UNSIGNED16 t_reply, UNSIGNED8 cmd);
int mm_createReadMVBDevicesRequestPDU(struct TNMPDU *PDU, UNSIGNED8 link);
int mm_createWriteMVBConfigurationRequestPDU(struct TNMPDU *PDU, UNSIGNED8 link_id);

/* WTB Link Group */ 

#ifdef O_GATEWAY

int mm_createReadWTBStatusRequestPDU(struct TNMPDU *PDU, UNSIGNED8 link);
int mm_createWriteWTBControlRequestPDU(struct TNMPDU *PDU, UNSIGNED8 link, UNSIGNED16 command);
int mm_createReadWTBNodesRequestPDU(struct TNMPDU *PDU, UNSIGNED8 link);
int mm_createReadWTBTopographyRequestPDU(struct TNMPDU *PDU, UNSIGNED8 link); 

#endif

/* Variables Group */
/* int mm_createReadPortConfigurationRequestPDU(struct TNMPDU *PDU, UNSIGNED8 link); */
int mm_createReadVariablesRequestPDU(struct TNMPDU *PDU, char* pv_name);
int mm_createForceVariableRequestPDU(struct TNMPDU *PDU, char* pv_name,UNSIGNED16 len, char* value);
int mm_createUnForceVariableRequestPDU(struct TNMPDU *PDU,char* pv_name);
int mm_createUnForceAllVariablesRequestPDU(struct TNMPDU *PDU, UNSIGNED16 stores);
/*int mm_createAttachPortRequestPDU(struct TNMPDU *PDU,UNSIGNED16 ds_name,UNSIGNED16 point, UNSIGNED16 filter, UNSIGNED16 gain, INTEGER16 offset);*/

/* Messenger Group */
int mm_createReadMessengerStatusRequestPDU(struct TNMPDU *PDU);
int mm_createWriteMessengerControlRequestPDU(struct TNMPDU *PDU, char* messenger_name, UNSIGNED8 send_time_out,UNSIGNED8 alive_time_out, UNSIGNED8 ack_time_out,UNSIGNED8 credit,UNSIGNED8 packet_size,UNSIGNED8 clear,UNSIGNED8 multicast_window);
int mm_createReadFunctionDirectoryRequestPDU(struct TNMPDU *PDU);
int mm_createWriteFunctionDirectoryRequestPDU(struct TNMPDU *PDU, UNSIGNED8 clear_directory, AM_DIR_ENTRY *list, UNSIGNED8 nb_of_entries);
int mm_createReadStationDirectoryRequestPDU(struct TNMPDU *PDU);
int mm_createWriteStationDirectoryRequestPDU(struct TNMPDU *PDU, UNSIGNED8 clear_directory,UNSIGNED8 nb_of_entries, AM_ST_ENTRY *st_entries);
int mm_createReadGroupDirectoryRequestPDU(struct TNMPDU *PDU);
int mm_createWriteGroupDirectoryRequestPDU(struct TNMPDU *PDU, UNSIGNED32 upper_bitset,UNSIGNED32 lower_bitset);
extern int mm_createReadNodeDirectoryRequestPDU(struct TNMPDU *PDU);
int mm_createWriteNodeDirectoryRequestPDU(struct TNMPDU *PDU,UNSIGNED8 clear_directory,UNSIGNED8 node_address, UNSIGNED16 device_address);

/* Domain Group */
int mm_createReadMemoryRequestPDU(struct TNMPDU *PDU, UNSIGNED32 base_address, UNSIGNED16 count, UNSIGNED8 type);
int mm_createWriteMemoryRequestPDU(struct TNMPDU *PDU, UNSIGNED32 base_address,UNSIGNED16 count, UNSIGNED8 type, char* Data);
int mm_createDownloadSetupRequestPDU(struct TNMPDU *PDU, UNSIGNED8 download_timeout, UNSIGNED8 command, UNSIGNED32 base_address, UNSIGNED32 count);
int mm_createDownloadSegmentRequestPDU(struct TNMPDU *PDU, UNSIGNED8 domain, UNSIGNED8 segment_type, UNSIGNED32 base_address,UNSIGNED32 count, char* data);

/* Task Group */
int mm_createWriteTaskControlRequestPDU(struct TNMPDU *PDU, UNSIGNED8 cmd);

/* Clock Group */
int mm_createReadClockRequestPDU(struct TNMPDU *PDU);
int mm_createSetClockRequestPDU(struct TNMPDU *PDU, UNSIGNED32 time);

#endif 


#endif /* _TNM_ */


