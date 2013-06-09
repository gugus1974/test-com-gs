/*=====================================================================================*/
/* Definitions for the compilation of TCN/JDP software                                 */
/* Header file (atr_sys.h)       													   */
/* 																					   */
/* Version 1.2																		   */
/*																					   */
/* (c)1996-1999 Ansaldo Trasporti													   */
/* Written by Giuseppe Carnevale													   */
/*=====================================================================================*/


#ifndef _ATR_SYS_
#define _ATR_SYS_


/*=====================================================================================*/
/* Basic constants */

#undef TRUE
#undef FALSE
enum {FALSE = 0, TRUE = 1};
#ifndef NULL
#define NULL 0L
#endif


/*=====================================================================================*/
/* Old AM net address included for compatibility */

typedef struct {
	char	vehicle;
	char	function;
	char	device;
} AM_NET_ADDR;


/*=====================================================================================*/
/* Hardware */

#if defined(__C166__)
#define O_C167				/* compile for STA/STR/DEV boards */
#define O_LE				/* little endian mode             */
#elif defined(_MRI)
#define O_COM				/* compile for COM board          */
#undef  O_LE				/* big endian mode                */
#elif defined(__QNX__)
#define O_QNX				/* compile for PC-QNX boards      */
#define O_LE				/* little endian mode             */
#else
#error System unknown
#endif

#define O_MVBC				/* MVBC hw: needed only by WTB LL; default in other modules */


/*=====================================================================================*/
/* Bus configuration */

/*-------------------------------------------------------------------------------------*/
/* Connections */

#define O_TCN

#ifdef ATR_WTB
#define NM_GW_SIMPLE  1
#else
#define NM_1VB_SIMPLE 1
#endif

/* NM_1VB_SIMPLE : there is just 1 VB and no TB, simple routing     */
/* NM_1VB        :   "    "  "   "  "  "  "  " , no simple routing  */
/* NM_1TB        : there is 1 TB and no VB, same as simple routing  */
/* NM_GW_SIMPLE  : there is 1 VB and 1 TB, simple routing           */
/* NM_GW_NO_TB   : there are several VBs but no TB, no simple rout. */


/*-------------------------------------------------------------------------------------*/
/* Traffic store ids */

#define MVB_TS_ID		0
#ifdef ATR_WTB
#define WTB_TS_ID		1
#endif


/*-------------------------------------------------------------------------------------*/
/* Traffic memory configuration */

#define TM_COUNT		1
#define TM0_ARBITRATION TM_SCR_ARB_0	/* do not use 3! */
#define TM0_WAITSTATES	TM_SCR_WS_1
#define TM0_SIZE		LC_MCM_256K
#define TM0_BASE		0xC00000L
#define TM0_COMMON_INT	FALSE
#define TM0_CHANNELS	LC_CH_BOTH
#define TM0_PIT			(TM0_BASE + 0x000000)
#define TM0_PCS			(TM0_BASE + 0x030000)
#define TM0_PRT			(TM0_BASE + 0x010000)
#define TM0_FRC			(TM0_BASE + 0x020000)
#define TM0_QO			LC_0_TO_256K
#define TM0_MO			LC_0_TO_256K
#define TM0_XQ0_SPACE	{(void*)(TM0_BASE + 0x03C000), 0, 112}
#define TM0_XQ1_SPACE	{(void*)(TM0_BASE + 0x03D000), 0, 112}
#define TM0_RQ_SPACE	{(void*)(TM0_BASE + 0x03E000), 0, 224}


/*-------------------------------------------------------------------------------------*/
/* WTB configuration */

#ifdef ATR_WTB
#define O_GATEWAY			/* declare we are a gateway            */
#define O_BMI_PIL_QUEUE		/* the WTB link layer uses a PIL queue */
#define WAG					/* enable long packets in the WTB      */
#endif


/*=====================================================================================*/
/* Special functions */

/*-------------------------------------------------------------------------------------*/
/* This section enables the transport layer messages dump */

#ifndef TRACE_ENB
#define TRACE_ENB			/* TM layer trace enabling */
#endif
#define get_binary(x) (0)
#ifndef ATR_WTB
#define nm_status_ind(x)
#endif


/*-------------------------------------------------------------------------------------*/
/* This section enables the MVB Bus Administrator LED blinking (where applicable) */

#if (defined(BAL_LINK_C) || defined(BAM_MAIN_C))

/* signal that we support the EKR functions */
#include "bah_hwre.h"
#undef bah_ekr_func
#define bah_ekr_func(param) param

/* delete the standard definition of the used functions */
#include "bai_ifce.h"
#undef bah_ekr2_waitstates
#undef bai_hi_led_bm_act_int_ext_di
#undef bai_hi_led_bm_pas_int_ext_di

/* provide our definition */
#include "atr_hw.h"
#define bah_ekr2_waitstates()
#define bai_hi_led_bm_act_int_ext_di() if (mvb_ba_led_handler) mvb_ba_led_handler(1); else
#define bai_hi_led_bm_pas_int_ext_di() if (mvb_ba_led_handler) mvb_ba_led_handler(0); else

#endif


/*=====================================================================================*/
/* Debugging */

/*-------------------------------------------------------------------------------------*/
/* Define the following macros to use printf() output instead of nse_record() in case of
   error */

/* #define O_LC_TESTING	*/	/* use printf for errors in LC */
/* #define O_LM_TESTING	*/	/* use printf for errors in LM */
/* #define O_PD_TEST	*/	/* use printf for errors in PD */
/* #define O_MD_TEST	*/	/* use printf for errors in MD */


/*=====================================================================================*/

#endif

