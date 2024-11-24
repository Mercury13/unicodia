#pragma once

// C++
#include <filesystem>

// STL
#include <string_view>
#include <span>

// Unicode
#include "UcFlags.h"

#define UCD_PVA "PropertyValueAliases.txt"
#define UCD_NAMES "NamesList.txt"
#define UCD_ALIASES "NameAliases.txt"
#define UCD_SCRIPTS "Scripts.txt"
#define UCD_UDATA "UnicodeData.txt"
#define UCD_PROP "PropList.txt"
#define UCD_BLOCKS "Blocks.txt"
#define UCD_MAIN "UnicodeData.txt"
#define UCD_UNIKEMET "Unikemet.txt"
#define DER_AGE "DerivedAge.txt"
#define DER_CORE "DerivedCoreProperties.txt"
#define DER_NAME "DerivedName.txt"
#define HAN_DIC "Unihan_DictionaryLikeData.txt"
#define HAN_NUM_VALUE "Unihan_NumericValues.txt"
#define HAN_IRG "Unihan_IRGSources.txt"
#define EMOJI_TEST "emoji-test.txt"
#define ENTITIES_HTML "entities.htm"
#define ENTITIES_JSON "entities.json"
#define LIBRARY_XML "library.xml"
#define MISCFILES "MiscFiles/"
#define RAWDATA "MiscFiles/RawData/"
#define EGYP_UNICODE "signunicode.xml"
#define EGYP_DESCRIPTION "signdescriptioneng.xml"
#define SUTTON_TXT "signwritingsequences.txt"

class LocalFile
{
public:
    constexpr LocalFile(std::string_view aFname) : fname(aFname) {}
    void preload() const;
    void cleanUp() const;
    std::filesystem::path localName() const;
protected:
    std::string_view fname;
};

class OldCompInfo : public LocalFile
{
public:
    const uc::OldComp flag;
    consteval OldCompInfo(std::string_view aFname, uc::OldComp aFlag);

    template<const std::string_view& aFname>
    static consteval OldCompInfo mk(uc::OldComp aFlag);
};

void checkLoader();
void cleanUpLoader();
std::span<const OldCompInfo> allOldComps();
