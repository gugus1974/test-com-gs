/*==============================================================================================*/
/* TCN human interface                                                                          */
/* Implementation file (tcn_mon.c) 													            */
/* 																					            */
/* Version 4.1																		            */
/*																					            */
/* (c)1996 Ansaldo Trasporti														            */
/* Written  by Giuseppe Carnevale													            */
/*==============================================================================================*/


/*==============================================================================================*/
/* Pragmas */

#ifdef __C166__
#pragma WL(0)
#pragma HL(Near 0)
#endif


/*==============================================================================================*/
/* Includes */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "pi_sys.h"
#include "atr_hw.h"
#include "bax_incl.h"
#include "md_layer.h"
#include "sio.h"
#include "mvbc.h"
#include "lp_sys.h"
#include "apd_incl.h"
#include "am_sys.h"
#include "conf_tcn.h"
#include "atr_nma.h"
#include "xmodem.h"
#include "mvbc_red.h"
#include "tcn_util.h"

#include "tcn_mon.h"
#include "ushell.h"
#include "c_utils.h"

#ifdef ATR_WTB
#include "cpmdrv.h"
#include "wtbcpm.h"
#include "pdm_lcfg.h"
#include "pdm_main.h"
#include "mac.h"
#endif

#include "lib_ver.h"

/*==============================================================================================*/
/* MVBC definitions */

#define func static

#define uint(x) (*(unsigned short*)(unsigned long)(x))

typedef unsigned long ulong;

#include "mvbc.inc"

/*----------------------------------------------------------------------------------------------*/
#define	DR   *((volatile short *)(TM0_BASE + 0x0FF88UL))
#define	SCR  *((volatile short *)(TM0_BASE + 0x0FF80UL))
#define	MR   *((volatile short *)(TM0_BASE + 0x0FFA0UL))
#define	MR2  *((volatile short *)(TM0_BASE + 0x0FFA4UL))
#define	MFS  *((volatile short *)(TM0_BASE + 0x0FF00UL))
#define	FC   *((volatile short *)(TM0_BASE + 0x0FF90UL))
#define	EC   *((volatile short *)(TM0_BASE + 0x0FF94UL))
#define	IPR0 *((volatile short *)(TM0_BASE + 0x0FFB0UL))
#define	IPR1 *((volatile short *)(TM0_BASE + 0x0FFB4UL))
#define	IMR0 *((volatile short *)(TM0_BASE + 0x0FFB8UL))
#define	IMR1 *((volatile short *)(TM0_BASE + 0x0FFBCUL))
#define	ISR0 *((volatile short *)(TM0_BASE + 0x0FFC0UL))
#define	ISR1 *((volatile short *)(TM0_BASE + 0x0FFC4UL))
#define	IVR0 *((volatile short *)(TM0_BASE + 0x0FFC8UL))
#define	IVR1 *((volatile short *)(TM0_BASE + 0x0FFCCUL))
#define	TR2  *((volatile short *)(TM0_BASE + 0x0FFF4UL))
#define	TC2  *((volatile short *)(TM0_BASE + 0x0FFFCUL))

#define	MSNK  ((volatile short *)(TM0_BASE + 0x0FE60UL))


/*==============================================================================================*/
/* Constants */

/* Max TCN message len */
#define MAX_MSG_LEN 1024


/*==============================================================================================*/
/* Global variables */

static char buf[2048];			/* generic buffer */
static unsigned int mac_view_enable;

/* messenger trace globals */
#ifdef TRACE_ENB
extern char nm_trace_enable;
#endif
extern unsigned short c_amfx_panic;				/* counter of MVB Bus Administrator panics         */
extern unsigned short emf_panic_count;			/* counter of BA stops triggered by too many EMFs  */
extern unsigned short emf_bm_count;				/* counter of EMFs in BM state                     */
extern unsigned short emf_slv_count;			/* counter of EMFs in other states                 */
extern unsigned short bti_deadlock_count;		/* counter of deadlocks discovered by BTIs         */
extern unsigned short bti_bm_count;				/* counter of BTIs in BM state                     */
extern unsigned short bti_slv_count;			/* counter of BTIs in other states                 */
extern unsigned short fev_count;				/* counter of FEVs                                 */
extern unsigned short dti6_bm_count;			/* counter of DTI6s in BM state                    */
extern unsigned short dti6_slv_count;			/* counter of DTI6s in other states                */
extern unsigned short tmr2_ra_silence_count;	/* counter of TMR2s in RA with silence on the line */
extern unsigned short tmr2_ra_noise_count;		/* counter of TMR2s in RA with noise on the line   */
extern unsigned short tmr2_deadlock_count;		/* counter of deadlocks discovered by TMR2s        */
extern unsigned short tmr2_panic_count;			/* counter of BTIs panics                          */

extern unsigned char  mac_trace[1024] ;
extern unsigned short  mac_trace_ndx  ;
extern unsigned char  mac_trigger[8] ;

extern  unsigned char zas_count;        /* Lifesign of ZAS (Counter)            */

/*==============================================================================================*/
/* Version commands */

/*----------------------------------------------------------------------------------------------*/
extern const char application_version[];
extern const char library_version[];
const char library_version[]     = LIB_VER;
static short version_dump(short argc, char *argv[])
{
	const NcTcnConf	*obj;		/* ptr to the object  */
	unsigned short	obj_size;	/* size of the object */
	char			c;			/* generic char       */

	if (argc != 1) return -1;

	if (application_version[0])	printf("*** Application : %s\n", application_version[0] != ' ' ?
										application_version : application_version + 1);
	if (library_version[0])     printf("*** Library     : %s\n", library_version);

	printf("*** TCN NCDB    : ");

	if (nc_get_db_element(NC_TCN_CONF_OBJ_ID, (void*)&obj, &obj_size) || obj_size != sizeof(*obj) ||
		obj->version_id.signature_1 == 0) printf("<error>\n");
	else {
		if (obj->vehicle_desc[41-1] || obj->vehicle_desc[01-1] || obj->vehicle_desc[42-1] ||
			obj->vehicle_desc[43-1] || obj->vehicle_desc[44-1] || obj->vehicle_desc[45-1])
			printf("%02X%02X%02X%02X%02X%02X ",
				obj->vehicle_desc[41-1], obj->vehicle_desc[01-1], obj->vehicle_desc[42-1],
				obj->vehicle_desc[43-1], obj->vehicle_desc[44-1], obj->vehicle_desc[45-1]);
		if (obj->version_id.signature_1) {
 			if (c = obj->version_id.signature_1 >> 24) putchar(c);
 			if (c = obj->version_id.signature_1 >> 16) putchar(c);
 			if (c = obj->version_id.signature_1 >>  8) putchar(c);
 			if (c = obj->version_id.signature_1 >>  0) putchar(c);
  			putchar(' ');
   		}
		if (obj->version_id.signature_2) {
 			if (c = obj->version_id.signature_2 >> 24) putchar(c);
 			if (c = obj->version_id.signature_2 >> 16) putchar(c);
 			if (c = obj->version_id.signature_2 >>  8) putchar(c);
 			if (c = obj->version_id.signature_2 >>  0) putchar(c);
  			putchar(' ');
   		}
		printf("%02X.%02X", obj->version_id.ver_1, obj->version_id.ver_2);
		if (obj->version_id.ver_3) printf(".%04X", obj->version_id.ver_3);
		putchar (' ');
		if (obj->version_id.day || obj->version_id.month || obj->version_id.year)
			printf("%02X/%02X/%02X", obj->version_id.day,   obj->version_id.month, obj->version_id.year);
		putchar('\n');
	}

	return 0;
}


/*==============================================================================================*/
/* Dump routines */

/*----------------------------------------------------------------------------------------------*/
/* Element ID decoding */

#define digit1(id, p) (int)((id >> ((7-p)*4)) & 0x000F)
#define digit2(id, p) (int)((id >> ((6-p)*4)) & 0x00FF)
#define digit3(id, p) (int)((id >> ((5-p)*4)) & 0x0FFF)
#define digit4(id, p) (int)((id >> ((4-p)*4)) & 0xFFFF)
#define digit5(id, p) ((id >> ((3-p)*4)) & 0x000FFFFF)
#define digit6(id, p) ((id >> ((2-p)*4)) & 0x00FFFFFF)
#define digit7(id, p) ((id >> ((1-p)*4)) & 0x0FFFFFFF)

static char *decode_eid(NcEleID id, char *s)
{
	switch (digit1(id,0)) {
		case 0xF:
		case 0xE:
			sprintf(s, "%07lX", id & 0x0FFFFFFF);
  			break;
		case 0x0:
			if (digit1(id,6) == 0xF)
  				sprintf(s, digit1(id,7) ? "%05lX/%X" : "%05lX", digit5(id,1), digit1(id,7));
			else
  				sprintf(s, digit1(id,6) == 1 ? "%02X.%03X%s/1%X" :
  				           digit1(id,7) ? "%02X.%03X%s/%X" : "%02X.%03X%s",
   	   					digit2(id,1), digit3(id,3),
   	   					digit1(id,6) == 0x0 ? ""  :
   	   					digit1(id,6) == 0xA ? "A" :
   	   					digit1(id,6) == 0xB ? "R" :
   	   					digit1(id,6) == 0xE ? "E" :
   	   					digit1(id,6) == 0x1 ? ""  :
   	   					digit1(id,6) == 0x2 ? "b" :
   	   					digit1(id,6) == 0x3 ? "t" : "?",
		   	   			digit1(id,7));
 			break;
   		default:
			sprintf(s, "%08lX", id);
	}
	return s;
}


/*----------------------------------------------------------------------------------------------*/
/* MVB process variable dump */

static short dump_mvb_pv(short ds_idx, short pv_idx, char out_ds_header)
{
	short				err = 0;			/* error code */
	const MvbDsEntry	*mvb_ds_p;
	const PvEntry		*mvb_pv_p;
	const AppDsEntry	*app_ds_p;
	const AppPvEntry	*app_pv_p;
   	struct STR_PV_NAME	pvn;
	unsigned char		var[32];			/* variable storage */
	unsigned short		refresh;			/* refresh variable */
	short				size;				/* var size */
	char				s[16];

	/* find the process variable */
	Check(nc_get_idx_mvb_pv(ds_idx, pv_idx >= 0 ? pv_idx : 0, &mvb_ds_p, &mvb_pv_p, &app_ds_p,
							&app_pv_p, &pvn));

	/* get the variable from the traffic store */
	Check(ap_get_variable(pvn, &var, &refresh));

	/* output the dataset header if required */
	if (out_ds_header)
		printf("+++ Dataset %d %-16s (id=%s, rf=%u)\n", mvb_ds_p->mf &0x0FFF,
 			   (app_ds_p && app_ds_p->txt_name) ? app_ds_p->txt_name : "<?>",
 			   decode_eid(mvb_ds_p->eid, s), refresh);

	/* exit if no process variable is required */
	if (pv_idx < 0) return 0;

	/* output the data */
	printf(" %-9s %-20s = 0x", decode_eid(mvb_pv_p->eid, s),
		   (app_pv_p && app_pv_p->txt_name) ? app_pv_p->txt_name : "<?>");
	size = lp_pv_size(&pvn);
	if (size != LPS_ERROR) {
#ifdef O_LE
		for (size--; size >= 0; size--) printf("%02x", (short)var[size]);
#else
		short i;
		for (i = 0; i < size; i++) printf("%02x", (short)var[i]);
#endif
	}
	else printf("??");
	putchar('\n');

error:
	return err;
}


/*----------------------------------------------------------------------------------------------*/
/* WTB process variable dump */

#ifdef ATR_WTB

static short dump_wtb_pv(short ds_idx, short pv_idx, char out_ds_header)
{
	short							err = 0;		/* error code                                */
	Type_Topography					t;				/* currentr train topography                 */
	struct STR_APD_PV_DESCRIPTOR	pvd;			/* process variaable descriptor              */
   	struct STR_PV_NAME				pvn;			/* process variable name in 32 bits          */
	unsigned char					ftf[2];			/* dataset's FTF                             */
	char							c;				/* generic check variable                    */
	unsigned short					refresh;		/* dataset's refresh counter                 */
	const WtbDsEntry				*wtb_ds_p;		/* WTB dataset in the configuration          */
	const PvEntry					*wtb_pv_p;		/* WTB process variable in the configuration */
	int								i;				/* generic index                             */
	short							size;			/* var size                                  */
	unsigned char					var[128];		/* variable storage                          */
	char							s[16];			/* placeholder for generic string            */

	/* get the current train topography (if any) */
	Check(ls_t_GetTopography(&t));

	/* check that the selected dataset is in the topography */
//	if (ds_idx > 32 && (t.bottom_address <= 32 || ds_idx < t.bottom_address) ||
//		ds_idx <= 32 && ds_idx > t.top_address) Check(-1);

	/* fix the node address if own */
	if (ds_idx == t.node_address) ds_idx = 0;

	/* get the FTF of the required dataset */
	pvd.pv_name.ts_id      = WTB_TS_ID;
	pvd.pv_name.prt_addr   = ds_idx * 4;
	pvd.pv_name.size       = 0;
	pvd.pv_name.bit_offset = 0;
	pvd.pv_name.type       = 0x0F;
 	pvd.pv_name.ctl_offset = 0x3FF;
	pvd.p_variable         = &ftf;
	pvd.p_validity         = &c;
	Check(apd_get_variable(&pvd, &refresh));

	/* output the dataset header if required */
	if (out_ds_header)
		printf("+++ Dataset %02d%c (ftf=0x%02X%02X, rf=%u)\n",
			   ds_idx ? ds_idx : t.node_address, ds_idx ? ' ' : '*',
			   ftf[0], ftf[1], refresh);

	/* exit if no process variable is required */
	if (pv_idx < 0) return 0;

	/* find the corresponding dataset (if any) */
	for (i = 0; ; i++) {
		if (nc_get_idx_wtb_pv(i, 0, &wtb_ds_p, 0, 0)) {
			wtb_ds_p = 0;
			break;
		}
		if (wtb_ds_p->ftf == *(unsigned short*)ftf) break;
	}

	/* find the pv */
	if (wtb_ds_p)
		Check(nc_get_idx_wtb_pv(i, pv_idx, &wtb_ds_p, &wtb_pv_p, (TYPE_PV_NAME_48 *)&pvd.pv_name));
	else {
		if (pv_idx > 0) Check(-1);
		pvd.pv_name.size = 63;
	}

	/* get  the variable */
	pvd.pv_name.prt_addr   = ds_idx * 4;
	pvd.pv_name.ctl_offset = 0x3FF;
	pvd.p_variable         = &var;
	Check(apd_get_variable(&pvd, &refresh));

	/* output the data */
	printf(" %-9s = 0x", wtb_ds_p ? decode_eid(wtb_pv_p->eid, s) : "?");
	conv_pvn48_pvn32((TYPE_PV_NAME_48*)&pvd.pv_name, &pvn);
	size = lp_pv_size(&pvn);
	if (size != LPS_ERROR) {
#ifdef O_LE
		for (size--; size >= 0; size--) printf("%02x", (short)var[size]);
#else
		for (i = 0; i < size; i++) printf("%02x", (short)var[i]);
#endif
	}
	else printf("??");
	putchar('\n');

error:
	return err;
}

#endif


/*==============================================================================================*/
/* TCN confirmation functions */

/*----------------------------------------------------------------------------------------------*/
static void call_conf(unsigned char caller_fct, char *ext_ref, const AM_ADDRESS *replier,
					  char *in_msg_addr, CARDINAL32 in_msg_size, AM_RESULT status)
{
	caller_fct = caller_fct;
	ext_ref = ext_ref;
	replier = replier;

	if (status) printf("\ncall_conf: status=%d\n", (short)status);
	else {
		printf("\nTransfer succesful. Reply:\n");
		dump_buffer(FALSE, (void*)in_msg_addr, in_msg_size);
	}

    if (in_msg_addr) am_buffer_free(in_msg_addr, in_msg_size);
	pi_post_semaphore(ushell_sema, 0);
}


/*----------------------------------------------------------------------------------------------*/
static void rcv_conf(unsigned char replier_fct, const AM_ADDRESS *caller, char *in_msg_addr,
					      CARDINAL32 in_msg_size, char *ext_ref)
{
    am_reply_requ(replier_fct, 0, 0, ext_ref, AM_OK);
    printf("\n<MESSAGE> to function %d from vehicle %d, function %d, station %d, topography %d:\n",
    	   replier_fct, caller->vehicle, caller->fct_or_sta, caller->next_sta, caller->tc);
    if (!ext_ref) dump_buffer(FALSE, (void*)in_msg_addr, in_msg_size);
    else {
		short i;
		printf(":: ");
 		for (i = 0; i < in_msg_size; i++) printf("%X ", in_msg_addr[i]);
		printf("::\n");
    }
    am_buffer_free(in_msg_addr, in_msg_size);
}


/*----------------------------------------------------------------------------------------------*/
static void reply_conf(unsigned char replier_fct, char *ext_ref)
{
    AM_RESULT		status;			/* am error code */
    am_rcv_requ(replier_fct, 0, MAX_MSG_LEN, ext_ref, &status);
}


/*==============================================================================================*/
/* MVBC status commands */

/*----------------------------------------------------------------------------------------------*/
static short mvbc_dump(short argc, char *argv[])
{
	if (argc == 1) {
		mvbc(0);
 		return 0;
  	}
	return -1;
}


/*----------------------------------------------------------------------------------------------*/
static short pp_dump(short argc, char *argv[])
{
	short	i;

	if (argc != 2) return -1;
	i = parse_number(argv[1]);
	mvbc_pp(i);
	return 0;
}


/*----------------------------------------------------------------------------------------------*/
static short da_dump(short argc, char *argv[])
{
	short	i;

	if (argc != 2) return -1;
	i = parse_number(argv[1]);
	mvbc_da(i);
	return 0;
}


/*----------------------------------------------------------------------------------------------*/
static short la_dump(short argc, char *argv[])
{
	short	i;

	if (argc != 2) return -1;
	i = parse_number(argv[1]);
	mvbc_la(i);
	return 0;
}


/*----------------------------------------------------------------------------------------------*/
#include "bai_ifce.h"
void bal_mvbc_amfx(unsigned short not_used);

#define MF_INTERCEPT_SIZE     300
#define MF_INTERCEPT_NEW_LINE 0x5000
#define MF_INTERCEPT_END      0x6000

static short          mf_intercept_count;
static unsigned short mf_intercept_mask;	/* if != 0 it is the mask to be intercepted, otherwise dump */
static unsigned short mf_intercept_value;	/* value to be intercepted */
static unsigned short mf_intercept_table[MF_INTERCEPT_SIZE];

static void amfx_intercept(unsigned short not_used)
{
	ulong _x;
	short _i, _j, _k;

	debug_pin_off();

	bal_mvbc_amfx(not_used);

	_x = TM0_BASE + ((ulong)get_mvbc_reg(0xf84, 5, 2) << 18) + ((ulong)get_mvbc_reg(0xfac, 0, 16) << 2);
	_j = get_mvbc_reg(0xfa4, 0, 5);

	if (mf_intercept_mask) {
		for (_i = 0; _i < _j; _i++) {
 			_k = uint(_x + _i*2);
   			if ((_k & mf_intercept_mask) == mf_intercept_value) debug_pin_on();
   		}
	}
	else if (mf_intercept_count >= 0) {
 		if (mf_intercept_count + _j >= MF_INTERCEPT_SIZE - 1) {
  			mf_intercept_table[mf_intercept_count++] = MF_INTERCEPT_END;
   			mf_intercept_count = -mf_intercept_count;
  		}
   		else {
   			for (_i = 0; _i < _j; _i++)
   	 			mf_intercept_table[mf_intercept_count++] = uint(_x + _i*2);
			mf_intercept_table[mf_intercept_count++] = MF_INTERCEPT_NEW_LINE;
   	   	}
 	}
}

static short mf_dump(short argc, char *argv[])
{
	char  c;
	short i;

	if (argc == 1) mf_intercept_mask = 0;
	else if (argc == 3) {
		mf_intercept_mask = parse_number(argv[1]);
 		mf_intercept_value = parse_number(argv[2]);
	}
	else return -1;

	mf_intercept_count = 0;
	bai_mvbc_lc_int_disconnect(LC_INT_AMFX);
    bai_mvbc_lc_int_connect (amfx_intercept, LC_INT_AMFX, LC_KERNEL, 0);
    do c = sio_poll_key(1); while (!c && mf_intercept_count >= 0);
   	bai_mvbc_lc_int_disconnect(LC_INT_AMFX);
    bai_mvbc_lc_int_connect (bal_mvbc_amfx, LC_INT_AMFX, LC_KERNEL, 0);
    if (mf_intercept_count < 0) for (i = 0;; i++) {
    	if (mf_intercept_table[i] == MF_INTERCEPT_NEW_LINE) printf("\n");
        else if (mf_intercept_table[i] == MF_INTERCEPT_END) break;
       	else printf("%04x ", mf_intercept_table[i]);
 	}

	debug_pin_off();
	return 0;
}


/*----------------------------------------------------------------------------------------------*/
static short mas_cmd(short argc, char *argv[])
{
	if (argc > 2) return -1;

	if (argc == 1) printf("Master frame enable flag value: %d\n", (SCR & TM_SCR_MAS) != 0);
	else {
		if (parse_number(argv[1])) SCR |= TM_SCR_MAS;
 		else SCR &= ~TM_SCR_MAS;
 	}

	return 0;
}


/*----------------------------------------------------------------------------------------------*/
#define IX_TACK	1
#define IX_CRC  2
#define IX_SQE  4
#define IX_ALO  8
#define IX_BNI  16
#define IX_TERR 32
#define IX_STO  64
#define IX_SFC  128
#define IX_EMF  256
#define IX_ESF  512
#define IX_RTI  1024
#define MAX_IX  2048

#define DFLT_ISR0_DBG_MASK (TM_I0_EMF | TM_I0_ESF | TM_I0_RTI)
#define DFLT_PCS1_DBG_MASK (TM_PCS1_CRC | TM_PCS1_SQE | TM_PCS1_ALO | TM_PCS1_BNI | \
							TM_PCS1_TERR | TM_PCS1_STO)

static short bt_cmd(short argc, char *argv[])
{
	static unsigned short mf = 0xF001;		/* master frame to use for the test              */
	static unsigned short count = 10;		/* number of master frames to send               */
	static unsigned short isr0_dbg_mask = DFLT_ISR0_DBG_MASK;
	static unsigned short pcs1_dbg_mask = DFLT_PCS1_DBG_MASK;

	unsigned short	isr0, isr1;				/* interrupt flags                               */
	unsigned short	msnk1, msnk2;
	unsigned short	dr, new_dr;				/* decoder register save and new values          */
	unsigned short	i, j, k;				/* generic indexes                               */
	char			f;						/* generic flag                                  */

	unsigned short  save_msnk[4];			/* storage for the MSNK port PCS                 */
	unsigned short  save_scr;				/* storage for the SCR                           */
	unsigned short  save_imr0, save_imr1;	/* storage for IMR0, IMR1                        */

	char            tx_line, rx_line;		/* MVB line selction storage                     */
	char			mvb_line = LC_MVB_LINE_CMD_SLA | LC_MVB_LINE_CMD_SLB; /* line to test    */

	unsigned short	dmf_counter = 0;	/* duplicated master frame counter                   */
	unsigned short	dsf_counter = 0;	/* duplicated slave frame counter                    */
	unsigned short	ec_counter = 0;		/* error counter                                     */
	unsigned long	rld_counter = 0;	/* redundant line disturbed 0->1 transitions counter */
	unsigned long	ls_counter = 0;		/* line switch counter                               */

	static unsigned short ix[MAX_IX];	/* storage for the results                           */

	/* parse the arguments */
	i = 1;
	if (--argc) {

		if (argv[1][0] == 'A' || argv[1][0] == 'a') {
			mvb_line = LC_MVB_LINE_CMD_SLA;
			argc--; i++;
		}
		else if (argv[1][0] == 'B' || argv[1][0] == 'b') {
			mvb_line = LC_MVB_LINE_CMD_SLB;
			argc--; i++;
		}

		if (argc) {
			mf = parse_number(argv[i++]);
			if (--argc) {
				count = parse_number(argv[i++]);

				if (--argc) {

					if (*argv[i] == '*') isr0_dbg_mask = DFLT_ISR0_DBG_MASK;
					else isr0_dbg_mask = parse_number(argv[i]);
					i++;

					if (--argc) {
						if (*argv[i] == '*') pcs1_dbg_mask = DFLT_PCS1_DBG_MASK;
						else pcs1_dbg_mask = parse_number(argv[i]);
						i++;

						if (--argc) return -1;
					}
				}
			}
		}
	}

/*
	if (argc > 5) return -1;
	if (argc >= 2) mf = parse_number(argv[1]);
	if (argc >= 3) count = parse_number(argv[2]);
	if (argc >= 4) {
		if (*argv[3] == '*') isr0_dbg_mask = DFLT_ISR0_DBG_MASK;
		else isr0_dbg_mask = parse_number(argv[3]);
	}
	if (argc >= 5) {
		if (*argv[4] == '*') pcs1_dbg_mask = DFLT_PCS1_DBG_MASK;
		else pcs1_dbg_mask = parse_number(argv[4]);
	}
*/

	/* clear the results */
	pi_zero8((char*)ix, sizeof(ix));

	pi_disable();							/* disable the bus administrator */

	/* save what we are going to change */
	save_scr = SCR;
	save_imr0 = IMR0;
	save_imr1 = IMR1;
	pi_copy16((void*)save_msnk, (void*)MSNK, 4);

	/* disable the bus administrator */
	SCR &= ~TM_SCR_MAS;						/* clear the master bit         */
	MR   = TM_MR_CSMF;						/* cancel sending master frames */
	while (MR & TM_MR_BUSY);				/* wait for BUSY clear          */

	/* wait for things to calm down (just to be sure) */
	for (i = 0; i < 65535; i++);

	/* enable what we need for our test */
	SCR |= TM_SCR_UTS;						/* use test sink port           */
	IMR0 = IMR1 = (short)0xFFFF;			/* enable all the interrupts    */

	MFS = mf;								/* set the MF                   */
	SCR |= TM_SCR_MAS;						/* set the master bit           */

	/* initialize the test PCS */
	MSNK[0] = (mf & TM_PCS_FCODE_MSK) |		/* F-code                       */
			  TM_PCS0_SINK |		   		/* sink port                    */
   			  TM_PCS0_TWCS |		   		/* transfer with check sequence */
           	  TM_PCS0_WA;			   		/* write always                 */
	MSNK[3] = 0;					   		/* check sequences = 0          */

	/* clear any pending interrupt */
	isr0 = ISR0; isr1 = ISR1;				/* read the interrupts          */
	ISR0 = ISR1 = 0;						/* clear the flags              */
	IVR0 = IVR1 = 0;						/* release the ISRi             */

	/* clear the error and frame counters */
	EC = FC = 0;

	/* load the decoder register value */
	dr = DR;

	/* get the MVB line selection and choose ours */
	get_mvb_line_selection(&tx_line, &rx_line);
	select_mvb_line(LC_MVB_LINE_CMD_SLA | LC_MVB_LINE_CMD_SLB, mvb_line);
	for (i = 0; i < 10000; i++);

	for (i = 0; i < count; i ++) {

		/* service the CPU watchdog (we are running with interrupts disabled) */
		hw_watchdog_service();

		/* send out the master frame if the low part is nonzero */
 		if (mf & 0x0FFF) {
	 		MR = TM_MR_SMFM;					/* set SMFM                */
			while (MR & TM_MR_SMFM);			/* wait for SMFM clear     */
  		}

		/* reset the PCS status */
		MSNK[1] = 0;					   		/* status = 0              */
		MSNK[2] = 0;							/* TACK = 0                */

		/* start the watchdog timer */
 		TC2 = TR2;

		/* wait for a frame */
		f = FALSE;
		do {

			/* get the MVBC status */
	 		isr0 = ISR0; isr1 = ISR1;			/* read the interrupts     */
  			msnk1 = MSNK[1]; msnk2 = MSNK[2];	/* read the PCS            */

			/* toggle the debug pin on error */
   	   		if ((isr0 & isr0_dbg_mask) || (msnk1 & pcs1_dbg_mask)) {
				debug_pin_on();
				debug_pin_off();
  			}

   	 		/* release the interrupt registers */
	  		ISR0 = ISR1 = 0;					/* clear the flags         */
	  		IVR0 = IVR1 = 0;					/* release the ISRi        */

			/* check for redundancy errors */
			new_dr = DR;
			if ((dr & 8) != (new_dr & 8)) ls_counter++;
			if (new_dr & 4) rld_counter++;
			DR = 0;
			dr = new_dr;

			/* check for duplicated frames */
			if (isr0 & TM_I0_DMF) dmf_counter++;
			if (isr0 & TM_I0_DSF) dsf_counter++;

			/* check for receive frame condition */
			if ((isr0 & (TM_I0_SFC | TM_I0_EMF | TM_I0_ESF | TM_I0_RTI | TM_I0_BTI)) ||
  				(isr1 & TM_I1_TI2)) f = TRUE;

		} while (!f);

		/* save the status */
  		j = 0;
   		if (isr0 & TM_I0_SFC)       j |= IX_SFC;
   		if (isr0 & TM_I0_EMF)       j |= IX_EMF;
   		if (isr0 & TM_I0_ESF)       j |= IX_ESF;
   		if (isr0 & TM_I0_RTI)       j |= IX_RTI;
   		if (j) {
  	   		if (msnk2)                  j |= IX_TACK;
	   		if (msnk1 & TM_PCS1_CRC)    j |= IX_CRC;
   			if (msnk1 & TM_PCS1_SQE)    j |= IX_SQE;
   			if (msnk1 & TM_PCS1_ALO)    j |= IX_ALO;
   			if (msnk1 & TM_PCS1_BNI)    j |= IX_BNI;
   			if (msnk1 & TM_PCS1_TERR)   j |= IX_TERR;
   			if (msnk1 & TM_PCS1_STO)    j |= IX_STO;
  			ix[j]++;
   		}

  		/* update the total count of ECs */
   		ec_counter += EC;
   		EC = 0;

		/* workaround for MCU deadlock */
		if (MR & TM_MR_BUSY) {
			SCR = (SCR & ~TM_SCR_IL_RUNNING) | TM_SCR_IL_CONFIG;
			SCR |= TM_SCR_IL_RUNNING;
		}
 	}

	/* restore what we have changed */
	pi_copy16((void*)MSNK, (void*)save_msnk, 4);
	SCR = save_scr;
	IMR0 = save_imr0;
	IMR1 = save_imr1;
	IPR0 = IPR1 = 0;
	IVR0 = IVR1 = 0;						/* release the ISRi                */

	/* restore the MVB line selection */
	select_mvb_line(tx_line, rx_line);

	pi_enable();							/* re-enable the bus administrator */

	/* print the report */
	printf("\n");
	printf("Lines selected         : %s\n",
		mvb_line == (LC_MVB_LINE_CMD_SLA | LC_MVB_LINE_CMD_SLB) ? "A+B" :
		mvb_line == LC_MVB_LINE_CMD_SLA                         ? "A"   : "B");
	printf("Master frame           : 0x%04x\n", mf);
	printf("Count                  : %u\n", count);
	printf("ISR0 debug signal mask : 0x%04x\n", isr0_dbg_mask);
	printf("PCS1 debug signal mask : 0x%04x\n", pcs1_dbg_mask);
	printf("\n");
	printf("SFC EMF ESF RTI  CRC SQE ALO BNI TERR STO  TACK  count  ISR0 PCS1\n");
	printf("--- --- --- ---  --- --- --- --- ---- ---  ----  -----  ---- ----\n");
	for (i = 1; i < MAX_IX; i++) {
		if (ix[i]) {
			j = k = 0;

			if (i & IX_SFC)  {printf("SFC ");  k |= TM_I0_SFC;}    else printf("    ");
			if (i & IX_EMF)  {printf("EMF ");  k |= TM_I0_EMF;}    else printf("    ");
			if (i & IX_ESF)  {printf("ESF ");  k |= TM_I0_ESF;}    else printf("    ");
			if (i & IX_RTI)  {printf("RTI ");  k |= TM_I0_RTI;}    else printf("    ");
			printf(" ");

			if (i & IX_CRC)  {printf("CRC ");  j |= TM_PCS1_CRC;}  else printf("    ");
			if (i & IX_SQE)  {printf("SQE ");  j |= TM_PCS1_SQE;}  else printf("    ");
			if (i & IX_ALO)  {printf("ALO ");  j |= TM_PCS1_ALO;}  else printf("    ");
			if (i & IX_BNI)  {printf("BNI ");  j |= TM_PCS1_BNI;}  else printf("    ");
			if (i & IX_TERR) {printf("TERR "); j |= TM_PCS1_TERR;} else printf("     ");
			if (i & IX_STO)  {printf("STO ");  j |= TM_PCS1_STO;}  else printf("    ");
			printf(" ");

			if (i & IX_TACK)  printf("TACK ");                     else printf("     ");
			printf(" ");

			printf("%5u  %04x %04x\n", ix[i], k, j);
  		}
	}
	printf("\n");
	printf("Error counter            = %u\n", ec_counter);
	printf("Duplicated master frames = %u\n", dmf_counter);
	printf("Duplicated slave frames  = %u\n", dsf_counter);
	printf("Trusted line switches    = %lu\n", ls_counter);
	printf("Redundant line disturbed = %lu\n", rld_counter);

	return 0;
}


/*==============================================================================================*/
/* Process variables commands */

/*----------------------------------------------------------------------------------------------*/
static short ds_dump(short argc, char *argv[])
{
	short				err;			/* error code */
	short				pa;				/* port address */
	short				ds_idx = 0;		/* dataset index */
	short				pv_idx;			/* process variable index */
	const MvbDsEntry	*mvb_ds_p;

	if (argc > 2) return -1;
	if (argc == 2) {
		pa = parse_number(argv[1]);
		while (1) {
			err = nc_get_idx_mvb_pv(ds_idx, 0, &mvb_ds_p, 0, 0, 0, 0);
			if (err) break;
  			if ((mvb_ds_p->mf & 0x0FFF) == pa) {
   				for (pv_idx = 0; !err; pv_idx++) err = dump_mvb_pv(ds_idx, pv_idx, pv_idx == 0);
				break;
			}
			ds_idx++;
		}
 	}
	else do {
		err = dump_mvb_pv(ds_idx, -1, 1);
		ds_idx++;
	} while (!err);

	return 0;
}


/*----------------------------------------------------------------------------------------------*/
static short pv_dump(short argc, char *argv[])
{
	short			ds_idx;		/* dataset index */
	short			pv_idx;		/* process variable index */
	short			i = -1;		/* index */
	char			s[16];		/* decoded process variable id */
	PvEntry			*mvb_pv_p;	/* generic MVB process variable entry in the configuration */

	if (argc != 2) return -1;

   	ds_idx = 0;
	do {
		for (pv_idx = 0; !nc_get_idx_mvb_pv(ds_idx, pv_idx, 0, &mvb_pv_p, 0, 0, 0); pv_idx++) {
			decode_eid(mvb_pv_p->eid, s);
			if (!strcmp(s, argv[1])) {
				dump_mvb_pv(ds_idx, pv_idx, ds_idx != i);
	  			i = ds_idx;
			}
		}
		ds_idx++;
	} while (pv_idx != 0);

	return 0;
}


/*----------------------------------------------------------------------------------------------*/
static short pv_force(short argc, char *argv[])
{
	short				ds_idx;			/* dataset index */
	short				pv_idx;			/* process variable index */
	short				i = -1, j = 0;	/* indexes */
	char				s[16];			/* decoded process variable id */
	PvEntry				*mvb_pv_p;		/* generic MVB process variable entry in the configuration */
	struct STR_PV_NAME 	pvn;			/* process variable name */
	int					count = 0;		/* number of pv found */

	if (argc > 3) return -1;

	if (argc == 1) {
		lp_unfrc_all(0);
	    printf("All variables unforced.\n");
     	return 0;
	}

   	ds_idx = 0;
	do {
		for (pv_idx = 0; !nc_get_idx_mvb_pv(ds_idx, pv_idx, 0, &mvb_pv_p, 0, 0, 0); pv_idx++) {
			decode_eid(mvb_pv_p->eid, s);
			if (!strcmp(s, argv[1])) {
				dump_mvb_pv(ds_idx, pv_idx, ds_idx != i);
	  			i = ds_idx;
   	 			j = pv_idx;
   	 			count++;
			}
		}
		ds_idx++;
	} while (pv_idx != 0);

	if (count == 1) {
		pi_zero8(buf, 32);
		if (argc == 3) {
	 		parse_buffer(argv[2], (void*)buf, 1);
   			nc_get_idx_mvb_pv(i, j, 0, 0, 0, 0, &pvn);
			ap_frc_variable(pvn, buf);
		    printf("Variable forced.\n");
        }
       	else {
       		ap_unfrc_variable(pvn);
		    printf("Variable unforced.\n");
       	}
	}
	else if (count == 0) printf("Variable not found!\n");
	else printf("More than 1 variable found!\n");

	return 0;
}


/*----------------------------------------------------------------------------------------------*/
static short pv_force_ds(short argc, char *argv[])
{
	short				ds_idx;		/* dataset index */
	short				pv_idx;		/* process variable index */
	static char			s[32];		/* temporary string */
	struct STR_PV_NAME 	pvn;		/* process variable name */
	short				pa;			/* port address */
	const MvbDsEntry	*mvb_ds_p;  /* generic MVB dataset entry in the configuration */
	const PvEntry		*mvb_pv_p;	/* generic MVB process variable entry in the configuration */
	const AppPvEntry	*app_pv_p;	/* generic application pv entry */

	if (argc < 2 || argc > 3) return -1;

	pa = parse_number(argv[1]);
	for (ds_idx = 0; !nc_get_idx_mvb_ds(ds_idx, &mvb_ds_p, 0); ds_idx++) {
		if ((mvb_ds_p->mf & 0x0FFF) == pa) {
			for (pv_idx = 0; !nc_get_idx_mvb_pv(ds_idx, pv_idx, 0, &mvb_pv_p, 0, &app_pv_p, &pvn);
				 pv_idx++) {
				printf(" %-9s %-20s = 0x", decode_eid(mvb_pv_p->eid, s),
		   				(app_pv_p && app_pv_p->txt_name) ? app_pv_p->txt_name : "<?>");
				if (get_line(s, sizeof(s), FALSE)) {
					pi_zero8(buf, 32);
			 		parse_buffer(s, (void*)buf, 1);
					ap_frc_variable(pvn, buf);
   					if (argc == 3) {
						printf("Press a key to continue...");
   	 					sio_get_key();
						printf("\n");
						pi_zero8(buf, 32);
						ap_frc_variable(pvn, buf);
			       		ap_unfrc_variable(pvn);
   	   				}
   				}
			}
		}
	}

	return 0;
}


/*----------------------------------------------------------------------------------------------*/
#ifdef ATR_WTB

static short wds_dump(short argc, char *argv[])
{
	short				err;			/* error code */
	short				pa;				/* port address */
	short				ds_idx = 0;		/* dataset index */
	short				pv_idx;			/* process variable index */

	if (argc == 1) {
		for (ds_idx = 1; ds_idx < 64; ds_idx++) err = dump_wtb_pv(ds_idx, -1, TRUE);
		return 0;
	}
	if (argc == 2) {
		ds_idx = parse_number(argv[1]);
		for (pv_idx = 0, err = 0; !err; pv_idx++) err = dump_wtb_pv(ds_idx, pv_idx, pv_idx == 0);
		return 0;
	}
	return -1;
}

#endif


/*==============================================================================================*/
/* Software status commands */

/*----------------------------------------------------------------------------------------------*/
extern struct BA_STR_DIAGNOSIS *ba_status;

#ifdef ATR_WTB
static void print_node_status(Type_NodeStatus node_status)
{
	if (node_status.node_report & DA1_BIT) printf("DA1 "); else printf("--- ");
	if (node_status.node_report & DA2_BIT) printf("DA2 "); else printf("--- ");
	if (node_status.node_report & DB1_BIT) printf("DB1 "); else printf("--- ");
	if (node_status.node_report & DB2_BIT) printf("DB2 "); else printf("--- ");
	if (node_status.node_report & INT_BIT) printf("INT "); else printf("--- ");
	if (node_status.node_report & DSC_BIT) printf("DSC "); else printf("--- ");
	if (node_status.node_report & SLP_BIT) printf("SLP "); else printf("--- ");
	if (node_status.node_report & SAM_BIT) printf("SAM "); else printf("--- ");
	printf("0x%02X", node_status.user_report);
}
#endif

static short jdp_dump(short argc, char *argv[])
{
#ifdef ATR_WTB
	const CMessageDef m_no_yes[]   = {0, "NO",  1, "YES", 0, 0};
	const CMessageDef m_off_on[]   = {0, "OFF", 1, "ON",  0, 0};
#endif

	static MvbcRedStatus					red_status;
	static struct STR_AM_RD_MSNGR_STATUS	am_status;
	static char								ts[12];

	int i, j;

	if (argc != 1) return -1;

	am_read_messenger_status(&am_status);

	printf("\n");

	printf("*** MVBD status ***\n");
	if (atr_hw_code == STA_CODE || atr_hw_code == COM_CODE)
		printf("LACO : %-5u  LBCO : %-5u\n", laco_counter, lbco_counter);
	printf("LAJO : %-5u  LBJO : %-5u  SHD : %-5u\n", lajo_counter, lbjo_counter, shd_counter);
	printf("\n");

	if (mvbc_red_status(&red_status) == 0) {
		const CMessageDef m_broken_ok[]   = {FALSE, "BROKEN", TRUE, "OK    ", 0, 0};
		printf("*** MVB redundancy status ***\n");
		for (i = 0; i < 2; i++) {
			printf("Line %c: %s - Devices: ", i == 0 ? 'A' : 'B',
					c_fetch_message(m_broken_ok, red_status.line_ok[i], ts, 0) );
			for (j = 0; j < 256; j++) if (c_get_bit(red_status.cl_2p[i], j)) printf("%d ", j);
			for (j = 0; j < 256; j++) if (c_get_bit(red_status.cl_1[i], j)) printf("%d ", j * 16);
			printf("\n");
		}
		printf("\n");
	}

	if (ba_status) {
		printf("*** MVB Bus Administrator status ***\n");
		printf("AMFX panic           : %-5u   EMF panic            : %u\n", c_amfx_panic, emf_panic_count);
		printf("EMF in Bus Master    : %-5u   EMF in other states  : %u\n", emf_bm_count, emf_slv_count);
		printf("BTI deadlock detect  : %-5u   BTI in Bus Master    : %u\n", bti_deadlock_count, bti_bm_count);
		printf("BTI in other states  : %-5u   FEV                  : %u\n", bti_slv_count, fev_count);
		printf("DTI6 in Bus Master   : %-5u   DTI6 in other states : %u\n", dti6_bm_count, dti6_slv_count);
		printf("TMR2 silence when RA : %-5u   TMR2 noise when RA   : %u\n", tmr2_ra_silence_count, tmr2_ra_noise_count);
		printf("TMR2 deadlock detect : %-5u   TMR2 panic           : %u\n", tmr2_deadlock_count, tmr2_panic_count);
		printf("c_scan_update        : %lu\n", ba_status->c_scan_update);
		printf("c_error_frame        : %lu\n", ba_status->c_error_frame);
		printf("c_ref_frame          : %lu\n", ba_status->c_ref_frame);
		printf("c_poll_event_error   : %lu\n", ba_status->c_poll_event_error);
		printf("c_poll_rsp_total     : %lu\n", ba_status->c_poll_rsp_total);

		printf("Event poll responses :\n");
 		for (i = 0; i < 256; i++) if (ba_status->p_poll_rsp->c_event[i]) {
			printf("  Dev %d : %lu\n", i, ba_status->p_poll_rsp->c_event[i]);
   		}

		printf("Present devices      : ");
 		for (i = 1; i < 256; i++) if (ba_status->p_scan_rslt_cl_2p->sn_rslt[i].rsp == 0)
			printf("%d (0x%x) ", i, ba_status->p_scan_rslt_cl_2p->sn_rslt[i].dsw);
 		for (i = 16; i < 256; i++) if (ba_status->p_scan_rslt_cl_1->sn_rslt[i].rsp == 0)
			printf("%d (0x%x) ", i*16, ba_status->p_scan_rslt_cl_1->sn_rslt[i].dsw);
		printf("\n");

//		printf("Absent devices       : ");
// 		for (i = 1; i < 256; i++) if (ba_status->p_scan_rslt_cl_2p->sn_rslt[i].rsp == 1)
//			printf("%d ", i);
//		for (i = 16; i < 256; i++) if (ba_status->p_scan_rslt_cl_1->sn_rslt[i].rsp == 1)
//			printf("%d ", i*16);
//		printf("\n");

		printf("Devices not polled   : ");
 		for (i = 1; i < 256; i++) if (ba_status->p_scan_rslt_cl_2p->sn_rslt[i].rsp == 2)
			printf("%d ", i);
 		for (i = 16; i < 256; i++) if (ba_status->p_scan_rslt_cl_1->sn_rslt[i].rsp == 2)
			printf("%d ", i*16);
		printf("\n");

		printf("Devices with error   : ");
 		for (i = 1; i < 256; i++) if (ba_status->p_scan_rslt_cl_2p->sn_rslt[i].rsp == 3)
			printf("%d ", i);
 		for (i = 16; i < 256; i++) if (ba_status->p_scan_rslt_cl_1->sn_rslt[i].rsp == 2)
			printf("%d ", i*16);
		printf("\n\n");

    }

	printf("*** Messenger status ***\n");
	printf("messages_sent     : %lu\n", am_status.messages_sent);
	printf("messages_received : %lu\n", am_status.messages_received);
	printf("packets_sent      : %lu\n", am_status.packets_sent);
	printf("packet_retries    : %lu\n", am_status.packet_retries);
	printf("multicast_retries : %lu\n", am_status.multicast_retries);
	printf("send_time_out     : %u\n", (int)am_status.send_time_out);
	printf("ack_time_out      : %u\n", (int)am_status.ack_time_out);
	printf("alive_time_out    : %u\n", (int)am_status.alive_time_out);
	printf("credit            : %u\n", (int)am_status.credit);
	printf("packet_size       : %u\n", (int)am_status.packet_size);
	printf("instances         : %u\n", (int)am_status.instances);
	printf("multicast_window  : %u\n", (int)am_status.multicast_window);
	printf("\n");

#ifdef ATR_WTB
	{
		const CMessageDef	m_hardware_state[]   = {0,"OK", 1,"FAILURE", 0,0};
		const CMessageDef	m_link_layer_state[] = {0,"INITIALIZED", 1,"CONFIGURED", 2,"READY_TO_NAME",
													3,"READY_TO_BE_NAMED", 4,"REG_OPERATION_SLAVE",
													5,"REG_OPERATION_WEAK", 6,"REG_OPERATION_STRONG",
													7,"INHIBITED", 0, 0};
		const CMessageDef	m_node_address[]     = {127,"UNNAMED (127)", 0,0};
		const CMessageDef	m_node_orient[]      = {0,"UNKNOWN", 1,"SAME", 2,"INVERSE", 0,0};
		const CMessageDef	m_node_strength[]    = {0,"UNDEFINED", 1,"SLAVE", 2,"STRONG", 3,"WEAK", 0,0};
		const CMessageDef	m_node_line_error[]  = {0,"NONE", 0,0};

		static Type_WTBStatus	s;
		static Type_Topography	t;
		static Type_WTBNodes	n;
		static unsigned char	addr_inaug_line_error, fritt_source_on;
		static char				node_status_avail;

		if (!ls_t_GetStatus(&s)) {
			printf("*** WTB Link Layer status ***\n");
			printf("WTB hardware      : %u\n", s.WTB_hardware);
			printf("WTB software      : %u\n", s.WTB_software);
			printf("Hardware state    : %s\n", c_fetch_message(m_hardware_state, s.hardware_state, ts, 0));
			printf("Link Layer state  : %s\n", c_fetch_message(m_link_layer_state, s.link_layer_state, ts, 0));
			printf("Net inhibit       : %s\n", c_fetch_message(m_no_yes, s.net_inhibit, ts, 0));
			printf("Node address      : %s\n", c_fetch_message(m_node_address, s.node_address, ts, "%02u"));
			printf("Node orientation  : %s\n", c_fetch_message(m_node_orient, s.node_orient, ts, 0));
			printf("Node strength     : %s\n", c_fetch_message(m_node_strength, s.node_strength, ts, 0));
			printf("Node frame size   : %u\n", s.node_descriptor.node_frame_size);
			printf("Node period       : %u\n", s.node_descriptor.node_period);
			printf("Node key & status : **>%02X%02X ",
				s.node_descriptor.node_key.node_type, s.node_descriptor.node_key.node_version);
			print_node_status(s.node_status);
			putchar('\n');

			mac_get_info(&addr_inaug_line_error, &fritt_source_on);
			printf("Line error node   : %s\n", c_fetch_message(m_node_line_error, addr_inaug_line_error, ts, "%02u"));

			printf("\n");

			if (s.node_address != 127 && !ls_t_GetTopography(&t)) {
				if (!ls_t_GetWTBNodes(&n)) node_status_avail = TRUE;
				else node_status_avail = FALSE;

				printf("*** WTB topography ***\n");
				printf("Node address      : %u\n", t.node_address);
				printf("Node orientation  : %s\n", c_fetch_message(m_node_orient, t.node_orient, ts, 0));
				printf("Topo counter      : %u\n", t.topo_counter);
				printf("Individual period : %u\n", t.individual_period);
				printf("Is strong         : %s\n", c_fetch_message(m_no_yes, t.is_strong, ts, 0));
				printf("Number of nodes   : %u\n", t.number_of_nodes);
				printf("Bottom address    : %u\n", t.bottom_address);
				printf("Top address       : %u\n", t.top_address);
				printf("Node key & status : ");
				for (i = 0, j = t.bottom_address; i < t.number_of_nodes; i++, j = j == 63 ? 1 : j + 1) {
//#ifndef UMS
					printf("%02u>%02X%02X ", j, t.NK[i].node_type, t.NK[i].node_version);
//#else
//					printf("%02u>???? ", j);
//#endif
					if (node_status_avail) print_node_status(n.node_status_list[i]);
					printf("\n                    ");
				}
				putchar('\n');
			}
		}
	}

	{
		const CMessageDef m_open_close[]    = {D_OPEN,"OPENED",  D_CLOSE,"CLOSED", 0,0};
		const CMessageDef m_line_a_b[]      = {D_LINE_A,"A", D_LINE_B,"B", 0,0};
		const CMessageDef m_direction_1_2[] = {D_DIRECT_1,"1", D_DIRECT_2,"2", 0,0};

		static Type_LLStatisticData	sd;
		static type_StatisticData 	*s;
		static WTBDriverStatus    	*d;

		printf("*** WTB statistic data ***\n");

		if (!ls_t_GetStatisticData(&sd)) {
			printf("Basic period counter  : %lu\n", sd.basic_period_cnt);
			printf("Inauguration counter  : %u\n", sd.inaug_cnt);
			printf("Topography counter    : %u\n", sd.topo_cnt);
			printf("Transmitted MD frames : %u\n", sd.NoTMD);
			printf("Received MD frames    : %u\n", sd.NoRMD);
		}

		d_read_statistic(&s);
		d = (WTBDriverStatus*)s->p_detailed_state;

		printf("Line switch counter   : %u\n", d->stat.LineSwitch);
		printf("MAU hardware failure  : %-s\n", c_fetch_message(m_no_yes, d->mauhf, ts, 0));
		printf("Main direction        : %-s\n", c_fetch_message(m_direction_1_2, d->direction, ts, 0));
		printf("\n");

		printf("*** WTB Driver status ********** (A1) ** (B1) ** (A2) ** (B2) **\n");
		printf("Transmitted frames             : %-7lu %-7lu %-7lu %-7lu\n",
				d->stat.A1.NoTxF, d->stat.B1.NoTxF, d->stat.A2.NoTxF, d->stat.B2.NoTxF);
		printf("Frames received without error  : %-7lu %-7lu %-7lu %-7lu\n",
				d->stat.A1.NoRxF, d->stat.B1.NoRxF, d->stat.A2.NoRxF, d->stat.B2.NoRxF);
		printf("Frames received with error     : %-7u %-7u %-7u %-7u\n",
				d->stat.A1.NoFE,  d->stat.B1.NoFE,  d->stat.A2.NoFE,  d->stat.B2.NoFE);
		printf("Timeouts (ACK mode only)       : %-7u %-7u %-7u %-7u\n",
				d->stat.A1.NoTO,  d->stat.B1.NoTO,  d->stat.A2.NoTO,  d->stat.B2.NoTO);
		printf("\n");

		printf("*** WTB line status ************** (A) *** (B) ***\n");
		printf("Interruption switch              : %-7s %-7s\n",
			c_fetch_message(m_open_close, d->interruption_A, ts, 0),
			c_fetch_message(m_open_close, d->interruption_B, ts, 0));
		printf("Hardware fault sampling period   : %-7u %-7u\n", d->HFAp, d->HFBp);
		printf("\n");

		printf("*** WTB channel status *********** (1) *** (2) ***\n");
		printf("Baudrate                         : %-7lu %-7lu\n", d->ch[0].baudrate, d->ch[1].baudrate);
		printf("Frame timeout value              : %-7u %-7u\n",   d->ch[0].timeout,  d->ch[1].timeout);
		printf("Panic timeout value              : %-7u %-7u\n",
			d->ch[0].panic_timeout,  d->ch[1].panic_timeout);
		printf("Line A connection                : %-7s %-7s\n",
			c_fetch_message(m_open_close, d->ch[0].switch_A, ts, 0),
			c_fetch_message(m_open_close, d->ch[1].switch_A, ts, 0));
		printf("Line B connection                : %-7s %-7s\n",
			c_fetch_message(m_open_close, d->ch[0].switch_B, ts, 0),
			c_fetch_message(m_open_close, d->ch[1].switch_B, ts, 0));
		printf("Trusted line                     : %-7s %-7s\n",
			c_fetch_message(m_line_a_b, d->ch[0].line, ts, 0),
			c_fetch_message(m_line_a_b, d->ch[1].line, ts, 0));
		printf("Auto line switch                 : %-7s %-7s\n",
			c_fetch_message(m_off_on, d->ch[0].auto_sw, ts, 0),
			c_fetch_message(m_off_on, d->ch[1].auto_sw, ts, 0));
		printf("Receiver status                  : %-7s %-7s\n",
			c_fetch_message(m_off_on, d->ch[0].rx, ts, 0),
			c_fetch_message(m_off_on, d->ch[1].rx, ts, 0));
		printf("Receiver address                 : 0x%02x    0x%02x\n",
			d->ch[0].rx_addr, d->ch[1].rx_addr);
		printf("Transmitter status               : %-7s %-7s\n",
			c_fetch_message(m_off_on, d->ch[0].tx, ts, 0),
			c_fetch_message(m_off_on, d->ch[1].tx, ts, 0));
		printf("Fritting status                  : %-7s %-7s\n",
			c_fetch_message(m_off_on, d->ch[0].fritting_on, ts, 0),
			c_fetch_message(m_off_on, d->ch[1].fritting_on, ts, 0));
		printf("Line A disturbed                 : %-7s %-7s\n",
			c_fetch_message(m_no_yes, d->ch[0].ald, ts, 0),
			c_fetch_message(m_no_yes, d->ch[1].ald, ts, 0));
		printf("Line B disturbed                 : %-7s %-7s\n",
			c_fetch_message(m_no_yes, d->ch[0].bld, ts, 0),
			c_fetch_message(m_no_yes, d->ch[1].bld, ts, 0));
		printf("\n");
	}

	{
		const CMessageDef m_status[] = {PDM_INIT,"INIT", PDM_DISABLED,"DISABLED",
										PDM_ENABLED,"ENABLED", 0,0};
		const CMessageDef m_mode[]   = {PDM_DEFAULT_MODE,"DEFAULT_MODE",
										PDM_DO_NOT_MARSH,"DO_NOT_MARSH", 0,0};

		static T_PDM_STATUS ps;

		pdm_get_status(&ps);

		printf("*** PDM status ***\n");
		printf("PDM software version, MDB version    : %04X, %04X\n",
			ps.pdm_sw_version, ps.mdb_version);
		printf("Initialized, configured              : %s, %s\n",
			c_fetch_message(m_no_yes, ps.initialised, ts, 0),
			c_fetch_message(m_no_yes, ps.configured, ts, 0));
		printf("WTB TS created, topography indicated : %s, %s\n",
			c_fetch_message(m_no_yes, ps.wtb_ts_created, ts, 0),
			c_fetch_message(m_no_yes, ps.topo_indicated, ts, 0));
		printf("Export status, mode, cycle, result   : %s, %s, %u, %u\n",
			c_fetch_message(m_status, ps.ex_status, ts, 0), c_fetch_message(m_mode, ps.ex_mode, ts, 0),
			ps.ex_cycle, ps.ex_result);
		printf("Import status, mode, cycle, result   : %s, %s, %u, %u\n",
			c_fetch_message(m_status, ps.im_status, ts, 0), c_fetch_message(m_mode, ps.im_mode, ts, 0),
			ps.im_cycle, ps.im_result);
		printf("Internal status, mode, cycle, result : %s, %s, %u, %u\n",
			c_fetch_message(m_status, ps.in_status, ts, 0), c_fetch_message(m_mode, ps.in_mode, ts, 0),
			ps.in_cycle, ps.in_result);
		printf("\n");
	}
#endif

	return 0;
}


/*==============================================================================================*/
/* Messenger commands */

/*----------------------------------------------------------------------------------------------*/
static short mts_cmd(short argc, char *argv[])
{
	if (argc > 2) return -1;

	if (argc == 1) printf("Messenger trace flag value: %d\n", (int)nm_trace_enable);
	else {
		int i;
		i = parse_number(argv[1]);
 		nm_trace_enable = i;
 	}

	return 0;
}


/*----------------------------------------------------------------------------------------------*/
static short sf_cmd(short argc, char *argv[])
{
	int			i;
	short 		err = 0;		/* generic error code */
    AM_RESULT	status;			/* am error code */

	if (argc == 1 || argc > 3) return -1;

	i = parse_number(argv[1]);

    am_bind_replier(i, rcv_conf, reply_conf, &status);
    Check(status);
    am_rcv_requ(i, 0, MAX_MSG_LEN, (void*)(argc == 3), &status);
    Check(status);

error:
	if (err) printf("Got error %d!", err);

	return 0;
}


/*----------------------------------------------------------------------------------------------*/
static short message_cmd(short argc, char *argv[])
{
	AM_ADDRESS	dest_net_addr;	/* destination adress */
	char		our_fct;		/* our function */
	int			mess_len;		/* message length */

	if (argc != 7) return -1;

	if (*argv[1] == '*') dest_net_addr.vehicle = AM_SAME_VEHICLE;
	else dest_net_addr.vehicle = parse_number(argv[1]);

	dest_net_addr.fct_or_sta = parse_number(argv[2]);

	if (*argv[3] == '*') dest_net_addr.next_sta = AM_UNKNOWN_DEVICE;
	else dest_net_addr.next_sta = parse_number(argv[3]);

	if (*argv[4] == '*') dest_net_addr.tc = AM_ANY_TC;
	else dest_net_addr.tc = parse_number(argv[4]);

	our_fct = parse_number(argv[5]);

	mess_len = parse_buffer(argv[6], (void*)buf, 1);
	am_call_requ(our_fct, &dest_net_addr, buf, mess_len, 0, 1100, 0, call_conf, 0);

  	pi_pend_semaphore(ushell_sema, PI_FOREVER, 0);

	return 0;
}


/*==============================================================================================*/
/* Network management routines */

#define GetTNM16(p) \
	((((unsigned short)((unsigned char*)p)[0]) << 8) | ((unsigned short)((unsigned char*)p)[1]))
#define NextTNMString(p) if (1) { int i = strlen((char*)p) + 1; i += (4 - (i & 3)) & 3; p += i; } else

/*----------------------------------------------------------------------------------------------*/
static short read_inventory_cmd(short argc, char *argv[])
{
	short 			err;			/* error code */
	char			*data = 0;		/* ptr to the data */
	unsigned short	data_len;		/* size of the data */
	unsigned char	*p;				/* generic char ptr */
	short			i;				/* generic index */
	unsigned char	veh, sta;		/* destination vehicle and station */

	/* parse the arguments */
	if (argc == 2) {
		veh = AM_SAME_VEHICLE;
		sta = parse_number(argv[1]);
	}
	else if (argc == 3) {
		veh = parse_number(argv[1]);
		sta = parse_number(argv[2]);
	}
	else return -1;

	Check(atr_nma_call(veh, sta, AM_ANY_TC, 2, 0, 0, &data, &data_len));

	p = (unsigned char *)data + 2;
	putchar('\n');
	printf("agent_version       : %s\n", p);
	NextTNMString(p);
	printf("manufacturer_name   : %s\n", p);
	NextTNMString(p);
	printf("device_type         : %s\n", p);
	NextTNMString(p);
	printf("service_set (1)     : 0x");
	for (i = 0; i < 16; i++) printf("%02X", *p++);
	putchar('\n');
	printf("service_set (2)     : 0x");
	for (i = 16; i < 32; i++) printf("%02X", *p++);
	putchar('\n');
	printf("link_set            : 0x%02X%02X\n", p[0], p[1]);
	p += 3;
	printf("station_id          : 0x%02X\n", *p++);
	printf("station_name        : %s\n", p);
	NextTNMString(p);
	printf("station_status_word : 0x%02X%02X\n", p[0], p[1]);

error:
	if (data) pi_free(data);
	if (err) printf("*** Unable to connect, err=%d\n", err);
	return 0;
}


/*----------------------------------------------------------------------------------------------*/

static char st_bf[256/8];	/* stations bitfield */

static short read_vehicle_cmd(short argc, char *argv[])
{
	const StEntry	*st;			/* station list                                 */
	unsigned short	st_num;			/* number of stations                           */
	short 			err2;			/* error codes                                  */
	short			dummy = 0;		/* data to be sent to get the station list      */
	char			*list = 0;		/* ptr to the station list                      */
	unsigned short	list_len;		/* size of the station list                     */
	char			*data = 0;		/* ptr to the station data                      */
	unsigned short	data_len;		/* size of the station data                     */
	unsigned char	*p, *p2;		/* generic char ptrs used to parse the messages */
	short			i, j;			/* generic indexes                              */
	unsigned short	nr_devices;		/* number of devices                            */
	unsigned short	address;		/* device address                               */

	if (argc != 1) return -1;

	/* get the station list */
	nc_get_st(&st, &st_num);

	/* clear the stations bitfield */
	pi_zero8(st_bf, sizeof(st_bf));

	/* loop for all the Bus Administrators */
	for (i = 0; i < st_num; i++) if (st[i].flags & NC_ST_IS_MVB_BA) {

		/* get the present devices list */
		err2 = atr_nma_call(AM_SAME_VEHICLE, st[i].mvb_address, AM_ANY_TC, 12,
							(void*)&dummy, sizeof(dummy), &list, &list_len);

		if (!err2) {

			/* point to nr_devices field */
			p = (unsigned char *)list + 4;

			/* get the number of present devices */
			nr_devices = (unsigned short)p[0] << 8 | p[1];
			p += 2;

			/* loop for all the devices */
			for (j = 0; j < nr_devices; j++) {

				/* get the device address */
				address = (unsigned short)p[0] << 8 | p[1];
				p += 4;

				/* mark it as present if not class 1 */
				if (address < 256) c_set_bit(st_bf, address);
			}
		}

		/* relase the list memory */
		if (list) pi_free(list);
		list = 0;
	}

	putchar('\n');

	/* loop for all the class 2+ devices */
	for (i = 0; i < 256; i++) {

		/* full report if it is found by the Bus Administrator */
		if (c_get_bit(st_bf, i)) {

			/* print the address */
			printf("[%3d] ", i);

			/* Read_Station_Inventory */
			err2 = atr_nma_call(AM_SAME_VEHICLE, i, AM_ANY_TC, 2, 0, 0, &data, &data_len);
			if (err2) {
				printf("Error = %d\n", err2);
				continue;
			}

			/* find the relevant strings */
			p = (unsigned char *)data + 2;
			NextTNMString(p);
			NextTNMString(p);
			p2 = p;
			NextTNMString(p2);
			p2 += 36;

			printf("(%8s) %s\n", p2, p);

			/* free the data */
			pi_free(data);
			data = 0;
		}
		else {
			/* if the device in our configuration give a note */
			for (j = 0; j < st_num; j++) if (st[j].mvb_address == i)
				printf("[%3d] Not found\n", i);
		}
	}

	return 0;
}


/*----------------------------------------------------------------------------------------------*/
static short rsh_cmd(short argc, char *argv[])
{
	short 			err;
	char  			*in;
	unsigned short 	in_len;
	int				i = 0;
	unsigned char	veh, sta;		/* destination vehicle and station */
	char			*cmd;			/* command string pointer          */

	if (argc == 3) {
		veh = AM_SAME_VEHICLE;
		sta = parse_number(argv[1]);
		cmd = argv[2];
	}
	else if (argc == 4) {
		veh = parse_number(argv[1]);
		sta = parse_number(argv[2]);
		cmd = argv[3];
	}
	else return -1;

	Check(atr_nma_call(veh, sta, AM_ANY_TC, ATR_NMA_SHELL_SIF, cmd, strlen(cmd)+1, &in, &in_len));
	while (in[i]) putchar(in[i++]);
	pi_free(in);

error:
	if (err) printf("Error: %d\n", err);
	return 0;
}


/*----------------------------------------------------------------------------------------------*/
static short ncdb_cmd(short argc, char *argv[])
{
	short 			err;		/* generic error code */
	const void		*obj;		/* ptr to the object */
	unsigned short	obj_size;	/* size of the object */

	if (argc != 2) return -1;

	Check(nc_get_db_element(parse_number(argv[1]), &obj, &obj_size));
	dump_buffer(FALSE, obj, obj_size);

error:
	if (err) printf("Got error %d!", err);

	return 0;
}


/*----------------------------------------------------------------------------------------------*/
static short ncdb_open_cmd(short argc, char *argv[])
{
	short 			err;		/* generic error code */

	if (argc != 1) return -1;

	Check(start_ext_ncdb_update());

error:
	if (err) printf("Got error %d!", err);

	return 0;
}


/*----------------------------------------------------------------------------------------------*/
static short ncdb_write_cmd(short argc, char *argv[])
{
	short 			err;		/* generic error code */
	unsigned short	len;		/* length of the data */

	if (argc != 3) return -1;

	len = parse_buffer(argv[2], (void*)(buf + sizeof(NcObjHeader)), 1);
	((NcObjHeader*)buf)->oid  = parse_number(argv[1]);
	((NcObjHeader*)buf)->size = len;
	Check(write_ext_ncdb_object((void*)buf));

error:
	if (err) printf("Got error %d!", err);

	return 0;
}


/*----------------------------------------------------------------------------------------------*/
static short ncdb_close_cmd(short argc, char *argv[])
{
	short 			err;		/* generic error code */

	if (argc != 1) return -1;

	Check(end_ext_ncdb_update());

error:
	if (err) printf("Got error %d!", err);

	return 0;
}


/*---------------------------------------------------------------------------------------------*/
/* NCDB XMODEM-CRC upload routines */

static const CMessageDef m_ncdb_upload_error[] = {
	{XMODEM_NCDB_WRITE_ERR,     "Unable to write the NCDB"},
	{XMODEM_NCDB_FORMAT_ERR,    "Wrong NCDB format"},
	{XMODEM_NCDB_MEMORY_ERR,    "Not enough memory"},
	{XMODEM_CANCEL_ERR, 		"Unable to write the NCDB"},
	{XMODEM_RETRY_ERR,  		"Unable to write the NCDB"},
	{0, 0}
};

static short ncdb_upload_cmd(short argc, char *argv[])
{
	short	err; 	/* error code       */
	char	ts[12];	/* temporary string */

	/* check the arguments */
	if (argc != 1) return -1;

	/* do the transfer */
	printf("Please start the XMODEM-CRC transfer...\n");
	err = xmodem_ext_ncdb_rcv(0);

	/* dump the result */
	if (err) printf("\n\nERROR: %s\n", c_fetch_message(m_ncdb_upload_error, err, ts, 0));

	return 0;
}


/*==============================================================================================*/
/* Network performance commands */

/*----------------------------------------------------------------------------------------------*/
/* Read Memory stress test */

static short tnm_read_memory_test_cmd(short argc, char *argv[])
{
	short			err = 0;	/* error code                        */
	unsigned char	veh, sta;	/* vehicle and station to test       */
	unsigned long	addr;		/* memory address                    */
	unsigned short	len;		/* number of bytes to transfer       */
	unsigned long	total = 0;	/* total number of transferred bytes */
	unsigned long	time;		/* the start and total time          */

	struct {
		unsigned char	reserved;
		unsigned char	nr_regions;
		unsigned long	base_address;
		unsigned short	nr_items;
		unsigned char	reserved2;
		unsigned char	item_size;
	} cmd;

	/* check the arguments */
	if (argc != 5) return -1;

	/* parse the arguments */
	veh  = parse_number(argv[1]);
	sta  = parse_number(argv[2]);
	addr = parse_number(argv[3]);
	len  = parse_number(argv[4]);

	/* fill the cmd fields */
	cmd.reserved     = 0;
	cmd.nr_regions   = 1;
	cmd.base_address = addr;
	cmd.nr_items     = len;
	cmd.reserved2    = 0;
	cmd.item_size    = 1;

	/* get the current time */
	time = pi_ticks_elapsed();

	while (TRUE) {

		/* execute the request discarding the result */
		Check(atr_nma_call(veh, sta, AM_ANY_TC, 50, (char*)&cmd, sizeof(cmd), 0, 0));
		total += len;

		/* wait for a keypress to exit */
		if (sio_poll_key(1)) break;
	}

	/* get the elapsed time in ticks */
	time = pi_ticks_elapsed() - time;

	/* print the transfer speed */
	printf("Transfer speed = %lu B/s\n", (1000UL / PI_TICK) * total / time);

error:
	if (err) printf("ERROR: %d\n", err);

	return 0;
}


/*==============================================================================================*/
/* WTB specific commands */

#ifdef ATR_WTB

/*----------------------------------------------------------------------------------------------*/
/* WTB frames intercept */

#define WF_DFLT_NUM_LOG_EVENTS	32		/* default number of events to log */

#define WF_LOG_SEND_ENTRY		1
#define WF_LOG_RCV_ENTRY		2

#define WF_IDU_SIZE				12

typedef struct {
	unsigned long	time;
	char			what;
	char			X;
	unsigned char	line;
	unsigned char	status;
	unsigned char	idu[WF_IDU_SIZE];
} WF_TraceEvent;

static volatile char	wf_log_enabled;
static volatile int		wf_log_rp, wf_log_wp;
static short			wf_num_log_events = WF_DFLT_NUM_LOG_EVENTS;
static WF_TraceEvent	*wf_trace_log;

static void trace_send_confirm(int X, type_idu_drv *p_idu, unsigned char send_state)
{
	if (wf_log_enabled) {
		WF_TraceEvent *e = &wf_trace_log[wf_log_wp];

		e->time   = cpm_risctimer_counter();
		e->what   = WF_LOG_SEND_ENTRY;
		e->X      = X;
		e->line   = 0;
		e->status = send_state;
		pi_copy8(&e->idu, p_idu, sizeof(e->idu));

		wf_log_wp = (wf_log_wp + 1) % wf_num_log_events;
		if (wf_log_wp == wf_log_rp) wf_log_enabled = 0;
	}
}

static void trace_rcv_event(int X, type_idu_drv *pdu, unsigned char line, unsigned char status)
{
	if (wf_log_enabled) {
		WF_TraceEvent *e = &wf_trace_log[wf_log_wp];

		e->time   = cpm_risctimer_counter();
		e->what   = WF_LOG_RCV_ENTRY;
		e->X      = X;
		e->line   = line;
		e->status = status;
		pi_copy8(&e->idu, pdu, sizeof(e->idu));

		wf_log_wp = (wf_log_wp + 1) % wf_num_log_events;
		if (wf_log_wp == wf_log_rp) wf_log_enabled = 0;
	}
}

static void print_idu(unsigned char *idu)
{
	int i, j;

	j = ((type_idu_drv *)idu)->size + 4;
	if (j > WF_IDU_SIZE) j = WF_IDU_SIZE;

	printf("%02X%02X%02X%02X ", idu[0], idu[1], idu[2], idu[3]);
	for (i = 4; i < j; i++) printf("%02X", idu[i]);
}

static short wf_cmd(short argc, char *argv[])
{
	int	 i = 0;
	char c;
	unsigned long time = 0;

	if (argc > 2) return -1;
	if (argc > 1) {
		if (*argv[1] == '?') {
			printf("l)SetSlave w)SetWeak s)SetStrong n)StartNaming r)Remove\n");
			printf("i)Inhibit  a)Allow   1)SetSleep  2)CancelSleep\n");
			return 0;
		}
		wf_num_log_events = parse_number(argv[1]);
	}

	wf_trace_log = pi_alloc(wf_num_log_events * sizeof(WF_TraceEvent));
	if (!wf_trace_log) {
		printf("*** Out of memory ***\n");
		return 0;
	}

	wf_log_enabled = 1;
	wf_log_wp = wf_log_rp = 0;
	d_trace_install(trace_send_confirm, trace_rcv_event);

	while (1) {
		while (wf_log_rp != wf_log_wp) {
			WF_TraceEvent 	*e = &wf_trace_log[wf_log_rp];
			unsigned char	*p = (unsigned char *)&e->idu;

			if (e->time - time > 99) printf("++ ");
			else printf("%2u ", e->time - time);
			time = e->time;

			if (e->what == WF_LOG_SEND_ENTRY) {
				if (e->status) printf("%d!", e->status);
				printf(" %d<<", e->X);
				print_idu(e->idu);
			}
			else if (e->what == WF_LOG_RCV_ENTRY) {
				if (e->status == D_RPT_TIMEOUT) printf(" %d>>(tmo)", e->X);
				else {
					printf("%c%d>>", (e->line & 1) ? 'A' : 'B', e->X);
					print_idu(e->idu);
				}
			}
			putchar('\n');
			wf_log_rp = (wf_log_rp + 1) % wf_num_log_events;

			if ((++i & 0x3f) == 0) break;
		}
		if (!wf_log_enabled && wf_log_rp == wf_log_wp) {
			printf("[...]\n");
			wf_log_enabled = 1;
		}
		c = sio_poll_key(1);
		if (c) switch (c) {
			case 'l':	ls_t_SetSlave();		break;
			case 'w':	ls_t_SetWeak();			break;
			case 's':	ls_t_SetStrong();		break;
			case 'n':	ls_t_StartNaming();		break;
			case 'r':	ls_t_Remove();			break;
			case 'i':	ls_t_Inhibit();			break;
			case 'a':	ls_t_Allow();			break;
			case '1':	ls_t_SetSleep();		break;
			case '2':	ls_t_CancelSleep();		break;
			default:	wf_log_enabled = FALSE; goto exit;
		}
	}
exit:
	d_trace_install(0, 0);

	pi_free(wf_trace_log);

	return 0;
}


/*----------------------------------------------------------------------------------------------*/
/* WTB Link Layer report monitor */

int		   ll_debug_flag = 0;
#ifndef UMS
extern int map_debug_flag;
#endif

static const CMessageDef	m_Report[]   = {
	LR_CONFIGURED,        "LR_CONFIGURED : the node is now configured",
	LR_STRONG,            "LR_STRONG : node changed to strong master operational mode",
	LR_SLAVE,             "LR_SLAVE : node changed to slave master operational mode",
	LR_PROMOTED,          "LR_PROMOTED : weak master node indicates promotion",
	LR_NAMING_SUCCESSFUL, "LR_NAMING_SUCCESSFUL : master node indicates successful end of inauguration",
	LR_NAMED,             "LR_NAMED : slave node indicates end of inauguration",
	LR_WEAK,              "LR_WEAK : master node changed to weak master mode",
	LR_REMOVED,           "LR_REMOVED : node changed to state LS_CONFIGURED",
	LR_DEMOTED,           "LR_DEMOTED : weak master indicates yelding",
	LR_DISCONNECTION,     "LR_DISCONNECTION : slave node indicates disconnection",
	LR_INHIBITED,         "LR_INHIBITED : slave trying to insert notes inhibition",
	LR_INCLUDED,          "LR_INCLUDED : slave node indicates inclusion",
	LR_LENGTHENING,       "LR_LENGTHENING : master detected lengthening of train",
	LR_DISRUPTION,        "LR_DISRUPTION : master indicates loss of end node (line)",
	LR_MASTER_CONFLICT,   "LR_MASTER_CONFLICT : strong master detected another strong master",
	LR_NAMING_FAILED,     "LR_NAMING_FAILED : master node indicates failure while naming",
	LR_NEW_TOPOGRAPHY,    "LR_NEW_TOPOGRAPHY : node indicates change of topography",
	LR_NODE_STATUS,       "LR_NODE_STATUS : master node indicates status changed",
	LR_POLL_LIST_OVF,     "LR_POLL_LIST_OVF : master node indicates poll list overflow",
	LR_ALLOWED,           "LR_ALLOWED : slave trying to insert notes removal of inhibition",
	0,0};

void tcn_monitor_wtbll_Report(unsigned short r)
{
	char	ts[12];	/* temporary string */
	if (ll_debug_flag) printf("WTB LL Report : %s\n", c_fetch_message(m_Report, r, ts, 0));
}

void tcn_monitor_mac(void)
{
    int i;
    if (mac_view_enable)
    {
       printf("MAC next  mac ch1 zas event  state ch2 err \n");
       for ( i = mac_trace_ndx-1; i>=0; i=i-8)
            printf("  %02x %d  %d  %d %04x  %02x  %1d  %1d \n",
             mac_trace[i],    //  next state
             mac_trace[i-1],      //  mac_count
             mac_trace[i-2],    // ch1_disabled
             mac_trace[i-3],    //  zas_count
             mac_trace[i-4],    //  event
             mac_trace[i-5],    //  mac_state
             mac_trace[i-6],    //  ch2_disabled
             mac_trace[i-7]     //  err
   
             );
   
    }
}




static short  mac_trigger_cmd(short argc, char *argv[])
{
	         printf("  %02x %d  %d  %d %04x  %02x  %1d  %1d \n",
             mac_trigger[7],    //  next state
             mac_trigger[6],      //  mac_count
             mac_trigger[5],    // ch1_disabled
             mac_trigger[4],    //  zas_count
             mac_trigger[3],    //  event
             mac_trigger[2],    //  mac_state
             mac_trigger[1],    //  ch2_disabled
             mac_trigger[0]
             );     //  err
 	return 0;           
}

static short rtf_cmd(short argc, char *argv[])
{
	int	flag;

	if (argc != 2) return -1;

	flag = parse_number(argv[1]);
	ll_debug_flag  = flag & 1;
#ifndef UMS
	map_debug_flag = flag & 2;
#endif
	return 0;
}

/*----------------------------------------------------------------------------------------------*/

static short mac_trace_cmd(short argc, char *argv[])
{
    if (argc < 2)
        printf ("mac_view_enable %d\n", mac_view_enable);
    else
        mac_view_enable =parse_number(argv[1]);
}

#endif
/*==============================================================================================*/
/* "Human" interface */

void tcn_monitor(void)
{
	ushell_register("ver", "", "Print the version string", version_dump);

	ushell_register("m", "", "Dump the MVBC status", mvbc_dump);
	ushell_register("pp", "<num>", "Display a phisical port", pp_dump);
	ushell_register("da", "<num>", "Display a device address data", da_dump);
	ushell_register("la", "<num>", "Display a logical address data", la_dump);
	ushell_register("mf", "[<mask> <value>]", "Master frame intercept", mf_dump);
	ushell_register("mas", "[<value>]", "Show/set the master frame enable flag", mas_cmd);
	ushell_register("bt", "[<line>] [<master frame> [<cnt> [*|<isr0_msk> [*|<pcs1_msk>]]]]", "Bus test", bt_cmd);

	ushell_register("ds", "[<num>]", "Dump a dataset", ds_dump);
	ushell_register("pv", "<name>", "Dump a process variable", pv_dump);
	ushell_register("fc", "[<name> [<data>]]", "Force/unforce a/all process variable/s", pv_force);
	ushell_register("fc_ds", "<num> [*]", "Force all the process variables in a dataset", pv_force_ds);

#ifdef ATR_WTB
	ushell_register("wds", "[<num>]", "Dump WTB datasets", wds_dump);
	ushell_register("wf", "[<n> | ?]", "Dump WTB frames", wf_cmd);
	ushell_register("rtf", "[<value>]", "Report trace flags (1:LL 2:MAP)", rtf_cmd);
        ushell_register("mac_trace", "[0|1]", "mac Trace", mac_trace_cmd);
     ushell_register("mac_tigger", "[]", "mac Trigger", mac_trigger_cmd);
        
#endif

	ushell_register("s", "", "JDP software status", jdp_dump);

	ushell_register("mts", "[<value>]", "Show/set the msg trace enable flags (1:pkt 2:tmo)", mts_cmd);
	ushell_register("sf", "<num> [*]", "Subscribe a function", sf_cmd);
	ushell_register("msg", "*|<veh> <fct> *|<sta> *|<topo> <snd_fct> <msg..>", "Send a message", message_cmd);

	ushell_register("read_inventory", "[<veh>] <sta>", "read the inventory of the specified station", read_inventory_cmd);
	ushell_register("vver", "", "read the inventory of all the vehicle", read_vehicle_cmd);
	ushell_register("rsh", "[<veh>] <sta> <command>", "Execute a command on a remote shell", rsh_cmd);
	ushell_register("ncdb", "<id>", "Dump an NCDB object", ncdb_cmd);
	ushell_register("ncdb_open", "", "Open the extern NCDB (erases it)", ncdb_open_cmd);
	ushell_register("ncdb_write", "<id> <data..>", "Write an object in the extern NCDB", ncdb_write_cmd);
	ushell_register("ncdb_close", "", "Close the extern NCDB", ncdb_close_cmd);
	ushell_register("ncdb_upload", "", "Upload the extern NCDB using XMODEM-CRC", ncdb_upload_cmd);

	ushell_register("tnm_read_mem", "<veh> <sta> <addr> <len>", "Read Memory test", tnm_read_memory_test_cmd);
}


/*==============================================================================================*/
