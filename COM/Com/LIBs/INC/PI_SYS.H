/*=====================================================================================*/
/* PIL interface for ATR boards                                                        */
/* Header file (pi_sys.h)       													   */
/* 																					   */
/* Version 1.1																		   */
/*																					   */
/* (c)1998-1999 Ansaldo Trasporti													   */
/* Written by Giuseppe Carnevale													   */
/*=====================================================================================*/

#ifndef _PI_SYS_
#define _PI_SYS_

/* include the ATR definitions */
#include "atr_sys.h"

/* include the platform's operating system */
#if defined(O_C167)
#include "pi_rtx.h"
#elif defined(O_COM)
#include "pi_vrtx.h"
#elif defined(O_QNX)
#include "pi_qnx.h"
#else
#error Unknown operating system
#endif

/* some old code uses RMK_TICK instead of PI_TICK */
#define RMK_TICK PI_TICK

#endif

