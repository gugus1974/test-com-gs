/***************************************************************************/
/* Test RAM
Questa procedura è chiamata direttamente da STARTDEV.A66 dentro di RAM2ROM2.INC
/***************************************************************************/
#include <reg51.h>
#include <stdio.h>
#include "test_ram.h"
#include "util.h"
/*********************************/
/* Definizione Variabili Globali */
/*********************************/
//volatile unsigned short xdata * ADDR _at_ RAM_START;  /* generic pointer */
unsigned short xdata * ADDR ;  /* generic pointer */

/*********************************/
/* Extern Variabili              */
/*********************************/
/*********************************/
/* Prototipi Funzioni di Modulo  */
/*********************************/
void test_ram(void);

/*********************************/
/*Definizione Variabili di Modulo*/
/*********************************/
static unsigned char i=0,j =0;
static unsigned char err =0;


/**************************/
/* Funzioni di Modulo     */
/**************************/

void test_ram(void)
{
	unsigned short adr;
	unsigned short appo;
	char c;


	printf("\nTest RAM ESTERNA (0x0000 .. 0x1FFF)\n\n");
	printf("Write pattern 0xAA55  \n");
    err=0;
	for( ADDR=RAM_START; ADDR<RAM_END; ADDR++)
	{
		adr = (unsigned short)ADDR ;
		printf("@%04X \r",adr );

		*ADDR=0xAA55;
	}
	
	printf("\nVerify at \n");
	for( ADDR=RAM_START; ADDR<RAM_END; ADDR++)
	{
		adr = (unsigned short)ADDR ;
		printf("%04X \r",adr );

 	    if(*ADDR!=0xAA55) 
			{
				printf("test RAM KO %04X @ read %04x\n",ADDR,*ADDR);
				err=1;
				break; 	 //set P1.7
			}
	}


	/* scrittura pattern AA*/
	printf("\nWrite pattern 0x55AA at \n");
	for( ADDR=RAM_START; ADDR<RAM_END; ADDR++)
	{
		adr = (unsigned short)ADDR ;
		printf("%04X \r",adr );

		*ADDR=0x55AA;
	}

	/* lettura pattern AA */
	printf("\nVerify at \n");
	for( ADDR=RAM_START; ADDR<RAM_END; ADDR++)
	{
		adr = (unsigned short)ADDR ;
		printf("%04X \r",adr );

 	    if(*ADDR!=0x55AA) 
			{
				printf("test RAM KO %lX @ read %04x\n",ADDR,*ADDR);
				err=2;
				break; 	 //set P1.7
			}
	}
	
	printf("\nTest 1-shifting on A0..A12 \n");
	
	ADDR = 0x0000;
	*ADDR = 0xAA55;
	printf("%04X (0x0000) ",(unsigned short)ADDR );
	if (*ADDR == 0xAA55)
		printf(" ..ok\n");
	else	
		printf("..KO %lx\n",*ADDR), err=3;

	for(i=0; i<255; i++);

	j=0;
	ADDR= 0x0001;
	appo = 0x0001;
	while (appo<0x2000) 
	{
		adr = (unsigned short)ADDR ;
		printf("%04X  (A%bd) ",((unsigned short)ADDR),j++ );

		*ADDR = 0xAA55;
		
		for(i=0; i<255; i++);
		
		if (*ADDR == 0xAA55)
			printf(" ..ok\n");
		else	
			printf("..KO %u\n",*ADDR), err=3;
		
		appo <<= 1;
		ADDR = (unsigned short xdata *)appo;
	}

	if(!err)
		printf("Test RAM OK\n");
	else 
		printf("Test RAM KO error  %bd\n",err);

	printf("Premere un tasto per proseguire.\r\n");
    while ( (c = getchar()) !=0x0a) ;
}

