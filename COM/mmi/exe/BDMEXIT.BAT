echo off
del c:\pemicro\pkg32z\startup.icd
echo reset >startup.icd
echo exit >startup.icd
rem echo pause >startup.icd
copy startup.icd c:\pemicro\pkg32z
rem pause
c:\pemicro\pkg32z\icd32z.exe io_delay_cnt 10 quiet
