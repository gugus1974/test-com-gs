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
|    File Name    :   BAH_HWRE.H
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

#ifndef   BAH_HWRE_H
#define   BAH_HWRE_H

/* new page
 -----------------------------------------------------------------------------
|
|   name        Includes
|
 ------------   A B S T R A C T   --------------------------------------------



*/

#include "lc_sys.h"

#include "bah_ebac.h"
#include "bah_tcfg.h"
#include "bal_link.h"
                                         #if defined (O_BAP)
#include "bfcomm.h"
                                         #else
#include "mvbc.h"
                                         #endif
                                         #if       defined (BX_SIMU)
#include "bsp_poll.h"
#include "bsh_hwre.h"
#include "bsc_cycl.h"
                                         #endif


/* new page
 ----------------------------------------------------------------------------
|
|   Mapping Definitions
|
 ------------   A B S T R A C T   -------------------------------------------

    The traffic Store mapping is defined here as a function of the
    base pointer supplied in bah_tcfg.h (by the porting engineer)

    If no switch is supplied, the default traffic store model 256 k is used
*/
                                #if   !defined (O_BAP)

                                    #if   (O_LC_MCM == 1)
                                        #if defined (O_BA_RTS)
#define BAH_PO_TS_DMA_CFGD   (0x7800 >> 2)
#define BAH_PO_TS_DMA_NCFG   (0x7A00 >> 2)
                                        #else
#define BAH_PO_TS_DMA_CFGD   (0x8000 >> 2)
#define BAH_PO_TS_DMA_NCFG   (0x3800  >> 2)
                                        #endif
                                        #if    !defined (BX_SIMU)
#define BAH_TS_DSW_PIT       0x00004000L
#define BAH_TS_DSW_PRT       0x00005000L
#define BAH_TS_SVCE          0x00007C00L
#define BAH_TS_IREG          0x00007F80L

#define BAH_TS_DSW_PCS       0x00007000L
                                            #if defined (O_BA_RTS)
#define BAH_TS_DMA_CFGD      0x00007800L
#define BAH_TS_DMA_NCFG      0x00007A00L
                                            #else
#define BAH_TS_DMA_CFGD      0x00008000L
#define BAH_TS_DMA_NCFG      0x00003800L
                                            #endif
                                        #endif
                                    #else
                                        #if    !defined (BX_SIMU)
#define BAH_TS_DSW_PIT       0x00002000L
#define BAH_TS_SVCE          0x0000FC00L
#define BAH_TS_IREG          0x0000FF80L
                                        #endif
                                        #if (O_LC_MCM == 2)

                                            #if defined (O_BA_RTS)
#define BAH_PO_TS_DMA_CFGD   (0xF800 >> 2)
#define BAH_PO_TS_DMA_NCFG   (0xFA00 >> 2)
                                            #else
#define BAH_PO_TS_DMA_CFGD   0x4000     /* 0x10000 */
#define BAH_PO_TS_DMA_NCFG   0x6000     /* 0x18000 */
                                            #endif

                                            #if    !defined (BX_SIMU)
#define BAH_TS_DSW_PCS       0x0000F000L
#define BAH_TS_DSW_PRT       0x0000E000L
                                                #if defined (O_BA_RTS)
#define BAH_TS_DMA_CFGD      0x0000F800L
#define BAH_TS_DMA_NCFG      0x0000FA00L
                                                #else
                                                    #if defined (O_SEG)
#define BAH_TS_DMA_CFGD      0x10000000L
#define BAH_TS_DMA_NCFG      0x10008000L
                                                    #else
#define BAH_TS_DMA_CFGD      0x00010000L
#define BAH_TS_DMA_NCFG      0x00018000L
                                                    #endif
                                                #endif
                                            #endif
                                        #else

#define BAH_PO_TS_DMA_CFGD   (0x6000 >> 2)
#define BAH_PO_TS_DMA_NCFG   (0xE000 >> 2)
                                            #if    !defined (BX_SIMU)
#define BAH_TS_DMA_CFGD      0x00006000L
#define BAH_TS_DMA_NCFG      0x0000E000L
                                            #endif

                                            #if (O_LC_MCM == 4)
                                                #if    !defined (BX_SIMU)
#define BAH_TS_DSW_PCS       0x00038000L
#define BAH_TS_DSW_PRT       0x00040000L
                                                #endif
                                            #else
                                                #if !defined (BX_SIMU)
#define BAH_TS_DSW_PCS       0x00004000L
                                                    #if defined (O_SEG)
#define BAH_TS_DSW_PRT       0x30008000L
                                                    #else
#define BAH_TS_DSW_PRT       0x00038000L
                                                    #endif
                                                #endif
                                            #endif
                                        #endif
                                    #endif
                                    #if    !defined (BX_SIMU)

#define BAH_PB_TS_DSW_PIT    ((LC_TYPE_TS_PIT *   ) (BAH_TS_BASE+BAH_TS_DSW_PIT))
#define BAH_PB_TS_DSW_PCS    ((LC_TYPE_TS_DA_PCS *) (BAH_TS_BASE+BAH_TS_DSW_PCS))
#define BAH_PB_TS_SVCE       ((void *)              (BAH_TS_BASE+BAH_TS_SVCE))
#define BAH_PB_TS_IREG       ((void *)              (BAH_TS_BASE+BAH_TS_IREG))
#define BAH_PB_TS_DSW_PRT    ((LC_TYPE_TS_DA_PRT *) (BAH_TS_BASE+BAH_TS_DSW_PRT))

                                        #if defined (O_BA_RTS)
#define BAH_PB_TS_DMA_CFGD   ( &bah_dta.ba_dma_cfgd)
#define BAH_PB_TS_DMA_NCFG   ( &bah_dta.ba_dma_ncfg)
#define BAH_PB_RTS_DMA_CFGD  ((void *)              (BAH_TS_BASE+BAH_TS_DMA_CFGD))
#define BAH_PB_RTS_DMA_NCFG  ((void *)              (BAH_TS_BASE+BAH_TS_DMA_NCFG))
                                        #else
#define BAH_PB_TS_DMA_CFGD   ((void *)              (BAH_TS_BASE+BAH_TS_DMA_CFGD))
#define BAH_PB_TS_DMA_NCFG   ((void *)              (BAH_TS_BASE+BAH_TS_DMA_NCFG))
                                        #endif
                                    #endif
                                #endif




/* new page
 -----------------------------------------------------------------------------
|
|   name        BAH_STR_DTA         static data
|
 ------------   A B S T R A C T   --------------------------------------------

    The BAC static data stores the BAC relevant information
*/

struct      BAH_STR_DBUG
{
    int     no_debug_today_the_bugs_have_gone_away;
};

struct      BAH_STR_DIV
{
    unsigned short no_second_poll;
    unsigned short t_guard;
    unsigned short c_tmr2;
};


/* new page
 -----------------------------------------------------------------------------
|
|   name        BAH_STR_TS_...      Traffic Store Structure Declarations
|
 ------------   A B S T R A C T   --------------------------------------------

*/

struct  BAH_STR_TS_DMA_CFGD
{
        unsigned short          mf[1024][16];
};

struct  BAH_STR_TS_DMA_NCFG
{
        unsigned short          mf[64][16];
};

struct  BAH_STR_TS_DMA_RTSM
{
        unsigned short          mf[4][32];
};

struct  BAH_STR_CFG_MAP
{
        struct  BAH_STR_TS_DMA_CFGD* pb_dma_cfgd;
        struct  BAH_STR_TS_DMA_NCFG* pb_dma_ncfg;
        LC_TYPE_TS_PIT *             pb_dsw_pit;
        LC_TYPE_TS_DA_PCS *          pb_dsw_pcs;
        LC_TYPE_TS_DA_PRT *          pb_dsw_prt;
#if defined (O_BAP)
        LC_TYPE_TS_PCS *             pb_bmt_src_pcs;
        LC_TYPE_TS_PRT *             pb_bmt_src_prt;
#else
        TM_TYPE_SERVICE_AREA    *    pb_svc;
        TM_TYPE_INT_REGS *           pb_ireg;
#endif
};


/* new page
 -----------------------------------------------------------------------------
|
|   name        bah_p_2_addr_....
|
 ------------   A B S T R A C T   --------------------------------------------

    dma pointer conversion : The DMA controller MVBC expects a 16 bit
    offset address covering 256 k (address / 4).

    The base pointer is located elsewhere.

*/

#if     defined (O_BAP)

#define bah_p_2_addr_cfgd_dma(ptr, ix)  \
        baa_p_2_abs(( ((unsigned char *) ptr) + ((ix) << 5) ))

#define bah_p_2_addr_ncfg_dma   bah_p_2_addr_cfgd_dma


#else
                                        #if  defined (O_BA_RTS)
#define bah_dma_p_srce(ptr, ix) ((void *) (((unsigned char *) ptr) + ((ix) << 5)))
                                        #endif
#define bah_p_2_addr_cfgd_dma(ptr, ix)  \
            (BAH_PO_TS_DMA_CFGD + ((ix) << 3)) ;

#define bah_p_2_addr_ncfg_dma(ptr, ix)  \
            (BAH_PO_TS_DMA_NCFG + ((ix) << 3)) ;
#endif


/* new page
 -----------------------------------------------------------------------------
|
|   name        Timer functionality
|
 ------------   A B S T R A C T   --------------------------------------------

    The MVBC offers two timers

    bus master mode:
    timer 1 synchronises the DMA
    timer 2 runs in parallel to support timer 1 wrap around detection
    timer 2 is not visible to the interface for this duty

    redundant mode:
    timer 2 wakes up the software every 1.3 ms
    this functionality is visible at the interface

*/


#if     defined (O_BAP)

#define bah_tmr_w_cw               bah_cpu_tmr1_w_cw

#define bah_tmr_r_c                bah_cpu_tmr1_r_c
#define bah_tmr_w_c                bah_cpu_tmr1_w_cnt
#define bah_tmr_w_c_max            bah_cpu_tmr1_w_cnt_a
#define bah_tmr_ei                 bah_cpu_tmr1_ei

#define bah_tmr2_w_c(par)          { unsigned short tmp = par; \
                                     tmp=tmp;   }
#define bah_tmr2_w_c_max(par)      { ;   }
#define bah_tmr2_r_c               bah_cpu_tmr1_r_c

#define BAH_TMRS_GO                BAH_CPU_TMR1_GO
#define BAH_TMR1_GO                BAH_CPU_TMR1_GO
#define BAH_TMR2_GO                BAH_CPU_TMR1_GO
#define BAH_TMRS_STOP              BAH_CPU_TMR1_STOP

#define BAH_TMR_0010US             BAH_CPU_TMR1_0010US
#define BAH_TMR_GUARD              BAH_CPU_TMR1_0100US
#define BAH_TMR_1000US             BAH_CPU_TMR1_1000US
#define BAH_TMR_1290US             BAH_CPU_TMR1_1290US
#define BAH_TMR_1300US             BAH_CPU_TMR1_1300US
#define BAH_TMR_1400US             BAH_CPU_TMR1_1400US
#define BAH_TMR_FACTOR             4
#define BAH_TMR_DIVISOR            1

#else

#define bah_tmr_ei()              /* empty function */

#define bah_tmr_w_cw(par)         {bah_dta.ts_map.pb_ireg->tcr.w = par; \
                                   bah_bsh_mvbc_tmr1_w_cw(par); }
#define bah_tmr_r_cw(p_par)       {*p_par = bah_dta.ts_map.pb_ireg->tcr.w; }
#define bah_tmr_w_c(par)          {bah_dta.ts_map.pb_ireg->tc1 = par; }
#define bah_tmr_w_c_max(par)      {bah_dta.ts_map.pb_ireg->tr1 = par; }
#define bah_tmr_r_c(p_par)        {*p_par = bah_dta.ts_map.pb_ireg->tc1;  }

#define bah_tmr2_w_c(par)         {bah_dta.ts_map.pb_ireg->tc2 = par; }
#define bah_tmr2_w_c_max(par)     {bah_dta.ts_map.pb_ireg->tr2 = par; }

#define bah_tmrs_delay()          {bah_dta.ts_map.pb_ireg->tc1 = BAH_TMR_MAX; \
                                   bah_dta.ts_map.pb_ireg->tc2 = BAH_TMR_MAX; \
                                   bah_dta.ts_map.pb_ireg->tr1 = BAH_TMR_MAX; \
                                   bah_dta.ts_map.pb_ireg->tr2 = BAH_TMR_MAX; }

#define bah_ireg_init_level(par)  {tm_put_sa (bah_dta.ts_map.pb_ireg, scr.w, \
                                   TM_SCR_IL_MSK, TM_SCR_IL_OFF, par);       }


#if defined (BX_SIMU)
#define bah_tmr2_r_c(p_par)       {*p_par = bah_dta.div.t_guard - 1;  }
#else
#define bah_tmr2_r_c(p_par)       {*p_par = bah_dta.ts_map.pb_ireg->tc2;  }
#endif
#define BAH_TMRS_STOP              0x0000   /* stop don't reset count */
#define BAH_TMRS_GO                0x0033   /* go,  can't reset count */
#define BAH_TMR1_STOP_TMR2_GO      0x0030   /* go,  can't reset count */
#define BAH_TMR1_GO_TMR2_STOP      0X0003
#define BAH_TMR2_MAX               (0xFFF0)

#define BAH_TMR_GUARD              (BAH_TMR2_4000US + BAH_TMR2_0100US)
#define BAH_TMR_FACTOR             1
#define BAH_TMR_DIVISOR            10

#define BAH_TMR_0010US             1U
#define BAH_TMR_0020US             (BAH_TMR_0010US * (  20/10))
#define BAH_TMR_0100US             (BAH_TMR_0010US * ( 100/10))
#define BAH_TMR_0650US             (BAH_TMR_0010US * ( 650/10))
#define BAH_TMR_1000US             (BAH_TMR_0010US * (1000/10))
#define BAH_TMR_1290US             (BAH_TMR_0010US * (1290/10))
#define BAH_TMR_1300US             (BAH_TMR_0010US * (1300/10))
#define BAH_TMR_1400US             (BAH_TMR_0010US * (1400/10))

#if defined (BX_SIMU)

#else
#define bah_bsh_mvbc_tmr1_w_cw(par) {;}

#endif
#endif

#define BAH_TMR2_BEYOND            (BAH_TMR2_4000US - BAH_TMR2_0100US)

#define BAH_TMR2_0001US            8U
#define BAH_TMR2_0100US            ( 100U * BAH_TMR2_0001US)
#define BAH_TMR2_1000US            (1000U * BAH_TMR2_0001US)
#define BAH_TMR2_1290US            (1290U * BAH_TMR2_0001US)
#define BAH_TMR2_1300US            (1300U * BAH_TMR2_0001US)
#define BAH_TMR2_1400US            (1400U * BAH_TMR2_0001US)
#define BAH_TMR2_2000US            (2000U * BAH_TMR2_0001US)
#define BAH_TMR2_4000US            (4000U * BAH_TMR2_0001US)
#define BAH_TMR2_MAX               (0xFFF0)


/* new page
 -----------------------------------------------------------------------------
|
|   name        DMA functionality
|
 ------------   A B S T R A C T   --------------------------------------------

    The MVBC has its own DMA channel which is triggered by MVBC timer 1

*/

#if defined (O_BAP)

#define bah_dma_w_a_srce           bah_cpu_dma1_w_a_srce
#define bah_dma_w_c_mf             bah_cpu_dma1_w_c_mf
#define bah_dma_w_cw               bah_cpu_dma1_w_cw
#define bah_dma_ei()               { bah_cpu_dma0_ei(); bah_cpu_dma1_ei(); }
#define bah_dma_w_a_dest(par)      { union BAA_UNN_DTA un_dta;      \
                                     un_dta.ul[0] = par;            \
                                     bah_cpu_dma0_w_a_dest(un_dta); \
                                     bah_cpu_dma1_w_a_dest(un_dta); }


#else

#define bah_dma_w_a_dest(par)      /* empty function */
#define bah_dma_w_cw(par)          /* empty function */
#define bah_dma_ei()               /* empty function */

#define bah_dma_w_a_srce(par)      { bah_dta.ts_map.pb_ireg->dpr2 = par; }

#define bah_dma_w_c_mf(par)        \
        { tm_act_sa (bah_dta.ts_map.pb_ireg, mr2.w, \
                     TM_MR_SMFX_MSK, TM_MR_SMFX_OFF, par); }

#define bah_panic_w_a_srce(par)    { bah_dta.ts_map.pb_ireg->dpr = par; }

#define bah_panic_w_c_mf(par)        \
        { tm_act_sa (bah_dta.ts_map.pb_ireg, mr.w, \
                     TM_MR_SMFX_MSK, TM_MR_SMFX_OFF, par); }

#define bah_check_4_panic()    \
( tm_get_sa (bah_dta.ts_map.pb_ireg, mr.w, TM_MR_SMFX_MSK, TM_MR_SMFX_OFF)==0)

#define bah_check_4_dbl_panic()    \
( tm_get_sa (bah_dta.ts_map.pb_ireg, mr2.w, TM_MR_SMFX_MSK, TM_MR_SMFX_OFF)==0)

#define bah_check_4_mcu_deadlock()     \
( tm_get_sa (bah_dta.ts_map.pb_ireg, mr.w, TM_MR_BUSY_MSK, TM_MR_BUSY_OFF)==1)



#define bah_dma_kick_start_w_a_srce(par) { bah_dta.ts_map.pb_ireg->dpr = par; }
#define bah_dma_kick_start_w_c_mf(par)    \
        { tm_act_sa (bah_dta.ts_map.pb_ireg, mr.w, \
                     TM_MR_SMFX_MSK, TM_MR_SMFX_OFF, par); }

#define bah_ireg_r_dpr2(p_par) { *p_par = bah_dta.ts_map.pb_ireg->dpr2.w; }
#define bah_ireg_r_dpr(p_par)  { *p_par = bah_dta.ts_map.pb_ireg->dpr.w;  }

#define bah_ireg_r_mr2(p_par) { *p_par = bah_dta.ts_map.pb_ireg->mr2.w;  }
#define bah_ireg_r_mr(p_par)  { *p_par = bah_dta.ts_map.pb_ireg->mr.w;   }

#endif





/* new page
 -----------------------------------------------------------------------------
|
|   name        Redirected / Bus Controller Dependant functionality
|
 ------------   A B S T R A C T   --------------------------------------------


*/

#if     defined (O_BAP)

#define bah_tm_rcve_sf             bah_bap_rcve_sf
#define bah_pl_rcve_sf             bah_bap_rcve_sf

#define bah_bm_disable             bah_bac_mabv_dis
#define bah_bm_enable              bah_bac_2_bm

#define bah_mvbc_bm_enable()       {; }

#define bah_reset_silence_monitor  bah_bac_at_reset
#define bah_satn                   bah_bac_satn

#define bah_mvbc_fc_reset()        {; }

#else

#define bah_c_error_frame_read(p_par)  \
        { *p_par = tm_get_sa (bah_dta.ts_map.pb_ireg, ec, 0xFFFF, 0); }

#define bah_c_error_frame_reset()      \
        { tm_act_sa(bah_dta.ts_map.pb_ireg, ec, 0xFFFF, 0, 0); }

#define bah_c_total_frame_reset()      \
        { tm_act_sa(bah_dta.ts_map.pb_ireg, fc, 0xFFFF, 0, 0); }

#define bah_bm_disable() baa_delay()
/*  \
        {  tm_put_sa (bah_dta.ts_map.pb_ireg, scr.w, \
                      TM_SCR_MAS_MSK, TM_SCR_MAS_OFF, TM_SCR_MAS_CLR); }
*/
#define bah_mvbc_bm_enable()           \
        {  tm_put_sa (bah_dta.ts_map.pb_ireg, scr.w, \
                      TM_SCR_MAS_MSK, TM_SCR_MAS_OFF, TM_SCR_MAS_SET); }

#define bah_smfe_cancel()
/*         \
        { tm_act_sa (bah_dta.ts_map.pb_ireg, mr.w, \
                     TM_MR_CSMF_MSK, TM_MR_CSMF_OFF, TM_MR_CSMF_SET); }
*/

#define bah_pl_rcve_sf(p_q)  (bah_rcve_sf (p_q, TM_PP_EFS))
#define bah_tm_rcve_sf(p_q)  (bah_rcve_sf (p_q, TM_PP_MOS))

#define bah_reset_silence_monitor  bah_mvbc_fc_reset

#define bah_wv                     bah_mvbc_wv
#define bah_send_mf                bah_mvbc_send_mf

#define bah_rcve_bmt_sf            bah_mvbc_rcve_bmt_sf

#define bah_mvbc_mcr_mo_40000H()    {bah_dta.ts_map.pb_ireg->mcr |= 0x20; }

#define bah_mvbc_fc_in(p_par)       {*p_par = bah_dta.ts_map.pb_ireg->fc; }
#define bah_mvbc_fc_reset()         {bah_dta.ts_map.pb_ireg->fc = 0; }

#define bah_mvbc_mr2_put(par)       {bah_dta.ts_map.pb_ireg->mr2.w = par;   \
                                     bah_bsh_mvbc_mr2_put(par); }
#define bah_mvbc_mfs_put(par)       {bah_dta.ts_map.pb_svc->mfs   = par; }

                                   /*  not used under MVBC   */
#define bah_bap_skrl_gap_set()
#define bah_bac_dma1_set()
#define bah_cpu_dma0_w_cw(par)
#define bah_cpu_eoi_dma()          /* empty function */

#define bah_mvbc_pb_svc_pp_pcs(ix) ((LC_TYPE_TS_PCS *) \
                                    &bah_dta.ts_map.pb_svc->pp_pcs[ix])
#define bah_mvbc_sf_put(sf,ix)                                             \
        { int vp;                                                          \
          LC_TYPE_TS_PCS * p_pcs = bah_mvbc_pb_svc_pp_pcs(ix);        \
          lc_get_pcs (&vp, p_pcs, LC_PCS_MSK_VP, LC_PCS_OFF_VP);  \
          vp ^= 1;                                                         \
          tm_1_data_wd(bah_dta.ts_map.pb_svc->pp_data, ix, vp, 0) = sf; \
          lc_put_pcs (p_pcs, LC_PCS_MSK_VP, LC_PCS_OFF_VP, vp);   \
        }

#define bah_mvbc_sf_get(p_sf,ix)                                        \
    { int vp;                                                           \
      LC_TYPE_TS_PCS * p_pcs = bah_mvbc_pb_svc_pp_pcs(ix);         \
      lc_get_pcs (&vp, p_pcs, LC_PCS_MSK_VP, LC_PCS_OFF_VP);   \
      *p_sf = tm_1_data_wd(bah_dta.ts_map.pb_svc->pp_data, ix, vp, 0);     \
    }

#endif




#if             defined (BX_SIMU)

#define         bah_time_enq_beyond()   RET_OK



#if             !defined (O_BAP)

#define bah_mvbc_send_mf(mfa)        bsh_send_mf(&mfa,1)
#endif
#else                                            /*   End of BX_SIMU        */

#define     bah_ekr2_waitstates()   {bah_outp_wcc(0x4314, 0);   \
                                     bah_outp_wcc(0x7FA0, 0xC038); }

short       bah_time_enq            (void);
short       bah_time_enq_beyond     (void);

#if !defined (O_BAP)

#define bah_bsh_mvbc_mr_put(par)         {;}
#define bah_bsh_mvbc_mr2_put(par)        {;}

#define     BAH_IOP_TMR1_CW

#define bah_mvbc_send_mf(mf)                \
{       bah_mvbc_mfs_put(mf);               \
        tm_act_sa (bah_dta.ts_map.pb_ireg, mr.w, \
                   TM_MR_SMFM_MSK, TM_MR_SMFM_OFF, TM_MR_SMFM_SET); }

#endif
#endif

/*
 ---------------------------------------------------------------------------Ä
|
|           Constants
|
 ----------------------------------------------------------------------------

    The DMA control word sets the mode of operation. There are two modes of
    operation which are supported: if the slot is full, no interrupt is
    needed after transmission, but if the slot is not full, an interrupt upon
    DMA channel empty is required to call the poll handler.

*/

#define         BAH_DMA_OFF                 0x0004

#define         BAH_DMA_CW_POLL_ON          0xF7A7
#define         BAH_DMA_CW_POLL_OFF         0xF6A7

#define         BAH_DMA_CW_POLL             0x0100

/*  new page
 ----------------------------------------------------------------------------
|
|   constant    io ports
|
 ------------   A B S T R A C T   -------------------------------------------

    These constants define the resource location.

*/

/*  new page
 ----------------------------------------------------------------------------
|
|   constant    io ports
|
 ------------   A B S T R A C T   -------------------------------------------

    These constants define the resource location.

*/

#if             defined (O_BAP)
#define         BAH_IOP_INT             BAH_CPU_IOP_INT
#define         BAH_IOP_EXT             BAH_BAC_IOP_EXT
#define         BAH_LOC_SATN            BAH_BAP_LOC_SATN

#endif


#if defined    (O_BAP)

#define         BAH_DMA1                0xFF
#define         BAH_DMA0                0xFE

#define         BAH_IOP_ISR             (BAH_IOP_INT + 0x30)

#define         BAH_IOP_DMA0            (BAH_IOP_INT + 0xC0)
#define         BAH_IOP_DMA1            (BAH_IOP_INT + 0xD0)
#define         BAH_IOP_EOI             (BAH_IOP_INT + 0x22)


#define         BAH_IOP_DMA_CW          0x0A
#define         BAH_IOP_DMA_CNT         0x08
#define         BAH_IOP_DMA_P_DH        0x06
#define         BAH_IOP_DMA_P_DL        0x04
#define         BAH_IOP_DMA_P_SH        0x02
#define         BAH_IOP_DMA_P_SL        0x00

#define         BAH_IOP_DMA0_CW         (BAH_IOP_DMA0 + BAH_IOP_DMA_CW  )
#define         BAH_IOP_DMA0_CNT        (BAH_IOP_DMA0 + BAH_IOP_DMA_CNT )
#define         BAH_IOP_DMA0_H_DEST     (BAH_IOP_DMA0 + BAH_IOP_DMA_P_DH)
#define         BAH_IOP_DMA0_L_DEST     (BAH_IOP_DMA0 + BAH_IOP_DMA_P_DL)
#define         BAH_IOP_DMA0_H_SRCE     (BAH_IOP_DMA0 + BAH_IOP_DMA_P_SH)
#define         BAH_IOP_DMA0_L_SRCE     (BAH_IOP_DMA0 + BAH_IOP_DMA_P_SL)

#define         BAH_IOP_DMA1_CW         (BAH_IOP_DMA1 + BAH_IOP_DMA_CW  )
#define         BAH_IOP_DMA1_CNT        (BAH_IOP_DMA1 + BAH_IOP_DMA_CNT )
#define         BAH_IOP_DMA1_H_DEST     (BAH_IOP_DMA1 + BAH_IOP_DMA_P_DH)
#define         BAH_IOP_DMA1_L_DEST     (BAH_IOP_DMA1 + BAH_IOP_DMA_P_DL)
#define         BAH_IOP_DMA1_H_SRCE     (BAH_IOP_DMA1 + BAH_IOP_DMA_P_SH)
#define         BAH_IOP_DMA1_L_SRCE     (BAH_IOP_DMA1 + BAH_IOP_DMA_P_SL)


#define         BAH_IOP_TMR1_IRPT       (BAH_IOP_INT  + 0x32)
#define         BAH_IOP_DMA0_IRPT       (BAH_IOP_INT  + 0x34)
#define         BAH_IOP_DMA1_IRPT       (BAH_IOP_INT  + 0x36)


#define         BAH_IOP_TMR1            (BAH_IOP_INT  + 0x58)

#define         BAH_IOP_TMR_CW          0x06
#define         BAH_IOP_TMR_CNT_B       0x04
#define         BAH_IOP_TMR_CNT_A       0x02
#define         BAH_IOP_TMR_CNT         0x00

#define         BAH_IOP_TMR1_CW         (BAH_IOP_TMR1 + BAH_IOP_TMR_CW   )
#define         BAH_IOP_TMR1_CNT_B      (BAH_IOP_TMR1 + BAH_IOP_TMR_CNT_B)
#define         BAH_IOP_TMR1_CNT_A      (BAH_IOP_TMR1 + BAH_IOP_TMR_CNT_A)
#define         BAH_IOP_TMR1_CNT        (BAH_IOP_TMR1 + BAH_IOP_TMR_CNT  )

#define         BAH_IOP_BAP_R00         (BAH_IOP_EXT  + 0x00)
#define         BAH_IOP_BAP_R02         (BAH_IOP_EXT  + 0x02)
#define         BAH_IOP_BAP_R04         (BAH_IOP_EXT  + 0x04)
#define         BAH_IOP_BAP_R06         (BAH_IOP_EXT  + 0x06)

#define         BAH_IOP_BAC_R40         (BAH_IOP_EXT  + 0x40)
#define         BAH_IOP_BAC_R42         (BAH_IOP_EXT  + 0x42)
#define         BAH_IOP_BAC_R44         (BAH_IOP_EXT  + 0x44)
#define         BAH_IOP_BAC_R46         (BAH_IOP_EXT  + 0x46)
#define         BAH_IOP_BAC_R48         (BAH_IOP_EXT  + 0x48)
#define         BAH_IOP_BAC_R4A         (BAH_IOP_EXT  + 0x4A)
#define         BAH_IOP_BAC_R4C         (BAH_IOP_EXT  + 0x4C)

#define         BAH_BAC_R40_AT_MASK     0xE0
#define         BAH_BAC_R40_DT_MASK     0x1C
#define         BAH_BAC_R40_EM_MASK     0x02

#define         BAH_BAC_R40_AT_VAL      0x80
#define         BAH_BAC_R40_AT_MISSING  0x40
#define         BAH_BAC_R40_AT_CORRUPT  0x20

#define         BAH_BAC_R40_DT_VAL      0x10
#define         BAH_BAC_R40_DT_MISSING  0x08
#define         BAH_BAC_R40_DT_CORRUPT  0x04


#define         BAH_RSP_BNI                 4
#define         BAH_RSP_TO                  1
#define         BAH_RSP_TERR                2


#define         BAH_BAC_R40_ATR_EM      0x02
#define         BAH_BAC_R48_MABV        0xA0

#endif

#define         bah_slot_time_left(xxx_dta) \
                     (xxx_dta.p_bah->c_timer1)



/* new page
 -----------------------------------------------------------------------------
|
|   name        BAH_STR_DTA         static data
|
 ------------   A B S T R A C T   --------------------------------------------

    The BAC static data stores the BAC relevant information
*/


struct      BAH_STR_DTA
{
            struct BAH_STR_DBUG    dbug;
            struct BAH_STR_DIV     div;
            struct BAH_STR_CFG_MAP ts_map;
#if defined (O_BAP) || defined (BX_SIMU) || defined (O_BA_RTS)
            struct BAH_STR_TS_DMA_CFGD  ba_dma_cfgd;
            struct BAH_STR_TS_DMA_NCFG  ba_dma_ncfg;
                                        #endif
                                        #if defined (O_BAP)
            struct BAH_STR_EBAC    ebac;
                                        #endif
};

extern  struct BAH_STR_DTA      bah_dta;


/* new page
 -----------------------------------------------------------------------------
|
|   prototypes
|
 ------------   A B S T R A C T   --------------------------------------------


*/

void            bah_open                (struct BAH_STR_DTA **  p_bah_dta);

void            bah_init                (void);

void            bah_init_no_second_poll (void);

void            bah_tmr_init            (unsigned short c_max,
                                         unsigned short t_guard);

void            bah_put_t_guard         (unsigned short  t_guard);

short           bah_check_t_guard       (void);
short           bah_check_slot 			(void);

short           bah_check_window    	(void);


#if !defined (O_BAP)


short	 	    bah_rcve_sf            (struct BAL_STR_RSP *	p_q,
                                        unsigned int            ix_pp);
void            bah_tmr1_interrupt     (void);
void            bah_tmr2_wdog_trigger  (void);

#endif


#if defined (O_BAP)
#define         bah_bap_func(param)    param
#define         bah_mvbc_func(param)   { ; } /* empty function */
#else
#define         bah_bap_func(param)    { ; } /* empty function */
#define         bah_mvbc_func(param)   param
#endif

#if defined (O_EKR)
#define         bah_ekr_func(param)    param
#else
#define         bah_ekr_func(param)    { ; } /* empty function */
#endif






#endif
