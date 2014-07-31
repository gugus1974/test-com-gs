:: Create the XRAY executable
cd EXE
c:\master\bin\xconfig -Tc:\sw\com\libs\lib ..\ComTest.def

:: Create the S-REC version
c:\mri\asm68k\lnk68k -o c:\sw_com\testcom\testcom\testcom.hex -c c:\sw\e403\gtw\ComTest.lin -Fs
