Opening log file C:\sw\mmi\exe\TRAM_WE1.RES
>;test RAM bus D23..D16 IC7 con /WE1
>mm.l 	00000000 $00000000
>dump.l  00000000 00000003
Dumping memory data, Press any key to abort.
00000000 00000000
>dump.l  00000000 00000003
Dumping memory data, Press any key to abort.
00000000 00000000
>
>mm.b 	00000001 $AA
>dump.l  00000000 00000003
Dumping memory data, Press any key to abort.
00000000 00AA0000
>dump.l  00000000 00000003
Dumping memory data, Press any key to abort.
00000000 00AA0000
>
>mm.b 	00000001 $55
>dump.l  00000000 00000003
Dumping memory data, Press any key to abort.
00000000 00550000
>dump.l  00000000 00000003
Dumping memory data, Press any key to abort.
00000000 00550000
>CAPTUREOFF
