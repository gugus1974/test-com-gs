Opening log file C:\sw\mmi\exe\TRAM_WE0.RES
>;verifica /WE0
>;test RAM bus D31..D24 IC6 con /WE0
>mm.l 	00000000 $00000000
>dump.l  00000000 00000003
Dumping memory data, Press any key to abort.
00000000 00000000
>dump.l  00000000 00000003
Dumping memory data, Press any key to abort.
00000000 00000000
>
>mm.b 	00000000 $55
>dump.l  00000000 00000003
Dumping memory data, Press any key to abort.
00000000 55000000
>dump.l  00000000 00000003
Dumping memory data, Press any key to abort.
00000000 55000000
>
>mm.b 	00000000 $AA
>dump.l  00000000 00000003
Dumping memory data, Press any key to abort.
00000000 AA000000
>dump.l  00000000 00000003
Dumping memory data, Press any key to abort.
00000000 AA000000
>CAPTUREOFF
