/*****************************************************************************
* Lun.17.nov.03           v.1.00   sw_st\sw_stb\test_stbxx\source\tstaux.c  *
******************************************************************************
* Routine per test delle porte seriali ausiliarie                            *
******************************************************************************
* 						                             *
*****************************************************************************/

#include <stdio.h>
#include "serbuf.h"
#include "consolle.h"
#include "libdew.h"
#include "tstflash.h"   
#include <cod_st.h>
#include "handshk.h"
#include "drvuart.h"
/*#include <reg167.h>
#include <bitfield.h>*/
#pragma ot(3,SPEED)



/* costante di ritardo per consentire la stampa dei messaggi */
#define  WAIT    (unsigned int)(40000)

typedef union LInt
{
    unsigned long b32;
    struct
    {
        unsigned char byte1;
        unsigned char byte2;
        unsigned char byte3;
        unsigned char byte4;
    } b8;
} LInt;


LInt out;
LInt rd;


unsigned char combuff[LUNGHEZZA_CODA];
unsigned short lenvar;
unsigned char  * serrx;
unsigned char  * sertx;

void writecombuff(unsigned char cmd,unsigned char par)
{
	combuff[0] = STX;
	combuff[1] = cmd;
	combuff[2] = par;
	combuff[3] = ETX;
}


void test_tem (void)
{
	unsigned int i,j,lettura;
	float temperature;


	i = 0;
        while (_getkey() != '\r')
	{
      		LD6_START_ADC166(15,0);  /*genera lo start of convertion     */
 			 		/*sul canale j dell'adc interno     */
 		for(j=0; j<100; j++);
         	LD6_WAIT_ADC166;        /* aspetta l'END OF CONVERTION      */
		lettura = (ADDAT & 0x3FF);
         	temperature = (((float)(lettura*4))/((float)(11))) - (float)(273);
		if ( (i == 0) || (i == 30000) ) printf("\rTemperatura della scheda = %.1f",temperature);
		i++;
	}
	printf("\r\n");
	
}



unsigned int send_cmd (unsigned char varcmd,unsigned char p)
{
	unsigned int i,er,j,txretry;
	unsigned int k;
	char uscita;
	unsigned char cmd,par;
	unsigned short * lenrx;
	unsigned short olenrx;
	unsigned long timeout;


 	lenrx = &lenvar;
	er = 0;
	cmd = 0;
	txretry = 0;
	while (cmd != ACK)
	{		
                if ( txretry == 10) break;
		txretry++;
		writecombuff(varcmd,p);
		drvUart_inviaDati(sertx,combuff,4);

		*lenrx = 0;
		olenrx = 0;
		uscita = 1;
		timeout = 0;
//		k = 0;
		while (uscita ) 
		{
//			if ( (k==0) || (k == 32000)) printf("\r%04x",k);
//			k++;
	       		drvUart_riceviDati(serrx,combuff,lenrx);
//                       	if ((olenrx != *lenrx) || (c == '\r')) {
//                       	if ((*c == 'r')) {
//				olenrx = *lenrx;
//				printf("\r                                                  ");
//				printf("\rLen=%04x rx=",*lenrx);
//				for (j = 0; j<*lenrx; j++) printf("%02x ",combuff[j]);
//			}
			if ( *lenrx != 0 )
			{
				if ( combuff[0] != STX)
				{
					*lenrx = 0;
					olenrx = 0;
				}
				else {
					if (*lenrx >= 4)
						if ( combuff[3] != ETX)
						{
							*lenrx = 0;
							olenrx = 0;
							printf("\r\nFRAME ERROR\r\n");
						}
						else uscita = 0;
				}	
			}
			if ( timeout == (unsigned long)(600000) )
			{
				printf("\r\nTIMEOUT ERROR\r\n");
				break;
			}
			timeout++;
		}
//		printf("\r\n");
		if (uscita == 0)
		{
			cmd = combuff[1];
			par = combuff[2];
		}
		else
		{
			er =1;
			break;
		}
	}
	return er;
}



unsigned int t_ser (unsigned char  * uarttx,unsigned char  * uartrx,unsigned char varcmd)
{
	unsigned int i,er,j;
	unsigned int k;
	char c,uscita;
	unsigned char cmd,par;
	unsigned short * lenrx;
	unsigned short olenrx;
	unsigned long timeout;

 	lenrx = &lenvar;
	er = 0;
	for ( i = 0; i <= 15; i++ )
	{
		combuff[0] = (0x0030 + i);
		drvUart_inviaDati(uarttx,combuff,1);

		cmd = 0;
		while (cmd != ACK)
		{		
			writecombuff(varcmd,0);
       			drvUart_inviaDati(sertx,combuff,4);

			*lenrx = 0;
			olenrx = 0;
			uscita = 1;
			timeout = 0;
//			k = 0;
			while (uscita && ((c=_getkey())!='\t')) 
			{
//				if ( (k==0) || (k == 32000)) printf("\r%04x",k);
//				k++;
		       		drvUart_riceviDati(serrx,combuff,lenrx);
//                        	if ((olenrx != *lenrx) || (c == '\r')) {
//					olenrx = *lenrx;
//					printf("\r                                                  ");
// 					printf("\rLen=%04x rx=",*lenrx);
//					for (j = 0; j<*lenrx; j++) printf("%02x ",combuff[j]);
//				}
				if ( *lenrx != 0 )
				{
					if ( combuff[0] != STX)
					{
						*lenrx = 0;
						olenrx = 0;
					}
					else {
						if (*lenrx >= 4)
							if ( combuff[3] != ETX)
							{
								*lenrx = 0;
								olenrx = 0;
								printf("\r\nFRAME ERROR\r\n");
							}
							else uscita = 0;
					}	
				}
				if ( timeout == (unsigned long)(600000) )
				{
					printf("\r\nTIMEOUT ERROR\r\n");
					break;
				}
				timeout++;
			}
//			printf("\r\n");
			if (uscita == 0)
			{
				cmd = combuff[1];
				par = combuff[2];
			}
			else
			{
				er = 1;
				break;
			}
		}
		if ( er)
		{
			printf("ERRORE!!!! Uscita forzata dal loop ricezione\r\n");
			break;
		}
		else
		{
			*lenrx = 0;
	      		while ((*lenrx == 0) && (_getkey() != '\t'))
			{
		       		drvUart_riceviDati(uartrx,combuff,lenrx);
				if ((*lenrx != 0) && (combuff[0] != (0x30 + i)))
				{
					printf("ERRORE Atteso Carattere %02x Letto %02x\r\n",(0x30 + i),par);
					er = 1;
				}
			}
		}
	}
	return er;
}

unsigned int swtsercom (unsigned char varcmd)
{
	unsigned int i,er,j;
	unsigned int k;
	char c,uscita;
	unsigned char cmd,par;
	unsigned short * lenrx;
	unsigned short olenrx;


 	lenrx = &lenvar;
	er = 0;
	cmd = 0;
	while (cmd != ACK)
	{		
		writecombuff(varcmd,0);
		drvUart_inviaDati(sertx,combuff,4);

		*lenrx = 0;
		olenrx = 0;
		uscita = 1;
		k = 0;
		while (uscita && ((c=_getkey())!='\t')) 
		{
			if ( (k==0) || (k == 32000)) printf("\r%04x",k);
			k++;
	       		drvUart_riceviDati(serrx,combuff,lenrx);
                       	if ((olenrx != *lenrx) || (c == '\r')) {
				olenrx = *lenrx;
				printf("\r                                                  ");
				printf("\rLen=%04x rx=",*lenrx);
				for (j = 0; j<*lenrx; j++) printf("%02x ",combuff[j]);
			}
			if ( *lenrx != 0 )
			{
				if ( combuff[0] != STX)
				{
					*lenrx = 0;
					olenrx = 0;
				}
				else {
					if (*lenrx >= 4)
						if ( combuff[3] != ETX)
						{
							*lenrx = 0;
							olenrx = 0;
							printf("\r\nFRAME ERROR\r\n");
						}
						else uscita = 0;
				}	
			}
		}
		printf("\r\n");
		if (uscita == 0)
		{
			cmd = combuff[1];
			par = combuff[2];
		}
		else
		{
			er =1;
			break;
		}
	}
	if (uscita == 0)
	{
		switch (varcmd)
		{
		case SWTSERC_D: 
			sertx= UART_C;
			serrx= UART_D;
			break;
		case SWTSERA_B: 
			sertx= UART_A;
			serrx= UART_B;
			break;
		default:
			er = 1;
			break;
		}
	}
	else printf ("IMPOSSIBILE CAMBIARE SERIALI DI CONTROLLO\r\n");
	return er;
}



unsigned int initsercom (void)
{
	unsigned int i,er,j;
	unsigned int k;
	char c,uscita;
	unsigned char cmd,par;
	unsigned short * lenrx;
	unsigned short olenrx;

	drvUart_init(UART_A,38400,0,_NONE);
	drvUart_init(UART_B,38400,0,_NONE);
	drvUart_init(UART_C,38400,0,_NONE);
	drvUart_init(UART_D,38400,0,_NONE);

	sertx= UART_A;
	serrx= UART_B;
 	lenrx = &lenvar;
	er = 0;

	cmd = 0;
	while (cmd != ACK)
	{		
		writecombuff(SINC,0);
		drvUart_inviaDati(sertx,combuff,4);

		*lenrx = 0;
		olenrx = 0;
		uscita = 1;
		k = 0;
		while (uscita && ((c=_getkey())!='\t')) 
		{
			if ( (k==0) || (k == 32000)) printf("\r%04x",k);
			k++;
	       		drvUart_riceviDati(serrx,combuff,lenrx);
                       	if ((olenrx != *lenrx) || (c == '\r')) {
				olenrx = *lenrx;
				printf("\r                                                  ");
				printf("\rLen=%04x rx=",*lenrx);
				for (j = 0; j<*lenrx; j++) printf("%02x ",combuff[j]);
			}
			if ( *lenrx != 0 )
			{
				if ( combuff[0] != STX)
				{
					*lenrx = 0;
					olenrx = 0;
				}
				else {
					if (*lenrx >= 4)
						if ( combuff[3] != ETX)
						{
							*lenrx = 0;
							olenrx = 0;
							printf("\r\nFRAME ERROR\r\n");
						}
						else uscita = 0;
				}	
			}
		}
		printf("\r\n");
		if (uscita == 0)
		{
			cmd = combuff[1];
			par = combuff[2];
		}
		else
		{
			er =1;
			break;
		}
	}
	if (uscita == 0)
	{
		if (!swtsercom (SWTSERC_D))
		{
			cmd = 0;
			while (cmd != ACK)
			{		
				writecombuff(SINC,0);
				drvUart_inviaDati(sertx,combuff,4);

				*lenrx = 0;
				olenrx = 0;
				uscita = 1;
				k = 0;
				while (uscita && ((c=_getkey())!='\t')) 
				{
					if ( (k==0) || (k == 32000)) printf("\r%04x",k);
					k++;
	       				drvUart_riceviDati(serrx,combuff,lenrx);
                       			if ((olenrx != *lenrx) || (c == '\r')) {
						olenrx = *lenrx;
						printf("\r                                                  ");
						printf("\rLen=%04x rx=",*lenrx);
						for (j = 0; j<*lenrx; j++) printf("%02x ",combuff[j]);
					}
					if ( *lenrx != 0 )
					{
						if ( combuff[0] != STX)
						{
							*lenrx = 0;
							olenrx = 0;
						}
						else {
							if (*lenrx >= 4)
								if ( combuff[3] != ETX)
								{
									*lenrx = 0;
									olenrx = 0;
									printf("\r\nFRAME ERROR\r\n");
								}
								else uscita = 0;
						}	
					}
				}
				printf("\r\n");
				if (uscita == 0)
				{
					cmd = combuff[1];
					par = combuff[2];
				}
				else
				{
					er =1;
					break;
				}
			}
			if (uscita == 0) er = swtsercom (SWTSERA_B);
			else printf ("ERRORE DI CONNESSIONE CON SCHEDA STIM SU UART D\r\n");
		}
		else er = 1;	
	}
	else printf ("ERRORE DI CONNESSIONE CON SCHEDA STIM SU UART A\r\n");
	return er;
}


void test_ser (void)
{
	unsigned long m;



     	printf("TEST Seriali\r\n");


/*
     i = 10000;
     *lenrx = 0;
     olenrx = 0;
	printf("\rLSR=%04x lerx=%04x rx=",serrx[LSR],(*lenrx));
     while ((c =_getkey()) != '\r'){
	switch (c){
		case '1':
			printf("\r\n1\r\n");
			combuff[0] = 0x21;
			combuff[1] = 0x22;
			combuff[2] = 0x23;
			combuff[3] = 0x24;
			drvUart_inviaDati(sertx,combuff,4);
			break;
		case '2':
			printf("\r\n2\r\n");
			combuff[0] = STX;
			combuff[1] = 0x21;
			combuff[2] = 0x22;
			combuff[3] = 0x23;
			drvUart_inviaDati(sertx,combuff,4);
			break;
		case '3':
			printf("\r\n3\r\n");
			writecombuff(SINC,0);
			drvUart_inviaDati(sertx,combuff,4);
			break;
		case '4':
			printf("\r\n4\r\n");
			writecombuff(0x66,0);
			drvUart_inviaDati(sertx,combuff,4);
			break;
		case '5':
			printf("\r\n5\r\n");
			writecombuff(SWTSERC_D,0);
			drvUart_inviaDati(sertx,combuff,4);
			sertx= UART_C;
			serrx= UART_D;
			break;
		case '6':
			printf("\r\n6\r\n");
			writecombuff(SWTSERA_B,0);
			drvUart_inviaDati(sertx,combuff,4);
			sertx= UART_A;
			serrx= UART_B;
			break;
		case '7':
		       	drvUart_riceviDati(serrx,combuff,lenrx);
			printf("\rLSR=%04x lerx=%04x rx=",serrx[LSR],(*lenrx));
			for (j = 0; j<*lenrx; j++) printf("%02x ",combuff[j]);
			break;
		default :
		       	drvUart_riceviDati(serrx,combuff,lenrx);
                        if ( (olenrx != *lenrx) || (i == 10000)) {              
				i = 0;
				olenrx = *lenrx;
 				printf("\rLSR=%04x rx=",serrx[LSR]);
				for (j = 0; j<*lenrx; j++) printf("%02x ",combuff[j]);
			}
       			break;

	}
	i++;
     }

*/
     if ( initsercom() == 0)
     {		
	for ( m = 0; m<(unsigned long)(400000); m++);

	printf("Test di SERC in TX e SERD in RX\r\n");	
	if (t_ser(UART_C,UART_D,RX_UARTD)) save_stato(TTXUARTCKO);
	else save_stato(TTXUARTCOK);

//	for ( m = 0; m<(unsigned long)(400000); m++);

	printf("Test di SERD in TX e SERC in RX\r\n");	
	if (t_ser(UART_D,UART_C,RX_UARTC)) save_stato(TTXUARTDKO);
	else save_stato(TTXUARTDOK);

//	for ( m = 0; m<(unsigned long)(400000); m++);

	swtsercom (SWTSERC_D);	

//	for ( m = 0; m<(unsigned long)(400000); m++);


	printf("Test di SERA in TX e SERB in RX\r\n");	
	if (t_ser(UART_A,UART_B,RX_UARTB)) save_stato(TTXUARTAKO);
	else save_stato(TTXUARTAOK);

//	for ( m = 0; m<(unsigned long)(400000); m++);

	printf("Test di SERB in TX e SERA in RX\r\n");	
	if (t_ser(UART_B,UART_A,RX_UARTA)) save_stato(TTXUARTBKO);
	else save_stato(TTXUARTBOK);

//	for ( m = 0; m<(unsigned long)(400000); m++);

	swtsercom (SWTSERA_B);	
     }
     else printf("Impossibile eseguire il test per mancato collegamento con STIM\r\n");	
	
}


unsigned int initpio (void)
{
	unsigned int i,er,j;
	unsigned int k;
	char c,uscita;
	unsigned char cmd,par;
	unsigned short * lenrx;
	unsigned short olenrx;


	/* inizializzazione I/O                            */
	P2 &= 0xFF87;
	i = 0;
	DP2 |= 0x0078;					/* P2.3-P2.6 in output*/
 	DP2 &= 0x87F8;					/* P2.0-P2.2,P2.11-P2.14 in input*/
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
	sertx= UART_A;
	serrx= UART_B;
 	lenrx = &lenvar;
	er = 0;
	cmd = 0;
	while (cmd != ACK)
	{		
		writecombuff(SINC,0);
//		printf("\r\n");
//		for (j = 0; j<4; j++) printf("%02x ",combuff[j]);
//		printf("\r\n");
		drvUart_inviaDati(sertx,combuff,4);
//		printf("\r\n");

		*lenrx = 0;
		olenrx = 0;
		uscita = 1;
		k = 0;
		while (uscita && ((c=_getkey())!='\t')) 
		{
			if ( (k==0) || (k == 32000)) printf("\r%04x",k);
			k++;
	       		drvUart_riceviDati(serrx,combuff,lenrx);
                       	if ((olenrx != *lenrx) || (c == '\r')) {
				olenrx = *lenrx;
				printf("\r                                                  ");
				printf("\rLen=%04x rx=",*lenrx);
				for (j = 0; j<*lenrx; j++) printf("%02x ",combuff[j]);
			}
			if ( *lenrx != 0 )
			{
				if ( combuff[0] != STX)
				{
					*lenrx = 0;
					olenrx = 0;
				}
				else {
					if (*lenrx >= 4)
						if ( combuff[3] != ETX)
						{
							*lenrx = 0;
							olenrx = 0;
							printf("\r\nFRAME ERROR\r\n");
						}
						else uscita = 0;
				}	
			}
		}
		printf("\r\n");
		if (uscita == 0)
		{
			cmd = combuff[1];
			par = combuff[2];
		}
		else
		{
			er =1;
			break;
		}
	}
	if (uscita != 0) printf ("ERRORE DI CONNESSIONE CON SCHEDA STIM SU UART A\r\n");
	return er;
}

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

void test_pio (void)
{
	unsigned int i,ko,ko1,intr,intrdp;
	unsigned char msra,msrb,msrc,msrd;
	unsigned long x;

	printf("**** TEST PIO ****\r\n");

        if ( initpio() == 0)
        {		
               	printf("\r\nTest delle linee di interrupt ");

		ko=0;

               	msra = UART_A[MSR];
               	msrb = UART_B[MSR];
               	msrc = UART_C[MSR];
               	msrd = UART_D[MSR];
		intr = P2 & 0x7800;	
                if ( intr != 0x0000)
		{ 
		   printf ("\r\nErrore: Interrupt ricevuta all'abilitazione inter=%04x MSRA=%02x MSRB=%02x MSRC=%02x MSRD=%02x",intr,msra,msrb,msrc,msrd);
		   ko = 1;    
		}
                UART_A[IER] = 0x08;
		intr = P2 & 0x7800;	
                if ( intr != 0x0000)
		{ 
		   printf ("\r\nErrore: Interrupt ricevuta all'abilitazione di UARTA inter=%04x",intr);
		   ko = 1;	
                }

	     	if (send_cmd(WRTOUT3,0x01) == 0) 
		{
			intr = P2 & 0x7800;	
	               	msra = UART_A[MSR];
			intrdp = P2 & 0x7800;	
        	       	msrb = UART_B[MSR];
               		msrc = UART_C[MSR];
               		msrd = UART_D[MSR];
	                if ( (intr != 0x0800) || (intrdp != 0))
			{ 
			   printf ("\r\nErrore: Interrupt non ricevuta su UARTA WR1 inter=%04x intrdp=%04x MSRA=%02x MSRB=%02x MSRC=%02x MSRD=%02x",intr,intrdp,msra,msrb,msrc,msrd);
			   ko=1;	
			}
		}
		else 
		{
			printf ("\r\nTest interrrupt UARTA non eseguito");
			ko =1;
		}
		
	     	if (send_cmd(WRTOUT3,0x00) == 0) 
		{
			intr = P2 & 0x7800;	
	               	msra = UART_A[MSR];
			intrdp = P2 & 0x7800;	
        	       	msrb = UART_B[MSR];
               		msrc = UART_C[MSR];
               		msrd = UART_D[MSR];
	                if ( (intr != 0x0800) || (intrdp != 0))
			{ 
			   printf ("\r\nErrore: Interrupt non ricevuta su UARTA WR0 inter=%04x intrdp=%04x MSRA=%02x MSRB=%02x MSRC=%02x MSRD=%02x",intr,intrdp,msra,msrb,msrc,msrd);
			   ko=1;	
			}
		}
		else 
		{
			printf ("\r\nTest interrrupt UARTA non eseguito");
			ko =1;
		}

                UART_A[IER] = 0x00;
                UART_B[IER] = 0x08;
		intr = P2 & 0x7800;	
                if ( intr != 0x0000)
		{ 
		   printf ("\r\nErrore: Interrupt ricevuta all'abilitazione di UARTB inter=%04x",intr);
		   ko = 1;	
                }

	     	if (send_cmd(WRTOUT3,0x02) == 0) 
		{
			intr = P2 & 0x7800;	
	               	msrb = UART_B[MSR];
			intrdp = P2 & 0x7800;	
        	       	msra = UART_A[MSR];
               		msrc = UART_C[MSR];
               		msrd = UART_D[MSR];
	                if ( (intr != 0x1000) || (intrdp != 0))
			{ 
			   printf ("\r\nErrore: Interrupt non ricevuta su UARTB WR1 inter=%04x intrdp=%04x MSRA=%02x MSRB=%02x MSRC=%02x MSRD=%02x",intr,intrdp,msra,msrb,msrc,msrd);
			   ko=1;	
			}
		}
		else 
		{
			printf ("\r\nTest interrrupt UARTB non eseguito");
			ko =1;
		}
		
	     	if (send_cmd(WRTOUT3,0x00) == 0) 
		{
			intr = P2 & 0x7800;	
	               	msrb = UART_B[MSR];
			intrdp = P2 & 0x7800;	
        	       	msra = UART_A[MSR];
               		msrc = UART_C[MSR];
               		msrd = UART_D[MSR];
	                if ( (intr != 0x1000) || (intrdp != 0))
			{ 
			   printf ("\r\nErrore: Interrupt non ricevuta su UARTB WR0 inter=%04x intrdp=%04x MSRA=%02x MSRB=%02x MSRC=%02x MSRD=%02x",intr,intrdp,msra,msrb,msrc,msrd);
			   ko=1;	
			}
		}
		else 
		{
			printf ("\r\nTest interrrupt UARTB non eseguito");
			ko =1;
		}

                UART_B[IER] = 0x00;
                UART_C[IER] = 0x08;
		intr = P2 & 0x7800;	
                if ( intr != 0x0000)
		{ 
		   printf ("\r\nErrore: Interrupt ricevuta all'abilitazione di UARTC inter=%04x",intr);
		   ko = 1;	
                }

	     	if (send_cmd(WRTOUT3,0x04) == 0) 
		{
			intr = P2 & 0x7800;	
	               	msrc = UART_C[MSR];
			intrdp = P2 & 0x7800;	
        	       	msra = UART_A[MSR];
               		msrb = UART_B[MSR];
               		msrd = UART_D[MSR];
	                if ( (intr != 0x2000) || (intrdp != 0))
			{ 
			   printf ("\r\nErrore: Interrupt non ricevuta su UARTC WR1 inter=%04x intrdp=%04x MSRA=%02x MSRB=%02x MSRC=%02x MSRD=%02x",intr,intrdp,msra,msrb,msrc,msrd);
			   ko=1;	
			}
		}
		else 
		{
			printf ("\r\nTest interrrupt UARTC non eseguito");
			ko =1;
		}
		
	     	if (send_cmd(WRTOUT3,0x00) == 0) 
		{
			intr = P2 & 0x7800;	
	               	msrc = UART_C[MSR];
			intrdp = P2 & 0x7800;	
        	       	msra = UART_A[MSR];
               		msrb = UART_B[MSR];
               		msrd = UART_D[MSR];
	                if ( (intr != 0x2000) || (intrdp != 0))
			{ 
			   printf ("\r\nErrore: Interrupt non ricevuta su UARTC WR0 inter=%04x intrdp=%04x MSRA=%02x MSRB=%02x MSRC=%02x MSRD=%02x",intr,intrdp,msra,msrb,msrc,msrd);
			   ko=1;	
			}
		}
		else 
		{
			printf ("\r\nTest interrrupt UARTC non eseguito");
			ko =1;
		}


                UART_C[IER] = 0x00;
                UART_D[IER] = 0x08;
		intr = P2 & 0x7800;	
                if ( intr != 0x0000)
		{ 
		   printf ("\r\nErrore: Interrupt ricevuta all'abilitazione di UARTD inter=%04x",intr);
		   ko = 1;	
                }

	     	if (send_cmd(WRTOUT3,0x08) == 0) 
		{
			intr = P2 & 0x7800;	
	               	msrd = UART_D[MSR];
			intrdp = P2 & 0x7800;	
        	       	msra = UART_A[MSR];
               		msrb = UART_B[MSR];
               		msrc = UART_C[MSR];
	                if ( (intr != 0x4000) || (intrdp != 0))
			{ 
			   printf ("\r\nErrore: Interrupt non ricevuta su UARTD WR1 inter=%04x intrdp=%04x MSRA=%02x MSRB=%02x MSRC=%02x MSRD=%02x",intr,intrdp,msra,msrb,msrc,msrd);
			   ko=1;	
			}
		}
		else 
		{
			printf ("\r\nTest interrrupt UARTD non eseguito");
			ko =1;
		}
		
	     	if (send_cmd(WRTOUT3,0x00) == 0) 
		{
			intr = P2 & 0x7800;	
	               	msrd = UART_D[MSR];
			intrdp = P2 & 0x7800;	
        	       	msra = UART_A[MSR];
               		msrb = UART_B[MSR];
               		msrc = UART_C[MSR];
	                if ( (intr != 0x4000) || (intrdp != 0))
			{ 
			   printf ("\r\nErrore: Interrupt non ricevuta su UARTD WR0 inter=%04x intrdp=%04x MSRA=%02x MSRB=%02x MSRC=%02x MSRD=%02x",intr,intrdp,msra,msrb,msrc,msrd);
			   ko=1;	
			}
		}
		else 
		{
			printf ("\r\nTest interrrupt UARTD non eseguito");
			ko =1;
		}

                UART_D[IER] = 0x00;
		if ( ko == 0) printf("OK");

		/* test degli output shift 0*/
		printf("\r\nTest degli OUTPUT Shift degli zeri\r\n");
		for (x = (unsigned long)(0x00000001); x != (unsigned long)(0x40000000); x = x<<1)
		{
		     ko1 = 1;
		     out.b32 = ((~x) & (unsigned long)(0x1FFFFFFF));	
		     while ( (ko1 == 1) && (_getkey() != '\r'))
		     {		
			     write_port_out1(out.b8.byte1);
			     write_port_out2(out.b8.byte2);
			     write_port_out3(out.b8.byte3);
			     write_port_out4(out.b8.byte4);
			     if (send_cmd(RDIN1,0) == 0) rd.b8.byte1 = combuff[2];
			     else rd.b8.byte1 = 0xAA;
			     if (send_cmd(RDIN2,0) == 0) rd.b8.byte2 = combuff[2];
			     else rd.b8.byte2 = 0xAA;
			     if (send_cmd(RDIN3,0) == 0) rd.b8.byte3 = combuff[2];
			     else rd.b8.byte3 = 0xAA;
			     if (send_cmd(RDIN4,0) == 0) rd.b8.byte4 = combuff[2];
			     else rd.b8.byte4 = 0xAA;
			     printf("Scritto %02x%02x%02x%02x ",out.b8.byte4,out.b8.byte3,out.b8.byte2,out.b8.byte1); 
			     printf("Letto %02x%02x%02x%02x ",rd.b8.byte4,rd.b8.byte3,rd.b8.byte2,rd.b8.byte1); 
			     if (out.b32  != rd.b32) 
			     {
				ko=1;
				printf("KO!!!! Invio per proseguire\r"); 
	       			for( i=0; i< WAIT; i++);
			     }
			     else  
			     {
				printf("OK"); 
				ko1=0;
			     }
       		     }	
		     printf("\r\n");	
		}

		/* test degli input Shift degli zeri*/
		printf("\r\nTest degli INPUT Shift degli zeri\r\n");
		for (x = (unsigned long)(0x00000001); x != (unsigned long)(0x40000000); x = x<<1)
		{
		     ko1 = 1;
		     out.b32 = ((~x) & (unsigned long)(0x1FFFFFFF));	
		     while ( (ko1 == 1) && (_getkey() != '\r'))
		     {		
			     if (send_cmd(WRTOUT1,out.b8.byte1) == 0) rd.b8.byte1 = read_port_in1() ;
			     else rd.b8.byte1 = 0x55;
			     if (send_cmd(WRTOUT2,out.b8.byte2) == 0) rd.b8.byte2 = read_port_in2() ;
			     else rd.b8.byte2 = 0x55;
			     if (send_cmd(WRTOUT3,out.b8.byte3) == 0) rd.b8.byte3 = read_port_in3() ;
			     else rd.b8.byte3 = 0x55;
			     if (send_cmd(WRTOUT4,out.b8.byte4) == 0) rd.b8.byte4 = read_port_in4() ;
			     else rd.b8.byte4 = 0x55;
			     printf("Scritto %02x%02x%02x%02x ",out.b8.byte4,out.b8.byte3,out.b8.byte2,out.b8.byte1); 
			     printf("Letto %02x%02x%02x%02x ",rd.b8.byte4,rd.b8.byte3,rd.b8.byte2,rd.b8.byte1); 
			     if (out.b32  != rd.b32) 
			     {
				ko=1;
				printf("KO!!!! Invio per proseguire\r"); 
	       			for( i=0; i< WAIT; i++);
			     }
			     else  
			     {
				printf("OK"); 
				ko1=0;
			     }
       		     }	
		     printf("\r\n");	
		}


		/* test degli output shift 1*/
		printf("\r\nTest degli OUTPUT Shift degli uno\r\n");
		for (out.b32 = (unsigned long)(0x00000001); out.b32 != (unsigned long)(0x20000000); out.b32 = out.b32<<1)
		{
		     ko1 = 1;
		     while ( (ko1 == 1) && (_getkey() != '\r'))
		     {		
			     write_port_out1(out.b8.byte1);
			     write_port_out2(out.b8.byte2);
			     write_port_out3(out.b8.byte3);
			     write_port_out4(out.b8.byte4);
			     if (send_cmd(RDIN1,0) == 0) rd.b8.byte1 = combuff[2];
			     else rd.b8.byte1 = 0xAA;
			     if (send_cmd(RDIN2,0) == 0) rd.b8.byte2 = combuff[2];
			     else rd.b8.byte2 = 0xAA;
			     if (send_cmd(RDIN3,0) == 0) rd.b8.byte3 = combuff[2];
			     else rd.b8.byte3 = 0xAA;
			     if (send_cmd(RDIN4,0) == 0) rd.b8.byte4 = combuff[2];
			     else rd.b8.byte4 = 0xAA;
			     printf("Scritto %02x%02x%02x%02x ",out.b8.byte4,out.b8.byte3,out.b8.byte2,out.b8.byte1); 
			     printf("Letto %02x%02x%02x%02x ",rd.b8.byte4,rd.b8.byte3,rd.b8.byte2,rd.b8.byte1); 
			     if (out.b32  != rd.b32) 
			     {
				ko=1;
				printf("KO!!!! Invio per proseguire\r"); 
	       			for( i=0; i< WAIT; i++);
			     }
			     else  
			     {
				printf("OK"); 
				ko1=0;
			     }
       		     }	
		     printf("\r\n");	
		}

		ko1 = 1;
		out.b32 = (unsigned long)(0x00000000);	
		while ( (ko1 == 1) && (_getkey() != '\r'))
		{		
		     write_port_out1(out.b8.byte1);
		     write_port_out2(out.b8.byte2);
		     write_port_out3(out.b8.byte3);
		     write_port_out4(out.b8.byte4);
		     if (send_cmd(RDIN1,0) == 0) rd.b8.byte1 = combuff[2];
		     else rd.b8.byte1 = 0xAA;
		     if (send_cmd(RDIN2,0) == 0) rd.b8.byte2 = combuff[2];
		     else rd.b8.byte2 = 0xAA;
		     if (send_cmd(RDIN3,0) == 0) rd.b8.byte3 = combuff[2];
		     else rd.b8.byte3 = 0xAA;
		     if (send_cmd(RDIN4,0) == 0) rd.b8.byte4 = combuff[2];
		     else rd.b8.byte4 = 0xAA;
		     printf("Scritto %02x%02x%02x%02x ",out.b8.byte4,out.b8.byte3,out.b8.byte2,out.b8.byte1); 
		     printf("Letto %02x%02x%02x%02x ",rd.b8.byte4,rd.b8.byte3,rd.b8.byte2,rd.b8.byte1); 
		     if (out.b32  != rd.b32) 
		     {
			ko=1;
			printf("KO!!!! Invio per proseguire\r"); 
	       		for( i=0; i< WAIT; i++);
		     }
		     else  
		     {
			printf("OK"); 
			ko1=0;
		     }
       		}	
		printf("\r\n");	

		/* test degli input Shift degli uno*/
		printf("\r\nTest degli INPUT Shift degli uno\r\n");
		for (out.b32 = (unsigned long)(0x00000001); out.b32 != (unsigned long)(0x20000000); out.b32 = out.b32<<1)
		{
		     ko1 = 1;
		     while ( (ko1 == 1) && (_getkey() != '\r'))
		     {		
			     if (send_cmd(WRTOUT1,out.b8.byte1) == 0) rd.b8.byte1 = read_port_in1() ;
			     else rd.b8.byte1 = 0x55;
			     if (send_cmd(WRTOUT2,out.b8.byte2) == 0) rd.b8.byte2 = read_port_in2() ;
			     else rd.b8.byte2 = 0x55;
			     if (send_cmd(WRTOUT3,out.b8.byte3) == 0) rd.b8.byte3 = read_port_in3() ;
			     else rd.b8.byte3 = 0x55;
			     if (send_cmd(WRTOUT4,out.b8.byte4) == 0) rd.b8.byte4 = read_port_in4() ;
			     else rd.b8.byte4 = 0x55;
			     printf("Scritto %02x%02x%02x%02x ",out.b8.byte4,out.b8.byte3,out.b8.byte2,out.b8.byte1); 
			     printf("Letto %02x%02x%02x%02x ",rd.b8.byte4,rd.b8.byte3,rd.b8.byte2,rd.b8.byte1); 
			     if (out.b32  != rd.b32) 
			     {
				ko=1;
				printf("KO!!!! Invio per proseguire\r"); 
	       			for( i=0; i< WAIT; i++);
			     }
			     else  
			     {
				printf("OK"); 
				ko1=0;
			     }
       		     }	
		     printf("\r\n");	
		}

	        ko1 = 1;
		out.b32 = (unsigned long)(0x00000000);	
		while ( (ko1 == 1) && (_getkey() != '\r'))
		{		
			if (send_cmd(WRTOUT1,out.b8.byte1) == 0) rd.b8.byte1 = read_port_in1() ;
			else rd.b8.byte1 = 0x55;
			if (send_cmd(WRTOUT2,out.b8.byte2) == 0) rd.b8.byte2 = read_port_in2() ;
			else rd.b8.byte2 = 0x55;
			if (send_cmd(WRTOUT3,out.b8.byte3) == 0) rd.b8.byte3 = read_port_in3() ;
			else rd.b8.byte3 = 0x55;
			if (send_cmd(WRTOUT4,out.b8.byte4) == 0) rd.b8.byte4 = read_port_in4() ;
			else rd.b8.byte4 = 0x55;
			printf("Scritto %02x%02x%02x%02x ",out.b8.byte4,out.b8.byte3,out.b8.byte2,out.b8.byte1); 
			printf("Letto %02x%02x%02x%02x ",rd.b8.byte4,rd.b8.byte3,rd.b8.byte2,rd.b8.byte1); 
			if (out.b32  != rd.b32) 
			{
				ko=1;
				printf("KO!!!! Invio per proseguire\r"); 
	       			for( i=0; i< WAIT; i++);
			}
			else  
			{
				printf("OK"); 
				ko1=0;
			}
       		}	
		printf("\r\n");	

                printf ("Verificare che lampeggi il led1 di AUXDEW (premere invio per proseguire)");
		
		P6 &= 0xFFDF;
		while ( _getkey() != '\r')
		{
			if( (P6 & 0x0020) == 0 ) P6 |= 0x0020 ;
			else P6 &= 0xFFDF;
			for (x = 0; x < (unsigned long)(350000); x++);
		}
		printf("\r\n");

		if (ko) save_stato(TPIOKO);
    		else save_stato(TPIOOK);

        }
        else printf("Impossibile eseguire il test per mancato collegamento con STIM\r\n");
	

}
