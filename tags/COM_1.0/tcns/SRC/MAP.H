/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> map_user_ls_Report defined                                                          */
/* <ATR:02> WTB dependencies only if it is present                                              */
/* <ATR:03> Modified the vehicle descriptor for FS compatibility                                */
/* <ATR:04> This file will be included only once                                                */
/*==============================================================================================*/

/*
   000000120
   MAP Includefile
   Wolfgang Landwehr
   11. Oktober 1995, 15:58
   3
 */

/*
   GENERAL TOP OF FILE
 */

   /*
      Declaration of all constants, structures for MAP modules and applications.
      This include file is the map data interface to all applications.
    */

   /*
      18.02.94 W. Landwehr
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
	     0.xx 14.04.94  W. Landwehr
	     Starting versions, starting point is:
	     UIC-MAPPING SERVER SPECIFICATION 1.0, Jan. 94   
	     1.00 20.04.94  W. Landwehr
	     Prototype map_get_topography
	     1.01 27.04.94  W. Landwehr
	     multicast prototype supplemented.
	     1.02 19.05.94  W. Landwehr
	     MAP_FUNCTION_NR now in map.h
	     1.03 20.05.94  W. Landwehr      {---MAP 1.12--- on train}
	     Parameter for map_install changed.
	     1.04 01.09.94  W. Landwehr
	     Supplements after train implementation: MULTICAST address types.
	     1.05 21.09.94  W. Landwehr
	     Workaround several datatypes.
	     1.06 21.11.94  W. Landwehr      {---MAP 1.15---}
	     Supplements for MVB use.
	     1.07 15.12.94  W. Landwehr
	     type MAP_INVOKE_CONF corrected
	     MAP_MAX_NO_VEH = 22, MAP_MAX_NO_NGL = 22
	     1.10 08.02.95  W. Landwehr      {---MAP 1.20---}
	     MAP_MAX_NODES fore new structure in ns_db.c adapted
	     1.11 21.02.95  P. Koerner       {---MAP 2.00---}
	     Adaptions for WTB 1.9
	     1.12 06.04.95  P.Koerner        {---MAP 2.04---}
	     node count adapted.
	   */


   /*
      2.00 11.10.95  W. Landwehr
      Adoption for MyFriend, Workaround
    */


/*
   DEFINES
 */

#define MAP_FUNCTION_NR  210   /* = 0xD2, for use with am_@@@ calls */

   /*
      INCLUDEFILE DEPENDENCIES {AM_SYS.H and BMI.H}
    */

#ifndef MAP_H
#define MAP_H
/* <ATR:04> */
// #endif

#ifndef AM_SYS_H
#error INCLUDE AM_SYS.H FIRST!
#endif

#ifdef ATR_WTB					/* <ATR:02> */
#ifndef LR_CONFIGURED
#error INCLUDE BMI.H FIRST!
#endif
#endif


#ifdef ATR_WTB					/* <ATR:02> */

   /*
      NOT IN USE, BUT in SPEC.
    */

	  /*
	     #*------------------------------------------------------------------------
	     System states of the mapping server (MPSERV10.DOC, 5.6, Pg.32)
	     *#
	     #define NODE_RESET          0     #* Power on, node awakes from idle state *#
	     #define TCN_CONFIGURATION   1     #* map_configure is running *#
	     #define NADI_SET_UP         2     #* map_setup is running: train inauguration *#
	     #define REGULAR_OPERATION   3     #* map_update is running *#
	   */

	  /*
	     #*------------------------------------------------------------------------
	     TCN node types  (MPSERV10.DOC, 4.2.1, Pg.27, adapted for WTB 1.9)
	     *#
	     #define MAP_UNDEFINED       L_UNDEFINED     #*  0 *#
	     #define MAP_SLAVE           L_SLAVE         #*  1 *#
	     #define MAP_STRONG          L_STRONG        #*  2 *#
	     #define MAP_WEAK            L_WEAK          #*  3 *#
	   */

#endif

   /*
      N A D I  STATES AND DIRECTIONS      {MPSERV10.DOC, 4.2.2, Pg.28}
    */
#define MAP_NADI_VALID      0x01
#define MAP_NADI_INVALID    0x99

#define MAP_ABSOLUTE        0x11
#define MAP_RELATIVE        0x22

#define MAP_DIREC_1         0x01
#define MAP_DIREC_2         0x02
#define MAP_DIREC_UNKNOWN   0x00


   /*
      SYSTEM LIMITS OF THE MAPPING SERVER {MPSERV10.DOC, 4.2.3, Pg.29}
    */
#define MAP_MAX_NODES     14	/* number of WTB nodes */
#define MAP_MAX_NO_VEH    22	/* max.no of vehicles (theor. max. = 32) */

#define MAP_MAX_NO_GRP    10	/* max.no of groups */
#define MAP_MAX_NO_VGL    50	/* max.no of tgm elem. for set_group */
#define MAP_MAX_NO_NGL    22	/* max.no of nodes for one group */
									/* maximum is MAP_MAX_NO_VEH !!! */

#define MAP_MAX_NODES_MVB 2		/* number of max MVB nodes */


   /*
      MAP RESULT CODES                    {MPSERV10.DOC, 5,     Pg.30,36}
    */

	  /*
	     DUE TO RESULT CODE MIX WITH RTP SERVICES USE RANGE ABOVE NUMBER 40 ! ! !
	   */
#define MAP_OK     0			/* ok, identical with AM_OK */

#define MAP_RNAL   40			/* service req. not accepted (locally) */
#define MAP_RNA    41			/* service req. not accepted by map */
#define MAP_NP     42			/* service not processable */

#define MAP_NOT_OK 43			/* summary error by MULTICAST or UPDATE */
							 /* This error returns by the confirm fct. */


   /*
      MAP EVENT TYPES FOR INDICATION FCT. {MPSERV10.DOC, 6.4.2, Pg.36}
    */
	  /* indication function, installed by map_IdInstall, module MAP_UI.C */
#define MAP_NADI_UPDATED    0x004
#define MAP_IDLE_REQUEST    0x005
#define MAP_SWITCHOVER      0x006
#define MAP_SWITCHBACK      0x007


   /*
      MAP EVENT TYPES FOR REPORT FCT.     {MPSERV10.DOC, 5.2,   Pg.31}
    */
	  /* report function, installed by map_install, modul MAP_BMI.C */
#define MAP_NODE_RESET           0x0001
#define MAP_NODE_STARTED         0x0002
#define MAP_TCN_CONFIGURED       0x0003
#define MAP_NEW_CONFIGURATION    0x0004
#define MAP_NADI_SET_UP          0x0005
#define MAP_NADI_UPDATE_RECEIVED 0x0006


   /*
      MAP MULTICAST ADDRESS TYPES         {MPSERV10.DOC, 7.1.1, pg.40}
      {UIC LL 556 appendix 1, page 14}
    */
	  /* fix address types                  */
#define MAP_MC_LEADING_VEHICLE 64	/* 0x40     bit 0.1 */
#define MAP_MC_TRACTION_LEADER 65	/* 0x41         0.3 */
#define MAP_MC_ALL_VEHICLES    66	/* 0x42             */
#define MAP_MC_ALL_CARRIAGES   67	/* 0x43 not bit(0.3 & 0.5) */
#define MAP_MC_LAST_VEHICLE    68	/* 0x44     bit 1.0 */

	  /* dynamic addresses by user defined group numbers */
#define MAP_MC_BASIC_GNR       120	/* 0x78 add 1..MAP_MAX_NO_GRP */



/*
   STRUCTURES
 */

   /*
      STRUC: map_VehicleDescr     {MPSERV10.DOC, 4.2.1,   Pg.27}
    */
	  /* one vehicle descriptor of NADI */
typedef struct {
	unsigned char NodeAddr;		/* TCN node adress */
	unsigned char RangeNo;		/* Vehicle range number */
	unsigned short VehicleNo;	/* Vehicle number for seat reservation */
	unsigned char Owner;		/* Owner of the vehicle */
	unsigned char VehicleId[4];	/* Identif. of the vehicle, BCD coded */
	unsigned char DynAttr[3];	/* Dynamic attributes, bit map */
	unsigned char PermAttr[8];	/* Permanent attributes, bit map */
	char NcV;					/* No. of controlled veh. by that node */
	unsigned char Functions[32];	/* List of funct. supported by the node */
	unsigned char TCNType;		/* TCN node type MASTER, SLAVE, ... */
	unsigned char VTy;			/* first 2 digits of the UIC vehicle id *//* <ATR:03> */
	unsigned char PAttrFS[5];	/* FS permanent attributes *//* <ATR:03> */
	unsigned char PMax;			/* max vehicle power *//* <ATR:03> */
	unsigned char Reserved[3];	/* was 10 *//* <ATR:03> */
} map_VehicleDescr;


   /*
      STRUC: map_NADIState        {MPSERV10.DOC, 4.2.2,   Pg.28}
    */
	  /* node state of the NADI */
typedef struct {
	unsigned short version;		/* value of NADI version counter */
	unsigned char validity;		/* NADI validity */
	unsigned char ownaddr;		/* TCN node adress of this node */
	unsigned char mdirect;		/* Direction of the TCN master node */
	unsigned char VehicleNo;	/* Vehicle Number [1...63] */
	unsigned char ScopeOfRangeNo;	/* Indication, range number rel./abs. */
	unsigned char reserved;
} map_NADIState;


   /*
      STRUC: map_TrainTopograph   {MPSERV10.DOC, 4.2.3,   Pg.29}
    */
	  /* the  o r i g i n a l  NADI */
typedef struct {
	map_NADIState state;
	map_VehicleDescr vehicle[MAP_MAX_NO_VEH];
} map_TrainTopography;


   /*
      FOR GROUP LIST              {MPSERV10.DOC, 4.2.4-5, Pg.29}
    */
	  /* Group membership of a node */
typedef struct {
	unsigned short length;		/* Number of list entries */
	unsigned char node[MAP_MAX_NO_NGL];		/* List of nodes to this group */
} map_NodeGroupList;

	  /* Group list with mapping of nodes to groups */
typedef struct {
	unsigned char node;			/* TCN node number */
	unsigned char group;		/* Group number */
} map_VGLentry;

	  /* Group list for UI service map_SetGroupList */
typedef struct {
	unsigned short length;		/* Number of entries */
	map_VGLentry member[MAP_MAX_NO_VGL];
} map_VehicleGroupList;


#ifdef ATR_WTB					/* <ATR:02> */
   /*
      FOR NODE SUPERVISOR
    */
typedef struct {
	unsigned short number;		/* number of entries in device list */
	AM_NET_ADDR *device;		/* device List */
} map_DeviceList;

typedef struct {
	unsigned short basic_period;
	unsigned short individual_period;
	Type_NodeDescriptor node_descriptor;
	void *ts_descriptor;		/* pointer to the traffic storc descriptor */
} map_Config;

	  /* used in NS_CONF.C, function ns_get_map_config(..) */
typedef struct {
	map_VehicleDescr vehicle_descriptor;
	map_DeviceList device_list;
	map_Config configuration;
} map_NodeConfigurationTable;

#endif


/*
   FUNCTION PROTOTYPES
 */

   /*
      TYPES OF PARAMETER FOR THE FUNCTIONS
    */
typedef unsigned short map_ReportType;
typedef unsigned short map_ErrorType;
typedef unsigned short map_result;


#ifdef ATR_WTB					/* <ATR:02> */
   /*
      TYPES OF REPORT FUNCTION FOR CONTROLL INTERFACE  {MPSERV10.DOC 5.2}
    */

	  /*
	     report and error function types,
	     installed by map_install(..) in module MAP_BMI.C
	   */
typedef void (*map_report) (map_ReportType report);

typedef void (*map_error) (map_ErrorType error);
#endif


   /*
      TYPES OF CONFIRM FUNCTION FOR USER INTERFACE     {MPSERV10.DOC 6.4}
    */
	  /* call back indication function for receivers site,
	     installed by map_idInstall(..), module MAP_UI.C
	   */
typedef void (*map_indication)
  (void *reference, unsigned char typ, map_result status);

	  /* confirm fct. for ui services */
typedef void (*map_setidle_conf) (void *reference, map_result status);
typedef void (*map_switchover_conf) (void *reference, map_result status);
typedef void (*map_switchback_conf) (void *reference, map_result status);
typedef void (*map_setgrplist_conf) (void *reference, map_result status);
typedef void (*map_setvhdescr_conf) (void *reference, map_result status);

	  /* multicast confirm fct. */
typedef void (*MAP_INVOKE_CONF)
  (unsigned char caller_fct, void *ext_ref,
   const AM_NET_ADDR * replier, AM_RESULT status);


#ifdef ATR_WTB					/* <ATR:02> */
   /*
      FUNCTIONS OF THE CONTROL INTERFACE MAP_BMI.C     {MPSERV10.DOC 5}
    */
map_result map_init (void);

map_result map_install (map_report new_proc_rep,
						map_report * old_proc_rep,
						map_error new_proc_err,
						map_error * old_proc_err);

map_result map_configure (map_NodeConfigurationTable *);

map_result map_get_topography (Type_Topography * p_topography,
							   unsigned short *own_address);

	  /* <ATR:01> */
extern void (*map_user_ls_Report) (l_report);
#endif

   /*
      FUNCTIONS OF THE USER INTERFACE MAP_UI.C         {MPSERV10.DOC 6}
    */
map_result map_idInit (void);

map_result map_idInstall (unsigned char caller,
						  map_indication indication,
						  void *reference);

map_result map_SetIdle (unsigned char caller,
						map_setidle_conf confirmation,
						void *reference);

map_result map_SwitchOver (unsigned char caller,
						   map_switchover_conf confirmation,
						   void *reference);

map_result map_SwitchBack (unsigned char caller,
						   map_switchback_conf confirmation,
						   void *reference);

map_result map_GetTrainTopography (unsigned char caller,
								   map_TrainTopography ** topo);

map_result map_GetOwnVhDescriptor (unsigned char caller,
								   map_VehicleDescr ** vh_descr);

map_result map_GetGroupList (unsigned char caller,
							 unsigned char group_number,
							 map_NodeGroupList ** node_list);

map_result map_SetGroupList (unsigned char caller,
							 map_VehicleGroupList * group_list,
							 map_setgrplist_conf confirmation,
							 void *reference);

map_result map_SetVehicleDescriptor (unsigned char caller,
									 map_VehicleDescr * descriptor,
									 map_setvhdescr_conf confirmation,
									 void *reference);

map_result map_CancelRequest (unsigned char caller);

map_result map_multicast_requ (unsigned char caller_fct,
							   AM_NET_ADDR * replier,
							   void *out_msg_addr,
							   unsigned int out_msg_size,
							   MAP_INVOKE_CONF confirm,
							   void *ext_ref);

/* <ATR:04> */
#endif
