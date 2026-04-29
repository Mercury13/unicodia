@rem Paths for Unicodia

@rem Qt we compile against
@set QTDIR=c:\Qt\6.2.4\mingw_64

@rem Workaround for Qt 4.2, maybe QtcW7Compat.7z
@set WORKAROUND=

@rem MinGW we compile with; also needed for tape
@set MINGW=c:\msys64\mingw64\bin

@rem 7-Zip, needed for everything: Unicodia, tape, data loading
@set SEVENZIP="c:\Program Files\7-zip\7z.exe"

@rem Comsole UTranslator
@set UTRANSL=UTransCon.exe

@rem Inno Setup
@set INNO="c:\Program Files (x86)\Inno Setup 6\ISCC.exe"
