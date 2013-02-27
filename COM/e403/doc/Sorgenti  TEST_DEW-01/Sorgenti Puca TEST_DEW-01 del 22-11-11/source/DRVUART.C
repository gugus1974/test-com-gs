/*********************************************************************/
/* 2001.mag.25,ven          v.1.07        \SWISV\LIB\SRC\drvUart.c   */
/*********************************************************************/

#include "stdio.h"
#include "string.h"     /* gestione stringhe */

#include "reg167.h"
#include "bitfield.h"
#include "intnum.h"
#include "drvuart.h"


// ------------------------------------
// Routine inizializzazione canale UART
// ------------------------------------
void drvUart_init(unsigned char xhuge * canale,
                  unsigned long int baud,
                  unsigned char bit_stop,
                  unsigned char parity)
{

    // BIT 11 Porta 2 in ingresso  segnale INTA UART
    DP2_11=0;
    // BIT 12 Porta 2 in ingresso  segnale INTB UART
    DP2_12=0;
    /* BIT 13 Porta 2 in ingresso  segnale INTC UART */
    DP2_13=0;
    /* BIT 14 Porta 2 in ingresso  segnale INTD UART */
    DP2_14=0;

    // --------------------------------------------------
    // FIFO Control Register (FCR)
    // +---+---+---+---+-----+-----+-----+-----+
    // | RFTL  | - | - | DMA | TFR | RFR | FEN |
    // +---+---+---+---+-----+-----+-----+-----+
    // 
    // FEN  = 1 --> Transmit and receive FIFOs enabled
    // RFR  = 0 --> Receiver    FIFO reset = 0
    // TFR  = 0 --> Transmitter FIFO reset = 0
    // DMA  = 1 --> RXRDY# and TXRDY# mode 1
    // RFTL = 0 --> Receiver FIFO Trigger Level = 1 bytes
    // --------------------------------------------------
    canale[FCR]  = 
                   1*_FEN +
                   0*_RFR +
                   0*_TFR +
                   1*_DMA +
                   0*_RFTL;

    // --------------------------------------------------
    // Line Control Register (LCR)
    // +------+-----+------+-----+-----+-----+--+--+
    // | DLAB | BEN | SPEN | EPS | PEN | STB | WLS |
    // +------+-----+------+-----+-----+-----+--+--+
    // 
    // WLS  = 3 --> Word length = 8 data bits
    // STB  = x --> Number of stop bits = user defined
    // PEN  = y --> Parity enable = user defined
    // EPS  = z --> Even-parity select = user defined
    // SPEN = 0 --> Stick parity disabled
    // BEN  = 0 --> Break disabled
    // DLAB = 1 --> Access to divisor latches enabled
    // --------------------------------------------------
    canale[LCR]  = 3*_WLS +
                   (bit_stop!=1)*_STB +
                   parity*_PEN +
                   0*_SPEN +
                   0*_BEN +
                   1*_DLAB;

    canale[DLL]  =  CLOCK_UART/(baud*16);
//    printf("\r\ndll = %02x\r\n",canale[DLL]);	

    canale[DLM]  = (CLOCK_UART/(baud*16)) >> 8;
//    printf("\r\ndlm = %02x\r\n",canale[DLM]);	

    // --------------------------------------------------
    // DLAB = 0 --> Access to receiver buffer enabled
    // --------------------------------------------------
    canale[LCR] &= ~_DLAB;


    // --------------------------------------------------
    // Interrupt Enable Register (IER)
    // +---+---+---+---+-------+-------+-------+------+
    // | 0 | 0 | 0 | 0 | EDSSI | ERLSI | ETBEI | ERBI |
    // +---+---+---+---+-------+-------+-------+------+
    // 
    // ERBI  = 0 --> Received data available interrupt disabled
    // ETBEI = 0 --> Transmitter holding register empty interrupt disabled
    // ERLSI = 0 --> Receiver line status interrupt disabled
    // EDSSI = 0 --> Modem status interrupt disabled   
    // --------------------------------------------------
    canale[IER]  = 
                   0*_ERBI  +
                   0*_ETBEI +
                   0*_ERLSI +
                   0*_EDSSI;


    // --------------------------------------------------
    // Modem Control Register (MCR)
    // +---+---+-----+------+------+---+-----+-----+
    // | 0 | 0 | AFE | LOOP | OUT2 | 0 | RTS | DTR |
    // +---+---+-----+------+------+---+-----+-----+
    // 
    // DTR  = 0 --> DTR# output forced high   
    // RTS  = 0 --> RTS# output forced high   
    // OUT2 = 1 --> External interrupt enabled   
    // LOOP = 0 --> Loop disabled
    // AFE  = 0 --> Autoflow control disabled
    // --------------------------------------------------
    canale[MCR]  = 0*_DTR  +
                   0*_RTS  +
                   1*_OUT2 +
                   0*_LOOP +
                   0*_AFE;


    // -------------------------------------
    // Azzeramento Scratchpad Register (SCR)
    // -------------------------------------
    canale[SCR]  = 0;
}


// --------------------
// Invia dati alla UART
// --------------------
void drvUart_inviaDati(unsigned char xhuge *canale,
                       unsigned char *dati,
                       unsigned short lunghezza)
{
    unsigned short indice=0;
//	unsigned long app;
//	unsigned int seg,off;

//				app = (unsigned long) canale;
//			off = (unsigned int)(app & 0x00FFFF);
//			app = app >> 16;
//			seg = (unsigned int)(app & 0x0000FF);
//			printf("\r\nseradd: %02x%04x\r\n",seg,off);

    while (indice < lunghezza)
      {
          canale[SCR]=VERO;
          if (canale[LSR]&_THRE){
//			  printf("%02x %02d ",dati[indice],indice);	
              canale[THR]=dati[indice++];
		  }
      }

    // Attendi che lo shift register di TX sia vuoto 
    while( (canale[LSR]& _TEMT)==0 );

}


// ----------------------
// Riceve dati dalla UART
// ----------------------
void drvUart_riceviDati(unsigned char xhuge * canale,
                        unsigned char *dati,
                        unsigned short *lunghezza)
{
    while(canale[LSR]&_DR)
    {
        dati[(*lunghezza)]=canale[RBR]; 
        if ((*lunghezza) < (LUNGHEZZA_CODA-1)) (*lunghezza)++;
    }


} 

