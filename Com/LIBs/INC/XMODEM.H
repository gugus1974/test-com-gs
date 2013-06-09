/*=============================================================================================*/
/* Memory XMODEM                                    								           */
/* Header file (xmodem.h)															           */
/* 																					           */
/* Version 1.0																		           */
/*																					           */
/* (c)1997 Ansaldo Trasporti														           */
/* Written by Giuseppe Carnevale													           */
/*=============================================================================================*/


/*=============================================================================================*/
/* Constants */

/* Error codes */
#define XMODEM_OK			0	/* data transfer succesful */
#define XMODEM_MEM_ERR		1	/* out of malloc() memory */
#define XMODEM_CANCEL_ERR	2	/* transfer cancelled */
#define XMODEM_RETRY_ERR	3	/* too many errors */
#define XMODEM_BUFFER_ERR	4	/* invalid buffer size */
#define XMODEM_ORDER_ERR	5	/* received packet out of order */

#define XMODEM_PACKET_SIZE	128	/* the size of the XMODEM packet */


/*=============================================================================================*/
/* Typedefs */

typedef short (*PutXPktFct)(long index, char *pkt);
typedef short (*GetXPktFct)(long *index, char **pkt);


/*=============================================================================================*/
/* Prototypes */

short xmodem_crc_rcv(PutXPktFct put_x_pkt_fct);
short xmodem_crc_snd(GetXPktFct get_x_pkt_fct);

short xmodem_crc_buff_rcv(char *buf, long buf_size, long *act_size);
short xmodem_crc_buff_snd(char *buf, long buf_size);

