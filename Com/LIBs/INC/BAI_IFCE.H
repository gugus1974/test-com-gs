/*
 ------------------------------------------------------------------------------
|
|    Property of  :   ABB Verkehrssysteme AG, CH-8050 ZÅrich / Switzerland
|    COPYRIGHT    :   (c) 1992 ABB Verkehrssysteme AG
|                     
 ------------------------------------------------------------------------------
|
|    Project      :   T C N   B U S   A D M I N I S T R A T O R
|
 ------------------------------------------------------------------------------
|                     
|    File Name    :   BAI_IFCE.H
|                     
|    Document     :
|                     
|    Abstract     :   Bus Administrator Import Interface 
|
|    Remarks      :
|
|    Dependencies :
|
|    Accepted     :
|                     
 ------------------------------------------------------------------------------
|
|    HISTORY:
|
|    Vers  Dept.   YY-MM-DD  State    Name    Reference
|    ----  ------  --------  -------  ------- ---------
|      0   BAEI-2  93-01-18  created  Marsden
|
 ------------------------------------------------------------------------------
*/

#ifndef BAI_IFCE_H
#define BAI_IFCE_H

                                    #if (0)
#include    <stdio.h>
#include    "logLib.h"
#define     vch_func_call(func) func
                                    #else
#define     vch_func_call(func)
                                    #endif


#include	"lc_sys.h"

#if     	defined (BX_SIMU)

#include	<alloc.h>
#include	<mem.h>

#include	"bsi_ifce.h"

#else

#include	"pi_sys.h"

#if 		defined (O_EKR)

#include	"du_sys.h"
#include	"hi_sys.h"
#include	"si_sys.h"
#include    "lp_sys.h"
#include    "tc_sys.h"

#endif
#endif

#define		BAI_HI_LED1_MSK             0x20
#define     BAI_HI_LED1_OFF             5


#define     BAI_DSW_MAS_MSK             LC_DSW_MAS_MSK
#define     BAI_DSW_ACT_MSK             LC_DSW_ACT_MSK
#define     BAI_DSW_IDX_MSK             LC_DSW_AX_MSK
#define     BAI_DSW_CLS_MSK             0xF000

#define     BAI_DSW_MAS_OFF             8
#define     BAI_DSW_ACT_OFF             9
#define     BAI_DSW_IDX_OFF             10
#define     BAI_DSW_CLS_OFF             12


#define     BAI_DSW_CLS_BA              4

#define     BAI_PCS_FCD_OFF

#define     BAI_MTT_ACC_MSK             0x8000
#define     BAI_MTT_ACC_OFF             15
#define     BAI_MTT_ACCEPTD             1


#define     bai_dsw_extract(dsw, msk, off) \
              (((dsw) & (msk)) >> (off))

#if defined (O_BAP)

#define     bai_ts_pcs_get(p_pcs, shrt_ix, mask, offset)          \
			((*(((short *) p_pcs) + shrt_ix) & (mask)) >> (offset))

#define     bai_ts_pcs_put(p_pcs, shrt_ix, mask, offset, value)  \
		    {   *(((short *) p_pcs) + shrt_ix) =             \
		      ( *(((short *) p_pcs) + shrt_ix) & ~(mask)) |  \
		       (((value) << (offset)) & (mask)) ;}

#define BAI_LC_INT_BMT  LC_INT_LP_RECV

#else

#define BAI_LC_INT_BMT  LC_INT_DTI6
#endif


#if     	defined (BX_SIMU)

#define		bai_disable()			/* empty function */
#define		bai_enable()			/* empty function */

#define     bai_si_disable_monitor()

#define		bai_sl_alloc			malloc

#define		bai_malloc				malloc
#define		bai_mfree				free

#define		bai_memcpy				memcpy

#define		bai_get_dvc_addr		bsi_get_dvc_addr

#define     bai_du_connect()        /* no du here */

#define		bai_sema_create(sz, pr, sema_id, p_rslt)  {*p_rslt = 0; \
                                                       *p_rslt = *p_rslt; \
                                                       *sema_id   = 5; }

#define		bai_sema_pend(sema_id, to, p_rslt) 	\
				{ *p_rslt = *p_rslt;  break; }

#define		bai_sema_post(sema_id, p_rslt) 	\
				{ *p_rslt = *p_rslt; }


#define		bai_q_create(sz, pr, q_id, p_rslt)  {*p_rslt = *p_rslt;}
#define		bai_q_pend(q_id, to, p_msg, p_rslt) 	\
                    { *p_rslt = *p_rslt; *p_msg = *p_msg; break; }

#if defined (O_BAP)
#define		bai_vec_bll_dma(p_func)	{;}
#define		bai_vec_bll_tmr1		bsh_cpu_tmr1_vec
#define     bai_mvbc_lc_int_connect(par1,par2,par3,par4) {;}
#define     bai_mvbc_lc_int_disconnect(par)              {;}
#define     bai_mvbc_lc_subscribe_emf(a,b,c)             {;}

#else
#define		bai_vec_bll_dma(p_func) {;}
#define		bai_vec_bll_tmr1(p_func){;}
#define     bai_mvbc_lc_int_connect          bai_lc_int_connect
#define     bai_mvbc_lc_int_disconnect       bai_lc_int_disconnect
#define     bai_mvbc_lc_subscribe_emf(bus_id,func,level) \
                bai_lc_int_connect(func, LC_INT_EMF, LC_NO_KERNEL, 0);
#endif

#define		bai_set_led(signal, operation)			/* empty function */
#define		bai_set_dsw(mask,   operation)	bsi_set_dsw(mask,operation);

#define     bai_lc_int_connect          bsh_lc_int_connect
#define     bai_lc_int_disconnect       bsh_lc_int_disconnect

#define     bai_si_set_signal(a,b)                       {;}


#else

#if defined (O_BAP)
#define     bai_mvbc_lc_int_connect(par1,par2,par3,par4) {;}
#define     bai_mvbc_lc_int_disconnect(par)              {;}
#define     bai_mvbc_lc_subscribe_emf(a,b,c)             {;}
#define		bai_vec_bll_tmr1(p_func)	                    \
			{   hi_set_vector (HI_EKR_TIMER1_VEC, p_func);  }

#define		bai_vec_bll_dma(p_func)							\
			{	hi_set_vector (HI_EKR_DMA0_VEC, p_func);    \
				hi_set_vector (HI_EKR_DMA1_VEC, p_func);	}

#else
#define		bai_vec_bll_dma(p_func) {;}
#define		bai_vec_bll_tmr1(p_func){;}
#define     bai_mvbc_lc_int_connect         bai_lc_int_connect
#define     bai_mvbc_lc_int_disconnect      bai_lc_int_disconnect
#define     bai_mvbc_lc_subscribe_emf       lc_subscribe_emf
#endif

#define		bai_disable					pi_disable
#define		bai_enable					pi_enable

#define		bai_sl_alloc				pi_alloc

#define		bai_malloc					pi_alloc
#define		bai_mfree					pi_free

#define     bai_si_disable_monitor()

#define		bai_hi_read_reg				hi_reg_read
#define		bai_hi_write_reg			hi_reg_write
#define     bai_lc_int_connect          lc_int_connect
#define     bai_lc_int_disconnect       lc_int_disconnect

#if defined (BX_DBUG)
#define     bai_du_connect()            du_connect();
#else
#define     bai_du_connect()
#endif

#define		bai_memcpy(p_d, p_s, sz)	pi_copy16(p_d, p_s, (sz) >> 1)

#define		bai_get_dvc_addr			lc_get_device_address

#define     bai_hi_led_bm_pas_int_ext_di()                             \
{                                                                      \
TYPE_UNIVERSAL_WORD temp;                                              \
	bai_hi_read_reg  (HI_EKR_ID_FCT_SUPER, &temp);                     \
	baa_bf_put       (&temp, 0, BAI_HI_LED1_MSK, BAI_HI_LED1_OFF);     \
	bai_hi_write_reg (HI_EKR_ID_FCT_SUPER, temp);                      \
}

#define     bai_hi_led_bm_act_int_ext_di()                             \
{                                                                      \
TYPE_UNIVERSAL_WORD temp;                                              \
	bai_hi_read_reg  (HI_EKR_ID_FCT_SUPER, &temp);                     \
	baa_bf_put       (&temp, 1, BAI_HI_LED1_MSK, BAI_HI_LED1_OFF);     \
	bai_hi_write_reg (HI_EKR_ID_FCT_SUPER, temp);                      \
}


#define		bai_sema_create(size, prio, sema_id, p_rslt)  	\
			{ *sema_id = pi_create_semaphore (size, prio, p_rslt); }

#define		bai_sema_pend(sema_id, timeout, p_rslt) 	\
				{ pi_pend_semaphore (sema_id, timeout, p_rslt); }

#define		bai_sema_post(sema_id, p_rslt) 	\
				{ pi_post_semaphore (sema_id, p_rslt); }

#define		bai_q_create(size, prio, q_id, p_rslt)  	\
                    { *q_id = pi_create_queue (size, prio, p_rslt); }

#define		bai_q_pend(q_id, timeout, p_msg, p_rslt) 	\
                    { *p_msg = pi_receive_queue (q_id, timeout, p_rslt); }

#define		bai_set_led(signal, operation)                  \
			{   si_set_signal_intr (signal, operation);          }

#define		bai_set_dsw(mask,   operation)					\
			{   lc_set_device_status_word(mask, operation); }
#endif

#define 	bai_memset(p_d, val, sz) 	 lc_memset16(p_d, val, (sz) >> 1)

#if defined (BX_EB)
#define     bai_si_trigger_watchdog        si_trigger_watchdog
#define     bai_hi_set_up_watchdog         hi_set_watchdog
#else
#define     bai_hi_set_up_watchdog(param); /* empty function */
#define     bai_si_trigger_watchdog()      /* empty function */
#endif

#define		bai_cpu_dma_di()	{	bah_cpu_dma0_di(); bah_cpu_dma1_di(); }
#define		bai_cpu_tmr_di()	{	bah_cpu_tmr1_di(); }


/*    Debug Utility   */

                                  #if defined (BX_SIMU) || defined (O_LC_TESTING)
/* Fake Hamster */
#include <stdio.h>

#define BAI_DU_POS                __FILE__, __LINE__
#define BAI_DU_ERR               "error"
#define BAI_DU_WARN              "warning"
#define BAI_DU_CONT              "cont."
#define BAI_DU_HALT              "halt"
#define BAI_DU_RESET             "reset"
#define BAI_DU_SUSP              "susp."

#define bai_hamster_cond(cond, type, action, expl)                 \
{ if (((cond) != 0)) \
{ printf ("EXCEPTION - File=%s, Line=%d, cond=%d, %s \n", __FILE__, __LINE__, cond, expl); } }
#define bai_hamster_uncond(    type, action, expl)                 \
{ printf ("EXCEPTION - File=%s, Line=%d, %s \n", __FILE__, __LINE__, expl); }

                                   #else

                                       #ifdef O_TCN

/* IEC TCN Event Recorder */

#include "recorder.h"

#define BAI_DU_POS                __FILE__,__LINE__
#define BAI_DU_ERR                NSE_EVENT_TYPE_ERROR
#define BAI_DU_WARN               NSE_EVENT_TYPE_WARNINGS
#define BAI_DU_CONT               NSE_ACTION_CONTINUE
#define BAI_DU_HALT               NSE_ACTION_HALT
#define BAI_DU_RESET              NSE_ACTION_REBOOT
#define BAI_DU_SUSP               NSE_ACTION_SUSPEND

#define bai_hamster_cond(cond, type, action, expl)                  \
        { if (cond) { nse_record (BAI_DU_POS, type, action, 0,0); } }
#define bai_hamster_uncond(    type, action, expl)                  \
        {           { nse_record (BAI_DU_POS, type, action, 0,0); } }

                                       #else
/* Real Hamster */
#include "du_sys.h"

#define BAI_DU_POS                DU_POSITION
#define BAI_DU_CONT               DU_CONTINUE
#define BAI_DU_HALT               DU_HALT
#define BAI_DU_RESET              DU_RESET
#define BAI_DU_SUSP               DU_SUSP
                                           #if defined (O_EKR)
#define bai_hamster_cond(cond, type, action, expl)                      \
        { if (cond) { hamster (BAI_DU_POS, action, 0); } }
#define bai_hamster_uncond(    type, action, expl)                      \
        {           { hamster (BAI_DU_POS, action, 0); } }
                                           #else
#define bai_hamster_cond(cond, type, action, expl)                      \
        { if (cond) { du_hamster_text (BAI_DU_POS, action, expl); } }
#define bai_hamster_uncond(    type, action, expl)                      \
        {           { du_hamster_text (BAI_DU_POS, action, expl); } }
                                           #endif
                                       #endif
                                   #endif

#endif
