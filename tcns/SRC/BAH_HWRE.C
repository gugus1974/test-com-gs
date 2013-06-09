/*
 ------------------------------------------------------------------------------
|
|    Property of  :   ABB Verkehrssysteme AG, CH-8050 ZÅrich / Switzerland
|    COPYRIGHT    :   (c) 1994 ABB Verkehrssysteme AG
|
 ------------------------------------------------------------------------------
|
|    Project      :   T C N   B U S   A D M I N I S T R A T O R
|
 ------------------------------------------------------------------------------
|
|    File Name    :   BAH_HWRE
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
|    Vers  Dept.   YY-MM-DD  State    Name    Reference
|    ----  ------  --------  -------  ------- ---------
|      0   BAEI-2  93-01-19  created  Marsden
|
 ------------------------------------------------------------------------------
*/

#define 	BAH_HWRE_C

#include    "lc_sys.h"

#include	"baa_auxi.h"
#include	"bah_hwre.h"
#include	"bah_ebac.h"
#include	"bai_ifce.h"
#include	"bal_link.h"
#include	"bax_incl.h"

#if 		defined (BX_SIMU)

#include	"bsh_hwre.h"

#endif


/* new page
 -----------------------------------------------------------------------------
|
|   name        bah_dta             global data
|
 ------------   A B S T R A C T   --------------------------------------------


	The BAH data is global, as many macro access routines need direct
	access to the internal bah data.

    This global data is only accessed by bah_ macros.

*/

struct BAH_STR_DTA  bah_dta;


/* 	new page
 ----------------------------------------------------------------------------
|
| 	name   		bah_init
|
 ------------   A B S T R A C T   -----------------------------------------------


	The internal variables are initialised.
	The simulation is initialised.
    The traffic store areas belonging to BA are initialised.
    The traffic store mapping is initialised.

*/

void			bah_init       		(void)

{
    bah_ekr_func (bah_ekr2_waitstates());

    bai_memset (&bah_dta, 0, sizeof (bah_dta));

    bai_hi_set_up_watchdog (TC_HW_WDT_EKR_209_7MS);

    bah_dta.ts_map.pb_dma_ncfg= BAH_PB_TS_DMA_NCFG;
    bah_dta.ts_map.pb_dma_cfgd= BAH_PB_TS_DMA_CFGD;
	bah_dta.ts_map.pb_dsw_pit = BAH_PB_TS_DSW_PIT;
	bah_dta.ts_map.pb_dsw_pcs = BAH_PB_TS_DSW_PCS;
	bah_dta.ts_map.pb_dsw_prt = BAH_PB_TS_DSW_PRT;

#if defined (O_BAP)
    bah_dta.ts_map.pb_bmt_src_pcs = BAH_PB_TS_BMT_PCS;
    bah_dta.ts_map.pb_bmt_src_prt = BAH_PB_TS_BMT_PRT;
#else
    bah_dta.ts_map.pb_ireg    = BAH_PB_TS_IREG;
    bah_dta.ts_map.pb_svc     = BAH_PB_TS_SVCE;
#endif

    bai_memset (bah_dta.ts_map.pb_dsw_pit,  0, sizeof (LC_TYPE_TS_PIT));
    bai_memset (bah_dta.ts_map.pb_dsw_pcs,  0, sizeof (LC_TYPE_TS_DA_PCS));
    bai_memset (bah_dta.ts_map.pb_dsw_prt,  0, sizeof (LC_TYPE_TS_DA_PRT));
    bai_memset (bah_dta.ts_map.pb_dma_cfgd, 0, sizeof (struct BAH_STR_TS_DMA_CFGD));
    bai_memset (bah_dta.ts_map.pb_dma_ncfg, 0, sizeof (struct BAH_STR_TS_DMA_NCFG));

    bah_dta.div.no_second_poll = BAA_ACTIVE;
}

/* 	new page
 ----------------------------------------------------------------------------
|
| 	name   		bah_init_no_second_poll
|
 ------------   A B S T R A C T   -----------------------------------------------
*/

void            bah_init_no_second_poll    (void)

{
    bah_dta.div.no_second_poll = BAA_ACTIVE;

}

/* 	new page
 ----------------------------------------------------------------------------
|
| 	name   		bah_tmr_init
|
| 	retval 		-
|
 ------------   A B S T R A C T   -----------------------------------------------

    The timing is done by two timers. Timer 1 triggers the DMA upon "zero"
    and reloads with "c_max". Timer 2 is used to detect a timer 1 overflow
    and is thus loaded with an offset of "4000 us". It is synchronised with
    timer 1.

*/

void bah_tmr_init (unsigned short c_max,
                   unsigned short t_guard)
#if defined (O_BAP)
{
    bah_tmr_w_c_max (c_max);
    bah_tmr_w_c     (BAH_TMR_0010US);
    bah_dta.div.t_guard =  c_max - t_guard;
    bah_dta.div.c_tmr2  = c_max;
}
#else
{
    bah_tmr_w_c_max (c_max);
    bah_tmr_w_c     (BAH_TMR_0020US);
	bah_dta.div.t_guard = t_guard;
    {
        unsigned short c_tmr2;
        unsigned short c_tmr2_max;

        c_tmr2_max = (80 * c_max)   + BAH_TMR2_4000US ;
        c_tmr2     = (80 * BAH_TMR_0020US) + c_tmr2_max ;

        bah_tmr2_w_c     (c_tmr2);
        bah_tmr2_w_c_max (c_tmr2_max);

        bah_dta.div.c_tmr2 = c_tmr2_max;
    }
}
#endif


/* 	new page
 ----------------------------------------------------------------------------
|
| 	name      	bah_check_t_guard
|
| 	ret val  	short result		RET_OK
|
 ------------   A B S T R A C T   -------------------------------------------

	The time remaining in the slot is determined and compared with the guard
	time. If within guard time, an error is returned.
    Timer Wrap around detection:
    This is done with a second, parallel timer with an offset. If the time
    is within the offset zone, then a timer wrap around is detected.
    BAC Timer Wrap Around Detection:
	The ISR's (interrupt in service bit of CPU) are checked that slot end
    was not passed (timer 1 interrupt pending means new DMA already active)

*/
#if defined (O_BAP)

short    bah_check_slot 			(void)
{
      bah_cpu_isr_r_tmr1 (&bah_dta.ebac.isr_tmr1);

      if  ((bah_dta.ebac.isr_tmr1 & BAH_CPU_ISR_BIT_TMR1) == BAH_CPU_ISR_BIT_TMR1)
      {
         return (BA_E);
      }
      else { return (RET_OK); }
}

#endif
short    bah_check_t_guard			(void)
{
  signed short	result;
unsigned short  c_tmr;

                                        #if !defined (O_BAP)
    if (bah_dta.div.no_second_poll == BAA_ACTIVE)
    {
		result = BA_E;
    }
    else
    {
        result = RET_OK;
        bah_tmr2_r_c (&c_tmr);

        if  (c_tmr < bah_dta.div.t_guard)
        {
		    result = BA_E;
	    }
    }
                                        #else
    {
      result = RET_OK;
      bah_tmr2_r_c (&c_tmr);

      if  (c_tmr > bah_dta.div.t_guard)
      {
        result = BA_E;
      }

      bah_cpu_isr_r_tmr1 (&bah_dta.ebac.isr_tmr1);

      if  ((bah_dta.ebac.isr_tmr1 & BAH_CPU_ISR_BIT_TMR1) == BAH_CPU_ISR_BIT_TMR1)
      {
         result = BA_E;
      }
    }
                                        #endif
	return (result);
}

short   bah_check_window 			(void)
{
unsigned short  c_tmr;

	bah_tmr2_r_c (&c_tmr);

    return (c_tmr < BAH_TMR2_BEYOND ? BA_E : RET_OK);
}



/* 	new page
 ----------------------------------------------------------------------------
|
| 	name      	bah_bap_rcve_sf
|
| 	(out)		p_state				BA_RSP_OK / RSP_MISSING / RSP_ERROR
| 	(out)		p_sf				16-bit slave frame
|
| 	ret val  	void
|
 ------------   A B S T R A C T   -------------------------------------------

	The status bits are polled until the Address Frame register is empty.

	The status bits are polled until either
		-	missing   dt
		-   valid     dt
		-   erroneous dt
	is indicated. If a valid dt is indicated, the dt is read from the
	receive register and returned to the caller.

	Dependency: It would be a good idea to reset the DT state bits in the
	BAC before sending the Master Frame.

*/

#if     defined (O_BAP) && !defined (BX_SIMU)

void 			bah_bap_rcve_sf			(struct BAL_STR_RSP *	p_sf)
{
union BAA_UNN_DTA	mf;
union BAA_UNN_DTA   sf;
union BAA_UNN_DTA	temp;
union BAA_UNN_DTA	utmp;
unsigned char *		p_state = &utmp.uc[0];

	do
	{
		bah_bac_reg_r40 (p_state);
	}   while ((utmp.uc[0] & BAH_BAC_R40_EM_MASK) == 0);

	while ((utmp.uc[0] & BAH_BAC_R40_DT_MASK) == 0)
	{
		bah_bac_reg_r40 (p_state);
	}

	temp.uc[0] = utmp.uc[0] & BAH_BAC_R40_DT_MASK;
	if	    (temp.uc[0] == BAH_BAC_R40_DT_MISSING)
	{
		p_sf->state = BA_RSP_MISSING;
	}
	else if	(temp.uc[0] == BAH_BAC_R40_DT_VAL)
	{
		bah_bap_reg_r00 (&mf.uc[0]);
		bah_bap_reg_r02 (&mf.uc[1]);
		bah_bap_reg_r04 (&sf.uc[0]);
		bah_bap_reg_r06 (&sf.uc[1]);

        p_sf->r_sf = sf.us[0];
        p_sf->state = BA_RSP_OK;
	}
	else
	{
		p_sf->state = BA_RSP_ERROR;
	}
}

#endif

/* 	new page
 ----------------------------------------------------------------------------
|
| 	name      	bah_bap_rcve_sf
|
 ------------   A B S T R A C T   -------------------------------------------

    The diagnostic bits in the PCS have been initialised to an invalid
    value. The routine pends upon these bits with a timeout for the
    termination of the transmission for this port.

    The transmission state values "DT_..." are used as index in the
    POLL transition table!

*/

#if !defined (O_BAP)
void           bah_aip_dummy    (void);

short		   bah_rcve_sf	    (struct BAL_STR_RSP *	p_q,
                                 unsigned int           ix_pp)
{
LC_TYPE_TS_PCS * p_pcs = (LC_TYPE_TS_PCS *) &bah_dta.ts_map.pb_svc->pp_pcs[ix_pp];
unsigned int   diag;
short          result = RET_OK;
int            i = 0;

    lc_get_pcs (&diag, p_pcs, LC_PCS_MSK_DIAG, LC_PCS_OFF_DIAG);

    while ((diag == LC_PCS_VAL_DIAG_RDY) && (i++ < 50))
    {
        lc_get_pcs (&diag, p_pcs, LC_PCS_MSK_DIAG, LC_PCS_OFF_DIAG);
        #if defined (O_AIP) || defined (O_VCH)
        bah_aip_dummy ();
        #endif
    }

    switch (diag)
    {
    case 0:    /* BNI = 0;  TERR = 0;  STO = 0 */
		p_q->state = BA_RSP_OK;
        bah_mvbc_sf_get (&p_q->r_sf, ix_pp);

    break;

    case 1:    /* BNI = 0;  TERR = 0;  STO = 1 */
		p_q->state = BA_RSP_MISSING;
        p_q->r_sf = 0;
    break;

    case 2:    /* BNI = 0;  TERR = 1;  STO = 0 */
    case 4:    /* BNI = 1;  TERR = 0;  STO = 0 */
    case 5:    /* BNI = 1;  TERR = 0;  STO = 1 */
    case 6:    /* BNI = 1;  TERR = 1;  STO = 0 */
		p_q->state = BA_RSP_ERROR;
        p_q->r_sf  = 0;
    break;

    default:   /* Not possible : 3 / 7                */
		  p_q->state = BA_RSP_MISSING;
          p_q->r_sf = 0;
          result = BA_E;
    break;
    }

    lc_put_pcs (p_pcs, LC_PCS_MSK_DIAG, LC_PCS_OFF_DIAG, LC_PCS_VAL_DIAG_RDY);

    return (result);

}

void    bah_aip_dummy (void)
{
}

/* 	new page
 ----------------------------------------------------------------------------
|
| 	name      	bah_tmr2_wdog_trigger
|
 ------------   A B S T R A C T   -------------------------------------------

    The timer 2 is reloaded with a value synchronised with timer 1. Under
    normal operating conditions, timer 2 will not reach "zero" for an
    automatic hardware reload.
    If timer 2 drops below 4000 us + 100 us, it has reached the forbidden
    zone and will be pulverised by the intergallactic WÑchter using a plasma
    type of transducer (standard silicon version).
*/

void            bah_tmr1_interrupt (void)
{
    bah_tmr_w_c  (1);
    bah_tmr2_w_c (bah_dta.div.c_tmr2);
}


void			bah_tmr2_wdog_trigger	(void)
{
unsigned short  c_tmr2;
unsigned short  c_tmr1;
/*
    bah_tmr2_r_c (&c_tmr2);

    if (c_tmr2 > BAH_TMR_GUARD)
    {
*/
        bah_tmr_r_c (&c_tmr1);
        c_tmr2 = (c_tmr1 * 80)  + bah_dta.div.c_tmr2;
        bah_tmr2_w_c (c_tmr2);
#if (0)
    }
    else
    {
        c_tmr2 += /* bah_dta.div.c_tmr2 */ BAH_TMR2_2000US ;
        bah_tmr2_w_c (c_tmr2);
    }
#endif
}

/*
 ---------------------------------------------------------------------------ƒ
|
|   M O D I F I C A T I O N S		BAH_HWRE.C
|   - - - - - - - - - - - - -       ----------
|
 ----------------------------------------------------------------------------


    name of object modified		Nr:	?  	dated:	9?-??-??

    description of what was done to object

 ----------------------------------------------------------------------------

*/

#endif



