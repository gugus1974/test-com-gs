/*=============================================================================================*/
/* Hardware definitions specific for the 68360                                                 */
/* Header file (hw_360.h)                                                                      */
/*                                                                                             */
/* Version 1.0                                                                                 */
/*                                                                                             */
/* (c)1997 Ansaldo Trasporti                                                                   */
/* Written by Giuseppe Carnevale                                                               */
/*                                                                                             */
/* Version history:                                                                            */
/*   1.0 (xx/xx/xx) : first release                                                            */
/*=============================================================================================*/

#ifndef _HW_360_
#define _HW_360_


/*=============================================================================================*/
/* 68360 definitions */

#include "quicc360.h"

/*---------------------------------------------------------------------------------------------*/
/* CPM Interrupt Controller defines */

#define CPIC_VEC_PC0		0x1F
#define CPIC_VEC_SCC1		0x1E
#define CPIC_VEC_SCC2		0x1D
#define CPIC_VEC_SCC3		0x1C
#define CPIC_VEC_SCC4		0x1B
#define CPIC_VEC_PC1		0x1A
#define CPIC_VEC_TIMER1		0x19
#define CPIC_VEC_PC2		0x18
#define CPIC_VEC_PC3		0x17
#define CPIC_VEC_SDMA_BERR	0x16
#define CPIC_VEC_IDMA1		0x15
#define CPIC_VEC_IDMA2		0x14
#define CPIC_VEC_TIMER2		0x12
#define CPIC_VEC_RISC_TIMER	0x11
#define CPIC_VEC_PC4		0x0F
#define CPIC_VEC_PC5		0x0E
#define CPIC_VEC_TIMER3		0x0C
#define CPIC_VEC_PC6		0x0B
#define CPIC_VEC_PC7		0x0A
#define CPIC_VEC_PC8		0x09
#define CPIC_VEC_TIMER4		0x07
#define CPIC_VEC_PC9		0x06
#define CPIC_VEC_SPI		0x05
#define CPIC_VEC_SMC1		0x04
#define CPIC_VEC_SMC2_PIP	0x03
#define CPIC_VEC_PC10		0x02
#define CPIC_VEC_PC11		0x01
#define CPIC_VEC_ERROR		0x00


/*---------------------------------------------------------------------------------------------*/
/* CICR register defines */

#define CICR_SCCd_SCC1     0x000000    /* assert SCC1 to SCCd */
#define CICR_SCCd_SCC2     0x400000    /* assert SCC2 to SCCd */
#define CICR_SCCd_SCC3     0x800000    /* assert SCC3 to SCCd */
#define CICR_SCCd_SCC4     0xc00000    /* assert SCC4 to SCCd */

#define CICR_SCCc_SCC1     0x000000    /* assert SCC1 to SCCc */
#define CICR_SCCc_SCC2     0x100000    /* assert SCC2 to SCCc */
#define CICR_SCCc_SCC3     0x200000    /* assert SCC3 to SCCc */
#define CICR_SCCc_SCC4     0x300000    /* assert SCC4 to SCCc */

#define CICR_SCCb_SCC1     0x000000    /* assert SCC1 to SCCb */
#define CICR_SCCb_SCC2     0x040000    /* assert SCC2 to SCCb */
#define CICR_SCCb_SCC3     0x080000    /* assert SCC3 to SCCb */
#define CICR_SCCb_SCC4     0x0c0000    /* assert SCC4 to SCCb */

#define CICR_SCCa_SCC1     0x000000    /* assert SCC1 to SCCa */
#define CICR_SCCa_SCC2     0x010000    /* assert SCC2 to SCCa */
#define CICR_SCCa_SCC3     0x020000    /* assert SCC3 to SCCa */
#define CICR_SCCa_SCC4     0x030000    /* assert SCC4 to SCCa */

#define CICR_IRQ(x)        ((x) << 13) /* irq level x */

#define CICR_HP(x)         ((x) << 8)  /* hp x */

#define CICR_VB(x)         ((x) << 5)  /* vector base x */

#define CICR_SPS           0x000001    /* spread priority */


/*---------------------------------------------------------------------------------------------*/
/* CPM BD's flags */

#define CPM_BD_E  0x8000
#define CPM_BD_R  0x8000
#define CPM_BD_W  0x2000
#define CPM_BD_I  0x1000
#define CPM_BD_L  0x0800
#define CPM_BD_F  0x0400
#define CPM_BD_TC 0x0400
#define CPM_BD_CM 0x0200
#define CPM_BD_P  0x0100
#define CPM_BD_DE 0x0080
#define CPM_BD_LG 0x0020
#define CPM_BD_NO 0x0010
#define CPM_BD_AB 0x0008
#define CPM_BD_CR 0x0004
#define CPM_BD_OV 0x0002
#define CPM_BD_UN 0x0002
#define CPM_BD_CD 0x0001
#define CPM_BD_CT 0x0001


/*---------------------------------------------------------------------------------------------*/
/* SxC Event and Mask Register bits */

#define CPM_E_RX	(1 << 0)
#define CPM_E_TX	(1 << 1)
#define CPM_E_BSY	(1 << 2)
#define CPM_E_CCR	(1 << 3)
#define CPM_E_BRK	(1 << 4)
#define CPM_E_BRKS	(1 << 5)
#define CPM_E_BRKE	(1 << 6)
#define CPM_E_GRA	(1 << 7)
#define CPM_E_IDL	(1 << 8)
#define CPM_E_AB	(1 << 9)
#define CPM_E_GLT	(1 << 11)
#define CPM_E_GLR	(1 << 12)

#define CPM_E_RXB	(1 << 0)
#define CPM_E_TXB	(1 << 1)
#define CPM_E_RXF	(1 << 3)
#define CPM_E_TXE	(1 << 4)
#define CPM_E_FLG	(1 << 9)
#define CPM_E_DCC	(1 << 10)


/*---------------------------------------------------------------------------------------------*/
/* SCC Status Register bits */

#define CPM_S_ID	(1 << 0)
#define CPM_S_CS	(1 << 1)
#define CPM_S_FG	(1 << 2)


/*---------------------------------------------------------------------------------------------*/
/* CPM commands */

#define CPM_CMD_INIT_RXTX_PARAMS	0
#define CPM_CMD_INIT_RX_PARAMS		1
#define CPM_CMD_INIT_TX_PARAMS		2
#define CPM_CMD_ENTER_HUNT_MODE		3
#define CPM_CMD_STOP_TX				4
#define CPM_CMD_GR_STOP_TX			5
#define CPM_CMD_INIT_IDMA			5
#define CPM_CMD_RESTART_TX			6
#define CPM_CMD_CLOSE_RX_BD			7
#define CPM_CMD_SET_GROUP_ADDR		8
#define CPM_CMD_SET_TIMER			8
#define CPM_CMD_RESET_BCS			10


/*---------------------------------------------------------------------------------------------*/
/* HDLC receive Buffer Descriptor status flags */
#define HDLC_RX_BD_E  0x8000
#define HDLC_RX_BD_W  0x2000
#define HDLC_RX_BD_I  0x1000
#define HDLC_RX_BD_L  0x0800
#define HDLC_RX_BD_F  0x0400
#define HDLC_RX_BD_CM 0x0200
#define HDLC_RX_BD_DE 0x0080
#define HDLC_RX_BD_LG 0x0020
#define HDLC_RX_BD_NO 0x0010
#define HDLC_RX_BD_AB 0x0008
#define HDLC_RX_BD_CR 0x0004
#define HDLC_RX_BD_OV 0x0002
#define HDLC_RX_BD_CD 0x0001

/* HDLC transmit Buffer Descriptor status flags */
#define HDLC_TX_BD_R  0x8000
#define HDLC_TX_BD_W  0x2000
#define HDLC_TX_BD_I  0x1000
#define HDLC_TX_BD_L  0x0800
#define HDLC_TX_BD_TC 0x0400
#define HDLC_TX_BD_CM 0x0200
#define HDLC_TX_BD_UN 0x0002
#define HDLC_TX_BD_CT 0x0001

/* HDLC event bits */
#define HDLC_EVENT_GLR 0x1000
#define HDLC_EVENT_GLT 0x0800
#define HDLC_EVENT_DCC 0x0400
#define HDLC_EVENT_FLG 0x0200
#define HDLC_EVENT_IDL 0x0100
#define HDLC_EVENT_GRA 0x0080
#define HDLC_EVENT_TXE 0x0010
#define HDLC_EVENT_RXF 0x0008
#define HDLC_EVENT_BSY 0x0004
#define HDLC_EVENT_TXB 0x0002
#define HDLC_EVENT_RXB 0x0001


/*=============================================================================================*/

#endif

