/*

cs
  5. August 1996, 10:11
*/
/****************************************************************
*  AAA   BBBB   BBBB   |                                        *
* A   A  B   B  B   B  |                mac.h                   *
* AAAAA  BBBB   BBBB   |                                        *
* A   A  B   B  B   B  |                                        *
* A   A  BBBB   BBBB   |       TRAIN COMMUNICATION NETWORK      *
*                      |                                        *
*  ASEA BROWN BOVERI   |      Copyright 1996 by ABB Henschel    *
*----------------------+----------------------------------------*
*                                                               *
* author   :  Schaefers                                         *
* version  :  1.0                                               *
* date     :  05.08.96                                          *
*                                                               *
*-------------------------------------------------------------- *
*                                                               *
*                        interface for MAC                      *
*                                                               *
*-------------------------------------------------------------- *
*  changes:                                                     *
*                                                               *
* version    date    name  reason                               *
* -------  --------  ----  ------                               *
* 1.0      05.08.96  cs    creation                             *
* 4.0-8                    mac_get_inaug_line_error_node()      *
****************************************************************/
#ifndef MAC_H
#define MAC_H
/*
 * include files
 */
/*
 * definition of general constants
 */
/*
 * definition of exported types
 */
/*
 * definition of exported functions
 */
extern void mac_get_info
                   ( unsigned char  *addr_inaug_line_error
                   , unsigned char  *fritt_source_on
                   );
/* precond.:  ---
 * action:    returns additional information about the MAC
 * postcond.: 
 * returns:   addr_inaug_line_error: the node address where a line error
 *               has een detected during inauguration
 *               0x00       no line error
 *               otherwise  link_layer address of the error node
 *            fritt_source_on: bitfield of the directions where the fritting
 *               source currently is switched on
 *               0x00       no fritting source is on
 *               0x01       fritting source in dir_1 is on
 *               0x02       fritting source in dir_2 is on
 *               0x03       fritting source in both directions are on
 */
#endif
