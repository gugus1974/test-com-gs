* Map the DPRAM @ $FFFF8000
fc 7
mm $3FF00 ;l
$FFFF8001.
fc 5
*
* Write the CLKOCR register
mm $FFFF900C ;b
$C.
*
* Write the SYPCR register (disable watchdog)
mm $FFFF9022 ;b
$70.
*
* Write the PEPAR register
mm $FFFF9016 ;w
$00B4.
*
* Program the CS0 (FLASH) @ $400000 1MB
mm $FFFF9050 ;l
$00400001.
mm $FFFF9054 ;l
$3FF00002.
*
* Program the CS1 (RAM) @ $0 2MB
mm $FFFF9060 ;l
$00000001.
mm $FFFF9064 ;l
$1FE00000.
*
* Program the CS2 (MVBC) @ $C00000 1MB
mm $FFFF9070 ;l
$00C00001.
mm $FFFF9074 ;l
$2FF00006.
*
* Program the CS5 (I/O) @ $B00000 2KB
mm $FFFF90A0 ;l
$00B00001.
mm $FFFF90A4 ;l
$1FFFF800.
*
