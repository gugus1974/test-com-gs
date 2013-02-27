/*************************************************************************
**        Copyright (C) Siemens AG 1997 All Rights Reserved. Confidential    
**========================================================================
**
**    PROJECT::    TCN Train Communication Network
		   JDP Joint Development Project

**    MODULE::     UIC Mapping Server

**    WORKFILE::   $Workfile:   UIMCS.H  $
**************************************************************************
**    TASK::       Definition of UIC Nadi and Group Server functions

**************************************************************************
**    AUTHOR::     Just ()  
**    CREATED::    20.10.97
**========================================================================
**    HISTORY::    AUTHOR::   $Author: schiavor $
      REVISION:: $Revision: 1.1.2.1 $  
      MODTIME::  $Date: 2006/12/01 10:41:46 $
      $Log: UIMCS.H,v $
      Revision 1.1.2.1  2006/12/01 10:41:46  schiavor
      01/12/2006                         Napoli                            SchiavoR
      Primo inserimento per utilizzo libreria nel GTW

 * 
 *    Rev 1.6   24 Jun 1998 07:47:48   REINWALD_LU
 * enums replaced by defines
 * 
 *    Rev 1.5   19 May 1998 10:13:00   Just_Pete
 * defines changed, UIMCS_OK_SYNC new
**************************************************************************/

#ifndef UIMCS_H 
#define UIMCS_H

#include "tcntypes.h"

#define   UIMCS_OK        0 /* a reply message is generated */
			    /* fatal errors: no reply message is generated */
#define   UIMCS_OK_SYNC   1 /* syncronous call OK; UAGT shall not generate a reply message */
#define   UIMCS_ERR_PTR   2 /* wrong pointer or size value */
#define   UIMCS_ERR_MSG   3 /* wrong message format */
#define   UIMCS_ERR_CODE  4 /* function code is unknown */
#define   UIMCS_ERR_REPLY 5 /* the memory for the reply could not be allocated */
#define   UIMCS_ERR_ALLOC 6 /* memory allocation failed */ 

typedef int UIMCS_T_RESULT;


typedef struct
{
   UNSIGNED8    mc_requests_max;     /* max. number of stored multicast requests */
   UNSIGNED8    open_calls_max;      /* max. number of open calls */
   CARDINAL32   reply_len_max;       /* max. length of reply message accepted */
   UNSIGNED16   reply_timeout;       /* max. timeout for replier */

} UIMCS_T_CONFIGURATION;

/****************************************************************************/
/* function prototypes                                                      */       
/****************************************************************************/

extern UIMCS_T_RESULT uimcs_init(UIMCS_T_CONFIGURATION* p_config_param);

extern UIMCS_T_RESULT uimcs_request(
	   void*       p_uic_callmsg,         /* pointer to the call message */
	   CARDINAL32  uic_callmsg_size,      /* size of the call message */
	   AM_ADDRESS* caller,                /* handle to the caller */
	   void**      pp_uic_replymsg,       /* (pointer to) reply message */
	   CARDINAL32* p_uic_replymsg_size,   /* (pointer to) size of the reply */
	   REPLY_FREE* p_reply_free_fct       /* function to free the 
						 uic_replymsg memory */
	  );

extern void uimcs_handler( void);
#endif
