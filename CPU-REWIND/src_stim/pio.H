/*************************************************************************************/
/* 06/01/10              \SRC\pio.h								                     */
/*************************************************************************************/

#define PIO_U26_PA_ADDR	0x4000		
#define PIO_U29_PA_ADDR	0x2000
#define PA 0
#define PB 1
#define PC 2
#define CR 3

#define OK 1
#define KO 0
/*
#define PIO_U26_PC_ADDR	0x4002
#define PIO_U26_CR_ADDR	0x4003

#define PIO_U26_PB_ADDR	0x4001
#define PIO_U26_PC_ADDR	0x4002
#define PIO_U26_CR_ADDR	0x4003
*/
#define PIO_U26_CONF	0x90 //PA:in  PB:out PC:out	  !!!! scrivo 1001 0000  80h
													//     leggo  0110 1111	 6fh 
#define DELAY for(del =0 ; del<200 ; del++) 

/*
#define PIO_U29_PA_ADDR	0x2000
#define PIO_U29_PB_ADDR	0x2001
#define PIO_U29_PC_ADDR	0x2002
#define PIO_U29_CR_ADDR	0x2003
*/
#define PIO_U29_CONF	0x8B //	PA:out  PB:in PC:in !!!! scrivo 1000 1011  8bh
													//   leggo  0111 0100  74h 

sbit PIO_RESET = P1^7;
/*
typedef struct{
	unsigned char pA;
	unsigned char pB;
	unsigned char pC;
	unsigned char CR;
} t_pio;
*/
void test_pio(void);
void test_pio_menu(void);