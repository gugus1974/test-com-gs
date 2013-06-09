echo off
echo capture loadlog.txt >startup.icd
echo macro %1\%1.icd >>startup.icd
del c:\pemicro\icd32z\startup.icd
copy c:\sw_com\%1\startup.icd c:\pemicro\icd32z

.\exec_com.bat %2 %3
