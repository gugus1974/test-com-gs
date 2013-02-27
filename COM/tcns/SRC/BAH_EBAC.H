/*
 ------------------------------------------------------------------------------
|
|    Property of  :   ABB Verkehrssysteme AG, CH-8050 Zrich / Switzerland
|    COPYRIGHT    :   (c) 1994 ABB Verkehrssysteme AG
|
 ------------------------------------------------------------------------------
|
|    Project      :   T C N   B U S   A D M I N I S T R A T O R
|
 ------------------------------------------------------------------------------
|
|    File Name    :   BAH_EBAC.H
|
|    Document     :
|
|    Abstract     :   Functions with Hardware Access
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
|    Vers   Dept.   YY-MM-DD  State     Name    Reference
|    ----   ------  --------  -------   ------- ---------
|      0    BATC    94-01-01  created   Marsden
|
 ------------------------------------------------------------------------------
*/

#ifndef         BAH_EBAC_H
#define         BAH_EBAC_H


#include "bal_link.h"
#if defined (O_BAP)

#define bah_bap_tmr_init      bah_tmr_init

#else

#define bah_bap_tmr_init(par_0, par_1) {;} /* empty function */

#endif


#if defined (O_BAP)


/* new page
 -----------------------------------------------------------------------------
|
|   1.1         BAH_STR_EBAC        BAP-BAC dependant static data
|
 ------------   A B S T R A C T   --------------------------------------------

    The BAC static data stores the BAC relevant information

    c_tmr1_a       stores max count. The count is subtracted from this max
                   count to emulate mvbc counting behaviour
    isr_tmr1       a slot overshoot is stored in this variable

*/

struct      BAH_STR_EBAC
{
            unsigned short         c_tmr1_a;
            unsigned short         isr_tmr1;
};



/* new page
 -----------------------------------------------------------------------------
|
|   name        Timer functionality
|
 ------------   A B S T R A C T   --------------------------------------------

    The BAC is synchronized by timer 1 on the EKR II

    bus master mode:
    timer 1 triggers the DMA

    redundant mode:
    timer 1 wakes up the software every 1.3 ms

*/
                                        /* time x frequency / 4 */

#define         BAH_CPU_TMR1_FACTOR     4
#define         BAH_CPU_TMR1_0010US     (  10 * 4)
#define         BAH_CPU_TMR1_0100US     ( 100 * 4)
#define         BAH_CPU_TMR1_1000US     (1000 * 4)
#define         BAH_CPU_TMR1_1200US     (1200 * 4)
#define         BAH_CPU_TMR1_1290US     (1290 * 4)
#define         BAH_CPU_TMR1_1300US     (1300 * 4)
#define         BAH_CPU_TMR1_1400US     (1400 * 4)

#define         BAH_CPU_TMR1_64MS       (32000)
#define         bah_c_64ms(par)         ((par) >> 3)

#define         BAH_CPU_ISR_BIT_TMR1    0x02

/*  new page
 ----------------------------------------------------------------------------
|
|   constant    timer 1
|
 ------------   A B S T R A C T   -------------------------------------------

    These constants define the contents to write to timer 1

    Bit 0       CONT    1           run continuously
    Bit 1       ALT     0           compare with Count A
    Bit 2       EXT     0           count internal clock / 4
    Bit 3       P       0           no prescaler
    Bit 4       RTG     0           level controlled counting
    Bit 5       MC      0           set by CPU, max count
    Bit 6..10   res     0
    Bit 11      0       0
    Bit 12      RIU     0           set by CPU
    Bit 13      INT     1           interrupt on terminal count
    Bit 14,15   INH/EN  11          enable counting

*/

#define         BAH_CPU_TMR1_GO         0xE001
#define         BAH_CPU_TMR1_STOP       0x4000

#define         BAH_CPU_VEC_DMA0        0x000A
#define         BAH_CPU_VEC_DMA1        0x000B



/*  new page
 ----------------------------------------------------------------------------
|
|   constant    io ports
|
 ------------   A B S T R A C T   -------------------------------------------

    These constants define the resource location.

*/

#define         BAH_CPU_IOP_INT             0x7F00
#define         BAH_BAC_IOP_EXT             0x4000
#define         BAH_BAP_LOC_SATN            0x0980L



/*  new page
 ----------------------------------------------------------------------------
|
|   constant    TS Mapping
|
 ------------   A B S T R A C T   -------------------------------------------


*/

#if defined (O_EKR)
#include "lc_sys.h"

#define BAH_PB_TS_DSW_PIT    ((       LC_TYPE_TS_PIT *)     0x00004000L)
#define BAH_PB_TS_DSW_PCS    ((       LC_TYPE_TS_DA_PCS *)  0x00003C00L)
#define BAH_PB_TS_DSW_PRT    ((       LC_TYPE_TS_DA_PRT *)  0x00005000L)
#define BAH_PB_TS_BMT_PCS    ((       LC_TYPE_TS_PCS     *) 0x000009A0L)
#define BAH_PB_TS_BMT_PRT    ((       LC_TYPE_TS_PRT     *) 0x00000A00L)

#define BAH_PB_TS_DMA_CFGD   ( &bah_dta.ba_dma_cfgd)
#define BAH_PB_TS_DMA_NCFG   ( &bah_dta.ba_dma_ncfg)
#define BAH_PO_TS_DMA_CFGD   ( &bah_dta.ba_dma_cfgd)
#define BAH_PO_TS_DMA_NCFG   ( &bah_dta.ba_dma_ncfg)

#endif

#define     bah_bac_at_reset()          bah_bac_reg_wc40(0x1F)
#define     bah_bac_dt_reset()          bah_bac_reg_wc42(0xE3)
#define     bah_bac_dmar_dis()          bah_bac_reg_wc46(0xFF)
#define     bah_bac_mabv_dis()          bah_bac_reg_wc48(0x27)
#define     bah_bac_be_reset()          bah_bac_reg_wc4c(0xFF)
#define     bah_bac_atrl_in             bah_bac_reg_r40

#define     bah_bac_mf_in(p_val)        {bah_bac_reg_r42(&p_val->us[0]); \
                                        {bah_bac_reg_r44(&p_val->us[1]); }


#if !defined (BX_SIMU)

#define bah_bap_skrl_gap_set() \
        { PUTBYTE (ADR_BAP_SKRL, GETBYTE (ADR_BAP_SKRL) | BIT7); }

#define     bah_bac_2_bm(val)           {unsigned char tmp = val+BAH_BAC_R48_MABV; \
                                         bah_bac_reg_wc48(tmp);}

#define     bah_bap_reg_r00(p_val)      {unsigned char * p_tmp = p_val;      \
                                         bah_inp_bvc(p_tmp, BAH_IOP_BAP_R00);}
#define     bah_bap_reg_r02(p_val)      {unsigned char * p_tmp = p_val;      \
                                         bah_inp_bvc(p_tmp, BAH_IOP_BAP_R02);}
#define     bah_bap_reg_r04(p_val)      {unsigned char * p_tmp = p_val;      \
                                         bah_inp_bvc(p_tmp, BAH_IOP_BAP_R04);}
#define     bah_bap_reg_r06(p_val)      {unsigned char * p_tmp = p_val;      \
                                         bah_inp_bvc(p_tmp, BAH_IOP_BAP_R06);}

#define     bah_bac_reg_wc40(val)       {bah_outp_bcc(BAH_IOP_BAC_R40, val);}
#define     bah_bac_reg_wc42(val)       {bah_outp_bcc(BAH_IOP_BAC_R42, val);}
#define     bah_bac_reg_wc44(val)       {bah_outp_bcc(BAH_IOP_BAC_R44, val);}
#define     bah_bac_reg_wc46(val)       {bah_outp_bcc(BAH_IOP_BAC_R46, val);}
#define     bah_bac_reg_wc48(val)       {bah_outp_bcc(BAH_IOP_BAC_R48, val);}
#define     bah_bac_reg_wc4a(val)       {bah_outp_bcc(BAH_IOP_BAC_R4A, val);}
#define     bah_bac_reg_wc4c(val)       {bah_outp_bcc(BAH_IOP_BAC_R4C, val);}

#define     bah_bac_reg_r40(p_val)      {bah_inp_bvc(p_val, BAH_IOP_BAC_R40);}
#define     bah_bac_reg_r42(p_val)      {bah_inp_bvc(p_val, BAH_IOP_BAC_R42);}
#define     bah_bac_reg_r44(p_val)      {bah_inp_bvc(p_val, BAH_IOP_BAC_R44);}
#define     bah_bac_reg_r4c(p_val)      {bah_inp_bvc(p_val, BAH_IOP_BAC_R4C);}

#define     bah_cpu_dma0_w_a_srce(val)  {bah_cpu_dma0_w_l_srce(val.us[0]); \
                                         bah_cpu_dma0_w_h_srce(val.us[1]); }
#define     bah_cpu_dma0_w_l_srce(val)  {unsigned short tmp = val;         \
                                         bah_outp_wcv(BAH_IOP_DMA0_L_SRCE,tmp); }
#define     bah_cpu_dma0_r_l_srce(p_val) {unsigned short * p_tmp = p_val;  \
                                         bah_inp_wvc(p_tmp, BAH_IOP_DMA0_L_SRCE); }
#define     bah_cpu_dma0_w_h_srce(val)  {unsigned short tmp = val;         \
                                         bah_outp_wcv(BAH_IOP_DMA0_H_SRCE,tmp); }
#define     bah_cpu_dma0_w_a_dest(val)  {bah_cpu_dma0_w_l_dest(val.us[0]); \
                                         bah_cpu_dma0_w_h_dest(val.us[1]); }
#define     bah_cpu_dma0_w_l_dest(val)  {unsigned short tmp = val;         \
                                         bah_outp_wcv(BAH_IOP_DMA0_L_DEST,tmp); }
#define     bah_cpu_dma0_w_h_dest(val)  {unsigned short tmp = val;         \
                                         bah_outp_wcv(BAH_IOP_DMA0_H_DEST,tmp); }
#define     bah_cpu_dma0_w_c_mf(val)    {unsigned short tmp = val;         \
                                         bah_outp_wcv(BAH_IOP_DMA0_CNT   ,tmp); }
#define     bah_cpu_dma0_w_cw(val)      {unsigned short tmp = val;\
                                         bah_outp_wcv(BAH_IOP_DMA0_CW    ,tmp); }

#define     bah_cpu_dma1_w_a_srce(val)  {bah_cpu_dma1_w_l_srce(val.us[0]); \
                                         bah_cpu_dma1_w_h_srce(val.us[1]); }
#define     bah_cpu_dma1_w_l_srce(val)  {unsigned short tmp = val;\
                                         bah_outp_wcv(BAH_IOP_DMA1_L_SRCE,tmp); }
#define     bah_cpu_dma1_r_l_srce(p_val) {unsigned short * p_tmp = p_val;  \
                                         bah_inp_wvc(p_tmp, BAH_IOP_DMA1_L_SRCE); }
#define     bah_cpu_dma1_w_h_srce(val)  {unsigned short tmp = val;\
                                         bah_outp_wcv(BAH_IOP_DMA1_H_SRCE,tmp); }
#define     bah_cpu_dma1_w_a_dest(val)  {bah_cpu_dma1_w_l_dest(val.us[0]); \
                                         bah_cpu_dma1_w_h_dest(val.us[1]); }
#define     bah_cpu_dma1_r_a_srce(p_val) {unsigned short * p_tmp = p_val;  \
                                         bah_inp_wvc(p_tmp, BAH_IOP_DMA1_L_SRCE); }
#define     bah_cpu_dma1_w_l_dest(val)  {unsigned short tmp = val;\
                                         bah_outp_wcv(BAH_IOP_DMA1_L_DEST,tmp); }
#define     bah_cpu_dma1_w_h_dest(val)  {unsigned short tmp = val;\
                                         bah_outp_wcv(BAH_IOP_DMA1_H_DEST,tmp); }
#define     bah_cpu_dma1_w_c_mf(val)    {unsigned short tmp = val;\
                                         bah_outp_wcv(BAH_IOP_DMA1_CNT   ,tmp); }
#define     bah_cpu_dma1_w_cw(val)      {unsigned short tmp = val;\
                                         bah_outp_wcv(BAH_IOP_DMA1_CW    ,tmp); }



#define     bah_cpu_isr_r_tmr1(p_val)   {unsigned short * p_tmp = p_val;  \
                                         bah_inp_wvc(p_tmp, BAH_IOP_ISR); }
#define     bah_cpu_eoi_dma()           {unsigned short tmp = BAH_CPU_VEC_DMA0;\
                                         bah_outp_wcv(BAH_IOP_EOI,tmp);        \
                                         tmp = BAH_CPU_VEC_DMA1;\
                                         bah_outp_wcv(BAH_IOP_EOI,tmp);}

#define     bah_cpu_tmr1_w_cw(val)      {unsigned short tmp = val;\
                                         bah_outp_wcv(BAH_IOP_TMR1_CW,    tmp);}
#define     bah_cpu_tmr1_w_cnt_a(val) {unsigned short tmp = val;\
                                       bah_outp_wcv(BAH_IOP_TMR1_CNT_A, tmp);\
                                       bah_dta.ebac.c_tmr1_a = tmp; }
#define     bah_cpu_tmr1_w_cnt(val)     {unsigned short tmp = val;\
                                         bah_outp_wcv(BAH_IOP_TMR1_CNT,   tmp);}
#define bah_cpu_tmr1_r_c(p_val)   {unsigned short * p_tmp = p_val;  \
                                   bah_inp_wvc(p_tmp, BAH_IOP_TMR1_CNT); \
                                   *p_tmp = bah_dta.ebac.c_tmr1_a - *p_tmp;}

#define     bah_cpu_dma0_ei()           {bah_outp_wcc(BAH_IOP_DMA0_IRPT, 0x02);}
#define     bah_cpu_dma1_ei()           {bah_outp_wcc(BAH_IOP_DMA1_IRPT, 0x02);}
#define     bah_cpu_tmr1_ei()           {bah_outp_wcc(BAH_IOP_TMR1_IRPT, 0x01);}



/*  new page
 ----------------------------------------------------------------------------
|
|   name        bah_send_mf
|   (in )       mf                  16-bit master frame
|   ret val     void
|
 ------------   A B S T R A C T   -------------------------------------------
*/

#define         bah_send_mf(mf)     \
{                                   \
    bah_bac_at_reset ();            \
    bah_bac_dt_reset ();            \
    bah_bac_satn     (mf);          \
}



void        bah_bap_rcve_sf         (struct BAL_STR_RSP *   p_sf);

#define     bah_bac_satn(mf)    {*(unsigned short *) BAH_LOC_SATN = mf;}


#define     bah_bac_atrl_dma(p_val)     {bah_bac_reg_r40(p_val);  \
                                         *p_val &= 0x01; }

#define     bah_bac_dma1_set()          bah_bac_reg_wc4a(0x01)


#else

#define         bah_bap_skrl_gap_set()
#define         bah_bac_satn(mf)        {mf=mf;}    /* empty function */

#define         bah_bac_2_bm(val)       {unsigned char tmp = val+BAH_BAC_R48_MABV; \
                                         bah_bac_reg_w48(tmp);}
#define         bah_bac_reg_r40         bsh_bac_reg_r40
#define         bah_bac_reg_r42         bsh_bac_reg_r42

#define         bah_bac_reg_wc40        bah_bac_reg_w40
#define         bah_bac_reg_wc42        bah_bac_reg_w40
#define         bah_bac_reg_wc46        bah_bac_reg_w40
#define         bah_bac_reg_wc48        bah_bac_reg_w40
#define         bah_bac_reg_wc4a        bah_bac_reg_w40

#define         bah_bac_reg_w40         bsh_bac_reg_w40
#define         bah_bac_reg_w42         bsh_bac_reg_w40
#define         bah_bac_reg_w46(val)    {;}
#define         bah_bac_reg_w48         bsh_bac_reg_w48
#define         bah_bac_reg_w4a         bsh_bac_reg_w40_

#define         bah_bap_reg_r04(addr_byte)
#define         bah_bap_reg_r06(addr_byte)

#define         bah_cpu_dma0_w_a_srce   bsh_cpu_dma0_a_srce
#define         bah_cpu_dma0_w_a_dest   bsh_cpu_dma0_a_dest
#define         bah_cpu_dma0_w_c_mf     bsh_cpu_dma0_c_mf
#define         bah_cpu_dma0_w_cw       bsh_cpu_dma0_ctrl

#define         bah_cpu_dma1_w_a_srce   bsh_cpu_dma1_a_srce
#define         bah_cpu_dma1_w_a_dest   bsh_cpu_dma1_a_dest
#define         bah_cpu_dma1_w_c_mf     bsh_cpu_dma1_c_mf
#define         bah_cpu_dma1_w_cw       bsh_cpu_dma1_ctrl

#define         bah_cpu_tmr1_w_cw       bsh_cpu_tmr1_cw
#define         bah_cpu_tmr1_w_cnt_a(val) { bah_dta.ebac.c_tmr1_a = val; \
                                            bsh_cpu_tmr1_cnt_a(val); }
#define         bah_cpu_tmr1_w_cnt      bsh_cpu_tmr1_cnt

#define         bah_cpu_tmr1_ei()       {;}
#define         bah_cpu_dma0_ei()       {;}
#define         bah_cpu_dma1_ei()       {;}

#define         bah_cpu_tmr1_r_c        bsh_cpu_tmr1_r_cnt
#define         bah_cpu_isr_r_tmr1      bsh_cpu_isr_r_tmr1
#define         bah_cpu_eoi_dma()          /* empty function */

#define         bah_poll_send_mf        bsp_bap_send_mf
#define         bah_poll_time           bsp_time

#define         bah_bus_access          bsh_bus_access

#define         bah_send_mf             bsp_bap_send_mf
#define         bah_bap_rcve_sf         bsp_bap_rcve_sf

#define         bah_cpu_dma0_r_l_srce(var)
#define         bah_cpu_dma1_r_l_srce(var)

#define         bah_bac_dma1_set() { bal_dta.io_port.act_dma = BAH_IOP_DMA1;}

#define         bah_bac_atrl_dma(p_val)  { \
                  if (bal_dta.io_port.act_dma == BAH_IOP_DMA0)               \
                  {   bal_dta.io_port.act_dma  = BAH_IOP_DMA1; *p_val = 0; } \
                  else                                                       \
                  {   bal_dta.io_port.act_dma  = BAH_IOP_DMA0; *p_val = 1; }}
#endif

/* new page
 -----------------------------------------------------------------------------
|
|   1.1         BAH_STR_EBAC        BAP-BAC dependant static data
|
 ------------   A B S T R A C T   --------------------------------------------
*/

#else

struct      BAH_STR_EBAC
{
    int     no_ekr2_bac_found_here_the_way_forward_is_very_clear;
};

#endif



#if defined (O_EKR)


#if !defined (BX_SIMU)

#define         bah_inp_wvc(p_val, port_addr)       \
{asm    {\
LES BX, p_val;\
MOV DX, port_addr;\
IN  AX, DX;\
MOV ES: [BX], AX;\
    }\
}

#define         bah_inp_bvc(p_val, port_addr)       \
{asm    {\
LES BX, p_val;\
MOV DX, port_addr;\
IN  AL, DX;\
MOV ES: [BX], AL;\
    }\
}

/*              outport word constant port addr, constant data value */

#define         bah_outp_wcc(port_addr, val)       \
{asm    {\
MOV DX, port_addr;\
MOV AX, val;\
OUT DX, AX;\
    }\
}

/*              outport word constant port addr, variable data value */

#define         bah_outp_wcv(port_addr, val)       \
{asm    {\
MOV DX, port_addr;\
MOV AX, val;\
OUT DX, AX;\
    }\
}

/*              outport byte constant port addr, constant data value */

#define         bah_outp_bcc(port_addr, val)       \
{asm    {\
MOV DX, port_addr;\
MOV AL, val;\
OUT DX, AL;\
    }\
}

/*              outport byte constant port addr, variable data value */

#define         bah_outp_bcv(port_addr, val)       \
{asm    {\
MOV DX, port_addr;\
MOV AL, val;\
OUT DX, AL;\
    }\
}
#endif
#endif


/* new page
 -----------------------------------------------------------------------------
|
|   name        prototypes
|
 ------------   A B S T R A C T   --------------------------------------------

*/

short   bah_bap_slot_verification  (void);

short   bah_bap_wv                 (void);

#endif
