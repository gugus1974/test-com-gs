/*********************************************************************************
* 20/03/13                                             v.1.00        tsteeprom.c *
**********************************************************************************
* Funzioni per il test della EEPROM                                              *
**********************************************************************************
* 					                                                             *
*********************************************************************************/
#include <stdio.h>
#include <string.h>
#include "hw_com.h"
#include "tsteprom.h"
#include "x25642.h"

extern volatile QUICC	*quicc;				/* pointer to the QUICC DP-RAM structure */



short tstEEPROM(short argc, char *argv[] )
{
    unsigned char status = 0;
    unsigned char buf[256];
    unsigned char vrfbuf[256];
    unsigned int i,j;                        
    unsigned char error= TSTEPROM_OK;
 	volatile QUICC 	*q;
 	
 	q=quicc;

 	if (q->pip_pbdat & (1<<17))//test 
 	{
     	/*lettura dello STATUS REGISTER*/
       	if ((x25642_read_status(&status))== X25642_OK){
       	    error=TSTEPROM_OK;
       	    
       	    printf("X25642 status byte: %02X(", status);
            if (status & WPEN_BIT)  printf("WPEN ");
            if (status & BP1_BIT)   printf("BP1 ");
            if (status & BP0_BIT)   printf("BP0 ");
            if (status & WEL_BIT)   printf("WEL ");
            if (status & WIP_BIT)   printf("WIP ");
            printf(")\n"); 
        
            /*scrittura di 32 byte*/
            for (i=0;i<256;i++) buf[i]=i;
          	
            //EEPROM size 8K= 256 * 32pages
            for (j=0;j<32;j++){
             	/*write 256 byte */
             	if ((x25642_write((unsigned short)(j*256), buf, 256))== X25642_OK)
                    printf("IC18 writing ...%d\r",j); 
                else{
                    error=TSTEPROM_WRKO;
                    printf("\nKO\n"); 
                    break;
                }
            }   
            
          	/*lettura del buffer*/
            for (j=0;j<32;j++){
                if ((x25642_read((unsigned short)(j*256), vrfbuf, 256))==X25642_OK)
                    printf("IC18 verify ...%d\r",j); 
                else{
                    error=TSTEPROM_RDKO;
                    printf("\nKO\n"); 
                    break;
                }
                /* verify del buffer letto*/
                for (i=0;i<256;i++){
                    if(vrfbuf[i]!=(unsigned char)(i)){
                        printf("error verify @%4x atteso %2xh letto %2xh\n",(256*j)+i,buf[i],vrfbuf[i] );
                        error = TSTEPROM_VRFYKO;
                        break;
                    }
                }
            }   
        
        }
        else{
         printf("impossibile leggere a IC18\n");   
        }
    }
    else{
        printf("!!! errore PB17 = 0\n");
    }    
        
    if (!error) 
        printf ("TEST OK\n");
    else
        printf ("TEST KO (%d)\n",error);

    return error;
}
