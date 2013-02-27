/*************************************************************************
**        Copyright (C) Siemens AG 1997 All Rights Reserved. Confidential    
**========================================================================
**
**    PROJECT::    TCN Train Communication Network
		   JDP Joint Development Project

**    MODULE::     UIC Mapping Server

**    WORKFILE::   $Workfile:   UNGS.H  $
**************************************************************************
**    TASK::       Definition of UIC Nadi and Group Server functions

**************************************************************************
**    AUTHOR::     Just ()  
**    CREATED::    20.10.97
**========================================================================
**    HISTORY::    AUTHOR::   $Author: schiavor $
      REVISION:: $Revision: 1.1.2.1 $  
      MODTIME::  $Date: 2006/12/01 10:41:46 $
      $Log: UNGS.H,v $
      Revision 1.1.2.1  2006/12/01 10:41:46  schiavor
      01/12/2006                         Napoli                            SchiavoR
      Primo inserimento per utilizzo libreria nel GTW

 * 
 *    Rev 1.7   30 Nov 1998 14:57:00   REINWALD_LU
 * add ungs_read_nadi function for the use by kft only
 * 
 *    Rev 1.6   24 Jun 1998 07:50:06   REINWALD_LU
 * enums replaced by defines
 * 
 *    Rev 1.5   31 Mar 1998 08:27:28   Reinwald_Lu
 * new function uic tcn convert
 * 
 *    Rev 1.4   09 Mar 1998 15:21:58   HILBRICH_KL
 * delivered to ADtranz on 09/03/98
**************************************************************************/
#ifndef UNGS_H_
#define UNGS_H_

#if defined _DEBUG || defined _UNGS_TRACE
	
 #define UNGSTRACE( txt, fp ) fwrite( txt, sizeof( char ), strlen( txt ), fp ); \
							   fflush( fp )
 #define UNGSPRINT( txt, x, y ) sprintf( txt, x, y )

#else

 #define UNGSTRACE( txt, fp )
 #define UNGSPRINT( txt, x, y ) 

#endif

		    /* Input for the buffer state machine   */
		    /* for NADI data                        */
#define BUFFER_INIT             0 /* Initialization                       */
#define BUFFER_READ             1 /* read from the 2 paged buffer         */
#define BUFFER_WRITE            2 /* write into the 2 paged buffer        */
#define BUFFER_WRITE_DONE       3 /* write to buffer completed            */
#define SET_NADI_STATE_ACT      4 /* set NADI state to NADI_STATE_ACT     */
#define SET_NADI_STATE_CONF     5 /* set NADI state to NADI_STATE_CONF    */
#define SET_NADI_STATE_INVALID  6 /* set NADI state to NADI_STATE_INVALID */

typedef int UNGS_BUFFER_TRIGGER;

		     /* states of the 2 paged buffer control */
		     /* for NADI data                        */
#define BUFFER_STATE_IDLE                    0 /* idle state                           */
#define BUFFER_STATE_NADI_UPDATE_IN_PROGRESS 1 /* NADI update is in progress   */
#define BUFFER_STATE_NADI_UPDATE_DONE        2 /* NADI update is done                  */

typedef int UNGS_BUFFER_STATE;

#define UIC_GROUP_MAX              54  /* max. supported Groups         */

#define UIC_GROUP_BASE_ADDR       201   /* Base address for groups       */

/* error codes of the UNGS returned to UTBC and UAGT */
typedef int UNGS_RESULT;

/*** function prototypes *******************/

/*********************************************************************/
/*  Name:       ungs_request                                         */
/*  Function:   global NADI server interface                         */
/*              The requested service is encoded in the uic_callmsg. */
/*              Services are read NADI, write NADI,                  */
/*              convert UIC address to TCN addres, set state of NADI,*/
/*              read, write or delete group information              */
/* Return codes:    UNGS_OK                                          */
/*                  UIC_S_REPLY_ERROR, when the ptr to the reply     */
/*                  buffer is invalid or the length of the reply     */
/*                  buffer is too small.                             */
/*                  Otherwise the return code of the functions called*/
/*                  by ungs_request is returned.                     */
/*********************************************************************/
UNGS_RESULT ungs_request (void          *p_call_msg, 
			  CARDINAL32    call_msg_size,
			  AM_ADDRESS    *p_caller,
			  void          **p_reply_msg, 
			  CARDINAL32    *p_reply_msg_size,
			  REPLY_FREE    *p_reply_free_fct); 

/*********************************************************************/
/*  Name:       ungs_init()                                          */
/*  Function:   Initialization of NADI and Group server.             */
/*              Allocation of memory for NADI and group information. */
/*              Assumption is that there will be no shutdown function*/
/*              to free the allocated memory (Always power off).     */
/*              Preset NADI read and write pointer.                  */
/*  returns :   UIC_S_OK , when everything went well                 */
/*              UIC_S_ALLOC_ERR, when memory could not be allocated. */
/*********************************************************************/
UNGS_RESULT ungs_init ( ums_NodeConfigurationTable* p_ct);


/*********************************************************************/
/*  Name:       ungs_NADI_write()                                    */
/*  Function:   Write the NADI                                       */
/*              Check if the number of vehicles in the call buffer   */
/*              would fit into the allocated NADI space              */
/*              copy UIC_GlobalDescr and all                         */
/*              UIC_VehDescr into the NADI                           */
/*  returns :   UIC_S_OK , when everything went well                 */
/*              UIC_S_NADI_TOO_SMALL, when the allocated NADI space  */
/*              too small                                            */
/*********************************************************************/
UNGS_RESULT ungs_NADI_write ( UIC_NADI *p_input_nadi );


void ungs_read_nadi(UIC_Nadi *p_output_nadi);
/* for KFT */


/*********************************************************************/
/*  Name:       ungs_UIC_TCN_convert()                               */
/*  Function:   Converts the UIC address into TCN address            */
/*              Check if the UIC address is valid                    */
/*  returns :   UIC_S_OK , when everything went well                 */
/*              UIC_S_ALLOC_ERR, when the allocation of the reply    */
/*                               message buffer failed               */
/*  Note :      The usage of this function is restricted !!          */
/*              The calling hierarchy must start with the function   */
/*              uagt_receive_confirm, i.e. the function calling      */
/*              ungs_UIC_TCN_convert() must be called itself by      */
/*              uagt_receive_confirm. Otherwise data integrity is    */
/*              not guaranteed                                       */
/*********************************************************************/

UNGS_RESULT ungs_UIC_TCN_convert(UNSIGNED8   uicAddress,    /* UIC Address   */
			 UIC_CvtUicToTcnList **p_reply,     /* pointer to reply */
			 UNSIGNED32          *p_reply_size);/* size of reply */
 


#endif  /* UNGS_H_      */
