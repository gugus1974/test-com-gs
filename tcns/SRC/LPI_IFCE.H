/*   LPI_IFCE.H
 ----------------------------------------------------------------------------- 
                                                                               
     Property of  :   ABB Verkehrssyteme AG, CH-8050 ZÅrich / Switzerland      
     COPYRIGHT    :   (c) 1991 ABB Verkehrssysteme AG                          
                                                                               
 ----------------------------------------------------------------------------- 
                                                                               
     Project      :   IEC - TCN                                                
     Subproject   :   Link Process                                             
                                                                               
 ----------------------------------------------------------------------------- 
                                                                               
     File         :   LPI_IFCE.H   (Link Process Procedures)                   
                                                                               
     Document     :   [1]: IEC WG 22-5, Upper Layers, Version 1.3              
                      [2]: Link Layer Process Specification                    
		      [3]: Link Layer Process Design

     Abstract     :   Import interface not covered by PIL                      
                      functionality implemented differently for TCN
                      than for MicOS (for hardware reasons)                    
                                                                               
     Remarks      :
                                                                               
     Dependencies :                                                            
                                                                               
     Accepted     :                                                            
                                                                               
 ----------------------------------------------------------------------------- 
                                                                               
     HISTORY:                                                                  
                                                                               
     Version    YY-MM-DD     Name           Dept.      Reference     Status    
     -------    --------     ----------     ------     ---------     -------   
        1       91-09-16     W. Marsden     BAEI-2         --                  
                                                                               
 ----------------------------------------------------------------------------- 
*/

#ifndef         LPI_IFCE_H
#define         LPI_IFCE_H

#include "lp_sys.h"

#if defined (O_PCMCIA)
#include "lpp_pcmc.h"
#endif

#if     defined (O_RP) || defined (O_KHW)
  #include  "lpp_page.h"
#endif

#if defined (O_PC)
  #include  "bfcomm.h"
  #include  "lc_sys.h"
  #include  "du_sys.h"

#elif defined (O_960)
  #include  "bfcomm.h"
  #include  "lc_sys.h"
  #if !defined (O_BAP)
  #include  "mvbc.h"
  #endif
#else
  #include  "lc_sys.h"
#endif

  #include  "lpa_as2c.h"

#if     defined (O_RP) || defined (O_KHW)

/*      --------------------------------------------------      */

#define     lpi_clr_pit         lpp_clr_pit
#define     lpi_get_prt_indx    lpp_get_prt_indx
#define     lpi_put_prt_indx    lpp_put_prt_indx

#else

#define     lpi_clr_pit         lpl_clr_pit
#define     lpi_get_prt_indx    lpl_get_prt_indx
#define     lpi_put_prt_indx    lpl_put_prt_indx

#if defined (O_TCN) || defined (O_PD_TEST)
#define     lpi_get_prt_indx_via_pvn    lpl_get_prt_indx_via_pvn
#else
#define     lpi_get_prt_indx_via_pvn    lpl_get_prt_indx
#endif

#endif



#if defined (O_RP) || defined (O_EKR)
#define     lpi_mbox_slave_lock     lpm_mbox_slave_lock
#define     lpi_mbox_slave_unlock   lpm_mbox_slave_unlock
#define     lpi_mbox_create         lpm_mbox_create
#define     lpi_mbox_activate       lpm_mbox_activate

#else
#define     lpi_mbox_slave_lock(p_cb)     0         /*  empty function  */
#define     lpi_mbox_slave_unlock(p_cb)             /*  empty function  */
#define     lpi_mbox_create(p_cfg)              /*  empty function  */
#define     lpi_mbox_activate(p_cfg)        /*  empty function  */

#endif

#if     defined (O_EKR)
#define     lpi_hw_enable       lpm_hw_enable

#else
#define     lpi_hw_enable(hw_type, p_cfg)       /* empty function */
#endif



#define     LPI_DB  DU_DB
/*
 -----------------------------------------------------------------------------
|
| Redirections  Project Hardware Interface
|
 -----------------------------------------------------------------------------
*/

#if defined (O_KHW)

	#define     lpi_lock()      pi_lock_task()
	#define     lpi_unlock()    pi_unlock_task()

#elif defined   (O_RP) || defined (O_EKR) || defined (O_PC)

	#define     lpi_lock()      pi_disable()
	#define     lpi_unlock()    pi_enable()

#else
	#define     lpi_lock()      pi_lock_task()
	#define     lpi_unlock()    pi_unlock_task()

#endif

#if defined (O_EKR) || defined (O_PC) || defined (O_RP) || defined (O_KHW) || defined (O_960)
    #define lpi_hw_bap_1_start()   { bf_bap_ready (); \
				     bf_bap_running(); }
#else
    #define lpi_hw_bap_1_start()   /*  !!! replace this function */
#endif

#if defined (O_PCMCIA)


    #define lpi_put_ts_short lpp_put_ts_short
    #define lpi_get_ts_short lpp_get_ts_short
    #define lpi_put_ts_array lpp_put_ts_array
    #define lpi_get_ts_array lpp_get_ts_array
    #define lpi_set_ts_array lpp_set_ts_array

    #define lpi_put_pit lpp_put_pit
    #define lpi_get_pit lpp_get_pit

    #define lpi_put_pcs lpp_put_pcs
    #define lpi_get_pcs lpp_get_pcs
    #define lpi_tgl_pcs lpp_tgl_pcs
    #define lpi_inc_pcs lpp_inc_pcs

    #define lpi_put_pcs_mvbc lpp_put_pcs
    #define lpi_put_pcs_bap  lc_put_pcs_bap


#else

    #define lpi_put_ts_array(p_cb,a,b,c)  pi_copy16(a,b,c)
    #define lpi_get_ts_array(p_cb,a,b,c)  pi_copy16(a,b,c)
    #define lpi_set_ts_array(p_cb,a,b,c)  lpi_memset(a,b,c)
    #define lpi_put_ts_short(p_ts, p_ap)  { *p_ts = *p_ap; }
    #define lpi_get_ts_short(p_ap, p_ts)  { *p_ap = *p_ts; }

    #define lpi_put_pit(p_cb,a,b,c)        lc_put_pit(a,b,c)
    #define lpi_get_pit(p_cb,a,b,c)        lc_get_pit(a,b,c)

    #define lpi_put_pcs(p_cb,a,b,c,d)      lc_put_pcs(a,b,c,d)
    #define lpi_get_pcs(p_cb,a,b,c,d)      lc_get_pcs(a,b,c,d)
    #define lpi_tgl_pcs(p_cb,a,b,c)        lc_tgl_pcs(a,b,c)
    #define lpi_inc_pcs(p_cb,a,b,c)        lc_inc_pcs(a,b,c)

    #define lpi_put_pcs_mvbc(p_cb,a,b,c,d) lc_put_pcs_mvbc(a,b,c,d)
    #define lpi_put_pcs_bap  lc_put_pcs_bap

#endif

/*
 -----------------------------------------------------------------------------
|
| Redirections  Debug Utility
|
 -----------------------------------------------------------------------------
*/

				   #if defined (O_PD_TEST)
/* Fake Hamster */
#include <stdio.h>

#define LPI_DU_POS                __FILE__, __LINE__
#define LPI_DU_ERR               "error"
#define LPI_DU_WARN              "warning"
#define LPI_DU_CONT              "cont."
#define LPI_DU_HALT              "halt"
#define LPI_DU_RESET             "reset"
#define LPI_DU_SUSP              "susp."

#define lpi_hamster_cond(cond, type, action, expl)                 \
{ if ((cond)) \
{ printf ("EXCEPTION - File=%s, Line=%d, cond=%d, %s \n", __FILE__, __LINE__, cond, expl); } }
#define lpi_hamster_uncond(    type, action, expl)                 \
{ printf ("EXCEPTION - File=%s, Line=%d, %s \n", __FILE__, __LINE__, expl); }

				   #else

				       #ifdef O_TCN

/* IEC TCN Event Recorder */

#include "recorder.h"


#define LPI_DU_POS                __FILE__,__LINE__
#define LPI_DU_ERR                NSE_EVENT_TYPE_ERROR
#define LPI_DU_WARN               NSE_EVENT_TYPE_WARNINGS
#define LPI_DU_CONT               NSE_ACTION_CONTINUE
#define LPI_DU_HALT               NSE_ACTION_HALT
#define LPI_DU_RESET              NSE_ACTION_REBOOT
#define LPI_DU_SUSP               NSE_ACTION_SUSPEND

#define lpi_hamster_cond(cond, type, action, expl)                  \
	{ if ((cond)) { nse_record (LPI_DU_POS, type, action, 0,0); } }
#define lpi_hamster_uncond(    type, action, expl)                  \
	{           { nse_record (LPI_DU_POS, type, action, 0,0); } }

				       #else
/* Real Hamster */
#include "du_sys.h"

#define LPI_DU_POS                DU_POSITION
#define LPI_DU_CONT               DU_CONTINUE
#define LPI_DU_HALT               DU_HALT
#define LPI_DU_RESET              DU_RESET
#define LPI_DU_SUSP               DU_SUSP

#define lpi_hamster_cond(cond, type, action, expl)                      \
	{ if ((cond)) { du_hamster_text (LPI_DU_POS, action, expl); } }
#define lpi_hamster_uncond(    type, action, expl)                      \
	{             { du_hamster_text (LPI_DU_POS, action, expl); } }

				       #endif
				   #endif
				   #if (0)
				       #if defined (O_TCN)

   #define LPI_POSITION __FILE__,__LINE__

#define lpi_nse_record(type, action) \
	nse_record (LPI_POSITION, type, action, 0, (UINT8 *) 0x0L);

#define hamster_continue(COND)                                              \
	{ if((COND))                                                        \
	  { lpi_nse_record (NSE_EVENT_TYPE_WARNINGS, NSE_ACTION_CONTINUE); }\
	}

#define hamster_suspend(COND)                                               \
	{ if((COND))                                                        \
	  { lpi_nse_record (NSE_EVENT_TYPE_ERROR, NSE_ACTION_SUSPEND); }    \
	}

#define hamster_halt(COND)                                                  \
	{ if((COND))                                                        \
	  { lpi_nse_record (NSE_EVENT_TYPE_ERROR, NSE_ACTION_HALT); }       \
	}

#define hamster_reset(COND)                                                 \
	{ if((COND))                                                        \
	  { lpi_nse_record (NSE_EVENT_TYPE_ERROR, NSE_ACTION_REBOOT); }     \
	}

#define lpi_fatal(condition, p_result)                                      \
	{  	if ((condition))                                            \
	  {                                                                 \
		*p_result = condition;                                      \
		lpi_nse_record (NSE_EVENT_TYPE_ERROR, NSE_ACTION_REBOOT);   \
		return (*p_result);                                         \
	  }                                                                 \
	}

#define lpi_dbf_fatal(condition)                        \
		{                                               \
			hamster_reset(condition);                   \
		}

#define lpi_dbs_fatal_0     lpi_fatal
#define lpi_dbf_fatal_0     lpi_fatal
#define lpi_dbs_fatal_1     lpi_fatal
#define lpi_dbf_fatal_1     lpi_fatal
#define lpi_dbs_fatal_2     lpi_fatal
#define lpi_dbf_fatal_2     lpi_fatal

				       #else
#include "du_sys.h"
#define lpi_dbf_fatal(condition)    lpi_dbf_fatal_0(condition, NULL)

#define lpi_dbs_fatal_0(condition, pointer) {hamster_reset (condition);}
#define lpi_dbf_fatal_0(condition, pointer) \
    { du_dblevel0 (condition, hamster (DU_POSITION, DU_RESET, 1, condition));}

			#if (LPI_DB >= 1)

#define lpi_dbs_fatal_1(condition, pointer) {hamster_reset (condition);}
#define lpi_dbf_fatal_1(condition, pointer) \
    { du_dblevel1 (condition, hamster (DU_POSITION, DU_RESET, 1, condition));}

			#else
#define lpi_dbs_fatal_1(condition, pointer) /* debug level not suprtd */
#define lpi_dbf_fatal_1(condition, pointer) /* debug level not suprtd */

			#endif

			#if (LPI_DB >= 2)

#define lpi_dbs_fatal_2(condition, pointer) {hamster_reset (condition);}
#define lpi_dbf_fatal_2(condition, pointer) \
    { du_dblevel2 (condition, hamster (DU_POSITION, DU_RESET, 1, result));}

			#else
#define lpi_dbs_fatal_2(condition, pointer) /* debug level not suprtd */
#define lpi_dbf_fatal_2(condition, pointer) /* debug level not suprtd */
			#endif

#endif
#endif
/*
 -----------------------------------------------------------------------------
|
| Redirections  Software Interface Library
|
 -----------------------------------------------------------------------------
*/

#if defined (O_EKR) || defined (O_RP) || defined (O_KHW)

#define     lpi_memset(p_start, value, count_8) \
		   {si_memset16 (p_start, value, (((unsigned)(count_8)) >> 1));}

#if defined (O_EKR)
#define     lpi_copy16  lpa_copy_16
#else
#define     lpi_copy16  pi_copy16
#endif

#else

#define     lpi_memset(p_start, value, count_8) \
	    { lpa_memset(p_start, value, count_8); }

#define     lpi_copy16  pi_copy16

#if defined (O_960)
void        xx_t_config   (void *   poi2dpr);
#define     lpi_tb_announce_ts_cfg(p_ts_cfg) xx_t_config(p_ts_cfg)
#else
#define     lpi_tb_announce_ts_cfg(p_ts_cfg)
#endif

#endif

#if !defined (O_PD_TEST)
#define lpi_lc_int_connect      lc_int_connect
#define lpi_lc_int_disconnect   lc_int_disconnect
#else
#define lpi_lc_int_connect(a,b,c,d)  0
#define lpi_lc_int_disconnect(param) 0
#endif

#define LPI_FRC_DTA             0
#define LPI_FRC_MSK             1
#define LPI_FRC_MSK_WTB         4

struct LPI_LC_STR_TS_PGE
{
    LC_TYPE_TS_PGE      pge;
} ;

					#if !defined (O_BAP)

#define LPI_TM_STSR_CYC_MSK     0xF000
#define LPI_TM_STSR_CYC_VAL     1
#define LPI_TM_STSR_CYC_OFF     12

#define LPI_TM_STSR_RGE_MSK     0x0FFF
#define LPI_TM_STSR_RGE_OFF     0
#define LPI_TM_STSR_RGE_MAX     256

					    #if !defined (O_PD_SIMU)
#define lpi_lc_get_p_sa()       lc_m_get_service_area_addr (0)
					    #else

#include "mvbc.h"


extern  TM_TYPE_SERVICE_AREA    tm_simu_service_area;
#define lpi_lc_get_p_sa()       &tm_simu_service_area;
					    #endif

#define lpi_lc_get_mfr_pa(p_sa)               \
	  tm_get_sa(p_sa, mfs, TM_MFS_PRT_ADDR_MSK, TM_MFS_PRT_ADDR_OFF)

#define lpi_lc_get_busy(p_sa)   \
	  tm_get_sa(p_sa, int_regs.mr.w, TM_MR_BUSY_MSK, TM_MR_BUSY_OFF)

#define lpi_lc_wait(time_in_us) lc_wait (time_in_us)

					#endif


/*
 -----------------------------------------------------------------------------
|
| Redirections  Link Layer MVB
|
 -----------------------------------------------------------------------------
*/

#define     lp_subscribe_to_ll


/*
 -----------------------------------------------------------------------------
|
| Version 2     Modifications
|
 -----------------------------------------------------------------------------



*/

#endif
