@rem Settings
@call xsetup.bat
@rem Local unversioned setup that overrides the main one
@if exist ~setup_local.bat call ~setup_local.bat

@rem Rest things
@set /p VERSION=<VERSION.
@set PRONAME=Unicodia\Unicodia.pro
@set PRONAME_AB=AutoBuilder\AutoBuilder.pro
@set EXENAME=Unicodia.exe
@set ARCNAME=Unicodia-w64-%VERSION%.7z
@set BUILD=~Build-win64
@set BUILD_AB=~Build-AB-win64
@set BUILD_SC=~Build-SC-win64
@set DEPLOY=~Deploy
@set DEPLOY1=~Deployed
@set DEPLOY2=~Installer
@set SMARTCOPY=%BUILD_SC%\SmartCopy.exe
@set UCAUTO=UcAuto.cpp
@set UCLIB=UcAutoLib.cpp
@set UCCOUNT=UcAutoCount.h
@set UCSUTTON=UcAutoSutton.cpp
@set UCSCRIPT=UcAutoScripts.h
@set UCOLDCOMP=UcAutoOldComp.cpp
@set AB_UCAUTO=%BUILD_AB%/%UCAUTO%
@set AB_UCLIB=%BUILD_AB%/%UCLIB%
@set AB_UCCOUNT=%BUILD_AB%/%UCCOUNT%
@set AB_UCSUTTON=%BUILD_AB%/%UCSUTTON%
@set AB_UCSCRIPT=%BUILD_AB%/%UCSCRIPT%
@set AB_UCOLDCOMP=%BUILD_AB%/%UCOLDCOMP%
@set CMAKE_CMD=%CMAKE% -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=%MINGW% -DCMAKE_BUILD_TYPE=Release
@set CMAKE_PAR=--config Release -j%NUMBER_OF_PROCESSORS%

@path %MINGW%;%PATH%

@echo My version is %VERSION%
@echo.

@echo ===== Checking for tools =====

@if exist %SEVENZIP% goto sz_ok
@echo BAD: 7-zip not found. Install it into standard directory, or change SEVENZIP variable.
@goto end
:sz_ok
@echo 7-zip OK

@if exist %CMAKE% goto cm_ok
@echo BAD: CMake not found. It's the main build system of Qt now.
@goto end
:cm_ok
@echo CMake OK

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
@echo BAD: UTransCon not found.
@echo Build it manually from https://github.com/Mercury13/utranslator
@goto end
:utransl_ok
@echo UTranslator OK

@if exist %INNO% goto inno_ok
@echo BAD: Inno Setup not found. It’s a well-known free program.
@goto end
:inno_ok
@echo Inno Setup OK

@echo ===== Creating directories =====
@if exist %DEPLOY% del /S /Q %DEPLOY% >nul
@if exist %DEPLOY% rmdir /S /Q %DEPLOY% >nul
@if exist %DEPLOY2% del /S /Q %DEPLOY2% >nul
@if exist %DEPLOY2% rmdir /S /Q %DEPLOY2% >nul
@if not exist %DEPLOY% md %DEPLOY%
@if not exist %DEPLOY1% md %DEPLOY1%
@if not exist %DEPLOY2% md %DEPLOY2%
@if not exist %BUILD% md %BUILD%

@set EMOJIARC=Fonts\emoji.zip
@if exist %EMOJIARC% goto emoji_ok
@echo.
@echo ===== Building graphic emoji =====
@call xtape.bat
@if exist %EMOJIARC% goto emoji_ok
@echo Emoji archive %EMOJIARC% not found!
@goto end
:emoji_ok

@echo.
@echo ===== Building SmartCopy =====
@%CMAKE_CMD% -S ./SmartCopy -B ./%BUILD_SC%
@%CMAKE% --build ./%BUILD_SC% %CMAKE_PAR%
@if not exist %SMARTCOPY% goto end

@echo.
@echo ===== Building AutoBuilder =====
@%CMAKE_CMD% -S ./AutoBuilder -B ./%BUILD_AB%
@%CMAKE% --build ./%BUILD_AB% %CMAKE_PAR%
@if not exist %BUILD_AB%/AutoBuilder.exe goto end

@echo.
@echo ===== Running AutoBuilder =====
@cd %BUILD_AB%
@AutoBuilder.exe
@cd ..

@echo.
@echo ===== Checking for file existence =====
@if not exist %AB_UCAUTO% goto end
@if not exist %AB_UCLIB% goto end
@if not exist %AB_UCCOUNT% goto end
@if not exist %AB_UCSUTTON% goto end
@if not exist %AB_UCSCRIPT% goto end
@if not exist %AB_UCOLDCOMP% goto end

@echo.
@echo ===== Running SmartCopy =====
@%SMARTCOPY% %AB_UCAUTO% Unicodia\Uc\%UCAUTO%
@%SMARTCOPY% %AB_UCLIB% Unicodia\Uc\%UCLIB%
@%SMARTCOPY% %AB_UCCOUNT% Unicodia\Uc\%UCCOUNT%
@%SMARTCOPY% %AB_UCSUTTON% Unicodia\Uc\%UCSUTTON%
@%SMARTCOPY% %AB_UCSCRIPT% Unicodia\Uc\%UCSCRIPT%
@%SMARTCOPY% %AB_UCOLDCOMP% Unicodia\Uc\%UCOLDCOMP%
@if errorlevel 1 goto end

@echo.
@echo ===== Building for Win64 =====
@cd %BUILD%
@%QTDIR%\bin\qmake.exe ..\%PRONAME% -r -spec win32-g++ "CONFIG+=release"
@%MINGW%\mingw32-make.exe -f Makefile.Release -j%NUMBER_OF_PROCESSORS%
@cd ..

@if exist %BUILD%\release\%EXENAME% goto exe_ok
@echo BAD: EXE NOT FOUND
@goto end
:exe_ok

@echo.

:end
@pause
