/***************************************************************************
*
*		Copyright (c) 1992 READY SYSTEMS CORPORATION.
*
*	All rights reserved. READY SYSTEMS' source code is an unpublished
*	work and the use of a copyright notice does not imply otherwise.
*	This source code contains confidential, trade secret material of
*	READY SYSTEMS. Any attempt or participation in deciphering, decoding,
*	reverse engineering or in any way altering the source code is
*	strictly prohibited, unless the prior written consent of
*	READY SYSTEMS is obtained.
*
*
*	Module Name:		bootcnfg.tpl
*
*	Identification:		 6.26 bootcnfg.tpl
*
*	Date:			6/28/94  13:34:38
*
****************************************************************************
*/



#include <boot.h>
#include <bootvsh.h>
#include <router.h>


/* Initialized variables are NOT modified in Code, hence declared as const */

/* Board-dependent portion of boot item list */

const target_t boot_item_target = target_mc68010;
const bootos_memory_t boot_item_memory_rom[] = {
    {(void *) 0x008000,	0x008000,	BOOTOS_MEMORY_BOOT | BOOTOS_MEMORY_READONLY,	0},
    {(void *) 0x010000,	0x0D0000,	BOOTOS_MEMORY_UNUSED_HOST,	0},
    {(void *) 0x100000,	0x020000,	BOOTOS_MEMORY_SHARED,	0},
    {(void *) 0x120000,	0x060000,	BOOTOS_MEMORY_BOOTED,	0},
    {(void *) 0x160000,	0x0A0000,	BOOTOS_MEMORY_UNUSED_TARGET,	0},

    /* above line must be terminated by a comma, unless memory map is empty */
    {0, 0, BOOTOS_MEMORY_END, 0}
};
bootos_memory_t boot_item_memory_ram[sizeof(boot_item_memory_rom)/sizeof(bootos_memory_t)];

/* Boot method table */

extern void boot_method_go(ANSIPROT2(const char *, void *));

const boot_method_t boot_method_table[] = {
    "go",	boot_method_go,	0,
    0,		0,	0
};

const int boot_user_vectors[] = {
	0
};

const int rtr_tx_que_len = 8;
const int rtr_rx_que_len = 16;


/* Initial environment table */

char * const boot_environ_rom[] = {
    "VERBOSITY=3",
    "NODENAME=default",
    "NAMESERVER=default",
    "BOARD=DEV_BOARD",
    "BOOT_ORDER=go=010000",
    0
};

/* Logio configuration parameters */

const int logio_malloc_nbuff = 96;
const int logio_dev_max = 32;
const int logio_max_event_number = 10;
const int logio_table_increment = 3;
const int logio_max_bufpools = 4;
const unsigned int logio_heap_size = 1024;
const int boot_logio_ws_length = 76912;
const int boot_logio_buffer_length = 1600;
char boot_logio_ws[76912];


#ifdef	SPARC
const	unsigned long	cpu_nwindows = ;
#endif

