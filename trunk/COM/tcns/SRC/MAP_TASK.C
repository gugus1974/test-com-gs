/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> removed the O_960 pragmas                                                           */
/* <ATR:02> RECORD_M_ERR() macros fixed                                                         */
/* <ATR:03> "recorder.h" always included                                                        */
/* <ATR:05> Include "atr_map.h" with UIC Mapping Server spacific patches                        */
/* <ATR:06> (unmarked) SPrint_F() replaced with sprintf()                                       */
/* <ATR:07> show_buf type fixed                                                                 */
/*==============================================================================================*/

#include "atr_map.h"			/* <ATR:05> */

/*
   UUUUUUUUUUUUUUUUUUUU
   MAP_TASK
   Wolfgang Landwehr
   11. Oktober 1995, 16:03
   3
 */
#include "am_sys.h"				/* #kör */
#include "bmi.h"
#include "map.h"

#define       FUNC_PROMOTION   14
#define       FUNC_CALLER      01
#define       MAX_DATA       5000

#define INDIV_PERIOD   0x00		/* 0=25ms, 1=50ms, 2=100ms */
#define NORMAL         0x02
#define STRESSED       0x00
#define FRAME_SIZE     128
#define NODE_TYPE      0x31
#define NODE_VERSION   0x00

Type_NodeDescriptor descriptor;

unsigned char SCTR;
unsigned char SCTR_9_5;
unsigned char SCTR_INIT;
unsigned char uic95_confirm;
AM_RESULT promotion_result;
AM_NET_ADDR net_adr;
unsigned char promotion_buf[MAX_DATA];
void *ext_ref;

unsigned short k;
char show_buf[128];				/* <ATR:07> */

/*
   GENERAL TOP OF FILE
 */

   /*
      Main task for the uic mapping server
    */

   /*
      30.03.95 W. Landwehr
      Start of developement.
    */

   /*
      Copyright (c) 1995 by ABB-Henschel
    */


/*
   MODUL HISTORY
 */

   /*
      2.00 30.03.95  W. Landwehr
      Adopted for MyFriend from version 1.14
      2.01 07.07.95  W. Landwehr
      EH_TRIGGER_CYCLE set from 10 to 20
      2.02 11.10.95  W. Landwehr
      Workaround
    */


/*
   COMPILER SWITCHES
 */

/* <ATR:01> */
// #ifdef O_960    
// #pragma noalign    
// #endif

/*
   INCLUDEFILES
 */
#include "pi_sys.h"

#ifdef EH

#include <eh_api.h>

#endif

#include "RECORDER.H"			/* <ATR:03> */

#ifdef RECORD

#include "RECORDER.H"

#endif


/*
   DEFINES
 */

#ifdef EH

#define EH_TRIGGER_CYCLE 20		/* multiplied by 40ms */

#endif
#define MAP_CYCLE 5				/* This means 50ms cycle time if RMK_TICK is 10ms */

/* <ATR:02> */
#define RECORD_M_ERR(p_msg) nse_record("MAP_TASK", __LINE__, \
                                NSE_EVENT_TYPE_ERROR,\
                                NSE_ACTION_HALT , \
                                (strlen(p_msg)+1),   \
                                (unsigned char *)p_msg)


/*
   GLOBAL DECLARATIONS
 */

#ifdef EH

int map_slot_nr;
unsigned short map_eh_enable;

#endif
unsigned short map_task_counter;


/*
   IMPLEMENTATION 
 */

   /*
      promotion_multi_call_conf
    */

void 
promotion_multi_call_conf (char caller_fct,
						   void *ext_ref,
						   const AM_NET_ADDR * replier,
						   AM_RESULT status)
{

	/*
	   confirm procedure for multicast call
	 */
	uic95_confirm = TRUE;

	/*
	   TERM_OUT
	 */

#ifdef TERM_OUT


	for (k = 0; k < 128; k++) {
		show_buf[k] = 0;
	}
	SPrint_F (show_buf, "\r^Bpmz: mc_call_conf, AM_RESULT: %d", status);

	/* write_serv(show_buf); */

#endif

}


   /*
      promotion_rcv_conf
    */

void 
promotion_rcv_conf (unsigned char replier_fct,
					const AM_NET_ADDR * caller,
					void *in_msg_addr,
					unsigned long in_msg_size,
					void *ext_ref)
{
	short i;

	typedef struct {
		unsigned char key;
		unsigned char uic_marker;
		unsigned short uic_code;
		unsigned char format;
		unsigned char state;
	} s_reply_tgm;

	s_reply_tgm reply_tlgm;
	unsigned long reply_tlgm_lng;

	reply_tlgm.key = 0;
	reply_tlgm.uic_marker = 0;
	reply_tlgm.uic_code = 0x9A05;
	reply_tlgm.format = 0xFF;
	reply_tlgm.state = 0xFF;

	reply_tlgm_lng = 6;

	am_reply_requ (FUNC_PROMOTION,
				   &reply_tlgm,
				   reply_tlgm_lng,
				   ext_ref,
				   AM_OK);

	/*
	   TERM_OUT
	 */

#ifdef TERM_OUT


	for (k = 0; k < 128; k++) {
		show_buf[k] = 0;
	}
	sprintf (show_buf, "\r^Bpmz: UIC_9_5 received from Node %2d: %4d Byte", caller->vehicle, in_msg_size);

	write_serv (show_buf);

#endif

}


   /*
      promotion_reply_conf
    */

void 
promotion_reply_conf (unsigned char replier_fct, void *ext_ref)
{
	AM_RESULT am_result;

	am_rcv_requ (FUNC_PROMOTION,
				 &promotion_buf,
				 MAX_DATA,
				 ext_ref,
				 &am_result);
}


   /*
      map_task()
    */

void 
map_task (void)
{
	short result, map_mbx_id;

	map_task_counter = 0;
	map_mbx_id = pi_create_queue (2,	/* entries */
								  PI_ORD_FCFS,	/* message ordering */
								  &result);

	if (result != PI_RET_OK) {
		RECORD_M_ERR ("can't create MAP mailbox, STOP!");
	} else {

#ifdef EH

		/* init exeption handler */
		map_slot_nr = eh_api_request_slot (EH_TRIGGER_CYCLE);

#endif

		/*
		   TRIGGER EH AND CALL MAP_NPT
		 */

		while (1) {				/*  f o r  e v e r */
			pi_receive_queue (map_mbx_id, MAP_CYCLE, &result);	/* wait n msec */

			if (!((result == PI_RET_OK) || (result == PI_ERR_TO))) {
				RECORD_M_ERR ("error calling receive_queue, STOP");
			}
#ifdef EH


			if (map_eh_enable != 0xAFFE) {
				eh_api_refresh_slot (map_slot_nr, EH_TRIGGER_CYCLE);
			}
#endif
			map_task_counter++;	/* still alive flag */

			map_npt ();			/* the original Nadi Process for Train bus */

			/*
			   Performance Measurement
			 */

			if (SCTR_INIT == FALSE) {
				SCTR_INIT = TRUE;

				/*
				   install receiver
				 */
				am_bind_replier (FUNC_PROMOTION,
								 promotion_rcv_conf,
								 promotion_reply_conf,
								 &promotion_result);

				am_rcv_requ (FUNC_PROMOTION,
							 &promotion_buf,
							 MAX_DATA,
							 ext_ref,
							 &promotion_result);

			}
			if (SCTR == 1) {

				/*
				   cycle = 25ms
				 */
				SCTR = 0;

				descriptor.node_frame_size = FRAME_SIZE;
				descriptor.node_period = INDIV_PERIOD;
				descriptor.node_key.node_type = NODE_TYPE;
				descriptor.node_key.node_version = NODE_VERSION;

				ls_t_ChgNodeDesc (descriptor);

			}
			if (SCTR == 2) {

				/*
				   cycle = 100ms
				 */
				SCTR = 0;

				descriptor.node_frame_size = FRAME_SIZE;
				descriptor.node_period = NORMAL;
				descriptor.node_key.node_type = NODE_TYPE;
				descriptor.node_key.node_version = NODE_VERSION;

				ls_t_ChgNodeDesc (descriptor);

			}
			if (SCTR_9_5) {

				/*
				   UIC 9.5 multicast
				 */
				/* only this part to address multicast */
				net_adr.vehicle = MAP_MC_ALL_VEHICLES;
				net_adr.function = FUNC_PROMOTION;
				net_adr.device = 0xFF;

				result = map_multicast_requ (FUNC_CALLER,
											 &net_adr,
											 promotion_buf,
											 (unsigned long) MAX_DATA,
											 (MAP_INVOKE_CONF)
											 promotion_multi_call_conf,
											 ext_ref);

			}
		}

	}
}
