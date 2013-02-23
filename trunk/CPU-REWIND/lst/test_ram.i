
#line 1 "src\test_ram.c" /0
 


 
  
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
 
 
#line 5 "src\test_ram.c" /0
 
  
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
 
 
 
#line 6 "src\test_ram.c" /0
 
  
#line 1 "SRC\TEST_RAM.H" /0
 

 
 
 
 
 
 
 
 
 void test_ram();
 
 
#line 7 "src\test_ram.c" /0
 
  
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
 
 
 
 
 
 
 
 
 
 
 
 
 
#line 8 "src\test_ram.c" /0
 
 
 
 
 
 unsigned short xdata * ADDR ;   
 
 
 
 
 
 
 
 void test_ram(void);
 
 
 
 
 static unsigned char i=0,j =0;
 static unsigned char err =0;
 
 
 
 
 
 
 void test_ram(void)
 {
 unsigned short adr;
 unsigned short appo;
 char c;
 
 
 printf("\nTest RAM ESTERNA (0x0000 .. 0x1FFF)\n\n");
 printf("Write pattern 0xAA55  \n");
 err=0;
 for( ADDR=0x0000; ADDR<0x1FFF; ADDR++)
 {
 adr = (unsigned short)ADDR ;
 printf("@%04X \r",adr );
 
 *ADDR=0xAA55;
 }
 
 printf("\nVerify at \n");
 for( ADDR=0x0000; ADDR<0x1FFF; ADDR++)
 {
 adr = (unsigned short)ADDR ;
 printf("%04X \r",adr );
 
 if(*ADDR!=0xAA55) 
 {
 printf("test RAM KO %04X @ read %04x\n",ADDR,*ADDR);
 err=1;
 break; 	  
 }
 }
 
 
 
 printf("\nWrite pattern 0x55AA at \n");
 for( ADDR=0x0000; ADDR<0x1FFF; ADDR++)
 {
 adr = (unsigned short)ADDR ;
 printf("%04X \r",adr );
 
 *ADDR=0x55AA;
 }
 
 
 printf("\nVerify at \n");
 for( ADDR=0x0000; ADDR<0x1FFF; ADDR++)
 {
 adr = (unsigned short)ADDR ;
 printf("%04X \r",adr );
 
 if(*ADDR!=0x55AA) 
 {
 printf("test RAM KO %lX @ read %04x\n",ADDR,*ADDR);
 err=2;
 break; 	  
 }
 }
 
 printf("\nTest 1-shifting on A0..A12 \n");
 
 ADDR = 0x0000;
 *ADDR = 0xAA55;
 printf("%04X (0x0000) ",(unsigned short)ADDR );
 if (*ADDR == 0xAA55)
 printf(" ..ok\n");
 else	
 printf("..KO %lx\n",*ADDR), err=3;
 
 for(i=0; i<255; i++);
 
 j=0;
 ADDR= 0x0001;
 appo = 0x0001;
 while (appo<0x2000) 
 {
 adr = (unsigned short)ADDR ;
 printf("%04X  (A%bd) ",((unsigned short)ADDR),j++ );
 
 *ADDR = 0xAA55;
 
 for(i=0; i<255; i++);
 
 if (*ADDR == 0xAA55)
 printf(" ..ok\n");
 else	
 printf("..KO %u\n",*ADDR), err=3;
 
 appo <<= 1;
 ADDR = (unsigned short xdata *)appo;
 }
 
 if(!err)
 printf("Test RAM OK\n");
 else 
 printf("Test RAM KO error  %bd\n",err);
 
 printf("Premere INVIO per proseguire.\r\n");
 while ( (c = getchar()) !=0x0a) ;
 }
 
