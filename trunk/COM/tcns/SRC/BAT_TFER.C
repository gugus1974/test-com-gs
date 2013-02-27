/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> the BA in the DSW is correctly treated as a flag                                    */
/*==============================================================================================*/

/*   BAT_TFER.C
 ------------------------------------------------------------------------------
|
|    Property of  :   ABB Verkehrssysteme AG, CH-8050 Zrich / Switzerland
|    COPYRIGHT    :   (c) 1992 ABB Verkehrssysteme AG
|                     
 ------------------------------------------------------------------------------
|
|    Project      :   T C N   B U S   A D M I N I S T R A T O R
|
 ------------------------------------------------------------------------------
|
|    File Name    :   BAT_TFER
|
|    Document     :
|
|    Abstract     :   MVB Periodic Bus Mastership Transfer
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
|      1   BAEI-2  93-02-08  created  Marsden
|
 ------------------------------------------------------------------------------
*/
 
#define      BAT_TFER_C

#include    "baa_auxi.h"
#include    "bad_diag.h"
#include    "bah_hwre.h"
#include    "bai_ifce.h"
#include    "bal_link.h"
#include    "bam_main.h"
#include    "bat_tfer.h"
#include    "bax_incl.h"

#if         defined (BX_SIMU)

#include    "bsi_ifce.h"

#endif
 

/*  new page
 ----------------------------------------------------------------------------
|
|   static variable declarations
|
 ----------------------------------------------------------------------------


    Each BA has a "ranking" which is its position in the "Known Class 4
    Device List". This ranking determines the waiting time before bus
    mastership is automatically siezed.

    Each BA-Configuration has a version information. Bus mastership is passed
    only to equal or equal + 1 versions.

    The "known class 4 device list pointer" points to the next bus mastership
    transfer master frame. It will be modified to point to the next BA if
    the target BA refuses the bus mastership.

    The "known class 4 device list" contains all the BA's which can take the
    bus mastership in order of the passing sequence. The function "add" will
    insert new BA's at their correct place.

*/


struct      BAT_STR_DBUG
{
    unsigned short c_task;
};

struct      BAT_STR_DTA
{
            struct BAT_STR_DBUG     dbug;
            unsigned int            rank;
            unsigned int            version;
            unsigned short          mf_nxt_bm;
            unsigned short          window;
            unsigned short          c_transfer;
            unsigned short          c_red_ba;
            unsigned short          kc4_list[BAT_RANGE];
};

static      struct  BAT_STR_DTA     bat_dta;


/*  new page
 ------------------------------------------------------------------------------
|
|   prototypes
|
 --------   A B S T R A C T   ---------------------------------------------

*/

short       bat_eval_dsw_rsp (struct BA_STR_SCAN_RSLT_MBR * p_dvc);


/*  new page
 ------------------------------------------------------------------------------
|
|   name    bat_init
|
 --------   A B S T R A C T   ---------------------------------------------


    The internal variables are initialised.

*/

void        bat_init            (void)

{
struct BAM_STR_DTA * p_bam_dta;
    bai_memset (&bat_dta, 0, sizeof (bat_dta));
    bam_open (&p_bam_dta);

}


/*  new page
 ------------------------------------------------------------------------------
|
|   name        bat_create
|
|   (in )       p_ba_cfg            pointer to BA-configuration list
|
|   func        result
|
 ------------   A B S T R A C T   -------------------------------------------


    The BA rank is calculated.


*/

short           bat_create          (struct BAM_STR_CFG_PT *  p_pt)
{
struct BA_STR_CFG_KC4 * p_kc4;
unsigned short *         p_mf;
unsigned short *         p_red_ba_list;
int             i;
short           result = RET_OK;
unsigned short  my_da;
unsigned short  da;

    if  (bai_get_dvc_addr (&my_da) == RET_OK)
    {
        bat_dta.version    = p_pt->p_div->c_version_update; 

#if defined (O_BAP)
    {
    LC_TYPE_TS_PCS * p_pcs;
    LC_TYPE_TS_PRT * p_prt;
        p_pcs = bah_dta.ts_map.pb_bmt_src_pcs;
        p_prt = bah_dta.ts_map.pb_bmt_src_prt;

        lc_put_pcs      (p_pcs, LC_PCS_MSK_FCODE, LC_PCS_OFF_FCODE,
                                                  LC_PCS_VAL_FCODE_08);
        lc_put_pcs      (p_pcs, LC_PCS_MSK_TYPE,  LC_PCS_OFF_TYPE,
                                                  LC_PCS_VAL_TYPE_SRC);
        lc_put_pcs_bap  (p_pcs, LC_PCS_MSK_IEX ,  LC_PCS_OFF_IEX ,
                                                  LC_PCS_VAL_IEX_2   );

        p_prt[0][0][0][0] = 0x8000 + (bat_dta.version & 0x7FFF);
        p_prt[0][1][0][0] = 0x8000 + (bat_dta.version & 0x7FFF);
    }
#else
    {
    LC_TYPE_TS_PCS *  p_pcs;
        p_pcs = bah_mvbc_pb_svc_pp_pcs(TM_PP_FC8);

        lc_put_pcs      (p_pcs, LC_PCS_MSK_DIAG,  LC_PCS_OFF_DIAG,
                                                  LC_PCS_VAL_DIAG_RDY);
        lc_put_pcs      (p_pcs, LC_PCS_MSK_FCODE, LC_PCS_OFF_FCODE,
                                                  LC_PCS_VAL_FCODE_08);
        lc_put_pcs      (p_pcs, LC_PCS_MSK_TYPE,  LC_PCS_OFF_TYPE,
                                                  LC_PCS_VAL_TYPE_SRC);
        lc_put_pcs_mvbc (p_pcs, LC_PCS_MSK_DTI,   LC_PCS_OFF_DTI,
                                                  LC_PCS_VAL_DTI_6   );
        lc_put_pcs_mvbc (p_pcs, LC_PCS_MSK_NUM,   LC_PCS_OFF_NUM,
                                                  LC_PCS_VAL_NUM_SET );

        p_pcs = bah_mvbc_pb_svc_pp_pcs(TM_PP_MOS);
        lc_put_pcs      (p_pcs, LC_PCS_MSK_FCODE, LC_PCS_OFF_FCODE,
                                                  LC_PCS_VAL_FCODE_08);
        lc_put_pcs      (p_pcs, LC_PCS_MSK_TYPE,  LC_PCS_OFF_TYPE,
                                                  LC_PCS_VAL_TYPE_SNK);
        lc_put_pcs_mvbc (p_pcs, LC_PCS_MSK_NUM,   LC_PCS_OFF_NUM,
                                                  LC_PCS_VAL_NUM_SET );

        bah_mvbc_sf_put ((0x8000 + (bat_dta.version & 0x7FFF)), TM_PP_FC8);
        bah_mvbc_sf_put ((0x8000 + (bat_dta.version & 0x7FFF)), TM_PP_FC8);

    }
#endif
        p_kc4 = p_pt->p_kc4;

        bat_dta.rank       = 16;
        bat_dta.c_transfer = p_pt->p_div->bmt_cycle < 5 ? 8 :
                            (p_pt->p_div->bmt_cycle) << 1;

        p_mf = &p_kc4->kc4_mf[0];
        p_red_ba_list = &bat_dta.kc4_list[0];

        bat_dta.c_red_ba = p_pt->p_div->c_kc4 - 1;

        for (i = 0; i <= bat_dta.c_red_ba; i++)
        {
            da = (*p_mf++) & 0xFF;

            if  (bat_dta.rank != 16)
            {
                *p_red_ba_list++ = da;
            }
            else if (da == my_da)
            {
                bat_dta.rank = i;
            }
        }

        p_mf = &p_kc4->kc4_mf[0];

        for (i = 0; i < bat_dta.rank; i++)
        {
            *p_red_ba_list++ = (*p_mf++) & 0xFF;
        }

        bai_hamster_cond (bat_dta.rank == 16, BAI_DU_WARN, BAI_DU_CONT, "dvc addr not found");

    }

    return (result);
}


/*  new page
 ------------------------------------------------------------------------------
|
|   name        bat_rank
|
|   func val    -
|
 ------------   A B S T R A C T   -------------------------------------------
*/

unsigned int    bat_rank (void)
{

    return (bat_dta.rank);
}



/*  new page
 ------------------------------------------------------------------------------
|
|   name        bat_eval_dsw_rsp
|
|   func val    -
|
 ------------   A B S T R A C T   -------------------------------------------
*/

short          bat_eval_dsw_rsp (struct BA_STR_SCAN_RSLT_MBR * p_dvc)

{
short rslt = BA_E;
unsigned short dsw = p_dvc->dsw;
unsigned short own_ix, alt_ix;
unsigned short tmp_ix;
unsigned short class;
unsigned short ba_cfgd;

    if  (p_dvc->rsp == BA_RSP_OK)
    {
        class   = bai_dsw_extract (dsw, BAI_DSW_CLS_MSK, BAI_DSW_CLS_OFF);
        ba_cfgd = bai_dsw_extract (dsw, BAI_DSW_ACT_MSK, BAI_DSW_ACT_OFF);

		/* <ATR:01> */
        if  ( (class & BAI_DSW_CLS_BA) && (ba_cfgd == BAA_ACTIVE))
//      if  ( (class == BAI_DSW_CLS_BA) && (ba_cfgd == BAA_ACTIVE))
        {
            own_ix = bat_dta.version & 0x3;
            alt_ix = (own_ix + 1)  & 0x3;
            tmp_ix = bai_dsw_extract (dsw, BAI_DSW_IDX_MSK, BAI_DSW_IDX_OFF);
            if ((own_ix == tmp_ix) || (alt_ix == tmp_ix))
            {
               rslt = RET_OK;
            }
        }
    }
    return (rslt);
}

/*  new page
 ------------------------------------------------------------------------------
|
|   name        bat_task
|
|   func val    -
|
 ------------   A B S T R A C T   -------------------------------------------

*/

void            bat_task        (void)
{
struct BA_STR_SCAN_RSLT_MBR sn_rslt;
int            i;
unsigned short dvc_nr;
  signed short rslt = !RET_OK;  /* 95-10-24 ma, was RET_OK */

        if  (++bat_dta.window >= bat_dta.c_transfer)
        {
            bat_dta.window    = 0;
            bat_dta.mf_nxt_bm = 0;

            for (i = 0; i < bat_dta.c_red_ba; i++)
            {
                dvc_nr = bat_dta.kc4_list[i] & 0xFF;
                bad_dsw_read (dvc_nr, &sn_rslt);
                if ((rslt = bat_eval_dsw_rsp (&sn_rslt)) == RET_OK)
                {
                    bat_dta.mf_nxt_bm = 0x8000 | dvc_nr;
                    break;
                }
            }
            if (rslt == RET_OK)
            {
                bal_prepare_m_transfer ();
            }
        }
}


void    bat_accept_mastership  (unsigned short func_param)

{
        func_param = func_param;

        bah_mvbc_fc_reset ();

        bam_st_trans_di_ext (BAM_BM);
}


void    bat_pass_on_mastership  (unsigned short func_param)

{
struct BAL_STR_RSP  sf_rsp;

        func_param = func_param;

#if !defined (O_BAP)
        {
        LC_TYPE_TS_PCS *  p_pcs;
            p_pcs = bah_mvbc_pb_svc_pp_pcs(TM_PP_MOS);
            lc_put_pcs (p_pcs, LC_PCS_MSK_DIAG, LC_PCS_OFF_DIAG,
                                                LC_PCS_VAL_DIAG_RDY);
        }
#endif

        bah_send_mf  (bat_dta.mf_nxt_bm);

        bah_tm_rcve_sf (&sf_rsp);

        bam_st_trans_di_ext (BAM_RA);

#if (0)
        if  (sf_rsp.state == BA_RSP_OK)
        {
            if (baa_bf_2_int (&sf_rsp.r_sf,
                              BAA_MTT_ACC_MSK,
                              BAA_MTT_ACC_OFF) != BAA_MTT_ACCEPTD)
            {
                bam_st_trans_di_ext (BAM_BM);
            }
        }
#endif

}

/*
 ---------------------------------------------------------------------------Ä
|
|   M O D I F I C A T I O N S       BAT_TFER.C
|   - - - - - - - - - - - - -       ----------
|
 ----------------------------------------------------------------------------


    bat_task                        Nr: .2      dated:  95-10-25

    rslt wrongly initialised

 ----------------------------------------------------------------------------
    name of object modified         Nr: .2      dated:  9?-??-??

    description of what was done to object

 ----------------------------------------------------------------------------

*/
