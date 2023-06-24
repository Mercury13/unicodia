#include "UcData.h"

#include "Skin.h"

// File names
constexpr std::string_view FNAME_NOTO = "NotoSerif-Regular.ttf";
constexpr uc::Family FNAME_NOTOMATH { "NotoSansMath-Regular.ttf", uc::Fafg::RAW_FONT };
constexpr std::string_view FNAME_NOTOSYM1 = "NotoSansSymbols-Regular.ttf";
constexpr uc::Family FNAME_NOTOSYM2 { "NotoSansSymbols2-Regular.ttf", uc::Fafg::RAW_FONT };
constexpr std::string_view FNAME_NOTOMUSIC = "NotoMusic-Regular.ttf";
constexpr uc::Family FNAME_MUSIC_FIXUP { "UnicodiaMusicFixup.ttf", uc::Fafg::RAW_FONT };
constexpr std::string_view FNAME_DEJAVU = "DejaVuSerif.ttf";
constexpr uc::Family FNAME_FUNKY { "UnicodiaFunky.ttf", uc::Fafg::RAW_FONT };
constexpr uc::Family FNAME_HANA_C { "HanaMinLiteCSC.ttf", uc::Fafg::RAW_FONT };
constexpr uc::Family FNAME_BABEL { "BabelStoneHan.ttf", uc::Fafg::RAW_FONT };
constexpr uc::Family FNAME_KOREAN = { "NotoSansKR-Regular.otf", uc::Fafg::RAW_FONT };

constexpr std::string_view FNAME_DEVA = "NotoSerifDevanagari-Regular.ttf";
constexpr auto STYLE_DEVA = ""_sty;
constexpr auto SIZE_DEVA = 110_pc;

constexpr std::string_view FNAME_NAND = "NotoSansNandinagari-Regular.ttf";

constinit const uc::Font uc::fontInfo[] = {
    { FAM_DEFAULT, Ffg::FALL_TO_NEXT | Ffg::BUG_AVOID },                        // Normal
      { FNAME_FUNKY, Ffg::FALL_TO_NEXT | Ffg::BUG_AVOID },                      // …1
      { FNAME_NOTO, Ffg::FALL_TO_NEXT },                                        // …2
      { FNAME_NOTOMATH, Ffg::FALL_TO_NEXT },                                    // …3
      { FNAME_NOTOSYM1, Ffg::FALL_TO_NEXT },                                    // …4
      { FNAME_NOTOSYM2, Ffg::FALL_TO_NEXT },                                    // …5
      { "Segoe UI Symbol", Ffg::FALL_TO_NEXT },                                 // …6  Let it be, system font
      { "Segoe UI Historic" },                                                  // …7  Let it be, system font
    { FNAME_NOTO, Ffg::FALL_TO_NEXT },                                          // Noto
     { FNAME_FUNKY },                                                           // …1
    { FNAME_NOTOSYM2 },                                                         // Noto symbol2
    { FNAME_NOTOSYM2, Ffg::DESC_BIGGER },                                       // Noto symbol2 bigger
    { "Segoe UI Symbol" },                                                      // Symbol
    { "Segoe UI Historic" },                                                    // Historic
    { FAM_DEFAULT, Ffg::FALL_TO_NEXT | Ffg::BUG_FIXUP },                        // Punctuation
      { FAM_EMOJI "," FAM_DEFAULT ",Arial", Ffg::FALL_TO_NEXT  },               // …1, both are built-in
      { FNAME_FUNKY, Ffg::FALL_TO_NEXT },                                       // …2 Fallback for special punctuation
      { FNAME_NOTO },                                                           // …3
    { FAM_DEFAULT, Ffg::FALL_TO_NEXT | Ffg::BUG_AVOID },                        // Math
      { FNAME_NOTOMATH, Ffg::DESC_BIGGER },                                     // …1
    { FNAME_MUSIC_FIXUP, Ffg::FALL_TO_NEXT, 110_pc },                           // Music
      { FNAME_NOTOMUSIC, 110_pc },                                              // …1
    { FNAME_NOTOMUSIC, 150_pc },                                                // Music bigger
    { FNAME_NOTOMUSIC },                                                        // Music normal
    { "Gadugi" },                                                               // Gadugi
    { FNAME_DEJAVU, Ffg::FALL_TO_NEXT },                                        // DejaVu
    { FNAME_FUNKY, Ffg::FALL_TO_NEXT },                                         // Funky
      { FNAME_NOTO },                                                           // …1
    { FNAME_FUNKY, Ffg::STUB_RTL },                                             // Funky RTL (unused but left)
        //-----
    { "NotoSansAdlam-Regular.ttf" },                                            // Adlam
    { "NotoSerifAhom-Regular.ttf", Ffg::FALL_TO_NEXT },                         // Ahom
    { "NotoSansAnatolianHieroglyphs-Regular.otf" },                             // Anatolian
    { "ScheherazadeNew-Regular.ttf", Ffg::STUB_ALM },                           // Arabic
    { "ScheherazadeNew-Regular.ttf", Ffg::STUB_ALM | Ffg::FALL_TO_NEXT },       // Arabic Noto — Scheh has a few nice chars!
      { "NotoNaskhArabic-Regular.ttf", Ffg::FALL_TO_NEXT },                     // …1, main font
      { FNAME_FUNKY },                                                          // …2, fallback font for Presentation-A
    { "NotoSansImperialAramaic-Regular.ttf" },                                  // Aramaic
        // Two fonts OK, as they both are built-in
    { FAM_DEFAULT, Ffg::FALL_TO_NEXT },                                         // Armenian
      { "NotoSerifArmenian-Regular.ttf" },                                      // …1
    { "NotoSansAvestan-Regular.ttf" },                                          // Avestan
    { "NotoSerifBalinese-Regular.ttf", Ffg::STUB_OFF | Ffg::FALL_TO_NEXT },     // Balinese
      { FNAME_FUNKY },                                                          // …1
    { "NotoSansBamum-Regular.ttf", 110_pc },                                    // Bamum
    { "NotoSansBassaVah-Regular.ttf" },                                         // Bassa Vah
    { "NotoSansBatak-Regular.ttf", Ffg::DESC_BADLY_HINTED },                    // Batak
    { FNAME_DEVA, Ffg::FALL_TO_NEXT, STYLE_DEVA, SIZE_DEVA },                   // Vedic = Deva → Nand → Beng
      { FNAME_NAND, Ffg::FALL_TO_NEXT },                                        // …1
    { "NotoSerifBengali-Regular.ttf", 120_pc },                                 // Bengali
    { "NotoSansBhaiksuki-Regular.ttf", Ffg::DESC_BIGGER, 130_pc },              // Bhaiksuki
    { "Segoe UI Historic", Ffg::FALL_TO_NEXT | Ffg::DESC_BIGGER | Ffg::FALL2_IF_ACCEPTED, // Brahmi
                    ProbeChar{0x11013} },
      { "NotoSansBrahmi-Regular.ttf", Ffg::FALL_TO_NEXT | Ffg::DESC_BIGGER },   // …1
      { FNAME_FUNKY },                                                          // …2
    { "Leelawadee UI", Ffg::FALL_TO_NEXT, 110_pc },                             // Buginese
      { "NotoSansBuginese-Regular.ttf" },                                       // …1
    { "NotoSansBuhid-Regular.ttf" },                                            // Buhid
    { "NotoSansCanadianAboriginal-Regular.ttf" },                               // Canadian aboriginal
    { "NotoSansCaucasianAlbanian-Regular.ttf" },                                // Caucasian Albanian
    { "NotoSansChakma-Regular.ttf", Ffg::DESC_BIGGER, "padding-bottom:12%;"_sty }, // Chakma
    { "NotoSansCham-Regular.ttf", Ffg::DESC_BADLY_HINTED, "padding-bottom:12%"_sty }, // Cham
    { "NotoSansCherokee-Regular.ttf" },                                         // Cherokee
    { "NotoSansChorasmian-Regular.ttf" },                                       // Chorasmian
        // CJK chars are square, and there’s always not enough detail → bigger
    { "SimSun", Ffg::NOHINT_TINY | Ffg::FALL_TO_NEXT | Ffg::BUG_AVOID, 120_pc },// CJK
      { "SimSun-ExtB", Ffg::NOHINT_TINY | Ffg::FALL_TO_NEXT | Ffg::BUG_AVOID, 120_pc }, // …1
      { FNAME_BABEL, Ffg::NOHINT_TINY | Ffg::FALL_TO_NEXT, 120_pc },            // …2
      { "Microsoft YaHei", Ffg::FALL_TO_NEXT, 120_pc },                         // …3
      { FNAME_HANA_C, Ffg::FALL_TO_NEXT, 120_pc },                              // …4
      { "MS Gothic", Ffg::FALL_TO_NEXT, 120_pc },                               // …5 Japanese
      { "Malgun Gothic", Ffg::FALL_TO_NEXT, 120_pc },                           // …6 Korean
      { "Yu Gothic", Ffg::FALL_TO_NEXT, 120_pc },                               // …7 Japanese
      { FNAME_KOREAN, 120_pc},                                                  // …8 for one W7 tofu FFA0
    { "Yu Gothic", Ffg::FALL_TO_NEXT, 120_pc },                                 // CJK compat
      { "MS Gothic", Ffg::FALL_TO_NEXT, 120_pc },                               // …1
      { FNAME_KOREAN, 120_pc },                                                 // …2
    { "Microsoft YaHei" },                                                      // CJK kanbun
    { "SimSun,Microsoft YaHei", Ffg::FALL_TO_NEXT, 120_pc },                    // CJK structure
      { FNAME_BABEL, Ffg::NOHINT_TINY | Ffg::FALL_TO_NEXT, 120_pc },            // …1
      { FNAME_FUNKY, 120_pc },                                                  // …2
    { FNAME_BABEL, Ffg::NOHINT_TINY | Ffg::FALL_TO_NEXT, 120_pc },              // CJK new Han
      { "UnicodiaHan.ttf", 120_pc },                                            // …1
    { "NotoSansCoptic-Regular.ttf" },                                           // Coptic
    { "NotoSansCuneiform-Regular.ttf" },                                        // Cuneiform
    { FNAME_DEVA, Ffg::DESC_STD, STYLE_DEVA, SIZE_DEVA },                       // Devanagari
    { FNAME_NOTOSYM1, Ffg::FALL_TO_NEXT },                                      // Dingbat
      { FNAME_NOTOSYM2, Ffg::FALL_TO_NEXT },                                    // …1
      { "Segoe UI Symbol", Ffg::FALL_TO_NEXT },                                 // …2
      { FNAME_FUNKY, Ffg::FALL_TO_NEXT },                                       // …3
      { FNAME_BABEL, 130_pc },                                                  // …4
    { FAM_DEFAULT, Ffg::FALL_TO_NEXT },                                         // Dingbat 2
      { FNAME_NOTOSYM1, Ffg::FALL_TO_NEXT },                                    // …1
      { FNAME_NOTOSYM2, Ffg::FALL_TO_NEXT },                                    // …2
      { "Segoe UI Symbol", Ffg::FALL_TO_NEXT },                                 // …3
      { FNAME_FUNKY },                                                          // …4
    { "NotoSerifDivesAkuru-Regular.ttf", Ffg::DESC_BIGGER,
            "padding-bottom:12%;"_sty, 110_pc },                                // Dives Akuru
      // Unlike Egyptian, OTF is poor;
      // FontForge’s auto-hinting is nice until you write a string: top line does not join
    { "NotoSerifDogra-Regular.ttf", Ffg::DESC_BIGGER },                         // Dogra
    { "NotoSansDuployan-Regular.ttf", Ffg::STUB_FINEGRAINED },                  // Duployan
    { FNAME_FUNKY, Ffg::CELL_BIGGER | Ffg::FALL_TO_NEXT },                      // Egyptian
      { "NotoSansEgyptianHieroglyphs-Regular.otf",
                Ffg::CELL_BIGGER | Ffg::BUG_AVOID | Ffg::FALL_TO_NEXT },        // …1
      { "NewGardinerSMP.ttf", Ffg::CELL_BIGGER },                               // …2
    { "NotoSansElbasan-Regular.ttf" },                                          // Elbasan
    { "NotoSansElymaic-Regular.ttf" },                                          // Elymaic
    { "NotoSerifEthiopic-Regular.ttf", Ffg::DESC_BIGGER },                      // Ethiopic
    { "Sylfaen", Ffg::FALL_TO_NEXT },                                           // Georgian Nuskhuri
      { FNAME_DEJAVU, Ffg::FALL_TO_NEXT },                                      // …1 — FALL to Georgian
    { "NotoSerifGeorgian-Regular.ttf" },                                        // Georgian
    { "Shafarik-Regular.ttf", 115_pc,
            StyleChange { .delta = 0xF0200 - 0x2C00 } },                        // Glagolitic
    { "NotoSansGrantha-Regular.ttf", Ffg::CELL_SMALLER,
            "padding-top:10%; padding-bottom:12%;"_sty },                       // Grantha
    { "NotoSerifGujarati-Regular.ttf", Ffg::DESC_BIGGER, 110_pc },              // Gujarati
    { "NotoSansGunjalaGondi-Regular.ttf", Ffg::DESC_BIGGER },                   // Gunjala Gondi
    { "NotoSerifGurmukhi-Regular.ttf" },                                        // Gurmukhi
    { "NotoSansHanunoo-Regular.ttf" },                                          // Hanunoo
    { "NotoSansHatran-Regular.ttf" },                                           // Hatran
    { "NotoSerifHebrew-Regular.ttf" },                                          // Hebrew
    { FNAME_FUNKY, Ffg::FALL_TO_NEXT, 120_pc },                                 // Hentaigana
      { FNAME_BABEL, Ffg::NOHINT_TINY, 120_pc },                                // …1
    { "NotoSansJavanese-Regular.ttf" },                                         // Javanese
    { "NotoSansKaithi-Regular.ttf", Ffg::FALL_TO_NEXT },                        // Kaithi
      { FNAME_FUNKY },                                                          // …1
    { "NotoSerifKannada-Regular.ttf", Ffg::LIGHT | Ffg::DESC_BIGGER, 110_pc },  // Kannada
    { "NotoSansKawi-Regular.ttf" },                                             // Kawi
    { "NotoSansKayahLi-Regular.ttf" },                                          // Kayah Li
    { "NotoSansKharoshthi-Regular.ttf", Ffg::DESC_BIGGER },                     // Kharoshthi
    { "NotoFangsongKSSVertical-Regular.ttf", 115_pc },                          // Khitan small
    { "NotoSansKhmer-Regular.ttf", Ffg::DESC_BIGGER, 110_pc },                  // Khmer
    { "UtoSerifKhojki-Regular.ttf", Ffg::DESC_BIGGER },                         // Khojki
    { "NotoSansKhudawadi-Regular.ttf" },                                        // Khudawadi
    { FNAME_KOREAN, Ffg::FALL_TO_NEXT | Ffg::MARK_AVOID, 110_pc },              // Korean
      { FNAME_FUNKY, 120_pc },                                                  // …1 Old Katakana/Hiragana
      { "Yu Gothic", Ffg::FALL_TO_NEXT },                                       // …2 — for P1 Katakana
      { FNAME_BABEL, Ffg::FALL_TO_NEXT, 120_pc },                               // …3 U14
    { "NotoSansTaiTham-Regular.ttf", Ffg::DESC_BIGGER },                        // Lanna
    { "UnicodiaSengBuhan-Regular.ttf", 115_pc },                                // Lao
    { FNAME_FUNKY, Ffg::FALL_TO_NEXT },                                         // Latin
      { FAM_DEFAULT, Ffg::FALL_TO_NEXT },                                       // …1
      { FNAME_NOTO },                                                           // …2
    { "NotoSansLepcha-Regular.ttf" },                                           // Lepcha
    { "NotoSansLimbu-Regular.ttf", Ffg::DESC_BIGGER },                          // Limbu
    { "NotoSansLinearA-Regular.ttf" },                                          // Linear A
    { "NotoSansLinearB-Regular.ttf" },                                          // Linear B
    { "NotoSansLisu-Regular.ttf" },                                             // Lisu
    { "NotoSansMahajani-Regular.ttf" },                                         // Mahajani
    { "MakasarGraphite.ttf", Ffg::STUB_OFF },                                   // Makasar
    { "NotoSansMalayalamUI-Light.ttf", Ffg::LIGHT | Ffg::DESC_BIGGER, 110_pc }, // Malayalam
    { "NotoSansMandaic-Regular.ttf" },                                          // Mandaic
    { "NotoSansManichaean-Regular.ttf" },                                       // Manichaean
    { "NotoSansMarchen-Regular.ttf", Ffg::DESC_BIGGER, "padding-bottom:20%;"_sty, 110_pc }, // Marchen
        // Simple resave by FontForge kills virama functionality → because of bad hinting BIGGER is everything remaining
    { "NotoSansMasaramGondi-Regular.ttf", Ffg::DESC_BADLY_HINTED },             // Masaram Gondi
    { "NotoSansMedefaidrin-Regular.ttf" },                                      // Medefaidrin
    { "NotoSansMeeteiMayek-Regular.ttf", Ffg::DESC_BIGGER },                    // Meetei Mayek
    { "NotoSansMendeKikakui-Regular.ttf" },                                     // Mende Kikakui
    { "NotoSansMeroitic-Regular.ttf" },                                         // Meroitic
    { "NotoSansModi-Regular.ttf", Ffg::DESC_BIGGER },                           // Modi
    { "UtoSansMongolian-Regular.ttf" },                                         // Mongol
    { "NotoSansMro-Regular.ttf" },                                              // Mro
    { "NotoSansMultani-Regular.ttf" },                                          // Multani
    { "PadaukBook-Regular.ttf", Ffg::DESC_BIGGER, 110_pc },                     // Myanmar
    { "NotoSansNabataean-Regular.ttf" },                                        // Nabataean
    { FNAME_NAND },                                                             // Nandinagari
    { "NotoSansNewa-Regular.ttf", Ffg::DESC_BADLY_HINTED },                     // Newa
    { "NotoSansNKo-Regular.ttf" },                                              // N’Ko
    { "NotoSansOldNorthArabian-Regular.ttf" },                                  // North Arabian
    { "NotoSerifNPHmong-Regular.ttf" },                                         // NP Hmong
    { "NotoTraditionalNushu-Light.ttf", Ffg::LIGHT },                           // Nushu
    { "NotoSansOgham-Regular.ttf", 110_pc },                                    // Ogham
    { "NotoSansOlChiki-Regular.ttf" },                                          // Ol Chiki
    { "OldHungarian.ttf" },                                                     // Old Hungarian runes
    { "NotoSansOldItalic-Regular.ttf" },                                        // Old Italic
    { "NotoSansOldPermic-Regular.ttf" },                                        // Old Permic
    { "NotoSansOldPersian-Regular.ttf" },                                       // Old Persian cuneiform
    { "NotoSansOldSogdian-Regular.ttf" },                                       // Old Sogdian
    { "NotoSerifOldUyghur-Regular.ttf", Ffg::STUB_RTL },                        // Old Uyghur
    { "NotoSansOriyaUI-Regular.ttf", 120_pc },                                  // Oriya
    { FNAME_FUNKY, Ffg::FALL_TO_NEXT },                                         // Orkhon
      { "NotoSansOldTurkic-Regular.ttf" },                                      // …1
    { "NotoSansPahawhHmong-Regular.ttf" },                                      // Pahawh Hmong
    { "NotoSansPalmyrene-Regular.ttf" },                                        // Palmyrene
    { "NotoSansInscriptionalParthian-Regular.ttf", Ffg::DESC_BIGGER },          // Parthian
    { "NotoSansPauCinHau-Regular.ttf" },                                        // Pau Cin Hau
    { "Microsoft PhagsPa", Ffg::DESC_BADLY_HINTED },                            // Phags-Pa
    { "NotoSansPhoenician-Regular.ttf" },                                       // Phoenician
    { "Segoe UI Emoji", Ffg::FALL_TO_NEXT },                                    // Mahjong tiles
    /// @todo [misrender] Want negative padding, but how??
      { FNAME_NOTOSYM2, 150_pc },                                               // …1 = Phaistos disc
    { "NotoSansMiao-Regular.ttf", Ffg::DESC_BIGGER },                           // Pollard
    { "NotoSansPsalterPahlavi-Regular.ttf" },                                   // Psalter Pahlavi
    { "NotoSansRejang-Regular.ttf", Ffg::DESC_BIGGER },                         // Rejang
    { "NotoSansHanifiRohingya-Regular.ttf", Ffg::DESC_BIGGER },                 // Rohingya (Hanifi)
    { "NotoSansRunic-Regular.ttf" },                                            // Runic
    { "NotoSansSamaritan-Regular.ttf" },                                        // Samaritan
    { "NotoSansSaurashtra-Regular.ttf" },                                       // Saurashtra
    { "NotoSansSharada-Regular.ttf", Ffg::DESC_BADLY_HINTED },                  // Sharada
    { "NotoSansSiddham-Regular.ttf", Ffg::DESC_BIGGER },                        // Siddham
    { "UnicodiaSutton-Regular.ttf", Ffg::DESC_BIGGER, 145_pc },                 // SignWriting
    { "NotoSansSinhala-Regular.ttf", Ffg::DESC_BADLY_HINTED },                  // Sinhala
    { "NotoSansIndicSiyaqNumbers-Regular.ttf" },                                // Siyaq Indic
    { "NotoSerifOttomanSiyaq-Regular.ttf" },                                    // Siyaq Ottoman
    { "NotoSansSogdian-Regular.ttf" },                                          // Sogdian
    { "NotoSansSoyombo-Regular.ttf", 115_pc },                                  // Soyombo
    { "UtoSansSundanese-Regular.ttf" },                                         // Sundanese
    { "NotoSansSylotiNagri-Regular.ttf" },                                      // Syloti Nagri
    { "SertoAntochBible_2020_Release.ttf", 120_pc },                            // Syriac
    { "NotoSansTagalog-Regular.ttf", Ffg::DESC_BIGGER },                        // Tagalog
    { "NotoSansTagbanwa-Regular.ttf" },                                         // Tagbanwa
    { "NotoSansTakri-Regular.ttf", Ffg::DESC_BIGGER },                          // Takri
    { "Microsoft Tai Le" },                                                     // Tai Le
    { "Microsoft New Tai Lue" },                                                // Tai Lue (new)
    { "NotoSansTaiViet-Regular.ttf" },                                          // Tai Viet
    { "Nirmala UI,Latha" },                                                     // Tamil
            // Somehow font does not respond to char support
    { Family{ "NotoSansTamilSupplement-Regular.ttf", Fafg::RAW_FONT } },        // Tamil supplement
    { "TangsaLakhumUnicode.ttf" },                                              // Tangsa
    { "NotoSerifTangut-Regular.ttf", 125_pc },                                  // Tangut
    { FNAME_DEJAVU, Ffg::FALL_TO_NEXT | Ffg::BUG_AVOID },                       // Technical
      { FNAME_FUNKY, Ffg::FALL_TO_NEXT | Ffg::BUG_FIXUP },                      // …1
      { FAM_DEFAULT, Ffg::FALL_TO_NEXT | Ffg::BUG_AVOID },                      // …2
      { FNAME_NOTOSYM1, Ffg::FALL_TO_NEXT },                                    // …3
      { FNAME_NOTOSYM2, Ffg::FALL_TO_NEXT },                                    // …4
      { FNAME_NOTOMATH },                                                       // …5
    { "NotoSansTeluguUI-Light.ttf", Ffg::LIGHT | Ffg::DESC_BIGGER | Ffg::FALL_TO_NEXT, 110_pc }, // Telugu
      { FNAME_FUNKY },                                                          // …1
    { "MV Boli", 110_pc },                                                      // Thaana
    { "Leelawadee,Leelawadee UI", "padding-bottom:10%;"_sty, 110_pc },          // Thai
    { "BabelStoneTibetanSlim.ttf", "padding-bottom:15%;"_sty, 120_pc },         // Tibetan
    { "NotoSansTifinagh-Regular.ttf" },                                         // Tifinagh
    { "NotoSansTirhuta-Regular.ttf" },                                          // Tirhuta
    { FNAME_NOTOSYM2, Ffg::FALL_TO_NEXT },                                      // Umlauts for symbols
      { FNAME_FUNKY, Ffg::FALL_TO_NEXT },                                       // …1
      { FNAME_NOTO, Ffg::FALL_TO_NEXT },                                        // …2
      { FNAME_FUNKY },                                                          // …3 — use it as default
    { "Ebrima" },                                                               // Vai
    { "NotoSerifVithkuqi-Regular.ttf" },                                        // Vithkuqi
    { "NotoSansWancho-Regular.ttf", Ffg::DESC_BIGGER },                         // Wancho
    { "NotoSansWarangCiti-Regular.ttf", Ffg::DESC_BIGGER },                     // Warang Citi
    { "NotoSerifYezidi-Regular.ttf", 110_pc },                                  // Yezidi
        // Normal is too thin, but bold is too coarse → worse
    { "Microsoft Yi Baiti", 120_pc },                                           // Yi
    { "NotoSansZanabazarSquare-Regular.ttf" },                                  // Zanabazar square
    { "MezenetsUnicode.otf", Ffg::STUB_FINEGRAINED, 120_pc },                   // Znamenny
};

static_assert (std::size(uc::fontInfo) == static_cast<size_t>(uc::EcFont::NN), "fontInfo size?");
