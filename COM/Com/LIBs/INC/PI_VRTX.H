/*=============================================================================================*/
/* Operating system routines														           */
/* Header file (pi_vrtx.h)															           */
/* 																					           */
/* Version 1.0																		           */
/*																					           */
/* (c)1997 Ansaldo Trasporti														           */
/* Written by Giuseppe Carnevale													           */
/*=============================================================================================*/


#ifndef _PI_VRTX_
#define _PI_VRTX_


/*=============================================================================================*/
/* OS header file inclusion */

#include <vrtxil.h>
#include <mriext.h>

#include <string.h>


/*=============================================================================================*/
/* Constants */

#define PI_ORD_PRIO 	VRTX_PEND_ORDER_PRIORITY
#define PI_ORD_FCFS 	VRTX_PEND_ORDER_FIFO

#define PI_FOREVER		0
#define PI_NO_WAIT		-1

#define PI_RET_OK		RET_OK
#define PI_ERR_NC		ER_NOCB
#define PI_ERR_PAR		ER_IIP
#define PI_ERR_ID		ER_ID
#define PI_ERR_TO		ER_TMO
#define PI_ERR_OV		ER_OVF
#define PI_ERR_NM		ER_NMP
#define PI_ERR_MM		ER_MEM
#define PI_ERR_FB		ER_QFL
#define PI_ERR_CT		0x1000
#define PI_ERR_RUN		0x1001
#define PI_ERR_NQ		0x1002

#define PI_TICK 		10		/* 10 msec tick */

#define PI_MAX_TIMEOUT	0x7FFF	/* maximum allowed value for a timeout */

#define TMO_TASK_PRIO	100		/* timeout tasks priority */


/*=============================================================================================*/
/* Generic purpose macros */

#define ms2tick(x) (x / PI_TICK)
#define tick2ms(x) (x * PI_TICK)

#define get_sys_time() (pi_ticks_elapsed() / (PI_TICK * 1000))

#define Check(x) if ((err = x) != 0) goto error; else


/*=============================================================================================*/
/* Globals */

extern int pi_err;				/* storage for a not used error code */

extern unsigned short sr_mask;	/* mask used to save the SR value */
extern unsigned short sr_save;	/* saved value for the SR register by pi_disable() */


/*=============================================================================================*/
/* Implementation */

/*---------------------------------------------------------------------------------------------*/
/* Standard PIL functions */

#define pi_copy8(target, source, size) memcpy((void*)(target), (void*)(source), size)
#define pi_copy16(target, source, size)				\
{													\
	short			*t = (short*)(target);			\
	const short		*s = (const short*)(source);	\
	unsigned short	i = size;						\
	for (; i > 0; i--) *(t++) = *(s++);				\
}

#define pi_disable() 					\
	asm(void,							\
		"	move.w SR,D0			",	\
 		"	ori.w  #$0700,SR		",	\
 		"	and.w  `sr_mask`,D0		",	\
 		"	clr.w  `sr_mask`		",	\
 		"	or.w   D0,`sr_save`		"	\
  	)
#define pi_enable()						\
	asm(void,							\
		"	move.w `sr_save`,D0		",	\
		"	move.w #$FFFF,`sr_mask`	",	\
		"	clr.w  `sr_save`		",	\
		"	move.w D0,SR			"	\
  	)

#define pi_call_hw_int(nr) ???

void *pi_alloc(unsigned short size);	/* warning: returns char* in its original definition */
void pi_free(void *p);					/* warning: accepts char* in its original definition */

short pi_create_semaphore(short init, short opt, short *errp);
short pi_inquiry_semaphore(short id, short *errp);
void pi_pend_semaphore(short id, long timeout, short *errp);
void pi_post_semaphore(short id, short *errp);

short pi_create_queue(short size, short opt, short *errp);
char *pi_receive_queue(short id, long timeout, short *errp);
void pi_send_queue(short id, char *msgp, short *errp);
char *pi_accept_queue(short id, short *errp);

#define pi_lock_task() sc_lock()
#define pi_unlock_task() sc_unlock()

short pi_create_timeout(short *p_id, void (*p_calledfunc)(), void *p_para, short count);
short pi_enable_timeout(short id, unsigned long ticks);
short pi_disable_timeout(short id);
short pi_delete_timeout(short id);


/*---------------------------------------------------------------------------------------------*/
/* Extra functions */

#define pi_in_isr() asm(short, "	move.w	SR,D0", "	and.w	#$0700,D0", "	lsr.w	#8,D0")

#define pi_fail() if (1) asm(void, "	illegal"); else

short pi_init(int mempool_size, short tmo_num);

unsigned long pi_mempool_use(char *mempool);

#define pi_zero8(target, size) memclr((char*)(target), size)
#define pi_zero16(target, size)						\
do {												\
	short			*t = (short*)(target);			\
	unsigned short	i = size;						\
	for (; i > 0; i--) *(t++) = 0;					\
} while (0)

#define pi_wait(ticks) sc_delay(ticks)
#define pi_ticks_elapsed() sc_gtime()

#define pi_task_create(name, id, prio) \
	(sc_tecreate((void (*)(void*))name, id, 255-(prio), TCMSUPER, 0, 0, 0, 0, &pi_err) != -1 ? 0 : -1)
#define pi_task_exit() sc_tdelete(0, 0, &pi_err)
#define PI_TASK(name, id, prio) 						\
void name##_main(void);									\
void name(void)			 								\
{														\
	/* xtaskws->tws_cust = 0; */						\
	name##_main();										\
} 														\
void name##_main(void)
#define PI_TASK_DEF(name) void name(void)


/*=============================================================================================*/

#endif

