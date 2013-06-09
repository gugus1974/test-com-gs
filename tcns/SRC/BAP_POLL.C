/*   BAP_POLL.C
 ----------------------------------------------------------------------------
|
|    Property of  :   ABB Verkehrssysteme AG, CH-8050 Zrich / Switzerland
|    COPYRIGHT    :   (c) 1993, 1995 ABB Verkehrssysteme AG
|
 ----------------------------------------------------------------------------
|
|    Project      :   T C N   B U S   A D M I N I S T R A T O R
|
 ----------------------------------------------------------------------------
|
|    File Name    :   BAP_POLL.C
|
|    Document     :
|
|    Abstract     :   MVB Event Poll
|
|    Remarks      :
|
|    Dependencies :
|
|    Accepted     :
|
 ----------------------------------------------------------------------------
|
|    HISTORY:
|
|    Vers  Dept.   YY-MM-DD  State    Name    Reference
|    ----  ------  --------  -------  ------- ---------
|      1   BAEI-2  93-01-11  created  Marsden
|
 ----------------------------------------------------------------------------
*/

#define      BAP_POLL_C

#include    "baa_auxi.h"
#include    "bac_cons.h"
#include    "bad_diag.h"
#include    "bah_hwre.h"
#include    "bai_ifce.h"
#include    "bal_link.h"
#include    "bap_poll.h"
#include    "bax_incl.h"


#if         defined (BX_SIMU)
#include    "bsp_poll.h"
#include    "btp_poll.h"
#endif



/*  new page
 ----------------------------------------------------------------------------
|
|   prototypes
|
 --------   A B S T R A C T   -----------------------------------------------

*/

void    bap_eth_error           (struct BAL_STR_Q * p_q_sf);
void    bap_etl_error           (struct BAL_STR_Q * p_q_sf);
int     bap_etl_poll_group      (struct BAL_STR_Q * p_q_sf);
int     bap_etl_poll_global     (struct BAL_STR_Q * p_q_sf);
int     bap_etl_global          (struct BAL_STR_Q * p_q_sf);
int     bap_etl_start           (struct BAL_STR_Q * p_q_sf);
int     bap_eth_poll_group      (struct BAL_STR_Q * p_q_sf);
int     bap_eth_start           (struct BAL_STR_Q * p_q_sf);
int     bap_eth_global          (struct BAL_STR_Q * p_q_sf);
int     bap_eth_poll_global     (struct BAL_STR_Q * p_q_sf);
int     bap_et_error            (struct BAL_STR_Q * p_q_sf);


/* new page
 ------------------------------------------------------------------------------
|
|   Global Data
|
 ------------   A B S T R A C T   ---------------------------------------------

*/

struct      BAP_STR_IND_IND_CALL    bap_gbl;



/*  new page
 ----------------------------------------------------------------------------
|
|   name        bap_p_dta           static data access function
|
 ------------   A B S T R A C T   -------------------------------------------


*/

static  struct  BAP_STR_DTA     bap_dta;

void            bap_open        (struct BAP_STR_DTA **  p_bap_dta)

{
                *p_bap_dta = &bap_dta;
}



/*  new page
 ------------------------------------------------------------------------------
|
|   name        bap_init
|
 ------------   A B S T R A C T   ---------------------------------------------

    Clears the static variables, exports the location of the constant
    transition table.

*/

void            bap_init            (void)
{

    bai_memset (&bap_dta, 0, sizeof (bap_dta));
    bai_memset (&bap_gbl, 0, sizeof (bap_gbl));

    bap_dta.p_tt             = &bap_trans[0];

    bap_gbl.p_iic            = &bap_dta.low.p_nxt;

    bap_dta.low.p_nxt        = bap_et_error;
    bap_dta.hgh.p_nxt        = bap_et_error;

	bap_dta.hgh.p_p_frontier = &bap_dta.low.p_nxt;
	bap_dta.low.p_p_frontier = &bap_dta.low.p_nxt;

	bap_create (BA_PL_ETH_N_ETL_Y >> 12, BAA_PASSIVE);

	bap_dta.low.gl_ins.n_mf = BAP_FRST_MF_GRP_POLL;
}


/* new page
 ------------------------------------------------------------------------------
|
|   name        bap_create
|
 ------------   A B S T R A C T   ---------------------------------------------


  To initialise the poll memory. All variables which are either not used
  for this particular strategy or which must be initialised by the state
  machine before use are initialised to "error". The illegal use of these
  values shall lead to a hamster.

  If both priority polling is supported, a switch from low to high priority
  polling is done after the detection of each low priority event. The switch
  back to low priority polling requires a "continue" telegram. In this case,
  "nxt_state" is CONTINUE_NO_REPLY. If only low priority polling is supported,
  there will be no switch to high priority polling, and the "nxt_state" is
  a further GROUP POLL.

  The variable "mf_nxt" must be written by the state machine before its first
  use. It is initialised to "error frame".

*/


void           bap_create          (unsigned short     strategy,
                                    unsigned short     no_second_poll)
{
	bap_dta.poll_strategy = strategy;
    if (no_second_poll == BAA_ACTIVE)
    {
        bah_init_no_second_poll ();
    }
}

short bap_reset (void)
{
//    short rslt = RET_OK;

	switch (bap_dta.poll_strategy)
	{

	/*
	 ------------------------------------------------------------
	|
	|   HIGH priority poll: NO
	|   LOW  priority poll: NO
	|
	 ------------   A B S T R A C T   ---------------------------
	*/

		case BAP_ETH_N_ETL_N:
		{
			bap_dta.hgh_rslt_glob_act = BA_E;
			bap_dta.hgh_rslt_glob_pas = BA_E;
			break;
        }

    /*
     ------------------------------------------------------------
    |
    |   HIGH priority poll: NO
    |   LOW  priority poll: YES
    |
     ------------   A B S T R A C T   ---------------------------

        Only LOW (EA 1) Priority Event Arbitration supported.

		The global indirect call pointer (bap_poll()) is initialised to call
        the first low priority function etl_start.

        The low.p_p_frontier is not used (read by high prio func for trans).
        The low.p_nxt  is bap_etl_start  (first low priority function)
		The low.p_n_mf is not used       (read by high prio func for trans).
        The low.n_mf   is read indirectly to start new round (hgh.p_n_mf)
                       ATTN: must be dynamically updated (<--> hgh.n_mf)

        The hgh.p_p_frontier redirects the calls to low.p_nxt
        The hgh.p_nxt  is not used, all calls redirected to low.p_nxt
        The hgh.p_n_mf is directed to NEXT low poll MF (updtd from q_sf.n_mf)
        The hgh.n_mf   is not used, as all accesses are redirected to low.n_mf

        The first MF is loaded to the queue.


    */

		case BAP_ETH_N_ETL_Y:
        {
            bap_gbl.p_iic             = &bap_dta.low.p_nxt;
            bap_dta.hgh_rslt_glob_act = BA_E;
            bap_dta.hgh_rslt_glob_pas = RET_OK;
			bap_dta.low_cont_mf       = BAP_ETL_MF_STRT_RPLY;

        /*  bap_dta.low.p_p_frontier = NULL;                       not used */
            bap_dta.low.p_nxt        = bap_etl_start;
        /*  bap_dta.low.p_n_mf       = NULL;                       not used */
            bap_dta.low.n_mf         = BAP_ETL_MF_STRT_RPLY;

            bap_dta.hgh.p_p_frontier = &bap_dta.low.p_nxt;
        /*  bap_dta.hgh.p_nxt        = NULL;                       not used */
            bap_dta.hgh.p_n_mf       = &bap_dta.low.n_mf;
        /*  bap_dta.hgh.n_mf         = NULL                        not used */

            bal_pl_q_put_n_pf (BAP_ETL_MF_STRT_RPLY);


            break;
        }

    /*
	 ------------------------------------------------------------
    |
    |   HIGH priority poll: NO
    |   LOW  priority poll: YES
    |
     ------------   A B S T R A C T   ---------------------------

        Only HIGH (EA 0) Priority Event Arbitration supported.

        The global indirect call pointer (bap_poll()) is initialised to call
        the first high priority function.

        The hgh.p_p_frontier is not used (read by low prio func for trans).
        The hgh.p_nxt  is bap_eth_start (first high priority function)
		The hgh.p_n_mf is not used       (read by low prio func for trans).
        The hgh.n_mf   is read indirectly to start new round (low.p_n_mf)

        The low.p_p_frontier redirects the calls to high priority p_nxt
        The low.p_nxt  is not used, all calls redirected to hgh.p_nxt
		The low.p_n_mf is directed to the first hgh poll MF.
        The low.n_mf   is not used   (all accesses are redirected to hgh.n_mf)

        The first MF is loaded to the queue.

    */

        case BAP_ETH_Y_ETL_N:
        {

            bap_gbl.p_iic            = &bap_dta.hgh.p_nxt;
        /*  bap_dta.hgh_rslt         = RET_OK;                     not used */
        /*  bap_dta.hgh_rslt         = RET_OK;                     not used */

		/*  bap_dta.hgh.p_p_frontier = NULL;                       not used */
            bap_dta.hgh.p_nxt        = bap_eth_start;
        /*  bap_dta.hgh.p_n_mf       = NULL;                       not used */
            bap_dta.hgh.n_mf         = BAP_ETH_MF_STRT_RPLY;

			bap_dta.low.p_p_frontier = &bap_dta.hgh.p_nxt;
        /*  bap_dta.low.p_nxt        = NULL;                       not used */
            bap_dta.low.p_n_mf       = &bap_dta.hgh.n_mf;
        /*  bap_dta.low.n_mf         = NULL                        not used */
		/*  bap_dta.low.low_cont_mf  = NULL                        not used */

            bal_pl_q_put_n_pf (BAP_ETH_MF_STRT_RPLY);

            break;
        }

    /*
     ------------------------------------------------------------
    |
	|   HIGH priority poll: YES
    |   LOW  priority poll: YES
    |
     ------------   A B S T R A C T   ---------------------------
    */

        case BAP_ETH_Y_ETL_Y:
		{
			bap_gbl.p_iic             = &bap_dta.low.p_nxt;
			bap_dta.hgh_rslt_glob_act = RET_OK;
			bap_dta.hgh_rslt_glob_pas = RET_OK;
			bap_dta.low_cont_mf  = BAP_ETL_MF_STRT_RPLY;

            bap_dta.hgh.p_p_frontier = &bap_dta.hgh.p_nxt;
            bap_dta.hgh.p_nxt        = bap_eth_start;
            bap_dta.hgh.p_n_mf       = &bap_dta.hgh.n_mf;
            bap_dta.hgh.n_mf         = BAP_ETH_MF_STRT_RPLY;

            bap_dta.low.p_p_frontier = &bap_dta.low.p_nxt;
			bap_dta.low.p_nxt        = bap_etl_start;
			bap_dta.low.p_n_mf       = &bap_dta.low_cont_mf;
            bap_dta.low.n_mf         = BAP_ETL_MF_STRT_RPLY;

            bal_pl_q_put_n_pf (BAP_ETL_MF_STRT_RPLY);

            break;
        }

		default:
		{
			bap_dta.poll_strategy = BAP_ETH_Y_ETL_Y;
			bap_reset ();

			break;
		}
	}
                                        #if !defined (O_BAP)
    {
    LC_TYPE_TS_PCS *  p_pcs;
        p_pcs = bah_mvbc_pb_svc_pp_pcs(TM_PP_EFS);
        lc_put_pcs      (p_pcs, LC_PCS_MSK_DIAG,  LC_PCS_OFF_DIAG,
                                                  LC_PCS_VAL_DIAG_RDY);
        lc_put_pcs      (p_pcs, LC_PCS_MSK_FCODE, LC_PCS_OFF_FCODE,
                                                  LC_PCS_VAL_FCODE_09);
        lc_put_pcs      (p_pcs, LC_PCS_MSK_TYPE,  LC_PCS_OFF_TYPE,
                                                  LC_PCS_VAL_TYPE_SNK);
        lc_put_pcs_mvbc (p_pcs, LC_PCS_MSK_NUM,   LC_PCS_OFF_NUM,
                                                  LC_PCS_VAL_NUM_SET );
    }
                                        #endif
	bap_dta.low.gl_ins.n_mf = BAP_FRST_MF_GRP_POLL;

//    return (rslt);
    return (RET_OK);
}





/* new page
 ----------------------------------------------------------------------------
|
|   name    bap_eth_start
|
|   return  void
|
 --------   A B S T R A C T   -----------------------------------------------

    This is the low to high priority poll switch frontier post (arrivals).
    The locomotive (bap_gbl.p_iic) is changed to high priority indirect call
    before the journey continues.

    The event log table is cleared.
*/

int bap_eth_start           (struct BAL_STR_Q * p_q_sf)

{
int result;
unsigned short state;

            if ((state = p_q_sf->rsp.state) == BA_RSP_MISSING)
            {
                result = BA_E;

                bap_gbl.p_iic     =  bap_dta.low.p_p_frontier;
                p_q_sf->n_pf      = *bap_dta.low.p_n_mf;

/*              bap_dta.hgh.p_nxt =  bap_eth_start;     same again      */

            }
            else if  (state == BA_RSP_OK)
            {
                result = BA_E;     /* end of this round */
                bap_dta_h_nxt (bap_eth_global, p_q_sf, BAP_ETH_MF_CONT_RPLY);
            }

            else /* if (state == BA_DT_CORRUPT) */
            {
                result = RET_OK;
                bap_dta_h_nxt (bap_eth_poll_group, p_q_sf, BAP_FRST_MF_GRP_POLL);
            }

            return (result);
}


/* new page
 ----------------------------------------------------------------------------
|
|   name    bap_eth_global
|
|   return  void
|
 --------   A B S T R A C T   -----------------------------------------------


    It looks like every event has been found. Just to be sure that none were
    missed, we'll poll the whole address space.

*/

int  bap_eth_global          (struct BAL_STR_Q * p_q_sf)
{
  signed int    result;
unsigned short  state;

    if ((state = p_q_sf->rsp.state) == BA_RSP_MISSING)
    {
        result = BA_E;
    /*  bap_dta_h_rst (bap_eth_start, BAP_ETH_MF_STRT_RPLY);    */

        bap_gbl.p_iic     =  bap_dta.low.p_p_frontier;
        p_q_sf->n_pf      = *bap_dta.low.p_n_mf;
        bap_dta.hgh.p_nxt =  bap_eth_start;

    }
    else if  (state == BA_RSP_OK)
    {
        result = BA_E;
        p_q_sf->n_pf      =  BAP_ETH_MF_CONT_RPLY;
/*      bap_dta.hgh.p_nxt =  bap_eth_global;        same again  */
    }

    else /* if (state == BA_DT_CORRUPT)  */
    {
        result = RET_OK;
        bap_dta_h_nxt (bap_eth_poll_group, p_q_sf, BAP_FRST_MF_GRP_POLL);
    }

    return (result);
}

/*  new page
 ----------------------------------------------------------------------------
|
|   name    bap_eth_poll_group
|
|   return  void
|
 --------   A B S T R A C T   -----------------------------------------------

    A group poll telegram is transmitted. Depending upon the response
    type, either further polling may be required.

*/

int   bap_eth_poll_group      (struct BAL_STR_Q * p_q_sf)

{
  signed int        result;
unsigned short      mf_nxt;
unsigned short      state;

    if ((state = p_q_sf->rsp.state) == BA_RSP_OK)
    {
        mf_nxt = bap_trans[p_q_sf->n_pf & 0x1FF].mfa[state];

        if  (mf_nxt == 0xFFFF)
        {
            result = BA_ERROR;
            bap_dta_h_nxt(bap_eth_global, p_q_sf, BAP_ETH_MF_CONT_RPLY);
        }
        else
        {
            result = RET_OK;
            p_q_sf->n_pf = mf_nxt; /* p_nxt remains same */
        }
    }
    else
    {
        mf_nxt = bap_trans[p_q_sf->n_pf & 0x1FF].mfa[state];

        if (mf_nxt == 0xFFFF)
        {
            result = BA_ERROR;

            if (state == BA_RSP_ERROR)
            {
                bap_eth_error (p_q_sf);
            }
            else
            {
                bap_dta_h_nxt (bap_eth_global, p_q_sf, BAP_ETH_MF_CONT_RPLY);
            }
        }
        else
        {
            result = RET_OK;

            p_q_sf->n_pf = mf_nxt; /* p_nxt remains same */
        }
    }

    return (result);
}



/*  new page
 ----------------------------------------------------------------------------
|
|   name        bap_etl_start
|
|   return      void
|
 ------------   A B S T R A C T   -------------------------------------------


*/


int             bap_etl_start       (struct BAL_STR_Q * p_q_sf)

{
  signed int    result;
unsigned short  state;


	bap_dta.low.sf_log = 0;
	bap_dta.low.gl_ins.c_mf = 0;

	if ((state = p_q_sf->rsp.state) == BA_RSP_MISSING)
	{
		result = bap_dta.hgh_rslt_glob_act;

		bap_gbl.p_iic         =  bap_dta.hgh.p_p_frontier;
		bap_dta.low.n_mf      =  BAP_ETL_MF_STRT_RPLY;
		p_q_sf->n_pf          = *bap_dta.hgh.p_n_mf;
	}
	else
	{
		bap_dta.low_cont_mf   =  BAP_ETL_MF_CONT_SLNC;

		if  (state == BA_RSP_OK)
		{
			result = bap_dta.hgh_rslt_glob_act;

			bap_gbl.p_iic         =  bap_dta.hgh.p_p_frontier;

			bap_dta.low.p_nxt     =  bap_etl_global;
			bap_dta.low.n_mf      =  BAP_ETL_MF_CONT_RPLY;

			p_q_sf->n_pf          = *bap_dta.hgh.p_n_mf;
		}

		else /* if (state == BA_DT_CORRUPT)  */
		{
			result = RET_OK;
			bap_dta_l_nxt (bap_etl_poll_group, p_q_sf, BAP_FRST_MF_GRP_POLL);
		}
	}

    return (result);
}

void        bap_etl_cont            (struct BAL_STR_Q * p_q_sf)
{
	 p_q_sf->n_pf = bap_dta.low.n_mf;
}

/*  new page
 ----------------------------------------------------------------------------
|
|   name        bap_etl_global
|
|   return      void
|
 ------------   A B S T R A C T   -------------------------------------------


*/

int             bap_etl_global      (struct BAL_STR_Q * p_q_sf)

{
  signed int    result;
unsigned short  state;

	if  ((state = p_q_sf->rsp.state) == BA_RSP_OK)
	{
		result = bap_dta.hgh_rslt_glob_act;

		bap_dta.low.n_mf =  BAP_ETL_MF_CONT_RPLY;
		bap_gbl.p_iic    =  bap_dta.hgh.p_p_frontier;
		p_q_sf->n_pf     = *bap_dta.hgh.p_n_mf;

		if  (bap_dta.low.sf_log == p_q_sf->rsp.r_sf)
		{
			result = RET_OK;
			bap_dta.low.n_mf = BAP_ETL_MF_STRT_RPLY;
			bap_dta_l_nxt (bap_etl_start, p_q_sf, BAP_ETL_MF_STRT_RPLY);
		}
		else
		{
			bap_dta.low.sf_log = p_q_sf->rsp.r_sf;
		}
	}

	else if (state == BA_RSP_ERROR)
	{
		result = RET_OK;
		bap_dta_l_nxt (bap_etl_poll_group, p_q_sf, bap_dta.low.gl_ins.n_mf);
	}

	else /* if (state == BA_DT_MISSING) */
	{
		result = RET_OK;
		bap_dta.low.n_mf = BAP_ETL_MF_STRT_RPLY;
		bap_dta_l_nxt (bap_etl_start, p_q_sf, BAP_ETL_MF_STRT_RPLY);
	}

	bap_dta.low.gl_ins.n_mf = BAP_FRST_MF_GRP_POLL;

	return (result);
}




/*  new page
 ----------------------------------------------------------------------------
|
|   name    bap_etl_poll_group
|
|   return  void
|
 --------   A B S T R A C T   -----------------------------------------------
*/

int         bap_etl_poll_group      (struct BAL_STR_Q * p_q_sf)

{
  signed int        result;
unsigned short      mf_nxt;
unsigned short      state;

    if ((state = p_q_sf->rsp.state) == BA_RSP_OK)
    {
        bap_dta.low.gl_ins.c_mf++;
        mf_nxt = bap_trans[p_q_sf->n_pf & 0x1FF].mfa[state];

        if  (mf_nxt == 0xFFFF)
        {
            result = bap_dta.hgh_rslt_glob_act;
            bap_dta.low.p_nxt =  bap_etl_global;
            bap_dta.low.n_mf  =  BAP_ETL_MF_CONT_RPLY;
        }
        else if (bap_dta.low.gl_ins.c_mf < 2)
        {
            result = bap_dta.hgh_rslt_glob_pas;
        /*  bap_dta.low.p_nxt =  bap_etl_poll;      cont with same val  */
            bap_dta.low.n_mf  =  mf_nxt;
        }
        else
        {
            result = bap_dta.hgh_rslt_glob_act;
            bap_dta.low.p_nxt       =  bap_etl_global;
            bap_dta.low.n_mf        =  BAP_ETL_MF_CONT_RPLY;
            bap_dta.low.gl_ins.n_mf =  mf_nxt;
        }

        bap_gbl.p_iic         =  bap_dta.hgh.p_p_frontier;
        p_q_sf->n_pf          = *bap_dta.hgh.p_n_mf;
    }
    else
    {
        mf_nxt = bap_trans[p_q_sf->n_pf & 0x1FF].mfa[state];

        if (mf_nxt == 0xFFFF)
        {
            if (state == BA_RSP_ERROR)
            {
                result = BA_ERROR;
                bap_etl_error (p_q_sf);
            }
            else
            {
                result = bap_dta.hgh_rslt_glob_act;
                bap_dta_l_nxt (bap_etl_global, p_q_sf, BAP_ETL_MF_CONT_RPLY);
            }
        }
        else
        {
            result = RET_OK;

        /*  bap_dta.low.p_nxt_nxt =  bap_etl_poll;      cont with same val  */
            p_q_sf->n_pf = mf_nxt;
        }
    }

    return (result);
}





void    bap_etl_error              (struct BAL_STR_Q * p_q_sf)
{
        bap_gbl.p_iic     =  bap_dta.hgh.p_p_frontier;
        p_q_sf->n_pf      = *bap_dta.hgh.p_n_mf;

        bap_dta.low.p_nxt =  bap_etl_start;
        bap_dta.low.n_mf  =  BAP_ETL_MF_STRT_RPLY;
}

void    bap_eth_error              (struct BAL_STR_Q * p_q_sf)
{
        bap_gbl.p_iic = bap_dta.low.p_p_frontier;
        p_q_sf->n_pf = *bap_dta.low.p_n_mf;
        bap_dta.hgh.p_nxt =  bap_eth_start;

}

int bap_et_error (struct BAL_STR_Q * p_q_sf)
{
//    int i;
    p_q_sf = p_q_sf;

    for (;;)
    {
//        i = 1;
//        i = i;
    }
}




/*
 ---------------------------------------------------------------------------Ä
|
|   M O D I F I C A T I O N S       BAP_POLL.C
|   - - - - - - - - - - - - -       ----------
|
 ----------------------------------------------------------------------------

    Transition table                Nr: 2  dated:  93-02-01

    Extended to accomodate "Response" and "Silence" separately

 ----------------------------------------------------------------------------

    bap_poll                        Nr: .3  dated:  93-02-17

    case table split into separate subscribable functions

 ----------------------------------------------------------------------------

*/
