/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> Arbitration mode is now user-configurable using the TM0_ARBITRATION macro           */
/* <ATR:02> Initialize to 0 the unused fields in the MCR register                               */
/*==============================================================================================*/


/*
*****************************************************************************
*                                                                           *
*    Property of  :   ABB Verkehrssysteme AG, CH-8050 Zuerich / Switzerland *
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
*    MODULE       : LC_LAYER.C                                              *
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
*                   All functions except interrupt handler                  *
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
*    4.1.0.0  94-01-20  zur Bonsen   BATC      --   created                 *
*                                                   (100% written new)      *
*                                                                           *
*    4.1.0.1  (No changes in this file)                                     *
*    4.1.0.2  94-08-29  zur Bonsen   BATC      --   see updates below       *
*    4.2.0.0  94-11-11  zur Bonsen   BATC      --   see updates below       *
*    4.3.0.0  95-01-31  zur Bonsen   BATL      --   See below               *
*    4.4.0.0  95-02-24  zur Bonsen   BATL      --   See below               *
*    4.5.0.0  95-02-24  zur Bonsen   BATL      --   See below               *
*    4.5.1.0  95-02-24  zur Bonsen   BATL      --   no changes in this file *
*    4.5.2.0  95-11-28  zur Bonsen   BATL      --   LME structure update    *
*    4.6.0.0  95-12-14  zur Bonsen   BAT:      --   Device Address Fixup    *
*                                                                           *
*****************************************************************************
*                                                                           *
* Updates in 4.1.0.2:                                                       *
*                                                                           *
* Corrected initialization of la_pit_addr                                   *
*                                                                           *
*****************************************************************************
* Updates in release 4.2.0.0:                                               *
*                                                                           *
* lc_m_int_connect: Parameter 'function' is a fucntion containing 1 and     *
* no longer 2 input parameters.                                             *
*                                                                           *
*****************************************************************************
* Updates in release 4.3.0.0:                                               *
*                                                                           *
* Adaption to targets with MC 68360 Processors (Big Endian)                 *
* Non-ASCII characters removed for use on non-PC systems                    *
*                                                                           *
*****************************************************************************
* Updates in release 4.4.0.0:                                               *
*                                                                           *
* All hardware-dependent information is located in "lc_tgt.h".              *
* This file makes references to "lc_tgt.h" now.  Waitstate and memory       *
* size description is now made flexible to support multiple traffic stores. *
*                                                                           *
*****************************************************************************
* Updates in release 4.5.0.0:                                               *
*                                                                           *
* LME support implemented: lc_m_mvb_status and lc_m_mvb_line                *
*                                                                           *
*****************************************************************************
*/

#include <stdio.h>
#include "lc_sys.h"    /* Upper Interface Hdr      */
#include "mvbc.h"      /* MVBC-related header      */
#include "lc_tgt.h"    /* Hardware-dependent items */
#include "lc_head.h"   /* External prototypes      */


/*
*****************************************************************************
* Global Constants (not subject to be changed)				    *
*****************************************************************************
*/

const static unsigned short lci_waitstates  [LCX_TM_COUNT] = LCX_WAITSTATES;
const static unsigned short lci_tm_size_code[LCX_TM_COUNT] = LCX_TM_SIZE_CODE;

/* Offset constants for Service Area */

const static unsigned long  tm_sa_offs [] = TM_SERVICE_OFFSETS;

/*
*****************************************************************************
* Global Varibles: Traffic Store Control Block				    *
*****************************************************************************
*/

LCI_TYPE_CTRL_BLK lci_ctrl_blk[LC_CTRL_BLK_SIZE];


/*
*****************************************************************************
* name          lci_mvbc_init                                               *
*                                                                           *
* param. in     ts_id            Traffic Store  Identification              *
*                                                                           *
* return value  status           LC_OK or LC_REJECT                         *
*****************************************************************************
* description   Hard MVBC Configuration and Initialization                  *
*                                                                           *
*               A system halt occurs if the MVBC cannot be initialized.     *
*                                                                           *
*               This function is reentrant as long the same MVBC/TS is not  *
*               affected twice.                                             *
*                                                                           *
* globals       -                                                           *
*****************************************************************************
* history       1   93-08-20   BG Created                                   *
*****************************************************************************
*/
short lci_mvbc_init ( unsigned short ts_id );
short lci_mvbc_init ( unsigned short ts_id )
{
    short ret_val  = LC_OK;             /* Return Value, default: OK         */
    TM_TYPE_INT_REGS *p_ir = NULL;      /* Internal Registers                */
    TM_TYPE_SCR       local_scr;        /* Local copy of Status Control Reg. */
    signed short      i;                /* Loop Index                        */


    /*
    ******************************
    * Reset MVBC                 *
    ******************************
    */

    /* Assume MVBC is in unknown MCM, but within lci_tm_size_code[ts_id] */

    for (i = lci_tm_size_code[ts_id]; i >= LC_MCM_16K; i--)
    {
        lci_ctrl_blk[ts_id].p_sa = lcx_ptr_add_seg( lci_ctrl_blk[ts_id].p_tm, tm_sa_offs[i] );
        p_ir = &(lci_ctrl_blk[ts_id].p_sa->int_regs);
        p_ir->scr.w = 0;
    }

    /*
    ******************************
    * Status Control Register    *
    ******************************
    */

/* <ATR:01> */
    local_scr.w =                 /* Configure MVBC                          */

          TM_SCR_IL_CONFIG        /* Configuration Mode                      */
//        | TM_SCR_ARB_3            /* Arbitration Strategy = Mixed CPU/MVBC   */
		  | TM0_ARBITRATION			/* Arbitration Strategy set in configuration */
	| ((LCX_INTEL_MODE == FALSE) ? 0 : TM_SCR_IM )
			          /* Intel or Motorola Mode                  */
        | TM_SCR_RCEV             /* Event Polling always permitted          */
        | TM_SCR_TMO_43US         /* Default Timeout: 42.7 us                */
        | lci_waitstates[ts_id];  /* Waitstate Specification                 */

    p_ir->scr.w = local_scr.w;


    /*
    ******************************
    * Test if MVBC+SCR are OK    *
    ******************************
    */

    p_ir->dpr = LCI_ALL_1;        /* Write access to a MVBC-characteristic   */
                                  /* register to check that MVBC and not a   */
                                  /* memory is accessed. Use 14-bit reg. DPR */

    if ((p_ir->scr.w != local_scr.w) || (p_ir->dpr != TM_DPR_MASK))
    {
        printf("error LCI_ALL_1\n");
        ret_val = LC_REJECT;      /* MVBC not found */
    }

    /*
    ******************************
    * MVBC Interrupt Logic       *
    ******************************
    */

    /* The interrupt controller of the MVBC is already initialized by        */
    /* setting SCR to zero.  See code above                                  */


    return (ret_val);
}


/*
*****************************************************************************
* name          lci_port_init                                               *
*                                                                           *
* param. in     p_srv_area       Pointer to Service Area                    *
*                                                                           *
* return value  -                                                           *
*****************************************************************************
* description   Configures the Device Status Source Port (FC15).            *
*                                                                           *
*               This function is reentrant as long the same MVBC/TS is not  *
*               affected twice.                                             *
*                                                                           *
* globals       -                                                           *
*****************************************************************************
* history       1   93-08-20   BG Created                                   *
*****************************************************************************
*/
void lci_port_init ( TM_TYPE_SERVICE_AREA *p_srv_area );
void lci_port_init ( TM_TYPE_SERVICE_AREA *p_srv_area )
{
    /*
    ******************************
    * Initialize Physical Ports  *
    ******************************
    */

    lcx_clear( p_srv_area->pp_pcs , sizeof(p_srv_area->pp_pcs ) );
    lcx_clear( p_srv_area->pp_data, sizeof(p_srv_area->pp_data) );

    /* All physical ports are passive now */

    /*
    ******************************
    * Device Status Port    FC15 *
    ******************************
    */

    p_srv_area->pp_pcs[TM_PP_FC15].word0.w =

          W_FC15                  /* F-Code 15; Mandatory                    */
        | TM_PCS0_SRC             /* Active Source                           */
        | TM_PCS0_NUM;            /* Data is numeric                         */

    p_srv_area->pp_pcs[TM_PP_FC15].word1.w =

          TM_PCS1_VP0;            /* Addresses Data Area page 0 only         */

    tm_1_data_wd(p_srv_area->pp_data,TM_PP_FC15,TM_PAGE_0,0) =     /* Word 0 */

                                    /* Device Status Report: Initial Value   */

          LC_DSW_TYPE_SPECIAL_CLR   /* No special device                     */
        | LC_DSW_TYPE_BUS_ADMIN_CLR /* No bus administrator yet attached     */
        | LC_DSW_TYPE_BRIDGE_CLR    /* No bridge/gateway SW yet attached     */
        | LC_DSW_TYPE_CLASS_2_3_CLR /* No Link-Layer Mesasge yet attached    */
        | LC_DSW_LAA_SET            /* Line A Active                         */
        | LC_DSW_DNR_SET;           /* Device not (yet) ready                */
}


/*
*****************************************************************************
* name          lci_check_ts_valid                                          *
*                                                                           *
* param. in     ts_id            Traffic Store Identification               *
* return value  status           LC_OK or LC_REJECT                         *
*****************************************************************************
* description   Checks if ts_id is within specified boundaries and          *
*               that the TS is properly configured.  If both conditions     *
*               are met, then LC_OK is returnecd.                           *
*                                                                           *
*               This function is fully reentrant                            *
*                                                                           *
* globals       lci_ctrl_blk[ts_id]                                         *
*****************************************************************************
* history       1   93-08-20   BG Created                                   *
*****************************************************************************
*/

short lci_check_ts_valid(unsigned short ts_id)
{
    short ret_val  = (ts_id < LC_CTRL_BLK_SIZE) ? LC_OK : LC_REJECT;

    if (ret_val == LC_OK) /* New in V 4.3.1.0: checks against LCX_TM_COUNT, too */
    {
        ret_val = (ts_id < LCX_TM_COUNT) ? LC_OK : LC_REJECT;
    }

    if (ret_val == LC_OK)
    {
        ret_val = (lci_ctrl_blk[ts_id].checklist != LCI_CHK_INIT) ? LC_OK : LC_REJECT;
    }
    return (ret_val);
}



/*
*****************************************************************************
* name          _lc_init                                                    *
*                                                                           *
* param. in     none                                                        *
* return value  status           LC_OK always                               *
*****************************************************************************
* description   Initializes the LC Basic Structure                          *
*                                                                           *
*               Clears entire control block.  MVBCs and Traffic Stores are  *
*               not yet accessible.                                         *
*                                                                           *
*               This function is fully reentrant                            *
*                                                                           *
* globals       lci_ctrl_blk (all of)                                       *
*****************************************************************************
* history       1   93-08-20   BG Created                                   *
*****************************************************************************
*/

short _lc_init( void )
{
    lcx_clear( lci_ctrl_blk, sizeof( lci_ctrl_blk) );
    return ( LC_OK ); /* Always */
}


/*
*****************************************************************************
* name          _lc_m_hardw_config                                          *
*                                                                           *
* param. in     ts_id            Traffic Store  Identification              *
*               line_config      MVB Line Configuration                     *
*               treply_config    MVB Reply Timeout Configuration            *
*                                                                           *
*               This function is reentrant as long the same MVBC/TS is not  *
*               affected twice.                                             *
*                                                                           *
* return value  status           LC_OK or LC_REJECT                         *
*****************************************************************************
* description   Additional HW Configuration:                                *
*                                                                           *
*               line_config      = { LC_CH_A, LC_CH_B, LC_CH_BOTH }         *
*               treply_config    = { LC_TREPLY_21US, LC_TREPLY_43US,        *
*                                    LC_TREPLY_64US, LC_TREPLY_85US,        *
*                                    LC_TREPLY_UNCHANGED }                  *
*                                                                           *
*               Call is optional, not mandatory.  Default values are        *
*               LC_CH_BOTH and LC_REPLY_43US, active since MVBC reset       *
*                                                                           *
* globals       lci_ctrl_blk[ts_id]                                         *
*                                                                           *
*               _lc_m_config must be called before                          *
*                                                                           *
*****************************************************************************
* history       1   94-01-11   BG Created                                   *
*****************************************************************************
*/

short _lc_m_hardw_config( unsigned short  ts_id,
                          unsigned short  line_config,
                          unsigned short  treply_config )

{
    short ret_val  = LC_OK;             /* Return Value, default: OK         */

    TM_TYPE_INT_REGS     *p_ir;         /* MVBC Internal Registers           */
    TM_TYPE_SCR           local_scr;    /* Local copy of SCR                 */
    TM_TYPE_SCR           testm_scr;    /* Local copy of SCR, Test Mode set  */

    TM_TYPE_RWORD        *p_dr;         /* Pointer to Decoder Register       */

    #define LCI_LS_LIMIT 10             /* Timeout after trying to switch    */
                                        /* Line.  This is an arbitrary value */
    unsigned short limit = LCI_LS_LIMIT;/* big enough to allow enough time   */
                                        /* for switching, even for fast CPUs */

    unsigned short exp_laa;             /* Exp. value of LAA after swtiching */


    if ( lci_check_ts_valid(ts_id) == LC_OK )
    {
        p_ir        = &(lci_ctrl_blk[ts_id].p_sa->int_regs);
        local_scr.w = p_ir->scr.w;

        /*
        ******************************
        * Reconfigure Reply Time     *
        ******************************
        */

        if (treply_config != LC_TREPLY_UNCHANGED)
        {
            if (treply_config <= LC_TREPLY_85US)
            {
                local_scr.b.tmo = treply_config; /* 1:1 Mapping to MVBC          */
                p_ir->scr.w = local_scr.w;
            }
            else
            {
                ret_val = LC_REJECT;        /* Invalid value in treply_config    */
            }
        }

        /*
        ******************************
        * Select Line                *
        ******************************
        */

        /* Line Switching may be influenced by following factors:            */
        /* RTI or BTI by MVBC which leads to additional line switch or RLD   */
        /* Therefore, multiple attempts to perform a successful line switch  */
        /* and then freeze the decoder input may be necessary.               */

        p_dr           = ( TM_TYPE_RWORD *) &(p_ir->dr);
        testm_scr      = local_scr;
        testm_scr.b.il = TM_SCR_IL_TEST;

        if (line_config == LC_CH_BOTH)
        {
	    *p_dr = 0;              /* Deactivate SLM                    */
	}
	else
	{
	    exp_laa   =  ( line_config == LC_CH_A ) ? TM_DR_LAA : 0;

	    p_ir->scr.w = testm_scr.w;  /* Test Mode = quiet MVB         */
	    do
	    {
		*p_dr = TM_DR_LS;   /* Line Switch, then                 */
                *p_dr = TM_DR_SLM;  /* lock again                        */
            }
            while ( ((*p_dr & TM_DR_LAA) != exp_laa) && (limit-->0) );

            p_ir->scr.w = local_scr.w;  /* Restore original SCR  */
        }

        if (limit==0)               /* Problems with line switching      */
        {
            ret_val = LC_REJECT;    /* Unsuccessful switchover           */
            lcx_hamster_continue( LC_REJECT );
            /* $H. Unsuccessful swtichover at MVBC */
        }

	_lc_set_laa_rld();          /* Update changes in DSW             */

    } /* if (lci_check_ts_valid ... */
    else
    {
        ret_val = LC_REJECT;
    }

    return (ret_val);
}


/*
*****************************************************************************
* name          _lc_m_config                                                *
*                                                                           *
* param. in     ts_id            Traffic Store  Identification              *
*               p_tm_start_addr  Start Address to Traffic Memory            *
*               mcm              Memory Configuration Mode                  *
*               mo               Master-Frame Offset Value                  *
*               qo               Queue Offset Value                         *
*                                                                           *
*               This function is reentrant as long the same MVBC/TS is not  *
*               affected twice.                                             *
*                                                                           *
* return value  status           LC_OK or LC_REJECT                         *
*****************************************************************************
* description   Initializes the LC Basic Structure                          *
*                                                                           *
*               The function lc_config will be used to configure ONE Traffic*
*               store with all associated context information.              *
*                                                                           *
* globals       lci_ctrl_blk[ts_id]                                         *
*****************************************************************************
* history       1   93-08-20   BG Created                                   *
*****************************************************************************
*/

short _lc_m_config( unsigned short  ts_id,
                    void           *p_tm_start_addr, 
                    short           mcm,
                    short           qo,
                    short           mo)

{
    short ret_val  = LC_REJECT;         /* Return Value, default: Reject     */

    TM_TYPE_WORD   *p_la_pit;           /* LA Port Index Table               */
    TM_TYPE_WORD   *p_da_pit;           /* DA Port Index Table               */
    TM_TYPE_WORD   *p_pcs;              /* Port Control + Status Register    */

    unsigned short i;                   /* Index variable, used for loops    */

    TM_TYPE_SERVICE_AREA  *p_srv_area;  /* Local pointer to Service Area     */
    TM_TYPE_MCR            local_mcr;   /* Local copy of Memory Config. Reg. */

    TM_TYPE_INT_REGS       *p_ir;       /* Pointer to internal registers     */

    /* Offset values, specific to Traffic Memory structure */

    unsigned short dev_addr1, dev_addr2; /* Copy of device address */

    static const unsigned long  la_pcs_addr[] = TM_LA_PCS_OFFSETS;
    static const unsigned long  da_pcs_addr[] = TM_DA_PCS_OFFSETS;
    static const unsigned long  la_pit_addr[] = TM_LA_PIT_OFFSETS;
    static const unsigned long  da_pit_addr[] = TM_DA_PIT_OFFSETS;
    static const unsigned short pit_size   [] = TM_PIT_BYTE_SIZES;

    if (ts_id < LC_CTRL_BLK_SIZE)
    {
        /*
        ******************************
        * Initialize Control Block   *
        ******************************
        */

        lcx_clear( &(lci_ctrl_blk[ts_id]), sizeof(lci_ctrl_blk[ts_id]) );

        /* Then reinitialize the Control Block */

        lci_ctrl_blk[ts_id].p_tm      = p_tm_start_addr;
        lci_ctrl_blk[ts_id].checklist = LCI_CHK_LC;

        /*
        ******************************
        * Configure MVBC             *
        ******************************
        */

        ret_val   = lci_mvbc_init(ts_id); /* Hard Initialization */
        printf("MVBC 1: %d\n",ret_val);
        
        /* Check if MCM is valid according to maximum available TM size */

        if ( ((unsigned short) mcm) > lci_tm_size_code[ts_id])
        {
            ret_val = LC_REJECT;
        printf("MVBC 2: %d\n",ret_val);
        }

        if (ret_val == LC_OK)
        {

			/* <ATR:02> */
			local_mcr.w = 0;
        
			local_mcr.b.qo  = qo;
            local_mcr.b.mo  = mo;
            local_mcr.b.mcm = mcm;

            lci_ctrl_blk[ts_id].p_sa->int_regs.mcr.w = local_mcr.w;

            /*
            ******************************
            * Compute Serv. Area Address *
            ******************************
            */

            p_srv_area = lcx_ptr_add_seg( p_tm_start_addr, tm_sa_offs[mcm] );
            lci_ctrl_blk[ts_id].p_sa = p_srv_area;
    
            /*
            ******************************
            * Clear Port Index Tables    *
            ******************************
            */

            p_la_pit = lcx_ptr_add_seg(p_tm_start_addr,la_pit_addr[mcm]);
            p_da_pit = lcx_ptr_add_seg(p_tm_start_addr,da_pit_addr[mcm]);

            for (i = 0; i < lcx_half(pit_size[mcm]); i++)
            {
                *p_la_pit++ = 0;
                *p_da_pit++ = 0;

                /* Harmless action if MCM=0: p_la_pit = p_da_pit */
            }

            /*
            ******************************
            * Make default ports inactive*
            ******************************
            */

            /* Port Index 0 in the logical address space */

            p_pcs    = lcx_ptr_add_seg(p_tm_start_addr, la_pcs_addr[mcm]);
            *p_pcs++ = 0; /* delete LA PCS(Port 0), Word 0 */
            *p_pcs++ = 0; /* delete LA PCS(Port 0), Word 1 */
    
            /* Port Index 0 in the device  address space  */
            /* Harmless action if MCM=0: p_pcs = p_la_pit */

            p_pcs    = lcx_ptr_add_seg(p_tm_start_addr, da_pcs_addr[mcm]);
            *p_pcs++ = 0; /* delete DA PCS(Port 0), Word 0 */
            *p_pcs++ = 0; /* delete DA PCS(Port 0), Word 1 */

            /*
            ******************************
            * Initialize Physical Ports  *
            ******************************
            */

            lci_port_init( p_srv_area );

            /*
            ******************************
            * Latch Device Address In    *
            ******************************
            */

            p_ir = &(lci_ctrl_blk[ts_id].p_sa->int_regs);

	    /* Read device address.  Take into account 'noisy' target  */
	    /* systems where outside device addresses may contain      */
	    /* interferences.					       */

	    dev_addr1 = p_ir->daor;
	    do
	    {
		dev_addr2 = dev_addr1;
	        dev_addr1 = p_ir->daor;
	    }
	    while (dev_addr2 != dev_addr1);
	   
	    p_ir->daor = dev_addr1;
	    p_ir->daok = TM_DAOK_ENABLE;


        } /* if (ret_val... */
    }     /* if (ts_id...   */

    return (ret_val);
}


/*
*****************************************************************************
* name          _lc_m_int_connect                                           *
*                                                                           *
* param. in     ts_id            Traffic Store  Identification              *
*               function         Function containing ISR                    *
*               interrupt_nr     Interrupt Number                           *
*               parameter        2nd parameter passed to ISR.               *
*               option           Indication of kernel calls inside ISR      *
*                                                                           *
*               This function is fully reentrant.  Task lock mechanisms     *
*               avoid collisions at critical areas. Do not call from ISRs.  *
*                                                                           *
* return value  status           LC_OK or LC_REJECT                         *
*****************************************************************************
* description   Attaches ISR (function) to a specified MVBC interrupt       *
*                                                                           *
* globals       lci_ctrl_blk[ts_id]                                         *
*****************************************************************************
* history       1   93-08-20   BG Created                                   *
*               2   94-11-11   *function has only 1 parameter left          *
*****************************************************************************
*/

short _lc_m_int_connect ( unsigned short ts_id,
                          void           (* function)(unsigned short),
                          short          interrupt_nr,
                          unsigned short option,
                          unsigned short parameter )

{
    short ret_val  = LC_REJECT;         /* Return Value, default: Reject     */
    short i_c_nr;                       /* Interrupt Controller 0 or 1       */
    unsigned short i_mask;              /* Interrupt Mask Pattern            */

                                        /* Internal Registers                */
    TM_TYPE_INT_REGS *p_ir = &(lci_ctrl_blk[ts_id].p_sa->int_regs);


    if ( lci_check_ts_valid(ts_id) == LC_OK )
    {
        /* Check if interrupt number is valid */

        if ( ((unsigned short) interrupt_nr) < LCI_MAX_ISR )
        {
            /*
            ******************************
            * Interrupt Vacancy Check    *
            ******************************
            */

            lcx_lock_task(); /* No other task shall enter this function */

            if (lci_ctrl_blk[ts_id].p_isr[interrupt_nr] == NULL) 
            {

                /*
                ******************************
                * Connect ISR                *
                ******************************
                */

                lci_ctrl_blk[ts_id].p_isr    [interrupt_nr] = function ;
                lci_ctrl_blk[ts_id].isr_param[interrupt_nr] = parameter;

                i_c_nr = interrupt_nr      / LCI_ISR_PER_CTRLR ;
                i_mask = 1U<<(interrupt_nr % LCI_ISR_PER_CTRLR); /* 16 bit pattern */

                if (option == LC_KERNEL) /* Set Kernel Flag */
                {
                    lci_ctrl_blk[ts_id].kernel_mask[i_c_nr] |= i_mask;
                }

                /*
                ******************************
                * Enable Interrupt in MVBC   *
                ******************************
                */

                p_ir = &(lci_ctrl_blk[ts_id].p_sa->int_regs);

                if (i_c_nr == 0)
                {
                    p_ir->imr0.w |= i_mask;
                }
                else
                {
                    p_ir->imr1.w |= i_mask;
                }

                ret_val = LC_OK;

            } /* if (lci_ctrl_blk... */

            lcx_unlock_task(); /* Other tasks may enter now */

        } /* if ( ... interrupt_nr...   */
    }     /* if ( lci_check_ts_valid... */

    return (ret_val);
}


/*
*****************************************************************************
* name          _lc_m_int_disconnect                                        *
*                                                                           *
* param. in     ts_id            Traffic Store  Identification              *
*               interrupt_nr     Interrupt Number                           *
*                                                                           *
* return value  status           LC_OK or LC_REJECT                         *
*****************************************************************************
* description   Detaches ISR (function) from a specified MVBC interrupt     *
*                                                                           *
*               This function is fully reentrant.  Task lock mechanisms     *
*               avoid collisions at critical areas. Do not call from ISRs.  *
*                                                                           *
* globals       lci_ctrl_blk[ts_id]                                         *
*****************************************************************************
* history       1   93-08-20   BG Created                                   *
*****************************************************************************
*/

short _lc_m_int_disconnect ( unsigned short ts_id, short interrupt_nr )

{
    short ret_val  = LC_REJECT;         /* Return Value, default: Reject     */
    short i_c_nr;                       /* Interrupt Controller 0 or 1       */
    unsigned short i_mask_n;            /* Interrupt Mask Pattern, inverted  */

    #define LCI_IT_LIMIT 10             /* Short loop count which allows     */
                                        /* enough time to process last-minu- */
    unsigned short limit = LCI_IT_LIMIT;/* te interrupts correctly before    */
                                        /* interrupt masks have been disabled*/

                                        /* Internal Registers                */
    TM_TYPE_INT_REGS *p_ir = &(lci_ctrl_blk[ts_id].p_sa->int_regs);

    if ( lci_check_ts_valid(ts_id) == LC_OK )
    {
        /* Check if interrupt number is valid */

        if ( ((unsigned short) interrupt_nr) < LCI_MAX_ISR )
        {
            /*
            ******************************
            * Interrupt Connect Check    *
            ******************************
            */

            lcx_lock_task(); /* No other task shall enter this function */

            if (lci_ctrl_blk[ts_id].p_isr[interrupt_nr] != NULL)
            {

                /*
                ******************************
                * Disable Interrupt in MVBC  *
                ******************************
                */

                i_c_nr   = interrupt_nr / LCI_ISR_PER_CTRLR; /* Intr. Ctrl Nr. */
                i_mask_n = ~(((unsigned short) 1)<<(interrupt_nr % LCI_ISR_PER_CTRLR));

                p_ir = &(lci_ctrl_blk[ts_id].p_sa->int_regs);

                if (i_c_nr == 0)        /* Mask Interrupts in MVBC           */
                {
                    p_ir->imr0.w &= i_mask_n;
                }
                else
                {
                    p_ir->imr1.w &= i_mask_n;
                }

                while (limit-- >0);     /* Brief Delay to process last-      */
                                        /* minute interrupts correctly       */
                /*
                ******************************
                * Disconnect ISR             *
                ******************************
                */

                lci_ctrl_blk[ts_id].p_isr    [interrupt_nr]  = NULL;
                lci_ctrl_blk[ts_id].isr_param[interrupt_nr]  = 0;
                lci_ctrl_blk[ts_id].kernel_mask[i_c_nr]     &= i_mask_n;

                ret_val = LC_OK;

            } /* if (lci_ctrl_blk... */

            lcx_unlock_task(); /* Other tasks may enter now */

        } /* if ( ... interrupt_nr...   */
    }     /* if ( lci_check_ts_valid... */

    return (ret_val);
}

/*
*****************************************************************************
* name          _lc_m_set_device_address                                    *
*                                                                           *
* param. in     ts_id            Traffic Store  Identification              *
*               address          New Device Address                         *
*                                                                           *
* return value  status           LC_OK or LC_REJECT                         *
*****************************************************************************
* description   Applies new Device Address to MVBC                          *
*                                                                           *
*               Fully reentrant, but do not call from ISRs                  *
*                                                                           *
* globals       lci_ctrl_blk[ts_id]                                         *
*****************************************************************************
* history       1   93-08-20   BG Created                                   *
*****************************************************************************
*/

short _lc_m_set_device_address(unsigned short ts_id, unsigned short address)
{
    short ret_val  = LC_REJECT;         /* Return Value, default: Reject     */
    TM_TYPE_INT_REGS  *p_ir;            /* MVBC Internal Registers           */

    if ( lci_check_ts_valid(ts_id) == LC_OK )
    {
        if ((address & LCI_DA_MASK) == address)
        {
            p_ir        = &lci_ctrl_blk[ts_id].p_sa->int_regs;
            p_ir->daor  = address;
            p_ir->daok  = TM_DAOK_ENABLE;

            if (p_ir->daor == address)  /* Check if successful               */
            {
                ret_val = LC_OK;
            }
        }
    }
    return (ret_val);
}


/*
*****************************************************************************
* name          _lc_m_get_device_address                                    *
*                                                                           *
* param. in     ts_id            Traffic Store  Identification              *
*               address          Pointer to device address returned         *
*                                                                           *
*               Fully reentrant, but 1st call must not originate from ISRs  *
*                                                                           *
* return value  status           LC_OK or LC_REJECT                         *
*****************************************************************************
* description   Retrieves Device Address from MVBC                          *
*                                                                           *
* globals       lci_ctrl_blk[ts_id]                                         *
*****************************************************************************
* history       1   93-08-20   BG Created                                   *
*****************************************************************************
*/

short _lc_m_get_device_address(unsigned short ts_id, unsigned short *address)
{
    short ret_val  = lci_check_ts_valid(ts_id);
                                        /* Return Value, check for valid ts  */

    if ( ret_val == LC_OK )
    {
        *address = lci_ctrl_blk[ts_id].p_sa->int_regs.daor;
    }
    return (ret_val);
}


/*
*****************************************************************************
* name          _lc_m_get_device_status_word                                *
*                                                                           *
* param. in     ts_id            Traffic Store  Identification              *
*                                                                           *
* return value  Device Status Word                                          *
*****************************************************************************
* description   Obtains Device Status Word from Traffic Store Service Area  *
*                                                                           *
*               Fully reentrant                                             *
*                                                                           *
* globals       lci_ctrl_blk[ts_id]                                         *
*****************************************************************************
* history       1   93-08-20   BG Created                                   *
*****************************************************************************
*/

unsigned short _lc_m_get_device_status_word(unsigned short ts_id)
{
    unsigned short dsw = 0;             /* Device Status Word                */

    /* DSW is read from TM: Data Area of Phys. Port FC15, Page 0, Word 0 */

    if (lci_check_ts_valid(ts_id) == LC_OK)
    {
        dsw = tm_1_data_wd(lci_ctrl_blk[ts_id].p_sa->pp_data,TM_PP_FC15,TM_PAGE_0,0);
    }
    else
    {
        lcx_hamster( LCX_POSITION, LCX_CONTINUE, 1, ts_id, 0, 0 );
        /* $H. Invalid ts_id, ts_id = %d (decimal) */
    }
    return (dsw);
}


/*
*****************************************************************************
* name          _lc_m_set_device_status_word                                *
*                                                                           *
* param. in     ts_id            Traffic Store  Identification              *
*               mask             Dev. Status Word Mask Bits                 *
*               value            New values for affected fields             *
*                                                                           *
* return value  -                                                           *
*****************************************************************************
* description   Modifies Device Status Word from Traffic Store Service Area *
*                                                                           *
*               Fully reentrant as long not the same MVBC/TS is accessed    *
*                                                                           *
* globals       lci_ctrl_blk[ts_id]                                         *
*****************************************************************************
* history       1   93-08-20   BG Created                                   *
*****************************************************************************
*/

void _lc_m_set_device_status_word(unsigned short ts_id, unsigned short mask,
                                  unsigned short value)
{
    /* DSW accessed to TM: Data Area of Phys. Port FC15, Page 0, Word 0 */

    /* Data Area of Physical Ports */
    TM_TYPE_DATA *p_data = lci_ctrl_blk[ts_id].p_sa->pp_data;

    /* Pointer to Device Status Word */
    TM_TYPE_WORD *p_dsw = &tm_1_data_wd(p_data,TM_PP_FC15,TM_PAGE_0,0);

    if (lci_check_ts_valid(ts_id) == LC_OK)
    {
        *p_dsw = (*p_dsw & ~mask) | (value & mask);
    }
    else
    {
        lcx_hamster( LCX_POSITION, LCX_CONTINUE, 1, ts_id, 0, 0 );
        /* $H. Invalid ts_id, ts_id = %d (decimal) */
    }
}


/*
*****************************************************************************
* name          _lc_m_get_service_area_addr                                 *
*                                                                           *
* param. in     ts_id            Traffic Store  Identification              *
*               mask             Dev. Status Word Mask Bits                 *
*               value            New values for affected fields             *
*                                                                           *
* return value  -                                                           *
*****************************************************************************
* description   Modifies Device Status Word from Traffic Store Service Area *
*                                                                           *
*               Fully reentrant                                             *
*                                                                           *
* globals       lci_ctrl_blk[ts_id]                                         *
*****************************************************************************
* history       1   93-08-20   BG Created                                   *
*****************************************************************************
*/

void *_lc_m_get_service_area_addr(unsigned short ts_id)
{
    void *ret_val;

    if (lci_check_ts_valid(ts_id) == LC_OK)
    {
        ret_val = lci_ctrl_blk[ts_id].p_sa;
    }
    else
    {
        ret_val = NULL;
        lcx_hamster( LCX_POSITION, LCX_CONTINUE, 1, ts_id, 0, 0 );
        /* $H. Invalid ts_id, ts_id = %d (decimal) */
    }
    return (ret_val);
}


/*
*****************************************************************************
* name          _lc_set_laa_rld                                             *
*                                                                           *
* param. in     -                                                           *
*                                                                           *
* return value  -                                                           *
*****************************************************************************
* description   Retrieves LAA and RLD from MVBC Decoder Register and updates*
*               it in the Device Status Word.                               * 
*                                                                           *
* globals       lci_ctrl_blk (all)                                          *
*****************************************************************************
* history       1   93-08-20   BG Created                                   *
*****************************************************************************
*/

void _lc_set_laa_rld( void )
{
    short i;                            /* Loop Index                        */
    unsigned short dr;                  /* Contents of Decoder Register      */
    unsigned short laa;                 /* Line A Active                     */
    unsigned short rld;                 /* Redundant Line Disturbed          */

    for (i = 0; i < LC_CTRL_BLK_SIZE; i++) /* All control blocks */
    {
        if (lci_ctrl_blk[i].checklist != LCI_CHK_INIT)
        {
            dr   = lci_ctrl_blk[i].p_sa->int_regs.dr.w;

            laa  = (dr & TM_DR_LAA) != 0 ? LC_DSW_LAA_SET : LC_DSW_LAA_CLR;
            rld  = (dr & TM_DR_RLD) != 0 ? LC_DSW_RLD_SET : LC_DSW_RLD_CLR;

            _lc_m_set_device_status_word(
               i, LC_DSW_LAA_MSK | LC_DSW_RLD_MSK, laa | rld );

        }
    }
}


/*
*****************************************************************************
* name          _lc_exit                                                    *
*                                                                           *
* param. in     -                                                           *
*                                                                           *
* return value  -                                                           *
*****************************************************************************
* description   Exit function: Deactivates MVBC and interrupts and clears   *
*               control block. The _lc_m_config() can be used to reconfigure*
*               the MVBC and TS again.                                      *
*                                                                           *
* globals       lci_ctrl_blk (all)                                          *
*****************************************************************************
* history       1   93-08-20   BG Created                                   *
*****************************************************************************
*/

void _lc_exit( void )
{
    short i;                            /* Loop Index                        */

    lcx_lock_task();
    for (i = 0; i < LC_CTRL_BLK_SIZE; i++) /* All control blocks */
    {
        if (lci_ctrl_blk[i].checklist != LCI_CHK_INIT)
        {
            lci_ctrl_blk[i].p_sa->int_regs.scr.w = 0;
            /* Reset MVBC, it automatically initializes interrupt logic      */
        }
    }
    _lc_init();                         /* Clear all Control Blocks          */
    lcx_unlock_task();
}



/*
*****************************************************************************
* name          _lc_m_go                                                    *
*                                                                           *
* param. in     ts_id            Traffic Store  Identification              *
*                                                                           *
* return value  status           LC_OK or LC_REJECT                         *
*****************************************************************************
* description   Starts MVBC operation                                       *
*                                                                           *
*               Reentrant if calls are not made to the same MVBC/TS         *
*                                                                           *
* globals       lci_ctrl_blk[ts_id]                                         *
*****************************************************************************
* history       1   93-08-20   BG Created                                   *
*****************************************************************************
*/

short _lc_m_go( unsigned short ts_id )
{
    short ret_val  = lci_check_ts_valid(ts_id);
                                        /* Return Value, check for valid ts  */

    if (ret_val == LC_OK)               /* MVB Communication Kickoff !       */
    {

        /* New: Enable mechanism to attach 2 ISR's to EMF. */

        _lc_set_laa_rld( );             /* Valid LAA and RLD                 */

        lci_ctrl_blk[ts_id].p_sa->int_regs.scr.w |= TM_SCR_IL_RUNNING;
    }
    return (ret_val);
}


/*
*****************************************************************************
*                                                                           *
* Starting here:                                                            *
*                                                                           *
* L C - I N T E R F A C E   f o r   L M E   s u p p o r t                   *
*                                                                           *
* Applies to version 4.4.1.0 and higher.  All older versions do not con-    *
* tain 4.4.1.0.                                                             *
*                                                                           *
*****************************************************************************
*/

/*
*****************************************************************************
* name          _lc_read_mvb_status                                         *
*                                                                           *
* param. in     bus_id           Traffic Store  Identification              *
*               p_mvb_status     Pointer to location where status is written*
*                                                                           *
* param. out    *p_mvb_status    Retrieved status information               *
*                                                                           *
* return value  status           LC_OK or LC_REJECT                         *
*****************************************************************************
* description   Modifies Device Status Word from Traffic Store Service Area *
*                                                                           *
*               Fully reentrant as long not the same MVBC/TS is accessed    *
*                                                                           *
* globals       lci_ctrl_blk[bus_id] (eqv. ts_id)                           *
*****************************************************************************
* history       1   95-10-25   BG Created                                   *
*****************************************************************************
*/

int _lc_read_mvb_status( unsigned int bus_id,
                           struct LC_STR_MVB_STATUS *p_mvb_status )
{
    short ret_val  = LC_REJECT;         /* Return Value, default: Reject    */
    unsigned short rev_idx = 0;         /* Revision Index of MVBC           */
    unsigned short i       = 0;         /* Revision Loop index              */

    TM_TYPE_INT_REGS *p_ir = NULL;      /* Internal Registers               */

    static char hardware_id[LC_HW_ID_SIZE] =
        "MVBC** (ABB Verkehrssysteme AG)"; /* !!! Exactly 31 characters!    */

    static char software_id[LC_SW_ID_SIZE] = lc_get_version();

    static char t_reply[TM_SCRV_TMO_SIZE]   = TM_SCRV_TMO_US;

    TM_TYPE_DR  dr;    /* Local copy of DR and SCR */
    TM_TYPE_SCR scr;

    if ( lci_check_ts_valid((unsigned short) bus_id) == LC_OK )
    {
        p_ir        = &lci_ctrl_blk[bus_id].p_sa->int_regs;


        /* Report Hardware and Software ID */
        /* ------------------------------- */

        rev_idx  = tm_mcr_get_rev_idx(p_ir->mcr);

        /* The following two lines fill digits into the two '**' symbols */
        /* of the hw_id in decimal notations.  Numbers are used inten-   */
        /* tionally for better readability in this case!                 */

        hardware_id[4] = (rev_idx / 10)+'0'; /* first  digit on 1st '*'  */
        hardware_id[5] = (rev_idx % 10)+'0'; /* second digit on 2nd '*'  */

        for (i=0; i<LC_HW_ID_SIZE; i++)
        {
            p_mvb_status->hardware_id[i] = hardware_id[i];
        }
        for (i=0; i<LC_SW_ID_SIZE; i++)
        {
            p_mvb_status->software_id[i] = software_id[i];
        }


        /* Report Physical Address and Device Status */
        /* ----------------------------------------- */

        ret_val = _lc_m_get_device_address((unsigned short) bus_id, &(p_mvb_status->link_address));

        p_mvb_status->link_address |= bus_id << 12; /* !!! No Const - Upper 4 bits */

        p_mvb_status->device_status_word = _lc_m_get_device_status_word((unsigned short)bus_id);


        /* Report Reply Timeout in Microseconds */
        /* ------------------------------------ */

        scr.w = p_ir->scr.w;
        p_mvb_status->t_reply = t_reply[scr.b.tmo];


        /* Report Frame Error Count             */
        /* ------------------------------------ */

        dr.w = p_ir->dr.w;
        if ( (dr.b.laa == 0) && (dr.b.slm != 0) ) /* Line B and SLM set */
        {
            p_mvb_status->lineA_errors = 0L;
            p_mvb_status->lineB_errors = (unsigned long) p_ir->ec;
        }
        else
        {
            p_mvb_status->lineA_errors = (unsigned long) p_ir->ec;
            p_mvb_status->lineB_errors = 0L;
        }

    }
    return (ret_val);
}

/*
*****************************************************************************
* name          _lc_mvb_line                                                *
*                                                                           *
* param. in     bus_id           Traffic Store  Identification              *
*               command          Command word (a bit mask pattern)          *
*                                Bit 0  = Ignored.                          *
*                                Bit 1  = Ignored.                          *
*                                Bit 2  = Ignored.                          *
*                                Bit 3  = Ignored.                          *
*                                Bit 4  = SLA (Switch to line A)            *
*                                Bit 5  = SLB (Switch to line B)            *
*                                Bit 6  = CLA (Reset error counter line A)  *
*                                Bit 7  = CLB (Reset error counter line B)  *
*                                                                           *
*                                If SLA=1 and SLB=1, then redundant line    *
*                                mode is activated.  If both=0, then no     *
*                                action.  If only one of them is 1, then    *
*                                single line mode to selected line is       *
*                                enabled.                                   *
*                                                                           *
* return value  status           LC_OK or LC_REJECT                         *
*****************************************************************************
* description   Modifies Device Status Word from Traffic Store Service Area *
*                                                                           *
*               Fully reentrant as long not the same MVBC/TS is accessed    *
*                                                                           *
* globals       lci_ctrl_blk[bus_id] (eqv. ts_id)                           *
*****************************************************************************
* history       1   95-10-25   BG Created                                   *
*****************************************************************************
*/


int _lc_mvb_line( unsigned int bus_id, unsigned char command )
{
    short ret_val  = LC_REJECT;         /* Return Value, default: Reject   */

    TM_TYPE_DR  dr;                     /* Local copy of DR                */
    unsigned short line_b_only     = FALSE; /* Indicates line B active&SLM */
    unsigned short line_b_only_new = FALSE; /* New selected line B         */

    unsigned short choose_ch = 0;      /* Choose a channel for hardw_cf'g  */

    static short hidden_fc_line_a = 0; /* Hidden counters used to maintain */
    static short hidden_fc_line_b = 0; /* value if other line is selected  */
    static short hidden_ec_line_a = 0;
    static short hidden_ec_line_b = 0;

    TM_TYPE_INT_REGS *p_ir = NULL;      /* Internal Registers                */

    if ( lci_check_ts_valid((unsigned short)bus_id) == LC_OK )
    {
        ret_val = LC_OK;

        p_ir        = &lci_ctrl_blk[bus_id].p_sa->int_regs;


        /* Acquire Frame and Error Count        */
        /* ------------------------------------ */

        dr.w = p_ir->dr.w;
        line_b_only = (dr.b.laa == 0) && (dr.b.slm != 0);

        if ( line_b_only != FALSE )       /* Line B         */
        {
            hidden_fc_line_b = p_ir->fc;
            hidden_ec_line_b = p_ir->ec;
        }
        else                              /* Line A or both */
        {
            hidden_fc_line_a = p_ir->fc;
            hidden_ec_line_a = p_ir->ec;
        }

        if ((command & LC_MVB_LINE_CMD_CLA) != 0)
        {
            hidden_fc_line_a = 0;
            hidden_ec_line_a = 0;
            if (line_b_only == FALSE)
            {
                p_ir->fc = hidden_fc_line_a;
                p_ir->ec = hidden_ec_line_a;
            }
        }

        if ((command & LC_MVB_LINE_CMD_CLB) != 0)
        {
            hidden_fc_line_b = 0;
            hidden_ec_line_b = 0;
            if (line_b_only != FALSE)
            {
                p_ir->fc = hidden_fc_line_b;
                p_ir->ec = hidden_ec_line_b;
            }
        }

        command &= LC_MVB_LINE_CMD_SLA|LC_MVB_LINE_CMD_SLB; /* Filter */

        switch (command)
        {
            case LC_MVB_LINE_CMD_SLA | LC_MVB_LINE_CMD_SLB:
                 {
                     choose_ch = LC_CH_BOTH;
                     line_b_only_new = FALSE;
                     break;
                 }
            case LC_MVB_LINE_CMD_SLA:
                 {
                     choose_ch = LC_CH_A;
                     line_b_only_new = FALSE;
                     break;
                 }
            case LC_MVB_LINE_CMD_SLB:
                 {
                     choose_ch = LC_CH_B;
                     line_b_only_new = TRUE;
                     break;
                 }
        }
        if (command != 0)
        {
            ret_val |= _lc_m_hardw_config((unsigned short)bus_id, choose_ch, LC_TREPLY_UNCHANGED );

            if ( (line_b_only != FALSE) && (line_b_only_new == FALSE) )
            {
                p_ir->fc = hidden_fc_line_a;
                p_ir->ec = hidden_ec_line_a;
            }

            if ( (line_b_only == FALSE) && (line_b_only_new != FALSE) )
            {
                p_ir->fc = hidden_fc_line_b;
                p_ir->ec = hidden_ec_line_b;
            }
        }
    }
    return (ret_val);
}

