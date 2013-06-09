/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> bal_mcu_deadlock() is called ONLY from the timer2 to avoid wrong data in the frames */
/* <ATR:02> added various debug counters                                                        */
/*==============================================================================================*/


/*  BAL_LINK.C
 ----------------------------------------------------------------------------
|
|   Property of  :   ABB Verkehrssysteme AG, CH-8050 ZÅrich / Switzerland
|   COPYRIGHT    :   (c) 1992 ABB Verkehrssysteme AG
|
 ----------------------------------------------------------------------------
|
|   Project      :   T C N   B U S   A D M I N I S T R A T O R
|
 ----------------------------------------------------------------------------
|
|   File Name    :   BLL_LINK
|
|   Document     :
|
|   Abstract     :   MVB Cyclic Master Frame Transmission, Link Layer
|
|   Remarks      :
|
|   Dependencies :
|
|   Accepted     :
|
 ----------------------------------------------------------------------------
|
|   HISTORY:
|
|   Vers  Dept.   YY-MM-DD  State    Name    Reference
|   ----  ------  --------  -------  ------- ---------
|     1   BAEI-2  93-06-28  created  Marsden
|
 ----------------------------------------------------------------------------
*/


#define      BAL_LINK_C

#include    "pi_sys.h"

#include    "baa_auxi.h"
#include    "bad_diag.h"
#include    "bah_hwre.h"
#include    "bah_tcfg.h"
#include    "bai_ifce.h"
#include    "bal_link.h"
#include    "bam_main.h"
#include    "bap_poll.h"
#include    "bat_tfer.h"
#include    "bax_incl.h"

                                        #if defined (O_BAP)
#include    "baf_fram.h"
#include    "bao_scfg.h"
                                        #endif


struct BAL_STR_INIT_SLOT
{
struct  BAL_STR_CP_P    ca[BA_CA_CNT];  /* define active ptr pair per alloc */
unsigned short        * p_cm_1ms;       /* temp pointer to 1 ms mf list     */
unsigned short          c_cm_1ms;       /* temp pointer to 1 ms mf list     */
struct  BA_STR_CFG_SCN *p_scan;
struct  BAL_STR_CP_P  * p_cp;           /* temp ptr to base   ptr for pair  */
struct  BAL_STR_SL    * p_slot_0;
int                     sl_cnt;         /* slot count, from 0 to 1023       */
unsigned short          fi_slot;        /* !!! will always be one later     */
unsigned short          fx_slot;        /* !!! will always be one later     */
};


/*  new page
 ----------------------------------------------------------------------------
|
|   external   static data
|
|
 ------------   A B S T R A C T   -------------------------------------------

    Pointers to static data from other modules. They are not used by this
    module, but passed to functions belonging to the external module to
    allow macro processing for speed.

*/

/*static*/ unsigned short c_amfx_panic;
static unsigned short c_amfx_panic_old = 1;

/* <ATR:02> */
unsigned short emf_panic_count;			/* counter of BA stops triggered by too many EMFs  */
unsigned short emf_bm_count;			/* counter of EMFs in BM state                     */
unsigned short emf_slv_count;			/* counter of EMFs in other states                 */
unsigned short bti_deadlock_count;		/* counter of deadlocks discovered by BTIs         */
unsigned short bti_bm_count;			/* counter of BTIs in BM state                     */
unsigned short bti_slv_count;			/* counter of BTIs in other states                 */
unsigned short fev_count;				/* counter of FEVs                                 */
unsigned short dti6_bm_count;			/* counter of DTI6s in BM state                    */
unsigned short dti6_slv_count;			/* counter of DTI6s in other states                */
unsigned short tmr2_ra_silence_count;	/* counter of TMR2s in RA with silence on the line */
unsigned short tmr2_ra_noise_count;		/* counter of TMR2s in RA with noise on the line   */
unsigned short tmr2_deadlock_count;		/* counter of deadlocks discovered by TMR2s        */
unsigned short tmr2_panic_count;		/* counter of BTIs panics                          */


/*  new page
 ----------------------------------------------------------------------------
|
|   prototypes
|
 ------------   A B S T R A C T   -------------------------------------------
*/

int          bal_amfx_nxt_dma   (void);
void         bal_amfx_panic     (void);
void         bal_blink_bm       (void);
void         bal_dummy_0        (void);
void         bal_init_slot      (struct BAL_STR_INIT_SLOT * p_is,
                                 unsigned short *           p_c_mf,
                                 unsigned short *           p_t_us);
void         bal_init_init_slot (struct BAL_STR_INIT_SLOT * p_iss,
                                 struct BAM_STR_CFG_PT *    p_pt,
                                 unsigned short             fi_slot,
                                 unsigned short             fx_slot);
void         bal_sl_pad         (unsigned *            c_pad,
                                    unsigned short *      p_sl,
                                    unsigned              c_mf,
                                    unsigned              t_us);

void         bal_mvbc_tmr2        (unsigned short not_used);
void         bal_mvbc_bti         (unsigned short not_used);
void         bal_mvbc_emf         (unsigned short not_used);
void         bal_mvbc_fev         (unsigned short not_used);
void         bal_mvbc_mstr_tsfr   (unsigned short not_used);


unsigned short  bap_q_n_pf      (void);



#if     defined (O_BA_RTS)

void    bal_init_rts  (void);

#else

#define bal_init_rts()
#endif

/*  new page
 -----------------------------------------------------------------------------
|
|   name        bal_p_dta           static data access function
|
 ------------   A B S T R A C T   --------------------------------------------


    The BAC static data stores the BAC relevant information, see
    discription in header file


*/

static  struct BAL_STR_DTA  bal_dta;


void            bal_open    (struct BAL_STR_DTA **  p_bal_dta)
{
        *p_bal_dta = &bal_dta;
}




/*  new page
 ----------------------------------------------------------------------------
|
|   local prototypes
|
 ------------   A B S T R A C T   -------------------------------------------
*/

void            bal_blink_ra      (void);

void            bal_mcu_deadlock  (void);



/*  new page
 ----------------------------------------------------------------------------
|
|   name        bal_init
|
 ------------   A B S T R A C T   -------------------------------------------

*/
#ifndef O_BAP
#define my_empty(param)
#endif
void            bal_init            (void)

{
struct  BAL_STR_SI_NXT *    p_cb;
struct  BAL_STR_SI_NXT *    p_cb_lst;
struct  BAH_STR_TS_DMA_NCFG *    p_dma;
unsigned short *    p_mf;
int                 i_slot;
unsigned short      my_da;
unsigned short      c_da;


    bai_memset (&bal_dta, 0, sizeof (bal_dta));

    bai_get_dvc_addr(&my_da);

    bal_dta.mf_padding  = 0xF000 + my_da;
    bal_dta.mf_transfer = 0x8000 + my_da;

    bal_dta.ra.c_reference = BAM_RA_T_WAIT_NCFG + my_da;
#if defined (O_BAP)
                                        /* BAC initialisation routine */
    bah_bac_reg_wc46(0xFF);
    bah_bac_reg_wc42(0xFF);
    bah_bac_reg_wc40(0xFF);
    bah_bac_reg_wc48(0x27);             /* not MABV */
#endif
    p_cb_lst  = &bal_dta.ncfg.cb[BAL_NCFG_SL_CNT - 1];
    bal_dta.ncfg.p_lst  = p_cb_lst;
    bal_dta.ncfg.c_tmr1 = BAH_TMR_1000US;

    i_slot = 0;
    c_da   = 0;

    p_cb = &bal_dta.ncfg.cb[0];

    p_dma=  bah_dta.ts_map.pb_dma_ncfg;

    while   (i_slot < BAL_NCFG_SL_CNT)
    {
                                        #if defined (O_BA_RTS)
        p_cb->dma.p_srce       = bah_dma_p_srce (p_dma, i_slot);
                                        #else
        p_cb->dma.a_srce       = bah_p_2_addr_ncfg_dma (p_dma, i_slot);
                                        #endif
#if defined (O_BAP)
        p_cb->dma.c_mf         = 5;
#else
        p_cb->dma.c_mf         = 5 | TM_MR_SMFT;
#endif
        p_cb->dma.w_ctrl       = BAH_DMA_CW_POLL_OFF;

        p_mf                   = &p_dma->mf[i_slot][0];
        *p_mf++                = 0xF000 + c_da++;
        *p_mf++                = 0xF000 + c_da++;
        *p_mf++                = 0xF000 + c_da++;
        *p_mf++                = 0xF000 + c_da++;
        p_cb->poll.p_mf        = p_mf;
        *p_mf                  = bal_dta.mf_padding;

        p_cb_lst->dma.p_slt_nxt  = p_cb;
        p_cb_lst->poll.p_nxt     = p_cb;
        p_cb_lst                 = p_cb;
        p_cb++;

        i_slot++;
    }

    bal_dta.nivana_slot.dma.p_slt_nxt = &bal_dta.nivana_slot;
    bal_dta.nivana_slot.dma.w_ctrl    = BAH_DMA_OFF;
#if !defined (O_BAP)
    bal_dta.nivana_slot.dma.c_mf      = 0xC0;
#endif
    bal_dta.nivana_slot.poll.p_mf     = &bal_dta.nivana_slot.dma.t_us;
    bal_dta.nivana_slot.poll.p_nxt    = &bal_dta.nivana_slot;

    bah_bap_func  (bah_tmr_w_cw (BAH_TMRS_STOP));

    bah_tmr_ei   ();
    bah_dma_ei    ();

    bal_init_rts ();

    bah_mvbc_bm_enable();

    bai_mvbc_lc_int_connect (bal_mvbc_amfx,     LC_INT_AMFX, LC_KERNEL, 0);

    bai_mvbc_lc_int_connect (bal_mvbc_mstr_tsfr,LC_INT_DTI6,LC_NO_KERNEL,0);
    bai_mvbc_lc_int_connect (bal_mvbc_tmr2, LC_INT_TIMER_2, LC_NO_KERNEL, 0);
    bai_mvbc_lc_int_connect (bal_mvbc_bti,  LC_INT_BTI    , LC_NO_KERNEL, 0);
    bai_mvbc_lc_int_connect (bal_mvbc_fev , LC_INT_FEV    , LC_NO_KERNEL, 0);
    bai_mvbc_lc_int_connect (bal_mvbc_emf , LC_INT_EMF    , LC_NO_KERNEL, 0);

}


#if     defined (O_BA_RTS)

void    bal_init_rts  (void)

{
int ix_a, ix_b, ix_c, ix_d;

struct  BAH_STR_TS_DMA_RTSM * p_cfgd_buffer = BAH_PB_RTS_DMA_CFGD;
struct  BAH_STR_TS_DMA_RTSM * p_ncfg_buffer = BAH_PB_RTS_DMA_NCFG;


    for (ix_a = 0, ix_b = 2; ix_a <= 3; ix_a++, ix_b = ((ix_b + 1) & 3))
    {
        ix_b = (ix_a + 1) & 3;
        ix_c = (ix_a + 2) & 3;
        ix_d = 2 * ix_c;

        bal_dta.rts_cfgd[ix_a].p_nxt_rts    = &bal_dta.rts_cfgd[ix_b];
        bal_dta.rts_cfgd[ix_a].a_srce       =  bah_p_2_addr_cfgd_dma(0, ix_d);
        bal_dta.rts_cfgd[ix_a].p_dma_buffer = &p_cfgd_buffer->mf[ix_c][0];

        bal_dta.rts_ncfg[ix_a].p_nxt_rts    = &bal_dta.rts_ncfg[ix_b];
        bal_dta.rts_ncfg[ix_a].a_srce       =  bah_p_2_addr_ncfg_dma(0, ix_d);
        bal_dta.rts_ncfg[ix_a].p_dma_buffer = &p_ncfg_buffer->mf[ix_c][0];
    }
}

#endif


/* new page
 -----------------------------------------------------------------------------
|
|   name        bal_init_slot_lists     slot list generation
|
 ------------   A B S T R A C T   --------------------------------------------


    The cyclic master frame lists are converted into 1024 separate slot lists
    which contain exactly those master frames which are to be transmitted.
    The slot lists are organized in two blocks because the TS does not
    provide a 64 k block for MVBC support. These slot lists are filled with
    master frames taken from the cyclic master frame lists which are arrays
    of short (master frames). The location of each array is stored in the
    cyclic master frame pointer table.

    The array count is coded in the slot allocation lists. There are five
    slot allocation counter arrays. The location of each array is stored
    in the cyclic master frame pointer table. Each counter is a pair of
    counters, the low byte being the count for the low cycle, the high
    byte the count for the high cycle. The master frame pointers come in
    pairs as to match these allocation count pairs. The slot allocation
    counter array is no longer than necessary. For  example, there are four
    counters for a 4 ms slot allocation array. The pointers are reset to the
    beginning of the array using the sax_masks.

*/

void     bal_init_init_slot     (struct BAL_STR_INIT_SLOT * p_iss,
                                 struct BAM_STR_CFG_PT *    p_pt,
                                 unsigned short             fi_slot,
                                 unsigned short             fx_slot)
{
struct BAL_STR_CX_PT * pb_pt = (struct BAL_STR_CX_PT *) p_pt->p_cpt;

    p_iss->p_slot_0 = (struct BAL_STR_SL *) bah_dta.ts_map.pb_dma_cfgd;
    p_iss->sl_cnt   = 0;
    p_iss->p_cm_1ms = pb_pt->c1[0].p_cm_1ms;
    p_iss->c_cm_1ms = pb_pt->c1[0].cm_cnt;
    p_iss->p_cp     = &pb_pt->cp[0];
    p_iss->fi_slot  = fi_slot;
    p_iss->fx_slot  = fx_slot;
    p_iss->p_scan   = p_pt->p_scn;
}

void    bal_init_slot     (struct BAL_STR_INIT_SLOT * p_is,
                           unsigned short *           p_c_mf,
                           unsigned short *           p_t_us)
{

struct  BAL_STR_SL *    pb_slot;
struct  BAL_STR_CP_P  * p_ca;           /* temp ptr to active ptr pair      */
struct  BAL_STR_CP_P  * p_cm;           /* temp ptr to base   ptr for pair  */
unsigned short        * p_mf_001;       /* temp pointer to 1 ms mf list     */
unsigned short        * p_slot;         /* fill this slot with mf's         */
unsigned int            i;
unsigned int            mf_cnt;         /* mf count for slot                */
unsigned int            al_cnt;         /* sa per list loop count  0 to 4   */
unsigned int            sa_cnt;         /* mf per cycle loop count (alloc)  */
unsigned int            sal_cnt;        /* mfs allocated from low  cycle    */
unsigned int            sah_cnt;        /* mfs allocated from high cycle    */

unsigned int            sal_mask;       /* reset mf low ptr when 0 unmasked */
unsigned int            sah_mask;       /* reset mf hgh ptr and sa ptr      */
unsigned int            t_slot;
unsigned                c_scan = 0;
unsigned                c_pad  = 0;

#if defined (O_BAP)
#define MY_INIT_MF_CNT 0
#else
#define MY_INIT_MF_CNT 0x0080           /* MVBC master register, SMFT */
#endif
         pb_slot =  p_is->p_slot_0 + (p_is->sl_cnt * p_is->fi_slot);
         p_slot  =  (unsigned short *) pb_slot;

        for (i = 0, mf_cnt = 0; i < p_is->fx_slot; i++, p_is->sl_cnt++)
        {
            p_mf_001 = p_is->p_cm_1ms;

            c_scan += p_is->p_scan->c_slot[p_is->sl_cnt];
            for (sa_cnt = 0; sa_cnt < p_is->c_cm_1ms; sa_cnt++)
            {
                *p_slot++ = *p_mf_001++;
                mf_cnt++;
            }

            for (al_cnt   = 0,            sal_mask = 0x0001,
                 p_ca     = &p_is->ca[0], sah_mask = 0x0003;
                 al_cnt   < BA_CA_CNT;
                 al_cnt++, p_ca++                         )
            {
                p_cm = (p_is->p_cp) + al_cnt;

                if  ((p_is->sl_cnt & sal_mask) == 0)
                {
                    p_ca->p_low = p_cm->p_low;

                    if  ((p_is->sl_cnt & sah_mask) == 0)
                    {
                        p_ca->p_hgh = p_cm->p_hgh;
                        p_ca->p_ca  = ((p_is->p_cp) + al_cnt)->p_ca;
                    }
                }

                sal_cnt = p_ca->p_ca->  c_low;
                sah_cnt = p_ca->p_ca++->c_hgh;

                for (sa_cnt = 0; sa_cnt < sal_cnt; sa_cnt++)
                {
                    *p_slot++ = *(p_ca->p_low++);
                    mf_cnt++;
                }
                for (sa_cnt = 0; sa_cnt < sah_cnt; sa_cnt++)
                {
                    *p_slot++ = *(p_ca->p_hgh++);
                    mf_cnt++;
                }
                sal_mask <<= 2;
                sah_mask <<= 2;
                sal_mask  |= 3;
                sah_mask  |= 3;
            }
        }

        bad_announce (p_slot, c_scan);
        p_slot = p_slot + c_scan;
        mf_cnt += c_scan;

        bal_slot_time (&t_slot, pb_slot, mf_cnt);
        bal_sl_pad (&c_pad, p_slot, mf_cnt, t_slot);
        if (c_pad != 0)
        {
            p_slot = p_slot + c_pad;
            mf_cnt += c_pad;
            bal_slot_time (&t_slot, pb_slot, mf_cnt);
        }
        *p_slot = bal_dta.mf_padding;
        mf_cnt++;
#if defined (O_BAP)
        *p_c_mf = mf_cnt;
#else
        *p_c_mf = mf_cnt | 0x80;
#endif
        *p_t_us = t_slot;
}

/* new page
 ------------------------------------------------------------------------------
|
|   name        bal_init_cpt
|
 ------------   A B S T R A C T   ---------------------------------------------

    The slot table contains 1024 separate slot lists, organised in slot
    blocks. Each slot block contains BAC_SL_CNT slot lists. A slot list is
    an array of master frames which must all be transmitted in this (1ms) slot.

    BAP-BAC: The slot block and slot segment size is identical. The slot
    segment is a static array. The location of the static array is written
    to the first slot block pointer.

    The index table for the cyclic master frame lists was created by
    BVLCONV. This index table is translated into a C pointer table.

    The index pointers consist of 11 offset pointers for the cycle mf list
    and 5 offset pointers to the slot allocation counts.

    A cyclic mf list is defined as not existant when the index for the next
    (towards longer cycles) cycle is identicle with the index of the target
    cycle. This translates to a NULL pointer in the pointer table.


*/

int           bal_init_cpt     (struct BAL_STR_CX_PT *    p_pt,
                                struct BA_STR_BA_CFG *    p_bax_cfg)
{
struct BA_STR_CYC_IX_PTR_TAB * p_it;
char *  p_char;
int     i;                                  /* loop counter                 */
int     k;                                  /* auxiliary index              */

int     result = RET_OK;

    p_char =  ((char *) p_bax_cfg + p_bax_cfg->i_ctl);
    p_it   =  (struct BA_STR_CYC_IX_PTR_TAB *) p_char;


    p_pt->c1[0].p_cm_1ms = p_it->i_cl[0] == p_it->i_cl[1] ? NULL :
                               (unsigned short *)(p_char + p_it->i_cl[0]);

    for     (i = 0; i < BA_CA_CNT; i++)
    {
        k = 2*i;

        p_pt->cp[i].p_low = p_it->i_cl[k+1] == p_it->i_cl[k+2] ? NULL :
                                   (unsigned short *) (p_char + p_it->i_cl[k+1]);

        p_pt->cp[i].p_hgh = p_it->i_cl[k+2] == p_it->i_cl[k+3] ? NULL :
                                   (unsigned short *) (p_char + p_it->i_cl[k+2]);

        p_pt->cp[i].p_ca = (struct BA_STR_CFG_SL *)(p_char + p_it->i_sl[i]);
    }

	p_pt->c1[0].cm_cnt  = (p_it->i_cl[1] - p_it->i_cl[0]) >> 1;

    return (result);
}


/*  new page
 ----------------------------------------------------------------------------
|
|   name        bal_create          initialise the slot information array
|
 ------------   A B S T R A C T   -------------------------------------------

    The slot information array contains the data which is written to the
    DMA controller and the timer each time a new slot is attached to the
    DMA. Saving it here saves time on-line.

    The "bal_dma" data consists of a chained list linking the slot
    descriptors together, the last link is attached to the first link

    Each slot contains the count for the timer
    Each slot contains the countdown till the end of the period

    If the time remaining in the slot is sufficient, then an extra
    telegramm is added ("padding") to this slot, which can be written in
    real time by the software. A default value "own device poll" is used
    when no poll active !!! analyze whether necessary!.

    The "bal_poll" data consists of a chained list linking the slot
    descriptors together which can poll.

    Two links exist between the slot and the next slots. "p_nxt" points to
    the next polling slot, INCLUDING the neighbour slot, whereas
    p_nxt_but_one points to the next free slot AFTER the neighbouring slot.

*/

void            bal_create          (struct  BAM_STR_CFG_PT * p_pt)

{
struct  BAL_STR_SI_NXT *    p_si;
struct  BAL_STR_SI_NXT *    p_si_lst;      /* initialised with lst - 1,
                                              corrected to last later ! */
struct BAL_STR_INIT_SLOT    iss;
unsigned short              c_sl;
unsigned short              c_tmr1;
unsigned short              c_slot = p_pt->p_div->c_slot;
unsigned short              b_cycl = p_pt->p_div->basic_cycle;

int                         i;

    bai_memset (&iss, 0, sizeof (struct BAL_STR_INIT_SLOT));

    bal_init_init_slot (&iss, p_pt, p_pt->p_div->fi_slot,
                                    p_pt->p_div->fx_slot);

    p_si_lst = &bal_dta.slot[c_slot-2];
    c_tmr1 = BAH_TMR_FACTOR * b_cycl / BAH_TMR_DIVISOR;

    bal_dta.slot[(c_slot>>1)-1].dma.activate_bad_task = BAA_ACTIVE;
    bal_dta.slot[ c_slot    -1].dma.activate_bad_task = BAA_ACTIVE;

    bal_dta.t_max = b_cycl;
    bal_dta.cfgd.c_tick        = p_pt->p_div->sys_c_tick;
    bal_dta.cfgd.c_tmr1        = c_tmr1;
    bal_dta.cfgd.p_lst_but_one = p_si_lst++;
    bal_dta.cfgd.p_lst         = p_si_lst;

    i    = 0;

    p_si = &bal_dta.slot[   0];

    while   (i < c_slot)
    {
     unsigned short c_mf;
        c_sl = iss.sl_cnt * iss.fi_slot;
        bal_init_slot (&iss, &p_si->dma.c_mf, &p_si->dma.t_us);
        c_mf = p_si->dma.c_mf & 0x1F;
                                        #if defined (O_BA_RTS)
        p_si->dma.p_srce     = bah_dma_p_srce        (iss.p_slot_0, c_sl);
                                        #else
        p_si->dma.a_srce     = bah_p_2_addr_cfgd_dma (iss.p_slot_0, c_sl);
                                        #endif
        p_si->dma.w_ctrl     = BAH_DMA_CW_POLL_OFF;
        p_si->poll.p_mf      = &(iss.p_slot_0 + c_sl)->sm[c_mf-1];

        p_si_lst->dma.p_slt_nxt = p_si;
        p_si_lst->poll.p_nxt    = p_si;
        p_si_lst                = p_si;
        p_si++;

        i++;
    }
    bal_dta.cfgd.p_lst_but_one->poll.p_nxt    = &bal_dta.slot[0];
    /* marker for bal_reset */
    bal_dta.cfgd.p_lst->poll.p_nxt->dma.w_ctrl = BAH_DMA_CW_POLL_ON;

    bal_dta.ra.c_reference = BAM_RA_T_WAIT_CFGD + bat_rank ();
}

/*      modifications

        The last slot has been protected from participating in the
        event arbitration by redirecting the pointer n-1 to slot 0

*/


short           bal_reset            (void)

{
struct BAL_STR_SI_NXT * p_si_nxt;
unsigned int i = 0;
short  rslt = RET_OK;

    bal_dta.cfgd.p_lst_but_one->poll.p_nxt = &bal_dta.slot[0];
    bal_dta.cfgd.p_lst->dma.p_slt_nxt      = &bal_dta.slot[0];
    *bal_dta.cfgd.p_lst->poll.p_mf         = bal_dta.mf_padding;

    p_si_nxt = bal_dta.cfgd.p_lst_but_one;

    while ((p_si_nxt->dma.w_ctrl != BAH_DMA_CW_POLL_ON) && (i++ < 1024))
    {
        *p_si_nxt->poll.p_mf = bal_dta.mf_padding;
        p_si_nxt = p_si_nxt->dma.p_slt_nxt;
    }
    p_si_nxt->dma.w_ctrl = BAH_DMA_CW_POLL_OFF;
    *bal_dta.slot[0].poll.p_mf = bal_dta.q_sf.n_pf;
    bal_dta.slot[0].dma.w_ctrl = BAH_DMA_CW_POLL_ON;


    return (rslt);
}



/*  new page
 ------------------------------------------------------------------------------
|
|   name        bal_check_bus_silence
|
 ------------   A B S T R A C T   ---------------------------------------------

    read atrl.at to determine activity on bus, and reset these bits

*/
#if defined (O_BAP)

short       bal_bap_check_bus_silence   (void)

{
unsigned char   atrl;
unsigned char * p_atrl = &atrl;

    bah_bac_atrl_in (p_atrl);
    bah_bac_at_reset();

    return ((*p_atrl & BAH_BAC_R40_AT_MASK) == 0 ? RET_OK : BA_E );
}
#else

short       bal_mvbc_check_bus_silence   (void)

{
unsigned short  c_frame;

    bah_mvbc_fc_in (&c_frame);
    if (c_frame != 0)
    {
        bah_mvbc_fc_reset ();
    }
    return (c_frame == 0 ? RET_OK : BA_E );
}
#endif

/*  new page
 ------------------------------------------------------------------------------
|
|   name        bal_amfx_panic
|
 ------------   A B S T R A C T   ---------------------------------------------

    The control word copy which caused the activation of this interrupt
    is resetted.

    A check is made to be sure that the cycle was not overshot by a bad
    interrupt latency. If the slot has been missed, the last poll MF is
    read from the old q data and retransmitted.

*/
#if !defined (O_BAP)

void bal_amfx_panic (void)
{
    struct BAL_STR_SI_NXT * p_si_dma;
//    unsigned short activate_bad_task = BAA_PASSIVE;
    unsigned short activate_bad_task;

    c_amfx_panic++;
    p_si_dma  = bal_dta.p_si_dma->dma.p_slt_nxt;

/* <ATR:01> */
//  bal_mcu_deadlock ();

#if defined (O_BA_RTS)
    bal_dta.p_rts = bal_dta.p_rts->p_nxt_rts;
    pi_copy16 (bal_dta.p_rts->p_dma_buffer,
               p_si_dma->dma.p_srce,
               p_si_dma->dma.c_mf & 0x1F);

    bah_panic_w_a_srce (bal_dta.p_rts->a_srce);
    bah_panic_w_c_mf   (p_si_dma->dma.c_mf);
#else
    bah_panic_w_a_srce (p_si_dma->dma.a_srce);
    bah_panic_w_c_mf   (p_si_dma->dma.c_mf);
#endif
    *bal_dta.p_si_poll->poll.p_mf = bal_dta.mf_padding;
    activate_bad_task = bal_dta.p_si_poll->dma.activate_bad_task;

    bal_dta.p_si_poll = bal_dta.p_si_dma;
    bal_dta.p_si_dma  = bal_dta.p_si_dma->dma.p_slt_nxt;

    p_si_dma  = bal_dta.p_si_dma->dma.p_slt_nxt;

#if defined (O_BA_RTS)
    bal_dta.p_rts = bal_dta.p_rts->p_nxt_rts;
    pi_copy16 (bal_dta.p_rts->p_dma_buffer,
               p_si_dma->dma.p_srce,
               p_si_dma->dma.c_mf & 0x1F);
    bal_dta.p_rts_mf = bal_dta.p_rts->p_dma_buffer +
                       ((p_si_dma->dma.c_mf & 0x1F) - 1);

    bah_dma_w_a_srce (bal_dta.p_rts->a_srce);
#else
    bah_dma_w_a_srce (p_si_dma->dma.a_srce);
    bah_dma_w_c_mf   (p_si_dma->dma.c_mf);
#endif
    activate_bad_task |= bal_dta.p_si_poll->dma.activate_bad_task;
    *bal_dta.p_si_poll->poll.p_mf = bal_dta.mf_padding;

    bal_dta.p_si_poll = bal_dta.p_si_dma;
    bal_dta.p_si_dma  = bal_dta.p_si_dma->dma.p_slt_nxt;

    bah_tmr2_wdog_trigger ();
    if  (activate_bad_task == BAA_ACTIVE)
    {
        bam_task_rdy ();
    }
}


/*  new page
 ----------------------------------------------------------------------------
|
|   name        bal_mvbc_amfx
|
 ------------   A B S T R A C T   -------------------------------------------

    mvbc interrupt handler call after end of bus administrator mf table
    "all master frames are transmitted" is called by interrupt upon AMFX

    It first checks whether the erroneous master frame count exceeds
    the tolerance (this would mean another bus administrator is sharing the
    bus). If this is the case, it calls the state transition to
    redundant bus administrator. The tolerance is rank dependant so hopefully
    only one BA will transit to redundant.
    Then it calls "nxt_dma()" to announce the next but one list (the next
    is already loaded and is activated upon the next timer 1 overflow)
    to the MR2 register
    It then checks the panic mode which means, is the next list really loaded,
    because if it isn't then the bus will stop because no amfx interrupt
    will load the list from MR2 to MR. In this case, the panic software is
    called which loads manually the MR reg from MR2 and loads a further list
    into MR2.
    It then checks whether there is an outstanding poll frame in the slot.
    If this is the case, then the bal_dma_empty routine is called to collect
    the response and evaluate the poll state.
    At the end, the tmr1_bm_cfgd() call adjusts the internal pointers
    bal_dta.p_si_dma and bal_dta.p_si_poll to take account for the next
    slot.
    The timer 2 watchdog is retriggered
*/


void            bal_mvbc_amfx       (unsigned short not_used)
{
  not_used = not_used;

  if (bal_dta.c_emf > bal_dta.c_emf_ref)
  {
  		emf_panic_count++;	/* <ATR:02> */
        bal_dta.p_si_dma  = &bal_dta.nivana_slot;
        bal_dta.p_si_poll = &bal_dta.nivana_slot;
        bam_st_trans_di_ext (BAM_RA);
  }
  else
  {
    if (bal_amfx_nxt_dma () == RET_OK)
    {
      if (bah_check_4_panic())
      {
        if  (bal_dta.state == BAM_BM)
        {
            bal_amfx_panic ();
            baa_clr (&bal_dta.p_si_poll->dma.w_ctrl, BAH_DMA_CW_POLL);
        }
      }
      else
      {
        if ((bal_dta.p_si_poll->dma.w_ctrl & BAH_DMA_CW_POLL_ON )
             == BAH_DMA_CW_POLL_ON)
        {
             bal_dma_empty ();
             baa_clr (&bal_dta.p_si_poll->dma.w_ctrl, BAH_DMA_CW_POLL);
        }
        bal_tmr1_bm_cfgd      ();
        bah_tmr2_wdog_trigger ();
      }
    }
  }
  bal_blink_bm ();
}


/*  new page
 ----------------------------------------------------------------------------
|
|   name        bal_mvbc_emf      interrupt erroneous master frame
|
 ------------   A B S T R A C T   -------------------------------------------

    This is the mechanism to detect a foreign bus master.

    The count of erroneous master frames is incremented. It is not evaluated
    because a state transition from here might leave an open AMFX interrupt
    hanging.

    It is cleared in the background by the agent for the 512 ms task.


*/

void            bal_task            (void)
{
    bal_dta.c_emf = 0;
    if (c_amfx_panic != c_amfx_panic_old)
    {
      /* printf ("count amfx panic %d ", c_amfx_panic ); */
      c_amfx_panic_old = c_amfx_panic;
    }
}

void            bal_mvbc_emf        (unsigned short not_used)
{
    not_used = not_used;

    vch_func_call (logMsg ("EMF - foreign BM active",0,0,0,0,0,0));

    if (bal_dta.state == BAM_BM)
    {
  		emf_bm_count++;		/* <ATR:02> */
        bal_dta.c_emf++;
    }
    else
    {
  		emf_slv_count++;	/* <ATR:02> */
        bal_dta.c_emf = 0;
    }
}

/*  new page
 ----------------------------------------------------------------------------
|
|   name        bal_mvbc_bti      interrupt bus timeout 1.3 ms
|
 ------------   A B S T R A C T   -------------------------------------------

    This function is called by the interrupt BTI

    When this function is called, there is acute danger of standard
    violation "bus activity at least every 1.4 ms"

    !!! workaround because of unreliable timer interrupt in bus master mode

    If the bus administrator is the bus master, then there might have
    been a ready hangup (deadlock) which must be reset by transitting
    the MVBC to init level and then back to running.
    (The real mechanism to treat this case is the tmr2_watchdog).

    !!! workaround because of unreliable timer interrupt in redundant mode
    If it is redundant, then it sends itself the mastership transfer
    telegram which will then cause this device to become bus master.

*/

void            bal_mvbc_bti        (unsigned short not_used)
{
    not_used = not_used;

    if (bal_dta.state == BAM_BM)
    {
       vch_func_call (logMsg ("BTI - watchdog barked",0,0,0,0,0,0));

       if (bah_check_4_mcu_deadlock ())
       {
  		   bti_deadlock_count++;	/* <ATR:02> */
/* <ATR:01> */
//         bal_mcu_deadlock ();
       }
       else
       {
  		   bti_bm_count++;			/* <ATR:02> */
           bal_amfx_panic ();
       }
    }
    else
    {
		bti_slv_count++;			/* <ATR:02> */
        vch_func_call (logMsg ("BTI - in redundant Mode",0,0,0,0,0,0));
    }
}

/*  new page
 ----------------------------------------------------------------------------
|
|   name        bal_mvbc_fev      error frame counter
|
 ------------   A B S T R A C T   -------------------------------------------

    The error frame counter is read and the error frame count mechanism
    in the MVBC is resetted.
    The error frame count is passed to the diagnostic module for
    further processing.
    This function is called upon the interrupt FEV.

*/

void bal_mvbc_fev   (unsigned short not_used)
{
	unsigned short error_frame_counter;

	fev_count++;			/* <ATR:02> */

    not_used = not_used;

    vch_func_call (logMsg ("FEV - frame counter",0,0,0,0,0,0));

    bah_c_error_frame_read (&error_frame_counter);
    bad_c_error_frame_put (error_frame_counter);

    bah_c_error_frame_reset();
    bah_c_total_frame_reset();

}

/*  new page
 ----------------------------------------------------------------------------
|
|   name        bal_mvbc_mstr_tsfr
|
 ------------   A B S T R A C T   -------------------------------------------

    mvbc interrupt handler call to transfer the bus mastership from and
    to this device (state dependant)
    This function is called by interrupt upon DTI6
*/

void    bal_mvbc_mstr_tsfr (unsigned short func_param)
{
        if ((bal_dta.state & BAM_BM) == BAM_BM )
        {
			dti6_bm_count++;			/* <ATR:02> */
            vch_func_call (logMsg ("DTI6 - master off",0,0,0,0,0,0));
            bat_pass_on_mastership  (func_param);
        }
        else
        {
			dti6_slv_count++;			/* <ATR:02> */
            vch_func_call (logMsg ("DTI6 - master on",0,0,0,0,0,0));
            bat_accept_mastership  (func_param);
        }
}


/*  new page
 ----------------------------------------------------------------------------
|
|   name        bal_mvbc_tmr2
|
 ------------   A B S T R A C T   -------------------------------------------

    mvbc interrupt handler call in bus master mode only

    The timer 2 is the internal watchdog. In bus master mode it is retriggered
    by the AMFX interrupt before it runs out. It should never generate an
    interrupt.
    If it does generate an interrupt, then the bus has stopped. First it
    checks whether the MR registers are empty. If this is the case, then
    the MR registers are refilled.
    If the MR registers are not empty, then there is a BUSY hangup check

*/

void    bal_mvbc_tmr2        (unsigned short not_used)
{
    not_used = not_used;
    bal_dbug_tmr1_ra_called ();

    if (bal_dta.state == BAM_RA)
    {
        bal_blink_ra ();
        if  (bal_check_bus_silence () == RET_OK)     /* Attention, no update! */
        {
			tmr2_ra_silence_count++;		/* <ATR:02> */
            if  ((bal_dta.ra.c_ountdown--) == 0)
            {
                bam_st_trans_di_ext (BAM_BM);
            }
        }
        else
        {
			tmr2_ra_noise_count++;		/* <ATR:02> */
            bal_dta.ra.c_ountdown = bal_dta.ra.c_reference;
        }
    }
    else
    {
        vch_func_call (logMsg ("timer 2 watchdog barked",0,0,0,0,0,0));
        if (bah_check_4_mcu_deadlock ())
        {
			tmr2_deadlock_count++;		/* <ATR:02> */
            bal_mcu_deadlock ();
        }
        else
        {
			tmr2_panic_count++;			/* <ATR:02> */
            bal_amfx_panic ();
        }
    }
}


/*  new page
 ----------------------------------------------------------------------------
|
|   name        bal_mcu_deadlock
|
 ------------   A B S T R A C T   -------------------------------------------

    The busy bit hangs up sometimes, stopping the mf transmission mechanism
    By setting the initialisation level to "init" and then back to "run",
    this hangup can be resetted.

*/

void bal_mcu_deadlock               (void)
{
    bah_ireg_init_level (TM_SCR_IL_VAL_CFG );
   	baa_delay (1);
   	bah_ireg_init_level (TM_SCR_IL_VAL_RUN );
}

#endif

/*  new page
 ----------------------------------------------------------------------------
|
|   name        bal_blink_bm
|
 ------------   A B S T R A C T   -------------------------------------------

     This function supports only the EKR hardware.
     Replace the LED drivers to tailor for your target.

     When the BA is bus master, the BM LED will blink with the
     active/passive ratio 15:1

*/


void    bal_blink_bm    (void)
{
        if ((bal_dta.dbug.c_tmr1++ & 0xF) == 0)
        {
            if ((bal_dta.dbug.c_tmr1 & 0xF0) == 0)
            {
                bah_ekr_func (bai_hi_led_bm_act_int_ext_di ());
            }
            else if ((bal_dta.dbug.c_tmr1 & 0xF0) == 0xF0)
            {
                bah_ekr_func (bai_hi_led_bm_pas_int_ext_di ());
            }
        }
}

/*  new page
 ----------------------------------------------------------------------------
|
|   name        bal_blink_ra
|
 ------------   A B S T R A C T   -------------------------------------------

     This function supports only the EKR hardware.
     Replace the LED drivers to tailor for your target.

     When the BA is in redundant mode, the BM LED will blink with the
     active/passive ratio 1 : 15

*/

void    bal_blink_ra    (void)
{
        if ((bal_dta.dbug.c_tmr1++ & 0x7) == 0)
        {
            if ((bal_dta.dbug.c_tmr1 & 0x3F8) == 0)
            {
                bah_ekr_func (bai_hi_led_bm_pas_int_ext_di ());
            }
            else if ((bal_dta.dbug.c_tmr1 & 0x3F8) == 0x3F8)
            {
                bah_ekr_func (bai_hi_led_bm_act_int_ext_di ());
            }
        }
}


void            bal_dma_empty       (void)

{
  struct BAL_STR_Q * p_q_sf = &bal_dta.q_sf;
  signed short       result = RET_OK;

    baa_spy_start (BAA_SPY_ID_BLL_PL, bal_dta);

    bal_dta.poll_common.pack = BAL_PL_PACK_RDY;

    if (p_q_sf->n_pf != BAP_ETL_MF_CONT_SLNC)
    {
#if defined (O_BAP)
        bah_pl_rcve_sf  (&p_q_sf->rsp);
        result = bah_check_slot ();
        if  (bal_dta.poll_common.w_ctrl != BAH_DMA_CW_POLL_ON)
        {
            result = BA_E;      /* wrong slot error */
        }
#else
        result = bah_pl_rcve_sf  (&p_q_sf->rsp);
#endif
        /* if result is erroneous, do nothing, recovery by cyclic handler */
        if (result == RET_OK)
        {
#if defined (O_BAP)
            bal_blink_bm ();
#endif
        }
        else
        {
            bal_dta.poll_common.pack = BAL_PL_PACK_ERROR;
            return;
        }
    }
    else
    {
        bap_etl_cont (p_q_sf);

        result = bah_check_t_guard ();
#if defined (O_BAP)
        if  (bal_dta.poll_common.w_ctrl != BAH_DMA_CW_POLL_ON)
        {
            result = BA_E;      /* wrong slot error */
        }
#endif
        if  (result != RET_OK)
        {
            bal_dta.poll_common.pack = BAL_PL_PACK_ERROR;
            return;
        }
        else
        {
            result = bal_pl_s_mf ();
        }
    }
/*  !!! Attention, poll mechanism to two per slot reduced */

    {
    unsigned int i = 0;
        while ((result == RET_OK) && (i++ < 2))
        {
            if (bap_poll (p_q_sf) == RET_OK)
            {
                result = bal_pl_s_mf ();
            }
            else
            {
                result = BA_E;
                break;
            }
        }
    }

    bal_pl_q_2_bus  ( );

    baa_spy_click( );
}


/*  new page
 ----------------------------------------------------------------------------
|
|   name        bal_tmr1_ra         timer 1 handler for redundant admin.
|
 ------------   A B S T R A C T   -------------------------------------------

    This routine is connected to the timer 1 interrupt handler. It verifies
    whether an external bus master is regularly sending the address frames.

    If the bus is dead, a countdown to take over the bus mastership is done.

    At the end of the countdown, the central control is informed.

     Only MicOS: The system timer is emulated.

*/

void            bal_dummy_0   (void)
{

}

void            bal_dummy_1   (unsigned short not_used)
{
    not_used = not_used;
}


#if defined (O_BAP)

short           bal_tmr1_ra     (void)

{
short           rslt = RET_OK;

    bal_blink_ra ();
    bal_dbug_tmr1_ra_called ();

    if  (bal_check_bus_silence () == RET_OK)     /* Attention, no update! */
    {
        if  ((bal_dta.ra.c_ountdown--) == 0)
        {
            bam_st_trans_di_ext (BAM_BM);
        }
    }
    else
    {
        bal_dta.ra.c_ountdown = bal_dta.ra.c_reference;
    }

    {
        if  ((bal_dta.cfgd.c_64ms += bah_c_64ms(BAH_TMR_1300US))
                                   > BAH_CPU_TMR1_64MS)
        {
            bai_si_trigger_watchdog ();
            bal_dta.cfgd.c_64ms = 0;
            rslt = 2;               /* !!! evaluated by ASM frame */
        }
    }

    return (rslt);
}

#endif



/*  new page
 ----------------------------------------------------------------------------
|
|   name        bal_tmr1_bm_cfgd    timer 1 handler for bus master - cfgd
|
 ------------   A B S T R A C T   -------------------------------------------

    This routine is connected to the timer 1 interrupt handler. It is
    called each time a new DMA cycle commences and the primary duty is to
    attach the next cyclic slot list to the redundant DMA channel before
    the next DMA cycle commences.

    The bal_dta_si_nxt_dma points to the dma channel used next:
    even for channel 0, odd for channel 1. It is initialised with zero and
    starts with channel 1 (due to ++ command).

    The next slot in the slot segment is attached to the DMA controller. The
    count is written to the transfer count register, the offset to the offset
    register and the control word register is updated.

    The pointer is set to the next slot, which is generally the next 1 ms
    slot, but not necessarily, as a couple of 1 ms slots can be converted
    into a double, treble, ... slot

*/

short            bal_tmr1_bm_cfgd   (void)
{
unsigned short   activate_bad_task;
short            rslt = RET_OK;

   baa_spy_start(BAA_SPY_ID_BLL_CY, bal_dta);
#if defined (O_BAP)
    bal_dta.poll_common.w_ctrl   = bal_dta.p_si_dma->dma.w_ctrl;
    baa_clr (&bal_dta.p_si_dma->dma.w_ctrl, BAH_DMA_CW_POLL);
#endif
    *bal_dta.p_si_poll->poll.p_mf = bal_dta.mf_padding;
    activate_bad_task = bal_dta.p_si_poll->dma.activate_bad_task;

    bal_dta.p_si_poll = bal_dta.p_si_dma;
    bal_dta.p_si_dma  = bal_dta.p_si_dma->dma.p_slt_nxt;


                            #if defined (O_BA_RTS)
    {
     unsigned short c_mf = bal_dta.p_si_dma->dma.c_mf & 0x1F;
        bal_dta.p_rts = bal_dta.p_rts->p_nxt_rts;
        pi_copy16 (bal_dta.p_rts->p_dma_buffer,
                   bal_dta.p_si_dma->dma.p_srce, c_mf);
        bal_dta.p_rts_mf = bal_dta.p_rts->p_dma_buffer + (c_mf - 1);

    }
                            #endif


    bal_dbug_tmr1_bm_cfgd_called ();

    if (bal_dta.poll_common.pack++ >= BAL_PL_PACK_ERROR)
    {
        bal_dta.poll_common.pack = BAL_PL_PACK_RDY;
        baa_set (&bal_dta.p_si_dma->poll.p_nxt->dma.w_ctrl, BAH_DMA_CW_POLL);
        *bal_dta.p_si_dma->poll.p_nxt->poll.p_mf = bal_dta.q_sf.n_pf;
    }

                                       #if defined (O_BAP)
    {
    char act_dma;
    char * p_temp = &act_dma;
        bah_bac_atrl_dma (p_temp);
        if  (act_dma == 1)
        {
            bah_cpu_dma0_w_a_srce (bal_dta.p_si_dma->dma.a_srce);
            /*  bah_dma_nxt_count */
            bah_cpu_dma0_w_c_mf   (bal_dta.p_si_dma->dma.c_mf);
            /*  bah_dma_nxt_cw    */
            bah_cpu_dma0_w_cw     (bal_dta.p_si_dma->dma.w_ctrl);
        }
        else
        {
            bah_dma_w_a_srce (bal_dta.p_si_dma->dma.a_srce);
            bah_dma_w_c_mf   (bal_dta.p_si_dma->dma.c_mf);
            bah_dma_w_cw     (bal_dta.p_si_dma->dma.w_ctrl);
        }
    }
                                       #endif

    if  (activate_bad_task == BAA_ACTIVE)
    {
        bam_task_rdy ();
        rslt = 1;                   /* evaluated by ASM frame */
    }
                                    #if defined (O_EKR) || defined (BX_SIMU)
    else
    {
        if  ((bal_dta.sys_tmr.c_64ms += bal_dta.sys_tmr.c_tick)
                                     >= BAL_SYS_TMR_64MS_TICKS)
        {
            if     ((bal_dta.poll_common.w_ctrl == BAH_DMA_CW_POLL_OFF) ||
                    (bal_dta.sys_tmr.c_64ms >= BAL_SYS_TMR_80MS_TICKS))
            {
                    bai_si_trigger_watchdog ();
                    bal_dta.sys_tmr.c_64ms  -= BAL_SYS_TMR_64MS_TICKS;
                    rslt = 2;       /* evaluated by ASM frame */
            }
        }
    }
                                    #endif
    baa_spy_click();

    return (rslt);
}

#if !defined (O_BAP)
/*  new page
 ----------------------------------------------------------------------------
|
|   name        bal_amfx_nxt
|
 ------------   A B S T R A C T   -------------------------------------------

    The next DMA must be announced immediately, because the MVBC latches the
    next transmission request upon the AMFX interrupt, which might not leave
    the software much time under adverse conditions.


*/

int bal_amfx_nxt_dma (void)
{
    struct BAL_STR_SI_NXT * p_si_dma = bal_dta.p_si_dma->dma.p_slt_nxt;
    unsigned short c_mf = p_si_dma->dma.c_mf;

    if (p_si_dma != &bal_dta.nivana_slot)  /* !!! debug */
    {

        if ((c_mf != 0xC0) && (bal_dta.state == BAM_BM))
        {
#if defined (O_BA_RTS)
            bah_dma_w_a_srce (bal_dta.p_rts->p_nxt_rts->a_srce);
#else
            bah_dma_w_a_srce (p_si_dma->dma.a_srce);
#endif
            bah_dma_w_c_mf   (p_si_dma->dma.c_mf);

            return (RET_OK);
        }
        else
        {
            return (BA_E);
        }
    }
    else /* !!! debug */
    {
        c_mf = 20;
        c_mf = c_mf;
        return (BA_E);
    }
}

#endif

/*  new page
 ----------------------------------------------------------------------------
|
|   name        bal_st_2_ra         state transition to redundant admin
|
 ------------   A B S T R A C T   -------------------------------------------

    Performs the actions associated to a state transition from offline to
    redundant bus administrator.

    The timer 1 is stopped. The reference count is adjusted to 1.3 ms to
    support the life supervision. The timer 1 count is set to 1.298 ms to
    generate an interrupt at once. The function to attach to the timer 1
    interrupt handler is attached.

    The BAC is set in state NOT BUS MASTER, maximum bus length.

    The timer 1 is started and will generate an interrupt every 1.3 ms.

*/

void            bal_st_2_ra    (void)
{

    bah_bap_func (bah_bm_disable() );

    bah_tmr_w_cw (BAH_TMRS_STOP);

    bal_dta.ra.c_ountdown  = bal_dta.ra.c_reference;

    bal_dta.state = BAM_RA;

/*
    bai_mvbc_lc_int_disconnect (LC_INT_EMF );  */

    bah_bap_func (bai_lc_int_disconnect (BAI_LC_INT_BMT));

    bah_bap_func (bah_bap_tmr_init (BAH_TMR_1300US, BAH_TMR_GUARD));

    bah_bap_func (bai_lc_int_connect    (bat_accept_mastership, BAI_LC_INT_BMT,
						    					  LC_KERNEL, 0));

    bai_vec_bll_tmr1 (baf_bll_tmr1_ra);
    bai_vec_bll_dma  (baf_bll_dma_empty_dummy);

    bah_mvbc_func (bah_tmr2_w_c_max (BAH_TMR2_1300US));
    bah_mvbc_func (bah_tmr2_w_c     (BAH_TMR2_1300US));
    bah_mvbc_func (bah_tmr_w_cw  (BAH_TMR1_STOP_TMR2_GO));
    bah_bap_func  (bah_tmr_w_cw  (BAH_TMRS_GO));
}


/*  new page
 ----------------------------------------------------------------------------
|
|   name        bal_st_2_bm     state transition to bus master
|
 ------------   A B S T R A C T   -------------------------------------------

    Performs the actions associated to a state transition from redundant BA
    to bus master (both configured and unconfigured, depends upon param)

    The bal_dta_si_nxt_dma points to the dma channel used next:
    even for channel 0, odd for channel 1. It is initialised with 1 to
    start with channel 1. The next BAC access is forced to channel 1. This
    is empty, so nothing will happen. Upon timer interrupt, the DMA channel
    will toggle to DMA 0, the first channel to transmit data.

    The first slot in the slot segment is attached to the DMA controller.
    The offset is written to the offset register. The control word is
    written to the control word register. The count is written to the
    transfer count register.

    Timer 1 ref value is updated last. The DMA transfer mechanism is
    activated by writing the control word. The transmission will commence
    upon timer 1 reaching the timer 1 reference value.

*/

void            bal_st_2_bm    (unsigned short  cfg_state)
{

unsigned short c_tmr_start;
    bah_tmr_w_cw (BAH_TMRS_STOP);
#if defined (O_BAP)
    {
    unsigned short  electrical_len;

        baa_ebl_get (&electrical_len);
        bah_bm_enable (electrical_len);
        bah_send_mf   (bal_dta.mf_padding);
    }
#else
    {
      unsigned short temp;
      temp = bah_dta.ts_map.pb_ireg->dr.w;
      vch_func_call (logMsg ("trans to BM - decoder reg %x \n",temp,0,0,0,0,0));
    }
#endif

    bah_mvbc_func (bah_ireg_init_level (TM_SCR_IL_VAL_CFG ));

    bah_dma_w_cw      (BAH_DMA_OFF);
    bah_cpu_dma0_w_cw (BAH_DMA_OFF);

    bah_dma_w_a_dest (BAH_LOC_SATN);

    bah_bac_dma1_set      ();                 /* force next access to DMA 1 */
    bai_vec_bll_tmr1 (baf_bll_tmr1_bm_cfgd);

    bah_cpu_eoi_dma ();

    bai_vec_bll_dma  (baf_bll_dma_empty);

    bal_dta.sys_tmr.c_64ms = BAL_SYS_TMR_32MS_TICKS; /* synchronise */

    bal_dta.state = BAM_BM;

    bal_dta.c_emf     = 0;

    if  (cfg_state == BAM_CFGD)
    {
        bal_dta.c_emf_ref = 4 * bat_rank ();
        bal_dta.sys_tmr.c_tick = bal_dta.cfgd.c_tick;

        c_tmr_start = bal_dta.cfgd.c_tmr1;

        bal_dta.p_si_dma  = &bal_dta.slot[0];
        bal_dta.p_si_poll = bal_dta.cfgd.p_lst;
                                        #if defined (O_BA_RTS)
        bal_dta.p_rts = &bal_dta.rts_cfgd[2];
                                        #endif
    }
    else
    {
        bal_dta.c_emf_ref = 4;

        bal_dta.sys_tmr.c_tick = BAL_SYS_TMR_NCFG_TICK;
        c_tmr_start = bal_dta.ncfg.c_tmr1;

        bal_dta.p_si_dma  = &bal_dta.ncfg.cb[0];
        bal_dta.p_si_poll = bal_dta.ncfg.p_lst;
                                        #if defined (O_BA_RTS)
        bal_dta.p_rts = &bal_dta.rts_ncfg[2];
                                        #endif
    }
    bah_tmr_init  (c_tmr_start, BAH_TMR_GUARD);

    *bal_dta.p_si_dma->poll.p_mf              = bal_dta.q_sf.n_pf;
    bal_dta.p_si_poll->poll.p_nxt->dma.w_ctrl = BAH_DMA_CW_POLL_ON;

#if defined (BX_SIMU)
    bsp_send_mf_no_print (bal_dta.q_sf.n_pf);
#endif

    bal_dta.p_si_poll = &bal_dta.nivana_slot;   /* disable bad activate */

#if !defined (O_BAP)

                                        #if defined (O_BA_RTS)
    pi_copy16 (bal_dta.p_rts->p_dma_buffer,
               bal_dta.p_si_dma->dma.p_srce,
               bal_dta.p_si_dma->dma.c_mf & 0x1F);

    bah_dma_kick_start_w_a_srce (bal_dta.p_rts->a_srce);
                                        #else
    bah_dma_kick_start_w_a_srce (bal_dta.p_si_dma->dma.a_srce);
                                        #endif

    bah_dma_kick_start_w_c_mf   ((bal_dta.p_si_dma->dma.c_mf & 0x1F) | 0x80);

    bal_dta.p_si_poll = bal_dta.p_si_dma;
    bal_dta.p_si_dma  = bal_dta.p_si_dma->dma.p_slt_nxt;

#endif
                                        #if defined (O_BA_RTS)
    bal_dta.p_rts = bal_dta.p_rts->p_nxt_rts;
    bah_dma_w_a_srce (bal_dta.p_rts->a_srce);
    bah_dma_w_c_mf   (bal_dta.p_si_dma->dma.c_mf);

    pi_copy16 (bal_dta.p_rts->p_dma_buffer,
               bal_dta.p_si_dma->dma.p_srce,
               bal_dta.p_si_dma->dma.c_mf & 0x1F);

    bal_dta.p_rts_mf = bal_dta.p_rts->p_dma_buffer +
                       ((bal_dta.p_si_dma->dma.c_mf & 0x1F) - 1);

                                        #else
    bah_dma_w_a_srce (bal_dta.p_si_dma->dma.a_srce);
    bah_dma_w_c_mf   (bal_dta.p_si_dma->dma.c_mf);
                                        #endif

    bah_bap_func (bah_dma_w_cw (bal_dta.p_si_dma->dma.w_ctrl));

                                        #if defined (O_BAP)
    if (bal_dta.has_been_bm_already == BAA_PASSIVE)
    {
        unsigned short tmp_tmr_start = c_tmr_start - BAH_TMR_GUARD;
        bal_dta.has_been_bm_already = BAA_ACTIVE;
        bah_tmr_w_c (tmp_tmr_start);
    }
    else
    {
        bal_dta.p_si_dma  = bal_dta.p_si_dma->dma.p_slt_nxt;

        bah_cpu_dma0_w_a_srce (bal_dta.p_si_dma->dma.a_srce);
        bah_cpu_dma0_w_c_mf   (bal_dta.p_si_dma->dma.c_mf);
        bah_cpu_dma0_w_cw     (bal_dta.p_si_dma->dma.w_ctrl);
    }

                                        #endif

    bah_mvbc_func (bah_ireg_init_level (TM_SCR_IL_VAL_RUN ));

    bah_tmr_w_cw     (BAH_TMRS_GO);

}


/*  new page
 ----------------------------------------------------------------------------
|
|   name        bal_sl_pad
|
 ------------   A B S T R A C T   -------------------------------------------

    Pads the slot to have an initial value of at least 3 telegrams
    to give the event arbitration time to write the event to the DMA list

    Pads the slot list to guarantee automatic transmission of life sign.

    Adds one padding to end of DMA list, which is replaced online by the
    event arbitration

*/


void        bal_sl_pad             (unsigned *            c_pad,
                                    unsigned short *      p_dest,
                                    unsigned              c_mf,
                                    unsigned              t_us)

{
unsigned short   cnt, time, t_fcode_1;

    cnt      = 0;

    if (t_us < bal_dta.t_max)
    {
        if ((time = bal_dta.t_max - t_us) > 1200)
        {
            t_fcode_1 = baa_pd_in_us (0);
            *p_dest++ = bal_dta.mf_padding;
            cnt++;
            while ((time -= t_fcode_1) > 1200)
            {
                *p_dest++ = bal_dta.mf_padding;
                cnt++;
            }
        }
        else
        {
            while ((cnt + c_mf) < 3)
            {
                *p_dest++ = bal_dta.mf_padding;
                cnt++;
             }
        }
    }
    else
    {
        bal_dummy_0 ();
    }
    *c_pad = cnt;
}

/*  new page
 ----------------------------------------------------------------------------
|
|   name        bal_q_2_bus
|
 ------------   A B S T R A C T   -------------------------------------------

    The data in the queue is identified by "c_sf". These data are
    copied to free spaces in the DMA lists, ignoring the next DMA list as
    there might not be enough time.
    The count "c_sf" is reset to zero (empty queue).
*/



void        bal_pl_activate     (void)
{
}

void        bal_pl_statistic    (unsigned long *           p_c_event_total,
                                 struct BA_STR_POLL_STAT * p_stat)
{
    *p_c_event_total = bal_dta.pl_stat.c_total;
    *p_stat          = bal_dta.pl_stat.distr;
}


void        bal_pl_q_2_bus      ()
{

                                        #if defined (O_BA_RTS)
unsigned short *            p_mf_tmp;
                                        #endif

struct BAL_STR_SI_NXT *     p_nxt;
struct BAL_STR_Q *          p_q_sf = &bal_dta.q_sf;
unsigned short *            p_mf;
unsigned short              c_sf;


#if !defined (O_BAP)
    if (bah_check_window () != RET_OK)
    {
        p_nxt = bal_dta.p_si_dma->poll.p_nxt;
    }
    else
#endif
    {
        p_nxt = bal_dta.p_si_poll->poll.p_nxt;
    }

    if  (p_q_sf->rsp.state == BA_RSP_OK)
    {
        bal_pl_q_event  ();
    }


    c_sf = p_q_sf->c_sf;

#if defined (BX_SIMU)

    bsp_q_2_bus (p_q_sf);

#endif

                                        #if defined (O_BA_RTS)
    p_mf_tmp = p_nxt->poll.p_mf;
                                        #endif
    if (c_sf != 0)
    {
        bal_dta.pl_stat.c_total += c_sf;
        p_mf = &p_q_sf->a_sf[0];

        while (c_sf-- != 0)
        {
        unsigned short mf;
            mf = *(p_mf++);
            *p_nxt->poll.p_mf = mf;
            p_nxt = p_nxt->poll.p_nxt;
            bal_dta.pl_stat.distr.c_event[mf & 0x00FF]++;
        }

        p_q_sf->c_sf = 0;
    }
    else
    {
#if defined (O_BAP)
        {
            p_nxt = p_nxt->poll.p_nxt;
        }
#else   /*   MVBC
        if (p_q_sf->rsp.state == BA_RSP_MISSING)
        {
            p_nxt = p_nxt->poll.p_nxt;
        }           */
#endif
    }
     baa_set (&p_nxt->dma.w_ctrl, BAH_DMA_CW_POLL);
    *p_nxt->poll.p_mf  = p_q_sf->n_pf;

                                        #if defined (O_BA_RTS)
                                        /* don't overwrite the bm transfer */
    if (*bal_dta.p_rts_mf != bal_dta.mf_transfer)
    {
        *bal_dta.p_rts_mf = *p_mf_tmp;  /* copy next poll to DMA buffer */
    }
                                        #endif

}



/*  new page
 ------------------------------------------------------------------------------
|
|   name        bal_pl_q_event
|
 ------------   A B S T R A C T   ---------------------------------------------

    The event log book checks whether the event has already been executed
    (Dauersender detection). It logs the new event and puts it in the
    queue for transmission

    Comment: All parameters are variables and pointers (as opposed to "5"
    or &loc) to support a later tuning by in-line assembler if required.

    Check for the forbidden events  1000  (8  - mastership transfer)
                                    1001  (9  - event start)
                                    1101  (13 - event group)
                                    1110  (14 - single event)

*/

void            bal_pl_q_event      (void)

{
unsigned short  fcode = (bal_dta.q_sf.rsp.r_sf >> 12);
    switch (fcode)
    {
        case  0:
        case  1:
        case  2:
        case  3:
        case  4:
        case  5:
        case  6:
        case  7:
        case 10:
        case 11:
        case 12:
        case 15:
        {
            bal_dta.q_sf.a_sf[bal_dta.q_sf.c_sf++] = bal_dta.q_sf.rsp.r_sf;
            bal_dta.q_sf.rsp.state = BA_RSP_MISSING;
            break;
        }

        case  8:
        case  9:
        case 13:
        case 14:
        default:
        {
            break;
        }
    }
}


void        bal_pl_q_put_n_pf       (unsigned short n_pf)
{
        bal_dta.q_sf.n_pf        = n_pf;
}

unsigned short       bap_q_n_pf     (void)
{
    return (bal_dta.q_sf.n_pf);
}


/*  new page
 ----------------------------------------------------------------------------
|
|   name    bal_pl_s_mf
|
 --------   A B S T R A C T   -----------------------------------------------

    The function checks whether there is enough time to send the mf and
    receive the answer (ca. 60 us to begin next slot).

    IF YES:
    The next masterframe is sent.

    While the bus is busy, the software uses the CPU resource to process the
    last sf (if available) !!! This feature will be removed.
    !!! Probably in time conflict with the 60 us at top!

    The slave frame and the state bits are received from the bus controller.

    IF NO:
    return error to inform caller that job must not carried out.

*/

short        bal_pl_s_mf             (void)

{
  signed short       result = RET_OK;

    if  (bah_check_t_guard () == RET_OK)
    {
        bah_send_mf  (bal_dta.q_sf.n_pf);

        if  (bal_dta.q_sf.rsp.state == BA_RSP_OK)
        {
            bal_pl_q_event  ();
        }

        bah_pl_rcve_sf  (&bal_dta.q_sf.rsp);
    }
    else
    {
        result = BA_E;

        if  (bal_dta.q_sf.rsp.state == BA_RSP_OK)
        {
            bal_pl_q_event  ();
        }
    }

    return (result);
}


void        bal_prepare_m_transfer      (void)
{
    *bal_dta.cfgd.p_lst->poll.p_mf     = bal_dta.mf_transfer;
     bal_dta.cfgd.p_lst->dma.p_slt_nxt = &bal_dta.nivana_slot;
}



/* new page
 ----------------------------------------------------------------------------
|
|   name        bac_slot_time_left
|
 ------------   A B S T R A C T   -------------------------------------------

    The time remaining in a slot is calculated by extracting the fcode from
    each master frame and using the fcode as index in the propagation delay
    table.

*/

void            bal_slot_time         (unsigned *          p_t_slot,
                                       struct BAL_STR_SL * p_sl,
                                       unsigned            c_mf)
{
int i;
unsigned time = 0;

    for (i = 0; i < c_mf; i++)
    {
        time += baa_pd_in_us (p_sl->sm[i] >> 12);
    }

    *p_t_slot = time;
}




/*
 ---------------------------------------------------------------------------ƒ
|
|   M O D I F I C A T I O N S       BAL_LINK.C
|   - - - - - - - - - - - - -       ----------
|
 ----------------------------------------------------------------------------


    name of object modified     Nr: .2      dated:  9?-??-??

    description of what was done to object

 ----------------------------------------------------------------------------

*/
