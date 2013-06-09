Opening log file C:\sw\mmi\exe\TRAM_WE2.RES
>;test RAM bus D15..D8 IC8 con /WE2
>mm.l 	00000000 $00000000
>dump.l  00000000 00000003
Dumping memory data, Press any key to abort.
00000000 00000000
>dump.l  00000000 00000003
Dumping memory data, Press any key to abort.
00000000 00000000
>
>mm.b 	00000002 $AA
>dump.l  00000000 00000003
Dumping memory data, Press any key to abort.
00000000 0000AA00
>dump.l  00000000 00000003
Dumping memory data, Press any key to abort.
00000000 0000AA00
>
>mm.b 	00000002 $55
>dump.l  00000000 00000003
Dumping memory data, Press any key to abort.
00000000 00005500
>dump.l  00000000 00000003
Dumping memory data, Press any key to abort.
00000000 00005500
>CAPTUREOFF
