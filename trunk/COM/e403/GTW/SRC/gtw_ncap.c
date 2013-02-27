/*=============================================================================================*/
/* Gateway NCAP                                                 					           */
/* Implementation file (gtw_ncap.c)											                   */
/*																					           */
/* Version 1.0																		           */
/*																					           */
/* (c)1998 Ansaldo Trasporti														           */
/* Written by Giuseppe Carnevale													           */
/*=============================================================================================*/


/*==============================================================================================*/
/* ### Do not touch this section ### */

/* include the interface */
#include "conf_tcn.h"

/* void the extern definition to cause the allocation */
#define EXTERN


/*==============================================================================================*/
/* Include the definitions */

#include "gtw_ncap.h"


/*==============================================================================================*/
/* Definitions */

/*----------------------------------------------------------------------------------------------*/
/* Datasets' process variables */

static const AppPvEntry pv_gtw_dso[] = {
	{GTW_PVO_UWTM_CONFLICT_ID,			"uwtm_conflict",		&gtw_dso.uwtm_conflict			},
	{GTW_PVO_UWTM_OPMODE_ID,			"uwtm_opmode",			&gtw_dso.uwtm_opmode			},
	{GTW_PVO_UWTM_STATE_ID,				"uwtm_state",			&gtw_dso.uwtm_state				},
	{GTW_PVO_UWTM_LEADING_ID,			"uwtm_leading",			&gtw_dso.uwtm_leading			},
	{GTW_PVO_UWTM_TOPO_COUNT_ID,		"uwtm_topo_count",		&gtw_dso.uwtm_topo_count		},
	{GTW_PVO_UWTM_NUMBER_VEHICLES_ID,	"uwtm_number_vehicles",	&gtw_dso.uwtm_number_vehicles	},
	{GTW_PVO_UWTM_ORIENTATION_ID,		"uwtm_orientation",		&gtw_dso.uwtm_orientation		},
	{GTW_PVO_UWTM_PDM_MODE_ID,			"uwtm_pdm_mode",		&gtw_dso.uwtm_pdm_mode			},
	{GTW_PVO_DRIVING_DIRECTION_ID,		"driving_direction",	&gtw_dso.driving_direction		},
	{GTW_PVO_COMM_ID,					"comm",					&gtw_dso.comm					},
	{GTW_PVO_SLEOUT_ID,					"sleout",				&gtw_dso.sleout					},
	{GTW_PVO_MVBON_ID,					"mvbon",				&gtw_dso.mvbon					},
	{GTW_PVO_KEY_ID,					"key",					&gtw_dso.key					},
	{GTW_PVO_CARBATON_ID,				"carbaton",				&gtw_dso.carbaton				},
	{GTW_PVO_LUCION_ID,					"lucion",				&gtw_dso.lucion					},
	{GTW_PVO_RECON_ID,					"recon",				&gtw_dso.recon					},
	{GTW_PVO_RSTRETEIN_ID,				"rstretein",			&gtw_dso.rstretein				},
	{GTW_PVO_VELOSIT_ID,				"velosit",				&gtw_dso.velosit				},
	{GTW_PVO_STATO_MT_ID,				"stato_mt",				&gtw_dso.stato_mt				},
	{GTW_PVO_RISIN1_ID,					"risin1",				&gtw_dso.risin1					},
	{GTW_PVO_RISIN2_ID,					"risin2",				&gtw_dso.risin2					},
	{GTW_PVO_RISIN3_ID,					"risin3",				&gtw_dso.risin3					},
	{GTW_PVO_RISIN4_ID,					"risin4",				&gtw_dso.risin4					},
	{GTW_PVO_RISIN5_ID,					"risin5",				&gtw_dso.risin5					},
	{GTW_PVO_RISIN6_ID,					"risin6",				&gtw_dso.risin6					},
	{GTW_PVO_RISOUT1_ID,				"risout1",				&gtw_dso.risout1				},
	{GTW_PVO_RISOUT2_ID,				"risout2",				&gtw_dso.risout2				},
	{GTW_PVO_RISOUT3_ID,				"risout3",				&gtw_dso.risout3				},
	{GTW_PVO_RISOUT4_ID,				"risout4",				&gtw_dso.risout4				},
	{GTW_PVO_OUT_VITA_ID,				"out_vita",				&gtw_dso.out_vita				},
	{GTW_PVO_STATUS_STANDBY_ID,			"status_standby",		&gtw_dso.status_standby			},
	{GTW_PVO_MVB_RED_ID,			    "mvb_red",		        &gtw_dso.mvb_red     			},
	{GTW_PVO_WTB_A1_DISTURBED_ID,   "WTB-A1 disturbed",		&gtw_dso.line_A1_disturbed   	},
	{GTW_PVO_WTB_B1_DISTURBED_ID,   "WTB-B1 disturbed",		&gtw_dso.line_B1_disturbed   	},
	{GTW_PVO_WTB_A2_DISTURBED_ID,   "WTB-A2 disturbed",		&gtw_dso.line_A2_disturbed   	},
	{GTW_PVO_WTB_B2_DISTURBED_ID,   "WTB-B2 disturbed",		&gtw_dso.line_B2_disturbed   	},
	{GTW_PVO_WTB_A_LINE_HWFAULT,	"Line A HW Fault",	    &gtw_dso.line_A_HW_Fault 	    },
	{GTW_PVO_WTB_B_LINE_HWFAULT,	"Line B HW Fault",	    &gtw_dso.line_B_HW_Fault 	    },
    {GTW_PVO_NEW_1_ID	,   "uic",                          &gtw_dso.uic[0]       },// non usati
    {GTW_PVO_NEW_2_ID	,   "uic",                          &gtw_dso.uic[1]       },// non usati
    {GTW_PVO_NEW_3_ID	,   "uic",                          &gtw_dso.uic[2]      },// non usati
    {GTW_PVO_NEW_4_ID	,   "uic",                          &gtw_dso.uic[3]        },// non usati
    {GTW_PVO_NEW_5_ID   ,   "log_1",                        &gtw_dso.logico[1]      },
    {GTW_PVO_NEW_6_ID   ,   "log_2",                        &gtw_dso.logico[2]      },
    {GTW_PVO_NEW_7_ID	,   "log_3",                        &gtw_dso.logico[3]      },
    {GTW_PVO_NEW_8_ID   ,   "log_4",                        &gtw_dso.logico[4]      },
    {GTW_PVO_NEW_9_ID   ,   "mt_01",                        &gtw_dso.vd_mt[1]       },
    {GTW_PVO_NEW_10_ID  ,   "mt_02",                        &gtw_dso.vd_mt[2]       },
    {GTW_PVO_NEW_11_ID  ,   "mt_03",                        &gtw_dso.vd_mt[3]       },
    {GTW_PVO_NEW_12_ID  ,   "mt_04",                        &gtw_dso.vd_mt[4]       },
    {GTW_PVO_NEW_13_ID  ,   "nadi_valid",                   &gtw_dso.nadi_valid     },
    {GTW_PVO_NEW_14_ID  ,   "vei_app",                      &gtw_dso.v_app          },
    {GTW_PVO_NEW_15_ID  ,   "Master_is_Z1",                 &gtw_dso.Master_is_Z1   },
    {GTW_PVO_NEW_16_ID  ,   "spare2",                       &gtw_dso.spare2         },
	{GTW_PVO_CHK_ID     ,	"chk",				            &gtw_dso.chk			},
	0
};

static const AppPvEntry pv_gtw_dsi[] = {
	{GTW_PVI_UWTM_OPMODE_SELECT_ID,		"uwtm_opmode_select",	&gtw_dsi.uwtm_opmode_select		},
	{GTW_PVI_UWTM_LEADING_REQUEST_ID,	"uwtm_leading_request",	&gtw_dsi.uwtm_leading_request	},
	{GTW_PVI_UWTM_LEADING_DIREC_ID,		"uwtm_leading_direc",	&gtw_dsi.uwtm_leading_direc		},
	{GTW_PVI_UWTM_SLEEP_CTRL_ID,		"uwtm_sleep_ctrl",		&gtw_dsi.uwtm_sleep_ctrl		},
	{GTW_PVI_UWTM_INAUG_CTRL_ID,		"uwtm_inaug_ctrl",		&gtw_dsi.uwtm_inaug_ctrl		},
	{GTW_PVI_UWTM_PDM_MODE_SET_ID,		"uwtm_pdm_mode_set",	&gtw_dsi.uwtm_pdm_mode_set		},
	{GTW_PVI_COMM_ID,					"comm",					&gtw_dsi.comm					},
	{GTW_PVI_SLEOUT_ID,					"sleout",				&gtw_dsi.sleout					},
	{GTW_PVI_MVBON_ID,					"mvbon",				&gtw_dsi.mvbon					},
	{GTW_PVI_KEY_ID,					"key",					&gtw_dsi.key					},
	{GTW_PVI_CARBATON_ID,				"carbaton",				&gtw_dsi.carbaton				},
	{GTW_PVI_LUCION_ID,					"lucion",				&gtw_dsi.lucion					},
	{GTW_PVI_RECON_ID,					"recon",				&gtw_dsi.recon					},
	{GTW_PVI_RSTRETEIN_ID,				"rstretein",			&gtw_dsi.rstretein				},
	{GTW_PVI_VELOSIT_ID,				"velosit",				&gtw_dsi.velosit				},
	{GTW_PVI_STATO_MT_ID,				"stato_mt",				&gtw_dsi.stato_mt				},
	{GTW_PVI_RISIN1_ID,					"risin1",				&gtw_dsi.risin1					},
	{GTW_PVI_RISIN2_ID,					"risin2",				&gtw_dsi.risin2					},
	{GTW_PVI_RISIN3_ID,					"risin3",				&gtw_dsi.risin3					},
	{GTW_PVI_RISIN4_ID,					"risin4",				&gtw_dsi.risin4					},
	{GTW_PVI_RISIN5_ID,					"risin5",				&gtw_dsi.risin5					},
	{GTW_PVI_RISIN6_ID,					"risin6",				&gtw_dsi.risin6					},
	{GTW_PVI_RISOUT1_ID,				"risout1",				&gtw_dsi.risout1				},
	{GTW_PVI_RISOUT2_ID,				"risout2",				&gtw_dsi.risout2				},
	{GTW_PVI_RISOUT3_ID,				"risout3",				&gtw_dsi.risout3				},
	{GTW_PVI_RISOUT4_ID,				"risout4",				&gtw_dsi.risout4				},
	{GTW_PVI_VID_ID,				    "Vid",			    	&gtw_dsi.vid				},
	{GTW_PVI_VITA_ID,					"vita",					&gtw_dsi.vita					},
	{GTW_PVI_CHK_ID,					"chk",					&gtw_dsi.chk					},
	0
};

static const AppPvEntry pv_gtw_dsm[] = {
	{GTW_PVM_VITA_ID,					"vita",					&gtw_dsm.vita					},
	{GTW_PVM_UNI1_ID,					"uni1",					&gtw_dsm.uni1					},
	{GTW_PVM_TSTHDW_ID,					"tsthdw",				&gtw_dsm.tsthdw					},
	{GTW_PVM_UNIM_ID,					"unim",					&gtw_dsm.unim					},
	{GTW_PVM_INHBT_ID,					"inhbt",				&gtw_dsm.inhbt					},
	{GTW_PVM_CHK_ID,					"chk",					&gtw_dsm.chk					},
	0
};

static const AppPvEntry pv_gtw_dss[] = {
	{GTW_PVS_VITA_ID,					"vita",					&gtw_dss.vita					},
	{GTW_PVS_COMM_ID,					"comm",					&gtw_dss.comm					},
	{GTW_PVS_SLEOUT_ID,					"sleout",				&gtw_dss.sleout					},
	{GTW_PVS_MVBON_ID,					"mvbon",				&gtw_dss.mvbon					},
	{GTW_PVS_KEY_ID,					"key",					&gtw_dss.key					},
	{GTW_PVS_CARBATON_ID,				"carbaton",				&gtw_dss.carbaton				},
	{GTW_PVS_LUCION_ID,					"lucion",				&gtw_dss.lucion					},
	{GTW_PVS_RECON_ID,					"recon",				&gtw_dss.recon					},
	{GTW_PVS_RSTRETEIN_ID,				"rstretein",			&gtw_dss.rstretein				},
	{GTW_PVS_VELOSIT_ID,				"velosit",				&gtw_dss.velosit				},
	{GTW_PVS_STATO_MT_ID,				"stato_mt",				&gtw_dss.stato_mt				},
	{GTW_PVS_RISIN1_ID,					"risin1",				&gtw_dss.risin1					},
	{GTW_PVS_RISIN2_ID,					"risin2",				&gtw_dss.risin2					},
	{GTW_PVS_RISIN3_ID,					"risin3",				&gtw_dss.risin3					},
	{GTW_PVS_RISIN4_ID,					"risin4",				&gtw_dss.risin4					},
	{GTW_PVS_RISIN5_ID,					"risin5",				&gtw_dss.risin5					},
	{GTW_PVS_RISIN6_ID,					"risin6",				&gtw_dss.risin6					},
	{GTW_PVS_UNI1_ID,					"uni1",					&gtw_dss.uni1					},
	{GTW_PVS_TSTHDW_ID,					"tsthdw",				&gtw_dss.tsthdw					},
	{GTW_PVS_UNIM_ID,					"unim",					&gtw_dss.unim					},
	{GTW_PVS_INHBT_ID,					"inhbt",				&gtw_dss.inhbt					},
	{GTW_PVS_CHK_ID,					"chk",					&gtw_dss.chk					},
	0
};
static const AppPvEntry pv_gtw_vid[] = {
	{GTW_VID_LOCO1_B0 ,			"vid[ 1]",			&gtw_vid.vid[0]           			},
	{GTW_VID_LOCO1_B1 ,			"vid[ 2]",			&gtw_vid.vid[1]           			},
	{GTW_VID_LOCO1_B2 ,			"vid[ 3]",			&gtw_vid.vid[2]           			},
	{GTW_VID_LOCO1_B3 ,			"vid[ 4]",			&gtw_vid.vid[3]           			},
	{GTW_VID_LOCO2_B0 ,			"vid[ 5]",			&gtw_vid.vid[4]           			},
	{GTW_VID_LOCO2_B1 ,			"vid[ 6]",			&gtw_vid.vid[5]           			},
	{GTW_VID_LOCO2_B2 ,			"vid[ 7]",			&gtw_vid.vid[6]           			},
	{GTW_VID_LOCO2_B3 ,			"vid[ 8]",			&gtw_vid.vid[7]           			},
	{GTW_VID_LOCO3_B0 ,			"vid[ 9]",			&gtw_vid.vid[8]           			},
	{GTW_VID_LOCO3_B1 ,			"vid[10]",			&gtw_vid.vid[9]           			},
	{GTW_VID_LOCO3_B2 ,			"vid[11]",			&gtw_vid.vid[10]           			},
	{GTW_VID_LOCO3_B3 ,			"vid[12]",			&gtw_vid.vid[11]           			},
	{GTW_VID_LOCO4_B0 ,			"vid[13]",			&gtw_vid.vid[12]           			},
	{GTW_VID_LOCO4_B1 ,			"vid[14]",			&gtw_vid.vid[13]           			},
	{GTW_VID_LOCO4_B2 ,			"vid[15]",			&gtw_vid.vid[14]           			},
	{GTW_VID_LOCO4_B3 ,			"vid[16]",			&gtw_vid.vid[15]           			},
	{GTW_VID_LIFE     ,			"life",	    		&gtw_vid.life           			},
	{GTW_VID_CHK_ID   ,			"chk",   			&gtw_vid.chk            			},
	0
};


static const AppPvEntry pv_330[] = {//SD_WITH_CHK(03,dir_loco_r3)};
       { 0x09940103, "loco_source_dir",      &dir_loco_r3.sd   },  /*  1 */  \
       { 0x09940203, "loco_source_chk",      &dir_loco_r3.chk  },  /*  2 */  \
0
};


/*----------------------------------------------------------------------------------------------*/
/* Datasets configuration */

static const AppDsEntry ds_table[] = {
	{ GTW_DSO_ID, "GTW_DSO", (void*)pv_gtw_dso, &ds_gtw_dso, &rf_gtw_dso, NC_APP_DS_TMO+NC_APP_DS_CAN_MISS },
//	{ GTW_DSI_ID, "GTW_DSI", (void*)pv_gtw_dsi, &ds_gtw_dsi, &rf_gtw_dsi, NC_APP_DS_TMO+NC_APP_DS_CAN_MISS },
//	{ GTW_DSM_ID, "GTW_DSM", (void*)pv_gtw_dsm, &ds_gtw_dsm, &rf_gtw_dsm, NC_APP_DS_TMO+NC_APP_DS_CAN_MISS },
//	{ GTW_DSS_ID, "GTW_DSS", (void*)pv_gtw_dss, &ds_gtw_dss, &rf_gtw_dss, NC_APP_DS_TMO+NC_APP_DS_CAN_MISS },
//	{ GTW_VID_ID, "GTW_VID", (void*)pv_gtw_vid, &ds_gtw_vid, &rf_gtw_vid, NC_APP_DS_TMO+NC_APP_DS_CAN_MISS },
//    { 0xE0000330,"DS 330 (SDR3_l_s)"  , (void*)pv_330,  &ds_330,  &dir_loco_r3.refresh,   NC_APP_DS_TMO},
	0
};


/*----------------------------------------------------------------------------------------------*/
/* Application configuration(s) */

const AppStEntry nc_ap_list[] = {

{
	GTWM_ST_ID,			/* st_eid */
	"GTWM",				/* name */
	32,					/* am_max_call_num */
	32,					/* am_max_inst_num */
	3000,				/* am_ms_reply_tmo */
	AM_MAX_CREDIT,		/* am_my_credit */
	(void*)ds_table		/* app_ds_list */
},

{
	GTWS_ST_ID,			/* st_eid */
	"GTWS",				/* name */
	32,					/* am_max_call_num */
	32,					/* am_max_inst_num */
	3000,				/* am_ms_reply_tmo */
	AM_MAX_CREDIT,		/* am_my_credit */
	(void*)ds_table		/* app_ds_list */
},

0

};


/*==============================================================================================*/

