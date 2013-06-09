/*************************************************************************
**        Copyright (C) Siemens AG 1997 All Rights Reserved. Confidential
**========================================================================
**
**    PROJECT::    TCN Train Communication Network
		   JDP Joint Development Project

**    MODULE::     UIC Mapping Server

**    WORKFILE::   $Workfile:   UNGS.C  $
**************************************************************************
**    TASK::       UIC  NADI and Group Server

**************************************************************************
**    AUTHOR::     Just (J)
**    CREATED::    20.10.97
**========================================================================
**    HISTORY::    AUTHOR::   $Author:   REINWALD_LU  $
		   REVISION:: $Revision:   1.20  $
		   MODTIME::  $Date:   30 Nov 1998 14:57:12  $
      $Log:   O:\PRJ\JDP\FUE\SW\PVCS\ARC\TCN_FW\UIC\SRC\UNGS.Cv_  $
 *
 *    Rev 1.20   30 Nov 1998 14:57:12   REINWALD_LU
 * add ungs_read_nadi function for the use by kft only
 *
 *    Rev 1.19   15 Oct 1998 09:44:24   Just_Pete
 * ungs_UIC_TCN_convert():
 * Wenn bei der Konvertierung von UIC zu TCN
 * eine TCN Adresse mehrfach aufgeführt ist,
 * wie bei Trainsets, dann werden die
 * überzähligen herausgefiltert.
 *
 *    Rev 1.18   07 Aug 1998 15:26:36   Just_Pete
 * Längenberechnung für Gruppenliste_schreiben
 * wurde korrigiert
 *
 *    Rev 1.17   07 Aug 1998 08:52:58   Just_Pete
 * Fehlerbeseitigung bei Gruppenliste schreiben
 *
 *    Rev 1.16   04 Aug 1998 08:41:34   Just_Pete
 * casts wegen warning korrigiert
 *
 *    Rev 1.15   31 Jul 1998 14:20:42   Just_Pete
 * Die Länge des Reply-Telegramms wird jetzt
 * genau für den Bytestream berechnet
 *
 *    Rev 1.14   24 Jun 1998 07:53:56   REINWALD_LU
 * no more globally initialized vars; simplified array calculation
 *
 *    Rev 1.13   08 Jun 1998 08:59:06   REINWALD_LU
 * pi_dis/enable replaced by spl
 *
 *    Rev 1.12   04 Jun 1998 14:09:14   Just_Pete
 * Die Suche der Sammeladressen wurde
 * vervollständigt
 *
 *    Rev 1.11   19 May 1998 12:56:58   Just_Pete
 * size was set at the wrong location
 *
 *    Rev 1.10   15 May 1998 13:54:40   Just_Pete
 * ungs_search_group_address(): loop , when
 * p_group != NULL
 *
 *    Rev 1.9   12 May 1998 08:45:26   REINWALD_LU
 * 'owner' handling changed
 *
 *    Rev 1.8   06 May 1998 14:55:52   Just_Pete
 * ungs_request(): Bei ungültiger Gruppen-ID
 * darf von der reply_msg nur der Header
 * konvertiert werden.
 *
 *    Rev 1.7   31 Mar 1998 14:47:00   HILBRICH_KL
 * convert-function name for UICToTCN
 * adapted
 *
 *    Rev 1.6   31 Mar 1998 08:29:10   Reinwald_Lu
 * Sammeladressen inserted, more PI_FREEs
 *
 *    Rev 1.4   09 Mar 1998 15:15:56   HILBRICH_KL
 * delivered to ADtranz on 09/03/98
 *
 *    Rev 1.3   03 Mar 1998 09:00:44   REINWALD_LU
 * Delivered to Adtranz in 03/98
**************************************************************************/
#include "profile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tcntypes.h"
#include "pi_sys.h"
#include "am_sys.h"
#include "bmi.h"
#include "ums.h"
#include "umsconv.h"
#include "ungs.h"


#ifdef UNGS_NADI_TESTAUSGABE

void NADI_Testausgabe( UIC_NADI* p_nadi );

#endif

static UIC_NADI ungs_NADI[2];       /* UIC NADI table 2-paged               */
static UIC_NADI *p_read_NADI;       /* pointer to NADI read page            */
static UIC_NADI *p_write_NADI;      /* pointer to NADI write page           */
static UNGS_BUFFER_STATE ungs_buffer_state;
				    /* state variable for controling        */
				    /* the pointers to the 2 page           */
				    /* NADI buffer                          */
static int ungs_NADI_entries_max; /* max. no of entries into the NADI   */

static UIC_Group_short *p_Group[UIC_GROUP_MAX];
				    /* pointer array to the group           */
				    /* information tables                   */

#ifdef _UNGS_TRACE
extern FILE *tracefile;
extern char txt[128];
#endif

/*********************************************************************/
/*  Name:       ungs_buffer_control()                                */
/*  Function:   This function controls the access to the 2 paged     */
/*              NADI buffer. One page can be accessed for Updates.   */
/*              From the 2nd page NADI data can be read. 2 pointers  */
/*              are provided for read and write. The pointers are    */
/*              switched after an UPDATE just before a READ from     */
/*              NADI data. This state machine is called just before  */
/*              every NADI read as well as before and after every    */
/*              NADI write                                           */
/*              The special moment to switch the NADI page pointers  */
/*              occurs, when the update is completed, no write is in */
/*              progress and a read will start now.                  */
/*              Set NADI state                                       */
/*  remarks :   Since READ and WRITE may be performed simultaniously */
/*              this function access has to be protected by a disable*/
/*              Interrupt.                                           */
/*  returns :   a) read or write pointer to the NADI                 */
/*              b) in case of "set NADI status" the read or write    */
/*                 pointer is returned, when the status could be     */
/*                 written. OR the                                   */
/*                 null-pointer, when the status could not be set    */
/*              c) In case of init or the state change after a write */
/*                 to BUFFER_STATE_NADI_UPDATE_DONE the null-pointer */
/*                 is returned.                                      */
/*********************************************************************/

UIC_NADI *ungs_buffer_control (UNGS_BUFFER_TRIGGER trigger)
			   /* trigger indicates a read or write NADI */
			   /* action or a command to set the NADI    */
			   /* status                                 */
{
    UIC_NADI * p_tmp_NADI = NULL;     /* temporary pointer to NADI   */

    /* This function may not be interrupted by annother call, otherwise*/
    /* the states of the state machine and the pointers to the 2 paged */
    /* NADI buffer may be inconsistent. Therefore all interrupts are   */
    /* disabled for a few microseconds, while the processor executes   */
    /* one of the following cases.                                     */
#ifdef SPL
    int ilvl = splmac();
#else
    pi_disable();
#endif
    switch (trigger) {

    case BUFFER_INIT :
	/* Initialize state machine; the state transition to           */
	/* BUFFER_STATE_IDLE is allowed from every state               */
	ungs_buffer_state =  BUFFER_STATE_IDLE;
#ifdef SPL
	splx(ilvl);
#else
	pi_enable();
#endif
	break;

    case BUFFER_READ :
	/* Start reading NADI data; The state transition allowed :     */
	/* from BUFFER_STATE_NADI_UPDATE_DONE to BUFFER_STATE_IDLE     */
	/* At the same moment the pointers to the NADI pages will be   */
	/* switched !!!                                                */
	if ( BUFFER_STATE_NADI_UPDATE_DONE == ungs_buffer_state) {
	    ungs_buffer_state = BUFFER_STATE_IDLE;
	    p_tmp_NADI = p_write_NADI;
	    p_write_NADI = p_read_NADI;
	    p_read_NADI = p_tmp_NADI;
	}
	else {
	    p_tmp_NADI = p_read_NADI;
	}
#ifdef SPL
	splx(ilvl);
#else
	pi_enable();
#endif
	break;

    case BUFFER_WRITE :
	/* NADI update started; The state transition allowed: from    */
	/* BUFFER_STATE_IDLE to BUFFER_STATE_NADI_UPDATE_IN_PROGRESS  */
	if (( BUFFER_STATE_IDLE == ungs_buffer_state) ||
	    (BUFFER_STATE_NADI_UPDATE_DONE == ungs_buffer_state)) {
	    ungs_buffer_state = BUFFER_STATE_NADI_UPDATE_IN_PROGRESS;
	}
	p_tmp_NADI = p_write_NADI;
#ifdef SPL
	splx(ilvl);
#else
	pi_enable();
#endif
	break;

    case BUFFER_WRITE_DONE :
	/* NADI update completed; The unique state transition allowed: */
	/* from BUFFER_STATE_NADI_UPDATE_IN_PROGRESS to                */
	/* BUFFER_STATE_NADI_UPDATE_DONE                               */
	if ( BUFFER_STATE_NADI_UPDATE_IN_PROGRESS == ungs_buffer_state) {
	    ungs_buffer_state = BUFFER_STATE_NADI_UPDATE_DONE;
	}
#ifdef SPL
	splx(ilvl);
#else
	pi_enable();
#endif
	break;

    case SET_NADI_STATE_INVALID :
	/* Set NADI state; THIS IS ALLOWED IN STATE                   */
	/* BUFFER_STATE_IDLE where p_read points to                   */
	/* AND BUFFER_STATE_NADI_UPDATE_DONE where p_write points to  */
	if ( BUFFER_STATE_IDLE == ungs_buffer_state) {
	    p_tmp_NADI = p_read_NADI;
	    p_tmp_NADI->global_descr.nadi_state = UIC_NADI_STATE_INVALID;
	}
	if ( BUFFER_STATE_NADI_UPDATE_DONE == ungs_buffer_state) {
	    p_tmp_NADI = p_write_NADI;
	    p_tmp_NADI->global_descr.nadi_state = UIC_NADI_STATE_INVALID;
	}
#ifdef SPL
	splx(ilvl);
#else
	pi_enable();
#endif
	break;

    case SET_NADI_STATE_ACT :
	/* Set NADI state; THIS IS ALLOWED IN STATE                   */
	/* BUFFER_STATE_IDLE where p_read points to                   */
	/* AND BUFFER_STATE_NADI_UPDATE_DONE where p_write points to  */
	if ( BUFFER_STATE_IDLE == ungs_buffer_state) {
	    p_tmp_NADI = p_read_NADI;
	    p_tmp_NADI->global_descr.nadi_state = UIC_NADI_STATE_ACT;
	}
	if ( BUFFER_STATE_NADI_UPDATE_DONE == ungs_buffer_state) {
	    p_tmp_NADI = p_write_NADI;
	    p_tmp_NADI->global_descr.nadi_state = UIC_NADI_STATE_ACT;
       }
#ifdef SPL
	splx(ilvl);
#else
	pi_enable();
#endif
	break;

    case SET_NADI_STATE_CONF :
	/* Set NADI state; THIS IS ALLOWED IN STATE                   */
	/* BUFFER_STATE_IDLE where p_read points to                   */
	/* AND BUFFER_STATE_NADI_UPDATE_DONE where p_write points to  */
	if ( BUFFER_STATE_IDLE == ungs_buffer_state) {
	    p_tmp_NADI = p_read_NADI;
	    p_tmp_NADI->global_descr.nadi_state = UIC_NADI_STATE_CONF;
	}
	if ( BUFFER_STATE_NADI_UPDATE_DONE == ungs_buffer_state) {
	    p_tmp_NADI = p_write_NADI;
	    p_tmp_NADI->global_descr.nadi_state = UIC_NADI_STATE_CONF;
	}
#ifdef SPL
	splx(ilvl);
#else
	pi_enable();
#endif
	break;

    default:
	/* the default will be an error                                */
#ifdef SPL
	splx(ilvl);
#else
	pi_enable();
#endif
#ifdef  _UNGS_TRACE
	UNGSPRINT(txt,"ungs_buffer_control(): wrong trigger value %02x\n",trigger);
	UNGSTRACE(txt,tracefile);
#endif
	break;
    }
    return p_tmp_NADI;
}

/**************************************************************************/
/*  Name:       ungs_search_collective_address()                          */
/*  Function:   searches for TCN addresses of given collective UIC address*/
/*              in the NADI.                                              */
/*              The addresses are written into the structure of type      */
/*              UIC_CvtUicToTcnList                                       */
/*  returns :   void                                                      */
/**************************************************************************/

void ungs_search_collective_address(UIC_NADI            *p_nadi,
				    UIC_CvtUicToTcnList *p_cvt_reply)
{
    UNSIGNED8 mask_ts = 0;
    UNSIGNED8 mask_veh = 0;
    UNSIGNED8 mask_veh_add = 0;
    UNSIGNED8 offset_ts = 0;
    UNSIGNED8 offset_veh = 0;
    UNSIGNED8 offset_veh_add = 0;
    UNSIGNED8 nadi_i, tcn_i = 0;
    UNSIGNED8 RangeNo, NodeAddr;

    switch (p_cvt_reply->uic_addr_in) {
    case UIC_V_THIS:
	    p_cvt_reply->tcn_addr[tcn_i++] = AM_SAME_VEHICLE;
	break;
    case UIC_V_LEADING:
	offset_veh = UIC_VEH_INFO_LEADING_OFFSET;
	mask_veh   = UIC_VEH_INFO_LEADING_MASK;
	break;
    case UIC_V_LED_TRACTION_UNITS:
	/* the vehicle must have a motor, but no leading bit */
	offset_ts = UIC_TS_INFO_MOTOR_VEHICLE_OFFSET;
	mask_ts   = UIC_TS_INFO_MOTOR_VEHICLE_MASK_ELECTRIC |
			UIC_TS_INFO_MOTOR_VEHICLE_MASK_DIESEL;
	offset_veh = UIC_VEH_INFO_LEADING_OFFSET;
	mask_veh   = UIC_VEH_INFO_LEADING_MASK;
	for (nadi_i=0; nadi_i < p_nadi->global_descr.num_entries; nadi_i++) {
	    if ((p_nadi->p_veh_descr[nadi_i].TSInfo[offset_ts] & mask_ts) &&
		(!(p_nadi->p_veh_descr[nadi_i].VehDescr.veh_info[offset_veh] & mask_veh))) {
		p_cvt_reply->tcn_addr[tcn_i++] = p_nadi->p_veh_descr[nadi_i].NodeAddr;
	    }
	}
	mask_ts = mask_veh = 0;
	break;
    case UIC_V_ALL_VEHICLES:
	/* list all valid TCN addresses */
	for (nadi_i=0; nadi_i < p_nadi->global_descr.num_entries; nadi_i++) {
	    if (p_nadi->p_veh_descr[nadi_i].NodeAddr != INVALID_TCN_ADDRESS) {
		p_cvt_reply->tcn_addr[tcn_i++] = p_nadi->p_veh_descr[nadi_i].NodeAddr;
	    }
	}
	break;
    case UIC_V_ALL_VEH_PASS_TRANSPORT:
	/* vehicle must have seats or has to be a dining or a sleeping or a recumbency car */
	offset_veh = UIC_VEH_INFO_SEATS_OFFSET;
	mask_veh   = UIC_VEH_INFO_SEATS_MASK_1ST_CLASS |
			UIC_VEH_INFO_SEATS_MASK_2ND_CLASS;
	offset_veh_add = UIC_VEH_INFO_CARS_OFFSET;
	mask_veh_add   = UIC_VEH_INFO_CARS_MASK_DINING_CAR |
			UIC_VEH_INFO_CARS_MASK_SLEEPING_CAR |
			UIC_VEH_INFO_CARS_MASK_RECUMBENCY_CAR;
	for (nadi_i=0; nadi_i < p_nadi->global_descr.num_entries; nadi_i++) {
	    if ((p_nadi->p_veh_descr[nadi_i].VehDescr.veh_info[offset_veh] & mask_veh) ||
		((p_nadi->p_veh_descr[nadi_i].VehDescr.veh_info[offset_veh_add] & mask_veh_add))) {
		p_cvt_reply->tcn_addr[tcn_i++] = p_nadi->p_veh_descr[nadi_i].NodeAddr;
	    }
	}
	mask_veh = mask_veh_add = 0;
	break;
    case UIC_V_LAST:
	/* last vehicle must have the highest UIC range number */
	for (nadi_i=0, RangeNo=0, NodeAddr=INVALID_TCN_ADDRESS;
				nadi_i < p_nadi->global_descr.num_entries; nadi_i++) {
	    if (RangeNo < p_nadi->p_veh_descr[nadi_i].RangeNo) {
		RangeNo  = p_nadi->p_veh_descr[nadi_i].RangeNo;
		NodeAddr = p_nadi->p_veh_descr[nadi_i].NodeAddr;
	    }
	}
	if (NodeAddr != INVALID_TCN_ADDRESS) {
	    p_cvt_reply->tcn_addr[tcn_i++] = NodeAddr;
	}
	break;
    case UIC_V_ALL_MOTOR_VEHICLES:
	/* the vehicle must have a motor */
	offset_ts = UIC_TS_INFO_MOTOR_VEHICLE_OFFSET;
	mask_ts   = UIC_TS_INFO_MOTOR_VEHICLE_MASK_ELECTRIC |
			UIC_TS_INFO_MOTOR_VEHICLE_MASK_DIESEL;
	break;
    case UIC_V_ALL_LOCOMOTIVES:
	/* the vehicle must have a motor but no seats */
	offset_ts = UIC_TS_INFO_MOTOR_VEHICLE_OFFSET;
	mask_ts   = UIC_TS_INFO_MOTOR_VEHICLE_MASK_ELECTRIC |
			UIC_TS_INFO_MOTOR_VEHICLE_MASK_DIESEL;
	offset_veh = UIC_VEH_INFO_SEATS_OFFSET;
	mask_veh   = UIC_VEH_INFO_SEATS_MASK_1ST_CLASS |
			UIC_VEH_INFO_SEATS_MASK_2ND_CLASS;
	for (nadi_i=0; nadi_i < p_nadi->global_descr.num_entries; nadi_i++) {
	    if ((p_nadi->p_veh_descr[nadi_i].TSInfo[offset_ts] & mask_ts) &&
		(!(p_nadi->p_veh_descr[nadi_i].VehDescr.veh_info[offset_veh] & mask_veh))) {
		p_cvt_reply->tcn_addr[tcn_i++] = p_nadi->p_veh_descr[nadi_i].NodeAddr;
	    }
	}
	mask_ts = mask_veh = 0;
	break;
    case UIC_V_ALL_POWER_CARS:
	/* the vehicle must have a motor and seats */
	offset_ts = UIC_TS_INFO_MOTOR_VEHICLE_OFFSET;
	mask_ts   = UIC_TS_INFO_MOTOR_VEHICLE_MASK_ELECTRIC |
			UIC_TS_INFO_MOTOR_VEHICLE_MASK_DIESEL;
	offset_veh = UIC_VEH_INFO_SEATS_OFFSET;
	mask_veh   = UIC_VEH_INFO_SEATS_MASK_1ST_CLASS |
			UIC_VEH_INFO_SEATS_MASK_2ND_CLASS;
	for (nadi_i=0; nadi_i < p_nadi->global_descr.num_entries; nadi_i++) {
	    if ((p_nadi->p_veh_descr[nadi_i].TSInfo[offset_ts] & mask_ts) &&
		((p_nadi->p_veh_descr[nadi_i].VehDescr.veh_info[offset_veh] & mask_veh))) {
		p_cvt_reply->tcn_addr[tcn_i++] = p_nadi->p_veh_descr[nadi_i].NodeAddr;
	    }
	}
	mask_ts = mask_veh = 0;
	break;
    case UIC_V_ALL_COACHES:
	/* The vehicle must not have a motor nor must it be a freight car */
	offset_ts = UIC_TS_INFO_MOTOR_VEHICLE_OFFSET;
	mask_ts   = UIC_TS_INFO_MOTOR_VEHICLE_MASK_ELECTRIC |
			UIC_TS_INFO_MOTOR_VEHICLE_MASK_DIESEL;
	offset_veh = UIC_VEH_INFO_FREIGHT_CAR_OFFSET;
	mask_veh   = UIC_VEH_INFO_FREIGHT_CAR_MASK;
	for (nadi_i=0; nadi_i < p_nadi->global_descr.num_entries; nadi_i++) {
	    if (!(p_nadi->p_veh_descr[nadi_i].TSInfo[offset_ts] & mask_ts) &&
		(!(p_nadi->p_veh_descr[nadi_i].VehDescr.veh_info[offset_veh] & mask_veh))) {
		p_cvt_reply->tcn_addr[tcn_i++] = p_nadi->p_veh_descr[nadi_i].NodeAddr;
	    }
	}
	mask_ts = mask_veh = 0;
	break;
    case UIC_V_ALL_WITH_SEATS:
	offset_veh = UIC_VEH_INFO_SEATS_OFFSET;
	mask_veh   = UIC_VEH_INFO_SEATS_MASK_1ST_CLASS |
			UIC_VEH_INFO_SEATS_MASK_2ND_CLASS;
	break;
    case UIC_V_WITH_SEATS_1ST_CLASS:
	offset_veh = UIC_VEH_INFO_SEATS_OFFSET;
	mask_veh   = UIC_VEH_INFO_SEATS_MASK_1ST_CLASS;
	break;
    case UIC_V_WITH_SEATS_2ND_CLASS:
	offset_veh = UIC_VEH_INFO_SEATS_OFFSET;
	mask_veh   = UIC_VEH_INFO_SEATS_MASK_2ND_CLASS;
	break;
    case UIC_V_LAST_COACH:
	/* The vehicle must not have a motor nor must it be a freight car */
	RangeNo = 0;
	offset_ts = UIC_TS_INFO_MOTOR_VEHICLE_OFFSET;
	mask_ts   = UIC_TS_INFO_MOTOR_VEHICLE_MASK_ELECTRIC |
			UIC_TS_INFO_MOTOR_VEHICLE_MASK_DIESEL;
	offset_veh = UIC_VEH_INFO_FREIGHT_CAR_OFFSET;
	mask_veh   = UIC_VEH_INFO_FREIGHT_CAR_MASK;
	for (nadi_i=0, tcn_i = 0; nadi_i < p_nadi->global_descr.num_entries; nadi_i++) {
	    if (!(p_nadi->p_veh_descr[nadi_i].TSInfo[offset_ts] & mask_ts) &&
		(!(p_nadi->p_veh_descr[nadi_i].VehDescr.veh_info[offset_veh] & mask_veh))) {
		if ((p_nadi->p_veh_descr[nadi_i].NodeAddr != INVALID_TCN_ADDRESS) &&
		    (p_nadi->p_veh_descr[nadi_i].RangeNo > RangeNo)) {
		    p_cvt_reply->tcn_addr[0] = p_nadi->p_veh_descr[nadi_i].NodeAddr;
		    RangeNo = p_nadi->p_veh_descr[nadi_i].RangeNo;
		    tcn_i = 1;
		}
	    }
	}
	mask_ts = mask_veh = 0;
	break;
    case UIC_V_ALL_DINING_CARS:
	offset_veh = UIC_VEH_INFO_CARS_OFFSET;
	mask_veh   = UIC_VEH_INFO_CARS_MASK_DINING_CAR;
	break;
    case UIC_V_ALL_RECUMBENCY_CARS:
	offset_veh = UIC_VEH_INFO_CARS_OFFSET;
	mask_veh   = UIC_VEH_INFO_CARS_MASK_RECUMBENCY_CAR;
	break;
    case UIC_V_ALL_SLEEPING_CARS:
	offset_veh = UIC_VEH_INFO_CARS_OFFSET;
	mask_veh   = UIC_VEH_INFO_CARS_MASK_SLEEPING_CAR;
	break;
    case UIC_V_ALL_BUGGAGE_CARS:
	offset_veh = UIC_VEH_INFO_CARS_OFFSET;
	mask_veh   = UIC_VEH_INFO_CARS_MASK_BUGGAGE_CAR;
	break;
    case UIC_V_TCN_BUS_MASTER:
	p_cvt_reply->tcn_addr[0] = AM_SAME_VEHICLE; /* if single node */
	for (nadi_i=0; nadi_i < p_nadi->global_descr.num_entries; nadi_i++)
	{
	    if (p_nadi->p_veh_descr[nadi_i].NodeAddr == 0x01)
	    {
		p_cvt_reply->tcn_addr[0] = 0x01;
		break;
	    }
	}
	tcn_i = 1;
	break;
    default:
	break;
    }

    if (mask_ts) {
	for (nadi_i=0; nadi_i < p_nadi->global_descr.num_entries; nadi_i++) {
	    if (p_nadi->p_veh_descr[nadi_i].TSInfo[offset_ts] & mask_ts) {
		p_cvt_reply->tcn_addr[tcn_i++] = p_nadi->p_veh_descr[nadi_i].NodeAddr;
	    }
	}
    }
    if (mask_veh) {
	for (nadi_i=0; nadi_i < p_nadi->global_descr.num_entries; nadi_i++) {
	    if (p_nadi->p_veh_descr[nadi_i].VehDescr.veh_info[offset_veh] & mask_veh) {
		p_cvt_reply->tcn_addr[tcn_i++] = p_nadi->p_veh_descr[nadi_i].NodeAddr;
	    }
	}
    }
    p_cvt_reply->num_tcnaddr = tcn_i;
    return;
}

/**************************************************************************/
/*  Name:       ungs_search_group_address()                               */
/*  Function:   searches for TCN addresses of given group members in the  */
/*              NADI. The addresses are written into the structure of type*/
/*              UIC_CvtUicToTcnList                                       */
/*  returns :   void                                                      */
/**************************************************************************/

void ungs_search_group_address(UIC_NADI            *p_nadi,
			       UIC_CvtUicToTcnList *p_cvt_reply)
{
    UNSIGNED8 grp_i, nadi_i, tcn_i;
    UIC_Group_short *p_group;

    p_group = p_Group[p_cvt_reply->uic_addr_in - UIC_GROUP_BASE_ADDR];

    /* loop over group entries */
    for(grp_i=0, tcn_i=0; (p_group != NULL) &&
			  (grp_i < p_group->GroupDescr.num_entries); grp_i++) {

	/* loop over NADI entries */
	for(nadi_i=0; nadi_i<p_nadi->global_descr.num_entries; nadi_i++) {

	    if(0 == memcmp(&(p_nadi->p_veh_descr[nadi_i].VehDescr.uic_id.uic_id),
			   &(p_group->GroupVeh[grp_i].uic_id.uic_id), sizeof(TYPE_UIC_ID))){

		/* UIC-IDs match!!         */
		p_cvt_reply->tcn_addr[tcn_i++] = p_nadi->p_veh_descr[nadi_i].NodeAddr;
	    }
	}
    }
    p_cvt_reply->num_tcnaddr = tcn_i;

    return;
}


/**************************************************************************/
/*  Name:       ungs_update_GroupVeh()                                    */
/*  Function:   Read from NADI the TCN and UIC address for a given        */
/*              UIC-Identification and write it to the output.            */
/*              TCN is "INVALID_TCN_ADDRESS"and UIC is "0", when no entry */
/*              in the NADI for                                           */
/*              this UIC-ID is found or when the NADI is INVALID          */
/*  returns :   number of vehicle descriptions written into output        */
/**************************************************************************/

UNSIGNED8 ungs_update_GroupVeh(UIC_NADI      *p_NADI,   /* read pointer to NADI  */
			  UIC_GroupVeh_short *p_input,  /* pointer to GroupVeh element */
			  UIC_GroupVeh_long  *p_output) /* pointer to GroupVeh in reply*/
{
    int i;                                      /* loop index                  */
    UNSIGNED8 cnt = 1;
    UIC_VehDescr *p_NADI_vehDescr;              /* ptr to VehDescr in NADI     */
						/* NADI is invalid or unvalid  */
    p_output->uic_id = p_input->uic_id;         /* UIC-ID (TYPE_VehInfo) copied*/
						/* TCN and UIC addr. are "0"   */
    p_output->NodeAddr = INVALID_TCN_ADDRESS;   /* This is a preset, since it  */
    p_output->RangeNo  = 0;                     /* has to be done, because of a*/
						/* invalid NADI or a missing   */
						/* UIC-ID entry in the NADI    */

    if (p_NADI->global_descr.nadi_state != UIC_NADI_STATE_INVALID) {

	/* NADI is valid; now compare the uic_id (referenced by input) with the*/
	/* uic_ids stored in the Vehicle descriptors of the NADI. Since uic_id */
	/* is of TYPE_UIC_ID, a memcmp must be used. When memcmp indicates "0" */
	/* both uic_ids are identical and the TCN and UIC address are copied   */
	/* to output. If all uic_ids do not match, the preset of "0" occurred  */
	/* already and the function returns.                                   */

	for ( i = 0; i < p_NADI->global_descr.num_entries; i++ ) {
	    p_output->uic_id = p_input->uic_id;
	    p_output->NodeAddr = INVALID_TCN_ADDRESS;
	    p_output->RangeNo  = 0;
	    p_NADI_vehDescr = &(p_NADI->p_veh_descr[ i ]);
	    if(0 == memcmp( &(p_NADI_vehDescr->VehDescr.uic_id.uic_id),
			    &(p_input->uic_id), sizeof( TYPE_UIC_ID ) ) ){
		/* UIC-IDs match!!         */
		p_output->NodeAddr = p_NADI_vehDescr->NodeAddr;
		p_output->RangeNo  = p_NADI_vehDescr->RangeNo;
		p_output++;
		cnt++;

		/* The loop can not be left right now. The complete      */
		/* NADI must be searched, since the identical UIC-ID     */
		/* may have the identical UIC address, but different TCNs*/
	    }
	}
    }
    if (cnt > 1) {
	cnt--;
    }
    return cnt;
}


/*********************************************************************/
/*  Name:       ungs_NADI_read()                                     */
/*  Function:   Read the NADI                                        */
/*              Check the NADI size against the reply buffer size    */
/*              copy the UIC_Header, UIC_GlobalDescr and             */
/*              UIC_VehDescr into the reply message buffer           */
/*  returns :   UNGS_OK , when everything went well                  */
/*              UIC_S_ALLOC_ERR, when allocation for the reply failed*/
/*********************************************************************/


UNGS_RESULT ungs_NADI_read( UIC_ReadNadi **p_msg,     /* pointer, where to return the NADI */
		    UNSIGNED32   *reply_size) /* size of returned msg in bytes     */
{
    int retval = UNGS_OK;                    /* return code                       */
    UIC_NADI *p_read;                        /* pointer to NADI (Stack variable)  */
    int cnt;
    UNSIGNED32 size;
    UIC_ReadNadi *p_NadiMsg;

    /* invoke the control of the 2 page NADI*/
    p_read = ungs_buffer_control( BUFFER_READ );

    /* size of reply has to be computed     */
    size = (sizeof(UIC_ReadNadi) +
	(p_read->global_descr.num_entries * sizeof(UIC_VehDescr)));
    /* allocate memory for reply */
    if( (p_NadiMsg = (UIC_ReadNadi*)PI_ALLOC(size)) == NULL ) {
	retval = UIC_S_ALLOC_ERR;
	*reply_size = 0;
    }
    else {
	p_NadiMsg->hdr.state_cmd = UNGS_OK;
	/* I) global part                       */
	p_NadiMsg->nadi.global_descr = p_read->global_descr;

	/* II) vehicle specific part            */
	for( cnt = 0; cnt < p_read->global_descr.num_entries; cnt++ ){
	    p_NadiMsg->nadi.veh_descr[ cnt ] = p_read->p_veh_descr[ cnt ];
	}
	/* recalculate size */
	size = UIC_SIZEOF_UIC_READNADI(p_read->global_descr.num_entries);
	*reply_size = size + (size & 0x01);
    }
    *p_msg = p_NadiMsg;
    return retval;
}

/*********************************************************************/
/*  Name:       ungs_read_nadi()                                     */
/*  Function:   Read the NADI, copy NADI in caller's buffer          */
/*********************************************************************/

void ungs_read_nadi( UIC_Nadi *p_nadi)
{
    UIC_NADI *p_read;            /* pointer to NADI (Stack variable)  */
    int cnt;

    /* invoke the control of the 2 page NADI*/
    p_read = ungs_buffer_control( BUFFER_READ );

    p_nadi->global_descr = p_read->global_descr;

    for( cnt = 0; cnt < p_read->global_descr.num_entries; cnt++ ){
	p_nadi->veh_descr[ cnt ] = p_read->p_veh_descr[ cnt ];
    }
}


/*********************************************************************/
/*  Name:       ungs_NADI_write()                                    */
/*  Function:   Write the NADI                                       */
/*              Check if the number of vehicles in the call buffer   */
/*              would fit into the allocated NADI space              */
/*              copy UIC_GlobalDescr and all                         */
/*              UIC_VehDescr into the NADI                           */
/*  returns :   UNGS_OK , when everything went well                  */
/*********************************************************************/

UNGS_RESULT ungs_NADI_write ( UIC_NADI *p_input_nadi ) /* pointer to received NADI msg*/
{
    int cnt;
    UIC_NADI *p_write;
    int retval = UNGS_OK;
    /* do not exceed the max. num. vehicles */
    if ( p_input_nadi->global_descr.num_entries <= ungs_NADI_entries_max ) {
       /* invoke the control of the 2 page NADI*/
       p_write = ungs_buffer_control( BUFFER_WRITE );

       /* I) global part                       */
       p_write->global_descr = p_input_nadi->global_descr;

       /* II) vehicle specific part in a loop  */
       for( cnt = 0; cnt < p_input_nadi->global_descr.num_entries; cnt++ ){
	   p_write->p_veh_descr[cnt] = p_input_nadi->p_veh_descr[cnt];
       }

#ifdef UNGS_NADI_TESTAUSGABE

       printf( "NADI in function 'ungs_NADI_write()': \n" );
       NADI_Testausgabe( p_write );

#endif

       /* invoke the control of the 2 page NADI */
       ungs_buffer_control( BUFFER_WRITE_DONE );
    }
    else {
	/* The number of NADI entries exceeds the memory allocated */
	/* in the initialization phase                             */
	retval = UIC_S_NADI_TOO_SMALL;
    }
    return retval;
}


/*********************************************************************/
/*  Name:       ungs_UIC_TCN_convert()                               */
/*  Function:   Converts the UIC address into TCN address            */
/*              Check if the UIC address is valid                    */
/*  returns :   UNGS_OK , when everything went well                  */
/*              UIC_S_ALLOC_ERR, when the allocation of the reply    */
/*                               message buffer failed               */
/*********************************************************************/

UNGS_RESULT ungs_UIC_TCN_convert(UNSIGNED8           uicAddress,    /* UIC Address   */
			 UIC_CvtUicToTcnList **p_reply,     /* pointer to reply */
			 UNSIGNED32          *p_reply_size) /* size of reply */
{
    int retval = UNGS_OK;
    UIC_NADI *p_NADI;
    UIC_CvtUicToTcnList *p_cvt_reply = NULL;
    int i, size;                               /* loop index in NADI    */
    UNSIGNED8 j = 0;                           /* index in reply buffer */
    UNSIGNED8 tmp_tcn_addr;

    /* invoke the control of the 2 page NADI */
    p_NADI = ungs_buffer_control( BUFFER_READ );

    /* size of reply has to be computed     */
    size = sizeof(UIC_CvtUicToTcnList) + ungs_NADI_entries_max * sizeof(UNSIGNED8);
    /* allocate memory for reply */
    if( (p_cvt_reply = (UIC_CvtUicToTcnList*)PI_ALLOC(size)) == NULL )
    {
	retval = UIC_S_ALLOC_ERR;
	size = 0;
    }
    else
    {
	if(p_NADI->global_descr.nadi_state == UIC_NADI_STATE_INVALID)
	{
	    p_cvt_reply->hdr.state_cmd = UNGS_OK;
	    p_cvt_reply->uic_addr_in  = uicAddress;
	    p_cvt_reply->topo_counter = p_NADI->global_descr.topo_counter;
	    p_cvt_reply->num_tcnaddr = 0;
	    p_cvt_reply->tcn_addr[0] = INVALID_TCN_ADDRESS;
	}
	else
	{
	    p_cvt_reply->hdr.state_cmd = UNGS_OK;
	    p_cvt_reply->uic_addr_in  = uicAddress;
	    p_cvt_reply->topo_counter = p_NADI->global_descr.topo_counter;
	    p_cvt_reply->num_tcnaddr = 0;
	    p_cvt_reply->tcn_addr[0] = INVALID_TCN_ADDRESS;
	    if (uicAddress <= UIC_INDIVIDUAL_ADDR_LIMIT)
	    {
		/* address range for UIC individual addresses */
		for(i = 0; i < p_NADI->global_descr.num_entries; i++ )
		{
		    if(p_NADI->p_veh_descr[ i ].RangeNo == uicAddress)
		    {
			/* UIC-IDs match!!                     */
			p_cvt_reply->tcn_addr[j++] = p_NADI->p_veh_descr[ i ].NodeAddr;
		    }
		}
		p_cvt_reply->num_tcnaddr = j;
	    }
	    else
	    {
		if (uicAddress <= UIC_COLLECTIVE_ADDR_LIMIT)
		{
		    /* address range for UIC collective addresses */
		    ungs_search_collective_address(p_NADI, p_cvt_reply);
		}
		else
		{
		    if (uicAddress <= UIC_GROUP_ADDR_LIMIT)
		    {
			/* address range for UIC collective addresses */
			ungs_search_group_address(p_NADI, p_cvt_reply);
		    }
		}
	    }
	    /* delete all multiple entries of TCN addresses in case of a trainset */
	    /* i is read index, j will be the write index */
	    for (i=0,j=0,tmp_tcn_addr=INVALID_TCN_ADDRESS; i<p_cvt_reply->num_tcnaddr; i++)
	    {
		if (tmp_tcn_addr != p_cvt_reply->tcn_addr[i])
		{
		    p_cvt_reply->tcn_addr[j++] = p_cvt_reply->tcn_addr[i];
		    tmp_tcn_addr = p_cvt_reply->tcn_addr[i];
		}
	    }
	    p_cvt_reply->num_tcnaddr = j;
	}
	/* recalculate size */
	size = UIC_SIZEOF_UIC_CVTUICTOTCNLIST(p_cvt_reply->num_tcnaddr);
    }
    *p_reply = p_cvt_reply;
    /* even reply buffer size */
    *p_reply_size = size + (size & 0x01);
    return retval;
}


/*********************************************************************/
/*  Name:       ungs_GROUP_read()                                    */
/*  Function:   Read the group information                           */
/*  returns :   UNGS_OK , when everything went well                  */
/*              UIC_S_ALLOC_ERR, when the allocation of the reply    */
/*                               message buffer failed               */
/*********************************************************************/

UNGS_RESULT ungs_GROUP_read(UNSIGNED8          index,       /* Group Identification*/
		    UIC_ReadGroupReply **p_reply,      /* pointer to reply */
		    UNSIGNED32         *p_reply_size)  /* size of reply    */

{
    int retval = UNGS_OK;
    UNSIGNED32 size;
    UNSIGNED8 cnt;
    UIC_NADI *p_read;
    UIC_ReadGroupReply *p_group_reply;
    int i;
					       /* index = Group_address -    */
					       /* group base address e.g. 201*/
    index = (UNSIGNED8)(index - UIC_GROUP_BASE_ADDR);

    /* size of reply has to be computed     */
    size = sizeof(UIC_ReadGroupReply) +
	    (UIC_GROUP_ENTRIES_MAX + ungs_NADI_entries_max) * sizeof(UIC_GroupVeh_long);
    /* allocate memory for reply */
    if( (p_group_reply = (UIC_ReadGroupReply*)PI_ALLOC(size)) == NULL )
    {
	retval = UIC_S_ALLOC_ERR;
	size = 0;
    }
    else {
	p_group_reply->hdr.state_cmd = UNGS_OK;
	if ((index < UIC_GROUP_MAX ) && (p_Group[index] != NULL))
	{
	    /* index is OK and group exists           */
	    /* I) Group descriptor part */
	    p_group_reply->group.GroupDescr = p_Group[index]->GroupDescr;

	    /*  Now add the NodeAddr (TCN) and RangeNO (UIC)   */
	    /*  to the vehicle specific part. The information  */
	    /*  is stored in the NADI. First thing is to       */
	    /*  invoke the control of the 2 page NADI          */
	    cnt = 0;
	    p_read = ungs_buffer_control( BUFFER_READ );
	    for( i = 0; i < p_Group[index]->GroupDescr.num_entries; i++ )
	    {
		cnt = (UNSIGNED8)(cnt + ungs_update_GroupVeh(p_read,
					&(p_Group[index]->GroupVeh[i]),
					&(p_group_reply->group.GroupVeh[ cnt ])));
	    }
	    p_group_reply->group.GroupDescr.num_entries = cnt;
	    /* recalculate size */
	    size = UIC_SIZEOF_UIC_READGROUPREPLY(cnt);
	}
	else {
	    /* Group does not exist*/
	    p_group_reply->hdr.state_cmd = UIC_S_GROUP_ERR;
	    /* recalculate size */
	    size = UIC_HEADER_BYTES;
	    retval = UIC_S_GROUP_ERR;
	}
    }

    *p_reply = p_group_reply;
    /* even reply buffer size */
    size += size & 0x01;
    *p_reply_size = size;

    return retval;
}


/*********************************************************************/
/*  Name:       ungs_GROUPLIST_read()                                */
/*  Function:   Read the listed groups information                   */
/*  returns :   UNGS_OK , when everything went well                  */
/*              UIC_S_GROUP_ERR in case of a wrong group ID          */
/*              UIC_S_ALLOC_ERR, when the allocation of the reply    */
/*                               message buffer failed               */
/*********************************************************************/

UNGS_RESULT ungs_GROUPLIST_read(UIC_GroupListReply **p_reply,      /* pointer to reply */
			UNSIGNED32         *p_reply_size)  /* size of reply    */

{
    int retval = UNGS_OK;
    UNSIGNED32 size;
    UNSIGNED8 group_cnt;
    UIC_GroupListReply *p_group_list_reply;
    int group_loop, i;

    /* size of reply has to be computed     */
    for (group_loop=0, size = sizeof(UIC_GroupListReply); group_loop < UIC_GROUP_MAX; group_loop++) {
	if (p_Group[group_loop] != NULL) {
	    size += sizeof(UIC_Group_short);
	}
    }
    /* allocate memory for reply */
    if( (p_group_list_reply = (UIC_GroupListReply*)PI_ALLOC(size)) == NULL ) {
	retval = UIC_S_ALLOC_ERR;
	size = 0;
    }
    else {
	p_group_list_reply->hdr.state_cmd = UNGS_OK;
	for (group_loop = 0, group_cnt = 0; group_loop < UIC_GROUP_MAX; group_loop++) {
	    if (p_Group[group_loop] != NULL) {
		/* I) Group descriptor part */
		p_group_list_reply->groupli.group[group_cnt].GroupDescr =
						    p_Group[group_loop]->GroupDescr;

		/* vehicle descriptions */
		for( i = 0; i < p_Group[group_loop]->GroupDescr.num_entries; i++ ){
		p_group_list_reply->groupli.group[group_cnt].GroupVeh[i] =
						    p_Group[group_loop]->GroupVeh[i];
		}
		group_cnt++;
	    } /* end of p_Group != NULL */
	} /* end of group_loop */
	p_group_list_reply->groupli.num_groups = group_cnt;
	/* recalculate size */
	size = UIC_SIZEOF_UIC_GROUPLISTREPLY(group_cnt);
    }

    *p_reply = p_group_list_reply;
    /* even reply buffer size */
    size += size & 0x01;
    *p_reply_size = size;

    return retval;
}

/*********************************************************************/
/*  Name:       ungs_Group_write()                                   */
/*  Function:   Write the Group information                          */
/*              Check if the Group ID is valid                       */
/*              When the group not already exists, free the buffer   */
/*              allocated the required memory space for the group    */
/*              copy UIC_GroupDescr and all UIC_VehDescr             */
/*  returns :   UNGS_OK , when everything went well                  */
/*              UIC_S_ALLOC_ERR, when the allocation went wrong      */
/*              UIC_S_GROUP_ERR, upon an invalid Group ID            */
/*********************************************************************/

UNGS_RESULT ungs_GROUP_write(UIC_Group_short *p_input,    /* pointer to a group */
		     UIC_Msg         **p_reply,
		     UNSIGNED32      *p_reply_size)
{
    int retval = UNGS_OK;
    int i, size;
    UIC_Msg *p_msg = NULL;
    UNSIGNED8 index =  (UNSIGNED8)(p_input->GroupDescr.Group_id - UIC_GROUP_BASE_ADDR);
			     /* index = Group_id - group base address */
    if (((index < UIC_GROUP_MAX) &&
	(p_input->GroupDescr.num_entries <= UIC_GROUP_ENTRIES_MAX)) &&
	(p_Group[index] == NULL)) {
	/* index is OK allocate memory for the new group   */
	p_Group[index] = (UIC_Group_short*) PI_ALLOC (sizeof(UIC_Group_short) +
		  p_input->GroupDescr.num_entries * sizeof(UIC_GroupVeh_short));

	if (p_Group[index] != NULL)
	{   /* alloc was successf.*/

	    /* I) Group descriptor part */
	    p_Group[index]->GroupDescr = p_input->GroupDescr;

	    for( i = 0; i < p_input->GroupDescr.num_entries; i++ ){
		p_Group[index]->GroupVeh[i] = p_input->GroupVeh[i];
	    }
	}
	else { /* alloc error        */
	    retval = UIC_S_ALLOC_ERR;
	}
    }
    else { /* Group ID error     */
	retval = UIC_S_GROUP_ERR;
    }
    /* size of reply has to be computed     */
    size = UIC_HEADER_BYTES;
    /* even reply buffer size */
    size += size & 0x01;
    /* allocate memory for reply */
    if( (p_msg = (UIC_Msg*)PI_ALLOC(size)) == NULL ) {
	retval = UIC_S_ALLOC_ERR;
	size = 0;
    }
    else {
	p_msg->hdr.state_cmd = (UNSIGNED8)retval;
	retval = UNGS_OK;
    }
    *p_reply = p_msg;
    *p_reply_size = size;

    return retval;
}

/*********************************************************************/
/*  Name:       ungs_GroupList_write()                               */
/*  Function:   Write the listed Group information                   */
/*              Check if the Group ID is valid                       */
/*              When the group not already exists, free the buffer   */
/*              allocated the required memory space for the group    */
/*              copy UIC_GroupDescr and all UIC_VehDescr             */
/*  returns :   UNGS_OK , when everything went well                 */
/*              UIC_S_ALLOC_ERR, when the allocation went wrong      */
/*              UIC_S_GROUP_ERR, upon an invalid Group ID            */
/*********************************************************************/

UNGS_RESULT ungs_GROUPLIST_write(UIC_GroupList *p_input,    /* list of groups */
			 UIC_Msg       **p_reply,
			 UNSIGNED32    *p_reply_size)
{
    int retval = UNGS_OK;
    int i, group_loop, size;
    UIC_Msg *p_msg = NULL;
    UNSIGNED8 index;

    if (p_input->num_groups <= UIC_GROUP_MAX) {
	for (group_loop = 0; group_loop < p_input->num_groups; group_loop++) {
	    /* for each group in the list */
	    index =  (UNSIGNED8)(p_input->group[group_loop].GroupDescr.Group_id -
							    UIC_GROUP_BASE_ADDR);
			     /* index = Group_id - group base address */
	    if (((index < UIC_GROUP_MAX) &&
		(p_input->group[group_loop].GroupDescr.num_entries <= UIC_GROUP_ENTRIES_MAX)) &&
		(p_Group[index] == NULL)) {
		/* index is OK allocate memory for the new group   */
		p_Group[index] = (UIC_Group_short*) PI_ALLOC (sizeof(UIC_Group_short) +
			    p_input->group[group_loop].GroupDescr.num_entries * sizeof(UIC_GroupVeh_short));

		if (p_Group[index] != NULL) {
		    /* alloc was successf.*/

		    /* I) Group descriptor part */
		    p_Group[index]->GroupDescr = p_input->group[group_loop].GroupDescr;

		    for( i = 0; i < p_input->group[group_loop].GroupDescr.num_entries; i++ ){
			p_Group[index]->GroupVeh[i] = p_input->group[group_loop].GroupVeh[i];
		    }
		}
		else { /* alloc error        */
		    retval = UIC_S_ALLOC_ERR;
		    break;
		}
	    }
	    else { /* Group ID error     */
		retval = UIC_S_GROUP_ERR;
		break;
	    }
	} /* end of group loop */
    }
    else { /* too many groups */
	retval = UIC_S_GROUP_ERR;
    }
    /* size of reply has to be computed     */
    size = UIC_HEADER_BYTES;
    /* even reply buffer size */
    size += size & 0x01;
    /* allocate memory for reply */
    if( (p_msg = (UIC_Msg*)PI_ALLOC(size)) == NULL ) {
	retval = UIC_S_ALLOC_ERR;
	size = 0;
    }
    else {
	p_msg->hdr.state_cmd = (UNSIGNED8)retval;
	retval = UNGS_OK;
    }
    *p_reply = p_msg;
    *p_reply_size = size;

    return retval;
}

/*********************************************************************/
/*  Name:       ungs_GROUP_delete()                                  */
/*  Function:   deletes the group information for a specified group; */
/*              frees previously allocated memory                    */
/*              resets the pointer to the group information          */
/*  returns :   UNGS_OK , when no error                              */
/*              UIC_S_GROUP_ERR, when invalid group ID               */
/*              UIC_S_ALLOC_ERR, when the allocation of the reply    */
/*                               message buffer failed               */
/*********************************************************************/

UNGS_RESULT ungs_GROUP_delete(UNSIGNED8  group_id,       /* Group Identification*/
		      UIC_Msg    **p_reply,      /* pointer to reply */
		      UNSIGNED32 *p_reply_size)  /* size of reply    */
{
    int size, retval = UNGS_OK;
    UIC_Header *p_msg;
    group_id = (UNSIGNED8)(group_id - UIC_GROUP_BASE_ADDR);
    /* size of reply has to be computed     */
    size = sizeof(UIC_Header);
    /* allocate memory for reply */
    if( (p_msg = (UIC_Header*)PI_ALLOC(size)) == NULL ) {
	retval = UIC_S_ALLOC_ERR;
	size = 0;
    }
    else {
	p_msg->state_cmd = UNGS_OK;
	if (group_id < UIC_GROUP_MAX) {
	    if (p_Group[group_id] != NULL) {
		PI_FREE(p_Group[group_id]);     /* free allocated memory */
		p_Group[group_id] = NULL;       /* clear pointer     */
	    }
	    else {
		p_msg->state_cmd = UIC_S_GROUP_ERR; /* No memory allocated   */
	    }
	}
	else {
	    p_msg->state_cmd = UIC_S_GROUP_ERR;     /* invalid group ID      */
	}
    }
    size = UIC_HEADER_BYTES;
    /* even reply buffer size */
    size += size & 0x01;
    *p_reply = (UIC_Msg*)p_msg;
    *p_reply_size = size;
    return retval;
}


/*********************************************************************/
/*  Name:       ungs_GROUPALL_delete()                               */
/*  Function:   deletes the all group information for all groups;    */
/*              frees previously allocated memory                    */
/*              resets the pointer to the group information          */
/*  returns :   UNGS_OK , when no error                              */
/*              UIC_S_ALLOC_ERR, when the allocation of the reply    */
/*                               message buffer failed               */
/*********************************************************************/

UNGS_RESULT ungs_GROUPALL_delete(UIC_Msg    **p_reply,      /* pointer to reply */
			 UNSIGNED32 *p_reply_size)  /* size of reply    */
{
    int size, retval = UNGS_OK;
    UIC_Header *p_msg;
    UNSIGNED8 group_id;
    /* size of reply has to be computed     */
    size = sizeof(UIC_Msg);
    /* even reply buffer size */
    size += size & 0x01;
    /* allocate memory for reply */
    if( (p_msg = (UIC_Header*)PI_ALLOC(size)) == NULL ) {
	retval = UIC_S_ALLOC_ERR;
	size = 0;
    }
    else {
	p_msg->state_cmd = UNGS_OK;
	for (group_id = 0; group_id < UIC_GROUP_MAX; group_id++) {
	    if (p_Group[group_id] != NULL) {
		PI_FREE(p_Group[group_id]);     /* free allocated memory */
		p_Group[group_id] = NULL;       /* clear pointer     */
	    }
	}
    }
    size = UIC_HEADER_BYTES;
    /* even reply buffer size */
    size += size & 0x01;
    *p_reply = (UIC_Msg*)p_msg;
    *p_reply_size = size;
    return retval;
}



/*********************************************************************/
/*  Name:       ungs_init()                                          */
/*  Function:   Initialization of NADI and Group server.             */
/*              Allocation of memory for NADI and group information. */
/*              Assumption is that there will be no shutdown function*/
/*              to free the allocated memory (Always power off).     */
/*              Preset NADI read and write pointer.                  */
/*  returns :   UNGS_OK , when everything went well                  */
/*              UIC_S_ALLOC_ERR, when memory could not be allocated. */
/*********************************************************************/

UNGS_RESULT ungs_init(ums_NodeConfigurationTable * p_ungs_ct)
{                                        /* configuration structure  */
    int i;                               /* loop index               */
    int retval = UNGS_OK;                /* funcion's return code    */
    int nadi_size;                       /* max. NADI size allocated */

	/* Get number of elements from the NSDB */
    ungs_NADI_entries_max = p_ungs_ct->num_NADI_entries;
					      /* NADI element found  */

    nadi_size = (ungs_NADI_entries_max * sizeof(UIC_VehDescr));

    /* allocate memory for the maximal size */
    /* 1st page                             */
    ungs_NADI[0].p_veh_descr = (UIC_VehDescr*) PI_ALLOC(nadi_size);

    /* allocate memory for the maximal size */
    /* 2nd page                             */
    ungs_NADI[1].p_veh_descr = (UIC_VehDescr*) PI_ALLOC(nadi_size);

    if (((UIC_VehDescr*)0 == ungs_NADI[0].p_veh_descr)||
	((UIC_VehDescr*)0 == ungs_NADI[1].p_veh_descr)){
					    /* Allocation failed     */
	if ((UIC_VehDescr*)0 != ungs_NADI[0].p_veh_descr){
	    PI_FREE ( ungs_NADI[0].p_veh_descr );
	}
	if ((UIC_VehDescr*)0 != ungs_NADI[1].p_veh_descr){
	    PI_FREE ( ungs_NADI[1].p_veh_descr );
	}
	retval = UIC_S_ALLOC_ERR;
	p_read_NADI = (UIC_NADI*)0;
	p_write_NADI = (UIC_NADI*)0;
    }
    else {
	p_read_NADI = &ungs_NADI[0];            /* read pointer          */
	p_write_NADI = &ungs_NADI[1];           /* write pointer         */

	ungs_NADI[0].global_descr.nadi_state = UIC_NADI_STATE_INVALID;
	ungs_NADI[0].global_descr.num_entries = 0;
	ungs_NADI[1].global_descr.nadi_state = UIC_NADI_STATE_INVALID;
	ungs_NADI[1].global_descr.num_entries = 0;

	/* invoke the control of the 2 page NADI */
	ungs_buffer_control( BUFFER_INIT );

	/* Initialization of the Group pointers */
	for (i = 0; i < UIC_GROUP_MAX; i ++ ) {
	    p_Group[i] = NULL;
	}
    }
    return retval;
}


/*********************************************************************/
/*  Name:       ungs_request                                         */
/*  Function:   global NADI server interface                         */
/*              The requested service is encoded in the uic_callmsg. */
/*              Services are read NADI, write NADI,                  */
/*              convert UIC address to TCN addres, set state of NADI,*/
/*              read, write or delete group information              */
/* Return codes:    UNGS_OK                                          */
/*                  UIC_S_REPLY_ERROR, when the ptr to thr reply     */
/*                  buffer is invalid or the length of the reply     */
/*                  buffer is too small.                             */
/*                  Otherwise the return code of the functions called*/
/*                  by ungs_request is returned.                     */
/*                  UIC_S_ALLOC_ERR, when the allocation of the reply*/
/*                               message buffer failed               */
/*********************************************************************/

UNGS_RESULT ungs_request(void        *p_uic_callmsg,        /* pointer to input message    */
			 CARDINAL32  uic_callmsg_size,      /* size of message in bytes    */
			 AM_ADDRESS  *p_caller,             /* am_caller                   */
			 void        **p_uic_replymsg,      /* pointer to reply message    */
			 CARDINAL32  *p_uic_replymsg_size,  /* size of reply message       */
			 REPLY_FREE  *p_reply_free_fct)     /* pointer to mem free function*/
{
    UIC_Msg* p_uic_call = (UIC_Msg*)p_uic_callmsg;
    int retval = UNGS_OK;
    UNSIGNED8 *p_byte_in;
    UNSIGNED32 size;

    UIC_Msg *p_msg, *p_ReadGroupListCall, *p_DeleteGroupReply;
    UIC_ReadNadi *p_NadiMsg;
    UIC_CvtUicToTcn *p_UicCvtCall;
    UIC_CvtUicToTcnList *p_UicCvtReply;
    UIC_WriteGroup *p_WriteGroupCall;
    UIC_ReadGroup *p_ReadGroupCall;
    UIC_ReadGroupReply *p_ReadGroupReply;
    UIC_DeleteGroup * p_DeleteGroup;
    UIC_WriteGroupList *p_WriteGroupList;
    UIC_GroupListReply *p_ReadGroupListReply;

    UNREFERENCED(uic_callmsg_size);
    UNREFERENCED(p_caller);

    *p_reply_free_fct = PI_FREE;
    p_byte_in = p_uic_callmsg;

	/* decode UIC service */
    switch( NTOH16(p_byte_in + UIC_HEADER_CMD_OFFSET )){

    case UIC_FC_READ_NADI:
#ifndef O_CONV
	/* The conversion of the call  message  */
	if ((p_msg = (UIC_Msg*)PI_ALLOC(sizeof(UIC_Msg))) == NULL) {
	    retval = UIC_S_ALLOC_ERR;
	}
	else {
	    retval = ungs_NADI_read(&p_NadiMsg, &size);
	    if (retval != UIC_S_ALLOC_ERR) {
		/* Convert the call message from a bytestream */
		wtbconv_00_01__NTOH ((UNSIGNED8*)p_uic_callmsg, &p_msg->hdr);
		/* prepare the uic reply header */
		exchange_UIC_HEADER((UIC_Msg*)p_NadiMsg,p_msg);
		/* Allocate memory for the bytestream reply */
		if ((*p_uic_replymsg = PI_ALLOC(size)) == NULL) {
		    retval = UIC_S_ALLOC_ERR;
		}
		else {
		    /* Convert the structured message into a bytestream */
		    wtbconv_00_01A_HTON ((UNSIGNED8*)(*p_uic_replymsg), p_NadiMsg);
		    *p_uic_replymsg_size = size;
		}
		/* free the structured message allocated in ungs_NADI_read() */
		PI_FREE (p_NadiMsg);
	    }
	    PI_FREE (p_msg);
	}
#else  /* no message conversion required */

	retval = ungs_NADI_read(&p_NadiMsg, &size);
	if (retval != UIC_S_ALLOC_ERR) {
	    /* prepare the uic reply header */
	    exchange_UIC_HEADER((UIC_Msg*)p_NadiMsg,p_uic_callmsg);
	    *p_uic_replymsg = p_NadiMsg;
	    *p_uic_replymsg_size = size;
	}
#endif
       break;

    case UIC_FC_CVT_UIC_TO_TCN:
#ifndef O_CONV
	/* The conversion of the call  message  */
	if ((p_UicCvtCall = (UIC_CvtUicToTcn*)PI_ALLOC(sizeof(UIC_Msg))) == NULL) {
	    retval = UIC_S_ALLOC_ERR;
	}
	else {
	    /* Convert the call message from a bytestream */
	    wtbconv_32_01_02__NTOH ((UNSIGNED8*)p_uic_callmsg, p_UicCvtCall);
	    retval = ungs_UIC_TCN_convert(p_UicCvtCall->uic_addr_in,
						    &p_UicCvtReply, &size);
	    if (retval != UIC_S_ALLOC_ERR ) {
		/* prepare the uic reply header */
		exchange_UIC_HEADER((UIC_Msg*)p_UicCvtReply,(UIC_Msg*)p_UicCvtCall);
		/* Allocate memory for the bytestream reply */
		if ((*p_uic_replymsg = PI_ALLOC(size)) == NULL) {
		    retval = UIC_S_ALLOC_ERR;
		}
		else {
		    /* Convert the structured message into a bytestream */
		    wtbconv_32_01_02A_HTON ((UNSIGNED8*)(*p_uic_replymsg), p_UicCvtReply);
		    *p_uic_replymsg_size = size;
		}
		/* free the structured message allocated in ungs_UIC_TCN_convert() */
		PI_FREE (p_UicCvtReply);
	    }
	    PI_FREE (p_UicCvtCall);
	}
#else   /* no message conversion required */
	/* Convert the call message from a bytestream */
	p_UicCvtCall = (UIC_CvtUicToTcn *)p_uic_callmsg;
	retval = ungs_UIC_TCN_convert(p_UicCvtCall->uic_addr_in,
						    &p_UicCvtReply, &size);
	if (retval != UIC_S_ALLOC_ERR ) {
	    /* prepare the uic reply header */
	    exchange_UIC_HEADER((UIC_Msg*)p_UicCvtReply,(UIC_Msg*)p_UicCvtCall);
	    *p_uic_replymsg = p_UicCvtReply;
	    *p_uic_replymsg_size = size;
	}
#endif
	break;

    case UIC_FC_READ_GROUP:
#ifndef O_CONV
	/* The conversion of the call  message  */
	if ((p_ReadGroupCall = (UIC_ReadGroup*)PI_ALLOC(sizeof(UIC_ReadGroup))) == NULL) {
	    retval = UIC_S_ALLOC_ERR;
	}
	else {
	    /* Convert the call message from a bytestream */
	    wtbconv_11_01__NTOH ((UNSIGNED8*)p_uic_callmsg, p_ReadGroupCall);
	    retval = ungs_GROUP_read(p_ReadGroupCall->group_number,
						    &p_ReadGroupReply, &size);
	    if (retval != UIC_S_ALLOC_ERR ) {
		/* prepare the uic reply header */
		exchange_UIC_HEADER((UIC_Msg*)p_ReadGroupReply,(UIC_Msg*)p_ReadGroupCall);
		/* Allocate memory for the bytestream reply */
		if ((*p_uic_replymsg = PI_ALLOC(size)) == NULL) {
		    retval = UIC_S_ALLOC_ERR;
		}
		else {
		    if (retval == UIC_S_GROUP_ERR) {
			/* no valid group; reply consists of a header */
			wtbconv_uic_hdr_HTON((UNSIGNED8*)*p_uic_replymsg, (UIC_Header*)p_ReadGroupReply);
			retval = UNGS_OK;
		    }
		    else {
			/* Convert the structured message into a bytestream */
			wtbconv_11_01A_HTON ((UNSIGNED8*)*p_uic_replymsg, p_ReadGroupReply);
		    }
		    *p_uic_replymsg_size = size;
		}
		/* free the structured message allocated in ungs_GROUP_read() */
		PI_FREE (p_ReadGroupReply);
	    }
	    PI_FREE (p_ReadGroupCall);
	}
#else  /* no message conversion required */
	p_ReadGroupCall = (UIC_ReadGroup *)p_uic_callmsg;
	retval = ungs_GROUP_read(p_ReadGroupCall->group_number,
						    &p_ReadGroupReply, &size);
	if (retval != UIC_S_ALLOC_ERR ) {
	    /* prepare the uic reply header */
	    exchange_UIC_HEADER((UIC_Msg*)p_ReadGroupReply,(UIC_Msg*)p_ReadGroupCall);
	    *p_uic_replymsg = p_ReadGroupReply;
	    *p_uic_replymsg_size = size;
	}
#endif
	break;
    case UIC_FC_READ_GROUP_LIST:
#ifndef O_CONV
	/* The conversion of the call  message  */
	if ((p_ReadGroupListCall = (UIC_Msg*)PI_ALLOC(sizeof(UIC_Msg))) == NULL) {
	    retval = UIC_S_ALLOC_ERR;
	}
	else {
	    /* Convert the call message from a bytestream */
	    wtbconv_11_02__NTOH ((UNSIGNED8*)p_uic_callmsg, (UIC_Header*)p_ReadGroupListCall);
	    retval = ungs_GROUPLIST_read(&p_ReadGroupListReply, &size);
	    if (retval != UIC_S_ALLOC_ERR ) {
		/* prepare the uic reply header */
		exchange_UIC_HEADER((UIC_Msg*)p_ReadGroupListReply,(UIC_Msg*)p_ReadGroupListCall);
		/* Allocate memory for the bytestream reply */
		if ((*p_uic_replymsg = PI_ALLOC(size)) == NULL) {
		    retval = UIC_S_ALLOC_ERR;
		}
		else {
		    /* Convert the structured message into a bytestream */
		    wtbconv_11_02A_HTON ((UNSIGNED8*)*p_uic_replymsg, p_ReadGroupListReply);
		    *p_uic_replymsg_size = size;
		}
		/* free the structured message allocated in ungs_GROUPLIST_read() */
		PI_FREE (p_ReadGroupListReply);
	    }
	    PI_FREE (p_ReadGroupListCall);
	}
#else  /* no message conversion required */
	    p_ReadGroupListCall = p_uic_callmsg;
	retval = ungs_GROUPLIST_read(&p_ReadGroupListReply, &size);
	if (retval != UIC_S_ALLOC_ERR ) {
	    /* prepare the uic reply header */
	    exchange_UIC_HEADER((UIC_Msg*)p_ReadGroupListReply,(UIC_Msg*)p_ReadGroupListCall);
	    *p_uic_replymsg = p_ReadGroupListReply;
	    *p_uic_replymsg_size = size;
	}
#endif
	break;
    case UIC_FC_WRITE_GROUP:
#ifndef O_CONV
	/* The conversion of the call  message  */
	if ((p_WriteGroupCall = (UIC_WriteGroup*)PI_ALLOC(sizeof(UIC_WriteGroup) +
	    sizeof(UIC_Group_short) +
	    UIC_GROUP_ENTRIES_MAX * sizeof(UIC_GroupVeh_short))) == NULL) {
	    retval = UIC_S_ALLOC_ERR;
	}
	else {
	    /* Convert the call message from a bytestream */
	    wtbconv_11_03__NTOH ((UNSIGNED8*)p_uic_callmsg, p_WriteGroupCall);
	    retval = ungs_GROUP_write(&p_WriteGroupCall->group,
						    &p_msg, &size);
	    if (retval != UIC_S_ALLOC_ERR ) {
		/* prepare the uic reply header */
		exchange_UIC_HEADER(p_msg,(UIC_Msg*)p_WriteGroupCall);
		/* Allocate memory for the bytestream reply */
		if ((*p_uic_replymsg = PI_ALLOC(size)) == NULL) {
		    retval = UIC_S_ALLOC_ERR;
		}
		else {
		    /* Convert the structured message into a bytestream */
		    wtbconv_11_03A_HTON ((UNSIGNED8*)*p_uic_replymsg, (UIC_Header*)p_msg);
		    *p_uic_replymsg_size = size;
		}
		/* free the structured message allocated in ungs_GROUP_write() */
		PI_FREE (p_msg);
	    }
	    PI_FREE (p_WriteGroupCall);
	}
#else  /* no message conversion required */
	p_WriteGroupCall = (UIC_WriteGroup *)p_uic_callmsg;
	retval = ungs_GROUP_write(&p_WriteGroupCall->group, &p_msg, &size);
	if (retval != UIC_S_ALLOC_ERR ) {
	    /* prepare the uic reply header */
	    exchange_UIC_HEADER(p_msg,(UIC_Msg*)p_WriteGroupCall);
	    *p_uic_replymsg = p_msg;
	    *p_uic_replymsg_size = size;
	}
#endif
	break;
    case UIC_FC_WRITE_ALL_GROUPS:
#ifndef O_CONV
	/* The conversion of the call  message  */
	if ((p_WriteGroupList = (UIC_WriteGroupList*)PI_ALLOC(sizeof(UIC_WriteGroupList) +
	    UIC_GROUP_MAX * UIC_SIZEOF_UIC_GROUP_SHORT)) == NULL) {
	    retval = UIC_S_ALLOC_ERR;
	}
	else {
	    /* Convert the call message from a bytestream */
	    wtbconv_11_04__NTOH ((UNSIGNED8*)p_uic_callmsg, p_WriteGroupList);
	    retval = ungs_GROUPLIST_write(&p_WriteGroupList->groupli,
						    &p_msg, &size);
	    if (retval != UIC_S_ALLOC_ERR ) {
		/* prepare the uic reply header */
		exchange_UIC_HEADER(p_msg,(UIC_Msg*)p_WriteGroupList);
		/* Allocate memory for the bytestream reply */
		if ((*p_uic_replymsg = PI_ALLOC(size)) == NULL) {
		    retval = UIC_S_ALLOC_ERR;
		}
		else {
		    /* Convert the structured message into a bytestream */
		    wtbconv_11_04A_HTON ((UNSIGNED8*)*p_uic_replymsg, (UIC_Header*)p_msg);
		    *p_uic_replymsg_size = size;
		}
		/* free the structured message allocated in ungs_GROUPLIST_write() */
		PI_FREE (p_msg);
	    }
	    PI_FREE (p_WriteGroupList);
	}
#else  /* no message conversion required */
	p_WriteGroupList = (UIC_WriteGroupList *)p_uic_callmsg;
	retval = ungs_GROUPLIST_write(&p_WriteGroupList->groupli, &p_msg, &size);
	if (retval != UIC_S_ALLOC_ERR ) {
	    /* prepare the uic reply header */
	    exchange_UIC_HEADER(p_msg,(UIC_Msg*)p_WriteGroupList);
	    *p_uic_replymsg = p_msg;
	    *p_uic_replymsg_size = size;
	}
#endif
	break;
    case UIC_FC_DELETE_GROUP:
#ifndef O_CONV
	/* The conversion of the call  message  */
	if ((p_DeleteGroup = (UIC_DeleteGroup*)PI_ALLOC(sizeof(UIC_DeleteGroup))) == NULL) {
	    retval = UIC_S_ALLOC_ERR;
	}
	else {
	    /* Convert the call message from a bytestream */
	    wtbconv_11_05__NTOH ((UNSIGNED8*)p_uic_callmsg, p_DeleteGroup);
	    retval = ungs_GROUP_delete(p_DeleteGroup->group_number,
						    &p_DeleteGroupReply, &size);
	    if (retval != UIC_S_ALLOC_ERR ) {
		/* prepare the uic reply header */
		exchange_UIC_HEADER((UIC_Msg*)p_DeleteGroupReply,(UIC_Msg*)p_DeleteGroup);
		/* Allocate memory for the bytestream reply */
		if ((*p_uic_replymsg = PI_ALLOC(size)) == NULL) {
		    retval = UIC_S_ALLOC_ERR;
		}
		else {
		    /* Convert the structured message into a bytestream */
		    wtbconv_11_05A_HTON ((UNSIGNED8*)*p_uic_replymsg, (UIC_Header*) p_DeleteGroupReply);
		    *p_uic_replymsg_size = size;
		}
		/* free the structured message allocated in ungs_GROUP_delete() */
		PI_FREE (p_DeleteGroupReply);
	    }
	    PI_FREE (p_DeleteGroup);
	}
#else   /* no message conversion required */
	p_DeleteGroup = (UIC_DeleteGroup *)p_uic_callmsg;
	retval = ungs_GROUP_delete(p_DeleteGroup->group_number,
						    &p_DeleteGroupReply, &size);
	if (retval != UIC_S_ALLOC_ERR ) {
	    /* prepare the uic reply header */
	    exchange_UIC_HEADER((UIC_Msg*)p_DeleteGroupReply,(UIC_Msg*)p_DeleteGroup);
	    *p_uic_replymsg = p_DeleteGroupReply;
	    *p_uic_replymsg_size = size;
	}
#endif
	break;
    case UIC_FC_DELETE_ALL_GROUPS:
#ifndef O_CONV
	/* The conversion of the call  message  */
	if ((p_msg = (UIC_Msg*)PI_ALLOC(sizeof(UIC_Msg))) == NULL) {
	    retval = UIC_S_ALLOC_ERR;
	}
	else {
	    retval = ungs_GROUPALL_delete(&p_DeleteGroupReply, &size);
	    if (retval != UIC_S_ALLOC_ERR) {
		/* Convert the call message from a bytestream */
		wtbconv_11_06__NTOH ((UNSIGNED8*)p_uic_callmsg, &p_msg->hdr);
		/* prepare the uic reply header */
		exchange_UIC_HEADER(p_DeleteGroupReply,p_msg);
		/* Allocate memory for the bytestream reply */
		if ((*p_uic_replymsg = PI_ALLOC(size)) == NULL) {
		    retval = UIC_S_ALLOC_ERR;
		}
		else {
		    /* Convert the structured message into a bytestream */
		    wtbconv_11_06A_HTON ((UNSIGNED8*)*p_uic_replymsg, (UIC_Header*)p_DeleteGroupReply);
		    *p_uic_replymsg_size = size;
		}
		/* free the structured message allocated in ungs_GROUPALL_delete() */
		PI_FREE (p_DeleteGroupReply);
	    }
	    PI_FREE (p_msg);
	}
#else  /* no message conversion required */
	p_msg = (UIC_Msg*)p_uic_callmsg;
	retval = ungs_GROUPALL_delete(&p_DeleteGroupReply, &size);
	if (retval != UIC_S_ALLOC_ERR) {
	    exchange_UIC_HEADER(p_DeleteGroupReply,p_msg);
	    *p_uic_replymsg = p_DeleteGroupReply;
	    *p_uic_replymsg_size = size;
	}
#endif
	break;

	/*** invalid command code *******************/
    default:
	retval = (UNSIGNED8) UIC_S_CMD_INVALID;
	*p_uic_replymsg = NULL;
	*p_uic_replymsg_size = 0;
	break;
    }

	return retval;
}

