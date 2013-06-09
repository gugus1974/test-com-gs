/*



*/
/****************************************************************
*  AAA   BBBB   BBBB   |                                        *
* A   A  B   B  B   B  |               pd_ts.h                  *
* AAAAA  BBBB   BBBB   |                                        *
* A   A  B   B  B   B  |                                        *
* A   A  BBBB   BBBB   |       TRAIN COMMUNICATION NETWORK      *
*                      |                                        *
*  ASEA BROWN BOVERI   |      Copyright 1996 by ABB Henschel    *
*----------------------+----------------------------------------*
*                                                               *
* author   :  P. Gehring                                        *
* version  :  1.0                                               *
* date     :  30.08.1993                                        *
*                                                               *
*-------------------------------------------------------------- *
*                                                               *
*                  MVB Traffic_Store definitions                *
*                                                               *
*-------------------------------------------------------------- *
*  changes:                                                     *
*                                                               *
* version   date    name  reason                                *
* ------- --------  ----  ------                                *
*                   Gg    data consistency over a whole WTB port*
*         10.03.95  Cl    LP_TS_TYPE_D added                    *
*                         (duplicate definition in lp_sys.h)    *
*                         2 duplicate protoypes removed         *
*                         General remark:  there are many       *
*                         duplicate definitions in this module  *
*                         which is very DANGEROUS PROGRAMMING   *
*                         STYLE                                 *
* 4.0-6    01.08.96  cs   synchronisation of process data       *
* 4.1-17   26.09.96  cs   size of STR_TB_PCS adjusted:          *
*                         sizeof(DS_EVENT) == 4                 *
* 4.1-43   27.01.97  cs   type_port_index_table not needed      *
* 4.1-44   27.01.97  cs   new switch O_LE                       *
****************************************************************/
#ifndef PD_JDP_H
#define PD_JDP_H
/*
 * include files
 */
#include "wtb_pd.h"
/*
 * definition of general constants
 */
#define LP_TS_TYPE_D 0x0003  /* duplicate definition!
                                better: include lp_sys.h instead.*/
/*
 * definition of exported types
 */
/*-----------------------     port index table     --------------------------*/
/*typedef unsigned short type_port_index_table[ WTB_PORT_MAX+1 ]; 4.0-5: instead of 0x3F; 4.1-43: not needed */
/*---------------------- port control & status field ------------------------*/
#if defined (O_BAP)
typedef struct STR_TB_PCS           /* !!! modified size !!! */
{
      unsigned int     reserved0 : 1 ;   /*  byte 0 */
      unsigned int  type         : 2 ;
      unsigned int     reserved1 : 1 ;
      unsigned int  fcode        : 4 ;
      unsigned int     reserved2 : 8 ;    /*  byte 1 */
      unsigned int     reserved3 : 3 ;    /*  byte 2 */
      unsigned int  page         : 1 ;
      unsigned int     reserved4 : 4 ;
      unsigned int  tack         : 8 ;    /*  byte 3 */
/*    unsigned char reserved_a[8];      *//* byte 4...11 */  /*4.0-6*/
      unsigned char reserved_a[2];        /* byte 4...5 */   /*4.0-6*/
      DS_EVENT      function;             /* byte 6..9*/     /*4.0-6*/
      UNSIGNED16    instance;             /* byte 10 + 11 */ /*4.0-6*/
                                                             /*4.1-17*/
      unsigned char size;                 /* byte 12 */
      unsigned char reserved_b[3];        /* byte 13...15 */
}        TYPE_TB_PCS;
#elif defined (O_MVBC)
typedef struct STR_TB_PCS           /* !!! new MVBC PCS structure !!! */
{
#ifndef O_LE                 /* 4.1-44: little endian */
unsigned int   fcode     :  4;   /* byte 0 + 1 */
unsigned int   reserved1 : 10;    
unsigned int   type      :  2;
unsigned int   reserved2 :  9;   /* byte 2 + 3 */
unsigned int   page      :  1;
unsigned int   reserved3 :  6;    
unsigned short tack;             /* byte 4 + 5*/
DS_EVENT       function;         
UNSIGNED16     instance;       
unsigned char  size;
unsigned char  reserved_b[3];
#else
unsigned int     reserved1 : 10;/*  byte 0 + 1 */
unsigned int   type        : 2 ;
unsigned int   fcode       : 4 ;
unsigned int     reserved2 : 6 ;/*  byte 2 + 3 */
unsigned int   page        : 1 ;
unsigned int     reserved3 : 9 ;
unsigned short tack;            /* byte 4 + 5*/
/*unsigned char  reserved_a[6];    byte 6.. 11 */  /*4.0-6*/
DS_EVENT      function;         /* byte 6..9 */    /*4.0-6*/
UNSIGNED16    instance;         /* byte 10 + 11 */ /*4.0-6*/
                                                       /*4.1-17*/
unsigned char size;             /* byte 12 */
unsigned char    reserved_b[3]; /* byte 13...15 */
#endif
      }        TYPE_TB_PCS;
#else
#error  identifier missing
#endif
typedef struct STR_TB_PRT_PGE       /* !!! modified size !!! */
      {
         unsigned char           tb_dck[128];
      }  TYPE_TB_PRT_PGE;
typedef struct STR_TB_PRT           /* !!! modified size !!! */
      {
         struct STR_TB_PRT_PGE   pge[2];
      }   TYPE_TB_PRT;
/*
----------------------------- frame structure ---------------------------------
*/
typedef struct STR_IDU
{
      unsigned char          dd;        /* destinationdevice     */
      unsigned char          llc;       /* logical-link-control  */
      unsigned char          sd;        /* sourcedevice          */
      unsigned char          size;      /* information-data-size */
      struct STR_TB_PRT_PGE   data;
} type_idu;               /* interface-data-unit   */
/*
---------------------------- f u n c t i o n s --------------------------------
*/
void dl_system_init_pdi  ( void                              );
void dl_enable_pdi       ( void *        pointer_to_pd_aerea,
                     unsigned char link_address,
                     unsigned char no_rec_ports,
                     unsigned char no_send_ports,
                     unsigned char port_size
                   );
/*        !!! modified prototype   !!! */
void      pd_receive_indicate( type_idu *idu                     );
/*        !!! new prototype   !!! */
type_idu *pd_request_to_send ( unsigned char destination_device  );
void pd_send_confirm     ( void *idu                         );
void pd_actualize_time   ( void                              );
/*-----------------------     port index table     --------------------------*/
/*-------------------- port control & status field table --------------------*/
/*----------------------------- MVB port page -------------------------------*/
typedef unsigned char type_mvb_prt_page[128];
/*------------------------------- MVB port ele ------------------------------*/
typedef type_mvb_prt_page type_prt_ele[0x02];
/*------------------------------ port data area -----------------------------*/
typedef type_prt_ele type_prt_area[1];
/*------------------------ traffic store descriptor -------------------------*/
typedef struct str_mvb_ts
         {
       unsigned short        *pb_pit;    /* TYPE_PORT_INDEX_TABLE */
       TYPE_TB_PCS           *pb_pcs;    /* TYPE_PCS_TABLE        */
       TYPE_TB_PRT           *pb_prt;    /* TYPE_PRT_AREA         */
       void                  *pb_frc;    /* not used */
       void                  *pb_def;    /* not used */
       void                  *pb_mwd;    /* not used */
       unsigned char         ownership;  /* not used */
       unsigned char         ts_type;
       unsigned short        prt_addr_max;
       unsigned short        prt_indx_max;
       unsigned short        prt_count;  /* not used */
       void                  *p_prt_cfg; /* not used */
         } type_mvb_descriptor;
/*
 * definition of exported functions
 */
#endif
