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
* author   :  WAG/NTL 2 Gg                                      *
* version  :  1.0                                               *
* date     :  24.09.92                                          *
*                                                               *
*-------------------------------------------------------------- *
*                                                               *
*                      Traffic_Store definitions                *
*                                                               *
*-------------------------------------------------------------- *
*  changes:                                                     *
*                                                               *
* version    date    name  reason                               *
* -------  --------  ----  ------                               *
*          15.12.92  Gg    delete structure type_idu            *
*          20.12.93  Br    timer_port added                     *
* 4.0-6    01.08.96  cs    synchronisation of process data      *
****************************************************************/
#ifndef PD_TS_H
#define PD_TS_H
/*
 * include files
 */
#include "wtb_pd.h"
#include "wtb.h"       /*4.2-3*/
/*
 * definition of general constants
 */
#define MAX_SEND_ADR              0xFF 
#define MAX_REC_ADR               0xFF
#define NO_PORT_ELEMENTS          0x03
#define MAX_PD_DATA_PER_FRAME     0x80
/*
 * definition of exported types
 */
/* structure port-element */
typedef struct 
      { unsigned long int rt_rc;     /* receive-time or repeat-counter */
        unsigned char     reserved;  /* this byte is history  */
        unsigned char     dd;        /* destinationdevice     */
        unsigned char     llc;       /* logical-link-control  */
        unsigned char     sd;        /* sourcedevice          */
        unsigned char     size;      /* information-data-size */
        unsigned char     data[MAX_PD_DATA_PER_FRAME]; /* information-data      */  
      } type_port_element; 
/* structure port */
/* !! port_data_size depends on the lengths of this structure !!*/
typedef struct 
      { unsigned char wi;      /* writing index of the port */
        unsigned char ri;      /* reading index of the port */
        unsigned char hi;      /* help index of the port */
        unsigned char valid;   /* flag for used ports */ 
        unsigned char open;    /* flag for open ports */
        unsigned char rec_no;  /* number of the process, which opened the port */
/*      unsigned char reserved_01;                                   4.0-6*/
        DS_EVENT      function;/* subscribed function */           /*4.0-6*/
/*      unsigned char reserved_02;                                   4.0-6*/
        UNSIGNED16    instance;/* instance of subscription */      /*4.0-6*/
        unsigned char port_data[0x400]; /* 0x400 is only a number */
      } type_port;
/* structure time_port */
typedef struct 
      { unsigned char wi;        /* writing index of the port */
        unsigned char ri;        /* reading index of the port */
        unsigned char hi;        /* help index of the port */
        unsigned long time[3];   /* system time */ 
      } type_time_port;
/* structure of pd-traffic-store */
typedef struct 
      {  unsigned char  version;            /* software version index */
         unsigned char  pd_error;           /* pd-data error-byte */
         unsigned char  data_enable;        /* flag for data transmission enabled */ 
         unsigned char  own_device_addr;    /* dito */ 
         unsigned char  no_rec_ports;       /* number of receive-ports */
         unsigned char  no_send_ports;      /* number of send-ports */
         unsigned char  pd_data_length;     /* max. number of pd-data bytes per PD-frame */
         unsigned char  rec_port_count;     /* actual count of used receive ports */   
/* 0x08*/unsigned char  send_port_count;    /* actual count of used send port */
         unsigned char  port_element_size;  /* bytes per port element */
         unsigned short port_data_size;     /* used bytes per port */
         unsigned short offset_send_port;   /* offset in the port_aerea_data - array */
         unsigned short offset_rec_port;    /* offset in the port_aerea_data - array */
/* 0x10*/type_time_port systime;            /* system time com-board */
         unsigned char  *poi2sendport;      /* pointer to the first sendport */
         unsigned char  *poi2recport;       /* pointer to the first receiveport */
         unsigned char  *end_of_pd_aerea;   /* pointer to the firts following byte after the pd_aerea */
/* 0x2B*/unsigned char  rec_port_map  [MAX_REC_ADR+1];  /* mapping table of the receive ports */
/*0x12B*/unsigned char  send_port_map [MAX_SEND_ADR+1]; /* mapping table of the send ports */
/*0x22B*/unsigned char  port_aerea_data[0x8000];      /* and now following the ports */
      } type_pd_dpr;
/*
 * definition of exported functions
 */
/*
 * exported structures
 */
/*4.2-3:*/
extern type_port_element*  pd_port_element_table[WTB_ADR_BROADCAST+1];
#endif
