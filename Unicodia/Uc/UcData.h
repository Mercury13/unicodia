#pragma once

// STL
#include <string>

// Qt
#include <QFont>

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

    enum class EcFont
    {
        NORMAL,
        GLAGOLITIC,
        NN
    };

    struct Font
    {
        std::string_view family, fileName;
        mutable struct Q {
            std::unique_ptr<QFont> table {};
            std::unique_ptr<QFont> big {};
            bool isLoaded = false;
        } q {};
        void load() const;
        const QFont& get(std::unique_ptr<QFont>& font, int size) const;
    };
    extern const Font fontInfo[static_cast<int>(EcFont::NN)];


    enum class EcScript {
        Zyyy,
        Adlm,
        Aghb,
        Ahom,
        Arab,
        Armi,
        Armn,
        Bali,
        Bamu,
        Batk,
        Beng,
        Bopo,
        Brai,
        Bugi,
        Buhd,
        Cans,
        Cham,
        Cher,
        Copt,
        Cyrl,
        Deva,
        Ethi,
        Geor,
        Glag,
        Grek,
        Gujr,
        Guru,
        Hang,
        Hani,
        Hano,
        Hebr,
        Hira,
        Kana,
        Khmr,
        Knda,
        Laoo,
        Latn,
        Mand,
        Mlym,
        Mong,
        Mymr,
        Nkoo,
        Ogam,
        Orya,
        Runr,
        Samr,
        Sinh,
        Syrc,
        Tagb,
        Taml,
        Telu,
        Tglg,
        Thaa,
        Thai,
        Tibt,
        Zinh,
        NN,

        //Afak  no chars
        //Aran  no chars
        Avst = Zyyy,        // beyond BMP
        Bass = Zyyy,        // beyond BMP
        Bhks = Zyyy,        // beyond BMP
        //Blis  no chars
        Brah = Zyyy,        // historical, beyond BMP
        Cakm = Zyyy,        // beyond BMP
        Cari = Zyyy,        // beyond BMP
        Chrs = Zyyy,        // Chorasmian, beyond BMP
        //Cirt no chars
        //Cpmn  no chars
        Cprt = Zyyy,        // beyond BMP
        //Cyrs  no chars
        Diak = Zyyy,        // Dives Akuru, beyond BMP
        Dogr = Zyyy,        // beyond BMP
        Dsrt = Zyyy,        // beyond BMP
        Dupl = Zyyy,        // beyond BMP
        //Egyd  no chars
        //Egyh  no chars
        Egyp = Zyyy,        // beyond BMP
        Elba = Zyyy,        // beyond BMP
        Elym = Zyyy,        // Elymaic, beyond BMP
        //Geok  no chars
        Gong = Zyyy,        // beyond BMP
        Gonm = Zyyy,        // beyond BMP
        Goth = Zyyy,        // beyond BMP
        Gran = Zyyy,        // beyond BMP
        //Hanb  no chars
        //Hans  no chars
        //Hant  no chars
        Hatr = Zyyy,        // beyond BMP
        Hluw = Zyyy,        // beyond BMP
        Hmng = Zyyy,        // beyond BMP
        Hmnp = Zyyy,        // beyond BMP
        //Hrkt  no chars
        Hung = Zyyy,        // beyond BMP
        //Inds  no chars
        Ital = Zyyy,        // beyond BMP
        //Jamo  no chars
        Java = Zyyy,        /// @todo [urgent] Javanese
        //Jpan  no chars
        //Jurc  no chars
        Kali = Zyyy,        /// @todo [urgent] Kayah Li
        Khar = Zyyy,        // beyond BMP
        Khoj = Zyyy,        // beyond BMP
        //Kitl  no chars
        Kits = Zyyy,        // beyond BMP
        //Kore  no chars
        //Kpel  no chars
        Kthi = Zyyy,        // beyond BMP
        Lana = Zyyy,        /// @todo [urgent] Tai Tham
        //Latf  no chars
        //Latg  no chars
        //Leke  no chars
        Lepc = Zyyy,        /// @todo [urgent] Lepcha
        Limb = Zyyy,        /// @todo [urgent] Limbu
        Lina = Zyyy,        // beyond BMP
        Linb = Zyyy,        // beyond BMP
        Lisu = Zyyy,        /// @todo [urgent] Lisu
        //Loma  no chars
        Lyci = Zyyy,        // beyond BMP
        Lydi = Zyyy,        // beyond BMP
        Mahj = Zyyy,        // Mahajani, beyond BMP
        Maka = Zyyy,        // beyond BMP
        Mani = Zyyy,        // beyond BMP
        Marc = Zyyy,        // beyond BMP
        //Maya  no chars
        Medf = Zyyy,        // beyond BMP
        Mend = Zyyy,        // beyond BMP
        Merc = Zyyy,        // beyond BMP
        Mero = Zyyy,        // beyond BMP
        Modi = Zyyy,        // beyond BMP
        //Moon  no chars
        Mroo = Zyyy,        // beyond BMP
        Mtei = Zyyy,        /// @todo [urgent] Meitei Mayek
        Mult = Zyyy,        // beyond BMP
        Nand = Zyyy,        // Nandinagari, beyond BMP
        Narb = Zyyy,        // beyond BMP
        Nbat = Zyyy,        // beyond BMP
        Newa = Zyyy,        // beyond BMP
        //Nkdb  no chars
        //Nkgb  no chars
        Nshu = Zyyy,        // beyond BMP
        Olck = Zyyy,        /// @todo [urgent] Ol Chiki
        Orkh = Zyyy,        // Turkic runes, beyond BMP
        Osge = Zyyy,        // beyond BMP
        Osma = Zyyy,        // beyond BMP
        Palm = Zyyy,        // beyond BMP
        Pauc = Zyyy,        // beyond BMP
        Perm = Zyyy,        // beyond BMP
        Phag = Zyyy,        /// @todo [urgent] Phags-pa
        Phli = Zyyy,        // beyond BMP
        Phlp = Zyyy,        // beyond BMP
        //Phlv  no chars
        Phnx = Zyyy,        // beyond BMP
        //Piqd  no chars — Klingon :)
        Plrd = Zyyy,        // beyond BMP
        Prti = Zyyy,        // beyond BMP
        Rjng = Zyyy,        /// @todo [urgent] Rejang
        Rohg = Zyyy,        // Hanifi Rohingya, beyond BMP
        //Roro  no chars
        //Sara  no chars
        Sarb = Zyyy,        // beyond BMP
        Saur = Zyyy,        /// @todo [urgent] Saurashtra
        Sgnw = Zyyy,        // SignWriting, beyond BMP
        Shaw = Zyyy,        // beyond BMP
        Shrd = Zyyy,        // beyond BMP
        //Shui  no chars
        Sidd = Zyyy,        // beyond BMP
        Sind = Zyyy,        // beyond BMP
        Sora = Zyyy,        // beyond BMP
        Sogd = Zyyy,        // beyond BMP
        Sogo = Zyyy,        // beyond BMP
        Soyo = Zyyy,        // beyond BMP
        Sund = Zyyy,        /// @todo [urgent] Sundanese
        Sylo = Zyyy,        /// @todo [urgent] Syloti Nagri
        //Syre  no chars
        //Syrj  no chars
        //Syrn  no chars
        Takr = Zyyy,        // beyond BMP
        Tale = Zyyy,        /// @todo [urgent] Tai Le
        Talu = Zyyy,        /// @todo [urgent] New Tai Lue
        Tang = Zyyy,        // beyond BMP
        Tavt = Zyyy,        /// @todo [urgent] Tai Viet
        //Teng  no chars
        Tfng = Zyyy,        /// @todo [urgent] Tifinagh
        Tirh = Zyyy,        // Tirhuta beyond BMP
        Ugar = Zyyy,        // Ugaritic beyond BMP
        Vaii = Zyyy,        /// @todo [urgent] Vai
        //Visp  no chars
        Wara = Zyyy,        // Warang Citi beyond BMP
        Wcho = Zyyy,        // Wancho beyond BMP
        //Wole  no chars
        Xpeo = Zyyy,        // Old Persian beyond BMP
        Xsux = Zyyy,        // Cuneiform beyond BMP
        Yezi = Zyyy,        // Yezidi beyond BMP
        Yiii = Zyyy,        /// @todo [urgent] Yi
        Zanb = Zyyy,        // Zanabazar beyond BMP
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
        EcFont ecFont = EcFont::NORMAL;
        mutable unsigned nChars = 0;
        mutable EcVersion ecVersion = EcVersion::UNKNOWN;

        inline const ScriptType& type() const { return scriptTypeInfo[static_cast<int>(ecType)]; }
        inline const LangLife& life() const { return langLifeInfo[static_cast<int>(ecLife)]; }
        inline const WritingDir& dir() const { return writingDirInfo[static_cast<int>(ecDir)]; }
        inline const Font& font() const { return fontInfo[static_cast<int>(ecFont)]; }
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

    constexpr int N_CHARS = 65536 * 17;
    extern Cp* cps[N_CHARS];

    void completeData();

}   // namespace uc
