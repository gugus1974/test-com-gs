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
*                 10.10.1994  Re     created
*
******************************************************************************/


/******************************************************************************
*
*   name:         madomain.c
*
*   function:     Serve Request PDU's for Domain Objects
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

#define ReadMemory			50
#define WriteMemory			51
#define DownloadSetup		52
#define DownloadSegment		53


#define MM_DNLD_PREPARE          0
#define MM_DNLD_CHECK_ONLY       1
#define MM_DNLD_START_ERASE      2
#define MM_DNLD_START_NOERASE    3
#define MM_DNLD_TERMINATE_BOOT   4
#define MM_DNLD_TERMINATE_NOBOOT 5
#define MM_DNLD_VERIFY           6

#define RAM         0
#define EEPROM      1
#define FLASH       2

static int ma_DownloadSetup(struct TNMPDU *InPDU)
{
    UNSIGNED8  i;
    UNSIGNED8  nb_domains;
    UNSIGNED8  download_timeout;
    UNSIGNED8  download_command;
    UNSIGNED32 base_address;
    UNSIGNED32 domain_size;

	nb_domains       = ma_decodeUNSIGNED8(&InPDU->Data[1]);
	download_timeout = ma_decodeUNSIGNED8(&InPDU->Data[2]);
    download_command = ma_decodeUNSIGNED8(&InPDU->Data[3]);

    for(i=0;i<nb_domains;i++) {
       base_address = ma_decodeUNSIGNED32(&InPDU->Data[(i*8)+4]);
       domain_size  = ma_decodeUNSIGNED32(&InPDU->Data[(i*8)+8]);

       /* boundary check */
        if ( domain_size > MAX_TNM_PDU)  {
             domain_size = MAX_TNM_PDU;
        }
        switch(download_command) {
         case MM_DNLD_PREPARE: {
         }; break;
         case MM_DNLD_CHECK_ONLY: {
         }; break;
         case MM_DNLD_START_ERASE: {
         }; break;
         case MM_DNLD_START_NOERASE: {
         }; break;
         case MM_DNLD_TERMINATE_BOOT: {
		 }; break;
         case MM_DNLD_TERMINATE_NOBOOT: {
         }; break;
         case MM_DNLD_VERIFY: {
         }; break;
         default: {
			return(MM_CMD_NOT_EXECUTED);
         }

       }

    }


    return(OK);
}



/* downloading on EEPROM or FLASH not supported at the moment */

static int ma_DownloadSegment(struct TNMPDU *InPDU)
{
    UNSIGNED32 i;
    UNSIGNED8  domain_ID;  /* for fun only */
    UNSIGNED8  segment_type;
    UNSIGNED32 base_address;
    UNSIGNED32 segment_size;

	 
    domain_ID     = ma_decodeUNSIGNED8(&InPDU->Data[0]);
    segment_type  = ma_decodeUNSIGNED8(&InPDU->Data[1]);
	base_address  = ma_decodeUNSIGNED32(&InPDU->Data[2]);
    segment_size  = ma_decodeUNSIGNED32(&InPDU->Data[6]);

    /* boundary check */
    if ( segment_size > MAX_TNM_PDU) {
         segment_size = MAX_TNM_PDU;
    }
    for(i=0;i<segment_size;i++) {
       switch(segment_type) {
         case RAM: {
			*(char *)base_address++ = InPDU->Data[i+10];
         }; break;
         case EEPROM: {
         }; break;
		 case FLASH: {
         }; break;
         default: {
			return(MM_CMD_NOT_EXECUTED);
         }

       }

    }


    return(OK);
}


int ma_DomainsObjects(struct TNMPDU *InPDU,struct TNMPDU * OutPDU,int* OutPDULen)
    {
    int  ReturnCode = OK;

	 switch (InPDU->SIF) {

      case ReadMemory:
      {

		 /* seems to be the most practible way to process the PDU herein */

        UNSIGNED8  nb_regions;
        int        i, j, k;
        int        l = 0;
		UNSIGNED32 base_address;
        UNSIGNED16 nb_items;
		UNSIGNED8  item_size;

        nb_regions = ma_decodeUNSIGNED8(&InPDU->Data[1]);

        for (i = 0; i < nb_regions; i++) {

           base_address = ma_decodeUNSIGNED32(&InPDU->Data[(i * 8) + 2]);
           nb_items     = ma_decodeUNSIGNED16(&InPDU->Data[(i * 8) + 6]);
           item_size    = ma_decodeUNSIGNED8 (&InPDU->Data[(i * 8) + 9]);
           switch(item_size) {
				  case 1: break; /* octet */
				  case 2: break; /* byte  */
				  case 4: break; /* word  */
				  default: {
					 return(MM_CMD_NOT_EXECUTED);
              }
           }

		   k = nb_items * item_size;

           /* boundary check */
           if (k > MAX_TNM_PDU) k = MAX_TNM_PDU;

      	   l += ma_encodeUNSIGNED16(&OutPDU->Data[l], k + (k & 1));

           for (j = 0; j < k; j++) OutPDU->Data[l++] = *(char *)base_address++;
           if (k & 1) OutPDU->Data[l++] = 0;
        }

        *OutPDULen = l + 2;

      };
		break;

      case WriteMemory:
      {

      /* seems to be the most practible way to process the PDU herein */

        UNSIGNED8 nb_regions;
        int i,j,k;
        int l = 0;
        UNSIGNED32 base_address;
        UNSIGNED16 nb_items;
        UNSIGNED8  item_size;

        nb_regions = ma_decodeUNSIGNED8(&InPDU->Data[1]);

        for(i=0;i<nb_regions;i++) {
           base_address = ma_decodeUNSIGNED32(&InPDU->Data[(i * 8) + 2]);
           nb_items     = ma_decodeUNSIGNED16(&InPDU->Data[(i * 8) + 6]);
           item_size    = ma_decodeUNSIGNED8 (&InPDU->Data[(i * 8) + 9]);
           switch(item_size) {
              case 1: break;
              case 2: break;
              case 4: break;
              default: {
					 return(MM_CMD_NOT_EXECUTED);
              }
           }

          /* boundary check */
           if ((nb_items * item_size) > MAX_TNM_PDU) {
                nb_items = MAX_TNM_PDU/item_size;
			  }

           for(j=0;j<nb_items;j++) {
              for(k=0;k<item_size;k++) {
              /* let's hope there is really some memory */
					 *(char *)base_address++ = InPDU->Data[((nb_items*8)+2)+(l++)];
				  }

           }

        }

      /* default response */
		};
		break;

      case DownloadSetup:
      {
         ReturnCode = ma_DownloadSetup(InPDU);
      /* default response */
      };
		break;

      case DownloadSegment:
      {
         ReturnCode = ma_DownloadSegment(InPDU);

        /* default response */
      };
		break;

      default: {
			return(MM_SIF_NOT_SUPPORTED);
      }
    }
    return(ReturnCode);
}
