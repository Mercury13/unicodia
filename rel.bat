cd ~Deploy
set ARCNAME=Unicodia-0.1.7z
set ARCPATH=..\~Deployed\%ARCNAME%
if exist %ARCPATH% del %ARCPATH%
"c:\Program Files\7-zip\7z.exe" a %ARCPATH% * -mx9
cd ..