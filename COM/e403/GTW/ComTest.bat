:: Create the XRAY executable
cd EXE
c:\master\bin\xconfig -Tc:\sw\com\libs\lib ..\ComTest.def

:: Create the S-REC version
c:\mri\asm68k\lnk68k -o testcom.hex -c c:\sw\e403\gtw\ComTest.lin -Fs
