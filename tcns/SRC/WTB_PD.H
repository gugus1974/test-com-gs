/*

cs
  1. August 1996,  8:49
*/
/****************************************************************
*  AAA   BBBB   BBBB   |                                        *
* A   A  B   B  B   B  |               wtb_pd.h                 *
* AAAAA  BBBB   BBBB   |                                        *
* A   A  B   B  B   B  |                                        *
* A   A  BBBB   BBBB   |       TRAIN COMMUNICATION NETWORK      *
*                      |                                        *
*  ASEA BROWN BOVERI   |      Copyright 1996 by ABB Henschel    *
*----------------------+----------------------------------------*
*                                                               *
* author   :  Schaefers                                         *
* version  :  1.0                                               *
* date     :  01.08.96                                          *
*                                                               *
*-------------------------------------------------------------- *
*                                                               *
*               external interface for Process_Data             *
*                                                               *
*-------------------------------------------------------------- *
*  changes:                                                     *
*                                                               *
* version    date    name  reason                               *
* -------  --------  ----  ------                               *
* 1.0      01.08.96  cs    creation                             *
* 4.0-5                    process data in poll frames          *
* 4.0-6                    data_set subscription                *
* 4.1-43   27.01.97  cs    WTB_PORT_MAX 0xFF -> 0xBF            *
* 4.1-44   27.01.97  cs    new switch O_LE                      *
****************************************************************/
#ifndef WTB_PD_H
#define WTB_PD_H
/*
 * include files
 */
#include "tcntypes.h"
/*
 * definition of general constants
 */
/* WTB port address structure (TCN/RTP 2.2.2.2.4):

   - for regular process_data the 6 least significant bits define 
     the link_layer node address
   - for destination oriented process data
     port 128                         is the receiving port
     port 128 + link_layer address    is the sending port at the master
 */
#define WTB_PORT_MIN                         0x01
#define WTB_PORT_MAX                         (0x80+0x3F) /*4.1-43*/
/* port number offset for destination oriented process_data 
   (piggybacked in poll frames) */
#define WTB_PORT_PIGGY_OFFSET                0x80
/*
 * function return values
 */
#define LP_OK                     0x00 /* correct termination */
/*      LP_FAILURE : */
#define LP_PRT_PASSIVE            0x01 /* warning dataset not active */
#define LP_ERROR                  0x02 /* general error */
#define LP_CONFIG                 0x03 /* configuration error */
#define LP_MEMORY                 0x04 /* not enough memeory to complete operation */
#define LP_UNKNOWN_TS             0x05 /* unknown traffic store */
#define LP_RANGE                  0x06 /* the data_set name is invalid */
#define LP_DATA_TYPE              0x07 /* unsupported data type */
/* the following values are not (yet) defined in the standard */
#define LP_SUBSCR_EXISTS          0x08 /* dataset has already been subscribed */
#define LP_SUBSCR_UNKNOWN         0x09 /* subscription is unknown */
/*
 * definition of exported types
 */
/* function return value type */
typedef UNSIGNED8              LP_RESULT;    /* possible values are defined above */
/* data_set name */
typedef union 
        {  /* for performance reasons we are using a union */
           UNSIGNED16     u1_ds_plain_name;   /* plain */
           struct data_set_name               /* structured: */
           {
#ifdef O_LE    /* 4.1-44: little/big endian support */
t_bitfield  u2_port_addr : 12;/* port address */
                              /* for WTB: link layer address */
t_bitfield  u2_ts_id     :  4;/* traffic store identifier */
#else
t_bitfield  u2_ts_id     :  4;/* traffic store identifier */
t_bitfield  u2_port_addr : 12;/* port address */
                              /* for WTB: link layer address */
#endif
           } _u2;
        }DS_NAME;

#define ds_plain_name     u1_ds_plain_name    /* label for the plain name */
#define ds_port_addr      _u2.u2_port_addr    /* label for the port index */
#define ds_ts_id          _u2.u2_ts_id        /* label for the traffic store id */
 
/* definition of call-back function type */
typedef void (*DS_EVENT) (UNSIGNED16 instance);
/*
 * definition of exported functions
 */
extern LP_RESULT ds_t_subscribe
                  ( DS_NAME*    data_set  /* name of the data_set */
                  , DS_EVENT    function  /* subscribed function */
                  , UNSIGNED16  instance  /* application instance */
                  );
/* precond.:  a subscription for <data_set> does not exist
 * action:    subscription is registered
 * postcond.: a subscription is registered correctly
 * remark:    a subscription for a WTB port is automatically being desubcribed
 *            after a train bus inauguration
 *            <data_set> determines if it is a send or a receive port
 */
extern LP_RESULT ds_t_unsubscribe
                  ( DS_NAME*    data_set  /* name of the data_set */
                  );
/* precond.:  there exists a subscription for <data_set>
 * action:    the subscription is unregistered
 * postcond.: there is no subscription for <data_set>
 */
#endif
