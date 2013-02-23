/*********************************************************************************
* Lun.08.set.03           v.1.00        \sw_st\sw_stb\tststbxx\source\tstflash.c *
**********************************************************************************
* Funzioni per il test della flash memory                                        *
**********************************************************************************
* 					                                         *
*********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <shd_var.h>
#include <cod_st.h>
#pragma ot(3,SPEED)

/* la flash viene vista come un array di interi */
#define flash1 ((unsigned int xhuge *)(0x80AAAA))
#define flash2 ((unsigned int xhuge *)(0x805554))

void save_stato (unsigned int stato);
void prn_stato (unsigned int prg, unsigned int cod);
void prn_report(void);
//void (*f_gest)(void);

 
/**********************************************************/
/* Funzione di erase flash                                */
/* Valore ritornato:   codice errore                      */
/**********************************************************/
int chip_erase(void)
{
	unsigned int data3,data1,i,a,tim1,tim2,ok1,ok2;
	int er;

	er = 0;
	printf("ERASE FLASH\r\n");
	/***    procedura di start erase         ***/
	*flash1 = 0xAAAA;
	*flash2 = 0x5555;
	*flash1 = 0x8080;
	*flash1 = 0xAAAA;
	*flash2 = 0x5555;
	*flash1 = 0x1010;
	/***    verifica del fine erase chip     ***/
	data3 = *flash1;
	i = 0;
	a = 0;
	ok1 = 0;
	ok2 = 0;
	tim1 = 0;
	tim2 = 0;
	while (!(ok1 && ok2)) {
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
		data3 = *flash1;
		if((i==0) || (i==32000)) {
			printf("%05d data=%04x\r",a,data3);
			a++;
		}
		i++;
                if ( _getkey()  == '\t' ) 
	        {
			er = 3;
			break;
			
		}				
	}
	printf("%05d data=%04x\r\n",a,data3);
	data3 = *flash1;
	return er;
}


/**********************************************************/
/* Funzione di copy ram to flash                          */
/* Variabili di ingresso:                                 */
/* Valore ritornato:   codice errore                      */
/**********************************************************/
int wr (void)
{
	unsigned int dat,data1,i,dt;
	int  er;
	unsigned int xhuge * pt_flash;
	unsigned int xhuge * pt_ram;

	er = 0;
	i=0;
	pt_ram = (unsigned int xhuge *)(0x000000);
	/*** ciclo di srittura e verifica su tutti gli indirizzi ***/
	for ( pt_flash = sctstr; pt_flash <=(unsigned int xhuge *)0x8FFFFE; pt_flash++){
	/***    procedura di start write     ***/
		dt = *pt_ram;
		*flash1 = 0xAAAA;
		*flash2 = 0x5555;
		*flash1 = 0xA0A0;
		/***   scrittura della flash         ***/
		*pt_flash = dt;
		/***  attesa fine scrittura           ***/
		dat = *pt_flash;
		data1 = *pt_flash;
		while (dat != data1) {
			dat = *pt_flash;
			data1 = *pt_flash;
       		        if ( _getkey()  == '\t' ) 
		        {
			   er = 4;	
			   break;
		        }				

		}

		i++;
		if(i==10000){
			printf(".");
			i = 0;
		}
		if ( dat != dt ) {
			er = 5;
			break;
		}
		if (pt_ram == (unsigned int xhuge *)(0x00EFFE))
		{
			pt_ram = (unsigned int xhuge *)(0x00FFFE);
			pt_flash = (unsigned int xhuge *)(0x80FFFE);
		}
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
int chip_erasesect(unsigned int xhuge * addr)
{
	unsigned int data3,data1,i,a,tim1,tim2,ok1,ok2,seg,off;
	int er;
	unsigned long app;	

	er = 0;
	app = (unsigned long) addr ;
	off = (unsigned int)(app & 0x00FFFF);
	app = app >> 16;
	seg = (unsigned int)(app & 0x0000FF);
	printf("ERASE SECTOR %02x%04x\r\n",seg,off);
	/***    procedura di start erase sector         ***/
	*flash1 = 0xAAAA;
	*flash2 = 0x5555;
	*flash1 = 0x8080;
	*flash1 = 0xAAAA;
	*flash2 = 0x5555;
	*addr   = 0x3030;
	/***    verifica del fine erase chip     ***/
	data3 = *addr;
	i = 0;
	a = 0;
	ok1 = 0;
	ok2 = 0;
	tim1 = 0;
	tim2 = 0;
	while (!(ok1 && ok2)) {
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
			printf("%05d data=%04x\r",a,data3);
			a++;
		}
		i++;
                if ( _getkey()  == '\t' ) 
	        {
			er = 8;
			break;
		}				
	}
	printf("%05d data=%04x\r\n",a,data3);
	data3 = *addr;
	return er;
}


/**********************************************************/
/* Funzione di write & verify flash                       */
/* Variabili di ingresso:                                 */
/*   dt           dato da scrivere nella flash            */
/* Valore ritornato:   codice errore                      */
/**********************************************************/
int wr_vrf(void)
{
	unsigned int data3,data1,ok1,ok2,tim1,tim2,i,data2,j,k,dt;
	int  er;
	unsigned int xhuge * pt_flash;
	unsigned int  off,seg;
	unsigned long app; 
	char c;

	printf("WRITE/VERIFY FLASH\r\n");
	er = 0;
	i=60000;
	/*** ciclo di scrittura e verifica su tutti gli indirizzi ***/
	for ( pt_flash = sctstr; pt_flash <= flashlimit; pt_flash++)
	{
	/***    procedura di start write     ***/
	     j = 0;
	     data2 = 0xFFFE;		
	     for (dt = 0x0001; dt != 0x0000; dt = dt << 1)
	     {		
		k = 10000;
		tim1 = 0;
		while( (((data3 = *pt_flash)  & dt) == 0) && ((c=_getkey())  != '\r') ) {
		     if ( k > 10000) 
		     {	   
			app = (unsigned long) pt_flash;
			off = (unsigned int)(app & 0x00FFFF);
			app = app >> 16;
			seg = (unsigned int)(app & 0x0000FF);
			printf("\rKO!!! Ad Address %02x%04x Bit %02d atteso 1 letto 0",seg,off, j);
			k = 0;
			tim1 = 1;
			er = 10;
		     }
		     k++;
		     if ( c == '\t' ) 
		     {
			er = 9;
			break;
		     }				
		}
		if ( tim1 ) printf("\r\n");
		if ( er == 9 ) break;	
		*flash1 = 0xAAAA;
		*flash2 = 0x5555;
		*flash1 = 0xA0A0;
		/***   scrittura della flash         ***/
		*pt_flash = data2;
		/***  attesa fine scrittura           ***/
		data3 = *pt_flash;
		ok1 = 0;     
		ok2 = 0;
		tim1 = 0;
		tim2 = 0;
		while (!(ok1 && ok2)) {
			if (!(ok1 || tim1)) {
				if ((data3 & 0x8000) == (data2 & 0x8000)){
					ok1 = 1;
				}
				else {
					if ((data3 & 0x2000) == 0x2000){
						data1 = *pt_flash;
						if ((data1 & 0x8000) != (data2 & 0x8000)){
							er = 11;
							tim1 = 1;
						}
						else ok1 = 1;
					}
				}
			}    
			if (!(ok2 || tim2)) {
				if ((data3 & 0x0080) == (data2 & 0x0080)){
					ok2 = 1;
				}
				else {
					if ((data3 & 0x0020) == 0x0020){
						data1 = *pt_flash;
						if ((data1 & 0x0080) != (data2 & 0x0080)){
							er = 12;
							tim2 = 1;
						}	
						else ok2 = 1;
					}
				}
			}
			if (tim1 || tim2) break;
			data3 = *pt_flash;
		        if ( _getkey()  == '\t' ) 
		        {
			   er = 9;	
			   break;
		        }				
		}
		if ( er == 9 ) break;
		i++;
		if (i==60000){
			printf(".");
			i = 0;
		}
		k = 10000;
		tim1 = 0;
		while( (((data3 = *pt_flash)  & dt) != 0) && ((c = _getkey())  != '\r') ) {    
		     if ( k > 10000) 
		     {	   
			app = (unsigned long) pt_flash;
			off = (unsigned int)(app & 0x00FFFF);
			app = app >> 16;
			seg = (unsigned int)(app & 0x0000FF);
			printf("\rKO!!! Ad Address %02x%04x Bit %02d atteso 0 letto 1",seg,off, j);
			k = 0;
			tim1 = 1;
			er = 13;
		     }
		     k++;		
		     if ( c == '\t' )
		     {
			er = 9;
			break;
		     }				
		}
		if ( tim1 ) printf("\r\n");
		if ( er == 9 ) break;	
		j++;
		data2 = data2 << 1;
	     }
             if ( er == 9 ) break;
	}
	printf("\r\n");
	return er;
}

/**********************************************************/
/* Programma test della flash                             */
/**********************************************************/
void tstflash (void )
{
	int error,i;
	unsigned int k,j,data3,seg,off,exit;
	unsigned int xhuge * sectaddr;
	unsigned int xhuge * sectstart;
	unsigned int xhuge * sectend;
	unsigned long app;
	char c;

/*
	unsigned int xhuge * pt_flash;
f_gest=0x400000;
	f_gest();	
        pt_flash = sctstr;
        c=0;
        while(1) {
	i = *pt_flash;
	printf("letto %04x",i);
	while((c=_getkey())!= '\r');
	if ( c == '\t') break;
	pt_flash++;
	}
*/

	i=i;
	exit = 0;
	printf("TEST FLASH ");
	error = chip_erase();
	for(i=0;i<1000;i++);
	if (!error ) {
		error = wr_vrf();
		for(i=0;i<1000;i++);
		if (!error) {	
			for ( sectstart = sctstr; sectstart <= sectlimit; sectstart += sctdelta){
				if (!(error = chip_erasesect(sectstart)))
				{
					sectend = sectstart + dimsct;
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
							error = 9;
							exit = 1;
							break;
		     				     }				
						     k++;		
						}
						if ( j ) printf("\r\n");
						if ( exit ) break;	
					}	
				}
				else break;
				if ( exit ) break;	
			}
	 	} 
	} 
	switch (error){
		case 0:
			save_stato (TFLAWRVROK);
			ram2flash ();
			break;
		case 1:
			prn_stato (0,TFLATOTERSCH1KO);
			break;
		case 2:
			prn_stato (0,TFLATOTERSCH2KO);
			break;
		case 3:
			prn_stato (0,TFLAFZEXITTOTERS);
			break;
		case 6:
			prn_stato (0,TFLASCTERSCH1KO);
			break;
		case 7:
			prn_stato (0,TFLASCTERSCH2KO);
			break;
		case 8:
			prn_stato (0,TFLAFZEXITSCTERS);
			break;
		case 9:
			prn_stato (0,TFLAFZEXITWRVR);
			break;
		case 10:
			prn_stato (0,TFLAVR1KO);
			break;
		case 11:
			prn_stato (0,TFLAPRGCH1KO);
			break;
		case 12:
			prn_stato (0,TFLAPRGCH2KO);
			break;
		case 13:
			prn_stato (0,TFLAVR0KO);
			break;
	}
	printf("\r\n");
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
		case TFLAFZEXITWRVR:
			printf("%04d  %04x  Test Flash forzata uscita durante write-verify..............   KO  ",prg,cod);
			break;
		case TFLAVR1KO:
			printf("%04d  %04x  Test Flash verifed 1........................................   KO  ",prg,cod);
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

void save_stato (unsigned int stato)
{
	unsigned int i,dt,dat,data1;
	unsigned int xhuge * pt_flash;
		
	i=0;
	pt_flash = (unsigned int xhuge *)(0x80F000);
	while ( ((dt = *pt_flash) != 0xFFFF) && (pt_flash <= (unsigned int xhuge *)(0x80FFFE))){
		pt_flash++;
		i++;
	}
	/***    procedura di start write     ***/
	*flash1 = 0xAAAA;
	*flash2 = 0x5555;
	*flash1 = 0xA0A0;
	/***   scrittura della flash         ***/
	*pt_flash = stato;
	/***  attesa fine scrittura           ***/
	dat = *pt_flash;
	data1 = *pt_flash;
	while (dat != data1) {
		dat = *pt_flash;
		data1 = *pt_flash;
	}
	prn_stato(i,stato);
}

void prn_report(void)
{
	unsigned int i,dt,dat,data1;
	unsigned int xhuge * pt_flash;
		
	printf("PROG.  COD.                    DESCRIZIONE DEL TEST                       ESITO\r\n");
	i=0;
	pt_flash = (unsigned int xhuge *)(0x80F000);
	while ( ((dt = *pt_flash) != 0xFFFF) && (pt_flash <= (unsigned int xhuge *)(0x80FFFE))){
		prn_stato(i,dt);
		pt_flash++;
		i++;
	}
}


void w_cmd(void)
{
		
	if (!chip_erase() ) ram2flash ();
}
