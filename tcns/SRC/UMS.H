/*************************************************************************
**        Copyright (C) Siemens AG 1998 All Rights Reserved. Confidential
**========================================================================
**
**    PROJECT::    TCN Train Communication Network
                   JDP Joint Development Project

**    MODULE::     UIC Mapping Server

**    WORKFILE::   $Workfile:   UMS.H  $
**************************************************************************
**    TASK::       Definition of UIC Mapping Server types and consts

**************************************************************************
**    AUTHOR::     Hilbrich ()
**    CREATED::    20.10.97
**========================================================================
**    HISTORY::    AUTHOR::   $Author: schiavor $
                   REVISION:: $Revision: 1.1.2.1 $
                   MODTIME::  $Date: 2006/12/01 10:41:46 $
      $Log: UMS.H,v $
      Revision 1.1.2.1  2006/12/01 10:41:46  schiavor
      01/12/2006                         Napoli                            SchiavoR
      Primo inserimento per utilizzo libreria nel GTW

 *
 *    Rev 1.28   16 Dec 1998 12:56:40   REINWALD_LU
 * new error code for too short uic header
 *
 *    Rev 1.27   10 Dec 1998 13:47:04   HEINDEL_AR
 * corrected UIC_ReadNADI define
 * comments for the configuration table structure
 *
 *    Rev 1.26   15 Oct 1998 13:59:44   REINWALD_LU
 * typos corrected: toporeply and write_all_groups
 *
 *    Rev 1.25   01 Oct 1998 09:02:14   HILBRICH_KL
 * additional error code added to returncodes of UTBC
 *
 *    Rev 1.24   21 Sep 1998 13:11:56   HILBRICH_KL
 * INVALID_UIC_ADDRESS defined
 *
 *    Rev 1.23   21 Sep 1998 12:33:26   HILBRICH_KL
 * wrong reservation number telegram format
 *
 *    Rev 1.21   05 Aug 1998 14:03:42   Just_Pete
 * Längenberechnung für UIC_GroupListReply
 * korrigiert
 *
 *    Rev 1.20   04 Aug 1998 09:15:56   REINWALD_LU
 * defines for uwtm reply telegrams added
 *
 *    Rev 1.19   04 Aug 1998 08:20:52   Just_Pete
 * Defines für die Länge der Reply-Messages
 * als Bytestream eingeführt.
 *
 *    Rev 1.18   17 Jul 1998 14:10:04   Just_Pete
 * Neuer Service: UIC_MCP_COLLECT_REPLY,
 * MC_DEST_MAX von uimcs.c hierher verlagert,
 * neue Struktur UIC_MCollectReply
 *
 *    Rev 1.17   09 Jul 1998 12:21:54   Just_Pete
 * Fehler code UIC_S_ERR_MC_SEND fuer
 * synchronen Aufruf an uimcs_request() neu.
 *
 *    Rev 1.16   24 Jun 1998 07:48:28   REINWALD_LU
 * enums replaced by defines
 *
 *    Rev 1.15   04 Jun 1998 14:14:00   Just_Pete
 * Sammeladressen vervollständigt
 *
 *    Rev 1.14   29 May 1998 10:38:52   Just_Pete
 * Bitmasks and offsets für einige Fahrzeugeigenschaften
 * korrigiert.
 *
 *    Rev 1.13   26 May 1998 13:19:42   REINWALD_LU
 * removed unnecessary defined
 *
 *    Rev 1.12   25 May 1998 09:32:08   Just_Pete
 * new error code for uagt.c in case of a
 * wrong owner identification:
 * UADT_OWNER_INVALID
 *
 *    Rev 1.11   15 May 1998 14:08:00   Just_Pete
 * UIC_MCP_SYNC defined
 *
 *    Rev 1.10   12 May 1998 08:01:52   REINWALD_LU
 * added owner offset
 *
 *    Rev 1.9   06 May 1998 16:11:52   Just_Pete
 * Adresse UIC_V_TCN_BUS_MASTER neu
 *
 *    Rev 1.8   31 Mar 1998 15:24:58   HILBRICH_KL
 * telegram names adapted to UIC556
 *
 *    Rev 1.5   09 Mar 1998 16:32:48   HILBRICH_KL
 * delivered to ADtranz 09/03/98  ( updated )
 *
 *    Rev 1.4   09 Mar 1998 15:21:54   HILBRICH_KL
 * delivered to ADtranz on 09/03/98
**************************************************************************/

#ifndef UMS_H_
#define UMS_H_

#ifndef TCNTYPES_H_
#error include "tcntypes.h" first
#endif

#ifndef BMI_H_
#error include "bmi.h" first
#endif

#define INVALID_TCN_ADDRESS     0x7F
#define INVALID_UIC_ADDRESS     0x00

/* length definitions for tfr-elements */
#define TFR_LEN_TS_INFO         22
#define TFR_LEN_UIC_ID          5
#define TFR_LEN_VEH_INFO        7
#define TFR_LEN_NA_VEH          8
#define TFR_LEN_VEH_RES_NUM     2
#define TFR_NUM_VEH_DESCR       6
#define TFR_LEN_VEH_DESCR       (TFR_LEN_UIC_ID + \
                                 TFR_LEN_VEH_RES_NUM + \
                                 TFR_LEN_VEH_INFO)


/* UIC-ID definition; the representation of the UIC-ID: */
/* uic_id[0] is MSO, uic_id[4] is LSO; for example,     */
/* the ID 123.456 will be coded as  00 00 01 40 E2      */
typedef struct
{
    UNSIGNED8   uic_id[ TFR_LEN_UIC_ID ];    /* binary coded UIC-ID */

} TYPE_UIC_ID;

/* the topo-frame is a bytestream read from the
   ID-TS ( 'TYPE_InaugData' ) containing the
   following elements

   a) header:


       datatype         element
    |--------------------------------------|
    |  UNSIGNED8        uic_Ind            |
    |--------------------------------------|
    |  UNSIGNED8        uic_Ver            |
    |--------------------------------------|
    |  UNSIGNED8        src_dev            |
    |--------------------------------------|
    |  UNSIGNED8        size               |
    |--------------------------------------|


  bit 0-3 of uic_Ind: set to 1, if vehicle
  is uic-conform

  bit 0-6 of src_dev: tcn-address
  bit 7 of src_dev: orientation of the node
  relativ to tcn-master:
        set to 0, if invers orientation
        set to 1, if same orientation


  b) up to 124 bytes of netto-data

    |--------------------------------------|
    |  UNSIGNED8        veh_management     |
    |--------------------------------------|
    |  UNSIGNED8        veh_owner          |
    |--------------------------------------|
    |  UNSIGNED8        application_id     |
    |--------------------------------------|
    |  UNSIGNED8        application_ver    |
    |--------------------------------------|
    |  UNSIGNED8        num_ctrl_veh       |
    |--------------------------------------|
    |  BITSET8          state_ctrl_info    |
    |--------------------------------------|
    |  UNSIGNED8        RangeNo            |
    |--------------------------------------|
    |  UNSIGNED8        num_veh            |
    |--------------------------------------|
    |  BITSET8          not_av_veh[8]      |
    |--------------------------------------|
    |  UNSIGNED8        uic_subversion     |
    |--------------------------------------|
    |  UNSIGNED8        trainset_info[22]  |
    |--------------------------------------|
    |  UNSIGNED8        reserved           |
    |--------------------------------------|

  c) following is the description for max.6
    controlled vehicles  ( array[ 6 ] )

    |--------------------------------------|
    |  TYPE_UIC_ID      uic_id             |
    |--------------------------------------|
    |  BITSET8          veh_info[ 7 ]      |
    |--------------------------------------|
    |  UNSIGNED16       veh_res_num        |
    |--------------------------------------|


  Byte 4.1 of veh_info: vehicle contains gateway

*/

/* bitmasks for 'state_ctrl_info' */
#define UI_DEL_CC             0x01
#define UI_CONF_UNKNOWN       0x02
#define UI_UIC_ADDR_SET       0x04
#define UI_CONF_SAVED         0x08
#define UI_VEH_LEADING        0x10
#define UI_DEMAND_LDSHP       0x20
#define UI_DIR_LD_DEMAND      0x40
#define UI_REDUNDANCY_BIT     0x80


/* meaning of 'state_ctrl_info':
   Bit 0: request to delete configuration
   Bit 1: configuration unknown
   Bit 2: correction received and UIC address is set
   Bit 3: configuration is saved
   Bit 4: vehicle is leading
   Bit 5: vehicle requests leadership
   Bit 6: trainsets -> request leadership from drivers cab
          UWTM_DIR1 = direction 1
          UWTM_DIR2 = direction 2
   Bit 7: redundancy bit
*/

/* offset definitions within netto-data */
#define TFR_OFFSET_TO_VEH_MAN          0
#define TFR_OFFSET_TO_VEH_OWNER        1
#define TFR_OFFSET_TO_APPL_ID          2
#define TFR_OFFSET_TO_APPL_VER         3
#define TFR_OFFSET_TO_NUM_CV           4
#define TFR_OFFSET_TO_ST_CTRL_INFO     5
#define TFR_OFFSET_TO_RNO              6
#define TFR_OFFSET_TO_NUM_VEH          7
#define TFR_OFFSET_TO_NA_VEH           8
#define TFR_OFFSET_TO_UIC_SUBVER      (TFR_LEN_NA_VEH + 8)
#define TFR_OFFSET_TO_TS_INFO         (TFR_OFFSET_TO_UIC_SUBVER + 1)
#define TFR_OFFSET_TO_VEH_DESCR       (TFR_OFFSET_TO_TS_INFO + TFR_LEN_TS_INFO + 1)

/* offsets within one veh-descr. */
#define TFR_OFFSET_TO_UIC_ID           0
#define TFR_OFFSET_TO_VEH_INFO         TFR_LEN_UIC_ID
#define TFR_OFFSET_TO_VEH_RES_NUM     (TFR_LEN_UIC_ID + TFR_LEN_VEH_INFO)

/* access macros to tfr data-elements */
#define TFR_VEH_MAN( inaug_data )           inaug_data.Data[ TFR_OFFSET_TO_VEH_MAN ]
#define TFR_VEH_OWNER( inaug_data )         inaug_data.Data[ TFR_OFFSET_TO_VEH_OWNER ]
#define TFR_APPL_ID( inaug_data )           inaug_data.Data[ TFR_OFFSET_TO_APPL_ID ]
#define TFR_APPL_VER( inaug_data )          inaug_data.Data[ TFR_OFFSET_TO_APPL_VER ]
#define TFR_NUM_CV( inaug_data )            inaug_data.Data[ TFR_OFFSET_TO_NUM_CV ]
#define TFR_ST_CTRL_INFO( inaug_data )      inaug_data.Data[ TFR_OFFSET_TO_ST_CTRL_INFO ]
#define TFR_RNO( inaug_data )               inaug_data.Data[ TFR_OFFSET_TO_RNO ]
#define TFR_NUM_VEH( inaug_data )           inaug_data.Data[ TFR_OFFSET_TO_NUM_VEH ]
#define TFR_UIC_SUBVER( inaug_data )        inaug_data.Data[ TFR_OFFSET_TO_UIC_SUBVER ]
#define TFR_P_NA_VEH( inaug_data )          &inaug_data.Data[ TFR_OFFSET_TO_NA_VEH ]
#define TFR_P_TS_INFO( inaug_data )         &inaug_data.Data[ TFR_OFFSET_TO_TS_INFO ]
#define TFR_P_VEH_DESCR( inaug_data, i )    &inaug_data.Data[ TFR_OFFSET_TO_VEH_DESCR + \
                                                              i*TFR_LEN_VEH_DESCR ]
#define TFR_P_VEH_INFO( inaug_data, i )     &inaug_data.Data[ TFR_OFFSET_TO_VEH_DESCR + \
                                                              i*TFR_LEN_VEH_DESCR + \
                                                              TFR_OFFSET_TO_VEH_INFO ]

/* UIC OWNER field */
#define UIC_O_UIC                      0
#define UIC_O_DB                      80
#define UIC_O_OEBB                    81

/* UIC VEHICLE address field */
#define UIC_V_THIS                     0
#define UIC_V_LEADING                 64
#define UIC_V_LED_TRACTION_UNITS      65
#define UIC_V_ALL_VEHICLES            66
#define UIC_V_ALL_VEH_PASS_TRANSPORT  67
#define UIC_V_LAST                    68
#define UIC_V_ALL_MOTOR_VEHICLES      70
#define UIC_V_ALL_LOCOMOTIVES         72
#define UIC_V_ALL_POWER_CARS          74
#define UIC_V_ALL_COACHES             80
#define UIC_V_ALL_WITH_SEATS          81
#define UIC_V_WITH_SEATS_1ST_CLASS    82
#define UIC_V_WITH_SEATS_2ND_CLASS    83
#define UIC_V_LAST_COACH              84
#define UIC_V_ALL_DINING_CARS         91
#define UIC_V_ALL_RECUMBENCY_CARS     93
#define UIC_V_ALL_SLEEPING_CARS       92
#define UIC_V_ALL_BUGGAGE_CARS        94
#define UIC_V_TCN_BUS_MASTER         200

/* UIC code field */

/* UTBC specific codes */
#define UIC_FC_DEL_CONFIG               0xF001
#define UIC_FR_DEL_CONFIG               0xFA01
#define UIC_FC_WRITE_CORRECTION         0xF002
#define UIC_FR_WRITE_CORRECTION         0xFA02
#define UIC_FC_WRITE_VEH_RESNUM         0xF003
#define UIC_FR_WRITE_VEH_RESNUM         0xFA03

/* NADI specific codes */
#define UIC_FC_READ_NADI                0x0001
#define UIC_FR_READ_NADI                0x0A01
#define UIC_FC_CVT_UIC_TO_TCN           0x00F1
#define UIC_FR_CVT_UIC_TO_TCN           0x0AF1

/* GROUP specific codes */
#define UIC_FC_READ_GROUP               0xB001
#define UIC_FR_READ_GROUP               0xBA01
#define UIC_FC_READ_GROUP_LIST          0xB002
#define UIC_FR_READ_GROUP_LIST          0xBA02
#define UIC_FC_WRITE_GROUP              0xB003
#define UIC_FR_WRITE_GROUP              0xBA03
#define UIC_FC_WRITE_ALL_GROUPS         0xB004
#define UIC_FR_WRITE_ALL_GROUPS         0xBA04
#define UIC_FC_DELETE_GROUP             0xB005
#define UIC_FR_DELETE_GROUP             0xBA05
#define UIC_FC_DELETE_ALL_GROUPS        0xB006
#define UIC_FR_DELETE_ALL_GROUPS        0xBA06

/* WTB control specific codes */
#define UIC_FC_SLEEP                    0xF004
#define UIC_FR_SLEEP                    0xFA04
#define UIC_FC_INAUGURATION_CONTROL     0xF005
#define UIC_FR_INAUGURATION_CONTROL     0xFA05
#define UIC_FC_READ_UWTM_STATE          0x0022
#define UIC_FR_READ_UWTM_STATE          0x0A22
#define UIC_FC_INAUGURATION_ENFORCE     0xF006
#define UIC_FR_INAUGURATION_ENFORCE     0xFA06
#define UIC_FC_CHANGE_OMODE             0x0003
#define UIC_FR_CHANGE_OMODE             0x0A03
#define UIC_FC_READ_UWTM_TOPOGRAPHY     0x0004
#define UIC_FR_READ_UWTM_TOPOGRAPHY     0x0A04
#define UIC_FC_SET_LEADING_REQU         0x00F0
#define UIC_FR_SET_LEADING_REQU         0x0AF0

/* UIMCS multicast specific codes */
#define UIC_FC_MULTICAST_CREATE         0xF007
#define UIC_FR_MULTICAST_CREATE         0xFA07


/* F080..F0DF: general reserved code-values */
#define UIC_FC_RESERVED_BASE_GENERAL    0xF080

/* F0E0..F0EF: reserved values for Siemens  */
#define UIC_FC_RESERVED_BASE_SIEMENS    0xF0E0

/* F0F0..F0FF: reserved values for ADtranz  */
#define UIC_FC_RESERVED_BASE_ADTRANZ    0xF0F0

/* UIC command field */

/* NADI specific */
#define UIC_C_CORRECT_CONFIG           0
#define UIC_C_CONFIRM_CONFIG           1

/* UIC status field */

/* 0.. 99: function has been executed correctly */
#define UIC_OK                       ALL_OK
#define UIC_S_OK_FIRST               ALL_OK     /* dummy entry */
#define UIC_S_OK                     ALL_OK

/* 100..199: reserved for correct processing */
#define UIC_S_OK_LAST                199     /* dummy entry */

/* 200..250: reserved for processing errors */
#define UIC_S_ERR                    200     /* dummy entry */

/* 201..209: error codes for UAGT */
#define UAGT_OK                     ALL_OK
#define UAGT_INIT_ERR               201 /* error during initialization */
#define UAGT_MAX_INST_ERR           202 /* number of recv-inst. overflow */
#define UAGT_OWNER_INVALID          203 /* invalid owner identification */
#define UAGT_MSG_TOO_SHORT          204 /* incoming msg has too short uic header */


/* 210..219: error codes for UTBC */
#define UIC_S_ERR_IDTS               210    /* error on open/read/write to ID-TS */
#define UIC_S_UIVER_CFL              211    /* UIC version conflict */
#define UIC_S_INIT_ERR               212    /* error on 'utbc_init()' */
#define UIC_S_MEM_ERR                213    /* error allocating buffer or invalid pointers */
#define UIC_S_PARAM_INCORRECT        214    /* input parameter incorrect */
#define UIC_S_UTBC_ERR               215    /* general error */
#define UIC_S_INSERT_ERR             216    /* node is not included in saved NADI */


/* 220..229: error codes for UNGS */
#define  UNGS_OK                     ALL_OK
#define  UIC_S_CMD_INVALID           220
#define  UIC_S_NADI_TOO_SMALL        225
#define  UIC_S_ALLOC_ERR             226
#define  UIC_S_GROUP_ERR             227
#define  UIC_S_ERROR                 228

/* 230..239: error codes for UWTM */
#define UIC_SM_PARAM                 230
#define UIC_SM_MSG                   231
#define UIC_SM_MSG_HEAD              232
#define UIC_SM_MSG_CODE              233
#define UIC_SM_PTR                   234
#define UIC_SM_BUF                   235
#define UIC_SM_MEM                   236
#define UIC_SM_CONFIG                237

#define UWTM_OK                      ALL_OK
#define UWTM_ERR_PARAM               UIC_SM_PARAM
#define UWTM_ERR_MSG                 UIC_SM_MSG
#define UWTM_ERR_PTR                 UIC_SM_PTR
#define UWTM_ERR_BUF                 UIC_SM_BUF
#define UWTM_ERR_MEM                 UIC_SM_MEM
#define UWTM_ERR_MSG_CODE            UIC_SM_MSG_CODE
#define UWTM_ERR_MSG_HEAD            UIC_SM_MSG_HEAD
#define UWTM_ERR_PARAM               UIC_SM_PARAM
#define UWTM_ERR_CONFIG              UIC_SM_CONFIG


/* 240..249: error codes for UIMCS */
#define UIC_S_ERR_MC_DEST            240     /* destination address is incorrect */
#define UIC_S_ERR_MC_SERVICE         241     /* unknown service */
#define UIC_S_ERR_MC_SEND            242     /* Not all multicast destinations  */
#define UIC_S_ERR_MESSAGE_FORMAT     249     /* message format incorrect (length?) */

#define UIC_S_ERR_STATUS             250     /* unknown command */

/* 251..255: processing errors */
#define UIC_S_ERR_FUNCTION_TESTMODE  251     /* processing later possible */
#define UIC_S_ERR_OVERLOAD           252     /* processing later possible */
#define UIC_S_ERR_FUNCTION_DISABLED  253
#define UIC_S_ERR_FUNCTION_OFF       254
#define UIC_S_ERR_FUNCTION_UNKNOWN   255
#define UIC_S_ERR_LAST               255     /* dummy entry */

typedef void (*REPLY_FREE)(void*);

/* ----------------------------------------------------------- */
/*                  UIC messages to the UMS                    */
/*                  UIC_TC_xxx  indicates a callmessage        */
/*                  UIC_TR_xxx  indicates a replymessage       */
/* ----------------------------------------------------------- */

/* UTBC specific messages */
#define UIC_TC_F0_01_DEL_CONFIG            UIC_Header
#define UIC_TR_FA_01_DEL_CONFIG            UIC_Header
#define UIC_TC_F0_02_WRITE_CORRECTION      UIC_MCreateRequ
#define UIC_TR_FA_02_WRITE_CORRECTION      UIC_Header
#define UIC_TC_F0_03_WRITE_VEH_RESNUM      UIC_MCreateRequ
#define UIC_TR_FA_03_WRITE_VEH_RESNUM      UIC_Header


/* NADI specific messages */
#define UIC_TC_00_01_READ_NADI             UIC_Header
#define UIC_TR_0A_01_READ_NADI             UIC_ReadNadi
#define UIC_TC_00_F1_CVT_UIC_TO_TCN        UIC_CvtUicToTcn
#define UIC_TR_0A_F1_CVT_UIC_TO_TCN        UIC_CvtUicToTcnList


/* GROUP specific messages */
#define UIC_TC_B0_01_READ_GROUP            UIC_ReadGroup
#define UIC_TR_BA_01_READ_GROUP            UIC_ReadGroupReply
#define UIC_TC_B0_02_READ_GROUP_LIST       UIC_Header
#define UIC_TR_BA_02_READ_GROUP_LIST       UIC_GroupListReply
#define UIC_TC_B0_03_WRITE_GROUP           UIC_MCreateRequ
#define UIC_TR_BA_03_WRITE_GROUP           UIC_Header
#define UIC_TC_B0_04_WRITE_ALL_GROUPS      UIC_MCreateRequ
#define UIC_TR_BA_04_WRITE_ALL_GROUPS      UIC_Header
#define UIC_TC_B0_05_DELETE_GROUP          UIC_MCreateRequ
#define UIC_TR_BA_05_DELETE_GROUP          UIC_Header
#define UIC_TC_B0_06_DELETE_ALL_GROUPS     UIC_MCreateRequ
#define UIC_TR_BA_06_DELETE_ALL_GROUPS     UIC_Header


/* WTB control specific messages */
#define UIC_TC_F0_04_SET_SLEEP             UIC_Header
#define UIC_TR_FA_04_SET_SLEEP             UIC_Header
#define UIC_TC_F0_05_INAUGURATION_CONTROL  UIC_Header
#define UIC_TR_FA_05_INAUGURATION_CONTROL  UIC_Header
#define UIC_TC_00_22_READ_UWTM_STATE       UIC_Header
#define UIC_TR_0A_22_READ_UWTM_STATE       UIC_UmsState
#define UIC_TC_F0_06_INAUGURATION_ENFORCE  UIC_Header
#define UIC_TR_FA_06_INAUGURATION_ENFORCE  UIC_Header
#define UIC_TC_00_03_CHANGE_OMODE          UIC_Header
#define UIC_TR_0A_03_CHANGE_OMODE          UIC_Header
#define UIC_TC_00_04_READ_UWTM_TOPO        UIC_Header
#define UIC_TR_0A_04_READ_UWTM_TOPO        UIC_TopoReply
#define UIC_TC_00_F0_SET_LEADING_REQU      UIC_SetLeadingRequest
#define UIC_TR_0A_F0_SET_LEADING_REQU      UIC_Header


/* MULTICAST specific messages */
#define UIC_TC_F0_07_MULTICAST_CREATE      UIC_MCreateRequ
#define UIC_TR_FA_07_MULTICAST_CREATE      UIC_MCreateReply


/* for all messages where the length is variable */
#define UIC_MSG_DUMMY_LENGTH    2

/* max. number of allowed vehicles in trainset */
#define UIC_MAX_VEH_TS          6

/* UIC major version >= 1 */
#define UICVER_MAJOR            1

/* UIC agent function-address */
#define UIC_F_UMS    15

/* sizes depending on target machine */
#define BYTESTREAM_SIZE_UIC_Header       10

/*** UIC GROUP definitions ***/
/* group boundaries */
#define UIC_GROUP_ENTRIES_MAX           22   /* max. number of entries into   */
#define UIC_GROUP_DESCRIPTION_LEN       32   /* no. of elements of unicode16  */
                                             /* group description             */
typedef unsigned short  UNICODE16;

typedef struct {

    TYPE_UIC_ID uic_id;       /* binary coded UIC-ID  */
    UNSIGNED8   NodeAddr;     /* TCN address          */
    UNSIGNED8   RangeNo;      /* UIC address          */
    UNSIGNED8   reserved;

} UIC_GroupVeh_long;

#define UIC_SIZEOF_UIC_GROUPVEH_LONG (sizeof(TYPE_UIC_ID)+3*sizeof(UNSIGNED8))

typedef struct {

    TYPE_UIC_ID uic_id;       /* binary coded UIC-ID  */
    UNSIGNED8   reserved;

} UIC_GroupVeh_short;

#define UIC_SIZEOF_UIC_GROUPVEH_SHORT (sizeof(TYPE_UIC_ID)+sizeof(UNSIGNED8))

typedef struct {

    UNICODE16   groupdescr[ UIC_GROUP_DESCRIPTION_LEN ];
                              /* descr. of group in unicode */

} UIC_UnicodeGrDescr;

#define UIC_SIZEOF_UIC_UNICODEGRDESCR (UIC_GROUP_DESCRIPTION_LEN*sizeof(UNICODE16))

typedef struct {

     UNSIGNED8  Group_id;               /* Group Identification  */
     UNSIGNED8  num_entries;            /* total number of entries in this group */
     UIC_UnicodeGrDescr uc_gr_descr;    /* Unicode group description */

} UIC_GroupDescr;

#define UIC_SIZEOF_UIC_GROUPDESCR (2*sizeof(UNSIGNED8)+UIC_SIZEOF_UIC_UNICODEGRDESCR)

typedef struct {

    UIC_GroupDescr      GroupDescr;   /* Group descriptor     */
    UIC_GroupVeh_long   GroupVeh[1];  /* Vehicle description 1 as dummy */

} UIC_Group_long;

#define UIC_SIZEOF_UIC_GROUP_LONG(x) (UIC_SIZEOF_UIC_GROUPDESCR+(x)*UIC_SIZEOF_UIC_GROUPVEH_LONG)

typedef struct {

    UIC_GroupDescr      GroupDescr;   /* Group descriptor     */
    UIC_GroupVeh_short  GroupVeh[UIC_GROUP_ENTRIES_MAX ];
                                      /* Vehicle description  */

} UIC_Group_short;

#define UIC_SIZEOF_UIC_GROUP_SHORT (UIC_SIZEOF_UIC_GROUPDESCR+UIC_GROUP_ENTRIES_MAX*UIC_SIZEOF_UIC_GROUPVEH_SHORT)

typedef struct {

    UNSIGNED8             num_groups;     /* number of groups         */
    UNSIGNED8             reserved;       /* reserved                 */
    UIC_Group_short       group[1];       /* group description 1 as dummy */

} UIC_GroupList;

#define UIC_SIZEOF_UIC_GROUPLIST(x) (2*sizeof(UNSIGNED8)+(x)*UIC_SIZEOF_UIC_GROUP_SHORT)

/*** UIC NADI definitions ***/

/* nadi-state */

#define    UIC_NADI_STATE_ACT     0         /* actual state      */
#define    UIC_NADI_STATE_CONF    1         /* confirmed state   */
#define    UIC_NADI_STATE_INVALID 2         /* invalid NADI      */

typedef int UIC_NadiState;

typedef struct {

    UNSIGNED8     uic_ver;
    UNSIGNED8     uic_subver;
    UNSIGNED8     nadi_state;
    UNSIGNED8     topo_counter;
    UNSIGNED8     num_entries;
    BITSET8       not_av_veh[ TFR_LEN_NA_VEH ];
    BITSET8       add_on_info;
    UNSIGNED8     reserved;

} UIC_GlobalDescr;

#define UIC_SIZEOF_UIC_GLOBALDESCR (5*sizeof(UNSIGNED8)+(TFR_LEN_NA_VEH+1)*sizeof(BITSET8)+sizeof(UNSIGNED8))

/* meaning of elements in UIC_GlobalDescr:

   uic_ver:         UIC main version (inauguration version)
   uic_subver:      UIC subversion   (R-Telegram version)
   nadi_state:      state of NADI ( saved or actual )
   topo_counter:    topography-counter
   num_entries:     total numbers of entries in NADI
   not_av_veh:      confirmed positions of veh. not available
   add_on_info:     Bit 0: orientation of UIC main driving-direction
                           relative to the TCN master
                           0 = invers
                           1 = same
                    Bit 1: set to 1, if at least one node became
                           aditionally active in a confirmed
                           configuration
                    Bit 2: set to 1, if at least one node disappeared
                           in a confirmed configuration
                    Bit 3-7: reserved
*/

typedef struct {

    UNSIGNED8       reserved1;
    UNSIGNED8       reserved2;
    TYPE_UIC_ID     uic_id;
    BITSET8         veh_info[ TFR_LEN_VEH_INFO ];
    UNSIGNED16      veh_res_num;

} TYPE_VehDescr;

#define UIC_SIZEOF_TYPE_VEHDESCR (2*sizeof(UNSIGNED8)+sizeof(TYPE_UIC_ID)+TFR_LEN_VEH_INFO*sizeof(BITSET8)+sizeof(UNSIGNED16))

/* offsets within vehinfo[] for specific attributes */
#define UIC_VEH_INFO_SEATS_OFFSET              0 /* 1st and 2nd class seats  */
#define UIC_VEH_INFO_SEATS_MASK_1ST_CLASS   0x01 /* mask for 1st class seats */
#define UIC_VEH_INFO_SEATS_MASK_2ND_CLASS   0x02 /* mask for 2nd class seats */
#define UIC_VEH_INFO_CARS_OFFSET               1 /* various cars             */
#define UIC_VEH_INFO_CARS_MASK_DINING_CAR   0x01 /* vehicle is a dining car  */
#define UIC_VEH_INFO_CARS_MASK_SLEEPING_CAR 0x10 /* vehicle is a sleeping car*/
#define UIC_VEH_INFO_CARS_MASK_RECUMBENCY_CAR 0x08 /* vehicle is a recumbency car*/
#define UIC_VEH_INFO_CARS_MASK_BUGGAGE_CAR  0x40 /* vehicle is a buggage car */
#define UIC_VEH_INFO_LEADING_OFFSET            9 /* offset leading vehicle   */
#define UIC_VEH_INFO_LEADING_MASK           0x04 /* mask leading vehicle     */
#define UIC_VEH_INFO_FREIGHT_CAR_OFFSET        2 /* offset fright car        */
#define UIC_VEH_INFO_FREIGHT_CAR_MASK       0x02 /* mask freight car         */

typedef struct {
    UNSIGNED8      NodeAddr;
        /* TCN address */
    INTEGER8       num_ctrl_veh;
        /* number of controlled vehicles */
    UNSIGNED8      RangeNo;
        /* UIC address */
    UNSIGNED8      veh_management;
        /* Managing railway company */
    UNSIGNED8      veh_owner;
        /* Owner of the vehicle */
    UNSIGNED8      application_ver;
        /* application version */
    UNSIGNED8      application_id;
        /* application ID */
    BITSET8        TSInfo [TFR_LEN_TS_INFO];
        /* train set information */
    TYPE_VehDescr  VehDescr;
        /* vehicle specific information */
    BITSET8        veh_add_on;
        /* Bit 0: orientation of this vehicle relativ to TCN Master
                  0 = invers
                  1 = same
           Bit 1: orientation of this vehicle relativ to UIC main driving-direction
                  0 = invers
                  1 = same
           Bit 2: property 'leading vehicle'
                  0 = not leading
                  1 = leading
           Bit 3: 'leading request' active  ( xxxx )
                  0 = not active
                  1 = active
           Bit 4-7: reserved */
} UIC_VehDescr;

#define UIC_SIZEOF_UIC_VEHDESCR (sizeof(UNSIGNED8)+sizeof(INTEGER8)+5*sizeof(UNSIGNED8)+TFR_LEN_TS_INFO*sizeof(BITSET8)+UIC_SIZEOF_TYPE_VEHDESCR+sizeof(BITSET8))

/* offsets within TSInfo[] for specific attributes */
#define UIC_TS_INFO_MOTOR_VEHICLE_OFFSET             2 /* motor vehicle attributes  */
#define UIC_TS_INFO_MOTOR_VEHICLE_MASK_ELECTRIC   0x01 /* mask for electric traction*/
#define UIC_TS_INFO_MOTOR_VEHICLE_MASK_DIESEL     0x02 /* mask for diesel traction  */


/* representation of the UIC-NADI in a UIC message */

#define UI_NADI_MAX_ENTRIES   64            /* max. possible NADI entries    */

typedef struct {
    UIC_GlobalDescr  global_descr;
       /* global NADI part */
    UIC_VehDescr     veh_descr [1];
       /* vehicle description 1 as dummy */
       /* the real number of vehicle descriptions depends on the number stored */
       /* in the element num_entries in the global NADI descriptor. */
} UIC_Nadi;

/* the real UIC-NADI  */
typedef struct {
    UIC_GlobalDescr  global_descr;       /* global NADI part       */
    UIC_VehDescr     *p_veh_descr;       /* ptr. to 1st veh. descr */
} UIC_NADI;


/* UIC-Header */
typedef struct {
     UNSIGNED8  owner;
     UNSIGNED8  reserved1;
     UNSIGNED8  dest_veh;
     UNSIGNED8  dest_fct;
     UNSIGNED8  src_veh;
     UNSIGNED8  src_fct;
     UNSIGNED16 code;
     UNSIGNED8  state_cmd;
     UNSIGNED8  reserved2;
} UIC_Header;

/* UIC address ranges boundaries */
#define UIC_INDIVIDUAL_ADDR_LIMIT    63         /* max. individul address  */
#define UIC_COLLECTIVE_ADDR_LIMIT   200         /* max. collective address */
#define UIC_GROUP_ADDR_LIMIT        254         /* max. group address      */


#define UIC_HEADER_OWNER_OFFSET       0         /* offset of owner field in UIC header  */
#define UIC_HEADER_CMD_OFFSET         6         /* determines the offset, where to find */
                                                /* the command within the bytestream    */
#define UIC_HEADER_BYTES             10         /* number of bytes the Header consists of*/


typedef struct {
    UIC_Header  hdr;
    UNSIGNED8   msg[ UIC_MSG_DUMMY_LENGTH ];
} UIC_Msg;

typedef struct {
    TYPE_UIC_ID  uic_id;
       /* set to 0 for inserted vehicles, */
       /* if unknown, set to UIC-ID for vehicle already present */
    BITSET8      ctrl_info;
       /* Bit 0-2: number of gateways inside vehicle */
       /* Bit 3  : 1: veh. was already present; 0: veh. was inserted */
       /* Bit 4-7: reserved */
    UNSIGNED16   veh_res_num;
       /* veh. reservation number that should be written additionally to correction */
} CorrectionData;

/* type definition for correct/confirm telegram:               */
/* it is required, that the sequence direction of announced    */
/* including inserted veh. is  from UIC addr 1 to n of         */
/* previous calculated config.                                 */
/* hdr.state_cmd:                                              */
/*             set to 0, if config. is corrected               */
/*             set to 1, if config. is only confirmed          */
/* num_veh:                                                    */
/*             corrected nb. of vehicles including inserted    */
/* corr_data:                                                  */
/*             dummy entry point for vehicle correction data   */

#define OFFSET_TO_NUM_VEH  10


typedef struct {
    UIC_Header       hdr;
    UNSIGNED8        num_veh;
    CorrectionData   corr_data[ 1 ];
} UIC_WriteCorrection;

/* For UIC Multicast service */

typedef struct {
    UNSIGNED8        num_veh;
    CorrectionData   corr_data[ 1 ];
} UIC_WriteCorrectionMC_data;


/* type definiton for write vehicle res.-number data   */
typedef struct {
    UNSIGNED8    uic_id [TFR_LEN_UIC_ID];
    UNSIGNED8    reserved;
    UNSIGNED16   veh_res_num;
} VehResData;


typedef struct {
    UIC_Header    hdr;
    UNSIGNED8     num_veh;
       /* number of vehicles to which reservation numbers should be written */
    VehResData    res_data [1];
       /* dummy entry point for vehicle reservation number data */
} UIC_WriteResNum;


/* For UIC Multicast service */
typedef struct {
    UNSIGNED8     num_veh;
    VehResData    res_data[ 1 ];
} UIC_WriteResNumMC_data;


typedef struct {
    UIC_Header    hdr;
    UNSIGNED8     uic_addr_in;
       /* uic-address to be converted */
} UIC_CvtUicToTcn;


/* type definition for convert uic to tcn reply telegram */
typedef struct {
    UIC_Header    hdr;
    UNSIGNED8     topo_counter;
    UNSIGNED8     uic_addr_in;
       /* given uic-address in callmsg. to be converted */
    UNSIGNED8     num_tcnaddr;
       /* number of nodes found for given uic address */
    UNSIGNED8     tcn_addr[ 1 ];
       /* dummy entry for list of tcn addresses */
} UIC_CvtUicToTcnList;


#define UIC_SIZEOF_UIC_CVTUICTOTCNLIST(x) (UIC_HEADER_BYTES+3*sizeof(UNSIGNED8)+(x)*sizeof(UNSIGNED8))


/* type definition for read NADI telegram */
typedef struct {
    UIC_Header    hdr;
    UIC_Nadi      nadi;
} UIC_ReadNadi;

#define UIC_SIZEOF_UIC_READNADI(x) (UIC_HEADER_BYTES+UIC_SIZEOF_UIC_GLOBALDESCR+(x)*UIC_SIZEOF_UIC_VEHDESCR)


/* type definition for read group telegram */
typedef struct {
    UIC_Header    hdr;
    UNSIGNED8     group_number;
       /* number of group (201..254) to read */
} UIC_ReadGroup;


/* type definition for read group reply telegram */
typedef struct {
    UIC_Header         hdr;
    UIC_Group_long     group;
       /* structure that contains the group number, number of vehicles, and unicode */
       /* description and dummy entry-point for description of vehicles belonging */
       /* to the given group */
} UIC_ReadGroupReply;

#define UIC_SIZEOF_UIC_READGROUPREPLY(x) (UIC_HEADER_BYTES+UIC_SIZEOF_UIC_GROUP_LONG(x))


/* type definition for write group telegram */
/* group:                                   */
/*    structure that contains the group-    */
/*    number, number of vehicles,           */
/*    and unicode description and all uic_id*/

typedef struct {

    UIC_Header       hdr;
    UIC_Group_short  group;

} UIC_WriteGroup;

/* type definition for write group list     */
/* telegram                                 */
/* num_groups:                              */
/* number of groups the list contains       */
/* group:                                   */
/*    structure that contains the group-    */
/*    number, number of vehicles,           */
/*    and unicode description and all uic_id*/

typedef struct {

    UIC_Header       hdr;
    UIC_GroupList    groupli;

} UIC_WriteGroupList;

/* type definition for delete group telegram */
/* group_number:                             */
/*    number of group (201..255) to be       */
/*    deleted                                */

typedef struct {

    UIC_Header    hdr;
    UNSIGNED8     group_number;

} UIC_DeleteGroup;


/* type definition for read group list   */
/* reply telegram                        */
/* num_groups:                           */
/*    number of group (201..255) to be   */
/*    deleted                            */
/* group:                                */
/*    dummy entry point of group(s)      */
/*    in group list; each element        */
/*    contains group_number, number      */
/*    vehicles in group, group attri-    */
/*    butes and unicode group descr.     */

typedef struct {

    UIC_Header      hdr;
    UIC_GroupList   groupli;

} UIC_GroupListReply;

#define UIC_SIZEOF_UIC_GROUPLISTREPLY(x) (UIC_HEADER_BYTES+UIC_SIZEOF_UIC_GROUPLIST(x))

/*
 *
 * UWTM
 *
 */

#define UIC_NO_CONFLICT                  0
#define UIC_CONFLICT                     1

#define UIC_NOT_STRONG                   0
#define UIC_IS_STRONG                    1

/* Operational modes */
#define UWTM_PASSIVE                      L_UNDEFINED
#define UWTM_SLAVE                        L_SLAVE
#define UWTM_WEAK                         L_WEAK
#define UWTM_STRONG                       L_STRONG

/* states of partner gateway */
#define UIC_ALIVE                        0
#define UIC_DEAD                         1

/* sleep mode */
#define UIC_NO_SLEEP                     0
#define UIC_SLEEP                        1

/* inauguration enforce */
#define UIC_NO_INAUG                     0
#define UIC_INAUG                        1

/* leading vehicle */
#define UIC_NOT_LEADING                  0
#define UIC_LEADING                      1
#define UIC_DIR1                         0
#define UIC_DIR2                         1

#define UWTM_DIR1                        UIC_DIR1
#define UWTM_DIR2                        UIC_DIR2

/* inauguration control */
#define UIC_ALLOWED                      0
#define UIC_INHIBIT                      1

/* inauguration reasons */
#define UIC_IR_UNKNOWN                   0
/* #define UIC_IR_SHORT                     1 cannot occur */
#define UIC_IR_LENGTH                    2
#define UIC_IR_DISCON                    3
#define UIC_IR_RED                       4
#define UIC_IR_CMD                       5

/* inauguration states */
#define UIC_I_OK                         ALL_OK
#define UIC_I_ERR                        1

/* UWTM states */
#define UIC_M_IDLE                       1
#define UIC_M_REGULAR                    2
#define UIC_M_RESTRICTED                 3
/* #define UIC_M_PASSIVE                    4 cannot occur */
#define UIC_M_SINGLE                     5

/* type definition for set leading request telegram   */
/* ld_direc:                                          */
/*           for trainsets with two driver-cabs       */

typedef struct {

    UIC_Header    hdr;
    UNSIGNED8     ld_direc;

} UIC_SetLeadingRequest;

/* type definition for read UMS state    */
/* reply telegram                        */

typedef struct {

    UIC_Header     hdr;
    UNSIGNED8      uwtm_state;
    UNSIGNED8      master_conflict;
    UNSIGNED8      red_state;
    UNSIGNED8      inaug_reason;
    UNSIGNED8      inaug_result;
    UNSIGNED8      res1;
    UNSIGNED32     rel_time;
    UNSIGNED32     tcn_inaug_count;
    UNSIGNED32     uic_inaug_count;
    UNSIGNED32     uic_inaug_error_count;
    UNSIGNED32     ll_inaug_count;
    UNSIGNED32     cmd_inaug_count;
    UNSIGNED32     e_count;
    UNSIGNED32     res2;
    UNSIGNED8      WTB_hardware;
    UNSIGNED8      WTB_software;
    UNSIGNED8      hardware_state;
    UNSIGNED8      link_layer_state;
    UNSIGNED8      net_inhibit;
    UNSIGNED8      node_address;
    UNSIGNED8      node_orient;
    UNSIGNED8      node_strength;
    UNSIGNED8      node_frame_size;
    UNSIGNED8      node_period;
    UNSIGNED8      node_type;
    UNSIGNED8      node_version;
    UNSIGNED8      node_report;
    UNSIGNED8      user_report;

} UIC_UmsState;


/* type definition for read topography reply telegram */
typedef struct {
    UIC_Header  hdr;
       /* UIC header */
#if 0   /* This information may not be forwarded */
    UNSIGNED8   node_address;
       /* TCN address of node that answered */
    UNSIGNED8   node_orient;
        /* orientation of the answering node */
    UNSIGNED8   individual_period;
        /* individual period of the answering node */
    UNSIGNED8   is_strong;
        /* strength of the WTB master */
#endif
    UNSIGNED8   topo_counter;
    UNSIGNED8   number_of_nodes;
        /* number of WTB nodes (gateways) in the train */
    UNSIGNED8   bottom_address;
    UNSIGNED8   top_address;
    UNSIGNED8   uic_address;
    UNSIGNED8   res1;
} UIC_TopoReply;


/* TYPES for UWTM */

#define UIC_TC_CHANGE_OMODE          UIC_Header
#define UIC_TR_CHANGE_OMODE          UIC_Header

#define UIC_TC_SLEEP                 UIC_Header
#define UIC_TR_SLEEP                 UIC_Header

#define UIC_TC_LEADING               UIC_SetLeadingRequest
#define UIC_TR_LEADING               UIC_Header

#define UIC_TC_INAUGURATION_CONTROL  UIC_Header
#define UIC_TR_INAUGURATION_CONTROL  UIC_Header

#define UIC_TC_INAUGURATION_ENFORCE  UIC_Header
#define UIC_TR_INAUGURATION_ENFORCE  UIC_Header

#define UIC_TC_READ_UWTM_TOPOGRAPHY  UIC_Header
#define UIC_TR_READ_UWTM_TOPOGRAPHY  UIC_TopoReply

#define UIC_TC_READ_UWTM_STATE       UIC_Header
#define UIC_TR_READ_UWTM_STATE       UIC_UmsState

#define UIC_TR_READ_UWTM_TOPOGRAPHY_LENGTH (UIC_HEADER_BYTES + 6 * sizeof(UNSIGNED8))
#define UIC_TR_READ_UWTM_STATE_LENGTH (UIC_HEADER_BYTES + 8 * sizeof(UNSIGNED32) + 20 * sizeof(UNSIGNED8))

/* type definition for multicast create    */
/* request telegram                        */
/* mc_destination_veh:                     */
/*    multicast destination address        */
/*    0 = own vehicle                      */
/*    66 = all vehicles                    */
/* mc_service:                             */
/*    supported value in this versions are */
/*    UIC_MCP_SIMPLE                       */
/*    UIC_MCP_SYNC                         */
/*    UIC_MCP_COLLECT_REPLY                */
/* mc_retry_counter:                       */
/*    value between 0..15                  */
/* These mc_.. variables will be copied    */
/* into the UIC telegramheader which will  */
/* be multicasted:                         */
/* mc_destination_veh will become dest_veh */
/* mc_destination_fct will become dest_fct */
/* mc_code will become code                */
/* mc_stus will become state_cmd in the    */
/* UIC header of the telegramm mulicasted. */
/* mc_service, mc_retry_counter and        */
/* mc_priority are control variables for   */
/* multicast server itself!                */

/* service definition of uimcs */

#define UIC_MCP_SIMPLE                 1
#define UIC_MCP_SYNC                   2
#define UIC_MCP_COLLECT_REPLY          3

typedef struct
{
   UNSIGNED8   mc_destination_veh;
   UNSIGNED8   mc_destination_fct;
   ENUM8       mc_service;
   UNSIGNED8   mc_retry_counter;
   UNSIGNED16  mc_code;
   UNSIGNED8   mc_status;
   UNSIGNED8   mc_priority;

} UIC_MCControl;

/* Multicast request telegram consists of UIC Header,*/
/* the multicast control data and the multicast      */
/* message data                                      */

typedef struct
{

   UIC_Header    hdr;
   UIC_MCControl mc_ctrl;
   union
   {                        /* message dummy bytes   */
       UNSIGNED8   mc_message[ UIC_MSG_DUMMY_LENGTH ];
                            /* write correction data */
       UIC_WriteCorrectionMC_data correction_data;
                            /* write reservation nmbr*/
       UIC_WriteResNumMC_data     res_num_data;
                            /* write group           */
       UIC_Group_short            group;
                            /* write list of groups  */
       UIC_GroupList              group_list;
                            /* delete a certain group*/
       UNSIGNED8                  group_number;

   } mc_msg_data;

} UIC_MCreateRequ;

/* number of control bytes mc_... required */
/* for a multicast request telegram        */

#define UIC_MCCREATE_CTRL_BYTES  8

/* type definition for multicast create    */
/* reply telegram                          */

typedef struct
{

   UIC_Header  hdr;
   UNSIGNED16  call_id;

} UIC_MCreateReply;

#define UIC_SIZEOF_UIC_MCREATEREPLY (UIC_HEADER_BYTES+sizeof(UNSIGNED16))

/* type definition for multicast create    */
/* collect reply telegram element          */

/* size in byte of one collect reply element */

#define SIZE_OF_COLLECT_REPLY_ELEMENT  32

typedef struct
{
    UNSIGNED8  data[SIZE_OF_COLLECT_REPLY_ELEMENT];

} UIC_CollectReplyElement;

/* max. number of multicast destinations */
#define MC_DEST_MAX           32

typedef struct
{

   UIC_Header  hdr;
   UNSIGNED8   num_replies;
   UNSIGNED8   reserved;
   UIC_CollectReplyElement reply[MC_DEST_MAX];


} UIC_MCollectReply;

/* type definition for configuration used to */
/* init the UTBC via utbc_init()             */

typedef struct {

    UNSIGNED8   veh_attr[ TFR_LEN_VEH_INFO ];
    UNSIGNED8   uic_number[ TFR_LEN_UIC_ID ];

} ums_VehicleConfiguration;


typedef struct {
    UNSIGNED16    basic_period;     /* the basic period of WTB-LL in ms.
                                       Value: 25 (usually) */
    UNSIGNED8     node_frame_size;  /* is 128, which is the maximum node
                                       frame size for process data */
    UNSIGNED8     node_period;      /* 1 means a 25ms cycle for process data,
                                       2 means 50ms, etc if basic_peroid is 25 */
    UNSIGNED8     uic_ind;          /* for UIC vehicles: 1, other values
                                       only for non-UIC veh. */
    UNSIGNED8     uic_version;      /* version id of supported inauguration
                                       frame version, currently 1 */
    UNSIGNED8     uic_subversion;   /* version of R-telegrams, currently  1 */
    UNSIGNED8     tcn_node_type;    /* 1=strong, 2=slave, 3=weak (usually 3) */
    UNSIGNED8     max_veh_count;    /* 32 vehicles (UIC says 22, but it
                                       does not matter to take more) */
    UNSIGNED8     max_num_nodes;    /* 32 nodes */
    UNSIGNED8     num_NADI_entries; /* 32 nadi entries */
    UNSIGNED8     veh_management;   /* UIC railway code of the managing
                                       railway organisation, user defined
                                       value, like 80=DB,.. */
    UNSIGNED8     veh_owner;        /* like above, but the owning railway */
    UNSIGNED8     national_version; /* user defined */
    UNSIGNED8     applicationID;    /* user defined, may be different for
                                       different projects, only informative
                                       for inauguration */
    BITSET8       KFT_used;         /* set to 2, means node enters single
                                       node mode after 5 sec, 0: it does not */
    UNSIGNED8     num_of_controlled_veh; /* number of controlled vehicles by
                                            this node, negative for vahicles
                                            with more than 1 node */
    UNSIGNED8     ts_attr[ TFR_LEN_TS_INFO ]; /* the trainset attributes
                                                 as defined in UIC556, important
                                                 for inauguration is only no.
                                                 48,49 for traction control */
    UNSIGNED8     name_space_prefix_train[ 10 ]; /* not used herein, set
                                                    to 0 */
    UNSIGNED8     name_space_prefix_half_train[ 10 ]; /* not used herein, set
                                                         to 0 */
    ums_VehicleConfiguration   veh_conf[ UIC_MAX_VEH_TS ];
                                     /* UIC IDs and attibutes for each
                                        vehicle (up to 6) as defined in UIC556 */
    /* UIC ID shall be big-endian "WORD40", but there are no calculations
       made with it */
} ums_NodeConfigurationTable;
void ums_monitor(void);
#endif /* UMS_H_ */
