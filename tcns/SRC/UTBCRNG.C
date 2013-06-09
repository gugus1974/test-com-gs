/*************************************************************************
**        Copyright (C) Siemens AG 1997 All Rights Reserved. Confidential    
**========================================================================
**
**    PROJECT::    TCN Train Communication Network
                   JDP Joint Development Project

**    MODULE::     range.c  utility functions to calculate range-numbers 

**    WORKFILE::   $Workfile:   UTBCRNG.C  $
**************************************************************************
**    TASK::       UIC-Train Bus Configurator

**************************************************************************
**    AUTHOR::     Hilbrich ()  
**    CREATED::    20.10.97
**========================================================================
**    HISTORY::    AUTHOR::   $Author:   HEINDEL_AR  $
          REVISION:: $Revision:   1.78  $  
          MODTIME::  $Date:   14 Dec 1998 16:46:20  $
      $Log:   O:\PRJ\JDP\FUE\SW\PVCS\ARC\TCN_FW\UIC\SRC\UTBCRNG.Cv_  $
 * 
 *    Rev 1.78   14 Dec 1998 16:46:20   HEINDEL_AR
 * process_unknown_nodes() signals  overflow
 * 
 *    Rev 1.77   14 Dec 1998 10:01:16   HILBRICH_KL
 * additional parameter 'rno' in function 'invalidate_entry()' defined
 * 
 *    Rev 1.76   08 Dec 1998 15:26:46   HILBRICH_KL
 * process_single_node(): KEIL warning in statement *p++ = *q++
 * removed
 * 
 *    Rev 1.75   01 Dec 1998 15:53:16   HILBRICH_KL
 * check for  s t r i c t  monotony of range-numbers
 * 
 *    Rev 1.74   27 Nov 1998 13:00:24   HILBRICH_KL
 * single node mode: remove irrelevant entries with uic = 0 and tcn = 127
 * 
 *    Rev 1.73   17 Nov 1998 16:41:28   HILBRICH_KL
 * bug fixed in 'process_unknown_nodes()'
 * 
 *    Rev 1.72   12 Nov 1998 12:22:26   HILBRICH_KL
 * read ld. direc from IDTS, if own veh. is leading
 * 
 *    Rev 1.71   06 Nov 1998 16:50:32   HILBRICH_KL
 * swap NADI entries, if single node was invers oriented to master before
 * 
 *    Rev 1.70   06 Nov 1998 12:00:26   HILBRICH_KL
 * actualize entries for single node NADI
 * 
 *    Rev 1.69   05 Nov 1998 17:06:50   HILBRICH_KL
 * single node mode as separate state
 * 
 *    Rev 1.68   04 Nov 1998 14:44:52   HILBRICH_KL
 * modifications in calculate_rnodir()
 * 
 *    Rev 1.67   03 Nov 1998 10:23:16   HILBRICH_KL
 * bug fixed in 'process_unknown_nodes()'
 * 
 *    Rev 1.66   30 Oct 1998 17:43:18   HILBRICH_KL
 * temporary solution modified
 * 
 *    Rev 1.65   30 Oct 1998 17:12:24   HILBRICH_KL
 * single node mode correction
 * 
 *    Rev 1.64   30 Oct 1998 12:17:48   HILBRICH_KL
 * bug fixed in single-node mode handling
 * 
 *    Rev 1.63   29 Oct 1998 15:54:40   HILBRICH_KL
 * correction handling: do not store last tcn-topo
 * 
 *    Rev 1.62   27 Oct 1998 12:40:42   HILBRICH_KL
 * single node mode-handling modified
 * 
 *    Rev 1.61   14 Oct 1998 15:52:14   HILBRICH_KL
 * function 'determine_ld_veh()' inserted: ignore invalid UIC addresses
 * 
 *    Rev 1.60   13 Oct 1998 11:56:06   HILBRICH_KL
 * 'build_act_nadi()' modified
 * 
 *    Rev 1.59   09 Oct 1998 14:24:34   HILBRICH_KL
 * optimization in 'set_leading_property()'
 * 
 *    Rev 1.58   08 Oct 1998 16:22:20   HILBRICH_KL
 * check for rno-dir consistency, if all nodes are saved
 * 
 *    Rev 1.57   07 Oct 1998 12:44:50   HILBRICH_KL
 * bug fixed in 'process_unknown_nodes()'
 * 
 *    Rev 1.56   06 Oct 1998 10:01:58   HILBRICH_KL
 * end node redundance-switch supported
 * 
 *    Rev 1.55   01 Oct 1998 08:56:26   HILBRICH_KL
 * leading request bit added to NADI-entries
 * 'ls_t_GetTopography()' replaced by 'uwtm_get_topo()' 
 * 
 *    Rev 1.54   29 Sep 1998 14:03:48   HILBRICH_KL
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
#ifdef O_KFT
#include "kft.h"
#endif

extern ums_NodeConfigurationTable   g_own_cfg;

extern TYPE_UIC_ID    g_own_uicid;    /* my own UIC ID */
extern TYPE_UIC_ID    g_ld_uicid;     /* leading vehicle UIC ID */
extern UNSIGNED8      g_rno_dir;      /* actual rno-direction */
extern BOOLEAN        g_rnochng;
extern UNSIGNED8      g_max_entries;  /* max entries in NADI */
extern UNSIGNED8      g_saved_nadi_entries;  /* nb. entries in saved NADI */


/* position of not available vehicles */
extern BITSET8        g_na_veh[ TFR_LEN_NA_VEH ]; 

/* nodes and states */
extern Node_and_State g_nd_st[ MAX_CORR_VEH ];

/* lost rno table */
extern Lost_Rno_Data  g_l_rno_tbl[ MAX_CORR_VEH ];

/* data for handling unknown nodes */
extern Unknown_Node_Data  g_u_nd_data[ MAX_CORR_VEH ];

extern UIC_VehDescr   *gp_savedVehDescr;
extern UIC_VehDescr   *gp_tmpVehDescr;
extern UIC_VehDescr   *gp_SavedStateVehDescr;

extern TYPE_UIC_ID    corr_id_first;
extern TYPE_UIC_ID    corr_id_last;
extern UNSIGNED8      corr_rno_first;
extern UNSIGNED8      corr_rno_last;


/***********************************************/
/* this function builds up an active NADI      */
/***********************************************/
void build_act_nadi( UI_NODES_Info *p_NodesInfo )
{
    UIC_VehDescr *p_vd;
    int act_entries;

    p_vd = gp_tmpVehDescr;
    act_entries = p_NodesInfo->nb_entries;
    /* look for leading vehicle using all entries */
    determine_ld_veh( p_vd, act_entries, TRUE );
    calculate_rnodir( p_NodesInfo, p_vd, act_entries, FROM_LD_POS );

    /* assigning range numbers */
    if( g_rno_dir == RNO_DIR_BOTTOM_TOP ){
        descend_range_act( p_vd, act_entries );
    }
    else{
        ascend_range_act( p_vd, act_entries );
    }
    
    /* uic main orientation and leading property */
    set_uicmain_orient( p_vd, act_entries );
    set_leading_property( p_vd, act_entries );
}



/***********************************************/
/* this function confirms an active NADI       */
/***********************************************/
void confirm_act_nadi( UI_NODES_Info *p_NodesInfo )
{
   int k, my_rno;

   build_act_nadi( p_NodesInfo );
   copy_act_to_saved_nadi( p_NodesInfo->nb_entries );
   my_rno = 0;

   /* number of saved vehicles */
   li_t_ChgInaugDataByte( TFR_OFFSET_TO_NUM_VEH, p_NodesInfo->ct_veh, 0xFF );

   /* find own UIC ID */
   for( k = 0; k < p_NodesInfo->nb_entries; k++ ){
       if( UIC_ID_CMP( &gp_savedVehDescr[ k ].VehDescr.uic_id, &g_own_uicid ) == 0 ){
           my_rno = gp_savedVehDescr[ k ].RangeNo;
           break;
       }
   }

   g_saved_nadi_entries = p_NodesInfo->nb_entries;
   
   /* set own range number */
   li_t_ChgInaugDataByte( TFR_OFFSET_TO_RNO, (UNSIGNED8)my_rno, 0xFF );

}


/***********************************************/
/* this function corrects an active NADI       */
/***********************************************/
void correct_act_nadi( UI_NODES_Info *p_NodesInfo )
{

   UIC_VehDescr *p_vd_use;
   UIC_VehDescr *p_vd_saved;
   int saved_entries;
   int act_entries;
   int saved_cnt;
   int rno_start;
   int ndx_bf;
   int offset;

   rno_start = 1;
   act_entries = p_NodesInfo->nb_entries;
   saved_entries = act_entries + cnt_ins_veh();
   saved_entries = MIN( saved_entries, g_max_entries );

   if( g_rno_dir == RNO_DIR_BOTTOM_TOP ){
       p_vd_saved = &gp_savedVehDescr[ saved_entries - 1 ];
       p_vd_use = &gp_tmpVehDescr[ act_entries - 1 ];
       offset = -1;
   }
   else{
       p_vd_saved = &gp_savedVehDescr[ 0 ];
       p_vd_use = &gp_tmpVehDescr[ 0 ];
       offset = 1;
   }

   ndx_bf = saved_cnt = 0;

   while( saved_cnt < saved_entries ){

       /* entry for confirmed leak ( vehicle ) */
       if( g_na_veh[ ndx_bf/8 ] & (1<<(ndx_bf%8)) ){
           invalidate_entry( p_vd_saved, rno_start );
           p_vd_saved += offset;
           saved_cnt++;
       }
       else{
           while( 1 ){
                *p_vd_saved = *p_vd_use;
                p_vd_saved->RangeNo = rno_start;
                saved_cnt++;
                p_vd_saved += offset;
                p_vd_use += offset;
                if( saved_cnt >= saved_entries ) break;
                if( !chk_same_veh( p_vd_use, offset ) ) break;
           }
       }
       ndx_bf++;
       rno_start++;
   };

   g_saved_nadi_entries = saved_entries;

}



/***********************************************/
/* this function updates a saved NADI          */
/***********************************************/
void build_saved_nadi( UI_NODES_Info *p_NodesInfo )
{
   
   UIC_VehDescr *p_vd_use;
   UIC_VehDescr *p_vd_saved;
   UNSIGNED8 tbl_ndx;
   int saved_st_entries;
   int saved_entries;
   int act_entries;
   int saved_cnt;
   int nb_av;
   int rno_start;
   int cnt_l_entr;
   int ndx_bf, offset, k;

   act_entries = p_NodesInfo->nb_entries;
   cnt_l_entr = cnt_lost_entries();

   saved_st_entries = fill_saved_state_dscr( act_entries );
   saved_entries = saved_st_entries + cnt_ins_veh() + cnt_l_entr;
   saved_entries = MIN( saved_entries, g_max_entries );

   ndx_bf = saved_cnt = 0;
   rno_start = 1;

   
   if( g_rno_dir == RNO_DIR_BOTTOM_TOP ){
       p_vd_saved = &gp_savedVehDescr[ saved_entries - 1 ];
       p_vd_use = &gp_SavedStateVehDescr[ saved_st_entries - 1 ];
       offset = -1;
   }
   else{
       p_vd_saved = &gp_savedVehDescr[ 0 ];
       p_vd_use = &gp_SavedStateVehDescr[ 0 ];
       offset = 1;
   }


   while( saved_cnt < saved_entries ){

       /* entry for confirmed leak ( vehicle ) */
       if( g_na_veh[ ndx_bf/8 ] & (1<<(ndx_bf%8)) ){
           invalidate_entry( p_vd_saved, rno_start );
           p_vd_saved += offset;
           saved_cnt++;
       }
       
       /* lost vehicle/node */
       else if( is_lost_rno( ndx_bf+1, &tbl_ndx ) )
       {
           /* vehicle with multiple gw */
           if( g_l_rno_tbl[ tbl_ndx ].nb_gw > 1 )
           {
               /* available entries */
               nb_av = g_l_rno_tbl[ tbl_ndx ].nb_gw - g_l_rno_tbl[ tbl_ndx ].nb_lost;
               for( k = 0; k < nb_av; k++ ){
                   *p_vd_saved = *p_vd_use;
                   p_vd_saved->RangeNo = rno_start;
                   p_vd_saved += offset;
                   p_vd_use += offset;
                   saved_cnt++;
               }
               /* lost entries */
               for( k = 0; k < g_l_rno_tbl[ tbl_ndx ].nb_lost; k++ ){
                   invalidate_entry( p_vd_saved, rno_start );
                   p_vd_saved += offset;
                   saved_cnt++;
               }
           }
           else{
               invalidate_entry( p_vd_saved, rno_start );
               p_vd_saved += offset;
               saved_cnt++;
           }
       }
       /* available vehicle */
       else{
           while( 1 ){
                *p_vd_saved = *p_vd_use;
                p_vd_saved->RangeNo = rno_start;
                p_vd_saved += offset;
                p_vd_use += offset;
                saved_cnt++;
                if( saved_cnt >= saved_entries ) break;
                if( !chk_same_veh( p_vd_use, offset ) ) break;
           }
       }
       ndx_bf++;
       rno_start++;
   };

   g_saved_nadi_entries = saved_entries;

}



/*******************************************************/
void process_single_node( UI_NODES_Info *p_NodesInfo )
{
    int i, i1, i2, n_entr, r_tmp;
    int nb_irrelevant_entr;
    TYPE_UIC_ID id_use;
    UIC_VehDescr *p_vd;
    UIC_VehDescr *p_vd_use;
    UIC_VehDescr *p_begin;
    UIC_VehDescr *p_end;
    UIC_VehDescr veh_d_tmp;

    n_entr = p_NodesInfo->nb_entries;

    /* single node received correction: we use two   */
    /* UIC-IDs from correction-telegram to calculate */
    /* the rno-direction                             */
    /* --------------------------------------------- */
    if( p_NodesInfo->nb_nodes_set )
    {
        for( i = 0; i < n_entr; i++ )
        {
             id_use = gp_tmpVehDescr[i].VehDescr.uic_id;
             
             if( UIC_ID_CMP( &id_use, &corr_id_first ) == 0 ){
                 i1 = i;
             }
             if( UIC_ID_CMP( &id_use, &corr_id_last ) == 0 ){
                 i2 = i;
             }
        }

        /* default direction */
        g_rno_dir = RNO_DIR_BOTTOM_TOP;
        
        if( ((corr_rno_first < corr_rno_last) && (i1 < i2)) || 
            ((corr_rno_first > corr_rno_last) && (i1 > i2)) ){
            g_rno_dir = RNO_DIR_TOP_BOTTOM;
        }
        correct_act_nadi( p_NodesInfo );
    }
    
    /* single node is already saved, so we can take   */
    /* the saved NADI calculated in last inauguration */
    /* ---------------------------------------------- */
    if( p_NodesInfo->nb_nodes_saved )
    {
        
        /* single node is always tcn-master; if it was */
        /* previously invers oriented to the master,   */
        /* swap the entries                            */
        if( my_last_orient( &g_own_uicid, g_saved_nadi_entries ) == 
              ORIENT_TO_TCN_MASTER_INVERS )
        {
            p_begin = &gp_savedVehDescr[ 0 ];
            p_end = &gp_savedVehDescr[ g_saved_nadi_entries-1  ];
            
            while( p_end - p_begin > 0 ){ 
                  veh_d_tmp = *p_begin;
                  *p_begin = *p_end;
                  *p_end = veh_d_tmp;
                  p_end--; p_begin++;
            }
        }
        
        for( i = 0; i < g_saved_nadi_entries; i++ )
        {
             p_vd = &gp_savedVehDescr[i];
             id_use = p_vd->VehDescr.uic_id;
             r_tmp = p_vd->RangeNo;

             if( !ismy_uicid( &id_use ) ){
                 /* not a confirmed leak */
                 if( (g_na_veh[ (r_tmp-1)/8 ] & (1<<((r_tmp-1)%8))) == 0 )
                 {
                     invalidate_entry( p_vd, r_tmp );
                 }
             }
             else{
                 get_act_entry( &id_use, p_vd, n_entr );
                 p_vd->RangeNo = r_tmp;
             }
        }

        /* filter out entries with rno = 0 and tcn = 127 */
        p_vd_use = &gp_savedVehDescr[0];
        nb_irrelevant_entr = 0;
        
        for( i = 0; i < g_saved_nadi_entries; i++ ){
            if( (p_vd_use->NodeAddr == INVALID_TCN_ADDRESS) &&
                (p_vd_use->RangeNo == INVALID_UIC_ADDRESS) ){
                 nb_irrelevant_entr++;
            }
            p_vd_use++;
        }

        if( nb_irrelevant_entr )
        {
            p_vd_use = &gp_savedVehDescr[0];
            p_vd = &gp_SavedStateVehDescr[0];

            for( i = 0; i < g_saved_nadi_entries; i++ ){
                if( (p_vd_use->NodeAddr != INVALID_TCN_ADDRESS) ||
                    (p_vd_use->RangeNo != INVALID_UIC_ADDRESS) ){
                     *p_vd = *p_vd_use;
                     p_vd++; p_vd_use++;
                }
                else{ 
                     p_vd_use++;
                }
            }

            g_saved_nadi_entries -= nb_irrelevant_entr;
            p_vd_use = &gp_savedVehDescr[0];
            p_vd = &gp_SavedStateVehDescr[0];
        
            for( i = 0; i < g_saved_nadi_entries; i++ ){
                 *p_vd_use = *p_vd;
                 p_vd++; p_vd_use++;
            }
        }

        calculate_rnodir( p_NodesInfo, gp_savedVehDescr, 
                          g_saved_nadi_entries, FROM_SAVED_RNO ); 

    }

}


/**********************************************************/
void get_act_entry( TYPE_UIC_ID *p_id, 
                    UIC_VehDescr *p_vd,
                    int num_entries )
{
    int k;
    UIC_VehDescr *p_vd_act;

    for( k = 0; k < num_entries; k++ )
    {
        p_vd_act = &gp_tmpVehDescr[k];

        /* entry found */
        if( UIC_ID_CMP( p_id, &p_vd_act->VehDescr.uic_id ) == 0 ){
            *p_vd = *p_vd_act;
            return;
        }
    }

}



/**********************************************************/
UNSIGNED8 my_last_orient( TYPE_UIC_ID *p_id, int num_entr )
{
    int k;

    for( k = 0; k < num_entr; k++ )
    {
        if( UIC_ID_CMP( p_id, &gp_savedVehDescr[k].VehDescr.uic_id ) == 0 ){
            return (UNSIGNED8)(gp_savedVehDescr[k].veh_add_on & ORIENT_TO_TCN_MASTER);
        }
    }

    return 0;
}



/*******************************************************/
BOOLEAN ismy_uicid( TYPE_UIC_ID *p_id )
{
    UNSIGNED8 ncv;
    int k;

    ncv = g_own_cfg.num_of_controlled_veh;
    if( ncv & MULTI_GW_MASK ) ncv = 1;

    for( k = 0; k < ncv; k++ ){
        if( UIC_ID_CMP( p_id, (TYPE_UIC_ID*)(&g_own_cfg.veh_conf[k].uic_number[0]) ) == 0 ){
            return TRUE;
        }
    }
    return FALSE;
}



/*******************************************************/
int process_unknown_nodes( void )
{
    int k, l, intgr_nb, intgr_ndx;
    int move_offset, adapt_offset;
    int nb_entries_to_move;
    UNSIGNED8 ncv;
    UIC_VehDescr *p_vd_use;
    UIC_VehDescr *p_vd_saved;
    UIC_VehDescr *p_vd_x;
    UIC_VehDescr *p_vd_y;
    int tcn_cmp_x, tcn_cmp_y;

    
    /* first all nodes that must be integrated */
    /* --------------------------------------- */
    for( k = 0; k < MAX_CORR_VEH; k++ ){
         
        if( g_u_nd_data[ k ].nd_addr && g_u_nd_data[ k ].intgr ){
             
             intgr_nb = g_u_nd_data[ k ].intgr_nb;
             intgr_ndx = g_u_nd_data[ k ].intgr_ndx;
             p_vd_use = g_u_nd_data[ k ].p_intgr;
             p_vd_saved = &gp_savedVehDescr[ intgr_ndx ];

             for( l = 0; l < intgr_nb; l++ ){
                  p_vd_use->RangeNo = p_vd_saved->RangeNo;
                  *p_vd_saved = *p_vd_use;
                  p_vd_use++;
                  p_vd_saved++;
             }
        }
    }


    /* finally nodes to insert with invalid UIC addresses */
    /* -------------------------------------------------- */
    for( k = 0; k < MAX_CORR_VEH; k++ ){

         if( g_u_nd_data[ k ].nd_addr && !g_u_nd_data[ k ].intgr ){

             intgr_nb = g_u_nd_data[ k ].intgr_nb;
             intgr_ndx = g_u_nd_data[ k ].intgr_ndx;
             tcn_cmp_x = g_u_nd_data[ k ].nd_addr;
             /* check, if position is at valid entry */
             if( intgr_ndx < g_saved_nadi_entries ){
                 tcn_cmp_y = gp_savedVehDescr[ intgr_ndx ].NodeAddr;
             }
             else{
                 tcn_cmp_y = INVALID_TCN_ADDRESS;
             }
             
             if( (g_saved_nadi_entries + intgr_nb) > g_max_entries ){
                 utbc_trace_event( EVENT_MAX_ENTR_ERR );
                 return -1;
             }

             move_offset = 0;
             adapt_offset = intgr_nb;
             
             /* look, if tcn-position is valid */
             if( tcn_cmp_y != INVALID_TCN_ADDRESS ){
                 
                 ncv = gp_savedVehDescr[ intgr_ndx ].num_ctrl_veh;
                 if( ncv & MULTI_GW_MASK ) ncv = 1;

                 if( tcn_cmp_x > 32 ) tcn_cmp_x -= 63;
                 if( tcn_cmp_y > 32 ) tcn_cmp_y -= 63;
                 if( tcn_cmp_x < tcn_cmp_y ) move_offset = ncv;
             }

             /* move entries */
             p_vd_x = &gp_savedVehDescr[ g_saved_nadi_entries - 1 ];
             p_vd_y = &gp_savedVehDescr[ g_saved_nadi_entries - 1 + intgr_nb ];
             
             /* can be < 0, if integrated at the end of the NADI */
             nb_entries_to_move = (int)g_saved_nadi_entries - intgr_ndx - move_offset;

             for( l = 0; l < nb_entries_to_move; l++ ){
                 *p_vd_y = *p_vd_x;
                  p_vd_y--;
                  p_vd_x--;
             }

             p_vd_saved = &gp_savedVehDescr[ intgr_ndx + move_offset ];
             p_vd_use = g_u_nd_data[ k ].p_intgr;

             /* fill entries with invalid UIC addresses */
             for( l = 0; l < intgr_nb; l++ ){
                 *p_vd_saved = *p_vd_use;
                  p_vd_saved->RangeNo = INVALID_UIC_ADDRESS;
                  p_vd_use++;
                  p_vd_saved++;
             }

             for( l = k+1; l < MAX_CORR_VEH; l++ ){
                 if( g_u_nd_data[ l ].nd_addr ){
                     g_u_nd_data[ l ].intgr_ndx += adapt_offset;
                 }
             }

             /* adapt number of saved NADI entries */
             g_saved_nadi_entries += intgr_nb;

         }
    }
    return 0;
}


/*****************************************************************/
void determine_ld_veh( UIC_VehDescr *p_vd,
                       int nadi_entries,
                       BOOLEAN all_entries )
{
    int nv_lr, nlv_lr;
    int ndx_v_lr, ndx_lv_lr;
    int use_ndx, k;

    nv_lr = nlv_lr = 0;

    for( k = 0; k < nadi_entries; k++ ){
        /* saved NADI: only entries with valid UIC addresses */
        if( all_entries || (p_vd[k].RangeNo != INVALID_UIC_ADDRESS) )
        {
            if( p_vd[k].veh_add_on & LD_REQ_BITMASK ){
                ndx_v_lr = k;
                nv_lr++;
                if( p_vd[k].veh_add_on & LD_VEH_BITMASK ){
                    ndx_lv_lr = k;
                    nlv_lr++;
                }
            }
        }
    }

    use_ndx = INVALID_INDEX;

    /* one vehicle requests leadership */
    if( nv_lr == 1 ){
        use_ndx = ndx_v_lr;
    }
    /* leading veh. requests leadership --> overwrite index */
    if( nlv_lr == 1 ){
        use_ndx = ndx_lv_lr;
    }

    if( use_ndx == INVALID_INDEX ) return;

    /* set UIC ID of leading vehicle */
    g_ld_uicid = p_vd[ use_ndx ].VehDescr.uic_id;

}



/*******************************************************/
BOOLEAN nodes_are_lost( void )
{
   int k, r_ndx;
   UIC_VehDescr *p_vd;

   p_vd = gp_savedVehDescr;

   for( k = 0; k < g_saved_nadi_entries; k++ ){
        /* entry with invalid tcn address found */
        if( p_vd->NodeAddr == INVALID_TCN_ADDRESS ){
            if( p_vd->RangeNo != INVALID_UIC_ADDRESS ){
                r_ndx = p_vd->RangeNo - 1;
                /* it is not a confirmed leak */
                if( (g_na_veh[r_ndx/8] & (1<<(r_ndx%8))) == 0 ){
                    return TRUE;
                }
            }
        }
        p_vd++;
   }

   return FALSE;

}


/*******************************************************/
BOOLEAN nodes_are_insert( void )
{
   UNSIGNED8 k;
   UIC_VehDescr *p_vd;

   p_vd = gp_savedVehDescr;

   for( k = 0; k < g_saved_nadi_entries; k++ ){
        if( p_vd->RangeNo == INVALID_UIC_ADDRESS ){
            return TRUE;
        }
        p_vd++;
   }

   return FALSE;

}




/*******************************************************/
BOOLEAN fill_lost_rno_tbl( UI_NODES_Info *p_NodesInfo )
{
    int k, l, m, i, ctrl_offset;
    UNSIGNED8 a_index, multi_entry;
    UNSIGNED8 rno_found, last_rno_found;
    Rno_and_CtrlVeh rno_cv[ MAX_CORR_VEH ];
    UNSIGNED8 nd_state, act_entries;
    UIC_VehDescr *p_vd_use;
    int offset;

    act_entries = p_NodesInfo->nb_entries;

    /* start index depending on rno-direction */
    if( g_rno_dir == RNO_DIR_BOTTOM_TOP ){
        p_vd_use = &gp_tmpVehDescr[ act_entries - 1 ];
        offset = -1;
    }
    else{
        p_vd_use = &gp_tmpVehDescr[ 0 ];
        offset = 1;
    }

    
    /* determination of lost range numbers */
    memset( g_l_rno_tbl, 0, sizeof( g_l_rno_tbl ) );
    memset( rno_cv, 0, sizeof( rno_cv ) );
    m = 0;

    for( l = 0; l < act_entries; l++ ){
         nd_state = get_st( p_vd_use->NodeAddr, act_entries );
         /* only for saved vehicles */
         if( nd_state == UI_CONF_SAVED ){
             rno_cv[ m ].rno = p_vd_use->RangeNo;
             rno_cv[ m ].ctrl_veh = p_vd_use->num_ctrl_veh;
             m++;
         }
         p_vd_use += offset;
    }


    k = i = 0;
    a_index = 0;
    last_rno_found = 0;
    ctrl_offset = 0;

    while( k+i < act_entries ){

        for( i = 0; i < (act_entries - k); i++ ){
            if( rno_cv[ k+i ].rno ){
                rno_found = rno_cv[ k+i ].rno;

                for( m = last_rno_found + 1; m < rno_found - i; m++ ){
                     g_l_rno_tbl[ a_index ].rno = m;
                     g_l_rno_tbl[ a_index ].nb_lost = 1;
                     g_l_rno_tbl[ a_index ].nb_gw = 1;
                     a_index++;
                }

                if( rno_cv[ k+i ].ctrl_veh & MULTI_GW_MASK ) ctrl_offset = 1;
                else ctrl_offset = rno_cv[ k+i ].ctrl_veh;

                /* should not occure! */
                if( ctrl_offset == 0 ){
                    utbc_trace_event( EVENT_ZERO_CV );
                    return FALSE;
                }

                last_rno_found = rno_found - i + ctrl_offset - 1;
                k = k + ctrl_offset;
                i = 0;
                break;
            }
        }
    }

    /* leaks at the end */
    for( k = last_rno_found + 1; k <= p_NodesInfo->nb_veh_saved; k++ ){
         g_l_rno_tbl[ a_index ].rno = k;
         g_l_rno_tbl[ a_index ].nb_lost = 1;
         g_l_rno_tbl[ a_index ].nb_gw = 1;
         a_index++;
    }

    last_rno_found = 0;
    k = 0;

    /* vehicles with multiple gateways */
    while( k < act_entries ){
         if( rno_cv[ k ].rno && (rno_cv[ k ].ctrl_veh & MULTI_GW_MASK) ){
             last_rno_found = rno_cv[ k ].rno;
             multi_entry = 0xFF - rno_cv[ k ].ctrl_veh + 1;
             for( m = k + 1; m < MIN( act_entries, k + multi_entry ); m++ ){
                  /* next rno */
                  if( rno_cv[ m ].rno != last_rno_found ){
                      g_l_rno_tbl[ a_index ].rno = last_rno_found;
                      g_l_rno_tbl[ a_index ].nb_lost = (multi_entry - (m - k) + 1);
                      g_l_rno_tbl[ a_index ].nb_gw = multi_entry;
                      a_index++;
                      break;
                  }
             }
             k = m;
         }
         else k++;
    }

    return TRUE;

}



/*******************************************************/
BOOLEAN is_lost_rno( UNSIGNED8 rno, UNSIGNED8 *p_index )
{
    int k;

    for( k = 0; k < MAX_CORR_VEH; k++ ){
        if( g_l_rno_tbl[ k ].rno == rno ){
            *p_index = k;
            return TRUE;
        }
    }
    return FALSE;
}


/*******************************************************/
UNSIGNED8 get_saved_rno( TYPE_UIC_ID *p_id, int *p_ndx )
{
    int k;
    *p_ndx = INVALID_INDEX;
    
    for( k = 0; k < g_max_entries; k++ ){
        if( UIC_ID_CMP( p_id, &gp_savedVehDescr[k].VehDescr.uic_id ) == 0 ){
            *p_ndx = k;
            return gp_savedVehDescr[k].RangeNo;
        }
    }
    return 0;
}



/*******************************************************/
void invalidate_entry( UIC_VehDescr *p_vd, UNSIGNED8 rno )
{
    memset( p_vd, 0, sizeof( UIC_VehDescr ) );
    p_vd->NodeAddr = INVALID_TCN_ADDRESS;
    p_vd->RangeNo = rno;
}


/*******************************************************/
BOOLEAN chk_same_veh( UIC_VehDescr *p_vd, int offset )
{
    TYPE_UIC_ID id_a, id_b;

    id_a = p_vd->VehDescr.uic_id;
    p_vd -= offset;
    id_b = p_vd->VehDescr.uic_id;

    /* same UIC ID */
    if( UIC_ID_CMP( &id_a, &id_b ) == 0 ){
        return TRUE;
    }
    return FALSE;
}


/*******************************************************/
UNSIGNED8 fill_saved_state_dscr( int num_entr )
{
   UNSIGNED8 state, saved_cnt;
   int k;

   saved_cnt = 0;
   
   for( k = 0; k < num_entr; k++ ){
       state = get_st( gp_tmpVehDescr[k].NodeAddr, num_entr );
       if( state == UI_CONF_SAVED ){
           gp_SavedStateVehDescr[saved_cnt] = gp_tmpVehDescr[k];
           saved_cnt++;
       }
   }
   return saved_cnt;
}


/*********************************************************/
UNSIGNED8 get_st( UNSIGNED8 tcn_addr, int num_entr )
{
   int k;

   for( k = 0; k < num_entr; k++ ){
       if( tcn_addr == g_nd_st[k].nd_addr ){
           return g_nd_st[k].state;
       }
   }
   return UI_CONF_UNKNOWN;
}


/*****************************************************/
void copy_act_to_saved_nadi( int num_entr )
{
   int i;

   for( i = 0; i < num_entr; i++ ){
       gp_savedVehDescr[i] = gp_tmpVehDescr[i];
   }
}



/*******************************************************/
BOOLEAN rnodir_changed( UNSIGNED8 old_direc,
                        UNSIGNED8 num_entr )
{
   
   int i, pos_ld_veh;
   int dist_a, dist_e;
   int tr_index;
   UIC_VehDescr *p_vd;
   TYPE_UIC_ID uic_id_cmp;
   BOOLEAN found, trac_ld_exists;

   found = FALSE;
   p_vd = gp_savedVehDescr;
   uic_id_cmp = g_ld_uicid;

   /* if no leading vehicle exists, look for traction leader */
   if( ISINVALID_UIC_ID( &uic_id_cmp ) ){
       trac_ld_exists = tractionleader_pos( p_vd, num_entr, &uic_id_cmp, &tr_index );
       if( !trac_ld_exists ){
           return FALSE;
       }
   }

   /* find position of (changed) leading vehicle in saved NADI */
   for( i = 0; i < num_entr; i++ ){
       if( UIC_ID_CMP( &uic_id_cmp, &p_vd[i].VehDescr.uic_id ) == 0 ){
           found = TRUE;
           pos_ld_veh = i;
           break;
       }
   }
   if( found ){
       dist_a = pos_ld_veh;
       dist_e = num_entr - pos_ld_veh - 1;

       /* if the leading vehicle is in the middle, */
       /* do not change the rno-direction! */
       if( dist_a == dist_e ){
           return FALSE;
       }
       
       /* direction has changed ? */
       if( ((dist_a < dist_e) && (old_direc == RNO_DIR_BOTTOM_TOP)) ||
           ((dist_a > dist_e) && (old_direc == RNO_DIR_TOP_BOTTOM)) )
       {
           if( old_direc == RNO_DIR_TOP_BOTTOM ){
               g_rno_dir = RNO_DIR_BOTTOM_TOP;
           }
           else{
               g_rno_dir = RNO_DIR_TOP_BOTTOM;
           }
           return TRUE;
       }
   }

   return FALSE;

}


/*******************************************************/
BOOLEAN tractionleader_pos( UIC_VehDescr *p_vd, int num_entr,
                            TYPE_UIC_ID *p_tr_ld_id, int *p_index )
{


    /* both vehicles are traction leaders */
    if( (p_vd[0].TSInfo[ TRACTION_LEADER_BYTE ] & TRACTION_LEADER_BITMASKS) &&
        (p_vd[num_entr-1].TSInfo[ TRACTION_LEADER_BYTE ] & TRACTION_LEADER_BITMASKS) ){
        return FALSE;
    }
    else if( p_vd[0].TSInfo[ TRACTION_LEADER_BYTE ] & TRACTION_LEADER_BITMASKS ){
        *p_tr_ld_id = *(TYPE_UIC_ID*)(&p_vd[0].VehDescr.uic_id.uic_id[0]);
        *p_index = 0;
        utbc_trace_event( EVENT_TR_LD_EXISTS );
        return TRUE;
    }
    else if( p_vd[num_entr-1].TSInfo[ TRACTION_LEADER_BYTE ] & TRACTION_LEADER_BITMASKS ){
        *p_tr_ld_id = *(TYPE_UIC_ID*)(&p_vd[num_entr-1].VehDescr.uic_id.uic_id[0]);
        *p_index = num_entr-1;
        utbc_trace_event( EVENT_TR_LD_EXISTS );
        return TRUE;
    }
    else{
        return FALSE;
    }
}



/*******************************************************/
void set_uicmain_orient( UIC_VehDescr *p_vd,
                         int nadi_entries )
{
   UNSIGNED8 *p_v_addon;
   int k;

   for( k = 0; k < nadi_entries; k++ )
   {
       if( p_vd[k].NodeAddr != INVALID_TCN_ADDRESS ){

           p_v_addon = &p_vd[k].veh_add_on;

           if( (*p_v_addon & ORIENT_TO_TCN_MASTER) == ORIENT_TO_TCN_MASTER_SAME )
           {
                if( g_rno_dir == RNO_DIR_BOTTOM_TOP )
                    *p_v_addon |= ORIENT_VEH_UICMAIN_SAME; /* same */
                else 
                    *p_v_addon &= (UNSIGNED8)(~ORIENT_VEH_UICMAIN_SAME); /* invers */
           }
           else if( (*p_v_addon & ORIENT_TO_TCN_MASTER) == ORIENT_TO_TCN_MASTER_INVERS )
           {
                if( g_rno_dir == RNO_DIR_BOTTOM_TOP )
                    *p_v_addon &= (UNSIGNED8)(~ORIENT_VEH_UICMAIN_SAME); /* invers */
                else
                    *p_v_addon |= ORIENT_VEH_UICMAIN_SAME;  /* same */
           }
       }
    }

}



/*******************************************************/
void set_leading_property( UIC_VehDescr *p_vd, 
                           int nadi_entries )
{
     
    int k, m;
    UNSIGNED8 ncv;
    BOOLEAN ld_my_cv_flag;
    TYPE_UIC_ID* p_id_cmp;
    UNSIGNED8* p_v_addon;

    ncv = g_own_cfg.num_of_controlled_veh;
    if( ncv & MULTI_GW_MASK ) ncv = 1;

    ld_my_cv_flag = FALSE;
    
    /* check, if 'id_ld_veh' is from my controlled vehicles */
    for( m = 0; m < ncv; m++ ){
        if( UIC_ID_CMP( &g_ld_uicid, (TYPE_UIC_ID*)(&g_own_cfg.veh_conf[m].uic_number[0]) ) == 0 ){
            ld_my_cv_flag = TRUE;
            break;
        }
    }

    /* for each NADI entry */ 
    for( k = 0; k < nadi_entries; k++ )
    {
        p_id_cmp = (TYPE_UIC_ID*)&(p_vd[k].VehDescr.uic_id.uic_id[0]);
        p_v_addon = &p_vd[k].veh_add_on;

        if( UIC_ID_CMP( &g_ld_uicid, p_id_cmp ) == 0 ){
            
            /* property leading */
            *p_v_addon |= LD_VEH_BITMASK;

            /* write property 'leading' in my topo-frame */
            if( ld_my_cv_flag ){
                li_t_ChgInaugDataByte( TFR_OFFSET_TO_ST_CTRL_INFO, UI_VEH_LEADING,
                                       UI_VEH_LEADING );
            }
        }
        else{

            *p_v_addon &= (UNSIGNED8)(~LD_VEH_BITMASK);

            /* reset property 'leading' in my topo-frame */
            if( !ld_my_cv_flag ){
                li_t_ChgInaugDataByte( TFR_OFFSET_TO_ST_CTRL_INFO, (UNSIGNED8)(~UI_VEH_LEADING),
                                       UI_VEH_LEADING );
            }
        }
    } 
    
    return;
}



/*******************************************************/
void ascend_range_act( UIC_VehDescr *p_vd, int num_entr )
{
    int i;
    UNSIGNED8 RNumber;
    TYPE_UIC_ID id_help;

    if( num_entr == 0 ){
        return;
    }
    
    i = 0;
    RNumber = 1;

    while(1) {
        id_help = p_vd[i].VehDescr.uic_id;
        p_vd[i].RangeNo = RNumber;
        /* more entries? */
        if( ++i >= num_entr ) break;
        /* next vehicle/leak? */
        if( UIC_ID_CMP( &p_vd[i].VehDescr.uic_id, &id_help ) != 0 ){
            RNumber++;
        }
    };
}


/*******************************************************/
void descend_range_act( UIC_VehDescr *p_vd, int num_entr )
{
    int i;
    UNSIGNED8 RNumber;
    TYPE_UIC_ID id_help;

    if( num_entr == 0 ){
        return;
    }
    
    i = num_entr-1;
    RNumber = 1;

    while(1) {
        id_help = p_vd[i].VehDescr.uic_id;
        p_vd[i].RangeNo = RNumber;
        /* more entries? */
        if( --i < 0 ) break;
        /* next vehicle/leak? */
        if( UIC_ID_CMP( &p_vd[i].VehDescr.uic_id, &id_help ) != 0 ){
            RNumber++;
        }
    };
}



/*******************************************************/
void change_to_ascend( UIC_VehDescr *p_vd, int num_entr )
{
    int i;
    UNSIGNED8 RNumber;
    UNSIGNED8 RNumberOld;
    BOOLEAN valid_rno_found;

    if( num_entr == 0 ){
        return;
    }
    
    i = 0;
    RNumber = 1;
    valid_rno_found = FALSE;

    while(1) {
        /* only for valid UIC addresses */
        if( p_vd[i].RangeNo != INVALID_UIC_ADDRESS ){
            if( valid_rno_found ){
                if( p_vd[i].RangeNo != RNumberOld ){
                    RNumber++;
                }
            }
            RNumberOld = p_vd[i].RangeNo;
            valid_rno_found = TRUE;
            p_vd[i].RangeNo = RNumber;
        }
        /* more entries? */
        if( ++i >= num_entr ) break;
    };
}



/*******************************************************/
void change_to_descend( UIC_VehDescr *p_vd, int num_entr )
{
    int i;
    UNSIGNED8 RNumber;
    UNSIGNED8 RNumberOld;
    BOOLEAN valid_rno_found;

    if( num_entr == 0 ){
        return;
    }
    
    i = num_entr-1;
    RNumber = 1;
    valid_rno_found = FALSE;

    while(1) {
        /* only for valid UIC addresses */
        if( p_vd[i].RangeNo != INVALID_UIC_ADDRESS ){
            if( valid_rno_found ){
                if( p_vd[i].RangeNo != RNumberOld ){
                    RNumber++;
                }
            }
            RNumberOld = p_vd[i].RangeNo;
            valid_rno_found = TRUE;
            p_vd[i].RangeNo = RNumber;
        }
        /* more entries? */
        if( --i < 0 ) break;
    };
}




/*******************************************************/
/*         calculates the rno-direction                */
/*******************************************************/
BOOLEAN calculate_rnodir( UI_NODES_Info *p_NodesInfo,
                          UIC_VehDescr *p_vd,
                          int num_entries,
                          UNSIGNED8 from )
{
    int pos1, pos2, i, j;
    int range_1, range_2;
    int ndx_saved, tcn_saved;
    int trac_index, ndx;
    int rng_saved, last_srno;
    int rng_cmp, rng_last;
    TYPE_UIC_ID id_help;
    TYPE_UIC_ID id_last;
    TYPE_UIC_ID tr_ld_id;
    BOOLEAN trac_ld_exists;
    BOOLEAN found;

    /* recalculate rno-direction applying rule a) - d) */
    /* and do not use the saved rno's from IDTS        */
    /***************************************************/
    if( from == FROM_LD_POS )
    {
        /* rule 1: leading vehicle at one end of the train */
        if( UIC_ID_CMP( &g_ld_uicid, &p_vd[0].VehDescr.uic_id ) == 0 ){
            g_rno_dir = RNO_DIR_TOP_BOTTOM;
            return TRUE;
        }
        if( UIC_ID_CMP( &g_ld_uicid, &p_vd[num_entries-1].VehDescr.uic_id ) == 0 ){ 
            g_rno_dir = RNO_DIR_BOTTOM_TOP;
            return TRUE;
        }
                   

        /* rule 2: leading vehicle within the train */
        pos1 = pos2 = i = 0;
        found = FALSE;

        while( i <= num_entries-1 ){
            /* leading vehicle found */
            if( UIC_ID_CMP( &g_ld_uicid, &p_vd[i].VehDescr.uic_id ) == 0 ){
                found = TRUE;
                pos1 = i;
                id_help = p_vd[i].VehDescr.uic_id;
                /* same vehicle */
                while( ++i <= num_entries-1 ){
                    if( UIC_ID_CMP( &p_vd[i].VehDescr.uic_id, &id_help ) != 0 ) break;
                }
                pos2 = i;
                break;
            }
            else{
                i++;
            }
        }

        if( found ){
            if( pos1 && (pos1 < (num_entries - pos2)) ){
                g_rno_dir = RNO_DIR_TOP_BOTTOM;
            }
            else{
                g_rno_dir = RNO_DIR_BOTTOM_TOP;
            }
            return TRUE;
        }

            
        /* rule 3: not rule 1 or 2, but traction leader at one end of the train */ 
        /* NOTE: the traction leader vehicle doesn't get the property 'leading' */
        trac_ld_exists = tractionleader_pos( p_vd, num_entries, &tr_ld_id, &trac_index );

        if( trac_ld_exists && (trac_index == 0) ){
            g_rno_dir = RNO_DIR_TOP_BOTTOM;
        }
        /* rule 4: traction leader at top or not rule 1-3 */
        else{
            g_rno_dir = RNO_DIR_BOTTOM_TOP;
        }
        
        return TRUE;
    } 
    /* use saved rno's from IDTS for calculation */
    /* of rno-direction                          */
    /*********************************************/
    else if( from == FROM_SAVED_RNO )
    {
        range_1 = range_2 = 0;

        /* initialize with default direction */
        g_rno_dir = RNO_DIR_BOTTOM_TOP;

        ndx = MIN( num_entries, g_max_entries );

        /* entries are sorted from bottom to top-node */
        for( i = 0; i < ndx; i++ ){
             if( (range_1 = p_vd[i].RangeNo) != 0 ){
                 ndx_saved = i;
                 break;
             }
        }
        for( j = i+1; j < ndx; j++ ){
             if( ((range_2 = p_vd[j].RangeNo) != 0) && (range_2 != range_1) ){
                 /* overwrite index ( used if only one rno set ) */
                 ndx_saved = j;
                 break;
             }
        }

        /* two range numbers found */
        if( range_1 * range_2 )
        {
            if( range_1 < range_2 ){
                g_rno_dir = RNO_DIR_TOP_BOTTOM;
            }

            /* check for monotony */
            for( i = 0; i < ndx; i++ ){
                if( (rng_last = p_vd[i].RangeNo) != 0 ){
                    id_last = p_vd[i].VehDescr.uic_id;
                    break;
                }
            }

            for( j = i+1; j < ndx; j++ ){
                if( (rng_cmp = p_vd[j].RangeNo) != 0 ){
                    /* next vehicle? */
                    if( UIC_ID_CMP( &id_last, &p_vd[j].VehDescr.uic_id ) != 0 ){
                        if( ((rng_cmp <= rng_last) && (g_rno_dir == RNO_DIR_TOP_BOTTOM)) ||
                            ((rng_cmp >= rng_last) && (g_rno_dir == RNO_DIR_BOTTOM_TOP)) ){
                            return FALSE;
                        }
                        rng_last = rng_cmp;
                        id_last = p_vd[j].VehDescr.uic_id;
                    }
                }
            }
            return TRUE;
        }


        /* only one range number found */
        if( (range_1 + range_2) != 0 ){

            if( range_1 ){
                rng_saved = range_1;
            }
            else{
                rng_saved = range_2;
            }
            
            tcn_saved = p_vd[ ndx_saved ].NodeAddr;
            last_srno = p_NodesInfo->nb_veh_saved;

            /* only special cases could be handled ... */
            if( ((rng_saved == 0x01) && (tcn_saved == p_NodesInfo->top_address)) ||
                ((rng_saved == last_srno) && (tcn_saved == p_NodesInfo->bottom_address)) ){
                 g_rno_dir = RNO_DIR_TOP_BOTTOM;
                 return TRUE;
            }

            if( ((rng_saved == 0x01) && (tcn_saved == p_NodesInfo->bottom_address)) ||
                ((rng_saved == last_srno) && (tcn_saved == p_NodesInfo->top_address)) ){
                 g_rno_dir = RNO_DIR_BOTTOM_TOP;
                 return TRUE;
            }

            return FALSE;
       } 

    } /* end 'from == FROM_SAVED_RNO' */

    return FALSE;
    
}



/*****************************************************/
UNSIGNED8 cnt_ins_veh( void )
{
   UNSIGNED8 i, count;

   count = i = 0;
   
   do {
        if( g_na_veh[i/8] & (1<<(i%8)) ) count++;
   } while( ++i < TFR_LEN_NA_VEH*8 );
   
   return count;

}


/*****************************************************/
UNSIGNED8 bot_entries_lost( UI_NODES_Info *p_NodesInfo )
{
    UNSIGNED8 rno;
    UNSIGNED8 dummy;
    UNSIGNED8 cnt;
    int offset, k;

    cnt = 0;

    if( g_rno_dir == RNO_DIR_BOTTOM_TOP ){
        offset = 1;
        rno = 1;
    }
    else{
        offset = -1;
        rno = p_NodesInfo->nb_veh_saved;
    }

    for( k = 0; k < TFR_LEN_NA_VEH; k++ ){
        if( is_lost_rno( rno, &dummy ) ) cnt++;
        else break;
        rno += offset;
    }
    return cnt;
}




/*****************************************************/
UNSIGNED8 top_entries_lost( UI_NODES_Info *p_NodesInfo )
{
    UNSIGNED8 rno;
    UNSIGNED8 dummy;
    UNSIGNED8 cnt;
    int offset, k;

    cnt = 0;
    rno = 1;

    if( g_rno_dir == RNO_DIR_TOP_BOTTOM ){
        offset = 1;
        rno = 1;
    }
    else{
        offset = -1;
        rno = p_NodesInfo->nb_veh_saved;
    }

    for( k = 0; k < TFR_LEN_NA_VEH; k++ ){
        if( is_lost_rno( rno, &dummy ) ) cnt++;
        else break;
        rno += offset;
    }
    return cnt;
}




/*****************************************************/
UNSIGNED8 cnt_lost_entries( void )
{
   UNSIGNED8 i, count;
   UNSIGNED8 cmp_ndx;

   /* do not count confirmed leaks! */
   for( count = i = 0; i < MAX_CORR_VEH; i++ ){
        cmp_ndx = g_l_rno_tbl[i].rno - 1;
        if( cmp_ndx >= 0 ){
            if( (g_na_veh[ cmp_ndx/8 ] & (1<<(cmp_ndx%8))) == 0 ){
                count += g_l_rno_tbl[i].nb_lost;
            }
        }
   }

   return count;
}


#ifdef O_KFT

/*****************************************************/
int bit_len( UNSIGNED8 *bitset )
{
  int i, j;
  int cnt = 0;

  for (i = 0; i < LEN_TCI-2; i++) {
    for (j = 7; j >= 0; j--) {
      if (bitset[i] & (1 << j)) {
        cnt = 0;
      }
      else {
        if (++cnt >= 3) {
          return i * 8 + (7 - j) - 2;
        }
      }
    }
  }  
  return 0;
}


/*****************************************************/
void bit_swap( UNSIGNED8 *bitset, int len )
{
  int i, j;
  UNSIGNED8 tmpset[LEN_TCI-2];

  for (i = 0; i < LEN_TCI-2; i++) {
    tmpset[i] = 0;
  }

  for (i = 0, j = len - 1; i < len; i++, j--) {
    tmpset[i / 8] |= (bitset[j / 8] & (1 << (7 - (j % 8)))) ? (1 << (7 - (i % 8))) : 0;
  }

  for (i = 0; i < LEN_TCI-2; i++) {
    bitset[i] = tmpset[i];
  }
} 

#endif


/*******************************************************/





