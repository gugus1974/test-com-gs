/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> Workarounds for MVBC "Last-Minute Interrupts" bug                                   */
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
*    SUBPROJECT   : TCN Link-Layer Common (LC)                              *
*                                                                           *
*****************************************************************************
*                                                                           *
*    MODULE       : LCINTR.C                                                *
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
*                   Interrupt Handler                                       *
*                                                                           *
*    REMARKS      :                                                         *
*                                                                           *
*    DEPENDENCIES : Link-Layer Common, Programmer's Interface Library       *
*                                                                           *
*    ACCEPTED     :                                                         *
*                                                                           *
*****************************************************************************
*                                                                           *
*    HISTORY:                                                               *
*                                                                           *
*    Version  YY-MM-DD  Name         Dept.    Ref   Comments                *
*    -------  --------  ----------   ------   ----  --------------------    *
*    4.1.0.0  94-01-20  zur Bonsen   BATC      --   Created                 *
*                                                   (100% written new)      *
*    4.1.0.1  (no changes in this file)                                     *
*    4.1.0.2  (no changes in this file)                                     *
*    4.2.0.0  94-11-11  zur Bonsen   BATC      --   see below               *
*    4.3.0.0  95-01-31  zur Bonsen   BATL      --   See below               *
*    4.4.0.0  95-02-24  zur Bonsen   BATL      --   Introduced lc_tgt.h     *
*    4.5.0.0  95-10-25  zur Bonsen   BATL      --   No changes in this file *
*    4.5.1.0  95-11-28  zur Bonsen   BATL      --   No changes in this file *
*    4.5.2.0  95-11-28  zur Bonsen   BATL      --   No changes in this file *
*    4.6.0.0  95-12-14  zur Bonsen   BAT:      --   No changes in this file *
*                                                                           *
*                                                                           *
*****************************************************************************
* Updates in release 4.2.0.0:                                               *
*                                                                           *
* Interrupt service routines have 1 paramter, no longer 2 parameters.       *
* Paramter "ts_id" will no longer apply.                                    *
*                                                                           *
*****************************************************************************
* Updates in release 4.3.0.0:                                               *
*                                                                           *
* Adaption to targets with MC 68360 Processors (Big Endian)                 *
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
*                                                                           *
* !!! Attention:                                                            *
* --------------                                                            *
*                                                                           *
* If MVBC INT0\ and INT1\ are connected to two separate interrupt inputs at *
* the CPU, so two different ISRs are called, then following functions must  *
* be called by the hardware-dependent interrupt handler:                    *
*                                                                           *
* _lc_intr_hdlr_0_k,  _lc_intr_hdlr_0_nk                                    *
* _lc_intr_hdlr_1_k,  _lc_intr_hdlr_1_nk                                    *
*                                                                           *
* If MVBC INT0\ and INT1\ are gated together to form a single interrupt     *
* line for the CPU, then the following functions must be called by the      *
* hardware-dependent interrupt handler instead:                             *
*                                                                           *
* _lc_intr_hdlr_b_k,  _lc_intr_hdlr_b_nk                                    *
*                                                                           *
*****************************************************************************
*/


/*
*****************************************************************************
* name          _lc_intr_hdlr_0_k                                          **
*                                                                           *
* param. in     ts_id            Traffic Store  Identification              *
*                                                                           *
* return value  -                                                           *
*****************************************************************************
* description   Services MVBC intr. line int0_n, kernel calls assumed inside*
*                                                                           *
* globals       lci_ctrl_blk[ts_id]                                         *
*****************************************************************************
* history       1   93-08-20   BG Created                                   *
*               2   94-11-11   call p_isrt: parameter ts_id removed         *
*****************************************************************************
*/

void _lc_intr_hdlr_0_k(unsigned short ts_id)
{
    TM_TYPE_INT_REGS *p_ir;         /* Ptr. to MVBC Internal Registers       */
    unsigned short    ivr0;         /* Local copy of IVR0 from MVBC          */
    unsigned short    ivr0_vec;     /* Contains IVR0 without IAV-bit         */
    LCI_TYPE_ISR_PTR  p_isrt;       /* Pointer to connected ISR              */
    unsigned short    param;        /* 2nd parameter, supplied by user       */

    p_ir = &(lci_ctrl_blk[ts_id].p_sa->int_regs);

	/* <ATR:01> */
	{ volatile unsigned short isr0 = p_ir->isr0.w; }

    ivr0 = p_ir->ivr0.w; /* Read 1st interrupt vector */

    while ((ivr0 & TM_IVR_IAV) != 0) //Indicates that at least one bit is set in the corresponding ISRi and that VEC3..0 is an
    {                                //applicable interrupt vector

        ivr0_vec = ivr0 & TM_IVR_MASK;

        p_isrt = lci_ctrl_blk[ts_id].p_isr     [ivr0_vec];
        param  = lci_ctrl_blk[ts_id].isr_param [ivr0_vec];

        if (p_isrt != NULL)
        {
            (*p_isrt)(param); /* Call ISR */
        }
        else
        {
            //lcx_hamster_continue(ivr0);
            /* $H. IVR0=0x%03x occurred while not connected */
        }
        ivr0 = p_ir->ivr0.w;
    }
    p_ir->ivr0.w = 0; /* Unfreeze Interrupt Controller */
}


/*
*****************************************************************************
* name          _lc_intr_hdlr_1_k                                          **
*                                                                           *
* param. in     ts_id            Traffic Store  Identification              *
*                                                                           *
* return value  -                                                           *
*****************************************************************************
* description   Services MVBC intr. line int1_n, kernel calls assumed inside*
*                                                                           *
* globals       lci_ctrl_blk[ts_id]                                         *
*****************************************************************************
* history       1   93-08-20   BG Created                                   *
*               2   94-11-11   call p_isrt: parameter ts_id removed         *
*****************************************************************************
*/


void _lc_intr_hdlr_1_k(unsigned short ts_id)
{
    TM_TYPE_INT_REGS *p_ir;         /* Ptr. to MVBC Internal Registers       */
    unsigned short    ivr1;         /* Local copy of IVR1 from MVBC          */
    unsigned short    ivr1_vec;     /* Contains IVR1 without IAV-bit         */
    unsigned short    isr1;         /* Interrupt Status Register fr. MVBC    */
    LCI_TYPE_ISR_PTR  p_isrt;       /* Pointer to connected ISR              */
    unsigned short    param;        /* 2nd parameter, supplied by user       */

    p_ir = &(lci_ctrl_blk[ts_id].p_sa->int_regs);

	/* <ATR:01> */
	{ volatile unsigned short isr1 = p_ir->isr1.w; }

    /* !!! Preferred Treatment of Timer Interrupt 1                   */
    /*     ****************************************                   */
    /*     Even though the MVBC treats Timer Interrupt 1 as a low     */
    /*     priority interrupt, software requirement make it necessary */
    /*     to treat INT_TIMER_1 before all other interrupts.          */


    #define LCI_INT_TIMER_1_MASK (1U << (LC_INT_TIMER_1 % LCI_ISR_PER_CTRLR))

    isr1 = (p_ir->isr1.w) & LCI_INT_TIMER_1_MASK;
    if (isr1 != 0)
    {
        p_isrt = lci_ctrl_blk[ts_id].p_isr     [LC_INT_TIMER_1];
        param  = lci_ctrl_blk[ts_id].isr_param [LC_INT_TIMER_1];

        if (p_isrt != NULL)
        {
            (*p_isrt)(param); /* Call ISR */
        }
        else
        {
            lcx_hamster_continue(isr1);
            /* $H. LCI_INT_TIMER_1 occurred while not connected */
        }
        p_ir->isr1.w = ~LCI_INT_TIMER_1_MASK; /* Acknowledge interrupt */
    }

    /* !!! End of Preferred Treatment, back to normal                 */
    /*     ******************************************                 */

    ivr1 = p_ir->ivr1.w; /* Read 1st interrupt vector */

    while ((ivr1 & TM_IVR_IAV) != 0)
    {
        ivr1_vec = (ivr1 & TM_IVR_MASK) + LCI_ISR_PER_CTRLR;

        p_isrt = lci_ctrl_blk[ts_id].p_isr     [ivr1_vec];
        param  = lci_ctrl_blk[ts_id].isr_param [ivr1_vec];

        if (p_isrt != NULL)
        {
            (*p_isrt)(param); /* Call ISR */
        }
        else
        {
            lcx_hamster_continue(ivr1);
            /* $H. IVR1=0x%03x occurred while not connected */
        }
        ivr1 = p_ir->ivr1.w; /* Read next interrupt vector */
    }

    p_ir->ivr1.w = 0; /* Unfreeze Interrupt Controller */
}


/*
*****************************************************************************
* name          _lc_intr_hdlr_b_k                                          **
*                                                                           *
* param. in     ts_id            Traffic Store  Identification              *
*                                                                           *
* return value  status           LC_OK or LC_REJECT or LC_K_CALLS           *
*****************************************************************************
* description   Services both MVBC intr. lines , kernel calls assumed inside*
*                                                                           *
* globals       lci_ctrl_blk[ts_id]                                         *
*****************************************************************************
* history       1   93-08-20   BG Created                                   *
*****************************************************************************
*/

void _lc_intr_hdlr_b_k(unsigned short ts_id)
{
    _lc_intr_hdlr_1_k(ts_id);
    _lc_intr_hdlr_0_k(ts_id);
}


/*
*****************************************************************************
* name          _lc_intr_hdlr_0_nk                                         **
*                                                                           *
* param. in     ts_id            Traffic Store  Identification              *
*                                                                           *
* return value  status           LC_OK or LC_REJECT or LC_K_CALLS           *
*****************************************************************************
* description   Services MVBC intr. line int0_n, no kernels assumed inside  *
*                                                                           *
* globals       lci_ctrl_blk[ts_id]                                         *
*****************************************************************************
* history       1   93-08-20   BG Created                                   *
*****************************************************************************
*/

short _lc_intr_hdlr_0_nk(unsigned short ts_id)
{
    short ret_val = LC_OK;
    TM_TYPE_INT_REGS *p_ir;

    p_ir = &(lci_ctrl_blk[ts_id].p_sa->int_regs);

	/* <ATR:01> */
	{ volatile unsigned short isr0 = p_ir->isr0.w; }

    if ((lci_ctrl_blk[ts_id].kernel_mask[0] & p_ir->isr0.w) != 0)
    {
        ret_val = LC_K_CALLS;
    }
    else
    {
        _lc_intr_hdlr_0_k(ts_id);
    }
    return (ret_val);
}


/*
*****************************************************************************
* name          _lc_intr_hdlr_1_nk                                         **
*                                                                           *
* param. in     ts_id            Traffic Store  Identification              *
*                                                                           *
* return value  status           LC_OK or LC_REJECT or LC_K_CALLS           *
*****************************************************************************
* description   Services MVBC intr. line int0_n, no kernels assumed inside  *
*                                                                           *
* globals       lci_ctrl_blk[ts_id]                                         *
*****************************************************************************
* history       1   93-08-20   BG Created                                   *
*****************************************************************************
*/

short _lc_intr_hdlr_1_nk(unsigned short ts_id)
{
    short ret_val = LC_OK;
    TM_TYPE_INT_REGS *p_ir;

    p_ir = &(lci_ctrl_blk[ts_id].p_sa->int_regs);

	/* <ATR:01> */
	{ volatile unsigned short isr1 = p_ir->isr1.w; }

    if ((lci_ctrl_blk[ts_id].kernel_mask[1] & p_ir->isr1.w) != 0)
    {
        ret_val = LC_K_CALLS;
    }
    else
    {
        _lc_intr_hdlr_1_k(ts_id);
    }
    return (ret_val);
}


/*
*****************************************************************************
* name          _lc_intr_hdlr_b_nk                                         **
*                                                                           *
* param. in     ts_id            Traffic Store  Identification              *
*                                                                           *
* return value  status           LC_OK or LC_REJECT or LC_K_CALLS           *
*****************************************************************************
* description   Services both MVBC intr. lines , no kernels assumed inside  *
*                                                                           *
* globals       lci_ctrl_blk[ts_id]                                         *
*****************************************************************************
* history       1   93-08-20   BG Created                                   *
*****************************************************************************
*/

short _lc_intr_hdlr_b_nk(unsigned short ts_id)
{
    short ret_val = LC_OK;
    TM_TYPE_INT_REGS *p_ir;

    p_ir = &(lci_ctrl_blk[ts_id].p_sa->int_regs);

	/* <ATR:01> */
	{ volatile unsigned short isr = p_ir->isr0.w | p_ir->isr1.w; }

    if ( ((lci_ctrl_blk[ts_id].kernel_mask[1] & p_ir->isr1.w) != 0) ||
         ((lci_ctrl_blk[ts_id].kernel_mask[0] & p_ir->isr0.w) != 0) )
    {
        ret_val = LC_K_CALLS;
    }
    else
    {
        _lc_intr_hdlr_1_k(ts_id);
        _lc_intr_hdlr_0_k(ts_id);
    }
    return (ret_val);
}

