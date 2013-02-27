/*



*/
/****************************************************************
*  AAA   BBBB   BBBB   |                                        *
* A   A  B   B  B   B  |                diag.h                  *
* AAAAA  BBBB   BBBB   |                                        *
* A   A  B   B  B   B  |                                        *
* A   A  BBBB   BBBB   |       TRAIN COMMUNICATION NETWORK      *
*                      |                                        *
*  ASEA BROWN BOVERI   |      Copyright 1996 by ABB Henschel    *
*----------------------+----------------------------------------*
*                                                               *
* author   :                                                    *
* version  :  1.0                                               *
* date     :  02.06.92                                          *
*                                                               *
*-------------------------------------------------------------- *
*                                                               *
*            interface for link layer diagnostic module         *
*                                                               *
*-------------------------------------------------------------- *
*  changes:                                                     *
*                                                               *
* version    date    name  reason                               *
* -------  --------  ----  ------                               *
* 4.1-44   27.01.97  cs    DEFINEs now here                     *
****************************************************************/
#ifndef DIAGWTB_H
#define DIAGWTB_H
/*
DEFINEs for diag_entry.diag_nr
*/
#define DIAG_MAC_ERR_GENERAL                0xE001
#define DIAG_MAC_ERR_EVENT                  0xE002
#define DIAG_MAC_ERR_AUX                    0xE003
#define DIAG_MAC_ERR_SD                     0xE005
#define DIAG_MAC_ERR_SEND_REQ               0xE006 /*4.1-32*/
/*#define DIAG_MAC_ERR_SEND_REQ1            0xE006   4.1-32*/
/*#define DIAG_MAC_ERR_SEND_REQ2            0xE007   4.1-32*/
#define DIAG_MAC_ERR_UNNAMED_BY_DETECT_RES  0xE008
#define DIAG_MAC_ERR_ENABLE                 0xE009
#define DIAG_MAC_ERR_DISABLE                0xE00A
#define DIAG_MAC_ERR_PROTOCOL               0xE00B /*4.1-28*/
#define DIAG_MMI_ERR_LOW_WS_FULL            0xE11A
#define DIAG_MMI_ERR_HIGH_WS_FULL           0xE11B
#define DIAG_MMI_ERR_MD_PACKET_LOST         0xE11C
#define DIAG_MMI_ERR_MD_FRAME_LOST          0xE11D
#define DIAG_MMI_ERR_MD_CONFIRM             0xE11E
#define DIAG_BMI_ERR                        0xE201
#define DIAG_PDH_ERR                        0xE301
#define DIAG_BM_ERR                         0xE401
/* Initialisierung bei Systemstart */
void diagwtb_sysinit  (void);
/* Eintrag einer Diagnose- oder Fehlermeldung */
void diagwtb_entry ( unsigned short diag_nr,
                  unsigned short diag_par1,
                  unsigned short diag_par2,
                  unsigned short diag_par3,
                  void           *diag_ptr
                );
#define DIAG_BUFFER_SIZE  80     /* diagnostic-buffer-size (amount of entries) */
typedef struct {                                   /* diagnostic buffer element */
                 unsigned short diag_nr;
                 unsigned short diag_par1;
                 unsigned short diag_par2;
                 unsigned short diag_par3;
                 void           *diag_ptr;
                 unsigned long  sys_time;
               } type_diag_buffer_element;
typedef struct  {
                  unsigned short sz;               /* writing pointer */
                  unsigned short res;              /* reserved */
                  unsigned short trigger;          /* trigger TRUE/FALSE */
                  unsigned short trigg_nr;         /* trigger error-number */
                  unsigned short trigg_par1;       /* trigger parameter1 */
                  unsigned short trigg_occ;        /* trigger occurence (it will be decremented) */
                  unsigned short trigg_akti;       /* action that will be done after trigger */
                  unsigned short entry_counter;    /* counter for diag_entry - calls */
                  type_diag_buffer_element diag_buff_ele [DIAG_BUFFER_SIZE]; /* buffer-entries */
                 } type_diag_buffer;
#endif
