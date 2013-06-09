/*************************************************************************
**        Copyright (C) Siemens AG 1997 All Rights Reserved. Confidential
**========================================================================
**
**    PROJECT::    TCN Train Communication Network
                   JDP Joint Development Project

**    MODULE::     UIC Mapping Server

**    WORKFILE::   $Workfile:   UMSCONV.C  $
**************************************************************************
**    TASK::       Conversion between bytestream and structures for all
                   UMS Datagrams

**************************************************************************
**    AUTHOR::     Just ()
**    CREATED::    12.02.98
**========================================================================
**    HISTORY::    AUTHOR:: $Author:   Just_Pete  $
      REVISION::            $Revision:   1.14  $
      MODTIME::             $Date:   07 Oct 1998 16:24:22  $
      $Log:   O:\PRJ\JDP\FUE\SW\PVCS\ARC\TCN_FW\UIC\SRC\UMSCONV.Cv_  $
 * 
 *    Rev 1.14   07 Oct 1998 16:24:22   Just_Pete
 * HTON16, NTOH16, HTON32, NTOH32:
 * Die Konvertierung wird auf little- und 
 * big-Endian-Plattformen identisch ausgeführt.
 * Die Compile-Option O_LE entfällt somit.
 * 
 *    Rev 1.13   21 Sep 1998 12:30:10   HILBRICH_KL
 * wrong reservation number telegram format
 *
 *    Rev 1.12   02 Jun 1998 11:32:26   Just_Pete
 * bug fixes in NTOH_uic_fr_read_uwtm_state()
 *
 *    Rev 1.11   22 May 1998 09:53:56   HILBRICH_KL
 * exchange_UIC_HEADER()': reserved bytes
 * handling changed
 *
 *    Rev 1.10   11 May 1998 16:24:38   REINWALD_LU
 * O_GATEWAY for exchange_UIC_header, less includes
 *
 *    Rev 1.9   08 May 1998 14:28:32   Just_Pete
 * wtbconv_15_07__HTON(): UIC_WRITE_VEH_RES_NUM:
 * Die Anzahl der beteiligten Fahrzeuge muß
 * in den  Bytestream übertragen werden.
 *
 *    Rev 1.8   08 May 1998 09:23:58   REINWALD_LU
 * reasons for compiler warnings removed
 *
 *    Rev 1.7   07 May 1998 13:46:06   Just_Pete
 * exchange_UIC_HEADER():um für die reply
 * die eigene UIC-Reihungsnummer eintragen
 * zu können, wird die Funktion uwtm_get_my_own_uic_address()
 * aufgerufen
 *
 *    Rev 1.6   28 Apr 1998 09:00:24   Just_Pete
 * wtbconv_15_07__HTON: Korrekturdaten
 * müssen kopiert werden, wenn mc_status == 0!!
 *
 *    Rev 1.4   31 Mar 1998 14:33:30   HILBRICH_KL
 * convert- function names adapted
 *
 *    Rev 1.1   09 Mar 1998 15:18:42   HILBRICH_KL
 * delivered to ADtranz on 09/03/98
 *
 *    Rev 1.0   03 Mar 1998 09:07:58   REINWALD_LU
 * delivered to Adtranz in 03/98 - new file for conversion routines
**************************************************************************/

#include <string.h> /* memcpy */
#include "profile.h"
#include "tcntypes.h"
#include "bmi.h"
#include "ums.h"
#ifdef O_GATEWAY
#include "am_sys.h"
#include "uwtm.h"
#endif

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

/*******************************************************/
int HTON16( UNSIGNED8* dest, UNSIGNED16 source )
{
                                        /* net is big endian       */
    *dest = (UNSIGNED8)(source>>8);     /* place high-order byte first*/
    dest++;
    *dest = (UNSIGNED8)(source & 0xFF); /* place low-order byte   */

    return sizeof( UNSIGNED16 );
}

/*******************************************************/
UNSIGNED16 NTOH16( UNSIGNED8* source )
{
    UNSIGNED16 tmp = 0;
    UNSIGNED16 dest = 0;

                          /* net is big endian       */
    tmp = *source++;      /* high order Byte arrives 1st */
    tmp = (UNSIGNED16)(tmp & 0xFF);
    dest = (UNSIGNED16)(tmp<<8);
    tmp = *source;
    tmp = (UNSIGNED16)(tmp & 0xFF);
    dest |= tmp;

    return dest;
}

/*******************************************************/
int HTON32( UNSIGNED8* dest, UNSIGNED32 source )
{
                                               /* net is big endian       */
    *dest = (UNSIGNED8)((source>>24) & 0xFF);  /* place high-order byte of high-order word */
    dest++;
    *dest = (UNSIGNED8)((source>>16) & 0xFF);  /* place low-order  byte of high-order word */
    dest++;
    *dest = (UNSIGNED8)((source>>8) & 0xFF);   /* place high-order byte of low-order  word */
    dest++;
    *dest = (UNSIGNED8)(source & 0xFF);      /* place low-order  byte of low-order  word */

    return sizeof( UNSIGNED32 );
}

/*******************************************************/
UNSIGNED32 NTOH32( UNSIGNED8* source )
{
    UNSIGNED32 tmp  = 0;
    UNSIGNED32 dest = 0;
                       /* net is big endian       */
    tmp = *source++;
    tmp = tmp & 0xFF;
    dest |= tmp<<24;
    tmp = *source++;
    tmp = tmp & 0xFF;
    dest |= tmp <<16;
    tmp = *source++;
    tmp = tmp & 0xFF;
    dest |= tmp<<8;
    tmp = *source;
    tmp = tmp & 0xFF;
    dest |= tmp;

    return dest;
}


/*******************************************************/
/* functions decoding and encoding structures from or  */
/* to the bytestream WTB format                        */
/*******************************************************/

/*************************************************************************/
/* function converts the header of an UIC telegram into a host data      */
/* structure from a big endian bytestream                                */
/*************************************************************************/

UNSIGNED8 *wtbconv_uic_hdr_NTOH (UNSIGNED8  *p_byte_in,
                                 UIC_Header *p_hdr)
{
   /* UIC-header for provider-request has to be converted           */
   /* Be careful about the sequence of the structure elements       */
   /* since this is a conversion from a byte stream into a structure*/
   p_hdr->owner       = *(p_byte_in++);
   p_hdr->reserved1   = *(p_byte_in++);
   p_hdr->dest_veh    = *(p_byte_in++);
   p_hdr->dest_fct    = *(p_byte_in++);
   p_hdr->src_veh     = *(p_byte_in++);
   p_hdr->src_fct     = *(p_byte_in++);
   p_hdr->code        = NTOH16(p_byte_in);
   p_byte_in += sizeof(UNSIGNED16);
   p_hdr->state_cmd   = *(p_byte_in++);
   p_hdr->reserved2   = *(p_byte_in++);

   return p_byte_in;
}

void NTOH_header (UNSIGNED8 *p_byte_in,
                  UIC_Header *p_hdr)
{
   /* UIC-header for provider-request has to be converted           */
   /* Be careful about the sequence of the structure elements       */
   /* since this is a conversion from a byte stream into a structure*/
   p_hdr->owner       = *(p_byte_in++);
   p_hdr->reserved1   = *(p_byte_in++);
   p_hdr->dest_veh    = *(p_byte_in++);
   p_hdr->dest_fct    = *(p_byte_in++);
   p_hdr->src_veh     = *(p_byte_in++);
   p_hdr->src_fct     = *(p_byte_in++);
   p_hdr->code        = NTOH16(p_byte_in);
   p_byte_in += sizeof(UNSIGNED16);
   p_hdr->state_cmd   = *(p_byte_in++);
   p_hdr->reserved2   = *(p_byte_in++);
}

/*************************************************************************/
/* function converts the header of an UIC telegram from host data        */
/* structure into a big endian bytestream                                */
/*************************************************************************/

UNSIGNED8 *wtbconv_uic_hdr_HTON (UNSIGNED8  *p_byte_out,
                                 UIC_Header *p_hdr)
{
   /* UIC-header from provider-request has to be converted           */
   /* Be careful about the sequence of the structure elements        */
   /* since this is a conversion from a byte stream into a structure */
   /* destination and source header data is exchanged                */
   *(p_byte_out++)  = p_hdr->owner;
   *(p_byte_out++)  = p_hdr->reserved1;
   *(p_byte_out++)  = p_hdr->dest_veh;
   *(p_byte_out++)  = p_hdr->dest_fct;
   *(p_byte_out++)  = p_hdr->src_veh;
   *(p_byte_out++)  = p_hdr->src_fct;
   p_byte_out += HTON16(p_byte_out, p_hdr->code);
   *(p_byte_out++)  = p_hdr->state_cmd;
   *(p_byte_out++)  = p_hdr->reserved2;
   return p_byte_out;            /* address into outgoing bytestream */
}

void HTON_header (UIC_Header *p_hdr,
                  UNSIGNED8  *p_byte_out)
{
   /* UIC-header from provider-request has to be converted           */
   /* Be careful about the sequence of the structure elements        */
   /* since this is a conversion from a byte stream into a structure */
   /* destination and source header data is exchanged                */
   *(p_byte_out++)  = p_hdr->owner;
   *(p_byte_out++)  = p_hdr->reserved1;
   *(p_byte_out++)  = p_hdr->dest_veh;
   *(p_byte_out++)  = p_hdr->dest_fct;
   *(p_byte_out++)  = p_hdr->src_veh;
   *(p_byte_out++)  = p_hdr->src_fct;
   p_byte_out += HTON16(p_byte_out, p_hdr->code);
   *(p_byte_out++)  = p_hdr->state_cmd;
   *(p_byte_out++)  = p_hdr->reserved2;
}

/*************************************************************************/
/* function converts the vehicle description of a NADI from a host data  */
/* structure into a big endian bytestream                                */
/*************************************************************************/

UNSIGNED8 *wtbconv_nadi_vehdescr_HTON (UIC_VehDescr *p_veh_descr,
                                       UNSIGNED8    *p_byte_out,
                                       UNSIGNED8    num_veh)
{
    UNSIGNED8 i, veh;

   /* Be careful about the sequence of the structure elements       */
   /* since this is a conversion into a byte stream from a structure*/

   for (veh = 0; veh < num_veh; veh++) {
       *(p_byte_out++)   = p_veh_descr->NodeAddr;
       *(p_byte_out++)   = p_veh_descr->num_ctrl_veh;
       *(p_byte_out++)   = p_veh_descr->RangeNo;
       *(p_byte_out++)   = p_veh_descr->veh_management;
       *(p_byte_out++)   = p_veh_descr->veh_owner;
       *(p_byte_out++)   = p_veh_descr->application_ver;
       *(p_byte_out++)   = p_veh_descr->application_id;

       for (i=0; i<TFR_LEN_TS_INFO; i++) {
           *(p_byte_out++)   = p_veh_descr->TSInfo[i];
       }
       *(p_byte_out++)   = p_veh_descr->VehDescr.reserved1;
       *(p_byte_out++)   = p_veh_descr->VehDescr.reserved2;

       /* UIC ID */
       for (i=0; i<TFR_LEN_UIC_ID; i++) {
           *(p_byte_out++)   = p_veh_descr->VehDescr.uic_id.uic_id[i];
       }

       /* vehicle information */
       for (i=0; i<TFR_LEN_VEH_INFO; i++) {
           *(p_byte_out++) = p_veh_descr->VehDescr.veh_info[i];
       }

       p_byte_out += HTON16(p_byte_out, p_veh_descr->VehDescr.veh_res_num);

       /* add on information */
       *(p_byte_out++) = p_veh_descr->veh_add_on;

       p_veh_descr++;
   }
   return p_byte_out;
}


/*************************************************************************/
/* function converts the vehicle description of a NADI into a host data  */
/* structure from a big endian bytestream                                */
/*************************************************************************/
UNSIGNED8 *wtbconv_nadi_vehdescr_NTOH (UIC_VehDescr *p_veh_descr,
                                       UNSIGNED8    *p_byte_in,
                                       UNSIGNED8    num_veh)
{
    UNSIGNED8 i, veh;

   /* Be careful about the sequence of the structure elements       */
   /* since this is a conversion from a byte stream into a structure*/

   for (veh = 0; veh < num_veh; veh++) {
       p_veh_descr->NodeAddr =         *(p_byte_in++);
       p_veh_descr->num_ctrl_veh =     *(p_byte_in++);
       p_veh_descr->RangeNo =          *(p_byte_in++);
       p_veh_descr->veh_management =   *(p_byte_in++);
       p_veh_descr->veh_owner =        *(p_byte_in++);
       p_veh_descr->application_ver =  *(p_byte_in++);
       p_veh_descr->application_id =   *(p_byte_in++);

       for (i=0; i<TFR_LEN_TS_INFO; i++) {
           p_veh_descr->TSInfo[i] =    *(p_byte_in++);
       }
       p_veh_descr->VehDescr.reserved1 = *(p_byte_in++);
       p_veh_descr->VehDescr.reserved2 = *(p_byte_in++);
       for (i=0; i<TFR_LEN_UIC_ID; i++) {
           p_veh_descr->VehDescr.uic_id.uic_id[i] = *(p_byte_in++);
       }
       for (i=0; i<TFR_LEN_VEH_INFO; i++) {
           p_veh_descr->VehDescr.veh_info[i] = *(p_byte_in++);
       }
       p_veh_descr->VehDescr.veh_res_num = NTOH16(p_byte_in);
       p_byte_in += sizeof(UNSIGNED16);
       p_veh_descr->veh_add_on = *(p_byte_in++);

       p_veh_descr++;
   }
   return p_byte_in;
}


/*************************************************************************/
/* function converts the group descriptor of a Group from a host data    */
/* structure into a big endian bytestream                                */
/*************************************************************************/

UNSIGNED8 *wtbconv_group_descr_HTON(UNSIGNED8      *p_byte_out,
                                    UIC_Group_long *p_group_src)
{
    UNSIGNED8 i;

   /* UIC-Group-descriptor for provider-request has to be converted */
   /* Be careful about the sequence of the structure elements       */
   /* since this is a conversion into a byte stream from a structure*/
    *(p_byte_out++)   = p_group_src->GroupDescr.Group_id;
    *(p_byte_out++)   = p_group_src->GroupDescr.num_entries;

    for (i=0; i<UIC_GROUP_DESCRIPTION_LEN; i++) {
        p_byte_out += HTON16(p_byte_out,
               p_group_src->GroupDescr.uc_gr_descr.groupdescr[i]);
    }
    return p_byte_out;
}

/*************************************************************************/
/* function converts the group vehicle descriptor of a Group from a host */
/* data structure into a big endian bytestream                           */
/*************************************************************************/

UNSIGNED8 *wtbconv_group_veh_HTON (UNSIGNED8         *p_byte_out,
                                   UIC_GroupVeh_long *p_veh_descr_src,
                                   UNSIGNED8         num_veh)
{
    UNSIGNED8 i, veh;

   /* Be careful about the sequence of the structure elements       */
   /* since this is a conversion from a byte stream into a structure*/

   for (veh = 0; veh < num_veh; veh++) {

       for (i=0; i<TFR_LEN_UIC_ID; i++) {
           *(p_byte_out++)   = p_veh_descr_src->uic_id.uic_id[i];
       }

       *(p_byte_out++)   = p_veh_descr_src->NodeAddr;
       *(p_byte_out++)   = p_veh_descr_src->RangeNo;
       *(p_byte_out++)   = p_veh_descr_src->reserved;
       p_veh_descr_src++;
   }
   return p_byte_out;
}

/*************************************************************************/
/* function converts the group descriptor of a Group from a host data    */
/* structure into a big endian bytestream                                */
/*************************************************************************/

UNSIGNED8 *wtbconv_group_descr_short_HTON(UNSIGNED8       *p_byte_out,
                                          UIC_Group_short *p_group_src)
{
    UNSIGNED8 i;

   /* UIC-Group-descriptor for provider-request has to be converted */
   /* Be careful about the sequence of the structure elements       */
   /* since this is a conversion into a byte stream from a structure*/
    *(p_byte_out++)   = p_group_src->GroupDescr.Group_id;
    *(p_byte_out++)   = p_group_src->GroupDescr.num_entries;

    for (i=0; i<UIC_GROUP_DESCRIPTION_LEN; i++) {
        p_byte_out += HTON16(p_byte_out,
               p_group_src->GroupDescr.uc_gr_descr.groupdescr[i]);
    }
    return p_byte_out;
}

/*************************************************************************/
/* function converts the group vehicle descriptor of a Group from a host */
/* data structure into a big endian bytestream                           */
/*************************************************************************/

UNSIGNED8 *wtbconv_group_veh_short_HTON (UNSIGNED8          *p_byte_out,
                                         UIC_GroupVeh_short *p_veh_descr_src,
                                         UNSIGNED8          num_veh)
{
    UNSIGNED8 i, veh;
   /* Be careful about the sequence of the structure elements       */
   /* since this is a conversion from a byte stream into a structure*/

   for (veh = 0; veh < num_veh; veh++) {

       for (i=0; i<TFR_LEN_UIC_ID; i++) {
           *(p_byte_out++)   = p_veh_descr_src->uic_id.uic_id[i];
       }
       *(p_byte_out++) = p_veh_descr_src->reserved;

       p_veh_descr_src++;
   }
   return p_byte_out;
}

/*************************************************************************/
/* function converts the group descriptor of a Group into a host data    */
/* structure from a big endian bytestream                                */
/*************************************************************************/

UNSIGNED8 *wtbconv_group_descr_NTOH(UNSIGNED8      *p_byte_in,
                                    UIC_Group_long *p_group_dest)
{
    UNSIGNED8 i;

   /* UIC-Group-descriptor for provider-request has to be converted */
   /* Be careful about the sequence of the structure elements       */
   /* since this is a conversion into a byte stream from a structure*/
    p_group_dest->GroupDescr.Group_id    = *(p_byte_in++);
    p_group_dest->GroupDescr.num_entries = *(p_byte_in++);

    for (i=0; i<UIC_GROUP_DESCRIPTION_LEN; i++) {
        p_group_dest->GroupDescr.uc_gr_descr.groupdescr[i] = NTOH16(p_byte_in);
        p_byte_in += sizeof(UNSIGNED16);
    }
    return p_byte_in;
}

/*************************************************************************/
/* function converts the group vehicle descriptor of a Group into a host */
/* data structure from a big endian bytestream                           */
/*************************************************************************/

UNSIGNED8 *wtbconv_group_veh_NTOH (UNSIGNED8         *p_byte_in,
                                   UIC_GroupVeh_long *p_veh_descr_dest,
                                   UNSIGNED8         num_veh)
{
    UNSIGNED8 i, veh;
    TYPE_UIC_ID *p_uic_id;
    p_uic_id = &p_veh_descr_dest->uic_id;

   /* Be careful about the sequence of the structure elements       */
   /* since this is a conversion from a byte stream into a structure*/

   for (veh = 0; veh < num_veh; veh++) {

       for (i=0; i<TFR_LEN_UIC_ID; i++) {
           p_veh_descr_dest->uic_id.uic_id[i] = *(p_byte_in++);
       }

       p_veh_descr_dest->NodeAddr = *(p_byte_in++);
       p_veh_descr_dest->RangeNo  = *(p_byte_in++);
       p_veh_descr_dest->reserved = *(p_byte_in++);
       p_veh_descr_dest++;
   }
   return p_byte_in;
}

/*************************************************************************/
/* function converts the group descriptor of a Group into a host data    */
/* structure from a big endian bytestream                                */
/*************************************************************************/

UNSIGNED8 *wtbconv_group_descr_short_NTOH(UNSIGNED8       *p_byte_in,
                                          UIC_Group_short *p_group_dest)
{
    UNSIGNED8 i;

   /* UIC-Group-descriptor for provider-request has to be converted */
   /* Be careful about the sequence of the structure elements       */
   /* since this is a conversion into a byte stream from a structure*/
    p_group_dest->GroupDescr.Group_id    = *(p_byte_in++);
    p_group_dest->GroupDescr.num_entries = *(p_byte_in++);

    for (i=0; i<UIC_GROUP_DESCRIPTION_LEN; i++) {
        p_group_dest->GroupDescr.uc_gr_descr.groupdescr[i] = NTOH16(p_byte_in);
        p_byte_in += sizeof(UNSIGNED16);
    }
    return p_byte_in;
}

/*************************************************************************/
/* function converts the group vehicle descriptor of a Group into a host */
/* data structure from a big endian bytestream                           */
/*************************************************************************/

UNSIGNED8 *wtbconv_group_veh_short_NTOH (UNSIGNED8          *p_byte_in,
                                         UIC_GroupVeh_short *p_veh_descr_dest,
                                         UNSIGNED8          num_veh)
{
    UNSIGNED8 i, veh;

   /* Be careful about the sequence of the structure elements       */
   /* since this is a conversion from a byte stream into a structure*/

   for (veh = 0; veh < num_veh; veh++) {

       for (i=0; i<TFR_LEN_UIC_ID; i++) {
          p_veh_descr_dest->uic_id.uic_id[i] = *(p_byte_in++);
       }
       p_veh_descr_dest->reserved = *(p_byte_in++);
       p_veh_descr_dest++;
   }
   return p_byte_in;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* - convert UIC adress into TCN adress, call                            */
/*************************************************************************/

void wtbconv_32_01_02__HTON (UNSIGNED8*       p_bytestream_dest,
                             UIC_CvtUicToTcn* p_uic_add_src)
{
    /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_dest = wtbconv_uic_hdr_HTON(p_bytestream_dest, &p_uic_add_src->hdr);
    *p_bytestream_dest = p_uic_add_src->uic_addr_in;

    return;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* - convert UIC adress into TCN adress, call                            */
/*************************************************************************/

void wtbconv_32_01_02__NTOH (UNSIGNED8*  p_bytestream_src,
                             UIC_CvtUicToTcn* p_uic_add_dest)
{
   /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_src = wtbconv_uic_hdr_NTOH(p_bytestream_src, &p_uic_add_dest->hdr);
    p_uic_add_dest->uic_addr_in = *p_bytestream_src;

    return;
}


/*************************************************************************/
/* function converts UIC telegram                                        */
/* - convert UIC adress into TCN adress, reply                           */
/*************************************************************************/

void wtbconv_32_01_02A_HTON (UNSIGNED8*           p_bytestream_dest,
                             UIC_CvtUicToTcnList* p_tcn_list_src)
{
    UNSIGNED8 tcn_cnt,i=0;
   /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_dest = wtbconv_uic_hdr_HTON(p_bytestream_dest, &p_tcn_list_src->hdr);
    *p_bytestream_dest++ = p_tcn_list_src->topo_counter;
    *p_bytestream_dest++ = p_tcn_list_src->uic_addr_in;
    *p_bytestream_dest++ = p_tcn_list_src->num_tcnaddr;
    tcn_cnt = p_tcn_list_src->num_tcnaddr;

    while (0 < tcn_cnt--)
    {
        *p_bytestream_dest++ = p_tcn_list_src->tcn_addr[i];
        i++;
    }

    return;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* - convert UIC adress into TCN adress, reply                           */
/*************************************************************************/

void wtbconv_32_01_02A_NTOH (UNSIGNED8*           p_bytestream_src,
                             UIC_CvtUicToTcnList* p_tcn_list_dest)
{
    UNSIGNED8 tcn_cnt, i=0;
   /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_src = wtbconv_uic_hdr_NTOH(p_bytestream_src, &p_tcn_list_dest->hdr);
    p_tcn_list_dest->topo_counter = *p_bytestream_src++;
    p_tcn_list_dest->uic_addr_in = *p_bytestream_src++;
    p_tcn_list_dest->num_tcnaddr = *p_bytestream_src++;
    tcn_cnt = p_tcn_list_dest->num_tcnaddr;

    while (0 < tcn_cnt-- )
    {
        p_tcn_list_dest->tcn_addr[i] = *p_bytestream_src++;
        i++;
    }

    return;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* - 0.01  read NADI, call                                               */
/*************************************************************************/

void wtbconv_00_01__HTON (UNSIGNED8*  p_bytestream_dest,
                          UIC_Header* p_header_src)
{
    /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_dest = wtbconv_uic_hdr_HTON(p_bytestream_dest, p_header_src);

    return;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* - 0.01  read NADI, call                                               */
/*************************************************************************/

void wtbconv_00_01__NTOH (UNSIGNED8*  p_bytestream_src,
                          UIC_Header* p_header_dest)
{
   /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_src = wtbconv_uic_hdr_NTOH(p_bytestream_src, p_header_dest);

    return;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* - 0.01A read NADI, reply                                              */
/*************************************************************************/

void wtbconv_00_01A_HTON (UNSIGNED8*    p_bytestream_dest,
                          UIC_ReadNadi* p_NadiMsg_src)
{
    UNSIGNED8 i;

   /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_dest = wtbconv_uic_hdr_HTON(p_bytestream_dest, &p_NadiMsg_src->hdr);

   /* Be careful about the sequence of the structure elements       */
   /* since this is a conversion from a byte stream into a structure*/

   /* UIC-NADI-header for provider-request has to be converted      */
    *(p_bytestream_dest++)  = p_NadiMsg_src->nadi.global_descr.uic_ver;
    *(p_bytestream_dest++)  = p_NadiMsg_src->nadi.global_descr.uic_subver;
    *(p_bytestream_dest++)  = p_NadiMsg_src->nadi.global_descr.nadi_state;
    *(p_bytestream_dest++)  = p_NadiMsg_src->nadi.global_descr.topo_counter;
    *(p_bytestream_dest++)  = p_NadiMsg_src->nadi.global_descr.num_entries;
    for (i=0; i<TFR_LEN_NA_VEH; i++) {
       *(p_bytestream_dest++)  = p_NadiMsg_src->nadi.global_descr.not_av_veh[i];
    }
    *(p_bytestream_dest++)  = p_NadiMsg_src->nadi.global_descr.add_on_info;
    *(p_bytestream_dest++)  = p_NadiMsg_src->nadi.global_descr.reserved;

   /* UIC-NADI-global descriptor for provider-request has to be converted*/
    wtbconv_nadi_vehdescr_HTON (p_NadiMsg_src->nadi.veh_descr, p_bytestream_dest,
                    p_NadiMsg_src->nadi.global_descr.num_entries);
   return;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* - 0.01A read NADI, reply                                              */
/* IMPORTANT NOTICE: provide enough stack or heap for p_NadiMsg_dest,    */
/*                   the specified type UIC_ReadNadi contains space for  */
/*                   only one vehicle!                                   */
/*************************************************************************/

void wtbconv_00_01A_NTOH (UNSIGNED8*  p_bytestream_src,
                          UIC_ReadNadi*   p_NadiMsg_dest)
{
    UNSIGNED8 i;

   /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_src = wtbconv_uic_hdr_NTOH(p_bytestream_src, &p_NadiMsg_dest->hdr);

   /* Be careful about the sequence of the structure elements       */
   /* since this is a conversion from a byte stream into a structure*/

   /* UIC-NADI-header for provider-request has to be converted      */
    p_NadiMsg_dest->nadi.global_descr.uic_ver =      *(p_bytestream_src++);
    p_NadiMsg_dest->nadi.global_descr.uic_subver =   *(p_bytestream_src++);
    p_NadiMsg_dest->nadi.global_descr.nadi_state =   *(p_bytestream_src++);
    p_NadiMsg_dest->nadi.global_descr.topo_counter = *(p_bytestream_src++);
    p_NadiMsg_dest->nadi.global_descr.num_entries =  *(p_bytestream_src++);
    for (i=0; i<TFR_LEN_NA_VEH; i++) {
       p_NadiMsg_dest->nadi.global_descr.not_av_veh[i] = *(p_bytestream_src++);
    }
    p_NadiMsg_dest->nadi.global_descr.add_on_info =  *(p_bytestream_src++);
    p_NadiMsg_dest->nadi.global_descr.reserved    =  *(p_bytestream_src++);

   /* UIC-NADI-global descriptor for provider-request has to be converted*/
    wtbconv_nadi_vehdescr_NTOH (p_NadiMsg_dest->nadi.veh_descr, p_bytestream_src,
                    p_NadiMsg_dest->nadi.global_descr.num_entries);
   return;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* -11.01  read group, call                                              */
/*************************************************************************/

void wtbconv_11_01__HTON (UNSIGNED8*     p_bytestream_dest,
                      UIC_ReadGroup* p_msg_src)
{
   /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_dest = wtbconv_uic_hdr_HTON(p_bytestream_dest, &p_msg_src->hdr);
    *p_bytestream_dest = p_msg_src->group_number;

    return;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* -11.01  read group, call                                              */
/*************************************************************************/

void wtbconv_11_01__NTOH (UNSIGNED8*     p_bytestream_src,
                          UIC_ReadGroup* p_msg_dest)
{
    /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_src = wtbconv_uic_hdr_NTOH(p_bytestream_src, &p_msg_dest->hdr);
    p_msg_dest->group_number = *p_bytestream_src;

    return;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* -11.01A read group, reply                                             */
/*************************************************************************/

void wtbconv_11_01A_HTON (UNSIGNED8*       p_bytestream_dest,
                          UIC_ReadGroupReply *p_msg_src)
{
   /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_dest = wtbconv_uic_hdr_HTON(p_bytestream_dest, &p_msg_src->hdr);

   /* group descriptor for provider-request has to be converted   */
    p_bytestream_dest = wtbconv_group_descr_HTON(p_bytestream_dest, &p_msg_src->group);

    /* group descriptor for provider-request has to be converted   */
    p_bytestream_dest = wtbconv_group_veh_HTON(p_bytestream_dest,
              p_msg_src->group.GroupVeh, p_msg_src->group.GroupDescr.num_entries);

    return;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* -11.01A read group, reply                                             */
/*************************************************************************/

void wtbconv_11_01A_NTOH (UNSIGNED8*          p_bytestream_src,
                          UIC_ReadGroupReply *p_msg_dest)
{
    /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_src = wtbconv_uic_hdr_NTOH(p_bytestream_src, &p_msg_dest->hdr);

    /* group descriptor for provider-request has to be converted   */
    p_bytestream_src = wtbconv_group_descr_NTOH(p_bytestream_src, &p_msg_dest->group);

    /* group descriptor for provider-request has to be converted   */
    p_bytestream_src = wtbconv_group_veh_NTOH(p_bytestream_src,
              p_msg_dest->group.GroupVeh, p_msg_dest->group.GroupDescr.num_entries);

    return;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* -11.02  list all groups, call                                         */
/*************************************************************************/

void wtbconv_11_02__HTON (UNSIGNED8*  p_bytestream_dest,
                          UIC_Header* p_header_src)
{
    /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_dest = wtbconv_uic_hdr_HTON(p_bytestream_dest, p_header_src);

    return;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* -11.02  list all groups, call                                         */
/*************************************************************************/

void wtbconv_11_02__NTOH (UNSIGNED8*  p_bytestream_src,
                          UIC_Header* p_header_dest)
{

    /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_src = wtbconv_uic_hdr_NTOH(p_bytestream_src, p_header_dest);

    return;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* -11.02A list all groups, reply                                        */
/*************************************************************************/

void wtbconv_11_02A_HTON (UNSIGNED8*          p_bytestream_dest,
                          UIC_GroupListReply* p_grouplist_src)
{
    UNSIGNED8 i;
    UIC_Group_short *p_group_src;

   /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_dest = wtbconv_uic_hdr_HTON(p_bytestream_dest, &p_grouplist_src->hdr);

    *(p_bytestream_dest++) = p_grouplist_src->groupli.num_groups;
    *(p_bytestream_dest++) = p_grouplist_src->groupli.reserved;

    /* loop all groups */
    for (i = 0; i< p_grouplist_src->groupli.num_groups; i++) {
        p_group_src = &(p_grouplist_src->groupli.group[i]);
       /* group descriptor for provider-request has to be converted   */
        p_bytestream_dest = wtbconv_group_descr_short_HTON(p_bytestream_dest, p_group_src);

       /* vehicle descriptor for provider-request has to be converted   */
        p_bytestream_dest = wtbconv_group_veh_short_HTON(p_bytestream_dest,
              p_group_src->GroupVeh, p_group_src->GroupDescr.num_entries);


    }

    return;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* -11.02A list all groups, reply                                        */
/*************************************************************************/

void wtbconv_11_02A_NTOH (UNSIGNED8*           p_bytestream_src,
                          UIC_GroupListReply*  p_grouplist_dest)
{
    UNSIGNED8 i;
    UIC_Group_short *p_group_dest;

   /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_src = wtbconv_uic_hdr_NTOH(p_bytestream_src, &p_grouplist_dest->hdr);

    p_grouplist_dest->groupli.num_groups = *(p_bytestream_src++);
    p_grouplist_dest->groupli.reserved    = *(p_bytestream_src++);

    /* loop all groups */
    for (i = 0; i< p_grouplist_dest->groupli.num_groups; i++) {
        p_group_dest = &(p_grouplist_dest->groupli.group[i]);
       /* group descriptor for provider-request has to be converted   */
        p_bytestream_src = wtbconv_group_descr_short_NTOH(p_bytestream_src, p_group_dest);

       /* group descriptor for provider-request has to be converted   */
        p_bytestream_src = wtbconv_group_veh_short_NTOH(p_bytestream_src,
              p_group_dest->GroupVeh, p_group_dest->GroupDescr.num_entries);
        p_group_dest = (UIC_Group_short*)&(p_group_dest->GroupVeh[p_group_dest->GroupDescr.num_entries+1]);


    }

    return;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* -11.03  write group, call                                             */
/*************************************************************************/

void wtbconv_11_03__HTON (UNSIGNED8*       p_bytestream_dest,
                          UIC_WriteGroup*  p_wgroup_src)
{
    /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_dest = wtbconv_uic_hdr_HTON(p_bytestream_dest, &p_wgroup_src->hdr);

   /* group descriptor for provider-request has to be converted   */
    p_bytestream_dest = wtbconv_group_descr_short_HTON(p_bytestream_dest, &p_wgroup_src->group);

   /* group descriptor for provider-request has to be converted   */
    p_bytestream_dest = wtbconv_group_veh_short_HTON(p_bytestream_dest,
              p_wgroup_src->group.GroupVeh, p_wgroup_src->group.GroupDescr.num_entries);
    return;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* -11.03  write group, call                                             */
/*************************************************************************/

void wtbconv_11_03__NTOH (UNSIGNED8*       p_bytestream_src,
                          UIC_WriteGroup*  p_wgroup_dest)
{
   /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_src = wtbconv_uic_hdr_NTOH(p_bytestream_src, &p_wgroup_dest->hdr);

   /* group descriptor for provider-request has to be converted   */
    p_bytestream_src = wtbconv_group_descr_short_NTOH(p_bytestream_src, &p_wgroup_dest->group);
   /* group descriptor for provider-request has to be converted   */
    p_bytestream_src = wtbconv_group_veh_short_NTOH(p_bytestream_src,
              p_wgroup_dest->group.GroupVeh, p_wgroup_dest->group.GroupDescr.num_entries);

    return;
}


/*************************************************************************/
/* function converts UIC telegram                                        */
/* -11.03A write group, reply                                            */
/*************************************************************************/

void wtbconv_11_03A_HTON (UNSIGNED8*  p_bytestream_dest,
                          UIC_Header* p_header_src)
{
    /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_dest = wtbconv_uic_hdr_HTON(p_bytestream_dest, p_header_src);

    return;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* -11.03A write group, reply                                            */
/*************************************************************************/

void wtbconv_11_03A_NTOH (UNSIGNED8*  p_bytestream_src,
                          UIC_Header* p_header_dest)
{
    /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_src = wtbconv_uic_hdr_NTOH(p_bytestream_src, p_header_dest);

    return;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* -11.04  write groups, call                                            */
/*************************************************************************/

void wtbconv_11_04__HTON (UNSIGNED8*          p_bytestream_dest,
                          UIC_WriteGroupList* p_grouplist_src)
{
    UNSIGNED8 i;
    UIC_Group_short *p_group_src;

   /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_dest = wtbconv_uic_hdr_HTON(p_bytestream_dest, &p_grouplist_src->hdr);

    *(p_bytestream_dest++) = p_grouplist_src->groupli.num_groups;
    *(p_bytestream_dest++) = p_grouplist_src->groupli.reserved;

    /* loop all groups */
    for (i = 0; i< p_grouplist_src->groupli.num_groups; i++) {
        p_group_src = &p_grouplist_src->groupli.group[i];
       /* group descriptor for provider-request has to be converted   */
        p_bytestream_dest = wtbconv_group_descr_short_HTON(p_bytestream_dest, p_group_src);

       /* group descriptor for provider-request has to be converted   */
        p_bytestream_dest = wtbconv_group_veh_short_HTON(p_bytestream_dest,
              p_group_src->GroupVeh, p_group_src->GroupDescr.num_entries);

    }

    return;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* -11.04  write groups, call                                            */
/*************************************************************************/

void wtbconv_11_04__NTOH (UNSIGNED8*          p_bytestream_src,
                          UIC_WriteGroupList* p_grouplist_dest)
{
    UNSIGNED8 i;
    UIC_Group_short *p_group_dest;

   /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_src = wtbconv_uic_hdr_NTOH(p_bytestream_src, &p_grouplist_dest->hdr);

    p_grouplist_dest->groupli.num_groups = *(p_bytestream_src++);
    p_grouplist_dest->groupli.reserved    = *(p_bytestream_src++);

    /* loop all groups */
    for (i = 0; i< p_grouplist_dest->groupli.num_groups; i++) {
        p_group_dest = &p_grouplist_dest->groupli.group[i];
       /* group descriptor for provider-request has to be converted   */
        p_bytestream_src = wtbconv_group_descr_short_NTOH(p_bytestream_src, p_group_dest);

       /* group descriptor for provider-request has to be converted   */
        p_bytestream_src = wtbconv_group_veh_short_NTOH(p_bytestream_src,
              p_group_dest->GroupVeh, p_group_dest->GroupDescr.num_entries);

    }

    return;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* -11.04A write groups, reply                                           */
/*************************************************************************/

void wtbconv_11_04A_HTON (UNSIGNED8*  p_bytestream_dest,
                          UIC_Header* p_header_src)
{
    /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_dest = wtbconv_uic_hdr_HTON(p_bytestream_dest, p_header_src);

    return;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* -11.04A write groups, reply                                           */
/*************************************************************************/

void wtbconv_11_04A_NTOH (UNSIGNED8*  p_bytestream_src,
                          UIC_Header* p_header_dest)
{
    /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_src = wtbconv_uic_hdr_NTOH(p_bytestream_src, p_header_dest);

    return;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* -11.05  delete group, call                                            */
/*************************************************************************/

void wtbconv_11_05__HTON (UNSIGNED8*       p_bytestream_dest,
                          UIC_DeleteGroup* p_msg_src)
{
    /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_dest = wtbconv_uic_hdr_HTON(p_bytestream_dest, &p_msg_src->hdr);
    *p_bytestream_dest = p_msg_src->group_number;

    return;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* -11.05  delete group, call                                            */
/*************************************************************************/

void wtbconv_11_05__NTOH (UNSIGNED8*       p_bytestream_src,
                          UIC_DeleteGroup* p_msg_dest)
{
    /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_src = wtbconv_uic_hdr_NTOH(p_bytestream_src, &p_msg_dest->hdr);
    p_msg_dest->group_number = *p_bytestream_src;

    return;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* -11.05A delete group, reply                                           */
/*************************************************************************/

void wtbconv_11_05A_HTON (UNSIGNED8*  p_bytestream_dest,
                          UIC_Header* p_header_src)
{
    /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_dest = wtbconv_uic_hdr_HTON(p_bytestream_dest, p_header_src);

    return;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* -11.05A delete group, reply                                           */
/*************************************************************************/

void wtbconv_11_05A_NTOH (UNSIGNED8*  p_bytestream_src,
                          UIC_Header* p_header_dest)
{
    /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_src = wtbconv_uic_hdr_NTOH(p_bytestream_src, p_header_dest);

    return;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* -11.06  delete all groups, call                                       */
/*************************************************************************/

void wtbconv_11_06__HTON (UNSIGNED8*  p_bytestream_dest,
                          UIC_Header* p_header_src)
{
    /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_dest = wtbconv_uic_hdr_HTON(p_bytestream_dest, p_header_src);

    return;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* -11.06  delete all groups, call                                       */
/*************************************************************************/

void wtbconv_11_06__NTOH (UNSIGNED8*  p_bytestream_src,
                          UIC_Header* p_header_dest)
{
    /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_src = wtbconv_uic_hdr_NTOH(p_bytestream_src, p_header_dest);

    return;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* -11.06A delete all groups, reply                                      */
/*************************************************************************/

void wtbconv_11_06A_HTON (UNSIGNED8*  p_bytestream_dest,
                          UIC_Header* p_header_src)
{
    /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_dest = wtbconv_uic_hdr_HTON(p_bytestream_dest, p_header_src);

    return;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* -11.06A delete all groups, reply                                      */
/*************************************************************************/

void wtbconv_11_06A_NTOH (UNSIGNED8*  p_bytestream_src,
                          UIC_Header* p_header_dest)
{
    /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_src = wtbconv_uic_hdr_NTOH(p_bytestream_src, p_header_dest);

    return;
}


/*
 *
 * UWTM conversion routines
 *
 */

/*
 * Leading request
 */

void NTOH_uic_fc_leading(UNSIGNED8 *callmsg, UIC_TC_LEADING *call_t_msg)
{
  callmsg = wtbconv_uic_hdr_NTOH(callmsg, (UIC_Header *)call_t_msg);
  call_t_msg->ld_direc = *callmsg;

}

void HTON_uic_fc_leading(UIC_TC_LEADING *call_t_msg, UNSIGNED8 *callmsg)
{
  callmsg = wtbconv_uic_hdr_HTON(callmsg, (UIC_Header *)call_t_msg);
  *callmsg = call_t_msg->ld_direc;
}

/*
 * UWTM State Request
 */
void HTON_uic_fr_read_uwtm_state(UIC_TR_READ_UWTM_STATE *reply_t_msg, UNSIGNED8 *replymsg)
{
  replymsg = wtbconv_uic_hdr_HTON(replymsg, (UIC_Header *)reply_t_msg);
  *(replymsg++) = reply_t_msg->uwtm_state;
  *(replymsg++) = reply_t_msg->master_conflict;
  *(replymsg++) = reply_t_msg->red_state;
  *(replymsg++) = reply_t_msg->inaug_reason;
  *(replymsg++) = reply_t_msg->inaug_result;
  *(replymsg++) = reply_t_msg->res1;
  replymsg     += HTON32(replymsg, reply_t_msg->rel_time);
  replymsg     += HTON32(replymsg, reply_t_msg->tcn_inaug_count);
  replymsg     += HTON32(replymsg, reply_t_msg->uic_inaug_count);
  replymsg     += HTON32(replymsg, reply_t_msg->uic_inaug_error_count);
  replymsg     += HTON32(replymsg, reply_t_msg->ll_inaug_count);
  replymsg     += HTON32(replymsg, reply_t_msg->cmd_inaug_count);
  replymsg     += HTON32(replymsg, reply_t_msg->e_count);
  replymsg     += HTON32(replymsg, reply_t_msg->res2);
  *(replymsg++) = reply_t_msg->WTB_hardware;
  *(replymsg++) = reply_t_msg->WTB_software;
  *(replymsg++) = reply_t_msg->hardware_state;
  *(replymsg++) = reply_t_msg->link_layer_state;
  *(replymsg++) = reply_t_msg->net_inhibit;
  *(replymsg++) = reply_t_msg->node_address;
  *(replymsg++) = reply_t_msg->node_orient;
  *(replymsg++) = reply_t_msg->node_strength;
  *(replymsg++) = reply_t_msg->node_frame_size;
  *(replymsg++) = reply_t_msg->node_period;
  *(replymsg++) = reply_t_msg->node_type;
  *(replymsg++) = reply_t_msg->node_version;
  *(replymsg++) = reply_t_msg->node_report;
  *(replymsg++) = reply_t_msg->user_report;
}

void NTOH_uic_fr_read_uwtm_state(UNSIGNED8 *replymsg, UIC_TR_READ_UWTM_STATE *reply_t_msg)
{
  replymsg = wtbconv_uic_hdr_NTOH(replymsg, (UIC_Header *)reply_t_msg);
  reply_t_msg->uwtm_state       = *replymsg++;
  reply_t_msg->master_conflict  = *replymsg++;
  reply_t_msg->red_state        = *replymsg++;
  reply_t_msg->inaug_reason     = *replymsg++;
  reply_t_msg->inaug_result     = *replymsg++;
  reply_t_msg->res1             = *replymsg++;
  reply_t_msg->rel_time         = NTOH32(replymsg);
  replymsg += sizeof(UNSIGNED32);
  reply_t_msg->tcn_inaug_count  = NTOH32(replymsg);
  replymsg += sizeof(UNSIGNED32);
  reply_t_msg->uic_inaug_count  = NTOH32(replymsg);
  replymsg += sizeof(UNSIGNED32);
  reply_t_msg->uic_inaug_error_count = NTOH32(replymsg);
  replymsg += sizeof(UNSIGNED32);
  reply_t_msg->ll_inaug_count   = NTOH32(replymsg);
  replymsg += sizeof(UNSIGNED32);
  reply_t_msg->cmd_inaug_count  = NTOH32(replymsg);
  replymsg += sizeof(UNSIGNED32);
  reply_t_msg->e_count          = NTOH32(replymsg);
  replymsg += sizeof(UNSIGNED32);
  reply_t_msg->res2             = NTOH32(replymsg);
  replymsg += sizeof(UNSIGNED32);
  reply_t_msg->WTB_hardware     = *replymsg++;
  reply_t_msg->WTB_software     = *replymsg++;
  reply_t_msg->hardware_state   = *replymsg++;
  reply_t_msg->link_layer_state = *replymsg++;
  reply_t_msg->net_inhibit      = *replymsg++;
  reply_t_msg->node_address     = *replymsg++;
  reply_t_msg->node_orient      = *replymsg++;
  reply_t_msg->node_strength    = *replymsg++;
  reply_t_msg->node_frame_size  = *replymsg++;
  reply_t_msg->node_period      = *replymsg++;
  reply_t_msg->node_type        = *replymsg++;
  reply_t_msg->node_version     = *replymsg++;
  reply_t_msg->node_report      = *replymsg++;
  reply_t_msg->user_report      = *replymsg++;
}

/*
 * UWTM Topography Request
 */

void HTON_uic_fr_read_uwtm_topography(UIC_TR_READ_UWTM_TOPOGRAPHY *reply_t_msg, UNSIGNED8 *replymsg)
{
  replymsg = wtbconv_uic_hdr_HTON(replymsg, (UIC_Header *)reply_t_msg);
#if 0 /* why is information not forwarded ? */
  *(replymsg++) = reply_t_msg->node_address;
  *(replymsg++) = reply_t_msg->node_orient;
  *(replymsg++) = reply_t_msg->individual_period;
  *(replymsg++) = reply_t_msg->is_strong;
#endif
  *(replymsg++) = reply_t_msg->topo_counter;
  *(replymsg++) = reply_t_msg->number_of_nodes;
  *(replymsg++) = reply_t_msg->bottom_address;
  *(replymsg++) = reply_t_msg->top_address;
  *(replymsg++) = reply_t_msg->uic_address;
  *(replymsg++) = reply_t_msg->res1;

}

void NTOH_uic_fr_read_uwtm_topography(UNSIGNED8 *replymsg, UIC_TR_READ_UWTM_TOPOGRAPHY *reply_t_msg)
{
  replymsg = wtbconv_uic_hdr_NTOH(replymsg, (UIC_Header *)reply_t_msg);
#if 0 /* why is information not forwarded ? */
  reply_t_msg->node_address      = (*replymsg++);
  reply_t_msg->node_orient       = (*replymsg++);
  reply_t_msg->individual_period = (*replymsg++);
  reply_t_msg->is_strong         = (*replymsg++);
#endif
  reply_t_msg->topo_counter      = (*replymsg++);
  reply_t_msg->number_of_nodes   = (*replymsg++);
  reply_t_msg->bottom_address    = (*replymsg++);
  reply_t_msg->top_address       = (*replymsg++);
  reply_t_msg->uic_address       = (*replymsg++);
  reply_t_msg->res1              = (*replymsg++);

}

#ifdef O_GATEWAY

/*************************************************************************/
/* function to move data from the call header into the reply header      */
/*************************************************************************/

void exchange_UIC_HEADER(UIC_Msg* r,  /* destination */
                         UIC_Msg* c)  /* source      */
{
    UNSIGNED8 src_veh = c->hdr.src_veh;

    r->hdr.owner     = c->hdr.owner;
    r->hdr.dest_veh  = c->hdr.src_veh;
    r->hdr.dest_fct  = c->hdr.src_fct;
    r->hdr.src_veh   = c->hdr.dest_veh;
    r->hdr.src_fct   = c->hdr.dest_fct;
    r->hdr.code      = (UNSIGNED16)(c->hdr.code + 0x0A00);
    r->hdr.reserved1 = c->hdr.reserved1;
    r->hdr.reserved2 = c->hdr.reserved2;
    if (src_veh <= UIC_INDIVIDUAL_ADDR_LIMIT)
    {
        r->hdr.dest_veh  = src_veh;
    }
    else
    {
        r->hdr.dest_veh = uwtm_my_uic_addr();
    }
    return;
}

#endif

/*************************************************************************/
/* function to convert correction telegram from WTB to host              */
/* -15.02  write correction, call                                        */
/*************************************************************************/
void wtbconv_15_02_NTOH (UNSIGNED8             *p_bytestream_src,
                         UIC_WriteCorrection   *pCorrection )
{
    UNSIGNED8 i, j;
    CorrectionData* p_CorrData;

    p_bytestream_src = wtbconv_uic_hdr_NTOH( p_bytestream_src, &pCorrection->hdr );
    pCorrection->num_veh = *p_bytestream_src++;
    p_CorrData = pCorrection->corr_data;

    /* for each vehicle */
    for( i = 0; i < pCorrection->num_veh; i++ )
    {
        for( j = 0; j < TFR_LEN_UIC_ID; j++) {
           p_CorrData[ i ].uic_id.uic_id[ j ] = *p_bytestream_src++;
        }
        p_CorrData[ i ].ctrl_info = *p_bytestream_src++;
        p_CorrData[ i ].veh_res_num = NTOH16( p_bytestream_src );
        p_bytestream_src += sizeof(UNSIGNED16);
    }

    return;
}


/*************************************************************************/
/* function to convert correction telegram from host to WTB              */
/* -15.02  write correction, call                                        */
/*************************************************************************/
void wtbconv_15_02_HTON (UNSIGNED8             *p_bytestream_dest,
                         UIC_WriteCorrection   *pCorrection )
{
    UNSIGNED8 i, j;
    CorrectionData* p_CorrData;

    p_bytestream_dest = wtbconv_uic_hdr_HTON( p_bytestream_dest, &pCorrection->hdr );
    *p_bytestream_dest++ = pCorrection->num_veh;
    p_CorrData = pCorrection->corr_data;

    /* for each vehicle */
    for( i = 0; i < pCorrection->num_veh; i++ )
    {
        for( j = 0; j < TFR_LEN_UIC_ID; j++) {
            *p_bytestream_dest++ = p_CorrData[ i ].uic_id.uic_id[ j ];
        }
        *p_bytestream_dest++ = p_CorrData[ i ].ctrl_info;
        HTON16( p_bytestream_dest, p_CorrData[ i ].veh_res_num );
        p_bytestream_dest += 2;
    }

    return;
}


/*************************************************************************/
/* function to convert correction telegram from WTB to host              */
/* -15.02A  write correction, reply                                      */
/*************************************************************************/
void wtbconv_15_02A_NTOH (UNSIGNED8  *p_bytestream_src,
                          UIC_Msg    *p_msg )
{
    wtbconv_uic_hdr_NTOH( p_bytestream_src, &p_msg->hdr );
    return;
}



/*************************************************************************/
/* function to convert correction telegram from host to WTB              */
/* -12.02A  write correction, reply                                      */
/*************************************************************************/
void wtbconv_15_02A_HTON (UNSIGNED8  *p_bytestream_dest,
                          UIC_Msg    *p_msg )
{
    wtbconv_uic_hdr_HTON( p_bytestream_dest, &p_msg->hdr );
    return;
}


/*************************************************************************/
/* function to convert vehicle res. num. from WTB to host                */
/* -15.03  WriteVehResNum, call                                          */
/*************************************************************************/
void wtbconv_15_03_NTOH (UNSIGNED8*       p_bytestream_src,
                         UIC_WriteResNum* p_veh_res_num )
{
    UNSIGNED8 i, j;
    VehResData* p_VehResData;

    p_bytestream_src = wtbconv_uic_hdr_NTOH( p_bytestream_src, &p_veh_res_num->hdr );
    p_veh_res_num->num_veh = *p_bytestream_src++;
    p_VehResData = p_veh_res_num->res_data;

    /* for each vehicle */
    for( i = 0; i < p_veh_res_num->num_veh; i++ )
    {
        for( j = 0; j < TFR_LEN_UIC_ID; j++) {
            p_VehResData[ i ].uic_id[ j ]  = *p_bytestream_src++;
        }
        p_VehResData [i].reserved     = *p_bytestream_src++;
        p_VehResData[ i ].veh_res_num = NTOH16( p_bytestream_src );
        p_bytestream_src += sizeof(UNSIGNED16);
    }

    return;
}


/*************************************************************************/
/* function to convert vehicle res. num. from host to WTB                */
/* -15.03  WriteVehResNum, call                                          */
/*************************************************************************/
void wtbconv_15_03_HTON (UNSIGNED8*       p_bytestream_dest,
                         UIC_WriteResNum* p_veh_res_num )
{
    UNSIGNED8 i, j;
    VehResData* p_VehResData;

    p_bytestream_dest = wtbconv_uic_hdr_HTON( p_bytestream_dest, &p_veh_res_num->hdr );
    *p_bytestream_dest++ = p_veh_res_num->num_veh;
    p_VehResData = p_veh_res_num->res_data;

    /* for each vehicle */
    for( i = 0; i < p_veh_res_num->num_veh; i++ )
    {
        for( j = 0; j < TFR_LEN_UIC_ID; j++) {
            *p_bytestream_dest++ = p_VehResData[ i ].uic_id[ j ];
        }
        (*p_bytestream_dest++)   = p_VehResData [i].reserved;
        HTON16( p_bytestream_dest, p_VehResData [i].veh_res_num);
        p_bytestream_dest += 2;
    }

    return;
}


/*************************************************************************/
/* function to convert vehicle res. num. from WTB to host                */
/* -15.02A  WriteVehResNum, reply                                        */
/*************************************************************************/
void wtbconv_15_03A_NTOH (UNSIGNED8  *p_bytestream_src,
                          UIC_Msg    *p_msg )
{
    wtbconv_uic_hdr_NTOH( p_bytestream_src, &p_msg->hdr );
    return;
}


/*************************************************************************/
/* function to convert vehicle res. num. from host to WTB                */
/* -12.02A  WriteVehResNum, reply                                        */
/*************************************************************************/
void wtbconv_15_03A_HTON (UNSIGNED8  *p_bytestream_dest,
                          UIC_Msg    *p_msg )
{
    wtbconv_uic_hdr_HTON( p_bytestream_dest, &p_msg->hdr );
    return;
}


/*
 *
 * Multicast create
 *
 */

/*************************************************************************/
/* function converts UIC telegram                                        */
/* -15.07  Multicast create, call                                        */
/*************************************************************************/

void wtbconv_15_07__HTON (UNSIGNED8*        p_bytestream_dest,
                          UIC_MCreateRequ*  p_MCreateRequ_src)
{
    UNSIGNED8 i,j;
    UIC_Group_short *p_group_src;
    CorrectionData  *p_CorrData;
    VehResData      *p_VehResData;

   /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_dest = wtbconv_uic_hdr_HTON(p_bytestream_dest, &p_MCreateRequ_src->hdr);

   /* Now insert the multicast control bytes */
   *(p_bytestream_dest++) = p_MCreateRequ_src->mc_ctrl.mc_destination_veh;
   *(p_bytestream_dest++) = p_MCreateRequ_src->mc_ctrl.mc_destination_fct;
   *(p_bytestream_dest++) = p_MCreateRequ_src->mc_ctrl.mc_service;
   *(p_bytestream_dest++) = p_MCreateRequ_src->mc_ctrl.mc_retry_counter;
   p_bytestream_dest     += HTON16(p_bytestream_dest, p_MCreateRequ_src->mc_ctrl.mc_code);
   *(p_bytestream_dest++) = p_MCreateRequ_src->mc_ctrl.mc_status;
   *(p_bytestream_dest++) = p_MCreateRequ_src->mc_ctrl.mc_priority;

   /* Now insert the multicast data bytes                  */
   /* p_bytestream_dest points to the first free data byte */

   switch (p_MCreateRequ_src->mc_ctrl.mc_code)
   {
   case UIC_FC_WRITE_CORRECTION:
       if (p_MCreateRequ_src->mc_ctrl.mc_status == 0)
       {
           /* number of vehicles */
           *(p_bytestream_dest++) = p_MCreateRequ_src->mc_msg_data.correction_data.num_veh;

           /* pointer to first correction entry */
           p_CorrData = p_MCreateRequ_src->mc_msg_data.correction_data.corr_data;

           /* for each vehicle */
           for( i = 0; i < p_MCreateRequ_src->mc_msg_data.correction_data.num_veh; i++ )
           {
               for( j = 0; j < TFR_LEN_UIC_ID; j++) {
                   *(p_bytestream_dest++) = p_CorrData[ i ].uic_id.uic_id[ j ];
               }
               (*p_bytestream_dest++) = p_CorrData[ i ].ctrl_info;
               HTON16( p_bytestream_dest, p_CorrData[ i ].veh_res_num );
               p_bytestream_dest += 2;
           }
       }
       break;
   case UIC_FC_WRITE_VEH_RESNUM:

       p_VehResData = p_MCreateRequ_src->mc_msg_data.res_num_data.res_data;
       *(p_bytestream_dest++) = p_MCreateRequ_src->mc_msg_data.res_num_data.num_veh;

       /* for each vehicle */
       for( i = 0; i < p_MCreateRequ_src->mc_msg_data.res_num_data.num_veh; i++ )
       {
           for( j = 0; j < TFR_LEN_UIC_ID; j++) {
                *(p_bytestream_dest++) = p_VehResData[ i ].uic_id[ j ];
           }
           *p_bytestream_dest++ = p_VehResData [i].reserved;
           HTON16( p_bytestream_dest, p_VehResData[ i ].veh_res_num );
           p_bytestream_dest += 2;
       }
       break;
   case UIC_FC_WRITE_GROUP:
        /* group descriptor for provider-request has to be converted   */
        p_bytestream_dest = wtbconv_group_descr_short_HTON(p_bytestream_dest,
                         &p_MCreateRequ_src->mc_msg_data.group);

        /* group descriptor for provider-request has to be converted   */
        p_bytestream_dest = wtbconv_group_veh_short_HTON(p_bytestream_dest,
                                p_MCreateRequ_src->mc_msg_data.group.GroupVeh,
                        p_MCreateRequ_src->mc_msg_data.group.GroupDescr.num_entries);
       break;

    case UIC_FC_WRITE_ALL_GROUPS:
        *(p_bytestream_dest++) = p_MCreateRequ_src->mc_msg_data.group_list.num_groups;
        *(p_bytestream_dest++) = p_MCreateRequ_src->mc_msg_data.group_list.reserved;

        /* loop all groups */
        for (i = 0; i< p_MCreateRequ_src->mc_msg_data.group_list.num_groups; i++) {
            p_group_src = &p_MCreateRequ_src->mc_msg_data.group_list.group[i];
            /* group descriptor for provider-request has to be converted   */
            p_bytestream_dest = wtbconv_group_descr_short_HTON(p_bytestream_dest, p_group_src);

            /* group descriptor for provider-request has to be converted   */
            p_bytestream_dest = wtbconv_group_veh_short_HTON(p_bytestream_dest,
                        p_group_src->GroupVeh, p_group_src->GroupDescr.num_entries);

        }
        break;
    case UIC_FC_DELETE_GROUP:
        *p_bytestream_dest = p_MCreateRequ_src->mc_msg_data.group_number;
        break;
    case UIC_FC_DELETE_ALL_GROUPS:
        break;
    case UIC_FC_SLEEP:
        break;
    default:
        /* These messages are not intended to be generated as a */
        /* multicast request from this host                     */
        return;
        break;
    }

    return;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* -15.07  Multicast create, call                                        */
/*************************************************************************/

void wtbconv_15_07__NTOH (UNSIGNED8*       p_bytestream_src,
                          UNSIGNED32       bytestream_src_len,
                          UIC_MCreateRequ* p_MCreateRequ_dest)
{
   UNSIGNED32 size_to_copy;

   /* UIC-NADI-header for provider-request has to be converted      */
   p_bytestream_src = wtbconv_uic_hdr_NTOH(p_bytestream_src, &p_MCreateRequ_dest->hdr);
   /* Now insert the multicast control bytes */
   p_MCreateRequ_dest->mc_ctrl.mc_destination_veh = *(p_bytestream_src++);
   p_MCreateRequ_dest->mc_ctrl.mc_destination_fct = *(p_bytestream_src++);
   p_MCreateRequ_dest->mc_ctrl.mc_service = *(p_bytestream_src++);
   p_MCreateRequ_dest->mc_ctrl.mc_retry_counter = *(p_bytestream_src++);
   p_MCreateRequ_dest->mc_ctrl.mc_code = NTOH16(p_bytestream_src);
   p_bytestream_src += sizeof(UNSIGNED16);
   p_MCreateRequ_dest->mc_ctrl.mc_status = *(p_bytestream_src++);
   p_MCreateRequ_dest->mc_ctrl.mc_priority = *(p_bytestream_src++);

   /* Now insert the multicast data bytes                 */
   /* p_bytestream_src points to the first data byte      */
   /* which is already as a bytestream in the WTB data    */
   /* format. This data has to be copied.                 */

   size_to_copy = bytestream_src_len - UIC_HEADER_BYTES - UIC_MCCREATE_CTRL_BYTES;

   memcpy (p_MCreateRequ_dest->mc_msg_data.mc_message, p_bytestream_src,
           (unsigned short)size_to_copy);

    return;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* -15.07A Multicast create, reply                                       */
/*************************************************************************/

void wtbconv_15_07A_HTON (UNSIGNED8*        p_bytestream_dest,
                          UIC_MCreateReply* p_MCreateReply_src)
{
    /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_dest   = wtbconv_uic_hdr_HTON(p_bytestream_dest, &p_MCreateReply_src->hdr);
    p_bytestream_dest += HTON16(p_bytestream_dest, p_MCreateReply_src->call_id);

    return;
}

/*************************************************************************/
/* function converts UIC telegram                                        */
/* -15.07A Multicast create, reply                                       */
/*************************************************************************/

void wtbconv_15_07A_NTOH (UNSIGNED8*        p_bytestream_src,
                          UIC_MCreateReply* p_MCreateReply_dest)
{
    /* UIC-NADI-header for provider-request has to be converted      */
    p_bytestream_src = wtbconv_uic_hdr_NTOH(p_bytestream_src, &p_MCreateReply_dest->hdr);
    p_MCreateReply_dest->call_id = NTOH16(p_bytestream_src);

    return;
}
