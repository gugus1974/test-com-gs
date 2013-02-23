@echo off
rem 2005/mag/27,ven 16:45               \SWISV\COMANDI\foto.bat
rem ************************************************************
rem     Procedura di creazione di una foto completa di:
rem Applic + COMANDI + LIB + L16x + directory opzionali (fino a 6)
rem ************************************************************
rem $Date: 2007-11-13 12:16:10 +0100 (mar, 13 nov 2007) $          $Revision: 2968 $
rem $Author: devitadf $


if "%winVrs%" == "w98"              goto winDef
if "%winVrs%" == "w2000"            goto winDef
goto errWin

:winDef
if "%2" == ""               goto errore
if EXIST \SWISVHST\%1.rar   goto errore
if EXIST \%1\nul            goto erroredir

echo ============================================================
echo    Si sta per creare la foto:     %1.rar
echo    contenente:
echo        \SWISV\%2
echo        \SWISV\COMANDI
echo        \SWISV\LIB
echo        \SWISV\L16x
if "%3" == "" goto NoOpzDir
  echo        \SWISV\%3
if "%4" == "" goto NoOpzDir
  echo        \SWISV\%4
if "%5" == "" goto NoOpzDir
  echo        \SWISV\%5
if "%6" == "" goto NoOpzDir
  echo        \SWISV\%6
if "%7" == "" goto NoOpzDir
  echo        \SWISV\%7
if "%8" == "" goto NoOpzDir
  echo        \SWISV\%8

:NoOpzDir
echo ============================================================
pause

md \%1
md \%1\TEMP.DAN

if NOT EXIST \SWISV\history.mod goto aggmod
    copy \SWISV\history.mod  \%1
:aggmod

echo ...                                                    >  \%1\%1.fot
echo ====================================================== >> \%1\history.mod
echo %1.rar                                                 >> \%1\history.mod

Uedit32 \%1\history.mod

pause

del \SWISV\*.fot
copy \%1\*.* \SWISV

xcopy \SWISV\COMANDI   \%1\COMANDI\ /s /h /k
xcopy \SWISV\%2        \%1\%2\      /s /h /k
xcopy \SWISV\LIB       \%1\LIB\     /s /h /k
xcopy \SWISV\L16x      \%1\L16x\    /s /h /k

if "%3" == "" goto NoFigl
  xcopy \SWISV\%3   \%1\%3\     /s /h /k
if "%4" == "" goto NoFigl
  xcopy \SWISV\%4   \%1\%4\     /s /h /k
if "%5" == "" goto NoFigl
  xcopy \SWISV\%5   \%1\%5\     /s /h /k
if "%6" == "" goto NoFigl
  xcopy \SWISV\%6   \%1\%6\     /s /h /k
if "%7" == "" goto NoFigl
  xcopy \SWISV\%7   \%1\%7\     /s /h /k
if "%8" == "" goto NoFigl
  xcopy \SWISV\%8   \%1\%8\     /s /h /k

:NoFigl

winrar95   a -r -s \SWISVHST\%1.rar \%1\*.*

pause

if "%winVrs%" == "w98"      goto ramo98
if "%winVrs%" == "w2000"    goto ramo2000

:ramo98
deltree /Y  \%1
goto listDir

:ramo2000
rmdir /S /Q \%1
goto listDir

:listDir
dir     \SWISVHST\*.rar /od /p
cd      \SWISV
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
dir \SWISVHST\*.rar /od /p /w
echo ====================================================================
echo SINTASSI  foto    NomeFoto  DirAppl  [Dir1 Dir2 Dir3 Dir4 Dir5 Dir6]
echo        Oltre   DirAppl,
echo        si includono automaticamente solo   COMANDI, LIB e L16x
echo        Ricordare di includere tutte le altre librerie in uso !
echo ====================================================================
echo .
goto end

:errWin
echo ======================================================================
echo  ATTENZIONE:   Non e' definita la versione Windows in uso
echo                Controllare che:    Ambiente SWISV\Prompt di lavoro.lnk
echo                lanci       \UtyATR\UtyATr98.bat
echo                oppure      \UtyATR\UtyATr2000.bat
echo ======================================================================

:end


rem ****************************************************************************/
rem $Log$
rem Revision 1.7  2005/05/27 14:48:46  mungiguerrav
rem 2005/mag/27,ven 16:45           Napoli      Mungi\Accardo
rem - Come esempio didattico di commit per Accardo (nuovo acquisto CVS)
rem   si corregge commento iniziale
rem
rem Revision 1.6  2005/03/30 13:24:55  mungiguerrav
rem 2005/mar/18,ven 19:40           Napoli      Mungi
rem - BugFix:   Avendo scoperto che le directory nascoste (come le CVS)
rem             non sono normalmente copiate da xcopy, si aggiungono
rem             le relative opzioni:
rem                 /h    copia anche i file nascosti e di sistema
rem                 /k    copia anche gli attributi, ossia le mantiene nascoste
rem - Si aggiunge anche L16x (oltre a Comandi e Lib) come directory aggiunta
rem   automaticamente alla foto e si correggono i commenti on line
rem
rem Revision 1.5  2005/01/25 18:21:19  mungiguerrav
rem 2005/gen/25,mar 19:00           Napoli      Mungi
rem - Si aggiungono keyword CVS e storia invertita
rem
rem
rem ------------    Fase transitoria tra *.mod e CVS    -------------
rem Revision 1.4        2004/06/28 14:08:30   nappoc
rem Revision 1.3.4.1    2004/06/21 15:41:12   buscob
rem 04.01   2003/dic/09,mar             Napoli                  Mungi\Calabrese
rem         - Si corregge stampa iniziale delle directory incluse
rem
rem Revision 1.3  2004/06/15 09:40:40   buscob
rem 04.00   2003/dic/01,lun             Madrid                  Mungi
rem         - Si cambia editor  da  q.exe   a   Uedit32.exe
rem         - Si controlla  %winVrs%:
rem             se manca,       si stampa messaggio di errore con help
rem                             per correggere problema
rem             se "w98"        si utilizza     deltree /Y
rem             se "w2000"      si utilizza     rmdir /S /Q
rem         - Si aggiunge stampa delle directory che saranno incluse nella foto,
rem           in modo che sia piu' difficile dimenticare qualche libreria opzionale
rem
rem Revision 1.2  2002/10/21 13:34:01   buscob
rem - Inserimento in CVS della versione in cmd03f
rem 03.05   2002/mar/20,mer         Napoli                  Mungi\Accardo
rem         - Si agg. Dir5 e Dir6 opzionale sui param7 e 8
rem
rem Revision 1.1  2002/10/21 13:15:09   buscob
rem - Inserimento in CVS della versione in cmd03b
rem 03.04   2001/mag/31,gio         Napoli                  Mungi
rem         - Si crea il file foto.mod
rem
rem -----------------       Prima del CVS       -------------
rem 03.03   2001/mag/02,mer         Napoli                  Busco
rem         - Si includono anche le sottodirectory di \SWISV\COMANDI
rem 03.02   2000/gen/13,gio         Napoli                  Mungi
rem         - Si utilizza winrar95 invece di rar2
rem 03.01   1998/mar/03,mar - Si agg. gestione history.mod e NomeFoto.fot
rem 03.00   1997/giu/12,gio - Si adegua a \SWISV e si controlla esistenza dir NomeFoto
rem 02.05   1997/giu/09,lun - Si gest. fino a 4 directory ausiliarie
rem 02.04   1997/giu/05,gio - Si agg. gest. LIBSIM e LIBDAU
rem 02.03   1997/mag/09,ven - Si modifica rar.exe   in   rar2.exe
rem 02.02   1997/mar/27,gio - Si cambia DataAppl  in  DataComandi
rem 02.01   1997/feb/19,mer - Si sposta in \SWDAU\COMANDI e usa SWDAUHST
rem 02.00   1996/giu/24,lun - Si adegua a SWDAN\COMANDI e \SWDAN\TEMP.DAN
rem 01.02   1996/mag/29,mer - Si agg.  [DirFigl]  opzionale
rem 01.01   1996/mar/15,ven - Si agg. /p ai comandi dir
rem                         - Si cambia NomeVers   in   nomeFoto
rem 01.00   1996/gen/09,mar - Creazione

