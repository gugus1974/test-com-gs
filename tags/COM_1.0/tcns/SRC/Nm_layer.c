/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> avoided NULL pointer referencing                                                    */
/* <ATR:02> the QNX compiler prefers 960-like definitions                                       */
/* <ATR:03> initialize the result value                                                         */
/*==============================================================================================*/


/************************************************************************/
/*  Asea Brown Boveri Research Center, CH-5405 Baden, Switzerland       */
/*  Copyright 1993                                                      */
/*                                                                      */
/*  NAME:                                                               */
/*    NM_LAYER.C                                                        */
/*                                                                      */
/*  ABSTRACT:                                                           */
/*    TCN Message Transport Protocol, Network Layer.                    */
/*    Accesses the link layers through the bus dispatcher NM_DISP. A    */
/*    link layer is identified by its bus number bus_nr<AM_MAX_BUSSES.  */
/*    Several vehicle busses and/or 1 train bus may be connected.       */
/*    Resets the MD_SEND_ACTIVE bit of the link layer status to perform */
/*    bus send timeout supervision for each VB.                         */
/*    Received packets which cannot be delivered are passed to the      */
/*    upper layer together with an error code, which allows error       */
/*    propagation back to the origin by the upper layer.                */
/*    Local addressing is used within the same vehicle normally, origin */
/*    and final vehicle are then both = AM_SAME_VEHICLE. Local addr.    */
/*    does not depend on inaugurations and is restricted to communica-  */
/*    tion within one vehicle. Global addressing must be used to access */
/*    the whole TCN.                                                    */
/*                                                                      */
/*    If there is a TB:                                                 */
/*    A 7-bit inauguration counter is maintained which is incremented   */
/*    after each inauguration and never has value 0 (1..127, cyclic).   */
/*    This inauguration counter exists only if there is a TB connected. */
/*    It is used to replace the own vehicle in the network header in    */
/*    case of global addressing. Packets consumed or sent to a VB con-  */
/*    tain the inauguration counter in the final vehicle field. For     */
/*    generated packets or packets incoming from a VB, the origin       */
/*    vehicle field is checked to be equal to inauguration counter or   */
/*    AM_SAME_VEHICLE and is then replaced by the own vehicle address.  */
/*    Bit 7 of the final/origin vehicle field is always used to distin- */
/*    guish logical from physical addressing.                           */
/*    The 'inaug_mutex' semaphore is used for the following reasons:    */
/*    - The own vehicle address always must be consistent with the      */
/*      inauguration counter. Both are changed with a TB link indic.    */
/*    - The correct inauguration counter must be assigned to packets    */
/*      incoming from the TB.                                           */
/*    - Packets sent to the TB must contain the correct origin vehicle. */
/*                                                                      */
/*  DEPENDENCIES:                                                       */
/*    uses module NM_MANAG for train configuration management and the   */
/*    function directory table AM_DIREC as well as the station direc-   */
/*    tory NM_STADI to retrieve routing information.                    */
/*                                                                      */
/* CONFIGURATION DEFINES:                                               */
/*    Several versions can be generated for this module. They are pro-  */
/*    vided for code optimization only and they all have exactly the    */
/*    same behaviour as the full version.                               */
/*    One of the following defines can be used to get a subset version: */
/*    - NM_1VB_SIMPLE: there is just 1 VB and no TB, simple routing     */
/*    - NM_1VB       :   "    "  "   "  "  "  "  " , no simple routing  */
/*    - NM_1TB       : there is 1 TB and no VB, same as simple routing  */
/*    - NM_GW_SIMPLE : there is 1 VB and 1 TB, simple routing           */
/*    - NM_GW_NO_TB  : there are several VBs but no TB, no simple rout. */
/*    The station table NM_STADI.C is not used in case of simple rou-   */
/*    ting. Modules NM_MANAG.C and NM_GROUP.C are not used if there is  */
/*    no TB.                                                            */
/*                                                                      */
/************************************************************************/
/*                                                                      */
/*  AUTHOR  :   26-Oct-1993     CRBC1   Hoepli                          */
/*  MODIFIED:   15-Nov-1993     CRBC1   Hoepli  char -> unsigned char   */
/*              04-Jan-1994     CRBC1   Hoepli  link_dev = next_hop     */
/*              27-Jan-1994     CRBC1   Hoepli     #ifdef O_960         */
/*              10-Feb-1994     CRBC1   Hoepli  t_status_ind: +put_pool */
/*              21-Feb-1994     CRBC1   Hoepli      "  : flush always   */
/*              24-Feb-1994     CRBC1   Hoepli  v_status_ind: hamster   */
/*              03-Mar-1994     CRBC1   Hoepli  PI_TICK for BUS_TMO     */
/*              07-Mar-1994     CRBC1   Hoepli  define DU_MODULE_NAME   */
/*              21-Jun-1994     CRBC1   Hoepli  GW as station 0 from VB */
/*              18-Jul-1994     CRBC1   Hoepli  topo count from ls_ if  */
/*  (topo count my_tc = inauguration counter, now exists also if no TB) */
/*              04-Aug-1994     CRBC1   Hoepli  next_sta in AM_ADDRESS  */
/*                - nm_send: dest.link_dev is always next station       */
/*                  (send to TB only if next_station is own station)    */
/*                - each device listens to station 0 and 255            */
/*                - function directory may map to non-next stations     */
/*                - don't report errors from link layer in nm_send_pack */
/*                  (all errors from network layer fail again on retry) */
/*               15-Feb-1995     BATL    Flum     Switch O_MD_STDA to   */
/*                                                compile the sources   */
/*                                                without C standard lib*/
/*               11-05-1995      BATL    Flum     Switch Discard to test*/
/*                                                the transport layer   */
/*               16-06-1995      BATL    FLum     abort_TB_conn         */
/*               14-09-1995      BATL    Flum     additional parameter  */
/*                                                ts_id for link        */
/*                                                layer functions       */
/*               11-10-1995      BATL    Flum     procedure             */
/*                                                nm_change_packet_lif..*/
/*                                                for TNM added         */
/*               15-11-1995      BATL    Flum     AM_ST_ENTRY changed   */
/************************************************************************/

#define DU_MODULE_NAME "NM_LAYE"
/* for hamster calls which are now mapped to JDP event recorder.
   DU_MODULE_NAME must be defined before "du_sys.h" is included.
*/

/* some constructed configuration defines for conditional compilation: */
/* <ATR:02> */
#if defined(O_960) || defined(__QNX__)
/*#kra 24.01.94 changed syntax of compiler switches (deleted: defined () )*/
#define NM_VB_END   (NM_1VB || NM_1VB_SIMPLE)
#define NM_NO_TB    (NM_VB_END || NM_GW_NO_TB) 
#define NM_END      (NM_VB_END || NM_1TB)
#define NM_SIMPLE   (NM_1VB_SIMPLE || NM_GW_SIMPLE || NM_1TB)
#define NM_HAS_TB   (NM_1TB || NM_GW_SIMPLE)
#define NM_NOT_END  (NM_GW_SIMPLE || NM_GW_NO_TB)
#define NM_NOT_1TB  (NM_VB_END || NM_NOT_END)
#define NM_NOT_1VB  (NM_1TB || NM_NOT_END)
#else
#define NM_VB_END   (defined ( ## NM_1VB ## ) || defined ( ## NM_1VB_SIMPLE ## ))
#define NM_NO_TB    (NM_VB_END || defined ( ## NM_GW_NO_TB ## )) 
#define NM_END      (NM_VB_END || defined ( ## NM_1TB ## ))
#define NM_SIMPLE   (defined ( ## NM_1VB_SIMPLE ## ) || defined ( ## NM_GW_SIMPLE ## ) || \
                     defined ( ## NM_1TB ## ))
#define NM_HAS_TB   (defined ( ## NM_1TB ## ) || defined ( ## NM_GW_SIMPLE ## ))
#define NM_NOT_END  (defined ( ## NM_GW_SIMPLE ## ) || defined ( ## NM_GW_NO_TB ## ))
#define NM_NOT_1TB  (NM_VB_END || NM_NOT_END)
#define NM_NOT_1VB  (defined ( ## NM_1TB ## ) || NM_NOT_END)
#endif

/* INCLUDES: ---------------------------------------------------------- */

#if defined (O_MD_STDA)
#define NULL     0
#else
#include <stdio.h>		/* NULL */
#endif

#ifdef DISCARD
#include "am_test.h"
#include "stdlib.h"
#endif

#include "md_ifce.h"
#include "am_sys.h"
#include "basetype.h"
#include "md_layer.h"
#include "lx_param.h"
#include "mm_pool.h"
#include "pi_sys.h"
#include "nm_layer.h"
#include "nm_disp.h"

#if !NM_SIMPLE
#include "nm_stadi.h"
#endif
#if !NM_NO_TB
#include "am_group.h"
#include "nm_manag.h"
#endif

/* CONST: ------------------------------------------------------------- */

/* EH 03-Mar-1994, BUS_TMO now adapts itself to PI_TICK: */
#define BUS_TMO_MS    960UL   /* units: [ms] */              
#define BUS_TMO      ((BUS_TMO_MS + PI_TICK - 1) / PI_TICK)      
/* units: ticks = period for timeout process, rounded to next higher */

/* default pool sizes: */
#define RCV_POOL_DFT_SIZE         50
#define TRANSIT_POOL_DFT_SIZE     20

/* maximal number of packets to poll within 'nm_rcv_pack': */
#define MAX_POLLED_PACK           20
/* maximal number of packets to poll from the same bus before polling
   another bus:
*/
#define MAX_STAY_PACK              3


#ifdef DISCARD
    /* enables random rejection of received frames */
    #define DISCARD_RANGE 1000
    #define DISCARD_LEVEL  100
    /* reject 'DISCARD_LEVEL' frames per 'DISCARD_RANGE' received frames */
    extern unsigned long accepted_frames , rejected_frames ;
#endif


/* TYPES: ------------------------------------------------------------- */

typedef struct {
void           *id;   /* pool identifier (root of pool) */
unsigned short  size; /* number of packets in the pool */
} POOL_INFO;

typedef struct {
unsigned char sent_more; /* 'lm_send_request' was called during this interval */
unsigned char pending;   /* {'lm_send_request' was called and no frames have
                            been transmitted} during the previous interval */
} BUS_SV_STORE;  /* memory used for bus supervision */

typedef enum {
PACK_NONE = 0,   /* no more packet in the receive queue */
PACK_TRANSIT,    /* transit packet to be forwarded to 'rcv.out_bus_nr' */
PACK_CONSUME     /* packet for myself or error */
} POLL_RESULT; /* result with polling for received packets */

typedef enum { /* added EH 04-Aug-1994 */
PR_NONE,       /* no pre-routing requirements */
PR_TB,         /* pre-routing requires forwarding to TB */
PR_VB          /* pre-routing requires forwarding to VB */
} PRE_ROUT;    /* pre-routing, done by session layer for outbound packets */

/* PROTOTYPES: -------------------------------------------------------- */

#ifndef NM_1TB
static void activate_bus_sv (void *);
static void do_bus_sv (void);
static void v_status_ind (int reason);
static POLL_RESULT v_get_pack (MD_PACKET **pack,
                               unsigned char *src_dev, 
                               unsigned char *dest_dev,
                               AM_RESULT *error);
#endif
#if !NM_NO_TB
static void t_status_ind (int reason);
static void check_hdr (MD_PACKET *pack, /* inout */
                       unsigned char *dest_veh,
                       AM_RESULT *error,
                       PRE_ROUT cond);  /* added EH 04-Aug-1994 */
static POLL_RESULT t_get_pack (MD_PACKET **pack,
                               unsigned char *src_dev, 
                               unsigned char *dest_dev,
                               AM_RESULT *error);
#endif
#if !NM_SIMPLE
static AM_RESULT check_st_entry (AM_ST_ENTRY *new);
#endif

/* VARIABLES: --------------------------------------------------------- */

#ifdef TRACE_ENB
/* binary switch 3 can be used to simulate inaugurations */
static int last_binary = 0;
#endif

/* pools of free packets for the receivers in the link layers: */
static POOL_INFO link_rcv_pool[AM_MAX_BUSSES]; 

#if !NM_END
/* pools of transit packets outgoing to a link layer: */
static POOL_INFO transit_pool[AM_MAX_BUSSES];
#endif

static unsigned char no_pool; /* boolean: pool not yet created */

static struct {
unsigned char has_tb;      /* one train bus and may be some vehicle busses */
unsigned char one_tb;      /* one train bus, but no vehicle bus */
unsigned char one_vb;      /* one vehicle bus, but no train bus */
} bus_cfg; /* short info about the connected busses */

static struct {
unsigned char nr_of_busses;          /* number of valid entries in 'bus_nr' */
unsigned char bus_nr[AM_MAX_BUSSES]; /* list of bus numbers */
/* bus_nr[nr_of_busses] is the TB if connected */
} vb; /* connected vehicle busses */

static struct {
unsigned char  ix;           /* next index into 'vb.bus_nr' for polling */
#if !NM_END
unsigned short stay_cnt;     /* number of packets polled from bus 'ix' */
#endif
unsigned char  non_empty_ix; /* last bus 'ix' that delivered packets */
unsigned char  now_empty;    /* queue of bus 'non_empty_ix' is now empty */
} poll;

#ifndef NM_1TB
static struct {
short          timer;      /* timer to activate cyclic bus supervision */
unsigned char  requ;       /* cyclic bus supervision is requested */
} bus_sv_control;    /* activation of bus supervision */

static BUS_SV_STORE bus_sv[AM_MAX_BUSSES];
#endif

static struct {
unsigned char out_bus_nr;  /* bus to be used for out transit */
unsigned char phys_bit;    /* physical addressing bit */
} rcv; /* information about a polled packet */

#if !NM_NO_TB
static short inaug_mutex;  /* semaphore */
#endif

static void (*nm_abort_TB_conn) (void);


/* FUNCTIONS: --------------------------------------------------------- */

void _am_nm_config (unsigned char bus_nr,
                    unsigned short rcv_pool_size,
                    unsigned short transit_pool_size,
                    AM_RESULT *status)
/* Defines the sizes of the receive pool and the transit pool for one bus.
   There is no transit pool, and 'rcv_pool_size' is just used to clip
   a dynamically defined pool size in case there is just one bus connected.
   This function has no effect if it is called after 'nm_init', because
   the pools are already created then using default values.
*/
{
  if (no_pool)
  {
    link_rcv_pool[bus_nr].size = rcv_pool_size;
#if !NM_END
    transit_pool [bus_nr].size = transit_pool_size;
#endif
#if !NM_NOT_END
#if !NM_END
    if (bus_cfg.one_vb || bus_cfg.one_tb) 
#endif
    {
      hamster_continue (transit_pool_size);
    }
#endif
    *status = AM_OK;
  }
  else
  {
    *status = AM_FAILURE;
  }
} /* _am_nm_config */

/**********************************************************************/
#if !NM_NO_TB
static void check_hdr (MD_PACKET *pack, /* inout */
                       unsigned char *dest_veh,
                       AM_RESULT *error,
                       PRE_ROUT cond)
/* Checks the network header of the packet and optionally changes there
   the origin/final vehicle field. Returns the destination vehicle without
   the physical addressing bit in *dest_veh, and AM_SAME_VEHICLE if
   the destination vehicle is the own vehicle.
   This function is just called if there is a train bus connected.
   Global addressing with AM_SAME_VEHICLE as final vehicle is just
   used internally by the network layer in case of errors.

   - Returns AM_INAUG_ERR if the origin vehicle field is not valid.
   - Defines the origin vehicle field in case of global addressing
     if final vehicle != AM_SAME_VEHICLE.
   - Returns AM_UNKNOWN_DEST_ERR if there is just one TB and not
     global addressing is used.
   - Checks whether the final vehicle exists in the actual train config.
   - Replaces the final vehicle field by the inauguration counter in
     case of global addressing.

   EH 04-Aug-1994, 'cond' added: (- PR_NONE: as before)
   - PR_TB: do not change final vehicle if final vehicle is own
   - PR_VB: do not change origin vehicle if final vehicle != own
*/
{
unsigned char tmp_veh, my_veh, phys_bit;

  *dest_veh = pack->port.lk.final_vehicle;
  phys_bit = *dest_veh & AM_PHYS_ADDR;
  *dest_veh &= ~AM_PHYS_ADDR;
  tmp_veh = pack->port.lk.origin_vehicle & ~AM_PHYS_ADDR;
  *error = AM_OK;
  if (*dest_veh == AM_SAME_VEHICLE) {
    if (tmp_veh == AM_SAME_VEHICLE) {
/* EH 04-Aug-1994, this is now checked in 'nm_send_pack' 
#if !NM_NOT_1TB
      if (bus_cfg.one_tb) {
        *error = AM_UNKNOWN_DEST_ERR;
      }
#endif
*/
    } else {
      pack->port.lk.final_vehicle = phys_bit | md_get_my_tc();
      *error = AM_FAILURE;
    } /* if (tmp_veh == AM_SAME_VEHICLE) */
  } else {
    if ((tmp_veh == AM_SAME_VEHICLE) || (tmp_veh == md_get_my_tc() )) {
      *error = nm_vehicle_exists (dest_veh);
    } else {
      *error = AM_INAUG_ERR;
    }
    my_veh = md_get_my_vehicle ();
    if (!((*dest_veh != my_veh) && (cond == PR_VB))) {
    /* EH 04-Aug-1994, condition added */
      pack->port.lk.origin_vehicle = phys_bit | my_veh;
    }
    if (*dest_veh == my_veh) {
      *dest_veh = AM_SAME_VEHICLE;
      if ((*error == AM_OK) /* condition added EH 18-Jul-1994, was a bug */
          && (cond != PR_TB)) { /* EH 04-Aug-1994, condition added */
	pack->port.lk.final_vehicle = phys_bit | md_get_my_tc();
      }
    }
  } /* if (*dest_veh == AM_SAME_VEHICLE) */
} /* check_hdr */
#endif
/**********************************************************************/
#ifndef NM_1TB
static POLL_RESULT v_get_pack (MD_PACKET **pack,
                               unsigned char *src_dev, 
                               unsigned char *dest_dev,
                               AM_RESULT *error)
/* Polls for packets that have been reveived from a vehicle bus.
   Writes the module variable 'rcv'.
   The bus number of the polled VB is 'vb.bus_nr[poll.ix]'.
   Checks whether the packet is directed to myself (result PACK_CONSUME) 
   or to some other device (result PACK_TRANSIT, use 'rcv.out_bus_nr').
   Returns PACK_NONE if there is no more packet in the queue, the other 
   out parameters are not valid in this case.
   Returns PACK_CONSUME in case of any error (*error != AM_OK).
   Updates the STADI for physically addressed packets.
   Specials:
   - if the own station is unknown, then no packets are neither forwarded 
     nor consumed (no communication at all).
   - if there is more than one VB and no TB, then the directory entry for
     the router function is used to distinguish direction from/to TB
     (packets come from the TB <--> router is the source device).
   - if there is just one VB and no TB, then the function directory is 
     not consulted for logical addressing.
   - if there is more than one bus and the own station is known, then an
     unknown final station means myself (with logical addressing).
   - if the own station is 0, then any physical address is consumed.
     Logical addressing is only supported if there is just one VB and 
     no TB (end-node, a router in operation cannot be station 0).
*/
{
int           status;
unsigned char src_next, my_dev;
unsigned char discard, to_own; /* boolean */
POLL_RESULT   result;
#if (!NM_SIMPLE || !NM_VB_END)
unsigned char local; /* boolean */
#endif
#if !NM_NO_TB
unsigned char dest_veh;
#endif
#if ((!NM_VB_END && !NM_HAS_TB) || !NM_SIMPLE)
unsigned char router_dev;
#endif
#if !NM_SIMPLE
AM_ST_ENTRY   st_entry;
#endif

  /* loop while there are packets to be discarded: */
  for (discard = 0;; discard = !0) {
    if (discard) {
      mm_pool_put_pack (*pack);
    }
    nm_link_fcts[vb.bus_nr[poll.ix]].lm_receive_poll 
      (vb.bus_nr[poll.ix], &src_next, dest_dev, pack, &status);
    if (status == MD_OK) {
      my_dev = md_get_my_device ();
      if (my_dev == AM_UNKNOWN_DEVICE) {
	continue; /* discard packet */
      }
#ifdef DISCARD
      if (random(DISCARD_RANGE) < DISCARD_LEVEL)
      {
	 rejected_frames++;
	 continue; /* discard packet */
      }
      else
      {
	 accepted_frames++;
      }
#endif
      *src_dev = src_next; /* source device = next hop */
      rcv.phys_bit = (*pack)->port.lk.final_vehicle & AM_PHYS_ADDR;
#if (!NM_SIMPLE || !NM_VB_END)
      local = (((*pack)->port.lk.final_vehicle & ~AM_PHYS_ADDR)
	       == AM_SAME_VEHICLE) &&
	      (((*pack)->port.lk.origin_vehicle & ~AM_PHYS_ADDR)
	       == AM_SAME_VEHICLE);
#endif
      if (rcv.phys_bit) {
#if NM_SIMPLE  /* 15.09.95 RF */
	if (src_next == AM_UNKNOWN_DEVICE)
#endif
#if !NM_SIMPLE
          /* Update STADI for phys. addr.: */
	  st_entry.final_station = (*pack)->port.lk.origin_fct_or_dev;
          /* EH, 04-Aug-1994: same or unknown is now possible! */
          if ((st_entry.final_station == AM_UNKNOWN_DEVICE) ||
              (st_entry.final_station == AM_SAME_DEVICE)) {
            continue; /* discard packet */
          }
          if (local || bus_cfg.has_tb) {
          } else if (bus_cfg.one_vb) {
            st_entry.final_station = NM_ST_TB_STATION;
          } else {
            am_get_dir_entry (AM_ROUTER_FCT, &router_dev);
            if (router_dev == AM_UNKNOWN_DEVICE) {
              continue; /* discard packet */
            } else if (nm_st_get_bus_nr (&router_dev) == AM_UNKNOWN_BUS_NR) {
              continue; /* discard packet */
            }
          } 
          st_entry.next_station = src_next;
          st_entry.bus_id = vb.bus_nr[poll.ix];
          st_entry.link_address =
	    nm_link_fcts[vb.bus_nr[poll.ix]].lm_get_src_phya (*pack,&status);
	  if (am_st_write (&st_entry, 1) != AM_OK)
#endif
	    continue; /* discard packet */
	} else { /* logical addressing */
        if (src_next == AM_UNKNOWN_DEVICE) {
          continue; /* discard packet */
        }
      } /* if (rcv.phys_bit) */

      result = PACK_CONSUME;
      *error = AM_OK;
#if !NM_VB_END
      /* check whether it is a transit packet, if yes set rcv.out_bus_nr: */
      /* check whether the packet must be forwarded to the TB: */
      if (bus_cfg.one_vb || local || (my_dev == AM_SAME_DEVICE)) {
#if !NM_NO_TB
      } else if (bus_cfg.has_tb) {  
        check_hdr (*pack, &dest_veh, error, PR_NONE);
        if (*error == AM_FAILURE) {
          continue; /* discard packet */
        } else if (*error != AM_OK) {
          break;
        } else if (dest_veh != AM_SAME_VEHICLE) {
          result = PACK_TRANSIT;
          *dest_dev = dest_veh;
          rcv.out_bus_nr = AM_TB_NR;
          break;
        }
#endif
#if !NM_HAS_TB
      } else {
        am_get_dir_entry (AM_ROUTER_FCT, &router_dev);
        if (router_dev == AM_UNKNOWN_DEVICE) {
          continue; /* discard packet */
        } else {
          /* EH 04-Aug-1994: dir entry may be a non-next station,
             replace router_dev by its next station before comparison */
#if !NM_SIMPLE
          rcv.out_bus_nr = nm_st_get_bus_nr (&router_dev);
#else
          rcv.out_bus_nr = AM_MY_VB_NR;
#endif
          if (src_next != router_dev) {
            result = PACK_TRANSIT;
            *dest_dev = router_dev;
            break;
          } /* if (src_next != router_dev) */
        } /* if (router_dev == AM_UNKNOWN_DEVICE) */
#endif
      } /* if (bus_cfg.one_vb || local) */
#endif
      /* result is PACK_CONSUME. */
      /* packet is for my vehicle, check whether it is for my device: */
      /* ext. EH 04-Aug-1994, consume packets to same or unknown station */
      to_own = (my_dev == AM_SAME_DEVICE) ||
	       (my_dev == (*pack)->port.lk.final_fct_or_dev) ||
	       (AM_SAME_DEVICE == (*pack)->port.lk.final_fct_or_dev) ||
	       (AM_UNKNOWN_DEVICE == (*pack)->port.lk.final_fct_or_dev);
/* added EH 21-Jun-1994, TB gateway listens to station 0 from VB: */
/* removed EH 04-Aug-1994, all stations listen to 0 and unknown 
#if !NM_NO_TB  
      if (bus_cfg.has_tb) {
        to_own = to_own || 
		 ((*pack)->port.lk.final_fct_or_dev == AM_SAME_DEVICE);
      }
#endif
*/
#if !NM_NOT_1VB
      if (bus_cfg.one_vb) {
        if (rcv.phys_bit && !to_own) {
          *error = AM_UNKNOWN_DEST_ERR;
        }
      } else 
#endif
      {
#if !NM_VB_END
        if (rcv.phys_bit) {
          if (!to_own) {
            result = PACK_TRANSIT;
	    *dest_dev = (*pack)->port.lk.final_fct_or_dev;
#if !NM_SIMPLE
            rcv.out_bus_nr = nm_st_get_bus_nr (dest_dev);
#else
            rcv.out_bus_nr = AM_MY_VB_NR;
#endif
          }
        } else if (my_dev == AM_SAME_DEVICE) {
          continue; /* discard packet */
        } else { /* logical addressing, use function directory: */
	  am_get_dir_entry ((*pack)->port.lk.final_fct_or_dev, dest_dev);
          if ((*dest_dev == my_dev) || 
              (*dest_dev == AM_UNKNOWN_DEVICE) ||
              (*dest_dev == AM_SAME_DEVICE)) {
            *dest_dev = AM_SAME_DEVICE;
          } else {
            result = PACK_TRANSIT;
#if !NM_SIMPLE
            rcv.out_bus_nr = nm_st_get_bus_nr (dest_dev);
#else
            rcv.out_bus_nr = AM_MY_VB_NR;
#endif
          }
        } /* if (rcv.phys_bit) */
#endif
      } /* if (bus_cfg.one_vb) */
    } else {
      result = PACK_NONE;
    } /* status == MD_OK) */
    break;
  } /* for */
#if !NM_END
  if ((result == PACK_TRANSIT) && !rcv.phys_bit) {
    am_get_dir_entry ((*pack)->port.lk.origin_fct_or_dev, &src_next);
/* added EH 04-Aug-1994, function directory may map to non-next station: */
#if !NM_SIMPLE
    src_next = nm_st_get_next (src_next);    
#endif
    if (src_next != *src_dev) {
      result = PACK_CONSUME;
      *error = AM_GW_SRC_NOT_REG_ERR;
    }
  } /* if ((result == PACK_TRANSIT) && !rcv.phys_bit) */
#endif
#if !NM_NO_TB
/* <ATR:01> */
  if (result != PACK_NONE) {
  if (*error != AM_OK) {
    /* allow to send back the packet without checking/changing topo count */
/* <ATR:01> */
    if (*pack) (*pack)->port.lk.origin_vehicle = rcv.phys_bit | AM_SAME_VEHICLE;
  } /* if (*error != AM_OK) */
  }
#endif
  return result;
} /* v_get_pack */
#endif
/**********************************************************************/
#if !NM_NO_TB
static POLL_RESULT t_get_pack (MD_PACKET **pack,
                               unsigned char *src_dev, 
                               unsigned char *dest_dev,
                               AM_RESULT *error)
/* Polls for packets that have been reveived from the train bus.
   Writes the module variable 'rcv'.
   Discards packets that are addressed to a group of which my vehicle is
   not a member, a next packet is polled in this case.
   Checks whether the packet is directed to myself (result PACK_CONSUME) 
   or to some other device (result PACK_TRANSIT, use 'rcv.out_bus_nr').
   Returns PACK_NONE if there is no more packet in the queue, the other 
   out parameters are not valid in this case.
   My own device is returned as '*src_dev' for all packets.
   Specials:
   - if the own station is unknown, then no packets are neither forwarded 
     nor consumed (no communication at all).
   - replaces the final vehicle field by the inauguration counter.
   - final station 0 can be used to physically address a TB station.
   - if there is just a TB and no VB, then physical addressing is only 
     accepted for the own device (or 0), even if the own device is 0;
     the function directory is not consulted for logical addressing.
   - if there is also some VB and the own station is 0, then logical
     addressing is not supported, and any physical address is consumed.
   - if there is also some VB and the own station is known, then an
     unknown final station means myself (with logical addressing).
*/
{
int           status;
unsigned char dest_veh;
unsigned char discard, to_own; /* boolean */
POLL_RESULT   result;

  /* loop while there are packets to be discarded: */
  for (discard = 0;; discard = !0) {
    if (discard) {
      mm_pool_put_pack (*pack);
    }
    nm_link_fcts[AM_TB_NR].lm_receive_poll 
      (AM_TB_NR,&dest_veh, &dest_veh, pack, &status);
    if (status == MD_OK) {
      *src_dev = md_get_my_device ();
      if (*src_dev == AM_UNKNOWN_DEVICE) {
        continue; /* discard packet */
      }
#ifdef DISCARD
      if (random(DISCARD_RANGE) < DISCARD_LEVEL)
      {
	 rejected_frames++;
	 continue; /* discard packet */
      }
      else
      {
	 accepted_frames++;
      }
#endif
      dest_veh = (*pack)->port.lk.final_vehicle;
      if (dest_veh & AM_GROUP_BIT) {
        if (!am_is_member_of (dest_veh & ~AM_PHYS_ADDR)) { /* discard */
          continue;
        }
      } /* if (dest_veh & AM_GROUP_BIT) */
      (*pack)->port.lk.final_vehicle
        = md_get_my_tc() | (dest_veh & AM_PHYS_ADDR);
      *error = AM_OK;
      result = PACK_CONSUME;
      rcv.phys_bit = dest_veh & AM_PHYS_ADDR;
      *dest_dev = (*pack)->port.lk.final_fct_or_dev;
      if (!rcv.phys_bit) {
        am_get_dir_entry (*dest_dev, dest_dev);
      }
      to_own = ((*dest_dev == AM_SAME_DEVICE) ||
                (*dest_dev == AM_UNKNOWN_DEVICE) ||
                (*dest_dev == *src_dev));
#if !NM_NOT_1TB
      if (bus_cfg.one_tb) {
        if (rcv.phys_bit && !to_own) {
          *error = AM_UNKNOWN_DEST_ERR;                  
        }
#endif
#ifndef NM_1TB
#if !NM_NOT_1TB
      } else 
#endif
      if (*src_dev == AM_SAME_DEVICE) {
        if (!rcv.phys_bit) {
          *error = AM_UNKNOWN_DEST_ERR;
        }
      } else if (!to_own) {
        result = PACK_TRANSIT;
#if !NM_SIMPLE
        rcv.out_bus_nr = nm_st_get_bus_nr (dest_dev);
#else
        rcv.out_bus_nr = AM_MY_VB_NR;
#endif
#endif
      } /* if (bus_cfg.one_tb) */
    } else {
      result = PACK_NONE;
    } /* if (status != MD_OK) */
    break;
  } /* for (;;) */
  return result;
} /* t_get_pack */
#endif
/**********************************************************************/
#if !NM_SIMPLE
static AM_RESULT check_st_entry (AM_ST_ENTRY *new)
/* Checks whether a new entry is allowed to be written into the STADI.
   May change the own station address if it was AM_SAME_DEVICE before,
   or write an additional entry with station = next station into the
   STADI (on success only).
   The following dependencies are checked:
   - The bus number of the new entry must either be unknown, or a connected
     vehicle bus, or AM_MY_VB_NR if there is not more than one VB.
   - The bus number must be known if the station is a next station.
   - The own station cannot be the next station for some other station.
   - If it is an entry for an own physical address, then the station must be
     or must change the own station (ref. comments in module LM_PARAM.H).
*/
{
int status;
unsigned char bus_nr, ix;
AM_RESULT     error = AM_FAILURE;
unsigned char st;

  bus_nr = new->bus_id;
  st = md_get_my_device();
  if (bus_nr == AM_UNKNOWN_BUS_NR) {
    if (new->next_station != new->final_station) {
      if (new->next_station == AM_UNKNOWN_DEVICE) {
        error = AM_OK;
      } else if (new->next_station == AM_SAME_DEVICE) {
        if (st == AM_SAME_DEVICE) {
          md_set_my_device (new->final_station);
          error = AM_OK;
        } else if ((st == new->final_station) && (st != AM_UNKNOWN_DEVICE)) {
          error = AM_OK;
        } /* if (st == AM_SAME_DEVICE) */
      } else if (new->final_station == AM_SAME_DEVICE) {
        if (st == AM_SAME_DEVICE) {
          md_set_my_device (new->next_station);
          error = AM_OK;
        } else if (st == new->next_station) {
          error = AM_OK;
        } /* if (st == AM_SAME_DEVICE) */
      } else if (new->next_station != st) {
        error = AM_OK;
      } /* if (new->next_station == AM_UNKNOWN_DEVICE) */
    } else if (new->final_station == AM_SAME_DEVICE) {
      error = AM_OK;
    } /* if ((new->next_station != new->final_station) */
  } else {
    if (bus_nr == AM_MY_VB_NR) {
      if (vb.nr_of_busses == 1) {
        error = AM_OK;
        bus_nr = vb.bus_nr[0];
      }
    } else {
      for (ix = 0; ix < vb.nr_of_busses; ix++) {
        if (vb.bus_nr[ix] == bus_nr) {
          error = AM_OK;
        }
      } /* for */
    } /* if (bus_nr == AM_MY_VB_NR) */
    if (error == AM_OK) { /* bus_nr is a connected vehicle bus */
      if (nm_link_fcts[bus_nr].lm_get_own_phya (bus_nr,&status) == new->link_address) {
	if (st == AM_UNKNOWN_DEVICE) {
	  if ((new->final_station != AM_SAME_DEVICE) ||
	      (new->next_station != AM_SAME_DEVICE)) {
	    error = AM_FAILURE;
	  }
	} else if ((new->next_station != new->final_station) &&
		   (new->next_station != AM_SAME_DEVICE) &&
		   (new->next_station != AM_UNKNOWN_DEVICE)) {
	  error = AM_FAILURE;
	} else if (st == AM_SAME_DEVICE) {
	  if (new->final_station != AM_SAME_DEVICE) {
	    md_set_my_device (new->final_station);
	  } else if ((new->next_station != AM_SAME_DEVICE) &&
		     (new->next_station != AM_UNKNOWN_DEVICE)) {
	    md_set_my_device (new->next_station);
	  } /* if (new->final_station != AM_SAME_DEVICE) */
	} else if ((st != new->final_station) && (new->final_station != AM_SAME_DEVICE)) {
	  error = AM_FAILURE;
	} /* if (st == AM_UNKNOWN_DEVICE) */
      } /* if (nm_link_fcts[... */
      if ((new->final_station != new->next_station) &&
          (new->next_station != AM_UNKNOWN_DEVICE) &&
          (error == AM_OK)) {
        st = new->final_station;
        new->final_station = new->next_station;
        hamster_halt (am_st_write (new, 1) != AM_OK);
        new->final_station = st;
      } /* if ((new->final_station != new->next_station) && */
    } /* if (error == AM_OK) */
  } /* if (bus_nr == AM_UNKNOWN_BUS_NR) */
  return error;
} /* check_st_entry */
#endif
/**********************************************************************/

int nm_rcv_pack (MD_PACKET ** pack /* out */,
                 unsigned int * size /* out */,
                 /* size of link data unit without size field */
		 TM_LINK_ID * src /* out */,
		 TM_LINK_ID * dest /* out */,
                 AM_RESULT * error /* out */ )
/* Polls for received packets (from the train bus or any vehicle bus).
   Only packets addressed to the own device or erroneous packets are
   returned. Packets that must be routed are forwarded internally and
   a next packet is polled.
   The function result returns the number of treated packets, which is
   0 if MAX_POLL_PACK packets are forwarded or if there are no more packets.
   With result 0 all other output parameters are not valid.
   Polling strategy: polls up to MAX_STAY_PACK packets from the same bus,
   then steps to the next bus (VB or TB) in the list 'vb.bus_nr[]'.
   Maintains the necessary poll info in the module variable 'poll'.
*/
{
unsigned short  nr_of_pack;  /* number of packets polled within one call */ 
POLL_RESULT     result /* <ATR:03> */ = PACK_NONE;
unsigned char   select_next_bus; /* boolean */
#if !NM_END
int             status;
#endif
#if !NM_NO_TB
short           pi_result;
#endif

#ifdef TRACE_ENB
  if (get_binary (3) != last_binary)
  {
    if (last_binary)
    {
      nm_status_ind (MD_OK);
    }
    else
    {
      nm_status_ind (MD_INAUGURATION);
    }
    last_binary = !last_binary;  
  }
#endif
#ifndef NM_1TB
  if (bus_sv_control.requ) 
  { /* do bus timeout supervision for all VBs */
    do_bus_sv ();
  }
#endif
#if !NM_NO_TB
  pi_pend_semaphore (inaug_mutex, PI_FOREVER, &pi_result);
#endif
  for (nr_of_pack = 0;;) {
    /* poll the currently selected bus for received packets: */
    if (poll.ix < vb.nr_of_busses) 
    {
#ifndef NM_1TB
      result = v_get_pack (pack, &src->link_dev, &dest->link_dev, error);
#endif
    } else {
#if !NM_NO_TB
      result = t_get_pack (pack, &src->link_dev, &dest->link_dev, error);
#endif
    }
    /* update variable 'poll', select the next bus to be polled.
       Return with function result 0 if all queues are empty. */
    select_next_bus = (result == PACK_NONE);
    if (!select_next_bus) {
      nr_of_pack++;
      poll.non_empty_ix = poll.ix;
      poll.now_empty = 0;
#if !NM_END
      select_next_bus = (++poll.stay_cnt >= MAX_STAY_PACK);
#endif
    } else if (poll.ix == poll.non_empty_ix) {
      poll.now_empty = !0;
    } /* if (!select_next_bus) */
    if (select_next_bus) {
#if !NM_END
      poll.stay_cnt = 0;
      if (++poll.ix >= vb.nr_of_busses) {
        if (!bus_cfg.has_tb || (poll.ix > vb.nr_of_busses)) {
          poll.ix = 0;
        }
      }
#endif
      if (poll.now_empty && (poll.ix == poll.non_empty_ix)) {
        /* result always is PACK_NONE, all bus queues are empty */
        nr_of_pack = 0;
        break;
      }
    } /* if (select_next_bus) */
    /* treat the received packet if any: */
    if (result == PACK_CONSUME) {
      /* the packet is for my device (or error), pass it to the caller: */
      *size = (*pack)->port.lk.size;
      src->vehicle     = (*pack)->port.lk.origin_vehicle;
      src->fct_or_dev  = (*pack)->port.lk.origin_fct_or_dev;
      dest->vehicle    = (*pack)->port.lk.final_vehicle;
      dest->fct_or_dev = (*pack)->port.lk.final_fct_or_dev;
      break;
#if !NM_END
    } else if (result == PACK_TRANSIT) {
      /* forward the packet to bus 'rcv.out_bus_nr' (may be unknown): 
         define the packet's priority, exchange the packet with a packet
         from the transit pool of the outgoing bus, and send the packet.
      */
      if (rcv.out_bus_nr == AM_UNKNOWN_BUS_NR) {
        mm_pool_put_pack (*pack); /* discard the packet */
      } else {
        if (rcv.out_bus_nr == AM_MY_VB_NR) {
          rcv.out_bus_nr = vb.bus_nr[0];
        }
        if (rcv.phys_bit) {
          (*pack)->control |= MD_HIGH_PRIO;
        } else {
          (*pack)->control &= ~MD_HIGH_PRIO;
        } /* if (rcv.phys_bit) */
        mm_pool_exchange (&transit_pool[rcv.out_bus_nr].id, pack);
        if (*pack != NULL) {
          bus_sv[rcv.out_bus_nr].sent_more = !0;
          nm_link_fcts[rcv.out_bus_nr].lm_send_request 
	    (rcv.out_bus_nr, AM_SAME_DEVICE, dest->link_dev, *pack, &status);
	  if (status != MD_OK) {
	    mm_pool_put_pack (*pack);
	  }
        } else { /* transit pool empty */
          hamster_continue (!0);
        } /* if (*pack != NULL) */
      } /* if (rcv.out_bus_nr == AM_UNKNOWN_BUS) */
#endif
    } /* if (result == PACK_CONSUME) */
    /* treat only a limited number of transit packets with each call */
    if (nr_of_pack >= MAX_POLLED_PACK) { 
      nr_of_pack = 0;
      break;
    }
  } /* for */
#if !NM_NO_TB
  pi_post_semaphore (inaug_mutex, &pi_result);
#endif
  return nr_of_pack;  
} /* nm_rcv_pack */

/**********************************************************************/

void nm_send_pack (MD_PACKET * pack,
                   unsigned int size,
                   /* size of link data unit without size field */
		   const TM_LINK_ID * src,
		   const TM_LINK_ID * dest,
		   AM_RESULT * error /* out */ )
/* Determines the outgoing bus and sends a packet.
   Fills the network header, size and priority fields into the packet.
   'src->link_dev' is ignored (replaced by own device).
   - Checks the network addresses if there is a TB connected.
   - If more than one bus is connected, then the source function must
     not be registered for some device != own with logical addressing.
   - If more than one bus is connected and the own station address is
     0, then logical addressing is not supported.
   - only packets to (dest->link_dev == my_device) are sent to the TB.
   - errors from a link layer are not passed through (EH 04-Aug-1994)
*/
{
int             status;
unsigned char   dev;
unsigned char   bus_nr;
unsigned char   is_phys; /* boolean */
unsigned char   dest_veh;
#if !NM_NO_TB
short           pi_result;
#endif

  if (pack != NULL)
  {
    pack->port.lk.size = (unsigned char) size;
    pack->port.lk.final_vehicle     = dest->vehicle;
    pack->port.lk.final_fct_or_dev  = dest->fct_or_dev;
    pack->port.lk.origin_vehicle    = src->vehicle;
    pack->port.lk.origin_fct_or_dev = src->fct_or_dev;
    is_phys = dest->vehicle & AM_PHYS_ADDR;
    if (is_phys)
    {
      pack->control |= MD_HIGH_PRIO;
      pack->port.lk.origin_vehicle |= AM_PHYS_ADDR;
    }
    else
    {
      pack->control &= ~MD_HIGH_PRIO;
      pack->port.lk.origin_vehicle &= ~AM_PHYS_ADDR;
    } /* if */
#if !NM_NO_TB
    pi_pend_semaphore (inaug_mutex, PI_FOREVER, &pi_result);
#endif
    do { /* no real loop, just allows break on error */
      dev = dest->link_dev;
#if !NM_SIMPLE
      /* EH 04-Aug-1994, require next station also for phys. addr. */
      if (is_phys && (dev == AM_UNKNOWN_DEVICE)) {
	if (dest->vehicle == (AM_PHYS_ADDR | AM_SAME_VEHICLE)) {
	  dev = dest->fct_or_dev;
        }
      } else if (dev == md_get_my_device()) {
      /* EH 04-Aug-1994, do not require Stadi entry for own device */
      } else if (dev == AM_SAME_DEVICE) {
      /* EH 04-Aug-1994, my device may have changed with Stadi entry! */
        dev = md_get_my_device(); 
      } else if (nm_st_get_next (dev) != dev) {
        *error = AM_ADDR_FMT_ERR;
        break;
      }
#endif
      /* added EH 04-Aug-1994, assign 'dev' and 'bus_nr'.
         'dev' may change later after 'check_hdr' (for groups).
         use new criteria for "out to TB".
      */
      if (dev == md_get_my_device ()) {
	dest_veh = pack->port.lk.final_vehicle & ~AM_PHYS_ADDR;
	if (dest_veh != AM_SAME_VEHICLE) {
	  if (bus_cfg.has_tb) {
	    dev = dest_veh;
	    bus_nr = AM_TB_NR;
	  } else {
            *error = AM_ADDR_FMT_ERR;
            break;
          } /* if (bus_cfg.has_tb) */
        } else {
          if (bus_cfg.one_tb) {
            *error = AM_UNKNOWN_DEST_ERR;
            break;
          } else {
            bus_nr = vb.bus_nr[0]; /* use this VB to send to own device */
          } /* if (bus_cfg.one_tb) */
        } /* if (dest_veh != AM_SAME_VEHICLE) */
#if !NM_NOT_1TB
      } else if (bus_cfg.one_tb) {
        *error = AM_UNKNOWN_DEST_ERR;
        break;
#endif
      } else {
#if !NM_SIMPLE
        bus_nr = nm_st_get_bus_nr (&dev);
#else
        bus_nr = vb.bus_nr[0];
#endif
      } /* if (dev == md_get_my_device ()) */
#if !NM_NOT_1VB
      if (bus_cfg.one_vb) 
      {
/* EH 04-Aug-1994, moved up
#if !NM_SIMPLE
        bus_nr = nm_st_get_bus_nr (&dev);
#else
        bus_nr = vb.bus_nr[0];
#endif
*/
      } else 
#endif
      {
#if !NM_VB_END
#if !NM_NO_TB
        if (bus_cfg.has_tb) {
          check_hdr (pack, &dest_veh, error, 
                     (bus_nr == AM_TB_NR) ? PR_TB : PR_VB);
          if ((bus_nr == AM_TB_NR) && (dest_veh != AM_SAME_VEHICLE))
          { /* added EH 04-Aug-1994 */
            dev = dest_veh;
          }
          if ((*error != AM_OK) && (*error != AM_FAILURE)) {
            break;
          }
        } else 
#endif
        {
/* deleted EH 04-Aug-1994
#if !NM_HAS_TB
	  dest_veh = pack->port.lk.final_vehicle & ~AM_PHYS_ADDR;
#endif
*/
        } /* if (bus_cfg.has_tb) */
#if !NM_NOT_1TB
        if (bus_cfg.one_tb) {
/* deleted EH 04-Aug-1994
          bus_nr = AM_TB_NR;
          dev = dest_veh;
*/
        } else 
#endif
        {       
#ifndef NM_1TB
          /* there is more than one bus connected */
          if (!is_phys) {
            if (md_get_my_device() == AM_SAME_DEVICE) {
              *error = AM_MY_DEV_UNKNOWN_ERR;
              break;
            }
            if (*error != AM_FAILURE) 
            {
            unsigned char tmp_dev;     /* Variable added EH 04-Aug-1994 */
              am_get_dir_entry (pack->port.lk.origin_fct_or_dev, &tmp_dev);
              if ((tmp_dev != AM_SAME_DEVICE) && 
                  (tmp_dev != AM_UNKNOWN_DEVICE) &&
                  (tmp_dev != md_get_my_device())) {
                *error = AM_GW_SRC_NOT_REG_ERR;
                break;
              }
            } 
          } /* if (!is_phys) */
/* EH 04-Aug-1994, deleted due to new criteria for "out to TB"!
          if (dest_veh == AM_SAME_VEHICLE) {
            dev = dest->link_dev;
#if !NM_SIMPLE
            bus_nr = nm_st_get_bus_nr (&dev);
#else
            bus_nr = vb.bus_nr[0];
#endif
#if !NM_NO_TB
          } else if (bus_cfg.has_tb) {
            bus_nr = AM_TB_NR;
            dev = dest_veh;
#endif
#if !NM_HAS_TB
          } else {
            am_get_dir_entry (AM_ROUTER_FCT, &dev);
#if !NM_SIMPLE
            bus_nr = nm_st_get_bus_nr (&dev);
#else
            bus_nr = AM_MY_VB_NR;
#endif
#endif
          } 
*/
#endif
        } /* if (bus_cfg.one_tb) */
#endif
      } /* if (bus_cfg.one_vb) */
#if !NM_SIMPLE
      if (bus_nr == AM_UNKNOWN_BUS_NR) {
        *error = AM_UNKNOWN_DEST_ERR;
        break;
      } else if (bus_nr == AM_MY_VB_NR) {
        bus_nr = vb.bus_nr[0];
      }
#endif
#ifndef NM_1TB
      bus_sv[bus_nr].sent_more = !0;
#endif
      nm_link_fcts[bus_nr].lm_send_request
	(bus_nr,(bus_nr == AM_TB_NR) ? AM_SAME_VEHICLE : AM_SAME_DEVICE,
         dev, pack, &status);
      /* EH 04-Aug-1994, do not pass through errors from a link layer
         because a retry may be successful:
      if (status == MD_OK) {
        *error = AM_OK;
      } else {
        *error = AM_FAILURE;
      }
      */
      *error = AM_OK;
      if (status != MD_OK) {
        mm_pool_put_pack (pack); 
      }
    } while (0);
#if !NM_NO_TB
    pi_post_semaphore (inaug_mutex, &pi_result);
#endif
    if (*error != AM_OK) {
      mm_pool_put_pack (pack); 
    }
  } else {
    *error = AM_FAILURE;
  } /* if (pack != NULL) */
} /* nm_send_pack */

/**********************************************************************/
#if !NM_NO_TB
void t_status_ind (int reason)
/* treat TB link layer exception indications. Forward to NM_MANAG module.
   reason == MD_OK: means TB is running, inauguration completed.
     Action: Increment the inauguration counter.
   reason != MD_OK: means TB is down.
   Do in both cases flush the TB send and receive queues.
*/ 
{
unsigned char  dummy_veh;
MD_PACKET     *dummy_pack;   
int            status;
short          pi_result;
  
  pi_pend_semaphore (inaug_mutex, PI_FOREVER, &pi_result);

  if (nm_abort_TB_conn != (void(*)(void)) 0 )
  {
    nm_abort_TB_conn();                   /* RF 160695 */
  }

  nm_link_fcts[AM_TB_NR].lm_send_queue_flush (AM_TB_NR);
  do {
    nm_link_fcts[AM_TB_NR].lm_receive_poll 
      (AM_TB_NR,&dummy_veh, &dummy_veh, &dummy_pack, &status);
    if (status == MD_OK) {  /* added 10-Feb-1994, EH */
      mm_pool_put_pack (dummy_pack);
    } else {
      break;
    }
  } while (!0);
  nm_status_ind (reason);
  pi_post_semaphore (inaug_mutex, &pi_result);
} /* t_status_ind */
#endif
/**********************************************************************/
#ifndef NM_1TB
void v_status_ind (int reason)
/* VB link layer exception indications are not used. */
{
  hamster_continue (reason != MD_READY);  
  /* 24-Feb-1994, EH: was hamster_halt */
} /* v_status_ind */
#endif
/**********************************************************************/

void nm_init (unsigned int nr_of_pack,void (*abort_TB_conn) (void))
/* Creates the receive pools, and also the transit pools if there is more
   than one link layer, for all connected link layers and init. these
   link layers. Init. bus timeout supervision for all connected vehicle
   busses and starts a timer for this purpose if there is any VB.
   The configured receive pool size is just a maximal size if there is just
   one bus connected. In this case the clipped value 'nr_of_pack' defines
   the real pool size.
*/
{
unsigned char bus_nr;
char ix;

  nm_abort_TB_conn = abort_TB_conn;
  no_pool = 0;
  ix = vb.nr_of_busses;
#if !NM_HAS_TB
  if (!bus_cfg.has_tb) ix--; /* vb.bus_nr[vb.nr_of_busses] = AM_TB_NR; */
#endif
  for (; ix >= 0; ix--) {
    bus_nr = vb.bus_nr[ix];
#if !NM_END
    nr_of_pack = nr_of_pack; /* suppress warning 'parameter not used' */
    if (!(bus_cfg.one_vb || bus_cfg.one_tb)) {
      mm_pool_create (transit_pool[bus_nr].size, sizeof (MD_PACKET), 0,
                      &transit_pool[bus_nr].id);
    } else 
#endif
    {
#if !NM_NOT_END
      if (nr_of_pack < link_rcv_pool[bus_nr].size) {
        link_rcv_pool[bus_nr].size = nr_of_pack;
      }
#endif
    } /* if (!bus_cfg.one_vb) || bus_cfg.one_tb)) */
    mm_pool_create (link_rcv_pool[bus_nr].size, sizeof (MD_PACKET), 0,
                    &link_rcv_pool[bus_nr].id);
    nm_link_fcts[bus_nr].lm_init (bus_nr, NULL, mm_pool_get_pack_from_ISR,
                    &link_rcv_pool[bus_nr].id,
	                mm_pool_put_pack_from_ISR,
#if NM_NO_TB
                    v_status_ind);
#elif defined (NM_1TB)
                    t_status_ind);
#else                                                               
                    (bus_nr == AM_TB_NR) ? t_status_ind : v_status_ind);
#endif
#ifndef NM_1TB
    bus_sv[bus_nr].sent_more = 0;
    bus_sv[bus_nr].pending = 0;
#endif
  } /* for */

#ifndef NM_1TB
  if (!bus_cfg.one_tb) {
    bus_sv_control.requ = 0;
    hamster_halt (pi_create_timeout (&bus_sv_control.timer, activate_bus_sv, 
                                     NULL, PI_FOREVER) != PI_RET_OK);
    pi_enable_timeout (bus_sv_control.timer, BUS_TMO);
  } /* if (!bus_cfg.one_tb) */
#endif
} /* nm_init */

/**********************************************************************/
#ifndef NM_1TB
void activate_bus_sv (void * param)
/* called from the external timeout handler */
{
  hamster_halt (param != NULL);
  bus_sv_control.requ = !0;
} /* activate_bus_sv */
#endif
/**********************************************************************/
#ifndef NM_1TB
void do_bus_sv (void)
/* Does the bus send timeout supervision which is necessary to limit
   the lifetime of a packet. Flushes the link send queue when either
   - a BUS_TMO interval has passed during which {no frames have been 
     transmitted and no send requests have been issued} or
   - a BUS_TMO interval has passed and {no frames have been transmitted
     but send requests have been issued} during the previous interval.
*/
{
unsigned short active;
unsigned char ix, bus_nr;
BUS_SV_STORE *p_bus_sv;

  for (ix = 0; ix < vb.nr_of_busses; ix++) {
    bus_nr = vb.bus_nr[ix];
    p_bus_sv = bus_sv + bus_nr;
    nm_link_fcts[bus_nr].lm_get_status 
      (bus_nr, MD_SEND_ACTIVE, MD_SEND_ACTIVE, &active);
    if (active)
    {
      p_bus_sv->pending = 0;
    }
    else
    {
      if((!p_bus_sv->sent_more) || p_bus_sv->pending)
      {
	nm_link_fcts[bus_nr].lm_send_queue_flush (bus_nr);
      }
      p_bus_sv->pending = p_bus_sv->sent_more;
    }   
    p_bus_sv->sent_more = 0;
  } /* for */
  bus_sv_control.requ = 0;
} /* do_bus_sv */
#endif
/**********************************************************************/

void nm_system_init (void)
{
unsigned short my_station;
unsigned short ix;
#if !NM_SIMPLE
unsigned short bus_nr;
int status;
AM_ST_ENTRY    st_entry;
#endif
#if !NM_NO_TB
short          pi_result;
#endif

  for (ix = 0; ix < AM_MAX_BUSSES; ix++) {
    link_rcv_pool[ix].size = RCV_POOL_DFT_SIZE;
#if !NM_END
    transit_pool [ix].size = TRANSIT_POOL_DFT_SIZE;
#endif
  } /* for */
  poll.ix = 0;
#if !NM_END
  poll.stay_cnt = 0;
#endif
  poll.non_empty_ix = 0;
  poll.now_empty = 0;
#if !NM_SIMPLE
  nm_st_system_init ();
  hamster_halt (nm_st_connect_check (check_st_entry) != AM_OK);
#endif
  nm_disp_system_init ();
  lm_system_init ();

  /* init. bus configuration: */
  am_disp_show_busses (&vb.nr_of_busses, vb.bus_nr);
  hamster_halt (vb.nr_of_busses == 0);
  bus_cfg.has_tb = (vb.bus_nr[vb.nr_of_busses - 1] == AM_TB_NR);
  if (bus_cfg.has_tb) {
    vb.nr_of_busses--;
  }
  bus_cfg.one_tb = (vb.nr_of_busses == 0);
  bus_cfg.one_vb = (vb.nr_of_busses == 1) && !bus_cfg.has_tb;

  /* init. own station and check its compatibility with the STADI: */
  lm_get_own_station (&my_station);
  if (my_station > AM_UNKNOWN_DEVICE)
  {
    my_station = AM_UNKNOWN_DEVICE;
  }
  if (bus_cfg.one_tb && (my_station == AM_UNKNOWN_DEVICE)) {
    my_station = AM_SAME_DEVICE;
  }
  md_set_my_device (my_station);

#if !NM_SIMPLE
  /* check the STADI entries for all own physical addresses and delete
     those entries which fail the check: 
  */
  for (ix = 0; ix < vb.nr_of_busses; ix++) {
    bus_nr = vb. bus_nr[ix];
    hamster_halt (nm_link_fcts[bus_nr].lm_get_own_phya == NULL);
    hamster_halt (nm_link_fcts[bus_nr].lm_get_src_phya == NULL);
    st_entry.final_station
      = nm_st_find_phya (nm_link_fcts[bus_nr].lm_get_own_phya (bus_nr,&status),bus_nr);
    if (st_entry.final_station != AM_UNKNOWN_DEVICE) {
      am_st_read (&st_entry, 1);
      if (check_st_entry (&st_entry) != AM_OK) { 
        /* delete entry from STADI: */
        st_entry.next_station = AM_UNKNOWN_DEVICE;
        st_entry.bus_id = AM_UNKNOWN_BUS_NR;
        hamster_halt (am_st_write (&st_entry, 1) != AM_OK);
      } /* if (check_st_entry (&st_entry) != AM_OK) */
    } /* if (st_entry.final_station != AM_UNKNOWN_DEVICE) */
  } /* for */
#endif

  md_set_my_veh_and_tc (AM_SAME_VEHICLE, AM_ANY_TC); /* EH 18-Jul-1994 */
  /* these values will never change if there is no TB connected. */
  no_pool = !0;
#if !NM_NO_TB
  nm_mng_system_init ();
  inaug_mutex = pi_create_semaphore (1, PI_ORD_FCFS, &pi_result);
  hamster_halt (pi_result != PI_RET_OK);
#endif
} /* nm_system_init */

#ifndef NM_1TB

/****************************************************************************
	 FUNCTION: nm_change_packet_lifetime

	 INPUT:    new_bus_tmo

	 OUTPUT:   -

	 RETURN:   -

	 ABSTRACT: The procedure changes the bus timeout supervision interval.
						 The default bus time out (BUS_TMO) is set during the
						 initialization of the network layer.
						 If a new timeout must be set, first the PI_FOREVER-timer is
						 stopped and then started with a new bus timeout value.
						 The bus timeout verify in each timeout interval
						 the status of the link layer (bus) and then, if a failure is
						 detected it flushes the send queue.

*****************************************************************************/

short nm_change_packet_lifetime(unsigned long new_bus_tmo)
{
	short change_tmo_error = !0;

	/* stop the bus timeout */
	if(pi_disable_timeout( bus_sv_control.timer) == PI_RET_OK)
	{
		/* start the cyclic timer with a new timeout */
		if (pi_enable_timeout( bus_sv_control.timer, new_bus_tmo) == PI_RET_OK)
		{
			change_tmo_error = 0;
		 }
	 } /*if*/
	return change_tmo_error;
}
#endif

