/*************************************************************************
**        Copyright (C) Siemens AG 1997 All Rights Reserved. Confidential    
**========================================================================
**
**    PROJECT::    TCN Train Communication Network
                   JDP Joint Development Project

**    MODULE::     ADID Application Defined Inauguration Data

**    WORKFILE::   $Workfile:   ADID.H  $
**************************************************************************
**    TASK::       ID Traffic Store Access

**************************************************************************
**    AUTHOR::     Heindel
**    CREATED::    20.10.97
**========================================================================
**    HISTORY::    AUTHOR::   $Author:   HEINDEL_AR  $
                   REVISION:: $Revision:   1.6  $  
                   MODTIME::  $Date:   17 Dec 1998 13:08:30  $
      $Log:   O:\PRJ\JDP\FUE\SW\PVCS\ARC\TCN_FW\UIC\INC\ADID.HV_  $
 * 
 *    Rev 1.6   17 Dec 1998 13:08:30   HEINDEL_AR
 * added prototype li_t_SetFrame() (non O_166)
 * 
 *    Rev 1.5   10 Dec 1998 13:40:50   HEINDEL_AR
 * prototype for li_t_read_nk() (#ifndef O_166)
 * 
 *    Rev 1.4   05 Aug 1998 15:56:24   REINWALD_LU
 * added sync function to copy own idts into sink port
 * 
 *    Rev 1.3   04 Aug 1998 15:11:04   HEINDEL_AR
 * extended interface for IDTS manipulation
 * 
 *    Rev 1.2   16 Mar 1998 09:43:06   Reinwald_Lu
 * new topoframe definition
 * 
 *    Rev 1.1   24 Feb 1998 13:32:54   REINWALD_LU
 * Changes UIC 12/97 
 * 
 *    Rev 1.0   27 Nov 1997 15:43:02   REINWALD_LU
 * First Release
**************************************************************************/


#ifndef ADID_H_
#define ADID_H_


#define LI_OK       ALL_OK
#define LI_INVALID  0x01
#define LI_NO_DATA  0x02

#define LI_INAUG_DATA_SIZE_MAX   124
#define LI_INAUG_ENTRIES_MAX     33 /* 1 for SRC and 32 for SINK */

typedef UNSIGNED8 LI_T_RESULT; 

/* definition for inauguration data  */

typedef struct{

    UNSIGNED8    UIC_Ind;     
    UNSIGNED8    UIC_Ver;  
    UNSIGNED8    src_dev;  
    UNSIGNED8    size;
    UNSIGNED8    Data[ LI_INAUG_DATA_SIZE_MAX ]; 

} TYPE_InaugData;


/* meaning of elements in TYPE_InaugData:

   UIC_Ind:         UIC indication
                    0 means 'no-UIC' vehicle
   UIC_Ver:         major UIC version
   src_dev:         Bits 0..6: node address,
                    Bit 7: orientation to the tcn-master, 0 = invers, 1 = same        
   size:            size of inauguration data in bytes ( up to 124 bytes )                 
 */



void li_t_IDTSInit (UNSIGNED8 *ts_start);
/*{
  Initializes internal variables.
  }*/

void li_t_IDTSReset (void);
/*{
  Reset the Inauguration Data Traffic Store: write zero in TCN address field
  }*/

void li_t_IDTSConfig (ums_NodeConfigurationTable *ums);
/*{
  Initializes internal variables, which are defined by application.
  }*/

LI_T_RESULT li_t_IDTSOpen(
  UNSIGNED8  *p_number_nodes, /* number of nodes entered in IDTS */
  UNSIGNED16 *p_handle        /* access handle of IDTS */
);
/*{
  Open ID-Traffic Store and create a handle to assure consistent access to
  the the IDTS.

  The handle of the IDTS is returned in 'handle' the number of inaugurated
  nodes is returned in 'p_number_nodes'.

  li_t_IDTSOpen returns LI_OK if vehicle is inaugurated, otherwise returns
      LI_NO_DATA    vehicle is not inaugurated, no IDTS available
  }*/

LI_T_RESULT li_t_IDTSReadNext(
  UNSIGNED16      handle,           /* Handle for IDTS */
  UNSIGNED8       *p_index,         /* internal index in IDTS */
  TYPE_InaugData  *p_inaug_data     /* inauguration data */
);
/*{
  Read one entry (inauguration data) in the ID Traffic Store identified by
  'handle'.

  '*p_index' has to be initialized with 0 for the first call and successive
  calls has to use the value returned by the previous call.
  
  The inauguration data is returned in 'p_inaug_data'.

  li_t_IDTSReadNext returns LI_OK when inauguration data was succesfully
  fetched, otherwise returns
      LI_NO_DATA  - no further entries in ID Traffic Store
      LI_INVALID  - inauguration data for *p_node_id is invalid
                  - 'handle' is not an IDTS handle
                  - '*p_index' is not a valid IDTS index
  }*/
 
LI_T_RESULT li_t_IDTSClose(
  UNSIGNED16 handle   /* Handle for IDTS */
);
/*{
  Closing of Inauguration Data Traffic Store

  li_t_IDTSClose returns LI_OK if all accesses to IDTS were consistent,
  otherwise
      LI_INVALID  - an inauguration or unname occured while reading
                    the IDTS
                  - 'handle' is not an IDTS handle
  }*/

LI_T_RESULT li_t_ChgInaugData(
  UNSIGNED8       inaug_data_size,     /* inauguration data length */
  TYPE_InaugData* p_inauguration       /* inauguration data */
);
/*{
  Change own inauguration data

  li_t_ChgInaugData returns LI_OK if the change of inauguration data was
  successful, otherwise returns
      LI_INVALID   'inaug_data_size' is greater than 124
                   'p_inauguration' is an invalid pointer
  }*/

LI_T_RESULT li_t_ChgInaugDataSD(UNSIGNED8      sd  /* inauguration data Source
                                                    * Device information 
                                                    * (offset 2) */
                                );
/*{
  changes own Inauguration Data Source Device field, uses IDTS port 0
  The Source Device field has the following content:
     (node_orient << 7) | tcn_addr
  also copies source port to own sink port
  returns LI_OK
  }*/

LI_T_RESULT li_t_ChgInaugDataByte(
  UNSIGNED8   inaug_data_offs, /* inauguration data offset */
  UNSIGNED8   data,            /* inauguration data, one byte */
  UNSIGNED8   mask             /* bit mask, set to 0xff for whole Byte */ 
);
/*{
  Change own inauguration data. Only the byte with the offset inaug_data_offs
  is set to the value *p_data. Only these bits are changed, whose mask bit is 1.

  li_t_ChgInaugDataByte returns LI_OK if the change of inauguration data was
  successful, otherwise returns
      LI_INVALID   'inaug_data_offs' is greater than 123
  }*/

LI_T_RESULT li_t_GetInaugDataByte(
  UNSIGNED8   inaug_data_offs, /* inauguration data offset */
  UNSIGNED8   *data            /* inauguration data, one byte */
);
/*{
  Returns one Data Byte with the offset inaug_data_offs from own inaug data.
  li_t_GetInaugDataByte returns LI_OK if the fetch of inauguration data was
  successful, otherwise returns
      LI_INVALID   'inaug_data_offs' is greater than 123
  }*/

LI_T_RESULT li_t_GetInaugData(
  void **p_inaug_data_list /* pointer to inauguration data list of all nodes */
);
/*{
  Get inauguration data of all nodes.
  This function is provided specifically for access by network management.
  li_t_GetInaugData is not implemented and returns LI_INVALID 
  }*/

void li_t_Sync(void);
/*{
  Copies own inauguration data from source port to sink port.
  }*/


UNSIGNED8 *li_t_GetOwnInaugData(void);
/*{
  Get own inauguration data.
  returns NULL in case of uninitialized Inauguration Data Traffic Store
  }*/
/***************************************************************************** 
 *  informal: ID TS structure needed by this module: 
 *
 * D_ID_TS_ADDR-> +--+--+--+--+----------------------------------------------+
 * Port 0 SRC     |NT|NV|SD|SZ|  Data[124]                                   |
 *                +--+--+--+--+----------------------------------------------+
 * Port 1 Sink    |NT|NV|SD|SZ|  Data[124]                                   |
 *                +--+--+--+--+----------------------------------------------+
 * Port 2/33 Sink |NT|NV|SD|SZ|  Data[124]                                   |
 *                +--+--+--+--+----------------------------------------------+
 * Port 3/34 Sink |NT|NV|SD|SZ|  Data[124]                                   |
 *                +--+--+--+--+----------------------------------------------+
 * .....          |  |  |  |  |                                              |
 *                
 * .....          .....
 *                
 * Port 29/60 Sink|NT|NV|SD|SZ|  Data[124]                                   |
 *                +--+--+--+--+----------------------------------------------+
 * Port 30/61 Sink|NT|NV|SD|SZ|  Data[124]                                   |
 *                +--+--+--+--+----------------------------------------------+
 * Port 31/62 Sink|NT|NV|SD|SZ|  Data[124]                                   |
 *                +--+--+--+--+----------------------------------------------+
 * Port 32/63 Sink|NT|NV|SD|SZ|  Data[124]                                   |
 *                +--+--+--+--+----------------------------------------------+
 *
 * Ports except Port 0 (own Source) and 1 (Sink for Master) are double-used. 
 * Due to the wtb-restriction to 32 devices, the ports cannot overlap. 
 *
 * NT = Node Type or UIC Version 
 * NV = Node Version or UIC Subversion 
 * SD = Source Device 
 * SZ = Size of Data
 * Data = up to 124 Bytes of application specific inauguration data
 ****************************************************************************/

#ifndef O_166
void li_t_SetFrame(unsigned char frame_sd,
                   unsigned char *frame_data,
                   unsigned char frame_size);
/*{
  copy inauguration data from IDTS into send frame
  }*/

void li_t_read_nk(Type_Topography *topo);
/*{
  read node key array from Inauguration Data Traffic Store
  for traditional TCN topography structure
  }*/
#endif

#endif   /* ADID_H_ */
