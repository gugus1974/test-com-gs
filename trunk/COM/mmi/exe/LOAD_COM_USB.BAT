echo off
del c:\pemicro\pkg32z\startup.icd
echo macro C:\sw\mmi\exe\testram2.icd >startup.icd
copy testram2.cpy testram2.icd 
copy startup.icd c:\pemicro\pkg32z
rem pause
c:\pemicro\pkg32z\icd32z.exe io_delay_cnt 10 quiet
timeout /t 20
