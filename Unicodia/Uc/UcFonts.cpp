#include "UcData.h"

#include "Skin.h"

consteval uc::StyleSheet operator "" _top (unsigned long long x)
    { return uc::StyleSheet{ .topPc = static_cast<short>(x), .botPc = 0 } ; }

consteval uc::StyleSheet operator "" _bot (unsigned long long x)
    { return uc::StyleSheet{ .topPc = 0, .botPc = static_cast<short>(x) } ; }

consteval uc::StyleSheet operator + (
        const uc::StyleSheet& x, const uc::StyleSheet& y)
    { return { .topPc = short(x.topPc + y.topPc), .botPc = short(x.botPc + y.botPc) }; }


// File names
constexpr uc::Family FNAME_NOTO { "NotoSerif-Regular.ttf", uc::Fafg::DEHINT_DOTC };
constexpr uc::Family FNAME_NOTOMATH { "NotoSansMath-Regular.ttf" };
constexpr std::string_view FNAME_NOTOSYM1 = "NotoSansSymbols-Regular.ttf";
constexpr uc::Family FNAME_NOTOSYM2 { "NotoSansSymbols2-Regular.ttf" };
constexpr uc::Family FNAME_NOTOMUSIC { "NotoMusic-Regular.ttf" };
constexpr uc::Family FNAME_MUSIC_FIXUP { "UnicodiaMusicFixup.ttf" };
constexpr std::string_view FNAME_DEJAVU = "DejaVuSerif.ttf";
constexpr uc::Family FNAME_FUNKY { "UnicodiaFunky.ttf" };
constexpr uc::Family FNAME_HANA_C { "HanaMinLiteCSC.ttf" };
constexpr uc::Family FNAME_BABEL { "BabelStoneHan.ttf" };
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

constinit const uc::Font uc::fontInfo[] = {
    { FAM_DEFAULT, Ffg::FALL_TO_NEXT | Ffg::BUG_AVOID },                        // Normal
      { FNAME_FUNKY, Ffg::FALL_TO_NEXT | Ffg::BUG_AVOID },                      // …1
      { FNAME_NOTO, Ffg::FALL_TO_NEXT },                                        // …2
      { FNAME_NOTOMATH, Ffg::FALL_TO_NEXT },                                    // …3
      { FNAME_NOTOSYM1, Ffg::GRAPHIC_SAMPLE | Ffg::FALL_TO_NEXT },              // …4
      { FNAME_NOTOSYM2, Ffg::GRAPHIC_SAMPLE | Ffg::FALL_TO_NEXT, 30_bot },      // …5
      { "Segoe UI Symbol", Ffg::FALL_TO_NEXT },                                 // …6  Let it be, system font
      { FAM_HISTORIC },                                                         // …7  Let it be, system font
    { FNAME_NOTO, Ffg::FALL_TO_NEXT },                                          // Noto
      { FNAME_FUNKY },                                                          // …1
    { FNAME_NOTOSYM2, Ffg::GRAPHIC_SAMPLE | Ffg::FALL_TO_NEXT, 30_bot },        // Noto symbol2
      { "BabelStonePseudographica.ttf", Ffg::FALL_TO_NEXT },                    // …1
      { FNAME_FUNKY },                                                          // …2
    { FNAME_NOTOSYM2, Ffg::DESC_BIGGER | Ffg::FALL_TO_NEXT | Ffg::GRAPHIC_SAMPLE, 30_bot }, // Noto symbol2 bigger
      // Intentionally unhinted
      { FNAME_FUNKY, 115_pc },                                                  // …1
    { "Segoe UI Symbol" },                                                      // Symbol
    { FAM_HISTORIC },                                                           // Historic
    { FAM_DEFAULT, Ffg::FALL_TO_NEXT | Ffg::BUG_FIXUP },                        // Punctuation
      { FAM_DEFAULT ",Arial", Ffg::FALL_TO_NEXT  },                             // …1, both are built-in
      { FNAME_FUNKY, Ffg::FALL_TO_NEXT },                                       // …2 Fallback for special punctuation
      { FNAME_NOTO },                                                           // …3
    { FAM_DEFAULT, Ffg::FALL_TO_NEXT | Ffg::BUG_AVOID },                        // Math
      { FNAME_FUNKY, Ffg::FALL_TO_NEXT },                                       // …1
      { FNAME_NOTOMATH, Ffg::DESC_BIGGER },                                     // …2
    { FNAME_MUSIC_FIXUP, Ffg::FALL_TO_NEXT, 110_pc },                           // Music
      { FNAME_NOTOMUSIC, 110_pc },                                              // …1
    { FNAME_NOTOMUSIC, Ffg::GRAPHIC_SAMPLE, 8_top, 155_pc },                    // Music bigger
    { FNAME_NOTOMUSIC, Ffg::GRAPHIC_SAMPLE, 20_top },                           // Music normal
    { "Gadugi" },                                                               // Gadugi
    { FNAME_DEJAVU, Ffg::FALL_TO_NEXT },                                        // DejaVu
    { FNAME_FUNKY, Ffg::FALL_TO_NEXT },                                         // Funky
      { FNAME_NOTO },                                                           // …1
    { FNAME_FUNKY, Ffg::DESC_BIGGER },                                          // Funky bigger
    { FAM_SANS_FRANKEN },                                                       // Sans franken
    { FAM_SANS_FRANKEN, Ffg::DESC_BIGGER },                                     // Sans franken bigger (Prti)
    { FAM_SANS_FRANKEN, 110_pc },                                               // Sans franken 110% (Ogam)
        //-----
    { "NotoSansAdlam-Regular.ttf" },                                            // Adlam
    { "NotoSerifAhom-Regular.ttf", Ffg::FALL_TO_NEXT },                         // Ahom
    { "NotoSansAnatolianHieroglyphs-Regular.otf" },                             // Anatolian
    { "ScheherazadeNew-Regular.ttf", Ffg::FALL_TO_NEXT },                       // Arabic
      { "NotoNaskhArabic-Regular.ttf", Ffg::FALL_TO_NEXT },                     // …1, fallback font for Presentation-A
      { FNAME_FUNKY },                                                          // …2, new additions
        // Two fonts OK, as they both are built-in
    { FAM_DEFAULT, Ffg::FALL_TO_NEXT },                                         // Armenian
      { "NotoSerifArmenian-Regular.ttf" },                                      // …1
    { "NotoSansAvestan-Regular.ttf" },                                          // Avestan
    { "NotoSerifBalinese-Regular.ttf", Ffg::STUB_OFF | Ffg::FALL_TO_NEXT },     // Balinese
      { FNAME_FUNKY },                                                          // …1
    { "NotoSansBamum-Regular.ttf", 110_pc },                                    // Bamum
    { "NotoSansBassaVah-Regular.ttf" },                                         // Bassa Vah
    { "NotoSansBatak-Regular.ttf", Ffg::DESC_BADLY_HINTED },                    // Batak
    { FNAME_DEVAFIX, Ffg::FALL_TO_NEXT, STYLE_DEVA, SIZE_DEVA },                // Vedic = Deva′ → Deva → Nand → Beng
      { FNAME_DEVA, Ffg::FALL_TO_NEXT, STYLE_DEVA, SIZE_DEVA },                 // …1
      { FAM_NAND, Ffg::FALL_TO_NEXT },                                          // …2, fall to Bengali
    { { "NotoSerifBengali-Regular.ttf", Fafg::DEHINT_DOTC }, 120_pc },          // Bengali
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
    { FNAME_BABEL, Ffg::NOHINT_TINY | Ffg::FALL_TO_NEXT | Ffg::BUG_AVOID, 120_pc }, // …CJK
      { "SimSun-ExtB", Ffg::NOHINT_TINY | Ffg::FALL_TO_NEXT, 120_pc },          // …1
      { "SimSun", Ffg::NOHINT_TINY | Ffg::FALL_TO_NEXT, 120_pc },               // …2
      { "Microsoft YaHei", Ffg::FALL_TO_NEXT, 120_pc },                         // …3
      { FNAME_HANA_C, Ffg::FALL_TO_NEXT, 120_pc },                              // …4
    { FAM_YU, Ffg::FALL_TO_NEXT, 115_pc },                                      // CJK compat
      { "MS Gothic", Ffg::FALL_TO_NEXT, 120_pc },                               // …1
      { FNAME_KOREAN, Ffg::FALL_TO_NEXT, 110_pc },                              // …2
      { "MS Mincho", 120_pc },                                                  // …3
    { "Microsoft YaHei" },                                                      // CJK kanbun
    { "SimSun,Microsoft YaHei", Ffg::FALL_TO_NEXT, 120_pc },                    // CJK structure
      { FNAME_FUNKY, Ffg::FALL_TO_NEXT, 100_pc },                               // …1
      { FNAME_BABEL, Ffg::NOHINT_TINY, 120_pc },                                // …2
    { "SimSun", Ffg::NOHINT_TINY | Ffg::BUG_AVOID | Ffg::FALL_TO_NEXT, 120_pc }, // CJK SimSun unhinted, fall to next
    { FNAME_BABEL, Ffg::NOHINT_TINY | Ffg::FALL_TO_NEXT, 120_pc },              // CJK new Han
      { FNAME_UNIHAN, 120_pc },                                                 // …1
    { FNAME_FUNKY, Ffg::FALL_TO_NEXT, 120_pc },                                 // CJK symbols
      { "SimSun", Ffg::FALL_TO_NEXT | Ffg::BUG_AVOID, 120_pc },                 // …1
      { FNAME_BABEL, Ffg::FALL_TO_NEXT, 120_pc },                               // …2
      { FNAME_KOREAN, 110_pc},                                                  // …3
    { "SimSun", Ffg::NOHINT_TINY | Ffg::FALL_TO_NEXT, 120_pc },                 // CJK fullwidth/halfwidth
      { FNAME_KOREAN, 110_pc},                                                  // …1
    { FNAME_KOREAN, Ffg::FALL_TO_NEXT | Ffg::GRAPHIC_SAMPLE, 125_pc},           // CJK enclosed
      { FNAME_BABEL, 120_pc },                                                  // …1
    { { "NotoSansCuneiform-Regular.ttf", recodeCuneiform } },                   // Cuneiform
    { FNAME_DEVA, Ffg::DESC_AVOID | Ffg::FALL_TO_NEXT, STYLE_DEVA, SIZE_DEVA }, // Devanagari
      { "Arial Unicode MS,Nirmala UI,Mangal" },                                 // …1 for descriptions
    { FNAME_DEVAFIX, Ffg::FALL_TO_NEXT, 10_top, SIZE_DEVA },                    // Devanagari bigger
      { FNAME_DEVA, 10_top, SIZE_DEVA },                                        // …1
    { FNAME_NOTOSYM1, Ffg::GRAPHIC_SAMPLE | Ffg::FALL_TO_NEXT | Ffg::BUG_AVOID}, // Dingbat
      { FNAME_NOTOSYM2, Ffg::GRAPHIC_SAMPLE | Ffg::FALL_TO_NEXT | Ffg::BUG_AVOID, 20_bot }, // …1
      { FNAME_FUNKY, Ffg::FALL_TO_NEXT },                                       // …2
      { FNAME_BABEL, 130_pc },                                                  // …3
    { FAM_DEFAULT, Ffg::FALL_TO_NEXT },                                         // Dingbat 2
      { FNAME_NOTOSYM1, Ffg::GRAPHIC_SAMPLE | Ffg::FALL_TO_NEXT, 10_bot },      // …1
      { FNAME_NOTOSYM2, Ffg::GRAPHIC_SAMPLE | Ffg::FALL_TO_NEXT, 20_bot },      // …2
      { "Segoe UI Symbol" },                                                    // …3
    { "NotoSerifDivesAkuru-Regular.ttf", Ffg::DESC_BIGGER, 12_bot, 110_pc },    // Dives Akuru
      // Unlike Egyptian, OTF is poor;
      // FontForge’s auto-hinting is nice until you write a string: top line does not join
    { "NotoSerifDogra-Regular.ttf", Ffg::DESC_BIGGER },                         // Dogra
    { FNAME_NOTOSYM2, Ffg::GRAPHIC_SAMPLE, 40_bot, 150_pc },                    // Dominoes
    { "NotoSansDuployan-Regular.ttf", Ffg::STUB_FINEGRAINED },                  // Duployan
    { "UnicodiaEgypFixupRegular.otf", Ffg::CELL_BIGGER | Ffg::DESC_AVOID | Ffg::FALL_TO_NEXT }, // Egyptian
      { "NotoSansEgyptianHieroglyphs-Regular.otf", Ffg::CELL_BIGGER },          // …1
    { "NotoSansElbasan-Regular.ttf" },                                          // Elbasan
    { "NotoSansElymaic-Regular.ttf" },                                          // Elymaic    
    { FNAME_NOTOSYM1, Ffg::FALL_TO_NEXT | Ffg::GRAPHIC_SAMPLE, 15_top, 115_pc },// Enclosed alnum
      { FNAME_NOTOSYM2, Ffg::GRAPHIC_SAMPLE, 30_bot, 130_pc },                  // …1
    { "NotoSerifEthiopic-Regular.ttf", Ffg::DESC_BIGGER },                      // Ethiopic
    { "Sylfaen", Ffg::FALL_TO_NEXT },                                           // Georgian Nuskhuri
      { FNAME_DEJAVU, Ffg::FALL_TO_NEXT },                                      // …1 — FALL to Georgian
    { "NotoSerifGeorgian-Regular.ttf" },                                        // Georgian
    { "Shafarik-Regular.ttf", Ffg::GRAPHIC_SAMPLE | Ffg::FALL_TO_NEXT, 115_pc,
            StyleChange { .delta = 0xF0200 - 0x2C00 } },                        // Glagolitic
      /// @todo [U16, gag] For Cyr C; replace with Noto Serif when it appears
      { FNAME_FUNKY },                                                          // …1  for Cyr C Khanty
    { "NotoSansGrantha-Regular.ttf", Ffg::CELL_SMALLER, 10_top + 12_bot },      // Grantha
    { "NotoSerifGujarati-Regular.ttf", Ffg::DESC_BIGGER, 110_pc },              // Gujarati
    { "NotoSansGunjalaGondi-Regular.ttf", Ffg::DESC_BIGGER },                   // Gunjala Gondi
    { { "NotoSerifGurmukhi-Regular.ttf", Fafg::DEHINT_DOTC } },                 // Gurmukhi
    { "NotoSansHanunoo-Regular.ttf", Ffg::DESC_BADLY_HINTED },                  // Hanunoo
    { "NotoSansHatran-Regular.ttf" },                                           // Hatran
    { { "NotoSerifHebrew-Regular.ttf", Fafg::DEHINT_DOTC } },                   // Hebrew
    { FNAME_FUNKY, Ffg::FALL_TO_NEXT, 120_pc },                                 // Hentaigana
      { FNAME_BABEL, Ffg::NOHINT_TINY, 120_pc },                                // …1
    { "NotoSansJavanese-Regular.ttf" },                                         // Javanese
    { "NotoSansKaithi-Regular.ttf" },                                           // Kaithi
    { "UtoSerifKannada-Regular.ttf", Ffg::DESC_BIGGER | Ffg::FALL_TO_NEXT, 110_pc }, // Kannada
      { FNAME_FUNKY },                                                          // …1
    { "NotoSansKawi-Regular.ttf", Ffg::FALL_TO_NEXT },                          // Kawi
      /// @todo [U16, gag] Someday Kawi will handle new char
      { FNAME_FUNKY },                                                          // …1, temp patch of U16
    { "NotoSansKayahLi-Regular.ttf" },                                          // Kayah Li
    { "NotoSansKharoshthi-Regular.ttf", Ffg::DESC_BIGGER },                     // Kharoshthi
    { "NotoFangsongKSSVertical-Regular.ttf", Ffg::GRAPHIC_SAMPLE | Ffg::FALL_TO_NEXT, 110_pc }, // Khitan small
      /// @todo [U16, gag] Someday Khitan will handle new char
      { FNAME_FUNKY },                                                          // …1, temp patch of U16
    { "NotoSansKhmer-Regular.ttf", Ffg::DESC_BIGGER, 110_pc },                  // Khmer
    { "NotoSerifKhojki-Regular.ttf", Ffg::DESC_BIGGER },                        // Khojki
    { "NotoSansKhudawadi-Regular.ttf", Ffg::DESC_BADLY_HINTED },                // Khudawadi
    { "Kanchenjunga-Regular.ttf" },                                             // Kirat
    { FNAME_KOREAN, Ffg::FALL_TO_NEXT | Ffg::MARK_AVOID, 110_pc },              // Korean
      { FNAME_FUNKY, Ffg::FALL_TO_NEXT, 120_pc },                               // …1 for sone patches
      { FNAME_UNIHAN, Ffg::FALL_TO_NEXT },                                      // …2 — for P1 Katakana
      { FNAME_BABEL, Ffg::FALL_TO_NEXT, 120_pc },                               // …3 U14
        // Hariphunchai4: too small, need smth for virtual virama, incomplete
        // SIL Payap Lanna: bad char for virtual virama, one char missing
        // Lamphun: some mod of older Hariphunchai4, cropped (fixed everything, now UnicodiaLanna)
    { "UnicodiaLanna.otf", Ffg::DESC_BIGGER },                                  // Lanna
    { "UnicodiaSengBuhan-Regular.ttf", 115_pc },                                // Lao
    { FNAME_FUNKY, Ffg::FALL_TO_NEXT },                                         // Latin
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
    { "NotoSansMasaramGondi-Regular.ttf", Ffg::DESC_BADLY_HINTED },             // Masaram Gondi
    { "NotoSansMeeteiMayek-Regular.ttf", Ffg::DESC_BIGGER },                    // Meetei Mayek
    { "NotoSansMendeKikakui-Regular.ttf" },                                     // Mende Kikakui
    { "NotoSansMeroitic-Regular.ttf" },                                         // Meroitic
    { "NotoSansModi-Regular.ttf", Ffg::DESC_BIGGER },                           // Modi
    { "UnicodiaMongolFixup.ttf", Ffg::FALL_TO_NEXT | Ffg::DESC_AVOID },         // Mongol
      { "NotoSansMongolian-Regular.ttf" },                                      // …1
    { "NotoSansMultani-Regular.ttf" },                                          // Multani
    { "PadaukBook-Regular.ttf", Ffg::DESC_BIGGER, 110_pc },                     // Myanmar
    { FAM_NAND },                                                               // Nandinagari
    { "NotoSansNewa-Regular.ttf", Ffg::DESC_BADLY_HINTED },                     // Newa
    { "NotoSerifNPHmong-Regular.ttf" },                                         // NP Hmong
    { "NotoTraditionalNushu-Light.ttf", Ffg::LIGHT },                           // Nushu
    { FNAME_NOTOSYM2, Ffg::GRAPHIC_SAMPLE, 20_bot, 150_pc },                    // OCR
    { "NotoSansOlChiki-Regular.ttf" },                                          // Ol Chiki
    { "OldHungarian.ttf" },                                                     // Old Hungarian runes
    { "NotoSansOldSogdian-Regular.ttf" },                                       // Old Sogdian
    { "NotoSerifOldUyghur-Regular.ttf" },                                       // Old Uyghur
    { "UtoSerifOriya-Regular.ttf", 120_pc },                                    // Oriya
    { "NotoSansPahawhHmong-Regular.ttf" },                                      // Pahawh Hmong
    { "NotoSansPauCinHau-Regular.ttf" },                                        // Pau Cin Hau
    { "Microsoft PhagsPa", Ffg::DESC_BADLY_HINTED },                            // Phags-Pa
    { "NotoSansPhoenician-Regular.ttf" },                                       // Phoenician
    { FAM_SEMOJI, Ffg::FALL_TO_NEXT },                                          // Mahjong tiles
    /// @todo [misrender] Want negative padding, but how??
      { FNAME_NOTOSYM2, 150_pc },                                               // …1 = Phaistos disc
    { "NotoSansMiao-Regular.ttf", Ffg::DESC_BIGGER },                           // Pollard
    { "BabelStonePseudographica.ttf" },                                         // Pseudographics
    { "NotoSansRejang-Regular.ttf", Ffg::DESC_BIGGER },                         // Rejang
    { "NotoSansHanifiRohingya-Regular.ttf", Ffg::DESC_BIGGER },                 // Rohingya (Hanifi)
    { "NotoSansSaurashtra-Regular.ttf" },                                       // Saurashtra
    { "NotoSansSharada-Regular.ttf", Ffg::DESC_BADLY_HINTED },                  // Sharada
    { "NotoSansSiddham-Regular.ttf", Ffg::DESC_BIGGER },                        // Siddham
    { "UnicodiaSutton-Regular.ttf", Ffg::DESC_BIGGER, 145_pc },                 // SignWriting
    { "NotoSansSinhala-Regular.ttf", Ffg::DESC_BADLY_HINTED },                  // Sinhala
    { "NotoSansIndicSiyaqNumbers-Regular.ttf" },                                // Siyaq Indic
    { "NotoSerifOttomanSiyaq-Regular.ttf" },                                    // Siyaq Ottoman
    { "NotoSansSogdian-Regular.ttf" },                                          // Sogdian
    { { "NotoSansSoyombo-Regular.ttf", Fafg::DEHINT_DOTC } , Ffg::DESC_BIGGER, 115_pc },                // Soyombo
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
    { "Nirmala UI,Latha" },                                                     // Tamil
    { "NotoSansTamilSupplement-Regular.ttf" },                                  // Tamil supplement
    { "TangsaLakhumUnicode.ttf" },                                              // Tangsa
    { "NotoSerifTangut-Regular.ttf", 125_pc },                                  // Tangut
    { FNAME_DEJAVU, Ffg::FALL_TO_NEXT | Ffg::BUG_AVOID },                       // Technical
      { FNAME_FUNKY, Ffg::FALL_TO_NEXT | Ffg::BUG_FIXUP },                      // …1
      { FAM_DEFAULT, Ffg::FALL_TO_NEXT | Ffg::BUG_AVOID },                      // …2
      { FNAME_NOTOSYM1, Ffg::GRAPHIC_SAMPLE | Ffg::FALL_TO_NEXT },              // …3
      { FNAME_NOTOSYM2, Ffg::GRAPHIC_SAMPLE | Ffg::FALL_TO_NEXT, 40_bot },      // …4
      { FNAME_NOTOMATH },                                                       // …5
    { "NotoSansTeluguUI-Light.ttf", Ffg::LIGHT | Ffg::DESC_BIGGER | Ffg::FALL_TO_NEXT, 110_pc }, // Telugu
      { FNAME_FUNKY },                                                          // …1
    { "MV Boli", 110_pc },                                                      // Thaana
    { "Leelawadee,Leelawadee UI", 10_bot, 110_pc },                             // Thai
    { "BabelStoneTibetanSlim.ttf", 15_bot, 120_pc },                            // Tibetan
    { "NotoSansTirhuta-Regular.ttf" },                                          // Tirhuta
    { "UnicodiaDaarage.otf", 120_pc },                                          // Tulu
    { FNAME_NOTOSYM2, Ffg::FALL_TO_NEXT },                                      // Umlauts for symbols
      { FNAME_FUNKY, Ffg::FALL_TO_NEXT },                                       // …1
      { FNAME_NOTO, Ffg::FALL_TO_NEXT },                                        // …2
      { FNAME_FUNKY },                                                          // …3 — use it as default
    { "Ebrima" },                                                               // Vai
    { "NotoSerifVithkuqi-Regular.ttf" },                                        // Vithkuqi
    { "NotoSansWarangCiti-Regular.ttf", Ffg::DESC_BIGGER },                     // Warang Citi
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
