/********************************************************************/
/* 2004/nov/26,ven 12:25                \SWISV\TCNS\SRC\pi_rtx.c    */
/********************************************************************/
/* Operating system routines                                        */
/********************************************************************/
/*
    $Date: 2004/11/26 12:04:44 $          $Revision: 1.2 $
    $Author: mungiguerrav $
*/


/*
   PIL (Process Interface Library) for the RTX-166 kernel
   The differences from the specifications are marked with "### WARNING ###"
*/


/*==========================================================================*/
/* Pragmas */
#pragma HLARGE
#pragma WL(3)


/*==========================================================================*/
/* Compilation switches */

/* #define PI_DEBUG */
/* #define PI_ZERO_ALLOC */
/* #define PI_USE_DEBUG_PIN */


/*==========================================================================*/
/* Includes */
#include "pi_rtx.h"
#include "malloc.h"
#include "recorder.h"

#ifdef PI_DEBUG
    #include <stdio.h>                      /* standard I/O .h-file */
#endif


/*==========================================================================*/
/* Operating system interface */

#ifndef PI_USE_DEBUG_PIN
    /* disable and enable the scheduling */
    #define xs_disable() \
        if (1) {sPSW = PSW; _atomic_(3); _bfld_(PSW, 0xF000, MAX_SYS_ILVL << 12); _nop_(); _nop_();} else
    #define xs_enable() PSW = sPSW
#else
    #include "atr_hw.h"
    /* disable and enable the scheduling */
    #define xs_disable() \
        if (1) {sPSW = PSW; _atomic_(3); _bfld_(PSW, 0xF000, MAX_SYS_ILVL << 12); _nop_(); _nop_(); \
                debug_pin_on();} else
    #define xs_enable() if (1) {debug_pin_off(); PSW = sPSW;}
#endif

/* returns TRUE if we are in an ISR */
#define xs_in_isr() ((PSW & 0xF000) != 0)

/* process id type */
typedef t_rtx_handle xs_task_id;

/* process priority type */
typedef unsigned short xs_task_prio;

/* return codes */
typedef t_rtx_exit xs_exit_code;
#define XS_TMO TMO_EVENT
#define XS_SIG SIG_EVENT

/* gets the number of ticks (unsigned long) since system startup */
#define xs_ticks_elapsed(x) if (1) {xs_disable(); *x = tick_counter; xs_enable();} else

/* return the running task priority */
#define xs_get_running_task_prio() os_running_task_prio()

/* change the running task priority */
#define xs_set_running_task_prio(prio) os_change_prio(prio)

/* return the id of the running task */
#define xs_get_running_task_id() os_running_task_id()

/* wait for a timeout */
#define xs_wait(tmo) os_wait(K_TMO, tmo, 0, 0, 0, 0)

/* wait for a signal with a timeout; returns XS_TMO or XS_SIG */
#define xs_wait_signal(tmo) os_wait(tmo == PI_FOREVER ? K_SIG : K_SIG|K_TMO, tmo, 0, 0, 0, 0)

/* send a signal to a process */
#define xs_send_signal(pid) if (xs_in_isr()) isr_send_signal(pid); else os_send_signal(pid)

/* clear a signal sent to a process */
#define xs_clear_signal(pid) if (xs_in_isr()) isr_clear_signal(pid); else os_clear_signal(pid)

/* hangup the system due to a software error */
#define xs_fail() _trap_(2) /* nmi */


/*==========================================================================*/
/* Constants */

/* maximum task priority */
#define MAX_TASK_PRIO   127

/* maximum number of ticks we can sleep */
#define MAX_SLEEP       0x7000


/*==========================================================================*/
/* Globals */

short           pi_wspsize;         /* default workspace size for the tasks        */
t_rtx_handle    pi_task_handle;     /* temporary storage for the task handles      */

char            pi_int_disabled;    /* if TRUE the interrupts are currently disab. */
unsigned short  pi_PSW;             /* saved PSW for interrupt disabling           */


/*==========================================================================*/
/* Macros */

/* returns the index from a pointer inside an array */
#define ptr2index(ptr, base) (((char*)ptr - (char*)base) / sizeof(*base))


/*==========================================================================*/
/* Private typedefs */

typedef unsigned char   uchar;
typedef unsigned short  ushort;
typedef unsigned long   ulong;

typedef struct q_elem           /**** Queue element **********************************/
{
    struct q_elem   *next;      /* next element queued                               */
    unsigned long   prio;       /* element priority                                  */
} q_elem;

typedef struct                  /**** Process descriptor *****************************/
{
    q_elem      q;              /* waiting queue                                     */
    xs_task_id  proc_id;        /* id of this process                                */
} proc_desc;

typedef struct                  /**** Semaphore descriptor ***************************/
{
    short       value;          /* semaphore value                                   */
    short       opt;            /* semaphore options (strategy)                      */
    q_elem      q_head;         /* first process queued                              */
} sem_desc;

typedef struct                  /**** Message queue element **************************/
{
    q_elem      q;              /* next message queue                                */
    void        *p_msg;         /* pointer to the message                            */
} mq_elem;

typedef struct                  /**** Message queue descriptor ***********************/
{
    short       opt;            /* message queue options (strategy)                  */
    short       read_sem_id;    /* read wait semaphore id                            */
    q_elem      q_head;         /* head of the used elements queue                   */
    q_elem      q_free_head;    /* head of the free elements queue                   */
} mq_desc;

typedef struct                  /**** Timeout descriptor *****************************/
{
    q_elem          q;          /* next queued timeout                               */
    short           count;      /* number of scheduled timeout calls (0 if not used) */
    void            (*func)();  /* ptr to called function                            */
    void            *param;     /* ptr to parameters of the function                 */
    unsigned long   ticks;      /* number of ticks between timeouts                  */
    short           pend_count; /* number of timeouts pending                        */
} tmo_desc;


/*==========================================================================*/
/* Private variables */

static int           isr_can_alloc;         /* if TRUE the alloc routines can be called in ISR */

static char          *system_pool = 0;      /* pointer to the system memory pool               */
static unsigned long tick_counter = 0;      /* ticks elasped since startup                     */
static xs_task_id    tmo_task_id;           /* id of the timeout task                          */

static xs_task_prio  saved_task_prio = 0;   /* saved task priority                             */
static short         sched_disab_count = 0; /* number of times the scheduler has been disabled */

static q_elem        q_proc_free_head;      /* free process descriptors queue head             */
static proc_desc     *proc;                 /* process descriptors                             */
static short         all_proc_num;          /* number of allocated process descriptors         */

static short         sem_free_index = 0;    /* index of the first free semaphore               */
static sem_desc      *sem;                  /* semaphore descriptors                           */
static short         all_sem_num;           /* number of allocated semaphore descriptors       */

static short         mq_free_index = 0;     /* index of the first free queue descriptor        */
static mq_desc       *mq;                   /* message queue descriptors                       */
static short         all_mq_num;            /* number of allocated message queue descriptors   */

static q_elem        q_tmo_head = {0, 0};   /* pending timeouts queue head                     */
static q_elem        q_tmo_free_head;       /* free timeouts queue head                        */
static tmo_desc      *tmo;                  /* timeout descriptors                             */
static short         all_tmo_num;           /* number of allocated timeout descriptors         */


/*==========================================================================*/
/* Generic routines (private) */

/*--------------------------------------------------------------------------*/
/* void SetErr(short *errp, short err)

   Saves the specified error code in the specified error location.
   If PI_DEBUG is defined, signals the error situation to the operator

   errp -> pointer to the error code storage (or NULL if none)
   err  -> error code

   ### WARNING ### errp could be NULL
*/

#ifdef PI_DEBUG
    static short SetErr(short *errp, short err)
    {
        if (errp)
        {
            *errp = err;
        }

        if (err != PI_RET_OK && err != PI_ERR_TO && err != PI_ERR_NM)
        {
            printf("<<%d>>\n", err);
        }

        return err;
    }
#else
    #define SetErr(errp, err) (errp ? (*(short*)errp = err) : err)
#endif


/*==========================================================================*/
/* Queue handling routines (private) */

/*--------------------------------------------------------------------------*/
/* void enqueue(q_elem *head, short opt, q_elem *elem)

   Puts an element in the queue specified by head

   head -> ptr to the queue head
   opt  -> queue options
   elem -> ptr to the element to be queued; elem->next is ignored
*/
static void enqueue(q_elem *p, short opt, q_elem *elem)
{
    q_elem        *q;
    unsigned long prio = elem->prio;

    if      (opt == PI_ORD_PRIO)            /* enqueue on priority */
    {
        while ((q = p->next) != 0)
        {
            if (q->prio < prio)
            {
                break;
            }

            p = q;
        }
    }
    else if (opt == PI_ORD_INV_PRIO)        /* enqueue on inverted priority */
    {
        while ((q = p->next) != 0)
        {
            if (q->prio > prio)
            {
                break;
            }

            p = q;
        }
    }
    else if (opt == PI_ORD_FCFS)            /* FIFO enqueue */
    {
        while ((q = p->next) != 0)
        {
            p = q;
        }
    }
    else
    {
        q = p->next;                        /* LIFO enqueue is the default */
    }

    elem->next  = q;
    p->next     = elem;
}


/*--------------------------------------------------------------------------*/
/* q_elem *dequeue_first(q_elem *head)

   Removes the first element from the queue specified by head

   head             -> ptr to the queue head
   dequeue_first() <-  ptr to the element dequeued or NULL if none
*/
static q_elem *dequeue_first(q_elem *head)
{
    q_elem *p = head->next;

    if (p)
    {
        head->next = p->next;
    }

    return p;
}


/*--------------------------------------------------------------------------*/
/* q_elem *dequeue_elem(q_elem *head, q_elem *elem)

   Removes the specified element from the queue specified by head

   head            -> ptr to the queue head
   elem            -> ptr to the element to be dequeued
   dequeue_elem() <-  ptr to the element dequeued or NULL if none
*/
static q_elem *dequeue_elem(q_elem *p, q_elem *elem)
{
    q_elem *q;

    while ((q = p->next) != 0)
    {
        if (q == elem)
        {
            p->next = elem->next;
            return elem;
        }
        p = q;
    }

    return 0;
}


/*==========================================================================*/
/* Initialization routine */

/* ### WARNING ### The following routine is not standard */

/*--------------------------------------------------------------------------*/
short pi_init(  char            *mempool,
                unsigned short  mempool_size,
                short           proc_num,
                short           sem_num,
                short           mq_num,
                short           tmo_num,
                short           tmo_wspsize,
                unsigned short  cpu_clock,
                int             allow_isr_alloc)
{
    short i;                /* generic index */
    short err = PI_RET_OK;  /* error code */

    /* save the ISR allocation option */
    isr_can_alloc = allow_isr_alloc;

    /* set the default workspace size for the tasks */
    pi_wspsize = tmo_wspsize;

    /* initialize the memory allocator */
    init_mempool(mempool, mempool_size);
    system_pool = mempool;

    /* allocate the descriptors */
    proc = (proc_desc*)pi_alloc(sizeof(proc_desc) * proc_num);
    if (!proc)
    {
        Check(PI_ERR_MM);
    }
    pi_zero8((void*)proc, sizeof(proc_desc) * proc_num);

    sem = (sem_desc*)pi_alloc(sizeof(sem_desc) * sem_num);
    if (!sem)
    {
        Check(PI_ERR_MM);
    }
    pi_zero8((void*)sem, sizeof(sem_desc) * sem_num);

    mq = (mq_desc*)pi_alloc(sizeof(mq_desc) * mq_num);
    if (!mq)
    {
        Check(PI_ERR_MM);
    }
    pi_zero8((void*)mq, sizeof(mq_desc) * mq_num);

    tmo = (tmo_desc*)pi_alloc(sizeof(tmo_desc) * tmo_num);
    if (!tmo)
    {
        Check(PI_ERR_MM);
    }
    pi_zero8((void*)tmo, sizeof(tmo_desc) * tmo_num);

    /* remember the number of allocated descriptors */
    all_proc_num    = proc_num;
    all_sem_num     = sem_num;
    all_mq_num      = mq_num;
    all_tmo_num     = tmo_num;

    /* create the chain of free timeouts descriptors (assuming 0 initialization) */
    q_tmo_free_head.next = &tmo[0].q;
    for (i = 0; i < tmo_num - 1; i++)
    {
        tmo[i].q.next = (q_elem *)(tmo + i + 1);
    }

    /* create the chain of free process descriptors (assuming 0 initialization) */
    q_proc_free_head.next = &proc[0].q;
    for (i = 0; i < proc_num - 1; i++)
    {
        proc[i].q.next = (q_elem *)(proc + i + 1);
    }

    /* use the timer 0 to generate an interrupt each tick */
    T0REL  = -(short)(cpu_clock / 8 * PI_TICK);
    T0     = -(short)(cpu_clock / 8 * PI_TICK);
    T01CON |= 0x0040;
    T0IC   = (1 << 6) | (2 << 2) | 0;

    /* adjust the RTX-166 tick */
    Check(os_set_slice(PI_TICK));

    /* start the timeout handling task */
    Check(os_create_task(&tmo_task_id, TMO_TASK_ID, tmo_wspsize));

error:
    return SetErr(0, err);
}


/*--------------------------------------------------------------------------*/
void pil_timer_interrupt() interrupt 0x20 using level_2_bank
{
    tick_counter++;
}


/*==========================================================================*/
/* Scheduler routines */

/* ### WARNING ### The following routines cannot be called from an ISR */

/*--------------------------------------------------------------------------*/
void pi_lock_task(void)
{
    xs_task_prio task_prio;     /* current task priority */

    /* fail if we are called from an ISR */
    if (xs_in_isr())
    {
        xs_fail();
    }

    /* get the current task priority */
    task_prio = xs_get_running_task_prio();

    /* if we do not have already raised our priority, do it now */
    if (task_prio != MAX_TASK_PRIO)
    {
        xs_set_running_task_prio(MAX_TASK_PRIO);
        saved_task_prio = task_prio;
    }

    /* increment the sheduler disable counter */
    sched_disab_count++;
}


/*--------------------------------------------------------------------------*/
void pi_unlock_task(void)
{
    /* fail if we are called from an ISR */
    if (xs_in_isr())
    {
        xs_fail();
    }

    /* if there are the conditions, restore the original priority */
    if (sched_disab_count)
    {
        if (!--sched_disab_count)
        {
            xs_set_running_task_prio(saved_task_prio);
        }
    }
}


/*==========================================================================*/
/* Memory allocation routines */

/*--------------------------------------------------------------------------*/
void pi_mempool_init(char *p, unsigned short size)
{
    init_mempool(p, size);
}


/*--------------------------------------------------------------------------*/
char *pi_malloc(char *mempool, unsigned short size)
{
    unsigned short  sPSW = 0;   /* saved PSW */
    char huge       *p;

    /* lock the accesses from others */
    if (isr_can_alloc)
    {
        xs_disable();
    }
    else
    {
        pi_lock_task();
    }

    /* execute the allocation */
    p = malloc(mempool ? mempool : system_pool, size);

    /* unlock the accesses from others */
    if (isr_can_alloc)
    {
        xs_enable();
    }
    else
    {
        pi_unlock_task();
    }

#ifdef PI_ZERO_ALLOC
    /* clear the allocated block */
    if (p)
    {
        pi_zero8(p, size);
    }
    else
    {
        SetErr(0, PI_ERR_MM);
    }
#endif

    return p;
}


/*--------------------------------------------------------------------------*/
void pi_free(char *p)
{
    unsigned short  sPSW = 0;   /* saved PSW */

    /* lock the accesses from others */
    if (isr_can_alloc)
    {
        xs_disable();
    }
    else
    {
        pi_lock_task();
    }

    /* release the allocated block */
    free(p);

    /* unlock the accesses from others */
    if (isr_can_alloc)
    {
        xs_enable();
    }
    else
    {
        pi_unlock_task();
    }
}


/*--------------------------------------------------------------------------*/
unsigned long pi_mempool_use(char *mempool)
{
    unsigned short  sPSW = 0;   /* saved PSW */
    unsigned short  s;

    /* lock the accesses from others */
    if (isr_can_alloc)
    {
        xs_disable();
    }
    else
    {
        pi_lock_task();
    }

    /* calculate the mempool usage */
    s = mempool_use(mempool ? mempool : system_pool);

    /* unlock the accesses from others */
    if (isr_can_alloc)
    {
        xs_enable();
    }
    else
    {
        pi_unlock_task();
    }

    return s;
}


/*==========================================================================*/
/* Semaphore routines */

/*--------------------------------------------------------------------------*/
/* ### WARNING ### The id returned in case of error is -1, instead of 0 */

short pi_create_semaphore(short init, short opt, short *errp)
{
    unsigned short  sPSW;   /* saved PSW */
    short           id;     /* allocated semaphore id */
    sem_desc        *semP;  /* pointer to the allocated semaphore descriptor */

    /* check for parameters errors */
    if (init < 0 || (opt != PI_ORD_PRIO && opt != PI_ORD_FCFS))
    {
        SetErr(errp, PI_ERR_PAR);
        return -1;
    }

    xs_disable();

    /* check for overflow in the number of semaphores allocated */
    if (sem_free_index == all_sem_num)
    {
        xs_enable();
        SetErr(errp, PI_ERR_NC);
        return -1;
    }

    /* create the semaphore */
    id = sem_free_index++;
    semP = sem + id;
    semP->value = init;
    semP->opt = opt;
    semP->q_head.next = 0;
    semP->q_head.prio = 0;

    /* succesful exit */
    xs_enable();
    SetErr(errp, PI_RET_OK);
    return id;
}


/*--------------------------------------------------------------------------*/
short pi_inquiry_semaphore(short id, short *errp)
{
    /* check the validity of the semaphore id */
    if (id < 0 || id >= sem_free_index)
    {
        SetErr(errp, PI_ERR_ID);
        return -1;
    }

    SetErr(errp, PI_RET_OK);
    return sem[id].value;
}


/*--------------------------------------------------------------------------*/
/* ### WARNING ### Can be called from ISR if timeout == 0 */

void pi_pend_semaphore(short id, long timeout, short *errp)
{
    unsigned short  sPSW;               /* saved PSW */
    sem_desc        *semP = sem + id;   /* ptr to the semaphore descriptor */
    xs_task_id      proc_id = 0;        /* current process id */
    xs_task_prio    proc_prio = 0;      /* current process priority */
    proc_desc       *procP;             /* ptr to the current process desc */
    xs_exit_code    exit_code;          /* sys call return code */
    char            post_made;          /* TRUE if there was a post on the sem */

    /* check for parameters errors */
    if (timeout < 0 || timeout > PI_FOREVER)
    {
        SetErr(errp, PI_ERR_PAR);
        return;
    }

    /* check the validity of the semaphore id */
    if (id < 0 || id >= sem_free_index)
    {
        SetErr(errp, PI_ERR_ID);
        return;
    }

    /* save the process info before disabling the OS */
    if (!xs_in_isr())
    {
        proc_id     = xs_get_running_task_id();
        proc_prio   = xs_get_running_task_prio();
        /*!!*/ xs_clear_signal(proc_id);
    }

    xs_disable();

    if (semP->value <= 0)
    {   /* we have to wait on the semaphore */

        /* no wait if no timeout (ISR compatibility) */
        if (timeout == 0)
        {
            xs_enable();
            SetErr(errp, PI_ERR_TO);
            return;
        }

        /* decrement the semaphore value */
        semP->value--;

        /* allocate the process descriptor */
        procP = (proc_desc *)dequeue_first(&q_proc_free_head);
        if (!procP)
        {
            xs_enable();
            SetErr(errp, PI_ERR_NC);
            return;
        }

        /* put this process on the waiting queue */
        procP->q.prio   = proc_prio;
        procP->proc_id  = proc_id;
        enqueue(&semP->q_head, semP->opt, &procP->q);

        /* wait */
        xs_enable();
        exit_code = xs_wait_signal(timeout);
        xs_disable();

        /* check to see if the other processes made a post call */
        post_made = (exit_code == XS_SIG || dequeue_elem(&semP->q_head, &procP->q) == 0);

        /* free the process descriptor */
        enqueue(&q_proc_free_head, PI_ORD_LCFS, &procP->q);

        if (post_made && exit_code == XS_TMO)
        {
            /* we have got a signal between the wait and the scheduler disable */
            xs_enable();
            xs_clear_signal(proc_id);
            SetErr(errp, PI_RET_OK);
            return;
        }

        if (!post_made)
        {
            /* increment the semaphore value and signal the timeout */
            semP->value++;
            xs_enable();
            SetErr(errp, PI_ERR_TO);
            return;
        }

    }
    else
    {
        semP->value--;     /* the semaphore is free, go on */
    }

    /* succesful exit */
    xs_enable();
    SetErr(errp, PI_RET_OK);
}


/*--------------------------------------------------------------------------*/
void pi_post_semaphore(short id, short *errp)
{
    unsigned short  sPSW;               /* saved PSW */
    sem_desc        *semP = sem + id;   /* ptr to the semaphore descriptor */
    xs_task_id      proc_id = 0;        /* id of a process waiting on a sem */

    /* check the validity of the semaphore id */
    if (id < 0 || id >= sem_free_index)
    {
        SetErr(errp, PI_ERR_ID);
        return;
    }

    xs_disable();

    /* check for semaphore overflows */
    if (semP->value == 32767)
    {
        xs_enable();
        SetErr(errp, PI_ERR_OV);
        return;
    }

    if (semP->value++ < 0)
    {/* there is someone waiting on the semaphore */
        proc_id = ((proc_desc*)dequeue_first(&semP->q_head))->proc_id;
        xs_enable();
        xs_send_signal(proc_id);
        SetErr(errp, PI_RET_OK);
        return;
    }

    /* succesful exit with no wake-up */
    xs_enable();
    SetErr(errp, PI_RET_OK);
}


/*==========================================================================*/
/* Message queue routines */

/*--------------------------------------------------------------------------*/
/* ### WARNING ### The id returned in case of error is -1, instead of 0 */

short pi_create_queue(short size, short opt, short *errp)
{
    unsigned short  sPSW;               /* saved PSW */
    short           id;                 /* allocated message queue id */
    mq_desc         *mqP;               /* ptr to the allocated message queue descriptor */
    mq_elem         *mqeP;              /* ptr to an element in the message queue */
    short           sem_id = -1;        /* semaphore id */
    short           err = PI_RET_OK;    /* local error code */
    short           i;                  /* index variable */

    xs_disable();

    /* check for overflow in the number of message queues allocated */
    if (mq_free_index == all_mq_num)
    {
        xs_enable();
        SetErr(errp, PI_ERR_MM);
        return -1;
    }

    /* allocate the queue element memory storage and create the semaphore */
    mqeP = (mq_elem *)malloc(system_pool, size * sizeof(mq_elem));
    if (mqeP) sem_id = pi_create_semaphore(0, PI_ORD_PRIO, &err);
    if (!mqeP || err != PI_RET_OK)
    {
        if (mqeP) free((char *)mqeP);
        xs_enable();
        SetErr(errp, PI_ERR_MM);
        return -1;
    }

    /* update the message queue fields */
    id = mq_free_index++;
    mqP = mq + id;
    mqP->opt = opt;
    mqP->read_sem_id = sem_id;
    mqP->q_head.next = 0;
    mqP->q_head.prio = 0;
    mqP->q_free_head.next = (q_elem*)mqeP;
    mqP->q_free_head.prio = 0;

    /* create the chain of free message queue elements */
    for (i = 0; i < size - 1; i++, mqeP++)
    {
        mqeP->q.next = (q_elem*)(mqeP + 1);
    }

    mqeP->q.next = 0;

    /* succesful exit */
    xs_enable();
    SetErr(errp, PI_RET_OK);
    return id;
}


/*--------------------------------------------------------------------------*/
/* ### WARNING ### may return PI_ERR_PAR */

char *pi_receive_queue(short id, long timeout, short *errp)
{
    unsigned short  sPSW;               /* saved PSW */
    mq_desc         *mqP = mq + id;     /* ptr to the message queue descriptor */
    mq_elem         *mqeP;              /* ptr to the received message queue element */
    char            *p_msg;             /* return value */
    short           err = PI_RET_OK;    /* local error code */

    /* check for parameters errors */
    if (timeout < 0 || timeout > PI_FOREVER)
    {
        SetErr(errp, PI_ERR_PAR);
        return 0;
    }

    /* check the validity of the message queue id */
    if (id < 0 || id >= mq_free_index)
    {
        SetErr(errp, PI_ERR_ID);
        return 0;
    }

    /* wait for a message */
    pi_pend_semaphore(mqP->read_sem_id, timeout, &err);
    if (err != PI_RET_OK)
    {
        SetErr(errp, err);
        return 0;
    }

    /* get the message */
    xs_disable();
    mqeP = (mq_elem*)dequeue_first(&mqP->q_head);
    p_msg = mqeP->p_msg;
    enqueue(&mqP->q_free_head, PI_ORD_LCFS, &mqeP->q);

    /* succesful exit */
    xs_enable();
    SetErr(errp, PI_RET_OK);
    return p_msg;
}


/*--------------------------------------------------------------------------*/
void pi_send_queue(short id, char *msgp, short *errp)
{
    unsigned short  sPSW;           /* saved PSW */
    mq_desc         *mqP = mq + id; /* ptr to the message queue descriptor */
    mq_elem         *mqeP;          /* ptr to the allocated message queue element */
    xs_task_prio    task_prio = -1; /* task priority (for ISRs it is the max) */

    /* check the validity of the message queue id */
    if (id < 0 || id >= mq_free_index)
    {
        SetErr(errp, PI_ERR_ID);
        return;
    }

    /* save the process info before disabling the OS */
    if (!xs_in_isr())
    {
        task_prio = xs_get_running_task_prio();
    }

    xs_disable();

    /* allocate the element */
    mqeP = (mq_elem*)dequeue_first(&mqP->q_free_head);
    if (!mqeP)
    {
        xs_enable();
        SetErr(errp, PI_ERR_FB);
        return;
    }

    /* enqueue the element */
    mqeP->q.prio = task_prio;
    mqeP->p_msg = msgp;
    enqueue(&mqP->q_head, mqP->opt, &mqeP->q);

    xs_enable();
    pi_post_semaphore(mqP->read_sem_id, 0);

    /* succesful exit */
    SetErr(errp, PI_RET_OK);
}


/*--------------------------------------------------------------------------*/
char *pi_accept_queue(short id, short *errp)
{
    short   err;    /* local error code */
    char    *p_msg; /* return value */

    p_msg = pi_receive_queue(id, 0, &err);
    if (err == PI_ERR_TO)
    {
        SetErr(errp, PI_ERR_NM);
    }
    else
    {
        SetErr(errp, err);
    }

    return p_msg;
}


/*==========================================================================*/
/* Timeout routines */

/*--------------------------------------------------------------------------*/
short pi_create_timeout(short *p_id, void (*p_calledfunc)(), void *p_para, short count)
{
    unsigned short  sPSW;   /* saved PSW */
    tmo_desc        *tmoP;  /* ptr to the allocated timeout descriptor */

    /* check for parameters errors */
    if (!p_id || !p_calledfunc || !count)
    {
        return SetErr(0, PI_ERR_PAR);
    }

    xs_disable();

    /* allocate the timeout descriptor */
    tmoP = (tmo_desc *)dequeue_first(&q_tmo_free_head);
    if (!tmoP)
    {
        xs_enable();
        return SetErr(0, PI_ERR_MM);
    }

    /* update the timeout fields */
    tmoP->count         = count;
    tmoP->func          = p_calledfunc;
    tmoP->param         = p_para;
    tmoP->ticks         = 0;
    tmoP->pend_count    = 0;

    /* succesful exit */
    xs_enable();
    *p_id = ptr2index(tmoP, tmo);
    return SetErr(0, PI_RET_OK);
}


/*--------------------------------------------------------------------------*/
short pi_enable_timeout(short id, unsigned long ticks)
{
    unsigned short  sPSW;               /* saved PSW */
    tmo_desc        *tmoP = tmo + id;   /* ptr to the specified timeout descriptor */
    unsigned long   cur_ticks;          /* current tick time */

    xs_ticks_elapsed(&cur_ticks);

    xs_disable();

    /* check the validity of the timeout id */
    if (!tmoP->count)
    {
        xs_enable();
        return SetErr(0, PI_ERR_ID);
    }

    /* check that the timeout is not running */
    if (tmoP->pend_count)
    {
        xs_enable();
        return SetErr(0, PI_ERR_CT);
    }

    /* enqueue the timeout */
    tmoP->ticks         = ticks;
    tmoP->pend_count    = tmoP->count;
    tmoP->q.prio        = cur_ticks + ticks;
    enqueue(&q_tmo_head, PI_ORD_INV_PRIO, &tmoP->q);

    /* succesful exit */
    xs_enable();
    xs_send_signal(tmo_task_id);
    return SetErr(0, PI_RET_OK);
}


/*--------------------------------------------------------------------------*/
short pi_disable_timeout(short id)
{
    unsigned short  sPSW;               /* saved PSW */
    tmo_desc        *tmoP = tmo + id;   /* ptr to the specified timeout descriptor */

    xs_disable();

    /* check the validity of the timeout id */
    if (!tmoP->count)
    {
        xs_enable();
        return SetErr(0, PI_ERR_ID);
    }

    /* remove the timeout from the queue */
    tmoP->pend_count = 0;
    if (!dequeue_elem(&q_tmo_head, &tmoP->q))
    {
        /* the requested timeout is not queued */
        xs_enable();
        xs_send_signal(tmo_task_id);
        return SetErr(0, PI_ERR_NQ);
    }

    /* succesful exit */
    xs_enable();
    xs_send_signal(tmo_task_id);
    return SetErr(0, PI_RET_OK);
}


/*--------------------------------------------------------------------------*/
short pi_delete_timeout(short id)
{
    unsigned short  sPSW;               /* saved PSW */
    tmo_desc        *tmoP = tmo + id;   /* ptr to the specified timeout descriptor */

    xs_disable();

    /* check the validity of the timeout id */
    if (!tmoP->count)
    {
        xs_enable();
        return SetErr(0, PI_ERR_ID);
    }

    /* check that the timeout is not enabled */
    if (tmoP->pend_count)
    {
        xs_enable();
        return SetErr(0, PI_ERR_RUN);
    }

    /* free the descriptor */
    tmoP->count = 0;
    enqueue(&q_tmo_free_head, PI_ORD_LCFS, &tmoP->q);

    /* succesful exit */
    xs_enable();
    return SetErr(0, PI_RET_OK);
}


/*--------------------------------------------------------------------------*/
void timeout_process(void) _task_ TMO_TASK_ID _priority_ TMO_TASK_PRIO
{
    unsigned short  sPSW;           /* saved PSW */
    unsigned long   sleep_value;    /* number of ticks to sleep */
    unsigned long   ticks;          /* current ticks value */
    tmo_desc        *tmoP;          /* ptr to the timeout descriptor */

    while (1) {

        xs_ticks_elapsed(&ticks);

        xs_disable();

        /* execute the ready timeout */
        if ((tmoP = (tmo_desc *)q_tmo_head.next) != 0)
        {
            if (tmoP->q.prio <= ticks)
            {
                dequeue_elem(&q_tmo_head, &tmoP->q);
                if (tmoP->pend_count != PI_FOREVER)
                {
                    tmoP->pend_count--;
                }

                if (tmoP->pend_count)
                {
                    tmoP->q.prio = ticks + tmoP->ticks;
                    enqueue(&q_tmo_head, PI_ORD_INV_PRIO, &tmoP->q);
                }
                xs_enable();
                (*tmoP->func)(tmoP->param);
                xs_disable();
            }
        }

        /* calculate how long we need to sleep */
        if ((tmoP = (tmo_desc *)q_tmo_head.next) != 0)
        {
            if (tmoP->q.prio > ticks)
            {
                sleep_value = tmoP->q.prio - ticks;
                if (sleep_value > MAX_SLEEP)
                {
                    sleep_value = MAX_SLEEP;
                }
            }
            else
            {
                sleep_value = 0;
            }
        }
        else
        {
            sleep_value = MAX_SLEEP;
        }

        xs_enable();

        /* sleep (if required) */
        if (sleep_value)
        {
            xs_wait_signal(sleep_value);
        }
    }
}


/*==========================================================================*/
/* Extras */

/*--------------------------------------------------------------------------*/
void pi_wait(unsigned long ticks)
{
    xs_wait(ticks);
}


/*--------------------------------------------------------------------------*/
unsigned long pi_ticks_elapsed(void)
{
    unsigned short  sPSW;           /* saved PSW */
    unsigned long   ticks;          /* current ticks */

    xs_ticks_elapsed(&ticks);
    return ticks;
}


/*==========================================================================*/
/* RTX-166 user error handler */

/*--------------------------------------------------------------------------*/
/* Report an RTX-166 error */

void rtx_user_error_handler(t_rtx_exit err_code, t_sys_call_id system_call)
{
    char fatal;                     /* TRUE if this is a fatal error */
    struct                          /* error descriptor to be saved */
    {
        t_rtx_exit    err_code;
        t_sys_call_id system_call;
    } err_desc;

    /* fill the error descriptor */
    err_desc.err_code       = err_code;
    err_desc.system_call    = system_call;

    /* determine if the error was fatal or not */
    switch (err_code)
    {
        case PARAM_ERROR:
        case BADHANDLE_ERROR:
        case NOHANDLE_ERROR:
        case RAMALLOC_ERROR:
        case INTERVAL_ERROR:
        case NOTDONE_ERROR:
            fatal = FALSE;
        break;

        default:
            fatal = TRUE;
    }

    /* save the error */
    nse_record(__FILE__, __LINE__, NSE_EVENT_TYPE_ERROR,
               fatal ? NSE_ACTION_REBOOT : NSE_ACTION_CONTINUE,
               sizeof(err_desc), (void*)&err_desc);

    /* set the error for the task */
    rtx_default_error_handler(err_code, system_call);
}



/* ****************************************************************************
    $Log: pi_rtx.c,v $
    Revision 1.2  2004/11/26 12:04:44  mungiguerrav
    2004/nov/26,ven 12:25           Napoli      Mungi
    - Si aggiungono keyword CVS e storia invertita
    - Si sostituiscono TAB con 4 spazi
    - Si applicano le regole sulle parentesi


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision            1.1         2004/11/26 09:45:54     mungiguerrav
    2004/nov/26,ven 10:48           Napoli      Mungi
    - Si aggiungono immodificati dalle directory inferiori TCNS\A\XXX\SRC,
      prima di iniziare il processo di ripulitura e adeguamento alle regole
      di codifica e alle regole di gestione CVS

 --------------  Spostamento da A\XXX\SRC a SRC  ---------------------

    Revision            1.2         2004/09/02 11:31:18     mungiguerrav
    2004/set/02,gio 13:25   Napoli          Mungi\Busco
    - Merge da branch di sviluppo  makefile
    - E' in effetti la creazione dei file nelle nuove directory dedicate alle
      singole schede registrate (DEA,DEV,STA,STB,STR)
    - Non sono ancora inseriti i token di gestioni automatiche CVS (DATE, FILE, AUTHOR, LOG)

    Revision [makefile] 1.1.2.1     2004/07/09 16:24:25     buscob
    - Inserimento in CVS in branch makefile
    - Addio monolite !!
02.40   1999/lug/07,                Napoli              Carnevale
        - bug fixes and no alloc in ISRs option

 ------------------------   Prima di CVS    ------------------------
02.3y   1998/mmm/gg                 Napoli              Carnevale
        - Modifiche non documentate
02.3x   1997/ago/27,mer             Napoli              Carnevale
        - Modifiche non documentate
02.30   1997/giu/10,                Napoli              Carnevale
        - RTX user error handler moved here
02.2x   199X/mmm/gg                 Napoli              Carnevale
        - Modifiche non documentate
02.20   199X/mmm/gg                 Napoli              Carnevale
        - Added the error PI_ERR_NQ in pi_disable_timeout()
02.1x   199X/mmm/gg                 Napoli              Carnevale
        - Modifiche non documentate
02.10   199X/mmm/gg                 Napoli              Carnevale
        - Minor optimizations
02.0x   199X/mmm/gg                 Napoli              Carnevale
        - Modifiche non documentate
02.00   1996/mmm/gg                 Napoli              Carnevale
        - Creazione adattata, ma come ?
01.00   aaaa/mmm/gg                 ??????              ????????
        - Creazione
*/

