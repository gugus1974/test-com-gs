/*************************************************************************
**        Copyright (C) Siemens AG 1997 All Rights Reserved. Confidential    
**========================================================================
**
**    PROJECT::    TCN Train Communication Network
                   JDP Joint Development Project

**    MODULE::     UIC-Train Bus Configurator 

**    WORKFILE::   $Workfile:   UTBC.C  $
**************************************************************************
**    TASK::       UIC-Train Bus Configurator

**************************************************************************
**    AUTHOR::     Hilbrich ()  
**    CREATED::    20.10.97
**========================================================================
**    HISTORY::    AUTHOR::   $Author:   HEINDEL_AR  $
                   REVISION:: $Revision:   1.88  $  
                   MODTIME::  $Date:   14 Dec 1998 16:44:20  $
      $Log:   O:\PRJ\JDP\FUE\SW\PVCS\ARC\TCN_FW\UIC\SRC\UTBC.Cv_  $
 * 
 *    Rev 1.88   14 Dec 1998 16:44:20   HEINDEL_AR
 * allocate 64 NADI entries
 * process_unknown_nodes will force act NADI in case of overflow
 * 
 *    Rev 1.87   10 Dec 1998 18:09:14   HEINDEL_AR
 * added decimal notation of uic_id in PrintNADI
 * 
 *    Rev 1.87   10 Dec 1998 18:08:26   HEINDEL_AR
 * added decimal notation of uic_id in PrintNADI
 * 
 *    Rev 1.86   10 Dec 1998 16:38:56   HEINDEL_AR
 * new NADI print format
 * 
 *    Rev 1.85   12 Nov 1998 12:20:58   HILBRICH_KL
 * read ld. direc from IDTS, if own veh. is leading
 * 
 *    Rev 1.84   09 Nov 1998 15:34:14   HILBRICH_KL
 * update kft-bitfield also, if own node is not included
 * 
 *    Rev 1.83   05 Nov 1998 17:06:26   HILBRICH_KL
 * single node mode as separate state
 * 
 *    Rev 1.82   04 Nov 1998 16:49:50   HILBRICH_KL
 * sort entries of single node depending on orientation
 * 
 *    Rev 1.81   04 Nov 1998 14:44:08   HILBRICH_KL
 * modifications in calculate_rnodir() 
 * 
 *    Rev 1.80   02 Nov 1998 12:00:44   REINWALD_LU
 * bugfix: LD_REQ_BITMASK instaed of UI_DEMAND_LD
 * 
 *    Rev 1.79   30 Oct 1998 12:17:22   HILBRICH_KL
 * bug fixed in single-node mode handling
 * 
 *    Rev 1.78   29 Oct 1998 15:54:18   HILBRICH_KL
 * correction handling: do not store last tcn-topo
 * 
 *    Rev 1.77   27 Oct 1998 12:40:06   HILBRICH_KL
 * single node mode-handling modified
 * 
 *    Rev 1.76   14 Oct 1998 15:51:44   HILBRICH_KL
 * function 'determine_ld_veh()' inserted: ignore invalid UIC addresses
 * 
 *    Rev 1.75   13 Oct 1998 11:53:40   HILBRICH_KL
 * confirm handling modified
 * 
 *    Rev 1.74   09 Oct 1998 14:21:34   HILBRICH_KL
 * set 'veh_add_on' to zero at initialisation
 * 
 *    Rev 1.73   09 Oct 1998 09:44:30   HILBRICH_KL
 * additional check for reinsertion
 * 
 *    Rev 1.72   08 Oct 1998 16:41:14   HILBRICH_KL
 * set 'leading request bit' in veh_add_on corrected
 * 
 *    Rev 1.71   07 Oct 1998 12:43:52   HILBRICH_KL
 * bug fixed in 'process_unknown_nodes()'
 * 
 *    Rev 1.70   06 Oct 1998 13:28:56   HILBRICH_KL
 * set NADI parameters for node, that is not included
 * 
 *    Rev 1.69   06 Oct 1998 10:00:24   HILBRICH_KL
 * end node redundance-switch supported
 * 
 *    Rev 1.68   01 Oct 1998 08:55:40   HILBRICH_KL
 * leading request bit added to NADI-entries
 * 'ls_t_GetTopography()' replaced by 'uwtm_get_topo()' 
 * 
 *    Rev 1.67   29 Sep 1998 14:02:40   HILBRICH_KL
 * complete revision
 * 
 * 
 * 
 **************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "profile.h"
#include "tcntypes.h"
#include "pi_sys.h"
#include "am_sys.h"
#include "bmi.h"
#include "ums.h"
#include "ungs.h"
#include "adid.h"
#include "utbc.h"
#include "uwtm.h"
#include "recorder.h"
#ifdef O_KFT 
#include "kft.h"
#endif
#ifdef O_CTRLIF
#include "ctrlif.h"
#endif
#include "umsconv.h"


#ifdef _UTBC_TRACE
extern FILE* tracefile;
char txt[ 128 ];
#endif


#ifdef NADI_TESTAUSGABE
void PrintNADI( void );
#endif


/* g l o b a l   v a r i a b l e s ( prefixed by g_xxx ) */

/* configuration-table of this node */
ums_NodeConfigurationTable   g_own_cfg;

/* own inauguartion data frame */
static TYPE_InaugData  g_own_inaug_data;  /* own inauguration data */

UNSIGNED8      g_max_entries;      /* max entries in NADI */
UNSIGNED8      g_max_num_nodes;    /* max nodes */

/* position of not available vehicles */
BITSET8        g_na_veh[ TFR_LEN_NA_VEH ]; 

/* nodes and states */
Node_and_State  g_nd_st[ MAX_CORR_VEH ];

/* lost rno table */
Lost_Rno_Data   g_l_rno_tbl[ MAX_CORR_VEH ];

/* data for handling unknown nodes */
Unknown_Node_Data  g_u_nd_data[ MAX_CORR_VEH ];                                

/* UTBC event buffer */
UNSIGNED8     g_utbc_events[ 16 ];
UNSIGNED8     g_ev_entries = 0;
UNSIGNED8     g_event_index = 0;


TYPE_UIC_ID    g_own_uicid;         /* my own UIC ID */
TYPE_UIC_ID    g_ld_uicid;          /* leading vehicle UIC ID */
UIC_NADI       g_utbc_nadi;         /* local NADI calculated by UTBC */
UTBC_REPORT    g_utbc_report;       /* utbc report function */
BOOLEAN        g_standby_start;     /* flag indicating red.-switch */
BOOLEAN        g_wr_standstill;     /* flag set from UWTM */
BOOLEAN        g_rd_standstill;     /* flag read at beginning of inauguration */
BOOLEAN        g_rnochng;           /* flag indicating rno dir. changed */
BOOLEAN        g_cfg_delbit_mynode; /* flag indicating that delete-bit was set on my node */
UNSIGNED8      g_rno_dir;           /* actual rno-direction */
UNSIGNED8      g_saved_nadi_entries;  /* nb. entries in saved NADI */ 

UIC_VehDescr   *gp_savedVehDescr;
UIC_VehDescr   *gp_tmpVehDescr;
UIC_VehDescr   *gp_SavedStateVehDescr;

/* interface to KFT */
#ifdef O_KFT
extern UNSIGNED8             m_terminal;
extern UIC_WriteCorrection  *p_wc;
UIC_WriteCorrection         *gp_kftcorr;
BITSET8                      g_kft_bf[ LEN_TCI ];  /* KFT bitfield */
#endif


/* if a single node is corrected, we must store */
/* two UIC-IDs and rno's to calculate the       */
/* rno-direction afterwards                     */
TYPE_UIC_ID                  corr_id_first;
TYPE_UIC_ID                  corr_id_last;
UNSIGNED8                    corr_rno_first;
UNSIGNED8                    corr_rno_last;


/* local defines */
#define BYTESTREAM_SIZE_CORR_DATA  0x08
#define BYTESTREAM_SIZE_VRES_DATA  0x07
#define OFFSET_TO_TCI( i )   (BYTESTREAM_SIZE_UIC_Header + 8*i + 1)


static UNSIGNED8   g_report_data[ 8 ]; /* input-data for report-function */

void uic_inaug_enf_requ( UNSIGNED8* px );


/*********************************************************************/
/*  Name:       utbc_global_request()                                */
/*  Function:   general request function of UTBC; handles following  */
/*              ums-telegrams:                                       */
/*              - write correction                                   */ 
/*              - confirm configuration                              */
/*              - delete configuration                               */
/*              - write vehicle reservation numbers                  */
/*              - write KFT correction                               */
/*  returns :   UIC_S_OK, when all went well                         */
/*              UIC_S_INIT_ERR, allocation of buffers failed         */
/*********************************************************************/
UTBC_RESULT  utbc_global_request( void  *p_uic_callmsg,
                   CARDINAL32  uic_callmsg_size,
                   AM_ADDRESS  *caller,
                   void        **p_uic_replymsg,
                   CARDINAL32  *p_uic_replymsg_size,
                   REPLY_FREE  *p_reply_free_fct )

{

    UIC_WriteCorrection* p_correction;
    UIC_WriteResNum* p_veh_res_num;
    UNSIGNED8 *p_byte_in;
    UNSIGNED8 cmd;
    CARDINAL32 cnt_corr_veh;
    UNSIGNED8* p_byte_out;
    UIC_Msg in_msg, out_msg;
#ifdef O_KFT 
    UNSIGNED8 k;
    CorrectionData* p_cd_src;
    CorrectionData* p_cd_dest;
    CARDINAL32 kft_reply_size;
#endif
    

    UNREFERENCED( caller );
    UNREFERENCED( uic_callmsg_size );

    p_byte_in = (UNSIGNED8*)p_uic_callmsg;
    
    /* test for valid ptrs */
    if( (p_byte_in == NULL) || (p_reply_free_fct == NULL) ){
         return UIC_S_MEM_ERR;
    }

    /* default reply size */
    *p_uic_replymsg_size = BYTESTREAM_SIZE_UIC_Header;
    *p_reply_free_fct = PI_FREE;

    /* extract incoming UIC-Message */
    wtbconv_uic_hdr_NTOH( p_byte_in, &in_msg.hdr );
    
    /* prepare outgoing UIC-message */
    exchange_UIC_HEADER( &out_msg, &in_msg );
    out_msg.hdr.state_cmd = UIC_S_OK;

    cmd = in_msg.hdr.state_cmd;

    switch( in_msg.hdr.owner ) 
    {
        /* UIC specific */
        /****************/
        case UIC_O_UIC:

            if( (*p_uic_replymsg = PI_ALLOC( sizeof(UIC_Msg)) ) == NULL ){
                return UIC_S_ALLOC_ERR;
            }
            p_byte_out = (UNSIGNED8*)(*p_uic_replymsg);

            switch( in_msg.hdr.code )
            {

                case UIC_FC_WRITE_CORRECTION:
                
                    /* configuration is only confirmed */
                    if( cmd == UIC_C_CONFIRM_CONFIG )
                    {
                        confirm_config();
                    }
                    /* configuration is corrected */
                    else if( cmd == UIC_C_CORRECT_CONFIG )
                    {
                        cnt_corr_veh = p_byte_in[ BYTESTREAM_SIZE_UIC_Header ];
                        
                        if( cnt_corr_veh > g_max_entries ){
                            utbc_trace_event( EVENT_MAX_ENTR_ERR );
                            out_msg.hdr.state_cmd = UIC_S_PARAM_INCORRECT;
                        }
                        else if( (cnt_corr_veh * BYTESTREAM_SIZE_CORR_DATA + 
                            BYTESTREAM_SIZE_UIC_Header) > uic_callmsg_size ){
                            out_msg.hdr.state_cmd = UIC_S_PARAM_INCORRECT;
                        }
                        else{
                            p_correction = PI_ALLOC( sizeof( UIC_WriteCorrection ) + 
                                                     cnt_corr_veh * sizeof( CorrectionData ) );
                            if( p_correction == NULL ){
                                out_msg.hdr.state_cmd = UIC_S_MEM_ERR;
                            }
                            else{
                                wtbconv_15_02_NTOH( p_byte_in, p_correction ); 
                                process_correction( p_correction, &out_msg );
                                PI_FREE( p_correction );
                            }
                        }
                    }
                    else
                    {
                        out_msg.hdr.state_cmd = UIC_S_PARAM_INCORRECT;
                    }
                    
                    wtbconv_15_02A_HTON( p_byte_out, &out_msg );
                    return UIC_S_OK;


                case UIC_FC_DEL_CONFIG:

                    delete_config();
                    wtbconv_uic_hdr_HTON( p_byte_out, &out_msg.hdr );
                    return UIC_S_OK;


                case UIC_FC_WRITE_VEH_RESNUM:

                    cnt_corr_veh = p_byte_in[ BYTESTREAM_SIZE_UIC_Header ];
                    
                    if( cnt_corr_veh > g_max_entries ){
                        utbc_trace_event( EVENT_MAX_ENTR_ERR );
                        out_msg.hdr.state_cmd = UIC_S_PARAM_INCORRECT;
                    }
                    else if( (cnt_corr_veh * BYTESTREAM_SIZE_VRES_DATA + 
                        BYTESTREAM_SIZE_UIC_Header) > uic_callmsg_size ){
                        out_msg.hdr.state_cmd = UIC_S_PARAM_INCORRECT; 
                    }
                    else{
                        p_veh_res_num = PI_ALLOC( sizeof( UIC_WriteResNum ) + 
                                                  cnt_corr_veh * sizeof( VehResData ) );

                        if( p_veh_res_num == NULL ){
                            out_msg.hdr.state_cmd = UIC_S_MEM_ERR;
                        }
                        else{
                            wtbconv_15_03_NTOH( p_byte_in, p_veh_res_num );
                            write_vehresnum( p_veh_res_num, &out_msg );
                            PI_FREE( p_veh_res_num );
                        }
                    }

                    wtbconv_15_03A_HTON( p_byte_out, &out_msg );
                    return UIC_S_OK;

                default:

                    out_msg.hdr.state_cmd = UIC_S_PARAM_INCORRECT;
                    wtbconv_uic_hdr_HTON( p_byte_out, &out_msg.hdr );
                    return UIC_S_OK;
            
            } /* end 'switch( in_msg.hdr.code )' */

 #ifdef O_KFT

        /* Siemens application specific */
        /********************************/
        case UIC_O_DB:

            switch( in_msg.hdr.code )
            {

               case FC_READ_KFTCORR:
                
                    kft_reply_size = sizeof( UIC_WriteCorrection ) +
                                       MAX_CORR_VEH * sizeof( CorrectionData ) + 8;
                    
                    if( (*p_uic_replymsg = PI_ALLOC( kft_reply_size )) == NULL ){
                        return UIC_S_ALLOC_ERR;
                    }
                    p_byte_out = (UNSIGNED8*)(*p_uic_replymsg);

                    if( gp_kftcorr == NULL ){
                        out_msg.hdr.state_cmd = UIC_S_MEM_ERR;
                        wtbconv_uic_hdr_HTON( p_byte_out, &out_msg.hdr );
                        return UIC_S_OK;
                    }

                    /*** enter critical section ***/
                    pi_lock_task();
                    
                    /* no KFT correction available */
                    if( !m_terminal ){
                        out_msg.hdr.state_cmd = KFT_CORR_NOT_AVAILABLE;
                        pi_unlock_task();
                        wtbconv_uic_hdr_HTON( p_byte_out, &out_msg.hdr );
                        return UIC_S_OK;
                    }
                    
                    /* copy KFT correction; reserved bytes */
                    /* contain some KFT specific information */
                    out_msg.hdr.reserved1 = p_wc->hdr.reserved1;
                    out_msg.hdr.reserved2 = p_wc->hdr.reserved2;

                    gp_kftcorr->hdr = out_msg.hdr;
                    gp_kftcorr->num_veh = p_wc->num_veh;

                    p_cd_dest = &gp_kftcorr->corr_data[ 0 ];
                    p_cd_src = &p_wc->corr_data[ 0 ];

                    for( k = 0; k <= p_wc->num_veh; k++ ){
                         p_cd_dest[ k ] = p_cd_src[ k ];
                    }

                    /*** leave critical section ***/
                    pi_unlock_task();

                    wtbconv_15_02_HTON( p_byte_out, gp_kftcorr );

                    /* (tci) train composition information */
                    memcpy( &p_byte_out[ OFFSET_TO_TCI(gp_kftcorr->num_veh) ],
                            &p_cd_dest[ gp_kftcorr->num_veh ], LEN_TCI );

                    *p_uic_replymsg_size = OFFSET_TO_TCI(gp_kftcorr->num_veh) + LEN_TCI;
                    return UIC_S_OK;

               
               case FC_WRITE_KFTCORR:
                   
                    if( (*p_uic_replymsg = PI_ALLOC( sizeof(UIC_Msg) )) == NULL ){
                        return UIC_S_ALLOC_ERR;
                    }
                    p_byte_out = (UNSIGNED8*)(*p_uic_replymsg);

                    cnt_corr_veh = p_byte_in[ BYTESTREAM_SIZE_UIC_Header ];
                            
                    if( cnt_corr_veh > g_max_entries ){
                        utbc_trace_event( EVENT_MAX_ENTR_ERR );
                        out_msg.hdr.state_cmd = UIC_S_PARAM_INCORRECT;
                    }

                    /* configuration is only confirmed */
                    if( cmd == UIC_C_CONFIRM_CONFIG )
                    {
                        confirm_config();
                    }
                    else if( cmd == UIC_C_CORRECT_CONFIG )
                    {
                        p_correction = PI_ALLOC( sizeof( UIC_WriteCorrection ) + 
                                                 cnt_corr_veh * sizeof( CorrectionData ) );
                        if( p_correction == NULL ){
                            out_msg.hdr.state_cmd = UIC_S_MEM_ERR;
                        }
                        else{
                            wtbconv_15_02_NTOH( p_byte_in, p_correction );
                            /* write train composition information */ 
                            write_tci( p_byte_in, (UNSIGNED8)cnt_corr_veh );
                            process_correction( p_correction, &out_msg );
                            PI_FREE( p_correction );
                        }
                    }
                    else
                    {
                        out_msg.hdr.state_cmd = UIC_S_PARAM_INCORRECT;
                    }

                    
                    wtbconv_15_02A_HTON( p_byte_out, &out_msg );
                    return UIC_S_OK;

               default:
                    
                    out_msg.hdr.state_cmd = UIC_S_PARAM_INCORRECT;
                    *p_uic_replymsg = NULL;
                    *p_uic_replymsg_size = 0;
                    
                    return UIC_S_PARAM_INCORRECT;

            } /* end 'switch( in_msg.hdr.code )' */

#endif  /* O_KFT */
            
        /* invalid owner */
        /*****************/
        default:
           
            out_msg.hdr.state_cmd = UIC_S_PARAM_INCORRECT;
            *p_uic_replymsg = NULL;
            *p_uic_replymsg_size = 0;

            return UIC_S_PARAM_INCORRECT;

    } /* end 'switch( in_msg.hdr.owner )' */

}




/*********************************************************************/
/*  Name:       utbc_init()                                          */
/*  Function:   initialization of the UTBC; builds up own id-frame   */
/*              an writes it into ID-TS; allocates buffer for        */
/*              NADI to be build up                                  */
/*  returns :   UIC_S_OK, when all went well                         */
/*              UIC_S_INIT_ERR, allocation of buffers failed         */
/*********************************************************************/
UTBC_RESULT  utbc_init( ums_NodeConfigurationTable *p_cfg,
                        UTBC_REPORT  utbc_report )
{
     
     unsigned int ix, alloc_size;
     BOOLEAN alloc_err;

     /* init event tracing */
     memset( g_utbc_events, 0, 16 );
     g_event_index = 0;
     g_ev_entries = 0;
     alloc_err = FALSE;

     /* test valid config-ptr */
     if( p_cfg == NULL ){
         utbc_trace_event( EVENT_UMSCFG_INVALID );
         return UIC_S_INIT_ERR;
     }

     /* get own configuration-data */
     g_own_cfg = *p_cfg;

     /* report function */
     g_utbc_report = utbc_report;
   
     g_max_entries = g_own_cfg.num_NADI_entries;
     g_max_num_nodes = g_own_cfg.max_num_nodes;

     if( g_max_entries > UI_NADI_MAX_ENTRIES ){
        return UIC_S_PARAM_INCORRECT;
     }

     g_own_uicid = my_uicid_from_cfg( &g_own_cfg );

     /* build up own inauguration frame */
     init_idata( &g_own_inaug_data );

     alloc_size = sizeof( UIC_VehDescr ) * g_max_entries;

     alloc_size = sizeof( UIC_VehDescr ) * 64; /*zzzzZZZZZZzzzz*/

     /* pointer initialisations */
     gp_tmpVehDescr = (UIC_VehDescr*)PI_ALLOC( alloc_size );
     if( gp_tmpVehDescr == (UIC_VehDescr*)NULL ){
         utbc_trace_event( EVENT_ALLOC_ERR );
         alloc_err = TRUE;
     }

     gp_savedVehDescr = (UIC_VehDescr*)PI_ALLOC( alloc_size );
     if( gp_savedVehDescr == (UIC_VehDescr*)NULL ){
         utbc_trace_event( EVENT_ALLOC_ERR );
         alloc_err = TRUE;
     }

     gp_SavedStateVehDescr = (UIC_VehDescr*)PI_ALLOC( alloc_size );
     if( gp_SavedStateVehDescr == (UIC_VehDescr*)NULL ){
         utbc_trace_event( EVENT_ALLOC_ERR );
         alloc_err = TRUE;
     }

#ifdef O_KFT
     if( g_own_cfg.KFT_used ){
        gp_kftcorr = (UIC_WriteCorrection*)PI_ALLOC( sizeof( UIC_WriteCorrection ) +
                             MAX_CORR_VEH * sizeof( CorrectionData ) + 8 );
        if( gp_kftcorr == (UIC_WriteCorrection*)NULL ){
            utbc_trace_event( EVENT_ALLOC_ERR );
            alloc_err = TRUE;
        }
     }
#endif

     if( alloc_err ){
         if( gp_tmpVehDescr ) PI_FREE( gp_tmpVehDescr );
         if( gp_savedVehDescr ) PI_FREE( gp_savedVehDescr );
         if( gp_SavedStateVehDescr ) PI_FREE( gp_SavedStateVehDescr );
#ifdef O_KFT
         if( gp_kftcorr ) PI_FREE( gp_kftcorr );
#endif
         return UIC_S_INIT_ERR;
     }

     /* undefined vehicle reservation numbers */
     for( ix = 0; ix < g_max_entries; ix++ ){
        gp_savedVehDescr[ ix ].VehDescr.veh_res_num = 0;
        gp_tmpVehDescr[ ix ].VehDescr.veh_res_num = 0;
     }

     /* write inauguration data to ID-TS ( error, if size >= 124 Bytes ) */
     if( LI_INVALID == li_t_ChgInaugData( g_own_inaug_data.size, &g_own_inaug_data ) ){
         return UIC_S_INIT_ERR;
     }

     reset_dynamic_idata();

     /* initialisation of some static variables */
     g_cfg_delbit_mynode = FALSE;
     g_standby_start = FALSE;

     return UIC_S_OK;

}


/**********************************************************************/
/*  Name:       utbc_build_nadi()                                     */
/*  Function:   is directly called from UWTM after UIC inauguration;  */
/*              calcualtes new NADI                                   */
/*  returns :   UIC_S_OK, when all went well                          */
/*              UIC_S_ERR_IDTS, error reading inauguration data       */
/*              UIC_S_UTBC_ERR  general error                         */
/*              if the function returns successful, following         */
/*              parameters are valid:                                 */
/*              p_status:  state of calculated NADI                   */
/*              p_uic_addr: uic-address of this vehicle               */
/*              p_leading: set to 1, if this vehicle is leading       */
/*              p_direc: direction of the vehicle that became leading */
/*              p_ld_exists: leading veh. exists                      */
/*              mode: Bit 0 set to 1: only one node exists            */
/*                    Bit 1 set to 1: node reinsertion / inclusion    */
/**********************************************************************/
UTBC_RESULT  utbc_build_nadi( UNSIGNED8 *p_status, 
                              UNSIGNED8 *p_uic_addr,
                              UNSIGNED8 *p_leading,
                              UNSIGNED8 *p_direc,
                              BOOLEAN *p_ld_exists,
                              UNSIGNED8 mode )
{
    
    UI_NODES_Info  NodesInfo;
    UNSIGNED8 own_index, own_ld;
    UTBC_RESULT u_res;

    /* read standstill flag */
    g_rd_standstill = g_wr_standstill;

    utbc_trace_event( EVENT_NEW_INAUG );

    /* default NADI */
    g_utbc_nadi.global_descr.nadi_state = UIC_NADI_STATE_INVALID;
    g_utbc_nadi.global_descr.num_entries = 0;

    u_res = read_config( &NodesInfo, mode ); 
    
    if( u_res == UIC_S_OK ){ 
        u_res = process_config( &g_utbc_nadi, &NodesInfo );
    }

    /* write ( also invalid ) NADI to UNGS */
    ungs_NADI_write ( &g_utbc_nadi );

    if( (u_res != UIC_S_OK) && (u_res != UIC_S_INSERT_ERR) ){
        return u_res;
    }

        
#ifdef NADI_TESTAUSGABE

    PrintNADI();
 
#endif

    /* NADI state */
    *p_status = g_utbc_nadi.global_descr.nadi_state;
    
    /* own uic address */
    *p_uic_addr = my_range( &g_own_uicid,
                            NodesInfo.my_address,
                            &own_index, 
                            &own_ld );

    /* own orientation to UIC main direction */
#ifdef O_CTRLIF
    ci_set_uic_orientation( (g_utbc_nadi.p_veh_descr[ own_index ].veh_add_on & ORIENT_VEH_UICMAIN) ? 1 : 2 );
#endif

    /* leading property ( vehicle or trainset ) */
    if( own_ld ){
        *p_leading = VEH_OR_TS_LEADING;
        li_t_GetInaugDataByte( TFR_OFFSET_TO_ST_CTRL_INFO, p_direc );
        if( *p_direc & UI_DIR_LD_DEMAND ) *p_direc = UIC_DIR2;
        else *p_direc = UIC_DIR1;
    }
    else{
        *p_leading = VEH_OR_TS_NOT_LEADING;
        *p_direc = 0;
    }

    /* leading vehicle exists */
    if( !ISINVALID_UIC_ID( &g_ld_uicid ) ){
        *p_ld_exists = TRUE;
    }
    else{
        *p_ld_exists = FALSE;
    }

    g_report_data[0] = 1;
    g_report_data[1] = g_rno_dir;
    g_report_data[2] = g_utbc_nadi.global_descr.num_entries;
    g_report_data[3] = NodesInfo.top_address;
    g_report_data[4] = NodesInfo.bottom_address;
    g_report_data[5] = NodesInfo.my_address;
    
    return UIC_S_OK;

}


/*********************************************************************/
/*  Name:       utbc_red_indicate()                                  */
/*  Function:   is called from the UWTM in case of this node         */
/*              starts because of redundancy switch                  */
/*********************************************************************/
UTBC_RESULT  utbc_red_indicate( void )
{
    g_standby_start = TRUE;
    li_t_ChgInaugDataByte( TFR_OFFSET_TO_ST_CTRL_INFO,
                      UI_REDUNDANCY_BIT, UI_REDUNDANCY_BIT );
    return UIC_S_OK;
}


/*********************************************************************/
/*  Name:       uic_standstill()                                     */
/*  Function:   is called from the UWTM to signal                    */
/*              if correction is allowed                             */
/*********************************************************************/
void uic_standstill( BOOLEAN option )
{
   g_wr_standstill = option;
}




/*********************************************************************/
/*  Name:       read_config()                                        */
/*  Function:   opens the ID-TS, reads all topo-frames and           */
/*              builds up the temporary configuration                */
/*  returns :   UIC_S_OK, when all went well                         */
/*              UIC_S_ERR_IDTS, error reading inauguration data      */
/*              UIC_S_UIVER_CFL, this vehicle is not uic-conform     */
/*              UIC_S_UTBC_ERR, nb. nodes exceeds max. allowed       */
/*                              value from NSDB                      */
/*                                                                   */
/*              when the function returns UIC_S_OK, following        */
/*              elements of 'p_NodesInfo' are valid:                 */
/*              nb_nodes: number of nodes in config.                 */
/*              nb_entries: number entries in config.                */
/*              bottom_address: bottom node in config.               */
/*              top_address: top node in config.                     */
/*              del_cc: at least one node requests to delete config. */
/*              nb_nodes_cu: nb. of unknown nodes                    */
/*              nb_nodes_set: nb. of nodes with UIC address set      */
/*              nb_nodes_saved: nb. of saved nodes                   */
/*              red_switch: at least one redundancy-switch occured   */
/*              ct_veh: vehicle counter                              */
/*              corr_info_corrupted: corr. info inconsistent         */
/*                                                                   */
/*********************************************************************/
UTBC_RESULT  read_config( UI_NODES_Info *p_NodesInfo,
                          BOOLEAN mode )
{

     UNSIGNED8 cnt, nb_nodes, ncv;
     UNSIGNED8 ix, index, ndx;
     UNSIGNED8 retval, nd_st_ndx;
     UNSIGNED8 uic_ind_cmp, src_device;
     UNSIGNED8 *p_veh_info;
     UNSIGNED8 *p_veh_descr;
     UNSIGNED16 h_idts;
     BITSET8 st_ctrl_info;
     BOOLEAN CorrInfoStored;
     BOOLEAN eq_gw_bit_found;
     TYPE_InaugData i_data;
     TYPE_UIC_ID uic_id_last;
     Type_Topography topo;
     UIC_VehDescr *p_vd_use;
     UIC_VehDescr *p_vd_ld;

     memset( p_NodesInfo, 0, sizeof(UI_NODES_Info) );
     memset( g_u_nd_data, 0, sizeof( g_u_nd_data ) );
     memset( g_nd_st, 0, sizeof( g_nd_st ) );
     
     nd_st_ndx = 0;

     /* set NADI temporary invalid */
     if( mode & SET_TEMP_INVALID ){
         return UIC_S_UTBC_ERR;
     }

     /* reinsertion of a middle node */
     if( mode & NODE_REINSERTION ){
         p_NodesInfo->node_reinsertion = TRUE;
     }

     /* new topo was received */
     if( mode & NEW_TOPO_RCVD ){
         p_NodesInfo->new_topo_received = TRUE;
     }

     SETINVALID_UIC_ID( &uic_id_last );
     SETINVALID_UIC_ID( &g_ld_uicid );

#ifdef O_KFT
     memset( g_kft_bf, 0, LEN_TCI );
#endif

     retval = UIC_S_OK;

     CorrInfoStored = FALSE;
     index = ndx = 0;
     p_NodesInfo->uic_ver = INVALID_VERSION;
     p_NodesInfo->uic_subver = INVALID_VERSION;

     if( uwtm_get_topo( &topo ) != L_OK ){
         utbc_trace_event( EVENT_TOPO_ERR );
         return UIC_S_UTBC_ERR;
     }
     
     p_NodesInfo->topo_counter = topo.topo_counter;
     p_NodesInfo->top_address = topo.top_address;
     p_NodesInfo->bottom_address = topo.bottom_address;
     p_NodesInfo->my_address = topo.node_address;
     
     /* open ID-Trafficstore */
     if( li_t_IDTSOpen( &nb_nodes, &h_idts ) != LI_OK ){
        retval = UIC_S_ERR_IDTS;
        goto read_config_exit;
     }


     /* parameter checking */
     if( (nb_nodes > g_max_num_nodes) || (nb_nodes == 0) ){
        utbc_trace_event( EVENT_NB_ND_GR_MAX );
        retval = UIC_S_UTBC_ERR;
        goto read_config_exit;
     }

     /* single node mode */
     if( nb_nodes == 1 ){
         p_NodesInfo->single_node_mode = TRUE;
     }

          
     /* for all entries ( nodes ) in ID-TS */
     /* ---------------------------------- */
     for( cnt = 0; cnt < nb_nodes; cnt++ ){
        
        if( li_t_IDTSReadNext( h_idts, &index, &i_data ) != LI_OK ){
            retval = UIC_S_ERR_IDTS;
            goto read_config_exit;
        }

        /* src device and state of topo frame */
        src_device = i_data.src_dev & SRC_DEV_BITMASK;
        st_ctrl_info = TFR_ST_CTRL_INFO( i_data );

        /* active configuration, middle node lost and uic inauguration */
        /* occured --> node entry in ID-TS has its tcn address set to 0 */
        if( src_device != 0x00 ){
            p_NodesInfo->nb_nodes++;
        }
        else{
            continue;
        }

        /* test uic-conformance */
        if( cnt == 0 ){
            uic_ind_cmp = (i_data.UIC_Ind & UIC_INDICATION_BITMASK);
        }
        else{
            if( uic_ind_cmp != (i_data.UIC_Ind & UIC_INDICATION_BITMASK) ){
                utbc_trace_event( EVENT_VER_CONFLICT );
                retval = UIC_S_UIVER_CFL;
                goto read_config_exit;
            }
        }

        /* minimum of uic-versions/subversions */
        p_NodesInfo->uic_ver = MIN( p_NodesInfo->uic_ver, i_data.UIC_Ver );
        p_NodesInfo->uic_subver = MIN( p_NodesInfo->uic_subver, TFR_UIC_SUBVER( i_data ) );

        /* nb. controlled vehicles < 0 */
        ncv = TFR_NUM_CV( i_data );
        if( ncv & MULTI_GW_MASK ) ncv = 1;
        eq_gw_bit_found = FALSE;

        
        /* read description of controlled vehicles and if necessary, */
        /* expand trainsets                                          */
        /* --------------------------------------------------------  */
        for( ix = 0; ix < ncv; ix++ )
        {
            p_veh_info = TFR_P_VEH_INFO( i_data, ix );
            p_veh_descr = TFR_P_VEH_DESCR( i_data, ix );
            p_vd_use = &gp_tmpVehDescr[ ndx ];
            
            p_vd_use->NodeAddr      = src_device;
            p_vd_use->num_ctrl_veh  = TFR_NUM_CV( i_data );
            p_vd_use->veh_add_on    = 0;

            if( *(p_veh_info + VEH_EQ_GW_OFFSET_BYTE) & VEH_EQ_GW_BITMASK ){
                
                eq_gw_bit_found = TRUE;
                
                /* store the saved range number */
                p_vd_use->RangeNo = TFR_RNO( i_data );
            }
            else{
                p_vd_use->RangeNo = 0;
            }

            p_vd_use->veh_management  = TFR_VEH_MAN( i_data );
            p_vd_use->veh_owner       = TFR_VEH_OWNER( i_data );
            p_vd_use->application_id  = TFR_APPL_ID( i_data );
            p_vd_use->application_ver = TFR_APPL_VER( i_data );

            /* trainset info */
            memcpy( p_vd_use->TSInfo, TFR_P_TS_INFO( i_data ), TFR_LEN_TS_INFO );
                                    
            /* UIC-ID */
            p_vd_use->VehDescr.uic_id = *(TYPE_UIC_ID*)(p_veh_descr + TFR_OFFSET_TO_UIC_ID);

            /* vehicle reservation number */
            p_vd_use->VehDescr.veh_res_num = NTOH16( p_veh_descr + TFR_OFFSET_TO_VEH_RES_NUM );

            /* reserved fields */
            p_vd_use->VehDescr.reserved1 = 0;
            p_vd_use->VehDescr.reserved2 = 0;

            /* bit 7 of 'i_data.src_dev' encodes orientation to tcn-master */
            /* and must be written to bit 0 of 'veh_add_on' */
            p_vd_use->veh_add_on |= (i_data.src_dev >> 7);

            /* vehicle info */
            memcpy( &(p_vd_use->VehDescr.veh_info), p_veh_info, TFR_LEN_VEH_INFO );

            /* vehicle counter */
            if( UIC_ID_CMP( &uic_id_last, &(p_vd_use->VehDescr.uic_id) ) != 0 ){
                uic_id_last = p_vd_use->VehDescr.uic_id;
                p_NodesInfo->ct_veh++;
                if( st_ctrl_info & UI_CONF_UNKNOWN ){
                    p_NodesInfo->ct_veh_cu++;
                }
                /* number of saved vehicles */
                if( st_ctrl_info & UI_CONF_SAVED ){
                    p_NodesInfo->ct_veh_cs++;
                }
            }

            /* unknown bottom/top entries */
            if( st_ctrl_info & UI_CONF_UNKNOWN ){
                if( src_device == p_NodesInfo->top_address){
                    p_NodesInfo->nb_entries_top_cu++;
                }
                else if( src_device == p_NodesInfo->bottom_address){
                    p_NodesInfo->nb_entries_bottom_cu++;
                }
            }

            /* number of entries exceeds max. value */
            if( ++ndx > g_max_entries ){
                utbc_trace_event( EVENT_MAX_ENTR_ERR );
                return UIC_S_UTBC_ERR;
            }

         } /* end 'for each controlled vehicle' */


         /* leading request */
         if( st_ctrl_info & UI_DEMAND_LDSHP ){
             /* UWTM_DIR2 */
             if( st_ctrl_info & UI_DIR_LD_DEMAND ) p_vd_ld = p_vd_use;
             /* UWTM_DIR1 */
             else p_vd_ld = p_vd_use - ncv + 1;
             p_vd_ld->veh_add_on |= LD_REQ_BITMASK;
         }

         /* leading property */
         if( st_ctrl_info & UI_VEH_LEADING ){
             /* UWTM_DIR2 */
             if( st_ctrl_info & UI_DIR_LD_DEMAND ) p_vd_ld = p_vd_use;
             /* UWTM_DIR1 */
             else p_vd_ld = p_vd_use - ncv + 1;
             p_vd_ld->veh_add_on |= LD_VEH_BITMASK;
         }


         if( !eq_gw_bit_found ){
             utbc_trace_event( EVENT_EQ_GWBIT_NOTSET );
         }

         if( st_ctrl_info & UI_DEL_CC ){
             p_NodesInfo->del_cc = 1;
         }

         if( st_ctrl_info & UI_CONF_UNKNOWN ){
             p_NodesInfo->nb_nodes_cu++;
             if( (st_ctrl_info & UI_REDUNDANCY_BIT) || g_standby_start ){
                 p_NodesInfo->red_switch = 1;
                 p_NodesInfo->red_addr = src_device;
             }
         }
         if( st_ctrl_info & UI_UIC_ADDR_SET ){
             p_NodesInfo->nb_nodes_set++;
         }
         if( st_ctrl_info & UI_CONF_SAVED ){
             p_NodesInfo->nb_nodes_saved++;
         }

         if( p_NodesInfo->my_address == src_device ){
             p_NodesInfo->my_state = st_ctrl_info & STATE_MASK;
         }
         
         g_nd_st[ nd_st_ndx ].nd_addr = src_device;
         g_nd_st[ nd_st_ndx ].state = st_ctrl_info & STATE_MASK;
         nd_st_ndx++;

         /* when node is in state 'UIC-address set' or 'configuration saved', */
         /* correction information must be stored */
         if( (st_ctrl_info & UI_UIC_ADDR_SET) || (st_ctrl_info & UI_CONF_SAVED) ) 
         {
             if( !CorrInfoStored ){
                 memcpy( g_na_veh, TFR_P_NA_VEH( i_data ), TFR_LEN_NA_VEH );
                 p_NodesInfo->nb_veh_saved = TFR_NUM_VEH( i_data );
#ifdef O_KFT
                 memcpy( g_kft_bf, (UNSIGNED8*)(TFR_P_TS_INFO( i_data ) + START_BS48), LEN_TCI );
#endif
                 CorrInfoStored = TRUE;
             }
             /* check consistency of announced correction-info */
             else if( (TFR_NUM_VEH( i_data ) != p_NodesInfo->nb_veh_saved) ||
                      (memcmp( g_na_veh, TFR_P_NA_VEH(i_data), TFR_LEN_NA_VEH) != 0) )
             {
                 if( p_NodesInfo->corr_info_corrupted == FALSE ){
                     utbc_trace_event( EVENT_CINFO_CORRUPTED );
                 }
                 p_NodesInfo->corr_info_corrupted = TRUE;
             }
         }
        
    } /* end 'for each node' */


    /* number of NADI entries */
    p_NodesInfo->nb_entries = ndx;

    /* parameter checking */
    if( ndx == 0 ){
        retval = UIC_S_UTBC_ERR;
        goto read_config_exit;
    }

    /* sort entries from bottom to top-node */
    sort_vdescr( gp_tmpVehDescr, ndx, p_NodesInfo->nb_nodes );

read_config_exit:

    li_t_IDTSClose( h_idts );
    return retval;

}


/*********************************************************************/
/*  Name:       process_config()                                     */
/*  Function:   this procedure sorts the configuration from          */
/*              tcn-bottom to top, sorts the vehicles within         */
/*              trainsets and completes the NADI with its            */
/*              range-numbers                                        */
/*  returns :   UIC_S_OK, when all went well                         */
/*********************************************************************/
UTBC_RESULT process_config( UIC_NADI *p_nadi, UI_NODES_Info *pNodesInfo )
{

    UNSIGNED8 nadi_entries;
    UIC_NadiState nadi_state;
    NodesState actual_state;
    UIC_VehDescr *p_vd_utbc_nadi;
    UIC_GlobalDescr *p_g_descr;
    UTBC_RESULT u_res;
                        
    /* default state is invalid */
    nadi_state = ST_INVALID;
    nadi_entries = 0;
    p_g_descr = &p_nadi->global_descr;
    g_rnochng = FALSE;

    actual_state = get_nodes_state( pNodesInfo );
    
    u_res = UIC_S_OK;
    
    switch( actual_state )
    {
        /**********/
        case ST_INVALID:

            u_res = UIC_S_UTBC_ERR;
            goto exit_read_cfg; 

        /**********/
        case ST_ACFG:
            build_act_nadi( pNodesInfo );
            nadi_state = UIC_NADI_STATE_ACT;
            nadi_entries = pNodesInfo->nb_entries;
            p_vd_utbc_nadi = gp_tmpVehDescr;
            reset_dynamic_idata();

            p_g_descr->add_on_info &= 
                  (UNSIGNED8)(~(NODE_LOSS_BITMASK | NODE_ACTIVE_BITMASK));

            break;

        /**********/
        case ST_CONFIRM_ACFG:
            confirm_act_nadi( pNodesInfo );
            nadi_state = UIC_NADI_STATE_CONF;
            nadi_entries = g_saved_nadi_entries;
            p_vd_utbc_nadi = gp_savedVehDescr;
            break;

        
        /**********/
        case ST_SINGLE_ND_SET:
        case ST_SINGLE_ND_SAV:
            process_single_node( pNodesInfo );
            nadi_state = UIC_NADI_STATE_CONF;
            nadi_entries = g_saved_nadi_entries;
            p_vd_utbc_nadi = gp_savedVehDescr;
            set_uic_params( p_vd_utbc_nadi, nadi_entries,
                            pNodesInfo, actual_state );
            update_dynamic_idata( pNodesInfo, nadi_entries );
            break;


        /**********/
        case ST_CORRECT_CFG:
            correct_act_nadi( pNodesInfo );
            nadi_state = UIC_NADI_STATE_CONF;
            nadi_entries = g_saved_nadi_entries;
            p_vd_utbc_nadi = gp_savedVehDescr;
            set_uic_params( p_vd_utbc_nadi, nadi_entries, 
                            pNodesInfo, actual_state );
            update_dynamic_idata( pNodesInfo, nadi_entries );
            break;


        /**********/
        case ST_SAVED_NADI:
            build_saved_nadi( pNodesInfo );
            nadi_state = UIC_NADI_STATE_CONF;
            nadi_entries = g_saved_nadi_entries;
            p_vd_utbc_nadi = gp_savedVehDescr;
            set_uic_params( p_vd_utbc_nadi, nadi_entries,
                            pNodesInfo, actual_state );
            update_dynamic_idata( pNodesInfo, nadi_entries );
            break;


        /**********/
        case ST_PROCESS_UNKNOWN_ND:
            nadi_state = UIC_NADI_STATE_CONF;
            nadi_entries = g_saved_nadi_entries;
            p_vd_utbc_nadi = gp_savedVehDescr;
            set_uic_params( p_vd_utbc_nadi, nadi_entries,
                            pNodesInfo, actual_state );
            /* own node can't be included */
            if( !integrate_my_node( pNodesInfo ) ){
                u_res = UIC_S_INSERT_ERR;
            }
            else{
                update_dynamic_idata( pNodesInfo, nadi_entries );
            }
            break;

     
        /**********/
        /* can never occur, but looks nicer */
        default:
            set_own_state_params( TRUE, FALSE, FALSE );
            break;
    
    } /* end switch */

    
    if( (actual_state == ST_SAVED_NADI) || 
        (actual_state == ST_PROCESS_UNKNOWN_ND) ||
        (actual_state == ST_SINGLE_ND_SAV) )
    {
        if( nodes_are_insert() ){
            utbc_trace_event( EVENT_MND_LATE_INSERT );
            p_g_descr->add_on_info |= NODE_ACTIVE_BITMASK;
        }
        if( nodes_are_lost() ){
            utbc_trace_event( EVENT_NODES_LOST );
            p_g_descr->add_on_info |= NODE_LOSS_BITMASK;
        }
    }


    /* orientation of tcn-master to uic-main direction */
    if( g_rno_dir == RNO_DIR_BOTTOM_TOP ){
        p_g_descr->add_on_info |= ORIENT_MASTER_UICMAIN_SAME; 
    }
    else{
        p_g_descr->add_on_info &= (UNSIGNED8)(~ORIENT_MASTER_UICMAIN_SAME);
    }

    /* global description of NADI */
    p_g_descr->uic_ver = pNodesInfo->uic_ver;
    p_g_descr->uic_subver = pNodesInfo->uic_subver;
    p_g_descr->nadi_state = nadi_state;
    p_g_descr->num_entries = nadi_entries;
    p_g_descr->topo_counter = pNodesInfo->topo_counter;
    p_g_descr->reserved = 0;
    memcpy( p_g_descr->not_av_veh, g_na_veh, TFR_LEN_NA_VEH );

    /* pointer to vehicle-descriptions */
    p_nadi->p_veh_descr = p_vd_utbc_nadi;


exit_read_cfg:

    /* reset redundancy-bit, if set */
    if( g_standby_start ){
        g_standby_start = FALSE;
        li_t_ChgInaugDataByte( TFR_OFFSET_TO_ST_CTRL_INFO,
                (UNSIGNED8)(~UI_REDUNDANCY_BIT), UI_REDUNDANCY_BIT );
    }

    /* reset delete-bit, if set */
    if( g_cfg_delbit_mynode ){
        g_cfg_delbit_mynode = FALSE;
        li_t_ChgInaugDataByte( TFR_OFFSET_TO_ST_CTRL_INFO,
                (UNSIGNED8)(~UI_DEL_CC), UI_DEL_CC );
    }

    return u_res;

}


#ifdef O_KFT
/*******************************************************/
void process_kft_bf( UIC_VehDescr *p_vd, 
                     int num_entr,
                     NodesState actual_state )
{
    int k, r1, r2;
    int kft_bf_len;
    TYPE_UIC_ID id_use;
    BOOLEAN kftbf_chng;

    r1 = r2 = INVALID_UIC_ADDRESS;
    kftbf_chng = FALSE;

    /* NADI was corrected */
    if( (actual_state == ST_CORRECT_CFG) ||
        (actual_state == ST_SINGLE_ND_SET) )
    {
        for( k = 0; k < num_entr; k++ ){
             id_use = p_vd[k].VehDescr.uic_id;
             if( UIC_ID_CMP( &id_use, &corr_id_first ) == 0 ){
                r1 = p_vd[k].RangeNo;
             }
             if( UIC_ID_CMP( &id_use, &corr_id_last ) == 0 ){
                r2 = p_vd[k].RangeNo;
             }
        }

        if( (r1 != corr_rno_first) || (r2 != corr_rno_last) ){
             kftbf_chng = TRUE; 
        }
    }
    /* NADI will have saved state */
    if( (actual_state == ST_SAVED_NADI) ||
        (actual_state == ST_SINGLE_ND_SAV) ||
        (actual_state == ST_PROCESS_UNKNOWN_ND) )
    {
        kftbf_chng = g_rnochng;
    }

    /* KFT bitfield must be changed */
    if( kftbf_chng ){
        kft_bf_len = bit_len( g_kft_bf );
        bit_swap( g_kft_bf, kft_bf_len );
    }

    /* write KFT bitfield into NADI */
    for( k = 0; k < num_entr; k++ ){
         memcpy( &p_vd[ k ].TSInfo[ START_BS48 ], g_kft_bf, LEN_TCI );
    }

    /* write KFT bitfield into IDTS */
    for( k = 0; k < LEN_TCI; k++ ){
         li_t_ChgInaugDataByte( (UNSIGNED8)(TFR_OFFSET_TO_TS_INFO + START_BS48 + k),
                                 g_kft_bf[k], 0xFF );
    }

    return;
}
#endif



/*******************************************************/
void set_uic_params( UIC_VehDescr *p_vd,
                     int nadi_entries,
                     UI_NODES_Info *p_NodesInfo,
                     NodesState actual_state )
{
    UNSIGNED8 last_direc;

    switch( actual_state ){

    /******************/
    case ST_CORRECT_CFG:
    case ST_SINGLE_ND_SET:
         
        /* look for leading vehicle using all NADI entries */
        determine_ld_veh( p_vd, nadi_entries, TRUE );
        
        last_direc = g_rno_dir;

        /* if NADI was corrected, apply rule a) - d) */
        calculate_rnodir( p_NodesInfo, p_vd, nadi_entries, FROM_LD_POS );
        
        /* check, if the direction has changed */
        if( last_direc != g_rno_dir ) g_rnochng = TRUE;
        if( g_rnochng ){
            utbc_trace_event( EVENT_RNODIR_CHANGED );
            if( g_rno_dir == RNO_DIR_BOTTOM_TOP ){
                change_to_descend( p_vd, nadi_entries );
            }
            else{
                change_to_ascend( p_vd, nadi_entries );
            }
        }
        
        break;

    /*****************/
    case ST_SAVED_NADI:
    case ST_SINGLE_ND_SAV:
    case ST_PROCESS_UNKNOWN_ND:

        /* look for leading vehicle using valid NADI entries */
        determine_ld_veh( p_vd, nadi_entries, FALSE );
         
        /* check, if direction must be changed */
        g_rnochng = rnodir_changed( g_rno_dir, nadi_entries );
        if( g_rnochng ){
            utbc_trace_event( EVENT_RNODIR_CHANGED );
            if( g_rno_dir == RNO_DIR_BOTTOM_TOP ){
                change_to_descend( p_vd, nadi_entries );
            }
            else{
                change_to_ascend( p_vd, nadi_entries );
            }
        }

        break;

    /******/
    default:
        break;

    }


#ifdef O_KFT
    process_kft_bf( p_vd, nadi_entries, actual_state );
#endif

    /* not available vehicle bitfield */
    update_na_veh( p_NodesInfo );
    
    /* orientation to uic 01 */
    set_uicmain_orient( p_vd, nadi_entries );

    /* property 'leading vehicle' */
    set_leading_property( p_vd, nadi_entries );

}



/******************************************************/
void update_na_veh( UI_NODES_Info *p_NodesInfo )
{
    BITSET8 inv_na_veh[ TFR_LEN_NA_VEH ];
    int k, j;

    if( g_rnochng )
    {
        memset( inv_na_veh, 0, TFR_LEN_NA_VEH );

        j = p_NodesInfo->nb_veh_saved - 1;

        for( k = 0; k < p_NodesInfo->nb_veh_saved; k++ ){
            inv_na_veh[k/8] |= (g_na_veh[j/8] & (1<<(j%8))) ? (1<<(k%8)) : 0;
            j--;
        }
        memcpy( g_na_veh, inv_na_veh, TFR_LEN_NA_VEH );
    }

}



/******************************************************/
void update_dynamic_idata( UI_NODES_Info *p_NodesInfo,
                           UNSIGNED8 nadi_entries )
{
    UNSIGNED8 my_new_rno;
    int k;

    my_new_rno = 0;

    for( k = 0; k < nadi_entries; k++ ){
        /* new range number for my vehicle */
        if( UIC_ID_CMP( &gp_savedVehDescr[ k ].VehDescr.uic_id,
                        &g_own_uicid ) == 0 ){
            my_new_rno = gp_savedVehDescr[ k ].RangeNo;
        }
    }

    if( my_new_rno == 0 ){
        utbc_trace_event( EVENT_OWN_ID_NOTFOUND_U );
    }

    /* write not available vehicles into IDTS */
    for( k = 0; k < TFR_LEN_NA_VEH; k++ ){
         li_t_ChgInaugDataByte( (UNSIGNED8)(TFR_OFFSET_TO_NA_VEH + k), g_na_veh[k], 0xFF );
    }

    li_t_ChgInaugDataByte( TFR_OFFSET_TO_NUM_VEH, p_NodesInfo->nb_veh_saved, 0xFF );

    /* update own range number */
    li_t_ChgInaugDataByte( TFR_OFFSET_TO_RNO, my_new_rno, 0xFF );

    /* own node can be included */
    set_own_state_params( FALSE, FALSE, TRUE );

}


/*******************************************************/
void reset_dynamic_idata( void )
{
    UNSIGNED8 k;
    
    memset( g_na_veh, 0, TFR_LEN_NA_VEH );
    memset( g_u_nd_data, 0, sizeof( g_u_nd_data ) ); 

    /* reset bitfield of not available vehicles */
    for( k = 0; k < TFR_LEN_NA_VEH; k++ ){
        li_t_ChgInaugDataByte( (UNSIGNED8)(TFR_OFFSET_TO_NA_VEH + k), 0x00, 0xFF );
    }

    /* reset corrected number vehicles */
    li_t_ChgInaugDataByte( TFR_OFFSET_TO_NUM_VEH, 0x00, 0xFF );

    /* reset own range number */
    li_t_ChgInaugDataByte( TFR_OFFSET_TO_RNO, 0x00, 0xFF );

    /* set own state to 'unknown' */
    set_own_state_params( TRUE, FALSE, FALSE );

}



/*******************************************************/
void init_idata( TYPE_InaugData *p_idata )
{
    
    int ix, ncv;
    UNSIGNED8 *pData;
    UNSIGNED8 *pBase;
    
    pData = pBase = &p_idata->Data[ 0 ];
    
    p_idata->UIC_Ind = g_own_cfg.uic_ind;
    p_idata->UIC_Ver = g_own_cfg.uic_version;
    *pData++ = g_own_cfg.veh_management;
    *pData++ = g_own_cfg.veh_owner;
    *pData++ = g_own_cfg.applicationID;
    *pData++ = g_own_cfg.national_version;
    *pData++ = g_own_cfg.num_of_controlled_veh;
    *pData++ = UI_CONF_UNKNOWN;  /* initial node-state */
    *pData++ = 0;  /* 'RangeNo' */
    *pData++ = 0;  /* 'num_veh' */

    /* 'not available vehicles' */
    memset( pData, 0, TFR_LEN_NA_VEH );
    pData += TFR_LEN_NA_VEH;
    
    /* UIC minor version */
    *pData++ = g_own_cfg.uic_subversion;   

    /* 'trainset info' */
    memcpy( pData, &g_own_cfg.ts_attr[ 0 ], TFR_LEN_TS_INFO );
    pData += TFR_LEN_TS_INFO;
    
    /* reserved */
    *pData++ = 0;   

    /* nb. controlled vehicles < 0 */
    if( g_own_cfg.num_of_controlled_veh & MULTI_GW_MASK ) ncv = 1;
    else ncv = g_own_cfg.num_of_controlled_veh;
    
    /* for max. 6 controlled vehicles */
    /**********************************/
    for( ix = 0; ix < ncv; ix++ ){
        
        /* 'UIC-ID' */
        memcpy( pData, &g_own_cfg.veh_conf[ ix ].uic_number[ 0 ], TFR_LEN_UIC_ID );
        pData += TFR_LEN_UIC_ID;

        /* 'veh_info' */
        memcpy( pData, &g_own_cfg.veh_conf[ ix ].veh_attr[ 0 ], TFR_LEN_VEH_INFO );
        pData += TFR_LEN_VEH_INFO;
        
        /* 'veh_res_num' */
        *pData++ = 0;
        *pData++ = 0;
        
    }

    /* finally set size of frame */
    p_idata->size = pData - pBase;

    return;

}




/*******************************************************/
TYPE_UIC_ID my_uicid_from_cfg( ums_NodeConfigurationTable* p_cfg )
{
     ums_VehicleConfiguration* p_vc;
     TYPE_UIC_ID my_uic_id;
     UNSIGNED8 ncv;
     int ix;

     SETINVALID_UIC_ID( &my_uic_id );

     /* nb. controlled vehicles < 0 */
     ncv = p_cfg->num_of_controlled_veh;
     if( ncv & MULTI_GW_MASK ) ncv = 1;
     
     for( ix = 0; ix < ncv; ix++ ){

         p_vc = &p_cfg->veh_conf[ix];

         /* 'vehicle contains gateway' */
         if( p_vc->veh_attr[ VEH_EQ_GW_OFFSET_BYTE ] & VEH_EQ_GW_BITMASK ){
             my_uic_id = *(TYPE_UIC_ID*)(&p_vc->uic_number[0]);
             break;
         }
     }
     return my_uic_id;
}



/*******************************************************/
UNSIGNED8 my_range( TYPE_UIC_ID* p_own_uicid,
                    UNSIGNED8  own_tcn,
                    UNSIGNED8* p_own_index,
                    UNSIGNED8* p_own_ld )
{
    UNSIGNED8 num, ix;
    UNSIGNED8 range, ncv;
    BOOLEAN ld_flag;
    UIC_VehDescr* p_vd;
    ums_VehicleConfiguration* p_vc;

    num = g_utbc_nadi.global_descr.num_entries;
    *p_own_ld = 0;
    range = 0;
    ld_flag = FALSE;

    /* leading veh. controlled by my node */
    ncv = g_own_cfg.num_of_controlled_veh;
    if( ncv & MULTI_GW_MASK ) ncv = 1;
     
    for( ix = 0; ix < ncv; ix++ ){
        p_vc = &g_own_cfg.veh_conf[ix];
        if( UIC_ID_CMP( &p_vc->uic_number[0], &g_ld_uicid ) == 0 ){
            ld_flag = TRUE;
        }
    }

    for( ix = 0; ix < num; ix++ ){
        p_vd = &g_utbc_nadi.p_veh_descr[ix];
        if( UIC_ID_CMP( p_own_uicid, &(p_vd->VehDescr.uic_id.uic_id[0]) ) == 0 ){
            *p_own_index = ix;
            range = p_vd->RangeNo;
        }
        if( ld_flag && (p_vd->veh_add_on & LD_REQ_BITMASK) &&
            (p_vd->veh_add_on & LD_VEH_BITMASK) && 
            (p_vd->NodeAddr == own_tcn) ){
            *p_own_ld = 1;
        }
    }

    return range;

}



/*******************************************************/
void sort_vdescr( UIC_VehDescr *pVD,
                  UNSIGNED8 num_entries,
                  UNSIGNED8 nb_nodes )
{

    UIC_VehDescr veh_d_tmp;
    UIC_VehDescr *p_begin;
    UIC_VehDescr *p_end;
    UIC_VehDescr *pVDtmp;
    int iy, m, count;
    int tmp1, tmp2;
    int work;

    /* sort vehicle descr. from top-address to bottom address */
    /* we use here the simple bubble sort algorithm */
    for( iy = 0; iy < num_entries - 1; iy++ ){
        for( m = num_entries - 1; m > iy; m-- ){
                if( pVD[m].NodeAddr > 32 ) tmp1 = pVD[m].NodeAddr-63;
                else tmp1 = pVD[m].NodeAddr;
                if( pVD[m-1].NodeAddr > 32 ) tmp2 = pVD[m-1].NodeAddr-63;
                else tmp2 = pVD[m-1].NodeAddr;
                             
                if( tmp1 > tmp2 ){
                     veh_d_tmp = pVD[m-1];
                     pVD[m-1] = pVD[m];
                     pVD[m] = veh_d_tmp;
                }
            }
    }


   /* each trainset announces it UIC-IDs from direction 1 to direction 2 */
   /* of its gateway. Before calculating UIC range numbers it may be     */
   /* necessary to sort the vehicle descriptions the other way round     */
   /* within one trainset depending on its orientation to the TCN master */

   if( nb_nodes > 0 )
   {
       count = 1;
       pVDtmp = &pVD[ num_entries-1 ];
       
       /* first set work to bottom address */
       work = pVDtmp->NodeAddr;
       p_begin = pVDtmp;

       for( iy = 0; iy < nb_nodes; iy++ )
       {
               while( (pVDtmp->NodeAddr == work) && (count < num_entries) ){
                        pVDtmp--; count++;
               }
               /* last node */
               if( iy == nb_nodes - 1 ) p_end = pVDtmp;
               else p_end = pVDtmp + 1;
   
               /* swap vehicles of each trainset, whoose */
               /* orientation is same to TCN master */
               if( (p_end->veh_add_on & ORIENT_TO_TCN_MASTER) == ORIENT_TO_TCN_MASTER_SAME){
                    while( p_begin - p_end > 0 ){ 
                            veh_d_tmp = *p_begin;
                            *p_begin = *p_end;
                            *p_end = veh_d_tmp;
                            p_end++; p_begin--;
                    }
               }
               work = pVDtmp->NodeAddr;
               p_begin = pVDtmp;
       } 
   }
   return;
}


/*******************************************************/
NodesState get_nodes_state( UI_NODES_Info *p_NodesInfo )
{
    
    UNSIGNED8 cnt_cmp;
    UNSIGNED8 n_top, n_bot;
    BOOLEAN c_flag;

    /* number of saved vehicles excluding inserted leaks */
    cnt_cmp = p_NodesInfo->nb_veh_saved - cnt_ins_veh();
   
    /* do first checks */
    /*******************/
    if( (p_NodesInfo->del_cc) || 
        (p_NodesInfo->corr_info_corrupted) ||
        (p_NodesInfo->nb_nodes_cu == p_NodesInfo->nb_nodes) )
    {
        return ST_ACFG;
    }

    /* single node mode */
    /********************/
    if( p_NodesInfo->single_node_mode )
    {
        if( p_NodesInfo->nb_nodes_cu )
        {
            return ST_ACFG;
        }
        else if( p_NodesInfo->nb_nodes_set )
        {
            return ST_SINGLE_ND_SET;
        }
        else if( p_NodesInfo->nb_nodes_saved )
        {
            return ST_SINGLE_ND_SAV;
        }
    }


    /* check, if NADI is confirmed */
    /*******************************/
    if( (p_NodesInfo->nb_nodes_saved == p_NodesInfo->nb_nodes) &&
        (p_NodesInfo->nb_veh_saved == 0) )
    {
        return ST_CONFIRM_ACFG;
    }

    /* calculate rno-direction from saved rno's */
    /********************************************/
    c_flag = calculate_rnodir( p_NodesInfo, gp_tmpVehDescr, 
                         p_NodesInfo->nb_entries, FROM_SAVED_RNO );
    
    if( !c_flag ){
       utbc_trace_event( EVENT_RNODIR_NOT_CALC );
       return ST_ACFG;
    }

    /* all nodes received correction data */
    /**************************************/
    if( p_NodesInfo->nb_nodes_set == p_NodesInfo->nb_nodes )
    {
        return ST_CORRECT_CFG;
    }

    /* detect not confirmed leaks */
    /******************************/
    if( !fill_lost_rno_tbl( p_NodesInfo ) ){
        return ST_ACFG;
    }


    /* all nodes are saved */
    /***********************/
    if( p_NodesInfo->nb_nodes_saved == p_NodesInfo->nb_nodes )
    {
        /* coupling of two saved configurations */
        if( p_NodesInfo->ct_veh_cs > cnt_cmp ){
            utbc_trace_event( EVENT_TRAIN_LTH );
            return ST_ACFG;
        }

        return ST_SAVED_NADI;
    }


    /* at least one unknown node */
    /*****************************/
    if( p_NodesInfo->nb_nodes_cu >= 1 )
    {
        n_top = p_NodesInfo->nb_entries_top_cu;
        n_bot = p_NodesInfo->nb_entries_bottom_cu;
        
        /* check for train lengthning */
        if( (n_top > top_entries_lost( p_NodesInfo )) ||
            (n_bot > bot_entries_lost( p_NodesInfo )) )
        {
            utbc_trace_event( EVENT_TRAIN_LTH );
            return ST_ACFG;
        }

        /* first build saved NADI */
        build_saved_nadi( p_NodesInfo );

        /* check each unknown node, if it must be integrated */
        /* into the saved NADI with valid or invalid UIC address */
        analyze_unknown_nodes( p_NodesInfo );

        if( process_unknown_nodes() != 0 ){
            return ST_ACFG;
        }

        return ST_PROCESS_UNKNOWN_ND;
    }

    /* default: invalid NADI */
    return ST_INVALID;

}


/*******************************************************/
void process_correction( UIC_WriteCorrection *p_correction, UIC_Msg* p_reply )
{

    UNSIGNED8 ix, iy, ncv;
    UNSIGNED8 my_new_rno;
    UNSIGNED8 cnt_corr_veh;
    UNSIGNED8 hbyte, lbyte;
    BITSET8 NaVeh[ TFR_LEN_NA_VEH ];
    CorrectionData* p_correction_data;
    UNSIGNED8 base_rn, base_id;
    UNSIGNED8 offset_rn, offset_id;
    TYPE_UIC_ID use_uic_id;
    BOOLEAN first_found;

    utbc_trace_event( EVENT_CORRECT );

    UNREFERENCED( p_reply );

    p_correction_data = (CorrectionData*)p_correction->corr_data;
    cnt_corr_veh = p_correction->num_veh;

    if( cnt_corr_veh == 0 ){
        utbc_trace_event( EVENT_CORR_VEH_ZERO );
        goto reset_correction;
    }

    SETNULL_UIC_ID( &corr_id_first );
    SETNULL_UIC_ID( &corr_id_last );
    corr_rno_first = corr_rno_last = INVALID_UIC_ADDRESS;
    first_found = FALSE;

    memset( NaVeh, 0, TFR_LEN_NA_VEH );
    my_new_rno = 0;
    
    /* set bitfield of not available vehicles */
    for( ix = 0; ix < cnt_corr_veh; ix++ ){
            
        use_uic_id = p_correction_data[ ix ].uic_id;
        
        /* inserted vehicle */
        if( (p_correction_data[ ix ].ctrl_info & CTRL_VEH_AP) == 0x00 ){
            NaVeh[ ix/8 ] |= (1<<(ix%8));
        }
        else{
            if( !first_found ){
                 corr_id_first = use_uic_id;
                 corr_rno_first = ix + 1;
                 first_found = TRUE;
            }
            else{
                 corr_id_last = use_uic_id;
                 corr_rno_last = ix + 1;
            }
        }
        /* calculate my new UIC range number in corrected configuration */
        if( UIC_ID_CMP( &use_uic_id, &g_own_uicid ) == 0 ){
            my_new_rno = ix + 1;
        }
    }

    
    /* vehicle reservation numbers */
    ncv = g_own_cfg.num_of_controlled_veh;
    if( ncv & MULTI_GW_MASK ) ncv = 1;

    base_rn = TFR_OFFSET_TO_VEH_DESCR + TFR_OFFSET_TO_VEH_RES_NUM;
    base_id = TFR_OFFSET_TO_VEH_DESCR + TFR_OFFSET_TO_UIC_ID;
    
    for( ix = 0; ix < cnt_corr_veh; ix++ ){
        for( iy = 0; iy < ncv; iy++ ){
             offset_rn = base_rn + iy*TFR_LEN_VEH_DESCR; 
             offset_id = base_id + iy*TFR_LEN_VEH_DESCR;
             
             /* UIC-ID found */
             if( UIC_ID_CMP( &p_correction_data[ ix ].uic_id,
                             &(g_own_inaug_data.Data[ offset_id ]) ) == 0 )
             {
                  hbyte = (UNSIGNED8)(p_correction_data[ ix ].veh_res_num >> 8);
                  lbyte = (UNSIGNED8)(p_correction_data[ ix ].veh_res_num & 0x00FF);
                  li_t_ChgInaugDataByte( offset_rn, hbyte, 0xFF );
                  li_t_ChgInaugDataByte( (UNSIGNED8)(offset_rn+1), lbyte, 0xFF );
              }
        }
    }

    /* own range number */
    li_t_ChgInaugDataByte( TFR_OFFSET_TO_RNO, my_new_rno, 0xFF );

    /* number of saved vehicles */
    li_t_ChgInaugDataByte( TFR_OFFSET_TO_NUM_VEH, cnt_corr_veh, 0xFF );
    
    /* not available vehicles */
    for( ix = 0; ix < TFR_LEN_NA_VEH; ix++ ){
       li_t_ChgInaugDataByte( (UNSIGNED8)(TFR_OFFSET_TO_NA_VEH + ix), NaVeh[ ix ], 0xFF );
    }

#ifdef O_KFT
    /* write KFT bitfield in ID-TS */
    for( ix = 0; ix < LEN_TCI; ix++ ){
         li_t_ChgInaugDataByte( (UNSIGNED8)(TFR_OFFSET_TO_TS_INFO + START_BS48 + ix),
                                 g_kft_bf[ix], 0xFF );
    }
#endif

    /* set state to 'UIC address set' */
    set_own_state_params( FALSE, TRUE, FALSE );

    p_reply->hdr.state_cmd = UIC_S_OK;
    return;
   

reset_correction:

    /* in case of wrong data, reset dynamic inaug.-data */
    reset_dynamic_idata();

}



/*******************************************************/
void write_vehresnum( UIC_WriteResNum* p_vehresnum, UIC_Msg* reply_msg )
{
    
    UNSIGNED8 num_veh, ix, iy; 
    UNSIGNED8 base_rn, offset_rn, ncv;
    UNSIGNED8 hbyte, lbyte;
    UNSIGNED8 base_id, offset_id;
    VehResData *p_vresdata;

    num_veh = p_vehresnum->num_veh;

    if( num_veh > g_max_entries ){
        reply_msg->hdr.state_cmd = UIC_S_PARAM_INCORRECT;
        return;
    }

    p_vresdata = (VehResData*)&(p_vehresnum->res_data[0]);

    base_rn = TFR_OFFSET_TO_VEH_DESCR + TFR_OFFSET_TO_VEH_RES_NUM;
    base_id = TFR_OFFSET_TO_VEH_DESCR + TFR_OFFSET_TO_UIC_ID;

    ncv = g_own_cfg.num_of_controlled_veh;
    if( ncv & MULTI_GW_MASK ) ncv = 1;

    for( ix = 0; ix < num_veh; ix++ ){
        for( iy = 0; iy < ncv; iy++ ){
             
             offset_rn = base_rn + iy*TFR_LEN_VEH_DESCR; 
             offset_id = base_id + iy*TFR_LEN_VEH_DESCR;
             
             /* UIC-ID found */
             if( UIC_ID_CMP( &(p_vresdata[ ix ].uic_id[0]),
                             &(g_own_inaug_data.Data[ offset_id ]) ) == 0 )
             {
                 hbyte = (UNSIGNED8)(p_vresdata[ ix ].veh_res_num >> 8);
                 lbyte = (UNSIGNED8)(p_vresdata[ ix ].veh_res_num & 0x00FF);
                 li_t_ChgInaugDataByte( offset_rn, hbyte, 0xFF );
                 li_t_ChgInaugDataByte( (UNSIGNED8)(offset_rn+1), lbyte, 0xFF );
             }
        }
    }
    reply_msg->hdr.state_cmd = UIC_S_OK;
    return;
}




/*******************************************************/
void confirm_config( void )
{
    utbc_trace_event( EVENT_CONFIRM_CFG );
    reset_dynamic_idata();
    set_own_state_params( FALSE, FALSE, TRUE );
    return;
}


/*******************************************************/
void delete_config( void )
{
    UNSIGNED8 tmp = UIC_INAUG;

    li_t_ChgInaugDataByte( TFR_OFFSET_TO_ST_CTRL_INFO, UI_DEL_CC, UI_DEL_CC );
    g_cfg_delbit_mynode = TRUE;

    /* enforce new inauguration */
    uic_inaug_enf_requ( &tmp );
    return;
}


/*******************************************************/
BOOLEAN integrate_my_node( UI_NODES_Info *p_NodesInfo )
{
    UNSIGNED8 k;
    UNSIGNED8 my_address;

    if( p_NodesInfo->my_state == UI_CONF_SAVED ){
        return TRUE;
    }

    my_address = p_NodesInfo->my_address;
    
    for( k = 0; k < MAX_CORR_VEH; k++ ){
         if( g_u_nd_data[ k ].nd_addr == my_address ){
             return g_u_nd_data[ k ].intgr;
         }
    }
    return FALSE;
}



/*******************************************************/
void analyze_unknown_nodes( UI_NODES_Info *p_NodesInfo )
{
    
    UNSIGNED8 intgr_ndx;
    UNSIGNED8 ctrl_offset;
    UNSIGNED8 k, l, nd_state;
    UNSIGNED8 nd_addr, diff;
    UNSIGNED8 n_entr, n_saved;
    UIC_VehDescr *p_vd;
    BOOLEAN include_flag;

    n_entr = p_NodesInfo->nb_entries;
    n_saved = p_NodesInfo->nb_veh_saved;

    k = l = 0;
    include_flag = FALSE;
    
    /* first check all vehicles controlled by one gateway */
    /******************************************************/
    while( k < n_entr ){

        p_vd = &gp_tmpVehDescr[ k ];
        
        nd_addr = p_vd->NodeAddr;
        ctrl_offset = p_vd->num_ctrl_veh;
        nd_state = get_st( nd_addr, n_entr );

        if( ctrl_offset & MULTI_GW_MASK ){
            k++;
            continue;
        }

        if( nd_state != UI_CONF_UNKNOWN ){
            k += ctrl_offset;
            continue;
        }

        diff = get_bounds( nd_addr, &intgr_ndx, n_entr, n_saved );

        /* default: do not include */
        include_flag = FALSE;

        /* store the node address */
        g_u_nd_data[l].nd_addr = nd_addr;

        
        /* a redundant or reinserted node must */
        /* fit exactly into the leaks          */
        /***************************************/
        if( diff == ctrl_offset )
        {
            /* redundant node */
            if( p_NodesInfo->red_switch && (nd_addr == p_NodesInfo->red_addr) )
            {
                 include_flag = TRUE;
                 utbc_trace_event( EVENT_REDSWITCH );
            }
         
            /* reinserted node */
            else if( (cnt_lost_entries() == diff) && (p_NodesInfo->nb_nodes_cu == 1) &&
                     ((p_NodesInfo->node_reinsertion) || (p_NodesInfo->new_topo_received)) &&
                     (nd_addr != p_NodesInfo->top_address) &&
                     (nd_addr != p_NodesInfo->bottom_address) )
            {
                 include_flag = TRUE;
                 utbc_trace_event( EVENT_REINSERTION );
            }
        }

        g_u_nd_data[l].intgr = include_flag;
        g_u_nd_data[l].intgr_nb = ctrl_offset;
        g_u_nd_data[l].intgr_ndx = intgr_ndx;
        g_u_nd_data[l].p_intgr = p_vd;

        l++;
        k += ctrl_offset;
    }

    return;
}



/*******************************************************/
UNSIGNED8 get_bounds( UNSIGNED8 nd_addr, 
                      UNSIGNED8 *p_intgr_ndx,
                      UNSIGNED8 n_entr,
                      UNSIGNED8 n_saved )
{
    UIC_VehDescr *p_vd_t;
    UIC_VehDescr *p_vd_b;
    UNSIGNED8 start, end;
    UNSIGNED8 nd_state, k, diff;
    UNSIGNED8 bound_top, bound_bot;
    UNSIGNED8 adapt_ndx;
    int offset, rno_chck;
    int ndx_top, dummy;

    /* pointer to first entry (top) */
    p_vd_t = &gp_tmpVehDescr[ 0 ];
    /* pointer to last entry (bottom) */
    p_vd_b = &gp_tmpVehDescr[ n_entr-1 ];
    
    /* default boundary values */
    if( g_rno_dir == RNO_DIR_BOTTOM_TOP ){
        bound_bot = 0;
        bound_top = n_saved + 1;
    }
    else{
        bound_top = 0;
        bound_bot = n_saved + 1;
    }

    ndx_top = -1;

    /* beginning from top ... */
    for( k = 0; k < n_entr; k++ ){
        /* break, if input address found */
        if( p_vd_t->NodeAddr == nd_addr ) break;
        nd_state = get_st( p_vd_t->NodeAddr, n_entr );
        if( nd_state == UI_CONF_SAVED ){
            bound_top = get_saved_rno( &p_vd_t->VehDescr.uic_id, &ndx_top );
        }
        p_vd_t += 1;
    }

    /* beginning from bottom ... */
    for( k = 0; k < n_entr; k++ ){
        /* break, if input address found */
        if( p_vd_b->NodeAddr == nd_addr ) break;
        nd_state = get_st( p_vd_b->NodeAddr, n_entr );
        if( nd_state == UI_CONF_SAVED ){
            bound_bot = get_saved_rno( &p_vd_b->VehDescr.uic_id, &dummy );
        }
        p_vd_b -= 1;
    }

    /* check, if there are confirmed leaks between */
    /* bound_top and bound_bottom; if there are, we */
    /* must subtract them */
    if( bound_top > bound_bot ){
        start = bound_bot;
        end = bound_top;
        offset = -1;
    }
    else{
        start = bound_top;
        end = bound_bot;
        offset = 1;
    }

    diff = end - start - 1;

    
    adapt_ndx = 0;
    rno_chck = bound_top;

    /* adapt integrate index */
    for( k = 0; k < diff; k++ ){
        rno_chck += offset;
        if( (g_na_veh[(rno_chck-1)/8] & (1<<((rno_chck-1)%8))) == 0 ) break;
        adapt_ndx++;
    }

    
    /* ignore confirmed leaks beginning from start */
    for( k = start; k < end-1; k++ ){
        if( g_na_veh[k/8] & (1<<(k%8)) ) diff--;
    }


    /* calculate integrate index */
    *p_intgr_ndx = ndx_top + adapt_ndx + 1;

    return diff;
}


/*******************************************************/
UTBC_RESULT  utbc_set_leading_req( UNSIGNED8 set_leading, UNSIGNED8 direc )
{

    if( set_leading == UIC_LEADING ){
        li_t_ChgInaugDataByte( TFR_OFFSET_TO_ST_CTRL_INFO, UI_DEMAND_LDSHP, UI_DEMAND_LDSHP );
        li_t_ChgInaugDataByte( TFR_OFFSET_TO_ST_CTRL_INFO, 
                               ( direc == UIC_DIR2 ) ? UI_DIR_LD_DEMAND : 0,
                               UI_DIR_LD_DEMAND );
    }
    else{
        li_t_ChgInaugDataByte( TFR_OFFSET_TO_ST_CTRL_INFO,
                       (UNSIGNED8)(~UI_DEMAND_LDSHP), UI_DEMAND_LDSHP );
        li_t_ChgInaugDataByte( TFR_OFFSET_TO_ST_CTRL_INFO,
                       (UNSIGNED8)(~UI_VEH_LEADING), UI_VEH_LEADING );
    }

    return UIC_S_OK;
}



/*****************************************************/
void get_utbc_params( UNSIGNED8 *params )
{
    if( params == NULL ){
        return;
    }

    memcpy( params, g_report_data, LEN_REPORT_DATA );

}

/*****************************************************/
void set_own_state_params( BOOLEAN conf_unknown,
                           BOOLEAN uicaddr_set,
                           BOOLEAN conf_saved )
{
    
    UNSIGNED8 ctrlinfo_byte;
    UNSIGNED8 bytemask;

    ctrlinfo_byte = 0;
    bytemask = UI_CONF_UNKNOWN | UI_UIC_ADDR_SET | UI_CONF_SAVED;

    if( conf_unknown ) ctrlinfo_byte = UI_CONF_UNKNOWN;
    if( uicaddr_set ) ctrlinfo_byte |= UI_UIC_ADDR_SET;
    if( conf_saved ) ctrlinfo_byte |= UI_CONF_SAVED;

    li_t_ChgInaugDataByte( TFR_OFFSET_TO_ST_CTRL_INFO,
                           ctrlinfo_byte,
                           bytemask );

}


#ifdef O_KFT

/*****************************************************/
void write_tci( UNSIGNED8 *p_byte_in, UNSIGNED8 num_veh )
{
    UNSIGNED8 start_tci;
    
    start_tci = BYTESTREAM_SIZE_UIC_Header + 8*num_veh + 1;
    memcpy( g_kft_bf, &p_byte_in[ start_tci ], LEN_TCI );
}

#endif



/****************** MONITOR & EVENT *****************************/
void utbc_trace_event( UNSIGNED8 u_event )
{
    g_utbc_events[ g_event_index++ ] = u_event;

    if( g_ev_entries < 0xFF ){
        g_ev_entries++;
    }
    
    if( g_event_index >= 16 ){
        g_event_index = 0;
    }
}


/****************************************************************/
void utbc_print_event( UNSIGNED8 u_event, char *p_buf )
{
    sprintf( p_buf, "tc: %d", g_utbc_nadi.global_descr.topo_counter );
    
    switch( u_event )
    {
    case EVENT_RED_IND:
        sprintf( p_buf, "redundance switch\n" );
        break;
    case EVENT_CORRECT:
        sprintf( p_buf, "correction\n" );
        break;
    case EVENT_ALLOC_ERR:
        sprintf( p_buf, "allocation error\n" );
        break;
    case EVENT_CORR_VEH_ZERO:
        sprintf( p_buf, "correction error: num. saved veh == 0\n" );
        break;
    case EVENT_OWN_ID_NOTFOUND_C:
        sprintf( p_buf, "correction error: own uic-id not found\n" );
        break;
    case EVENT_OWN_ID_NOTFOUND_U:
        sprintf( p_buf, "update error: own uic-id not found\n" );
        break;
    case EVENT_CONFIRM_CFG:
        sprintf( p_buf, "confirmation\n" );
        break;
    case EVENT_RNODIR_NOT_CALC:
        sprintf( p_buf, "error: rno-direction could not be calculated\n" );
        break;
    case EVENT_RNODIR_CHANGED:
        sprintf( p_buf, "rno-direction has changed\n" );
        break;
    case EVENT_TR_LD_EXISTS:
        sprintf( p_buf, "traction leader is defined\n" );
        break;
    case EVENT_NB_ND_GR_MAX:
        sprintf( p_buf, "error: num. nodes > max. defined nodes\n" );
        break;
    case EVENT_VER_CONFLICT:
        sprintf( p_buf, "error: version conflict\n" );
        break;
    case EVENT_MAX_ENTR_ERR:
        sprintf( p_buf, "error: num. entries > max. defined entries\n" );
        break;
    case EVENT_UMSCFG_INVALID:
        sprintf( p_buf, "error: ums-configuration == NULL\n" );
        break;
    case EVENT_CINFO_CORRUPTED:
        sprintf( p_buf, "error: correction info corrupted\n" );
        break;
    case EVENT_REINSERTION:
        sprintf( p_buf, "node reinsertion\n" );
        break;
    case EVENT_REDSWITCH:
        sprintf( p_buf, "redundance switch\n" );
        break;
    case EVENT_NODES_LOST:
        sprintf( p_buf, "node(s) are lost\n" );
        break;
    case EVENT_MND_LATE_INSERT:
        sprintf( p_buf, "node(s) inserted with invalid UIC address\n" );
        break;
    case EVENT_TOPO_ERR:
        sprintf( p_buf, "error: ls_t_GetTopography() failed\n" );
        break;
    case EVENT_LD_ID_NOTFOUND_C:
        sprintf( p_buf, "corr.: ld. uic id not found\n" );
        break;
    case EVENT_NEW_INAUG:
        sprintf( p_buf, "**** new inaug. ****\n" );
        break;
    case EVENT_TRAIN_LTH:
        sprintf( p_buf, "train lengthning\n" );
        break;
    case EVENT_EQ_GWBIT_NOTSET:
        sprintf( p_buf, "error: no gw-bit defined for some node!\n" );
        break;
    case EVENT_ZERO_CV:
        sprintf( p_buf, "error: saved node with ctrl. veh = 0\n" );
        break;
    default:
        break;
    }
}

#ifdef O_166
#pragma FLOAT64
#endif

/****************************************************************/
void PrintNADI( void )
{
    int ix;
    int tmp_ndx;
    char buffer[ 128 ];
    UIC_VehDescr *p_vd;
    UIC_GlobalDescr*  p_g_descr;

    p_g_descr = &g_utbc_nadi.global_descr;

    printf( "\n--------- N A D I content ---------------\n" ); 
    SRVWDT();
    printf( "-----------------------------------------\n" ); 
    SRVWDT();
    printf( "UIC Version: %02x\n", p_g_descr->uic_ver ); 
    SRVWDT();
    printf( "UIC Subversion: %02x\n", p_g_descr->uic_subver );
    SRVWDT();
    printf( "status: %d \n", p_g_descr->nadi_state );
    SRVWDT();
    printf( "num entries: %d \n", p_g_descr->num_entries );
    SRVWDT();
    printf( "orientation master to uic-main direction: %s\n",
            ( p_g_descr->add_on_info & 0x01 ) ? "same" : "invers" );
    SRVWDT();

    printf( "not available vehicles: %02x %02x %02x %02x\n",
             p_g_descr->not_av_veh[0],
             p_g_descr->not_av_veh[1],
             p_g_descr->not_av_veh[2],
             p_g_descr->not_av_veh[3] );

    SRVWDT();
    printf( "-----------------------------------------\n" );

    printf( "UIC-ID:      (raw hex       )     TCN  UIC  r l u t\n");
    SRVWDT();
    for( ix = 0; ix < p_g_descr->num_entries; ix++ )
    {
        double d_uid = 0.0;
        int i;
        p_vd = &g_utbc_nadi.p_veh_descr[ix];
        for (i = 0; i < 5; i++) {
            d_uid = (d_uid * 256.0) + (double)p_vd->VehDescr.uic_id.uic_id[i];
        }
        
        SRVWDT();
        printf( "%012.0f (%02X %02X %02X %02X %02X)     %3d   %02d",
                d_uid,
                p_vd->VehDescr.uic_id.uic_id[0],
                p_vd->VehDescr.uic_id.uic_id[1],
                p_vd->VehDescr.uic_id.uic_id[2],
                p_vd->VehDescr.uic_id.uic_id[3],
                p_vd->VehDescr.uic_id.uic_id[4],
                p_vd->NodeAddr,
                p_vd->RangeNo);

        SRVWDT();
        if(p_vd->NodeAddr  == 0x7F ) {
            printf("  x x x x");
        }
        else {
            printf("  %d %d %d %d",
                   ( p_vd->veh_add_on & 0x08 ) != 0,
                   ( p_vd->veh_add_on & 0x04 ) != 0,
                   ( p_vd->veh_add_on & 0x02 ) != 0,
                   ( p_vd->veh_add_on & 0x01 ) != 0);
        }

        printf( "\n" );

    }
    SRVWDT();
    printf( "-----------------------------------------\n" );

    
    /* buffer is not full */
    if( g_ev_entries <= 16 ){
        tmp_ndx = 0;
    }
    else{
        tmp_ndx = g_event_index;
    }
    
    /* output event entries */
    for( ix = 0; ix < 16; ix++ ){
        if( g_utbc_events[ tmp_ndx ] ){
            utbc_print_event( g_utbc_events[ tmp_ndx ], buffer );
            printf( buffer );
            SRVWDT();
        }
        tmp_ndx++;
        if( tmp_ndx >= 16 ) tmp_ndx = 0;
    }
    printf( "-----------------------------------------\n" );
}


/*************************************************************************************/
