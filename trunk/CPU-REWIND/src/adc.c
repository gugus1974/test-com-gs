/*************************************************************************************/
/* 04/01/10 1.04.13             \SRC\adc.c                     */
/*************************************************************************************/

#include "stdlib.h"
#include "reg51.h"
#include "stdio.h"
#include <string.h>
#include "adc.h"
#include "pio.h"
/*
	clock 614.06 KHz ->  1.63us
*/
/*************************************************************************************/
#define SAMPLE 10
#define ADC_ADDR_START  0xA000
#define	EPS  13 //~5% di fondoscala
/*************************************************************************************/
sbit EOC = P1^2;

/*************************************************************************************/
static unsigned char  val_atteso[8] ={		0xB0,//IN0 3.46 Volt 
											0x5B,//IN1 1.8	 //
											0x59,//IN2 1.76  //
											0x8E,//IN3 2.8 	 //
											0x84,//IN4 2.6 	 //
											0xb5,//IN5 3.56 	 //
											0, 
											0};
static char result[3];

/*************************************************************************************/

static unsigned short  canale[8];
static unsigned short  xdata esito[8];
unsigned char xdata *adc;
//volatile unsigned char xdata  adc _at_ ADC_ADDR_START;  /* generic pointer */

unsigned char num_s;

unsigned char i=0;
unsigned char k=0,s;
unsigned short appo;
char c;
extern unsigned char tipo_scheda;

/*************************************************************************************/

void test_adc(void);

/*************************************************************************************/
unsigned char test_adc_canale(unsigned char i,unsigned char val)
{
	unsigned char ret;
	// inizializza var lettura
	canale[i]=0;
   	ret =0;
	for(num_s=1 ;num_s<=SAMPLE; num_s++)
    {
        adc = ADC_ADDR_START + i;
        *adc = 0x7f;	// comanda il s.o.c. all' ADC
        
		while(!EOC) /*printf("*")*/; //attende la fine della conversione

		s  = *adc; //lettura tramite buffer

		printf("ch: %bd   ",i);
		printf("sample n° %bd   ",num_s);
		printf("lettura: 0x%bX (%1.2f Volt) \n",(*adc), *adc *0.0196 );
       
        canale[i] = canale[i] + *adc ;
    }
    canale[i] = canale[i] / SAMPLE;
	printf("ch: %bd  lettura 0x%X   (%1.2f Volt)\n", i, canale[i],(canale[i]* 0.0196));

	if( abs(canale[i] - val) <= EPS) 
		  {strcpy (result, "OK"+'\0'); ret = 0;}
	else
		  {strcpy (result, "KO" +'\0'); ret = 1;}
	printf("Esito test ch IN%bd = 0x%02X   lettura: %1.2fV  Val atteso: %1.2fV    ",i,canale[i],(canale[i]* 0.0196),(unsigned short)val* 0.0196);
	printf("esito: %s\n",result);
	return (ret);
}
void test_adc(void)
{
//	static char s;
	int ritardo;
//	char del;

//	static unsigned char exit, scelta;
	for(i=0; i<8; i++) esito[i]=0;
  	printf("\n\n*****  Test ADC  *****   \n\n");
/*	do
	{
	  	printf("Selezionare il tipo di scheda :\n");
	  	printf("   1 - DG630-12\n");
	  	printf("   2 - DG630-29\n\n");
		printf("     Scelta: ");
		s = _getkey();
		printf("\n");
		exit =0;	
		scelta = s-0x30;
		if(scelta>0 && scelta<3)
		{
		   if(tipo_scheda == 1) 
		   	val_atteso[5] =0;
		   else 
		   	val_atteso[5] =0xb5; 
		   exit =1;
		}
	}
	while(!exit);
*/
    if(tipo_scheda == 1) 
	   	val_atteso[5] =0;
    else 
	   	val_atteso[5] =0xb5; 


	printf("\nPosizionare lo switch NOISE al centro e\n");
	printf("verificare che i led S1 e S2 siano spenti\n");
	printf("Premere INVIO per proseguire \n");while ( (c = getchar())!= 0x0A) ;
	printf("\nTest canale IN0\n");

	for( ritardo=0; ritardo<5000; ritardo++); 
	for( ritardo=0; ritardo<10000; ritardo++); 
	for( ritardo=0; ritardo<10000; ritardo++); 
	for( ritardo=0; ritardo<10000; ritardo++); 
	for( ritardo=0; ritardo<10000; ritardo++); 
	for( ritardo=0; ritardo<10000; ritardo++); 
	for( ritardo=0; ritardo<10000; ritardo++); 
	for( ritardo=0; ritardo<10000; ritardo++); 
	for( ritardo=0; ritardo<10000; ritardo++); 
	for( ritardo=0; ritardo<10000; ritardo++); 
	for( ritardo=0; ritardo<10000; ritardo++); 
	for( ritardo=0; ritardo<10000; ritardo++); 
	for( ritardo=0; ritardo<10000; ritardo++); 
	for( ritardo=0; ritardo<10000; ritardo++); 
	for( ritardo=0; ritardo<10000; ritardo++); 
	for( ritardo=0; ritardo<10000; ritardo++); 
	for( ritardo=0; ritardo<10000; ritardo++); 
	for( ritardo=0; ritardo<10000; ritardo++); 
	for( ritardo=0; ritardo<10000; ritardo++); 
	for( ritardo=0; ritardo<10000; ritardo++); 
	for( ritardo=0; ritardo<10000; ritardo++); 



	esito[0] = test_adc_canale(0, 0);

	if (esito[0]) esito[0]<<=1;
	printf("Premere INVIO per proseguire \n");while ( (c = getchar())!= 0x0A) ;
	printf("\nTest canale IN1\n");
	for( ritardo=0; ritardo<5000; ritardo++); 
	esito[1] = test_adc_canale(1, 0);

	if (esito[1]) esito[1]<<=1;
	printf("Premere INVIO per proseguire \n");while ( (c = getchar())!= 0x0A) ;
	printf("\nTest canale IN2\n");
	for( ritardo=0; ritardo<5000; ritardo++); 
	esito[2] = test_adc_canale(2, 0);

	if (esito[2]) esito[2]<<=1;

	printf("Premere INVIO per proseguire \n");while ( (c = getchar())!= 0x0A) ;

    for(i=0; i<8; i++) //in0..in7
    {
		printf("\nTest IN%bd\n",i);
		if(i == 0) 
			printf("Posizionare lo switch NOISE su NOISE1 e verificare cke il led S1 sia acceso (test IN%bd)\n",i);
		else if(i == 1) 
			printf("Posizionare lo switch NOISE su NOISE1 (test IN%bd)\n",i);
		else if(i == 2) 
			printf("Posizionare lo switch NOISE su NOISE2 e verificare cke il led S2 sia acceso (test IN%bd)\n",i);
		printf("Premere INVIO per proseguire \n");while ( (c = getchar())!= 0x0A) ;

		if(i < 2 ) 
		{
			for( ritardo=0; ritardo<10000; ritardo++); 
			for( ritardo=0; ritardo<10000; ritardo++); 
			for( ritardo=0; ritardo<10000; ritardo++); 
			for( ritardo=0; ritardo<10000; ritardo++); 
			for( ritardo=0; ritardo<10000; ritardo++); 
			for( ritardo=0; ritardo<10000; ritardo++); 
			for( ritardo=0; ritardo<10000; ritardo++); 
			for( ritardo=0; ritardo<10000; ritardo++); 
			for( ritardo=0; ritardo<10000; ritardo++); 
			for( ritardo=0; ritardo<10000; ritardo++); 
			for( ritardo=0; ritardo<10000; ritardo++); 
			for( ritardo=0; ritardo<10000; ritardo++); 
			for( ritardo=0; ritardo<10000; ritardo++); 
			for( ritardo=0; ritardo<10000; ritardo++); 
			for( ritardo=0; ritardo<10000; ritardo++); 
			for( ritardo=0; ritardo<10000; ritardo++); 
			for( ritardo=0; ritardo<10000; ritardo++); 
			for( ritardo=0; ritardo<10000; ritardo++); 
			for( ritardo=0; ritardo<10000; ritardo++); 
			for( ritardo=0; ritardo<10000; ritardo++); 
		}
		esito[i] |= test_adc_canale(i, val_atteso[i]);
		
		printf("Premere INVIO per proseguire \n");
	    while ( (c = getchar())!= 0x0A) ;
    }

	printf("Riepilogo test ADC\n");

    for(i=0; i<8; i++)
    {
		if(( abs(canale[i] - val_atteso[i]) <= EPS) && (esito[i]&0x02)==0) 
			  strcpy (result, "OK"+'\0');
		else
			  strcpy (result, "KO" +'\0');
		printf("Test IN%bd = 0x%02X   lettura: %1.2fV  Val atteso: %1.2fV    ",i,canale[i],(canale[i]* 0.0196),((unsigned short)val_atteso[i])* 0.0196);
		printf("esito: %s\n",result);
    }
	getchar();
    printf("\n\n");
}
