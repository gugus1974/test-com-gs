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
*    MODULE       : LC_WAIT.C                                               *
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
*    ABSTRACT     : Link-Layer Common Module                                *
*                   Real Time Wait Functions (uses MVBC timer and is        *
*                   therefore independent from OS or non-MVBC hardware      *
*                   resources.  Time units: Microseconds.                   *
*                                                                           *
*    REMARKS      :                                                         *
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
*    4.1.0.1  94-05-26  zur Bonsen   BATC      --   created                 *
*                                                   (100% written new)      *
*    4.1.0.2  (no changes in this file)                                     *
*    4.2.0.0  (no changes in this file)                                     *
*    4.3.0.0  95-01-31  zur Bonsen   BATL      --   See below               *
*    4.4.0.0  95-02-24  zur Bonsen   BATL      --   Introduced lc_tgt.h     *
*    4.5.0.0  95-10-25  zur Bonsen   BATL      --   No changes in this file *
*    4.5.1.0  95-11-28  zur Bonsen   BATL      --   No changes in this file *
*    4.5.2.0  95-11-28  zur Bonsen   BATL      --   No changes in this file *
*    4.6.0.0  95-12-14  zur Bonsen   BAT:      --   No changes in this file *
*                                                                           *
*                                                                           *
*****************************************************************************
* Updates in release 4.3.0.0:                                               *
*                                                                           *
* Non-ASCII characters removed for use on non-PC systems                    *
*                                                                           *
*****************************************************************************
*/

#include "lc_sys.h"    /* Upper Interface Hdr      */
#include "mvbc.h"      /* MVBC-related header      */
#include "lc_tgt.h"    /* Hardware-dependent items */
#include "lc_head.h"   /* External prototypes      */


/*
*****************************************************************************
* name          _lc_wait ()                                                 *
*                                                                           *
* param. in     timer            Number of microseconds to wait             *
*                                                                           *
* return value  -                                                           *
*****************************************************************************
* description   System waits for (at least) specified number of microseconds*
*               The minimum time is guaranteed.  The time may be greater    *
*               which depends on CPU instruction execution overhead and     *
*               CPU clock speed, or OS behavior (task switching)            *
*                                                                           *
*               Waiting is realized with the MVBC timer 2 function.         *
*                                                                           *
*               A system halt occurs if the MVBC cannot be initialized.     *
*                                                                           *
*               Conditions: lc_configure() has been called successfully and *
*               MVBC Universal Timer #2 must not be used by any other SW    *
*                                                                           *
* attention !!! SAFE LIMIT: 1/2 TIME RANGE = 4 MILLISECONDS !!!             *
*                                                                           *
* globals       -                                                           *
*****************************************************************************
* history       1   93-08-20   BG Created                                   *
*****************************************************************************
*/

void _lc_wait (unsigned short timer)
{
    TM_TYPE_INT_REGS  *ir;

    #define LCI_FFFF  0xFFFF

    /* No const: Convert microseconds to Timer 2 units */ 

    timer = LCI_FFFF-(timer<<3); /* !!! Numbers used for better readability */

    ir = &(lci_ctrl_blk[0].p_sa->int_regs);


    /* Start stopwatch */

    ir->tr2    = LCI_FFFF;
    ir->tc2    = LCI_FFFF;
    ir->tcr.w |= TM_TCR_TA2;

    /* Wait */

    while (ir->tc2 > timer);

    /* Stop stopwatch */

    ir->tcr.w &= ~TM_TCR_TA2;

}
