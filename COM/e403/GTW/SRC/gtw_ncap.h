/*=============================================================================================*/
/* Gateway NCAP                                                 					           */
/* Header file (gtw_ncap.h)													                   */
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

/* define the extern macro if not supplied */
#ifndef EXTERN
#define EXTERN extern
#endif


/*==============================================================================================*/
/* Include the definitions */

#include "gtw_ncdb.h"


/*==============================================================================================*/
/* Application definitions */

/*----------------------------------------------------------------------------------------------*/
/* Typedefs */

typedef struct {
	unsigned char	uwtm_conflict;
	unsigned char	uwtm_opmode;
	unsigned char	uwtm_state;
	unsigned char	uwtm_leading;
	unsigned char	uwtm_topo_count;
	unsigned char	uwtm_number_vehicles;
	unsigned char	uwtm_orientation;
	unsigned short	uwtm_pdm_mode;
	unsigned char	driving_direction;
	unsigned char	comm;
	unsigned char	sleout;
	unsigned char	mvbon;
	unsigned char	key;
	unsigned char	carbaton;
	unsigned char	lucion;
	unsigned char	recon;
	unsigned char	rstretein;
	unsigned char	velosit;
	unsigned char	stato_mt;
	unsigned char	risin1;
	unsigned char	risin2;
	unsigned char	risin3;
	unsigned char	risin4;
	unsigned char	risin5;
	unsigned char	risin6;
	unsigned char	risout1;
	unsigned char	risout2;
	unsigned char	risout3;
	unsigned char	risout4;
	unsigned char	out_vita;
	unsigned char	status_standby;
	unsigned char	mvb_red;
	unsigned char	line_A1_disturbed;
	unsigned char	line_B1_disturbed;
	unsigned char	line_A2_disturbed;
	unsigned char	line_B2_disturbed;
	unsigned char	line_A_HW_Fault;
	unsigned char	line_B_HW_Fault;
	unsigned char	chk;

    unsigned char  	uic[5];
	unsigned char	logico[5];
	unsigned char	vd_mt[5];
	unsigned char	nadi_valid;     // NADI valido
	unsigned char	v_app;          // veicolo appartenenza

	unsigned char	vid[4];         // Vehicle ID

	unsigned char   Master_is_Z1;
	unsigned char   spare2;


} GtwDso;

typedef struct {
	unsigned char	uwtm_opmode_select;
	unsigned char	uwtm_leading_request;
	unsigned char	uwtm_leading_direc;
	unsigned char	uwtm_sleep_ctrl;
	unsigned char	uwtm_inaug_ctrl;
	unsigned short	uwtm_pdm_mode_set;
	unsigned char	comm;
	unsigned char	sleout;
	unsigned char	mvbon;
	unsigned char	key;
	unsigned char	carbaton;
	unsigned char	lucion;
	unsigned char	recon;
	unsigned char	rstretein;
	unsigned char	velosit;
	unsigned char	stato_mt;
	unsigned char	risin1;
	unsigned char	risin2;
	unsigned char	risin3;
	unsigned char	risin4;
	unsigned char	risin5;
	unsigned char	risin6;
	unsigned char	risout1;
	unsigned char	risout2;
	unsigned char	risout3;
	unsigned char	risout4;
	unsigned char	vid;        //Vehicle ID
	unsigned char	vita;
	unsigned char	chk;
} GtwDsi;

typedef struct {
	unsigned char	vita;
	unsigned char	uni1;
	unsigned char	tsthdw;
	unsigned char	unim;
	unsigned char	inhbt;
	unsigned char	chk;
} GtwDsm;

typedef struct {
	unsigned char	vita;
	unsigned char	comm;
	unsigned char	sleout;
	unsigned char	mvbon;
	unsigned char	key;
	unsigned char	carbaton;
	unsigned char	lucion;
	unsigned char	recon;
	unsigned char	rstretein;
	unsigned char	velosit;
	unsigned char	stato_mt;
	unsigned char	risin1;
	unsigned char	risin2;
	unsigned char	risin3;
	unsigned char	risin4;
	unsigned char	risin5;
	unsigned char	risin6;
	unsigned char	uni1;
	unsigned char	tsthdw;
	unsigned char	unim;
	unsigned char	inhbt;
	unsigned char	chk;
} GtwDss;

typedef struct
{
   unsigned char sd[8];
   unsigned char chk[8];
   unsigned short refresh;
} source_dir;

typedef struct
{
   unsigned char vid[16];
   unsigned char life;
   unsigned char chk;
} GtwVid;

/*----------------------------------------------------------------------------------------------*/
/* Variables */

EXTERN GtwDso gtw_dso;
EXTERN GtwDsi gtw_dsi;
EXTERN GtwDsm gtw_dsm;
EXTERN GtwDss gtw_dss;
EXTERN GtwVid gtw_vid;


/*----------------------------------------------------------------------------------------------*/
/* Datasets and refresh counters */

EXTERN TYPE_LP_DS_SET ds_gtw_dso, ds_gtw_dsi, ds_gtw_dsm, ds_gtw_dss, ds_gtw_vid, ds_330;
EXTERN unsigned short rf_gtw_dso, rf_gtw_dsi, rf_gtw_dsm, rf_gtw_dss, rf_gtw_vid;
EXTERN source_dir dir_loco_r3;

/*----------------------------------------------------------------------------------------------*/

