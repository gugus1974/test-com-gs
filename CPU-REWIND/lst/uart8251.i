
#line 1 "src\uart8251.c" /0
 
 
 
 
  
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
 
 
#line 5 "src\uart8251.c" /0
 
  
#line 1 "C:\KEIL\C51\INC\CTYPE.H" /0






 
 
 
 
 
 #pragma SAVE
 #pragma REGPARMS
 extern bit isalpha (unsigned char);
 extern bit isalnum (unsigned char);
 extern bit iscntrl (unsigned char);
 extern bit isdigit (unsigned char);
 extern bit isgraph (unsigned char);
 extern bit isprint (unsigned char);
 extern bit ispunct (unsigned char);
 extern bit islower (unsigned char);
 extern bit isupper (unsigned char);
 extern bit isspace (unsigned char);
 extern bit isxdigit (unsigned char);
 extern unsigned char tolower (unsigned char);
 extern unsigned char toupper (unsigned char);
 extern unsigned char toint (unsigned char);
 
 
 
 
 #pragma RESTORE
 
 
#line 6 "src\uart8251.c" /0
 
  
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
 
 
 
#line 7 "src\uart8251.c" /0
 
  
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
 
 
 
 
 
 
 
 
 
 
 
 
 
#line 8 "src\uart8251.c" /0
 
  
#line 1 "SRC\UART8251.H" /0
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 void uart_reset();
 void test_init();
 void test_uartU27();
 void test_uartU30();
 void test_uartU27_rx_tx();
 void test_uartU27_ricevi();
 void test_uartU27_trasmetti();
 
#line 9 "src\uart8251.c" /0
 
  
#line 1 "SRC\PIO.H" /0
 
 
 
 
 
 
 
 
 
 
 
 
 







 
 
 
 
 





 
 
 
 
 sbit PIO_RESET = P1^7;







 
 void test_pio(void);
 void test_pio_menu(void);
#line 10 "src\uart8251.c" /0
 
 
 
 sbit TXE_U27=	P1^1;
 sbit TXE_U30=	P1^0 ;
 
 static char c;
 static unsigned char xdata buff_U27[256];
 static unsigned char xdata buff_U30[256];
 static unsigned char inizio=0;
 static unsigned char fine_u30=0;
 static unsigned char fine_u27=0;
 static unsigned int xdata start_ritardo=0;
 
 
 static unsigned char i;
 extern unsigned char del;
 extern unsigned char tipo_scheda;
 extern int get_timer0();
 volatile unsigned char xdata uartU27[2]  _at_ 0x6000 ;
 volatile unsigned char xdata uartU30[2]  _at_ 0x8000 ;
 
 
 static void uartU30_init ( ) ;
 


 
 
 static void RX_RDY_U30_isr (void) interrupt 0   
 {
 EA = 0;
 buff_U30[fine_u30++] = uartU30[0];
 EA  = 1;
 }
 
 static void RX_RDY_U27_isr (void) interrupt 2   
 {
 EA = 0;
 buff_U27[fine_u27] = uartU27[0];
 fine_u27++;
 EA  = 1;
 }
 
 
 
 
 
 void uart_reset (void)
 {
 
 PIO_RESET = 1;
 for(del =0 ; del<100 ; del++) ;
 PIO_RESET = 0;
 printf("\n\nReset uart\n");
 }
 
 static void uartU27_init (void)
 {
 uart_reset();
 
 
 printf("conf uart U27 @%04X\n",(unsigned short) uartU27);
 
 uartU27[1] = 
#line 83 "src\uart8251.c" /0
 ((0<<7) | (1<<6) | (0<<5) | (0<<4) | (1<<3) | (1<<2) | (1<<1) | (tipo_scheda==2) );
 printf("1 - uart U27->%02bXH\n", uartU27[1]);
 
 
 
 uartU27[1] = 
#line 97 "src\uart8251.c" /0
 ((0<<7) | (0<<6) | (1<<5) | (1<<4) | (0<<3) | (1<<2) | (1<<1) | 1 );
 printf("2 - uart U27->%02bXH\n", uartU27[1]);
 
 
 
 printf("3 - uart U27->%02bX\n", uartU27[0]);
 
 
 
 
 }
 
 static void uartU30_init ()
 {
 
 uart_reset();
 
 
 printf("conf uart U30 \n");
 
 
 uartU30[1] = 
#line 128 "src\uart8251.c" /0
 ((0<<7) | (1<<6) | (0<<5) | (0<<4) | (1<<3) | (1<<2) | (1<<1) | (tipo_scheda==2) );
 
 

 
 uartU30[1] = 
#line 142 "src\uart8251.c" /0
 ((0<<7) | (0<<6) | (1<<5) | (1<<4) | (0<<3) | (1<<2) | (0<<1) | 1 );
 
 
 
 
 
 }
 
 
 
 void test_uartU27_rx_tx()
 {
 
 
 uartU27_init();	
 
 fine_u27 =0;
 
 uartU27[1] = 0x35;  
 
 while (1) 
 {
 
 
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
 uartU27[0]= 0x55;








  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
 
 
 






 
 
 }
 
 
 }
 
 
 void test_uartU27_ricevi()
 {
 
 
 uartU27_init();	
  for(del =0 ; del<200 ; del++);
 fine_u27 =0;
 
 uartU27[1] = 0x35; 
  for(del =0 ; del<200 ; del++);
 while (1) 
 {
 if (fine_u27!=0)
 {
 printf("\nRicevuti %bd byte : ",fine_u27);
 for(i=0; i<fine_u27; i++)
 printf("%02bXH ",buff_U27[i]);
 printf("\n");
 fine_u27 =0;
 }		
 }
 }
 void test_uartU27_trasmetti()
 {
 
 
 uartU27_init();	
  for(del =0 ; del<200 ; del++);
 
 uartU27[1] = 0x37; 
 
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
 
 while (1) 
 {
 
 
 
 uartU27[0]=0x55;
 printf("\nTX: 0x%02bxH ",0x55);
 
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
 }
 
 
 }
 
 
 void test_uartU30()
 {
 
 unsigned char i=0;
 char exit_char;
 printf("\nPremere un tasto e verificare che corrisponda a quello ricevuto (q per uscire)\n");
 uartU30_init();	
 fine_u30 =0;
 while (1) 
 {
 c=getchar();
 if((exit_char=toupper(c))=='Q') break;
 if (TXE_U30 ) 
 {








 
 uartU30[0] = c;
 printf("\nTX:%c(%02bxH)  ",c,c);
 }
 
 if (fine_u30!=0)
 {
 printf("  RX (%bd byte):",fine_u30);
 for(i=0; i<fine_u30; i++)
 {
 printf("% 02bx ",buff_U30[i]);
 
 }
 printf("\n\n");
 fine_u30 = 0;
 }
 }
 printf("\n");




 
 }
