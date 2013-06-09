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
*   name:         maclock.c
*
*   function:     Serve Request PDU's for Clock Objects
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



#define ReadClock    70
#define SetClock     71


int ma_ClockObjects(struct TNMPDU *InPDU,struct TNMPDU * OutPDU,int * OutPDULen)
    {
    int  ReturnCode = OK;

    switch (InPDU->SIF) {

		case ReadClock:
		 {
			return(MM_SIF_NOT_SUPPORTED);
		 };
		 break;

		case SetClock:
		 {
		   return(MM_SIF_NOT_SUPPORTED);
		 };
		 break;
		 
		default:
		{
		  return(MM_SIF_NOT_SUPPORTED);
		}
    }
    return(ReturnCode);
}
