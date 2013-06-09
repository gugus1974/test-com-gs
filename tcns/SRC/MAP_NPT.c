/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:02> (unmarked) SPrint_F() replaced with sprintf()                                       */
/* <ATR:03> removed the O_960 pragmas                                                           */
/* <ATR:04> Include "atr_map.h" with UIC Mapping Server spacific patches                        */
/* <ATR:05> map_xbuf type fixed                                                                 */
/* <ATR:06> Event buffer mapping to screen commented out                                        */
/* <ATR:07> RECORD_M_EVENT() macro fixed                                                        */
/* <ATR:08> UI globals allocation moved from MAP_NPT to MAP_UI                                  */
/* <ATR:09> Do not overwrite the Reserved field of the vehicle descriptor                       */
/* <ATR:10> fixed the search for the node descriptor                                            */
/* <ATR:11> force inauguration on mode or leading/led change                                    */
/* <ATR:12> start the PDM when switching to update mode                                         */
/* <ATR:13> if MVB calls during setup reply error and wait for TCN reinitialization by master   */
/* <ATR:14> close the calls if setup failure                                                    */
/* <ATR:15> AdTranz compatibility: fix the validity in the NADI                                 */
/* <ATR:16> The Master do not send the GROUP msg in brodcast: a failure forces an inauguration  */
/* <ATR:17> The Slave waits the GROUP msg without timeout: the Master will inaugurate on fail   */
/*==============================================================================================*/

/* <ATR:04> */
#include "atr_map.h"

/* <ATR:11> */
#ifdef PDM
#include "atr_pdm.h"
#include "pdm_main.h"
#endif
static unsigned short act_order;

/*
   000000408
   MAP_NPT
   Wolfgang Landwehr
   12. Oktober 1995, 13:12
   3
 */

/*
   GENERAL TOP OF FILE
 */

   /*
      This module contains all functions for the UIC inauguration (setup mode) and
      the user interface functions (update mode).
    */

   /*
      03.02.94 W. Landwehr, 
      Start of development. Base is
      UIC-MAPPING SERVER SPECIFICATION 1.0, Jan. 94
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
	     1.00  23.04.94  W. Landwehr
	     Includes all test results until first implementation on target
	     system INTEL960.
	     1.01  03.05.94  W. Landwehr
	     Error handling without repeats after any timeout result by calling
	     am_@@@. Always the same error.
	     1.02  25.05.94  W. Landwehr
	     Setup with NADI distribution on vehicle bus.
	     Event recorder for error messages included.
	     1.03  22.06.94  W. Landwehr       {---MAP 1.12 on Train---}
	     am_call_cancel with replier.vehicle. Range numbers now distributed
	     from MASTER to SLAVEs due to row consistence.
	     1.04  01.09.94  W. Landwehr
	     Supplements after MAP 1.12 implementation. Now multicast is
	     implementation for WTB.
	     1.05  23.09.94  W. Landwehr
	     Adaptions wich were agreed within workshop 14.9.94:
	     -internal copy for NADI and GROUP_LIST
	     -call outstanding confirms within map_stop().
	     1.06  25.11.94  W. Landwehr
	     Advanced with MVB functionalities.
	     1.07  08.12.94  W. Landwehr
	     More advances for MVB, agreed with Mr. Schomisch ABB-CH.
	     1.08  05.01.95  W. Landwehr       {---MAP 1.17---}
	     changed INIT_ORDER -> NEXT_ORDER, INIT_ORDER only at first run of
	     map_npt. confirm function disabled in map_stop().
	     Call the indication function at senders site now available.
	     Semaphores to close NADI and GROUP_LIST acces after a
	     recv_conf_update and map_update_WTB. rr_size=6.
	     xNADI and yNADI now set to invalid after stop command.
	     1.09  20.01.95  W. Landwehr
	     Delay n cycle after setup to force system rest.
	     map_Set@@@ services repeat one time after error.
	     Implemented advanced error msg after fatal errors.
	     1.10  25.01.95  W. Landwehr
	     Disable confirm function after calling them.
	     1.11  08.02.95  W. Landwehr       {---MAP 1.20 on Train---}
	     Send NADI to MVB after INAUGURATION/INCLUDE. Increased nr of
	     parallel am_rcv_requ for update until MAP_MAX_NODES.
	   */


   /*
      2.00  23.02.95  W. Landwehr       {---MAP 2.00---}
      Starting adaption for WTB 1.9.
      2.01  14.03.95  W. Landwehr
      no error state after broadcast inm setup.
      2.02  17.03.95  W. Landwehr
      Now call map_report function after SETUP and UPDATE is available.
      2.03  30.03.95  W. Landwehr
      Several adaptions for the new map_bmi.c control interface.
      2.04  06.04.95  W. Landwehr
      Call map_CtrlReport with MAP_NEW_INAUGUARTION parameter.
      Now without halt after 10 tries after fatal_error. Inauguration
      loop gets quiet after a certain time.
      2.05  11.04.95  W. Landwehr
      Test services: map_show_nadi and TCN node info after start.
      2.06  12.04.95  W. Landwehr
      Semaphore deadlock fixed in recv_conf_update (discovered at AEG nodes)
      2.07  03.05.95  W. Landwehr
      Adaptions of version string and mvb handling.
      2.08  31.05.95  W. Landwehr
      Watching broadcast receiv in slave by timeout. This is necessary in case
      that an error occours during broadcast send in setup mode.
      2.08a 07.07.95  W. Landwehr       {---MAP 2.09 on Train---}
      New version string, because MAP_TASK.C adapted by Mr. Leeb.
      2.09  12.10.95  W. Landwehr       {---MAP 2.10 lab/Train---}
      Adoption for MyFriend, work around, remove DOS test aids.
    */


/*
   DESCRIPTION: TRACE INFO, ERROR NUMBERS
 */

   /*
      TRACE BUFFER REC_xxx
    */

	  /*
	     The trace buffers are separated for SETUP and UPDATE phase. All trace info
	     for setup is stored into >map_REC_SETUP< buffer (16 bytes, last is index).
	     The update trace info is stored into >map_REC_UPDATE_WTB< for all actions
	     only on WTB (32 bytes, last is index), and into >map_REC_UPDATE_MVB< for all
	     actions to the MVB (16 bytes, last is index).

	     To see the content, type:

	     COM960: rw E00xxxxx,40     were xxxxx is the address map_REC_SETUP
	     MICAS:  dw ssss:ooool40    were ssss:oooo is the address      "   
	   */


   /*
      REC_SETUP:  MASTER
    */

	  /*
	     10  Init phase, am_call_cancel                            map_setup_master
	     11  am_call_requ, send VEH_DES_REQUEST to all other nodes map_setup_master
	     12  VEH_DES received from all nodes and saved into NADI.  call_conf_setup
	     13  all responsesfor VEH_DES from slaves received.        map_setup_master
	     14  am_call_requ, send NADI to all other nodes on WTB.    map_setup_master
	     15  NADI response from all other received (WTB).          call_conf_setup
	     16  hint: MVB available on this WTB node.                 map_setup_slave/master
	     18  am_call_requ, send BROADCAST to all (Update ok).      map_setup_master
	     19  END_INAUGURATION ok (WTB).                            map_setup_master
	     42  error, status!=AM_OK for rcv. VEH_DES (expected 12).  call_conf_setup
	     45  error, status!=AM_OK for rcv. NADI resp. (exp. 15).   call_conf_setup
	     47  error, not all NADI responsed on WTB (exp. 16).       map_setup_master
	     49  error result during setup. NEW NAMING REQUIRED.       map_setup_master
	   */


   /*
      REC_SETUP:  SLAVE
    */

	  /*
	     1A  NADI request to the MASTER                            map_include_slave
	     1B  GROUP_LIST request to the MASTER (summary 3 times)    map_include_slave
	     1C  NADI from MASTER received                             call_conf_include
	     1D  GROUP_LIST from MASTER received                       call_conf_include
	     1E  any error                                             call_conf_include
	     1F  include ready                                         map_include_slave
	     20  Init phase, am_bind_replier.                          map_setup_slave
	     21  am_rcv_requ, get ready to receive the VEH_DES_REQUEST map_setup_slave
	     22  waiting for VEH_DES_REQUEST, waiting for NADI.        map_setup_slave
	     23  received VEH_DES_REQUEST, am_rcv_requ for rcv. NADI   recv_conf_setup
	     24  reply for received VEH_DES_REQUEST                    reply_conf_setup
	     25  NADI received, reply.                                 map_setup_slave
	     26  reply for received NADI.                              recv_conf_setup
	     27  hint: MVB available on this WTB node.                 map_setup_slave
	     27  hint: MVB available on this WTB node.                 map_setup_master
	     28  END_INAUGURATION, build_vrno()                        map_setup_slave
	     40  error status from am_bind_replier (expected 20).      map_setup_slave
	     41  error status from am_rcv_requ VEH_DES_REQ (exp. 21).  map_setup_slave
	     43  error, wrong tgm code (expected 23).                  recv_conf_setup
	     44  error status from am_rcv_requ NADI (expected 24).     reply_conf_setup
	     46  error, wrong tgm code (should be step 25).            recv_conf_setup
	     48  error result during setup. NEW NAMING REQUIRED by M.  map_setup_slave
	     4E  no NADI/GROUP_LIST received for include.              map_include_slave
	   */


   /*
      REC_SETUP:  MASTER/SLAVE
    */

	  /*
	     31  rule a, leading end is index 0 (ascend).              build_vrno,
	     32  rule a, leading end is index max (descend).           build_vrno,
	     33  rule b, leading car, nr of cars to 0 < max.           build_vrno
	     34  rule b, in the middle,  nr of cars to 0 > max         build_vrno
	     35  rule c, no leading car, tract. car is index 0         build_vrno
	     36  rule c,              tract. car is index max          build_vrno
	     37  rule d, rest of cases                                 build_vrno
	     ------------------------------------------------------------------------
	     3A  am_call_requ, send NADI to all nodes on MVB           map_setup_mvb
	     3B  Response for NADI from any MVB                        call_conf_setup_vb
	     3C  NADI response from all other received (MVB)           call_conf_setup_vb
	     4A  error, number of MVB > MAP_MAX_NO_MVB.                map_setup_mvb
	     4B  error, status!=AM_OK for rcv. NADI resp. (expect 3B)  call_conf_setup_vb
	     4C  error, not all NADI distributed on MVB (expect 3C)    call_conf_setup_vb
	     4D  error, no one NADI distributed on MVB (expect 3C)     call_conf_setup_vb
	   */


   /*
      REC_UPDATE: MASTER/SLAVE ON WTB
    */

	  /*
	     50  Any order was received.                               map_update
	     51  no order from MAP_UI.                                 map_update_WTB
	     52  order: by am_call_requ send a tgm to all other nodes. map_update_WTB
	     53  order: any from sended tgms has responsed.            call_conf_update
	     54  order: called a installed user function, ok.          map_update
	     55  order: called a installed user function, not ok.      map_update_WTB
	     56  order: all responses for sended tgms are received.    map_update_WTB
	     58  Perform IDLE_STATE_REQUEST on local, after response.  map_update_WTB
	     59  Perform DYN_ATTR_REQUEST on local.                    map_update_WTB
	     5A  Perform GROUP_LIST_REQUEST on local.                  map_update_WTB
	     5B  Not all nodes have responsed by Set_xxx user service. map_update_WTB
	     5C  order from interface UI (to WTB and MVB members)      map_update
	     5D  order from interface WTB (to MVB members only).       map_update
	     5E  order from interface MVB (to WTB members only).       map_update
	     5F  not all nodes have responsed (AEG sos time)           map_update_WTB
	     60  Init: am_bind_replier, am_rcv_requ 1+2.               map_update
	     61  received: DYN_ATTR_REQU, VEH_DES (node addr. sender)  recv_conf_update
	     62  received: GROUP_LIST_REQUEST, new groups from sender. recv_conf_update_wtb
	     63  received: IDLE_STATE_REQUEST, set IDLE_MODE at own n. recv_conf_update_wtb
	     64  received: SWITCHOVER_REQUEST.                         recv_conf_update_wtb
	     65  received: SWITCHBACK_REQUEST.                         recv_conf_update_wtb
	     66  received: MULTICAST (only for TEST).                  recv_conf_update_wtb
	     67  reply the request ok with call indicate...            recv_conf_update_wtb
	     68  reply the request ok                                  recv_conf_update_wtb
	     69  ready for the next...                                 recv_conf_update_wtb
	     6A  next am_rcv_requ, get ready to receive any tgm.       reply_conf_update_wtb
	     --------------------------------------------------------------------------
	     82  error, status != AM_OK, internal (expected 53).       call_conf_mc_update
	     83  error, status != AM_OK, internal (expected 53).       call_conf_update
	     84  error, status != AM_OK, remote (expected 53).         call_conf_mc_update
	     85  NADI request from SLAVE for  i n c l u d e            recv_conf_update_wtb
	     86  GROUP_LIST request from SLAVE for  i n c l u d e      recv_conf_update_wtb
	     90  error status from am_bind_replier (expected 60).      map_update
	     9E  error status from am_rcv_requ any tgm (expected 60).  map_update
	     9F  error status from am_rcv_requ any tgm. (expected 60). map_update
	     91  error, node not found in NADI (expected 61).          recv_conf_update
	     97  reply the request not ok (expected 67).               recv_conf_update
	     99  fatal error, number of nodes in train topography=0!!  main module map_npt
	     9A  error by call am_rcv_requ, (should be 6A).            reply_conf_update
	   */


   /*
      REC_UPDATE: MASTER/SLAVE STOP ON WTB/MVB
    */

	  /*
	     70  stop event from TCN level during UPDATE.              map_stop
	     71  stop event from TCN level during SETUP, STRONG.       map_stop
	     72  stop event from TCN level during SETUP, SLAVE.        map_stop
	     75  order stored into buffer.                             map_PutOrder
	     76  order read from buffer.                               map_GetOrder
	     7D  order from UI, last order not yet ready.              map_PutOrder
	     7E  invalid order.                                        map_PutOrder
	     7F  no buffer element empty to store the current order.   map_PutOrder
	   */


   /*
      REC_UPDATE: MASTER/SLAVE MULTICAST ON WTB/MVB
    */

	  /*
	     E1  Multicast: ALL_VEHICLES.                              map_update_wtb
	     E2  Multicast: ALL_CARRIAGES.                             map_update_wtb
	     E3  Multicast: TRACTION_LEADER.                           map_update_wtb
	     E4  Multicast: LEADING_VEHICLE.                           map_update_wtb
	     E5  Multicast: LAST_VEHICLE.                              map_update_wtb
	     E6  Multicast: selected by GROUP_NUMBER (n x E6)          map_update_wtb
	     E7  Multicast: ready by GROUP_NUMBER (after n x E6)       map_update_wtb
	     E8  Multicast: GROUP_NUMBER out of range                  map_update_wtb
	     E9  Multicast: no GROUP_LIST entries, no MC               map_update_wtb
	     ED  Multicast: am_call_requ after E1..E5.                 map_update_wtb
	     EF  Multicast: own address, after E1..E5.                 map_update_wtb
	   */


   /*
      REC_UPDATE: MASTER/SLAVE WTB TO MVB, ON MVB
    */

	  /*
	     A0  initialise finished.                                  map_update_mvb
	     A1  order: no order (default case).                       map_update_mvb
	     A2  order: by am_call_requ send a tgm to all other nodes. map_update_mvb
	     A3  order: a sended tgm has responsed.                    call_conf_update_mvb
	     A4  order: not all nodes have responsed.                  call_conf_update_mvb
	     A5  order: all nodes have responsed.                      call_conf_update_mvb
	     A8  invalid order.                                        map_update (MVB)
	     6B  Init: am_bind_replier, am_rcv_requ 1+2.               map_update
	     B1  received: DYN_ATTR_REQU, VEH_DES (node addr. sender). recv_conf_update_mvb
	     B2  received: GROUP_LIST_REQUEST, new groups from sender. recv_conf_update_mvb
	     B3  received: IDLE_STATE_REQUEST, set IDLE_MODE at own n. recv_conf_update_mvb
	     B4  received: SWITCHOVER_REQUEST.                         recv_conf_update_mvb
	     B5  received: SWITCHBACK_REQUEST.                         recv_conf_update_mvb
	     B6  received: MVB_NADI_REQUEST, MVB_GROUP_LIST_REQUEST.   recv_conf_update_mvb
	     B7  reply the request ok with call indicate...            recv_conf_update_mvb
	     B8  reply the request ok                                  recv_conf_update_mvb
	     B9  next am_rcv_requ, get ready to receive any tgm.       reply_conf_update_mvb
	     BA  ready for the next...                                 recv_conf_update_mvb
	     BC  order: default case                                   recv_conf_update_mvb
	     C3  error, status != AM_OK, internal (expected A3).       call_conf_update_mvb
	     D0  error status from am_bind_replier (expected B0).      map_update_mvb
	     DE  error status from am_rcv_requ any tgm. (expected B0). map_update_mvb
	     DF  error status from am_rcv_requ any tgm. (expected B0). map_update_mvb
	     D1  error, node not found in NADI (expected B1).          recv_conf_update_mvb
	     D7  reply the request not ok (expected B7 or B8).         recv_conf_update_mvb
	     D9  error by call am_rcv_requ, (expected B9).             reply_conf_update_mvb
	   */


   /*
      TABLE OF ERROR NUMBERS
    */

	  /*
	     see address: map_REC_ERR_CODE   Recorder error.code  8 words
	     map_REC_ERR_INFO   Recorder error.info  8 words

	     type COM960: rw E00xxxxx,10     were xxxxx is the address map_REC_ERR_CODE
	     MICAS:  dw ssss:ooool10    were ssss:oooo is the address      "      
	   */

	  /*
	     code   info       Meaning
	     ------------------------------------------------------------------------------------
	     '9101' am_status  MAP_ERR_SETUP_BR
	     error in map_setup_slave, am_bind_replier deliverd error status.
	     '9103' am_status  MAP_ERR_SETUP_REQ
	     error in map_setup_slave, am_rcv_requ to rcv. the VEH_DES_REQUEST
	     deliverd error status.
	     '9104' am_status  MAP_ERR_INCLUDE_NADI
	     error in call_conf_include, am_rcv_requ for receive the NADI after
	     include deliverd error status.
	     '9105' am_status  MAP_ERR_INCLUDE_GROUP_LIST
	     error in map_setup_slave, am_rcv_requ for receive the
	     VEH_DES_REQUEST deliverd error status.
	     '9200' nr. of     MAP_ERR_SETUP_MASTER_NR
	     rcv. resp. error in map_setup_master, not all slaves have responsed on
	     distributed NADI ( W T B ).
	     '9300' nr of      MAP_ERR_SETUP_VEHBUS_NR
	     rcv. resp. error in map_setup_vb, not all slaves have responsed on distributed
	     NADI ( W V B ).
	     '9401' am_status  MAP_ERR_CALL_CONFS_VEH
	     error in call_conf_setup, waiting for an VEHICLE DESCRIPTOR from any
	     of slaves, but am status not ok or wrong tgm code received.
	     '9402' am_status  MAP_ERR_CALL_CONFS_NADI
	     '9412' (MVB)      error in call_conf_setup, waiting on response for the distribution
	     of NADI from any of slaves, but am status not ok or wrong tgm code
	     received.
	     '9501' tgm code   MAP_ERR_RECV_CONFS_VEH
	     error in recv_conf_setup, veh. descr. request was expected, but
	     wrong tgm code was received.
	     '9502' tgm code   MAP_ERR_RECV_CONFS_NADI
	     error in recv_conf_setup, NADI was expected, but wrong tgm code was
	     received.
	     '9503' am_status  MAP_ERR_RECV_CONFS_REQ
	     error in reply_conf_setup, called function am_rcv_requ for receive
	     the NADI delivered error status.
	     '9601' am_status  MAP_ERR_UPDATE_BR
	     error in map_update, am_bind_replier deliverd error status.
	     '9602' am_status  MAP_ERR_UPDATE_RCV
	     error in map_update, calling am_rcv_requ for any tgm deliverd an
	     error status.
	     '9603' am_status  MAP_ERR_UPDATE_BR_MVB
	     error in map_update, am_bind_replier deliverd error status.
	     '9604' am_status  MAP_ERR_UPDATE_RCV_MVB
	     error in map_update, calling am_rcv_requ for any tgm deliverd an
	     error status.
	     '9701' am_status  MAP_ERR_CALL_CONFU
	     error in call_conf_update, any error status is delivered.
	     '9702' am_status  MAP_ERR_CALL_CONFU_MVB
	     error in call_conf_update, any error status is delivered.
	     '9803' am_status  MAP_ERR_RECV_CONFU_REQ
	     error in reply_conf_update, calling am_rcv_requ for any tgm
	     delivered an error status.
	     '9804' am_status  MAP_ERR_RECV_CONFU_REQ_MVB
	     error in reply_conf_update, calling am_rcv_requ for any tgm
	     delivered an error status.
	     '9901' err code   MAP_ERR_SEMA
	     pi_create  Cant get sema from PIL.
	     '9902' nr. of     MAP_ERR_TO
	     rcv. resp. Not all nodes have responsed after a sos-time
	     (AEG special. Each node should at least response by timeout)
	     '9999' 9999       MAP_ERR_TOPO
	     number of nodes in train topography = 0!!? Any programm causes an
	     error...
	   */



/*
   COMPILER SWITCHES
 */

/* <ATR:03> */
// #ifdef O_960    
// #pragma noalign    
// #endif

/*
   INCLUDEFILES
 */
#include "am_sys.h"				/* am@@@ messenger */
#include "pi_sys.h"				/* PIL interface */
#include "bmi.h"				/* TCN control interface */
#include "map.h"				/* includes for map modules and apps */
#include "map_ums.h"			/* includes for map modules */
#include "map_bmi.h"			/* includes for map_npt.c and map_bmi.c */
#include "recorder.h"

/*
   IMPLEMENTATION
 */

   /*
      ID-STRING
    */
char *p_map_ver = "Name=UIC Mapping Server-V:2.10A-D:30.11.95-C: measurement";


   /*
      DEFINES
    */
#define MAX_ORDER 6				/* Order from UI, WTB and NVB */
#define UI  66
#define WTB 77
#define MVB 88

#ifdef O_960

#define RECORD_M_ERR( p_msg ) ( \
              nse_record( "MAP_NPT", __LINE__, NSE_EVENT_TYPE_ERROR, \
                           NSE_ACTION_CONTINUE, ( strlen ( p_msg ) +1), \
                           ( UINT8 *) p_msg ) )

#define RECORD_M_EVENT( p_msg ) ( \
              nse_record( "MAP_NPT", __LINE__, NSE_EVENT_TYPE_SPECIAL, \
                           NSE_ACTION_CONTINUE, ( strlen ( p_msg ) +1), \
                           ( UINT8 *) p_msg ) )

#endif


   /*
      EXTERNALS
    */
extern map_ctrl_field map_CTRL;	/* in MAP_BMI.H */

	  /* externals are set in module mon_serv.c (aeg_fw directory) */
extern unsigned short MAP_EVENT_BUF;	/* read the event buffer, set != 0  */
extern unsigned short MAP_SHOW_NADI;	/* current NADI(x,y) = 1, org. NADI = 2 */
extern unsigned short MAP_TRACE_ON;		/* update trace on/off = 1/0 */


   /*
      GLOBAL DECLARATIONS
    */

	  /*
	     TRAIN DATABASE: NADI, GROUP_LIST
	   */
map_TrainTopography NADI;
map_NodeGroupList GROUP_LIST[MAP_MAX_NO_GRP];


	  /*
	     WORK DATABASES: XY-NADI, XY-GROUP_LIST
	   */
		 /* <ATR:08> */ extern map_TrainTopography map_xnadi_ui;
		 /* <ATR:08> */ extern map_TrainTopography map_ynadi_ui;
		 /* <ATR:08> */ extern map_NodeGroupList map_xgroup_list_ui[MAP_MAX_NO_GRP];
		 /* <ATR:08> */ extern map_NodeGroupList map_ygroup_list_ui[MAP_MAX_NO_GRP];


	  /*
	     TCN TOPOLOGY
	   */
unsigned char map_tcn_nodes_max;
unsigned char map_tcn_node_addr[MAP_MAX_NO_VEH];

s_CommInfo map_ci;				/* Interface between map_bmi and map_npt */

	  /* <ATR:08> */
extern s_vw map_vw;
s_vw *p_map_vw;

unsigned char map_numN, map_numE;	/* counter for replys on MVB */
short map_sema;					/* semaphore id */
short map_sos_time;				/* 1.08, 19.12.94, Test Mr. Bahrs */
short map_ui_delay;				/* actual delay UI */
short map_ui_delay_max;			/* delay to get ready UI */
unsigned char map_ndx;			/* global for(..) index */
unsigned char map_event_loop;	/* delay for display output */
unsigned char map_include_repeat;	/* repeat of include if am-error */
unsigned char map_update_init;	/* check flag if initialise is done */
unsigned char map_test_cnt;		/* only for DOS-Test */
unsigned char map_not_broadcast;	/* flag for setup */
char map_xbuf[80];				/* protocoll buffer for sprintf *//* <ATR:05> */

unsigned short map_MC_to_own_node;	/* set to 0xCCCC send MC to own node */

	  /*
	     TELEGRAMM BUFFERS
	   */
s_tgm_update map_upd_in_tgm_wtb[MAP_MAX_NODES];
s_tgm_update map_upd_in_tgm_mvb[MAP_MAX_NODES_MVB];
s_tgm_update map_upd_out_tgm, map_upd1_out_tgm;
s_tgm_nadi map_tgm_nadi;


	  /*
	     ORDER BUFFER
	   */
struct {
	unsigned char read, write;
	unsigned char type[MAX_ORDER];	/* UI, WTB or MVB */
	unsigned short order[MAX_ORDER];	/* to distinguish the data type */
	u_data data[MAX_ORDER];
} map_order;


	  /*
	     BUFFERS FOR RTP ERROR RESULTS
	   */
struct {
	unsigned char lla;
	unsigned char status;
} map_cc_setup[1 + MAP_MAX_NO_VEH],

  map_cc_update[1 + MAP_MAX_NO_VEH],
  map_cc1_update[1 + MAP_MAX_NO_MVB],
  map_cc_update_mc[1 + MAP_MAX_NO_VEH];



   /*
      PROTOTYPES
    */
char *map_get_version (void);

void read_event_buffer_to_term (void);
void map_show_nadi (void);

void map_stop (void);
char map_GetOrder (void);
char map_PutOrder (unsigned char, unsigned short, u_data *);

void map_reply_conf_setup (char, char *);
void map_reply_conf_update_MVB (char, unsigned short *);
void map_reply_conf_update (char, unsigned short *);

void map_recv_conf_update (char, const AM_NET_ADDR *, s_tgm_update *,
						   CARDINAL32, char *);
void map_recv_conf_setup (char, const AM_NET_ADDR *, s_tgm_nadi *,
						  CARDINAL32, char *);

void map_call_conf_setup (char, char *, const AM_NET_ADDR *,
						  s_tgm_vehdes *, CARDINAL32, unsigned char);
void map_call_conf_include (char, char *, const AM_NET_ADDR *,
							s_tgm_update *, CARDINAL32, unsigned char);
void map_call_conf_update_MVB (char, char *, const AM_NET_ADDR *,
							   s_tgm_vehdes *, CARDINAL32, unsigned char);
void map_call_conf_mc_update_WTB (char, void *, const AM_NET_ADDR *,
								  s_tgm_vehdes *, CARDINAL32, unsigned char);
void map_call_conf_update_WTB (char, char *, const AM_NET_ADDR *,
							   s_tgm_vehdes *, CARDINAL32, unsigned char);

void map_setup_slave (void);
void map_setup_master (void);
void map_include_slave (void);

char map_update_ini (void);
void map_update_WTB (void);
void map_update_MVB (void);
void map_update (void);


   /*
      FUNCTIONS
    */

	  /*
	     map_get_version
	   */

char *
map_get_version (void)
{
	return (p_map_ver);			/* returns the MAP version string */
}


	  /*
	     map_show_nadi
	   */

void 
map_show_nadi ()
{

#if defined ( TERM_OUT )


	/*
	   Writes NADI info to the terminal.
	 */
	char buf[80], i, j;
	map_TrainTopography *p_nadi;

	if (MAP_SHOW_NADI == 1) {

		if (p_map_vw->actual_nadi == 0) {
			p_nadi = &map_xnadi_ui;
			write_serv ("\n\n\rMAP: xNADI state:");
		} else {
			p_nadi = &map_ynadi_ui;
			write_serv ("\n\n\rMAP: yNADI state:");
		}
	} else {

		if (MAP_SHOW_NADI == 2) {
			p_nadi = &NADI;
			write_serv ("\n\n\rMAP: NADI state:");
		}
	}

	/*
	   NADI: STATE INFORMATION {validity, direction, rel/abs}
	 */

	for (i = 0; i < 80; i++) {
		buf[i] = 0;
	}
	sprintf (buf, "version = %u\t  validity  = '%2x'",
			 p_nadi->state.version, p_nadi->state.validity);
	write_serv (buf);

	for (i = 0; i < 80; i++) {
		buf[i] = 0;
	}
	sprintf (buf, "ownaddr = %u\t  direction = %u",
			 p_nadi->state.ownaddr, p_nadi->state.mdirect);
	write_serv (buf);

	for (i = 0; i < 80; i++) {
		buf[i] = 0;
	}
	sprintf (buf, "cnt.veh = %u\t  rel/abs   = %u",
			 p_nadi->state.VehicleNo, p_nadi->state.ScopeOfRangeNo);
	write_serv (buf);


	/*
	   NADI: NODE ADDRESSES
	 */

	for (i = 0; i < 80; i++) {
		buf[i] = 0;
	}
	write_serv ("\nMAP: NADI node addresses:");

	sprintf (buf, "%u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u", p_nadi->vehicle[0].NodeAddr, p_nadi->vehicle[1].NodeAddr,
			 p_nadi->vehicle[2].NodeAddr, p_nadi->vehicle[3].NodeAddr,
			 p_nadi->vehicle[4].NodeAddr, p_nadi->vehicle[5].NodeAddr,
			 p_nadi->vehicle[6].NodeAddr, p_nadi->vehicle[7].NodeAddr,
			 p_nadi->vehicle[8].NodeAddr, p_nadi->vehicle[9].NodeAddr,
			 p_nadi->vehicle[10].NodeAddr, p_nadi->vehicle[11].NodeAddr,
			 p_nadi->vehicle[12].NodeAddr, p_nadi->vehicle[13].NodeAddr,
			 p_nadi->vehicle[14].NodeAddr, p_nadi->vehicle[15].NodeAddr,
			 p_nadi->vehicle[16].NodeAddr, p_nadi->vehicle[17].NodeAddr,
			 p_nadi->vehicle[18].NodeAddr, p_nadi->vehicle[19].NodeAddr,
			 p_nadi->vehicle[20].NodeAddr, p_nadi->vehicle[21].NodeAddr);

	write_serv (buf);


	/*
	   NADI: VEHICLE ID
	 */
	write_serv ("\nMAP: NADI Vehicle Id:");

	for (i = 0; i < 80; i++) {
		buf[i] = 0x20;
	}
	buf[59] = 0;

	/*
	   ID 1..12
	 */

	for (j = 0; j < 12; j++) {

		for (i = 0; i < 4; i++) {

			if (p_nadi->vehicle[j].VehicleId[i] != 0) {
				buf[i + j * 5] = p_nadi->vehicle[j].VehicleId[i];
			}
		}
	}
	write_serv (buf);

	/*
	   ID 13..22
	 */

	for (i = 0; i < 80; i++) {
		buf[i] = 0x20;
	}
	buf[59] = 0;

	for (j = 0; j < 10; j++) {

		for (i = 0; i < 4; i++) {

			if (p_nadi->vehicle[j + 12].VehicleId[i] != 0) {
				buf[i + j * 5] = p_nadi->vehicle[j + 12].VehicleId[i];
			}
		}
	}
	write_serv (buf);
	write_serv ("\r");


#endif
}


	  /*
	     Functions: MAP_STOP, MAP_PUTORDER, MAP_GETORDER
	   */

		 /*
		    MAP_STOP {70, 71, 72}
		  */

void 
map_stop (void)
{

	/*
	   This function stops all activity in the current program phase.
	 */
	static AM_RESULT status;
	static AM_NET_ADDR replier;

	char i;

	replier.function = MAP_FUNCTION_NR;
	replier.device = AM_UNKNOWN_DEVICE;

	if (p_map_vw->phase == UPDATE_MODE) {

		/*
		   STOP DURING UPDATE
		 */

		for (i = 0; i < map_tcn_nodes_max; i++) {
			/* all caller instances on WTB */
			replier.vehicle = map_tcn_node_addr[i];
			am_call_cancel (MAP_FUNCTION_NR, &replier, &status);
		}

		for (i = 0; i < map_tcn_nodes_max; i++) {
			/* all replier instances on WTB */
			am_rcv_cancel (MAP_FUNCTION_NR, &p_map_vw->ext_u_wtb[i], &status);
		}
		am_unbind_replier (MAP_FUNCTION_NR);

		for (i = 0; i < map_ci.NodeKonf->device_list.number; i++) {
			/* all caller instances on MVB */
			replier = map_ci.NodeKonf->device_list.device[i];

			if (replier.device != 0) {
				am_call_cancel (MAP_FUNCTION_NR, &replier, &status);
			}
		}

		if (p_map_vw->f.confirm != 0) {

			/*
			   HANDLE USER CALL CONFIRM FUNCTION
			 */

			if (p_map_vw->old_order == MULTICAST) {
				p_map_vw->f.multicast (p_map_vw->mc_caller,
									   p_map_vw->r_confirm,
									   &p_map_vw->mc_replier,
									   MAP_NOT_OK);
			} else {
				p_map_vw->f.confirm (
										p_map_vw->r_confirm,
										MAP_NOT_OK);
			}
			/* disable the confirm function */
			p_map_vw->f.confirm = 0;
			p_map_vw->r_confirm = 0;

		}
		/*
		   TERM_OUT
		 */

#if defined ( TERM_OUT )


		if (MAP_TRACE_ON) {
			write_serv ("^MAP-STOP during update^");
		}
#endif

		recorder (0x70);

	} else {

		if (p_map_vw->phase == SETUP_MODE) {

			/*
			   STOP DURING SETUP
			 */

			if (map_ci.bmi_type != L_SLAVE) {

				/*
				   MASTER
				 */

				for (i = 0; i < map_tcn_nodes_max; i++) {
					/* Cancel all not received vehicle description requests on WTB. */
					replier.vehicle = map_tcn_node_addr[i];
					am_call_cancel (MAP_FUNCTION_NR, &replier, &status);
				}
				recorder (0x71);

			} else {

				/*
				   SLAVE
				 */
				/* Cancels a non responsed reply confirm */
				am_rcv_cancel (MAP_FUNCTION_NR, &p_map_vw->ext_c_wtb[0], &status);
				am_rcv_cancel (MAP_FUNCTION_NR, &p_map_vw->ext_c_wtb[1], &status);
				am_rcv_cancel (MAP_FUNCTION_NR, &p_map_vw->ext_c_wtb[2], &status);
				am_unbind_replier (MAP_FUNCTION_NR);

				recorder (0x72);

			}

			/*
			   TERM_OUT
			 */

#if defined ( TERM_OUT )

			write_serv ("^MAP: STOP during setup.^");

#endif


		}
	}
	NADI.state.validity = MAP_NADI_INVALID;
	NADI.state.mdirect = MAP_DIREC_UNKNOWN;

	map_xnadi_ui = NADI;
	map_ynadi_ui = NADI;

	p_map_vw->code = MAP_OK;
	p_map_vw->info = 0;

	p_map_vw->phase = IDLE;
	p_map_vw->upd_status = MAP_UPDATE_NOT_READY;

	return;
}


		 /*
		    MAP_PUTORDER {75, 7D, 7E, 7F}
		  */

char 
map_PutOrder (unsigned char type, unsigned short order, u_data * data_addr)
{

	/*
	   A received order by the user interface, MVB or WTB by tgm data from 
	   other nodes is stored for further work through, if this node is a   
	   GATEWAY. If no buffer empty, an error message is reported.          
	   --------------------------------------------------------------------
	   type      = UI, WTB or MVB.                                         
	   code      = tgm code to identify the data structure.                
	   *tgm_addr = received data from MVB or WTB.                          
	 */
	char q, i;

	i = map_order.write;
	q = FALSE;

	if ((order == 0) || (order > 0xFAFF)) {

		/*
		   ILLEGAL ORDER
		 */

#ifdef O_960

		RECORD_M_EVENT ("illegal ORDER received, ignore! ");

#endif
		recorder (0x7E);

	} else {

		if ((type == UI) &&
			(p_map_vw->upd_status == MAP_UPDATE_AT_WORK)) {

			/*
			   UI CALL REJECTED
			 */

#ifdef O_960

			RECORD_M_EVENT ("UI call rejected, another works!");

#endif
			recorder (0x7D);

		} else {

			if (map_order.order[i] != NEXT_ORDER) {

				/*
				   NO EMPTY ELEMENT
				 */

#ifdef O_960


				if (type == MVB) {
					RECORD_M_EVENT ("MVB order rejected, no buffer!");
				} else {
					RECORD_M_EVENT ("WTB order rejected, no buffer!");
				}

#endif
				recorder (0x7F);

			} else {

				/*
				   EMPTY ELEMENT
				 */
				map_order.type[i] = type;
				map_order.order[i] = order;

				if (order == DYN_ATTR_REQUEST) {
					map_order.data[i].vehicle = data_addr->vehicle;
				} else {

					if ((order == GROUP_LIST_REQUEST) ||
						(order == MVB_GROUP_LIST_INI_REQU)) {
						map_order.data[i].group = data_addr->group;
					}
				}

				if (++i >= MAX_ORDER) {
					i = 0;
				}
				map_order.write = i;

				q = TRUE;

				recorder (0x75);

			}
		}
	}
	return q;
}


		 /*
		    MAP_GETORDER {76}
		  */

char 
map_GetOrder ()
{

	/*
	   The last order from the queue is reading and stored into the actual
	   -------------------------------------------------------------------
	   return code = UI, WTB or MVB. FALSE = no order.
	 */
	char q, i, j;

	i = map_order.read;
	q = FALSE;

	for (j = 0; j < MAX_ORDER; j++) {

		if ((map_order.order[i] != NEXT_ORDER) &&
			(map_order.order[i] != INIT_ORDER)) {

			/*
			   FOUND NEXT ORDER
			 */
			p_map_vw->order = map_order.order[i];
			q = map_order.type[i];

			if (map_order.order[i] == DYN_ATTR_REQUEST) {
				p_map_vw->upd.vehicle = map_order.data[i].vehicle;
			} else {

				if ((map_order.order[i] == GROUP_LIST_REQUEST) ||
					(map_order.order[i] == MVB_GROUP_LIST_INI_REQU)) {
					p_map_vw->upd.group = map_order.data[i].group;
				}
			}
			map_order.order[i] = NEXT_ORDER;	/* free element */

			if (++i >= MAX_ORDER) {
				i = 0;
			}
			map_order.read = i;

			recorder (0x76);

			break;
		} else {

			if (++i >= MAX_ORDER) {
				i = 0;
			}
		}
	}
	return q;
}



	  /*
	     Functions: REPLY CONFIRMs
	   */

		 /*
		    MAP_REPLY_CONFIRM_SETUP {24, 26}
		  */

void 
map_reply_conf_setup (char replier_function, char *ext_ref)
{

	/*
	   Internal receiver for reply from TCN level.
	 */

	if ((*ext_ref == RECV_VEH_DES_REQUEST) || (*ext_ref == RECV_NADI)) {
		/* VEH_DES response ok. */
		recorder (0x24);
	} else {
		/* NADI response ok, end of inauguration. */
		p_map_vw->function_step = END_INAUGURATION;
		recorder (0x26);
	}
	return;
}


		 /*
		    MAP_REPLY_CONFIRM_UPDATE_MVB {B9, D9}
		  */

void 
map_reply_conf_update_MVB (char replier, unsigned short *ext_ref)
{

	/*
	   Internal receiver for reply from TCN level during UPDATE mode (from MVB).
	 */
	CARDINAL32 in_size;
	AM_RESULT status;

	/* prepare the  n e x t   R E C E I V E */
	in_size = SIZE_VH_DESCR + 5;
	am_rcv_requ (replier, &map_upd_in_tgm_mvb[*ext_ref], in_size, ext_ref, &status);

	if ((status != AM_OK) || (status != MAP_OK)) {
		p_map_vw->code = MAP_ERR_RECV_CONFU_REQ_MVB;
		p_map_vw->info = status;

		err_rec ();

		recorder (0xD9);
	}
	recorder (0xB9);

	return;
}


		 /*
		    MAP_REPLY_CONF_UPDATE {6A, 9A}
		  */

void 
map_reply_conf_update (char replier, unsigned short *ext_ref)
{

	/*
	   Internal receiver for reply from TCN level during UPDATE mode.
	 */
	CARDINAL32 in_size;
	AM_RESULT status;

	/* prepare the  n e x t   r e c e i v e */
	in_size = SIZE_VH_DESCR + 5;
	am_rcv_requ
		(replier, &map_upd_in_tgm_wtb[*ext_ref], in_size, ext_ref, &status);

	if ((status != AM_OK) || (status != MAP_OK)) {
		p_map_vw->code = MAP_ERR_RECV_CONFU_REQ;
		p_map_vw->info = status;

		err_rec ();

		recorder (0x9A);
	} else {
		recorder (0x6A);
	}
	return;
}



	  /*
	     Functions: RECEIVE CONFIRMs
	   */

		 /*
		    MAP_RECV_CONV_SETUP {23, 25, 43, 44, 46, 4A}
		  */

void 
map_recv_conf_setup (char replier_function,
					 const AM_NET_ADDR * caller,
					 s_tgm_nadi * in_msg_adr,
					 CARDINAL32 in_msg_size,
					 char *ext_ref)
{

	/*
	   DESCRIPTION {---SLAVE NODE---}
	 */

	/*
	   a) receive vehicle description request from master.
	   It sends the own vehicle descriptor back to the requestor (master).
	   b) receive the NADI request from master.
	   It sends the reply of receiving NADI back to the requestor (master).
	   The switch into the next work phase is realized by using the
	   reply_conf function.
	   c) receive the broadcast from master.
	   ------------------------------------------------------------------------
	   Installd by am_bind_replier, called by MAP_SETUP_SLAVE.
	 */


	/*
	   DATA DECLARATION
	 */
	char i, j;
	unsigned short ui_work;

	static AM_RESULT status;
	static CARDINAL32 out_size, nadi_size, in_size;
	static s_tgm_vehdes out_tgm;	/* vehicle descriptor */

	/* <ATR:13> */
	if (caller->vehicle == AM_SAME_VEHICLE) {
		am_reply_requ (replier_function, 0, 0, ext_ref, AM_FAILURE);
		return;
	}

	ui_work = in_msg_adr->id.code;

	if (*ext_ref == RECV_VEH_DES_REQUEST) {

		/*
		   SEND VEH_DES
		 */

		if (ui_work == ATTR_LIST_REQUEST) {
			/* reply with own vehicle description into e-telegram. */
			out_tgm.vehicle = *map_ci.p_VD;
			/* complete with TCN type. */
			out_tgm.vehicle.TCNType = map_ci.bmi_type;

			out_tgm.id.key = 0;
			out_tgm.id.owner = 0;
			out_tgm.id.code = ATTR_LIST_RESPONSE;
			out_tgm.format = 0;
			out_size = SIZE_VH_DESCR + 5;

			/* V E H _ D E S  ---> MASTER */
			am_reply_requ (replier_function, &out_tgm, out_size, ext_ref, AM_OK);

			/*
			   PREPARE NADI RECEIVE
			 */
			nadi_size = map_tcn_nodes_max * SIZE_VH_DESCR + SIZE_NN_STATE + 5;
			p_map_vw->ext_c_wtb[1] = RECV_NADI;

			am_rcv_requ (replier_function, &map_tgm_nadi, nadi_size,
						 &p_map_vw->ext_c_wtb[1], &status);

			if (status == AM_OK) {
				p_map_vw->function_step = WAIT;

				recorder (0x23);	/* SETUP  */
			} else {

				/*
				   RTP ERROR
				 */
				p_map_vw->code = MAP_ERR_RECV_CONFS_REQ;
				p_map_vw->info = status;
				err_rec ();

				p_map_vw->function_step = ERR_INAUGURATION;

				recorder (0x44);	/* SETUP  */

			}
		} else {

			/*
			   ERROR HANDLING
			 */
			p_map_vw->code = MAP_ERR_RECV_CONFS_VEH;
			p_map_vw->info = ui_work;
			err_rec ();

			p_map_vw->function_step = ERR_INAUGURATION;

			recorder (0x43);

			/* <ATR:14> */
			am_rcv_cancel (MAP_FUNCTION_NR, &p_map_vw->ext_c_wtb[0], &status);
			am_rcv_cancel (MAP_FUNCTION_NR, &p_map_vw->ext_c_wtb[1], &status);
			am_rcv_cancel (MAP_FUNCTION_NR, &p_map_vw->ext_c_wtb[2], &status);
			am_unbind_replier (MAP_FUNCTION_NR);
		}

	} else {

		if (*ext_ref == RECV_NADI) {

			/*
			   SEND NADI RESPONSE
			 */

			if (ui_work == ATTR_DIR_REQUEST) {
				/* enter received NADI from e-telegram into NADI */
				NADI = in_msg_adr->NADI;

				/* <ATR:15> */
				if (!NADI.state.validity)
					NADI.state.validity = MAP_NADI_VALID;

				out_tgm.id.key = 0;
				out_tgm.id.owner = 0;
				out_tgm.id.code = ATTR_DIR_RESPONSE;
				out_tgm.format = 0;
				out_size = 4;

				/* N A D I  response  ---> MASTER */
				am_reply_requ
					(replier_function, &out_tgm, out_size, ext_ref, AM_OK);

				/*
				   PREPARE BROADCAST RECEIVE
				 */
				in_size = 6;
				p_map_vw->ext_c_wtb[2] = INAUG_READY;

				am_rcv_requ (replier_function, &map_tgm_nadi, in_size,
							 &p_map_vw->ext_c_wtb[2], &status);

				if (status == AM_OK) {
					/* <ATR:17> */
					// p_map_vw->function_step = WAIT_BROADCAST;
					p_map_vw->function_step = WAIT;

					recorder (0x25);
				} else {

					/*
					   RTP ERROR
					 */
					p_map_vw->code = MAP_ERR_RECV_CONFS_REQ;
					p_map_vw->info = status;
					err_rec ();

					p_map_vw->function_step = ERR_INAUGURATION;

					recorder (0x4A);

				}
			} else {

				/*
				   ERROR HANDLING
				 */
				p_map_vw->code = MAP_ERR_RECV_CONFS_NADI;
				p_map_vw->info = ui_work;
				err_rec ();

				p_map_vw->function_step = ERR_INAUGURATION;

				recorder (0x46);

				/* <ATR:14> */
				am_rcv_cancel (MAP_FUNCTION_NR, &p_map_vw->ext_c_wtb[0], &status);
				am_rcv_cancel (MAP_FUNCTION_NR, &p_map_vw->ext_c_wtb[1], &status);
				am_rcv_cancel (MAP_FUNCTION_NR, &p_map_vw->ext_c_wtb[2], &status);
				am_unbind_replier (MAP_FUNCTION_NR);
			}

		} else {

			if (*ext_ref == INAUG_READY) {

				/*
				   BROADCAST
				 */
				out_tgm.id.key = 0;
				out_tgm.id.owner = 0;
				out_tgm.id.code = INAUG_READY_RESP;
				out_tgm.format = 0;
				out_size = 4;

				am_reply_requ (replier_function, &out_tgm, out_size, ext_ref, AM_OK);

				/* shortening the remaining delay time */
				map_ui_delay = map_ui_delay_max - 3;

			}
		}
	}
	return;
}


		 /*
		    MAP_RECV_CONF_UPDATE {61-67, 85, 86, 91, 97, B1, B6}
		  */

void 
map_recv_conf_update (char replier_function,
					  const AM_NET_ADDR * caller,
					  s_tgm_update * in_msg_adr,
					  CARDINAL32 in_msg_size,
					  char *ext_ref)
{

	/*
	   DESCRIPTION {---MASTER and SLAVE---}
	 */

	/*
	   This is the recv_confirm function for  u p d a t e  mode.
	   ----------------------------------------------------------
	   Installd by am_bind_replier, called by map_update.
	 */


	/*
	   DATA DECLARATION
	 */
	char i, j, k;
	short err;
	unsigned short ui_work;
	unsigned char uc_work, type;
	AM_RESULT status;
	CARDINAL32 rr_size;
	static s_tgm_def rr_tgm;	/* vehicle descriptor */

	ui_work = in_msg_adr->id.code;
	rr_size = 6;
	type = 0;					/* indication type */

	/*
	   PREPARE THE REPLY {61-66,85,86,91,97,B1,B6}
	 */

	switch (ui_work) {

		/*
		   M V B  INITIALISATION {B6}
		 */

	case MVB_NADI_INI_REQU:
		{

			/*
			   NADI DISTRIBUTION TO MVB REQUESTED
			 */
			map_PutOrder (MVB, in_msg_adr->id.code, &in_msg_adr->content);

			recorder (0xB6);

			break;
		}

	case MVB_GROUP_LIST_INI_REQU:
		{

			/*
			   GROUP_LIST DISTRIBUTION TO MVB REQUESTED
			 */
			k = 0;

			for (j = 0; j < MAP_MAX_NO_GRP; j++) {

				for (i = 0; i < GROUP_LIST[j].length; i++) {
					in_msg_adr->content.group.member[k].node =
						GROUP_LIST[j].node[i];
					in_msg_adr->content.group.member[k].group = j + 1;
					k++;
				}
			}
			in_msg_adr->content.group.length = k;

			map_PutOrder (MVB, in_msg_adr->id.code, &in_msg_adr->content);

			recorder (0xB6);

			break;
		}


		/*
		   NODE RECOVERY {85, 86}
		 */

	case I_NADI_REQUEST:
		{

			/*
			   SEND NADI TO INCLUDED NODE {85}
			 */
			map_tgm_nadi.id.key = 0;
			map_tgm_nadi.id.code = I_NADI_RESPONSE;
			map_tgm_nadi.id.owner = 0;

			map_tgm_nadi.format = 0;

			map_tgm_nadi.NADI = NADI;
			rr_size = map_tcn_nodes_max * SIZE_VH_DESCR + SIZE_NN_STATE + 5;

			am_reply_requ (replier_function, &map_tgm_nadi, rr_size, ext_ref, MAP_OK);
			rr_size = 1;		/* no standard reply */

			recorder (0x85);

			break;

		}

	case I_GROUP_LIST_REQUEST:
		{

			/*
			   SEND GROUP LIST TO INCLUDED NODE {86}
			 */
			k = 0;

			for (j = 0; j < MAP_MAX_NO_GRP; j++) {

				for (i = 0; i < GROUP_LIST[j].length; i++) {
					map_upd_out_tgm.content.group.member[k].node = GROUP_LIST[j].node[i];
					map_upd_out_tgm.content.group.member[k].group = j + 1;

					k++;
				}
			}
			map_upd_out_tgm.content.group.length = k;

			map_upd_out_tgm.id.key = 0;
			map_upd_out_tgm.id.code = I_GROUP_LIST_RESPONSE;
			map_upd_out_tgm.id.owner = 0;

			map_upd_out_tgm.format = 0;

			rr_size = MAP_MAX_NO_VGL * 2 + 2 + 5;

			am_reply_requ (replier_function, &map_upd_out_tgm, rr_size, ext_ref, MAP_OK);
			rr_size = 1;

			recorder (0x86);

			break;

		}


		/*
		   SWITCHOVER, SWITCHBACK {64,65}
		 */

	case SWITCHOVER_REQUEST:
		{
			type = MAP_SWITCHOVER;
			rr_tgm.code = SWITCHOVER_RESPONSE;

			/*
			   TERM_OUT
			 */

#if defined ( TERM_OUT )


			if (MAP_TRACE_ON) {
				write_serv ("^MAP: switch_over received^");
			}
#endif

			map_PutOrder (WTB, in_msg_adr->id.code, &in_msg_adr->content);

			recorder (0x64);

			break;
		}

	case SWITCHBACK_REQUEST:
		{
			type = MAP_SWITCHBACK;
			rr_tgm.code = SWITCHBACK_RESPONSE;

			/*
			   TERM_OUT
			 */

#if defined ( TERM_OUT )


			if (MAP_TRACE_ON) {
				write_serv ("^MAP: switch_back received^");
			}
#endif

			map_PutOrder (WTB, in_msg_adr->id.code, &in_msg_adr->content);

			recorder (0x65);

			break;
		}


	case IDLE_STATE_REQUEST:
		{

			/*
			   PREPARE TGM {63}
			 */

			if (caller->vehicle != AM_SAME_VEHICLE) {

				/*
				   TERM_OUT
				 */

#if defined ( TERM_OUT )


				if (MAP_TRACE_ON) {
					write_serv ("^MAP: idle state received^");
				}
#endif

				type = MAP_IDLE_REQUEST;
				rr_tgm.code = IDLE_STATE_RESPONSE;

				map_PutOrder (WTB, in_msg_adr->id.code, &in_msg_adr->content);

				recorder (0x63);
			} else {

				/*
				   MVB ORDER
				 */
				/* from MVB, store order to distribute on WTB and MVB */
				map_PutOrder (MVB, in_msg_adr->id.code, &in_msg_adr->content);

				recorder (0xB3);

			}
			break;

		}

	case DYN_ATTR_REQUEST:
		{

			/*
			   REPLACE RECEIVED VEHICLE DESCRIPTOR WITHIN NADI {61,91,B1}
			 */

			if (caller->vehicle != AM_SAME_VEHICLE) {

				/*
				   Input the received veh. descriptor into the related NADI member.
				 */
				uc_work = in_msg_adr->content.vehicle.NodeAddr;		/* sender addr. */
				j = FALSE;		/* found flag */

#ifdef O_960

				pi_pend_semaphore (map_sema, PI_FOREVER, &err);

#endif

/* <ATR:10> */
				for (i = 0; i < map_tcn_nodes_max; i++)
//                            for ( i=0; i<map_tcn_nodes_max; i++ )
				{

					if (NADI.vehicle[i].NodeAddr == uc_work) {

						/*
						   STORE VEHICLE DESCRIPTOR -> NADI
						 */

						/*
						   node member found within NADI, replace content.
						 */
						NADI.vehicle[i] = in_msg_adr->content.vehicle;
						j = TRUE;	/* found flag */

						build_vrno ();	/* New range numbers are required */
						NADI.state.version++;	/* increment version */

						/*
						   set the  u s e r  NADI to perform data consistence
						 */

						if (p_map_vw->actual_nadi == 0) {
							map_ynadi_ui = NADI;
							p_map_vw->actual_nadi = 1;
						} else {
							map_xnadi_ui = NADI;
							p_map_vw->actual_nadi = 0;
						}

						/*
						   TERM_OUT
						 */

#ifdef TERM_OUT


						if (MAP_TRACE_ON) {

							for (map_ndx = 0; map_ndx < 80; map_ndx++) {
								map_xbuf[map_ndx] = 0;
							}
							sprintf (map_xbuf, "^MAP: dyn attr rcv, veh='%2X'^", uc_work);

							write_serv (map_xbuf);
						}
#endif

						break;

					}
				}

#ifdef O_960

				pi_post_semaphore (map_sema, &err);

#endif

				if (j) {

					/*
					   FINISH STORE ACTIONS
					 */
					type = MAP_NADI_UPDATED;
					rr_tgm.code = DYN_ATTR_RESPONSE;

					map_PutOrder (WTB, in_msg_adr->id.code, &in_msg_adr->content);

					if (map_CTRL.map_reportF != 0) {
						map_CTRL.map_reportF (MAP_NADI_UPDATE_RECEIVED);
					}
					recorder (0x61);

				} else {
					/* Node not found in NADI */
					rr_size = 0;
					recorder (0x91);
				}
			} else {

				/*
				   MVB ORDER
				 */
				/* from MVB, store order to distribute on WTB and MVB */
				map_PutOrder (MVB, in_msg_adr->id.code, &in_msg_adr->content);

				recorder (0xB1);

			}
			break;

		}

	case GROUP_LIST_REQUEST:
		{

			/*
			   REPLACE GROUP LIST WITH NEW GROUP INFO {62,B2}
			 */

			if (caller->vehicle != AM_SAME_VEHICLE) {

				/*
				   TERM_OUT
				 */

#if defined ( TERM_OUT )


				if (MAP_TRACE_ON) {
					write_serv ("^MAP: group list received^");
				}
#endif


#ifdef O_960

				pi_pend_semaphore (map_sema, PI_FOREVER, &err);

#endif

				/*
				   REPLACE GROUP LIST WITH TGM CONTENT
				 */

				/*
				   Reset the GROUP_LIST.
				 */

				for (i = 0; i < MAP_MAX_NO_GRP; i++) {
					GROUP_LIST[i].length = 0;

					for (j = 0; j < MAP_MAX_NO_NGL; j++) {
						GROUP_LIST[i].node[j] = 0;
					}
				}

				for (j = 0; j < in_msg_adr->content.group.length; j++) {
					/* group number is index onto GROUP_LIST */
					i = in_msg_adr->content.group.member[j].group - 1;

					if (i <= MAP_MAX_NO_GRP) {
						ui_work = in_msg_adr->content.group.member[j].node;
						k = GROUP_LIST[i].length;

						GROUP_LIST[i].node[k] = ui_work;
						GROUP_LIST[i].length++;
					}
					/*
					   TERM_OUT
					 */

#if defined ( TERM_OUT )


					if (MAP_TRACE_ON) {

						for (k = 0; k < 80; k++) {
							map_xbuf[k] = 0;
						}
						sprintf (map_xbuf, "\t^group = %3d,\tnode = '%2X'^", i + 1, ui_work);

						write_serv (map_xbuf);
					}
#endif

				}

				/*
				   set the  u s e r  GROUP LIST to perform data consistence
				 */

				if (p_map_vw->actual_group_list == 0) {

					for (i = 0; i < MAP_MAX_NO_GRP; i++) {
						map_ygroup_list_ui[i] = GROUP_LIST[i];
					}
					p_map_vw->actual_group_list = 1;
				} else {

					for (i = 0; i < MAP_MAX_NO_GRP; i++) {
						map_xgroup_list_ui[i] = GROUP_LIST[i];
					}
					p_map_vw->actual_group_list = 0;
				}


#ifdef O_960

				pi_post_semaphore (map_sema, &err);

#endif

				if (map_CTRL.map_reportF != 0) {
					map_CTRL.map_reportF (MAP_NADI_UPDATE_RECEIVED);
				}
				type = MAP_NADI_UPDATED;
				rr_tgm.code = GROUP_LIST_RESPONSE;

				map_PutOrder (WTB, in_msg_adr->id.code, &in_msg_adr->content);

				recorder (0x62);
			} else {

				/*
				   MVB ORDER
				 */
				/* from MVB, store order to distribute on WTB and MVB */
				map_PutOrder (MVB, in_msg_adr->id.code, &in_msg_adr->content);

				recorder (0xB2);

			}
			break;

		}

	case MULTICAST:
		{

			/*
			   TEST FOR MULTICAST {66}
			 */

			/*
			   This case is normally impossible, because the MC tgm is  d i r e c t l y
			   send to the applictaion task ands this task performs the response to the
			   requestor map_npt.
			   But it is reasonable to give a test feature.
			 */

			/*
			   TERM_OUT
			 */

#if defined ( TERM_OUT )


			if (MAP_TRACE_ON) {
				write_serv ("^MAP: multicast received^");	/* for test only */
			}
#endif

			recorder (0x66);

			break;

		}
	default:{

			/*
			   WRONG TGM {97}
			 */
			rr_size = 4;
			rr_tgm.code = 0x9999;

			am_reply_requ (replier_function, &rr_tgm, rr_size, ext_ref, MAP_NP);

			/*
			   TERM_OUT
			 */

#ifdef TERM_OUT


			if (MAP_TRACE_ON) {
				write_serv ("^MAP: invalid tgm received^");		/* for test only */
			}
#endif

			recorder (0x97);

			break;

		}
	}


	if (rr_size == 6) {
		am_reply_requ (replier_function, &rr_tgm, rr_size, ext_ref, MAP_OK);

		if (in_msg_adr->id.code == IDLE_STATE_REQUEST) {
			map_stop ();
		}
		if (type != 0 && p_map_vw->f_indication != 0) {

			/*
			   Inform the caller about the network event.
			 */
			p_map_vw->f_indication (p_map_vw->r_indication, type, MAP_OK);

			recorder (0x67);
		} else {
			recorder (0x68);
		}
	}
	return;
}



	  /*
	     Functions: CALL CONFIRMs
	   */

		 /*
		    MAP_CALL_CONF_INCLUDE {1C, 1D, 1E}
		  */

void 
map_call_conf_include (char caller_function,
					   char *ext_ref,
					   const AM_NET_ADDR * replier,
					   s_tgm_update * in_msg_adr,
					   CARDINAL32 in_msg_size,
					   AM_RESULT status)
{

	/*
	   --- S L A V E ---
	   This is the call_confirm function for:
	   a) receive the NADI from MASTER after an include.
	   b) receive the GROUP_LIST from the MASTER after an include.
	   Installd by am_call_requ, called by map_include_slave().
	 */
	unsigned char i, j, k;
	unsigned short ui_work;

	/* Counter of nodes wich have responsed */
	p_map_vw->numN++;

	ui_work = in_msg_adr->id.code;

	if (status == AM_OK) {

		if (ui_work == I_NADI_RESPONSE) {

			/*
			   a) NADI received
			 */
			/* enter content from e-telegram into the own NADI. */
			NADI = ((s_tgm_nadi *) (in_msg_adr))->NADI;

			/* local node info */
			NADI.state.ownaddr = map_ci.p_VD->NodeAddr;
			NADI.state.mdirect = map_ci.NODEstate.node_orient;

			recorder (0x1C);

		} else {

			/*
			   b) GROUP_LIST received
			 */

			for (j = 0; j < in_msg_adr->content.group.length; j++) {
				/* group number is index onto GROUP_LIST */
				i = in_msg_adr->content.group.member[j].group - 1;

				if (i <= MAP_MAX_NO_GRP) {
					ui_work = in_msg_adr->content.group.member[j].node;
					k = GROUP_LIST[i].length;
					GROUP_LIST[i].node[k] = ui_work;

					GROUP_LIST[i].length++;
				}
			}
			recorder (0x1D);

		}
	} else {

		/*
		   RTP ERROR
		 */
		p_map_vw->numE++;		/* error counter */

		if (ui_work == I_NADI_RESPONSE) {
			p_map_vw->code = MAP_ERR_INCLUDE_NADI;
		} else {
			p_map_vw->code = MAP_ERR_INCLUDE_GROUP_LIST;
		}
		p_map_vw->info = status;
		err_rec ();

		recorder (0x1E);

	}
	return;
}


		 /*
		    MAP_CALL_CONF_SETUP {12, 15, 17, 42, 45}
		  */

void 
map_call_conf_setup (char caller_function,
					 char *ext_ref,
					 const AM_NET_ADDR * replier,
					 s_tgm_vehdes * in_msg_adr,
					 CARDINAL32 in_msg_size,
					 AM_RESULT status)
{

	/*
	   --- M A S T E R ---
	   This is the call_confirm function for:
	   a) receive vehicle description from all other train bus members
	   and put them into the NADI.
	   b) receive the responses from all other train bus members if they
	   have received the delivered NADI.
	   c) receive the broadcast responses.
	   Installd by am_call_requ, called by MAP_SETUP_MASTER.
	 */
	char i, j;

	/* Counter of nodes wich have responsed */
	p_map_vw->numN++;

	if (status == AM_OK) {

		if (in_msg_adr->id.code == ATTR_LIST_RESPONSE) {

			/*
			   a) VEHICLE DESCRIPTOR
			 */
			/* enter content from e-telegram into NADI member */
			NADI.vehicle[*ext_ref] = in_msg_adr->vehicle;

			recorder (0x12);

		} else {

			if (in_msg_adr->id.code ==
				ATTR_DIR_RESPONSE) {

				/*
				   b) NADI RESPONSE
				 */
				recorder (0x15);	/* SETUP  */

			} else {

				/*
				   c) BROADCAST
				 */
				recorder (0x17);	/* SETUP  */

			}
		}
	} else {

		/*
		   RTP ERROR
		 */

		if (map_not_broadcast) {
			p_map_vw->numE++;	/* error counter */

			if (in_msg_adr->id.code == ATTR_LIST_RESPONSE) {
				p_map_vw->code = MAP_ERR_CALL_CONFS_VEH;

				i = *ext_ref;

				recorder (0x42);
			} else {
				p_map_vw->code = MAP_ERR_CALL_CONFS_NADI;

				i = *ext_ref - MAP_MAX_NO_VEH;

				recorder (0x45);
			}
			p_map_vw->info = status;
			err_rec ();

			p_map_vw->function_step = ERR_INAUGURATION;

			/* record the missing or wrong answered nodes */
			j = ++map_cc_setup[0].lla;
			map_cc_setup[j].lla = map_tcn_node_addr[i];
			map_cc_setup[j].status = status;
		}
	}
	return;
}


		 /*
		    MAP_CALL_CONF_UPDATE_MVB {A3, A4,A5, C3}
		  */

void 
map_call_conf_update_MVB (char caller_function,
						  char *ext_ref,
						  const AM_NET_ADDR * replier,
						  s_tgm_vehdes * in_msg_adr,
						  CARDINAL32 in_msg_size,
						  AM_RESULT status)
{

	/*
	   --- M A S T E R / S L A V E --- 
	   This is the call_confirm function for u p d a t e  mode.
	   Installd by am_call_requ, called by map_update_MVB.
	 */
	char i;

	map_numN++;					/* count of nodes */

	if (status == AM_OK) {
		recorder (0xA3);

		if (*ext_ref < MAP_MAX_NO_MVB) {

			if (map_numN >= map_ci.NodeKonf->device_list.number) {
				p_map_vw->order = NEXT_ORDER;
				p_map_vw->code = MAP_OK;
				p_map_vw->info = 0;

				if (map_numE == 0) {
					recorder (0xA5);
				} else {
					recorder (0xA4);
				}
			}
		}
	} else {

		/*
		   RTP ERROR
		 */
		map_numE++;				/* error counter */

		p_map_vw->code = MAP_ERR_CALL_CONFU_MVB;
		p_map_vw->info = status;
		err_rec ();

		/* record the missing or wrong answered nodes */
		i = ++map_cc1_update[0].lla;
		map_cc1_update[i].lla = map_ci.NodeKonf->device_list.device[*ext_ref].device;
		map_cc1_update[i].status = status;

		recorder (0xC3);

	}
	return;
}


		 /*
		    MAP_CALL_CONF_MC_UPDATE_WTB {53, 82, 84}
		  */

void 
map_call_conf_mc_update_WTB (char caller_function,
							 void *ext_ref,
							 const AM_NET_ADDR * replier,
							 s_tgm_vehdes * in_msg_adr,
							 CARDINAL32 in_msg_size,
							 AM_RESULT status)
{

	/*
	   call_confirm function for MULTICAST.
	   Installd by am_call_requ, called by map_update_WTB.
	   ---------------------------------------------------
	   ext_ref = node number (lla).
	 */
	unsigned char i;

	/* Counter of nodes wich have responsed */
	p_map_vw->numN++;

	if (status == AM_OK) {
		recorder (0x53);
	} else {

		/*
		   RTP ERROR
		 */

		if ((status == AM_FAILURE) ||
			(status == AM_NO_LOC_MEM_ERR) ||
			(status == AM_CALL_LEN_OVF)) {
			/* local AM errors will be registered */
			p_map_vw->numE++;	/* error counter */

			p_map_vw->code = MAP_ERR_CALL_CONFU;
			p_map_vw->info = status;
			err_rec ();

			recorder (0x82);
		} else {
			/* any remote AM_@@@ error */
			recorder (0x84);
		}
		/* record the missing or wrong answered node */
		i = ++map_cc_update_mc[0].lla;
		map_cc_update_mc[i].lla = *(unsigned char *) (ext_ref);
		map_cc_update_mc[i].status = status;

	}
	return;
}


		 /*
		    MAP_CALL_CONF_UPDATE_WTB {53, 83}
		  */

void 
map_call_conf_update_WTB (char caller_function,
						  char *ext_ref,
						  const AM_NET_ADDR * replier,
						  s_tgm_vehdes * in_msg_adr,
						  CARDINAL32 in_msg_size,
						  AM_RESULT status)
{

	/*
	   --- M A S T E R / S L A V E ---
	   call_confirm function for all ui services except multicast.
	   Installd by am_call_requ, called by map_update_WTB
	   ----------------------------------------------------------
	   ext_ref = node number (lla).
	 */
	char i;

	p_map_vw->numN++;			/* count of nodes */

	if (status == AM_OK) {
		recorder (0x53);
	} else {
		p_map_vw->numE++;		/* error counter */

		p_map_vw->code = MAP_ERR_CALL_CONFU;
		p_map_vw->info = status;
		err_rec ();

		/* record the missing or wrong answered nodes */
		i = ++map_cc_update[0].lla;
		map_cc_update[i].lla = map_tcn_node_addr[*ext_ref];
		map_cc_update[i].status = status;

		recorder (0x83);
	}
	return;
}



	  /*
	     Functions: MAP_SETUP_MASTER, MAP_SETUP_SLAVE, MAP_INCLUDE
	   */

		 /*
		    MAP_SETUP_MASTER {10, 11, 13, 14, 18, 19, 47, 49}
		  */

void 
map_setup_master ()
{

	/*
	   Processes all necessary steps to do the train inauguration on MASTER.
	 */

	/*
	   DATA DECLARATION
	 */
	short err;
	unsigned char i, work, j;
	CARDINAL32 out_msg_size, in_msg_size, rr_size;

	static AM_NET_ADDR replier;
	static s_tgm_nvehdes in_tgm_veh;	/* recv buffer for n vehicle descriptors */
	static s_tgm_def rr_tgm[MAP_MAX_NO_VEH];	/* short request, short reply */
	static s_tgm_def xx_tgm;

	replier.function = MAP_FUNCTION_NR;
	replier.device = AM_UNKNOWN_DEVICE;

	switch (p_map_vw->function_step) {

	case START_SETUP:
		{

			/*
			   REQUEST THE VEHICLE DESCRIPTOR FROM ALL SLAVES
			 */

			for (i = 0; i < MAP_MAX_NO_VEH; i++) {
				p_map_vw->ext_c_wtb[i] = i;
			}
			p_map_vw->numN = 1;	/* count of received responses reset */
			p_map_vw->numE = 0;	/* count of errors reset */

			recorder (0x10);	/* SETUP  */

			rr_size = 4;
			rr_tgm[0].key = 0;
			rr_tgm[0].owner = 0;
			rr_tgm[0].code = ATTR_LIST_REQUEST;

			in_msg_size = SIZE_VH_DESCR + 5;

			/*
			   REQUEST TO ALL NODES
			 */

			for (i = 0; i < map_tcn_nodes_max; i++) {
				/* get link layer address (TCN node number) */
				replier.vehicle = map_tcn_node_addr[i];

				if (map_ci.NODEstate.node_address == replier.vehicle) {

					/*
					   MASTER HIMSELF
					 */
					NADI.vehicle[i] = *map_ci.p_VD;

					/* complete with TCN type. */
					NADI.vehicle[i].TCNType = map_ci.bmi_type;
				} else {
					recorder (0x11);	/* SETUP  */

					/* current veh. descriptor is input buffer */
					am_call_requ (MAP_FUNCTION_NR, &replier,
								  &rr_tgm[0], rr_size,
								  &in_tgm_veh.member[i],
								  in_msg_size,
								  E2_TIMEOUT, (AM_CALL_CONF)
								  map_call_conf_setup,
								  &p_map_vw->ext_c_wtb[i]);
				}
			}

			p_map_vw->function_step = SEND_NADI;	/* n e x t  s t e p */

			break;

		}

	case SEND_NADI:
		{

			/*
			   WAIT FOR VEH_DES, SEND NADI TO ALL SLAVES
			 */

			if (p_map_vw->numN >= map_tcn_nodes_max) {

				/*
				   All vehicle descriptors are received, send NADI.
				 */
				recorder (0x13);

				if (p_map_vw->numE == 0) {

					/*
					   PREPARE THE NEXT STEP
					 */

					/*
					   COMPLETE NADI WITH LOCAL INFORMATIONS
					 */
					NADI.state.version++;
					NADI.state.validity = MAP_NADI_VALID;
					NADI.state.ownaddr = map_ci.p_VD->NodeAddr;
					NADI.state.VehicleNo = map_tcn_nodes_max;
					NADI.state.mdirect = map_ci.NODEstate.node_orient;

					build_vrno ();	/* range numbers */


					/*
					   PREPARE TGM CONTENT AND PARAMETER
					 */
					map_tgm_nadi.id.key = 0;
					map_tgm_nadi.id.owner = 0;
					map_tgm_nadi.id.code = ATTR_DIR_REQUEST;
					map_tgm_nadi.format = 0;
					map_tgm_nadi.NADI = NADI;	/* fill NADI telegramm */

					out_msg_size = map_tcn_nodes_max * SIZE_VH_DESCR + SIZE_NN_STATE + 5;
					rr_size = 6;

					for (i = 0; i < MAP_MAX_NO_MVB; i++) {
						p_map_vw->ext_c_wtb[i] = i + 100;	/* ext. ref. */
					}
					p_map_vw->numN = 1;		/* count of responses reset */


					for (i = 0; i < map_tcn_nodes_max; i++) {
						/* get link layer address (TCN node number) */
						replier.vehicle = map_tcn_node_addr[i];

						if (map_ci.NODEstate.node_address != replier.vehicle) {
							recorder (0x14);

							/* send NADI to node  [ i ] */
							am_call_requ (MAP_FUNCTION_NR, &replier,
										  &map_tgm_nadi, out_msg_size,
										  &rr_tgm[i], rr_size,
										  E2_TIMEOUT,
										  (AM_CALL_CONF) map_call_conf_setup,
										  &p_map_vw->ext_c_wtb[i]);
						}
					}
					p_map_vw->function_step = WAIT;		/* set the next step */

				} else {
					/* set the  n e x t  s t e p */
					p_map_vw->function_step = ERR_INAUGURATION;
				}
			}
			break;

		}

	case WAIT:
		{

			/*
			   WAIT FOR NADI RESPONSE, SEND BROADCAST
			 */

			if (p_map_vw->numN >= map_tcn_nodes_max) {

				if (p_map_vw->numE == 0) {

					/*
					   PREPARE THE NEXT STEP
					 */

					/*
					   PREPARE THE WORK BUFFERS FOR NADI AND GROUP_LIST
					 */
					map_xnadi_ui = NADI;
					map_ynadi_ui = NADI;

					for (i = 0; i < MAP_MAX_NO_GRP; i++) {
						map_xgroup_list_ui[i] = GROUP_LIST[i];
						map_ygroup_list_ui[i] = GROUP_LIST[i];
					}
					p_map_vw->actual_nadi = 0;	/* the first acces is X.. */
					p_map_vw->actual_group_list = 0;

					out_msg_size = 4;
					xx_tgm.key = 0;
					xx_tgm.owner = 0;
					xx_tgm.code = INAUG_READY;
					in_msg_size = 6;

					for (i = 0; i < map_tcn_nodes_max; i++) {
						/* link layer address (TCN node number) */
						replier.vehicle = map_tcn_node_addr[i];

						if (map_ci.NODEstate.node_address != replier.vehicle) {
							/* send ready tgm to node  [ i ] */
							am_call_requ (MAP_FUNCTION_NR, &replier,
										  &xx_tgm, out_msg_size,
										  &rr_tgm[i], in_msg_size,
										  E2_TIMEOUT,
										  (AM_CALL_CONF) map_call_conf_setup,
										  &p_map_vw->ext_c_wtb[i]);
						}
					}

					/*
					   Initialize am_@@@ in MASTER at this time, because the MASTER is
					   ready at latest.
					 */
					recorder (0xFF);	/* testrecorder for UPDATE mode reset */

					map_update_init = map_update_ini ();
					p_map_vw->numN = 1;		/* count of received responses reset */
					p_map_vw->function_step = WAIT_BROADCAST;	/* n e x t  s t e p */
					// map_not_broadcast = FALSE; /* <ATR:16> */

					recorder (0x18);

				} else {
					/* set the  n e x t  s t e p */
					p_map_vw->function_step = ERR_INAUGURATION;

					recorder (0x47);
				}
			}
			break;

		}

	case WAIT_BROADCAST:
		{

			/*
			   WAIT FOR BROADCAST RESPONSE, CHANGE INTO UPDATE MODE
			 */

			if (p_map_vw->numN >= map_tcn_nodes_max) {

				map_not_broadcast = FALSE; /* <ATR:16> */

				p_map_vw->f1st_run_u_wtb = 0;	/* starts update mode */
				p_map_vw->f1st_run_u_mvb = 0;	/* starts update mode */

				map_ui_delay = map_ui_delay_max - 3;
				p_map_vw->phase = UPDATE_MODE;

#ifdef O_960

				pi_pend_semaphore (map_sema, PI_FOREVER, &err);

#endif
				p_map_vw->order = NEXT_ORDER;
				p_map_vw->order_ui = NEXT_ORDER;

				if (map_ci.NodeKonf->device_list.number != 0) {

					/*
					   NADI distribution to MVB nodes if the current node is
					   a gateway node by storing this special order for update mode.
					 */
					p_map_vw->order_ui = MVB_NADI_INI_REQU;
				}
				p_map_vw->upd_status = MAP_UPDATE_READY;	/* to get a request */

#ifdef O_960

				pi_post_semaphore (map_sema, &err);

#endif

				/*
				   EVENT RECORDER
				 */

#ifdef O_960

				RECORD_M_EVENT ("NADI distributed, UPDATE mode.");

#endif


				/*
				   TERM_OUT
				 */

#ifdef TERM_OUT

				write_serv ("^MAP: NADI distributed, SETUP ready.^");

#endif


				if (map_CTRL.map_reportF != 0) {
					map_CTRL.map_reportF (MAP_NADI_SET_UP);
				}
				recorder (0x19);
			}
			break;

		}

	case ERR_INAUGURATION:
		{

			/*
			   ERROR HANDLING, NEW INAUGURATION
			 */
			p_map_vw->code = MAP_ERR_SETUP_MASTER_NR;
			p_map_vw->info = p_map_vw->numN - p_map_vw->numE;
			err_rec ();

			NADI.state.mdirect = MAP_DIREC_UNKNOWN;
			NADI.state.validity = MAP_NADI_INVALID;
			map_xnadi_ui.state.mdirect = MAP_DIREC_UNKNOWN;
			map_xnadi_ui.state.validity = MAP_NADI_INVALID;
			map_ynadi_ui.state.mdirect = MAP_DIREC_UNKNOWN;
			map_ynadi_ui.state.validity = MAP_NADI_INVALID;

			p_map_vw->upd_status = MAP_UPDATE_NOT_READY;
			p_map_vw->phase = IDLE;

			RECORD_M_ERR ("RTP error, new naming required.");

			/*
			   TERM_OUT
			 */

#if defined ( TERM_OUT )

			write_serv ("^MAP: RTP error, new naming required.^");

#endif


			/*
			   New TCN inauguration is the only possibility to synchronize the restart
			   of the mapping server at all nodes.
			 */
			/* same procedure for L_STRONG and L_WEAK */
			map_CtrlReport (MAP_NEW_INAUGURATION);

			recorder (0x49);

			break;

		}
	}
	return;
}


		 /*
		    MAP_SETUP_SLAVE {20, 21, 22, 27, 28, 40, 41}
		  */

void 
map_setup_slave ()
{

	/*
	   Processes all necessary steps to do the train inauguration on SLAVE.
	 */
	char i;
	short err;
	static AM_RESULT status;
	static CARDINAL32 in_size;

	switch (p_map_vw->function_step) {

	case START_SETUP:
		{

			/*
			   RECEIVE VEH_DES REQUEST
			 */
			am_bind_replier (MAP_FUNCTION_NR,
							 (AM_RCV_CONF) map_recv_conf_setup,
							 (AM_REPLY_CONF) map_reply_conf_setup,
							 &status);

			if (status == AM_OK) {

				/*
				   PREPARE RECEIVE FOR VEH_DES REQU.
				 */
				recorder (0x20);	/* SETUP  */

				in_size = 6;
				p_map_vw->ext_c_wtb[0] = RECV_VEH_DES_REQUEST;

				/* V E H I C L E  D E S C R.  is send in recv_conf_setup. */
				am_rcv_requ (MAP_FUNCTION_NR, &map_tgm_nadi, in_size,
							 &p_map_vw->ext_c_wtb[0], &status);

				if (status == AM_OK) {
					/* next step is wait ... */
					p_map_vw->function_step = WAIT;
					recorder (0x21);
				} else {
					p_map_vw->function_step = ERR_INAUGURATION;
					p_map_vw->code = MAP_ERR_SETUP_REQ;
					p_map_vw->info = status;
					err_rec ();
					recorder (0x41);
				}

			} else {
				p_map_vw->function_step = ERR_INAUGURATION;

				p_map_vw->code = MAP_ERR_SETUP_BR;
				p_map_vw->info = status;
				err_rec ();

				recorder (0x40);
			}
			break;

		}

	case WAIT:
		{
			/* DO NOTHING, ACTION IS DONE BY RECV_CONF_SETUP */
			recorder (0x22);

			break;
		}

	case WAIT_BROADCAST:
		{

			/*
			   CHECK DELAY TIME
			 */

			if (++map_ui_delay >= map_ui_delay_max) {

				/*
				   no reply from MASTER (no broadcast) within timeout period.
				   So the next step completes the inauguration at this SLAVE.
				 */
				map_ui_delay = map_ui_delay_max;
				p_map_vw->function_step = END_INAUGURATION;

				recorder (0x27);
			}
			break;

		}

	case END_INAUGURATION:
		{

			/*
			   PREPARE FOR UPDATE MODE
			 */
			NADI.state.ownaddr = map_ci.p_VD->NodeAddr;
			NADI.state.mdirect = map_ci.NODEstate.node_orient;

			/*
			   PREPARE THE WORK BUFFERS FOR NADI AND GROUP_LIST
			 */
			map_xnadi_ui = NADI;
			map_ynadi_ui = NADI;

			for (i = 0; i < MAP_MAX_NO_GRP; i++) {
				map_xgroup_list_ui[i] = GROUP_LIST[i];
				map_ygroup_list_ui[i] = GROUP_LIST[i];
			}
			p_map_vw->actual_nadi = 0;	/* the first acces is X.. */
			p_map_vw->actual_group_list = 0;

			/* reset for next run */
			am_unbind_replier (MAP_FUNCTION_NR);

			p_map_vw->f1st_run_u_wtb = 0;	/* starts update mode */
			p_map_vw->f1st_run_u_mvb = 0;	/* starts update mode */

			p_map_vw->phase = UPDATE_MODE;

#ifdef O_960

			pi_pend_semaphore (map_sema, PI_FOREVER, &err);

#endif
			p_map_vw->order = NEXT_ORDER;
			p_map_vw->order_ui = NEXT_ORDER;

			if (map_ci.NodeKonf->device_list.number != 0) {

				/*
				   NADI distribution to MVB nodes if the current node is
				   a gateway node by storing this special order for update mode.
				 */
				p_map_vw->order_ui = MVB_NADI_INI_REQU;
			}
			p_map_vw->upd_status = MAP_UPDATE_READY;	/* to get a request */

#ifdef O_960

			pi_post_semaphore (map_sema, &err);

#endif

			/*
			   EVENT RECORDER
			 */

#ifdef O_960

			RECORD_M_EVENT ("NADI received, UPDATE mode.");

#endif


			/*
			   TERM_OUT
			 */

#if defined ( TERM_OUT )

			write_serv ("^MAP: NADI received, SETUP ready.^");

#endif


			if (map_CTRL.map_reportF != 0) {
				map_CTRL.map_reportF (MAP_NADI_SET_UP);
			}
			recorder (0xFF);	/* testrecorder for UPDATE mode  reset */
			recorder (0x28);

			break;

		}

	case ERR_INAUGURATION:
		{

			/*
			   ERROR HANDLING, NEW INAUGURATION
			 */
			NADI.state.validity = MAP_NADI_INVALID;
			NADI.state.mdirect = MAP_DIREC_UNKNOWN;

			map_xnadi_ui = NADI;
			map_ynadi_ui = NADI;

			p_map_vw->upd_status = MAP_UPDATE_NOT_READY;
			p_map_vw->function_step = START_SETUP;
			p_map_vw->phase = IDLE;

			/*
			   New TCN inauguration is the only possibility to synchronize the restart
			   of the mapping server at all nodes.
			 */
			RECORD_M_ERR ("RTP error, new naming required by MASTER");

			/*
			   TERM_OUT
			 */

#if defined ( TERM_OUT )

			write_serv ("^MAP: RTP error, new naming required by MASTER.^");

#endif

			recorder (0x48);

			break;

		}
	}
	return;
}


		 /*
		    MAP_INCLUDE_SLAVE {1A, 1B, 1F, 4F}
		  */

void 
map_include_slave ()
{

	/*
	   Processes all necessary steps to do the train inauguration after 
	   a node inclusion.
	 */

	/*
	   DATA DECLARATION
	 */
	short err;
	unsigned char i, work, j;

	static CARDINAL32 in_msg_size, r_size;
	static AM_RESULT status;
	static AM_NET_ADDR replier;
	static s_tgm_def r_tgm;		/* short request */

	replier.function = MAP_FUNCTION_NR;
	replier.device = AM_UNKNOWN_DEVICE;
	replier.vehicle = 1;		/* MASTER */

	r_size = 4;
	r_tgm.key = 0;
	r_tgm.owner = 0;

	switch (p_map_vw->function_step) {

	case START_RECOVERY:
		{

			/*
			   SEND NADI REQUEST TO THE MASTER NODE
			 */
			/* for security: cancel all prabably receive requests */
			am_rcv_cancel (MAP_FUNCTION_NR, &p_map_vw->ext_u_wtb[0], &status);
			am_rcv_cancel (MAP_FUNCTION_NR, &p_map_vw->ext_u_wtb[1], &status);
			am_rcv_cancel (MAP_FUNCTION_NR, &p_map_vw->ext_u_wtb[2], &status);
			am_unbind_replier (MAP_FUNCTION_NR);

			p_map_vw->ext_c_wtb[0] = 1;
			p_map_vw->numN = 0;	/* count of received responses reset */
			p_map_vw->numE = 0;	/* count of errors reset */

			r_tgm.code = I_NADI_REQUEST;
			in_msg_size = map_tcn_nodes_max * SIZE_VH_DESCR + SIZE_NN_STATE + 5;

			/* Send request to the MASTER node to get the NADI. */
			am_call_requ (MAP_FUNCTION_NR, &replier, &r_tgm, r_size,
						  &map_tgm_nadi, in_msg_size,
						  E2_TIMEOUT, (AM_CALL_CONF) map_call_conf_include,
						  &p_map_vw->ext_c_wtb[0]);

			p_map_vw->function_step = GET_GROUP_LIST;	/* n e x t  s t e p */

			recorder (0x1A);

			break;

		}

	case GET_GROUP_LIST:
		{

			/*
			   WAIT FOR NADI, SEND GROUP_LIST REQUEST TO THE MASTER NODE
			 */

			if (p_map_vw->numN == 1) {

				if (p_map_vw->numE == 0) {
					p_map_vw->ext_c_wtb[0] = 2;
					p_map_vw->numN = 0;
					r_tgm.code = I_GROUP_LIST_REQUEST;
					in_msg_size = MAP_MAX_NO_VGL * 2 + 2 + 5;

					/* Request to the MASTER node to get the GROUP_LIST */
					am_call_requ (MAP_FUNCTION_NR, &replier,
								  &r_tgm, r_size,
								  &map_upd_out_tgm, in_msg_size,
								  E2_TIMEOUT,
								  (AM_CALL_CONF) map_call_conf_include,
								  &p_map_vw->ext_c_wtb[0]);

					/* n e x t  s t e p */
					p_map_vw->function_step = WAIT;

					recorder (0x1B);
				} else {
					/* n e x t  s t e p */
					p_map_vw->function_step = ERR_INAUGURATION;
				}
			}
			break;

		}

	case WAIT:
		{

			/*
			   WAIT FOR GROUP_LIST, CHANGE INTO UPDATE MODE
			 */

			if (p_map_vw->numN == 1) {

				if (p_map_vw->numE == 0) {

					/*
					   PREPARE FOR UPDATE MODE
					 */
					NADI.state.ownaddr = map_ci.p_VD->NodeAddr;
					NADI.state.mdirect = map_ci.NODEstate.node_orient;

					/*
					   PREPARE THE WORK BUFFERS FOR NADI AND GROUP_LIST
					 */
					map_xnadi_ui = NADI;
					map_ynadi_ui = NADI;

					for (i = 0; i < MAP_MAX_NO_GRP; i++) {
						map_xgroup_list_ui[i] = GROUP_LIST[i];
						map_ygroup_list_ui[i] = GROUP_LIST[i];
					}
					p_map_vw->actual_nadi = 0;	/* the first acces is X.. */
					p_map_vw->actual_group_list = 0;

					/* reset for next run */
					am_unbind_replier (MAP_FUNCTION_NR);

					p_map_vw->f1st_run_u_wtb = 0;	/* starts update mode */
					p_map_vw->f1st_run_u_mvb = 0;	/* starts update mode */

					p_map_vw->phase = UPDATE_MODE;

					recorder (0x1F);

#ifdef O_960

					pi_pend_semaphore (map_sema, PI_FOREVER, &err);

#endif
					p_map_vw->upd_status = MAP_UPDATE_READY;
					p_map_vw->order = NEXT_ORDER;
					p_map_vw->order_ui = NEXT_ORDER;

					if (map_ci.NodeKonf->device_list.number != 0) {

						/*
						   NADI distribution to MVB nodes if the current node is
						   a gateway node by storing this special order for update mode.
						 */
						p_map_vw->order_ui = MVB_NADI_INI_REQU;
					}
#ifdef O_960

					pi_post_semaphore (map_sema, &err);
					RECORD_M_EVENT ("NADI and GROUP_LIST included.   ");

#endif

					/*
					   TERM_OUT
					 */

#if defined ( TERM_OUT )

					write_serv ("^MAP: NADI/GROUP_LIST included.^");

#endif


					if (map_CTRL.map_reportF != 0) {
						map_CTRL.map_reportF (MAP_NADI_SET_UP);
					}
				} else {
					/* n e x t  s t e p */
					p_map_vw->function_step = ERR_INAUGURATION;
				}
			}
			break;

		}

	case ERR_INAUGURATION:
		{

			/*
			   ERROR HANDLING
			 */
			NADI.state.validity = MAP_NADI_INVALID;
			NADI.state.mdirect = MAP_DIREC_UNKNOWN;

			map_xnadi_ui = NADI;
			map_ynadi_ui = NADI;

			p_map_vw->upd_status = MAP_UPDATE_NOT_READY;

			map_include_repeat++;

			if (map_include_repeat < 5) {
				p_map_vw->function_step = START_RECOVERY;
			} else {
				map_include_repeat = 3;
				p_map_vw->phase = IDLE;

#ifdef O_960

				RECORD_M_ERR ("1:Fatal error, no NADI/GROUP_LIST");
				RECORD_M_ERR ("2:included after 5 requests!    ");

				/*
				   TERM_OUT
				 */

#if defined ( TERM_OUT )

				write_serv
					("^MAP error, no NADI/GROUP_LIST included, 5 requ.!^");

#endif


#endif
			}
			recorder (0x4E);

			break;

		}
	}
	return;
}



	  /*
	     Functions: MAP_UPDATE_INI, MAP_UPDATE_MVB, MAP_UPDATE_WTB, MAP_UPDATE
	   */

		 /*
		    MAP_UPDATE_INI {60, 90, 9E}
		  */

			/*
			   This function prepares the receiver site for update mode.
			 */

char 
map_update_ini (void)
{
	unsigned char i;
	CARDINAL32 in_size;
	AM_RESULT status;

	am_bind_replier (MAP_FUNCTION_NR,
					 (AM_RCV_CONF) map_recv_conf_update,
					 (AM_REPLY_CONF) map_reply_conf_update,
					 &status);

	if (!((status == AM_OK) || (status == AM_ALREADY_USED))) {
		am_unbind_replier (MAP_FUNCTION_NR);

		p_map_vw->code = MAP_ERR_UPDATE_BR;
		p_map_vw->info = status;
		err_rec ();

		recorder (0x90);

		/*
		   EVENT RECORDER
		 */

#ifdef O_960

		RECORD_M_ERR ("err: am_bind_replier UPDATE-INIT.");

#endif

		return 0;				/* error, do nothing, retry next time */
	}
	/*
	   PREPARE ALL AM_RCV_REQU
	 */

	/*
	   By reason of asynchronous behavior of the receive_confirm and
	   the reply_conf functions, it is necessary to perform a am_rcv_request
	   for each node at the same time.
	 */
	in_size = SIZE_VH_DESCR + 5;

	for (i = 0; i < map_tcn_nodes_max; i++) {
		p_map_vw->ext_u_wtb[i] = i;

		am_rcv_requ (MAP_FUNCTION_NR, &map_upd_in_tgm_wtb[i], in_size,
					 &p_map_vw->ext_u_wtb[i], &status);

		if (status != AM_OK) {
			p_map_vw->code = MAP_ERR_UPDATE_RCV;
			p_map_vw->info = status;
			err_rec ();

			/*
			   EVENT RECORDER
			 */

#ifdef O_960

			RECORD_M_ERR ("RTP err: am_rcv_requ in UPDATE-INIT");

#endif


			for (i = 0; i < map_tcn_nodes_max; i++) {
				am_rcv_cancel (MAP_FUNCTION_NR, &p_map_vw->ext_u_wtb[i], &status);
			}
			am_unbind_replier (MAP_FUNCTION_NR);

			recorder (0x9E);

			return 0;			/* ------> error, do nothing, retry next time */
		}
	}

	recorder (0x60);

	return 1;
}


		 /*
		    MAP_UPDATE_MVB {A1, A2}
		  */

			/*
			   This function serves all user interface calls for the MVB level.
			 */

void 
map_update_MVB ()
{

	/*
	   DATA DECLARATION
	 */
	char i, k, l;
	unsigned char uc_work;
	AM_RESULT status;

	static CARDINAL32 out_size, rr_size;
	static s_tgm_def rr_tgm[MAP_MAX_NO_MVB];
	static AM_NET_ADDR replier;


	if (p_map_vw->f1st_run_u_mvb != NPT) {
		p_map_vw->f1st_run_u_mvb = NPT;		/* First run succesfull complete */

		recorder (0xF1);		/* update test recorder reset */

		/*
		   RECORD BUFFER
		 */

#ifdef O_960

		RECORD_M_EVENT ("Start UPDATE mode for MVB.      ");

#endif

	}
	if ((p_map_vw->order != NEXT_ORDER) &&
		(p_map_vw->order != INIT_ORDER)) {

		/*
		   PREPARE SOME DATA
		 */

		for (i = 0; i < MAP_MAX_NO_MVB; i++) {
			p_map_vw->ext_c_mvb[i] = i;		/* ext. ref. */
		}

		for (i = 0; i < MAP_MAX_NO_VEH; i++) {
			/* to collect error codes */
			map_cc1_update[i].lla = 0;
			map_cc1_update[i].status = 0;
		}
		map_upd1_out_tgm.format = 0;

		map_numN = 0;			/* count of received responses reset */
		map_numE = 0;			/* count of errors reset */

		map_upd1_out_tgm.id.code = p_map_vw->order;		/* default */


		/*
		   PREPARE FOR THE CURRENT ORDER
		 */

		switch (p_map_vw->order) {

		case MVB_NADI_INI_REQU:
			{

				/*
				   UI SETUP MVB
				 */
				map_tgm_nadi.NADI = NADI;
				map_tgm_nadi.id.code = MVB_NADI_INI_REQU;
				map_tgm_nadi.id.key = 0;
				map_tgm_nadi.id.owner = 0;
				out_size = map_tcn_nodes_max * SIZE_VH_DESCR + SIZE_NN_STATE + 5;

				/*
				   TERM_OUT
				 */

#if defined ( TERM_OUT )

				write_serv ("^MAP: send NADI to MVB^");

#endif

				break;

			}

		case DYN_ATTR_REQUEST:
			{

				/*
				   UI CALL: map_SetVehicleDescriptor
				 */
				map_upd1_out_tgm.content.vehicle = p_map_vw->upd.vehicle;
				out_size = SIZE_VH_DESCR + 5;

				/*
				   TERM_OUT
				 */

#if defined ( TERM_OUT )


				if (MAP_TRACE_ON) {
					write_serv ("^MAP: send dyn attr to MVB^");
				}
#endif

				break;

			}

		case GROUP_LIST_REQUEST:
		case MVB_GROUP_LIST_INI_REQU:
			{

				/*
				   UI CALL: map_SetGroupList  or  UI SETUP MVB
				 */
				map_upd1_out_tgm.id.code = GROUP_LIST_REQUEST;
				map_upd1_out_tgm.content.group = p_map_vw->upd.group;
				map_upd1_out_tgm.format = 4;
				out_size = p_map_vw->upd.group.length * 2 + 2 + 5;

				/*
				   TERM_OUT
				 */

#if defined ( TERM_OUT )


				if (MAP_TRACE_ON) {
					write_serv ("^MAP: send group list to MVB^");
				}
#endif

				break;

			}

		case IDLE_STATE_REQUEST:
			{

				/*
				   UI CALL: map_SetIdle
				 */
				out_size = 4;

				/*
				   TERM_OUT
				 */

#if defined ( TERM_OUT )


				if (MAP_TRACE_ON) {
					write_serv ("^MAP: send idle state to MVB^");
				}
#endif

				break;

			}

		case CANCEL_REQUEST:
			{

				/*
				   UI CALL: map_cancel
				 */

				for (i = 0; i < map_ci.NodeKonf->device_list.number; i++) {
					replier = map_ci.NodeKonf->device_list.device[i];
					am_call_cancel (MAP_FUNCTION_NR, &replier, &status);
				}
				p_map_vw->upd_status = MAP_UPDATE_READY;
				p_map_vw->order = NEXT_ORDER;

				/*
				   TERM_OUT
				 */

#if defined ( TERM_OUT )


				if (MAP_TRACE_ON) {
					write_serv ("^MAP: cancel request MVB^");
				}
#endif

				break;

			}
		default:{

				/*
				   WRONG ORDER
				 */
				p_map_vw->order = NEXT_ORDER;

				recorder (0xA1);

				break;

			}
		}


		/*
		   DO THE CURRENT ORDER
		 */

		if (p_map_vw->order != NEXT_ORDER) {
			map_upd1_out_tgm.id.key = 0;
			map_upd1_out_tgm.id.owner = 0;
			rr_size = 6;

			for (i = 0; i < map_ci.NodeKonf->device_list.number; i++) {
				replier = map_ci.NodeKonf->device_list.device[i];	/* address */

				if (replier.device != 0) {
					p_map_vw->ext_c_mvb[i] = replier.vehicle;

					if (p_map_vw->order == MVB_NADI_INI_REQU) {
						am_call_requ (MAP_FUNCTION_NR,
									  &replier, &map_tgm_nadi,
									  out_size, &rr_tgm[i],
									  rr_size, E2_TIMEOUT,
									  (AM_CALL_CONF)
									  map_call_conf_update_MVB,
									  &p_map_vw->ext_c_mvb[i]);
					} else {
						am_call_requ (MAP_FUNCTION_NR,
									  &replier, &map_upd1_out_tgm,
									  out_size, &rr_tgm[i],
									  rr_size, E2_TIMEOUT,
									  (AM_CALL_CONF)
									  map_call_conf_update_MVB,
									  &p_map_vw->ext_c_mvb[i]);
					}
				}
			}
			p_map_vw->order = NEXT_ORDER;

			recorder (0xA2);
		}
	} else {
		recorder (0xA1);
	}
	return;
}


		 /*
		    MAP_UPDATE_WTB {50-52, 54-56, 58-5A, 5D, 5F, E1-E9, ED, EF}
		  */

			/*
			   This function serves all user interface calls for the WTB level.
			 */

void 
map_update_WTB ()
{

	/*
	   DATA DECLARATION
	 */
	short err;
	char fix_addr;
	char caller_fct;
	char i, j, k, mc_sent;
	unsigned char type, uc_work;
	AM_RESULT status;
	map_result map_status;

	static CARDINAL32 out_size, rr_size;
	static s_tgm_def rr_tgm[MAP_MAX_NO_VEH];
	static AM_NET_ADDR replier;

	recorder (R_ORD);			/* p_map_vw->order */

	if (p_map_vw->order != NEXT_ORDER) {

		/*
		   PREPARE SOME DATA
		 */

		for (i = 0; i < MAP_MAX_NO_VEH; i++) {
			p_map_vw->ext_c_wtb[i] = 0;
		}

		for (i = 0; i < MAP_MAX_NO_VEH; i++) {
			map_cc_update[i].lla = 0;
			map_cc_update[i].status = 0;
		}
		map_upd_out_tgm.format = 0;
		replier.device = AM_UNKNOWN_DEVICE;
		map_sos_time = 0;

		p_map_vw->code = MAP_OK;
		p_map_vw->info = 0;

		recorder (0x50);


		/*
		   PREPARE THE CURRENT ORDER
		 */

		switch (p_map_vw->order) {

		case DYN_ATTR_REQUEST:
			{
				/*
				   UI CALL: map_SetVehicleDescriptor
				 */

				map_upd_out_tgm.content.vehicle = p_map_vw->upd.vehicle;
				map_upd_out_tgm.id.code = DYN_ATTR_REQUEST;
				out_size = SIZE_VH_DESCR + 5;

#ifdef O_960

				pi_pend_semaphore (map_sema, PI_FOREVER, &err);

#endif

				/* <ATR:11> */
				if (p_map_vw->upd.vehicle.NodeAddr == NADI.state.ownaddr) {
					if (map_ci.p_VD->TCNType != p_map_vw->upd.vehicle.TCNType) {
						/* do not multicast the new VD; a new inauguration will happen */
						act_order = p_map_vw->order = NEXT_ORDER;
						/* force a new inauguration */
						ls_t_Remove();
					}
					else {
#ifdef PDM
						/* set the PDM mode associated with the current VD */
						if (pdm_set_node_mode(node_mode_from_vhdes(&p_map_vw->upd.vehicle))) {
							/* do not multicast the new VD; a new inauguration will happen */
							act_order = p_map_vw->order = NEXT_ORDER;
						}
#endif
					}
				}

				if (p_map_vw->upd.vehicle.NodeAddr == NADI.state.ownaddr) {
					/* actualize the  o w n  vehicle descriptor */
					*map_ci.p_VD = p_map_vw->upd.vehicle;
				}
				/* Input the vehicle descriptor into the  o w n  NADI */
				j = map_tcn_nodes_max;

				for (i = 0; i < j; i++) {

					if (NADI.vehicle[i].NodeAddr == p_map_vw->upd.vehicle.NodeAddr) {

						NADI.vehicle[i] = p_map_vw->upd.vehicle;
						build_vrno ();	/* New range numbers are required */
						NADI.state.version++;

						break;
					}
				}

				/*
				   Toggle buffer for the user interface to perform dataconsistency.
				 */

				if (p_map_vw->actual_nadi == 0) {
					map_ynadi_ui = NADI;
					p_map_vw->actual_nadi = 1;
				} else {
					map_xnadi_ui = NADI;
					p_map_vw->actual_nadi = 0;
				}

#ifdef O_960

				pi_post_semaphore (map_sema, &err);

#endif
				recorder (0x59);

				/*
				   TERM_OUT
				 */

#ifdef TERM_OUT


				if (MAP_TRACE_ON) {
					write_serv ("^MAP: map_SetVehicleDescriptor^");
				}
#endif

				break;

			}

		case IDLE_STATE_REQUEST:
			{

				/*
				   UI CALL: map_SetIdle
				 */
				map_upd_out_tgm.id.code = IDLE_STATE_REQUEST;
				out_size = 4;

				/*
				   TERM_OUT
				 */

#if defined ( TERM_OUT )


				if (MAP_TRACE_ON) {
					write_serv ("^MAP: map_SetIdle^");
				}
#endif

				break;

			}

		case GROUP_LIST_REQUEST:
			{

				/*
				   UI CALL: map_SetGroupList
				 */
				map_upd_out_tgm.content.group = p_map_vw->upd.group;
				map_upd_out_tgm.id.code = GROUP_LIST_REQUEST;
				map_upd_out_tgm.format = 4;
				out_size = p_map_vw->upd.group.length * 2 + 2 + 5;

#ifdef O_960

				pi_pend_semaphore (map_sema, PI_FOREVER, &err);

#endif

				/*
				   INITIALIZE THE GROUP_LIST
				 */

				for (i = 0; i < MAP_MAX_NO_GRP; i++) {
					GROUP_LIST[i].length = 0;

					for (j = 0; j < MAP_MAX_NO_NGL; j++) {
						GROUP_LIST[i].node[j] = 0;
					}
				}


				/*
				   MOVE UI GROUP LIST CONTENT INTO OWN GROUP_LIST
				 */

				for (j = 0; j < p_map_vw->upd.group.length; j++) {
					/* group number is index onto GROUP_LIST */
					i = p_map_vw->upd.group.member[j].group - 1;

					if (i <= MAP_MAX_NO_GRP) {
						uc_work = p_map_vw->upd.group.member[j].node;
						k = GROUP_LIST[i].length;
						GROUP_LIST[i].node[k] = uc_work;
						GROUP_LIST[i].length++;
					}
				}


				if (p_map_vw->actual_group_list == 0) {

					for (i = 0; i < MAP_MAX_NO_GRP; i++) {
						map_ygroup_list_ui[i] = GROUP_LIST[i];
					}
					p_map_vw->actual_group_list = 1;
				} else {

					for (i = 0; i < MAP_MAX_NO_GRP; i++) {
						map_xgroup_list_ui[i] = GROUP_LIST[i];
					}
					p_map_vw->actual_group_list = 0;
				}

#ifdef O_960

				pi_post_semaphore (map_sema, &err);

#endif
				recorder (0x5A);

				/*
				   TERM_OUT
				 */

#if defined ( TERM_OUT )

				write_serv ("^MAP: map_SetGroupList^");

#endif

				break;

			}

		case SWITCHOVER_REQUEST:
			{

				/*
				   UI CALL: map_SwitchOver
				 */
				map_upd_out_tgm.id.code = SWITCHOVER_REQUEST;
				out_size = 4;

				/*
				   TERM_OUT
				 */

#if defined ( TERM_OUT )


				if (MAP_TRACE_ON) {
					write_serv ("^MAP: map_SwitchOver^");
				}
#endif

				break;

			}

		case SWITCHBACK_REQUEST:
			{

				/*
				   UI CALL: map_SwitchBack
				 */
				map_upd_out_tgm.id.code = SWITCHBACK_REQUEST;
				out_size = 4;

				/*
				   TERM_OUT
				 */

#if defined ( TERM_OUT )


				if (MAP_TRACE_ON) {
					write_serv ("^MAP: map_SwitchBack^");
				}
#endif

				break;

			}

		case MULTICAST:
			{

				/*
				   UI CALL: map_multicast_requ
				 */

				/*
				   PREPARE SEVERAL DATA
				 */

				for (i = 0; i < MAP_MAX_NO_VEH; i++) {
					map_cc_update_mc[i].lla = 0;
					map_cc_update_mc[i].status = 0;
				}
				p_map_vw->numN = map_tcn_nodes_max;		/* max. amount of nodes */
				p_map_vw->numE = 0;

				replier.function = p_map_vw->mc_replier.function;	/* destination */
				caller_fct = p_map_vw->mc_caller;
				rr_size = 6;

				mc_sent = FALSE;


				for (i = 0; i < NADI.state.VehicleNo; i++) {
					fix_addr = FALSE;	/* send check */

					switch (p_map_vw->mc_replier.vehicle) {

						/*
						   CHECK FIXED ADDRESSES
						 */

					case MAP_MC_ALL_VEHICLES:
						{
							fix_addr = TRUE;	/* send check */

							recorder (0xE1);
							break;
						}

					case MAP_MC_ALL_CARRIAGES:
						{

							/*
							   bit0.3 is traction leader
							 */

							if (!(NADI.vehicle[i].DynAttr[0] & 0x08)) {
								fix_addr = TRUE;	/* send check */
							}
							recorder (0xE2);
							break;
						}

					case MAP_MC_TRACTION_LEADER:
						{

							/*
							   bit0.3 is traction leader
							 */

							if (NADI.vehicle[i].DynAttr[0] & 0x08) {
								fix_addr = TRUE;	/* send check */
							}
							recorder (0xE3);
							break;
						}

					case MAP_MC_LEADING_VEHICLE:
						{

							/*
							   bit0.1 is leading vehicle
							 */

							if (NADI.vehicle[i].DynAttr[0] & 0x02) {
								fix_addr = TRUE;	/* send check */
							}
							recorder (0xE4);
							break;
						}

					case MAP_MC_LAST_VEHICLE:
						{

							/*
							   bit1.0 last vehicle
							 */

							if (NADI.vehicle[i].DynAttr[1] & 0x01) {
								fix_addr = TRUE;	/* send check */
							}
							recorder (0xE5);
							break;
						}

					default:{

							/*
							   ADDRESSED BY GROUP NUMBER
							 */

							if ((p_map_vw->mc_replier.vehicle < MAP_MC_BASIC_GNR + 1) ||
								(p_map_vw->mc_replier.vehicle > MAP_MC_BASIC_GNR + MAP_MAX_NO_VGL)) {
								/* group number out of range, no MULTICAST */
								recorder (0xE9);
							} else {

								/*
								   SEND TO GROUP NODES
								 */
								/* work index for GROUP_LIST */
								i = p_map_vw->mc_replier.vehicle - MAP_MC_BASIC_GNR - 1;

								if (GROUP_LIST[i].length > 0) {

									for (j = 0; j < GROUP_LIST[i].length; j++) {
										replier.vehicle = GROUP_LIST[i].node[j];

										if (replier.vehicle != 0) {
											p_map_vw->ext_c_wtb[j] = replier.vehicle;

											am_call_requ (caller_fct, &replier,
														  p_map_vw->mc_tgm, p_map_vw->mc_tgm_length,
											&rr_tgm[j], rr_size, E2_TIMEOUT,
														  (AM_CALL_CONF) map_call_conf_mc_update_WTB,
												   &p_map_vw->ext_c_wtb[j]);

											mc_sent = TRUE;

											/* decrement the number of responses */
											p_map_vw->numN--;
											recorder (0xE6);
										}
									}
									recorder (0xE7);
								} else {

									/*
									   NO ENTRIES
									 */

									/*
									   no entries, no MULTICAST
									 */
									p_map_vw->numE = map_tcn_nodes_max;

									recorder (0xE8);

								}

							}
							break;

						}
					}

					if (fix_addr) {

						/*
						   SEND TO FIX ADDR.
						 */
						replier.vehicle = NADI.vehicle[i].NodeAddr;

						if ((replier.vehicle != 0) &&
							((replier.vehicle != map_ci.NODEstate.node_address) ||
							 (map_MC_to_own_node == 0xCCCC))) {
							p_map_vw->ext_c_wtb[i] = replier.vehicle;

							/* 2.10.95 ??
							   not p_map_vw->mc_caller, because the MAP must get the replies
							   to check the work out and to call the confirm function */

							am_call_requ (caller_fct, &replier,
								  p_map_vw->mc_tgm, p_map_vw->mc_tgm_length,
										  &rr_tgm[i], rr_size, E2_TIMEOUT,
								 (AM_CALL_CONF) map_call_conf_mc_update_WTB,
										  &p_map_vw->ext_c_wtb[i]);

							mc_sent = TRUE;

							/* decrement the number of responses */
							p_map_vw->numN--;

							recorder (0xED);
						} else {
							recorder (0xEF);
						}

					} else {
						break;
					}
				}

#ifdef O_960

				pi_pend_semaphore (map_sema, PI_FOREVER, &err);

#endif
				p_map_vw->upd_status = MAP_UPDATE_AT_WORK;

#ifdef O_960

				pi_post_semaphore (map_sema, &err);

#endif
				p_map_vw->old_order = p_map_vw->order;
				p_map_vw->order = NEXT_ORDER;	/* next step is send check */

				/*
				   TERM_OUT
				 */

#if defined ( TERM_OUT )


				if (MAP_TRACE_ON && mc_sent) {
					write_serv ("^MAP: map_multicast_requ^");
				}
#endif

				recorder (0x52);

				break;

			}

		case CANCEL_REQUEST:
			{

				/*
				   UI CALL: map_CancelRequest
				 */

				/*
				   CANCEL ALL WTB CALLS
				 */

				for (i = 0; i < map_tcn_nodes_max; i++) {
					replier.vehicle = map_tcn_node_addr[i];
					am_call_cancel (caller_fct, &replier, &status);
				}


				/*
				   CANCEL ALL MVB CALLS
				 */
				j = map_ci.NodeKonf->device_list.number;

				if (j != 0) {

					for (i = 0; i < j; i++) {
						replier = map_ci.NodeKonf->device_list.device[i];

						if (replier.device != 0) {
							am_call_cancel (caller_fct, &replier, &status);
						}
					}
				}
				if (p_map_vw->f.confirm != 0) {

					if (p_map_vw->old_order == MULTICAST) {
						p_map_vw->f.multicast (
												  p_map_vw->mc_caller,
												  p_map_vw->r_confirm,
												  &p_map_vw->mc_replier,
												  MAP_NOT_OK);
					} else {
						p_map_vw->f.confirm (
												p_map_vw->r_confirm,
												MAP_NOT_OK);
					}
					/* disable confirm function */
					p_map_vw->f.confirm = 0;
					p_map_vw->r_confirm = 0;
				}
				p_map_vw->upd_status = MAP_UPDATE_READY;
				p_map_vw->order = NEXT_ORDER;
				p_map_vw->old_order = NEXT_ORDER;

				p_map_vw->numN = 0;		/* count of received responses reset */
				p_map_vw->numE = 0;		/* count of errors reset */

				/*
				   TERM_OUT
				 */

#if defined ( TERM_OUT )


				if (MAP_TRACE_ON) {
					write_serv ("^MAP: map_CancelRequest^");
				}
#endif

				break;

			}
		default:{

				/*
				   WRONG ORDER
				 */
				p_map_vw->order = NEXT_ORDER;

				recorder (0x51);

				break;

			}
		}


		/*
		   DO THE CURRENT ORDER
		 */

		if (p_map_vw->order != NEXT_ORDER) {
			p_map_vw->numN = 1;	/* count of received responses reset = own node */
			p_map_vw->numE = 0;	/* count of errors reset */

			replier.function = MAP_FUNCTION_NR;

			map_upd_out_tgm.id.key = 0;
			map_upd_out_tgm.id.owner = 0;
			rr_size = 6;

			/*
			   Send to all other node members by the course like follow:
			   Except of the own node, the telegram will be send to all other
			   nodes. The call_confirm function will receive the responses from each
			   node. If all responses have arrived, recognizable by the counter
			   p_map_vw->numN, the user confirm function with MAP_OK status will be
			   called for finishing the user request (called by any ui function).
			 */

			for (i = 0; i < map_tcn_nodes_max; i++) {
				/* get link layer address (TCN node number) */
				replier.vehicle = map_tcn_node_addr[i];

				if ((replier.vehicle != 0) &&
					(map_ci.p_VD->NodeAddr != replier.vehicle)) {
					p_map_vw->ext_c_wtb[i] = replier.vehicle;

					/* Send telegramm to all other nodes. */
					am_call_requ (MAP_FUNCTION_NR, &replier,
								  &map_upd_out_tgm, out_size, &rr_tgm[i],
								  rr_size, E2_TIMEOUT,
								  (AM_CALL_CONF) map_call_conf_update_WTB,
								  &p_map_vw->ext_c_wtb[i]);
				}
			}
			/* current order is done */
			p_map_vw->old_order = p_map_vw->order;
			p_map_vw->order = NEXT_ORDER;

#ifdef O_960

			pi_pend_semaphore (map_sema, PI_FOREVER, &err);

#endif
			p_map_vw->upd_status = MAP_UPDATE_AT_WORK;

#ifdef O_960

			pi_post_semaphore (map_sema, &err);

#endif

			/*
			   TERM_OUT
			 */

#if defined ( TERM_OUT )

			write_serv ("^MAP: ui service performed^");

#endif

			recorder (0x52);
		}
	} else {
		recorder (0x51);
	}

	if (p_map_vw->numN >= map_tcn_nodes_max) {

		/*
		   FINISH THE CURRENT ORDER
		 */

		if (p_map_vw->numE == 0) {

			/*
			   All nodes have responsed.
			 */
			recorder (0x56);
		} else {
			recorder (0x5B);
		}

		/*
		   CALL CONFIRM FUNCTION IF NECESSARY
		 */

		if (p_map_vw->f.confirm != 0) {

			if (p_map_vw->numE == 0) {
				map_status = MAP_OK;

				recorder (0x54);
			} else {
				map_status = MAP_NOT_OK;

				recorder (0x55);
			}

			if (p_map_vw->old_order == MULTICAST) {
				p_map_vw->f.multicast (
										  p_map_vw->mc_caller,
										  p_map_vw->r_confirm,
										  &p_map_vw->mc_replier,
										  (AM_RESULT) map_status);
			} else {
				p_map_vw->f.confirm (
										p_map_vw->r_confirm,
										map_status);
			}
			/* disable the current confirm function */
			p_map_vw->f.confirm = 0;
			p_map_vw->r_confirm = 0;
		}
		/*
		   CALL INDICATION FUNTION IF NECESSARY
		 */

		if (p_map_vw->f_indication != 0) {

			switch (p_map_vw->old_order) {

			case IDLE_STATE_REQUEST:
				{
					type = MAP_IDLE_REQUEST;

					break;
				}

			case GROUP_LIST_REQUEST:
			case DYN_ATTR_REQUEST:
				{
					type = MAP_NADI_UPDATED;

					break;
				}

			case SWITCHOVER_REQUEST:
				{
					type = MAP_SWITCHOVER;

					break;
				}

			case SWITCHBACK_REQUEST:
				{
					type = MAP_SWITCHBACK;

					break;
				}
			default:{
					type = 0;
				}
			}

			if (type != 0) {

				/*
				   This function must be installed by map_idInstall()
				 */
				p_map_vw->f_indication (p_map_vw->r_indication, type, MAP_OK);
			}
		}
#ifdef O_960

		pi_pend_semaphore (map_sema, PI_FOREVER, &err);

#endif
		p_map_vw->upd_status = MAP_UPDATE_READY;

#ifdef O_960

		pi_post_semaphore (map_sema, &err);

#endif

		if (p_map_vw->old_order == IDLE_STATE_REQUEST) {
			/* set own node into IDLE state */
			map_stop ();

			recorder (0x58);
		} else {

			/*
			   CALL REPORT FUNCTION IF NECESSARY
			 */

			if ((map_CTRL.map_reportF != 0) &&
				((p_map_vw->old_order == GROUP_LIST_REQUEST) ||
				 (p_map_vw->old_order == DYN_ATTR_REQUEST))) {
				map_CTRL.map_reportF (MAP_NADI_UPDATE_RECEIVED);
			}
		}
		p_map_vw->old_order = NEXT_ORDER;

	} else {

		/*
		   SECURITY CHECK
		 */

		/*
		   security check, this case should never be possible, but it
		   occurs by application tests of Mr. Bahrs, AEG.
		 */
		map_sos_time++;

		if (map_sos_time > 600) {
			/* simulates a receive of all nodes after 30 seconds */
			p_map_vw->numN = map_tcn_nodes_max;

			p_map_vw->code = MAP_ERR_TO;
			p_map_vw->info = map_tcn_nodes_max;
			err_rec ();

			recorder (0x5F);
		}
	}
	return;
}


		 /*
		    MAP_UPDATE
		  */

void 
map_update ()
{

	/*
	   DESCRIPTION
	 */

	/*
	   In depend of the actual service call, ordered by the user interface, by another
	   WTB node or by a connected MVB node the functions map_update_WTB() and/or
	   map_update_MVB() will be called. If no MVB on the current node is connected,
	   only the user interface order is relevant.

	   The following cases of order combinations are possible on a gateway node
	   1. direct order from the user interface:
	   update on both, WTB and MVB is necessary.
	   2. order by any incoming tgm's from another WTB node:
	   update only on MVB is necessary.
	   3. order by an incoming tgm from the MVB level:
	   update on both, WTB and MVB is necessary.

	   Orders from MVB or WTB are stored into a queue by map_PutOrder(..).
	   Work through from the queue is fifo, so the orders shall distribute in
	   a time scheduled sequence.

	   Orders by an UI call are only permitted one at each time !!!
	   If there is more then one user call, so this call will not be accepted. This
	   information is given to the user by the return value of the UI call.
	 */


	/*
	   DATA DECLARATION
	 */
	short err;
	char replier_fct;
	unsigned char uc_work;
/* <ATR:11> */
//	unsigned short act_order;
	CARDINAL32 in_size;
	AM_RESULT status;


	if (p_map_vw->f1st_run_u_wtb != NPT) {

		/*
		   INITIALIZATIONS
		 */

		if (!map_update_init) {

			if (!(map_update_init = map_update_ini ())) {
				return;			/* error, do nothing, retry next time */
			}
		}
		/* Initialise further values and arrays */
		p_map_vw->numN = 0;		/* count of received responses reset */
		p_map_vw->numE = 0;		/* count of errors reset */

		p_map_vw->f1st_run_u_wtb = NPT;		/* First run succesfull complete */

#ifdef O_960

		RECORD_M_EVENT ("Start UPDATE mode for WTB.      ");

#endif

		/* <ATR:12> */
		pdm_start_node();
	}
#ifdef O_960

	pi_pend_semaphore (map_sema, PI_FOREVER, &err);

#endif

	if ((p_map_vw->order_ui != NEXT_ORDER) && (p_map_vw->order_ui != 0)) {

		if (map_PutOrder (UI, p_map_vw->order_ui, &p_map_vw->upd_ui)) {
			/* ok, next order from user interface is possible */
			p_map_vw->order_ui = NEXT_ORDER;
		}
	}
#ifdef O_960

	pi_post_semaphore (map_sema, &err);

#endif

	if (p_map_vw->upd_status == MAP_UPDATE_READY) {

		/*
		   HANDLE THE NEXT ORDER
		 */

		/*
		   For the first UI call all nodes must be ready with the
		   am_bind of all recv_confirm functions.
		   (WAIT-TIME aprox. 1.5 sec/node)
		 */

		if (++map_ui_delay >= map_ui_delay_max) {
			map_ui_delay = map_ui_delay_max;

			switch (map_GetOrder ()) {

			case UI:
			case MVB:
				{
					recorder (0x5C);

					/* order from UI or MVB, deliver on WTB and MVB */
					act_order = p_map_vw->order;	/* save order */
					map_update_WTB ();

					if (map_ci.NodeKonf->device_list.number != 0) {
						/* restore order for mvb */
						p_map_vw->order = act_order;

						recorder (0x5D);

						/* order from WTB, deliver on MVB */
						map_update_MVB ();
					}
					break;
				}

			case WTB:
				{

					if (map_ci.NodeKonf->device_list.number != 0) {
						recorder (0x5D);

						/* order from WTB, deliver on MVB */
						map_update_MVB ();
					}
					break;
				}
			}
		}
	} else {
		/* complete any running order */
		map_update_WTB ();
	}
	return;
}




   /*
      MAIN: MAP_NPT
    */

void 
map_npt (void)
{
	unsigned char i, j, work, cnt;
	short err;
	char caller, function;
	static AM_NET_ADDR replier;
	static AM_RESULT status;

	/*
	   RESET VALUE FOR NADI MEMBER
	 */
	map_VehicleDescr m_vh =
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	p_map_vw = &map_vw;			/* pointer onto VERGANGENHEITSWERTE */

	if ((p_map_vw->f1st_run_s != NPT) && (p_map_vw->f1st_run_s != NP_ERR)) {

		/*
		   INITIALIZATION
		 */

		/*
		   ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! !
		   This program block is only running after system boot or
		   so long if any error occurs.
		 */
		p_map_vw->f1st_run_s = NPT;		/* ok */

		recorder (0);			/* TESTRECORDER reset */

#ifdef O_960

		map_sema = pi_create_semaphore (1, PI_ORD_FCFS, &err);

		if (err != PI_RET_OK) {
			RECORD_M_ERR ("Fatal err, cant get sema from PIL");

			/*
			   TERM_OUT
			 */

#if defined ( TERM_OUT )

			write_serv
				("^MAP: Fatal error, can't get semaphore from PIL.^");

#endif

			p_map_vw->f1st_run_s = NP_ERR;	/* not ok */
			p_map_vw->code = MAP_ERR_SEMA;
			p_map_vw->info = err;
			err_rec ();
		}
#endif
		p_map_vw->tstart = 0;
		p_map_vw->tstop = 0;

	}
	if (p_map_vw->f1st_run_s == NPT) {

		/*
		   HANDLE BMI EVENT
		 */
		recorder (R_TCN);		/* record map_ci.bmi_event */

		switch (map_ci.bmi_event) {

		case START_INAUGURATION:
		case START_INCLUDE:
			{

				/*
				   GENERAL RESET
				 */
				p_map_vw->tstart++;		/* start event from TCN */

				recorder (0xEE);	/* setup testrecorder reset */

				/*
				   FILL THE OWN VEHICLE DESCRIPTOR WITH SOME TEST VALUES
				 */

				if (map_ci.bmi_type == L_STRONG) {
					work = 0x11;
				} else {
					work = 0xFF;
				}

/* <ATR:09> */
//                           for ( i=0; i<10; i++ )
//    {
//                              map_ci.p_VD->Reserved[ i ] = work;
//    }


				/*
				   BUILD THE MAP_TCN_NODE_ADDR LIST
				 */
				/* Own node address from TCN level */
				map_ci.p_VD->NodeAddr = map_ci.NODEstate.node_address;

				for (i = 0; i < MAP_MAX_NO_VEH; i++) {
					map_tcn_node_addr[i] = 0;
				}
				map_tcn_nodes_max = map_ci.p_TT->number_of_nodes;

				if ((map_tcn_nodes_max > 0) && (map_tcn_nodes_max <= MAP_MAX_NODES)) {
					work = map_ci.p_TT->top_address;

					for (i = 0; i < map_tcn_nodes_max; i++) {
						map_tcn_node_addr[i] = work;

						if (work == 1) {
							work = 63;
						} else {
							work--;
						}
					}
				} else {

					/*
					   TOPO ERROR
					 */
					p_map_vw->phase = IDLE;

					p_map_vw->code = 0x9999;
					p_map_vw->info = 0x9999;
					err_rec ();

					recorder (0x99);

#ifdef O_960

					RECORD_M_ERR ("Fatal err, node nr == 0 or > MAP_MAX_NODES !");

#endif

					/*
					   TERM_OUT
					 */

#ifdef TERM_OUT

					write_serv ("^Fatal err, node number == 0 or > MAP_MAX_NODES !^");

#endif


				}


				/*
				   TERM_OUT: MAP_TCN_NODE_ADDR
				 */

#ifdef TERM_OUT

				sprintf (map_xbuf, "^MAP-START %02u nodes delivered from TCN^",
						 map_tcn_nodes_max);
				write_serv (map_xbuf);

				for (map_ndx = 0; map_ndx < 80; map_ndx++) {
					map_xbuf[map_ndx] = 0;
				}
				sprintf (map_xbuf,
						 "^MAP-addr( 1-14): %u %u %u %u %u %u %u %u %u %u %u %u %u %u^",
						 map_tcn_node_addr[0],
						 map_tcn_node_addr[1],
						 map_tcn_node_addr[2],
						 map_tcn_node_addr[3],
						 map_tcn_node_addr[4],
						 map_tcn_node_addr[5],
						 map_tcn_node_addr[6],
						 map_tcn_node_addr[7],
						 map_tcn_node_addr[8],
						 map_tcn_node_addr[9],
						 map_tcn_node_addr[10],
						 map_tcn_node_addr[11],
						 map_tcn_node_addr[12],
						 map_tcn_node_addr[13]);
				write_serv (map_xbuf);

				if (map_tcn_nodes_max > 14) {
					sprintf (map_xbuf,
							 "^MAP-addr(15-22): %u %u %u %u %u %u %u %u^",
							 map_tcn_node_addr[14],
							 map_tcn_node_addr[15],
							 map_tcn_node_addr[16],
							 map_tcn_node_addr[17],
							 map_tcn_node_addr[18],
							 map_tcn_node_addr[19],
							 map_tcn_node_addr[20],
							 map_tcn_node_addr[21]);
					write_serv (map_xbuf);
				}
#endif


				/*
				   INITIALIZE THE NADI
				 */

				for (i = 0; i < MAP_MAX_NO_VEH; i++) {
					NADI.vehicle[i] = m_vh;

					map_cc_setup[i].lla = 0;
					map_cc_setup[i].status = 0;
				}
				NADI.state.mdirect = MAP_DIREC_UNKNOWN;
				NADI.state.validity = MAP_NADI_INVALID;
				NADI.state.version = 0;
				NADI.state.ownaddr = 0;
				NADI.state.VehicleNo = 0;

				map_xnadi_ui = NADI;
				map_ynadi_ui = NADI;

				p_map_vw->actual_nadi = 0;	/* the first */


				/*
				   INITIALIZE THE GROUP_LIST
				 */

				for (i = 0; i < MAP_MAX_NO_GRP; i++) {
					GROUP_LIST[i].length = 0;

					map_xgroup_list_ui[i].length = 0;

					map_ygroup_list_ui[i].length = 0;

					for (j = 0; j < MAP_MAX_NO_NGL; j++) {
						GROUP_LIST[i].node[j] = 0;

						map_xgroup_list_ui[i].node[j] = 0;

						map_ygroup_list_ui[i].node[j] = 0;
					}
				}
				p_map_vw->actual_group_list = 0;


				/*
				   INITIALIZE THE ORDER BUFFER
				 */

				for (i = 0; i < MAX_ORDER; i++) {
					map_order.order[i] = 0xFFFF;
					map_order.type[i] = 0;
				}
				map_order.read = 0;
				map_order.write = 0;


				/*
				   RESET SEVERAL DATA
				 */
				p_map_vw->upd_status = MAP_UPDATE_NOT_READY;	/* disable user interface */

				map_MC_to_own_node = 0;		/* map_MC_to_own_node == 0xCCCC if MC to own node to */

				p_map_vw->actual_group_list = 0;
				p_map_vw->code = MAP_OK;
				p_map_vw->info = 0;
				p_map_vw->send_delay = 0;
				p_map_vw->f1st_run_u_wtb = 0;
				p_map_vw->f1st_run_u_mvb = 0;
				map_update_init = 0;

				map_ui_delay_max = DELAY_TIME * map_tcn_nodes_max;

#ifdef O_960

				pi_pend_semaphore (map_sema, PI_FOREVER, &err);

#endif
				map_vw.order_ui = INIT_ORDER;

#ifdef O_960

				pi_post_semaphore (map_sema, &err);

#endif


				/*
				   MVB TOPO CHECK
				 */

				if (map_ci.NodeKonf->device_list.number > MAP_MAX_NO_MVB) {
					p_map_vw->phase = IDLE;

					p_map_vw->code = 0x99D8;
					p_map_vw->info = 0x99D8;
					err_rec ();

					recorder (0xD8);

#ifdef O_960

					RECORD_M_ERR ("Fatal err, MVB count > MAP_MAX_NO_MVB !");

#endif

					/*
					   TERM_OUT
					 */

#ifdef TERM_OUT

					write_serv ("^Fatal err, MVB count > MAP_MAX_NO_MVB !^");

#endif

				}
				if (map_ci.bmi_event == START_INAUGURATION) {
					map_stop ();

					/* disable indication function */
					p_map_vw->f_indication = 0;
					p_map_vw->r_indication = 0;

					p_map_vw->function_step = START_SETUP;
					p_map_vw->phase = SETUP_MODE;

					map_ui_delay = 0;
					map_not_broadcast = TRUE;
				} else {
					p_map_vw->function_step = START_RECOVERY;
					p_map_vw->phase = INCLUDE_MODE;

					map_ui_delay = map_ui_delay_max - DELAY_TIME;
					map_include_repeat = 0;
				}
				break;
			}

		case STOP:
			{
				map_stop ();

#ifdef O_960

				RECORD_M_EVENT ("STOP command received from BMI.");

#endif
				p_map_vw->tstop++;	/* stop event from TCN */

				break;
			}
		}
		map_ci.bmi_event = NO_EVENT;


		/*
		   HANDLE PROGRAM PHASE
		 */
		recorder (R_PHASE);		/* p_map_vw->phase */

		switch (p_map_vw->phase) {

		case SETUP_MODE:
			{

				if ((map_ci.bmi_type == L_STRONG) || (map_ci.bmi_type == L_WEAK)) {

					if (++p_map_vw->send_delay > MASTER_DELAY_TIME) {
						map_setup_master ();
					}
				} else {

					if (map_ci.bmi_type == L_SLAVE) {

						if (++p_map_vw->send_delay > SLAVE_DELAY_TIME) {
							map_setup_slave ();
						}
					}
				}
				break;
			}

		case INCLUDE_MODE:
			{
				map_include_slave ();

				break;
			}

		case UPDATE_MODE:
			{
				map_update ();

				break;
			}
		default:{

				/*
				   IDLE MODE
				 */
			}
		}

	} else {

		/*
		   NP_ERR, no sema from PIL
		 */
	}

	/*
	   HANDLE TERMINAL SERVICES "N" AND "E" (NADI, EVENT_BUFFER)
	 */

	/*
	   Both services are called by a command typed at the terminal.
	   N = show NADI (current version)
	   E = show content of the event_buffer
	   The dialog is realized in mon_serv.c (AEG_FW)
	 */

	if (MAP_SHOW_NADI != 0) {
		map_show_nadi ();
		MAP_SHOW_NADI = 0;
	}

/* <ATR:06> */
//            if ( MAP_EVENT_BUF != 0 )
//    {
//    
//               if ( MAP_EVENT_BUF == 1 )
//    {
//                  /* page 1 */
//                  map_event_loop = 0;
//                  read_event_buffer_to_term();
//    }else{
//    
//                  if ( map_event_loop++ >= 100 )
//    {
//                     /* page 2 */
//                     read_event_buffer_to_term();
//    }
//    }
//    }

}
