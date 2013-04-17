CAPTURE reset.res
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
; Program the CS0 (FLASH) and CS1 (RAM)
mm.l FFFF1060 00000001
mm.l FFFF1064 1FE00000
mm.l FFFF1050 00400001
mm.l FFFF1054 3FF00002
; Program the SYPCR to disable the watchdog (to allow Flash burning)
mm.b FFFF1022 4C

mm.l 00000000 0000ffff 
dump.l 00000000 00000000

mm.l 00000000 ffff0000 
dump.l 00000000 00000000


; Carica il programma per il test della RAM a 0, ne verifica il corretto
; caricamento, lo esegue e visualizza i risultati:
; All'indirizzo 50 c'e' il risultato del test: A5 = OK
; In caso FAIL all'indirizzo 54 c'e' l'indirizzo in cui il test e' fallito
capture testram.res
loadv testram.hex
; *****  ATTENDERE (circa 25 sec) TEST RAM IN CORSO SULLA SCHEDA  *****
;
g 0
dump.l 50 50
dump.l 54 54
captureoff

; Un-Map the DPRAM
debugger_dfc 7
debugger_sfc 7
mm.l 3FF00 0
debugger_dfc 5
debugger_sfc 5
capture loadapp.res
loadv boot.het


; *****  ATTENDERE CARICAMENTO APPLICATIVO DI TEST IN CORSO...  *****
loadv comtest.hex
captureoff
pc 100
goexit
