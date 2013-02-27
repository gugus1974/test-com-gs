/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> recorder() and err_rec() commented out                                              */
/* <ATR:02> WTB dependencies only if it is present                                              */
/* <ATR:03> E-telegrams must be packed on the gateway; the MVB stations unpacks them "manually" */
/* <ATR:04> SIZE_VH_DESCR fixed                                                                 */
/*==============================================================================================*/

/* <ATR:03> */
#ifdef ATR_WTB
#define ATR_PACKED packed
#else
#define ATR_PACKED
#endif


/*
   UUUUUUUUUUUUUUUUUUUU
   MAP_UMS Includefile
   Wolfgang Landwehr
   12. Oktober 1995, 11:06
   3
 */

/*
   GENERAL TOP OF FILE
 */

   /*
      Internal declaration of all constants, structures for MAP modules
    */

   /*
      02.02.94 W. Landwehr
      Start of developement.
    */

   /*
      Copyright (c) 1994-1995 by ABB-Henschel
    */


/*
   MODUL HISTORY
 */

   /*
      VERSION 1.xx
    */

	  /*
	     0.xx 08.04.94  W. Landwehr
	     Starting versions, starting point is:
	     UIC-MAPPING SERVER SPECIFICATION 1.0, Jan. 94   
	     1.00 14.04.94  W. Landwehr
	     NADI states included
	     1.01 03.05.94  W. Landwehr
	     Workaround after 1st integration.
	     1.02 26.05.94  W. Landwehr
	     Supplement VMB features, E2_TIMEOT 200->400
	     1.03 22.06.94  W. Landwehr      {---MAP 1.12--- on train}
	     Number_of_nodes add to VW structure.
	     1.04 10.08.94  W. Landwehr
	     Multicast structures.
	     1.05 21.09.94  W. Landwehr
	     Adaptions wich were agreed within workshop 14.9.
	     -parameter fct = unsigned char for map_multicast
	     1.06 21.11.94  W. Landwehr
	     MVB data added and include after node recovery.
	     1.07 07.12.94  W. Landwehr
	     more MVB additions after new experiences with MVB.
	     1.08 19.12.94  W. Landwehr
	     Workaround.
	     1.09 20.01.95  W. Landwehr
	     record buffer for update WTB is now 32 byte long
	     1.10 07.02.95  W. Landwehr
	     Delay time for uic inauguration adapted.
	   */


   /*
      2.00 30.03.95  W. Landwehr
      Adaptions for WTB 1.9
      2.01 05.04.95  W. Landwehr
      new SLAVE_DELAY_TIME 
      2.02 10.04.95  W. Landwehr
      E2_TIMEOUT = 1500   (old 400)
      2.03 03.05.95  W. Landwehr     {---MAP 2.07---}
      E2_TIMEOUT = 150    (old 1500)
      2.04 01.06.95  W. Landwehr     {---MAP 2.09--- on train}
      E2_TIMEOUT = 80     (old 150)
      2.05 12.10.95  W. Landwehr
      Adoptions for MyFriend, workaround, map_np.h + map_ums.h brought together
    */


/*
   DEFINES
 */

   /*
      GENERAL CODEs AND LENGTHENINGs
    */
#define NPT     0xAFFE			/* ID for Train bus module */
#define NPV     0xCAFE			/* ID for Vehicle bus module */
#define NP_ERR  0x9999			/* Fatal Erroor */

/* <ATR:04> */
// #define SIZE_VH_DESCR   66   /* member: vehicle descriptor */
#define SIZE_VH_DESCR   sizeof(map_VehicleDescr)	/* member: vehicle descriptor (64) */
#define SIZE_NN_STATE   8		/* member: NADI & Node state */
#define MAP_MAX_NO_MVB  8		/* maximum nodes on MVB */


   /*
      TIMER VALUES
    */
#define E2_TIMEOUT        80	/* timeout for am_call_requ */
										/* n x 64 msec ( based on RTP) */

	  /* Broadcast timeot for SETUP mode. If the Broadcast tgm not receives any slave,
	     so after this timeout the UIC inauguration finished. Condition is the 
	     succesful NADI distribution
	   */
#define DELAY_TIME        17	/* each node about 1.0 sec., base is 64ms */

	  /* Delays to calm down the system after an ls_t_StartNaming due to new UIC
	     inauguration.
	   */
#define MASTER_DELAY_TIME   4	/* wait until slaves and RTP are ready */
#define SLAVE_DELAY_TIME    1	/* wait until RTP canceled */


   /*
      MAP ERROR CODES
    */
#define MAP_ERR_SETUP_BR           0x9101
#define MAP_ERR_SETUP_REQ          0x9103
#define MAP_ERR_INCLUDE_NADI       0x9104
#define MAP_ERR_INCLUDE_GROUP_LIST 0x9105

#define MAP_ERR_SETUP_MASTER_NR    0x9200

#define MAP_ERR_SETUP_VEHBUS_NR    0x9300

#define MAP_ERR_CALL_CONFS_VEH     0x9401
#define MAP_ERR_CALL_CONFS_NADI    0x9402

#define MAP_ERR_RECV_CONFS_VEH     0x9501
#define MAP_ERR_RECV_CONFS_NADI    0x9502
#define MAP_ERR_RECV_CONFS_REQ     0x9503

#define MAP_ERR_UPDATE_BR          0x9601
#define MAP_ERR_UPDATE_RCV         0x9602
#define MAP_ERR_UPDATE_BR_MVB      0x9603
#define MAP_ERR_UPDATE_RCV_MVB     0x9604

#define MAP_ERR_CALL_CONFU         0x9701
#define MAP_ERR_CALL_CONFU_MVB     0x9702

#define MAP_ERR_RECV_CONFU_REQ     0x9803
#define MAP_ERR_RECV_CONFU_REQ_MVB 0x9804

#define MAP_ERR_SEMA               0x9901
#define MAP_ERR_TO                 0x9902
#define MAP_ERR_TOPO               0x9999


   /*
      EVENTS BETWEEN MAP_NPT and MAP_BMI
    */

	  /*
	     (this was the old map_ums.h)
	   */
#define NO_EVENT             0

#define START_INAUGURATION   1	/* from BMI to NPT */
#define STOP                 2
#define START_INCLUDE        3

#define MAP_NEW_INAUGURATION 0x88	/* from NPT to BMI */


   /*
      OPERATION PHASES
    */
#define IDLE                    0
#define SETUP_MODE              1
#define UPDATE_MODE             2
#define INCLUDE_MODE            3


   /*
      FUNCTION STEPS FOR SETUP MODE
    */
#define SEND_NADI               1	/* MASTER */
#define WAIT_BROADCAST          5	/* MASTER */
#define RECV_VEH_DES_REQUEST    2	/* SLAVE */
#define RECV_NADI               3	/* SLAVE */
#define START_SETUP             0	/* MASTER, SLAVE */
#define WAIT                    4	/* MASTER, SLAVE */
#define END_INAUGURATION        6	/* MASTER, SLAVE */
#define ERR_INAUGURATION        0x99	/* MASTER, SLAVE */


   /*
      FUNCTION STEPS FOR NODE RECOVERY
    */
#define START_RECOVERY          6	/* MASTER */
#define GET_GROUP_LIST          7	/* MASTER */
#define I_RECV_NADI             8	/* SLAVE */
#define I_RECV_GROUP_LIST       9	/* SLAVE */


   /*
      TGM/ORDER CODES FOR SETUP MODE
    */
#define ATTR_LIST_REQUEST   0xF001	/* request vehicle descriptor */
#define ATTR_DIR_REQUEST    0xF002	/* send NADI, request acknowledge */
#define INAUG_READY         0x0005	/* Broadcast inauguration ready */
#define ATTR_LIST_RESPONSE  0xFA01	/* send vehicle descriptor */
#define ATTR_DIR_RESPONSE   0xFA02	/* send acknowledge NADI */
#define INAUG_READY_RESP    0x0A05	/* send acknowledge inaug. ready */


   /*
      TGM/ORDER CODES FOR UPDATE MODE ON WTB
    */
#define DYN_ATTR_REQUEST        0xF003
#define IDLE_STATE_REQUEST      0xF004
#define GROUP_LIST_REQUEST      0xF005
#define SWITCHOVER_REQUEST      0xF006
#define SWITCHBACK_REQUEST      0xF007

#define DYN_ATTR_RESPONSE       0xFA03
#define IDLE_STATE_RESPONSE     0xFA04
#define GROUP_LIST_RESPONSE     0xFA05
#define SWITCHOVER_RESPONSE     0xFA06
#define SWITCHBACK_RESPONSE     0xFA07


   /*
      TGM/ORDER CODES FOR NODE RECOVERY ON WTB
    */
#define I_NADI_REQUEST          0x0003
#define I_GROUP_LIST_REQUEST    0x0004

#define I_NADI_RESPONSE         0x0A03
#define I_GROUP_LIST_RESPONSE   0x0A04

#define MULTICAST               0x0222	/* only order */


   /*
      TGM/ORDER CODES FOR UPDATE MODE ON MVB
    */
#define MVB_NADI_INI_REQU       0x0001
#define MVB_GROUP_LIST_INI_REQU 0x0002

#define MVB_NADI_INI_RESP       0x0A01

#define CANCEL_REQUEST          0x0111	/* only order */


   /*
      DEFAULT ORDER CODES FOR THE UI
    */
#define INIT_ORDER              0xF000	/* only in initial phase */
#define NEXT_ORDER              0xFFFF	/* only in initial phase */


   /*
      STATE CODES FOR THE UI SERVICE STEP
    */
#define MAP_UPDATE_NOT_READY 0	/* ui is not ready to serve any function call */
#define MAP_UPDATE_READY     1	/* ui is ready to serve the function call */
#define MAP_UPDATE_AT_WORK   2	/* user function can't be served, because an */


   /*
      TRACE RECORDER
    */
#define R_TCN   1
#define R_PHASE 2
#define R_ORD   3
#define R_ERR   4

#define MAX_REC15 15
typedef struct {
	unsigned char val[MAX_REC15];
	unsigned char ix;
} s_rec15;

#define MAX_REC31 31
typedef struct {
	unsigned char val[MAX_REC31];
	unsigned char ix;
} s_rec31;

#define MAX_REC7 7
typedef struct {
	unsigned short val[MAX_REC7];
	unsigned short ix;
} s_rec7;



#ifdef ATR_WTB					/* <ATR:02> */
/*
   STRUCTURE COMMON INFO {map_npt.c|map_bmi.c}
 */
typedef struct {

	unsigned char bmi_event;	/* Event from BMI to NPT */
	unsigned char bmi_type;		/* L_STRONG, L_WEAK, l_SLAVE */

	Type_Topography *p_TT;		/* TCN topography */
	map_VehicleDescr *p_VD;		/* own veh.des., delivered from TCN level */
	Type_WTBStatus NODEstate;	/* Status from link layer */
	map_NodeConfigurationTable *NodeKonf;

} s_CommInfo;

#endif


/*
   STRUCTURES E-TGM
 */

   /*
      DEFAULT TGM and ID
    */
	  /* for standard responses */
typedef ATR_PACKED struct {
	unsigned char key;
	unsigned char owner;
	unsigned short code;
	unsigned char format;
	unsigned char status;
} s_tgm_def;

typedef ATR_PACKED struct {
	unsigned char key;
	unsigned char owner;
	unsigned short code;
} s_tgm_id;


   /*
      NADI and VEHICLE DESCRIPTOR
    */
typedef ATR_PACKED struct {
	s_tgm_id id;
	unsigned char format;
	map_TrainTopography NADI;
} s_tgm_nadi;

typedef ATR_PACKED struct {
	s_tgm_id id;
	unsigned char format;
	map_VehicleDescr vehicle;
} s_tgm_vehdes;

	  /* to collect the vehicle descriptors it is prerequisite to prepare
	     receive buffers for each node to enable parallel receives.
	   */
typedef struct {
	s_tgm_vehdes member[MAP_MAX_NO_VEH];
} s_tgm_nvehdes;


   /*
      FOR UPDATE MODE
    */
typedef union {
	map_VehicleDescr vehicle;	/* update dyn. attr. */
	map_VehicleGroupList group;	/* update group list */
} u_data;

typedef union {
	map_VehicleDescr vehicle;	/* update dyn. attr. */
	map_VehicleGroupList group;	/* update group list */
	map_TrainTopography NADI;
} u_data1;

	  /* for WTB */
typedef ATR_PACKED struct {
	s_tgm_id id;
	unsigned char format;
	u_data content;
} s_tgm_update;

	  /* for MVB */
typedef ATR_PACKED struct {
	s_tgm_id id;
	unsigned char format;
	u_data1 content;
} s_tgm_update_mvb;



/*
   STRUCTURE VERGANGENHEITSWERTE
 */
typedef struct {

	/*
	   DATA FOR SETUP
	 */
	unsigned short f1st_run_s;	/* flag for first run setup */

	unsigned short code;		/* err Describes error location */
	unsigned short info;		/* err additional error information */

	unsigned char tstart;		/* counter start events from TCN */
	unsigned char tstop;		/* counter stop events from TCN */

	unsigned char phase;		/* work phase SETUP, UPDATE, STOP */
	unsigned char function_step;	/* steps during train inauguration */

	unsigned char numN;			/* number of received responses WTB */
	unsigned char numE;			/* number of error responses WTB */

	unsigned short send_delay;	/* delay time counter to calm down the system */

	char ext_c_wtb[MAP_MAX_NO_VEH];		/* for call_confirm request (22) */
	char ext_c_mvb[MAP_MAX_NO_MVB];		/* for call_confirm request (8) */

	unsigned short f1st_run_u_wtb;	/* flag for first run setup on WTB */
	unsigned short f1st_run_u_mvb;	/* flag for first run setup on MVB */

	unsigned short upd_status;	/* MAP_AT_WORK or MAP_UPDATE_READY */

	/* INPUT from user interface */
	unsigned short order_ui;	/* order */
	u_data upd_ui;				/* data for ui order */

	unsigned short order;		/* CURRENT work through order */
	unsigned short old_order;	/* last order */
	u_data upd;					/* data for act. order */

	/*
	   INDICATION FUNCTION BY map_idInstall FOR RECEIVERS SITE
	 */
	map_indication f_indication;	/* indication func. for any network events */

	void *r_indication;			/* ext. ref for the indication function */


	/*
	   CONFIRM FUNCTION BY map_Set_xxx FOR SENDERS SITE
	 */
	union {						/* on  c a l l e r s  site */
		map_setidle_conf confirm;	/* union for all. */
		map_setidle_conf setidle;
		map_switchover_conf switchover;
		map_switchback_conf switchback;
		map_setgrplist_conf setgrplist;
		map_setvhdescr_conf setvhdescr;
		MAP_INVOKE_CONF multicast;
	} f;						/* confirm func. for user interfaces */

	void *r_confirm;			/* ext. ref for the confirm functions */

	unsigned short ext_u_wtb[14];	/* for recv_confirm */
	unsigned short ext_u_mvb[2];	/* for recv_confirm */
	unsigned char *mc_tgm;		/* buffer addr. for multicast tgm */
	unsigned short mc_tgm_length;	/* buffer length for multicast tgm */
	AM_NET_ADDR mc_replier;		/* *.node = groupnr. */
	unsigned char mc_caller;
	unsigned char actual_nadi;	/* toggle between 0/1 */
	unsigned char actual_group_list;	/* toggle between 0/1 */

} s_vw;


/*
   PROTOTYPES
 */

   /*
      P r o t o t y p e s     for common functions within map_npt.c and map_npv.c
    */
/* <ATR:01> */
//   void  recorder(unsigned char);
//   void  err_rec(void);
void build_vrno (void);
