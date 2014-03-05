Opening log file C:\sw\mmi\exe\testram.res
>loadv testram.hex
Loading ...  loaded.
Load time was 0.00s.
Verifying ...  verified.
Verify time was 0.00s.
Map file does not exist.
>;Esegue il TESTRAM
>g 0
Waiting for keystroke or breakpoint ...
Hardware breakpoint signal received from ICD cable.
>
>;dump CODICE ESITO TEST
>dump.l 50 50
Dumping memory data, Press any key to abort.
00000050 000000A5
>;stampa CODICE DI ERRORE
>dump.l 54 54
Dumping memory data, Press any key to abort.
00000054 00000000
>;TESTRAM TERMINATO
>CAPTUREOFF
