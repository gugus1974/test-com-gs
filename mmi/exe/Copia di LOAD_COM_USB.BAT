rem echo off
del c:\pemicro\pkg32z\startup.icd
copy testram2.cpy startup.icd 
copy startup.icd c:\pemicro\pkg32z
pause
c:\pemicro\pkg32z\icd32z.exe io_delay_cnt 10 quiet
