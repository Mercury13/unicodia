@rem Settings
@call xsetup.bat
@rem Local unversioned setup that overrides the main one
@if exist ~setup_local.bat call ~setup_local.bat

@rem Rest things
@set /p VERSION=<VERSION.
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
@set DEPLOY2=~Installer
@set SMARTCOPY=%BUILD_SC%\release\SmartCopy.exe
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

@path %MINGW%;%PATH%

@echo My version is %VERSION%
@echo.

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
@if not exist %BUILD_AB% md %BUILD_AB%
@if not exist %BUILD_SC% md %BUILD_SC%
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
@if not exist %AB_UCLIB% goto end
@if not exist %AB_UCCOUNT% goto end
@if not exist %AB_UCSUTTON% goto end

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
@echo ===== Copying files =====
@copy %BUILD%\release\%EXENAME% %DEPLOY%
@copy %MINGW%\libgcc_s_seh-1.dll %DEPLOY%
@copy "%MINGW%\libstdc++-6.dll" %DEPLOY%
@copy %MINGW%\libwinpthread-1.dll %DEPLOY%
@copy %MINGW%\libcrypto-3-x64.dll %DEPLOY%
@copy %MINGW%\libssl-3-x64.dll %DEPLOY%
@copy %QTDIR%\bin\Qt6Core.dll %DEPLOY%
@copy %QTDIR%\bin\Qt6Gui.dll %DEPLOY%
@copy %QTDIR%\bin\Qt6Widgets.dll %DEPLOY%
@copy %QTDIR%\bin\Qt6Svg.dll %DEPLOY%
@copy %QTDIR%\bin\Qt6SvgWidgets.dll %DEPLOY%
@copy MiscFiles\Unicodia.xml %DEPLOY%
@copy LICENSE %DEPLOY%
@rem Used! — need for toolbar icons
@md %DEPLOY%\iconengines
@copy %QTDIR%\plugins\iconengines\qsvgicon.dll %DEPLOY%\iconengines
@rem Unused right now, plain SVG loading does not need this
@rem md %DEPLOY%\imageformats
@rem copy %QTDIR%\plugins\imageformats\qsvg.dll %DEPLOY%\imageformats
@md %DEPLOY%\platforms
@copy %QTDIR%\plugins\platforms\qwindows.dll %DEPLOY%\platforms
@md %DEPLOY%\styles
@copy %QTDIR%\plugins\styles\qwindowsvistastyle.dll %DEPLOY%\styles
@md %DEPLOY%\Fonts
@copy Fonts\* %DEPLOY%\Fonts >nul
@copy MiscFiles\WinManifest\Resources.pri %DEPLOY%
@copy MiscFiles\WinManifest\Unicodia.VisualElementsManifest.xml %DEPLOY%
@md %DEPLOY%\Start
@md %DEPLOY%\Start\en-US
@copy MiscFiles\WinManifest\Root\Start\en-US\*.png %DEPLOY%\Start\en-US

@echo.
@echo ===== Building L10n =====
@md %DEPLOY%\Languages
@rem ...
@rem English, original
@set DIR_EN=%DEPLOY%\Languages\English
@md %DIR_EN%
@copy lang-src\en\locale.xml %DIR_EN%
@%UTRANSL% lang-src\en.uorig -build:%DIR_EN%
@rem no QM for English
@rem ...
@rem Chinese
@set DIR_ZH=%DEPLOY%\Languages\Chinese
@md %DIR_ZH%
@copy lang-src\zh\locale.xml %DIR_ZH%
@%UTRANSL% lang-src\zh.utran -update -build:%DIR_ZH%
@copy %QTDIR%\translations\qtbase_zh_CN.qm %DIR_ZH%
@rem ...
@rem Japanese
@set DIR_JA=%DEPLOY%\Languages\Japanese
@md %DIR_JA%
@copy lang-src\ja\locale.xml %DIR_JA%
@%UTRANSL% lang-src\ja.utran -update -build:%DIR_JA%
@copy %QTDIR%\translations\qtbase_ja.qm %DIR_JA%
@rem ...
@rem Russian, known
@set DIR_RU=%DEPLOY%\Languages\Russian
@md %DIR_RU%
@copy lang-src\ru\locale.xml %DIR_RU%
@%UTRANSL% lang-src\ru.utran -rqupdate -build:%DIR_RU%
@if errorlevel 1 goto end
@copy %QTDIR%\translations\qtbase_ru.qm %DIR_RU%
@rem ...
@rem Turkish
@set DIR_TR=%DEPLOY%\Languages\Turkish
@md %DIR_TR%
@copy lang-src\tr\locale.xml %DIR_TR%
@%UTRANSL% lang-src\tr.utran -update -build:%DIR_TR%
@copy %QTDIR%\translations\qtbase_tr.qm %DIR_TR%
@rem ...
@rem Ukrainian, known
@set DIR_UK=%DEPLOY%\Languages\Ukrainian
@md %DIR_UK%
@copy lang-src\uk\locale.xml %DIR_UK%
@%UTRANSL% lang-src\uk.utran -rqupdate -build:%DIR_UK%
@if errorlevel 1 goto end
@copy %QTDIR%\translations\qtbase_uk.qm %DIR_UK%

@echo.
@echo ===== Making simple archive =====
@set ARCPATH=%DEPLOY1%\%ARCNAME%
@set ARCPATH2=%DEPLOY2%\%ARCNAME%
@if exist %ARCPATH% del %ARCPATH%
@cd %DEPLOY%
@%SEVENZIP% a ..\%ARCPATH% * -mx9 -mmt%NUMBER_OF_PROCESSORS%
@cd ..
@copy %ARCPATH% %ARCPATH2%

@echo.
@echo ===== Making installer =====
@%INNO% /Qp /O%DEPLOY2% MiscFiles/Unicodia-w64.iss

@echo.
@echo ===== Copying USesh font =====
@copy Fonts\UnicodiaSesh.ttf %DEPLOY2%

@echo.
@echo ===== Copying W7/8 compat layer =====
@copy MiscFiles\Win7.8_Compatibility.7z %DEPLOY2%

@echo.
@echo ===== Running auto-tests =====
@set QAFONTS=%BUILD%\font_layout.txt
@set QAHTTP=%BUILD%\http_test.txt
@if exist %QAFONTS% del %QAFONTS%
@if exist %QAHTTP% del %QAHTTP%
@%DEPLOY%\Unicodia.exe /qafonts:%QAFONTS% /qahttp:%QAHTTP%
@rem Checking fonts
@if exist %QAFONTS% goto fqa_exists
@echo FONTS FAILED. No font QA file for some reason.
@echo It should have been at %QAFONTS%.
@goto fqa_done
:fqa_exists
@fc %QAFONTS% AutoQa\font_layout.txt >nul
@if not errorlevel 1 goto fqa_ok
@echo.
@echo FONTS FAILED. I use updated W10, different OS can fail.
@echo The new fonts' QA is %QAFONTS%.
@goto fqa_done
:fqa_ok
@echo Fonts OK
:fqa_done

@rem Checking HTTP
@if exist %QAHTTP% goto hqa_exists
@echo HTTP FAILED. No HTTP(s) QA file for some reason.
@echo It should have been at %QAHTTP%.
@goto hqa_done
:hqa_exists
@fc %QAHTTP% AutoQa\http_test.txt >nul
@if not errorlevel 1 goto hqa_ok
@echo.
@echo HTTP(s) FAILED, it's the new file
@type %QA_HTTP%
@echo .
@goto hqa_done
:hqa_ok
@echo HTTP(s) OK
:hqa_done

@echo.

:end
@pause