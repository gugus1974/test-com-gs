/*=====================================================================================*/
/* Definitions for compatibility with old TCN/JDP software                             */
/* Header file (old_sys.h)       													   */
/* 																					   */
/* Version 1.0																		   */
/*																					   */
/* (c)1998 Ansaldo Trasporti														   */
/* Written by Giuseppe Carnevale													   */
/*=====================================================================================*/

#ifndef _OLD_SYS_
#define _OLD_SYS_


/*=====================================================================================*/
/* AM compatibility definitions */

#include "am_sys.h"

#define xam_call_requ(caller_fct, replier, out_msg_addr, out_msg_size, in_msg_addr, 	\
					  in_msg_size, reply_tmo, confirm, ext_ref)	{						\
	AM_ADDRESS new_replier;																\
	new_replier.vehicle    = (replier)->vehicle;										\
	new_replier.fct_or_sta = (replier)->function;										\
	new_replier.next_sta   = (replier)->device;											\
	new_replier.tc         = AM_ANY_TC;													\
	_am_send_requ(caller_fct, &new_replier, out_msg_addr, out_msg_size, in_msg_addr,	\
				 in_msg_size, reply_tmo, (AM_CALL_CONF)(confirm), (AM_INVOKE_CONF) NULL,\
				 ext_ref);																\
}
#undef  am_call_requ
#define am_call_requ(caller_fct, replier, out_msg_addr, out_msg_size, in_msg_addr, 		\
					  in_msg_size, reply_tmo, confirm, ext_ref) 						\
	xam_call_requ(caller_fct, replier, out_msg_addr, out_msg_size, in_msg_addr, 		\
				  in_msg_size, reply_tmo, confirm, ext_ref)

#define xam_call_cancel(caller_fct, replier, status) {									\
	AM_ADDRESS new_replier;																\
	new_replier.vehicle    = (replier)->vehicle;										\
	new_replier.fct_or_sta = (replier)->function;										\
	new_replier.next_sta   = (replier)->device;											\
	new_replier.tc         = AM_ANY_TC;													\
	_am_call_cancel(caller_fct, &new_replier, status);									\
}
#undef  am_call_cancel
#define am_call_cancel(caller_fct, replier, status) 									\
	xam_call_cancel(caller_fct, replier, status)

#define xam_bind_replier(replier_fct, rcv_conf, reply_conf, status)						\
	_am_bind_replier(replier_fct, (AM_RCV_CONF)(rcv_conf), reply_conf, status)
#undef  am_bind_replier
#define am_bind_replier(replier_fct, rcv_conf, reply_conf, status)						\
	xam_bind_replier(replier_fct, rcv_conf, reply_conf, status)


/*=====================================================================================*/

#endif

