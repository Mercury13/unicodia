rem AFAIK, W10 1803+ has built-in cURL
@call ../../xsetup.bat
@if exist ../../~setup_local.bat call ../../~setup_local.bat

@echo ===== Checking for tools =====

@if exist %SEVENZIP% goto sz_ok
@echo BAD: 7-zip not found. Install it into standard directory, or change SEVENZIP variable.
@goto end
:sz_ok
@echo 7-zip OK

set VER=https://www.unicode.org/Public/18.0.0
set UCD=%VER%/ucd
curl %UCD%/DerivedAge.txt -L --output DerivedAge.txt
curl %UCD%/NameAliases.txt -L --output NameAliases.txt
curl %UCD%/NamesList.txt -L --output NamesList.txt
curl %UCD%/Scripts.txt -L --output Scripts.txt
curl %UCD%/LineBreak.txt -L --output LineBreak.txt
curl %UCD%/PropertyValueAliases.txt -L --output PropertyValueAliases.txt
curl %UCD%/DerivedCoreProperties.txt -L --output DerivedCoreProperties.txt
curl %UCD%/PropList.txt -L --output PropList.txt
curl %UCD%/Blocks.txt -L --output Blocks.txt
curl %UCD%/extracted/DerivedName.txt -L --output DerivedName.txt
curl %UCD%/UnicodeData.txt -L --output UnicodeData.txt
curl %UCD%/Unikemet.txt -L --output Unikemet.txt
curl %UCD%/Unihan.zip -L --output Unihan.zip
curl %VER%/emoji/emoji-test.txt -L --output emoji-test.txt
curl https://mjn.host.cs.st-andrews.ac.uk/egyptian/unicode/signunicode.xml -L --output signunicode.xml
curl https://mjn.host.cs.st-andrews.ac.uk/egyptian/unicode/signdescriptioneng.xml -L --output signdescriptioneng.xml
curl https://html.spec.whatwg.org/entities.json -L --output entities.json
rem Moved and changed structure, also JSON appeared → use it!
rem curl https://dev.w3.org/html5/html-author/charref --output entities.htm
rem Press F to pay respect!
rem curl https://babelstone.co.uk/CJK/IDS.TXT --output IDS.txt
del Unihan_DictionaryLikeData.txt
del Unihan_NumericValues.txt
del Unihan_IRGSources.txt
%SEVENZIP% e Unihan.zip Unihan_DictionaryLikeData.txt Unihan_NumericValues.txt Unihan_IRGSources.txt
del Unihan.zip
:end
@pause