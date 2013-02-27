/******************************************************************************************
* Mer.02.mar.05                 V.1.00         sw_dew\dew01\dewcamp\source\dewcamp.c      *
*******************************************************************************************
* Programma principale per applicativo test DEW  scheda campione                          *
*******************************************************************************************
*                                                                                         *
******************************************************************************************/

#include "stdio.h"
#include "serbuf.h"
#include "consolle.h"
#include "handshk.h"
#include "drvuart.h"
/*#include "tstbcom.h"*/
#include <reg167.h>
#include <bitfield.h>
#pragma ot(3,SPEED)

/************** definizioni registri MVB ****************************/
unsigned int xhuge * regscr  = (unsigned int xhuge *)0x203F80;     /* indirizzo del registro SCR dell'MVB     */ 
unsigned int xhuge * regscr1 = (unsigned int xhuge *)0x20FF80;     /* indirizzo del registro SCR dell'MVB nel mode 'funmode'     */ 
unsigned int xhuge * regmcr  = (unsigned int xhuge *)0x203F84;     /* indirizzo del registro MCR dell'MVB     */ 
unsigned int xhuge * regmcr1 = (unsigned int xhuge *)0x20FF84;     /* indirizzo del registro MCR dell'MVB nel mode 'funmode'     */ 
unsigned int xhuge * regdr   = (unsigned int xhuge *)0x20FF88;     /* indirizzo del registro DR dell'MVB     */ 
unsigned int xhuge * regstsr = (unsigned int xhuge *)0x20FF8C;     /* indirizzo del registro STSR dell'MVB    */ 
unsigned int xhuge * regfc   = (unsigned int xhuge *)0x20FF90;     /* indirizzo del registro FC dell'MVB      */ 
unsigned int xhuge * regec   = (unsigned int xhuge *)0x20FF94;     /* indirizzo del registro EC dell'MVB      */ 
unsigned int xhuge * regmfr  = (unsigned int xhuge *)0x20FF98;     /* indirizzo del registro MFR dell'MVB     */ 
unsigned int xhuge * regmfre = (unsigned int xhuge *)0x20FF9C;     /* indirizzo del registro MFRE dell'MVB    */ 
unsigned int xhuge * regmr   = (unsigned int xhuge *)0x20FFA0;     /* indirizzo del registro MR dell'MVB      */ 
unsigned int xhuge * regmr2  = (unsigned int xhuge *)0x20FFA4;     /* indirizzo del registro MR2 dell'MVB     */ 
unsigned int xhuge * regdpr1  = (unsigned int xhuge *)0x20FFA8;     /* indirizzo del registro DPR dell'MVB     */ 
unsigned int xhuge * regdpr2 = (unsigned int xhuge *)0x20FFAC;     /* indirizzo del registro DPR2 dell'MVB    */ 
unsigned int xhuge * regipr0 = (unsigned int xhuge *)0x20FFB0;     /* indirizzo del registro IPR0 dell'MVB    */ 
unsigned int xhuge * regipr1 = (unsigned int xhuge *)0x20FFB4;     /* indirizzo del registro IPR1 dell'MVB    */ 
unsigned int xhuge * regimr0 = (unsigned int xhuge *)0x20FFB8;     /* indirizzo del registro IMR0 dell'MVB    */ 
unsigned int xhuge * regimr1 = (unsigned int xhuge *)0x20FFBC;     /* indirizzo del registro IMR1 dell'MVB    */ 
unsigned int xhuge * regisr0 = (unsigned int xhuge *)0x20FFC0;     /* indirizzo del registro ISR0 dell'MVB    */ 
unsigned int xhuge * regisr1 = (unsigned int xhuge *)0x20FFC4;     /* indirizzo del registro ISR1 dell'MVB    */ 
unsigned int xhuge * regivr0 = (unsigned int xhuge *)0x20FFC8;     /* indirizzo del registro IVR0 dell'MVB    */ 
unsigned int xhuge * regivr1 = (unsigned int xhuge *)0x20FFCC;     /* indirizzo del registro IVR1 dell'MVB    */ 
unsigned int xhuge * regdaor = (unsigned int xhuge *)0x203FD8;     /* indirizzo del registro DAOR dell'MVB     */ 
unsigned int xhuge * regdaor1= (unsigned int xhuge *)0x20FFD8;     /* indirizzo del registro DAOR dell'MVB nel mode 'funmode'     */ 
unsigned int xhuge * regdaok = (unsigned int xhuge *)0x20FFDC;     /* indirizzo del registro DAOK dell'MVB    */ 
unsigned int xhuge * regtcr  = (unsigned int xhuge *)0x20FFE0;     /* indirizzo del registro TCR dell'MVB     */ 
unsigned int xhuge * regtr1  = (unsigned int xhuge *)0x20FFF0;     /* indirizzo del registro TR1 dell'MVB     */ 
unsigned int xhuge * regtr2  = (unsigned int xhuge *)0x20FFF4;     /* indirizzo del registro TR2 dell'MVB     */ 
unsigned int xhuge * regtc1  = (unsigned int xhuge *)0x20FFF8;     /* indirizzo del registro TC1 dell'MVB     */ 
unsigned int xhuge * regtc2  = (unsigned int xhuge *)0x20FFFC;     /* indirizzo del registro TC2 dell'MVB     */ 
unsigned int xhuge * regmfs  = (unsigned int xhuge *)0x20FF00;     /* indirizzo del registro MFS dell'MVB     */ 
unsigned long st_DA_PIT       = (unsigned long) 0x202000;            /* indirizzo di partenza dei DA_PIT        */ 
unsigned long st_DA_PCS       = (unsigned long) 0x238000;            /* indirizzo di partenza dei DA_PCS        */ 
unsigned long st_DA_DATA      = (unsigned long) 0x240000;            /* indirizzo di partenza dei DA_DATA       */ 
unsigned long end_DA_PIT      = (unsigned long) 0x203FFE;            /* indirizzo di fine della DA_PIT          */ 
unsigned long end_DA_PCS      = (unsigned long) 0x23FFFE;            /* indirizzo di fine dei DA_PCS            */ 
unsigned long st_LA_PIT       = (unsigned long) 0x200000;            /* indirizzo di partenza dei LA_PIT        */ 
unsigned long st_LA_PCS       = (unsigned long) 0x230000;            /* indirizzo di partenza dei LA_PCS        */ 
unsigned long end_LA_PIT      = (unsigned long) 0x201FFE;            /* indirizzo di fine della LA_PIT          */ 
unsigned long end_LA_PCS      = (unsigned long) 0x237FFE;            /* indirizzo di fine dei LA_PCS            */ 
unsigned long st_FC15_PCS     = (unsigned long) 0x20FE38;            /* indirizzo di partenza dei FC15_PCS      */ 
unsigned long st_FC15_DATA0   = (unsigned long) 0x20FC58;            /* indirizzo FC15_DATA in pg0              */ 
unsigned long st_FC15_DATA1   = (unsigned long) 0x20FC78;            /* indirizzo FC15_DATA in pg1              */ 
unsigned int funmode = 4;   					   /* Mode che determina la dimensione della TM dell'MVBC (4=1MB)*/     
unsigned int xhuge * st_DA_PITptr;
unsigned int xhuge * st_DA_PCSptr;
unsigned int xhuge * st_DA_DATA0ptr;
unsigned int xhuge * st_DA_DATA1ptr;
unsigned int xhuge * st_DA_PCSptr1;
unsigned int xhuge * st_DA_DATA0ptr1;
unsigned int xhuge * st_DA_DATA1ptr1;
unsigned int xhuge * FC15_PCSptr;
unsigned int xhuge * FC15_DATAptr;

/* la flash viene vista come un array di interi */
#define flash1 ((unsigned int xhuge *)(0x80AAAA))
#define flash2 ((unsigned int xhuge *)(0x805554))



unsigned char combuff[LUNGHEZZA_CODA];
unsigned short lenvar; 



void write_port_out1(unsigned char outx)
{
	P7 &= 0xFF00;
	P7 |= ((unsigned int)outx & 0x00FF);
}

void write_port_out2(unsigned char outx)
{
	unsigned int app;
	
	app = (((unsigned int)outx) << 3);
	P2 &= 0xFF87;
	P2 |= (app & 0x0078);  
	app = (((unsigned int)outx) << 5);
	P3 &= 0xFDFF;
	P3 |= (app & 0x0200);
	app = (((unsigned int)outx) << 8);
	P3 &= 0xDFFF;
	P3 |= (app & 0x2000);
	app = (((unsigned int)outx) >> 5);
	P8 &= 0xFFFD;
	P8 |= (app & 0x0002);
	app = ((unsigned int)outx) ;
	P6 &= 0xFF7F;
	P6 |= (app & 0x0080);
}

void write_port_out3(unsigned char outx)
{
	unsigned char app,app1;
	
	app = ((outx << 1) & 0x02);
	app1 = (UART_A[MCR] & 0xFD);
        app1 |= app;
	UART_A[MCR] = app1;
	app = (outx & 0x02);
	app1 = (UART_B[MCR] & 0xFD);
        app1 |= app;
	UART_B[MCR] = app1;
	app = ((outx >> 1) & 0x02);
	app1 = (UART_C[MCR] & 0xFD);
        app1 |= app;
	UART_C[MCR] = app1;
	app = ((outx >> 2) & 0x02);
	app1 = (UART_D[MCR] & 0xFD);
        app1 |= app;
	UART_D[MCR] = app1;
	app = ((outx >> 4) & 0x01);
	app1 = (UART_A[MCR] & 0xFE);
        app1 |= app;
	UART_A[MCR] = app1;
	app = ((outx >> 5) & 0x01);
	app1 = (UART_B[MCR] & 0xFE);
        app1 |= app;
	UART_B[MCR] = app1;
	app = ((outx >> 6) & 0x01);
	app1 = (UART_C[MCR] & 0xFE);
        app1 |= app;
	UART_C[MCR] = app1;
	app = ((outx >> 7) & 0x01);
	app1 = (UART_D[MCR] & 0xFE);
        app1 |= app;
	UART_D[MCR] = app1;

}

void write_port_out4(unsigned char outx)
{
	unsigned int app;
	
	app = (((unsigned int)outx) << 6);
	P6 &= 0xFFBF;
	P6 |= (app & 0x0040);  
	app = (((unsigned int)outx) >> 1);
	P3 &= 0xFFFC;
	P3 |= (app & 0x0003);
	app = (((unsigned int)outx) << 3);
	P3 &= 0xFF3F;
	P3 |= (app & 0x00C0);
}

unsigned char read_port_in1(void)
{
	unsigned int app;
	unsigned char in;
	
	in = 0;
	app = (P2 & 0x0003);
	in |= (unsigned char) app; 
	app = (P8 & 0x00FC);
	in |= (unsigned char) app;
	return in; 
}

unsigned char read_port_in2(void)
{
	unsigned int app;
	unsigned char in,app1;

	in = 0;
	app = (P2 & 0x0004);
	app = app >> 2;
	in |= (unsigned char) app; 
	app = (P3 & 0x0030);
	app = app >> 3;
	in |= (unsigned char) app;
	app = (P3 & 0x0100);
	app = app >> 5;
	in |= (unsigned char) app;
	app1 = ((~UART_A[MSR]) & 0x40);
	app1 = app1 >> 2;
	in |= app1;
	app1 = ((~UART_B[MSR]) & 0x40);
	app1 = app1 >> 1;
	in |= app1;
	app1 = ((~UART_C[MSR]) & 0x40);
	in |= app1;
	app1 = ((~UART_D[MSR]) & 0x40);
	app1 = app1 << 1;
	in |= app1;
	return in; 
}

unsigned char read_port_in3(void)
{
	unsigned char in,app;
	
	in = 0;
	app = (UART_A[MSR] & 0x10);
	app = app >> 4;
	in |= app;
	app = (UART_B[MSR] & 0x10);
	app = app >> 3;
	in |= app;
	app = (UART_C[MSR] & 0x10);
	app = app >> 2;
	in |= app;
	app = (UART_D[MSR] & 0x10);
	app = app >> 1;
	in |= app;
	app = (UART_A[MSR] & 0x20);
	app = app >> 1;
	in |= app;
	app = (UART_B[MSR] & 0x20);
	in |= app;
	app = (UART_C[MSR] & 0x20);
	app = app << 1;
	in |= app;
	app = (UART_D[MSR] & 0x20);
	app = app << 2;
	in |= app;
	return in;
}

unsigned char read_port_in4(void)
{
	unsigned int app;
	unsigned char in,app1;

	in = 0;
	app = (P8 & 0x0001);
	in |= (unsigned char) app; 
	app1 = ((~UART_A[MSR]) & 0x80);
	app1 = app1 >> 6;
	in |= app1;
	app1 = ((~UART_B[MSR]) & 0x80);
	app1 = app1 >> 5;
	in |= app1;
	app1 = ((~UART_C[MSR]) & 0x80);
	app1 = app1 >> 4;
	in |= app1;
	app1 = ((~UART_D[MSR]) & 0x80);
	app1 = app1 >> 3;
	in |= app1;
	return in; 
}


/**********************************************************/
/* Funzione di erase flash                                */
/* Valore ritornato:   codice errore                      */
/**********************************************************/
int chip_erase(void)
{
	unsigned int dat,data1,i,a,tim1,tim2,ok1,ok2;
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
	dat = *flash1;
	i = 0;
	a = 0;
	ok1 = 0;
	ok2 = 0;
	tim1 = 0;
	tim2 = 0;
	while (!(ok1 && ok2)) {
		if (!(ok1 || tim1)) {
			if ((dat & 0x8000) == 0x8000){
				ok1 = 1;
			}
			else {
				if ((dat & 0x2000) == 0x2000){
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
			if ((dat & 0x0080) == 0x0080){
				ok2 = 1;
			}
			else {
				if ((dat & 0x0020) == 0x0020){
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
		dat = *flash1;
		if((i==0) || (i==32000)) {
			printf("%05d data=%04x\r",a,dat);
			a++;
		}
		i++;
	}
	printf("%05d data=%04x\r\n",a,dat);
	dat = *flash1;
	return er;
}


/**********************************************************/
/* Funzione di copy ram to flash                          */
/* Variabili di ingresso:                                 */
/* Valore ritornato:   codice errore                      */
/**********************************************************/
int wr_vrf(void)
{
	unsigned int dat,data1,i,dt;
	int  er;
	unsigned int xhuge * pt_flash;
	unsigned int xhuge * pt_ram;

	printf("WRITE FLASH\r\n");
	er = 0;
	i=0;
	pt_ram = (unsigned int xhuge *)(0x000000);
	/*** ciclo di srittura e verifica su tutti gli indirizzi ***/
	for ( pt_flash=(unsigned int xhuge *)(0x800000); pt_flash <=(unsigned int xhuge *)0x8FFFFE; pt_flash++){
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
/*		if (pt_ram == (unsigned int xhuge *)0x00DFFE) {
			printf("\r\n SEG 0 \r\n");
			pt_ram = (unsigned int xhuge *)0x010000;
			pt_flash = (unsigned int xhuge *)0x410000;
		}
*/
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
	error = chip_erase();
	for(i=0;i<1000;i++);
	if (!error) {
		error = wr_vrf();
	} 
	switch (error){
		case 0:
			printf("OK");
			break;
		case 1:
			printf("KO!!! chip1 erase timeout");
			break;
		case 2:
			printf("KO!!! chip2 erase timeout");
			break;
		case 5:
			printf("KO!!! verify write error");
			break;
		case 6:
			printf("KO!!! verify erase error");
			break;
	}
	printf("\r\n");
}

void MVB_init(void)
{
	unsigned int data1,i,vp;
	unsigned int  off,seg;
	unsigned long app; 

	DP3_2 = 1;
	DP3_3 = 1;

	*regscr1 = 0x0000; 						/* reset software				*/
	*regscr = 0x0000;
	for (i = 0; i<10; i++);								/* istruzione di attesa				*/
	*regscr = 0x0001;						/* predispone per l'inizializzazione dei reg.	*/
	*regmcr = funmode;						/* setta size della TM				*/
	for (i = 0; i<10; i++);								/* istruzione di attesa				*/
	/* azzera la LA port index table così tutti i LA port puntano al port 0 */
        for (st_DA_PITptr=(unsigned int xhuge *) st_LA_PIT; st_DA_PITptr<=(unsigned int xhuge *) end_LA_PIT; st_DA_PITptr++) *st_DA_PITptr = 0;	
	/* azzera tutti gli LA PCS per disattivare tutti gli LA port  */
        for (st_DA_PITptr=(unsigned int xhuge *) st_LA_PCS; st_DA_PITptr<=(unsigned int xhuge *) end_LA_PCS; st_DA_PITptr++) *st_DA_PITptr = 0;	
	/* azzera la DA port index table così tutti i DA port puntano al port 0 */
        for (st_DA_PITptr=(unsigned int xhuge *) st_DA_PIT; st_DA_PITptr<=(unsigned int xhuge *) end_DA_PIT; st_DA_PITptr++) *st_DA_PITptr = 0;	
	/* azzera tutti i DA PCS per disattivare tutti i DA port  */
        for (st_DA_PITptr=(unsigned int xhuge *) st_DA_PCS; st_DA_PITptr<=(unsigned int xhuge *) end_DA_PCS; st_DA_PITptr++) *st_DA_PITptr = 0;	
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
        st_DA_PITptr = (unsigned int xhuge *)(st_DA_PIT + 0x000E*2);
	*st_DA_PITptr = 0x0001;						/* utilizza il DA port n°1 per il proprio device add.*/
        st_DA_PITptr = (unsigned int xhuge *)(st_DA_PIT + 0x000D*2);
	*st_DA_PITptr = 0x0002;						/* utilizza il DA port n°2 per il device add. della stim*/
        st_DA_PCSptr = (unsigned int xhuge *)(st_DA_PCS + (0x0001<<3));
        *st_DA_PCSptr = 0xF420;		/* FC=15, sink port, no check seq., no write always, DTI1, no queue, no num data, no forcing data*/
	*(st_DA_PCSptr + 1) = 0x0000;
        st_DA_PCSptr1 = (unsigned int xhuge *)(st_DA_PCS + (0x0002<<3));
        *st_DA_PCSptr1 = 0xF440;		/* FC=15, sink port, no check seq., no write always, DTI2, no queue, no num data, no forcing data*/
	*(st_DA_PCSptr1 + 1) = 0x0000;
	vp = 0;
	st_DA_DATA0ptr =  (unsigned int xhuge *)(st_DA_DATA+(((0x0001<<4)&0xFFC0)|(vp<<5)|((0x0001<<3)&0x0018)));
	st_DA_DATA0ptr1 =  (unsigned int xhuge *)(st_DA_DATA+(((0x0002<<4)&0xFFC0)|(vp<<5)|((0x0002<<3)&0x0018)));
	vp = 1;
	st_DA_DATA1ptr =  (unsigned int xhuge *)(st_DA_DATA+(((0x0001<<4)&0xFFC0)|(vp<<5)|((0x0001<<3)&0x0018)));
	st_DA_DATA1ptr1 =  (unsigned int xhuge *)(st_DA_DATA+(((0x0002<<4)&0xFFC0)|(vp<<5)|((0x0002<<3)&0x0018)));
	FC15_PCSptr = (unsigned int xhuge *)st_FC15_PCS; 
	FC15_DATAptr = (unsigned int xhuge *)st_FC15_DATA0;
        *FC15_PCSptr = 0xF860;		/* FC=15, source port, no check seq., no write always, DTI3, no queue, no num data, no forcing data*/
	*(FC15_PCSptr + 1) = 0x0000;
        *FC15_DATAptr = 0xAAAA;
	P3_2 = 1;                                                      /* chiude relay KMA linea 1 MVB           */
	P3_3 = 1;                                                      /* chiude relay KMB linea 2 MVB           */
	*regscr1 = 0x0503;						/* abilito in full mode l'mvb	*/


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
	printf("P2=%04x\r\n",P2);
	printf("ISR0=%04x ISR1=%04x\r\n",*regisr0,*regisr1);
	printf("FC=%04x EC=%04x\r\n",*regfc,*regec);
	printf("PCSW0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_PCSptr,*(st_DA_PCSptr+1),*(st_DA_PCSptr+2),*(st_DA_PCSptr+3));
	printf("DATA0W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_DATA0ptr,*(st_DA_DATA0ptr+1),*(st_DA_DATA0ptr+2),*(st_DA_DATA0ptr+3));
	printf("DATA1W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_DATA1ptr,*(st_DA_DATA1ptr+1),*(st_DA_DATA1ptr+2),*(st_DA_DATA1ptr+3));
	printf("PCS1W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_PCSptr1,*(st_DA_PCSptr1+1),*(st_DA_PCSptr1+2),*(st_DA_PCSptr1+3));
	printf("DATA10W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_DATA0ptr1,*(st_DA_DATA0ptr1+1),*(st_DA_DATA0ptr1+2),*(st_DA_DATA0ptr1+3));
	printf("DATA11W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_DATA1ptr1,*(st_DA_DATA1ptr1+1),*(st_DA_DATA1ptr1+2),*(st_DA_DATA1ptr1+3));
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

void writecombuff(unsigned char cmd,unsigned char par)
{
	combuff[0] = STX;
	combuff[1] = cmd;
	combuff[2] = par;
	combuff[3] = ETX;
}

/****************************************************************/
/*  Programma principale scheda STB Campione                    */
/*                                                              */
/****************************************************************/
void C167_init()
{
    consolle_prompt("DEW_slave>") ;
    
}


void C167_background()
{
	unsigned int i,er,j;
	char c,startcont,uscita;
	unsigned char cmd,par;
	unsigned char xhuge * serrx;
	unsigned char xhuge * sertx;
	unsigned long timeout;
	unsigned short * lenrx;
	unsigned short olenrx;

	lenrx = &lenvar;
	/* inizializzazione I/O                            */
	P2 &= 0xFF87;
	i = 0;
	DP2 |= 0x0078;					/* P2.3-P2.6 in output*/
 	DP2 &= 0xFFF8;					/* P2.0-P2.2 in input*/
	P3 &= 0xDD3C;
	i = 0;
 	DP3 |= 0x22C3;					/* P3.0,P3.1,P3.6,P3.7,P3.9,P3.13 in output*/
 	DP3 &= 0xFECF;					/* P3.4-P3.5,P3.8 in input*/
	P6 &= 0xFF1F;
	i = 0;
 	DP6 |= 0x00E0;					/* P6.5-P6.7 in output*/
	P7 &= 0xFF00;
	i = 0;
 	DP7 |= 0x00FF;					/* P7.0-P7.7 in output*/
 	DP8 &= 0xFF02;					/* P8.0, P8.2-P8.7 in input*/
	P8 &= 0xFFFD;
	i = 0;
 	DP8 |= 0x0002;					/* P8.1 in output*/
	drvUart_init(UART_A,38400,0,_NONE);
	drvUart_init(UART_B,38400,0,_NONE);
	drvUart_init(UART_C,38400,0,_NONE);
	drvUart_init(UART_D,38400,0,_NONE);
	MVB_init();
	sertx= UART_A;
	serrx= UART_B;
	while (1)
	{
		*lenrx = 0;
		olenrx = 0;
		printf("\r\n**** ATTESA COMANDO DA MASTER!!!! ****\r\n");
		i = 10000;
		timeout = 0;
		startcont = 0;
		uscita = 1;
		while (uscita) 
		{
		       	drvUart_riceviDati(serrx,combuff,lenrx);
                        if ((olenrx != *lenrx) || (i == 10000)) {
				i = 0;
				olenrx = *lenrx;
				printf("\r                                                  ");
 				printf("\rLSR=%04x rx=",serrx[LSR]);
				for (j = 0; j<*lenrx; j++) printf("%02x ",combuff[j]);
	                        *regisr0=0;
				*regivr0=0;
				*regisr1=0;
				*regivr1=0;
				*regfc=0;
				*regec=0;
			}
			if ( *lenrx != 0 )
			{
				if ( combuff[0] != STX)
				{
					*lenrx = 0;
					olenrx = 0;
				}
				else {
					startcont = 1; 
					if (*lenrx >= 4)
						if ( combuff[3] != ETX)
						{
							*lenrx = 0;
							olenrx = 0;
							timeout = 0;
							startcont = 0;
							writecombuff(NACK,0);
							drvUart_inviaDati(sertx,combuff,4);
							printf("\r\nFRAME ERROR\r\n");
							i = 30000;
						}
						else uscita = 0;
				}	
			}
			i++;
			if ( startcont) timeout++;
			if ( timeout == (unsigned long)(1000000))
			{
				*lenrx = 0;
				olenrx = 0;
				timeout = 0;
				startcont = 0;
				writecombuff(NACK,0);
				drvUart_inviaDati(sertx,combuff,4);
				printf("\r\nERRORE TIMEOUT\r\n");
				i = 30000;
			}
			if ((c = _getkey())  == 'w') ram2flash();
			if (c == 'r')
			{
				printf("\r\nP2=%04x\r\n",P2);
				printf("ISR0=%04x ISR1=%04x\r\n",*regisr0,*regisr1);
 				printf("FC=%04x EC=%04x\r\n",*regfc,*regec);
				printf("PCSW0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_PCSptr,*(st_DA_PCSptr+1),*(st_DA_PCSptr+2),*(st_DA_PCSptr+3));
				printf("DATA0W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_DATA0ptr,*(st_DA_DATA0ptr+1),*(st_DA_DATA0ptr+2),*(st_DA_DATA0ptr+3));
    		       		printf("DATA1W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_DATA1ptr,*(st_DA_DATA1ptr+1),*(st_DA_DATA1ptr+2),*(st_DA_DATA1ptr+3));
				printf("PCS1W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_PCSptr1,*(st_DA_PCSptr1+1),*(st_DA_PCSptr1+2),*(st_DA_PCSptr1+3));
		      		printf("DATA10W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_DATA0ptr1,*(st_DA_DATA0ptr1+1),*(st_DA_DATA0ptr1+2),*(st_DA_DATA0ptr1+3));
		       		printf("DATA11W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_DATA1ptr1,*(st_DA_DATA1ptr1+1),*(st_DA_DATA1ptr1+2),*(st_DA_DATA1ptr1+3));
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
			        *FC15_DATAptr = ~*FC15_DATAptr;
			}
		}
		printf("\r\n");
		cmd = combuff[1];
		par = combuff[2];

		switch (cmd)
		{
		case TX_UARTA :   /* Comando trasmette carattere su uart A             */
			printf("ESEGUO COMANDO TX_UARTA\r\n");
			printf("COMANDO ESEGUITO\r\n");
			break;
		case RX_UARTA :   /* Comando ricevi carattere su uart A                */
			printf("ESEGUO COMANDO RX_UARTA\r\n");
			*lenrx = 0;
		       	drvUart_riceviDati(UART_A,combuff,lenrx);
			if (*lenrx != 0) 
			{
				drvUart_inviaDati(UART_B,combuff,1);	
				writecombuff(ACK,0);
				drvUart_inviaDati(sertx,combuff,4);
			}
			else
			{
				writecombuff(NACK,0);
				drvUart_inviaDati(sertx,combuff,4);
			}
			printf("COMANDO ESEGUITO\r\n");
			break;
		case TX_UARTB :   /* Comando trasmette carattere su uart B             */
			printf("ESEGUO COMANDO TX_UARTB\r\n");
			printf("COMANDO ESEGUITO\r\n");
			break;
		case RX_UARTB :   /* Comando ricevi carattere su uart B                */
			printf("ESEGUO COMANDO RX_UARTB\r\n");
			*lenrx = 0;
		       	drvUart_riceviDati(UART_B,combuff,lenrx);
			if (*lenrx != 0) 
			{
				drvUart_inviaDati(UART_A,combuff,1);	
				writecombuff(ACK,0);
				drvUart_inviaDati(sertx,combuff,4);
			}
			else
			{
				writecombuff(NACK,0);
				drvUart_inviaDati(sertx,combuff,4);
			}
			printf("COMANDO ESEGUITO\r\n");
			break;
		case TX_UARTC :   /* Comando trasmette carattere su uart C             */
			printf("ESEGUO COMANDO TX_UARTC\r\n");
			printf("COMANDO ESEGUITO\r\n");
			break;
		case RX_UARTC :   /* Comando ricevi carattere su uart C                */
			printf("ESEGUO COMANDO RX_UARTC\r\n");
			*lenrx = 0;
		       	drvUart_riceviDati(UART_C,combuff,lenrx);
			if (*lenrx != 0) 
			{
				drvUart_inviaDati(UART_D,combuff,1);	
				writecombuff(ACK,0);
				drvUart_inviaDati(sertx,combuff,4);
			}
			else
			{
				writecombuff(NACK,0);
				drvUart_inviaDati(sertx,combuff,4);
			}
			printf("COMANDO ESEGUITO\r\n");
			break;
		case TX_UARTD :   /* Comando trasmette carattere su uart D             */
			printf("ESEGUO COMANDO TX_UARTD\r\n");
			printf("COMANDO ESEGUITO\r\n");
			break;
		case RX_UARTD :   /* Comando ricevi carattere su uart D                */
			printf("ESEGUO COMANDO RX_UARTD\r\n");
			*lenrx = 0;
		       	drvUart_riceviDati(UART_D,combuff,lenrx);
			if (*lenrx != 0) 
			{
				drvUart_inviaDati(UART_C,combuff,1);	
				writecombuff(ACK,0);
				drvUart_inviaDati(sertx,combuff,4);
			}
			else
			{
				writecombuff(NACK,0);
				drvUart_inviaDati(sertx,combuff,4);
			}
			printf("COMANDO ESEGUITO\r\n");
			break;
		case SWTSERC_D :   /* Comando Switch della seriale di controllo da (A-B) a (C-D)  */
			printf("ESEGUO COMANDO Switch seriale controllo C_D\r\n");
			writecombuff(ACK,0);
			drvUart_inviaDati(sertx,combuff,4);
			sertx= UART_C;
			serrx= UART_D;
			printf("COMANDO ESEGUITO\r\n");
			break;
		case SWTSERA_B :   /* Comando Switch della seriale di controllo da (C-D) a (A-B)  */
			printf("ESEGUO COMANDO Switch seriale controllo A_B\r\n");
			writecombuff(ACK,0);
			drvUart_inviaDati(sertx,combuff,4);
			sertx= UART_A;
			serrx= UART_B;
			printf("COMANDO ESEGUITO\r\n");
			break;
		case WRTOUT1 :   /* Comando scrive OUT1                                     */
			printf("ESEGUO COMANDO WRTOUT1\r\n");
			write_port_out1(par);
			writecombuff(ACK,0);
			drvUart_inviaDati(sertx,combuff,4);
			printf("COMANDO ESEGUITO\r\n");
			break;
		case WRTOUT2 :   /* Comando scrive OUT2                                     */
			printf("ESEGUO COMANDO WRTOUT1\r\n");
			write_port_out2(par);
			writecombuff(ACK,0);
			drvUart_inviaDati(sertx,combuff,4);
			printf("COMANDO ESEGUITO\r\n");
			break;
		case WRTOUT3 :   /* Comando scrive OUT3                                     */
			printf("ESEGUO COMANDO WRTOUT1\r\n");
			write_port_out3(par);
			writecombuff(ACK,0);
			drvUart_inviaDati(sertx,combuff,4);
			printf("COMANDO ESEGUITO\r\n");
			break;
		case WRTOUT4 :   /* Comando scrive OUT4                                     */
			printf("ESEGUO COMANDO WRTOUT1\r\n");
			write_port_out4(par);
			writecombuff(ACK,0);
			drvUart_inviaDati(sertx,combuff,4);
			printf("COMANDO ESEGUITO\r\n");
			break;
		case RDIN1 :   /* Comando Read IN1                                          */
			printf("ESEGUO COMANDO RDIN1\r\n");
			par=read_port_in1();
			writecombuff(ACK,par);
			drvUart_inviaDati(sertx,combuff,4);
			printf("COMANDO ESEGUITO\r\n");
			break;
		case RDIN2 :   /* Comando Read IN2                                          */
			printf("ESEGUO COMANDO RDIN2\r\n");
			par=read_port_in2();
			writecombuff(ACK,par);
			drvUart_inviaDati(sertx,combuff,4);
			printf("COMANDO ESEGUITO\r\n");
			break;
		case RDIN3 :   /* Comando Read IN3                                          */
			printf("ESEGUO COMANDO RDIN3\r\n");
			par=read_port_in3();
			writecombuff(ACK,par);
			drvUart_inviaDati(sertx,combuff,4);
			printf("COMANDO ESEGUITO\r\n");
			break;
		case RDIN4 :   /* Comando Read IN4                                          */
			printf("ESEGUO COMANDO RDIN4\r\n");
			par=read_port_in4();
			writecombuff(ACK,par);
			drvUart_inviaDati(sertx,combuff,4);
			printf("COMANDO ESEGUITO\r\n");
			break;
		case SINC :   /* Comando di sincronizzazione                                 */
			printf("ESEGUO COMANDO SINC\r\n");
			writecombuff(ACK,0);
			drvUart_inviaDati(sertx,combuff,4);
			printf("COMANDO ESEGUITO\r\n");
			break;
		default :
			printf("COMANDO SCONOSCIUTO!!!!! %02x\r\n",cmd);
			writecombuff(NACK,0);
			drvUart_inviaDati(sertx,combuff,4);
			break;
		}
 	}

}




