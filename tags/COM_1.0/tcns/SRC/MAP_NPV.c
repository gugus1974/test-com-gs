/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:02> removed the O_960 pragmas                                                           */
/* <ATR:03> Include "atr_map.h" with UIC Mapping Server spacific patches                        */
/* <ATR:04> recorder() commented out                                                            */
/* <ATR:05> WTB dependencies only if it is present                                              */
/* <ATR:06> Little Endian support                                                               */
/* <ATR:07> do not modify the output message when not needed                                    */
/* <ATR:08> order_ui correct handling                                                           */
/* <ATR:09> corrected interface structure definitions with the UI                               */
/* <ATR:10> do not allow updates between recv and reply (messenger must have higher prio)       */
/* <ATR:11> wrong checking for errors                                                           */
/* <ATR:12> pack and unpack telegrams                                                           */
/* <ATR:14> removed ext_ref check in call_conf_update()                                         */
/* <ATR:15> do not retry the init request calls more that X times (the gateway could be off)    */
/* <ATR:16> si agg. possibilit… di richiedere il NADI ogni 800 cicli fino all' ottenimento      */
/* dello stesso
/*==============================================================================================*/

#include "atr_map.h"            /* <ATR:03> */

static char waiting_reply;      /* <ATR:10> */

/*
   000000120
   MAP_NPV
   Wolfgang Landwehr
   11. Oktober 1995, 16:02
   3
 */

/*
   GENERAL TOP OF FILE
 */

   /*
      This module contains all functions for connection to the WTB and the user
      interface functions (update mode).
    */

   /*
      25.11.94 W. Landwehr,
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
      1.xx  08.12.94  W. Landwehr
      Includes all test results until first implementation on target
      system INTEL960.
      1.90  03.05.94  W. Landwehr
      Advanced for node recovery and later inauguration.
      1.91  25.05.94  W. Landwehr       {---MAP 2.09---on train}
      Adaptions after lab test in week 17/95.
    */

   /*
      2.00  11.10.95  W. Landwehr
      Adoption for MyFriend, workaround.
    */


/*
   COMPILER SWITCHES
 */

/* <ATR:02> */
// #ifdef O_960
// #pragma noalign
// #endif

/*
   INCLUDEFILES
 */
#include "am_sys.h"             /* am@@@ messenger */
#include "pi_sys.h"             /* PIL interface */
#ifdef ATR_WTB                  /* <ATR:05> */
#include "bmi.h"                /* TCN control interface */
#endif
#include "map.h"                /* includes for map modules and apps */
#include "map_ums.h"            /* includes for map modules */


/*
   IMPLEMENTATION
 */

   /*
      DEFINES
    */
#define MAP_MVB 0xCAFE


   /*
      GLOBAL DECLARATIONS
    */
/*------------------T R A I N--D A T A B A S E ------------------------*/
map_TrainTopography NADI;
map_NodeGroupList GROUP_LIST[MAP_MAX_NO_GRP];
/*---------------------------------------------------------------------*/

/* <ATR:09> */
extern map_TrainTopography map_xnadi_ui;
extern map_TrainTopography map_ynadi_ui;
extern map_NodeGroupList map_xgroup_list_ui[MAP_MAX_NO_GRP];
extern map_NodeGroupList map_ygroup_list_ui[MAP_MAX_NO_GRP];
#define xNADI_UI map_xnadi_ui
#define yNADI_UI map_ynadi_ui
#define xGROUP_LIST_UI map_xgroup_list_ui
#define yGROUP_LIST_UI map_ygroup_list_ui
//    map_TrainTopography xNADI_UI;  /* for consistent data in map_ui */
//    map_TrainTopography yNADI_UI;
//    map_NodeGroupList   xGROUP_LIST_UI[ MAP_MAX_NO_GRP ];
//    map_NodeGroupList   yGROUP_LIST_UI[ MAP_MAX_NO_GRP ];

short map_sema;                 /* semaphore id */
short err;

/*------------------I N T E R N A L  D A T A --------------------------*/
/* <ATR:09> */
extern s_vw map_vw;
#define VW_MAP map_vw
//    s_vw VW_MAP;

s_rec15 REC_MVB;                /* Testrecorder for program steps */
s_rec7 REC_ERR_CODE;            /* Recorder error.code */
s_rec7 REC_ERR_INFO;            /* Recorder error.info */

/*------------------T G M--B U F F E R S-------------------------------*/
s_tgm_update_mvb upd_in_tgm[2];
s_tgm_update upd_out_tgm;
s_tgm_nadi tgm_nadi;

   /*
      PROTOTYPES
    */
/* <ATR:04> */
//    void recorder( unsigned char );

void map_stop (void);

void call_conf_update (char, char *, const AM_NET_ADDR *, s_tgm_vehdes *,
                       CARDINAL32, AM_RESULT);
void recv_conf_update (char, const AM_NET_ADDR *, s_tgm_update_mvb *,
                       CARDINAL32, char *);
void reply_conf_update (char, unsigned short *);

void map_update (void);


   /*
      FUNCTIONS {70, A2,A3,A8, B0-B6,B9, C3, D0,D2,D3,D9,DE,DF}
    */

      /*
         RECORDER
       */
/* <ATR:04> */
//         void recorder( unsigned char type )
//    {
//
//            /*
//            Internal trace recorder
//            */
//            short i;
//
//            switch( type )
//    {
//
//               case 0:
//    {
//
//                  /*
//                  INITIALIZE ERROR BUFFERS
//                  */
//
//                     for ( i=0; i<MAX_REC7; i++ )
//    {
//                        REC_ERR_CODE.val[ i ] = 0x2222;
//                        REC_ERR_INFO.val[ i ] = 0x3333;
//    }
//                     REC_ERR_CODE.ix = MAX_REC7;
//                     REC_ERR_INFO.ix = MAX_REC7;
//
//                     break;
//
//    }
//
//               case 0xFF:
//    {
//
//                  /*
//                  INITIALIZE TRACE BUFFERS
//                  */
//
//                     for ( i=0; i<MAX_REC15; i++ )
//    {
//                        REC_MVB.val[ i ] = 0xFF;
//    }
//                     REC_MVB.ix = MAX_REC15;
//
//                     break;
//
//    }
//
//               case R_ERR:
//    {
//
//                  /*
//                  ERROR RECORDER
//                  */
//                     i = REC_ERR_CODE.ix;
//
//                     if ( ( REC_ERR_CODE.val[ i ] != VW_MAP.code ) ||
//                          ( REC_ERR_INFO.val[ i ] != VW_MAP.info ) )
//    {
//
//                        if ( ++i >= MAX_REC7 )
//    {
//                           i = 0;
//    }
//                        REC_ERR_CODE.ix = i;
//                        REC_ERR_INFO.ix = i;
//                        REC_ERR_CODE.val[ i ] = VW_MAP.code;
//                        REC_ERR_INFO.val[ i ] = VW_MAP.info;
//    }
//                     break;
//
//    }
//    default:{
//
//                  /*
//                  STANDARD TRACE
//                  */
//                  i = REC_MVB.ix;
//
//                  if ( REC_MVB.val[ i ] != type )
//    {
//
//                     if ( ++i >= MAX_REC15 )
//    {
//                        i = 0;
//    }
//                     REC_MVB.ix = i;
//                     REC_MVB.val[ i ] = type;
//    }
//                  break;
//    }
//    }
//            return;
//    }


      /*
         MAP_STOP {70}
       */

void
map_stop (void)
{

    /*
       This function stops all activities and turn back into SETUP phase.
     */
    static AM_RESULT status;
    static AM_NET_ADDR replier;

    /* Replier address is GW node */
    replier.vehicle = AM_SAME_VEHICLE;
    replier.device = AM_UNKNOWN_DEVICE;
    replier.function = MAP_FUNCTION_NR;

    VW_MAP.code = MAP_OK;
    VW_MAP.info = 0;

    /* a possibly call to the gateway. */
    am_call_cancel (MAP_FUNCTION_NR, &replier, &status);

    /* Cancels a non responsed replier instance. */
    am_rcv_cancel (MAP_FUNCTION_NR, &VW_MAP.ext_u_wtb[0], &status);
    am_rcv_cancel (MAP_FUNCTION_NR, &VW_MAP.ext_u_wtb[1], &status);
    am_unbind_replier (MAP_FUNCTION_NR);

    if (VW_MAP.f.confirm != 0) {
        /* not succesfull processed */
        VW_MAP.f.confirm (VW_MAP.r_confirm, MAP_NOT_OK);
    }
    VW_MAP.f.confirm = 0;
    VW_MAP.r_confirm = 0;

    /* NADI is invalid, direction unknown. */
    NADI.state.mdirect = MAP_DIREC_UNKNOWN;
    NADI.state.validity = MAP_NADI_INVALID;

    xNADI_UI = NADI;
    yNADI_UI = NADI;

    VW_MAP.upd_status = MAP_UPDATE_NOT_READY;

    /* next cycle is start up again */
    VW_MAP.f1st_run_s = 0;

    recorder (0x70);
}

/* CALL_CONF_UPDATE {A3, C3, D3} */

void call_conf_update ( char caller_function,
                        char *ext_ref,
                        const AM_NET_ADDR * replier,
                        s_tgm_vehdes * in_msg_adr,
                        CARDINAL32 in_msg_size,
                        AM_RESULT status)
{
    /*
       call_confirm function for all ui services.
       Installd by am_call_requ, called by map_update
     */

/* <ATR:14> */
//            if ( *ext_ref == NPV )
//    {

    caller_function = caller_function;
    ext_ref = ext_ref;
    replier = replier;
    in_msg_adr = in_msg_adr;
    in_msg_size = in_msg_size;
    if (status == AM_OK)
    {
        if (VW_MAP.f.confirm != 0)
        {
            /* succesfull processed */
            VW_MAP.f.confirm (VW_MAP.r_confirm, MAP_OK);
        }
        VW_MAP.f.confirm = 0;
        VW_MAP.r_confirm = 0;

        recorder (0xA3);
    }
    else
    {
        /* ERROR HANDLE */

        if (VW_MAP.f.confirm != 0)
        {
            /* not succesfull processed */
            VW_MAP.f.confirm (VW_MAP.r_confirm, MAP_NOT_OK);
        }
        VW_MAP.f.confirm = 0;
        VW_MAP.r_confirm = 0;

        if ((VW_MAP.old_order == MVB_NADI_INI_REQU) ||
            (VW_MAP.old_order == MVB_GROUP_LIST_INI_REQU))
        {
            /* <ATR:15> */
            static int retry_count = 0;
            if (retry_count)
            {
                retry_count--;
                /* retry the NADI/GROUP_LIST ini request */
                VW_MAP.order_ui = VW_MAP.old_order;
            }
        }
        VW_MAP.code = MAP_ERR_CALL_CONFU;
        VW_MAP.info = status;
        recorder (R_ERR);
        recorder (0xC3);
    }
/* <ATR:14> */
//    }else{
//               recorder( 0xD3 );
//    }
}

/* RECV_CONF_UPDATE {B1-B6, D2} */

void recv_conf_update ( char replier_fct, const AM_NET_ADDR * caller,
                        s_tgm_update_mvb * in_msg_adr,
                        CARDINAL32 in_msg_size, char *ext_ref)
{
    char i, j, k;
    unsigned char uc_work, type;
    AM_RESULT status;

    /* for reply tgm */
    CARDINAL32 rr_size;
    static s_tgm_id rr_tgm;

    /* <ATR:10> */
    pi_pend_semaphore (map_sema, &err);

    unpack_uic_et (in_msg_adr, &in_msg_size);   /* <ATR:12> */
    conv_be_s_tgm_id (in_msg_adr->id);  /* <ATR:06> */

    rr_size = 4;
    type = 0;

    switch (in_msg_adr->id.code)
    {
        case MVB_NADI_INI_REQU:
        {
            conv_be_TrainTopography (in_msg_adr->content.NADI);     /* <ATR:06> */

            /*
               NADI FROM WTB-GW {B6}
             */
            NADI = in_msg_adr->content.NADI;

            /* Fill in the NADI copies */
            xNADI_UI = NADI;
            yNADI_UI = NADI;

            VW_MAP.actual_nadi = 0;

            /* next get the GROUP_LIST */
            VW_MAP.order_ui = MVB_GROUP_LIST_INI_REQU;
            rr_tgm.code = MVB_NADI_INI_RESP;

            recorder (0xB0);

            break;
        }

        case DYN_ATTR_REQUEST:
        {
            conv_be_VehicleDescr (in_msg_adr->content.vehicle);     /* <ATR:06> */

            /*
               INPUT A RECEIVED VEHICLE DESCRIPTOR FROM GW INTO NADI
               {B1, D1}
             */

            uc_work = in_msg_adr->content.vehicle.NodeAddr;     /* from sender */

            j = NADI.state.VehicleNo;

            for (i = 0; i < j; i++)
            {
                if (NADI.vehicle[i].NodeAddr == uc_work)
                {
                    NADI.vehicle[i] = in_msg_adr->content.vehicle;
                    NADI.state.version++;
                    j = 0;      /* ok */

                    /*
                       Work copy of NADI to perform data consitence for UI
                     */

                    if (VW_MAP.actual_nadi == 0)
                    {
                        yNADI_UI = NADI;
                        VW_MAP.actual_nadi = 1;
                    }
                    else
                    {
                        xNADI_UI = NADI;
                        VW_MAP.actual_nadi = 0;
                    }
                    type = MAP_NADI_UPDATED;

                    recorder (0xB1);

                    break;
                }
            }

            if (j != 0) {
                rr_size = 0;    /* Node not found within NADI */
                recorder (0xD1);
            }
            rr_tgm.code = DYN_ATTR_RESPONSE;

            break;

        }

    case GROUP_LIST_REQUEST:
        {
            conv_be_VehicleGroupList (in_msg_adr->content.group);   /* <ATR:06> */

            /*
               REPLACE THE GROUP LIST WITH THE RECEIVED DATA {B2}
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
                    k = GROUP_LIST[i].length;

                    GROUP_LIST[i].node[k] = in_msg_adr->content.group.member[j].node;
                    GROUP_LIST[i].length++;
                }
            }

            /*
               Work copy of GROUP LIST to perform data consitence for UI
             */

            if (VW_MAP.actual_group_list == 0) {

                for (i = 0; i < MAP_MAX_NO_GRP; i++) {
                    yGROUP_LIST_UI[i] = GROUP_LIST[i];
                }
                VW_MAP.actual_group_list = 1;
            } else {

                for (i = 0; i < MAP_MAX_NO_GRP; i++) {
                    xGROUP_LIST_UI[i] = GROUP_LIST[i];
                }
                VW_MAP.actual_group_list = 0;
            }
            VW_MAP.upd_status = MAP_UPDATE_READY;
            type = MAP_NADI_UPDATED;
            rr_tgm.code = GROUP_LIST_RESPONSE;

            recorder (0xB2);

            break;

        }

    case IDLE_STATE_REQUEST:
        {

            /*
               PREPARE RESPONSE TGM {B3}
             */
            type = MAP_IDLE_REQUEST;
            rr_tgm.code = IDLE_STATE_RESPONSE;

            recorder (0xB3);

            break;

        }
    default:{

            /*
               STRANGE TGM {D2}
             */
            am_reply_requ (replier_fct, &rr_tgm, rr_size, ext_ref, MAP_NP);

            rr_size = 0;

            recorder (0xD2);

            break;

        }
    }

    if (rr_size == 4) {
        conv_be_s_tgm_id (rr_tgm);  /* <ATR:06> */
        pack_uic_et (&rr_tgm, &rr_size);    /* <ATR:12> */

        /* <ATR:10> */
        waiting_reply = TRUE;

        /*
           REPLY TO REQUESTOR {B4, B5}
         */
        am_reply_requ (replier_fct, &rr_tgm, rr_size, ext_ref, MAP_OK);

        if (in_msg_adr->id.code == IDLE_STATE_REQUEST) {
            map_stop ();        /* set own node into IDLE */
        }
        if (type != 0 && VW_MAP.f_indication != 0) {
            VW_MAP.f_indication (VW_MAP.r_indication, type, MAP_OK);

            recorder (0xB4);
        } else {
            recorder (0xB5);
        }

    }

    /* <ATR:10> */
    pi_post_semaphore (map_sema, &err);
}


      /*
         REPLY_CONF_UPDATE {B9, D9}
       */

void
reply_conf_update (char replier, unsigned short *ext_ref)
{
    CARDINAL32 in_size;
    AM_RESULT status;

    /* <ATR:10> */
    waiting_reply = FALSE;

    /* n e x t   r e c e i v e */
    in_size = MAP_MAX_NO_VEH * SIZE_VH_DESCR + SIZE_NN_STATE + 5;
    am_rcv_requ (replier, &upd_in_tgm[*ext_ref], in_size, ext_ref, &status);

    if ((status == AM_OK) || (status == MAP_OK)) {
        recorder (0xB9);
    } else {
        VW_MAP.code = MAP_ERR_RECV_CONFU_REQ;
        VW_MAP.info = status;
        recorder (R_ERR);

        recorder (0xD9);
    }
}


      /*
         MAP_UPDATE {A2, A8, B0, D0, DE, DF}
       */

void
map_update (void)
{

    /*
       This function realizes all NADI update functions requested by the user
       interface UI or for the initialisation.
     */
    char i;
    unsigned char uc_work;
    AM_RESULT status;

    static CARDINAL32 in_size, out_size, rr_size;
    static s_tgm_def rr_tgm;
    static AM_NET_ADDR replier;

    status = AM_OK;

    if (VW_MAP.f1st_run_u_mvb != NPV) {

        /*
           INITIALISATION {B0,D0,DE,DF}
         */

        /*
           First run initialize am_@@@ to receive anything from the WTB level.
           Do it twice for security, so always one receiver is prepared.
         */
        recorder (0xFF);        /* reset trace buffer */

        am_rcv_cancel (MAP_FUNCTION_NR, &upd_in_tgm[0], &status);
        am_rcv_cancel (MAP_FUNCTION_NR, &upd_in_tgm[1], &status);
        am_unbind_replier (MAP_FUNCTION_NR);

        am_bind_replier (MAP_FUNCTION_NR, (AM_RCV_CONF) recv_conf_update,
                         (AM_REPLY_CONF) reply_conf_update, &status);

        if ((status == AM_OK) || (status == AM_ALREADY_USED)) {

            /*
               PREPARE RECEIVES 1 AND 2
             */
            /* reset some buffers */
            upd_out_tgm.format = 0;
            upd_out_tgm.id.key = 0;
            upd_out_tgm.id.owner = 0;

            /* max. size is length of the NADI */
            in_size = MAP_MAX_NO_VEH * SIZE_VH_DESCR + SIZE_NN_STATE + 5;
            rr_size = 6;

            VW_MAP.ext_u_wtb[0] = 0;    /* recv. */
            VW_MAP.ext_u_wtb[1] = 1;    /* recv. */
            VW_MAP.ext_c_wtb[0] = NPV;  /* send. */

            /* Replier address is GW node */
            replier.vehicle = AM_SAME_VEHICLE;
            replier.device = AM_UNKNOWN_DEVICE;
            replier.function = MAP_FUNCTION_NR;

            /*
               RECEIVE  nr 1
             */
            am_rcv_requ (MAP_FUNCTION_NR, &upd_in_tgm[0], in_size,
                         &VW_MAP.ext_u_wtb[0], &status);

            if (status == AM_OK) {

                /*
                   RECEIVE nr 2
                 */
                am_rcv_requ (MAP_FUNCTION_NR, &upd_in_tgm[1], in_size,
                             &VW_MAP.ext_u_wtb[1], &status);

                if (status == AM_OK) {
                    recorder (0xB0);

                    /* initialisation done */
                    VW_MAP.f1st_run_u_mvb = NPV;
                } else {
                    am_unbind_replier (MAP_FUNCTION_NR);

                    am_rcv_cancel (MAP_FUNCTION_NR,
                                   &upd_in_tgm[0], &status);

                    VW_MAP.code = MAP_ERR_UPDATE_RCV_MVB;
                    VW_MAP.info = status;
                    recorder (R_ERR);

                    recorder (0xDF);
                }

            } else {
                am_unbind_replier (MAP_FUNCTION_NR);

                VW_MAP.code = MAP_ERR_UPDATE_RCV_MVB;
                VW_MAP.info = status;
                recorder (R_ERR);

                recorder (0xDE);
            }

        } else {

            /*
               error, try next time
             */
            am_unbind_replier (MAP_FUNCTION_NR);

            VW_MAP.code = MAP_ERR_UPDATE_BR_MVB;
            VW_MAP.info = status;
            recorder (R_ERR);

            recorder (0xD0);
        }

    } else {

        /* <ATR:10> */
        if (waiting_reply) return;

        pi_pend_semaphore (map_sema, PI_FOREVER, &err);

/* <ATR:07> */
//               /*
//               PREPARE FOR THE NEXT UI ORDER {A8}
//               */
//                  upd_out_tgm.id.code = VW_MAP.order_ui;
//                  VW_MAP.old_order = NEXT_ORDER;

        switch (VW_MAP.order_ui) {

        case MVB_NADI_INI_REQU:
            {

                /*
                   GET NADI FROM WTB-GW
                 */
                /* to repeat the request until succeed */
                VW_MAP.old_order = MVB_NADI_INI_REQU;
                out_size = 4;

                break;

            }

        case MVB_GROUP_LIST_INI_REQU:
            {

                /*
                   GET GROUP LIST FROM WTB-GW
                 */
                /* to repead the request until succeed */
                VW_MAP.old_order = MVB_GROUP_LIST_INI_REQU;
                out_size = 4;

                break;

            }

        case DYN_ATTR_REQUEST:
            {

                /*
                   UI CALL: map_SetVehicleDescriptor
                 */
                upd_out_tgm.content.vehicle = VW_MAP.upd_ui.vehicle;
                out_size = SIZE_VH_DESCR + 5;

                conv_be_VehicleDescr (upd_out_tgm.content.vehicle);     /* <ATR:06> */

                break;

            }

        case IDLE_STATE_REQUEST:
            {

                /*
                   UI CALL: map_SetIdle
                 */
                out_size = 4;

                break;

            }

        case GROUP_LIST_REQUEST:
            {

                /*
                   UI CALL:map_SetGroupList
                 */
                upd_out_tgm.content.group = VW_MAP.upd_ui.group;
                upd_out_tgm.format = 4;
                out_size = VW_MAP.upd_ui.group.length * 2 + 2 + 5;

                conv_be_VehicleGroupList (upd_out_tgm.content.group);   /* <ATR:06> */

                break;

            }

        case CANCEL_REQUEST:
            {

                /*
                   UI CALL: map_CancelRequest
                 */
                am_call_cancel (MAP_FUNCTION_NR, &replier, &status);

                VW_MAP.upd_status = MAP_UPDATE_NOT_READY;
                VW_MAP.numN = 0;
                VW_MAP.order_ui = NEXT_ORDER;

                break;

            }
        default:{
                VW_MAP.order_ui = NEXT_ORDER;

                recorder (0xA8);

                break;
            }
        }


        if (VW_MAP.order_ui != NEXT_ORDER) {

            upd_out_tgm.id.code = VW_MAP.order_ui;  /* <ATR:08> */
            conv_be_s_tgm_id (upd_out_tgm.id);  /* <ATR:06> */
            pack_uic_et (&upd_out_tgm, &out_size);  /* <ATR:12> */
            VW_MAP.old_order = VW_MAP.order_ui;
            VW_MAP.order_ui = NEXT_ORDER;

            /*
               WORK OUT THE CURRENT ORDER {A2}
             */
            am_call_requ (MAP_FUNCTION_NR, &replier,
                       &upd_out_tgm, out_size, &rr_tgm, rr_size, E2_TIMEOUT,
                     (AM_CALL_CONF) call_conf_update, &VW_MAP.ext_c_wtb[0]);

            if ((VW_MAP.order_ui == MVB_NADI_INI_REQU) ||
                (VW_MAP.order_ui == MVB_GROUP_LIST_INI_REQU)) {
                /* user interface disabled until setup is finished. */
                VW_MAP.order_ui = INIT_ORDER;
            }
            VW_MAP.code = MAP_OK;
            VW_MAP.info = 0;

            recorder (0xA2);

        }
//      <ATR:16>
        else if (VW_MAP.old_order == MVB_NADI_INI_REQU)
        {
            static int time_count=800;
            if (time_count > 0)
                time_count--;
            else if (time_count == 0)
            {
                time_count = 800;
                VW_MAP.order_ui = VW_MAP.old_order;
            }
        }

        pi_post_semaphore (map_sema, &err);
    }
}



   /*
      MAIN: MAP_NPV
    */

void
map_npv (void)
{
    char i, j;

    /*
       RESET VALUE FOR NADI MEMBER
     */
    map_VehicleDescr m_vh =
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


    if (VW_MAP.f1st_run_s != NPV) {
        recorder (0);           /* reset error buffer */

        map_sema = pi_create_semaphore (1, PI_ORD_FCFS, &err);

/* <ATR:11> */
//          if ( err != PI_RET_OK )
        if (err == PI_RET_OK) {

            /*
               RESET NADI
             */

            for (i = 0; i < MAP_MAX_NO_VEH; i++) {
                NADI.vehicle[i] = m_vh;
            }
            NADI.state.mdirect = MAP_DIREC_UNKNOWN;
            NADI.state.validity = MAP_NADI_INVALID;
            NADI.state.version = 0;
            NADI.state.ownaddr = 0;
            NADI.state.VehicleNo = 0;

            /* copies for the user interface due to data consistence */
            xNADI_UI = NADI;
            yNADI_UI = NADI;

            VW_MAP.actual_nadi = 0;


            /*
               RESET GROUP LIST
             */

            for (i = 0; i < MAP_MAX_NO_GRP; i++) {
                GROUP_LIST[i].length = 0;

                for (j = 0; j < MAP_MAX_NO_NGL; j++) {
                    GROUP_LIST[i].node[j] = 0;
                }
            }

            /*
               copies for the user interface due to data consistence.
             */

            for (i = 0; i < MAP_MAX_NO_GRP; i++) {
                xGROUP_LIST_UI[i] = GROUP_LIST[i];
                yGROUP_LIST_UI[i] = GROUP_LIST[i];
            }
            VW_MAP.actual_group_list = 0;

            VW_MAP.code = MAP_OK;
            VW_MAP.info = 0;

            VW_MAP.f1st_run_u_mvb = 0;
            VW_MAP.upd_status = MAP_UPDATE_NOT_READY;

            /* start with NADI setup */
            VW_MAP.order_ui = MVB_NADI_INI_REQU;

            VW_MAP.f1st_run_s = NPV;
        } else {

            /*
               ERROR HANDLING
             */
            VW_MAP.code = MAP_ERR_SEMA;
            VW_MAP.info = err;

            recorder (R_ERR);

        }
    } else {
        map_update ();
    }
}
