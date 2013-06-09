/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> lpl_refresh() is more optimistic on the refresh time                                */
/*==============================================================================================*/

/*   LPL_4TCN.H
 ---------------------------------------------------------------------------- 
                                                                              
     Property of  :   ABB Verkehrssyteme AG, CH-8050 ZÅrich / Switzerland     
     COPYRIGHT    :   (c) 1991 ABB Verkehrssysteme AG                         
                                                                              
 ---------------------------------------------------------------------------- 
                                                                              
     Project      :   IEC - TCN                                               
     Subproject   :   Process Data                                            
                                                                              
 ---------------------------------------------------------------------------- 

     File         :   LPL_4TCN.H        Lowest Access                         
                                                                              
     Document     :   [1]: IEC WG 22-5, Upper Layers, Version 1.3             
                      [2]: Link Layer Process Specification                   
                      [3]: Link Layer Process Design                          

     Abstract     :   Low level access routines to traffic store in           
                      procedural and macro implementation                     
                                                                              
                      These functions do different things for different       
                      traffic store models (Switch controlled)                
                                                                              
                      Switch:  O_BAP          BAP  Traffic Store
                               -              MVBC Traffic store
                                                                              
     Remarks      :
                                                                              
     Dependencies :                                                           
                                                                              
     Accepted     :                                                           
                                                                              
 ---------------------------------------------------------------------------- 
                                                                              
     Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Name     Reference
     ----  ------  --------  -------  ----------  -------  ---------          
       1   BAEI-2  91-09-01  created  LPDBR2.TXT  Marsden      --             
                                      LPDBR3.TXT                              
                                                                              
 ---------------------------------------------------------------------------- 
*/

#ifndef         LPL_4TCN_H
#define         LPL_4TCN_H

#include "lp_sys.h"
#include "lp_tmo.h"

#if !defined (O_BAP)
#include    "mvbc.h"
#endif




/*
 ----------------------------------------------------------------------------
|
| constants
|
 ------------   A B S T R A C T   ------------------------------------------- 


  Constants with only local importance

      LPL_SHIFT_BIT_OFFSET_TO_BYTE_OFFSET  bit_offset / 8 = byte_offset
      LPL_MASK_BYTE_OFFSET_IN_BIT_OFFSET enables the operation bit_offset & 7


*/
  
#define         LPL_SHIFT_BIT_OFFSET_TO_BYTE_OFFSET     3
#define         LPL_MASK_BYTE_OFFSET_IN_BIT_OFFSET      7


/* new page
 ---------------------------------------------------------------------------- 
|
| type          TYPE_U_LP_PV_FORMAT
|
 ------------   A B S T R A C T   ------------------------------------------- 
  

  The pv_format can be decoded in different ways.

  The extended type definition extends the size field to 5 bits. It is used
  for the type "8 bit array". The definition is such, that the array size in
  bytes is coded in this extended field as (size - 1).

  The extended type definition is used for the switch "case" table which
  selects the correct bus variable and application variable type.
  The extension is necessary, as the size of the type is in the size field
  overlayed by this extension.
*/


typedef         struct 
{
#if defined (O_LE)
                TYPE_LP_BITFIELD        nothing      : 3;
                TYPE_LP_BITFIELD        ext_size     : 5;
                TYPE_LP_BITFIELD        bit_offset   : 8;
#else
                TYPE_LP_BITFIELD        bit_offset   : 8;
                TYPE_LP_BITFIELD        ext_size     : 5;
                TYPE_LP_BITFIELD        nothing      : 3;
#endif
}               TYPE_LPL_PV_FORMAT_EXT_SIZE;


typedef         struct 
{
#if defined     (O_LE)
                TYPE_LP_BITFIELD        ext_type     : 6;
                TYPE_LP_BITFIELD        nothing      : 2;
                TYPE_LP_BITFIELD        bit_offset   : 8;
#else
                TYPE_LP_BITFIELD        bit_offset   : 8;
                TYPE_LP_BITFIELD        nothing      : 2;
                TYPE_LP_BITFIELD        ext_type     : 6;
#endif
}               TYPE_LPL_PV_FORMAT_EXT_TYPE;

typedef         union
{
                TYPE_LPL_PV_FORMAT_EXT_SIZE     s0;
                TYPE_LPL_PV_FORMAT_EXT_TYPE     s1;

}               TYPE_U_LPL_PV_FORMAT;


/* new page
 ---------------------------------------------------------------------------- 
|
| constant      LP_PCS_...
|
 ------------   A B S T R A C T   ------------------------------------------- 
 
 
  These Constants are used to define PCS configuration and state information

        LPL_PCS_PASSIVE                 Port does not take part in network
        LPL_PCS_SINK                    Port accepts data from bus
        LPL_PCS_SRCE                    Port provides data for bus
        LPL_PCS_BDIR                    Port provides data for bus and appl.

        LPL_PCS_NO_INDICATION

        LPL_PCS_TARGET_INDICATION       Interrupt generated upon trgt update
        LPL_PCS_SOURCE_INDICATION       Interrupt generated upon srce transm

        LPL_PCS_UNMASK_PAGE             Unmask page bit in pcs
*/

#define         LPL_PCS_PASSIVE                 0
#define         LPL_PCS_SINK                    LP_CFG_SINK
#define         LPL_PCS_SRCE                    LP_CFG_SRCE
#define         LPL_PCS_BDIR                    LP_CFG_BIDIRECTIONAL

#define         LPL_PCS_IE_OFF                  0

#if   defined   (O_BAP)
#define         LPL_PCS_IE0                     1
#define         LPL_PCS_IE1                     2
#define         LPL_PCS_IE2                     4
#define         LPL_PCS_IE3                     8
#define         LPL_PCS_UPDATED                 0xFF
#else
#define         LPL_PCS_IE0                     1
#define         LPL_PCS_IE2                     2
#define         LPL_PCS_UPDATED                 0xFFFF
#endif

#define         LPL_PCS_FE_ACT					1
#define         LPL_PCS_FE_PAS					0

#define         LPL_PCS_FCODE_6                 6
#define         LPL_PCS_FCODE_F                 15

#define         LPL_PCS_D_SIZE_IN_PWR2          4

#if defined   (O_BAP)

#define         LPL_PCS_A_SIZE_IN_PWR2          2
#define         LPL_PCS_C_SIZE_IN_PWR2          1

#else
#define         LPL_PCS_B_SIZE_IN_PWR2          3

#endif


#if defined (O_BAP)

typedef struct STR_LPL_PCS_D
{
                unsigned int     reserved0 : 1 ;        /*  byte 0 */
                unsigned int type          : 2 ;
                unsigned int     reserved1 : 1 ;
                unsigned int fcode         : 4 ;
                unsigned int     reserved2 : 8 ;    /*  byte 1 */
                unsigned int     reserved3 : 3 ;    /*  byte 2 */
                unsigned int page          : 1 ;
                unsigned int     reserved4 : 4 ;
                unsigned int tack          : 8 ;    /*  byte 3 */

                unsigned char reserved_a[8];        /* byte 4...11 */
                unsigned char size;                 /* byte 12 */
                unsigned char reserved_b[3];        /* byte 13...15 */
}               TYPE_LPL_PCS_D;

#else

typedef struct STR_LPL_PCS_D
{
                unsigned int     reserved1 : 10;    /*  byte 0 + 1 */
                unsigned int type          : 2 ;
                unsigned int fcode         : 4 ;
                unsigned int     reserved2 : 6 ;    /*  byte 2 + 3 */
                unsigned int page          : 1 ;
                unsigned int     reserved3 : 9 ;

                unsigned short   tack;
                unsigned char    reserved_a[6];     /* byte 6...11 */
                unsigned char size;                 /* byte 12 */
                unsigned char    reserved_b[3];     /* byte 13...15 */
}               TYPE_LPL_PCS_D;

#endif



/* 	new page
 ----------------------------------------------------------------------------
|
| 	macro		lpl_frc_enable
|
 ------------   A B S T R A C T   -------------------------------------------

	sets the "frc bit" for MVBC type PKRs to activate the auto forcing

*/

#define lpl_frc_enable(p_cb, p_pcs, value) \
		{ lpi_put_pcs_mvbc (p_cb, p_pcs, LC_PCS_MSK_FE , LC_PCS_OFF_FE, value); }

#define lpl_num_enable(p_cb, p_pcs, value) \
		{ lpi_put_pcs_mvbc (p_cb, p_pcs, LC_PCS_MSK_NUM, LC_PCS_OFF_NUM, value); }


/*
 ----------------------------------------------------------------------------
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-11-09  created  LPDBR3.TXT  3.3  Marsden      --
 ----------------------------------------------------------------------------
End of function lp_chk_prt_indx_max                                         */


/* new page
 ----------------------------------------------------------------------------
|
| constants     LPL_
| 
 ---------------------------------------------------------------------------- 


  These constants are used by lpl_var_access for the case table which decodes
  the size extende type of the bus variable.

*/

#define         LPL_SIZE_A8_1           (LP_SIZE_A8_1  << 4)
#define         LPL_SIZE_A8_2           (LP_SIZE_A8_2  << 4)
#define         LPL_SIZE_A8_3           (LP_SIZE_A8_3  << 4)
#define         LPL_SIZE_A8_4           (LP_SIZE_A8_4  << 4)
#define         LPL_SIZE_A8_5           (LP_SIZE_A8_5  << 4)
#define         LPL_SIZE_A8_6           (LP_SIZE_A8_6  << 4)
#define         LPL_SIZE_A8_7           (LP_SIZE_A8_7  << 4)
#define         LPL_SIZE_A8_8           (LP_SIZE_A8_8  << 4)

#define         LPL_SIZE_A16_1          (LP_SIZE_A16_1 << 4)
#define         LPL_SIZE_A16_2          (LP_SIZE_A16_2 << 4)
#define         LPL_SIZE_A16_3          (LP_SIZE_A16_3 << 4)
#define         LPL_SIZE_A16_4          (LP_SIZE_A16_4 << 4)

#define         LPL_SIZE_A32_1          (LP_SIZE_A32_1 << 4)
#define         LPL_SIZE_A32_2          (LP_SIZE_A32_2 << 4)

#define         LPL_SIZE_BITS           (LP_SIZE_BITS << 4)
#define         LPL_SIZE_8              (LP_SIZE_8    << 4)
#define         LPL_SIZE_16             (LP_SIZE_16   << 4)
#define         LPL_SIZE_32             (LP_SIZE_32   << 4)
#define         LPL_SIZE_48             (LP_SIZE_48   << 4)
#define         LPL_SIZE_64             (LP_SIZE_64   << 4)

#define         LPL_A8_A                (LPL_SIZE_A8_1 + LP_TYPE_A8_ODD)
#define         LPL_A8_B                (LPL_SIZE_A8_2 + LP_TYPE_A8_EVEN )
#define         LPL_A8_C                (LPL_SIZE_A8_3 + LP_TYPE_A8_ODD  )
#define         LPL_A8_D                (LPL_SIZE_A8_4 + LP_TYPE_A8_EVEN )
#define         LPL_A8_E                (LPL_SIZE_A8_5 + LP_TYPE_A8_ODD  )
#define         LPL_A8_F                (LPL_SIZE_A8_6 + LP_TYPE_A8_EVEN )
#define         LPL_A8_G                (LPL_SIZE_A8_7 + LP_TYPE_A8_ODD  )
#define         LPL_A8_H                (LPL_SIZE_A8_8 + LP_TYPE_A8_EVEN )

#define         LPL_A16_SIGN_A          (LPL_SIZE_A16_1 + LP_TYPE_A16_SIGN )
#define         LPL_A16_SIGN_B          (LPL_SIZE_A16_2 + LP_TYPE_A16_SIGN )
#define         LPL_A16_SIGN_C          (LPL_SIZE_A16_3 + LP_TYPE_A16_SIGN )
#define         LPL_A16_SIGN_D          (LPL_SIZE_A16_4 + LP_TYPE_A16_SIGN )

#define         LPL_A16_CARD_A          (LPL_SIZE_A16_1 + LP_TYPE_A16_CARD )
#define         LPL_A16_CARD_B          (LPL_SIZE_A16_2 + LP_TYPE_A16_CARD )
#define         LPL_A16_CARD_C          (LPL_SIZE_A16_3 + LP_TYPE_A16_CARD )
#define         LPL_A16_CARD_D          (LPL_SIZE_A16_4 + LP_TYPE_A16_CARD )

#define         LPL_A32_SIGN_A          (LPL_SIZE_A32_1 + LP_TYPE_A32_SIGN )
#define         LPL_A32_SIGN_B          (LPL_SIZE_A32_2 + LP_TYPE_A32_SIGN )

#define         LPL_A32_CARD_A          (LPL_SIZE_A32_1 + LP_TYPE_A32_CARD )
#define         LPL_A32_CARD_B          (LPL_SIZE_A32_2 + LP_TYPE_A32_CARD )

#define         LPL_BOOL_1              (LPL_SIZE_BITS + LP_TYPE_BOOL_1)
#define         LPL_BOOL_2              (LPL_SIZE_BITS + LP_TYPE_BOOL_2)
#define         LPL_BCD_4               (LPL_SIZE_BITS + LP_TYPE_BCD_4)

#define         LPL_BITSET_8            (LPL_SIZE_8  + LP_TYPE_BITSET)
#define         LPL_BITSET_16           (LPL_SIZE_16 + LP_TYPE_BITSET)
#define         LPL_BITSET_32           (LPL_SIZE_32 + LP_TYPE_BITSET)
#define         LPL_BITSET_64           (LPL_SIZE_64 + LP_TYPE_BITSET)

#define         LPL_CARD_8              (LPL_SIZE_8  + LP_TYPE_CARD)
#define         LPL_CARD_16             (LPL_SIZE_16 + LP_TYPE_CARD)
#define         LPL_CARD_32             (LPL_SIZE_32 + LP_TYPE_CARD)
#define         LPL_CARD_64             (LPL_SIZE_64 + LP_TYPE_CARD)

#define         LPL_SIGN_8              (LPL_SIZE_8  + LP_TYPE_SIGN)
#define         LPL_SIGN_16             (LPL_SIZE_16 + LP_TYPE_SIGN)
#define         LPL_SIGN_32             (LPL_SIZE_32 + LP_TYPE_SIGN)
#define         LPL_SIGN_64             (LPL_SIZE_64 + LP_TYPE_SIGN)

#define         LPL_TIMEDATE_48         (LPL_SIZE_48 + LP_TYPE_TIMEDATE)
#define         LPL_TIMEDATE_64         (LPL_SIZE_64 + LP_TYPE_TIMEDATE)

#define         LPL_FRAC_200_16         (LPL_SIZE_16 + LP_TYPE_FRAC_200)
#define         LPL_FRAC_400_16         (LPL_SIZE_16 + LP_TYPE_FRAC_400)
#define         LPL_FRAC_800_16         (LPL_SIZE_16 + LP_TYPE_FRAC_800)

#define         LPL_REAL_32             (LPL_SIZE_32 + LP_TYPE_REAL)


/* new page
 ---------------------------------------------------------------------------- 
|
| prototypes
|
 ---------------------------------------------------------------------------- 
*/

void            _lpl_clr_pit            (void *         p_cb);

#define          lpl_clr_pit(p_cb)                                          \
                _lpl_clr_pit(p_cb)

/*              -----------------------------------------------------       */

void            _lpl_put_def_values     (void *         p_cb,
                                         void *         pb_def);

#define          lpl_put_def_values(p_cb, pb_def)                           \
                _lpl_put_def_values(p_cb, pb_def)


/*              -----------------------------------------------------       */

int             _lpl_chk_prt_size               (void *         p_cb,
												 unsigned short prt_indx,
												 unsigned short size);

void            _lpl_put_prt_size               (void *         p_cb,
                                                 void *         p_pcs,
												 unsigned short size);
void            _lpl_tb_put_prt_size            (void *         p_pcs,
                                                 unsigned short size);

#define          lpl_chk_prt_size  _lpl_chk_prt_size

#define          lpl_put_prt_size(p_cb, p_pcs, size)                        \
                         { if (lpc_get_ts_type(p_cb) == LP_TS_TYPE_D)       \
                           { _lpl_tb_put_prt_size (p_pcs, size); } else     \
                           { _lpl_put_prt_size    (p_cb, p_pcs, size); } }



void        _lpl_tb_put_pv_x     (void *                 p_port,
                                 struct STR_LP_PV_X *   p_pv_x,
                                 unsigned short         var_count);

void        _lpl_tb_get_pv_x    (void *                 p_port,
                                 struct STR_LP_PV_X *   p_pv_x,
                                 unsigned short         var_count);


/* new page
 ---------------------------------------------------------------------------- 
|
| macro funct.  lpl_chk_prt_addr_max
|
| param. in     prt_addr                Proposed port address max
| return value  result                  LPS_OK
|                                       LPS_CONFIG
|
 ------------   A B S T R A C T   ------------------------------------------- 


  Checks whether port address max is an allowed value (Count in twos)

*/

#define         lpl_chk_prt_addr_max(p_cb, prt_addr)                         \
                    ((((prt_addr) & LP_PRT_ADDR_MAX_MASK) ==                 \
                                    LP_PRT_ADDR_MAX_MASK) ? LPS_OK: LPS_CONFIG)

/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-11-09  created  LPDBR3.TXT  3.3  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lp_chk_prt_addr_max                                         */

/* new page
 ---------------------------------------------------------------------------- 
|
| macro funct.  lpl_chk_prt_indx_max
|
| param. in     prt_indx                Proposed port index max
|
| return value  result                  LPS_OK
|                                       LPS_CONFIG
|
 ------------   A B S T R A C T   ------------------------------------------- 


  Checks whether port index max is an allowed value (count in fours)

*/

#define         lpl_chk_prt_indx_max(p_cb, prt_indx) \
                    ((((prt_indx) & LP_PRT_INDX_MAX_MASK) ==    \
                        LP_PRT_INDX_MAX_MASK) ? LPS_OK : LPS_CONFIG)

/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-11-09  created  LPDBR3.TXT  3.3  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lp_chk_prt_indx_max                                         */

/* new page
 ---------------------------------------------------------------------------- 
|
| macro funct.  lpl_chk_prt_size_type_d
|
| param. in     size in bytes
|
| return value  result                  LPS_OK
|                                       LPS_CONFIG
|
 ------------   A B S T R A C T   ------------------------------------------- 


  Checks whether port size is an allowed value

*/

#define lpl_chk_prt_size_type_d(p_cb, size)     \
         ( ( (size) <= LPL_SIZEOF_TB_PRT_PGE) ? LPS_OK : LPS_CONFIG)


/* new page
 ---------------------------------------------------------------------------- 
|
| macrofunction lpl_gen_pcs_offset
|
| param. in     p_cb
|               prt_indx
|        out    -
| return value  offset in bytes
| 
 ------------   A B S T R A C T   ------------------------------------------- 

  
  The byte offset between the base of the pcs array and the pcs defined by
  the port index is calculated and returned in bytes.

*/

#define         lpl_gen_pcs_offset(p_cb, prt_indx)                          \
                        ((prt_indx) << lpc_get_pcs_size_in_pwr_2(p_cb) )


/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-09-29  created  LPDBR3.TXT  3.1  Marsden      --   
 ---------------------------------------------------------------------------- 
End of function lpl_gen_pcs_offset                                          */

/* new page
 ---------------------------------------------------------------------------- 
|
| macrofunction lpl_gen_pit_offset
|
| param. in     pit_max
|               pit_type
|
|        out    -
|               
| return value  pit_size                in bytes
|
 ------------   A B S T R A C T   ------------------------------------------- 

  
  The byte offset between the base address of the array port index table and
  the element referenced by the prt_addr is "generated" and returned.

 (((pit_type) == LP_PIT_TYPE_A) ? (prt_addr) : (prt_addr) << 1)

  !!! When implementing the MVBC software, this function should be switch
  controlled.

*/

#define         lpl_gen_pit_offset(p_cb, prt_addr)               \
                        (lpc_get_pit_type(p_cb) == LP_PIT_TYPE_A ? \
                                (prt_addr) : ((prt_addr) << 1))

/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-09-29  created  LPDBR3.TXT  3.1  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpl_gen_pit_offset                                          */

/* new page
 ----------------------------------------------------------------------------- 
| 
| macrofunction lpl_gen_prt_offset
|
| param. in     prt_indx
|
|        out    -
|
| return value  prt_offset
|
 ------------   A B S T R A C T   -------------------------------------------- 


  The offset in bytes of the data port within the data port buffer is
  calculated and returned.

  This is done using following algorithm:

        16 * (prt_indx - (prt_indx % 4)) + 8 * (prt_indx % 4)

*/

#define lpl_gen_prt_offset(p_cb, prt_indx)        \
         (lpc_get_ts_type(p_cb) == LP_TS_TYPE_D ? \
         (prt_indx) << 8 :                        \
         ((((prt_indx) & 0xfffc) << 4) + (((prt_indx) & 0x3) << 3)))

/*
 ----------------------------------------------------------------------------
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  1    BAEI-2  91-09-20  created  LPDBR3.TXT  3.1  Marsden      --
|  2    BATC    95-09-26  ported   LPDBR3.TXT  3.1  Ma Mask extended for MVBC
 ----------------------------------------------------------------------------
End of function lpl_gen_prt_offset                                          */


/* new page
 ---------------------------------------------------------------------------- 
|
| macro funct.  lpl_get_prt_type
|
| param. in     p_pcs
|        out    -
|               
| return value  prt_type                LPL_PCS_SINK
|                                       LPL_PCS_SRCE
|
 ------------   A B S T R A C T   ------------------------------------------- 

  
  The pcs type (source, sink) is returned. As the BAP 15-2/3 does not support
  bidirectional ports, the port type "LPL_PCS_BDIR" is returned as source.

*/

#define lpl_get_prt_type(p_cb, p_type, p_pcs)   \
        { lpi_get_pcs (p_cb, p_type, p_pcs, LC_PCS_MSK_TYPE, LC_PCS_OFF_TYPE); }


/*
 ----------------------------------------------------------------------------
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-09-03  created  LPDBR3.TXT  3.2  Marsden      --
|   2   BATC    94-02-08  mod'fd                    Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpl_get_prt_type                                            */



/* new page
 ---------------------------------------------------------------------------- 
|
| macro funct.  lpl_get_prt_indx
|
| param. in     pb_pit
|               prt_addr
|
|        out    -
|
| return value  prt_indx
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The port index table is the logical connection between the abstract port_
  address (as defined in pv_name) and the physical prt_indx.

  This function reads the prt_indx stored under to the prt_addr in the array
  of character port index table.

*/

#if defined   (O_BAP)
#define         lpl_get_prt_indx_via_pvn(p_cb, p_ix, pb_pit, prt_addr) \
                { *p_ix =  lpc_get_hw_type(p_cb) == LP_HW_TYPE_HDLC ? \
                         (* (((short *) pb_pit) + (prt_addr >> 2))) : \
                         (* (((char  *) pb_pit) +  prt_addr) & 0xFF); }
#define         lpl_get_prt_indx(p_cb, p_ix, pb_pit, prt_addr) \
                { *p_ix =  lpc_get_hw_type(p_cb) == LP_HW_TYPE_HDLC ?  \
                         (* (((short *) pb_pit) +  prt_addr)       ) : \
                         (* (((char  *) pb_pit) +  prt_addr) & 0xFF);  }
#else
    #if defined (O_PCMCIA)
        #define     lpl_get_prt_indx_via_pvn     lpl_get_prt_indx

    #else
        #define     lpl_get_prt_indx_via_pvn(p_cb, p_ix, pb_pit, prt_addr)  \
                    { if (lpc_get_hw_type(p_cb) == LP_HW_TYPE_HDLC)         \
                      { *p_ix = * (((short *) pb_pit) + (prt_addr >> 2)); } \
                      else {lc_get_pit (p_ix, pb_pit, prt_addr); }          }
    #endif
        #define     lpl_get_prt_indx(p_cb, p_ix, pb_pit, prt_addr) \
                          lpi_get_pit (p_cb, p_ix, pb_pit, prt_addr)

#endif



/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-09-29  created  LPDBR3.TXT  3.3  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpl_get_prt_indx                                            */


/* new page
 ---------------------------------------------------------------------------- 
|
| macro funct.  lpl_get_prt_page
|
| param. in     p_pcs
|        out    -
|
| return value  prt_page
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The pcs page bit is read. The page bit indicates, which prt page (0/1)
  contains the valid data. Writers write to invalid page, readers read from
  valid page.

  The page bit is toggled by the writer after a valid update.

*/

#define lpl_get_prt_page(p_cb, p_vp, p_pcs)                     \
        { lpi_get_pcs (p_cb, p_vp, p_pcs, LC_PCS_MSK_VP, LC_PCS_OFF_VP); }

/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-09-29  created  LPDBR3.TXT  3.2  Marsden      --   
|   2   BATC    94-02-08  mod'fd                    Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpl_get_prt_page                                            */


/* new page
 ---------------------------------------------------------------------------- 
|
| macrofunction lpl_get_pge_offset_rd
|
| param. in     p_pcs
|        out    -
|               
| return value  offset in bytes
| 
 ------------   A B S T R A C T   ------------------------------------------- 

  
  The byte offset between the base of the target port (page 0) and the active
  page for reading traffic store data (page 0 or page 1) is returned.

  !!!Attention: Disable interrupts before using this function and reenable them
                interrupts only after completing the data set read operation.

*/

#define lpl_get_pge_offset_rd(p_cb, p_sizeof, p_pcs)                   \
        {unsigned int vp;                                              \
         lpi_get_pcs (p_cb, &vp, p_pcs, LC_PCS_MSK_VP, LC_PCS_OFF_VP); \
         *p_sizeof = vp == 0  ? 0 : sizeof (LC_TYPE_TS_PGE) ;   }


/* new page
 ---------------------------------------------------------------------------- 
|
| macrofunction lpl_tb_get_pge_offset_rd
|
| param. in     p_pcs
|        out    -
|
| return value  offset in bytes
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The byte offset between the base of the target port (page 0) and the active
  page for reading traffic store data of type TB is returned.

  This is 0 bytes or 128 bytes

  !!!Attention: Disable interrupts before using this function and reenable them
                interrupts only after completing the data set read operation.

*/

#define LPL_SIZEOF_TB_PRT_PGE   128

#define lpl_tb_get_pge_offset_rd(p_cb, p_sizeof, p_pcs)         \
        {int vp;                                                \
         lpi_get_pcs (p_cb, &vp, p_pcs, LC_PCS_MSK_VP, LC_PCS_OFF_VP); \
         *p_sizeof = vp == 0  ? 0 : LPL_SIZEOF_TB_PRT_PGE ;   }

#define lpl_tb_get_pcs_size(p_cb, p_size, p_pcs)                \
    { *p_size = ((TYPE_LPL_PCS_D *) (p_pcs))->size; }


/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-10-18  created  LPDBR3.TXT  3.1  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpl_get_pge_offset_rd                                       */


/* new page
 ---------------------------------------------------------------------------- 
|
| macrofunction lpl_get_pge_offset_wt
|
| param. in     p_pcs
|        out    -
|
| return value  offset in bytes
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The byte offset between the base of the target port (page 0) and the active
  page for writing traffic store data (page 0 or page 1) is returned.

  !!!Attention: Disable interrupts before using this function and reenable them
                interrupts only after completing the data set write operation.

*/

#define lpl_get_pge_offset_wt(p_cb, p_sizeof, p_pcs)                   \
        {unsigned int vp;                                              \
         lpi_get_pcs (p_cb, &vp, p_pcs, LC_PCS_MSK_VP, LC_PCS_OFF_VP); \
         *p_sizeof = vp == 0  ? sizeof (LC_TYPE_TS_PGE) : 0 ;   }


/* new page
 ---------------------------------------------------------------------------- 
|
| macrofunction lpl_tb_get_pge_offset_wt
|
| param. in     p_pcs
|        out    -
|
| return value  offset in bytes
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The byte offset between the base of the target port (page 0) and the active
  page for writing traffic store data (page 0 or page 1) is returned.

  !!!Attention: Disable interrupts before using this function and reenable them
                interrupts only after completing the data set write operation.

*/

#define lpl_tb_get_pge_offset_wt(p_cb, p_sizeof, p_pcs)         \
        {int vp;                                                \
         lpi_get_pcs (p_cb, &vp, p_pcs, LC_PCS_MSK_VP, LC_PCS_OFF_VP); \
         *p_sizeof = vp == 0  ? LPL_SIZEOF_TB_PRT_PGE : 0;   }


/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-10-18  created  LPDBR3.TXT  3.1  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpl_get_pge_offset_wt                                       */


/* new page
 ---------------------------------------------------------------------------- 
|
| macro funct.  lpl_get_prt_size
|
| param. in     p_pcs
|        out    -
|
| return value  prt_size in 16 bits
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The pcs fcode bitfield which contains the port size is read and decoded. The
  port size in 16 bit units is returned.

  0/1/2/3/4: 1/2/4/8/16 16-bit words

  7        : size is in size_fcode_7 field in 16 bit words

*/
#define lpl_get_pcs_fcode(p_cb, p_fcode, p_pcs)   \
        { lpi_get_pcs (p_cb, p_fcode, p_pcs, LC_PCS_MSK_FCODE, LC_PCS_OFF_FCODE); }

#define lpl_get_prt_size(p_cb, p_sizeof, p_pcs)                     \
        { unsigned int fcode;                                       \
          lpl_get_pcs_fcode(p_cb, &fcode, p_pcs);                   \
          if (fcode == 15) { *p_sizeof = 1; } else                  \
          { *p_sizeof = lpc_get_hw_type(p_cb) == LP_HW_TYPE_HDLC ?  \
            (*(((unsigned char *) p_pcs) + 12) >> 1) : 1 << fcode ; }}



/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-09-03  created  LPDBR3.TXT  3.2  Marsden      --
|   2   BATC    94-02-08  mod'fd                    Marsden      --
|   3   BATC    95-10-30  mod'fd   Fcode 15         Marsden      --
 ----------------------------------------------------------------------------
End of function lpl_get_prt_size                                            */



/* new page
 ---------------------------------------------------------------------------- 
|
| macro funct.  lpl_get_prt_tack
|
| param. in     p_pcs
|        out    -
|
| return value  prt_tack
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The pcs tack bits are read. This function will be called by lp_refresh ().

*/

#define lpl_get_prt_tack(p_cb, p_tack, p_pcs)   \
        { lpi_get_pcs (p_cb, p_tack, p_pcs, LC_PCS_MSK_TACK, LC_PCS_OFF_TACK); }


/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-09-29  created  LPDBR3.TXT  3.2  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpl_get_prt_tack                                            */


/* new page
 ---------------------------------------------------------------------------- 
|
| macro funct.  lpl_inc_prt_tack
|
| param. in     p_pcs
|
|        out    -
|               
| return value  -
|
 ------------   A B S T R A C T   -------------------------------------------

  
  The tack (transfer acknowledge) field in the pcs is incremented.

  This function will generally be used to write the update the time since the
  last bus transmission.

  The resolution of the counter update is defined by the constant LP_TMO_CYCLE
  which describes the frequency of the function call.

*/  

#if defined (O_BAP)
#define lpl_inc_prt_tack(p_cb, p_pcs)   \
        {  lpi_inc_pcs (p_cb, p_pcs, LC_PCS_MSK_TACK, LC_PCS_OFF_TACK); }

#else
#define lpl_inc_prt_tack(p_cb, p_pcs)                                      \
{  unsigned int tack;                                                      \
     lpi_get_pcs (p_cb, &tack, p_pcs, LC_PCS_MSK_TACK, LC_PCS_OFF_TACK);   \
     tack = tack - 64;                                                     \
     if (tack < LP_TMO_MAX_COUNT) { tack = LP_TMO_MAX_COUNT; }             \
     lpi_put_pcs (p_cb, p_pcs, LC_PCS_MSK_TACK, LC_PCS_OFF_TACK, tack);    }
#endif

/*
 ----------------------------------------------------------------------------
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-10-23  created  LPDBR2.TXT  2.1  Marsden      --
|   2   BATC    95-10-30  MDFD     MVBC emu         Marsden
 ----------------------------------------------------------------------------
End of function lpl_inc_prt_tack                                            */


/* new page
 ---------------------------------------------------------------------------- 
|
| macro funct.  lpl_put_prt_fcode
|
| param. in     p_pcs
|               value
|        out    -
|
| return value  -
| 
 ------------   A B S T R A C T   ------------------------------------------- 


  The pcs fcode is written.


*/

#define lpl_put_prt_fcode(p_cb, p_pcs, value)            \
	    { lpi_put_pcs (p_cb, p_pcs, LC_PCS_MSK_FCODE, LC_PCS_OFF_FCODE, value); }



/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  92-09-09  created  LPDBR2.TXT  2.1  Marsden      --
|   2   BATC    94-02-09  mod'fd                    Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpl_put_prt_fcode                                           */

/* new page
 ---------------------------------------------------------------------------- 
|
| macro funct.  lpl_put_prt_ie_x
|
| param. in     p_pcs
|               value
|        out    -
|
| return value  -
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The pcs ie_x is written.


*/
#if defined (O_BAP)
#define lpl_put_prt_ie_x(p_cb, p_pcs, value)                               \
		{ lpi_put_pcs_bap  (p_pcs, LC_PCS_MSK_IEX, LC_PCS_OFF_IEX, value); }
#else
#define lpl_put_prt_ie_x(p_cb, p_pcs, value)                               \
		{ lpi_put_pcs_mvbc (p_cb, p_pcs, LC_PCS_MSK_DTI, LC_PCS_OFF_DTI, value); }
#endif


/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  92-09-09  created  LPDBR2.TXT  2.1  Marsden      --
|   2   BATC    94-02-09  mod'fd                    Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpl_put_prt_ie_x                                           */

/* new page
 ---------------------------------------------------------------------------- 
|
| name          lpl_put_prt_indx
|
| param. in     pb_pit                  pointer to port index table
|               prt_addr                port address
|               prt_indx                port index
|
|        out    -
|
| return value  -
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The 8 bit port index table is loaded with the port index under port address.

*/

#if defined     (O_BAP)

#define         lpl_put_prt_indx(p_cb, pb_pit, prt_addr, prt_indx) \
                        if (lpc_get_hw_type(p_cb) == LP_HW_TYPE_HDLC) \
                        {  * (((short *) pb_pit) + prt_addr) = prt_indx; }  \
                        else { * (((char *) pb_pit) + prt_addr)     = prt_indx; }
#else

#define         lpl_put_prt_indx(p_cb, pb_pit, prt_addr, prt_indx) \
                {  lpi_put_pit (p_cb, pb_pit, prt_addr, prt_indx); }

#endif


/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-10-07  created  LPDBR3.TXT  3.3  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpl_put_prt_indx   */



/* new page
 ---------------------------------------------------------------------------- 
|
| macro funct.  lpl_put_prt_tack
|
| param. in     p_pcs
|               value
|        out    -
|
| return value  -
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The pcs tack bits are written.

  Generally, this function will be called by the lp_put_... routine to
  emulate the cyclic BAP update mechanism, which writes this field to "FF".

*/

#define lpl_put_prt_tack(p_cb, p_pcs, value)    \
        { lpi_put_pcs (p_cb, p_pcs, LC_PCS_MSK_TACK, LC_PCS_OFF_TACK, value); }


/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-11-09  created  LPDBR2.TXT  2.1  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpl_put_prt_tack                                            */

/* new page
 ---------------------------------------------------------------------------- 
| 
| function      lpl_put_prt_type
|
| param. in     p_pcs
|               prt_flow
|        out    -
|               
| return value  result          LPS_OK
|                               LPS_CONFIG      unknown type
|
 ------------   A B S T R A C T   ------------------------------------------- 

  
  The pcs type (LPL_PCS_SRCE, LPL_PCS_SINK, LPL_PCS_BDIR) is written
  to the pcs.

*/

#define lpl_put_prt_type(p_cb, p_pcs, value)    \
		{ unsigned short temp = value == LPL_PCS_BDIR ? LPL_PCS_SRCE : value;\
		  lpi_put_pcs (p_cb, p_pcs, LC_PCS_MSK_TYPE, LC_PCS_OFF_TYPE, temp);}


/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-09-03  created  LPDBR3.TXT  3.2  Marsden      --
|   2   BATC    94-02-09  mod'fd                    Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpl_put_prt_type                                            */

/*
 ---------------------------------------------------------------------------- 
|
| name          lpl_refresh
|
| param. in     p_pcs
|
|        out    p_refresh               Time in ms since last update
|
| return value  -
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The "tack" byte contains the time which has passed since the last update
  in 64 ms ticks. The start value is -1.

  This routine reads the TACK. The value is incremented to compensate the
  start value. The value is multiplied by 64 to calculate the time since the
  last update in ms. This value is stored in the variable "p_refresh". This
  variable will generally be provided by the application.

*/
#if defined (O_BAP)
#define lpl_refresh(p_cb, p_pcs, p_refresh)                           \
        {unsigned char temp;                                          \
         lpi_get_pcs (p_cb, &temp, p_pcs, LC_PCS_MSK_TACK, LC_PCS_OFF_TACK); \
         * (unsigned short *) p_refresh = temp == 0xFF ?              \
         0 : (((unsigned short) temp) << 6); }
#else
/* <ATR:01> */
/* #define lpl_refresh(p_cb, p_pcs, p_refresh)                           		\ */
/*         {unsigned int temp;                                           		\ */
/*          lpi_get_pcs (p_cb, &temp, p_pcs, LC_PCS_MSK_TACK, LC_PCS_OFF_TACK); \ */
/*          *p_refresh = ~(unsigned short) temp; }                                */
#define lpl_refresh(p_cb, p_pcs, p_refresh) {                          \
	unsigned short temp;                                               \
    lpi_get_pcs(p_cb, &temp, p_pcs, LC_PCS_MSK_TACK, LC_PCS_OFF_TACK); \
    temp = ~temp;                                                      \
    *p_refresh = temp > 64 ? temp : 0;                                 \
}
#endif

/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  -------^--
|  0.1  BAEI-2  91-09-03  created  LPDBR2.TXT  2.1  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpl_refresh                                                 */


/* new page
 ---------------------------------------------------------------------------- 
|
| macro funct.  lpl_tgl_prt_page
|                                                                             
| param. in     p_pcs
|        out    -
|
| return value  -
| 
 ------------   A B S T R A C T   ------------------------------------------- 

  
  The pcs page bit is toggled. The page bit indicates, which prt page (0/1)
  contains the valid data. Writers write to invalid page, readers read from
  valid page. This function must only be used by a writer.

*/  

#define         lpl_tgl_prt_page(p_cb, p_pcs)   \
				 { lpi_tgl_pcs(p_cb, p_pcs, LC_PCS_MSK_VP, LC_PCS_OFF_VP); }


/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-09-29  created  LPDBR3.TXT  3.2  Marsden      --
|   2   BATC    94-02-09  mod'fd                    Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpl_tgl_prt_page                                            */


/*
 ---------------------------------------------------------------------------- 
|
| Version x.x   Modifications
| 
 ---------------------------------------------------------------------------- 


  1) 93-11-09   Hoepli CRBC-1  switch for big endian added

  2) 93-11-30   Ma             lpl_put_prt_indx
                               lpl_get_prt_indx


 ----------------------------------------------------------------------------

                                                    Nr: 3   dated:  94-01-14

		Switch order changed, O_PC with O_TCN (no longer O_EKR)

 ----------------------------------------------------------------------------

                                                    Nr: 4   dated:  94-01-27

        New function lpl_tb_get_pge_offset_rd()

 ----------------------------------------------------------------------------

                                                    Nr: 5   dated:  94-02-09

        MVBC Migration

 ----------------------------------------------------------------------------

                                                    Nr: 6   dated:  94-06-14

        Location of force mask and force data swapped because of MVBC

 ----------------------------------------------------------------------------

                                                    Nr: 7   dated:  94-09-24

        lpl_refresh modified to 64 ms resolution, 0/-1LSB accuracy


 ----------------------------------------------------------------------------

                                                    Nr: 8   dated:  95-09-

        lpl_gen_prt_offset: Prt Index Mask modified 


*/
#endif
