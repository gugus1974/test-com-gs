/*********************************************************************/
/* 2000.dic.22,ven          v.1.01          \SWISV\LIB\SRC\drvuart.h */
/*********************************************************************/

// Definizione puntatori UART
#define UART_A ((unsigned char xhuge *) 0X300700)
#define UART_B ((unsigned char xhuge *) 0X300B00)
#define UART_C ((unsigned char xhuge *) 0X300D00)
#define UART_D ((unsigned char xhuge *) 0X300E00)
  
// Indirizzo Registri Interni UART
#define RBR   0x0           /* Riceve Register */
#define THR   0x0           /* Trasmitt Register */
#define DLL   0x0           /* Divisor latch register (LSB) DLAB=1  */
#define DLM   0x1           /* Divisor latch register (MSB) DLAB=1  */
#define IER   0x1           /* Interrupt enable register */
#define FCR   0x2           /* Interrupt identification register (read only) */
#define IIR   0x2           /* Interrupt Status */
#define LCR   0x3           /* Line Control register */
#define MCR   0x4           /* Modem Control register */
#define LSR   0x5           /* Line Status register */
#define MSR   0x6           /* Modem Status register */
#define SCR   0x7           /* Scratch register */

// Campi Registro IER
#define _ERBI    0x01
#define _ETBEI   0x02
#define _ERLSI   0x04
#define _EDSSI   0x08

// Campi Registro FCR
#define _FEN     0x01
#define _RFR     0x02
#define _TFR     0x04
#define _DMA     0x08
#define _RFTL    0x40

// Campi Registro LCR
#define _WLS     0x01
#define _STB     0x04
#define _PEN     0x08
#define _EPS     0x10
#define _SPEN    0x20
#define _BEN     0x40
#define _DLAB    0x80

#define _EVEN    0x03
#define _ODD     0x01     
#define _NONE    0x00     

// Campi Registro MCR
#define _DTR     0x01
#define _RTS     0x02
#define _OUT2    0x08
#define _LOOP    0x10
#define _AFE     0x20

// Campi Registro LSR
#define _DR      0x01
#define _OE      0x02
#define _PE      0x04
#define _FE      0x08
#define _BI      0x10
#define _THRE    0x20
#define _TEMT    0x40
#define _RFER    0x80

// Campi Registro MSR
#define _DCTS    0x01
#define _DDSR    0x02
#define _TERI    0x04
#define _DDCD    0x08
#define _CTS     0x10
#define _DSR     0x20
#define _RI      0x40
#define _DCD     0x80

#define CLOCK_UART  16000000 
#define VERO   1
#define FALSO  0
#define LUNGHEZZA_CODA  100
 
// Prototipi funzioni definite nel modulo uart.c 

void drvUart_init(unsigned char xhuge * canale,
                  unsigned long int baud,
                  unsigned char bit_stop,
                  unsigned char parity);


void drvUart_inviaDati(unsigned char xhuge * canale,
                                unsigned char *dati,
                                unsigned short lunghezza);

void drvUart_riceviDati(unsigned char xhuge * canale,
                        unsigned char *dati,
                        unsigned short *lunghezza);


