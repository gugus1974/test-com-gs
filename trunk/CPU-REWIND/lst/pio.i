
#line 1 "src\pio.c" /0
 
 
 
  
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
 
 
#line 4 "src\pio.c" /0
 
 
  
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
 
 
#line 6 "src\pio.c" /0
 
  
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
 
 
 
#line 7 "src\pio.c" /0
 
  
#line 1 "SRC\PIO.H" /0
 
 
 
 
 
 
 
 
 
 
 
 
 







 
 
 
 
 





 
 
 
 
 sbit PIO_RESET = P1^7;







 
 void test_pio(void);
 void test_pio_menu(void);
#line 8 "src\pio.c" /0
 
 
 
 
 sbit P1_3 = P1^3;
 sbit P1_4 = P1^4;
 sbit P1_5 = P1^5;
 sbit P1_6 = P1^6;
 
 
 
 volatile unsigned char 	xdata  U26[4] _at_ 0x4000;
 volatile unsigned char 	xdata  U29[4] _at_ 0x2000;
 volatile unsigned char 	xdata  buff_U37 _at_ 0xE000;
 volatile unsigned char 	xdata  buff_U38 _at_ 0xC000;
 
 
 static unsigned char xdata cont=0;
 static unsigned short xdata start_ritardo=0;
 static unsigned char xdata error;	
 static unsigned char xdata error_PA;	
 static unsigned char xdata error_U38;	
 static unsigned char xdata esito_test_led_off;	
 static unsigned char xdata esito_test_led_on;	
 static unsigned char xdata esito_test_sw1;	
 static unsigned char xdata esito_test_u15;	
 static unsigned char xdata esito_test_u26_pb;	
 static unsigned char xdata esito_test_u26_pc;	
 static unsigned char xdata esito_test_u29_pa;	
 static unsigned char xdata esito_test_u38;	
 static unsigned char xdata esito_test_pio;	
 extern unsigned char del;	
 static unsigned char xdata c;	
 static unsigned char xdata i;
 static unsigned char xdata mask, mask2;
 
 
 unsigned char ins_stato_led();
 
 unsigned char test_U15();   
 unsigned char test_U26_PC();	   
 unsigned char test_U26_PB();	   
 unsigned char test_U29_PA();	   
 unsigned char test_U38();	   
 unsigned char test_SW1();
 
 
 unsigned char xdata exit;	
 char scelta;
 
 
 
 
 
 void pio_reset()
 {
 
 PIO_RESET = 1;
  for(del =0 ; del<200 ; del++);
  for(del =0 ; del<200 ; del++);
 PIO_RESET = 0;
 printf("Reset PIO\n");
 }




















 
 void pio_conf()
 {
 
 
 printf("CONF PIO U26 ");
 U26[3] = 0x90;    




 
 printf("CONF PIO U29 ");
 U29[3] = 0x8B;    




 
 }
 void test_pio_menu()
 {
 unsigned char	exit_menu; 
 
 char c;
 do
 {
 printf("\n\n");
 printf("***********************************\n");
 printf("*   scelta test PIO               *\n");
 printf("***********************************\n");
 printf("\n");
 printf("	 1 - comando reset\n");
 printf("	 2 - configurazione registri\n");
 printf("	 3 - test I/O completo\n");
 printf("	 4 - test lettura SW1 \n");
 printf("	 5 - test pilotaggio relè U15 \n");
 printf("	 6 - test output U26 PB \n");
 printf("	 7 - test output U26 PC \n");
 printf("	 8 - test output U29 PA \n");
 printf("	 9 - test output U38 \n");
 printf("	 0 - menu principale\n");
 printf ("\n     Scelta test: ");
 c = _getkey();
 
 scelta = c-0x30;
 
 exit_menu=0;  
 
 if(scelta>=0 && scelta<10)
 {
 
 switch (scelta)
 {
 case 1: 	pio_reset();
 break;
 case 2: 	pio_conf();
 break;
 case 3: 	test_pio();
 break;
 case 4: 	
 pio_reset();
 pio_conf();
 test_SW1();
 break;
 case 5: 	test_U15();
 break;
 case 6: 	
 pio_reset();
 pio_conf();
 test_U26_PB();
 break;
 case 7: 	
 pio_reset();
 pio_conf();
 test_U26_PC();
 break;
 case 8: 	
 pio_reset();
 pio_conf();
 test_U29_PA();
 break;
 case 9: 	
 pio_reset();
 pio_conf();
 test_U38();
 
 break;
 


 
 case 0: 	
 default:   	exit_menu=1;  
 break;
 }
 }
 }
 while(!exit_menu);
 
 }
 
 











































 
 
 
 unsigned char  test_U26_PB()	   
 {
 static unsigned char err_cnt;
 err_cnt =0;
 
 
 printf("\n\nTEST U26 porta B con rilettura su U29 porta C\n");
 printf("\nVerificare accensione dei led U13 (U26 PB = 00H) \n");
 
 U26[1] = 0x00; 
  for(del =0 ; del<200 ; del++);
 if( (U29[2]& 0x3f )==0x0F )
 printf("rilettura su U29.PC OK (0x%02bx)\n",(U29[2]));
 else{
 printf("!!! ERRATA rilettura su U29.PC  (0x%02bx invece di 0x0F)\n",(U29[2]));
 err_cnt++ ;
 }
 printf("premi <INVIO>\n");	while ( (c = getchar())!= 0x0A) ;
 
 printf("\nVerificare spegnimento dei led U13 (U26 PB = FFH)\n");
 U26[1] = 0xff; 
  for(del =0 ; del<200 ; del++);
 if( (U29[2]& 0x3f )==0x30 )
 printf("rilettura su U29.PC OK (0x%02bx)\n",(U29[2]));
 else{
 printf("!!! ERRATA rilettura su U29.PC  (0x%02bx)\n",(U29[2]));
 err_cnt++ ;
 }
 printf("premi <INVIO>\n");while ( (c = getchar())!= 0x0A) ;
 
 
 mask = 0x01;
 
 for(i=0; i<4; i++)  
 {
 U26[1] = 0xff;	 
 U26[1] &= ~mask;   
 printf("\nTest U26.PB%bd (0x%02bx) - Verificare accensione U13 led %bd\n",i,U26[1],i+2);
 
 if( (U29[2]& 0x3f) == (0x30| mask))
 printf("rilettura su U29.PC OK (0x%02bx)\n",(U29[2]));
 else{
 printf("!!! ERRATA rilettura su U29.PC (0x%02bx)\n",(U29[2]));
 err_cnt++ ;
 }
 
 mask <<=1;
 printf("premi <INVIO>\n");
 while ( (c = getchar())!= 0x0A) ;
 }
 
 for(i=4; i<6; i++)  
 {
 U26[1] = 0xff;	 
 U26[1] &= ~mask;   
 printf("\nTest U26.PB%bd (0x%02bx) - Verificare accensione U13 led %bd\n",i,U26[1],i+2);
 
 if( (U29[2]& 0x3f) == (0x30&(~mask)))
 printf("rilettura su U29.PC OK (0x%02bx)\n",(U29[2]));
 else{
 printf("!!! ERRATA rilettura su U29.PC (0x%02bx)\n",(U29[2]));
 err_cnt++ ;
 }
 
 mask <<=1;
 printf("premi <INVIO>\n");	while ( (c = getchar())!= 0x0A) ;
 }
 
 U26[1] = 0xff;	 
 U26[1] &= ~mask;   
 printf("\nTest U26.PB6 (0x%02bx)- Verificare accensione U13 led %bd\n",U26[1],8);
 if(ins_stato_led() != 0)
 err_cnt++ ;
 
 
 
 mask <<=1;
 
 U26[1] = 0xff;	 
 U26[1] &= ~mask;   
 printf("\nTest U26.PB7 (0x%02bx)- Verificare accensione U13 led %bd\n",U26[1],9);
 if(ins_stato_led() != 0)
 err_cnt++ ;
 
 
 if (err_cnt!=0) return 1;
 return 0;
 }
 
 unsigned char  test_U26_PC()	   
 {
 unsigned char ret;
 ret =0;
 printf("\n\nTEST U26 porta C\n");
 U26[2] = 0x00;	 
 mask = 0x01;
 
 for(i=0; i<8; i++)  
 {
 U26[2] = mask;
 printf("\nVerificare accensione U12 led %bd",i+2);
 mask <<=1;
 ret =ret + ins_stato_led();
 
 }
 return ret;
 }
 
 unsigned char  test_U29_PA()	   
 {
 static unsigned char err_cnt=0;
 printf("\nTEST U29 porta A con rilettura su U26 porta A\n");
 error_PA =0;
 mask = 1;
 
 buff_U38 = 0x80; 
 U29[0] = 0x01;  
 printf("\nVerificare accensione dei led U14 (U29 PA = 01H) \n");
 if( (U26[0]& 0x3F )==0x3f )
 printf("rilettura su U26.PA OK (0x%02bx)\n",(U26[0]));
 else{
 printf("!!! ERRATA rilettura su U26.PA (0x%02bx)\n",(U26[0]));
 err_cnt++ ;
 }
 printf("premi <INVIO>\n");while ( (c = getchar())!= 0x0A) ;
 
 
 buff_U38 = 0x80; 
 U29[0] = 0xFE;  
 printf("\nVerificare spegnimento dei led U14 (U29 PA = FEH)\n");
 if( (U26[0]& 0x3F )==0x00 )
 printf("rilettura su U26.PA OK (0x%02bx)\n",(U26[0]));
 else{
 printf("!!! ERRATA rilettura su U26.PA (0x%02bx)\n",(U26[0]));
 err_cnt++ ;
 }
 printf("premi <INVIO>\n");	while ( (c = getchar())!= 0x0A) ;
 
 
 buff_U38 = 0x80; 
 U29[0] = 0xFF;	 
 i=0;
 printf("\nTest U29.PA%bd (0x%02bx) - Verificare accensione U14 led %bd\n",i,U29[0],i+2);
 if(ins_stato_led() != 0)
 err_cnt++ ;
 
 
 buff_U38 = 0x80; 
 U29[0] = 0xFE;	 
 i=0;
 printf("\nTest U29.PA%bd (0x%02bx) - Verificare spegnimento U14 led %bd\n",i,U29[0],i+2);
 if(ins_stato_led() != 0)
 err_cnt++ ;
 
 
 mask = 0x02;
 mask2 = 0x01;
 for(i=1; i<8; i++)  
 {
 U29[0] = 0xFE;	 
 U29[0] &= ~mask;   
 printf("\nTest U29.PA%bd (0x%02bx) - Verificare accensione U14 led %bd\n",i,U29[0],i+2);
 if(i<7){		
 if( U26[0] ==  mask2)
 printf("rilettura su U26.PA OK (0x%02bx)\n",(U26[0]));
 else{
 printf("!!! ERRATA rilettura su U26.PA (0x%02bx)\n",(U26[0]));
 err_cnt++ ;
 }
 }
 mask <<=1;
 mask2 <<=1;
 printf("premi <INVIO>\n");while ( (c = getchar())!= 0x0A) ;
 }
 
 if( (U26[0]&0x40) == 0)
 printf("lettura pin U26.PA6 OK (0x%02bx)\n",(U26[0]));
 else{
 printf("!!! ERRATA lettura su U26.PA6 (0x%02bx)\n",(U26[0]));
 err_cnt++ ;
 }
 esito_test_sw1 =0;
 esito_test_sw1 = test_SW1();
 
 if (err_cnt!=0  || esito_test_sw1) return 1; 
 return 0;  
 
 }
 
 
 
 unsigned char  test_SW1()
 {
 static unsigned char err_cnt;
 
 err_cnt =0;
 esito_test_sw1 =0;
 printf("\n\nTEST  SW1\n");
 
 printf("\nPosizionare SW1 su ON e ");
 printf("premi <INVIO>\n");while ( (c = getchar())!= 0x0A) ;
 
 if( (U26[0]& 0x80) ==0x80 )
 printf("rilettura su U26.PA7 OK (0x%02bx)\n",(U26[0]));
 else{
 printf("!!! ERRATA rilettura su U26.PA7 (0x%02bx)\n",(U26[0]));
 err_cnt=1 ;
 }
 
 printf("\nPosizionare SW1 su OFF e ");
 printf("premi <INVIO>\n");while ( (c = getchar())!= 0x0A) ;
 
 if( (U26[0]& 0x80 )!= 0x80)
 printf("rilettura su U26.PA7 OK (0x%02bx)\n",(U26[0]));
 else{
 printf("!!! ERRATA rilettura su U26.PA7 (0x%02bx)\n",(U26[0]));
 err_cnt=2 ;
 }
 
 return err_cnt;
 
 }
 
 unsigned char  test_U38()	   
 {
 static unsigned char err_cnt;
 printf("\n\nTEST U38\n");
 err_cnt = 0;
 
 buff_U38 = 0x00; 
 printf("Verificare accensione dei led U15 (U38 = 0x00) \n");
 
 if( U29[1] == 0xFF )
 printf("rilettura su U29.PB OK (0x%02bx)\n",(U29[1]));
 else{
 printf("!!! ERRATA rilettura su U29.PB (0x%02bx)\n",(U29[1]));
 err_cnt++;
 }
 printf("premi <INVIO>\n");while ( (c = getchar())!= 0x0A) ;
 
 buff_U38 = 0xFF; 
 printf("Verificare spegnimento dei led U15 (U38 = 0xff) \n");
 if( U29[1] == 0x00 )
 printf("rilettura su U29.PB OK (0x%02bx)\n",(U29[1]));
 else{
 printf("!!! ERRATA rilettura su U29.PB (0x%02bx)\n",(U29[1]));
 err_cnt++;
 }
 printf("premi <INVIO>\n");while ( (c = getchar())!= 0x0A) ;
 
 mask = 0x01;
 for(i=0; i<8; i++) 
 {
 buff_U38 = 0xFF; 
 buff_U38 = 0xFF & (~mask) ;   
 printf("Test U38.P%bd (0x%02bx) - Verificare accensione su U15 del LED %bd\n",(19-i),(0xFF & (~mask)),(i+2));
 if( U29[1] == mask)
 printf("rilettura su U29.PB OK (0x%02bx)\n",(U29[1]));
 else{
 printf("!!! ERRATA rilettura su U29.PB (0x%02bx)\n",(U29[1]));
 err_cnt++;
 }
 mask <<=1;
 printf("premi <INVIO>\n");while ( (c = getchar())!= 0x0A) ;
 }
 if (err_cnt!=0 ) return 1; 
 return 0;  
 
 }
 
 
 
 unsigned char  test_U15()  
 {
 static unsigned char err_cnt;
 err_cnt =0;
 
 printf("\n\nTEST U15 porta P1 \n");
 P1_3 = 1;
 P1_4 = 0;
 P1_5 = 0;
 P1_6 = 0;
  PIO_RESET = 1;
 printf("\nVerifica accensione U11 led 2");
 if(ins_stato_led()!=0) err_cnt++;
 
 
 P1_3 = 0;
 P1_4 = 1;
 printf("\nVerifica accensione U11 led 3");
 if(ins_stato_led()!=0) err_cnt++;
 
 
 P1_4 = 0;
 P1_5 = 1;
 printf("\nVerifica accensione U11 led 4");
 if(ins_stato_led()!=0) err_cnt++;
 
 
 P1_5 = 0;
 P1_6 = 1;
 printf("\nVerifica accensione U11 led 5");
 if(ins_stato_led()!=0) err_cnt++;
 
 
 P1_6 = 0;
 
 buff_U37 = 1;
 printf("\nVerifica accensione U11 led 6");
 if(ins_stato_led()!=0) err_cnt++;
 
 buff_U37 = 0;
 
  PIO_RESET = 1;
 printf("\nVerifica accensione U11 led 7");
  PIO_RESET = 0;
 if(ins_stato_led()!=0) err_cnt++;
 
 if (err_cnt!=0 ) return 1; 
 return 0;  
 }
 
 
 
 static unsigned char test_led_OFF()
 {
 esito_test_led_off =0;
 buff_U38 = 0xff;   
 
  for(del =0 ; del<200 ; del++);
 buff_U37 = 0;
 
 P1_3 = 0;
 P1_4 = 0;
 P1_5 = 0;
 P1_6 = 0;
  PIO_RESET = 0;
 
 
 U26[1] = 0xff;
 
 
 U26[2] = 0x00;
 
 
 U29[0] = 0xfe;
 
 
 printf("\n");
 printf("Verificare che tutti i led siano spenti tranne led 7 di U11\n");
 return ( ins_stato_led()); 
 
 
 }
 
 
 static unsigned char test_led_ON()
 {
 esito_test_led_on =0;
 buff_U38 = 0x00;   
 
 buff_U37 = 1;
 
 P1_3 = 1;
 P1_4 = 1;
 P1_5 = 1;
 P1_6 = 1;
  PIO_RESET = 0;
 
 
 U26[1] = 0x00;
 
 
 U26[2] = 0xff;
 
 
 U29[0] = 0x00;
 
 
 printf("\n");
 printf("Verificare che tutti i led siano accesi\n");
 return ( ins_stato_led()); 
 }
 
 















 
 unsigned char ins_stato_led()
 {
 char e;
 do{
 printf("\n(1=0K 0=KO ):");
 e = toupper(getchar());
 }
 while ((e != '1')&&(e != '0'));
 
 if (e == '0') return 1;  
 return 0;  
 
 }
 void test_pio ()      
 {
 printf("\n\n*****  Test I/O   *****   \n\n");
 error = 0;
 
 printf("\n");
 pio_reset();	 
 pio_conf();		 
 
 esito_test_led_on  = 0;
 esito_test_led_off = 0;
 esito_test_u26_pc  = 0;
 esito_test_u26_pb  = 0;
 esito_test_u29_pa  = 0;
 esito_test_u38     = 0;
 esito_test_u15     = 0;
 
 esito_test_led_on  = test_led_ON();
 esito_test_led_off = test_led_OFF();
 
 esito_test_u26_pc  = test_U26_PC();
 esito_test_u26_pb  = test_U26_PB();
 esito_test_u29_pa  = test_U29_PA();
 esito_test_u38     = test_U38();
 esito_test_u15     = test_U15();
 
 printf ("\n");
 if(!esito_test_led_on   &&
 !esito_test_led_off  &&
 !esito_test_u26_pb   &&
 !esito_test_u26_pc   &&
 !esito_test_u29_pa   &&
 !esito_test_u38      &&
 !esito_test_u15)
 printf("\nTest I/O OK\n");
 else
 { 
 printf("\nTest I/O KO !! \n" );
 if(esito_test_led_on)	printf ("Test led ON  KO\n");
 if(esito_test_led_off)  printf ("Test led OFF KO\n");
 if(esito_test_u26_pb)   printf ("Test U26 PB  KO\n");
 if(esito_test_u26_pc)   printf ("Test U26 PC  KO\n");
 if(esito_test_u29_pa)   printf ("Test U29 PA  KO\n");
 if(esito_test_u38)      printf ("Test U38     KO\n");
 if(esito_test_u15)		printf ("Test U15     KO\n");
 
 }
 printf("Premere un tasto per proseguire.\n");
 while ( (c = getchar()) !=0x0a) ;
 
 











































































































































































































 
 }
