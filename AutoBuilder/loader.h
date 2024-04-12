#pragma once

#include <vector>

#define UCD_ZIP "ucd.all.flat.zip"
#define UCD_XML "ucd.all.flat.xml"
#define UCD_PVA "PropertyValueAliases.txt"
#define UCD_NAMES "NamesList.txt"
#define UCD_ALIASES "NameAliases.txt"
#define UCD_SCRIPTS "Scripts.txt"
#define UCD_MIRRORING "BidiMirroring.txt"
#define UCD_UDATA "UnicodeData.txt"
#define UCD_PROP "PropList.txt"
#define UCD_BLOCKS "Blocks.txt"
#define UCD_MAIN "UnicodeData.txt"
#define DER_AGE "DerivedAge.txt"
#define DER_CORE "DerivedCoreProperties.txt"
#define DER_NAME "DerivedName.txt"
#define HAN_DIC "Unihan_DictionaryLikeData.txt"
#define HAN_NUM_VALUE "Unihan_NumericValues.txt"
#define EMOJI_TEST "emoji-test.txt"
#define ENTITIES_HTML "entities.htm"
#define ENTITIES_JSON "entities.json"
#define LIBRARY_XML "library.xml"
#define SINGLEEMOJI_TXT "single-char-emoji.txt"
#define MISCFILES "MiscFiles/"
#define RAWDATA "MiscFiles/RawData/"
#define EGYP_UNICODE "signunicode.xml"
#define EGYP_DESCRIPTION "signdescriptioneng.xml"
#define SUTTON_TXT "signwritingsequences.txt"

extern std::vector<unsigned char> memXml;

void checkLoader();
