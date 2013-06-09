/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> (3) li_t_SetFrame() calls added to update the IDTS (thanks Armin Heindel)           */
/* <ATR:02> SVF_TOPO_RESPONSE_SIZE fixed for UMS (thanks Lutz Reinwald)                         */
/* <ATR:03> TMO_AWAIT_DETECT modified for ADTranz (5) compatibility                             */
/* ATR modifications by Schiavo marked with <ATR:xx>                                            */
/*<ATR:04> 14/06/02 si elim. condizionamento a O_MAC_TRACE                                      */
/*<ATR:05> 14/06/02 si mod. TMO_STOP se mac_state_stop == S_MASTER_AWAIT_PD_RES                 */
/*==============================================================================================*/

/*
MEDIUM ACCESS CONTROL WTB V1.9
iMB, Mike Becker
  3. Oktober 1995,  9:14
*/
/*
GENERAL TOP OF FILE
*/
/*
;=========================================================================
; (c) 1994 ABB Henschel
;-------------------------------------------------------------------------
; project   : TRAIN COMMUNICATION NETWORK
;-------------------------------------------------------------------------
; department: AHE/STL3      author: M.Becker, Ch.Leeb     date: 02.11.1994
;-------------------------------------------------------------------------
; filename  : \TCN\MOD\MAC.30\SOURCE\MAC.X
;-------------------------------------------------------------------------
; function  : medium access control of TCN link layer  version 3.0
;             related to TCN norm Part 4: WIRE TRAIN BUS Version 1.9
;=========================================================================
*/
/*
NOTES
*/

/*
LIST OF CHANGES SINCE DATE OF CREATION
*/
/*
;-------------------------------------------------------------------------
;- nr.  0 -- author: Mike Becker ----- date: 02.11.1994 -- version: 3.0  -
;  in development
;-------------------------------------------------------------------------
; see #Br                                                                -
;=========================================================================


 010195  Le  0.2  created
 030495  Le  0.3  double fault treatment level 1
                  (correct behaviour when 1 node is added)
 090495  Le  0.31 bug fixed which resulted in a composition
                  containing 2 nodes with same address
 300495  Le  0.32 detection added if > one ZAS entry is called

 020695  Le  0.33 correct behaviour if > one ZAS entry is called;
                  random detection period;
                  accept Name.Req only when >= 1 Detect.Req received
                  topo.topo_count is never 0
                  correct md/pd_send_confirm when MAC_BM_Stop is issued

 200695  Le  0.34 double fault treatment level 2;
                  shortening/lengthening disabled when inhibit.
                  flexible base period;
                  nse_record added

 050795  Le  0.35 double fault treatment level 2: some bugs fixed;
                  result returned by d_1/2_rx_disable/enable processed
                  hierarchical initialisation order/
                  own ZAS subscription added
                  d_sleep dummy function removed

 110795  Le  0.40 statistical data added

 110995  Le  0.41 -bug in state S_INCLUSION_PRESENCE removed (must now be
                   intermediate node to re-integrate)
                  -bug in state S_SLAVE_REGULAR / event Topo_Req removed
                   (inauguration key incremented and saved)
 311095 iMB       -porting to ABB 12MX02
                  -inclusion of recorder.h only for O_960
                  -also nse_record/SPrint_F only for O_960
                  -pragma code deleted
                  -pragma argsused deleted
                  -missing prototypes temporarily added
                  -diag_entry(MAC_ERROR_SD,...) changed (pointer extension)
 170796  cs  4.0  follow up of bug fixes that have been detected for version v3.1 and v3.2
                  (1) Sending a Detection_Response even in the case of a Strong
                      Master Conflict in AuxResponse() /cs/
                  (2) Setting the Master_Report in Detection_Response Frames
                      in the S_DETECTION_... states /cs/
                  (4) TMO_AWAIT_RESPONSE incremented from 2 to 3 due to timing
                      problems in case of inclusion  /imB/
                  error fixes
                  (3) changes to MAC to avoid open bus_switch problem:
                      new command MAC_BM_CloseSwitch_Req and
                      indication  BM_MAC_CloseSwitch_Con and
                      new state S_AWAIT_CLOSE_DELAY
                  functional extensions
                  (5) process data in process poll frames
                      master: check for data when sending a PD_Request
                      slave:  indicate data when receiving a PD_Request
                  (6) process data synchronization
                  (7) fritting
                  (7a)  - changing the number of retries from 3 -> 5
                  (7b)  - configuring fritting and
                          switching the fritting source on and off
                  (8) mac_get_info()
                      reporting of the node number where a line error has been detected
                      during inauguration
                      state of the fritting sources
                  (10)sleep mode correction:
                  (10a) - making sure that the bus_switch is open
                  (10b) - making sure that both AMEDs (directions) are attached
  300896 cs 4.1   continuation of 4.0 changes
                  (11) changing the interface to the WTB driver to the original commands
  240996          (12) conditional compilations MAC_TRACE for <mac_scx_cnt> added
                  (13) redundant IF statement removed
                  (14) a Topo_Request frame is now also handled when Master_Topo has not
                       changed
                  (15) missing BREAK-statements added
                  (16) modification of 4.0-3: confirm is sent directly
                  (20) macros write_serv and Sprint_F deleted
                  (25) correction of presence timeout:
                       one needs to take care of the fact that the first poll phase is idle
                       while the next one may be busy
                  (26) correction of poll list algorithm:
                       assuming that to the slaves polled most frequently process data
                       is sent within the poll frame
                  (27) additions to 4.0-2
                  (28) tracing protocol error during detection
 311096 cs 4.1    optimization
                  (31) elimitation of d_X_receive_request() call to driver
                  (32) optimization of the d_X_send_request() call/Send() macro
                  (33) argument of mac_handler(... <line> ...) extended
                  (34) fritting relais (not all changes have been marked)
                  (36) make sure that an End_Node failure is reported every mac_max_status_fail
 270197           (40) d_get_rand() is now called after d_sys_init()
 270197           (44) MAC_ERROR_xxx -> DIAG_MAC_ERR_xxx now in diag.h defined
 060297           (47) mac_my_str is not decremented in case of a failure to avoid an error
                       in the strength value
 170297           (48) counter of basoc_cycles is incremented incorrectly
*/
/*
COMPILER SWITCHES
*/
#ifdef O_960
#include "coco.h"
#endif
/* #pragma code #iMB */

/*
INCLUDE FILES
*/
#include "tcntypes.h"    /*4.0*/
#include "wtbdrv.h"      /* driver */
#include "wtb.h"         /*4.1*/
#include "wtbcpm.h"         /*4.1*/
#if defined(O_960) || defined(O_VCU6U_GW)
#include "pi_sys.h"
#include "pd_jdp.h"
#else
#include "pil.h"
#include "pd_hand.h"
#endif
#include "mmi.h"     /* TCN messenger link layer interface */
#include "diagwtb.h"    /* TCN diagnostic */
#include "zas.h"     /* TCN Zentrale Ablauf Steuerung */
#include "bmi.h"     /* TCN busmanagment interface */
#include "ll.h"      /* TCN link layer internal */
#include "nvm.h"
#ifdef O_960
#include "recorder.h"
#endif
#include "mac.h"      /*4.0-8*/
#ifdef O_MVBC
#include "lc_sys.h"
#endif
#ifdef UMS
#include "ums.h"
#include "adid.h"
#endif
/*
IMPLEMENTATION
*/
/*
DEFINES
*/
/*
Type aliases
*/
#define Byte                 unsigned char
#define Boolean              unsigned char
#define Natural              unsigned short
#define TIME                 unsigned short
#define Type_Cardinal16      unsigned short
#define Type_Dir             unsigned char
#define Type_CompositionEnds unsigned char
#define Type_SendMode        unsigned char
/*
General settings for link layer
*/
/* RESPONSE_TIME used to configure the driver*/
#define RESPONSE_TIME 80   /* in 10 usec */
#define MD_LIST_SIZE (MAX_NODES + 1)
#define SP_LIST_SIZE (MAX_NODES + 1)
#define HIGHEST_NODE_ADDRESS  63
/*
time-outs
*/
/* reply time awaited by master when it sends a status or presence request
   to a slave */
/* <ATR:03> */
/* #define TMO_AWAITDETECT  (1+3) */
#define TMO_AWAITDETECT  (1+4)

/* time awaited before slave opens interruption switch after reception of
   first unname request */
#define TMO_UNNAME       (1+1)
#define TMO_UNNAME_FRITT (2*TMO_UNNAME)                /*4.1-34*/

/* time awaited to avoid interruption switch bouncing */
#define TMO_SWITCH_SLAVE  (8+1)
#define TMO_SWITCH_MASTER (TMO_SWITCH_SLAVE+2)

/* time awaited by naming master to allow a slave switching main channel
   direction. It is the time between reception of the name response
   and sending of the first status request*/
#define TMO_SWITCH_MAIN_DIRECTION  (1+1)
#define TMO_SLAVE_EXP_STATUS       ((6 * BASIC_CYCLE) + 5)
/*#define TMO_SLAVE_EXP_PRESENCE   (BASIC_CYCLE + 5)        4.1-25*/
/*4.1-25: BASIC_CYCLE + process data time + presence exchange + timer inaccurary */
#define TMO_SLAVE_EXP_PRESENCE     (BASIC_CYCLE + 15 + 1 + 1)
#define TMO_END_NODE_CHECK         (7 * BASIC_CYCLE)      /*4.1-25*/
#define TMO_STATUS                 BASIC_CYCLE

/* set to shortest possible delay with MAC Timer but >= RESPONSE_TIME;
used in Inclusion: a slave waits TMO_AWAIT_RESPONSE after
receiving a process data poll frame with his former address.
If no other node responds after TMO_AWAIT_RESPONSE that slave
reintegrates. */
#define TMO_AWAIT_RESPONSE  (1+1+1)       /*4.0-4*/

/* reply time awaited by master between sending the last topography frame and polling of the first node */
#define TMO_TOPO  BASIC_CYCLE

/* time awaited before MAC_BM_STOP_REQ is replied with BM_MAC_STOP_CON. It is at least the time needed for sending the
longest possible frame, since frame sending could be under way at the time of receiving MAC_BM_STOP_REQ  */
#define TMO_STOP (2+1)

/*
constants for calculation of poll list usage
*/
/* Bit stuff factor in [%]
   used in BuildPollList */
#define BIT_STUFF_FACTOR 120
/* Maximal average process data usage of a basic cycle in [%];
   used in BuildPollList */
#define MAX_AVG_PD_USAGE 60
/*  TELEGRAM_OVERHEAD of a process data telegram (request and response)
    in bit times:
    2 * 32 bit times preamble + flags
    2 * 16 * BIT_STUFF_FACTOR(1.2)      bit times frame check sequence
    100 bit times inter frame spacing   (res <-> req)
    320 bit times inter frame spacing   (req <-> res);
    The bit times for the whole telegram calculates as follows:
         TELEGRAM_OVERHEAD + BIT_STUFF_FACTOR * Payload;
    used in BuildPollList */
#define TELEGRAM_OVERHEAD  522
/*  PRESENCE_OVERHEAD:  time taken by the Presence telegram in each
    basic period, in *s:

    Presence.req -> Detect.req -> Detect.res  -> Presence.res
                                  or time-out

Presence.req
    32                                  bit times preamble + flags
    16 * BIT_STUFF_FACTOR(1.2)          bit times frame check sequence
    8 * 8  * BIT_STUFF_FACTOR(1.2)      bit times payload
    320                                 bit times interframe spacing/
                                        reaction time at slave side
    ===
    448

Detect.req
    32                                  bit times preamble + flags
    16 * BIT_STUFF_FACTOR(1.2)          bit times frame check sequence
    6 * 8  * BIT_STUFF_FACTOR(1.2)      bit times payload
    ===
    109

Detect.res time-out
    RESPONSE_TIME

Presence.res
    32                                  bit times preamble + flags
    16 * BIT_STUFF_FACTOR(1.2)          bit times frame check sequence
    6 * 8  * BIT_STUFF_FACTOR(1.2)      bit times payload
    100                                 bit times interframe spacing
    ===
    209

*/
#define PRESENCE_OVERHEAD ( ((1000000/BITRATE) * 766) + RESPONSE_TIME)
/* some special link addresses */
#define ADR_BROADCAST   255
#define ADR_UNNAMED     127
#define ADR_TRANSPARENT   0
#define ADR_MASTER        1
#define ADR_STARTUP       ADR_UNNAMED
#define MAX_TOPO_COUNT 63
/*
definition of bit positions
*/
/* DESCENDING_BIT: used in Name request.YA */
#define DESCENDING_BIT 0x80
#define LINE_STATUS_MASK (~(DA1_BIT | DA2_BIT | DB1_BIT | DB2_BIT))
/* INSIST_BIT : in strength */
#define INSIST_BIT 0x01
/* STRONG_BIT : in strength */
#define STRONG_BIT 0x80
#define DA_BIT     0x01
#define DB_BIT     0x02
#define C12_BIT    0x04
#define INH_BIT    0x08
#define BA_DIR 0
#define TA_DIR 1
/*
retries and time-out counters
*/
/* Max. number of basic cyles the detection service
   waits for nodes before it completes with
   BM_MAC_Detect_Con(MAC_RESULT_DET_NOTHING)
*/
#define MAX_PROMOTED 32
/* After "MAX_YWM_CNT" detection cycles,
   a end node weak master composition which has yielded
  (sending no more detection requests by itself) switches
   back to insist again
*/
#define MAX_YWM_CNT 50
/* number of basic cycles a lone detecting strong master
   waits for a further node on opposite channel if
   it has detected a strong master on one channel
 */
#define MAX_SMC                3
#define MAX_SMC_FRITT          5    /*4.0-7a*/
/* "MAXSTATUSFAIL" times a missing status/presence
    response per direction is tolerated by naming master
    and slave. This yields a worst case slave
    time-out of 2 * 3 * basic cycle = 150ms
*/
#define MAX_STATUS_FAIL        3
#define MAX_STATUS_FAIL_FRITT  5    /*4.0-7a*/
/* maximum number of status requests that a master
   issues per Basic Cycle. Prevents from preemting
   message data traffic when many status requests
   are pending. */

#define MAX_STATUS 4
/* maximum number of retries when master is
   naming or distributing the topography */

#define MAX_TRY                3
#define MAX_TRY_FRITT          5    /*4.0-7a*/
/* maximum number of retries when master is polling
   an intermediate slave for process data. After
   MAX_POLL_TRY that slave's status is set to
   InvalidStatus. */

#define MAX_POLL_TRY           3
#define MAX_POLL_TRY_FRITT     5    /*4.0-7a*/
/*
Frame types , encoding
*/
/*
data frames
*/
#define PD_REQUEST              0x80  /* 10AC I000 */
#define PD_RESPONSE             0x00  /* 00AC I000 */

#define PD_RESPONSE_A           (0x00 | A_BIT)  /* 001C I000 */

#define MD_REQUEST              0x87  /* 10AC I111 */
#define MD_RESPONSE             0x07  /* 00AC I111 */

#define MD_RESPONSE_A           (0x07 | A_BIT)  /* 001C I111 */
/*
supervisory frames
*/

#define SVF_DETECT_REQUEST      0xC0  /* 1100 0000 */
#define SVF_DETECT_RESPONSE     0x40  /* 0100 0000 */
#define SVF_STATUS_REQUEST      0xC1  /* 1100 0001 */
#define SVF_STATUS_RESPONSE     0x41  /* 0100 0001 */
#define SVF_SETINT_REQUEST      0xC2  /* 1100 0010 */
#define SVF_SETINT_RESPONSE     0x42  /* 0100 0010 */
#define SVF_SETEND_REQUEST      0xC3  /* 1100 0011 */
#define SVF_SETEND_RESPONSE     0x43  /* 0100 0011 */
#define SVF_UNNAME_REQUEST      0xC4  /* 1100 0100 */
#define SVF_NAME_REQUEST        0xC5  /* 1100 0101 */
#define SVF_NAME_RESPONSE       0x45  /* 0100 0101 */
#define SVF_TOPO_REQUEST        0xC6  /* 1100 0110 */
#define SVF_TOPO_RESPONSE       0x46  /* 0100 0110 */
#define SVF_PRESENCE_REQUEST    0xC7  /* 1100 I111 */
#define SVF_PRESENCE_RESPONSE   0x47  /* 010R I111 */

/*
Link frame sizes of the various frame types
*/
#define PD_REQUEST_SIZE              0
/* PD_RESPONSE has variable size */
#define MD_REQUEST_SIZE              0
/* MD_RESPONSE has variable size */

#define SVF_DETECT_REQUEST_SIZE      2
#define SVF_DETECT_RESPONSE_SIZE     2
#define SVF_STATUS_REQUEST_SIZE      2
#define SVF_STATUS_RESPONSE_SIZE     8
#define SVF_SETINT_REQUEST_SIZE      0
#define SVF_SETINT_RESPONSE_SIZE     0
#define SVF_SETEND_REQUEST_SIZE      2
#define SVF_SETEND_RESPONSE_SIZE     0
#define SVF_UNNAME_REQUEST_SIZE      0
#define SVF_NAME_REQUEST_SIZE        2
#define SVF_NAME_RESPONSE_SIZE       0
/* SVF_TOPO_REQUEST has variable size */
/* <ATR:02> */
/* #define SVF_TOPO_RESPONSE_SIZE       0 */
#ifdef UMS
#define SVF_TOPO_RESPONSE_SIZE 128
#else
#define SVF_TOPO_RESPONSE_SIZE 0
#endif
#define SVF_PRESENCE_REQUEST_SIZE    4
#define SVF_PRESENCE_RESPONSE_SIZE   2
/*
Position of parameters in data field of frame
*/
/* Position of parameters in data field of frame.
   They are equal throughout all frames except
   for the topo request frame */

#define POS_LS 1  /* local strength */
#define POS_MR 0  /* master report in Detect.Req/Res,
                                      Presence.Req,
                                      Status.Req*/
#define POS_YA 0  /* your address */
#define POS_NR 0  /* node report */
#define POS_RS 1  /* remote strength */
#define POS_UR 3  /* status report */
#define POS_NF 4  /* node frame size */
#define POS_NP 5  /* node poll */
#define POS_NT 6  /* node type */
#define POS_NV 7  /* node version */

#define POS_BA 0  /* bottom address (address of bottom node) */

#define POS_MT_L 3  /* node id low byte */
#define POS_MT_H__YP 2  /* node id high byte (requested nodeperiod) */
#define POS_NK_OFFSET 4


/* Position of reserved field in
    SVF_PRESENCE_REQUEST;
    SVF_SETEND_REQUEST  */
#define POS_RES   0
/* Position of reserved field in
    SVF_STATUS_RESPONSE; */
#define POS_RES_S 2

#define RESERVED_FIELD_VALUE 0
/*
States
*/
#define S_UNCONFIGURED                             0x01
#define S_IDLE                                     0x02
#define S_DETECT_AWAIT_SWITCH                      0x03
#define S_DETECTION_INSIST_REQ                     0x04
#define S_DETECTION_INSIST_RES                     0x05
#define S_DETECTION_YIELD                          0x06
#define S_DETECTION_YIELD_ACCEPT_NAME              0x07
#define S_DETECTION_STOP_SMC                       0x08
#define S_DETECTION_STOP_INSIST                    0x09
#define S_INHIBIT                                  0x0A
#define S_INCLUSION_PRESENCE                       0x0B
#define S_INCLUSION_PD_REQ                         0x0C
#define S_INCLUSION_PD_RES                         0x0D
#define S_UNNAME_AWAIT_SC                          0x0E
#define S_AWAIT_STOP_DELAY                         0x0F
#define S_AWAIT_CLOSE_DELAY                        0x10  /*4.0-3*/
/*
4.0-3: for the following states the state value has been incremende by 1
*/
/* !!! For the following states applies:
   S_NAMING_AWAIT_INT_RES must be the first state,
   S_MASTER_PRESENCE_AWAIT_SC must be the last state, and
   the state ID's must be contiguous !!! */
#define S_NAMING_AWAIT_INT_RES                     0x11
#define S_NAMING_WAIT_FOR_MASTER_CLOSING_SWITCH    0x12
#define S_NAMING_AWAIT_NAME_RES                    0x13
#define S_NAMING_AWAIT_DELAY                       0x14
#define S_NAMING_WAIT_FOR_MASTER_OPENING_SWITCH    0x15
#define S_NAMING_AWAIT_END_RES                     0x16
#define S_NAMING_WAIT_FOR_SLAVE_OPENING_SWITCH     0x17
#define S_NAMING_AWAIT_STATUS_RES                  0x18
#define S_NAMING_AWAIT_DET_RES                     0x19
#define S_SLAVE_REGULAR                            0x1A
#define S_SLAVE_TOPO                               0x1B
#define S_SLAVE_AWAIT_SC_INT_RES                   0x1C
#define S_SLAVE_AWAIT_SC_END_RES                   0x1D
#define S_SLAVE_UNNAME                             0x1E
#define S_SLAVE_CLOSESWITCH                        0x1F
#define S_SLAVE_OPENSWITCH                         0x20
#define S_SLAVE_TOPO_AWAIT_DETECT_RES              0x21
#define S_SLAVE_REGULAR_AWAIT_DETECT_RES           0x22
#define S_SLAVE_AWAIT_SC_PD_RES                    0x23
#define S_SLAVE_AWAIT_SC_MD_RES                    0x24
#define S_MASTER_AWAIT_PRESENCE_RES                0x25
#define S_MASTER_AWAIT_DET_RES                     0x26
#define S_MASTER_AWAIT_TOPO_RES                    0x27
#define S_MASTER_AWAIT_TOPO_DELAY                  0x28
#define S_MASTER_AWAIT_STATUS_RES                  0x29
#define S_MASTER_AWAIT_MD_RES                      0x2A
#define S_MASTER_AWAIT_PD_RES                      0x2B
#define S_MASTER_IDLE_PHASE                        0x2C
#define S_MASTER_AWAIT_SC_PD_RES                   0x2D
#define S_MASTER_AWAIT_SC_MD_RES                   0x2E
#define S_MASTER_AWAIT_SC_PD_REQ                   0x2F
#define S_MASTER_AWAIT_SC_MD_REQ                   0x30
#define S_MASTER_AWAIT_SC_PRESENCE_RES             0x31
/*
Macros
*/
#define SYSTIME ((unsigned short) d_tc_get_systime())
/*
ISGREATER
*/
#define LONGEST_DELAY 1000
/*
if (a>b) then
   if (a-b) > LONGEST_DELAY then FALSE  (Case 1)
                            else TRUE   (Case 2)
else
   if (b-a) > LONGEST_DELAY then TRUE   (Case 3)
                            else FALSE  (Case 4)


<--unsigned short -->    "a>b" yields

|--A--------------B--|  ->  TRUE        (Case 3)
|--B--------------A--|  ->  FALSE       (Case 1)
|-------A-B----------|  ->  FALSE       (Case 4)
|-------B-A----------|  ->  TRUE        (Case 2)
*/
#define ISGREATER(a,b) ((a>b) ? (((a-b)>LONGEST_DELAY) ? FALSE:TRUE) : (((b-a)>LONGEST_DELAY) ? TRUE:FALSE))
#define MAC_DIR1       0
#define MAC_DIR2       1
#define OPPOSITE_OF(x) (1 - x)
#define MAC_DIR12      3                           /*4.0-7b*/
/*4.0-7b*/
#define MAC_DIR_TO_BITFIELD( dir)       ((dir == MAC_DIR1)  ? 0x01 : \
                                        ((dir == MAC_DIR2)  ? 0x02 : \
                                        ((dir == MAC_DIR12) ? 0x03 : 0x00) ) )
#define SET_SC_FLAG \
((mac_main_direction == MAC_DIR1) ? (mac_SC_Flag1 = TRUE) : (mac_SC_Flag2 = TRUE))

#define CLEAR_SC_FLAG \
((mac_main_direction == MAC_DIR1) ? (mac_SC_Flag1 = FALSE) : (mac_SC_Flag2 = FALSE))
/*
void Send(Type_Dir TheChannel, type_idu_drv *Frame, Type_SendMode Mode )
*/
/*4.1-32..
#define Send(TheChannel, Frame, Mode) \
  if (TheChannel == MAC_DIR1)\
  {\
          if((mac_drv_err = d_1_send_request(Frame, Mode)) != D_OK)\
              mac_shutdown(DIAG_MAC_ERR_SEND_REQ1,\
                           mac_drv_err);\
  }\
  else\
  {\
          if((mac_drv_err = d_2_send_request(Frame, Mode)) != D_OK)\
              mac_shutdown(DIAG_MAC_ERR_SEND_REQ2,\
                           mac_drv_err);\
  }
..4.1-32*/
/*4.1-32: channel has to be D_DIRECT_1 or D_DIRECT_2 */
#define Send( channel, frame, mode) \
  if ((mac_drv_err = d_send_request( channel, frame, mode)) != D_OK)\
     mac_shutdown(DIAG_MAC_ERR_SEND_REQ, mac_drv_err| (channel << 8) )
/*
Random number generator
*/
#define KONST_M   ((unsigned short)11373)
#define KONST_K   ((unsigned short)13849)
/*
Line Redundancy
*/
/* minimal number of frames that have to be received on both lines on the auxiliary channel after a double fault
condition was recognized until the double fault is taken as removed.
This makes the double fault recognition more robust against crosstask between line A and B indicating erroneously two
healthy lines.*/
#define MIN_GOOD_FRMS 5
/*
TYPEDEFS
*/
typedef unsigned char Type_ByteArray[MAX_NODES] ;
#define HNA HIGHEST_NODE_ADDRESS
/* index 0 is unused !*/
typedef Type_NodeStatus     Type_NodeAddr2NodeStatusArray [HNA+1] ;
/* index 0 is unused !*/
typedef unsigned char       Type_NodeAddr2NodeIxArray [HNA+1] ;
/* index 0 is unused !*/
typedef Type_NodeDescriptor Type_NodeAddr2NodeDescArray [HNA+1] ;
/*4.1-34*/
typedef struct
    {  type_idu_drv *rcv_frame;
       UNSIGNED8    line;
       UNSIGNED8    sub_event;
    }Type_SaveEvent;

/*
Types for Naming
*/
enum Type_NamingJob { Get_Status, Name, TrySetInt } ;
typedef struct
{
  unsigned char        Node ;
  unsigned char        Descend ;
  unsigned char        Cnt ;
  unsigned char        FirstStatus ;
  unsigned short       AwakeTime ;
  enum Type_NamingJob  Job ;
} Type_NamingChannelContext ;
typedef Type_NamingChannelContext Type_NamingContext [2] ;
/*
Types for Master
*/
typedef struct
{
  unsigned char NA; /* node address */
  unsigned char IP; /* individual period in multiples of basic cycle */
  unsigned char IC; /* individual counter */
  unsigned char EC; /* error counter */
} Type_PollEntry;
typedef  Type_PollEntry Type_PollList [MAX_NODES];
typedef  unsigned char Type_BooleanArray [MAX_NODES];
typedef  unsigned char Type_EndNodePresence [2];
/*
Types for Slave
*/
typedef unsigned char Type_PresenceTmo [2];
/*
DECLARATIONS
*/
/*
trace variables
*/
//#ifdef O_MAC_TRACE /*<ATR:04>*/
unsigned char  mac_trace[1024] ;
unsigned short  mac_trace_ndx  ;

unsigned char  mac_trigger[8] ;

unsigned short mac_drv1_cnt ;
unsigned short mac_drv2_cnt ;
unsigned short mac_sc1_cnt ;
unsigned short mac_sc2_cnt ;
static unsigned short ch1_disabled ; /*<ATR:04>*/
static unsigned short ch2_disabled ; /*<ATR:04>*/
//#endif /*<ATR:04>*/
unsigned char mac_state  ;     /* actual state of MAC state machine */
unsigned char mac_event ;      /* actual event of MAC state machine */
unsigned char mac_sub_event ;  /* actual sub event of MAC state machine */
Boolean       mac_busy   ;     /* busy flag of MAC */
unsigned char mac_stop_state  ; /* remembers the state when a
                                   MAC_BM_Stop_Req is issued to do late
                                   PD/MD-Confirmation */
type_idu_drv * mac_rcv_frame ;
unsigned char mac_line ;     /* corresponding receive line */
unsigned char mac_drv_err ;
unsigned char mac_SC_Flag1 ;
unsigned char mac_SC_Flag2 ;
unsigned char mac_UDF_SC_Flag ;
unsigned char mac_UDF_ZAS_Flag ;
unsigned char mac_UDF_TIMER1_Flag ;
unsigned char mac_UDF_TIMER2_Flag ;
signed short mac_current_random_number ;  /* used in random number generator */
Type_ZASElement * mac_p2msg ;  /* pointer to message from ZAS */
Type_ZASElement   mac_msg2bm ; /* message to BM */
Type_MACStatistic *p_statistic_data;
/*
variable declaration taken from SDL specification
*/
static Type_WTBStatus *   p_WTB_status ;
static Type_WTBNodes  *   p_WTB_nodes ;
/* MAC data which is accessible by Bus Manager
   Implementation: Use locking for access */
static unsigned short mac_count;
 extern unsigned char zas_count;        /* Lifesign of ZAS (Counter)            */
Type_Topography      mac_topo ;
Type_Configuration   mac_configuration ;
Type_Cardinal16      mac_master_topo ;
Type_NodeStatus InvalidStatus ;  /* these "constants" are set in                                               mac_init"*/
Type_NodeStatus PowerUpStatus ;
Type_Dir mac_main_direction ;
Byte mac_my_str ; /* own strength */
Byte mac_CI_bits ; /* C-Bit and I-Bit used by master and slave */
 /* remembers last node address if node was slave to calculate the delay
    until promotion of a weak master */
Byte mac_last_node_addr ;
/*
communication main <-> auxiliary channel
*/
/* strength of remote composition, updated by auxiliary channel */
Byte mac_aux_RS ;
unsigned char mac_aux_NS ;
/* Prevents detecting node from sending detection requests after yielding.
Used in detection service and procedure AuxResponse */
Boolean mac_insist ;
/*
S_NAMING_ and S_MASTER states
*/
/* array of the node descriptors of all nodes at master. Index is node
   address. Set in naming and updated with MAC_BM_ChangeNodeDesc
   Used in states NAMING_ and MASTER_  */
Type_NodeAddr2NodeDescArray mac_ND_array ;
/* Array of Node stati of all nodes at master, received when naming.
   Index is node address. Built during Naming.
   Used in states NAMING_ and MASTER_ */
Type_NodeAddr2NodeStatusArray mac_NS_array ;
/* Flag at master: set (TRUE) after naming and cleared (FALSE) after first
   topo distribution. Only at first topo distribution the contents of
   mac_NS_array are copied to WTB_nodes */
Boolean mac_first_topo ;
/* set in NamingInit
   Used in states NAMING_ and MASTER_ */
unsigned char mac_master_rank ;
/* current direction: used in naming service and in MasterCheckEndNodes*/
Type_Dir mac_cur_dir ;
type_idu_drv SndFrm,           /* general purpose for main direction */
             PD_req_frm,       /* master's PD.Req
                                  SD/LLC/SIZE preset in mac_init */
             MD_req_frm,       /* master's MD.Req
                                  SD/LLC/SIZE preset in mac_init */
             Pre_req_frm,      /* master's Presence.Req
                                  SD/LLC/SIZE/LS/MT_L/MT_H__YP preset
                                  in MasterPrepare */
             Pre_res_frm,      /* Presence.Res (master and slave)
                                  SD/SIZE/LS preset
                                  in StartMaster/StartSlave */
             Topo_res_frm,     /* Topography.Res */
             Det_req_frm,      /* Detect.Req for aux direction
                                  DD/SD/LLC/SIZE preset in mac_init */
             aux_frm,          /* Detect.Res in function aux_response */
             frm1,             /* Detect.Req in DETECT_.. states*/
             frm2 ;
/* temporary remote strength, used instead of parameter to functions
Naming_Process_Status_Res,
Master_Process_Status_Res,
Master_Process_Presence_Res
*/
Byte    mac_tmp_RS ;
Boolean mac_tmp_remote_inhibit ;
type_idu_drv * p2_TmpFrm ;    /* frame pointer used with PD and MD                                          handler in
matser and slave's
                                 regular operation */
/*
S_INCLUSION_ states
*/
/* set in state S_INCLUSION_PRESENCE and later on used in InclusionFail() to decide whether we disrupt the bus or we
go to inhibit state */
Byte mac_tmp_LLC ;
Type_SaveData mac_tmp_SV ;  /* set in Inclusion_Req, used in
                               S_INCLUSION_PRESENCE state*/
/* used in Inclusion */
Boolean mac_can_recover ;
/* yielding weak master count. When 0 a yielding weak master's composition
   starts sending detection requests again. */
Natural mac_YWM_cnt ;
/*
S_DETECTION_ states
*/
/* TRUE if lone yielding weak master */
Boolean mac_lone_YWM ;
/* Counter: When 0 a lone yielding weak master starts sending detection
  requests again. */
Natural mac_lone_YWM_cnt ;
/* after mac_slave_cnt_max basic cycles the detection service returns after if
   it was started as slave (strength 0). mac_slave_cnt_max depends on the node's
   former address */
Natural mac_slave_cnt_max ;
/* Counter: When 0 then detection service returns with 0
   (nothing detected) */
Natural mac_slave_cnt ;
/* Strong master conflict count:
   When 0 detection service reports strong master conflict */
Natural mac_SMC_cnt ;
/*
S_NAMING_ states
*/
Type_NamingContext mac_ct ;
Byte    mac_tmp_NR ; /* used in S_NAMING_AWAIT_STATUS_RES */
/*
S_SLAVE_ states
*/
/* Is FALSE until the first presence frame from an end node is
   received/sent. From then the slave knowns from which direction it has
   to expect a precence frame */
Boolean mac_presence_startup ;
/* Presence time-out counter for the two directions */
Type_PresenceTmo mac_presence_tmo ;
/* expected direction from which the slave expects a detection frame after
   mac_presence_startup is TRUE */
Type_CompositionEnds mac_exp_dir ;
/*
S_MASTER_ states
*/
/* Counter for retries */
Natural mac_retry ;
/* temporary variable, used instead of parameter to function MasterSendTopoReq */
Byte mac_tmp_MT_H__YP ;
Type_PollList mac_poll_list ;
/* 1. Current index during Topography distribution;
   2. Current polllist index in PDPhase. Runs from 0 to no_of_nodes */
Natural mac_ix ;
/* temporary node index */
Natural mac_NIx ;
/* Counter for each end node. When 0 in either direction the master
   reports BM_MAC_MasterEndNodeFail_Ind */
Type_EndNodePresence mac_present ;
TIME mac_cycle_start_time ;
TIME mac_time_account ;
/* OR-Combination over mac_inh_array,calculated in SleepInhibit*/
Boolean mac_ORed_inhibit ;
/* Counter: Is set to MAX_STATUS at the begin of each Basic cycle.
   Counts to number of remaining status request allowed to poll in this
   basic cycle */
Natural mac_status_cnt ;
/* Array of node addresses of all nodes at master. Index
   0..(number_of_nodes-1), ordered from bottom to top node.
   Built in MasterBuildTopo. */
Type_ByteArray mac_NA_array ;
/* Array of node periods of all nodes at master.
   Index 0..(number_of_nodes-1), ordered from bottom to top node.
   Built first in MasterBuildTopo by copying node period from mac_ND_array and
   then possibly adjusted by MasterBuildTopo if process data usage
   conditions are violated. */
Type_ByteArray mac_NP_array ;
/* Array of Inhibit-Bits of all nodes at master.
   Index 0..(number_of_nodes-1), ordered from bottom to top node.
   Cleared in MasterBuildTopo and updated whenever a process data poll
   frame is received. */
Type_BooleanArray mac_inh_array ;
/* Array of Sleep-Requests of all nodes at master.
   Index 0..(number_of_nodes-1), ordered from bottom to top node.
   Cleared in MasterBuildTopo and updated whenever a status response
   frame is received. */
Type_BooleanArray mac_slp_array ;
/* Array of node indices of all nodes at master. Array Index is node
   address. It is a translation table converting a node address to the
   according index of the Array WTB_nodes, mac_NA_array,...
                    Index  = node address;
                    Result = Node index.
   Built in MasterBuildTopo. */
Type_NodeAddr2NodeIxArray mac_NIx_array ;
Byte mac_late_confirm ;
/*
variables for line redundancy
*/
unsigned char mac_last_line_status ;
/* Used by master during inauguration: mac_main_A/B_ok is set to FALSE each time the master has named a node and starts
polling it with Status.Req. After each Status.Res (or time-out) the master's and slave's main channel line bits are
processed to find out if line A or/and line B is healthy. If yes, the according mac_main_A/B_ok is set to TRUE. */

Boolean mac_main_A_ok;
Boolean mac_main_B_ok;
/* used my master during inauguration when a Status.Res is only received on one line. */
Boolean mac_got_status_response;
unsigned char line_status_update_counter ;
unsigned char mac_MR ;
unsigned char  mac_DF_count ;
/* double fault was recognized by the auxiliary channel, now MIN_GOOD_FRMS must be received again until double fault
condition is removed. See also comment at definition of MIN_GOOD_FRMS.*/
Boolean        mac_DF_flag ;
/*
initialization variables for retry counters                  4.0-7a
*/
/*4.0-7a*/
static unsigned short mac_max_smc;
static unsigned short mac_max_status_fail;
static unsigned short mac_max_try;
static unsigned short mac_max_poll_try;

#ifdef UMS
#define MAX_UIC_REPEAT 3
int uic_repeat;
Boolean wtb_restricted;
Boolean wtb_topo_node;
int topo_freeze;
#endif

/*4.0-7b*/
Type_Dir         mac_fritt_source_on;   /* bitfield indicating which direction has the
                                         fritting source on */
unsigned char    mac_addr_inaug_line_error;                /*4.0-8*/
/*4.1-31*/
Type_SaveEvent   mac_save_event[2];
/*
PROTOTYPES OF LOCAL FUNCTIONS
*/
/* external function */
#ifdef O_960 /*4.1-20*/
void SPrint_F( char*, char*, ...);
#endif
/* local functions */
static void mac_handler( void);
static void mac_timer1_entry(void) ;
static void mac_timer2_entry(void) ;
static void mac_driver1_sc(type_idu_drv *p_idu, unsigned char send_state) ;
static void mac_driver2_sc(type_idu_drv *p_idu, unsigned char send_state) ;
static void mac_general_events(void) ;
static void mac_error(void);
/*4.1-31*/
void mac_rcv1_evt(    type_idu_drv *rcv_frame,
                      UNSIGNED8    line,
                      UNSIGNED8    sub_event
                      );
void mac_rcv2_evt(    type_idu_drv *rcv_frame,
                      UNSIGNED8    line,
                      UNSIGNED8    sub_event
                      );
static void scan_WTB_nodes (void) ;
static void update_line_status_bits (void) ;
static void mac_shutdown (unsigned short reason, unsigned short param) ;
static void          set_random_seed  (signed short seed);
static signed short  get_random_number (void);
/*4.0-7b*/
static void mac_fritting_on(
                 Type_Dir       direction);
static void mac_fritting_off(
                 Type_Dir       direction);
static void mac_fritting_test_main(
                 unsigned char  line_status);
/*
SDL Procedures
*/
static Boolean SendDetectReqProlog(void) ;
static void SendDetectReqEpilog(void) ;
static void SendDetectReq12(void) ;
static void MasterPrepare(void) ;
static void MasterSendTopoReq(void) ;
static void SendPresenceRes(void) ;
static void IncAddr(void) ;
static void SendNameReq(void) ;
static void SendStatusReq(void) ;

static void Master_AP_Phase(void) ;
static void Master_Prepare_Topo(void) ;
static void Master_Basic_Cycle(void) ;
static void Master_PD_Phase(void) ;
static void Master_Process_Presence_Res(void) ;
static void Naming_Process_Status_Res(void) ;
static void Naming_Schedule(void) ;
static void Naming_Schedule_Resume(void) ;
static void Naming_Check_End_Of_Naming(void) ;
static void InclusionFail(void) ;
static void InclusionSuccess(void) ;
static void AuxResponse(void) ;
static void Enable(Type_Dir TheChannel, Byte TheAddress) ;
static void Disable(Type_Dir TheChannel) ;
static Boolean BuildPollList(void) ;
/* List operations */

static void AddMDList (unsigned char address) ;
static unsigned char GetMDList(void) ;
static void ClearMDList(void) ;
static void AddSPList (unsigned char address) ;
static unsigned char GetSPList(void) ;
static void ClearSPList(void) ;
static unsigned char IsEmptyMDList (void);
static unsigned char IsEmptySPList (void);
#ifdef UMS
static void MasterSendTopoResp(void);
#endif
void TopoPhaseEnd(void);

/*
void mac_handler()
                     M A C   S T A T E   M A C H I N E
*/
void mac_handler ( void )
{
  /*
  variable declarations
  */
  static unsigned short mac_trace_ndx_old;
  
  /* temporary address in Master, Slave MD polling */
  Byte          mac_MD_addr ;
  /* temporary line status */
  unsigned char tmp_status ;
  pi_disable() ; /* lock critical area */
  if (mac_busy)
  {
    /*already running*/
    pi_enable() ;
  }
  else
  {
    mac_busy = TRUE ;
    do{
      /*
      resolve events
      */
      if (mac_save_event[0].sub_event==D_RPT_NO_ENTRY)/*4.1-31*/
      {
        if (mac_save_event[1].sub_event==D_RPT_NO_ENTRY)/*4.1-31*/
        {
          /*
          other events
          */
          if (mac_UDF_SC_Flag)
          {
            pi_enable() ;
            mac_UDF_SC_Flag = FALSE ;
            mac_event = SC_DIR12;
          }
          else
          {
            if (mac_UDF_TIMER1_Flag)
            {
              pi_enable() ;
              mac_UDF_TIMER1_Flag = FALSE ;
              mac_event = UDF_TIMER1 ;
            }
            else
            {
              if (mac_UDF_TIMER2_Flag)
              {
                pi_enable() ;
                mac_UDF_TIMER2_Flag = FALSE ;
                mac_event = UDF_TIMER2 ;
              }
              else
              {
                if (mac_UDF_ZAS_Flag)
                {
                  pi_enable() ;
                  mac_event = mac_p2msg->enr ;
                }
                else
                {
                  mac_busy = FALSE ;
                  pi_enable() ;
                  break ;
                }
              }
            }
          }
        }
        else
        {
          /*4.1-31*/
          mac_event     = UDF_DIR2;
          mac_sub_event = mac_save_event[1].sub_event;
          mac_rcv_frame = mac_save_event[1].rcv_frame;
          mac_line      = mac_save_event[1].line;
          mac_save_event[1].sub_event = D_RPT_NO_ENTRY;
          pi_enable() ;
        }
      }
      else
      {


        /*4.1-31*/
        mac_event     = UDF_DIR1;
        mac_sub_event = mac_save_event[0].sub_event;
        mac_rcv_frame = mac_save_event[0].rcv_frame;
        mac_line      = mac_save_event[0].line;
        mac_save_event[0].sub_event = D_RPT_NO_ENTRY;
        pi_enable() ;
      }
      /*
         M A C   S T A T E S
      */
      /*
      trace state and event
      */
//#ifdef O_MAC_TRACE /*<ATR:04>*/
if (mac_trace_ndx>1016)
	mac_trace_ndx = 0;
	
      mac_trace[mac_trace_ndx++] = mac_drv_err ; /*<ATR:04>*/
      mac_trace[mac_trace_ndx++] = ch2_disabled; /*<ATR:04>*/
      mac_trace[mac_trace_ndx++] = mac_state ;
      mac_trace[mac_trace_ndx++] = mac_event ;
      mac_trace[mac_trace_ndx++] = zas_count ;//num retry
     mac_trace[mac_trace_ndx++] = ch1_disabled ;//num retry
     mac_trace[mac_trace_ndx++] =mac_count ;//num retry
     mac_trace[mac_trace_ndx++] =0xff ;//next state
     mac_count++;
//#endif /*<ATR:04>*/
      switch (mac_state)
      {
        /*
        S_UNCONFIGURED
        */
        case S_UNCONFIGURED :
        {
          if (mac_event == MAC_BM_Configure_Req)
          {
            /* copy the configuration given by p34 */
            mac_configuration = *((Type_Configuration*)(mac_p2msg->p34)) ;
            p_WTB_status->node_descriptor = mac_configuration.node_descriptor;
            /*
            install timer call back function
            */
            d_tc_timer_subscribe( MAC_TIMER1, mac_timer1_entry, D_MODE_NONCYCLIC ) ;
            d_tc_timer_control( MAC_TIMER1, 1 ) ; /* proc. call after timeout */
            d_tc_timer_subscribe( MAC_TIMER2, mac_timer2_entry, D_MODE_NONCYCLIC ) ;
            d_tc_timer_control( MAC_TIMER2, 1 ) ; /* proc. call after timeout */
                                                  /*4.0-7a*/
            if (mac_configuration.fritting_disabled == TRUE)
            {
              mac_max_smc         = MAX_SMC;
              mac_max_status_fail = MAX_STATUS_FAIL;
              mac_max_try         = MAX_TRY;
              mac_max_poll_try    = MAX_POLL_TRY;
              /*
              configure no fritting     4.0-7b
              */
              /*4.1-11*/
              d_sw_frittung_control( D_FRITTING_OFF, D_DIRECT_1, D_LINE_A);
              d_sw_frittung_control( D_FRITTING_OFF, D_DIRECT_1, D_LINE_B);
              d_sw_frittung_control( D_FRITTING_OFF, D_DIRECT_2, D_LINE_A);
              d_sw_frittung_control( D_FRITTING_OFF, D_DIRECT_2, D_LINE_B);
            }
            else
            {
              mac_max_smc         = MAX_SMC_FRITT;
              mac_max_status_fail = MAX_STATUS_FAIL_FRITT;
              mac_max_try         = MAX_TRY_FRITT;
              mac_max_poll_try    = MAX_POLL_TRY_FRITT;
              /*
              configure fritting     4.0-7b
              */
              /*4.1-11*/
              d_sw_frittung_control( D_FRITTING_PASSIVE, D_DIRECT_1, D_LINE_A);
              d_sw_frittung_control( D_FRITTING_PASSIVE, D_DIRECT_1, D_LINE_B);
              d_sw_frittung_control( D_FRITTING_PASSIVE, D_DIRECT_2, D_LINE_A);
              d_sw_frittung_control( D_FRITTING_PASSIVE, D_DIRECT_2, D_LINE_B);
            }
            /* confirm the request */
            mac_msg2bm.enr = BM_MAC_Configure_Con ;
            zas_send(WSNR_BM, &mac_msg2bm) ;
            mac_state = S_IDLE ;
          }
          else
          {
            mac_general_events();
          }
          break;
        }
        /*
        S_IDLE
        */
        case S_IDLE :
        {
          switch (mac_event)
          {
            /*
            MAC_BM_Inhibit_Req
            */
            case MAC_BM_Inhibit_Req:
            {
              d_sw_direction(D_DIRECT_1) ;
              d_1_install(mac_driver1_sc, mac_rcv1_evt/*4.1-31*/ , BITRATE, RESPONSE_TIME) ;
              d_2_install(mac_driver2_sc, mac_rcv2_evt/*4.1-31*/ , BITRATE, RESPONSE_TIME) ;
              Enable(MAC_DIR1, ADR_MASTER) ;
              d_tc_timer_reset(MAC_TIMER1, (mac_max_try /*4.0-7a*/ * (BASIC_CYCLE + 1))) ;
              mac_state = S_INHIBIT ;
              break;
            }
            /*
            MAC_BM_Inclusion_Req
            */
            case MAC_BM_Inclusion_Req:
            {
              /*  if node is end node do not try to sense bus traffic */
              if (p_WTB_status->node_status.node_report & INT_BIT)
              {
                if ((Type_SaveData*)(mac_p2msg->p34) == (void*)0 )
                {
                  mac_can_recover = FALSE ;
                }
                else
                {
                  mac_can_recover = TRUE ;
                  mac_tmp_SV = *((Type_SaveData*)(mac_p2msg->p34)) ;
                }
                d_sw_direction(D_DIRECT_1) ;
                d_1_install(mac_driver1_sc, mac_rcv1_evt/*4.1-31*/, BITRATE, RESPONSE_TIME);
                d_2_install(mac_driver2_sc, mac_rcv2_evt/*4.1-31*/, BITRATE, RESPONSE_TIME);
                /* attach direction 1, detach unused direction 2 */
                d_sw_line_attachment(
                   /* 1A */ D_CLOSE,
                   /* 1B */ D_CLOSE,
                   /* 2A */ D_OPEN,
                   /* 2B */ D_OPEN) ;
                Enable(MAC_DIR1,ADR_TRANSPARENT) ;
                /* Find a presence response during MAX_TRY * BASIC_CYCLE */
                d_tc_timer_reset(MAC_TIMER1, BASIC_CYCLE * mac_max_try /*4.0-7a*/) ;
                mac_state = S_INCLUSION_PRESENCE ;
              }
              else
              {
                /*
                BM_MAC_Inclusion_Con(MAC_RESULT_INC_DISRUPT)
                */
                mac_msg2bm.enr = BM_MAC_Inclusion_Con ;
                mac_msg2bm.p1  = (unsigned short) MAC_RESULT_INC_DISRUPT ;
                zas_send(WSNR_BM, &mac_msg2bm) ;
                mac_state = S_IDLE ;
              }
              break;
            }
            /*
            MAC_BM_Detect_Req
            */
            case MAC_BM_Detect_Req:
            {
              mac_my_str = (Byte) mac_p2msg->p1 ;
              /*
              set some aux channel variables and WTB_status
              */
              mac_lone_YWM = FALSE;
              mac_aux_RS   = 0;
              mac_SMC_cnt  = mac_max_smc;      /*4.0-7a*/
              p_WTB_status->node_address  = ADR_UNNAMED ;
              p_WTB_status->net_inhibit   = FALSE;
              p_WTB_status->node_strength = L_UNDEFINED ;
              p_WTB_status->node_orient   = L_UNKNOWN ;
              /* clear line status bits in node_report */
              pi_disable() ;
              p_WTB_status->node_status.node_report &=  LINE_STATUS_MASK;
              pi_enable() ;
              /*
              calculate mac_slave_cnt_max = f(mac_last_node_addr)
              */
              if (mac_last_node_addr == ADR_UNNAMED)
              {
                mac_slave_cnt_max = MAX_PROMOTED ;
              }
              else
              {
                if (mac_last_node_addr < 32)
                {
                  mac_slave_cnt_max = (mac_last_node_addr-1)<<1 ;
                }
                else
                {
                  mac_slave_cnt_max = ((64-mac_last_node_addr)<<1)-1 ;
                }
                if (mac_slave_cnt_max > MAX_PROMOTED)
                {
                  mac_slave_cnt_max = MAX_PROMOTED ;
                }
              }
              /* reset mac_last_node_addr */
              mac_last_node_addr = ADR_UNNAMED ;
              /* DetectionInit1 */
              mac_slave_cnt = mac_slave_cnt_max ;
              /* switch main channel to direction 1 */
              d_sw_direction(D_DIRECT_1) ;
              d_1_install(mac_driver1_sc, mac_rcv1_evt/*4.1-31*/, BITRATE, RESPONSE_TIME) ;
              d_2_install(mac_driver2_sc, mac_rcv2_evt/*4.1-31*/, BITRATE, RESPONSE_TIME) ;
              /* if interruption switch is closed... */
              if (p_WTB_status->node_status.node_report & INT_BIT)
              {
                mac_fritting_off(MAC_DIR12);    /*4.1-34*/
                /* ...open it ...*/
                d_sw_interruption(D_LINE_A,D_OPEN) ;
                d_sw_interruption(D_LINE_B,D_OPEN) ;
                /* ... and attach both directions. */
                d_sw_line_attachment(
                   /* 1A */ D_CLOSE,
                   /* 1B */ D_CLOSE,
                   /* 2A */ D_CLOSE,
                   /* 2B */ D_CLOSE) ;
                /*
                clear INT-Bit in p_WTB_status->node_report
                */
                pi_disable() ;
                p_WTB_status->node_status.node_report &= (~INT_BIT) ;
                pi_enable() ;
                d_tc_timer_reset(MAC_TIMER1, TMO_SWITCH_SLAVE) ;
              }
              else
              {
                mac_UDF_TIMER1_Flag = TRUE ;
              }
              mac_state = S_DETECT_AWAIT_SWITCH ;
              break;
            }
            /*
            MAC_BM_Name_Req
            */
            case MAC_BM_Name_Req:
            {
              /* if (mac_event == MAC_BM_Name_Req) { ... } else { mac_general_events(); } 4.1-13*/
#ifdef O_MVBC
//            lc_set_device_status_word(LC_DSW_DYD_MSK, LC_DSW_DYD_SET);
#endif
              mac_master_rank = (unsigned char) mac_p2msg->p1 ;
              if (mac_master_rank == STRONG)
              {
                mac_my_str = 0x82 ;
                p_WTB_status->node_strength = L_STRONG ;
              }
              else
              {
                mac_my_str = 2 ;
                p_WTB_status->node_strength = L_WEAK ;
              }
              /*
              set some variables
              */
              mac_ct[MAC_DIR1].Node      = HIGHEST_NODE_ADDRESS ;
              mac_ct[MAC_DIR1].Descend   = DESCENDING_BIT ;
              mac_ct[MAC_DIR1].Cnt       = mac_max_status_fail;    /*4.0-7a*/
              mac_ct[MAC_DIR1].AwakeTime = SYSTIME ;
              mac_ct[MAC_DIR1].Job       = Get_Status ;
              mac_ct[MAC_DIR2].Node      = 2 ;
              mac_ct[MAC_DIR2].Descend   = 0 ;
              mac_ct[MAC_DIR2].Cnt       = mac_max_status_fail;    /*4.0-7a*/
              mac_ct[MAC_DIR2].AwakeTime = SYSTIME ;
              mac_ct[MAC_DIR2].Job       = Get_Status ;
              mac_ct[OPPOSITE_OF(mac_main_direction)].Node = ADR_MASTER ;
              mac_aux_RS = 0 ;
              mac_YWM_cnt = MAX_YWM_CNT ;
              mac_cur_dir = mac_main_direction ;
              mac_insist = TRUE ;
              mac_first_topo = TRUE ;
              mac_ND_array[ADR_MASTER] = mac_configuration.node_descriptor ;
              p_WTB_status->node_address = ADR_MASTER ;
              /*
              prepare line redundancy
              */
              pi_disable() ;
              p_WTB_status->node_status.node_report &=  LINE_STATUS_MASK;
              pi_enable() ;
              mac_MR = 0 ;
              d_get_cur_line_status (&tmp_status) ;
              if (mac_main_direction == MAC_DIR1)
              {
                if (tmp_status & DA1_BIT)
                {
                  mac_MR |= DA_BIT ;
                }
                if (tmp_status & DB1_BIT)
                {
                  mac_MR |= DB_BIT ;
                }
              }
              else
              {
                if (tmp_status & DA2_BIT)
                {
                  mac_MR |= DA_BIT ;
                }
                if (tmp_status & DB2_BIT)
                {
                  mac_MR |= DB_BIT ;
                }
              }
              if (mac_main_direction == MAC_DIR2)
              {
                d_sw_direction(D_DIRECT_2) ;
                d_1_install(mac_driver1_sc, mac_rcv1_evt/*4.1-31*/, BITRATE, RESPONSE_TIME) ;
                d_2_install(mac_driver2_sc, mac_rcv2_evt/*4.1-31*/, BITRATE, RESPONSE_TIME) ;
              }
              Enable(mac_main_direction, ADR_MASTER) ;
              Enable(OPPOSITE_OF(mac_main_direction), ADR_UNNAMED) ;
              SendNameReq() ;
              mac_addr_inaug_line_error = 0x00;      /*4.0-8*/
              mac_state = S_NAMING_AWAIT_NAME_RES ;
              break;
            }
            /*
            MAC_BM_StartMaster_Cmd
            */
            case MAC_BM_StartMaster_Cmd :
            {
              mac_topo.number_of_nodes  = ((mac_my_str & (~STRONG_BIT))>>1) ;
              mac_cur_dir = MAC_DIR2 ;
              mac_present[MAC_DIR1] = mac_max_status_fail;    /*4.0-7a*/
              mac_present[MAC_DIR2] = mac_max_status_fail;    /*4.0-7a*/
              mac_insist  = TRUE ;
              mac_CI_bits = 0x00 ;
              p_WTB_status->net_inhibit = FALSE ;
              mac_YWM_cnt  = MAX_YWM_CNT ;
              ClearMDList() ;
              ClearSPList() ;
              /* preset Presence.Res frame */
              Pre_res_frm.dd   = ADR_BROADCAST ;
              Pre_res_frm.sd   = ADR_MASTER ;
              Pre_res_frm.llc  = SVF_PRESENCE_RESPONSE ;
              Pre_res_frm.size = SVF_PRESENCE_RESPONSE_SIZE ;
              /*
              init line redundancy
              */
              mac_last_line_status = p_WTB_status->node_status.node_report;
              /* clear line status bits */
              d_get_int_line_status (& tmp_status) ;
              line_status_update_counter =  (1 << MAX_NODE_PERIOD);
              d_clear_statistic();
              p_statistic_data->NoTMD = 0;
              p_statistic_data->NoRMD = 0;
              Master_Prepare_Topo() ;
              break;
            }
            /*
            MAC_BM_StartSlave_Cmd
            */
            case MAC_BM_StartSlave_Cmd:
            {
              if (mac_main_direction == MAC_DIR2)
              {
                d_sw_direction(D_DIRECT_2) ;
                d_1_install(mac_driver1_sc, mac_rcv1_evt/*4.1-31*/, BITRATE, RESPONSE_TIME) ;
                d_2_install(mac_driver2_sc, mac_rcv2_evt/*4.1-31*/, BITRATE, RESPONSE_TIME) ;
              }
              mac_last_node_addr = p_WTB_status->node_address ;
              mac_aux_RS = 0 ;
              mac_CI_bits = 0x00 ;
              mac_insist = TRUE ;
              p_WTB_status->net_inhibit = FALSE ;
              mac_YWM_cnt = MAX_YWM_CNT ;
              /*
              init line redundancy
              */
              mac_last_line_status = p_WTB_status->node_status.node_report;
              /* clear line status bits */
              d_get_int_line_status (& tmp_status) ;
              line_status_update_counter = (1 << MAX_NODE_PERIOD);
              /* clear everything except SLP_BIT and SAM_BIT in node_report */
              pi_disable() ;
              p_WTB_status->node_status.node_report &= (SLP_BIT | SAM_BIT);
              pi_enable() ;
              if (p_WTB_status->node_orient == L_SAME)
              {
                /*
                set SAM-Bit in p_WTB_status->node_report
                */
                pi_disable() ;
                p_WTB_status->node_status.node_report |= SAM_BIT ;
                pi_enable() ;
              }
              else
              {
                /*
                clear SAM-Bit in p_WTB_status->node_report
                */
                pi_disable() ;
                p_WTB_status->node_status.node_report &= (~SAM_BIT) ;
                pi_enable() ;
              }
              /* enable main and aux channel */
              Enable(mac_main_direction, p_WTB_status->node_address) ;
              Enable(OPPOSITE_OF(mac_main_direction), ADR_UNNAMED) ;
              d_tc_timer_reset(MAC_TIMER1, TMO_SLAVE_EXP_STATUS) ;
              Pre_res_frm.dd = ADR_BROADCAST ;
              Pre_res_frm.sd = p_WTB_status->node_address ;
              Pre_res_frm.size = SVF_PRESENCE_RESPONSE_SIZE ;
              p_statistic_data->inaug_cnt++;
              d_clear_statistic();
              p_statistic_data->NoTMD = 0;
              p_statistic_data->NoRMD = 0;
#ifdef UMS
              uic_repeat = MAX_UIC_REPEAT;
              li_t_IDTSReset();
              mac_topo.top_address = 0; /* invalidate top address */
#endif
              mac_state = S_SLAVE_TOPO ;
              break;
            }
            /*
            MAC_BM_Unname_Req
            */
            case MAC_BM_Unname_Req:
            {
              SndFrm.dd   = ADR_BROADCAST ;
              SndFrm.sd   = ADR_MASTER ;
              SndFrm.llc  = SVF_UNNAME_REQUEST ;
              SndFrm.size = SVF_UNNAME_REQUEST_SIZE ;
              mac_retry = mac_max_try /*4.0-7a*/;
              SET_SC_FLAG ;
              Send(mac_main_direction,&SndFrm, D_MODE_NOACK) ;
              mac_fritting_off(MAC_DIR12);    /*4.1-34*/
              mac_state = S_UNNAME_AWAIT_SC ;
#ifdef O_MVBC
//            lc_set_device_status_word(LC_DSW_DYD_MSK, LC_DSW_DYD_CLR);
#endif
              break;
            }
            /*
            4.0-3:
            MAC_BM_CloseSwitch_Req
            */
            /*4.0-3..*/
            case MAC_BM_CloseSwitch_Req:
            {
              if (p_WTB_status->node_status.node_report & INT_BIT)
              {






                d_tc_timer_reset(MAC_TIMER2, 0 ) ;
                /*
                4.1-16
                BM_MAC_CloseSwitch_Con
                */
                mac_msg2bm.enr = BM_MAC_CloseSwitch_Con;
                zas_send(WSNR_BM, &mac_msg2bm) ;
                mac_state = S_IDLE ;   /*4.1-16*/
              }
              else
              {
                mac_fritting_off(MAC_DIR12);    /*4.1-34: redundant*/
                /* close the interruption switch: */
                d_sw_interruption(D_LINE_A, D_CLOSE) ;
                d_sw_interruption(D_LINE_B, D_CLOSE) ;
                d_tc_timer_reset(MAC_TIMER2, TMO_SWITCH_SLAVE ) ;
                pi_disable() ;
                p_WTB_status->node_status.node_report |= INT_BIT ;
                pi_enable() ;
                mac_state = S_AWAIT_CLOSE_DELAY ;  /*4.1-16*/
              }
              break;
              /*..4.0-3*/
            }
            default:
            {
              mac_general_events() ;
            }
          }
          break;
        }
        /*
        S_INHIBIT
        */
        case S_INHIBIT :
        {
          if (mac_event == UDF_TIMER1)
          {
            /*
            BM_MAC_Inhibit_Con (FALSE)
            */
            /* treat timeout like not-inhibit */
            mac_msg2bm.enr = BM_MAC_Inhibit_Con;
            mac_msg2bm.p1  = (unsigned short) FALSE ;
            zas_send(WSNR_BM, &mac_msg2bm) ;
            Disable(MAC_DIR1) ;
            mac_state = S_IDLE ;
          }
          else
          {
            if (mac_event == UDF_DIR1)
            {
              if ((mac_rcv_frame -> llc & (~I_BIT)) == SVF_PRESENCE_RESPONSE)
              {
                d_tc_timer_reset(MAC_TIMER1,0) ;
                /*
                BM_MAC_Inhibit_Con(FALSE / TRUE)
                */
                mac_msg2bm.enr = BM_MAC_Inhibit_Con ;
                if ( mac_rcv_frame->llc & I_BIT )
                {
                  mac_msg2bm.p1 = TRUE;
                }
                else
                {
                  mac_msg2bm.p1 = FALSE ;
                }
                zas_send(WSNR_BM, &mac_msg2bm) ;
                Disable(MAC_DIR1) ;
                mac_state = S_IDLE ;
              }
              else
              {
                /* mac_state = S_INHIBIT */
              }
            }
            else
            {
              mac_general_events();
            }
          }
          break ;
        }
        /*
        S_INCLUSION...
        */
        /*
        S_INCLUSION_PRESENCE
        */
        case S_INCLUSION_PRESENCE :
        {
          if (mac_event == UDF_TIMER1)
          {
            /*
            BM_MAC_Inclusion_Con(MAC_RESULT_INC_DISRUPT)
            */
            mac_msg2bm.enr = BM_MAC_Inclusion_Con;
            mac_msg2bm.p1 = MAC_RESULT_INC_DISRUPT ;
            zas_send(WSNR_BM, &mac_msg2bm) ;
            Disable(MAC_DIR1)  ;
            mac_state = S_IDLE ;
          }
          else
          {
            if (mac_event == UDF_DIR1)
            {
              if ( (mac_rcv_frame -> llc & (~I_BIT)) == SVF_PRESENCE_REQUEST )
              {
                /*
                check possibility of inclusion
                */
                /* is later on used in InclusionFail() to decide whether we disrupt the bus or we go to
                inhibit state */
                mac_tmp_LLC = mac_rcv_frame -> llc ;
                /* Can I re-integrate ? */
                if (
                mac_can_recover                                           &&
                (mac_tmp_SV.WTB_status.node_address != ADR_MASTER)        &&
                ((mac_tmp_SV.WTB_status.node_status.node_report & INT_BIT) ==
                INT_BIT)
                )
                {
                  /* is master topo correct ? */
                  if (
                  (mac_rcv_frame -> data[POS_MT_L] == mac_tmp_SV.MT_L) &&
                  (mac_rcv_frame -> data[POS_MT_H__YP] == mac_tmp_SV.MT_H__YP)
                  )
                  {
                    /*
                    Restore slave context from mac_tmp_SV
                    */
                    *p_WTB_status       = mac_tmp_SV.WTB_status ;
                    mac_topo            = mac_tmp_SV.topo ;
                    mac_main_direction  = mac_tmp_SV.main_direction ;
                    mac_last_node_addr  = mac_tmp_SV.last_node_addr ;
                    mac_my_str          = mac_tmp_SV.my_str ;
                    /*
                    InclusionTest1
                    */
                    /* InclusionTest1 */
                    Enable(MAC_DIR1,p_WTB_status->node_address) ;
                    /*
                    Wait for process data poll frame during MAX_TRY * former individual period
                    */
                    d_tc_timer_reset(MAC_TIMER1, BASIC_CYCLE * mac_max_try /*4.0-7a*/ *
                    mac_topo.individual_period) ;
                    mac_state = S_INCLUSION_PD_REQ ;
                  }
                  else
                  {
                    InclusionFail() ;
                    mac_state = S_IDLE ;
                  }
                }
                else
                {
                  InclusionFail() ;
                  mac_state = S_IDLE ;
                }
              }
              else
              {
                /*
                mac_state = S_INCLUSION_PRESENCE
                */
              }
            }
            else
            {
              mac_general_events();
            }
          }
          break ;
        }
        /*
        S_INCLUSION_PD_REQ
        */
        case S_INCLUSION_PD_REQ :
        {
          if (mac_event == UDF_TIMER1)
          {
            InclusionFail() ;
            mac_state = S_IDLE ;
          }
          else
          {
            if (mac_event == UDF_DIR1)
            {
              if (mac_rcv_frame -> llc == PD_REQUEST)
              {
                /* Listen if there is somebody replying the process data poll frame */
                d_tc_timer_reset( MAC_TIMER1, TMO_AWAIT_RESPONSE) ;
                mac_state = S_INCLUSION_PD_RES ;
              }
              else
              {
                /*
                mac_state = S_INCLUSION_PD_REQ
                */
              }
            }
            else
            {
              mac_general_events();
            }
          }
          break ;
        }
        /*
        S_INCLUSION_PD_RES
        */
        case S_INCLUSION_PD_RES :
        {
          if (mac_event == UDF_TIMER1)
          {
            /*
            BM_MAC_Inclusion_Con(MAC_RESULT_INC_INCLUDED)
            */
            mac_msg2bm.enr = BM_MAC_Inclusion_Con ;
            mac_msg2bm.p1  = (unsigned short) MAC_RESULT_INC_INCLUDED ;
            zas_send(WSNR_BM, &mac_msg2bm) ;
            InclusionSuccess() ;
            mac_state = S_SLAVE_REGULAR ;
          }
          else
          {
            if (mac_event == UDF_DIR1)
            {
              if (
                  (mac_rcv_frame -> llc == PD_RESPONSE) &&
                  (mac_rcv_frame -> sd == p_WTB_status->node_address))
              {
                InclusionFail() ;
                mac_state = S_IDLE ;
              }
              else
              {
                /*
                BM_MAC_Inclusion_Con(MAC_RESULT_INC_INCLUDED)
                */
                mac_msg2bm.enr = BM_MAC_Inclusion_Con ;
                mac_msg2bm.p1  = (unsigned short) MAC_RESULT_INC_INCLUDED ;
                zas_send(WSNR_BM, &mac_msg2bm) ;
                InclusionSuccess() ;
                mac_state = S_SLAVE_REGULAR ;
              }
            }
            else
            {
              mac_general_events();
            }
          }
          break ;
        }
        /*
        S_DETECT...
        */
        /*
        S_DETECTION_INSIST_REQ
        */
        case S_DETECTION_INSIST_REQ:
        {
          if (mac_event == UDF_DIR1)
          {
            switch (mac_sub_event)
            {
              case D_OK:
              {
                /*
                evaluate received frame
                */
                mac_main_direction = MAC_DIR1 ;
                mac_slave_cnt = mac_slave_cnt_max ;
                switch (mac_rcv_frame -> llc)
                {
                  case SVF_DETECT_RESPONSE :
                  {
                    mac_aux_RS = mac_rcv_frame->data[POS_RS] ;
                    if ( (mac_my_str >= 128) &&
                    (mac_aux_RS >= 128) )
                    {
                      if (mac_SMC_cnt == 0)
                      {
                        d_tc_timer_reset(MAC_TIMER1,0) ;
                        Disable(MAC_DIR1)  ;
                        Disable(MAC_DIR2)  ;
                        d_tc_timer_reset(MAC_TIMER2, TMO_STOP) ;
                        mac_state = S_DETECTION_STOP_SMC ;
                      }
                      else
                      {
                        /* see if we find something <> strong master */
                        mac_SMC_cnt-- ;
                        d_tc_timer_reset(MAC_TIMER1, BASIC_CYCLE + get_random_number()) ;
                        mac_state = S_DETECTION_INSIST_RES ;
                      }
                    }
                    else
                    {
                      /*
                      no SMC
                      */
                      mac_SMC_cnt = mac_max_smc;       /*4.0-7a*/
                      if (mac_aux_RS & INSIST_BIT)
                      {
                        /* I yield */
                        if (mac_my_str == 2)
                        {
                          /* reset weak master strength to 0 */
                          mac_my_str       = 0 ;
                          mac_lone_YWM     = TRUE ;
                          mac_lone_YWM_cnt = MAX_YWM_CNT ;
                        }
                        d_tc_timer_reset(MAC_TIMER1, BASIC_CYCLE + get_random_number()) ;
                        mac_state = S_DETECTION_YIELD_ACCEPT_NAME ;
                      }
                      else
                      {
                        d_tc_timer_reset(MAC_TIMER1,0) ;
                        Disable(MAC_DIR1)  ;
                        Disable(MAC_DIR2)  ;
                        d_tc_timer_reset(MAC_TIMER2, TMO_STOP) ;
                        mac_state = S_DETECTION_STOP_INSIST ;
                      }
                    }
                    break;
                  }
                  case SVF_DETECT_REQUEST :
                  {
                    /* jtter */
                    d_tc_timer_reset(MAC_TIMER1, BASIC_CYCLE + get_random_number()) ;
                    mac_state = S_DETECTION_INSIST_RES ;
                    break;
                  }
                  default:
                  {
                    /* protocol error */
                    d_tc_timer_reset(MAC_TIMER1, BASIC_CYCLE + get_random_number()) ;
                    mac_state = S_DETECTION_INSIST_RES ;
                  }
                }
                break;
              }
              case D_RPT_TIMEOUT:
              {
                d_tc_timer_reset(MAC_TIMER1, BASIC_CYCLE + get_random_number());
                /*
                check sleep bit
                */
                if (p_WTB_status->node_status.node_report & SLP_BIT)
                {
                  mac_fritting_off( MAC_DIR12);   /*4.0-7b*/
                  /* the bus_switch is open in this state anyway */
                  /* d_sw_interruption(D_LINE_A,D_OPEN) ;   4.0-10a*/
                  /* d_sw_interruption(D_LINE_B,D_OPEN) ;   4.0-10a*/
                  d_sleep();
                  while (TRUE)
                  {

                  }
                }
                mac_state = S_DETECTION_INSIST_RES ;
                break;
              }
              default:
              {
                d_tc_timer_reset(MAC_TIMER1, BASIC_CYCLE + get_random_number()) ;
                mac_state = S_DETECTION_INSIST_RES ;
              }
            }
          }
          else
          {
            if  (mac_event == UDF_DIR2)
            {
              /*
              evaluate received frame
              */
              mac_main_direction = MAC_DIR2 ;
              mac_slave_cnt = mac_slave_cnt_max ;
              if (mac_sub_event == D_OK)
              {
                switch (mac_rcv_frame -> llc)
                {
                  case SVF_DETECT_RESPONSE :
                  {
                    mac_aux_RS = mac_rcv_frame->data[POS_RS] ;
                    if ( (mac_my_str >= 128) && (mac_aux_RS >= 128) )
                    {
                      if (mac_SMC_cnt == 0)
                      {
                        d_tc_timer_reset(MAC_TIMER1,0) ;
                        Disable(MAC_DIR1)  ;
                        Disable(MAC_DIR2)  ;
                        d_tc_timer_reset(MAC_TIMER2, TMO_STOP) ;
                        mac_state = S_DETECTION_STOP_SMC ;
                      }
                      else
                      {
                        /* see if we find something <> strong master */
                        mac_SMC_cnt-- ;
                        d_tc_timer_reset(MAC_TIMER1, BASIC_CYCLE + get_random_number()) ;
                        mac_state = S_DETECTION_INSIST_RES ;
                      }
                    }
                    else
                    {
                      /*
                      no SMC
                      */
                      mac_SMC_cnt = mac_max_smc;                   /*4.0-7a*/
                      if (mac_aux_RS & INSIST_BIT)
                      {
                        /* I yield */
                        if (mac_my_str == 2)
                        {
                          /* reset weak master strength to 0 */
                          mac_my_str      = 0 ;
                          mac_lone_YWM    = TRUE ;
                          mac_lone_YWM_cnt = MAX_YWM_CNT ;
                        }
                        d_tc_timer_reset(MAC_TIMER1, BASIC_CYCLE + get_random_number()) ;
                        mac_state = S_DETECTION_YIELD_ACCEPT_NAME ;
                      }
                      else
                      {
                        d_tc_timer_reset(MAC_TIMER1,0) ;
                        Disable(MAC_DIR1)  ;
                        Disable(MAC_DIR2)  ;
                        d_tc_timer_reset(MAC_TIMER2, TMO_STOP) ;
                        mac_state = S_DETECTION_STOP_INSIST ;
                      }
                    }
                    break;
                  }
                  case SVF_DETECT_REQUEST :
                  {
                    /* jtter */
                    d_tc_timer_reset(MAC_TIMER1, BASIC_CYCLE + get_random_number()) ;
                    mac_state = S_DETECTION_INSIST_RES ;
                    break;
                  }
                  default:
                  {
                    /* protocol */
                  }
                }
              }
            }
            else
            {
              mac_general_events();
            }
          }
          break;
        }
        /*
        S_DETECTION_INSIST_RES
        */
        case S_DETECTION_INSIST_RES:
        {
          if (mac_event == UDF_TIMER1)
          {
            SendDetectReq12() ;
            mac_state = S_DETECTION_INSIST_REQ ;
          }
          else
          {
            if  ( (mac_event == UDF_DIR1) ||
                  (mac_event == UDF_DIR2) )
            {
              mac_slave_cnt = mac_slave_cnt_max ;
              /*
              evaluate received frame
              */
              if (mac_sub_event == D_OK)
              {
                switch (mac_rcv_frame -> llc)
                {
                  case SVF_DETECT_REQUEST :
                  {
                    /*
                    detection request frame received
                    */
                    if (mac_event == UDF_DIR1)
                    {
                      mac_main_direction = MAC_DIR1 ;
                      frm1.dd       = ADR_BROADCAST ;
                      frm1.sd       = ADR_UNNAMED ;
                      frm1.llc      = SVF_DETECT_RESPONSE;
                      frm1.size     = SVF_DETECT_RESPONSE_SIZE ;
                      /* DA_,DB_,INH_BITs are cleared */
                      frm1.data[POS_MR]  = 0; /*4.2-17/C12_BIT,v4.0-2*/
                      frm1.data[POS_LS]  = mac_my_str ;
                      /*frm1.data[POS_RES] = RESERVED_FIELD_VALUE ; 4.1-27*/
                    }
                    else
                    {
                      mac_main_direction = MAC_DIR2 ;
                      frm2.dd       = ADR_BROADCAST ;
                      frm2.sd       = ADR_UNNAMED ;
                      frm2.llc      = SVF_DETECT_RESPONSE ;
                      frm2.size     = SVF_DETECT_RESPONSE_SIZE ;
                      /* DA_,DB_,INH_BITs are cleared */
                      frm2.data[POS_MR]  = C12_BIT ; /*v4.0-2*/
                      frm2.data[POS_LS]  = mac_my_str ;
                      /*frm2.data[POS_RES] = RESERVED_FIELD_VALUE ; 4.1-27*/
                    }
                    mac_aux_RS = mac_rcv_frame -> data[POS_RS] ;
                    if ( (mac_my_str >= 128) && (mac_aux_RS >= 128))
                    {
                      /*
                      strong master conflict
                      */
                      if (mac_event == UDF_DIR1)
                      {
                        frm1.data[POS_LS] = mac_my_str | INSIST_BIT ;
                        Send(MAC_DIR1, &frm1, D_MODE_NOACK) ;
                      }
                      else
                      {
                        frm2.data[POS_LS] = mac_my_str | INSIST_BIT ;
                        Send(MAC_DIR2, &frm2, D_MODE_NOACK) ;
                      }
                      if (mac_SMC_cnt == 0)
                      {
                        d_tc_timer_reset(MAC_TIMER1,0) ;
                        Disable(MAC_DIR1)  ;
                        Disable(MAC_DIR2)  ;
                        d_tc_timer_reset(MAC_TIMER2, TMO_STOP) ;
                        mac_state = S_DETECTION_STOP_SMC ;
                      }
                      else
                      {
                        mac_SMC_cnt-- ;
                        /* see if we find something <> strong master */
                      }
                    }
                    else
                    {
                      mac_SMC_cnt = mac_max_smc;               /*4.0-7a*/
                      if ( mac_aux_RS > mac_my_str )
                      {
                        /* DetectionReqYield */
                        if (mac_event == UDF_DIR1)
                        {
                          Send(MAC_DIR1, &frm1, D_MODE_NOACK) ;
                        }
                        else
                        {
                          Send(MAC_DIR2, &frm2, D_MODE_NOACK) ;
                        }
                        if (mac_my_str == 2)
                        {
                          /* set weak master strength to 0 */
                          mac_my_str       = 0 ;
                          mac_lone_YWM     = TRUE ;
                          mac_lone_YWM_cnt = MAX_YWM_CNT ;
                        }
                        mac_state = S_DETECTION_YIELD_ACCEPT_NAME ;
                      }
                      else
                      {
                        /*
                        I insist
                        */
                        if (mac_event == UDF_DIR1)
                        {
                          frm1.data[POS_LS] |= INSIST_BIT ;
                          Send(MAC_DIR1, &frm1, D_MODE_NOACK) ;
                        }
                        else
                        {
                          frm2.data[POS_LS] |= INSIST_BIT ;
                          Send(MAC_DIR2, &frm2, D_MODE_NOACK) ;
                        }
                        d_tc_timer_reset(MAC_TIMER1,0) ;
                        Disable(MAC_DIR1)  ;
                        Disable(MAC_DIR2)  ;
                        d_tc_timer_reset(MAC_TIMER2, TMO_STOP) ;
                        mac_state = S_DETECTION_STOP_INSIST ;
                      }
                    }
                    break;
                  }
                  default:
                  {
                    /* protocol error */
                  }
                }
              }
            }
            else
            {
              mac_general_events();
            }
          }
          break;
        }
        /*
        S_DETECTION_YIELD
        */
        case S_DETECTION_YIELD:
        {
          if (mac_event == UDF_TIMER1)
          {
            /*
            check sleep bit
            */
            if (p_WTB_status->node_status.node_report & SLP_BIT)
            {
              /*4.0-7b*/
              mac_fritting_off( MAC_DIR12);
              /* the bus_switch is open in this state anyway */
              /* d_sw_interruption(D_LINE_A,D_OPEN) ; 4.0-10a*/
              /* d_sw_interruption(D_LINE_B,D_OPEN) ; 4.0-10a*/
              d_sleep() ;
              while (TRUE)
              {

              }
            }
            /*
            detection timeout
            */
            if (mac_lone_YWM)
            {
              if (--mac_lone_YWM_cnt == 0)
              {
                mac_my_str = 2 ;
                mac_aux_RS = 0 ;
                mac_lone_YWM = FALSE ;
                SendDetectReq12() ;
                mac_state = S_DETECTION_INSIST_REQ ;
              }
              else
              {
                d_tc_timer_reset(MAC_TIMER1, BASIC_CYCLE + get_random_number()) ;
              }
            }
            else
            {
              /* must be unnamed slave */
              if (--mac_slave_cnt == 0)
              {
                /*
                BM_MAC_Detect_Con(MAC_RESULT_DET_NOTHING)
                */
                /* confirm detect request */
                mac_msg2bm.enr = BM_MAC_Detect_Con ;
                mac_msg2bm.p1  = (unsigned short) MAC_RESULT_DET_NOTHING ;
                zas_send(WSNR_BM, &mac_msg2bm) ;
                Disable(MAC_DIR1)  ;
                Disable(MAC_DIR2)  ;
                /*4.1-34: fritting is not switched off */
                mac_state = S_IDLE ;
              }
              else
              {
                d_tc_timer_reset(MAC_TIMER1, BASIC_CYCLE + get_random_number()) ;
              }
            }
          }
          else
          {
            if  ( (mac_event == UDF_DIR1) ||
                  (mac_event == UDF_DIR2) )
            {
              mac_slave_cnt = mac_slave_cnt_max ;
              /*
              evaluate received frame
              */
              if (mac_sub_event == D_OK)
              {
                switch (mac_rcv_frame -> llc)
                {
                  case SVF_DETECT_REQUEST :
                  {
                    /*
                    detection request frame received
                    */
                    if (mac_event == UDF_DIR1)
                    {
                      mac_main_direction = MAC_DIR1 ;
                      frm1.dd       = ADR_BROADCAST ;
                      frm1.sd       = ADR_UNNAMED ;
                      frm1.llc      = SVF_DETECT_RESPONSE;
                      frm1.size     = SVF_DETECT_RESPONSE_SIZE ;
                      /* DA_,DB_,INH_BITs are cleared */
                      frm1.data[POS_MR]  = 0; /*4.1-27/C12_BIT:v4.0-2*/
                      frm1.data[POS_LS]  = mac_my_str ;
                      /*frm1.data[POS_RES] = RESERVED_FIELD_VALUE ; 4.1-27*/
                      Send(MAC_DIR1, &frm1, D_MODE_NOACK) ;
                    }
                    else
                    {
                      mac_main_direction = MAC_DIR2 ;
                      frm2.dd       = ADR_BROADCAST ;
                      frm2.sd       = ADR_UNNAMED ;
                      frm2.llc      = SVF_DETECT_RESPONSE ;
                      frm2.size     = SVF_DETECT_RESPONSE_SIZE ;
                      /* DA_,DB_,INH_BITs are cleared */
                      frm2.data[POS_MR]  = C12_BIT ; /*v4.0-2*/
                      frm2.data[POS_LS]  = mac_my_str ;
                      /*frm2.data[POS_RES] = RESERVED_FIELD_VALUE ; 4.1-27*/
                      Send(MAC_DIR2, &frm2, D_MODE_NOACK) ;
                    }
                    mac_lone_YWM_cnt = MAX_YWM_CNT ;
                    mac_state = S_DETECTION_YIELD_ACCEPT_NAME ;
                    break;
                  }
                  default:
                  {
                    /* protocol error */
                    diagwtb_entry(
                    DIAG_MAC_ERR_PROTOCOL,
                    (unsigned short) mac_state,
                    (unsigned short) mac_event,
                    0x0003,
                    (void*)
                    (0xFFFF0000+((mac_rcv_frame->llc) | (mac_sub_event<<8) ) )
                    ) ; /*4.1-28*/
                  }
                }
              }
              else
              {
                diagwtb_entry(
                DIAG_MAC_ERR_PROTOCOL,
                (unsigned short) mac_state,
                (unsigned short) mac_event,
                0x0003,
                (void*)
                (0xFFFF0000+((mac_rcv_frame->llc) | (mac_sub_event<<8) ) )
                ) ; /*4.1-28*/
              }
            }
            else
            {
              mac_general_events();
            }
          }
          break;
        }
        /*
        S_DETECTION_YIELD_ACCEPT_NAME
        */
        case S_DETECTION_YIELD_ACCEPT_NAME:
        {
          if (mac_event == UDF_TIMER1)
          {
            /*
            detection timeout
            */
            if (mac_lone_YWM)
            {
              if (--mac_lone_YWM_cnt == 0)
              {
                mac_my_str = 2 ;
                mac_aux_RS = 0 ;
                mac_lone_YWM = FALSE ;
                SendDetectReq12() ;
                mac_state = S_DETECTION_INSIST_REQ ;
              }
              else
              {
                d_tc_timer_reset(MAC_TIMER1, BASIC_CYCLE + get_random_number()) ;
              }
            }
            else
            {
              /* must be unnamed slave */
              if (--mac_slave_cnt == 0)
              {
                /*
                BM_MAC_Detect_Con(MAC_RESULT_DET_NOTHING)
                */
                /* confirm detect request */
                mac_msg2bm.enr = BM_MAC_Detect_Con ;
                mac_msg2bm.p1  = (unsigned short) MAC_RESULT_DET_NOTHING ;
                zas_send(WSNR_BM, &mac_msg2bm) ;
                Disable(MAC_DIR1)  ;
                Disable(MAC_DIR2)  ;
                /*4.1-32: fritting is not switched off */
                mac_state = S_IDLE ;
              }
              else
              {
                d_tc_timer_reset(MAC_TIMER1, BASIC_CYCLE + get_random_number()) ;
              }
            }
          }
          else
          {
            if  ( (mac_event == UDF_DIR1) ||
                  (mac_event == UDF_DIR2) )
            {
              mac_slave_cnt = mac_slave_cnt_max ;
              /*
              evaluate received frame
              */
              if (mac_sub_event == D_OK)
              {
                switch (mac_rcv_frame -> llc)
                {
                  case SVF_DETECT_REQUEST :
                  {
                    /*
                    detection request frame received
                    */
                    if (mac_event == UDF_DIR1)
                    {
                      mac_main_direction = MAC_DIR1 ;
                      frm1.dd       = ADR_BROADCAST ;
                      frm1.sd       = ADR_UNNAMED ;
                      frm1.llc      = SVF_DETECT_RESPONSE;
                      frm1.size     = SVF_DETECT_RESPONSE_SIZE ;
                      /* DA_,DB_,INH_BITs are cleared */
                      frm1.data[POS_MR]  = 0; /*4.1-27/C12_BIT:v4.0-2*/
                      frm1.data[POS_LS]  = mac_my_str ;
                      /*frm1.data[POS_RES] = RESERVED_FIELD_VALUE; 4.1-27*/
                      Send(MAC_DIR1, &frm1, D_MODE_NOACK) ;
                    }
                    else
                    {
                      mac_main_direction = MAC_DIR2 ;
                      frm2.dd       = ADR_BROADCAST ;
                      frm2.sd       = ADR_UNNAMED ;
                      frm2.llc      = SVF_DETECT_RESPONSE ;
                      frm2.size     = SVF_DETECT_RESPONSE_SIZE ;
                      /* DA_,DB_,INH_BITs are cleared */
                      frm2.data[POS_MR]  = C12_BIT ; /*v4.0-2*/
                      frm2.data[POS_LS]  = mac_my_str ;
                      /*frm2.data[POS_RES] = RESERVED_FIELD_VALUE; 4.1-27*/
                      Send(MAC_DIR2, &frm2, D_MODE_NOACK) ;
                    }
                    mac_lone_YWM_cnt = MAX_YWM_CNT ;
                    break;
                  }
                  case SVF_NAME_REQUEST :
                  {
                    /*
                    naming request frame received
                    */
#ifdef O_MVBC
//                  lc_set_device_status_word(LC_DSW_DYD_MSK, LC_DSW_DYD_SET);
#endif
                    mac_my_str = mac_rcv_frame -> data[POS_LS] ;
                    p_WTB_status->node_strength = L_SLAVE ;
                    p_WTB_status->node_address = mac_rcv_frame -> data[POS_YA] & (~DESCENDING_BIT) ;
                    if (mac_event == UDF_DIR1)
                    {
                      mac_main_direction = MAC_DIR1 ;
                      frm1.dd   = ADR_MASTER ;
                      frm1.sd   = ADR_UNNAMED ;
                      frm1.llc  = SVF_NAME_RESPONSE ;
                      frm1.size = SVF_NAME_RESPONSE_SIZE ;
                      Send(MAC_DIR1, &frm1, D_MODE_NOACK) ;
                    }
                    else
                    {
                      mac_main_direction = MAC_DIR2 ;
                      frm2.dd   = ADR_MASTER ;
                      frm2.sd   = ADR_UNNAMED ;
                      frm2.llc  = SVF_NAME_RESPONSE ;
                      frm2.size = SVF_NAME_RESPONSE_SIZE ;
                      Send(MAC_DIR2, &frm2, D_MODE_NOACK) ;
                    }
                    /*
                    determine node orientation
                    */
                    if ( ( (mac_rcv_frame -> data[POS_YA]) & DESCENDING_BIT ) == 0)
                    {
                      if (mac_main_direction == MAC_DIR1)
                      {
                        p_WTB_status->node_orient = L_SAME ;
                      }
                      else
                      {
                        p_WTB_status->node_orient = L_INVERSE ;
                      }
                    }
                    else
                    {
                      if (mac_main_direction == MAC_DIR1)
                      {
                        p_WTB_status->node_orient = L_INVERSE ;
                      }
                      else
                      {
                        p_WTB_status->node_orient = L_SAME ;
                      }
                    }
                    Disable(MAC_DIR1)  ;
                    Disable(MAC_DIR2)  ;
                    d_tc_timer_reset(MAC_TIMER1,0) ;
                    /*
                    BM_MAC_Detect_Con(MAC_RESULT_DET_YIELD)
                    */
                    mac_msg2bm.enr = BM_MAC_Detect_Con ;
                    mac_msg2bm.p1  = (unsigned short) MAC_RESULT_DET_YIELD ;
                    zas_send(WSNR_BM, &mac_msg2bm) ;
                    mac_fritting_off(mac_main_direction);   /*4.1-32*/
                    mac_state = S_IDLE ;
                    break;
                  }
                  default:
                  {
                    /* protocol error */
                  }
                }
              }
            }
            else
            {
              mac_general_events();
            }
          }
          break;
        }
        /*
        S_DETECT_AWAIT_SWITCH
        */
        case S_DETECT_AWAIT_SWITCH :
        {
          if (mac_event == UDF_TIMER1)
          {
            mac_fritting_on( MAC_DIR12 );   /*4.0-7*/
            Enable(MAC_DIR1, ADR_UNNAMED);
            Enable(MAC_DIR2, ADR_UNNAMED);
            if (mac_my_str > 1)
            {
              SendDetectReq12();
              mac_state = S_DETECTION_INSIST_REQ;
            }
            else
            {
              d_tc_timer_reset(MAC_TIMER1, BASIC_CYCLE + get_random_number());
              mac_state = S_DETECTION_YIELD;
            }
          }
          else
          {
            mac_general_events();
          }
          break ;
        }
        /*
        S_DETECTION_STOP_SMC
        */
        case S_DETECTION_STOP_SMC :
        {
          if (mac_event == UDF_TIMER2)
          {
            /*
            BM_MAC_Detect_Con(MAC_RESULT_DET_SMC)
            */
            mac_msg2bm.enr = BM_MAC_Detect_Con ;
            mac_msg2bm.p1  = (unsigned short) MAC_RESULT_DET_SMC ;
            zas_send(WSNR_BM, &mac_msg2bm) ;
            /*4.1-34: fritting is not switched off*/
            mac_state = S_IDLE ;
          }
          else
          {
            if  ( (mac_event == UDF_DIR1) ||
                  (mac_event == UDF_DIR2) )
            {

            }
            else
            {
              mac_general_events() ;
            }
          }
          break ;
        }
        /*
        S_DETECTION_STOP_INSIST
        */
        case S_DETECTION_STOP_INSIST :
        {
          if (mac_event == UDF_TIMER2)
          {
            /*
            BM_MAC_Detect_Con(MAC_RESULT_DET_INSIST)
            */
            mac_msg2bm.enr = BM_MAC_Detect_Con ;
            mac_msg2bm.p1  = (unsigned short) MAC_RESULT_DET_INSIST ;
            zas_send(WSNR_BM, &mac_msg2bm) ;
            mac_fritting_off(mac_main_direction);  /*4.1-32*/
            mac_state = S_IDLE ;
          }
          else
          {
            if  ( (mac_event == UDF_DIR1) ||
                  (mac_event == UDF_DIR2) )
            {

            }
            else
            {
              mac_general_events() ;
            }
          }
          break ;
        }
        /*
        S_NAMING.....
        */
        /*
        S_NAMING_AWAIT_DELAY
        */
        case S_NAMING_AWAIT_DELAY  :
        {
          if (mac_event == UDF_TIMER1)
          {
            Naming_Schedule_Resume() ;
          }
          else
          {
            mac_general_events() ;
          }
          break;
        }
        /*
        S_NAMING_WAIT_FOR_MASTER_CLOSING_SWITCH
        */
        case S_NAMING_WAIT_FOR_MASTER_CLOSING_SWITCH:
        {
          if (mac_event == UDF_TIMER1)
          {
            Enable(mac_main_direction, ADR_MASTER) ;
            /*
            set INT-Bit in p_WTB_status->node_report
            */
            pi_disable() ;
            p_WTB_status->node_status.node_report |= INT_BIT ;
            pi_enable() ;
            IncAddr() ;
            SendNameReq() ;
            mac_state = S_NAMING_AWAIT_NAME_RES ;
          }
          else
          {
            mac_general_events() ;
          }
          break;
        }
        /*
        S_NAMING_WAIT_FOR_MASTER_OPENING_SWITCH
        */
        case S_NAMING_WAIT_FOR_MASTER_OPENING_SWITCH :
        {
          if (mac_event == UDF_TIMER1)
          {
            Enable(mac_main_direction, ADR_MASTER) ;
            Enable(OPPOSITE_OF(mac_main_direction), ADR_UNNAMED) ;
            mac_fritting_on( OPPOSITE_OF(mac_main_direction));    /*4.0-7b*/
            mac_YWM_cnt = MAX_YWM_CNT ;
            mac_insist  = TRUE ;
            mac_aux_RS  = 0 ;
            /*
            clear INT-Bit in p_WTB_status->node_report
            */
            pi_disable() ;
            p_WTB_status->node_status.node_report &= (~INT_BIT) ;
            pi_enable() ;
            mac_ct[mac_cur_dir].FirstStatus  = FALSE ;
            mac_ct[mac_cur_dir].AwakeTime    = SYSTIME ;
            mac_ct[mac_cur_dir].Cnt = mac_max_status_fail;    /*4.0-7a*/
            mac_ct[mac_cur_dir].Job = Get_Status ;
            Naming_Schedule() ;
          }
          else
          {
            mac_general_events() ;
          }
          break;
        }
        /*
        S_NAMING_WAIT_FOR_SLAVE_OPENING_SWITCH
        */
        case S_NAMING_WAIT_FOR_SLAVE_OPENING_SWITCH:
        {
          if (mac_event == UDF_TIMER1)
          {
            mac_ct[mac_cur_dir].FirstStatus = FALSE ;
            mac_ct[mac_cur_dir].AwakeTime   = SYSTIME ;
            mac_ct[mac_cur_dir].Cnt = mac_max_status_fail;   /*4.0-7a*/
            mac_ct[mac_cur_dir].Job = Get_Status ;
            Naming_Schedule() ;
          }
          else
          {
            mac_general_events() ;
          }
          break;
        }
        /*
        S_NAMING_AWAIT_NAME_RES
        */
        case S_NAMING_AWAIT_NAME_RES:
        {
          if (((mac_event == UDF_DIR1) && (mac_main_direction == MAC_DIR1)) ||
              ((mac_event == UDF_DIR2) && (mac_main_direction == MAC_DIR2)))
          {
            if (
                 (mac_sub_event == D_OK)                     &&
                 (mac_rcv_frame -> llc == SVF_NAME_RESPONSE)
                )
            {
              /* increment strength */
              mac_my_str += 2 ;  /*XXX*/
              mac_ct[mac_cur_dir].Cnt = mac_max_status_fail;   /*4.0-7a*/
              mac_ct[mac_cur_dir].FirstStatus  = TRUE ;
              mac_ct[mac_cur_dir].Job       = Get_Status ;
              mac_ct[mac_cur_dir].AwakeTime = SYSTIME + TMO_SWITCH_MAIN_DIRECTION ;
              Naming_Schedule() ;
            }
            else
            {
              if (SndFrm.dd == ADR_UNNAMED)
              {
                SndFrm.dd = mac_ct[mac_cur_dir].Node ;
                Send(mac_main_direction, &SndFrm, D_MODE_ACK) ;
                /* mac_state = S_NAMING_AWAIT_NAME_RES ; */
              }
              else
              {
                if (--mac_retry == 0)
                {
                  /*
                  NamingDecAddr
                  */
                  /* NamingDecAddr */
                  if (mac_cur_dir == MAC_DIR1)
                  {
                    if (mac_ct[mac_cur_dir].Node == HIGHEST_NODE_ADDRESS)
                    {
                      mac_ct[mac_cur_dir].Node = ADR_MASTER ;
                    }
                    else
                    {
                      mac_ct[mac_cur_dir].Node++ ;
                    }
                  }
                  else
                  {
                    mac_ct[mac_cur_dir].Node-- ;
                  }
                  mac_ct[mac_cur_dir].FirstStatus = TRUE ;
                  if (
                  (mac_cur_dir == mac_main_direction) &&
                  (mac_ct[mac_cur_dir].Node == ADR_MASTER)
                  )
                  {
                    /*
                    BM_MAC_Name_Con(Again)
                    */
                    mac_msg2bm.enr = BM_MAC_Name_Con ;
                    mac_msg2bm.p1  = (unsigned short) MAC_RESULT_NAME_AGAIN ;
                    zas_send(WSNR_BM, &mac_msg2bm) ;
                    Disable(MAC_DIR1)  ;
                    Disable(MAC_DIR2)  ;
                    p_WTB_status->node_strength = L_UNDEFINED ;
                    mac_state = S_IDLE ;
                  }
                  else
                  {
                    /*mac_my_str -= 2 ; 4.1-47 */
                    /*
                    NamingSetEnd
                    */
                    if (mac_ct[mac_cur_dir].Node == ADR_MASTER)
                    {
                      Disable(mac_main_direction) ;
                      mac_fritting_off(MAC_DIR12);    /*4.1-34*/
                      d_sw_interruption(D_LINE_A, D_OPEN) ;
                      d_sw_interruption(D_LINE_B, D_OPEN) ;
                      /* attach both directions */
                      d_sw_line_attachment(
                      /* 1A */ D_CLOSE,
                      /* 1B */ D_CLOSE,
                      /* 2A */ D_CLOSE,
                      /* 2B */ D_CLOSE) ;
                      d_tc_timer_reset(MAC_TIMER1, TMO_SWITCH_SLAVE) ;
                      mac_state = S_NAMING_WAIT_FOR_MASTER_OPENING_SWITCH ;
                    }
                    else
                    {
                      SndFrm.dd   = mac_ct[mac_cur_dir].Node ;
                      SndFrm.sd   = ADR_MASTER ;
                      SndFrm.llc  = SVF_SETEND_REQUEST ;
                      SndFrm.size = SVF_SETEND_REQUEST_SIZE ;
                      SndFrm.data[POS_LS] = mac_my_str ;
                      mac_retry = mac_max_try /*4.0-7a*/;
                      Send(mac_main_direction, &SndFrm, D_MODE_ACK) ;
                      mac_state = S_NAMING_AWAIT_END_RES ;
                    }
                  }
                }
                else
                {
                  SndFrm.dd = ADR_UNNAMED;
                  Send(mac_main_direction, &SndFrm, D_MODE_ACK) ;
                  /* mac_state = S_NAMING_AWAIT_NAME_RES ; */
                }
              }
            }
          }
          else
          {
            mac_general_events();
          }
          break;
        }
        /*
        S_NAMING_AWAIT_END_RES
        */
        case S_NAMING_AWAIT_END_RES:
        {
          if (((mac_event == UDF_DIR1) && (mac_main_direction == MAC_DIR1)) ||
              ((mac_event == UDF_DIR2) && (mac_main_direction == MAC_DIR2)))
          {
            if (
                (mac_sub_event    == D_OK)                &&
                (mac_rcv_frame -> llc == SVF_SETEND_RESPONSE)
               )
            {
              /*
              clear INT-Bit in  mac_NS_array[x].node_report
              */
              mac_NS_array[mac_ct[mac_cur_dir].Node].node_report &= (~INT_BIT) ;
              d_tc_timer_reset(MAC_TIMER1,TMO_SWITCH_MASTER) ;
              mac_state = S_NAMING_WAIT_FOR_SLAVE_OPENING_SWITCH ;
            }
            else
            {
              if (--mac_retry == 0)
              {
                /*
                BM_MAC_Name_Con(MAC_RESULT_NAME_FAILURE)
                */
                mac_msg2bm.enr = BM_MAC_Name_Con ;
                mac_msg2bm.p1  = (unsigned short) MAC_RESULT_NAME_FAILURE ;
                zas_send(WSNR_BM, &mac_msg2bm) ;
                Disable(MAC_DIR1)  ;
                Disable(MAC_DIR2)  ;
                p_WTB_status->node_strength = L_UNDEFINED ;
                mac_state = S_IDLE ;
              }
              else
              {
                Send(mac_main_direction, &SndFrm, D_MODE_ACK) ;
                /* S_NAMING_AWAIT_END_RES ; */
              }
            }
          }
          else
          {
            mac_general_events();
          }
          break;
        }
        /*
        S_NAMING_AWAIT_INT_RES
        */
        case S_NAMING_AWAIT_INT_RES:
        {
          if (((mac_event == UDF_DIR1) && (mac_main_direction == MAC_DIR1)) ||
              ((mac_event == UDF_DIR2) && (mac_main_direction == MAC_DIR2)))
          {
            if (
                 (mac_sub_event    == D_OK)                  &&
                 (mac_rcv_frame -> llc == SVF_SETINT_RESPONSE)
               )
            {
              /*
              set INT-Bit in  mac_NS_array[x].node_report
              */
              mac_NS_array[mac_ct[mac_cur_dir].Node].node_report |= INT_BIT ;
              mac_ct[mac_cur_dir].Job = Name ;
              mac_ct[mac_cur_dir].AwakeTime = SYSTIME + TMO_SWITCH_MASTER ;
              Naming_Schedule() ;
            }
            else
            {
              if (mac_retry == 1)
              {
                /*
                BM_MAC_Name_Con(MAC_RESULT_NAME_FAILURE)
                */
                mac_msg2bm.enr = BM_MAC_Name_Con ;
                mac_msg2bm.p1  = (unsigned short) MAC_RESULT_NAME_FAILURE ;
                zas_send(WSNR_BM, &mac_msg2bm) ;
                Disable(MAC_DIR1)  ;
                Disable(MAC_DIR2)  ;
                p_WTB_status->node_strength = L_UNDEFINED ;
                mac_state = S_IDLE ;
              }
              else
              {
                mac_retry-- ;
                Send(mac_main_direction,&SndFrm,D_MODE_ACK) ;
                /* mac_state = S_NAMING_AWAIT_INT_RES ; */
              }
            }
          }
          else
          {
            mac_general_events();
          }
          break;
        }
        /*
        S_NAMING_AWAIT_DET_RES
        */
        case S_NAMING_AWAIT_DET_RES:
        {
          if  ( (mac_event == UDF_DIR1) ||
                (mac_event == UDF_DIR2) )
          {
            SendDetectReqEpilog() ;
            mac_tmp_RS = mac_aux_RS ;
            mac_tmp_remote_inhibit = mac_aux_NS & INH_BIT ;
            Naming_Process_Status_Res() ;
          }
          else
          {
            mac_general_events();
          }
          break;
        }
        /*
        S_NAMING_AWAIT_STATUS_RES
        */
        case S_NAMING_AWAIT_STATUS_RES :
        {
          if (((mac_event == UDF_DIR1) && (mac_main_direction == MAC_DIR1)) ||
              ((mac_event == UDF_DIR2) && (mac_main_direction == MAC_DIR2)))
          {
            d_tc_timer_reset(MAC_TIMER2,0) ;
            if ((mac_sub_event == D_OK) &&
                ((mac_rcv_frame -> llc & (~RI_BIT)) == SVF_STATUS_RESPONSE))
            {
              /*
              status response received
              */
              /*
              testing for line error 4.0-8/4.1-33
              */
              if (  (mac_line & (D_LINE_A_ERR | D_LINE_B_ERR) )
              && (mac_addr_inaug_line_error == 0)
              )
              {
                mac_addr_inaug_line_error = mac_rcv_frame->sd ;
              }
              else
              {
                /* nothing to do */
              }
              mac_tmp_RS = mac_rcv_frame->data[POS_RS] ;
              mac_tmp_remote_inhibit = mac_rcv_frame -> llc & RI_BIT ;
              mac_NS_array[mac_ct[mac_cur_dir].Node].user_report = mac_rcv_frame->data[POS_UR] ;
              mac_tmp_NR = mac_rcv_frame->data[POS_NR] ;
              mac_NS_array[mac_ct[mac_cur_dir].Node].node_report = mac_tmp_NR ;
              mac_ND_array[mac_ct[mac_cur_dir].Node].node_frame_size       = mac_rcv_frame->data[POS_NF] ;
              mac_ND_array[mac_ct[mac_cur_dir].Node].node_period           = mac_rcv_frame->data[POS_NP] ;
              mac_ND_array[mac_ct[mac_cur_dir].Node].node_key.node_type    = mac_rcv_frame->data[POS_NT] ;
              mac_ND_array[mac_ct[mac_cur_dir].Node].node_key.node_version = mac_rcv_frame->data[POS_NV] ;
              /* if first status poll after naming then
              send up to MAX_TRY Status.Reqs when OL is disturbed */

              if (mac_ct[mac_cur_dir].FirstStatus)
              {
                /*
                double fault recognition:
                find out, if any master or slave signals that its main channel line (relative to master)
                is disturbed (update "mac_main_A_ok" or "mac_main_B_ok").
                */
                /* get line status bits */
                d_get_cur_line_status (& tmp_status) ;
                /* evaluate DA, DB of main direction in master AND slave */
                if (mac_main_direction == MAC_DIR1)
                {
                  /*
                  treat direction 1
                  */
                  /* if slave node has same orientation */
                  if (mac_tmp_NR & SAM_BIT)
                  {
                    /* slave "before" master */
                    if (mac_cur_dir == MAC_DIR1)
                    {
                      if ((tmp_status & DA1_BIT)||(mac_tmp_NR & DA2_BIT))
                      {

                      }
                      else
                      {
                        mac_main_A_ok = TRUE ;
                      }
                      if ((tmp_status & DB1_BIT)||(mac_tmp_NR & DB2_BIT))
                      {

                      }
                      else
                      {
                        mac_main_B_ok = TRUE ;
                      }
                    }
                    else
                    {
                      if ((tmp_status & DA1_BIT)||(mac_tmp_NR & DA1_BIT))
                      {

                      }
                      else
                      {
                        mac_main_A_ok = TRUE ;
                      }
                      if ((tmp_status & DB1_BIT)||(mac_tmp_NR & DB1_BIT))
                      {

                      }
                      else
                      {
                        mac_main_B_ok = TRUE ;
                      }
                    }
                  }
                  else
                  {
                    /* slave "before" master */
                    if (mac_cur_dir == MAC_DIR1)
                    {
                      if ((tmp_status & DA1_BIT)||(mac_tmp_NR & DB1_BIT))
                      {

                      }
                      else
                      {
                        mac_main_A_ok = TRUE ;
                      }
                      if ((tmp_status & DB1_BIT)||(mac_tmp_NR & DA1_BIT))
                      {

                      }
                      else
                      {
                        mac_main_B_ok = TRUE ;
                      }
                    }
                    else
                    {
                      if ((tmp_status & DA1_BIT)||(mac_tmp_NR & DB2_BIT))
                      {

                      }
                      else
                      {
                        mac_main_A_ok = TRUE ;
                      }
                      if ((tmp_status & DB1_BIT)||(mac_tmp_NR & DA2_BIT))
                      {

                      }
                      else
                      {
                        mac_main_B_ok = TRUE ;
                      }
                    }
                  }
                }
                else
                {
                  /*
                  treat direction 2
                  */
                  /* if slave node has same orientation */
                  if (mac_tmp_NR & SAM_BIT)
                  {
                    /* slave "before" master */
                    if (mac_cur_dir == MAC_DIR1)
                    {
                      if ((tmp_status & DA2_BIT)||(mac_tmp_NR & DA2_BIT))
                      {

                      }
                      else
                      {
                        mac_main_A_ok = TRUE ;
                      }
                      if ((tmp_status & DB2_BIT)||(mac_tmp_NR & DB2_BIT))
                      {

                      }
                      else
                      {
                        mac_main_B_ok = TRUE ;
                      }
                    }
                    else
                    {
                      if ((tmp_status & DA2_BIT)||(mac_tmp_NR & DA1_BIT))
                      {

                      }
                      else
                      {
                        mac_main_A_ok = TRUE ;
                      }
                      if ((tmp_status & DB2_BIT)||(mac_tmp_NR & DB1_BIT))
                      {

                      }
                      else
                      {
                        mac_main_B_ok = TRUE ;
                      }
                    }
                  }
                  else
                  {
                    /* slave "before" master */
                    if (mac_cur_dir == MAC_DIR1)
                    {
                      if ((tmp_status & DA2_BIT)||(mac_tmp_NR & DB1_BIT))
                      {

                      }
                      else
                      {
                        mac_main_A_ok = TRUE ;
                      }
                      if ((tmp_status & DB2_BIT)||(mac_tmp_NR & DA1_BIT))
                      {

                      }
                      else
                      {
                        mac_main_B_ok = TRUE ;
                      }
                    }
                    else
                    {
                      if ((tmp_status & DA2_BIT)||(mac_tmp_NR & DB2_BIT))
                      {

                      }
                      else
                      {
                        mac_main_A_ok = TRUE ;
                      }
                      if ((tmp_status & DB2_BIT)||(mac_tmp_NR & DA2_BIT))
                      {

                      }
                      else
                      {
                        mac_main_B_ok = TRUE ;
                      }
                    }
                  }
                }
                if (mac_main_A_ok &&
                mac_main_B_ok)
                {
                  Naming_Process_Status_Res() ;
                }
                else
                {
                  /* remember that we have received at least one valid Status.Res (on the trusted line) */
                  mac_got_status_response = TRUE ;
                  if (--mac_retry == 0)
                  {
                    /*
                    update mac_MR for double fault recognition
                    */
                    if (mac_main_A_ok)
                    {

                    }
                    else
                    {
                      mac_MR |= DA_BIT;
                    }
                    if (mac_main_B_ok)
                    {

                    }
                    else
                    {
                      mac_MR |= DB_BIT;
                    }
                    Naming_Process_Status_Res() ;
                  }
                  else
                  {
                    d_tc_timer_reset(MAC_TIMER2, TMO_AWAITDETECT) ;
                    /* line status bits are already cleared */
                    Send(mac_main_direction,&SndFrm,D_MODE_NOACK) ;
                    /* mac_state = S_NAMING_AWAIT_STATUS_RESponse */
                  }
                }
              }
              else
              {
                Naming_Process_Status_Res() ;
              }
              break; /*!!!*/
            }
          }
          else
          {
            if (mac_event == UDF_TIMER2)
            {

            }
            else
            {
              mac_general_events();
              break; /*!!!*/
            }
          }
          /*
          time out or wrong response
          */
          /*
          double fault recognition:
          find out, any master or slave signals that its main channel line (relative to master) is
          disturbed (update "mac_main_A_ok / mac_main_B_ok")
          */
          /* get line status bits */
          d_get_cur_line_status (& tmp_status) ;
          /* evaluate DA, DB of main direction */
          if (mac_main_direction == MAC_DIR1)
          {
            /* ignore if A and B are disturbed */
            if ( (tmp_status & (DA1_BIT|DB1_BIT)) == (DA1_BIT|DB1_BIT) )
            {

            }
            else
            {
              /* if A disturbed */
              if (tmp_status & DA1_BIT)
              {

              }
              else
              {
                mac_main_B_ok = TRUE ;
              }
              /* if B disturbed */
              if (tmp_status & DB1_BIT)
              {

              }
              else
              {
                mac_main_A_ok = TRUE ;
              }
            }
          }
          else
          {
            /* ignore if A and B are disturbed */
            if ( (tmp_status & (DA2_BIT|DB2_BIT)) == (DA2_BIT|DB2_BIT) )
            {

            }
            else
            {
              /* if A disturbed */
              if (tmp_status & DA2_BIT)
              {

              }
              else
              {
                mac_main_B_ok = TRUE ;
              }
              /* if B disturbed */
              if (tmp_status & DB2_BIT)
              {

              }
              else
              {
                mac_main_A_ok = TRUE ;
              }
            }
          }
          if (--mac_retry == 0)
          {
            if (mac_got_status_response)
            {
              /*
              update mac_MR for double fault recognition
              */
              if (mac_main_A_ok)
              {

              }
              else
              {
                mac_MR |= DA_BIT;
              }
              if (mac_main_B_ok)
              {

              }
              else
              {
                mac_MR |= DB_BIT;
              }
              Naming_Process_Status_Res() ;
            }
            else
            {
              /*
              BM_MAC_Name_Con(MAC_RESULT_NAME_FAILURE)
              */
              mac_msg2bm.enr = BM_MAC_Name_Con ;
              mac_msg2bm.p1  = (unsigned short) MAC_RESULT_NAME_FAILURE ;
              zas_send(WSNR_BM, &mac_msg2bm) ;
              Disable(MAC_DIR1)  ;
              Disable(MAC_DIR2)  ;
              p_WTB_status->node_strength = L_UNDEFINED ;
              mac_state = S_IDLE ;
            }
          }
          else
          {
            d_tc_timer_reset(MAC_TIMER2, TMO_AWAITDETECT) ;
            /* line status bits are cleared */
            Send(mac_main_direction,&SndFrm,D_MODE_NOACK) ;
            /* mac_state = S_NAMING_AWAIT_STATUS_RESponse */
          }
          break;
        }
        /*
        S_MASTER.....
        */
        /*  states when distributing topography */
        /*
        S_MASTER_AWAIT_TOPO_RES
        */
        case S_MASTER_AWAIT_TOPO_RES:
        {
#ifdef UMS
          int new;
          int master_sc = (mac_event == SC_DIR12 && mac_NA_array[mac_ix] == ADR_MASTER);
#endif
          if (((mac_event == UDF_DIR1) && (mac_main_direction == MAC_DIR1)) ||
              ((mac_event == UDF_DIR2) && (mac_main_direction == MAC_DIR2))
#ifdef UMS
              || master_sc
#endif
              )
          {
#ifdef UMS
            /* Topo distribution for UIC 556 - 12. draft */
            if (((mac_sub_event == D_OK) &&
                 (mac_rcv_frame->llc == SVF_TOPO_RESPONSE))
                || master_sc
                ) {

              /*
               * MasterTopoRes1
               */
              /* check continuous counting addresses in topo responses:
               * ... 62 63 1 2 3 ...
               * NOTE: there is no response frame from the master at the master;
               *       identify the master by checking the event SC_DIR12
               */
              new = (mac_event == SC_DIR12) ? ADR_MASTER : mac_rcv_frame->sd;

/* <ATR:01> */
              if (new != ADR_MASTER) {
                  li_t_SetFrame(mac_rcv_frame->sd, mac_rcv_frame->data,
                                            mac_rcv_frame->size);
              }

              if (new != wtb_topo_node) {
                if ((wtb_topo_node == 63 && new == ADR_MASTER) ||
                    (new - wtb_topo_node == 1 /* difference one */)) {
                  wtb_topo_node = new;
                }
                else {
                  wtb_restricted = TRUE;
                }
              }
            }
            if (mac_NA_array[mac_ix] == mac_topo.top_address && uic_repeat == 1) {
              if (wtb_restricted == FALSE &&
                  wtb_topo_node != mac_topo.top_address) {
                wtb_restricted = TRUE;
              }

              if (wtb_restricted) {
                Disable(MAC_DIR1);
                Disable(MAC_DIR2);

                md_free_packets();

                /*
                  BM_MAC_MasterStop_Ind(MAC_MASTER_STOP_FAILURE)
                  */
                mac_msg2bm.enr = BM_MAC_MasterStop_Ind;
                mac_msg2bm.p1  = (unsigned short) MAC_MASTER_STOP_FAILURE;
                zas_send(WSNR_BM, &mac_msg2bm);

                mac_state = S_IDLE;
              }
              else {
                MasterPrepare();

                d_tc_timer_reset(MAC_TIMER1,TMO_TOPO);

                mac_state = S_MASTER_AWAIT_TOPO_DELAY;
              }

#ifdef O_MVBC
//            lc_set_device_status_word(LC_DSW_DYD_MSK, LC_DSW_DYD_CLR);
#endif
            }
            else {
              if (--uic_repeat <= 0) {
                uic_repeat = MAX_UIC_REPEAT;
                mac_ix++;
              }

              if (mac_NA_array[mac_ix] == ADR_MASTER) {
                MasterSendTopoResp();
              }
              else {
                MasterSendTopoReq();

                /* mac_state = S_MASTER_AWAIT_TOPO_RES */
              }
            }

#else
            if (
                 (mac_sub_event == D_OK) &&
                 (mac_rcv_frame -> llc == SVF_TOPO_RESPONSE)
               )


            /*  || ( (mac_retry == 1) &&
                      (p_WTB_status->node_strength == L_STRONG ) ) */
            {
              /*
              MasterTopoRes1
              */
              /* MasterTopoRes1 */
              if (mac_NA_array[mac_ix] == mac_topo.top_address)
              {
                MasterPrepare() ;
                d_tc_timer_reset(MAC_TIMER1,TMO_TOPO) ;
                mac_state = S_MASTER_AWAIT_TOPO_DELAY ;
              }
              else
              {
                mac_ix++ ;
                mac_retry = mac_max_try /*4.0-7a*/;
                if (mac_NA_array[mac_ix] == ADR_MASTER)
                {
                  if (mac_NA_array[mac_ix] == mac_topo.top_address)
                  {
                    MasterPrepare() ;
                    d_tc_timer_reset(MAC_TIMER1,TMO_TOPO) ;
                    mac_state = S_MASTER_AWAIT_TOPO_DELAY ;
                  }
                  else
                  {
                    mac_ix++ ;
                    MasterSendTopoReq() ;
                    /* mac_state = S_MASTER_AWAIT_TOPO_RES */
                  }
                }
                else
                {
                  MasterSendTopoReq() ;
                  /* mac_state = S_MASTER_AWAIT_TOPO_RES */
                }
              }
            }
            else
            {
              if (--mac_retry == 0)
              {
                Disable(MAC_DIR1)  ;
                Disable(MAC_DIR2)  ;
                md_free_packets() ;
                /*
                BM_MAC_MasterStop_Ind(MAC_MASTER_STOP_FAILURE)
                */
                mac_msg2bm.enr = BM_MAC_MasterStop_Ind ;
                mac_msg2bm.p1  = (unsigned short) MAC_MASTER_STOP_FAILURE ;
                zas_send(WSNR_BM, &mac_msg2bm) ;
#ifdef O_MVBC
//              lc_set_device_status_word(LC_DSW_DYD_MSK, LC_DSW_DYD_CLR);
#endif
                mac_state = S_IDLE ;
              }
              else
              {
                Send(mac_main_direction,&SndFrm,D_MODE_ACK) ;
                /* mac_state = S_MASTER_AWAIT_TOPO_RES*/
              }
            }
#endif
          }
          else
          {
            mac_general_events();
          }
          break;
        }
        /*
        S_MASTER_AWAIT_TOPO_DELAY
        */
        case S_MASTER_AWAIT_TOPO_DELAY:
        {
          if (mac_event == UDF_TIMER1)
          {
            p_statistic_data->basic_period_cnt = 0;
            p_statistic_data->topo_cnt++;
            mac_time_account = 0;
            mac_cycle_start_time = SYSTIME ;
#ifdef O_MVBC
//          lc_set_device_status_word(LC_DSW_DYD_MSK, LC_DSW_DYD_CLR);
#endif
            Master_Basic_Cycle() ;
          }
          else
          {
            mac_general_events() ;
          }
          break;
        }
        /*  states when in regular operation */
        /*
        S_MASTER_AWAIT_PD_RES
        */
        case S_MASTER_AWAIT_PD_RES:
        {
          if (mac_event == SC_DIR12)    /*4.0-6*/
          {
            /*
            report Send_Confirm      4.0-6
            */
            CLEAR_SC_FLAG ;
            if (p2_TmpFrm -> size == 0)
            {

            }
            else
            {
              pd_send_confirm (p2_TmpFrm) ;
            }
          }
          else
          {
            if (((mac_event == UDF_DIR1) && (mac_main_direction == MAC_DIR1)) ||
                ((mac_event == UDF_DIR2) && (mac_main_direction == MAC_DIR2)))
            {
              if (
                   (mac_sub_event == D_OK) &&
                  ((mac_rcv_frame -> llc & ACI_MASK) ==
                                     PD_RESPONSE)
                 )
              {
                /*
                evaluate PD response
                */
                if (mac_rcv_frame -> sd == PD_req_frm.dd)
                {
                  if (mac_poll_list[mac_ix].EC == 1)
                  {
                    AddSPList(PD_req_frm.dd) ;
                  }
                  /* MasterPDRes1 */
                  mac_poll_list[mac_ix].EC = mac_max_poll_try /*4.0-7a*/ ;
                  pd_receive_indicate((type_idu*) mac_rcv_frame) ;
                  if (mac_rcv_frame -> llc & A_BIT)
                  {
                    AddMDList(PD_req_frm.dd) ;
                  }
                  if (mac_rcv_frame -> llc & C_BIT)
                  {
                    AddSPList(PD_req_frm.dd) ;
                  }
                  mac_inh_array[mac_NIx_array[PD_req_frm.dd]] = (mac_rcv_frame -> llc & I_BIT) ;
                }
                else
                {
                  diagwtb_entry(
                  DIAG_MAC_ERR_SD,
                  (unsigned short) mac_state,
                  (unsigned short) mac_event,
                  0x0003,
                  (void*)
                  (0xFFFF0000+((mac_rcv_frame -> sd) | ((PD_req_frm.dd)<<8)))

                  ) ;
                }
                mac_ix++ ;
                Master_PD_Phase() ;
              }
              else
              {
                if (mac_poll_list[mac_ix].EC == 1)
                {
                  /*
                  Send BM_MAC_WTBNodes_Ind if not already sent
                  */
                  if (
                  (p_WTB_nodes->node_status_list[mac_NIx_array[PD_req_frm.dd]].
                  node_report == InvalidStatus.node_report) &&
                  (p_WTB_nodes->node_status_list[mac_NIx_array[PD_req_frm.dd]].
                  user_report == InvalidStatus.user_report)
                  )
                  {

                  }
                  else
                  {
                    p_WTB_nodes->node_status_list[mac_NIx_array[PD_req_frm.dd]] = InvalidStatus ;
                    /*
                    BM_MAC_WTBNodes_Ind
                    */
                    mac_msg2bm.enr = BM_MAC_WTBNodes_Ind ;
                    zas_send(WSNR_BM, &mac_msg2bm) ;
                  }
                }
                else
                {
                  mac_poll_list[mac_ix].EC-- ;
                }
                mac_ix++ ;
                Master_PD_Phase() ;
              }
            }
            else
            {
              mac_general_events();
            }

          }
          break;
        }
        /*
        S_MASTER_AWAIT_SC_PD_REQ
        */
        case S_MASTER_AWAIT_SC_PD_REQ:
        {
          if (mac_event == SC_DIR12)
          {
            CLEAR_SC_FLAG ;
            /*
            OwnPD
            */
            p2_TmpFrm = (type_idu_drv*) pd_request_to_send (ADR_BROADCAST) ;
            if ( p2_TmpFrm == (type_idu_drv*)0 )
            {
              SndFrm.sd = ADR_MASTER;
              SndFrm.dd = ADR_BROADCAST ;
              SndFrm.size = 0 ;
              if ( md_check_for_data() )
              {
                SndFrm.llc = PD_RESPONSE_A | mac_CI_bits ;
                AddMDList(ADR_MASTER) ;
              }
              else
              {
                SndFrm.llc = PD_RESPONSE | mac_CI_bits ;
              }
              SET_SC_FLAG ;
              Send(mac_main_direction, &SndFrm, D_MODE_NOACK) ;
            }
            else
            {
              p2_TmpFrm -> sd = ADR_MASTER;
              p2_TmpFrm -> dd = ADR_BROADCAST ;
              if ( md_check_for_data() )
              {
                p2_TmpFrm -> llc = PD_RESPONSE_A | mac_CI_bits ;
                AddMDList(ADR_MASTER) ;
              }
              else
              {
                p2_TmpFrm -> llc = PD_RESPONSE | mac_CI_bits ;
              }
              SET_SC_FLAG ;
              Send(mac_main_direction, p2_TmpFrm, D_MODE_NOACK) ;
            }
            /* update masters inhibit bit */
            mac_inh_array[mac_NIx_array[ADR_MASTER]] = (mac_CI_bits & I_BIT) ;
            /*
            OwnPDCBit
            */
            /* OwnPDCBit */
            if (mac_CI_bits & C_BIT)
            {
              /*
              Clear C-Bit
              */
              pi_disable() ;
              mac_CI_bits &= (~C_BIT) ;
              pi_enable() ;
              /* add master to SP-List */
              AddSPList(ADR_MASTER) ;
            }
            mac_ix++ ;
            mac_state = S_MASTER_AWAIT_SC_PD_RES ;
            /* break;                     4.1-15*/
          }
          else
          {
            mac_general_events();
          }
          break;                      /*4.1-15*/
        }
        /*
        S_MASTER_AWAIT_SC_PD_RES
        */
        case S_MASTER_AWAIT_SC_PD_RES:
        {
          if (mac_event == SC_DIR12)
          {
            CLEAR_SC_FLAG ;
            if (p2_TmpFrm -> size == 0)
            {

            }
            else
            {
              pd_send_confirm (p2_TmpFrm) ;
            }
            Master_PD_Phase() ;
          }
          else
          {
            mac_general_events();
          }
          break;
        }
        /*  status poll */
        /*
        S_MASTER_AWAIT_STATUS_RES
        */
        case S_MASTER_AWAIT_STATUS_RES :
        {
          if (
              ((mac_event == UDF_DIR1) && (mac_main_direction == MAC_DIR1)) ||
              ((mac_event == UDF_DIR2) && (mac_main_direction == MAC_DIR2))
             )
          {
            if (
                 (mac_sub_event == D_OK) &&
                 (mac_rcv_frame -> llc == SVF_STATUS_RESPONSE)    )
            {
              /*
              evaluate status response
              */
              if (
                  (p_WTB_nodes->node_status_list[mac_NIx_array[SndFrm.dd]].node_report ==
              InvalidStatus.node_report) &&
                  (p_WTB_nodes->node_status_list[mac_NIx_array[SndFrm.dd]].user_report ==
              InvalidStatus.user_report)
                 )
              {
                /*
                indicate that failed intermediate node is alive again:
                issue BM_MAC_WTBNodes_Ind
                */
                mac_msg2bm.enr = BM_MAC_WTBNodes_Ind ;
                zas_send(WSNR_BM, &mac_msg2bm) ;
              }
              /* MasterStatusRes1 */
              /* Update WTB_nodes */
              p_WTB_nodes->node_status_list[mac_NIx_array[SndFrm.dd]].node_report = mac_rcv_frame ->
              data[POS_NR] & (~DSC_BIT) ;
              p_WTB_nodes->node_status_list[mac_NIx_array[SndFrm.dd]].user_report = mac_rcv_frame ->
              data[POS_UR] ;
              scan_WTB_nodes() ;
              /* update sleep bit in mac_slp_array */
              mac_slp_array[mac_NIx_array[SndFrm.dd]] = mac_rcv_frame -> data[POS_NR] & SLP_BIT ;
              if (mac_rcv_frame -> data[POS_NR] & DSC_BIT)
              {
                /*
                store new node descriptor in mac_ND_array
                */
                mac_ND_array[SndFrm.dd].node_frame_size = mac_rcv_frame -> data[POS_NF] ;
                mac_ND_array[SndFrm.dd].node_period = mac_rcv_frame -> data[POS_NP] ;
                mac_ND_array[SndFrm.dd].node_key.node_type = mac_rcv_frame -> data[POS_NT] ;
                mac_ND_array[SndFrm.dd].node_key.node_version = mac_rcv_frame -> data[POS_NV] ;
                Master_Prepare_Topo() ;
              }
              else
              {
                Master_AP_Phase() ;
              }
            }
            else
            {
              /*
              check WTB_status
              */
              if (
                  (p_WTB_nodes->node_status_list[mac_NIx_array[SndFrm.dd]].node_report ==
              InvalidStatus.node_report) &&
                  (p_WTB_nodes->node_status_list[mac_NIx_array[SndFrm.dd]].user_report ==
              InvalidStatus.user_report)
                 )
              {

              }
              else
              {
                /* add again at end of SP-list if node is not a failing intermediate node */
                AddSPList(SndFrm.dd) ;
              }
              Master_AP_Phase() ;
            }
          }
          else
          {
            mac_general_events();
          }
          break;
        }
        /*  MD traffic */
        /*
        S_MASTER_AWAIT_MD_RES
        */
        case S_MASTER_AWAIT_MD_RES:
        {
          if (((mac_event == UDF_DIR1) && (mac_main_direction == MAC_DIR1)) ||
              ((mac_event == UDF_DIR2) && (mac_main_direction == MAC_DIR2)))
          {
            if (
                  (mac_sub_event == D_OK)          &&
                 ((mac_rcv_frame -> llc & ACI_MASK) ==
                         MD_RESPONSE)
               )
            {
              /*
              MasterMDRes1
              */
              if (mac_rcv_frame -> sd == MD_req_frm.dd)
              {
                /* MasterMDRes1 */
                if (mac_rcv_frame -> llc & A_BIT)
                {
                  AddMDList(MD_req_frm.dd) ;
                }
                if (mac_rcv_frame -> llc & C_BIT)
                {
                  AddSPList(MD_req_frm.dd) ;
                }
                /* ignore inhibit bit here */
                /* loopback */
                if (((mac_rcv_frame -> dd == ADR_MASTER) && (mac_rcv_frame -> size != 0)) || (mac_rcv_frame
                -> dd == ADR_BROADCAST) )
                {
                  p_statistic_data->NoRMD++;
                  md_receive_indicate(mac_rcv_frame) ;
                }
              }
              else
              {
                diagwtb_entry(
                DIAG_MAC_ERR_SD,
                (unsigned short) mac_state,
                (unsigned short) mac_event,
                0x0002,
                (void*)
                (0xFFFF0000+((mac_rcv_frame -> sd) | ((MD_req_frm.dd)<<8)))
                ) ;
              }
            }
            else
            {
              /* ignore timout since node will announce message data again in next PD poll */
            }
            Master_AP_Phase() ;
          }
          else
          {
            mac_general_events();
          }
          break;
        }
        /*
        S_MASTER_AWAIT_SC_MD_REQ
        */
        case S_MASTER_AWAIT_SC_MD_REQ:
        {
          if (mac_event == SC_DIR12)
          {
            CLEAR_SC_FLAG ;
            /*
            MDPhaseOwnMD
            */
            /* MDPhaseOwnMD */
            p2_TmpFrm = md_request_to_send ( &mac_MD_addr ) ;
            if (p2_TmpFrm == (type_idu_drv*)0)
            {
              SndFrm.dd = ADR_MASTER ;
              SndFrm.size = 0 ;
              SndFrm.sd = ADR_MASTER ;
              if ( md_check_for_data() )
              {
                SndFrm.llc = MD_RESPONSE_A | mac_CI_bits ;
                AddMDList(ADR_MASTER) ;
              }
              else
              {
                SndFrm.llc = MD_RESPONSE | mac_CI_bits ;
              }
              SET_SC_FLAG ;
              Send(mac_main_direction, &SndFrm, D_MODE_NOACK) ;
            }
            else
            {
              p2_TmpFrm -> dd = mac_MD_addr ;
              p2_TmpFrm -> sd = ADR_MASTER ;
              if ( md_check_for_data() )
              {
                p2_TmpFrm -> llc = MD_RESPONSE_A | mac_CI_bits ;
                AddMDList(ADR_MASTER) ;
              }
              else
              {
                p2_TmpFrm -> llc = MD_RESPONSE | mac_CI_bits ;
              }
              SET_SC_FLAG ;
              Send(mac_main_direction, p2_TmpFrm, D_MODE_NOACK) ;
              /*
              loopback
              */
              if ( (mac_MD_addr == ADR_BROADCAST) ||
              (mac_MD_addr == ADR_MASTER   )   )
              {
                /* activate message data interface */
                md_receive_indicate(p2_TmpFrm) ;
              }
            }
            /* MDPhaseOwnMD1 */
            if (mac_CI_bits & C_BIT)
            {
              /*
              Clear C-Bit
              */
              pi_disable() ;
              mac_CI_bits &= (~C_BIT) ;
              pi_enable() ;
              /* add master to SP-List */
              AddSPList(ADR_MASTER) ;
            }
            p_statistic_data->NoTMD++;
            mac_state = S_MASTER_AWAIT_SC_MD_RES ;
            /*break;                      4.1-15*/
          }
          else
          {
            mac_general_events();
          }
          break;                      /*4.1-15*/
        }
        /*
        S_MASTER_AWAIT_SC_MD_RES
        */
        case S_MASTER_AWAIT_SC_MD_RES:
        {
          if (mac_event == SC_DIR12)
          {
            CLEAR_SC_FLAG ;
            if (p2_TmpFrm -> size == 0)
            {

            }
            else
            {
              md_send_confirm () ;
            }
            Master_AP_Phase() ;
          }
          else
          {
            mac_general_events();
          }
          break;
        }
        /*  presence poll */
        /*
        S_MASTER_AWAIT_DET_RES
        */
        case S_MASTER_AWAIT_DET_RES:
        {
          if (((mac_event == UDF_DIR1) && (mac_main_direction == MAC_DIR2)) ||
              ((mac_event == UDF_DIR2) && (mac_main_direction == MAC_DIR1)))
          {
            SendDetectReqEpilog() ;
            SendPresenceRes();
            mac_state = S_MASTER_AWAIT_SC_PRESENCE_RES ;
          }
          else
          {
            mac_general_events();
          }
          break;
        }
        /*
        S_MASTER_AWAIT_SC_PRESENCE_RES
        */
        case S_MASTER_AWAIT_SC_PRESENCE_RES:
        {
          if (mac_event == SC_DIR12)
          {
            CLEAR_SC_FLAG ;
            update_line_status_bits();
            mac_tmp_RS = mac_aux_RS ;
            mac_tmp_remote_inhibit = mac_aux_NS & INH_BIT ;
            Master_Process_Presence_Res() ;
          }
          else
          {
            mac_general_events();
          }
          break;
        }
        /*
        S_MASTER_AWAIT_PRESENCE_RES
        */
        case S_MASTER_AWAIT_PRESENCE_RES:
        {
          if (((mac_event == UDF_DIR1) && (mac_main_direction == MAC_DIR1)) ||
              ((mac_event == UDF_DIR2) && (mac_main_direction == MAC_DIR2)))
          {
            d_tc_timer_reset(MAC_TIMER2, 0) ;
            if (((mac_rcv_frame -> llc) & ACI_MASK ) ==     SVF_PRESENCE_RESPONSE)
            {
              /*
              */
              /*
              handle fritting         4.0-7b
              */
              /*4.0-7b..*/
              if (p_WTB_status->node_status.node_report & INT_BIT)
              {

              }
              else
              {
                /* this_node is an end-node */
                mac_fritting_test_main( mac_line); /*4.1-33*/
              }
              /*..4.0-7b*/
              update_line_status_bits();
              if (Pre_req_frm.dd == mac_rcv_frame -> sd)
              {
                mac_present[mac_cur_dir] = mac_max_status_fail;    /*4.0-7a*/
                mac_tmp_RS = mac_rcv_frame -> data[POS_RS] ;
                mac_tmp_remote_inhibit = mac_rcv_frame -> llc & RI_BIT ;
                Master_Process_Presence_Res() ;
              }
              else
              {
                diagwtb_entry(
                DIAG_MAC_ERR_SD,
                (unsigned short) mac_state,
                (unsigned short) mac_event,
                0x0001,
                (void*)
                (0xFFFF0000+((mac_rcv_frame -> sd) | ((Pre_req_frm.dd)<<8)))
                ) ;
                if (--mac_present[mac_cur_dir] == 0)
                {
                  /*
                  BM_MAC_MasterEndNodeFail_Ind
                  */
                  mac_msg2bm.enr = BM_MAC_MasterEndNodeFail_Ind ;
                  zas_send(WSNR_BM, &mac_msg2bm) ;
                  mac_present[mac_cur_dir] = mac_max_status_fail;/*4.1-36*/
                }
                Master_AP_Phase() ;
              }
            }
            else
            {
              if (--mac_present[mac_cur_dir] == 0)
              {
                /*
                BM_MAC_MasterEndNodeFail_Ind
                */
                mac_msg2bm.enr = BM_MAC_MasterEndNodeFail_Ind ;
                zas_send(WSNR_BM, &mac_msg2bm) ;
                mac_present[mac_cur_dir] = mac_max_status_fail;/*4.1-36*/
              }
              Master_AP_Phase() ;
            }
          }
          else
          {
            if (mac_event == UDF_TIMER2)
            {
              if (--mac_present[mac_cur_dir] == 0)
              {
                /*
                BM_MAC_MasterEndNodeFail_Ind
                */
                mac_msg2bm.enr = BM_MAC_MasterEndNodeFail_Ind ;
                zas_send(WSNR_BM, &mac_msg2bm) ;
                mac_present[mac_cur_dir] = mac_max_status_fail;/*4.1-36*/
              }
              Master_AP_Phase() ;
            }
            else
            {
              mac_general_events();
            }
          }
          break;
        }
        /*  idle phase */
        /*
        S_MASTER_IDLE_PHASE
        */
        case S_MASTER_IDLE_PHASE :
        {
          if (mac_event == UDF_TIMER1)
          {
            Master_Basic_Cycle() ;
          }
          else
          {
            mac_general_events() ;
          }
          break;
        }
        /*
        S_SLAVE......
        */
        /*  states when waiting for topography */
        /*
        S_SLAVE_TOPO
        */
        case S_SLAVE_TOPO :
      mac_S_SLAVE_TOPO:
        {
          if (((mac_event == UDF_DIR1) && (mac_main_direction == MAC_DIR1)) ||
              ((mac_event == UDF_DIR2) && (mac_main_direction == MAC_DIR2)))
          {
            if (mac_sub_event == D_OK)
            {
              switch (mac_rcv_frame -> llc)
              {
                /*
                SVF_TOPO_REQUEST
                */
                case SVF_TOPO_REQUEST:
                {
#ifdef UMS
                  d_tc_timer_reset(MAC_TIMER1, TMO_SLAVE_EXP_STATUS);
#endif
                  Topo_res_frm.sd   = p_WTB_status->node_address;
                  Topo_res_frm.llc  = SVF_TOPO_RESPONSE;
#ifndef UMS
                  Topo_res_frm.dd   = ADR_MASTER;
#else
                  Topo_res_frm.dd   = ADR_BROADCAST;
                  memcpy(Topo_res_frm.data, li_t_GetOwnInaugData(), 128);
                  li_t_ChgInaugDataSD(Topo_res_frm.sd |
                                      ((p_WTB_status->node_orient != L_INVERSE) << 7));
#endif
                  Topo_res_frm.size = SVF_TOPO_RESPONSE_SIZE;

                  Send(mac_main_direction, &Topo_res_frm, D_MODE_NOACK) ;
#ifdef UMS
                  li_t_Sync();
#endif
                  mac_my_str = mac_rcv_frame -> data[POS_LS];
                  mac_topo.node_address = p_WTB_status->node_address;
                  mac_topo.node_orient  = p_WTB_status->node_orient;
                  if (++mac_topo.topo_counter == (MAX_TOPO_COUNT + 1))
                  {
                    mac_topo.topo_counter = 1;
                  }
                  mac_topo.individual_period = 1 << (mac_rcv_frame -> data[POS_MT_H__YP] >> 4);
                  mac_topo.is_strong         = ((mac_my_str & STRONG_BIT) == STRONG_BIT);
                  mac_topo.number_of_nodes   = (mac_my_str & (~STRONG_BIT)) >> 1;
                  mac_topo.bottom_address    = mac_rcv_frame -> data[POS_BA];
#ifdef UMS
                /* check continuous counting addresses in topo responses:
                 * ... 62 63 1 2 3 ...
                 */
                if (mac_topo.node_address == mac_topo.bottom_address) {
                    wtb_topo_node = mac_topo.node_address;
                    wtb_restricted = TRUE;
                }
                if (mac_topo.node_address != wtb_topo_node) {
                    if ((wtb_topo_node == 63 && mac_topo.node_address == ADR_MASTER) ||
                        (mac_topo.node_address - wtb_topo_node == 1)) {
                        wtb_topo_node = mac_topo.node_address ;
                    }
                }
#endif

#ifndef UMS
                  /*
                  Copy Frame -> mac_topo
                  */
                  mac_ix = 0;
                  while ( mac_ix != mac_topo.number_of_nodes )
                  {
                    mac_topo.NK[mac_ix].node_type    = mac_rcv_frame -> data[ POS_NK_OFFSET + (mac_ix << 1)
                    ];
                    mac_topo.NK[mac_ix].node_version = mac_rcv_frame -> data[ POS_NK_OFFSET + (mac_ix << 1) + 1
                    ];
                    mac_ix ++;
                  }
#endif
                  /*
                  Determine mac_topo.top_address
                  */
                  if (mac_topo.bottom_address == 1)
                  {
                    mac_topo.top_address = mac_topo.number_of_nodes ;
                  }
                  else
                  {
                    mac_topo.top_address = mac_topo.number_of_nodes + mac_topo.bottom_address - 64 ;
                  }
                  /* Write save data to mac_tmp_SV */
                  mac_tmp_SV.MT_L           =  mac_rcv_frame -> data[POS_MT_L];
                  mac_tmp_SV.MT_H__YP       = (mac_rcv_frame -> data[POS_MT_H__YP]) & 0x0F;
#ifndef UMS
                  mac_tmp_SV.WTB_status     = *p_WTB_status;
                  mac_tmp_SV.topo           = mac_topo;
                  mac_tmp_SV.main_direction = mac_main_direction;
                  mac_tmp_SV.last_node_addr = mac_last_node_addr;
                  mac_tmp_SV.my_str         = mac_my_str;
                  /*
                  BM_MAC_Topo_Ind
                  */
                  mac_msg2bm.enr = BM_MAC_Topo_Ind ;
                  mac_msg2bm.p34 = & mac_tmp_SV ;
                  zas_send(WSNR_MAC, &mac_msg2bm) ;
                  mac_presence_startup     = TRUE;
                  mac_presence_tmo[BA_DIR] = mac_max_status_fail;     /*4.0-7a*/
                  mac_presence_tmo[TA_DIR] = mac_max_status_fail;     /*4.0-7a*/
                  /*
                  init line redundancy
                  */
                  mac_last_line_status = p_WTB_status->node_status.node_report;
                  /* clear line status bits */
                  d_get_int_line_status (& tmp_status) ;
                  line_status_update_counter =  (1 << MAX_NODE_PERIOD);
                  d_tc_timer_reset(MAC_TIMER1, TMO_END_NODE_CHECK /*4.1-25*/) ;
                  p_statistic_data->basic_period_cnt = 0;
                  p_statistic_data->topo_cnt++;
                  mac_state = S_SLAVE_REGULAR ;
#ifdef O_MVBC
//                lc_set_device_status_word(LC_DSW_DYD_MSK, LC_DSW_DYD_CLR);
#endif
#else
                if (mac_topo.node_address == mac_topo.top_address) {
                    uic_repeat--;
                    if (uic_repeat <= 0) {

                        TopoPhaseEnd();
                        /* allow topo count increment */
                        topo_freeze = FALSE;

                    }
                }
#endif

                  break;
                }

#ifdef UMS
              case SVF_TOPO_RESPONSE:
                d_tc_timer_reset(MAC_TIMER1, TMO_SLAVE_EXP_STATUS);

/* <ATR:01> */
                li_t_SetFrame(mac_rcv_frame->sd, mac_rcv_frame->data,
                              mac_rcv_frame->size);

                /* check continuous counting addresses in topo responses:
                 * ... 62 63 1 2 3 ...
                 */
                if (mac_rcv_frame->sd == mac_topo.bottom_address) {
                    wtb_topo_node = mac_rcv_frame->sd;
                    wtb_restricted = TRUE;
                }
                if ((wtb_topo_node == 63 && mac_rcv_frame->sd == ADR_MASTER) ||
                    (mac_rcv_frame->sd - wtb_topo_node == 1)) {
                    wtb_topo_node = mac_rcv_frame->sd ;
                }
                if (mac_rcv_frame->sd == mac_topo.top_address) {
                    uic_repeat--;
                    if (uic_repeat <= 0) {
                        TopoPhaseEnd();
                        /* allow topo count increment */
                        topo_freeze = FALSE;
                    }
                }

                break;
#endif
                /*
                SVF_STATUS_REQUEST
                */
                case SVF_STATUS_REQUEST:
                {
                  mac_my_str = mac_rcv_frame -> data[POS_LS];
                  /*
                  set mac_MR
                  */
                  d_get_cur_line_status (&tmp_status) ;
                  /* set mac_MR: it is an OR combination of
                  1. DA_BIT/DB_BIT from Status.Req[NS] (considering my orientation)
                  2. own main channel line status bits set by Status.Req reception */
                  mac_MR = 0 ;
                  if (p_WTB_status->node_status.node_report & SAM_BIT)
                  {
                    if ((mac_rcv_frame -> data[POS_MR]) & DA_BIT)
                    {
                      mac_MR |= DA_BIT ;
                    }
                    if ((mac_rcv_frame -> data[POS_MR]) & DB_BIT)
                    {
                      mac_MR |= DB_BIT ;
                    }
                  }
                  else
                  {
                    if ((mac_rcv_frame -> data[POS_MR]) & DA_BIT)
                    {
                      mac_MR |= DB_BIT ;
                    }
                    if ((mac_rcv_frame -> data[POS_MR]) & DB_BIT)
                    {
                      mac_MR |= DA_BIT ;
                    }
                  }
                  if (mac_main_direction == MAC_DIR1)
                  {
                    if (tmp_status & DA1_BIT)
                    {
                      mac_MR |= DA_BIT ;
                    }
                    if (tmp_status & DB1_BIT)
                    {
                      mac_MR |= DB_BIT ;
                    }
                  }
                  else
                  {
                    if (tmp_status & DA2_BIT)
                    {
                      mac_MR |= DA_BIT ;
                    }
                    if (tmp_status & DB2_BIT)
                    {
                      mac_MR |= DB_BIT ;
                    }
                  }
                  SndFrm.dd   = ADR_BROADCAST ;
                  SndFrm.llc  = SVF_STATUS_RESPONSE;
                  SndFrm.sd   = p_WTB_status->node_address;
                  SndFrm.size = SVF_STATUS_RESPONSE_SIZE;
                  SndFrm.data[POS_UR] = p_WTB_status->node_status.user_report;
                  SndFrm.data[POS_NP] = mac_configuration.node_descriptor.node_period;
                  SndFrm.data[POS_NF] = mac_configuration.node_descriptor.node_frame_size;
                  SndFrm.data[POS_NT] = mac_configuration.node_descriptor.node_key.node_type;
                  SndFrm.data[POS_NV] = mac_configuration.node_descriptor.node_key.node_version;
                  SndFrm.data[POS_RES_S] = RESERVED_FIELD_VALUE ;
                  d_tc_timer_reset(MAC_TIMER1, TMO_SLAVE_EXP_STATUS) ;
                  /* if intermediate node */
                  if (p_WTB_status->node_status.node_report & INT_BIT)
                  {
                    /* this path is only used with conformance tester */
                    SndFrm.data[POS_NR] = p_WTB_status->node_status.node_report;
                    SndFrm.data[POS_RS] = 0 ;
                    Send(mac_main_direction, &SndFrm, D_MODE_NOACK) ;
                    /* same state */
                  }
                  else
                  {
                    /* if Detect.Req was really sent */
                    if (SendDetectReqProlog())
                    {
                      mac_state = S_SLAVE_TOPO_AWAIT_DETECT_RES;
                    }
                    else
                    {
                      SndFrm.data[POS_NR] = p_WTB_status->node_status.node_report;
                      SndFrm.data[POS_RS] = mac_aux_RS ;
                      Send(mac_main_direction, &SndFrm, D_MODE_NOACK) ;
                      /* same state */
                    }
                  }
                  break;
                }
                /*
                SVF_STATUS_RESPONSE
                */
                case SVF_STATUS_RESPONSE:
                {
                  d_tc_timer_reset(MAC_TIMER1, TMO_SLAVE_EXP_STATUS) ;
                  /* same state */
                  break;
                }
                /*
                SVF_SETINT_REQUEST
                */
                case SVF_SETINT_REQUEST:
                {
                  SndFrm.sd   = p_WTB_status->node_address;
                  SndFrm.llc  = SVF_SETINT_RESPONSE;
                  SndFrm.dd   = ADR_MASTER;
                  SndFrm.size = SVF_SETINT_RESPONSE_SIZE;
                  /* if end node */
                  if ((p_WTB_status->node_status.node_report & INT_BIT) == 0)
                  {
                    SET_SC_FLAG ;
                    Send(mac_main_direction, &SndFrm, D_MODE_NOACK) ;
                    mac_state = S_SLAVE_AWAIT_SC_INT_RES ;
                  }
                  else
                  {
                    Send(mac_main_direction, &SndFrm, D_MODE_NOACK) ;
                    /* same state */
                  }
                  break;
                }
                /*
                SVF_SETEND_REQUEST
                */
                case SVF_SETEND_REQUEST:
                {
                  mac_my_str = mac_rcv_frame -> data[POS_LS];
                  SndFrm.sd   = p_WTB_status->node_address;
                  SndFrm.llc  = SVF_SETEND_RESPONSE;
                  SndFrm.dd   = ADR_MASTER;
                  SndFrm.size = SVF_SETEND_RESPONSE_SIZE;
                  /* if end node */
                  if ((p_WTB_status->node_status.node_report & INT_BIT) == 0)
                  {
                    Send(mac_main_direction, &SndFrm, D_MODE_NOACK) ;
                    /* same state */
                  }
                  else
                  {
                    SET_SC_FLAG ;
                    Send(mac_main_direction, &SndFrm, D_MODE_NOACK) ;
                    mac_state = S_SLAVE_AWAIT_SC_END_RES ;
                  }
                  break;
                }
                /*
                SVF_NAME_REQUEST
                */
                case SVF_NAME_REQUEST :
                {
#ifdef O_MVBC
//                lc_set_device_status_word(LC_DSW_DYD_MSK, LC_DSW_DYD_SET);
#endif
                  SndFrm.dd   = ADR_MASTER ;
                  SndFrm.sd   = p_WTB_status->node_address;
                  SndFrm.llc  = SVF_NAME_RESPONSE ;
                  SndFrm.size = SVF_NAME_RESPONSE_SIZE ;
                  Send(mac_main_direction, &SndFrm, D_MODE_NOACK) ;
                  /* same state */
                  break;
                }
                /*
                SVF_UNNAME_REQUEST
                */
                case SVF_UNNAME_REQUEST:
                {
                  d_tc_timer_reset(MAC_TIMER1, 0) ;
                  /*
                  set SAM-Bit in p_WTB_status->node_report
                  */
                  pi_disable() ;
                  p_WTB_status->node_status.node_report |= SAM_BIT ;
                  pi_enable() ;
                  mac_last_node_addr = ADR_UNNAMED;
                  p_WTB_status->net_inhibit = FALSE ;
                  p_WTB_status->node_strength = L_UNDEFINED;
                  p_WTB_status->node_orient   = L_UNKNOWN;
                  Disable(MAC_DIR1) ;
                  Disable(MAC_DIR2) ;
                  mac_fritting_off( MAC_DIR12);                /*4.1-34*/
                  d_tc_timer_reset(MAC_TIMER2,
                  (mac_configuration.fritting_disabled == TRUE)
                  ? TMO_UNNAME : TMO_UNNAME_FRITT) ;    /*4.1-34*/
                  mac_state = S_SLAVE_UNNAME ;
#ifdef O_MVBC
//                lc_set_device_status_word(LC_DSW_DYD_MSK, LC_DSW_DYD_CLR);
#endif
                  break;
                }
                /*
                SVF_DETECT_RESPONSE
                */
                case SVF_DETECT_RESPONSE:
                {
                  diagwtb_entry(
                  DIAG_MAC_ERR_UNNAMED_BY_DETECT_RES,
                  (unsigned short) mac_state,
                  (unsigned short) mac_event,
                  0x0,
                  (void*)0
                  ) ;
                  p_WTB_status->net_inhibit = FALSE;
                  /*
                  set SAM-Bit in p_WTB_status->node_report
                  */
                  pi_disable() ;
                  p_WTB_status->node_status.node_report |= SAM_BIT ;
                  pi_enable() ;
                  p_WTB_status->node_strength = L_UNDEFINED;
                  p_WTB_status->node_orient   = L_UNKNOWN;
                  Disable(MAC_DIR1) ;
                  Disable(MAC_DIR2) ;
                  /*
                  BM_MAC_SlaveStop_Ind (MAC_SLAVE_STOP_ENDNODEFAIL)
                  */
                  mac_msg2bm.enr = BM_MAC_SlaveStop_Ind ;
                  mac_msg2bm.p1 = (unsigned char) MAC_SLAVE_STOP_ENDNODEFAIL ;
                  zas_send(WSNR_MAC, &mac_msg2bm) ;
                  mac_state = S_IDLE ;
#ifdef O_MVBC
//                lc_set_device_status_word(LC_DSW_DYD_MSK, LC_DSW_DYD_CLR);
#endif
                  break;
                }
#ifdef UMS
                /* a little bit unexpected end of topo phase */
              case PD_REQUEST:
              case PD_RESPONSE:
              case MD_REQUEST:
              case MD_RESPONSE:
              case SVF_PRESENCE_REQUEST:
              case SVF_PRESENCE_RESPONSE:
                TopoPhaseEnd();
                /* fall direct into state S_SLAVE_REGULAR */
                goto mac_S_SLAVE_REGULAR;
                break;
#endif
              }
            }
            else
            {
              /* a CRC error might appear here*/
            }
          }
          else
          {
            if (mac_event == UDF_TIMER1)
            {
              p_WTB_status->net_inhibit = FALSE;
              /*
              set SAM-Bit in p_WTB_status->node_report
              */
              pi_disable() ;
              p_WTB_status->node_status.node_report |= SAM_BIT ;
              pi_enable() ;
              p_WTB_status->node_strength = L_UNDEFINED;
              p_WTB_status->node_orient = L_UNKNOWN;
              Disable(MAC_DIR1) ;
              Disable(MAC_DIR2) ;
              /*
              BM_MAC_SlaveStop_Ind (MAC_SLAVE_STOP_ENDNODEFAIL)
              */
              mac_msg2bm.enr = BM_MAC_SlaveStop_Ind ;
              mac_msg2bm.p1 = (unsigned char) MAC_SLAVE_STOP_ENDNODEFAIL ;
              zas_send(WSNR_MAC, &mac_msg2bm) ;
#ifdef O_MVBC
//            lc_set_device_status_word(LC_DSW_DYD_MSK, LC_DSW_DYD_CLR);
#endif
              mac_state = S_IDLE ;
            }
            else
            {
              mac_general_events();
            }
          }
          break;
        }
        /*
        S_SLAVE_CLOSESWITCH
        */
        case S_SLAVE_CLOSESWITCH :
        {
          if (mac_event == UDF_TIMER2)
          {
            Enable (mac_main_direction, p_WTB_status->node_address) ;
            /*
            set INT-Bit in p_WTB_status->node_report
            */
            pi_disable() ;
            p_WTB_status->node_status.node_report |= INT_BIT ;
            pi_enable() ;
            d_tc_timer_reset(MAC_TIMER1, TMO_SLAVE_EXP_STATUS) ;
            mac_state = S_SLAVE_TOPO ;
          }
          else
          {
            mac_general_events();
          }
          break;
        }
        /*
        S_SLAVE_OPENSWITCH
        */
        case S_SLAVE_OPENSWITCH :
        {
          if (mac_event == UDF_TIMER2)
          {
            Enable (mac_main_direction, p_WTB_status->node_address) ;
            Enable (OPPOSITE_OF(mac_main_direction), ADR_UNNAMED) ;
            /*
            clear INT-Bit in p_WTB_status->node_report
            */
            pi_disable() ;
            p_WTB_status->node_status.node_report &= ~INT_BIT ;
            pi_enable() ;
            /* Reset Aux Channel */
            mac_YWM_cnt = MAX_YWM_CNT;
            mac_insist = TRUE;
            mac_aux_RS = 0;
            mac_fritting_on( OPPOSITE_OF( mac_main_direction)); /*4.0-7b*/
            d_tc_timer_reset(MAC_TIMER1, TMO_SLAVE_EXP_STATUS) ;
            mac_state = S_SLAVE_TOPO ;
          }
          else
          {
            mac_general_events();
          }
          break;
        }
        /*
        S_SLAVE_AWAIT_SC_INT_RES
        */
        case S_SLAVE_AWAIT_SC_INT_RES :
        {
          if (mac_event == SC_DIR12)
          {
            Disable(MAC_DIR1)  ;
            Disable(MAC_DIR2)  ;
            mac_fritting_off( MAC_DIR12);    /*4.1-34*/
            d_sw_interruption(D_LINE_A, D_CLOSE) ;
            d_sw_interruption(D_LINE_B, D_CLOSE) ;
            if (mac_main_direction == MAC_DIR1)
            {
              /* detach unused direction 2 */
              d_sw_line_attachment(
                 /* 1A */ D_CLOSE,
                 /* 1B */ D_CLOSE,
                 /* 2A */ D_OPEN,
                 /* 2B */ D_OPEN) ;
            }
            else
            {
              /* detach unused direction 1 */
              d_sw_line_attachment(
                 /* 1A */ D_OPEN,
                 /* 1B */ D_OPEN,
                 /* 2A */ D_CLOSE,
                 /* 2B */ D_CLOSE) ;
            }
            d_tc_timer_reset(MAC_TIMER2, TMO_SWITCH_SLAVE) ;
            mac_state = S_SLAVE_CLOSESWITCH ;
          }
          else
          {
            mac_general_events();
          }
          break;
        }
        /*
        S_SLAVE_AWAIT_SC_END_RES
        */
        case S_SLAVE_AWAIT_SC_END_RES :
        {
          if (mac_event == SC_DIR12)
          {
            Disable(mac_main_direction);
            mac_fritting_off( MAC_DIR12);  /*4.0-7b*/
            d_sw_interruption(D_LINE_A,D_OPEN) ;
            d_sw_interruption(D_LINE_B,D_OPEN) ;
            /* attach both directions */
            d_sw_line_attachment(
               /* 1A */ D_CLOSE,
               /* 1B */ D_CLOSE,
               /* 2A */ D_CLOSE,
               /* 2B */ D_CLOSE) ;
            d_tc_timer_reset(MAC_TIMER2, TMO_SWITCH_SLAVE) ;
            mac_state = S_SLAVE_OPENSWITCH ;
          }
          else
          {
            mac_general_events();
          }
          break;
        }
        /*
        S_SLAVE_TOPO_AWAIT_DETECT_RES
        */
        case S_SLAVE_TOPO_AWAIT_DETECT_RES :
        {
          if (((mac_event == UDF_DIR1) && (mac_main_direction == MAC_DIR2)) ||
              ((mac_event == UDF_DIR2) && (mac_main_direction == MAC_DIR1)))
          {
            SendDetectReqEpilog() ;
            d_get_cur_line_status (&tmp_status) ;
            SndFrm.data[POS_NR] = p_WTB_status->node_status.node_report | tmp_status ;
            SndFrm.data[POS_RS] = mac_aux_RS ;
            /* update RI-Bit in Status.Res */
            if (mac_aux_NS & INH_BIT)
            {
              SndFrm.llc |= RI_BIT ;
            }
            Send(mac_main_direction, &SndFrm, D_MODE_NOACK) ;
            mac_state = S_SLAVE_TOPO ;
          }
          else
          {
            mac_general_events();
          }
          break;
        }
        /* regular operation states */
        /*
        S_SLAVE_REGULAR
        */
        case S_SLAVE_REGULAR :
          mac_S_SLAVE_REGULAR : /* quick and dirty but it should work */
        {
          if (((mac_event == UDF_DIR1) && (mac_main_direction == MAC_DIR1))    ||
              ((mac_event == UDF_DIR2) && (mac_main_direction == MAC_DIR2)))
          {
            if (mac_sub_event == D_OK)
            {
              switch (mac_rcv_frame->llc & ACI_MASK)
              {
                /*
                PD_RESPONSE
                */
                case PD_RESPONSE:
                {
                  /* activate process data interface */
                  pd_receive_indicate((type_idu*) mac_rcv_frame) ;
                  break;
                }
                /*
                MD_RESPONSE
                */
                case MD_RESPONSE:
                {
                  /* activate message data interface */
                  md_receive_indicate(mac_rcv_frame) ;
                  p_statistic_data->NoRMD++;
                  break;
                }
                /*
                PD_REQUEST
                */
                case PD_REQUEST:
                {
                  p2_TmpFrm = (type_idu_drv*) pd_request_to_send (ADR_BROADCAST) ;
                  if (p2_TmpFrm == (type_idu_drv*)0)
                  {
                    if ( md_check_for_data() )
                    {
                      SndFrm.llc = PD_RESPONSE_A | mac_CI_bits ;
                    }
                    else
                    {
                      SndFrm.llc = PD_RESPONSE | mac_CI_bits ;
                    }
                    SndFrm.sd   = p_WTB_status->node_address;
                    SndFrm.dd   = ADR_BROADCAST ;
                    SndFrm.size = 0 ;
                    Send(mac_main_direction, &SndFrm, D_MODE_NOACK) ;
                  }
                  else
                  {
                    if ( md_check_for_data() )
                    {
                      p2_TmpFrm -> llc = PD_RESPONSE_A | mac_CI_bits ;
                    }
                    else
                    {
                      p2_TmpFrm -> llc = PD_RESPONSE | mac_CI_bits ;
                    }
                    p2_TmpFrm -> sd = p_WTB_status->node_address;
                    p2_TmpFrm -> dd = ADR_BROADCAST ;
                    SET_SC_FLAG ;
                    Send(mac_main_direction, p2_TmpFrm, D_MODE_NOACK) ;
                    mac_state = S_SLAVE_AWAIT_SC_PD_RES ;
                  }
                  /*4.0-5..*/
                  if (mac_rcv_frame->size != 0)
                  {
                    /* activate process data interface */
                    /* this is done after sending the response  to avoid timing problems */
                    pd_receive_indicate((type_idu*) mac_rcv_frame) ;
                  }
                  /*..4.0-5*/
                  break;
                }
                /*
                MD_REQUEST
                */
                case MD_REQUEST:
                {
                  p2_TmpFrm = md_request_to_send ( & mac_MD_addr ) ;
                  if (p2_TmpFrm == (type_idu_drv *)0)
                  {
                    if ( md_check_for_data() )
                    {
                      SndFrm.llc = MD_RESPONSE_A | mac_CI_bits ;
                    }
                    else
                    {
                      SndFrm.llc = MD_RESPONSE | mac_CI_bits ;
                    }
                    SndFrm.dd = ADR_MASTER ;
                    SndFrm.size = 0 ;
                    SndFrm.sd = p_WTB_status->node_address ;
                    Send(mac_main_direction, &SndFrm, D_MODE_NOACK) ;
                  }
                  else
                  {
                    if ( md_check_for_data() )
                    {
                      p2_TmpFrm -> llc = MD_RESPONSE_A | mac_CI_bits ;
                    }
                    else
                    {
                      p2_TmpFrm -> llc = MD_RESPONSE | mac_CI_bits ;
                    }
                    /* dd is set by MMI */
                    p2_TmpFrm -> sd = p_WTB_status->node_address ;
                    SET_SC_FLAG ;
                    Send(mac_main_direction, p2_TmpFrm, D_MODE_NOACK) ;
                    p_statistic_data->NoTMD++;
                    /*
                    loopback
                    */
                    if ( (mac_MD_addr == ADR_BROADCAST)         ||
                    (mac_MD_addr == p_WTB_status->node_address)   )
                    {
                      /* activate message data interface */
                      md_receive_indicate(p2_TmpFrm) ;
                    }
                    mac_state = S_SLAVE_AWAIT_SC_MD_RES ;
                  }
                  break;
                }
                /*
                SVF_TOPO_REQUEST
                */
                case SVF_TOPO_REQUEST:
#ifndef UMS
                {
                  mac_master_topo =  mac_tmp_SV.MT_L + (mac_tmp_SV.MT_H__YP << 8);
                  if (  ( mac_rcv_frame->data[POS_MT_L]             == mac_tmp_SV.MT_L)                /*4.
                  1-14*/
                  && ((mac_rcv_frame->data[POS_MT_H__YP] & 0x0F) == (mac_tmp_SV.MT_H__YP & 0x0F) )
                  )
                  {
                    /* 4.1-14 */
                    /* the Topo_Response frame got lost ... */
                    /* thus send it once more ... */
                  }
                  else
                  {
                    mac_master_topo = (mac_master_topo + 1) & 0x0FFF ;
                  }
                  if (  (  (mac_rcv_frame->data[POS_MT_L]             == (mac_master_topo & 0xFF) )
                  && (  (mac_rcv_frame->data[POS_MT_H__YP] & 0x0F) == (mac_master_topo >> 8  ) )  )
                  )
                  {
                    d_tc_timer_reset(MAC_TIMER1, TMO_END_NODE_CHECK /*4.1-25*/) ;
                    SndFrm.sd   = p_WTB_status->node_address;
                    SndFrm.llc  = SVF_TOPO_RESPONSE;
                    SndFrm.dd   = ADR_MASTER;
                    SndFrm.size = SVF_TOPO_RESPONSE_SIZE;
                    Send(mac_main_direction, &SndFrm, D_MODE_NOACK) ;
                    mac_presence_startup     = TRUE;
                    mac_presence_tmo[BA_DIR] = mac_max_try /*4.0-7a*/;
                    mac_presence_tmo[TA_DIR] = mac_max_try /*4.0-7a*/;
                    /*
                    find index mac_ix of own node key in topography
                    */
                    if (mac_topo.bottom_address == 1)
                    {
                      mac_ix = p_WTB_status->node_address - 1 ;
                    }
                    else
                    {
                      if (p_WTB_status->node_address <= mac_topo.top_address)
                      {
                        mac_ix = p_WTB_status->node_address + (HIGHEST_NODE_ADDRESS - mac_topo.bottom_address)  ;
                      }
                      else
                      {
                        mac_ix = p_WTB_status->node_address - mac_topo.bottom_address  ;
                      }
                    }
                    /* max_ix = 2 * mac_ix */
                    mac_ix <<= 1 ;
                    /* save new master topo */
                    mac_tmp_SV.MT_L     =  mac_rcv_frame -> data[POS_MT_L];
                    mac_tmp_SV.MT_H__YP = (mac_rcv_frame -> data[POS_MT_H__YP]) & 0x0F;
                    /* test if node key changed accordingly */
                    if ( (mac_rcv_frame->data[POS_NK_OFFSET + mac_ix ]    ==
                    mac_configuration.node_descriptor.node_key.node_type)
                    &&
                    (mac_rcv_frame->data[POS_NK_OFFSET + mac_ix + 1] ==
                    mac_configuration.node_descriptor.node_key.node_version) )
                    {
                      if (++mac_topo.topo_counter == (MAX_TOPO_COUNT + 1))
                      {
                        mac_topo.topo_counter = 1;
                      }
                      mac_topo.individual_period = 1 << (mac_rcv_frame -> data[POS_MT_H__YP] >> 4);
                      mac_my_str                 = mac_rcv_frame -> data[POS_LS];
                      mac_topo.is_strong         = ((mac_my_str & STRONG_BIT) == STRONG_BIT);
                      /*
                      Copy topography from frame
                      */
                      mac_ix = 0;
                      while ( mac_ix != mac_topo.number_of_nodes )
                      {
                        mac_topo.NK[mac_ix].node_type = mac_rcv_frame -> data[ POS_NK_OFFSET + (mac_ix << 1)     ];

                        mac_topo.NK[mac_ix].node_version = mac_rcv_frame -> data[ POS_NK_OFFSET + (mac_ix << 1) + 1
                        ];
                        mac_ix ++;
                      }
                      /*
                      clear DSC-Bit in p_WTB_status->node_report
                      */
                      pi_disable() ;
                      p_WTB_status->node_status.node_report &= ~DSC_BIT ;
                      pi_enable() ;
                      /* Write save data to mac_tmp_SV */
                      mac_tmp_SV.WTB_status     = *p_WTB_status;
                      mac_tmp_SV.topo           = mac_topo;
                      mac_tmp_SV.main_direction = mac_main_direction;
                      mac_tmp_SV.last_node_addr = mac_last_node_addr;
                      mac_tmp_SV.my_str         = mac_my_str;
                      /*
                      BM_MAC_Topo_Ind
                      */
                      mac_msg2bm.enr = BM_MAC_Topo_Ind ;
                      mac_msg2bm.p34 = &mac_tmp_SV ;
                      zas_send(WSNR_MAC, &mac_msg2bm) ;
                    }
                    else
                    {
                      /*
                      set C-Bit again
                      */
                      pi_disable() ;
                      mac_CI_bits |= C_BIT ;
                      pi_enable() ;
                    }
                    p_statistic_data->topo_cnt++;
                    /* same state */
                  }
                  else
                  {
                    /*
                    stop
                    */
                    p_WTB_status->net_inhibit = FALSE;
                    /*
                    set SAM-Bit in p_WTB_status->node_report
                    */
                    pi_disable() ;
                    p_WTB_status->node_status.node_report |= SAM_BIT ;
                    pi_enable() ;
                    p_WTB_status->node_strength = L_UNDEFINED;
                    p_WTB_status->node_orient   = L_UNKNOWN;
                    Disable(MAC_DIR1) ;
                    Disable(MAC_DIR2) ;
                    /*
                    BM_MAC_SlaveStop_Ind (MAC_SLAVE_STOP_ENDNODEFAIL)
                    */
                    mac_msg2bm.enr = BM_MAC_SlaveStop_Ind ;
                    mac_msg2bm.p1 = (unsigned char) MAC_SLAVE_STOP_ENDNODEFAIL ;
                    zas_send(WSNR_MAC, &mac_msg2bm) ;
                    mac_state = S_IDLE ;
                  }
                  break;
                }
#else
              case SVF_TOPO_RESPONSE:
                /* UIC topo distribution */
                /*
                 * clear DSC-Bit in p_WTB_status->node_report
                 */
                pi_disable();
                p_WTB_status->node_status.node_report &= ~DSC_BIT;
                pi_enable();

                li_t_IDTSReset();

/* <ATR:01> */
                li_t_SetFrame(mac_rcv_frame->sd, mac_rcv_frame->data,
                              mac_rcv_frame->size);

                mac_state = S_SLAVE_TOPO;
                uic_repeat = MAX_UIC_REPEAT;
                mac_topo.top_address = 0; /* invalidate top address */
                goto mac_S_SLAVE_TOPO;
#endif


                /*
                SVF_PRESENCE_REQUEST
                */
                case SVF_PRESENCE_REQUEST:
                {
                  /* is inauguration key correct ? */
                  if (
                  (mac_rcv_frame -> data[POS_MT_L]     == mac_tmp_SV.MT_L) &&
                  (mac_rcv_frame -> data[POS_MT_H__YP] == mac_tmp_SV.MT_H__YP)
                  )
                  {
                    if ( p_WTB_status->node_address == mac_topo.top_address )
                    {
                      mac_presence_tmo [TA_DIR] = mac_max_status_fail; /*4.0-7a*/
                      mac_exp_dir = BA_DIR ;
                    }
                    else
                    {
                      if (p_WTB_status->node_address == mac_topo.bottom_address)
                      {
                        mac_presence_tmo [BA_DIR] = mac_max_status_fail;  /*4.0-7a*/
                        mac_exp_dir = TA_DIR ;
                      }
                      else
                      {
                        /* same state */
                        break; /* !!! */
                      }
                    }
                    /*
                    update I-Bit in Presence.Res
                    */
                    if (mac_rcv_frame -> llc & I_BIT)
                    {
                      Pre_res_frm.llc = (SVF_PRESENCE_RESPONSE | I_BIT) ;
                      p_WTB_status->net_inhibit = TRUE ;
                    }
                    else
                    {
                      Pre_res_frm.llc = SVF_PRESENCE_RESPONSE ;
                      p_WTB_status->net_inhibit = FALSE ;
                    }
                    mac_presence_startup = FALSE ;
                    d_tc_timer_reset(MAC_TIMER1, TMO_SLAVE_EXP_PRESENCE) ;
                    /*
                    set mac_MR for subsequent Detect.Req
                    */
                    mac_MR = 0 ;
                    if (p_WTB_status->node_status.node_report & SAM_BIT)
                    {
                      if ((mac_rcv_frame -> data[POS_MR]) & DA_BIT)
                      {
                        mac_MR |= DA_BIT ;
                      }
                      if ((mac_rcv_frame -> data[POS_MR]) & DB_BIT)
                      {
                        mac_MR |= DB_BIT ;
                      }
                    }
                    else
                    {
                      if ((mac_rcv_frame -> data[POS_MR]) & DA_BIT)
                      {
                        mac_MR |= DB_BIT ;
                      }
                      if ((mac_rcv_frame -> data[POS_MR]) & DB_BIT)
                      {
                        mac_MR |= DA_BIT ;
                      }
                    }
                    /* if Detect.Req was really sent */
                    if (SendDetectReqProlog())
                    {
                      mac_state = S_SLAVE_REGULAR_AWAIT_DETECT_RES;
                    }
                    else
                    {
                      update_line_status_bits();
                      Pre_res_frm.data[POS_NR] = p_WTB_status->node_status.node_report;
                      Pre_res_frm.data[POS_RS] = mac_aux_RS ;
                      Send(mac_main_direction, &Pre_res_frm, D_MODE_NOACK) ;
                      /* same state */
                    }
                  }
                  else
                  {
                    /* same state */
                  }
                  p_statistic_data->basic_period_cnt++;
                  break;
                }
                /*
                SVF_PRESENCE_RESPONSE
                */
                case SVF_PRESENCE_RESPONSE:
                {
                  update_line_status_bits();
                  /*
                  handle fritting    4.0-7b
                  */
                  /*4.0-7b*/
                  if (p_WTB_status->node_status.node_report & INT_BIT)
                  {

                  }
                  else
                  {
                    /* this_node is an end-node */
                    mac_fritting_test_main( mac_line);  /*4.1-33*/
                  }
                  if (mac_rcv_frame->sd == mac_topo.top_address)
                  {
                    mac_presence_tmo [TA_DIR] = mac_max_status_fail;   /*4.0-7a*/
                    mac_exp_dir = BA_DIR ;
                  }
                  else
                  {
                    if (mac_rcv_frame->sd == mac_topo.bottom_address)
                    {
                      mac_presence_tmo [BA_DIR] = mac_max_status_fail;   /*4.0-7a*/
                      mac_exp_dir = TA_DIR ;
                    }
                    else
                    {
                      /* same state */
                      break; /* !!! */
                    }
                  }
                  mac_presence_startup = FALSE ;
                  p_WTB_status->net_inhibit = ((mac_rcv_frame->llc & I_BIT) == I_BIT) ;
                  d_tc_timer_reset(MAC_TIMER1, TMO_SLAVE_EXP_PRESENCE) ;
                  p_statistic_data->basic_period_cnt++;
                  /* same state */
                  break;
                }
                /*
                SVF_STATUS_REQUEST
                */
                case SVF_STATUS_REQUEST:
                {
                  SndFrm.llc = SVF_STATUS_RESPONSE;
                  SndFrm.dd = ADR_MASTER ;
                  SndFrm.sd = p_WTB_status->node_address;
                  SndFrm.size = SVF_STATUS_RESPONSE_SIZE;
                  SndFrm.data[POS_RS] = mac_aux_RS ;
                  SndFrm.data[POS_UR] = p_WTB_status->node_status.user_report;
                  SndFrm.data[POS_NP] = mac_configuration.node_descriptor.node_period;
                  SndFrm.data[POS_NF] = mac_configuration.node_descriptor.node_frame_size;
                  SndFrm.data[POS_NT] = mac_configuration.node_descriptor.node_key.node_type;
                  SndFrm.data[POS_NV] = mac_configuration.node_descriptor.node_key.node_version;
                  SndFrm.data[POS_RES_S] = RESERVED_FIELD_VALUE ;
                  /*
                  Clear C-Bit
                  */
                  pi_disable() ;
                  mac_CI_bits &= (~C_BIT) ;
                  pi_enable() ;
                  SndFrm.data[POS_NR] = (p_WTB_status->node_status.node_report &  LINE_STATUS_MASK) |
                  mac_last_line_status;
                  Send(mac_main_direction, &SndFrm, D_MODE_NOACK) ;
                  /* same state */
                  break;
                }
                /*
                SVF_UNNAME_REQUEST
                */
                case SVF_UNNAME_REQUEST:
                {
                  d_tc_timer_reset(MAC_TIMER1, 0) ;
                  mac_last_node_addr = ADR_UNNAMED;
                  p_WTB_status->net_inhibit = FALSE ;
                  /*
                  set SAM-Bit in p_WTB_status->node_report
                  */
                  pi_disable() ;
                  p_WTB_status->node_status.node_report |= SAM_BIT ;
                  pi_enable() ;
                  p_WTB_status->node_strength = L_UNDEFINED;
                  p_WTB_status->node_orient   = L_UNKNOWN;
                  Disable(MAC_DIR1) ;
                  Disable(MAC_DIR2) ;
                  mac_fritting_off( MAC_DIR12); /*4.1-34*/
                  d_tc_timer_reset(MAC_TIMER2,
                  (mac_configuration.fritting_disabled == TRUE)
                  ? TMO_UNNAME : TMO_UNNAME_FRITT) ;    /*4.1-34*/
                  mac_state = S_SLAVE_UNNAME ;
                  break;
                }
                default:
                {
                  /* a SVF_DETECT_REQUEST might appear here*/
                }
              }
              /* same state except when MD_REQUEST and PD_REQUEST */
            }
            else
            {
              /* a CRC error might appear here*/
            }
          }
          else
          {
            if (mac_event == UDF_TIMER1)
            {
              /*
              Presence time-out
              */
              if (mac_presence_startup || (mac_presence_tmo[mac_exp_dir] == 1) )
              {
                p_WTB_status->net_inhibit = FALSE;
                /*
                set SAM-Bit in p_WTB_status->node_report
                */
                pi_disable() ;
                p_WTB_status->node_status.node_report |= SAM_BIT ;
                pi_enable() ;
                p_WTB_status->node_strength = L_UNDEFINED;
                p_WTB_status->node_orient   = L_UNKNOWN;
                Disable(MAC_DIR1) ;
                Disable(MAC_DIR2) ;
                /* if sleep bit cleared */
                if (p_WTB_status->node_status.node_report & SLP_BIT)
                {
                  mac_fritting_off( MAC_DIR12); /*4.0-7b*/
                  /*4.0-10: optimization*/
                  if (p_WTB_status->node_status.node_report & INT_BIT)
                  {
                    d_sw_interruption(D_LINE_A, D_OPEN);
                    d_sw_interruption(D_LINE_B, D_OPEN);
                    /*4.0-10b: both directions are attached */
                    d_sw_line_attachment(
                    /* 1A */ D_CLOSE,
                    /* 1B */ D_CLOSE,
                    /* 2A */ D_CLOSE,
                    /* 2B */ D_CLOSE) ;
                  }
                  d_sleep() ;
                  while (TRUE)
                  {

                  }
                }
                else
                {
                  md_free_packets() ;
                  /*
                  BM_MAC_SlaveStop_Ind (MAC_SLAVE_STOP_ENDNODEFAIL)
                  */
                  mac_msg2bm.enr = BM_MAC_SlaveStop_Ind ;
                  mac_msg2bm.p1 = (unsigned char) MAC_SLAVE_STOP_ENDNODEFAIL ;
                  zas_send(WSNR_MAC, &mac_msg2bm) ;
                  mac_state = S_IDLE ;
                }
              }
              else
              {
                mac_presence_tmo[mac_exp_dir]-- ;
                if (mac_exp_dir == BA_DIR)
                {
                  mac_exp_dir = TA_DIR ;
                }
                else
                {
                  mac_exp_dir = BA_DIR ;
                }
                d_tc_timer_reset(MAC_TIMER1, TMO_SLAVE_EXP_PRESENCE) ;
                /* same state */
              }
            }
            else
            {
              mac_general_events();
            }
          }
          break;
        }
        /*
        S_SLAVE_REGULAR_AWAIT_DETECT_RES
        */
        case S_SLAVE_REGULAR_AWAIT_DETECT_RES :
        {
          if (((mac_event == UDF_DIR1) && (mac_main_direction == MAC_DIR2)) ||
              ((mac_event == UDF_DIR2) && (mac_main_direction == MAC_DIR1)))
          {
            SendDetectReqEpilog() ;
            update_line_status_bits();
            Pre_res_frm.data[POS_NR] = p_WTB_status->node_status.node_report;
            Pre_res_frm.data[POS_RS] = mac_aux_RS ;
            /*
            update RI-Bit in Presence.Res
            */
            if (mac_aux_NS & INH_BIT)
            {
              Pre_res_frm.llc |= (SVF_PRESENCE_RESPONSE | RI_BIT) ;
            }
            Send(mac_main_direction, &Pre_res_frm, D_MODE_NOACK) ;
            mac_state = S_SLAVE_REGULAR ;
          }
          else
          {
            mac_general_events();
          }
          break;
        }
        /*
        S_SLAVE_AWAIT_SC_MD_RES
        */
        case S_SLAVE_AWAIT_SC_MD_RES :
        {
          if (mac_event == SC_DIR12)
          {
            CLEAR_SC_FLAG ;
            md_send_confirm () ;
            mac_state = S_SLAVE_REGULAR ;
          }
          else
          {
            if (mac_event == UDF_TIMER1)
            {
              d_tc_timer_reset(MAC_TIMER1, 1) ;
              /* same state */
            }
            else
            {
              mac_general_events();
            }
          }
          break;
        }
        /*
        S_SLAVE_AWAIT_SC_PD_RES
        */
        case S_SLAVE_AWAIT_SC_PD_RES :
        {
          if (mac_event == SC_DIR12)
          {
            CLEAR_SC_FLAG ;
            pd_send_confirm (p2_TmpFrm) ;
            mac_state = S_SLAVE_REGULAR ;
          }
          else
          {
            if (mac_event == UDF_TIMER1)
            {
              d_tc_timer_reset(MAC_TIMER1, 1) ;
              /* same state */
            }
            else
            {
              mac_general_events();
            }
          }
          break;
        }
        /*  states when unnaming */
        /*
        S_SLAVE_UNNAME
        */
        case S_SLAVE_UNNAME :
        {
          if (mac_event == UDF_TIMER2)
          {
            /*
            BM_MAC_SlaveStop_Ind (MAC_SLAVE_STOP_UNNAMED)
            */
            mac_msg2bm.enr = BM_MAC_SlaveStop_Ind ;
            mac_msg2bm.p1 = (unsigned short) MAC_SLAVE_STOP_UNNAMED; ;
            zas_send(WSNR_MAC, &mac_msg2bm) ;
            mac_state = S_IDLE ;
          }
          else
          {
            mac_general_events();
          }
          break;
        }
        /*
        S_UNNAME_AWAIT_SC
        */
        case S_UNNAME_AWAIT_SC :
        {
          if (mac_event == SC_DIR12)
          {
            CLEAR_SC_FLAG ;
            mac_retry-- ;
            if (mac_retry == 0)
            {
              /*
              BM_MAC_Unname_Con
              */
              mac_msg2bm.enr = BM_MAC_Unname_Con ;
              zas_send(WSNR_BM, &mac_msg2bm) ;
              p_WTB_status->net_inhibit   = FALSE ;
              p_WTB_status->node_strength = L_UNDEFINED ;
              mac_state = S_IDLE ;
            }
            else
            {
              SET_SC_FLAG ;
             
              Send(mac_main_direction,&SndFrm, D_MODE_NOACK) ;
              if (mac_drv_err == D_ERR_NOT_ALLOWED)
             { 	
             	    mac_retry++;
            }
            
              /* same state */
            }
            
          }
          else
          {
            mac_general_events();
               mac_msg2bm.enr = BM_MAC_Unname_Con ;
              zas_send(WSNR_BM, &mac_msg2bm) ;
              p_WTB_status->net_inhibit   = FALSE ;
              p_WTB_status->node_strength = L_UNDEFINED ;
              mac_state = S_IDLE ;
              mac_trace[mac_trace_ndx-1] =0xAA ;//next state
   
      mac_trigger[7] = mac_drv_err ; /*<ATR:04>*/
      mac_trigger[6] = ch2_disabled; /*<ATR:04>*/
      mac_trigger[5] = mac_state ;
      mac_trigger[4] = mac_event ;
      mac_trigger[3] = zas_count ;//num retry
     mac_trigger[2] = ch1_disabled ;//num retry
     mac_trigger[1] =mac_count ;//num retry
     mac_trigger[0] =0xAA ;//next state

           d_tc_timer_reset(MAC_TIMER2, TMO_STOP);/*<ATR:05>*/

          }
          break;
        }
        /*
        S_AWAIT_STOP_DELAY
        */
        case S_AWAIT_STOP_DELAY:
        {
          if (mac_event == UDF_TIMER2)
          {
            switch (mac_stop_state)
            {
              case S_SLAVE_AWAIT_SC_PD_RES:
              case S_MASTER_AWAIT_SC_PD_RES:
              {
                if (p2_TmpFrm -> size == 0)
                {

                }
                else
                {
                  pd_send_confirm (p2_TmpFrm) ;
                }
                break;
              }
              case S_SLAVE_AWAIT_SC_MD_RES:
              case S_MASTER_AWAIT_SC_MD_RES:
              {
                if (p2_TmpFrm -> size == 0)
                {

                }
                else
                {
                  md_send_confirm () ;
                }
                break;
              }
            }
            md_free_packets() ;
            /*
            BM_MAC_Stop_Con
            */
            mac_msg2bm.enr = BM_MAC_Stop_Con;
            zas_send(WSNR_BM, &mac_msg2bm) ;
            mac_state = S_IDLE ;
          }
          else
          {
            mac_general_events();
          }
          break;
        }
        /*
        4.0-3:
        S_AWAIT_CLOSE_DELAY
        */
        /*4.0-3..*/
        case S_AWAIT_CLOSE_DELAY:
        {
          if (mac_event == UDF_TIMER2)
          {
            /*
            BM_MAC_CloseSwitch_Con
            */
            mac_msg2bm.enr = BM_MAC_CloseSwitch_Con;
            zas_send(WSNR_BM, &mac_msg2bm) ;
            mac_state = S_IDLE ;
          }
          else
          {
            mac_general_events();
          }
          break;

          /*..4.0-3*/
        }
        default:
        {
          mac_error() ;
        }
      }
      /*
      acknowledge event
      */
      switch (mac_event)
      {
        case UDF_DIR1:
        {
          d_1_receive_quit (mac_rcv_frame) ;
          break;
        }
        case UDF_DIR2:
        {
          d_2_receive_quit (mac_rcv_frame) ;
          break;
        }
        default:
        {
          if (mac_UDF_ZAS_Flag)
          {
            mac_UDF_ZAS_Flag = FALSE ;
            zas_continue();
          }
        }
      }
      pi_disable() ; /* lock critical area */

    }
    while (TRUE) ;
    zas_activate() ;
  }
  
 
}
/*
functions of statemachine
*/
/*
void Master_Basic_Cycle(void)
*/
void Master_Basic_Cycle(void)
{
  if ( ISGREATER(SYSTIME, mac_cycle_start_time) )
  {
    mac_time_account = SYSTIME - mac_cycle_start_time ;
  }
  mac_cycle_start_time += BASIC_CYCLE ;
  mac_ix = 0 ;
  mac_status_cnt = MAX_STATUS ;
  p_statistic_data->basic_period_cnt++;   /*4.1-48*/
  /* check if weak master has been promoted or
           if strong master has been demoted */
  if (
       ((mac_my_str >= 128) && (mac_master_rank ==   WEAK)) ||
       ((mac_my_str <  128) && (mac_master_rank == STRONG))
     )
  {
    if (mac_master_rank == WEAK)
    {
      /* set to weak */
      mac_my_str &= (~STRONG_BIT) ;
      p_WTB_status->node_strength = L_WEAK ;
    }
    else
    {
      /* set to weak */
      mac_my_str |= STRONG_BIT ;
      p_WTB_status->node_strength = L_STRONG ;
    }
    Master_Prepare_Topo() ;
  }
  else
  {
    Master_PD_Phase() ;
  }
}
/*
void Master_PD_Phase(void)
*/
void Master_PD_Phase(void)
{
  /*
  local variables
  */
  /* AND-Combination over mac_slp_array, calculated in SleepInhibit */
  Boolean ANDed_sleep ;
  /*p_statistic_data->basic_period_cnt++;   4.1-48*/
  while (1)
  {
    if (mac_ix == mac_topo.number_of_nodes)
    {
      /*
      SleepInhibit
      */
      /*
      evaluate mac_ANDed_sleep and mac_ORed_inhibit
      */
      mac_ORed_inhibit = FALSE ;
      ANDed_sleep   = TRUE ;
      mac_ix = 0 ;
      while (mac_ix != mac_topo.number_of_nodes)
      {
        if (mac_inh_array[mac_ix])
        /* logical OR over all inhibit bits */
        {
          mac_ORed_inhibit = TRUE ;
        }
        if (!mac_slp_array[mac_ix++])
        /* logical AND over all sleep bits */
        {
          ANDed_sleep = FALSE ;
        }
      }
      if (ANDed_sleep)
      {
        /*4.0-7b*/
        mac_fritting_off( MAC_DIR12);
        /*4.0-10: optimization*/
        if (p_WTB_status->node_status.node_report & INT_BIT)
        {
          /*4.0-10a*/
          d_sw_interruption(D_LINE_A, D_OPEN);
          d_sw_interruption(D_LINE_B, D_OPEN);
          /*4.0-10b: both directions are attached */
          d_sw_line_attachment(
             /* 1A */ D_CLOSE,
             /* 1B */ D_CLOSE,
             /* 2A */ D_CLOSE,
             /* 2B */ D_CLOSE) ;
        }
        d_sleep() ;
        while (TRUE)
        {

        }
      }
      else
      {
        /*
        CheckEndNodes
        */
        mac_cur_dir = OPPOSITE_OF(mac_cur_dir) ;
        p_WTB_status->net_inhibit = mac_ORed_inhibit ;
        if (mac_cur_dir == MAC_DIR2)
        {
          Pre_req_frm.dd = mac_topo.top_address ;
        }
        else
        {
          Pre_req_frm.dd = mac_topo.bottom_address ;
        }
        if (Pre_req_frm.dd == ADR_MASTER)
        {
          /* send Detect.Req */
          /* if Detect.Req was really sent */
          if (SendDetectReqProlog())
          {
            mac_state = S_MASTER_AWAIT_DET_RES ;
          }
          else
          {
            SendPresenceRes();
            mac_state = S_MASTER_AWAIT_SC_PRESENCE_RES ;
          }
        }
        else
        {
          /* send Presence.Req */
          Pre_req_frm.data[POS_MR] = mac_MR ;
          if (mac_ORed_inhibit)
          {
            Pre_req_frm.llc = (SVF_PRESENCE_REQUEST | I_BIT) ;
          }
          else
          {
            Pre_req_frm.llc = SVF_PRESENCE_REQUEST ;
          }
          Send(mac_main_direction,&Pre_req_frm, D_MODE_NOACK) ;
          d_tc_timer_reset(MAC_TIMER2, TMO_AWAITDETECT) ;
          mac_state = S_MASTER_AWAIT_PRESENCE_RES ;
        }
        break ; /*!!!*/
      }
    }
    else
    {
      if (mac_poll_list[mac_ix].IC == 0 )
      {
        /*
        PD polling
        */
        mac_poll_list[mac_ix].IC = mac_poll_list[mac_ix].IP - 1 ;
        PD_req_frm.dd = mac_poll_list[mac_ix].NA ;
        /*4.0-5...*/
        /* look for process data to be unicasted */
        p2_TmpFrm = (type_idu_drv*) pd_request_to_send (PD_req_frm.dd) ;
        if ( p2_TmpFrm == (type_idu_drv*)0 )
        {
          p2_TmpFrm = &PD_req_frm;
        }
        else
        {
          p2_TmpFrm->sd = ADR_MASTER;
          p2_TmpFrm->dd = PD_req_frm.dd ;
          p2_TmpFrm->llc= PD_REQUEST;
        }
        /*..4.0-5*/
        if (PD_req_frm.dd == ADR_MASTER)
        {
          /* Send own PD request and wait for send confirmation */
          SET_SC_FLAG;
          Send(mac_main_direction, p2_TmpFrm /* &PD_req_frm 4.0-5*/, D_MODE_NOACK) ;
          mac_state = S_MASTER_AWAIT_SC_PD_REQ ;
        }
        else
        {
          SET_SC_FLAG;                       /*4.0-6*/
          Send(mac_main_direction, p2_TmpFrm /* &PD_req_frm 4.0-5*/, D_MODE_ACK) ;
          mac_state = S_MASTER_AWAIT_PD_RES ;
        }
        break ; /*!!!*/
      }
      else
      {
        mac_poll_list[mac_ix++].IC-- ;
        /* Loop */
      }
    }
  }
}
/*
void Master_AP_Phase(void)
*/
void Master_AP_Phase(void)
{
  TIME time_diff ;
  Byte tmp_NR ;
  while (1)
  {
    if ( ISGREATER(mac_cycle_start_time - mac_time_account, SYSTIME) )
    {
      if ( IsEmptySPList() ||
           (mac_status_cnt == 0)  )
      {
        /*
        MDPhase
        */
        /* MDPhase */
        if ( IsEmptyMDList() )
        {
          /*
          set timer
          */
          mac_time_account = 0;
          time_diff = mac_cycle_start_time - SYSTIME ;
          if ( (time_diff > LONGEST_DELAY) || (time_diff == 0) )
          {
            mac_UDF_TIMER1_Flag = TRUE ;
          }
          else
          {
            d_tc_timer_reset(MAC_TIMER1, time_diff) ;
          }
          mac_state = S_MASTER_IDLE_PHASE ;
        }
        else
        {
          if ((MD_req_frm.dd = GetMDList()) == ADR_MASTER)
          {
            /* Send own MD request and wait for send confirmation */
            SET_SC_FLAG;
            Send(mac_main_direction, &MD_req_frm, D_MODE_NOACK) ;
            mac_state = S_MASTER_AWAIT_SC_MD_REQ ;
          }
          else
          {
            Send(mac_main_direction, &MD_req_frm, D_MODE_ACK) ;
            mac_state = S_MASTER_AWAIT_MD_RES ;
          }
        }
        break ; /*!!!*/
      }
      else
      {
        mac_status_cnt-- ;
        if ((SndFrm.dd = GetSPList()) == ADR_MASTER)
        {
          /*
          Master_Prepare_Topo
          */
          tmp_NR = p_WTB_status->node_status.node_report ;
          /*
          clear DSC-Bit in p_WTB_status->node_report anyway
          */
          pi_disable() ;
          p_WTB_status->node_status.node_report &= (~DSC_BIT) ;
          pi_enable() ;
          /* Update WTB_nodes */
          p_WTB_nodes->node_status_list[mac_NIx_array[ADR_MASTER]].node_report =
          (p_WTB_status->node_status.node_report & LINE_STATUS_MASK ) | mac_last_line_status ;
          p_WTB_nodes->node_status_list[mac_NIx_array[ADR_MASTER]].user_report =
          p_WTB_status->node_status.user_report ;
          scan_WTB_nodes();
          /* update sleep bit in mac_slp_array*/
          mac_slp_array[mac_NIx_array[ADR_MASTER]] = p_WTB_status->node_status.node_report & SLP_BIT ;
          if (tmp_NR & DSC_BIT)
          {
            /* store new node descriptor in mac_ND_array */
            mac_ND_array[ADR_MASTER] = p_WTB_status->node_descriptor ;
            Master_Prepare_Topo() ;
            break ; /*!!!*/
          }
          else
          {
            /* Loop */
          }
        }
        else
        {
          /*
          SPPhase
          */
          /* SPPhase */
          SndFrm.llc           = SVF_STATUS_REQUEST ;
          SndFrm.sd            = ADR_MASTER ;
          SndFrm.size          = SVF_STATUS_REQUEST_SIZE ;
          SndFrm.data[POS_LS]  = mac_my_str ;
          /*SndFrm.data[POS_RES] = RESERVED_FIELD_VALUE; 4.1-27*/
          Send(mac_main_direction, &SndFrm, D_MODE_ACK) ;
          mac_state = S_MASTER_AWAIT_STATUS_RES ;
          break ; /*!!!*/
        }
      }
    }
    else
    {
      mac_UDF_TIMER1_Flag = TRUE ;
      mac_state = S_MASTER_IDLE_PHASE ;
      break ; /*!!!*/
    }
  }
}
/*
void Master_Prepare_Topo(void)
*/
void Master_Prepare_Topo(void)
{
#ifndef UMS
  unsigned short li ;
#endif
  mac_ix = 0 ;
  mac_topo.bottom_address = mac_ct[MAC_DIR1].Node ;
  mac_topo.top_address    = mac_ct[MAC_DIR2].Node ;
  /*
  mac_topo.bottom_address
  */
  if (mac_topo.bottom_address == ADR_MASTER)
  {

  }
  else
  {
    mac_NIx = mac_topo.bottom_address ;
    do{
#ifndef UMS
      mac_topo.NK[mac_ix] = mac_ND_array[mac_NIx].node_key ;
#endif
      mac_NP_array[mac_ix] = mac_ND_array[mac_NIx].node_period ;
      mac_NA_array[mac_ix] = mac_NIx ;
      mac_ix++ ;
      mac_NIx++ ;
    }
    while (mac_NIx != 64);
  }
#ifndef UMS
  mac_topo.NK[mac_ix]        = mac_ND_array[ADR_MASTER].node_key ;
#endif
  mac_NA_array[mac_ix]       = ADR_MASTER ;
  mac_NP_array[mac_ix]       = mac_ND_array[ADR_MASTER].node_period ;
  mac_topo.individual_period = mac_ND_array[ADR_MASTER].node_period ;
  mac_ix++ ;
  /*
  mac_topo.top_address
  */
  if (mac_topo.top_address == ADR_MASTER)
  {

  }
  else
  {
    mac_NIx = 1 ;
    do{
      mac_NIx++ ;
#ifndef UMS
      mac_topo.NK[mac_ix] = mac_ND_array[mac_NIx].node_key ;
#endif
      mac_NA_array[mac_ix] = mac_NIx ;
      mac_NP_array[mac_ix] = mac_ND_array[mac_NIx].node_period ;
      mac_ix++ ;
    }
    while (mac_NIx != mac_topo.top_address);
  }
  /*
  BuildTopo1
  */
  /* BuildTopo1 */
  mac_master_topo = (mac_master_topo + 1) & 4095 ;
  /* Build WTB_nodes and own Topos fixed parts, if Topography is built after Naming */
  if (++mac_topo.topo_counter == (MAX_TOPO_COUNT + 1))
  {
    mac_topo.topo_counter = 1;
  }
  mac_topo.is_strong          = ((mac_my_str & STRONG_BIT) == STRONG_BIT);
  if (mac_first_topo)
  {
    mac_topo.node_address       = p_WTB_status->node_address;
    mac_topo.node_orient        = L_UNKNOWN ;
    mac_topo.number_of_nodes    = ((mac_my_str & (~STRONG_BIT)) >> 1) ;
    p_WTB_nodes->bottom_address = mac_topo.bottom_address;
    p_WTB_nodes->top_address    = mac_topo.top_address;
    /*
    initialize p_WTB_nodes->node_status_list with stati collected
    during inauguration (mac_NS_array).
    */
    mac_ix = 0 ;
    while (mac_ix != mac_topo.number_of_nodes)
    {
      p_WTB_nodes->node_status_list[mac_ix] = mac_NS_array[mac_NA_array[mac_ix]] ;
      mac_ix++ ;
    }
  }
  /*
  BuildPollList1
  */
  /* BuildPollList1 */
  if (BuildPollList())
  {
    mac_first_topo = FALSE ;
    Enable(mac_main_direction, ADR_TRANSPARENT) ;
#ifndef UMS
    if (mac_NA_array[0] == ADR_MASTER)
    {
      mac_ix = 1 ;
    }
    else
    {
      mac_ix = 0 ;
    }
#else
    mac_ix = 0;
    wtb_restricted = FALSE;
#endif
    SndFrm.llc            = SVF_TOPO_REQUEST ;
    SndFrm.sd             = ADR_MASTER ;
    SndFrm.data[POS_BA]   = mac_topo.bottom_address ;
    SndFrm.data[POS_LS]   = mac_my_str ;
    SndFrm.data[POS_MT_L] = mac_master_topo & 0xFF;
    mac_tmp_MT_H__YP = mac_master_topo >> 8 ;
#ifndef UMS
    /*
    Copy topography into frame
    */
    li = 0;
    li --;
    do{
      li ++;
      SndFrm.data[ POS_NK_OFFSET + (li << 1)     ] = mac_topo.NK[li].node_type;
      SndFrm.data[ POS_NK_OFFSET + (li << 1) + 1 ] = mac_topo.NK[li].node_version;
    }
    while (mac_NA_array[li] != mac_topo.top_address);
    SndFrm.size = POS_NK_OFFSET + ((li + 1) << 1) ;
#else
    SndFrm.size = POS_NK_OFFSET;
#endif

    mac_retry = mac_max_try /*4.0-7a*/;
#ifndef UMS
    MasterSendTopoReq() ;
#else
    li_t_IDTSReset();
    wtb_topo_node = mac_topo.bottom_address;
    if (mac_NA_array[mac_ix] == ADR_MASTER) {
        /* distribute master node topography */
        MasterSendTopoResp();
    }
    else {
        /* poll slave for his topography */
        MasterSendTopoReq();
    }
    uic_repeat = MAX_UIC_REPEAT;
#endif

    mac_state = S_MASTER_AWAIT_TOPO_RES ;
  }
  else
  {
    Disable(MAC_DIR1)  ;
    Disable(MAC_DIR2)  ;
    md_free_packets() ;
    /*
    BM_MAC_MasterStop_Ind(MAC_MASTER_STOP_POLLLISTOVF)
    */
    mac_msg2bm.enr = BM_MAC_MasterStop_Ind ;
    mac_msg2bm.p1  = (unsigned short) MAC_MASTER_STOP_POLLLISTOVF ;
    zas_send(WSNR_BM, &mac_msg2bm) ;
#ifdef O_MVBC
//  lc_set_device_status_word(LC_DSW_DYD_MSK, LC_DSW_DYD_CLR);
#endif
    mac_state = S_IDLE ;
  }
}

#ifdef UMS
void MasterSendTopoResp(void)
{
    Topo_res_frm.sd   = ADR_MASTER;
    Topo_res_frm.llc  = SVF_TOPO_RESPONSE;
    Topo_res_frm.dd   = ADR_BROADCAST;
    Topo_res_frm.size = SVF_TOPO_RESPONSE_SIZE;

    memcpy(Topo_res_frm.data, li_t_GetOwnInaugData(), 128);
    li_t_ChgInaugDataSD(Topo_res_frm.sd | (1 << 7));
    SET_SC_FLAG;
    Send(mac_main_direction, &Topo_res_frm, D_MODE_NOACK);
    li_t_Sync();
}
#endif

void TopoPhaseEnd(void)
{
    unsigned char tmp_status;

    /* Write save data to mac_tmp_SV */
    mac_tmp_SV.WTB_status = *p_WTB_status;
    mac_tmp_SV.topo = mac_topo;
    mac_tmp_SV.main_direction = mac_main_direction;
    mac_tmp_SV.last_node_addr = mac_last_node_addr;
    mac_tmp_SV.my_str = mac_my_str;

#ifdef UMS
    if (mac_topo.top_address == 0) {
        /* never seen a topo request, so we know nothing about the rest of the
         * composition
         */
        mac_topo.number_of_nodes = 0;
    }
    if (wtb_topo_node == mac_topo.top_address) {
        wtb_restricted = FALSE;
    }
#endif

    /*
     * BM_MAC_Topo_Ind
     */
    mac_msg2bm.enr = BM_MAC_Topo_Ind;
#ifdef UMS
    mac_msg2bm.p1 = wtb_restricted;
#endif
    mac_msg2bm.p34 = & mac_tmp_SV;
    zas_send(WSNR_MAC, &mac_msg2bm);

    mac_presence_startup = TRUE;
    mac_presence_tmo[BA_DIR] = mac_max_status_fail;
    mac_presence_tmo[TA_DIR] = mac_max_status_fail;

    /*
     * init line redundancy
     */
    mac_last_line_status = p_WTB_status->node_status.node_report;

    /* clear line status bits */
    d_get_int_line_status (& tmp_status);

    line_status_update_counter = (1 << MAX_NODE_PERIOD);

    d_tc_timer_reset(MAC_TIMER1, 6 * TMO_SLAVE_EXP_PRESENCE);

    p_statistic_data->basic_period_cnt = 0;
    p_statistic_data->topo_cnt++;

    mac_state = S_SLAVE_REGULAR;
#ifdef O_MVBC
//  lc_set_device_status_word(LC_DSW_DYD_MSK, LC_DSW_DYD_CLR);
#endif
}


/*
void Master_Process_Presence_Res(void)
*/
void Master_Process_Presence_Res(void)
{
  if (mac_tmp_RS == 0)
  {
    /* no further node found */
  }
  else
  {
    if (
         (mac_tmp_RS >= 128) &&
         (mac_my_str >= 128)
       )
    {
      /*
      strong master conflict: issue BM_MAC_MasterSMC_Ind
      */
      mac_msg2bm.enr = BM_MAC_MasterSMC_Ind ;
      zas_send(WSNR_BM, &mac_msg2bm) ;
    }
    else
    {
      /* no composition change if own or remote composition signals inhibit*/
      if (
          (mac_tmp_remote_inhibit) ||
          (p_WTB_status->net_inhibit)
         )
      {

      }
      else
      {
        if (
             (!(mac_tmp_RS & INSIST_BIT)) ||
             (  mac_tmp_RS == 1)
           )
        {
          /*
          yielding composition or unnamed node found:

          issue BM_MAC_MasterLengthening_Ind
          */
          mac_msg2bm.enr = BM_MAC_MasterLengthening_Ind ;
          zas_send(WSNR_BM, &mac_msg2bm) ;
        }
        else
        {
          /*
          stronger composition found: issue BM_MAC_MasterYield_Ind
          */
          mac_msg2bm.enr = BM_MAC_MasterYield_Ind ;
          zas_send(WSNR_BM, &mac_msg2bm) ;
        }
      }
    }
  }
  Master_AP_Phase() ;
}
/*
void Naming_Process_Status_Res(void)
*/
void Naming_Process_Status_Res(void)
{
  mac_ct[mac_cur_dir].FirstStatus = FALSE ;
  /* if no further node detected or remote composition signals inhibit */
  if ((mac_tmp_RS == 0) || (mac_tmp_remote_inhibit))
  {
    Naming_Check_End_Of_Naming() ;
  }
  else
  {
    if (mac_tmp_RS == 1)   /* unnamed node found */
    {
      /* Page2 */
      /* stop if already >= MAX_NODES nodes in composition */
      if ((mac_my_str & (~STRONG_BIT)) < (2*MAX_NODES))
      {
        /*
        try to set intermediate
        */
        mac_ct[mac_cur_dir].Cnt = mac_max_status_fail;   /*4.0-7a*/
        if ( mac_ct[OPPOSITE_OF(mac_cur_dir)].Job != Name )
        {
          /*
          set intermediate
          */
          if (mac_ct[mac_cur_dir].Node == ADR_MASTER)
          {
            Disable(MAC_DIR1)  ;
            Disable(MAC_DIR2)  ;
            mac_fritting_off(MAC_DIR12);    /*4.1-34: redundant*/
            d_sw_interruption(D_LINE_A, D_CLOSE) ;
            d_sw_interruption(D_LINE_B, D_CLOSE) ;
            /*
            detach unused direction
            */
            if (mac_main_direction == MAC_DIR1)
            {
              /* detach unused direction 2 */
              d_sw_line_attachment(
              /* 1A */ D_CLOSE,
              /* 1B */ D_CLOSE,
              /* 2A */ D_OPEN,
              /* 2B */ D_OPEN) ;
            }
            else
            {
              /* detach unused direction 1 */
              d_sw_line_attachment(
              /* 1A */ D_OPEN,
              /* 1B */ D_OPEN,
              /* 2A */ D_CLOSE,
              /* 2B */ D_CLOSE) ;
            }
            d_tc_timer_reset(MAC_TIMER1, TMO_SWITCH_SLAVE) ;
            mac_state = S_NAMING_WAIT_FOR_MASTER_CLOSING_SWITCH ;
          }
          else
          {
            mac_retry = mac_max_try /*4.0-7a*/;
            SndFrm.dd   = mac_ct[mac_cur_dir].Node ;
            SndFrm.sd   = ADR_MASTER ;
            SndFrm.llc  = SVF_SETINT_REQUEST ;
            SndFrm.size = SVF_SETINT_REQUEST_SIZE;
            Send(mac_main_direction, &SndFrm, D_MODE_ACK) ;
            mac_state = S_NAMING_AWAIT_INT_RES ;
          }
        }
        else
        {
          /* try setting slave to intermediate position later again */
          mac_ct[mac_cur_dir].Job = TrySetInt ;
          mac_ct[mac_cur_dir].AwakeTime = SYSTIME + TMO_SWITCH_MASTER ;
          Naming_Schedule() ;
        }
      }
      else
      {
        p_WTB_status->node_strength = L_UNDEFINED ;
        /*
        BM_MAC_Name_Con(MAC_RESULT_NAME_FAILURE)
        */
        mac_msg2bm.enr = BM_MAC_Name_Con ;
        mac_msg2bm.p1  = (unsigned short) MAC_RESULT_NAME_FAILURE ;
        zas_send(WSNR_BM, &mac_msg2bm) ;
        Disable(MAC_DIR1)  ;
        Disable(MAC_DIR2)  ;
        mac_state = S_IDLE ;
      }
    }
    else
    {
      /*
      */
      if (
           (mac_tmp_RS >= 128) &&
           (mac_my_str >= 128)
         )
      {
        Naming_Check_End_Of_Naming() ;
      }
      else
      {
        mac_ct[mac_cur_dir].Cnt = mac_max_status_fail;   /*4.0-7a*/
        if (mac_tmp_RS & INSIST_BIT)
        {
          /* I yield */
          p_WTB_status->node_strength = L_UNDEFINED ;
          /*
          BM_MAC_Name_Con(MAC_RESULT_NAME_YIELD)
          */
          mac_msg2bm.enr = BM_MAC_Name_Con ;
          mac_msg2bm.p1  = (unsigned short) MAC_RESULT_NAME_YIELD ;
          zas_send(WSNR_BM, &mac_msg2bm) ;
          mac_state = S_IDLE ;
        }
        else
        {
          /* I insist */
          Naming_Check_End_Of_Naming() ;
        }
      }
    }
  }
}
/*
void Naming_Schedule(void)
*/
void Naming_Schedule(void)
{
  TIME time_diff ;
  if ( !ISGREATER(mac_ct[OPPOSITE_OF(mac_cur_dir)].AwakeTime, SYSTIME) )
  {
    mac_cur_dir = OPPOSITE_OF(mac_cur_dir) ; /* Toggle channel */
    mac_UDF_TIMER1_Flag = TRUE ;
  }
  else
  {
    if ( !ISGREATER(mac_ct[mac_cur_dir].AwakeTime, SYSTIME) )
    {
      mac_UDF_TIMER1_Flag = TRUE ;
    }
    else
    {
      if ( ISGREATER(mac_ct[MAC_DIR1].AwakeTime,mac_ct[MAC_DIR2].AwakeTime) )
      {
        mac_cur_dir = MAC_DIR2 ;
      }
      else
      {
        mac_cur_dir = MAC_DIR1 ;
      }
      /*
      set timer
      */
      time_diff = mac_ct[mac_cur_dir].AwakeTime - SYSTIME ;
      if ( (time_diff > LONGEST_DELAY) || (time_diff == 0) )
      {
        mac_UDF_TIMER1_Flag = TRUE ;
      }
      else
      {
        d_tc_timer_reset(MAC_TIMER1, time_diff) ;
      }
    }
  }
  mac_state = S_NAMING_AWAIT_DELAY ;
}
/*
void Naming_Schedule_Resume(void)
*/
void Naming_Schedule_Resume(void)
{
  switch (mac_ct[mac_cur_dir].Job)
  {
    /*
    case Get_Status
    */
    case Get_Status:
    {
      if (mac_ct[mac_cur_dir].Node == ADR_MASTER)
      {
        /* if Detect.Req was really sent */
        if (SendDetectReqProlog())
        {
          mac_state = S_NAMING_AWAIT_DET_RES ;
        }
        else
        {
          mac_tmp_RS = mac_aux_RS ;
          Naming_Process_Status_Res() ;
        }
      }
      else
      {
        /*
        prepare line redundancy
        */
        mac_main_A_ok           = FALSE ;
        mac_main_B_ok           = FALSE ;
        mac_got_status_response = FALSE ;
        SendStatusReq() ;
        mac_state = S_NAMING_AWAIT_STATUS_RES ;
      }
      break;
    }
    /*
    case Name
    */
    case Name:
    {
      IncAddr() ;
      SendNameReq() ;
      mac_state = S_NAMING_AWAIT_NAME_RES ;
      break;
    }
    /*
    case TrySetInt
    */
    case TrySetInt:
    {
      if ( mac_ct[OPPOSITE_OF(mac_cur_dir)].Job != Name )
      {
        /*
        set intermediate
        */
        if (mac_ct[mac_cur_dir].Node == ADR_MASTER)
        {
          Disable(MAC_DIR1)  ;
          Disable(MAC_DIR2)  ;
          mac_fritting_off(MAC_DIR12);    /*4.1-34: redundant*/
          d_sw_interruption(D_LINE_A, D_CLOSE) ;
          d_sw_interruption(D_LINE_B, D_CLOSE) ;
          /*
          detach unused direction
          */
          if (mac_main_direction == MAC_DIR1)
          {
            /* detach unused direction 2 */
            d_sw_line_attachment(
               /* 1A */ D_CLOSE,
               /* 1B */ D_CLOSE,
               /* 2A */ D_OPEN,
               /* 2B */ D_OPEN) ;
          }
          else
          {
            /* detach unused direction 1 */
            d_sw_line_attachment(
               /* 1A */ D_OPEN,
               /* 1B */ D_OPEN,
               /* 2A */ D_CLOSE,
               /* 2B */ D_CLOSE) ;
          }
          d_tc_timer_reset(MAC_TIMER1, TMO_SWITCH_SLAVE) ;
          mac_state = S_NAMING_WAIT_FOR_MASTER_CLOSING_SWITCH ;
        }
        else
        {
          mac_retry = mac_max_try /*4.0-7a*/;
          SndFrm.dd   = mac_ct[mac_cur_dir].Node ;
          SndFrm.sd   = ADR_MASTER ;
          SndFrm.llc  = SVF_SETINT_REQUEST ;
          SndFrm.size = SVF_SETINT_REQUEST_SIZE;
          Send(mac_main_direction, &SndFrm, D_MODE_ACK) ;
          mac_state = S_NAMING_AWAIT_INT_RES ;
        }
      }
      else
      {
        /* try setting slave to intermediate position later again */
        mac_ct[mac_cur_dir].Job = TrySetInt ;
        mac_ct[mac_cur_dir].AwakeTime = SYSTIME + TMO_SWITCH_MASTER ;
        Naming_Schedule() ;
      }
      break;
    }
  }
}
/*
void Naming_Check_End_Of_Naming(void)
*/
void Naming_Check_End_Of_Naming(void)
{
  if (mac_ct[mac_cur_dir].Cnt == 0)
  /* no further node found, strong master conflict or yielding composition */
  {
    if ( (mac_ct[MAC_DIR1].Cnt == 0) &&
         (mac_ct[MAC_DIR2].Cnt == 0) )
    /* naming timeout expired: no further nodes found */
    {
      mac_NS_array[ADR_MASTER] = p_WTB_status->node_status ;
      p_statistic_data->inaug_cnt++;
      /*
      BM_MAC_Name_Con(MAC_RESULT_NAME_OK)
      */
      mac_msg2bm.enr = BM_MAC_Name_Con ;
      mac_msg2bm.p1  = (unsigned short) MAC_RESULT_NAME_OK ;
      zas_send(WSNR_BM, &mac_msg2bm) ;
      mac_state = S_IDLE ;
    }
    else
    {
      mac_ct[mac_cur_dir].Job = Get_Status ;
      mac_ct[mac_cur_dir].AwakeTime = SYSTIME + TMO_STATUS ;
      Naming_Schedule() ;
    }
  }
  else
  {
    mac_ct[mac_cur_dir].Cnt-- ;
    mac_ct[mac_cur_dir].Job = Get_Status ;
    mac_ct[mac_cur_dir].AwakeTime = SYSTIME + TMO_STATUS ;
    Naming_Schedule() ;
  }
}
/*
SDL procedures
*/
/*
List Operations
*/
/* MD (message data) and SP (supervisory data) list implementation.
   The lists are organized as FIFOs. */
/*
Variable Declarations
*/
/* Read and write indices for MD and SP list) */
unsigned char WriteMDList;
unsigned char ReadMDList;
unsigned char WriteSPList;
unsigned char ReadSPList;

/* Arrays forming MD and SP list */
unsigned char MDList [MD_LIST_SIZE];
unsigned char SPList [SP_LIST_SIZE];
/*
void AddMDList (unsigned char address)
*/
/* add one node to MD list*/
void AddMDList (unsigned char address)
{
  unsigned char i;
  unsigned char do_add;
  do_add = 1;
  i = ReadMDList;
  while (i != WriteMDList)
  {
    if (MDList[i] == address)
    {
      do_add = 0;
      break;
    }
    else
    {
      i++;
      i %= MD_LIST_SIZE;
    }
  }
  if (do_add)
  {
    if ((WriteMDList + 1) % MD_LIST_SIZE != ReadMDList)
    {
      MDList[WriteMDList++] = address;
      WriteMDList %= MD_LIST_SIZE;
    }
    else
    {
      /* else {DIAGNOSE }*/
    }
  }
}
/*
unsigned char GetMDList(void)
*/
/* get one node from MD list*/
unsigned char GetMDList(void)
{
  unsigned char Addr = MDList[ReadMDList];
  ReadMDList++;
  ReadMDList %= MD_LIST_SIZE;
  return (Addr);
}

/*
void ClearMDList(void)
*/
/* clear MD list*/
void ClearMDList(void)
{
  ReadMDList = 0;
  WriteMDList= 0;
}

/*
void AddSPList (unsigned char address)
*/
/* add one node to SP list*/
void AddSPList (unsigned char address)
{
  unsigned char i;
  unsigned char do_add;
  do_add = 1;
  i = ReadSPList;
  while (i != WriteSPList)
  {
    if (SPList[i] == address)
    {
      do_add = 0;
      break;
    }
    else
    {
      i++;
      i %= SP_LIST_SIZE;
    }
  }
  if (do_add)
  {
    if ((WriteSPList + 1) % SP_LIST_SIZE != ReadSPList)
    {
      SPList[WriteSPList++] = address;
      WriteSPList %= SP_LIST_SIZE;
    }
    else
    {
      /* else {DIAGNOSE }*/
    }
  }
}
/*
unsigned char GetSPList(void)
*/
unsigned char GetSPList(void)
{
  unsigned char Addr = SPList[ReadSPList];
  ReadSPList++;
  ReadSPList %= SP_LIST_SIZE;
  return (Addr);
}

/*
void ClearSPList(void)
*/
void ClearSPList(void)
{
  ReadSPList = 0;
  WriteSPList= 0;
}

/*
unsigned char IsEmptyMDList(void)
*/
/* check if MD list is empty*/
unsigned char IsEmptyMDList (void)
{
  return (ReadMDList == WriteMDList);
}
/*
unsigned char IsEmptySPList(void)
*/
/* check if SP list is empty*/
unsigned char IsEmptySPList (void)
{
  return (ReadSPList == WriteSPList);
}
/*
various send requests
*/
/*
Boolean SendDetectReqProlog(void)
*/
Boolean SendDetectReqProlog(void)
{
  Boolean sent;
  Det_req_frm.data[POS_MR] = mac_MR ;
  if (mac_main_direction == MAC_DIR1)
  {
    Det_req_frm.data[POS_MR] |= C12_BIT ;
  }
  if (p_WTB_status->net_inhibit)
  {
    Det_req_frm.data[POS_MR] |= INH_BIT ;
  }
  Det_req_frm.data[POS_LS] = mac_my_str | INSIST_BIT ;
  if ((mac_insist) || (--mac_YWM_cnt == 0))
  {
    /*4.1-32..
    if (mac_main_direction == MAC_DIR1)
       sent = !d_2_send_request(&Det_req_frm, D_MODE_ACK);
    else
       sent = !d_1_send_request(&Det_req_frm, D_MODE_ACK);
    ..4.1-32..*/
    sent = !d_send_request( OPPOSITE_OF(mac_main_direction),
            &Det_req_frm, D_MODE_ACK);
    /*..4.1-32*/
    if (mac_YWM_cnt == 0)
    {
      mac_insist = TRUE ;
      mac_YWM_cnt = MAX_YWM_CNT ;
      mac_aux_RS = 0 ;
    }
  }
  else
  {
    sent = FALSE;
  }
  return sent;
}
/*
void SendDetectReqEpilog(void)
*/
void SendDetectReqEpilog(void)
/* 4.0-7b:
 * the fritting source is
 * switched off: if a valid Detection_Response has been received
 * switched on:  if some invalid frame been received
 */
{
  /*
  local variables
  */
  unsigned char check_DF ;
  unsigned char is_DF       = FALSE ;
  /* temporary line status */
  unsigned char tmp_status ;
  if (mac_sub_event == D_OK)
  {
    if (mac_rcv_frame -> llc == SVF_DETECT_RESPONSE)
    {
      /*
      double fault recognition
      */
      mac_aux_NS = mac_rcv_frame -> data[POS_MR] ;
      d_get_cur_line_status (& tmp_status) ;
      /* now check if auxiliary channel is disturbed:
         if not no further check required.
         if yes indicate double fault if either
            own or remote opposite line is disturbed */
      if (mac_main_direction == MAC_DIR1)
      {
        check_DF = tmp_status & (DA2_BIT|DB2_BIT) ;
      }
      else
      {
        check_DF = tmp_status & (DA1_BIT|DB1_BIT) ;
      }
      if (check_DF)
      {
        if (mac_main_direction == MAC_DIR1)
        {
          /*
          aux = 2
          */
          /* own aux line A disturbed ? */
          if (tmp_status & DA2_BIT)
          {
            /* own main line B disturbed ? */
            if (mac_MR & DB_BIT)
            {
              is_DF = TRUE ;
            }
            else
            {
              /* remote main line B disturbed ? */
              if (mac_aux_NS & C12_BIT)
              {
                /* inverse orientation */
                is_DF = (mac_aux_NS & DA_BIT) ;
              }
              else
              {
                /* same orientation */
                is_DF = (mac_aux_NS & DB_BIT) ;
              }
            }
          }
          else
          {
            /* own aux line B disturbed ? */
            if (tmp_status & DB2_BIT)
            {
              /* own main line A disturbed ? */
              if (mac_MR & DA_BIT)
              {
                is_DF = TRUE ;
              }
              else
              {
                /* remote main line A disturbed ? */
                if (mac_aux_NS & C12_BIT)
                {
                  /* inverse orientation */
                  is_DF = (mac_aux_NS & DB_BIT) ;
                }
                else
                {
                  /* same orientation */
                  is_DF = (mac_aux_NS & DA_BIT) ;
                }
              }
            }
          }
        }
        else
        {
          /*
          aux = 1
          */
          /* own aux line A disturbed ? */
          if (tmp_status & DA1_BIT)
          {
            /* own main line B disturbed ? */
            if (mac_MR & DB_BIT)
            {
              is_DF = TRUE ;
            }
            else
            {
              if (mac_aux_NS & C12_BIT)
              {
                /* same orientation */
                is_DF = (mac_aux_NS & DB_BIT) ;
              }
              else
              {
                /* inverse orientation */
                is_DF = (mac_aux_NS & DA_BIT) ;
              }
            }
          }
          else
          {
            /* own aux line B disturbed ? */
            if (tmp_status & DB1_BIT)
            {
              /* own main line A disturbed ? */
              if (mac_MR & DA_BIT)
              {
                is_DF = TRUE ;
              }
              else
              {
                if (mac_aux_NS & C12_BIT)
                {
                  /* same orientation */
                  is_DF = (mac_aux_NS & DA_BIT) ;
                }
                else
                {
                  /* inverse orientation */
                  is_DF = (mac_aux_NS & DB_BIT) ;
                }
              }
            }
          }
        }
      }

      if (is_DF)
      {
        mac_DF_flag = TRUE ;
        mac_DF_count = MIN_GOOD_FRMS;
        /* hide node */
        mac_aux_RS = 0 ;
      }
      else
      {
        if (    mac_DF_flag          &&
             (--mac_DF_count != 0)
           )
        {

        }
        else
        {
          mac_DF_flag = FALSE ;
          mac_aux_RS = mac_rcv_frame -> data[POS_RS] ;
          /* insist if partner signals yield or if strong master conflict */

          if (
               (!(mac_aux_RS & INSIST_BIT))                                          ||
               ((mac_my_str >= 128) &&
                (mac_aux_RS >= 128))
             )
          {
            /* I insist */
          }
          else
          {
            /* I yield */
            mac_insist = FALSE ;
          }
          /* "1" in POS_RS means unnamed node found and "0" means no node found */
          if (mac_aux_RS == 0)
          {
            mac_aux_RS = 1 ;
          }
        }
      }
    }
  }
  else
  {
    mac_aux_RS = 0 ;
    mac_aux_NS = 0;
  }
}
/*
void SendDetectReq12(void)
*/
void SendDetectReq12(void)
{
  frm1.dd  = ADR_UNNAMED ;
  frm1.sd  = ADR_UNNAMED ;
  frm1.llc = SVF_DETECT_REQUEST ;
  frm1.size = SVF_DETECT_REQUEST_SIZE ;
  frm1.data[POS_LS] = mac_my_str | INSIST_BIT ;
  /* DA_,DB_,INH_BITs are cleared */
  frm1.data[POS_MR] = 0 ;
  Send (MAC_DIR1, &frm1, D_MODE_ACK) ;
  frm2.dd  = ADR_UNNAMED ;
  frm2.sd  = ADR_UNNAMED ;
  frm2.llc = SVF_DETECT_REQUEST ;
  frm2.size = SVF_DETECT_REQUEST_SIZE ;
  frm2.data[POS_LS] = mac_my_str | INSIST_BIT ;
  /* DA_,DB_,INH_BITs are cleared */
  frm2.data[POS_MR] = C12_BIT ;
  Send (MAC_DIR2, &frm2, D_MODE_NOACK) ;
}
/*
void MasterSendTopoReq(void)
*/
void MasterSendTopoReq(void)
{
  /* only the fields [DD, MT_H__YP] are changed for each node */
  SndFrm.dd  = mac_NA_array[mac_ix] ;
#ifdef UMS
  SndFrm.llc  = SVF_TOPO_REQUEST;
  SndFrm.size = POS_NK_OFFSET;
#endif

  SndFrm.data[POS_MT_H__YP] = mac_tmp_MT_H__YP | (mac_NP_array[mac_ix]<<4) ;
  Send(mac_main_direction, &SndFrm, D_MODE_ACK) ;
}
/*
void SendNameReq(void)
*/
void SendNameReq(void)
{
  SndFrm.dd           = ADR_UNNAMED ;
  SndFrm.sd           = ADR_MASTER ;
  SndFrm.llc          = SVF_NAME_REQUEST ;
  SndFrm.size         = SVF_NAME_REQUEST_SIZE ;
  SndFrm.data[POS_LS] = mac_my_str + 2 ;
  SndFrm.data[POS_YA] = mac_ct[mac_cur_dir].Node |
                        mac_ct[mac_cur_dir].Descend ;
  mac_retry = mac_max_try /*4.0-7a*/;
  Send( mac_main_direction, &SndFrm, D_MODE_ACK ) ;
}
/*
void SendStatusReq(void)
*/
void SendStatusReq(void)
{
  SndFrm.dd  = mac_ct[mac_cur_dir].Node ;
  SndFrm.sd  = ADR_MASTER ;
  SndFrm.llc = SVF_STATUS_REQUEST ;
  SndFrm.size = SVF_STATUS_REQUEST_SIZE ;
  SndFrm.data[POS_LS] = mac_my_str ;
  SndFrm.data[POS_MR] = mac_MR ;
  mac_retry = mac_max_try /*4.0-7a*/;
  d_tc_timer_reset(MAC_TIMER2, TMO_AWAITDETECT) ;
  Send( mac_main_direction,  &SndFrm, D_MODE_NOACK ) ;
}
/*
void SendPresenceRes(void)
*/
void SendPresenceRes(void)
{
  Pre_res_frm.data[POS_RS] = 0;
  Pre_res_frm.data[POS_NR] = p_WTB_status->node_status.node_report ;
  if (mac_ORed_inhibit)
  {
    Pre_res_frm.llc |= I_BIT ;
  }
  SET_SC_FLAG ;
  Send(mac_main_direction, &Pre_res_frm, D_MODE_NOACK) ;
}
/*
void InclusionSuccess(void)
*/
void InclusionSuccess(void)
{
  unsigned char dummy_status ;
  Disable(MAC_DIR1)  ;
  d_tc_timer_reset(MAC_TIMER1, 0) ;
  if (mac_main_direction == MAC_DIR1)
  {
    d_sw_direction(D_DIRECT_1) ;
    /* detach unused direction 2 */
    d_sw_line_attachment(
       /* 1A */ D_CLOSE,
       /* 1B */ D_CLOSE,
       /* 2A */ D_OPEN,
       /* 2B */ D_OPEN) ;
  }
  else
  {
    d_sw_direction(D_DIRECT_2) ;
    /* detach unused direction 1 */
    d_sw_line_attachment(
       /* 1A */ D_OPEN,
       /* 1B */ D_OPEN,
       /* 2A */ D_CLOSE,
       /* 2B */ D_CLOSE) ;
  }
  d_1_install(mac_driver1_sc, mac_rcv1_evt/*4.1-31*/, BITRATE, RESPONSE_TIME) ;
  d_2_install(mac_driver2_sc, mac_rcv2_evt/*4.1-31*/, BITRATE, RESPONSE_TIME) ;
  Enable(mac_main_direction, p_WTB_status->node_address) ;
  mac_insist = TRUE ;
  mac_CI_bits = 0 ;
  mac_aux_RS = 0 ;
  mac_presence_startup = TRUE ;
  /*
  init line redundancy
  */
  mac_last_line_status = p_WTB_status->node_status.node_report;
  /* clear line status bits */
  d_get_int_line_status (& dummy_status) ;
  line_status_update_counter = (1 << MAX_NODE_PERIOD);
  d_tc_timer_reset(MAC_TIMER1, TMO_END_NODE_CHECK /*4.1-25*/) ;
}
/*
void InclusionFail(void)
*/
void InclusionFail(void)
{
  if (mac_tmp_LLC & I_BIT)
  {
    /*
    BM_MAC_Inclusion_Con(MAC_RESULT_INC_INHIBIT)
    */
    mac_msg2bm.enr = BM_MAC_Inclusion_Con ;
    mac_msg2bm.p1  = (unsigned short) MAC_RESULT_INC_INHIBIT ;
    zas_send(WSNR_BM, &mac_msg2bm) ;
  }
  else
  {
    /*
    BM_MAC_Inclusion_Con(MAC_RESULT_INC_DISRUPT)
    */
    mac_msg2bm.enr = BM_MAC_Inclusion_Con ;
    mac_msg2bm.p1  = (unsigned short) MAC_RESULT_INC_DISRUPT ;
    zas_send(WSNR_BM, &mac_msg2bm) ;
  }
  Disable(MAC_DIR1)  ;
}
/*
void MasterPrepare(void)
*/
void MasterPrepare(void)
{
  mac_topo.individual_period = 1 << mac_NP_array[mac_NIx_array[ADR_MASTER]] ;
  p_WTB_nodes->number_of_nodes = mac_topo.number_of_nodes ;
  /* only topography and master address is filled in mac_tmp_SV*/
  mac_tmp_SV.topo = mac_topo;
  mac_tmp_SV.WTB_status.node_address = ADR_MASTER ;
  /*
  BM_MAC_Topo_Ind(mac_tmp_SV)
  */
  mac_msg2bm.enr = BM_MAC_Topo_Ind ;
#ifdef UMS
  mac_msg2bm.p1 = wtb_restricted;
#endif
  mac_msg2bm.p34  = &mac_tmp_SV ;
  zas_send(WSNR_BM, &mac_msg2bm) ;
  /* enable aux channel if I am end node */
  if (p_WTB_status->node_status.node_report & I_BIT)
  {

  }
  else
  {
    Enable(OPPOSITE_OF(mac_main_direction), ADR_UNNAMED) ;
  }
  /* preset Presence request frame */
  Pre_req_frm.sd = ADR_MASTER ;
  Pre_req_frm.llc = SVF_PRESENCE_REQUEST ;
  Pre_req_frm.size = SVF_PRESENCE_REQUEST_SIZE ;
  Pre_req_frm.data[POS_LS] = mac_my_str ;
  Pre_req_frm.data[POS_MT_L] = mac_master_topo & 0xFF ;
  Pre_req_frm.data[POS_MT_H__YP] = mac_master_topo >> 8 ;
  scan_WTB_nodes() ;
}
/*
void IncAddr(void)
*/
void IncAddr(void)
{
  if (mac_cur_dir == MAC_DIR1)
  {
    if (mac_ct[mac_cur_dir].Node == ADR_MASTER)
    {
      mac_ct[mac_cur_dir].Node = HIGHEST_NODE_ADDRESS ;
    }
    else
    {
      mac_ct[mac_cur_dir].Node-- ;
    }
  }
  else
  {
    /* Update Node Status */
    mac_ct[mac_cur_dir].Node++ ;
  }
  mac_ct[mac_cur_dir].FirstStatus = TRUE ;
}
/*
void AuxResponse(void)
*/
void AuxResponse(void)
{
  /*
  local variables
  */
  /* temporary line status */
  unsigned char
  tmp_status ;
  /* check for double fault */
  Boolean check_DF ;
  /* is double fault */
  Boolean is_DF       = FALSE ;
  unsigned char tmp_rem_str ;
  if (mac_rcv_frame -> llc == SVF_DETECT_REQUEST)
  {
    aux_frm.dd   = ADR_BROADCAST ;
    aux_frm.sd   = ADR_UNNAMED ;
    aux_frm.llc  = SVF_DETECT_RESPONSE ;
    aux_frm.size = SVF_DETECT_RESPONSE_SIZE ;
    aux_frm.data[POS_MR] = mac_MR ;
    if (mac_main_direction == MAC_DIR1)
    {
      aux_frm.data[POS_MR] |= C12_BIT ;
    }
    if (p_WTB_status->net_inhibit)
    {
      aux_frm.data[POS_MR] |= INH_BIT ;
    }
    tmp_rem_str = mac_rcv_frame -> data[POS_RS] ;
    mac_YWM_cnt = MAX_YWM_CNT ;
    if (  (mac_my_str  >= 128) &&
          (tmp_rem_str >= 128)    )
    {
      aux_frm.data[POS_LS] = mac_my_str | INSIST_BIT; /*4.0-1*/
    }
    else
    {
      if (tmp_rem_str > mac_my_str)
      {
         /* I yield */
        aux_frm.data[POS_LS] = mac_my_str ;
        mac_insist = FALSE ;
      }
      else
      {
         /* I insist */
        aux_frm.data[POS_LS] = mac_my_str | INSIST_BIT ;
        tmp_rem_str &= (~INSIST_BIT) ; /* Signal that remote node yields */
      }
      /*
      4.0-1: Detection_Response is now send in any case
      */
    }
    /*4.0-1...*/
    /* send via aux direction and don't care if already sending */
    /*4.1-32..
    if (mac_main_direction == MAC_DIR1)
       d_2_send_request(&Det_req_frm, D_MODE_ACK);
    else
       d_1_send_request(&Det_req_frm, D_MODE_ACK);
    ..4.1-32..*/
    d_send_request( OPPOSITE_OF(mac_main_direction), &aux_frm, D_MODE_NOACK);
    /*..4.1-32*/
    /*...4.0-1*/
    /*
    double fault recognition
    ========================
    Input:  remote node status (DetectReq.data[POS_MR])
            own main line stataus (mac_MR)
            current auxiliary line status (via d_get_cur_line_status)
    Output: is_DF
    */
    mac_aux_NS = mac_rcv_frame -> data[POS_MR] ;
    d_get_cur_line_status (& tmp_status) ;
    /* now check if auxiliary channel is disturbed:
       if not, no further check required.
       if yes, indicate double fault if either
          own or remote opposite line is disturbed */
    if (mac_main_direction == MAC_DIR1)
    {
      check_DF = tmp_status & (DA2_BIT|DB2_BIT) ;
    }
    else
    {
      check_DF = tmp_status & (DA1_BIT|DB1_BIT) ;
    }
    if (check_DF)
    {
      if (mac_main_direction == MAC_DIR1)
      {
        /*
        aux = 2
        */
        /* own aux line A disturbed ? */
        if (tmp_status & DA2_BIT)
        {
          /* own main line B disturbed ? */
          if (mac_MR & DB_BIT)
          {
            is_DF = TRUE ;
          }
          else
          {
            /* remote main line B disturbed ? */
            if (mac_aux_NS & C12_BIT)
            {
              /* inverse orientation */
              is_DF = (mac_aux_NS & DA_BIT) ;
            }
            else
            {
              /* same orientation */
              is_DF = (mac_aux_NS & DB_BIT) ;
            }
          }
        }
        else
        {
          /* own aux line B disturbed ? */
          if (tmp_status & DB2_BIT)
          {
            /* own main line A disturbed ? */
            if (mac_MR & DA_BIT)
            {
              is_DF = TRUE ;
            }
            else
            {
              /* remote main line A disturbed ? */
              if (mac_aux_NS & C12_BIT)
              {
                /* inverse orientation */
                is_DF = (mac_aux_NS & DB_BIT) ;
              }
              else
              {
                /* same orientation */
                is_DF = (mac_aux_NS & DA_BIT) ;
              }
            }
          }
        }
      }
      else
      {
        /*
        aux = 1
        */
        /* own aux line A disturbed ? */
        if (tmp_status & DA1_BIT)
        {
          /* own main line B disturbed ? */
          if (mac_MR & DB_BIT)
          {
            is_DF = TRUE ;
          }
          else
          {
            if (mac_aux_NS & C12_BIT)
            {
              /* same orientation */
              is_DF = (mac_aux_NS & DB_BIT) ;
            }
            else
            {
              /* inverse orientation */
              is_DF = (mac_aux_NS & DA_BIT) ;
            }
          }
        }
        else
        {
          /* own aux line B disturbed ? */
          if (tmp_status & DB1_BIT)
          {
            /* own main line A disturbed ? */
            if (mac_MR & DA_BIT)
            {
              is_DF = TRUE ;
            }
            else
            {
              if (mac_aux_NS & C12_BIT)
              {
                /* same orientation */
                is_DF = (mac_aux_NS & DA_BIT) ;
              }
              else
              {
                /* inverse orientation */
                is_DF = (mac_aux_NS & DB_BIT) ;
              }
            }
          }
        }
      }
    }

    /* "hide" detection result if double fault */
    if (is_DF)
    {
      mac_DF_flag = TRUE ;
      mac_DF_count = MIN_GOOD_FRMS;
      mac_aux_RS = 0 ;
    }
    else
    {
      if (
              mac_DF_flag          &&
           (--mac_DF_count != 0)
         )
      {

      }
      else
      {
        mac_DF_flag = FALSE ;
        mac_aux_RS = tmp_rem_str ;
      }
    }
  }
}
/*
void Enable(Type_Dir TheChannel, Byte TheAddress)
*/
void Enable(Type_Dir TheChannel, Byte TheAddress)
{
  unsigned char result;
  if (TheChannel == MAC_DIR1)
  {
    result = d_1_rx_enable(TheAddress) ;
        ch1_disabled = 0;/*<ATR:04>*/
  }
  else
  {
    result = d_2_rx_enable(TheAddress) ;
        ch2_disabled = 0;/*<ATR:04>*/
  }
  if (result != D_OK)
  {
    diagwtb_entry(
                DIAG_MAC_ERR_ENABLE,
                (unsigned short) mac_state,
                (unsigned short) mac_event,
                result | (TheChannel << 8),
                (void*)0
               ) ;
    mac_shutdown(DIAG_MAC_ERR_ENABLE,
                 result | (TheChannel << 8));
  }
}
/*
void Disable(Type_Dir TheChannel)
*/
void Disable(Type_Dir TheChannel)
{
  unsigned char result;
  if (TheChannel == MAC_DIR1)
  {
    result = d_1_rx_disable()  ;
    ch1_disabled = 1;/*<ATR:04>*/
  }
  else
  {
    result = d_2_rx_disable()  ;
    ch2_disabled = 1;/*<ATR:04>*/
  }
  if (result != D_OK)
  {
    diagwtb_entry(
                DIAG_MAC_ERR_DISABLE,
                (unsigned short) mac_state,
                (unsigned short) mac_event,
                result | (TheChannel << 8),
                (void*)0
               ) ;
    mac_shutdown(DIAG_MAC_ERR_DISABLE,
                 result | (TheChannel << 8));
  }
}
/*
Boolean BuildPollList(void)
*/
/*
test
*/
#ifdef POLLLISTSIMULATION
#include <stdio.h>
#include <conio.h>
#endif
unsigned char BuildPollList(void)
{
  /* BuildPollList */
  /*
  test
  */
#ifdef POLLLISTSIMULATION
  /*
  type and variable definitions
  */
  /* System level definitions */
#define TRUE (1==1)
#define FALSE !TRUE
#define MAX_POLL_TRY         3
#define BIT_STUFF_FACTOR   120
#define BASIC_CYCLE         25
#define MAX_AVG_PD_USAGE   60
#define TELEGRAM_OVERHEAD  522 /* Bit times */
#define BITRATE            1000000
#define MAX_NODE_PERIOD    7
  typedef struct
  {
    unsigned char NA;
    unsigned char IP;
    unsigned char IC;
    unsigned char EC;
  } Type_PollEntry;

  typedef  Type_PollEntry Type_PollList[64];

  typedef struct
  {
    unsigned char NF;
    unsigned char NP;
  } Type_NodeDescriptor;

  typedef  Type_NodeDescriptor Type_NodeDescriptorArray[64];

  typedef unsigned char Type_ByteArray[64];

  typedef struct
  {
    unsigned char bottom_address;
    unsigned char top_address;
  } Type_Topology;
  Type_ByteArray mac_NA_array, mac_NP_array, mac_inh_array, mac_slp_array, mac_NIx_array;
  Type_Topology mac_topo;
  Type_PollList mac_poll_list;
  Type_NodeDescriptorArray mac_ND_array;
  unsigned char mac_first_topo = 1;
  unsigned char ix;
#endif
  /*
  local variables
  */
  unsigned char success;
  unsigned char node_ix ;   /* Node index */
  unsigned char poll_ix;    /* Poll list index */
  unsigned char macro_cycle_ix ;
  unsigned short period;    /* Period in No of BASIC_CYCLEs*/
  /* Period Code: 0 = 1*BASIC_CYCLE
                  1 = 2*BASIC_CYCLE
                  2 = 4*BASIC_CYCLE
                  3 = 8*BASIC_CYCLE ...*/
  unsigned char period_code;
  /* macro cycle in multiple of basic cycle time.
     Equal longest period found when building poll list*/
  unsigned char macro_cycle;
  /*4.1-26*/
  t_boolean  mac_poll_pd[MAX_NODES];    /* process data in poll frames */
  UNSIGNED16 cnt_poll_pd;               /* number of ports with process data in poll frames */
  /* current phase */
  unsigned char phase;
  /* start phase: incremented with each poll list entry to get a staggered    macro cycle usage */
  unsigned char start_phase;
  /* total number of nodes found when building poll list */
  unsigned char number_of_nodes;
  /* time used in current basic cycle */
  unsigned short basic_cycle_time;
  /* Accumulated time used in all basic cycles; is used to calculate the average basic cycle usage */
  unsigned short macro_cycle_time_div256;
  /* variables for optimisation */

  /* period code which is currently being considered for
     optimisation */
  unsigned short opt_period ;
  /* TRUE if at least one node's period has been changed
     during loop throu all period codes.
     Gives up optimisation and returns BM_MAC_MASTER_STOP_IND if
     any_change is FALSE after looping throu all period codes.  */
  unsigned char any_change ;
  /* TRUE if at least one node's period has been changed
     during loop throu one particular period code.
  */
  unsigned char some_node ;
  /*
  local constants
  */
  /* Maximal available time for periodic data in macro cycle */
  unsigned short allowed_macro_time_div256 = (((1000*BASIC_CYCLE) / 100 * MAX_AVG_PD_USAGE)>>8);
  unsigned short Telegram_Ovhd_us = ((unsigned short)
         (1000000/BITRATE) * TELEGRAM_OVERHEAD);
  unsigned short us_per_byte_mul10 = ((unsigned short)
         (1000000/BITRATE) * 8 * BIT_STUFF_FACTOR / 10);
  /*
  test
  */
#ifdef POLLLISTSIMULATION
  mac_ND_array[60].node_frame_size = 0x80;  mac_ND_array[60].node_period = 0x00;
  mac_ND_array[61].node_frame_size = 0x80;  mac_ND_array[61].node_period = 0x06;
  mac_ND_array[62].node_frame_size = 0x80;  mac_ND_array[62].node_period = 0x00;
  mac_ND_array[63].node_frame_size = 0x80;  mac_ND_array[63].node_period = 0x00;
  mac_ND_array[ 1].node_frame_size = 0x80;  mac_ND_array[ 1].node_period = 0x00;
  mac_ND_array[ 2].node_frame_size = 0x80;  mac_ND_array[ 2].node_period = 0x00;
  mac_ND_array[ 3].node_frame_size = 0x80;  mac_ND_array[ 3].node_period = 0x00;
  mac_ND_array[ 4].node_frame_size = 0x80;  mac_ND_array[ 4].node_period = 0x00;
  mac_ND_array[ 5].node_frame_size = 0x80;  mac_ND_array[ 5].node_period = 0x00;
  mac_ND_array[ 6].node_frame_size = 0x80;  mac_ND_array[ 6].node_period = 0x00;
  mac_ND_array[ 7].node_frame_size = 0x80;  mac_ND_array[ 7].node_period = 0x00;
  mac_ND_array[ 8].node_frame_size = 0x80;  mac_ND_array[ 8].node_period = 0x03;
  /*mac_ND_array[60].node_frame_size = 0x80;  mac_ND_array[60].node_period = 0x00;
  mac_ND_array[61].node_frame_size = 0x80;  mac_ND_array[61].node_period = 0x01;
  mac_ND_array[62].node_frame_size = 0x80;  mac_ND_array[62].node_period = 0x00;
  mac_ND_array[63].node_frame_size = 0x80;  mac_ND_array[63].node_period = 0x00;
  mac_ND_array[ 1].node_frame_size = 0x80;  mac_ND_array[ 1].node_period = 0x01;
  mac_ND_array[ 2].node_frame_size = 0x80;  mac_ND_array[ 2].node_period = 0x01;
  mac_ND_array[ 3].node_frame_size = 0x80;  mac_ND_array[ 3].node_period = 0x01;
  mac_ND_array[ 4].node_frame_size = 0x80;  mac_ND_array[ 4].node_period = 0x01;
  mac_ND_array[ 5].node_frame_size = 0x80;  mac_ND_array[ 5].node_period = 0x01;
  mac_ND_array[ 6].node_frame_size = 0x80;  mac_ND_array[ 6].node_period = 0x02;
  mac_ND_array[ 7].node_frame_size = 0x80;  mac_ND_array[ 7].node_period = 0x01;
  mac_ND_array[ 8].node_frame_size = 0x80;  mac_ND_array[ 8].node_period = 0x03;*/
  mac_topo.bottom_address = 60;
  mac_topo.top_address = 8;
  mac_NA_array[0]  = 60;
  mac_NA_array[1]  = 61;
  mac_NA_array[2]  = 62;
  mac_NA_array[3]  = 63;
  mac_NA_array[4]  =  1;
  mac_NA_array[5]  =  2;
  mac_NA_array[6]  =  3;
  mac_NA_array[7]  =  4;
  mac_NA_array[8]  =  5;
  mac_NA_array[9]  =  6;
  mac_NA_array[10] =  7;
  mac_NA_array[11] =  8;
  printf ("\nn Telegram_Ovhd_us: %u",Telegram_Ovhd_us);
  printf ("\nn us_per_byte_mul10: %u",us_per_byte_mul10);
  ix=0;
  do{
    mac_NP_array[ix] = mac_ND_array[mac_NA_array[ix]].node_period;
  }
  while (mac_NA_array[ix++] != mac_topo.top_address);
#endif
  /*
  clear mac_inh_array and mac_slp_array
  */
  if (mac_first_topo)
  {
    node_ix=0;
    do{
      mac_inh_array[node_ix] = FALSE;
      mac_slp_array[node_ix] = FALSE;
    }
    while (mac_NA_array[node_ix++] != mac_topo.top_address);
  }
  opt_period = MAX_NODE_PERIOD ;
  any_change = FALSE ;
  do{
    if (opt_period == MAX_NODE_PERIOD)
    {
      any_change = FALSE ;
    }
    /*
    Build poll list
    */
    start_phase = 0;
    poll_ix=0;
    cnt_poll_pd = MAX(0, mac_configuration.source_port_count-1); /*4.1-26*/
    for (period_code=0, period=1;
         period_code <= MAX_NODE_PERIOD;
         period_code++, period <<= 1)
    {
      node_ix=0;
      phase = start_phase++;
      do{
        if ((mac_NP_array[node_ix]) == period_code)
        {
          mac_poll_list[poll_ix].NA = mac_NA_array[node_ix];
          mac_poll_list[poll_ix].IP = period;
          mac_poll_list[poll_ix].EC = mac_max_poll_try /*4.0-7a*/;
          /*4.1-26*/
          if (cnt_poll_pd > 0)
          {
            mac_poll_pd[poll_ix] = TRUE;
            cnt_poll_pd--;
          }
          else
          {
            mac_poll_pd[poll_ix] = FALSE;
          }
          mac_NIx_array[mac_poll_list[poll_ix].NA] = node_ix;
          mac_poll_list[poll_ix++].IC = phase;
          phase = (phase+1) % period ;
          macro_cycle = period;
        }
      }
      while (mac_NA_array[node_ix++] != mac_topo.top_address);
    }
    /*
    Check if poll list satifies conditions (returning "success")
    */
    success = TRUE ;
    number_of_nodes = poll_ix;
    macro_cycle_time_div256 = 0;
    for (macro_cycle_ix=1; macro_cycle_ix<=macro_cycle; macro_cycle_ix++)
    {
      /*
      calculate basic_cycle_time for macro cycle i
      */
      basic_cycle_time = 0;
      for (poll_ix=0; poll_ix<=number_of_nodes-1; poll_ix++)
      {
        mac_poll_list[poll_ix].IC++;
        if (mac_poll_list[poll_ix].IC == mac_poll_list[poll_ix].IP)
        {
          basic_cycle_time += Telegram_Ovhd_us
                           + us_per_byte_mul10 * mac_ND_array[mac_poll_list[poll_ix].NA].node_frame_size / 10;
          /*4.1-26*/
          if (mac_poll_pd[poll_ix])
          {
            basic_cycle_time += us_per_byte_mul10 * mac_ND_array[WTB_ADR_MASTER].node_frame_size/10;
          }
          mac_poll_list[poll_ix].IC = 0;
        }
      }
      /*
      test
      */
#ifdef POLLLISTSIMULATION
      printf ("\nbasic_cycle_time: %u",basic_cycle_time);
#endif
      if (basic_cycle_time > ( (1000 * BASIC_CYCLE) - PRESENCE_OVERHEAD))
      {
        success = FALSE;
        break;
      }
      else
      {
        macro_cycle_time_div256 += (basic_cycle_time >> 8);
      }
    }
    /*
    test
    */
#ifdef POLLLISTSIMULATION
    printf ("\n\nmacro_cycle_time_div256: %u",macro_cycle_time_div256);
#endif
    if (macro_cycle_time_div256 >= (((unsigned short)macro_cycle) * allowed_macro_time_div256))
    {
      success = FALSE;
    }
    if (success)
    {

    }
    else
    {
      /*
      Optimize
      */
      do{
        opt_period-- ;
        some_node = FALSE ;
        node_ix = 0;
        do{
          if ((mac_NP_array[node_ix]) == opt_period)
          {
            mac_NP_array[node_ix] ++ ;
            some_node = TRUE ;
          }
        }
        while (mac_NA_array[node_ix++] != mac_topo.top_address);
      }
      while ( (! some_node) && (opt_period != 0) );
      if (some_node)
      {
        any_change = TRUE ;
      }
      if (opt_period == 0)
      {
        opt_period = MAX_NODE_PERIOD ;
      }
    }
  }
  while ( (!success) &&
          ( ((!any_change) && (opt_period == MAX_NODE_PERIOD)) == FALSE) );
  return(success) ;
}
/*
test
*/
#ifdef POLLLISTSIMULATION
/*
main program for test
*/
int main (void)
{
  clrscr();
  BuildPollList();
  return 0;
}
#endif
/*
call back functions
*/
/*
void mac_zas_entry(...)
*/
void mac_zas_entry (void * p2msg)
{
  mac_p2msg = (Type_ZASElement *)p2msg ;
  zas_halt();
  mac_UDF_ZAS_Flag = TRUE ;
  mac_handler( ); /*4.1-31*/
}
/*
void mac_timer1_entry(void)
*/
void mac_timer1_entry (void)
{
  mac_UDF_TIMER1_Flag = TRUE ;
  mac_handler( ); /*4.1-31*/
}
/*
void mac_timer2_entry(void)
*/
void mac_timer2_entry (void)
{
  mac_UDF_TIMER2_Flag = TRUE ;
  mac_handler( ); /*4.1-31*/
}
/*
void mac_driver1_sc(...)
*/
/* #pragma argsused #iMB */
void mac_driver1_sc(type_idu_drv * p_idu, unsigned char send_state)
{
//#ifdef O_MAC_TRACE    /*4.1-12*//*<ATR:04>*/
  mac_sc1_cnt++ ;
//#endif/*<ATR:04>*/
  if (mac_SC_Flag1)
  {
    mac_UDF_SC_Flag = TRUE ;
    mac_handler( ); /*4.1-31*/
  }
}
/*
void mac_driver2_sc(...)
*/
/* #pragma argsused */
void mac_driver2_sc(type_idu_drv *p_idu, unsigned char send_state)
{
//#ifdef O_MAC_TRACE        /*4.1-12*//*<ATR:04>*/
  mac_sc2_cnt++ ;
//#endif/*<ATR:04>*/
  if (mac_SC_Flag2)
  {
    mac_UDF_SC_Flag = TRUE ;
    mac_handler( ); /*4.1-31*/
  }
}
/*
void mac_rcv1_evt(...)                             4.1-31
*/
void mac_rcv1_evt(    type_idu_drv *rcv_frame,
                      UNSIGNED8    line,
                      UNSIGNED8    sub_event
                      )
{
  /* store a possible frame */
      mac_save_event[0].sub_event = sub_event;
      mac_save_event[0].line      = line;
      mac_save_event[0].rcv_frame = rcv_frame;
      mac_handler();
}
/*
void mac_rcv2_evt(...)                             4.1-31
*/
void mac_rcv2_evt(    type_idu_drv *rcv_frame,
                      UNSIGNED8    line,
                      UNSIGNED8    sub_event
                      )
{
  /* store a possible frame */
      mac_save_event[1].sub_event = sub_event;
      mac_save_event[1].line      = line;
      mac_save_event[1].rcv_frame = rcv_frame;
      mac_handler();
}
/*
various
*/
/*
void mac_init(Type_WTBStatus    * WTB_status,
              Type_WTBNodes     * WTB_nodes,
              Type_MACStatistic * statistic_data)
*/
void mac_init(Type_WTBStatus    * WTB_status,
              Type_WTBNodes     * WTB_nodes,
              Type_MACStatistic * statistic_data)
{
  unsigned short seed ; /* 4.1-40 = (unsigned short)d_get_rand(); */
#ifdef UMS
  topo_freeze = FALSE;
  wtb_restricted = FALSE;
#endif
  p_WTB_status     = WTB_status;
  p_WTB_nodes      = WTB_nodes;
  p_statistic_data = statistic_data;
  zas_subscribe (PNR_MAC, mac_zas_entry) ;
  d_sysinit() ;          /* initialize driver functions */
  dl_system_init_pdi() ; /* init. PD-HANDLER */
  mac_md_system_init() ; /* messenger <-> MAC interface on MAC module */
  /*
  Initialize constants for node status
  */
  InvalidStatus.node_report = 0xFF ;
  InvalidStatus.user_report = 0xFF ;
  PowerUpStatus.node_report = INT_BIT | SAM_BIT ;
  PowerUpStatus.user_report = 0x00 ;
  /*
  initialize trace variables
  */
//#ifdef O_MAC_TRACE/*<ATR:04>*/
  for (mac_trace_ndx = 0 ; mac_trace_ndx < 1023 ; mac_trace_ndx++)
  {
    mac_trace[mac_trace_ndx] = 0x88 ;
  }
  mac_trace[1023] = 0x88 ;
  mac_trace_ndx = 0x00 ;
  mac_drv1_cnt = 0 ;
  mac_drv2_cnt = 0 ;
  mac_sc1_cnt = 0 ;
  mac_sc2_cnt = 0 ;
        mac_trigger[7] =0 ; /*<ATR:04>*/
      mac_trigger[6] = 0; /*<ATR:04>*/
      mac_trigger[5] = 0 ;
      mac_trigger[4] = 0 ;
      mac_trigger[3] = 0 ;//num retry
     mac_trigger[2] = 0 ;//num retry
     mac_trigger[1] =0 ;//num retry
     mac_trigger[0] =0 ;//next state

//#endif/*<ATR:04>*/
  /*
  initialize ZAS messages
  */
  /* message to BM */
  mac_msg2bm.pnr = PNR_BM ;

  /*
  initialize WTB_status
  */
  p_WTB_status->WTB_hardware   = WTB_HARDWARE_VERSION ;
  p_WTB_status->WTB_software   = WTB_SOFTWARE_VERSION ;
  p_WTB_status->hardware_state = HARDWARE_STATE_OK ;
  p_WTB_status->net_inhibit    = FALSE;
  p_WTB_status->node_address   = ADR_UNNAMED;
  p_WTB_status->node_orient    = L_UNKNOWN;
  p_WTB_status->node_strength  = L_UNDEFINED;
  p_WTB_status->node_status    = PowerUpStatus;
  /*
  reset statistical data
  */
  p_statistic_data->basic_period_cnt = 0;
  p_statistic_data->inaug_cnt        = 0;
  p_statistic_data->topo_cnt         = 0;
  d_sw_line_receive (/* channel 1 */ D_LINE_AUTO,
                     /* channel 2 */ D_LINE_AUTO) ;
  /*
  initialize WTB_nodes
  */
  p_WTB_nodes->bus_identifier  = 0;
  p_WTB_nodes->number_of_nodes = 0;
  mac_topo.topo_counter = 0;
  seed = (unsigned short)d_get_rand();  /* 4.1-40*/
  set_random_seed ( (signed short) seed );
  mac_last_node_addr = ADR_STARTUP ;
  mac_CI_bits        = 0x00 ;
  mac_master_topo    = seed & 0x0FFF ;
  mac_busy  = FALSE ;
  mac_state = S_UNCONFIGURED ;
  mac_event = NO_EVENT ;
  mac_save_event[0].sub_event = D_RPT_NO_ENTRY;    /*4.1-31*/
  mac_save_event[1].sub_event = D_RPT_NO_ENTRY;    /*4.1-31*/
  mac_SC_Flag1        = FALSE ;
  mac_SC_Flag2        = FALSE ;
  mac_UDF_SC_Flag     = FALSE ;
  mac_UDF_TIMER1_Flag = FALSE ;
  mac_UDF_TIMER2_Flag = FALSE ;
  mac_UDF_ZAS_Flag    = FALSE ;
  mac_main_direction =  MAC_DIR1 ;
  mac_DF_flag = FALSE ;
                                         /*4.0-7a*/
  mac_max_smc         = MAX_SMC;
  mac_max_status_fail = MAX_STATUS_FAIL;
  mac_max_try         = MAX_TRY;
  mac_max_poll_try    = MAX_POLL_TRY;
  mac_fritt_source_on = 0x00;            /*4.0-7b*/
  mac_addr_inaug_line_error = 0x00;      /*4.0-8*/
  /*
  prepare some frequently used frames
  */
  PD_req_frm.sd  =  ADR_MASTER ;
  PD_req_frm.llc =  PD_REQUEST ;
  PD_req_frm.size = PD_REQUEST_SIZE ;
  MD_req_frm.sd  =  ADR_MASTER ;
  MD_req_frm.llc =  MD_REQUEST ;
  MD_req_frm.size = MD_REQUEST_SIZE ;
  Det_req_frm.dd   = ADR_UNNAMED ;
  Det_req_frm.sd   = ADR_UNNAMED ;
  Det_req_frm.llc  = SVF_DETECT_REQUEST ;
  Det_req_frm.size = SVF_DETECT_REQUEST_SIZE ;
}
/*
void mac_general_events(void)
*/
void mac_general_events(void)
{
  switch (mac_event)
  {
    /*
    UDF_DIR1
    */
    case UDF_DIR1 :
    {
    	      unsigned char tmo_stop;/*<ATR:05>*/

      if (mac_main_direction == MAC_DIR2)
      {
        if ( (mac_state >= S_NAMING_AWAIT_INT_RES)       &&
             (mac_state <= S_MASTER_AWAIT_SC_PRESENCE_RES)
           )
        {
          AuxResponse();
        }
        else
        {
          diagwtb_entry(
                      DIAG_MAC_ERR_AUX,
                      (unsigned short) mac_state,
                      (unsigned short) mac_event,
                      0x0001,
                      (void*)0
                    ) ;
        
        }
      }
      else
      {
        /* ignore all frames ( SVF_DETECT_REQUEST is likely here) */
      }
      break;
    }
    /*
    UDF_DIR2
    */
    case UDF_DIR2 :
    {
	      unsigned char tmo_stop;/*<ATR:05>*/

      if (mac_main_direction == MAC_DIR1)
      {
        if (
          (mac_state >= S_NAMING_AWAIT_INT_RES)  &&
          (mac_state <= S_MASTER_AWAIT_SC_PRESENCE_RES)
           )
        {
          AuxResponse();
        }
        else
        {
          diagwtb_entry(
                      DIAG_MAC_ERR_AUX,
                      (unsigned short) mac_state,
                      (unsigned short) mac_event,
                      0x0002,
                      (void*)0
                    ) ;
        }
      }
      else
      {
        /* ignore all frames ( SVF_DETECT_REQUEST is likely here) */
      }
      break;
    }
    /*
    MAC_BM_Stop_Req
    */
    case MAC_BM_Stop_Req :
    {
      unsigned char tmo_stop;/*<ATR:05>*/

      d_tc_timer_reset(MAC_TIMER1, 0) ;
      Disable(MAC_DIR1)  ;
      Disable(MAC_DIR2)  ;
      mac_last_node_addr = ADR_STARTUP ;
      p_WTB_nodes->number_of_nodes = 0 ;
      p_WTB_status->net_inhibit = FALSE ;
      /*
      set SAM-Bit in p_WTB_status->node_report
      */
      pi_disable() ;
      p_WTB_status->node_status.node_report |= SAM_BIT ;
      pi_enable() ;
      p_WTB_status->node_strength = L_UNDEFINED;
      p_WTB_status->node_orient   = L_UNKNOWN;
      if (mac_state == S_MASTER_AWAIT_PD_RES)/*<ATR:05>*/
            tmo_stop = 7*TMO_STOP;
         else
            tmo_stop = TMO_STOP;

      d_tc_timer_reset(MAC_TIMER2, tmo_stop);/*<ATR:05>*/
      /* remember state here for late PD/MD confirmation*/
      mac_stop_state = mac_state;
#ifdef O_MVBC
//    lc_set_device_status_word(LC_DSW_DYD_MSK, LC_DSW_DYD_CLR);
#endif
      mac_state = S_AWAIT_STOP_DELAY ;
      break;
    }
    /*
    MAC_BM_ChgNodeDesc_Cmd
    */
    case MAC_BM_ChgNodeDesc_Cmd :
    {
      /* copy the new node descriptor */
      mac_configuration.node_descriptor =
                    *((Type_NodeDescriptor*)(mac_p2msg->p34)) ;
      p_WTB_status->node_descriptor = mac_configuration.node_descriptor ;
      pi_disable() ;
      p_WTB_status->node_status.node_report |= DSC_BIT ; /* set DSC-Bit in p_WTB_status->node_report */
      mac_CI_bits |= C_BIT ;                    /* set C-Bit */
      pi_enable() ;
      /* same state */
      break;
    }
    /*
    MAC_BM_ChgUserReport_Cmd
    */
    case MAC_BM_ChgUserReport_Cmd :
    {
      pi_disable() ;
      /* set mask */
      p_WTB_status->node_status.user_report |= (Byte)mac_p2msg->p1 ;
      /* clear mask */
      p_WTB_status->node_status.user_report &= ~( (Byte)mac_p2msg->p2 ) ;
      /* set C-Bit */
      mac_CI_bits |= C_BIT ;
      pi_enable() ;
      /* same state */
      break;
    }
    /*
    MAC_BM_Inhibit_Cmd
    */
    case MAC_BM_Inhibit_Cmd :
    {
      pi_disable() ;
      mac_CI_bits |= I_BIT ;  /* set I-Bit */
      pi_enable() ;
      /* same state */
      break;
    }
    /*
    MAC_BM_Allow_Cmd
    */
    case MAC_BM_Allow_Cmd :
    {
      pi_disable() ;
      mac_CI_bits &= ~I_BIT ; /* clear I-Bit */
      pi_enable() ;
      /* same state */
      break;
    }
    /*
    MAC_BM_SetStrong_Cmd
    */
    case MAC_BM_SetStrong_Cmd :
    {
      mac_master_rank = STRONG ;
      /* same state */
      break;
    }
    /*
    MAC_BM_SetWeak_Cmd
    */
    case MAC_BM_SetWeak_Cmd :
    {
      mac_master_rank = WEAK ;
      /* same state */
      break;
    }
    /*
    MAC_BM_Sleep_Cmd
    */
    case MAC_BM_Sleep_Cmd :
    {
      pi_disable() ;
      p_WTB_status->node_status.node_report |= SLP_BIT ; /* set SLP-Bit in p_WTB_status->node_report */
      mac_CI_bits |= C_BIT ;                    /* set C-Bit */
      pi_enable() ;
      /* same state */
      break;
    }
    /*
    MAC_BM_CancelSleep_Cmd
    */
    case MAC_BM_CancelSleep_Cmd :
    {
      pi_disable() ;
      p_WTB_status->node_status.node_report &= ~SLP_BIT ; /*clear SLP-Bit in p_WTB_status->node_report*/
      mac_CI_bits |= C_BIT ; /* set C-Bit */
      pi_enable() ;
      /* same state */
      break;
    }
    /*
    SC_DIR12
    */
    case SC_DIR12 :
    {
      /* accept a late SC_DIR12 event from main channel */
      CLEAR_SC_FLAG ;
      break;
    }
    /*
    UDF_TIMER1,2
    */
    case UDF_TIMER1 :
    case UDF_TIMER2 :
    {
      /* ignore a late timer event
         (it was preemted ) */
      break;
    }
    default:
    {
      diagwtb_entry(
                  DIAG_MAC_ERR_EVENT,
                  (unsigned short) mac_state,
                  (unsigned short) mac_event,
                  0x8888,
                  (void*)0
                ) ;
      zas_halt() ;
    }
  }
}
/*
void mac_error(void)
*/
void mac_error(void)
{
  diagwtb_entry(
              DIAG_MAC_ERR_GENERAL,
              (unsigned short) mac_state,
              (unsigned short) mac_event,
              0x8888,
              (void*)0
            ) ;
  zas_halt() ;
}
/*
random number generator
*/
/* defines the seed value for the pseudo random number sequence:
   input is unsigned short */

void set_random_seed (signed short seed)
{
  mac_current_random_number = seed ;
}
/* returns a pseudo random number, integer [-4;+4] */

signed short get_random_number (void)
{
  return (
           (mac_current_random_number =
            (mac_current_random_number * KONST_M + KONST_K )
           ) % 5
         );
}
/*
void scan_WTB_nodes (void)
*/
void scan_WTB_nodes (void)
{
  /* update mac_MR */
  Boolean flag_master_passed = FALSE ;
  unsigned char ix;
  Byte tmp_NR;
  Boolean main_A_disturbed = FALSE;
  Boolean main_B_disturbed = FALSE;
  /*
  find out, if any node signals its main channel line (relative to master) disturbed (update "mac_main_A_disturbed" or
  "mac_main_B_disturbed")
  */
  /* The array p_WTB_nodes->node_status_list is scanned from bottom to top.
  The flag "flag_master_passed" is used to remember that the master node has been passed. The nodes "before" the
  master (flag_master_passed = FALSE) have their main channel in direction 2 (1 if orientation is inverse). The nodes
  "after" the
  master (flag_master_passed = TRUE) have their main channel in direction 1 (2 if orientation is inverse).

  The master node is then treated specially.

  scan direction of "p_WTB_nodes->node_status_list":

             ---------------->

          ___      ___      ___      ___
     ____/   \____/   \____/   \____/   \ ___
    |1  2|   |1  2|   |1  2|   |1  2|   |1  2|
    |A  M|   |O  M|   |    |   |M  O|   |M  A|
    |    |   |    |   |    |   |    |   |    |
    |Bot-|   |    |   |Mas-|   |    |   |Top |
    |tom |   |    |   | ter|   |    |   |    |
    |____|   |____|   |____|   |____|   |____|

  A means auxiliary channel
  M means main channel
  O means channel switched off

  */
  ix = 0 ;
  while (ix != mac_topo.number_of_nodes)
  {
    if (mac_NA_array[ix] == ADR_MASTER)
    {
      flag_master_passed = TRUE ;
    }
    else
    {
      tmp_NR = p_WTB_nodes->node_status_list[ix].node_report ;
      /* ignore failing intermediate nodes */
      if (tmp_NR == InvalidStatus.node_report)
      {

      }
      else
      {
        /* if same orientation */
        if (tmp_NR & SAM_BIT)
        {
          if (flag_master_passed)
          {
            main_A_disturbed |= (tmp_NR & DA1_BIT) ;
            main_B_disturbed |= (tmp_NR & DB1_BIT) ;
          }
          else
          {
            main_A_disturbed |= (tmp_NR & DA2_BIT) ;
            main_B_disturbed |= (tmp_NR & DB2_BIT) ;
          }
        }
        else
        {
          if (flag_master_passed)
          {
            main_A_disturbed |= (tmp_NR & DB2_BIT) ;
            main_B_disturbed |= (tmp_NR & DA2_BIT) ;
          }
          else
          {
            main_A_disturbed |= (tmp_NR & DB1_BIT) ;
            main_B_disturbed |= (tmp_NR & DA1_BIT) ;
          }
        }
      }
    }
    ix++;
  }
  /* treat master */
  if (mac_main_direction == MAC_DIR1)
  {
    main_A_disturbed |= (p_WTB_status->node_status.node_report & DA1_BIT) ;
    main_B_disturbed |= (p_WTB_status->node_status.node_report & DB1_BIT) ;
  }
  else
  {
    main_A_disturbed |= (p_WTB_status->node_status.node_report & DA2_BIT) ;
    main_B_disturbed |= (p_WTB_status->node_status.node_report & DB2_BIT) ;
  }
  mac_MR = 0 ;
  if (main_A_disturbed)
  {
    mac_MR |= DA_BIT;
  }
  if (main_B_disturbed)
  {
    mac_MR |= DB_BIT;
  }
}
/*
void update_line_status_bits (void)
*/
void update_line_status_bits (void)
{
  unsigned char st;
  /* if a macroperiod is through then set the C-BIT if integrated line status has changed since last macro period */
  if (--line_status_update_counter == 0)
  {
    line_status_update_counter = (1 << MAX_NODE_PERIOD);
    d_get_int_line_status (&st) ;
    pi_disable() ;
    p_WTB_status->node_status.node_report &=  LINE_STATUS_MASK;
    p_WTB_status->node_status.node_report |= st ;
    pi_enable() ;
    if (st == mac_last_line_status)
    {

    }
    else
    {
      mac_last_line_status = st ;
      pi_disable() ;
      mac_CI_bits |= C_BIT ;
      pi_enable() ;
    }
  }
}
/*
void mac_shutdown (unsigned short reason, unsigned short param)
*/
void mac_shutdown (unsigned short reason, unsigned short param)
{
  char buff [18];
  diagwtb_entry(
              reason,
              (unsigned short) mac_state,
              (unsigned short) mac_event,
              param,
              (void*)0
             ) ;
  Disable(MAC_DIR1)  ;
  Disable(MAC_DIR2)  ;
#ifdef O_960
  SPrint_F (buff, "WTB_LL dead %2u %2u", reason, param);
  nse_record( "MAC.C",__LINE__,
              NSE_EVENT_TYPE_ERROR,
              NSE_ACTION_HALT,
              sizeof(buff),
              buff
            );
#endif

}
/*
static void mac_fritting_off()              4.0-7b
*/
static void mac_fritting_off(
                 Type_Dir    direction)
{
  unsigned char dir_off = MAC_DIR_TO_BITFIELD(direction) & mac_fritt_source_on;
  if (mac_configuration.fritting_disabled == TRUE)
  {
    return;
  }
  switch (dir_off)
  {
    case 0x01:
    {
      /*4.1-11*/
      d_sw_frittung_control( D_FRITTING_PASSIVE, D_DIRECT_1, D_LINE_B);
      break;
    }
    case 0x02:
    {
      /*4.1-11*/
      d_sw_frittung_control( D_FRITTING_PASSIVE, D_DIRECT_2, D_LINE_A);
      break;
    }
    case 0x03:
    {
      /*4.1-11*/
      d_sw_frittung_control( D_FRITTING_PASSIVE, D_DIRECT_1, D_LINE_B);
      d_sw_frittung_control( D_FRITTING_PASSIVE, D_DIRECT_2, D_LINE_A);
      break;
    }
  }
  mac_fritt_source_on &= ~dir_off;
}
/*
static void mac_fritting_on()               4.0-7b
*/
static void mac_fritting_on(
                 Type_Dir    direction)
{
  unsigned char dir_on = MAC_DIR_TO_BITFIELD( direction) & ~mac_fritt_source_on;
  if (mac_configuration.fritting_disabled == TRUE)
  {
    return;
  }
  switch (dir_on)
  {
    case 0x01:
    {
      /*4.1-11*/
      d_sw_frittung_control( D_FRITTING_ACTIVE, D_DIRECT_1, D_LINE_B);
      break;
    }
    case 0x02:
    {
      /*4.1-11*/
      d_sw_frittung_control( D_FRITTING_ACTIVE, D_DIRECT_2, D_LINE_A);
      break;
    }
    case 0x03:
    {
      /*4.1-11*/
      d_sw_frittung_control( D_FRITTING_ACTIVE, D_DIRECT_1, D_LINE_B);
      d_sw_frittung_control( D_FRITTING_ACTIVE, D_DIRECT_2, D_LINE_A);
      break;
    }
  }
  mac_fritt_source_on |= dir_on;
}
/*
static void mac_fritting_test_main()        4.0-7b
*/
static void mac_fritting_test_main(
                          unsigned char line_status )
/* purpose:   tests whether a frame (Presence_Response) has not only been received on the line
 *            for which the node controls the fritting source
 *            may switch on the fritting source with respect to the defined guard times
 * assumtion: node is an end-node
 */

{
  if (mac_configuration.fritting_disabled == TRUE)
  {
    return;
  }
     /* test whether the frame was only received on one line */
  if (  ( (mac_main_direction == MAC_DIR1) && (line_status & D_LINE_B_ERR) )
     || ( (mac_main_direction == MAC_DIR2) && (line_status & D_LINE_A_ERR) ) )
  {
    /* fritting on *//*4.1-34*/
    mac_fritting_on( mac_main_direction);
  }
  else
  {
    /* no need for fritting *//*4.1-34*/
    mac_fritting_off( mac_main_direction);
  }
}
/*
void mac_get_info()                         4.0-8
*/
/*4.0-8*/
void mac_get_info
                            ( unsigned char  *addr_inaug_line_error
                            , unsigned char  *fritt_source_on
                            )
/* purpose:   returns additional information about the MAC
 *
 * returns:   addr_inaug_line_error: the node address where a line error
 *               has een detected during inauguration
 *               0x00       no line error
 *               otherwise  link_layer address of the error node
 *            fritt_source_on: bitfield of the directions where the fritting
 *               source currently is switched on
 *               0x00       no fritting source is on
 *               0x01       fritting source in dir_1 is on
 *               0x02       fritting source in dir_2 is on
 *               0x03       fritting source in both directions are on
 */
{
  *addr_inaug_line_error = mac_addr_inaug_line_error;
  *fritt_source_on       = mac_fritt_source_on;
}
