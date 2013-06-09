/*
 ------------------------------------------------------------------------------
|
|    Property of  :   ABB Verkehrssysteme AG, CH-8050 ZÅrich / Switzerland
|    COPYRIGHT    :   (c) 1992 ABB Verkehrssysteme AG
|                     
 ------------------------------------------------------------------------------
|
|    Project      :   T C N   B U S   A D M I N I S T R A T O R
|
 ------------------------------------------------------------------------------
|
|    File Name    :   BAA_AUXI
|
|    Document     :
|
|    Abstract     :   Auxiliary Functions
|
|    Remarks      :
|
|    Dependencies :
|
|    Accepted     :
|
 ------------------------------------------------------------------------------
|
|    HISTORY:
|
|    Vers  Dept.   YY-MM-DD  State    Name    Reference
|    ----  ------  --------  -------  ------- ---------
|      1   BAEI-2  93-02-09  created  Marsden
|
 ------------------------------------------------------------------------------
*/

#ifndef     BAA_AUXI_H
#define     BAA_AUXI_H

#include    "bam_main.h"

typedef     unsigned short      T_CARD16;
typedef              short      T_INT16;


/*  new page
 ----------------------------------------------------------------------------
|
|   structures
|
 ----------------------------------------------------------------------------
*/

#if defined (O_BAP)
#define BAA_STR_TS_PCS          BAA_STR_TS_PCS_A
#else
#define BAA_STR_TS_PCS          BAA_STR_TS_PCS_B
#endif

union       BAA_UNN_DTA
{
            unsigned long   ul[1];
                     long   sl[1];
            void *          p_vd[1];
					 long * p_sl[1];
            unsigned long * p_ul[1];
                     short* p_ss[1];
            unsigned short* p_us[1];
                     char * p_sc[1];
            unsigned char * p_uc[1];
            unsigned short  us[1];
					 short  ss[1];
            unsigned char   uc[1];
                     char   sc[1];

};

/* new page
 ----------------------------------------------------------------------------
|
|   name        BAD_STR_SHORT_ARRAY
|
 ------------   A B S T R A C T   -------------------------------------------

	These structures are used to support debugging (

*/

struct  BAA_STR_SHORT_ARRAY
{
	unsigned short item[64];
};

struct  BAA_STR_TS_PIT
{
	unsigned char  prt_addr[4096];
};

struct  BAA_STR_CHAR_ARRAY
{
    unsigned char  item[64];
};





#if defined     (O_BAP)

struct  BAA_STR_TS_PCS_A
{
		unsigned int    tmp     :   1;           /* PCS Byte 0           */
		unsigned int    type    :   2;
		unsigned int    dc_0    :   1;
		unsigned int    fcode   :   4;

		unsigned int    ie_x         : 4;        /* PCS Byte 1           */
		unsigned int    size_fcode_7 : 4;        /* size in 16 bits      */

		unsigned int    diag    :   3;           /* PCS Byte 2           */
		unsigned int    page    :   1;
		unsigned int    dc_2    :   4;

		unsigned int    tack    :   8;           /* PCS Byte 3           */

};

#else

struct BAA_STR_TS_PCS_B
{
		unsigned int fe            : 1 ;    /*  byte 0 + 1 */
		unsigned int     reserved0 : 4 ;
		unsigned int ie_x          : 3 ;
		unsigned int     reserved1 : 2 ;
		unsigned int type          : 2 ;
		unsigned int fcode         : 4 ;
        unsigned int diag          : 3 ;    /*  byte 2 + 3 */
		unsigned int     reserved2 : 3 ;
		unsigned int page          : 1 ;
		unsigned int     reserved3 : 9 ;

		unsigned char    reserved_a[8];     /* byte 4...11 */
		unsigned char size;                 /* byte 12 */
		unsigned char    reserved_b[3];     /* byte 13...15 */
};

#endif

struct  BAA_STR_TS_PRT
{
		unsigned short          sf;
		unsigned short          garfield_was_here[3];
};

/* new page
 ----------------------------------------------------------------------------
|
|   name    BAA_STR_PDT
|
 --------   A B S T R A C T   -----------------------------------------------

    The propagation delay table defines the time in us which each telegram
    (MF and SF) requires depending upon FCode and Delay Class.

*/

#define         BAA_PDC_1             330
#define         BAA_PDC_2             662
#define         BAA_PDC_3             994
#define         BAA_PDC_4            1326
#define         BAA_PDC_5            1658
#define         BAA_PDC_6            2000

struct      BAA_STR_PDT
{
            unsigned short  us[6][5];
};



/* new page
 -----------------------------------------------------------------------------
|
|   name        BAA_STR_DTA         static data
|
 ------------   A B S T R A C T   --------------------------------------------



    pd      propagation delay structure

*/

struct      BAA_STR_EBL
{
            int         class;
            int         ix_table;
};

struct      BAA_STR_DTA
{
    struct  BAA_STR_EBL     ebl;        /* electrical bus length            */
};

struct  BAA_STR_SPY_A
{
    unsigned short              strt;
    unsigned short              clck_1;
    unsigned short              clck_2;
};

#define BAA_SPY_ID_BLL_CY       0
#define BAA_SPY_ID_BLL_PL       1
#define BAA_SPY_ID_BLL_HW       2

#define BAA_SPY_ID_BLL_CNT      3

#define BAA_C_SPY               100

struct  BAA_STR_SPY
{
    struct BAA_STR_SPY_A        a_spy[BAA_C_SPY];
};

#define     baa_spy_start(id, s_data)   {;}
#define     baa_spy_click()             {;}

/*

#define     baa_spy_start(id, s_data)                           \
            unsigned short *     p_hdle;                        \
            {   unsigned short * p_temp;                        \
                unsigned short count = s_data.dbug.c_tmr1;      \
                p_temp = (count < BAA_C_SPY )     ?             \
                &s_data.spy[id].a_spy[count].strt :             \
                &s_data.spy[id].a_spy[BAA_C_SPY - 1].strt ;     \
                bah_cpu_tmr1_r_c(p_temp);                        \
                p_hdle = p_temp + 1;                             }

#define     baa_spy_click()                                  {  \
                bah_cpu_tmr1_r_c(p_hdle);                       \
                p_hdle++;                                        }

*/
#if defined (O_BAP)
#define  baa_ts_get_diag(p_val, p_pcs)              \
         { short tmp = *(((short *) p_pcs) + 1);    \
           *p_val = tmp & 0xFF;                     \
           *(((short *) p_pcs) + 1) = tmp & 0xFFF8; }
#else
#define  baa_ts_get_diag(p_val, p_pcs)              \
         { *p_val = (*(((short *) p_pcs) + 1) & 0xFF);}
#endif


/*
 ---------------------------------------------------------------------------ƒ
|
|   Constants
|
 ----------------------------------------------------------------------------
*/

#define     BAA_PASSIVE         0
#define     BAA_ACTIVE          1

#define     BAA_MARK            0xFF

#define     BAA_EBL_MAX         0x6     /* maximum bus length: class 6 */
/*
 ---------------------------------------------------------------------------ƒ
|
|   structures
|
 ----------------------------------------------------------------------------
*/

struct      BAA_STR_P_A16
{
            unsigned short *    ptr;
            unsigned short      cnt;
};


#define     BAA_MTT_ACC_MSK             0x8000
#define     BAA_MTT_ACC_OFF             15
#define     BAA_MTT_ACCEPTD             1


#define     baa_bf_2_int(bf, msk, off) \
              (((*bf) & (msk)) >> (off))



/*
 ---------------------------------------------------------------------------ƒ
|
|   Prototypes
|
 ----------------------------------------------------------------------------
*/

#define         baa_clr(p_trgt, val) \
                { *(p_trgt) &= ~(val); }

#define         baa_set(p_trgt, val) \
                { *(p_trgt) |= (val); }

#define         baa_bf_put(p_trgt, val, mask, offs)    \
				{ *(p_trgt) = (*(p_trgt) & ~(mask)) |  \
				   (((val) << (offs)) & (mask)) ;}

void            baa_open            (struct BAA_STR_DTA **  p_baa_dta);

void            baa_init            (void);

short           baa_create          (struct BAM_STR_CFG_PT *  p_pt);

void            baa_ix2p            (struct BAA_STR_P_A16 * p_a16,
                                     void *                 p_base,
                                     unsigned short         ix,
                                     unsigned short         ix_next);

unsigned short  baa_pd_in_us        (unsigned short ix_size);

unsigned short  baa_pd_ix_table     (void);

short           baa_ebl_put         (unsigned short electrical_bus_length);

void            baa_ebl_get         (unsigned short * electrical_bus_length);

void            baa_delay           (unsigned         count);

#if defined (O_SEG)

union BAA_UNN_DTA baa_p_2_abs   (void *     ptr);

void *  baa_p_arith             (void *     ptr, unsigned long l_offset);

void *  baa_abs_2_p             (unsigned long address);

#else

#define baa_p_arith(ptr, l_offset) ((void *)(((char *)(ptr)) + l_offset))

#endif


/*
 ---------------------------------------------------------------------------ƒ
|
|   M O D I F I C A T I O N S       BAA_AUXI.C
|   - - - - - - - - - - - - -       ----------
|
 ----------------------------------------------------------------------------


    name of object modified     Nr: .2      dated:  9?-??-??

    description of what was done to object

 ----------------------------------------------------------------------------

*/
#endif
