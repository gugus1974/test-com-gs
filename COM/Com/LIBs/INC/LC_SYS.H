/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> Included "atr_sys.h"                                                                */
/* <ATR:02> Added DYD bit in device status word                                                 */
/* <ATR:03> Added STD bit in device status word                                                 */
/*==============================================================================================*/

/* <ATR:01> */
#include "atr_sys.h"


/*
********************************************************************************
*                                                                              *
*    Property of  :   ABB Verkehrssysteme AG, CH-8050 Zurich / Switzerland     *
*    COPYRIGHT    :   (c) 1990 ABB Verkehrssysteme AG                          *
*                                                                              *
********************************************************************************
*                                                                              *
*    PROJECT      : IEC TCN (Train Communication Network)  Real Time Protocol  *
*                                                                              *
*    SUBPROJECT   : Link Layer Common                                          *
*                                                                              *
********************************************************************************
*                                                                              *
*    MODULE       : lc_sys.h                                                   *
*                                                                              *
*    DOCUMENT     :                                                            *
*                                                                              *
*    ABSTRACT     : Definition of constants and prototypes link layer (LC)     *
*                                                                              *
*    REMARKS      : Indirect call table is defined in lcindt.c                 *
*                                                                              *
*                   !!! ATTENTION !!! MVB-HW: MVBC                             *
*                   ------------------------------                             *
*                                                                              *
*                   This LC is intended for target hardware containing MVBC    *
*                   It applies to all LC files, not just lc_sys.h.             *
*                                                                              *
*    DEPENDENCIES : vcos_sys.h                                                 *
*                                                                              *
*                   A header file with link to an operating system.            *
*                   Details are described inside this lc_sys file.             *
*                                                                              *
*                                                                              *
*    ACCEPTED     :                                                            *
*                                                                              *
*                                                                              *
********************************************************************************
*                                                                              *
*    HISTORY:                                                                  *
*                                                                              *
*    Version  YY-MM-DD  Name   Dept.   Ref   Status                            *
*    -------  --------  -----  ------  ----  -------                           *
*     1.0     92-05-05  Mr     BAEI-2   --   created                           *
*     1.1     92-07-14  roe    BAEI-2   --   adapted to new BF/LC interface    *
*     1.1.0.2 93-02-08  MuengerBAEI-2   --   intr_enter/intr_exit added        *
*     1.2.0.2 93-02-08  Gatz   BAEI-2   --   read_foreign_dsw only PC          *
*     1.3     93-03-24  MuengerBAEI-2   --   added reset ISR for extint        *
*                                                                              *
*     Attention: MVBC Versions of LC starts with 4.1.0.0.                      *
*                (Version 3 has been left free intentionally!)                 *
*                                                                              *
*     4.1.0.0 94-01-20  zur Bonsen   BATC      --   Adaption to MVBC           *
*     4.1.0.1 94-05-26  zur Bonsen   BATC      --   lc_wait added              *
*     4.1.0.2 (no changes in this file)                                        *
*     4.2.0.0 94-11-11  zur Bonsen   BATC      --   See below                  *
*     4.3.0.0 95-01-31  zur Bonsen   BATL      --   See below                  *
*     4.4.0.0 95-01-31  zur Bonsen   BATL      --   See below                  *
*     4.4.0.1 95-03-21  Marsden      BATL      --   See below                  *
*     4.4.0.2 95-10-25  zur Bonsen   BAT:      --   LC_PCS_..._TWCS added      *
*     4.5.0.0 95-10-25  zur Bonsen   BAT:      --   See below                  *
*     4.5.1.0 95-11-28  zur Bonsen   BAT:      --   No changes in this file    *
*     4.5.2.0 95-11-28  zur Bonsen   BAT:      --   LME structure update       *
*     4.6.0.0 95-12-14  zur Bonsen   BAT:      --   Device Address Fixup       *
*                                                                              *
********************************************************************************
* Change in 4.1.0.2:   (a)  see lc_layer.c  (only affected "C" file)           *
*                      (b)  compile mvbc.h version 1.1 (see header !)          *
*                                                                              *
********************************************************************************
* Change in 4.2.0.0:   (a)  lc_int_connect: ISR function carries 1 parameter   *
*                           and no longer 2 parameters                         *
*                                                                              *
*                      (b)  Indirect calling mechanism has become optional     *
*                                                                              *
*                           !!! Attention !!!                                  *
*                                                                              *
*                           Starting in version 4.2, the compiler switch       *
*                           O_ICM must be defined to activate the indirect     *
*                           calling mecanism.  Default: O_ICM not set.         *
*                                                                              *
********************************************************************************
* Updates in release 4.3.0.0:                                                  *
*                                                                              *
* (a) Adaption to targets with MC 68360 Processors (Big Endian)                *
*     Non-ASCII characters removed for use on non-PC systems                   *
*                                                                              *
* (b) Corrected LC_INT_LP_RCEV (formerly known as LC_INT_LM_SEND which was     *
*     wrong.                                                                   *
*                                                                              *
* (c) Decoupled lc_sys.h from old MicOS structures, namely "Boot Code" (BC)    *
*     The indirect calling table has been organized in order to fit	       *
*     different operating systems and no longer just "MicOS".	 	       *
*                                                                              *
* (d) Inputs from Wilfred Marsden: New constants and macros which are used     *
*     by Link Layer Process Data and Bus Administrator			       *
*                                                                              *
* (e) JDP Version Information added                                            *
*                                                                              *
********************************************************************************
* Updates in release 4.4.0.0:                                                  *
*                                                                              *
* (a) Introdcution of lc_target.h:  True support for multiple MVBC systesm     *
*     with 1 LC program, all hardware-dependent stuff is located in            *
*     lc_target.h.  Pointer arithmetic has been moved from lc_head.h to        *
*     lc_sys.h since the same arithmetics is used by both LC and LM.  This     *
*     pointer arithmetic shall not be used by applicants and is therefore not  *
*     documented. 							       *
*                                                                              *
* (e) Support TCN Hamster (Event Recorder) introduced                          *
*                                                                              *
********************************************************************************
* Updates in release 4.4.0.1:                                                  *
*                                                                              *
* (a) New Function lc_memset16(* ptr, val, size)      - all unsigned short     *
*                                                                              *
*****************************************************************************
* Updates in release 4.5.0.0:                                               *
*                                                                           *
* LME support implemented: lc_m_mvb_status and lc_m_mvb_line                *
*                                                                           *
********************************************************************************
*/

#ifndef         LC_SYS_H
    #define     LC_SYS_H

#define LC_TRUE  (0==0)
#define LC_FALSE (0!=0)


/*
        ***********************************************************
        *                                                         *
        * Includes to Operating Systems if ICM is used            *
        *                                                         *
        ***********************************************************
*/

#ifdef O_ICM

    /* Obtain access to operating system */
    /* --------------------------------- */

    #include "vcos_sys.h"

    /* Items "vcos_sys.h" must define in order to use indirect calling mechanism and version	*/
    /* verification mechanism properly:								*/
    /*												*/
    /* VCOS_SUBPROJECT_LC:  An ID referring to LC and no other subproject, e.g. a number	*/
    /*												*/
    /* vcos_version_verify: A central function which can verify the version of any subproject,  */
    /*			    e.g. this one.							*/
    /*			    1st. input param:   ID, e.g. VCOS_SUBPROJECT_LC, appropriate type	*/
    /*			    2nd. input param:   Version Number, of numeric type (number passed) */
    /*			    3rd. input param:   Release Number, of numeric type (number passed) */
    /*			    return value:       None allowed 					*/
    /*                      This function is called when "lc_version()" is invoked.  The oper-  */
    /*			    ating system shall compare version and release number supplied by   */
    /*			    input parameters with the version info stored in the beginning of   */
    /*			    the indirect calling table.  If a version mismatch occurs, then the */
    /*			    operating system shall take actions accordingly, e.g. report it     */
    /*			    and refuse continuing.						*/
    /*												*/
    /* LC_ADR_TABLE:        Address to indirect calling table 					*/
    /*												*/
    /* If you have a different 'operating system', then you should create a header file         */
    /* "vcos_sys.h" which defines and/or connects the symbols listed above.			*/

#endif

/*
        ***********************************************************
        *                                                         *
        * Version Verification Mechanism                          *
        *                                                         *
        ***********************************************************
*/

#define LC_VERSION       4
#define LC_RELEASE       6

/*      Other version info: LC_UPDATE and LC_EVOLUTION, are located in lcindt.c.		*/
/*	Reason:  Changes at "update" or "evolution" level shall have no effect to the upper	*/
/*	user interface, e.g. lc_sys.h 								*/

/* JDP Version Information */
/* ----------------------- */

#define lc_get_version() \
"RTP_LC-V4.6-14.12.95           "  /* !!! Must be exactly 31 Chars + '\0' */



/*
        ***********************************************************
        *                                                         *
        * Basic Constants, Types and Functions                    *
        *                                                         *
        ***********************************************************
*/

/* Interrupt Codes */

#define LC_INT_DTI1      0x00
#define LC_INT_DTI2      0x01
#define LC_INT_DTI3      0x02
#define LC_INT_DTI4      0x03
#define LC_INT_DTI5      0x04
#define LC_INT_DTI6      0x05
#define LC_INT_DTI7      0x06
#define LC_INT_BTI       0x07
#define LC_INT_RTI       0x08
#define LC_INT_SFC       0x09
#define LC_INT_MFC       0x0A
#define LC_INT_AMFX      0x0B
#define LC_INT_DSF       0x0C
#define LC_INT_DMF       0x0D
#define LC_INT_ESF       0x0E
#define LC_INT_EMF       0x0F

#define LC_INT_EXT_0     0x10
#define LC_INT_EXT_1     0x11
#define LC_INT_TIMER_1   0x12
#define LC_INT_FEV       0x17
#define LC_INT_RQC       0x18
#define LC_INT_XQ0C      0x19
#define LC_INT_XQ1C      0x1A
#define LC_INT_RQE       0x1B
#define LC_INT_XQE       0x1C
#define LC_INT_EXT_2     0x1D
#define LC_INT_EXT_3     0x1E
#define LC_INT_TIMER_2   0x1F

/* Expressions for compatibility reasons */

#define LC_INT_LP_SEND       LC_INT_DTI1
#define LC_INT_LP_RECV       LC_INT_DTI2  /* Corrected fr. LM_SEND */
#define LC_INT_EXT_5         LC_INT_EXT_0
#define LC_INT_EXT_6         LC_INT_EXT_1
#define LC_INT_TIMER         LC_INT_TIMER_1

#define LC_KERNEL                     0   /* !!! keep consistent with     */
#define LC_NO_KERNEL                  1   /*     constants in LCINTR.S    */

#define LC_OK                         0   /* Return Values */
#define LC_REJECT                     1
#define LC_K_CALLS                    2

#define LC_DSW_SER_MSK              0x0001   /* Device Status Report */
#define LC_DSW_SER_SET              0x0001
#define LC_DSW_SER_CLR              0x0000

#define LC_DSW_DNR_MSK              0x0002
#define LC_DSW_DNR_SET              0x0002
#define LC_DSW_DNR_CLR              0x0000

#define LC_DSW_FRC_MSK              0x0004
#define LC_DSW_FRC_SET              0x0004
#define LC_DSW_FRC_CLR              0x0000

#define LC_DSW_ERD_MSK              0x0008
#define LC_DSW_ERD_SET              0x0008
#define LC_DSW_ERD_CLR              0x0000

#define LC_DSW_SDD_MSK              0x0010
#define LC_DSW_SDD_SET              0x0010
#define LC_DSW_SDD_CLR              0x0000

#define LC_DSW_SSD_MSK              0x0020
#define LC_DSW_SSD_SET              0x0020
#define LC_DSW_SSD_CLR              0x0000

#define LC_DSW_RLD_MSK              0x0040
#define LC_DSW_RLD_SET              0x0040
#define LC_DSW_RLD_CLR              0x0000

#define LC_DSW_LAA_MSK              0x0080
#define LC_DSW_LAA_SET              0x0080
#define LC_DSW_LAA_CLR              0x0000

#define LC_DSW_MAS_MSK              0x0100
#define LC_DSW_MAS_SET              0x0100
#define LC_DSW_MAS_CLR              0x0000

#define LC_DSW_ACT_MSK              0x0200
#define LC_DSW_ACT_SET              0x0200
#define LC_DSW_ACT_CLR              0x0000

#define LC_DSW_AX_MSK               0x0C00
#define LC_DSW_AX_0                 0x0000
#define LC_DSW_AX_1                 0x0400
#define LC_DSW_AX_2                 0x0800
#define LC_DSW_AX_3                 0x0C00
#define LC_DSW_AX_UNIT              LC_DSW_AX_1

/* <ATR:02> */
#define LC_DSW_DYD_MSK              0x0400
#define LC_DSW_DYD_SET              0x0400
#define LC_DSW_DYD_CLR              0x0000

/* <ATR:03> */
#define LC_DSW_STD_MSK              0x0800
#define LC_DSW_STD_SET              0x0800
#define LC_DSW_STD_CLR              0x0000

#define LC_DSW_TYPE_MSK             0xF000

#define LC_DSW_TYPE_SPECIAL_MSK     0x8000
#define LC_DSW_TYPE_SPECIAL_SET     0x8000
#define LC_DSW_TYPE_SPECIAL_CLR     0x0000

#define LC_DSW_TYPE_BUS_ADMIN_MSK   0x4000
#define LC_DSW_TYPE_BUS_ADMIN_SET   0x4000
#define LC_DSW_TYPE_BUS_ADMIN_CLR   0x0000

#define LC_DSW_TYPE_BRIDGE_MSK      0x2000
#define LC_DSW_TYPE_BRIDGE_SET      0x2000
#define LC_DSW_TYPE_BRIDGE_CLR      0x0000

#define LC_DSW_TYPE_CLASS_2_3_MSK   0x1000
#define LC_DSW_TYPE_CLASS_2_3_SET   0x1000
#define LC_DSW_TYPE_CLASS_2_3_CLR   0x0000

#define LC_DSW_TYPE_SPEC_MSK        0x7000
#define LC_DSW_TYPE_SPEC_CLR        0x0000
#define LC_DSW_TYPE_SPEC_CLASS1_SET 0x7000

/* Other allowed LC_DSW_TYPE_SPEC_..._SET values for future use: */
/* 0x6000, 0x5000, 0x4000, 0x3000, 0x2000, 0x1000, 0x0000        */

#define LC_DSW_ALL_MSK              0xFFFF
#define LC_DSW_ALL_CLR              0x0000


#define LC_MCM_16K               0        /* Memory Config. Modes */
#define LC_MCM_32K               1
#define LC_MCM_64K               2
#define LC_MCM_256K              3
#define LC_MCM_1M                4

#define LC_0_TO_256K             0        /* MF-Table/Queue Offset Values */
#define LC_256_TO_512K           1
#define LC_512_TO_768K           2
#define LC_768K_TO_1M            3

#define LC_CTRL_BLK_SIZE         8        /* Description Control Blocks */


/* MVBC Hardware Configuration Parameters */

#define LC_CH_A                  1        /* Channel A */
#define LC_CH_B                  0        /* Channel B */
#define LC_CH_BOTH               2        /* Both Channels */

#define LC_TREPLY_21US           0        /* Reply Timeout Coefficients */
#define LC_TREPLY_43US           1
#define LC_TREPLY_64US           2
#define LC_TREPLY_85US           3

#define LC_TREPLY_UNCHANGED     10        /* Don't change TREPLY at all */


/*
 ---------------------------------------------------------------------------
 Volatile attribute: Assures that register accesses are not optimized by "C"
 ---------------------------------------------------------------------------
*/

#ifndef VOL
   #define VOL volatile
#endif

/*
 ---------------------------------------------------------------------------
    Traffic Store Size Constants
 ---------------------------------------------------------------------------
*/


#define LC_TS_MAX_PORT_ADDR          4095

#if !defined (O_LC_MCM)
    #define O_LC_MCM                 LC_MCM_256K
#endif

#if   (O_LC_MCM == LC_MCM_16K)

    #define LC_TS_MAX_DA_PORT_INDX   0
    #define LC_TS_MAX_LA_PORT_INDX   255

#elif (O_LC_MCM == LC_MCM_32K)

    #define LC_TS_MAX_DA_PORT_INDX   255
    #define LC_TS_MAX_LA_PORT_INDX   255

#elif (O_LC_MCM == LC_MCM_64K)

    #define LC_TS_MAX_DA_PORT_INDX   255
    #define LC_TS_MAX_LA_PORT_INDX   1023

#elif (O_LC_MCM == LC_MCM_256K)

    #define LC_TS_MAX_DA_PORT_INDX   1023
    #define LC_TS_MAX_LA_PORT_INDX   4095

#elif (O_LC_MCM == LC_MCM_1024K)

    #define LC_TS_MAX_DA_PORT_INDX   4095
    #define LC_TS_MAX_LA_PORT_INDX   4095

#endif

/*
 ---------------------------------------------------------------------------
    Traffic Store Type Definitions for PIT, PCS and Port
 ---------------------------------------------------------------------------
*/

#if (O_LC_MCM <= LC_MCM_32K)
typedef unsigned short     LC_TYPE_TS_PIT [(LC_TS_MAX_PORT_ADDR + 1) / 2];
#else
typedef unsigned short     LC_TYPE_TS_PIT [LC_TS_MAX_PORT_ADDR + 1];
#endif

typedef VOL unsigned short LC_TYPE_TS_PCS [4];

typedef LC_TYPE_TS_PCS     LC_TYPE_TS_LA_PCS [LC_TS_MAX_LA_PORT_INDX + 1];

typedef unsigned short     LC_TYPE_TS_DCK [4];
typedef LC_TYPE_TS_DCK     LC_TYPE_TS_PGE [4];
typedef LC_TYPE_TS_PGE     LC_TYPE_TS_PRT [2];

typedef unsigned short     LC_TYPE_TS_PGE_WTB [64];
typedef LC_TYPE_TS_PGE_WTB LC_TYPE_TS_PRT_WTB [2];

#if (O_LC_MCM >= LC_MCM_32K)
typedef LC_TYPE_TS_PCS     LC_TYPE_TS_DA_PCS [LC_TS_MAX_DA_PORT_INDX + 1];
typedef LC_TYPE_TS_PRT     LC_TYPE_TS_DA_PRT [(LC_TS_MAX_DA_PORT_INDX + 1) / 4];
#endif


/*
 ---------------------------------------------------------------------------
    Traffic Store Access Routines for PIT
 ---------------------------------------------------------------------------
*/

#if defined (O_LE)
#define LC_PIT_MSK_ODD    0x00FF
#define LC_PIT_MSK_EVN    0xFF00
#define LC_PIT_OFF_ODD    8
#define LC_PIT_OFF_EVN    0
#else
#define LC_PIT_MSK_ODD    0xFF00
#define LC_PIT_MSK_EVN    0x00FF
#define LC_PIT_OFF_ODD    0
#define LC_PIT_OFF_EVN    8
#endif

#if (O_LC_MCM <= LC_MCM_32K)

#define lc_put_pit(p_pit, prt_addr, prt_indx)             \
{ unsigned short tmp_prt_ad = (prt_addr) >> 1;            \
  unsigned short tmp_prt_ix;                              \
  if (((prt_addr) & 1) == 1)                              \
  { tmp_prt_ix  = p_pit[0][tmp_prt_ad] & LC_PIT_MSK_ODD;  \
    tmp_prt_ix |= ((prt_indx) << LC_PIT_OFF_ODD);         \
    p_pit[0][tmp_prt_ad] = tmp_prt_ix;                    \
  } else                                                  \
  { tmp_prt_ix  = p_pit[0][tmp_prt_ad] & LC_PIT_MSK_EVN;  \
    tmp_prt_ix |= ((prt_indx) << LC_PIT_OFF_EVN);         \
    p_pit[0][tmp_prt_ad] = tmp_prt_ix;                    \
  } }


#define lc_get_pit(p_ix, p_pit, prt_addr)                                   \
{ if (((prt_addr) & 1) == 1)                                                \
  { *p_ix = (p_pit[0][(prt_addr) >> 1] & LC_PIT_MSK_EVN) >> LC_PIT_OFF_ODD; } \
  else                                                                      \
  { *p_ix = (p_pit[0][(prt_addr) >> 1] & LC_PIT_MSK_ODD) >> LC_PIT_OFF_EVN; } \
}


#else
#define lc_put_pit(p_pit, prt_addr, prt_indx)        \
        { p_pit[0][(prt_addr)] = (prt_indx); }

#define lc_get_pit(p_ix, p_pit, prt_addr)            \
        { *(p_ix) = p_pit[0][(prt_addr)]; }
#endif

/*
 ---------------------------------------------------------------------------
    Traffic Store Access Routines for PCS
 ---------------------------------------------------------------------------
*/

#define lc_put_pcs(p_pcs, mask, offset, value)       \
        {VOL unsigned short * p_pcs_vol = (VOL unsigned short *) (p_pcs); \
         *(p_pcs_vol + ((offset) / 16))  =       \
        ((mask) & ((value) << ((offset) % 16))) |       \
        ( *(p_pcs_vol + ((offset) / 16)) & ~(mask));}

#define lc_get_pcs(p_val, p_pcs, mask, offset)                    \
        {VOL unsigned short * p_pcs_vol = (VOL unsigned short *) (p_pcs); \
         *p_val = (*(p_pcs_vol + ((offset) / 16)) & (mask)) \
                >> ((offset) % 16);}

#define lc_tgl_pcs(p_pcs, mask, offset) \
        {VOL unsigned short * p_pcs_vol = (VOL unsigned short *) (p_pcs); \
          (*(p_pcs_vol + ((offset) / 16))) ^= (mask); }

#define lc_inc_pcs(p_pcs, mask, offset) \
        {VOL unsigned short * p_pcs_vol = (VOL unsigned short *) (p_pcs); \
          (*(p_pcs_vol + ((offset) / 16))) += 1; }


#define lc_put_pcs_bap(a,b,c,d)         /* empty function for mvbc */
#define lc_put_pcs_mvbc         lc_put_pcs



/*
 ---------------------------------------------------------------------------
    Constants as Parameter for Traffic Store PCS Access Routines
 ---------------------------------------------------------------------------
*/


#define LC_PCS_MSK_FCODE        0xF000
#define LC_PCS_OFF_FCODE        12
#define LC_PCS_VAL_FCODE_08     8
#define LC_PCS_VAL_FCODE_09     9
#define LC_PCS_VAL_FCODE_15     15

#define LC_PCS_MSK_TYPE         0x0C00
#define LC_PCS_OFF_TYPE         10
#define LC_PCS_VAL_TYPE_CLR     0
#define LC_PCS_VAL_TYPE_SNK     1
#define LC_PCS_VAL_TYPE_SRC     2

#define LC_PCS_MSK_TWCS         0x0200
#define LC_PCS_OFF_TWCS         9
#define LC_PCS_VAL_TWCS_ACT     1

#define LC_PCS_MSK_DTI          0x00E0
#define LC_PCS_OFF_DTI          5
#define LC_PCS_VAL_DTI_CLR      0
#define LC_PCS_VAL_DTI_1        1
#define LC_PCS_VAL_DTI_2        2
#define LC_PCS_VAL_DTI_3        3
#define LC_PCS_VAL_DTI_4        4
#define LC_PCS_VAL_DTI_5        5
#define LC_PCS_VAL_DTI_6        6

#define LC_PCS_MSK_FE           0x0001
#define LC_PCS_OFF_FE           0
#define LC_PCS_VAL_FE_CLR       0
#define LC_PCS_VAL_FE_SET       1

#define LC_PCS_MSK_NUM          0x0002
#define LC_PCS_OFF_NUM          1
#define LC_PCS_VAL_NUM_CLR      0
#define LC_PCS_VAL_NUM_SET      1

#define LC_PCS_MSK_VP           0x0040
#define LC_PCS_OFF_VP           (16 + 6)

#define LC_PCS_MSK_DIAG         0x0007
#define LC_PCS_OFF_DIAG         (16 + 0)
#define LC_PCS_VAL_DIAG_CLR     0
#define LC_PCS_VAL_DIAG_STO     1
#define LC_PCS_VAL_DIAG_TERR    2
#define LC_PCS_VAL_DIAG_RDY     3
#define LC_PCS_VAL_DIAG_BNI     4

#define LC_PCS_MSK_TACK         0xFFFF
#define LC_PCS_OFF_TACK         (32 + 0)


/*
 ---------------------------------------------------------------------------
    LME Support
 ---------------------------------------------------------------------------
*/

#define LC_HW_ID_SIZE   32 /* Bytes */
#define LC_SW_ID_SIZE   32 /* Bytes */

struct LC_STR_MVB_STATUS
       {
           unsigned char  hardware_id[LC_HW_ID_SIZE];
                                              /* Identifies MVBC type     */
           unsigned char  software_id[LC_SW_ID_SIZE];
                                              /* Identifies LC version    */
           unsigned long  lineA_errors;       /* Error ctr for line A     */
           unsigned long  lineB_errors;       /* Error ctr for line B     */
           unsigned short link_address;       /* ts_id (high 4 bits), and */
                                              /* dev_addr (lower 12 bits) */
           unsigned short device_status_word; /* Device status word       */
           unsigned char  t_reply;            /* Reply timeout, in us     */

           /* lineA_errors is used if line A (single line mode) or       */
           /*              redundant line communication (A+B) is used.   */
           /* lineB_errors is used if line B (single line mode) is used  */
       };

/* Bit masks to select bit-wise command for 2nd param of lc_m_mvb_line() */

#define LC_MVB_LINE_CMD_CLB  0x80
#define LC_MVB_LINE_CMD_CLA  0x40
#define LC_MVB_LINE_CMD_SLB  0x20
#define LC_MVB_LINE_CMD_SLA  0x10



/*
        ***********************************************************
        *                                                         *
        * Direct Function Prototypes                              *
        *                                                         *
        ***********************************************************
*/

extern short          _lc_init                     (void);
extern short          _lc_m_config                 (unsigned short,
                                                    void *,short,short,short);
extern short          _lc_m_int_connect            (unsigned short,
                                                    void (*)(unsigned short),
                                                    short,
                                                    unsigned short, 
                                                    unsigned short);
extern short          _lc_m_int_disconnect         (unsigned short,short);
extern short          _lc_m_get_device_address     (unsigned short,
                                                    unsigned short *);
extern short          _lc_m_set_device_address     (unsigned short,
                                                    unsigned short);
extern unsigned short _lc_m_get_device_status_word (unsigned short);
extern void           _lc_m_set_device_status_word (unsigned short,
                                                    unsigned short,
                                                    unsigned short);
extern void *         _lc_m_get_service_area_addr  (unsigned short);
extern void           _lc_set_laa_rld              (void);
extern void           _lc_exit                     (void);
extern short          _lc_m_go                     (unsigned short);
extern short          _lc_m_hardw_config           (unsigned short,
                                                    unsigned short,
                                                    unsigned short);
extern void           _lc_intr_hdlr_0_k            (unsigned short);
extern void           _lc_intr_hdlr_1_k            (unsigned short);
extern void           _lc_intr_hdlr_b_k            (unsigned short);
extern short          _lc_intr_hdlr_0_nk           (unsigned short);
extern short          _lc_intr_hdlr_1_nk           (unsigned short);
extern short          _lc_intr_hdlr_b_nk           (unsigned short);
extern void           _lc_wait                     (unsigned short);
extern void           _lc_memset16                 (unsigned short * ptr,
                                                    unsigned short   val,
                                                    unsigned short   sze);
/* Starting here: LME support functions */

extern int            _lc_read_mvb_status          ( unsigned int,
                                                     struct LC_STR_MVB_STATUS* );
extern int            _lc_mvb_line                 ( unsigned int,
                                                     unsigned char );


/*
        ***********************************************************
        *                                                         *
        * Indirect Calling Mechanism (ICM)                        *
        *                                                         *
        ***********************************************************
*/

/* Function Prototypes inside indirect calling table */
/* ------------------------------------------------- */

typedef struct
{

    unsigned char lc_version_info[4];   	/* !!! No const: 4 = Four entries:          */
						/* version / release / update / evolution   */

    short          (*plc_init)                     (void);
    short          (*plc_m_config)                 (unsigned short,
                                                    void *,
                                                    short,
                                                    short,
                                                    short);
    short          (*plc_m_int_connect)            (unsigned short,
                                                    void (*)(unsigned short),
                                                    short,
                                                    unsigned short, 
                                                    unsigned short);
    short          (*plc_m_int_disconnect)         (unsigned short,short);
    short          (*plc_m_get_device_address)     (unsigned short,
                                                    unsigned short *);
    short          (*plc_m_set_device_address)     (unsigned short,
                                                    unsigned short);
    unsigned short (*plc_m_get_device_status_word) (unsigned short);
    void           (*plc_m_set_device_status_word) (unsigned short,
                                                    unsigned short,
                                                    unsigned short);
    void *         (*plc_m_get_service_area_addr)  (unsigned short);
    void           (*plc_set_laa_rld)              (void);
    void           (*plc_exit)                     (void);
    short          (*plc_m_go)                     (unsigned short);
    short          (*plc_m_hardw_config)           (unsigned short,
                                                    unsigned short,
                                                    unsigned short);
    void           (*plc_intr_hdlr_0_k)            (unsigned short);
    void           (*plc_intr_hdlr_1_k)            (unsigned short);
    void           (*plc_intr_hdlr_b_k)            (unsigned short);
    short          (*plc_intr_hdlr_0_nk)           (unsigned short);
    short          (*plc_intr_hdlr_1_nk)           (unsigned short);
    short          (*plc_intr_hdlr_b_nk)           (unsigned short);
    void           (*plc_wait)                     (unsigned short);
    void           (*plc_memset16)                 (unsigned short * ptr,
                                                    unsigned short   val,
                                                    unsigned short   sze);
    /* Starting here: LME support functions */

    int            (*plc_read_mvb_status)          (unsigned int,
                                                    struct LC_STR_MVB_STATUS* );
    int            (*plc_mvb_line)                 (unsigned int,
                                                    unsigned char);
}
    TYPE_LC_IND_CALL;


#ifdef O_ICM


    extern const TYPE_LC_IND_CALL lc_ind_call_table;


    /* Define location for indirect calling table */
    /* ------------------------------------------ */

    /* If O_USE_VARIABLE is defined, then the indirect calling table will be referenced	        */
    /* by a local variable called "lc_pc_ptr_to_table".  This feature allows using an indirect	*/
    /* calling table without presence of a central operating system which maintains pointers to */
    /* indirect calling tables 									*/


    #ifdef O_USE_VARIABLE
        extern  TYPE_LC_IND_CALL*   lc_pc_ptr_to_table;
        #define PLC_IND_CALL        lc_pc_ptr_to_table
    #else
        #define PLC_IND_CALL      (*(TYPE_LC_IND_CALL * *) LC_ADR_TABLE)
    #endif


    /* Connection to Indirect Calling Mechanism */
    /* ---------------------------------------- */

    /* lc_connect() must be the first lc_...()-function to be called when using lc_sys.h with	*/
    /* indirect calling mechanism. 								*/


    #define lc_connect() PLC_IND_CALL = (void *) &lc_ind_call_table


    /* Connection to user with Indirect Calling Mechanism */
    /* -------------------------------------------------- */

    /* The following #define-statements establish function names which are available to the	*/
    /* users.  This will allow a 'transparent' access to a desired function via indirect	*/
    /* calling mechanism. 									*/


    #define lc_init                     (* (PLC_IND_CALL->plc_init))
    #define lc_m_config                 (* (PLC_IND_CALL->plc_m_config))
    #define lc_m_int_connect            (* (PLC_IND_CALL->plc_m_int_connect))
    #define lc_m_int_disconnect         (* (PLC_IND_CALL->plc_m_int_disconnect))
    #define lc_m_get_device_address     (* (PLC_IND_CALL->plc_m_get_device_address))
    #define lc_m_set_device_address     (* (PLC_IND_CALL->plc_m_set_device_address))
    #define lc_m_get_device_status_word (* (PLC_IND_CALL->plc_m_get_device_status_word))
    #define lc_m_set_device_status_word (* (PLC_IND_CALL->plc_m_set_device_status_word))
    #define lc_m_get_service_area_addr  (* (PLC_IND_CALL->plc_m_get_service_area_addr))
    #define lc_set_laa_rld              (* (PLC_IND_CALL->plc_set_laa_rld))
    #define lc_exit                     (* (PLC_IND_CALL->plc_exit))
    #define lc_m_go                     (* (PLC_IND_CALL->plc_m_go))
    #define lc_m_hardw_config           (* (PLC_IND_CALL->plc_m_hardw_config))
    #define lc_intr_hdlr_0_k            (* (PLC_IND_CALL->plc_intr_hdlr_0_k))
    #define lc_intr_hdlr_1_k            (* (PLC_IND_CALL->plc_intr_hdlr_1_k))
    #define lc_intr_hdlr_b_k            (* (PLC_IND_CALL->plc_intr_hdlr_b_k))
    #define lc_intr_hdlr_0_nk           (* (PLC_IND_CALL->plc_intr_hdlr_0_nk))
    #define lc_intr_hdlr_1_nk           (* (PLC_IND_CALL->plc_intr_hdlr_1_nk))
    #define lc_intr_hdlr_b_nk           (* (PLC_IND_CALL->plc_intr_hdlr_b_nk))
    #define lc_wait                     (* (PLC_IND_CALL->plc_wait))
    /* LME support */
    #define lc_read_mvb_status          (* (PLC_IND_CALL->plc_read_mvb_status))
    #define lc_mvb_line                 (* (PLC_IND_CALL->plc_mvb_line))
/*
    #define lc_memset16                 (* (PLC_IND_CALL->plc_memset16))
*/

    /* Version Verification Call:  Not supported here */
    /* ---------------------------------------------- */

    /* The version verification call makes use of an operating-system-specific version 		*/
    /* verification function. 									*/

    #define lc_version()    vcos_version_verify ( VCOS_SUBPROJECT_LC, \
                                                  LC_VERSION,         \
                                                  LC_RELEASE );


#else /* #ifdef O_ICM, now: not O_ICM */

/*
        ***********************************************************
        *                                                         *
        * Direct Calling Mechanism (no ICM)                       *
        *                                                         *
        ***********************************************************
*/

    /* Function lc_connect() has no effect here */
    /* ---------------------------------------- */

    #define lc_connect()  {;}


    /* Connection to user by direct call */
    /* --------------------------------- */

    #define lc_init                     _lc_init
    #define lc_m_config                 _lc_m_config
    #define lc_m_int_connect            _lc_m_int_connect
    #define lc_m_int_disconnect         _lc_m_int_disconnect
    #define lc_m_get_device_address     _lc_m_get_device_address
    #define lc_m_set_device_address     _lc_m_set_device_address
    #define lc_m_get_device_status_word _lc_m_get_device_status_word
    #define lc_m_set_device_status_word _lc_m_set_device_status_word
    #define lc_m_get_service_area_addr  _lc_m_get_service_area_addr
    #define lc_set_laa_rld              _lc_set_laa_rld
    #define lc_exit                     _lc_exit
    #define lc_m_go                     _lc_m_go
    #define lc_m_hardw_config           _lc_m_hardw_config
    #define lc_intr_hdlr_0_k            _lc_intr_hdlr_0_k
    #define lc_intr_hdlr_1_k            _lc_intr_hdlr_1_k
    #define lc_intr_hdlr_b_k            _lc_intr_hdlr_b_k
    #define lc_intr_hdlr_0_nk           _lc_intr_hdlr_0_nk
    #define lc_intr_hdlr_1_nk           _lc_intr_hdlr_1_nk
    #define lc_intr_hdlr_b_nk           _lc_intr_hdlr_b_nk
    #define lc_wait                     _lc_wait
    /* LME Support */
    #define lc_read_mvb_status          _lc_read_mvb_status
    #define lc_mvb_line                 _lc_mvb_line

/*
    #define lc_memset16                 _lc_memset16
*/


    /* Version Verification Call:  Not supported here */
    /* ---------------------------------------------- */

    #define lc_version() {;}


#endif /* #ifndef O_ICM */

/*
        ***********************************************************
        *                                                         *
        * LC-spec. function name remapping for default systems    *
        * containing one Traffic Memory (usual case)              *
        *                                                         *
        ***********************************************************
*/

#define lc_config(a,b,c,d)             lc_m_config(0,a,b,c,d)
#define lc_int_connect(a,b,c,d)        lc_m_int_connect(0,a,b,c,d)
#define lc_int_disconnect(a)           lc_m_int_disconnect(0,a)
#define lc_get_device_address(a)       lc_m_get_device_address(0,a)
#define lc_set_device_address(a)       lc_m_set_device_address(0,a)
#define lc_get_device_status_word()    lc_m_get_device_status_word(0)
#define lc_set_device_status_word(a,b) lc_m_set_device_status_word(0,a,b)
#define lc_get_service_area_addr()     lc_m_get_service_area_addr(0)
#define lc_go()                        lc_m_go(0)
#define lc_hardw_config(a,b)           lc_m_hardw_config(0,a,b)


/*
 ---------------------------------------------------------------------------

    Functions implemented as macros

 ---------------------------------------------------------------------------
*/

#define lc_memset16(ptr, val, size)      \
        {                                \
        unsigned short ii  = (size);     \
        unsigned short *pp = (unsigned short *) (ptr); \
            while (ii-- != 0)            \
            {                            \
                *pp++ = val;             \
            }                            \
        }




#endif                                 /* LC_SYS_H */
