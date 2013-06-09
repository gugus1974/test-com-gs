@echo off
rem 2005/gen/25,mar 19:00               \SW\LOADER\foto.bat
rem ************************************************************
rem     Procedura di creazione di una foto completa di:
rem applicativo + directory di libreria
rem ************************************************************
rem $Date: $          $Revision: $
rem $Author: $






if "%1" == ""               goto errore
if EXIST \SWHST\%1.rar      goto errore
if EXIST \%1\nul            goto erroredir

echo ============================================================
echo    Si sta per creare la foto:     %1.rar
echo    contenente:
echo        \sw\%2
echo        \sw\ATR
echo        \sw\COM
echo        \sw\LOADER
echo        \sw\TCNS
echo        \sw\LBATR
echo        \sw\LIB
if "%3" == "" goto NoOpzDir
  echo        \sw\%3
if "%4" == "" goto NoOpzDir
  echo        \sw\%4
if "%5" == "" goto NoOpzDir
  echo        \sw\%5
if "%6" == "" goto NoOpzDir
  echo        \sw\%6
if "%7" == "" goto NoOpzDir
  echo        \sw\%7
if "%8" == "" goto NoOpzDir
  echo        \sw\%8

:NoOpzDir
echo ============================================================
pause

md \%1
md \%1\TEMP.DAN
md \%1\%2

if NOT EXIST \sw\%2\history.mod goto aggmod
    copy \sw\%2\history.mod  \%1\%2\
:aggmod

echo ...                                                    >  \%1\%1.fot
echo ====================================================== >> \%1\%2\history.mod
echo %1.rar                                                 >> \%1\%2\history.mod

Uedit32 \%1\%2\history.mod

pause

del \sw\*.fot
copy \%1\*.* \sw

xcopy \sw\ATR      \%1\ATR\    /s
xcopy \sw\COM      \%1\COM\    /s
xcopy \sw\LOADER   \%1\LOADER\ /s
xcopy \sw\TCNS      \%1\TCNS\    /s
xcopy \sw\LBATR   \%1\LBATR\ /s
xcopy \sw\LIB      \%1\LIB\    /s

if "%2" == "" goto NoAppl
  xcopy \sw\%2       \%1\%2\   /s


if "%3" == "" goto NoFigl
  xcopy \sw\%3   \%1\%3\     /s
if "%4" == "" goto NoFigl
  xcopy \sw\%4   \%1\%4\     /s
if "%5" == "" goto NoFigl
  xcopy \sw\%5   \%1\%5\     /s
if "%6" == "" goto NoFigl
  xcopy \sw\%6   \%1\%6\     /s
if "%7" == "" goto NoFigl
  xcopy \sw\%7   \%1\%7\     /s
if "%8" == "" goto NoFigl
  xcopy \sw\%8   \%1\%8\     /s

:NoAppl
:NoFigl


c:\programmi\winrar\winrar   a -r -s \swHST\%1.rar \%1\*.*

pause



:ramo2000
rmdir /S /Q \%1
goto listDir

:listDir
dir     \swHST\*.rar /od /p
cd      \sw
goto end

:erroredir
echo .
echo ======================================================================
echo  ATTENZIONE: La directory  \%1  gia' esiste e verrebbe distrutta !!!
echo ======================================================================
echo .
goto end

:errore
echo   Versioni gia' esistenti:
dir \swHST\*.rar /od /p /w
echo ====================================================================
echo SINTASSI  foto    NomeFoto  [DirAppl]
echo        Oltre   DirAppl,
echo        si includono automaticamente ATR,COM,LOADER,TCNS
echo
echo ====================================================================
echo .
goto end


:end


rem ****************************************************************************/
rem $Log: $
rem Revision 1.0  22/04/05                  Schiavo

