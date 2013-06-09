/*



*/
/*
HEADER
*/
/*****************************************************************
;*  AAA   BBBB   BBBB   |                                        *
;* A   A  B   B  B   B  |           M o d u l h e a d e r        *
;* AAAAA  BBBB   BBBB   |                                        *
;* A   A  B   B  B   B  |                                        *
;* A   A  BBBB   BBBB   |      TRAIN COMMUNICATION NETWORK       *
;*                      |                                        *
;*  ASEA BROWN BOVERI   |      Copyright 1992 by ABB Henschel    *
;*----------------------+----------------------------------------*
;*                                                               *
;*Functions:       ZENTRALE ABLAUFSTEUERUNG                      *
;*                                                               *
;*Date:            10.01.92                                      *
;*Author:          G. Hans                                       *
;*Version:         1.0                                           *
;*Release:         xx.xx.xx                                      *
;*Tools:           IC86 Compiler (Intel)                         *
;*                                                               *
;*-------------------------------------------------------------- *
;*                      Short Description                        *
;*                                                               *
;*---------------------------------------------------------------*
;*File:                    -                                     *
;*---------------------------------------------------------------*
;*Procedures:                                                    *
;*                                                               *
;*                                                               *
;*---------------------------------------------------------------*
;*Configuration:                                                 *
;*                                                               *
;*---------------------------------------------------------------*
;*                                                               *
;*Called Procedures:       -                                     *
;*                         -                                     *
;*Used Files:              -                                     *
;*                                                               *
;*---------------------------------------------------------------*
;*Segmentation:                                                  *
;*                                                               *
;*---------------------------------------------------------------*
;*Changes:                                                       *
;*version   yy-mm-dd   name             dept.                    *
;*-------   --------   ----             ----                     *
;*  1.0     92-02-20   G.Hans           WAG/M/TL3                *
;*  3.0                                                          *
;*  3.0     93-09-23   M.Becker         WAG/NTL1                 *
;*                     type_ws_el  &unsigned int& changed        *
;*                                  to &unsigned short&          *
;*  4.0     94-12-13   M.Becker         AHE/STL3                 *
;*                     COCO.H added                              *
;*                     WS_EL_SIZE changed                        *
;*                     OK changed to ZAS_OK (illegal macro def.) *
;*  4.0-9   96-08-05   cs                                        *
;*                     checking for a new entry during operation *
;*                     in zas_activate()                         *
;*  4.1-20  96-09-26   cs                                        *
;*                     portability change: logical instead of    *
;*                     binary OR                                 *
;*  4.1-37  97-01-09   cs                                        *
;*                     zas_busy has to be set in critical region *
;*****************************************************************/
/*
INCLUDES
*/
#ifndef COCO_H
#include "coco.h"
#endif
#include "tcntypes.h"   /*4.0*/
#include "bmi.h"
#include "wtbdrv.h"
#if defined(O_960) || defined(O_VCU6U_GW)
#include "pi_sys.h"
#else
#include "pil.h"
#endif
#include "zas.h"
/*
DEFINES
*/
/*****************************************************************/
/**               C O N S T A N T S                             **/
/*****************************************************************/
#define WS0_SIZE 032             /* Size of Queue 0 in elements */
#define WS1_SIZE 001             /* Size of Queue 1 in elements */
#define WS2_SIZE 001             /* Size of Queue 2 in elements */
#define WS_EL_SIZE 5             /* (Size_of_queue_element - 3) in word */
#define MAX_WS     3             /* Number of queues */
#define MAX_PROC   10            /* Number of Processes */
/*-- Errorcodes and States --*/
#define STAT_INIT   0x0001 
#define STAT_RUN    0x0002 
#define STAT_ERR1   0x0003
#define STAT_ERR2   0x0004 
#define  ZAS_OK      0x0000   
#define  ERR_PNR     0x0001   
#define  ERR_WSNR    0x0002
#define  ERR_WSVOLL  0x0001   
#define  ERR_SUB     0x0002   
/*
TYPEDEFS
*/
/*****************************************************************************
                        Type Definitions
*****************************************************************************/
  typedef struct { 
    unsigned short pnr ; 
    unsigned short data [WS_EL_SIZE];
    unsigned long systime;
  } type_ws_el;
/*
DECLARATIONS
*/
/*****************************************************************************
                        Declaration of variables 
*****************************************************************************/
  unsigned char zas_wa;           /* Flag, true if entry in queue is made */
  unsigned char zas_count;        /* Lifesign of ZAS (Counter)            */
  unsigned char zas_status;       /* Statusbyte of ZAS                    */
  unsigned char zas_busy;         /* Busyflag of ZAS, Set if ZAS busy     */
  unsigned char zas_new_entry;    /* flag, set has received a new entry   */ /*4.0-9*/
  unsigned char zas_wf;           /* Flag, sets ZAS in wait state         */
  unsigned char zas_hf;           /* Flag, sets ZAS in halt state         */
  unsigned char zas_if;           /* Flag, set if ZAS initialized         */
  unsigned char zas_ze;           /* Flag, set if ZAS handles queues      */
  unsigned char zas_watchdog;     /* Flag, always set to 0 by ZAS         */
  unsigned char zas_wssize [MAX_WS]; /* Size of individual queues */
  unsigned char zas_wsadr  [MAX_WS]; /* Realtive index of individual queues */
  unsigned char zas_lz  [MAX_WS]; /* Read Pointers */
  unsigned char zas_sz  [MAX_WS]; /* Write Pointers */
  type_ws_el  ws [WS0_SIZE+WS1_SIZE+WS2_SIZE]; /* Queues */
/*
PROTOTYPES
*/
/*****************************************************************************
                        P R O T O T Y P E S
*****************************************************************************/
void (*zas_ptab[10]) (type_ws_el *mptr);
void (*zas_destproc) (type_ws_el *mptr);
void zas_dmyprc (type_ws_el *mptr);
void zas (void);
/*
FUNCTIONS
*/
/*
void zas_activate (void)
*/
/*****************************************************************
;*                       zas_activate                            *
;*-------------------------------------------------------------- *
;* Function:  Activation of the ZAS                              *
;*-------------------------------------------------------------- *
;*                   Description & Remarks                       *
;*                                                               *
;*                                                               *
;*****************************************************************/

void zas_activate (void)
{
  pi_disable ();            /* start of critical region */
  if (zas_busy == FALSE)
  {
    zas_busy = TRUE;
    do{
      zas_new_entry = FALSE;  /*4.0-9*/
      pi_enable();            /* end of critical region */
      zas();
      pi_disable();           /*4.0-9*//* start of critical region */
    }
    while (zas_new_entry);    /*4.0-9*/
    zas_busy = FALSE;         /*4.1-37*/
    pi_enable();              /*4.0-9*//* end of critical region */
    /* zas_busy = FALSE;        4.1-37*/
  }
  else
  {
    zas_new_entry = TRUE; /*4.0-9*/
    pi_enable();
  }
}
/*
void zas_init (void)
*/
/*****************************************************************
;*                       zas_init                                *
;*-------------------------------------------------------------- *
;* Function:  initialization of the ZAS                          *
;*-------------------------------------------------------------- *
;*                   Description & Remarks                       *
;*                                                               *
;*                                                               *
;*****************************************************************/


void zas_init (void)
{
  int j;
  /*** Initialize flags and counters ***/
  zas_status    = STAT_INIT;
  zas_if        = TRUE;
  zas_busy      = FALSE;
  zas_new_entry = FALSE;    /*4.0-9*/
  zas_count     = 0;
  zas_wa        = 0;
  zas_wf        = FALSE;
  zas_hf        = FALSE;
  /*** Initialize tables for queue administration ***/
  zas_wssize[0]  = WS0_SIZE;
  zas_wsadr[0]   = 0;
  zas_wssize[1]  = WS1_SIZE;
  zas_wsadr[1]   = WS0_SIZE;
  zas_wssize[2]  = WS2_SIZE;
  zas_wsadr[2]   = WS0_SIZE + WS1_SIZE;
  /*** Initialize queues ***/
  zas_norm();           
  /*** Initialize process table ***/
  for (j = 0; j <= (MAX_PROC-1); j++)
  {
    zas_ptab[j] = zas_dmyprc;
  }
}
/*
void zas_norm (void)
*/
/*****************************************************************
;*                       zas_norm                                *
;*-------------------------------------------------------------- *
;* Function:  normalize the queues                               *
;*-------------------------------------------------------------- *
;*                   Description & Remarks                       *
;*                                                               *
;*                                                               *
;*****************************************************************/

void zas_norm (void)
{
  int i;
   /* initialize write and read indices of the queues */
  for (i=0; i<=(MAX_WS-1); i++)
  {
    zas_sz[i] = 0;
    zas_lz[i] = 0;
  }
  /* initialize queues */ 
  ws[0].pnr = 0;
  ws[0].systime = 0;
  for (i=0; i<=(WS_EL_SIZE-1); i++)
  {
    ws[0].data[i] = 0;
  }
  for (i=1; i<=(WS0_SIZE+WS1_SIZE+WS2_SIZE-1); i++)
  {
    ws[i] = ws[0];
  }
}
/*
unsigned char zas_send (unsigned char m, void *dataptr)
*/
/*****************************************************************
;*                       zas_send                                *
;*-------------------------------------------------------------- *
;* Function:  Put a message into a queue                         *
;*-------------------------------------------------------------- *
;*                   Description & Remarks                       *
;*                                                               *
;* The message is copied into queue with number m.               *
;*                                                               *
;*                                                               *
;*****************************************************************/

unsigned char zas_send (unsigned char m, void *dataptr)
{
  type_ws_el *ptr;
  unsigned char neuzei,ws_i,sz_akt;
  unsigned char x_error;
  if (m < MAX_WS)                        /* priority valid ? */
  {
    pi_disable();                        /* start critical region */
    neuzei = zas_sz[m] + 1;              /* calculate queue index */
    if (neuzei >= zas_wssize[m])
    {
      neuzei = 0;
    }
    if (zas_lz[m] == neuzei)             /* queue full ? */
    {
      /* case: YES */
      pi_enable();                     /* critical region end */
      x_error = ERR_WSVOLL;            /* error */
    }
    else
    {
      /* case: NO */
      sz_akt = zas_sz[m];              /* sz_akt := actual write index */
      zas_sz[m] = neuzei;              /* increment write index */
      ++zas_wa;                        /* lock ZAS */
      pi_enable();                     /* critical region end */
      ws_i  = zas_wsadr[m] + sz_akt;      /* calculate index of queue element */
      ptr = dataptr;                   /* pointer to element */
      ws[ws_i] = *ptr;                 /* copy element */ 
      ws[ws_i].systime = d_tc_get_systime(); /* time stamp */
      --zas_wa;                       /* unlock ZAS */
      x_error = ZAS_OK ;               /* no error */
    }
  }
  else
  {
    /* case: priority not valid */
    x_error = ERR_WSNR;
  }
  return x_error;
}
/*
unsigned char zas_subscribe (unsigned char pnr, void (*p_proc) (void *mptr))
*/
/*****************************************************************
;*                       zas_subscribe                           *
;*-------------------------------------------------------------- *
;* Function:  Subscribe a process                                *
;*-------------------------------------------------------------- *
;*                   Description & Remarks                       *
;*                                                               *
;* The address of a function representimg a ZAS process          *
;* is stored within a function table zas_ptab                    *
;*                                                               *
;*****************************************************************/

unsigned char zas_subscribe (unsigned char pnr, void (*p_proc) (void *mptr))
{
  unsigned char x_error;
  if (pnr > MAX_PROC-1)
  {
    x_error = ERR_PNR;             /* priority correct ? */
  }
  else
  {
    if (zas_ptab [pnr] != zas_dmyprc)
    {
      x_error = ERR_SUB; /* already subscribed ? */
    }
    else
    {
      zas_ptab [pnr] = (void (*) (type_ws_el *)) p_proc; /* make entry */
      x_error = ZAS_OK;
    }
  }
  return x_error;
}
/*
void zas_dmyprc (type_ws_el *mptr)
*/
/*****************************************************************
;*                       zas_dmyprc                              *
;*-------------------------------------------------------------- *
;* Function:  internal dummy process                             *
;*-------------------------------------------------------------- *
;*                   Description & Remarks                       *
;*                                                               *
;*                                                               *
;*****************************************************************/

void zas_dmyprc (type_ws_el *mptr)
{
  zas_hf = TRUE;                 /* set halt flag */
  zas_ze = TRUE;                 /* cycle end */
  zas_status = STAT_ERR2;
}
/*
void zas_wait (void)
*/
/*****************************************************************
;*                       zas_wait                                *
;*-------------------------------------------------------------- *
;* Function:  Set wait flag                                      *
;*-------------------------------------------------------------- *
;*                   Description & Remarks                       *
;*                                                               *
;*****************************************************************/

void zas_wait (void)
{
  zas_wf = TRUE;
}
/*
void zas_unwait (void)
*/
/*****************************************************************
;*                       zas_unwait                              *
;*-------------------------------------------------------------- *
;* Function:  Reset wait flag                                    *
;*-------------------------------------------------------------- *
;*                   Description & Remarks                       *
;*                                                               *
;*****************************************************************/

void zas_unwait (void)
{
  zas_wf = FALSE;
}
/*
void zas_halt (void)
*/
/*****************************************************************
;*                       zas_halt                                *
;*-------------------------------------------------------------- *
;* Function:  Set halt flag                                      *
;*-------------------------------------------------------------- *
;*                   Description & Remarks                       *
;*                                                               *
;*****************************************************************/

void zas_halt (void)
{
  zas_hf = TRUE;
}
/*
void zas_continue (void)
*/
/*****************************************************************
;*                       zas_continue                            *
;*-------------------------------------------------------------- *
;* Function:  Reset halt flag                                    *
;*-------------------------------------------------------------- *
;*                   Description & Remarks                       *
;*                                                               *
;*****************************************************************/

void zas_continue (void)
{
  zas_hf = FALSE;
}
/*
void zas (void)
*/
/*****************************************************************
;*                         zas                                   *
;*-------------------------------------------------------------- *
;* Function:  main function of zas                               *
;*-------------------------------------------------------------- *
;*                   Description & Remarks                       *
;*                                                               *
;* If no halt or wait condition is present, all queues starting  *
;* with queue no. 0 are searched for message entries. If an      *
;* entry is found the related process, known by the first word   *
;* of a message, is called for handling the message. After that, *
;* the procedure starts with queue no. 0 again. If all queues    *
;* are empty, the zas finishes.                                  *
;*                                                               *
;* Remark: a queue will be empty if the values of the read       *
;*         index and the write index are equal                   *
;*****************************************************************/

void zas (void)
{
  char n;
  type_ws_el *ptr;
  unsigned char ws_el;
  n = 0;                                     /* set start condidtions */
  zas_ze = FALSE;
  ++zas_count;                               /* increment ZAS lifesign      */
  while (!(zas_ze || zas_wf || zas_hf))      /* ZAS allowed to operate ? *//*4.1-20: logical OR*/
  {
    /* case: Yes */
    if(zas_if && (n < MAX_WS))              /* all queues handled ? */
    {
      /* case: NO */
      zas_status = STAT_RUN;
      if ((zas_lz[n] == zas_sz[n]) || (zas_wa != 0))  
         /* actual queue empty ?, remote queue access ? */
         /*4.1-20: logical OR*/
      {
        /* case: YES */
        n = n+1;                          /* next queue */
        if (!zas_wa)
        {
          zas_watchdog = 0;    /* reset watchdog flag */
        }
      }
      else
      {
        /* case: NO */
        zas_wf = FALSE;                   /* reset wait flag */
        ws_el  = zas_wsadr[n] + zas_lz[n];/* calculate index of queue element */
        ptr    = &ws[ws_el]; /* pointer to queue element */
        if (ws[ws_el].pnr > MAX_PROC-1)
        {
          zas_hf = TRUE; /* set halt flag */
          zas_ze = TRUE; /* cycle end */
          zas_status = STAT_ERR1; /* error */
        }
        else
        {
          /* case: no error */
          zas_destproc = zas_ptab[ws[ws_el].pnr];  /* pointer to destination process */
          (*zas_destproc) (ptr);        /* call to destination process */
        }
        if (!zas_wf)                      /* wait flag set ? */
        {
          /* case: NO */
          zas_watchdog = 0;              /* reset watchdog flag */
          ++zas_lz[n];                   /* increment read pointer */
          if (zas_lz[n] >= zas_wssize[n])
          {
            zas_lz[n] = 0;
          }
        }
        n = 0; /* start again with first queue */
      }
    }
    else
    {
      zas_ze = TRUE;
    }
  }
}
