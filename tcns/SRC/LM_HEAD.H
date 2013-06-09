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
*    SUBPROJECT   : TCN Link-Layer Message (LM)                             *
*                                                                           *
*****************************************************************************
*                                                                           *
*    MODULE       : LM_HEAD.H                                               *
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
*                                                                           *
*    REMARKS      :                                                         *
*                                                                           *
*    DEPENDENCIES : Programmer's Interface Library                          *
*                                                                           *
*    ACCEPTED     :                                                         *
*                                                                           *
*****************************************************************************
*    !!! ATTENTION: MVBC TARGET	                                            *
*                                                                           *
*    This software is intended for MVBC-based target hardware.		    *
*    It is not suitable for BAP15-2/3-based target hardware.		    *
*                                                                           *
*                                                                           *
*****************************************************************************
*                                                                           *
*    HISTORY:                                                               *
*                                                                           *
*    Version  YY-MM-DD  Name         Dept.    Ref   Comments                *
*    -------  --------  ----------   ------   ----  ---------------------   *
*    4.1.0.0  94-01-20  zur Bonsen   BATC      --   created                 *
*                                                   (100% written new)      *
*    4.1.0.1  (No changes in this file)                                     *
*    4.2.0.0  (No changes in this file)                                     *
*    4.3.0.0  95-01-31  zur Bonsen   BATL      --   See below               *
*    4.3.1.0  95-01-31  zur Bonsen   BATL      --   TCN Hamster introduced  *
*    4.3.2.0  95-11-18  M.Brotz      BATL      --   See below               *
*    4.3.2.1  95-12-07  zur Bonsen   BATL      --   lmx_p16_to_ptr          *
*                                                   "+" intead of "|" used  *
*                                                                           *
*                                                                           *
*                                                                           *
*****************************************************************************
* Updates in release 4.3.0.0:                                               *
*                                                                           *
* Adaption to targets with MC 68360 Processors (Big Endian)                 *
* Non-ASCII characters removed for use on non-PC systems                    *
* !!! Attention:  O_VCH must be set.                                        *
*                                                                           *
*****************************************************************************
* Updates in release 4.3.2.0:                                               *
*                                                                           *
* Restriction to traffic store location on 256 KByte boundary removed.      *
* Now the requirement for the traffic store location depends on the         *
* traffic store size. 16K traffic store must be located on 16K boundaries,  *
* 32K traffic stores must be located on 32K boundaries 64K traffic stores   *
* must be located on 64K boundaries and 256K or bigger traffic stores must  *
* be located on 256K boundaries as before.                                  *
*****************************************************************************
*/



/*
******************************************************************************
* Compile Check                                                              *
******************************************************************************
*/

#ifndef LM_HEAD_H

    #define LM_HEAD_H

    #ifndef MVBC_H
        #error >>> Include MVBC.H before compiling LM_HEAD.H
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
* Access to Programmer's Interface Library				     *
******************************************************************************
*/


/* As of now, same code applies to all target hardware */

#include "pi_sys.h"

#define lmx_copy16(d,s,n) { pi_copy16(d,s,n); }

#define lmx_disable()     { pi_disable();     }
#define lmx_enable()      { pi_enable();      }

#define lmx_lock_task()   { pi_lock_task();   }
#define lmx_unlock_task() { pi_unlock_task(); }

#define lmx_clear(ptr,size)                  \
            {                                \
                short ii  = (size);          \
                short *pp = (short *) (ptr); \
                while (ii-->0)               \
                {                            \
                    *pp++ = 0;               \
                    ii--;                    \
                }                            \
            }


/*
******************************************************************************
* Access to Debugging System (Hamster)                                       *
******************************************************************************
*/

    #ifdef O_LM_TESTING

        /* Fake Hamster */

        #include <stdio.h>

        #define LMX_POSITION              "!!! hamster(File=%s,Line=%d,Act=%s,NPar=%d 0x%04x 0x%04x 0x%04x)\n", __FILE__, __LINE__
        #define LMX_CONTINUE              "cont."
        #define LMX_HALT                  "halt"
        #define LMX_RESET                 "reset"
        #define LMX_SUSPEND               "susp."

        #define lmx_hamster               printf
        #define lmx_hamster_continue(i) { if (i) { lmx_hamster( LMX_POSITION, LMX_CONTINUE, 1,(i),0,0); } }
        #define lmx_hamster_halt(i)     { if (i) { lmx_hamster( LMX_POSITION, LMX_HALT,     1,(i),0,0); } }


    #else

	#ifdef O_TCN

	    /* IEC TCN Event Recorder */

	    #include "recorder.h"

            #define LMX_POSITION              __FILE__,__LINE__,NSE_EVENT_TYPE_ERROR
            #define LMX_CONTINUE              NSE_ACTION_CONTINUE
            #define LMX_HALT                  NSE_ACTION_HALT
            #define LMX_RESET                 NSE_ACTION_REBOOT
            #define LMX_SUSPEND               NSE_ACTION_SUSPEND
 
            #define lmx_hamster_continue(i) { if (i) { nse_record( LMX_POSITION, LMX_CONTINUE, 0,0); } }
            #define lmx_hamster_halt(i)     { if (i) { nse_record( LMX_POSITION, LMX_HALT,  0,0); } }

            #define lmx_hamster(abc,d,e,f,g,h) \
            {          nse_record(abc,d,0,(UINT8*)0) ; }

	#else

            /* Real Hamster */

            #include "du_sys.h"

            #define LMX_POSITION              DU_POSITION
            #define LMX_CONTINUE              DU_CONTINUE
            #define LMX_HALT                  DU_HALT
            #define LMX_RESET                 DU_RESET
            #define LMX_SUSPEND               DU_SUSP
 
            #define lmx_hamster_continue(i) { hamster_continue(i); }
            #define lmx_hamster_halt(i)     { hamster_halt(i)    ; }
            #define lmx_hamster               hamster

	#endif

    #endif




/*
******************************************************************************
* Address Arithmetics                                                        *
******************************************************************************
*                                                                            *
* !!! ATTENTION  The following macros must be used in order to realize       *
* -------------  arithmetics.  Addition is permitted with POSITIVE values    *
*                only.  Not allowed: Pointer inc/dec across aligned          *
*                64K blocks.  OTHERWISE, this code will not work on Intel    *
*                80x86 target systems with segment/offset addressing.        *
*                                                                            *
*                If O_PC or O_EKR is defined, then an Intel CPU is assumed.  *
*                                                                            *
******************************************************************************
*/

#if defined(O_PC) || defined(O_EKR) || defined (O_SEG)
/*
*****************************************************************************
* Conversion from (void *) to linear address (unsigned long)                *
*****************************************************************************
* !!! Pointer converted to linear address in order to allow address calcu-  *
*     lation over the entire addressing range.  This function respects the  *
*     Intel segment/offset addressing method.                               *
*****************************************************************************
*/
    #define lmx_ptr_to_linear(ptr) ( (((unsigned long) (ptr)) & 0xFFFFL) + ((((unsigned long) (ptr))>>12) & 0xFFFF0L) )
  
/*
*****************************************************************************
* Conversion from linear address (unsigned long) to (void *)                *
*****************************************************************************
* !!! Linear address converted back to pointer.  This function respects the *
*     Intel segment/offset addressing method.  The result is NORMALIZED.    *
*****************************************************************************
*/
    #define lmx_linear_to_ptr(lin) ((void *) ( (((lin) & 0xFFFF0L)<<12) | ((lin) & 0xFL) ) )
 
/*
*****************************************************************************
* Pointer Normalization                                                     *
*****************************************************************************
* !!! Intel Pointer Normalization                                           * 
*                                                                           *
*****************************************************************************
*/
    #define lmx_normalize_ptr(ptr) lmx_linear_to_ptr(lmx_ptr_to_linear(ptr))

/*
*****************************************************************************
* Full Pointer Addition over entire memory range                            *
*****************************************************************************
* !!! This function respects the Intel segment/offset addressing method.    *
*     The result is a NORMALIZED pointer.                                   *
*****************************************************************************
*/

    #define lmx_ptr_add(ptr,ofs) lmx_linear_to_ptr( lmx_ptr_to_linear(ptr) + ((unsigned long) (ofs)) )

/*
*****************************************************************************
* Pointer addition within a segment (Intel only)                            *
*****************************************************************************
* !!! This function performs an efficient pointer addition within a segment.*
*     This restriction applies to Intel processors only.                    *
*     The results is not normalized, the segment remains unchanged.         *
*****************************************************************************
*/

    #define lmx_ptr_add_ofs(ptr,ofs) ((void *) (((unsigned long) (ptr)) + ((unsigned long) (ofs))))

/*
*****************************************************************************
* Pointer addition at segment level (Intel only)                            *
*****************************************************************************
* !!! This function performs an efficient pointer addition over segments.   *
*     The granularity is 16 bytes, so the lower 4 bits of 'ofs are ignored. *
*     The offset remains unchanged all time.                                *
*****************************************************************************
*/

    #define lmx_ptr_add_seg(ptr,ofs) ((void *) (((unsigned long) (ptr)) + ((((unsigned long) (ofs)) & 0xFFFF0l) << 12) ))

#else /* 'normal' (no Intel) microprocessors with linear addressing */

    #define lmx_ptr_to_linear(ptr)    ((unsigned long) (ptr))
    #define lmx_linear_to_ptr(lin)    ((void *) (lin))
    #define lmx_ptr_add(ptr,ofs)      ( (void *) ( ((unsigned char *) (ptr)) + ((unsigned long) (ofs)) ) )
    #define lmx_ptr_add_seg(ptr,ofs)  lmx_ptr_add(ptr,ofs)
    #define lmx_ptr_add_ofs(ptr,ofs)  lmx_ptr_add(ptr,ofs)
    #define lmx_normalize_ptr(ptr)    (ptr)

#endif

/*
*****************************************************************************
* Conversion from (void *) to MVBC 16-bit pointer                           *
*****************************************************************************
* !!! This conversion causes information loss.  This macro outputs a 16-bit *
*     pointer which addresses a MF-Table or message queue inside a specified*
*     256K region.                                                          *
*****************************************************************************
*/

#define lmx_ptr_to_p16(p) tm_linear_to_p16(lmx_ptr_to_linear(p))

/*
*****************************************************************************
* Conversion from MVBC 16-bit pointer to (void *)                           *
*****************************************************************************
* !!! This conversion requires two parameters in order to restore the full  *
*     address as seen from the CPU.  'p16' is the 16-bit pointer, and       *
*     'base' is a (void *) pointing to -> any <- location inside a 256 KB   *
*     block where the 16-bit pointer is applied by the MVBC.  For example,  *
*     'base' may point to any queue element in the affected Traffic Store   *
*     if the 16-bit pointer is a queue pointer.                             *
* MBr: Version 4.3.2.0                                                      *
*      Requirements to the location of the traffic memory now dependent     *
*      from the traffic memory size. Macro TM_SIZE_xxK is used to determine *
*      the actual traffic memory size                                       *
*****************************************************************************
*/
         
#if defined (TM_SIZE_16K)
  #define lmx_p16_to_ptr(p16,base) lmx_linear_to_ptr( (lmx_ptr_to_linear(base) & 0xFFFFC000L) + ( ((unsigned long) (p16)) <<2) );
#elif defined (TM_SIZE_32K)
  #define lmx_p16_to_ptr(p16,base) lmx_linear_to_ptr( (lmx_ptr_to_linear(base) & 0xFFFF8000L) + ( ((unsigned long) (p16)) <<2) );
#elif defined (TM_SIZE_64K)
  #define lmx_p16_to_ptr(p16,base) lmx_linear_to_ptr( (lmx_ptr_to_linear(base) & 0xFFFF0000L) + ( ((unsigned long) (p16)) <<2) );
#else
  #define lmx_p16_to_ptr(p16,base) lmx_linear_to_ptr( (lmx_ptr_to_linear(base) & 0xFFFC0000L) + ( ((unsigned long) (p16)) <<2) );
#endif





/*
******************************************************************************
* LM Constants                                                               * 
******************************************************************************
*/


/* Send Queue Identifiers; Number of Send Queues available */

#define LMI_XQ0                  0      /* Points Transmit Queue 0           */
#define LMI_XQ1                  1      /* Points Transmit Queue 1           */
#define LMI_XQ_COUNT             2      /* Number of transmit queues         */


/* TACK Bits */

#define LMI_TACK_ACTIVE          0xFFFF /* Successful MD transfer            */
#define LMI_TACK_PASSIVE         0x0000 /* No or failed MD transfer          */


/* Message-Data Related Constants */

#define LMI_PT_TO_PT             1      /* Communication Mode (CM) set to  1 */
#define LMI_MSG_SIZE_WD          16     /* Number of words inside a MD packet*/


/* LM Control Block Checklist values */

#define LMI_CHK_INIT             0      /* lm_v_system_init() called         */
#define LMI_CHK_CONFIGURED       1      /* lm_m_v_config()    called         */
#define LMI_CHK_OPERATIONAL      2      /* lm_m_v_init()      called         */


/* Limited Device Address Boundary */

#define LMI_HIGHEST_DA           0xFF   /* Highest allowed Dev. Address      */


/* MVBC Message Queue related constants */

#define LMI_Q_BLK_P16U_SIZE  8          /* MD size in 32-Byte units          */
#define LMI_Q_BLK_BYTE_SIZE 32          /* Q. Data size, in bytes = MD size  */
#define LMI_MIN_QUEUE_SIZE   2          /* Minimum allowed MD queue size     */


/*
******************************************************************************
* Types: LM Control Block                                                    * 
******************************************************************************
*/

typedef struct
    {
        TM_TYPE_SERVICE_AREA *p_tm_service_area;   /* Service Area St. Addr */
        TM_TYPE_LLR *p_xq[LMI_XQ_COUNT];           /* Ptr to LLR XQ0 & XQ1  */
        TM_TYPE_LLR *p_rq;                         /* Ptr to LLR RQ         */
        unsigned short checklist;                  /* Checklist             */
        unsigned short rq_overflow;                /* Queue Overflow Flag   */
        void (* get_pack)(void **, MD_PACKET ** ); /* Adopted get_pack fn.  */
        void (* put_pack)( MD_PACKET * );          /* Adopted put_pack fn.  */
        void **ident;                              /* Pack. Pool Identifier */
    } 
        LMI_TYPE_CTRL_BLK;

extern  LMI_TYPE_CTRL_BLK lmi_ctrl_blk[LC_CTRL_BLK_SIZE];


/*
******************************************************************************
* Common Macros                                                              * 
******************************************************************************
*/

#define lmx_half(x) ((x)>>1)


#endif /* LM_HEAD_H */
