/*=============================================================================================*/
/* X25642 serial E2PROM access routines                                                        */
/* Implementation file (x25642.c)   												           */
/* 																					           */
/* Version 1.0																		           */
/*																					           */
/* (c)1998 Ansaldo Trasporti														           */
/* Written by Giuseppe Carnevale													           */
/*=============================================================================================*/


/*=============================================================================================*/
/* Includes */

#include "x25642.h"
#include "pi_sys.h"


/*=============================================================================================*/
/* Constants */

/* X25642 commands */
#define X25642_WREN  0x06
#define X25642_WRDI  0x04
#define X25642_RDSR  0x05
#define X25642_WRSR  0x01
#define X25642_READ  0x03
#define X25642_WRITE 0x02

/* X25642 page size */
#define X25642_PAGE_SIZE 32

/* X25642 status register bits */
#define X25642_WPEN  0x80
#define X25642_BP1   0x08
#define X25642_BP0   0x04
#define X25642_WEL   0x02
#define X25642_WIP   0x01


/*=============================================================================================*/
/* Globals */

static X25642_select_fptr	select_func;	/* user select function */
static X25642_transfer_fptr	transfer_func;	/* user transfer function */


/*=============================================================================================*/
/* Private routines */

/*---------------------------------------------------------------------------------------------*/
/* Executes a transfer sequence */

static short x25642_transfer(void *out, unsigned short out_size, void *in, unsigned short in_size)
{
	short err = X25642_OK;

	/* check if the library has been initialized */
	if (!transfer_func) return X25642_UNINITIALIZED_ERR;

	/* quick fix to ensure 2us between /CS assertions (should be improved) */
	pi_wait(2);

	/* select the chip and execute the two transfers */
	Check((*select_func)(1));
	if (out_size) Check((*transfer_func)(out, out_size));
	if (in_size) Check((*transfer_func)(in, in_size));
	Check((*select_func)(0));

error:
	if (err) {
		select_func   = 0;
		transfer_func = 0;
	}
	return err;
}


/*---------------------------------------------------------------------------------------------*/
/* Sends the Write Enable command */

static short x25642_write_enable(void)
{
	unsigned char cmd = X25642_WREN;
	return x25642_transfer(&cmd, sizeof(cmd), 0, 0);
}


/*---------------------------------------------------------------------------------------------*/
/* Waits for a write completion */

static short x25642_write_wait(void)
{
	short		  err;
	unsigned char status;
	unsigned long tick = pi_ticks_elapsed() + ms2tick(20);

	do {
		Check(x25642_read_status(&status));
	} while ((status & X25642_WIP) && tick < pi_ticks_elapsed());

	if (status & X25642_WIP) Check(X25642_FAILURE_ERR);

error:
	return err;
}


/*=============================================================================================*/
/* Implementation */

/*---------------------------------------------------------------------------------------------*/
/* Initializes this library */

short x25642_init(X25642_select_fptr select_fptr, X25642_transfer_fptr transfer_fptr)
{
	/* save the specified function pointers */
	select_func   = select_fptr;
	transfer_func = transfer_fptr;

	return X25642_OK;
}


/*---------------------------------------------------------------------------------------------*/
/* Reads the chip status register */

short x25642_read_status(unsigned char *status)
{
	unsigned char cmd = X25642_RDSR;
	return x25642_transfer(&cmd, sizeof(cmd), status, sizeof(*status));
}


/*---------------------------------------------------------------------------------------------*/
/* Writes the chip status register */

short x25642_write_status(unsigned char status)
{
	short		  err;
	unsigned char cmd[2] = {X25642_WRSR, 0};
	
	Check(x25642_write_enable());
	cmd[1] = status;
	Check(x25642_transfer(&cmd, sizeof(cmd), 0, 0));
	Check(x25642_write_wait());

error:
	return err;
}


/*---------------------------------------------------------------------------------------------*/
/* Reads from the memory */

short x25642_read(unsigned short address, char *buf, unsigned short size)
{
	unsigned char cmd[3] = {X25642_READ, 0, 0};
	cmd[1] = (unsigned char)(address >> 8);
	cmd[2] = (unsigned char)(address);
	return x25642_transfer(&cmd, sizeof(cmd), buf, size);
}


/*---------------------------------------------------------------------------------------------*/
/* Writes to the memory */

short x25642_write(unsigned short address, const char *buf, unsigned short size)
{
	short          err = X25642_OK;
	unsigned short tr_size;
	unsigned char  cmd[3+X25642_PAGE_SIZE];

	while (size) {

		/* calculate the size of this transfer */
		tr_size = X25642_PAGE_SIZE - (address % X25642_PAGE_SIZE);
		if (tr_size > size) tr_size = size;

		/* execute the write operation and wait for its completion */
		Check(x25642_write_enable());
		cmd[0] = X25642_WRITE;
		cmd[1] = (unsigned char)(address >> 8);
		cmd[2] = (unsigned char)(address);
		pi_copy8(&cmd[3], buf, tr_size);
		Check(x25642_transfer(&cmd, 3+tr_size, 0, 0));
		Check(x25642_write_wait());

		/* update the buffer to write */
		address += tr_size;
		buf     += tr_size;
		size    -= tr_size;
	}

error:
	return err;
}


/*---------------------------------------------------------------------------------------------*/

