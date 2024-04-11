rem AFAIK, W10 1803+ has built-in cURL
set UCD=https://www.unicode.org/Public/15.1.0/ucd
curl https://www.unicode.org/Public/15.1.0/ucdxml/ucd.all.flat.zip -L --output ucd.all.flat.zip
curl %UCD%/BidiMirroring.txt -L --output BidiMirroring.txt
curl %UCD%/DerivedAge.txt -L --output DerivedAge.txt
curl %UCD%/NameAliases.txt -L --output NameAliases.txt
curl %UCD%/NamesList.txt -L --output NamesList.txt
curl %UCD%/Scripts.txt -L --output Scripts.txt
curl %UCD%/PropertyValueAliases.txt -L --output PropertyValueAliases.txt
curl %UCD%/UnicodeData.txt -L --output UnicodeData.txt
curl https://www.unicode.org/Public/emoji/15.1/emoji-test.txt -L --output emoji-test.txt
curl https://mjn.host.cs.st-andrews.ac.uk/egyptian/unicode/signunicode.xml -L --output signunicode.xml
curl https://mjn.host.cs.st-andrews.ac.uk/egyptian/unicode/signdescriptioneng.xml -L --output signdescriptioneng.xml
curl https://html.spec.whatwg.org/entities.json -L --output entities.json
rem Moved and changed structure, also JSON appeared → use it!
rem curl https://dev.w3.org/html5/html-author/charref --output entities.htm