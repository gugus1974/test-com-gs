/*****************************************************************************
* Sab.14.feb.04           v.1.00       sw_st\sw_stb\tststbxx\source\tstmvb.c *
******************************************************************************
* Funzioni per il test del MVB BUS                                           *
******************************************************************************
*                                                                            *
*****************************************************************************/

#include "stdio.h"
//#include "shd_var.h"
#include "tstflash.h"   
#include "cod_st.h"
#include "hw_com.h"
//#pragma ot(3,SPEED)


unsigned int  * saddmvb       = (unsigned int *)0xC00000;     /* indirizzo di partenza dello spazio di memoria dell'MVBC */   
unsigned int  * eaddmvb       = (unsigned int *)0xCFFFFE;     /* indirizzo di fine dello spazio di memoria dell'MVBC     */ 
unsigned int  * sasaddmvb     = (unsigned int *)0xC0FC00;   /* indirizzo di partenza della service area dell'MVBC nel mode 'funmode'*/     
unsigned int  * saeaddmvb     = (unsigned int *)0xC0FFFE;   /* indirizzo di fine della service area dell'MVBC nel mode 'funmode'*/
unsigned int funmode = 4;   					   /* Mode che determina la dimensione della TM dell'MVBC (4=1MB)*/     
unsigned int  * saeaddmvb1;
/************** definizioni registri MVB ****************************/
unsigned int  * regscr  = (unsigned int  *)0xC03F80;     /* indirizzo del registro SCR dell'MVB (MCM =0)    */ 
unsigned int  * regscr1 = (unsigned int  *)0xC0FF80;     /* indirizzo del registro SCR dell'MVB nel mode 'funmode'     */ 
unsigned int  * regmcr  = (unsigned int  *)0xC03F84;     /* indirizzo del registro MCR dell'MVB     */ 
unsigned int  * regmcr1 = (unsigned int  *)0xC0FF84;     /* indirizzo del registro MCR dell'MVB nel mode 'funmode'     */ 
unsigned int  * regdr   = (unsigned int  *)0xC0FF88;     /* indirizzo del registro DR dell'MVB     */ 
unsigned int  * regstsr = (unsigned int  *)0xC0FF8C;     /* indirizzo del registro STSR dell'MVB    */ 
unsigned int  * regfc   = (unsigned int  *)0xC0FF90;     /* indirizzo del registro FC dell'MVB      */ 
unsigned int  * regec   = (unsigned int  *)0xC0FF94;     /* indirizzo del registro EC dell'MVB      */ 
unsigned int  * regmfr  = (unsigned int  *)0xC0FF98;     /* indirizzo del registro MFR dell'MVB     */ 
unsigned int  * regmfre = (unsigned int  *)0xC0FF9C;     /* indirizzo del registro MFRE dell'MVB    */ 
unsigned int  * regmr   = (unsigned int  *)0xC0FFA0;     /* indirizzo del registro MR dell'MVB      */ 
unsigned int  * regmr2  = (unsigned int  *)0xC0FFA4;     /* indirizzo del registro MR2 dell'MVB     */ 
unsigned int  * regdpr1 = (unsigned int  *)0xC0FFA8;     /* indirizzo del registro DPR dell'MVB     */ 
unsigned int  * regdpr2 = (unsigned int  *)0xC0FFAC;     /* indirizzo del registro DPR2 dell'MVB    */ 
unsigned int  * regipr0 = (unsigned int  *)0xC0FFB0;     /* indirizzo del registro IPR0 dell'MVB    */ 
unsigned int  * regipr1 = (unsigned int  *)0xC0FFB4;     /* indirizzo del registro IPR1 dell'MVB    */ 
unsigned int  * regimr0 = (unsigned int  *)0xC0FFB8;     /* indirizzo del registro IMR0 dell'MVB    */ 
unsigned int  * regimr1 = (unsigned int  *)0xC0FFBC;     /* indirizzo del registro IMR1 dell'MVB    */ 
unsigned int  * regisr0 = (unsigned int  *)0xC0FFC0;     /* indirizzo del registro ISR0 dell'MVB    */ 
unsigned int  * regisr1 = (unsigned int  *)0xC0FFC4;     /* indirizzo del registro ISR1 dell'MVB    */ 
unsigned int  * regivr0 = (unsigned int  *)0xC0FFC8;     /* indirizzo del registro IVR0 dell'MVB    */ 
unsigned int  * regivr1 = (unsigned int  *)0xC0FFCC;     /* indirizzo del registro IVR1 dell'MVB    */ 
unsigned int  * regdaor = (unsigned int  *)0xC03FD8;     /* indirizzo del registro DAOR dell'MVB     */ 
unsigned int  * regdaor1= (unsigned int  *)0xC0FFD8;     /* indirizzo del registro DAOR dell'MVB nel mode 'funmode'     */ 
unsigned int  * regdaok = (unsigned int  *)0xC0FFDC;     /* indirizzo del registro DAOK dell'MVB    */ 
unsigned int  * regtcr  = (unsigned int  *)0xC0FFE0;     /* indirizzo del registro TCR dell'MVB     */ 
unsigned int  * regtr1  = (unsigned int  *)0xC0FFF0;     /* indirizzo del registro TR1 dell'MVB     */ 
unsigned int  * regtr2  = (unsigned int  *)0xC0FFF4;     /* indirizzo del registro TR2 dell'MVB     */ 
unsigned int  * regtc1  = (unsigned int  *)0xC0FFF8;     /* indirizzo del registro TC1 dell'MVB     */ 
unsigned int  * regtc2  = (unsigned int  *)0xC0FFFC;     /* indirizzo del registro TC2 dell'MVB     */ 
unsigned int  * regmfs  = (unsigned int  *)0xC0FF00;     /* indirizzo del registro MFS dell'MVB     */ 
unsigned long st_LA_PIT       = (unsigned long) 0xC00000;            /* indirizzo di partenza dei LA_PIT        */ 
unsigned long end_LA_PIT      = (unsigned long) 0xC01FFE;            /* indirizzo di fine della LA_PIT          */ 
unsigned long st_DA_PIT       = (unsigned long) 0xC02000;            /* indirizzo di partenza dei DA_PIT        */ 
unsigned long end_DA_PIT      = (unsigned long) 0xC03FFE;            /* indirizzo di fine della DA_PIT          */ 
unsigned long st_FC15_PCS     = (unsigned long) 0xC0FE38;            /* indirizzo di partenza dei FC15_PCS      */ 
unsigned long st_FC15_DATA0   = (unsigned long) 0xC0FC58;            /* indirizzo FC15_DATA in pg0              */ 
unsigned long st_FC15_DATA1   = (unsigned long) 0xC0FC78;            /* indirizzo FC15_DATA in pg1              */ 
unsigned long st_LA_PCS       = (unsigned long) 0xC30000;            /* indirizzo di partenza dei LA_PCS        */ 
unsigned long end_LA_PCS      = (unsigned long) 0xC37FFE;            /* indirizzo di fine dei LA_PCS            */ 
unsigned long st_DA_PCS       = (unsigned long) 0xC38000;            /* indirizzo di partenza dei DA_PCS        */ 
unsigned long end_DA_PCS      = (unsigned long) 0xC3FFFE;            /* indirizzo di fine dei DA_PCS            */ 
unsigned long st_DA_DATA      = (unsigned long) 0xC40000;            /* indirizzo di partenza dei DA_DATA       */ 

unsigned int  * st_DA_PITptr;
unsigned int  * st_DA_PCSptr;
unsigned int  * st_DA_DATA0ptr;
unsigned int  * st_DA_DATA1ptr;
unsigned int  * st_DA_PCSptr1;
unsigned int  * st_DA_DATA0ptr1;
unsigned int  * st_DA_DATA1ptr1;
unsigned int  * FC15_PCSptr;
unsigned int  * FC15_DATAptr;




/**********************************************************/
/* Funzione test ram TEST SHIFT DEGLI 1 TRA GLI 0 NEL DATO*/
/* Variabili di ingresso:                                 */
/*   startadd    indirizzo di start della ram             */
/*   endadd      indirizzo di fine della ram              */
/**********************************************************/
int shf0(unsigned int  * startadd, unsigned int  * endadd)
{
	unsigned int data3,vrf,tim1,i,k,dt;
	int  er;
	unsigned int  * pt_ram;
	unsigned int  off,seg;
	unsigned long app; 
	char c;

	printf("TEST SHIFT DEGLI 1 TRA GLI 0 NEL DATO\n");
	er = 0;
	i=0;
	c = 0;
	/*** ciclo di scrittura e verifica su tutti gli indirizzi ***/
	for ( pt_ram = (startadd +1); pt_ram <= endadd; pt_ram++)
	{
	/***    procedura di start write     ***/
	     for (dt = 0x0001; dt != 0x0000; dt = dt << 1)
	     {		
    		*pt_ram = dt;
    		*(pt_ram-1)=0;
    		k = 10000;
    		tim1 = 0;
    		vrf = ((data3 = *pt_ram)  != dt);
    		printf("@%06lX \r",pt_ram );
    		while( ((c=_getkey())  != '\r') &&  vrf ) {
                         er = 1;
    		     if ( k == 10000) 
    		     {	   
        			app = (unsigned long) pt_ram;
        			off = (unsigned int)(app & 0x00FFFF);
        			app = app >> 16;
        			seg = (unsigned int)(app & 0x0000FF);
        			printf("\rKO!!! Ad Address %02x%04x atteso %04x letto %04x",seg,off,dt,data3);
        			k = 0;
        			tim1 = 1;
    		     }
    		     k++;
    		     if ( c == '\t' ) break;				
    		     *pt_ram = dt;
    		     data3 = *pt_ram;	
    		}
    		if ( tim1 ) printf("\r\n");
                    if ( c == '\t' ) break;				
    		i++;
    		if (i==1024){
    			printf(".");
    			i = 0;
    		}
	    }
             if ( c == '\t' ) break;				
	}
	printf("\r\n");
	return er;
	/*
	if (er) printf("\r\nKO!!!!!!!!\r\n");
	else    printf("\r\nOK\r\n");
	*/
}



/**********************************************************/
/* Funzione test ram TEST SHIFT DEGLI 0 TRA GLI 1 NEL DATO*/
/* Variabili di ingresso:                                 */
/*   startadd    indirizzo di start della ram             */
/*   endadd      indirizzo di fine della ram              */
/**********************************************************/
int shf1(unsigned int  * startadd, unsigned int  * endadd)
{
	unsigned int data3,vrf,tim1,i,k,dt;
	int  er;
	unsigned int  * pt_ram;
	unsigned int  off,seg;
	unsigned long app; 
	char c;

	printf("TEST SHIFT DEGLI 0 TRA GLI 1 NEL DATO\n");
	er = 0;
	i=0;
	c = 0;
	/*** ciclo di scrittura e verifica su tutti gli indirizzi ***/
	for ( pt_ram = (startadd +1); pt_ram <= endadd; pt_ram++)
	{
	/***    procedura di start write     ***/
	     for (dt = 0x0001; dt != 0x0000; dt = dt << 1)
	     {		
		*pt_ram = ~dt;
		*(pt_ram-1)=0;
		k = 10000;
		tim1 = 0;
   		printf("@%06lX  %04X\r",pt_ram,*(pt_ram)  );
		vrf = ((data3 = *pt_ram)  != (~dt));
   		printf("@%06lX \r",pt_ram );
		while( ((c=_getkey())  != '\r') && vrf ) {
		     er = 1;
		     if ( k == 10000) 
		     {	   
			app = (unsigned long) pt_ram;
			off = (unsigned int)(app & 0x00FFFF);
			app = app >> 16;
			seg = (unsigned int)(app & 0x0000FF);
			printf("\rKO!!! Ad Address %02x%04x atteso %04x letto %04x",seg,off,~dt,data3);
			k = 0;
			tim1 = 1;
		     }
		     k++;
		     if ( c == '\t' ) break; 
	             *pt_ram = ~dt;
		     data3 = *pt_ram;					
		}
		if ( tim1 ) printf("\r\n");
		if ( c == '\t' ) break;	
		i++;
		if (i==1024){
			printf(".");
			i = 0;
		}
	     }
             if ( c == '\t' ) break;
	}
	printf("\r\n");
	return er;
	/*
	if (er) printf("\r\nKO!!!!!!!!\r\n");
	else    printf("\r\nOK\r\n");
	*/
}



/**********************************************************/
/* Funzione test ram March C-                             */
/* Variabili di ingresso:                                 */
/*   startadd    indirizzo di start della ram             */
/*   endadd      indirizzo di fine della ram              */
/**********************************************************/
int march(unsigned int  * startadd, unsigned int  * endadd)
{
	unsigned int data3,tim1,i,k,dt,j,vrf;
	int  er;
	unsigned int  * pt_ram;
	unsigned int  off,seg;
	unsigned long app; 
	char c;

//	printf("TEST MARCH C-\r\n");
	er = 0;
	i=0;
	c = 0;
	/*** ciclo I  azzera dato ***/
	printf("Ciclo azzeramento RAM\r\n");
	for ( pt_ram = startadd; pt_ram <= endadd; pt_ram++) *pt_ram = 0;

	printf("Ciclo in cui si rilegge 0 e si scrive 1 bit per bit incrementando indirizzo\r\n");
	for ( pt_ram = startadd; pt_ram <= endadd; pt_ram++)
	{
	     j = 0;	
	     for (dt = 0x0001; dt != 0x0000; dt = dt << 1)
	     {		
    		k = 10000;
    		tim1 = 0;
    		vrf = ((data3 = (*pt_ram & dt)) != 0);
    		printf("@%06lX  %04X\r",pt_ram,*(pt_ram)  );
    		while( ((c=_getkey())  != '\r') && vrf ) {
    		     er = 1;
    		     if ( k == 10000) 
    		     {	   
        			app = (unsigned long) pt_ram;
        			off = (unsigned int)(app & 0x00FFFF);
        			app = app >> 16;
        			seg = (unsigned int)(app & 0x0000FF);
        			printf("\rKO!!! Ad Address %02x%04x Bit %02d atteso 0 letto %04x",seg,off,j,data3);
        			k = 0;
        			tim1 = 1;
        		 }
    		     k++;
    		     if ( c == '\t' ) break;
    		     data3 = (*pt_ram );				
    		}
    		if ( tim1 ) printf("\r\n");
    		if ( c == '\t' ) break;
    		data3 = *pt_ram;
    		*pt_ram = data3 | dt;	
    		i++;
    		if (i==60000){
    			printf(".");
    			i = 0;
    		}
    		j++;
	    }
        if ( c == '\t' ) break;
	}
	printf("\r\n");

	printf("\r\nCiclo in cui si rilegge 1 e si scrive 0 bit per bit incrementando indirizzo\r\n");
	for ( pt_ram = startadd; pt_ram <= endadd; pt_ram++)
	{
	     j = 0;	
	     for (dt = 0x0001; dt != 0x0000; dt = dt << 1)
	     {		
		k = 10000;
		tim1 = 0;
		vrf = ((data3 = (*pt_ram & dt)) == 0);
   		printf("@%06lX  %04X\r",pt_ram,*(pt_ram)  );
		while( ((c=_getkey())  != '\r') && vrf ) {
		     er = 1;
		     if ( k == 10000) 
		     {	   
			app = (unsigned long) pt_ram;
			off = (unsigned int)(app & 0x00FFFF);
			app = app >> 16;
			seg = (unsigned int)(app & 0x0000FF);
			printf("\rKO!!! Ad Address %02x%04x Bit %02d atteso 1 letto %04x",seg,off,j,data3);
			k = 0;
			tim1 = 1;
		     }
		     k++;
		     if ( c == '\t' ) break;				
		     data3 = (*pt_ram );
		}
		if ( tim1 ) printf("\r\n");
		if ( c == '\t' ) break;
		data3 = *pt_ram;
		*pt_ram = data3 & (~dt);	
		i++;
		if (i==60000){
			printf(".");
			i = 0;
		}
		j++;
	     }
             if ( c == '\t' ) break;
	}
	printf("\r\n");

	printf("\r\nCiclo in cui si rilegge 0 e si scrive 1 bit per bit decrementando indirizzo\r\n");
	for ( pt_ram = endadd; pt_ram >= startadd; pt_ram--)
	{
	     j = 0;	
	     for (dt = 0x8000; dt != 0x0000; dt = dt >> 1)
	     {		
		k = 10000;
		tim1 = 0;
		vrf = ((data3 = (*pt_ram & dt)) != 0);
   		printf("@%06lX  %04X\r",pt_ram,*(pt_ram)  );
		while( ((c=_getkey())  != '\r') && vrf ) {
		     er = 1;
		     if ( k == 10000) 
		     {	   
			app = (unsigned long) pt_ram;
			off = (unsigned int)(app & 0x00FFFF);
			app = app >> 16;
			seg = (unsigned int)(app & 0x0000FF);
			printf("\rKO!!! Ad Address %02x%04x Bit %02d atteso 0 letto %04x",seg,off,j,data3);
			k = 0;
			tim1 = 1;
		     }
		     k++;
		     if ( c == '\t' ) break;				
		     data3 = (*pt_ram );
		}
		if ( tim1 ) printf("\r\n");
		if ( c == '\t' ) break;
		data3 = *pt_ram;
		*pt_ram = data3 | dt;	
		i++;
		if (i==60000){
			printf(".");
			i = 0;
		}
		j++;
	     }
             if ( c == '\t' ) break;
	}
	printf("\r\n");

	printf("\r\nCiclo in cui si rilegge 1 e si scrive 0 bit per bit decrementando indirizzo\r\n");
	for ( pt_ram = endadd; pt_ram >= startadd; pt_ram--)
	{
	     j = 0;	
	     for (dt = 0x8000; dt != 0x0000; dt = dt >> 1)
	     {		
		k = 10000;
		tim1 = 0;
		vrf = ((data3 = (*pt_ram & dt)) == 0);
   		printf("@%06lX  %04X\r",pt_ram,*(pt_ram)  );
		while( ((c=_getkey())  != '\r') && vrf ) {
		     er = 1;
		     if ( k == 10000) 
		     {	   
			app = (unsigned long) pt_ram;
			off = (unsigned int)(app & 0x00FFFF);
			app = app >> 16;
			seg = (unsigned int)(app & 0x0000FF);
			printf("\rKO!!! Ad Address %02x%04x Bit %02d atteso 1 letto %04x",seg,off,j,data3);
			k = 0;
			tim1 = 1;
		     }
		     k++;
		     if ( c == '\t' ) break;				
		     data3 = (*pt_ram);
		}
		if ( tim1 ) printf("\r\n");
		if ( c == '\t' ) break;
		data3 = *pt_ram;
		*pt_ram = data3 & (~dt);	
		i++;
		if (i==60000){
			printf(".");
			i = 0;
		}
		j++;
	     }
             if ( c == '\t' ) break;
	}
	printf("\r\n");

	printf("\r\nCiclo in cui si rilegge 0 l'intera ram\r\n");
	for ( pt_ram = startadd; pt_ram <= endadd; pt_ram++)
	{
	     j = 0;	
	     for (dt = 0x0001; dt != 0x0000; dt = dt << 1)
	     {		
		k = 10000;
		tim1 = 0;
		vrf = ((data3 = (*pt_ram & dt)) != 0);
   		printf("@%06lX  %04X\r",pt_ram,*(pt_ram)  );
		while(  ((c=_getkey())  != '\r') && vrf ) {
		     er = 1;
		     if ( k == 10000) 
		     {	   
			app = (unsigned long) pt_ram;
			off = (unsigned int)(app & 0x00FFFF);
			app = app >> 16;
			seg = (unsigned int)(app & 0x0000FF);
			printf("\rKO!!! Ad Address %02x%04x Bit %02d atteso 0 letto %04x",seg,off,j,data3);
			k = 0;
			tim1 = 1;
		     }
		     k++;
		     if ( c == '\t' ) break;				
		     data3 = (*pt_ram);
		}
		if ( tim1 ) printf("\r\n");
		if ( c == '\t' ) break;
		i++;
		if (i==60000){
			printf(".");
			i = 0;
		}
		j++;
	     }
             if ( c == '\t' ) break;
	}
	printf("\r\n");

	return er;
/*	if (er) printf("\r\nKO!!!!!!!!\r\n");
	else    printf("\r\nOK\r\n");
*/
}


/**********************************************************/
/* Funzione test ram massimo rumore. Scrive in modo       */
/* alternato AAAA e 5555	                          */
/* Variabili di ingresso:                                 */
/*   startadd    indirizzo di start della ram             */
/*   endadd      indirizzo di fine della ram              */
/**********************************************************/
int maxrum(unsigned int  * startadd, unsigned int  * endadd)
{
	unsigned int data3,vrf,tim1,i,k,dt;
	int  er;
	unsigned int  * pt_ram;
	unsigned int  off,seg;
	unsigned long app; 
	char c;

	printf("TEST MASSIMO RUMORE\r\n");
	er = 0;
	i=0;
	c = 0;
	/*** ciclo di scrittura e verifica su tutti gli indirizzi ***/
	dt = 0xAAAA;
	*startadd = dt;
	for ( pt_ram = (startadd +1); pt_ram <= endadd; pt_ram++)
	{
		*pt_ram = ~dt;
		k = 10000;
		tim1 = 0;
		vrf = ((data3 = *(pt_ram-1))  != dt);
   		printf("@%06lX  %04X\r",pt_ram,*(pt_ram)  );
		while( ((c=_getkey())  != '\r') && vrf ) {
		     er = 1;
		     if ( k == 10000) 
		     {	   
			app = (unsigned long) pt_ram;
			off = (unsigned int)(app & 0x00FFFF);
			app = app >> 16;
			seg = (unsigned int)(app & 0x0000FF);
			printf("\rKO!!! Ad Address %02x%04x atteso %04x letto %04x",seg,off,dt,data3);
			k = 0;
			tim1 = 1;
		     }
		     k++;
		     if ( c == '\t' ) break;				
		     *pt_ram = ~dt;
		     data3 = *(pt_ram-1);
		}
		if ( tim1 ) printf("\r\n");
		if ( c == '\t' ) break;	
		i++;
		if (i==20000){
			printf(".");
			i = 0;
		}
		dt = ~dt;
	}
	printf("\r\n");

	dt = 0x5555;
	*startadd = dt;
	for ( pt_ram = (startadd +1); pt_ram <= endadd; pt_ram++)
	{
		*pt_ram = ~dt;
		k = 10000;
		tim1 = 0;
		vrf = ((data3 = *(pt_ram-1))  != dt);
   		printf("@%06lX  %04X\r",pt_ram,*(pt_ram)  );
		while( ((c=_getkey())  != '\r') && vrf ) {
		     er = 1;
		     if ( k == 10000) 
		     {	   
			app = (unsigned long) pt_ram;
			off = (unsigned int)(app & 0x00FFFF);
			app = app >> 16;
			seg = (unsigned int)(app & 0x0000FF);
			printf("\rKO!!! Ad Address %02x%04x atteso %04x letto %04x",seg,off,dt,data3);
			k = 0;
			tim1 = 1;
		     }
		     k++;
		     if ( c == '\t' ) break;				
		     *pt_ram = ~dt;
		     data3 = *(pt_ram-1);
		}
		if ( tim1 ) printf("\r\n");
		if ( c == '\t' ) break;	
		i++;
		if (i==20000){
			printf(".");
			i = 0;
		}
		dt = ~dt;
	}
	printf("\r\n");

	return er;
/*	if (er) printf("\r\nKO!!!!!!!!\r\n");
	else    printf("\r\nOK\r\n");
*/
}


/**********************************************************/
/* Funzione test ram shift degli 0                        */
/* Variabili di ingresso:                                 */
/*   startadd    indirizzo di start della ram             */
/*   endadd      indirizzo di fine della ram              */
/**********************************************************/
int shf0_mvb(unsigned int  * startadd, unsigned int  * endadd)
{
	unsigned int data3,tim1,i,k,dt,vrf;
	int  er;
	unsigned int  * pt_ram;
	unsigned int  off,seg;
	unsigned long app; 
	char c;

	saeaddmvb1 = saeaddmvb + 1;	
	printf("TEST SHIFT DEGLI 1 TRA GLI 0 NEL DATO\r\n");
	er = 0;
	i=0;
	c = 0;
	/*** ciclo di scrittura e verifica su tutti gli indirizzi ***/
	for ( pt_ram = (startadd +1); pt_ram <= endadd; pt_ram++)
	{
	     if (pt_ram == sasaddmvb) pt_ram = saeaddmvb1;	
	/***    procedura di start write     ***/
	     for (dt = 0x0001; dt != 0x0000; dt = dt << 1)
	     {	
		*pt_ram = dt;
		if (pt_ram == saeaddmvb1) *(sasaddmvb-1) = 0;
		else *(pt_ram-1)=0;
		k = 10000;
		tim1 = 0;
		vrf = ((data3 = *pt_ram)  != dt);
		while( ((c=_getkey())  != '\r') && vrf ) {
		     er = 1;	
		     if ( k == 10000) 
		     {	   
			app = (unsigned long) pt_ram;
			off = (unsigned int)(app & 0x00FFFF);
			app = app >> 16;
			seg = (unsigned int)(app & 0x0000FF);
			printf("\rKO!!! Ad Address %02x%04x atteso %04x letto %04x",seg,off,dt,data3);
			k = 0;
			tim1 = 1;
		     }
		     k++;
		     if ( c == '\t' ) break;				
		     *pt_ram = dt;
		     data3 = *pt_ram;	
		}
		if ( tim1 ) printf("\r\n");
                if ( c == '\t' ) break;				
		i++;
		if (i==60000){
			printf(".");
			i = 0;
		}
	     }
             if ( c == '\t' ) 
	     {	
		er = 1;
		break;
	     }					
	}
	printf("\r\n");
	return er;
}



/**********************************************************/
/* Funzione test ram shift degli 1                        */
/* Variabili di ingresso:                                 */
/*   startadd    indirizzo di start della ram             */
/*   endadd      indirizzo di fine della ram              */
/**********************************************************/
int shf1_mvb(unsigned int  * startadd, unsigned int  * endadd)
{
	unsigned int data3,tim1,i,k,dt,vrf;
	int  er;
	unsigned int  * pt_ram;
	unsigned int  off,seg;
	unsigned long app; 
	char c;

	saeaddmvb1 = saeaddmvb + 1;	
	printf("TEST SHIFT DEGLI 0 TRA GLI 1 NEL DATO\r\n");
	er = 0;
	i=0;
	c = 0;
	/*** ciclo di scrittura e verifica su tutti gli indirizzi ***/
	for ( pt_ram = (startadd +1); pt_ram <= endadd; pt_ram++)
	{
	     if (pt_ram == sasaddmvb) pt_ram = saeaddmvb1;	
	/***    procedura di start write     ***/
	     for (dt = 0x0001; dt != 0x0000; dt = dt << 1)
	     {		
		*pt_ram = ~dt;
		if (pt_ram == saeaddmvb1) *(sasaddmvb-1) = 0;
		else *(pt_ram-1)=0;
		k = 10000;
		tim1 = 0;
		vrf = ((data3 = *pt_ram)  != (~dt));
		while( ((c=_getkey())  != '\r') && vrf ) {
		     er = 1;	
		     if ( k == 10000) 
		     {	   
			app = (unsigned long) pt_ram;
			off = (unsigned int)(app & 0x00FFFF);
			app = app >> 16;
			seg = (unsigned int)(app & 0x0000FF);
			printf("\rKO!!! Ad Address %02x%04x atteso %04x letto %04x",seg,off,~dt,data3);
			k = 0;
			tim1 = 1;
		     }
		     k++;
		     if ( c == '\t' ) break; 
	             *pt_ram = ~dt;
		     data3 = *pt_ram;					
		}
		if ( tim1 ) printf("\r\n");
		if ( c == '\t' ) break;	
		i++;
		if (i==60000){
			printf(".");
			i = 0;
		}
	     }
             if ( c == '\t' ) 
	     {	
		er = 1;
		break;
	     }					
	}
	printf("\r\n");
	return er;
}



/**********************************************************/
/* Funzione test ram March C-                             */
/* Variabili di ingresso:                                 */
/*   startadd    indirizzo di start della ram             */
/*   endadd      indirizzo di fine della ram              */
/**********************************************************/
int march_mvb(unsigned int  * startadd, unsigned int  * endadd)
{
	unsigned int data3,tim1,i,k,dt,j,vrf;
	int  er;
	unsigned int  * pt_ram;
	unsigned int  off,seg;
	unsigned long app; 
	char c;

	saeaddmvb1 = saeaddmvb + 1;	
	printf("TEST MARCH C-\r\n");
	er = 0;
	i=0;
	c = 0;
	/*** ciclo I  azzera dato ***/
	printf("Ciclo azzeramento RAM\r\n");
	for ( pt_ram = startadd; pt_ram <= endadd; pt_ram++) 
	{
	     if (pt_ram == sasaddmvb) pt_ram = saeaddmvb1;	
	     *pt_ram = 0;
	}
	printf("\r\n");

	printf("Ciclo in cui si rilegge 0 e si scrive 1 bit per bit incrementando indirizzo\r\n");
	for ( pt_ram = startadd; pt_ram <= endadd; pt_ram++)
	{
	     j = 0;	
	     if (pt_ram == sasaddmvb) pt_ram = saeaddmvb1;	
	     for (dt = 0x0001; dt != 0x0000; dt = dt << 1)
	     {		
		k = 10000;
		tim1 = 0;
		vrf =  ((((data3 = *pt_ram) & dt) != 0));
		while( ((c=_getkey())  != '\r') && vrf ) {
		     er = 1;	
		     if ( k == 10000) 
		     {	   
			app = (unsigned long) pt_ram;
			off = (unsigned int)(app & 0x00FFFF);
			app = app >> 16;
			seg = (unsigned int)(app & 0x0000FF);
			printf("\rKO!!! Ad Address %02x%04x Bit %02d atteso 0 letto %04x",seg,off,j,data3);
			k = 0;
			tim1 = 1;
		     }
		     k++;
		     if ( c == '\t' ) break;
		     data3 = (*pt_ram );				
		}
		if ( tim1 ) printf("\r\n");
		if ( c == '\t' ) break;
		data3 = *pt_ram;
		*pt_ram = data3 | dt;	
		i++;
		if (i==60000){
			printf(".");
			i = 0;
		}
		j++;
	     }
             if ( c == '\t' ) 
	     {	
		er = 1;
		break;
	     }					
	}
	printf("\r\n");

	printf("\r\nCiclo in cui si rilegge 1 e si scrive 0 bit per bit incrementando indirizzo\r\n");
	for ( pt_ram = startadd; pt_ram <= endadd; pt_ram++)
	{
	     j = 0;	
	     if (pt_ram == sasaddmvb) pt_ram = saeaddmvb1;	
	     for (dt = 0x0001; dt != 0x0000; dt = dt << 1)
	     {		
		k = 10000;
		tim1 = 0;
		vrf = ((((data3 = *pt_ram) & dt) == 0));
		while( ((c=_getkey())  != '\r') && vrf ) {
		     er = 1;	
		     if ( k == 10000) 
		     {	   
			app = (unsigned long) pt_ram;
			off = (unsigned int)(app & 0x00FFFF);
			app = app >> 16;
			seg = (unsigned int)(app & 0x0000FF);
			printf("\rKO!!! Ad Address %02x%04x Bit %02d atteso 1 letto %04x",seg,off,j,data3);
			k = 0;
			tim1 = 1;
		     }
		     k++;
		     if ( c == '\t' ) break;				
		     data3 = (*pt_ram );
		}
		if ( tim1 ) printf("\r\n");
		if ( c == '\t' ) break;
		data3 = *pt_ram;
		*pt_ram = data3 & (~dt);	
		i++;
		if (i==60000){
			printf(".");
			i = 0;
		}
		j++;
	     }
             if ( c == '\t' ) 
	     {	
		er = 1;
		break;
	     }					
	}
	printf("\r\n");

	printf("\r\nCiclo in cui si rilegge 0 e si scrive 1 bit per bit decrementando indirizzo\r\n");
	for ( pt_ram = endadd; pt_ram >= startadd; pt_ram--)
	{
	     j = 0;	
	     for (dt = 0x8000; dt != 0x0000; dt = dt >> 1)
	     {		
		k = 10000;
		tim1 = 0;
		vrf = ((((data3 = *pt_ram) & dt) != 0));
		while( ((c=_getkey())  != '\r') && vrf ) {
		     er = 1;	
		     if ( k == 10000) 
		     {	   
			app = (unsigned long) pt_ram;
			off = (unsigned int)(app & 0x00FFFF);
			app = app >> 16;
			seg = (unsigned int)(app & 0x0000FF);
			printf("\rKO!!! Ad Address %02x%04x Bit %02d atteso 0 letto %04x",seg,off,j,data3);
			k = 0;
			tim1 = 1;
		     }
		     k++;
		     if ( c == '\t' ) break;				
		     data3 = (*pt_ram );
		}
		if ( tim1 ) printf("\r\n");
		if ( c == '\t' ) break;
		data3 = *pt_ram;
		*pt_ram = data3 | dt;	
		i++;
		if (i==60000){
			printf(".");
			i = 0;
		}
		j++;
	     }
             if ( c == '\t' ) 
	     {	
		er = 1;
		break;
	     }					
	     if (pt_ram == saeaddmvb1) pt_ram = sasaddmvb;	
	}
	printf("\r\n");

	printf("\r\nCiclo in cui si rilegge 1 e si scrive 0 bit per bit decrementando indirizzo\r\n");
	for ( pt_ram = endadd; pt_ram >= startadd; pt_ram--)
	{
	     j = 0;	
	     for (dt = 0x8000; dt != 0x0000; dt = dt >> 1)
	     {		
		k = 10000;
		tim1 = 0;
		vrf = ((((data3 = *pt_ram) & dt) == 0));
		while( ((c=_getkey())  != '\r') && vrf ) {
		     er = 1;	
		     if ( k == 10000) 
		     {	   
			app = (unsigned long) pt_ram;
			off = (unsigned int)(app & 0x00FFFF);
			app = app >> 16;
			seg = (unsigned int)(app & 0x0000FF);
			printf("\rKO!!! Ad Address %02x%04x Bit %02d atteso 1 letto %04x",seg,off,j,data3);
			k = 0;
			tim1 = 1;
		     }
		     k++;
		     if ( c == '\t' ) break;				
		     data3 = (*pt_ram);
		}
		if ( tim1 ) printf("\r\n");
		if ( c == '\t' ) break;
		data3 = *pt_ram;
		*pt_ram = data3 & (~dt);	
		i++;
		if (i==60000){
			printf(".");
			i = 0;
		}
		j++;
	     }
             if ( c == '\t' ) 
	     {	
		er = 1;
		break;
	     }					
	     if (pt_ram == saeaddmvb1) pt_ram = sasaddmvb;	
	}
	printf("\r\n");

	printf("\r\nCiclo in cui si rilegge 0 l'intera ram\r\n");
	for ( pt_ram = startadd; pt_ram <= endadd; pt_ram++)
	{
	     if (pt_ram == sasaddmvb) pt_ram = saeaddmvb1;	
	     j = 0;	
	     for (dt = 0x0001; dt != 0x0000; dt = dt << 1)
	     {		
		k = 10000;
		tim1 = 0;
		vrf = ((((data3 = *pt_ram) & dt) != 0));
		while( ((c=_getkey())  != '\r') && vrf ) {
		     er = 1;	
		     if ( k == 10000) 
		     {	   
			app = (unsigned long) pt_ram;
			off = (unsigned int)(app & 0x00FFFF);
			app = app >> 16;
			seg = (unsigned int)(app & 0x0000FF);
			printf("\rKO!!! Ad Address %02x%04x Bit %02d atteso 0 letto %04x",seg,off,j,data3);
			k = 0;
			tim1 = 1;
		     }
		     k++;
		     if ( c == '\t' ) break;				
		     data3 = (*pt_ram);
		}
		if ( tim1 ) printf("\r\n");
		if ( c == '\t' ) break;
		i++;
		if (i==60000){
			printf(".");
			i = 0;
		}
		j++;
	     }
             if ( c == '\t' ) 
	     {	
		er = 1;
		break;
	     }					
	}
	printf("\r\n");

	return er;
}


/**********************************************************/
/* Funzione test ram massimo rumore. Scrive in modo       */
/* alternato AAAA e 5555	                          */
/* Variabili di ingresso:                                 */
/*   startadd    indirizzo di start della ram             */
/*   endadd      indirizzo di fine della ram              */
/**********************************************************/
int maxrum_mvb(unsigned int  * startadd, unsigned int  * endadd)
{
	unsigned int data3,tim1,i,k,dt,vrf;
	int  er;
	unsigned int  * pt_ram;
	unsigned int  * pt_ram1;
	unsigned int  off,seg;
	unsigned long app; 
	char c;

	saeaddmvb1 = saeaddmvb + 1;	
	printf("TEST MASSIMO RUMORE\r\n");
	er = 0;
	i=0;
	c = 0;
	/*** ciclo di scrittura e verifica su tutti gli indirizzi ***/
	dt = 0xAAAA;
	*startadd = dt;
	for ( pt_ram = (startadd +1); pt_ram <= endadd; pt_ram++)
	{
                pt_ram1 = pt_ram;	
	        if (pt_ram == sasaddmvb) pt_ram = saeaddmvb1;	
		*pt_ram = ~dt;
		k = 10000;
		tim1 = 0;
		vrf = (((data3 = *(pt_ram1-1))  != dt));
		while( ((c=_getkey())  != '\r') && vrf ) {
		     er = 1;	
		     if ( k == 10000) 
		     {	   
			app = (unsigned long) pt_ram;
			off = (unsigned int)(app & 0x00FFFF);
			app = app >> 16;
			seg = (unsigned int)(app & 0x0000FF);
			printf("\rKO!!! Ad Address %02x%04x atteso %04x letto %04x",seg,off,dt,data3);
			k = 0;
			tim1 = 1;
		     }
		     k++;
		     if ( c == '\t' ) break;				
		     *pt_ram = ~dt;
		     data3 = *(pt_ram1-1);
		}
		if ( tim1 ) printf("\r\n");
             if ( c == '\t' ) 
	     {	
		er = 1;
		break;
	     }					
		i++;
		if (i==20000){
			printf(".");
			i = 0;
		}
		dt = ~dt;
	}
	printf("\r\n");

	dt = 0x5555;
	*startadd = dt;
	for ( pt_ram = (startadd +1); pt_ram <= endadd; pt_ram++)
	{
                pt_ram1 = pt_ram;	
	        if (pt_ram == sasaddmvb) pt_ram = saeaddmvb1;	
		*pt_ram = ~dt;
		k = 10000;
		tim1 = 0;
		vrf = (((data3 = *(pt_ram1-1))  != dt)); 
		while( ((c=_getkey())  != '\r') && vrf ) {
		     er = 1;	
		     if ( k == 10000) 
		     {	   
			app = (unsigned long) pt_ram;
			off = (unsigned int)(app & 0x00FFFF);
			app = app >> 16;
			seg = (unsigned int)(app & 0x0000FF);
			printf("\rKO!!! Ad Address %02x%04x atteso %04x letto %04x",seg,off,dt,data3);
			k = 0;
			tim1 = 1;
		     }
		     k++;
		     if ( c == '\t' ) break;				
		     *pt_ram = ~dt;
		     data3 = *(pt_ram1-1);
		}
		if ( tim1 ) printf("\r\n");
             if ( c == '\t' ) 
	     {	
		er = 1;
		break;
	     }					
		i++;
		if (i==20000){
			printf(".");
			i = 0;
		}
		dt = ~dt;
	}
	printf("\r\n");

	return er;
}



/**********************************************************/
/* Funzione init MVB               */
/**********************************************************/
void MVB_init(void)
{
	unsigned short data1,i,vp;
	unsigned short  off,seg;
	unsigned long app; 



	*regscr1 = 0x0000; 						/* reset software				*/
	*regscr = 0x0000;
	for (i = 0; i<10; i++);								/* istruzione di attesa				*/
	*regscr = 0x8001;						/* predispone per l'inizializzazione dei reg.	*/
	*regmcr = funmode;						/* setta size della TM				*/
	for (i = 0; i<10; i++);								/* istruzione di attesa				*/
	
	/* azzera la LA port index table così tutti i LA port puntano al port 0 */
    for (st_DA_PITptr=(unsigned int  *) st_LA_PIT; st_DA_PITptr<=(unsigned int  *) end_LA_PIT; st_DA_PITptr++) *st_DA_PITptr = 0;	
	
	/* azzera tutti gli LA PCS per disattivare tutti gli LA port  */
    for (st_DA_PITptr=(unsigned int  *) st_LA_PCS; st_DA_PITptr<=(unsigned int  *) end_LA_PCS; st_DA_PITptr++) *st_DA_PITptr = 0;	
	
	/* azzera la DA port index table così tutti i DA port puntano al port 0 */
    for (st_DA_PITptr=(unsigned int  *) st_DA_PIT; st_DA_PITptr<=(unsigned int  *) end_DA_PIT; st_DA_PITptr++) *st_DA_PITptr = 0;	
	
	/* azzera tutti i DA PCS per disattivare tutti i DA port  */
    for (st_DA_PITptr=(unsigned int  *) st_DA_PCS; st_DA_PITptr<=(unsigned int  *) end_DA_PCS; st_DA_PITptr++) *st_DA_PITptr = 0;	
	
	*regdaor1 = 0x000D;
	data1 = *regdaor1;						/* legge il Device Address			*/ 
	
	*regdr = 0x0000;						/* consente lo switch tra le linee		*/
	printf ( "DR = %04x DA=%04x\r\n",*regdr,data1);
	
	*regdaok = 0x0094;						/* sovrascrive indirizzo hardware               */
	
	data1 = *regdaor1;						/* legge il Device Address			*/ 
	printf ( "DA=%04x\r\n",data1);
	*regimr0 = 0xFFFF;						/* abilita tutte le interrupt			*/
	*regimr1 = 0xFFFF;
	*regivr0 = 0;
	*regivr1 = 0;

     st_DA_PITptr = (unsigned int  *)(st_DA_PIT + 0x000E*2);
	*st_DA_PITptr = 0x0001;						/* utilizza il DA port n°1 per il proprio device add.*/

     st_DA_PITptr = (unsigned int  *)(st_DA_PIT + 0x000D*2);
	*st_DA_PITptr = 0x0002;						/* utilizza il DA port n°2 per il device add. della stim*/
 
     st_DA_PCSptr = (unsigned int  *)(st_DA_PCS + (0x0001<<3));
    *st_DA_PCSptr = 0xF420;		/* FC=15, sink port, no check seq., no write always, DTI1, no queue, no num data, no forcing data*/

	*(st_DA_PCSptr + 1) = 0x0000;
      st_DA_PCSptr1 = (unsigned int  *)(st_DA_PCS + (0x0002<<3));
  
     *st_DA_PCSptr1 = 0xF440;		/* FC=15, sink port, no check seq., no write always, DTI2, no queue, no num data, no forcing data*/
	*(st_DA_PCSptr1 + 1) = 0x0000;

	vp = 0;
	st_DA_DATA0ptr =  (unsigned int  *)(st_DA_DATA+(((0x0001<<4)&0xFFC0)|(vp<<5)|((0x0001<<3)&0x0018)));
	st_DA_DATA0ptr1 =  (unsigned int  *)(st_DA_DATA+(((0x0002<<4)&0xFFC0)|(vp<<5)|((0x0002<<3)&0x0018)));

	vp = 1;
	st_DA_DATA1ptr =  (unsigned int  *)(st_DA_DATA+(((0x0001<<4)&0xFFC0)|(vp<<5)|((0x0001<<3)&0x0018)));
	st_DA_DATA1ptr1 =  (unsigned int  *)(st_DA_DATA+(((0x0002<<4)&0xFFC0)|(vp<<5)|((0x0002<<3)&0x0018)));

	FC15_PCSptr = (unsigned int  *)st_FC15_PCS; 
	FC15_DATAptr = (unsigned int  *)st_FC15_DATA0;

    *FC15_PCSptr = 0xF860;		/* FC=15, source port, no check seq., no write always, DTI3, no queue, no num data, no forcing data*/
	*(FC15_PCSptr + 1) = 0x0000;
    *FC15_DATAptr = 0xAAAA;

   	set_out_port(0, DOP0_KMA|DOP0_KMB, DOP0_KMA|DOP0_KMB);

	*regscr1 = 0x8503;						/* abilito in full mode l'mvb	*/


	app = (unsigned long) st_DA_PITptr;
	off = (unsigned int)(app & 0x00FFFF);
	app = app >> 16;
	seg = (unsigned int)(app & 0x0000FF);
	printf("DA_PIT=%02x%04x\r\n",seg,off);
	app = (unsigned long) st_DA_PCSptr;
	off = (unsigned int)(app & 0x00FFFF);
	app = app >> 16;
	seg = (unsigned int)(app & 0x0000FF);
	printf("DA_PCS=%02x%04x\r\n",seg,off);
	app = (unsigned long) st_DA_DATA0ptr;
	off = (unsigned int)(app & 0x00FFFF);
	app = app >> 16;
	seg = (unsigned int)(app & 0x0000FF);
	printf("DA_DATA0=%02x%04x\r\n",seg,off);
	app = (unsigned long) st_DA_DATA1ptr;
	off = (unsigned int)(app & 0x00FFFF);
	app = app >> 16;
	seg = (unsigned int)(app & 0x0000FF);
	printf("DA_DATA1=%02x%04x\r\n",seg,off);
	app = (unsigned long) st_DA_PCSptr1;
	off = (unsigned int)(app & 0x00FFFF);
	app = app >> 16;
	seg = (unsigned int)(app & 0x0000FF);
	printf("DA_PCS1=%02x%04x\r\n",seg,off);
	app = (unsigned long) st_DA_DATA0ptr1;
	off = (unsigned int)(app & 0x00FFFF);
	app = app >> 16;
	seg = (unsigned int)(app & 0x0000FF);
	printf("DA_DATA10=%02x%04x\r\n",seg,off);
	app = (unsigned long) st_DA_DATA1ptr1;
	off = (unsigned int)(app & 0x00FFFF);
	app = app >> 16;
	seg = (unsigned int)(app & 0x0000FF);
//	printf("DA_DATA11=%02x%04x\r\n",seg,off);
////	printf("P2=%04x\r\n",P2);
//	printf("ISR0=%04x ISR1=%04x\r\n",*regisr0,*regisr1);
//	printf("FC=%04x EC=%04x\r\n",*regfc,*regec);
//	printf("PCSW0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_PCSptr,*(st_DA_PCSptr+1),*(st_DA_PCSptr+2),*(st_DA_PCSptr+3));
//	printf("DATA0W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_DATA0ptr,*(st_DA_DATA0ptr+1),*(st_DA_DATA0ptr+2),*(st_DA_DATA0ptr+3));
//	printf("DATA1W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_DATA1ptr,*(st_DA_DATA1ptr+1),*(st_DA_DATA1ptr+2),*(st_DA_DATA1ptr+3));
//	printf("PCS1W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_PCSptr1,*(st_DA_PCSptr1+1),*(st_DA_PCSptr1+2),*(st_DA_PCSptr1+3));
//	printf("DATA10W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_DATA0ptr1,*(st_DA_DATA0ptr1+1),*(st_DA_DATA0ptr1+2),*(st_DA_DATA0ptr1+3));
//	printf("DATA11W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_DATA1ptr1,*(st_DA_DATA1ptr1+1),*(st_DA_DATA1ptr1+2),*(st_DA_DATA1ptr1+3));
    
    *regisr0=0;
	*regivr0=0;
	*regisr1=0;
	*regivr1=0;
	
	*regfc=0;
	*regec=0;
	
	*st_DA_DATA0ptr=0;
	*(st_DA_DATA0ptr+1)=0;
	*(st_DA_DATA0ptr+2)=0;
	*(st_DA_DATA0ptr+3)=0;
	*st_DA_DATA1ptr=0;
	*(st_DA_DATA1ptr+1)=0;
	*(st_DA_DATA1ptr+2)=0;
	*(st_DA_DATA1ptr+3)=0;
	*st_DA_DATA0ptr1=0;
	*(st_DA_DATA0ptr1+1)=0;
	*(st_DA_DATA0ptr1+2)=0;
	*(st_DA_DATA0ptr1+3)=0;
	*st_DA_DATA1ptr1=0;
	*(st_DA_DATA1ptr1+1)=0;
	*(st_DA_DATA1ptr1+2)=0;
	*(st_DA_DATA1ptr1+3)=0;

}

/**********************************************************/
/* Funzione di test  del  mvb bus elettrico               */
/**********************************************************/
int t_mvbel (void)
{
	unsigned short data1,i,er,vp;
	unsigned long j;
	char c;
	unsigned short  off,seg,p2val,fcval,ecval,isr0val,isr1val;
	unsigned long   app; 
	unsigned short  * st_DA_PITptr;
	unsigned short  * st_DA_PCSptr;
	unsigned short  * st_DA_DATA0ptr;
	unsigned short  * st_DA_DATA1ptr;
	unsigned short  * st_DA_PCSptr1;
	unsigned short  * st_DA_DATA0ptr1;
	unsigned short  * st_DA_DATA1ptr1;
	unsigned short  * FC15_PCSptr;
	unsigned short  * FC15_DATAptr;

	er = 0;

	*regscr1 = 0x0000; 						/* reset software				*/
	*regscr = 0x0000;
	for (i = 0; i<20; i++);								/* istruzione di attesa				*/
	
	*regscr = 0x8001;						/* predispone per l'inizializzazione dei reg.	*/
	*regmcr = funmode;						/* setta size della TM				*/
	
	for (i = 0; i<20; i++);								/* istruzione di attesa				*/
	
	/* azzera la LA port index table così tutti i LA port puntano al port 0 */
        for (st_DA_PITptr=(unsigned short  *) st_LA_PIT; st_DA_PITptr<=(unsigned short  *) end_LA_PIT; st_DA_PITptr++) *st_DA_PITptr = 0;	
	/* azzera tutti gli LA PCS per disattivare tutti gli LA port  */
        for (st_DA_PITptr=(unsigned short  *) st_LA_PCS; st_DA_PITptr<=(unsigned short  *) end_LA_PCS; st_DA_PITptr++) *st_DA_PITptr = 0;	
	/* azzera la DA port index table così tutti i DA port puntano al port 0 */
        for (st_DA_PITptr=(unsigned short  *) st_DA_PIT; st_DA_PITptr<=(unsigned short  *) end_DA_PIT; st_DA_PITptr++) *st_DA_PITptr = 0;	
	/* azzera tutti i DA PCS per disattivare tutti i DA port  */
        for (st_DA_PITptr=(unsigned short  *) st_DA_PCS; st_DA_PITptr<=(unsigned short  *) end_DA_PCS; st_DA_PITptr++) *st_DA_PITptr = 0;	
	
	data1 = *regdaor1;						/* legge il Device Address			*/ 
	*regdr = 0x0009;						/* blocca lo switch tra le linee LAA=1 SLM=1		*/
	printf ( "DR = %04x DA=%04x\r\n",*regdr,data1);  
	*regdaor1 = 0x000E;                     /* sovrascrive Device address con 0x000E*/
	*regdaok = 0x0094;						/* 94 sovrascrive indirizzo hardware            */
	                                        /* 49 disabilita override indirizzo hardware    */
	data1 = *regdaor1;						/* legge il Device Address			            */ 
	printf ( "DA=%04x\r\n",data1);
	*regimr0 = 0xFFFF;						/* abilita tutte le interrupt			*/
	*regimr1 = 0xFFFF;
	*regivr0 = 0;
	*regivr1 = 0;
        st_DA_PITptr = (unsigned short  *)(st_DA_PIT + data1*2);
	*st_DA_PITptr = 0x0001;						/* utilizza il DA port n°1 per il proprio device add.*/
        st_DA_PITptr = (unsigned short  *)(st_DA_PIT + 0x000D*2);
	*st_DA_PITptr = 0x0002;						/* utilizza il DA port n°2 per il device add. della stim*/
        st_DA_PCSptr = (unsigned short  *)(st_DA_PCS + (0x0001<<3));
        *st_DA_PCSptr = 0xF420;		/* FC=15, sink port, no check seq., no write always, DTI1 abilitato, no queue, no num data, no forcing data*/
	*(st_DA_PCSptr + 1) = 0x0000;
        st_DA_PCSptr1 = (unsigned short  *)(st_DA_PCS + (0x0002<<3));
        *st_DA_PCSptr1 = 0xF440;		/* FC=15, sink port, no check seq., no write always, DTI2 abilitato, no queue, no num data, no forcing data*/
	*(st_DA_PCSptr1 + 1) = 0x0000;
	vp = 0;
	st_DA_DATA0ptr =  (unsigned short  *)(st_DA_DATA+(((0x0001<<4)&0xFFC0)|(vp<<5)|((0x0001<<3)&0x0018)));
	st_DA_DATA0ptr1 =  (unsigned short  *)(st_DA_DATA+(((0x0002<<4)&0xFFC0)|(vp<<5)|((0x0002<<3)&0x0018)));
	vp = 1;
	st_DA_DATA1ptr =  (unsigned short  *)(st_DA_DATA+(((0x0001<<4)&0xFFC0)|(vp<<5)|((0x0001<<3)&0x0018)));
	st_DA_DATA1ptr1 =  (unsigned short  *)(st_DA_DATA+(((0x0002<<4)&0xFFC0)|(vp<<5)|((0x0002<<3)&0x0018)));
	FC15_PCSptr = (unsigned short  *)st_FC15_PCS; 
	FC15_DATAptr = (unsigned short  *)st_FC15_DATA0;
        *FC15_PCSptr = 0xF860;		/* FC=15, source port, no check seq., no write always, DTI3, no queue, no num data, no forcing data*/
	*(FC15_PCSptr + 1) = 0x0000;
        *FC15_DATAptr = 0x0001;
	
	*regmfs = data1 | 0xF000; 			        /* master frame di richiesta proprio stato*/ 
	*regmr = 0;
/* GUGU da verificare*/	
//	p2val = (P2 & 0x0600);
//	if (p2val != 0x0600)
//	{
//		printf("KO!!!    P2=%04x\r\n",p2val);
//		er = 1;	
//	}

//	P3_2 = 1;                                                      /* chiude relay KMA linea 1 MVB           */
//	P3_3 = 1;                                                      /* chiude relay KMB linea 2 MVB           */
	/* connect to the MVB line */
	set_out_port(0, DOP0_KMA|DOP0_KMB, DOP0_KMA|DOP0_KMB);
	*regscr1 = 0x050B;						/* predispone per l'inizializzazione dei reg.	*/
                                            /* 	*/

	while  ((c=_getkey()) != '\r') {
    	switch (c) {
    	case '1':
    //		 P3_2 = 1;
        	/* connect to the MVB line */
    //    	set_out_port(0, DOP0_KMA|DOP0_KMB, DOP0_KMA|DOP0_KMB);
        	set_out_port(0, DOP0_KMA, DOP0_KMA);
    		 printf("%c",c);	
    		 break;
    	case '2':
    //		 P3_2 = 0;
        	set_out_port(0, DOP0_KMA, 0);
    		 printf("%c",c);	
    		 break;
    	case '3':
    //		 P3_3 = 1;
        	set_out_port(0, DOP0_KMB, DOP0_KMB);
    		 printf("%c",c);	
    		 break;
    	case '4':
    //		 P3_3 = 0;
        	set_out_port(0, DOP0_KMB, 0);
    		 printf("%c",c);	
    		 break;
    	}
	}

	app = (unsigned long) st_DA_PITptr;
	off = (unsigned int)(app & 0x00FFFF);
	app = app >> 16;
	seg = (unsigned int)(app & 0x0000FF);
	printf("DA_PIT=%02x%04x\r\n",seg,off);
	app = (unsigned long) st_DA_PCSptr;
	off = (unsigned int)(app & 0x00FFFF);
	app = app >> 16;
	seg = (unsigned int)(app & 0x0000FF);
	printf("DA_PCS=%02x%04x\r\n",seg,off);
	app = (unsigned long) st_DA_DATA0ptr;
	off = (unsigned int)(app & 0x00FFFF);
	app = app >> 16;
	seg = (unsigned int)(app & 0x0000FF);
	printf("DA_DATA0=%02x%04x\r\n",seg,off);
	app = (unsigned long) st_DA_DATA1ptr;
	off = (unsigned int)(app & 0x00FFFF);
	app = app >> 16;
	seg = (unsigned int)(app & 0x0000FF);
	printf("DA_DATA1=%02x%04x\r\n",seg,off);
	app = (unsigned long) st_DA_PCSptr1;
	off = (unsigned int)(app & 0x00FFFF);
	app = app >> 16;
	seg = (unsigned int)(app & 0x0000FF);
	printf("DA_PCS1=%02x%04x\r\n",seg,off);
	app = (unsigned long) st_DA_DATA0ptr1;
	off = (unsigned int)(app & 0x00FFFF);
	app = app >> 16;
	seg = (unsigned int)(app & 0x0000FF);
	printf("DA_DATA10=%02x%04x\r\n",seg,off);
	app = (unsigned long) st_DA_DATA1ptr1;
	off = (unsigned int)(app & 0x00FFFF);
	app = app >> 16;
	seg = (unsigned int)(app & 0x0000FF);
	printf("DA_DATA11=%02x%04x\r\n",seg,off);

//GUGU	printf("P2=%04x ",P2);
	printf("ISR0=%04x ISR1=%04x FC=%04x EC=%04x\r\n",*regisr0,*regisr1,*regfc,*regec);
	printf("PCSW0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_PCSptr,*(st_DA_PCSptr+1),*(st_DA_PCSptr+2),*(st_DA_PCSptr+3));
	printf("DATA0W0-W1-W2-W3=%04x-%04x-%04x-%04x ",*st_DA_DATA0ptr,*(st_DA_DATA0ptr+1),*(st_DA_DATA0ptr+2),*(st_DA_DATA0ptr+3));
	printf("DATA1W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_DATA1ptr,*(st_DA_DATA1ptr+1),*(st_DA_DATA1ptr+2),*(st_DA_DATA1ptr+3));
	printf("PCS1W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_PCSptr1,*(st_DA_PCSptr1+1),*(st_DA_PCSptr1+2),*(st_DA_PCSptr1+3));
	printf("DATA10W0-W1-W2-W3=%04x-%04x-%04x-%04x ",*st_DA_DATA0ptr1,*(st_DA_DATA0ptr1+1),*(st_DA_DATA0ptr1+2),*(st_DA_DATA0ptr1+3));
	printf("DATA11W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_DATA1ptr1,*(st_DA_DATA1ptr1+1),*(st_DA_DATA1ptr1+2),*(st_DA_DATA1ptr1+3));

	for (i = 0; i<30000; i++);					/* istruzione di attesa				*/

        *regisr0=0;
	*regivr0=0;
	*regisr1=0;
	*regivr1=0;
	*regfc=0;
	*regec=0;
	*st_DA_DATA0ptr=0;
	*(st_DA_DATA0ptr+1)=0;
	*(st_DA_DATA0ptr+2)=0;
	*(st_DA_DATA0ptr+3)=0;
	*st_DA_DATA1ptr=0;
	*(st_DA_DATA1ptr+1)=0;
	*(st_DA_DATA1ptr+2)=0;
	*(st_DA_DATA1ptr+3)=0;
	*st_DA_DATA0ptr1=0;
	*(st_DA_DATA0ptr1+1)=0;
	*(st_DA_DATA0ptr1+2)=0;
	*(st_DA_DATA0ptr1+3)=0;
	*st_DA_DATA1ptr1=0;
	*(st_DA_DATA1ptr1+1)=0;
	*(st_DA_DATA1ptr1+2)=0;
	*(st_DA_DATA1ptr1+3)=0;


/* test del loop interno con interrogazione del proprio stato e KMA e KMB chiusi */
	for (i = 0; i<20000; i++);					/* istruzione di attesa				*/
	printf("test del loop interno con interrogazione del proprio stato e KMA e KMB chiusi\r\n");
	for (off=0; off != 0x1FFE; off++)	/*while ( (c=_getkey())!= '\t')*/
	{
//		i = 0;
//		j = (unsigned long) 600000;
		while (((c=_getkey())!= '\t') && ((*regmr & 0x0200) != 0)){
//			if ( j == (unsigned long) 600000) {
//			       	printf ("\r%04x mr %04x",i,*regmr);
//			       	j = 0;  
//				i = ~i;
//			}
//			j++;
		} 
		if ( c == '\t') break; 
	        *FC15_DATAptr = *FC15_DATAptr + 1;
		*regmr=0x0020;					/* invia master frame manualmente */
	}
	printf("\r\n");
	for (i = 0; i<20000; i++);					/* istruzione di attesa				*/
//GUGU	p2val = (P2 & 0x0600);
	isr0val = *regisr0;
	isr1val = *regisr1;
	fcval = *regfc;
	ecval = *regec;
        *regisr0=0;
	*regivr0=0;
	*regisr1=0;
	*regivr1=0;
	*regfc=0;
	*regec=0;
	if ( (p2val != 0x0400) | (isr0val != 0x0685) | (isr1val != 0) | (fcval != 0x3FFC) | ( ecval > 10) | (*st_DA_DATA0ptr == 0) | ( *st_DA_DATA1ptr == 0) | (*st_DA_DATA0ptr1 != 0) | (*st_DA_DATA1ptr1 != 0) )
	{
// gugu		printf("KO!!!!!!    P2=%04x ",p2val);
		printf("KO!!!!!!    ");
		printf("ISR0=%04x ISR1=%04x FC=%04x EC=%04x\r\n",isr0val,isr1val,fcval,ecval);
		printf("PCSW0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_PCSptr,*(st_DA_PCSptr+1),*(st_DA_PCSptr+2),*(st_DA_PCSptr+3));
		printf("DATA0W0-W1-W2-W3=%04x-%04x-%04x-%04x ",*st_DA_DATA0ptr,*(st_DA_DATA0ptr+1),*(st_DA_DATA0ptr+2),*(st_DA_DATA0ptr+3));
    		printf("DATA1W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_DATA1ptr,*(st_DA_DATA1ptr+1),*(st_DA_DATA1ptr+2),*(st_DA_DATA1ptr+3));
		printf("PCS1W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_PCSptr1,*(st_DA_PCSptr1+1),*(st_DA_PCSptr1+2),*(st_DA_PCSptr1+3));
		printf("DATA10W0-W1-W2-W3=%04x-%04x-%04x-%04x ",*st_DA_DATA0ptr1,*(st_DA_DATA0ptr1+1),*(st_DA_DATA0ptr1+2),*(st_DA_DATA0ptr1+3));
		printf("DATA11W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_DATA1ptr1,*(st_DA_DATA1ptr1+1),*(st_DA_DATA1ptr1+2),*(st_DA_DATA1ptr1+3));
		er = 1;
	}
	*st_DA_DATA0ptr=0;
	*(st_DA_DATA0ptr+1)=0;
	*(st_DA_DATA0ptr+2)=0;
	*(st_DA_DATA0ptr+3)=0;
	*st_DA_DATA1ptr=0;
	*(st_DA_DATA1ptr+1)=0;
	*(st_DA_DATA1ptr+2)=0;
	*(st_DA_DATA1ptr+3)=0;
	*st_DA_DATA0ptr1=0;
	*(st_DA_DATA0ptr1+1)=0;
	*(st_DA_DATA0ptr1+2)=0;
	*(st_DA_DATA0ptr1+3)=0;
	*st_DA_DATA1ptr1=0;
	*(st_DA_DATA1ptr1+1)=0;
	*(st_DA_DATA1ptr1+2)=0;
	*(st_DA_DATA1ptr1+3)=0;

/*gugu 
	p2val = (P2 & 0x0600);
	if (p2val != 0x0600)
	{
		printf("KO!!!    P2=%04x\r\n",(P2 & 0x0600));
		er = 1;	
	}
*/	
	/* open to the MVB line */
	set_out_port(0, DOP0_KMA|DOP0_KMB, 0);

/*	while ( _getkey() != '\r' );  */
/* test del loop interno con interrogazione del proprio stato e KMA e KMB aperti */
	printf("test del loop interno con interrogazione del proprio stato e KMA e KMB aperti\r\n");
	for (i = 0; i<20000; i++);					/* istruzione di attesa				*/
	for (off=0; off != 0x1FFE; off++)	/*while ( (c=_getkey())!= '\t')*/
	{
//		i = 0;
//		j = (unsigned long) 600000;
		while (((c=_getkey())!= '\t') && ((*regmr & 0x0200) != 0) ){
//			if ( j == (unsigned long) 600000) {
//			       	printf ("\r%04x mr %04x",i,*regmr);
//			       	j = 0;
//				i = ~i;  
//			}
//			j++;
		} 
		if ( c == '\t') break; 
	        *FC15_DATAptr = *FC15_DATAptr + 1;
		*regmr=0x0020;					/* invia master frame manualmente */
	}
	printf("\r\n");
	for (i = 0; i<20000; i++);					/* istruzione di attesa				*/
//gugu	p2val = (P2 & 0x0600);
	isr0val = *regisr0;
	isr1val = *regisr1;
	fcval = *regfc;
	ecval = *regec;
        *regisr0=0;
	*regivr0=0;
	*regisr1=0;
	*regivr1=0;
	*regfc=0;
	*regec=0;
	if ( (p2val != 0x0400) | (isr0val != 0x0685) | (isr1val != 0) | (fcval != 0x3FFC) | ( ecval > 10) | (*st_DA_DATA0ptr == 0) | ( *st_DA_DATA1ptr == 0) | (*st_DA_DATA0ptr1 != 0) | (*st_DA_DATA1ptr1 != 0))
	{
		printf("KO!!!!!!   ");
//gugu		printf("KO!!!!!!    P2=%04x ",p2val);
		printf("ISR0=%04x ISR1=%04x FC=%04x EC=%04x\r\n",isr0val,isr1val,fcval,ecval);
		printf("PCSW0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_PCSptr,*(st_DA_PCSptr+1),*(st_DA_PCSptr+2),*(st_DA_PCSptr+3));
		printf("DATA0W0-W1-W2-W3=%04x-%04x-%04x-%04x ",*st_DA_DATA0ptr,*(st_DA_DATA0ptr+1),*(st_DA_DATA0ptr+2),*(st_DA_DATA0ptr+3));
    		printf("DATA1W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_DATA1ptr,*(st_DA_DATA1ptr+1),*(st_DA_DATA1ptr+2),*(st_DA_DATA1ptr+3));
		printf("PCS1W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_PCSptr1,*(st_DA_PCSptr1+1),*(st_DA_PCSptr1+2),*(st_DA_PCSptr1+3));
		printf("DATA10W0-W1-W2-W3=%04x-%04x-%04x-%04x ",*st_DA_DATA0ptr1,*(st_DA_DATA0ptr1+1),*(st_DA_DATA0ptr1+2),*(st_DA_DATA0ptr1+3));
		printf("DATA11W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_DATA1ptr1,*(st_DA_DATA1ptr1+1),*(st_DA_DATA1ptr1+2),*(st_DA_DATA1ptr1+3));
		er = 1;
	}
	*st_DA_DATA0ptr=0;
	*(st_DA_DATA0ptr+1)=0;
	*(st_DA_DATA0ptr+2)=0;
	*(st_DA_DATA0ptr+3)=0;
	*st_DA_DATA1ptr=0;
	*(st_DA_DATA1ptr+1)=0;
	*(st_DA_DATA1ptr+2)=0;
	*(st_DA_DATA1ptr+3)=0;
	*st_DA_DATA0ptr1=0;
	*(st_DA_DATA0ptr1+1)=0;
	*(st_DA_DATA0ptr1+2)=0;
	*(st_DA_DATA0ptr1+3)=0;
	*st_DA_DATA1ptr1=0;
	*(st_DA_DATA1ptr1+1)=0;
	*(st_DA_DATA1ptr1+2)=0;
	*(st_DA_DATA1ptr1+3)=0;
////gugu
//	p2val = (P2 & 0x0600);
//	if (p2val != 0x0600)
//	{
//		printf("KO!!!    P2=%04x\r\n",(P2 & 0x0600));
//		er = 1;	
//	}


//	P3_2 = 1;                                                      /* chiude relay KMA linea 1 MVB           */
//	P3_3 = 1;                                                      /* chiude relay KMB linea 2 MVB           */
    	set_out_port(0, DOP0_KMA|DOP0_KMB, DOP0_KMA|DOP0_KMB);
	*regmfs = 0xF00D;   		                               /* master frame di richiesta stato della stim*/
/*	while ( _getkey() != '\r' );    */
/* test esterno KMA=1 e KMB=1  */
	printf("test esterno linea 1 KMA=1 e KMB=1\r\n");
	for (i = 0; i<20000; i++);					/* istruzione di attesa				*/
	for (off=0; off != 0x1FFE; off++)	/*while ( (c=_getkey())!= '\t')*/
	{
//		i = 0;
//		j = (unsigned long) 600000;
		while (((c=_getkey())!= '\t') && ((*regmr & 0x0200) != 0)){
//			if ( j == (unsigned long) 600000) {
//			       	printf ("\r%04x mr %04x",i,*regmr);
//			       	j = 0;  
//				i = ~i;
//			}
//			j++;
		}
		if ( c == '\t') break; 
		*regmr=0x0020;					/* invia master frame manualmente */
	}
	printf("\r\n");
	for (i = 0; i<20000; i++);					/* istruzione di attesa				*/
//gugu	p2val = (P2 & 0x0600);
	isr0val = *regisr0;
	isr1val = *regisr1;
	fcval = *regfc;
	ecval = *regec;
        *regisr0=0;
	*regivr0=0;
	*regisr1=0;
	*regivr1=0;
	*regfc=0;
	*regec=0;
	if ( (p2val != 0x0400) | (isr0val != 0x0682) | (isr1val != 0) | (fcval != 0x3FFC) | ( ecval > 10) | (*st_DA_DATA0ptr != 0) | ( *st_DA_DATA1ptr != 0) | (*st_DA_DATA0ptr1 != 0xAAAA) | (*st_DA_DATA1ptr1 != 0xAAAA))
	{
//gugu		printf("KO!!!!!!    P2=%04x ",p2val);
        printf("KO!!!!!!    ");
		printf("ISR0=%04x ISR1=%04x FC=%04x EC=%04x\r\n",isr0val,isr1val,fcval,ecval);
		printf("PCSW0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_PCSptr,*(st_DA_PCSptr+1),*(st_DA_PCSptr+2),*(st_DA_PCSptr+3));
		printf("DATA0W0-W1-W2-W3=%04x-%04x-%04x-%04x ",*st_DA_DATA0ptr,*(st_DA_DATA0ptr+1),*(st_DA_DATA0ptr+2),*(st_DA_DATA0ptr+3));
    		printf("DATA1W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_DATA1ptr,*(st_DA_DATA1ptr+1),*(st_DA_DATA1ptr+2),*(st_DA_DATA1ptr+3));
		printf("PCS1W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_PCSptr1,*(st_DA_PCSptr1+1),*(st_DA_PCSptr1+2),*(st_DA_PCSptr1+3));
		printf("DATA10W0-W1-W2-W3=%04x-%04x-%04x-%04x ",*st_DA_DATA0ptr1,*(st_DA_DATA0ptr1+1),*(st_DA_DATA0ptr1+2),*(st_DA_DATA0ptr1+3));
		printf("DATA11W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_DATA1ptr1,*(st_DA_DATA1ptr1+1),*(st_DA_DATA1ptr1+2),*(st_DA_DATA1ptr1+3));
		er = 1;
	}
	*st_DA_DATA0ptr=0;
	*(st_DA_DATA0ptr+1)=0;
	*(st_DA_DATA0ptr+2)=0;
	*(st_DA_DATA0ptr+3)=0;
	*st_DA_DATA1ptr=0;
	*(st_DA_DATA1ptr+1)=0;
	*(st_DA_DATA1ptr+2)=0;
	*(st_DA_DATA1ptr+3)=0;
	*st_DA_DATA0ptr1=0;
	*(st_DA_DATA0ptr1+1)=0;
	*(st_DA_DATA0ptr1+2)=0;
	*(st_DA_DATA0ptr1+3)=0;
	*st_DA_DATA1ptr1=0;
	*(st_DA_DATA1ptr1+1)=0;
	*(st_DA_DATA1ptr1+2)=0;
	*(st_DA_DATA1ptr1+3)=0;
/*gugu
	p2val = (P2 & 0x0600);
	if (p2val != 0x0600)
	{
		printf("KO!!!    P2=%04x\r\n",(P2 & 0x0600));
		er = 1;	
	}
*/
//	P3_2 = 0;                                                      /* apre relay KMA linea 1 MVB           */
//	P3_3 = 1;                                                      /* chiude relay KMB linea 2 MVB           */
   	/* connect to the MVB line */
   	set_out_port(0, DOP0_KMA, 0);
   	set_out_port(0, DOP0_KMB, DOP0_KMB);

/*	while ( _getkey() != '\r' ); */
/* test esterno KMA=0 e KMB=1  */
	printf("test esterno linea 1 KMA=0 e KMB=1\r\n");
	for (i = 0; i<20000; i++);					/* istruzione di attesa				*/
	for (off=0; off != 0x1FFE; off++)	/*while ( (c=_getkey())!= '\t')*/
	{
//		i = 0;
//		j = (unsigned long) 600000;
		while (((*regmr & 0x0200) != 0) && ((c=_getkey())!= '\t')){
//			if ( j == (unsigned long) 600000) {
//			       	printf ("\r%04x mr %04x",i,*regmr);
//			       	j = 0; 
//				i = ~i; 
//			}
//			j++;
		}
		if ( c == '\t') break; 
		*regmr=0x0020;					/* invia master frame manualmente */
	}
	printf("\r\n");
	for (i = 0; i<20000; i++);					/* istruzione di attesa				*/
//gugu	p2val = (P2 & 0x0600);
	isr0val = *regisr0;
	isr1val = *regisr1;
	fcval = *regfc;
	ecval = *regec;
        *regisr0=0;
	*regivr0=0;
	*regisr1=0;
	*regivr1=0;
	*regfc=0;
	*regec=0;
	if ( (p2val != 0x0400) | (isr0val != 0x0580) | (isr1val != 0) | (fcval != 0x3FFC) | ( ecval < 0x1FFE) | (*st_DA_DATA0ptr != 0) | ( *st_DA_DATA1ptr != 0) | (*st_DA_DATA0ptr1 != 0) | (*st_DA_DATA1ptr1 != 0))
	{
		printf("KO!!!!!!    P2=%04x ",p2val);
		printf("ISR0=%04x ISR1=%04x FC=%04x EC=%04x\r\n",isr0val,isr1val,fcval,ecval);
		printf("PCSW0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_PCSptr,*(st_DA_PCSptr+1),*(st_DA_PCSptr+2),*(st_DA_PCSptr+3));
		printf("DATA0W0-W1-W2-W3=%04x-%04x-%04x-%04x ",*st_DA_DATA0ptr,*(st_DA_DATA0ptr+1),*(st_DA_DATA0ptr+2),*(st_DA_DATA0ptr+3));
    		printf("DATA1W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_DATA1ptr,*(st_DA_DATA1ptr+1),*(st_DA_DATA1ptr+2),*(st_DA_DATA1ptr+3));
		printf("PCS1W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_PCSptr1,*(st_DA_PCSptr1+1),*(st_DA_PCSptr1+2),*(st_DA_PCSptr1+3));
		printf("DATA10W0-W1-W2-W3=%04x-%04x-%04x-%04x ",*st_DA_DATA0ptr1,*(st_DA_DATA0ptr1+1),*(st_DA_DATA0ptr1+2),*(st_DA_DATA0ptr1+3));
		printf("DATA11W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_DATA1ptr1,*(st_DA_DATA1ptr1+1),*(st_DA_DATA1ptr1+2),*(st_DA_DATA1ptr1+3));
		er = 1;
	}
	*st_DA_DATA0ptr=0;
	*(st_DA_DATA0ptr+1)=0;
	*(st_DA_DATA0ptr+2)=0;
	*(st_DA_DATA0ptr+3)=0;
	*st_DA_DATA1ptr=0;
	*(st_DA_DATA1ptr+1)=0;
	*(st_DA_DATA1ptr+2)=0;
	*(st_DA_DATA1ptr+3)=0;
	*st_DA_DATA0ptr1=0;
	*(st_DA_DATA0ptr1+1)=0;
	*(st_DA_DATA0ptr1+2)=0;
	*(st_DA_DATA0ptr1+3)=0;
	*st_DA_DATA1ptr1=0;
	*(st_DA_DATA1ptr1+1)=0;
	*(st_DA_DATA1ptr1+2)=0;
	*(st_DA_DATA1ptr1+3)=0;

/*  gugu
	p2val = (P2 & 0x0600);
	if (p2val != 0x0600)
	{
		printf("KO!!!    P2=%04x\r\n",(P2 & 0x0600));
		er = 1;	
	}
*/
	*regdr = 0x0002;                                                /* forza il line switch		*/
	while ( (*regdr & 0x0008) != 0); 						
	*regdr = 0x0001;						/* blocca il line switch		*/
/*	printf ( "DR = %04x\r\n",*regdr);  */

//	P3_2 = 1;                                                      /* chiude relay KMA linea 1 MVB           */
//	P3_3 = 1;                                                      /* chiude relay KMB linea 2 MVB           */
    set_out_port(0, DOP0_KMA|DOP0_KMB, DOP0_KMA|DOP0_KMB);

	*regmfs = 0xF00D;   		                               /* master frame di richiesta stato della stim*/
/*	while ( _getkey() != '\r' );    */
/* test esterno KMA=1 e KMB=1  */
	printf("test esterno linea 2 KMA=1 e KMB=1\r\n");
	for (i = 0; i<20000; i++);					/* istruzione di attesa				*/
	for (off=0; off != 0x1FFE; off++)	/*while ( (c=_getkey())!= '\t')*/
	{
//		i = 0;
//		j = (unsigned long) 600000;
		while (((*regmr & 0x0200) != 0) && ((c=_getkey())!= '\t')){
//			if ( j == (unsigned long) 600000) {
//			       	printf ("\r%04x mr %04x",i,*regmr);
//			       	j = 0;
//				i = ~i;  
//			}
//			j++;
		} 
		if ( c == '\t') break;
		*regmr=0x0020;					/* invia master frame manualmente */
	}
	printf("\r\n");
	for (i = 0; i<20000; i++);					/* istruzione di attesa				*/
//gugu	p2val = (P2 & 0x0600);
	isr0val = *regisr0;
	isr1val = *regisr1;
	fcval = *regfc;
	ecval = *regec;
        *regisr0=0;
	*regivr0=0;
	*regisr1=0;
	*regivr1=0;
	*regfc=0;
	*regec=0;
	if ( (p2val != 0x0400) | (isr0val != 0x0682) | (isr1val != 0) | (fcval != 0x3FFC) | ( ecval > 10) | (*st_DA_DATA0ptr != 0) | ( *st_DA_DATA1ptr != 0) | (*st_DA_DATA0ptr1 != 0xAAAA) | (*st_DA_DATA1ptr1 != 0xAAAA))
	{
//gugu		printf("KO!!!!!!    P2=%04x ",p2val);
		printf("KO!!!!!!    ");
		printf("ISR0=%04x ISR1=%04x FC=%04x EC=%04x\r\n",isr0val,isr1val,fcval,ecval);
		printf("PCSW0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_PCSptr,*(st_DA_PCSptr+1),*(st_DA_PCSptr+2),*(st_DA_PCSptr+3));
		printf("DATA0W0-W1-W2-W3=%04x-%04x-%04x-%04x ",*st_DA_DATA0ptr,*(st_DA_DATA0ptr+1),*(st_DA_DATA0ptr+2),*(st_DA_DATA0ptr+3));
    		printf("DATA1W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_DATA1ptr,*(st_DA_DATA1ptr+1),*(st_DA_DATA1ptr+2),*(st_DA_DATA1ptr+3));
		printf("PCS1W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_PCSptr1,*(st_DA_PCSptr1+1),*(st_DA_PCSptr1+2),*(st_DA_PCSptr1+3));
		printf("DATA10W0-W1-W2-W3=%04x-%04x-%04x-%04x ",*st_DA_DATA0ptr1,*(st_DA_DATA0ptr1+1),*(st_DA_DATA0ptr1+2),*(st_DA_DATA0ptr1+3));
		printf("DATA11W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_DATA1ptr1,*(st_DA_DATA1ptr1+1),*(st_DA_DATA1ptr1+2),*(st_DA_DATA1ptr1+3));
		er = 1;
	}
	*st_DA_DATA0ptr=0;
	*(st_DA_DATA0ptr+1)=0;
	*(st_DA_DATA0ptr+2)=0;
	*(st_DA_DATA0ptr+3)=0;
	*st_DA_DATA1ptr=0;
	*(st_DA_DATA1ptr+1)=0;
	*(st_DA_DATA1ptr+2)=0;
	*(st_DA_DATA1ptr+3)=0;
	*st_DA_DATA0ptr1=0;
	*(st_DA_DATA0ptr1+1)=0;
	*(st_DA_DATA0ptr1+2)=0;
	*(st_DA_DATA0ptr1+3)=0;
	*st_DA_DATA1ptr1=0;
	*(st_DA_DATA1ptr1+1)=0;
	*(st_DA_DATA1ptr1+2)=0;
	*(st_DA_DATA1ptr1+3)=0;

/*gugu
	p2val = (P2 & 0x0600);
	if (p2val != 0x0600)
	{
		printf("KO!!!    P2=%04x\r\n",(P2 & 0x0600));
		er = 1;	
	}
*/

//	P3_2 = 1;                                                      /* chiude relay KMA linea 1 MVB           */
//	P3_3 = 0;                                                      /* apre relay KMB linea 2 MVB           */
   	/* connect to the MVB line */
   	set_out_port(0, DOP0_KMA, DOP0_KMA);
   	set_out_port(0, DOP0_KMB, 0);


/*	while ( _getkey() != '\r' ); */
/* test esterno KMA=0 e KMB=1  */
	printf("test esterno linea 2 KMA=1 e KMB=0\r\n");
	for (i = 0; i<20000; i++);					/* istruzione di attesa				*/
	for (off=0; off != 0x1FFE; off++)	/*while ( (c=_getkey())!= '\t')*/
	{
//		i = 0;
//		j = (unsigned long) 600000;
		while (((*regmr & 0x0200) != 0) && ((c = _getkey())!= '\t')){
//			if ( j == (unsigned long) 600000) {
//			       	printf ("\r%04x mr %04x",i,*regmr);
//			       	j = 0; 
//				i = ~i; 
//			}
//			j++;
		} 
		if ( c == '\t') break;
		*regmr=0x0020;					/* invia master frame manualmente */
	}
	printf("\r\n");
	for (i = 0; i<20000; i++);					/* istruzione di attesa				*/
//gugu    	p2val = (P2 & 0x0600);
	isr0val = *regisr0;
	isr1val = *regisr1;
	fcval = *regfc;
	ecval = *regec;
        *regisr0=0;
	*regivr0=0;
	*regisr1=0;
	*regivr1=0;
	*regfc=0;
	*regec=0;
	if ( (p2val != 0x0400) | (isr0val != 0x0580) | (isr1val != 0) | (fcval != 0x3FFC) | ( ecval < 0x1FFE) | (*st_DA_DATA0ptr != 0) | ( *st_DA_DATA1ptr != 0) | (*st_DA_DATA0ptr1 != 0) | (*st_DA_DATA1ptr1 != 0))
	{
		printf("KO!!!!!!    ");
//		printf("KO!!!!!!    P2=%04x ",p2val);
		printf("ISR0=%04x ISR1=%04x FC=%04x EC=%04x\r\n",isr0val,isr1val,fcval,ecval);
		printf("PCSW0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_PCSptr,*(st_DA_PCSptr+1),*(st_DA_PCSptr+2),*(st_DA_PCSptr+3));
		printf("DATA0W0-W1-W2-W3=%04x-%04x-%04x-%04x ",*st_DA_DATA0ptr,*(st_DA_DATA0ptr+1),*(st_DA_DATA0ptr+2),*(st_DA_DATA0ptr+3));
    		printf("DATA1W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_DATA1ptr,*(st_DA_DATA1ptr+1),*(st_DA_DATA1ptr+2),*(st_DA_DATA1ptr+3));
		printf("PCS1W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_PCSptr1,*(st_DA_PCSptr1+1),*(st_DA_PCSptr1+2),*(st_DA_PCSptr1+3));
		printf("DATA10W0-W1-W2-W3=%04x-%04x-%04x-%04x ",*st_DA_DATA0ptr1,*(st_DA_DATA0ptr1+1),*(st_DA_DATA0ptr1+2),*(st_DA_DATA0ptr1+3));
		printf("DATA11W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_DATA1ptr1,*(st_DA_DATA1ptr1+1),*(st_DA_DATA1ptr1+2),*(st_DA_DATA1ptr1+3));
		er = 1;
	}
	*st_DA_DATA0ptr=0;
	*(st_DA_DATA0ptr+1)=0;
	*(st_DA_DATA0ptr+2)=0;
	*(st_DA_DATA0ptr+3)=0;
	*st_DA_DATA1ptr=0;
	*(st_DA_DATA1ptr+1)=0;
	*(st_DA_DATA1ptr+2)=0;
	*(st_DA_DATA1ptr+3)=0;
	*st_DA_DATA0ptr1=0;
	*(st_DA_DATA0ptr1+1)=0;
	*(st_DA_DATA0ptr1+2)=0;
	*(st_DA_DATA0ptr1+3)=0;
	*st_DA_DATA1ptr1=0;
	*(st_DA_DATA1ptr1+1)=0;
	*(st_DA_DATA1ptr1+2)=0;
	*(st_DA_DATA1ptr1+3)=0;
// gugu
//	p2val = (P2 & 0x0600);
//	if (p2val != 0x0600)
//	{
//		printf("KO!!!    P2=%04x\r\n",(P2 & 0x0600));
//		er = 1;	
//	}

//	P3_2 = 1;                                                      /* chiude relay KMA linea 1 MVB           */
//	P3_3 = 1;                                                      /* chiude relay KMB linea 2 MVB           */
	/* connect to the MVB line */
	set_out_port(0, DOP0_KMA|DOP0_KMB, DOP0_KMA|DOP0_KMB);

	*regmfs = 0xF00D;   		                               /* master frame di richiesta stato della stim*/
/*	while ( _getkey() != '\r' );    */
/* test FEV interrupt  */
	printf("test FEV interrupt\r\n");
	for (i = 0; i<20000; i++);					/* istruzione di attesa				*/
	for (off=0; off != 0x8000; off++)	/*while ( (c=_getkey())!= '\t')*/
	{
//		i = 0;
//		j = (unsigned long) 600000;
		while (((*regmr & 0x0200) != 0) && ((c = _getkey())!= '\t')){
//			if ( j == (unsigned long) 600000) {
//			      	printf ("\r%04x mr %04x",i,*regmr);
//			       	j = 0;  
//				i = ~i;
//			}
//			j++;
		} 
		if ( c == '\t') break;
		*regmr=0x0020;					/* invia master frame manualmente */
	}
	printf("\r\n");
	for (i = 0; i<20000; i++);					/* istruzione di attesa				*/
//gugu	p2val = (P2 & 0x0600);
	isr0val = *regisr0;
	isr1val = *regisr1;
	fcval = *regfc;
	ecval = *regec;
        *regisr0=0;
	*regivr0=0;
	*regisr1=0;
	*regivr1=0;
	*regfc=0;
	*regec=0;
	if ( (p2val != 0x0000) | (isr0val != 0x0682) | (isr1val != 0x0080) | (fcval != 0xFFFF))
	{
//gugu		printf("KO!!!!!!    P2=%04x ",p2val);
//		printf("KO!!!!!!   ");
		printf("ISR0=%04x ISR1=%04x FC=%04x EC=%04x\r\n",isr0val,isr1val,fcval,ecval);
		printf("PCSW0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_PCSptr,*(st_DA_PCSptr+1),*(st_DA_PCSptr+2),*(st_DA_PCSptr+3));
		printf("DATA0W0-W1-W2-W3=%04x-%04x-%04x-%04x ",*st_DA_DATA0ptr,*(st_DA_DATA0ptr+1),*(st_DA_DATA0ptr+2),*(st_DA_DATA0ptr+3));
    		printf("DATA1W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_DATA1ptr,*(st_DA_DATA1ptr+1),*(st_DA_DATA1ptr+2),*(st_DA_DATA1ptr+3));
		printf("PCS1W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_PCSptr1,*(st_DA_PCSptr1+1),*(st_DA_PCSptr1+2),*(st_DA_PCSptr1+3));
		printf("DATA10W0-W1-W2-W3=%04x-%04x-%04x-%04x ",*st_DA_DATA0ptr1,*(st_DA_DATA0ptr1+1),*(st_DA_DATA0ptr1+2),*(st_DA_DATA0ptr1+3));
		printf("DATA11W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_DATA1ptr1,*(st_DA_DATA1ptr1+1),*(st_DA_DATA1ptr1+2),*(st_DA_DATA1ptr1+3));
		er = 1;
	}

	return er;
}








/**********************************************************/
/* Funzione di test  del  mvb bus                         */
/**********************************************************/
unsigned char test_SW1 (void)
{   
    static unsigned char error=0;
    static unsigned char exit=0;
    static unsigned short data1=0;
    static unsigned char sts_testsw1=0;
    static unsigned char old_sts_testsw1=0;
    char c;
    
    unsigned short i,vp;
    
    printf("TEST SW1\n");
    exit = 0;

	*regscr1 = 0x0000; 						/* reset software				*/
	*regscr = 0x0000;
	for (i = 0; i<10; i++);								/* istruzione di attesa				*/
	*regscr = 0x8001;						/* predispone per l'inizializzazione dei reg.	*/
	*regmcr = funmode;						/* setta size della TM				*/
	for (i = 0; i<10; i++);								/* istruzione di attesa				*/

	*regdaor1 = 0x000D;
	data1 = *regdaor1;						/* legge il Device Address			*/ 
	
	*regdr = 0x0000;						/* consente lo switch tra le linee		*/
	printf ( "DR = %04x DA=%04x\r\n",*regdr,data1);
	
	*regdaok = 0x0094;						/* sovrascrive indirizzo hardware               */
	
	data1 = *regdaor1;						/* legge il Device Address			*/ 
	printf ( "DA=%04x\r\n",data1);
	*regimr0 = 0xFFFF;						/* abilita tutte le interrupt			*/
	*regimr1 = 0xFFFF;
	*regivr0 = 0;
	*regivr1 = 0;

   	set_out_port(0, DOP0_KMA|DOP0_KMB, DOP0_KMA|DOP0_KMB);

	*regscr1 = 0x8503;						/* abilito in full mode l'mvb	*/


    while (!exit){
        if ((c=_getkey())  == '\t') exit= 1;
  		
            
        switch(sts_testsw1)
        {
            case 0://premi invio verificare 1
                printf("step 1: Posizionare SW1 su 1 e premere INVIO\n");
                while ((c=_getkey())  != '\r')
                {
                    data1 = *regdaor & 0x000F;  // legge il DA per l'indirizzamento hw
                    printf("daor %4X\r",*regdaor);    
                }
        	    if (data1 == 1){
        	        sts_testsw1 = 1;
        	        printf("OK\n");
        	    }
        	    else{
        	        old_sts_testsw1 = sts_testsw1;
        	        sts_testsw1 = 10;// errore
        	    }
                break;
            case 1://premi invio verificare 2
                printf("step 2: Posizionare SW1 su 2 e premere INVIO ");
                while (((c=_getkey())  != '\r')&&((c=_getkey())  != 'x'));
                if (c=='x') 
                    sts_testsw1 = 5;
                data1 = *regdaor & 0x000F;  // legge il DA per l'indirizzamento hw
        	    if (data1 == 2){
        	        sts_testsw1 = 2;
        	        printf("OK\n");
        	    }
        	    else{
        	        old_sts_testsw1 = sts_testsw1;
        	        sts_testsw1 = 10;// errore
        	    }
                break;
            case 2://premi invio verificare 4
                printf("step 3: Posizionare SW1 su 4 e premere INVIO ");
                while (((c=_getkey())  != '\r')&&((c=_getkey())  != 'x'));
                if (c=='x') 
                    sts_testsw1 = 5;
                data1 = *regdaor & 0x000F;  // legge il DA per l'indirizzamento hw
        	    if (data1 == 4){
        	        sts_testsw1 = 3;
        	        printf("OK\n");
        	    }
        	    else{
        	        old_sts_testsw1 = sts_testsw1;
        	        sts_testsw1 = 10;// errore
        	    }
                break;
            case 3://premi invio verificare 8
                printf("step 4: Posizionare SW1 su 8 e premere INVIO ");
                while (((c=_getkey())  != '\r')&&((c=_getkey())  != 'x'));
                if (c=='x') 
                    sts_testsw1 = 5;
                data1 = *regdaor & 0x000F;  // legge il DA per l'indirizzamento hw
        	    if (data1 == 8){
        	        sts_testsw1 = 4;
        	        printf("OK\n");
        	    }
        	    else{
        	        old_sts_testsw1 = sts_testsw1;
        	        sts_testsw1 = 10;// errore
        	    }
                break;
            case 4://premi invio verificare 0
                printf("step 5: Posizionare SW1 su 0 e premere INVIO ");
                while (((c=_getkey())  != '\r')&&((c=_getkey())  != 'x'));
                if (c=='x') 
                    sts_testsw1 = 5;
                data1 = *regdaor & 0x000F;  // legge il DA per l'indirizzamento hw
        	    if (data1 == 0){
        	        sts_testsw1 = 5;
        	        printf("OK\n");
        	    }
        	    else{
        	        old_sts_testsw1 = sts_testsw1;
        	        sts_testsw1 = 10;// errore
        	    }
                break;
            case 5://test terminato
                    exit =1;
                    printf("EXIT\n");
                    sts_testsw1=0;
                break;
            case 10://stampa errore
                printf("ERROR riletto %04X\n",data1);
                sts_testsw1 = old_sts_testsw1;//ritorna al test precedente
                error = old_sts_testsw1;
                break;
            default:
             break;
        }    
        
    }


    return error;
}
/**********************************************************/
/* Funzione di test  del  mvb bus                         */
/**********************************************************/
void test_mvb (void)
{
	unsigned int data1,i,er;
	char c;

	printf("********** TEST MVBC *************\r\n");

	printf("Test del selettore d'indirizzo\r\n");
	*regmcr1 = *regmcr1 & 0xFFF8;// set MCM=000 
	*regscr = *regscr | 0x8001;  // set IL=01  configuration mode
	er = 0;
	data1 = *regdaor & 0x000F;  // legge il DA per l'indirizzamento hw
	if (data1 != 1){
	     save_stato(TMVBSELKO);
	     printf("\n !! selezionare SW1 in posizione 1  %d\n",data1);
	}
	else 
    {
	     save_stato(TMVBSELOK);
    }

	printf("Test Service Area in Traffic Memory (C0FC00H..C0FFFEH)\r\n");

	if (shf0(sasaddmvb,saeaddmvb)) save_stato(TMVBSASHF0KO);
	else save_stato(TMVBSASHF0OK);	

	if (shf1(sasaddmvb,saeaddmvb)) save_stato(TMVBSASHF1KO);
	else save_stato(TMVBSASHF1OK);	

/*
	if (march(sasaddmvb,saeaddmvb)) save_stato(TMVBSAMARCHKO);
	else save_stato(TMVBSAMARCHOK);	

	if (maxrum(sasaddmvb,saeaddmvb)) save_stato(TMVBSAMXRUMKO);
	else save_stato(TMVBSAMXRUMOK);	
*/

	printf("Test della Traffic Memory\r\n");
	*regmcr = *regmcr | funmode;

	if (shf0_mvb(saddmvb,eaddmvb)) save_stato(TMVBTMSHF0KO);
	else save_stato(TMVBTMSHF0OK);	

	if (shf1_mvb(saddmvb,eaddmvb)) save_stato(TMVBTMSHF1KO);
	else save_stato(TMVBTMSHF1OK);	
/*
	if (march_mvb(saddmvb,eaddmvb)) save_stato(TMVBTMMARCHKO);
	else save_stato(TMVBTMMARCHOK);	

	if (maxrum_mvb(saddmvb,eaddmvb)) save_stato(TMVBTMMXRUMKO);
	else save_stato(TMVBTMMXRUMOK);	

*/
        
	if (!(er = t_mvbel())) {
			printf("Scambiare i connettori dell'MVB BUS e premere invio per proseguire\r\n");
			while ( _getkey() != '\r' );
			er = t_mvbel();
	}

	if (er) save_stato(TMVBBUSKO);
	else save_stato(TMVBBUSOK);	

    printf("TEST MVB terminato");
}

