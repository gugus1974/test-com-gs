;LOG RESET COMMNAD
CAPTURE reset.log
reset
reset
reset
CAPTUREOFF

; Map the DPRAM @ $FFFF0000
debugger_dfc 7
debugger_sfc 7
mm.l 3FF00 FFFF0001
debugger_dfc 5
debugger_sfc 5

; Clear the sw fault reset sources (just to be sure)
mm.b FFFF1009 30
; Write the PEPAR register
mm.w FFFF1016 00B4
; Program the CS1 (RAM)
; Write BR1 0x00000001
mm.l FFFF1060 00000001
; Write OR1 0x00000001
mm.l FFFF1064 1FE00000
; Program the CS0 (FLASH), set A22
; Write BR0 0x00400001
mm.l FFFF1050 00400001
; Write OR0 0x00000001
mm.l FFFF1054 3FF00002
; Program the SYPCR to disable the watchdog (to allow Flash burning)
mm.b FFFF1022 4C


CAPTURE pretestram.log
;************************************
;verifica /WE0
;test RAM bus D31..D24 IC6 con /WE0
mm.l 	00000000 $00000000
dump.l  00000000 00000003
dump.l  00000000 00000003

mm.b 	00000000 $55
dump.l  00000000 00000003
dump.l  00000000 00000003

mm.b 	00000000 $AA
dump.l  00000000 00000003
dump.l  00000000 00000003

;mm.b 	00000000 $00
;dump.l  00000000 00000003
;dump.l  00000000 00000003

;mm.b 	00000000 $55
;dump.l  00000000 00000003
;dump.l  00000000 00000003

;test RAM bus D23..D16 IC7 con /WE1
mm.l 	00000000 $00000000
dump.l  00000000 00000003
dump.l  00000000 00000003

mm.b 	00000001 $AA
dump.l  00000000 00000003
dump.l  00000000 00000003

mm.b 	00000001 $55
dump.l  00000000 00000003
dump.l  00000000 00000003

 ;test RAM bus D15..D8 IC8 con /WE2
mm.l 	00000000 $00000000
dump.l  00000000 00000003
dump.l  00000000 00000003

mm.b 	00000002 $AA
dump.l  00000000 00000003
dump.l  00000000 00000003

mm.b 	00000002 $55
dump.l  00000000 00000003
dump.l  00000000 00000003



 ;test RAM bus D8..D0 IC9 con /WE3
mm.l 	00000000 $00000000
dump.l  00000000 00000003
dump.l  00000000 00000003

mm.b 	00000003 $AA
dump.l  00000000 00000003
dump.l  00000000 00000003

mm.b 	00000003 $55
dump.l  00000000 00000003
dump.l  00000000 00000003

CAPTUREOFF

; Carica il programma per il test della RAM a 0, ne verifica il corretto
; caricamento, lo esegue e visualizza i risultati:
; All'indirizzo 50 c'e' il risultato del test: A5 = OK
; In caso FAIL all'indirizzo 54 c'e' l'indirizzo in cui il test e' fallito

CAPTURE testram.res
loadv testram.hex


.
;Esegue il TESTRAM 
g 0

;dump CODICE ESITO TEST
dump.l 50 50
;stampa CODICE DI ERRORE
dump.l 54 54
;TESTRAM TERMINATO
CAPTUREOFF

; Un-Map the DPRAM
debugger_dfc 7
debugger_sfc 7
mm.l 3FF00 0
debugger_dfc 5
debugger_sfc 5

; *****  ATTENDERE CARICAMENTO APPLICATIVO DI TEST IN CORSO...  *****
capture loadapp.res
loadv boot.het
loadv testcom.hex
pc 100
captureoff
goexit
