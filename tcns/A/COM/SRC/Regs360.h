/***********************************
 * registers.h,v 1.5 1994/06/13 17:21:13 alexr Exp
 ***********************************
 *
 ***************************************
 * Definitions of the QUICC registers
 ***************************************
 */

#ifndef __REGS360_H
#define __REGS360_H

/*****************************************************************
        Command Register
*****************************************************************/

/* bit fields within command register */
#define SOFTWARE_RESET  0x8000
#define CMD_OPCODE      0x0f00
#define CMD_CHANNEL     0x00f0
#define CMD_FLAG        0x0001

/* general command opcodes */
#define INIT_RXTX_PARAMS        0x0000
#define INIT_RX_PARAMS          0x0100
#define INIT_TX_PARAMS          0x0200
#define ENTER_HUNT_MODE         0x0300
#define STOP_TX                 0x0400
#define GR_STOP_TX              0x0500
#define RESTART_TX              0x0600
#define CLOSE_RX_BD             0x0700
#define SET_ENET_GROUP          0x0800
#define RESET_ENET_GROUP        0x0900

/* Timer commands */
#define SET_TIMER               0x0800


/*
 * General SCC mode register (GSMR)
 */

#define MODE_HDLC               0x0
#define MODE_APPLE_TALK         0x2
#define MODE_SS7                0x3
#define MODE_UART               0x4
#define MODE_PROFIBUS           0x5
#define MODE_ASYNC_HDLC         0x6
#define MODE_V14                0x7
#define MODE_BISYNC             0x8
#define MODE_DDCMP              0x9
#define MODE_ETHERNET           0xc

#define DIAG_NORMAL             0x0
#define DIAG_LOCAL_LPB          0x1
#define DIAG_AUTO_ECHO          0x2
#define DIAG_LBP_ECHO           0x3

/* For RENC and TENC fields in GSMR */
#define ENC_NRZ                 0x0
#define ENC_NRZI                0x1
#define ENC_FM0                 0x2
#define ENC_MANCH               0x4
#define ENC_DIFF_MANC           0x6

/* For TDCR and RDCR fields in GSMR */
#define CLOCK_RATE_1            0x0
#define CLOCK_RATE_8            0x1
#define CLOCK_RATE_16           0x2
#define CLOCK_RATE_32           0x3

#define TPP_00                  0x0
#define TPP_10                  0x1
#define TPP_01                  0x2
#define TPP_11                  0x3

#define TPL_NO                  0x0
#define TPL_8                   0x1
#define TPL_16                  0x2
#define TPL_32                  0x3
#define TPL_48                  0x4
#define TPL_64                  0x5
#define TPL_128                 0x6

#define TSNC_INFINITE           0x0
#define TSNC_14_65              0x1
#define TSNC_4_15               0x2
#define TSNC_3_1                0x3

#define EDGE_BOTH               0x0
#define EDGE_POS                0x1
#define EDGE_NEG                0x2
#define EDGE_NO                 0x3

#define SYNL_NO                 0x0
#define SYNL_4                  0x1
#define SYNL_8                  0x2
#define SYNL_16                 0x3

#define TCRC_CCITT16            0x0
#define TCRC_CRC16              0x1
#define TCRC_CCITT32            0x2


/*****************************************************************
        TODR (Transmit on demand) Register
*****************************************************************/
#define TODR_TOD        0x8000  /* Transmit on demand */

/*****************************************************************
        interrupt registers
*****************************************************************/

#define INTR_PIO_PC0    0x80000000      /* parallel i/o C bit 0 */
#define INTR_SCC1       0x40000000      /* SCC port 1 */
#define INTR_SCC2       0x20000000      /* SCC port 2 */
#define INTR_SCC3       0x10000000      /* SCC port 3 */
#define INTR_SCC4       0x08000000      /* SCC port 4 */
#define INTR_PIO_PC1    0x04000000      /* parallel i/o C bit 1 */
#define INTR_TIMER1     0x02000000      /* timer 1 */
#define INTR_PIO_PC2    0x01000000      /* parallel i/o C bit 2 */
#define INTR_PIO_PC3    0x00800000      /* parallel i/o C bit 3 */
#define INTR_SDMA_BERR  0x00400000      /* SDMA channel bus error */
#define INTR_DMA1       0x00200000      /* idma 1 */
#define INTR_DMA2       0x00100000      /* idma 2 */
#define INTR_TIMER2     0x00040000      /* timer 2 */
#define INTR_CP_TIMER   0x00020000      /* CP timer */
#define INTR_PIP_STATUS 0x00010000      /* PIP status */
#define INTR_PIO_PC4    0x00008000      /* parallel i/o C bit 4 */
#define INTR_PIO_PC5    0x00004000      /* parallel i/o C bit 5 */
#define INTR_TIMER3     0x00001000      /* timer 3 */
#define INTR_PIO_PC6    0x00000800      /* parallel i/o C bit 6 */
#define INTR_PIO_PC7    0x00000400      /* parallel i/o C bit 7 */
#define INTR_PIO_PC8    0x00000200      /* parallel i/o C bit 8 */
#define INTR_TIMER4     0x00000080      /* timer 4 */
#define INTR_PIO_PC9    0x00000040      /* parallel i/o C bit 9 */
#define INTR_SCP        0x00000020      /* SCP */
#define INTR_SMC1       0x00000010      /* SMC 1 */
#define INTR_SMC2       0x00000008      /* SMC 2 */
#define INTR_PIO_PC10   0x00000004      /* parallel i/o C bit 10 */
#define INTR_PIO_PC11   0x00000002      /* parallel i/o C bit 11 */
#define INTR_ERR        0x00000001      /* error */



/*****************************************************************
        chip select option register
*****************************************************************/
#define DTACK           0xe000
#define ADR_MASK        0x1ffc
#define RW_MASK         0x0002
#define FC_MASK         0x0001

/*****************************************************************
        tbase and rbase registers
*****************************************************************/
#define TBD_ADDR(quicc,pram) ((struct quicc_bd *) \
    (quicc->udata_bd_ucode + pram->tbase))
#define RBD_ADDR(quicc,pram) ((struct quicc_bd *) \
    (quicc->udata_bd_ucode + pram->rbase))
#define TBD_CUR_ADDR(quicc,pram) ((struct quicc_bd *) \
    (quicc->udata_bd_ucode + pram->tbptr))
#define RBD_CUR_ADDR(quicc,pram) ((struct quicc_bd *) \
    (quicc->udata_bd_ucode + pram->rbptr))
#define TBD_SET_CUR_ADDR(bd,quicc,pram) pram->tbptr = \
    ((unsigned short)((char *)(bd) - (char *)(quicc->udata_bd_ucode)))
#define RBD_SET_CUR_ADDR(bd,quicc,pram) pram->rbptr = \
    ((unsigned short)((char *)(bd) - (char *)(quicc->udata_bd_ucode)))
#define INCREASE_TBD(bd,quicc,pram) {  \
    if((bd)->status & T_W)             \
        (bd) = TBD_ADDR(quicc,pram);   \
    else                               \
        (bd)++;                        \
}
#define DECREASE_TBD(bd,quicc,pram) {  \
    if ((bd) == TBD_ADDR(quicc, pram)) \
        while ((bd)->status & T_W)     \
            (bd)++;                    \
    else                               \
        (bd)--;                        \
}
#define INCREASE_RBD(bd,quicc,pram) {  \
    if((bd)->status & R_W)             \
        (bd) = RBD_ADDR(quicc,pram);   \
    else                               \
        (bd)++;                        \
}
#define DECREASE_RBD(bd,quicc,pram) {  \
    if ((bd) == RBD_ADDR(quicc, pram)) \
        while ((bd)->status & T_W)     \
            (bd)++;                    \
    else                               \
        (bd)--;                        \
}

#endif
