@rem Settings
@set VERSION=1.5.4
@set QTDIR=c:\Qt\6.1.3\mingw81_64
@rem Workaround for that Qt, maybe QtcW7Compat.7z
@set WORKAROUND=
@set MINGW=c:\msys64\mingw64\bin
@set SEVENZIP="c:\Program Files\7-zip\7z.exe"
@set UTRANSL=UTransCon.exe

@rem Rest things
@set PRONAME=Unicodia\Unicodia.pro
@set PRONAME_AB=AutoBuilder\AutoBuilder.pro
@set PRONAME_SC=SmartCopy\SmartCopy.pro
@set EXENAME=Unicodia.exe
@set ARCNAME=Unicodia-w64-%VERSION%.7z
@set BUILD=~Build-win64
@set BUILD_AB=~Build-AB-win64
@set BUILD_SC=~Build-SC-win64
@set DEPLOY=~Deploy
@set DEPLOY1=~Deployed
@set SMARTCOPY=%BUILD_SC%\release\SmartCopy.exe
@set UCAUTO=UcAuto.cpp
@set UCCOUNT=UcAutoCount.h
@set AB_UCAUTO=%BUILD_AB%/%UCAUTO%
@set AB_UCCOUNT=%BUILD_AB%/%UCCOUNT%

@path %MINGW%;%PATH%

@echo ===== Checking for tools =====

@if exist %SEVENZIP% goto sz_ok
@echo BAD: 7-zip not found. Install it into standard directory, or change SEVENZIP variable.
@goto end
:sz_ok
@echo 7-zip OK

@if exist %QTDIR%\bin\qmake.exe goto qt_ok
@echo BAD: Qmake not found. Install Qt, and set QTDIR variable.
@goto end
:qt_ok
@echo Qt OK

@if exist %MINGW%\g++.exe goto mingw_ok
@echo BAD: MinGW not found. Install it into standard MSYS directory.
@goto end
:mingw_ok
@echo MinGW OK

@if exist %UTRANSL% goto utransl_ok
@echo BAD: UTranslCon not found.
@echo Build it manually from https://github.com/Mercury13/utranslator
@goto end
:utransl_ok
@echo UTranslator OK

@echo ===== Creating directories =====
@if exist %DEPLOY% del /S /Q %DEPLOY% >nul
@if exist %DEPLOY% rmdir /S /Q %DEPLOY% >nul
@if not exist %DEPLOY% md %DEPLOY%
@if not exist %DEPLOY1% md %DEPLOY1%
@if not exist %BUILD_AB% md %BUILD_AB%
@if not exist %BUILD_SC% md %BUILD_SC%
@if not exist %BUILD% md %BUILD%

@if exist Fonts\emoji.zip goto fonts_ok
@echo.
@echo ===== Building graphic emoji =====
@call tape.bat
:fonts_ok

@rem echo.
@rem echo ===== Pulling UTranslator =====
@rem cd utranslator
@rem git reset --hard
@rem git pull
@rem cd ..

@echo.
@echo ===== Building SmartCopy =====
@cd %BUILD_SC%
@%QTDIR%\bin\qmake.exe ..\%PRONAME_SC% -r -spec win32-g++ "CONFIG+=release"
@%MINGW%\mingw32-make.exe -f Makefile.Release -j%NUMBER_OF_PROCESSORS%
@cd ..

@echo.
@echo ===== Building AutoBuilder =====
@cd %BUILD_AB%
@%QTDIR%\bin\qmake.exe ..\%PRONAME_AB% -r -spec win32-g++ "CONFIG+=release"
@%MINGW%\mingw32-make.exe -f Makefile.Release -j%NUMBER_OF_PROCESSORS%

@echo.
@echo ===== Running AutoBuilder =====
@release\AutoBuilder.exe
@cd ..

@echo.
@echo ===== Checking for file existence =====
@if not exist %AB_UCAUTO% goto end

@echo.
@echo ===== Running SmartCopy =====
@%SMARTCOPY% %AB_UCAUTO% Unicodia\Uc\%UCAUTO%
@if errorlevel 1 goto end

@echo.
@echo ===== Building for Win64 =====
@cd %BUILD%
@%QTDIR%\bin\qmake.exe ..\%PRONAME% -r -spec win32-g++ "CONFIG+=release"
@%MINGW%\mingw32-make.exe -f Makefile.Release -j%NUMBER_OF_PROCESSORS%
@cd ..

@if not exist %BUILD%\release\%EXENAME% goto no_exe

@echo.
@echo ===== Copying files =====
@copy %BUILD%\release\%EXENAME% %DEPLOY%
@copy %MINGW%\libgcc_s_seh-1.dll %DEPLOY%
@copy "%MINGW%\libstdc++-6.dll" %DEPLOY%
@copy %MINGW%\libwinpthread-1.dll %DEPLOY%
@copy %QTDIR%\bin\Qt6Core.dll %DEPLOY%
@copy %QTDIR%\bin\Qt6Gui.dll %DEPLOY%
@copy %QTDIR%\bin\Qt6Widgets.dll %DEPLOY%
@copy %QTDIR%\bin\Qt6Svg.dll %DEPLOY%
@copy MiscFiles\Unicodia.xml %DEPLOY%
@copy LICENSE %DEPLOY%
@md %DEPLOY%\platforms
@copy %QTDIR%\plugins\platforms\qwindows.dll %DEPLOY%\platforms
@md %DEPLOY%\styles
@copy %QTDIR%\plugins\styles\qwindowsvistastyle.dll %DEPLOY%\styles
@md %DEPLOY%\Fonts
@copy Fonts\* %DEPLOY%\Fonts >nul
@if [%WORKAROUND%] == [] goto no_workaround
@%SEVENZIP% x MiscFiles\%WORKAROUND% -o%DEPLOY%
:no_workaround

@echo.
@echo ===== Building L10n =====
@md %DEPLOY%\Languages
@rem Russian
@set DIR_RU=%DEPLOY%\Languages\Russian
@md %DIR_RU%
@copy lang-src\ru\locale.xml %DIR_RU%
@%UTRANSL% lang-src\ru.uorig -build:%DIR_RU%
@copy %QTDIR%\translations\qtbase_ru.qm %DIR_RU%
@rem English
@set DIR_EN=%DEPLOY%\Languages\English
@md %DIR_EN%
@copy lang-src\en\locale.xml %DIR_EN%
@%UTRANSL% lang-src\en.utran -build:%DIR_EN%
@rem no QM for English

@echo.
@echo ===== Archiving =====
@cd %DEPLOY%
@set ARCPATH=..\%DEPLOY1%\%ARCNAME%
@if exist %ARCPATH% del %ARCPATH%
@%SEVENZIP% a %ARCPATH% * -mx9 -mmt%NUMBER_OF_PROCESSORS%
@cd ..

@goto end

:no_exe
@echo BAD: EXE NOT FOUND
@goto end

:end
@pause