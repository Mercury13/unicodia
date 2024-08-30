rem AFAIK, W10 1803+ has built-in cURL
set UCD=https://www.unicode.org/Public/16.0.0/ucd
curl %UCD%/DerivedAge.txt -L --output DerivedAge.txt
curl %UCD%/NameAliases.txt -L --output NameAliases.txt
curl %UCD%/NamesList.txt -L --output NamesList.txt
curl %UCD%/Scripts.txt -L --output Scripts.txt
curl %UCD%/PropertyValueAliases.txt -L --output PropertyValueAliases.txt
curl %UCD%/DerivedCoreProperties.txt -L --output DerivedCoreProperties.txt
curl %UCD%/PropList.txt -L --output PropList.txt
curl %UCD%/Blocks.txt -L --output Blocks.txt
curl %UCD%/extracted/DerivedName.txt -L --output DerivedName.txt
curl %UCD%/UnicodeData.txt -L --output UnicodeData.txt
curl %UCD%/Unihan.zip -L --output Unihan.zip
curl https://www.unicode.org/Public/draft/emoji/emoji-test.txt -L --output emoji-test.txt
curl https://mjn.host.cs.st-andrews.ac.uk/egyptian/unicode/signunicode.xml -L --output signunicode.xml
curl https://mjn.host.cs.st-andrews.ac.uk/egyptian/unicode/signdescriptioneng.xml -L --output signdescriptioneng.xml
curl https://html.spec.whatwg.org/entities.json -L --output entities.json
rem Moved and changed structure, also JSON appeared → use it!
rem curl https://dev.w3.org/html5/html-author/charref --output entities.htm
curl https://babelstone.co.uk/CJK/IDS.TXT --output IDS.txt
del Unihan_DictionaryLikeData.txt
del Unihan_NumericValues.txt
del Unihan_IRGSources.txt
"c:\Program Files\7-zip\7z.exe" e Unihan.zip Unihan_DictionaryLikeData.txt Unihan_NumericValues.txt Unihan_IRGSources.txt
del Unihan.zip
