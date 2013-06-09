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
*    MODULE       : LC_HEAD.H                                               *
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
*    ABSTRACT     : Macros, access to Programmer's Interface Library        *
*                   Safe memory manipulations                               *
*                   LC-Internal Definitions                                 *
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
*    4.1.0.1  94-05-26  zur Bonsen   BATC      --   corrections             *
*    4.1.0.2  (no changes in this file)                                     *
*    4.2.0.0  94-11-11  zur Bonsen   BATC      --   See below               *
*    4.3.0.0  95-01-31  zur Bonsen   BATL      --   See below               *
*    4.4.0.0  95-02-25  zur Bonsen   BATL      --   See below               *
*    4.5.0.0  95-10-25  zur Bonsen   BATL      --   No changes in this file *
*    4.5.1.0  95-11-28  zur Bonsen   BATL      --   No changes in this file *
*    4.5.2.0  95-11-28  zur Bonsen   BATL      --   No changes in this file *
*    4.6.0.0  95-12-14  zur Bonsen   BAT:      --   No changes in this file *
*                                                                           *
*****************************************************************************
* Updates in release 4.2.0.0:                                               *
*                                                                           *
* LCI_TYPE_ISR_PTR contains 1 parameter and no longer 2                     *
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
* Moved all hardware-specific contents to "lc_tgt.h" in order to keep       *
* all hardware-related items in a single LC file.                           *
*                                                                           *
*****************************************************************************
*/


/*
******************************************************************************
* Compile Check                                                              *
******************************************************************************
*/

#ifndef LC_HEAD_H

    #define LC_HEAD_H

    #ifndef MVBC_H
        #include "mvbc.h"
    #endif
    
    #ifndef LC_TGT_H
        #include "lc_tgt.h"
    #endif

/*  #endif  at bottom of file */

/*
******************************************************************************
* Standard Constants                                                         *
******************************************************************************
*/

#ifndef NULL
    #define NULL 0L
#endif

#ifndef TRUE
    #define TRUE (0==0)
#endif

#ifndef FALSE
    #define FALSE (0!=0)
#endif



/*
******************************************************************************
* Common Macros                                                              * 
******************************************************************************
*/

#define lcx_half(x) ((x)>>1)



/*
******************************************************************************
* Function Prototypes                                                        *
******************************************************************************
*/


typedef void (*LCI_TYPE_ISR_PTR) (unsigned short);   /* Intr. service rout   */

short        lci_check_ts_valid  (unsigned short);   /* Control Block Check  */



/*
******************************************************************************
* Internal Constants                                                         *
******************************************************************************
*/

#define LCI_MAX_ISR       32                /* Max. possible #intr from MVBC */

#define LCI_ISR_PER_CTRLR 16                /* #intr per MVBC intr ctrl. set */

#define LCI_IC_SET         2                /* #intr ctrl sets in MVBC       */

#define LCI_ALL_1 ((unsigned short) 0xFFFF) /* All bits in 16-bit word = '1' */

#define LCI_DA_MASK     0xFF                /* Max. Dev Addr for Class 2/3/4 */



/*
******************************************************************************
* Traffic Store Control Blocks                                               *
******************************************************************************
*                                                                            *
* The LC contains a fixed number of control blocks which describe location   *
* and size of supported Traffic Store; Jump Table to Interrupt Service       *
* Routines.                                                                  *
*                                                                            *
* lc_init     Initializes the control block array to all zero                *
* lc_config   Configures one control block to one MVBC                       *
*                                                                            *
*                                                                            *
*                                                                            *
* ts_id  -->  *************     *********************                        *
*             *ctrl blk  0* --> *ts start address   *                        *
*             *************     *serv. area st. addr*                        *
*              :         :      *                   *                        *
*             *************     *Other ts info.     *                        *
*             *ctrl blk  n*     *                   *                        *
*             *************     *Intr. Handler Info *                        *
*                               *(ISR Jump Table)   *                        *
*             n =               *********************                        *
*             LC_CTRL_BLK_SIZE                                               *
*                                                                            *
*             (see lc_sys.h)                                                 *
*                                                                            *
******************************************************************************
*/

/* All other _CHK_-constants are in LC_SYS.H */


#define LCI_CHK_INIT  0x00 /* Initialized with lc_init                       */
#define LCI_CHK_LC    0x01 /* Link Layer Common checked in                   */

typedef struct
    {
        void                 * p_tm;
        TM_TYPE_SERVICE_AREA * p_sa;
        LCI_TYPE_ISR_PTR       p_isr       [ LCI_MAX_ISR ];
        unsigned short         isr_param   [ LCI_MAX_ISR ];
        unsigned short         kernel_mask [ LCI_IC_SET  ];
                               /* !!! No const: Two Intr. Ctrl in MVBC */
        unsigned short         checklist;
    }
    LCI_TYPE_CTRL_BLK;


    /* Record Item    Description                                           */
    /* -----------    ----------------------------------------------------- */

    /* p_tm         = Pointer to Traffic Memory                             */
    /* p_sa         = Pointer to Service Area                               */
    /* p_isr        = Pointers to Interrupt Service Routines                */
    /* isr_param    = Parameter passed to p_isr [ interrupt_nr ]            */
    /* kernel_mask  = 2 x 16 Bits indicator for ISRs making kernel calls    */
    /* checklist    = Control Block Checklist                               */


    /* p_esf_isr0/1 are referenced in the function _lc_emf( ) which is able */
    /* to service two interrupt service routines on EMF                     */


extern  LCI_TYPE_CTRL_BLK lci_ctrl_blk[LC_CTRL_BLK_SIZE];

#endif /* LC_HEAD_H */
