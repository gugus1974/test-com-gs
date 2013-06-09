/*****************************************************************************
*Dom.08.feb.04           v.1.00      sw_st\sw_stb\tststbxx\header\handshk.h  *
******************************************************************************
* Definizioni macro per handshaking con scheda slave nel cestello            *
******************************************************************************
*                                                                            *
*****************************************************************************/

/* codice start TX */
#define STX 0x02			

/* codice end TX */
#define ETX 0x03			

/* codice ack */
#define ACK 0x06			

/* codice nack */
#define NACK 0x15			

/************************************** definizione dei codici di comando scambiati *********************************************/
/* codice per ordirare la trasmissione di un carattere su UART A */ 
#define  TX_UARTA 0x41
/* codice per ordirare la trasmissione di un carattere su UART B */ 
#define  TX_UARTB 0x42
/* codice per ordirare la trasmissione di un carattere su UART C */ 
#define  TX_UARTC 0x43
/* codice per ordirare la trasmissione di un carattere su UART D */ 
#define  TX_UARTD 0x44
/* codice per ordirare la ricezione di un carattere su UART A */ 
#define  RX_UARTA 0x45
/* codice per ordirare la ricezione di un carattere su UART A */ 
#define  RX_UARTB 0x46
/* codice per ordirare la ricezione di un carattere su UART A */ 
#define  RX_UARTC 0x47
/* codice per ordirare la ricezione di un carattere su UART A */ 
#define  RX_UARTD 0x48
/* codice per ordirare lo della seriale di controllo da (A-B) a (C-D)  */ 
#define  SWTSERC_D 0x49
/* codice per ordirare lo della seriale di controllo da (C-D) a (A-B)  */ 
#define  SWTSERA_B 0x54
/* codice per ordirare la scrittura di un  carattere su Port OUT1 */ 
#define  WRTOUT1 0x4A
/* codice per ordirare la scrittura di un  carattere su Port OUT2 */ 
#define  WRTOUT2 0x4B
/* codice per ordirare la scrittura di un  carattere su Port OUT3 */ 
#define  WRTOUT3 0x4C
/* codice per ordirare la scrittura di un  carattere su Port OUT4 */ 
#define  WRTOUT4 0x4D
/* codice per ordirare la lettura di un  carattere da Port IN1 */ 
#define  RDIN1 0x4E
/* codice per ordirare la lettura di un  carattere da Port IN2 */ 
#define  RDIN2 0x4F
/* codice per ordirare la lettura di un  carattere da Port IN3 */ 
#define  RDIN3 0x51
/* codice per ordirare la lettura di un  carattere da Port IN4 */ 
#define  RDIN4 0x52
/* codice inviato per risincronizzare */ 
#define  SINC 0x53


