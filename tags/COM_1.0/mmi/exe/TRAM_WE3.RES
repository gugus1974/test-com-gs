Opening log file C:\sw\mmi\exe\TRAM_WE3.RES
>;test RAM bus D8..D0 IC9 con /WE3
>mm.l 	00000000 $00000000
>dump.l  00000000 00000003
Dumping memory data, Press any key to abort.
00000000 00000000
>dump.l  00000000 00000003
Dumping memory data, Press any key to abort.
00000000 00000000
>
>mm.b 	00000003 $AA
>dump.l  00000000 00000003
Dumping memory data, Press any key to abort.
00000000 000000AA
>dump.l  00000000 00000003
Dumping memory data, Press any key to abort.
00000000 000000AA
>
>mm.b 	00000003 $55
>dump.l  00000000 00000003
Dumping memory data, Press any key to abort.
00000000 00000055
>dump.l  00000000 00000003
Dumping memory data, Press any key to abort.
00000000 00000055
>
>CAPTUREOFF
