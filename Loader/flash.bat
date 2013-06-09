rem 22/07/04   Napoli   Schiavo R.
rem creazione loader GTW

echo off
rem %1 nome progetto

IF %1A == A GOTO FAIL1
rem %2 nomesw
IF %2A == A GOTO FAIL2

mkdir flash\.
cd flash\.


mkdir %2\.
mkdir %2\grep\.
mkdir %2\%2\.

copy ..\com.icd  %2\%2\.
rename %2\%2\com.icd %2.icd

echo loadv %2\xdm68kR.hex            >> %2\%2\%2.icd
echo loadv %2\boot.het               >> %2\%2\%2.icd
echo loadv %2\%2.hex                 >> %2\%2\%2.icd
echo pc 110                          >> %2\%2\%2.icd
echo goexit                          >> %2\%2\%2.icd



copy ..\startup.icd %2\.
copy ..\exec_com.bat %2\.
copy ..\load_com.bat %2\.
copy ..\test_com.bat %2\.

echo load_com_usb %2  >> %2\burn_usb.bat

copy ..\..\%1\gtw\exe\%2.hex  %2\%2\.
copy ..\..\com\boot\boot.het  %2\%2\.
copy ..\..\com\monitor\xdm68kR.hex  %2\%2\.


xcopy ..\grep\. %2\grep\. /e


rem pkzip -rP a:\%2.zip %2\*.*

rem copy ..\getsw.bat a:\
rem copy \uty\pkunzip.exe a:\

cd ..
rem deltree tmp\.

GOTO EXIT


:FAIL1
echo Manca il parametro nome progetto
echo ====================================================================
echo SINTASSI  flash    Nomeprogetto nomeEseguibile
echo ====================================================================
echo .
goto EXIT

:FAIL2
echo Manca il parametro nomesw
echo ====================================================================
echo SINTASSI  flash    Nomeprogetto nomeEseguibile
echo ====================================================================
echo .
goto EXIT

:EXIT
