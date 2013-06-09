/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> Included "atr_sys.h"                                                                */
/*==============================================================================================*/

/* <ATR:01> */
#include "atr_sys.h"


/*
 ----------------------------------------------------------------------------- 
                                                                               
     Property of  :   ABB Verkehrssyteme AG, CH-8050 Zrich / Switzerland      
     COPYRIGHT    :   (c) 1991 ABB Verkehrssysteme AG                          
                                                                               
 ----------------------------------------------------------------------------- 
                                                                               
     Project      :   MicOS                                                    
     Subproject   :   Link Process                                             
                                                                               
 ----------------------------------------------------------------------------- 
                                                                               
     File         :   LP_SYS.H                                                 
                                                                               
     Document     :   [1] IEC Document WG 22-5 "Upper Layers", Version 1.3     
                      [2] Link Layer Process Specification                     
                      [3] Link Layer Process Design                            
                                                                               
     Abstract     :   Interface Application - Link Layer Process
                      Interface MicOS Boot  - Link Layer Process
                                                                               
     Remarks      :   Version History explanations at end of file              
                                                                               
     Dependencies :                                                            
                                                                               
     Accepted     :                                                            
                                                                               
 ----------------------------------------------------------------------------- 
 
     HISTORY:
 
     Version  YY-MM-DD  Name        Dept.   Status
     -------  --------  ----------  ------  -------
     SEE end of file                                                                          
                                                                               
 -----------------------------------------------------------------------------
*/


#if defined (__cplusplus)
 extern "C"
 {
#endif


#ifndef         LP_SYS_H
#define         LP_SYS_H

#if defined (O_ICM) && !defined (O_USE_VARIABLE)
#include "vcos_sys.h"
#endif

                                       #if !defined PD_T_RESULT
#define PD_T_RESULT
typedef int                 TYPE_PD_RESULT;
                                       #endif


#if defined (GD_SUN) || defined (O_960) || defined (O_TRACS)
        typedef         unsigned short    TYPE_LP_BITFIELD;
#else
        typedef         unsigned int      TYPE_LP_BITFIELD;
#endif

/*
 -----------------------------------------------------------------------------
|
|  Macro        pv_name
|
 ------------   A B S T R A C T   --------------------------------------------


   Macros to initialise and access the different types of LP pv name
   (little endian / big endian, 16 and 32 bit aligned bitfields )

*/

#if defined (O_BAP) || defined (O_960)
#define lp_put_pvn_pvf(pv_name,member,val) {(pv_name)->pv_format.member = val;}
#define lp_put_pvn_dsn(pv_name,member,val) {(pv_name)->ds_name.member   = val;}
#define lp_get_pvn_pvf(pv_name,member    ) ((pv_name)->pv_format.member )
#define lp_get_pvn_dsn(pv_name,member    ) ((pv_name)->ds_name.member   )
#else
#define lp_put_pvn_pvf(pv_name,member,val) {(pv_name)->member = val;}
#define lp_put_pvn_dsn(pv_name,member,val) {(pv_name)->member = val;}
#define lp_get_pvn_pvf(pv_name,member    ) ((pv_name)->member)
#define lp_get_pvn_dsn(pv_name,member    ) ((pv_name)->member)
#endif


/*  These macros generate the old "prototype" pv_name structure */
#if defined (O_BAP) || defined (O_960)
#if defined (O_LE)
#define lp_init_pvn(dsn, pvf)           {{pvf},{dsn}}
#define lp_init_dsn(ts_id, prt_addr  )   prt_addr, ts_id
#define lp_init_pvf(bit_o, size, type)   type, size, bit_o
#else
#define lp_init_pvn(dsn, pvf)           {{dsn}, {pvf}}
#define lp_init_dsn(ts_id, prt_addr  )   ts_id, prt_addr
#define lp_init_pvf(bit_o, size, type)   bit_o, size, type
#endif
#else
#if defined (O_LE)
#define lp_init_pvn(dsn, pvf)           {pvf, dsn}
#define lp_init_dsn(ts_id, prt_addr  )  prt_addr, ts_id
#define lp_init_pvf(bit_o, size, type)  type, size, bit_o
#else
#define lp_init_pvn(dsn, pvf)           {dsn, pvf}
#define lp_init_dsn(ts_id, prt_addr  )  ts_id, prt_addr
#define lp_init_pvf(bit_o, size, type)  bit_o, size, type
#endif
#endif


/*
 -----------------------------------------------------------------------------
| 
| Chapter       union of basic "c" types
|
 ------------   A B S T R A C T   --------------------------------------------


  This union allows an 8-bit, 16-bit or 32 bit access to a variable.

*/

union           UNN_LP_BASIC
{
                unsigned char   uc[1];
                         char   sc[1];
                unsigned short  us[1];
                         short  ss[1];
                unsigned long   ul[1];
                         long   sl[1];
};


/*
 -----------------------------------------------------------------------------
| 
| constant     LP_PUT, GET
|
 ----------------------------------------------------------------------------- 


  These constants are used to define the data flow direction. The constants are
  compatible to LP_SRCE (put) and LP_SINK (get). They are used to define the
  operation in macro functions

*/


#define         LP_GET                  1
#define         LP_PUT                  2


/*
 ----------------------------------------------------------------------------- 
| 
| constants     LP_..._MAX
|
 ------------   A B S T R A C T   -------------------------------------------- 
  

  These constants define range boundaries

*/

#if                                     defined (O_EKR)
        #define LP_TS_ID_MAX            7
  #elif                                 defined (O_RP) || defined (O_KHW)
        #define LP_TS_ID_MAX            3
  #else
        #define LP_TS_ID_MAX            15
#endif

#define         LP_HW_TYPE_MAX          63
#define         LP_PIT_TYPE_MAX          1

#define         LP_TS_ARRAY_INDEX       (LP_TS_ID_MAX + 1)


/*
 ----------------------------------------------------------------------------- 
| 
| constant      LP_FUP_....
|
 ------------   A B S T R A C T   -------------------------------------------- 
 
 
  Constants are employed to define the "TRUE" and "FALSE" bus values,
  which are compiler independant.


  LP_FUP_TRUE   is defined as bit0 == 1

*/
  
#define         LP_FUP_FALSE            0
#define         LP_FUP_TRUE             1




/* new page
 ---------------------------------------------------------------------------- 
|
| Chapter 3.1   TYPE_PV_FORMAT
|
 ------------   A B S T R A C T   -------------------------------------------

  Parameters required to identify a bus data variable.

*/
typedef         struct                  STR_PV_FORMAT
{
#if defined (O_LE)
                TYPE_LP_BITFIELD        type        : 4;
                TYPE_LP_BITFIELD        size        : 4;
                TYPE_LP_BITFIELD        bit_offset  : 8;
#else
                TYPE_LP_BITFIELD        bit_offset  : 8;
                TYPE_LP_BITFIELD        size        : 4;
                TYPE_LP_BITFIELD        type        : 4;
#endif
}                                       TYPE_PV_FORMAT;


/*
 ----------------------------------------------------------------------------- 
| 
| Chapter 3.1   TYPE_DS_NAME
|
 ------------   A B S T R A C T   -------------------------------------------- 
  
  Parameters required to identify a bus data set. 

*/

typedef struct                  STR_DS_NAME
{
#if defined (O_LE)
                TYPE_LP_BITFIELD        prt_addr    : 12;
                TYPE_LP_BITFIELD        ts_id       : 4;
#else
                TYPE_LP_BITFIELD        ts_id       : 4;
                TYPE_LP_BITFIELD        prt_addr    : 12;
#endif
}                                       TYPE_DS_NAME;

typedef struct  STR_DS_NAME_TB
{
                TYPE_LP_BITFIELD        reserved    : 2 ;
                TYPE_LP_BITFIELD        prt_addr    : 10;
                TYPE_LP_BITFIELD        ts_id       : 4;
}               TYPE_DS_NAME_TB;


/*
 -----------------------------------------------------------------------------
|
| Chapter 3.1   TYPE_PV_NAME
|
 ------------   A B S T R A C T   --------------------------------------------

  Parameters required to identify a bus data variable within a device.

*/

#if defined (O_BAP) || defined (O_960)
typedef struct  STR_PV_NAME
{
#if defined (O_LE)
                struct STR_PV_FORMAT    pv_format   ;
                struct STR_DS_NAME      ds_name     ;
#else
                struct STR_DS_NAME      ds_name     ;
                struct STR_PV_FORMAT    pv_format   ;
#endif
}               TYPE_PV_NAME;

#else
#if defined (O_LE)
typedef struct  STR_PV_NAME
{
                TYPE_LP_BITFIELD        type        : 4;
                TYPE_LP_BITFIELD        size        : 4;
                TYPE_LP_BITFIELD        bit_offset  : 8;
                TYPE_LP_BITFIELD        prt_addr    : 12;
                TYPE_LP_BITFIELD        ts_id       : 4;
}               TYPE_PV_NAME;
#else
typedef struct  STR_PV_NAME
{               TYPE_LP_BITFIELD        ts_id       : 4;
                TYPE_LP_BITFIELD        prt_addr    : 12;
                TYPE_LP_BITFIELD        bit_offset  : 8;
                TYPE_LP_BITFIELD        size        : 4;
                TYPE_LP_BITFIELD        type        : 4;
}               TYPE_PV_NAME;
#endif
#endif


typedef struct  STR_PV_NAME_TB
{
                TYPE_LP_BITFIELD        type        : 4;
                TYPE_LP_BITFIELD        size        : 4;
                TYPE_LP_BITFIELD        bit_offset  : 10;
                TYPE_LP_BITFIELD        prt_addr    : 10;
                TYPE_LP_BITFIELD        ts_id       : 4;

}               TYPE_PV_NAME_TB;

typedef union   UNN_PV_NAME_TB_VB
{
                TYPE_PV_NAME            vb;
                TYPE_PV_NAME_TB         tb;

}               TYPE_PV_NAME_TB_VB;


/*
 ----------------------------------------------------------------------------- 
|
| Chapter 3.1   TYPE_LP_TS_...
|
 ------------   A B S T R A C T   -------------------------------------------- 

  Traffic Store components, memory size definitions for applications.

*/

typedef         struct          STR_LP_PIT_A
{
				unsigned char   prt_addr;

}                               TYPE_LP_PIT_A;


typedef         struct          STR_LP_PIT_B
{
				unsigned short  prt_addr;

}                               TYPE_LP_PIT_B;


#define                         STR_LP_PIT_C            STR_LP_PIT_A
#define                         STR_LP_PIT_D            STR_LP_PIT_B

typedef         struct          STR_LP_PCS_A
{
				unsigned char   pcs_byte[4];

}                               TYPE_LP_PCS_A;


typedef         struct          STR_LP_PCS_B
{
				unsigned char   pcs_byte[8];

}                               TYPE_LP_PCS_B;


typedef         struct          STR_LP_PCS_C
{
				unsigned char   pcs_byte[2];

}                               TYPE_LP_PCS_C;

typedef         struct          STR_LP_PCS_D
{
				unsigned char   pcs_byte[16];

}                               TYPE_LP_PCS_D;

typedef         struct          STR_LP_PGE_A
{
				unsigned char   prt_byte[8];

}                               TYPE_LP_PGE_A;

typedef         struct          STR_LP_PGE_D
{
				unsigned char   prt_byte[128];

}                               TYPE_LP_PGE_D;

typedef         struct          STR_LP_PRT_A
{
				unsigned char   prt_byte[16];

}                               TYPE_LP_PRT_A;

#define         STR_LP_PRT_B    STR_LP_PRT_A
#define         STR_LP_PRT_C    STR_LP_PRT_A

typedef         struct          STR_LP_PRT_D
{
				unsigned char   prt_byte[256];

}                               TYPE_LP_PRT_D;

#define         STR_LP_FRC_A    STR_LP_PRT_A
#define         STR_LP_FRC_B    STR_LP_PRT_A
#define         STR_LP_FRC_C    STR_LP_PRT_A
#define         STR_LP_FRC_D    STR_LP_PRT_D

#define         STR_LP_DEF_A    STR_LP_PGE_A
#define         STR_LP_DEF_B    STR_LP_PGE_A
#define         STR_LP_DEF_C    STR_LP_PGE_A
#define			STR_LP_DEF_D	STR_LP_PGE_D

typedef         struct          STR_LP_MWD
{
				unsigned short  m_accs;
				unsigned short  m_word;

}                               TYPE_LP_MWD;


#define         STR_LP_MWD_A    STR_LP_MWD
#define         STR_LP_MWD_B    STR_LP_MWD
#define         STR_LP_MWD_C    STR_LP_MWD


/* new page
 ----------------------------------------------------------------------------- 
|
| Chapter 3.2   LPS_......
|
 ----------------------------------------------------------------------------- 


  Link Level Status Messages (function values).

        LPS_OK                          Command successfully executed
        LPS_PRT_PASSIVE                 ... Warning: no copying for this var
        LPS_ERROR                       ... Error  : general error (-1)
        LPS_CONFIG                      ... Error  : configuration error
        LPS_MEMORY                      ... Error  : Allocation error
        LPS_UNKNOWN_TS_ID               ... Error  : ts out of range
                                                     ts not created
        LPS_RANGE                       ... port address > port address max
                                        ... port index > port index max
                                        ... indication nr > indication nr max
                                        ... hw_type > hw_type_max
        LPS_UNKNOWN_DATA_TYPE           ... undefined data type used
*/

#define         LPS_ERROR               -1
#define         LPS_OK                  0
#define         LPS_PRT_PASSIVE         1
#define         LPS_PRT_ADDR_0          2
#define         LPS_CONFIG              3
#define         LPS_MEMORY              4
#define         LPS_RANGE               5
#define         LPS_UNKNOWN_TS_ID       6
#define         LPS_UNKNOWN_DATA_TYPE   7


/*
 ----------------------------------------------------------------------------- 
| 
| Chapter 3.2   LP_SIZE_...
|
 ----------------------------------------------------------------------------- 

  These constants are used to configure PV_NAME.

*/

                                                #ifndef         TC_IGNORE
#define         LP_SIZE_BITS                    0x0
#define         LP_SIZE_8                       0x0
#define         LP_SIZE_16                      0x1
#define         LP_SIZE_32                      0x2
#define         LP_SIZE_48                      0x3
#define         LP_SIZE_64                      0x4


#define         LP_SIZE_A8_1                    0x0
#define         LP_SIZE_A8_2                    0x0
#define         LP_SIZE_A8_3                    0x1
#define         LP_SIZE_A8_4                    0x1
#define         LP_SIZE_A8_5                    0x2
#define         LP_SIZE_A8_6                    0x2
#define         LP_SIZE_A8_7                    0x3
#define         LP_SIZE_A8_8                    0x3
#define         LP_SIZE_A8_9                    0x4
#define         LP_SIZE_A8_10                   0x4
#define         LP_SIZE_A8_11                   0x5
#define         LP_SIZE_A8_12                   0x5
#define         LP_SIZE_A8_13                   0x6
#define         LP_SIZE_A8_14                   0x6
#define         LP_SIZE_A8_15                   0x7
#define         LP_SIZE_A8_16                   0x7
#define         LP_SIZE_A8_17                   0x8
#define         LP_SIZE_A8_18                   0x8
#define         LP_SIZE_A8_19                   0x9
#define         LP_SIZE_A8_20                   0x9
#define         LP_SIZE_A8_21                   0xA
#define         LP_SIZE_A8_22                   0xA
#define         LP_SIZE_A8_23                   0xB
#define         LP_SIZE_A8_24                   0xB
#define         LP_SIZE_A8_25                   0xC
#define         LP_SIZE_A8_26                   0xC
#define         LP_SIZE_A8_27                   0xD 
#define         LP_SIZE_A8_28                   0xD
#define         LP_SIZE_A8_29                   0xE
#define         LP_SIZE_A8_30                   0xE
#define         LP_SIZE_A8_31                   0xF
#define         LP_SIZE_A8_32                   0xF
/* new page */
#define         LP_SIZE_A16_1                   0x0
#define         LP_SIZE_A16_2                   0x1
#define         LP_SIZE_A16_3                   0x2
#define         LP_SIZE_A16_4                   0x3
#define         LP_SIZE_A16_5                   0x4
#define         LP_SIZE_A16_6                   0x5
#define         LP_SIZE_A16_7                   0x6
#define         LP_SIZE_A16_8                   0x7
#define         LP_SIZE_A16_9                   0x8
#define         LP_SIZE_A16_10                  0x9
#define         LP_SIZE_A16_11                  0xA
#define         LP_SIZE_A16_12                  0xB
#define         LP_SIZE_A16_13                  0xC
#define         LP_SIZE_A16_14                  0xD
#define         LP_SIZE_A16_15                  0xE
#define         LP_SIZE_A16_16                  0xF 

#define         LP_SIZE_A32_1                   0x1
#define         LP_SIZE_A32_2                   0x3
#define         LP_SIZE_A32_3                   0x5
#define         LP_SIZE_A32_4                   0x7
#define         LP_SIZE_A32_5                   0x9
#define         LP_SIZE_A32_6                   0xB
#define         LP_SIZE_A32_7                   0xD
#define         LP_SIZE_A32_8                   0xF


                                                #endif
/* new page
 ----------------------------------------------------------------------------- 
| 
| Chapter 3.2  LP_TYPE_...
|
 ----------------------------------------------------------------------------- 


  These constants are used to configure PV_NAME.

*/

                                                #ifndef         TC_IGNORE

#define         LP_TYPE_BOOL_1                  0x0
#define         LP_TYPE_BOOL_2                  0x1
#define         LP_TYPE_BCD_4                   0x2
#define         LP_TYPE_CHAR                    0x7
#define         LP_TYPE_TIMEDATE                0x2
#define         LP_TYPE_REAL                    0x3
#define         LP_TYPE_BITSET                  0x4
#define         LP_TYPE_CARD                    0x5
#define         LP_TYPE_SIGN                    0x6

#define         LP_TYPE_FRAC_200                0x8
#define         LP_TYPE_FRAC_400                0x9
#define         LP_TYPE_FRAC_800                0xA

#define         LP_TYPE_A8_EVEN                 0xF
#define         LP_TYPE_A8_ODD                  0x7

#define         LP_TYPE_A16_CARD                0xD
#define         LP_TYPE_A16_SIGN                0xE

#define         LP_TYPE_A32_CARD                0xB
#define         LP_TYPE_A32_SIGN                0xC

                                                #endif

/*
 ----------------------------------------------------------------------------- 
| 
| Chapter 3.2   LP_TS_TYPE_...
|
 ----------------------------------------------------------------------------- 

  These constants are used to identify the TS structure types

*/

#define         LP_TS_TYPE_A            0x0000          /* MVB, BAP 15-2/3  */
#define         LP_TS_TYPE_B            0x0001          /* MVB, MVBC        */
#define         LP_TS_TYPE_C            0x0002          /* MIC, DP-RAM      */
#define         LP_TS_TYPE_D            0x0003          /* WTB              */

#define         LP_PIT_TYPE_A           0x0000          /*  8 Bit Port Indx */
#define         LP_PIT_TYPE_B           0x0001          /* 16 Bit Port Indx */
#define         LP_PIT_TYPE_C           LP_PIT_TYPE_A   /*  8 Bit Port Indx */
#define			LP_PIT_TYPE_D			LP_PIT_TYPE_B

#define         LP_PCS_TYPE_A           0x0000          /*  4 Byte PCS      */
#define         LP_PCS_TYPE_B           0x0001          /*  8 Byte PCS      */
#define         LP_PCS_TYPE_C           0x0002          /*  2 Byte PCS      */
#define			LP_PCS_TYPE_D			0x0003			/* 16 Byte PCS      */

#define         LP_PRT_TYPE_A           0x0000          /*  4 x 8 Byte Port */
#define         LP_PRT_TYPE_B           LP_PRT_TYPE_A   /*  4 x 8 Byte Port */
#define         LP_PRT_TYPE_C           LP_PRT_TYPE_A   /*  4 x 8 Byte Port */
#define			LP_PRT_TYPE_D			0x0003

#define         LP_MAGIC_WORD           0xA55A
#define         LP_MBOX_DELAY           500             /*  500 ms          */


/* new page
 ----------------------------------------------------------------------------- 
|
| Chapter 3.2   LP_HW_TYPE_.....
|
 ------------   A B S T R A C T   -------------------------------------------- 
  
  These constants are used to identify the bus (controller)

                                        IEC normed busses (1 ..... 31)
        LP_HW_TYPE_BAP                  MVB old Bus Controller
        LP_HW_TYPE_MVBC                 MVB new Bus Controller
        LP_HW_TYPE_HDLC                 DVB / WTB

                                        Additional busses (32 ..... 63)
        LP_HW_TYPE_MIC_0                No Bus (i.e. intertask commu)
        LP_HW_TYPE_MIC_1                AMS_BUS
        LP_HW_TYPE_MIC_2                P_BUS
        LP_HW_TYPE_MIC_3                Serial EEPROM

*/

#define         LP_HW_TYPE_BAP          0x0000
#define         LP_HW_TYPE_MVBC         0x0001

#define         LP_HW_TYPE_HDLC         9

#define         LP_HW_TYPE_MIC_0        0x0020
#define         LP_HW_TYPE_MIC_1        0x0021
#define         LP_HW_TYPE_MIC_2        0x0022
#define         LP_HW_TYPE_MIC_3        0x0023
#define         LP_HW_TYPE_PCM_0        0x0024



/* new page
 ----------------------------------------------------------------------------- 
|
| Chapter 3.2   LP_P_MVB_....
|
 ------------   A B S T R A C T   --------------------------------------------

  These constants are used for hardware dependant Traffic Store locations

        LP_P_MVB_PIT                    Pointer to MVB Port Index Table
		LP_P_MVB_PCS                    Pointer to MVB Port Commu & Status Reg
        LP_P_MVB_PRT                    Pointer to MVB Data Port Array

*/


#if                                     defined (O_RP) || defined (O_KHW)

        #define LP_P_TS                 0xE000
        #define LP_P_MVB_PIT            (0x1000 + LP_P_TS)
        #define LP_P_MVB_PCS            (0x0C00 + LP_P_TS)
        #define LP_P_MVB_PRT            (0x0000 + LP_P_TS)

#elif                                   defined (O_EKR)

        #define LP_P_TS                 0x0L
        #define LP_P_MVB_PIT            (0x00001000L + LP_P_TS)
        #define LP_P_MVB_PCS            (0x00000C00L + LP_P_TS)
        #define LP_P_MVB_PRT            (0x00002000L + LP_P_TS)

#endif




/* new page
 -----------------------------------------------------------------------------
|
| Chapter 4.1   LP_CFG_....
|
 -----------------------------------------------------------------------------

  The port index table can take 8 bit and 16 bit port indexes. The type of
  port index is defined with these constants.

  Configuration constants for PCS size and PCS type (data flow direction)

*/

                                                #ifndef         TC_IGNORE

#define         LP_CFG_TS_ALIEN                  0
#define         LP_CFG_TS_OWNED                  1

#define         LP_CFG_XX_BYTES_MSK              0x7F
#define         LP_CFG_02_BYTES_DSW              (128 + LP_CFG_02_BYTES)
#define         LP_CFG_02_BYTES                  2
#define         LP_CFG_04_BYTES                  4
#define         LP_CFG_08_BYTES                  8
#define         LP_CFG_16_BYTES                 16
#define         LP_CFG_32_BYTES                 32
#define         LP_CFG_128_BYTES               128

#define         LP_CFG_PASSIVE                   0

                                                #endif

#define         LP_CFG_SINK                      1
#define         LP_CFG_SRCE                      2

                                                #ifndef         TC_IGNORE

#define         LP_CFG_BIDIRECTIONAL             3
#define         LP_CFG_BDIR                     LP_CFG_BIDIRECTIONAL

#define         LP_AMS_WINDOW_00000H               0
#define         LP_AMS_WINDOW_40000H            0x10
#define         LP_AMS_WINDOW_80000H            0x20
#define         LP_AMS_WINDOW_C0000H            0x30

#define         LP_AMS_MBOX_00000H              0x00
#define         LP_AMS_MBOX_10000H              0x01
#define         LP_AMS_MBOX_20000H              0x02
#define         LP_AMS_MBOX_30000H              0x03
#define         LP_AMS_MBOX_40000H              0x04
#define         LP_AMS_MBOX_50000H              0x05
#define         LP_AMS_MBOX_60000H              0x06
#define         LP_AMS_MBOX_70000H              0x07
#define         LP_AMS_MBOX_80000H              0x08
#define         LP_AMS_MBOX_90000H              0x09
#define         LP_AMS_MBOX_A0000H              0x0A
#define         LP_AMS_MBOX_B0000H              0x0B
#define         LP_AMS_MBOX_C0000H              0x0C
#define         LP_AMS_MBOX_D0000H              0x0D
#define         LP_AMS_MBOX_E0000H              0x0E
#define         LP_AMS_MBOX_F0000H              0x0F

#define         LP_AMS_DVC_MBOX_OFF             0x00
#define         LP_AMS_DVC_MBOX_ON              0x10

#define         LP_AMS_BUS_MBOX_OFF             0x00
#define         LP_AMS_BUS_MBOX_ON              0x20

#define         LP_AMS_NOT_ANY_RQST             0x00
#define         LP_AMS_ANY_RQST                 0x40

#define         LP_AMS_SLAVE                    0x00
#define         LP_AMS_MASTER                   0x80

#define         LP_AMS_BUS_RESET_ON             0x01
#define         LP_AMS_BUS_RESET_OFF            0x00

#define         LP_AMS_DVC_RESET_ON             0x00
#define         LP_AMS_DVC_RESET_OFF            0x01

                                                #endif


/* new page
 ----------------------------------------------------------------------------- 
| 
| Chapter 4.2   TYPE_LP_PRT_CFG
|
 ------------   A B S T R A C T   -------------------------------------------- 
  

  The parameters required to open a traffic store port are configured in the 
  port configuration list. 

  prt_addr      The port index table is an array of port_nr. This is the
                virtual connection between the port_address and the port_number

  prt_indx      The port_nr defines the port to use for the telegram

  size          Defines the port size in bytes

  type          Defines the port direction (0/1/2: passive / sink / source)

*/

typedef         struct                  STR_LP_PRT_CFG
{
                unsigned short          prt_addr;
                unsigned short          prt_indx;
                unsigned short          size;
                unsigned short          type;

}                                       TYPE_LP_PRT_CFG;

/* new page
 ---------------------------------------------------------------------------- 
|
| Chapter 4.2   TYPE_LP_TS_CFG
|
 ------------   A B S T R A C T   ------------------------------------------- 
  

  The parameters required to open a traffic store port are configured in this
  type of list.

  Traffic Stores consist generally of

        - a port index table (pit) used to connect the full range port address
          a reduced range physical data port 

        - port control and status registers (pcs) which are used to store per
          port the port dependant control and status information

        - data ports which store the data alternatively in pages (range 2)

        - a force sub traffic store which consists of force ports which store
          a predefined value to be used instead of the process variable and
          force status registers which are used to decode whether the force
          or the process variable is to be used

  Each element can be either provided (static) or will be created (dynamic) by
  the link process software. A NULL pointer indicates the need to create the
  buffer. At all times, the sizes are supplied.

  It is possible to recreate traffic stores online, e.g. for a train reconfigu-
  ration. 

  pb_pit        A pointer to the base address of the port index table.
                NULL: dynamic allocation from memory utility

  pb_pcs        A pointer to the port communication & status field base address.
                NULL: dynamic allocation from memory utility

  pb_prt        A pointer to the port base address.
                NULL: dynamic allocation from memory utility

  pb_frc        A pointer to the force table base address.
                NULL: dynamic allocation from memory utility

  pb_def        Pointer to buffer with port default values
                NULL: Initialise all ports to zero

  ts_type       LP_TS_TYPE_A ...

  prt_addr_max  Port index table range is 0 .... prt_addr_max

  prt_indx_max  Port range is 0 .... prt_indx_max
                This value determines the range of the port_data, port_pcs and
                force table

  ownership     LP_CFG_TS_ALIEN / LP_CFG_TS_OWNED
                The traffic store can be owned by the caller or by a third
                party located (perhaps) on a different computer behind a
                dual ported RAM

  prt_count     Independant of LP_MAX_PRT_COUNT, the structure is
                terminated after prt_count elements of config_list

  p_prt_cfg     Pointer to the port configuration, see TYPE_LP_PRT_CFG

new page */

typedef         struct                  STR_LP_TS_CFG
{
                void *                  pb_pit;
                void *                  pb_pcs;
                void *                  pb_prt;
                void *                  pb_frc;
                void *                  pb_def;
                void *                  pb_mwd;
                unsigned char           ownership;
                unsigned char           ts_type;
                unsigned short          prt_addr_max;
                unsigned short          prt_indx_max;
                unsigned short          prt_count;
                struct STR_LP_PRT_CFG * p_prt_cfg;

}                                       TYPE_LP_TS_CFG;


/* new page
 ----------------------------------------------------------------------------- 
| 
| Chapter 4.2   TYPE_LP_HW_BAP_CFG
|
 ------------   A B S T R A C T   -------------------------------------------- 
  

  The parameters required to initialise the type BAP bus controller hardware
  are listed here.

*/

typedef         struct          STR_LP_HW_BAP_CFG
{
                void *          p;

}                               TYPE_LP_HW_BAP_CFG;


/*
 ----------------------------------------------------------------------------- 
| 
| Chapter 4.2   TYPE_LP_HW_MIC_1_CFG
|
 ------------   A B S T R A C T   -------------------------------------------- 
  

  The parameters required to initialise the type MICOS_1 bus controller
  hardware are listed here.

        mbox_base       Mail box base address: 00000H, 10000H, ...., F0000H
        bus_base        AMS bus window, 00000H, 40000H, 80000H, F0000H
        master          (0/1) The master (1) provides the AMS bus clock
        any_rqst        low priority master waits for end of bus transfer
                        high priority master gets bus after transfer end
        dvc_resets_bus  device reset does not reset AMS bus
                        device reset causes AMS reset
        bus_resets_dvc  AMS reset does not reset device
                        AMS reset causes device reset
        dvc_mbox        no mailbox access by device
                        mailbox access by device        
        bus_mbox        no mailbox access by AMS bus
                        mailbox access by AMS bus       

*/

typedef         struct          STR_LP_HW_MIC_1_CFG
{
                char            mbox_base;
                char            bus_base;
                char            master;
                char            dvc_resets_bus;
                char            bus_resets_dvc;
                char            dvc_mbox;
                char            bus_mbox;
                char            any_rqst;

}                               TYPE_LP_HW_MIC_1_CFG;

/* new page
 ----------------------------------------------------------------------------- 
| 
| Chapter 4.2   TYPE_LP_HW_MIC_2_CFG
|
 ------------   A B S T R A C T   -------------------------------------------- 
  

  The parameters required to initialise the type MICOS_2 bus controller
  hardware are listed here.

*/

typedef         struct          STR_LP_HW_MIC_2_CFG
{
                void *          p;

}                               TYPE_LP_HW_MIC_2_CFG;


/*
 ----------------------------------------------------------------------------- 
| 
| Chapter 4.2   TYPE_LP_HW_MIC_3_CFG
|
 ------------   A B S T R A C T   -------------------------------------------- 
  

  The parameters required to initialise the type MICOS_2 bus controller
  hardware are listed here.

*/

typedef         struct          STR_LP_HW_MIC_3_CFG
{
                void *          p;

}                               TYPE_LP_HW_MIC_3_CFG;


/*
 ----------------------------------------------------------------------------- 
| 
| Chapter 4.2   Initialisation - Type definitions  TYPE_LP_HW_CFG
|
 ------------   A B S T R A C T   -------------------------------------------- 
  

  The parameters required to initialise the a bus controller hardware are
  listed here as union of all bus controller types.

*/

typedef         union                   UNN_LP_HW_CFG
{
                struct STR_LP_HW_BAP_CFG        bap_1;
                struct STR_LP_HW_MIC_1_CFG      mic_1;
                struct STR_LP_HW_MIC_2_CFG      mic_2;
                struct STR_LP_HW_MIC_3_CFG      mic_3;

}                                       TYPE_LP_HW_CFG;

/* new page
 ---------------------------------------------------------------------------- 
|
| Chapter 4.3   Prototypes (connection to user)
|
 ----------------------------------------------------------------------------
*/

                                                #ifndef         TC_IGNORE


/* new page
 ---------------------------------------------------------------------------- 
|
| Chapter 4.4   Initialisation - Traffic Store Macro Definitions
|
| macro funct.  lp_prt_addr_max
|
| param. in     prt_addr                Proposed port address max
|               pit_type                LP_PIT_TYPE_A / LP_CFG_PIT_16
|
| return value  prt_addr_max            next defined prt_addr_max
|
 ------------   A B S T R A C T   ------------------------------------------- 

  Convert proposed port address max to next defined port address max.
  Used to build correct configuration list
*/

#define         lp_prt_addr_max(prt_addr, pit_type) \
                (((((prt_addr) & 1) == 0) & ((pit_type) == LP_PIT_TYPE_A)) ? \
                    (unsigned short) ((prt_addr) + 1) : \
                    (unsigned short)  (prt_addr))

/*
 ----------------------------------------------------------------------------
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-11-09  created                   Marsden      --
 ----------------------------------------------------------------------------
End of function lp_prt_addr_max                                            */

/* new page
 ----------------------------------------------------------------------------
|
| Chapter 4.4   Initialisation - Traffic Store Macro Definitions
|
| macro funct.  lp_prt_blck_addr
|
| param. in     prt_addr                Proposed port address max
|               pit_type                LP_PIT_TYPE_A
|                                       LP_PIT_TYPE_B
|
| return value  prt_addr_max            next defined prt_addr_max
|
 ------------   A B S T R A C T   -------------------------------------------

  Convert proposed port address max to next defined port address max.
  Add "1" as array is from 0 .... prt_addr_max.
  Used to define the traffic store port index table memory.
*/

#define         lp_prt_blck_addr(prt_addr, pit_type) \
                        (lp_prt_addr_max(prt_addr, pit_type) + 1)

/*
 ----------------------------------------------------------------------------
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-11-09  created                   Marsden      --
 ----------------------------------------------------------------------------
End of function lp_prt_blck_addr                                           */

/* new page
 ---------------------------------------------------------------------------- 
|
| Chapter 4.4   Initialisation - Traffic Store Macro Definitions
|
| macro funct.  lp_prt_indx_max
|
| param. in     prt_indx                Proposed port index max
|
| return value  prt_indx_max            next defined prt_indx_max
|
 ------------   A B S T R A C T   ------------------------------------------- 

  Convert proposed port index max to next defined port index max.
  Used to build correct configuration list

  !!! Does not yet support the types "B" and "C"

*/

#define         lp_prt_indx_max(prt_indx, prt_type) \
                  ((unsigned short) (prt_indx) | LP_PRT_INDX_MAX_MASK)
/*
 ----------------------------------------------------------------------------
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-11-09  created                   Marsden      --
 ----------------------------------------------------------------------------
End of function lp_prt_indx_max                                            */

/* new page
 ---------------------------------------------------------------------------- 
|
| Chapter 4.4   Initialisation - Traffic Store Macro Definitions
|
| macro funct.  lp_prt_blck_indx
|
| param. in     prt_indx                Proposed port index max
|
| return value  prt_indx_max            next defined prt_indx_max
|
 ------------   A B S T R A C T   ------------------------------------------- 

  Convert proposed port index max to next defined port index max.
  Used to define the traffic store port data buffer, pcs, force, def memory.

*/

#define         lp_prt_blck_indx(prt_indx, prt_type) \
                    (lp_prt_indx_max(prt_indx, prt_type) + 1)
/*
 ----------------------------------------------------------------------------
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-11-09  created                   Marsden      --
|  0.2  BAEI-2  92-07-02  modified                  Marsden      --
|                         functionality changed
 ----------------------------------------------------------------------------
End of function lp_prt_blck_indx                                           */


/* new page
 ----------------------------------------------------------------------------
|
| Chapter 4.4.5 Initialisation - Traffic Store Macro Definitions
|
| macro funct.  lp_pit_size
|
| param. in     prt_addr                Proposed port address max
|               pit_type                A, B or C
|
| return value  size                    size in bytes
|
 ------------   A B S T R A C T   ------------------------------------------- 

  Convert proposed port address max to minimum port index table size.

*/

#define         lp_pit_size(prt_addr, pit_type) \
                (pit_type == LP_PIT_TYPE_A ?                    \
                  (lp_prt_blck_addr(prt_addr, pit_type)) :     \
                  (lp_prt_blck_addr(prt_addr, pit_type) << 1) )

/*
 ----------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  92-03-20  created                   Marsden      --
 ----------------------------------------------------------------------------- 
End of function lp_pit_size*/


/* new page
 ---------------------------------------------------------------------------- 
|
| Chapter 4.4.5 Initialisation - Traffic Store Macro Definitions
|
| macro funct.  lp_pcs_size
|
| param. in     prt_indx                Proposed prt index max
|               pcs_type                A, B or C
|
| return value  size                    size in bytes
|
 ------------   A B S T R A C T   ------------------------------------------- 

  Convert port index max to minimum required pcs array size in bytes

  LP_PCS_TYPE_A:        4 * (lp_prt_indx_max(prt_indx) + 1)
  LP_PCS_TYPE_B:        8 * (lp_prt_indx_max(prt_indx) + 1)
  LP_PCS_TYPE_C:        2 * (lp_prt_indx_max(prt_indx) + 1)

  !!! Does not yet support the types "B" and "C"

*/

#define         lp_pcs_size(prt_indx, pcs_type) \
                (sizeof(struct STR_LP_PCS_A) * (lp_prt_blck_indx(prt_indx, pcs_type)))

/*
 ----------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  92-03-20  created                   Marsden      --
 ----------------------------------------------------------------------------- 
End of function lp_pcs_size*/

/* new page
 ---------------------------------------------------------------------------- 
|
| Chapter 4.4.5 Initialisation - Traffic Store Macro Definitions
|
| macro funct.  lp_prt_size
|
| param. in     prt_indx                Proposed prt index max
|               prt_type                A, B or C
|
| return value  size                    size in bytes
|
 ------------   A B S T R A C T   ------------------------------------------- 

  Convert port index max to minimum required port data array size in bytes

*/

#define         lp_prt_size(prt_indx, prt_type) \
                (sizeof(struct STR_LP_PRT_A) * \
                (lp_prt_blck_indx(prt_indx, prt_type)))

/*
 ----------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  92-03-20  created                   Marsden      --
 ----------------------------------------------------------------------------- 
End of function lp_prt_size*/

/* new page
 ---------------------------------------------------------------------------- 
|
| Chapter 4.4.5 Initialisation - Traffic Store Macro Definitions
|
| macro funct.  lp_frc_size
|
| param. in     prt_indx                Proposed prt index max
|               prt_type                A, B or C
|
| return value  size                    size in bytes
|
 ------------   A B S T R A C T   ------------------------------------------- 

  Convert port index max to minimum required force table size in bytes

*/

#define         lp_frc_size(prt_indx, prt_type) \
                lp_prt_size(prt_indx, prt_type)

/*
 -----------------------------------------------------------------------------
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  92-03-20  created                   Marsden      --
 -----------------------------------------------------------------------------
End of function lp_frc_size */

/* new page
 ----------------------------------------------------------------------------
|
| Chapter 4.4.5 Initialisation - Traffic Store Macro Definitions
|
| macro funct.  lp_def_size
|
| param. in     prt_indx                Proposed prt index max
|               prt_type                A, B or C
|
| return value  size                    size in bytes
|
 ------------   A B S T R A C T   ------------------------------------------- 

  Convert port index max to minimum required default value array size in bytes

*/

#define         lp_def_size(prt_indx, prt_type) \
                (lp_prt_size(prt_indx, prt_type) >> 1)

/*
 ----------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  92-03-20  created                   Marsden      --
 ----------------------------------------------------------------------------- 
End of function lp_def_size */


#endif

/* new page
 ----------------------------------------------------------------------------- 
| 
| Chapter 5.1   TYPE_LP_CLUS_VAR
|
 ------------   A B S T R A C T   -------------------------------------------- 
  

  The parameters required to update one bus variable within the cluster copy
  list

  pv_name       see TYPE_PV_NAME

  p_var         pointer to the application copy of the bus variable

*/

typedef         struct                  STR_LP_CLUS_VAR
{
                struct STR_PV_NAME      pv_name;
                void *                  p_var;

}                                       TYPE_LP_CLUS_VAR;



/*
 ----------------------------------------------------------------------------- 
| 
| Chapter 5.1   TYPE_LP_CLUS_PRT
|
 ------------   A B S T R A C T   -------------------------------------------- 


  The parameters required to update "var_count" bus variables in one port
  within the cluster copy list.

  p_refresh     pointer to a variable to write the refresh information for a
                data set

  var_count     var_count gives the real count of the var_list array

  p_clus_var    see TYPE_LP_CLUS_VAR

  p_clus_prt    Pointer to parameter set for next port in cluster copy list

*/

typedef         struct                  STR_LP_CLUS_PRT
{
                struct STR_LP_CLUS_PRT* p_clus_prt;
                TYPE_DS_NAME            ds_name;
                unsigned short          prt_indx;
                unsigned short    *     p_refresh;
                TYPE_LP_CLUS_VAR  *     p_clus_var;
                unsigned short          var_count;

}                                       TYPE_LP_CLUS_PRT;

/* new page
 ----------------------------------------------------------------------------- 
| 
| type          TYPE_LP_CLUS_TS
|
 ------------   A B S T R A C T   -------------------------------------------- 


  The parameters required to address "prt_count" ports within the traffic
  store "ts_id".

  ts_id         Identifier to define target traffic store

  ds_count      ds_count gives the real count of elements STR_LP_CLUS_DS

  p_clus_ds     Pointer, see structure STR_LP_CLUS_DS
  
*/

typedef         struct                  STR_LP_CLUS_TS
{
                unsigned short          ts_id;
                unsigned short          ds_count;
                struct STR_LP_CLUS_DS * p_clus_ds;

}                                       TYPE_LP_CLUS_TS;



/*
 ----------------------------------------------------------------------------- 
| 
| Chapter 5.1   TYPE_PV_CLUS
|
 ------------   A B S T R A C T   -------------------------------------------- 


  The parameters required to update the bus variables specified by the
  cluster copy list for "ts_count" different traffic stores.

  ts_count      ts_count gives the real count of pointers to p_clus_ts

  p_clus_ts     see TYPE_LP_CLUS_TS

*/

typedef         struct                  STR_PV_CLUS
{
                unsigned short          ts_count;
                unsigned short          reserved;
                struct STR_LP_CLUS_TS * p_clus_ts[LP_TS_ARRAY_INDEX];

}                                       TYPE_PV_CLUS;


/*
 ----------------------------------------------------------------------------- 
| 
| Chapter 5.x
|
 ------------   A B S T R A C T   -------------------------------------------- 



*/

struct          STR_LP_DS_X
{
                unsigned short *        p_refresh;
                void *                  p_pcs;
                void *                  p_prt;
                void *                  p_frc;
                unsigned char           prt_sze;
                unsigned char           pv_x_cnt;
};

struct          STR_LP_DS_Y
{
                unsigned short          ts_id;
                unsigned char           with_frce;
                unsigned char           with_else;
};

struct          STR_LP_PV_X
{
                void *                  p_appl;
                unsigned char           type;
                unsigned char           size;
                unsigned char           byte_offset;
                unsigned char           bit_offset;
};

struct          STR_LP_DS_SET
{
                struct  STR_LP_PV_X *   p_pv_x;
                struct  STR_LP_DS_X     ds_x;
                struct  STR_LP_DS_Y     ds_y;
};

struct          STR_LP_PV_SET
{
                struct STR_PV_NAME      pv_name;
                void *                  p_var;

};

struct          STR_LP_CLUS_DS
{
                struct STR_LP_CLUS_DS * p_nxt_lst;
                struct STR_LP_PV_X *    p_dta_lst;
                struct STR_LP_DS_X      ds_x;
};

                                                #ifndef         TC_IGNORE

#define         LP_VAR_WITHOUT          0

#define         LP_VAR_WITH_FRCE        1
#define         LP_VAR_WITH_ELSE        1


/*
 ----------------------------------------------------------------------------- 
| 
| Chapter 5.3   struct STR_LP_PAC_X
|
 ------------   A B S T R A C T   -------------------------------------------- 


  The pack list structures are a FUPLA feature.

*/

                                                #endif

struct          STR_LP_PAC_2
{
                unsigned short          i_bool;
                unsigned char           zero;
                unsigned char           boff;
};

struct          STR_LP_PAC_1
{
                unsigned short          i_pac_2;
                unsigned short          i_bitset;
                unsigned short          bit_count;
};

struct          STR_LP_PAC_0
{
                unsigned short          i_pac_1;
                unsigned short          pac_1_count;
};


/* new page
 ----------------------------------------------------------------------------- 
| 
| Chapter 7.1   LP_FORCE  constants
|
 ----------------------------------------------------------------------------- 


  Force Constants for force state and force operation parameter

*/

#define         LP_FORCE                1
#define         LP_UNFORCE              0
#define         LP_UNFORCED             LP_UNFORCE
#define         LP_FORCED               LP_FORCE
#define         LP_NO_FORCE             !LP_FORCE


#define         LP_RED_PASSIVE          0
#define         LP_RED_ACTIVE           1

/* new page
 -----------------------------------------------------------------------------
| 
| Chapter 8.1   TYPE_LP_TMO_CLUS_TS
|
 ------------   A B S T R A C T   -------------------------------------------- 


  The parameters required to access the update timeout counters of "prt_count"
  ports within the traffic store "ts_id".

  ts_id         Identifier to define target traffic store

  prt_count     prt_count gives the real count of pointers to prt_list

  p_clus_prt    Pointer to sublist, see TYPE_LP_TMO_CLUS_PRT
  
*/

typedef         struct                  STR_LP_TMO_CLUS_TS
{
                unsigned short          ts_id;
                unsigned short          prt_count;
                struct STR_DS_NAME *    p_ds_name;

}                                       TYPE_LP_TMO_CLUS_TS;



/*
 ----------------------------------------------------------------------------- 
| 
| Chapter 8.1   LP_TMO_...
|
 ----------------------------------------------------------------------------- 


  Timeout Constants for timeout status

*/

#define         LP_TMO_PASSIVE          0
#define         LP_TMO_ACTIVE           1

#define			LP_PASSIVE				0
#define			LP_ACTIVE				1

#define         LP_TMO_MAX_TIME         4096

/*
 ----------------------------------------------------------------------------- 
| 
| Chapter 8.2   TYPE_LP_TMO_CLUS
|
 ------------   A B S T R A C T   -------------------------------------------- 


  The parameters required to update the update counters of "ts_count"
  traffic strores

  ts_count      ts_count gives the real count of pointers to p_clus_ts

  p_clus_ts     see TYPE_LP_CLUS_TO

*/

typedef         struct                          STR_LP_TMO_CLUS
{
                unsigned short                  ts_count;
                unsigned short                  reserved;
                struct STR_LP_TMO_CLUS_TS *     p_clus_ts[LP_TS_ID_MAX + 1];

}                                               TYPE_LP_TMO_CLUS;


                                                #ifndef         TC_IGNORE


#define         LP_PRT_INDX_PASSIVE     0
#define         LP_PRT_ADDR_MAX_MASK    1
#define         LP_PRT_INDX_MAX_MASK    3

#define         LP_PRT_ADDR_MAX_MVB     4095
                                                #endif

/* new page
 --------------------------------------------------------------------------- 
|
| type          TYPE_LP_CB
| 
 ------------   A B S T R A C T   ------------------------------------------ 
  
  
  Structure of the Link Layer Process Control Block. This structure exists
  separately for each created traffic store. The structure is accessed via
  the traffic store id (the control block index).

        pb_pit                  base pointer to port index table
        pb_pcs                  base pointer to port status and comm registers
        pb_prt                  base pointer to port data buffer
        pb_frc                  base pointer to force table
        pit_type                type of pit     (LP_PIT_8 / LP_PIT_16)
        prt_addr_max            port address must not exceed this value
        prt_indx_max            port index must not exceed this value
        pit_mem_type            pit memory type (LPC_STATIC / LPC_DYNAMIC)
        pcs_mem_type            pcs memory type (LPC_STATIC / LPC_DYNAMIC)
        prt_mem_type            prt memory type (LPC_STATIC / LPC_DYNAMIC)
        frc_mem_type            frc memory type (LPC_STATIC / LPC_DYNAMIC)
        irpt_sink               the handler for receive indication is sub-
                                scribed         (LP_IRPT_FREE / LP_IRPT_IN_USE)
        irpt_srce               the handler for sent indication is subscribed
        state                   init state      (LPC_FREE
                                                 LPC_IN_USE
                                                 LPC_ERROR)
        frc_flag                forced data     (LP_UNFORCED / LP_FORCED)
        hw_type                 hardware type   (LP_WTB, LP_BAP_1, LP_MICOS_1)

*/

typedef struct             STR_LP_CB
{
    void *                 pb_pit;
    void *                 pb_pcs;
    void *                 pb_prt;
    void *                 pb_frc;
                                                #if defined (O_PCMCIA)
    void *                 pb_frc_shadow;
                                                #endif
    void *                 pb_mwd;
    void *                 p_root;              /* root of recyclable mem */
    const struct STR_LP_TS_CFG * p_ts_cfg;
    unsigned int           ts_id;
    unsigned short         prt_addr_max;
    unsigned short         prt_indx_max;
    unsigned char          ts_type;
    unsigned char          hw_type;
    unsigned char          ts_owner;
    unsigned char          pcs_power_of_2;      /* size in powers of 2 */
    unsigned char          irpt_sink;
    unsigned char          irpt_srce;
    unsigned char          frc_flag;
    unsigned char          state;
}   TYPE_LP_CB;

/* new page
 ---------------------------------------------------------------------------
|
| type          TYPE_LP_RD_PRT_CFG
|
 ------------   A B S T R A C T   ------------------------------------------



*/

typedef struct             STR_LP_RD_PRT_CFG
{
    unsigned short port_address;
    unsigned char  port_config;
    unsigned char  port_size;
}   TYPE_LP_RD_PRT_CFG;


/* new page
 -----------------------------------------------------------------------------
| 
| constant     LP_TYPE
|
 ----------------------------------------------------------------------------- 
*/

typedef         unsigned char           TYPE_LP_8;
typedef         unsigned short          TYPE_LP_16;

typedef         char                    TYPE_LP_BOOL_1;
typedef         char                    TYPE_LP_BOOL_2;
typedef         char                    TYPE_LP_BCD_4;
typedef         char                    TYPE_LP_FLD_8;

typedef         unsigned char           TYPE_LP_BITSET_8;
typedef         unsigned short          TYPE_LP_BITSET_16;
typedef         unsigned long           TYPE_LP_BITSET_32;

typedef         char                    TYPE_LP_CHAR_8;
typedef         unsigned char           TYPE_LP_CARD_8;
typedef         unsigned short          TYPE_LP_CARD_16;
typedef         unsigned long           TYPE_LP_CARD_32;

typedef         unsigned short          TYPE_LP_FRAC_200;
typedef         unsigned short          TYPE_LP_FRAC_400;
typedef         unsigned short          TYPE_LP_FRAC_800;

typedef         char                    TYPE_LP_SIGN_8;
typedef         short                   TYPE_LP_SIGN_16;
typedef         long                    TYPE_LP_SIGN_32;

typedef         char                    TYPE_LP_A8_SIGN;
typedef         short                   TYPE_LP_A16_SIGN;
typedef         long                    TYPE_LP_A32_SIGN;

typedef         unsigned char           TYPE_LP_A8_CARD;
typedef         unsigned short          TYPE_LP_A16_CARD;
typedef         unsigned long           TYPE_LP_A32_CARD;


typedef         struct                  STR_LP_TIMEDATE_48
{
                TYPE_LP_CARD_32 seconds;
                TYPE_LP_CARD_16 ticks;
}                                       TYPE_LP_TIMEDATE_48;




/*
 ----------------------------------------------------------------------------
|
|       Function Prototypes
|
 ----------------------------------------------------------------------------
*/

TYPE_PD_RESULT   _lp_init                (       void);
TYPE_PD_RESULT   _lpm_mbox_req_create    (const  struct STR_LP_TS_CFG *  p_ts_cfg);
TYPE_PD_RESULT   _lp_create              (       unsigned short          ts_id,
                                                 unsigned short          hw_type,
                                          const  struct STR_LP_TS_CFG *  p_ts_cfg,
                                          const  union  UNN_LP_HW_CFG *  p_hw_cfg);
TYPE_PD_RESULT   _lp_redundancy          (       unsigned int            ts_id,
                                          unsigned char           action  );
TYPE_PD_RESULT   _lp_cluster             (const  struct STR_PV_CLUS   *  p,
                                          short                   data_flow);
TYPE_PD_RESULT   _lp_get_data_set        (const  struct STR_LP_DS_SET *  p_ds_set);
TYPE_PD_RESULT   _lp_put_data_set        (const  struct STR_LP_DS_SET *  p_ds_set);
TYPE_PD_RESULT   _lp_get_variable        (const  struct STR_LP_PV_SET *  p_pv_set,
                                          unsigned short *        p_refresh);
TYPE_PD_RESULT   _lp_put_variable        (const  struct STR_LP_PV_SET *  p_pv_set);
void             _lpa_pack               (const  struct STR_LP_PAC_0 *   p_pac_0);
void             _lpa_unpack             (const  struct STR_LP_PAC_0 *   p_pac_0);
TYPE_PD_RESULT   _ap_get_variable        (const  struct STR_PV_NAME      pv_name,
                                        void *                  p_variable,
                                        unsigned short *        p_refresh);
TYPE_PD_RESULT   _ap_put_variable        (const  struct STR_PV_NAME      pv_name,
                                        void *                  p_variable);
TYPE_PD_RESULT   _lp_frc_single          (const  struct STR_LP_PV_SET *  p_pv_set,
                                        short                   operation);
TYPE_PD_RESULT   _ap_frc_single          (const  struct STR_PV_NAME      pv_name,
                                        void *                  p_variable,
                                        short                   operation);
TYPE_PD_RESULT   _lpc_frc_flag_sum       (       void);
TYPE_PD_RESULT   _lp_unfrc_all           (       unsigned short          ts_id);

void    _lp_tmo_clr_state       (       void);
TYPE_PD_RESULT   _lp_tmo_get_state       (       void);
void    _lp_tmo_set_state       (       void);
TYPE_PD_RESULT   _lp_tmo_counter         (const  struct STR_LP_TMO_CLUS *p_tmo);
TYPE_PD_RESULT   _lp_subscribe           (       unsigned short          ds_type,
                                        struct STR_DS_NAME      ds_name,
                                        void (*proc) (struct STR_DS_NAME));
TYPE_PD_RESULT   _lp_desubscribe         (       unsigned short          ds_type,
                                        struct STR_DS_NAME      ds_name);
TYPE_PD_RESULT   _lp_pv_size             (const  struct STR_PV_NAME  *   pv_name);

void *  _lpx_gen_clus_ts        (       unsigned short          ts_id,
                                        unsigned short          ds_count);
void *  _lpx_gen_clus_ds        (const  struct STR_DS_NAME *    p_ds_name,
                                 const  struct STR_LP_PV_SET *  p_pv_set,
                                        unsigned short *        p_refresh,
                                        unsigned short          pv_set_cnt);
void    _lpx_gen_ds_set         (       struct STR_LP_DS_SET *  p_ds_set,
                                        struct STR_LP_PV_X   *  p_pv_x,
                                 const  struct STR_LP_PV_SET *  p_pv_set,
                                        unsigned short *        p_refresh,
                                        unsigned short          pv_set_cnt);
TYPE_PD_RESULT   _lpc_enquiry            (       unsigned short          ts_id,
                                        void *                  p_cb_struct);


void    _lpl_put_pv_x           (       void *                 	p_port,
										struct STR_LP_PV_X *   	p_pv_x,
										unsigned short         	var_count);

void    _lpl_get_pv_x           (       void *                 	p_port,
										struct STR_LP_PV_X *   	p_pv_x,
										unsigned short         	var_count);

TYPE_PD_RESULT   _lp_rd_prt_cfg          (       unsigned int            bus_id,
                                 struct STR_LP_RD_PRT_CFG *     p_rd_prt_cfg,
                                        unsigned int *          p_c_prts,
                                        unsigned int            c_prts_max);

/* !!! dummy interface !!!*/
TYPE_PD_RESULT   _lp_put_cluster         (const  struct STR_PV_CLUS   *  p);
TYPE_PD_RESULT   _lp_get_cluster         (const  struct STR_PV_CLUS   *  p);

/*
 -----------------------------
    internal prototypes
 -----------------------------
*/


void            lpx_gen_ds_x    (       struct STR_LP_DS_X *    p_x,
                                 const  struct STR_DS_NAME *    p_ds_name);
void            lpx_gen_pv_x    (       struct STR_LP_PV_X *    p_x,
                                 const  struct STR_LP_PV_SET *  p_pv_set,
                                        unsigned short          count);
void            lpx_recycle     (void **                        p_root);

TYPE_PD_RESULT           lpx_remember    (void **                        p_root,
                                 void *                         the_pointer);



/*
 ----------------------------------------------------------------------------
|
|       Structure Declaration for Indirect Call Table
|
 ----------------------------------------------------------------------------
*/
#if defined (O_BAP)

typedef         struct                  STR_LP_IND_CALL
{
    unsigned char                       lp_version_info[4];
    TYPE_PD_RESULT (*plp_init)           (       void);
    TYPE_PD_RESULT (*plp_mbox_req_create)(const  struct STR_LP_TS_CFG *  p_ts_cfg );
    TYPE_PD_RESULT (*plp_create)         (       unsigned short          ts_id,
                                        unsigned short          hw_type,
                                 const  struct STR_LP_TS_CFG *  p_ts_cfg,
                                 const  union  UNN_LP_HW_CFG *  p_hw_cfg);
    TYPE_PD_RESULT (*plp_cluster)        (const  struct STR_PV_CLUS *    p,
                                        short                   data_flow);
    TYPE_PD_RESULT (*plp_cluster_res)    (const  struct STR_PV_CLUS *    p,
                                        short                   data_flow);
    TYPE_PD_RESULT (*plp_get_data_set)   (const  struct STR_LP_DS_SET *  p_ds_set);
    TYPE_PD_RESULT (*plp_put_data_set)   (const  struct STR_LP_DS_SET *  p_ds_set);
    TYPE_PD_RESULT (*plp_get_variable)   (const  struct STR_LP_PV_SET *  p_pv_set,
                                        unsigned short        * p_refresh);
    TYPE_PD_RESULT (*plp_put_variable)   (const  struct STR_LP_PV_SET *  p_pv_set);
    void  (*plp_pack)           (const  struct STR_LP_PAC_0 *   p_pac_0);
    void  (*plp_unpack)         (const  struct STR_LP_PAC_0 *   p_pac_0);
    TYPE_PD_RESULT (*plp_frc_single)     (const  struct STR_LP_PV_SET *  p_pv,
                                        short                   operation);
    TYPE_PD_RESULT (*plp_unfrc_all)      (       unsigned short          ts_id);
    TYPE_PD_RESULT (*plp_frc_get_state)  (       void);
    void  (*plp_tmo_clr_state)  (       void);
    TYPE_PD_RESULT (*plp_tmo_get_state)  (       void);
    void  (*plp_tmo_set_state)  (       void);
    TYPE_PD_RESULT (*plp_tmo_counter)    (const  struct STR_LP_TMO_CLUS *p_tmo);
    TYPE_PD_RESULT (*plp_subscribe)      (       unsigned short          ds_type,
                                        struct STR_DS_NAME     ds_name,
                                        void (*proc) (struct STR_DS_NAME));
    TYPE_PD_RESULT (*plp_pv_size)        (const  struct STR_PV_NAME *    pv_name);
    TYPE_PD_RESULT (*pap_get_variable)   (const  struct STR_PV_NAME      pv_name,
                                        void *                  p_variable,
                                        unsigned short *        p_refresh);
    TYPE_PD_RESULT (*pap_put_variable)   (const  struct STR_PV_NAME      pv_name,
                                        void *                  p_variable);
    TYPE_PD_RESULT (*pap_frc_single)     (const  struct STR_PV_NAME      pv_name,
                                        void *                  p_variable,
                                        short                   operation);
    void *(*plp_gen_clus_ts)    (       unsigned short          ts_id,
                                        unsigned short          ds_count);
    void *(*plp_gen_clus_ds)    (const  struct STR_DS_NAME *    p_ds_name,
                                 const  struct STR_LP_PV_SET *  p_pv_set,
                                        unsigned short *        p_refresh,
                                        unsigned short          pv_set_cnt);
    void  (*plp_gen_ds_set)     (       struct STR_LP_DS_SET *  p_ds_set,
                                        struct STR_LP_PV_X   *  p_pv_x,
                                 const  struct STR_LP_PV_SET *  p_pv_set,
                                        unsigned short *        p_refresh,
                                        unsigned short          pv_set_cnt);
    TYPE_PD_RESULT (*plp_enquiry)        (       unsigned short          ts_id,
                                        void *                  p_cb_struct);
    TYPE_PD_RESULT (*plp_desubscribe)    (       unsigned short          ds_type,
                                        struct STR_DS_NAME      ds_name);
    void  (*plp_get_pv_x)       (       void *                  p_port,
                                        struct STR_LP_PV_X *    p_pv_x,
                                        unsigned short          var_count);
    void  (*plp_put_pv_x)       (       void *                  p_port,
                                        struct STR_LP_PV_X *    p_pv_x,
                                        unsigned short          var_count);
    TYPE_PD_RESULT (*plp_redundancy)     (       unsigned int            ts_id,
                                        unsigned char           action   );
    TYPE_PD_RESULT (*plp_rd_prt_cfg)     (       unsigned int            ts_id,
                                 struct STR_LP_RD_PRT_CFG *     p_rd_prt_cfg,
                                        unsigned int *          p_c_prts,
                                        unsigned int            c_prts_max);

}                                       TYPE_LP_IND_CALL;

#else

typedef         struct                  STR_LP_IND_CALL
{
    unsigned char                       lp_version_info[4];
    TYPE_PD_RESULT (*plp_init)           (       void);
    TYPE_PD_RESULT (*plp_create)         (       unsigned short          ts_id,
                                        unsigned short          hw_type,
                                 const  struct STR_LP_TS_CFG *  p_ts_cfg,
                                 const  union  UNN_LP_HW_CFG *  p_hw_cfg);
    TYPE_PD_RESULT (*plp_cluster)        (const  struct STR_PV_CLUS *    p,
                                        short                   data_flow);
    TYPE_PD_RESULT (*plp_get_data_set)   (const  struct STR_LP_DS_SET *  p_ds_set);
    TYPE_PD_RESULT (*plp_put_data_set)   (const  struct STR_LP_DS_SET *  p_ds_set);
    TYPE_PD_RESULT (*plp_get_variable)   (const  struct STR_LP_PV_SET *  p_pv_set,
                                        unsigned short        * p_refresh);
    TYPE_PD_RESULT (*plp_put_variable)   (const  struct STR_LP_PV_SET *  p_pv_set);
    void  (*plp_pack)           (const  struct STR_LP_PAC_0 *   p_pac_0);
    void  (*plp_unpack)         (const  struct STR_LP_PAC_0 *   p_pac_0);
    TYPE_PD_RESULT (*plp_frc_single)     (const  struct STR_LP_PV_SET *  p_pv,
                                        short                   operation);
    TYPE_PD_RESULT (*plp_unfrc_all)      (       unsigned short          ts_id);
    TYPE_PD_RESULT (*plp_frc_get_state)  (       void);
    TYPE_PD_RESULT (*plp_pv_size)        (const  struct STR_PV_NAME *    pv_name);
    TYPE_PD_RESULT (*pap_get_variable)   (const  struct STR_PV_NAME      pv_name,
                                        void *                  p_variable,
                                        unsigned short *        p_refresh);
    TYPE_PD_RESULT (*pap_put_variable)   (const  struct STR_PV_NAME      pv_name,
                                        void *                  p_variable);
    TYPE_PD_RESULT (*pap_frc_single)     (const  struct STR_PV_NAME      pv_name,
                                        void *                  p_variable,
                                        short                   operation);
    void *(*plp_gen_clus_ts)    (       unsigned short          ts_id,
                                        unsigned short          ds_count);
    void *(*plp_gen_clus_ds)    (const  struct STR_DS_NAME *    p_ds_name,
                                 const  struct STR_LP_PV_SET *  p_pv_set,
                                        unsigned short *        p_refresh,
                                        unsigned short          pv_set_cnt);
    void  (*plp_gen_ds_set)     (       struct STR_LP_DS_SET *  p_ds_set,
                                        struct STR_LP_PV_X   *  p_pv_x,
                                 const  struct STR_LP_PV_SET *  p_pv_set,
                                        unsigned short *        p_refresh,
                                        unsigned short          pv_set_cnt);
    TYPE_PD_RESULT (*plp_enquiry)        (       unsigned short          ts_id,
                                        void *                  p_cb_struct);
    void  (*plp_get_pv_x)       (       void *                  p_port,
                                        struct STR_LP_PV_X *    p_pv_x,
                                        unsigned short          var_count);
    void  (*plp_put_pv_x)       (       void *                  p_port,
                                        struct STR_LP_PV_X *    p_pv_x,
                                        unsigned short          var_count);
    TYPE_PD_RESULT (*plp_redundancy)     (       unsigned int            ts_id,
                                        unsigned char           action   );
    TYPE_PD_RESULT (*plp_rd_prt_cfg)     (       unsigned int            ts_id,
                                 struct STR_LP_RD_PRT_CFG *     p_rd_prt_cfg,
                                        unsigned int *          p_c_prts,
                                        unsigned int            c_prts_max);

}                                       TYPE_LP_IND_CALL;

#endif
/*
 ----------------------------------------------------------------------------
|
|       Function Definitions
|
 ----------------------------------------------------------------------------
*/

#if defined (O_ICM)

extern const    TYPE_LP_IND_CALL        lp_ind_call_table;

#if defined (O_USE_VARIABLE)
    extern      TYPE_LP_IND_CALL *      lp_pc_ptr_to_table;
    #define     PLP_IND_CALL            lp_pc_ptr_to_table
#else
    #define     PLP_IND_CALL            (*(TYPE_LP_IND_CALL * *) LP_ADR_TABLE)
#endif

#define lp_connect() { PLP_IND_CALL = (void *) &lp_ind_call_table; }

#define lp_init                 (* (PLP_IND_CALL->plp_init              ))
#define lp_enquiry              (* (PLP_IND_CALL->plp_enquiry           ))
#define lp_redef_req_create     (* (PLP_IND_CALL->plp_mbox_req_create   ))
#define lp_create               (* (PLP_IND_CALL->plp_create            ))
#define lp_redundancy           (* (PLP_IND_CALL->plp_redundancy        ))

#define lp_cluster              (* (PLP_IND_CALL->plp_cluster           ))
#define lp_get_data_set         (* (PLP_IND_CALL->plp_get_data_set      ))
#define lp_put_data_set         (* (PLP_IND_CALL->plp_put_data_set      ))
#define lp_get_variable         (* (PLP_IND_CALL->plp_get_variable      ))
#define lp_put_variable         (* (PLP_IND_CALL->plp_put_variable      ))

#define lp_pack                 (* (PLP_IND_CALL->plp_pack              ))
#define lp_unpack               (* (PLP_IND_CALL->plp_unpack            ))

#define lp_frc_single           (* (PLP_IND_CALL->plp_frc_single        ))
#define lp_unfrc_all            (* (PLP_IND_CALL->plp_unfrc_all         ))
#define lp_frc_get_state        (* (PLP_IND_CALL->plp_frc_get_state     ))

#define lp_tmo_clr_state        (* (PLP_IND_CALL->plp_tmo_clr_state     ))
#define lp_tmo_get_state        (* (PLP_IND_CALL->plp_tmo_get_state     ))
#define lp_tmo_set_state        (* (PLP_IND_CALL->plp_tmo_set_state     ))
#define lp_tmo_counter          (* (PLP_IND_CALL->plp_tmo_counter       ))

#define lp_subscribe            (* (PLP_IND_CALL->plp_subscribe         ))
#define lp_pv_size              (* (PLP_IND_CALL->plp_pv_size           ))

#define ap_get_variable         (* (PLP_IND_CALL->pap_get_variable      ))
#define ap_put_variable         (* (PLP_IND_CALL->pap_put_variable      ))

#define ap_frc_single           (* (PLP_IND_CALL->pap_frc_single        ))

#define lp_gen_clus_ts          (* (PLP_IND_CALL->plp_gen_clus_ts       ))
#define lp_gen_clus_ds          (* (PLP_IND_CALL->plp_gen_clus_ds       ))
#define lp_gen_ds_set           (* (PLP_IND_CALL->plp_gen_ds_set        ))

#define lp_desubscribe          (* (PLP_IND_CALL->plp_desubscribe       ))

#define lp_put_pv_x             (* (PLP_IND_CALL->plp_put_pv_x          ))
#define lp_get_pv_x             (* (PLP_IND_CALL->plp_get_pv_x          ))

#define lp_read_port_configuration (* (PLP_IND_CALL->plp_rd_prt_cfg     ))

#define lp_version()            {   bc_version_verify ( BC_SUBPROJECT_LP, \
                                                        LP_VERSION,       \
                                                        LP_RELEASE ); }

#define lp_req_create(hw_type, p_ts_cfg)       lp_redef_req_create(p_ts_cfg)

#else

#define lp_req_create(x,y)     	0	/*   not part of TCN functionality  */
#define lp_connect()              /* */

#define	lp_init			    _lp_init
#define lp_create	     	_lp_create
#define lp_redundancy       _lp_redundancy
#define	lp_cluster          _lp_cluster
#define	lp_get_data_set		_lp_get_data_set
#define	lp_put_data_set		_lp_put_data_set
#define	lp_get_variable		_lp_get_variable
#define	lp_put_variable		_lp_put_variable
#define	lp_pack			    _lpa_pack
#define	lp_unpack		    _lpa_unpack
#define	lp_frc_single		_lp_frc_single
#define	lp_unfrc_all		_lp_unfrc_all
#define	lp_frc_get_state	_lpc_frc_flag_sum
#define	lp_tmo_clr_state	_lp_tmo_clr_state
#define	lp_tmo_get_state	_lp_tmo_get_state
#define	lp_tmo_set_state	_lp_tmo_set_state
#define	lp_tmo_counter		_lp_tmo_counter
#define	lp_subscribe		_lp_subscribe
#define	lp_pv_size		    _lp_pv_size
#define	lp_gen_clus_ts		_lpx_gen_clus_ts
#define	lp_gen_clus_ds		_lpx_gen_clus_ds
#define	lp_gen_ds_set		_lpx_gen_ds_set
#define	lp_enquiry   		_lpc_enquiry
#define	lp_desubscribe		_lp_desubscribe
#define	lp_put_pv_x		    _lpl_put_pv_x
#define	lp_get_pv_x		    _lpl_get_pv_x
#define lp_read_port_configuration _lp_rd_prt_cfg

#define	ap_get_variable		_ap_get_variable
#define	ap_put_variable		_ap_put_variable
#define	ap_frc_single		_ap_frc_single

#endif


#define lp_frc_variable(p_pv_set)       lp_frc_single (p_pv_set, LP_FORCE)
#define lp_unfrc_variable(p_pv_set)     lp_frc_single (p_pv_set, LP_UNFORCE)
#define ap_frc_variable(pv_name, p_var) ap_frc_single (pv_name, p_var, LP_FORCE)
#define ap_unfrc_variable(pv_name)      ap_frc_single (pv_name, NULL, LP_UNFORCE)

#define lp_subscribe_sent(ds_name, proc)       lp_subscribe (LP_CFG_SRCE, ds_name, proc)
#define lp_subscribe_received(ds_name, proc)   lp_subscribe (LP_CFG_SINK, ds_name, proc)

#define lp_desubscribe_sent(ds_name)           lp_desubscribe (LP_CFG_SRCE, ds_name)
#define lp_desubscribe_received(ds_name)       lp_desubscribe (LP_CFG_SINK, ds_name)

#define lp_get_cluster(p_clus)                 lp_cluster (p_clus, LP_GET)
#define lp_put_cluster(p_clus)                 lp_cluster (p_clus, LP_PUT)

#define ap_force_variable                      ap_frc_variable
#define ap_unforce_variable                    ap_unfrc_variable
#define ap_unforce_all                         lp_unfrc_all

#define ap_get_cluster                         lp_get_cluster
#define ap_put_cluster                         lp_put_cluster

#define ap_get_data_set                        lp_get_data_set
#define ap_put_data_set                        lp_put_data_set

#define ap_ds_subscribe_sent                   lp_subscribe_sent
#define ap_ds_subscribe_received               lp_subscribe_received

#define ap_ds_desubscribe_sent                 lp_desubscribe_sent
#define ap_ds_desubscribe_received             lp_desubscribe_received


/*
 -----------------------------------
  Version Verification Mechanism
 -----------------------------------
*/

#define LP_VERSION       2                      /* Interface check    */
#define LP_RELEASE       7

#define LP_UPDATE        3
#define LP_EVOLUTION     0



#if defined (O_BAP)
#define lp_get_version() \
"Name=RTP-LP BAP  V:2.6.17-D:95-08-10-C: Last changes: apd_subscribe"
#else
#define lp_get_version() \
"Name=RTP-LP MVBC V:2.6.17-D:95-08-10-C: Last changes: apd_subscribe"
#endif

#endif



#if defined (__cplusplus)
 }
#endif


/*
 ----------------------------------------------------------------------------
|
|       M O D I F I C A T I O N S
|       - - - - - - - - - - - - -
|
 ---------------------------------------------------------------------------- 

        file version:   4
        dated:          92-12-16


        name:           lp_enquiry

        This new function was added to the interface

----------------------------------------------------------------------

        name:           struct STR_LP_CB

        This structure was transferred from lpc_blck.h to lp_sys.h, because
        it is part of lp_enquiry

------------------------------------------------------------------------------

        file version:   5

        dated:          93-01-27

        name            version management (LP_UPDATE, LP_EVOLUTION)

        These constants are now located in LP_INDT.C

------------------------------------------------------------------------------

        file version:   6

        dated:          93-05-05

        name            LP_TYPE_A16_CARD / LP_TYPE_A16_SIGN (plus A32)

        Modified values of standard taken


------------------------------------------------------------------------------

        file version:   7

        dated:          93-06-16

        name            lp_desubscribe

        new function added


------------------------------------------------------------------------------

        file version:   8

        dated:          93-08-13

        name            ap_.....

        (provisional) "ap_" interface added


------------------------------------------------------------------------------

        file version:   9

        dated:          93-11-09

        Switch for big endian  added

------------------------------------------------------------------------------

        file version:   10

        dated:          93-11-30

        LP_CFG_128_BYTES        added
        STR_PV_NAME_TB          added
        TYPE_PV_NAME_TB_VB      added
------------------------------------------------------------------------------

	file version:   11

	dated:          93-12-14

	lp_pcs_size             corrected
	lp_prt_size             corrected
	lp_frc_size             corrected

------------------------------------------------------------------------------

	file version:   12

	dated:          94-02-08

	Switch for MVBC introduced
        Indirect call Mechanism removed for O_TCN

------------------------------------------------------------------------------

	file version:   13

	dated:          94-09-02

	struct STR_DS_NAME_TB added

------------------------------------------------------------------------------

	file version:   14
	dated:          95-10-25

	LME  lp_read_port_configuration added

------------------------------------------------------------------------------

	file version:   15
	dated:          95-12-06

    LP_STR_CB - new member pb_frc_shadow for pcmcia operation

 ------------------------------------------------------------------------------



*/
