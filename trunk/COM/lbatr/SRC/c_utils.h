/*=============================================================================================*/
/* Generic utility functions in ANSI C												           */
/* Header file (c_utils.h)															           */
/* 																					           */
/* Version 1.2																		           */
/*																					           */
/* (c)1997-9 Ansaldo Trasporti														           */
/* Written by Giuseppe Carnevale													           */
/*=============================================================================================*/

#ifndef _C_UTILS_
#define _C_UTILS_


/*=============================================================================================*/
/* Typedefs */

typedef int (*OperatorLT)(const void *a, const void *b);
typedef int (*OperatorEQ)(const void *a, const void *b);

typedef struct {		/**** Character ring descriptor */
	char	*ring;		/* pointer to the data          */
	int		ring_size;	/* size of the ring             */
	int		rp;			/* read pointer                 */
	int		wp;			/* write pointer                */
} CRingDesc;

typedef struct {		/**** Message definition */
	int        code;	/* code                  */
	const char *text;	/* text (0 in the last)  */
} CMessageDef;


/*=============================================================================================*/
/* Macros */

/*---------------------------------------------------------------------------------------------*/
/* Bitfields handling */

#define c_set_bit(bf, pos) ((char*)(bf))[(pos) / 8] |= 1 << ((pos) % 8)
#define c_clear_bit(bf, pos) ((char*)(bf))[(pos) / 8] &= ~(1 << ((pos) % 8))
#define c_put_bit(bf, pos, v) \
	((char*)(bf))[(pos) / 8] = (((char*)(bf))[(pos) / 8] & ~(1 << ((pos) % 8))) | \
							   (((v) ? 1 : 0) << ((pos) % 8))
#define c_get_bit(bf, pos) ((((char*)(bf))[(pos) / 8] & (1 << ((pos) % 8))) != 0)


/*---------------------------------------------------------------------------------------------*/
/* Endianess handling (C166 bug free)*/

#define _c_ucharp_off(p,x) ((unsigned char *)(p) + x)
#define _c_uchar(p,x) (*_c_ucharp_off(p,x))

#define c_read_be16(p) (((unsigned short)_c_uchar(p,0) << 8) | _c_uchar(p,1))
#define c_read_be32(p) (((unsigned long)c_read_be16(p) << 16) | c_read_be16(_c_ucharp_off(p,2)))

#define c_read_le16(p) (((unsigned short)_c_uchar(p,1) << 8) | _c_uchar(p,0))
#define c_read_le32(p) (((unsigned long)c_read_le16(_c_ucharp_off(p,2)) << 16) | c_read_le16(p))

#define c_write_be16(p,v) do { _c_uchar(p,0) = (v) >> 8; _c_uchar(p,1) = (v); } while (0)
#define c_write_be32(p,v) do { c_write_be16(p, (v) >> 16); c_write_be16(_c_ucharp_off(p,2),v); } while (0)


/*---------------------------------------------------------------------------------------------*/
/* Bit swapping */

#define _c_bswap(t,s,x) do {				\
	t _x = *(x), _y = 0;					\
	int _i;									\
	for (_i = 0; _i < (s); _i++, _x >>= 1)	\
		_y = (_y << 1) | (_x & 1);			\
	*(x) = _y;								\
} while (0)

#define c_bswap8(x)  _c_bswap(char,   8, x)
#define c_bswap16(x) _c_bswap(short, 16, x)
#define c_bswap32(x) _c_bswap(long,  32, x)


/*---------------------------------------------------------------------------------------------*/
/* Queue handling */

#define c_enqueue_fifo(TYPE, head, elem) do {				\
	TYPE *p = (head), *q;									\
	while ((q = p->next) != 0) p = q;						\
	p->next = (elem);										\
	(elem)->next = 0;										\
} while (0)

#define c_enqueue_lifo(TYPE, head, elem) do {				\
	(elem)->next = (head)->next;							\
	(head)->next = (elem);									\
} while (0)

#define c_enqueue_ord(TYPE, head, elem) do {				\
	TYPE *p = (head), *q;									\
	while ((q = p->next) != 0) {							\
		if (q->ord > (elem)->ord) break;					\
		p = q;												\
	}														\
	(elem)->next = q;										\
	p->next = (elem);										\
} while (0)

#define c_dequeue_elem(TYPE, head, elem, elem_p) do {		\
	TYPE *p = (head), *q;									\
	while ((q = p->next) != 0) {							\
		if (q == (elem)) break;								\
		p = q;												\
	}														\
	if (q) {												\
		(elem_p) = (elem);									\
		p->next = (elem)->next;								\
	}														\
	else (elem_p) = 0;										\
} while (0)

#define c_dequeue_first(TYPE, head, elem_p) do {			\
	TYPE *p = (head)->next;									\
	if (p) (head)->next = p->next;							\
	(elem_p) = p;											\
} while (0)


/*---------------------------------------------------------------------------------------------*/
/* Array handling */

#define C_array_new(array, size) (array = (void*)pi_alloc(size * sizeof(*array)))
#define C_array_free(array) pi_free((void*)array)
#define C_array_find(el, array, size, ix) do {					\
	for (ix = 0; ix < size; ix++) if (array[ix] == el) break;	\
	if (ix == size) ix = -1;									\
} while (0)


/*---------------------------------------------------------------------------------------------*/
/* Signal processing */

/* delay a LO to HI transition for the specified delay (filtering it if it is too short) */
#define C_sig_lo2hi_delay(delay, time, in, out) do {		\
	static char          _in = 0;							\
	static unsigned long _time;								\
	if (in) {												\
		if (!_in) { _in = 1; _time = (time) + (delay); }	\
		if (_time < (time)) (out) = 1;						\
		else (out) = 0;										\
	}														\
	else { _in = 0; (out) = 0; }							\
} while (0)

/* delay a HI to LO transition for the specified delay (filtering it if it is too short) */
#define C_sig_hi2lo_delay(delay, time, in, out) do {		\
	static char          _in = 0;							\
	static unsigned long _time;								\
	if (!(in)) {											\
		if (_in) { _in = 0; _time = (time) + (delay); }		\
		if (_time < (time)) (out) = 0;						\
		else (out) = 1;										\
	}														\
	else { _in = 1; (out) = 1; }							\
} while (0)

/* delay a transition for the specified delay (filtering it if it is too short) */
#define C_sig_delay(delay, time, in, out) do {				\
	static char          _in, _out = 2;						\
	static unsigned long _time;								\
	char				 _nin = (in);						\
	if (_out == 2) _out = _nin;								\
	if (_out != _nin) {										\
		if (_nin != _in) _time = (time) + (delay);			\
		if (_time < (time)) _out = _nin;					\
	}														\
	_in = _nin;												\
	(out) = _out;											\
} while (0)


/*---------------------------------------------------------------------------------------------*/
/* Miscellanea */

#define c_int_roundup(x,y) (x + ((y - (x % y)) % y))


/*=============================================================================================*/
/* Prototypes */

int c_ushort_lt(const void *a, const void *b);
int c_ushort_eq(const void *a, const void *b);

int c_ulong_lt(const void *a, const void *b);
int c_ulong_eq(const void *a, const void *b);

void c_swap(void *a, void *b, int size);
void c_sort(void *array, int el_size, int el_num, OperatorLT lt);
int c_find(const void *el, const void *array, int el_size, int el_num, OperatorLT eq);

int c_ring_free_size(CRingDesc *desc);
int c_ring_free_size_cont(CRingDesc *desc);
int c_ring_ready_size(CRingDesc *desc);
int c_ring_ready_size_cont(CRingDesc *desc);
int c_ring_put(CRingDesc *desc, const char *buf, int req_len);
int c_ring_get(CRingDesc *desc, char *buf, int req_len);

const char *c_fetch_message(const CMessageDef *messages, int code, char *storage,
							const char *unknown_rule);


/*=============================================================================================*/

#endif

