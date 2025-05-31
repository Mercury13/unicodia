// My header
#include "UcOldInput.h"

// L10n
#include "LocDic.h"

// Libs
#include "u_Cmap.h"


#define NBSP "\u00A0"

constinit const uc::InputMethods uc::InputMethods::NONE {};

using ReverseMap = Cmap<char16_t, unsigned char, 128>;
template class Cmap<char16_t, unsigned char, 128>;

constexpr unsigned short operator "" _mb (unsigned long long x) { return x * 1024; }

///
/// @warning
///   skip completely because of strange purpose those characters:
///      House, Open box
///
constinit const uc::old::Info uc::old::info[] {
    // Amstrad CPC
    { .key = "AmstradCpc",
      .fixedName = u8"Amstrad" NBSP "CPC",
      .country = Country::GB,
      .type = Type::HOME_EDUC_PC,
      .graphics = Graphics::YES,
      .color = Color::YES,
      .sales = Sales::OVER_3M,
      .cpuDataWidth = 8,
      .supportedSince = uc::EcVersion::V_16_0,
      .year = 1984,
      .charTypes = CharType::ARROWS         // At least all 4
                 | CharType::MISC_IMAGES    // Watches, faces, card suits
                 | CharType::MATH           // At least circled dot/X
                 | CharType::PSEUDO_BORDERS | CharType::PSEUDO_BLOCKS
                 | CharType::GAME_SPRITES   // Men, bomb, explosion
                 | CharType::SHAPES         // Triangles, circles
                 | CharType::TYPOGRAPHIC_CHARS // 2 quotes
                 | CharType::CONTROL_PICTURES, // Substitute, delete, bell
      .mem { 64, 128 } },
    // Apple II
    { .key = "Apple2",
      .fixedName = u8"Apple" NBSP "II",
      .country = Country::US,
      .type = Type::HOBBY_PC,
      .graphics = Graphics::YES,
      .color = Color::YES,
      .sales = Sales::OVER_3M,
      .cpuDataWidth = 8,
      .supportedSince = uc::EcVersion::V_13_0,
      .flags = Ocfg::UNENCODED,
      .year = 1977,
      .charTypes = CharType::MISC_IMAGES    // Hourglass
                 | CharType::ARROWS         // At least Enter
                 | CharType::PSEUDO_BORDERS | CharType::PSEUDO_BLOCKS
                 | CharType::SHAPES
                 | CharType::CONTROL_PICTURES
                 | CharType::MULTICELL      // Running man
                 | CharType::UI,
            // Typographic chars: only …
      .mem { 4, 128 } },
    // Mattel Aquarius
    { .key = "Aquarius",
      .fixedName = u8"Mattel" NBSP "Aquarius",
      .country = Country::US_HK,
      .type = Type::HOME_EDUC_PC,
      .graphics = Graphics::NO,
      .color = Color::YES,
      .sales = Sales::OVER_10K,
      .cpuDataWidth = 8,
      .supportedSince = uc::EcVersion::V_16_0,
      .year = 1983,
      .charTypes = CharType::ARROWS         // At least 4 normal
                 | CharType::PSEUDO_BORDERS | CharType::PSEUDO_BLOCKS
                 | CharType::SHAPES         // At least 25AA
                 | CharType::GAME_SPRITES | CharType::MULTICELL
                 | CharType::MISC_IMAGES,
      .mem { 4 } },
    // Atari 400 / 800
    { .key = "Atari400",
      .fixedName = u8"Atari" NBSP "400/800",
      .country = Country::US,
      .type = Type::HOME_EDUC_PC,
      .graphics = Graphics::LIMITED,
      .color = Color::YES,
      .sales = Sales::OVER_3M,
      .cpuDataWidth = 8,
      .supportedSince = uc::EcVersion::V_13_0,
      .year = 1979,
      .charTypes = CharType::ARROWS         // At least 4 normal
                 | CharType::CONTROL_PICTURES  // At least ESC
                 | CharType::PSEUDO_BORDERS | CharType::PSEUDO_BLOCKS
                 | CharType::MISC_IMAGES,   // Card suits
      .mem { 4, 64 } },
    // Atari ST
    { .key = "AtariSt",
      .fixedName = u8"Atari" NBSP "ST",
      .country = Country::US,
      .type = Type::ADVANCED_PC,
      .graphics = Graphics::YES,
      .color = Color::YES,
      .sales = Sales::OVER_1M,
      .cpuDataWidth = 16,
      .supportedSince = uc::EcVersion::V_13_0,
      .flags = Ocfg::UNENCODED,
      .year = 1985,
      .charTypes = CharType::ARROWS         // 4 white
                 | CharType::CONTROL_PICTURES  // FF, CR, ESC, house
                 | CharType::ALTERNATE_DIGITS  // LCD
                 | CharType::MATH           // At least integral
                 | CharType::TYPOGRAPHIC_CHARS
                 | CharType::MISC_IMAGES,   // How to call those crosses? + eighth note
      .mem { 512, 4_mb } },
    // Minitel
    { .key = "Minitel",
      .fixedName = u8"Minitel",
      .country = Country::FR,
      .type = Type::TERMINAL_SERVICE,
      .graphics = Graphics::NO,
      .color = Color::LATER_MODELS,
      .sales = Sales::OVER_3M,
      .cpuDataWidth = 0,
      .supportedSince = uc::EcVersion::V_13_0,
      .year = 1982,
      .charTypes = CharType::PSEUDO_BORDERS | CharType::PSEUDO_BLOCKS
                 | CharType::ARROWS,
      .mem {} },
    // MSX
    { .key = "Msx",
      .fixedName = u8"MSX",
      .country = Country::JP,
      .type = Type::HOME_EDUC_PC,
      .graphics = Graphics::YES,
      .color = Color::YES,
      .sales = Sales::OVER_3M,
      .cpuDataWidth = 0,
      .supportedSince = uc::EcVersion::V_13_0,
      .year = 1983,
      .charTypes = CharType::TYPOGRAPHIC_CHARS | CharType::MATH
                 | CharType::PSEUDO_BORDERS | CharType::PSEUDO_BLOCKS
                 | CharType::SHAPES,   // house only among control pics
      .mem { 8, 64 } },
    // Oric
    { .key = "Oric",
      .fixedName = u8"Oric",
      .country = Country::GB,
      .type = Type::HOME_EDUC_PC,
      .graphics = Graphics::YES,
      .color = Color::YES,
      .sales = Sales::OVER_100K,
      .cpuDataWidth = 8,
      .supportedSince = uc::EcVersion::V_13_0,
      .year = 1982,
      .charTypes = CharType::ARROWS | CharType::PSEUDO_BLOCKS,
      .mem { 16, 48 } },
    // Commodore
    { .key = "C64",
      .fixedName = u8"Commodore" NBSP "PET/64",
      .altName = u8"Commodore" NBSP "64",
      .country = Country::US,
      .type = Type::HOME_EDUC_PC,
      .graphics = Graphics::YES,
      .color = Color::YES,
      .sales = Sales::OVER_10M,
      .cpuDataWidth = 8,
      .supportedSince = uc::EcVersion::V_13_0,
      .year = 1982,
      .charTypes = CharType::ARROWS
                 | CharType::PSEUDO_BORDERS | CharType::PSEUDO_BLOCKS
                 | CharType::SHAPES
                 | CharType::MISC_IMAGES,
      .mem { 64 } },
    // RISC OS
    { .key = "RiscOs",
      .fixedName = u8"RISC" NBSP "OS",
      .altName = u8"Acorn" NBSP "Archimedes" NBSP "(RISC" NBSP "OS)",
      .country = Country::GB,
      .type = Type::ADVANCED_PC,
      .graphics = Graphics::YES,
      .color = Color::YES,
      .sales = Sales::OVER_100K,
      .cpuDataWidth = 32,
      .supportedSince = uc::EcVersion::V_13_0,
      .year = 1987,
      .charTypes = CharType::TYPOGRAPHIC_CHARS
                 | CharType::ARROWS
                 | CharType::MATH
                 | CharType::MISC_IMAGES,
      .mem { 512, 16_mb } },
    // Sinclair ZX80/81
    { .key = "Zx80",
      .fixedName = u8"Sinclair" NBSP "ZX80/81",
      .country = Country::GB,
      .type = Type::HOME_EDUC_PC,
      .graphics = Graphics::NO,
      .color = Color::BW,
      .sales = Sales::OVER_1M,
      .cpuDataWidth = 8,
      .supportedSince = uc::EcVersion::V_13_0,
      .year = 1980,
      .charTypes = CharType::PSEUDO_BLOCKS,
      .mem { 1, 32 } },
    // Teletext
    { .key = "Teletext",
      .fixedName = {},
      .country = Country::EU,
      .type = Type::INFO_SERVICE,
      .graphics = Graphics::NO,
      .color = Color::LATER_MODELS,
      .sales = Sales::NOMATTER,
      .cpuDataWidth = 0,
      .supportedSince = uc::EcVersion::V_16_0,
      .flags = Ocfg::NOTE_YEAR,
      .year = 1977,
      .charTypes = CharType::TYPOGRAPHIC_CHARS // lots of quotes etc
                 | CharType::ARROWS
                 | CharType::PSEUDO_BORDERS | CharType::PSEUDO_BLOCKS
                 | CharType::SHAPES
                 | CharType::MISC_IMAGES,  // musical note
      .mem {} },
    // TRS-80
    { .key = "Trs80",
      .fixedName = u8"TRS-80",
      .altName = u8"Tandy/RadioShack" NBSP "TRS-80" NBSP "Model" NBSP "I/III/4",
      .country = Country::US,
      .type = Type::HOBBY_PC,
      .graphics = Graphics::LATER_MODELS,
      .color = Color::BW,
      .sales = Sales::OVER_300K,
      .cpuDataWidth = 8,
      .supportedSince = uc::EcVersion::V_13_0,
      .year = 1977,
      .charTypes = CharType::TYPOGRAPHIC_CHARS
                 | CharType::ARROWS
                 | CharType::MATH
                 | CharType::CONTROL_PICTURES
                 | CharType::PSEUDO_BLOCKS
                 | CharType::MISC_IMAGES
                 | CharType::MULTICELL,
      .mem { 4, 128 } },
    // Tandy CoCo
    { .key = "CoCo",
      .fixedName = u8"TRS-80" NBSP "Color",
      .altName = u8"Tandy/RadioShack" NBSP "TRS-80" NBSP "Color" NBSP "Computer",
      .country = Country::US,
      .type = Type::HOME_EDUC_PC,
      .graphics = Graphics::LATER_MODELS,
      .color = Color::YES,
      .sales = Sales::OVER_100K,
      .cpuDataWidth = 8,
      .supportedSince = uc::EcVersion::V_13_0,
      .year = 1980,
      .charTypes = CharType::ARROWS
                 | CharType::PSEUDO_BLOCKS,
      .mem { 4, 512 } },
    // Korvet
    { .key = "Korvet",
      .fixedName {},
      .country = Country::SU,
      .type = Type::HOME_EDUC_PC,
      .graphics = Graphics::YES,
      .color = Color::YES,
      .sales = Sales::OVER_100K,
      .cpuDataWidth = 8,
      .supportedSince = uc::EcVersion::V_13_0,
      .year = 1987,
      .charTypes = CharType::ARROWS
                 | CharType::SHAPES
                 | CharType::PSEUDO_BLOCKS
                 | CharType::MISC_IMAGES,
      .mem { 64 } },
    // Smalltalk
    { .key = "Smalltalk",
      .fixedName = u8"Smalltalk",
      .country = Country::US,
      .type = Type::PROGRAMMING_LANGUAGE,
      .graphics = Graphics::NOMATTER,
      .color = Color::NOMATTER,
      .sales = Sales::NOMATTER,
      .cpuDataWidth = 0,
      .supportedSince = uc::EcVersion::V_16_0,
      .year = 1972,
      .charTypes = CharType::ARROWS
                 | CharType::MISC_IMAGES,
      .mem {} },
    // Sharp MZ
    { .key = "SharpMz",
      .fixedName = u8"Sharp" NBSP "MZ",
      .country = Country::JP,
      .type = Type::HOBBY_PC,
      .graphics = Graphics::LATER_MODELS,
      .color = Color::LATER_MODELS,
      .sales = Sales::OVER_100K,
      .cpuDataWidth = 8,
      .supportedSince = uc::EcVersion::V_16_0,
      .flags = Ocfg::UNENCODED,
      .year = 1978,
      .charTypes = CharType::TYPOGRAPHIC_CHARS
                 | CharType::ARROWS
                 | CharType::CONTROL_PICTURES
                 | CharType::PSEUDO_BORDERS | CharType::PSEUDO_BLOCKS
                 | CharType::SHAPES
                 | CharType::MISC_IMAGES
                 | CharType::ENGINEER_GRAPHICS
                 | CharType::GAME_SPRITES
                 | CharType::MULTICELL
                 | CharType::ALTERNATE_ALPHABET
                 | CharType::ALTERNATE_DIGITS,
      .mem { 48 } },
    // Ohio Scientific
    { .key = "Ohio",
      .fixedName = u8"Ohio" NBSP "Scientific",
      .country = Country::US,
      .type = Type::HOME_EDUC_PC,
      .graphics = Graphics::LATER_MODELS,
      .color = Color::LATER_MODELS,
      .sales = Sales::OVER_10K,
      .cpuDataWidth = 8,
      .supportedSince = uc::EcVersion::V_16_0,
      .year = 1977,
      .charTypes = CharType::ARROWS
                 | CharType::MATH
                 | CharType::PSEUDO_BORDERS | CharType::PSEUDO_BLOCKS
                 | CharType::SHAPES
                 | CharType::MISC_IMAGES
                 | CharType::GAME_SPRITES,
      .mem { 4, 8 } },
    // Robotron
    { .key = "Robotron",
      .fixedName = u8"Robotron" NBSP "Z9001",
      .country = Country::DD,
      .type = Type::HOME_EDUC_PC,
      .graphics = Graphics::NO,
      .color = Color::LATER_MODELS,
      .sales = Sales::OVER_30K,
      .cpuDataWidth = 8,
      .supportedSince = uc::EcVersion::V_16_0,
      .year = 1984,
      .charTypes = CharType::ARROWS
                 | CharType::PSEUDO_BORDERS | CharType::PSEUDO_BLOCKS
                 | CharType::SHAPES
                 | CharType::MULTICELL
                 | CharType::GAME_SPRITES
                 | CharType::UI,
      .mem { 17 } },
    // HP terminal
    { .key = "Hp",
      .fixedName {},
      .country = Country::US,
      .type = Type::TERMINAL,
      .graphics = Graphics::LATER_MODELS,
      .color = Color::UNSUCCESSFUL,
      .sales = Sales::OVER_30K,
      .cpuDataWidth = 8,
      .supportedSince = uc::EcVersion::V_16_0,
      .year = 1974,
      .charTypes = CharType::ARROWS
                 | CharType::PSEUDO_BORDERS | CharType::PSEUDO_BLOCKS
                 | CharType::MATH
                 | CharType::ENGINEER_GRAPHICS
                 | CharType::MULTICELL,
      .mem { 1, 8 } },
    // Kaypro
    { .key = "Kaypro",
      .fixedName = u8"Kaypro",
      .country = Country::US,
      .type = Type::ADVANCED_PC,
      .graphics = Graphics::LATER_MODELS,
      .color = Color::BW,
      .sales = Sales::OVER_100K,
      .cpuDataWidth = 8,
      .supportedSince = uc::EcVersion::V_16_0,
      .year = 1982,
      .charTypes = CharType::ARROWS
                 | CharType::PSEUDO_BORDERS | CharType::PSEUDO_BLOCKS,
      .mem { 64 } },
    // IBM
    { .key = "Ibm",
      .fixedName = u8"IBM" NBSP "PC",
      .country = Country::US,
      .type = Type::ADVANCED_PC,
      .graphics = Graphics::LATER_MODELS,
      .color = Color::LATER_MODELS,
      .sales = Sales::OVER_10M,
      .cpuDataWidth = 16,
      .supportedSince = uc::EcVersion::V_1_1,
      .flags = Ocfg::NOTE_SALES | Ocfg::NOTE_MEMORY,
      .year = 1981,
      .charTypes = CharType::ARROWS
                 | CharType::PSEUDO_BORDERS | CharType::PSEUDO_BLOCKS
                 | CharType::MATH | CharType::MISC_IMAGES | CharType::SHAPES
                 | CharType::TYPOGRAPHIC_CHARS,
      .mem { 16, 4_mb } },
    // Old Motorola Macintosh
    { .key = "Mac",
      .fixedName = u8"Apple" NBSP "Macintosh",
      .country = Country::US,
      .type = Type::ADVANCED_PC,
      .graphics = Graphics::YES,
      .color = Color::LATER_MODELS,
      .sales = Sales::OVER_1M,
      .cpuDataWidth = 16,
      .supportedSince = uc::EcVersion::V_1_1,
      .flags = Ocfg::NOTE_SALES | Ocfg::NOTE_MEMORY | Ocfg::UNENCODED,
      .year = 1984,
      .charTypes = CharType::ARROWS
                 | CharType::MATH | CharType::MISC_IMAGES | CharType::SHAPES
                 | CharType::TYPOGRAPHIC_CHARS | CharType::UI,
      .mem { 128, 8_mb } },
    // ZX Spectrum
    { .key = "Spectrum",
      .fixedName = u8"ZX" NBSP "Spectrum",
      .altName = u8"Sinclair" NBSP "ZX" NBSP "Spectrum",
      .country = Country::GB,
      .type = Type::HOME_EDUC_PC,
      .graphics = Graphics::YES,
      .color = Color::YES,
      .sales = Sales::OVER_3M,
      .cpuDataWidth = 8,
      .supportedSince = uc::EcVersion::V_1_1,
      .flags = Ocfg::NOTE_MEMORY | Ocfg::NOTE_SALES,
      .year = 1982,
      .charTypes = CharType::PSEUDO_BLOCKS | CharType::ARROWS,
      .mem { 48, 128 } },
    // BBC Master
    { .key = "BbcMaster",
      .fixedName = u8"BBC" NBSP "Master",
      .country = Country::GB,
      .type = Type::ADVANCED_PC,
      .graphics = Graphics::YES,
      .color = Color::LATER_MODELS,
      .sales = Sales::OVER_300K,
      .cpuDataWidth = 8,
      .supportedSince = uc::EcVersion::V_1_1,
      .flags = NO_FLAGS,
      .year = 1986,
      .charTypes = CharType::PSEUDO_BORDERS | CharType::PSEUDO_BLOCKS | CharType::ARROWS
                   | CharType::MATH | CharType::TYPOGRAPHIC_CHARS,
      .mem { 128, 512 } },
};

constexpr auto I_LAST_OLD_COMP = std::size(uc::old::info) - 1;
static_assert((1 << I_LAST_OLD_COMP) == static_cast<int>(uc::OldComp::LAST));

std::u8string uc::old::Info::locName() const
{
    if (!fixedName.empty()) {
        return std::u8string{fixedName};
    } else {
        char buf[40];
        snprintf(buf, std::size(buf), "OldComp.%s.Name", key.data());
        std::u8string r = loc::get(buf);
        str::replace(r, u8" ", u8"" NBSP);
        return r;
    }
}

std::u8string uc::old::Info::locLongName() const
{
    if (!altName.empty()) {
        return std::u8string{altName};
    } else {
        return locName();
    }
}

const uc::old::Info* uc::old::findComp(std::string_view target)
{
    for (auto& v : info) {
        if (v.key == target) {
            return &v;
        }
    }
    return nullptr;
}


constexpr const uc::old::CountryInfo countryInfoArray[] {
    { .key = "DD" },
    { .key = "EU" },
    { .key = "FR" },
    { .key = "GB" },
    { .key = "JP" },
    { .key = "SU" },
    { .key = "US" },
    { .key = "USHK" },
};
constinit const ec::Array<uc::old::CountryInfo, uc::old::Country> uc::old::countryInfo(
        ec::ARRAY_INIT, countryInfoArray);


constexpr const uc::old::TypeInfo typeInfoArray[] {
    { .key = "Home" },
    { .key = "Desk" },
    { .key = "Adv" },
    { .key = "Term" },
    { .key = "TermSvc" },
    { .key = "Info" },
    { .key = "Prog" },
};
constinit const ec::Array<uc::old::TypeInfo, uc::old::Type> uc::old::typeInfo(
        ec::ARRAY_INIT, typeInfoArray);


constexpr const uc::old::GraphicsInfo graphicsInfoArray[] {
    { .key = "" },
    { .key = "No" },
    { .key = "Lim" },
    { .key = "Later" },
    { .key = "Yes" },
};
constinit const ec::Array<uc::old::GraphicsInfo, uc::old::Graphics> uc::old::graphicsInfo(
        ec::ARRAY_INIT, graphicsInfoArray);


constexpr const uc::old::SalesInfo salesInfoArray[] {
    { .key = "" },
    { .key = "10k" },
    { .key = "30k" },
    { .key = "100k" },
    { .key = "300k" },
    { .key = "1M" },
    { .key = "3M" },
    { .key = "10M" },
};
constinit const ec::Array<uc::old::SalesInfo, uc::old::Sales> uc::old::salesInfo(
        ec::ARRAY_INIT, salesInfoArray);


constexpr const uc::old::ColorInfo colorInfoArray[] {
    { .key = "" },
    { .key = "Bw" },
    { .key = "Later" },
    { .key = "Dis" },
    { .key = "Yes" },
};
constinit const ec::Array<uc::old::ColorInfo, uc::old::Color> uc::old::colorInfo(
        ec::ARRAY_INIT, colorInfoArray);


constinit const uc::old::CharTypeInfo uc::old::charTypeInfo[] {
    { .key = "Typ" },    // Typographic
    { .key = "Arr" },    // Arrows
    { .key = "Mat" },    // Mathematics
    { .key = "Alp" },    // Latin alphabet
    { .key = "Dig" },    // Digits
    { .key = "Lin" },    // Lines
    { .key = "Blk" },    // Blocks
    { .key = "Sha" },    // Shapes
    { .key = "Con" },    // Control pictures
    { .key = "Gam" },    // Game sprites
    { .key = "Mul" },    // Multi-cell graphics
    { .key = "Eng" },    // MultiEngineer graphics
    { .key = "Img" },    // Misc images
    { .key = "UI" },     // UI
};
constexpr auto I_LAST_CHAR_TYPE = std::size(uc::old::charTypeInfo) - 1;
static_assert((1 << I_LAST_CHAR_TYPE) == static_cast<int>(uc::old::CharType::LAST));


namespace {

    #define REV_4(x, y) {(x),(y)}, {(x)+1,(y)+1}, {(x)+2,(y)+2}, {(x)+3,(y)+3}
    #define REV_8(x, y)  REV_4(x, y), REV_4((x)+4, (y)+4)
    #define REV_16(x, y) REV_8(x, y), REV_8((x)+8, (y)+8)
    #define REV_32(x, y) REV_16(x, y), REV_16((x)+16, (y)+16)

    constinit const ReverseMap rmDosCommon {{
        { u'☺',  1 }, { u'☻',  2 }, { u'♥',  3 },  { u'♦',  4 },
        { u'♣',  5 }, { u'♠',  6 }, { u'•',  7 },  { u'◘',  8 },
        { u'○',  9 }, { u'◙', 10 }, { u'♂', 11 },  { u'♀', 12 },
        { u'♪', 13 }, { u'♫', 14 }, { u'☼', 15 },  { u'►', 16 },
        { u'◄', 17 }, { u'↕', 18 }, { u'‼', 19 },  { u'¶', 20 },
        { u'§', 21 }, { u'▬', 22 }, { u'↨', 23 },  { u'↑', 24 },
        { u'↓', 25 }, { u'→', 26 }, { u'←', 27 },  { u'∟', 28 },
        { u'↔', 29 }, { u'▲', 30 }, { u'▼', 31 },  { u'⌂', 127 },
        { u'░', 0xB0 }, { u'▒', 0xB1 }, { u'▓', 0xB2 }, { u'│', 0xB3 },
        { u'┤', 0xB4 }, { u'╡', 0xB5 }, { u'╢', 0xB6 }, { u'╖', 0xB7 },
        { u'╕', 0xB8 }, { u'╣', 0xB9 }, { u'║', 0xBA }, { u'╗', 0xBB },
        { u'╝', 0xBC }, { u'╜', 0xBD }, { u'╛', 0xBE }, { u'┐', 0xBF },
        { u'└', 0xC0 }, { u'┴', 0xC1 }, { u'┬', 0xC2 }, { u'├', 0xC3 },
        { u'─', 0xC4 }, { u'┼', 0xC5 }, { u'╞', 0xC6 }, { u'╟', 0xC7 },
        { u'╚', 0xC8 }, { u'╔', 0xC9 }, { u'╩', 0xCA }, { u'╦', 0xCB },
        { u'╠', 0xCC }, { u'═', 0xCD }, { u'╬', 0xCE }, { u'╧', 0xCF },
        { u'╨', 0xD0 }, { u'╤', 0xD1 }, { u'╥', 0xD2 }, { u'╙', 0xD3 },
        { u'╘', 0xD4 }, { u'╒', 0xD5 }, { u'╓', 0xD6 }, { u'╫', 0xD7 },
        { u'╪', 0xD8 }, { u'┘', 0xD9 }, { u'┌', 0xDA }, { u'█', 0xDB },
        { u'▄', 0xDC }, { u'▌', 0xDD }, { u'▐', 0xDE }, { u'▀', 0xDF },
        { u'■', 0xFE }, { 0xA0, 0xFF }
    }};

    constinit const ReverseMap rmDosRu {{
        REV_32(u'А', 0x80),      // Cyrillic A
        REV_16(u'а', 0xA0),      // Cyrillic a
        REV_16(u'р', 0xE0),      // Cyrillic r
        { u'Ё', 0xF0 }, { u'ё', 0xF1 }, { u'Є', 0xF2 }, { u'є', 0xF3 },
        { u'Ї', 0xF4 }, { u'ї', 0xF5 }, { u'Ў', 0xF6 }, { u'ў', 0xF7 },
        { u'°', 0xF8 }, { u'∙', 0xF9 }, { u'·', 0xFA }, { u'√', 0xFB },
        { u'№', 0xFC }, { u'¤', 0xFD }
    }};

    constinit const ReverseMap rmDosEn {{
        { u'Ç', 0x80 }, { u'ü', 0x81 }, { u'é', 0x82 }, { u'â', 0x83 },
        { u'ä', 0x84 }, { u'à', 0x85 }, { u'å', 0x86 }, { u'ç', 0x87 },
        { u'ê', 0x88 }, { u'ë', 0x89 }, { u'è', 0x8A }, { u'ï', 0x8B },
        { u'î', 0x8C }, { u'ì', 0x8D }, { u'Ä', 0x8E }, { u'Å', 0x8F },
        { u'É', 0x90 }, { u'æ', 0x91 }, { u'Æ', 0x92 }, { u'ô', 0x93 },
        { u'ö', 0x94 }, { u'ò', 0x95 }, { u'û', 0x96 }, { u'ù', 0x97 },
        { u'ÿ', 0x98 }, { u'Ö', 0x99 }, { u'Ü', 0x9A }, { u'¢', 0x9B },
        { u'£', 0x9C }, { u'¥', 0x9D }, { u'₧', 0x9E }, { u'ƒ', 0x9F },
        { u'á', 0xA0 }, { u'í', 0xA1 }, { u'ó', 0xA2 }, { u'ú', 0xA3 },
        { u'ñ', 0xA4 }, { u'Ñ', 0xA5 }, { u'ª', 0xA6 }, { u'º', 0xA6 },
        { u'¿', 0xA8 }, { u'⌐', 0xA9 }, { u'¬', 0xAA }, { u'½', 0xAB },
        { u'¼', 0xAC }, { u'¡', 0xAD }, { u'«', 0xAE }, { u'»', 0xAF },
        { u'α', 0xE0 }, { u'ß', 0xE1 }, { u'Γ', 0xE2 }, { u'π', 0xE3 },
        { u'Σ', 0xE4 }, { u'σ', 0xE5 }, { u'µ', 0xE6 }, { u'τ', 0xE7 },
        { u'Φ', 0xE8 }, { u'Θ', 0xE9 }, { u'Ω', 0xEA }, { u'δ', 0xEB },
        { u'∞', 0xEC }, { u'φ', 0xED }, { u'ε', 0xEE }, { u'∩', 0xEF },
        { u'≡', 0xF0 }, { u'±', 0xF1 }, { u'≥', 0xF2 }, { u'≤', 0xF3 },
        { u'⌠', 0xF4 }, { u'⌡', 0xF5 }, { u'÷', 0xF6 }, { u'≈', 0xF7 },
        { u'°', 0xF8 }, { u'∙', 0xF9 }, { u'·', 0xFA }, { u'√', 0xFB },
        { u'ⁿ', 0xFC }, { u'²', 0xFD }
    }};

    constinit const ReverseMap rmDosEl {{
        REV_16(u'Α', 0x80),        // Greek Α
        { u'Ρ', 0x90 }, { u'Σ', 0x91 }, { u'Τ', 0x92 }, { u'Υ', 0x93 },
        { u'Φ', 0x94 }, { u'Χ', 0x95 }, { u'Ψ', 0x96 }, { u'Ω', 0x97 },
        REV_8 (u'α', 0x98),
        REV_16(u'ι', 0xA0),
        { u'ω', 0xe0 }, { u'ά', 0xE1 }, { u'έ', 0xE2 }, { u'ή', 0xE3 },
        { u'ϊ', 0xE4 }, { u'ί', 0xE5 }, { u'ό', 0xE6 }, { u'ύ', 0xE7 },
        { u'ϋ', 0xe8 }, { u'ώ', 0xE9 }, { u'Ά', 0xEA }, { u'Έ', 0xEB },
        { u'Ή', 0xEC }, { u'Ί', 0xED }, { u'Ό', 0xEE }, { u'Ύ', 0xEF },
        { u'Ώ', 0xF0 }, { u'±', 0xF1 }, { u'≥', 0xF2 }, { u'≤', 0xF3 },
        { u'Ϊ', 0xF4 }, { u'Ϋ', 0xF5 }, { u'÷', 0xF6 }, { u'≈', 0xF7 },
        { u'°', 0xF8 }, { u'∙', 0xF9 }, { u'·', 0xFA }, { u'√', 0xFB },
        { u'ⁿ', 0xFC }, { u'²', 0xFD }
    }};

    constinit const ReverseMap rmWin {{
        { u'€', 128 }, { u'‚', 130 }, { u'ƒ', 131 }, { u'„', 132 },
        { u'…', 133 }, { u'†', 134 }, { u'‡', 135 }, { u'ˆ', 136 },
        { u'‰', 137 }, { u'Š', 138 }, { u'‹', 139 }, { u'Œ', 140 },
        { u'Ž', 142 }, { u'‘', 145 }, { u'’', 146 }, { u'“', 147 },
        { u'”', 148 }, { u'•', 149 }, { u'–', 150 }, { u'—', 151 },
        { u'˜', 152 }, { u'™', 153 }, { u'š', 154 }, { u'›', 155 },
        { u'œ', 156 }, { u'ž', 158 }, { u'Ÿ', 159 },
    }};

    using AltgrPair = std::pair<char16_t, uc::AltgrKey>;

    consteval AltgrPair altgr(char16_t subj, char key)
        { return { subj, { key } }; }

    consteval AltgrPair altgruml(char16_t subj, char key)
        { return { subj, { key, 0, true } }; }

    constinit Cmap kmBirman {{
        altgr ( 0xA0, ' ' ),
        altgruml ( u'̀', '~' ),
            { u'ẁ', { '~', 'w' } },
            { u'è', { '~', 'e' } },
            { u'ỳ', { '~', 'y' } },
            { u'ù', { '~', 'u' } },
            { u'ì', { '~', 'i' } },
            { u'ò', { '~', 'o' } },
            { u'à', { '~', 'a' } },
            { u'ǹ', { '~', 'n' } },
            { u'Ẁ', { '~', 'W' } },
            { u'È', { '~', 'E' } },
            { u'Ỳ', { '~', 'Y' } },
            { u'Ù', { '~', 'U' } },
            { u'Ì', { '~', 'I' } },
            { u'Ò', { '~', 'O' } },
            { u'À', { '~', 'A' } },
            { u'Ǹ', { '~', 'N' } },
        altgr ( u'¹', '1' ),
        altgr ( u'¡', '!' ),
        altgr ( u'²', '2' ),
        altgr ( u'½', '@' ),
        altgr ( u'³', '3' ),
        altgr ( u'⅓', '#' ),
        altgr ( u'$', '4' ),
        altgr ( u'¼', '$' ),
        altgr ( u'‰', '5' ),
        altgr ( u'↑', '6' ),
        altgruml ( u'̂', '^' ),
            { u'ŵ', { '^', 'w' } },
            { u'ê', { '^', 'e' } },
            { u'ŷ', { '^', 'y' } },
            { u'û', { '^', 'u' } },
            { u'î', { '^', 'i' } },
            { u'ô', { '^', 'o' } },
            { u'â', { '^', 'a' } },
            { u'ŝ', { '^', 's' } },
            { u'ĝ', { '^', 'g' } },
            { u'ĥ', { '^', 'h' } },
            { u'ĵ', { '^', 'j' } },
            { u'ẑ', { '^', 'z' } },
            { u'ĉ', { '^', 'c' } },
            { u'Ŵ', { '^', 'W' } },
            { u'Ê', { '^', 'E' } },
            { u'Ŷ', { '^', 'Y' } },
            { u'Û', { '^', 'U' } },
            { u'Î', { '^', 'I' } },
            { u'Ô', { '^', 'O' } },
            { u'Â', { '^', 'A' } },
            { u'Ŝ', { '^', 'S' } },
            { u'Ĝ', { '^', 'G' } },
            { u'Ĥ', { '^', 'H' } },
            { u'Ĵ', { '^', 'J' } },
            { u'Ẑ', { '^', 'Z' } },
            { u'Ĉ', { '^', 'C' } },
        altgr ( u'¿', '&' ),
        altgr ( u'∞', '8' ),
        altgr ( u'←', '9' ),
        altgr ( u'‹', '(' ),
        altgr ( u'→', '0' ),
        altgr ( u'›', ')' ),
        altgr ( u'—', '-' ),
        altgr ( u'–', '_' ),
        altgr ( u'≠', '=' ),
        altgr ( u'±', '+' ),
        altgruml ( u'̆', 'Q' ),
            { u'ĕ', { 'Q', 'e' } },
            { u'ŭ', { 'Q', 'u' } },
            { u'ĭ', { 'Q', 'i' } },
            { u'ŏ', { 'Q', 'o' } },
            { u'ă', { 'Q', 'a' } },
            { u'ğ', { 'Q', 'g' } },
            { u'Ĕ', { 'Q', 'E' } },
            { u'Ŭ', { 'Q', 'U' } },
            { u'Ĭ', { 'Q', 'I' } },
            { u'Ŏ', { 'Q', 'O' } },
            { u'Ă', { 'Q', 'A' } },
            { u'Ğ', { 'Q', 'G' } },
        altgr ( u'✓', 'w' ),
        altgr ( u'⌃', 'W' ),
        altgr ( u'€', 'e' ),
        altgr ( u'⌥', 'E' ),
        altgr ( u'®', 'r' ),
        altgruml ( u'̊', 'R' ),
            { u'ů', { 'R', 'u' } },
            { u'å', { 'R', 'a' } },
            { u'Ů', { 'R', 'U' } },
            { u'Å', { 'R', 'A' } },
        altgr ( u'™', 't' ),
        altgr ( u'ѣ', 'y' ),
        altgr ( u'Ѣ', 'Y' ),
        altgr ( u'ѵ', 'u' ),
        altgr ( u'Ѵ', 'U' ),
        altgr ( u'і', 'i' ),
        altgr ( u'І', 'I' ),
        altgr ( u'ѳ', 'o' ),
        altgr ( u'Ѳ', 'O' ),
        altgr ( u'′', 'p' ),
        altgr ( u'″', 'P' ),
        altgr ( u'[', '[' ),
        altgr ( u'{', '{' ),
        altgr ( u']', ']' ),
        altgr ( u'}', '}' ),
        altgr ( u'≈', 'a' ),
        altgr ( u'⌘', 'A' ),
        altgr ( u'§', 's' ),
        altgr ( u'⇧', 'S' ),
        altgr ( u'°', 'd' ),
        altgr ( u'⌀', 'D' ),
        altgr ( u'£', 'f' ),
        altgr ( u'₽', 'h' ),
        altgruml ( u'̋', 'H' ),
            { u'ő', { 'H', 'o' } },
            { u'ű', { 'H', 'u' } },
            { u'Ő', { 'H', 'O' } },
            { u'Ű', { 'H', 'U' } },
        altgr ( u'„', 'j' ),
        altgr ( u'“', 'k' ),
        altgr ( u'”', 'l' ),
        altgr ( u'‘', ';' ),
        altgruml ( u'̈', ':' ),
            { u'ẅ', { ':', 'w' } },
            { u'ë', { ':', 'e' } },
            { u'ÿ', { ':', 'y' } },
            { u'ü', { ':', 'u' } },
            { u'ï', { ':', 'i' } },
            { u'ö', { ':', 'o' } },
            { u'ä', { ':', 'a' } },
            { u'ẍ', { ':', 'x' } },
            { u'Ẅ', { ':', 'W' } },
            { u'Ë', { ':', 'E' } },
            { u'Ÿ', { ':', 'Y' } },
            { u'Ü', { ':', 'U' } },
            { u'Ï', { ':', 'I' } },
            { u'Ö', { ':', 'O' } },
            { u'Ä', { ':', 'A' } },
            { u'Ẍ', { ':', 'X' } },
        altgr ( u'’', '\'' ),
        altgr ( u'\'', '"' ),
        altgruml ( u'̧', 'Z' ),
            { u'ȩ', { 'Z', 'e' } },
            { u'ŗ', { 'Z', 'r' } },
            { u'ţ', { 'Z', 't' } },
            { u'ş', { 'Z', 's' } },
            { u'ḑ', { 'Z', 'd' } },
            { u'ģ', { 'Z', 'g' } },
            { u'ḩ', { 'Z', 'h' } },
            { u'ķ', { 'Z', 'k' } },
            { u'ļ', { 'Z', 'l' } },
            { u'ç', { 'Z', 'c' } },
            { u'ņ', { 'Z', 'n' } },
            { u'Ȩ', { 'Z', 'E' } },
            { u'Ŗ', { 'Z', 'R' } },
            { u'Ţ', { 'Z', 'T' } },
            { u'Ş', { 'Z', 'S' } },
            { u'Ḑ', { 'Z', 'D' } },
            { u'Ģ', { 'Z', 'G' } },
            { u'Ḩ', { 'Z', 'H' } },
            { u'Ķ', { 'Z', 'K' } },
            { u'Ļ', { 'Z', 'L' } },
            { u'Ç', { 'Z', 'C' } },
            { u'Ņ', { 'Z', 'N' } },
        altgr ( u'×', 'x' ),
        altgr ( u'·', 'X' ),
        altgr ( u'©', 'c' ),
        altgr ( u'¢', 'C' ),
        altgr ( u'↓', 'v' ),
        altgruml ( u'̌', 'V' ),
            { u'ě', { 'V', 'e' } },
            { u'ř', { 'V', 'r' } },
            { u'ť', { 'V', 't' } },
            { u'ǔ', { 'V', 'u' } },
            { u'ǐ', { 'V', 'i' } },
            { u'ǒ', { 'V', 'o' } },
            { u'ǎ', { 'V', 'a' } },
            { u'š', { 'V', 's' } },
            { u'ď', { 'V', 'd' } },
            { u'ǧ', { 'V', 'g' } },
            { u'ȟ', { 'V', 'h' } },
            { u'ǰ', { 'V', 'j' } },
            { u'ǩ', { 'V', 'k' } },
            { u'ž', { 'V', 'z' } },
            { u'č', { 'V', 'c' } },
            { u'ň', { 'V', 'n' } },
            { u'Ě', { 'V', 'E' } },
            { u'Ř', { 'V', 'R' } },
            { u'Ť', { 'V', 'T' } },
            { u'Ǔ', { 'V', 'U' } },
            { u'Ǐ', { 'V', 'I' } },
            { u'Ǒ', { 'V', 'O' } },
            { u'Ǎ', { 'V', 'A' } },
            { u'Š', { 'V', 'S' } },
            { u'Ď', { 'V', 'S' } },
            { u'Ǧ', { 'V', 'G' } },
            { u'Ȟ', { 'V', 'H' } },
            //{ u'ǰ', { 'V', 'J' } },  no cap letter J-caron here
            { u'Ǩ', { 'V', 'K' } },
            { u'Ž', { 'V', 'Z' } },
            { u'Č', { 'V', 'C' } },
            { u'Ň', { 'V', 'N' } },
        altgr ( u'ß', 'b' ),
        altgr ( u'ẞ', 'B' ),
        altgruml ( u'̃', 'N' ),
            { u'ỹ', { 'N', 'y' } },
            { u'ũ', { 'N', 'u' } },
            { u'ĩ', { 'N', 'i' } },
            { u'õ', { 'N', 'o' } },
            { u'ã', { 'N', 'a' } },
            { u'ṽ', { 'N', 'v' } },
            { u'ñ', { 'N', 'n' } },
            { u'Ỹ', { 'N', 'Y' } },
            { u'Ũ', { 'N', 'U' } },
            { u'Ĩ', { 'N', 'I' } },
            { u'Õ', { 'N', 'O' } },
            { u'Ã', { 'N', 'A' } },
            { u'Ṽ', { 'N', 'V' } },
            { u'Ñ', { 'N', 'N' } },
        altgr ( u'−', 'm' ),
        altgr ( u'•', 'm' ),
        altgr ( u'«', ',' ),
        //altgr ( u'„', '<' ),  need multimap!
        altgr ( u'»', '.' ),
        altgr ( u'…', '/' ),
        altgruml ( u'́', '?' ),
            { u'ẃ', { '?', 'w' } },
            { u'é', { '?', 'e' } },
            { u'ŕ', { '?', 'r' } },
            { u'ý', { '?', 'y' } },
            { u'ú', { '?', 'u' } },
            { u'í', { '?', 'i' } },
            { u'ó', { '?', 'o' } },
            { u'ṕ', { '?', 'p' } },
            { u'á', { '?', 'a' } },
            { u'ś', { '?', 's' } },
            { u'ǵ', { '?', 'g' } },
            { u'ĺ', { '?', 'l' } },
            { u'ź', { '?', 'z' } },
            { u'ć', { '?', 'c' } },
            { u'ń', { '?', 'n' } },
            { u'ḿ', { '?', 'm' } },
            { u'Ẃ', { '?', 'W' } },
            { u'É', { '?', 'E' } },
            { u'Ŕ', { '?', 'R' } },
            { u'Ý', { '?', 'Y' } },
            { u'Ú', { '?', 'U' } },
            { u'Í', { '?', 'I' } },
            { u'Ó', { '?', 'O' } },
            { u'Ṕ', { '?', 'P' } },
            { u'Á', { '?', 'A' } },
            { u'Ś', { '?', 'S' } },
            { u'Ǵ', { '?', 'G' } },
            { u'Ĺ', { '?', 'L' } },
            { u'Ź', { '?', 'Z' } },
            { u'Ć', { '?', 'C' } },
            { u'Ń', { '?', 'N' } },
            { u'Ḿ', { '?', 'M' } },
    }};

}   // anon namespace

uc::InputMethods uc::cpInputMethods(char32_t cp)
{
    uc::InputMethods r;
    if (cp < 127) {   // ASCII characters except 7F
        switch (cp) {
        case 13:
        case 10:
            r.sometimesKey = u8"{{k|Enter}}, {{k|Shift/Ctrl/Alt|Enter}}";
            break;
        case 9:
            r.sometimesKey = u8"{{k|Tab}}";
            break;
        default:;
        }
    } else if (cp >= 0xA0 && cp < 0x10000) {  // Rest of BMP
        rmDosCommon.query(cp, r.alt.dosCommon);
        rmDosEn.query(cp, r.alt.dosEn);
        rmDosRu.query(cp, r.alt.dosRu);
        rmDosEl.query(cp, r.alt.dosEl);
        if (cp <= 0xFF) {     // ISO1
            r.alt.win = cp;
        } else {
            rmWin.query(cp, r.alt.win);
        }
        r.alt.unicode = cp;
        kmBirman.query(cp, r.birman);
    }
    return r;
}


Flags<uc::OldComp> uc::cpOldComps(char32_t cp)
{
    auto beg = uc::oldCompSpans;
    auto end = beg + N_OLDCOMP_SPANS;
    auto it = std::upper_bound(beg, end, cp);
    if (it == beg)
        return {};
    --it;
    cp -= it->firstCp;
    if (cp < it->span.size())
        return it->span[cp];
    return {};
}
