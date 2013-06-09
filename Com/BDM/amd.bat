asm68k amdera.s
lnk68k -Fs -o amdera.hex amdera.obj
m68k-coff-objcopy -O srec --gap-fill=0 amdera.hex amdera.d32
rm amdera.obj amdera.hex
asm68k amdp.s
lnk68k -Fs -o amdp.hex amdp.obj
m68k-coff-objcopy -O srec --gap-fill=0 amdp.hex amdp.d32
rm amdp.obj amdp.hex

