@set PRONAME=Unicodia\Unicodia.pro
@set EXENAME=Unicodia.exe
@set VERSION=1.2.1
@set ARCNAME=Unicodia-w64-%VERSION%.7z
@set BUILD=~Build-win64
@set DEPLOY=~Deploy
@set DEPLOY1=~Deployed
@set MINGW=c:\msys64\mingw64\bin
@set SEVENZIP="c:\Program Files\7-zip\7z.exe"
@set QTDIR=c:\Qt\6.1.3\mingw81_64

@echo ===== Creating directories =====
@if exist %DEPLOY% del /S /Q %DEPLOY%
@if exist %DEPLOY% rmdir /S /Q %DEPLOY%
@if not exist %DEPLOY% md %DEPLOY%
@if not exist %DEPLOY1% md %DEPLOY1%
@if not exist %BUILD% md %BUILD%

@echo ===== Building for Win64 =====
@path %MINGW%;%PATH%
@cd %BUILD%
@%QTDIR%\bin\qmake.exe ..\%PRONAME% -r -spec win32-g++ "CONFIG+=release"
@%MINGW%\mingw32-make.exe -f Makefile.Release -j%NUMBER_OF_PROCESSORS%
@cd ..

@if not exist %BUILD%\release\%EXENAME% goto no_exe

@echo ===== Copying files =====
@copy %BUILD%\release\%EXENAME% %DEPLOY%
@copy %MINGW%\libgcc_s_seh-1.dll %DEPLOY%
@copy "%MINGW%\libstdc++-6.dll" %DEPLOY%
@copy %MINGW%\libwinpthread-1.dll %DEPLOY%
@copy %QTDIR%\bin\Qt6Core.dll %DEPLOY%
@copy %QTDIR%\bin\Qt6Gui.dll %DEPLOY%
@copy %QTDIR%\bin\Qt6Widgets.dll %DEPLOY%
@copy %QTDIR%\translations\qtbase_ru.qm %DEPLOY%
@copy LICENSE %DEPLOY%
@md %DEPLOY%\platforms
@copy %QTDIR%\plugins\platforms\qwindows.dll %DEPLOY%\platforms
@md %DEPLOY%\styles
@copy %QTDIR%\plugins\styles\qwindowsvistastyle.dll %DEPLOY%\styles
@md %DEPLOY%\Fonts
@copy Fonts\* %DEPLOY%\Fonts >nul

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