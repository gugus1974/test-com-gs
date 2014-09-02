/*****************************************************************************
* Sab.14.feb.04           v.1.00       sw_st\sw_stb\tststbxx\source\tstmvb.c *
******************************************************************************
* Funzioni per il test del MVB BUS                                           *
******************************************************************************
*                                                                            *
*****************************************************************************/

#include "stdio.h"
//#include "shd_var.h"
#include "tstflash.h"   
#include "cod_st.h"
#include "hw_com.h"
//#pragma ot(3,SPEED)


unsigned short  * saddmvb       = (unsigned short *)0xC00000;     /* indirizzo di partenza dello spazio di memoria dell'MVBC */   
unsigned short  * eaddmvb       = (unsigned short *)0xCFFFFE;     /* indirizzo di fine dello spazio di memoria dell'MVBC     */ 
unsigned short  * sasaddmvb     = (unsigned short *)0xC0FC00;   /* indirizzo di partenza della service area dell'MVBC nel mode 'funmode'*/     
unsigned short  * saeaddmvb     = (unsigned short *)0xC0FFFE;   /* indirizzo di fine della service area dell'MVBC nel mode 'funmode'*/
unsigned short  * sasaddmvb1     = (unsigned short *)0xC03C00;   /* indirizzo di partenza della service area dell'MVBC nel mode 'funmode'*/     
unsigned short  * saeaddmvb1     = (unsigned short *)0xC03FFE;   /* indirizzo di fine della service area dell'MVBC nel mode 'funmode'*/
unsigned short funmode = 4;   					   /* Mode che determina la dimensione della TM dell'MVBC (4=1MB)*/     
unsigned short  * saeaddmvb1;
/************** definizioni registri MVB ****************************/
unsigned short  * regscr  = (unsigned short  *)0xC03F80;     /* indirizzo del registro SCR dell'MVB (MCM =0)    */ 
unsigned short  * regscr1 = (unsigned short  *)0xC0FF80;     /* indirizzo del registro SCR dell'MVB nel mode 'funmode'     */ 
unsigned short  * regmcr  = (unsigned short  *)0xC03F84;     /* indirizzo del registro MCR dell'MVB     */ 
unsigned short  * regmcr1 = (unsigned short  *)0xC0FF84;     /* indirizzo del registro MCR dell'MVB nel mode 'funmode'     */ 
unsigned short  * regdr   = (unsigned short  *)0xC0FF88;     /* indirizzo del registro DR dell'MVB     */ 
unsigned short  * regstsr = (unsigned short  *)0xC0FF8C;     /* indirizzo del registro STSR dell'MVB    */ 
unsigned short  * regfc   = (unsigned short  *)0xC0FF90;     /* indirizzo del registro FC dell'MVB      */ 
unsigned short  * regec   = (unsigned short  *)0xC0FF94;     /* indirizzo del registro EC dell'MVB      */ 
unsigned short  * regmfr  = (unsigned short  *)0xC0FF98;     /* indirizzo del registro MFR dell'MVB     */ 
unsigned short  * regmfre = (unsigned short  *)0xC0FF9C;     /* indirizzo del registro MFRE dell'MVB    */ 
unsigned short  * regmr   = (unsigned short  *)0xC0FFA0;     /* indirizzo del registro MR dell'MVB      */ 
unsigned short  * regmr2  = (unsigned short  *)0xC0FFA4;     /* indirizzo del registro MR2 dell'MVB     */ 
unsigned short  * regdpr1 = (unsigned short  *)0xC0FFA8;     /* indirizzo del registro DPR dell'MVB     */ 
unsigned short  * regdpr2 = (unsigned short  *)0xC0FFAC;     /* indirizzo del registro DPR2 dell'MVB    */ 
unsigned short  * regipr0 = (unsigned short  *)0xC0FFB0;     /* indirizzo del registro IPR0 dell'MVB    */ 
unsigned short  * regipr1 = (unsigned short  *)0xC0FFB4;     /* indirizzo del registro IPR1 dell'MVB    */ 
unsigned short  * regimr0 = (unsigned short  *)0xC0FFB8;     /* indirizzo del registro IMR0 dell'MVB    */ 
unsigned short  * regimr1 = (unsigned short  *)0xC0FFBC;     /* indirizzo del registro IMR1 dell'MVB    */ 
unsigned short  * regisr0 = (unsigned short  *)0xC0FFC0;     /* indirizzo del registro ISR0 dell'MVB    */ 
unsigned short  * regisr1 = (unsigned short  *)0xC0FFC4;     /* indirizzo del registro ISR1 dell'MVB    */ 
unsigned short  * regivr0 = (unsigned short  *)0xC0FFC8;     /* indirizzo del registro IVR0 dell'MVB    */ 
unsigned short  * regivr1 = (unsigned short  *)0xC0FFCC;     /* indirizzo del registro IVR1 dell'MVB    */ 
unsigned short  * regdaor = (unsigned short  *)0xC03FD8;     /* indirizzo del registro DAOR dell'MVB     */ 
unsigned short  * regdaor1= (unsigned short  *)0xC0FFD8;     /* indirizzo del registro DAOR dell'MVB nel mode 'funmode'     */ 
unsigned short  * regdaok = (unsigned short  *)0xC0FFDC;     /* indirizzo del registro DAOK dell'MVB    */ 
unsigned short  * regtcr  = (unsigned short  *)0xC0FFE0;     /* indirizzo del registro TCR dell'MVB     */ 
unsigned short  * regtr1  = (unsigned short  *)0xC0FFF0;     /* indirizzo del registro TR1 dell'MVB     */ 
unsigned short  * regtr2  = (unsigned short  *)0xC0FFF4;     /* indirizzo del registro TR2 dell'MVB     */ 
unsigned short  * regtc1  = (unsigned short  *)0xC0FFF8;     /* indirizzo del registro TC1 dell'MVB     */ 
unsigned short  * regtc2  = (unsigned short  *)0xC0FFFC;     /* indirizzo del registro TC2 dell'MVB     */ 
unsigned short  * regmfs  = (unsigned short  *)0xC0FF00;     /* indirizzo del registro MFS dell'MVB     */ 
unsigned long st_LA_PIT       = (unsigned long) 0xC00000;            /* indirizzo di partenza dei LA_PIT        */ 
unsigned long end_LA_PIT      = (unsigned long) 0xC01FFE;            /* indirizzo di fine della LA_PIT          */ 
unsigned long st_DA_PIT       = (unsigned long) 0xC02000;            /* indirizzo di partenza dei DA_PIT        */ 
unsigned long end_DA_PIT      = (unsigned long) 0xC03FFE;            /* indirizzo di fine della DA_PIT          */ 
unsigned long st_FC15_PCS     = (unsigned long) 0xC0FE38;            /* indirizzo di partenza dei FC15_PCS      */ 
unsigned long st_FC15_DATA0   = (unsigned long) 0xC0FC58;            /* indirizzo FC15_DATA in pg0              */ 
unsigned long st_FC15_DATA1   = (unsigned long) 0xC0FC78;            /* indirizzo FC15_DATA in pg1              */ 
unsigned long st_LA_PCS       = (unsigned long) 0xC30000;            /* indirizzo di partenza dei LA_PCS        */ 
unsigned long end_LA_PCS      = (unsigned long) 0xC37FFE;            /* indirizzo di fine dei LA_PCS            */ 
unsigned long st_DA_PCS       = (unsigned long) 0xC38000;            /* indirizzo di partenza dei DA_PCS        */ 
unsigned long end_DA_PCS      = (unsigned long) 0xC3FFFE;            /* indirizzo di fine dei DA_PCS            */ 
unsigned long st_DA_DATA      = (unsigned long) 0xC40000;            /* indirizzo di partenza dei DA_DATA       */ 

unsigned short  * st_DA_PITptr;
unsigned short  * st_DA_PCSptr;
unsigned short  * st_DA_DATA0ptr;
unsigned short  * st_DA_DATA1ptr;
unsigned short  * st_DA_PCSptr1;
unsigned short  * st_DA_DATA0ptr1;
unsigned short  * st_DA_DATA1ptr1;
unsigned short  * FC15_PCSptr;
unsigned short  * FC15_DATAptr;


/**********************************************************/
/*                                                        */
/*                                                        */
/*                                                        */
/*                                                        */
/**********************************************************/
//static short bt_cmd(short argc, char *argv[])
//{
//
//	/* parse the arguments */
//	i = 1;
//	if (--argc) {
//
//		if (argv[1][0] == 'A' || argv[1][0] == 'a') {
//			mvb_line = LC_MVB_LINE_CMD_SLA;
//			argc--; i++;
//		}
//		else if (argv[1][0] == 'B' || argv[1][0] == 'b') {
//			mvb_line = LC_MVB_LINE_CMD_SLB;
//			argc--; i++;
//		}
//
//		if (argc) {
//			mf = parse_number(argv[i++]);
//			if (--argc) {
//				count = parse_number(argv[i++]);
//
//				if (--argc) {
//
//					if (*argv[i] == '*') isr0_dbg_mask = DFLT_ISR0_DBG_MASK;
//					else isr0_dbg_mask = parse_number(argv[i]);
//					i++;
//
//					if (--argc) {
//						if (*argv[i] == '*') pcs1_dbg_mask = DFLT_PCS1_DBG_MASK;
//						else pcs1_dbg_mask = parse_number(argv[i]);
//						i++;
//
//						if (--argc) return -1;
//					}
//				}
//			}
//		}
//	}
//
//
//	/* clear the results */
//	pi_zero8((char*)ix, sizeof(ix));
//
//	pi_disable();							/* disable the bus administrator */
//
//	/* save what we are going to change */
//	save_scr = SCR;
//	save_imr0 = IMR0;
//	save_imr1 = IMR1;
//	pi_copy16((void*)save_msnk, (void*)MSNK, 4);
//
//	/* disable the bus administrator */
//	SCR &= ~TM_SCR_MAS;						/* clear the master bit         */
//	MR   = TM_MR_CSMF;						/* cancel sending master frames */
//	while (MR & TM_MR_BUSY);				/* wait for BUSY clear          */
//
//	/* wait for things to calm down (just to be sure) */
//	for (i = 0; i < 65535; i++);
//
//	/* enable what we need for our test */
//	SCR |= TM_SCR_UTS;						/* use test sink port           */
//	IMR0 = IMR1 = (short)0xFFFF;			/* enable all the interrupts    */
//
//	MFS = mf;								/* set the MF                   */
//	SCR |= TM_SCR_MAS;						/* set the master bit           */
//
//	/* initialize the test PCS */
//	MSNK[0] = (mf & TM_PCS_FCODE_MSK) |		/* F-code                       */
//			  TM_PCS0_SINK |		   		/* sink port                    */
//   			  TM_PCS0_TWCS |		   		/* transfer with check sequence */
//           	  TM_PCS0_WA;			   		/* write always                 */
//	MSNK[3] = 0;					   		/* check sequences = 0          */
//
//	/* clear any pending interrupt */
//	isr0 = ISR0; isr1 = ISR1;				/* read the interrupts          */
//	ISR0 = ISR1 = 0;						/* clear the flags              */
//	IVR0 = IVR1 = 0;						/* release the ISRi             */
//
//	/* clear the error and frame counters */
//	EC = FC = 0;
//
//	/* load the decoder register value */
//	dr = DR;
//
//	/* get the MVB line selection and choose ours */
//	get_mvb_line_selection(&tx_line, &rx_line);
//	select_mvb_line(LC_MVB_LINE_CMD_SLA | LC_MVB_LINE_CMD_SLB, mvb_line);
//	for (i = 0; i < 10000; i++);
//
//	for (i = 0; i < count; i ++) {
//
//		/* service the CPU watchdog (we are running with interrupts disabled) */
//		hw_watchdog_service();
//
//		/* send out the master frame if the low part is nonzero */
// 		if (mf & 0x0FFF) {
//	 		MR = TM_MR_SMFM;					/* set SMFM                */
//			while (MR & TM_MR_SMFM);			/* wait for SMFM clear     */
//  		}
//
//		/* reset the PCS status */
//		MSNK[1] = 0;					   		/* status = 0              */
//		MSNK[2] = 0;							/* TACK = 0                */
//
//		/* start the watchdog timer */
// 		TC2 = TR2;
//
//		/* wait for a frame */
//		f = FALSE;
//		do {
//
//			/* get the MVBC status */
//	 		isr0 = ISR0; isr1 = ISR1;			/* read the interrupts     */
//  			msnk1 = MSNK[1]; msnk2 = MSNK[2];	/* read the PCS            */
//
//			/* toggle the debug pin on error */
//   	   		if ((isr0 & isr0_dbg_mask) || (msnk1 & pcs1_dbg_mask)) {
//				debug_pin_on();
//				debug_pin_off();
//  			}
//
//   	 		/* release the interrupt registers */
//	  		ISR0 = ISR1 = 0;					/* clear the flags         */
//	  		IVR0 = IVR1 = 0;					/* release the ISRi        */
//
//			/* check for redundancy errors */
//			new_dr = DR;
//			if ((dr & 8) != (new_dr & 8)) ls_counter++; //LAA toggle(mean line switching)
//			if (new_dr & 4) rld_counter++;              //Bad data or silence detected on observed line
//			DR = 0;
//			dr = new_dr;
//
//			/* check for duplicated frames */
//			if (isr0 & TM_I0_DMF) dmf_counter++;    //two duplicate Master Frames have been received within Slave Frame reply time
//			if (isr0 & TM_I0_DSF) dsf_counter++;    //two duplicate Slave Frames have been received within the time a Slave Frame has been expected
//                                                    //within Slave Frame reply time
//
//			/* check for receive frame condition */
//			if ((isr0 & (TM_I0_SFC | TM_I0_EMF | TM_I0_ESF | TM_I0_RTI | TM_I0_BTI)) ||
//  				(isr1 & TM_I1_TI2)) f = TRUE;
//
//		} while (!f);
//
//		/* save the status */
//  		j = 0;
//   		if (isr0 & TM_I0_SFC)       j |= IX_SFC;
//   		if (isr0 & TM_I0_EMF)       j |= IX_EMF;
//   		if (isr0 & TM_I0_ESF)       j |= IX_ESF;
//   		if (isr0 & TM_I0_RTI)       j |= IX_RTI;
//   		if (j) {
//  	   		if (msnk2)                  j |= IX_TACK;
//	   		if (msnk1 & TM_PCS1_CRC)    j |= IX_CRC;
//   			if (msnk1 & TM_PCS1_SQE)    j |= IX_SQE;
//   			if (msnk1 & TM_PCS1_ALO)    j |= IX_ALO;
//   			if (msnk1 & TM_PCS1_BNI)    j |= IX_BNI;
//   			if (msnk1 & TM_PCS1_TERR)   j |= IX_TERR;
//   			if (msnk1 & TM_PCS1_STO)    j |= IX_STO;
//  			ix[j]++;
//   		}
//
//  		/* update the total count of ECs */
//   		ec_counter += EC;
//   		EC = 0;
//
//		/* workaround for MCU deadlock */
//		if (MR & TM_MR_BUSY) {
//			SCR = (SCR & ~TM_SCR_IL_RUNNING) | TM_SCR_IL_CONFIG;
//			SCR |= TM_SCR_IL_RUNNING;
//		}
// 	}
//
//	/* restore what we have changed */
//	pi_copy16((void*)MSNK, (void*)save_msnk, 4);
//	SCR = save_scr;
//	IMR0 = save_imr0;
//	IMR1 = save_imr1;
//	IPR0 = IPR1 = 0;
//	IVR0 = IVR1 = 0;						/* release the ISRi                */
//
//	/* restore the MVB line selection */
//	select_mvb_line(tx_line, rx_line);
//
//	pi_enable();							/* re-enable the bus administrator */
//
//	/* print the report */
//	printf("\n");
//	printf("Lines selected         : %s\n",
//		mvb_line == (LC_MVB_LINE_CMD_SLA | LC_MVB_LINE_CMD_SLB) ? "A+B" :
//		mvb_line == LC_MVB_LINE_CMD_SLA                         ? "A"   : "B");
//	printf("Master frame           : 0x%04x\n", mf);
//	printf("Count                  : %u\n", count);
//	printf("ISR0 debug signal mask : 0x%04x\n", isr0_dbg_mask);
//	printf("PCS1 debug signal mask : 0x%04x\n", pcs1_dbg_mask);
//	printf("\n");
//	printf("SFC EMF ESF RTI  CRC SQE ALO BNI TERR STO  TACK  count  ISR0 PCS1\n");
//	printf("--- --- --- ---  --- --- --- --- ---- ---  ----  -----  ---- ----\n");
//	for (i = 1; i < MAX_IX; i++) {
//		if (ix[i]) {
//			j = k = 0;
//
//			if (i & IX_SFC)  {printf("SFC ");  k |= TM_I0_SFC;}    else printf("    ");
//			if (i & IX_EMF)  {printf("EMF ");  k |= TM_I0_EMF;}    else printf("    ");
//			if (i & IX_ESF)  {printf("ESF ");  k |= TM_I0_ESF;}    else printf("    ");
//			if (i & IX_RTI)  {printf("RTI ");  k |= TM_I0_RTI;}    else printf("    ");
//			printf(" ");
//
//			if (i & IX_CRC)  {printf("CRC ");  j |= TM_PCS1_CRC;}  else printf("    ");
//			if (i & IX_SQE)  {printf("SQE ");  j |= TM_PCS1_SQE;}  else printf("    ");
//			if (i & IX_ALO)  {printf("ALO ");  j |= TM_PCS1_ALO;}  else printf("    ");
//			if (i & IX_BNI)  {printf("BNI ");  j |= TM_PCS1_BNI;}  else printf("    ");
//			if (i & IX_TERR) {printf("TERR "); j |= TM_PCS1_TERR;} else printf("     ");
//			if (i & IX_STO)  {printf("STO ");  j |= TM_PCS1_STO;}  else printf("    ");
//			printf(" ");
//
//			if (i & IX_TACK)  printf("TACK ");                     else printf("     ");
//			printf(" ");
//
//			printf("%5u  %04x %04x\n", ix[i], k, j);
//  		}
//	}
//	printf("\n");
//	printf("Error counter            = %u\n", ec_counter);
//	printf("Duplicated master frames = %u\n", dmf_counter);
//	printf("Duplicated slave frames  = %u\n", dsf_counter);
//	printf("Trusted line switches    = %lu\n", ls_counter);
//	printf("Redundant line disturbed = %lu\n", rld_counter);
//
//	return 0;
//}



/**********************************************************/
/* Funzione test ram TEST SHIFT DEGLI 1 TRA GLI 0 NEL DATO*/
/* Variabili di ingresso:                                 */
/*   startadd    indirizzo di start della ram             */
/*   endadd      indirizzo di fine della ram              */
/**********************************************************/
int shf0(unsigned short  * startadd, unsigned short  * endadd)
{
	unsigned short data3,vrf,tim1,i,k,dt;
	short  er;
	unsigned short  * pt_ram;
	unsigned int  off,seg;
	unsigned long app; 
	char c;

	printf("TEST SHIFT DEGLI 1 TRA GLI 0 NEL DATO da @%06lX  a @%06lX \n",startadd,endadd);
	er = 0;
	i=0;
	c = 0;
	/*** ciclo di scrittura e verifica su tutti gli indirizzi ***/
	for ( pt_ram = (startadd +1); pt_ram <= endadd; pt_ram++)
	{
	/***    procedura di start write     ***/
	     for (dt = 0x0001; dt != 0x0000; dt = dt << 1)
	     {		
    		*pt_ram = dt;
    		*(pt_ram-1)=0;
    		k = 10000;
    		tim1 = 0;
    		vrf = ((data3 = *pt_ram)  != dt);
    		printf("@%06lX \r",pt_ram );
            while( /*((c=_getkey())  != '\r') &&   vrf*/  ((data3 = *pt_ram)  != dt) ) {
                er = 1;
                if ( k == 10000)
                {
                    app = (unsigned long) pt_ram;
                    off = (unsigned int)(app & 0x00FFFF);
                    app = app >> 16;
                    seg = (unsigned int)(app & 0x0000FF);
                    printf("KO!!! Ad Address %02x%04x atteso %04x letto %04x\r",seg,off,dt,data3);
                    k = 0;
                    tim1 = 1;
                }
                k++;
                if ( c == '\r' ) break;
                *pt_ram = dt;
                data3 = *pt_ram;
            }
    		if ( tim1 ) printf("\n");
            if ( c == '\r' ) break;				
    		i++;
    		if (i==1024){
    			printf(".");
    			i = 0;
    		}
	    }
        if ( c == '\t' ) break;				
	}
	printf("\r\n");
	return er;
	/*
	if (er) printf("\r\nKO!!!!!!!!\r\n");
	else    printf("\r\nOK\r\n");
	*/
}



/**********************************************************/
/* Funzione test ram TEST SHIFT DEGLI 0 TRA GLI 1 NEL DATO*/
/* Variabili di ingresso:                                 */
/*   startadd    indirizzo di start della ram             */
/*   endadd      indirizzo di fine della ram              */
/**********************************************************/
int shf1(unsigned short  * startadd, unsigned short  * endadd)
{
	unsigned short data3,vrf,tim1,i,k,dt;
	short  er;
	unsigned short  * pt_ram;
	unsigned int  off,seg;
	unsigned long app; 
	char c;

	printf("TEST SHIFT DEGLI 0 TRA GLI 1 NEL DATO\n");
	er = 0;
	i=0;
	c = 0;
	/*** ciclo di scrittura e verifica su tutti gli indirizzi ***/
	for ( pt_ram = (startadd +1); pt_ram <= endadd; pt_ram++)
	{
	/***    procedura di start write     ***/
	    for (dt = 0x0001; dt != 0x0000; dt = dt << 1)
	    {		
            *pt_ram = ~dt;
            *(pt_ram-1)=0;
            k = 10000;
            tim1 = 0;
            
            vrf = ((data3 = *pt_ram)  != (unsigned short)(~dt));
            printf("@%06lX \r",pt_ram );
            while(/*((c=_getkey())  != '\r') &&  vrf*/  ((data3 = *pt_ram)  != (unsigned short)(~dt)) ) {
		        er = 1;
		        if ( k == 10000) {	   
			        app = (unsigned long) pt_ram;
        			off = (unsigned int)(app & 0x00FFFF);
        			app = app >> 16;
        			seg = (unsigned int)(app & 0x0000FF);
        			printf("\rKO!!! Ad Address %02x%04x atteso %04x letto %04x\r",seg,off,~dt,data3);
        			k = 0;
        			tim1 = 1;
        		}
        		k++;
		        if ( c == '\t' ) break; 
	            *pt_ram = ~dt;
		        data3 = *pt_ram;					
		    }
    		if ( tim1 ) printf("\r\n");
    		if ( c == '\t' ) break;	
    		i++;
    		if (i==1024){
    			printf(".");
    			i = 0;
    		}
	     }
             if ( c == '\t' ) break;
	}
	printf("\r\n");
	return er;
	/*
	if (er) printf("\r\nKO!!!!!!!!\r\n");
	else    printf("\r\nOK\r\n");
	*/
}



/**********************************************************/
/* Funzione test ram March C-                             */
/* Variabili di ingresso:                                 */
/*   startadd    indirizzo di start della ram             */
/*   endadd      indirizzo di fine della ram              */
/**********************************************************/
int march(unsigned short  * startadd, unsigned short  * endadd)
{
	unsigned short data3,tim1,i,k,dt,j,vrf;
	int  er;
	unsigned short  * pt_ram;
	unsigned short  off,seg;
	unsigned long app; 
	char c;

//	printf("TEST MARCH C-\r\n");
	er = 0;
	i=0;
	c = 0;
	/*** ciclo I  azzera dato ***/
	printf("Ciclo azzeramento RAM\r\n");
	for ( pt_ram = startadd; pt_ram <= endadd; pt_ram++) *pt_ram = 0;

	printf("Ciclo in cui si rilegge 0 e si scrive 1 bit per bit incrementando indirizzo\r\n");
	for ( pt_ram = startadd; pt_ram <= endadd; pt_ram++)
	{
	     j = 0;	
	     for (dt = 0x0001; dt != 0x0000; dt = dt << 1)
	     {		
    		k = 10000;
    		tim1 = 0;
    		vrf = ((data3 = (*pt_ram & dt)) != 0);
    		printf("@%06lX  %v\r",pt_ram, *pt_ram  );
    		while(/* ((c=_getkey())  != '\r') &&*/ vrf ) {
    		     er = 1;
    		     if ( k == 10000) 
    		     {	   
        			app = (unsigned long) pt_ram;
        			off = (unsigned short)(app & 0x00FFFF);
        			app = app >> 16;
        			seg = (unsigned short)(app & 0x0000FF);
        			printf("\rKO!!! Ad Address %02x%04x Bit %02d atteso 0 letto %04x",seg,off,j,data3);
        			k = 0;
        			tim1 = 1;
        		 }
    		     k++;
    		     if ( c == '\t' ) break;
    		     data3 = (*pt_ram );				
    		}
    		if ( tim1 ) printf("\r\n");
    		if ( c == '\t' ) break;
    		data3 = *pt_ram;
    		*pt_ram = data3 | dt;	
    		i++;
    		if (i==60000){
    			printf(".");
    			i = 0;
    		}
    		j++;
	    }
        if ( c == '\t' ) break;
	}
	printf("\r\n");

	printf("\r\nCiclo in cui si rilegge 1 e si scrive 0 bit per bit incrementando indirizzo\r\n");
	for ( pt_ram = startadd; pt_ram <= endadd; pt_ram++)
	{
	     j = 0;	
	     for (dt = 0x0001; dt != 0x0000; dt = dt << 1)
	     {		
		k = 10000;
		tim1 = 0;
		vrf = ((data3 = (*pt_ram & dt)) == 0);
   		printf("@%06lX  %04X\r",pt_ram,*(pt_ram)  );
		while( /*((c=_getkey())  != '\r') &&*/ vrf ) {
		     er = 1;
		     if ( k == 10000) 
		     {	   
			app = (unsigned long) pt_ram;
			off = (unsigned short)(app & 0x00FFFF);
			app = app >> 16;
			seg = (unsigned short)(app & 0x0000FF);
			printf("\rKO!!! Ad Address %02x%04x Bit %02d atteso 1 letto %04x",seg,off,j,data3);
			k = 0;
			tim1 = 1;
		     }
		     k++;
		     if ( c == '\t' ) break;				
		     data3 = (*pt_ram );
		}
		if ( tim1 ) printf("\r\n");
		if ( c == '\t' ) break;
		data3 = *pt_ram;
		*pt_ram = data3 & (~dt);	
		i++;
		if (i==60000){
			printf(".");
			i = 0;
		}
		j++;
	     }
             if ( c == '\t' ) break;
	}
	printf("\r\n");

	printf("\r\nCiclo in cui si rilegge 0 e si scrive 1 bit per bit decrementando indirizzo\r\n");
	for ( pt_ram = endadd; pt_ram >= startadd; pt_ram--)
	{
	     j = 0;	
	     for (dt = 0x8000; dt != 0x0000; dt = dt >> 1)
	     {		
		k = 10000;
		tim1 = 0;
		vrf = ((data3 = (*pt_ram & dt)) != 0);
   		printf("@%06lX  %04X\r",pt_ram,*(pt_ram)  );
		while(/*((c=_getkey())  != '\r') && */vrf ) {
		     er = 1;
		     if ( k == 10000) 
		     {	   
			app = (unsigned long) pt_ram;
			off = (unsigned short)(app & 0x00FFFF);
			app = app >> 16;
			seg = (unsigned short)(app & 0x0000FF);
			printf("\rKO!!! Ad Address %02x%04x Bit %02d atteso 0 letto %04x",seg,off,j,data3);
			k = 0;
			tim1 = 1;
		     }
		     k++;
		     if ( c == '\t' ) break;				
		     data3 = (*pt_ram );
		}
		if ( tim1 ) printf("\r\n");
		if ( c == '\t' ) break;
		data3 = *pt_ram;
		*pt_ram = data3 | dt;	
		i++;
		if (i==60000){
			printf(".");
			i = 0;
		}
		j++;
	     }
             if ( c == '\t' ) break;
	}
	printf("\r\n");

	printf("\r\nCiclo in cui si rilegge 1 e si scrive 0 bit per bit decrementando indirizzo\r\n");
	for ( pt_ram = endadd; pt_ram >= startadd; pt_ram--)
	{
	     j = 0;	
	     for (dt = 0x8000; dt != 0x0000; dt = dt >> 1)
	     {		
		k = 10000;
		tim1 = 0;
		vrf = ((data3 = (*pt_ram & dt)) == 0);
   		printf("@%06lX  %04X\r",pt_ram,*(pt_ram)  );
		while( ((c=_getkey())  != '\r') && vrf ) {
		     er = 1;
		     if ( k == 10000) 
		     {	   
			app = (unsigned long) pt_ram;
			off = (unsigned short)(app & 0x00FFFF);
			app = app >> 16;
			seg = (unsigned short)(app & 0x0000FF);
			printf("\rKO!!! Ad Address %02x%04x Bit %02d atteso 1 letto %04x",seg,off,j,data3);
			k = 0;
			tim1 = 1;
		     }
		     k++;
		     if ( c == '\t' ) break;				
		     data3 = (*pt_ram);
		}
		if ( tim1 ) printf("\r\n");
		if ( c == '\t' ) break;
		data3 = *pt_ram;
		*pt_ram = data3 & (~dt);	
		i++;
		if (i==60000){
			printf(".");
			i = 0;
		}
		j++;
	     }
             if ( c == '\t' ) break;
	}
	printf("\r\n");

	printf("\r\nCiclo in cui si rilegge 0 l'intera ram\r\n");
	for ( pt_ram = startadd; pt_ram <= endadd; pt_ram++)
	{
	     j = 0;	
	     for (dt = 0x0001; dt != 0x0000; dt = dt << 1)
	     {		
		k = 10000;
		tim1 = 0;
		vrf = ((data3 = (*pt_ram & dt)) != 0);
   		printf("@%06lX  %04X\r",pt_ram,*(pt_ram)  );
		while(  ((c=_getkey())  != '\r') && vrf ) {
		     er = 1;
		     if ( k == 10000) 
		     {	   
			app = (unsigned long) pt_ram;
			off = (unsigned short)(app & 0x00FFFF);
			app = app >> 16;
			seg = (unsigned short)(app & 0x0000FF);
			printf("\rKO!!! Ad Address %02x%04x Bit %02d atteso 0 letto %04x",seg,off,j,data3);
			k = 0;
			tim1 = 1;
		     }
		     k++;
		     if ( c == '\t' ) break;				
		     data3 = (*pt_ram);
		}
		if ( tim1 ) printf("\r\n");
		if ( c == '\t' ) break;
		i++;
		if (i==60000){
			printf(".");
			i = 0;
		}
		j++;
	     }
             if ( c == '\t' ) break;
	}
	printf("\r\n");

	return er;
/*	if (er) printf("\r\nKO!!!!!!!!\r\n");
	else    printf("\r\nOK\r\n");
*/
}


/**********************************************************/
/* Funzione test ram massimo rumore. Scrive in modo       */
/* alternato AAAA e 5555	                          */
/* Variabili di ingresso:                                 */
/*   startadd    indirizzo di start della ram             */
/*   endadd      indirizzo di fine della ram              */
/**********************************************************/
int maxrum(unsigned int  * startadd, unsigned int  * endadd)
{
	unsigned int data3,vrf,tim1,i,k,dt;
	int  er;
	unsigned int  * pt_ram;
	unsigned int  off,seg;
	unsigned long app; 
	char c;

	printf("TEST MASSIMO RUMORE\r\n");
	er = 0;
	i=0;
	c = 0;
	/*** ciclo di scrittura e verifica su tutti gli indirizzi ***/
	dt = 0xAAAA;
	*startadd = dt;
	for ( pt_ram = (startadd +1); pt_ram <= endadd; pt_ram++)
	{
		*pt_ram = ~dt;
		k = 10000;
		tim1 = 0;
		vrf = ((data3 = *(pt_ram-1))  != dt);
   		printf("@%06lX  %04X\r",pt_ram,*(pt_ram)  );
		while( ((c=_getkey())  != '\r') && vrf ) {
		     er = 1;
		     if ( k == 10000) 
		     {	   
			app = (unsigned long) pt_ram;
			off = (unsigned int)(app & 0x00FFFF);
			app = app >> 16;
			seg = (unsigned int)(app & 0x0000FF);
			printf("\rKO!!! Ad Address %02x%04x atteso %04x letto %04x",seg,off,dt,data3);
			k = 0;
			tim1 = 1;
		     }
		     k++;
		     if ( c == '\t' ) break;				
		     *pt_ram = ~dt;
		     data3 = *(pt_ram-1);
		}
		if ( tim1 ) printf("\r\n");
		if ( c == '\t' ) break;	
		i++;
		if (i==20000){
			printf(".");
			i = 0;
		}
		dt = ~dt;
	}
	printf("\r\n");

	dt = 0x5555;
	*startadd = dt;
	for ( pt_ram = (startadd +1); pt_ram <= endadd; pt_ram++)
	{
		*pt_ram = ~dt;
		k = 10000;
		tim1 = 0;
		vrf = ((data3 = *(pt_ram-1))  != dt);
   		printf("@%06lX  %04X\r",pt_ram,*(pt_ram)  );
		while( ((c=_getkey())  != '\r') && vrf ) {
		     er = 1;
		     if ( k == 10000) 
		     {	   
			app = (unsigned long) pt_ram;
			off = (unsigned int)(app & 0x00FFFF);
			app = app >> 16;
			seg = (unsigned int)(app & 0x0000FF);
			printf("\rKO!!! Ad Address %02x%04x atteso %04x letto %04x",seg,off,dt,data3);
			k = 0;
			tim1 = 1;
		     }
		     k++;
		     if ( c == '\t' ) break;				
		     *pt_ram = ~dt;
		     data3 = *(pt_ram-1);
		}
		if ( tim1 ) printf("\r\n");
		if ( c == '\t' ) break;	
		i++;
		if (i==20000){
			printf(".");
			i = 0;
		}
		dt = ~dt;
	}
	printf("\r\n");

	return er;
/*	if (er) printf("\r\nKO!!!!!!!!\r\n");
	else    printf("\r\nOK\r\n");
*/
}


/**********************************************************/
/* Funzione test ram shift degli 0                        */
/* Variabili di ingresso:                                 */
/*   startadd    indirizzo di start della ram             */
/*   endadd      indirizzo di fine della ram              */
/**********************************************************/
int shf0_mvb(unsigned short  * startadd, unsigned short  * endadd)
{
	unsigned short data3,tim1,i,j,k,dt,vrf,cnt;
	int  er;
	unsigned short  * pt_ram;
	unsigned int  off,seg;
	unsigned long app; 
	char c;

	saeaddmvb1 = saeaddmvb + 1;	
	printf("TEST SHIFT DEGLI 1 TRA GLI 0 NEL DATO\r\n");
	er = 0;
	i=0;
	c = 0;
	j=0;
	/*** ciclo di scrittura e verifica su tutti gli indirizzi ***/
//	for ( pt_ram = (startadd +1); pt_ram <= endadd; pt_ram++)
    for ( cnt = 1; cnt<10; cnt++)
    for ( j=0; j<16 ; j++)
	{
        pt_ram= startadd+(1<<j);

        /*salta la SA memory*/
        if (pt_ram == sasaddmvb1) pt_ram = saeaddmvb1;	
	    
	    /***    procedura di start write     ***/
	    for (dt = 0x0001; dt != 0x0000; dt = dt << 1)
	    {	
    		*pt_ram = dt;
    		if (pt_ram == saeaddmvb1) *(sasaddmvb-1) = 0;
    		else *(pt_ram-1)=0;
    		k = 10000;
    		tim1 = 0;
    		printf("@%06lX \r",pt_ram );
    		vrf = ((data3 = *pt_ram)  != dt);
            while( /*((c=_getkey())  != '\r') && */vrf ) {
                er = 1;
                if ( k == 10000)
                {
                    app = (unsigned long) pt_ram;
                    off = (unsigned int)(app & 0x00FFFF);
                    app = app >> 16;
                    seg = (unsigned int)(app & 0x0000FF);
                    printf("\rKO!!! Ad Address %02x%04x atteso %04x letto %04x",seg,off,dt,data3);
                    k = 0;
                    tim1 = 1;
                }
                k++;
                if ( c == '\t' ) break;
                    *pt_ram = dt;
                    data3 = *pt_ram;
                }
        		if ( tim1 ) printf("\r\n");
                if ( c == '\t' ) break;				
        		i++;
        		if (i==60000){
        			printf(".");
        			i = 0;
        		}
    	    }
            if ( c == '\t' ) 
	        {	
		        er = 1;
		        break;
	        }					
	    }
	printf("\n");
    return er;
}



/**********************************************************/
/* Funzione test ram shift degli 1                        */
/* Variabili di ingresso:                                 */
/*   startadd    indirizzo di start della ram             */
/*   endadd      indirizzo di fine della ram              */
/**********************************************************/
int shf1_mvb(unsigned short  * startadd, unsigned short  * endadd)
{
	unsigned short data3,tim1,i,j,k,dt,vrf,cnt;
	int  er;
	unsigned short  * pt_ram;
	unsigned int  off,seg;
	unsigned long app; 
	char c;

	saeaddmvb1 = saeaddmvb + 1;	
	printf("TEST SHIFT DEGLI 0 TRA GLI 1 NEL DATO\r\n");
	er = 0;
	i=0;
	c = 0;
	/*** ciclo di scrittura e verifica su tutti gli indirizzi ***/
//    for ( pt_ram = (startadd +1); pt_ram <= endadd; pt_ram++)
    j=0;
    for ( cnt = 1; cnt<10; cnt++)
    for ( j=0; j<16 ; j++)
    {
        pt_ram= startadd+(1<<j);

        if (pt_ram == sasaddmvb1) pt_ram = saeaddmvb1;
            /***    procedura di start write     ***/
        for (dt = 0x0001; dt != 0x0000; dt = dt << 1)
        {
            *pt_ram = ~dt;
            if (pt_ram == saeaddmvb1) *(sasaddmvb-1) = 0;
            else *(pt_ram-1)=0;
            k = 10000;
            tim1 = 0;
            printf("@%06lX \r",pt_ram );
            vrf = ((data3 = *pt_ram)  != (unsigned short)(~dt));
            while(/*((c=_getkey())  != '\r') && */vrf ) {
                er = 1;
                if ( k == 10000)
                {
                    app = (unsigned long) pt_ram;
                    off = (unsigned int)(app & 0x00FFFF);
                    app = app >> 16;
                    seg = (unsigned int)(app & 0x0000FF);
                    printf("\rKO!!! Ad Address %02x%04x atteso %04x letto %04x",seg,off,~dt,data3);
                    k = 0;
                    tim1 = 1;
                }
                k++;
                if ( c == '\t' ) break;
                *pt_ram = ~dt;
                data3 = *pt_ram;
            }
            if ( tim1 ) printf("\r\n");
            if ( c == '\t' ) break;
            i++;
            if (i==60000){
            printf(".");
            i = 0;
            }
        }
        if ( c == '\t' )
        {
            er = 1;
            break;
        }
    }
	printf("\r\n");
	return er;
}



/**********************************************************/
/* Funzione test ram March C-                             */
/* Variabili di ingresso:                                 */
/*   startadd    indirizzo di start della ram             */
/*   endadd      indirizzo di fine della ram              */
/**********************************************************/
int march_mvb(unsigned short  * startadd, unsigned short  * endadd)
{
	unsigned short data3,tim1,i,k,dt,j,vrf;
	int  er;
	unsigned short  * pt_ram;
	unsigned int  off,seg;
	unsigned long app; 
	char c;

	saeaddmvb1 = saeaddmvb + 1;	
	printf("TEST MARCH C-\r\n");
	er = 0;
	i=0;
	c = 0;
	/*** ciclo I  azzera dato ***/
	printf("Ciclo azzeramento RAM\r\n");
	for ( pt_ram = startadd; pt_ram <= endadd; pt_ram++) 
	{
	     if (pt_ram == sasaddmvb) pt_ram = saeaddmvb1;	
	     *pt_ram = 0;
	}
	printf("\r\n");

	printf("Ciclo in cui si rilegge 0 e si scrive 1 bit per bit incrementando indirizzo\r\n");
	for ( pt_ram = startadd; pt_ram <= endadd; pt_ram++)
	{
	     j = 0;	
	     if (pt_ram == sasaddmvb) pt_ram = saeaddmvb1;	
	     for (dt = 0x0001; dt != 0x0000; dt = dt << 1)
	     {		
		k = 10000;
		tim1 = 0;
		vrf =  ((((data3 = *pt_ram) & dt) != 0));
		while( ((c=_getkey())  != '\r') && vrf ) {
		     er = 1;	
		     if ( k == 10000) 
		     {	   
			app = (unsigned long) pt_ram;
			off = (unsigned int)(app & 0x00FFFF);
			app = app >> 16;
			seg = (unsigned int)(app & 0x0000FF);
			printf("\rKO!!! Ad Address %02x%04x Bit %02d atteso 0 letto %04x",seg,off,j,data3);
			k = 0;
			tim1 = 1;
		     }
		     k++;
		     if ( c == '\t' ) break;
		     data3 = (*pt_ram );				
		}
		if ( tim1 ) printf("\r\n");
		if ( c == '\t' ) break;
		data3 = *pt_ram;
		*pt_ram = data3 | dt;	
		i++;
		if (i==60000){
			printf(".");
			i = 0;
		}
		j++;
	     }
             if ( c == '\t' ) 
	     {	
		er = 1;
		break;
	     }					
	}
	printf("\r\n");

	printf("\r\nCiclo in cui si rilegge 1 e si scrive 0 bit per bit incrementando indirizzo\r\n");
	for ( pt_ram = startadd; pt_ram <= endadd; pt_ram++)
	{
	     j = 0;	
	     if (pt_ram == sasaddmvb) pt_ram = saeaddmvb1;	
	     for (dt = 0x0001; dt != 0x0000; dt = dt << 1)
	     {		
		k = 10000;
		tim1 = 0;
		vrf = ((((data3 = *pt_ram) & dt) == 0));
		while( ((c=_getkey())  != '\r') && vrf ) {
		     er = 1;	
		     if ( k == 10000) 
		     {	   
			app = (unsigned long) pt_ram;
			off = (unsigned int)(app & 0x00FFFF);
			app = app >> 16;
			seg = (unsigned int)(app & 0x0000FF);
			printf("\rKO!!! Ad Address %02x%04x Bit %02d atteso 1 letto %04x",seg,off,j,data3);
			k = 0;
			tim1 = 1;
		     }
		     k++;
		     if ( c == '\t' ) break;				
		     data3 = (*pt_ram );
		}
		if ( tim1 ) printf("\r\n");
		if ( c == '\t' ) break;
		data3 = *pt_ram;
		*pt_ram = data3 & (~dt);	
		i++;
		if (i==60000){
			printf(".");
			i = 0;
		}
		j++;
	     }
             if ( c == '\t' ) 
	     {	
		er = 1;
		break;
	     }					
	}
	printf("\r\n");

	printf("\r\nCiclo in cui si rilegge 0 e si scrive 1 bit per bit decrementando indirizzo\r\n");
	for ( pt_ram = endadd; pt_ram >= startadd; pt_ram--)
	{
	     j = 0;	
	     for (dt = 0x8000; dt != 0x0000; dt = dt >> 1)
	     {		
		k = 10000;
		tim1 = 0;
		vrf = ((((data3 = *pt_ram) & dt) != 0));
		while( ((c=_getkey())  != '\r') && vrf ) {
		     er = 1;	
		     if ( k == 10000) 
		     {	   
			app = (unsigned long) pt_ram;
			off = (unsigned int)(app & 0x00FFFF);
			app = app >> 16;
			seg = (unsigned int)(app & 0x0000FF);
			printf("\rKO!!! Ad Address %02x%04x Bit %02d atteso 0 letto %04x",seg,off,j,data3);
			k = 0;
			tim1 = 1;
		     }
		     k++;
		     if ( c == '\t' ) break;				
		     data3 = (*pt_ram );
		}
		if ( tim1 ) printf("\r\n");
		if ( c == '\t' ) break;
		data3 = *pt_ram;
		*pt_ram = data3 | dt;	
		i++;
		if (i==60000){
			printf(".");
			i = 0;
		}
		j++;
	     }
             if ( c == '\t' ) 
	     {	
		er = 1;
		break;
	     }					
	     if (pt_ram == saeaddmvb1) pt_ram = sasaddmvb;	
	}
	printf("\r\n");

	printf("\r\nCiclo in cui si rilegge 1 e si scrive 0 bit per bit decrementando indirizzo\r\n");
	for ( pt_ram = endadd; pt_ram >= startadd; pt_ram--)
	{
	     j = 0;	
	     for (dt = 0x8000; dt != 0x0000; dt = dt >> 1)
	     {		
		k = 10000;
		tim1 = 0;
		vrf = ((((data3 = *pt_ram) & dt) == 0));
		while( ((c=_getkey())  != '\r') && vrf ) {
		     er = 1;	
		     if ( k == 10000) 
		     {	   
			app = (unsigned long) pt_ram;
			off = (unsigned int)(app & 0x00FFFF);
			app = app >> 16;
			seg = (unsigned int)(app & 0x0000FF);
			printf("\rKO!!! Ad Address %02x%04x Bit %02d atteso 1 letto %04x",seg,off,j,data3);
			k = 0;
			tim1 = 1;
		     }
		     k++;
		     if ( c == '\t' ) break;				
		     data3 = (*pt_ram);
		}
		if ( tim1 ) printf("\r\n");
		if ( c == '\t' ) break;
		data3 = *pt_ram;
		*pt_ram = data3 & (~dt);	
		i++;
		if (i==60000){
			printf(".");
			i = 0;
		}
		j++;
	     }
             if ( c == '\t' ) 
	     {	
		er = 1;
		break;
	     }					
	     if (pt_ram == saeaddmvb1) pt_ram = sasaddmvb;	
	}
	printf("\r\n");

	printf("\r\nCiclo in cui si rilegge 0 l'intera ram\r\n");
	for ( pt_ram = startadd; pt_ram <= endadd; pt_ram++)
	{
	     if (pt_ram == sasaddmvb) pt_ram = saeaddmvb1;	
	     j = 0;	
	     for (dt = 0x0001; dt != 0x0000; dt = dt << 1)
	     {		
		k = 10000;
		tim1 = 0;
		vrf = ((((data3 = *pt_ram) & dt) != 0));
		while( ((c=_getkey())  != '\r') && vrf ) {
		     er = 1;	
		     if ( k == 10000) 
		     {	   
			app = (unsigned long) pt_ram;
			off = (unsigned int)(app & 0x00FFFF);
			app = app >> 16;
			seg = (unsigned int)(app & 0x0000FF);
			printf("\rKO!!! Ad Address %02x%04x Bit %02d atteso 0 letto %04x",seg,off,j,data3);
			k = 0;
			tim1 = 1;
		     }
		     k++;
		     if ( c == '\t' ) break;				
		     data3 = (*pt_ram);
		}
		if ( tim1 ) printf("\r\n");
		if ( c == '\t' ) break;
		i++;
		if (i==60000){
			printf(".");
			i = 0;
		}
		j++;
	     }
             if ( c == '\t' ) 
	     {	
		er = 1;
		break;
	     }					
	}
	printf("\r\n");

	return er;
}


/**********************************************************/
/* Funzione test ram massimo rumore. Scrive in modo       */
/* alternato AAAA e 5555	                          */
/* Variabili di ingresso:                                 */
/*   startadd    indirizzo di start della ram             */
/*   endadd      indirizzo di fine della ram              */
/**********************************************************/
int maxrum_mvb(unsigned short  * startadd, unsigned short  * endadd)
{
	unsigned short data3,tim1,i,k,dt,vrf;
	int  er;
	unsigned short  * pt_ram;
	unsigned short  * pt_ram1;
	unsigned int  off,seg;
	unsigned long app; 
	char c;

	saeaddmvb1 = saeaddmvb + 1;	
	printf("TEST MASSIMO RUMORE\r\n");
	er = 0;
	i=0;
	c = 0;
	/*** ciclo di scrittura e verifica su tutti gli indirizzi ***/
	dt = 0xAAAA;
	*startadd = dt;
	for ( pt_ram = (startadd +1); pt_ram <= endadd; pt_ram++)
	{
                pt_ram1 = pt_ram;	
	        if (pt_ram == sasaddmvb) pt_ram = saeaddmvb1;	
		*pt_ram = ~dt;
		k = 10000;
		tim1 = 0;
		vrf = (((data3 = *(pt_ram1-1))  != dt));
		while( ((c=_getkey())  != '\r') && vrf ) {
		     er = 1;	
		     if ( k == 10000) 
		     {	   
			app = (unsigned long) pt_ram;
			off = (unsigned int)(app & 0x00FFFF);
			app = app >> 16;
			seg = (unsigned int)(app & 0x0000FF);
			printf("\rKO!!! Ad Address %02x%04x atteso %04x letto %04x",seg,off,dt,data3);
			k = 0;
			tim1 = 1;
		     }
		     k++;
		     if ( c == '\t' ) break;				
		     *pt_ram = ~dt;
		     data3 = *(pt_ram1-1);
		}
		if ( tim1 ) printf("\r\n");
             if ( c == '\t' ) 
	     {	
		er = 1;
		break;
	     }					
		i++;
		if (i==20000){
			printf(".");
			i = 0;
		}
		dt = ~dt;
	}
	printf("\r\n");

	dt = 0x5555;
	*startadd = dt;
	for ( pt_ram = (startadd +1); pt_ram <= endadd; pt_ram++)
	{
                pt_ram1 = pt_ram;	
	        if (pt_ram == sasaddmvb) pt_ram = saeaddmvb1;	
		*pt_ram = ~dt;
		k = 10000;
		tim1 = 0;
		vrf = (((data3 = *(pt_ram1-1))  != dt)); 
		while( ((c=_getkey())  != '\r') && vrf ) {
		     er = 1;	
		     if ( k == 10000) 
		     {	   
			app = (unsigned long) pt_ram;
			off = (unsigned int)(app & 0x00FFFF);
			app = app >> 16;
			seg = (unsigned int)(app & 0x0000FF);
			printf("\rKO!!! Ad Address %02x%04x atteso %04x letto %04x",seg,off,dt,data3);
			k = 0;
			tim1 = 1;
		     }
		     k++;
		     if ( c == '\t' ) break;				
		     *pt_ram = ~dt;
		     data3 = *(pt_ram1-1);
		}
		if ( tim1 ) printf("\r\n");
             if ( c == '\t' ) 
	     {	
		er = 1;
		break;
	     }					
		i++;
		if (i==20000){
			printf(".");
			i = 0;
		}
		dt = ~dt;
	}
	printf("\r\n");

	return er;
}



/**********************************************************/
/* Funzione init MVB               */
/**********************************************************/
void MVB_init(void)
{
	unsigned short data1,i,vp;
	unsigned short  off,seg;
	unsigned long app; 
    


	*regscr1 = 0x0000; 						/* reset software				*/
	*regscr = 0x0000;
	for (i = 0; i<10; i++);								/* istruzione di attesa				*/
	*regscr = 0x8001;						/* predispone per l'inizializzazione dei reg.	*/
	*regmcr = funmode;						/* setta size della TM				*/
	for (i = 0; i<10; i++);								/* istruzione di attesa				*/
	
	/* azzera la LA port index table così tutti i LA port puntano al port 0 */
    for (st_DA_PITptr=(unsigned short  *) st_LA_PIT; st_DA_PITptr<=(unsigned short  *) end_LA_PIT; st_DA_PITptr++) *st_DA_PITptr = 0;	
	
	/* azzera tutti gli LA PCS per disattivare tutti gli LA port  */
    for (st_DA_PITptr=(unsigned short  *) st_LA_PCS; st_DA_PITptr<=(unsigned short  *) end_LA_PCS; st_DA_PITptr++) *st_DA_PITptr = 0;	
	
	/* azzera la DA port index table così tutti i DA port puntano al port 0 */
    for (st_DA_PITptr=(unsigned short  *) st_DA_PIT; st_DA_PITptr<=(unsigned short  *) end_DA_PIT; st_DA_PITptr++) *st_DA_PITptr = 0;	
	
	/* azzera tutti i DA PCS per disattivare tutti i DA port  */
    for (st_DA_PITptr=(unsigned short  *) st_DA_PCS; st_DA_PITptr<=(unsigned short  *) end_DA_PCS; st_DA_PITptr++) *st_DA_PITptr = 0;	
	
	*regdaor1 = 0x000D;
	data1 = *regdaor1;						/* legge il Device Address			*/ 
	
	*regdr = 0x0000;						/* consente lo switch tra le linee		*/
	printf ( "DR = %04x DA=%04x\r\n",*regdr,data1);
	
	*regdaok = 0x0094;						/* sovrascrive indirizzo hardware               */
	
	data1 = *regdaor1;						/* legge il Device Address			*/ 
	printf ( "DA=%04x\r\n",data1);
	*regimr0 = 0xFFFF;						/* abilita tutte le interrupt			*/
	*regimr1 = 0xFFFF;
	*regivr0 = 0;
	*regivr1 = 0;

     st_DA_PITptr = (unsigned short  *)(st_DA_PIT + 0x000E*2);
	*st_DA_PITptr = 0x0001;						/* utilizza il DA port n°1 per il proprio device add.*/

     st_DA_PITptr = (unsigned short  *)(st_DA_PIT + 0x000D*2);
	*st_DA_PITptr = 0x0002;						/* utilizza il DA port n°2 per il device add. della stim*/
 
     st_DA_PCSptr = (unsigned short  *)(st_DA_PCS + (0x0001<<3));
    *st_DA_PCSptr = 0xF420;		/* FC=15, sink port, no check seq., no write always, DTI1, no queue, no num data, no forcing data*/

	*(st_DA_PCSptr + 1) = 0x0000;
      st_DA_PCSptr1 = (unsigned short  *)(st_DA_PCS + (0x0002<<3));
  
     *st_DA_PCSptr1 = 0xF440;		/* FC=15, sink port, no check seq., no write always, DTI2, no queue, no num data, no forcing data*/
	*(st_DA_PCSptr1 + 1) = 0x0000;

	vp = 0;
	st_DA_DATA0ptr =  (unsigned short  *)(st_DA_DATA+(((0x0001<<4)&0xFFC0)|(vp<<5)|((0x0001<<3)&0x0018)));
	st_DA_DATA0ptr1 =  (unsigned short  *)(st_DA_DATA+(((0x0002<<4)&0xFFC0)|(vp<<5)|((0x0002<<3)&0x0018)));

	vp = 1;
	st_DA_DATA1ptr =  (unsigned short  *)(st_DA_DATA+(((0x0001<<4)&0xFFC0)|(vp<<5)|((0x0001<<3)&0x0018)));
	st_DA_DATA1ptr1 =  (unsigned short  *)(st_DA_DATA+(((0x0002<<4)&0xFFC0)|(vp<<5)|((0x0002<<3)&0x0018)));

	FC15_PCSptr = (unsigned short  *)st_FC15_PCS; 
	FC15_DATAptr = (unsigned short  *)st_FC15_DATA0;

    *FC15_PCSptr = 0xF860;		/* FC=15, source port, no check seq., no write always, DTI3, no queue, no num data, no forcing data*/
	*(FC15_PCSptr + 1) = 0x0000;
    *FC15_DATAptr = 0xAAAA;

   	set_out_port(0, DOP0_KMA|DOP0_KMB, DOP0_KMA|DOP0_KMB);

	*regscr1 = 0x8503;						/* abilito in full mode l'mvb	*/


	app = (unsigned long) st_DA_PITptr;
	off = (unsigned int)(app & 0x00FFFF);
	app = app >> 16;
	seg = (unsigned int)(app & 0x0000FF);
	printf("DA_PIT=%02x%04x\r\n",seg,off);
	app = (unsigned long) st_DA_PCSptr;
	off = (unsigned int)(app & 0x00FFFF);
	app = app >> 16;
	seg = (unsigned int)(app & 0x0000FF);
	printf("DA_PCS=%02x%04x\r\n",seg,off);
	app = (unsigned long) st_DA_DATA0ptr;
	off = (unsigned int)(app & 0x00FFFF);
	app = app >> 16;
	seg = (unsigned int)(app & 0x0000FF);
	printf("DA_DATA0=%02x%04x\r\n",seg,off);
	app = (unsigned long) st_DA_DATA1ptr;
	off = (unsigned int)(app & 0x00FFFF);
	app = app >> 16;
	seg = (unsigned int)(app & 0x0000FF);
	printf("DA_DATA1=%02x%04x\r\n",seg,off);
	app = (unsigned long) st_DA_PCSptr1;
	off = (unsigned int)(app & 0x00FFFF);
	app = app >> 16;
	seg = (unsigned int)(app & 0x0000FF);
	printf("DA_PCS1=%02x%04x\r\n",seg,off);
	app = (unsigned long) st_DA_DATA0ptr1;
	off = (unsigned int)(app & 0x00FFFF);
	app = app >> 16;
	seg = (unsigned int)(app & 0x0000FF);
	printf("DA_DATA10=%02x%04x\r\n",seg,off);
	app = (unsigned long) st_DA_DATA1ptr1;
	off = (unsigned int)(app & 0x00FFFF);
	app = app >> 16;
	seg = (unsigned int)(app & 0x0000FF);
//	printf("DA_DATA11=%02x%04x\r\n",seg,off);
////	printf("P2=%04x\r\n",P2);
//	printf("ISR0=%04x ISR1=%04x\r\n",*regisr0,*regisr1);
//	printf("FC=%04x EC=%04x\r\n",*regfc,*regec);
//	printf("PCSW0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_PCSptr,*(st_DA_PCSptr+1),*(st_DA_PCSptr+2),*(st_DA_PCSptr+3));
//	printf("DATA0W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_DATA0ptr,*(st_DA_DATA0ptr+1),*(st_DA_DATA0ptr+2),*(st_DA_DATA0ptr+3));
//	printf("DATA1W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_DATA1ptr,*(st_DA_DATA1ptr+1),*(st_DA_DATA1ptr+2),*(st_DA_DATA1ptr+3));
//	printf("PCS1W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_PCSptr1,*(st_DA_PCSptr1+1),*(st_DA_PCSptr1+2),*(st_DA_PCSptr1+3));
//	printf("DATA10W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_DATA0ptr1,*(st_DA_DATA0ptr1+1),*(st_DA_DATA0ptr1+2),*(st_DA_DATA0ptr1+3));
//	printf("DATA11W0-W1-W2-W3=%04x-%04x-%04x-%04x\r\n",*st_DA_DATA1ptr1,*(st_DA_DATA1ptr1+1),*(st_DA_DATA1ptr1+2),*(st_DA_DATA1ptr1+3));
    
    *regisr0=0;
	*regivr0=0;
	*regisr1=0;
	*regivr1=0;
 	
	*regfc=0;
	*regec=0;
	
	*st_DA_DATA0ptr=0;
	*(st_DA_DATA0ptr+1)=0;
	*(st_DA_DATA0ptr+2)=0;
	*(st_DA_DATA0ptr+3)=0;
	*st_DA_DATA1ptr=0;
	*(st_DA_DATA1ptr+1)=0;
	*(st_DA_DATA1ptr+2)=0;
	*(st_DA_DATA1ptr+3)=0;
	*st_DA_DATA0ptr1=0;
	*(st_DA_DATA0ptr1+1)=0;
	*(st_DA_DATA0ptr1+2)=0;
	*(st_DA_DATA0ptr1+3)=0;
	*st_DA_DATA1ptr1=0;
	*(st_DA_DATA1ptr1+1)=0;
	*(st_DA_DATA1ptr1+2)=0;
	*(st_DA_DATA1ptr1+3)=0;

}

/**********************************************************/
/* Funzione di test  del  mvb bus elettrico               */
/**********************************************************/
int t_mvbel (void)
{
	unsigned short data1,i,er,vp;
	unsigned long j;
	char c;
	static unsigned char   stato=0;
	static unsigned char   next_step=0;
	unsigned char   test_mvb_lineA,test_mvb_lineB;
	unsigned char   test_mvb_lineAOFF,test_mvb_lineBOFF;
	unsigned short  off,seg,p2val,fcval,ecval,isr0val,isr1val;
	unsigned long   app; 
	unsigned short  * st_DA_PITptr;
	unsigned short  * st_DA_PCSptr;
	unsigned short  * st_DA_DATA0ptr;
	unsigned short  * st_DA_DATA1ptr;
	unsigned short  * st_DA_PCSptr1;
	unsigned short  * st_DA_DATA0ptr1;
	unsigned short  * st_DA_DATA1ptr1;
	unsigned short  * FC15_PCSptr;
	unsigned short  * FC15_DATAptr;

	er = 0;

	*regscr1 = 0x0000; 						/* reset software funmode 4*/
	*regscr = 0x0000;                       /* reset software funmode 0*/

	for (i = 0; i<20; i++);					/* istruzione di attesa				*/
	
	*regscr = 0x0409;						/* Motorola Mode,Config Mode, TMO43.7us, MAS=1	,*/
	*regmcr = funmode;						/* write MCM 4				*/
	
	/* read MVBC type*/
	if ((*regmcr & 0xF800) == 0x0000)       printf("MVBC01 detected\n");
	else if ((*regmcr & 0xF800) == 0x1000)  printf("MVBC02A detected\n");
	else if ((*regmcr & 0xF800) == 0x1800)  printf("MVBC02B detected\n");
	else if ((*regmcr & 0xF800) == 0x2000)  printf("MVBC02C detected\n");
	    
	/* DA ORA MCM = 4*/
	for (i = 0; i<20; i++);								/* istruzione di attesa				*/
	
	/* azzera la LA port index table così tutti i LA port puntano al port 0 */
    for (st_DA_PITptr=(unsigned short  *) st_LA_PIT; st_DA_PITptr<=(unsigned short  *) end_LA_PIT; 
        st_DA_PITptr++) *st_DA_PITptr = 0;	
	
	/* azzera tutti gli LA PCS per disattivare tutti gli LA port  */
    for (st_DA_PITptr=(unsigned short  *) st_LA_PCS; st_DA_PITptr<=(unsigned short  *) end_LA_PCS; 
        st_DA_PITptr++) *st_DA_PITptr = 0;	
	
	/* azzera la DA port index table così tutti i DA port puntano al port 0 */
    for (st_DA_PITptr=(unsigned short  *) st_DA_PIT; st_DA_PITptr<=(unsigned short  *) end_DA_PIT; 
        st_DA_PITptr++) *st_DA_PITptr = 0;	
	
	/* azzera tutti i DA PCS per disattivare tutti i DA port  */
    for (st_DA_PITptr=(unsigned short  *) st_DA_PCS; st_DA_PITptr<=(unsigned short  *) end_DA_PCS; 
        st_DA_PITptr++) *st_DA_PITptr = 0;	
	
	data1 = *regdaor1;						/* legge il Device Address			*/ 
	*regdr = 0x0009;						/* LAA=1 SLM=1 blocca lo switch tra le linee LAA=1 SLM=1		*/
	
//	printf ( "DR = %04x DA=%04x\n",*regdr,data1);  
	
	*regdaor1 = 0x0001;                     /* sovrascrive Device address con 0x0001*/
	*regdaok = 0x0094;						/* 94 sovrascrive indirizzo hardware            */
	                                        /* 49 disabilita override indirizzo hardware    */
	data1 = *regdaor1;						/* legge il Device Address			            */ 
	printf ( "DA=%04x\n",data1);
	*regimr0 = 0xFFFF;						/* abilita tutte le interrupt			*/
	*regimr1 = 0xFFFF;
	*regivr0 = 0;
	*regivr1 = 0;
	
	
	/* Set TM for status report */
	FC15_PCSptr = (unsigned short  *)st_FC15_PCS; 
	
	FC15_DATAptr = (unsigned short  *)st_FC15_DATA0;

//    *FC15_PCSptr = 0xF860;		/* FC=15, source port, no check seq., no write always, DTI3, no queue, no num data, no forcing data*/
    *FC15_PCSptr = 0xF802;		/* FC=15, source port, no check seq., no write always, DTI3, no queue, no num data, no forcing data*/

	*(FC15_PCSptr + 1) = 0x0000;

    *FC15_DATAptr = 0x0001;
		
	*regmfs =  0xF001;       /* master frame di richiesta proprio stato*/ 
	printf("set MF %04xH\n",*regmfs); /*  */            
	
	*regmr = 0;                     /*  */

	/* connect to the MVB line A*/
	set_out_port(0, DOP0_KMA|DOP0_KMB, DOP0_KMA);

	*regscr1 = 0x050B;						/* predispone per l'inizializzazione dei reg.	*/

	for (i = 0; i<30000; i++);					/* istruzione di attesa				*/

    *regisr0=0;
	*regivr0=0;
	*regisr1=0;
	*regivr1=0;
	*regfc=0;
	*regec=0;
	*st_DA_DATA0ptr=0;
	*(st_DA_DATA0ptr+1)=0;
	*(st_DA_DATA0ptr+2)=0;
	*(st_DA_DATA0ptr+3)=0;
	*st_DA_DATA1ptr=0;
	*(st_DA_DATA1ptr+1)=0;
	*(st_DA_DATA1ptr+2)=0;
	*(st_DA_DATA1ptr+3)=0;
	*st_DA_DATA0ptr1=0;
	*(st_DA_DATA0ptr1+1)=0;
	*(st_DA_DATA0ptr1+2)=0;
	*(st_DA_DATA0ptr1+3)=0;
	*st_DA_DATA1ptr1=0;
	*(st_DA_DATA1ptr1+1)=0;
	*(st_DA_DATA1ptr1+2)=0;
	*(st_DA_DATA1ptr1+3)=0;

    stato = 0;
    test_mvb_lineA = 0;
    test_mvb_lineAOFF = 0;
    test_mvb_lineB = 0;
    test_mvb_lineBOFF = 0;
    

	for (i = 0; i<20000; i++) ;					/* istruzione di attesa				*/

	while( stato != 10 )	
	{
	    switch (stato)
	    {
	        default:
	        case 0:// AVVIA TEST SU LINEA A
            	printf("\n\nPROVA TRASMISSIONE CON RELE' CHIUSI SULLA LINEA A:\n\n");
            	printf("Verificare con un oscilloscopio che tra i pin 1 e 2 del connettore MVB1\n");
            	printf("della scheda EMD sia presente un segnale di trasmissione seriale con   \n");
            	printf("bit rate di 1 usec e ampiezza co mpresa tra 2 e 12 V p.p.\n\n");
            	printf("Premere 1 se la forma d'onda sulla linea A tra i pin 1 e 2 è corretta altrimenti premere 0\n");
               	set_out_port(0, DOP0_KMA|DOP0_KMB, DOP0_KMA);
	            stato = 1;
	           break;
	        
	        case 1:// AVVIA TRASMISSIONE ED ATTENDE CARATTERE DI ESITO TEST TEST SU LINEA A
               	set_out_port(0, DOP0_KMA|DOP0_KMB, DOP0_KMA);
        		//while (((c=sio_poll_key(2))!= '0') && ((c=sio_poll_key(2))!= '1') && ((*regmr & 0x0200) != 0))
           		c=0x00;
           		while ((c!= '0') && (c!= '1') && ((*regmr & 0x0200) != 0))
        		{ 
        		    c=sio_poll_key(2);
        		} 
                *FC15_DATAptr = *FC15_DATAptr + 1;
        		*regmr=0x0020;/* set SMSM :invia master frame manualmente */

//                if (strcmp(c,"1")==0)  
                if ( c == '1')  
                {
                    test_mvb_lineA = 1;//OK
                    stato = 2;
//                            save_stato(TESTMVB_LINE_A_OK);// Test TX MVB su LINEA  A OK
                }
//                else if (strcmp(c,"0")==0)  
                else if ( c == '0')   
                {
                    test_mvb_lineA = 0;//KO
                    stato = 2;
//                            save_stato(TESTMVB_LINE_A_KO);// Test TX MVB su LINEA  A KO    
                }
                else 
                    stato = 1;
	           break;

	        case 2://PROVA ISOLAMENTO LINEA A
                printf("\n\nPROVA ISOLAMENTO LINEA A\n\n");    
            	printf("Verificare con un oscilloscopio che tra i pin 1 e 2 dei connettori CN1 e CN2\n");
            	printf("della scheda EMD non sia presente alcun segnale\n\n");
                printf("\nPremere 1 se la trasmissione sulla linea A tra i pin 1 e 2 si interrompe altrimenti premere 0\n");    
                stato = 3;
	           break;

	        case 3: //ATTENDE CARATTERE DI ESITO TEST SU LINEA A
              	set_out_port(0, DOP0_KMA|DOP0_KMB, 0);

/*
        		while (((c=sio_poll_key(2))!= '0') && ((c=sio_poll_key(2))!= '1') && ((*regmr & 0x0200) != 0))
        		{ 
        		} 
*/
           		c=0x00;
           		while ((c!= '0') && (c!= '1') && ((*regmr & 0x0200) != 0))
        		{ 
        		    c=sio_poll_key(2);
        		} 

                *FC15_DATAptr = *FC15_DATAptr + 1;
        		*regmr=0x0020;					/* set SMSM :invia master frame manualmente */

                if ( c == '1')  
                {
                    test_mvb_lineAOFF = 1;//OK
                    stato = 4;
//                  save_stato(TESTMVB_LINE_A_OFF_OK);// Test TX MVB su LINEA  A OK
                }
                else if ( c == '0')  
                {
                    test_mvb_lineAOFF = 0;//KO
                    stato = 4;
//                  save_stato(TESTMVB_LINE_A_OFF_KO);// Test TX MVB su LINEA  A KO    
                }
                else 
                    stato = 3;

	           break;

	        case 4:// AVVIA TEST SU LINEA B
	               
            	printf("\n\nPROVA TRASMISSIONE CON RELE' CHIUSI SULLA LINEA B:\n\n");
            	printf("Verificare con un oscilloscopio che tra i pin 4 e 5 del connettore MVB1\n");
            	printf("della scheda EMD sia presente un segnale di trasmissione seriale con   \n");
            	printf("bit rate di 1 usec e ampiezza co mpresa tra 2 e 12 V p.p.\n\n");
            	printf("\nPremere 1 se la forma d'onda sulla linea B tra i pin 4 e 5 è corretta altrimenti premere 0\n");
              	set_out_port(0, DOP0_KMA|DOP0_KMB, DOP0_KMB);
	            stato = 5;
	           break;
	        
	        case 5:// AVVIA TRASMISSIONE ED ATTENDE CARATTERE DI ESITO TEST TEST SU LINEA 5
              	set_out_port(0, DOP0_KMA|DOP0_KMB, DOP0_KMB);
/*
        		while (((c=sio_poll_key(2))!= '0') && ((c=sio_poll_key(2))!= '1') && ((*regmr & 0x0200) != 0))
        		{ 
        		} 

*/
           		c=0x00;
           		while ((c!= '0') && (c!= '1') && ((*regmr & 0x0200) != 0))
        		{ 
        		    c=sio_poll_key(2);
        		} 

                *FC15_DATAptr = *FC15_DATAptr + 1;
        		*regmr=0x0020;					/* set SMSM :invia master frame manualmente */

                if ( c == '1')  
                {
                    test_mvb_lineB = 1;//OK
                    stato = 6;
//                  save_stato(TESTMVB_LINE_B_OK);// Test TX MVB su LINEA B OK
                }
                else if ( c == '0')  
                {
                    test_mvb_lineB = 0;//KO
                    stato = 6;
//                  save_stato(TESTMVB_LINE_B_KO);// Test TX MVB su LINEA B KO    
                }
                else 
                    stato = 5;
	           break;
	        
	        case 6://PROVA ISOLAMENTO LINEA B
              	set_out_port(0, DOP0_KMA|DOP0_KMB, 0);
               	printf("\n\nPROVA ISOLAMENTO LINEA B\n\n");    
            	printf("Verificare con un oscilloscopio che tra i pin 4 e 5 dei connettori CN1 e CN2\n");
            	printf("della scheda EMD non sia presente alcun segnale\n\n");
                printf("\nPremere 1 se la trasmissione sulla linea B tra i pin 4 e 5 si interrompe altrimenti premere 0\n");    
                stato = 7;
	           break;

	        case 7: //ATTENDE CARATTERE DI ESITO TEST SU LINEA B
              	set_out_port(0, DOP0_KMA|DOP0_KMB, 0);
/*
        		while (((c=sio_poll_key(2))!= '0') && ((c=sio_poll_key(2))!= '1') && ((*regmr & 0x0200) != 0))
        		{ 
        		} 
*/
           		c=0x00;
           		while ((c!= '0') && (c!= '1') && ((*regmr & 0x0200) != 0))
        		{ 
        		    c=sio_poll_key(2);
        		} 

                *FC15_DATAptr = *FC15_DATAptr + 1;
        		*regmr=0x0020;					/* set SMSM :invia master frame manualmente */

                if ( c == '1') 
                {
                    test_mvb_lineBOFF = 1;//OK
                    stato = 8;
//                  save_stato(TESTMVB_LINE_B_OFF_OK);// Test TX MVB su LINEA B OK
                }
                else if ( c == '0')  
                {
                    test_mvb_lineBOFF = 0;//KO
                    stato = 8;
//                  save_stato(TESTMVB_LINE_B_OFF_KO);// Test TX MVB su LINEA B KO    
                }
                else 
                    stato = 7;
	           break;

	        case 8: //SALVA ESITO TEST
	                
	                printf("\n\nESITO TEST:\n");
	                if (test_mvb_lineA) save_stato(TESTMVB_LINE_A_OK);// Test TX MVB su LINEA A OK
	                else                save_stato(TESTMVB_LINE_A_KO);// Test TX MVB su LINEA A KO
	                
	                if (test_mvb_lineAOFF)  save_stato(TESTMVB_LINE_A_OFF_OK);// Test ISOL MVB su LINEA A OK
	                else                    save_stato(TESTMVB_LINE_A_OFF_KO);// Test ISOL MVB su LINEA A KO

	                if (test_mvb_lineB) save_stato(TESTMVB_LINE_B_OK);// Test TX MVB su LINEA B OK
	                else                save_stato(TESTMVB_LINE_B_KO);// Test TX MVB su LINEA B KO
	            
	                if (test_mvb_lineBOFF)  save_stato(TESTMVB_LINE_B_OFF_OK);// Test ISOL MVB su LINEA B OK
	                else                    save_stato(TESTMVB_LINE_B_OFF_KO);// Test ISOL MVB su LINEA B KO
                    stato = 10;
                    
                    if (test_mvb_lineA && test_mvb_lineAOFF && test_mvb_lineB && test_mvb_lineBOFF)
                        er=0;
                    else
                        er=1;
                    
	            break;
	        case 10: 
	            break;
	    
	    }
	
	
	
	}
//
//    if (strcmp(c,'1')==0)  {
//        test_mvb_lineA = 1;//OK
//        save_stato(TESTMVB_LINE_A_OK);// Test TX MVB su LINEA  A OK
//    }
//    if (strcmp(c,'0')==0)  {
//        test_mvb_lineA = 0;//KO
//        save_stato(TESTMVB_LINE_A_KO);// Test TX MVB su LINEA  A KO    
//    }
//
//	for (i = 0; i<20000; i++);					/* istruzione di attesa				*/
//
//	isr0val = *regisr0;     /* INT0 interrupts*/
//	isr1val = *regisr1;     /* INT1 interrupts*/
//	fcval = *regfc;         /* Frame Counter */
//	ecval = *regec;         /* Error Counter */
//    *regisr0=0;
//	*regivr0=0;
//	*regisr1=0;
//	*regivr1=0;
//	*regfc=0;
//	*regec=0;
//	if (  (isr0val != 0x0685) | (isr1val != 0) | (fcval != 0x3FFC) | ( ecval > 10) | (*st_DA_DATA0ptr == 0) | ( *st_DA_DATA1ptr == 0) | (*st_DA_DATA0ptr1 != 0) | (*st_DA_DATA1ptr1 != 0) )
//	{
////		printf("KO!!!!!!    ");
////		printf("\nISR0=%04x ISR1=%04x FC=%04x EC=%04x\r\n",isr0val,isr1val,fcval,ecval);
////		er = 1;
//	}
//	
//	printf("\n");
//    printf("PROVA ISOLAMENTO DALLA LINEA A\n");    
//    printf("Premere 1 se la trasmissione sulla linea A tra i pin 1 e 2 si interrompre  altrimenti premere 0\n");    
//    while (((c=_getkey())!= '0') && ((c=_getkey())!= '1'));
//    if ((strcmp(c,'1')==0)  {
//        test_mvb_lineAOFF = 1;//OK
//        save_stato(TESTMVB_LINE_A_OFF_OK);// Test TX MVB su LINEA  A OK
//    }
//    if ((strcmp(c,'0')==0)  {
//        test_mvb_lineAOFF = 0;//KO
//        save_stato(TESTMVB_LINE_A_OFF_KO);// Test TX MVB su LINEA  A KO    
//    }
//  	set_out_port(0, DOP0_KMA, 0);
//
//	*st_DA_DATA0ptr=0;
//	*(st_DA_DATA0ptr+1)=0;
//	*(st_DA_DATA0ptr+2)=0;
//	*(st_DA_DATA0ptr+3)=0;
//	*st_DA_DATA1ptr=0;
//	*(st_DA_DATA1ptr+1)=0;
//	*(st_DA_DATA1ptr+2)=0;
//	*(st_DA_DATA1ptr+3)=0;
//	*st_DA_DATA0ptr1=0;
//	*(st_DA_DATA0ptr1+1)=0;
//	*(st_DA_DATA0ptr1+2)=0;
//	*(st_DA_DATA0ptr1+3)=0;
//	*st_DA_DATA1ptr1=0;
//	*(st_DA_DATA1ptr1+1)=0;
//	*(st_DA_DATA1ptr1+2)=0;
//	*(st_DA_DATA1ptr1+3)=0;
//
///* test del loop interno con interrogazione del proprio stato e KMA e KMB chiusi */
//	for (i = 0; i<20000; i++);					/* istruzione di attesa				*/
//	printf("test MVB LINEA B e premere invio dopo aver verificato la forma d'onda sulla linea B tra i pin 4 e 5\r\n");
//   	set_out_port(0, DOP0_KMB, DOP0_KMB);
//	for (off=0; (off != 0x1FFE || c != '\r'); off++)	/*while ( (c=_getkey())!= '\t')*/
//	{
//		while (((c=_getkey())!= '\r') && ((*regmr & 0x0200) != 0)){ //MVBC BUSY
//		} 
//		if ( c == '\r') break; 
//        *FC15_DATAptr = *FC15_DATAptr + 1;
//		*regmr=0x0020;					/* set SMSM :invia master frame manualmente */
//	
//	}
//	printf("\r\n");
//	for (i = 0; i<20000; i++);					/* istruzione di attesa				*/
//
//	isr0val = *regisr0;     /* INT0 interrupts*/
//	isr1val = *regisr1;     /* INT1 interrupts*/
//	fcval = *regfc;         /* Frame Counter */
//	ecval = *regec;         /* Error Counter */
//    *regisr0=0;
//	*regivr0=0;
//	*regisr1=0;
//	*regivr1=0;
//	*regfc=0;
//	*regec=0;
////	if (  (isr0val != 0x0685) | (isr1val != 0) | (fcval != 0x3FFC) | ( ecval > 10) | (*st_DA_DATA0ptr == 0) | ( *st_DA_DATA1ptr == 0) | (*st_DA_DATA0ptr1 != 0) | (*st_DA_DATA1ptr1 != 0) )
////	{
//////		printf("KO!!!!!!    ");
//////		printf("\nISR0=%04x ISR1=%04x FC=%04x EC=%04x\r\n",isr0val,isr1val,fcval,ecval);
//////		er = 1;
////	}
//	
//    printf("VERIFICARE INTERRUZIONE  MVB LINE A - Premere invio e verificare che la forma d'onda su  linea A tra i pin 1 e 2 sia assente\r\n");    
//    while ((c=_getkey())!= '\r');
//  	set_out_port(0, DOP0_KMB, 0);
//
//	
//	
//	*st_DA_DATA0ptr=0;
//	*(st_DA_DATA0ptr+1)=0;
//	*(st_DA_DATA0ptr+2)=0;
//	*(st_DA_DATA0ptr+3)=0;
//	*st_DA_DATA1ptr=0;
//	*(st_DA_DATA1ptr+1)=0;
//	*(st_DA_DATA1ptr+2)=0;
//	*(st_DA_DATA1ptr+3)=0;
//	*st_DA_DATA0ptr1=0;
//	*(st_DA_DATA0ptr1+1)=0;
//	*(st_DA_DATA0ptr1+2)=0;
//	*(st_DA_DATA0ptr1+3)=0;
//	*st_DA_DATA1ptr1=0;
//	*(st_DA_DATA1ptr1+1)=0;
//	*(st_DA_DATA1ptr1+2)=0;
//	*(st_DA_DATA1ptr1+3)=0;
	return er;
}










/**********************************************************/
/* Funzione di test  del  mvb bus                         */
/**********************************************************/
short test_SW1 (short argc, char *argv[] )
{   
    static unsigned char error=0;
    static unsigned char exit=0;
    static unsigned short data1=0;
    static unsigned char sts_testsw1=0;
    static unsigned char old_sts_testsw1=0;
    char c;
    
    unsigned short i,vp;
    
    
    exit = 0;

	*regscr = 0x0000;
	*regscr1 = 0x0000;
	for (i = 0; i<10; i++);                 /* istruzione di attesa				*/
	*regscr = 0x8001;						/* predispone per l'inizializzazione dei reg.	*/
	for (i = 0; i<10; i++);					/* istruzione di attesa				*/
	*regdaok = 0x0049;						/* DISAB sovrascrive indirizzo hardware               */
	data1 = *regdaor;						/* legge il Device Address			*/ 
	*regdr = 0x0000;						/* consente lo switch tra le linee		*/
	*regimr0 = 0x0000;						/* abilita tutte le interrupt			*/
	*regimr1 = 0x0000;
	*regivr0 = 0;
	*regivr1 = 0;
//   	set_out_port(0, DOP0_KMA|DOP0_KMB, DOP0_KMA|DOP0_KMB);
//	*regscr = 0x8503;						/* abilito in full mode l'mvb	*/
//	*regscr = 0x8502;						/* abilito in full mode l'mvb	*/

    if ((argc==2)&& (strcmp(argv[1],"get")==0))
    {
        	*regdaok = 0x0049;						/* sovrascrive indirizzo hardware               */
            data1 = *regdaor & 0x000F;  // legge il DA per l'indirizzamento hw
            printf("SW1 %x\r",data1);    
            return 0;    
    }

    printf("TEST SW1\n");
    while (!exit){
        if ((c=sio_poll_key(2))  == '\t') 
            {
            
                exit= 1;
  		
  		    }
            
        switch(sts_testsw1)
        {
            case 0://premi invio verificare 1
                printf("step 1: Posizionare SW1 su 1 e premere INVIO\n");
                while ((c=sio_poll_key(2))  != '\r')
                {
                    data1 = *regdaor & 0x000F;  // legge il DA per l'indirizzamento hw
                    printf("SW1 %x\r",data1);    
                }
        	    if (data1 == 1){
        	        sts_testsw1 = 1;
        	        printf("\n");
        	    }
        	    else{
        	        old_sts_testsw1 = sts_testsw1;
        	        sts_testsw1 = 10;// errore
           	        save_stato (TESTSW1_KO1);

        	    }
                break;
            case 1://premi invio verificare 2
                printf("step 2: Posizionare SW1 su 2 e premere INVIO\n");
                while (((c=sio_poll_key(2))  != '\r')/*&&((c=sio_poll_key(2))  != 'x')*/)
                {
                    data1 = *regdaor & 0x000F;  // legge il DA per l'indirizzamento hw
                    printf("SW1 %x\r",data1);    
                }

                if (c=='x') sts_testsw1 = 5;
                    
        	    if (data1 == 2){
        	        sts_testsw1 = 2;
        	        printf("\n");
        	    }
        	    else{
        	        old_sts_testsw1 = sts_testsw1;
        	        sts_testsw1 = 10;// errore
           	        save_stato (TESTSW1_KO2);
        	    }
                break;
            case 2://premi invio verificare 4
                printf("step 3: Posizionare SW1 su 4 e premere INVIO\n");
                while (((c=sio_poll_key(2))  != '\r')/*&&((c=sio_poll_key(2))  != 'x')*/)
                {
                    data1 = *regdaor & 0x000F;  // legge il DA per l'indirizzamento hw
                    printf("SW1 %x\r",data1);    
                }

                if (c=='x') 
                    sts_testsw1 = 5;
        	    if (data1 == 4){
        	        sts_testsw1 = 3;
        	        printf("\n");
        	    }
        	    else{
        	        old_sts_testsw1 = sts_testsw1;
        	        sts_testsw1 = 10;// errore
           	        save_stato (TESTSW1_KO4);
        	    }
                break;
            case 3://premi invio verificare 8
                printf("step 4: Posizionare SW1 su 8 e premere INVIO\n");
                while (((c=sio_poll_key(2))  != '\r')/*&&((c=sio_poll_key(2))  != 'x')*/)
                {
                    data1 = *regdaor & 0x000F;  // legge il DA per l'indirizzamento hw
                    printf("SW1 %x\r",data1);    
                }

                if (c=='x') sts_testsw1 = 5;

        	    if (data1 == 8){
        	        sts_testsw1 = 4;
        	        printf("\n");
        	    }
        	    else{
        	        old_sts_testsw1 = sts_testsw1;
        	        sts_testsw1 = 10;// errore
           	        save_stato (TESTSW1_KO8);
        	    }
                break;
            case 4://premi invio verificare 0
                printf("step 5: Posizionare SW1 su 0 e premere INVIO\n");
                while (((c=sio_poll_key(2))  != '\r')/*&&((c=sio_poll_key(2))  != 'x')*/)
                {
                    data1 = *regdaor & 0x000F;  // legge il DA per l'indirizzamento hw
                    printf("SW1 %x\r",data1);    
                }

                if (c=='x') sts_testsw1 = 5;
                    
        	    if (data1 == 0){
        	        sts_testsw1 = 5;
        	        printf("\nTEST SW1 OK\n");
        	        save_stato (TESTSW1OK);
        	    }
        	    else{
        	        old_sts_testsw1 = sts_testsw1;
        	        sts_testsw1 = 10;// errore
           	        save_stato (TESTSW1_KO0);
        	    }
                break;
            case 5://test terminato
                    exit =1;
                    printf("EXIT\n");
                    sts_testsw1=0;
                break;
            case 10://stampa errore
                printf("ERROR riletto %x\n",data1);
                sts_testsw1 = old_sts_testsw1;//ritorna al test precedente
                error = old_sts_testsw1;

                break;
            default:
             break;
        }    
        
    }


    return error;
}



short tstmvb_txrx(short argc, char *argv[] )
{
	unsigned short data1,i,er;
    char c;
  

	printf("********** TEST MVBC *************\r\n");
	printf("POSIZIONARE SW1 IN POSIZIONE 1 E PREMERE INVIO\r\n");
    while( (c=_getkey())  != '\r');

	printf("VERIFICA selettore d'indirizzo IN POSIZ 1\r\n");
	/* MCM IS 0 ->TM SIZE 16KB*/
	*regmcr = *regmcr & 0xFFF8;// set MCM=000 
	*regscr = *regscr | 0x8001;// set IL=01  configuration mode
	
	er = 0;
	data1 = *regdaor & 0x000F;  // legge il DA per l'indirizzamento hw
	*regdaok = 0x0049;						/* sovrascrive indirizzo hardware               */
	if (data1 != 0x0001){
	     printf("\n !! selezionare SW1 in posizione 1 \n",data1);
	}
    
  	printf("Test della Traffic Memory 1MB (MCM = 4)\n");
	*regmcr = *regmcr | funmode;
    printf("Collegare il tappo di terminazione MVB al connettore MVB2 e premere invio per proseguire\n");    
    while ( (c=_getkey()) != '\r' );

	if (!(er = t_mvbel())) {
//			while ( _getkey() != '\r' );
//			er = t_mvbel();
	}

	if (er) save_stato(TMVBBUSKO);
	else save_stato(TMVBBUSOK);	

  
    return er;  
}

/**********************************************************/
/* Funzione di test  del  mvb bus                         */
/**********************************************************/
short test_mvb (short argc, char *argv[] )
{
	unsigned short data1,i,er[4];
	char c;

	printf("\nTEST Traffic Memory MVB\n");
	/* MCM IS 0 ->TM SIZE 16KB*/
	*regmcr1 = *regmcr1 & 0xFFF8;// set MCM=000 
	*regscr = *regscr | 0x8001;// set IL=01  configuration mode
//	er = 0;


	printf("Test Service Area (C0FC00H..C0FFFEH)\r\n");

//	sasaddmvb1     = 0xC03C00;   /* indirizzo di partenza della service area dell'MVBC nel mode 'funmode'*/     
//    saeaddmvb1     = 0xC03FFE;   /* indirizzo di fine della service area dell'MVBC nel mode 'funmode'*/

	if (shf0(sasaddmvb,saeaddmvb)){
	    save_stato(TMVBSASHF0KO);
	    er[0]=1;
	}
	else{
	    save_stato(TMVBSASHF0OK);	
	    er[0]=0;
	}

	if (shf1(sasaddmvb,saeaddmvb)){
	    save_stato(TMVBSASHF1KO);
	    er[1]=1;
	}
	else{
        save_stato(TMVBSASHF1OK);	
        er[1]=0;
    }
/*
	if (march(sasaddmvb,saeaddmvb)) save_stato(TMVBSAMARCHKO);
	else save_stato(TMVBSAMARCHOK);	

	if (maxrum(sasaddmvb,saeaddmvb)) save_stato(TMVBSAMXRUMKO);
	else save_stato(TMVBSAMXRUMOK);	
*/

	printf("Test della Traffic Memory 1MB MCM = 4 evitando la Service Area testata prima\r\n");
	*regmcr = *regmcr | funmode;

	if (shf0_mvb(saddmvb,eaddmvb)) {
	    save_stato(TMVBTMSHF0KO);
	    er[2]=1;
	}
	else {
	    save_stato(TMVBTMSHF0OK);	
	    er[2]=0;
	}

	if (shf1_mvb(saddmvb,eaddmvb)) {
	    save_stato(TMVBTMSHF1KO);
	    er[3]=1;
	}
	else {
	    save_stato(TMVBTMSHF1OK);	
	    er[3]=0;
	}
/*
	if (march_mvb(saddmvb,eaddmvb)) save_stato(TMVBTMMARCHKO);
	else save_stato(TMVBTMMARCHOK);	

	if (maxrum_mvb(saddmvb,eaddmvb)) save_stato(TMVBTMMXRUMKO);
	else save_stato(TMVBTMMXRUMOK);	

*/
    if(er[0] || er[1] || er[2] || er[3] )    printf("TEST_TM_MVB_KO\n");
    else printf("TEST_TM_MVB_OK\n");
}

