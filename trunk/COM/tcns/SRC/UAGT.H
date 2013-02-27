/*************************************************************************
**        Copyright (C) Siemens AG 1997 All Rights Reserved. Confidential    
**========================================================================
**
**    PROJECT::    TCN Train Communication Network
                   JDP Joint Development Project

**    MODULE::     UIC-Agent

**    WORKFILE::   $Workfile:   UAGT.H  $
**************************************************************************
**    TASK::       Definition of UIC-Agent types and functions

**************************************************************************
**    AUTHOR::     Hilbrich ()  
**    CREATED::    20.10.97
**========================================================================
**    HISTORY::    AUTHOR::   $Author: schiavor $
                   REVISION:: $Revision: 1.2 $  
                   MODTIME::  $Date: 2006/12/01 13:59:21 $
      $Log: UAGT.H,v $
      Revision 1.2  2006/12/01 13:59:21  schiavor
      01/12/2006                         Napoli                            SchiavoR
      Primo inserimento per utilizzo libreria nel GTW

      Revision 1.1.2.1  2006/12/01 10:41:46  schiavor
      01/12/2006                         Napoli                            SchiavoR
      Primo inserimento per utilizzo libreria nel GTW

 * 
 *    Rev 1.6   07 Aug 1998 15:33:52   Just_Pete
 * Bei der max. Message-Länge müssen auch
 * die Multicast-Steuerdaten berücksichtigt 
 * werden.
 * 
 *    Rev 1.5   05 Aug 1998 15:40:34   Just_Pete
 * UAGT_MAX_MSG_LEN neu definiert wegen
 * dem Telegramm UIC_GroupListReply
 * 
 *    Rev 1.4   09 Mar 1998 16:32:46   HILBRICH_KL
 * delivered to ADtranz 09/03/98  ( updated )
 * 
 *    Rev 1.3   09 Mar 1998 15:21:52   HILBRICH_KL
 * delivered to ADtranz on 09/03/98
**************************************************************************/
#ifndef UAGT_H_
#define UAGT_H_

#ifndef TCNTYPES_H_
#error include "tcntypes.h" first 
#endif

#ifndef UMS_H_
#error include "ums.h" first 
#endif

#ifndef AM_SYS_H
#error include "am_sys.h" first
#endif



#define UAGT_SIZE_DEF_MSG       10

#define UAGT_MAX_MSG_LEN        UIC_MCCREATE_CTRL_BYTES+UIC_SIZEOF_UIC_GROUPLISTREPLY(54) 
                                        /* max. len for received request:         */
                                        /* depends on max. size of WriteGroupList */
                                        /* plus the number of bytes required for  */
                                        /* the multicast control bytes            */
                                        /* (10+2+54*(2+64+22*6)=12+10692=10704)+8 */
#define UAGT_MAX_PROV           5       /* max. number of providers               */
#define UAGT_MAX_REPL_INST      10      /* max. number of replier instances       */


/* external interface of UIC Agent */

/* error codes of the UAGT */
typedef int UAGT_RESULT;



UAGT_RESULT  uagt_init( UNSIGNED8 nb_instances, ums_NodeConfigurationTable* );
/*{ this procedure initializes the UIC-agent with a list
    of providers and the number of instances }*/


void uagt_receive_confirm (unsigned char, const AM_ADDRESS*, void*, CARDINAL32, void*);



/* reference for receive request instances */

typedef struct {
    UNSIGNED8  ref;
    REPLY_FREE p_buffer_free_fct;
    void       *p_buffer;
} uagt_reference;

#endif  /* UAGT_H_ */


/**********************************************************************/
