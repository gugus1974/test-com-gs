/******************************************************************************
* Lun.10.nov.03            v.1.00      \sw_st\sw_stb\tststbxx\source\tstadc.c *
*******************************************************************************
* Routine per test di input analogici                                         *
*******************************************************************************
* 							                      *
******************************************************************************/

#include "stdio.h"
#include "stdlib.h"
#include "libdew.h"
#include "tstflash.h"   
#include <cod_st.h>
	

#pragma ot(3,SPEED)    
		       /* evita che il compilatore commetta errore sulle */
                       /* letture consecutive dell' adc                  */

#define ncamp 		10
#define errmarg         16

/***********************************************************/
/* Effettua test input dei canali adc                      */
/***********************************************************/
void test_adc(void)
{
    int		j,i,lmax,lmin,x,ko,k,er;
    int		ch[16];	
   
    printf("**** TEST ADC 167 ****\r\n");
    printf("Verificare che sul Test point Vref della scheda AUXDEW ci sia un tensione\r\n");
    printf("di 4.096V+/-5mV\r\n");
    printf("Premere invio per proseguire.\r\n");
    while ( CONS_in() != '\r');

    er = 0;	
    ko = 0;
    lmin = 1000 - errmarg;
    lmax = 1000 + errmarg;
    for (j=0; j<=14; j++) {
    	ko = 0;
	printf("Forzare una tensione di 4.000 V sul canale %d\r\n",j+1);
	printf("Premere INVIO per proseguire.\r\n");
       	printf("AN01 AN02 AN03 AN04 AN05 AN06 AN07 AN08 AN09 AN10 AN11 AN12 AN13 AN14 AN15 \r\n");
        while ( CONS_in()  != '\r' ){
	   for (k = 0; k <=14; k++) ch[k] = 0;
 	   for (k = 0; k <=14; k++) {
		for ( i = 0; i < ncamp; i++) {
         		LD6_START_ADC166(k,0);  /*genera lo start of convertion     */
 						/*sul canale j dell'adc interno     */
 			for(x=0; x<100; x++);
         		LD6_WAIT_ADC166;        /* aspetta l'END OF CONVERTION      */
         		ch[k] = ch[k]+(ADDAT & 0x3FF);
		}
		ch[k] = ch[k]/ncamp;
		if ( k != 14 ) printf("%03x  ",ch[k]);
		else printf("%03x\r",ch[k]);
		for(x=0; x < 15000; x++);
	   }
	}
	for (k = 0; k <=14; k++) {
		if ( k == j ) ko = ko | ((ch[k] < lmin) || (ch[k] > lmax));
		else ko = ko | (ch[k] < 1016); 
	}
       	if ( ko)
	{
		er = 1;
		printf("\r\nKO!!!\r\n");
	}
	else printf("\r\nOK\r\n"); 
    }	
    lmin = 512-errmarg;
    lmax = 512+errmarg;
    for (j=0; j<=14; j++) {
    	ko = 0;
	printf("Forzare una tensione di 2.048 V sul canale %d\r\n",j+1);
	printf("Premere INVIO per proseguire.\r\n");
       	printf("AN01 AN02 AN03 AN04 AN05 AN06 AN07 AN08 AN09 AN10 AN11 AN12 AN13 AN14 AN15 \r\n");
        while ( CONS_in()  != '\r' ){
	   for (k = 0; k <=14; k++) ch[k] = 0;
 	   for (k = 0; k <=14; k++) {
		for ( i = 0; i < ncamp; i++) {
         		LD6_START_ADC166(k,0);  /*genera lo start of convertion     */
 						/*sul canale j dell'adc interno     */
 			for(x=0; x<100; x++);
         		LD6_WAIT_ADC166;        /* aspetta l'END OF CONVERTION      */
         		ch[k] = ch[k]+(ADDAT & 0x3FF);
		}
		ch[k] = ch[k]/ncamp;
		if ( k != 14 ) printf("%03x  ",ch[k]);
		else printf("%03x\r",ch[k]);
		for(x=0; x < 15000; x++);
	   }
	}
	for (k = 0; k <=14; k++) {
		if ( k == j ) ko = ko | ((ch[k] < lmin) || (ch[k] > lmax));
		else ko = ko | (ch[k] < 1016); 
	}
       	if ( ko) 
	{
		er = 1;
		printf("\r\nKO!!!\r\n");
	}
	else printf("\r\nOK\r\n"); 
    }	

    for (j=0; j<=14; j++) {
    	ko = 0;
	printf("Forzare una tensione di 0 V sul canale %d\r\n",j+1);
	printf("Premere INVIO per proseguire.\r\n");
       	printf("AN01 AN02 AN03 AN04 AN05 AN06 AN07 AN08 AN09 AN10 AN11 AN12 AN13 AN14 AN15 \r\n");
        while ( CONS_in()  != '\r' ){
	   for (k = 0; k <=14; k++) ch[k] = 0;
 	   for (k = 0; k <=14; k++) {
		for ( i = 0; i < ncamp; i++) {
         		LD6_START_ADC166(k,0);  /*genera lo start of convertion     */
 						/*sul canale j dell'adc interno     */
 			for(x=0; x<100; x++);
         		LD6_WAIT_ADC166;        /* aspetta l'END OF CONVERTION      */
         		ch[k] = ch[k]+(ADDAT & 0x3FF);
		}
		ch[k] = ch[k]/ncamp;
		if ( k != 14 ) printf("%03x  ",ch[k]);
		else printf("%03x\r",ch[k]);
		for(x=0; x < 15000; x++);
	   }
	}
	for (k = 0; k <=14; k++) {
		if ( k == j ) ko = ko | (ch[k] > errmarg);
		else ko = ko | (ch[k] < 1016); 
	}
       	if ( ko) 
	{
		er = 1;
		printf("\r\nKO!!!\r\n");
	}
	else printf("\r\nOK\r\n"); 
    }	

    printf("\r\n");
    if (er) save_stato(TADCKO);
    else    save_stato(TADCOK);

    
}














