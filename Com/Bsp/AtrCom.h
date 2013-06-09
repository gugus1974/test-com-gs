/*=====================================================================================*/
/* COM board BSP configuration                                                         */
/* Header file (atrcom.h)						          							   */
/* 																					   */
/* Version 1.0																		   */
/*																					   */
/* (c)1997 Ansaldo Trasporti														   */
/* Written by Giuseppe Carnevale													   */
/*=====================================================================================*/

/*=====================================================================================*/
/* These flags specify the options we want. It must be consistent with "atrcom.def"    */

#define SERIAL_DEVICE                   0
#define SERIAL_INTERRUPT                0
#define TIMER_DEVICE                    0
#define XDM                             1


/*=====================================================================================*/
/* What follows is the file "mo360ads.h" for the mo360ads board modified for our needs */


/***********************************************************************
*
*               Header File for the QUICC ( MC68360 )
*                                                           
***********************************************************************
*/

#if 0
INIT    0x0402000        /* QUADS Board DRAM User Space*/
#endif

/****  MASTER QUICC ****/

/*************************** DUAL PORT RAM ****************************** */

#define MASTER_MBAR    0x03FF00 /* Module Base Address, (MBAR)=REGB */


#define SLAVE_MBAR   0x03FF04 	/* Module Base Address,in SLAVE mode */
#define MBARE   0x03FF08 	/* Module Base Address Enable SLAVE */

#define MO360ADS_DPRBASE 0x00022000       /* Base address of dual-port RAM */

#if 0
#define MO360ADS_REGB    0x100   /* Base address of Internal Registers */
#define MO360ADS_SCC1PB  0xC00   /* SCC1 Protocol Base address */
#define MO360ADS_SCC2PB  0xD00   /* SCC2 Protocol Base address */
#define MO360ADS_SCC3PB  0xE00   /* SCC3 Protocol Base address */
#define MO360ADS_SCC4PB  0xF00   /* SCC4 Protocol Base address */
#define MO360ADS_SPIPB   0xD80   /* SPI Protocol Base */
#define MO360ADS_SMC1PB  0xE80   /* SMC1 Protocol Base */
#define MO360ADS_SMC2PB  0xF80   /* SMC2 Protocol Base */
#define MO360ADS_IDMA1PB 0xE70   /* IDMA1 Protocol Base */
#define MO360ADS_IDMA2PB 0xF70   /* IDMA2 Protocol Base */
#endif

#define MO360ADS_REGB    0x23000   /* Base address of Internal Registers */
#define MO360ADS_SCC1PB  0x22C00   /* SCC1 Protocol Base address */
#define MO360ADS_SCC2PB  0x22D00   /* SCC2 Protocol Base address */
#define MO360ADS_SCC3PB  0x22E00   /* SCC3 Protocol Base address */
#define MO360ADS_SCC4PB  0x22F00   /* SCC4 Protocol Base address */
#define MO360ADS_SPIPB   0x22D80   /* SPI Protocol Base */
#define MO360ADS_SMC1PB  0x22E80   /* SMC1 Protocol Base */
#define MO360ADS_SMC2PB  0x22F80   /* SMC2 Protocol Base */
#define MO360ADS_IDMA1PB 0x22E70   /* IDMA1 Protocol Base */
#define MO360ADS_IDMA2PB 0x22F70   /* IDMA2 Protocol Base */


/************************ SIM Module registers **************************/
#define MO360ADS_MCR     0x0000      /* Module Configuration */
#define MO360ADS_AVR     0x0008      /* Auto Vector */
#define MO360ADS_RSR     0x0009      /* Reset Status */
#define MO360ADS_CLKOCR  0x000C      /* CLKO Control */
#define MO360ADS_PPLCR   0x0010      /* PPL Control */
#define MO360ADS_CDVCR   0x0014      /* Clock Divider Control */

#define MO360ADS_PEPAR   0x0016      /* Port E Pin Assignment */

#define MO360ADS_SYPCR   0x0022      /* System Protection Control */
#define MO360ADS_SWIV    0x0023      /* S/w Interrupt Vector */
#define MO360ADS_PICR    0x0026      /* Periodic Interrupt Control */
#define MO360ADS_PITR    0x002A      /* Periodic Interrupt Timing */
#define MO360ADS_SWSR    0x002F      /* S/w Service */
#define MO360ADS_BKAR    0x0032      /* Breakpoint Address */
#define MO360ADS_BKCQ    0x0034      /* Breakpoint Control */

#define MO360ADS_GMR     0x0040      /* Global Memory */
#define MO360ADS_MSTAT   0x0044      /* MEMC Status */

#define MO360ADS_BR0     0x0050      /* Base Register 0 */
#define MO360ADS_OR0     0x0054      /* Option Register 0 */
#define MO360ADS_BR1     0x0060      /* Base Register 1 */
#define MO360ADS_OR1     0x0064      /* Option Register 1 */
#define MO360ADS_BR2     0x0070      /* Base Register 2 */
#define MO360ADS_OR2     0x0074      /* Option Register 2 */
#define MO360ADS_BR3     0x0080      /* Base Register 3 */
#define MO360ADS_OR3     0x0084      /* Option Register 3 */
#define MO360ADS_BR4     0x0090      /* Base Register 4 */
#define MO360ADS_OR4     0x0094      /* Option Register 4 */
#define MO360ADS_BR5     0x00A0      /* Base Register 5 */
#define MO360ADS_OR5     0x00A4      /* Option Register 5 */
#define MO360ADS_BR6     0x00B0      /* Base Register 6 */
#define MO360ADS_OR6     0x00B4      /* Option Register 6 */
#define MO360ADS_BR7     0x00C0      /* Base Register 7 */
#define MO360ADS_OR7     0x00C4      /* Option Register 7 */

#define MO360ADS_TSTMRA  0x00F0      /* Test Module Master Shift A */
#define MO360ADS_TSTMRB  0x00F2      /* Test Module Master Shift B */
#define MO360ADS_TSTSC   0x00F4      /* Test Module Shift Count */
#define MO360ADS_TSTRC   0x00F6      /* Test Module Reprtition Counter */
#define MO360ADS_CREG    0x00F8      /* Test Module Control */
#define MO360ADS_DREG    0x00FA      /* Test Module Distributed */
/* *********************** CPM Module registers ******************* */

#define MO360ADS_ICCR    0x0500      /* Channel Configuration */
/* *** IDMA1, 2 */
#define MO360ADS_CMR1    0x0504      /* IDMA1 Mode */
#define MO360ADS_SAPR1   0x0508      /* IDMA1 Source Address Pointer */
#define MO360ADS_DAPR1   0x050C      /* IDMA1 destination Address Pointer */
#define MO360ADS_BCR1    0x0510      /* IDMA1 Byte Count */
#define MO360ADS_FCR1    0x0514      /* IDMA1 Fuction Code */
#define MO360ADS_CMAR1   0x0516      /* IDMA1 Channel Mask */
#define MO360ADS_CSR1    0x0518      /* IDMA1 Channel Status */

#define MO360ADS_CMR2    0x0526      /* IDMA2 Mode */
#define MO360ADS_SAPR2   0x0528      /* IDMA2 Source Address Pointer */
#define MO360ADS_DAPR2   0x052C      /* IDMA2 destination Address Pointer */
#define MO360ADS_BCR2    0x0530      /* IDMA2 Byte Count */
#define MO360ADS_FCR2    0x0534      /* IDMA2 Fuction Code */
#define MO360ADS_CMAR2   0x0536      /* IDMA2 Channel Mask */
#define MO360ADS_CSR2    0x0538      /* IDMA2 Channel Status */

/* *** SDMA */

#define MO360ADS_SDSR    0x051C      /* SDMA Status */
#define MO360ADS_SDCR    0x051E      /* SDMA Configuration Register */
#define MO360ADS_SDAR    0x0520      /* SDMA Address Register */

/* *** CP Interrupt */

#define MO360ADS_CICR    0x0540      /* CP Interrupt Configuration */
#define MO360ADS_CIPR    0x0544      /* CP Interrupt Pending */
#define MO360ADS_CIMR    0x0548      /* CP Interrupt Mask */
#define MO360ADS_CISR    0x054C      /* CP Interrupt In-service */

/* *** Port A, B, C */

#define MO360ADS_PADIR   0x0550      /* Port A Data Direction Assignment */
#define MO360ADS_PAPAR   0x0552      /* Port A Pin Function Assignment */
#define MO360ADS_PAODR   0x0554      /* Port A Open Drain Assignment */
#define MO360ADS_PADAT   0x0556      /* Port A Data Content */

#define MO360ADS_PBDIR   0x06B8      /* Port B Data Direction Assignment */
#define MO360ADS_PBPAR   0x06BC      /* Port B Pin Function Assignment */
#define MO360ADS_PBODR   0x06C0      /* Port B Open Drain Assignment */
#define MO360ADS_PBDAT   0x06C4      /* Port B Data Content */

#define MO360ADS_PCDIR   0x0560      /* Port C Data Direction Assignment */
#define MO360ADS_PCPAR   0x0562      /* Port C Pin Assignment */
#define MO360ADS_PCSO    0x0564      /* Port C Special Options */
#define MO360ADS_PCDAT   0x0566      /* Port C Data Content */
#define MO360ADS_PCINT   0x0568      /* Port C Interrupt Control */

/* **** Timer 1, 2, 3, 4 */

#define MO360ADS_TGCR    0x0580      /* Timer Global Configuration */

#define MO360ADS_TMR1    0x0590      /* Timer1 Mode */
#define MO360ADS_TMR2    0x0592      /* Timer2 Mode */
#define MO360ADS_TRR1    0x0594      /* Timer1 Reference */
#define MO360ADS_TRR2    0x0596      /* Timer2 Reference */
#define MO360ADS_TCR1    0x0598      /* Timer1 Capture */
#define MO360ADS_TCR2    0x059A      /* Timer2 Capture */
#define MO360ADS_TCN1    0x059C      /* Timer1 Counter */
#define MO360ADS_TCN2    0x059E      /* Timer2 Counter */

#define MO360ADS_TMR3    0x05A0      /* Timer3 Mode */
#define MO360ADS_TMR4    0x05A2      /* Timer4 Mode */
#define MO360ADS_TRR3    0x05A4      /* Timer3 Reference */
#define MO360ADS_TRR4    0x05A6      /* Timer4 Reference */
#define MO360ADS_TCR3    0x05A8      /* Timer3 Capture */
#define MO360ADS_TCR4    0x05AA      /* Timer4 Capture */
#define MO360ADS_TCN3    0x05AC      /* Timer3 Counter */
#define MO360ADS_TCN4    0x05AE      /* Timer4 Counter */

#define MO360ADS_TER1    0x05B0      /* Timer1 Event */
#define MO360ADS_TER2    0x05B2      /* Timer2 Event */
#define MO360ADS_TER3    0x05B4      /* Timer3 Event */
#define MO360ADS_TER4    0x05B6      /* Timer4 Event */

/* *** CP */

#define MO360ADS_CR      0x05C0      /* CP Command */
#define MO360ADS_RCCR    0x05C4      /* RISC Configuration */
#define MO360ADS_RTER    0x05D6      /* RISC Timer Event Register */
#define MO360ADS_RTMR    0x05DA      /* RISC Timer Mask Register */

/* BRG 1, 2, 3, 4 */

#define MO360ADS_BRGC1   0x05F0      /* BRG1 Configuration */
#define MO360ADS_BRGC2   0x05F4      /* BRG2 Configuration */
#define MO360ADS_BRGC3   0x05F8      /* BRG3 Configuration */
#define MO360ADS_BRGC4   0x05FC      /* BRG4 Configuration */

/* *** SCC 1, 2, 3, 4 */
/* SCC1 */
#define MO360ADS_GSMR_L1 0x0600      /* SCC1 General Mode */
#define MO360ADS_GSMR_H1 0x0604      /* SCC1 General Mode */
#define MO360ADS_PSMR1   0x0608      /* SCC1 Protocol Specific Mode */
#define MO360ADS_TODR1   0x060C      /* SCC1 Transmit On Demand */
#define MO360ADS_DSR1    0x060E      /* SCC1 Data Sync */
#define MO360ADS_SCCE1   0x0610      /* SCC1 Event */
#define MO360ADS_SCCM1   0x0614      /* SCC1 Mask */
#define MO360ADS_SCCS1   0x0617      /* SCC1 Status */

/* SCC2 */

#define MO360ADS_GSMR_L2 0x0620      /* SCC2 General Mode */
#define MO360ADS_GSMR_H2 0x0624      /* SCC2 General Mode */
#define MO360ADS_PSMR2   0x0628      /* SCC2 Protocol Specific Mode */
#define MO360ADS_TODR2   0x062C      /* SCC2 Transmit On Demand */
#define MO360ADS_DSR2    0x062E      /* SCC2 Data Sync */
#define MO360ADS_SCCE2   0x0630      /* SCC2 Event */
#define MO360ADS_SCCM2   0x0634      /* SCC2 Mask */
#define MO360ADS_SCCS2   0x0637      /* SCC2 Status */

/* SCC3 */

#define MO360ADS_GSMR_L3 0x0640      /* SCC3 General Mode */
#define MO360ADS_GSMR_H3 0x0644      /* SCC3 General Mode */
#define MO360ADS_PSMR3   0x0648      /* SCC3 Protocol Specific Mode */
#define MO360ADS_TODR3   0x064C      /* SCC3 Transmit On Demand */
#define MO360ADS_DSR3    0x064E      /* SCC3 Data Sync */
#define MO360ADS_SCCE3   0x0650      /* SCC3 Event */
#define MO360ADS_SCCM3   0x0654      /* SCC3 Mask */
#define MO360ADS_SCCS3   0x0657      /* SCC3 Status */

/* SCC4 */

#define MO360ADS_GSMR_L4 0x0660      /* SCC4 General Mode */
#define MO360ADS_GSMR_H4 0x0664      /* SCC4 General Mode */
#define MO360ADS_PSMR4   0x0668      /* SCC4 Protocol Specific Mode */
#define MO360ADS_TODR4   0x066C      /* SCC4 Transmit On Demand */
#define MO360ADS_DSR4    0x066E      /* SCC4 Data Sync */
#define MO360ADS_SCCE4   0x0670      /* SCC4 Event */
#define MO360ADS_SCCM4   0x0674      /* SCC4 Mask */
#define MO360ADS_SCCS4   0x0677      /* SCC4 Status */

/*** SMC 1, 2 */

#define MO360ADS_SMCMR1  0x0682      /* SMC1 Mode */
#define MO360ADS_SMCE1   0x0686      /* SMC1 Event */
#define MO360ADS_SMCM1   0x068A      /* SMC1 Mask */

#define MO360ADS_SMCMR2  0x0692      /* SMC2 Mode */
#define MO360ADS_SMCE2   0x0696      /* SMC2 Event */
#define MO360ADS_SMCM2   0x069A      /* SMC2 Mask */

/*** SPI */

#define MO360ADS_SPMODE  0x06A0      /* SPI Mode */
#define MO360ADS_SPIE    0x06A6      /* SPI Event */
#define MO360ADS_SPIM    0x06AA      /* SPI Mask */
#define MO360ADS_SPCOM   0x06AD      /* SPI Command */

/*** PIP */

#define MO360ADS_PIPC    0x06B2      /* PIP Configuration */
#define MO360ADS_PTRR    0x06B6      /* PIP Timing Parameter */

/*** Serial Interface */

#define MO360ADS_SIMODE  0x06E0      /* SI Mode */
#define MO360ADS_SIGMR   0x06E4      /* SI Global Mode */
#define MO360ADS_SISTR   0x06E6      /* SI Status */
#define MO360ADS_SICMR   0x06E7      /* SI Command */
#define MO360ADS_SICR    0x06EC      /* SI Clock Routing */
#define MO360ADS_SIRP    0x06F0      /* SI RAM Pointer */
#define MO360ADS_SIRAM   0x0700      /* SI Routing RAM */

/* *********************** END of SIM/CPM Register MAP ******************* */

/* *********************** SCC Parameters ******************************** */


/* ***** SCCx */

#define MO360ADS_RBASE  0x000     /* SCCx RX BD Base Address */
#define MO360ADS_TBASE  0x002     /* SCCx TX BD Base Address */
#define MO360ADS_RFCR   0x004     /* SCCx RX Function Code */
#define MO360ADS_TFCR   0x005     /* SCCx TX Function Code */
#define MO360ADS_MRBLR  0x006     /* SCCx MAX Buffer Length - RX */
#define MO360ADS_RSTATE 0x008     /* SCCx RX Internal State */
#define MO360ADS_RBPRT  0x010     /* SCCx RX BD Pointer */
#define MO360ADS_TSTATE 0x018     /* SCCx TX Internal State */
#define MO360ADS_TBPRT  0x020     /* SCCx TX BD Pointer */
#define MO360ADS_RCRC   0x028     /* SCCx Temp Receive CRC */
#define MO360ADS_TCRC   0x02C     /* SCCx Temp Transmit CRC */

/* SCCx extra regs (specific to transparent mode only) */

#define MO360ADS_CRC_C  0x030     /* SCCx CRC Constant Total-Transparent */
#define MO360ADS_CRC_P  0x034     /* SCCx CRC Preset Total-Transparent */

/* ****** SMC1/SMC2 => SMCA/SMCB, simplify assembly coding */

#define MO360ADS_RBASE  0x000     /* SMC RX BD Base Address */
#define MO360ADS_TBASE  0x002     /* SMC TX BD Base Address */
#define MO360ADS_RFCR   0x004     /* SMC RX Function Code */
#define MO360ADS_TFCR   0x005     /* SMC TX Function Code */
#define MO360ADS_MRBLR  0x006     /* SMC MAX Buffer Length - RX */
#define MO360ADS_RSTATE 0x008     /* SMC RX Internal State */
#define MO360ADS_RBPRT  0x010     /* SMC RX BD Pointer */
#define MO360ADS_TSTATE 0x018     /* SMC TX Internal State */
#define MO360ADS_TBPRT  0x020     /* SMC TX BD Pointer */


/* Port A regs  bits */

/* PAODR porta open drain register */

#define MO360ADS_PAODR_OD7    0x80      /* od7 */ 
#define MO360ADS_PAODR_OD6    0x40      /* od6 */ 
#define MO360ADS_PAODR_OD5    0x20      /* od5 */ 
#define MO360ADS_PAODR_OD4    0x10      /* od4 */ 
#define MO360ADS_PAODR_OD3    0x08      /* od3 */ 
#define MO360ADS_PAODR_OD1    0x02      /* od1 */ 


/* PADAT port A data register */

#define MO360ADS_PADAT_D15    0x8000      /* d15 */ 
#define MO360ADS_PADAT_D14    0x4000      /* d14 */ 
#define MO360ADS_PADAT_D13    0x2000      /* d13 */ 
#define MO360ADS_PADAT_D12    0x1000      /* d12 */ 
#define MO360ADS_PADAT_D11    0x0800      /* d11 */ 
#define MO360ADS_PADAT_D10    0x0400      /* d10 */ 
#define MO360ADS_PADAT_D9     0x0200      /* d9 */ 
#define MO360ADS_PADAT_D8     0x0100      /* d8 */ 
#define MO360ADS_PADAT_D7     0x0080      /* d7 */ 
#define MO360ADS_PADAT_D6     0x0040      /* d6 */ 
#define MO360ADS_PADAT_D5     0x0020      /* d5 */ 
#define MO360ADS_PADAT_D4     0x0010      /* d4 */ 
#define MO360ADS_PADAT_D3     0x0008      /* d3 */ 
#define MO360ADS_PADAT_D2     0x0004      /* d2 */ 
#define MO360ADS_PADAT_D1     0x0002      /* d1 */ 
#define MO360ADS_PADAT_D0     0x0001      /* d0 */ 


/* PADIR  port A data direction register */
#define MO360ADS_PADIR_D15    0x8000      /* d15 */
#define MO360ADS_PADIR_D14    0x4000      /* d14 */
#define MO360ADS_PADIR_D13    0x2000      /* d13 */
#define MO360ADS_PADIR_D12    0x1000      /* d12 */
#define MO360ADS_PADIR_D11    0x0800      /* d11 */
#define MO360ADS_PADIR_D10    0x0400      /* d10 */
#define MO360ADS_PADIR_D9     0x0200      /* d9 */
#define MO360ADS_PADIR_D8     0x0100      /* d8 */
#define MO360ADS_PADIR_D7     0x0080      /* d7 */
#define MO360ADS_PADIR_D6     0x0040      /* d6 */
#define MO360ADS_PADIR_D5     0x0020      /* d5 */
#define MO360ADS_PADIR_D4     0x0010      /* d4 */
#define MO360ADS_PADIR_D3     0x0008      /* d3 */
#define MO360ADS_PADIR_D2     0x0004      /* d2 */
#define MO360ADS_PADIR_D1     0x0002      /* d1 */
#define MO360ADS_PADIR_D0     0x0001      /* d0 */

/* PAPAR  port A data direction register */
#define MO360ADS_PAPAR_DD15    0x8000      /* dd15 */
#define MO360ADS_PAPAR_DD14    0x4000      /* dd14 */
#define MO360ADS_PAPAR_DD13    0x2000      /* dd13 */
#define MO360ADS_PAPAR_DD12    0x1000      /* dd12 */
#define MO360ADS_PAPAR_DD11    0x0800      /* dd11 */
#define MO360ADS_PAPAR_DD10    0x0400      /* dd10 */
#define MO360ADS_PAPAR_DD9     0x0200      /* dd9 */
#define MO360ADS_PAPAR_DD8     0x0100      /* dd8 */
#define MO360ADS_PAPAR_DD7     0x0080      /* dd7 */
#define MO360ADS_PAPAR_DD6     0x0040      /* dd6 */
#define MO360ADS_PAPAR_DD5     0x0020      /* dd5 */
#define MO360ADS_PAPAR_DD4     0x0010      /* dd4 */
#define MO360ADS_PAPAR_DD3     0x0008      /* dd3 */
#define MO360ADS_PAPAR_DD2     0x0004      /* dd2 */
#define MO360ADS_PAPAR_DD1     0x0002      /* dd1 */
#define MO360ADS_PAPAR_DD0     0x0001      /* dd0 */



/* CICR defines */
#define MO360ADS_SCCd_SCC1     0x000000    /* assert SCC1 to SCCd */
#define MO360ADS_SCCd_SCC2     0x400000    /* assert SCC2 to SCCd */
#define MO360ADS_SCCd_SCC3     0x800000    /* assert SCC3 to SCCd */
#define MO360ADS_SCCd_SCC4     0xc00000    /* assert SCC4 to SCCd */

#define MO360ADS_SCCc_SCC1     0x000000    /* assert SCC1 to SCCc */
#define MO360ADS_SCCc_SCC2     0x100000    /* assert SCC2 to SCCc */
#define MO360ADS_SCCc_SCC3     0x200000    /* assert SCC3 to SCCc */
#define MO360ADS_SCCc_SCC4     0x300000    /* assert SCC4 to SCCc */

#define MO360ADS_SCCb_SCC1     0x000000    /* assert SCC1 to SCCb */
#define MO360ADS_SCCb_SCC2     0x040000    /* assert SCC2 to SCCb */
#define MO360ADS_SCCb_SCC3     0x080000    /* assert SCC3 to SCCb */
#define MO360ADS_SCCb_SCC4     0x0c0000    /* assert SCC4 to SCCb */

#define MO360ADS_SCCa_SCC1     0x000000    /* assert SCC1 to SCCa */
#define MO360ADS_SCCa_SCC2     0x010000    /* assert SCC2 to SCCa */
#define MO360ADS_SCCa_SCC3     0x020000    /* assert SCC3 to SCCa */
#define MO360ADS_SCCa_SCC4     0x030000    /* assert SCC4 to SCCa */

#define MO360ADS_CICR_IRQ0     0x000000    /* irq level 0 */
#define MO360ADS_CICR_IRQ1     0x002000    /* irq level 1 */
#define MO360ADS_CICR_IRQ2     0x004000    /* irq level 2 */
#define MO360ADS_CICR_IRQ3     0x006000    /* irq level 3 */
#define MO360ADS_CICR_IRQ4     0x008000    /* irq level 4 */
#define MO360ADS_CICR_IRQ5     0x00a000    /* irq level 5 */
#define MO360ADS_CICR_IRQ6     0x00c000    /* irq level 6 */
#define MO360ADS_CICR_IRQ7     0x00e000    /* irq level 7 */

#define MO360ADS_CICR_HP4      0x001000    /* HP4 */
#define MO360ADS_CICR_HP3      0x000800    /* HP3 */
#define MO360ADS_CICR_HP2      0x000400    /* HP2 */
#define MO360ADS_CICR_HP1      0x000200    /* HP1 */
#define MO360ADS_CICR_HP0      0x000100    /* HP0 */




#define MO360ADS_CICR_VBA0     0x000000    /* vector base 0 */
#define MO360ADS_CICR_VBA1     0x000020    /* vector base 1 */
#define MO360ADS_CICR_VBA2     0x000040    /* vector base 2 */
#define MO360ADS_CICR_VBA3     0x000060    /* vector base 3 */
#define MO360ADS_CICR_VBA4     0x000080    /* vector base 4 */
#define MO360ADS_CICR_VBA5     0x0000a0    /* vector base 5 */
#define MO360ADS_CICR_VBA6     0x0000c0    /* vector base 6 */
#define MO360ADS_CICR_VBA7     0x0000e0    /* vector base 7 */



/* CIPR, CIMR, CISR bit defines, 32 bit register! */

#define MO360ADS_CIXX_PC0	0x80000000      /*  pc0 */
#define MO360ADS_CIXX_SCC1	0x40000000      /*  scc1 */
#define MO360ADS_CIXX_SCC2	0x20000000      /*  scc2 */
#define MO360ADS_CIXX_SCC3	0x10000000      /*  scc3 */
#define MO360ADS_CIXX_SCC4	0x08000000      /*  scc4 */
#define MO360ADS_CIXX_PC1	0x04000000      /*  pc1  */
#define MO360ADS_CIXX_TIM1	0x02000000      /*  timer1 */
#define MO360ADS_CIXX_PC2	0x01000000      /*  pc2 */
#define MO360ADS_CIXX_PC3	0x00800000      /*  pc3 */
#define MO360ADS_CIXX_SDMA	0x00400000      /*  sdma */
#define MO360ADS_CIXX_IDMA1	0x00200000      /*  idma1 */
#define MO360ADS_CIXX_IDMA2	0x00100000      /*  idma2 */
#define MO360ADS_CIXX_TIM2	0x00040000      /*  timer2 */
#define MO360ADS_CIXX_RTT	0x00020000      /*  rtt */
#define MO360ADS_CIXX_PC4	0x00008000      /*  pc4 */
#define MO360ADS_CIXX_PC5	0x00004000      /*  pc5 */
#define MO360ADS_CIXX_TIM3	0x00001000      /*  timer 3 */
#define MO360ADS_CIXX_PC6	0x00000800      /*  pc6 */
#define MO360ADS_CIXX_PC7	0x00000400      /*  pc7 */
#define MO360ADS_CIXX_PC8	0x00000200      /*  pc8 */
#define MO360ADS_CIXX_TIM4	0x00000080      /*  timer 4 */
#define MO360ADS_CIXX_PC9	0x00000040      /*  pc9 */
#define MO360ADS_CIXX_SPI	0x00000020      /*  spi */
#define MO360ADS_CIXX_SMC1	0x00000010      /*  smc1*/
#define MO360ADS_CIXX_SMC2	0x00000008      /*  scm2/pip */
#define MO360ADS_CIXX_PC10	0x00000004      /*  pc10 */
#define MO360ADS_CIXX_PC11	0x00000002      /*  pc11 */


/* command set register defines */

#define MO360ADS_CR_RST         0x8000        /* software reset */

/* cr opcodes */
#define MO360ADS_CR_INIT_RXTX   0x0000       /*init. rx & tx parameters */
#define MO360ADS_CR_INIT_RX     0x0100        /* init. rx parameters */
#define MO360ADS_CR_INIT_TX     0x0200        /* init. tx parameters */
#define MO360ADS_CR_HUNT        0x0300        /* enter hunt mode */
#define MO360ADS_CR_STOP_TX     0x0400        /* stop tx */
#define MO360ADS_CR_GRSTOP_TX   0x0500        /* graceful stop tx */
#define MO360ADS_CR_RESTART     0x0600        /* restart tx */
#define MO360ADS_CR_CLOSE_RX    0x0700        /* close rxbd */
#define MO360ADS_CR_SET_GA      0x0800        /* set group addr.  */
#define MO360ADS_CR_GCI         0x0900        /* gci abort request */
#define MO360ADS_CR_RESET_BCS   0x0a00        /* reset bcs */ 

/* cr channel number */

#define MO360ADS_CR_SCC1	0x0000        /* channel SCC1 */ 
#define MO360ADS_CR_SCC2   	0x0040        /* channel SCC2 */ 
#define MO360ADS_CR_SPI   	0x0050        /* SPI/RISC timers */
#define MO360ADS_CR_SCC3   	0x0080        /* channel SCC3 */ 
#define MO360ADS_CR_SMC1   	0x0090        /* channel SMC1/IDMA1 */ 
#define MO360ADS_CR_SCC4   	0x00c0        /* channel SCC4 */ 
#define MO360ADS_CR_SMC2   	0x00d0        /* channel SMC2/IDMA2 */ 

#define MO360ADS_CR_FLG   	0x0001        /* command flag */




/* general SCC Mode register, gsmrl */

#define MO360ADS_GSMRL_BEDGE  	0x00000000        /* both positive & negitive is used */
#define MO360ADS_GSMRL_PEDGE  	0x20000000        /* positive edge */
#define MO360ADS_GSMRL_NEDGE  	0x40000000        /* negitive edge */
#define MO360ADS_GSMRL_NOEDGE  	0x60000000        /* no adjustment */

#define MO360ADS_GSMRL_TCI   	0x10000000        /* transmit clock invert */

#define MO360ADS_GSMRL_TSNC0   	0x00000000         /* transmit sense, infinite */
#define MO360ADS_GSMRL_TSNC1   	0x04000000         /* 14/6.5 determine by RDCR bits*/
#define MO360ADS_GSMRL_TSNC2   	0x08000000         /* 4/1.5 determine by RDCR bits*/
#define MO360ADS_GSMRL_TSNC3   	0x0c000000         /* 3/1 determine by RDCR bits*/


#define MO360ADS_GSMRL_RINV   	0x02000000        /* transmit clock invert */
#define MO360ADS_GSMRL_TINV   	0x01000000        /* transmit clock invert */

#define MO360ADS_GSMRL_TPL0   	0x00000000        /* tx preamble length, no preamble */
#define MO360ADS_GSMRL_TPL8   	0x00200000        /* 8 bits */
#define MO360ADS_GSMRL_TPL16   	0x00400000        /* 16 bits */
#define MO360ADS_GSMRL_TPL32   	0x00600000        /* 32 bits */
#define MO360ADS_GSMRL_TPL48   	0x00800000        /* 48 bits */
#define MO360ADS_GSMRL_TPL64   	0x00a00000        /* 64 bits */
#define MO360ADS_GSMRL_TPL128   0x00c00000        /* 128 bits */

#define MO360ADS_GSMRL_TPP0   	0x00000000        /* tx preamble patern, all zeros */
#define MO360ADS_GSMRL_TPP10   	0x00080000        /* repeating 10's, use for ethernet */
#define MO360ADS_GSMRL_TPP01   	0x00100000        /* repeating 01's */
#define MO360ADS_GSMRL_TPP1   	0x00180000        /* all ones */


#define MO360ADS_GSMRL_TEND   	0x00040000        /* transmitter frame ending */

#define MO360ADS_GSMRL_TDCR1   	0x00000000        /* transmit divide clock rate, 1x */
#define MO360ADS_GSMRL_TDCR8   	0x00010000        /* 8x */
#define MO360ADS_GSMRL_TDCR16  	0x00020000        /* 16x */
#define MO360ADS_GSMRL_TDCR32  	0x00030000        /* 32x */

#define MO360ADS_GSMRL_RDCR1   	0x00000000        /* receive divide clock rate, 1x */
#define MO360ADS_GSMRL_RDCR8   	0x00004000        /* 8x */
#define MO360ADS_GSMRL_RDCR16  	0x00008000        /* 16x */
#define MO360ADS_GSMRL_RDCR32  	0x0000c000        /* 32x */

#define MO360ADS_GSMRL_RNRZ1 	0x00000000        /* receiver decoding method, NRZ */ 
#define MO360ADS_GSMRL_RNRZ2 	0x00000800        /* NRZ Mark */ 
#define MO360ADS_GSMRL_RFM0   	0x00001000        /* FM0  */
#define MO360ADS_GSMRL_RMAN1   	0x00002000        /* Manchester */
#define MO360ADS_GSMRL_RMAN2   	0x00003000        /* Differential Manchester */

#define MO360ADS_GSMRL_TNRZ1 	0x00000000        /* transmit decoding method, NRZ */ 
#define MO360ADS_GSMRL_TNRZ2 	0x00000100        /* NRZ Mark */ 
#define MO360ADS_GSMRL_TFM0   	0x00000200        /* FM0  */
#define MO360ADS_GSMRL_TMAN1   	0x00000400        /* Manchester */
#define MO360ADS_GSMRL_TMAN2   	0x00000600        /* Differential Manchester */

#define MO360ADS_GSMRL_DIAGN   	0x00000000        /* diagnostic mode, normal operation */
#define MO360ADS_GSMRL_DIAGL   	0x00000040        /* local loopback */
#define MO360ADS_GSMRL_DIAGA   	0x00000080        /* automatic echo mode */
#define MO360ADS_GSMRL_DIAGLE  	0x000000c0        /* loopback & echo mode */


#define MO360ADS_GSMRL_ENR   	0x00000020        /* enable receive */
#define MO360ADS_GSMRL_ENT   	0x00000010        /* enable transmit */


#define MO360ADS_GSMRL_MHDLC   	0x00000000        /* channel protocol mode, HDLC */
#define MO360ADS_GSMRL_MAPP   	0x00000002        /* AppleTalk */
#define MO360ADS_GSMRL_MSS7   	0x00000003        /* SS7 */
#define MO360ADS_GSMRL_MUART   	0x00000004        /* UART */
#define MO360ADS_GSMRL_MPROF   	0x00000005        /* PROFIBUS */
#define MO360ADS_GSMRL_MAHDLC   0x00000006        /* Async HDLC */
#define MO360ADS_GSMRL_MV14   	0x00000007        /* V.14 */
#define MO360ADS_GSMRL_MBISY   	0x00000008        /* BYSINC */
#define MO360ADS_GSMRL_MDDCM   	0x00000009        /* DDCMP */
#define MO360ADS_GSMRL_METH   	0x0000000c        /* Ethernet */


#define MO360ADS_GSMRH_RFW   	0x00000020        /* rx fifo width */


/* transmit & receive FCR bit defines */

#define MO360ADS_FCR_MOT      0x10         /* type of byte ordering */
#define MO360ADS_FCR_FC3      0x08         /* function code 3 - 0 */
#define MO360ADS_FCR_FC2      0x04     
#define MO360ADS_FCR_FC1      0x02   
#define MO360ADS_FCR_FC0      0x01 



/* sicr register bit defines */
#define MO360ADS_SICR_GRX   	0x80              /* grant support for sccx */
#define MO360ADS_SICR_SCX   	0x40              /* sccx connection */

#define MO360ADS_SICR_SC1   	0                 /* scc1 bit defines starts */
#define MO360ADS_SICR_SC2   	8                 /* scc2 bit defines starts */
#define MO360ADS_SICR_SC3   	16                /* scc3 bit defines starts */
#define MO360ADS_SICR_SC4   	32                /* scc4 bit defines starts */

/* sicr defines */
#define MO360ADS_SICR_R_BRG1	0x00              /* sccx receive clock is brg1 */
#define MO360ADS_SICR_R_BRG2	0x08              /* sccx receive clock is brg2 */
#define MO360ADS_SICR_R_BRG3	0x10              /* sccx receive clock is brg3 */
#define MO360ADS_SICR_R_BRG4	0x18              /* sccx receive clock is brg3 */
#define MO360ADS_SICR_R_CLK1	0x20              /* sccx receive clock is clock1(x=1,2) or clock 5 (x=1,2)*/
#define MO360ADS_SICR_R_CLK2	0x28              /* sccx receive clock is clock2(x=1,2) or clock 6 (x=1,2)*/
#define MO360ADS_SICR_R_CLK3	0x30              /* sccx receive clock is clock3(x=1,2) or clock 7 (x=1,2)*/
#define MO360ADS_SICR_R_CLK4	0x38              /* sccx receive clock is clock4(x=1,2) or clock 8 (x=1,2)*/


#define MO360ADS_SICR_T_BRG1	0x00              /* sccx transmit
clock is brg1 */
#define MO360ADS_SICR_T_BRG2	0x01              /* sccx transmit clock is brg2 */
#define MO360ADS_SICR_T_BRG3	0x02              /* sccx transmit clock is brg3 */
#define MO360ADS_SICR_T_BRG4	0x03              /* sccx transmit clock is brg3 */
#define MO360ADS_SICR_T_CLK1	0x04              /* sccx transmit clock is clock1(x=1,2) or clock 5 (x=1,2)*/
#define MO360ADS_SICR_T_CLK2	0x05              /* sccx transmit clock is clock2(x=1,2) or clock 6 (x=1,2)*/
#define MO360ADS_SICR_T_CLK3	0x06              /* sccx transmit clock is clock3(x=1,2) or clock 7 (x=1,2)*/
#define MO360ADS_SICR_T_CLK4	0x07              /* sccx transmit clock is clock4(x=1,2) or clock 8 (x=1,2)*/


/* BRGC */

#define MO360ADS_BRGC_EN        0x00010000        /* enable BRG count */
#define MO360ADS_BRGC_DIV16     0x00000001        /* BRG clock prescaler divded by 16 */




/****************************************************************************************/
/* specific to the mo360ads board; for the SIM60 PIT */


/*
 * formula from MC68360 user's manual (pg. 6-11):
 *
 *    PIT period = PITR / ( ((EXTAL/divider)/prescaler) / 4)
 *               = PITR * ((divider * prescaler * 4)/ EXTAL)
 *
 *    possible values for the divider: 1 or 128
 *    possible values for the prescaler: 1 or 512    (512 if prescaler is enabled)
 *
 *    for the QUAD's board that we have: the divider = 128, prescaler can be 1 or
512,
 *    and clock input = 25Mhz.
 *
 *    for divider = 1 and prescaler = 1 (prescaler is disabled) the
 *    PIT period ranges from .0016ms to .0004ms.
 *
 *    for divider = 1 and prescaler = 512 (prescaler is enabled) the
 *    PIT period ranges from .0819ms to 20.9ms.
 *
 *    for divider = 128 and prescaler = 1 (prescaler is disabled) the
 *    PIT period ranges from .0205ms to 5.22ms.  This setting will be
 *    the default for the QUAD's board.
 *
 *    for divider = 128 and prescaler = 512 (prescaler is enabled) the
 *    PIT period ranges from 10.5ms to 2670ms.  (DOES NOT WORK FOR THIS
 *    SETTING, even when preload value = 3)
 *
 */
 


#define MO360ADS_PIT_PERIOD      160        /* 4/25000000 x 10^9 nano seconds */
#define MO360ADS_PIT_VECTOR      0x81       /* interrupt vector of PIT */

#define MO360ADS_PIT_DIVIDER_1       1           /* divider for the mo360 quad board */ 
#define MO360ADS_PIT_DIVIDER_128     128         /* divider of 128 */

/* Timer periodic constants */

/* at 10ms, does not work with vconsole & serial bridge */
/* so leave at 5ms */
#define DIVIDER128			

#ifdef DIVIDER128_AND_PRESCALER
#define MO360ADS_PIT_10MS    0x01               /* approx. 10.5 ms, with divider 128 & */
                                                /* prescaler enabled  */
#endif

#ifdef DIVIDER128
#define MO360ADS_PIT_5MS    0xFF               /* approx. 5 ms, with divider 128 & */
                                               /* prescaler disabled  */
#endif

#ifdef DIVIDER1_AND_PRESCALER
#define MO360ADS_PIT_10MS    0x7A               /* approx. 10 ms, with divider 1 & */
                                                /* prescaler enabled  */
#define MO360ADS_PIT_20MS    0xFF               /* approx. 20 ms, with divider 1 & */
                                                /* prescaler enabled */
#endif

#define MO360ADS_PIT_MAX     0x0FF              /* 8 bit counter */






