/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*==============================================================================================*/

/*
   000000120
   MAP_BMI Includefile
   Wolfgang Landwehr
   11. Oktober 1995, 15:58
   3
 */

/*
   GENERAL TOP OF FILE
 */

   /*
      Internal declaration of all constants, structures for MAP_BMI and
      MAP_NPT modules (the last only needs map_ctrl_field).
    */

   /*
      15.04.94 F. Herrmann
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
	     0.xx 08.04.94  F. Herrmann
	     Starting version, starting point is:
	     UIC-MAPPING SERVER SPECIFICATION 1.0, Jan. 94
	     Modified after first implementation with am environment.
	     1.00 28.04.94  F. Herrmann
	     Modified for error recorder, 1st integration.
	     1.01 19.05.94  F. Herrmann
	     M_TCN_REGULAR_OPERATION
	     1.02 20.05.94  F. Herrmann      {---MAP 1.12--- on train}
	     EVENT_RECORDER, MAP_TIMEOUT
	     1.03 22.06.94  F. Herrmann
	     counter for master conflict
	     1.04 16.08.94  F. Herrmann
	     supplemented for weak master
	     1.05 24.11.94  F. Herrmann
	     include
	     1.06 19.01.95  W. Landwehr
	     new naming added
	   */


   /*
      2.00 21.02.95  P. Koerner
      Upgraded to WTB 1.9
      2.01 27.03.95  W. Landwehr
      Renamed from map_tcn to map_bmi. Module name in macro adapted.
      2.02 11.10.95  W. Landwehr
      Adoption for MyFriend, workaround
    */


/*
   STRUCTURE MAP CONTROL FIELD {map_npt.c|map_bmi.c}
 */

   /*
      This structure is interface between map_bmi and map_npt.
      map_npt calls the report function after NADI_SETUP and NADI UPDATE.
    */
typedef struct {

	map_NodeConfigurationTable map_config;	/* structure in map.h */
	map_report map_reportF;		/* report fct. for network event */
	map_error map_errorF;		/* error fct. */

} map_ctrl_field;


/*
   STRUCTURE BUS STATE
 */
typedef struct {

	unsigned short cnt_lengthening;
	unsigned short cnt_disruption;
	unsigned short cnt_master_conflict;
	unsigned short reserved[3];

} map_bus_status;


/*
   STRUCTURE VERGANGENHEITSWERTE
 */
typedef struct {

	unsigned char state;		/* momentane Position in der Ablaufkette    */
	unsigned char init;			/* Anlegen des vollst. Konfigurationsfeldes */

	unsigned char start_configurate;
	unsigned char start_strong;
	unsigned char start_naming_master;
	unsigned char start_slaveWeakMode;
	unsigned char start_nadi;
	unsigned char stop_nadi;
	unsigned char start_include;

	unsigned char new_naming;

	unsigned char lr_configured;
	unsigned char lr_strong;
	unsigned char lr_slave;
	unsigned char lr_promoted;
	unsigned char lr_naming_successful;
	unsigned char lr_named;
	unsigned char lr_weak;
	unsigned char lr_removed;
	unsigned char lr_demoted;
	unsigned char lr_disconnection;
	unsigned char lr_inhibited;
	unsigned char lr_included;
	unsigned char lr_lengthening;
	unsigned char lr_disruption;
	unsigned char lr_master_conflict;
	unsigned char lr_naming_failed;
	unsigned char lr_new_topography;
	unsigned char lr_node_status;
	unsigned char lr_poll_list_ovf;
	unsigned char lr_allowed;

	unsigned char reserved[18];

} vergangenheitswerte_map_control;


/*
   PROTOTYPES
 */
void map_eh_timer (unsigned short *);
void map_eh_timer_reset (unsigned short, unsigned short *);
void map_eh_timer_stop (unsigned short *);
void map_eh_refresh (unsigned short *, unsigned int);

void map_CtrlErrorReport (l_report);
void map_CtrlReport (l_report);
