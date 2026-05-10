@call xsetup.bat
@if exist ~setup_local.bat call ~setup_local.bat

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

@call xtape.bat

:end
@pause