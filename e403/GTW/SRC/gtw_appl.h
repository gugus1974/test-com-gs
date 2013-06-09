/*=============================================================================================*/
/* Gateway application                                             					           */
/* Header file (gtw_appl.h)													                   */
/*																					           */
/* Version 1.01                                                                                                                                                     */
/*																					           */
/* (c)1998 Ansaldo Trasporti														           */
/* Written by Giuseppe Carnevale													           */
/*=============================================================================================*/


/*=============================================================================================*/
/* Incudes */

#include "hw_com.h"
#include "gtw_ncdb.h"
#include "gtw_ncap.h"






/*=============================================================================================*/
/* Defines */

/*---------------------------------------------------------------------------------------------*/
/* Some constants */

#define APPL_CYCLE_MS			100			/* application cycle time in ms              */

#define APPL_GTWM_LED_ON_MS		300			/* application led on time in ms when GTWM   */
#define APPL_GTWM_LED_OFF_MS	200			/* application led off time in ms when GTWM  */

#define APPL_GTWS_LED_ON_MS		300			/* application led on time in ms when GTWS   */
#define APPL_GTWS_LED_OFF_MS	700			/* application led off time in ms when GTWS  */

#define RF_TMO					500			/* datasets refresh timeout                  */

#define DI_DELAY_TICKS			ms2tick(300)	/* delay to be used for the HW inputs        */
#define MVB_DELAY_TICKS			ms2tick(500)	/* delay to be used for HW to MVB comparison */

#define DELAY_1S_TICKS			ms2tick(1000)	/* delay 1 sec */
#define DELAY_2S_TICKS			ms2tick(2000)	/* delay 2 sec */
#define DELAY_5S_TICKS			ms2tick(5000)	/* delay 5 sec */
#define DELAY_500MS_TICKS		ms2tick(500)	/* delay 500 msec */

/*---------------------------------------------------------------------------------------------*/
/* I/O signals access */

#define get_comm()      ((get_in_port(0) & DIN0_IND6 ) != 0)
#define get_sleout()	((get_in_port(0) & DIN0_IND1 ) != 0)
#define get_mvbon()		((get_in_port(0) & DIN0_IND7 ) != 0)
#define get_key()		((get_in_port(0) & DIN0_IND2 ) != 0)
#define get_carbaton()	((get_in_port(0) & DIN0_IND3 ) != 0)
#define get_lucion()	((get_in_port(0) & DIN0_IND4 ) != 0)
#define get_recon()		((get_in_port(0) & DIN0_IND5 ) != 0)
#define get_rstretein()	((get_in_port(0) & DIN0_IND10) != 0)
#define get_velosit()	((get_in_port(0) & DIN0_IND8 ) != 0)
#define get_stato_mt()	((get_in_port(0) & DIN0_IND9 ) != 0)
#define get_risin1()	((get_in_port(0) & DIN0_IND11) != 0)
#define get_risin2()	((get_in_port(0) & DIN0_IND12) != 0)
#define get_risin3()	((get_in_port(0) & DIN0_IND13) != 0)
#define get_risin4()	((get_in_port(0) & DIN0_IND14) != 0)
#define get_risin5()	((get_in_port(0) & DIN0_IND15) != 0)
#define get_risin6()	((get_in_port(0) & DIN0_IND16) != 0)

#define set_risout1(flag)	set_out_port(0, DOP0_OUTD1, flag ? DOP0_OUTD1 : 0)
#define set_risout2(flag)	set_out_port(0, DOP0_OUTD2, flag ? DOP0_OUTD2 : 0)
#define set_risout3(flag)	set_out_port(0, DOP0_OUTD3, flag ? DOP0_OUTD3 : 0)
#define set_risout4(flag)	set_out_port(0, DOP0_OUTD4, flag ? DOP0_OUTD4 : 0)

#define get_risout1() ((cpm_porta() & 0x0100) != 0)
#define get_risout2() ((cpm_porta() & 0x0400) != 0)
#define get_risout3() ((cpm_portb() & 0x0010) != 0)
#define get_risout4() ((cpm_portb() & 0x0100) != 0)

#define get_unim()  ((get_in_port(0) & DIN0_IND21) != 0)
#define get_unis()  ((get_in_port(0) & DIN0_IND20) != 0)
#define get_fault() ((get_in_port(0) & DIN0_IND19) != 0)
#define get_uni1()  ((get_in_port(0) & DIN0_IND18) != 0)

#define set_unim(flag)  set_out_port(1, DOP1_OUTD16, flag ? DOP1_OUTD16 : 0)
#define set_unis(flag)  set_out_port(1, DOP1_OUTD15, flag ? DOP1_OUTD15 : 0)
#define set_fault(flag) set_out_port(1, DOP1_OUTD13, flag ? 0 : DOP1_OUTD13)
#define set_inb(flag)   set_out_port(1, DOP1_OUTD14, flag ? DOP1_OUTD14 : 0)

//#define set_led_hfw(flag) set_out_port(0, DOP0_OUTD5,  flag ? DOP0_OUTD5  : 0)
//#define set_led_hfm(flag) set_out_port(0, DOP0_OUTD6,  flag ? DOP0_OUTD6  : 0)
//#define set_led_wna(flag) set_out_port(0, DOP0_OUTD7,  flag ? DOP0_OUTD7  : 0)
//#define set_led_mna(flag) set_out_port(0, DOP0_OUTD8,  flag ? DOP0_OUTD8  : 0)
//#define set_led_mba(flag) set_out_port(0, DOP0_RISA,   flag ? DOP0_RISA   : 0)
//#define set_led_wms(flag) set_out_port(0, DOP0_RISB,   flag ? DOP0_RISB   : 0)
//#define set_led_ris(flag) set_out_port(1, DOP1_OUTD17, flag ? DOP1_OUTD17 : 0)

#define set_led_hfw(flag) set_out_port(0, DOP0_OUTD7,  flag ? DOP0_OUTD7  : 0)
#define set_led_hfm(flag) set_out_port(0, DOP0_OUTD8,  flag ? DOP0_OUTD8  : 0)
#define set_led_wna(flag) set_out_port(0, DOP0_RISB,   flag ? DOP0_RISB   : 0)
#define set_led_mna(flag) set_out_port(0, DOP0_RISA,   flag ? DOP0_RISA   : 0)
#define set_led_mba(flag) set_out_port(0, DOP0_OUTD6,  flag ? DOP0_OUTD6  : 0)
#define set_led_wms(flag) set_out_port(1, DOP1_OUTD17, flag ? DOP1_OUTD17 : 0)
#define set_led_ris(flag) set_out_port(0, DOP0_OUTD5,  flag ? DOP0_OUTD5  : 0)


/*---------------------------------------------------------------------------------------------*/
/* I/O signals operations */

#define signal_b1_a2(b1) (b1 ? GTW_A2_SIGNAL_ON : GTW_A2_SIGNAL_OFF)
#define signal_a2_b1(a2) (a2 == GTW_A2_SIGNAL_ON ? 1 : 0)

#define select_a2(x, y) (x == GTW_A2_SIGNAL_UNDEF ? y : x)

#define  MAX_MOT           4+1



/*=============================================================================================*/
/* Typedefs */



typedef struct
{
    unsigned char valido;
    unsigned char congruente;
    unsigned char update;
}nadiresult;

typedef struct
{
   unsigned char        curr_stato_rete;         // Stato rete corrente wtb
   unsigned char        tc_curr;                 // tc corrente da GW (0 == invalido, preset)
   unsigned char        v_app;                   // Veicolo di appartenenza
   unsigned char        v_mt;                    // Veicolo master di trazione
   unsigned char        attiva;                  // La UDM e' attiva
   unsigned char        num_loc;                 // numero di loco globale
   unsigned char        lst_mot[MAX_MOT];        // lista ind.log.loco
   map_TrainTopography  topo;
}dbs_type;

typedef struct
{
    unsigned char     topo_valid;
    unsigned char     configTreno_valid;
    unsigned char     gwTCNmode;
    unsigned char     master_conflict;
    dbs_type          db;
    nadiresult        nadiState;
    unsigned char     logico [256];
    unsigned char     ind_nodo[MAX_MOT];   // indirizzo nodo assocato a logico
    map_VehicleDescr* VD_ptr[MAX_MOT];     // da msfntw puntatore al VD associato a logico
}t_teleout;


typedef struct {												/**** I/O signals in A2 format */
	char	comm, sleout, mvbon, key, carbaton, lucion, recon,	/* inputs                      */
			rstretein, velosit, stato_mt, risin1, risin2,
			risin3, risin4, risin5, risin6;
	char	risout1, risout2, risout3, risout4;					/* outputs                     */
} GtwIO;


/*=============================================================================================*/

