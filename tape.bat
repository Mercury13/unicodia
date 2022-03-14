rem Settings
@set QTDIR=c:\Qt\6.1.3\mingw81_64
@set MINGW=c:\msys64\mingw64\bin
@set SEVENZIP="c:\Program Files\7-zip\7z.exe"
@set EMOJI=NotoEmoji

rem Rest things
@set BUILD_TAPE=~Build-Tape
@set PRONAME_TAPE=TapeMaker\TapeMaker.pro

@path %MINGW%;%PATH%

@echo.
@echo ===== Creating directories =====
@if not exist %BUILD_TAPE% md %BUILD_TAPE%

@echo.
@echo ===== Building TapeMaker =====
@cd %BUILD_TAPE%
@%QTDIR%\bin\qmake.exe ..\%PRONAME_TAPE% -r -spec win32-g++ "CONFIG+=release"
@%MINGW%\mingw32-make.exe -f Makefile.Release -j%NUMBER_OF_PROCESSORS%
@cd ..

@echo.
@echo ===== Running TapeMaker =====
@cd %EMOJI%
@..\%BUILD_TAPE%\release\TapeMaker.exe
@if exist emoji.zip del emoji.zip
@%SEVENZIP% a emoji.zip *.bin -mx9
@del *.bin
@cd ..

@echo.
@echo ===== Moving emoji ZIP =====
@if exist Fonts\emoji.zip del Fonts\emoji.zip
@move %EMOJI%\emoji.zip Fonts\emoji.zip

@pause