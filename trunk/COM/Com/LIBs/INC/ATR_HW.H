/********************************************************************/
/* 2004/nov/26,ven 08:30                \SWISV\LBATR\SRC\atr_hw.h   */
/********************************************************************/
/*              ATR boards hardware routines                        */
/********************************************************************/
/*
    $Date: 2006/11/17 13:08:34 $          $Revision: 1.2.2.2.2.1.2.1 $
    $Author: schiavor $
*/


#ifndef _ATR_HW_
#define _ATR_HW_


/*==========================================================================*/
/* Debug pin support (different for each board) */

/*--------------------------------------------------------------------------*/
/* STA-STR */

#ifdef __C166__
/*
#ifndef _REG167_H_
#define _REG167_H_
#include "reg167.h"
#endif
*/

#include "cpudrv.h"

// sbit debug_direction_pin = DP3^9;    /* debug pin direction bit */
// sbit debug_pin = P3^9;               /* debug pin bit */

// /* set the debug pin to a boolean value */
// #define debug_pin_on() debug_pin = 1
// #define debug_pin_off() debug_pin = 0

#define debug_pin_on()
#define debug_pin_off()

#endif


/*--------------------------------------------------------------------------*/
/* COM */

#ifdef _MRI

#define debug_pin_on()
#define debug_pin_off()

#endif


/*==========================================================================*/
/* Constants */

/* board codes */
#define STA01_CODE  2213101
#define STA04_CODE  2213104
#define STA_CODE    22131
#define STR_CODE    22351
#define STD_CODE    2235100
#define COM_CODE    22528
#define DEV_CODE    30071
#define IDUDEV_CODE 30074
#define DEW_CODE    30073
#define DEVB_CODE   30072
#define PC_CODE     8086
#define DEA_CODE    60413


/*==========================================================================*/
/* Typedefs */

typedef void (*tcn_event_handler)(void);
typedef void (*bus_error_handler)(long address);

typedef void (*led_handler)(int status);


/*==========================================================================*/
/* Globals */

extern volatile unsigned short  laco_counter;       /* MVBD LACO rising edge counter */
extern volatile unsigned short  lbco_counter;       /* MVBD LBCO rising edge counter */
extern volatile unsigned short  lajo_counter;       /* MVBD LAJO both edges counter  */
extern volatile unsigned short  lbjo_counter;       /* MVBD LBJO both edges counter  */
extern volatile unsigned short  shd_counter;        /* MVBD SHD both edges counter   */

extern tcn_event_handler        laco_handler;       /* MVBD LACO rising edge handler */
extern tcn_event_handler        lbco_handler;       /* MVBD LBCO rising edge handler */
extern tcn_event_handler        lajo_handler;       /* MVBD LAJO both edges handler  */
extern tcn_event_handler        lbjo_handler;       /* MVBD LBJO both edges handler  */
extern tcn_event_handler        shd_handler;        /* MVBD SHD both edges handler   */

extern led_handler              mvb_ba_led_handler; /* MVB BA led blinker            */

extern volatile unsigned short  vme_berr_counter;   /* VME bus error counter         */
extern bus_error_handler        vme_berr_handler;   /* VME bus error handler         */

extern const unsigned long      atr_hw_code;        /* board type                    */
extern const char               *atr_hw_name;       /* board name                    */
extern const unsigned long      atr_cpu_clock;      /* board clock speed in Hz       */


/*==========================================================================*/
/* Prototypes */

/*--------------------------------------------------------------------------*/
/* Provided functions */

void hw_init(unsigned long baud_rate);
short hw_install_timeouts(void);
short hw_install_lc_interrupts(void);

short get_mvb_connection(short ts_id, short line);
void set_mvb_connection(short ts_id, short line, short value);

void hw_reset(void);
void hw_halt(void);
void hw_nmi(void);
void hw_watchdog_service(void);

void hw_reset2rom(void);
void hw_reboot(void *entry_point, long param);
void hw_bootstrap_loader(void);

void hw_ram2rom(void);
unsigned short hw_rom_crc(void);

short hw_ext_data_init(const void **data, long *size);
short hw_ext_data_open(char erase_flag);
short hw_ext_data_write(void *data, unsigned short data_len);
short hw_ext_data_close(void);


/*--------------------------------------------------------------------------*/

#endif


/* ****************************************************************************
    $Log: atr_hw.h,v $
    Revision 1.2.2.2.2.1.2.1  2006/11/17 13:08:34  schiavor
    17/11/2006             Napoli                      Schiavor
     -creata nuovo scheda IDUDEV

    Revision 1.2.2.2.2.1  2005/09/09 09:53:14  schiavor
    Si gestisce scheda DEW

    14/07/05                  Napoli                       SchiavoR
    si agg. DEW_CODE
    Revision 1.2.2.2  2005/03/01 14:55:24  schiavor
    01/03/05                  Napoli                       SchiavoR
    sostituito include reg167.h con cpudrv.h

    Revision 1.2.2.1  2005/02/10 09:55:47  schiavor

        09/02/2005               Napoli                     SchiavoR
        si inserisce catenaccio su include reg167.h

    Revision 1.2  2004/11/26 09:38:02  mungiguerrav

    2004/nov/26,ven 08:30           Napoli      Mungi
    - Si aggiungono keyword CVS e storia invertita
    - Si sostituiscono TAB con 4 spazi


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.1        2002/10/22 14:38:47     buscob
    - Inserimento in CVS
01.11   2001/giu/12,mar             Napoli              Accardo
        - Si agg. #define DEA_CODE   60413

 ------------------------   Prima di CVS    ------------------------
01.10   1999/lug/13,mar             Napoli              Carnevale
        - Modifiche non documentate
01.0n   199X/mmm/gg                 Napoli              Carnevale
        - Modifiche non documentate
01.00   1996/mmm/gg                 Napoli              Carnevale
        - Creazione non documentata
*/

