/*************************************************************************
**        Copyright (C) Siemens AG 1997 All Rights Reserved. Confidential
**========================================================================
**
**    PROJECT::    TCN Train Communication Network
                   JDP Joint Development Project

**    MODULE::     ADID Application Defined Inauguration Data

**    WORKFILE::   $Workfile:   ADID.C  $
**************************************************************************
**    TASK::       ID Traffic Store Access

**************************************************************************
**    AUTHOR::     Heindel / Reinwald
**    CREATED::    12.11.97
**========================================================================
**    HISTORY::    AUTHOR::   $Author:   HEINDEL_AR  $
                   REVISION:: $Revision:   1.9  $
                   MODTIME::  $Date:   17 Dec 1998 13:05:42  $
      $Log:   O:\PRJ\JDP\FUE\SW\PVCS\ARC\TCN_FW\UIC\SRC\ADID.CV_  $
 *
 *    Rev 1.9   17 Dec 1998 13:05:42   HEINDEL_AR
 * added function li_t_SetFrame() (non O_166)
 *
 *    Rev 1.8   10 Dec 1998 13:42:18   HEINDEL_AR
 * implementation for li_t_read_nk() (#ifndef O_166)
 *
 *    Rev 1.7   01 Oct 1998 16:13:38   REINWALD_LU
 * ls_T_GetTopo... replaced
 *
 *    Rev 1.6   05 Aug 1998 15:57:06   REINWALD_LU
 * added sync function to copy own idts into sink port
 *
 *    Rev 1.5   04 Aug 1998 15:11:56   HEINDEL_AR
 * extended interface for IDTS manipulation
 *
 *    Rev 1.4   26 Jun 1998 09:10:54   REINWALD_LU
 * uic_ind inserted into NSDB and idts
 *
 *    Rev 1.3   29 May 1998 08:05:16   REINWALD_LU
 * adaption to unnamed node
 *
 *    Rev 1.2   16 Mar 1998 09:40:34   Reinwald_Lu
 * bottom-top sort order, new topoframe definition
 *
 *    Rev 1.1   24 Feb 1998 13:33:06   REINWALD_LU
 * Changes UIC 12/97
 *
 *    Rev 1.0   27 Nov 1997 15:44:56   REINWALD_LU
 * First Release
**************************************************************************/
#include "profile.h"
#include "tcntypes.h"
#include "pi_sys.h"
#include "am_sys.h"
#include <string.h>
#include "bmi.h"
#include "ums.h"
#include "adid.h"
#include "uwtm.h"


/* global variables */
BOOLEAN idts_opened;
UNSIGNED8 *p_idts;
Type_NodeDescriptor node_desc;
UNSIGNED8 number_of_nodes;
UNSIGNED8 top_addr;
UNSIGNED8 bottom_addr;
UNSIGNED8 own_node_addr;
static Type_Topography a_topo;

/*****************************************************************************
 *
 *   public functions
 *
 *****************************************************************************/

/******************************************************************************
*   name:         li_t_IDTSInit
*   function:     initialisation of global variables
******************************************************************************/
void li_t_IDTSInit(UNSIGNED8 *ts_start)
{
  idts_opened = FALSE;
  p_idts = ts_start;
  /* clear whole idts */
  memset(p_idts, 0, LI_INAUG_ENTRIES_MAX * sizeof(TYPE_InaugData));
} /* Ende li_t_IDTSInit */

/******************************************************************************
*   name:         li_t_IDTSReset
*   function:     write zero in TCN address field
******************************************************************************/
void li_t_IDTSReset(void)
{
  int i;
  UNSIGNED8 *p;
  SPLDECL

  SPLMAC();
  /* write zero in TCN address field */
  p = p_idts;
  for (i = 0; i < LI_INAUG_ENTRIES_MAX; i++) {
    *(p + 2) = 0;
    p += sizeof(TYPE_InaugData);
  }
  SPLX();
}

/******************************************************************************
*   name:         li_t_IDTSConfig
*   function:     initialisation of global variables,which are defined in NSDB
******************************************************************************/
void li_t_IDTSConfig(ums_NodeConfigurationTable *ums)
{
  node_desc.node_frame_size = ums->node_frame_size;
  node_desc.node_period = ums->node_period;
  node_desc.node_key.node_type = ums->uic_ind;
  node_desc.node_key.node_version = ums->uic_version;
} /* Ende li_t_IDTSConfig */

/******************************************************************************
*   name:         li_t_IDTSOpen
*   function:
*   return value:
******************************************************************************/
LI_T_RESULT li_t_IDTSOpen(
  UNSIGNED8 *p_number_nodes, /* number of nodes entered in IDTS */
  UNSIGNED16 *p_handle   /* access handle of IDTS */
  )
{
  if (uwtm_get_topo (&a_topo) != LI_OK || !map_wtb_inaugurated()) {
    /* look if WTB is really inaugurated and working, topo is not sure enough */
    *p_number_nodes = 1;
    *p_handle = 0; /* topo is never 0 */
    number_of_nodes = 1;
    return(LI_OK);
  }
  *p_number_nodes = a_topo.number_of_nodes;
  number_of_nodes = a_topo.number_of_nodes;
  bottom_addr = a_topo.bottom_address;
  top_addr = a_topo.top_address;
  own_node_addr = a_topo.node_address;

  *p_handle = a_topo.topo_counter;

  idts_opened = TRUE;
  return (LI_OK);
} /* Ende li_t_IDTSOpen */

/******************************************************************************
*   name:         li_t_IDTSReadNext
*   function:
*   return value:
******************************************************************************/
LI_T_RESULT li_t_IDTSReadNext(
  UNSIGNED16      handle,        /* Handle for IDTS */
  UNSIGNED8       *p_index,      /* internal index in IDTS */
  TYPE_InaugData  *p_inaug_data  /* inauguration data */
  )
{
  SPLDECL

  int wtb_result;
  int i;

  if (number_of_nodes == 1) {
    if (*p_index == 0) {
      *p_inaug_data = *((TYPE_InaugData*)(p_idts));
    }
    (*p_index)++;
    return (LI_OK);
  }


  if (*p_index > number_of_nodes-1) {   /* index may be 0..num_nodes -1 */
    return (LI_NO_DATA);
  }

  /* lock against other calls */
  SPLMAC();
  i = bottom_addr + *p_index;
  if (i>63) i -= 63;
  if (i > 32) {
    i -= 31;
  }
  *p_inaug_data = *((TYPE_InaugData*)(p_idts + i*sizeof(TYPE_InaugData)));

  SPLX();

  if (!map_wtb_inaugurated()) {
    /* look if WTB is really inaugurated and working, topo is not sure enough */
    return(LI_INVALID);
  }
  wtb_result = uwtm_get_topo (&a_topo);
  if (wtb_result != LI_OK) { /* no valid topo */
    return(LI_INVALID);
  }
  if (handle != a_topo.topo_counter) {
    return(LI_INVALID);
  }
  (*p_index)++;
  return (LI_OK);
} /* Ende li_t_IDTSReadNext */

/******************************************************************************
*   name:         li_t_IDTSClose
*   function:
*   return value: LI_OK or LI_INVALID
******************************************************************************/
LI_T_RESULT li_t_IDTSClose(
  UNSIGNED16 handle   /* Handle for IDTS */
  )
{
  int wtb_result;

  idts_opened = FALSE;

  if (number_of_nodes != 1) {
    if (!map_wtb_inaugurated()) {
      /* look if WTB is really inaugurated and working, topo is not sure enough */
      return(LI_INVALID);
    }
    wtb_result = uwtm_get_topo (&a_topo);
    if (wtb_result != LI_OK) { /* no valid topo */
      return(LI_INVALID);
    }
  }
  if (handle != a_topo.topo_counter) {
    return (LI_INVALID);
  }

  return(LI_OK);
} /* Ende if li_t_IDTSClose */

/******************************************************************************
*   name:         li_t_ChgInaugDataByte
*   function:     changes own inauguration data, uses ID port 0
*                 only one Byte is accessed, or even bits if mask is != 0xff
*   return value: LI_OK or LI_INVALID
******************************************************************************/
LI_T_RESULT li_t_ChgInaugDataByte(
  UNSIGNED8 inaug_data_offs, /* inauguration data offset */
  UNSIGNED8 data,            /* inauguration data byte */
  UNSIGNED8 mask             /* inauguration data mask */
  )
{
  SPLDECL
  UNSIGNED8 *p_data;

  if (inaug_data_offs >= LI_INAUG_DATA_SIZE_MAX) {   /* offset may be 0..123 */
    return (LI_INVALID);
  }
  /* calculate address of data byte: remember header size 4 Bytes */
  p_data = p_idts + 4 * sizeof(UNSIGNED8) + inaug_data_offs;

  /* lock against other calls */
  SPLMAC();

  *p_data  = (*p_data & ~mask) | (mask & data);

  SPLX();
  return (LI_OK);
} /* Ende li_t_ChgInaugDataByte */

/******************************************************************************
*   name:         li_t_GetInaugDataByte
*   function:     fetches own inauguration data, uses ID port 0
*   return value: LI_OK or LI_INVALID
******************************************************************************/
LI_T_RESULT li_t_GetInaugDataByte(
  UNSIGNED8 inaug_data_offs, /* inauguration data offset */
  UNSIGNED8 *data            /* inauguration data byte */
  )
{
  UNSIGNED8 *p_data;

  if (inaug_data_offs >= LI_INAUG_DATA_SIZE_MAX) {   /* offset may be 0..123 */
    return (LI_INVALID);
  }
  /* calculate address of data byte: remember header size 4 Bytes */
  p_data = p_idts + 4 * sizeof(UNSIGNED8) + inaug_data_offs;

  *data = *p_data;

  return (LI_OK);
} /* Ende li_t_GetInaugDataByte */

/******************************************************************************
*   name:         li_t_IDTSChgInaugData
*   function:     changes own inauguration data, uses ID port 0
*   return value: LI_OK or LI_INVALID
******************************************************************************/
LI_T_RESULT li_t_ChgInaugData(
  UNSIGNED8      inaug_data_size, /* inauguration data length */
  TYPE_InaugData *p_inauguration  /* inauguration data */
  )
{
  SPLDECL

  if (inaug_data_size > LI_INAUG_DATA_SIZE_MAX) {
    return (LI_INVALID);
  }
  if (p_inauguration == NULL) {
    return (LI_INVALID);
  }

  p_inauguration->size = inaug_data_size;
  p_inauguration->UIC_Ind = node_desc.node_key.node_type;
  p_inauguration->UIC_Ver = node_desc.node_key.node_version;
  /* lock against other calls */
  SPLMAC();
  memcpy(p_idts,p_inauguration,inaug_data_size);
  SPLX();
  return (LI_OK);
} /* Ende li_t_IDTSChgInaugData */

/******************************************************************************
*   name:         li_t_IDTSChgInaugDataSD
*   function:     changes own Inauguration Data Source Device field, uses IDTS
*                 port 0
*                 also copies source port to own sink port
*   return value: LI_OK or LI_INVALID
******************************************************************************/
LI_T_RESULT li_t_ChgInaugDataSD(UNSIGNED8      sd  /* inauguration data Source
                                                    * Device information
                                                    * (offset 2) */
                                )
{
#if !defined(O_166)
  int index;
#endif

  *(p_idts + 2) = sd;
#if !defined(O_166)
  index = (sd & 0x7f);
  if (index > 32) {
    index -= 31;
  }
  *((TYPE_InaugData *)p_idts + index) = *(TYPE_InaugData *)p_idts;
#endif
  return (LI_OK);
} /* Ende li_t_IDTSChgInaugData */

/******************************************************************************
*   name:         li_t_IDTSGetOwnInaugData
*   function:     get own Inauguration Data
*   return value: pointer to own Inauguration Data
******************************************************************************/
UNSIGNED8 *li_t_GetOwnInaugData(void)
{
  return(p_idts);
}


/******************************************************************************
*   name:         li_t_IDTSGetInaugData
*   function:     not implemented
*   return value: LI_INVALID
******************************************************************************/
LI_T_RESULT li_t_GetInaugData(
  void **p_inaug_data_list /* pointer to inauguration data list of all nodes */
  )
{
  UNREFERENCED(p_inaug_data_list);
  /* zzzz: TBD */
  return(LI_INVALID);
} /* Ende li_t_GetInaugData */

/******************************************************************************
*   name:         li_t_Sync
*   function:     copies own inauguration data from source to sink port
*   return value: none
******************************************************************************/
void li_t_Sync(void)
{
  UNSIGNED8 sd = ((TYPE_InaugData *)p_idts)->src_dev & 0x7F; /* mask direction bit */
  if (sd >32 ) sd -= 31;
  *((TYPE_InaugData *)p_idts + sd) = *(TYPE_InaugData *)p_idts;
} /* Ende li_t_Sync */


#ifndef O_166
/* <ATR:01> */
void li_t_SetFrame(unsigned char frame_sd,
                   unsigned char *frame_data,
                   unsigned char frame_size)
{
  unsigned char *pidts = p_idts + ((frame_sd > 32) ? (frame_sd - 31) : frame_sd) *
                   sizeof(TYPE_InaugData);

  memcpy(pidts, frame_data, frame_size);
  if (frame_size < sizeof(TYPE_InaugData)) {
    memset(pidts + frame_size, 0, sizeof(TYPE_InaugData) - frame_size);
  }
}

/******************************************************************************
*   name:         li_t_read_nk
*   function:     read node key array from Inauguration Data Traffic Store
*                 for traditional TCN topography structure
*   return value: none
******************************************************************************/
void li_t_read_nk(Type_Topography *topo)
{
	int i, node_addr = topo->bottom_address;

	for (i = 0; i < topo->number_of_nodes && i < sizeof(topo->NK) / sizeof(topo->NK[0]);
		 i++, node_addr = (node_addr == 63 ? 1 : (node_addr + 1))) {
		int j = ((node_addr > 32) ? (node_addr - 31) : node_addr);

		if      (p_idts[j * sizeof(TYPE_InaugData) +  9] & 0x20) topo->NK[i].node_type = 0x10;
		else if (p_idts[j * sizeof(TYPE_InaugData) + 24] & 0x06) topo->NK[i].node_type = 0x20;
		else                                                     topo->NK[i].node_type = 0x30;
		topo->NK[i].node_type |= p_idts[j * sizeof(TYPE_InaugData)] & 0x0F;

		topo->NK[i].node_version = p_idts[j * sizeof(TYPE_InaugData) + 20];
	}

//    int i;
//    UNSIGNED8 node_addr = topo->bottom_address;
//
//    for (i=0; i<topo->number_of_nodes;i++) {
//        if (node_addr > 32) {
//            topo->NK[i].node_type = *(p_idts + sizeof (TYPE_InaugData) *
//                                      (node_addr - 31));
//            topo->NK[i].node_version = *(p_idts + sizeof (TYPE_InaugData) *
//                                         (node_addr - 31) + 1);
//        } else {
//            topo->NK[i].node_type = *(p_idts + sizeof (TYPE_InaugData) *
//                                      node_addr);
//            topo->NK[i].node_version = *(p_idts + sizeof (TYPE_InaugData) *
//                                         node_addr + 1);
//        }
//        node_addr++;
//        if (node_addr > 63) node_addr = 1;
//    }
//    for (/* rest of NK array */;i<63;i++) {
//        topo->NK[i].node_type = 0;
//        topo->NK[i].node_version = 0;
//    }
}
#endif

