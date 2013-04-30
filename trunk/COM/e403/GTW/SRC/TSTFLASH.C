/*********************************************************************************
* Lun.08.set.03           v.1.00        \sw_st\sw_stb\tststbxx\source\tstflash.c *
**********************************************************************************
* Funzioni per il test della flash memory                                        *
**********************************************************************************
* 					                                         *
*********************************************************************************/

#include <stdio.h>
#include <string.h>
#include "cod_st.h"
#include "atr_hw.h"
#include "am29f040.h"
#include "hw_com.h"


/* la flash viene vista come un array di interi */
#define flash1 ((unsigned int  *)(0x402AAA))
#define flash2 ((unsigned int  *)(0x405555))
#define KEY1    0x5555
#define KEY2    0x2AAA
#define am29f040(base, offset) *((( unsigned short  *)(base)) + (offset))

//#define flash2 ((unsigned int  *)(0x405554))

//extern volatile QUICC *quicc;				/* pointer to the QUICC DP-RAM structure */
//static volatile QUICC 	* q;



void save_stato (unsigned short stato);
void prn_stato (unsigned int prg, unsigned int cod);

//void (*f_gest)(void);

/* variabili per dimensione flash impostati ad 1 MB */
unsigned short  * sctstr    = (unsigned short  *)0x400000L;  /*  limite inferiore indirizzi settore (unsigned int  *)0x400000; */
unsigned short  * sectlimit = (unsigned short  *)0x4FE000L;  /*  limite superiore indirizzi settore (unsigned int  *)0xBE0000; */
unsigned long sctdelta                     = 0x10000L;  /*  delta incremento indirizzi settore (unsigned int  *)0x010000; */
unsigned long dimsct                       = 0xFFFE;  /*  dimensione settore settore (unsigned int  *)0x00FFFF; */
unsigned short  * flashlimit =(unsigned short  *)0x48FFFE; /*  limite superiore indirizzi (unsigned int  *)0xBFFFFE; */





static short am29f040_wait(void *base, long offset, short value)
{
    short t;
//    err = am29f040_wait(base, 0, 0xFFFF);

    do
    {
        t   = am29f040(base, offset);
        if ((t & 0x0080) == (value & 0x0080))
        {
            goto pass1;                                 /* CLAMOROSO !!!!!!!!   */
        }
    } while ((t & 0x0020) != 0x0020);

    if ((t & 0x0080) == (value & 0x0080))
    {
        goto pass1;                                     /* CLAMOROSO !!!!!!!!   */
    }

    return AM29F040_FAILURE_ERR;

pass1:
    do
    {
        t   = am29f040(base, offset);

        if ((t & 0x8000) == (value & 0x8000))
        {
            goto pass2;                                 /* CLAMOROSO !!!!!!!!   */
        }
    } while ((t & 0x2000) != 0x2000);

    if ((t & 0x8000) == (value & 0x8000))
    {
        goto pass2;                                     /* CLAMOROSO !!!!!!!!   */
    }

    return AM29F040_FAILURE_ERR;

pass2:
    return AM29F040_OK;
}

 
/**********************************************************/
/* Funzione di erase flash                                */
/* Valore ritornato:   codice errore                      */
/**********************************************************/
int chip_erase(void)
{
	unsigned short data3,data1,i,a,tim1,tim2,ok1,ok2;
	int er;
    char c;
	er = 0;//azzera errore
	printf("ERASE FLASH...");
	/***    procedura di start erase         ***/
    am29f040(sctstr, 0)       = 0xF0F0;//reset
    am29f040(sctstr, KEY1)    = 0xAAAA;//KEY1= 5555
    am29f040(sctstr, KEY2)    = 0x5555;//KEY2= 2AAA
    am29f040(sctstr, KEY1)    = 0x8080;
    am29f040(sctstr, KEY1)    = 0xAAAA;
    am29f040(sctstr, KEY2)    = 0x5555;
    am29f040(sctstr, KEY1)    = 0x1010;
	/***    verifica del fine erase chip     ***/
	data3 = am29f040(sctstr, 0);
	i = 0;//indice
	a = 0;//address
	ok1 = 0;
	ok2 = 0;
	tim1 = 0;//timeout 
	tim2 = 0;
	while (!(ok1 && ok2)) 
	{
		if (!(ok1 || tim1)) {
			if ((data3 & 0x8000) == 0x8000){
				ok1 = 1;
			}
			else {
				if ((data3 & 0x2000) == 0x2000){
					data1 = *flash1;
					if ((data1 & 0x8000) != 0x8000){
						er = 1;
						tim1 = 1;
					}
					else ok1 = 1;
				}
			}
		}
		if (!(ok2 || tim2)) {
			if ((data3 & 0x0080) == 0x0080){
				ok2 = 1;
			}
			else {
				if ((data3 & 0x0020) == 0x0020){
					data1 = *flash1;
					if ((data1 & 0x0080) != 0x0080){
						er = 2;
						tim2 = 1;
					}
					else ok2 = 1;
				}
			}
		}
		if (tim1 || tim2) break;
		
		data3 =  am29f040(sctstr, 0);
		
		if(/*(i==0) ||*/ (i==32000)) {
			printf("%d %05x data=%04x\r",i,a,data3);
			a++;
		}
		
		i++;
        
        if ( (c=_getkey()) == '\t' ) 
	    {
		    er = 3;
	    	break;
			
		}				
//	    printf("\ni=%d ok1=%d  ok2=%d  addr=%05d data=%04x tim1=%d  tim2=%d\n",i,ok1,ok2,a,data3 , tim1, tim2);
    	//printf("%05d data=%04x\r\n",a,data3);
	}
	data3 = *flash1;
//	printf("\n RESET FLASH CHIP\n");
	*flash1 = 0xF0F0;

	return er;
}


/**********************************************************/
/* Funzione di copy ram to flash                          */
/* Variabili di ingresso:                                 */
/* Valore ritornato:   codice errore                      */
/**********************************************************/
int wr (void)
{
	unsigned short dat,data1,i,dt;
	int  er;
	unsigned short  * pt_flash;
	unsigned short  * pt_ram;

	er = 0;
	i=0;
	pt_ram = (unsigned short  *)(0x000000);
	/*** ciclo di srittura e verifica su tutti gli indirizzi ***/
	for ( pt_flash = sctstr; pt_flash <=flashlimit; pt_flash++){
	/***    procedura di start write     ***/
		dt = *pt_ram;
        am29f040(sctstr, KEY1)    = 0xAAAA;
        am29f040(sctstr, KEY2)    = 0x5555;
        am29f040(sctstr, KEY1)    = 0xA0A0;
//        am29f040(sctstr, offset)  = data;
        am29f040(pt_flash, 0)  = dt;

//		*flash2 = 0xAAAA;
//		*flash1 = 0x5555;
//		*flash2 = 0xA0A0;
//		/***   scrittura della flash         ***/
//		*pt_flash = dt;
		/***  attesa fine scrittura           ***/
		er = am29f040_wait(pt_flash, 0, dt);
//		dat = *pt_flash;
//		data1 = *pt_flash;
//		while (dat != data1) {
//			dat = *pt_flash;
//			data1 = *pt_flash;
//            if ( _getkey()  == '\t' ) 
//		    {
//			    er = 4;	
//			    break;
//		    }				
//
//		}

		i++;
		if(i==10000){
			printf(".");
			i = 0;
		}
		if ( dat != dt ) {
			er = 5;
			break;
		}
//		if (pt_ram == (unsigned short  *)(0x00EFFE))
//		{
//			pt_ram = (unsigned short  *)(0x00FFFE);
//			pt_flash = (unsigned short  *)(0x40FFFE);
//		}
		pt_ram++;
	}
	printf("\r\n");
	return er;
}

/**********************************************************/
/* copia la ram sulla flash flash                         */
/**********************************************************/
void ram2flash (void )
{
	int error,i;

	i=i;
	printf("\r\nCopy RAM TO FLASH \r\n");
	error = wr();
//	hw_ram2rom();
	switch (error){
		case 0:
			save_stato (TFLACOPYR2FOK);
			break;
		case 4:
			save_stato (TFLAFZEXITR2F);
			break;
		case 5:
			save_stato (TFLACOPYR2FKO);
			break;
	}
	printf("\r\n");
}



/**********************************************************/
/* Funzione di erase flash per settore                    */
/* Valore ingresso : indirizzo settore                    */
/* Valore ritornato:   codice errore                      */
/**********************************************************/
int chip_erasesect(unsigned short  * addr)
{
	unsigned int data3,data1,i,a,tim1,tim2,ok1,ok2,seg,off;
	int er;
	unsigned long app;	
	unsigned long base;	
    char c;
	er = 0;
	 
	app = (unsigned long) addr ;
	off = (unsigned int)(app & 0x00FFFF);
	app = app >> 16;
	seg = (unsigned int)(app & 0x0000FF);
//	printf("ERASING SECTOR #%d @%02x%04x\n",(app & 0x000007),seg,off);
	/***    procedura di start erase sector         ***/
//    am29f040(sctstr, 0)       = 0xF0F0;
    am29f040(sctstr, KEY1)    = 0xAAAA;//5555
    am29f040(sctstr, KEY2)    = 0x5555;//2AAA
    am29f040(sctstr, KEY1)    = 0x8080;
    am29f040(sctstr, KEY1)    = 0xAAAA;
    am29f040(sctstr, KEY2)    = 0x5555;
    am29f040(addr  ,    0)    = 0x3030;
	/***    verifica del fine erase chip     ***/
	data3 = am29f040(addr  ,    0);
	i = 0;
	a = 0;
	ok1 = 0;
	ok2 = 0;
	tim1 = 0;
	tim2 = 0;
//	data3 = *addr;
	while (!(ok1 && ok2)) {
	    printf("@%06lx data3=%04X\r",addr,data3);
		if (!(ok1 || tim1)) {
			if ((data3 & 0x8000) == 0x8000){
				ok1 = 1;
			}
			else {
				if ((data3 & 0x2000) == 0x2000){
					data1 = *flash1;
					if ((data1 & 0x8000) != 0x8000){
						er = 6;
						tim1 = 1;
					}
					else ok1 = 1;
				}
			}
		}
		if (!(ok2 || tim2)) {
			if ((data3 & 0x0080) == 0x0080){
				ok2 = 1;
			}
			else {
				if ((data3 & 0x0020) == 0x0020){
					data1 = *flash1;
					if ((data1 & 0x0080) != 0x0080){
						er = 7;
						tim2 = 1;
					}
					else ok2 = 1;
				}
			}
		}
		if (tim1 || tim2) break;
		data3 = *addr;
		if((i==0) || (i==32000)) {
			printf("1 %05d data=%04lX\r",a,data3);
			a++;
		}
		i++;
        if ((c=_getkey())  == '\t' ) 
	        {
			er = 8;
			break;
		}				
	}
	data3 = *addr;
	printf("\n");
	return er;
}


/**********************************************************/
/* Funzione di write & verify flash                       */
/* Variabili di ingresso:                                 */
/*   dt           dato da scrivere nella flash            */
/* Valore ritornato:   codice errore                      */
/**********************************************************/
//int wr_vrf(short data)
//{
//	unsigned short  mask;
//	unsigned short data3,data1,ok1,ok2,tim1,tim2,i,data2,j,k,dt,cnt,cnt_dt;
//	int  er;
//	unsigned short  * pt_flash;
//	unsigned short  off,seg;
//	unsigned long app; 
//	char c;
//    
//	printf("WRITE/VERIFY FLASH SHIT 0 nei dati per 1 negli indirizzi\r\n");
//	er = 0;
//	i=60000;
//
//
//	pt_flash = (unsigned short  *)0x400000;//address start flash memory
//    
//	/*** ciclo di scrittura e verifica su tutti gli indirizzi ***/
//	for ( cnt = 1; cnt <21 ; cnt++)
//	{
//	    mask=0xFFFF;
//		for ( cnt_dt = 0; cnt_dt <17 ; cnt_dt++)
//	    {		
//	        data = mask;
//    		k = 10000;
//    		tim1 = 0;
//    		while( (((data3 = *pt_flash) & mask) != mask) && ((c = _getkey())  != '\r') ) 
//    		{    
//                if ( k > 10000) 
//		        {	   
//			        app = (unsigned long) pt_flash;
//        			off = (unsigned int)(app & 0x00FFFF);
//        			app = app >> 16;
//        			seg = (unsigned int)(app & 0x0000FF);
//        			printf("\r1-KO!!! Ad Address %02x%04x Bit %02d atteso 1 letto 0",seg,off, cnt_dt);
//        			k = 0;
//        			tim1 = 1;
//        			er = 13;
//        	    }
//		        k++;		
//       		    if ( c == '\t' )
//    		    {
//        		    er = 9;
//        			break;
//    		    }				
//		    }
//
//			printf("@%6X  %4X\n",pt_flash,mask);
//	        /* azzera bit */
//	        mask ^= (1<<cnt_dt);
//	        //            am29f040(sctstr, 0)       = 0xF0F0;//KEY1= 5555
//            am29f040(sctstr, KEY1)    = 0xAAAA;//KEY1= 5555
//            am29f040(sctstr, KEY2)    = 0x5555;//KEY2= 2AAA
//            am29f040(sctstr, KEY1)    = 0xA0A0;
//    		/***   scrittura della flash         ***/
//    		*pt_flash = mask;
//
//    		/***  attesa fine scrittura           ***/
//    		data3 = *pt_flash;
//    		ok1 = 0;     
//    		ok2 = 0;
//    		tim1 = 0;
//    		tim2 = 0;
//    		/*attendi operazione di scrittura */
//    		while (!(ok1 && ok2)) {
//    			if (!(ok1 || tim1)) {
//    				if ((data3 & 0x8000) == (mask & 0x8000)){
//    					ok1 = 1;
//    				}
//    				else {
//    					if ((data3 & 0x2000) == 0x2000){
//    						data1 = *pt_flash;
//    						if ((data1 & 0x8000) != (mask & 0x8000)){
//    							er = 11;
//    							tim1 = 1;
//    						}
//    						else ok1 = 1;
//    					}
//    				}
//    			}    
//    			if (!(ok2 || tim2)) {
//    				if ((data3 & 0x0080) == (mask & 0x0080)){
//    					ok2 = 1;
//    				}
//    				else {
//    					if ((data3 & 0x0020) == 0x0020){
//    						data1 = *pt_flash;
//    						if ((data1 & 0x0080) != (mask & 0x0080)){
//    							er = 12;
//    							tim2 = 1;
//    						}	
//    						else ok2 = 1;
//    					}
//    				}
//    			}
//    			if (tim1 || tim2) break;
//    			data3 = *pt_flash;
//    		    if ( _getkey()  == '\t' ) {
//    			   er = 9;	
//    			   break;
//    		    }				
//    		}
//	        /*verifica scrittura in flash*/
//            k = 10000;
//    		tim1 = 0;
//    		while( (((data3 = *pt_flash) & mask) != mask) && ((c = _getkey())  != '\r') ) 
//    		{    
//                if ( k > 10000) 
//		        {	   
//			        app = (unsigned long) pt_flash;
//        			off = (unsigned int)(app & 0x00FFFF);
//        			app = app >> 16;
//        			seg = (unsigned int)(app & 0x0000FF);
//        			printf("\r2-KO!!! Ad Address %02x%04x Bit %02d atteso 1 letto 0",seg,off, cnt_dt);
//        			k = 0;
//        			tim1 = 1;
//        			er = 13;
//        	    }
//		        k++;		
//       		    if ( c == '\t' )
//    		    {
//        		    er = 9;
//        			break;
//    		    }				
//		    }
//	    }
//
//		/* shift l'indirizzo flash successivo*/
//	    pt_flash=(unsigned short  *)( 0x400000 + (1<<cnt));
//
//	}
//	
//
//	if(chip_erase()) 
//	    printf("ERASE FAILED \n");
//	else
//	    printf("ERASE OK\n");
//
//	
//	/*** ciclo di scrittura e verifica su tutti gli indirizzi ***/
//	printf("\n");
//	for ( pt_flash = sctstr; pt_flash <= flashlimit; pt_flash=pt_flash+1)
//	{
//	/***    procedura di start write     ***/
//       
//	    j = 0;
//        data2 = 0xFFFE;		
//	    for (dt = 0x0001; dt != 0x0000; dt = dt << 1)
//	    {		
//		    k = 10000;
//		    tim1 = 0;
//   	    
//            //printf("-> @%06lx *pt=%04x data3:%04x  data2:%04x\r",pt_flash,*pt_flash,data3,data2);
//		    /*legge  e verifica fino a max tentativi o '\r'*/
//		    while( (((data3 = *pt_flash)  & dt) == 0)/* && ((c=_getkey())  != '\r') */)
//		    {
//    		     if ( k > 10000) //1000 tentativi
//    		     {	   
//        			app = (unsigned long) pt_flash;
//        			off = (unsigned int)(app & 0x00FFFF);
//        			app = app >> 16;
//        			seg = (unsigned int)(app & 0x0000FF);
//        			printf("\n1 KO!!! Ad Address %02x%04x Bit %02d atteso 1 letto 0",seg,off, j);
//        			k = 0;
//        			tim1 = 1;
//        			er = 10;
//    		     }
//    		     k++;
//    		     if ( (c=_getkey()) == '\t' ){
//    			    er = 9;
//    			    break;
//    		     }				
//		    }
//    		if ( tim1 ) printf("\r\n");
//    		if ( er == 9 ) break;	
//    		
////            am29f040(sctstr, 0)       = 0xF0F0;//KEY1= 5555
//            am29f040(sctstr, KEY1)    = 0xAAAA;//KEY1= 5555
//            am29f040(sctstr, KEY2)    = 0x5555;//KEY2= 2AAA
//            am29f040(sctstr, KEY1)    = 0xA0A0;
//    		/***   scrittura della flash         ***/
//    		*pt_flash = ( short)data2;
//    		/***  attesa fine scrittura           ***/
//            printf("@%06lx wr data2:%04x\r",pt_flash,data2);
//
//    		ok1 = 0;     
//    		ok2 = 0;
//    		tim1 = 0;
//    		tim2 = 0;
//    		data3 = *pt_flash;
//    		while (!(ok1 && ok2)) {
//    			if (!(ok1 || tim1)) {
//    				if ((data3 & 0x8000) == (data2 & 0x8000)){//scrittura terminata
//    					ok1 = 1;
//    				}
//    				else {
//    					if ((data3 & 0x2000) == 0x2000){
//    						data1 = *pt_flash;
//    						if ((data1 & 0x8000) != (data2 & 0x8000)){
//    							er = 11;
//    							tim1 = 1;
//    						}
//    						else ok1 = 1;
//    					}
//    				}
//    			}    
//    			if (!(ok2 || tim2)) {
//    				if ((data3 & 0x0080) == (data2 & 0x0080)){
//    					ok2 = 1;
//    				}
//    				else {
//    					if ((data3 & 0x0020) == 0x0020)
//    					{
//    						data1 = *pt_flash;
//    						if ((data1 & 0x0080) != (data2 & 0x0080)){
//    							er = 12;
//    							tim2 = 1;
//    						}	
//    						else ok2 = 1;
//    					}
//    				}
//    			}
//    			
//    			if (tim1 || tim2)
//    			{
//    			     printf("\nerrore di timeout in scrittura in flash di %04x\n",data2);
//    			     break;
//    			}
//
//    			data3 = *pt_flash;
//   		    
//    		    if ( (c=_getkey())   == '\t' ) 
//    		    {
//    			   er = 9;	
//    			   break;
//    		    }				
//    		}
//            //printf("@%06lx \r",(pt_flash));
//
//    		if ( er == 9 ) break;
//    		i++;
//    		if (i==60000)
//    		{
//    			printf(".");
//    			i = 0;
//    		}
//    		k = 1000;
//    		tim1 = 0;
////            printf(" .. vrf  *pt=%04x data3=%04x  mask = %04x \r",(*pt_flash),data3,dt);
//		while( ((data3 = *pt_flash)  & dt) != 0 /*&& ((c = _getkey())  != '\r') */) 
//		{    
//		    if ( k > 1000) 
//		    {	   
//    			app = (unsigned long) pt_flash;
//    			off = (unsigned int)(app & 0x00FFFF);
//    			app = app >> 16;
//    			seg = (unsigned int)(app & 0x0000FF);
//    			printf("%x2 KO!!! Ad Address %02x%04x Bit %02d atteso 0x%04x letto  0x%04X   )\r",k,seg,off, j,data,(*pt_flash));
//    			k = 0;
//    			tim1 = 1;
//    			er = 13;
//            }
//		    k++;		
//		    if (( c = _getkey()) == '\t' )
//		    {
//			    er = 9;
//			    break;
//		    }				
//		}
//		if ( tim1 ) printf("\r\n");
//		if ( er == 9 ) break;	
//		j++;
//		data2 = data2 << 1;
//	    }
//        if ( er == 9 ) break;
//	}
//	printf("\r\n");
//	return er;
//}

/*dopo la cancellacione verifica che la flash sia a FFFFh poi scrive e
 verifica la word data per tutto lo spazio di indirizzi*/
int wr_vrf_fast(unsigned short data)
{
	unsigned short data3,data1,ok1,ok2,tim1,tim2,i,data2,j,k,dt;
	int  er;
	int  erw;
	unsigned short *pt_flash;
	unsigned int  off,seg;
	unsigned long app; 
//    long offset;
	char c;
    
    er=0;
//    printf("VERIFY BLANCK FLASH (FFFF)\n");
	i=60000;
//    offset = 0;
	
	/*** verifica FFFF , scrive data e lo verifica su tutti gli indirizzi ***/
	for ( pt_flash = sctstr; pt_flash <= flashlimit; pt_flash=pt_flash+1)
	{
	/***    procedura di start write     ***/
       
	    k = 1000;
	    tim1 = 0;
   	    printf("@%06lx\r",pt_flash);

       /*verifica FFFF */
	    while( (((data3 = *pt_flash)  & 0xFFFF) != 0xFFFF) /*&& ((c=_getkey())  != '\r')*/ )
	    {
    	     if ( k > 1000) //1000 tentativi
    	     {	   
       			app = (unsigned long) pt_flash;
       			off = (unsigned int)(app & 0x00FFFF);
       			app = app >> 16;
       			seg = (unsigned int)(app & 0x0000FF);
       			printf("\nKO!!! Ad Address %02x%04x  atteso FFFFh letto %04x",seg,off, data3);
       			k = 0;
       			tim1 = 1;
       			er = 10;
    	     }
    	     k++;
    	     if ( (c=_getkey()) == '\t' ){
    		    er = 9;
    		    break;
    	     }				
	    }
//   }
//    printf("\nstep 2 - WRITE FLASH %04Xh SU OGNI INDIRIZZO\n", data);
    /* reset */
//    am29f040(sctstr, 0)       = 0xF0F0;
//    pt_flash = sctstr;
//    offset = 0;
//	for ( pt_flash = sctstr; pt_flash <= flashlimit; pt_flash=pt_flash+1,offset+=2)
//	{

        /* start program data */
        am29f040(sctstr, KEY1)    = 0xAAAA;
        am29f040(sctstr, KEY2)    = 0x5555;
        am29f040(sctstr, KEY1)    = 0xA0A0;
//        am29f040(sctstr, offset)  = data;
        am29f040(pt_flash, 0)  = data;

        /* wait for write completion */
//        erw = am29f040_wait(sctstr, offset, data);
        erw = am29f040_wait(pt_flash, 0, data);
        if (erw)
        {
            am29f040(sctstr, 0)   = 0xF0F0;
            er= 11;
        }
//   	    printf("WRITE @%06lx\r",(sctstr+offset));
	    k = 1000;
	    tim1 = 0;
   	    //pt_flash = sctstr + offset;
//   	    printf("  -> READ  dato =%04x\r",(*pt_flash));
       /*verifica scrittura di data */
	    while( (((data3 = *pt_flash)  & 0xFFFF) != (data & 0xFFFF))/* && ((c=_getkey())  != '\r') */)
	    {
    	     if ( k > 1000) //1000 tentativi
    	     {	   
       			app = (unsigned long) pt_flash;
       			off = (unsigned int)(app & 0x00FFFF);
       			app = app >> 16;
       			seg = (unsigned int)(app & 0x0000FF);
       			printf("\nKO!!! Ad Address %02x%04x  atteso %04xh letto %04x",seg,off,data, data3);
       			k = 0;
       			tim1 = 1;
       			er = 12;
    	     }
    	     k++;
    	     if ( (c=_getkey()) == '\t' ){
    		    er = 9;
    		    break;
    	     }				
	    }
    
//    offset+=2    
    }
    
   	return er;
 
    		
}

/**********************************************************/
/* Programma test della flash                             */
/**********************************************************/
//void tstflash (void )
//{
//	int error,error5555,errorAAAA,errorSECT,i;
//	unsigned int k,j,seg,off,exit;
//	unsigned short  data3;
//	unsigned short  * sectaddr;
//	unsigned short  * sectstart;
//	unsigned short  * sectend;
//	unsigned long app;
//	char c;
//
//
//	i=i;
//	exit = 0;
//	printf("TEST ERASE FLASH...\n");
//	error = chip_erase();
//	if(error) 
//	    printf("ERASE FAILED (error = %d)\n",error);
//	else
//	    printf("ERASE OK\n");
//	    
//	for(i=0;i<1000;i++);
//	
//	
//	
//	if (!error ) {
//		error5555 = wr_vrf(0x5555);
//	    printf("\nwr_vrf (error %d)\n",error5555);
//		
//		for(i=0;i<1000;i++);
//		//* test di ersa sectore: verifica che sia riuscita la cancellazione a settori , verificando che ogni locazione sia a FFFF*//
//		
//		
//		if (error5555==0 || error5555==9) {	
////            printf("TEST SECTOR ERASING ...\n ");
//			for ( sectstart = sctstr; sectstart <= sectlimit; sectstart += sctdelta)
//			{
////				printf("ERASING from @%06lX\n",sectstart);
//				if (!(errorSECT = chip_erasesect(sectstart)))
//				{
//				    printf("ERASED from @%06lX\n",sectstart);
//					sectend = sectstart + dimsct;
////					printf("3 VERIFY ERASING SECTOR start @%06lX -> end @%06lX \n",sectstart,sectend);
//					for ( sectaddr = sectstart; sectaddr <= sectend; sectaddr++)
//					{
//						k = 10000;
//						j = 0;
//						while( ((data3 = *sectaddr) != 0xFFFF) && ((c = _getkey())  != '\r') ) { 
//						     if ( k > 10000) 
//						     {	   
//    							app = (unsigned long) sectaddr;
//    							off = (unsigned int)(app & 0x00FFFF);
//    							app = app >> 16;
//    							seg = (unsigned int)(app & 0x0000FF);
//    							printf("\rKO!!! Ad Address %02x%04x letto %04x",seg,off,data3);
//    							k = 0;
//    							j = 1;
//						     }
//						     if ( c == '\t' ) 
//	     				     {
//    							errorSECT = 9;
//    							exit = 1;
//    							break;
//	     				     }				
//						     k++;		
//						}
//						if ( j ) printf("\r\n");
//						if ( exit ) break;	
//					}	
//				}
//				else {
//				    printf("\nerrorSECT = %d\n",errorSECT);
//				    break;   
//				}
//				if ( exit ) break;	
//			}
//	 	}
////	 	if(!errorSECT) 
////	 	    errorAAAA = wr_vrf(0xAAAA);
//	 	 
//	 	
//	} 
//	
//	/* lettura dello codice/*
//    /* send the autoselect command */
//    am29f040(sctstr, 0)       = 0xF0F0;
//    am29f040(sctstr, KEY1)    = 0xAAAA;
//    am29f040(sctstr, KEY2)    = 0x5555;
//    am29f040(sctstr, KEY1)    = 0x9090;
//
//    /* check the manufacture code and Am29F040 device code */
//    if (am29f040(sctstr, 0) != 0x0101 || am29f040(sctstr, 1) != 0xA4A4)
//        printf("\nAM29F04O NOT detected\n");
//    else
//        printf("\nAM29F04O  detected\n");
//
//    /* reset to read mode and return */
//    am29f040(sctstr, 0)       = 0xF0F0;
//	
//	
//	switch (error){
//		case 0:
//			//save_stato (TFLAWRVROK);
//			//ram2flash ();
//			break;
//		case 1:
//			prn_stato (0,TFLATOTERSCH1KO);
//			break;
//		case 2:
//			prn_stato (0,TFLATOTERSCH2KO);
//			break;
//		case 3:
//			prn_stato (0,TFLAFZEXITTOTERS);
//			break;
//		case 6:
//			prn_stato (0,TFLASCTERSCH1KO);
//			break;
//		case 7:
//			prn_stato (0,TFLASCTERSCH2KO);
//			break;
//		case 8:
//			prn_stato (0,TFLAFZEXITSCTERS);
//			break;
//		case 9:
//			prn_stato (0,TFLAFZEXITWRVR);
//			break;
//		case 10:
//			prn_stato (0,TFLAVR1KO);
//			break;
//		case 11:
//			prn_stato (0,TFLAPRGCH1KO);
//			break;
//		case 12:
//			prn_stato (0,TFLAPRGCH2KO);
//			break;
//		case 13:
//			prn_stato (0,TFLAVR0KO);
//			break;
//	}
//	printf("\r\n");
//}

/**********************************************************/
/* Programma test della flash VELOCE 5555 AAAA            */
/**********************************************************/
short tstflash_vel (short argc, char *argv[] )
{
	int error,error5555,errorAAAA,errorSECT,i;
	unsigned int k,j,seg,off,exit;
	unsigned short  data3;
	unsigned short  * sectaddr;
	unsigned short  * sectstart;
	unsigned short  * sectend;
	unsigned long app;
	char c;


	i=i;
	
	error = 0;
	error5555 = 0;
	errorAAAA = 0;
	errorSECT = 0;
	
	exit = 0;
	/* lettura dello codice/*
    /* send the autoselect command */
    am29f040(sctstr, 0)       = 0xF0F0;
    am29f040(sctstr, KEY1)    = 0xAAAA;
    am29f040(sctstr, KEY2)    = 0x5555;
    am29f040(sctstr, KEY1)    = 0x9090;

    /* check the manufacture code and Am29F040 device code */
    if (am29f040(sctstr, 0) != 0x0101 || am29f040(sctstr, 1) != 0xA4A4)
        printf("\nAM29F04O NON RICONOSCIUTA\n");
    else
        printf("\nAM29F04O RICONOSCIUTA\n");

    /* reset to read mode and return */
    am29f040(sctstr, 0)       = 0xF0F0;
	
	
	printf("TEST ERASE FLASH...\n");
	error = chip_erase();
	if(error) 
	    printf("ERASE FAILED (error = %d)\n",error);
	else
	    printf("ERASE OK\n");
	    
	for(i=0;i<1000;i++);
	
	if (!error ) {
	    
     	printf("TEST FLASH SCRITTURA 5555h\n");
		error5555 = wr_vrf_fast(0x5555);

	    if (error5555!=0) printf("\nwr_vrf (error %d)\n",error5555);
		for(i=0;i<1000;i++);
		//* test di ersa sectore: verifica che sia riuscita la cancellazione a settori , verificando che ogni locazione sia a FFFF*//
		if (error5555==0) {	
            printf("TEST SECTOR ERASING ...\n ");
			for ( sectstart = sctstr; sectstart <= sectlimit; sectstart += sctdelta)
			{
				if (!(errorSECT = chip_erasesect(sectstart)))
				{
//				    printf("2 ERASED from @%06lX\n",sectstart);
					sectend = sectstart + (dimsct/2);
					printf(" VERIFY ERASING SECTOR start @%06lX -> end @%06lX \n",sectstart,sectend);
					for ( sectaddr = sectstart; sectaddr <= sectend; sectaddr++)
					{
						k = 10000;
						j = 0;
						while( ((data3 = *sectaddr) != 0xFFFF) && ((c = _getkey())  != '\r') ) { 
    					     if ( k > 10000) 
						     {	   
    							app = (unsigned long) sectaddr;
    							off = (unsigned int)(app & 0x00FFFF);
    							app = app >> 16;
    							seg = (unsigned int)(app & 0x0000FF);
    							printf("\rKO!!! Ad Address %02x%04x letto %04x",seg,off,data3);
    							k = 0;
    							j = 1;
						     }
						     if ( c == '\t' ) 
	     				     {
    							errorSECT = 9;
    							exit = 1;
    							break;
	     				     }				
						     k++;		
						}
						if ( j ) printf("\r\n");
						if ( exit ) break;	
					}	
				}
				else {
				    printf("\nerrorSECT = %d\n",errorSECT);
				    break;   
				}
				if ( exit ) break;	
			}

            am29f040(sctstr, 0)       = 0xF0F0;//reset
    		for(i=0;i<1000;i++);
    		
         	printf("TEST FLASH SCRITTURA AAAAh\n");
    	 	if(!errorSECT) 
    	 	    errorAAAA = wr_vrf_fast(0xAAAA);
	 	}
	} 

	error = chip_erase();
	if(error) 
	    printf("ERASE FAILED (error = %d)\n",error);
	else
	    printf("ERASE OK\n");
	
	
	if (!error && !error5555 && !errorAAAA && !errorSECT){
		save_stato (TFLAWRVROK);
//		ram2flash ();
	}
    else
    { 
        if (error == 1)  prn_stato (0,TFLATOTERSCH1KO);
        if (error == 2)  prn_stato (0,TFLATOTERSCH2KO);
        if (error == 3)  prn_stato (0,TFLAFZEXITTOTERS);
            
        if (error5555 == 9)  prn_stato (0,TFLAFZEXITWRVR55);
        if (error5555 == 10)  prn_stato (0,TFLAFVRFFKO);
        if (error5555 == 11)  prn_stato (0,TFLAFWR55KO);
        if (error5555 == 12)  prn_stato (0,TFLAFVR55KO);

        if (errorSECT == 6)  prn_stato (0,TFLASCTERSCH1KO);
        if (errorSECT == 7)  prn_stato (0,TFLASCTERSCH2KO);
        if (errorSECT == 8)  prn_stato (0,TFLAFZEXITSCTERS);

 
        if (errorAAAA == 9)  prn_stato (0,TFLAFZEXITWRVRAA);
        if (errorAAAA == 10)  prn_stato (0,TFLAFVRFFKO);
        if (errorAAAA == 11)  prn_stato (0,TFLAFWRAAKO);
        if (errorAAAA == 12)  prn_stato (0,TFLAFWRAAKO);
    }	
	

    return error;
}



void prn_stato (unsigned int prg, unsigned int cod)
{
	switch (cod){
		case TFLAFZEXITR2F:
			printf("%04d  %04x  Test Flash forzata uscita durante copy RAM TO FLASH.........   KO  ",prg,cod);
			break;
		case TFLACOPYR2FOK:
			printf("%04d  %04x  Test Flash Copy RAM TO FLASH................................   OK  ",prg,cod);
			break;
		case TFLACOPYR2FKO:
			printf("%04d  %04x  Test Flash Copy RAM TO FLASH................................   KO  ",prg,cod);
			break;
		case TFLATOTERSCH1KO:
			printf("%04d  %04x  Test Flash Total Erase Chip1................................   KO  ",prg,cod);
			break;
		case TFLATOTERSCH2KO:
			printf("%04d  %04x  Test Flash Total Erase Chip2................................   KO  ",prg,cod);
			break;
		case TFLAFZEXITTOTERS:
			printf("%04d  %04x  Test Flash forzata uscita durante total erase...............   KO  ",prg,cod);
			break;
		case TFLASCTERSCH1KO:
			printf("%04d  %04x  Test Flash Sector Erase Chip1...............................   KO  ",prg,cod);
			break;
		case TFLASCTERSCH2KO:
			printf("%04d  %04x  Test Flash Sector Erase Chip2...............................   KO  ",prg,cod);
			break;
		case TFLAFZEXITSCTERS:
			printf("%04d  %04x  Test Flash forzata uscita durante sector erase..............   KO  ",prg,cod);
			break;
		case TFLAPRGCH1KO:
			printf("%04d  %04x  Test Flash Program Chip1....................................   KO  ",prg,cod);
			break;
		case TFLAPRGCH2KO:
			printf("%04d  %04x  Test Flash Program Chip2....................................   KO  ",prg,cod);
			break;
		case TFLAFZEXITWRVR55:
			printf("%04d  %04x  Test Flash forzata uscita durante write-verify 55...........   KO  ",prg,cod);
			break;
		case TFLAFZEXITWRVRAA:
			printf("%04d  %04x  Test Flash forzata uscita durante write-verify AA...........   KO  ",prg,cod);
			break;

		case TFLAFWR55KO:
			printf("%04d  %04x  Test Flash write 55.........................................   KO  ",prg,cod);
			break;
		case TFLAFVR55KO:
			printf("%04d  %04x  Test Flash verify 55........................................   KO  ",prg,cod);
			break;
		case TFLAFWRAAKO:
			printf("%04d  %04x  Test Flash write 55.........................................   KO  ",prg,cod);
			break;
		case TFLAFVRAAKO:
			printf("%04d  %04x  Test Flash verify 55........................................   KO  ",prg,cod);
			break;
		case TFLAVR0KO:
			printf("%04d  %04x  Test Flash verifed 0........................................   KO  ",prg,cod);
			break;
		case TFLAWRVROK:
			printf("%04d  %04x  Test Flash write-verify test................................   OK  ",prg,cod);
			break;
		case TSIOOK:
			printf("%04d  %04x  Test BUS SIO ...............................................   OK  ",prg,cod);
			break;
		case TSIOKO:
			printf("%04d  %04x  Test BUS SIO ...............................................   KO  ",prg,cod);
			break;
		case TADCOK:
			printf("%04d  %04x  Test ADC ...................................................   OK  ",prg,cod);
			break;
		case TADCKO:
			printf("%04d  %04x  Test ADC ...................................................   KO  ",prg,cod);
			break;
		case TPIOOK:
			printf("%04d  %04x  Test PIO ...................................................   OK  ",prg,cod);
			break;
		case TPIOKO:
			printf("%04d  %04x  Test PIO ...................................................   KO  ",prg,cod);
			break;
		case TMVBSASHF0OK:
			printf("%04d  %04x  Test MVB BUS Service Area Shift degli 1 tra gli 0...........   OK  ",prg,cod);
			break;
		case TMVBSASHF0KO:
			printf("%04d  %04x  Test MVB BUS Service Area Shift degli 1 tra gli 0...........   KO  ",prg,cod);
			break;
		case TMVBSASHF1OK:
			printf("%04d  %04x  Test MVB BUS Service Area Shift degli 0 tra gli 1...........   OK  ",prg,cod);
			break;
		case TMVBSASHF1KO:
			printf("%04d  %04x  Test MVB BUS Service Area Shift degli 0 tra gli 1...........   KO  ",prg,cod);
			break;
		case TMVBSAMARCHOK:
			printf("%04d  %04x  Test MVB BUS Service Area March C-..........................   OK  ",prg,cod);
			break;
		case TMVBSAMARCHKO:
			printf("%04d  %04x  Test MVB BUS Service Area March C-..........................   KO  ",prg,cod);
			break;
		case TMVBSAMXRUMOK:
			printf("%04d  %04x  Test MVB BUS Service Area Massimo rumore....................   OK  ",prg,cod);
			break;
		case TMVBSAMXRUMKO:
			printf("%04d  %04x  Test MVB BUS Service Area Massimo rumore....................   KO  ",prg,cod);
			break;
		case TMVBTMSHF0OK:
			printf("%04d  %04x  Test MVB BUS Traffic Memory Shift degli 1 tra gli 0.........   OK  ",prg,cod);
			break;
		case TMVBTMSHF0KO:
			printf("%04d  %04x  Test MVB BUS Traffic Memory Shift degli 1 tra gli 0.........   KO  ",prg,cod);
			break;
		case TMVBTMSHF1OK:
			printf("%04d  %04x  Test MVB BUS Traffic Memory Shift degli 0 tra gli 1.........   OK  ",prg,cod);
			break;
		case TMVBTMSHF1KO:
			printf("%04d  %04x  Test MVB BUS Traffic Memory Shift degli 0 tra gli 1.........   KO  ",prg,cod);
			break;
		case TMVBTMMARCHOK:
			printf("%04d  %04x  Test MVB BUS Traffic Memory March C-........................   OK  ",prg,cod);
			break;
		case TMVBTMMARCHKO:
			printf("%04d  %04x  Test MVB BUS Traffic Memory March C-........................   KO  ",prg,cod);
			break;
		case TMVBTMMXRUMOK:
			printf("%04d  %04x  Test MVB BUS Traffic Memory Massimo rumore..................   OK  ",prg,cod);
			break;
		case TMVBTMMXRUMKO:
			printf("%04d  %04x  Test MVB BUS Traffic Memory Massimo rumore..................   KO  ",prg,cod);
			break;
		case TMVBSELOK:
			printf("%04d  %04x  Test MVB BUS Selettore indirizzi............................   OK  ",prg,cod);
			break;
		case TMVBSELKO:
			printf("%04d  %04x  Test MVB BUS Selettore indirizzi............................   KO  ",prg,cod);
			break;
		case TMVBBUSOK:
			printf("%04d  %04x  Test MVB BUS di Comunicazione...............................   OK  ",prg,cod);
			break;
		case TMVBBUSKO:
			printf("%04d  %04x  Test MVB BUS di Comunicazione...............................   KO  ",prg,cod);
			break;
		case TTXUARTAOK:
			printf("%04d  %04x  Test UARTA IN TX UARTB IN RX................................   OK  ",prg,cod);
			break;
		case TTXUARTAKO:
			printf("%04d  %04x  Test UARTA IN TX UARTB IN RX................................   KO  ",prg,cod);
			break;
		case TTXUARTBOK:
			printf("%04d  %04x  Test UARTB IN TX UARTA IN RX................................   OK  ",prg,cod);
			break;
		case TTXUARTBKO:
			printf("%04d  %04x  Test UARTB IN TX UARTA IN RX................................   KO  ",prg,cod);
			break;
		case TTXUARTCOK:
			printf("%04d  %04x  Test UARTC IN TX UARTD IN RX................................   OK  ",prg,cod);
			break;
		case TTXUARTCKO:
			printf("%04d  %04x  Test UARTC IN TX UARTD IN RX................................   KO  ",prg,cod);
			break;
		case TTXUARTDOK:
			printf("%04d  %04x  Test UARTD IN TX UARTC IN RX................................   OK  ",prg,cod);
			break;
		case TTXUARTDKO:
			printf("%04d  %04x  Test UARTD IN TX UARTC IN RX................................   KO  ",prg,cod);
			break;
		default:
			printf("%04d  %04x  Stato test sconosciuto......................................   KO  ",prg,cod);
			break;


	}
	printf("\r\n");
	
}

void save_stato (unsigned short stato)
{
	unsigned short i,dt,dat,data1;
	unsigned short  * pt_flash;
		
	i=0;
	pt_flash = (unsigned short  *)(0x4E0000);
	while ( ((dt = (*pt_flash)) != 0xFFFF) && (pt_flash <= (unsigned short  *)(0x4EFFFE))){
		pt_flash++;
		i++;
	}
	/***    procedura di start write     ***/
    am29f040(sctstr, KEY1)    = 0xAAAA;
    am29f040(sctstr, KEY2)    = 0x5555;
    am29f040(sctstr, KEY1)    = 0xA0A0;
	/***   scrittura della flash         ***/
	am29f040(pt_flash ,0)     = stato;
	/***  attesa fine scrittura           ***/
	dat = *pt_flash;
	data1 = *pt_flash;
	while (dat != data1) {
		dat = *pt_flash;
		data1 = *pt_flash;
	}
    printf("\n salvo @%06lX codice @%04X \n",pt_flash, (*pt_flash) );
	prn_stato(i,stato);
}

short prn_report(short argc, char *argv[])
{
	unsigned short i,dt,dat,data1;
	unsigned short  * pt_flash;
		
	printf("PROG.  COD.                    DESCRIZIONE DEL TEST                       ESITO\r\n");
	i=0;
	pt_flash = (unsigned short  *)(0x4E0000);
	while ( ((dt = *pt_flash) != 0xFFFF) && (pt_flash <= (unsigned short  *)(0x4EFFFE))){
		prn_stato(i,dt);
		pt_flash++;
		i++;
	}
}


short w_cmd(short argc, char *argv[])
{
    int i;
	argc=argc;
	argv=argv;
	
	quicc->sim_sypcr = 0x4c; /* disable wd */
    
	// hw_ram2rom ();
		/* write the memory contents to flash */
	am29f040_chip_erase((void*)0x400000);
    
    for(i=0;i<50000;i++)
    if( (i % 10000)==0) printf("w");
        	
	asm(void, "	move.w	#$2700,SR");
	
	for (i = 0; i < 64; i++) {
		led_user1(i & 1);
		led_user2(i & 2);
		led_user3(i & 4);
		led_user4(i & 8);
		led_user5(i & 16);
		led_user6(i & 32);
		am29f040_write((void*)0x400000, i * (0x100000 / 64), (void*)(i * (0x100000 / 64)), 0x100000 / 64);
	}
    for(i=0;i<50000;i++)
    if( (i % 10000)==0) printf("w");
	hw_reset();
 
}
