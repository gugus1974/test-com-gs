/*



*/
/* 
;=========================================================================
; copyright 1995 by ABB Henschel AG
;-------------------------------------------------------------------------
; project   : TRAIN COMMUNICATION NETWORK 
;-------------------------------------------------------------------------
; department: AHE/STL3   author: Becker,Mike              date: 27.01.1995
;-------------------------------------------------------------------------
; filename  : \TCN\MOD\NVM.20\SOURCE\NVM.H
;-------------------------------------------------------------------------
; function  : functions for non volatile memory
;=========================================================================
; 4.1 10.12.96 (24) additional recovery method
*/
#ifdef O_960
#define EEPROM_ADDRESS 0x40001F00ul
#endif
unsigned short restore_recovery_data(Type_SaveData * psd) ;
void save_recovery_data(Type_SaveData * psd) ;
void flush_recovery_data(void) ;
#ifdef O_NVM_VERSIONID   /*4.1-24*/
void init_recovery_data (void);
#endif
