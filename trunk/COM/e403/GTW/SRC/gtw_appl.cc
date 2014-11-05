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
//ErrorEvent  (mvb_a_fail,      0x0022, "MVB line A failure");
//WarningEvent(mvb_a_ok,        0x0023, "MVB line A OK");
//ErrorEvent  (mvb_b_fail,      0x0024, "MVB line B failure");
//WarningEvent(mvb_b_ok,        0x0025, "MVB line B OK");
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
//static char				mvb_failure_flag;					/* the MVB is not working properly */

//static char				mvb_shd_failure_flag;		/* the MVB signals a SHD               */
//static char				mvb_a_failure_flag;			/* the MVB signals a failure on line A */
//static char				mvb_b_failure_flag;			/* the MVB signals a failure on line B */
//static char				wtb_failure_flag;			/* the WTB is not working properly     */
//static char				hw_failure_flag;			/* some hardware not working properly  */
//static char				failure_flag;				/* this board is in failure (latched)  */
//
//static char				mvb_activity_flag;			/* we are communicating over the MVB    */
//static char				wtb_inaugurated_flag;		/* the WTB is inaugurated               */
//static char				wtb_master_flag;			/* this board is the WTB master         */
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
            printf("   10 - COPY RAM to ROM\n");       
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
                stsTest = 0;
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
	            hw_ram2rom ();
	            hw_reset();
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

//    ushell_exec("tstcom\n");
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

	/* handle the transitions 
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
	*/
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
}


/*---------------------------------------------------------------------------------------------*/
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


}



/*=============================================================================================*/

