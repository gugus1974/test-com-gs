rem 12/12/06   Napoli   Schiavo R.
rem 
echo off
rem %1 nome progetto
rem %2 nome eseguibile

IF %1A == A GOTO FAIL
IF %2A == A GOTO FAIL


copy c:\SW\%1\gtw\exe\%2.hex c:\sw_com\%2\%2

:FAIL
echo Manca UN parametro 
echo ====================================================================
echo SINTASSI  reFRESH    NOME PROGETTO nomeEseguibile
echo ====================================================================
echo .
goto EXIT

:EXIT
