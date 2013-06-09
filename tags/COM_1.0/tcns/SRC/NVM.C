/*
NON VOLATILE MEMORY WTB V1.9
iMB, Mike Becker
  3. Oktober 1995,  9:15
*/
/*
ABB HEADER
*/
/* 
;=========================================================================
; copyright 1995 by ABB Henschel AG                                       
;-------------------------------------------------------------------------
; project   : TRAIN COMMUNICATION NETWORK
;-------------------------------------------------------------------------
; department: AHE/STL3        author:Becker,Mike          date: 27.01.1995
;-------------------------------------------------------------------------
; filename  : \TCN\MOD\NVM.20\SOURCE\NVM.C                                
;-------------------------------------------------------------------------
; function  : functions for non volatile                                  
;=========================================================================
*/
/*
NOTES
*/

/*
LIST OF CHANGES
*/
/*
;- nr.  0 -- author: Br          ----- date: 27.02.1995 -- version: 2.0  -
;  completed
;-------------------------------------------------------------------------
;- nr.  1 -- author: Br          ----- date: 31.10.1995 -- version: 2.0  -
;  member .ID of Type_SaveData changed to .MT_H__YP 
 18.10.96 cs 4.1 (24) addition of method O_NVM_EEPROM to verify SaveData
*/
/*
INCLUDES
*/
#ifndef COCO_H
#include "coco.h"
#endif
#include "tcntypes.h"   /*4.0*/
#include "bmi.h"
#include "ll.h"
#if defined(O_960) || defined(O_VCU6U_GW)
#include "pi_sys.h"
#else
#include "pil.h"
#endif
#ifdef O_NVM_EEPROM /*4.1-24*/
#include "hi_sys.h"
#include "du_sys.h"
#endif
/*
IMPLEMENTATION
*/
/*
defines
*/
#ifdef O_NVM_EEPROM  /*4.1-24*/
#define NVM_ID_FLUSHED    0xFFFF
#define NVM_VERS_CURRENT  0x1000
#define NVM_EEPROM_ADDR_CHKSUM 0x80   /* start of checksum      (word address !) */
#define NVM_EEPROM_ADDR_DATA   0x81   /* start of recovery data (word address !) */
#endif
/*
prototypes
*/
/* prototypes */
unsigned short calc_checksum(void) ;
#ifdef O_NVM_EEPROM /*4.1-24*/
void          nvm_task (void);
static UINT16 get_checksum (UINT16 *p_data, UINT16 word_count);
#endif
/*
global variables
*/
#ifndef O_960
Type_SaveData nvm_SaveData ;
#ifdef O_NVM_EEPROM /*4.1-24*/
Type_SaveData  nvm_tmpData;  /* used for nvm_task */
static short   nvm_sem_id = 0;   /* semaphore used for synchronization */
#endif
#endif
void flush_recovery_data(void)
{
  /*
  set ID to zero
  */
#ifdef O_960
  /*
  only for 80960
  */
  unsigned char * ptr2eeprom = (unsigned char *)EEPROM_ADDRESS;
  ptr2eeprom[0] = 0x00 ; /* EEPROM.SaveData.MT_H__YP = 0x00 */
  while ((ptr2eeprom[0] != 0x00) || (ptr2eeprom[1] != 0x00))
  {
    /* wait typically 3ms */
  }
  /**/
#else
#ifdef O_NVM_EEPROM  /*4.1-24*/
  /*
  4.1-24: version handling 
  */
  short result;
  nvm_SaveData.id = NVM_ID_FLUSHED;
  pi_post_semaphore(nvm_sem_id, &result);    /* inform nvm_task */
#else
  /*
  default
  */
  nvm_SaveData.MT_H__YP = 0x00 ; 
  nvm_SaveData.MT_L     = 0x00 ; /* #iMB */
#endif
#endif
}
unsigned short restore_recovery_data(Type_SaveData *psd)
{
  /*
  get NVM segment for recovering 
  */
#ifdef O_960
  /*
  JDP method
  */
  unsigned short * ptr2eeprom = (unsigned short *)EEPROM_ADDRESS ;
  if ( *ptr2eeprom == 0x0000 )
  {
    /* NVM has been flushed */
    return(1) ;
  }
  else
  {
    *psd = *((Type_SaveData*)ptr2eeprom) ; 
    /* NVM data is available */
    return(0) ;
  }
#else
#ifdef O_NVM_EEPROM /*4.1-24*/
  /*
  4.1-24: version handling
  */
  if (  (nvm_SaveData.id   == NVM_ID_FLUSHED)
     || (nvm_SaveData.vers != NVM_VERS_CURRENT) 
     )
  {
    return (1);/* recovery data flushed or version has changed */
  }
  else
  {
    /* NVM data is available */
    *psd = nvm_SaveData;
    return (0);
  }
#else
  /*
  default:
  */
  if ( (nvm_SaveData.MT_H__YP == 0x00) &&
       (nvm_SaveData.MT_L     == 0x00) ) /* #iMB */
  {
    /* NVM has been flushed */
    return(1) ;
  }
  else
  {
    if ( nvm_SaveData.checksum != calc_checksum() )
    {
      /* checksum invalid */
      return(1) ;
    }
    else
    {
      /* checksum valid */
      *psd = nvm_SaveData ;
      /* NVM data is available */
      return(0) ;
    }
  }
#endif
#endif
}
void save_recovery_data(Type_SaveData *psd)
{
  /*
  update NVM segment for recovering
  */
#ifdef O_960
  /*
  only for 80960
  */
  unsigned char * ptr2eeprom = (unsigned char *)EEPROM_ADDRESS ;
  unsigned short byte_cnt = 0 ;
  unsigned short i ;
  unsigned char try_again;
  while ( byte_cnt < sizeof(Type_SaveData) ) 
  {
    pi_disable();
    for (i=0; i<32 ;i++)
    {
      ptr2eeprom[i+byte_cnt] = ((unsigned char*)psd)[i+byte_cnt];
    }
    pi_enable();
    do{
      try_again = FALSE ;
      for (i=0; i<32 ; i++)
      {
        if (ptr2eeprom[i+byte_cnt] == ((unsigned char*)psd)[i+byte_cnt])
        {

        }
        else
        {
          try_again = TRUE ;
          break;
        }
      }
    }
    while (try_again);
    byte_cnt += 32;
  }
#else
#ifdef O_NVM_EEPROM  /*4.1-24*/
  /*
  4.1-24: version handling
  */
   pi_disable();
   nvm_SaveData = *psd ;
   nvm_SaveData.vers = NVM_VERS_CURRENT;
   nvm_SaveData.id++;
  if (nvm_SaveData.id == NVM_ID_FLUSHED)
  {
    nvm_SaveData.id = 0;
  }
  pi_enable();
  {
    short result;
    pi_post_semaphore(nvm_sem_id, &result);    /* inform nvm_task */
  }
#else
  /*
  default:
  */
  nvm_SaveData = *psd ;
  nvm_SaveData.checksum = calc_checksum() ; 
#endif
#endif
}
unsigned short calc_checksum(void)
{
  /*
  calculate checksum
  */
  unsigned short i, checksum ;
  unsigned char * chptr ;
  checksum = 0x0000 ;
  chptr = (unsigned char*)(&nvm_SaveData) ;
  for (i=0 ; i < (sizeof(nvm_SaveData)-2) ; i++)
  {
    checksum += chptr[i] ;
  }
  return checksum ;
}
#if defined (O_NVM_EEPROM)   /*4.1-24*/
/*
static UINT16 get_checksum (UINT16 *p_data, UINT16 word_count)
*/
static UINT16 get_checksum (UINT16 *p_data, UINT16 word_count)
{
  UINT32 chksum = 0;
  UINT16 i;
  for (i = 0; i < word_count; i++)
  {
    chksum+= *(p_data +i);
  }
  return ((unsigned short) chksum);
}
/*
void init_recovery_data (void)
*/
void init_recovery_data (void)
{
  UINT16 error1, error2, chksum, tmp_chksum;
  /* read recovery data from serial eeprom */
  error1 = hi_ser_eepr_read(HI_EEPR_INTERNAL,
                            (UINT16 *) &nvm_tmpData,
                            NVM_EEPROM_ADDR_DATA,
                            sizeof(nvm_tmpData) / 2);
  error2 = hi_ser_eepr_read(HI_EEPR_INTERNAL,
                            (UINT16 *) &chksum,
                            NVM_EEPROM_ADDR_CHKSUM,
                            sizeof(chksum) / 2);
  if ((error1 == HI_SER_OK) && (error2 == HI_SER_OK))
  {
    tmp_chksum = get_checksum((UINT16 *) &nvm_tmpData,
                              sizeof(nvm_tmpData) / 2);
    if (chksum == tmp_chksum)
    {
      nvm_SaveData = nvm_tmpData;
    }
    else
    {
      nvm_SaveData.id = NVM_ID_FLUSHED;
    }
  }
  else
  {
    /* some error occurred */
    du_hamster_text(DU_POSITION, DU_HALT,
           "hi_ser_eepr_read failed !!!");
  }
}
/*
void nvm_task (void)
*/
void nvm_task (void)
{
  short result;
  unsigned short chksum, error1, error2;
  nvm_sem_id = pi_create_semaphore(0, PI_ORD_FCFS, &result);
  if (result != PI_RET_OK)     /* initialization ok ? */
  {
    du_hamster_text(DU_POSITION, DU_HALT,
                    "pi_create_semaphore failed !!!");
  }
  while (TRUE)
  {
    pi_pend_semaphore(nvm_sem_id, PI_FOREVER, &result);
    if (result == PI_RET_OK)                /* new recovery data ? */
    {
      if (nvm_SaveData.id != nvm_tmpData.id)  /* to be saved ? */
      {
        pi_disable();
        nvm_tmpData = nvm_SaveData;
        pi_enable();
        if (nvm_tmpData.id == NVM_ID_FLUSHED) /* flush data */
        {
          chksum = 0;
          error1 = hi_ser_eepr_write(
             HI_EEPR_INTERNAL,
             (UINT16 *) &nvm_tmpData.id,
             NVM_EEPROM_ADDR_DATA,
             sizeof (nvm_tmpData.id) / 2);
          error2 = hi_ser_eepr_write(
             HI_EEPR_INTERNAL,
             (UINT16 *) &chksum,
             NVM_EEPROM_ADDR_CHKSUM,
             sizeof (chksum) / 2);
        }
        else
        {
          /* save all */
          chksum = get_checksum((UINT16 *) & 
             nvm_tmpData,
             sizeof(nvm_tmpData) / 2);
          error1 = hi_ser_eepr_write( 
             HI_EEPR_INTERNAL,
             (UINT16 *) &nvm_tmpData,
             NVM_EEPROM_ADDR_DATA,
             sizeof (nvm_tmpData) / 2);
          error2 = hi_ser_eepr_write(
             HI_EEPR_INTERNAL,
             (UINT16 *) &chksum,
             NVM_EEPROM_ADDR_CHKSUM,
             sizeof (chksum) / 2);
        }
        if ((error1 != HI_SER_OK) || (error2 != HI_SER_OK))
        {
          du_hamster_text(DU_POSITION, DU_HALT,
                          "hi_ser_eepr_write failed !!!");
        }
      }
    }
    else
    {
      /* PI_ERR_ID */
      du_hamster_text(DU_POSITION, DU_HALT,
      "semaphore ID not existing !!!");
    }
  }
  /* while */
}
#endif
