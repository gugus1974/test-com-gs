/*=============================================================================================*/
/* Serial standard input/output routines using the 68360 CPM    					           */
/* header file (sio_cpm.h)													                   */
/*																					           */
/* Version 1.0																		           */
/*																					           */
/* (c)1997 Ansaldo Trasporti														           */
/* Written by Giuseppe Carnevale													           */
/*=============================================================================================*/

#ifndef _SIO_CPM_
#define _SIO_CPM_


/*=============================================================================================*/
/* Prototypes */

void sio_cpm_init(int sxc, int brg_clk, unsigned long baud_rate);
void sio_cpm_get(char *buf, short req_num, short timeout, short *act_num);
void sio_cpm_put(const char *buf, short req_num);
void sio_cpm_put_flush(void);


/*=============================================================================================*/


#endif

