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
	unsigned char xdata * ADDR;  /* generic pointer */
	unsigned short adr;
	unsigned short appo;
	


	printf("\nTest RAM ESTERNA (0x0000 .. 0x1FFF)\n\n");
	printf("Write pattern 0x55 at \n");
    err=0;
	/* scrittura pattern AA55*/
	for( ADDR=RAM_START; ADDR<RAM_END; ADDR++)
	{
		adr = (unsigned short)ADDR ;
		printf("%04X \r",adr );

		*ADDR=0x55;
	}
	
	printf("\nVerify at \n");
	/* lettura pattern 55 */
	for( ADDR=RAM_START; ADDR<RAM_END; ADDR++)
	{
		adr = (unsigned short)ADDR ;
		printf("%04X \r",adr );

 	    if(*ADDR!=0x55) 
			{
				printf("test RAM KO %04X @ read %04x\n",ADDR,*ADDR);
				err=1;
				break; 	 //set P1.7
			}
	}


	/* scrittura pattern AA*/
	printf("\nWrite pattern 0xAA at \n");
	for( ADDR=RAM_START; ADDR<RAM_END; ADDR++)
	{
		adr = (unsigned short)ADDR ;
		printf("%04X \r",adr );

		*ADDR=0xAA;
	}

	/* lettura pattern AA */
	printf("\nVerify at \n");
	for( ADDR=RAM_START; ADDR<RAM_END; ADDR++)
	{
		adr = (unsigned short)ADDR ;
		printf("%04X \r",adr );

 	    if(*ADDR!=0xAA) 
			{
				printf("test RAM KO %lX @ read %04x\n",ADDR,*ADDR);
				err=2;
				break; 	 //set P1.7
			}
	}
	
	printf("\nTest 1-shifting on A0..A12 \n");
	
	ADDR = 0x0000;
	appo = ADDR+1;
	j=0;
	while (appo<0x2000) 
	{
		adr = (unsigned short)ADDR ;
		printf("%04X (A%d) ",adr,j++ );

		*ADDR = 0x55;
		
		for(i=0; i<255; i++);
		
		if (*ADDR == 0x55)
			printf(" ..ok\n");
		else	
			printf("..KO %lx\n",*ADDR), err=3;
		
		appo <<= 1;
		ADDR = (unsigned char xdata *)appo;
	}

	if(!err)
		printf("Test RAM OK\n");
	else 
		printf("Test RAM KO error  %d\n",err);
}

