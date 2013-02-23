
#line 1 "src\adc.c" /0
 
 
 
 
  
#line 1 "C:\KEIL\C51\INC\STDLIB.H" /0






 
 
 
 
 
 
 
 typedef unsigned int size_t;
 
 
 
 
 
 
 
 
 typedef char wchar_t;
 
 
 
 #pragma SAVE
 #pragma REGPARMS
 
 extern int    abs  (int   val);
 extern long  labs  (long  val);
 
 extern float atof (char *s1);
 extern long  atol (char *s1);
 extern int   atoi (char *s1);
 extern int   rand ();
 extern void  srand (int);
 
 extern float         strtod  (char *, char **);
 extern long          strtol  (char *, char **, unsigned char);
 extern unsigned long strtoul (char *, char **, unsigned char);
 
 
 
 extern void init_mempool          (void xdata *p, unsigned int size);
 extern void xdata *malloc  (unsigned int size);
 extern void free                  (void xdata *p);
 extern void xdata *realloc (void xdata *p, unsigned int size);
 extern void xdata *calloc  (unsigned int size, unsigned int len);
 
 #pragma RESTORE
 
 
#line 5 "src\adc.c" /0
 
  
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
 
 
#line 6 "src\adc.c" /0
 
  
#line 1 "C:\KEIL\C51\INC\STDIO.H" /0






 
 
 
 
 
 
 
 
 
 
#line 17 "C:\KEIL\C51\INC\STDIO.H" /1
  
 
#line 19 "C:\KEIL\C51\INC\STDIO.H" /0
 
 
#line 21 "C:\KEIL\C51\INC\STDIO.H" /1
  
 
 
#line 24 "C:\KEIL\C51\INC\STDIO.H" /0
 
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
 
 
 
#line 7 "src\adc.c" /0
 
  
#line 1 "C:\KEIL\C51\INC\STRING.H" /0






 
 
 
 
 
 
#line 13 "C:\KEIL\C51\INC\STRING.H" /1
  
 
 
#line 16 "C:\KEIL\C51\INC\STRING.H" /0
 
 
#line 18 "C:\KEIL\C51\INC\STRING.H" /1
  
 
#line 20 "C:\KEIL\C51\INC\STRING.H" /0
 
 #pragma SAVE
 #pragma REGPARMS
 extern char *strcat (char *s1, char *s2);
 extern char *strncat (char *s1, char *s2, int n);
 
 extern char strcmp (char *s1, char *s2);
 extern char strncmp (char *s1, char *s2, int n);
 
 extern char *strcpy (char *s1, char *s2);
 extern char *strncpy (char *s1, char *s2, int n);
 
 extern int strlen (char *);
 
 extern char *strchr (const char *s, char c);
 extern int strpos (const char *s, char c);
 extern char *strrchr (const char *s, char c);
 extern int strrpos (const char *s, char c);
 
 extern int strspn (char *s, char *set);
 extern int strcspn (char *s, char *set);
 extern char *strpbrk (char *s, char *set);
 extern char *strrpbrk (char *s, char *set);
 extern char *strstr  (char *s, char *sub);
 extern char *strtok  (char *str, const char *set);
 
 extern char memcmp (void *s1, void *s2, int n);
 extern void *memcpy (void *s1, void *s2, int n);
 extern void *memchr (void *s, char val, int n);
 extern void *memccpy (void *s1, void *s2, char val, int n);
 extern void *memmove (void *s1, void *s2, int n);
 extern void *memset  (void *s, char val, int n);
 #pragma RESTORE
 
 
#line 8 "src\adc.c" /0
 
  
#line 1 "SRC\ADC.H" /0
 
 
 
 
 void test_adc(void);
#line 9 "src\adc.c" /0
 
  
#line 1 "SRC\PIO.H" /0
 
 
 
 
 
 
 
 
 
 
 
 
 







 
 
 
 
 





 
 
 
 
 sbit PIO_RESET = P1^7;







 
 void test_pio(void);
 void test_pio_menu(void);
#line 10 "src\adc.c" /0
 


 
 
 
 
 
 
 sbit EOC = P1^2;
 
 
 static unsigned char  val_atteso[8] ={		0xB0, 
 0x5B, 
 0x59, 
 0x8E, 
 0x84, 
 0xb5, 
 0, 
 0};
 static char result[3];
 
 
 
 static unsigned short  canale[8];
 static unsigned short  xdata esito[8];
 unsigned char xdata *adc;
 
 
 unsigned char num_s;
 
 unsigned char i=0;
 unsigned char k=0,s;
 unsigned short appo;
 char c;
 extern unsigned char tipo_scheda;
 
 
 
 void test_adc(void);
 
 
 unsigned char test_adc_canale(unsigned char i,unsigned char val)
 {
 unsigned char ret;
 
 canale[i]=0;
 ret =0;
 for(num_s=1 ;num_s<=10; num_s++)
 {
 adc = 0xA000 + i;
 *adc = 0x7f;	 
 
 while(!EOC)  ;  
 
 s  = *adc;  
 
 printf("ch: %bd   ",i);
 printf("sample n° %bd   ",num_s);
 printf("lettura: 0x%bX (%1.2f Volt) \n",(*adc), *adc *0.0196 );
 
 canale[i] = canale[i] + *adc ;
 }
 canale[i] = canale[i] / 10;
 printf("ch: %bd  lettura 0x%X   (%1.2f Volt)\n", i, canale[i],(canale[i]* 0.0196));
 
 if( abs(canale[i] - val) <= 13) 
 {strcpy (result, "OK"+'\0'); ret = 0;}
 else
 {strcpy (result, "KO" +'\0'); ret = 1;}
 printf("Esito test ch IN%bd = 0x%02X   lettura: %1.2fV  Val atteso: %1.2fV    ",i,canale[i],(canale[i]* 0.0196),(unsigned short)val* 0.0196);
 printf("esito: %s\n",result);
 return (ret);
 }
 void test_adc(void)
 {
 
 int ritardo;
 
 
 
 for(i=0; i<8; i++) esito[i]=0;
 printf("\n\n*****  Test ADC  *****   \n\n");




















 
 if(tipo_scheda == 1) 
 val_atteso[5] =0;
 else 
 val_atteso[5] =0xb5; 
 
 
 printf("\nPosizionare lo switch NOISE al centro e\n");
 printf("verificare che i led S1 e S2 siano spenti\n");
 printf("Premere INVIO per proseguire \n");while ( (c = getchar())!= 0x0A) ;
 printf("\nTest canale IN0\n");
 
 for( ritardo=0; ritardo<5000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 
 
 
 esito[0] = test_adc_canale(0, 0);
 
 if (esito[0]) esito[0]<<=1;
 printf("Premere INVIO per proseguire \n");while ( (c = getchar())!= 0x0A) ;
 printf("\nTest canale IN1\n");
 for( ritardo=0; ritardo<5000; ritardo++); 
 esito[1] = test_adc_canale(1, 0);
 
 if (esito[1]) esito[1]<<=1;
 printf("Premere INVIO per proseguire \n");while ( (c = getchar())!= 0x0A) ;
 printf("\nTest canale IN2\n");
 for( ritardo=0; ritardo<5000; ritardo++); 
 esito[2] = test_adc_canale(2, 0);
 
 if (esito[2]) esito[2]<<=1;
 
 printf("Premere INVIO per proseguire \n");while ( (c = getchar())!= 0x0A) ;
 
 for(i=0; i<8; i++)  
 {
 printf("\nTest IN%bd\n",i);
 if(i == 0) 
 printf("Posizionare lo switch NOISE su NOISE1 e verificare cke il led S1 sia acceso (test IN%bd)\n",i);
 else if(i == 1) 
 printf("Posizionare lo switch NOISE su NOISE1 (test IN%bd)\n",i);
 else if(i == 2) 
 printf("Posizionare lo switch NOISE su NOISE2 e verificare cke il led S2 sia acceso (test IN%bd)\n",i);
 printf("Premere INVIO per proseguire \n");while ( (c = getchar())!= 0x0A) ;
 
 if(i < 2 ) 
 {
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 for( ritardo=0; ritardo<10000; ritardo++); 
 }
 esito[i] |= test_adc_canale(i, val_atteso[i]);
 
 printf("Premere INVIO per proseguire \n");
 while ( (c = getchar())!= 0x0A) ;
 }
 
 printf("Riepilogo test ADC\n");
 
 for(i=0; i<8; i++)
 {
 if(( abs(canale[i] - val_atteso[i]) <= 13) && (esito[i]&0x02)==0) 
 strcpy (result, "OK"+'\0');
 else
 strcpy (result, "KO" +'\0');
 printf("Test IN%bd = 0x%02X   lettura: %1.2fV  Val atteso: %1.2fV    ",i,canale[i],(canale[i]* 0.0196),((unsigned short)val_atteso[i])* 0.0196);
 printf("esito: %s\n",result);
 }
 getchar();
 printf("\n\n");
 }
