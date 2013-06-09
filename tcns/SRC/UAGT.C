
/*************************************************************************
**        Copyright (C) Siemens AG 1997 All Rights Reserved. Confidential
**========================================================================
**
**    PROJECT::    TCN Train Communication Network
		   JDP Joint Development Project

**    MODULE::     UIC-Agent

**    WORKFILE::   $Workfile:   UAGT.C  $
**************************************************************************
**    TASK::       UIC-Agent

**************************************************************************
**    AUTHOR::     Hilbrich ()
**    CREATED::    20.10.97
**========================================================================
**    HISTORY::    AUTHOR:: $Author:   REINWALD_LU  $
	  REVISION::            $Revision:   1.16  $
	  MODTIME::             $Date:   16 Dec 1998 12:56:06  $
      $Log:   O:\PRJ\JDP\FUE\SW\PVCS\ARC\TCN_FW\UIC\SRC\UAGT.Cv_  $
 *
 *    Rev 1.16   16 Dec 1998 12:56:06   REINWALD_LU
 * send reply, if uic not ok
 *
 *    Rev 1.15   30 Jul 1998 15:08:14   Just_Pete
 * size für reply_message korrigiert
 *
 *    Rev 1.14   22 Jun 1998 15:19:50   REINWALD_LU
 * removed include "nsdbserv.h"
 *
 *    Rev 1.13   09 Jun 1998 13:13:14   Just_Pete
 * Updated to the new command codes
 * used by KfT.
 *
 *    Rev 1.12   25 May 1998 09:21:44   Just_Pete
 * new error code in case of a wrong owner
 *
 *    Rev 1.11   19 May 1998 11:57:28   Just_Pete
 * O_KFT and prepared for synchrous
 * multicast request
 *
 *    Rev 1.10   15 May 1998 15:54:28   Just_Pete
 * bug in owner handling fixed
 *
 *    Rev 1.9   15 May 1998 13:57:54   Just_Pete
 * handling of private E-Telegrams
 *
 *    Rev 1.7   24 Apr 1998 14:11:42   Just_Pete
 * in uagt_receive_confirm vor
 * am_buffer_free()  Abfrage auf ptr != NULL
 *
 *    Rev 1.6   31 Mar 1998 08:18:14   Reinwald_Lu
 * e telegram counter inserted
 *
 *    Rev 1.4   09 Mar 1998 15:15:06   HILBRICH_KL
 * delivered to ADtranz on 09/03/98
 *
 *    Rev 1.3   03 Mar 1998 08:59:54   REINWALD_LU
 * Delivered to Adtranz in 03/98
**************************************************************************/
#include "profile.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "tcntypes.h"
#include "pi_sys.h"
#include "am_sys.h"
#include "bmi.h"
#include "ums.h"
#include "adid.h"
#include "uagt.h"
#include "uimcs.h"
#include "uwtm.h"
#include "umsconv.h"
#include "ungs.h"
#include "utbc.h"
#ifdef O_KFT
#include "kft.h"
#endif

UNSIGNED32 uic_e_count; /* rei, counter for handled E-telegrams for statistics */
uagt_reference * reference;
unsigned char replier_function;

static uagt_reference uagt_ref[ UAGT_MAX_REPL_INST ];

/*********************************************************************/
/*  Name:       uagt_receive_confirm()                               */
/*  Function:   receive confirm function of the uic agent. The byte- */
/*              stream uic telegram is decoded, the appropriate      */
/*              function provider is called and the reply message is */
/*              sent back as a bytestream uic telegram to the calling*/
/*              function.                                            */
/*  returns :   void                                                 */
/*********************************************************************/

void uagt_receive_confirm( unsigned char replier_fct,
				       const AM_ADDRESS *caller,
				       void *in_msg_addr,
				       CARDINAL32 in_msg_size,
				       void *ext_ref )
{

    void* p_uic_replymsg = NULL;
    UNSIGNED32 uic_replymsg_size;
    AM_ADDRESS in_caller;
    REPLY_FREE p_reply_free_fct = NULL;
    UNSIGNED8* p_byte_in;
    UNSIGNED16 service;
    UNSIGNED8  owner;
    UIC_Msg msg_in, msg_out;
    int f_err = 0;

    printf("\n Ricevuta richiesta servizio UIC\n");
    in_caller = *caller;
    reference = (uagt_reference*) ext_ref;
    replier_function = replier_fct;
    p_byte_in = (UNSIGNED8*)in_msg_addr;

    uic_e_count++;

    /* test for valid ptrs and message size */
    if( ((UNSIGNED8*)0 == p_byte_in) ||
	((uagt_reference*)0 == reference) ||
	(in_msg_size < BYTESTREAM_SIZE_UIC_Header) ){
	/* This error may be be caused by a non-UIC telegramm */
	/* No reply is therefore generated. A protocol timeout*/
	/* will be generated                                  */
	goto uagt_return_for_timeout;
    }

    /* decode UIC service */
    service = NTOH16(p_byte_in + UIC_HEADER_CMD_OFFSET );
    owner   = NTOH8 (*(p_byte_in + UIC_HEADER_OWNER_OFFSET));

    switch (owner) {
      case UIC_O_UIC:
      /* UIC messages ********************************************/

    	switch( service ) {
          /*** UIC_WTM *******************************************/
          case UIC_FC_CHANGE_OMODE:
          case UIC_FC_INAUGURATION_CONTROL:
          case UIC_FC_INAUGURATION_ENFORCE:
	  case UIC_FC_SLEEP:
	  case UIC_FC_READ_UWTM_STATE:
	  case UIC_FC_READ_UWTM_TOPOGRAPHY:
	  case UIC_FC_SET_LEADING_REQU:
	    if (UIC_S_OK != (f_err = uwtm_request ( in_msg_addr,
						    in_msg_size,
						    &in_caller,
						    &p_uic_replymsg,
						    &uic_replymsg_size,
						    &p_reply_free_fct )))
	    {
		if (p_uic_replymsg != NULL) {
		    PI_FREE ( p_uic_replymsg );
		}
		goto uagt_error;
	    }
	    break;

	  /*** UIC_UNGS **********************************************/
	  case UIC_FC_READ_NADI:
	  case UIC_FC_CVT_UIC_TO_TCN:
	  case UIC_FC_READ_GROUP:
	  case UIC_FC_READ_GROUP_LIST:
	  case UIC_FC_WRITE_GROUP:
	  case UIC_FC_WRITE_ALL_GROUPS:
	  case UIC_FC_DELETE_GROUP:
	  case UIC_FC_DELETE_ALL_GROUPS:
	    if (UNGS_OK  != (f_err = ungs_request ( in_msg_addr,
						    in_msg_size,
						    &in_caller,
						    &p_uic_replymsg,
						    &uic_replymsg_size,
						    &p_reply_free_fct )))
	    {
		if (p_uic_replymsg != NULL) {
		    PI_FREE ( p_uic_replymsg );
		}
		goto uagt_error;
	    }
	    break;

	  /*** UIC_UTBC ***********************************************/
	  case UIC_FC_WRITE_CORRECTION:
	  case UIC_FC_DEL_CONFIG:
	  case UIC_FC_WRITE_VEH_RESNUM:
	    if (UIC_S_OK != (f_err = utbc_global_request(   in_msg_addr,
							    in_msg_size,
							    &in_caller,
							    &p_uic_replymsg,
							    &uic_replymsg_size,
							    &p_reply_free_fct )))
	    {
		if (p_uic_replymsg != NULL) {
		    PI_FREE ( p_uic_replymsg );
		}
		goto uagt_error;
	    }
	    break;

	  /*** UIC_GROUP_UIMCS *****************************************/
	  case UIC_FC_MULTICAST_CREATE:
	    if (UIC_S_OK != (f_err = uimcs_request( in_msg_addr,
						    in_msg_size,
						    &in_caller,
						    &p_uic_replymsg,
						    &uic_replymsg_size,
						    &p_reply_free_fct )))
	    {
		if (f_err == UIMCS_OK_SYNC) {
		    goto uagt_return_for_sync;
		}
		if (p_uic_replymsg != NULL) {
		    PI_FREE ( p_uic_replymsg );
		}
		goto uagt_error;
	    }
	    break;

  	  /*** default: invalid service code ****************************/
	  default:
	    f_err = UIC_S_ERR_FUNCTION_UNKNOWN;
	    goto uagt_error;
	    break;
	} /* end of switch service */
	break; /* end of case UIC_O_UIC */

	case UIC_O_DB:
	/* Siemens application specific command code */
	  switch( service ) {
#ifdef O_KFT
	    /*** UIC_GROUP_UTBC/KFT **************************************/
	    case FC_READ_KFTCORR:
	    case FC_WRITE_KFTCORR:
		if (UIC_S_OK != (f_err = utbc_global_request(   in_msg_addr,
								in_msg_size,
								&in_caller,
								&p_uic_replymsg,
								&uic_replymsg_size,
								&p_reply_free_fct )))
		{
		    if (p_uic_replymsg != NULL) {
			PI_FREE ( p_uic_replymsg );
		    }
		    goto uagt_error;
		}
		break;
#endif
	    /*** UIC_GROUP_UIMCS *****************************************/
	    /*** multicast of a non-UIC message **************************/
	    case UIC_FC_MULTICAST_CREATE:
		if (UIC_S_OK != (f_err = uimcs_request( in_msg_addr,
							in_msg_size,
							&in_caller,
							&p_uic_replymsg,
							&uic_replymsg_size,
							&p_reply_free_fct )))
		{
		    if (f_err == UIMCS_OK_SYNC) {
			goto uagt_return_for_sync;
		    }
		    if (p_uic_replymsg != NULL) {
			PI_FREE ( p_uic_replymsg );
		    }
		    goto uagt_error;
		}
		break;
	    /*** default: invalid service code ****************************/
	    default:
		f_err = UIC_S_ERR_FUNCTION_UNKNOWN;
		goto uagt_error;
		break;
	} /* end of switch service */
	break; /* end of case UIC_O_DB */

      /*** default: invalid owner************************************/
      default:
        f_err = UAGT_OWNER_INVALID;
        goto uagt_error;
	break;
    } /* end of switch owner */

    /* Save pointer to reply message and pointer to the free function */
    /* returned by the provider ungs for the specific instance of the */
    /* uagt.                                                          */
    reference->p_buffer = p_uic_replymsg;
    reference->p_buffer_free_fct = p_reply_free_fct;

    /* free the call message allocated by the messenger */
    am_buffer_free( in_msg_addr, in_msg_size);

    /* send back provider-result */
    am_reply_requ( replier_fct,
		    p_uic_replymsg,         /* out_msg_addr */
		    uic_replymsg_size,      /* out_msg_size */
		    ext_ref,                /* extern reference */
	    AM_OK );


    return;


/* in case of an error */
uagt_error:

    /* allocate reply buffer */
    p_uic_replymsg = (UNSIGNED8*)PI_ALLOC(UIC_HEADER_BYTES);
    if( !p_uic_replymsg ){
	goto uagt_return_for_timeout;
    }

#ifndef O_CONV
    /* Convert the header of the call message. Since a sole UIC Header */
    /* is required the conversion function of the telegram "read NADI, */
    /* call" is used                                                   */
    wtbconv_00_01__NTOH(in_msg_addr, &msg_in.hdr);
    /* preset the reply header */
    exchange_UIC_HEADER(&msg_out, &msg_in);
    wtbconv_00_01__HTON(p_uic_replymsg, &msg_out.hdr);
    /* Save pointer to reply message and pointer to the free function */
    /* returned by the provider ungs for the specific instance of the */
    /* uagt.                                                          */
    reference->p_buffer = p_uic_replymsg;
    reference->p_buffer_free_fct = PI_FREE;
#else
    /* preset the reply header */
    exchange_UIC_HEADER(&msg_out, &msg_in);
#endif
    msg_out.hdr.state_cmd = (UNSIGNED8)f_err;

    am_reply_requ( replier_fct,
		    p_uic_replymsg,     /* out_msg_addr */
		    UIC_HEADER_BYTES,   /* out_msg_size */
		    ext_ref,            /* external reference */
         	    AM_OK );
    if (in_msg_addr != NULL) {
        /* free the call message allocated by the messenger */
	am_buffer_free( in_msg_addr, in_msg_size);
    }

    return;
    /* in case of a severe telegram error */
uagt_return_for_timeout:
    /* send return with error and length 0 to free the connection again */
    am_reply_requ(replier_fct, NULL,0,ext_ref,UAGT_MSG_TOO_SHORT);

uagt_return_for_sync:
    /* return for sync means, uimcs will create the reply ! */
    if (in_msg_addr != NULL) {
        /* free the call message allocated by the messenger */
	am_buffer_free( in_msg_addr, in_msg_size);
    }
    return;
}


/*********************************************************************/
/*  Name:       uagt_reply_confirm()                                 */
/*  Function:   reply confirm function of the uic agent. In case a   */
/*              message reply buffer had been allocated this function*/
/*              frees for this instance of uic agent the buffer      */
/*  returns :   void                                                 */
/*********************************************************************/

void uagt_reply_confirm( unsigned char replier_fct, void *ext_ref )
{

    AM_RESULT  result;
    uagt_reference * ref;
    ref = ext_ref;

    UNREFERENCED(replier_fct);

    /* free the previously allocated message reply buffer */
    if (ref->p_buffer && ref->p_buffer_free_fct) {
	(*ref->p_buffer_free_fct)(ref->p_buffer);
	ref->p_buffer = NULL;
    }

    /* set up new receive_request */
    am_rcv_requ( UIC_F_UMS,             /* replier function */
		 NULL,                  /* in_msg_addr */
		 UAGT_MAX_MSG_LEN,      /* in_msg_size */
		 (UNSIGNED8*)ext_ref,   /* extern ref */
		 &result  );            /* result */
    return;
}



/*********************************************************************/
/*  Name:       uagt_init()                                          */
/*  Function:   initialization of the uic agent. For all instances   */
/*              specified in the NSDB a receive request is set up.   */
/*  returns :   UAGT_OK, when all went well                          */
/*              UAGT_MAX_INST_ERR, when the amount of instances      */
/*                                 exceeds a maximum count           */
/*              UAGT_INIT_ERROR, when a am_bind_replier() or a       */
/*                              call to am_rcv_req() failed.         */
/*********************************************************************/

UAGT_RESULT  uagt_init( UNSIGNED8  nb_instances,
			ums_NodeConfigurationTable * ums_ct )
{

    UNSIGNED8 result, i;
    UNREFERENCED(ums_ct);

    uic_e_count = 0;

    /* number of instances exceeds max. allowed */
    if( nb_instances > UAGT_MAX_REPL_INST ){
	return UAGT_MAX_INST_ERR;
    }

    /* init extern references */
    for( i = 0; i < nb_instances; i++ ){
	uagt_ref[i].ref = (UNSIGNED8)(i+1);
	uagt_ref[i].p_buffer_free_fct = (REPLY_FREE) NULL;
	uagt_ref[i].p_buffer = NULL;
    }


    am_bind_replier( UIC_F_UMS,
		   uagt_receive_confirm,
		   uagt_reply_confirm,
		   &result );

    if( result != AM_OK ){
	return UAGT_INIT_ERR;
    }
    else{

	/* setup receive requests for nb_instances */
	for( i = 0; i < nb_instances; i++ ){
	    am_rcv_requ( UIC_F_UMS,
			    NULL,
			    UAGT_MAX_MSG_LEN,
			    &uagt_ref[ i ],
			    &result );

	    if( result != AM_OK ){
		return UAGT_INIT_ERR;
	    }
	}
    }
    return UAGT_OK;
}


/*************  end of file *******************************/
