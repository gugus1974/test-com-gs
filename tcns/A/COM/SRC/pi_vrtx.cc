/*=============================================================================================*/
/* Operating system routines                                                                   */
/* Implementation file (pi_vrtx.c)                                                             */
/*                                                                                             */
/* Version 1.1                                                                                 */
/*                                                                                             */
/* (c)1997 Ansaldo Trasporti                                                                   */
/* Written by Giuseppe Carnevale                                                               */
/*																				               */
/* Version history:																	           */
/* 1.0 (24/09/98) : added C++ support. If this file is comipled with C++, it enables the C++   */
/*                  support routines. All the interface is still in old-good-C-code.           */
/*                                                                                             */
/*=============================================================================================*/


/*=============================================================================================*/
/* Includes */

#ifdef __cplusplus
extern "C" {
#endif

#include "pi_vrtx.h"
#include "syskind.h"
#include "c_utils.h"

void _main();					/* C++ static objects creation routine */

#ifdef __cplusplus
}
#endif


/*=============================================================================================*/
/* Compilation switches */

/* if defined enables the memory debug */
// #define PIL_MEM_DEBUG


/*=============================================================================================*/
/* Typedefs */

typedef void (*TmoFuncPtr)(void *);	/* timeout function pointer */

typedef struct Tmo {			/**** Timeout descriptor ************************************/
	struct Tmo		*next;		/* next element in the queue                                */
	unsigned long	ord;		/* next time when it has to be executed                     */
    TmoFuncPtr  	func;		/* function to be called                                    */
    void        	*para;		/* parameter to be used                                     */
    short       	count;		/* number of times to call the timeout or PI_FOREVER        */
    short			pend_count;	/* number of timeouts pending; if 0 it is not activated     */
    unsigned long	ticks;		/* ticks between activation                                 */
} Tmo;


/*=============================================================================================*/
/* System globals and externals */

int			errno;					/* do not use: for compatibility only */
FILE		_iob[3];				/* do not use: for compatibility only */

extern char *sys_unused_low,		/* first unused byte in the heap */
			*sys_unused_high;		/* highest heap address          */


/*=============================================================================================*/
/* Globals */

int				pi_hid = 0;         	/* heap id used by this PIL                        */
unsigned long	pi_total_mem = 0;		/* number of bytes of the heap                     */
unsigned long	pi_allocated_mem = 0;	/* number of bytes allocated in the heap           */

int pi_err = 0;							/* storage for a not used error code               */

unsigned short sr_mask = 0xFFFF;    	/* mask used to save the SR value                  */
unsigned short sr_save = 0x0000;    	/* saved value for the SR register by pi_disable() */

static Tmo		*tmo;					/* ptr to the allocated timeout descriptors        */
static Tmo		tmo_head;				/* head of the timeouts queue                      */
static Tmo		tmo_free_head;			/* head of the free timeouts queue                 */
static short	tmo_sem;				/* input semaphore used by the timeout task        */


/*=============================================================================================*/
/* Glue code */

short pi_create_semaphore(short init, short opt, short *errp)
{
	int err;
	short t = sc_screate(init, opt, &err);
	if (errp) *errp = (short)err;
	return t;
}

short pi_inquiry_semaphore(short id, short *errp)
{
	int err;
	short t = sc_sinquiry(id, &err);
	if (errp) *errp = (short)err;
	return t;
}

void pi_pend_semaphore(short id, long timeout, short *errp)
{
	int err;
	sc_spend(id, timeout, &err);
	if (errp) *errp = (short)err;
}

void pi_post_semaphore(short id, short *errp)
{
	int err;
	sc_spost(id, &err);
	if (errp) *errp = (short)err;
}

short pi_create_queue(short size, short opt, short *errp)
{
	int err;
	short t = sc_qecreate(-1, size, opt, &err);
	if (errp) *errp = (short)err;
	return t;
}

char *pi_receive_queue(short id, long timeout, short *errp)
{
	int err;
	char *t = sc_qpend(id, timeout, &err);
	if (errp) *errp = (short)err;
	return t;
}

void pi_send_queue(short id, char *msgp, short *errp)
{
	int err;
	sc_qpost(id, msgp, &err);
	if (errp) *errp = (short)err;
}

char *pi_accept_queue(short id, short *errp)
{
	int err;
	char *t = sc_qaccept(id, &err);
	if (errp) *errp = (short)err;
	return t;
}


/*=============================================================================================*/
/* Memory management */

#define BLOCK_MARKER 0xF1F0	/* value used to mark a block */


/*---------------------------------------------------------------------------------------------*/
void *pi_alloc(unsigned short size)
{
	/* allocate the specified block plus the header */
	char *p = sc_halloc(pi_hid, (int)size + 4, &pi_err);

	if (!p) {
#ifdef PIL_MEM_DEBUG
		int info[3];
		sc_hinquiry(info, pi_hid, &pi_err);
		printf("*** Alloc fail *** size=0x%X, info=%d,%d,%d, hsize=0x%X, hused=0x%X\n",
				size, info[0], info[1],info[2], pi_total_mem, pi_allocated_mem);
#endif
		return 0;
	}

	/* fill-in the user block header */
	((unsigned short *)p)[0] = BLOCK_MARKER;
	((unsigned short *)p)[1] = size;

	/* keep track of the number of allocated bytes */
	sc_lock();
	pi_allocated_mem += (int)size + 4;
	sc_unlock();
	
#ifdef PIL_MEM_DEBUG
	printf(">A> %X @%lX\n", size, (long)p + 4);
#endif

	/* return the user block pointer */
	return p + 4;
}


/*---------------------------------------------------------------------------------------------*/
void pi_free(void *p)
{
	unsigned short size;	/* size of the block */

	if (!p) return;

#ifdef PIL_MEM_DEBUG
	printf("<F< %lX\n", (long)p);
#endif

	/* retrieve the system block pointer */
	p = (char *)p - 4;

	/* check the user block header */
	if (((unsigned short *)p)[0] != BLOCK_MARKER) pi_fail();
	((unsigned short *)p)[0] = 0;
	size = ((unsigned short *)p)[1];

	/* keep track of the number of allocated bytes */
	sc_lock();
	pi_allocated_mem -= (int)size + 4;
	sc_unlock();

	/* release the specified block */
	sc_hfree(pi_hid, (char*)p, &pi_err);
}


/*---------------------------------------------------------------------------------------------*/
unsigned long pi_mempool_use(char *mempool)
{
	return pi_allocated_mem;
}


/*=============================================================================================*/
/* Timeout handling */

/*---------------------------------------------------------------------------------------------*/
short pi_create_timeout(short *p_id, void (*p_calledfunc)(), void *p_para, short count)
{
	short	err = 0;	/* error code         */
	Tmo		*tmoP;		/* ptr to the timeout */

	/* check for parameters errors */
	if (!p_id || !p_calledfunc) Check(PI_ERR_PAR);

	/* get the first free descriptor (if any) */
	pi_lock_task();
	c_dequeue_first(Tmo, &tmo_free_head, tmoP);
	pi_unlock_task();
	if (!tmoP) Check(PI_ERR_MM);

	/* fill the descriptor */
    tmoP->ord        = 0;
    tmoP->func       = (TmoFuncPtr)p_calledfunc;
    tmoP->para       = p_para;
    tmoP->count      = count;
    tmoP->pend_count = 0;
    tmoP->ticks      = 0;

	/* save the return value */
	*p_id = tmoP - tmo;

error:
	return err;
}


/*---------------------------------------------------------------------------------------------*/
short pi_enable_timeout(short id, unsigned long ticks)
{
	short	err = 0;			/* error code */
	Tmo		*tmoP = tmo + id;	/* ptr to the timeout */

	pi_lock_task();

	/* check the validity of the timeout id, and that it is not running */
	if (!tmoP->func) Check(PI_ERR_ID);
	if (tmoP->pend_count) Check(PI_ERR_CT);
	
	/* update the timeout descriptor and queue it */
    if (tmoP->count != PI_FOREVER) tmoP->pend_count = tmoP->count;
	else tmoP->pend_count = 1;
	tmoP->ticks = ticks;
	tmoP->ord = pi_ticks_elapsed() + ticks;
	c_enqueue_ord(Tmo, &tmo_head, tmoP);

error:
	pi_unlock_task();
	pi_post_semaphore(tmo_sem, &err);
	return err;
}


/*---------------------------------------------------------------------------------------------*/
short pi_disable_timeout(short id)
{
	short	err = 0;			/* error code */
	Tmo		*tmoP = tmo + id;	/* ptr to the specified timeout descriptor */
	Tmo		*tmoP2;

	pi_lock_task();

	/* check the validity of the timeout id */
	if (!tmoP->func) Check(PI_ERR_ID);

	/* remove the timeout from the queue */
	tmoP->pend_count = 0;
	c_dequeue_elem(Tmo, &tmo_head, tmoP, tmoP2);
	if (!tmoP2) Check(PI_ERR_NQ);

error:
	pi_unlock_task();
	return err;
}


/*---------------------------------------------------------------------------------------------*/
short pi_delete_timeout(short id)
{
	short	err = 0;			/* error code */
	Tmo		*tmoP = tmo + id;	/* ptr to the specified timeout descriptor */

	pi_lock_task();

	/* check the validity of the timeout id and that it is not enabled */
	if (!tmoP->func) Check(PI_ERR_ID);
	if (tmoP->pend_count) Check(PI_ERR_RUN);

	/* free the descriptor */
	pi_zero8(tmoP, sizeof(Tmo));
	c_enqueue_lifo(Tmo, &tmo_free_head, tmoP);

error:
	pi_unlock_task();
	return err;
}


/*---------------------------------------------------------------------------------------------*/
static void timeout_task(void)
{
	unsigned long	sleep_value;	/* number of ticks to sleep */
	unsigned long	ticks;			/* current ticks value */
	Tmo				*tmoP;			/* ptr to the timeout descriptor */

	while (1) {

		ticks = pi_ticks_elapsed();

		/* execute the first ready timeout */
		if ((tmoP = tmo_head.next) != 0 && tmoP->ord <= ticks) {
			c_dequeue_first(Tmo, &tmo_head, tmoP);
			if (tmoP->count != PI_FOREVER) tmoP->pend_count--;
			if (tmoP->pend_count) {
				tmoP->ord = ticks + tmoP->ticks;
				c_enqueue_ord(Tmo, &tmo_head, tmoP);
			}
			(*tmoP->func)(tmoP->para);
		}

		/* calculate how long we need to sleep */
		if ((tmoP = tmo_head.next) != 0) {
			if (tmoP->ord > ticks) {
				sleep_value = tmoP->ord - ticks;
				if (sleep_value > PI_MAX_TIMEOUT) sleep_value = PI_MAX_TIMEOUT;
			}
			else sleep_value = 0;
		}
		else sleep_value = PI_MAX_TIMEOUT;

		/* sleep (if required) */
		if (sleep_value) pi_pend_semaphore(tmo_sem, sleep_value, 0);
	}
}


/*=============================================================================================*/
/* Initialization */

/*---------------------------------------------------------------------------------------------*/
short pi_init(int mempool_size, short tmo_num)
{
	int		err;
	void	*mempool;
	short	i;

	/* create the timeout descriptors, semaphore and task */
	tmo = (Tmo*)sys_mem_get(tmo_num * sizeof(Tmo));
	if (!tmo) Check(PI_ERR_MM);
	pi_zero8(tmo, tmo_num * sizeof(Tmo));
	for (i = 0; i < tmo_num; i++) c_enqueue_lifo(Tmo, &tmo_free_head, &tmo[i]);
	tmo_sem = sc_screate(0, 0, &err);
	Check(err);
	Check(pi_task_create(timeout_task, 0, TMO_TASK_PRIO));

	/* create the heap, reserving the specified memory if <= 0 */
	if (mempool_size <= 0) mempool_size += (int)(sys_unused_high - sys_unused_low);
	mempool = (void*)sys_mem_get(mempool_size);
	if (!mempool) Check(PI_ERR_MM);
	pi_hid = sc_hcreate((char*)mempool, mempool_size, 6, &err);
	Check(err);
	pi_total_mem = mempool_size;
	
#ifdef __cplusplus
	/* initialize C++ static objects */
	_main();
#endif

error:
	return err;
}


/*=============================================================================================*/
/* C++ support */

#ifdef __cplusplus

/*---------------------------------------------------------------------------------------------*/
void *operator new(size_t size)
{
	return pi_alloc(size);
}


/*---------------------------------------------------------------------------------------------*/
void operator delete(void* p)
{
	pi_free(p);
}


/*---------------------------------------------------------------------------------------------*/

#endif


/*=============================================================================================*/

