/*=============================================================================================*/
/* Generic utility functions in ANSI C												           */
/* Implementation file (c_utils.c)													           */
/* 																					           */
/* Version 1.1																		           */
/*																					           */
/* (c)1997 Ansaldo Trasporti														           */
/* Written by Giuseppe Carnevale													           */
/*=============================================================================================*/


/*=============================================================================================*/
/* Includes */               

#include "c_utils.h"
// #include "pi_sys.h"

#include <string.h>
#include <stdio.h>


/*=============================================================================================*/
/* Pragmas */

#ifdef __C166__
#pragma WL(3)
#endif


/*=============================================================================================*/
/* Implementation */

/*---------------------------------------------------------------------------------------------*/
int c_ushort_lt(const void *a, const void *b)
{
	return *(const unsigned short *)a < *(const unsigned short *)b;
}


int c_ushort_eq(const void *a, const void *b)
{
	return *(const unsigned short *)a == *(const unsigned short *)b;
}


int c_ulong_lt(const void *a, const void *b)
{
	return *(const unsigned long *)a < *(const unsigned long *)b;
}


int c_ulong_eq(const void *a, const void *b)
{
	return *(const unsigned long *)a == *(const unsigned long *)b;
}


/*---------------------------------------------------------------------------------------------*/
void c_swap(void *a, void *b, int size)
{
	int  i;
	char *p1, *p2, t;

	for (i = size, p1 = a, p2 = b; i > 0; i--) {
		t = *p1;
 		*p1++ = *p2;
  		*p2++ = t;
	}
}


/*---------------------------------------------------------------------------------------------*/
void c_sort(void *array, int el_size, int el_num, OperatorLT lt)
{
	int		i, j;
	char	*p1, *p2;
	
	for (i = 0, p1 = array; i < el_num; i++, p1 += el_size) {
		for (j = i+1, p2 = p1 + el_size; j < el_num; j++, p2 += el_size) {
			if ((*lt)(p2, p1)) c_swap(p1, p2, el_size);
		}
	}
}


/*---------------------------------------------------------------------------------------------*/
int c_find(const void *el, const void *array, int el_size, int el_num, OperatorLT eq)
{
	int			i;
	const char	*p;
	
	for (i = 0, p = array; i < el_num; i++, p += el_size) {
		if ((*eq)(p, el)) return i;
	}
	return -1;
}


/*=============================================================================================*/
/* Character ring functions */

/*---------------------------------------------------------------------------------------------*/
int c_ring_free_size(CRingDesc *desc)
{
	int free_size = desc->rp - 1 - desc->wp;
	if (free_size < 0) free_size += desc->ring_size;
	return free_size;
}


int c_ring_free_size_cont(CRingDesc *desc)
{
	int free_size = c_ring_free_size(desc);	/* number of free bytes in the ring */
	int i = desc->ring_size - desc->wp;
	if (i < free_size) return i;
	return free_size;
}


/*---------------------------------------------------------------------------------------------*/
int c_ring_ready_size(CRingDesc *desc)
{
	int ready_size = desc->wp - desc->rp;
	if (ready_size < 0) ready_size += desc->ring_size;	
	return ready_size;
}


int c_ring_ready_size_cont(CRingDesc *desc)
{
	int ready_size = c_ring_ready_size(desc);	/* number of ready bytes in the input buffer */
	int i = desc->ring_size - desc->rp;
	if (i < ready_size) return i;
	return ready_size;
}


/*---------------------------------------------------------------------------------------------*/
int c_ring_put(CRingDesc *desc, const char *buf, int req_len)
{
	int free_size = c_ring_free_size(desc);	/* number of free bytes in the ring */
	int len;								/* number of bytes to be transferred */

	/* calculate the number of bytes we can transfer */
	if (free_size >= req_len) len = req_len;
	else len = req_len = free_size;

	/* copy the data from the source if any (splitting it if necessary) */
	if (len) {
		if (buf) {
			int i = desc->ring_size - desc->wp;
			if (len > i) {
				memcpy(desc->ring + desc->wp, (void*)buf, i);
 				buf += i;
	  			len -= i;
	   			desc->wp = 0;
			}
			memcpy(desc->ring + desc->wp, (void*)buf, len);
		}
		desc->wp = (desc->wp + len) % desc->ring_size;
	}

	/* return the number of bytes actually transferred */
	return req_len;
}


/*---------------------------------------------------------------------------------------------*/
int c_ring_get(CRingDesc *desc, char *buf, int req_len)
{
	int ready_size = c_ring_ready_size(desc);	/* number of ready bytes in the input buffer */
	int len;									/* number of bytes to be transferred */

	/* calculate the number of bytes we can transfer */
	if (ready_size > req_len) len = req_len;
	else len = req_len = ready_size;

	/* copy the data to the destination if any (splitting it if necessary) */
	if (len) {
		if (buf) {
			int i = desc->ring_size - desc->rp;
			if (len > i) {
				memcpy(buf, desc->ring + desc->rp, i);
 				buf += i;
  				len -= i;
   				desc->rp = 0;
			}
			memcpy(buf, desc->ring + desc->rp, len);
		}
		desc->rp = (desc->rp + len) % desc->ring_size;
	}

	/* return the number of bytes actually transferred */
	return req_len;
}

/*=============================================================================================*/
/* Message fetch functions */

/*---------------------------------------------------------------------------------------------*/
const char *c_fetch_message(const CMessageDef *messages, int code, char *storage,
							const char *unknown_rule)
{
	/* find the code in the table */
	for (; messages->text; messages++) {
		if (messages->code == code) return messages->text;
	}

	/* not found, build one */
	if (storage) sprintf(storage, unknown_rule? unknown_rule : "%d", code);
	return (const char *)storage;
}


/*=============================================================================================*/

