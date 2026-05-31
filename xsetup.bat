@rem Paths for Unicodia
@rem NOTE: You can make your own unversioned ~setup_local.bat.
@rem Scenarios:
@rem 1. 7-Zip is banned for itself, but it is a so indispensable
@rem    program that it it exists somewhere (irony).
@rem 2. Some program is installed as portable.
@rem 3. You use the newer Qt.

@rem Qt we compile against
@set QTDIR=c:\Qt\6.2.4\mingw_64

@rem MinGW we compile with; also needed for tape
@rem WARNING: Since 3.1.6, Unicodia needs OpenSSL 3
@rem  in DLL binaries, headers and libraries
@rem  (before, it had OpenSSL1.1 bins indirectly used by QtNet)
@set MINGW=c:\msys64\mingw64\bin

@rem 7-Zip, needed for everything: Unicodia, tape, data loading
@set SEVENZIP="c:\Program Files\7-zip\7z.exe"
@rem Possible replacement:
@rem set SEVENZIP="c:\Program Files\PeaZip\res\bin\7z\7z.exe"

@rem Console UTranslator
@set UTRANSL=UTransCon.exe

@rem Inno Setup
@set INNO="c:\Program Files (x86)\Inno Setup 6\ISCC.exe"


@rem As 7-zip is banned sometimes, but it's ubiquitous...
@if exist %SEVENZIP% goto szexists

@set SEVENZIP1="C:\Program Files\PeaZip\res\bin\7z\7z.exe"
@if exist %SEVENZIP1% set SEVENZIP=%SEVENZIP1%

:szexists
