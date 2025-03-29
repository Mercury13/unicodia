#include "UcData.h"

#include "Skin.h"

consteval uc::StyleSheet operator "" _top (unsigned long long x)
    { return uc::StyleSheet{ .topPc = static_cast<short>(x), .botPc = 0 } ; }

consteval uc::StyleSheet operator "" _bot (unsigned long long x)
    { return uc::StyleSheet{ .topPc = 0, .botPc = static_cast<short>(x) } ; }

consteval uc::StyleSheet operator + (
        const uc::StyleSheet& x, const uc::StyleSheet& y)
    { return { .topPc = short(x.topPc + y.topPc), .botPc = short(x.botPc + y.botPc) }; }

#define FAM_DEFAULT { FACE_DEFAULT, Fafg::BUILTIN }

// File names
constexpr uc::Family FNAME_NOTO { "NotoSerif-Regular.ttf", uc::Fafg::DEHINT_DOTC };
constexpr uc::Family FNAME_NOTOMATH { "NotoSansMath-Regular.ttf" };
constexpr std::string_view FNAME_NOTOSYM1 = "NotoSansSymbols-Regular.ttf";
constexpr uc::Family FNAME_NOTOSYM2 { "NotoSansSymbols2-Regular.ttf" };
constexpr uc::Family FNAME_NOTOMUSIC { "NotoMusic-Regular.ttf" };
constexpr uc::Family FNAME_MUSIC_FIXUP { "UnicodiaMusicFixup.ttf" };
constexpr std::string_view FNAME_DEJAVU = "DejaVuSerif.ttf";
constexpr std::string_view FNAME_FUNKY = "UnicodiaFunky.ttf";
constexpr uc::Family FAM_FUNKY { FNAME_FUNKY };
constexpr uc::Family FNAME_HANA_C { "HanaMinLiteCSC.ttf" };
constexpr std::string_view FNAME_BABEL_01 = "BabelStoneHanBasic.ttf";
constexpr uc::Family FAM_BABEL_23 { "BabelStoneHanExtra.ttf" };
constexpr uc::Family FNAME_KOREAN = { "UtoSansKR-Regular.ttf" };
constexpr uc::Family FNAME_UNIHAN { "UnicodiaHan.ttf" };

constexpr uc::Family FAM_YU { "Yu Gothic", uc::ProbeChar { 0x3042 } };
constexpr uc::Family FAM_HISTORIC { "Segoe UI Historic", uc::ProbeChar { 0x11013 } };
constexpr uc::Family FAM_SEMOJI { "Segoe UI Emoji", uc::ProbeChar { 0x1F600 } };
constexpr uc::Family FAM_NAND { "NotoSansNandinagari-Regular.ttf", uc::Fafg::DEHINT_DOTC };
constexpr uc::Family FAM_SANS_FRANKEN { "UtoSansFranken.ttf", uc::Fafg::DEHINT_DOTC };

constexpr uc::Family FNAME_DEVA { "NotoSerifDevanagari-Regular.ttf", uc::Fafg::DEHINT_DOTC };
constexpr std::string_view FNAME_DEVAFIX = "UnicodiaDevaFixup-Regular.ttf";
constexpr uc::StyleSheet STYLE_NONE {};
constexpr auto STYLE_DEVA = STYLE_NONE;
constexpr auto SIZE_DEVA = 110_pc;

char32_t recodeCuneiform(char32_t x)
{
    /// @todo [U16, gag] Recode cuneiform
    switch (x) {
    case 0x12326: return 0x12327;
    case 0x12327: return 0x12326;
    default: return x;
    }
}

char32_t recodeBengali(char32_t x)
{
    /// @todo [U17, gag] Recode Bengali
    switch (x) {
    case 0x09FF: return 0x09F0;
    default: return x;
    }
}

char32_t recodeOriya(char32_t x)
{
    /// @todo [U17, gag] Recode Oriya
    switch (x) {
    case 0x0B53: return 0x307;
    case 0x0B54: return 0x308;
    default: return x;
    }
}

char32_t recodePseudographica(char32_t x)
{
    /// @todo [U17, gag] Recode Pseudographica
    switch (x) {
    // Legacy ex span 1 (3 chars)
    case 0x1CCFA: return 0x1F40D;   // snake
    case 0x1CCFB: return 0x1F6F8;   // flying saucer
    //case 0x1CCFC:  nose: missing
    // Legacy ex span 2 (5 chars)
    case 0x1CEBA: return 0x1F377;   // fragile
    case 0x1CEBB: return 0x1F3E2;   // office building
    case 0x1CEBC: return 0x1F333;   // tree
    case 0x1CEBD: return 0x1F34E;   // apple
    case 0x1CEBE: return 0x1F352;   // cherry
    case 0x1CEBF: return 0x1F353;   // strawberry
    // Legacy (1 char)
    //case 0x1FBFA: missing somehow
    default: return x;
    }
}

constinit const uc::Font uc::fontInfo[] = {
    { FAM_DEFAULT, Ffg::FALL_TO_NEXT | Ffg::BUG_AVOID },                        // Normal
      { FAM_FUNKY, Ffg::FALL_TO_NEXT | Ffg::BUG_AVOID },                      // …1
      { FNAME_NOTO, Ffg::FALL_TO_NEXT },                                        // …2
      { FNAME_NOTOMATH, Ffg::FALL_TO_NEXT },                                    // …3
      { FNAME_NOTOSYM1, Ffg::GRAPHIC_SAMPLE | Ffg::FALL_TO_NEXT },              // …4
      { FNAME_NOTOSYM2, Ffg::GRAPHIC_SAMPLE | Ffg::FALL_TO_NEXT, 30_bot },      // …5
      { "Segoe UI Symbol", Ffg::FALL_TO_NEXT },                                 // …6  Let it be, system font
      { FAM_HISTORIC },                                                         // …7  Let it be, system font
    { FNAME_NOTO, Ffg::FALL_TO_NEXT },                                          // Noto
      { FAM_FUNKY },                                                          // …1
    { FNAME_NOTOSYM2, Ffg::GRAPHIC_SAMPLE | Ffg::FALL_TO_NEXT, 30_bot },        // Noto symbol2
      { "BabelStonePseudographica.ttf", Ffg::FALL_TO_NEXT },                    // …1
      { FAM_FUNKY },                                                          // …2
    { FNAME_NOTOSYM2, Ffg::DESC_BIGGER | Ffg::FALL_TO_NEXT | Ffg::GRAPHIC_SAMPLE, 30_bot }, // Noto symbol2 bigger
      // Intentionally unhinted
      { FAM_FUNKY, 115_pc },                                                  // …1
    { "Segoe UI Symbol" },                                                      // Symbol
    { FAM_HISTORIC },                                                           // Historic
    { FAM_DEFAULT, Ffg::FALL_TO_NEXT | Ffg::BUG_FIXUP },                        // Punctuation
      { FACE_DEFAULT ",Arial", Ffg::FALL_TO_NEXT  },                             // …1, both are built-in
      { FAM_FUNKY, Ffg::FALL_TO_NEXT },                                       // …2 Fallback for special punctuation
      { FNAME_NOTO },                                                           // …3
    { FAM_DEFAULT, Ffg::FALL_TO_NEXT | Ffg::BUG_AVOID },                        // Math
      { FAM_FUNKY, Ffg::FALL_TO_NEXT },                                       // …1
      { FNAME_NOTOMATH, Ffg::DESC_BIGGER },                                     // …2
    { FNAME_MUSIC_FIXUP, Ffg::FALL_TO_NEXT, 110_pc },                           // Music
      { FNAME_NOTOMUSIC, 110_pc },                                              // …1
    { FNAME_NOTOMUSIC, Ffg::GRAPHIC_SAMPLE, 8_top, 155_pc },                    // Music bigger
    { FNAME_NOTOMUSIC, Ffg::GRAPHIC_SAMPLE, 20_top },                           // Music normal
    { "Gadugi" },                                                               // Gadugi
    { FNAME_DEJAVU, Ffg::FALL_TO_NEXT },                                        // DejaVu
    { FAM_FUNKY, Ffg::FALL_TO_NEXT },                                         // Funky
      { FNAME_NOTO },                                                           // …1
    { FAM_FUNKY, Ffg::DESC_BIGGER },                                          // Funky description bigger
    { FAM_FUNKY, Ffg::DESC_BIGGER, 115_pc },                                  // Funky font bigger
    { FAM_SANS_FRANKEN },                                                       // Sans franken
    { FAM_SANS_FRANKEN, Ffg::DESC_BIGGER },                                     // Sans franken bigger (Prti)
    { FAM_SANS_FRANKEN, 110_pc },                                               // Sans franken 110% (Ogam)
    { FAM_SANS_FRANKEN, Ffg::STUB_FINEGRAINED },                                // Sans franken finegrained (Dupl)
        //-----
    { "NotoSansAdlam-Regular.ttf" },                                            // Adlam
    { "NotoSerifAhom-Regular.ttf", Ffg::FALL_TO_NEXT },                         // Ahom
    { "NotoSansAnatolianHieroglyphs-Regular.otf" },                             // Anatolian
    { "ScheherazadeNew-Regular.ttf", Ffg::FALL_TO_NEXT },                       // Arabic
      { "NotoNaskhArabic-Regular.ttf", Ffg::FALL_TO_NEXT },                     // …1, fallback font for Presentation-A
      { FAM_FUNKY },                                                            // …2, new additions
        // Two fonts OK, as they both are built-in
    { FAM_DEFAULT, Ffg::FALL_TO_NEXT | Ffg::BUG_AVOID },                        // Armenian
      { "NotoSerifArmenian-Regular.ttf" },                                      // …1
    { "NotoSerifBalinese-Regular.ttf", Ffg::STUB_OFF | Ffg::FALL_TO_NEXT },     // Balinese
      { FAM_FUNKY },                                                            // …1
    { "NotoSansBamum-Regular.ttf", 110_pc },                                    // Bamum
    { "NotoSansBatak-Regular.ttf", Ffg::DESC_BADLY_HINTED },                    // Batak
    { FNAME_DEVAFIX, Ffg::FALL_TO_NEXT, STYLE_DEVA, SIZE_DEVA },                // Vedic = Deva′ → Deva → Nand → Beng
      { FNAME_DEVA, Ffg::FALL_TO_NEXT, STYLE_DEVA, SIZE_DEVA },                 // …1
      { FAM_NAND, Ffg::FALL_TO_NEXT },                                          // …2, fall to Bengali    
    { { "NotoSerifBengali-Regular.ttf", Fafg::DEHINT_DOTC, recodeBengali }, 120_pc }, // Bengali
    { "Kedebideri-Regular.ttf" },                                               // Beria
    { "NotoSansBhaiksuki-Regular.ttf", Ffg::DESC_BIGGER, 130_pc },              // Bhaiksuki
    { FAM_HISTORIC, Ffg::FALL_TO_NEXT | Ffg::DESC_BIGGER },                     // Brahmi
      { "NotoSansBrahmi-Regular.ttf", Ffg::DESC_BIGGER },                       // …1
    { "Leelawadee UI", Ffg::FALL_TO_NEXT, 110_pc },                             // Buginese
      { "NotoSansBuginese-Regular.ttf" },                                       // …1
    { "NotoSansBuhid-Regular.ttf" },                                            // Buhid
    { "NotoSansCanadianAboriginal-Regular.ttf" },                               // Canadian aboriginal
    { "NotoSansChakma-Regular.ttf", Ffg::DESC_BIGGER, 12_bot },                 // Chakma
    { "NotoSansCham-Regular.ttf", Ffg::DESC_BADLY_HINTED, 12_bot },             // Cham
        // CJK chars are square, and there’s always not enough detail → bigger
    { "SimSun", Ffg::NOHINT_TINY | Ffg::BUG_AVOID | Ffg::FALL_TO_NEXT, 120_pc }, // CJK fullwidth/halfwidth, fall to next
      { FNAME_BABEL_01, Ffg::NOHINT_TINY, 120_pc },                             // …1
    { FNAME_BABEL_01, Ffg::NOHINT_TINY | Ffg::PLANE_0 | Ffg::PLANE_1 // …CJK
                    | Ffg::FALL_TO_NEXT | Ffg::BUG_AVOID | Ffg::DESC_EXTENDED, 120_pc },
      { FAM_BABEL_23, Ffg::NOHINT_TINY | Ffg::PLANE_2 | Ffg::PLANE_3 | Ffg::FALL_TO_NEXT | Ffg::BUG_AVOID, 120_pc }, // 1
        // SimSun needed for one hiero 34F0: BS drew a hypothetical (not attested by Unicode) Chinese version
      { "SimSun", Ffg::NOHINT_TINY | Ffg::PLANE_0 | Ffg::FALL_TO_NEXT, 120_pc }, // …2
      { "SimSun-ExtB", Ffg::NOHINT_TINY | Ffg::PLANE_2 | Ffg::FALL_TO_NEXT, 120_pc }, // …3
      { FNAME_UNIHAN, Ffg::FALL_TO_NEXT, 120_pc },                              // …4
      { FNAME_HANA_C, Ffg::FALL_TO_NEXT, 120_pc },                              // …5
      { FNAME_UNIHAN, 120_pc },                                                 // …6 for tofu
    { FNAME_BABEL_01, Ffg::NOHINT_TINY | Ffg::FALL_TO_NEXT | Ffg::BUG_AVOID, 120_pc }, // …CJK planes 0/1, backed with Unicodia
      { FNAME_UNIHAN, 120_pc },                                                 // …1
    { FAM_YU, Ffg::FALL_TO_NEXT, 115_pc },                                      // CJK compat
      { "MS Gothic", Ffg::FALL_TO_NEXT, 120_pc },                               // …1
      { FNAME_KOREAN, Ffg::FALL_TO_NEXT, 110_pc },                              // …2
      { "MS Mincho", 120_pc },                                                  // …3
    { "Microsoft YaHei" },                                                      // CJK kanbun
    { "SimSun,Microsoft YaHei", Ffg::FALL_TO_NEXT, 120_pc },                    // CJK vertical
      { FAM_FUNKY, Ffg::FALL_TO_NEXT, 120_pc },                                 // …1
      { FNAME_BABEL_01, Ffg::NOHINT_TINY, 120_pc },                             // …2
    { FAM_FUNKY, Ffg::FALL_TO_NEXT, 120_pc },                                   // CJK symbols
      { "SimSun", Ffg::FALL_TO_NEXT | Ffg::BUG_AVOID, 120_pc },                 // …1
      { FNAME_BABEL_01, Ffg::FALL_TO_NEXT, 120_pc },                            // …2
      { FNAME_KOREAN, 110_pc},                                                  // …3
    { "SimSun", Ffg::NOHINT_TINY | Ffg::FALL_TO_NEXT, 120_pc },                 // CJK fullwidth/halfwidth
      { FNAME_KOREAN, 110_pc},                                                  // …1
    { FNAME_KOREAN, Ffg::FALL_TO_NEXT | Ffg::GRAPHIC_SAMPLE, 125_pc},           // CJK enclosed
      { FNAME_BABEL_01, 120_pc },                                                  // …1
    { { "NotoSansCuneiform-Regular.ttf", recodeCuneiform } },                   // Cuneiform
    { FNAME_DEVA, Ffg::DESC_AVOID | Ffg::FALL_TO_NEXT, STYLE_DEVA, SIZE_DEVA }, // Devanagari
      { "Arial Unicode MS,Nirmala UI,Mangal" },                                 // …1 for descriptions
    { FNAME_DEVAFIX, Ffg::FALL_TO_NEXT, 10_top, SIZE_DEVA },                    // Devanagari bigger
      { FNAME_DEVA, 10_top, SIZE_DEVA },                                        // …1
    { FAM_FUNKY, Ffg::FALL_TO_NEXT },                                           // Dingbat
      { FNAME_NOTOSYM1, Ffg::GRAPHIC_SAMPLE | Ffg::FALL_TO_NEXT | Ffg::BUG_AVOID}, // …1
      { FNAME_NOTOSYM2, Ffg::GRAPHIC_SAMPLE | Ffg::FALL_TO_NEXT | Ffg::BUG_AVOID, 20_bot }, // …2
      { FNAME_BABEL_01, 130_pc },                                               // …3
    { FAM_DEFAULT, Ffg::FALL_TO_NEXT },                                         // Dingbat 2
      { FNAME_NOTOSYM1, Ffg::GRAPHIC_SAMPLE | Ffg::FALL_TO_NEXT, 10_bot },      // …1
      { FNAME_NOTOSYM2, Ffg::GRAPHIC_SAMPLE | Ffg::FALL_TO_NEXT, 20_bot },      // …2
      { "Segoe UI Symbol" },                                                    // …3
    { "NotoSerifDivesAkuru-Regular.ttf", Ffg::DESC_BIGGER, 12_bot, 110_pc },    // Dives Akuru
      // Unlike Egyptian, OTF is poor;
      // FontForge’s auto-hinting is nice until you write a string: top line does not join
    { "NotoSerifDogra-Regular.ttf", Ffg::DESC_BIGGER },                         // Dogra
    { FNAME_NOTOSYM2, Ffg::GRAPHIC_SAMPLE, 40_bot, 150_pc },                    // Dominoes
    { "UnicodiaEgypFixupRegular.otf",                                           // Egyptian
            Ffg::CELL_BIGGER | Ffg::DESC_AVOID | Ffg::FALL_TO_NEXT, 105_pc },
      { "NotoSansEgyptianHieroglyphs-Regular.otf",                              // …1
            Ffg::CELL_BIGGER | Ffg::FALL_TO_NEXT, 105_pc },
      { "UnicodiaSesh.ttf", Ffg::CELL_BIGGER, 115_pc },                         // …2
    { "NotoSansElbasan-Regular.ttf" },                                          // Elbasan
    { FNAME_NOTOSYM1, Ffg::FALL_TO_NEXT | Ffg::GRAPHIC_SAMPLE, 15_top, 115_pc },// Enclosed alnum
      { FNAME_NOTOSYM2, Ffg::GRAPHIC_SAMPLE, 30_bot, 130_pc },                  // …1
    { "NotoSerifEthiopic-Regular.ttf", Ffg::DESC_BIGGER },                      // Ethiopic
    { "NotoSerifEthiopic-Regular.ttf" },                                        // Ethiopic smaller
    { "Sylfaen", Ffg::FALL_TO_NEXT },                                           // Georgian Nuskhuri
      { FNAME_DEJAVU, Ffg::FALL_TO_NEXT },                                      // …1 — FALL to Georgian
    { "NotoSerifGeorgian-Regular.ttf" },                                        // Georgian
    { "Shafarik-Regular.ttf", Ffg::GRAPHIC_SAMPLE | Ffg::FALL_TO_NEXT, 115_pc,
            StyleChange { .delta = 0xF0200 - 0x2C00 } },                        // Glagolitic
      /// @todo [U16, gag] For Cyr C; replace with Noto Serif when it appears
      { FAM_FUNKY },                                                          // …1  for Cyr C Khanty
    { "NotoSansGrantha-Regular.ttf", Ffg::CELL_SMALLER, 10_top + 12_bot },      // Grantha
    { FNAME_NOTO, Ffg::FALL_TO_NEXT | Ffg::DESC_AVOID },                        // Greek/Coptic
      { FAM_FUNKY },                                                          // …1
    { "NotoSerifGujarati-Regular.ttf", Ffg::DESC_BIGGER, 110_pc },              // Gujarati
    { "NotoSansGunjalaGondi-Regular.ttf", Ffg::DESC_BIGGER },                   // Gunjala Gondi
    { { "NotoSerifGurmukhi-Regular.ttf", Fafg::DEHINT_DOTC } },                 // Gurmukhi
    { "NotoSansHanunoo-Regular.ttf", Ffg::DESC_BADLY_HINTED },                  // Hanunoo
    { { "NotoSerifHebrew-Regular.ttf", Fafg::DEHINT_DOTC } },                   // Hebrew
    { FAM_FUNKY, Ffg::FALL_TO_NEXT, 120_pc },                                 // Hentaigana
      { FNAME_BABEL_01, Ffg::NOHINT_TINY, 120_pc },                                // …1
    { "NotoSansJavanese-Regular.ttf" },                                         // Javanese
    { "NotoSansKaithi-Regular.ttf" },                                           // Kaithi
    { "UtoSerifKannada-Regular.ttf", Ffg::DESC_BIGGER | Ffg::FALL_TO_NEXT, 110_pc }, // Kannada
      { FAM_FUNKY },                                                          // …1
    { "NotoSansKawi-Regular.ttf", Ffg::FALL_TO_NEXT },                          // Kawi
      /// @todo [U16, gag] Someday Kawi will handle new char
      { FAM_FUNKY },                                                          // …1, temp patch of U16
    { "NotoSansKayahLi-Regular.ttf" },                                          // Kayah Li
    { "NotoSansKharoshthi-Regular.ttf", Ffg::DESC_BIGGER },                     // Kharoshthi
    { "NotoFangsongKSSVertical-Regular.ttf", Ffg::GRAPHIC_SAMPLE | Ffg::FALL_TO_NEXT, 110_pc }, // Khitan small
      /// @todo [U16, gag] Someday Khitan will handle new char
      { FAM_FUNKY },                                                          // …1, temp patch of U16
    { "NotoSerifKhmer-Regular.ttf", Ffg::DESC_BIGGER },                         // Khmer
    { "NotoSerifKhmer-Regular.ttf", Ffg::DESC_AVOID | Ffg::FALL_TO_NEXT, 90_pc }, // Khmer symbols
      { FAM_FUNKY, Ffg::DESC_BIGGER },
    { "NotoSerifKhojki-Regular.ttf", Ffg::DESC_BIGGER },                        // Khojki
    { "NotoSansKhudawadi-Regular.ttf", Ffg::DESC_BADLY_HINTED },                // Khudawadi
    { "Kanchenjunga-Regular.ttf" },                                             // Kirat
    { FNAME_KOREAN, Ffg::FALL_TO_NEXT | Ffg::MARK_AVOID, 110_pc },              // Korean
      { FAM_FUNKY, Ffg::FALL_TO_NEXT, 120_pc },                               // …1 for sone patches
      { FNAME_UNIHAN, Ffg::FALL_TO_NEXT },                                      // …2 — for P1 Katakana
      { FNAME_BABEL_01, Ffg::FALL_TO_NEXT, 120_pc },                            // …3 U14
        // Hariphunchai4: too small, need smth for virtual virama, incomplete
        // SIL Payap Lanna: bad char for virtual virama, one char missing
        // Lamphun: some mod of older Hariphunchai4, cropped (fixed everything, now UnicodiaLanna)
    { "UnicodiaLanna.otf", Ffg::DESC_BIGGER },                                  // Lanna
    { "UnicodiaSengBuhan-Regular.ttf", 115_pc },                                // Lao
    { FAM_FUNKY, Ffg::FALL_TO_NEXT },                                         // Latin
      { FAM_DEFAULT, Ffg::FALL_TO_NEXT | Ffg::MARK_AVOID },                     // …1
      { FNAME_NOTO },                                                           // …2
         // The biggest font possible w/o changes in height
    { "Mingzat-Regular.ttf", Ffg::GRAPHIC_SAMPLE, 18_top, 80_pc },              // Lepcha
    { "NotoSansLimbu-Regular.ttf", Ffg::DESC_BIGGER },                          // Limbu
    { "NotoSansMahajani-Regular.ttf" },                                         // Mahajani
    { "NotoSansMalayalamUI-Light.ttf", Ffg::LIGHT | Ffg::DESC_BIGGER, 110_pc }, // Malayalam
    { "NotoSansMandaic-Regular.ttf" },                                          // Mandaic
    { "NotoSansManichaean-Regular.ttf" },                                       // Manichaean
    { "NotoSansMarchen-Regular.ttf", Ffg::DESC_BIGGER | Ffg::CELL_BIGGER,       // Marchen
                        20_bot, 110_pc },
        // Simple resave by FontForge kills virama functionality → because of bad hinting BIGGER is everything remaining
        /// @todo [ttf guts, future] Cannot dehint dotted circle for some reason
    { "NotoSansMasaramGondi-Regular.ttf", Ffg::DESC_BADLY_HINTED },             // Masaram Gondi
    { "NotoSansMeeteiMayek-Regular.ttf", Ffg::DESC_BIGGER },                    // Meetei Mayek
    { "NotoSansMeroitic-Regular.ttf" },                                         // Meroitic
    { "NotoSansModi-Regular.ttf", Ffg::DESC_BIGGER },                           // Modi
    { "UnicodiaMongolFixup.ttf", Ffg::FALL_TO_NEXT | Ffg::DESC_AVOID },         // Mongol
      { "NotoSansMongolian-Regular.ttf" },                                      // …1
    { "PadaukBook-Regular.ttf", Ffg::DESC_BIGGER, 110_pc },                     // Myanmar
    { FAM_NAND },                                                               // Nandinagari
    { "NotoSansNewa-Regular.ttf", Ffg::DESC_BADLY_HINTED },                     // Newa
    { "NotoSansNKo-Regular.ttf" },                                              // N’Ko
    { "NotoSerifNPHmong-Regular.ttf" },                                         // NP Hmong
    { "UtoNumberFixup.ttf" },                                                   // Number forms
    { "NotoTraditionalNushu-Light.ttf", Ffg::LIGHT },                           // Nushu
    { FNAME_NOTOSYM2, Ffg::GRAPHIC_SAMPLE, 20_bot, 150_pc },                    // OCR
    { "NotoSansOlChiki-Regular.ttf" },                                          // Ol Chiki
    { "OldHungarian.ttf" },                                                     // Old Hungarian runes
    { "NotoSansOldSogdian-Regular.ttf" },                                       // Old Sogdian
    { "NotoSerifOldUyghur-Regular.ttf" },                                       // Old Uyghur
    { "UtoSerifOriya-Regular.ttf", Ffg::FALL_TO_NEXT, 120_pc },                 // Oriya
      { { FNAME_FUNKY, recodeOriya } },                                         // …1
    { "NotoSansPahawhHmong-Regular.ttf" },                                      // Pahawh Hmong
    { "NotoSansOldPermic-Regular.ttf" },                                        // Permic old
    { "Microsoft PhagsPa", Ffg::DESC_BADLY_HINTED },                            // Phags-Pa
    { FAM_SEMOJI, Ffg::FALL_TO_NEXT },                                          // Mahjong tiles
    /// @todo [misrender] Want negative padding, but how??
      { FNAME_NOTOSYM2, 150_pc },                                               // …1 = Phaistos disc
    { "NotoSansMiao-Regular.ttf", Ffg::DESC_BIGGER },                           // Pollard
    { { "BabelStonePseudographica.ttf", recodePseudographica }, Ffg::FALL_TO_NEXT }, // Pseudographics
      { FNAME_FUNKY },                                                          // …1
    { "NotoSansRejang-Regular.ttf", Ffg::DESC_BIGGER },                         // Rejang
    { "NotoSansHanifiRohingya-Regular.ttf", Ffg::DESC_BIGGER },                 // Rohingya (Hanifi)
    { "NotoSansSamaritan-Regular.ttf" },                                        // Samaritan
    { "NotoSansSaurashtra-Regular.ttf" },                                       // Saurashtra
    { "NotoSansSharada-Regular.ttf", Ffg::DESC_BADLY_HINTED },                  // Sharada
    { "NotoSansSiddham-Regular.ttf", Ffg::DESC_BIGGER },                        // Siddham
    { "UnicodiaSutton-Regular.ttf", Ffg::DESC_BIGGER, 145_pc },                 // SignWriting
    { "NotoSansSinhala-Regular.ttf", Ffg::DESC_BADLY_HINTED },                  // Sinhala
    { "NotoSansIndicSiyaqNumbers-Regular.ttf" },                                // Siyaq Indic
    { "NotoSansSogdian-Regular.ttf" },                                          // Sogdian
    { { "NotoSansSoyombo-Regular.ttf", Fafg::DEHINT_DOTC } , Ffg::DESC_BIGGER, 115_pc }, // Soyombo
    { "UtoSansSundanese-Regular.ttf" },                                         // Sundanese
    { "Mukdum-Final-Unicode.ttf", 140_pc },                                     // Sunuwar
    { "UtoSansSylotiNagri-Regular.ttf" },                                       // Syloti Nagri
    { "SertoAntochBible_2020_Release.ttf", 120_pc },                            // Syriac
    { "UtoSansTagalog-Regular.ttf", Ffg::DESC_BIGGER },                         // Tagalog
    { "NotoSansTagbanwa-Regular.ttf" },                                         // Tagbanwa
    { "NotoSansTakri-Regular.ttf", Ffg::DESC_BIGGER },                          // Takri
    { "Microsoft Tai Le" },                                                     // Tai Le
    { "Microsoft New Tai Lue" },                                                // Tai Lue (new)
    { "NotoSansTaiViet-Regular.ttf" },                                          // Tai Viet
    { "NotoSerifTaiYo-Regular.ttf" },                                           // Tai Yo
    { "Nirmala UI,Latha" },                                                     // Tamil
    { "NotoSansTamilSupplement-Regular.ttf" },                                  // Tamil supplement
    { "TangsaLakhumUnicode.ttf" },                                              // Tangsa
    { "NotoSerifTangut-Regular.ttf", 125_pc },                                  // Tangut
    { FNAME_DEJAVU, Ffg::FALL_TO_NEXT | Ffg::BUG_AVOID },                       // Technical
      { FAM_FUNKY, Ffg::FALL_TO_NEXT | Ffg::BUG_FIXUP },                      // …1
      { FAM_DEFAULT, Ffg::FALL_TO_NEXT | Ffg::BUG_AVOID },                      // …2
      { FNAME_NOTOSYM1, Ffg::GRAPHIC_SAMPLE | Ffg::FALL_TO_NEXT },              // …3
      { FNAME_NOTOSYM2, Ffg::GRAPHIC_SAMPLE | Ffg::FALL_TO_NEXT, 40_bot },      // …4
      { FNAME_NOTOMATH },                                                       // …5
    { "NotoSansTeluguUI-Light.ttf", Ffg::LIGHT | Ffg::DESC_BIGGER | Ffg::FALL_TO_NEXT, 110_pc }, // Telugu
      { FAM_FUNKY },                                                          // …1
    { "MV Boli", 110_pc },                                                      // Thaana
    { "Leelawadee,Leelawadee UI", 10_bot, 110_pc },                             // Thai
    { "BabelStoneTibetanSlim.ttf", 15_bot, 120_pc },                            // Tibetan
    { "NotoSansTirhuta-Regular.ttf" },                                          // Tirhuta
    { "UnicodiaDaarage.otf", 120_pc },                                          // Tulu
    { FNAME_NOTOSYM2, Ffg::FALL_TO_NEXT },                                      // Umlauts for symbols
      { FAM_FUNKY, Ffg::FALL_TO_NEXT },                                       // …1
      { FNAME_NOTO, Ffg::FALL_TO_NEXT },                                        // …2
      { FAM_FUNKY },                                                          // …3 — use it as default
    { "Ebrima" },                                                               // Vai
    { "NotoSerifYezidi-Regular.ttf", 110_pc },                                  // Yezidi
        // Normal is too thin, but bold is too coarse → worse
    { "Microsoft Yi Baiti", 120_pc },                                           // Yi
    { { "NotoSansZanabazarSquare-Regular.ttf", Fafg::DEHINT_DOTC } },           // Zanabazar square
    { "MezenetsUnicode.otf", Ffg::STUB_FINEGRAINED, 120_pc },                   // Znamenny
};

static_assert (std::size(uc::fontInfo) == static_cast<size_t>(uc::EcFont::NN), "fontInfo size?");
//
//  Warning: font array will jump over 256 soon.
//  So if you cache fonts somehow, you’ll need to do nice programming:
//    e.g. offset in font chain
//
