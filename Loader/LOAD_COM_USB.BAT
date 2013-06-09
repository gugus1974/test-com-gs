echo off
del c:\pemicro\pkg32z\startup.icd
copy c:\sw_com\%1\startup.icd c:\pemicro\pkg32z
echo capture loadlog.txt >startup.icd
echo macro %1\%1.icd >>startup.icd


c:\pemicro\pkg32z\icd32z.exe io_delay_cnt 10 quiet
