/***************************************************************************/
/*2004.ven.23.lug      v.2.00         \SW\LOSA\GTW\SRC\gtw_appl.cc         */
/*                                                                         */
/* Module: gtw_appl                                                        */
/* Description:modulo  applicativo del GTW                                 */
/*                                                     IMPLEMENTATION FILE  */
/***************************************************************************/
/***************************************************************************/


/*=============================================================================================*/
/* Include files */

#include <stdio.h>

extern "C" {
#include "lc_sys.h"
#include "lp_sys.h"
#include "pi_sys.h"
#include "am_sys.h"
#include "conf_tcn.h"
#include "pdm_main.h"
#include "pdm_lcfg.h"
#include "atr_pdm.h"
#include "map.h"
#include "sio.h"
#include "gtw_appl.h"
#include "gtw_util.h"
#include "gtw_ncdb.h"

#include "c_utils.h"
#include "ushell.h"
#include "atr_log.h"
#include "atr_hw.h"
#include "wtbcpm.h"
#include "cpmdrv.h"
#include "mvbc_red.h"

#include "strt_tcn.h"
#include "tcn_mon.h"
#include "tstflash.h"
#include "tstmvb.h"
#include "tsteprom.h"
#include "tstio.h"

}


/*=============================================================================================*/
/* Error constants */

typedef struct {
	char			event_type;
	char			event_action;
	unsigned short	code;
	const char		*text;
} LogEvent;

#define DebugEvent(name, code, text) \
	const LogEvent name##_event = { NSE_EVENT_TYPE_SPECIAL, NSE_ACTION_CONTINUE, code, text }
#define WarningEvent(name, code, text) \
	const LogEvent name##_event = { NSE_EVENT_TYPE_WARNINGS, NSE_ACTION_CONTINUE, code, text }
#define ErrorEvent(name, code, text) \
	const LogEvent name##_event = { NSE_EVENT_TYPE_ERROR, NSE_ACTION_CONTINUE, code, text }
#define FailureEvent(name, code, text) \
	const LogEvent name##_event = { NSE_EVENT_TYPE_ERROR, NSE_ACTION_HALT, code, text }

#define log_event(name) \
	atr_record(__FILE__, __LINE__, name##_event.event_type, name##_event.event_action, \
			   sizeof(name##_event.code), (unsigned char *)&name##_event.code, pi_ticks_elapsed(), \
			   (char *)name##_event.text)

//ErrorEvent  (wrong_ncdb,      0x0001, "Wrong NCDB configuration");
//ErrorEvent  (inhibit_fail,    0x0002, "INHIBIT failure");
//ErrorEvent  (mvb_fail,        0x0003, "MVB connection failure");
//WarningEvent(mvb_ok,          0x0004, "MVB connection OK");
//ErrorEvent  (mvbd_shd_fail,   0x0005, "MVBD SHD error");
//WarningEvent(mvbd_shd_ok,     0x0006, "MVBD SHD OK");
//ErrorEvent  (comm_fail,       0x0007, "COMM signal failure");
//ErrorEvent  (sleout_fail,     0x0008, "SLEOUT signal failure");
//ErrorEvent  (mvbon_fail,      0x0009, "MVBON signal failure");
//ErrorEvent  (key_fail,        0x000A, "KEY signal failure");
//ErrorEvent  (carbaton_fail,   0x000B, "CARBATON signal failure");
//ErrorEvent  (lucion_fail,     0x000C, "LUCION signal failure");
//ErrorEvent  (recon_fail,      0x000D, "RECON signal failure");
//ErrorEvent  (rstretein_fail,  0x000E, "RSTRETEIN signal failure");
//ErrorEvent  (velosit_fail,    0x000F, "VELOSIT signal failure");
//ErrorEvent  (stato_mt_fail,   0x0010, "STATO_MT signal failure");
//ErrorEvent  (risin1_fail,     0x0011, "RISIN1 signal failure");
//ErrorEvent  (risin2_fail,     0x0012, "RISIN2 signal failure");
//ErrorEvent  (risin3_fail,     0x0013, "RISIN3 signal failure");
//ErrorEvent  (risin4_fail,     0x0014, "RISIN4 signal failure");
//ErrorEvent  (risin5_fail,     0x0015, "RISIN5 signal failure");
//ErrorEvent  (risin6_fail,     0x0016, "RISIN6 signal failure");
//ErrorEvent  (risout1_fail,    0x0017, "RISOUT1 signal failure");
//ErrorEvent  (risout2_fail,    0x0018, "RISOUT2 signal failure");
//ErrorEvent  (risout3_fail,    0x0019, "RISOUT3 signal failure");
//ErrorEvent  (risout4_fail,    0x001A, "RISOUT4 signal failure");
//WarningEvent(be_gtwm,         0x001B, "Switching to GTWM");
//WarningEvent(be_gtws,         0x001C, "Switching to GTWS");
//WarningEvent(mvb_inhibit,     0x001D, "Got inhibit signal from MVB");
//ErrorEvent  (conf_read_fail,  0x001E, "Unable to read the configuration from the EEPROM");
//ErrorEvent  (conf_write_fail, 0x001F, "Unable to write the configuration to the EEPROM");
//ErrorEvent  (wtb_fail,        0x0020, "WTB hardware failure");
//WarningEvent(comm_change,     0x0021, "COMM signal change during operation");
ErrorEvent  (mvb_a_fail,      0x0022, "MVB line A failure");
WarningEvent(mvb_a_ok,        0x0023, "MVB line A OK");
ErrorEvent  (mvb_b_fail,      0x0024, "MVB line B failure");
WarningEvent(mvb_b_ok,        0x0025, "MVB line B OK");
//ErrorEvent  (wtb_hangup,      0x0026, "WTB Link Layer hangup");


/*=============================================================================================*/
/* Globals */

/*---------------------------------------------------------------------------------------------*/
/* Used when GTWM and GTWS */
char mvbon_flag;   /**** compatibilità libreria TCN */
unsigned long			start_ticks;				/* ticks value at the application start */
unsigned long			ticks;						/* current time in ticks                */

//static GtwApplConf		gtw_conf;					/* gateway configuration structure */
//static char				is_A_flag;							/* TRUE if we are the "A" unit     */
//static unsigned char	standby_status = GTW_STANDBY_READY;	/* status of the standby unit      */
static NcEleID			station_id;							/* station ID value                */
static char				mvb_failure_flag;					/* the MVB is not working properly */

static char				mvb_shd_failure_flag;		/* the MVB signals a SHD               */
static char				mvb_a_failure_flag;			/* the MVB signals a failure on line A */
static char				mvb_b_failure_flag;			/* the MVB signals a failure on line B */
static char				wtb_failure_flag;			/* the WTB is not working properly     */
//static char				hw_failure_flag;			/* some hardware not working properly  */
//static char				failure_flag;				/* this board is in failure (latched)  */
//
static char				mvb_activity_flag;			/* we are communicating over the MVB    */
static char				wtb_inaugurated_flag;		/* the WTB is inaugurated               */
static char				wtb_master_flag;			/* this board is the WTB master         */
//static char				wtb_intermediate_flag;		/* this GTW is intermediate         */
//static char				comm_flag;					/* value of the COMM signal (0 -> WTB*) */
//static char				mvb_red_working_flag;		/* the MVB redundancy module is active  */


//static char				f_DA1;		            /* Line A1 disturbed flag  */
//static char				f_DA2;		            /* Line A2 disturbed flag  */
//static char				f_DB1;		            /* Line B1 disturbed flag  */
//static char				f_DB2;		            /* Line B2 disturbed flag  */

//static char				gtw_dso_valid;				/* DSO validity flag             */
//static char				gtw_dsi_valid;				/* DSI validity flag             */
//static char				gtw_dsm_valid;				/* DSM validity flag             */
//static char				gtw_dss_valid;				/* DSS validity flag             */
//
// static const GtwDso 	default_gtw_dso = {};		/* DSO default values            */
//static const GtwDsi 	default_gtw_dsi = {0xFF};	/* DSI default values            */
//static const GtwDsm 	default_gtw_dsm = {};		/* DSM default values            */
//static const GtwDss 	default_gtw_dss = {};		/* DSS default values            */

//static GtwDsi			old_gtw_dsi;				/* DSI previous values           */
//static GtwIO			hw_io;						/* signals at the hardware interface  */
//static GtwIO			sw_io;						/* signals considered by the software */

//static char				no_mvb_refresh_flag;		/* if TRUE the MVB refresh cnt are ignored */
//static unsigned int DO_forced = 0;
//static Type_Topography	wtb_topo;			        /* current WTB topography        */
//static unsigned char    old_gtw_dsi_vid;            /* Vehicle ID previous values           */
//static unsigned short   vid_bcd;
//static unsigned short   vid_bin;
/*---------------------------------------------------------------------------------------------*/
/* Used only when GTWM */

//static char					master_conflict_flag;	/* TRUE if there is a master conflict */
//static map_TrainTopography	map_topo;				/* current Mapping Server topography  */
//static map_VehicleDescr		*veh_desc_p;		/* pointer to our vehicle descriptor      */
//static unsigned char ind_mt = 0;
//static unsigned char tab_rem[MAX_MOT];
//static unsigned char locv_mt[MAX_MOT];
//static unsigned char r3loc_logico[MAX_MOT];
//static unsigned char r3loc_chk[MAX_MOT];
//  static unsigned char cont, new_key =0;
//static const unsigned char mask1 [] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
//static unsigned char nadi_reverse = FALSE;  // rel 1.21

//static char  err_maubusy; /*if true it means the maubusy signal is active SVN_454_a*/
//static char  cnt_maubusy; /*the counter was defined to obtain a delay before the reset to allow the message print. SVN_454_a*/
//static int  cnt_delay=0; /*the counter was defined to obtain a delay before the reset to allow the message print. SVN_454_a*/



unsigned char ucToogle = 0;
unsigned int  uiStrtUp = 0;

t_teleout teleout;
//unsigned char num_loc;
//unsigned char gugu_dbg;
//
//ONTD_NEW(signal_A1D, DELAY_500MS_TICKS);
//ONTD_NEW(signal_B1D, DELAY_500MS_TICKS);
//ONTD_NEW(signal_A2D, DELAY_500MS_TICKS);
//ONTD_NEW(signal_B2D, DELAY_500MS_TICKS);
//ONTD_NEW(signal_HFA, DELAY_500MS_TICKS);
//ONTD_NEW(signal_HFB, DELAY_500MS_TICKS);
//
//
//OFTD_NEW(delay_A1D, DELAY_5S_TICKS);
//OFTD_NEW(delay_B1D, DELAY_5S_TICKS);
//OFTD_NEW(delay_A2D, DELAY_5S_TICKS);
//OFTD_NEW(delay_B2D, DELAY_5S_TICKS);
//OFTD_NEW(delay_HFA, DELAY_5S_TICKS);
//OFTD_NEW(delay_HFB, DELAY_5S_TICKS);

/*---------------------------------------------------------------------------------------------*/
/* Used only when GTWS */


/*==============================================================================================*/
/* Macros */

// #undef Check
// #define Check(x, label) if ((err = x) != 0) {errl = __LINE__; errs = #x; goto label;} else

// #undef Error
// #define Error()	\
// 	if (err) printf("*** ERROR: file='%s', line=%u, command='%s', err=%d\n", __FILE__, errl, errs, err);
/*==============================================================================================*/

//static unsigned char elab_topography()
//{
//    unsigned char num_nodi, index, primo_nodo, _logico;
//    map_VehicleDescr * pVD;
//
//    num_nodi = map_topo.state.VehicleNo;
//
//    // set del tc corrente   rel 1.2
//    teleout.db.tc_curr = wtb_topo.topo_counter;
//
//    //inizz variabili
//    ind_mt     = 0;
//    teleout.db.num_loc = 0;
//    _logico    = 0;
//    primo_nodo = 0;
//    for (index = 0; index < MAX_MOT; index++)
//    {
//        locv_mt[index] = 0;
//        tab_rem[index] = 0;
//    }
//    teleout.db.v_mt     = 0;
//    nadi_reverse = FALSE;//01/07/09 12.03.37
//    //pVD si posiziona al primo descrittore di veicolo
//    pVD = &map_topo.vehicle[0];
//
//    if (pVD->RangeNo !=1) nadi_reverse = TRUE;
//    if(gugu_dbg) printf("1 : vehicle[0] %d\n",pVD->RangeNo);
//    //pVD si posiziona sul descrittore con UIC 1
//
//    if (nadi_reverse) pVD+=num_nodi-1;
//    if(gugu_dbg) printf("1 : nadi_reverse  %d  %d\n",nadi_reverse,pVD->RangeNo);
//
//    // esamina tutti i VD
//    for (index=0; index < num_nodi; index++)
//    {
//        if (pVD->NcV >= -1)  // corrispondenza 1:1 nodo/veicolo
//        {
//            primo_nodo = 0;
//            _logico++;
//
//            if ((c_get_bit(&pVD -> PermAttr,0)) || (c_get_bit(&pVD -> DynAttr,3))  || (c_get_bit(&pVD -> DynAttr,1)))
//            {
//                teleout.db.lst_mot[teleout.db.num_loc] = _logico;
//                teleout.db.num_loc++;
//            }
//
//            teleout.logico[pVD -> NodeAddr] =_logico; // tcn -> uic
//            if(gugu_dbg) printf("1 : tcn %d -> log %d\n",pVD -> NodeAddr,_logico);
//
//            teleout.ind_nodo[_logico] = pVD->NodeAddr;// uic -> tcn
//
//            if ((c_get_bit(&pVD -> DynAttr,3))  || (c_get_bit(&pVD -> DynAttr,1)))
//                locv_mt[++ind_mt] = _logico;
//
//            teleout.VD_ptr[_logico] = pVD;
//        }
//        else
//        {
//            if (primo_nodo==0){
//                primo_nodo = 1;
//                _logico++;
//
//                if ((c_get_bit(&pVD -> PermAttr,0)) || (c_get_bit(&pVD -> DynAttr,3))  || (c_get_bit(&pVD -> DynAttr,1)))
//                {
//                    teleout.db.lst_mot[teleout.db.num_loc] = _logico;
//                    teleout.db.num_loc++;
//                }
//                teleout.logico[pVD -> NodeAddr] =_logico;
//                if(gugu_dbg) printf("2 : tcn %d -> log %d\n",pVD -> NodeAddr,_logico);
//
//                teleout.ind_nodo[_logico] = pVD->NodeAddr;
//                if ((c_get_bit(&pVD -> DynAttr,3))  || (c_get_bit(&pVD -> DynAttr,1)) == 0x01)
//                    locv_mt[++ind_mt] = _logico;
//                teleout.VD_ptr[_logico] = pVD;
//            }
//            else
//            {
//                teleout.logico[pVD -> NodeAddr] = _logico;
//                if(gugu_dbg) printf("3 : tcn %d -> log %d\n",pVD -> NodeAddr,_logico);
//
//                primo_nodo++;
//                if (primo_nodo+pVD->NcV==0) primo_nodo = 0; // criterio di arresto
//            }
//        }
//        // rel 1.21
//        if (nadi_reverse) pVD--; else pVD++;
//    }
//
//
//
//    teleout.db.v_app = teleout.logico[teleout.db.topo.state.ownaddr];
//
//    teleout.db.v_mt = teleout.db.lst_mot[1];
//
//    return 1;
//}
/*=============================================================================================*/

//static unsigned char get_dir (const TYPE_LP_DS_SET *ds_ptr , source_dir *sd_inp,
//   unsigned char *log_vei_ptr, unsigned char *chk_vei_ptr, unsigned char vei_max)
//{
//   unsigned char vei_num;
//   unsigned char index;
//   unsigned char address;
//
//   unsigned char sd_entry;
//   unsigned char chk_entry;
//
//   unsigned char *sd_ptr;
//   unsigned char *chk_ptr;
//
//   sd_ptr  = sd_inp -> sd;
//   chk_ptr = sd_inp -> chk;
//
//   vei_num = 0; /* Azzera il numero di veicoli trovati                      */
//   /* Presetta la lista di veicoli                                          */
//   for (index=0; index < vei_max; index++) *(log_vei_ptr + index) = 0x0;
//   /* Presetta la lista dei check                                           */
//   for (index=0; index < vei_max; index++) *(chk_vei_ptr + index) = 0x0;
//
//   lp_get_data_set (ds_ptr);
//   /* Ricava la lista dei veicoli trovati                                   */
//   address = 0;
//   while ((address < 64) && (vei_num < vei_max))
//   {
//      sd_entry  = *(sd_ptr  + (address / 8));
//      chk_entry = *(chk_ptr + (address / 8));
//      // intervenire su questo codice .... del check
//
//      if ((sd_entry & mask1 [address % 8]) != 0)
//      {
//         *(log_vei_ptr + vei_num) = teleout.logico [address];
//         *(chk_vei_ptr + vei_num) = chk_entry & mask1[address % 8];
//
//
//		// Indirizzo UIC (1,2,3..)
//		tab_rem[vei_num]= teleout.logico[address];
//        if (gugu_dbg) printf("%d ,%d -> %d\n",vei_num, address, tab_rem[vei_num]);
//        vei_num = vei_num + 1;
//      }
//      address = address + 1;
//   }
//   return vei_num;
//}

/*=============================================================================================*/
/* Redundancy handling */

/*---------------------------------------------------------------------------------------------*/
/* Inhibit the other board */

//static void inhibit(void)
//{
//	unsigned long tick;		/* timeout tick time ;*/
//
///*	 shut down the other unit */
//	set_inb(1);
//
///*	 wait for the shutdown of the other unit   */
//	tick = pi_ticks_elapsed() + ms2tick(50);
//	while ( (get_fault() || get_unim() || get_unis()) &&
//		   !(get_fault() && get_unim() && get_unis()) &&
//		   tick > pi_ticks_elapsed());
//
///*	 signal error if the other unit is not off */
//	if ( (get_fault() || get_unim() || get_unis()) &&
//	    !(get_fault() && get_unim() && get_unis())) log_event(inhibit_fail);
//}


/*---------------------------------------------------------------------------------------------*/
/* Redundancy check */

//static unsigned long red_check(void)
//{
//	static char         red_started;			/* if TRUE the redundant unit started */
//	static char         old_mvb_failure;		/* previous MVB failure status        */
//	char		mvb_failure;			/* MVB failure status                 */
//	static int ticks_fault;
//	static char fault_flag;
//	char		fault = get_fault();	/* FAULT signal status                */
//	char		unim = get_unim();		/* UNIM signal status                 */
//	char		unis = get_unis();		/* UNIS signal status                 */
//
//	/* we are always the GTWM if the GTWS unit is not configured */
//	if (standby_status == GTW_STANDBY_ABSENT) return GTWM_ST_ID;
//
//	/* we are not allowed to change our status if in failure */
//	if (failure_flag) {
//		set_unis(0); set_unim(0);
//		return station_id;
//	}
//
//	/* before killing the other gateway wait some time */
//	if (!red_started && (fault || (!unim && !unis)) &&
//		ticks - start_ticks < ms2tick(20000)) {
//		set_unis(0); set_unim(1);
//		return GTWM_ST_ID;
//	}
//	red_started = TRUE;
//
//	/* adjust the MVB fail signal timings considering the communication times */
//	if (station_id == GTWM_ST_ID)
//		C_sig_lo2hi_delay(MVB_DELAY_TICKS, ticks, mvb_failure_flag && standby_status == GTW_STANDBY_READY,
//						  mvb_failure);
//	else if (station_id == GTWS_ST_ID)
//		C_sig_hi2lo_delay(MVB_DELAY_TICKS, ticks, mvb_failure_flag && standby_status == GTW_STANDBY_READY,
//						  mvb_failure);
//	else mvb_failure = FALSE;
//	if (mvb_failure && !old_mvb_failure) log_event(mvb_fail);
//	if (!mvb_failure && old_mvb_failure) log_event(mvb_ok);
//	old_mvb_failure = mvb_failure;
//
//	/* follow the master in the sleep if required */
////	if (station_id == GTWS_ST_ID && !fault && !unim && !unis) d_sleep();
//
//
//    /* Modifica per inserire un ritardo sull'inhibit all'altro nodo */
//    /*  if FAULT is high or UNIM and UNIS are both high or low the other board failed */
//        if (!fault_flag || standby_status != GTW_STANDBY_READY)
// 	  ticks_fault = ticks;
//
// 	if (fault || (!unim && !unis) || (unim && unis && !mvb_failure))
//	{
//		if ( (standby_status == GTW_STANDBY_READY) &&
//		     ((ticks - ticks_fault) > ms2tick(3000))
//		   )
//		{
//			standby_status = is_A_flag ? GTW_STANDBY_FAILURE_B : GTW_STANDBY_FAILURE_A;
//			inhibit();
//     		//printf("killed con ticks %d ticks_fault %d",ticks,ticks_fault);
//		}
//
//        fault_flag = TRUE;
//  		set_unis(0); set_unim(1);
//		return GTWM_ST_ID;
//	}
//	else
//		fault_flag = FALSE;
//
//    /* Se l'altra unita' e' in fault non si spegne ma questa unita'diventa master */
//    /* si elimina perche provoca problemi come bus admin. rimanendo GTWM su MVB    */
//    /*
// 	if (fault || (!unim && !unis) || (unim && unis && !mvb_failure))
//	{
//  		set_unis(0); set_unim(1);
//		return GTWM_ST_ID;
//	}
//    */
//
//
//    	/* the other unit is ready */
//	standby_status = GTW_STANDBY_READY;
//
//	/* if UNIM is high we are a slave unit */
//	if (unim) {
//		set_unim(0); set_unis(1);
//		return GTWS_ST_ID;
//	}
//
//	/* if only UNIS is high we are a master unit */
//	set_unis(standby_status == GTW_STANDBY_READY && mvb_failure); set_unim(1);
//	return GTWM_ST_ID;
//}


/*=============================================================================================*/
/* Callbacks */

/*---------------------------------------------------------------------------------------------*/
/* MVBD shutdown report function */

//static void mvbd_shd_report(void)
//{
//	mvb_shd_failure_flag = shd_counter & 1;
//
//	if (mvb_shd_failure_flag) log_event(mvbd_shd_fail);
//	else log_event(mvbd_shd_ok);
//}


/*---------------------------------------------------------------------------------------------*/
/* MVB Bus Administrator led blink function */

static void mvb_ba_led(int status)
{
//	led_user5(status);
	set_led_mba(status);
}

/*---------------------------------------------------------------------------------------------*/
/* TESTCOM*/
static short TestCOM_cmd(short argc, char *argv[])
{
    static unsigned char stsTest=0;
    static unsigned char exit=0;
    static unsigned char toggle=0;
    char s[32];
    
    stsTest=0;
    do
    {
        led_user5(toggle & 0x02);
        toggle++;

        switch (stsTest)
        {
         /* Stampa menu */
         default:
         case 0 :
            printf("\n******* TEST COM v1.0 ********\n");       
            printf("    1 - test FLASH 5555/AAAA\n");       
//            printf("    2 - test FLASH 0-shifting\n");       
            printf("    2 - test EEPROM\n");       
            printf("    3 - test IO digitali\n");       
            printf("    4 - test MVB Traffic Memory\n");       
            printf("    5 - test SW1 MVB \n");       
            printf("    6 - test TX/RX MVB\n");       
            printf("    7 - test LED\n");       
            printf("    8 - test REPORT\n");       
//            printf("   10 - COPY RAM to ROM\n");       
            printf("    x - EXIT\n");       
            stsTest = 1;/*attesa della scelta*/
            break;
            
         case 1 :
    		if (!get_line(s, sizeof(s), FALSE))
    		    stsTest = 0;
    		
    		if(strcmp(s,"x") == 0)
    		{		
    	        
    	        printf("bye bye!!\n");
    	        exit= 1;
    	        break;
    	    }
    	    
    		else if(strcmp(s,"11") == 0)
            {		
//    	        printf("AVVIO MVB INIT\n");
//    	        MVB_init();
    	    }
    		else if(strcmp(s,"1") == 0)
    		{		
    	        printf("AVVIO TEST FLASH 5555/AAAA\n");
    	        tstflash_vel(0,0);
    	        stsTest = 0;
    	    }
//    		else if (strcmp(s,"2") == 0)
//    		{		
//    	        printf("Test FLASH\n");
//    	        tstflash();
//    	    }
    		else if (strcmp(s,"2") == 0)
    		{		
    	        printf("Test EEPROM\n");
    	        tstEEPROM(0,0);
    	        stsTest = 0;
    	        
    	    }
    		else if (strcmp(s,"3") == 0)
    		{		
    	        printf("Test IO\n");
    	        tstio(0,0);
    	        stsTest = 0;
    	    }
    		else if (strcmp(s,"4") == 0)
    		{		
    	        printf("test MVB Traffic Memory\n");
    	        test_mvb(0,0);
    	        stsTest = 0;
    	    }
    		else if (strcmp(s,"5") == 0)
    		{		
    	        printf("Test SW1 %d\n");
    	        test_SW1(0,0);
    	        stsTest = 0;
    	    }
    		else if (strcmp(s,"6") == 0)
    		{		
    	        printf("test TX/RX MVB\n");
    	        tstmvb_txrx(0,0);
    	    }
    	    else if (strcmp(s,"7") == 0)
    	    {
                printf("test LED\n");
                tstio_led(0,0);
                stsTest = 0;
            }
    	    else if (strcmp(s,"8") == 0)
    	    {
                printf("REPORT TEST\n");
                prn_report(0,0);
                stsTest = 0;
            }
    	    else if (strcmp(s,"10") == 0)
    	    {
                printf("COPY RAM->ROM\n");
//	             hw_ram2rom ();
	            //hw_reset();
            }
            else
                stsTest = 0;
            break;
           
           case 10 :/*exit*/
            break;
           }
       }
       while (!exit);
        
       exit = 0;
       stsTest=0;
       return 0;
}
/*---------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------*/
/* Determine the station ID */

NcEleID get_station_id(unsigned short mvb_hw_address)
{
//	unsigned long tick;								/* timeout tick time */

	/* initialize the time base */
	start_ticks = ticks = pi_ticks_elapsed();

	/* attach out MVB BA led blinking */
	mvb_ba_led_handler = mvb_ba_led;

	/* signal no fail */
	set_fault(0);

    
    ushell_register("tstcom", "", "esegue il Test COM",             TestCOM_cmd);
    ushell_register("tstflash", "", "esegue il Test FLASH",         tstflash_vel);
    ushell_register("tste2prom", "", "esegue il Test EEPROM",       tstEEPROM);
    ushell_register("tstio", "", "esegue il Test IO",               tstio);
    ushell_register("tstled", "", "esegue il test interfaccie MVB", tstio_led);
    ushell_register("tstsw1", "", "esegue il test dello switch SW1",test_SW1);
    ushell_register("tstmvb", "", "esegue il test della TM MVB",     test_mvb);
    ushell_register("tstmtx", "", "esegue il test dell'interfaccia MVB",     tstmvb_txrx);
    ushell_register("rpt", "", "stampa report memorizzati in flash", prn_report);
    
    
    ushell_register("di", "", "Digital Inputs",                     di_cmd);
    ushell_register("do", "<ch> <0|1>", "Digital Onputs",           do_cmd);
    ushell_register("w", "", "copy RAM to Flash",                   w_cmd);   

    ushell_exec("tstcom\n");
	return station_id = 0;
}


/*---------------------------------------------------------------------------------------------*/
/* WTB LL Report function */

//void user_ls_report(unsigned short r)
//{
//	/* update the master_conflict_flag */
//	if (r == LR_MASTER_CONFLICT) master_conflict_flag = TRUE;
//	else master_conflict_flag = FALSE;
//}


/*---------------------------------------------------------------------------------------------*/
/* Mapping Server Report function */

void user_map_report(unsigned short r)
{
	r = r;
}


/*=============================================================================================*/
/* Generic routines */

/*---------------------------------------------------------------------------------------------*/
/* Check the MVB lines */

void mvb_red_check(void)
{
	MvbcRedStatus status;	/* MVBC line redundancy status */

	/* acquire the MVB lines failure flags */
	if (mvbc_red_status(&status)) {
//		mvb_red_working_flag = FALSE;
		return;
	}
//	mvb_red_working_flag = TRUE;

	/* handle the transitions */
	if (!status.line_ok[0] && !mvb_a_failure_flag) {
		log_event(mvb_a_fail);
		mvb_a_failure_flag = TRUE;
	}
	if (status.line_ok[0] && mvb_a_failure_flag) {
		log_event(mvb_a_ok);
		mvb_a_failure_flag = FALSE;
	}
	if (!status.line_ok[1] && !mvb_b_failure_flag) {
		log_event(mvb_b_fail);
		mvb_b_failure_flag = TRUE;
	}
	if (status.line_ok[1] && mvb_b_failure_flag) {
		log_event(mvb_b_ok);
		mvb_b_failure_flag = FALSE;
	}
}


/*=============================================================================================*/
/* Main loops */

/*---------------------------------------------------------------------------------------------*/
/* Acquire the hardware signals */

#define get_hwi_sig(x) do {																		\
	char t;																						\
	C_sig_delay(DI_DELAY_TICKS, ticks, get_##x(), t);											\
	hw_io.x = signal_b1_a2(t);																	\
} while (0)

//
//void get_hw_io(void)
//{
//	get_hwi_sig(comm);
//	get_hwi_sig(sleout);
//	get_hwi_sig(mvbon);
//	get_hwi_sig(key);
//	get_hwi_sig(carbaton);
//	get_hwi_sig(lucion);
//	get_hwi_sig(recon);
//	get_hwi_sig(rstretein);
//	get_hwi_sig(velosit);
//	get_hwi_sig(stato_mt);
//	get_hwi_sig(risin1);
//	get_hwi_sig(risin2);
//	get_hwi_sig(risin3);
//	get_hwi_sig(risin4);
//	get_hwi_sig(risin5);
//	get_hwi_sig(risin6);

//	hw_io.comm      = signal_b1_a2(get_comm());
//	hw_io.sleout    = signal_b1_a2(get_sleout());
//	hw_io.mvbon     = signal_b1_a2(get_mvbon());
//	hw_io.key       = signal_b1_a2(get_key());
//	hw_io.carbaton  = signal_b1_a2(get_carbaton());
//	hw_io.lucion    = signal_b1_a2(get_lucion());
//	hw_io.recon     = signal_b1_a2(get_recon());
//	hw_io.rstretein = signal_b1_a2(get_rstretein());
//	hw_io.velosit   = signal_b1_a2(get_velosit());
//	hw_io.stato_mt  = signal_b1_a2(get_stato_mt());
//	hw_io.risin1    = signal_b1_a2(get_risin1());
//	hw_io.risin2    = signal_b1_a2(get_risin2());
//	hw_io.risin3    = signal_b1_a2(get_risin3());
//	hw_io.risin4    = signal_b1_a2(get_risin4());
//	hw_io.risin5    = signal_b1_a2(get_risin5());
//	hw_io.risin6    = signal_b1_a2(get_risin6());
//}


/*---------------------------------------------------------------------------------------------*/
/* Acquire the datasets */

//#define get_ds(x) do {																			\
//	lp_get_data_set(&ds_gtw_ds##x);																\
//	if ((!no_mvb_refresh_flag && rf_gtw_ds##x >= RF_TMO) ||		 								\
//		gtw_ds##x.chk == GTW_CHK_ERROR || gtw_ds##x.chk == GTW_CHK_UNDEFINED) {					\
//		gtw_ds##x##_valid = FALSE;																\
//		gtw_ds##x = default_gtw_ds##x;															\
//	} else gtw_ds##x##_valid = TRUE;															\
//} while (0)
//
// static void get_dso(void) { get_ds(o); }
//static void get_dsi(void) { get_ds(i); }
//static void get_dsm(void) { get_ds(m); }
//static void get_dss(void) { get_ds(s); }


/*---------------------------------------------------------------------------------------------*/
/* Check the hardware signals */

//#define check_hwi_sig(d,x) do {																	\
//	static char failed = FALSE;		/* TRUE if the signal failed at least one time */			\
//	if (!failed && gtw_ds##d##_valid) {															\
////		C_sig_lo2hi_delay(MVB_DELAY_TICKS, ticks, gtw_ds##d##.x ^ hw_io.x, failed);				\
//		if (failed) log_event(x##_fail);														\
//	}																							\
//} while (0)
//
//#define check_hwo_sig(x) do {																	\
//	static char failed = FALSE;		/* TRUE if the signal failed at least one time */			\
//	if (!failed) {																				\
//		if (get_##x() != signal_a2_b1(hw_io.x)) failed = TRUE;									\
//		if (failed) log_event(x##_fail);														\
//	}																							\
//} while (0)
/*
#define check_hw_signals(d) do {	\
	check_hwi_sig(d,comm);			\
	check_hwi_sig(d,sleout);		\
	check_hwi_sig(d,mvbon);			\
	check_hwi_sig(d,key);			\
	check_hwi_sig(d,carbaton);		\
	check_hwi_sig(d,lucion);		\
	check_hwi_sig(d,recon);			\
	check_hwi_sig(d,rstretein);		\
	check_hwi_sig(d,velosit);		\
	check_hwi_sig(d,stato_mt);		\
	check_hwi_sig(d,risin1);		\
	check_hwi_sig(d,risin2);		\
	check_hwi_sig(d,risin3);		\
	check_hwi_sig(d,risin4);		\
	check_hwi_sig(d,risin5);		\
	check_hwi_sig(d,risin6);		\
	check_hwo_sig(risout1);			\
	check_hwo_sig(risout2);			\
	check_hwo_sig(risout3);			\
	check_hwo_sig(risout4);			\
} while (0)
*/

/*---------------------------------------------------------------------------------------------*/
/* Main loop when GTWM */

void gtwm_loop(void)
{
//	static Type_WTBStatus		wtb_status;			/* current WTB status                     */
//	static Type_Topography		wtb_topo;			/* current WTB topography                 */
////	static map_VehicleDescr		*veh_desc_p;		/* pointer to our vehicle descriptor      */
//	static T_PDM_STATUS			pdm_status;			/* current PDM status                     */
//	static type_StatisticData	*wtbdrv_s;			/* WTB driver statistic data              */
//	static WTBDriverStatus		*wtbdrv_d;			/* WTB driver detailed statistic data     */
//
//	static char			can_sleep = FALSE;	/* if TRUE we could sleep                 */
//	static unsigned long		sleep_ticks;		/* can_sleep hi->lo transition time       */
//	static unsigned long		sleep_noi_ticks;	/* no-inauguration transition time        */
//
////	static unsigned long		total_tx_frames;	/* total number of WTB frames transmitted */
////	static unsigned long		last_tx_ticks;		/* last time there was a WTB transmission */
//
//	map_TrainTopography			*new_topo;			/* train topography pointer               */
//	int							i;					/* generic integer                        */
////	unsigned long				ul;					/* generic unsigned long                  */
//    unsigned char j;



//    static unsigned char Acc;

/**** STEP 1 : acquire all the inputs ****/

	/* acquire the hardware signals */
//	get_hw_io();

	/* get the GTW_DSI and GTW_DSS dataset signals checking the MVB communication */
//	get_dsi();
//	if (standby_status != GTW_STANDBY_ABSENT) get_dss();

	/* get new WTB link layer status and topography */
//	ls_t_GetStatus(&wtb_status);
//	if (wtb_status.node_address == 127 || ls_t_GetTopography(&wtb_topo))
//		pi_zero8(&wtb_topo, sizeof(wtb_topo));


//	/* get the UIC train topography */
//#ifndef UMS
//	if (map_GetTrainTopography(0, &new_topo) == MAP_OK) map_topo = *new_topo;
//	else pi_zero8(&map_topo, sizeof(map_topo));
//#endif

//	/* find the pointer to our vehicle descriptor in the topography */
//	veh_desc_p = 0;
//	if (map_topo.state.validity == MAP_NADI_VALID)
//	{
//		for (i = 0; i < map_topo.state.VehicleNo; i++)
//		{
//			if (map_topo.vehicle[i].NodeAddr == map_topo.state.ownaddr)
//				veh_desc_p = &map_topo.vehicle[i];
//				break;
//		}
////occhio	}
//
//	/*  Legge il proprio descrittore di Veicolo  */
//    	map_GetOwnVhDescriptor(0, &veh_desc_p); //23/06/09 10.17.04
//        c_set_bit(veh_desc_p->DynAttr,0);
//        if(gtw_dsi.uwtm_pdm_mode_set == 1)
//        {
//            if (ucToogle || (uiStrtUp == 19))
//        	{
//                printf("uiStrtUp = %d\n",uiStrtUp);
//                c_set_bit(veh_desc_p->DynAttr, 1);
//                c_set_bit(veh_desc_p->DynAttr, 3);
//                /* fix the node strenght */
//    	        veh_desc_p->TCNType = 3 /*L_WEAK*/;
//    
//    	        /* set the new vehicle descriptor */
//                map_SetVehicleDescriptor(0, veh_desc_p, 0, 0);
//    
//        	    // Stampa indirizzo TCN e UIC
//        	    //printf(" Indirizzo TCN veh_desc_p->NodeAddr = %d\n",veh_desc_p->NodeAddr);
//        	    //printf(" Indirizzo UIC veh_desc_p->RangeNo = %d\n",veh_desc_p->RangeNo);
//        	    ucToogle = 0;
//        	    printf("PDM mode set received, set the dynamic attribute   = %d\n",gtw_dsi.uwtm_pdm_mode_set);
//            	new_key =0;
//    
//        	}
//    
//        }
//        else if(gtw_dsi.uwtm_pdm_mode_set == 2)
//        {
//            // Caso in cui il pdm mode è 2
//            if(!ucToogle)
//        	{
//                c_clear_bit(veh_desc_p->DynAttr, 1);
//                c_clear_bit(veh_desc_p->DynAttr, 3);
//                /* fix the node strenght */
//    	        veh_desc_p->TCNType = 3 /*L_WEAK*/;
//    
//    	        /* set the new vehicle descriptor */
//                map_SetVehicleDescriptor(0, veh_desc_p, 0, 0);
//        	    // Stampa indirizzo TCN e UIC
//        	    //printf(" Indirizzo TCN veh_desc_p->NodeAddr = %d\n",veh_desc_p->NodeAddr);
//        	    //printf(" Indirizzo UIC veh_desc_p->RangeNo = %d\n",veh_desc_p->RangeNo);
//    
//        	    printf("PDM received, reset the dynamic attribute  =  %d\n",gtw_dsi.uwtm_pdm_mode_set);
//        	    ucToogle = 1;
//            	new_key =0;
//    
//        	}
//        }
//    }//occhio
//	/* get the PDM status */
//	pdm_get_status(&pdm_status);
//
//	/* get the WTB driver status */
//	d_read_statistic(&wtbdrv_s);
//	wtbdrv_d = (WTBDriverStatus*)wtbdrv_s->p_detailed_state;
//	if (wtbdrv_d->mauhf && !wtb_failure_flag)
//        {
// 		log_event(wtb_fail);
//		wtb_failure_flag = TRUE;
//	}
//
//
///**** STEP 2 : generate some global signals ****/
//
//	if (standby_status != GTW_STANDBY_ABSENT)
//		mvb_failure_flag = (!gtw_dsi_valid && !gtw_dss_valid);
//	mvb_activity_flag = gtw_dsi_valid;
//
//	wtb_inaugurated_flag = (wtb_status.node_address != 0 && wtb_status.node_address != 127);
//	wtb_master_flag      = (wtb_status.node_address == 1);
//	wtb_intermediate_flag = ((wtb_status.node_status.node_report & INT_BIT) != 0x00 );
//
//    f_DA1 = ((wtb_status.node_status.node_report & DA1_BIT) != 0x00);
//    f_DA2 = ((wtb_status.node_status.node_report & DA2_BIT) != 0x00);
//    f_DB1 = ((wtb_status.node_status.node_report & DB1_BIT) != 0x00);
//    f_DB2 = ((wtb_status.node_status.node_report & DB2_BIT) != 0x00);
//
//
///**** STEP 3 : check the validity of the signals acquired through the HW interface ****/
//
//	/* process the inhibit signal from MVB */
//	if (gtw_dss_valid && gtw_dss.inhbt) {
//		log_event(mvb_inhibit);
//		hw_halt();
//	}
//
//	check_hw_signals(s);
//
//
///**** STEP 4 : generate the software inputs using the GTW_DSI signals ****/
//
//	sw_io.comm    = select_a2(gtw_dsi.comm,  hw_io.comm);       // in 1
//	sw_io.sleout  = select_a2(gtw_dsi.sleout,   hw_io.sleout  );// in 2
//	sw_io.mvbon     = select_a2(gtw_dsi.mvbon,     hw_io.mvbon);
//	sw_io.key       = select_a2(gtw_dsi.key,       hw_io.key);
//	sw_io.carbaton  = select_a2(gtw_dsi.carbaton,  hw_io.carbaton);
//	sw_io.lucion    = select_a2(gtw_dsi.lucion,    hw_io.lucion);
//	sw_io.recon     = select_a2(gtw_dsi.recon,     hw_io.recon);
//	sw_io.rstretein = select_a2(gtw_dsi.rstretein, hw_io.rstretein);
//	sw_io.velosit   = select_a2(gtw_dsi.velosit,   hw_io.velosit);
//	sw_io.stato_mt  = select_a2(gtw_dsi.stato_mt,  hw_io.stato_mt);
//	sw_io.risin1    = select_a2(gtw_dsi.risin1,    hw_io.risin1);
//	sw_io.risin2    = select_a2(gtw_dsi.risin2,    hw_io.risin2);
//	sw_io.risin3    = select_a2(gtw_dsi.risin3,    hw_io.risin3);
//	sw_io.risin4    = select_a2(gtw_dsi.risin4,    hw_io.risin4);
//	sw_io.risin5    = select_a2(gtw_dsi.risin5,    hw_io.risin5);
//	sw_io.risin6    = select_a2(gtw_dsi.risin6,    hw_io.risin6);
//
//
///**** STEP 5 : sleep mode handling ****/
//
//        if (signal_a2_b1(sw_io.sleout) | signal_a2_b1(sw_io.key) | signal_a2_b1(sw_io.carbaton) |
//           signal_a2_b1(sw_io.lucion) | signal_a2_b1(sw_io.recon))
//           can_sleep = FALSE;
//	    else
//	    {
//
//		/* record the sleep allowed conditions */
//		if (!can_sleep) sleep_ticks = sleep_noi_ticks = ticks;
//		can_sleep = TRUE;
//
//		/* check the sleep timeout without inauguration */
//		if (wtb_inaugurated_flag) sleep_noi_ticks = ticks;
//		else if (ticks - sleep_noi_ticks > ms2tick(1*60*1000))
//			gtw_dsi.uwtm_sleep_ctrl = GTW_UWTM_SLEEP;
//
//		/* check the long sleep timeout */
//		if (ticks - sleep_ticks > ms2tick(45*60*1000))
//			gtw_dsi.uwtm_sleep_ctrl = GTW_UWTM_SLEEP;
//	    }
//
//
///**** STEP 6 : process the GTW_DSI commands ****/
//    /* setting Vehicle ID  */
//    if (gtw_dsi.vid != 0 && gtw_dsi.vid!= old_gtw_dsi_vid )
//    {
//
//        vid_bcd=0;
//
//        vid_bcd = bin2bcd(gtw_dsi.vid);
//        printf ("vid_bcd: %d  -> %d\n",gtw_dsi.vid,vid_bcd);
//
//        /* update vehiche id in configuration memory */
//        gtw_conf.veh_serial[2] = (vid_bcd >> 8) & 0xFF;
//        gtw_conf.veh_serial[3] = (vid_bcd >> 0) & 0xFF;
//        printf ("serial[2]: %x  -> %x\n",gtw_conf.veh_serial[2],vid_bcd);
//        printf ("serial[3]: %x  -> %x\n",gtw_conf.veh_serial[3],vid_bcd);
//
//	    gtw_conf.reboot_count++;
//
//	    if (set_conf(&gtw_conf)) printf("*** Errore nell'aggiornamento della configurazione ***\n");
//	    else printf("Nuovo numero di serie del veicolo : %lX\n", vid_bcd);
//
//        printf ("set_conf  vid : %02X%02X%02X%02X  ok\n",gtw_conf.veh_serial[0],gtw_conf.veh_serial[1],gtw_conf.veh_serial[2],gtw_conf.veh_serial[3]);
//
//        /* update vehiche id in Vehiche Descriptor    */
//        veh_desc_p->VehicleId[2] = gtw_conf.veh_serial[2];
//        veh_desc_p->VehicleId[3] = gtw_conf.veh_serial[3];
//   		map_SetVehicleDescriptor(0, veh_desc_p, 0, 0);
//        printf ("VD->VehicleId[2]: %x  -> %x\n",gtw_conf.veh_serial[2],veh_desc_p->VehicleId[2]);
//        printf ("VD->VehicleId[3]: %x  -> %x\n",gtw_conf.veh_serial[3],veh_desc_p->VehicleId[3]);
//
//    }
//    old_gtw_dsi_vid = gtw_dsi.vid;
//
//	/* handle the opmode_select command */
//	if (gtw_dsi.uwtm_opmode_select != GTW_UWTM_UNDEFINED && veh_desc_p) {
//		static unsigned char last_TCNType = 0xFF;
//		unsigned char		 TCNType;
//
//		switch (gtw_dsi.uwtm_opmode_select) {
//			case GTW_UWTM_PASSIVE:
//				TCNType = L_UNDEFINED;
//				break;
//			case GTW_UWTM_SLAVE:
//				TCNType = L_SLAVE;
//				break;
//			case GTW_UWTM_WEAK:
//				if (veh_desc_p->TCNType == L_SLAVE) TCNType = L_SLAVE;
//				else TCNType = L_WEAK;
//			    break;
//			case GTW_UWTM_STRONG:
//				TCNType = L_STRONG;
//				break;
//			default:
//				TCNType = veh_desc_p->TCNType;
//		}
//
//		if (TCNType != last_TCNType && TCNType != veh_desc_p->TCNType) {
//			veh_desc_p->TCNType = TCNType;
//#ifndef UMS
//	   		map_SetVehicleDescriptor(0, veh_desc_p, 0, 0);
//#endif
//		}
//
//		last_TCNType = TCNType;
//	}
//
//	/* handle the sleep_ctrl command */
//	if (gtw_dsi.uwtm_sleep_ctrl != old_gtw_dsi.uwtm_sleep_ctrl)
//		switch (gtw_dsi.uwtm_sleep_ctrl) {
//			case GTW_UWTM_NOSLEEP: ls_t_CancelSleep(); break;
//			case GTW_UWTM_SLEEP:   ls_t_SetSleep();    break;
//		}
//
//	/* handle the inaug_ctrl command */
//	if (gtw_dsi.uwtm_inaug_ctrl != old_gtw_dsi.uwtm_inaug_ctrl)
//		switch (gtw_dsi.uwtm_inaug_ctrl)
//		{
//			case GTW_UWTM_ALLOWED:   ls_t_Allow();   break;
//			case GTW_UWTM_INHIBITED: ls_t_Inhibit(); break;
//		}
//
//	/* handle the pdm_mode_set command */
////occhio	if (ticks > ms2tick(2000))
//	if (map_topo.state.validity == MAP_NADI_VALID)
//    	if (gtw_dsi.uwtm_pdm_mode_set != old_gtw_dsi.uwtm_pdm_mode_set )
//    	{
//            /* set PDM mode */
//    		if (!gtw_dsi.uwtm_pdm_mode_set)
//    			pdm_set_node_mode_mask(0x0000, gtw_dsi.uwtm_pdm_mode_set);
//    		else
//    		{
//    			if (!comm_flag) pdm_set_node_mode_mask(0x00FF, 0x0100);
//    			else pdm_set_node_mode_mask(0x00FF, 0x0000);
//    		}
//    
//    	}
//    	else
//    	    new_key =1;
//
//
///**** STEP 7 : generate the software output signals ****/
//
//	/* the software outputs are fixed to OFF */
//	if (!DO_forced)
//
//	{
//		    sw_io.risout1   = GTW_A2_SIGNAL_OFF;
//    	    sw_io.risout2   = GTW_A2_SIGNAL_OFF;
//	        sw_io.risout3   = GTW_A2_SIGNAL_OFF;
//	        sw_io.risout4   = GTW_A2_SIGNAL_OFF;
//
//	}
///**** STEP 8 : generate the hardware output signals ****/
//
//	/* select the forced signal if any */
//	hw_io.risout1   = select_a2(gtw_dsi.risout1, sw_io.risout1);
//	hw_io.risout2   = select_a2(gtw_dsi.risout2, sw_io.risout2);
//	hw_io.risout3   = select_a2(gtw_dsi.risout3, sw_io.risout3);
//	hw_io.risout4   = select_a2(gtw_dsi.risout4, sw_io.risout4);
//
//	/* put the signals at the hardware interface */
//	set_risout1(signal_a2_b1(hw_io.risout1));
//	set_risout2(signal_a2_b1(hw_io.risout2));
//	set_risout3(signal_a2_b1(hw_io.risout3));
//	set_risout4(signal_a2_b1(hw_io.risout4));
//
//
///**** STEP 9 : generate the GTW_DSO signals ****/
//
//    if (!new_key)
//    {
//        gtw_dso.nadi_valid = 0;
//        if (cont--) new_key = 0;
//    }
//    else
//        {
//            gtw_dso.nadi_valid = (map_topo.state.validity == MAP_NADI_VALID);
//            cont = 20;
//        }
//
//    if (map_topo.state.validity == MAP_NADI_VALID )
//    {
//        /* ricava i dati per la LV */
//        elab_topography();
//
//        /* Veicolo di appartenenza */
//        gtw_dso.v_app = veh_desc_p->RangeNo;
//
//   	    for (i = 0; i < map_topo.state.VehicleNo; i++)
//   	    {
//            for (j = 0; j < 4; j++)
//              gtw_vid.vid[4*i+j] = teleout.VD_ptr[i+1]->VehicleId[j];//map_topo.vehicle[i].VehicleId[j];
//            
//            /* 
//                Se la master del convoglio ha vid(0)= 80 e vid(1)=90
//                è una Z1 e setta la flag 
//            */
//            if((locv_mt[1] == (i+1)) && gtw_vid.vid[4*i]==0x80 && gtw_vid.vid[(4*i) +1]==0x90)  
//                gtw_dso.Master_is_Z1 =1;
//        }
//        // pacchetti R3 di locomotiva, previa congruenza
//        num_loc = get_dir (&ds_330, &dir_loco_r3, r3loc_logico, r3loc_chk, MAX_MOT-1);
//
//
//    }
//    else
//    {
//        gtw_dso.Master_is_Z1 =0;
//        for (j = 0; j <= 63; j++) teleout.logico[j] =0;
//        for (j = 0; j <= MAX_MOT; j++)
//        {
//            locv_mt[j] = 0;
//            tab_rem[j] = 0;
//            teleout.ind_nodo[j] =0;
//	        /*  azzera i vid nel DSO  */
//	        gtw_dso.vid[j] = 0;
//        }
//        teleout.db.v_mt =0;
//        gtw_dso.v_app  =0;
//    }
//
//    for (i = 0; i < map_topo.state.VehicleNo; i++)
//        gtw_dso.uic[i] = map_topo.vehicle[i].RangeNo;
//
//
//    /*  pubblica i Vehicle ID nel GTW_VID  */
//    if (gtw_vid.life++ == 16) gtw_vid.life = 0;
//    gtw_vid.chk = 1;
//
//    gtw_dso.logico[1] = tab_rem[0]; //teleout.logico[teleout.ind_nodo[1]];
//    gtw_dso.logico[2] = tab_rem[1]; //teleout.logico[teleout.ind_nodo[2]];
//    gtw_dso.logico[3] = tab_rem[2]; //teleout.logico[teleout.ind_nodo[3]];
//    gtw_dso.logico[4] = tab_rem[3]; //teleout.logico[teleout.ind_nodo[4]];
//    gtw_dso.vd_mt[1] = locv_mt[1];  //teleout.ind_nodo[1];
//    gtw_dso.vd_mt[2] = locv_mt[2];  //teleout.ind_nodo[2];
//    gtw_dso.vd_mt[3] = locv_mt[3];  //teleout.ind_nodo[3];
//    gtw_dso.vd_mt[4] = locv_mt[4];  //teleout.ind_nodo[4];
//
//	/* set the conflict status */
//	gtw_dso.uwtm_conflict = master_conflict_flag;
//
//	/* set the opmode status */
//	switch (wtb_status.link_layer_state) {
//		case LS_READY_TO_BE_NAMED:
//			gtw_dso.uwtm_opmode = GTW_UWTM_OPMODE_SLAVE;
//			break;
//		case LS_READY_TO_NAME:
//		case LS_REG_OPERATION_STRONG:
//			gtw_dso.uwtm_opmode = GTW_UWTM_OPMODE_STRONG;
//			break;
//		case LS_REG_OPERATION_SLAVE:
//			gtw_dso.uwtm_opmode = GTW_UWTM_OPMODE_WEAK_SLAVE;
//			break;
//		case LS_REG_OPERATION_WEAK:
//			gtw_dso.uwtm_opmode = GTW_UWTM_OPMODE_WEAK_MASTER;
//			break;
//		default:
//			gtw_dso.uwtm_opmode = GTW_UWTM_OPMODE_PASSIVE;
//	}
//    
//        /* Reset HW on MAU busy */
//    if (wtbdrv_d->maubusy == 1) /*|| (err_maubusy))*/
//	{
//		if (err_maubusy)
//		{
//			cnt_maubusy++;
//			printf("cnt_maubusy %d\n",cnt_maubusy);
//        	if (cnt_maubusy>=50)
//			{
//                hw_reset();
//			}
///*        	if (cnt_maubusy>=20)
//			{
//		    ls_t_Reset();
//			}
//*/			
//		}
//		else
//		{
//            printf(" \n WARNING maubusy error active\n");
//            err_maubusy = 1;   				/*that is a not recoverable error*/
//		}
//	}
//	else
//	    {
//	         err_maubusy = 0;
//	         cnt_maubusy = 0;
//	    }
//
//
//
//
//
//
//	/* the UWTM state is always "regular"... */
//	gtw_dso.uwtm_state = GTW_UWTM_STATE_REGULAR;
//
//        /* set the leading status */
//	if (gtw_dsi.uwtm_leading_request == GTW_UWTM_LEADING  && sw_io.key == GTW_A2_SIGNAL_ON)
//		gtw_dso.uwtm_leading = GTW_UWTM_LEADING;
//	else gtw_dso.uwtm_leading = GTW_UWTM_NOT_LEADING;
//
//	/* non prevista var nel DSO*/
//	/*if (veh_desc_p)
//	    gtw_dso.uwtm_uic_address = veh_desc_p->RangeNo;
//        else
//        gtw_dso.uwtm_uic_address = 0;
//    */
//
//	/* set the topo_count status */
//	gtw_dso.uwtm_topo_count = wtb_topo.topo_counter;
//
//	/* set the number_vehicles status */
//	gtw_dso.uwtm_number_vehicles = wtb_topo.number_of_nodes;
//
//	/* set the orientation status */
//	if (wtb_status.node_orient == L_INVERSE) gtw_dso.uwtm_orientation = GTW_UWTM_INVERSE;
//	else gtw_dso.uwtm_orientation = GTW_UWTM_NOT_INVERSE;
//
//	/* set the pdm_mode status */
//	gtw_dso.uwtm_pdm_mode = pdm_status.ex_mode;
//
//	/* set the driving direction */
//	gtw_dso.driving_direction = get_driving_direction(&map_topo);
//
//	/* set the digital inputs status */
//	gtw_dso.comm      = hw_io.comm;
//	gtw_dso.sleout = hw_io.sleout;
//	gtw_dso.mvbon     = hw_io.mvbon;
//	gtw_dso.key       = hw_io.key;
//	gtw_dso.carbaton  = hw_io.carbaton;
//	gtw_dso.lucion    = hw_io.lucion;
//	gtw_dso.recon     = hw_io.recon;
//	gtw_dso.rstretein = hw_io.rstretein;
//	gtw_dso.velosit   = hw_io.velosit;
//	gtw_dso.stato_mt  = hw_io.stato_mt;
//	gtw_dso.risin1    = hw_io.risin1;
//	gtw_dso.risin2    = hw_io.risin2;
//	gtw_dso.risin3    = hw_io.risin3;
//	gtw_dso.risin4    = hw_io.risin4;
//	gtw_dso.risin5    = hw_io.risin5;
//	gtw_dso.risin6    = hw_io.risin6;
//
//	/* set the software outputs status */
//	gtw_dso.risout1   = signal_a2_b1(sw_io.risout1);
//	gtw_dso.risout2   = signal_a2_b1(sw_io.risout2);
//	gtw_dso.risout3   = signal_a2_b1(sw_io.risout3);
//	gtw_dso.risout4   = signal_a2_b1(sw_io.risout4);
//
//	/* set the out_vita status */
//	gtw_dso.out_vita = gtw_dso.out_vita + 1;
//
//	/* set the status_standby status */
//	gtw_dso.status_standby = standby_status;
//
//	/* set the mvb_red status */
//	if (!mvb_red_working_flag)							gtw_dso.mvb_red = GTW_MVB_RED_UNKNOWN;
//	else if (mvb_a_failure_flag && mvb_b_failure_flag)	gtw_dso.mvb_red = GTW_MVB_RED_FAIL;
//	else if (mvb_a_failure_flag)						gtw_dso.mvb_red = GTW_MVB_RED_FAIL_A;
//	else if (mvb_b_failure_flag)						gtw_dso.mvb_red = GTW_MVB_RED_FAIL_B;
//	else												gtw_dso.mvb_red = GTW_MVB_RED_OK;
//
//	/* set the check variable */
//	gtw_dso.chk = GTW_CHK_OK;
//
//
///**** STEP 10 : generate the GTW_DSM signals ****/
//
//	/* set the vita status */
//    gtw_dsm.vita = gtw_dsm.vita + 1;
//
//	/* set the check variable */
//	gtw_dsm.chk = GTW_CHK_OK;
//
//
///**** STEP 11 : set all the dataset outputs ****/
//    if (wtb_inaugurated_flag && !wtb_intermediate_flag)
//    {
//
//   /** pulisce il segnale A1 Line Disturbed dal flick **/
//    LEG(f_DA1);
//    ONTD(signal_A1D);
//    OFTD(delay_A1D);
//    STA1(gtw_dso.line_A1_disturbed);
//
//    /** pulisce il segnale B1 Line Disturbed dal flick **/
//        LEG(f_DB1);
//        ONTD(signal_B1D);
//        OFTD(delay_B1D);
//    STA1(gtw_dso.line_B1_disturbed);
//
//    /** pulisce il segnale A2 Line Disturbed dal flick **/
//        LEG(f_DA2);
//        ONTD(signal_A2D);
//        OFTD(delay_A2D);
//    STA1(gtw_dso.line_A2_disturbed);
//
//    /** pulisce il segnale B2 Line Disturbed dal flick **/
//        LEG(f_DB2);
//        ONTD(signal_B2D);
//        OFTD(delay_B2D);
//    STA1(gtw_dso.line_B2_disturbed);
//    }
//    else
//    {
//        gtw_dso.line_A1_disturbed =0;
//        gtw_dso.line_A2_disturbed =0;
//        gtw_dso.line_B1_disturbed =0;
//        gtw_dso.line_B2_disturbed =0;
//    }
//
//
//    /** pulisce il segnale HFA dai flick **/
//    LEG((get_in_port(0)&DIN0_HWFLTA_)==0);
//    ONTD(signal_HFA);
//    OFTD(delay_HFA);
//    STA1(gtw_dso.line_A_HW_Fault);
//    /*if HFA is set , clear A1D and A2D */
//    if(gtw_dso.line_A_HW_Fault)
//    {
//        gtw_dso.line_A1_disturbed =0;
//        gtw_dso.line_A2_disturbed =0;
//    }
//
//    /** pulisce il segnale HFB dai flick **/
//    LEG((get_in_port(0)&DIN0_HWFLTB_)==0);
//    ONTD(signal_HFB);
//    OFTD(delay_HFB);
//    STA1(gtw_dso.line_B_HW_Fault);
//    /*if HFB is set , clear B1D and B2D */
//    if(gtw_dso.line_B_HW_Fault)
//    {
//        gtw_dso.line_B1_disturbed =0;
//        gtw_dso.line_B2_disturbed =0;
//    }
//    
//    /* clear all WTB diagnostic at power-on */
//    if (cnt_delay < 100)
//    {
//        gtw_dso.line_A1_disturbed =0;
//        gtw_dso.line_A2_disturbed =0;
//        gtw_dso.line_B1_disturbed =0;
//        gtw_dso.line_B2_disturbed =0;
//        gtw_dso.line_A_HW_Fault = 0;
//        gtw_dso.line_B_HW_Fault = 0;
//        cnt_delay++;
//    }
//
//
//
	lp_put_data_set(&ds_gtw_dso);
//	if (standby_status != GTW_STANDBY_ABSENT) lp_put_data_set(&ds_gtw_dsm);
//
//	lp_put_data_set(&ds_gtw_vid);
//
//
///**** STEP 12 : make a copy of the current DSI values ****/
//
//	old_gtw_dsi = gtw_dsi;
//	(uiStrtUp <= 19) ? uiStrtUp++ : uiStrtUp = 20;
//

/**** STEP 13 : WTB hangup supervision ****/

//	/* update the tx count and change time */
//	ul = wtbdrv_d->stat.A1.NoTxF + wtbdrv_d->stat.B1.NoTxF +
//		 wtbdrv_d->stat.A2.NoTxF + wtbdrv_d->stat.B2.NoTxF;
//	if (total_tx_frames != ul) {
//		total_tx_frames = ul;
//		last_tx_ticks = ticks;
//	}
//
//	/* check that we had a transmission in the last 2 seconds */
//	if (ticks - last_tx_ticks > ms2tick(2000)) {
//		log_event(wtb_hangup);
//		d_sleep();
//	}
}


/*---------------------------------------------------------------------------------------------*/
/* Main loop when GTWS */

//void gtws_loop(void)
//{
//**** STEP 0 : the first time we run we should wait the GTWM (we arrive here early) ****/
//
//	static char first_run = TRUE;
//
//	if (first_run) {
//		unsigned long tick = pi_ticks_elapsed() + ms2tick(1000);	/* timeout tick time */;
//
//		first_run = FALSE;
//
//		/* wait for the GTWM or timeout */
//		do get_dsm(); while (!gtw_dsm_valid && tick > pi_ticks_elapsed());
//	}
//
//
//**** STEP 1 : acquire all the inputs ****/
//
//	/* acquire the hardware signals */
//	get_hw_io();
//
//	/* get the GTW_DSI and GTW_DSM dataset signals */
//	get_dsi();
//	get_dsm();
//	mvb_failure_flag = (!gtw_dsi_valid && !gtw_dsm_valid);
//
//
//**** STEP 2 : check the validity of the signals acquired through the HW interface ****/
//
//	check_hw_signals(o);
//
//
//**** STEP 3 : generate the GTW_DSS values ****/
//
//	/* set the digital inputs status */
//	gtw_dss.comm = hw_io.comm;
//	gtw_dss.sleout  = hw_io.sleout;
//	gtw_dss.mvbon     = hw_io.mvbon;
//	gtw_dss.key       = hw_io.key;
//	gtw_dss.carbaton  = hw_io.carbaton;
//	gtw_dss.lucion    = hw_io.lucion;
//	gtw_dss.recon     = hw_io.recon;
//	gtw_dss.rstretein = hw_io.rstretein;
//	gtw_dss.velosit   = hw_io.velosit;
//	gtw_dss.stato_mt  = hw_io.stato_mt;
//        gtw_dss.risin1    = hw_io.risin1;
//	gtw_dss.risin2    = hw_io.risin2;
//	gtw_dss.risin3    = hw_io.risin3;
//	gtw_dss.risin4    = hw_io.risin4;
//	gtw_dss.risin5    = hw_io.risin5;
//	gtw_dss.risin6    = hw_io.risin6;
//
//	/* set the vita status */
//	gtw_dss.vita = gtw_dss.vita + 1;
//
//	/* set the check variable */
//	gtw_dss.chk = GTW_CHK_OK;
//
//
//**** STEP 4 : set all the outputs ****/
//
//	lp_put_data_set(&ds_gtw_dss);
//}
//

/*=============================================================================================*/
/* Shell commands */

/*---------------------------------------------------------------------------------------------*/
//static short gtw_debug_cmd(short argc, char *argv[])
//{
//	if (argc != 1) return -1;
//	argv = argv;
//
//	no_mvb_refresh_flag = TRUE;
//
//	return 0;
//}

//static short gtw_DI_cmd(short argc, char *argv[])
//{
//	if (argc != 1) return -1;
//	argv = argv;
//
//    printf("sleout  - in1  %d \n", get_sleout()     );
//    printf("key     - in2  %d \n", get_key()        );
//    printf("CBon    - in3  %d \n", get_carbaton()      );
//    printf("lucion  - in4  %d \n", get_lucion()   );
//    printf("RECon   - in5  %d \n", get_recon()     );
//    printf("comm    - in6  %d \n", get_comm()       );
//    printf("mvbon   - in7  %d \n", get_mvbon() );
//    printf("velosit - in8  %d \n", get_velosit()	);
//    printf("MT      - in9  %d \n", get_stato_mt()	);
//    printf("rstWTB  - in10 %d \n", get_rstretein()	    );
//    printf("riserva - in11 %d \n", get_risin2()	    );
//    printf("riserva - in12 %d \n", get_risin3()	    );
//    printf("riserva - in13 %d \n", get_risin4()	    );
//    printf("riserva - in14 %d \n", get_risin5()	    );
//    printf("riserva - in15 %d \n", get_risin6()	    );
//
//	return 0;
//}

//static short gtw_RidondDI_cmd(short argc, char *argv[])
//{
//	if (argc != 1) return -1;
//	argv = argv;
//
//    printf("unim_I-IND21 %d \n", get_unim());
//    printf("unis_I-IND20 %d \n", get_unis());
//    printf("fault_I-IND19 %d \n", get_fault());
//    printf("uni1-IND18 %d \n", get_uni1());
//
//	return 0;
//}


//static short gtw_DO_cmd(short argc, char *argv[])
//{
//	if (argc < 1) return -1;
//	argv = argv;
//
//	if (argc<2)
//	{
//	    printf("out1-OUTD1 %d \n", get_risout1());
//        printf("out2-OUTD2 %d \n", get_risout2());
//	    printf("out3-OUTD3 %d \n", get_risout3());
//	    printf("out4-OUTD4 %d \n", get_risout4());
//
//    }
//    else if (argc == 2)
//
//	{
//		if (argv[1][0] = 'u')
//		{
//			DO_forced = 0;
//			printf("Canali DO non forzati");
//		}
//    }
//
//    else if (argc == 3)
//	{
//		int canale;
//        int value;
//
//		DO_forced = 1;
//        canale = parse_number(argv[1]);
//        value = parse_number(argv[2]);
//
//        if ((canale>=1) && (canale<=4) && (value>=0) && (value<=1))
//        {
//                 switch(canale)
//                 {
//                 case 1:
//                 		if (value) 	sw_io.risout1   = GTW_A2_SIGNAL_ON;
//                 		else        sw_io.risout1   = GTW_A2_SIGNAL_OFF;
//
//                 break;
//                 case 2:
//
//                 		if (value) 	sw_io.risout2   = GTW_A2_SIGNAL_ON;
//                 		else        sw_io.risout2   = GTW_A2_SIGNAL_OFF;
//                 break;
//                 case 3:
//                 		if (value) 	sw_io.risout3   = GTW_A2_SIGNAL_ON;
//                 		else        sw_io.risout3   = GTW_A2_SIGNAL_OFF;
//
//                 break;
//                 case 4:
//                 		if (value) 	sw_io.risout4   = GTW_A2_SIGNAL_ON;
//                 		else        sw_io.risout4   = GTW_A2_SIGNAL_OFF;
//
//                 break;
//
//		    	}
//		}
//	}
//	return 0;
//}
//
//static short gtw_noRido_cmd(short argc, char *argv[])
//{
//    char noRido;
//
//	if (argc < 1) return -1;
//	argv = argv;
//
//	if (argc<2)
//	{
//        printf("status_standby %d \n",standby_status);
//        printf("noRido %d \n",gtw_conf.noRido);
//    }
//    else if (argc == 2)
//
//	{
//	    noRido = parse_number(argv[1]);
//        if (noRido != gtw_conf.noRido)
//    	{
//             int i;
//                gtw_conf.noRido = noRido;
//                gtw_conf.reboot_count++;
//                if (set_conf(&gtw_conf)) log_event(conf_write_fail);
//                for (i=0;i<10000;i++)
//                hw_reset();
//
//        }
//
//
//	}
//	return 0;
//}
/*---------------------------------------------------------------------------------------------*/

void vh_des_conf(void *ref, map_result status)
{
	ref = ref;
	status = status;
	printf(">>> Update result = %d <<<\n", status);
}
/*---------------------------------------------------------------------------------------------*/

/*static void dynattr_conf(void * ref, map_result status)
{
    ref = ref;
    if (status==MAP_OK)
    {
            printf("\ndynattr_conf -> Notifica transazione di chiave ok \r\n");
    }
    else
    {
            printf("\ndynattr_conf -> FAIL \r\n");
    }
}
*/

//static short gtw_topo_cmd(short argc, char *argv[])
//{
//    unsigned char i;
//	argv = argv;
//    printf("TC CORRENTE          %d \r\n",wtb_topo.topo_counter);        // rel 1.2
//    printf("NADI VALIDO          %d \r\n",(map_topo.state.validity == MAP_NADI_VALID));  // rel 1.2
//    // Indirizzo UIC veicolo locale
//    printf("VEICOLO DI APPART.   %d \r\n",veh_desc_p->RangeNo);
//
//    // Veicolo con banco abilitato
//    printf("VEICOLO MASTER TRAZ. %d \r\n",locv_mt[1]);
//
//    // numero di loco
//    printf("NUM. LOCOMOTIVE      %d \r\n",teleout.db.num_loc);
//
//    printf("EVENTUALE INVERSIONE %d \r\n",(gtw_dso.driving_direction == GTW_DRIVING_DIRECTION_DIR2));
//    printf("LOCO \r\n");
//    for (i=0;i<teleout.db.num_loc;i++)
//    {
//        printf("LOGICO %d NODE_ADDR %d VD_ptr %06lX \r\n",
//        teleout.db.lst_mot[i], teleout.ind_nodo[teleout.db.lst_mot[i]],teleout.VD_ptr[teleout.db.lst_mot[i]]);
//        printf("VD %d TCN_ADDR %d  UIC %d \n",i,map_topo.vehicle[i].NodeAddr,map_topo.vehicle[i].RangeNo);
//
//    }
//    printf("\n");
//
//    for (i=0;i<MAX_MOT;i++)
//    if (r3loc_logico[i]!=0)
//        printf("INDEX %d SD LOC R2 %d CHK %d \r\n",i,r3loc_logico[i],r3loc_chk[i]);
//    printf("\n");
//
//	return 0;
//}
/*---------------------------------------------------------------------------------------------*/

/*=============================================================================================*/
/* Entry point */

extern "C" void gtw_test_install(void);

PI_TASK(application_task, STRT_APPLICATION_TASK_ID, STRT_APPLICATION_TASK_PRIO)
{
	int			  led_time = 0;		/* application LED blink time                 */
	int			  wait_ticks;		/* ticks to wait for next execution           */
//	unsigned long new_station_id;	/* station id requested by redundancy control */

//    ushell_register("testcom", "", "esegue il Test COM", TestCOM_cmd);
//    testflash_init();
//	hw_watchdog_service();//addomestica il cane

	if (station_id == GTWM_ST_ID || station_id == GTWS_ST_ID) {

		while (1) {

			/* get the current time */
			ticks = pi_ticks_elapsed();


			/* execute the application loop and blink the led */
			if (station_id == GTWM_ST_ID) {
				gtwm_loop();
				led_user6(led_time < APPL_GTWM_LED_ON_MS);
				led_time = (led_time + APPL_CYCLE_MS) % (APPL_GTWM_LED_ON_MS + APPL_GTWM_LED_OFF_MS);
			}
			else if (station_id == GTWS_ST_ID) {
//				gtws_loop();
				led_user6(led_time < APPL_GTWS_LED_ON_MS);
				led_time = (led_time + APPL_CYCLE_MS) % (APPL_GTWS_LED_ON_MS + APPL_GTWS_LED_OFF_MS);
			}


			/* set the leds */
			set_led_hfw(wtb_failure_flag);
			set_led_hfm(mvb_shd_failure_flag || mvb_failure_flag ||
						mvb_a_failure_flag || mvb_b_failure_flag);
			set_led_wna(wtb_inaugurated_flag);
			set_led_mna(mvb_activity_flag);
			set_led_wms(wtb_master_flag);
			led_user1(mvb_a_failure_flag);
			led_user2(mvb_b_failure_flag);
			led_user3(wtb_inaugurated_flag);
			led_user4(wtb_master_flag);



//            tcn_monitor_mac();
			/* wait for the next execution time */
			wait_ticks = ms2tick(APPL_CYCLE_MS) - (pi_ticks_elapsed() - ticks);
			pi_wait (wait_ticks > 0 ? wait_ticks : 1);
		}
	}
}



/*=============================================================================================*/

