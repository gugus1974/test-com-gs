/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> removed the O_960 pragmas                                                           */
/* <ATR:02> Include "atr_map.h" with UIC Mapping Server spacific patches                        */
/* <ATR:03> WTB dependencies only if it is present                                              */
/* <ATR:04> UI globals allocation moved from MAP_NPT/MAP_NPV to MAP_UI                          */
/* <ATR:05> GROUP_LIST definition fixed                                                         */
/*==============================================================================================*/

#include "atr_map.h"            /* <ATR:02> */

/*
   000000120
   MAP_UI
   Wolfgang Landwehr
   11. Oktober 1995, 15:59
   3
 */

/*
   GENERAL TOP OF FILE
 */

   /*
      This module contains the library functions for the user interface.
    */

   /*
      28.03.94 W. Landwehr
      Start of developement.
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
         0.0n 28.03.94  W. Landwehr
         Starting version, starting point is
         UIC-MAPPING SERVER SPECIFICATION 1.0, Feb. 94
         Includes all test results until first implementation on target
         system INTEL960.
         1.00 23.04.94  W. Landwehr
         First integration in lab
         1.01 03.05.94  W. Landwehr
         Workaround after 1st integration
         1.02 25.05.94  W. Landwehr
         Adaption to other map modules.
         1.03 01.09.94  W. Landwehr
         Advanced with multicast.
         1.04 23.09.94  W. Landwehr
         Adaptions wich were agreed within workshop 14.9.
         -internal copy for NADI and GROUP_LIST is used (data consistence)
         -parameter fct = unsigned char for map_multicast
         -map_idInit does no longer enable ui features
         1.05 25.11.94  W. Landwehr
         Adaptions for MVB use
         1.06 08.12.94  W. LAndwehr
         Sema on MVB
         1.07 16.12.94  W. Landwehr      {---MAP 1.17---}
         Workaround for Get services
         1.08 28.03.95  W. Landwehr
         prefixes map to global names
         1.09 28.04.95  W. Landwehr
         check NADI validity by calling map_Get_xxx
       */


   /*
      2.00 28.04.95  W. Landwehr      {---MAP 2.09 on train ---}
      check NADI validity within map_GetTrainTopography
      2.01 11.10.95  W. Landwehr      {---MAP 2.10---}
      Adoption for MyFriend, Workaround
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
#include "AM_SYS.H"             /* RTP interface */
#include "PI_SYS.H"             /* PIL interface */
#ifdef ATR_WTB                  /* <ATR:03> */
#include "BMI.H"                /* WTB busmanager interface */
#endif
#include "MAP.H"
#include "MAP_UMS.H"
#ifdef ATR_WTB                  /* <ATR:03> */
#include "MAP_BMI.H"
#endif

/*
   IMPLEMENTATION
 */

   /*
      EXTERNALS
    */
extern map_TrainTopography NADI;
extern map_NodeGroupList GROUP_LIST[MAP_MAX_NO_GRP /* <ATR:05> MAP_MAX_NO_VGL */];

      /*
         To provide data consistence for NADI and GROUP LIST
       */
      /* <ATR:04> extern */ map_TrainTopography map_xnadi_ui;
      /* <ATR:04> extern */ map_TrainTopography map_ynadi_ui;
      /* <ATR:04> extern */ map_NodeGroupList map_xgroup_list_ui[MAP_MAX_NO_GRP];
      /* <ATR:04> extern */ map_NodeGroupList map_ygroup_list_ui[MAP_MAX_NO_GRP];

extern short map_sema;

      /* <ATR:04> extern */ s_vw map_vw;


   /*
      FUNCTIONS OF THE USER INTERFACE
    */

      /*
         MAP_IDINIT
       */

map_result
map_idInit (void)
{

    /*
       This function initialises the interface functions.
       (UIC-MAPPING SERVER SPEC. 1.0, Pg. 30)
       return values:
       MAP_OK   ok
       MAP_RNA  not accepted, SETUP mode or another is not yet ready
     */
    short status;
    map_result q;

#ifdef O_960

    pi_pend_semaphore (map_sema, PI_FOREVER, &status);

#endif

    if (map_vw.upd_status == MAP_UPDATE_READY) {
        q = MAP_OK;
    } else {
        q = MAP_RNA;
    }

#ifdef O_960

    pi_post_semaphore (map_sema, &status);

#endif
    return q;
}

/* MAP_IDINSTALL */

map_result map_idInstall (  unsigned char caller,
                            map_indication indication,
                            void *reference)
{
    /*
       This function installs the call back indication procedure wich informs the
       user about any network event.
       Return values:
       MAP_OK    ok
       MAP_RNAL  another is working
     */
    short status;
    map_result q;

    caller = caller;
    pi_pend_semaphore (map_sema, PI_FOREVER, &status);

    if (map_vw.upd_status == MAP_UPDATE_READY)
    {
        /* replace actual value */
        map_vw.f_indication = indication;
        map_vw.r_indication = reference;

        q = MAP_OK;
    }
    else
    {
        q = MAP_RNAL;
    }
    pi_post_semaphore (map_sema, &status);

    return q;
}

/* MAP_SETIDLE */

map_result map_SetIdle (unsigned char caller,
                        map_setidle_conf confirmation,
                        void *reference)
{
    /*
       This function delivers the idle request to all other train members and
       switches the own vehicle (mapping server) into idle state.
       Return values:
       MAP_OK    ok
       MAP_RNA   service request not accepted, remote services like SET_@@@
    */
    short status;
    map_result q;

    caller = caller;
    pi_pend_semaphore (map_sema, PI_FOREVER, &status);

    if (map_vw.order_ui == NEXT_ORDER)
    {
        /* no order pending */
        map_vw.mc_caller = caller;
        map_vw.f.setidle = confirmation;
        map_vw.r_confirm = reference;
        map_vw.order_ui = IDLE_STATE_REQUEST;
        q = MAP_OK;
    }
    else
    {
        q = MAP_RNA;
    }
    pi_post_semaphore (map_sema, &status);

    return q;
}

/* MAP_SWITCHOVER */

map_result map_SwitchOver ( unsigned char caller,
                            map_switchover_conf confirmation,
                            void *reference)
{
    /*
       This function delivers the switch over request to all other train members.
       The own vehicle switches controll to UIC lines.
       Return values:
       MAP_OK    ok
       MAP_RNA   service request not accepted, remote services like SET_@@@
    */
    short status;
    map_result q;

    caller = caller;
    pi_pend_semaphore (map_sema, PI_FOREVER, &status);

    if (map_vw.order_ui == NEXT_ORDER)
    {
        /* no order pending */
        map_vw.f.switchover = confirmation;
        map_vw.r_confirm = reference;
        map_vw.order_ui = SWITCHOVER_REQUEST;

        q = MAP_OK;
    }
    else
    {
        if (map_vw.upd_status == MAP_UPDATE_AT_WORK)
        {
            q = MAP_RNA;
        }
    }
    pi_post_semaphore (map_sema, &status);

    return q;
}

/* MAP_SWITCHBACK */

map_result map_SwitchBack ( unsigned char caller,
                            map_switchback_conf confirmation,
                            void *reference)
{
    /*
       This function delivers the switch back request to all other train members.
       The own vehicle switches controll back to TCN.
       Return values:
       MAP_OK    ok
       MAP_RNA   service request not accepted, remote services like SET_@@@
    */
    short status;
    map_result q;

    caller = caller;
    pi_pend_semaphore (map_sema, PI_FOREVER, &status);

    if (map_vw.order_ui == NEXT_ORDER)
    {
        /* no order pending */
        map_vw.f.switchover = confirmation;
        map_vw.r_confirm = reference;
        map_vw.order_ui = SWITCHBACK_REQUEST;

        q = MAP_OK;
    }
    else
    {
        if (map_vw.upd_status == MAP_UPDATE_AT_WORK)
        {
            q = MAP_RNA;
        }
    }
    pi_post_semaphore (map_sema, &status);

    return q;
}

/* MAP_GETOWNVHDESCRIPTOR */

map_result map_GetOwnVhDescriptor ( unsigned char caller,
                                    map_VehicleDescr ** vh_des)
{
    /*
       Returns a pointor onto the own vehicle descriptor.
       Return values:
       MAP_OK    ok
       MAP_RNAL  NADI invalid or another is working
       MAP_NP    own vehicle descriptor not found (normaly impossible)
    */
    short status;
    char i, j;
    map_result q;

    caller = caller;
    q = MAP_NP;

    pi_pend_semaphore (map_sema, PI_FOREVER, &status);

    if (map_vw.upd_status != MAP_UPDATE_NOT_READY)
    {
        if (NADI.state.validity == MAP_NADI_VALID)
        {
            j = NADI.state.VehicleNo;

            /* FIND OWN VEHICLE DESCRIPTOR */

            for (i = 0; i < j; i++)
            {
                if (map_vw.actual_nadi == 0)
                {
                    if (map_xnadi_ui.vehicle[i].NodeAddr == map_xnadi_ui.state.ownaddr)
                    {
                        *vh_des = &map_xnadi_ui.vehicle[i];
                        q = MAP_OK;
                        break;
                    }
                }
                else
                {
                    if (map_ynadi_ui.vehicle[i].NodeAddr
                        == map_ynadi_ui.state.ownaddr)
                    {
                        *vh_des = &map_ynadi_ui.vehicle[i];
                        q = MAP_OK;
                        break;
                    }
                }
            }
        }
        else
        {
            /* NADI is invalid, set by the
               map_bmi */
            *vh_des = 0;
            q = MAP_RNAL;
        }
    }
    else
    {
        *vh_des = 0;
        q = MAP_RNAL;
    }
    pi_post_semaphore (map_sema, &status);

    return q;
}


/* MAP_GETTRAINTOPOGRAPHY */

map_result map_GetTrainTopography ( unsigned char caller,
                                    map_TrainTopography ** topo)
{
    /*
       Returns a pointer onto the current train topography to the caller.
       Return values:
       MAP_OK    ok
       MAP_RNAL  NADI invalid or another is working
       MAP_NP    own vehicle descriptor not found (normaly impossible)
    */
    short status;
    map_result q;

    caller = caller;
    pi_pend_semaphore (map_sema, PI_FOREVER, &status);

    if (map_vw.upd_status != MAP_UPDATE_NOT_READY)
    {
        if (NADI.state.validity == MAP_NADI_VALID)
        {
            if (map_vw.actual_nadi == 0)
            {
                *topo = &map_xnadi_ui;
            }
            else
            {
                *topo = &map_ynadi_ui;
            }
            q = MAP_OK;
        }
        else
        {
            /* NADI is invalid, set by the map_bmi */
            *topo = 0;
            q = MAP_RNAL;
        }
    }
    else
    {
        *topo = 0;
        q = MAP_RNAL;
    }
    pi_post_semaphore (map_sema, &status);

    return q;
}

/* MAP_GETGROUPLIST */

map_result map_GetGroupList (unsigned char caller,
                             unsigned char group_number,
                             map_NodeGroupList ** node_list)
{
    /*
       Returns a pointer to a list of nodes wich are member of the given group.
       Return values:
       MAP_OK    ok
       MAP_RNAL  another is working
       MAP_NP    group number out of range
    */
    short status;
    map_result q;

    caller = caller;
    pi_pend_semaphore (map_sema, PI_FOREVER, &status);

    if (map_vw.upd_status != MAP_UPDATE_NOT_READY)
    {
        if (((char) (group_number) >= 0) ||
            ((char) (group_number) < MAP_MAX_NO_VGL))
        {
            if (map_vw.actual_group_list == 0)
            {
                *node_list = &map_xgroup_list_ui[group_number];
            }
            else
            {
                *node_list = &map_ygroup_list_ui[group_number];
            }
            q = MAP_OK;
        }
        else
        {
            *node_list = 0;
            q = MAP_NP;
        }
    }
    else
    {
        *node_list = 0;
        q = MAP_RNAL;
    }
    pi_post_semaphore (map_sema, &status);

    return q;
}

/* MAP_SETGROUPLIST */

map_result map_SetGroupList (unsigned char caller,
                             map_VehicleGroupList * group_list,
                             map_setgrplist_conf confirmation,
                             void *reference)
{
    /*
       This function describes the group membership.
       return values:
       MAP_OK   ok
       MAP_RNA  not accepted, SETUP mode or another is not yet ready
    */
    short status;
    map_result q;

    caller = caller;
    pi_pend_semaphore (map_sema, PI_FOREVER, &status);

    if (map_vw.order_ui == NEXT_ORDER)
    {
        map_vw.f.setgrplist = confirmation;
        map_vw.r_confirm = reference;
        map_vw.upd_ui.group = *group_list;
        map_vw.order_ui = GROUP_LIST_REQUEST;

        q = MAP_OK;
    }
    else
    {
        q = MAP_RNA;
    }
    pi_post_semaphore (map_sema, &status);

    return q;
}


      /*
         MAP_SETDEFAULTGROUPLIST
       */

map_result
map_SetDefaultGroupList (unsigned short length,
                         map_VehicleGroupList * group_list)
{

    /*
       This function describes the default group membership only localy
       on each node. Called by Node supervisor during start up. Each node must have
       the same group_list contend (basic entrys)!
       This function call is allowed only in SETUP mode or earlier!return values:
       MAP_OK   ok
       MAP_NP   not SETUP_MODE, not allowed
     */
    unsigned short j, i, group, node_ix, node;
    map_result q;

    if (map_vw.phase == SETUP_MODE) {

        /*
           GROUP LIST RESET
         */

        for (i = 0; i < MAP_MAX_NO_GRP; i++) {
            GROUP_LIST[i].length = 0;

            for (j = 0; j < MAP_MAX_NO_NGL; j++) {
                GROUP_LIST[i].node[j] = 0;
            }
            map_xgroup_list_ui[i] = GROUP_LIST[i];
            map_ygroup_list_ui[i] = GROUP_LIST[i];
        }

        q = MAP_OK;

        for (j = 0; j < length; j++) {
            /* group number is index onto GROUP_LIST */
            group = group_list->member[j].group - 1;

            if (group <= MAP_MAX_NO_GRP) {
                node = group_list->member[j].node;
                node_ix = GROUP_LIST[group].length;
                GROUP_LIST[group].node[node_ix] = node;
                GROUP_LIST[group].length++;
                map_ygroup_list_ui[group] = GROUP_LIST[group];
                map_xgroup_list_ui[group] = GROUP_LIST[group];
            }
        }
    } else {
        q = MAP_NP;
    }
    return q;
}

/* MAP_SETVEHICLEDESCRIPTOR */

map_result map_SetVehicleDescriptor (unsigned char caller,
                                     map_VehicleDescr * descriptor,
                                     map_setvhdescr_conf confirmation,
                                     void *reference)
{
    /*
       This function sets a new vehicle descriptor. This can be done either when a
       dynamic attribute changes or when a vehicle description shall be inserted
       by operating staff.
       Return values:
       MAP_OK   ok
       MAP_NP   the node address of the vehicle descriptor is unknown in NADI
       MAP_RNA  another is working
    */
    short status;
    map_result q;
    unsigned char i;

    caller = caller;
    pi_pend_semaphore (map_sema, PI_FOREVER, &status);

    if (map_vw.order_ui == NEXT_ORDER)
    {
        q = MAP_NP;
        for (i = 0; i < NADI.state.VehicleNo; i++)
        {
            if (NADI.vehicle[i].NodeAddr == descriptor->NodeAddr)
            {
                map_vw.upd_ui.vehicle = *descriptor;
                map_vw.order_ui = DYN_ATTR_REQUEST;
                map_vw.f.setvhdescr = confirmation;
                map_vw.r_confirm = reference;

                q = MAP_OK;
                break;
            }
        }
    }
    else
    {
        q = MAP_RNA;
    }
    pi_post_semaphore (map_sema, &status);

    return q;
}

/* MAP_CANCELREQUEST */

map_result map_CancelRequest (unsigned char caller)
{
    /*
       Cancels a service request witch is not yet confirmed.
       Return values:
       MAP_OK   ok
    */
    short status;
//    map_result q;

    caller = caller;
    pi_pend_semaphore (map_sema, PI_FOREVER, &status);

    if (map_vw.order_ui == NEXT_ORDER)
    {
        map_vw.order_ui = CANCEL_REQUEST;
    }
    else
    {
        map_vw.order_ui = NEXT_ORDER;
    }
//    q = MAP_OK;

    pi_post_semaphore (map_sema, &status);

    return MAP_OK;
}

/* MAP_MULTICAST_REQU */

map_result map_multicast_requ ( unsigned char caller_fct,
                                AM_NET_ADDR * replier,
                                void *out_msg_addr,
                                unsigned int out_msg_size,
                                MAP_INVOKE_CONF confirm,
                                void *ext_ref)
{
    /*
       Requests a telegram transmission using the multicast protocol. This service is
       only available on WTB level.
       Return values:
       MAP_OK    ok
       MAP_RNA   another is working
       MAP_NP    only allowed on WTB
    */
    short status;
    map_result q;
//    unsigned char i;

    pi_pend_semaphore (map_sema, PI_FOREVER, &status);

    if (map_vw.f1st_run_s == NPT)
    {
        /* PERFORM ON WTB ONLY */

        if (map_vw.order_ui == NEXT_ORDER)
        {
            /* Parameter store for map_npt */
            map_vw.order_ui = MULTICAST;
            map_vw.f.multicast = confirm;
            map_vw.r_confirm = (void *) ext_ref;

            map_vw.mc_tgm = (unsigned char *) out_msg_addr;
            map_vw.mc_tgm_length = out_msg_size;
            map_vw.mc_replier = *replier;
            map_vw.mc_caller = caller_fct;

            q = MAP_OK;
        }
        else
        {
            q = MAP_RNA;
        }
    }
    else
    {
        q = MAP_NP;
    }
    pi_post_semaphore (map_sema, &status);

    return q;
}
