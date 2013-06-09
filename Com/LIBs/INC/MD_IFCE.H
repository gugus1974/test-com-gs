/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> Included "atr_sys.h"                                                                */
/* <ATR:02> Casted the strings in hamster_continue() and hamster_halt() to (UINT8*)             */
/*==============================================================================================*/

/* <ATR:01> */
#include "atr_sys.h"


/*
*****************************************************************************
*                                                                           *
*    Property of  :   ABB Verkehrssysteme AG, CH-8050 Z*rich / Switzerland  *
*    COPYRIGHT    :   (c) 1995 ABB Verkehrssysteme AG                       *
*                                                                           *
*****************************************************************************
*                                                                           *
*    PROJECT      : IEC TCN                                                 *
*                                                                           *
*    SUBPROJECT   : TCN Message Data (MD)                                   *
*                                                                           *
*****************************************************************************
*                                                                           *
*    MODULE       : MD_IFCE.H                                               *
*                                                                           *
*    DOCUMENT     :                                                         *
*                                                                           *
*    ABSTRACT     : The file "md_ifce.h" contains the handling of hamster   *
*                   calls. It can be chosen between printf(hamster is mapped*
*                   on printf), nse_record(TCN handling) and normal hamster *                                                        *
*                                                                           *
*    REMARKS      : !!! ATTENTION !!!                                       *
*		    If you want a Test Version -> set O_MD_TEST switch      *
*                   If you want a TCN Version -> set O_TCN switch           *
*                   If you want a normal Version -> set none                *
*    DEPENDENCIES :                                                         *
*                                                                           *
*    ACCEPTED     :                                                         *
*                                                                           *
*****************************************************************************
*                                                                           *
*    HISTORY:                                                               *
*                                                                           *
*    Version  YY-MM-DD  Name         Dept.    Ref   Comments                *
*    -------  --------  ----------   ------   ----  ---------------------   *
*    0.0      95-02-22  Flum         BATL      --   Created                 *
*    0.1      95-06-09  Leeb         CRBC      --   __FILE__ replaced by    *
*                                                   DU_MODULE_NAME,         *
*                                                   adapted to new          *
*                                                   nse_record(...)         *
*                                                                           *
*****************************************************************************/

#ifndef MD_IFCE_H

    #define MD_IFCE_H

/*
******************************************************************************
*                      Hamster_handling                                      *
******************************************************************************
*/

    #ifdef O_MD_TEST

        /* Fake Hamster */

        #include <stdio.h>

	#define DU_POSITION              "!!! hamster(File=%s,Line=%d,Act=%s,NPar=%d 0x%04x 0x%04x 0x%04x)\n", __FILE__, __LINE__
	#define DU_CONTINUE              "cont."
	#define DU_HALT                  "halt"
	#define DU_RESET                 "reset"
	#define DU_SUSPEND               "susp."

	#define hamster                 printf
	#define hamster_continue(i) { if (i) { hamster( DU_POSITION, DU_CONTINUE, 1,(i),0,0); } }
	#define hamster_halt(i)     { if (i) { hamster( DU_POSITION, DU_HALT,     1,(i),0,0); } }


    #else

	#ifdef O_TCN

	    /* IEC TCN Event Recorder */

		 #include "recorder.h"

		 #define DU_POSITION              DU_MODULE_NAME,__LINE__,NSE_EVENT_TYPE_ERROR
		 #define DU_CONTINUE              NSE_ACTION_CONTINUE
		 #define DU_HALT                  NSE_ACTION_HALT
		 #define DU_RESET                 NSE_ACTION_REBOOT
		 #define DU_SUSPEND               NSE_ACTION_SUSPEND

/* <ATR:02> */
		 #define hamster_continue(i) { if (i) { nse_record( DU_POSITION, DU_CONTINUE, 13,(UINT8*)"Hamster_cont"); } }
		 #define hamster_halt(i)     { if (i) { nse_record( DU_POSITION, DU_HALT,     13,(UINT8*)"Hamster_halt"); } }
		 #define hamster               nse_record


	#else

            /* Real Hamster */

            #include "du_sys.h"
      #endif
  #endif

#endif /* LC_TGT_H */


