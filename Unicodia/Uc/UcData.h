#pragma once

// STL
#include <string>

// Unicode data
#include "UcDefines.h"

/// ALL NAMES HERE ARE IN UTF-8!

template <class T, size_t N>
const T* findInArray(std::string_view needle, const T (&haystack)[N])
{
    for (auto& v : haystack) {
        if (v.id == needle) {
            return &v;
        }
    }
    return nullptr;
}

namespace uc {

    struct Font
    {
        int index;
        std::string name;
    };


    enum class EcScript {
        Zyyy,
        Adlm,
        Aghb,
        Ahom,
        Arab,
        Armi,
        Armn,
        Beng,
        Cans,
        Cher,
        Cyrl,
        Deva,
        Ethi,
        Geor,
        Grek,
        Gujr,
        Guru,
        Hang,
        Hani,
        Hebr,
        Hira,
        Kana,
        Knda,
        Laoo,
        Latn,
        Mand,
        Mlym,
        Mymr,
        Nkoo,
        Orya,
        Samr,
        Sinh,
        Syrc,
        Taml,
        Telu,
        Thaa,
        Thai,
        Tibt,
        Zinh,
        NN,

        //Afak  no chars
        //Aran  no chars
        Avst = Zyyy,
        Bali = Zyyy,
        Bamu = Zyyy,
        Bass = Zyyy,
        Batk = Zyyy,
        Bhks = Zyyy,
        //Blis  no chars
        Bopo = Zyyy,
        Brah = Zyyy,
        Brai = Zyyy,
        Bugi = Zyyy,
        Buhd = Zyyy,
        Cakm = Zyyy,
        Cari = Zyyy,
        Cham = Zyyy,
        Chrs = Zyyy,
        Cirt = Zyyy,
        Copt = Zyyy,
        //Cpmn  no chars
        Cprt = Zyyy,
        //Cyrs  no chars
        Diak = Zyyy,
        Dogr = Zyyy,
        Dsrt = Zyyy,
        Dupl = Zyyy,
        //Egyd  no chars
        //Egyh  no chars
        Egyp = Zyyy,
        Elba = Zyyy,
        Elym = Zyyy,
        //Geok  no chars
        Glag = Zyyy,
        Gong = Zyyy,
        Gonm = Zyyy,
        Goth = Zyyy,
        Gran = Zyyy,
        //Hanb  no chars
        Hano = Zyyy,
        //Hans  no chars
        //Hant  no chars
        Hatr = Zyyy,
        Hluw = Zyyy,
        Hmng = Zyyy,
        Hmnp = Zyyy,
        //Hrkt  no chars
        Hung = Zyyy,
        Inds = Zyyy,
        Ital = Zyyy,
        //Jamo  no chars
        Java = Zyyy,
        //Jpan  no chars
        //Jurc  no chars
        Kali = Zyyy,
        Khar = Zyyy,
        Khmr = Zyyy,
        Khoj = Zyyy,
        //Kitl  no chars
        Kits = Zyyy,
        //Kore  no chars
        //Kpel  no chars
        Kthi = Zyyy,
        Lana = Zyyy,
        //Latf  no chars
        //Latg  no chars
        //Leke  no chars
        Lepc = Zyyy,
        Limb = Zyyy,
        Lina = Zyyy,
        Linb = Zyyy,
        Lisu = Zyyy,
        //Loma  no chars
        Lyci = Zyyy,
        Lydi = Zyyy,
        Mahj = Zyyy,
        Maka = Zyyy,
        Mani = Zyyy,
        Marc = Zyyy,
        //Maya  no chars
        Medf = Zyyy,
        Mend = Zyyy,
        Merc = Zyyy,
        Mero = Zyyy,
        Modi = Zyyy,
        Mong = Zyyy,
        //Moon  no chars
        Mroo = Zyyy,
        Mtei = Zyyy,
        Mult = Zyyy,
        Nand = Zyyy,
        Narb = Zyyy,
        Nbat = Zyyy,
        Newa = Zyyy,
        //Nkdb  no chars
        //Nkgb  no chars
        Nshu = Zyyy,
        Ogam = Zyyy,
        Olck = Zyyy,
        Orkh = Zyyy,
        Osge = Zyyy,
        Osma = Zyyy,
        Palm = Zyyy,
        Pauc = Zyyy,
        Perm = Zyyy,
        Phag = Zyyy,
        Phli = Zyyy,
        Phlp = Zyyy,
        //Phlv  no chars
        Phnx = Zyyy,
        //Piqd  no chars — Klingon :)
        Plrd = Zyyy,
        Prti = Zyyy,
        Rjng = Zyyy,
        Rohg = Zyyy,
        //Roro  no chars
        Runr = Zyyy,
        //Sara  no chars
        Sarb = Zyyy,
        Saur = Zyyy,
        Sgnw = Zyyy,
        Shaw = Zyyy,
        Shrd = Zyyy,
        //Shui  no chars
        Sidd = Zyyy,
        Sind = Zyyy,
        Sora = Zyyy,
        Sogd = Zyyy,
        Sogo = Zyyy,
        Soyo = Zyyy,
        Sund = Zyyy,
        Sylo = Zyyy,
        //Syre  no chars
        //Syrj  no chars
        //Syrn  no chars
        Tagb = Zyyy,
        Takr = Zyyy,
        Tale = Zyyy,
        Talu = Zyyy,
        Tang = Zyyy,
        Tavt = Zyyy,
        //Teng  no chars
        Tfng = Zyyy,
        Tglg = Zyyy,
        Tirh = Zyyy,
        Ugar = Zyyy,
        Vaii = Zyyy,
        //Visp  no chars
        Wara = Zyyy,
        Wcho = Zyyy,
        //Wole  no chars
        Xpeo = Zyyy,
        Xsux = Zyyy,
        Yezi = Zyyy,
        Yiii = Zyyy,
        Zanb = Zyyy,
        //Zmth  no chars
        //Zsye  no chars
        //Zsym  no chars
        //Zxxx  no chars
        //Zzzz  no chars
    };


    struct LangLife
    {
        std::string_view locName;
    };
    extern const LangLife langLifeInfo[static_cast<int>(EcLangLife::NN)];


    struct ScriptType
    {
        std::string_view locName;
    };
    extern const ScriptType scriptTypeInfo[static_cast<int>(EcScriptType::NN)];


    struct WritingDir
    {
        std::string_view locName;
    };
    extern const WritingDir writingDirInfo[static_cast<int>(EcWritingDir::NN)];

    struct Version
    {
        std::string_view name;
        unsigned year;

        //unsigned nNewChars = 0;
    };
    extern const Version versionInfo[static_cast<int>(EcVersion::NN)];

    struct Script
    {
        std::string_view id;
        EcScriptType ecType;
        EcLangLife ecLife;
        EcWritingDir ecDir;
        std::string_view locName, locTime, locLangs, locDescription;
        mutable unsigned nChars = 0;
        mutable EcVersion ecVersion = EcVersion::UNKNOWN;

        inline const ScriptType& type() const { return scriptTypeInfo[static_cast<int>(ecType)]; }
        inline const LangLife& life() const { return langLifeInfo[static_cast<int>(ecLife)]; }
        inline const WritingDir& dir() const { return writingDirInfo[static_cast<int>(ecDir)]; }
        const Version& version() const { return versionInfo[static_cast<int>(ecVersion)]; }
    };
    extern const Script scriptInfo[static_cast<int>(EcScript::NN)];
    inline const Script* findScript(std::string_view x) { return findInArray(x, scriptInfo); }


//    struct Category
//    {
//        std::string name;
//    };


    struct Range
    {
        std::string name;
        int startingCp = 0, endingCp = 0;
        Script* script;
    };


    struct Category
    {
        UpCategory upCat;
        std::string_view id;
        std::string_view locName;
        std::string_view locDescription;
        mutable unsigned nChars = 0;
    };

    extern const Category categoryInfo[static_cast<int>(EcCategory::NN)];
    inline const Category* findCategory(std::string_view x) { return findInArray(x, categoryInfo); }

    struct NumType
    {
        std::string_view locName;
    };
    extern const NumType numTypeInfo[static_cast<int>(EcNumType::NN)];

    struct BidiClass
    {
        std::string_view id;
        EcBidiStrength strength;
        std::string_view locName;
        std::string_view locDescription;
        mutable unsigned nChars = 0;
    };
    extern const BidiClass bidiClassInfo[static_cast<int>(EcBidiClass::NN)];
    inline const BidiClass* findBidiClass(std::string_view x) { return findInArray(x, bidiClassInfo); }

    struct Cp   // code point
    {
        char32_t subj = 0, proxy = NO_CHAR;
        struct Name {
            std::string_view tech;
            std::string_view loc;
        } name;
        EcCategory ecCategory;
        EcVersion ecVersion;
        EcBidiClass ecBidiClass;
        EcScript ecScript;
        struct Numeric {
            EcNumType ecType = EcNumType::NONE;
            long long num = 0, denom = 0;
            const NumType& type() const { return numTypeInfo[static_cast<int>(ecType)]; }
            bool isPresent() const { return (ecType != EcNumType::NONE); }
        } numeric;
        //const Range* range = nullptr;
        const Version& version() const { return versionInfo[static_cast<int>(ecVersion)]; }
        const Category& category() const { return categoryInfo[static_cast<int>(ecCategory)]; }
        const BidiClass& bidiClass() const { return bidiClassInfo[static_cast<int>(ecBidiClass)]; }
        const Script& script() const { return scriptInfo[static_cast<int>(ecScript)]; }
    };

    extern unsigned nCps();
    extern Cp cpInfo[];

    void completeData();

}   // namespace uc
