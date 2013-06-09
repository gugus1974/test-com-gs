/*=============================================================================================*/
/* Xilinx FPGAs utilities                                                                      */
/* Implementation file (xilinx.c)													           */
/* 																					           */
/* Version 1.0																		           */
/*																					           */
/* (c)1999 Ansaldo Trasporti														           */
/* Written by Giuseppe Carnevale													           */
/*																					           */
/*=============================================================================================*/


/*=============================================================================================*/
/* Includes */

#include "pi_sys.h"
#include "c_utils.h"

#include "xilinx.h"


/*=============================================================================================*/
/* Private routines */

/*---------------------------------------------------------------------------------------------*/
/* Get the length from the program header */

static long xilinx_get_len(const unsigned char *program)
{
	char t[4];	/* temporary storage for the header data */
	long l;		/* generic long                          */

   	/* load the header data and swap the bits */
   	t[0] = program[1]; t[1] = program[2]; t[2] = program[3]; t[3] = program[4];
   	c_bswap8(t + 0);   c_bswap8(t + 1);   c_bswap8(t + 2);   c_bswap8(t + 3);

	/* check the header */
	if (program[0] != 0xFF || (t[0] & 0xF0) != 0x20 || (t[3] & 0x0F) != 0x0F) return -1;

	/* return the number of clocks rounded to a byte boundary */
	l = (c_read_be32(t) >> 4) & 0x00FFFFFFL;
	return c_int_roundup(l, 8);
}


/*---------------------------------------------------------------------------------------------*/
/* Send the data to the FPGA */

static void xilinx_send(const unsigned char *data, long size, XilinxSend send_func, long max_size)
{
	unsigned short	n_transfer;		/* number of bytes in each transfer  */

	while (size) {

		/* calculate the number of bytes in this transfer */
		n_transfer = size;
		if (max_size && n_transfer > max_size) n_transfer = max_size;

		/* send the data */
 		send_func(data, n_transfer);

		/* update the pointer and the number of bytes to send */
 		data += n_transfer;
 		size -= n_transfer;
    }
}


/*=============================================================================================*/
/* Public routines */

/*---------------------------------------------------------------------------------------------*/
/* Merges the programs specified in 'program_list' and sends them using 'send_func' in chunks  */
/* of 'max_size' bytes                                                                         */

short xilinx_program(const unsigned char **program_list, XilinxSend send_func, long max_size)
{
	int				i;				/* generic index                */
	const char		*p;				/* pointer in the program       */
	int				n_devices;		/* number of devices to program */
	long			clk;			/* clock counter                */
	long			l;				/* generic long                 */
	char			hdr[5];			/* header buffer                */

	/* loop for all the programs to count them and calculate the number of clocks for the loading */
	for (i = 0, n_devices = 0, clk = 0; (p = program_list[i]) != 0; i++, n_devices++) {
		if ((l = xilinx_get_len(p)) < 40) return XILINX_FORMAT_ERR;
 		clk += l - 40;
    }
    clk += 40 + (n_devices - 1) * 8;

	/* build the new header and send it */
	hdr[0] = 0xFF;
	c_write_be32(hdr + 1, (clk << 4) | 0x2000000FL);
   	c_bswap8(hdr + 1);
   	c_bswap8(hdr + 2);
   	c_bswap8(hdr + 3);
   	c_bswap8(hdr + 4);
	xilinx_send(hdr, sizeof(hdr), send_func, max_size);

	/* send all the programs */
	for (i = 0; (p = program_list[i]) != 0; i++)
		xilinx_send(p+5, (xilinx_get_len(p) - 40) / 8, send_func, max_size);

	/* send the trailing bytes necessary to flush the more distant devices */
	for (i = 1; i <= n_devices; i++) {
		hdr[0] = 0xFF;
		xilinx_send(hdr, 1, send_func, max_size);
	}

	return 0;
}


/*=============================================================================================*/

