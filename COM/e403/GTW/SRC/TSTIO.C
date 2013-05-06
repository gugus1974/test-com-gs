/*********************************************************************************
* 20/03/13                                             v.1.00        tstio.c *
**********************************************************************************
* Funzioni per il test della EEPROM                                              *
**********************************************************************************
* 					                                                             *
*********************************************************************************/
#include <stdio.h>
#include <string.h>
#include "tstio.h"
#include "hw_com.h"
#include "c_utils.h"
#include "strParse.h"
#include "tstflash.h"
#include "COD_ST.h"


extern volatile QUICC	*quicc;				/* pointer to the QUICC DP-RAM structure */
volatile QUICC 	*q;
 	
    unsigned short   lettura_di[4];
    unsigned long   comandi_do[2];

short	err[55]; 	/* channel test error code       */
char	ts[12];	/* temporary string */

void printbits(long p1,int n, char v0, char v1);


static const CMessageDef out1[49]   = {
    0, "OUTD1", 
    1, "OUTD2",
    2, "OUTD3",
    3, "OUTD4",
    4, "OUTD5",
    5, "OUTD6",
    6, "OUTD7",   
    7, "OUTD8",
    8, "OUTD9", 
    9, "OUTD10",
   10, "OUTD11",
   11, "OUTD12",
   12, "DTR1M",
   13, "RTS4S",
   14, "KMB",   
   15, "KMA",
   16, "RISB", 
   17, "RISA",
   18, "KFB",
   19, "KFA",
   20, "K4B",
   21, "K4A",
   22, "K3B",   
   23, "K3A",
   24, "K1B",
   25, "K1A",
   26, "RESETB", 
   27, "RESETA", 
   28, "SEL2A/B", 
   29, "SEL1A/B", 
   30, "PS1B", 
   31, "PS1A",    
   32, "OUTD13",
   33, "OUTD14",
   34, "OUTD15", 
   35, "OUTD16", 
   36, "OUTD17", 
   37, "OUTD18", 
   38, "OUTD19", 
   39, "OUTD20",    
   40, "LED6B",
   41, "LED6A",
   42, "LED5B",
   43, "LED5A",
   44, "LED4B", 
   45, "LED4A", 
   46, "spare",
   47, "SLEEPM", 
  255,       0};
static const CMessageDef in[33]   = {
    1, "IND1",
    2, "IND2",
    3, "IND3",
    4, "IND4",
    5, "IND5",
    6, "IND6",
    7, "IND7",
    8, "IND8",
    9, "IND9",
   10, "IND10",
   11, "IND11",
   12, "IND12",
   13, "IND13",
   14, "IND14",
   15, "IND15",
   16, "IND16",
   17, "IND17",
   18, "IND18",
   19, "IND19",
   20, "IND20",
   21, "IND21",
   22, "IND22",
   23, "IND23",
   24, "DSR1M",   
   25, "RI1M",
   26, "HWFLTB",
   27, "HWFLTA",
   28, "SHD",
   29, "AR2",
   30, "AR3",
   31, "AR4",
   32, "AR5",
    0 , 0};


static const CMessageDef testio[55]   = {
    1, "1* OUTD1 - (17bJ2) IND1 - (16bJ2) ",                                 
    2, "2 * OUTD2 - (18bJ2) IND2 - (15bJ2)",                                 
    3, "3 * OUTD3 - (19bJ2) IND3 - (14bJ2)",                                 
    4, "4 * OUTD4 - (20bJ2) IND4 - (13bJ2)",                                 
    5, "5 * OUTD5 - (21bJ2) IND5 - (12bJ2)",                                 
    6, "6 * OUTD6 - (22bJ2) IND6 - (11bJ2)",                                 
    7, "7 * OUTD7 - (23bJ2) IND7 - (10bJ2)",                                 
    8, "8 * OUTD8 - (24bJ2) IND8 - (9bJ2) ",                                 
    9, "9 * OUTD9 - (25bJ2) IND9 - (8bJ2) ",                                 
   10, "10 * OUTD10 - (26bJ2) IND10 - (7bJ2)",                               
   11, "11 * OUTD11 - (27bJ2) IND11 - (6bJ2)",                               
   12, "12 * OUTD12 - (28bJ2) IND12 - (5bJ2)",                               
   13, "13 * OUTD13 - (29bJ2) IND13 - (4bJ2)",                               
   14, "14 * OUTD14 - (30bJ2) IND14 - (3bJ2)",                               
   15, "15 * OUTD15 - (31bJ2) IND15 - (2bJ2)",                               
   16, "16 * OUTD16 - (31cJ2) IND16 - (1bJ2)",                               
   17, "17 * OUTD17 - (30cJ2) IND17 - (31aJ2)",                              
   18, "18 * SLEEPM - (24cJ2) IND18 - (30aJ2)",                              
   19, "19 * RISA - (15aJ2) IND19 - (29aJ2)",                                
   20, "20 * K3A - (16aJ2) IND20 - (28aJ2)",                                 
   21, "21 * K4A - (17aJ2) IND21 - (27aJ2)",                                 
   22, "22 * KFA - (18aJ2) IND22 - (26aJ2)",                                 
   23, "23 * K1A - (14aJ2) HWFLTA - (13aJ2)",                                
   24, "24 * K1B - (14cJ2) HWFLTB - (13cJ2)",                                 
   25, "25 * RISB - (15cJ2) IND23 - (3SK9)",                                 
   26, "26 * RESETA - (12aJ2) CS1 [PC9] - (7aJ2)",                           
   27, "27 * PS1A - (11aJ2) TLD1 [PC2] - (8aJ2)",                            
   28, "28 * SEL1A/B - (10aJ2) OLD1 [PC0] - (9aJ2)",                         
   29, "29 * RTS4SB - (21aJ2) CLK4SB [PA11] - (22aJ2)",                      
   30, "30 * SEL2A/B - (10cJ2) OLD2 [PC3] - (9cJ2)",                         
   31, "31 * PS1B - (11cJ2) TLD2 [PC1] - (8cJ2)   ",                         
   32, "32 * RESETB - (12cJ2) CS2 [PC11] - (7cJ2) ",                         
   33, "33 * K3B - (16cJ2) DREQ2 [PB8] - (29cJ2)  ",                         
   34, "34 * K4B - (17bJ2) DREQ1 [PB4] - (28cJ2)  ",                         
   35, "35 * KFB - (18cJ2) TIN2 [PA10] - (27cJ2)  ",                         
   36, "36 * TX1D [PA5] - (1aJ2) RX1C [PA12] - (6aJ2)",                      
   37, "37 * RTS1 [PB14] - (3aJ2) RX1D [PA4] - (2aJ2)",                      
   38, "38 * CTS1 [PC8] - (4aJ2)",                                             
   39, "39 * TX4SB [PB10] - (20aJ2) RX4SB [PB11] - (20aJ2) ",                
   40, "40 * BRG2SB [PB5] - (23cJ2) TIN1 [PA8] - (26cJ2)   ",                
   41, "41 * TX2SB [PA3] - (19cJ2) CLK2SB [PA9] - (22cJ2)  ",                
   42, "42 * BRG4SB [PB16] - (23aJ2) SYN4SB [PB9] - (24aJ2)",                
   43, "43 * TX2D [PA7] - (1cJ2) RX2C [PA14] - (6cJ2)      ",                
   44, "44 * RTS2 [PB15] - (3cJ2) RX2D [PA6] - (2cJ2)      ",                
   45, "45 CTS2 [PC10] - (4cJ2)",                
   46, "46 * RTS2SB [PB13] - (21cJ2) RX2SB [PA2] - (20cJ2) ",                
   47, "47 Request to send [PB12] - (7CM1) # Clear to send [PC4] - (8CM1)",
   48, "48 Transmit data [PA1] - (3CM1) # Receive data [PA0] - (2CM1)    ",
   49, "49 OUTD18232 - (1SK9) Carrier detect [PC5] - (1CM1) #            ", 
   50, "50 OUTD19232 - (2SK9) Data set ready DSR1M - (6CM1) #            ", 
   51, "51 Data term ready DTR1M - (4CM1) # Ring indicator RI1M - (9CM1) ", 
   52, "52 [PB6] (1SK8) [PB7] (2SK8)",                                   
   53, "53 * TX1C [PA13] - (5aJ2)       ",                                   
   54, "54 * TX2C [PA15] - (5cJ2)       ",                                   
    0 , 0};


char ts[12];
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void printbits(long p1,int n, char v0, char v1)
{
    int i;
    for (i = 0; i < n; i++)
    {
//        if (p2 && c_get_bit(p2, i) && c_get_bit(p1, i))
//        {
//            putchar(v3);
//        }
//        else if (p2 && c_get_bit(p2, i))
//        {
//            putchar(v2);
//        }
//        else 
        if (p1 & (1<<i))
        {
            putchar(v1);
        }
        else
        {
            putchar(v0);
        }
    }
}



/*la procedura satta val sul segnale sig-esimo della tabella dei comandi di uscita*/
void tstio_setDO(unsigned char sig, unsigned char v)
{
//    printf("setDO %d %d\n",sig,v);
    switch(sig)
    {
     case 1 ://outd1 
     case 2 ://outd2 
     case 3 ://outd3 
     case 4 ://outd4 
     case 5 ://outd5 
     case 6 ://outd6 
     case 7 ://outd7 
     case 8 ://outd8 
     case 9 ://outd9 
     case 10://outd10
     case 11://outd11
     case 12://outd12
	        if (v)  set_out_port(0,(1<<(sig-1)),(1<<(sig-1)));    
	        else    set_out_port(0,(1<<(sig-1)), 0          );    
        break;   
     case 13://outd13
     case 14:// ..
     case 15:// ..
     case 16:// ..
     case 17://outd17
	        if (v)  set_out_port(1,(1<<(sig-13)),(1<<(sig-13)));    
	        else    set_out_port(1,(1<<(sig-13)), 0          );    
        break;   
     case 18://SLEEPM
	        if (v)  set_out_port(1,(1<<15),(1<<15));    
	        else    set_out_port(1,(1<<15), 0);    
        break;   
     case 19://RISA
	        if (v)  set_out_port(0,(1<<17),(1<<17));    
	        else    set_out_port(0,(1<<17), 0);    
        break;   
     case 20://K3A
	        if (v)  set_out_port(0,(1<<23),(1<<23));    
	        else    set_out_port(0,(1<<23), 0);    
        break;   
     case 21://K4A
	        if (v)  set_out_port(0,(1<<21),(1<<21));    
	        else    set_out_port(0,(1<<21), 0);    
        break;   
     case 22://KFA
	        if (v)  set_out_port(0,(1<<19),(1<<19));    
	        else    set_out_port(0,(1<<19), 0);    
        break;   
     case 23://K1A
	        if (v)  set_out_port(0,(1<<25),(1<<25));
	        else    set_out_port(0,(1<<25), 0);
        break;   
     case 24://K1B
	        if (v)  set_out_port(0,(1<<24),(1<<24));
	        else    set_out_port(0,(1<<24), 0);
        break;   
     case 25://RISB
	        if (v)  set_out_port(0,(1<<16),(1<<16));
	        else    set_out_port(0,(1<<16), 0);
        break;   
     case 26://RESETA
	        if (v)  set_out_port(0,(1<<27),(1<<27));
	        else    set_out_port(0,(1<<27), 0);
        break;   
     case 27://PS1A
	        if (v)  set_out_port(0,(1<<31),(1<<31));
	        else    set_out_port(0,(1<<31), 0);
        break;   
     case 28://SEL1A/B
	        if (v)  set_out_port(0,(1<<29),(1<<29));
	        else    set_out_port(0,(1<<29), 0);
        break;   
     case 29://RTS4S
	        if (v)  set_out_port(0,(1<<13),(1<<13));
	        else    set_out_port(0,(1<<13), 0);
        break;   
     case 30://SEL2A/B
	        if (v)  set_out_port(0,(1<<28),(1<<28));
	        else    set_out_port(0,(1<<28), 0);
        break;   
     case 31://PS1B
	        if (v)  set_out_port(0,(1<<30),(1<<30));
	        else    set_out_port(0,(1<<30), 0);
        break;   
     case 32://RESETB
	        if (v)  set_out_port(0,(1<<26),(1<<26));
	        else    set_out_port(0,(1<<26), 0);
        break;   
     case 33://K3B
	        if (v)  set_out_port(0,(1<<22),(1<<22));
	        else    set_out_port(0,(1<<22), 0);
        break;      
     case 34://K4B
	        if (v)  set_out_port(0,(1<<20),(1<<20));
	        else    set_out_port(0,(1<<20), 0);
        break;   
     case 35://KFB
	        if (v)  set_out_port(0,(1<<18),(1<<18));
	        else    set_out_port(0,(1<<18), 0);
        break;   
     case 36://TX1D [PA5] - (1aJ2)
	        if (v)  q->pio_padat |= (1<<5);
	        else    q->pio_padat &= ~(1<<5);
        break;   
     case 37://RTS1 [PB14] - (3aJ2)
	        if (v)  q->pip_pbdat |= (1<<14);
	        else    q->pip_pbdat &= ~(1<<14);
        break;   
     case 38://
        break;   
     case 39://TX4SB [PB10] - (20aJ2)
	        if (v)  q->pip_pbdat |= (1<<10);
	        else    q->pip_pbdat &= ~(1<<10);
        break;   
     case 40://BRG2SB [PB5] - (23cJ2)
	        if (v)  q->pip_pbdat |= (1<<5);
	        else    q->pip_pbdat &= ~(1<<5);
        break;   
     case 41://TX2SB [PA3] - (19cJ2)
	        if (v)  q->pio_padat |= (1<<3);
	        else    q->pio_padat &= ~(1<<3);
        break;   
     case 42://BRG4SB [PB16] - (23aJ2)
	        if (v)  q->pip_pbdat |= (1<<16);
	        else    q->pip_pbdat &= ~(1<<16);
        break;   
     case 43://TX2D [PA7] - (1cJ2)
	        if (v)  q->pio_padat |= (1<<7);
	        else    q->pio_padat &= ~(1<<7);
        break;   
     case 44://RTS2 [PB15] - (3cJ2)
	        if (v)  q->pip_pbdat |= (1<<15);
	        else    q->pip_pbdat &= ~(1<<15);
        break;   
     case 45://
        break;   
     case 46://RTS2SB [PB13] - (21cJ2) 
	        if (v)  q->pip_pbdat |= (1<<13);
	        else    q->pip_pbdat &= ~(1<<13);
        break;   
     case 47://Request to send [PB12](7CM1) 
	        if (v)  q->pip_pbdat |= (1<<12);
	        else    q->pip_pbdat &= ~(1<<12);
        break;   
     case 48://Transmit data [PA1] - (3CM1) 
	        if (v)  q->pio_padat |= (1<<1);
	        else    q->pio_padat &= ~(1<<1);
        break;   
     case 49://OUTD18232 - (1SK9) 
	        if (v)  set_out_port(1,(1<<5),(1<<5));
	        else    set_out_port(1,(1<<5), 0);
        break;   
     case 50://OUTD19232 - (2SK9) 
	        if (v)  set_out_port(1,(1<<6),(1<<6));
	        else    set_out_port(1,(1<<6), 0);
        break;   
     case 51://Data term ready DTR1M - (4CM1)
	        if (v)  set_out_port(0,(1<<12),(1<<12));
	        else    set_out_port(0,(1<<12), 0);
        break;   
     case 52://[PB6]  (1SK8)
	        if (v)  q->pip_pbdat |= (1<<6);
	        else    q->pip_pbdat &= ~(1<<6);
        break;   
     case 53://TX1C [PA13] - (5aJ2)
	        if (v)  q->pio_padat |= (1<<13);
	        else    q->pio_padat &= ~(1<<13);
        break;   
     case 54:
        break;   
        
    }


}
void clearDO()
{
    unsigned int sig;
    
    for(sig=1;sig<55;sig++)
        tstio_setDO(sig,0);

}

//void tstio_getDI()
//{
//    unsigned int i,j;    
//    lettura_di[0]= 0;
//    /* */
//    /*bit 0..21 - IND1..IND22*/    
//    for (i=0;i<22;i++)   lettura_di[0] |= (get_in_port(0) & (1<<i));
//    /*bit 22 - HWFLTB*/
//    lettura_di[0] |= ((get_in_port(0) & (1<<25))!=0)<<22;
//    /*bit 23 - HWFLTA*/
//    lettura_di[0] |= ((get_in_port(0) & (1<<26))!=0)<<23;
//    /*bit 24 - IND23*/
//    lettura_di[0] |= ((get_in_port(0) & (1<<22))!=0)<<24;
//    /*bit 25 - CS1*/
//    lettura_di[0] |= ((q->pio_pcdat & (1<<9))!=0)   <<25;
//    /*bit 26 - TLD1*/
//    lettura_di[0] |= ((q->pio_pcdat & (1<<2))!=0)   <<26;
//    /*bit 27 - OLD1*/
//    lettura_di[0] |= ((q->pio_pcdat & (1<<0))!=0)   <<27;
//    /*bit 28 - CLK4SB*/
//    lettura_di[0] |= ((q->pio_padat & (1<<11))!=0)  <<28;
//    /*bit 29 - OLD2*/
//    lettura_di[0] |= ((q->pio_pcdat & (1<<3))!=0)   <<29;
//    /*bit 30 - TLD2*/
//    lettura_di[0] |= ((q->pio_pcdat & (1<<1))!=0)   <<30;
//    /*bit 31 - CS2*/
//    lettura_di[0] |= ((q->pio_pcdat & (1<<11))!=0)  <<31;
//
//    lettura_di[1] = 0;
//    /*bit0  - DREQ2*/
//    lettura_di[1] |= ((q->pip_pbdat & (1<<8))!=0)   <<0;
//    
//    /*bit 1 - DREQ1*/
//    lettura_di[1] |= ((q->pip_pbdat & (1<<4))!=0)   <<1;
//    /*bit 2 - TIN2*/
//    lettura_di[1] |= ((q->pio_padat & (1<<10))!=0)  <<2;
//    /*bit 3 - RX1C*/
//    lettura_di[1] |= ((q->pio_padat & (1<<12))!=0)  <<3;
//    /*bit 4 - RX1D*/
//    lettura_di[1] |= ((q->pio_padat & (1<<4))!=0)   <<4;    
//    /*bit 5 - CTS1*/
//    lettura_di[1] |= ((q->pio_pcdat & (1<<8))!=0)   <<5;
//    /*bit 6 - RX4SB*/
//    lettura_di[1] |= ((q->pip_pbdat & (1<<11))!=0)  <<6;
//    /*bit 7 - TIN1*/
//    lettura_di[1] |= ((q->pio_padat & (1<<8))!=0)   <<7;
//    /*bit 8 - CLK2SB*/
//    lettura_di[1] |= ((q->pio_padat & (1<<9))!=0)   <<8;
//    /*bit 9 - SYN4SB*/
//    lettura_di[1] |= ((q->pip_pbdat & (1<<9))!=0)   <<9;
//    /*bit 10 - RX2C*/
//    lettura_di[1] |= ((q->pio_padat & (1<<14))!=0)  <<10;
//    /*bit 11 - RX2D*/
//    lettura_di[1] |= ((q->pio_padat & (1<<6))!=0)   <<11;
//    /*bit 12 - CTS2*/
//    lettura_di[1] |= ((q->pio_pcdat & (1<<10))!=0)  <<12;
//    /*bit 13 - RX2SB*/
//    lettura_di[1] |= ((q->pio_padat & (1<<2))!=0)   <<13;
//    /*bit 14 - Clear to send 8CM1*/
//    lettura_di[1] |= ((q->pio_pcdat & (1<<4))!=0)   <<14;
//    /*bit 15 - RXD 2CM1 */
//    lettura_di[1] |= ((q->pio_padat & (1<<0))!=0)   <<15;
//    /*bit 16 - CD 1CM1*/
//    lettura_di[1] |= ((q->pio_pcdat & (1<<5))!=0)   <<16;
//    /*bit 17 - DSR1 6CM1 (IND24)*/
//    lettura_di[1] |= ((get_in_port(0) & (1<<23))!=0) <<17;
//    /*bit 18 - RI1M 9CM1)*/
//    lettura_di[1] |= ((get_in_port(0) & (1<<24))!=0) <<18;
//    /*bit 19 - PB7*/
//    lettura_di[1] |= ((q->pip_pbdat & (1<<7))!=0)    <<19;
//    /*bit 20 - TX1C*/
//    lettura_di[1] |= ((q->pio_padat & (1<<13))!=0)   <<20;
//    /*bit 21 - TX2C*/
//    lettura_di[1] |= ((q->pio_padat & (1<<15))!=0)   <<21;
//}

void tstio_getDI()
{
    unsigned int i,j;    
    lettura_di[0]= 0;
    /* */
    /*riga 1..16 - IND1..IND16*/    
    for (i=0;i<=15;i++)   lettura_di[0] |= (get_in_port(0) & (1<<i));

    lettura_di[1] = 0;
    /*17 - IND17*/    
    lettura_di[1] |= ((get_in_port(0) & (1<<16))!=0)<<0;
    /*18- IND18*/    
    lettura_di[1] |= ((get_in_port(0) & (1<<17))!=0)<<1;
    /*19- IND19*/    
    lettura_di[1] |= ((get_in_port(0) & (1<<18))!=0)<<2;
    /*20- IND20*/    
    lettura_di[1] |= ((get_in_port(0) & (1<<19))!=0)<<3;
    /*21- IND21*/    
    lettura_di[1] |= ((get_in_port(0) & (1<<20))!=0)<<4;
    /*22- IND22*/    
    lettura_di[1] |= ((get_in_port(0) & (1<<21))!=0)<<5;
    /*23- HWFLTA*/
    lettura_di[1] |= ((get_in_port(0) & (1<<26))!=0)<<6;
    /*24- HWFLTB*/
    lettura_di[1] |= ((get_in_port(0) & (1<<25))!=0)<<7;
    /*25- IND23*/
    lettura_di[1] |= ((get_in_port(0) & (1<<22))!=0)<<8;
    /*26- CS1*/
    lettura_di[1] |= ((q->pio_pcdat & (1<<9))!=0)   <<9;
    /*27- TLD1*/
    lettura_di[1] |= ((q->pio_pcdat & (1<<2))!=0)   <<10;
    /*28- OLD1*/
    lettura_di[1] |= ((q->pio_pcdat & (1<<0))!=0)   <<11;
    /*29- CLK4SB*/
    lettura_di[1] |= ((q->pio_padat & (1<<11))!=0)  <<12;
    /*30- OLD2*/
    lettura_di[1] |= ((q->pio_pcdat & (1<<3))!=0)   <<13;
    /*31- TLD2*/
    lettura_di[1] |= ((q->pio_pcdat & (1<<1))!=0)   <<14;
    /*32- CS2*/
    lettura_di[1] |= ((q->pio_pcdat & (1<<11))!=0)  <<15;

    lettura_di[2] = 0;
    /*33- DREQ2*/
    lettura_di[2] |= ((q->pip_pbdat & (1<<8))!=0)   <<0;
    /*34- DREQ1*/
    lettura_di[2] |= ((q->pip_pbdat & (1<<4))!=0)   <<1;
    /*35- TIN2*/
    lettura_di[2] |= ((q->pio_padat & (1<<10))!=0)  <<2;
    /*36- RX1C*/
    lettura_di[2] |= ((q->pio_padat & (1<<12))!=0)  <<3;
    /*37- RX1D*/
    lettura_di[2] |= ((q->pio_padat & (1<<4))!=0)   <<4;    
    /*38 bit 5 - CTS1*/
    lettura_di[2] |= ((q->pio_pcdat & (1<<8))!=0)   <<5;
    /*39 bit 6 - RX4SB*/
    lettura_di[2] |= ((q->pip_pbdat & (1<<11))!=0)  <<6;
    /*40 bit 7 - TIN1*/
    lettura_di[2] |= ((q->pio_padat & (1<<8))!=0)   <<7;
    /*41 bit 8 - CLK2SB*/
    lettura_di[2] |= ((q->pio_padat & (1<<9))!=0)   <<8;
    /*42 bit 9 - SYN4SB*/
    lettura_di[2] |= ((q->pip_pbdat & (1<<9))!=0)   <<9;
    /*43 bit 10 - RX2C*/
    lettura_di[2] |= ((q->pio_padat & (1<<14))!=0)  <<10;
    /*44 bit 11 - RX2D*/
    lettura_di[2] |= ((q->pio_padat & (1<<6))!=0)   <<11;
    /*45 bit 12 - CTS2*/
    lettura_di[2] |= ((q->pio_pcdat & (1<<10))!=0)  <<12;
    /*46 bit 13 - RX2SB*/
    lettura_di[2] |= ((q->pio_padat & (1<<2))!=0)   <<13;
    /*47 bit 14 - Clear to send 8CM1*/
    lettura_di[2] |= ((q->pio_pcdat & (1<<4))!=0)   <<14;
    /*48 bit 15 - RXD 2CM1 */
//    lettura_di[2] |= ((q->pio_padat & (1<<0))!=0)   <<15;

    lettura_di[3] = 0;
    /*49 bit 0 - CD 1CM1*/
    lettura_di[3] |= ((q->pio_pcdat & (1<<5))!=0)    <<0;
    /*50 bit 1 - DSR1 6CM1 (IND24)*/
    lettura_di[3] |= ((get_in_port(0) & (1<<23))!=0) <<1;
    /*51 bit 2 - RI1M 9CM1)*/
    lettura_di[3] |= ((get_in_port(0) & (1<<24))!=0) <<2;
    /*52 bit 3 - PB7*/
    lettura_di[3] |= ((q->pip_pbdat & (1<<7))!=0)    <<3;
    /*53 bit 4 - TX1C*/
    lettura_di[3] |= ((q->pio_padat & (1<<13))!=0)   <<4;
    /*54 bit 5 - TX2C*/
    lettura_di[3] |= ((q->pio_padat & (1<<15))!=0)   <<5;
    lettura_di[4] = 0;
}

short tstio_led(short argc, char *argv[])
{
    unsigned int i,j;    
    char c;
    
    
    printf("Verificare accensione dei led secondo la segquenza 4A-4B-5A-5B-6A-6B\n");
    set_out_port(1,DOP1_LD6b_,DOP1_LD6b_);
    set_out_port(1,DOP1_LD6a_,DOP1_LD6a_);
    set_out_port(1,DOP1_LD5b_,DOP1_LD5b_);
    set_out_port(1,DOP1_LD5a_,DOP1_LD5a_);
    set_out_port(1,DOP1_LD4b_,DOP1_LD4b_);
    set_out_port(1,DOP1_LD4a_,DOP1_LD4a_);
    
    for(i=0;i<500000;i++);
    set_out_port(1,DOP1_LD4a_,0);
    for(i=0;i<500000;i++);
    set_out_port(1,DOP1_LD4a_,DOP1_LD4a_);
    for(i=0;i<500000;i++);
    set_out_port(1,DOP1_LD4b_,0);
    for(i=0;i<500000;i++);
    set_out_port(1,DOP1_LD4b_,DOP1_LD4b_);
    for(i=0;i<500000;i++);
    set_out_port(1,DOP1_LD5a_,0);
    for(i=0;i<500000;i++);
    set_out_port(1,DOP1_LD5a_,DOP1_LD5a_);
    for(i=0;i<500000;i++);
    set_out_port(1,DOP1_LD5b_,0);
    for(i=0;i<500000;i++);
    set_out_port(1,DOP1_LD5b_,DOP1_LD5b_);
    for(i=0;i<500000;i++);
    set_out_port(1,DOP1_LD6a_,0);
    for(i=0;i<500000;i++);
    set_out_port(1,DOP1_LD6a_,DOP1_LD6a_);
    for(i=0;i<500000;i++);
    set_out_port(1,DOP1_LD6b_,0);
    for(i=0;i<500000;i++);
    set_out_port(1,DOP1_LD6b_,DOP1_LD6b_);
    for(i=0;i<500000;i++);
    for(i=0;i<500000;i++);
    
    set_out_port(1,DOP1_LD6b_,0);
    set_out_port(1,DOP1_LD6a_,0);
    set_out_port(1,DOP1_LD5b_,0);
    set_out_port(1,DOP1_LD5a_,0);
    set_out_port(1,DOP1_LD4b_,0);
    set_out_port(1,DOP1_LD4a_,0);
    for(i=0;i<500000;i++);
    set_out_port(1,DOP1_LD6b_,DOP1_LD6b_);
    set_out_port(1,DOP1_LD6a_,DOP1_LD6a_);
    set_out_port(1,DOP1_LD5b_,DOP1_LD5b_);
    set_out_port(1,DOP1_LD5a_,DOP1_LD5a_);
    set_out_port(1,DOP1_LD4b_,DOP1_LD4b_);
    set_out_port(1,DOP1_LD4a_,DOP1_LD4a_);
   
    for(i=0;i<500000;i++);
    set_out_port(1,DOP1_LD6b_,0);
    set_out_port(1,DOP1_LD6a_,0);
    set_out_port(1,DOP1_LD5b_,0);
    set_out_port(1,DOP1_LD5a_,0);
    set_out_port(1,DOP1_LD4b_,0);
    set_out_port(1,DOP1_LD4a_,0);
    for(i=0;i<500000;i++);
    set_out_port(1,DOP1_LD6b_,DOP1_LD6b_);
    set_out_port(1,DOP1_LD6a_,DOP1_LD6a_);
    set_out_port(1,DOP1_LD5b_,DOP1_LD5b_);
    set_out_port(1,DOP1_LD5a_,DOP1_LD5a_);
    set_out_port(1,DOP1_LD4b_,DOP1_LD4b_);
    set_out_port(1,DOP1_LD4a_,DOP1_LD4a_);
   
    for(i=0;i<500000;i++);
    set_out_port(1,DOP1_LD6b_,0);
    set_out_port(1,DOP1_LD6a_,0);
    set_out_port(1,DOP1_LD5b_,0);
    set_out_port(1,DOP1_LD5a_,0);
    set_out_port(1,DOP1_LD4b_,0);
    set_out_port(1,DOP1_LD4a_,0);
    for(i=0;i<500000;i++);
    set_out_port(1,DOP1_LD6b_,DOP1_LD6b_);
    set_out_port(1,DOP1_LD6a_,DOP1_LD6a_);
    set_out_port(1,DOP1_LD5b_,DOP1_LD5b_);
    set_out_port(1,DOP1_LD5a_,DOP1_LD5a_);
    set_out_port(1,DOP1_LD4b_,DOP1_LD4b_);
    set_out_port(1,DOP1_LD4a_,DOP1_LD4a_);
   
    printf("Premere 1 se la sequenza di accensione è corretta altrimenti premere 0\n");
	        
    while (((c=sio_poll_key(1))!= '0') && ((c=sio_poll_key(1))!= '1') ){}
    
    if (c == '1') save_stato(TESTLED_OK);
    else          save_stato(TESTLED_KO);

}





/* di shell command*/
short di_cmd(short argc, char *argv[])
{
        unsigned long input;
        unsigned char i;
       	q=quicc;

    	argv = argv;
        input = get_in_port(0);
        printf("Digital Inputs (%X)\n",input);
        for (i=0;i<32;i++)
            printf("IN%2d -[%s] -> %d\n",i+1,c_fetch_message(in, (i+1), ts, 0),((get_in_port(0) & (1<<i)) !=0));

        printf("portA data %08x\n",q->pio_padat);
        printf("portB data %04x\n",q->pip_pbdat);
        printf("portC data %04x\n",q->pio_pcdat);
       return 0;
}
/* di shell command*/
short do_cmd(short argc, char *argv[])
{
    unsigned long output[2];
    unsigned char i;
    unsigned char ch;
    unsigned char ch33_48;
    unsigned char v;
   
	if (argc== 3){
	    ch = parse_number(argv[1]);
        v = parse_number(argv[2]);
        
        if (ch >47) return -1;
        else if(ch<32){
            
	        if (v)  set_out_port(0,(1<<ch),(1<<ch));    
	        else    set_out_port(0,(1<<ch),0);    
            printf("forced OUT%d -[ %s ] -> %d\n",ch,c_fetch_message(out1, ch, ts, 0) ,((get_out_port(0) & (1<<ch))!=0));
        
	    }
        else if(ch<48){
            ch33_48 = ch-32;
	        if (v)  set_out_port(1,(1<<ch33_48),(1<<ch33_48));    
            else    set_out_port(1,(1<<ch33_48),0);
            printf("forced OUT%d -[%s] -> %d\n",ch,c_fetch_message(out1, ch, ts, 0) ,((get_out_port(1) & (1<<ch33_48))!=0));
	        
	    }
	
	}
   	else if (argc == 1){
    	argv = argv;
        output[0] = get_out_port(0);
        output[1] = get_out_port(1);
//        printf("Digital Outputs\n");
//        for (i=0;i<32;i++)           
//            printf("OUT%d -[%-7s] -> %-7d\n",i,c_fetch_message(out1, i, ts, 0) ,((get_out_port(0) & (1<<i))!=0));        
//        ch33_48 = 0;
//        for (i=32;i<48;i++,ch33_48++)
//            printf("OUT%d -[%-7s] -> %-7d\n",i,c_fetch_message(out1, i, ts, 0),((get_out_port(1) & (1<<ch33_48))!=0));
    }

    return 0;
}


short tstio(short argc, char *argv[] )
{
    static  unsigned short  lettura_di_prec[4];
    static  unsigned short  result[4];

    static  unsigned char   posiz;
    
    unsigned int i,j,k;    
    unsigned short  bkpA_C[4];
    unsigned long   bkpB[2];
    static unsigned int test_ret=0; 
 	char c;
 	q=quicc;
  
  /*
  PA0 PA1 PA2 PA3 PA4 PA5 PA6 PA7 PA8 PA9 PA10 PA11 PA12 PA13 PA14 PA15
   IN OUT IN  OUT IN  OUT IN  OUT  IN  IN   IN   IN   IN   IN   IN  IN
   0:in 1:out
   */
	bkpA_C[0]= q->pio_papar;	
	bkpA_C[1]= q->pio_padir;
//    printf("PA PAR %x  DIR %x DATA %x \n", bkpA_C[0],bkpA_C[1],q->pio_pcdat);	
//	q->pio_papar &= 0x0003;//rs232 cm1
//	q->pio_padir = 0x0000;
    
//    q->pio_papar = 0x0000;
	q->pio_padir =  /*(1<<1)|*/ //PA1=out
	                (1<<3)| //PA3=out
	                (1<<5)| //PA5=out
	                (1<<7); //PA7=out
	bkpA_C[0]= q->pio_papar;	
	bkpA_C[1]= q->pio_padir;
//    printf("PA PAR %x  DIR %x DATA %x \n", bkpA_C[0],bkpA_C[1],q->pio_pcdat);	

    /*
    PB0 PB1 PB2 PB3 PB4 PB5 PB6 PB7 PB8 PB9 PB10 PB11 PB12 PB13 PB14 PB15 PB16 PB17
    <   eeprom --->  IN OUT OUT  IN  IN  IN  OUT   IN  OUT  OUT  OUT  OUT OUT  <Eeprom>   0:in 1:out
    */
	bkpB[0]= q->pip_pbpar;	
	bkpB[1]= q->pip_pbdir;

//    printf("PB PAR %x  DIR %x\n", bkpB[0],bkpB[1]);	
	q->pip_pbpar = 0x0000000E;	
	q->pip_pbdir = 0x0001F46E;
    /*
    PC0 PC1 PC2 PC3 PC4 PC5 PC6 PC7 PC8 PC9 PC10 PC11
     IN  IN  IN  IN  IN  IN <e2p  >  IN  IN   IN   IN 
     */
    q->pio_pcpar = 0x0000;
    q->pio_pcdir = 0x0000;
   	bkpA_C[2]= q->pio_pcpar;	
	bkpA_C[3]= q->pio_pcdir;
//    printf("PC PAR %x  DIR %x DATA %x\n", q->pio_pcpar,q->pio_pcdir,q->pio_pcdat);	
 
 
    /*
    Procedere stimolando prima a 1 e poi a 0 uno per volta i segnali riportati nella colonna dei Segnali di Uscita
    della tabella 2 e 
    rileggerne lo stato attraverso il corrispondente segnale di ingresso riportato sulla stessa riga
    verificando che lo stato di tutti gli altri segnali di ingresso sia rimasto inalterato. 
    Per i segnali riportati nella
    colonna destra della tabella e per i quali non esiste un corrispondente nella colonna sinistra, occorrerà
    provvedere ad una stimolazione esterna alla scheda stessa.    
    */
//    printf("\n      ");
//    for (i = 1; i <= 54; i++) putchar('0' + (i / 10));
//    printf("\n      ");
//    for (i = 1; i <= 54; i++) putchar('0' + (i % 10));
//    printf("\n");
//    
        /*  TEST1 */
    for(i=1;i<55;i++)
    {
        /*reset error */
        err[i]=0;
        
//        while( ((c=_getkey())  != '\t'));
        
        /*set a 0 di tutti i DO*/
        clearDO();

        for(j=0;j<100;j++);

        /*salvo lo stato dei di prima del test1*/
        tstio_getDI();
        lettura_di_prec[0] = lettura_di[0];
        lettura_di_prec[1] = lettura_di[1];
        lettura_di_prec[2] = lettura_di[2];
        lettura_di_prec[3] = lettura_di[3];

        /*set a 1 l'output */    
        tstio_setDO(i,1);

        for(j=0;j<1000;j++);
        /* aggiorna la lettura degli ingressi*/
        tstio_getDI();
         
//        printf("\nTest a 1 riga %2d\n",i);
//        printf("READ:");
//        printbits(lettura_di[0],16,'0','1');
//        printbits(lettura_di[1],16,'0','1');
//        printbits(lettura_di[2],16,'0','1');
//        printbits(lettura_di[3],6,'0','1');

        /* confrontando le variazioni sui di*/
        result[0] = lettura_di_prec[0]^lettura_di[0];
        result[1] = lettura_di_prec[1]^lettura_di[1];
        result[2] = lettura_di_prec[2]^lettura_di[2];
        result[3] = lettura_di_prec[3]^lettura_di[3];
//        printf("\nVRFY:");
//        printbits(result[0],16,'0','1');
//        printbits(result[1],16,'0','1');
//        printbits(result[2],16,'0','1');
//        printbits(result[3],6,'0','1');
//        printf("\n");
        /*controlla le variazioni degli ingressi*/
        for(j=0;j<4;j++){
            for(k = 0; k <= 15; k++){
                posiz = k+(j*16)+1;
                if (result[j] & (1<<k)){
//                    printf("TEST 1 riga=%d -> letto 1 posiz=%d  (k,j,i)=(%d,%d,%d)\n",i,posiz,k,j,i);
                    if ( posiz != i)
                    {                
                        if (((i==1 || i==5) && posiz!=38)||
                            ((i==2 || i==6) && posiz!=45)||
                            ((i==3 || i==7) && posiz!=53)||
                            ((i==4 || i==8) && posiz!=54)||
                            (i>8))
                         {
                            printf("!!! TEST 1  riga %d KO -> letto 1 alla riga %d della tabella 2 della specifica di collaudo \n",i,posiz);
                            err[i] = 1; //error
                            break;
                         }
                    } 
                }      
            }
        }
//        printf("errT1 =%d\n",err[i]);
        
        /* TEST0*/
        lettura_di_prec[0] = lettura_di[0];
        lettura_di_prec[1] = lettura_di[1];
        lettura_di_prec[2] = lettura_di[2];
        lettura_di_prec[3] = lettura_di[3];

       /*set a 0 l'output */    
        tstio_setDO(i,0);

        /*just a delay*/
        for(j=0;j<1000;j++);
        /* aggiorna la lettura degli ingressi*/
        tstio_getDI();
//        printf("\nTest a 0 riga %2d\n",i);
//        printf("READ:");
//        printbits(lettura_di[0],16,'0','1');
//        printbits(lettura_di[1],16,'0','1');
//        printbits(lettura_di[2],16,'0','1');
//        printbits(lettura_di[3],6,'0','1');
//        printf("\n");
        /* confrontando le variazioni sui di*/
        


        /*controlla le variazioni degli ingressi*/
        for(j=0;j<4;j++){
           for(k = 0; k <= 15; k++){
                posiz = k+(j*16)+1;
                if (lettura_di[j] & (1<<k)){
                    printf("!!! TEST 0 riga %d KO -> letto 1 alla riga %d della tabella 2 della specifica di collaudo \n",i,posiz);
                    err[i] = 2; //error
                    break;
                }
            } 
        }
//        printf("errT0 =%d \n",err[i]);

    }
    
    test_ret= 0;//superato
 	/* ripristino la configurazione delle porte*/
 	q->pio_papar = bkpA_C[0];	
	q->pio_padir = bkpA_C[1];
       
 	q->pip_pbpar = bkpB[0];	
	q->pip_pbdir = bkpB[1];
       
 	q->pio_pcpar = bkpA_C[2];	
	q->pio_pcdir = bkpA_C[3];
       
    for(j=0;j<1000;j++);
    
    printf("\nESITO TEST IO DIGITALI\n");
    for(i=1;i<54;i++)
    {
        if (err[i]==1){
 //           printf("KO test 1 line %d %s \n",i,c_fetch_message(testio, i, ts, 0));
            test_ret= 1;//non superato
        }
        else if (err[i]==2){
 //           printf("KO test 0 line %d %s \n",i,c_fetch_message(testio, i, ts, 0));
            test_ret= 1;//non superato
        }
    }


    if(test_ret){
        printf("TEST IO FALLITO\n");
        save_stato(TESTIOKO);
    }
    else
        {    
        printf("TEST IO SUPERATO\n");
        save_stato(TESTIOOK);
    }
 	/* ripristino la configurazione delle porte*/
// 	q->pio_papar = bkpA_C[0];	
//	q->pio_padir = bkpA_C[1];
//       
// 	q->pip_pbpar = bkpB[0];	
//	q->pip_pbdir = bkpB[1];
//       
// 	q->pio_pcpar = bkpA_C[2];	
//	q->pio_pcdir = bkpA_C[3];
       
        
}

