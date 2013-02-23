/*************************************************************************************/
/* 06/01/10              \SRC\pio.c    								                 */
/*************************************************************************************/
#include <ctype.h>

#include "reg51.h"
#include "stdio.h"
#include "pio.h"

/*************************************************************************************/

sbit P1_3 = P1^3;
sbit P1_4 = P1^4;
sbit P1_5 = P1^5;
sbit P1_6 = P1^6;
#define P1_7   PIO_RESET
/*************************************************************************************/

volatile unsigned char 	xdata  U26[4] _at_ PIO_U26_PA_ADDR;
volatile unsigned char 	xdata  U29[4] _at_ PIO_U29_PA_ADDR;
volatile unsigned char 	xdata  buff_U37 _at_ 0xE000;
volatile unsigned char 	xdata  buff_U38 _at_ 0xC000;

/*************************************************************************************/
static unsigned char xdata cont=0;
static unsigned short xdata start_ritardo=0;
static unsigned char xdata error;	
static unsigned char xdata error_PA;	
static unsigned char xdata error_U38;	
static unsigned char xdata esito_test_led_off;	
static unsigned char xdata esito_test_led_on;	
static unsigned char xdata esito_test_sw1;	
static unsigned char xdata esito_test_u15;	
static unsigned char xdata esito_test_u26_pb;	
static unsigned char xdata esito_test_u26_pc;	
static unsigned char xdata esito_test_u29_pa;	
static unsigned char xdata esito_test_u38;	
static unsigned char xdata esito_test_pio;	
extern unsigned char del;	
static unsigned char xdata c;	
static unsigned char xdata i;
static unsigned char xdata mask, mask2;

/*************************************************************************************/
unsigned char ins_stato_led();

unsigned char test_U15();  //test porta OUT da P1 del micro
unsigned char test_U26_PC();	  // test pilotaggio relè
unsigned char test_U26_PB();	  // test pilotaggio led e rilettura
unsigned char test_U29_PA();	  // test pilotaggio led e rilettura
unsigned char test_U38();	  // test pilotaggio led e rilettura
unsigned char test_SW1();
/*************************************************************************************/

unsigned char xdata exit;	
char scelta;
//extern int get_timer0();
//void print_pio_port (void);
/*************************************************************************************/


void pio_reset()
{
	/* si puliscono i registri di controllo dei PIO*/
	PIO_RESET = 1;
	DELAY;
	DELAY;
	PIO_RESET = 0;
	printf("Reset PIO\n");
}
/*void test_registri()
{
	// si puliscono i registri di controllo dei PIO
	PIO_RESET = 1;
	DELAY;
	DELAY;
	PIO_RESET = 0;
	printf("reset registri PIO\n");

	U26[CR] = PIO_U26_CONF;   // scrittura del Control Register
//	U29[CR] = PIO_U29_CONF;   // scrittura del Control Register
	DELAY;

	while(1)
	{
		printf("U26 CR KO (%bx %bx)\r",U26[CR],PIO_U26_CONF);
	//	printf("U29 CR KO (%bx %bx)\n\n",U29[CR],PIO_U29_CONF);
		
	}
}
*/
void pio_conf()
{
	// U26  PA:in  PB:out  PC:out
	// U29  PA:out PB:in   PC:in
	printf("CONF PIO U26 ");
	U26[CR] = PIO_U26_CONF;   // scrittura del Control Register
/*	if( (~U26[CR]) == PIO_U26_CONF)
		printf(" OK\n");
	else
		printf(" *KO* (%bx)\n",~U26[CR]);
*/
	printf("CONF PIO U29 ");
	U29[CR] = PIO_U29_CONF;   // scrittura del Control Register
/*	if((~U29[CR]) == PIO_U29_CONF)
		printf(" OK\n");
	else
		printf(" *KO* (%bx)\n",~U29[CR]);
*/
}
void test_pio_menu()
{
	unsigned char	exit_menu; 
	
	char c;
	do
	{
		printf("\n\n");
		printf("***********************************\n");
		printf("*   scelta test PIO               *\n");
		printf("***********************************\n");
		printf("\n");
		printf("	 1 - comando reset\n");
		printf("	 2 - configurazione registri\n");
		printf("	 3 - test I/O completo\n");
		printf("	 4 - test lettura SW1 \n");
		printf("	 5 - test pilotaggio relè U15 \n");
		printf("	 6 - test output U26 PB \n");
		printf("	 7 - test output U26 PC \n");
		printf("	 8 - test output U29 PA \n");
		printf("	 9 - test output U38 \n");
		printf("	 0 - menu principale\n");
		printf ("\n     Scelta test: ");
		c = _getkey();
		
		scelta = c-0x30;

		exit_menu=0; //evita di tornare al menu principale
		
		if(scelta>=0 && scelta<10)
		{
		
			switch (scelta)
			{
			case 1: 	pio_reset();
						break;
			case 2: 	pio_conf();
						break;
			case 3: 	test_pio();
						break;
			case 4: 	
						pio_reset();
						pio_conf();
						test_SW1();
						break;
			case 5: 	test_U15();
						break;
			case 6: 	
						pio_reset();
						pio_conf();
						test_U26_PB();
						break;
			case 7: 	
						pio_reset();
						pio_conf();
						test_U26_PC();
						break;
			case 8: 	
						pio_reset();
						pio_conf();
						test_U29_PA();
						break;
			case 9: 	
						pio_reset();
						pio_conf();
						test_U38();

						break;

/*			case 9: 	test_registri();
						break;
*/
			case 0: 	
			default:   	exit_menu=1; //ritorno al menù principale
						break;
			}
		}
	}
	while(!exit_menu);
	
}


/*
static void shift_test (unsigned char *p_out, unsigned char * p_in )
{
    mask =0x01;
	error = 0;
	p_out = 0; 
	DELAY;		
	if(*p_out != *p_in) // ??
	{
		error = 1;
		printf("Test bit%bd  error:%bd   out:%bx  in:%bx \n",cont,error,*p_out,*p_in); 
	}

	*p_out = 1;
	cont = 1;
	exit =0;

	start_ritardo = get_timer0(); 
    do
	{
		if((get_timer0() - start_ritardo) == 16 )
		{// eseguito ogni 10 * 50ms
			if(cont<8)
			{
				*p_out <<= 1; 
				DELAY;		
				if(*p_out != *p_in) // ??
				{
					error = 1;
            		printf("Test bit%bd  error:%bd   out:%bx  in:%bx \n",cont,error,*p_out,*p_in); 
				}
				cont++;
			}
			else
				exit =1;
			start_ritardo = get_timer0();
		}
		printf("\n");
		print_char_bin(*U29_PA),printf("\r");
	}
	while ( !exit) ;
}

*/


unsigned char  test_U26_PB()	  // test pilotaggio led e rilettura
{
	static unsigned char err_cnt;
	err_cnt =0;


	printf("\n\nTEST U26 porta B con rilettura su U29 porta C\n");
	printf("\nVerificare accensione dei led U13 (U26 PB = 00H) \n");

	U26[PB] = 0x00; 
	DELAY;
	if( (U29[PC]& 0x3f )==0x0F )
		printf("rilettura su U29.PC OK (0x%02bx)\n",(U29[PC]));
	else{
		printf("!!! ERRATA rilettura su U29.PC  (0x%02bx invece di 0x0F)\n",(U29[PC]));
		err_cnt++ ;
	}
	printf("premi <INVIO>\n");	while ( (c = getchar())!= 0x0A) ;

	printf("\nVerificare spegnimento dei led U13 (U26 PB = FFH)\n");
	U26[PB] = 0xff; 
	DELAY;
	if( (U29[PC]& 0x3f )==0x30 )
		printf("rilettura su U29.PC OK (0x%02bx)\n",(U29[PC]));
	else{
		printf("!!! ERRATA rilettura su U29.PC  (0x%02bx)\n",(U29[PC]));
		err_cnt++ ;
	}
	printf("premi <INVIO>\n");while ( (c = getchar())!= 0x0A) ;

//	U26[PB] = 0xff;	// spegne i led 
	mask = 0x01;
	
	for(i=0; i<4; i++) //
	{
	    U26[PB] = 0xff;	// spegne i led
		U26[PB] &= ~mask;  //pone a 0 il bit i-simo
		printf("\nTest U26.PB%bd (0x%02bx) - Verificare accensione U13 led %bd\n",i,U26[PB],i+2);
		
		if( (U29[PC]& 0x3f) == (0x30| mask))
			printf("rilettura su U29.PC OK (0x%02bx)\n",(U29[PC]));
		else{
			printf("!!! ERRATA rilettura su U29.PC (0x%02bx)\n",(U29[PC]));
			err_cnt++ ;
			}
    	
		mask <<=1;
		printf("premi <INVIO>\n");
		while ( (c = getchar())!= 0x0A) ;
	}

	for(i=4; i<6; i++) //
	{
	    U26[PB] = 0xff;	// spegne i led
		U26[PB] &= ~mask;  //pone a 0 il bit i-simo
		printf("\nTest U26.PB%bd (0x%02bx) - Verificare accensione U13 led %bd\n",i,U26[PB],i+2);
		
		if( (U29[PC]& 0x3f) == (0x30&(~mask)))
			printf("rilettura su U29.PC OK (0x%02bx)\n",(U29[PC]));
		else{
			printf("!!! ERRATA rilettura su U29.PC (0x%02bx)\n",(U29[PC]));
			err_cnt++ ;
			}
    	
		mask <<=1;
		printf("premi <INVIO>\n");	while ( (c = getchar())!= 0x0A) ;
	}

    U26[PB] = 0xff;	// spegne i led
	U26[PB] &= ~mask;  //pone a 0 il bit i-simo
	printf("\nTest U26.PB6 (0x%02bx)- Verificare accensione U13 led %bd\n",U26[PB],8);
	if(ins_stato_led() != 0)
		err_cnt++ ;

//	printf("premi <INVIO>\n");while ( (c = getchar())!= 0x0A) ;

	mask <<=1;

    U26[PB] = 0xff;	// spegne i led
	U26[PB] &= ~mask;  //pone a 0 il bit i-simo
	printf("\nTest U26.PB7 (0x%02bx)- Verificare accensione U13 led %bd\n",U26[PB],9);
	if(ins_stato_led() != 0)
		err_cnt++ ;

//	printf("menu <INVIO>\n");while ( (c = getchar())!= 0x0A) ;
	if (err_cnt!=0) return 1;
	return 0;
}

unsigned char  test_U26_PC()	  // test pilotaggio relè
{
	unsigned char ret;
	ret =0;
	printf("\n\nTEST U26 porta C\n");
	U26[PC] = 0x00;	// init 
	mask = 0x01;
	
	for(i=0; i<8; i++) //
	{
		U26[PC] = mask;
		printf("\nVerificare accensione U12 led %bd",i+2);
    	mask <<=1;
		ret =ret + ins_stato_led();
		//while ( (c = getchar())!= 0x0A) ;
	}
	return ret;
}

unsigned char  test_U29_PA()	  // test pilotaggio led e rilettura
{
	static unsigned char err_cnt=0;
	printf("\nTEST U29 porta A con rilettura su U26 porta A\n");
   	error_PA =0;
	mask = 1;

	buff_U38 = 0x80;// per abilitare l'uscita di PA0
	U29[PA] = 0x01; //PA0 è invertito
	printf("\nVerificare accensione dei led U14 (U29 PA = 01H) \n");
	if( (U26[PA]& 0x3F )==0x3f )
		printf("rilettura su U26.PA OK (0x%02bx)\n",(U26[PA]));
	else{
		printf("!!! ERRATA rilettura su U26.PA (0x%02bx)\n",(U26[PA]));
		err_cnt++ ;
		}
	printf("premi <INVIO>\n");while ( (c = getchar())!= 0x0A) ;


	buff_U38 = 0x80;// per abilitare l'uscita di PA0
	U29[PA] = 0xFE; //PA0 è invertito
	printf("\nVerificare spegnimento dei led U14 (U29 PA = FEH)\n");
	if( (U26[PA]& 0x3F )==0x00 )
		printf("rilettura su U26.PA OK (0x%02bx)\n",(U26[PA]));
	else{
		printf("!!! ERRATA rilettura su U26.PA (0x%02bx)\n",(U26[PA]));
		err_cnt++ ;
		}
	printf("premi <INVIO>\n");	while ( (c = getchar())!= 0x0A) ;

	// TEST PA0 =
	buff_U38 = 0x80;// per abilitare l'uscita di PA0
    U29[PA] = 0xFF;	// spegne i led	 , accende su U14 led 2
	i=0;
	printf("\nTest U29.PA%bd (0x%02bx) - Verificare accensione U14 led %bd\n",i,U29[PA],i+2);
	if(ins_stato_led() != 0)
		err_cnt++ ;
//	printf("premi <INVIO>\n");	while ( (c = getchar())!= 0x0A) ;

	buff_U38 = 0x80;// per abilitare l'uscita di PA0
    U29[PA] = 0xFE;	// spegne i led
	i=0;
	printf("\nTest U29.PA%bd (0x%02bx) - Verificare spegnimento U14 led %bd\n",i,U29[PA],i+2);
	if(ins_stato_led() != 0)
		err_cnt++ ;
//	printf("premi <INVIO>\n");	while ( (c = getchar())!= 0x0A) ;

	mask = 0x02;
	mask2 = 0x01;
	for(i=1; i<8; i++) //
	{
	    U29[PA] = 0xFE;	// spegne i led
		U29[PA] &= ~mask;  //pone a 0 il bit i-simo
		printf("\nTest U29.PA%bd (0x%02bx) - Verificare accensione U14 led %bd\n",i,U29[PA],i+2);
		if(i<7){		
			if( U26[PA] ==  mask2)
				printf("rilettura su U26.PA OK (0x%02bx)\n",(U26[PA]));
			else{
				printf("!!! ERRATA rilettura su U26.PA (0x%02bx)\n",(U26[PA]));
				err_cnt++ ;
				}
    	}
		mask <<=1;
		mask2 <<=1;
		printf("premi <INVIO>\n");while ( (c = getchar())!= 0x0A) ;
	}

	if( (U26[PA]&0x40) == 0)
		printf("lettura pin U26.PA6 OK (0x%02bx)\n",(U26[PA]));
	else{
		printf("!!! ERRATA lettura su U26.PA6 (0x%02bx)\n",(U26[PA]));
		err_cnt++ ;
	}
	esito_test_sw1 =0;
	esito_test_sw1 = test_SW1();
	
   	if (err_cnt!=0  || esito_test_sw1) return 1;//test KO
	return 0; //test OK

}



unsigned char  test_SW1()
{
	static unsigned char err_cnt;

	err_cnt =0;
	esito_test_sw1 =0;
	printf("\n\nTEST  SW1\n");

	printf("\nPosizionare SW1 su ON e ");
	printf("premi <INVIO>\n");while ( (c = getchar())!= 0x0A) ;
	
	if( (U26[PA]& 0x80) ==0x80 )
		printf("rilettura su U26.PA7 OK (0x%02bx)\n",(U26[PA]));
	else{
		printf("!!! ERRATA rilettura su U26.PA7 (0x%02bx)\n",(U26[PA]));
		err_cnt=1 ;
	}
	
	printf("\nPosizionare SW1 su OFF e ");
	printf("premi <INVIO>\n");while ( (c = getchar())!= 0x0A) ;
	
	if( (U26[PA]& 0x80 )!= 0x80)
		printf("rilettura su U26.PA7 OK (0x%02bx)\n",(U26[PA]));
	else{
		printf("!!! ERRATA rilettura su U26.PA7 (0x%02bx)\n",(U26[PA]));
		err_cnt=2 ;
	}

	return err_cnt;

}

unsigned char  test_U38()	  // test pilotaggio led e rilettura
{
	static unsigned char err_cnt;
	printf("\n\nTEST U38\n");
   	err_cnt = 0;

	buff_U38 = 0x00;//  led	 on
	printf("Verificare accensione dei led U15 (U38 = 0x00) \n");
	
	if( U29[PB] == 0xFF )
		printf("rilettura su U29.PB OK (0x%02bx)\n",(U29[PB]));
	else{
		printf("!!! ERRATA rilettura su U29.PB (0x%02bx)\n",(U29[PB]));
		err_cnt++;
	}
	printf("premi <INVIO>\n");while ( (c = getchar())!= 0x0A) ;

	buff_U38 = 0xFF;//  led	 off
	printf("Verificare spegnimento dei led U15 (U38 = 0xff) \n");
	if( U29[PB] == 0x00 )
		printf("rilettura su U29.PB OK (0x%02bx)\n",(U29[PB]));
	else{
		printf("!!! ERRATA rilettura su U29.PB (0x%02bx)\n",(U29[PB]));
		err_cnt++;
	}
	printf("premi <INVIO>\n");while ( (c = getchar())!= 0x0A) ;

	mask = 0x01;
	for(i=0; i<8; i++) 
	{
		buff_U38 = 0xFF;//  led	 off
		buff_U38 = 0xFF & (~mask) ;  // pone a 0 il bit i-simo
		printf("Test U38.P%bd (0x%02bx) - Verificare accensione su U15 del LED %bd\n",(19-i),(0xFF & (~mask)),(i+2));
		if( U29[PB] == mask)
				printf("rilettura su U29.PB OK (0x%02bx)\n",(U29[PB]));
			else{
				printf("!!! ERRATA rilettura su U29.PB (0x%02bx)\n",(U29[PB]));
				err_cnt++;
			}
		mask <<=1;
		printf("premi <INVIO>\n");while ( (c = getchar())!= 0x0A) ;
	}
   	if (err_cnt!=0 ) return 1;//test KO
	return 0; //test OK

}



unsigned char  test_U15() //test porta OUT da P1 del micro
{
	static unsigned char err_cnt;
	err_cnt =0;

	printf("\n\nTEST U15 porta P1 \n");
	P1_3 = 1;
	P1_4 = 0;
	P1_5 = 0;
	P1_6 = 0;
	P1_7 = 1;
	printf("\nVerifica accensione U11 led 2");
    if(ins_stato_led()!=0) err_cnt++;
//	while ( (c = getchar())!= 0x0A) ;
	
	P1_3 = 0;
	P1_4 = 1;
	printf("\nVerifica accensione U11 led 3");
    if(ins_stato_led()!=0) err_cnt++;
//    while ( (c = getchar())!= 0x0A) ;

	P1_4 = 0;
	P1_5 = 1;
	printf("\nVerifica accensione U11 led 4");
    if(ins_stato_led()!=0) err_cnt++;
//    while ( (c = getchar())!= 0x0A) ;

	P1_5 = 0;
	P1_6 = 1;
	printf("\nVerifica accensione U11 led 5");
    if(ins_stato_led()!=0) err_cnt++;
//    while ( (c = getchar())!= 0x0A) ;

	P1_6 = 0;

	buff_U37 = 1;
	printf("\nVerifica accensione U11 led 6");
    if(ins_stato_led()!=0) err_cnt++;
//    while ( (c = getchar())!= 0x0A) ;
	buff_U37 = 0;

	P1_7 = 1;
	printf("\nVerifica accensione U11 led 7");
	P1_7 = 0;
    if(ins_stato_led()!=0) err_cnt++;
//    while ( (c = getchar())!= 0x0A) ;
   	if (err_cnt!=0 ) return 1;//test KO
	return 0; //test OK
}



static unsigned char test_led_OFF()
{
	esito_test_led_off =0;
	buff_U38 = 0xff;  // led spenti
//	printf("U38 %bx\n",buff_U38);
	DELAY;
	buff_U37 = 0;
//	printf("U37 %bx\n",buff_U37);
	P1_3 = 0;
	P1_4 = 0;
	P1_5 = 0;
	P1_6 = 0;
	P1_7 = 0;
//	printf("P1_3..7 = 0\n");

	U26[PB] = 0xff;
//	printf("U26_PB =%bx\n",(unsigned char *)U26[PB]);

	U26[PC] = 0x00;
//	printf("U26_PC =%bx\n",(unsigned char *)U26_PC);

	U29[PA] = 0xfe;
//	printf("U29_PA =%bx\n",(unsigned char *)U29_PA);

	printf("\n");
	printf("Verificare che tutti i led siano spenti tranne led 7 di U11\n");
	return ( ins_stato_led());//(0=OK 1= KO)
    //while ( (c = getchar())!= 0x0A) ;
 
}


static unsigned char test_led_ON()
{
	esito_test_led_on =0;
	buff_U38 = 0x00;  // led spenti
//	printf("U38 %bx\n",(unsigned char *)buff_U38);
	buff_U37 = 1;
//	printf("U37 %bx\n",(unsigned char *)buff_U37);
	P1_3 = 1;
	P1_4 = 1;
	P1_5 = 1;
	P1_6 = 1;
	P1_7 = 0;
//	printf("P1_3..7 = 0\n");

	U26[PB] = 0x00;
//	printf("U26_PB =%bx\n",(unsigned char *)U26_PB);

	U26[PC] = 0xff;
//	printf("U26_PC =%bx\n",(unsigned char *)U26_PC);

	U29[PA] = 0x00;
//	printf("U29_PA =%bx\n",(unsigned char *)U29_PA);

	printf("\n");
	printf("Verificare che tutti i led siano accesi\n");
	return ( ins_stato_led());//(0=OK 1= KO)
}


/*
void print_pio_port ()
{
	printf("    bit 7654 3210\n");
	printf("U26  PA "),print_char_bin(*U26_PA),printf("\n");
	printf("U26  PB "),print_char_bin(*U26_PB),printf("\n");
	printf("U26  PC "),print_char_bin(*U26_PC),printf("\n");

	printf("    bit 7654 3210\n");
	printf("U29  PA "),print_char_bin(*U29_PA),printf("\n");
	printf("U29  PB "),print_char_bin(*U29_PB),printf("\n");
	printf("U29  PC "),print_char_bin(*U29_PC),printf("\n");

}

*/									  
unsigned char ins_stato_led()
{
   char e;
   do{
	   printf("\n(1=0K 0=KO ):");
	   e = toupper(getchar());
   }
   while ((e != '1')&&(e != '0'));

   if (e == '0') return 1; //KO
   return 0; //OK

}
void test_pio ()      
{
	printf("\n\n*****  Test I/O   *****   \n\n");
	error = 0;
	
	printf("\n");
	pio_reset();	// azzera tutti i registri
    pio_conf();		// conf registri

	esito_test_led_on  = 0;
	esito_test_led_off = 0;
	esito_test_u26_pc  = 0;
	esito_test_u26_pb  = 0;
	esito_test_u29_pa  = 0;
	esito_test_u38     = 0;
	esito_test_u15     = 0;

	esito_test_led_on  = test_led_ON();
 	esito_test_led_off = test_led_OFF();

	esito_test_u26_pc  = test_U26_PC();
	esito_test_u26_pb  = test_U26_PB();
	esito_test_u29_pa  = test_U29_PA();
	esito_test_u38     = test_U38();
	esito_test_u15     = test_U15();

	printf ("\n");
	if(!esito_test_led_on   &&
	   !esito_test_led_off  &&
	   !esito_test_u26_pb   &&
	   !esito_test_u26_pc   &&
	   !esito_test_u29_pa   &&
	   !esito_test_u38      &&
	   !esito_test_u15)
		printf("\nTest I/O OK\n");
	else
	{ 
		printf("\nTest I/O KO !! \n" );
		if(esito_test_led_on)	printf ("Test led ON  KO\n");
	   	if(esito_test_led_off)  printf ("Test led OFF KO\n");
	    if(esito_test_u26_pb)   printf ("Test U26 PB  KO\n");
	    if(esito_test_u26_pc)   printf ("Test U26 PC  KO\n");
	    if(esito_test_u29_pa)   printf ("Test U29 PA  KO\n");
	  	if(esito_test_u38)      printf ("Test U38     KO\n");
	   	if(esito_test_u15)		printf ("Test U15     KO\n");

	}
	printf("Premere un tasto per proseguire.\n");
    while ( (c = getchar()) !=0x0a) ;
	
	
/**	
	test_pio_conf();
	led_OFF();
	led_ON();
	print_pio_port();
/*
	appoggio =	 *U26_CR;
	printf("U26  CR %bx\n",appoggio);
	appoggio =	(char) *U29_CR;
	printf("U29  CR %bx\n",appoggio);

	printf("\n");
	printf("Premere INVIO per proseguire con 1-shifting sulle USCITE U26_PB\n");
    while ( (c = getchar())!= 0x0A) ;

	shift_test(U29_PA,U26_PA );
	   /*shift di 1 sulle uscite U29 PA*
	error = 0;
	U29_PA = 0; 
	DELAY;		
	if(*U29_PA != *U29_PA) // ??
	{
		error = 1;
		printf("Test bit%bd  error:%bd   out:%bx  in:%bx \n",cont,error,*U29_PA,*U29_PA); 
	}

	*U29_PA = 1;
	cont = 1;
	exit =0;

	start_ritardo = get_timer0(); 
    do
	{
		if((get_timer0() - start_ritardo) == 16 )
		{// eseguito ogni 10 * 50ms
			if(cont<8)
			{
				*U29_PA <<= 1; 
				DELAY;		
				if(*U29_PA != *U29_PA) // ??
				{
					error = 1;
					printf("Test bit%bd  error:%bd   out:%bx  in:%bx \n",cont,error,*U29_PA,*U29_PA); 
				}
				cont++;
			}
			else
				exit =1;
			start_ritardo = get_timer0();
		}
		printf("U29  PA "),print_char_bin(*U29_PA),printf("\r");
	}
	while ( !exit) ;

	printf("\n");
	
	cont = 0;
	*U26_PB=0;
	DELAY;		
	if(*U26_PB != *U26_PB) // ??
	{
		error = 2;
		printf("Test bit%bd  error:%bd   out:%bx  in:%bx \n",cont,error,*U26_PB,*U26_PB); 
	}

	*U26_PB = 1;
	exit =0;
	cont = 1;
	start_ritardo = get_timer0(); 
    do
	{
		if((get_timer0() - start_ritardo) == 16 )
		{// eseguito ogni 10 * 50ms
			if(cont<8)
			{
				*U26_PB <<= 1; 
				DELAY;		
				if(*U26_PB != *U26_PB) // ??
				{
					error = 2;
					printf("Test bit%bd  error:%bd   out:%bx  in:%bx \n",cont,error,*U26_PB,*U26_PB); 
				}
				cont++;
			}
			else
			{
				exit =1;
			}
			start_ritardo = get_timer0();
		}
		printf("U26  PB "),print_char_bin(*U26_PB),printf("\r");
	}
	while ( !exit) ;

	printf("\n\n");

	cont = 0;
	*U26_PC=0;
	DELAY;		
	if(*U26_PC !=*U26_PC) // ??
	{
		error = 3;
		printf("Test bit%bd  error:%bd   out:%bx  in:%bx \n",cont,error,*U26_PC,*U26_PC); 
	}

	cont = 1;
	*U26_PC = 1;
	exit =0;
	start_ritardo = get_timer0(); 
    do
	{
		if((get_timer0() - start_ritardo) == 16 )
		{// eseguito ogni 10 * 50ms
			if(cont<8)
			{
				*U26_PC <<= 1; 
				DELAY;		
				if(*U26_PC!= *U26_PC) // ??
				{
					error = 3;
					printf("Test bit%bd  error:%bd   out:%bx  in:%bx \n",cont,error,*U26_PC,*U26_PC); 
				}
				cont++;
			}
			else
			{
				exit =1;
			}
			start_ritardo = get_timer0();
		}
		printf("U26  PC "),print_char_bin(*U26_PC),printf("\r");
	}
	while ( !exit) ;
	
	printf("\n");
	printf("Verifiva accensione LED USCITA U36 1 shifting\n");
	printf("\n");

	cont = 0;
	*buff_U37 = 1;
	start_ritardo = get_timer0(); 
	exit =0;
    do
	{
		if((get_timer0() - start_ritardo) == 16 )
		{// eseguito ogni 10 * 50ms
			if(cont<=7)
			{
				*buff_U37 <<= 1; 
				cont++;
			}
			else
			{
				cont = 1;
				exit =1;
			}
			start_ritardo = get_timer0();

		}
		printf("U37 (b7..b0)  "),print_char_bin((unsigned char *)buff_U37),printf("\r");
	}
	while ( !exit) ;

	printf("\n");
	printf("Verifiva accensione LED USCITA U36\n");
	printf("\n");

	cont = 0;
	*buff_U38 =1;
	start_ritardo = get_timer0(); 
	exit =0;
    do
	{
		if((get_timer0() - start_ritardo) == 16 )
		{// eseguito ogni 10 * 50ms
			if(cont<=7)
			{
				*buff_U38 <<= 1; 
				cont++;
			}
			else
			{
				cont = 1;
				exit =1;
			}
			start_ritardo = get_timer0();

		}
		printf("U38 (b7..b0)  "),print_char_bin((unsigned char *)buff_U38),printf("\r");
	}
	while ( !exit) ;
 

	printf("\n");








	if (error) 	printf("Test PIO non riuscito (error %bd)\n\n",error);
*/
}