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
*                 11.10.1994  Re     created
*
******************************************************************************/


/******************************************************************************
*
*   name:         matasks.c
*
*   function:     Serve Request PDU's for Task Objects
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


#define StopAllTasks    0
#define StartAllTasks   1

#define TaskControl     61


extern int ma_errors;


int ma_TasksObjects(struct TNMPDU *InPDU,struct TNMPDU * OutPDU,int * OutPDULen)
    {
    int  ReturnCode = OK;
    OutPDU = OutPDU;
    OutPDULen = OutPDULen;

    switch (InPDU->SIF) {

      case TaskControl:
	  {
	    UNSIGNED8 command;
	    /* UNSIGNED8 task_ID;  for fun only */
	    command = ma_decodeUNSIGNED8(&InPDU->Data[0]);

	    switch(command) {
	      case StopAllTasks:
	      {
		ReturnCode = MM_SIF_NOT_SUPPORTED;
	      }; break;

	      case StartAllTasks:
	      {
		ReturnCode = MM_SIF_NOT_SUPPORTED;
	      }; break;

	      default:
	      {
		ReturnCode = MM_CMD_NOT_EXECUTED;
	      }
	    }
	    
	    /* default response */
	  };
	  break;

	default:
	{
	  return(MM_SIF_NOT_SUPPORTED);
	}
    }
    return(ReturnCode);
}
