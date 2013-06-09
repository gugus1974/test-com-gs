/*************************************************************************
**        Copyright (C) Siemens AG 1997 All Rights Reserved. Confidential
**========================================================================
**
**    PROJECT::    TCN Train Communication Network
                   JDP Joint Development Project

**    MODULE::     UIC Mapping Server

**    WORKFILE::   $Workfile:   UTBC.H  $
**************************************************************************
**    TASK::       Definition of UIC Train Bus Configurator functions

**************************************************************************
**    AUTHOR::     Hilbrich ()
**    CREATED::    20.10.97
**    UPDATED::    19.01.98 ( Review in Winehome )
**========================================================================
**    HISTORY::    AUTHOR::   $Author: schiavor $
                   REVISION:: $Revision: 1.1.2.1 $
                   MODTIME::  $Date: 2006/12/01 10:41:46 $
      $Log: UTBC.H,v $
      Revision 1.1.2.1  2006/12/01 10:41:46  schiavor
      01/12/2006                         Napoli                            SchiavoR
      Primo inserimento per utilizzo libreria nel GTW

 *
 *    Rev 1.60   14 Dec 1998 16:47:24   HEINDEL_AR
 * process_unknown_nodes() signals overflow
 *
 *    Rev 1.59   14 Dec 1998 10:12:26   HILBRICH_KL
 * addditional parameter 'rno' in fct. 'invalidate_entry()'
 *
 *    Rev 1.58   12 Nov 1998 12:23:40   HILBRICH_KL
 * read ld. direc from IDTS, if own veh. is leading
 *
 *    Rev 1.57   09 Nov 1998 15:36:30   HILBRICH_KL
 * update kft-bitfield also, if own node is not included
 *
 *    Rev 1.56   06 Nov 1998 16:52:40   HILBRICH_KL
 * swap NADI entries, if single node was invers oriented to master before
 *
 *    Rev 1.55   06 Nov 1998 12:02:12   HILBRICH_KL
 * actualize entries for single node NADI
 *
 *    Rev 1.54   05 Nov 1998 17:08:50   HILBRICH_KL
 * single node mode as separate state
 *
 *    Rev 1.53   04 Nov 1998 16:50:48   HILBRICH_KL
 * sort entries of single node depending on orientation
 *
 *    Rev 1.52   04 Nov 1998 14:46:00   HILBRICH_KL
 * modifications in calculate_rnodir()
 *
 *    Rev 1.51   30 Oct 1998 12:20:02   HILBRICH_KL
 * bug fixed in single-node mode handling
 *
 *    Rev 1.50   29 Oct 1998 15:55:36   HILBRICH_KL
 * correction handling: do not store last tcn-topo
 *
 *    Rev 1.49   27 Oct 1998 12:41:48   HILBRICH_KL
 * single node mode-handling modified
 *
 *    Rev 1.48   14 Oct 1998 15:53:40   HILBRICH_KL
 * additional recorder entry added
 *
 *    Rev 1.47   07 Oct 1998 12:47:26   HILBRICH_KL
 * function 'adapt_indices()' removed
 *
 *    Rev 1.46   06 Oct 1998 10:03:44   HILBRICH_KL
 * additional recorder entry added
 *
 *    Rev 1.45   01 Oct 1998 09:00:16   HILBRICH_KL
 * bitmask for 'leading request bit' defined
 *
 *    Rev 1.44   29 Sep 1998 14:06:34   HILBRICH_KL
 * complete revision
 *
 *
**************************************************************************/
#ifndef UTBC_H_
#define UTBC_H_

#ifndef BMI_H_
#error include "bmi.h" first
#endif

#ifndef ADID_H_
#error include "adid.h" first
#endif


#if defined _DEBUG || defined _UTBC_TRACE

  #define UTBCTRACE( txt, fp ) fwrite( txt, sizeof( char ), strlen( txt ), fp ); \
                               fflush( fp )
  #define UTBCPRINT( txt, x, y ) sprintf( txt, x, y )

#else

  #define UTBCTRACE( txt, fp )
  #define UTBCPRINT( txt, x, y )

#endif


/* local defines for UTBC / macros. NOTE: byte-/bitoffsets begin with 0 */

#define VEH_EQ_GW_OFFSET_BYTE         0x04   /* offset byte in 'veh_info': veh. contains gateway */
#define VEH_EQ_GW_BITMASK             0x02   /* bit 1 in offset byte 'VEH_EQ_GW_OFFSET_BYTE' */
#define TRACTION_LEADER_BYTE          0x02   /* offset byte in 'ts_info': traction leader */
#define TRACTION_LEADER_BITMASKS      0x03   /* bit 0-1: traction leader properties */
#define CTRL_VEH_AP                   0x08   /* bit 3: correction telegram: veh. already present */
#define SRC_DEV_BITMASK               0x7F   /* Bit 0-6: src-device in 'i_data.src_dev' */
#define ORIENT_TO_TCN_MASTER          0x01   /* Bit 0 in 'veh_add_on' */
#define ORIENT_VEH_UICMAIN            0x02   /* Bit 1 in 'veh_add_on' */
#define LD_VEH_BITMASK                0x04   /* Bit 2 in 'veh_add_on' */
#define LD_REQ_BITMASK                0x08   /* Bit 3 in 'veh_add_on' */

#define ORIENT_MASTER_UICMAIN         0x01   /* Bit 0 in 'add_on_info' in NADI glob. descr. */
#define NODE_ACTIVE_BITMASK           0x02   /* Bit 1 in 'add_on_info' in NADI glob. descr. */
#define NODE_LOSS_BITMASK             0x04   /* Bit 2 in 'add_on_info' in NADI glob. descr. */

#define ORIENT_TO_TCN_MASTER_SAME     0x01
#define ORIENT_TO_TCN_MASTER_INVERS   0x00
#define ORIENT_VEH_UICMAIN_SAME       0x02
#define ORIENT_MASTER_UICMAIN_SAME    0x01

#define UIC_INDICATION_BITMASK        0x07  /* bits 0-3 of UIC_Ind */
#define UIC_INDICATION                0x01  /* UIC conform vehicle */

#define LEN_REPORT_DATA               0x06

#define NODE_ADDR_TCNMASTER           0x01
#define INVALID_INDEX                 0xFF
#define INVALID_VERSION               0xFF
#define INVALID_DIR                   0xFF

#define VEH_OR_TS_LEADING             0x01
#define VEH_OR_TS_NOT_LEADING         0x00

#define SINGLE_NODE_MODE              0x01
#define NODE_REINSERTION              0x02
#define SET_TEMP_INVALID              0x04
#define NEW_TOPO_RCVD                 0x08

#define MODE_TOP                      0x00
#define MODE_BOTTOM                   0x01

#define FROM_SAVED_RNO                0x01
#define FROM_LD_POS                   0x02

#define MULTI_GW_MASK                 0x80


/* process_correction(): max. 32 vehicles allowed */
#define MAX_CORR_VEH                  0x20

#ifdef O_KFT
#define LEN_TCI  0x08
#endif

#undef MIN
#undef MAX

#define MIN( p,q ) ((p)<(q)?(p):(q))
#define MAX( p,q ) ((p)>=(q)?(p):(q))
#define ABS_VAL( p,q ) ((p)>(q)?((p)-(q)):((q)-(p)))


#define ISNULL_UIC_ID( p ) ( ( (p)->uic_id[0] | (p)->uic_id[1] | (p)->uic_id[2] | \
                               (p)->uic_id[3] | (p)->uic_id[4] ) == 0 )

#define ISINVALID_UIC_ID( p ) ( ( (p)->uic_id[0] & (p)->uic_id[1] & (p)->uic_id[2] & \
                                  (p)->uic_id[3] & (p)->uic_id[4] ) == 0xFF )


/* macro to compare two UIC-IDs  */
#define UIC_ID_CMP( p, q ) ( memcmp( (TYPE_UIC_ID*)p, (TYPE_UIC_ID*)q, TFR_LEN_UIC_ID ) )


#define SETNULL_UIC_ID( p ) memset( p, 0, TFR_LEN_UIC_ID );
#define SETINVALID_UIC_ID( p ) memset( p, 0xFF, TFR_LEN_UIC_ID );


/* report function: called when UTBC finishs */
typedef int (* UTBC_REPORT)(void*);


/* range number directions */
#define RNO_DIR_INVALID         0    /* for single-node mode */
#define RNO_DIR_BOTTOM_TOP      1    /* range numbers from bottom to top */
#define RNO_DIR_TOP_BOTTOM      2    /* range numbers from top to bottom */

#define STATE_MASK   0x0E  /* unknown, set or saved */


/* events of the UTBC */
void utbc_trace_event( UNSIGNED8 u_event );
void utbc_print_event( UNSIGNED8 u_event, char *p_buf );

#define    EVENT_RED_IND                0x01
#define    EVENT_CORRECT                0x02
#define    EVENT_ALLOC_ERR              0x03
#define    EVENT_CORR_VEH_ZERO          0x04
#define    EVENT_OWN_ID_NOTFOUND_C      0x05
#define    EVENT_CONFIRM_CFG            0x06
#define    EVENT_RNODIR_NOT_CALC        0x07
#define    EVENT_RNODIR_CHANGED         0x08
#define    EVENT_TR_LD_EXISTS           0x09
#define    EVENT_NB_ND_GR_MAX           0x0A
#define    EVENT_VER_CONFLICT           0x0B
#define    EVENT_MAX_ENTR_ERR           0x0C
#define    EVENT_UMSCFG_INVALID         0x0D
#define    EVENT_OWN_ID_NOTFOUND_U      0x0E
#define    EVENT_CINFO_CORRUPTED        0x0F
#define    EVENT_REINSERTION            0x10
#define    EVENT_REDSWITCH              0x11
#define    EVENT_NODES_LOST             0x12
#define    EVENT_MND_LATE_INSERT        0x13
#define    EVENT_TOPO_ERR               0x14
#define    EVENT_LD_ID_NOTFOUND_C       0x15
#define    EVENT_NEW_INAUG              0x16
#define    EVENT_TRAIN_LTH              0x17
#define    EVENT_EQ_GWBIT_NOTSET        0x18
#define    EVENT_ZERO_CV                0x19



/* definition of different node-states  */
#define    ST_ACFG                  1   /* active NADI */
#define    ST_CONFIRM_ACFG          2   /* NADI is confirmed */
#define    ST_CORRECT_CFG           3   /* NADI is corrected */
#define    ST_SAVED_NADI            4   /* saved NADI is updated */
#define    ST_PROCESS_UNKNOWN_ND    5   /* check for insertion of unknown nodes */
#define    ST_SINGLE_ND_SET         6   /* single node mode corrected */
#define    ST_SINGLE_ND_SAV         7   /* single node mode saved */
#define    ST_INVALID               8   /* invalid state */

typedef UNSIGNED8  NodesState;

typedef struct {

    UNSIGNED8    nd_addr;
    UNSIGNED8    state;      /* unknown, set or saved */

} Node_and_State;

typedef struct {

    UNSIGNED8    rno;
    UNSIGNED8    ctrl_veh;

} Rno_and_CtrlVeh;

typedef struct {

    UNSIGNED8   rno;
    UNSIGNED8   nb_lost;
    UNSIGNED8   nb_gw;

} Lost_Rno_Data;


typedef struct {

    UNSIGNED8     nd_addr;
    BOOLEAN       intgr;        /* integrate with valid UIC address or not */
    UNSIGNED8     intgr_ndx;    /* index where to integrate */
    UNSIGNED8     intgr_nb;     /* number of entries to be integrated */
    UIC_VehDescr  *p_intgr;     /* pointer to active entry */

} Unknown_Node_Data;


typedef struct{

     UNSIGNED8      top_address;
     UNSIGNED8      bottom_address;
     UNSIGNED8      my_address;
     UNSIGNED8      topo_counter;
     UNSIGNED8      nb_nodes;
     UNSIGNED8      nb_entries;
     UNSIGNED8      nb_nodes_cu;
     UNSIGNED8      nb_nodes_saved;
     UNSIGNED8      nb_nodes_set;
     UNSIGNED8      nb_entries_top_cu;
     UNSIGNED8      nb_entries_bottom_cu;
     UNSIGNED8      nb_veh_saved;
     UNSIGNED8      del_cc;
     UNSIGNED8      red_switch;
     UNSIGNED8      red_addr;
     UNSIGNED8      uic_ind;
     UNSIGNED8      uic_ver;
     UNSIGNED8      uic_subver;
     UNSIGNED8      ct_veh;
     UNSIGNED8      ct_veh_cu;
     UNSIGNED8      ct_veh_cs;
     BOOLEAN        corr_info_corrupted;
     BOOLEAN        single_node_mode;
     BOOLEAN        node_reinsertion;
     BOOLEAN        new_topo_received;
     NodesState     my_state;

} UI_NODES_Info;



/* error codes of the TBC returned to UWTM */
typedef int UTBC_RESULT;


/* external interface of UTBC */
/*****************************/
UTBC_RESULT  utbc_global_request( void        *p_uic_callmsg,
                                  CARDINAL32  uic_callmsg_size,
                                  AM_ADDRESS  *caller,
                                  void        **p_replymsg,
                                  CARDINAL32  *p_uic_replymsg_size,
                                  REPLY_FREE  *p_reply_free_fct );
/*{ this procedure handles all global requests, i.e.
    E-telegrams to UTBC }*/





/* internal interface to the UWTM */
/**********************************/
UTBC_RESULT  utbc_init( ums_NodeConfigurationTable *p_cfg,
                        UTBC_REPORT  utbc_report );
/*{ this procedure initializes the UTBC; it must be the
    first call to UTBC }*/


UTBC_RESULT  utbc_build_nadi( UNSIGNED8 *p_status,
                              UNSIGNED8 *p_uic_addr,
                              UNSIGNED8 *p_leading,
                              UNSIGNED8 *p_direc,
                              BOOLEAN *p_ld_exists,
                              BOOLEAN single_node_mode );
/*{ this procedure calculates the NADI and is called
    directly from UWTM after UIC-inauguration }*/


UTBC_RESULT  utbc_red_indicate( void );
/*{ this procedure indicates a redundancy-switch }*/


UTBC_RESULT  utbc_set_leading_req( UNSIGNED8 set_leading, UNSIGNED8 direc );
/*{ sets/resets the leading request bit }*/



/* internal interface of the UTBC */
/**********************************/
void init_idata( TYPE_InaugData *p_idata );
/*{ this procedure read own inauguration data from configuration-table
    and builds up a big-endian byte stream to enter in ID-TS }*/


UTBC_RESULT read_config( UI_NODES_Info *p_NodesInfo,
                         BOOLEAN single_node_mode );
/*{ this procedure opens the ID-TS, reads all topo-frames, builds up a
    temporary configuration }*/


UTBC_RESULT process_config( UIC_NADI *p_nadi, UI_NODES_Info *pNodesInfo );
/*{ this processes the configuration and determines its final state }*/


void sort_vdescr( UIC_VehDescr *pVD,
                  UNSIGNED8 num_entries,
                  UNSIGNED8 nb_nodes );
/*{ this procedure sorts the vehicle descriptions from top-address to
    bottom-address and swaps the UIC-IDs within one trainset
    depending on the orientation to the TCN-master }*/

NodesState get_nodes_state( UI_NODES_Info *p_NodesInfo );
/*{ this procedure gets the state of the configuration }*/

void build_act_nadi( UI_NODES_Info *p_NodesInfo );
/*{ builds active NADI }*/

void confirm_act_nadi( UI_NODES_Info *p_NodesInfo );
/*{ confirms active NADI }*/

void correct_act_nadi( UI_NODES_Info *p_NodesInfo );
/*{ corrects active NADI }*/

void build_saved_nadi( UI_NODES_Info *p_NodesInfo );
/*{ updates saved NADI }*/

int process_unknown_nodes( void );
/*{ handles all unknown nodes and reintegrates, if possible }*/

void process_single_node( UI_NODES_Info *p_NodesInfo );
/*{ handles single node }*/

BOOLEAN ismy_uicid( TYPE_UIC_ID *p_id );

UNSIGNED8 my_last_orient( TYPE_UIC_ID *p_id, int num_entr );

void get_act_entry( TYPE_UIC_ID *p_id,
                    UIC_VehDescr *p_vd,
                    int num_entries );

BOOLEAN chk_same_veh( UIC_VehDescr *p_vd, int offset );
/*{ checks, if the next entry is for next vehicle }*/

BOOLEAN nodes_are_lost( void );
/*{ checks, if there are lost nodes }*/

BOOLEAN nodes_are_insert( void );
/*{ checks, if there are nodes with invalid UIC addresses }*/

void uic_standstill( BOOLEAN option );
/*{ indicates, if correction is allowed }*/

UNSIGNED8 fill_saved_state_dscr( int num_entr );

UNSIGNED8 get_saved_rno( TYPE_UIC_ID *p_id, int *p_ndx );

UNSIGNED8 get_bounds( UNSIGNED8 nd_addr,
                      UNSIGNED8 *p_intgr_ndx,
                      UNSIGNED8 n_entr,
                      UNSIGNED8 n_saved );

void analyze_unknown_nodes( UI_NODES_Info *p_NodesInfo );

void set_uicmain_orient( UIC_VehDescr *p_vd,
                         int nadi_entries );
/*{ sets bitmask for orientation to uic main-driving direction }*/

void set_leading_property( UIC_VehDescr *p_vd,
                           int nadi_entries );
/*{ sets bitmask for leading property }*/

void determine_ld_veh( UIC_VehDescr *p_vd,
                       int nadi_entries,
                       BOOLEAN all_entries );


BOOLEAN calculate_rnodir( UI_NODES_Info *p_NodesInfo,
                          UIC_VehDescr *p_vd,
                          int num_entries,
                          UNSIGNED8 from );

UNSIGNED8 my_range( TYPE_UIC_ID* p_own_uicid,
                    UNSIGNED8 own_tcn,
                    UNSIGNED8* p_own_index,
                    UNSIGNED8* p_own_ld );

BOOLEAN rnodir_changed( UNSIGNED8 old_direc,
                        UNSIGNED8 num_entr );

BOOLEAN integrate_my_node( UI_NODES_Info *p_NodesInfo );

BOOLEAN tractionleader_pos( UIC_VehDescr *p_vd,
                            int num_entr,
                            TYPE_UIC_ID *p_tr_ld_id,
                            int *p_index );

void set_own_state_params( BOOLEAN conf_unknown,
                           BOOLEAN uicaddr_set,
                           BOOLEAN conf_saved );

UNSIGNED8 get_st( UNSIGNED8 tcn_addr, int num_entr );

UNSIGNED8 bot_entries_lost( UI_NODES_Info *p_NodesInfo );

UNSIGNED8 top_entries_lost( UI_NODES_Info *p_NodesInfo );

BOOLEAN fill_lost_rno_tbl( UI_NODES_Info *p_NodesInfo );

BOOLEAN is_lost_rno( UNSIGNED8 rno, UNSIGNED8 *p_index );

void invalidate_entry( UIC_VehDescr *p_vd, UNSIGNED8 rno );

void copy_act_to_saved_nadi( int num_entr );

void ascend_range_act( UIC_VehDescr *p_vd, int num_entr );

void descend_range_act( UIC_VehDescr *p_vd, int num_entr );

void change_to_ascend( UIC_VehDescr *p_vd, int num_entr );

void change_to_descend( UIC_VehDescr *p_vd, int num_entr );

void process_correction( UIC_WriteCorrection *p_correction, UIC_Msg* p_reply );

void write_vehresnum( UIC_WriteResNum* p_vehresnum, UIC_Msg* reply_msg );

void confirm_config( void );

void delete_config( void );

TYPE_UIC_ID my_uicid_from_cfg( ums_NodeConfigurationTable* p_cfg );

void set_uic_params( UIC_VehDescr *p_vd,
                     int nadi_entries,
                     UI_NODES_Info *p_NodesInfo,
                     NodesState actual_state );

void reset_dynamic_idata( void );

void update_na_veh( UI_NODES_Info *p_NodesInfo );

void update_dynamic_idata( UI_NODES_Info *p_NodesInfo,
                           UNSIGNED8 nadi_entries );

void get_utbc_params( UNSIGNED8 *params );

UNSIGNED8 cnt_ins_veh( void );

UNSIGNED8 cnt_lost_entries( void );

#ifdef O_KFT
void write_tci( UNSIGNED8 *p_byte_in, UNSIGNED8 num_veh );

int bit_len( UNSIGNED8 *bitset );

void bit_swap( UNSIGNED8 *bitset, int len );

void process_kft_bf( UIC_VehDescr *p_vd, int num_entr,
                     NodesState actual_state );

#endif


#endif  /* UTBC_H_ */


/**********************************************************************/
