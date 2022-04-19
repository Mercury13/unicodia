@rem Settings
@set VERSION=1.2.14
@set QTDIR=c:\Qt\6.1.3\mingw81_64
@set MINGW=c:\msys64\mingw64\bin
@set SEVENZIP="c:\Program Files\7-zip\7z.exe"

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

@echo ===== Creating directories =====
@if exist %DEPLOY% del /S /Q %DEPLOY%
@if exist %DEPLOY% rmdir /S /Q %DEPLOY%
@if not exist %DEPLOY% md %DEPLOY%
@if not exist %DEPLOY1% md %DEPLOY1%
@if not exist %BUILD_AB% md %BUILD_AB%
@if not exist %BUILD_SC% md %BUILD_SC%
@if not exist %BUILD% md %BUILD%

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
@copy %QTDIR%\translations\qtbase_ru.qm %DEPLOY%
@copy LICENSE %DEPLOY%
@md %DEPLOY%\platforms
@copy %QTDIR%\plugins\platforms\qwindows.dll %DEPLOY%\platforms
@md %DEPLOY%\styles
@copy %QTDIR%\plugins\styles\qwindowsvistastyle.dll %DEPLOY%\styles
@md %DEPLOY%\Fonts
@copy Fonts\* %DEPLOY%\Fonts >nul

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