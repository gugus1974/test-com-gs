#include <reg51.h>
#include <stdio.h>

#include "util.h"
#include "test_ram.h"
#include "adc.h"
#include "pio.h"
#include "uart8251.h"

/*------------------------------------------------
Definizione variabili
------------------------------------------------*/
//sfr 	P3 = 0xB0;	
sbit	WD = P3^5;
sbit	uart_tx_enable = P3^4;

//static unsigned short idata timer0_tick=0;
unsigned short idata timer0_tick=0;


unsigned char del;
unsigned char aaa;
unsigned char tipo_scheda;
unsigned char ritardo;
char scelta_test;
char scelta_scheda;

int get_timer0();
/*------------------------------------------------
	Interrupt Service Routines
------------------------------------------------*/
static void timer0_isr (void) interrupt 1
{
	TR0 = 0;  //ferma il timer
	EA = 0;	  // disabilita interrupt entranti
	
	WD ^= 1;	//toggle per zittire il cane da guardia
	timer0_tick++;
	TH0 = 0x4b;	 //setta il punto di partenza del timer0
	TL0 = 0xff;
	
	TR0 = 1;   //via!
	EA  = 1;
}

/*------------------------------------------------
	Definizione delle Funzioni
------------------------------------------------*/

int get_timer0(void)
{
	return ((int)timer0_tick);
}

void init (void)
{
	SCON 	= 0x52;		//SCON 0101 0010
	TMOD	= 0x21;  	//timer 1,8-bit autoreload
						//timer 0, 16 bit

    TH1 	= 0xfd;		//f3 2400bps con SMOD 0//fd 9600bps con SMOD 0// 0xfd 19200 con SMOD =1   
	TL1 	= TH1;

	TH0 	= 0x4b; 	//4bff=ffff-46080 -> 50ms
	TL0 	= 0xff;

	IP		= 0x02;	 	// timer 0 prioritario
//	IE 		= 0x82;	 	// abilita l'interrupt per overflow del timer 0
	IT0 	= 1;		// Configure interrupt 0 for falling edge on /INT0 (P3.2)
	IT1 	= 1;		// Configure interrupt 0 for falling edge on /INT0 (P3.2)
	IE 		= 0x87;	 	// abilita l'interrupt per overflow del timer 0
	ET0 	= 1;		/* Enable Timer 0 Interrupts */

	TR0 	= 1;    /* Start Timer 0 Running */
	TR1 	= 1;    /* Start Timer 1 Running */

    TI    	= 1;    /* TI:   set TI to send first char of UART    */
	
	uart_tx_enable =0; // abilita la TX	    
}
/*
void echo (void)
{
	printf("\necho attivo\n");
	do
	{
		aaa = _getkey();
		putchar(aaa);
	 }
	 while ((aaa != 0x0D) || (aaa != 0x0A));
}
*/
/*------------------------------------------------
	Stampa menu test
------------------------------------------------*/

void menu (void)
{
    unsigned char	exit_menu; 
	char c;
	do
    {
		printf("\n\n");
		printf("***********************************\n");
		printf("*   Collaudo scheda CPU REVIND    *\n");
		printf("*   sw ver. testrew 1.0           *\n");

		if (tipo_scheda == 1)	printf("*   scheda tipo: DG630-12 (x16)   *\n");
		if (tipo_scheda == 2)	printf("*   scheda tipo: DG630-29 (x64)   *\n");
		printf("***********************************\n");
		printf("\n\n");
		printf("	 1 - test RAM\n");
		printf("	 2 - test ADC\n");
		printf("	 3 - test I/O Progr\n");
		printf("	 4 - test UART_U27 - RICEZIONE\n");
		printf("	 5 - test UART_U27 - TRASMISSIONE\n");
		printf("	 6 - test UART_U30\n");
		printf("	 7 - set tipo scheda \n\n");
	    printf ("\n\n     Scelta test: ");
	    //scanf ("%d", &scelta_test);
		c = _getkey();

		scelta_test = c-0x30;
		
		if(scelta_test>0 && scelta_test<8)
		    exit_menu=1;
		else 
			exit_menu=0;
	
		/*inserire qui il sottomenu*/
	
	}
	while(!exit_menu);
}
unsigned char menu_scheda (void)
{
    unsigned char	exit_menu; 
	char c;
	do
    {
		printf("\n\n");
		printf("***********************************\n");
		printf("*   Collaudo scheda CPU REVIND    *\n");
		printf("*   sw ver. testrew 1.0           *\n");
		printf("***********************************\n");
		printf("\n\n");
		printf("	 1 - DG630-12(prescaler x16)\n");
		printf("	 2 - DG630-29(prescaler x64)\n");
	    printf ("\n\n     Scelta scheda: ");
		c = _getkey();

		scelta_scheda = c-0x30;
		
		if(scelta_scheda>0 && scelta_scheda<3)
		    exit_menu=1;
		else 
			exit_menu=0;
	
		/*inserire qui il sottomenu*/
	
	}
	while(!exit_menu);
	return scelta_scheda;
}

/*------------------------------------------------
MAIN Program
------------------------------------------------*/
void main (void)
{
	/*--------------------------------------
	Configura la seriale del micro 
	--------------------------------------*/
	
	init();		//configura i registr della uart e del TIMER1
	WD = 0;			/* setta il P3.5 */
	/*--------------------------------------
		MAIN LOOP
	--------------------------------------*/
	printf("\n\n\n");
	printf("\n\n\n");
	tipo_scheda = menu_scheda();

	while (1)
  	{
/**
  		test_uartU27_rx_tx();
**/
//		tipo_scheda = menu_scheda();
		
		menu();

		switch (scelta_test)
		{
		 case 1: 	test_ram();
					break;
		 case 2: 	test_adc();
		 			break;
		 case 3: 	test_pio_menu();
		 			break;
		 case 4: 	test_uartU27_ricevi();
		 			break;
		 case 5: 	test_uartU27_trasmetti();
		 			break;
		 case 6: 	test_uartU30();
		 			break;
		 case 7: 	tipo_scheda = menu_scheda();
		 			break;

		default: break;

		}
/**/
	}
}
