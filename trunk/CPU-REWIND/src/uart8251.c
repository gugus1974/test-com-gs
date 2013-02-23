/*************************************************************************************/
/* 18/01/10              \SRC\uart8251.c    								         */
/*************************************************************************************/

#include "reg51.h"
#include <ctype.h>
#include "stdio.h"
#include "util.h"
#include "uart8251.h"
#include "pio.h"

/*************************************************************************************/
sbit TXE_U27=	P1^1;
sbit TXE_U30=	P1^0 ;

static char c;
static unsigned char xdata buff_U27[256];
static unsigned char xdata buff_U30[256];
static unsigned char inizio=0;
static unsigned char fine_u30=0;
static unsigned char fine_u27=0;
static unsigned int xdata start_ritardo=0;

//static unsigned char xdata buff[256];
static unsigned char i;
extern unsigned char del;
extern unsigned char tipo_scheda;
extern int get_timer0();
volatile unsigned char xdata uartU27[2]  _at_ UART8251_U27_DATA_ADDR ;
volatile unsigned char xdata uartU30[2]  _at_ UART8251_U30_DATA_ADDR ;
//volatile unsigned char xdata * uartU27 = (char xdata *) UART8251_U27_DATA_ADDR;
//volatile unsigned char xdata * uartU30 = (char xdata *) UART8251_U30_DATA_ADDR;
static void uartU30_init ( ) ;

/*------------------------------------------------
	Interrupt Service Routines
------------------------------------------------*/

static void RX_RDY_U30_isr (void) interrupt 0  //Int0
{
	EA = 0;
	buff_U30[fine_u30++] = uartU30[0];
	EA  = 1;
}

static void RX_RDY_U27_isr (void) interrupt 2  //Int1
{
	EA = 0;
		buff_U27[fine_u27] = uartU27[0];
		fine_u27++;
	EA  = 1;
}
/*************************************************************************************/




void uart_reset (void)
{
	/* si puliscono i registri di controllo dei PIO*/
	PIO_RESET = 1;
	for(del =0 ; del<100 ; del++) ;
	PIO_RESET = 0;
	printf("\n\nReset uart\n");
}

static void uartU27_init (void)
{
	uart_reset();
	/* CND a 1 @8001 */ 
//	uartU27 = UART8251_U27_CTRL_ADDR; 
	printf("conf uart U27 @%04X\n",(unsigned short) uartU27);
	/* Scrivo il Mode Instruction Register (MIR) = 0x4F*/
	uartU27[1] = BIN8(
				0	,//b7 -	 s2	s1
				1	,//b6 -	  0  1 -> stop bit 1
				0	,//b5 -	 ep pen
				0	,//b4 -	  0	 0 -> none
				1	,//b3 -	 l3 l2
				1	,//b2 -	  1	 1 -> 8 bit data
				1	,//b1 -  b2 b1
				(tipo_scheda==2)	 //b0 -	  1  1 -> x64
				);
	printf("1 - uart U27->%02bXH\n", uartU27[1]);

	/* abilito la trasmissione Control Register (CIR) = 0x35(trasmissione attiva)*/
	/* abilito la trasmissione Control Register (CIR) = 0x37(ricezione attiva)*/
	uartU27[1] = BIN8(
				0	,//b7 -	hunt 
				0	,//b6 -	internal reset  
				1	,//b5 -	RTS 
				1	,//b4 -	reset errori
				0	,//b3 -	send break char
				1	,//b2 -	RXen  
				1	,//b1 - DTR  (0-trasmette   1-riceve)
				1	 //b0 - TXen	 
				);
	printf("2 - uart U27->%02bXH\n", uartU27[1]);

	/* CND a 0 @8000 */ 
//    uartU27 = UART8251_U27_DATA_ADDR;
	printf("3 - uart U27->%02bX\n", uartU27[0]);




}

static void uartU30_init ()
{
	/* configura la uart come rs485 , seriale asincrona, prescaler 16, 8,N,1*/
	uart_reset();
	/* CND a 1 @8001 */ 
	//uartU30 = UART8251_U30_CTRL_ADDR; 
	printf("conf uart U30 \n");

	/* Scrivo il Mode Instruction Register (MIR) = 0x4E*/
	uartU30[1] = BIN8(
				0	,//b7 -	 s2	s1
				1	,//b6 -	  0  1 -> stop bit 1
				0	,//b5 -	 ep pen
				0	,//b4 -	  0	 0 -> none
				1	,//b3 -	 l3 l2
				1	,//b2 -	  1	 1 -> 8 bit data
				1	,//b1 -  b2 b1
				     /*b0 -	  1  0 -> x16  (default per scheda DG630-12)
				              1  1 -> x64  (default per scheda DG630-29) */
				(tipo_scheda==2));
//	printf("1 - uart U30->%02bXH\n", uartU30[1]);

	/* abilito la trasmissione e la ricezione nel
	   Control Register (CIR) = 0x37*/
	uartU30[1] = BIN8(
				0	,//b7 -	hunt 
				0	,//b6 -	internal reset  
				1	,//b5 -	RTS 
				1	,//b4 -	reset errori
				0	,//b3 -	send break char
				1	,//b2 -	RXen  
				0	,//b1 - DTR    (0:trasmissione)
				1	 //b0 - TXen	 
				);
//	printf("2 - uart U30->%02bXH\n", uartU30[1]);

	/* CND a 0 @8000 */ 
//    uartU30 = UART8251_U30_DATA_ADDR;
//	printf("3 - uart U30->%04XH   %02bXH\n", (unsigned short)uartU30,uartU30[0]);
}



void test_uartU27_rx_tx()
{
//	char exit_char;

	uartU27_init();	

	fine_u27 =0;

	uartU27[1] = 0x35; //abilita la ricezione

	while (1) 
	{
//		c=getchar();
//		if((exit_char=toupper(c))=='Q') return;
			DELAY;
			DELAY;
			DELAY;
			DELAY;
			DELAY;
			DELAY;
			DELAY;
			DELAY;
			DELAY;
			DELAY;
			DELAY;
			DELAY;
			DELAY;
			DELAY;
			uartU27[0]= 0x55;
/*
		if (fine_u27!=0)
		{
			printf("\nRX (%bd byte): ",fine_u27);
			for(i=0; i<fine_u27; i++) printf("%bd:%02bXH  ",i,buff_U27[i]);
				
			printf("\n");
		}
*/			DELAY;
			DELAY;
			DELAY;
			DELAY;
			DELAY;
			DELAY;
			DELAY;
			DELAY;
			DELAY;
			DELAY;
			DELAY;
			DELAY;
			DELAY;
//			uartU27[1] = 0x37; //abilita la trasmissione


/*			printf("\nTX (%bd byte): ",fine_u27);
			for(i=0; i<fine_u27; i++) 
				uartU27[0]=buff_U27[i],printf("%bd:%02bXH  ",i,buff_U27[i]);

			fine_u27 =0;
			printf("\n");
*/
		
	}
 	

}


void test_uartU27_ricevi()
{
//	char exit_char;

	uartU27_init();	
	DELAY;
	fine_u27 =0;
	//abilita la ricezione
	uartU27[1] = 0x35; 
	DELAY;
	while (1) 
	{
		if (fine_u27!=0)
		{
			printf("\nRicevuti %bd byte : ",fine_u27);
			for(i=0; i<fine_u27; i++)
				printf("%02bXH ",buff_U27[i]);
			printf("\n");
			fine_u27 =0;
		}		
	}
}
void test_uartU27_trasmetti()
{
//	char exit_char;

	uartU27_init();	
	DELAY;
	//abilita la trasmissione
	uartU27[1] = 0x37; 

	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;

	while (1) 
	{
		//abilita la trasmissione


		uartU27[0]=0x55;
		printf("\nTX: 0x%02bxH ",0x55);

		DELAY;
		DELAY;
		DELAY;
		DELAY;
		DELAY;
		DELAY;
		DELAY;
		DELAY;
		DELAY;
		DELAY;
		DELAY;
		DELAY;
		DELAY;
		DELAY;
		DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
	DELAY;
		DELAY;
	}
 	

}


void test_uartU30()
{
//	static unsigned char err_u30=0;
	unsigned char i=0;
	char exit_char;
	printf("\nPremere un tasto e verificare che corrisponda a quello ricevuto (q per uscire)\n");
	uartU30_init();	
	fine_u30 =0;
	while (1) 
	{
		c=getchar();
		if((exit_char=toupper(c))=='Q') break;
		if (TXE_U30 ) 
		{
			/*uartU30[1]= 0x35; // in trasmissione
			DELAY;
			DELAY;
			DELAY;
			DELAY;
			DELAY;
			DELAY;

			 */
			uartU30[0] = c;
			printf("\nTX:%c(%02bxH)  ",c,c);
		}

		if (fine_u30!=0)
		{
			printf("  RX (%bd byte):",fine_u30);
			for(i=0; i<fine_u30; i++)
			{
				printf("% 02bx ",buff_U30[i]);
				//if (c!= buff_U30[i]) err_u30++;
			}
			printf("\n\n");
			fine_u30 = 0;
		}
	}
	printf("\n");
/*	if (err_u30) 
		printf("Test UART U30 KO ,errori rx %bd/255",err_u30);
	else 
		printf("Test UART U30 OK ,errori rx %bd/255",err_u30);
*/
}
