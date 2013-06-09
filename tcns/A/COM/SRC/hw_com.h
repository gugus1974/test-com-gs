/*=============================================================================================*/
/* Hardware definitions specific for the COM board                                             */
/* Header file (hw_com.h)                                                                      */
/*                                                                                             */
/* Version 1.0                                                                                 */
/*                                                                                             */
/* (c)1997 Ansaldo Trasporti                                                                   */
/* Written by Giuseppe Carnevale                                                               */
/*                                                                                             */
/* Version history:                                                                            */
/*   1.0 (xx/xx/xx) : first release                                                            */
/*=============================================================================================*/

#ifndef _HW_COM_
#define _HW_COM_


/*=============================================================================================*/
/* Includes */

#include <cpu.h>
#include "hw_360.h"


/*=============================================================================================*/
/* Constants */

#define IO_PORT_BASE		0xB00000	/* base address for the I/O ports */

#define RISC_TIMERS_TICK_US	512			/* value for the RISC timers tick (MUST BE 512!) */

/* assigned interrupt levels */
#define CPM_ILVL	6
#define MVBC_0_ILVL	5
#define MVBC_1_ILVL	4


/*---------------------------------------------------------------------------------------------*/
/* Digital Inputs */

#define PORTC_RISMC1	(1<<0)		/* reserved                           */
#define PORTC_TLD2_		(1<<1)		/* WTB trusted line disturbed (2)     */
#define PORTC_TLD1_		(1<<2)		/* WTB trusted line disturbed (1)     */
#define PORTC_RISMC2	(1<<3)		/* reserved                           */
#define PORTC_CTS1M		(1<<4)
#define PORTC_CD1M		(1<<5)
#define PORTC_LACO		(1<<6)
#define PORTC_LBCO		(1<<7)
#define PORTC_CTS1		(1<<8)
#define PORTC_CS1		(1<<9)
#define PORTC_CTS2		(1<<10)
#define PORTC_CS2		(1<<11)

#define DIN0_IND1		(1<<0)
#define DIN0_IND2		(1<<1)
#define DIN0_IND3		(1<<2)
#define DIN0_IND4		(1<<3)
#define DIN0_IND5		(1<<4)
#define DIN0_IND6		(1<<5)
#define DIN0_IND7		(1<<6)
#define DIN0_IND8		(1<<7)
#define DIN0_IND9		(1<<8)
#define DIN0_IND10		(1<<9)
#define DIN0_IND11		(1<<10)
#define DIN0_IND12		(1<<11)
#define DIN0_IND13		(1<<12)
#define DIN0_IND14		(1<<13)
#define DIN0_IND15		(1<<14)
#define DIN0_IND16		(1<<15)
#define DIN0_IND17		(1<<16)
#define DIN0_IND18		(1<<17)
#define DIN0_IND19		(1<<18)
#define DIN0_IND20		(1<<19)
#define DIN0_IND21		(1<<20)
#define DIN0_IND22		(1<<21)
#define DIN0_IND23		(1<<22)
#define DIN0_DSR1M		(1<<23)
#define DIN0_RI1M		(1<<24)
#define DIN0_HWFLTB_	(1<<25)		/* WTB line B hardware fault          */
#define DIN0_HWFLTA_	(1<<26)		/* WTB line A hardware fault          */
#define DIN0_SHD_		(1<<27)		/* MVB shutdown                       */
#define DIN0_SW2_1		(1<<28)		/* Pin 1 of switch SW2                */
#define DIN0_SW2_2		(1<<29)		/* Pin 2 of switch SW2                */
#define DIN0_SW2_4		(1<<30)		/* Pin 4 of switch SW2                */
#define DIN0_SW2_8		(1<<31)		/* Pin 8 of switch SW2                */


/*---------------------------------------------------------------------------------------------*/
/* Digital Outputs */

#define DOP0_OUTD1		(1<<0)
#define DOP0_OUTD2		(1<<1)
#define DOP0_OUTD3		(1<<2)
#define DOP0_OUTD4		(1<<3)
#define DOP0_OUTD5		(1<<4)
#define DOP0_OUTD6		(1<<5)
#define DOP0_OUTD7		(1<<6)
#define DOP0_OUTD8		(1<<7)
#define DOP0_FRLOAD1_	(1<<8)		/* WTB fritting load on A1            */
#define DOP0_FRLOAD2	(1<<9)		/* WTB fritting load on A2            */
#define DOP0_OUTD11		(1<<10)
#define DOP0_OUTD12		(1<<11)
#define DOP0_DTR1M		(1<<12)
#define DOP0_RTS4S		(1<<13)
#define DOP0_KMB		(1<<14)		/* MVB line B attachment              */
#define DOP0_KMA		(1<<15)		/* MVB line A attachment              */
#define DOP0_RISB		(1<<16)
#define DOP0_RISA		(1<<17)
#define DOP0_KFB_		(1<<18)		/* WTB line B fritting (GFISOB_)      */
#define DOP0_KFA_		(1<<19)		/* WTB line A fritting (GFISOA_)      */
#define DOP0_K4B_		(1<<20)		/* WTB line B attachment (2)          */
#define DOP0_K4A_		(1<<21)		/* WTB line A attachment (2)          */
#define DOP0_K3B_		(1<<22)		/* WTB line B attachment (1)          */
#define DOP0_K3A_		(1<<23)		/* WTB line A attachment (1)          */
#define DOP0_K1B_		(1<<24)		/* WTB line B bus switch              */
#define DOP0_K1A_		(1<<25)		/* WTB line A bus switch              */
#define DOP0_RESETB_	(1<<26)		/* WTB line B reset                   */
#define DOP0_RESETA_	(1<<27)		/* WTB line A reset                   */
#define DOP0_SEL2A		(1<<28)		/* WTB direction 1 line A or B select */
#define DOP0_SEL1A		(1<<29)		/* WTB direction 1 line A or B select */
#define DOP0_GFONA_		(1<<30)		/* WTB fritting on line B (PS1A_)     */
#define DOP0_GFONB_		(1<<31)		/* WTB fritting on line A (PS1B_)     */

#define DOP1_OUTD13		(1<<0)
#define DOP1_OUTD14		(1<<1)
#define DOP1_OUTD15		(1<<2)
#define DOP1_OUTD16		(1<<3)
#define DOP1_OUTD17		(1<<4)
#define DOP1_OUTD18		(1<<5)
#define DOP1_OUTD19		(1<<6)		/* reserved for debugging: MVBC interrupt 0 */
#define DOP1_OUTD20		(1<<7)		/* reserved for debugging: MVBC interrupt 1 */
#define DOP1_LD6b_		(1<<8)
#define DOP1_LD6a_		(1<<9)		/* MVB Bus Administrator */
#define DOP1_LD5b_		(1<<10)		/* MVB Line B failure    */
#define DOP1_LD5a_		(1<<11)		/* MVB Line A failure    */
#define DOP1_LD4b_		(1<<12)		/* Board failure         */
#define DOP1_LD4a_		(1<<13)		/* Train speed > 5 km/h  */
#define DOP1_NC			(1<<14)
#define DOP1_SLEEPM_	(1<<15)		/* enables the sleep mode */


/*=============================================================================================*/
/* Typedefs */

typedef unsigned short Cpu_Sr;				/* CPU Status Register type */


/*=============================================================================================*/
/* Globals */

extern volatile QUICC	*quicc;				/* pointer to the QUICC DP-RAM structure */
extern unsigned long	out_port_shadow[2];	/* output ports shadow                   */
extern unsigned long	out_port_xor[2];	/* output ports XOR mask                 */


/*=============================================================================================*/
/* Macros */

/* return the current ILVL value */
#define get_ilvl() asm(short, "	move.w	SR,D0", "	and.w	#$0700,D0", "	lsr.w	#8,D0")

/* fast interrupt switching on and off */
#define int_disable(sr) \
	asm(void, "	move.w	SR,D0", "	or.w	#$0700,SR", "	move.w	D0,`" #sr "`")
#define int_enable(sr) asm(void, "	move.w	`" #sr "`,SR")

/* I/O ports access */
#define IO_PORT ((unsigned long*)(IO_PORT_BASE))
#define set_out_port_xor(port, mask, xor_value) if (1) {									\
	Cpu_Sr	sr = 0;																			\
	int_disable(sr);																		\
	out_port_xor[port] = (out_port_xor[port] & ~(mask)) | (xor_value);						\
	int_enable(sr);																			\
} else
#define set_out_port(port, mask, value) if (1) {											\
	Cpu_Sr	sr = 0;																			\
	int_disable(sr);																		\
	IO_PORT[port] = (out_port_shadow[port] = (out_port_shadow[port] & ~(mask)) | (value)) ^ \
					out_port_xor[port];														\
	int_enable(sr);																			\
} else
#define get_out_port(port) (out_port_shadow[port])
#define get_in_port(port) (IO_PORT[port])

/* LED handling */
#define led_user1(status) set_out_port(1, DOP1_LD4a_, (status) ? 0 : DOP1_LD4a_)
#define led_user2(status) set_out_port(1, DOP1_LD4b_, (status) ? 0 : DOP1_LD4b_)
#define led_user3(status) set_out_port(1, DOP1_LD5a_, (status) ? 0 : DOP1_LD5a_)
#define led_user4(status) set_out_port(1, DOP1_LD5b_, (status) ? 0 : DOP1_LD5b_)
#define led_user5(status) set_out_port(1, DOP1_LD6a_, (status) ? 0 : DOP1_LD6a_)
#define led_user6(status) set_out_port(1, DOP1_LD6b_, (status) ? 0 : DOP1_LD6b_)


/*=============================================================================================*/
/* Prototypes */

void *alloc_quicc_dp_ram(int size);
void install_isr(int vector, void *isr_address);


/*=============================================================================================*/

#endif

