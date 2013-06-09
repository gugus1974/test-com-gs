/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> the BA in the DSW is correctly treated as a flag                                    */
/*==============================================================================================*/

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
|    File Name    :   BAM_MAIN.C
|
|    Document     :
|
|    Abstract     :   Bus Administrator State Control
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
|    Vers   Dept.   YY-MM-DD  State    Name    Reference
|    ----   ------  --------  -------  ------- ---------
|      1    BATC    94-01-01  created  Marsden
|
 ------------------------------------------------------------------------------
*/

#define      BAM_MAIN_C

#include    "pi_sys.h"
#include    "baa_auxi.h"
#include    "bad_diag.h"
#include    "bai_ifce.h"
#include    "bah_hwre.h"
#include    "bah_tcfg.h"
#include    "bal_link.h"
#include    "bam_main.h"
#include    "bap_poll.h"
#include    "bat_tfer.h"
#include    "bax_incl.h"


#if defined (O_EKR)
#include    "bao_scfg.h"
#endif

#if         defined (BX_SIMU)

#include    "bsm_main.h"
#include    "btm_main.h"

#endif

#define      BAM_INIT_KEY 0x5A93C68EL


/*  new page
 ----------------------------------------------------------------------------
|
|   contents
|
 ----------------------------------------------------------------------------

    data                ba_scan
                        bam_dta
                        bam_cpt
    constants

    functions           bam_open
                        bam_init
                        bam_config
                        bam_create
                        bam_reset
                        bam_task_act
                        bam_st_2_bm
                        bam_st_2_ra
                        bam_st_trans_di_ext
                        bam_main
                        ba_task
                        ba_switch_configuration
                        ba_subscribe_diagnosis

*/

void  bam_config              (struct BA_STR_CONFIGURATION *  p_cfg_1);
short bam_reset               (void);


/*  new page
 ----------------------------------------------------------------------------
|
|   Internal data and access functions
|
 ----------------------------------------------------------------------------

    ba_scan replaces the scan list which should be provided by the
    configuration

    bam_cpt is the cyclic pointer table. It is filled in by BAC_CYCL

*/

static  struct BAL_STR_CX_PT    bam_cpt;
static  struct BA_STR_CFG_SCN   ba_scan;
static  struct BAM_STR_DTA      bam_dta;

void            bam_open        (struct BAM_STR_DTA **  p_bam_dta)

{
                *p_bam_dta = &bam_dta;
}


/*  new page
 ----------------------------------------------------------------------------
|
|   name        bam_init
|
 ------------   A B S T R A C T   -------------------------------------------

	Creates a determined start up state by clearing all project control
	information. It must be called first.

    It calls each module init routine. bal_init () is called last.
    It creates the semaphore required by bam_main ().

    The BA is a redundant, unconfigured BA after this call is processed.
    The BA will take over bus mastership in accordance to the bus silence
    countdown rules.

    Attention: Do not modify calling order
*/

void            bam_init            (void)
{
	bai_memset (&bam_dta, 0, sizeof (struct BAM_STR_DTA));

   	bah_init ();
    baa_init ();
    bad_init ();
    bat_init ();
    bap_init ();

    {
    short rslt ;
        bai_sema_create (0, PI_ORD_FCFS, &bam_dta.sema_id, &rslt);
        bai_hamster_cond (rslt, BAI_DU_ERR, BAI_DU_RESET, "semaphore could not be created");
        if (rslt == RET_OK)
        {
            bam_dta.init_key = BAM_INIT_KEY;
        }
    }

    bal_init ();
    bam_st_trans_di_int (BAM_RA_NCFG);
}


/*  new page
 ----------------------------------------------------------------------------
|
|   name        bam_create
|
 ------------   A B S T R A C T   -------------------------------------------

    This function process the new configuration. If it terminates
    successfully, the BA will transit to "configured redundant", or
    "configured bus master".

    If a configuration error is detected, the state will remain
    "unconfigured"

*/

int bam_create (unsigned int bus_id,
                struct BA_STR_CONFIGURATION *  p_cfg)

{
short           rslt = RET_OK;

    bus_id = bus_id;
    bai_memset (&bam_cpt, 0, sizeof (struct BAL_STR_CX_PT));

	if  (p_cfg != NULL)
	{
		if  ((bam_dta.state & BAM_NCFG) == BAM_NCFG)
		{
			bam_dta.mtvc = (p_cfg->mtvc & 3) << 10;
            bai_get_dvc_addr (&bam_dta.ba_cfg.own_dvc_addr);
            bam_config (p_cfg);

			rslt = baa_create   (&bam_dta.pt_cfg) == RET_OK ? rslt : BA_E ;
			rslt = bal_init_cpt (&bam_cpt, p_cfg) == RET_OK ? rslt : BA_E;
				   bap_create   (bam_dta.ba_cfg.pl_strategy,
                                 bam_dta.ba_cfg.pl_no_second_poll);
			rslt = bat_create   (&bam_dta.pt_cfg) == RET_OK ? rslt : BA_E ;
                   bad_create   (&bam_dta.pt_cfg);

			if (rslt == RET_OK)
			{
				bal_create (&bam_dta.pt_cfg);
				bam_st_trans_di_int (BAM_CFGD);
                vch_func_call (printf("configuration active"));
			}
            else
            {
                bai_hamster_uncond (BAI_DU_WARN, BAI_DU_CONT, "BA config error");
            }
		}
		else
		{
            bai_hamster_uncond (BAI_DU_WARN, BAI_DU_CONT, "create on configured BA error");
			rslt = BA_E;
		}
	}
	return (rslt);
}


/*  new page
 ----------------------------------------------------------------------------
|
|   name        bam_task
|
 ------------   A B S T R A C T   -------------------------------------------

  This function will be executed as a 512 ms task, pending upon a semaphore
  with a timeout cycle of 512 ms.

  It must not be started BEFORE the bam_init () sequence. This is checked
  by the init key. If the init_key is not set, then there is no existing
  semaphore to pend on. It must create its own which will not be known to
  the rest of the software because bam_init() will clear the static
  variables. It will poll upon this semaphore searching for the "init_key".
  When the init_key is found, it will transit to pending upon the official
  semaphore.

  Its duty is to provide a 512 ms timed task for the DSW scan module and
  the bus master transfer module.

  See bad_task () and bat_task () for further details.

*/


void    bam_task      (unsigned int bus_id)
{
unsigned short timeout;
    bus_id = bus_id;

    if  (bam_dta.init_key != BAM_INIT_KEY)
    {
    short rslt ;
    short sema_id;
        timeout = 512 / PI_TICK;
        bai_sema_create (0, PI_ORD_FCFS, &sema_id, &rslt);
        bai_hamster_cond (rslt, BAI_DU_ERR, BAI_DU_RESET, "sema create error");

        while (bam_dta.init_key != BAM_INIT_KEY)
        {
            bai_sema_pend (sema_id, timeout, &rslt);
        }
    }
    bam_dta.init_key = 0;

    timeout = 256 / PI_TICK;
                                        #if !defined (BX_SIMU)
    for (;;)
    {
     short   rslt = BA_E;

        if  ((bam_dta.state & BAM_CFGD) == BAM_CFGD)
        {
           for (;;)
           {
            bai_sema_pend (bam_dta.sema_id, PI_FOREVER, &rslt);
            bai_hamster_cond (rslt, BAI_DU_ERR, BAI_DU_RESET, "sema pend error");
            bad_task ();
            bah_mvbc_func (bal_task ());
            bat_task ();
            vch_func_call (printf ("."));
           }
        }
        else
        {
            bai_sema_pend (bam_dta.sema_id, timeout, &rslt);
            vch_func_call (printf("-"));

            if  (bam_dta.state == BAM_BM_NCFG)
            {
                if ((rslt = bad_ncfg ()) == BAA_ACTIVE)
                {
                    bai_disable ();
                    bam_st_trans_di_ext (BAM_RA);
                    bai_enable  ();
                }
            }
        }
	}
                                        #endif
}



/*  new page
 ----------------------------------------------------------------------------
|
|   name        bam_main            BA task
|
 ------------   A B S T R A C T   -------------------------------------------

    The bus administrator must have one, semaphore driven task.

    bam_main must be called from this task. The semaphore is created
    by this function and there will be no return to the caller!

    bam_main initialises the BA software, creates an unconfigured or
    configured bus administrator dependent upon the pointer p_ba_cfg
    (NULL unconfigured / pointer to configuration)

    An alternative interface is:
    bam_init   ();
    bam_create ();
    bam_task   (); from a task which will become the bus administrator task

*/

void    bam_main            (unsigned int                   bus_id,
                             struct BA_STR_CONFIGURATION *  p_ba_cfg)
{

    bai_si_disable_monitor ();  /* empty function for TCN mode */

	bam_init ();

	bam_create (bus_id, p_ba_cfg);

    bam_task   (bus_id);
}


/*  new page
 ----------------------------------------------------------------------------
|
|   Bus Administrator Interface
|
 ------------   A B S T R A C T   -------------------------------------------

    This is the lme interface "ba_subscribe_diagnosis".

*/



void  bam_diag   (unsigned int bus_id,
                  void (*prc_notified) (struct BA_STR_DIAGNOSIS * p_diag))
{
      bus_id = bus_id;
      bad_diag (prc_notified);
}

/*  new page
 ----------------------------------------------------------------------------
|
|   name        bam_config
|
 ------------   A B S T R A C T   -------------------------------------------

	The data contained in the structure BA_STR_CONFIGURATION is converted
    into structure BA_STR_BA_DTA data, which is processed on-line by the bus
	master. This function will only execute in the states "passive",
	otherwise an error is returned.

*/

/* new page
 -----------------------------------------------------------------------------
|
|   name        EXTRA INFORMATION CONFIGURATION
|
 ------------   A B S T R A C T   --------------------------------------------

    The information configured here is not provided by the BVLCONV tool.

    1000/2000  1 or 2 ms basic period
    0/1        scan known devices only / scan range 0 .... 4095
    5....      pass on bus mastership after x macrocycles.

*/

#define BAH_C_MACROS_IN_TURN 8          /* 5 ... 255    */
#define BAH_SCAN_STRATEGY    1          /* 0 / 1        */
#define BAH_POLL_STRATEGY    1          /* 0 / 1: multi / single poll */

#define BAH_B_PERIOD         1000       /* 1000 or 2000 */

#if     BAH_B_PERIOD <       1501
#define BAH_F_PERIOD         1
#elif   BAH_B_PERIOD <       3001
#define BAH_F_PERIOD         2
#elif   BAH_B_PERIOD <       6001
#define BAH_F_PERIOD         4
#else
#define BAH_F_PERIOD         8
#endif

#define BAH_C_PERIOD         (1024 / BAH_F_PERIOD)

void            bam_config          (struct BA_STR_CONFIGURATION *  p_cfg_1)
{
unsigned char *        p_char;
struct BA_STR_CYC_IX_PTR_TAB * p_it;
int                    i;
    bam_dta.ba_cfg.c_version_update = p_cfg_1->mtvc;
    bam_dta.ba_cfg.basic_cycle      = p_cfg_1->basic_period;
    bam_dta.ba_cfg.bmt_cycle        = p_cfg_1->c_macros_in_turn;
    bam_dta.ba_cfg.ebl              = p_cfg_1->t_reply_max;
    bam_dta.ba_cfg.c_slot           = p_cfg_1->c_periods_in_macro;
    bam_dta.ba_cfg.sn_strategy      = p_cfg_1->scan_strategy;
    bam_dta.ba_cfg.pl_strategy      = p_cfg_1->eps >> 12    ;
    bam_dta.ba_cfg.pl_no_second_poll= /* p_cfg_1->eps & */ 0x0001;

    bam_dta.ba_cfg.c_kc3 = (p_cfg_1->i_nu1 - p_cfg_1->i_kdl) >> 1;
    bam_dta.ba_cfg.c_kc4 = (p_cfg_1->i_snl - p_cfg_1->i_bal) >> 1;

    bam_dta.pt_cfg.p_div  = &bam_dta.ba_cfg;
    bam_dta.pt_cfg.p_cyc  = &bam_dta.ba_cyc;
    bam_dta.pt_cfg.p_cpt  = &bam_cpt;
    bam_dta.pt_cfg.p_scn  = (struct BA_STR_CFG_SCN *)
                            (((char *) p_cfg_1) + p_cfg_1->i_snl);
    bam_dta.pt_cfg.p_kc4  = (struct BA_STR_CFG_KC4 *)
                            (((char *) p_cfg_1) + p_cfg_1->i_bal);

    bam_dta.pt_cfg.p_kc3  = (struct BA_STR_CFG_KC3 *)
                            (((char *) p_cfg_1) + p_cfg_1->i_kdl);

    p_char = ((unsigned char *) p_cfg_1) + p_cfg_1->i_ctl;
    p_it   =  (struct BA_STR_CYC_IX_PTR_TAB *)  p_char;

    bam_dta.ba_cyc.c_0001ms = (p_it->i_cl[1] - p_it->i_cl[0]) >> 1;

    bam_dta.ba_cyc.p_0001ms = p_it->i_cl[0] == p_it->i_cl[1] ? NULL :
                             (TYPE_USHRT_A100 *) (p_char + p_it->i_cl[0]);

    bam_dta.ba_cyc.p_c_0002 =(TYPE_CHAR_A50X2 *) (p_char + p_it->i_sl[0]);

    bam_dta.ba_cyc.p_0002ms = p_it->i_cl[1] == p_it->i_cl[2] ? NULL :
                             (TYPE_USHRT_A100 *)(p_char + p_it->i_cl[1]);

    bam_dta.ba_cyc.p_0004ms = p_it->i_cl[2] == p_it->i_cl[3] ? NULL :
                             (TYPE_USHRT_A100 *)(p_char + p_it->i_cl[2]);

    bam_dta.ba_cyc.p_c_0008 =(TYPE_CHAR_A50X2 *) (p_char + p_it->i_sl[1]);

    bam_dta.ba_cyc.p_0008ms = p_it->i_cl[3] == p_it->i_cl[4] ? NULL :
                             (TYPE_USHRT_A100 *) (p_char + p_it->i_cl[3]);

    bam_dta.ba_cyc.p_0016ms = p_it->i_cl[4] == p_it->i_cl[5] ? NULL :
                             (TYPE_USHRT_A100 *) (p_char + p_it->i_cl[4]);

    bam_dta.ba_cyc.p_c_0032 =(TYPE_CHAR_A50X2 *) (p_char + p_it->i_sl[2]);

    bam_dta.ba_cyc.p_0032ms = p_it->i_cl[5] == p_it->i_cl[6] ? NULL :
                             (TYPE_USHRT_A100 *) (p_char + p_it->i_cl[5]);

    bam_dta.ba_cyc.p_0064ms = p_it->i_cl[6] == p_it->i_cl[7] ? NULL :
                             (TYPE_USHRT_A100 *) (p_char + p_it->i_cl[6]);

    bam_dta.ba_cyc.p_c_0128 =(TYPE_CHAR_A50X2 *) (p_char + p_it->i_sl[3]);

    bam_dta.ba_cyc.p_0128ms = p_it->i_cl[7] == p_it->i_cl[8] ? NULL :
                             (TYPE_USHRT_A100 *) (p_char + p_it->i_cl[7]);

    bam_dta.ba_cyc.p_0256ms = p_it->i_cl[8] == p_it->i_cl[9] ? NULL :
                             (TYPE_USHRT_A100 *) (p_char + p_it->i_cl[8]);

    bam_dta.ba_cyc.p_c_0512 =(TYPE_CHAR_A50X2 *) (p_char + p_it->i_sl[4]);

    bam_dta.ba_cyc.p_0512ms = p_it->i_cl[9] == p_it->i_cl[10] ? NULL :
                             (TYPE_USHRT_A100 *) (p_char + p_it->i_cl[9]);

    bam_dta.ba_cyc.p_1024ms = p_it->i_cl[10] == p_it->i_cl[11] ? NULL :
                             (TYPE_USHRT_A100 *) (p_char + p_it->i_cl[10]);

    if (p_cfg_1->basic_period == 0)
    {
        bam_dta.pt_cfg.p_scn            = &ba_scan;

        bam_dta.ba_cfg.c_slot           = BAH_C_PERIOD;
        bam_dta.ba_cfg.basic_cycle      = BAH_B_PERIOD;
        bam_dta.ba_cfg.fi_slot          = 1;
        bam_dta.ba_cfg.fx_slot          = BAH_F_PERIOD;
        bam_dta.ba_cfg.bmt_cycle        = BAH_C_MACROS_IN_TURN;
        bam_dta.ba_cfg.sn_strategy      = BAH_SCAN_STRATEGY;

        bai_memset (&ba_scan, 0, sizeof (ba_scan));

	    for (i = 128 - (16 * BAH_F_PERIOD); i < 128; i = i + BAH_F_PERIOD)
	    {
		    ba_scan.c_slot [i + 0  ] = 1;
		    ba_scan.c_slot [i + 128] = 1;
		    ba_scan.c_slot [i + 256] = 1;
		    ba_scan.c_slot [i + 384] = 1;
		    ba_scan.c_slot [i + 512 + 0  ] = 1;
		    ba_scan.c_slot [i + 512 + 128] = 1;
		    ba_scan.c_slot [i + 512 + 256] = 1;
		    ba_scan.c_slot [i + 512 + 384] = 1;
        }
    }
    else
    {
        if      (bam_dta.ba_cfg.c_slot == 1024) {bam_dta.ba_cfg.fi_slot = 1;}
        else if (bam_dta.ba_cfg.c_slot ==  512) {bam_dta.ba_cfg.fi_slot = 2;}
        else if (bam_dta.ba_cfg.c_slot ==  256) {bam_dta.ba_cfg.fi_slot = 4;}
        else if (bam_dta.ba_cfg.c_slot ==  128) {bam_dta.ba_cfg.fi_slot = 8;}
        else { bai_hamster_uncond (BAI_DU_ERR, BAI_DU_RESET, "c_slot wrong"); }

        bam_dta.ba_cfg.fx_slot          = 1;       /* slot for slot */
    }
    if      (bam_dta.ba_cfg.basic_cycle <  625) {bam_dta.ba_cfg.sys_c_tick =  2; }
    else if (bam_dta.ba_cfg.basic_cycle <  875) {bam_dta.ba_cfg.sys_c_tick =  3; }
    else if (bam_dta.ba_cfg.basic_cycle < 1125) {bam_dta.ba_cfg.sys_c_tick =  4; }
    else if (bam_dta.ba_cfg.basic_cycle < 1375) {bam_dta.ba_cfg.sys_c_tick =  5; }
    else if (bam_dta.ba_cfg.basic_cycle < 1625) {bam_dta.ba_cfg.sys_c_tick =  6; }
    else if (bam_dta.ba_cfg.basic_cycle < 1875) {bam_dta.ba_cfg.sys_c_tick =  7; }
    else if (bam_dta.ba_cfg.basic_cycle < 2125) {bam_dta.ba_cfg.sys_c_tick =  8; }
    else if (bam_dta.ba_cfg.basic_cycle < 3001) {bam_dta.ba_cfg.sys_c_tick = 12; }
    else if (bam_dta.ba_cfg.basic_cycle < 4001) {bam_dta.ba_cfg.sys_c_tick = 16; }
    else if (bam_dta.ba_cfg.basic_cycle < 5001) {bam_dta.ba_cfg.sys_c_tick = 20; }
    else if (bam_dta.ba_cfg.basic_cycle < 6001) {bam_dta.ba_cfg.sys_c_tick = 24; }
    else if (bam_dta.ba_cfg.basic_cycle < 7001) {bam_dta.ba_cfg.sys_c_tick = 28; }
    else                                    {bam_dta.ba_cfg.sys_c_tick = 32; }
}



/*  new page
 ----------------------------------------------------------------------------
|
|   name        bam_reset
|
 ------------   A B S T R A C T   -------------------------------------------

    During a state transition to "bus master", key modules must be reset
    to guarantee correct restart functionality

*/

short           bam_reset         (void)
{
short   rslt = RET_OK;

        rslt = bap_reset () == RET_OK ? rslt : BA_E;
        rslt = bad_reset () == RET_OK ? rslt : BA_E;
        rslt = bal_reset () == RET_OK ? rslt : BA_E;
        return (rslt);
}



/*  new page
 ----------------------------------------------------------------------------
|
|   name        bam_task_act        activates bus master task
|
 ------------   A B S T R A C T   -------------------------------------------

    The bus administrator task must be reactivated during key periods.
    The handler will call this function to reactivate the BA task.


*/

void            bam_task_act        (void)
{
short tmp_rslt = BA_E;

      bai_sema_post    (bam_dta.sema_id, &tmp_rslt);
      bai_hamster_cond (tmp_rslt, BAI_DU_ERR, BAI_DU_RESET, "sema post error");
}


/*  new page
 ----------------------------------------------------------------------------
|
|   name        bam_st_2_ra         to redundant administrator
|
 ------------   A B S T R A C T   -------------------------------------------

    State transition to redundant administrator. The target is no longer
    a bus master after this function has been called.

    It may become bus master again according to the bus mastership
    acceptance rules.

    First the internal state variable is set to redundant active.
    Then the link layer state transition is called.
    Then the device status word is set "bus administrator, redundant".

    If the BA is configured, then the configuration is announced in the DSW
    and bam_reset is called (resets poll, dsw scan and link layer).
    If the BA is not configured, then only the poll need be reset.

*/

void            bam_st_2_ra         (unsigned short     state)
{
short rslt;

	bam_dta.state =  BAM_RA | state;
	bal_st_2_ra ();                 /* state trans to redundant BA */

	/* <ATR:01> */
	bai_set_dsw (LC_DSW_TYPE_BUS_ADMIN_MSK, LC_DSW_TYPE_BUS_ADMIN_SET);
//	bai_set_dsw (LC_DSW_TYPE_MSK, LC_DSW_TYPE_BUS_ADMIN_SET);
	bai_set_dsw (LC_DSW_MAS_MSK,  LC_DSW_MAS_CLR);

	if ((bam_dta.state & BAM_CFGD) == BAM_CFGD)
	{
		bai_set_dsw (LC_DSW_ACT_MSK,  LC_DSW_ACT_SET);
		bai_set_dsw (LC_DSW_AX_MSK,   bam_dta.mtvc);

        rslt = bam_reset ();
        bai_hamster_cond (rslt, BAI_DU_ERR, BAI_DU_RESET, "unknown error");
	}
	else
	{
        bap_reset ();
		bai_set_dsw (LC_DSW_ACT_MSK,  LC_DSW_ACT_CLR);
		bai_set_dsw (LC_DSW_AX_MSK,   0);
	}

    bah_ekr_func (bai_hi_led_bm_pas_int_ext_di ());
}



/*  new page
 ----------------------------------------------------------------------------
|
|   name        bam_st_2_bm         state transition to bus master
|
 ------------   A B S T R A C T   -------------------------------------------

    State transition to bus master.

    Comment: This function may be modified to check for bus silence before
    taking over the bus mastership.

    The link layer state transition to bus master is called.
    The interrupt handler "accept bus mastership"  is disconnected from LC
    The interrupt handler "pass on bus mastership" is connected to LC

    The condition is announced in the device status word

*/

void            bam_st_2_bm         (unsigned short     state)

{
/*
unsigned short  s_chk_silence;

	s_chk_silence = (bam_dta.state & BAM_BM) == BAM_BM ? BAA_PASSIVE :
														 BAA_ACTIVE  ; */
	bam_dta.state =  BAM_BM | state;

    bal_st_2_bm (state);
/*
	if (bal_bm_go (s_chk_silence) != RET_OK)
	{
		bam_st_2_ra (state);
	}
	else
*/
	{
        bah_bap_func (bai_lc_int_disconnect (BAI_LC_INT_BMT));
        bah_bap_func (bai_lc_int_connect    (bat_pass_on_mastership, BAI_LC_INT_BMT,
						    		           LC_KERNEL, 0));
 		bai_set_dsw (LC_DSW_MAS_MSK,  LC_DSW_MAS_SET);

		if ((bam_dta.state & BAM_CFGD) == BAM_CFGD)
		{
			bai_set_dsw (LC_DSW_ACT_MSK,  LC_DSW_ACT_SET);
			bai_set_dsw (LC_DSW_AX_MSK,   bam_dta.mtvc);
		}
		else
		{
			bai_set_dsw (LC_DSW_ACT_MSK,  LC_DSW_ACT_CLR);
			bai_set_dsw (LC_DSW_AX_MSK,   0);
		}
        bah_ekr_func (bai_hi_led_bm_act_int_ext_di ());
	}
}




/*   new page
 ----------------------------------------------------------------------------
|
|    name        bam_st_trans_di_ext
|
 ------------   A B S T R A C T   -------------------------------------------

     State Transition Machine for all State changes.

     The caller must guarantee externally that this function will not be
     called a second time (not reeentrant, thus external disable interrupt)

     It might have been called by bam_st_trans() which disables the interrupt
     in a macro shell.



*/

void            bam_st_trans_di_ext     (unsigned short     state)
{

    bah_reset_silence_monitor ();        /* empty function for MVBC */

    switch (bam_dta.state)
    {
		case    0:

                bam_st_2_ra (BAM_NCFG);
        break;

        case    BAM_BM_CFGD:

                if      (state == BAM_RA)
                {
                    bam_st_2_ra (BAM_CFGD);
				}

        break;

        case    BAM_RA_CFGD:

                if      (state == BAM_BM)
                {
                    bam_st_2_bm (BAM_CFGD);
                }


        break;

        case    BAM_BM_NCFG:

                if      (state == BAM_RA)
                {
					bam_st_2_ra (BAM_NCFG);
                }

                else if (state == BAM_CFGD)
                {
					bam_st_2_ra (BAM_CFGD);
                }
        break;

        case    BAM_RA_NCFG:

                if      (state == BAM_BM)
                {
                    bam_st_2_bm (BAM_NCFG);
                }

                else if (state == BAM_CFGD)
                {
					bam_st_2_ra (BAM_CFGD);
                }
        break;

        default:
        break;
    }
}


/*
 -------------------------------------------------------------------------Ä
|
|   M O D I F I C A T I O N S       BAM_MAIN.C
|   - - - - - - - - - - - - -       ----------
|
 --------------------------------------------------------------------------


    V: ...   D: 9./../..   object: ........................................

    description of what was done to object

 --------------------------------------------------------------------------

*/
