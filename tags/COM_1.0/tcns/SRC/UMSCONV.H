/*************************************************************************
**        Copyright (C) Siemens AG 1997 All Rights Reserved. Confidential
**========================================================================
**
**    PROJECT::    TCN Train Communication Network
                   JDP Joint Development Project

**    MODULE::     UIC Mapping Server

**    WORKFILE::   $Workfile:   UMSCONV.H  $
**************************************************************************
**    TASK::       Converion of all UIC Mapping Server Datagrams

**************************************************************************
**    AUTHOR::     Just ()
**    CREATED::    12.02.98
**========================================================================
**    HISTORY::    AUTHOR::   $Author:   HILBRICH_KL  $
                   REVISION:: $Revision:   1.5  $
                   MODTIME::  $Date:   21 Sep 1998 12:33:40  $
      $Log:   O:\PRJ\JDP\FUE\SW\PVCS\ARC\TCN_FW\UIC\INC\UMSCONV.Hv_  $
 * 
 *    Rev 1.5   21 Sep 1998 12:33:40   HILBRICH_KL
 *  
 *
 *    Rev 1.4   11 May 1998 16:22:28   REINWALD_LU
 * O_GATEWAY for exchange_UIC_header
 *
 *    Rev 1.3   31 Mar 1998 15:27:26   HILBRICH_KL
 * telegram names adapted to UIC556
 *
 *    Rev 1.2   18 Mar 1998 16:46:48   REINWALD_LU
 * some functions removed
 *
 *    Rev 1.1   09 Mar 1998 15:21:56   HILBRICH_KL
 * delivered to ADtranz on 09/03/98
**************************************************************************/

#ifndef UMSCONV_H_
#define UMSCONV_H_

/*************************************************************************/
/* Functions for conversion of bytestream data:                          */
/* This file contains functions to convert structures from a little      */
/* endian system into a big endian bytestream UIC telegram defined by    */
/* UIC 556 for the WTB and vice versa.                                   */
/* "host" (H) is the little endian system, where "net" (N) is the big    */
/* endian bytestream transportation medium like the WTB.                 */
/* A function name ending with "NTOH" means that this function does a    */
/* conversion from NET TO HOST i.e. from a big endian bytestream into a  */
/* certain structure on a little endian system. "HTON" does the opposite.*/
/* The UIC 556 telegrams currently supported are                         */
/* - 0.01  read NADI, call                                               */
/* - 0.01A read NADI, reply                                              */
/* - 1.2   convert UIC adress into TCN adress, call                      */
/* - 1.2A  convert UIC adress into TCN adress, reply                     */
/* -11.01  read group, call                                              */
/* -11.01A read group, reply                                             */
/* -11.02  list all groups, call                                         */
/* -11.02A list all groups, reply                                        */
/* -11.03  write group, call                                             */
/* -11.03A write group, reply                                            */
/* -11.04  write groups, call                                            */
/* -11.04A write groups, reply                                           */
/* -11.05  delete group, call                                            */
/* -11.05A delete group, reply                                           */
/* -11.06  delete all groups, call                                       */
/* -11.06A delete all groups, reply                                      */
/*                                                                       */
/* Assumption is that the caller provides the destination buffer         */
/*************************************************************************/


/*
 *  Common conversion routines for simple types
 */



/*******************************************************/

#define HTON8(x) (x)

/*******************************************************/

#define NTOH8(x) (x)

/*******************************************************/

int HTON16( UNSIGNED8* dest, UNSIGNED16 source );

/*******************************************************/

UNSIGNED16 NTOH16( UNSIGNED8* source );

/*******************************************************/

int HTON32( UNSIGNED8* dest, UNSIGNED32 source );

/*******************************************************/

UNSIGNED32 NTOH32( UNSIGNED8* source );


/*************************************************************************/
/* function converts the header of an UIC telegram into a host data      */
/* structure from a big endian bytestream                                */
/*************************************************************************/

UNSIGNED8 *wtbconv_uic_hdr_NTOH (UNSIGNED8  *p_byte_in,
                                 UIC_Header *p_hdr);

void NTOH_header(UNSIGNED8 *p_byte_in, UIC_Header *p_hdr);

/*************************************************************************/
/* function converts the header of an UIC telegram from host data        */
/* structure into a big endian bytestream                                */
/*************************************************************************/

UNSIGNED8 *wtbconv_uic_hdr_HTON (UNSIGNED8  *p_byte_out,
                                 UIC_Header *p_hdr);

void HTON_header(UIC_Header *p_hdr, UNSIGNED8 *p_byte_out);


/*************************************************************************
 * 0.01: read NADI, call  (host->net)
 *                  call  (net->host)
 *                  reply (host->net)
 *                  reply (net->host)
 *************************************************************************/
void  wtbconv_00_01__HTON (UNSIGNED8     *p_bytestream_dest,
                           UIC_Header    *p_header_src);
void  wtbconv_00_01__NTOH (UNSIGNED8     *p_bytestream_src,
                           UIC_Header    *p_header_dest);
void  wtbconv_00_01A_HTON (UNSIGNED8     *p_bytestream_dest,
                           UIC_ReadNadi  *p_Nadi_src);
void  wtbconv_00_01A_NTOH (UNSIGNED8     *p_bytestream_src,
                           UIC_ReadNadi  *p_Nadi_dest);


/*************************************************************************
 * 0.22: UWTM State Request, call  (host->net)
 *                           call  (net->host)
 *                           reply (host->net)
 *                           reply (net->host)
 *************************************************************************/
#define HTON_uic_fc_read_uwtm_state(s,b) HTON_header(s,b)
#define NTOH_uic_fc_read_uwtm_state(b,s) NTOH_header(b,s)
void  HTON_uic_fr_read_uwtm_state (UIC_TR_READ_UWTM_STATE  *reply_t_msg,
                                   UNSIGNED8               *replymsg);
void  NTOH_uic_fr_read_uwtm_state (UNSIGNED8               *replymsg,
                                   UIC_TR_READ_UWTM_STATE  *reply_t_msg);


/*************************************************************************
 * 0.03: Change Operational Mode, call  (host->net)
 *                                call  (net->host)
 *                                reply (host->net)
 *                                reply (net->host)
 *************************************************************************/
#define HTON_uic_fc_change_omode(s,b) HTON_header(s,b)
#define NTOH_uic_fc_change_omode(b,s) NTOH_header(b,s)
#define HTON_uic_fr_change_omode(s,b) HTON_header(s,b)
#define NTOH_uic_fr_change_omode(b,s) NTOH_header(b,s)


/*************************************************************************
 * 0.04: UWTM Topography Request, call  (host->net)
 *                                call  (net->host)
 *                                reply (host->net)
 *                                reply (net->host)
 *************************************************************************/
#define HTON_uic_fc_read_uwtm_topography(s,b) HTON_header(s,b)
#define NTOH_uic_fc_read_uwtm_topography(b,s) NTOH_header(b,s)
void  HTON_uic_fr_read_uwtm_topography (UIC_TR_READ_UWTM_TOPOGRAPHY  *reply_t_msg,
                                        UNSIGNED8                    *replymsg);
void  NTOH_uic_fr_read_uwtm_topography (UNSIGNED8                    *replymsg,
                                        UIC_TR_READ_UWTM_TOPOGRAPHY  *reply_t_msg);


/*************************************************************************
 * 0.f0: delete all groups, call  (host->net)
 *                          call  (net->host)
 *                          reply (host->net)
 *                          reply (net->host)
 *************************************************************************/
void HTON_uic_fc_leading (UIC_TC_LEADING  *call_t_msg,
                          UNSIGNED8       *callmsg);
void NTOH_uic_fc_leading (UNSIGNED8       *callmsg,
                          UIC_TC_LEADING  *call_t_msg);
#define HTON_uic_fr_leading(s,b) HTON_header(s,b)
#define NTOH_uic_fr_leading(b,s) NTOH_header(b,s)


/*************************************************************************
 * 0.f1: convert UIC adress into TCN adress, call  (host->net)
 *                                           call  (net->host)
 *                                           reply (host->net)
 *                                           reply (net->host)
 *************************************************************************/
void  wtbconv_32_01_02__HTON (UNSIGNED8            *p_bytestream_dest,
                              UIC_CvtUicToTcn      *p_uic_add_src);
void  wtbconv_32_01_02__NTOH (UNSIGNED8            *p_bytestream_src,
                              UIC_CvtUicToTcn      *p_uic_add_dest);
void  wtbconv_32_01_02A_HTON (UNSIGNED8            *p_bytestream_dest,
                              UIC_CvtUicToTcnList  *p_tcn_list_src);
void  wtbconv_32_01_02A_NTOH (UNSIGNED8            *p_bytestream_src,
                              UIC_CvtUicToTcnList  *p_tcn_list_dest);


/*************************************************************************
 * 11.01: read group, call  (host->net)
 *                    call  (net->host)
 *                    reply (host->net)
 *                    reply (net->host)
 *************************************************************************/
void  wtbconv_11_01__HTON (UNSIGNED8           *p_bytestream_dest,
                           UIC_ReadGroup       *p_msg_src);
void  wtbconv_11_01__NTOH (UNSIGNED8           *p_bytestream_src,
                           UIC_ReadGroup       *p_msg_dest);
void  wtbconv_11_01A_HTON (UNSIGNED8           *p_bytestream_dest,
                           UIC_ReadGroupReply  *p_msg_src);
void  wtbconv_11_01A_NTOH (UNSIGNED8           *p_bytestream_src,
                           UIC_ReadGroupReply  *p_msg_dest);


/*************************************************************************
 * 11.02: list all groups, call  (host->net)
 *                         call  (net->host)
 *                         reply (host->net)
 *                         reply (net->host)
 *************************************************************************/
void  wtbconv_11_02__HTON (UNSIGNED8           *p_bytestream_dest,
                           UIC_Header          *p_header_src);
void  wtbconv_11_02__NTOH (UNSIGNED8           *p_bytestream_src,
                           UIC_Header          *p_header_dest);
void  wtbconv_11_02A_HTON (UNSIGNED8           *p_bytestream_dest,
                           UIC_GroupListReply  *p_grouplist_src);
void  wtbconv_11_02A_NTOH (UNSIGNED8           *p_bytestream_src,
                           UIC_GroupListReply  *p_grouplist_dest);


/*************************************************************************
 * 11.03: write group, call  (host->net)
 *                     call  (net->host)
 *                     reply (host->net)
 *                     reply (net->host)
 *************************************************************************/
void  wtbconv_11_03__HTON (UNSIGNED8       *p_bytestream_dest,
                           UIC_WriteGroup  *p_wgroup_src);
void  wtbconv_11_03__NTOH (UNSIGNED8       *p_bytestream_src,
                           UIC_WriteGroup  *p_wgroup_dest);
void  wtbconv_11_03A_HTON (UNSIGNED8       *p_bytestream_dest,
                           UIC_Header      *p_header_src);
void  wtbconv_11_03A_NTOH (UNSIGNED8       *p_bytestream_src,
                           UIC_Header      *p_header_dest);


/*************************************************************************
 * 11.04: write all groups, call  (host->net)
 *                          call  (net->host)
 *                          reply (host->net)
 *                          reply (net->host)
 *************************************************************************/
void  wtbconv_11_04__HTON (UNSIGNED8           *p_bytestream_dest,
                           UIC_WriteGroupList  *p_grouplist_src);
void  wtbconv_11_04__NTOH (UNSIGNED8           *p_bytestream_src,
                           UIC_WriteGroupList  *p_grouplist_dest);
void  wtbconv_11_04A_HTON (UNSIGNED8           *p_bytestream_dest,
                           UIC_Header          *p_header_src);
void  wtbconv_11_04A_NTOH (UNSIGNED8           *p_bytestream_src,
                           UIC_Header          *p_header_dest);


/*************************************************************************
 * 11.05: delete group, call  (host->net)
 *                      call  (net->host)
 *                      reply (host->net)
 *                      reply (net->host)
 *************************************************************************/
void  wtbconv_11_05__HTON (UNSIGNED8        *p_bytestream_dest,
                           UIC_DeleteGroup  *p_msg_src);
void  wtbconv_11_05__NTOH (UNSIGNED8        *p_bytestream_src,
                           UIC_DeleteGroup  *p_msg_dest);
void  wtbconv_11_05A_HTON (UNSIGNED8        *p_bytestream_dest,
                           UIC_Header       *p_header_src);
void  wtbconv_11_05A_NTOH (UNSIGNED8        *p_bytestream_src,
                           UIC_Header       *p_header_dest);


/*************************************************************************
 * 11.06: delete all groups, call  (host->net)
 *                           call  (net->host)
 *                           reply (host->net)
 *                           reply (net->host)
 *************************************************************************/
void  wtbconv_11_06__HTON (UNSIGNED8   *p_bytestream_dest,
                           UIC_Header  *p_header_src);
void  wtbconv_11_06__NTOH (UNSIGNED8   *p_bytestream_src,
                           UIC_Header  *p_header_dest);
void  wtbconv_11_06A_HTON (UNSIGNED8   *p_bytestream_dest,
                           UIC_Header  *p_header_src);
void  wtbconv_11_06A_NTOH (UNSIGNED8   *p_bytestream_src,
                           UIC_Header  *p_header_dest);


/*************************************************************************
 * 15.01: erase configuration, call  (host->net)
 *                             call  (net->host)
 *                             reply (host->net)
 *                             reply (net->host)
 *************************************************************************/
#define NTOH_uic_fc_erase_configuration(b,s) NTOH_header(b,s)
#define HTON_uic_fc_erase_configuration(s,b) HTON_header(s,b)
#define HTON_uic_fr_erase_configuration(s,b) HTON_header(s,b)
#define NTOH_uic_fr_erase_configuration(b,s) NTOH_header(b,s)


/*************************************************************************
 * 15.02: UTBC Write Correction, call  (host->net)
 *                               call  (net->host)
 *                               reply (host->net)
 *                               reply (net->host)
 *************************************************************************/
void  wtbconv_15_02_HTON  (UNSIGNED8            *p_bytestream_dest,
                           UIC_WriteCorrection  *pCorrection);
void  wtbconv_15_02_NTOH  (UNSIGNED8            *p_bytestream_src,
                           UIC_WriteCorrection  *pCorrection);
void  wtbconv_15_02A_HTON (UNSIGNED8            *p_bytestream_dest,
                           UIC_Msg              *p_msg);
void  wtbconv_15_02A_NTOH (UNSIGNED8            *p_bytestream_src,
                           UIC_Msg              *p_msg);


/*************************************************************************
 * 15.03: Write Vehicle Reservation Numbers, call  (host->net)
 *                                           call  (net->host)
 *                                           reply (host->net)
 *                                           reply (net->host)
 *************************************************************************/
void  wtbconv_15_03_HTON  (UNSIGNED8        *p_bytestream_dest,
                           UIC_WriteResNum  *p_veh_res_num);
void  wtbconv_15_03_NTOH  (UNSIGNED8        *p_bytestream_src,
                           UIC_WriteResNum  *p_veh_res_num);
void  wtbconv_15_03A_HTON (UNSIGNED8        *p_bytestream_dest,
                           UIC_Msg          *p_msg);
void  wtbconv_15_03A_NTOH (UNSIGNED8        *p_bytestream_src,
                           UIC_Msg          *p_msg);


/*************************************************************************
 * 15.04: Sleep Request, call  (host->net)
 *                       call  (net->host)
 *                       reply (host->net)
 *                       reply (net->host)
 *************************************************************************/
#define HTON_uic_fc_sleep(s,b) HTON_header(s,b)
#define NTOH_uic_fc_sleep(b,s) NTOH_header(b,s)
#define HTON_uic_fr_sleep(s,b) HTON_header(s,b)
#define NTOH_uic_fr_sleep(b,s) NTOH_header(b,s)


/*************************************************************************
 * 15.05: Inauguration Control, call  (host->net)
 *                              call  (net->host)
 *                              reply (host->net)
 *                              reply (net->host)
 *************************************************************************/
#define NTOH_uic_fc_inauguration_control(b,s) NTOH_header(b,s)
#define HTON_uic_fr_inauguration_control(s,b) HTON_header(s,b)
#define HTON_uic_fc_inauguration_control(s,b) HTON_header(s,b)
#define NTOH_uic_fr_inauguration_control(b,s) NTOH_header(b,s)


/*************************************************************************
 * 15.06: Inauguration Enforce, call  (host->net)
 *                              call  (net->host)
 *                              reply (host->net)
 *                              reply (net->host)
 *************************************************************************/
#define HTON_uic_fc_inauguration_enforce(s,b) HTON_header(s,b)
#define NTOH_uic_fc_inauguration_enforce(b,s) NTOH_header(b,s)
#define HTON_uic_fr_inauguration_enforce(s,b) HTON_header(s,b)
#define NTOH_uic_fr_inauguration_enforce(b,s) NTOH_header(b,s)


/*************************************************************************
 * 15.07: UIMCS Multicast create, call  (host->net)
 *                                call  (net->host)
 *                                reply (host->net)
 *                                reply (net->host)
 *************************************************************************/
void  wtbconv_15_07__HTON (UNSIGNED8         *p_bytestream_dest,
                           UIC_MCreateRequ   *p_MCreateRequ_src);
void  wtbconv_15_07__NTOH (UNSIGNED8         *p_bytestream_src,
                           UNSIGNED32         bytestream_src_len,
                           UIC_MCreateRequ   *p_MCreateRequ_dest);
void  wtbconv_15_07A_HTON (UNSIGNED8         *p_bytestream_dest,
                           UIC_MCreateReply  *p_MCreateReply_src);
void  wtbconv_15_07A_NTOH (UNSIGNED8         *p_bytestream_src,
                           UIC_MCreateReply  *p_MCreateReply_dest);

#ifdef O_GATEWAY
/*
 * utility
 */

void exchange_UIC_HEADER(UIC_Msg* dest, UIC_Msg* src);
#endif

#endif  /* UMSCONV_H_ */
