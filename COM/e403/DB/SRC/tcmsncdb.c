/*=============================================================================================*/
/* E402A TCN definitions              														   */
/* Implementation file (02a_ncdb.c)															   */
/* 																							   */
/* Version 1.0																				   */
/*																							   */
/* (c)1998 Ansaldo Trasporti																   */
/*																							   */
/* Define the macro NO_GTWS to exclude it from the configuration (no redundancy in GTW)        */
/*																							   */
/* Static attributes (v.10) : 1,  3,          4,  5,  6,  29, 32                               */
/* Static attributes (v.12) : 38, 44, 45, 46, 44, 45, 46, 40, 43                               */
/* Static attributes (FS)   : 1, 2, 8, 9, 10, 11, 12, 13, 14, 15, 21, 26, 29, 30, 31, 32, 33,  */
/*                            34, 35, 36                                                       */
/*=============================================================================================*/


/*==============================================================================================*/
/* ### Do not touch this section ###                                                            */

/* include the interface                                                                        */
#include "ncdb.h"


/*==============================================================================================*/
/* Include the definitions */

#include "uic2ncdb.h"
#include "tcmsncdb.h"
#include "gtw_ncdb.h"


/*==============================================================================================*/
/*
	0xFxxxxxxx : station xxxxxxx

	0xExxxxxxx : dataset xxxxxxx

	0x0xxyyy00 : process variable xx.yyy
	0x0xxyyy0z : process variable xx.yyy/z
	0x0xxyyy1z : process variable xx.yyy/1z
	0x0xxyyy20 : process variable xx.yyyBIS
	0x0xxyyy30 : process variable xx.yyyTER
	0x0xxyyyA0 : process variable xx.yyyA
	0x0xxyyyAz : process variable xx.yyyA/z
	0x0xxyyyB0 : process variable xx.yyyR
	0x0xxyyyBz : process variable xx.yyyR/z
	0x0xxyyyE0 : process variable xx.yyyE
	0x0xxyyyEz : process variable xx.yyyE/z

*/

#define BIT8(b0,b1,b2,b3,b4,b5,b6,b7) ((b0<<0)|(b1<<1)|(b2<<2)|(b3<<3)|(b4<<4)|(b5<<5)|(b6<<6)|(b7<<7))


/*==============================================================================================*/
/* Definitions */

/*----------------------------------------------------------------------------------------------*/
/* Constants */

/* Object ids */
enum {
	TCN_CONF_OID = NC_TCN_CONF_OBJ_ID,

	ST_OID,
	FUNC_OID,

	MVB_DS_OID,
/*
	MVB_PV_501_OID,
	MVB_PV_502_OID,
	MVB_PV_503_OID,

	MVB_PV_300_OID,
	MVB_PV_301_OID,
	MVB_PV_302_OID,
	MVB_PV_303_OID,
	MVB_PV_304_OID,
	MVB_PV_305_OID,
	MVB_PV_306_OID,
	MVB_PV_307_OID,
	MVB_PV_308_OID,
	MVB_PV_309_OID,
	MVB_PV_310_OID,
	MVB_PV_311_OID,
	MVB_PV_312_OID,
	MVB_PV_313_OID,
	MVB_PV_314_OID,
	MVB_PV_315_OID,
	MVB_PV_316_OID,
	MVB_PV_317_OID,
	MVB_PV_318_OID,
	MVB_PV_319_OID,
	MVB_PV_320_OID,
	MVB_PV_321_OID,
	MVB_PV_322_OID,

	MVB_PV_350_OID,
	MVB_PV_351_OID,
	MVB_PV_352_OID,
	MVB_PV_353_OID,
	MVB_PV_354_OID,
	MVB_PV_355_OID,
	MVB_PV_356_OID,
	MVB_PV_330_OID,
	MVB_PV_331_OID,
	MVB_PV_332_OID,
	MVB_PV_333_OID,
	MVB_PV_334_OID,
	MVB_PV_335_OID,
	MVB_PV_336_OID,

	MVB_PV_370_OID,
	MVB_PV_371_OID,

	MVB_PV_590_OID,
*/
	MVB_PV_390_OID
/*
	MVB_PV_391_OID,

	MVB_PV_360_OID,
	MVB_PV_400_OID,

	MVB_PV_551_OID,
	MVB_PV_601_OID,


// CCUA
    MVB_PV_801_OID,
    MVB_PV_802_OID,
    MVB_PV_803_OID,

// CCUS
    MVB_PV_851_OID,
    MVB_PV_852_OID,
    MVB_PV_853_OID,


// RIO1M
    MVB_PV_811_OID,
    MVB_PV_812_OID,
    MVB_PV_813_OID,

    MVB_PV_814_OID,

// RIO1S
    MVB_PV_861_OID,
    MVB_PV_862_OID,
    MVB_PV_863_OID,


// RIO2M
    MVB_PV_821_OID,
    MVB_PV_822_OID,
    MVB_PV_823_OID,

    MVB_PV_824_OID,


// RIO2S
    MVB_PV_871_OID,
    MVB_PV_872_OID,
    MVB_PV_873_OID,


// RIO3M
    MVB_PV_831_OID,
    MVB_PV_832_OID,
    MVB_PV_833_OID,

    MVB_PV_834_OID,


// RIO3S
    MVB_PV_881_OID,
    MVB_PV_882_OID,
    MVB_PV_883_OID,

// TCU Input
    MVB_PV_180_OID,
    MVB_PV_181_OID,

// TCU Output
    MVB_PV_540_OID,
    MVB_PV_541_OID,
    MVB_PV_542_OID,
    MVB_PV_543_OID,
    MVB_PV_544_OID,

    MVB_PV_560_OID,
    MVB_PV_561_OID,
    MVB_PV_562_OID,
    MVB_PV_563_OID,
    MVB_PV_564_OID,

// CGS Input
    MVB_PV_600_OID,

// CGS Output
    MVB_PV_602_OID,
    MVB_PV_603_OID,
    MVB_PV_604_OID,

    MVB_PV_605_OID,
    MVB_PV_606_OID,
    MVB_PV_607_OID,

// BCU Input
    MVB_PV_1110_OID,
    MVB_PV_1111_OID,

// BCU Output
    MVB_PV_1115_OID,
    MVB_PV_1116_OID,
    MVB_PV_1117_OID,
    MVB_PV_1118_OID,

    MVB_PV_200_OID,
    MVB_PV_201_OID,

    MVB_PV_209_OID,
    MVB_PV_210_OID,
    MVB_PV_211_OID,
    MVB_PV_212_OID,
    MVB_PV_213_OID,
    MVB_PV_214_OID,
    MVB_PV_215_OID,
    MVB_PV_216_OID,
    MVB_PV_217_OID,
/*
    MVB_PV_700_OID,
    MVB_PV_701_OID,
    MVB_PV_702_OID,
    MVB_PV_703_OID,
    MVB_PV_704_OID,
    MVB_PV_705_OID,
    MVB_PV_706_OID,
    MVB_PV_708_OID,
    MVB_PV_709_OID,
    MVB_PV_710_OID,
    MVB_PV_711_OID,
    MVB_PV_712_OID,
    MVB_PV_713_OID,
    MVB_PV_714_OID,
*/


/*	WTB_DS_OID,
	WTB_PV_R0_OID,
	WTB_PV_R1_OID,
	WTB_PV_R2_OID,
	WTB_PV_R3_OID,
	WTB_PV_R1X_OID,
	WTB_PV_R2X_OID,
	WTB_PV_R3X_OID,

	PDM_OID

	PDM_ACT_IMPORT_OID,
	PDM_ACT_IMPORTX_OID,
	PDM_ACT_EXPORT_R0_OID,
	PDM_DV_EXPORT_R0_OID,
	PDM_ACT_EXPORT_R1_OID,
	PDM_DV_EXPORT_R1_OID,
	PDM_ACT_EXPORT_R2_OID,
	PDM_DV_EXPORT_R2_OID,
	PDM_ACT_EXPORT_R3_OID,
	PDM_DV_EXPORT_R3_OID,
	PDM_ACT_EXPORT_R1X_OID,
	PDM_DV_EXPORT_R1X_OID,
	PDM_ACT_EXPORT_R2X_OID,
	PDM_DV_EXPORT_R2X_OID,
	PDM_ACT_EXPORT_R3X_OID,
	PDM_DV_EXPORT_R3X_OID
*/
};

/* Number of elements in each object */
enum {
#ifndef NO_GTWS
	ST_NUM		   = 2,
#else
	ST_NUM		   = 1,
#endif

	FUNC_NUM	   = 2,

#ifndef NO_GTWS
//	MVB_DS_NUM	   = 125,
//	MVB_DS_NUM	   = 112,
	MVB_DS_NUM	   = 1,
#else
//	MVB_DS_NUM	   = 123,
//	MVB_DS_NUM	   = 110,
	MVB_DS_NUM	   = 1,
#endif
/*
	MVB_PV_501_NUM = R1_PV_NUM,
	MVB_PV_502_NUM = R2_PV_NUM,
	MVB_PV_503_NUM = R3_PV_NUM,

	MVB_PV_300_NUM = SDIR_WITH_CHK_PV_NUM,
	MVB_PV_301_NUM = R3_PV_NUM,
	MVB_PV_302_NUM = R3_PV_NUM,
	MVB_PV_303_NUM = R3_PV_NUM,
	MVB_PV_304_NUM = R3_PV_NUM,
	MVB_PV_305_NUM = R3_PV_NUM,
	MVB_PV_306_NUM = R3_PV_NUM,
	MVB_PV_307_NUM = R3_PV_NUM,
	MVB_PV_308_NUM = R3_PV_NUM,
	MVB_PV_309_NUM = R3_PV_NUM,
	MVB_PV_310_NUM = R3_PV_NUM,
	MVB_PV_311_NUM = R3_PV_NUM,
	MVB_PV_312_NUM = R3_PV_NUM,
	MVB_PV_313_NUM = R3_PV_NUM,
	MVB_PV_314_NUM = R3_PV_NUM,
	MVB_PV_315_NUM = R3_PV_NUM,
	MVB_PV_316_NUM = R3_PV_NUM,
	MVB_PV_317_NUM = R3_PV_NUM,
	MVB_PV_318_NUM = R3_PV_NUM,
	MVB_PV_319_NUM = R3_PV_NUM,
	MVB_PV_320_NUM = R3_PV_NUM,
	MVB_PV_321_NUM = R3_PV_NUM,
	MVB_PV_322_NUM = R3_PV_NUM,

	MVB_PV_350_NUM = SDIR_WITH_CHK_PV_NUM,
    MVB_PV_351_NUM = R2_PV_NUM,
	MVB_PV_352_NUM = R2_PV_NUM,
	MVB_PV_353_NUM = R2_PV_NUM,
	MVB_PV_354_NUM = R2_PV_NUM,
	MVB_PV_355_NUM = R2_PV_NUM,
	MVB_PV_356_NUM = R2_PV_NUM,
	MVB_PV_330_NUM = SDIR_WITH_CHK_PV_NUM,
    MVB_PV_331_NUM = R3_PV_NUM,
	MVB_PV_332_NUM = R3_PV_NUM,
	MVB_PV_333_NUM = R3_PV_NUM,
	MVB_PV_334_NUM = R3_PV_NUM,
	MVB_PV_335_NUM = R3_PV_NUM,
	MVB_PV_336_NUM = R3_PV_NUM,

	MVB_PV_370_NUM = R1_PV_NUM,
	MVB_PV_371_NUM = R3_PV_NUM,

	MVB_PV_590_NUM = GTW_DSI_PV_NUM,
*/
	MVB_PV_390_NUM = GTW_DSO_PV_NUM
/*
	MVB_PV_391_NUM = GTW_VID_PV_NUM,

	MVB_PV_360_NUM = GTW_DSM_PV_NUM,
	MVB_PV_400_NUM = GTW_DSS_PV_NUM,

	MVB_PV_551_NUM = 5,
	MVB_PV_601_NUM = 5,


    MVB_PV_801_NUM = R_S_PV_NUM,
    MVB_PV_802_NUM = R_AI_PV_NUM,
    MVB_PV_803_NUM = R_DI_PV_NUM,

    MVB_PV_851_NUM = R_S_PV_NUM,
    MVB_PV_852_NUM = R_AI_PV_NUM,
    MVB_PV_853_NUM = R_DI_PV_NUM,

    MVB_PV_811_NUM = R_S_PV_NUM,
    MVB_PV_812_NUM = R_AI_PV_NUM,
    MVB_PV_813_NUM = R_DI_PV_NUM,
    MVB_PV_814_NUM = R_DO_PV_NUM,

    MVB_PV_861_NUM = R_S_PV_NUM,
    MVB_PV_862_NUM = R_AI_PV_NUM,
    MVB_PV_863_NUM = R_DI_PV_NUM,

    MVB_PV_821_NUM = R_S_PV_NUM,
    MVB_PV_822_NUM = R_AI_PV_NUM,
    MVB_PV_823_NUM = R_DI_PV_NUM,
    MVB_PV_824_NUM = R_DO_PV_NUM,

    MVB_PV_871_NUM = R_S_PV_NUM,
    MVB_PV_872_NUM = R_AI_PV_NUM,
    MVB_PV_873_NUM = R_DI_PV_NUM,

    MVB_PV_831_NUM = R_S_PV_NUM,
    MVB_PV_832_NUM = R_AI_PV_NUM,
    MVB_PV_833_NUM = R_DI_PV_NUM,
    MVB_PV_834_NUM = R_DO_PV_NUM,

    MVB_PV_881_NUM = R_S_PV_NUM,
    MVB_PV_882_NUM = R_AI_PV_NUM,
    MVB_PV_883_NUM = R_DI_PV_NUM,

    MVB_PV_180_NUM = TCU_I_PV_NUM,
    MVB_PV_181_NUM = TCU_I_PV_NUM,

    MVB_PV_540_NUM = TCUA_O_PV_NUM,
    MVB_PV_541_NUM = TCUB_O_PV_NUM,
    MVB_PV_542_NUM = TCUC_O_PV_NUM,
    MVB_PV_543_NUM = TCUD_O_PV_NUM,
    MVB_PV_544_NUM = TCUE_O_PV_NUM,

    MVB_PV_560_NUM = TCUA_O_PV_NUM,
    MVB_PV_561_NUM = TCUB_O_PV_NUM,
    MVB_PV_562_NUM = TCUC_O_PV_NUM,
    MVB_PV_563_NUM = TCUD_O_PV_NUM,
    MVB_PV_564_NUM = TCUE_O_PV_NUM,

    MVB_PV_600_NUM = CGS_I_PV_NUM,

    MVB_PV_602_NUM = CGSA_O_PV_NUM,
    MVB_PV_603_NUM = CGSA_O_PV_NUM,
    MVB_PV_604_NUM = CGSA_O_PV_NUM,

    MVB_PV_605_NUM = CGSB_O_PV_NUM,
    MVB_PV_606_NUM = CGSB_O_PV_NUM,
    MVB_PV_607_NUM = CGSB_O_PV_NUM,

    MVB_PV_1110_NUM = BCU_IN1_PV_NUM,
    MVB_PV_1111_NUM = BCU_IN2_PV_NUM,

    MVB_PV_1115_NUM = BCU_OUT1_PV_NUM,
    MVB_PV_1116_NUM = BCU_OUT2_PV_NUM,
    MVB_PV_1117_NUM = BCU_OUT3_PV_NUM,
    MVB_PV_1118_NUM = BCU_OUT4_PV_NUM,

    MVB_PV_200_NUM = ORA_IDU_PV_NUM,
    MVB_PV_201_NUM = ORA_IDU_PV_NUM,

    MVB_PV_209_NUM = IDU1_I_ST_PV_NUM,
    MVB_PV_210_NUM = IDU1_I_ST_PV_NUM,
    MVB_PV_211_NUM = IDU2_I_ST_PV_NUM,
    MVB_PV_212_NUM = IDU3_I_ST_PV_NUM,
    MVB_PV_213_NUM = IDU4_I_ST_PV_NUM,
    MVB_PV_214_NUM = IDU5_I_ST_PV_NUM,

    MVB_PV_215_NUM = IDU6_I_ST_PV_NUM,
    MVB_PV_216_NUM = IDU7_I_ST_PV_NUM,
    MVB_PV_217_NUM = IDU8_I_ST_PV_NUM,
/*
    MVB_PV_700_NUM = MESS_ST_PV_NUM,
    MVB_PV_701_NUM = MESS_ST_PV_NUM,
    MVB_PV_702_NUM = MESS_ST_PV_NUM,
    MVB_PV_703_NUM = MESS_ST_PV_NUM,
    MVB_PV_704_NUM = MESS_ST_PV_NUM,
    MVB_PV_705_NUM = MESS_ST_PV_NUM,
    MVB_PV_706_NUM = MESS_ST_PV_NUM,
    MVB_PV_708_NUM = MESS_ST_PV_NUM,
    MVB_PV_709_NUM = MESS_ST_PV_NUM,
    MVB_PV_710_NUM = MESS_ST_PV_NUM,
    MVB_PV_711_NUM = MESS_ST_PV_NUM,
    MVB_PV_712_NUM = MESS_ST_PV_NUM,
    MVB_PV_713_NUM = MESS_ST_PV_NUM,
    MVB_PV_714_NUM = MESS_ST_PV_NUM,
*/
/*

	WTB_DS_NUM		= 7,
	WTB_PV_R0_NUM	= R0_PV_NUM,
	WTB_PV_R1_NUM	= R3_PV_NUM + R1_PV_NUM,
	WTB_PV_R2_NUM	= R3_PV_NUM + R2_PV_NUM,
	WTB_PV_R3_NUM	= R3_PV_NUM,
	WTB_PV_R1X_NUM	= R3_PV_NUM + R1_PV_NUM,
	WTB_PV_R2X_NUM	= R3_PV_NUM + R2_PV_NUM,
	WTB_PV_R3X_NUM	= R3_PV_NUM,

	PDM_NUM						= 14,
	PDM_ACT_IMPORT_NUM			= 10,
	PDM_ACT_IMPORTX_NUM			= 10,
	PDM_ACT_EXPORT_R0_NUM		= 2,
	PDM_DV_EXPORT_R0_NUM		= 2,
	PDM_ACT_EXPORT_R1_NUM		= 6,
	PDM_DV_EXPORT_R1_NUM		= 2,
	PDM_ACT_EXPORT_R2_NUM		= 6,
	PDM_DV_EXPORT_R2_NUM		= 2,
	PDM_ACT_EXPORT_R3_NUM		= 4,
	PDM_DV_EXPORT_R3_NUM		= 2,
	PDM_ACT_EXPORT_R1X_NUM		= 6,
	PDM_DV_EXPORT_R1X_NUM		= 2,
	PDM_ACT_EXPORT_R2X_NUM		= 6,
	PDM_DV_EXPORT_R2X_NUM		= 2,
	PDM_ACT_EXPORT_R3X_NUM		= 4,
	PDM_DV_EXPORT_R3X_NUM		= 2
*/
};


/*----------------------------------------------------------------------------------------------*/
/* NC DB structure                                                                              */

const struct {

	NcObjHeader	start_of_db_header;

	NcObjHeader	tcn_conf_header;
	NcTcnConf	tcn_conf;

	NC_OBJ_DEF(StEntry,    ST);
	NC_OBJ_DEF(FuncEntry,  FUNC);
	NC_OBJ_DEF(MvbDsEntry, MVB_DS);
/*
	NC_OBJ_DEF(PvEntry,    MVB_PV_501);
	NC_OBJ_DEF(PvEntry,    MVB_PV_502);
	NC_OBJ_DEF(PvEntry,    MVB_PV_503);

	NC_OBJ_DEF(PvEntry,    MVB_PV_300);
	NC_OBJ_DEF(PvEntry,    MVB_PV_301);
	NC_OBJ_DEF(PvEntry,    MVB_PV_302);
	NC_OBJ_DEF(PvEntry,    MVB_PV_303);
	NC_OBJ_DEF(PvEntry,    MVB_PV_304);
	NC_OBJ_DEF(PvEntry,    MVB_PV_305);
	NC_OBJ_DEF(PvEntry,    MVB_PV_306);
	NC_OBJ_DEF(PvEntry,    MVB_PV_307);
	NC_OBJ_DEF(PvEntry,    MVB_PV_308);
	NC_OBJ_DEF(PvEntry,    MVB_PV_309);
	NC_OBJ_DEF(PvEntry,    MVB_PV_310);
	NC_OBJ_DEF(PvEntry,    MVB_PV_311);
	NC_OBJ_DEF(PvEntry,    MVB_PV_312);
	NC_OBJ_DEF(PvEntry,    MVB_PV_313);
	NC_OBJ_DEF(PvEntry,    MVB_PV_314);
	NC_OBJ_DEF(PvEntry,    MVB_PV_315);
	NC_OBJ_DEF(PvEntry,    MVB_PV_316);
	NC_OBJ_DEF(PvEntry,    MVB_PV_317);
	NC_OBJ_DEF(PvEntry,    MVB_PV_318);
	NC_OBJ_DEF(PvEntry,    MVB_PV_319);
	NC_OBJ_DEF(PvEntry,    MVB_PV_320);
	NC_OBJ_DEF(PvEntry,    MVB_PV_321);
	NC_OBJ_DEF(PvEntry,    MVB_PV_322);

	NC_OBJ_DEF(PvEntry,    MVB_PV_350);
	NC_OBJ_DEF(PvEntry,    MVB_PV_351);
	NC_OBJ_DEF(PvEntry,    MVB_PV_352);
	NC_OBJ_DEF(PvEntry,    MVB_PV_353);
	NC_OBJ_DEF(PvEntry,    MVB_PV_354);
	NC_OBJ_DEF(PvEntry,    MVB_PV_355);
	NC_OBJ_DEF(PvEntry,    MVB_PV_356);
	NC_OBJ_DEF(PvEntry,    MVB_PV_330);
	NC_OBJ_DEF(PvEntry,    MVB_PV_331);
	NC_OBJ_DEF(PvEntry,    MVB_PV_332);
	NC_OBJ_DEF(PvEntry,    MVB_PV_333);
	NC_OBJ_DEF(PvEntry,    MVB_PV_334);
	NC_OBJ_DEF(PvEntry,    MVB_PV_335);
	NC_OBJ_DEF(PvEntry,    MVB_PV_336);

	NC_OBJ_DEF(PvEntry,    MVB_PV_370);
	NC_OBJ_DEF(PvEntry,    MVB_PV_371);

	NC_OBJ_DEF(PvEntry,    MVB_PV_590);
*/
	NC_OBJ_DEF(PvEntry,    MVB_PV_390);
/*
	NC_OBJ_DEF(PvEntry,    MVB_PV_391);

	NC_OBJ_DEF(PvEntry,    MVB_PV_360);
	NC_OBJ_DEF(PvEntry,    MVB_PV_400);

	NC_OBJ_DEF(PvEntry,    MVB_PV_551);
	NC_OBJ_DEF(PvEntry,    MVB_PV_601);

    NC_OBJ_DEF(PvEntry,    MVB_PV_801);
    NC_OBJ_DEF(PvEntry,    MVB_PV_802);
    NC_OBJ_DEF(PvEntry,    MVB_PV_803);

    NC_OBJ_DEF(PvEntry,    MVB_PV_851);
    NC_OBJ_DEF(PvEntry,    MVB_PV_852);
    NC_OBJ_DEF(PvEntry,    MVB_PV_853);

    NC_OBJ_DEF(PvEntry,    MVB_PV_811);
    NC_OBJ_DEF(PvEntry,    MVB_PV_812);
    NC_OBJ_DEF(PvEntry,    MVB_PV_813);
    NC_OBJ_DEF(PvEntry,    MVB_PV_814);

    NC_OBJ_DEF(PvEntry,    MVB_PV_861);
    NC_OBJ_DEF(PvEntry,    MVB_PV_862);
    NC_OBJ_DEF(PvEntry,    MVB_PV_863);

    NC_OBJ_DEF(PvEntry,    MVB_PV_821);
    NC_OBJ_DEF(PvEntry,    MVB_PV_822);
    NC_OBJ_DEF(PvEntry,    MVB_PV_823);
    NC_OBJ_DEF(PvEntry,    MVB_PV_824);

    NC_OBJ_DEF(PvEntry,    MVB_PV_871);
    NC_OBJ_DEF(PvEntry,    MVB_PV_872);
    NC_OBJ_DEF(PvEntry,    MVB_PV_873);

    NC_OBJ_DEF(PvEntry,    MVB_PV_831);
    NC_OBJ_DEF(PvEntry,    MVB_PV_832);
    NC_OBJ_DEF(PvEntry,    MVB_PV_833);
    NC_OBJ_DEF(PvEntry,    MVB_PV_834);

    NC_OBJ_DEF(PvEntry,    MVB_PV_881);
    NC_OBJ_DEF(PvEntry,    MVB_PV_882);
    NC_OBJ_DEF(PvEntry,    MVB_PV_883);

    NC_OBJ_DEF(PvEntry,    MVB_PV_180);
    NC_OBJ_DEF(PvEntry,    MVB_PV_181);

    NC_OBJ_DEF(PvEntry,    MVB_PV_540);
    NC_OBJ_DEF(PvEntry,    MVB_PV_541);
    NC_OBJ_DEF(PvEntry,    MVB_PV_542);
    NC_OBJ_DEF(PvEntry,    MVB_PV_543);
    NC_OBJ_DEF(PvEntry,    MVB_PV_544);

    NC_OBJ_DEF(PvEntry,    MVB_PV_560);
    NC_OBJ_DEF(PvEntry,    MVB_PV_561);
    NC_OBJ_DEF(PvEntry,    MVB_PV_562);
    NC_OBJ_DEF(PvEntry,    MVB_PV_563);
    NC_OBJ_DEF(PvEntry,    MVB_PV_564);

    NC_OBJ_DEF(PvEntry,    MVB_PV_600);

    NC_OBJ_DEF(PvEntry,    MVB_PV_602);
    NC_OBJ_DEF(PvEntry,    MVB_PV_603);
    NC_OBJ_DEF(PvEntry,    MVB_PV_604);
    NC_OBJ_DEF(PvEntry,    MVB_PV_605);
    NC_OBJ_DEF(PvEntry,    MVB_PV_606);
    NC_OBJ_DEF(PvEntry,    MVB_PV_607);




    NC_OBJ_DEF(PvEntry,    MVB_PV_1110);
    NC_OBJ_DEF(PvEntry,    MVB_PV_1111);

    NC_OBJ_DEF(PvEntry,    MVB_PV_1115);
    NC_OBJ_DEF(PvEntry,    MVB_PV_1116);
    NC_OBJ_DEF(PvEntry,    MVB_PV_1117);
    NC_OBJ_DEF(PvEntry,    MVB_PV_1118);

    NC_OBJ_DEF(PvEntry,    MVB_PV_200);
    NC_OBJ_DEF(PvEntry,    MVB_PV_201);

    NC_OBJ_DEF(PvEntry,    MVB_PV_209);
    NC_OBJ_DEF(PvEntry,    MVB_PV_210);
    NC_OBJ_DEF(PvEntry,    MVB_PV_211);
    NC_OBJ_DEF(PvEntry,    MVB_PV_212);
    NC_OBJ_DEF(PvEntry,    MVB_PV_213);
    NC_OBJ_DEF(PvEntry,    MVB_PV_214);

    NC_OBJ_DEF(PvEntry,    MVB_PV_215);
    NC_OBJ_DEF(PvEntry,    MVB_PV_216);
    NC_OBJ_DEF(PvEntry,    MVB_PV_217);
/*
    NC_OBJ_DEF(PvEntry,    MVB_PV_700);
    NC_OBJ_DEF(PvEntry,    MVB_PV_701);
    NC_OBJ_DEF(PvEntry,    MVB_PV_702);
    NC_OBJ_DEF(PvEntry,    MVB_PV_703);
    NC_OBJ_DEF(PvEntry,    MVB_PV_704);
    NC_OBJ_DEF(PvEntry,    MVB_PV_705);
    NC_OBJ_DEF(PvEntry,    MVB_PV_706);
    NC_OBJ_DEF(PvEntry,    MVB_PV_708);
    NC_OBJ_DEF(PvEntry,    MVB_PV_709);
    NC_OBJ_DEF(PvEntry,    MVB_PV_710);
    NC_OBJ_DEF(PvEntry,    MVB_PV_711);
    NC_OBJ_DEF(PvEntry,    MVB_PV_712);
    NC_OBJ_DEF(PvEntry,    MVB_PV_713);
    NC_OBJ_DEF(PvEntry,    MVB_PV_714);
*/
/*
	NC_OBJ_DEF(WtbDsEntry, WTB_DS);
	NC_OBJ_DEF(PvEntry,    WTB_PV_R0);
	NC_OBJ_DEF(PvEntry,    WTB_PV_R1);
	NC_OBJ_DEF(PvEntry,    WTB_PV_R2);
	NC_OBJ_DEF(PvEntry,    WTB_PV_R3);
	NC_OBJ_DEF(PvEntry,    WTB_PV_R1X);
	NC_OBJ_DEF(PvEntry,    WTB_PV_R2X);
	NC_OBJ_DEF(PvEntry,    WTB_PV_R3X);

	NC_OBJ_DEF(PdmEntry,   PDM);
	NC_OBJ_DEF(PdmActEntry,PDM_ACT_IMPORT);
	NC_OBJ_DEF(PdmActEntry,PDM_ACT_IMPORTX);
	NC_OBJ_DEF(PdmActEntry,PDM_ACT_EXPORT_R0);
	NC_OBJ_DEF(char,       PDM_DV_EXPORT_R0);
	NC_OBJ_DEF(PdmActEntry,PDM_ACT_EXPORT_R1);
	NC_OBJ_DEF(char,       PDM_DV_EXPORT_R1);
	NC_OBJ_DEF(PdmActEntry,PDM_ACT_EXPORT_R2);
	NC_OBJ_DEF(char,       PDM_DV_EXPORT_R2);
	NC_OBJ_DEF(PdmActEntry,PDM_ACT_EXPORT_R3);
	NC_OBJ_DEF(char,       PDM_DV_EXPORT_R3);
	NC_OBJ_DEF(PdmActEntry,PDM_ACT_EXPORT_R1X);
	NC_OBJ_DEF(char,       PDM_DV_EXPORT_R1X);
	NC_OBJ_DEF(PdmActEntry,PDM_ACT_EXPORT_R2X);
	NC_OBJ_DEF(char,       PDM_DV_EXPORT_R2X);
	NC_OBJ_DEF(PdmActEntry,PDM_ACT_EXPORT_R3X);
	NC_OBJ_DEF(char,       PDM_DV_EXPORT_R3X);
*/
	NcObjHeader	end_of_db_header;

} nc_db = {


/*----------------------------------------------------------------------------------------------*/
/* Start of database                                                                            */

{ NC_START_OF_DB_OBJ_ID, 0 },


/*----------------------------------------------------------------------------------------------*/
/* TCN configuration root                                                                       */

{ TCN_CONF_OID, sizeof(NcTcnConf) },

{
	0,  				           /* version                                                   */
	0,  				           /* min_version                                               */

	ST_OID,				           /* st_oid                                                    */
	FUNC_OID,			           /* func_oid                                                  */
	0x00,			           /* pdm_oid                                                   */

	0,					           /* flags                                                     */

	MVB_DS_OID,			           /* mvb_ds_oid                                                */
	0,					           /* mtvc                                                      */
	42,					           /* t_reply_max                                               */
	1000,				           /* basic_period                                              */
	0xC000,				           /* ev_poll_strategy                                          */
	5,					           /* c_macros_in_turn                                          */
	1,					           /* scan_strategy                                             */

	0x41545200, /* 'ATR' */        /* characters of the signature (the company)                 */
	0x47694361, /* 'GiCa' */       /* characters of the signature (the person)                  */
	0x01,                          /* BCD coded first digits of the version                     */
	0x00,                          /* BCD coded second digits of the version                    */
	0xB002,                        /* extra characters of the version                           */
	0x05,                          /* BCD coded day number                                      */
	0x06,                          /* BCD coded month number                                    */
	0x09,                          /* BCD coded year number                                     */

	0x00,			           /* wtb_ds_oid                                                */

	0, 0, 0, 0,                    /* vehicle descriptor header (not used)                      */

	0x00,                          /* 001    : 135                                              */
	0x83,                          /* 002    : 136 [owner] 83:FS                                */
	0x00,                          /* 003    : 138                                              */
	0x00,                          /* 004    : 137                                              */
	1,                             /* 005    : 139 [NcV]                                        */
	0,                             /* 006    : 501 502 503 504 510 511 515 516                  */
	0,                             /* 007    : 507                                              */
	0,                             /* 008    : 508                                              */
	0,                             /* 009    : 509                                              */
	0,                             /* 010    : 509                                              */
	0,                             /* 011    : 509                                              */
	0,                             /* 012    : 509                                              */
	0,                             /* 013    : 509                                              */
	0,                             /* 014    : 509                                              */
	0,                             /* 015    : 509                                              */
	0,                             /* 016    : 509                                              */
	0x01,                          /* 017    : 134                                              */
	BIT8(0,0,0,0,0,0,0,0),         /* 018    : [ 17] [ 19]   21    22    23    24    29    30   */
	BIT8(0,0,0,0,0,0,0,0),         /* 019    :   31    32    33    34    35    36    37   142   */
	BIT8(1,0,1,0,1,1,1,0),         /* 020    : [ 38] [ 39] [ 40] [ 41] [ 44] [ 45] [ 46]   47   */
	BIT8(0,1,0,0,0,0,0,0),         /* 021    :   48    49  [ 50] [ 51]   52    53    54    55   */
	BIT8(0,0,0,0,0,0,0,0),         /* 022    :   56    57    58    59    60    61    62    63   */
	BIT8(0,0,0,0,0,0,0,0),         /* 023    :   64    65    66    67    68    69    70    71   */
    BIT8(0,0,0,0,0,0,0,0),         /* 024    :   72    73    74    75    76    77    78    79   **/
	BIT8(0,0,0,0,0,0,0,0),         /* 025    :   80    81    82    83    84  [ 85]   86    87   */
	BIT8(0,0,0,0,0,0,0,0),         /* 026    :   88    89    90    91    92    93    94    95   */
	BIT8(0,0,0,0,0,0,0,0),         /* 027    :   96    97    98    99   100   101   102   103   */
	BIT8(0,0,0,0,0,0,0,0),         /* 028    :  104   106   107   110   111   112   113   114   */
	BIT8(0,0,0,0,0,0,0,0),         /* 029    :  115   116   117   119   120   121   123   124   */
	BIT8(0,0,0,0,0,0,0,0),         /* 030    :  126   128   131   132     -     -     -     -   */
	0,                             /* 031    :                                                  */
	0,                             /* 032    :                                                  */
	0,                             /* 033    :                                                  */
	0,                             /* 034    :                                                  */
	0,                             /* 035    :                                                  */
	0,                             /* 036    :                                                  */
	0,                             /* 037    :                                                  */
	0,                             /* 038    :                                                  */
	0,                             /* 039    :                                                  */
	0,                             /* 040    :                                                  */

	0x00, 0x40, 0x30, 0x00, 0x00,  /* 041-045: [UIC vehicle identifier]                         */
	BIT8(0,0,0,0,0,0,0,0),         /* 046    : [  1] [  2] [  3] [  4] [  5] [  6] [  7] [  8]  */
	BIT8(0,0,0,0,0,0,0,0),         /* 047    : [  9] [ 10] [ 11] [ 12] [ 13] [ 14] [ 15] [ 16]  */
	BIT8(0,0,0,0,0,0,0,1),         /* 048    : [ 18] [ 20]   25    26    27    28  [ 42] [ 43]  */
	BIT8(0,0,0,0,0,0,0,0),         /* 049    :  105   108   109   118   122   125   127   129   */
	BIT8(0,1,0,0,0,0,0,0),         /* 050    :  130   141     -     -     -     -     -     -   */
	0x00, 0x00,                    /* 051-052:                                                  */
	0x00, 0x00,                    /* 053-054: [513] veh. number for seat reservation           */

	0x00, 0x00, 0x00, 0x00, 0x00,  /* 055-059: [UIC vehicle identifier]                         */
	BIT8(0,0,0,0,0,0,0,0),         /* 060    : [  1] [  2] [  3] [  4] [  5] [  6] [  7] [  8]  */
	BIT8(0,0,0,0,0,0,0,0),         /* 061    : [  9] [ 10] [ 11] [ 12] [ 13] [ 14] [ 15] [ 16]  */
	BIT8(0,0,0,0,0,0,0,0),         /* 062    : [ 18] [ 20]   25    26    27    28  [ 42] [ 43]  */
	BIT8(0,0,0,0,0,0,0,0),         /* 063    :  105   108   109   118   122   125   127   129   */
	BIT8(0,0,0,0,0,0,0,0),         /* 064    :  130   141     -     -     -     -     -     -   */
	0x00, 0x00,                    /* 065-066:                                                  */
	0x00, 0x00,                    /* 067-068: [513] veh. number for seat reservation           */

	0x00, 0x00, 0x00, 0x00, 0x00,  /* 069-073: [UIC vehicle identifier]                         */
	BIT8(0,0,0,0,0,0,0,0),         /* 074    : [  1] [  2] [  3] [  4] [  5] [  6] [  7] [  8]  */
	BIT8(0,0,0,0,0,0,0,0),         /* 075    : [  9] [ 10] [ 11] [ 12] [ 13] [ 14] [ 15] [ 16]  */
	BIT8(0,0,0,0,0,0,0,0),         /* 076    : [ 18] [ 20]   25    26    27    28  [ 42] [ 43]  */
	BIT8(0,0,0,0,0,0,0,0),         /* 077    :  105   108   109   118   122   125   127   129   */
	BIT8(0,0,0,0,0,0,0,0),         /* 078    :  130   141     -     -     -     -     -     -   */
	0x00, 0x00,                    /* 079-080:                                                  */
	0x00, 0x00,                    /* 081-082: [513] veh. number for seat reservation           */

	0x00, 0x00, 0x00, 0x00, 0x00,  /* 083-087: [UIC vehicle identifier]                         */
	BIT8(0,0,0,0,0,0,0,0),         /* 088    : [  1] [  2] [  3] [  4] [  5] [  6] [  7] [  8]  */
	BIT8(0,0,0,0,0,0,0,0),         /* 089    : [  9] [ 10] [ 11] [ 12] [ 13] [ 14] [ 15] [ 16]  */
	BIT8(0,0,0,0,0,0,0,0),         /* 090    : [ 18] [ 20]   25    26    27    28  [ 42] [ 43]  */
	BIT8(0,0,0,0,0,0,0,0),         /* 091    :  105   108   109   118   122   125   127   129   */
	BIT8(0,0,0,0,0,0,0,0),         /* 092    :  130   141     -     -     -     -     -     -   */
	0x00, 0x00,                    /* 093-094:                                                  */
	0x00, 0x00,                    /* 095-096: [513] veh. number for seat reservation           */

	0x00, 0x00, 0x00, 0x00, 0x00,  /* 097-101: [UIC vehicle identifier]                         */
	BIT8(0,0,0,0,0,0,0,0),         /* 102    : [  1] [  2] [  3] [  4] [  5] [  6] [  7] [  8]  */
	BIT8(0,0,0,0,0,0,0,0),         /* 103    : [  9] [ 10] [ 11] [ 12] [ 13] [ 14] [ 15] [ 16]  */
	BIT8(0,0,0,0,0,0,0,0),         /* 104    : [ 18] [ 20]   25    26    27    28  [ 42] [ 43]  */
	BIT8(0,0,0,0,0,0,0,0),         /* 105    :  105   108   109   118   122   125   127   129   */
	BIT8(0,0,0,0,0,0,0,0),         /* 106    :  130   141     -     -     -     -     -     -   */
	0x00, 0x00,                    /* 107-108:                                                  */
	0x00, 0x00,                    /* 109-110: [513] veh. number for seat reservation           */

	0x00, 0x00, 0x00, 0x00, 0x00,  /* 111-115: [UIC vehicle identifier]                         */
	BIT8(0,0,0,0,0,0,0,0),         /* 116    : [  1] [  2] [  3] [  4] [  5] [  6] [  7] [  8]  */
	BIT8(0,0,0,0,0,0,0,0),         /* 117    : [  9] [ 10] [ 11] [ 12] [ 13] [ 14] [ 15] [ 16]  */
	BIT8(0,0,0,0,0,0,0,0),         /* 118    : [ 18] [ 20]   25    26    27    28  [ 42] [ 43]  */
	BIT8(0,0,0,0,0,0,0,0),         /* 119    :  105   108   109   118   122   125   127   129   */
	BIT8(0,0,0,0,0,0,0,0),         /* 120    :  130   141     -     -     -     -     -     -   */
	0x00, 0x00,                    /* 121-122:                                                  */
	0x00, 0x00,                    /* 123-124: [513] veh. number for seat reservation           */

	BIT8(1,1,0,0,0,0,0,1),         /* 001    : FS-attr :   1   2   3   4   5   6   7   8        */
	BIT8(1,1,1,1,1,1,1,1),         /* 002    : FS-attr :   9  10  11  12  13  14  15  16        */
	BIT8(1,0,1,0,1,0,1,1),         /* 003    : FS-attr :  17  18  19  20  21  22  23  24        */
	BIT8(1,1,0,0,1,1,1,1),         /* 004    : FS-attr :  25  26  27  28  29  30  31  32        */
	BIT8(1,1,1,1,0,0,0,0),         /* 005    : FS-attr :  33  34  35  36   -   -   -   -        */
	0x99,                          /* 006    : PMax                                             */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 007-016:                                                  */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 017-026:                                                  */
	0, 0, 0, 0, 0, 0               /* 027-032:                                                  */
},


/*----------------------------------------------------------------------------------------------*/
/* Stations                                                                                     */

{ ST_OID, ST_NUM * sizeof(StEntry) },

{
////    { CCUM_ST_ID,  NUM_STATIO(CCUM_ST_ID), NC_ST_IS_MVB_BA|NC_ST_NEEDS_NADI,   0, 0 },	/*  1 */
//    { CCUM_ST_ID,  NUM_STATIO(CCUM_ST_ID), 0/*NC_ST_NEEDS_NADI*/,         			 0, 0 },	/*  1 */
//	{ CCUS_ST_ID,  NUM_STATIO(CCUS_ST_ID),  0,                                   0, 0 },	/*  2 */
//	{ RIOAM_ST_ID, NUM_STATIO(RIOAM_ST_ID), 0,                                   0, 0 },	/*  3 */
//	{ RIOAS_ST_ID, NUM_STATIO(RIOAS_ST_ID), 0,                                   0, 0 },	/*  4 */
//	{ RIOBM_ST_ID, NUM_STATIO(RIOBM_ST_ID), 0,                                   0, 0 },	/*  5 */
//	{ RIOBS_ST_ID, NUM_STATIO(RIOBS_ST_ID), 0,                                   0, 0 },	/*  6 */
//	{ RIOCM_ST_ID, NUM_STATIO(RIOCM_ST_ID), NC_ST_IS_MVB_BA,                     0, 0 },	/*  7 */
////	{ RIOCM_ST_ID, NUM_STATIO(RIOCM_ST_ID), 0,                     0, 0 },	/*  7 */
//	{ RIOCS_ST_ID, NUM_STATIO(RIOCS_ST_ID), 0,                                   0, 0 },	/*  8 */
//	{ TCU1_ST_ID,  NUM_STATIO(TCU1_ST_ID),  0,                                   0, 0 },	/*  9 */
//	{ TCU2_ST_ID,  NUM_STATIO(TCU2_ST_ID),  0,                                   0, 0 },	/* 10 */
//	{ CGS1_ST_ID,  NUM_STATIO(CGS1_ST_ID),  0,                                   0, 0 },	/* 11 */
//	{ CGS2_ST_ID,  NUM_STATIO(CGS2_ST_ID),  0,                                   0, 0 },	/* 12 */
//	{ CGS3_ST_ID,  NUM_STATIO(CGS3_ST_ID),  0,                                   0, 0 },	/* 13 */
//
//	{ BCU_ST_ID,   NUM_STATIO(BCU_ST_ID),   0,                                   0, 0 },	/* 14 */
//	{ IDU1_ST_ID,   NUM_STATIO(IDU1_ST_ID),   0,                                   0, 0 },	/* 15 */
//	{ IDU2_ST_ID,   NUM_STATIO(IDU2_ST_ID),   0,                                   0, 0 },	/* 16 */
//
	{ GTWM_ST_ID,  NUM_STATIO(GTWM_ST_ID), NC_ST_IS_MVB_BA, 		             0, 0 },	/* 17 */
#ifndef NO_GTWS
	{ GTWS_ST_ID, NUM_STATIO(GTWS_ST_ID),   0,                  		         0, 0 }	/* 18 */
#endif
},


/*----------------------------------------------------------------------------------------------*/
/* Functions                                                                                    */

{ FUNC_OID, FUNC_NUM * sizeof(FuncEntry) },

{
//	{ CCUM_ST_ID, CABINE_DE_CONDUITE_FCT,                      0,              0 }, /*  1 */
//	{ CCUM_ST_ID, COMMANDE_DU_TRAIN_FCT,                       0,              0 }, /*  2 */
//	{ CCUM_ST_ID, COMMANDE_DE_L_ENGIN_MOTEUR_FCT,              0,              0 }, /*  3 */
//    { CCUM_ST_ID, EXPLOITATIONS_ANNEXES_DE_L_ENGIN_MOTEUR_FCT, 0,              0 }, /*  4 */
//    { CCUM_ST_ID, FREINS_FCT,                                  0,              0 }, /*  5 */
//	{ CCUM_ST_ID, ALIMENTATION_EN_ENERGIE_FCT,                 0,              0 }, /*  6 */
//	{ CCUM_ST_ID, DIAGNOSTIC_FCT,                              0,              0 }, /*  7 */
//	{ CCUM_ST_ID, PORTES_FCT,                                  0,              0 }, /*  8 */
//	{ CCUM_ST_ID, MESURE_DE_PARCOURS_VITESSE_FCT,              0,              0 }, /*  9 */
//	{ CCUM_ST_ID, AFFICHAGE_CABINE_DE_CONDUITE_FCT,            0,              0 }, /* 10 */
    { GTWM_ST_ID, NC_ROUTER_FCT,                               NC_FUNC_NO_WTB, 0 }, /* 11 */
    { GTWM_ST_ID, MAPPING_SERVER_FCT,                          0,              0 } /* 12 */
//    { CCUS_ST_ID, CCUS_FCT,                                    NC_FUNC_NO_WTB, 0 }  /* 13 */
},


/*----------------------------------------------------------------------------------------------*/
/* MVB datasets                                                                                 */

{ MVB_DS_OID, MVB_DS_NUM * sizeof(MvbDsEntry) },

{
//	{ 0xE0000501, CCUM_ST_ID, NC_MVB_DS_32_BYTES + 501, 32+18, 128, MVB_PV_501_OID },	/*  1 */
//	{ 0xE0000502, CCUM_ST_ID, NC_MVB_DS_32_BYTES + 502, 32+25, 128, MVB_PV_502_OID },	/*  2 */
//	{ 0xE0000503, CCUM_ST_ID, NC_MVB_DS_32_BYTES + 503, 32+28, 128, MVB_PV_503_OID },	/*  3 */
//
//	{ 0xE0000300, GTWM_ST_ID, NC_MVB_DS_16_BYTES + 300, 256+168, 1024, MVB_PV_300_OID },	/*  4 */
//	{ 0xE0000301, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 301, 256+184, 1024, MVB_PV_301_OID },	/*  5 */
//	{ 0xE0000302, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 302, 256+190, 1024, MVB_PV_302_OID },	/*  6 */
//	{ 0xE0000303, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 303, 256+183, 1024, MVB_PV_303_OID },	/*  7 */
//	{ 0xE0000304, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 304, 256+179, 1024, MVB_PV_304_OID },	/*  8 */
//	{ 0xE0000305, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 305, 256+243, 1024, MVB_PV_305_OID },	/*  9 */
//	{ 0xE0000306, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 306, 256+181, 1024, MVB_PV_306_OID },	/* 10 */
//	{ 0xE0000307, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 307, 256+186, 1024, MVB_PV_307_OID },	/* 11 */
//	{ 0xE0000308, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 308, 256+189, 1024, MVB_PV_308_OID },	/* 12 */
//	{ 0xE0000309, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 309, 256+171, 1024, MVB_PV_309_OID },	/* 13 */
//	{ 0xE0000310, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 310, 256+163, 1024, MVB_PV_310_OID },	/* 14 */
//	{ 0xE0000311, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 311, 256+190, 1024, MVB_PV_311_OID },	/* 15 */
//	{ 0xE0000312, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 312, 256+191, 1024, MVB_PV_312_OID },	/* 16 */
//	{ 0xE0000313, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 313, 256+194, 1024, MVB_PV_313_OID },	/* 17 */
//	{ 0xE0000314, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 314, 256+197, 1024, MVB_PV_314_OID },	/* 18 */
//	{ 0xE0000315, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 315, 256+175, 1024, MVB_PV_315_OID },	/* 19 */
//	{ 0xE0000316, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 316, 256+232, 1024, MVB_PV_316_OID },	/* 20 */
//	{ 0xE0000317, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 317, 256+199, 1024, MVB_PV_317_OID },	/* 21 */
//	{ 0xE0000318, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 318, 256+200, 1024, MVB_PV_318_OID },	/* 22 */
//	{ 0xE0000319, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 319, 256+229, 1024, MVB_PV_319_OID },	/* 23 */
//    { 0xE0000320, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 320, 256+202, 1024, MVB_PV_320_OID },	/* 24 */
//	{ 0xE0000321, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 321, 256+227, 1024, MVB_PV_321_OID },	/* 25 */
//	{ 0xE0000322, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 322, 256+204, 1024, MVB_PV_322_OID },	/* 26 */
//
//	{ 0xE0000350, GTWM_ST_ID, NC_MVB_DS_16_BYTES + 350, 64+26, 256, MVB_PV_350_OID },	/* 27 */
//	{ 0xE0000351, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 351, 64+31, 256, MVB_PV_351_OID },	/* 28 */
//	{ 0xE0000352, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 352, 64+24, 256, MVB_PV_352_OID },	/* 29 */
//	{ 0xE0000353, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 353, 64+29, 256, MVB_PV_353_OID },	/* 30 */
//	{ 0xE0000354, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 354, 64+30, 256, MVB_PV_354_OID },	/* 31 */
//	{ 0xE0000355, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 355, 64+ 3, 256, MVB_PV_355_OID },	/* 32 */
//	{ 0xE0000356, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 356, 64+ 8, 256, MVB_PV_356_OID },	/* 33 */
//
//	{ 0xE0000330, GTWM_ST_ID, NC_MVB_DS_16_BYTES + 330, 64+47, 256, MVB_PV_330_OID },	/* 34 */
//	{ 0xE0000331, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 331, 64+15, 256, MVB_PV_331_OID },	/* 35 */
//	{ 0xE0000332, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 332, 64+14, 256, MVB_PV_332_OID },	/* 36 */
//	{ 0xE0000333, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 333, 64+42, 256, MVB_PV_333_OID },	/* 37 */
//	{ 0xE0000334, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 334, 64+34, 256, MVB_PV_334_OID },	/* 38 */
//	{ 0xE0000335, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 335, 64+46, 256, MVB_PV_335_OID },	/* 39 */
//	{ 0xE0000336, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 336, 64+11, 256, MVB_PV_336_OID },	/* 40 */
//
//	{ 0xE0000370, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 370, 32+12, 128, MVB_PV_370_OID },	/* 41 */
//	{ 0xE0000371, GTWM_ST_ID, NC_MVB_DS_32_BYTES + 371, 64+39, 256, MVB_PV_371_OID },	/* 42 */
//
//	{ GTW_DSI_ID, CCUM_ST_ID, GTW_DSI_BYTES      + 590, 64+ 1, 256, MVB_PV_590_OID },	/* 43 */
	{ GTW_DSO_ID, GTWM_ST_ID, GTW_DSO_BYTES      + 390, 64+ 0, 256, MVB_PV_390_OID }	/* 44 */
//	{ GTW_VID_ID, GTWM_ST_ID, GTW_VID_BYTES      + 391,256+113,256, MVB_PV_391_OID },	/* 44 */

#ifndef NO_GTWS
//	{ GTW_DSM_ID, GTWM_ST_ID, GTW_DSM_BYTES      + 360,128+ 2, 512, MVB_PV_360_OID },	/* 45 */
//	{ GTW_DSS_ID, GTWS_ST_ID, GTW_DSS_BYTES      + 400,128+32, 512, MVB_PV_400_OID },	/* 46 */
#endif

//	{ 0xE0000551, CCUM_ST_ID, NC_MVB_DS_08_BYTES  + 551,  64+33, 256, MVB_PV_551_OID },	/* 47 */
//	{ 0xE0000601, CCUS_ST_ID, NC_MVB_DS_08_BYTES  + 601,  64+32, 256, MVB_PV_601_OID },	/* 48 */
//
//    { 0xE0000801, CCUM_ST_ID,  NC_MVB_DS_32_BYTES + 801, 128+125,  512, MVB_PV_801_OID }, /*  49 */
//    { 0xE0000802, CCUM_ST_ID,  NC_MVB_DS_32_BYTES + 802, 128+126,  512, MVB_PV_802_OID }, /*  50 */
//    { 0xE0000803, CCUM_ST_ID,  NC_MVB_DS_32_BYTES + 803, 128+127,  512, MVB_PV_803_OID }, /*  51 */
//
//    { 0xE0000851, CCUS_ST_ID,  NC_MVB_DS_32_BYTES + 851, 128+119,  512, MVB_PV_851_OID }, /*  52 */
//    { 0xE0000852, CCUS_ST_ID,  NC_MVB_DS_32_BYTES + 852, 128+122,  512, MVB_PV_852_OID }, /*  53 */
//    { 0xE0000853, CCUS_ST_ID,  NC_MVB_DS_32_BYTES + 853, 128+117,  512, MVB_PV_853_OID }, /*  54 */
//
//    { 0xE0000811, RIOAM_ST_ID, NC_MVB_DS_32_BYTES + 811,  16+  4,  128, MVB_PV_811_OID }, /*  55 */
//    { 0xE0000812, RIOAM_ST_ID, NC_MVB_DS_32_BYTES + 812,  16+  5,  128, MVB_PV_812_OID }, /*  56 */
//    { 0xE0000813, RIOAM_ST_ID, NC_MVB_DS_32_BYTES + 813,  16+  6,  128, MVB_PV_813_OID }, /*  57 */
//    { 0xE0000814, CCUM_ST_ID,  NC_MVB_DS_32_BYTES + 814,  32+ 31,  128, MVB_PV_814_OID }, /*  58 */
//
//    { 0xE0000861, RIOAS_ST_ID, NC_MVB_DS_32_BYTES + 861, 128+108,  512, MVB_PV_861_OID }, /*  59 */
//    { 0xE0000862, RIOAS_ST_ID, NC_MVB_DS_32_BYTES + 862, 128+107,  512, MVB_PV_862_OID }, /*  60 */
//    { 0xE0000863, RIOAS_ST_ID, NC_MVB_DS_32_BYTES + 863, 128+ 91,  512, MVB_PV_863_OID }, /*  61 */
//
//    { 0xE0000821, RIOBM_ST_ID, NC_MVB_DS_32_BYTES + 821,  16+ 13,  128, MVB_PV_821_OID }, /*  62 */
//    { 0xE0000822, RIOBM_ST_ID, NC_MVB_DS_32_BYTES + 822,  16+  0,  128, MVB_PV_822_OID }, /*  63 */
//    { 0xE0000823, RIOBM_ST_ID, NC_MVB_DS_32_BYTES + 823,  16+  1,  128, MVB_PV_823_OID }, /*  64 */
//    { 0xE0000824, CCUM_ST_ID,  NC_MVB_DS_32_BYTES + 824,  32+  2,  128, MVB_PV_824_OID }, /*  65 */
//
//    { 0xE0000871, RIOBS_ST_ID, NC_MVB_DS_32_BYTES + 871, 128+ 23,  512, MVB_PV_871_OID }, /*  66 */
//    { 0xE0000872, RIOBS_ST_ID, NC_MVB_DS_32_BYTES + 872, 128+105,  512, MVB_PV_872_OID }, /*  67 */
//    { 0xE0000873, RIOBS_ST_ID, NC_MVB_DS_32_BYTES + 873, 128+104,  512, MVB_PV_873_OID }, /*  68 */
//
//    { 0xE0000831, RIOCM_ST_ID, NC_MVB_DS_32_BYTES + 831,  16+  3,  128, MVB_PV_831_OID }, /*  69 */
//    { 0xE0000832, RIOCM_ST_ID, NC_MVB_DS_32_BYTES + 832,  16+  4,  128, MVB_PV_832_OID }, /*  70 */
//    { 0xE0000833, RIOCM_ST_ID, NC_MVB_DS_32_BYTES + 833,  16+ 10,  128, MVB_PV_833_OID }, /*  71 */
//    { 0xE0000834, CCUM_ST_ID,  NC_MVB_DS_32_BYTES + 834,  32+  6,  128, MVB_PV_834_OID }, /*  72 */
//
//    { 0xE0000881, RIOCS_ST_ID, NC_MVB_DS_32_BYTES + 881, 128+120,  512, MVB_PV_881_OID }, /*  73 */
//    { 0xE0000882, RIOCS_ST_ID, NC_MVB_DS_32_BYTES + 882, 128+ 73,  512, MVB_PV_882_OID }, /*  74 */
//    { 0xE0000883, RIOCS_ST_ID, NC_MVB_DS_32_BYTES + 883, 128+ 87,  512, MVB_PV_883_OID }, /*  75 */
//
//    { 0xE0000180, CCUM_ST_ID,  NC_MVB_DS_32_BYTES + 180,  32+  9,  128, MVB_PV_180_OID }, /*  76 */
//    { 0xE0000181, CCUM_ST_ID,  NC_MVB_DS_32_BYTES + 181,  32+ 27,  128, MVB_PV_181_OID }, /*  77 */
//
//    { 0xE0000540, TCU1_ST_ID,  NC_MVB_DS_32_BYTES + 540,  32+ 11,  128, MVB_PV_540_OID }, /*  78 */
//    { 0xE0000541, TCU1_ST_ID,  NC_MVB_DS_32_BYTES + 541,  32+  7,  128, MVB_PV_541_OID }, /*  79 */
//    { 0xE0000542, TCU1_ST_ID,  NC_MVB_DS_32_BYTES + 542,  32+ 13,  128, MVB_PV_542_OID }, /*  80 */
//
//    { 0xE0000543, TCU1_ST_ID,  NC_MVB_DS_32_BYTES + 543, 256+213, 1024, MVB_PV_543_OID }, /*  81 */
//    { 0xE0000544, TCU1_ST_ID,  NC_MVB_DS_32_BYTES + 544, 256+254, 1024, MVB_PV_544_OID }, /*  82 */
//
//    { 0xE0000560, TCU2_ST_ID,  NC_MVB_DS_32_BYTES + 560,  32+ 16,  128, MVB_PV_560_OID }, /*  83 */
//    { 0xE0000561, TCU2_ST_ID,  NC_MVB_DS_32_BYTES + 561,  32+ 17,  128, MVB_PV_561_OID }, /*  84 */
//    { 0xE0000562, TCU2_ST_ID,  NC_MVB_DS_32_BYTES + 562,  32+ 18,  128, MVB_PV_562_OID }, /*  85 */
//
//    { 0xE0000563, TCU2_ST_ID,  NC_MVB_DS_32_BYTES + 563, 256+174, 1024, MVB_PV_563_OID }, /*  86 */
//    { 0xE0000564, TCU2_ST_ID,  NC_MVB_DS_32_BYTES + 564, 256+172, 1024, MVB_PV_564_OID }, /*  87 */
//
//    { 0xE0000600, CCUM_ST_ID,  NC_MVB_DS_04_BYTES + 600,  32+  1,  128, MVB_PV_600_OID }, /*  88 */
//
//    { 0xE0000602, CGS1_ST_ID,  NC_MVB_DS_16_BYTES + 602,  32+ 22,  128, MVB_PV_602_OID }, /*  89 */
//    { 0xE0000603, CGS2_ST_ID,  NC_MVB_DS_16_BYTES + 603,  32+ 25,  128, MVB_PV_603_OID }, /*  90 */
//    { 0xE0000604, CGS3_ST_ID,  NC_MVB_DS_16_BYTES + 604,  32+ 28,  128, MVB_PV_604_OID }, /*  91 */
//
//    { 0xE0000605, CGS1_ST_ID,  NC_MVB_DS_32_BYTES + 605, 256+184, 1024, MVB_PV_605_OID }, /*  92 */
//    { 0xE0000606, CGS2_ST_ID,  NC_MVB_DS_32_BYTES + 606, 256+168, 1024, MVB_PV_606_OID }, /*  93 */
//    { 0xE0000607, CGS3_ST_ID,  NC_MVB_DS_32_BYTES + 607, 256+248, 1024, MVB_PV_607_OID },  /*  94 */
//
//    { 0xE0001110, CCUM_ST_ID,  NC_MVB_DS_32_BYTES + 1110,  64+ 59,  256, MVB_PV_1110_OID },  /*  95 */
//    { 0xE0001111, CCUM_ST_ID,  NC_MVB_DS_32_BYTES + 1111, 128+ 94,  512, MVB_PV_1111_OID },  /*  96 */
//
//    { 0xE0001115, BCU_ST_ID,   NC_MVB_DS_32_BYTES + 1115, 128+ 83,  512, MVB_PV_1115_OID },  /*  97 */
//    { 0xE0001116, BCU_ST_ID,   NC_MVB_DS_32_BYTES + 1116, 128+ 79,  512, MVB_PV_1116_OID },  /*  97 */
//    { 0xE0001117, BCU_ST_ID,   NC_MVB_DS_32_BYTES + 1117, 128+ 78,  512, MVB_PV_1117_OID },  /*  97 */
//    { 0xE0001118, BCU_ST_ID,   NC_MVB_DS_32_BYTES + 1118, 128+111,  512, MVB_PV_1118_OID },  /*  97 */
//
//    { 0xE0000200, IDU1_ST_ID,  NC_MVB_DS_32_BYTES +  200, 64+  5, 256, MVB_PV_200_OID },  /*  95 */
//    { 0xE0000201, IDU2_ST_ID,  NC_MVB_DS_32_BYTES +  201, 64+  2, 256, MVB_PV_201_OID },  /*  96 */
//
//    { 0xE0000209, CCUM_ST_ID,   NC_MVB_DS_32_BYTES + 209, 64+ 32,  256, MVB_PV_209_OID },  /*  97 */
//    { 0xE0000210, CCUM_ST_ID,   NC_MVB_DS_32_BYTES + 210, 64+ 12,  256, MVB_PV_210_OID },  /*  98 */
//    { 0xE0000211, CCUM_ST_ID,   NC_MVB_DS_32_BYTES + 211, 64+ 10,  256, MVB_PV_211_OID },  /*  99 */
//    { 0xE0000212, CCUM_ST_ID,   NC_MVB_DS_32_BYTES + 212, 64+ 21,  256, MVB_PV_212_OID },  /* 100 */
//    { 0xE0000213, CCUM_ST_ID,   NC_MVB_DS_32_BYTES + 213, 64+  8,  256, MVB_PV_213_OID },  /* 101 */
//    { 0xE0000214, CCUM_ST_ID,   NC_MVB_DS_32_BYTES + 214, 64+  7,  256, MVB_PV_214_OID },  /*  97 */
//
//    { 0xE0000215, CCUM_ST_ID,   NC_MVB_DS_32_BYTES + 215, 64+ 37,  256, MVB_PV_215_OID },  /*  97 */
//    { 0xE0000216, CCUM_ST_ID,   NC_MVB_DS_32_BYTES + 216, 64+ 55,  256, MVB_PV_216_OID },  /*  97 */
//    { 0xE0000217, CCUM_ST_ID,   NC_MVB_DS_32_BYTES + 217, 64+ 23,  256, MVB_PV_217_OID }  /*  97 */
//
////    { 0xE0000700, CCUM_ST_ID,    NC_MVB_DS_32_BYTES + 700, 32+  9,  128, MVB_PV_700_OID },  /*  97 */
////    { 0xE0000701, CCUM_ST_ID,    NC_MVB_DS_32_BYTES + 701, 32+ 21,  128, MVB_PV_701_OID },  /*  97 */
////    { 0xE0000702, CCUM_ST_ID,    NC_MVB_DS_32_BYTES + 702, 32+ 22,  128, MVB_PV_702_OID },  /*  97 */
////    { 0xE0000703, CCUM_ST_ID,    NC_MVB_DS_32_BYTES + 703, 32+ 23,  128, MVB_PV_703_OID },  /*  97 */
////    { 0xE0000704, CCUM_ST_ID,    NC_MVB_DS_32_BYTES + 704, 32+ 24,  128, MVB_PV_704_OID },  /*  97 */
////    { 0xE0000705, CCUM_ST_ID,    NC_MVB_DS_32_BYTES + 705, 32+ 25,  128, MVB_PV_705_OID },  /*  97 */
////    { 0xE0000706, CCUM_ST_ID,    NC_MVB_DS_32_BYTES + 706, 32+ 26,  128, MVB_PV_706_OID },  /*  97 */
////    { 0xE0000708, CCUS_ST_ID,    NC_MVB_DS_32_BYTES + 708, 32+ 28,  128, MVB_PV_708_OID },  /*  97 */
////    { 0xE0000709, RIOAM_ST_ID,   NC_MVB_DS_32_BYTES + 709, 32+ 29,  128, MVB_PV_709_OID },  /*  97 */
////    { 0xE0000710, RIOAS_ST_ID,   NC_MVB_DS_32_BYTES + 710, 32+ 30,  128, MVB_PV_710_OID },  /*  97 */
////    { 0xE0000711, RIOBM_ST_ID,   NC_MVB_DS_32_BYTES + 711, 32+ 31,  128, MVB_PV_711_OID },  /*  97 */
////    { 0xE0000712, RIOBS_ST_ID,   NC_MVB_DS_32_BYTES + 712, 32+ 19,  128, MVB_PV_712_OID },  /*  97 */
////    { 0xE0000713, RIOCM_ST_ID,   NC_MVB_DS_32_BYTES + 713, 32+ 14,  128, MVB_PV_713_OID },  /*  97 */
////    { 0xE0000714, RIOCS_ST_ID,   NC_MVB_DS_32_BYTES + 714, 32+ 15,  128, MVB_PV_714_OID }  /*  97 */

},


/*----------------------------------------------------------------------------------------------*/
/* Datasets 501/502/503 */
//
//{ MVB_PV_501_OID, MVB_PV_501_NUM * sizeof(PvEntry) }, { R1(00, 0) },
//{ MVB_PV_502_OID, MVB_PV_502_NUM * sizeof(PvEntry) }, { R2(00, 0) },
//{ MVB_PV_503_OID, MVB_PV_503_NUM * sizeof(PvEntry) }, { R3(00, 0) },
//

/*----------------------------------------------------------------------------------------------*/
/* Datasets 300-322 */

//{ MVB_PV_300_OID, MVB_PV_300_NUM * sizeof(PvEntry) }, { SOURCE_DIR_WITH_CHK(01) },
//{ MVB_PV_301_OID, MVB_PV_301_NUM * sizeof(PvEntry) }, { R3(01, 0) },
//{ MVB_PV_302_OID, MVB_PV_302_NUM * sizeof(PvEntry) }, { R3(02, 0) },
//{ MVB_PV_303_OID, MVB_PV_303_NUM * sizeof(PvEntry) }, { R3(03, 0) },
//{ MVB_PV_304_OID, MVB_PV_304_NUM * sizeof(PvEntry) }, { R3(04, 0) },
//{ MVB_PV_305_OID, MVB_PV_305_NUM * sizeof(PvEntry) }, { R3(05, 0) },
//{ MVB_PV_306_OID, MVB_PV_306_NUM * sizeof(PvEntry) }, { R3(06, 0) },
//{ MVB_PV_307_OID, MVB_PV_307_NUM * sizeof(PvEntry) }, { R3(07, 0) },
//{ MVB_PV_308_OID, MVB_PV_308_NUM * sizeof(PvEntry) }, { R3(08, 0) },
//{ MVB_PV_309_OID, MVB_PV_309_NUM * sizeof(PvEntry) }, { R3(09, 0) },
//{ MVB_PV_310_OID, MVB_PV_310_NUM * sizeof(PvEntry) }, { R3(0A, 0) },
//{ MVB_PV_311_OID, MVB_PV_311_NUM * sizeof(PvEntry) }, { R3(0B, 0) },
//{ MVB_PV_312_OID, MVB_PV_312_NUM * sizeof(PvEntry) }, { R3(0C, 0) },
//{ MVB_PV_313_OID, MVB_PV_313_NUM * sizeof(PvEntry) }, { R3(0D, 0) },
//{ MVB_PV_314_OID, MVB_PV_314_NUM * sizeof(PvEntry) }, { R3(0E, 0) },
//{ MVB_PV_315_OID, MVB_PV_315_NUM * sizeof(PvEntry) }, { R3(0F, 0) },
//{ MVB_PV_316_OID, MVB_PV_316_NUM * sizeof(PvEntry) }, { R3(10, 0) },
//{ MVB_PV_317_OID, MVB_PV_317_NUM * sizeof(PvEntry) }, { R3(11, 0) },
//{ MVB_PV_318_OID, MVB_PV_318_NUM * sizeof(PvEntry) }, { R3(12, 0) },
//{ MVB_PV_319_OID, MVB_PV_319_NUM * sizeof(PvEntry) }, { R3(13, 0) },
//{ MVB_PV_320_OID, MVB_PV_320_NUM * sizeof(PvEntry) }, { R3(14, 0) },
//{ MVB_PV_321_OID, MVB_PV_321_NUM * sizeof(PvEntry) }, { R3(15, 0) },
//{ MVB_PV_322_OID, MVB_PV_322_NUM * sizeof(PvEntry) }, { R3(16, 0) },
//
//
///*----------------------------------------------------------------------------------------------*/
///* Datasets 350-356/330-336 */
//
//{ MVB_PV_350_OID, MVB_PV_350_NUM * sizeof(PvEntry) }, { SOURCE_DIR_WITH_CHK(02) },
//{ MVB_PV_351_OID, MVB_PV_351_NUM * sizeof(PvEntry) }, { R2(01, 0) },
//{ MVB_PV_352_OID, MVB_PV_352_NUM * sizeof(PvEntry) }, { R2(02, 0) },
//{ MVB_PV_353_OID, MVB_PV_353_NUM * sizeof(PvEntry) }, { R2(03, 0) },
//{ MVB_PV_354_OID, MVB_PV_354_NUM * sizeof(PvEntry) }, { R2(04, 0) },
//{ MVB_PV_355_OID, MVB_PV_355_NUM * sizeof(PvEntry) }, { R2(05, 0) },
//{ MVB_PV_356_OID, MVB_PV_356_NUM * sizeof(PvEntry) }, { R2(06, 0) },
//{ MVB_PV_330_OID, MVB_PV_330_NUM * sizeof(PvEntry) }, { SOURCE_DIR_WITH_CHK(03) },
//{ MVB_PV_331_OID, MVB_PV_331_NUM * sizeof(PvEntry) }, { R3(21, 0) },
//{ MVB_PV_332_OID, MVB_PV_332_NUM * sizeof(PvEntry) }, { R3(22, 0) },
//{ MVB_PV_333_OID, MVB_PV_333_NUM * sizeof(PvEntry) }, { R3(23, 0) },
//{ MVB_PV_334_OID, MVB_PV_334_NUM * sizeof(PvEntry) }, { R3(24, 0) },
//{ MVB_PV_335_OID, MVB_PV_335_NUM * sizeof(PvEntry) }, { R3(25, 0) },
//{ MVB_PV_336_OID, MVB_PV_335_NUM * sizeof(PvEntry) }, { R3(26, 0) },
//
//
///*----------------------------------------------------------------------------------------------*/
///* Datasets 370/371 */
//
//{ MVB_PV_370_OID, MVB_PV_370_NUM * sizeof(PvEntry) }, { R1(01, 0) },
//{ MVB_PV_371_OID, MVB_PV_371_NUM * sizeof(PvEntry) }, { R3(30, 0) },
//
//
///*----------------------------------------------------------------------------------------------*/
///* Datasets 590/390/391 */
//
//{ MVB_PV_590_OID, MVB_PV_590_NUM * sizeof(PvEntry) }, { GTW_DSI },
{ MVB_PV_390_OID, MVB_PV_390_NUM * sizeof(PvEntry) }, { GTW_DSO },
//{ MVB_PV_391_OID, MVB_PV_391_NUM * sizeof(PvEntry) }, { GTW_VID },


/*----------------------------------------------------------------------------------------------*/
/* Datasets 360/400 */

//{ MVB_PV_360_OID, MVB_PV_360_NUM * sizeof(PvEntry) }, { GTW_DSM },
//{ MVB_PV_400_OID, MVB_PV_400_NUM * sizeof(PvEntry) }, { GTW_DSS },


/*----------------------------------------------------------------------------------------------*/
/* Datasets 551/601 */

//{ MVB_PV_551_OID, MVB_PV_551_NUM * sizeof(PvEntry) },
//
//{
//	{ 0x05000100, NC_ANTIVALENT2,    8* 0+0, 0     	    },	/*  1 */
//	{ 0x05000200, NC_UNSIGNED8,      8* 1+0, 0x05000100 },	/*  2 */
//    { 0x05000300, NC_BOOLEAN1,       8* 2+0, 0x05000100 },  /*  3 */
//    { 0x05000400, NC_BOOLEAN1,       8* 2+1, 0x05000100 },  /*  4 */
//    { 0x05000500, NC_BOOLEAN1,       8* 2+2, 0x05000100 }   /*  5 */
//},
//
//{ MVB_PV_601_OID, MVB_PV_601_NUM * sizeof(PvEntry) },
//
//{
//	{ 0x05010100, NC_ANTIVALENT2,    8* 0+0, 0     	    },	/*  1 */
//	{ 0x05010200, NC_UNSIGNED8,      8* 1+0, 0x05010100 },	/*  2 */
//    { 0x05010300, NC_BOOLEAN1,       8* 2+0, 0x05010100 },  /*  3 */
//    { 0x05010400, NC_BOOLEAN1,       8* 2+1, 0x05010100 },  /*  4 */
//    { 0x05010500, NC_BOOLEAN1,       8* 2+2, 0x05010100 }   /*  5 */
//},
//
//
//
///*----------------------------------------------------------------------------------------------*/
///* Datasets 801/2/3/6/7/8 */
//
//{ MVB_PV_801_OID, MVB_PV_801_NUM * sizeof(PvEntry) }, { R_S (01) },
//{ MVB_PV_802_OID, MVB_PV_802_NUM * sizeof(PvEntry) }, { R_AI(01) },
//{ MVB_PV_803_OID, MVB_PV_803_NUM * sizeof(PvEntry) }, { R_DI(01) },
//
//{ MVB_PV_851_OID, MVB_PV_851_NUM * sizeof(PvEntry) }, { R_S (05) },
//{ MVB_PV_852_OID, MVB_PV_852_NUM * sizeof(PvEntry) }, { R_AI(05) },
//{ MVB_PV_853_OID, MVB_PV_853_NUM * sizeof(PvEntry) }, { R_DI(05) },
//
//
///*----------------------------------------------------------------------------------------------*/
///* Datasets 411/2/3/4/6/7/8 */
//
//{ MVB_PV_811_OID, MVB_PV_811_NUM * sizeof(PvEntry) }, { R_S (02) },
//{ MVB_PV_812_OID, MVB_PV_812_NUM * sizeof(PvEntry) }, { R_AI(02) },
//{ MVB_PV_813_OID, MVB_PV_813_NUM * sizeof(PvEntry) }, { R_DI(02) },
//{ MVB_PV_814_OID, MVB_PV_814_NUM * sizeof(PvEntry) }, { R_DO(02) },
//
//{ MVB_PV_861_OID, MVB_PV_861_NUM * sizeof(PvEntry) }, { R_S (06) },
//{ MVB_PV_862_OID, MVB_PV_862_NUM * sizeof(PvEntry) }, { R_AI(06) },
//{ MVB_PV_863_OID, MVB_PV_863_NUM * sizeof(PvEntry) }, { R_DI(06) },
//
//
///*----------------------------------------------------------------------------------------------*/
///* Datasets 421/2/3/4/6/7/8 */
//
//{ MVB_PV_821_OID, MVB_PV_821_NUM * sizeof(PvEntry) }, { R_S (03) },
//{ MVB_PV_822_OID, MVB_PV_822_NUM * sizeof(PvEntry) }, { R_AI(03) },
//{ MVB_PV_823_OID, MVB_PV_823_NUM * sizeof(PvEntry) }, { R_DI(03) },
//{ MVB_PV_824_OID, MVB_PV_824_NUM * sizeof(PvEntry) }, { R_DO(03) },
//
//{ MVB_PV_871_OID, MVB_PV_871_NUM * sizeof(PvEntry) }, { R_S (07) },
//{ MVB_PV_872_OID, MVB_PV_872_NUM * sizeof(PvEntry) }, { R_AI(07) },
//{ MVB_PV_873_OID, MVB_PV_873_NUM * sizeof(PvEntry) }, { R_DI(07) },
//
//
///*----------------------------------------------------------------------------------------------*/
///* Datasets 431/2/3/4/6/7/8 */
//
//{ MVB_PV_831_OID, MVB_PV_831_NUM * sizeof(PvEntry) }, { R_S (04) },
//{ MVB_PV_832_OID, MVB_PV_832_NUM * sizeof(PvEntry) }, { R_AI(04) },
//{ MVB_PV_833_OID, MVB_PV_833_NUM * sizeof(PvEntry) }, { R_DI(04) },
//{ MVB_PV_834_OID, MVB_PV_834_NUM * sizeof(PvEntry) }, { R_DO(04) },
//
//{ MVB_PV_881_OID, MVB_PV_881_NUM * sizeof(PvEntry) }, { R_S (08) },
//{ MVB_PV_882_OID, MVB_PV_882_NUM * sizeof(PvEntry) }, { R_AI(08) },
//{ MVB_PV_883_OID, MVB_PV_883_NUM * sizeof(PvEntry) }, { R_DI(08) },
//
//
///*----------------------------------------------------------------------------------------------*/
///* Datasets 180/1 */
//
//{ MVB_PV_180_OID, MVB_PV_180_NUM * sizeof(PvEntry) }, { TCU_I(01) },
//{ MVB_PV_181_OID, MVB_PV_181_NUM * sizeof(PvEntry) }, { TCU_I(02) },
//
///*----------------------------------------------------------------------------------------------*/
///* Datasets 540/1/2 */
//
//{ MVB_PV_540_OID, MVB_PV_540_NUM * sizeof(PvEntry) }, { TCUA_O(01) },
//{ MVB_PV_541_OID, MVB_PV_541_NUM * sizeof(PvEntry) }, { TCUB_O(01) },
//{ MVB_PV_542_OID, MVB_PV_542_NUM * sizeof(PvEntry) }, { TCUC_O(01) },
//{ MVB_PV_543_OID, MVB_PV_543_NUM * sizeof(PvEntry) }, { TCUD_O(01) },
//{ MVB_PV_544_OID, MVB_PV_544_NUM * sizeof(PvEntry) }, { TCUE_O(01) },
//
///*----------------------------------------------------------------------------------------------*/
///* Datasets 560/1/2 */
//
//{ MVB_PV_560_OID, MVB_PV_560_NUM * sizeof(PvEntry) }, { TCUA_O(02) },
//{ MVB_PV_561_OID, MVB_PV_561_NUM * sizeof(PvEntry) }, { TCUB_O(02) },
//{ MVB_PV_562_OID, MVB_PV_562_NUM * sizeof(PvEntry) }, { TCUC_O(02) },
//{ MVB_PV_563_OID, MVB_PV_563_NUM * sizeof(PvEntry) }, { TCUD_O(02) },
//{ MVB_PV_564_OID, MVB_PV_564_NUM * sizeof(PvEntry) }, { TCUE_O(02) },
//
///*----------------------------------------------------------------------------------------------*/
///* Datasets 600 */
//
//{ MVB_PV_600_OID, MVB_PV_600_NUM * sizeof(PvEntry) }, { CGS_I(01) },
//
///*----------------------------------------------------------------------------------------------*/
///* Datasets 602/3/4 */
//
//{ MVB_PV_602_OID, MVB_PV_602_NUM * sizeof(PvEntry) }, { CGSA_O(01) },
//{ MVB_PV_603_OID, MVB_PV_603_NUM * sizeof(PvEntry) }, { CGSA_O(02) },
//{ MVB_PV_604_OID, MVB_PV_604_NUM * sizeof(PvEntry) }, { CGSA_O(03) },
//
///*----------------------------------------------------------------------------------------------*/
///* Datasets 605/6/7 */
//
//{ MVB_PV_605_OID, MVB_PV_605_NUM * sizeof(PvEntry) }, { CGSB_O(01) },
//{ MVB_PV_606_OID, MVB_PV_606_NUM * sizeof(PvEntry) }, { CGSB_O(02) },
//{ MVB_PV_607_OID, MVB_PV_607_NUM * sizeof(PvEntry) }, { CGSB_O(03) },
//
//
///*----------------------------------------------------------------------------------------------*/
///* Datasets 1110/1111 */
//
//{ MVB_PV_1110_OID, MVB_PV_1110_NUM * sizeof(PvEntry) }, { BCU_IN1(01) },
//{ MVB_PV_1111_OID, MVB_PV_1111_NUM * sizeof(PvEntry) }, { BCU_IN2(01) },
//
///*----------------------------------------------------------------------------------------------*/
///* Datasets 1115-1118 */
//
//{ MVB_PV_1115_OID, MVB_PV_1115_NUM * sizeof(PvEntry) }, { BCU_OUT1(01) },
//{ MVB_PV_1116_OID, MVB_PV_1116_NUM * sizeof(PvEntry) }, { BCU_OUT2(01) },
//{ MVB_PV_1117_OID, MVB_PV_1117_NUM * sizeof(PvEntry) }, { BCU_OUT3(01) },
//{ MVB_PV_1118_OID, MVB_PV_1118_NUM * sizeof(PvEntry) }, { BCU_OUT4(01) },
//
//
///*----------------------------------------------------------------------------------------------*/
///* Datasets 200/201 */
//
//{ MVB_PV_200_OID, MVB_PV_200_NUM * sizeof(PvEntry) }, { ORA_IDU(01) },
//{ MVB_PV_201_OID, MVB_PV_201_NUM * sizeof(PvEntry) }, { ORA_IDU(02) },
//
///*----------------------------------------------------------------------------------------------*/
///* Datasets 210/214 */
//
//{ MVB_PV_209_OID, MVB_PV_209_NUM * sizeof(PvEntry) }, { IDU1_I(209) },
//{ MVB_PV_210_OID, MVB_PV_210_NUM * sizeof(PvEntry) }, { IDU1_I(210) },
//{ MVB_PV_211_OID, MVB_PV_211_NUM * sizeof(PvEntry) }, { IDU2_I(211) },
//{ MVB_PV_212_OID, MVB_PV_212_NUM * sizeof(PvEntry) }, { IDU3_I(212) },
//{ MVB_PV_213_OID, MVB_PV_213_NUM * sizeof(PvEntry) }, { IDU4_I(213) },
//{ MVB_PV_214_OID, MVB_PV_214_NUM * sizeof(PvEntry) }, { IDU5_I(214) },
//{ MVB_PV_215_OID, MVB_PV_215_NUM * sizeof(PvEntry) }, { IDU6_I(215) },
//{ MVB_PV_216_OID, MVB_PV_216_NUM * sizeof(PvEntry) }, { IDU7_I(216) },
//{ MVB_PV_217_OID, MVB_PV_217_NUM * sizeof(PvEntry) }, { IDU8_I(217) },
//
///*----------------------------------------------------------------------------------------------*/
///* Datasets 700/714 */
///*
//{ MVB_PV_700_OID, MVB_PV_700_NUM * sizeof(PvEntry) }, { MESS(700) },
//{ MVB_PV_701_OID, MVB_PV_701_NUM * sizeof(PvEntry) }, { MESS(701) },
//{ MVB_PV_702_OID, MVB_PV_702_NUM * sizeof(PvEntry) }, { MESS(702) },
//{ MVB_PV_703_OID, MVB_PV_703_NUM * sizeof(PvEntry) }, { MESS(703) },
//{ MVB_PV_704_OID, MVB_PV_704_NUM * sizeof(PvEntry) }, { MESS(704) },
//{ MVB_PV_705_OID, MVB_PV_705_NUM * sizeof(PvEntry) }, { MESS(705) },
//{ MVB_PV_706_OID, MVB_PV_706_NUM * sizeof(PvEntry) }, { MESS(706) },
//
//{ MVB_PV_708_OID, MVB_PV_708_NUM * sizeof(PvEntry) }, { MESS(708) },
//{ MVB_PV_709_OID, MVB_PV_709_NUM * sizeof(PvEntry) }, { MESS(709) },
//{ MVB_PV_710_OID, MVB_PV_710_NUM * sizeof(PvEntry) }, { MESS(710) },
//{ MVB_PV_711_OID, MVB_PV_711_NUM * sizeof(PvEntry) }, { MESS(711) },
//{ MVB_PV_712_OID, MVB_PV_712_NUM * sizeof(PvEntry) }, { MESS(712) },
//{ MVB_PV_713_OID, MVB_PV_713_NUM * sizeof(PvEntry) }, { MESS(713) },
//{ MVB_PV_714_OID, MVB_PV_714_NUM * sizeof(PvEntry) }, { MESS(714) },
//*/
//
///*----------------------------------------------------------------------------------------------*/
///* WTB datasets */
//
//{ WTB_DS_OID, WTB_DS_NUM * sizeof(WtbDsEntry) },
//
//{
//	{ 0xE0100000, 0xFFFF, 128, WTB_PV_R0_OID,    2, NC_WTB_DS_PERIOD_128 },	/*  1 */
//	{ 0xE0100001, 0x1101, 128, WTB_PV_R1_OID,  128, NC_WTB_DS_PERIOD_1   },	/*  2 */
//	{ 0xE0100002, 0x2101, 128, WTB_PV_R2_OID,  128, NC_WTB_DS_PERIOD_1   },	/*  3 */
//	{ 0xE0100003, 0x3101, 128, WTB_PV_R3_OID,   40, NC_WTB_DS_PERIOD_4   },	/*  4 */
//	{ 0xE0100011, 0x1001, 128, WTB_PV_R1X_OID,  64, NC_WTB_DS_PERIOD_1   },	/*  5 */
//	{ 0xE0100012, 0x2001, 128, WTB_PV_R2X_OID,  64, NC_WTB_DS_PERIOD_1   },	/*  6 */
//	{ 0xE0100013, 0x3001, 128, WTB_PV_R3X_OID,  64, NC_WTB_DS_PERIOD_4   }	/*  7 */
//},
//
//
///*----------------------------------------------------------------------------------------------*/
///* Datasets R0 / R1 / R2 / R3 / R1* / R2* / R3* */
//
//{ WTB_PV_R0_OID,  WTB_PV_R0_NUM  * sizeof(PvEntry) }, { R0(00, 0)             },
//
//{ WTB_PV_R1_OID,  WTB_PV_R1_NUM  * sizeof(PvEntry) }, { R3(01, 0), R1(01, 40) },
//{ WTB_PV_R2_OID,  WTB_PV_R2_NUM  * sizeof(PvEntry) }, { R3(02, 0), R2(02, 40) },
//{ WTB_PV_R3_OID,  WTB_PV_R3_NUM  * sizeof(PvEntry) }, { R3(03, 0)             },
//
//{ WTB_PV_R1X_OID, WTB_PV_R1X_NUM * sizeof(PvEntry) }, { R3(11, 0), R1(11, 40) },
//{ WTB_PV_R2X_OID, WTB_PV_R2X_NUM * sizeof(PvEntry) }, { R3(12, 0), R2(12, 40) },
//{ WTB_PV_R3X_OID, WTB_PV_R3X_NUM * sizeof(PvEntry) }, { R3(13, 0)             },
//
//
///*----------------------------------------------------------------------------------------------*/
///* Process Data Marshalling */
//
//{ PDM_OID, PDM_NUM * sizeof(PdmEntry) },
//
//{
//	{ NC_PDM_IMPORT, 0x0000, 20, 0, PDM_ACT_IMPORT_OID,     0                     }, /*  1 */
//	{ NC_PDM_EXPORT, 0x0000, 20, 0, PDM_ACT_EXPORT_R0_OID,  PDM_DV_EXPORT_R0_OID  }, /*  2 */
//	{ NC_PDM_IMPORT, 0x0001, 20, 0, PDM_ACT_IMPORT_OID,     0                     }, /*  3 */
//	{ NC_PDM_EXPORT, 0x0001, 20, 0, PDM_ACT_EXPORT_R1_OID,  PDM_DV_EXPORT_R1_OID  }, /*  4 */
//	{ NC_PDM_IMPORT, 0x0002, 20, 0, PDM_ACT_IMPORT_OID,     0                     }, /*  5 */
//	{ NC_PDM_EXPORT, 0x0002, 20, 0, PDM_ACT_EXPORT_R2_OID,  PDM_DV_EXPORT_R2_OID  }, /*  6 */
//	{ NC_PDM_IMPORT, 0x0003, 20, 0, PDM_ACT_IMPORT_OID,     0                     }, /*  7 */
//	{ NC_PDM_EXPORT, 0x0003, 20, 0, PDM_ACT_EXPORT_R3_OID,  PDM_DV_EXPORT_R3_OID  }, /*  8 */
//	{ NC_PDM_IMPORT, 0x0101, 20, 0, PDM_ACT_IMPORTX_OID,    0                     }, /*  9 */
//	{ NC_PDM_EXPORT, 0x0101, 20, 0, PDM_ACT_EXPORT_R1X_OID, PDM_DV_EXPORT_R1X_OID }, /* 10 */
//	{ NC_PDM_IMPORT, 0x0102, 20, 0, PDM_ACT_IMPORTX_OID,    0                     }, /* 11 */
//	{ NC_PDM_EXPORT, 0x0102, 20, 0, PDM_ACT_EXPORT_R2X_OID, PDM_DV_EXPORT_R2X_OID }, /* 12 */
//	{ NC_PDM_IMPORT, 0x0103, 20, 0, PDM_ACT_IMPORTX_OID,    0                     }, /* 13 */
//	{ NC_PDM_EXPORT, 0x0103, 20, 0, PDM_ACT_EXPORT_R3X_OID, PDM_DV_EXPORT_R3X_OID }, /* 14 */
//},
//
//
///*----------------------------------------------------------------------------------------------*/
///* PDM import actions for TCN */
//
//{ PDM_ACT_IMPORT_OID, PDM_ACT_IMPORT_NUM * sizeof(PdmActEntry) },
//
//{
//	{ 0x08510101, NC_PDM_SOURCE, 0,                 0  },	/*  1 */
//	{ 0x08510101, NC_PDM_COPY,   NC_PDM_COPY_NORM,  0  }, 	/*  2 */
//	{ 0x08530101, NC_PDM_SOURCE, 0,                 0  },	/*  3 */
//	{ 0x08530130, NC_PDM_COPY,   NC_PDM_COPY_NORM,  0  }, 	/*  4 */
//
//	{ 0x08520102, NC_PDM_SOURCE, 0,                 0  },	/*  5 */
//	{ 0x08520101, NC_PDM_COPY,   NC_PDM_COPY_S_DIR, 5  }, 	/*  6 */
//	{ 0x08530102, NC_PDM_SOURCE, 0,                 0  },	/*  7 */
//	{ 0x08530121, NC_PDM_COPY,   NC_PDM_COPY_S_DIR, 5  }, 	/*  8 */
//
//	{ 0x08530103, NC_PDM_SOURCE, 0,                 0  },	/*  9 */
//	{ 0x08530101, NC_PDM_COPY,   NC_PDM_COPY_S_DIR, 22 } 	/* 10 */
//},
//
//
///*----------------------------------------------------------------------------------------------*/
///* PDM import actions for TCN* */
//
//{ PDM_ACT_IMPORTX_OID, PDM_ACT_IMPORTX_NUM * sizeof(PdmActEntry) },
//
//{
//	{ 0x08510111, NC_PDM_SOURCE, 0,                 0  },	/*  1 */
//	{ 0x08510101, NC_PDM_COPY,   NC_PDM_COPY_NORM,  0  }, 	/*  2 */
//	{ 0x08530111, NC_PDM_SOURCE, 0,                 0  },	/*  3 */
//	{ 0x08530130, NC_PDM_COPY,   NC_PDM_COPY_NORM,  0  }, 	/*  4 */
//
//	{ 0x08520112, NC_PDM_SOURCE, 0,                 0  },	/*  5 */
//	{ 0x08520101, NC_PDM_COPY,   NC_PDM_COPY_S_DIR, 5  }, 	/*  6 */
//	{ 0x08530112, NC_PDM_SOURCE, 0,                 0  },	/*  7 */
//	{ 0x08530121, NC_PDM_COPY,   NC_PDM_COPY_S_DIR, 5  }, 	/*  8 */
//
//	{ 0x08530113, NC_PDM_SOURCE, 0,                 0  },	/*  9 */
//	{ 0x08530101, NC_PDM_COPY,   NC_PDM_COPY_S_DIR, 22 } 	/* 10 */
//},
//
//
///*----------------------------------------------------------------------------------------------*/
///* PDM export R0 mode */
//
//{ PDM_ACT_EXPORT_R0_OID, PDM_ACT_EXPORT_R0_NUM * sizeof(PdmActEntry) },
//
//{
//	{ 0x09700100, NC_PDM_DFLT_VALUE, 0, 0 },	/*  1 */
//	{ 0x09700200, NC_PDM_DFLT_VALUE, 0, 1 } 	/*  2 */
//},
//
//{ PDM_DV_EXPORT_R0_OID, PDM_DV_EXPORT_R0_NUM * sizeof(char) },
//
//{ 0xFF, 0xFF },
//
//
///*----------------------------------------------------------------------------------------------*/
///* PDM export R1 mode */
//
//{ PDM_ACT_EXPORT_R1_OID, PDM_ACT_EXPORT_R1_NUM * sizeof(PdmActEntry) },
//
//{
//	{ 0x09730101, NC_PDM_DFLT_VALUE, 0, 0 },	/*  1 */
//	{ 0x09730201, NC_PDM_DFLT_VALUE, 0, 1 }, 	/*  2 */
//	{ 0x08530300, NC_PDM_SOURCE,     0, 0 },	/*  3 */
//	{ 0x08530301, NC_PDM_COPY,       0, 0 }, 	/*  4 */
//	{ 0x08510200, NC_PDM_SOURCE,     0, 0 },	/*  5 */
//	{ 0x08510201, NC_PDM_COPY,       0, 0 } 	/*  6 */
//},
//
//{ PDM_DV_EXPORT_R1_OID, PDM_DV_EXPORT_R1_NUM * sizeof(char) },
//
//{ 0x11, 0x01 },
//
//
///*----------------------------------------------------------------------------------------------*/
///* PDM export R2 mode */
//
//{ PDM_ACT_EXPORT_R2_OID, PDM_ACT_EXPORT_R2_NUM * sizeof(PdmActEntry) },
//
//{
//	{ 0x09730102, NC_PDM_DFLT_VALUE, 0, 0 },	/*  1 FTF */
//	{ 0x09730202, NC_PDM_DFLT_VALUE, 0, 1 }, 	/*  2     */
//	{ 0x08530300, NC_PDM_SOURCE,     0, 0 },	/*  3 R3  */
//	{ 0x08530302, NC_PDM_COPY,       0, 0 }, 	/*  4      */
//	{ 0x08520200, NC_PDM_SOURCE,     0, 0 },	/*  5     */
//	{ 0x08520202, NC_PDM_COPY,       0, 0 } 	/*  6  */
//},
//
//{ PDM_DV_EXPORT_R2_OID, PDM_DV_EXPORT_R2_NUM * sizeof(char) },
//
//{ 0x21, 0x01 },
//
//
///*----------------------------------------------------------------------------------------------*/
///* PDM export R3 mode */
//
//{ PDM_ACT_EXPORT_R3_OID, PDM_ACT_EXPORT_R3_NUM * sizeof(PdmActEntry) },
//
//{
//	{ 0x09730103, NC_PDM_DFLT_VALUE, 0, 0 },	/*  1 */
//	{ 0x09730203, NC_PDM_DFLT_VALUE, 0, 1 }, 	/*  2 */
//	{ 0x08530300, NC_PDM_SOURCE,     0, 0 },	/*  3 */
//	{ 0x08530303, NC_PDM_COPY,       0, 0 } 	/*  4 */
//},
//
//{ PDM_DV_EXPORT_R3_OID, PDM_DV_EXPORT_R3_NUM * sizeof(char) },
//
//{ 0x31, 0x01 },
//
//
///*----------------------------------------------------------------------------------------------*/
///* PDM export R1* mode */
//
//{ PDM_ACT_EXPORT_R1X_OID, PDM_ACT_EXPORT_R1X_NUM * sizeof(PdmActEntry) },
//
//{
//	{ 0x09730111, NC_PDM_DFLT_VALUE, 0, 0 },	/*  1 */
//	{ 0x09730211, NC_PDM_DFLT_VALUE, 0, 1 }, 	/*  2 */
//	{ 0x08530300, NC_PDM_SOURCE,     0, 0 },	/*  3 */
//	{ 0x08530311, NC_PDM_COPY,       0, 0 }, 	/*  4 */
//	{ 0x08510200, NC_PDM_SOURCE,     0, 0 },	/*  5 */
//	{ 0x08510211, NC_PDM_COPY,       0, 0 } 	/*  6 */
//},
//
//{ PDM_DV_EXPORT_R1X_OID, PDM_DV_EXPORT_R1X_NUM * sizeof(char) },
//
//{ 0x10, 0x01 },
//
//
///*----------------------------------------------------------------------------------------------*/
///* PDM export R2* mode */
//
//{ PDM_ACT_EXPORT_R2X_OID, PDM_ACT_EXPORT_R2X_NUM * sizeof(PdmActEntry) },
//
//{
//	{ 0x09730112, NC_PDM_DFLT_VALUE, 0, 0 },	/*  1 */
//	{ 0x09730212, NC_PDM_DFLT_VALUE, 0, 1 }, 	/*  2 */
//	{ 0x08530300, NC_PDM_SOURCE,     0, 0 },	/*  3 */
//	{ 0x08530312, NC_PDM_COPY,       0, 0 }, 	/*  4 */
//	{ 0x08520200, NC_PDM_SOURCE,     0, 0 },	/*  5 */
//	{ 0x08520212, NC_PDM_COPY,       0, 0 } 	/*  6 */
//},
//
//{ PDM_DV_EXPORT_R2X_OID, PDM_DV_EXPORT_R2X_NUM * sizeof(char) },
//
//{ 0x20, 0x01 },
//
//
///*----------------------------------------------------------------------------------------------*/
///* PDM export R3* mode */
//
//{ PDM_ACT_EXPORT_R3X_OID, PDM_ACT_EXPORT_R3X_NUM * sizeof(PdmActEntry) },
//
//{
//	{ 0x09730113, NC_PDM_DFLT_VALUE, 0, 0 },	/*  1 */
//	{ 0x09730213, NC_PDM_DFLT_VALUE, 0, 1 }, 	/*  2 */
//	{ 0x08530300, NC_PDM_SOURCE,     0, 0 },	/*  3 */
//	{ 0x08530313, NC_PDM_COPY,       0, 0 } 	/*  4 */
//},
//
//{ PDM_DV_EXPORT_R3X_OID, PDM_DV_EXPORT_R3X_NUM * sizeof(char) },
//
//{ 0x30, 0x01 },


/*----------------------------------------------------------------------------------------------*/
/* End of database */

{ NC_END_OF_DB_OBJ_ID, 0 }

};


/*==============================================================================================*/
/* Globals */

const NcObjHeader *nc_db_ptr = (const NcObjHeader *)&nc_db;


/*==============================================================================================*/


