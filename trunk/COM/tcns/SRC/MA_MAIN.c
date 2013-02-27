/*==============================================================================================*/
/* Unmarked modifications by Giuseppe Carnevale                                                 */
/*==============================================================================================*/

#define UNREFERENCED(x) x=x


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
*    history:     date          sign   remark
*                 04.10.1994    Re     created
*                 29.03.1996    Hil    'main' changed to 'nma_appl'   ->
*                                      this task must be started by 'ns_main()'!
*                                      group-identifier now determined by SIF/MAX_GROUP_ELEMENTS
*                                      NOTE: when using the services 'am_read_messenger_status'
*                                      and 'am_write_messenger_control' from RTP, Version 4.0 ->
*                                      AM_NET_ADDRESS must be replaced by AM_ADDR !
*
*
*
******************************************************************************/


/******************************************************************************
*
*   name:         mm_main.c
*
*   function:     Main Modul for Train Network Management Agent
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
#include "am_sys.h"
#include "tnm.h"

char InPDU[MAX_TNM_PDU];   /* request buffer  */
char OutPDU[MAX_TNM_PDU];  /* response buffer */

struct str_rec_msg {
	char           replier_fct;
	AM_NET_ADDR    caller;
	void           *msg_addr;
	CARDINAL32     msg_size;
};

static struct str_rec_msg rxi_msg;

typedef struct {
  char * SIFDescription;
  int (* SIFGroup) (struct TNMPDU *, struct TNMPDU *, int *);   
} SIFElement;

typedef struct {
  char * SIFDescription;
  int (* SIFFunction) (void *, void *, int *);   
} UserSIFElement;

static const SIFElement SIFTab[] = {

  "READ_STATION_STATUS"       /*(00)*/, ma_StationObjects,
  "WRITE_STATION_CONTROL*"    /*(01)*/, ma_StationObjects,
  "READ_STATION_INVENTORY"    /*(02)*/, ma_StationObjects,
  "WRITE_RESERVATION*"        /*(03)*/, ma_StationObjects,
  "READ_SERVICE_DESCRIPTOR*"  /*(04)*/, ma_StationObjects,
  0                           /*(05)*/, ma_BadSIFGroup,
  "READ_LINKS_DESCRIPTOR*"    /*(06)*/, ma_StationObjects,
  "WRITE_LINKS_DESCRIPTOR*"   /*(07)*/, ma_StationObjects,
  0                           /*(08)*/, ma_BadSIFGroup,
  0                           /*(09)*/, ma_BadSIFGroup,
  "READ_MVB_STATUS*"          /*(10)*/, ma_MVBObjects,
  "WRITE_MVB_CONTROL*"        /*(11)*/, ma_MVBObjects,
  "READ_MVB_DEVICES"          /*(12)*/, ma_MVBObjects,
  "WRITE_MVB_ADMINISTRATOR*"  /*(13)*/, ma_MVBObjects,
  0                           /*(14)*/, ma_BadSIFGroup,
  0                           /*(15)*/, ma_BadSIFGroup,
  0                           /*(16)*/, ma_BadSIFGroup,
  0                           /*(17)*/, ma_BadSIFGroup,
  0                           /*(18)*/, ma_BadSIFGroup,
  0                           /*(19)*/, ma_BadSIFGroup,
#ifdef O_GATEWAY
  "READ_WTB_STATUS*"          /*(20)*/, ma_WTBObjects,
  "WRITE_WTB_CONTROL*"        /*(21)*/, ma_WTBObjects,
  "READ_WTB_NODES*"           /*(22)*/, ma_WTBObjects,
  0                           /*(23)*/, ma_BadSIFGroup,
  "READ_WTB_TOPOGRAPHY*"      /*(24)*/, ma_WTBObjects,
#else
  "READ_WTB_STATUS*"          /*(20)*/, ma_BadSIFGroup,
  "WRITE_WTB_CONTROL*"        /*(21)*/, ma_BadSIFGroup,
  "READ_WTB_NODES*"           /*(22)*/, ma_BadSIFGroup,
  0                           /*(23)*/, ma_BadSIFGroup,
  "READ_WTB_TOPOGRAPHY*"      /*(24)*/, ma_BadSIFGroup,
#endif
  0                           /*(25)*/, ma_BadSIFGroup,
  0                           /*(26)*/, ma_BadSIFGroup,
  0                           /*(27)*/, ma_BadSIFGroup,
  0                           /*(28)*/, ma_BadSIFGroup,
  "WRITE_ATTACH_PORT*"        /*(29)*/, ma_VariablesObjects,
  "READ_PORTS_CONFIGURATION*" /*(30)*/, ma_VariablesObjects,
  "WRITE_PORTS_CONFIGURATION*"/*(31)*/, ma_VariablesObjects,
  "READ_VARIABLES"            /*(32)*/, ma_VariablesObjects,
  "WRITE_FORCE_VARIABLES*"    /*(33)*/, ma_VariablesObjects,
  0                           /*(34)*/, ma_BadSIFGroup,
  "WRITE_UNFORCE_VARIABLES*"  /*(35)*/, ma_VariablesObjects,
  0                           /*(36)*/, ma_BadSIFGroup,
  "WRITE_UNFORCE_ALL*"        /*(37)*/, ma_VariablesObjects,
  "READ_VARIABLE_BINDINGS*"   /*(38)*/, ma_VariablesObjects,
  "WRITE_VARIABLE_BINDINGS*"  /*(39)*/, ma_VariablesObjects,
  "READ_MESSENGER_STATUS*"    /*(40)*/, ma_MessengerObjects,
  "WRITE_MESSENGER_CONTROL*"  /*(41)*/, ma_MessengerObjects,
  "READ_FUNCTION_DIRECTORY*"  /*(42)*/, ma_MessengerObjects,
  "WRITE_FUNCTION_DIRECTORY*" /*(43)*/, ma_MessengerObjects,
  "READ_STATION_DIRECTORY*"   /*(44)*/, ma_MessengerObjects,
  "WRITE_STATION_DIRECTORY*"  /*(45)*/, ma_MessengerObjects,
  "READ_GROUP_DIRECTORY*"     /*(46)*/, ma_MessengerObjects,
  "WRITE_GROUP_DIRECTORY*"    /*(47)*/, ma_MessengerObjects,
  "READ_NODE_DIRECTORY*"      /*(48)*/, ma_MessengerObjects,
  "WRITE_NODE_DIRECTORY*"     /*(49)*/, ma_MessengerObjects,
  "READ_MEMORY"               /*(50)*/, ma_DomainsObjects,
  "WRITE_MEMORY*"             /*(51)*/, ma_DomainsObjects,
  0                           /*(52)*/, ma_BadSIFGroup,
  "WRITE_DOWNLOAD_SETUP*"     /*(53)*/, ma_DomainsObjects,
  0                           /*(54)*/, ma_BadSIFGroup,
  "WRITE_DOWNLOAD_SEGMENT*"   /*(55)*/, ma_DomainsObjects,
  0                           /*(56)*/, ma_BadSIFGroup,
  0                           /*(57)*/, ma_BadSIFGroup,
  0                           /*(58)*/, ma_BadSIFGroup,
  0                           /*(59)*/, ma_BadSIFGroup,
  "READ_TASKS_STATUS*"        /*(60)*/, ma_TasksObjects,
  "WRITE_TASKS_CONTROL*"      /*(61)*/, ma_TasksObjects,
  0                           /*(62)*/, ma_BadSIFGroup,
  0                           /*(63)*/, ma_BadSIFGroup,
  0                           /*(64)*/, ma_BadSIFGroup,
  0                           /*(65)*/, ma_BadSIFGroup,
  0                           /*(66)*/, ma_BadSIFGroup,
  0                           /*(67)*/, ma_BadSIFGroup,
  0                           /*(68)*/, ma_BadSIFGroup,
  0                           /*(69)*/, ma_BadSIFGroup,
  "READ_CLOCK*"               /*(70)*/, ma_ClockObjects,
  "WRITE_CLOCK*"              /*(71)*/, ma_ClockObjects,
  0                           /*(72)*/, ma_BadSIFGroup,
  0                           /*(73)*/, ma_BadSIFGroup,
  0                           /*(74)*/, ma_BadSIFGroup,
  0                           /*(75)*/, ma_BadSIFGroup,
  0                           /*(76)*/, ma_BadSIFGroup,
  0                           /*(77)*/, ma_BadSIFGroup,
  0                           /*(78)*/, ma_BadSIFGroup,
  0                           /*(79)*/, ma_BadSIFGroup,
  "READ_JOURNAL*"             /*(80)*/, ma_BadSIFGroup,
  "READ_EQUIPMENT*"           /*(82)*/, ma_BadSIFGroup
};

static UserSIFElement UserSIFTab[] =
/* User Function 0 */ { {0, ma_BadSIFGroup},
/* User Function 1 */   {0, ma_BadSIFGroup},
/* User Function 2 */   {0, ma_BadSIFGroup},
/* User Function 3 */   {0, ma_BadSIFGroup},
/* User Function 4 */   {0, ma_BadSIFGroup},
/* User Function 5 */   {0, ma_BadSIFGroup},
/* User Function 6 */   {0, ma_BadSIFGroup},
/* User Function 7 */   {0, ma_BadSIFGroup}
};

#define SIF_NUM			(sizeof(SIFTab)/sizeof(SIFTab[0]))
#define USER_SIF_NUM 	(sizeof(UserSIFTab)/sizeof(UserSIFTab[0]))


/****** receive-confirm procedure ******/
/***************************************/

void ma_receive_confirm(unsigned char replier_fct, const AM_NET_ADDR *caller,
						void *in_msg_addr, CARDINAL32 in_msg_size, void *ext_ref)
{

	struct str_rec_msg  *p_msg;
	int                 OutPDULen;

	p_msg = (struct str_rec_msg *) ext_ref;
	p_msg->replier_fct = replier_fct;
	p_msg->caller      = *caller;
	p_msg->msg_addr    = in_msg_addr;
	p_msg->msg_size    = in_msg_size;

	/*** now process the request ***/

	if (ma_processPDU((struct TNMPDU *)InPDU,(struct TNMPDU *)OutPDU,&OutPDULen) != OK)
		am_reply_requ(replier_fct, OutPDU, 0, &rxi_msg, AM_FAILURE);
	else
		am_reply_requ(replier_fct, OutPDU, OutPDULen, &rxi_msg, AM_OK);
}


/****** reply-confirm procedure ******/
/*************************************/

void ma_reply_confirm(unsigned char replier_fct, void *ext_ref)
{
	AM_RESULT       result;

	UNREFERENCED(ext_ref);
	UNREFERENCED(replier_fct);

	/* set up a new receive_request */
	am_rcv_requ(AM_SERVICE_FCT, InPDU, MAX_TNM_PDU, &rxi_msg, &result);
}


/****** main distributor ******/
/******************************/

int ma_processPDU(struct TNMPDU *InPDU, struct TNMPDU *OutPDU, int *OutPDULen)
{
    /* plausability check */
	if (InPDU->TNM_code != TNM && InPDU->TNM_code != TNM_NEW_CALL) return(MM_SIF_NOT_SUPPORTED);

	 /*** check, if it's a modifing request ******/

	 if (InPDU->SIF < 128 && (InPDU->SIF & 1) && (InPDU->SIF != 3))
		  if (ma_getmanager_identifier() != 0) return(MM_RDONLY_ACCESS);

    /* feel comfort: default response */
    OutPDU->TNM_code = InPDU->TNM_code == TNM ? TNM : TNM_NEW_REPLY;
    OutPDU->SIF      = InPDU->SIF;
    *OutPDULen       = 2;

    /* main distributor */
	if (InPDU->SIF < SIF_NUM)
		return SIFTab[InPDU->SIF].SIFGroup(InPDU, OutPDU, OutPDULen);
    if (InPDU->SIF - 128 < USER_SIF_NUM)
    	return UserSIFTab[InPDU->SIF - 128].SIFFunction(&InPDU->Data[0], &OutPDU->Data[0], OutPDULen);
    return MM_SIF_NOT_SUPPORTED;
}


/*
 *
 * the following procedures are called directly from another task,
 * respectively call code of another task
 *
 * - Train Network Management - Version 1.0 October 1994
 *
 */

int ma_subscribe(UNSIGNED16 cmd, UNSIGNED8 sif_code, int (ma_service_call)(),
				 int (ma_service_close)(), void *service_descr)
{
	UNREFERENCED(ma_service_close);

	if ((sif_code < 128) || (sif_code > 135)) return MM_SIF_NOT_SUPPORTED;

	if (cmd & 1) {
		/* unsubscribe*/
		UserSIFTab[sif_code - 128].SIFFunction    = ma_BadSIFGroup;
        UserSIFTab[sif_code - 128].SIFDescription = 0;
	} else {
		/* subscribe */
      	UserSIFTab[sif_code - 128].SIFFunction    = ma_service_call;
      	UserSIFTab[sif_code - 128].SIFDescription = (char*)service_descr;
   }

   return OK;
}


char* ma_getGroupDescription(UNSIGNED8 SIF)
{
  /* User defined Objects */
  if (SIF < SIF_NUM) return(SIFTab[SIF].SIFDescription);
  if (SIF - 128 < USER_SIF_NUM) return(UserSIFTab[SIF - 128].SIFDescription);
  return NULL;
}


int ma_BadSIFGroup(struct TNMPDU *InPDU,struct TNMPDU *OutPDU, int *OutPDULen)
{
    OutPDU->TNM_code = InPDU->TNM_code == TNM ? TNM : TNM_NEW_REPLY;
	OutPDU->SIF      = InPDU->SIF;
	*OutPDULen       = 2;

	return(MM_SIF_NOT_SUPPORTED);
} 


/* initialisation and set-up the first rcv_request */
int ma_init(void)
{
  AM_RESULT result;
  
  am_bind_replier(AM_SERVICE_FCT, (AM_RCV_CONF)ma_receive_confirm,
                  (AM_REPLY_CONF)ma_reply_confirm, &result);

  if (result == AM_OK) am_rcv_requ(AM_SERVICE_FCT, InPDU, MAX_TNM_PDU, &rxi_msg, &result);

  return result;
} /* end 'ma_init' */

