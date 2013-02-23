/***************************************************************************/
/* 10/01/10 19.02.57        v.1.00          SRC\util.h     */
/***************************************************************************/
/* Funzioni di uso Comune                               */
/***************************************************************************/

#ifndef  _UTIL_H_
#define  _UTIL_H_


#ifndef EXTERN_UTIL
#define EXTERN_UTIL extern
#endif





/*********************************/
/* Define di Modulo              */
/*********************************/

/********************************************************/
/*      DEFINIZIONE   M A C R O  di  LOGICA             */
/********************************************************/
/*---------------------- Macro Utili ------------------------------------------------*/
#define BIN8(b7,b6,b5,b4,b3,b2,b1,b0) \
                ((b7<<7) |      \
                 (b6<<6) |      \
                 (b5<<5) |      \
                 (b4<<4) |      \
                 (b3<<3) |      \
                 (b2<<2) |      \
                 (b1<<1) |      \
                  b0          )

#define BIN16(b15,b14,b13,b12,b11,b10,b9,b8,b7,b6,b5,b4,b3,b2,b1,b0)    \
                ( (b15<<15) |   \
                  (b14<<14) |   \
                  (b13<<13) |   \
                  (b12<<12) |   \
                  (b11<<11) |   \
                  (b10<<10) |   \
                  (b9<<9)   |   \
                  (b8<<8)   |   \
                  (b7<<7)   |   \
                  (b6<<6)   |   \
                  (b5<<5)   |   \
                  (b4<<4)   |   \
                  (b3<<3)   |   \
                  (b2<<2)   |   \
                  (b1<<1)   |   \
                   b0             )

#define LEG(A)          Acc = (A)
#define AND(A)          Acc = Acc && (A)
#define OR(A)           Acc = Acc || (A)
#define STA1(A)         A   = Acc
#define STA0(A)         A   = !Acc

#define ONTD_N_NEW(A,n)   static unsigned int CN_##A[n],XN_##A[n]
#define ONTD_N_INIT(A,V,n)  {                       \
                               char i;              \
                               for (i=0; i<n; i++)  \
                               {                    \
                                   CN_##A[i]= V;    \
                                   XN_##A[i] = V;   \
                               }                    \
							 }


#define ONTD_NEW(A,V)   static unsigned int CN_##A = V, XN_##A = V
#define ONTD(A)         if (!Acc)               \
                            CN_##A = XN_##A;    \
                        else if (CN_##A)        \
                        {                       \
                            CN_##A--;           \
                            Acc=FALSO;          \
                        }

#define OFTD_NEW(A,V)   static unsigned int CF_##A = V, XF_##A = V
#define OFTD(A)         if ( Acc)               \
                            CF_##A = XF_##A;    \
                        else if (CF_##A)        \
                        {                       \
                            CF_##A--;           \
                            Acc=VERO;           \
                        }

#define COMPIST_NEW(A,SI,SS)    static int SI_##A=SI;       \
                                static int SS_##A=SS;       \
                                static BOOLEAN ST_##A=FALSO
#define COMPIST(A,PAR)  if (ST_##A)             \
                        {                       \
                            if (PAR<=SI_##A)    \
                                ST_##A=FALSO;   \
                        }                       \
                        else                    \
                        {                       \
                            if (PAR>=SS_##A)    \
                                ST_##A=VERO;    \
                        }

typedef unsigned char uchar;
typedef unsigned int  uint;
typedef unsigned short  ushort;

typedef unsigned char UCHAR;
typedef unsigned int  UINT;
typedef unsigned short  USHORT;



EXTERN_UTIL unsigned char set_char_bit(unsigned char *variabile, unsigned char posizione, unsigned char valore);
EXTERN_UTIL unsigned char set_int_bit(unsigned int *variabile,unsigned char posizione,unsigned char valore);

EXTERN_UTIL unsigned char get_char_bit(unsigned char *variabile, char posizione);
EXTERN_UTIL unsigned char get_int_bit(unsigned int *variabile, char posizione);
EXTERN_UTIL unsigned char get_long_bit(unsigned long *variabile, char posizione);

EXTERN_UTIL void print_char_bin(unsigned char variabile);
EXTERN_UTIL void print_int_bin(unsigned int valore);
EXTERN_UTIL void print_long_bin(unsigned long variabile);










#endif


