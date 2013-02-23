
#line 1 "src\Util.C" /0
 
 
 
 
 



 
 
  
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
 
 
 
#line 11 "src\Util.C" /0
 
  
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
 
 
#line 12 "src\Util.C" /0
 
 
  
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
 
 
 
 
 
 
 
 
 
 
 
 
 
#line 14 "src\Util.C" /0
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 unsigned char get_char_bit(unsigned char *variabile, char posizione)
 {
 int tmp;
 
 if (posizione<8) tmp=(*variabile>>posizione) & 0x01;
 else tmp=9;
 
 return (tmp);
 }
 






























































 
 void print_char_bin(unsigned char valore)
 {
 char i;
 
 for (i=7; i>=0; i--)
 {
 if (i==3) printf(" ");
 printf("%01bd",get_char_bit(&valore,i));
 }
 }
 























 
