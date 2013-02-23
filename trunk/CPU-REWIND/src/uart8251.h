/*************************************************************************************/
/* 18/01/10              \SRC\uart8251.h								                     */
/*************************************************************************************/

#define UART8251_U27_DATA_ADDR	0x6000
#define UART8251_U27_CTRL_ADDR	0x6001


#define UART8251_U30_DATA_ADDR	0x8000
#define UART8251_U30_CTRL_ADDR	0x8001


// mode register bits
// 
#define UART_STOP_MASK   0xc0
#define UART_2STOP       0xc0
#define UART_15STOP      0x80
#define UART_1STOP       0x40
#define UART_0STOP       0x00    // invalid

#define UART_PARITY_MASK 0x30
#define UART_EVEN_PARITY 0x20
#define UART_ODD_PARITY  0x10
#define UART_NO_PARITY   0x00

#define UART_BITS_MASK   0x0c
#define UART_8BITS       0x0c
#define UART_7BITS       0x08
#define UART_6BITS       0x04
#define UART_5BITS       0x00

#define UART_PRESCALER   0x03
#define UART_X64         0x03
#define UART_X16         0x02
#define UART_X1          0x01
#define UART_SYNC_MODE   0x00    // not implemented


// command register bits
// 
#define UART_EH          0x80    // enter hunt-mode: not implemented
#define UART_IR          0x40    // internal reset
#define UART_RTS         0x20    // 1: nRTS=0  0: nRTS=1
#define UART_ER          0x10    // reset error flags in status register
#define UART_SBRK        0x08    // send break, forces TXD low
#define UART_RXE         0x04    // receiver enable bit
#define UART_DTR         0x02    // 1: nDTR=0  1: nDTR=1
#define UART_TXE         0x01    // transmitter enable bit


// status register bits
// 
#define UART_DSR         0x80
#define UART_SYNDET_BD   0x40
#define UART_FE          0x20
#define UART_OE          0x10
#define UART_PE          0x08
#define UART_TXEMPTY     0x04
#define UART_RXRDY       0x02
#define UART_TXRDY       0x01




/*************************************************************************************/

void uart_reset();
void test_init();
void test_uartU27();
void test_uartU30();
void test_uartU27_rx_tx();
void test_uartU27_ricevi();
void test_uartU27_trasmetti();
/*************************************************************************************/
