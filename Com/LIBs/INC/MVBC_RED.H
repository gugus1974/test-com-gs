/*=============================================================================================*/
/* MVBC line redundancy monitoring													           */
/* Header file (mvbc_red.h)															           */
/* 																					           */
/* Version 1.0																		           */
/*																					           */
/* (c)1998 Ansaldo Trasporti														           */
/* Written by Giuseppe Carnevale													           */
/*=============================================================================================*/


#ifndef _MVBC_RED_
#define _MVBC_RED_


/*=============================================================================================*/
/* Typedefs */

typedef struct {					/**** MVBC line redundancy configuration *******************/
	unsigned short	timeout;		/* time in ms to schedule the check process                */
	unsigned short	n_dev;			/* number of devices to check                              */
	/* array of MvbcRedDevEntry follows */
} MvbcRedConfHeader;

typedef struct {					/**** MVBC line redundancy device info entry ***************/
	unsigned short	mvb_address;	/* device address on the bus                               */
	unsigned short	mf;				/* master frame to check                                   */
} MvbcRedDevEntry;

typedef struct {					/**** MVBC line redundancy status record *******************/
	char			line_ok[2];		/* if FALSE there is a problem on the corresponding line   */
	char			cl_2p[2][32];	/* bitfield of class 2+ present devices for each line      */
	char			cl_1[2][32];	/* bitfield of class 1 present devices for each line       */
} MvbcRedStatus;


/*=============================================================================================*/
/* Prototypes */

unsigned char get_mvb_line(unsigned int bus_id);
int set_mvb_line(unsigned int bus_id, unsigned char command);
void get_mvb_line_selection(char *tx_line, char *rx_line);
void select_mvb_line(char tx_line, char rx_line);

short mvbc_red_init(const MvbcRedConfHeader *cfg);
short mvbc_red_status(MvbcRedStatus *status);


/*=============================================================================================*/

#endif

