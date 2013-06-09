/*  BAD_DIAG.C
 ----------------------------------------------------------------------------
|
|   Property of :   ABB Verkehrssysteme AG, CH-8050 ZÅrich / Switzerland
|   COPYRIGHT   :   (c) 1993 ABB Verkehrssysteme AG
|
 ----------------------------------------------------------------------------
|
|   Project     :   T C N   B U S   A D M I N I S T R A T O R
|
 ----------------------------------------------------------------------------
|
|   File Name   :   BAD_DIAG
|
|   Document    :
|
|   Abstract    :   MVB Scan
|
|   Remarks     :
|

|   Dependencies:
|
|   Accepted    :
|
 ----------------------------------------------------------------------------
|
|   HISTORY:
|
|   Version     :   1   |
|   Date (Week) :   313 |     |     |     |     |     |     |     |     |
|   Name        :   Ma  |
|
 ----------------------------------------------------------------------------
*/

#define      BAD_DIAG_C

#include    "bax_incl.h"

#include    "baa_auxi.h"
#include    "bad_diag.h"
#include    "bah_hwre.h"
#include    "bai_ifce.h"
#include    "bal_link.h"
#include    "bam_main.h"
#include    "bap_poll.h"

#if         defined (BX_SIMU)

#include    "bsd_diag.h"
#include    "bsh_hwre.h"
#include    "btd_diag.h"

#endif


/* new page
 ----------------------------------------------------------------------------
|
|   name        bad_pit_clr
|
 ------------   A B S T R A C T   -------------------------------------------

*/

void    bad_pit_clr         (void);


/* new page
 ----------------------------------------------------------------------------
|
|   name        bad_p_dta           static data access function
|
 ------------   A B S T R A C T   -------------------------------------------


    static data declaration and debug access function

*/

static  struct  BAD_STR_DTA bad_dta;


void            bad_open        (struct BAD_STR_DTA **  p_bad_dta)

{
                *p_bad_dta = &bad_dta;
}





/*  new page
 ----------------------------------------------------------------------------
|
|   name        bad_init
|
 ------------   A B S T R A C T   -------------------------------------------

    Full MVBC:
	In unconfigured mode, the scan range is limited to 1..255
                                         is mapped to the PCS 512 ... 767
    In configured   mode, the scan range is mapped to the PCS 1   ... 511
    (See bad_create)
    Reduced Traffic Store MVBC and BAP:
	In unconfigured mode, the scan range is limited to 1..255
                                         is mapped to the PCS 1   ... 255
    In configured   mode, the scan range is mapped to the PCS 128 ... 255
    (See bad_create) and dynamically reconfigured on-line

    The internal variables are initialised.
    The subscribable function is subscribed to a dummy function.
	The first device address to be scanned is 0000.

*/

void            bad_init            (void)

{
LC_TYPE_TS_PCS *    p_pcs;
int             i;
LC_TYPE_TS_PIT * p_pit;

	bai_memset (&bad_dta, 0, sizeof (bad_dta));

	bad_dta.id[0].p_func = bad_dummy;

    bad_dta.id[0].pt[0].c_ref_frame       = 0x0000FFFFL;
    bad_dta.id[0].pt[1].c_ref_frame       = 0x0000FFFFL;

    bad_dta.id[0].pt[0].p_scan_rslt_cl_1  = &bad_dta.id[0].scan_rslt[0];
    bad_dta.id[0].pt[1].p_scan_rslt_cl_1  = &bad_dta.id[0].scan_rslt[2];

    bad_dta.id[0].pt[0].p_scan_rslt_cl_2p = &bad_dta.id[0].scan_rslt[1];
    bad_dta.id[0].pt[1].p_scan_rslt_cl_2p = &bad_dta.id[0].scan_rslt[3];

    bad_dta.id[0].pt[0].p_poll_rsp        = &bad_dta.id[0].poll_stat[0];
    bad_dta.id[0].pt[1].p_poll_rsp        = &bad_dta.id[0].poll_stat[1];

    bad_dta.id[0].pt[0].c_poll_rsp        = 1;
    bad_dta.id[0].pt[1].c_poll_rsp        = 1;

    bad_dta.id[0].pt[0].c_scan_rslt_cl_1  = 1;
    bad_dta.id[0].pt[1].c_scan_rslt_cl_1  = 1;

    bad_dta.id[0].pt[0].c_scan_rslt_cl_2p = 1;
    bad_dta.id[0].pt[1].c_scan_rslt_cl_2p = 1;

    bad_dta.id[0].pt[0].s_poll_rsp        = sizeof (struct BA_STR_POLL_STAT);
    bad_dta.id[0].pt[0].s_scan_rslt_cl_1  = sizeof (struct BA_STR_SCAN_RESULT);
    bad_dta.id[0].pt[1].s_scan_rslt_cl_1  = sizeof (struct BA_STR_SCAN_RESULT);

    bad_dta.id[0].pt[0].s_scan_rslt_cl_2p = sizeof (struct BA_STR_SCAN_RESULT);
    bad_dta.id[0].pt[1].s_scan_rslt_cl_2p = sizeof (struct BA_STR_SCAN_RESULT);


#if defined (O_BAP) || (O_LC_MCM == 1) || (O_LC_MCM == 2)
	p_pcs = &bah_dta.ts_map.pb_dsw_pcs[0][1];
	p_pit =  bah_dta.ts_map.pb_dsw_pit;

	for (i = 1; i < 256; i++, p_pcs++)
	{
        lc_put_pit (p_pit, i, i);

        lc_put_pcs (p_pcs, LC_PCS_MSK_FCODE, LC_PCS_OFF_FCODE,
                                             LC_PCS_VAL_FCODE_15);
        lc_put_pcs (p_pcs, LC_PCS_MSK_TYPE , LC_PCS_OFF_TYPE ,
                                             LC_PCS_VAL_TYPE_SNK);
        lc_put_pcs_mvbc (p_pcs, LC_PCS_MSK_DIAG,  LC_PCS_OFF_DIAG ,
                                                  LC_PCS_VAL_DIAG_RDY);
        lc_put_pcs_mvbc (p_pcs, LC_PCS_MSK_NUM, LC_PCS_OFF_NUM,
                                                LC_PCS_VAL_NUM_SET);
	}
#else
	p_pcs = &bah_dta.ts_map.pb_dsw_pcs[0][1];

	for (i = 1; i < 768; i++, p_pcs++)
	{
        lc_put_pcs (p_pcs, LC_PCS_MSK_FCODE, LC_PCS_OFF_FCODE,
                                             LC_PCS_VAL_FCODE_15);
        lc_put_pcs (p_pcs, LC_PCS_MSK_TYPE , LC_PCS_OFF_TYPE ,
                                             LC_PCS_VAL_TYPE_SNK);
        lc_put_pcs (p_pcs, LC_PCS_MSK_DIAG , LC_PCS_OFF_DIAG ,
                                             LC_PCS_VAL_DIAG_RDY);
        lc_put_pcs_mvbc (p_pcs, LC_PCS_MSK_NUM, LC_PCS_OFF_NUM,
                                                LC_PCS_VAL_NUM_SET);
	}

	p_pit = bah_dta.ts_map.pb_dsw_pit;

	for (i = 0; i < 256; i++)                /*  class 3 address range */
	{
        lc_put_pit (p_pit, i, i + 512);
	}
#endif
    bah_bap_skrl_gap_set ();
}


/*  new page
 ----------------------------------------------------------------------------
|
|   name        bad_create
|
 ------------   A B S T R A C T   -------------------------------------------

    The device status word scan is prepared. Depending upon the configuration,m
    either the whole address range (0 ... 256 / 256 ... 4096 in steps of 16)
    is scanned, or only the devices configured by the application.

    The location look up table is the table containing all 512 prepared
    scan master frames for a scan cycle, the location in the DMA lists
    where these scan master frames should be copied to and also when
    to do this.

    The DSW port address table is initialised: The port indexes are written

    p_mf_1 allcoates the class 1 addresses to the PCS's   1...255
    p_mf_3 allocates the class 3 addresses to the PCS's 256...511

    The location look up table contains 512 master frames, which all will
    be transmitted once in a scan period.
    Strategy "Scan known devices"
    First, these 512 master frames are initialised to "Scan own device"
    as a kind of padding, then the known devices replace the padding
    till no known devices are left
    Strategy "Scan all devices"
    The first  256 master frames are initialised to scan the class 1 range
    The second 256 master frames are initialised to scan the class 3 range

*/

void            bad_create          (struct BAM_STR_CFG_PT *  p_pt)
{
unsigned short * p_mf;
unsigned short * p_mf_1;
unsigned short * p_mf_3;
unsigned short * p_kwn_dvc;
unsigned short i;
unsigned short mf;
unsigned short mf_pad = 0xF000 + p_pt->p_div->own_dvc_addr;

    bad_dta.llut.mf_padding = mf_pad;
    bad_dta.task.strategy = p_pt->p_div->sn_strategy;

    p_mf   = &bad_dta.llut.mf[  0];
    p_mf_1 = &bad_dta.llut.mf[  0];
    p_mf_3 = &bad_dta.llut.mf[256];

    if  (bad_dta.task.strategy == BA_SCAN_KWN_DVC)
    {
        p_kwn_dvc = &p_pt->p_kc3->kc3_mf[0];

        for (i = 0; i < 512; i++)
        {
            *p_mf++ = mf_pad;
        }

        for (i = p_pt->p_div->c_kc3; i > 0; i--)
        {
            mf = *(p_kwn_dvc) & 0xFFF;
            if  (mf < 256)
            {
                *(p_mf_3 + mf) = *p_kwn_dvc++;
            }
            else
            {
                *(p_mf_1 + (mf >> 4)) = *p_kwn_dvc++;
            }
        }
    }
    else
    {
        for (i = 0; i < 256; i++)         /* Prepare the scan master frames */
        {
            *p_mf_1++ = 0xF000 + (i << 4);
            *p_mf_3++ = 0xF000 + i;
        }
    }
#if !defined (O_BAP) && (O_LC_MCM != 1) && (O_LC_MCM != 2)
    {
    LC_TYPE_TS_PIT  * p_pit;

	    p_pit = bah_dta.ts_map.pb_dsw_pit;

	    for (i = 1; i < 256; i++)                 /*  class 1 address range */
	    {
            lc_put_pit (p_pit, i << 4, i);
	    }
	    for (i = 0; i < 256; i++)                 /*  class 3 address range */
	    {
            lc_put_pit (p_pit, i, i + 256);
	    }
    }
#endif
}


/*  new page
 ----------------------------------------------------------------------------
|
|   name        bad_pit_clr
|
 ------------   A B S T R A C T   -------------------------------------------

	This function deletes the last scan connections from the port index
	table. It is called by bad_task () after reading the results from
	the ports.

    The parameter step allows to differentiate between class 1 and class 3
    devices.
*/


void    bad_pit_clr         (void)
{
unsigned int    count = 64;
unsigned short  prt_addr = bad_dta.task.prt_ad;
unsigned short  step     = bad_dta.task.stp;

		while (count--  > 0)
		{
            lc_put_pit (bah_dta.ts_map.pb_dsw_pit, prt_addr, 0);
            prt_addr += step;
		}
}


/*  new page
 ----------------------------------------------------------------------------
|
|   name        bad_announce
|
 ------------   A B S T R A C T   -------------------------------------------

	Fields were reserved in the DMA input list (slot list) for the scan
	master frames. The position of these and the count are registered in
	the llut (location look up table). These fields are all filled with
	padding as only the first 64 will be used later for scanning.

*/

void    bad_announce        (unsigned short    *p_dest,
							 unsigned int       cnt)
{
unsigned *	p_ix;
struct BAD_STR_LLUT_ITEM * p_item;

		p_ix   = &bad_dta.llut.ix_cfgd;
		p_item = &bad_dta.llut.cfgd[*p_ix];

		(*p_ix)++;

		p_item->ptr = (void *) p_dest;
		p_item->cnt = cnt;


		while (cnt-- != 0)
		{
			*p_dest++ = bad_dta.llut.mf_padding;
		}
}

/*  new page
 ----------------------------------------------------------------------------
|
|   name        bad_reset
|
 ------------   A B S T R A C T   -------------------------------------------

    Is called upon the tranistion to configured master.
    Prepares the first two scan intervals (2 * (64 scans / 512 ms))

*/

short   bad_reset        (void)
{
		bad_dta.task.window = -1;
		bad_dta.task.prt_ad = 0;
		bad_dta.task.prt_ix = 128;
		bad_dta.task.stp    = 16;
		bad_invest ();

		bad_dta.task.window = 0;
		bad_dta.task.prt_ad = 1024;
		bad_dta.task.prt_ix = 192;
		bad_dta.task.stp    = 16;
		bad_invest ();

		return (RET_OK);
}


/*  new page
 ----------------------------------------------------------------------------
|
|   name        bad_c_error_frame_put
|
 ------------   A B S T R A C T   -------------------------------------------

    The error frame count is stored.

*/


void    bad_c_error_frame_put (unsigned short c_error_frame)
{

        bad_dta.id[0].c_error_frame = (unsigned long) c_error_frame;
}


/*  new page
 ----------------------------------------------------------------------------
|
|   name        bad_harvest
|
 ------------   A B S T R A C T   -------------------------------------------

    Every scan interval, this function is called to collect the scan result
    of 64 scans and store them in the result array.

    The scanned ports are removed from the PIT.
    The diagnostic bits in the PCS are marked "READY" for the next trans

*/
#if defined (O_BAP)
#define bad_bap_conv_p_dvc_rsp(p_rsp)                   \
        { if (*(p_rsp) != 1)                            \
          {                                             \
                 if (*(p_rsp) == 4) { *(p_rsp) = 0; }    \
            else if (*(p_rsp) == 0) { *(p_rsp) = 2; }    \
            else                    { *(p_rsp) = 3; }    \
          }                                             \
        }
#else
#define bad_bap_conv_p_dvc_rsp(p_rsp)
#endif


void            bad_harvest        (void)
{
LC_TYPE_TS_PCS *   p_pcs;
LC_TYPE_TS_DCK *   p_prt;
LC_TYPE_TS_DCK *   pb_prt;

int                             i, j;

struct  BA_STR_SCAN_RSLT_MBR *  p_dvc;

                           #if defined (O_BAP) || (O_LC_MCM == 1) || (O_LC_MCM == 2)
	bad_pit_clr ();

	p_pcs      = &bah_dta.ts_map.pb_dsw_pcs[0][bad_dta.task.prt_ix];
	pb_prt     = &bah_dta.ts_map.pb_dsw_prt[0][bad_dta.task.prt_ix/4][0][0];
                           #else
	p_pcs      = &bah_dta.ts_map.pb_dsw_pcs[0][bad_dta.task.ix_dvc];
	pb_prt     = &bah_dta.ts_map.pb_dsw_prt[0][bad_dta.task.ix_dvc/4][0][0];
                           #endif

	p_dvc = &bad_dta.task.pb_scan_rslt->sn_rslt[bad_dta.task.ix_dvc & 0xFF];

	for (i = 0; i < 16; i++, pb_prt = pb_prt + 4)
	{
		for (j = 0; j < 4; j++, p_pcs++, p_dvc++, pb_prt++)
		{
         int vp;
            lc_get_pcs (&vp, p_pcs, LC_PCS_MSK_VP  , LC_PCS_OFF_VP);
            lc_get_pcs (&p_dvc->rsp, p_pcs, LC_PCS_MSK_DIAG, LC_PCS_OFF_DIAG);

            bad_bap_conv_p_dvc_rsp (&p_dvc->rsp);

            lc_put_pcs (p_pcs, LC_PCS_MSK_DIAG, LC_PCS_OFF_DIAG,
                                                LC_PCS_VAL_DIAG_RDY);

            p_prt = vp == 0 ? pb_prt : pb_prt + 4;

			p_dvc->dsw = *p_prt[0];
#if defined (O_BAP) || (O_LC_MCM == 1) || (O_LC_MCM == 2)
			*p_prt[0]  = 0;
#endif
		}
	}
}

/*  new page
 ----------------------------------------------------------------------------
|
|   name        bad_invest
|
 ------------   A B S T R A C T   -------------------------------------------



*/


void	        bad_invest    (void)
{
struct  BAD_STR_LLUT_ITEM * p_item;
unsigned short *            p_mf;
int		step            = bad_dta.task.stp;
int     cnt, sum;
unsigned short *          p_dest;

#if defined (O_BAP) || (O_LC_MCM == 1) || (O_LC_MCM == 2)
unsigned short prt_ad = bad_dta.task.prt_ad;
unsigned short prt_ix = bad_dta.task.prt_ix;

        if (bad_dta.task.prt_ad == 256)
        {
            sum     = 16;
            cnt     = 64 - 16;
            prt_ix += 16;
        }
        else
        {
            sum = 0;
            cnt = 64;
        }

		while (cnt--  > 0)
		{
                lc_put_pit (bah_dta.ts_map.pb_dsw_pit, prt_ad, prt_ix);
				prt_ix++;
				prt_ad += step;
		}
#else
        sum = bad_dta.task.prt_ad == 256 ? 16 : 0;
#endif
		p_item = &bad_dta.llut.cfgd[0];

		if ((bad_dta.task.window & 1) == 0)
		{
			p_item = p_item + (bad_dta.llut.ix_cfgd >> 1);
		}

        if (step == 1)
        {
            p_mf = &bad_dta.llut.mf[256 + bad_dta.task.prt_ad];
        }
        else
        {
            p_mf = &bad_dta.llut.mf[bad_dta.task.prt_ad >> 4];
        }


		while (sum < 64)
		{
		    if ((cnt = p_item->cnt) != 0)
		    {
			    p_dest = (void *) p_item->ptr;

			    while (cnt-- != 0)
			    {
                    {
				        *p_dest++ = *p_mf++;
                    }
				    if (++sum >= 64)
		         	{
					    break;
				    }
			    }
		    }
		   	p_item++;
		}
}



/*  new page
 ----------------------------------------------------------------------------
|
|   name        bad_dummy
|
 ------------   A B S T R A C T   -------------------------------------------

	This dummy function is attached to the subscribable function to prevent a
    call to nivana if the diagnostic application does not exist.

*/

void            bad_dummy           (struct BA_STR_DIAGNOSIS * p_pt)

{
#if defined (BX_SIMU)
    bsh_printf_diag_bus (p_pt);
#endif
    p_pt = p_pt;
}



/*  new page
 ----------------------------------------------------------------------------
|
|   name        bad_diag            BA FIRMWARE INTERFACE FUNCTION
|
 ------------   A B S T R A C T   -------------------------------------------

	The diagnostic application subscribes a procedure which will be called
    each time the whole scan range has been covered.

    The location of the procedure is stored in the static data.

*/

void   bad_diag    (void (*p_func) (struct BA_STR_DIAGNOSIS * p_pt))
{

	bad_dta.id[0].p_func = p_func;

}

/*  new page
 ----------------------------------------------------------------------------
|
|   name        bad_dsw_read
|
 ------------   A B S T R A C T   -------------------------------------------

    Provides the last valid dsw and response type. The information might be
    up to 2 seconds old

*/

void            bad_dsw_read    (unsigned short                 dvc_nr,
                                 struct BA_STR_SCAN_RSLT_MBR *  p_dvc_trgt)

{
int vp;

    vp          = bad_dta.id[0].ivp == 0 ? 1 : 0;

    *p_dvc_trgt = dvc_nr < 256 ?
                  bad_dta.id[0].pt[vp].p_scan_rslt_cl_2p->sn_rslt[dvc_nr] :
                  bad_dta.id[0].pt[vp].p_scan_rslt_cl_1->sn_rslt[dvc_nr/16];

}


/*  new page
 ----------------------------------------------------------------------------
|
|   name        bad_task
|
 ------------   A B S T R A C T   -------------------------------------------

    The bad task is a cyclic task which is activated every 512 ms. It is
    synchronised with the scan, so that the last scan result can be read
    and the next scan prepared while the current scan is in progress.

*/

void            bad_task            (void)
{

	switch (bad_dta.task.window++ & 0x7)
	{

	case 0:
			bad_dta.task.ix_dvc = 0;
			bad_dta.task.prt_ix = 128;

			bad_dta.task.prt_ad = 0;
			bad_dta.task.stp    = 16;

            bad_dta.task.pb_scan_rslt =
            bad_dta.id[0].pt[bad_dta.id[0].ivp].p_scan_rslt_cl_1;

			bad_harvest ();

			bad_dta.task.prt_ad = 2048;
			bad_dta.task.stp    = 16;
			bad_invest ();

			break;

	case 1:
			bad_dta.task.ix_dvc = 64;
			bad_dta.task.prt_ix = 192;

			bad_dta.task.prt_ad = 1024;
			bad_dta.task.stp    = 16;
			bad_harvest ();

			bad_dta.task.prt_ad = 3072;
			bad_dta.task.stp    = 16;
			bad_invest ();

			break;

	case 2:
			bad_dta.task.ix_dvc = 128;
			bad_dta.task.prt_ix = 128;

			bad_dta.task.prt_ad = 2048;
			bad_dta.task.stp    = 16;
			bad_harvest ();

			bad_dta.task.prt_ad = 0;
			bad_dta.task.stp    = 1;
			bad_invest ();
			break;

	case 3:
			bad_dta.task.ix_dvc = 192;
			bad_dta.task.prt_ix = 192;

			bad_dta.task.prt_ad = 3072;
			bad_dta.task.stp    = 16;
			bad_harvest ();

			bad_dta.task.prt_ad = 64;
			bad_dta.task.stp    = 1;
			bad_invest ();

			break;

	case 4:
			bad_dta.task.ix_dvc = 256;
			bad_dta.task.prt_ix = 128;

			bad_dta.task.prt_ad = 0;
			bad_dta.task.stp    = 1;

            bad_dta.task.pb_scan_rslt =
            bad_dta.id[0].pt[bad_dta.id[0].ivp].p_scan_rslt_cl_2p;
			bad_harvest ();

			bad_dta.task.prt_ad = 128;
			bad_dta.task.stp    = 1;
			bad_invest  ();

			break;

	case 5:
			bad_dta.task.ix_dvc = 320;
			bad_dta.task.prt_ix = 192;

			bad_dta.task.prt_ad = 64;
			bad_dta.task.stp    = 1;
			bad_harvest ();

			bad_dta.task.prt_ad = 192;
			bad_dta.task.stp    = 1;
			bad_invest  ();

			break;

	case 6:
			bad_dta.task.ix_dvc = 384;
			bad_dta.task.prt_ix = 128;

			bad_dta.task.prt_ad = 128;
			bad_dta.task.stp    = 1;
			bad_harvest ();

			bad_dta.task.prt_ad = 256;
			bad_dta.task.stp    = 16;
			bad_invest  ();

			break;

	case 7:
			bad_dta.task.ix_dvc = 448;
			bad_dta.task.prt_ix = 192;

			bad_dta.task.prt_ad = 192;
			bad_dta.task.stp    = 1;
			bad_harvest ();

			bad_dta.task.prt_ad = 1024;
			bad_dta.task.stp    = 16;
			bad_invest ();

			if  (bad_dta.id[0].ivp == 0)
			{
				bad_dta.id[0].pt[0].c_scan_update =
				bad_dta.id[0].pt[1].c_scan_update + 1;
                bad_dta.id[0].pt[0].c_error_frame =
                bad_dta.id[0].c_error_frame;
				bad_dta.id[0].pt[0].p_scan_rslt_cl_1->sn_rslt[0].rsp = 3;
				bad_dta.id[0].pt[0].p_scan_rslt_cl_2p->sn_rslt[0].rsp = 3;
				bad_dta.id[0].ivp =  1;
                bal_pl_statistic (&bad_dta.id[0].pt[0].c_poll_rsp_total,
                                   bad_dta.id[0].pt[0].p_poll_rsp);
		     	(*(bad_dta.id[0].p_func)) (&bad_dta.id[0].pt[0]);
			}
			else
			{
				bad_dta.id[0].pt[1].c_scan_update =
				bad_dta.id[0].pt[0].c_scan_update + 1;
                bad_dta.id[0].pt[1].c_error_frame =
                bad_dta.id[0].c_error_frame;
				bad_dta.id[0].pt[1].p_scan_rslt_cl_1->sn_rslt[0].rsp = 3;
				bad_dta.id[0].pt[1].p_scan_rslt_cl_2p->sn_rslt[0].rsp = 3;
				bad_dta.id[0].ivp =  0;
                bal_pl_statistic (&bad_dta.id[0].pt[1].c_poll_rsp_total,
                                   bad_dta.id[0].pt[1].p_poll_rsp);
		     	(*(bad_dta.id[0].p_func)) (&bad_dta.id[0].pt[1]);
			}


			break;

	 default:

			break;
	}
}

/*  new page
 ----------------------------------------------------------------------------
|
|   name        bad_ncfg
|
 ------------   A B S T R A C T   -------------------------------------------

    scan for non-configured bus masters.
    The whole scan result is copied to the scan list
    If an active bus administrator is spotted, the function returns "Active"
    otherwise, the function returns "passive"

*/

#if defined  (O_BAP) || (O_LC_MCM == 1) || (O_LC_MCM == 2)

int     bad_ncfg   (void)
{
LC_TYPE_TS_PCS *   p_pcs;
LC_TYPE_TS_DCK *   p_prt;
LC_TYPE_TS_DCK *   pb_prt;
struct  BA_STR_SCAN_RSLT_MBR *  p_dvc;

int                             i, j;
int     result;

	p_pcs  = &bah_dta.ts_map.pb_dsw_pcs[0][0];
	pb_prt = &bah_dta.ts_map.pb_dsw_prt[0][0][0][0];

	p_dvc  = &bad_dta.id[0].pt[bad_dta.id[0].ivp].p_scan_rslt_cl_2p->sn_rslt[0];

	for (i = 0; i < 64; i++, pb_prt = pb_prt + 4)
	{
		for (j = 0; j < 4; j++, p_pcs++, pb_prt++, p_dvc++)
		{
         int vp;
            lc_get_pcs (&vp, p_pcs, LC_PCS_MSK_VP  , LC_PCS_OFF_VP);
            lc_get_pcs (&p_dvc->rsp, p_pcs, LC_PCS_MSK_DIAG, LC_PCS_OFF_DIAG);

            lc_put_pcs (p_pcs, LC_PCS_MSK_DIAG, LC_PCS_OFF_DIAG,
                                                LC_PCS_VAL_DIAG_RDY);

            p_prt = vp == 0 ? pb_prt : pb_prt + 4;

			p_dvc->dsw = *p_prt[0];

            if  (((p_dvc->dsw & 0x4200) == 0x4200) &&
                  (p_dvc->rsp == BA_RSP_OK))
            {
                result = BAA_ACTIVE;
            }
		}
	}
    bad_dta.id[0].ivp ^=  1;
    return (result);
}
#else

int     bad_ncfg            (void)
{

LC_TYPE_TS_PCS *        p_pcs;
LC_TYPE_TS_DCK *        p_prt;
LC_TYPE_TS_DCK *        pb_prt;

int                  i, j;

struct  BA_STR_SCAN_RSLT_MBR *  p_sn_rslt;
int     result = BAA_PASSIVE;

	p_pcs      = &bah_dta.ts_map.pb_dsw_pcs[0][512];
	pb_prt     = &bah_dta.ts_map.pb_dsw_prt[0][128][0][0];

	p_sn_rslt  = &bad_dta.id[0].pt[bad_dta.id[0].ivp].p_scan_rslt_cl_2p
               ->sn_rslt[0];

	for (i = 0; i < 64; i++, pb_prt = pb_prt + 4)
	{
		for (j = 0; j < 4; j++, p_pcs++, p_sn_rslt++, pb_prt++)
		{
         int vp;
            lc_get_pcs (&vp, p_pcs, LC_PCS_MSK_VP  , LC_PCS_OFF_VP);
            lc_get_pcs (&p_sn_rslt->rsp, p_pcs, LC_PCS_MSK_DIAG, LC_PCS_OFF_DIAG);

            lc_put_pcs (p_pcs, LC_PCS_MSK_DIAG, LC_PCS_OFF_DIAG,
                                                LC_PCS_VAL_DIAG_RDY);

            p_prt = vp == 0 ? pb_prt : pb_prt + 4;

			p_sn_rslt->dsw = *p_prt[0];

            if  (((p_sn_rslt->dsw & 0x4200) == 0x4200) &&
                  (p_sn_rslt->rsp == BA_RSP_OK))
            {
                result = BAA_ACTIVE;
            }
		}
	}
    bad_dta.id[0].ivp ^=  1;

    return (result);
}

#endif


/*
 ---------------------------------------------------------------------------ƒ
|
|   M O D I F I C A T I O N S       BAD_DIAG.C
|   - - - - - - - - - - - - -       ----------
|
 ----------------------------------------------------------------------------


	name: ......................... Nr: ....    date:   ..........

    description of what was done to object

 ----------------------------------------------------------------------------

*/
