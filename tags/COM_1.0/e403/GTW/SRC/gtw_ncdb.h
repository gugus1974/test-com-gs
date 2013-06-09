/*==============================================================================================*/
/* Gateway NCDB telegram definitions                                                            */
/*==============================================================================================*/

/*==============================================================================================*/
/* Stations definitions */

#define GTWM_ST_ID 0xF1000001	/* Gateway TCN Master */
#define GTWS_ST_ID 0xF1000002	/* Gateway TCN Slave  */


/*==============================================================================================*/
/* Public definitions */

/*----------------------------------------------------------------------------------------------*/
/* Datasets id definitions */

#define GTW_DSO_ID 0xE1000001	/* output                      */
#define GTW_DSI_ID 0xE1000002	/* input                       */
#define GTW_DSM_ID 0xE1000003	/* from GTWM to GTWS (private) */
#define GTW_DSS_ID 0xE1000004	/* from GTWS to GTWM (private) */
#define GTW_VID_ID 0xE1000005	/* outup: vehicle id in composition */


/*----------------------------------------------------------------------------------------------*/
/* Process variables id definitions */

/* output 8130000 - 8139900     */


/* output 81001 - 81034*/
#define GTW_PVO_UWTM_CONFLICT_ID        0x08102600
#define GTW_PVO_UWTM_OPMODE_ID          0x08100500
#define GTW_PVO_UWTM_STATE_ID           0x08100600
#define GTW_PVO_UWTM_LEADING_ID         0x08102700
#define GTW_PVO_UWTM_TOPO_COUNT_ID      0x08100200
#define GTW_PVO_UWTM_NUMBER_VEHICLES_ID 0x08100300
#define GTW_PVO_UWTM_ORIENTATION_ID     0x08100800
#define GTW_PVO_UWTM_PDM_MODE_ID        0x08100100
#define GTW_PVO_DRIVING_DIRECTION_ID 	0x08100900
#define GTW_PVO_COMM_ID              	0x08101000
#define GTW_PVO_SLEOUT_ID            	0x08101100
#define GTW_PVO_MVBON_ID             	0x08101200
#define GTW_PVO_KEY_ID               	0x08101300
#define GTW_PVO_CARBATON_ID          	0x08101400
#define GTW_PVO_LUCION_ID            	0x08101500
#define GTW_PVO_RECON_ID             	0x08101600
#define GTW_PVO_RSTRETEIN_ID         	0x08101700
#define GTW_PVO_VELOSIT_ID           	0x08101800
#define GTW_PVO_STATO_MT_ID          	0x08101900
#define GTW_PVO_RISIN1_ID            	0x08102000
#define GTW_PVO_RISIN2_ID            	0x08102100
#define GTW_PVO_RISIN3_ID            	0x08102200
#define GTW_PVO_RISIN4_ID            	0x08102300
#define GTW_PVO_RISIN5_ID            	0x08102400
#define GTW_PVO_RISIN6_ID            	0x08102500
#define GTW_PVO_RISOUT1_ID           	0x08102800
#define GTW_PVO_RISOUT2_ID           	0x08102900
#define GTW_PVO_RISOUT3_ID           	0x08103000
#define GTW_PVO_RISOUT4_ID           	0x08103100
#define GTW_PVO_OUT_VITA_ID          	0x08100400
#define GTW_PVO_STATUS_STANDBY_ID    	0x08100700
#define GTW_PVO_MVB_RED_ID    			0x08103300
#define GTW_PVO_WTB_A1_DISTURBED_ID     0x08103400
#define GTW_PVO_WTB_A2_DISTURBED_ID     0x08103500
#define GTW_PVO_WTB_B1_DISTURBED_ID     0x08103600
#define GTW_PVO_WTB_B2_DISTURBED_ID     0x08103700
#define GTW_PVO_WTB_A_LINE_HWFAULT      0x08103800
#define GTW_PVO_WTB_B_LINE_HWFAULT      0x08103900
#define GTW_PVO_CHK_ID              	0x08103200

#define GTW_PVO_NEW_1_ID			0x08104100
#define GTW_PVO_NEW_2_ID	    	0x08104200
#define GTW_PVO_NEW_3_ID			0x08104300
#define GTW_PVO_NEW_4_ID			0x08104400
#define GTW_PVO_NEW_5_ID            0x08104500
#define GTW_PVO_NEW_6_ID            0x08104600
#define GTW_PVO_NEW_7_ID			0x08104700
#define GTW_PVO_NEW_8_ID            0x08104800
#define GTW_PVO_NEW_9_ID            0x08104900
#define GTW_PVO_NEW_10_ID           0x08105000
#define GTW_PVO_NEW_11_ID           0x08105100
#define GTW_PVO_NEW_12_ID           0x08105200
#define GTW_PVO_NEW_13_ID			0x08105300
#define GTW_PVO_NEW_14_ID           0x08105400
#define GTW_PVO_NEW_15_ID           0x08105500
#define GTW_PVO_NEW_16_ID           0x08105600




/* input */
#define GTW_PVI_UWTM_OPMODE_SELECT_ID   0x08106000
#define GTW_PVI_UWTM_LEADING_REQUEST_ID 0x08106100
#define GTW_PVI_UWTM_LEADING_DIREC_ID   0x08106200
#define GTW_PVI_UWTM_SLEEP_CTRL_ID      0x08106300
#define GTW_PVI_UWTM_INAUG_CTRL_ID      0x08106400
#define GTW_PVI_UWTM_PDM_MODE_SET_ID    0x08106500
#define GTW_PVI_COMM_ID              	0x08106600
#define GTW_PVI_SLEOUT_ID            	0x08106700
#define GTW_PVI_MVBON_ID             	0x08106800
#define GTW_PVI_KEY_ID               	0x08106900
#define GTW_PVI_CARBATON_ID          	0x08107000
#define GTW_PVI_LUCION_ID            	0x08107100
#define GTW_PVI_RECON_ID             	0x08107200
#define GTW_PVI_RSTRETEIN_ID         	0x08107300
#define GTW_PVI_VELOSIT_ID           	0x08107400
#define GTW_PVI_STATO_MT_ID          	0x08107500
#define GTW_PVI_RISIN1_ID            	0x08107600
#define GTW_PVI_RISIN2_ID            	0x08107700
#define GTW_PVI_RISIN3_ID            	0x08107800
#define GTW_PVI_RISIN4_ID            	0x08107900
#define GTW_PVI_RISIN5_ID            	0x08108000
#define GTW_PVI_RISIN6_ID            	0x08108100
#define GTW_PVI_RISOUT1_ID           	0x08108200
#define GTW_PVI_RISOUT2_ID           	0x08108300
#define GTW_PVI_RISOUT3_ID           	0x08108400
#define GTW_PVI_RISOUT4_ID           	0x08108500
#define GTW_PVI_VID_ID               	0x08108800
#define GTW_PVI_VITA_ID              	0x08108600
#define GTW_PVI_CHK_ID               	0x08108700

/* from GTWM to GTWS (private) */
#define GTW_PVM_VITA_ID              	0x08110100
#define GTW_PVM_UNI1_ID              	0x08110200
#define GTW_PVM_TSTHDW_ID            	0x08110300
#define GTW_PVM_UNIM_ID              	0x08110400
#define GTW_PVM_INHBT_ID             	0x08110500
#define GTW_PVM_CHK_ID               	0x08110600

/* from GTWS to GTWM (private) */
#define GTW_PVS_VITA_ID              	0x08120100
#define GTW_PVS_COMM_ID              	0x08120200
#define GTW_PVS_SLEOUT_ID            	0x08120300
#define GTW_PVS_MVBON_ID             	0x08120400
#define GTW_PVS_KEY_ID               	0x08120500
#define GTW_PVS_CARBATON_ID          	0x08120600
#define GTW_PVS_LUCION_ID            	0x08120700
#define GTW_PVS_RECON_ID             	0x08120800
#define GTW_PVS_RSTRETEIN_ID         	0x08120900
#define GTW_PVS_VELOSIT_ID           	0x08121000
#define GTW_PVS_STATO_MT_ID          	0x08121100
#define GTW_PVS_RISIN1_ID            	0x08121200
#define GTW_PVS_RISIN2_ID            	0x08121300
#define GTW_PVS_RISIN3_ID            	0x08121400
#define GTW_PVS_RISIN4_ID            	0x08121500
#define GTW_PVS_RISIN5_ID            	0x08121600
#define GTW_PVS_RISIN6_ID            	0x08121700
#define GTW_PVS_UNI1_ID              	0x08121800
#define GTW_PVS_TSTHDW_ID            	0x08121900
#define GTW_PVS_UNIM_ID              	0x08122000
#define GTW_PVS_INHBT_ID             	0x08122100
#define GTW_PVS_CHK_ID               	0x08122200

/* from GTWM Vehicle ID */

#define GTW_VID_LOCO1_B0     	            0x08131000
#define GTW_VID_LOCO1_B1     	            0x08131100
#define GTW_VID_LOCO1_B2     	            0x08131200
#define GTW_VID_LOCO1_B3     	            0x08131300
#define GTW_VID_LOCO2_B0     	            0x08132000
#define GTW_VID_LOCO2_B1     	            0x08132100
#define GTW_VID_LOCO2_B2     	            0x08132200
#define GTW_VID_LOCO2_B3     	            0x08132300
#define GTW_VID_LOCO3_B0     	            0x08133000
#define GTW_VID_LOCO3_B1     	            0x08133100
#define GTW_VID_LOCO3_B2     	            0x08133200
#define GTW_VID_LOCO3_B3     	            0x08133300
#define GTW_VID_LOCO4_B0     	            0x08134000
#define GTW_VID_LOCO4_B1     	            0x08134100
#define GTW_VID_LOCO4_B2     	            0x08134200
#define GTW_VID_LOCO4_B3     	            0x08134300
#define GTW_VID_LIFE                     	0x08130500
#define GTW_VID_CHK_ID                  	0x08130600

/*----------------------------------------------------------------------------------------------*/
/* Process variables values definitions */

/* check variables */
#define GTW_CHK_ERROR		0
#define GTW_CHK_OK			1
#define GTW_CHK_FORCED		2
#define GTW_CHK_UNDEFINED	3

/* ANTIVALENT2 digital signals */
#define GTW_A2_SIGNAL_ABSENT	0
#define GTW_A2_SIGNAL_UNDEF		0
#define GTW_A2_SIGNAL_OFF		1
#define GTW_A2_SIGNAL_ON		2

/* BOOLEAN1 digital signals */
#define GTW_B1_SIGNAL_OFF	0
#define GTW_B1_SIGNAL_ON	1

/* GTW_PVO_UWTM_CONFLICT */
#define GTW_UWTM_NOCONFLICT	0
#define GTW_UWTM_CONFLICT	1

/* GTW_PVO_UWTM_OPMODE */
#define GTW_UWTM_OPMODE_SLAVE		0
#define GTW_UWTM_OPMODE_STRONG		1
#define GTW_UWTM_OPMODE_WEAK_SLAVE	2
#define GTW_UWTM_OPMODE_WEAK_MASTER	3
#define GTW_UWTM_OPMODE_PASSIVE		4

/* GTW_PVO_UWTM_STATE */
#define GTW_UWTM_STATE_IDLE			0
#define GTW_UWTM_STATE_PASSIVE		1
#define GTW_UWTM_STATE_REGULAR		2
#define GTW_UWTM_STATE_RESTRICTED	3

/* GTW_PVO_UWTM_LEADING */
#define GTW_UWTM_NOT_LEADING	0
#define GTW_UWTM_LEADING		1

/* GTW_PVO_UWTM_ORIENTATION */
#define GTW_UWTM_INVERSE		0
#define GTW_UWTM_NOT_INVERSE	1

/* GTW_PVO_DRIVING_DIRECTION */
#define GTW_DRIVING_DIRECTION_UNKNOWN	0
#define GTW_DRIVING_DIRECTION_DIR1		1
#define GTW_DRIVING_DIRECTION_DIR2		2
#define GTW_DRIVING_DIRECTION_ERROR		3

/* GTW_PVO_STATUS_STANDBY */
#define GTW_STANDBY_ABSENT		0
#define GTW_STANDBY_READY		1
#define GTW_STANDBY_FAILURE_A	2
#define GTW_STANDBY_FAILURE_B	3

/* GTW_PVO_MVB_RED */
#define GTW_MVB_RED_OK			0
#define GTW_MVB_RED_FAIL_A		1
#define GTW_MVB_RED_FAIL_B		2
#define GTW_MVB_RED_FAIL		3
#define GTW_MVB_RED_UNKNOWN		4

/* GTW_PVI_UWTM_OPMODE_SELECT */
#define GTW_UWTM_PASSIVE	0
#define GTW_UWTM_SLAVE		1
#define GTW_UWTM_WEAK		2
#define GTW_UWTM_STRONG		3
#define GTW_UWTM_UNDEFINED	15

/* GTW_PVI_UWTM_LEADING_REQUEST */
#define GTW_UWTM_NOT_LEADING	0
#define GTW_UWTM_LEADING		1

/* GTW_PVI_UWTM_LEADING_DIREC */
#define GTW_UWTM_DIR1	0
#define GTW_UWTM_DIR2	1

/* GTW_PVI_UWTM_SLEEP_CTRL */
#define GTW_UWTM_NOSLEEP	0
#define GTW_UWTM_SLEEP		1

/* GTW_PVI_UWTM_INAUG_CTRL */
#define GTW_UWTM_ALLOWED	0
#define GTW_UWTM_INHIBITED	1


/*==============================================================================================*/
/* Sample instances */

/*----------------------------------------------------------------------------------------------*/

#define GTW_DSO                                                                            \
	{ GTW_PVO_UWTM_CONFLICT_ID,         NC_BOOLEAN1,    8*11+0, GTW_PVO_CHK_ID }, /* 01 */ \
	{ GTW_PVO_UWTM_OPMODE_ID,           NC_ENUM4,       8* 5+0, GTW_PVO_CHK_ID }, /* 02 */ \
	{ GTW_PVO_UWTM_STATE_ID,            NC_ENUM4,       8* 5+4, GTW_PVO_CHK_ID }, /* 03 */ \
	{ GTW_PVO_UWTM_LEADING_ID,          NC_BOOLEAN1,    8*11+1, GTW_PVO_CHK_ID }, /* 04 */ \
	{ GTW_PVO_UWTM_TOPO_COUNT_ID,       NC_UNSIGNED8,   8* 2+0, GTW_PVO_CHK_ID }, /* 05 */ \
	{ GTW_PVO_UWTM_NUMBER_VEHICLES_ID,  NC_UNSIGNED8,   8* 3+0, GTW_PVO_CHK_ID }, /* 06 */ \
	{ GTW_PVO_UWTM_ORIENTATION_ID,      NC_ANTIVALENT2, 8* 6+4, GTW_PVO_CHK_ID }, /* 07 */ \
	{ GTW_PVO_UWTM_PDM_MODE_ID,         NC_UNSIGNED16,  8* 0+0, GTW_PVO_CHK_ID }, /* 08 */ \
	{ GTW_PVO_DRIVING_DIRECTION_ID,     NC_ANTIVALENT2, 8* 6+6, GTW_PVO_CHK_ID }, /* 09 */ \
	{ GTW_PVO_COMM_ID,                  NC_ANTIVALENT2, 8* 7+0, GTW_PVO_CHK_ID }, /* 10 */ \
	{ GTW_PVO_SLEOUT_ID,                NC_ANTIVALENT2, 8* 7+2, GTW_PVO_CHK_ID }, /* 11 */ \
	{ GTW_PVO_MVBON_ID,                 NC_ANTIVALENT2, 8* 7+4, GTW_PVO_CHK_ID }, /* 12 */ \
	{ GTW_PVO_KEY_ID,                   NC_ANTIVALENT2, 8* 7+6, GTW_PVO_CHK_ID }, /* 13 */ \
	{ GTW_PVO_CARBATON_ID,              NC_ANTIVALENT2, 8* 8+0, GTW_PVO_CHK_ID }, /* 14 */ \
	{ GTW_PVO_LUCION_ID,                NC_ANTIVALENT2, 8* 8+2, GTW_PVO_CHK_ID }, /* 15 */ \
	{ GTW_PVO_RECON_ID,                 NC_ANTIVALENT2, 8* 8+4, GTW_PVO_CHK_ID }, /* 16 */ \
	{ GTW_PVO_RSTRETEIN_ID,             NC_ANTIVALENT2, 8* 8+6, GTW_PVO_CHK_ID }, /* 17 */ \
	{ GTW_PVO_VELOSIT_ID,               NC_ANTIVALENT2, 8* 9+0, GTW_PVO_CHK_ID }, /* 18 */ \
	{ GTW_PVO_STATO_MT_ID,              NC_ANTIVALENT2, 8* 9+2, GTW_PVO_CHK_ID }, /* 19 */ \
	{ GTW_PVO_RISIN1_ID,                NC_ANTIVALENT2, 8* 9+4, GTW_PVO_CHK_ID }, /* 20 */ \
	{ GTW_PVO_RISIN2_ID,                NC_ANTIVALENT2, 8* 9+6, GTW_PVO_CHK_ID }, /* 21 */ \
	{ GTW_PVO_RISIN3_ID,                NC_ANTIVALENT2, 8*10+0, GTW_PVO_CHK_ID }, /* 22 */ \
	{ GTW_PVO_RISIN4_ID,                NC_ANTIVALENT2, 8*10+2, GTW_PVO_CHK_ID }, /* 23 */ \
	{ GTW_PVO_RISIN5_ID,                NC_ANTIVALENT2, 8*10+4, GTW_PVO_CHK_ID }, /* 24 */ \
	{ GTW_PVO_RISIN6_ID,                NC_ANTIVALENT2, 8*10+6, GTW_PVO_CHK_ID }, /* 25 */ \
	{ GTW_PVO_RISOUT1_ID,               NC_BOOLEAN1,    8*11+2, GTW_PVO_CHK_ID }, /* 26 */ \
	{ GTW_PVO_RISOUT2_ID,               NC_BOOLEAN1,    8*11+3, GTW_PVO_CHK_ID }, /* 27 */ \
	{ GTW_PVO_RISOUT3_ID,               NC_BOOLEAN1,    8*11+4, GTW_PVO_CHK_ID }, /* 28 */ \
	{ GTW_PVO_RISOUT4_ID,               NC_BOOLEAN1,    8*11+5, GTW_PVO_CHK_ID }, /* 29 */ \
	{ GTW_PVO_OUT_VITA_ID,              NC_UNSIGNED8,   8* 4+0, GTW_PVO_CHK_ID }, /* 30 */ \
	{ GTW_PVO_STATUS_STANDBY_ID,        NC_ENUM4,       8* 6+0, GTW_PVO_CHK_ID }, /* 31 */ \
	{ GTW_PVO_MVB_RED_ID,               NC_ENUM4,       8*12+0, GTW_PVO_CHK_ID }, /* 32 */ \
	{ GTW_PVO_WTB_A1_DISTURBED_ID,      NC_BOOLEAN1,    8*13+0, GTW_PVO_CHK_ID }, /* 33 */ \
	{ GTW_PVO_WTB_B1_DISTURBED_ID,      NC_BOOLEAN1,    8*13+1, GTW_PVO_CHK_ID }, /* 34 */ \
	{ GTW_PVO_WTB_A2_DISTURBED_ID,      NC_BOOLEAN1,    8*13+2, GTW_PVO_CHK_ID }, /* 35 */ \
	{ GTW_PVO_WTB_B2_DISTURBED_ID,      NC_BOOLEAN1,    8*13+3, GTW_PVO_CHK_ID }, /* 36 */ \
	{ GTW_PVO_WTB_A_LINE_HWFAULT,       NC_BOOLEAN1,    8*13+6, GTW_PVO_CHK_ID }, /* 37 */ \
	{ GTW_PVO_WTB_B_LINE_HWFAULT,       NC_BOOLEAN1,    8*13+7, GTW_PVO_CHK_ID }, /* 38 */ \
	{ GTW_PVO_NEW_1_ID,               	NC_UNSIGNED8,   8*15+0, GTW_PVO_CHK_ID }, /* 39 */ \
	{ GTW_PVO_NEW_2_ID,               	NC_UNSIGNED8,   8*16+0, GTW_PVO_CHK_ID }, /* 40 */ \
	{ GTW_PVO_NEW_3_ID,               	NC_UNSIGNED8,   8*17+0, GTW_PVO_CHK_ID }, /* 41 */ \
	{ GTW_PVO_NEW_4_ID,               	NC_UNSIGNED8,   8*18+0, GTW_PVO_CHK_ID }, /* 42 */ \
	{ GTW_PVO_NEW_5_ID,               	NC_UNSIGNED8,   8*19+0, GTW_PVO_CHK_ID }, /* 43 */ \
	{ GTW_PVO_NEW_6_ID,               	NC_UNSIGNED8,   8*20+0, GTW_PVO_CHK_ID }, /* 44 */ \
	{ GTW_PVO_NEW_7_ID,               	NC_UNSIGNED8,   8*21+0, GTW_PVO_CHK_ID }, /* 45 */ \
	{ GTW_PVO_NEW_8_ID,               	NC_UNSIGNED8,   8*22+0, GTW_PVO_CHK_ID }, /* 46 */ \
	{ GTW_PVO_NEW_9_ID,               	NC_UNSIGNED8,   8*23+0, GTW_PVO_CHK_ID }, /* 47 */ \
	{ GTW_PVO_NEW_10_ID,               	NC_UNSIGNED8,   8*24+0, GTW_PVO_CHK_ID }, /* 48 */ \
	{ GTW_PVO_NEW_11_ID,               	NC_UNSIGNED8,   8*25+0, GTW_PVO_CHK_ID }, /* 49 */ \
	{ GTW_PVO_NEW_12_ID,               	NC_UNSIGNED8,   8*26+0, GTW_PVO_CHK_ID }, /* 50 */ \
	{ GTW_PVO_NEW_13_ID,               	NC_UNSIGNED8,   8*27+0, GTW_PVO_CHK_ID }, /* 51 */ \
	{ GTW_PVO_NEW_14_ID,               	NC_UNSIGNED8,   8*28+0, GTW_PVO_CHK_ID }, /* 52 */ \
	{ GTW_PVO_NEW_15_ID,               	NC_UNSIGNED8,   8*29+0, GTW_PVO_CHK_ID }, /* 53 */ \
	{ GTW_PVO_NEW_16_ID,               	NC_UNSIGNED8,   8*30+0, GTW_PVO_CHK_ID }, /* 54 */ \
	{ GTW_PVO_CHK_ID,                   NC_ANTIVALENT2, 8*31+6, 0              }  /* 55 */
#define GTW_DSO_PV_NUM 55
#define GTW_DSO_BYTES NC_MVB_DS_32_BYTES

#define GTW_DSI                                                                            \
	{ GTW_PVI_UWTM_OPMODE_SELECT_ID,    NC_ENUM4,       8* 3+0, GTW_PVI_CHK_ID }, /*  1 */ \
	{ GTW_PVI_UWTM_LEADING_REQUEST_ID,  NC_BOOLEAN1,    8* 8+4, GTW_PVI_CHK_ID }, /*  2 */ \
	{ GTW_PVI_UWTM_LEADING_DIREC_ID,    NC_BOOLEAN1,    8* 8+5, GTW_PVI_CHK_ID }, /*  3 */ \
	{ GTW_PVI_UWTM_SLEEP_CTRL_ID,       NC_BOOLEAN1,    8* 8+6, GTW_PVI_CHK_ID }, /*  4 */ \
	{ GTW_PVI_UWTM_INAUG_CTRL_ID,       NC_BOOLEAN1,    8* 8+7, GTW_PVI_CHK_ID }, /*  5 */ \
	{ GTW_PVI_UWTM_PDM_MODE_SET_ID,     NC_UNSIGNED16,  8* 0+0, GTW_PVI_CHK_ID }, /*  6 */ \
	{ GTW_PVI_COMM_ID,                  NC_ANTIVALENT2, 8* 3+4, GTW_PVI_CHK_ID }, /*  7 */ \
	{ GTW_PVI_SLEOUT_ID,                NC_ANTIVALENT2, 8* 3+6, GTW_PVI_CHK_ID }, /*  8 */ \
	{ GTW_PVI_MVBON_ID,                 NC_ANTIVALENT2, 8* 4+0, GTW_PVI_CHK_ID }, /*  9 */ \
	{ GTW_PVI_KEY_ID,                   NC_ANTIVALENT2, 8* 4+2, GTW_PVI_CHK_ID }, /* 10 */ \
	{ GTW_PVI_CARBATON_ID,              NC_ANTIVALENT2, 8* 4+4, GTW_PVI_CHK_ID }, /* 11 */ \
	{ GTW_PVI_LUCION_ID,                NC_ANTIVALENT2, 8* 4+6, GTW_PVI_CHK_ID }, /* 12 */ \
	{ GTW_PVI_RECON_ID,                 NC_ANTIVALENT2, 8* 5+0, GTW_PVI_CHK_ID }, /* 13 */ \
	{ GTW_PVI_RSTRETEIN_ID,             NC_ANTIVALENT2, 8* 5+2, GTW_PVI_CHK_ID }, /* 14 */ \
	{ GTW_PVI_VELOSIT_ID,               NC_ANTIVALENT2, 8* 5+4, GTW_PVI_CHK_ID }, /* 15 */ \
	{ GTW_PVI_STATO_MT_ID,              NC_ANTIVALENT2, 8* 5+6, GTW_PVI_CHK_ID }, /* 16 */ \
	{ GTW_PVI_RISIN1_ID,                NC_ANTIVALENT2, 8* 6+0, GTW_PVI_CHK_ID }, /* 17 */ \
	{ GTW_PVI_RISIN2_ID,                NC_ANTIVALENT2, 8* 6+2, GTW_PVI_CHK_ID }, /* 18 */ \
	{ GTW_PVI_RISIN3_ID,                NC_ANTIVALENT2, 8* 6+4, GTW_PVI_CHK_ID }, /* 19 */ \
	{ GTW_PVI_RISIN4_ID,                NC_ANTIVALENT2, 8* 6+6, GTW_PVI_CHK_ID }, /* 20 */ \
	{ GTW_PVI_RISIN5_ID,                NC_ANTIVALENT2, 8* 7+0, GTW_PVI_CHK_ID }, /* 21 */ \
	{ GTW_PVI_RISIN6_ID,                NC_ANTIVALENT2, 8* 7+2, GTW_PVI_CHK_ID }, /* 22 */ \
	{ GTW_PVI_RISOUT1_ID,               NC_ANTIVALENT2, 8* 7+4, GTW_PVI_CHK_ID }, /* 23 */ \
	{ GTW_PVI_RISOUT2_ID,               NC_ANTIVALENT2, 8* 7+6, GTW_PVI_CHK_ID }, /* 24 */ \
	{ GTW_PVI_RISOUT3_ID,               NC_ANTIVALENT2, 8* 8+0, GTW_PVI_CHK_ID }, /* 25 */ \
	{ GTW_PVI_RISOUT4_ID,               NC_ANTIVALENT2, 8* 8+2, GTW_PVI_CHK_ID }, /* 26 */ \
	{ GTW_PVI_VID_ID,                   NC_UNSIGNED8,   8* 9+0, GTW_PVI_CHK_ID }, /* 27 */ \
	{ GTW_PVI_VITA_ID,                  NC_UNSIGNED8,   8* 2+0, GTW_PVI_CHK_ID }, /* 28 */ \
	{ GTW_PVI_CHK_ID,                   NC_ANTIVALENT2, 8*15+6, 0              }  /* 29 */
#define GTW_DSI_PV_NUM 29
#define GTW_DSI_BYTES NC_MVB_DS_16_BYTES

#define GTW_DSM                                                                            \
	{ GTW_PVM_VITA_ID,                  NC_UNSIGNED8,   8* 0+0, GTW_PVM_CHK_ID }, /*  1 */ \
	{ GTW_PVM_UNI1_ID,                  NC_BOOLEAN1,    8* 1+0, GTW_PVM_CHK_ID }, /*  2 */ \
	{ GTW_PVM_TSTHDW_ID,                NC_BOOLEAN1,    8* 1+1, GTW_PVM_CHK_ID }, /*  3 */ \
	{ GTW_PVM_UNIM_ID,                  NC_BOOLEAN1,    8* 1+2, GTW_PVM_CHK_ID }, /*  4 */ \
	{ GTW_PVM_INHBT_ID,                 NC_BOOLEAN1,    8* 1+3, GTW_PVM_CHK_ID }, /*  5 */ \
	{ GTW_PVM_CHK_ID,                   NC_ANTIVALENT2, 8* 1+6, 0              }, /*  6 */
#define GTW_DSM_PV_NUM 6
#define GTW_DSM_BYTES NC_MVB_DS_02_BYTES

#define GTW_DSS                                                                            \
	{ GTW_PVS_VITA_ID,                  NC_UNSIGNED8,   8* 0+0, GTW_PVS_CHK_ID }, /*  1 */ \
	{ GTW_PVS_COMM_ID,                  NC_ANTIVALENT2, 8* 1+0, GTW_PVS_CHK_ID }, /*  2 */ \
	{ GTW_PVS_SLEOUT_ID,                NC_ANTIVALENT2, 8* 1+2, GTW_PVS_CHK_ID }, /*  3 */ \
	{ GTW_PVS_MVBON_ID,                 NC_ANTIVALENT2, 8* 1+4, GTW_PVS_CHK_ID }, /*  4 */ \
	{ GTW_PVS_KEY_ID,                   NC_ANTIVALENT2, 8* 1+6, GTW_PVS_CHK_ID }, /*  5 */ \
	{ GTW_PVS_CARBATON_ID,              NC_ANTIVALENT2, 8* 2+0, GTW_PVS_CHK_ID }, /*  6 */ \
	{ GTW_PVS_LUCION_ID,                NC_ANTIVALENT2, 8* 2+2, GTW_PVS_CHK_ID }, /*  7 */ \
	{ GTW_PVS_RECON_ID,                 NC_ANTIVALENT2, 8* 2+4, GTW_PVS_CHK_ID }, /*  8 */ \
	{ GTW_PVS_RSTRETEIN_ID,             NC_ANTIVALENT2, 8* 2+6, GTW_PVS_CHK_ID }, /*  9 */ \
	{ GTW_PVS_VELOSIT_ID,               NC_ANTIVALENT2, 8* 3+0, GTW_PVS_CHK_ID }, /* 10 */ \
	{ GTW_PVS_STATO_MT_ID,              NC_ANTIVALENT2, 8* 3+2, GTW_PVS_CHK_ID }, /* 11 */ \
	{ GTW_PVS_RISIN1_ID,                NC_ANTIVALENT2, 8* 3+4, GTW_PVS_CHK_ID }, /* 12 */ \
	{ GTW_PVS_RISIN2_ID,                NC_ANTIVALENT2, 8* 3+6, GTW_PVS_CHK_ID }, /* 13 */ \
	{ GTW_PVS_RISIN3_ID,                NC_ANTIVALENT2, 8* 4+0, GTW_PVS_CHK_ID }, /* 14 */ \
	{ GTW_PVS_RISIN4_ID,                NC_ANTIVALENT2, 8* 4+2, GTW_PVS_CHK_ID }, /* 15 */ \
	{ GTW_PVS_RISIN5_ID,                NC_ANTIVALENT2, 8* 4+4, GTW_PVS_CHK_ID }, /* 16 */ \
	{ GTW_PVS_RISIN6_ID,                NC_ANTIVALENT2, 8* 4+6, GTW_PVS_CHK_ID }, /* 17 */ \
	{ GTW_PVS_UNI1_ID,                  NC_BOOLEAN1,    8* 5+0, GTW_PVS_CHK_ID }, /* 18 */ \
	{ GTW_PVS_TSTHDW_ID,                NC_BOOLEAN1,    8* 5+1, GTW_PVS_CHK_ID }, /* 19 */ \
	{ GTW_PVS_UNIM_ID,                  NC_BOOLEAN1,    8* 5+2, GTW_PVS_CHK_ID }, /* 20 */ \
	{ GTW_PVS_INHBT_ID,                 NC_BOOLEAN1,    8* 5+3, GTW_PVS_CHK_ID }, /* 21 */ \
	{ GTW_PVS_CHK_ID,                   NC_ANTIVALENT2, 8* 7+6, 0              }  /* 22 */
#define GTW_DSS_PV_NUM 22
#define GTW_DSS_BYTES NC_MVB_DS_08_BYTES

#define GTW_VID                                                                            \
	{ GTW_VID_LOCO1_B0 ,                NC_UNSIGNED8,   8*  0+0 , GTW_VID_CHK_ID }, /* 01 */ \
	{ GTW_VID_LOCO1_B1 ,                NC_UNSIGNED8,   8*  1+0 , GTW_VID_CHK_ID }, /* 02 */ \
	{ GTW_VID_LOCO1_B2 ,                NC_UNSIGNED8,   8*  2+0 , GTW_VID_CHK_ID }, /* 03 */ \
	{ GTW_VID_LOCO1_B3 ,                NC_UNSIGNED8,   8*  3+0 , GTW_VID_CHK_ID }, /* 04 */ \
	{ GTW_VID_LOCO2_B0 ,                NC_UNSIGNED8,   8*  4+0 , GTW_VID_CHK_ID }, /* 05 */ \
	{ GTW_VID_LOCO2_B1 ,                NC_UNSIGNED8,   8*  5+0 , GTW_VID_CHK_ID }, /* 06 */ \
	{ GTW_VID_LOCO2_B2 ,                NC_UNSIGNED8,   8*  6+0 , GTW_VID_CHK_ID }, /* 07 */ \
	{ GTW_VID_LOCO2_B3 ,                NC_UNSIGNED8,   8*  7+0 , GTW_VID_CHK_ID }, /* 08 */ \
	{ GTW_VID_LOCO3_B0 ,                NC_UNSIGNED8,   8*  8+0 , GTW_VID_CHK_ID }, /* 09 */ \
	{ GTW_VID_LOCO3_B1 ,                NC_UNSIGNED8,   8*  9+0 , GTW_VID_CHK_ID }, /* 10 */ \
	{ GTW_VID_LOCO3_B2 ,                NC_UNSIGNED8,   8* 10+0 , GTW_VID_CHK_ID }, /* 11 */ \
	{ GTW_VID_LOCO3_B3 ,                NC_UNSIGNED8,   8* 11+0 , GTW_VID_CHK_ID }, /* 12 */ \
	{ GTW_VID_LOCO4_B0 ,                NC_UNSIGNED8,   8* 12+0 , GTW_VID_CHK_ID }, /* 13 */ \
	{ GTW_VID_LOCO4_B1 ,                NC_UNSIGNED8,   8* 13+0 , GTW_VID_CHK_ID }, /* 14 */ \
	{ GTW_VID_LOCO4_B2 ,                NC_UNSIGNED8,   8* 14+0 , GTW_VID_CHK_ID }, /* 15 */ \
	{ GTW_VID_LOCO4_B3 ,                NC_UNSIGNED8,   8* 15+0 , GTW_VID_CHK_ID }, /* 16 */ \
	{ GTW_VID_LIFE  ,                   NC_ENUM4 ,      8* 31+0, GTW_VID_CHK_ID },  /* 17 */ \
	{ GTW_VID_CHK_ID,                   NC_ANTIVALENT2, 8* 31+6, 0              }   /* 18 */
#define GTW_VID_PV_NUM 18
#define GTW_VID_BYTES NC_MVB_DS_32_BYTES


/*==============================================================================================*/

