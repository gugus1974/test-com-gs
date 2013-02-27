echo off

echo capture loadlog.txt >startup.icd
echo reset >>startup.icd
rem echo exit >>startup.icd

.\exec_com.bat %1 %2
