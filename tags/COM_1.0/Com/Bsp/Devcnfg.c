/*=====================================================================================*/
/* COM board BSP configuration                                                         */
/* Implementation file (devcnfg.h)  			          							   */
/* 																					   */
/* Version 1.0																		   */
/*																					   */
/* (c)1997 Ansaldo Trasporti														   */
/* Written by Giuseppe Carnevale													   */
/*=====================================================================================*/


/*=====================================================================================*/
/* What follows is the file "devcnfg.c" for the mo360ads board modified for our needs  */


/* ***********************************************************
** Include Files
*************************************************************/
#include "ansiprot.h"      /* ANSI prototypes                */
#include "logio.h"         /* logio defines and typedefs     */
#include "vbsp_id.h"     /* VBSP control block typedef     */
#include "vbsp.h"          /* VBSP return codes and typedefs */
#include "logiobuf.h" /* circular character buffers     */
#include "/sw/com/bsp/atrcom.h"      /* board specific defines and typedefs */

#if SERIAL_DEVICE == 1
#include "mo360scc.h"       /* serial defines and typedefs */
#include "serial_1.h"      /* serial interface data structs  */
#endif

#if TIMER_DEVICE == 1
#include "mo3xxpit.h"         /* timer defines and typedefs */
#include "timer.h"
#endif



/* ***********************************************************
** Defines (to be specified in .def file)
*************************************************************/

/******************************************************************************
****************************  SERIAL DEVICES   ********************************
******************************************************************************/
#if SERIAL_DEVICE == 1

#if SERIAL_INTERRUPT == 1
vbsp_return_t mo360scc_int_start(); /* board specific ISR start handling  */
vbsp_return_t mo360scc_int_end();   /* board specific ISR end handling    */
vbsp_return_t mo360scc_int_en();    /* board specific int enable handling */
vbsp_return_t mo360scc_int_dis();   /* board specific int disable handling*/
#endif

/*
 * to calculate the vector # figure out the interrupt # (pg. 7-369) for the sccx 
 * get the lower 5 bits of the vector and concatenate it with the 3 higher bits 
 * of VBA2-VBA0 in the register cicr : eg: scc3 lower 5 bits = 11100, cicr =
 * 0x00e49f60 (VBA2-VBA0 = 011 (binary) ), hence vector no. = 1111100 = 0x7c 
 *
 */


#define MO360SCC_RX_VECTOR   0x7c     /* mo360scc receive interrupt vector       */
#define MO360SCC_TX_VECTOR   0x7c     /* mo360scc transmit interrupt vector      */
#define MO360SCC_RXE_VECTOR  0x7c     /* mo360scc receive error interrupt vector */


mo360scc_config_t mo360scc_config_1 =
{
    /* serial_1_config_t */
    {
#ifdef XDM
        serial_tty_device,		/* XDM only: force to serial for XDM      */
#else
        serial_packet_device,            /* serial_packet_device, serial_tty_device */ 
#endif
	25000000,               /* clock speed in Mhz */
#ifdef XDM
        9600,                          /* baud rate: maximum of 19200 */
#else
        19200,
#endif
	/* at initialization the parity, bit, & stop fields are ignored */
        /* see the field in specifics for psmr where these fields are */
        /* configured at initialization */

	PARITY_NONE,            /* from serial_1.h */
	BITS_8,                 /* from serial_1.h */
	STOP_BITS_1,            /* from serial_1.h */
	MO360SCC_RX_VECTOR,     /* mo360scc receive interrupt vector */
	MO360SCC_TX_VECTOR,     /* mo360scc transmit interrupt vector */
	MO360SCC_RXE_VECTOR,    /* mo360scc receive error interrupt vector */
	0                       /* void pointer for interface expansion */
    },

     /* mo360scc_reg_map_t from mo360scc.h */
     /* change these according to the sccx that is used for serial*/
     {
         (BYTE *)(MO360ADS_REGB),        /* reg_base */
	 (BYTE *)(MO360ADS_SCC3PB),      /* scc3 base */
	 (LONG *)(MO360ADS_REGB+MO360ADS_GSMR_L3),  /* sccx general mode register */
	 (LONG *)(MO360ADS_REGB+MO360ADS_GSMR_H3),  /* sccx general mode register */
	 (WORD *)(MO360ADS_REGB+MO360ADS_PSMR3),    /* sccx protocol specific mode register */
	 (WORD *)(MO360ADS_REGB+MO360ADS_TODR3),     /* sccx transmit on demand */
	 (WORD *)(MO360ADS_REGB+MO360ADS_DSR3),     /* sccx data sync. register */
	 (WORD *)(MO360ADS_REGB+MO360ADS_SCCE3),    /* sccx event register */
	 (WORD *)(MO360ADS_REGB+MO360ADS_SCCM3),     /* sccx mask register */
	 (WORD *)(MO360ADS_REGB+MO360ADS_SCCS3),      /* sccx status register */
	 (WORD *)(MO360ADS_REGB+MO360ADS_BRGC1)      /* clock source */

    },

    /* ----------------------------------------------------------------------
    **   INSTRUCTION:
    **      Fill in the board specific function handlers for the device.
    **      These are for any board specific functions required during
    **      register accesses or functions to interface to an external
    **      interrupt controller.  Refer to extio_1.h.
    ** --------------------------------------------------------------------*/

    /* extern_fops_t from extio_1.h */

    {
        0,                      /* register write byte */
	0,                      /* register write word */
	0,                      /* register write long */
	0,                      /* register read  byte */
	0,                      /* register read  word */
	0,                      /* register read  long */

#if SERIAL_INTERRUPT == 1
	0, /*mo360scc_int_start,  /* int_strt function pointer */
	0, /*mo360scc_int_end,    /* int_end function pointer */
	0, /*mo360scc_int_dis,    /* board specific int disable handling */
	0, /*mo360scc_int_en,     /* board specific int enable handling */
#else
	0,                      /* int_strt function pointer */
	0,                      /* int_end function pointer */
	0,                      /* board specific int disable handling */
	0,                      /* board specific int enable handling */
#endif
	0
    },


    /* fill in the specific initialization */
    /* mo360scc_specific_t from mo360scc.h */
    {
        /* do not change the following uart values */
  
        (WORD)0x2,              /* SCCx max. idle state */  
	(WORD)0,                /* SCCx break count register */
	(WORD)0,                /* SCCx parity error condition */
	(WORD)0,                /* SCCx framing error counter */
	(WORD)0,                /* SCCx noise counter */
	(WORD)0,                /* SCCx break count register */
	(WORD)0,                /* SCCx UART Address Character 1 */
	(WORD)0,                /* SCCx UART Address Character 2 */
	(WORD)0,                /* SCCx tx out of sequence character */
	(WORD)0x8000,           /* SCCx control character 1 */
	(WORD)0x8000,           /* SCCx control character 2 */
	(WORD)0x8000,           /* SCCx control character 3 */
	(WORD)0x8000,           /* SCCx control character 4 */
	(WORD)0x8000,           /* SCCx control character 5 */
	(WORD)0x8000,           /* SCCx control character 6 */
	(WORD)0x8000,           /* SCCx control character 7 */
	(WORD)0x8000,           /* SCCx control character 8 */
	(WORD)0xc0ff,           /* SCCx rx control character mask */



	/* receive function code register :0x15 */
	(BYTE)(MO360ADS_FCR_MOT | MO360ADS_FCR_FC2 | MO360ADS_FCR_FC0),           
	/* transmit function code register :0x15 */
	(BYTE)(MO360ADS_FCR_MOT | MO360ADS_FCR_FC2 | MO360ADS_FCR_FC0),           

	/* for tty device rx buffers are 1 char. length */
	(WORD)MAX_PACKET_SIZE,       /* max. rx buffer */

	/* change these value to desired rxbd's offset */
	(WORD *)0x22000,        /* 1st rxbd start */
	(WORD )0,              /* offset of the 1st rxbd */
	(WORD *)0x22040,        /* 1st txbd start */
	(WORD )0x40,           /* offset of the 1st txbd */

	/* change to the correct sccx channel */
	(LONG)MO360ADS_CIXX_SCC3,  /* cmp interrupt source, sccx */
	/* change to the correct sccx channel */
	(WORD)MO360ADS_CR_SCC3,  /* which sccx channel to send commands to */

	/* (LONG)0x00028004,   gsmr_l */

	(LONG)(MO360ADS_GSMRL_TDCR16 | MO360ADS_GSMRL_RDCR16 | MO360ADS_GSMRL_MUART),
	/* (LONG)0x00000020,    gsmr_h , fifo width = 8*/	                        
	(LONG)(MO360ADS_GSMRH_RFW), 

	/*
	 * psmr :set this field for first initializing the chip for stop, bit, parity 
	 * for more details see o360quicc manual pg 7-160:
         *
         * bit  15     14   13 12 11 10  9     8     7     6     5   4    3 2   1 0
         *      + ----+----+-----+-----+-----+-----+-----+-----+---+-----+-----+----+
         *      | FLC | SL | CL  | UM  | FRZ | RZS | SYN | DRT | - | PEN | RPM | TPM |
         *      + ------------------------------------------------------------------+
         *
         *  FLC : flow control       0 normal operation
         *                           1 Asysn. flow control
         *  SL  : stop length        0 one stop bit
         *                           1 two stop bit
         *  CL  : character length   00 5 data bits
         *                           01 6 data bits
         *                           10 7 data bits
         *                           11 8 data bits
         *  UM  : uart mode          00 normal uart
         *                           01 multidrop non automatic
         *                           10 reserved
         *                           11 multidrop automatic
         *                           1 two stop bit
         *  FRZ : freeze transmission  0 normal operation
         *                             1 halt transmitter & continue
         *                               from same point later
         *  RZS : receive 0 stop bits  0 normal operation
         *                             1 continue receiving if missing
         *                               stop bit
         *  SYN : synchronous mode   0 normal operation
         *                           1 enable sync. mode
         *  DRT : disable receiver while transmitting
         *                           0 normal operation
         *                           1 enable option
         *  PEN : enable parity      0 no parity
         *                           1 enable parity
         *  RPM : receiver parity    00 odd parity
         *                           01 low parity
         *                           10 even parity
         *                           11 high parity
         *  TPM : transmitter parity 00 odd parity
         *                           01 low parity
         *                           10 even parity
         *                           11 high parity
         */
	 
	/* automatic flow, 8 bit data, stop = 1, no parity */
	(WORD)(MO360SCC_PSMR_FLC |MO360SCC_PSMR_CL8), 

	/* fractional stop bits are set in dsr */
	/* set pg 7-156 mo360quicc manual for further details */
	(WORD)0x7e7e,            /* dsr */


	/* baud rate configuration */
	(BYTE)MO360SCC_BRGC_NODIV16,  /* BRGC: no divider of 16 for prescaler,  */


	/* do not modify, manipulated by the driver */
        (char)0,                      /* buffer for txbd */
        (int)0,                       /* curretn index into rxbd buffer */
        (int)0,                       /* no. of char. in rxbd buffer */
        (WORD *)0,                    /* current rxbd */
        (int)0                        /* buffer full/empty condition */
    }
};



#endif

/******************************************************************************
****************************  TIMER     DEVICES   *****************************
******************************************************************************/

#if TIMER_DEVICE == 1

mo3xxpit_specific_t RAM_mo3xxpit_specific;

mo3xxpit_config_t pit_config_rom =
{
   {                           /* timer config_t */
   MO360ADS_PIT_VECTOR,        /* NOTE: the pit must be the only */
			       /* source on this interrupt vector */
   MO360ADS_PIT_PERIOD,        /* clock period in nano seconds */
   MO3XXPIT_PRESCALER_CNT1,    /* count, prescaler's count */
   MO360ADS_PIT_5MS,           /* preload,must leave at 5ms */
			       /* `cus serial bridge does not work with vconsole at 10 ms*/
   &RAM_mo3xxpit_specific      /* specific */
   },

   {   0,0,0,0,0,0,0,0,0,0   }, /* externio */

   {                            /* reg map */
   (WORD *)(0x2102a),           /* pitr */
   (WORD *)(0x21026), 	        /* picr*/
   },

   {                           /* mo3xxpit_specific_t */
   MO3XXPIT_PICR_IRQ_6,        /* ipl_levels */
   MO3XXPIT_PRESCALER_OFF,     /* disable prescaler */
   MO360ADS_PIT_DIVIDER_128,   /* type of divider for the timer */
   }

};


#endif





/* ***********************************************************
** Descriptors
*************************************************************/
#if SERIAL_DEVICE == 1
serial_dev_desc_t serial_1_dev_desc;
#endif

#if TIMER_DEVICE == 1
timer_dev_desc_t     mo3xxpit_timer_dev;
#endif

/* ***********************************************************
** RAM to copy config structures into
*************************************************************/
#if SERIAL_DEVICE == 1
mo360scc_config_t mo360scc_config_ram;
#endif

#if TIMER_DEVICE == 1
mo3xxpit_config_t pit_config_ram;
#endif

/* ***********************************************************
** Methods
*************************************************************/
#if SERIAL_DEVICE == 1
logio_method_t logio_serial_1_method = {&logio_serial_fops,(void *)&serial_1_dev_desc};
#endif

#if TIMER_DEVICE == 1
logio_method_t logio_timer_1_method  = {&logio_timer_fops,  (void *)&mo3xxpit_timer_dev};
#endif

/* ***********************************************************
** ids
*************************************************************/
#if SERIAL_DEVICE == 1
logio_device_id_t logio_serial_1_id = &logio_serial_1_method;
#endif

#if TIMER_DEVICE == 1
logio_device_id_t logio_timer_1_id  = &logio_timer_1_method;
#endif


/*
** logio_device_install(void)
**
** Description:
**
**    This function will call the install functions for each device.
**    Any ROM to RAM copies of configuration structures are performed
**    here.
**
** Returns:
**
**    LOGIO_STATUS_OK:              All devices installed properly
**    LOGIO_STATUS_NOT_INITIALIZED: function failed
*/
logio_status_t logio_device_install(void)
{


   long cicr;

#if TIMER_DEVICE == 1 
   pit_config_ram = pit_config_rom;
   if(mo3xxpit_install(&pit_config_ram, &mo3xxpit_timer_dev))
      return(LOGIO_STATUS_NOTINITIALIZED);
#endif


#if 0

   /* Initialize the ports for the QUADS board */
   /* 
    ** The reason that the init. of ports are in devcnfg.c and not in 
    ** the ethernet or serial driver is 'cus custmer may have a different
    ** board with different pin assignments.
    */

   /*
    **
    **  Section  Slave QUICC Port A of the M68360QUADS User's
    **	Manual
    ** The pins of interest are :
    ** (The states we will use are marked with an * )
    **
    **  0	Ethernet Rx - Configured as receive data of SCC1 in the
    **                        slave QUICC.
    **  1	Ethernet Tx - Configured as transmit data of SCC1 in the
    **                        slave QUICC.
    **  4	RS-232 Tx   - Configured as receive data of SCC3 in the
    **                        slave QUICC.
    **  5	RS-232 Rx   - Configured as transmit data of SCC3 in the
    **                        slave QUICC.
    **  6	Enable DRAM - Configured as input pin in slave QUICC.  The 
    **                        level indicates if DRAM is accessable on the 
    **                        QUADS.  Level = 0 , DRAM is enabled. 
    **  7	Enable Flash - Configured as input pin in slave QUICC.  The 
    **                        level indicates if FLASH is accessable on the 
    **                        QUADS.  Level = 0 , FLASH is enabled. 
    **  8	Ethernet TCLK - Configured as transmit clock of SCC1 in the
    **                          slave QUICC.
    **  9	Ethernet RCLK - Configured as receive clock of SCC1 in the 
    **                          slave QUICC.
    **
    **
    ** PAPAR is the pin assignment register 
    **	0 = General-purpose I/O
    **	1 = Dedicated peripheral function
    **
    ** PADIR is the pin data direction register
    **	0 = input
    **	1 = output
    ** 
    ** PAODR is the pin open-drain register.
    **	0 = I/O pin is actively driven as an output
    **	1 = I/O pin is an open-drain driver; as an output, pin as actively
    **      driven low, otherwise in three-stated
    **
    **		
    ** register		bit state x= dont care 	
    **		1 1 1 1  1 1 
    **		5 4 3 2  1 0 9 8  7 6 5 4  3 2 1 0
    ** 		- - - -  - - - -  - - - -  - - - -
    ** PAPAR	x x x x  x x 1 1  0 0 1 1  x x 1 1  = 0x0333
    ** PADIR	x x x x  x x x x  0 0 x x  x x x x  
    ** PAODR	x x x x  x x 0 0  0 0 0 0  x x 0 0  
    ** PADAT	this is not initialized and undefined at reset.
    **
    ** 
    */

   *(WORD *)(MO360ADS_REGB+MO360ADS_PAPAR) &= 0xfc0c; /* clear */
   *(WORD *)(MO360ADS_REGB+MO360ADS_PAPAR) |= 0x0333; /* set */

   *(WORD*) (/*0x23550*/MO360ADS_REGB + MO360ADS_PADIR) &= 0xff3f; /* clear */

   *(WORD*)(/*0x23554*/MO360ADS_REGB+ MO360ADS_PAODR) &= 0xfc0c; /* clear */



   /*
    ** Inorder for the MC68160 device to function we need to setup the
    ** portc parallel IO pins correctly.
    **
    ** For a detailed description of the pins see :
    **  Section 4.7.9.3 Slave QUICC Port C of the M68360QUADS User's
    **	Manual
    ** The pins of interest are 
    **	The states we will use are marked with an *
    **
    **  0	TENA     - Configured as the RTS signal of SCC1
    **  2	EEST CS2 - Configuerd as output pin. A
    **			 1 = EEST operates in standby mode
    **			*0 = EEST operates in normal mode
    **  3	TPEN     - Configured as I/O pin. Which determines which
    **			port is is slected AUI or TP
    **		We do use this pin we let the device auto select.
    **  4	CLSN	- Configured as the CTS signal of SCC1
    **  5	RENA	- Configured as CD signal of SCC1
    **  
    **  7	APORT	- Configured as ouput.
    **			*1 = EEST automatically select TP or AUI
    **			 0 = TPEN selects the operating port
    **  8	TPAPCE	- Configured as output.
    **			*1 - automatic polarity correction is enabled
    **			    on the TP port
    **  9	TPSQEL	- Configured as output.
    **			 0 = enable testing the EEST internal TP
    **			    collision detect circuitry
    **			*1
    ** 10	TPFULDL	- Configured as output.
    **			 0 = Allows simultaneous transmit and receive 
    **			    operations on the tp port without an 
    **			    indicated collision
    **			*1
    ** 11	LOOP	- Configured as output.
    **			 1 = enables diagnostic loopback 
    **			*0
    **
    ** I am only going to discussed the setting of the following portc
    ** registers, PCPAR,PCDIR,PCCSO,PCDAT
    **
    ** PCPAR is the pin assignment register 
    **	0 = General-purpose I/O
    **	1 = Dedicated peripheral function
    **
    ** PCDIR is the pin data direction register
    **	0 = input
    **	1 = output
    **
    **
    **		
    ** register		bit state x= dont care 	
    **		1 1 
    **		1 0 9 8  7 6 5 4  3 2 1 0
    ** 		- - - -  - - - -  - - - -
    ** PCPAR	0 0 0 0  0 x 0 0  0 0 x 1
    ** PCDIR	1 1 1 1  1 x 0 0  1 1 x 0
    ** PCSO		x x x x  x x 1 1  x x x x
    ** PCDAT	0 1 0 1  1 0 0 0  0 0 0 0
    **
    ** 
    */

   /*
    ** Set the bits according to the above. 
    ** REMEMBER ONLY TOUCH BITS OF INTEREST. 
    ** NEVER ASSUME THAT THE OTHER BIT ARE OURS TO PLAY WITH.
    */

   /* Set the PCPAR */
   *(WORD *)(MO360ADS_REGB + MO360ADS_PCPAR) &= 0x0042; /* clear */
   *(WORD *)(MO360ADS_REGB + MO360ADS_PCPAR) |= 0x0001; /* set */
   /* Set the PCDIR */
   *(WORD *)(MO360ADS_REGB + MO360ADS_PCDIR) &= 0x0042; /* clear */;
   *(WORD *)(MO360ADS_REGB + MO360ADS_PCDIR) |= 0x0f8c; /* set */;
   /* Set the PCSO */
   *(WORD *)(MO360ADS_REGB + MO360ADS_PCSO) &= 0xffcf; /* clear */
   *(WORD *)(MO360ADS_REGB + MO360ADS_PCSO) |= 0x0030; /* set */
   /* Set PCDAT */
   *(WORD *)(MO360ADS_REGB + MO360ADS_PCDAT) &= 0x004f;
   *(WORD *)(MO360ADS_REGB + MO360ADS_PCDAT) |= 0x0580;
   

   /* interrupt service mask = 7 & arbit id = 5 */
   *(WORD *)(/*0x2351e*/MO360ADS_REGB + MO360ADS_SDCR) |= 0x750;


   /* route clk1 -> TCK  & clk2 -> RCK for SCC1 & select NMSI pins */
   /* route br1 to scc3 & select NMSI */

   *(LONG *)(/*0x236ec*/MO360ADS_REGB+ MO360ADS_SICR) = 0x0000002c;


   /* SCC4 assert in sccd, SCC3 assert in sccc, SCC2 assert in sccb, SCC1 assert in scca */
   /* IRQ level = 4  : 0x00e49f60  */
   /* set scc3 assert in sccb : 0x00d29f60*/ 

   cicr = (LONG)(MO360ADS_SCCd_SCC4 |MO360ADS_SCCc_SCC3 |MO360ADS_SCCb_SCC2 |MO360ADS_SCCa_SCC1
          |MO360ADS_CICR_IRQ4 | MO360ADS_CICR_HP4 | MO360ADS_CICR_HP3 | MO360ADS_CICR_HP2 
          | MO360ADS_CICR_HP1 | MO360ADS_CICR_HP0 |MO360ADS_CICR_VBA3);


   *(LONG *)(MO360ADS_REGB + MO360ADS_CICR) = cicr; 

#endif


#if SERIAL_DEVICE == 1
   mo360scc_config_ram = mo360scc_config_1;
   if (mo360scc_install(&mo360scc_config_ram, &serial_1_dev_desc))
      return (LOGIO_STATUS_NOTINITIALIZED);
#endif


   return(LOGIO_STATUS_OK);
}



