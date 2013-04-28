capture loadapp.res

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

; Un-Map the DPRAM
debugger_dfc 7
debugger_sfc 7
mm.l 3FF00 0
debugger_dfc 5
debugger_sfc 5

; *****  ATTENDERE CARICAMENTO APPLICATIVO DI TEST IN CORSO...  *****

loadv boot.het
loadv comtest.hex

pc 110

captureoff
goexit
