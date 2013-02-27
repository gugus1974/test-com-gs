/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> Defined system configuration for ATR boards                                         */
/* <ATR:02> Defined lcx_ptr_add_seg() for O_C167                                                */
/* <ATR:03> Workaround for C166 compiler bug                                                    */
/*==============================================================================================*/


/*
*****************************************************************************
*                                                                           *
*    Property of  :   ABB Verkehrssysteme AG, CH-8050 Z*rich / Switzerland  *
*    COPYRIGHT    :   (c) 1991 ABB Verkehrssysteme AG                       *
*                                                                           *
*****************************************************************************
*                                                                           *
*    PROJECT      : IEC TCN                                                 *
*                                                                           *
*    SUBPROJECT   : TCN Link-Layer Common  (LC)                             *
*                                                                           *
*****************************************************************************
*                                                                           *
*    MODULE       : LC_TGT.H                                                *
*                                                                           *
*    DOCUMENT     : TCN Link-Layer Common (LC) / Link-Layer Message (LM)    *
*                   Migration Document, 3EHT 420 015                        *
*                                                                           *
*                   TCN Link-Layer Common (LC) / Link-Layer Message (LM)    *
*                   User's Manual, 3EHL 420 332                             *
*                                                                           *
*                   MVBC ASIC Data Sheet                                    *
*                   Data Sheet, 3EHL 420 441                                *
*                                                                           *
*    ABSTRACT     : The files "lc_tgt.h" contain all HW-specific code.      *
*                   This file helps to avoid introducint hardware-dependent *
*		    code into the actual LC software.                       *
*                                                                           *
*                                                                           *
*    REMARKS      : !!! ATTENTION !!!                                       *
*									    *
*		    If you are compiling for a new target hardware, then    *
*		    you must introduce one new compiler switch, typically   *
*		    O_????? and add code to this file.                      *
*                                                                           *
*                   LC_TGT.* is used by at least LC and LM projects.        *
*                                                                           *
*                                                                           *
*    DEPENDENCIES : Programmer's Interface Library                          *
*                                                                           *
*    ACCEPTED     :                                                         *
*                                                                           *
*****************************************************************************
*                                                                           *
*    HISTORY:                                                               *
*                                                                           *
*    Version  YY-MM-DD  Name         Dept.    Ref   Comments                *
*    -------  --------  ----------   ------   ----  ---------------------   *
*    4.4.0.0  95-02-24  zur Bonsen   BATL      --   Created                 *
*    4.4.0.1  95-02-24  zur Bonsen   BATL      --   AIP uses Ch. B, not A   *
*    4.5.0.0  95-10-25  zur Bonsen   BATL      --   No changes in this file *
*    4.5.1.0  95-10-25  zur Bonsen   BATL      --   No changes in this file *
*    4.5.2.0  95-10-25  zur Bonsen   BATL      --   No changes in this file *
*    4.6.0.0  95-12-14  zur Bonsen   BATL      --   No changes in this file *
*                                                                           *
*****************************************************************************
*                                                                           *
*    4.4.0.0: First version.  Created for following target systems          *
*									    *
*    O_EKR    Einkartenrechner EKR II with MVBC-Test-Subprint               *
*    O_EVA    68360-based evaluation board - no MVBC, therefore illegal     *
*    O_AIP    AIP 500 made by ABB Relays AG, Baden                          *
*    O_VCH    Vehicle Control Family, made by ABB Transportation Systems    *
*									    *
*****************************************************************************
*/

#ifndef LC_TGT_H

    #define LC_TGT_H

    #include "mvbc.h"
    #include "lc_sys.h"



/*
******************************************************************************
* Hardware-Specific Aspects to take care of				     *
******************************************************************************
*                                                                            *
*   0.  Boolean Stuff							     *
*                                                                            *
*   1.  Access to Programmer's Interface Library                             *
*                                                                            *
*   2.  Access to Debugging System (Hamster)                                 *
*                                                                            *
*   3.  Traffic Memory Characteristics                                       *
*                                                                            *
*   4.  Byte Order (Endian Format)                                           *
*                                                                            *
*   5.  Address Arithmetics                                                  *
*                                                                            *
******************************************************************************
*/


/*
******************************************************************************
*   0.  Boolean Stuff							     *
******************************************************************************
*/

    #ifndef TRUE
        #define TRUE (0==0)
    #endif

    #ifndef FALSE
        #define FALSE (0!=0)
    #endif



/*
******************************************************************************
*   1.  Access to Programmer's Interface Library                             *
******************************************************************************
*/


/* As of now, same code applies to all target hardware */

#include "pi_sys.h"

#define lcx_lock_task()   { pi_lock_task();   }
#define lcx_unlock_task() { pi_unlock_task(); }

/* <ATR:03> */
#define lcx_clear(ptr,size) pi_zero16((void*)(ptr), (size) / 2)
/* #define lcx_clear(ptr,size)                  \ */
/*             {                                \ */
/*                 short ii  = (size);          \ */
/*                 short *pp = (short *) (ptr); \ */
/*                 while (ii-->0)               \ */
/*                 {                            \ */
/*                     *pp++ = 0;               \ */
/*                     ii--;                    \ */
/*                 }                            \ */
/*             }                                  */


/*
******************************************************************************
*   2.  Access to Debugging System (Hamster)                                 *
******************************************************************************
*/

    #ifdef O_LC_TESTING

        /* Fake Hamster */

        #include <stdio.h>

        #define LCX_POSITION              "!!! hamster(File=%s,Line=%d,Act=%s,NPar=%d 0x%04x 0x%04x 0x%04x)\n", __FILE__, __LINE__
        #define LCX_CONTINUE              "cont."
        #define LCX_HALT                  "halt"
        #define LCX_RESET                 "reset"
        #define LCX_SUSPEND               "susp."

        #define lcx_hamster               printf
        #define lcx_hamster_continue(i) { if (i) { lcx_hamster( LCX_POSITION, LCX_CONTINUE, 1,(i),0,0); } }
        #define lcx_hamster_halt(i)     { if (i) { lcx_hamster( LCX_POSITION, LCX_HALT,     1,(i),0,0); } }


    #else

	#ifdef O_TCN

	    /* IEC TCN Event Recorder */

	    #include "recorder.h"

            #define LCX_POSITION              __FILE__,__LINE__,NSE_EVENT_TYPE_ERROR
            #define LCX_CONTINUE              NSE_ACTION_CONTINUE
            #define LCX_HALT                  NSE_ACTION_HALT
            #define LCX_RESET                 NSE_ACTION_REBOOT
            #define LCX_SUSPEND               NSE_ACTION_SUSPEND


            #define lcx_hamster_continue(i) { if (i) { nse_record( LCX_POSITION, LCX_CONTINUE, 0,0); } }
            #define lcx_hamster_halt(i)     { if (i) { nse_record( LCX_POSITION, LCX_HALT,  0,0); } }

            #define lcx_hamster(abc,d,e,f,g,h) \
            {          nse_record(abc,d,0,(UINT8*)0) ; }
 	#else

            /* Real Hamster */

            #include "du_sys.h"

            #define LCX_POSITION              DU_POSITION
            #define LCX_CONTINUE              DU_CONTINUE
            #define LCX_HALT                  DU_HALT
            #define LCX_RESET                 DU_RESET
            #define LCX_SUSPEND               DU_SUSP

            #define lcx_hamster_continue(i) { hamster_continue(i); }
            #define lcx_hamster_halt(i)     { hamster_halt(i)    ; }
            #define lcx_hamster               hamster

	#endif

    #endif


/*
******************************************************************************
*   3.  Traffic Memory and Interrupt-Wireup Characteristics                  *
******************************************************************************
*/

/*
******************************************************************************
*                                                                            *
* LCX_TM_COUNT     =  { 1..LCX_CTRL_BLK_SIZE }                               *
*                       Defines number of traffic memories attachable to the *
*                       target system.  Usually, it's 1.                     *
*                                                                            *
* LCX_WAITSTATES   = { TM_SCR_WS_0, _1, _2, _3 }  TM Access waitstates       *
*                                                                            *
* LCX_TM_SIZE_CODE = { LC_MCM_16K, LC_MCM_32K,    Maximum TM size possible   *
*                      LC_MCM_64K, LC_MCM_256K,   on target HW.              *
*                      LC_MCM_1M }                                           *
*                                                                            *
*                                                                            *
* LCX_TM_SIZE is used to help to find the Service Area after a reset due     *
* to a SW problem, i.e. SW resets itself while the MVBC resides in MCM=2     *
* (64K).  In this case, the LL SW must find the Service Area at 16/32/64K    *
* in order to reset the MVBC properly.                                       *
* In addition, following checks are made when the MVBC is configured in      *
* order not to loose access to Service Area and therefore control of MVBC:   *
*                                                                            *
* Setting MCM to 32K         works only if size code is 32K or more.         *
* Setting MCM to 64K or more works only if size code is 64K or more.         *
*                                                                            *
* ALC_TARGET_TM_ST_ADDR = Start Address to Traffic Memory.                   *
*                                                                            *
* ALC_COMMON_INT        = { FALSE, TRUE }                                    *
*                                                                            *
*                         If FALSE: Assumed that both MVBC interrupt lines   *
*				    are separately led into 2 separate CPU   *
*				    interrupt inputs 			     *
*			  If TRUE:  Assumed that both MVBC interrupt lines   *
*				    are OR-gated and led into one CPU        *
*				    interrupt input			     *
*                                                                            *
* ALC_CHANNEL           = { LC_CH_A, LC_CH_B, LC_CH_BOTH }                   *
*                         See LC_SYS.H where one or both channels are chosen *
*                                                                            *
******************************************************************************
* !!! ATTENTION !!!                                                          *
*                                                                            *
* ALC_-Prefix in symbols mean that these symbols are not used in actual LC   *
* software.  It is more available as a help for users to install the LC into *
* a personal target system.						     *
*                                                                            *
******************************************************************************
* !!! ATTENTION !!!                                                          *
*                                                                            *
* The values for LCX_WAITSTATES, LCX_TM_SIZE_CODE and ALC_TARGET_TM_ST_ADDR  *
* must be specified inside {}-braces.  If LCX_TM_COUNT is a value other than *
* 1 (one), then the figures LCX_WAITSTATES, LCX_TM_SIZE_CODE and ALC-TARGET- *
* TM_ST_ADDR contain multiple values in order to describe every traffic      *
* memory properly.  Example:                                                 *
*                                                                            *
* #define LCX_TM_COUNT 3                                                     *
*                                                                            *
* #define LCX_WAITSTATES        { TM_SCR_WS_0, TM_SCR_WS_1, TM_SCR_WS_1 }    *
* #define LCX_TM_SIZE_CODE      { LC_MCM_256K, LC_MCM_64K , LC_MCM_64K  }    *
* #define ALC_TARGET_TM_ST_ADDR { 0xA0000000 , 0xB0000000 , 0xC0000000  }    *
* #define ALC_COMMON_INT        { TRUE       , FALSE      , FALSE       }    *
* #define ALC_CHANNELS          { LC_CH_BOTH , LC_CH_BOTH , LC_CH_A     }    *
*                                                                            *
******************************************************************************
*/

#if defined (O_EKR)

    #define LCX_TM_COUNT            1

    #define LCX_WAITSTATES        { TM_SCR_WS_0 }
    #define LCX_TM_SIZE_CODE      { LC_MCM_256K }
    #define ALC_TARGET_TM_ST_ADDR { 0x40000000L }
    #define ALC_COMMON_INT        { FALSE       }
    #define ALC_CHANNELS          { LC_CH_BOTH  }

#elif defined (O_AIP)

    #define LCX_TM_COUNT            1

    #define LCX_WAITSTATES        { TM_SCR_WS_3 }
    #define LCX_TM_SIZE_CODE      { LC_MCM_256K }
    #define ALC_TARGET_TM_ST_ADDR { 0x00d00000L }
    #define ALC_COMMON_INT        { FALSE       }
    #define ALC_CHANNELS          { LC_CH_B     }

#elif defined (O_VCH)

    #define LCX_TM_COUNT            1

    #define LCX_WAITSTATES        { TM_SCR_WS_1 }
    #define LCX_TM_SIZE_CODE      { LC_MCM_256K }
    #define ALC_TARGET_TM_ST_ADDR { 0x01000000L }
    #define ALC_COMMON_INT        { FALSE       }
    #define ALC_CHANNELS          { LC_CH_A     }

/* <ATR:01> */
#elif defined (_ATR_SYS_)

    #define LCX_TM_COUNT            TM_COUNT

    #define LCX_WAITSTATES        { TM0_WAITSTATES }
    #define LCX_TM_SIZE_CODE      { TM0_SIZE }
    #define ALC_TARGET_TM_ST_ADDR { TM0_BASE }
    #define ALC_COMMON_INT        { TM0_COMMON_INT }
    #define ALC_CHANNELS          { TM0_CHANNELS }


#else

    #error No other target hardware defined

#endif



/*
******************************************************************************
*   4.  Byte Order (Endian Format)                                           *
******************************************************************************
*/

/* O_LE must be defined for all Little-Endian-Computers                     */
/*      No compiler switch is needed for Big-Endian-Computers               */

#ifdef O_LE

    #define LCX_INTEL_MODE   1

#else

    #define LCX_INTEL_MODE   0

#endif



/*
******************************************************************************
*   5.  Address Arithmetics                                                  *
******************************************************************************
*                                                                            *
*  lcx_ptr_add_seg(ptr,ofs):						     *
*									     *
*      On Intel-80x86-based systems:					     *
*          The addition is restricted to segment part only.  The offset part *
*          is not affected.  Therefore, granularity is limited to 16 bytes or*
*          integral multiple of it. 					     *
*									     *
*      On other systems:						     *
*          Pure address addition takes place.				     *
*									     *
******************************************************************************
*/

#if defined (O_EKR) || defined (O_SEG) /* Special Case: Intel Processors */

    #if defined (O_C167)

/* <ATR:02> */
	#define lcx_ptr_add_seg(ptr,ofs)  ( (char *)((char xhuge *)ptr + ofs ))
    
    #else    

    #define lcx_ptr_add_seg(ptr,ofs)  (  (void *) (((unsigned long) (ptr)) \
            + ((((unsigned long) (ofs)) & 0xFFFF0l) << 12) )  )

	#endif

#else /* 'normal' (no Intel) microprocessors with linear addressing */

    #define lcx_ptr_add_seg(ptr,ofs)  (  (void *) (((unsigned long) (ptr)) \
            + (  (unsigned long) (ofs)) )  )

#endif


#endif /* LC_TGT_H */
