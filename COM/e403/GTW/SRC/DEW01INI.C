/****************************************************************************************
* Sab.22.gen.05               v.1.00           \sw_dew\dew01\tstdew\source\dew01ini.c   *
*****************************************************************************************
*  Comandi di consolle per test scheda dew01                                            *
*****************************************************************************************
*                                                                                       *
****************************************************************************************/

#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "serbuf.h"
#include "libdew.h"      /* libreria prog.   */
#include "consolle.h"    /* gestore dei comandi da tastiera    */


#include "tstflash.h"   
#include "tstadc.h"
#include "tstpio.h" 
#include "tstmvb.h"
#include "tstaux.h"




/* prototipi funzioni dei comandi definiti nel modulo */

void gest_tflash(     int argc, char *argv[]);
void gest_tadc  (     int argc, char *argv[]);
void gest_tpio  (     int argc, char *argv[]);
void gest_tmvb  (     int argc, char *argv[]);
void gest_tser  (     int argc, char *argv[]);
void gest_ttem  (     int argc, char *argv[]);
void gest_report(     int argc, char *argv[]);
void gest_w     (     int argc, char *argv[]);

/************************* definizione variabili condivise dai vari moduli **********************************************/
/* versione programma */
unsigned char *vers6 ="tdew1 2.00 22/11/2011";
unsigned int sktype = 1;	

/* variabili per dimensione flash impostati ad 8 MB */
unsigned int XHUGE * sctstr    = (unsigned int  *)0x400000;   /*  limite inferiore indirizzi settore (unsigned int  *)0x400000; */
unsigned int  * sectlimit = (unsigned int  *)0xFE0000;   /*  limite superiore indirizzi settore (unsigned int  *)0xBE0000; */
unsigned long sctdelta          = 0x010000;                        /*  delta incremento indirizzi settore (unsigned int  *)0x010000; */
unsigned long dimsct            = 0x00FFFF;                        /*  dimensione settore settore (unsigned int  *)0x00FFFF; */
unsigned int  * flashlimit = (unsigned int  *)0xFFFFF;  /*  limite superiore indirizzi (unsigned int  *)0xBFFFFE; */
 
/* variabili per dimensione mvb bus */		 
unsigned int  * saddmvb = (unsigned int  *)0x200000;     /* indirizzo di partenza dello spazio di memoria dell'MVBC */   
unsigned int  * eaddmvb = (unsigned int  *)0x2FFFFE;     /* indirizzo di fine dello spazio di memoria dell'MVBC     */ 
unsigned int  * regscr  = (unsigned int  *)0x203F80;     /* indirizzo del registro SCR dell'MVB     */ 
unsigned int  * regscr1 = (unsigned int  *)0x20FF80;     /* indirizzo del registro SCR dell'MVB nel mode 'funmode'     */ 
unsigned int  * regmcr  = (unsigned int  *)0x203F84;     /* indirizzo del registro MCR dell'MVB     */ 
unsigned int  * regmcr1 = (unsigned int  *)0x20FF84;     /* indirizzo del registro MCR dell'MVB nel mode 'funmode'     */ 
unsigned int  * regdr   = (unsigned int  *)0x20FF88;     /* indirizzo del registro DR dell'MVB     */ 
unsigned int  * regstsr = (unsigned int  *)0x20FF8C;     /* indirizzo del registro STSR dell'MVB    */ 
unsigned int  * regfc   = (unsigned int  *)0x20FF90;     /* indirizzo del registro FC dell'MVB      */ 
unsigned int  * regec   = (unsigned int  *)0x20FF94;     /* indirizzo del registro EC dell'MVB      */ 
unsigned int  * regmfr  = (unsigned int  *)0x20FF98;     /* indirizzo del registro MFR dell'MVB     */ 
unsigned int  * regmfre = (unsigned int  *)0x20FF9C;     /* indirizzo del registro MFRE dell'MVB    */ 
unsigned int  * regmr   = (unsigned int  *)0x20FFA0;     /* indirizzo del registro MR dell'MVB      */ 
unsigned int  * regmr2  = (unsigned int  *)0x20FFA4;     /* indirizzo del registro MR2 dell'MVB     */ 
unsigned int  * regdpr  = (unsigned int  *)0x20FFA8;     /* indirizzo del registro DPR dell'MVB     */ 
unsigned int  * regdpr2 = (unsigned int  *)0x20FFAC;     /* indirizzo del registro DPR2 dell'MVB    */ 
unsigned int  * regipr0 = (unsigned int  *)0x20FFB0;     /* indirizzo del registro IPR0 dell'MVB    */ 
unsigned int  * regipr1 = (unsigned int  *)0x20FFB4;     /* indirizzo del registro IPR1 dell'MVB    */ 
unsigned int  * regimr0 = (unsigned int  *)0x20FFB8;     /* indirizzo del registro IMR0 dell'MVB    */ 
unsigned int  * regimr1 = (unsigned int  *)0x20FFBC;     /* indirizzo del registro IMR1 dell'MVB    */ 
unsigned int  * regisr0 = (unsigned int  *)0x20FFC0;     /* indirizzo del registro ISR0 dell'MVB    */ 
unsigned int  * regisr1 = (unsigned int  *)0x20FFC4;     /* indirizzo del registro ISR1 dell'MVB    */ 
unsigned int  * regivr0 = (unsigned int  *)0x20FFC8;     /* indirizzo del registro IVR0 dell'MVB    */ 
unsigned int  * regivr1 = (unsigned int  *)0x20FFCC;     /* indirizzo del registro IVR1 dell'MVB    */ 
unsigned int  * regdaor = (unsigned int  *)0x203FD8;     /* indirizzo del registro DAOR dell'MVB     */ 
unsigned int  * regdaor1= (unsigned int  *)0x20FFD8;     /* indirizzo del registro DAOR dell'MVB nel mode 'funmode'     */ 
unsigned int  * regdaok = (unsigned int  *)0x20FFDC;     /* indirizzo del registro DAOK dell'MVB    */ 
unsigned int  * regtcr  = (unsigned int  *)0x20FFE0;     /* indirizzo del registro TCR dell'MVB     */ 
unsigned int  * regtr1  = (unsigned int  *)0x20FFF0;     /* indirizzo del registro TR1 dell'MVB     */ 
unsigned int  * regtr2  = (unsigned int  *)0x20FFF4;     /* indirizzo del registro TR2 dell'MVB     */ 
unsigned int  * regtc1  = (unsigned int  *)0x20FFF8;     /* indirizzo del registro TC1 dell'MVB     */ 
unsigned int  * regtc2  = (unsigned int  *)0x20FFFC;     /* indirizzo del registro TC2 dell'MVB     */ 
unsigned int  * regmfs  = (unsigned int  *)0x20FF00;     /* indirizzo del registro MFS dell'MVB     */ 
unsigned long st_DA_PIT       = (unsigned long) 0x202000;            /* indirizzo di partenza dei DA_PIT        */ 
unsigned long st_DA_PCS       = (unsigned long) 0x238000;            /* indirizzo di partenza dei DA_PCS        */ 
unsigned long st_DA_DATA      = (unsigned long) 0x240000;            /* indirizzo di partenza dei DA_DATA       */ 
unsigned long end_DA_PIT      = (unsigned long) 0x203FFE;            /* indirizzo di fine della DA_PIT          */ 
unsigned long end_DA_PCS      = (unsigned long) 0x23FFFE;            /* indirizzo di fine dei DA_PCS            */ 
unsigned long st_LA_PIT       = (unsigned long) 0x200000;            /* indirizzo di partenza dei LA_PIT        */ 
unsigned long st_LA_PCS       = (unsigned long) 0x230000;            /* indirizzo di partenza dei LA_PCS        */ 
unsigned long end_LA_PIT      = (unsigned long) 0x201FFE;            /* indirizzo di fine della LA_PIT          */ 
unsigned long end_LA_PCS      = (unsigned long) 0x237FFE;            /* indirizzo di fine dei LA_PCS            */ 
unsigned long st_FC15_PCS     = (unsigned long) 0x20FE38;            /* indirizzo di partenza dei FC15_PCS      */ 
unsigned long st_FC15_DATA0   = (unsigned long) 0x20FC58;            /* indirizzo FC15_DATA in pg0              */ 
unsigned long st_FC15_DATA1   = (unsigned long) 0x20FC78;            /* indirizzo FC15_DATA in pg1              */ 
unsigned int funmode = 4;   					   /* Mode che determina la dimensione della TM dell'MVBC (4=1MB)*/     
unsigned int  * sasaddmvb = (unsigned int  *)0x20FC00;   /* indirizzo di partenza della service area dell'MVBC nel mode 'funmode'*/     
unsigned int  * saeaddmvb = (unsigned int  *)0x20FFFE;   /* indirizzo di fine della service area dell'MVBC nel mode 'funmode'*/
       


void tstbcom_init(void)
{
	consolle_addcmd("tflash", "",  "Test della flash", gest_tflash);
	consolle_addcmd("tadc",   "",  "Test degli ADC  ", gest_tadc);
	consolle_addcmd("tpio",   "",  "Test degli I/O port", gest_tpio);
	consolle_addcmd("tmvb",   "",  "Test del MVB    ", gest_tmvb);
	consolle_addcmd("tser",   "",  "Test delle Seriali", gest_tser);
	consolle_addcmd("ttemp",   "",  "Test del sensore di temperatura", gest_ttem);
	consolle_addcmd("report", "",  "Generazione del Test Report", gest_report);
	consolle_addcmd("w",      "",  "", gest_w);
}

  


/*********************************************/
/* comando tflash  -  test della flash       */
/*********************************************/
void gest_tflash  (int argc,char *argv[])
{
    
	argc = argc; argv = argv;
      
	tstflash();

	return;

}


/*********************************************/
/* comando tadc  -  test degli ADC           */
/*********************************************/
void gest_tadc  (int argc,char *argv[])
{
    
	argc = argc; argv = argv;

	test_adc();

	return;

}


/*********************************************/
/* comando tpio  -  test degli I/O Port      */
/*********************************************/
void gest_tpio  (int argc,char *argv[])
{
    
	argc = argc; argv = argv;

	test_pio();

	return;

}


/*********************************************/
/* comando tmvb  -  test del MVB             */
/*********************************************/
void gest_tmvb (int argc,char *argv[])
{
    
	argc = argc; argv = argv;

	test_mvb(); 

	return;

}

/*********************************************/
/* comando tsio  -  test del SIO BUS         */
/*********************************************/
void gest_tser  (int argc,char *argv[])
{
    
	argc = argc; argv = argv;

	test_ser();

	return;

}

/*********************************************/
/* comando ttem  -  test del sensore temp.   */
/*********************************************/
void gest_ttem  (int argc,char *argv[])
{
    
	argc = argc; argv = argv;

	test_tem();

	return;

}

/*********************************************/
/* comando report  -  generazione test report*/
/*********************************************/
void gest_report  (int argc,char *argv[])
{
    
	argc = argc; argv = argv;

	prn_report();

	return;

}

/*********************************************/
/* comando w  -  write program in flash      */
/*********************************************/
void gest_w  (int argc,char *argv[])
{
    
	argc = argc; argv = argv;

	w_cmd();

	return;

}

