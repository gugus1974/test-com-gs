/*****************************************************************************
*Sab.07.feb.04           v.1.00      sw_st\sw_stb\tststbxx\header\shd_var.h  *
******************************************************************************
* Definizioni variabili comuni a tutti i moduli scheda                       *
******************************************************************************
*                                                                            *
*****************************************************************************/

/* variabile per la definizione ddel tipo di scheda definite in sta03ini.c */
extern unsigned int sktype;

/* variabili per dimensione flash  definite in sta03ini.c */
extern unsigned int XHUGE * sctstr; 		 /*  limite inferiore indirizzi settore */
extern unsigned int  * sectlimit; 		 /*  limite superiore indirizzi settore */
extern unsigned long sctdelta;                   /*  delta incremento indirizzi settore */
extern unsigned long dimsct;                     /*  dimensione settore                 */
extern unsigned int  * flashlimit;          /*  limite superiore indirizzi 	*/

/* variabili per dimensione mvb bus definite in sta03ini.c */		 
/* variabili per dimensione mvb bus */		 
extern unsigned int  * saddmvb;      	 /* indirizzo di partenza dello spazio di memoria dell'MVBC */   
extern unsigned int  * eaddmvb;      	 /* indirizzo di fine dello spazio di memoria dell'MVBC     */ 
extern unsigned int  * regscr;       	 /* indirizzo del registro SCR dell'MVB     */ 
extern unsigned int  * regscr1;      	 /* indirizzo del registro SCR dell'MVB nel mode 'funmode'     */ 
extern unsigned int  * regmcr;       	 /* indirizzo del registro MCR dell'MVB     */ 
extern unsigned int  * regmcr1;      	 /* indirizzo del registro MCR dell'MVB nel mode 'funmode'     */ 
extern unsigned int  * regdr;        	 /* indirizzo del registro DR dell'MVB      */ 
extern unsigned int  * regstsr;      	 /* indirizzo del registro STSR dell'MVB    */ 
extern unsigned int  * regfc;        	 /* indirizzo del registro FC dell'MVB      */ 
extern unsigned int  * regec;        	 /* indirizzo del registro EC dell'MVB      */ 
extern unsigned int  * regmfr;       	 /* indirizzo del registro MFR dell'MVB     */ 
extern unsigned int  * regmfre;      	 /* indirizzo del registro MFRE dell'MVB    */ 
extern unsigned int  * regmr;        	 /* indirizzo del registro MR dell'MVB      */ 
extern unsigned int  * regmr2;       	 /* indirizzo del registro MR2 dell'MVB     */ 
extern unsigned int  * regdpr;       	 /* indirizzo del registro DPR dell'MVB     */ 
extern unsigned int  * regdpr2;      	 /* indirizzo del registro DPR2 dell'MVB    */ 
extern unsigned int  * regipr0;      	 /* indirizzo del registro IPR0 dell'MVB    */ 
extern unsigned int  * regipr1;      	 /* indirizzo del registro IPR1 dell'MVB    */ 
extern unsigned int  * regimr0;      	 /* indirizzo del registro IMR0 dell'MVB    */ 
extern unsigned int  * regimr1;      	 /* indirizzo del registro IMR1 dell'MVB    */ 
extern unsigned int  * regisr0;      	 /* indirizzo del registro ISR0 dell'MVB    */ 
extern unsigned int  * regisr1;      	 /* indirizzo del registro ISR1 dell'MVB    */ 
extern unsigned int  * regivr0;      	 /* indirizzo del registro IVR0 dell'MVB    */ 
extern unsigned int  * regivr1;      	 /* indirizzo del registro IVR1 dell'MVB    */ 
extern unsigned int  * regdaor;      	 /* indirizzo del registro DAOR dell'MVB    */ 
extern unsigned int  * regdaor1;     	 /* indirizzo del registro DAOR dell'MVB nel mode 'funmode'     */ 
extern unsigned int  * regdaok;      	 /* indirizzo del registro DAOK dell'MVB    */ 
extern unsigned int  * regtcr;       	 /* indirizzo del registro TCR dell'MVB     */ 
extern unsigned int  * regtr1;       	 /* indirizzo del registro TR1 dell'MVB     */ 
extern unsigned int  * regtr2;       	 /* indirizzo del registro TR2 dell'MVB     */ 
extern unsigned int  * regtc1;       	 /* indirizzo del registro TC1 dell'MVB     */ 
extern unsigned int  * regtc2;       	 /* indirizzo del registro TC2 dell'MVB     */ 
extern unsigned int  * regmfs;       	 /* indirizzo del registro MFS dell'MVB     */ 
extern unsigned long st_DA_PIT;                    /* indirizzo di partenza dei DA_PIT        */ 
extern unsigned long st_DA_PCS;                    /* indirizzo di partenza dei DA_PCS        */ 
extern unsigned long st_DA_DATA;                   /* indirizzo di partenza dei DA_DATA       */ 
extern unsigned long end_DA_PIT;            	 /* indirizzo di fine della DA_PIT          */ 
extern unsigned long end_DA_PCS;            	 /* indirizzo di fine dei DA_PCS            */ 
extern unsigned long st_LA_PIT;            	 /* indirizzo di partenza dei LA_PIT        */ 
extern unsigned long st_LA_PCS;            	 /* indirizzo di partenza dei LA_PCS        */ 
extern unsigned long end_LA_PIT;            	 /* indirizzo di fine della LA_PIT          */ 
extern unsigned long end_LA_PCS;            	 /* indirizzo di fine dei LA_PCS            */ 
extern unsigned long st_FC15_PCS;                  /* indirizzo di partenza dei FC15_PCS      */ 
extern unsigned long st_FC15_DATA0;                /* indirizzo FC15_DATA in pg0              */ 
extern unsigned long st_FC15_DATA1;                /* indirizzo FC15_DATA in pg1              */ 
extern unsigned int funmode;    		  /* Mode che determina la dimensione della TM dell'MVBC (3=256KB)*/     
extern unsigned int  * sasaddmvb;    	 /* indirizzo di partenza della service area dell'MVBC nel mode 'funmode'*/     
extern unsigned int  * saeaddmvb;    	 /* indirizzo di fine della service area dell'MVBC nel mode 'funmode'*/

