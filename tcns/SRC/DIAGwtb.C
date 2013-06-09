/*
DIAGNOSTIC WTB V1.9
iMB, Mike Becker
  3. Oktober 1995,  9:12
*/
/*
document header
*/
/*****************************************************************
*   AAA   BBBB   BBBB   |                                        *
*  A   A  B   B  B   B  |           M o d u l k o p f            *
*  AAAAA  BBBB   BBBB   |                                        *
*  A   A  B   B  B   B  |                                        *
*  A   A  BBBB   BBBB   |      TRAIN COMMUNICATION NETWORK       *
*                       |                                        *
*  ASEA  BROWN  BOVERI  |      Copyright 1991 by ABB Henschel    *
*-----------------------+----------------------------------------*
*                                                                *
* Funktionalitaet: Diagnose- und Fehlerinstanz                   *
*                                                                *
* Erstellt am:     06.10.92                                      *
* Autor(en):       Gg                                            *
* Version:         1.1                                           *
* Freigegeben am:                                                *
* Abhdngikeiten:   ANSI C                                        *
*                                                                *
*----------------------------------------------------------------*
*                       Kurzbeschreibung                         *
*                                                                *
*  Diese Instanz sammelt Diagnose und Fehlermeldungen und spei-  *
*  chert diese inclusive Zeitstempel in einem Ringpuffer ab.     *
*  Jeder Eintrag hat eine Kennummer, an der die Meldung zu       *
*  identifizieren ist. Optional kann auf ein bestimmtes Ereignis *
*  getriggert werden.                                            *
*                                                                *
*                                                                *
*                                                                *
*----------------------------------------------------------------*
* Ablage:                DIAG.X (diamond X-Tools 2.0)            *
*----------------------------------------------------------------*
* Enthaltene Prozeduren: vgl. Headerfile DIAG.H                  *
*                                                                *
*                                                                *
*----------------------------------------------------------------*
* Gerufene Prozeduren:   vgl. Ausblendung ext. includes          *
*                                                                *
*                                                                *
*----------------------------------------------------------------*
* Benvtigten Files:      vgl. Ausblendung ext. includes          *
*                                                                *
*                                                                *
*----------------------------------------------------------------*
* Aenderungen:                                                   *
*                                                                *
* version   yy-mm-dd   name             dept.                    *
* -------   --------   ----             ----                     *
*   1.0     92-06-92    Gg            WAG/NTL2                   *
*   1.1     92-12-15    Gg            WAG/NTL2                   *
*           adaprion to new project structure & driver rev.E     *
*   1.1     94-03-01    Br            AHE/STL1                   *
*           headerfile without absolute pathnames                *
*           94-03-15    GH                                       *
*           conditional compiling for LED control                *
*   3.0     95-01-23    Br                                       *
*           adaption to WTB V1.9                                 *
*****************************************************************/

/* Identification */
#pragma title ("Internal Diagnosis GVPx xxxx xx Rxxxx")
/*#pragma mn(GVP0_0028_00_R0100)*/
/*
includes
*/
#ifndef COCO_H
#include "coco.h"
#endif
#include "tcntypes.h"    /*4.0*/
#include "bmi.h"
#include "ll.h"     /* TCN Link Layer Standard Headerfile */
#if defined(O_960) || defined(O_VCU6U_GW)
#include "pi_sys.h"
#else
#include "pil.h"
#endif
#include "wtbdrv.h" /* TCN WTB-driver */
#include "zas.h"    /* zentrale Ablaufsteuerung */
#include "diagwtb.h"
/*
LED function prototype
*/
#ifdef LED   /*4.1-20*/
void d_led_switch (unsigned char led_switch);
#define LED_ON  0x01
#define LED_OFF 0x00
#endif
/*
defines      (local)
*/
/* trigger action */
#define EVENT_WATCH_DOG   0x0001 /*  */
#define EVENT_LED_ON      0x0002 /*  */
#define EVENT_LED_OFF     0x0004 /*  */
#define EVENT_ZAS_HALT    0x0008 /*  */

#define TRIGGER_MATCHED   0xAFFE
/*
types        (local)
*/

/*
declarations (local)
*/
/* diagnostic buffer */
type_diag_buffer diag_buffer;
unsigned char unlimited;
/*
functions    (implementation)
*/
/*
basic initialization
*/
void diagwtb_sysinit (void)
{
  unsigned int i; /* loop-variable */
  /* initialize buffercontrol */
  diag_buffer.sz            = 0;
  diag_buffer.res           = 0;
  diag_buffer.trigger       = TRUE;
  diag_buffer.trigg_nr      = 0x0044; /* default on data enable */
  diag_buffer.trigg_par1    = 0;
  diag_buffer.trigg_occ     = 0x0001;
  diag_buffer.trigg_akti    = EVENT_LED_ON;
  diag_buffer.entry_counter = 0;

  unlimited = 0;
  /* initialize buffer-elements */
  for (i = 0 ;i < DIAG_BUFFER_SIZE ; i++)
  {
    diag_buffer.diag_buff_ele[i].diag_nr   = 0;
    diag_buffer.diag_buff_ele[i].diag_par1 = 0;
    diag_buffer.diag_buff_ele[i].diag_par2 = 0;
    diag_buffer.diag_buff_ele[i].diag_par3 = 0;
    diag_buffer.diag_buff_ele[i].diag_ptr  = NULL;
    diag_buffer.diag_buff_ele[i].sys_time  = 0;
  }
}
/*
diagnostic entry
*/
/* function for diagnostic-buffer entry  !!! REENTRANT !!! */
void diagwtb_entry ( unsigned short diag_nr,
                  unsigned short diag_par1,
                  unsigned short diag_par2,
                  unsigned short diag_par3,
                  void           *diag_ptr
                )
{
  unsigned short   sz; /* index for this entry */
  /* !!! entry in the critical region !!! */
  pi_disable ();
  sz = diag_buffer.sz;  /* store the actuall writing-pointer */
  /* increment the writing-pointer */
  diag_buffer.sz++;
  diag_buffer.sz %= DIAG_BUFFER_SIZE;

  /* increment the entry_counter */
  diag_buffer.entry_counter++;
  /* !!! leave the critical region !!! */
  pi_enable ();
  /* copy parameter into the diagnosticbuffer */
  diag_buffer.diag_buff_ele[sz].sys_time  = d_tc_get_systime();
  diag_buffer.diag_buff_ele[sz].diag_nr   = diag_nr;
  diag_buffer.diag_buff_ele[sz].diag_par1 = diag_par1;
  diag_buffer.diag_buff_ele[sz].diag_par2 = diag_par2;
  diag_buffer.diag_buff_ele[sz].diag_par3 = diag_par3;
  diag_buffer.diag_buff_ele[sz].diag_ptr  = diag_ptr;
  /*
  test for trigger event
  */
  /* trigger mode enabled */
  if (diag_buffer.trigger != FALSE)
  {
    /* trigger on the received diag event ? */
    if (diag_nr == diag_buffer.trigg_nr)
    {
      /* decrement occurence counter */
      diag_buffer.trigg_occ--;
      /* occurence counter == 0 ? */
      if (diag_buffer.trigg_occ == 0x0000)
      {
        diag_buffer.trigger = TRIGGER_MATCHED;
        if ( (diag_buffer.trigg_akti & EVENT_LED_ON) == EVENT_LED_ON)
        {
#if defined (LED)
           d_led_switch(LED_ON);
#endif
        }
        if ((diag_buffer.trigg_akti & EVENT_LED_OFF) == EVENT_LED_OFF)
        {
#if defined (LED)
          d_led_switch(LED_OFF);
#endif
        }
        if ((diag_buffer.trigg_akti & EVENT_ZAS_HALT) == EVENT_ZAS_HALT)
        {
          zas_halt();
        }
        if ((diag_buffer.trigg_akti & EVENT_WATCH_DOG) == EVENT_WATCH_DOG)
        {
          /* hang it up - the system */
          pi_disable();
          /* an never ending story ... until watchdog */
          while (TRUE)  /*4.1-20*/
          {
            unlimited++;
          }
        }
      }
    }
  }
}

