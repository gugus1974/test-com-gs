/*=============================================================================================*/
/* UMS utility functions           													           */
/* Implementation file (atr_ums.c)													           */
/* 																					           */
/* Version 1.0																		           */
/*																					           */
/* (c)1998 Ansaldo Trasporti														           */
/* Written by Giuseppe Carnevale													           */
/*=============================================================================================*/


/*=============================================================================================*/
/* Includes */

#include "pi_sys.h"
#include "am_sys.h"
#include "tcntypes.h"
#include "bmi.h"
#include "ums.h"
#include "umsconv.h"
#include "ushell.h"

#include "atr_ums.h"


/*=============================================================================================*/
/* Defines */

/* max size of the message to accept */
#define MAX_UIC_MSG_SIZE 1024


/*---------------------------------------------------------------------------------------------*/
/* Message codes */

/* UTBC specific codes */
#define UIC_FC_F0_01_DEL_CONFIG               UIC_FC_DEL_CONFIG
#define UIC_FR_FA_01_DEL_CONFIG               UIC_FR_DEL_CONFIG
#define UIC_FC_F0_02_WRITE_CORRECTION         UIC_FC_WRITE_CORRECTION
#define UIC_FR_FA_02_WRITE_CORRECTION         UIC_FR_WRITE_CORRECTION
#define UIC_FC_F0_03_WRITE_VEH_RESNUM         UIC_FC_WRITE_VEH_RESNUM
#define UIC_FR_FA_03_WRITE_VEH_RESNUM         UIC_FR_WRITE_VEH_RESNUM

/* NADI specific codes */
#define UIC_FC_00_01_READ_NADI                UIC_FC_READ_NADI
#define UIC_FR_0A_01_READ_NADI                UIC_FR_READ_NADI
#define UIC_FC_00_F1_CVT_UIC_TO_TCN           UIC_FC_CVT_UIC_TO_TCN
#define UIC_FR_0A_F1_CVT_UIC_TO_TCN           UIC_FR_CVT_UIC_TO_TCN

/* GROUP specific codes */
#define UIC_FC_B0_01_READ_GROUP               UIC_FC_READ_GROUP
#define UIC_FR_BA_01_READ_GROUP               UIC_FR_READ_GROUP
#define UIC_FC_B0_02_READ_GROUP_LIST          UIC_FC_READ_GROUP_LIST
#define UIC_FR_BA_02_READ_GROUP_LIST          UIC_FR_READ_GROUP_LIST
#define UIC_FC_B0_03_WRITE_GROUP              UIC_FC_WRITE_GROUP
#define UIC_FR_BA_03_WRITE_GROUP              UIC_FR_WRITE_GROUP
#define UIC_FC_B0_04_WRITE_ALL_GROUPS         UIC_FC_WRITE_ALL_GROUPS
#define UIC_FR_BA_04_WRITE_ALL_GROUPS         UIC_FR_WRITE_ALL_GROUPS
#define UIC_FC_B0_05_DELETE_GROUP             UIC_FC_DELETE_GROUP
#define UIC_FR_BA_05_DELETE_GROUP             UIC_FR_DELETE_GROUP
#define UIC_FC_B0_06_DELETE_ALL_GROUPS        UIC_FC_DELETE_ALL_GROUPS
#define UIC_FR_BA_06_DELETE_ALL_GROUPS        UIC_FR_DELETE_ALL_GROUPS

/* WTB control specific codes */
#define UIC_FC_F0_04_SET_SLEEP                UIC_FC_SLEEP
#define UIC_FR_FA_04_SET_SLEEP                UIC_FR_SLEEP
#define UIC_FC_F0_05_INAUGURATION_CONTROL     UIC_FC_INAUGURATION_CONTROL
#define UIC_FR_FA_05_INAUGURATION_CONTROL     UIC_FR_INAUGURATION_CONTROL
#define UIC_FC_00_02_READ_UWTM_STATE          UIC_FC_READ_UWTM_STATE
#define UIC_FR_0A_02_READ_UWTM_STATE          UIC_FR_READ_UWTM_STATE
#define UIC_FC_F0_06_INAUGURATION_ENFORCE     UIC_FC_INAUGURATION_ENFORCE
#define UIC_FR_FA_06_INAUGURATION_ENFORCE     UIC_FR_INAUGURATION_ENFORCE
#define UIC_FC_00_03_CHANGE_OMODE             UIC_FC_CHANGE_OMODE
#define UIC_FR_0A_03_CHANGE_OMODE             UIC_FR_CHANGE_OMODE
#define UIC_FC_00_04_READ_UWTM_TOPO           UIC_FC_READ_UWTM_TOPOGRAPHY
#define UIC_FR_0A_04_READ_UWTM_TOPO     	  UIC_FR_READ_UWTM_TOPOGRAPHY
#define UIC_FC_00_F0_SET_LEADING_REQU         UIC_FC_SET_LEADING_REQU
#define UIC_FR_0A_F0_SET_LEADING_REQU         UIC_FR_SET_LEADING_REQU

/* UIMCS multicast specific codes */
#define UIC_FC_F0_07_MULTICAST_CREATE         UIC_FC_MULTICAST_CREATE
#define UIC_FR_FA_07_MULTICAST_CREATE         UIC_FR_MULTICAST_CREATE


/*---------------------------------------------------------------------------------------------*/
/* Message types */

/* the types are defined in "ums.h"; here we have only the patches */
// #undef UIC_TR_0A_01_READ_NADI
// #define UIC_TR_0A_01_READ_NADI             UIC_ReadNadi
// #undef UIC_TR_B0_A4_WRITE_ALL_GROUPS
// #define UIC_TR_BA_04_WRITE_ALL_GROUPS      UIC_Header
// #undef UIC_TR_0A_04_READ_UWTM_TOPO
// #define UIC_TR_0A_04_READ_UWTM_TOPO        UIC_TopoReply
#define UIC_TC_00_02_READ_UWTM_STATE       UIC_Header
#define UIC_TR_0A_02_READ_UWTM_STATE       UIC_UmsState


/*---------------------------------------------------------------------------------------------*/
/* Message conversion routines */

/* UTBC specific messages */
#define UIC_CC_F0_01_DEL_CONFIG(s,d)           HTON_header(s,d)
#define UIC_CR_FA_01_DEL_CONFIG(s,d)           NTOH_header(d,s)
#define UIC_CC_F0_02_WRITE_CORRECTION(s,d)     wtbconv_15_02_HTON(d,s)
#define UIC_CR_FA_02_WRITE_CORRECTION(s,d)     wtbconv_15_02A_NTOH(s,d)
#define UIC_CC_F0_03_WRITE_VEH_RESNUM(s,d)     wtbconv_15_03_HTON(d,s)
#define UIC_CR_FA_03_WRITE_VEH_RESNUM(s,d)     wtbconv_15_03A_NTOH(s,d);

/* NADI specific messages */
#define UIC_CC_00_01_READ_NADI(s,d)            wtbconv_00_01__HTON(d,s)
#define UIC_CR_0A_01_READ_NADI(s,d)            wtbconv_00_01A_NTOH(s,d)
#define UIC_CC_00_F1_CVT_UIC_TO_TCN(s,d)       wtbconv_32_01_02__HTON(d,s)
#define UIC_CR_0A_F1_CVT_UIC_TO_TCN(s,d)       wtbconv_32_01_02A_NTOH(s,d)

/* GROUP specific messages */
#define UIC_CC_B0_01_READ_GROUP(s,d)           wtbconv_11_01__HTON(d,s)
#define UIC_CR_BA_01_READ_GROUP(s,d)           wtbconv_11_01A_NTOH(s,d)
#define UIC_CC_B0_02_READ_GROUP_LIST(s,d)      wtbconv_11_02__HTON(d,s)
#define UIC_CR_BA_02_READ_GROUP_LIST(s,d)      wtbconv_11_02A_NTOH(s,d)
#define UIC_CC_B0_03_WRITE_GROUP(s,d)          wtbconv_11_03__HTON(d,s)
#define UIC_CR_BA_03_WRITE_GROUP(s,d)          wtbconv_11_03A_NTOH(s,d)
#define UIC_CC_B0_04_WRITE_ALL_GROUPS(s,d)     wtbconv_11_04__HTON(d,s)
#define UIC_CR_BA_04_WRITE_ALL_GROUPS(s,d)     wtbconv_11_04A_NTOH(s,d)
#define UIC_CC_B0_05_DELETE_GROUP(s,d)         wtbconv_11_05__HTON(d,s)
#define UIC_CR_BA_05_DELETE_GROUP(s,d)         wtbconv_11_05A_NTOH(s,d)
#define UIC_CC_B0_06_DELETE_ALL_GROUPS(s,d)    wtbconv_11_06__HTON(d,s)
#define UIC_CR_BA_06_DELETE_ALL_GROUPS(s,d)    wtbconv_11_06A_NTOH(s,d)

/* WTB control specific messages */
#define UIC_CC_F0_04_SET_SLEEP(s,d)            HTON_uic_fc_sleep(s,d)
#define UIC_CR_FA_04_SET_SLEEP(s,d)            NTOH_uic_fr_sleep(s,d)
#define UIC_CC_F0_05_INAUGURATION_CONTROL(s,d) HTON_uic_fc_inauguration_control(s,d)
#define UIC_CR_FA_05_INAUGURATION_CONTROL(s,d) NTOH_uic_fr_inauguration_control(s,d)
#define UIC_CC_00_02_READ_UWTM_STATE(s,d)      HTON_uic_fc_read_uwtm_state(s,d)
#define UIC_CR_0A_02_READ_UWTM_STATE(s,d)      NTOH_uic_fr_read_uwtm_state(s,d)
#define UIC_CC_F0_06_INAUGURATION_ENFORCE(s,d) HTON_uic_fc_inauguration_enforce(s,d)
#define UIC_CR_FA_06_INAUGURATION_ENFORCE(s,d) NTOH_uic_fr_inauguration_enforce(s,d)
#define UIC_CC_00_03_CHANGE_OMODE(s,d)         HTON_uic_fc_change_omode(s,d)
#define UIC_CR_0A_03_CHANGE_OMODE(s,d)         NTOH_uic_fr_change_omode(s,d)
#define UIC_CC_00_04_READ_UWTM_TOPO(s,d)       HTON_uic_fc_read_uwtm_topography(s,d)
#define UIC_CR_0A_04_READ_UWTM_TOPO(s,d)       NTOH_uic_fr_read_uwtm_topography(s,d)
#define UIC_CC_00_F0_SET_LEADING_REQU(s,d)     HTON_uic_fc_leading(s,d)
#define UIC_CR_0A_F0_SET_LEADING_REQU(s,d)     NTOH_uic_fr_leading(s,d)

/* MULTICAST specific messages */
#define UIC_CC_F0_07_MULTICAST_CREATE(s,d)     wtbconv_15_07__HTON(d,s)
#define UIC_CR_FA_07_MULTICAST_CREATE(s,d)     wtbconv_15_07A_NTOH(s,d)


/*=============================================================================================*/
/* Typedefs */

typedef struct {
	void		*out_msg;	/* sent message            */
	UmsUicInd	ind;		/* user indication routine */
	void		*user_ref;	/* user reference pointer  */
} UicCallDesc;


/*=============================================================================================*/
/* Globals */


/*=============================================================================================*/
/* Utilities */

/*---------------------------------------------------------------------------------------------*/
/* Generic UIC message copy with conversion routine */

#define UIC_CASE_HANDLE(code)				\
case UIC_F##code:							\
	if (src && dst) UIC_C##code(src,dst);	\
	if (size) *size = sizeof(UIC_T##code);	\
	break;

static void ums_uic_copy(unsigned short code, void *src, void *dst, unsigned short *size)
{
	switch (code) {

		/* UTBC specific codes */
		UIC_CASE_HANDLE(C_F0_01_DEL_CONFIG)
		UIC_CASE_HANDLE(R_FA_01_DEL_CONFIG)
		UIC_CASE_HANDLE(C_F0_02_WRITE_CORRECTION)
		UIC_CASE_HANDLE(R_FA_02_WRITE_CORRECTION)
		UIC_CASE_HANDLE(C_F0_03_WRITE_VEH_RESNUM)
		UIC_CASE_HANDLE(R_FA_03_WRITE_VEH_RESNUM)

		/* NADI specific codes */
		UIC_CASE_HANDLE(C_00_01_READ_NADI)
		UIC_CASE_HANDLE(R_0A_01_READ_NADI)
		UIC_CASE_HANDLE(C_00_F1_CVT_UIC_TO_TCN)
		UIC_CASE_HANDLE(R_0A_F1_CVT_UIC_TO_TCN)

		/* GROUP specific codes */
		UIC_CASE_HANDLE(C_B0_01_READ_GROUP)
		UIC_CASE_HANDLE(R_BA_01_READ_GROUP)
		UIC_CASE_HANDLE(C_B0_02_READ_GROUP_LIST)
		UIC_CASE_HANDLE(R_BA_02_READ_GROUP_LIST)
		UIC_CASE_HANDLE(C_B0_03_WRITE_GROUP)
		UIC_CASE_HANDLE(R_BA_03_WRITE_GROUP)
		UIC_CASE_HANDLE(C_B0_04_WRITE_ALL_GROUPS)
		UIC_CASE_HANDLE(R_BA_04_WRITE_ALL_GROUPS)
		UIC_CASE_HANDLE(C_B0_05_DELETE_GROUP)
		UIC_CASE_HANDLE(R_BA_05_DELETE_GROUP)
		UIC_CASE_HANDLE(C_B0_06_DELETE_ALL_GROUPS)
		UIC_CASE_HANDLE(R_BA_06_DELETE_ALL_GROUPS)

		/* WTB control specific codes */
		UIC_CASE_HANDLE(C_F0_04_SET_SLEEP)
		UIC_CASE_HANDLE(R_FA_04_SET_SLEEP)
		UIC_CASE_HANDLE(C_F0_05_INAUGURATION_CONTROL)
		UIC_CASE_HANDLE(R_FA_05_INAUGURATION_CONTROL)
		UIC_CASE_HANDLE(C_00_02_READ_UWTM_STATE)
		UIC_CASE_HANDLE(R_0A_02_READ_UWTM_STATE)
		UIC_CASE_HANDLE(C_F0_06_INAUGURATION_ENFORCE)
		UIC_CASE_HANDLE(R_FA_06_INAUGURATION_ENFORCE)
		UIC_CASE_HANDLE(C_00_03_CHANGE_OMODE)
		UIC_CASE_HANDLE(R_0A_03_CHANGE_OMODE)
		UIC_CASE_HANDLE(C_00_04_READ_UWTM_TOPO)
		UIC_CASE_HANDLE(R_0A_04_READ_UWTM_TOPO)
		UIC_CASE_HANDLE(C_00_F0_SET_LEADING_REQU)
		UIC_CASE_HANDLE(R_0A_F0_SET_LEADING_REQU)

		/* UIMCS multicast specific codes */
		UIC_CASE_HANDLE(C_F0_07_MULTICAST_CREATE)
		UIC_CASE_HANDLE(R_FA_07_MULTICAST_CREATE)

		/* unknown message */
		default:
			break;
	}
}


/*---------------------------------------------------------------------------------------------*/
/* Generic UIC message sender */

static void call_conf(unsigned char caller_fct, char *ext_ref, const AM_ADDRESS *replier,
					  char *in_msg_addr, CARDINAL32 in_msg_size, AM_RESULT status)
{
	UicCallDesc	*cd = (UicCallDesc *)ext_ref;	/* recover the call descriptor pointer */
	char		*msg = 0;						/* converted message                   */

    printf("\ncall_conf status %d\n",status);

	/* check the length of the received message */
	if (in_msg_size < sizeof(UIC_Header))
	    status = AM_FAILURE;

	if (status == AM_OK)
	{
		/* allocate the memory for the converted message */
		/* !!! should be fixed when the structs definition will have the maximum size !!!*/
		msg = pi_alloc(in_msg_size + 128);
		if (!msg) status = AM_FAILURE;
	}

	if (status == AM_OK)
	{
		/* convert the received message */
		ums_uic_copy(NTOH16(in_msg_addr + UIC_HEADER_CMD_OFFSET), in_msg_addr, msg, 0);

		/* invoke the callback routine */
		cd->ind(status, (UIC_Header*)msg, cd->user_ref);
	}
	else cd->ind(status, 0, cd->user_ref);
    printf("\ncall_conf status %d\n",status);

	/* deallocate the memory */
	if (msg) pi_free(msg);
    if (in_msg_addr) am_buffer_free(in_msg_addr, in_msg_size);
    if (cd->out_msg) pi_free(cd->out_msg);
}
static void call_conf2(unsigned char caller_fct, char *ext_ref, const AM_ADDRESS *replier,
					  char *in_msg_addr, CARDINAL32 in_msg_size, AM_RESULT status)
{
	char		*msg = 0;						/* converted message                   */

	printf("\ncall_conf status %d\n",status);

	/* check the length of the received message */
	if (in_msg_size < sizeof(UIC_Header))
	    status = AM_FAILURE;

	if (status == AM_OK)
	{
		/* allocate the memory for the converted message */
		/* !!! should be fixed when the structs definition will have the maximum size !!!*/
		msg = pi_alloc(in_msg_size + 128);
		if (!msg) status = AM_FAILURE;
	}

	printf("\ncall_conf status %d\n",status);

	/* deallocate the memory */
	if (msg) pi_free(msg);
    if (in_msg_addr) am_buffer_free(in_msg_addr, in_msg_size);
	pi_post_semaphore(ushell_sema, 0);

}


static short ums_uic_call(unsigned char owner, unsigned char dest_veh, unsigned char src_fct,
						  unsigned short code, unsigned char state_cmd,
						  UIC_Header *msg, UmsUicInd ind, void *user_ref)
{
	short			   err = 0;						/* error code                         */
	static char			  *out;							/* message to be sent                 */
	unsigned short	  out_size;						/* size of the message to be sent     */
	static UicCallDesc     *cd;							/* call description pointer           */
    static AM_ADDRESS  replier;						        /* net destination                    */

	/* fill the message header */
	msg->owner     = owner;
	msg->reserved1 = 0;
	msg->dest_veh  = dest_veh;
	msg->dest_fct  = UIC_F_UMS;
	msg->src_veh   = 0;
	msg->src_fct   = src_fct;
	msg->code      = code;
	msg->state_cmd = state_cmd;
	msg->reserved2 = 0;

	/* allocate and copy the message to be sent */
	ums_uic_copy(code, 0, 0, &out_size);
	out = pi_alloc(out_size + sizeof(*cd));
	if (!out) Check(-1);
	ums_uic_copy(code, msg, out, 0);

	/* fill the call descriptor */
	cd = (UicCallDesc *)(out + out_size);
	cd->out_msg  = out;
	cd->ind      = ind;
	cd->user_ref = user_ref;

	/* fill the net destination */
	replier.vehicle    = dest_veh;
	replier.fct_or_sta = UIC_F_UMS;
	replier.next_sta   = AM_UNKNOWN_DEVICE;
	replier.tc         = AM_ANY_TC;



	/* start the transfer */

	am_call_requ(src_fct, &replier, out, out_size, 0, MAX_UIC_MSG_SIZE, 0, call_conf, cd);

error:
	return err;
}


/*=============================================================================================*/
/* Implementation */

/*---------------------------------------------------------------------------------------------*/
//AM_RESULT ums_uic_read_nadi(char fct, UmsUicInd ind, void *user_ref)
//{
//}

/*---------------------------------------------------------------------------------------------*/
static void shell_ind(AM_RESULT status, UIC_Header *msg, void *user_ref)
{
	int i, j;
    if (status == AM_OK)
    {
	    switch (msg->code)
	    {

		    case UIC_FR_0A_01_READ_NADI:
			{
				UIC_ReadNadi *p = (UIC_ReadNadi*)msg;

				printf("\n*** NADI ***\n\n");

				printf("uic_ver      : %u\n",p->nadi.global_descr.uic_ver);
				printf("uic_subver   : %u\n",p->nadi.global_descr.uic_subver);
				printf("nadi_state   : %u\n",p->nadi.global_descr.nadi_state);
				printf("topo_counter : %u\n",p->nadi.global_descr.topo_counter);
				printf("num_entries  : %u\n",p->nadi.global_descr.num_entries);
				printf("not_av_veh   : 0x");
				for (i = 0; i < TFR_LEN_NA_VEH; i++)
					printf("%02X", p->nadi.global_descr.not_av_veh[i]);
				putchar('\n');
				printf("add_on_info  : 0x%02X\n\n", p->nadi.global_descr.add_on_info);

				for (i = 0; i < p->nadi.global_descr.num_entries; i++) {
					printf("Node %2u (TCN %02u), ID : %02X-%02X-%02X%02X%02X%02X%02X, No : %u, NcV : %d, Appl  : %02X-%02X\n",
						p->nadi.veh_descr[i].RangeNo, p->nadi.veh_descr[i].NodeAddr,
						p->nadi.veh_descr[i].veh_management, p->nadi.veh_descr[i].veh_owner,
						p->nadi.veh_descr[i].VehDescr.uic_id.uic_id[0],
						p->nadi.veh_descr[i].VehDescr.uic_id.uic_id[1],
						p->nadi.veh_descr[i].VehDescr.uic_id.uic_id[2],
						p->nadi.veh_descr[i].VehDescr.uic_id.uic_id[3],
						p->nadi.veh_descr[i].VehDescr.uic_id.uic_id[4],
						p->nadi.veh_descr[i].VehDescr.veh_res_num, p->nadi.veh_descr[i].num_ctrl_veh,
						p->nadi.veh_descr[i].application_id, p->nadi.veh_descr[i].application_ver);
					printf("    TSInfo   : ");
					for (j = 0; j < TFR_LEN_TS_INFO; j++)
						printf("%02X", p->nadi.veh_descr[i].TSInfo[j]);
					putchar('\n');
					printf("    veh_info : ");
					for (j = 0; j < TFR_LEN_VEH_INFO; j++)
						printf("%02X", p->nadi.veh_descr[i].VehDescr.veh_info[j]);
					printf(" veh_add_on : ");
					if (p->nadi.veh_descr[i].veh_add_on & 0x01) printf("TINV ");
					if (p->nadi.veh_descr[i].veh_add_on & 0x02) printf("UINV ");
					if (p->nadi.veh_descr[i].veh_add_on & 0x04) printf("LEAD ");
					if (p->nadi.veh_descr[i].veh_add_on & 0x08) printf("LREQ ");
					if (p->nadi.veh_descr[i].veh_add_on & 0xF0)
						printf("(0x%02X)", p->nadi.veh_descr[i].veh_add_on);
					putchar('\n');
				}
			}
			break;

		    case UIC_FR_0A_F0_SET_LEADING_REQU:
			break;

		    default:
		    {
		        int ix;
			    printf("*** Unknown code %04X\n", (unsigned short)msg->code);
			    for (ix=0;ix<sizeof(UIC_ReadNadi);ix++)
			        printf("*** byte ix%d %02X\n",ix,msg[ix]);
		    }
			break;
	    }
	}

	pi_post_semaphore(ushell_sema, 0);
}


static short nadi_cmd(short argc, char *argv[])
{
	short err;
	static UIC_TC_00_01_READ_NADI	msg;
  //  static AM_ADDRESS replier;

	Check(ums_uic_call(0, 0, 99, UIC_FC_00_01_READ_NADI, 0, (UIC_Header *)&msg, shell_ind, 0));
/*    replier.vehicle    = 0;
	replier.fct_or_sta = UIC_F_UMS;
	replier.next_sta   = AM_UNKNOWN_DEVICE;
	replier.tc         = AM_ANY_TC;
    am_call_requ(99, &replier, &msg, sizeof(UIC_TC_00_01_READ_NADI),0, 1100, 0, call_conf2, 0);
*/

  	pi_pend_semaphore(ushell_sema, PI_FOREVER, 0);
error:
	return err;
}

static short lead_cmd(short argc, char *argv[])
{
	short							err;
	UIC_TC_00_F0_SET_LEADING_REQU	msg;
	unsigned char					state_cmd;

	if (argc != 2) return -1;

	state_cmd = parse_number(argv[1]);
	msg.ld_direc = UIC_DIR1;

	Check(ums_uic_call(0, 0, 99, UIC_FC_00_F0_SET_LEADING_REQU, state_cmd, (UIC_Header *)&msg, shell_ind, 0));

  	pi_pend_semaphore(ushell_sema, PI_FOREVER, 0);
error:
	return err;
}


/*---------------------------------------------------------------------------------------------*/
void ums_monitor(void)
{
	ushell_register("nadi", "", "Print the NADI", nadi_cmd);
	ushell_register("lead", "<ld_direc>", "Leading request", lead_cmd);
}

/*=============================================================================================*/

