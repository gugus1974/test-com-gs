/*=============================================================================================*/
/* TCN configuration	    														           */
/* Header file (conf_tcn.h)															           */
/* 																					           */
/* Version 2.2                                                                                                                                                     */
/*																					           */
/* (c)1996/8 Ansaldo Trasporti														           */
/* Written by Giuseppe Carnevale													           */
/* 29.11.2001    ver 2.2           Schiavo                                                    */
/*   - si agg. func nc_mvb_map_cfg()                                                           */
/*=============================================================================================*/


#ifndef _CONF_TCN_
#define _CONF_TCN_


/*=============================================================================================*/
/* Includes */

#include "lc_sys.h"
#include "lp_sys.h"
#include "am_sys.h"
#include "pi_sys.h"
#include "bax_incl.h"
#include "mvbc_red.h"
#include "tcntypes.h"

#ifdef ATR_WTB
#include "bmi.h"
#include "pdm_main.h"
#ifdef UMS
#include "ums.h"
#include "uimcs.h"
#include "uwtm.h"
#else
#include "map.h"
#endif
#endif


/*=============================================================================================*/
/* NCDB definitions (in included file) */

#include "ncdb.h"


/*=============================================================================================*/
/* Constants */

/* Error codes */
#define NC_OK					0	/* no error */
#define NC_NOT_FOUND_ERR		1	/* missing element in the NC DB */
#define NC_INVALID_INDEX_ERR	2	/* invalid index specified */
#define NC_WRONG_CONF_ERR		3	/* the NCDB doesn't match the application configuration */
#define NC_MEM_ERR				4	/* out of memory */

/* Flags for the application datasets */
#define	NC_APP_DS_FORCE			BIT(0)	/* use the force data */
#define	NC_APP_DS_ELSE			BIT(1)	/* allow the access to someone else */
#define	NC_APP_DS_TMO			BIT(2)	/* handle the timeout on this dataset */
#define	NC_APP_DS_SAME_PI		BIT(3)	/* use the same Port Index of the last entry */
#define NC_APP_DS_CAN_MISS		BIT(4)	/* the ds and its pvs can be missing from the NCDB */


/*=============================================================================================*/
/* Typedefs */

typedef struct STR_LP_DS_SET TYPE_LP_DS_SET;


/*---------------------------------------------------------------------------------------------*/
/* Application TCN configuration */

typedef struct AppStEntry {					/**** Application station configuration entry ******/
	NcEleID			 	st_eid;				/* id of the station                               */
	const char			*txt_name;			/* name used for debug                             */
	unsigned short		am_max_call_num;	/* max number of simultaneous calls on this stat.  */
	unsigned short		am_max_inst_num;	/* max number of simultaneous replier instances    */
	unsigned short		am_ms_reply_tmo;	/* default reply timeout for call requests in ms   */
	unsigned short		am_my_credit;		/* max credit for all conn. ending on this stat.   */
	struct AppDsEntry	*app_ds_list;		/* ptr to the list of application datasets (MVB)   */
} AppStEntry;

typedef struct AppDsEntry {					/**** Application dataset entry ********************/
	NcEleID				ds_eid;				/* id of the dataset                               */
	const char			*txt_name;			/* name used for debug                             */
	struct AppPvEntry	*app_pv_list;		/* ptr to the list of process variables            */
	TYPE_LP_DS_SET		*p_ds_set;			/* ptr to the application dataset                  */
	unsigned short		*p_refresh;			/* ptr to the application refresh variable         */
	short			 	flags;				/* dataset flags                                   */
} AppDsEntry;

typedef struct AppPvEntry {					/**** Application process variable entry ***********/
	NcEleID				pv_eid;				/* id of the process variable                      */
	const char			*txt_name;			/* name used for debug                             */
	void				*p_data;			/* ptr to the data inside the application          */
} AppPvEntry;


/*=============================================================================================*/
/* Prototypes */

short nc_set_app_st_conf(const AppStEntry *app_st_list, NcEleID st_eid);

short nc_set_db(const void *extern_nc_db, const void *nc_db);
short nc_get_db_element(unsigned short id, const void **p_element, unsigned short *element_size);

short nc_mvb_address(NcEleID st_eid, unsigned short *address);

short nc_mvb_ts_cfg(TYPE_LP_TS_CFG **ts_cfg_h);
short nc_mvb_ds_cfg(void);
short nc_mvb_ba_cfg(struct BA_STR_CONFIGURATION **ba_cfg_h);

short nc_mvb_map_cfg(void);

short nc_tmo_cfg(TYPE_LP_TMO_CLUS **tmo_cfg_h, unsigned char bottom_address, unsigned char top_address);

short nc_func_cfg(AM_DIR_ENTRY **am_dir_h, short *am_dir_size);

short nc_mvbc_red_cfg(MvbcRedConfHeader **mvbc_red_cfg_h);

void ncapext_init(void);

#ifdef ATR_WTB
short nc_pdm_cfg(void **pdm_cfg_h, unsigned short *ftf_size);
short nc_wtb_ts_cfg(TYPE_LP_TS_CFG **ts_cfg_h);
short nc_node_cfg(unsigned short export_mode, Type_NodeDescriptor *node_descriptor);
#ifdef UMS
short nc_ums_cfg(ums_NodeConfigurationTable *ums_cfg,
				 UWTM_NODE_CONFIG_TABLE_T *uwtm_cfg, UIMCS_T_CONFIGURATION *uimcs_cfg);
#else
short nc_map_cfg(map_NodeConfigurationTable *map_cfg);
#endif
#endif

const AppStEntry *nc_get_app_st_conf(void);
void nc_get_st(const StEntry **st_h, unsigned short *st_num_p);
short nc_get_idx_mvb_ds(short ds_idx, const MvbDsEntry **mvb_ds_h, const AppDsEntry **app_ds_h);
short nc_get_idx_mvb_pv(short ds_idx, short pv_idx,
					   	const MvbDsEntry **mvb_ds_h, const PvEntry **mvb_pv_h,
					   	const AppDsEntry **app_ds_h, const AppPvEntry **app_pv_h,
   	  				   	struct STR_PV_NAME *pvn);
short nc_get_mvb_el(NcEleID mvb_ds_eid, NcEleID mvb_pv_eid,
					const MvbDsEntry **mvb_ds_h, const PvEntry **mvb_pv_h,
					const AppDsEntry **app_ds_h, const AppPvEntry **app_pv_h,
   	   				struct STR_PV_NAME *pvn);
#ifdef ATR_WTB
short nc_get_idx_wtb_pv(short ds_idx, short pv_idx,
					   	const WtbDsEntry **wtb_ds_h, const PvEntry **wtb_pv_h,
					   	TYPE_PV_NAME_48 *pvn);
short nc_get_wtb_el(NcEleID wtb_ds_eid, NcEleID wtb_pv_eid,
					const WtbDsEntry **wtb_ds_h, const PvEntry **wtb_pv_h);


                    
#endif


/*=============================================================================================*/

#endif

