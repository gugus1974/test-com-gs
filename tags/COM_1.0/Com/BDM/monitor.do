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
* Program the CS0 (FLASH) everywhere with the correct waitstates
mm $FFFF9050 ;l
$00000001.
mm $FFFF9054 ;l
$30000002.
*
* Set the external commands load address
driver $FFFF8000
*
* Erase the FLASH
amdera
*
* Write the monitor
amdp c:\sw\com\monitor\xdm68k.d32
*
