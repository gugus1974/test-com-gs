/********************************************************************/
/* 2004/nov/26,ven 13:00                \SWISV\TCNS\SRC\pi_rtx.h    */
/********************************************************************/
/* Operating system routines                                        */
/********************************************************************/
/*
    $Date: 2005/03/01 15:00:54 $          $Revision: 1.2.4.2 $
    $Author: schiavor $
*/


#ifndef _PI_RTX_
#define _PI_RTX_

/*==========================================================================*/
/* OS header file inclusion */

#undef FALSE
#undef TRUE
#undef BOOLEAN

#define send_msg xx_send_msg
#define TRUE _TRUE
#define FALSE _FALSE

#include <rtx166.h>                   /* RTX-166 tiny functions & defines     */

#undef send_msg
#undef TRUE
#undef FALSE


/*==========================================================================*/
/* Includes */
/*
#ifndef _REG167_H_
#define _REG167_H_
#include "reg167.h"
#endif
*/
#include "cpudrv.h"
#include <intrins.h>                  /* intrinsic functions                  */


/*==========================================================================*/
/* Constants */

#define PI_ORD_PRIO         0
#define PI_ORD_FCFS         1
#define PI_ORD_LCFS         2           /* ### WARNING ### Not available in PIL */
#define PI_ORD_INV_PRIO     3           /* ### WARNING ### Not available in PIL */

#define PI_FOREVER NILCARD
#define PI_NO_WAIT          0

#define PI_RET_OK           0
#define PI_ERR_NC           1
#define PI_ERR_PAR          2
#define PI_ERR_ID           3
#define PI_ERR_TO           4
#define PI_ERR_OV           5
#define PI_ERR_NM           6
#define PI_ERR_MM           7
#define PI_ERR_FB           8
#define PI_ERR_CT           9
#define PI_ERR_RUN          10
#define PI_ERR_NQ           11

#define PI_TICK             10          /* 10 msec tick */

#define TMO_TASK_ID         1           /* timeout task id */
#define TMO_TASK_PRIO       100         /* timeout task priority */

#define MAX_SYS_ILVL        0xB         /* maximum ILVL supporting OS calls */


/*==========================================================================*/
/* Macros */

// #define pi_disable() if (1) {_atomic_(3); IEN = 0; _nop_(); _nop_();} else
// #define pi_enable() IEN = 1

#define pi_in_isr() ((PSW & 0xF000) != 0)

#define pi_disable()                                \
    if (!pi_int_disabled)                           \
    {                                               \
        pi_PSW = PSW;                               \
        _atomic_(3);                                \
        _bfld_(PSW, 0xF000, MAX_SYS_ILVL << 12);    \
        _nop_();                                    \
        _nop_();                                    \
        pi_int_disabled = TRUE;                     \
    } else

#define pi_enable()                                 \
    if (pi_int_disabled)                            \
    {                                               \
        pi_int_disabled = FALSE;                    \
        PSW = pi_PSW;                               \
    } else

#define pi_call_hw_int(nr) _trap_(nr)

#define pi_alloc(x) pi_malloc(0,x)

#define ms2tick(x) (x / PI_TICK)
#define tick2ms(x) (x * PI_TICK)

#define get_sys_time() (pi_ticks_elapsed() / (PI_TICK * 1000))

#define Check(x) if ((err = x) != 0) goto error; else


/*==========================================================================*/
/* Globals */

extern short          pi_wspsize;       /* default workspace size for the tasks        */
extern t_rtx_handle   pi_task_handle;   /* temporary storage for the task handles      */

extern char           pi_int_disabled;  /* if TRUE the interrupts are currently disab. */
extern unsigned short pi_PSW;           /* saved PSW for interrupt disabling           */


/*==========================================================================*/
/* Prototypes */

/*--------------------------------------------------------------------------*/
/* External functions (defined in pi_rtx_a.a66) */

extern void pi_copy8(char huge *target, char huge *source, unsigned short size) @0x1F10;
extern void pi_copy16(int huge *target, int huge *source, unsigned short size) @0x1F10;
extern void pi_zero8(char huge *target, unsigned short size) @0x0310;
extern void pi_zero16(int huge *target, unsigned short size) @0x0310;


/*--------------------------------------------------------------------------*/
/* Functions (defined in pi_rtx.c) */

short           pi_init(                char            *mempool,
                                        unsigned short  mempool_size,
                                        short           proc_num,
                                        short           sem_num,
                                        short           mq_num,
                                        short           tmo_num,
                                        short           tmo_wspsize,
                                        unsigned short  cpu_clock,
                                        int             allow_isr_alloc);


void            pi_lock_task(           void);
void            pi_unlock_task(         void);

void            pi_mempool_init(        char            *p,
                                        unsigned short  size);

char            *pi_malloc(             char            *mempool,
                                        unsigned short  size);

void            pi_free(                char            *p);

unsigned long   pi_mempool_use(         char            *mempool);


short           pi_create_semaphore(    short           init,
                                        short           opt,
                                        short           *errp);

short           pi_inquiry_semaphore(   short           id,
                                        short           *errp);

void            pi_pend_semaphore(      short           id,
                                        long            timeout,
                                        short           *errp);

void            pi_post_semaphore(      short           id,
                                        short           *errp);



short           pi_create_queue(        short           size,
                                        short           opt,
                                        short           *errp);

char            *pi_receive_queue(      short           id,
                                        long            timeout,
                                        short           *errp);

void            pi_send_queue(          short           id,
                                        char            *msgp,
                                        short           *errp);

char            *pi_accept_queue(       short           id,
                                        short           *errp);


short           pi_create_timeout(      short           *p_id,
                                        void            (*p_calledfunc)(),
                                        void            *p_para,
                                        short           count);

short           pi_enable_timeout(      short           id,
                                        unsigned long   ticks);

short           pi_disable_timeout(     short           id);
short           pi_delete_timeout(      short           id);


void            pi_wait(                unsigned long   ticks);
unsigned long   pi_ticks_elapsed(       void);

#define pi_task_create(name, id, prio) os_create_task(&pi_task_handle, id, WSPSIZE)
#define pi_task_exit() os_delete_task(os_running_task_id())
#define PI_TASK(name, id, prio)                     \
void name##_main();                                 \
void name() _task_ id _priority_ prio               \
{                                                   \
    os_set_error_handler(rtx_user_error_handler);   \
    name##_main();                                  \
    os_delete_task(os_running_task_id());           \
}                                                   \
void name##_main()
#define PI_TASK_DEF(name) void name(void)

/* warning: this call is here for backward compatibility only */
#define install_os_error_handler() os_set_error_handler(rtx_user_error_handler)

void rtx_user_error_handler(t_rtx_exit err_code, t_sys_call_id system_call);


#endif


/* ****************************************************************************
    $Log: pi_rtx.h,v $
    Revision 1.2.4.2  2005/03/01 15:00:54  schiavor
     01/03/05                  Napoli                       SchiavoR
    sostituito include reg167.h con cpudrv.h

    Revision 1.2.4.1  2005/02/10 12:28:29  schiavor
    09/02/2005               Napoli                     SchiavoR
        si inserisce catenaccio su include reg167.h

    Revision 1.2  2004/11/26 12:24:38  mungiguerrav
    
    2004/nov/26,ven 13:00           Napoli      Mungi
    - Si aggiungono keyword CVS e storia invertita
    - Si sostituiscono TAB con 4 spazi


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
02.40   1999/lug/07,mer             Napoli              Carnevale
        - Modifiche non documentate

 ------------------------   Prima di CVS    ------------------------
02.3x   1998/mmm/gg                 Napoli              Carnevale
        - Modifiche non documentate
02.2x   199X/mmm/gg                 Napoli              Carnevale
        - Modifiche non documentate
02.1x   199X/mmm/gg                 Napoli              Carnevale
        - Modifiche non documentate
02.0x   199X/mmm/gg                 Napoli              Carnevale
        - Modifiche non documentate
02.00   1996/mmm/gg                 Napoli              Carnevale
        - Creazione adattata, ma come ?
01.00   aaaa/mmm/gg                 ??????              ????????
        - Creazione
*/

