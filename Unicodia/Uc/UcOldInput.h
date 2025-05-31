#pragma once

///
/// Legacy computers and input
///

// Unicode
#include "UcCommon.h"

// Libs
#include "u_EcArray.h"

namespace uc {

    namespace old {

        DEFINE_ENUM_TYPE_IN_NS2(uc, old, Type, unsigned char,
            HOME_EDUC_PC,
            LUXURY_PC,
            HOBBY_PC,
            ADVANCED_PC,
            TERMINAL,
            TERMINAL_SERVICE,
            INFO_SERVICE,
            PROGRAMMING_LANGUAGE)

        DEFINE_ENUM_TYPE_IN_NS2(uc, old, Sales, unsigned char,
            NOMATTER,
            OVER_10K,
            OVER_30K,
            OVER_100K,
            OVER_300K,
            OVER_1M,
            OVER_3M,
            OVER_10M)

        DEFINE_ENUM_TYPE_IN_NS2(uc, old, Country, unsigned char,
            DD, ///< East Germany
            EU, ///< All Europe
            FR, ///< France
            GB, ///< Great Britain
            JP, ///< Japan
            SU, ///< Soviet Union
            US, ///< United States
            US_HK ///< United States / Hong Kong
        )

        DEFINE_ENUM_TYPE_IN_NS2(uc, old, Graphics, unsigned char,
            NOMATTER,
            NO,
            LIMITED,
            LATER_MODELS,
            YES)

        DEFINE_ENUM_TYPE_IN_NS2(uc, old, Color, unsigned char,
            NOMATTER,
            BW,
            LATER_MODELS,
            UNSUCCESSFUL,
            YES)    ///< Actually all colour computers can display 16 colours somehow

        enum class CharType : unsigned short {
            TYPOGRAPHIC_CHARS = 1<<0,
            ARROWS = 1<<1,
            MATH = 1<<2,
            ALTERNATE_ALPHABET = 1<<3,
            ALTERNATE_DIGITS = 1<<4,
            PSEUDO_BORDERS = 1<<5,
            PSEUDO_BLOCKS = 1<<6,
            SHAPES = 1<<7,
            CONTROL_PICTURES = 1<<8,
            GAME_SPRITES = 1<<9,
            MULTICELL = 1<<10,
            ENGINEER_GRAPHICS = 1<<11,
            MISC_IMAGES = 1<<12,
            UI = 1<<13,
            // Technical
            DUMMY,
            LAST = DUMMY - 1,
        };
        DEFINE_ENUM_OPS(CharType)

        enum class Ocfg : unsigned char {
            UNENCODED   = 1<<0,  ///< Some unencoded chars remain
            NOTE_YEAR   = 1<<1,  ///< Note near year (teletext)
            NOTE_SALES  = 1<<2,  ///< Note near sales (IBM PC)
            NOTE_MEMORY = 1<<3,  ///< Note near memory (IBM PC, Mac)
        };
        DEFINE_ENUM_OPS(Ocfg)

        struct Info {
            std::string_view key;
            // Name
            std::u8string_view fixedName;
            std::u8string_view altName {};
            // Other data
            Country country;
            Type type;
            Graphics graphics;
            Color color;
            Sales sales;
            unsigned char cpuDataWidth;
            EcVersion supportedSince;
            Flags<Ocfg> flags = NO_FLAGS;
            unsigned short year;
            Flags<CharType> charTypes;
            struct Mem {
                unsigned short lo = 0, hi = 0;
            } mem;  /// Memory, in Kbytes

            std::u8string locName() const;
            std::u8string locLongName() const;
            const uc::Version& versionSupportedSince() const
                { return versionInfo[static_cast<int>(supportedSince)]; }
        };
        extern const Info info[];

        const Info* findComp(std::string_view target);

        struct CountryInfo {
            const char* key;
        };
        extern const ec::Array<CountryInfo, Country> countryInfo;

        struct TypeInfo {
            const char* key;
        };
        extern const ec::Array<TypeInfo, Type> typeInfo;

        struct GraphicsInfo {
            const char* key;
        };
        extern const ec::Array<GraphicsInfo, Graphics> graphicsInfo;

        struct SalesInfo {
            const char* key;
        };
        extern const ec::Array<SalesInfo, Sales> salesInfo;

        struct ColorInfo {
            const char* key;
        };
        extern const ec::Array<ColorInfo, Color> colorInfo;

        struct CharTypeInfo {
            const char* key;
        };
        extern const CharTypeInfo charTypeInfo[];
    }

    Flags<OldComp> cpOldComps(char32_t cp);

    struct AltCode {
        unsigned char dosCommon = 0, dosEn = 0, dosRu = 0, dosEl = 0, win = 0;
        unsigned short unicode = 0;
        bool hasLocaleIndependent() const
            { return (dosCommon != 0 || win != 0); }
        constexpr bool operator == (const AltCode& x) const = default;
    };

    struct AltgrKey {
        char key = 0, letter = 0;
        bool isTwice = false;
        constexpr bool operator == (const AltgrKey& x) const = default;
    };

    struct InputMethods {
        AltCode alt;
        AltgrKey birman;
        std::u8string_view sometimesKey;
        constexpr bool operator == (const InputMethods& x) const = default;
        static const uc::InputMethods NONE;
        bool hasSmth() const { return (*this != NONE); }
        bool hasAltCode() const { return (alt != NONE.alt); }
        bool hasBirman() const { return (birman != NONE.birman); }
    };

    InputMethods cpInputMethods(char32_t cp);
}
