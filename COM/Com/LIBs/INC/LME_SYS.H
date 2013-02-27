/****************************************************************************
*                                                                           *
* Property of   : ABB Verkerssysteme AG, CH-8050 Zuerich/Switzerland        *
*                                                                           *
* Copyright      : (c) 1990 ABB Verkehrsysteme AG                           *
*****************************************************************************
*                                                                           *
* Project       : IEC TCN - Train Network Management                        *
*                                                                           *
* Subproject    : Layer Management Entity (LME)                             *
*                                                                           *
*****************************************************************************
*                                                                           *
* Module        : lme_sys.h                                                 *
*                                                                           *
* Document      : -                                                         *
*                                                                           *
* Abstract      : Definitions of types, constants and prototypes            *
*                                                                           *
* Remarks       : Most of types for LME are defined in included *.h files   *
*                                                                           *
* Dependencies  : all included *.h files                                    *
*                                                                           *
* Accepted    	:                                                           *
*                                                                           *
*****************************************************************************
*                                                                           *
* History       :                                                           *
*                                                                           *
*	Version	YY-MM-DD Name  Dept. Ref Status                                   *
* ------- -------- ----  ----- --- ------	                                  *
*	  1.0   95.10.10 Flum  BATL   -  created                                  *
*                                                                           *
****************************************************************************/


#ifndef LME_SYS_H
  #define LME_SYS_H

#include "am_group.h"   /* for message data LME's     */
#include "lc_sys.h"
#include "bax_incl.h"   /* for busadministrator LME's */
#include "lp_sys.h"     /* for process data LME's     */
#include "apd_incl.h"   /* for process data LME's     */
#include "am_sys.h"     /* for message data LME's     */



/****************************************************************************
*                                                                           *
*                               defines                                     *
*                                                                           *
****************************************************************************/


#define VERSION  1
#define RELEASE  0
     
/****************************************************************************
*                                                                           *
*  LME return values issue from Process Data Application Layer LME's        *
*                                                                           *
****************************************************************************/


#define LME_APD_OK          0   /* correct function call termination */
#define LME_APD_PRT_PASSIVE 1   /* target data set not active        */
#define LME_APD_ERROR       2   /* unspecified failure               */
#define LME_APD_CONFIG      3   /* configuration error occured       */
#define LME_APD_MEMORY      4   /* not enough memory available       */
#define LME_APD_UNKNOWN_TS  5   /* unknown traffic store             */
#define LME_APD_RANGE       6   /* unspecified out of range error    */
#define LME_APD_DATA_TYPE   7   /* unsuported data type              */


/****************************************************************************
*                                                                           *
*  LME return values issue from Process Data Link Layer LME's               *
*                                                                           *
****************************************************************************/


#define LME_LP_ERROR            -1  /* general error                       */
#define LME_LP_OK                0  /* command successfully executed       */
#define LME_LP_PRT_PASSIVE       1  /* no copying of this variable         */
#define LME_LP_PRT_ADDR_O        2  /* port address overflow               */
#define LME_LP_CONFIG            3  /* configuration error                 */
#define LME_LP_MEMORY            4  /* Allocation error                    */
#define LME_LP_RANGE             5  /* port address > port address max     */
                                    /* port index > port index max         */
                                    /* indication nr > indication nr max   */
                                    /* hw_type > hw_type_max               */
#define LME_LP_UNKNOWN_TS_ID     6  /* traffic store ident. out of range   */
#define LME_LP_UNKNOWN_DATA_TYPE 7  /* undefined data type used            */


/****************************************************************************
*                                                                           *
*  LME return values issue from Link Layer Common LME's (MVB)               *
*                                                                           *
****************************************************************************/


#define LME_LC_OK             0   /* operation terminated successfully */
#define LME_LC_REJECT        !0   /* operation failed                  */


/****************************************************************************
*                                                                           *
*  LME return values issue from Message Data LME's                          *
*                                                                           *
****************************************************************************/


#define LME_AM_OK                   0  /* normal success                   */
#define LME_AM_FAILURE              1  /* unspecified failure              */
#define LME_AM_BUS_ERR              2  /* no bus transmission possible     */
#define LME_AM_REM_CONN_OVF         3  /* too many incoming connections    */
#define LME_AM_CONN_TMO_ERR         4  /* connect request not answered     */
#define LME_AM_SEND_TMO_ERR         5  /* send timeout (connect was ok)    */
#define LME_AM_REPLY_TMO_ERR        6  /* no reply received                */
#define LME_AM_ALIVE_TMO_ERR        7  /* no complete message received     */
#define LME_AM_NO_LOC_MEM_ERR       8  /* not enough memory                */
#define LME_AM_NO_REM_MEM_ERR       9  /* not enough memory at partner     */
#define LME_AM_REM_CANC_ERR         10 /* cancelled by partner             */
#define LME_AM_ALREADY_USED         11 /* same operation already done      */
#define LME_AM_ADDR_FMT_ERR         12 /* address format error             */
#define LME_AM_NO_REPLY_EXP_ERR	    13 /* no such reply expected           */
#define LME_AM_NR_OF_CALLS_OVF      14 /* too many calls requested         */
#define LME_AM_REPLY_LEN_OVF        15 /* reply message too long           */
#define LME_AM_DUPL_LINK_ERR        16 /* duplicated link error            */
#define LME_AM_MY_DEV_UNKNOWN_ERR   17 /* my device unknown or not valid   */
#define LME_AM_NO_READY_INST_ERR    18 /* no ready replier  instance       */
#define LME_AM_NR_OF_INST_OVF       19 /* too many replier instances       */
#define LME_AM_CALL_LEN_OVF         20 /* call message too long            */
#define LME_AM_UNKNOWN_DEST_ERR     21 /* partner unknown (not existing)   */
#define LME_AM_INAUG_ERR            22 /* train inauguration occurred      */
#define LME_AM_TRY_LATER_ERR        23 /* (internally used only)           */
#define LME_AM_DEST_NOT_REG_ERR	    24 /* not registered at caller's site  */
#define LME_AM_GW_DEST_NOT_REG_ERR  25 /* dest. not registered on gateway  */
#define LME_AM_GW_SRC_NOT_REG_ERR   26 /* source not registered on gateway */


/****************************************************************************
*                                                                           *
*                           Connection to the user                          *
*                                                                           *
****************************************************************************/


#define lme_lc_read_mvb_status          lc_read_mvb_status
#define lme_ba_task                     bam_main
#define lme_lc_mvb_line                 lc_mvb_line
#define lme_ba_subscribe_diagnosis      ba_subscribe_diagnosis
#define lme_lp_read_port_configuration  lp_read_port_configuration
#define lme_apd_get_variable            apd_get_variable
#define lme_apd_force_variable          apd_force_variable
#define lme_apd_unforce_variable        apd_unforce_variable
#define lme_apd_unforce_all             apd_unforce_all
#define lme_am_read_messenger_status    am_read_messenger_status
#define lme_am_write_messenger_control  am_write_messenger_control
#define lme_am_get_fdir_entry           am_get_dir_entry
#define lme_am_insert_fdir_entries      am_insert_dir_entries
#define lme_am_sdir_read                am_st_read
#define lme_am_sdir_write               am_st_write
#define lme_am_is_member_of_gdir        am_is_member_of
#define lme_am_write_gdir               am_join_groups
#define lme_am_leave_gdir               am_leave_groups


/****************************************************************************
*                                                                           *
*                            types                                          *
*                                                                           *
****************************************************************************/


typedef int             LME_TYPE_APD_RESULT;
typedef int             LME_TYPE_LP_RESULT;
typedef int             LME_TYPE_LC_RESULT;
typedef int             LME_TYPE_AM_RESULT;


#endif
