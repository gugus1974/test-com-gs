/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> the QNX compiler prefers 960-like definitions                                       */
/*==============================================================================================*/


/************************************************************************/
/*  Asea Brown Boveri Research Center, CH-5405 Baden, Switzerland       */
/*  Copyright 1993                                                      */
/*                                                                      */
/*  NAME:                                                               */
/*    LM_PARAM.C                                                        */
/*                                                                      */
/*  ABSTRACT:                                                           */
/*    Link Layer Parametrization for TCN Message Data.                  */
/*    This module is used by the network layer to initialize all        */
/*    underlying link layers and to provide a hardware defined station  */
/*    address. It is not part of the messenger but must be provided for */
/*    the messenger together with the link layers. The messenger does   */
/*    not call the link layer functions directly, he cannot do so       */
/*    because he does not know which link layers are present. Instead   */
/*    each link layer must connect itself to the messenger. The link    */
/*    layers are decoupled from the messenger by the link dispatcher    */
/*    NM_DISP.C which provides a jump table for the link layer func-    */
/*    tions. This jump table is filled when the link layers are connec- */
/*    ted. This module is responsable to connect the link layers. Thus  */
/*    it depends on which busses are connected to a station and must be */
/*    changed by the system configurator.                               */
/*                                                                      */
/*  CONFIGURATION DEFINES:                                              */  
/*    This original module supplies some versions for standard configu- */
/*    rations. The defines NM_1VB_SIMPLE, NM_GW_SIMPLE and              */
/*    NM_1TB, which are described in the header of module NM_LAYER.C,   */
/*    are used for conditional compilation and allow to generate such   */
/*    a standard version. This module can be used without change if one */
/*    of these defines is present (simple routing).                     */  
/*                                                                      */
/************************************************************************/
/*                                                                      */
/*  AUTHOR  :   09-Sep-1993     CRBC1   Hoepli                          */
/*  MODIFIED:   05-Jan-1994     CRBC1   Hoepli  NM_1VB not standard     */
/*              27-Jan-1994     CRBC1   Hoepli  #ifdef O_960            */
/*              07-Mar-1994     CRBC1   Hoepli  define DU_MODULE_NAME   */
/*              15-Sep-1995     BATL    Flum    lm_m_v_get_own_phya     */
/*                                              a. lm_m_v_get_src_phya  */
/*                                              added                   */
/*                                                                      */
/************************************************************************/
/*                                                                      */
/*  CHANGES:                                                            */
/*                                                                      */
/*  1.      95-02-28  BATL  zur Bonsen  Renamed to LX_PARAM.C           */
/*  1.1     95-09-15  BATL  Flum        lm_m_v_get_own_phya and         */
/*                                      lm_m_v_get_src_phya added       */
/*                                                                      */
/*                                                                      */
/************************************************************************/

#define DU_MODULE_NAME "LM_PARA"
/* for hamster calls which are now mapped to JDP event recorder.
   DU_MODULE_NAME must be defined before "du_sys.h" is included.
*/

/* some constructed configuration defines for conditional compilation: */
/* <ATR:01> */
#if defined(O_960) || defined(__QNX__)
/*#kra 24.01.94 syntax to compilerswitches changed (deleted: defined() )*/
#define NM_HAS_1VB (NM_1VB_SIMPLE || NM_GW_SIMPLE)
#define NM_HAS_TB  (NM_1TB || NM_GW_SIMPLE)
#else
#define NM_HAS_1VB (defined ( ## NM_1VB_SIMPLE ## ) || defined ( ## NM_GW_SIMPLE ## ))
#define NM_HAS_TB  (defined ( ## NM_1TB ## ) || defined ( ## NM_GW_SIMPLE ## ))
#endif



/* INCLUDES: ---------------------------------------------------------- */
#include "stdio.h"
#include "basetype.h"
#include "md_layer.h"
#include "lm_layer.h"
#include "am_sys.h"
#include "nm_disp.h"
#include "md_ifce.h"

/* VARIABLES: --------------------------------------------------------- */

#if NM_HAS_1VB
const NM_LINK_LAYER_FCTS vb_link_fcts =
  {lm_m_v_init, lm_m_v_get_status, lm_m_v_send_request, lm_m_v_send_queue_flush,
   lm_m_v_receive_poll, lm_m_v_get_own_phya, lm_m_v_get_src_phya};
/* if functions lm_m_v_get_own_phya and lm_m_v_get_src_phya  not be
   implemented in the link layer, set the entries in vb_link_fcts NULL */
#endif

#if NM_HAS_TB
const NM_LINK_LAYER_FCTS tb_link_fcts =
  {lm_m_t_init, lm_m_t_get_status, lm_m_t_send_request, lm_m_t_send_queue_flush,
   lm_m_t_receive_poll, NULL, NULL};
#endif

/* FUNCTIONS: --------------------------------------------------------- */

void lm_system_init (void)
{
#if NM_HAS_1VB
  lm_v_system_init ();
  hamster_halt (nm_disp_connect_bus (0, &vb_link_fcts));
#endif
#if NM_HAS_TB
  lm_t_system_init ();
  hamster_halt (nm_disp_connect_bus (AM_TB_NR, &tb_link_fcts));
#endif
} /* lm_system_init */

/************************************************************************/

void lm_get_own_station (unsigned short *own_station)
{
#if defined(O_MD_SAME_DEVICE)
  *own_station = (unsigned char) AM_SAME_DEVICE;
#else
#if NM_HAS_1VB
  phy_get_address (own_station);
#else
  *own_station = (unsigned char) AM_SAME_DEVICE;
#endif
#endif
} /* lm_get_own_station */


