NAME	TEST
;#########################################################################################          
;               DEFINEs
;#########################################################################################          

    TXE_UART1	EQU P1.0	; TXE UART 1                      
    TXE_UART2	EQU P1.1	; TXE UART 2                      
    EOC			EQU P1.2	; end of conversion AD converter                      
    RL1_3		EQU P1.3	; uscita RL1.3                      
    RL1_4		EQU P1.4	; uscita RL1.4                      
    RL1_5		EQU P1.5	; uscita RL1.5                      
    RL1_6		EQU P1.6	; uscita RL1.6                      
	FAULT		EQU P1.7	; uscita FAULT
	WD			EQU P3.5	; watch dog
;#########################################################################################          


;#########################################################################################          
	                      
;********************************************************************************
;*      VETTORE DELLE INTERRUZIONI                                              *
;********************************************************************************
;** CONFIG EXTERNAL INTERRUPT 0
;**
        ORG     03H
        RETI

;** CONFIG TIMER 0 INTERRUPT
;**
        ORG  0BH
;        LCALL   TIMER_0
        RETI

;** CONFIG EXTERNAL INTERRUPT 1
;**
        ORG  13H
        RETI

;** CONFIG TIMER 1 INTERRUPT
;**
        ORG  1BH
		CPL		WD
		RETI
;	LJMP	TIMER1_ISR

;** CONFIG SERIAL INTERRUPT
;**
        ORG  23H
        RETI

;********************************************************************************
;*      FINE VETTORE DELLE INTERRUZIONI                                         *
;********************************************************************************



;#########################################################################################          
; 1 - configura il Timer 1 affinchè non scatti il WD  max 1.6 sec
; 2 - configurare la seriale del micro per parlare su bus 422 usando il Timer 0
	ORG		0H
	MOV		TMOD, #21h;             /* TMOD: timer 1, mode 2, 8-bit reload ,Timer 0 fermo       */
	MOV		SCON, #52h;             /* seriale in mode 1, TI a 1
	
	
	MOV		IP, #10h;               /* setta ad hgh la priorità della seriale
	
	MOV		IE, #8Ah;               /* TMOD: timer 1, mode 2, 8-bit reload        */
	MOV		IE, #8Ah;               /* TMOD: timer 1, mode 2, 8-bit reload        */



	JMP		CPU_INIT	

CPU_INIT :
	CLR		WD
	
;#########################################################################################          

;SCRIVE SU P1 PER TEST 
RESET:	
	MOV		A, #0EFH
	CPL		WD
LOOP:	
	MOV		P1,A
	INC		A
	JZ		RESET
	JMP		LOOP


END

