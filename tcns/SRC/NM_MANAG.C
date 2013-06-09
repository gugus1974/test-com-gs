#if !(defined (NM_1VB) || defined (NM_1VB_SIMPLE) || defined (NM_GW_NO_TB))
/************************************************************************/
/*  Asea Brown Boveri Research Center, CH-5405 Baden, Switzerland       */
/*  Copyright 1991                                                      */
/*                                                                      */
/*  NAME:                                                               */
/*    NM_MANAG.C                                                        */
/*    NM = Network interface for Messages                               */
/*                                                                      */
/*  ABSTRACT:                                                           */
/*    TCN Message Transport Protocol, Network layer management.         */
/*    This module is used in the VB/TB gateway or a TB end-system only. */
/*    It contains the train configuration part of the network layer.    */
/*    This module is the only part of the messenger that communicates   */
/*    with the train inauguration software.                             */
/*    The messenger calls 'nm_mng_system_init' during 'am_init' to do   */
/*    module internal initializations and he needs the following        */
/*    functions of this module:                                         */
/*                                                                      */
/*    - nm_status_ind: the messenger specifies this function as TB      */
/*            status indication handler when he calls 'lm_t_init'       */
/*            (parameter (*status_indicate)). Thus this function will   */
/*            be called when a TB inauguration starts or completes.     */
/*            The messenger must be informed about the own vehicle      */
/*            address. This function must therefore call                */ 
/*            "md_set_my_vehicle (AM_SAME_VEHICLE)" when train inaugu-  */
/*            ration starts and "md_set_my_vehicle (my_vehicle)" when   */
/*            train inauguration completes. "md_set_my_vehicle" is part */
/*            of the messenger, and the information "my_vehicle" can    */
/*            be fetched from the TB link layer.                        */
/*                                                                      */
/*    - nm_vehicle_exists: the messenger only sends packets on the TB   */
/*            if the destination vehicle exists. He acquires this       */
/*            information by calling function "nm_vehicle_exists".      */
/*            It is a module internal decision how to implement this    */
/*            function. The necessary information is available from     */
/*            the train inauguration software.                          */            
/*                                                                      */
/*  RESTRICTIONS !!!:                                                   */
/*    The current version is preliminary only and does not yet use      */
/*    the train inauguration software. Instead the vehicle address      */
/*    is supposed to be fix and must be defined with 'nm_set_vehicle'   */
/*    (this function will disappear in the final version).              */
/*  COMPILATION DEFINES:                                                */
/*    This is now a final version, the above restriction is no more     */
/*    valid! The old, preliminary version can still be obtained by      */
/*    defining NO_LS at compile time.                                   */
/*                                                                      */
/************************************************************************/
/*                                                                      */
/*  AUTHOR  :   12-Mar-1992     CRBC1   Hoepli                          */
/*  MODIFIED:   05-May-1993     CRBC1   Hoepli    comments only         */
/*              02-Jun-1993     CRBC1   Hoepli    now uses ls_t_* fcts  */
/*              11-Nov-1993     CRBC1   Hoepli    discard group         */
/*              01-Dec-1993     CRBC1   Hoepli    changed to WTB ls_t_* */  
/*                (use #define GENF for previous ls_t_* interface )     */
/*              31-Jan-1994     CRBC1   Hoepli  #ifdef for whole module */
/*              09-Feb-1995     CRBC1   Leeb    Adaption to WTB 1.9     */
/*              02-May-1995     CRBC1   Leeb    hamster_halt removed    */
/*                                              when Topography invalid */
/*              09-Jun-1995     CRBC1   Leeb    update for RTP 4.xx     */
/************************************************************************/

#define DU_MODULE_NAME "NM_MANA"
/* for hamster calls which are now mapped to JDP event recorder.
	DU_MODULE_NAME must be defined before "du_sys.h" is included.
*/

/*
#define NO_LS
to generate the old, preliminary version with a constant vehicle address.
*/

#include "md_ifce.h"
#include "pi_sys.h"
#include "am_sys.h"
#include "basetype.h"
#include "md_layer.h"
#include "nm_manag.h"


#ifndef NO_LS
#ifdef GENF
#include "ls_layer.h"
#else
#include "bmi.h"       /* supplied by ABB Henschel */
#endif
#endif

#define LS_TOPO_SIZE            64

/* VARIABLES: --------------------------------------------------------- */

#ifdef NO_LS
static unsigned char my_const_vehicle; /* !!! will disappear! */
#else
#ifdef GENF
static const LS_TYPE_TOPOLOGY_FIELD *topo_p;    /* pointer to TB topology */
static const LS_TYPE_STATUS_FIELD   *status_p;  /* pointer to TB status */
#else /* WTB */
static Type_Topography   tb_topo;
static unsigned char  tb_exists[LS_TOPO_SIZE];  /* boolean, index = vehicle */
#endif /* GENF */
#endif /* NO_LS */
static AM_RESULT veh_exists_err;
/* result for 'nm_vehicle_exists' in case of an error. The reason may be
   that there is no such vehicle or that inauguration is in execution.
*/

/* FUNCTIONS: --------------------------------------------------------- */

#ifdef NO_LS
void nm_set_vehicle (unsigned char vehicle)
/* !!! This function will not exist anymore as soon as dynamic train
   configuration management is implemented.
*/
{
  my_const_vehicle = vehicle;
} /* nm_set_vehicle */
#endif

/**********************************************************************/

void nm_status_ind ( int status)
{
  if (status == MD_OK)
  {
#ifdef NO_LS
	 /* !!! later: get my vehicle address from link layer management */
	 hamster_halt (my_const_vehicle == AM_SAME_VEHICLE); /* CL 17/12/92 */
	 md_set_my_vehicle (my_const_vehicle);
	 veh_exists_err = AM_UNKNOWN_DEST_ERR;
#else
#ifdef GENF
	 md_set_my_vehicle (status_p->link_adr);
	 veh_exists_err = AM_UNKNOWN_DEST_ERR;
#else /* WTB: tb_topo, tb_exists and my_vehicle/my_tc */
	 {
	 unsigned short ix;
	 unsigned char  tc;

	if (ls_t_GetTopography (&tb_topo) == L_OK)
	{
		 for (ix = 0; ix < LS_TOPO_SIZE; ix++) { /* first clear all */
			tb_exists[ix] = 0;
		 } /* for */

		 if (tb_topo.bottom_address == 1)
		 {
			for (ix = 1; ix <= tb_topo.top_address; ix++)
				tb_exists[ix] = !0;
		 }
		 else
		 {
			for (ix = tb_topo.bottom_address; ix < 64; ix++)
				tb_exists[ix] = !0;
			for (ix = 1; ix <= tb_topo.top_address; ix++)
				tb_exists[ix] = !0;
		 }
		 tb_exists[0] = !0;  /* myself */
		 /* update externals my_vehicle and my_tc: */
		 if (tb_topo.topo_counter <= AM_MAX_TC) {
		  hamster_continue (tb_topo.topo_counter == AM_ANY_TC);
		  tc = tb_topo.topo_counter;
		 } else {
		  hamster_continue (!0);
		  tc = AM_MAX_TC;
		 }
		 md_set_my_veh_and_tc (tb_topo.node_address, tc);
		 veh_exists_err = AM_UNKNOWN_DEST_ERR;
	}
	 }
#endif /* GENF */
#endif /* NO_LS */
  }
  else
  {
	 md_set_my_vehicle (AM_SAME_VEHICLE);
    veh_exists_err = AM_INAUG_ERR;
  }
} /* nm_status_ind */

/**********************************************************************/

AM_RESULT nm_vehicle_exists (unsigned char *vehicle)
/* vehicle without physical address bit */
{
  if (veh_exists_err != AM_INAUG_ERR)
  {
    if (*vehicle & AM_GROUP_BIT) {
      *vehicle = AM_ALL_VEH;
      return AM_OK;
    }
    if (*vehicle >= LS_TOPO_SIZE)   /* 11-Nov-1993, instead of hamster */
    {
      return AM_UNKNOWN_DEST_ERR;
    }
#ifdef NO_LS
    return AM_OK;
#else
#ifdef GENF
    switch (topo_p->node[*vehicle].status) 
    {
      case LS_NODE_NONE:  return AM_UNKNOWN_DEST_ERR;
      case LS_NODE_OK:    return AM_OK;
      case LS_NODE_ERROR: return AM_BUS_ERR;
      default:            hamster_continue (!0);
			  return AM_FAILURE;
    } 
#else  /* WTB */
    return (tb_exists[*vehicle] ? AM_OK : AM_UNKNOWN_DEST_ERR);
#endif /* GENF */
#endif /* NO_LS */
  }
  else
  {
    return veh_exists_err;
  }
} /* nm_vehicle_exists */

/**********************************************************************/

void nm_mng_system_init (void)
{
  veh_exists_err = AM_INAUG_ERR;
#ifdef NO_LS
  my_const_vehicle = AM_SAME_VEHICLE;
#else
#ifdef GENF
  topo_p = ls_t_get_topology ();
  hamster_halt (topo_p == NULL);
  status_p = ls_t_get_status ();
  hamster_halt (status_p == NULL);
#endif /* GENF */
#endif /* NO_LS */
} /* nm_mng_system_init */
#endif
