/*=============================================================================================*/
/* MVBC line redundancy monitoring													           */
/* Implementation file (mvbc_red.c)													           */
/* 																					           */
/* Version 1.0																		           */
/*																					           */
/* (c)1998 Ansaldo Trasporti														           */
/* Written by Giuseppe Carnevale													           */
/*=============================================================================================*/


/*=============================================================================================*/
/* Includes */

#include "pi_sys.h"
#include "lc_sys.h"
#include "lp_sys.h"
#pragma WL(0)
#include "mvbc.h"
#pragma WL(3)
#include "lpc_blck.h"
#include "lpl_4tcn.h"

#include "atr_hw.h"
#include "c_utils.h"

#include "mvbc_red.h"


/*=============================================================================================*/
/* Constants */

#define REFRESH_TICK 64		/* TMO supervision refresh time in ms */


/*=============================================================================================*/
/* Globals */

static const MvbcRedConfHeader *cfg_head;	/* redundancy configuration header pointer         */
static const MvbcRedDevEntry   *cfg_dev;	/* device configuration array pointer              */

static short				timeout_id;		/* ID of the PIL timeout                           */
static void					**p_pcs;		/* array of PCS pointers for the datasets to check */

static char					line_ok[2];		/* if FALSE the corresponding line is disabled     */
static char					*dev_ok[2];		/* device ok flags array pointer for each line     */
static char					*old_dev_ok[2];	/* old device ok flags array pointer for each line */


/*=============================================================================================*/
/* Utilities */

/*---------------------------------------------------------------------------------------------*/
/* Get the MVBC line selection status                                                          */

unsigned char get_mvb_line(unsigned int bus_id)
{
	TM_TYPE_SERVICE_AREA	*sa;		/* MVBC service area pointer */
	unsigned short			dr;			/* MVBC Decoder Register     */

	/* get MVBC's service area address and DR register value */
	sa = lc_m_get_service_area_addr(bus_id);
	dr = sa->int_regs.dr.w;

	/* return the status */
	if (dr & TM_DR_SLM) return (dr & TM_DR_LAA) ? LC_MVB_LINE_CMD_SLA : LC_MVB_LINE_CMD_SLB;
	return LC_MVB_LINE_CMD_SLA | LC_MVB_LINE_CMD_SLB;
}


/*---------------------------------------------------------------------------------------------*/
/* Use this instead of lc_mvb_line(): the MVBC is not put in configuration mode.               */
/* This makes the switching slower, but potential problems are avoided.                        */

int set_mvb_line(unsigned int bus_id, unsigned char command)
{
	TM_TYPE_SERVICE_AREA	*sa;		/* MVBC service area pointer */

	/* get MVBC's service area address */
	sa = lc_m_get_service_area_addr(bus_id);

	/* if both lines are selected choose the automatic switching */
	if ((command & LC_MVB_LINE_CMD_SLA) && (command & LC_MVB_LINE_CMD_SLB)) sa->int_regs.dr.w = 0;
	else {

		/* find the LAA bit desired value */
		unsigned short ln = ((command & LC_MVB_LINE_CMD_SLA) ? TM_DR_LAA : 0);

		/* select the single line mode */
		sa->int_regs.dr.w = TM_DR_SLM;

		/* switch line until our request is satisfied */
 		while ((sa->int_regs.dr.w & TM_DR_LAA) != ln) {
  			sa->int_regs.dr.w = TM_DR_LS;
			while (sa->int_regs.dr.w & TM_DR_LS);
			sa->int_regs.dr.w = TM_DR_SLM;
		}
	}

	/* clear the frame and error counters if required */
	if (command & (LC_MVB_LINE_CMD_CLA | LC_MVB_LINE_CMD_CLB)) {
		pi_disable();
		sa->int_regs.fc = sa->int_regs.ec = 0;
		pi_enable();
	}

	/* our error checking is a bit relaxed... */
	return 0;
}


/*---------------------------------------------------------------------------------------------*/
/* This routine returns the TX and RX lines selected the for the MVB                           */

void get_mvb_line_selection(char *tx_line, char *rx_line)
{
	/* get the TX lines selection */
	if (tx_line) *tx_line = (get_mvb_connection(MVB_TS_ID, 0) ? LC_MVB_LINE_CMD_SLA : 0) |
							(get_mvb_connection(MVB_TS_ID, 1) ? LC_MVB_LINE_CMD_SLB : 0);

	/* get the RX lines selection */
	if (rx_line) *rx_line = get_mvb_line(MVB_TS_ID);
}


/*---------------------------------------------------------------------------------------------*/
/* This routine selects the TX and RX lines for the MVB                                        */

void select_mvb_line(char tx_line, char rx_line)
{
	/* set the MVB connection (TX/RX) */
	set_mvb_connection(MVB_TS_ID, 0, (tx_line & LC_MVB_LINE_CMD_SLA) ? 1 : 0);
	set_mvb_connection(MVB_TS_ID, 1, (tx_line & LC_MVB_LINE_CMD_SLB) ? 1 : 0);

	/* set the RX line(s) */
	set_mvb_line(MVB_TS_ID, rx_line);	
}


/*=============================================================================================*/
/* Implementation */

/*---------------------------------------------------------------------------------------------*/
/* Returns TRUE if A is equal to B */

static char dev_ok_equal(char *a, char *b)
{
	int i;
	
	for (i = 0; i < cfg_head->n_dev; i++) if (a[i] != b[i]) return FALSE;
	return TRUE;
}


/*---------------------------------------------------------------------------------------------*/
/* Returns TRUE if A includes B */

static char dev_ok_inclusion(char *a, char *b)
{
	int i;

	if (dev_ok_equal(a, b)) return FALSE;

	for (i = 0; i < cfg_head->n_dev; i++) if (!a[i] && b[i]) return FALSE;
	return TRUE;
}


/*---------------------------------------------------------------------------------------------*/
static void mvbc_red_timeout(void)
{
	static char		the_line = 0;				/* line currently active (0 = A, 1 = B)    */
	static char		single_line_flag = FALSE;	/* if TRUE we are looking at one line only */

	int				i;							/* generic index                           */
	unsigned int	temp;						/* temporary storage                       */
	unsigned short	refresh;					/* refresh counter value                   */

	/* copy the device ok flags to the old storage */
	pi_copy8(old_dev_ok[the_line], dev_ok[the_line], cfg_head->n_dev * sizeof(dev_ok[0][0]));

	/* update the device ok flags for the current line */
	for (i = 0; i < cfg_head->n_dev; i++) {

		/* look at the specified dataset if any, otherwise assume it's ok */
		if (p_pcs[i]) {

			/* get the refresh counter for the device fixing it */
			lc_get_pcs(&temp, p_pcs[i], LC_PCS_MSK_TACK, LC_PCS_OFF_TACK);
			refresh = ~(unsigned short)temp;
			if (refresh <= REFRESH_TICK) refresh = 0;
//printf("%d:%u ", the_line, refresh);
			/* check for refresh overflow */
			if (refresh < cfg_head->timeout) dev_ok[the_line][i] = TRUE;
			else                             dev_ok[the_line][i] = FALSE;
		}
		else dev_ok[the_line][i] = TRUE;
	}
//putchar('\n');
	if (single_line_flag) {
		/* exit single line mode if there is a change in the ok devices */
		if (!dev_ok_equal(dev_ok[the_line], old_dev_ok[the_line])) {

			/* reset to line A with switching enabled */
			the_line = 0;
			single_line_flag = FALSE;
			
			/* reset the status results */
			line_ok[0] = line_ok[1] = TRUE;
			pi_zero8(dev_ok[0], sizeof(dev_ok[0][0]) * cfg_head->n_dev * 4);
		}
	}
	else {

		/* if we are looking at line B it's time to check what we have seen */
		if (the_line == 1) {

			/* we proceed only if we are sure of what we have seen */
			if (dev_ok_equal(dev_ok[0], old_dev_ok[0]) && dev_ok_equal(dev_ok[1], old_dev_ok[1])) {

				/* if we see more on line A, B is broken */
				if (dev_ok_inclusion(dev_ok[0], dev_ok[1])) {

					/* force the usage of line A only */
					the_line = 0;
					single_line_flag = TRUE;

					/* signal that line B is not working */
					line_ok[1] = FALSE;
				}

				/* if we see more on line B, A is broken */
				if (dev_ok_inclusion(dev_ok[1], dev_ok[0])) {

					/* force the usage of line B only */
					the_line = 1;
					single_line_flag = TRUE;

					/* signal that line A is not working */
					line_ok[0] = FALSE;
				}
			}

		}

		/* switch line if we are not in single line mode */
		if (!single_line_flag) {
			if (the_line == 0) the_line = 1;
			else               the_line = 0;
		}
	}

	/* look at the wanted line */
	select_mvb_line(single_line_flag ?
						(the_line == 0 ? LC_MVB_LINE_CMD_SLA : LC_MVB_LINE_CMD_SLB) :
						LC_MVB_LINE_CMD_SLA | LC_MVB_LINE_CMD_SLB,
					the_line == 0 ? LC_MVB_LINE_CMD_SLA : LC_MVB_LINE_CMD_SLB);

	/* reschedule our execution */
	pi_enable_timeout(timeout_id, (cfg_head->timeout + 2*REFRESH_TICK) / PI_TICK);
}



/*=============================================================================================*/
/* Implementation */

/*---------------------------------------------------------------------------------------------*/
short mvbc_red_init(const MvbcRedConfHeader *cfg)
{
	short			err = 0;	/* error code                                  */
	int				i;			/* generic index                               */
	void			*p_cb;		/* pointer to the control block for the MVB TS */
	LC_TYPE_TS_PIT	*pb_pit;	/* MVBC PIT pointer                            */
	unsigned short  prt_indx;	/* port index                                  */

	/* go ahead only if a configuration is provided */
	if (!cfg) return 0;

	/* save the configuration pointers */
	cfg_head = cfg;
	cfg_dev  = (const MvbcRedDevEntry *)(cfg + 1);

	/* allocate the memory for the PCS array */
	p_pcs = (void**)pi_alloc(cfg_head->n_dev * sizeof(*p_pcs));
	if (!p_pcs) Check(-1);

	/* get the CB and PCS pointers */
    Check(lpc_open(MVB_TS_ID, &p_cb));
	pb_pit = lpc_get_pb_pit(p_cb);
	for (i = 0; i < cfg_head->n_dev; i++) {
		if ((cfg_dev[i].mf & 0xF000) == 0xF000) p_pcs[i] = 0;
		else {
		    lpl_get_prt_indx_via_pvn(p_cb, &prt_indx, pb_pit, (cfg_dev[i].mf & 0x0FFF));
    	    p_pcs[i] = ((unsigned char *)lpc_get_pb_pcs(p_cb)) + lpl_gen_pcs_offset(p_cb, prt_indx);
		}
	}

	/* allocate the memory for the device status results */
	dev_ok[0] = pi_alloc(sizeof(dev_ok[0][0]) * cfg_head->n_dev * 4);
	if (!dev_ok[0]) Check(-1);
	dev_ok[1]     = dev_ok[0] + cfg_head->n_dev;
	old_dev_ok[0] = dev_ok[0] + cfg_head->n_dev * 2;
	old_dev_ok[1] = dev_ok[0] + cfg_head->n_dev * 3;

	/* initialize the status results */
	line_ok[0] = line_ok[1] = TRUE;
	pi_zero8(dev_ok[0], sizeof(dev_ok[0][0]) * cfg_head->n_dev * 4);

	/* initialize the MVB connection */
	select_mvb_line(LC_MVB_LINE_CMD_SLA | LC_MVB_LINE_CMD_SLB, LC_MVB_LINE_CMD_SLA);

	/* create the timeout handler and schedule its first execution */
	Check(pi_create_timeout(&timeout_id, mvbc_red_timeout, 0, 1));
	Check(pi_enable_timeout(timeout_id, (cfg->timeout + 2*REFRESH_TICK) / PI_TICK));

error:
	return err;
}


/*---------------------------------------------------------------------------------------------*/
short mvbc_red_status(MvbcRedStatus *status)
{
	int i, j;
	
	/* proceed only if we have been initialized */
	if (!cfg_head) return -1;

	/* clear the status report */
	pi_zero8((void*)status, sizeof(*status));

	/* fill the line status (we don't use a semaphore because this data is not critical) */
	for (i = 0; i < 2; i++) {
		status->line_ok[i] = line_ok[i];
		for (j = 0; j < cfg_head->n_dev; j++) if (dev_ok[i][j]) {
			if (cfg_dev[j].mvb_address >= 256)
				c_set_bit(status->cl_1[i], cfg_dev[j].mvb_address / 16);
			else c_set_bit(status->cl_2p[i], cfg_dev[j].mvb_address);
		}
	}

	return 0;
}


/*=============================================================================================*/

