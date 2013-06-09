echo off



IF %1A == A GOTO PARAM0
IF %2A == A GOTO PARAM1

:PARAM2
IF %2 == 0 GOTO PARAM2NODELAY
c:\pemicro\icd32z\icd32z.exe %1 io_delay_cnt %2 quiet

:PARAM2NODELAY
c:\pemicro\icd32z\icd32z.exe %1 quiet

:PARAM1
c:\pemicro\icd32z\icd32z.exe %1 io_delay_cnt 10 quiet

:PARAM0
c:\pemicro\icd32z\icd32z.exe lpt2 io_delay_cnt 10 quiet

