
#line 1 "src\main.c" /0
  
#line 1 "C:\KEIL\C51\INC\REG51.H" /0






 
 
 
 
 
 
 sfr P0   = 0x80;
 sfr P1   = 0x90;
 sfr P2   = 0xA0;
 sfr P3   = 0xB0;
 sfr PSW  = 0xD0;
 sfr ACC  = 0xE0;
 sfr B    = 0xF0;
 sfr SP   = 0x81;
 sfr DPL  = 0x82;
 sfr DPH  = 0x83;
 sfr PCON = 0x87;
 sfr TCON = 0x88;
 sfr TMOD = 0x89;
 sfr TL0  = 0x8A;
 sfr TL1  = 0x8B;
 sfr TH0  = 0x8C;
 sfr TH1  = 0x8D;
 sfr IE   = 0xA8;
 sfr IP   = 0xB8;
 sfr SCON = 0x98;
 sfr SBUF = 0x99;
 
 
 
 
 sbit CY   = 0xD7;
 sbit AC   = 0xD6;
 sbit F0   = 0xD5;
 sbit RS1  = 0xD4;
 sbit RS0  = 0xD3;
 sbit OV   = 0xD2;
 sbit P    = 0xD0;
 
 
 sbit TF1  = 0x8F;
 sbit TR1  = 0x8E;
 sbit TF0  = 0x8D;
 sbit TR0  = 0x8C;
 sbit IE1  = 0x8B;
 sbit IT1  = 0x8A;
 sbit IE0  = 0x89;
 sbit IT0  = 0x88;
 
 
 sbit EA   = 0xAF;
 sbit ES   = 0xAC;
 sbit ET1  = 0xAB;
 sbit EX1  = 0xAA;
 sbit ET0  = 0xA9;
 sbit EX0  = 0xA8;
 
 
 sbit PS   = 0xBC;
 sbit PT1  = 0xBB;
 sbit PX1  = 0xBA;
 sbit PT0  = 0xB9;
 sbit PX0  = 0xB8;
 
 
 sbit RD   = 0xB7;
 sbit WR   = 0xB6;
 sbit T1   = 0xB5;
 sbit T0   = 0xB4;
 sbit INT1 = 0xB3;
 sbit INT0 = 0xB2;
 sbit TXD  = 0xB1;
 sbit RXD  = 0xB0;
 
 
 sbit SM0  = 0x9F;
 sbit SM1  = 0x9E;
 sbit SM2  = 0x9D;
 sbit REN  = 0x9C;
 sbit TB8  = 0x9B;
 sbit RB8  = 0x9A;
 sbit TI   = 0x99;
 sbit RI   = 0x98;
 
 
#line 1 "src\main.c" /0
 
  
#line 1 "C:\KEIL\C51\INC\STDIO.H" /0






 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 typedef unsigned int size_t;
 
 
 #pragma SAVE
 #pragma REGPARMS
 extern char _getkey (void);
 extern char getchar (void);
 extern char ungetchar (char);
 extern char putchar (char);
 extern int printf   (const char *, ...);
 extern int sprintf  (char *, const char *, ...);
 extern int vprintf  (const char *, char *);
 extern int vsprintf (char *, const char *, char *);
 extern char *gets (char *, int n);
 extern int scanf (const char *, ...);
 extern int sscanf (char *, const char *, ...);
 extern int puts (const char *);
 
 #pragma RESTORE
 
 
 
#line 2 "src\main.c" /0
 
 
  
#line 1 "SRC\UTIL.H" /0
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 








 
 
















 
 
 
 
 
 
 
 
 







 
 
 
 






 
 
 






 
 


 









 
 typedef unsigned char uchar;
 typedef unsigned int  uint;
 typedef unsigned short  ushort;
 
 typedef unsigned char UCHAR;
 typedef unsigned int  UINT;
 typedef unsigned short  USHORT;
 
 
 
  extern unsigned char set_char_bit(unsigned char *variabile, unsigned char posizione, unsigned char valore);
  extern unsigned char set_int_bit(unsigned int *variabile,unsigned char posizione,unsigned char valore);
 
  extern unsigned char get_char_bit(unsigned char *variabile, char posizione);
  extern unsigned char get_int_bit(unsigned int *variabile, char posizione);
  extern unsigned char get_long_bit(unsigned long *variabile, char posizione);
 
  extern void print_char_bin(unsigned char variabile);
  extern void print_int_bin(unsigned int valore);
  extern void print_long_bin(unsigned long variabile);
 
 
 
 
 
 
 
 
 
 
 
 
 
#line 4 "src\main.c" /0
 
  
#line 1 "SRC\TEST_RAM.H" /0
 

 
 
 
 
 
 
 
 
 void test_ram();
 
 
#line 5 "src\main.c" /0
 
  
#line 1 "SRC\ADC.H" /0
 
 
 
 
 void test_adc(void);
#line 6 "src\main.c" /0
 
  
#line 1 "SRC\PIO.H" /0
 
 
 
 
 
 
 
 
 
 
 
 
 







 
 
 
 
 





 
 
 
 
 sbit PIO_RESET = P1^7;







 
 void test_pio(void);
 void test_pio_menu(void);
#line 7 "src\main.c" /0
 
  
#line 1 "SRC\UART8251.H" /0
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 void uart_reset();
 void test_init();
 void test_uartU27();
 void test_uartU30();
 void test_uartU27_rx_tx();
 void test_uartU27_ricevi();
 void test_uartU27_trasmetti();
 
#line 8 "src\main.c" /0
 
 


 
 
 sbit	WD = P3^5;
 sbit	uart_tx_enable = P3^4;
 
 
 unsigned short idata timer0_tick=0;
 
 
 unsigned char del;
 unsigned char aaa;
 unsigned char tipo_scheda;
 unsigned char ritardo;
 char scelta_test;
 char scelta_scheda;
 
 int get_timer0();


 
 static void timer0_isr (void) interrupt 1
 {
 TR0 = 0;   
 EA = 0;	   
 
 WD ^= 1;	 
 timer0_tick++;
 TH0 = 0x4b;	  
 TL0 = 0xff;
 
 TR0 = 1;    
 EA  = 1;
 }
 


 
 
 int get_timer0(void)
 {
 return ((int)timer0_tick);
 }
 
 void init (void)
 {
 SCON 	= 0x52;		 
 TMOD	= 0x21;  	 
 
 
 TH1 	= 0xfd;		 
 TL1 	= TH1;
 
 TH0 	= 0x4b; 	 
 TL0 	= 0xff;
 
 IP		= 0x02;	 	 
 
 IT0 	= 1;		 
 IT1 	= 1;		 
 IE 		= 0x87;	 	 
 ET0 	= 1;		 
 
 TR0 	= 1;     
 TR1 	= 1;     
 
 TI    	= 1;     
 
 uart_tx_enable =0;  
 }











 


 
 
 void menu (void)
 {
 unsigned char	exit_menu; 
 char c;
 do
 {
 printf("\n\n");
 printf("***********************************\n");
 printf("*   Collaudo scheda CPU REVIND    *\n");
 printf("*   sw ver. testrew 1.0           *\n");
 
 if (tipo_scheda == 1)	printf("*   scheda tipo: DG630-12 (x16)   *\n");
 if (tipo_scheda == 2)	printf("*   scheda tipo: DG630-29 (x64)   *\n");
 printf("***********************************\n");
 printf("\n\n");
 printf("	 1 - test RAM\n");
 printf("	 2 - test ADC\n");
 printf("	 3 - test I/O Progr\n");
 printf("	 4 - test UART_U27 - RICEZIONE\n");
 printf("	 5 - test UART_U27 - TRASMISSIONE\n");
 printf("	 6 - test UART_U30\n");
 printf("	 7 - set tipo scheda \n\n");
 printf ("\n\n     Scelta test: ");
 
 c = _getkey();
 
 scelta_test = c-0x30;
 
 if(scelta_test>0 && scelta_test<8)
 exit_menu=1;
 else 
 exit_menu=0;
 
 
 
 }
 while(!exit_menu);
 }
 unsigned char menu_scheda (void)
 {
 unsigned char	exit_menu; 
 char c;
 do
 {
 printf("\n\n");
 printf("***********************************\n");
 printf("*   Collaudo scheda CPU REVIND    *\n");
 printf("*   sw ver. testrew 1.0           *\n");
 printf("***********************************\n");
 printf("\n\n");
 printf("	 1 - DG630-12(prescaler x16)\n");
 printf("	 2 - DG630-29(prescaler x64)\n");
 printf ("\n\n     Scelta scheda: ");
 c = _getkey();
 
 scelta_scheda = c-0x30;
 
 if(scelta_scheda>0 && scelta_scheda<3)
 exit_menu=1;
 else 
 exit_menu=0;
 
 
 
 }
 while(!exit_menu);
 return scelta_scheda;
 }
 


 
 void main (void)
 {


 
 
 init();		 
 WD = 0;			 


 
 printf("\n\n\n");
 printf("\n\n\n");
 tipo_scheda = menu_scheda();
 
 while (1)
 {


 
 
 
 menu();
 
 switch (scelta_test)
 {
 case 1: 	test_ram();
 break;
 case 2: 	test_adc();
 break;
 case 3: 	test_pio_menu();
 break;
 case 4: 	test_uartU27_ricevi();
 break;
 case 5: 	test_uartU27_trasmetti();
 break;
 case 6: 	test_uartU30();
 break;
 case 7: 	tipo_scheda = menu_scheda();
 break;
 
 default: break;
 
 }
 
 }
 }
