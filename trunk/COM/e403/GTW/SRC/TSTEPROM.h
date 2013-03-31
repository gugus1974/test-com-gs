/*********************************************************************************
* 20/03/13                                             v.1.00        tsteeprom.h *
**********************************************************************************
* Funzioni per il test della EEPROM                                              *
**********************************************************************************
* 					                                                             *
*********************************************************************************/
#define WPEN_BIT    0x80 
#define BP1_BIT     0x08 
#define BP0_BIT     0x04 
#define WEL_BIT     0x02 
#define WIP_BIT     0x01 

#define TSTEPROM_OK     0x00 
#define TSTEPROM_WRKO   0x01 
#define TSTEPROM_RDKO   0x02 
#define TSTEPROM_VRFYKO 0x02 
       


unsigned char tstEEPROM(void );


