/*=============================================================================================*/
/* Memory XMODEM                                    								           */
/* Implementation file (xmodem.c)													           */
/*																					           */
/* Version 1.0																		           */
/*																					           */
/* (c)1997 Ansaldo Trasporti														           */
/* Written by Giuseppe Carnevale													           */
/*=============================================================================================*/


/*=============================================================================================*/
/* Includes */

#include <string.h>

#include "xmodem.h"

#include "crc.h"
#include "pi_sys.h"
#include "sio.h"


/*=============================================================================================*/
/* Pragmas */

#ifdef __C166__
#pragma WL(3)
#endif


/*=============================================================================================*/
/* Constants */

/* Number of allowed retries */
#define RETRY_NUM	10

/* Protocol status */
#define STATUS_INITIALIZING		0
#define STATUS_PACKET_OK		1
#define STATUS_PACKET_NOT_OK	2


/*=============================================================================================*/
/* Module globals */
		
/* Characters used in the protocols */
static const char ascii_SOH = 0x01;
static const char ascii_EOT = 0x04;
static const char ascii_ACK = 0x06;
static const char ascii_NAK = 0x15;
static const char ascii_CAN = 0x18;
static const char ascii_C   = 0x43;


/*=============================================================================================*/
/* Implementation */

/*---------------------------------------------------------------------------------------------*/
/* Receive with the XMODEM-CRC protocol */

short xmodem_crc_rcv(PutXPktFct put_x_pkt_fct)
{
	short 			err = XMODEM_OK;				/* error code                 */
	char			*pkt;							/* temporary packet storage   */
	short 			cnt;							/* number of bytes in pkt     */
	short 			retry;							/* retry counter              */
	long			pkt_num = 0;					/* number of received packets */
	unsigned short	pkt_crc;						/* CRC of the packet          */
	short			status = STATUS_INITIALIZING;	/* protocol status            */

	/* memory allocation */
	pkt = pi_alloc(2+128+2);
	if (!pkt) Check(XMODEM_MEM_ERR);

	for (;;) {

		retry = 0;

		do {
			/* in case of error wait for silence on the line */
  			if (status == STATUS_PACKET_NOT_OK) do {
				sio_get(pkt, 1, 100 / PI_TICK, &cnt);
   			} while (cnt);

			/* check the error counter */
  			if (retry++ == RETRY_NUM) Check(XMODEM_RETRY_ERR);

			/* start the packet transfer */
			sio_put(status == STATUS_INITIALIZING  ?    &ascii_C   :
  					status == STATUS_PACKET_OK     ?    &ascii_ACK :
   	  			 /*	status == STATUS_PACKET_NOT_OK ? */ &ascii_NAK, 1);
			sio_get(pkt, 1, 3000 / PI_TICK, &cnt);

			/* check for cancel */
 			if (cnt && pkt[0] == ascii_CAN) Check(XMODEM_CANCEL_ERR);
 			
 			/* assume something is wrong if it isn't the first packet */
			if (status != STATUS_INITIALIZING) status = STATUS_PACKET_NOT_OK;

		} while (!cnt || (pkt[0] != ascii_SOH && pkt[0] != ascii_EOT));
 		
  		/* check for end of transfer */
   		if (pkt[0] == ascii_EOT) break;

		/* receive the packet */
		sio_get(pkt, 2+128+2, 5000 / PI_TICK, &cnt);
 		if (cnt != 2+128+2) {
			status = STATUS_PACKET_NOT_OK;
  			continue;
   		}

		/* check the packet integrity */
		pkt_crc = ((unsigned short)pkt[2+128] << 8) | (unsigned char)pkt[2+128+1];
 		if (pkt[1] != (char)(-1 - pkt[0]) ||
 			pkt_crc != crc_16(crc_table_0x1021, 0, (unsigned char *)pkt + 2, 128)) {
			status = STATUS_PACKET_NOT_OK;
 			continue;
   		}

		/* check the packet order */
		if (pkt[0] == (char)pkt_num) {
			status = STATUS_PACKET_OK;
 			continue;
 		}
		if (pkt[0] != (char)(pkt_num + 1)) Check(XMODEM_ORDER_ERR);

		/* packet ok, save it */
 		Check(put_x_pkt_fct(pkt_num, pkt + 2));
   		pkt_num++;
		status = STATUS_PACKET_OK;
	}
	
error:
	/* close the transfer */
	if (err) {
		sio_put(&ascii_CAN, 1);
 		pi_wait(ms2tick(500));
		sio_put(&ascii_CAN, 1);
 		pi_wait(ms2tick(500));
		sio_put(&ascii_CAN, 1);
	}
	else sio_put(&ascii_ACK, 1);

	/* deallocate the memory and exit */
	if (pkt) pi_free(pkt);
	return err;
}


/*---------------------------------------------------------------------------------------------*/
/* Receive a buffer with the XMODEM-CRC protocol */

static char *put_buf;		/* ptr of the put buffer */
static long put_buf_size;	/* size of the put buffer */
static long put_act_size;	/* actual size of the put buffer */

static short put_x_pkt_fct(long index, char *pkt)
{
	/* put the packet in the put buffer (if enough space is available) */
	put_act_size = (index + 1) * 128;
	if (put_act_size > put_buf_size) return XMODEM_BUFFER_ERR;
	pi_copy8((void*)(put_buf + index * 128), (void*)pkt, 128);
	return XMODEM_OK;
}

short xmodem_crc_buff_rcv(char *buf, long buf_size, long *act_size)
{
	short err;	/* error code */

	/* initialize the put buffer informations */
	put_buf = buf;
	put_buf_size = buf_size;
	put_act_size = 0;

	/* start the transfer */
	err = xmodem_crc_rcv(put_x_pkt_fct);
	
	/* save the results and return to the caller */
	if (act_size) *act_size = put_act_size;
	return err;
}


/*---------------------------------------------------------------------------------------------*/
/* Send with the XMODEM-CRC protocol */

short xmodem_crc_snd(GetXPktFct get_x_pkt_fct)
{
	short 			err = XMODEM_OK;				/* error code                    */
	short 			retry = 0;						/* retry counter                 */
	long			pkt_num = 0;					/* number of transmitted packets */
	char			*pkt;							/* ptr to the current packet     */
	unsigned short	pkt_crc;						/* CRC of the packet             */
	char			c;								/* generic char                  */
	short 			cnt;							/* number of received bytes      */

	for (;;) {

		/* check the error counter */
		if (retry++ == RETRY_NUM) Check(XMODEM_RETRY_ERR);

		/* wait for a packet request */
		sio_get(&c, 1, 6000 / PI_TICK, &cnt);
 
		if (cnt) {

			/* send the packet if required */
			if ((pkt_num == 0 && c == ascii_C) ||
				(pkt_num != 0 && (c == ascii_ACK || c == ascii_NAK))) {

				/* check for end of transfer */
   				if (c == ascii_ACK && !pkt) break;

				/* advance the packet counter and clear the retry counter if everything is ok */
				if (c == ascii_ACK || c == ascii_C) {
   					Check(get_x_pkt_fct(&pkt_num, &pkt));
					pkt_num++;
   					retry = 0;
   	   			}
   				
   	  			if (pkt) {
					/* send the data */
					sio_put(&ascii_SOH, 1);				/* SOH       */
					c = pkt_num;
					sio_put(&c, 1);						/* blk#      */
					c = -1 - c;
					sio_put(&c, 1);						/* 255- blk# */
					sio_put(pkt, 128);					/* data[128] */
               		pkt_crc = crc_16(crc_table_0x1021, 0, (unsigned char *)pkt, 128);
               		c = pkt_crc >> 8;
					sio_put(&c, 1);						/* CRC hi    */
               		c = pkt_crc;
					sio_put(&c, 1);						/* CRC lo    */
				}
   				else sio_put(&ascii_EOT, 1);
  			}
   			else retry++;
  		}
		else retry++;
	}
			
error:
	/* close the transfer */
	if (err) sio_put(&ascii_CAN, 1);
	return err;
}


/*---------------------------------------------------------------------------------------------*/
/* Send a buffer with the XMODEM-CRC protocol */

static char *get_buf;		/* ptr of the get buffer */
static long get_buf_size;	/* size of the get buffer */

static short get_x_pkt_fct(long *index, char **pkt)
{
	/* set the packet ptr (if present) */
	if ((*index + 1) * 128 > get_buf_size) *pkt = 0;
	else *pkt = get_buf + *index * 128;
	return XMODEM_OK;
}

short xmodem_crc_buff_snd(char *buf, long buf_size)
{
	/* check for invalid buffer size */
	if (buf_size % 128 != 0) return XMODEM_BUFFER_ERR;

	/* initialize the put buffer informations */
	get_buf = buf;
	get_buf_size = buf_size;

	/* start the transfer */
	return xmodem_crc_snd(get_x_pkt_fct);
}


/*=============================================================================================*/
/* Test code */

#if 0

#include <stdio.h>
#include "ushell.h"

/*---------------------------------------------------------------------------------------------*/
static short rx(short argc, char *argv[])
{
	char  *buf;
	long  act_size;
	short err;

	if (argc != 1) return -1;

	buf = pi_alloc(16384);
	if (!buf) return 1;

	err = xmodem_crc_buff_rcv(buf, 16384, &act_size);
	if (err) printf("Received error: %d\n", err);
	else {
		printf("Received bytes: %ld\n\n", act_size);
	    dump_buffer(FALSE, (unsigned char*)buf, act_size);
	}

	pi_free(buf);

	return 0;
}


/*---------------------------------------------------------------------------------------------*/
static short sx(short argc, char *argv[])
{
	char  	*buf;
	short 	err;
	short	s;
	short	i;

	if (argc != 2) return -1;
	s = parse_number(argv[1]) * 128;

	buf = pi_alloc(s);
	if (!buf) return 1;
	
	for (i = 0; i < s; i ++) buf[i] = i % 16;

	err = xmodem_crc_buff_snd(buf, s);
	printf("Transfer status: %d\n", err);

	pi_free(buf);

	return 0;
}


/*---------------------------------------------------------------------------------------------*/
extern void install_xmodem_test(void);
void install_xmodem_test(void)
{
	ushell_register("rx", "", "Receive data using XMODEM-CRC", rx);
	ushell_register("sx", "<blocks>", "Send data using XMODEM-CRC", sx);

}

#endif

