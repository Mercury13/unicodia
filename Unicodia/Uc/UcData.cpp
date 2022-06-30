#include "UcData.h"

// Qt
#include <QFontDatabase>
#include <QFontMetrics>
#include <QRawFont>

// Libs
#include "i_TempFont.h"
#include "u_Strings.h"
#include "u_Qstrings.h"
#include "u_Iterator.h"
#include "u_Cmap.h"
#include "mojibake.h"

// Project-local
#include "Skin.h"

// L10n
#include "LocDic.h"
#include "u_Decoders.h"


#define NBSP "\u00A0"

using ReverseMap = Cmap<char16_t, unsigned char, 128>;
template class Cmap<char16_t, unsigned char, 128>;

using namespace std::string_view_literals;
const uc::Cp* uc::cpsByCode[CAPACITY];
short uc::blocksByCode16[CAPACITY >> 4];
const QString uc::Font::qempty;
constinit const uc::InputMethods uc::InputMethods::NONE;

constexpr QChar ZWSP(0x200B);

// [+] any missing char is tofu (BUGGY)  [-] try smth from system
constexpr bool FORCE_TOFU = false;

namespace fst {
    constexpr auto DEFAULT = FORCE_TOFU ? TOFU : COMPAT;
}

// File names
constexpr std::string_view FNAME_NOTO = "NotoSerif-Regular.ttf";
constexpr std::string_view FNAME_NOTOMATH = "NotoSansMath-Regular.ttf";
constexpr std::string_view FNAME_NOTOSYM1 = "NotoSansSymbols-Regular.ttf";
constexpr std::string_view FNAME_NOTOSYM2 = "NotoSansSymbols2-Regular.ttf";
constexpr std::string_view FNAME_NOTOMUSIC = "NotoMusic-Regular.ttf";
constexpr std::string_view FNAME_DEJAVU = "DejaVuSerif.ttf";
constexpr uc::Family FNAME_FUNKY { "FunkySample.ttf", uc::Fafg::RAW_FONT };
constexpr std::string_view FNAME_HANA_C = "HanaMinLiteCSC.ttf";
constexpr std::string_view FNAME_BABEL = "BabelStoneHan.ttf";
constexpr std::string_view FNAME_KOREAN = "NotoSansKR-Regular.otf";

constexpr std::string_view FNAME_DEVA = "NotoSerifDevanagari.ttf";
constexpr auto STYLE_DEVA = "padding-top:10%"_sty;
constexpr auto SIZE_DEVA = 110_pc;

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
    { FAM_DEFAULT, Ffg::FALL_TO_NEXT },                                         // Math Cambria (fall to next)
    { FNAME_NOTOMATH },                                                         // Math
    { FNAME_FUNKY, Ffg::FALL_TO_NEXT, 110_pc },                                 // Music
      { FNAME_NOTOMUSIC, 110_pc },                                              // …1
    { FNAME_NOTOMUSIC, 150_pc },                                                // Music bigger
    { FNAME_NOTOMUSIC },                                                        // Music normal
    { "Gadugi" },                                                               // Gadugi
    { FNAME_DEJAVU },                                                           // DejaVu
    { FNAME_FUNKY },                                                            // Funky
        //-----
    { "NotoSansAdlam-Regular.ttf" },                                            // Adlam
    { "NotoSerifAhom-Regular.ttf", Ffg::FALL_TO_NEXT },                         // Ahom
      { FNAME_FUNKY },                                                          // …1
    { "NotoSansAnatolianHieroglyphs-Regular.otf" },                             // Anatolian
    { "ScheherazadeNew-Regular.ttf", Ffg::STUB_RTL },                           // Arabic
    { "ScheherazadeNew-Regular.ttf", Ffg::STUB_RTL | Ffg::FALL_TO_NEXT },       // Arabic Noto — Scheh has a few nice chars!
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
    { FNAME_DEVA, Ffg::FALL_TO_NEXT, STYLE_DEVA, SIZE_DEVA },                   // Vedic = Devanagari → Bengali
    { "NotoSerifBengali-Regular.ttf", 120_pc },                                 // Bengali
    { "NotoSansBhaiksuki-Regular.ttf", Ffg::DESC_BIGGER, 130_pc },              // Bhaiksuki
    { "NotoSansBrahmi-Regular.ttf", Ffg::FALL_TO_NEXT | Ffg::DESC_BIGGER },     // Brahmi
      { FNAME_FUNKY },                                                          // …1
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
    { "SimSun", Ffg::FALL_TO_NEXT | Ffg::BUG_AVOID, 120_pc },                   // CJK
      { "SimSun-ExtB", Ffg::FALL_TO_NEXT | Ffg::BUG_AVOID, 120_pc },            // …1
      { FNAME_BABEL, Ffg::FALL_TO_NEXT, 120_pc },                               // …2
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
    { "SimSun,Microsoft YaHei" },                                               // CJK structure
    { "NotoSansCoptic-Regular.ttf" },                                           // Coptic
    { "NotoSansCuneiform-Regular.ttf" },                                        // Cuneiform
    { FNAME_DEVA, STYLE_DEVA, SIZE_DEVA },                                      // Devanagari
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
    { "NotoSansGlagolitic-Regular.ttf" },                                       // Glagolitic
    { "NotoSansGrantha-Regular.ttf", Ffg::CELL_SMALLER,
            "padding-top:10%; padding-bottom:12%;"_sty },                       // Grantha
    { "NotoSerifGujarati-Regular.ttf", Ffg::DESC_BIGGER, 110_pc },              // Gujarati
    { "NotoSansGunjalaGondi-Regular.ttf", Ffg::DESC_BIGGER },                   // Gunjala Gondi
    { "NotoSansGurmukhiUI-Regular.ttf", 110_pc },                               // Gurmukhi
    { "NotoSansHanunoo-Regular.ttf" },                                          // Hanunoo
    { "NotoSansHatran-Regular.ttf" },                                           // Hatran
    { "NotoSerifHebrew-Regular.ttf" },                                          // Hebrew
    { "HanaMinA.ttf", Ffg::DESC_STD, 110_pc },                                  // Hentaigana
    { "NotoSansJavanese-Regular.ttf" },                                         // Javanese
    { "NotoSansKaithi-Regular.ttf", Ffg::FALL_TO_NEXT },                        // Kaithi
      { FNAME_FUNKY },                                                          // …1
    { "NotoSerifKannada-Regular.ttf", Ffg::LIGHT | Ffg::DESC_BIGGER, 110_pc },  // Kannada
    { "NotoSansKayahLi-Regular.ttf" },                                          // Kayah Li
    { "NotoSansKharoshthi-Regular.ttf", Ffg::DESC_BIGGER },                     // Kharoshthi
    { "BabelStoneKhitanSmallLinear.ttf", 115_pc },                              // Khitan small
    { "NotoSansKhmerUI-Regular.ttf", Ffg::DESC_BIGGER, 110_pc },                // Khmer — initial Noto loses umlauts
    { "NotoSansKhojki-Regular.ttf", Ffg::DESC_BIGGER },                         // Khojki
    { "NotoSansKhudawadi-Regular.ttf" },                                        // Khudawadi
    { FNAME_KOREAN, Ffg::FALL_TO_NEXT | Ffg::STUB_OFF, 110_pc },                // Korean: managed to auto-rename OTF
      { "Yu Gothic", Ffg::FALL_TO_NEXT, },                                      // …1 — for P1 Katakana
      { FNAME_BABEL, Ffg::FALL_TO_NEXT, 120_pc },                               // …2 U14
      { FNAME_FUNKY },                                                          // …3 Old Katakana/Hiragana
    { "NotoSansTaiTham-Regular.ttf", Ffg::DESC_BIGGER },                        // Lanna
    { "SengBuhan-Regular.ttf", 115_pc },                                        // Lao
    { FAM_DEFAULT, Ffg::FALL_TO_NEXT },                                         // Latin
      { FNAME_NOTO, Ffg::FALL_TO_NEXT },                                        // …1
      { FNAME_FUNKY },                                                          // …2
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
    { "NotoSansMongolian-Regular.ttf" },                                        // Mongol
    { "NotoSansMro-Regular.ttf" },                                              // Mro
    { "NotoSansMultani-Regular.ttf" },                                          // Multani
    { "PadaukBook-Regular.ttf", Ffg::DESC_BIGGER, 110_pc },                     // Myanmar
    { "NotoSansNabataean-Regular.ttf" },                                        // Nabataean
    { "NotoSansNandinagari-Regular.ttf" },                                      // Nandinagari
    { "NotoSansNewa-Regular.ttf", Ffg::DESC_BADLY_HINTED },                     // Newa
    { "NotoSansNKo-Regular.ttf" },                                              // N’Ko
    { "NotoSansOldNorthArabian-Regular.ttf" },                                  // North Arabian
    { "NotoSerifHmongNyiakeng-Regular.ttf" },                                   // NP Hmong
    { "NotoSansNushu-Regular.ttf" },                                            // Nushu
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
    { "NotoSansPauCinHau-Regular.ttf" },                                        // Pau Cin Hau
    { "Microsoft PhagsPa", Ffg::DESC_BADLY_HINTED },                            // Phags-Pa
    { "NotoSansPhoenician-Regular.ttf" },                                       // Phoenician
    { "Segoe UI Emoji", Ffg::FALL_TO_NEXT },                                    // Playing cards
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
    { "NotoSansSignWriting-Regular.ttf", Ffg::STUB_OFF | Ffg::DESC_BIGGER, 145_pc }, // SignWriting
    { "NotoSansSinhala-Light.ttf", Ffg::LIGHT | Ffg::DESC_BADLY_HINTED },       // Sinhala
    { "NotoSansIndicSiyaqNumbers-Regular.ttf" },                                // Siyaq Indic
    { "NotoSansSogdian-Regular.ttf" },                                          // Sogdian
    { "NotoSansSoyombo-Regular.ttf", 115_pc },                                  // Soyombo
    { "NotoSansSundanese-Regular.ttf" },                                        // Sundanese
    { "NotoSansSylotiNagri-Regular.ttf" },                                      // Syloti Nagri
    { "SertoAntochBible_2020_Release.ttf", 120_pc },                            // Syriac
    { "NotoSansTagalog-Regular.ttf", Ffg::DESC_BIGGER },                        // Tagalog
    { "NotoSansTagbanwa-Regular.ttf" },                                         // Tagbanwa
    { "NotoSansTakri-Regular.ttf", Ffg::DESC_BIGGER | Ffg::FALL_TO_NEXT },      // Takri
      { FNAME_FUNKY },                                                          // …1
    { "Microsoft Tai Le" },                                                     // Tai Le
    { "Microsoft New Tai Lue" },                                                // Tai Lue (new)
    { "NotoSansTaiViet-Regular.ttf" },                                          // Tai Viet
    { "Nirmala UI,Latha" },                                                     // Tamil
            // Somehow font does not respond to char support
    { Family{ "NotoSansTamilSupplement-Regular.ttf", Fafg::RAW_FONT } },        // Tamil supplement
    { "TangsaLakhumUnicode.ttf" },                                              // Tangsa
    { "NotoSerifTangut-Regular.ttf", 125_pc },                                  // Tangut
    { FAM_DEFAULT, Ffg::FALL_TO_NEXT | Ffg::BUG_FIXUP },                        // Technical
      { "Segoe UI Emoji", Ffg::FALL_TO_NEXT },                                  // …1
      { FNAME_NOTOSYM1, Ffg::FALL_TO_NEXT },                                    // …2
      { FNAME_NOTOMATH, Ffg::FALL_TO_NEXT },                                    // …3
      { FNAME_NOTOSYM2, Ffg::FALL_TO_NEXT, },                                   // …4
      { FNAME_FUNKY, Ffg::FALL_TO_NEXT },                                       // …5
      { FNAME_DEJAVU },                                                         // …6
    { "NotoSansTeluguUI-Light.ttf", Ffg::LIGHT | Ffg::DESC_BIGGER | Ffg::FALL_TO_NEXT, 110_pc }, // Telugu
      { FNAME_FUNKY },                                                          // …1
    { "MV Boli", 110_pc },                                                      // Thaana
    { "Leelawadee,Leelawadee UI", "padding-bottom:10%;"_sty, 110_pc },          // Thai
    { "BabelStoneTibetanSlim.ttf", "padding-bottom:15%;"_sty, 120_pc },         // Tibetan
    { "NotoSansTifinagh-Regular.ttf" },                                         // Tifinagh
    { "NotoSansTirhuta-Regular.ttf" },                                          // Tirhuta
    { FNAME_NOTOSYM2, Ffg::FALL_TO_NEXT },                                      // Umlauts
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

static_assert (std::size(uc::fontInfo) == static_cast<size_t>(uc::EcFont::NN));

constinit const uc::LangLife uc::langLifeInfo[static_cast<int>(EcLangLife::NN)] {
    { {} },
    { "Prop.Life.Alive" },
    { "Prop.Life.Danger" },
    { "Prop.Life.Competing" },
    { "Prop.Life.Dead" },
    { "Prop.Life.Hist" },
    { "Prop.Life.Sacred" },
    { "Prop.Life.New"},
    { "Prop.Life.Reborn" },
    { "Prop.Life.Exp" },
    //{ "Error" },
};


constinit const uc::WritingDir uc::writingDirInfo[static_cast<int>(EcWritingDir::NN)] {
    { {} },
    { "Prop.Dir.L" },
    { "Prop.Dir.R" },
    { "Prop.Dir.L_cjk" },
    { "Prop.Dir.L_cunei" },
    { "Prop.Dir.L_ogam" },
    { "Prop.Dir.L_col" },
    { "Prop.Dir.TD" },
    { "Prop.Dir.Both" },
    { "Prop.Dir.Tang" },
    { "Prop.Dir.R_col" },
    { "Prop.Dir.R_most" },
    { "Prop.Dir.Sogd" },
    //{ "Error" },
};


constinit const uc::ScriptType uc::scriptTypeInfo[] {
    { {} },
    { "Prop.Type.Alph" },
    { "Prop.Type.Argu" },
    { "Prop.Type.Cons" },
    { "Prop.Type.Syll" },
    { "Prop.Type.Asyl" },
    { "Prop.Type.Abug" },
    { "Prop.Type.Brah" },
    { "Prop.Type.Hier" },
    { "Prop.Type.Syhi" },
    { "Prop.Type.Cohi" },
    { "Prop.Type.Code" },
    { "Prop.Type.Nota" },
    //{ "Error" },
};
static_assert (std::size(uc::scriptTypeInfo) == static_cast<int>(uc::EcScriptType::NN));

// Usage: u8"15.0" VER_BETA
#define VER_BETA "\u00A0" "β"

const uc::Version uc::versionInfo[] {
    { {},     0    },
    //{ "1.0",  1991 },
    //{ "1.0.1",1992 },
    { u8"1.1",  1993 },
    { u8"2.0",  1996 },
    { u8"2.1",  1998 },
    { u8"3.0",  1999 },
    { u8"3.1",  2001 },
    { u8"3.2",  2002 },
    { u8"4.0",  2003 },
    { u8"4.1",  2005 },
    { u8"5.0",  2006 },
    { u8"5.1",  2008 },
    { u8"5.2",  2009 },
    { u8"6.0",  2010 },
    { u8"6.1",  2012 },
    { u8"6.2",  2012 },
    { u8"6.3",  2013 },
    { u8"7.0",  2014 },
    { u8"8.0",  2015 },
    { u8"9.0",  2016 },
    { u8"10.0", 2017 },
    { u8"11.0", 2018 },
    { u8"12.0", 2019 },
    { u8"12.1", 2019 },
    { u8"13.0", 2020 },
    { u8"14.0", 2021 },
    { u8"15.0" VER_BETA, 2022 },
};
static_assert (std::size(uc::versionInfo) == static_cast<int>(uc::EcVersion::NN));

constinit const uc::Category uc::categoryInfo[static_cast<int>(uc::EcCategory::NN)] {
    { UpCategory::CONTROL,      "Cc",   Graphical::NO  },
    { UpCategory::FORMAT,       "Cf",   Graphical::NO  },
    // PRIVATE_USE,     -- unused as Unicodia has nothing to tell
    // SURROGATE,       -- unused as Unicodia has nothing to tell
    { UpCategory::LETTER,       "Ll",   Graphical::YES },
    { UpCategory::LETTER,       "Lm",   Graphical::YES },
    { UpCategory::LETTER,       "Lo",   Graphical::YES },
    { UpCategory::LETTER,       "Lt",   Graphical::YES },
    { UpCategory::LETTER,       "Lu",   Graphical::YES },
    { UpCategory::MARK,         "Mc",   Graphical::YES },
    { UpCategory::MARK,         "Me",   Graphical::YES },
    { UpCategory::MARK,         "Mn",   Graphical::YES },
    { UpCategory::NUMBER,       "Nd",   Graphical::YES },
    { UpCategory::NUMBER,       "Nl",   Graphical::YES },
    { UpCategory::NUMBER,       "No",   Graphical::YES },
    { UpCategory::PUNCTUATION,  "Pc",   Graphical::YES },
    { UpCategory::PUNCTUATION,  "Pd",   Graphical::YES },
    { UpCategory::PUNCTUATION,  "Pe",   Graphical::YES },
    { UpCategory::PUNCTUATION,  "Pf",   Graphical::YES },
    { UpCategory::PUNCTUATION,  "Pi",   Graphical::YES },
    { UpCategory::PUNCTUATION,  "Po",   Graphical::YES },
    { UpCategory::PUNCTUATION,  "Ps",   Graphical::YES },
    { UpCategory::SYMBOL,       "Sc",   Graphical::YES },
    { UpCategory::SYMBOL,       "Sk",   Graphical::YES },
    { UpCategory::SYMBOL,       "Sm",   Graphical::YES },
    { UpCategory::SYMBOL,       "So",   Graphical::YES },
    { UpCategory::SEPARATOR,    "Zl",   Graphical::NO  },
    { UpCategory::SEPARATOR,    "Zp",   Graphical::NO  },
    { UpCategory::SEPARATOR,    "Zs",   Graphical::YES },
    //{ u8"Error"sv },     //check for equal number
};


constinit const uc::Script uc::scriptInfo[] {
    { "Zyyy", QFontDatabase::Any,
        EcScriptType::NONE, EcLangLife::NOMATTER, EcWritingDir::NOMATTER, EcContinent::NONE,
            Dating::none(),
            EcFont::NORMAL, Sfg::NONSCRIPT | Sfg::NO_LANGS },
    // Adlam OK, W10 has, but placement of umlauts + RTL = ??? → better Noto
    { "Adlm", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::RTL, EcContinent::AFRICA,
        Dating::special(), EcFont::ADLAM },
    // Caucasian Albanian OK, W10 none, installed Google Noto
    { "Aghb", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::yapprox(400), EcFont::CAUCASIAN_ALBANIAN },
    // Ahom OK, installed Google Noto font
    { "Ahom", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::REVIVED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(13), EcFont::AHOM },
    // Arabic OK, three fonts: SIL Scheherazade main, Google Noto Math + Google Noto Arabic for special ranges
    { "Arab", QFontDatabase::Arabic,
        EcScriptType::CONSONANT, EcLangLife::ALIVE, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::century(7, StdNote::MODERN_FORM), EcFont::ARABIC },
    // Imperial Aramaic OK, because of sheer importance install Google Noto
    { "Armi", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::crange(-10, -9), EcFont::ARAMAIC },
    // Armenian OK, Cambria+Noto is more than nice!
    { "Armn", QFontDatabase::Armenian,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(405), EcFont::ARMENIAN },
    // Avestan OK, installed Google Noto
    /// @todo [future, link] wait for book Pahlavi (sorry, cannot put todo at lockit)
    { "Avst", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::SACRED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::yapprox(400), EcFont::AVESTAN },
    // Balinese OK, installed Google Noto font
    { "Bali", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::SACRED, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::yapprox(1000), EcFont::BALINESE },
    // Bamum OK, none even in W10, installed Google Noto and fixed stub
    { "Bamu", QFontDatabase::Any,
        EcScriptType::SYLLABLE, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::AFRICA,
        Dating::yrange(1895, 1910), EcFont::BAMUM },
    // Bassa Vah OK, none in W10, installed Google Noto
    { "Bass", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::AFRICA,
        Dating::yapprox(1900), EcFont::BASSA_VAH },
    // Batak OK, installed Google Noto font
    { "Batk", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::yapprox(1300), EcFont::BATAK },
    // Bengali OK, W7 has tofu of 2009+ → installed Google Noto
    { "Beng", QFontDatabase::Bengali,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::special(), EcFont::BENGALI },
    // Bhaiksuki OK, installed Google Noto
    { "Bhks", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(11), EcFont::BHAIKSUKI },
    // Bopomofo OK
    { "Bopo", QFontDatabase::Any,
        EcScriptType::ALPHASYLLABLE, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::CJK,
        Dating::year(1913), EcFont::CJK },
    // Brahmi OK, because of sheer importance installed Google Noto
    { "Brah", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::special(), EcFont::BRAHMI },
    // Braille OK, “Segoe UI Symbol”
    { "Brai", QFontDatabase::Any,
        EcScriptType::CODE, EcLangLife::NOMATTER, EcWritingDir::NOMATTER, EcContinent::NONE,
        Dating::year(1824), EcFont::SYMBOL },
    // Buginese OK, W10 only → made LelawadeeUI → Google Noto chain
    { "Bugi", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::crange(15, 17), EcFont::BUGINESE },
    // Buhid OK, installed Google Noto font
    { "Buhd", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::yapprox(1300), EcFont::BUHID },
    // Chakma OK, added Noto font, mostly because of missing glyphs
    { "Cakm", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::unknown(), EcFont::CHAKMA },
    // Carian OK, W10 P1
    /// @todo [font???] Carian
    { "Cari", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::BOTH, EcContinent::ASIA,
        Dating::century(-7), EcFont::HISTORIC },
    // Canadian syllabics OK, W10 “Gadugi”, W7 lacks 2009 extensions, installed Google Noto font
    { "Cans", QFontDatabase::Any,
        EcScriptType::ABUGIDA, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::AMERICA,
        Dating::year(1833), EcFont::CANADIAN_ABORIGINAL },
    { "Cham", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(4), EcFont::CHAM },
    // Cherokee OK, installed Google Noto font. Need it, W7 has no 2014 extensions.
    { "Cher", QFontDatabase::Any,
        EcScriptType::SYLLABLE, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::AMERICA,
        Dating::year(1821), EcFont::CHEROKEE },
    // Chorasmian OK, installed Google Noto
    { "Chrs", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::SOGDIAN, EcContinent::ASIA,
        Dating::century(2, StdNote::FIRST_KNOWN), EcFont::CHORASMIAN },
    // Coptic OK, W7 problems → installed Google Noto font
    { "Copt", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::SACRED, EcWritingDir::LTR, EcContinent::AFRICA,
        Dating::century(2), EcFont::COPTIC },
    // Cypro-Minoan OK, made nice script at FunkySample
    { "Cpmn", QFontDatabase::Any,
        EcScriptType::SYLLABLE, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::yapprox(-1550, StdNote::FIRST_KNOWN), EcFont::FUNKY },
    // Cypriot OK, W10 Segoe Historic
    { "Cprt", QFontDatabase::Any,
        EcScriptType::SYLLABLE, EcLangLife::HISTORICAL, EcWritingDir::RTL_MOSTLY, EcContinent::EUROPE,
        Dating::century(-11), EcFont::HISTORIC },
    // Cyr OK except enclosing; managed to modify Noto
    { "Cyrl", QFontDatabase::Cyrillic,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::ybefore(900), EcFont::NORMAL },
    // Devanagari OK, added 8 characters to Noto Serif
    { "Deva", QFontDatabase::Devanagari,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::crange(1, 7), EcFont::DEVANAGARI },
    // Diak OK, built some samples
    { "Diak", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::crange(9, 10, StdNote::FIRST_KNOWN), EcFont::DIVES_AKURU },
    // Dogri OK, W10 off → installed Google Noto
    { "Dogr", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::REVIVED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(19), EcFont::DOGRA },
    // Deseret OK, W10 Segoe UI Symbol, plane 1
    { "Dsrt", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::EXPERIMENTAL, EcWritingDir::LTR, EcContinent::AMERICA,
        Dating::year(1854), EcFont::SYMBOL },
    // Duployan OK, W10 none → installed Google Noto
    { "Dupl", QFontDatabase::Any,
        EcScriptType::CODE, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::year(1860), EcFont::DUPLOYAN },
    // Egyptian hiero OK
    { "Egyp", QFontDatabase::Any,
        EcScriptType::CONSONANTHIEROGLYPH, EcLangLife::HISTORICAL, EcWritingDir::BOTH, EcContinent::AFRICA,
        Dating::yapprox(-3200), EcFont::EGYPTIAN },
    // Elbasan OK, none in W10 → installed Google Noto
    { "Elba", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::EXPERIMENTAL, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::century(18), EcFont::ELBASAN },
    // Elymaic OK, W10 none → installed Google Noto
    { "Elym", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::century(-2), EcFont::ELYMAIC },
    // Ethiopic OK, lots of tofu, espec. in W7 → installed Google Noto
    { "Ethi", QFontDatabase::Any,
        EcScriptType::ABUGIDA, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::AFRICA,
        Dating::century(4, StdNote::CUSTOM), EcFont::ETHIOPIC },
    // Georgian OK, installed Google Noto font
    { "Geor", QFontDatabase::Georgian,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::year(430, StdNote::FIRST_KNOWN), EcFont::GEORGIAN },
    // Glagolitic OK, installed Google Noto font
    { "Glag", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::century(9), EcFont::GLAGOLITIC },
    // Gondi/Gunjala OK, W10 none, installed Google Noto
    { "Gong", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::REVIVED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1750, StdNote::FIRST_KNOWN), EcFont::GUNJALA_GONDI },
    // Gondi/Masaram OK, W10 none, installed Google Noto
    { "Gonm", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1918), EcFont::MASARAM_GONDI },
    // Gothic OK, took from Junicode and enlarged ×135%
    { "Goth", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::century(4), EcFont::FUNKY },
    // Grantha OK, W10 none → installed Google Noto
    { "Gran", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::SACRED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::special(), EcFont::GRANTHA },
    // Greek OK, W7 Cambria + Noto do the job
    { "Grek", QFontDatabase::Greek,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::crange_ins(-9, -8), EcFont::NOTO },
    // Gujarati OK, installed Google Noto: cannot find a good pair for W7/10, and Noto is REALLY nice.
    { "Gujr", QFontDatabase::Gujarati,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1592, StdNote::FIRST_KNOWN), EcFont::GUJARATI },
    // Gurmukhi OK, installed Google Noto *UI* because of W7 troubles; UI better handles umlauts
    { "Guru", QFontDatabase::Gurmukhi,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(16), EcFont::GURMUKHI },
    // Hangul OK, installed Noto CJK font
    { "Hang", QFontDatabase::Korean,
        EcScriptType::ARGUABLE, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::CJK,
        Dating::year(1443), EcFont::KOREAN },
    // Hani OK
    { "Hani", WS_HANI,       // Special rules for hieroglyphs, SimChi triggers them
        EcScriptType::HIEROGLYPH, EcLangLife::ALIVE, EcWritingDir::LTR_CJK, EcContinent::CJK,
        Dating::yapprox(-2000), EcFont::CJK },
    // Hanunoo OK, installed Google Noto font
    { "Hano", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::yapprox(1300), EcFont::HANUNOO },
    // Hatran OK, W10 none, installed Google Noto
    { "Hatr", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::yapprox(-100), EcFont::HATRAN },
    // Hebrew OK, installed very nice Noto, implemented yod triangle
    { "Hebr", QFontDatabase::Hebrew,
        EcScriptType::CONSONANT, EcLangLife::ALIVE, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::century(-2, StdNote::MODERN_FORM), EcFont::HEBREW },
    // Hentaigana currently OK
    { "Hent", QFontDatabase::Japanese,
        EcScriptType::SYLLABLE, EcLangLife::HISTORICAL, EcWritingDir::LTR_CJK, EcContinent::CJK,
        Dating::crange(8, 9), EcFont::HENTAIGANA },
    // Hiragana OK, installed small version of Noto CJK Korean
    { "Hira", QFontDatabase::Japanese,
        EcScriptType::SYLLABLE, EcLangLife::ALIVE, EcWritingDir::LTR_CJK, EcContinent::CJK,
        Dating::crange(8, 9), EcFont::KOREAN },
    // Anatolian hiero OK, installed Google Noto
    { "Hluw", QFontDatabase::Any,
        EcScriptType::HIEROGLYPH, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::crange(-14, -13, StdNote::FIRST_KNOWN), EcFont::ANATOLIAN },
    // Pahawh Hmong OK, W10 none → installed Google Noto
    { "Hmng", QFontDatabase::Any,
        EcScriptType::ABUGIDA, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1959, StdNote::CUSTOM), EcFont::PAHAWH_HMONG },
    // NP Hmong OK, W10 none → installed Google Noto
    { "Hmnp", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::AMERICA,
        Dating::decade(1980), EcFont::NP_HMONG },
    // Hungarian runes OK, W10 none → installed some open-source font
    { "Hung", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::EUROPE,
        Dating::century(10, StdNote::MAYBE_EARLIER), EcFont::OLD_HUNGARIAN },
    // Old Italic OK, installed Google Noto (Segoe has no newer chars od 2014+)
    { "Ital", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::BOTH, EcContinent::EUROPE,
        Dating::yapprox(-700), EcFont::OLD_ITALIC },
    // Javanese OK, W10 has “Javanese Text”, W7 does not, installed Google Noto font
    { "Java", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::century(15), EcFont::JAVANESE },
    // Kayah Li OK, W10 none, installed Google Noto font
    { "Kali", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1962), EcFont::KAYAH_LI },
    // Katakana OK, installed small version of Noto CJK Korean
    { "Kana", QFontDatabase::Japanese,
        EcScriptType::SYLLABLE, EcLangLife::ALIVE, EcWritingDir::LTR_CJK, EcContinent::CJK,
        Dating::crange(8, 9), EcFont::KOREAN },
    /// @todo [U15] Kawi
    { "Kawi", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::century(8, StdNote::FIRST_KNOWN), EcFont::NORMAL },
    // Kharoshthi OK, W10 tofu → installed Noto
    { "Khar", QFontDatabase::Any,
        EcScriptType::ABUGIDA, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::century(-4), EcFont::KHAROSHTHI },
    // Khmer OK, fancy and inconsistent in W7/10, installed Google Noto *UI* because of umlauts
    { "Khmr", QFontDatabase::Khmer,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(611, StdNote::FIRST_KNOWN), EcFont::KHMER },
    { "Khoj", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::SACRED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::crange(14, 16), EcFont::KHOJKI },
    // Khitan small OK, installed a font
    { "Kits", QFontDatabase::Any,
        EcScriptType::SYLLABOHIEROGLYPH, EcLangLife::HISTORICAL, EcWritingDir::RTL_COL, EcContinent::CJK,
        Dating::year(925), EcFont::KHITAN_SMALL },
    // Kannada OK, W7 has no recent extensions → installed Google Noto
    { "Knda", QFontDatabase::Kannada,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(14, StdNote::CUSTOM), EcFont::KANNADA },
    // Kaithi OK, W10 none → installed Google Noto
    { "Kthi", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(16), EcFont::KAITHI},
    // Lanna OK, W10 none, installed Google Noto font
    { "Lana", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1376, StdNote::FIRST_KNOWN), EcFont::LANNA },
    // Lao OK, W10 partial, installed SengBuhan font
    { "Laoo", QFontDatabase::Lao,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::yapprox(1350), EcFont::LAO },
    // Latin OK, managed to modify
    { "Latn", QFontDatabase::Latin,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::yapprox(-700), EcFont::NORMAL },
    // Lepcha OK, W10 none, installed Google Noto font
    { "Lepc", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::yapprox(1700), EcFont::LEPCHA },
    // Limbu OK, W10 none, installed Google Noto font
    { "Limb", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(18), EcFont::LIMBU },
    // Linear A OK, W10 none, installed Google Noto font
    { "Lina", QFontDatabase::Any,
        EcScriptType::SYLLABOHIEROGLYPH, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::yapprox(-1800), EcFont::LINEAR_A },
    // Linear B OK, W10 none, installed Google Noto font
    { "Linb", QFontDatabase::Any,
        EcScriptType::SYLLABOHIEROGLYPH, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::yapprox(-1450), EcFont::LINEAR_B },
    // W10 has, W7 none (though lots of software install) → installed Google Noto
    { "Lisu", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1915), EcFont::LISU },
    // Lycian OK, W10 P1 Segoe Historic
    /// @todo [font???] Lycian
    { "Lyci", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(-6), EcFont::HISTORIC },
    // Lydian OK, W10 P1 Segoe Historic
    /// @todo [font???] Lydian
    { "Lydi", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::RTL_MOSTLY, EcContinent::ASIA,
        Dating::crange(-8, -7), EcFont::HISTORIC },
    // Mahajani OK, W10 none, installed Google Noto
    { "Mahj", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::unknown(), EcFont::MAHAJANI },
    // Makasar OK, installed strange font by Anshuman Pandey
    { "Maka", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::crange(16, 17), EcFont::MAKASAR },
    // Mandaic OK, W10 has not, installed Google Noto
    { "Mand", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::ENDANGERED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::crange(2, 7), EcFont::MANDAIC },
    // Manichaean OK, W10 has not, installed Google Noto
    { "Mani", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::century(3), EcFont::MANICHAEAN },
    // Marchen OK, W10 none, installed Google Noto
    { "Marc", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::SACRED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(7), EcFont::MARCHEN },
    // Medefaidrin OK, W10 none, installed Google Noto
    { "Medf", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::AFRICA,
        Dating::yapprox(1931), EcFont::MEDEFAIDRIN },
    // Mende Kikakui OK, W10 none → installed Google Noto
    { "Mend", QFontDatabase::Any,
        EcScriptType::SYLLABLE, EcLangLife::DEAD, EcWritingDir::RTL, EcContinent::AFRICA,
        Dating::yapprox(1920), EcFont::MENDE_KIKAKUI },
    // Meroitic cursive OK, W10 has small part
    { "Merc", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::AFRICA,
        Dating::century(-3), EcFont::MEROITIC },
    // Meroitic OK, W10 none
    { "Mero", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::RTL_COL, EcContinent::AFRICA,
        Dating::century(-3), EcFont::MEROITIC, Sfg::DESC_FROM_PREV },
    // Malayalam OK, W7 does not have 2014/17 extensions → installed Google Noto
    { "Mlym", QFontDatabase::Malayalam,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(830, StdNote::FIRST_KNOWN), EcFont::MALAYALAM },
    // Modi OK, W10 none, installed Google Noto
    { "Modi", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::yrange(1200, 1600), EcFont::MODI },
    // Mongol OK, dislike standard font of W10 → installed Google Noto
    { "Mong", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::REVIVED, EcWritingDir::LTR_COL, EcContinent::ASIA,
        Dating::special(), EcFont::MONGOLIAN },
    // Mro OK, installed Google Noto font
    { "Mroo", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::decade(1980), EcFont::MRO },
    // Meetei Mayek OK, W10 does not have extensions → installed Google Noto
    { "Mtei", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::REVIVED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::yapprox(1500, StdNote::FIRST_KNOWN), EcFont::MEETEI },
    // Multani OK, W10 none → installed Google Noto
    { "Mult", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(18), EcFont::MULTANI },
    // Myanmar OK, W10 “Myanmar Text”, W7 none → installed Google Noto font
    { "Mymr", QFontDatabase::Myanmar,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1035, StdNote::FIRST_KNOWN), EcFont::MYANMAR },
    /// @todo [U15] Nag alphabet (Mundari)
    { "Nagm", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::yrange(1949, 1982), EcFont::NORMAL },
    // Nandinagari OK, in Nov 2021 Google finally made Noto font
    { "Nand", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(8, StdNote::FIRST_KNOWN), EcFont::NANDINAGARI },
    // North Arabian OK, W10 none → installed Google Noto
    { "Narb", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::century(-3, StdNote::FIRST_KNOWN), EcFont::NORTH_ARABIAN },
    // Nabataean OK, W10 none → installed Google Noto
    { "Nbat", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::century(-2), EcFont::NABATAEAN },
    // Newa OK, W10 none → installed Google Noto
    { "Newa", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1654, StdNote::CUSTOM), EcFont::NEWA },
    // NKo ok, W10 “Ebrima”, W7 lacks a few chars → installed Google Noto
    { "Nkoo", QFontDatabase::Nko,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::RTL, EcContinent::AFRICA,
        Dating::year(1949), EcFont::NKO },
    // Nushu OK, W10 off, installed Google Noto
    { "Nshu", QFontDatabase::Any,
        EcScriptType::SYLLABLE, EcLangLife::DEAD, EcWritingDir::LTR_CJK, EcContinent::CJK,
        Dating::special(), EcFont::NUSHU },
    // Ogham OK, Windows font is differently named in W7 and W10 → installed Google Noto
    { "Ogam", QFontDatabase::Ogham,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::LTR_BU, EcContinent::EUROPE,
        Dating::century(4, StdNote::FIRST_KNOWN), EcFont::OGHAM },
    // Ol Chiki OK, W10 “Nirmala UI”, W7 none → installed Google Noto
    { "Olck", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1925), EcFont::OLCHIKI },
    { "Orkh", QFontDatabase::Any,
        EcScriptType::ALPHASYLLABLE, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::century(6), EcFont::ORKHON },
    // Oriya OK, took Noto + everything bad manually fixed
    { "Orya", QFontDatabase::Oriya,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(14, StdNote::MODERN_FORM), EcFont::ORIYA },
    // Osage OK, W10 Gadugi
    { "Osge", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::AMERICA,
        Dating::year(2006), EcFont::GADUGI },
    // Osmanya OK, W10 Ebrima
    { "Osma", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::AFRICA,
        Dating::yapprox(1920), EcFont::VAI },
    // Old Uyghur OK, installed Noto font
    { "Ougr", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::LTR_COL, EcContinent::ASIA,
        Dating::century(8), EcFont::OLD_UYGHUR },
    // Plamyrene OK, W10 none, installed Google Noto
    { "Palm", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::year(-44, StdNote::FIRST_KNOWN), EcFont::PALMYRENE },
    // Pau Cin Hau OK, W10 none, installed Google Noto
    { "Pauc", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::special(), EcFont::PAU_CIN_HAU},
    // Old Permic OK, W10 none, installed Google Noto
    { "Perm", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::year(1372), EcFont::OLD_PERMIC },
    // Phags-Pa OK, have “Microsoft PhagsPa” font
    { "Phag", QFontDatabase::Any,
        EcScriptType::ARGUABLE, EcLangLife::SACRED, EcWritingDir::LTR_COL, EcContinent::ASIA,
        Dating::year(1269), EcFont::PHAGS_PA },
    // Inscriptional Pahlavi OK, W10 Historic
    { "Phli", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::century(-2), EcFont::HISTORIC },
    // Psalter Pahlavi OK, W10 none → installed Google Noto
    { "Phlp", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::century(6, StdNote::FIRST_KNOWN), EcFont::PSALTER_PAHLAVI },
    // Phoenician OK, because of importance installed Google Noto
    { "Phnx", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::century(-11), EcFont::PHOENICIAN },
    // Miao/Pollard OK, W10 none, installed Google Noto
    { "Plrd", QFontDatabase::Any,
        EcScriptType::ABUGIDA, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1936), EcFont::POLLARD },
    // Inscriptional Parthian OK, W10 Historic
    { "Prti", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::yapprox(-250), EcFont::HISTORIC },
    // Rejang OK, installed Google Noto font
    { "Rjng", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::yapprox(1750, StdNote::FIRST_KNOWN), EcFont::REJANG },
    // Rohingya, installed Google Noto font
    { "Rohg", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::decade(1980), EcFont::ROHINGYA },
    // Runes OK, several tofu → installed Google Noto font
    { "Runr", QFontDatabase::Runic,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::century(2, StdNote::FIRST_KNOWN), EcFont::RUNIC },
    // Samaritan OK, installed Google Noto font, added dotted circle
    { "Samr", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::ENDANGERED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::yrange(-600, -200), EcFont::SAMARITAN },
    // South Arabian OK, W10 Segoe UI Historic
    /// @todo [font???] South Arabian
    { "Sarb", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::RTL_MOSTLY, EcContinent::ASIA,
        Dating::century(-9, StdNote::FIRST_KNOWN), EcFont::HISTORIC },
    // Saurashtra OK, W10 none, installed Google Noto
    { "Saur", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(19, StdNote::MAYBE_EARLIER), EcFont::SAURASHTRA },
    // Sutton SignWriting OK, installed Google Noto font
    { "Sgnw", QFontDatabase::Any,
        EcScriptType::NONTEXT, EcLangLife::NEW, EcWritingDir::TD, EcContinent::NONE,
        Dating::year(1974), EcFont::SIGNWRITING },
    // Shavian OK, W10 Segoe UI Historic
    { "Shaw", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::EXPERIMENTAL, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::year(1960), EcFont::HISTORIC },
    // Sharada OK, W10 none → installed Google Noto
    { "Shrd", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::SACRED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::yapprox(700), EcFont::SHARADA },
    // Siddham OK, W10 none, installed Google Noto font
    { "Sidd", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::SACRED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(6), EcFont::SIDDHAM },
    // Sindhi (Khudawadi) OK, W10 none, installed Google Noto font
    { "Sind", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(16), EcFont::KHUDAWADI },
    // Sinhala OK, W10 obviously has no 2020 extension → installed Google Noto
    { "Sinh", QFontDatabase::Sinhala,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::century(13, StdNote::MODERN_FORM), EcFont::SINHALA },
    // Sogdian OK, W10 None → installed Google Noto
    { "Sogd", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::SOGDIAN, EcContinent::ASIA,
        Dating::yapprox(500), EcFont::SOGDIAN },
    // Old Sogdian OK, W10 None → installed Google Noto
    { "Sogo", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::yapprox(100), EcFont::OLD_SOGDIAN, Sfg::DESC_FROM_PREV },
    // Sora Sompeng OK, W10 Nirmala UI
    { "Sora", QFontDatabase::Any,
        EcScriptType::ARGUABLE, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1936), EcFont::TAMIL },
    // Soyombo OK, W10 none → installed Google Noto
    { "Soyo", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::SACRED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1686), EcFont::SOYOMBO },
    // Sundanese OK, even W10 off → installed Google Noto
    { "Sund", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::REVIVED, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::century(14), EcFont::SUNDANESE },
    // Syloti Nagri OK, W10 none → installed Google Noto
    { "Sylo", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(15), EcFont::SYLOTI_NAGRI },
    // Syriac OK, W10 has no Malayalam extensions → Beth Mardutho made a nice font.
    { "Syrc", QFontDatabase::Syriac,
        EcScriptType::CONSONANT, EcLangLife::ENDANGERED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::century(1), EcFont::SYRIAC },
    // Tagbanwa OK, W10 off → installed Google Noto
    { "Tagb", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::yapprox(1300), EcFont::TAGBANWA },
    // Takri OK, W10 none → installed Google Noto
    { "Takr", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::crange(14, 18), EcFont::TAKRI },
    // Old Tai Le OK, M$ has a good font
    { "Tale", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::yapprox(1200), EcFont::TAILE },
    // New Tai Lue OK, M$ has a good font
    { "Talu", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::EXPERIMENTAL, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::decade(1950), EcFont::TAILUE },
    // Tamil BMP OK, different fonts in W10 and W7
    { "Taml", QFontDatabase::Tamil,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::crange(6, 9), EcFont::TAMIL },
    // Tangut OK, installed Google Noto
    { "Tang", QFontDatabase::Any,
        EcScriptType::HIEROGLYPH, EcLangLife::HISTORICAL, EcWritingDir::TANGUT, EcContinent::CJK,
        Dating::year(1036), EcFont::TANGUT },
    // Tai Viet OK, installed Google Noto font
    { "Tavt", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(16), EcFont::TAI_VIET },
    // Telugu OK, W7 lacks recent extensions → installed Google Noto
    { "Telu", QFontDatabase::Telugu,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(14, StdNote::CUSTOM), EcFont::TELUGU },
    // Tifinagh OK, W7 “Ebrima” of course has no 2012 chars → installed Google Noto
    { "Tfng", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::AFRICA,
        Dating::special(), EcFont::TIFINAGH },
    // Tagalog OK, W10 off → installed Google Noto
    { "Tglg", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::OCEAN,
        Dating::yrange(1300, 1500), EcFont::TAGALOG },
    // Thaana OK, Monotype/M$ “MV Boli” font
    { "Thaa", QFontDatabase::Thaana,
        EcScriptType::ARGUABLE, EcLangLife::ALIVE, EcWritingDir::RTL, EcContinent::OCEAN,
        Dating::century(18), EcFont::THAANA },
    // Thai OK, W7 and 10 fonts are different, but managed to
    { "Thai", QFontDatabase::Thai,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1283), EcFont::THAI },
    // Tibetan OK, W10 did not have swastika, installed Google Noto
    { "Tibt", QFontDatabase::Tibetan,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::yapprox(650), EcFont::TIBETAN },
    // Tirhuta OK, W10 none, installed Google Noto
    { "Tirh", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::century(13, StdNote::CUSTOM), EcFont::TIRHUTA },
    // Tangsa OK, U14, called author and he gave font
    { "Tnsa", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1990), EcFont::TANGSA },
    // Toto OK, U14, moved that font to FunkySample
    { "Toto", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(2015), EcFont::FUNKY },
    // Ugaritic OK
    { "Ugar", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::yapprox(-1400), EcFont::HISTORIC },
    // Vai OK, have good M$ font Ebrima
    { "Vaii", QFontDatabase::Any,
        EcScriptType::SYLLABLE, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::AFRICA,
        Dating::decade(1830), EcFont::VAI },
    // Vithkuqi OK, installed Google Noto
    { "Vith", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::EXPERIMENTAL, EcWritingDir::LTR, EcContinent::EUROPE,
        Dating::year(1844, StdNote::FIRST_KNOWN), EcFont::VITHKUQI },
    // Wancho OK, W10 none, installed Google Noto
    { "Wcho", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::yrange(2001, 2012), EcFont::WANCHO },
    // Warang Citi OK, W10 none → installed Google Noto
    { "Wara", QFontDatabase::Any,
        EcScriptType::ARGUABLE, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::decade(1950), EcFont::WARANG_CITI },
    // Old Persian OK. Small font, let’s install!!
    { "Xpeo", QFontDatabase::Any,
        EcScriptType::ALPHASYLLABLE, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::yapprox(-520), EcFont::OLD_PERSIAN },
    /// @todo [semi-tofu, P1] Sumerian cuneiform shows badly, Noto = M$ (but Noto is larger!)
    { "Xsux", QFontDatabase::Any,
        EcScriptType::SYLLABOHIEROGLYPH, EcLangLife::HISTORICAL, EcWritingDir::LTR_CUNEIFORM, EcContinent::ASIA,
        Dating::yapprox(-3200), EcFont::CUNEIFORM },
    // Yezidi OK, W10 none → installed Google Noto
    { "Yezi", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::SACRED, EcWritingDir::RTL, EcContinent::ASIA,
        Dating::crange(13, 17), EcFont::YEZIDI },
    // Yi OK, have M$ font. Though quite thin, Noto is better, I managed to counteract by increasing font.
    { "Yiii", QFontDatabase::Any,
        EcScriptType::SYLLABLE, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::year(1974, StdNote::CUSTOM), EcFont::YI },
    // Zanabazar square OK, installed Google Noto
    { "Zanb", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::ASIA,
        Dating::ybefore(1700), EcFont::ZANABAZAR },
    { "Zinh", QFontDatabase::Any,
        EcScriptType::NONE, EcLangLife::NOMATTER, EcWritingDir::NOMATTER, EcContinent::NONE,
        Dating::none(), EcFont::NORMAL, Sfg::NO_LANGS },
};


static_assert(std::size(uc::scriptInfo) == static_cast<size_t>(uc::EcScript::NN));


const uc::Script* uc::findScript(std::string_view x) { return findInArray(x, scriptInfo); }


constinit const uc::NumType uc::numTypeInfo[static_cast<int>(uc::EcNumType::NN)] {
    { {} },
    { "Prop.Num.Digit" },
    { "Prop.Num.SpecDigit" },
    { "Prop.Num.Number" },
};


constinit const uc::BidiClass uc::bidiClassInfo[static_cast<int>(EcBidiClass::NN)] {
    { "DIR", u8"FSI, LRE, LRI…",    EcBidiStrength::EXPLICIT, },    // Explicit chars in GenPunct
    { "AL",  u8"AL",                EcBidiStrength::STRONG },       // Any Arabic letter
    { "AN",  u8"AN",                EcBidiStrength::WEAK },         // Arabic digits (0660…), Rumi (10E60)

    { "B",   u8"B",                 EcBidiStrength::NEUTRAL },      // CR, LF and a few more
    { "BN",  u8"BN",                EcBidiStrength::NEUTRAL },      // Most controls
    { "CS",  u8"CS",                EcBidiStrength::WEAK },         // ,.

    { "EN",  u8"EN",                EcBidiStrength::WEAK },         // 012…
    { "ES",  u8"ES",                EcBidiStrength::WEAK },         // +-
    { "ET",  u8"ET",                EcBidiStrength::WEAK },         // #$°

    { "L",   u8"L",                 EcBidiStrength::STRONG },       // ABC…
    { "NSM", u8"NSM",               EcBidiStrength::WEAK },         // Combining umlaut
    { "ON",  u8"ON\u00A0+\u00A0¬\u00A0Bidi_M", EcBidiStrength::NEUTRAL },  // !"

    { "NM",  u8"ON\u00A0+\u00A0Bidi_M", EcBidiStrength::NEUTRAL },  // ()

    { "R",   u8"R",                 EcBidiStrength::STRONG },       // Hebrew
    { "S",   u8"S",                 EcBidiStrength::NEUTRAL },      // 09, 0B, 1F
    { "WS",  u8"WS",                EcBidiStrength::NEUTRAL },      // FF, space
    // { u8"Error" },  // check for equal number
};

void uc::BidiClass::printfLocKey(char* buf, size_t n, const char* suffix) const
    { snprintf(buf, n, "Prop.Bidi.%s.%s", id.data(), suffix); }



constinit const uc::Block uc::blocks[] {
    // Basic Latin OK
    { 0x0000, 0x007F, { 'L', EcContinent::EUROPE },
            "Basic Latin",
            uc::EcScript::Latn, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Latin-1 OK
    { 0x0080, 0x00FF, { 0xE4, EcContinent::EUROPE },
            "Latin-1 Supplement",
            EcScript::Latn, EcFont::NORMAL, Bfg::NO_EMOJI | Bfg::HAS_DESCRIPTION },
    // Latin extended A OK
    { 0x0100, 0x017F, { 0x153, EcContinent::EUROPE },
            "Latin Extended-A",
            EcScript::Latn, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Latin extended B OK
    { 0x0180, 0x024F, { 0x188, EcContinent::EUROPE },
            "Latin Extended-B",
            EcScript::Latn, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // IPA extensions OK
    { 0x0250, 0x02AF, { 0x259, EcContinent::NONE },
            "IPA Extensions" },
    // Spacing letter modifiers OK
    { 0x02B0, 0x02FF, { 0x2D0, EcContinent::NONE },
            "Spacing Modifier Letters",
            EcScript::NONE, EcFont::NORMAL,
            Bfg::FORCE_FONT | Bfg::SCRIPTLIKE },
    /// @todo [semi-tofu, BMP] Diacritical marks work somehow, though circle from 6 circles is too rough
    { 0x0300, 0x036F, { 0x301, EcContinent::NONE },
            "Combining Diacritical Marks" },
    // Greek and Coptic OK
    { 0x0370, 0x03FF, { L'ξ', EcContinent::EUROPE, Ifg::CONTINENT_OK },
            "Greek and Coptic" },
    // Cyrillic OK
    { 0x0400, 0x04FF, { 0x42F, EcContinent::EUROPE },
            "Cyrillic",
            EcScript::Cyrl, EcFont::NORMAL, Bfg::UNGLITCH_MARKS },
    // Cyrillic supplement OK
    { 0x0500, 0x052F, { 0x524, EcContinent::EUROPE },
            "Cyrillic Supplement",
            EcScript::Cyrl, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Armenian OK
    { 0x0530, 0x058F, { 0x536, EcContinent::ASIA },
            "Armenian", EcScript::Armn },
    // Hebrew OK, managed to draw yod triangle
    { 0x0590, 0x05FF, { 0x5D0, EcContinent::ASIA },
            "Hebrew", EcScript::Hebr },
    // Arabic OK
    { 0x0600, 0x06FF, { 0x636, EcContinent::ASIA },
            "Arabic", EcScript::Arab },
    /// @todo [font] #86 Which font to select and what to do with Syriac Malayalam?
    { 0x0700, 0x074F, { 0x71B, EcContinent::ASIA },
            "Syriac", EcScript::Syrc },
    // Arabic supplement OK
    { 0x0750, 0x077F, { 0x762, EcContinent::ASIA },
            "Arabic Supplement",
            EcScript::Arab, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Thaana OK
    { 0x0780, 0x07BF, { 0x798, EcContinent::OCEAN },
            "Thaana", EcScript::Thaa },
    // N’ko OK
    { 0x07C0, 0x07FF, { 0x7D0, EcContinent::AFRICA },
            "NKo", EcScript::Nkoo },
    // Samaritan OK
    { 0x0800, 0x083F, { 0x800, EcContinent::ASIA },
            "Samaritan", EcScript::Samr },
    // Mandaic OK
    { 0x0840, 0x085F, { 0x84C, EcContinent::ASIA },
            "Mandaic", EcScript::Mand },
    /// @todo [font] #86 Which font to select and what to do with Syriac Malayalam?
    { 0x0860, 0x086F, { 0x862, EcContinent::ASIA },
            "Syriac Supplement",
            EcScript::Syrc, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Arabic ex B OK, Sceherazade updated in Nov 2021
    { 0x0870, 0x089F, { 0x877, EcContinent::ASIA },
            "Arabic Extended-B",
            EcScript::Arab, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Arabic ex A OK
    { 0x08A0, 0x08FF, { 0x8A4, EcContinent::ASIA },
            "Arabic Extended-A",
            EcScript::Arab, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Devanagari OK
    { 0x0900, 0x097F, { 0x915, EcContinent::ASIA },
            "Devanagari", EcScript::Deva },
    // Bengali OK
    { 0x0980, 0x09FF, { 0x98B, EcContinent::ASIA },
            "Bengali", EcScript::Beng },
    // Gurmukhi OK
    { 0x0A00, 0x0A7F, { 0xA15, EcContinent::ASIA },
            "Gurmukhi", EcScript::Guru },
    // Gujarati OK, Google Noto + FORCE_TOFU show nothing
    { 0x0A80, 0x0AFF, { 0xA9B, EcContinent::ASIA },
            "Gujarati", EcScript::Gujr },
    // Oriya OK, Noto + everything bad manually fixed
    { 0x0B00, 0x0B7F, { 0xB1C, EcContinent::ASIA },
            "Oriya", EcScript::Orya },
    // Tamil OK, font pair is good
    { 0x0B80, 0x0BFF, { 0xB95, EcContinent::ASIA },
            "Tamil", EcScript::Taml },
    // Telugu OK
    { 0x0C00, 0x0C7F, { 0xC1C, EcContinent::ASIA },
            "Telugu", EcScript::Telu },
    // Kannada OK
    { 0x0C80, 0x0CFF, { 0xCA0, EcContinent::ASIA },
            "Kannada", EcScript::Knda },
    // Malayalam OK
    { 0x0D00, 0x0D7F, { 0xD16, EcContinent::ASIA },
            "Malayalam", EcScript::Mlym },
    // Sinhala OK
    { 0x0D80, 0x0DFF, { 0xD9A, EcContinent::OCEAN },
            "Sinhala", EcScript::Sinh },
    // Thai OK
    { 0x0E00, 0x0E7F, { 0xE0A, EcContinent::ASIA },
            "Thai", EcScript::Thai },
    // Lao OK
    { 0x0E80, 0x0EFF, { 0xE9D, EcContinent::ASIA },
            "Lao", EcScript::Laoo },
    // Tibetan OK
    { 0x0F00, 0x0FFF, { 0xF4F, EcContinent::ASIA },
            "Tibetan", EcScript::Tibt },
    // Myanmar OK
    { 0x1000, 0x109F, { 0x101E, EcContinent::ASIA },
            "Myanmar", EcScript::Mymr },
    // Georgian OK
    { 0x10A0, 0x10FF, { 0x10D3, EcContinent::EUROPE },
            "Georgian", EcScript::Geor },
    { 0x1100, 0x11FF, { 0x113F, EcContinent::CJK },
            "Hangul Jamo",
            EcScript::Hang, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Ethiopic OK
    { 0x1200, 0x137F, { 0x12F1, EcContinent::AFRICA },
            "Ethiopic", EcScript::Ethi },
    // Ethiopic supplement OK
    { 0x1380, 0x139F, { 0x1382, EcContinent::AFRICA },
            "Ethiopic Supplement",
            EcScript::Ethi, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Cherokee OK
    { 0x13A0, 0x13FF, { 0x13B6, EcContinent::AMERICA },
            "Cherokee", EcScript::Cher },
    // Canadian aboriginal OK
    { 0x1400, 0x167F, { 0x140E, EcContinent::AMERICA },
            "Unified Canadian Aboriginal Syllabics", EcScript::Cans },
    // Ogham OK
    { 0x1680, 0x169F, { 0x168E, EcContinent::EUROPE },
            "Ogham", EcScript::Ogam },
    // Runic OK
    { 0x16A0, 0x16FF, { 0x16A0, EcContinent::EUROPE },
            "Runic", EcScript::Runr },
    // Tagalog OK
    { 0x1700, 0x171F, { 0x1706, EcContinent::OCEAN },
            "Tagalog", EcScript::Tglg },
    // Hanunoo OK
    { 0x1720, 0x173F, { 0x1726, EcContinent::OCEAN },
            "Hanunoo", EcScript::Hano },
    // Buhid OK
    { 0x1740, 0x175F, { 0x1746, EcContinent::OCEAN },
            "Buhid", EcScript::Buhd },
    // Tagbanwa OK
    { 0x1760, 0x177F, { 0x1766, EcContinent::OCEAN },
            "Tagbanwa", EcScript::Tagb },
    // Khmer OK
    { 0x1780, 0x17FF, { 0x178D, EcContinent::ASIA },
            "Khmer", EcScript::Khmr },
    // Mongol OK, implemented ligature
    { 0x1800, 0x18AF, { 0x183E, EcContinent::ASIA, Ifg::APPROX_ROTATED },
            "Mongolian", EcScript::Mong },
    // Canadian extended OK
    { 0x18B0, 0x18FF, { 0x18E5, EcContinent::AMERICA },
            "Unified Canadian Aboriginal Syllabics Extended",
            EcScript::Cans, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Limbu OK
    { 0x1900, 0x194F, { 0x1900, EcContinent::ASIA },
            "Limbu", EcScript::Limb },
    // Tai Le OK
    { 0x1950, 0x197F, { 0x195E, EcContinent::ASIA },
            "Tai Le", EcScript::Tale },
    // New Tai Lue OK
    { 0x1980, 0x19DF, { 0x19C5, EcContinent::ASIA },
            "New Tai Lue", EcScript::Talu },
    // Khmer syms OK
    { 0x19E0, 0x19FF, { 0x19EF, EcContinent::ASIA },
            "Khmer Symbols",
            EcScript::Khmr, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Buginese OK
    { 0x1A00, 0x1A1F, { 0x1A01, EcContinent::OCEAN },
            "Buginese", EcScript::Bugi },
    // Lanna OK
    { 0x1A20, 0x1AAF, { 0x1A28, EcContinent::ASIA },
            "Tai Tham", EcScript::Lana },
    // Diacritical ex OK
    { 0x1AB0, 0x1AFF, { 0x1AB2, EcContinent::NONE },
            "Combining Diacritical Marks Extended",
            EcScript::NONE, EcFont::NOTO },
    // Balinese OK
    { 0x1B00, 0x1B7F, { 0x1B11, EcContinent::OCEAN },
            "Balinese", EcScript::Bali },
    // Sundanese OK
    { 0x1B80, 0x1BBF, { 0x1B91, EcContinent::OCEAN },
            "Sundanese", EcScript::Sund },
    // Batak OK
    { 0x1BC0, 0x1BFF, { 0x1BD4, EcContinent::OCEAN },
            "Batak", EcScript::Batk },
    // Lepcha OK
    { 0x1C00, 0x1C4F, { 0x1C1D, EcContinent::ASIA },
            "Lepcha", EcScript::Lepc },
    // Ol Chiki OK
    { 0x1C50, 0x1C7F, { 0x1C5A, EcContinent::ASIA },
            "Ol Chiki", EcScript::Olck },
    // Cyr C OK
    { 0x1C80, 0x1C8F, { 0x1C88, EcContinent::EUROPE },
            "Cyrillic Extended-C",
            EcScript::Cyrl, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Geor ex OK
    { 0x1C90, 0x1CBF, { 0x1C93, EcContinent::EUROPE, Ifg::APPROX_LINES },
            "Georgian Extended",
            EcScript::Geor, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Sundanese supplement OK
    { 0x1CC0, 0x1CCF, { 0x1CC6, EcContinent::OCEAN },
            "Sundanese Supplement",
            EcScript::Sund, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Vedic ex OK
    { 0x1CD0, 0x1CFF, { 0x1CEC, EcContinent::ASIA, Ifg::CONTINENT_OK },
            "Vedic Extensions", EcScript::NONE, EcFont::VEDIC },
    // Phonetic ext OK
    { 0x1D00, 0x1D7F, { 0x1D6F, EcContinent::NONE },
            "Phonetic Extensions",
            EcScript::NONE, EcFont::NORMAL, Bfg::FORCE_FONT },
    // Phonetic ext supp OK
    { 0x1D80, 0x1DBF, { 0x1D95, EcContinent::NONE },
            "Phonetic Extensions Supplement", },
    // Combining supp OK, fixed manually in Noto
    { 0x1DC0, 0x1DFF, { 0x1DD4, EcContinent::NONE },
            "Combining Diacritical Marks Supplement",
            EcScript::NONE, EcFont::NOTO },
    // Lat ex OK
    { 0x1E00, 0x1EFF, { 0x1EEA, EcContinent::EUROPE },
            "Latin Extended Additional",
            EcScript::Latn, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Greek ex OK, though DejaVu is a big gun
    { 0x1F00, 0x1FFF, { 0x1F6A, EcContinent::EUROPE },
            "Greek Extended",
            EcScript::Grek, EcFont::DEJAVU, Bfg::HAS_DESCRIPTION },
    // General punct OK, changed 2-dot leader to low one.
    { 0x2000, 0x206F, { L'‰', EcContinent::NONE },
            "General Punctuation",
            EcScript::NONE, EcFont::PUNCTUATION },
    // Sup/sub OK
    { 0x2070, 0x209F, { 0x2074, EcContinent::NONE, Ifg::APPROX_2_CHARS },
            "Superscripts and Subscripts" },
    // Curr symbols OK
    { 0x20A0, 0x20CF, { 0x20AC, EcContinent::NONE },
            "Currency Symbols" },
    // Marks for symbols OK, made optimized versions in Noto
    { 0x20D0, 0x20FF, { 0x20E0, EcContinent::NONE },
            "Combining Diacritical Marks for Symbols",
            EcScript::NONE, EcFont::UMLAUT_SYMBOL },
    // Letterlike OK
    { 0x2100, 0x214F, { 0x211D, EcContinent::NONE },
            "Letterlike Symbols",
            EcScript::NONE, EcFont::NORMAL, Bfg::SCRIPTLIKE },
    // Number forms OK
    { 0x2150, 0x218F, { L'⅓', EcContinent::NONE },
            "Number Forms", },
    // Arrows OK
    { 0x2190, 0x21FF, { L'↑', EcContinent::NONE },
            "Arrows",
            EcScript::NONE, EcFont::NORMAL, Bfg::NO_EMOJI },
    // Math op OK
    { 0x2200, 0x22FF, { L'√', EcContinent::NONE },
            "Mathematical Operators" },
    // Misc tech OK
    { 0x2300, 0x23FF, { L'⏻', EcContinent::NONE },
            "Miscellaneous Technical",
            EcScript::NONE, EcFont::TECHNICAL },
    // Control pictures OK
    { 0x2400, 0x243F, { L'␣', EcContinent::NONE },
            "Control Pictures",
            EcScript::NONE, EcFont::NOTO_SYMBOL2_BIGGER },
    // OCR/MICR OK
    { 0x2440, 0x245F, { L'⑀', EcContinent::NONE },
            "Optical Character Recognition",
            // Fonts coincide
            EcScript::NONE, EcFont::PHAISTOS_DISC },
    // Enclosed alnum OK, need ordinary Cambria
    { 0x2460, 0x24FF, { L'①', EcContinent::NONE },
            "Enclosed Alphanumerics",
            EcScript::NONE, EcFont::NORMAL, Bfg::SCRIPTLIKE },
    // Box drawing OK
    { 0x2500, 0x257F, { L'╢', EcContinent::NONE },
            "Box Drawing",
            EcScript::NONE, EcFont::DEJAVU },    // Block elements OK
    // Block elements OK
    { 0x2580, 0x259F, { L'░', EcContinent::NONE },
            "Block Elements",
            EcScript::NONE, EcFont::DEJAVU },
    // Geometric OK
    { 0x25A0, 0x25FF, { L'◆', EcContinent::NONE },
            "Geometric Shapes" },
    // Misc sym OK
    { 0x2600, 0x26FF, { L'☺', EcContinent::NONE },
            "Miscellaneous Symbols" },
    // Dingbats OK, need Cambria here!
    { 0x2700, 0x27BF, { L'❧', EcContinent::NONE },
            "Dingbats",
            EcScript::NONE, EcFont::DINGBAT2 },
    // Misc math A OK
    { 0x27C0, 0x27EF, { L'⟈', EcContinent::NONE },
            "Miscellaneous Mathematical Symbols-A",
            EcScript::NONE, EcFont::MATH },
    // Arrows A OK
    { 0x27F0, 0x27FF, { L'⟳', EcContinent::NONE },
            "Supplemental Arrows-A" },
    // Braille OK
    { 0x2800, 0x28FF, { L'⠝', EcContinent::NONE },
            "Braille Patterns", EcScript::Brai },
    // Arrows B OK
    { 0x2900, 0x297F, { L'⤶', EcContinent::NONE },
            "Supplemental Arrows-B" },
    // Misc math B OK
    { 0x2980, 0x29FF, { L'⧮', EcContinent::NONE },
            "Miscellaneous Mathematical Symbols-B",
            EcScript::NONE, EcFont::MATH },
    // Supp math ops OK
    { 0x2A00, 0x2AFF, { L'⨔', EcContinent::NONE },
            "Supplemental Mathematical Operators",
            EcScript::NONE, EcFont::MATH },
    // Misc syms & arrows OK
    { 0x2B00, 0x2BFF, { L'⮊', EcContinent::NONE },
            "Miscellaneous Symbols and Arrows" },
    // Glagolitic OK
    { 0x2C00, 0x2C5F, { 0x2C19, EcContinent::EUROPE },
            "Glagolitic", EcScript::Glag },
    // Latin C OK
    { 0x2C60, 0x2C7F, { L'Ɱ', EcContinent::EUROPE },
            "Latin Extended-C",
            EcScript::Latn, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Coptic OK
    { 0x2C80, 0x2CFF, { L'Ⲁ', EcContinent::AFRICA },
            "Coptic", EcScript::Copt },
    // Georgian supp OK
    { 0x2D00, 0x2D2F, { L'ⴃ', EcContinent::EUROPE },
            "Georgian Supplement",
            EcScript::Geor, EcFont::GEORGIAN_NUSKHURI, Bfg::HAS_DESCRIPTION },
    // Tifinagh OK
    { 0x2D30, 0x2D7F, { L'ⵣ', EcContinent::AFRICA },
            "Tifinagh", EcScript::Tfng },
    // Ethiopic ex OK
    { 0x2D80, 0x2DDF, { L'ⶤ', EcContinent::AFRICA },
            "Ethiopic Extended",
            EcScript::Ethi, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Cyr ex A OK, fixed those renderings in Noto
    { 0x2DE0, 0x2DFF, { 0x2DF0, EcContinent::EUROPE },
            "Cyrillic Extended-A",
            EcScript::Cyrl, EcFont::NOTO, Bfg::HAS_DESCRIPTION },
    // Supp punct OK, drew U14 in FunkySample
    { 0x2E00, 0x2E7F, { L'⸘', EcContinent::NONE },
            "Supplemental Punctuation",
            EcScript::NONE, EcFont::PUNCTUATION },
    // CJK radicals supp OK
    { 0x2E80, 0x2EFF, { L'⺓', EcContinent::CJK },
            "CJK Radicals Supplement",
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Kangxi OK
    { 0x2F00, 0x2FDF, { L'⼊', EcContinent::CJK },
            "Kangxi Radicals",
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Ideographic desc OK, not collapsible: small block
    { 0x2FF0, 0x2FFF, { L'⿺', EcContinent::CJK, Ifg::CONTINENT_OK },
            "Ideographic Description Characters",
            EcScript::NONE, EcFont::CJK_STRUCTURE, Bfg::CJK },
    // CJK sym/punct OK
    { 0x3000, 0x303F, { L'〠', EcContinent::CJK },
            "CJK Symbols and Punctuation",
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Hiragana OK, installed small version of Noto CJK Korean
    { 0x3040, 0x309F, { L'ぢ', EcContinent::CJK },
            "Hiragana", EcScript::Hira },
    // Katakana OK, not collapsible: syllabary
    { 0x30A0, 0x30FF, { L'ヂ', EcContinent::CJK },
            "Katakana", EcScript::Kana },
    // Bopomofo OK
    { 0x3100, 0x312F, { L'ㄉ', EcContinent::CJK },
            "Bopomofo",
            EcScript::Bopo, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // Hangul compat jamo OK
    { 0x3130, 0x318F, { L'ㅭ', EcContinent::CJK },
            "Hangul Compatibility Jamo",
            EcScript::Hang, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Kanbun OK, not collapsible: small block
    { 0x3190, 0x319F, { L'㆝', EcContinent::CJK, Ifg::CONTINENT_OK },
            "Kanbun",
            EcScript::NONE, EcFont::CJK_KANBUN, Bfg::CJK },
    // Bopomofo OK, at least in W10
    { 0x31A0, 0x31BF, { L'ㆮ', EcContinent::CJK },
            "Bopomofo Extended",
            EcScript::Bopo, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // CJK strokes OK
    { 0x31C0, 0x31EF, { L'㇉', EcContinent::CJK },
            "CJK Strokes",
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Katakana phon OK, not collapsible: small block
    { 0x31F0, 0x31FF, { L'ㇹ', EcContinent::CJK },
            "Katakana Phonetic Extensions",
            EcScript::Kana, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Enclosed CJK letters and months OK
    { 0x3200, 0x32FF, { L'㋀', EcContinent::CJK },
            "Enclosed CJK Letters and Months",
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // CJK compatibility OK
    { 0x3300, 0x33FF, { L'㌀', EcContinent::CJK },
            "CJK Compatibility",
            EcScript::Hani, EcFont::CJK_COMPAT, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    { 0x3400, 0x4DBF, { L'㐾', EcContinent::CJK },
            "CJK Unified Ideographs Extension A",
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Yijing OK
    { 0x4DC0, 0x4DFF, { L'䷜', EcContinent::CJK, Ifg::CONTINENT_OK },
            "Yijing Hexagram Symbols" },
    // CJK hieroglyphs OK
    { 0x4E00, 0x9FFF, { L'丈', EcContinent::CJK },
            "CJK Unified Ideographs",
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // Yi OK
    { 0xA000, 0xA48F, { 0xA011, EcContinent::ASIA },
            "Yi Syllables", EcScript::Yiii },
    // Yi radicals OK
    { 0xA490, 0xA4CF, { 0xA4BA, EcContinent::ASIA },
            "Yi Radicals",
            EcScript::Yiii, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Lisu OK
    { 0xA4D0, 0xA4FF, { L'ꓤ', EcContinent::ASIA },
            "Lisu", EcScript::Lisu },
    // Vai OK
    { 0xA500, 0xA63F, { L'ꔃ', EcContinent::AFRICA },
            "Vai", EcScript::Vaii },
    // Cyr ex B OK, modified font a bit
    { 0xA640, 0xA69F, { L'Ꙛ', EcContinent::EUROPE },
            "Cyrillic Extended-B",
            EcScript::Cyrl, EcFont::NORMAL, Bfg::UNGLITCH_MARKS | Bfg::HAS_DESCRIPTION },
    // Bamum OK
    { 0xA6A0, 0xA6FF, { 0xA6AE, EcContinent::AFRICA },
            "Bamum", EcScript::Bamu },
    // Modifier tone OK
    { 0xA700, 0xA71F, { L'ꜛ', EcContinent::NONE },
            "Modifier Tone Letters" },
    // Latin ex D OK, dereferenced bad chars of Google Noto, implemented U14 in Funky
    { 0xA720, 0xA7FF, { L'Ꝕ', EcContinent::EUROPE },
            "Latin Extended-D",
            EcScript::Latn, EcFont::LATIN, Bfg::HAS_DESCRIPTION },
    // Syloti OK
    { 0xA800, 0xA82F, { 0xA808, EcContinent::ASIA },
            "Syloti Nagri", EcScript::Sylo },
    // Common Indic OK
    { 0xA830, 0xA83F, { 0xA835, EcContinent::ASIA },
            "Common Indic Number Forms",
            EcScript::Deva, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Phang-pa OK
    { 0xA840, 0xA87F, { 0xA850, EcContinent::ASIA, Ifg::APPROX_ROTATED },
            "Phags-pa", EcScript::Phag },
    // Saurashtra OK
    { 0xA880, 0xA8DF, { 0xA8A5, EcContinent::ASIA },
            "Saurashtra", EcScript::Saur },
    // Devanagari ex OK, drew 4 chars
    { 0xA8E0, 0xA8FF, { 0xA8EC, EcContinent::ASIA },
            "Devanagari Extended",
            EcScript::Deva, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Kayah Li OK
    { 0xA900, 0xA92F, { 0xA922, EcContinent::ASIA, Ifg::APPROX_2_CHARS },
            "Kayah Li", EcScript::Kali },
    // Rejang OK
    { 0xA930, 0xA95F, { 0xA93B, EcContinent::OCEAN },
            "Rejang", EcScript::Rjng },
    // Hangul jamo A OK
    { 0xA960, 0xA97F, { L'ꥵ', EcContinent::CJK },
            "Hangul Jamo Extended-A",
            EcScript::Hang, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Javanese OK
    { 0xA980, 0xA9DF, { L'ꦈ', EcContinent::OCEAN },
            "Javanese", EcScript::Java },
    // Myanmar ex B OK
    { 0xA9E0, 0xA9FF, { L'ꧬ', EcContinent::ASIA },
            "Myanmar Extended-B",
            EcScript::Mymr, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Cham OK
    { 0xAA00, 0xAA5F, { 0xAA0C, EcContinent::ASIA },
            "Cham", EcScript::Cham },
    // Myanmar ex A OK
    { 0xAA60, 0xAA7F, { L'ꩴ', EcContinent::ASIA },
            "Myanmar Extended-A",
            EcScript::Mymr, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Tai Viet OK
    { 0xAA80, 0xAADF, { 0xAA8F, EcContinent::ASIA },
            "Tai Viet", EcScript::Tavt },
    // Meetei ex OK
    { 0xAAE0, 0xAAFF, { 0xAAE6, EcContinent::ASIA },
            "Meetei Mayek Extensions",
            EcScript::Mtei, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Ethiopic ex A OK
    { 0xAB00, 0xAB2F, { 0xAB2A, EcContinent::AFRICA },
            "Ethiopic Extended-A",
            EcScript::Ethi, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Latin ex E OK, dereferenced bad chars of Google Noto
    { 0xAB30, 0xAB6F, { 0xAB66, EcContinent::EUROPE },
            "Latin Extended-E",
            EcScript::Latn, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Cherokee supp OK
    { 0xAB70, 0xABBF, { L'ꮆ', EcContinent::AMERICA },
            "Cherokee Supplement",
            EcScript::Cher, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Meetei OK
    { 0xABC0, 0xABFF, { 0xABC4, EcContinent::ASIA },
            "Meetei Mayek", EcScript::Mtei },
    // Hangul syllables OK
    { 0xAC00, 0xD7AF, { L'괏', EcContinent::CJK },
            "Hangul Syllables",
            EcScript::Hang, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Hangul Jamo B OK
    { 0xD7B0, 0xD7FF, { L'ힽ', EcContinent::CJK },
            "Hangul Jamo Extended-B",
            EcScript::Hang, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // CJK compat hiero OK
    { 0xF900, 0xFAFF, { L'車', EcContinent::CJK },
            "CJK Compatibility Ideographs",
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Alphabetic presentation forms OK
    { 0xFB00, 0xFB4F, { L'ﬄ', EcContinent::NONE },
            "Alphabetic Presentation Forms" },
    // Arabic presentation forms A OK
    { 0xFB50, 0xFDFF, { 0xFB6D, EcContinent::ASIA },
            "Arabic Presentation Forms-A",
            EcScript::Arab, EcFont::ARABIC_NOTO, Bfg::HAS_32_NONCHARS | Bfg::HAS_DESCRIPTION },
    // Variation selectors OK
    { 0xFE00, 0xFE0F, { 0xFE00, EcContinent::NONE, Ifg::APPROX_COLLECTIVE },
            "Variation Selectors" },
    // Vertical forms OK, not collapsible: small
    { 0xFE10, 0xFE1F, { L'︗', EcContinent::CJK, Ifg::CONTINENT_OK },
            "Vertical Forms",
            EcScript::NONE, EcFont::CJK_STRUCTURE },
    // Combining half marks OK
    { 0xFE20, 0xFE2F, { 0xFE20, EcContinent::NONE, Ifg::APPROX_2_CHARS },
            "Combining Half Marks",
            EcScript::NONE, EcFont::NOTO },
    // CJK compat forms OK
    { 0xFE30, 0xFE4F, { L'﹃', EcContinent::CJK },
            "CJK Compatibility Forms",
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Small variants OK
    { 0xFE50, 0xFE6F, { L'﹖', EcContinent::CJK, Ifg::CONTINENT_OK },
            "Small Form Variants",
            EcScript::NONE, EcFont::CJK, Bfg::COLLAPSIBLE },
    // Arabic forms B OK, no combinings
    { 0xFE70, 0xFEFF, { 0xFEA0, EcContinent::ASIA },
            "Arabic Presentation Forms-B",
            EcScript::Arab, EcFont::ARABIC_NOTO, Bfg::HAS_DESCRIPTION },
    // Half/full OK
    { 0xFF00, 0xFFEF, { L'５', EcContinent::CJK, Ifg::CONTINENT_OK },
            "Halfwidth and Fullwidth Forms",
            EcScript::NONE, EcFont::CJK, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Specials OK
    { 0xFFF0, 0xFFFF, { L'�', EcContinent::NONE },
            "Specials", },
    // Linear B syll OK
    { 0x10000, 0x1007F, { 0x10002, EcContinent::EUROPE },
            "Linear B Syllabary", EcScript::Linb },
    // Linear B hier OK
    { 0x10080, 0x100FF, { 0x100E0, EcContinent::EUROPE },
            "Linear B Ideograms", EcScript::Linb },
    // Aegean numbers OK
    { 0x10100, 0x1013F, { 0x1010D, EcContinent::EUROPE },
            "Aegean Numbers",
            EcScript::Linb, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Greek numbers OK
    { 0x10140, 0x1018F, { 0x10144, EcContinent::EUROPE },
        "Ancient Greek Numbers",
            EcScript::Grek, EcFont::NOTO_SYMBOL2, Bfg::HAS_DESCRIPTION },
    // Ancient symbols OK
    { 0x10190, 0x101CF, { 0x10192, EcContinent::EUROPE, Ifg::CONTINENT_OK },
            "Ancient Symbols",
            EcScript::NONE, EcFont::NOTO_SYMBOL2 },
    // Phaistos Disc OK
    { 0x101D0, 0x101FF, { 0x101D1, EcContinent::EUROPE, Ifg::CONTINENT_OK },
        "Phaistos Disc",
            EcScript::NONE, EcFont::PHAISTOS_DISC },
    // Lycian OK
    { 0x10280, 0x1029F, { 0x10299, EcContinent::ASIA },
            "Lycian", EcScript::Lyci },
    // Carian OK
    { 0x102A0, 0x102DF, { 0x102C6, EcContinent::ASIA },
            "Carian", EcScript::Cari },
    // Coptic epact OK
    { 0x102E0, 0x102FF, { 0x102F1, EcContinent::AFRICA },
        "Coptic Epact Numbers",
            EcScript::Copt, EcFont::NOTO_SYMBOL2, Bfg::HAS_DESCRIPTION },
    // Old Italic OK
    { 0x10300, 0x1032F, { 0x10300, EcContinent::EUROPE },
            "Old Italic", EcScript::Ital },
    // Gothic OK
    { 0x10330, 0x1034F, { 0x10330, EcContinent::EUROPE },
            "Gothic", EcScript::Goth },
    // Old Permic OK
    { 0x10350, 0x1037F, { 0x10350, EcContinent::EUROPE },
            "Old Permic", EcScript::Perm },
    // Ugaritic OK
    { 0x10380, 0x1039F, { 0x10380, EcContinent::ASIA },
            "Ugaritic", EcScript::Ugar },
    // Old Persian OK
    { 0x103A0, 0x103DF, { 0x103A2, EcContinent::ASIA },
            "Old Persian", EcScript::Xpeo },
    // Deseret OK
    { 0x10400, 0x1044F, { 0x10414, EcContinent::AMERICA },
            "Deseret", EcScript::Dsrt },
    // Shavian OK
    { 0x10450, 0x1047F, { 0x10459, EcContinent::EUROPE },
            "Shavian", EcScript::Shaw },
    // Osmanya OK
    { 0x10480, 0x104AF, { 0x10499, EcContinent::AFRICA },
            "Osmanya", EcScript::Osma },
    // Osage OK
    { 0x104B0, 0x104FF, { 0x104C8, EcContinent::AMERICA },
            "Osage", EcScript::Osge },
    // Elbasan OK
    { 0x10500, 0x1052F, { 0x10500, EcContinent::EUROPE },
            "Elbasan", EcScript::Elba },
    // Caucasian OK
    { 0x10530, 0x1056F, { 0x10530, EcContinent::EUROPE },
            "Caucasian Albanian", EcScript::Aghb },
    // Vithkuqi OK
    { 0x10570, 0x105BF, { 0x10570, EcContinent::EUROPE },
            "Vithkuqi", EcScript::Vith },
    // Linear A OK
    { 0x10600, 0x1077F, { 0x1062C, EcContinent::EUROPE },
            "Linear A", EcScript::Lina },
    // Latin ex F OK, drew in FunkySample
    { 0x10780, 0x107BF, { 0x107BA, EcContinent::EUROPE, Ifg::APPROX_LINES },
            "Latin Extended-F",
            EcScript::Latn, EcFont::FUNKY, Bfg::HAS_DESCRIPTION },
    // Cypriot OK
    { 0x10800, 0x1083F, { 0x1080E, EcContinent::EUROPE },
            "Cypriot Syllabary", EcScript::Cprt },
    // Imperial Aramaic OK
    { 0x10840, 0x1085F, { 0x10840, EcContinent::ASIA },
            "Imperial Aramaic", EcScript::Armi },
    // Palmyrene OK
    { 0x10860, 0x1087F, { 0x10860, EcContinent::ASIA },
            "Palmyrene", EcScript::Palm },
    // Nabataean OK
    { 0x10880, 0x108AF, { 0x10880, EcContinent::ASIA },
            "Nabataean", EcScript::Nbat },
    // Hatran OK
    { 0x108E0, 0x108FF, { 0x108E0, EcContinent::ASIA },
            "Hatran", EcScript::Hatr },
    // Phoenician OK
    { 0x10900, 0x1091F, { 0x10900, EcContinent::ASIA },
            "Phoenician", EcScript::Phnx },
    // Lydian OK
    { 0x10920, 0x1093F, { 0x10920, EcContinent::ASIA },
            "Lydian", EcScript::Lydi },
    // Meroitic hiero OK
    { 0x10980, 0x1099F, { 0x10980, EcContinent::AFRICA },
            "Meroitic Hieroglyphs", EcScript::Mero },
    // Meroitic cursive OK
    { 0x109A0, 0x109FF, { 0x109A0, EcContinent::AFRICA },
            "Meroitic Cursive", EcScript::Mero },
    // Kharoshthi OK
    { 0x10A00, 0x10A5F, { 0x10A10, EcContinent::ASIA },
            "Kharoshthi", EcScript::Khar },
    // Old South Arab OK
    { 0x10A60, 0x10A7F, { 0x10A71, EcContinent::ASIA },
            "Old South Arabian", EcScript::Sarb },
    // Old North Arab OK
    { 0x10A80, 0x10A9F, { 0x10A91, EcContinent::ASIA },
            "Old North Arabian", EcScript::Narb },
    // Manichaean OK
    { 0x10AC0, 0x10AFF, { 0x10AC0, EcContinent::ASIA },
            "Manichaean", EcScript::Mani },
    // Avestan OK
    { 0x10B00, 0x10B3F, { 0x10B00, EcContinent::ASIA },
            "Avestan", EcScript::Avst },
    // Inscr Parthian OK
    { 0x10B40, 0x10B5F, { 0x10B40, EcContinent::ASIA },
            "Inscriptional Parthian", EcScript::Prti },
    // Inscr Pahlavi OK
    { 0x10B60, 0x10B7F, { 0x10B60, EcContinent::ASIA },
            "Inscriptional Pahlavi", EcScript::Phli },
    // Psalter Pahlavi OK
    { 0x10B80, 0x10BAF, { 0x10B99, EcContinent::ASIA, Ifg::APPROX_COLOR },
            "Psalter Pahlavi", EcScript::Phlp },
    // Turkic runes OK
    { 0x10C00, 0x10C4F, { 0x10C14, EcContinent::ASIA },
            "Old Turkic", EcScript::Orkh },
    // Hung runes OK
    { 0x10C80, 0x10CFF, { 0x10CAF, EcContinent::EUROPE },
            "Old Hungarian", EcScript::Hung },
    // Hanifi OK
    { 0x10D00, 0x10D3F, { 0x10D0D, EcContinent::ASIA },
            "Hanifi Rohingya", EcScript::Rohg },
    // Rumi OK
    { 0x10E60, 0x10E7F, { 0x10E77, EcContinent::AFRICA, Ifg::CONTINENT_OK },
            "Rumi Numeral Symbols",
            EcScript::NONE, EcFont::NOTO_SYMBOL2 },
    // Yezidi OK
    { 0x10E80, 0x10EBF, { 0x10E91, EcContinent::ASIA },
            "Yezidi", EcScript::Yezi },
    /// @todo [U15] Arabic ex C
    { 0x10EC0, 0x10EFF, { '?', EcContinent::MISSING, Ifg::CONTINENT_OK },
            "Arabic Extended-C",
            EcScript::Arab, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Old Sogdian OK
    { 0x10F00, 0x10F2F, { 0x10F00, EcContinent::ASIA },
            "Old Sogdian", EcScript::Sogo },
    // Sogdian OK
    { 0x10F30, 0x10F6F, { 0x10F30, EcContinent::ASIA },
            "Sogdian", EcScript::Sogd },
    // Old Uyghur OK
    { 0x10F70, 0x10FAF, { 0x10F70, EcContinent::ASIA },
            "Old Uyghur", EcScript::Ougr },
    // Chorasmian OK
    { 0x10FB0, 0x10FDF, { 0x10FB0, EcContinent::ASIA },
            "Chorasmian", EcScript::Chrs },
    // Elymaic OK
    { 0x10FE0, 0x10FFF, { 0x10FE0, EcContinent::ASIA },
            "Elymaic", EcScript::Elym },
    // Brahmi OK
    { 0x11000, 0x1107F, { 0x1101C, EcContinent::ASIA },
            "Brahmi", EcScript::Brah },
    // Kaithi OK
    { 0x11080, 0x110CF, { 0x1108D, EcContinent::ASIA },
            "Kaithi", EcScript::Kthi },
    // Sora OK
    { 0x110D0, 0x110FF, { 0x110D0, EcContinent::ASIA },
            "Sora Sompeng", EcScript::Sora },
    // Chakma OK
    { 0x11100, 0x1114F, { 0x11110, EcContinent::ASIA },
            "Chakma", EcScript::Cakm },
    // Mahajani OK
    { 0x11150, 0x1117F, { 0x11158, EcContinent::ASIA },
            "Mahajani", EcScript::Mahj },
    // Sharada OK
    { 0x11180, 0x111DF, { 0x11191, EcContinent::ASIA },
            "Sharada", EcScript::Shrd },
    // Sinhala archaic OK
    // Noto Sinhala is light but archaic numbers are bold!
    { 0x111E0, 0x111FF, { 0x111E1, EcContinent::OCEAN },
            "Sinhala Archaic Numbers",
            EcScript::Sinh, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Khojki OK
    { 0x11200, 0x1124F, { 0x11208, EcContinent::ASIA },
            "Khojki", EcScript::Khoj },
    // Multani OK
    { 0x11280, 0x112AF, { 0x11284, EcContinent::ASIA },
            "Multani", EcScript::Mult },
    // Khuda OK
    { 0x112B0, 0x112FF, { 0x112BA, EcContinent::ASIA },
            "Khudawadi", EcScript::Sind },
    // Grantha OK
    { 0x11300, 0x1137F, { 0x11315, EcContinent::ASIA },
            "Grantha", EcScript::Gran },
    // Newa OK
    { 0x11400, 0x1147F, { 0x1140E, EcContinent::ASIA },
            "Newa", EcScript::Newa },
    // Tirhuta OK
    { 0x11480, 0x114DF, { 0x1148F, EcContinent::ASIA },
            "Tirhuta", EcScript::Tirh },
    // Siddham OK
    { 0x11580, 0x115FF, { 0x1158E, EcContinent::ASIA },
            "Siddham", EcScript::Sidd },
    // Modi OK
    { 0x11600, 0x1165F, { 0x1160E, EcContinent::ASIA },
            "Modi", EcScript::Modi },
    // Mongolian supp OK
    { 0x11660, 0x1167F, { 0x11668, EcContinent::ASIA, Ifg::APPROX_ROTATED },
            "Mongolian Supplement",
            EcScript::Mong, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Takri OK
    { 0x11680, 0x116CF, { 0x1168A, EcContinent::ASIA },
            "Takri", EcScript::Takr },
    // Ahom OK
    { 0x11700, 0x1174F, { 0x11700, EcContinent::ASIA },
            "Ahom", EcScript::Ahom },
    // Dogra OK
    { 0x11800, 0x1184F, { 0x1180A, EcContinent::ASIA },
            "Dogra", EcScript::Dogr },
    // Warang OK
    { 0x118A0, 0x118FF, { 0x118FF, EcContinent::ASIA },
            "Warang Citi", EcScript::Wara },
    // Dives Akuru OK
    { 0x11900, 0x1195F, { 0x1190D, EcContinent::OCEAN },
            "Dives Akuru", EcScript::Diak },
    // Nandi OK
    { 0x119A0, 0x119FF, { 0x119B4, EcContinent::ASIA },
            "Nandinagari", EcScript::Nand },
    // Zanabazar square OK
    { 0x11A00, 0x11A4F, { 0x11A0B, EcContinent::ASIA },
            "Zanabazar Square", EcScript::Zanb },
    // Soyombo OK
    { 0x11A50, 0x11AAF, { 0x11A5C, EcContinent::ASIA },
            "Soyombo", EcScript::Soyo },
    // Canadian A OK
    { 0x11AB0, 0x11ABF, { 0x11AB0, EcContinent::AMERICA },
            "Unified Canadian Aboriginal Syllabics Extended-A",
            EcScript::Cans, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Pau Cin Hau OK
    { 0x11AC0, 0x11AFF, { 0x11AC0, EcContinent::ASIA },
            "Pau Cin Hau", EcScript::Pauc },
    /// @todo [U15] Devadagari Ex A
    { 0x11B00, 0x11B5F, { '?', EcContinent::MISSING, Ifg::CONTINENT_OK },
            "Devanagari Extended-A",
            EcScript::Deva, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Bhaiksuki OK
    { 0x11C00, 0x11C6F, { 0x11C0E, EcContinent::ASIA },
            "Bhaiksuki", EcScript::Bhks },
    /// @todo [semi-tofu] #85 Marchen too small because of subjoining
    { 0x11C70, 0x11CBF, { 0x11C74, EcContinent::ASIA },
            "Marchen", EcScript::Marc },
    { 0x11D00, 0x11D5F, { 0x11D0C, EcContinent::ASIA },
            "Masaram Gondi", EcScript::Gonm },
    { 0x11D60, 0x11DAF, { 0x11D7B, EcContinent::ASIA },
            "Gunjala Gondi", EcScript::Gong },
    // Makasar OK
    { 0x11EE0, 0x11EFF, { 0x11EE4, EcContinent::OCEAN },
            "Makasar", EcScript::Maka },
    /// @todo [U15] Kawi
    { 0x11F00, 0x11F5F, { '?', EcContinent::MISSING, Ifg::CONTINENT_OK },
            "Kawi", EcScript::Kawi },
    // Lisu supplement OK, new font engine works
    { 0x11FB0, 0x11FBF, { 0x11FB0, EcContinent::ASIA },
            "Lisu Supplement",
            EcScript::Lisu, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Tamil supp OK
    { 0x11FC0, 0x11FFF, { 0x11FD8, EcContinent::ASIA },
            "Tamil Supplement",
            EcScript::Taml, EcFont::TAMIL_SUPPLEMENT, Bfg::HAS_DESCRIPTION },
    // Cuneiform OK
    { 0x12000, 0x123FF, { 0x1202D, EcContinent::ASIA },
            "Cuneiform", EcScript::Xsux },
    // Cuneiform nembers and punct OK
    { 0x12400, 0x1247F, { 0x1240A, EcContinent::ASIA },
            "Cuneiform Numbers and Punctuation",
            EcScript::Xsux, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Early cuneiform OK
    { 0x12480, 0x1254F, { 0x12525, EcContinent::ASIA },
            "Early Dynastic Cuneiform", EcScript::Xsux },
    // Cypro-Minoan OK
    { 0x12F90, 0x12FFF, { 0x12FCC, EcContinent::EUROPE, Ifg::APPROX_HISTORICAL },
            "Cypro-Minoan", EcScript::Cpmn },
    // Egyptian hiero OK
    { 0x13000, 0x1342F, { 0x1302F, EcContinent::AFRICA },
            "Egyptian Hieroglyphs", EcScript::Egyp },
    // Egyptian hiero format OK
    { 0x13430, 0x1345F, { 0x13434, EcContinent::AFRICA },
            "Egyptian Hieroglyph Format Controls",
            EcScript::Egyp, EcFont::FUNKY, Bfg::HAS_DESCRIPTION },
    // Anatolian hiero OK
    { 0x14400, 0x1467F, { 0x1446E, EcContinent::ASIA },
            "Anatolian Hieroglyphs", EcScript::Hluw },
    // Bamum supplement OK
    { 0x16800, 0x16A3F, { 0x16812, EcContinent::AFRICA },
            "Bamum Supplement",
            EcScript::Bamu, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Mro OK
    { 0x16A40, 0x16A6F, { 0x16A52, EcContinent::ASIA },
            "Mro", EcScript::Mroo },
    // Tangsa OK, found the author
    { 0x16A70, 0x16ACF, { 0x16A86, EcContinent::ASIA },
            "Tangsa", EcScript::Tnsa },
    // Bassa OK
    { 0x16AD0, 0x16AFF, { 0x16AD0, EcContinent::AFRICA },
            "Bassa Vah", EcScript::Bass },
    // Pahawh OK
    { 0x16B00, 0x16B8F, { 0x16B00, EcContinent::ASIA },
            "Pahawh Hmong", EcScript::Hmng },
    // Medefaidrin OK
    { 0x16E40, 0x16E9F, { 0x16E60, EcContinent::AFRICA },
            "Medefaidrin", EcScript::Medf },
    // Miao OK
    { 0x16F00, 0x16F9F, { 0x16F03, EcContinent::ASIA },
            "Miao", EcScript::Plrd },
    // Ideographic sym/punct OK
    { 0x16FE0, 0x16FFF, { 0x16FF0, EcContinent::CJK },
            "Ideographic Symbols and Punctuation",
            // Hard block, though small, but all chars have scripts.
            // Hani is just over-insurance.
            EcScript::Hani, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Tangut OK
    { 0x17000, 0x187FF, { 0x17032, EcContinent::CJK, Ifg::APPROX_HISTORICAL },
            "Tangut",
            EcScript::Tang, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // Tangut components OK
    { 0x18800, 0x18AFF, { 0x18844, EcContinent::CJK, Ifg::APPROX_HISTORICAL },
            "Tangut Components",
            EcScript::Tang, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Khitan small OK
    { 0x18B00, 0x18CFF, { 0x18B4C, EcContinent::CJK, Ifg::APPROX_HISTORICAL },
            "Khitan Small Script",
            EcScript::Kits, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // Tangut supplement OK, not collapsible: too small
    { 0x18D00, 0x18D8F, { 0x18D00, EcContinent::CJK, Ifg::APPROX_HISTORICAL },
            "Tangut Supplement",
            EcScript::Tang, EcFont::NORMAL, Bfg::CJK | Bfg::HAS_DESCRIPTION },
    // Kana ex B OK: Made for myself tofu from GlyphWiki
    { 0x1AFF0, 0x1AFFF, { 0x1AFFB, EcContinent::CJK },
            "Kana Extended-B",
            EcScript::Kana, EcFont::NORMAL, Bfg::CJK | Bfg::HAS_DESCRIPTION },
    // Kana supp OK
    { 0x1B000, 0x1B0FF, { 0x1B046, EcContinent::CJK },
            "Kana Supplement",
            EcScript::Hent, EcFont::KOREAN, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Kana ex A OK
    { 0x1B100, 0x1B12F, { 0x1B10E, EcContinent::CJK },
            "Kana Extended-A",
            EcScript::Hent, EcFont::KOREAN, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Small kana ex OK
    { 0x1B130, 0x1B16F, { 0x1B150, EcContinent::CJK },
            "Small Kana Extension",
            EcScript::Kana, EcFont::KOREAN, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Nushu OK
    { 0x1B170, 0x1B2FF, { 0x1B1E0, EcContinent::CJK },
            "Nushu",
            EcScript::Nshu, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // Duployan OK
    { 0x1BC00, 0x1BC9F, { 0x1BC22, EcContinent::EUROPE },
            "Duployan", EcScript::Dupl },
    // Shorthand format controls OK
    { 0x1BCA0, 0x1BCAF, { 0x1BCA3, EcContinent::NONE },
            "Shorthand Format Controls",
            EcScript::NONE, EcFont::DUPLOYAN, Bfg::SCRIPTLIKE | Bfg::HAS_DESCRIPTION },
    // Znamenny OK, found cool font on Ponomar
    { 0x1CF00, 0x1CFCF, { 0x1CF50, EcContinent::EUROPE, Ifg::CONTINENT_OK },
            "Znamenny Musical Notation",
            EcScript::NONE, EcFont::ZNAMENNY },
    // Byzantine music seemingly OK
    { 0x1D000, 0x1D0FF, { 0x1D035, EcContinent::EUROPE, Ifg::CONTINENT_OK },
            "Byzantine Musical Symbols",
            EcScript::NONE, EcFont::MUSIC_BIGGER  },
    // Music OK
    { 0x1D100, 0x1D1FF, { 0x1D161, EcContinent::NONE },
            "Musical Symbols",
            EcScript::NONE, EcFont::MUSIC },
    // Greek music OK
    { 0x1D200, 0x1D24F, { 0x1D200, EcContinent::EUROPE },
            "Ancient Greek Musical Notation",
            EcScript::Grek, EcFont::MUSIC_NORMAL, Bfg::HAS_DESCRIPTION },
    // Kaktovik numerals OK, implemented in Funky
    { 0x1D2C0, 0x1D2DF, { '?', EcContinent::MISSING, Ifg::CONTINENT_OK },
            "Kaktovik Numerals",
            EcScript::NONE, EcFont::FUNKY },
    // Mayan numerals OK
    { 0x1D2E0, 0x1D2FF, { 0x1D2ED, EcContinent::AMERICA, Ifg::CONTINENT_OK },
            "Mayan Numerals",
            EcScript::NONE, EcFont::NOTO_SYMBOL2 },
    // Tai Xuan Jing OK
    { 0x1D300, 0x1D35F, { 0x1D329, EcContinent::CJK, Ifg::CONTINENT_OK },
            "Tai Xuan Jing Symbols" },
    // Counting rods OK
    { 0x1D360, 0x1D37F, { 0x1D378, EcContinent::NONE },
            "Counting Rod Numerals",
            EcScript::NONE, EcFont::NOTO_SYMBOL2_BIGGER },
    // Math alnum OK
    { 0x1D400, 0x1D7FF, { 0x1D4E0, EcContinent::NONE },
            "Mathematical Alphanumeric Symbols",
            EcScript::NONE, EcFont::MATH_CAMBRIA },
    // Sutton SignWriting OK
    { 0x1D800, 0x1DAAF, { 0x1D8A0, EcContinent::NONE },
            "Sutton SignWriting", EcScript::Sgnw },
    // Latin ex G OK, drew in FunkySample
    { 0x1DF00, 0x1DFFF, { 0x1DF04, EcContinent::EUROPE },
            "Latin Extended-G",
            EcScript::Latn, EcFont::FUNKY, Bfg::HAS_DESCRIPTION },
    // Glagolitic supp OK
    { 0x1E000, 0x1E02F, { 0x1E000, EcContinent::EUROPE },
            "Glagolitic Supplement",
            EcScript::Glag, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    /// @todo [U15] Cyr ex D
    { 0x1E030, 0x1E08F, { '?', EcContinent::MISSING, Ifg::CONTINENT_OK },
            "Cyrillic Extended-D",
            EcScript::Cyrl, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // NP Hmong OK
    { 0x1E100, 0x1E14F, { 0x1E118, EcContinent::AMERICA },
            "Nyiakeng Puachue Hmong", EcScript::Hmnp },
    // Toto OK
    { 0x1E290, 0x1E2BF, { 0x1E290, EcContinent::ASIA },
            "Toto", EcScript::Toto },
    // Wancho OK
    { 0x1E2C0, 0x1E2FF, { 0x1E2C0, EcContinent::ASIA },
            "Wancho", EcScript::Wcho },
    /// @todo [U15] Nag alphabet (Mundari)
    { 0x1E4D0, 0x1E4FF, { '?', EcContinent::MISSING, Ifg::CONTINENT_OK },
            "Nag Mundari", EcScript::Nagm },
    // Ethiopic ex B OK, Noto quickly arrived
    { 0x1E7E0, 0x1E7FF, { 0x1E7FB, EcContinent::AFRICA },
            "Ethiopic Extended-B",
            EcScript::Ethi, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Mende Kikakui OK
    { 0x1E800, 0x1E8DF, { 0x1E800, EcContinent::AFRICA },
            "Mende Kikakui", EcScript::Mend },
    // Adlam OK
    { 0x1E900, 0x1E95F, { 0x1E902, EcContinent::AFRICA },
            "Adlam", EcScript::Adlm },
    // Indic Siyaq OK
    { 0x1EC70, 0x1ECBF, { 0x1EC77, EcContinent::ASIA, Ifg::CONTINENT_OK },
            "Indic Siyaq Numbers",
            EcScript::NONE, EcFont::SIYAQ_INDIC },
    // Ottoman Siyaq OK, drew in FunkySample
    { 0x1ED00, 0x1ED4F, { 0x1ED0A, EcContinent::ASIA, Ifg::CONTINENT_OK },
            "Ottoman Siyaq Numbers",
            EcScript::NONE, EcFont::FUNKY },
    // Arab mathematical OK, currently we use Noto Math
    { 0x1EE00, 0x1EEFF, { 0x1EEA5, EcContinent::ASIA },
            "Arabic Mathematical Alphabetic Symbols",
            EcScript::Arab, EcFont::MATH, Bfg::HAS_DESCRIPTION },
    // Mahjong tiles OK
    { 0x1F000, 0x1F02F, { 0x1F022, EcContinent::NONE, Ifg::APPROX_COLOR },
            "Mahjong Tiles",
            EcScript::NONE, EcFont::PLAYING_CARDS },
    // Domino tiles OK
    { 0x1F030, 0x1F09F, { 0x1F043, EcContinent::NONE },
            "Domino Tiles",
            // Fonts coincide
            EcScript::NONE, EcFont::PHAISTOS_DISC },
    // Cards OK
    { 0x1F0A0, 0x1F0FF, { 0x1F0B1, EcContinent::NONE, Ifg::APPROX_COLOR },
            "Playing Cards",
            // Font coincides with Phaistos Disc, with one difference:
            // One emoji here → colour emoji font
            EcScript::NONE, EcFont::PLAYING_CARDS },
    // Enclosed alnum OK, added a few symbols to FunkySample
    { 0x1F100, 0x1F1FF, { 0x1F19B, EcContinent::NONE },
            "Enclosed Alphanumeric Supplement",
            EcScript::NONE, EcFont::DINGBAT, Bfg::SCRIPTLIKE },
    // Enclosed hiero OK
    { 0x1F200, 0x1F2FF, { 0x1F202, EcContinent::CJK, Ifg::CONTINENT_OK | Ifg::APPROX_COLOR },
            "Enclosed Ideographic Supplement",
            EcScript::NONE, EcFont::DINGBAT },
    // Misc OK
    { 0x1F300, 0x1F5FF, { 0x1F52B, EcContinent::NONE },
            "Miscellaneous Symbols and Pictographs" },
    // Emoticons OK
    { 0x1F600, 0x1F64F, { 0x1F60D, EcContinent::NONE },
            "Emoticons",
            EcScript::NONE, EcFont::DINGBAT },
    // Orna dingbats OK
    { 0x1F650, 0x1F67F, { 0x1F650, EcContinent::NONE },
            "Ornamental Dingbats",
            EcScript::NONE, EcFont::NOTO_SYMBOL2 },
    // Transport/map OK
    { 0x1F680, 0x1F6FF, { 0x1F697, EcContinent::NONE },
            "Transport and Map Symbols",
            EcScript::NONE, EcFont::DINGBAT },
    // Alchem OK
    { 0x1F700, 0x1F77F, { 0x1F708, EcContinent::NONE },
            "Alchemical Symbols" },
    // Geo shapes ex OK
    { 0x1F780, 0x1F7FF, { 0x1F7B0, EcContinent::NONE },
            "Geometric Shapes Extended",
            EcScript::NONE, EcFont::DINGBAT },
    // Arrows C OK
    { 0x1F800, 0x1F8FF, { 0x1F882, EcContinent::NONE },
            "Supplemental Arrows-C" },
    // Supp sym/picto OK
    { 0x1F900, 0x1F9FF, { 0x1F98A, EcContinent::NONE },
            "Supplemental Symbols and Pictographs",
            EcScript::NONE, EcFont::DINGBAT },
    // Chess OK, turned bad pawn
    { 0x1FA00, 0x1FA6F, { 0x1FA10, EcContinent::NONE },
            "Chess Symbols" },
    // Sym and picto ext A OK
    { 0x1FA70, 0x1FAFF, { 0x1FA82, EcContinent::NONE },
            "Symbols and Pictographs Extended-A",
            EcScript::NONE, EcFont::DINGBAT },
    // Legacy OK
    { 0x1FB00, 0x1FBFF, { 0x1FBB2, EcContinent::NONE, Ifg::APPROX_2_CHARS },
            "Symbols for Legacy Computing",
            EcScript::NONE, EcFont::NOTO_SYMBOL2_BIGGER },
    // CJK B OK
    { 0x20000, 0x2A6DF, { 0x20024, EcContinent::CJK },
            "CJK Unified Ideographs Extension B",
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // CJK C OK
    { 0x2A700, 0x2B73F, { 0x2A704, EcContinent::CJK },
            "CJK Unified Ideographs Extension C",
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // CJK D OK
    { 0x2B740, 0x2B81F, { 0x2B742, EcContinent::CJK },
            "CJK Unified Ideographs Extension D",
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // CJK E OK
    // Remove 2nd horizontal line from above from 2B283, and you’ll get
    //   common 4E1C “east”. But that’s probably feature, not bug
    { 0x2B820, 0x2CEAF, { 0x2B823, EcContinent::CJK },
            "CJK Unified Ideographs Extension E",
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // CJK F OK
    { 0x2CEB0, 0x2EBEF, { 0x2CEB5, EcContinent::CJK },
            "CJK Unified Ideographs Extension F",
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // CJK compat OK
    { 0x2F800, 0x2FA1F, { 0x2F81F, EcContinent::CJK },
            "CJK Compatibility Ideographs Supplement",
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // CJK G OK, found a HanaMin version supporting CJK G
    { 0x30000, 0x3134F, { 0x3000C, EcContinent::CJK },
            "CJK Unified Ideographs Extension G",
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    /// @todo [U15] CJK H
    { 0x31350, 0x323AF, { '?', EcContinent::MISSING, Ifg::CONTINENT_OK },
            "CJK Unified Ideographs Extension H",
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Tags OK
    { 0xE0000, 0xE007F, { 0xE0054, EcContinent::NONE, Ifg::APPROX_COLLECTIVE },
            "Tags",
            EcScript::NONE, EcFont::NORMAL, Bfg::SCRIPTLIKE },
    // Var sel supp OK
    { 0xE0100, 0xE01EF, { 0xE0100, EcContinent::NONE, Ifg::APPROX_COLLECTIVE },
            "Variation Selectors Supplement" },
};
static_assert(std::size(uc::blocks) == uc::N_BLOCKS,
              "Sorry, need to set block data manually");

constinit const uc::TermCat uc::termCats[] {
    { "TermCat.Encoding",      SortTerms::YES, },
    { "TermCat.Serialization", SortTerms::NO   },
    { "TermCat.Scripts",       SortTerms::YES  },
    { "TermCat.Chars",         SortTerms::YES  },
    { "TermCat.Algos",         SortTerms::YES  },
    { "TermCat.General",       SortTerms::YES  },
    { "TermCat.Styles",        SortTerms::YES  },
    { "TermCat.Input",         SortTerms::NO   },
    { "TermCat.Misc",          SortTerms::YES  },
};
static_assert (std::size(uc::termCats) == static_cast<size_t>(uc::EcTermCat::NN));

constinit const uc::Term uc::terms[] {
    /// @todo [windows] draw buttons
    { "winemoji", EcTermCat::INPUT, {} },
    { "altcode", EcTermCat::INPUT, u8"Alt codes" },
    { "appentry", EcTermCat::INPUT, {} },
    { "birman", EcTermCat::INPUT, {} },

    { "hiero", EcTermCat::SCRIPT_CLASS, u8"ideographic script" },
    { "abjad", EcTermCat::SCRIPT_CLASS, u8"consonant script, abjad" },
    { "syllabic", EcTermCat::SCRIPT_CLASS, u8"syllabic script" },
    { "abugida", EcTermCat::SCRIPT_CLASS, u8"abugida" },
    { "brahmic", EcTermCat::SCRIPT_CLASS, u8"Brahmic abugida, Indic script" },
    { "alphabet", EcTermCat::SCRIPT_CLASS, u8"alphabet script" },
    { "arguable", EcTermCat::SCRIPT_CLASS, {} },
    { "cjk", EcTermCat::SCRIPT_CLASS, u8"CJK = Chinese, Japanese, Korean" },
    { "code", EcTermCat::SCRIPT_CLASS, u8"codes and shorthand" },
    { "nontext", EcTermCat::SCRIPT_CLASS, u8"non-text notation" },

    { "acrophonic", EcTermCat::CHAR_CLASS, u8"acrophonic character" },
    { "virama", EcTermCat::CHAR_CLASS, u8"virama" },
    { "graphic", EcTermCat::CHAR_CLASS, u8"graphic character, printable character" },
    { "deprecated", EcTermCat::CHAR_CLASS, u8"deprecated character" },
    { "ignorable", EcTermCat::CHAR_CLASS, u8"default-ignorable character" },
    { "combining", EcTermCat::CHAR_CLASS, u8"combining mark" },
    { "ligature", EcTermCat::CHAR_CLASS, u8"ligature" },
    { "precomposed", EcTermCat::CHAR_CLASS, u8"precomposed character" },
    { "space", EcTermCat::CHAR_CLASS, u8"whitespace", { "CharCat.Zs.Text" } },
    { "spacing", EcTermCat::CHAR_CLASS, u8"spacing character" },
    { "pseudographics", EcTermCat::CHAR_CLASS, u8"pseudographics, semigraphics, box drawing" },
    { "control", EcTermCat::CHAR_CLASS, u8"control character", { "CharCat.Cc.Text" } },
    { "obsolete", EcTermCat::CHAR_CLASS, u8"obsolete character" },
    { "format", EcTermCat::CHAR_CLASS, u8"format character", { "CharCat.Cf.Text" } },
    { "number", EcTermCat::CHAR_CLASS, u8"number character" },
    { "emoji", EcTermCat::CHAR_CLASS, u8"emoji" },

    { "stability", EcTermCat::PRINCIPLES, u8"stability guarantees" },
    { "dynamic", EcTermCat::PRINCIPLES, u8"dynamic composition" },
    { "logical", EcTermCat::PRINCIPLES, u8"logical order" },
    { "convertibility", EcTermCat::PRINCIPLES, u8"convertibility" },
    { "plaintext", EcTermCat::PRINCIPLES, u8"plain text" },
    { "semantics", EcTermCat::PRINCIPLES, u8"semantics" },
    { "character", EcTermCat::PRINCIPLES, u8"characters, not glyphs" },
    { "universality", EcTermCat::PRINCIPLES, u8"universality" },
    { "unification", EcTermCat::PRINCIPLES, u8"unification" },
    { "efficiency", EcTermCat::PRINCIPLES, u8"efficiency" },

    { "ascii", EcTermCat::ENCODING, u8"ASCII", { "Block.0000.Text" } },
    { "bmp", EcTermCat::ENCODING, u8"BMP = Basic Multilingual Plane" },
    { "noncharacter", EcTermCat::ENCODING, u8"non-character" },
    { "codepoint", EcTermCat::ENCODING, u8"code point" },
    { "latin1", EcTermCat::ENCODING, u8"Latin-1", { "Block.0080.Text" } },
    { "plane", EcTermCat::ENCODING, u8"plane" },
    { "private", EcTermCat::ENCODING, u8"private-use character" },

    { "bidi", EcTermCat::ALGORITHM, u8"bidirectional text" },
    { "normalization", EcTermCat::ALGORITHM, u8"normalization" },
    { "casefolding", EcTermCat::ALGORITHM, u8"case folding" },

    { "serialization", EcTermCat::SERIALIZATION, u8"serialization" },
    { "punycode", EcTermCat::SERIALIZATION, u8"Punycode", },
    { "utf7", EcTermCat::SERIALIZATION, u8"UTF-7" },
    { "utf8", EcTermCat::SERIALIZATION, u8"UTF-8" },
    { "utf16", EcTermCat::SERIALIZATION, u8"UTF-16, surrogate pair" },
    { "utf32", EcTermCat::SERIALIZATION, u8"UTF-32" },
    { "bom", EcTermCat::SERIALIZATION, u8"BOM = byte order mark" },

    { "cursive", EcTermCat::WRITING_STYLE, u8"cursive" },
    { "minuscule", EcTermCat::WRITING_STYLE, u8"minuscule writing" },

    { "abc", EcTermCat::OTHER, u8"collation" },
    { "mono", EcTermCat::OTHER, u8"character cell" },
    { "inclusion", EcTermCat::OTHER, u8"inclusion and diversity" },
    { "tofu", EcTermCat::OTHER, u8"tofu" },
};

constinit const uc::Term* uc::sortedTerms[std::size(terms)] { nullptr };

size_t uc::nTerms() { return std::size(terms); }


const uc::Continent uc::continentInfo[] {
    // None
    { { Qt::white, Qt::black } },
    // Europe
    { { 0x7fffff, 0x204a87 } },
    // Asia
    { { 0xffff83, 0xf57900 } },
    // Ocean
    { { 0xc8ffc8, 0x006400 } },
    // Africa
    { { 0xf5deb3, 0x8f5902 } },
    // America
    { { 0xffc8c8, 0xcc0000 } },
    // CJK
    { { 0xfff0f5, 0x5c3566 } },
    // Missing
    { { 0xa40000, 0xFFFFFF } },
};
static_assert(std::size(uc::continentInfo) == static_cast<int>(uc::EcContinent::NN));

constinit const uc::OldCompInfo uc::oldCompInfo[] {
    { u8"Amstrad" NBSP "CPC" },
    { u8"Apple" NBSP "IIc+" },
    { u8"Mattel" NBSP "Aquaruis" },
    { u8"Atari" NBSP "ST" },
    { u8"Atari" NBSP "400/800" },
    { u8"Minitel" },
    { u8"MSX" },
    { u8"Oric" },
    { u8"Commodore" NBSP "PET/64" },
    { u8"RISC OS" },
    { u8"Sinclair" NBSP "ZX80/81" },
    { {}, "Prop.OldComp.Teletext" },
    { u8"Tandy" NBSP "TRS80" NBSP "Mark" NBSP "1" },
    { u8"Tandy" NBSP "TRS80" NBSP "Color" },
    { {}, "Prop.OldComp.Korvet" },
};

std::u8string uc::OldCompInfo::locName() const
{
    if (locKey.empty()) {
        return std::u8string{fixedName};
    } else {
        std::u8string r = loc::get(locKey);
        str::replace(r, u8" ", u8"" NBSP);
        return r;
    }
}

#define REP_2(x) x,x
#define REP_3(x) x,x,x
#define REP_4(x) x,x,x,x
#define REP_5(x) x,x,x,x,x
#define REP_6(x) x,x,x,x,x,x
#define REP_7(x) x,x,x,x,x,x,x
#define REP_8(x) REP_4(x),REP_4(x)
#define REP_10(x) REP_4(x),REP_4(x),x,x
#define REP_11(x) REP_4(x),REP_4(x),REP_3(x)
#define REP_12(x) REP_4(x),REP_4(x),REP_4(x)
#define REP_13(x) REP_12(x),x
#define REP_16(x) REP_4(x),REP_4(x),REP_4(x),REP_4(x)
#define REP_32(x) REP_16(x),REP_16(x)
#define REP_48(x) REP_16(x),REP_16(x),REP_16(x)
#define REP_60(x) REP_48(x),REP_12(x)

namespace {

    using OC = uc::OldComp;

    constinit Flags<uc::OldComp> oldCompData[] {
        REP_60 (OC::AQUARIUS | OC::TANDY_COCO | OC::MINITEL | OC::ORIC | OC::TELETEXT | OC::TANDY_TRS80 | OC::KORVET ), // 00..3B 6block
        REP_48 (OC::TELETEXT ),                                                 // 3C..6B diagonals and braided flags
        REP_4  (OC::MSX | OC::TELETEXT ),                                       // 6C..6F triangles
                OC::COMMODORE | OC::TELETEXT,                                   // 70 col2
        REP_4  (OC::COMMODORE),                                                 // 71..74 col3..6
                OC::COMMODORE | OC::TELETEXT,                                   // 75 col7
        REP_6  (OC::COMMODORE),                                                 // 76..7B line2..6
                OC::APPLE | OC::COMMODORE,                                      // 7C LB corner
        REP_3  (OC::COMMODORE),                                                 // 7D..7F rest 3 corners
                OC::APPLE,                                                      // 80 lines18
                OC::APPLE,                                                      // 81 lines1358
                OC::MSX | OC::COMMODORE,                                        // 82 lines12
        REP_2  (OC::COMMODORE),                                                 // 83,84 lines123,1-5
                OC::MSX | OC::COMMODORE,                                        // 85 lines1-6
                OC::COMMODORE,                                                  // 86 lines1-7
                OC::MSX | OC::COMMODORE,                                        // 87 cols78
        REP_2  (OC::COMMODORE),                                                 // 88,89 cols678,4-8
                OC::MSX | OC::COMMODORE | OC::TANDY_TRS80,                      // 8A cols3-8
                OC::COMMODORE,                                                  // 8B cols2-8
                                        // 16 chars skipped by Consortium
                OC::AMSTRAD | OC::AQUARIUS | OC::COMMODORE,                     // 8C left50%
                OC::AMSTRAD | OC::AQUARIUS,                                     // 8D right50%
                OC::AMSTRAD | OC::AQUARIUS | OC::SINCLAIR,                      // 8E top50%
                OC::AMSTRAD | OC::AQUARIUS | OC::COMMODORE | OC::SINCLAIR,      // 8F bot50%
                OC::APPLE | OC::COMMODORE | OC::SINCLAIR,                       // 90 all50%
                OC::COMMODORE | OC::SINCLAIR,                                   // 91 top100%+bot50%
                OC::SINCLAIR,                                                   // 92 top50%+bot100%
                {},                                                             // 93 hole
                OC::COMMODORE,                                                  // 94 left50%+right100%
                OC::MSX | OC::COMMODORE,                                        // 95 checker1
                OC::AMSTRAD | OC::COMMODORE,                                    // 96 checker2
                OC::TANDY_TRS80,                                                // 97 heavy=
        REP_2  (OC::MSX | OC::COMMODORE),                                       // 98,99 fill\ and /
        REP_2  (OC::MSX),                                                       // 9A,9B butterflies
        REP_4  (OC::AMSTRAD),                                                   // 9C..9F diag50%
        REP_8  (OC::AMSTRAD | OC::MINITEL | OC::TELETEXT),                      // A0..A3 quarter / or \…
                                                                                // A4..A7 two such slashes nearby
        REP_2  (OC::AMSTRAD | OC::MINITEL),                                     // A8,A9 two such slashes opposite
        REP_4  (OC::MINITEL),                                                   // AA..AD three such slashes
                OC::AMSTRAD | OC::MINITEL,                                      // AE four such slashes
                OC::MSX,                                                        // AF full -, incomplete |
                OC::APPLE,                                                      // B0 mouse cursor
                OC::APPLE | OC::COMMODORE,                                      // B1 inverse check
        REP_11 (OC::APPLE),                                                     // B2..BC misc Apple chars
        REP_3  (OC::ATARI_ST),                                                  // BD..BF inverse saltires
                OC::RISC_OS | OC::TANDY_TRS80,                                  // C0 white saltire
        REP_4  (OC::TANDY_TRS80),                                               // C1..C3 pointing finger
                                                                                // C4 inverse ?
                OC::AMSTRAD | OC::TANDY_TRS80,                                  // C5 man
        REP_3  (OC::AMSTRAD),                                                   // C6..C8 three men
        REP_2  (OC::TANDY_TRS80),                                               // C9 woman
                                                                                // CA chevron
        REP_5  ({}),                                                            // CB..CF unused
        REP_32 ({}),                                                            // D0..EF unused
        REP_10 (OC::ATARI_ST)                                                   // F0..F9 7seg digits
    };

    static_assert(std::size(oldCompData) == 0xFA);                              // FA first free

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

    constinit const ReverseMap rmWin {{
        { u'‚', 130 }, { u'ƒ', 131 }, { u'„', 132 }, { u'…', 133 },
        { u'†', 134 }, { u'‡', 135 }, { u'ˆ', 136 }, { u'‰', 137 },
        { u'Š', 138 }, { u'‹', 139 }, { u'Œ', 140 }, { u'Ž', 142 },
        { u'‘', 145 }, { u'’', 146 }, { u'“', 147 }, { u'”', 148 },
        { u'•', 149 }, { u'–', 150 }, { u'—', 151 }, { u'˜', 152 },
        { u'™', 153 }, { u'š', 154 }, { u'›', 155 }, { u'œ', 156 },
        { u'ž', 158 }, { u'Ÿ', 159 }
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
            r.sometimesKey = u8"Enter, Shift/Ctrl/Alt+Enter";
            break;
        case 9:
            r.sometimesKey = u8"Tab";
            break;
        default:;
        }
    } else if (cp >= 0xA0 && cp < 0x10000) {  // Rest of BMP
        rmDosCommon.query(cp, r.alt.dosCommon);
        rmDosEn.query(cp, r.alt.dosEn);
        rmDosRu.query(cp, r.alt.dosRu);
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
    cp -= 0x1FB00;
    if (cp >= std::size(oldCompData))
        return {};
    return oldCompData[cp];
}

///// Sutton SignWriting ///////////////////////////////////////////////////////

namespace {

    constexpr int SHIFT_FILL = 0;
    constexpr int SHIFT_FILLPLUS = SHIFT_FILL + uc::SwInfo::N_FILL;
    constexpr int SHIFT_ROT = SHIFT_FILLPLUS;
    constexpr int SHIFT_ROTPLUS = SHIFT_ROT + uc::SwInfo::N_ROT;
    constexpr int SHIFT_MISC = SHIFT_ROTPLUS;
    enum class Sw {
        F1  = 1 << (SHIFT_FILL + 0),
        F2  = 1 << (SHIFT_FILL + 1),
        F3  = 1 << (SHIFT_FILL + 2),
        F4  = 1 << (SHIFT_FILL + 3),
        F5  = 1 << (SHIFT_FILL + 4),
        F6  = 1 << (SHIFT_FILL + 5),
        F_TO_2 = F1 | F2,
        F_TO_3 = F1 | F2 | F3,
        F_TO_4 = F1 | F2 | F3 | F4,
        F_TO_5 = F1 | F2 | F3 | F4 | F5,
        F_ALL = F1 | F2 | F3 | F4 | F5 | F6,
        R1  = 1 << (SHIFT_ROT + 0),
        R2  = 1 << (SHIFT_ROT + 1),
        R3  = 1 << (SHIFT_ROT + 2),
        R4  = 1 << (SHIFT_ROT + 3),
        R5  = 1 << (SHIFT_ROT + 4),
        R6  = 1 << (SHIFT_ROT + 5),
        R7  = 1 << (SHIFT_ROT + 6),
        R8  = 1 << (SHIFT_ROT + 7),
        R9  = 1 << (SHIFT_ROT + 8),
        R10 = 1 << (SHIFT_ROT + 9),
        R11 = 1 << (SHIFT_ROT + 10),
        R12 = 1 << (SHIFT_ROT + 11),
        R13 = 1 << (SHIFT_ROT + 12),
        R14 = 1 << (SHIFT_ROT + 13),
        R15 = 1 << (SHIFT_ROT + 14),
        R16 = 1 << (SHIFT_ROT + 15),
        R_TO_2 = R1 | R2,
        R_TO_4 = R1 | R2 | R3 | R4,
        R_TO_6 = R1 | R2 | R3 | R4 | R5 | R6,
        R_TO_8 = R1 | R2 | R3 | R4 | R5 | R6 | R7 | R8,
        R_TO_9 = R1 | R2 | R3 | R4 | R5 | R6 | R7 | R8 | R9,
        R_ALL = ((1 << uc::SwInfo::N_ROT) - 1) << SHIFT_ROT,
        R_124568 = R1 | R2 | R4 | R5 | R6 | R8,
        R_XC_3_7_11_15 = R_ALL & ~(R3 | R7 | R11 | R15),
        ALL = F_ALL | R_ALL,
        // Misc flags
        NO_TRANS    = 1 << (SHIFT_MISC + 0),
        FINEGRAINED = 1 << (SHIFT_MISC + 1),
        SIMPLE = F1 | R1 | NO_TRANS,
    };
    DEFINE_ENUM_OPS(Sw)

    constinit Flags<Sw> signWritingData[] {
        REP_16(Sw::ALL),            // 1D80*
        REP_16(Sw::ALL),            // 1D81*
        REP_16(Sw::ALL),            // 1D82*
        REP_16(Sw::ALL),            // 1D83*
        REP_13(Sw::ALL),            // 1D840..4C
          Sw::F2 | Sw::R_ALL,       // 1D84D
          Sw::ALL,                  // 1D84E
          Sw::F2 | Sw::R_ALL,       // 1D84F
        Sw::ALL,                    // 1D850
          Sw::F2 | Sw::R_ALL,       // 1D851
          REP_6(Sw::ALL),           // 1D852..57
          REP_3(Sw::ALL),           // 1D858..5A
          Sw::F_TO_4 | Sw::R_ALL,   // 1D85B
          Sw::F2 | Sw::R_ALL,       // 1D85C
          Sw::ALL,                  // 1D85D
          Sw::F2 | Sw::R_ALL,       // 1D85E
          Sw::ALL,                  // 1D85F
        REP_16(Sw::ALL),            // 1D86*
        REP_16(Sw::ALL),            // 1D87*
        REP_16(Sw::ALL),            // 1D88*
        REP_16(Sw::ALL),            // 1D89*
        REP_16(Sw::ALL),            // 1D8A*
        REP_16(Sw::ALL),            // 1D8B*
        REP_16(Sw::ALL),            // 1D8C*
        REP_16(Sw::ALL),            // 1D8D*
        REP_16(Sw::ALL),            // 1D8E*
        REP_6(Sw::ALL),             // 1D8F0..F5
          Sw::F2 | Sw::R_ALL,       // 1D8F6
          Sw::ALL,                  // 1D8F7
          REP_8(Sw::ALL),           // 1D8F8..FF
        REP_4(Sw::ALL),             // 1D900..03
          Sw::F2 | Sw::R_ALL,       // 1D904
          Sw::SIMPLE,               // 1D905
          Sw::F_TO_2 | Sw::R_TO_4,  // 1D906
          Sw::F_TO_2 | Sw::R_TO_4,  // 1D907
          Sw::SIMPLE,               // 1D908
          Sw::F_TO_2 | Sw::R_TO_4,  // 1D909
          Sw::F_TO_2 | Sw::R_TO_4,  // 1D90A
          Sw::SIMPLE,               // 1D90B
          Sw::F_TO_2 | Sw::R_TO_4,  // 1D90C
          Sw::F_TO_2 | Sw::R_TO_4,  // 1D90D
          Sw::SIMPLE,               // 1D90E
          Sw::F_TO_2 | Sw::R_TO_4,  // 1D90F
        Sw::F_TO_2 | Sw::R_TO_4,    // 1D910
          Sw::SIMPLE,               // 1D911
          Sw::F_TO_2 | Sw::R_TO_4,  // 1D912
          Sw::F_TO_2 | Sw::R_TO_4,  // 1D913
          Sw::F_TO_2 | Sw::R_TO_8,  // 1D914
          Sw::F_TO_2 | Sw::R_TO_4,  // 1D915
          Sw::SIMPLE,               // 1D916
          Sw::SIMPLE,               // 1D917
          Sw::F_TO_2 | Sw::R_TO_4,  // 1D918
          Sw::F_TO_2 | Sw::R_TO_4,  // 1D919
          Sw::ALL,                  // 1D91A
          Sw::SIMPLE,               // 1D91B
          Sw::SIMPLE,               // 1D91C
          Sw::F_TO_2 | Sw::R_TO_4,  // 1D91D
          Sw::F_TO_2 | Sw::R_TO_4,  // 1D91E
          Sw::ALL,                  // 1D91F
        Sw::F_TO_2 | Sw::R_TO_8,    // 1D920
          REP_2(Sw::F_TO_5 | Sw::R_TO_8), // 1D921,22
          REP_2(Sw::ALL),           // 1D923,24
          REP_3(Sw::F_TO_4 | Sw::R_TO_8), // 1D925..27
          REP_2(Sw::F_TO_5 | Sw::R_TO_8), // 1D928,29
          REP_5(Sw::F_TO_4 | Sw::R_TO_8), // 1D92A..2E
          Sw::F_TO_3 | Sw::R_TO_8,  // 1D92F
        Sw::F_TO_3 | Sw::R_TO_8,    // 1D930
          REP_3(Sw::F_TO_3 | Sw::R_ALL), // 1D931..33
          REP_2(Sw::F_TO_3 | Sw::R_TO_8), // 1D934,35
          REP_2(Sw::F_TO_3 | Sw::R_ALL), // 1D936,37
          REP_8(Sw::F_TO_4 | Sw::R_ALL), // 1D938..3F
        REP_11(Sw::F_TO_4 | Sw::R_ALL), // 1D940..4A
          REP_3(Sw::ALL),           // 1D94B..4D
          REP_2(Sw::F_ALL | Sw::R_XC_3_7_11_15), // 1D94E,4F
        Sw::F_ALL | Sw::R_XC_3_7_11_15, // 1D950
          Sw::F_TO_3 | Sw::R_TO_8,  // 1D951
          REP_3(Sw::F_TO_3 | Sw::R_ALL), // 1D952..54
          REP_3(Sw::F_TO_5 | Sw::R_124568), // 1D955..57
          REP_5(Sw::F_TO_5 | Sw::R_124568), // 1D958..5C
          REP_3(Sw::F_TO_4 | Sw::R_124568), // 1D95D..5F
        REP_5(Sw::F_TO_4 | Sw::R_124568), // 1D960..64
          REP_3(Sw::F_TO_4 | Sw::R_TO_8), // 1D965..67
          REP_2(Sw::F_TO_4 | Sw::R_TO_8), // 1D968,69
          REP_2(Sw::F_TO_3 | Sw::R_TO_8), // 1D96A,6B
          REP_3(Sw::F_TO_3 | Sw::R_ALL), // 1D96C..6E
          Sw::F_TO_3 | Sw::R_TO_8,  // 1D96F
        Sw::F_TO_3 | Sw::R_TO_8,    // 1D970
          REP_2(Sw::F_TO_3 | Sw::R_ALL), // 1D971,72
          REP_5(Sw::F_TO_4 | Sw::R_ALL), // 1D973..77
          REP_8(Sw::F_TO_4 | Sw::R_ALL), // 1D978..7F
        Sw::F_TO_4 | Sw::R_ALL,     // 1D980
          REP_6(Sw::ALL),           // 1D981..86
          Sw::F_TO_3 | Sw::R_TO_8,  // 1D987
          REP_8(Sw::F_TO_4 | Sw::R_ALL), // 1D988..8F
        REP_16(Sw::F_TO_4 | Sw::R_ALL), // 1D99*
        REP_2(Sw::F_TO_4 | Sw::R_ALL),  // 1D9A0,A1
          REP_3(Sw::ALL),           // 1D9A2..A4
          Sw::F_TO_5 | Sw::R_ALL,   // 1D9A5
          REP_2(Sw::F_TO_3 | Sw::R_TO_4), // 1D9A6,A7
          REP_8(Sw::F_TO_3 | Sw::R_TO_4), // 1D9A8..AF
        REP_4(Sw::F_TO_3 | Sw::R_TO_4), // 1D9B0..B3
          REP_3(Sw::F_TO_3 | Sw::R_XC_3_7_11_15), // 1D9B4..B6
          Sw::F_ALL | Sw::R_TO_8,   // 1D9B7
          Sw::F_ALL | Sw::R_TO_8,   // 1D9B8
          REP_7(Sw::F_TO_4 | Sw::R_TO_8), // 1D9B9..BF
        REP_3(Sw::F_TO_4 | Sw::R_TO_8), // 1D9C0..C2
          REP_5(Sw::F_ALL | Sw::R_TO_8), // 1D9C3..C7
          REP_8(Sw::F_TO_4 | Sw::R_TO_8), // 1D9C8..CF
        REP_2(Sw::F_TO_4 | Sw::R_TO_8), // 1D9D0,D1
          REP_3(Sw::F_ALL | Sw::R_TO_8), // 1D9D2..D4
          REP_3(Sw::F_TO_4 | Sw::R_ALL), // 1D9D5..D7
          REP_7(Sw::F_TO_4 | Sw::R_ALL), // 1D9D8..DE
          Sw::ALL,                  // 1D9DF
        REP_2(Sw::ALL),             // 1D9E0,E1
          Sw::F_TO_5 | Sw::R_ALL,   // 1D9E2
          REP_4(Sw::F_TO_3 | Sw::R_ALL), // 1D9E3..E6
          Sw::ALL,                  // 1D9E7
          REP_5(Sw::ALL),           // 1D9E8..EC
          REP_2(Sw::F_TO_3 | Sw::R_ALL), // 1D9ED,EE
          Sw::F_TO_3 | Sw::R_TO_6,  // 1D9EF
        Sw::F_TO_3 | Sw::R_TO_6,    // 1D9F0
          REP_4(Sw::F_TO_2 | Sw::R_TO_8), // 1D9F1..F4
          REP_2(Sw::F_ALL | Sw::R_TO_8), // 1D9F5,F6
          Sw::F_TO_4 | Sw::R1,      // 1D9F7
          Sw::F1 | Sw::R_TO_8,      // 1D9F8
          REP_2(Sw::F_TO_4 | Sw::R1), // 1D9F9,FA
          REP_4(Sw::F1 | Sw::R_TO_8), // 1D9FB..FE
          Sw::F_TO_4 | Sw::R_TO_4,  // 1D9FF
        Sw::F1 | Sw::R_TO_8,        // 1DA00
          Sw::F_ALL | Sw::R_TO_8,   // 1DA01
          Sw::F_ALL | Sw::R_TO_2,   // 1DA02
          Sw::F_ALL | Sw::R_TO_8,   // 1DA03
          REP_3(Sw::F_TO_2 | Sw::R_TO_4), // 1DA04..06
          Sw::F_ALL | Sw::R_TO_2,   // 1DA07
          REP_2(Sw::F_TO_3 | Sw::R_ALL), // 1DA08,09
          REP_6(Sw::F_ALL | Sw::R1), // 1DA0A..0F
        Sw::F_ALL | Sw::R1,         // 1DA10
          REP_3(Sw::F_TO_2 | Sw::R1), // 1DA11..13
          REP_4(Sw::F_TO_5 | Sw::R1), // 1DA14..17
          REP_8(Sw::F_TO_5 | Sw::R1), // 1DA18..1F
        Sw::F_TO_5 | Sw::R1,        // 1DA20
          REP_6(Sw::F_TO_4 | Sw::R_TO_8), // 1DA21..26
          Sw::F_TO_3 | Sw::R_ALL,   // 1DA27
          REP_2(Sw::F_TO_3 | Sw::R_ALL), // 1DA28,29
          REP_3(Sw::F_TO_5 | Sw::R1), // 1DA2A..2C
          REP_3(Sw::F_TO_4 | Sw::R1), // 1DA2D..2F
        Sw::F_TO_5 | Sw::R1,        // 1DA30
          REP_4(Sw::F_TO_2 | Sw::R1), // 1DA31..34
          REP_2(Sw::F_ALL | Sw::R1), // 1DA35,36
          REP_2(Sw::F_TO_2 | Sw::R_TO_8), // 1DA37,38
          REP_2(Sw::F_ALL | Sw::R1), // 1DA39,3A
          REP_5(Sw::F_TO_2 | Sw::R1), // 1DA3B..3F
        REP_16(Sw::F_TO_2 | Sw::R1), // 1DA4*
        REP_6(Sw::F_TO_2 | Sw::R1), // 1DA50..55
          REP_3(Sw::F_ALL | Sw::R1), // 1DA56..58
          REP_5(Sw::F_TO_2 | Sw::R_TO_8), // 1DA59..5D
          Sw::F_TO_4 | Sw::R_TO_8 | Sw::FINEGRAINED, // 1DA5E
          Sw::F_ALL | Sw::R1,       // 1DA5F
        Sw::F_TO_4 | Sw::R1,        // 1DA60
          REP_7(Sw::F_ALL | Sw::R1), // 1DA61..67
          REP_2(Sw::F_TO_3 | Sw::R_TO_8), // 1DA68,69
          Sw::F_ALL | Sw::R1,       // 1DA6A
          REP_2(Sw::F_TO_4 | Sw::R1), // 1DA6B,6C
          Sw::F_TO_3 | Sw::R_TO_4,  // 1DA6D
          Sw::F_TO_5 | Sw::R_TO_6,  // 1DA6E
          Sw::ALL,                  // 1DA6F
        REP_2(Sw::ALL),             // 1DA70,71
          REP_3(Sw::F_TO_4 | Sw::R_TO_4), // 1DA72..74
          Sw::F_ALL | Sw::R_TO_8,   // 1DA75
          Sw::F1 | Sw::R_ALL,       // 1DA76
          REP_7(Sw::F_TO_2 | Sw::R_ALL), // 1DA77..7D
          Sw::ALL,                  // 1DA7E
          Sw::F_TO_4 | Sw::R_TO_8,  // 1DA7F
        Sw::F_TO_4 | Sw::R_TO_8,    // 1DA80
          Sw::F_TO_2 | Sw::R_TO_8,  // 1DA81
          Sw::F1 | Sw::R_TO_9,      // 1DA82
          Sw::F1 | Sw::R_TO_8,      // 1DA83
          Sw::F_ALL | Sw::R_TO_8,   // 1DA84
          Sw::F_TO_5 | Sw::R_TO_8,  // 1DA85
          Sw::ALL,                  // 1DA86
          REP_2(Sw::F_TO_4 | Sw::R_TO_8), // 1DA87,88
          REP_3(Sw::F1 | Sw::R_TO_8), // 1DA89,8B
    };

    static_assert(std::size(signWritingData) == 0x28C);

}   // anon namespace


uc::SwInfo uc::SwInfo::get(char32_t cp)
{
    auto index = cp - 0x1D800;
    uc::SwInfo r;
    if (index < std::size(signWritingData)) {
        // OK, we are here!
        r.flags = signWritingData[index].numeric();
        r.fCp = cp;
    }
    return r;
}


bool uc::SwInfo::hasFill0(int i) const
{
    if (i >= uc::SwInfo::N_FILL)
        return false;
    return flags & (static_cast<int>(Sw::F1) << i);
}


bool uc::SwInfo::hasRot0(int i) const
{
    if (i >= uc::SwInfo::N_ROT)
        return false;
    return flags & (static_cast<int>(Sw::R1) << i);
}


bool uc::SwInfo::isSimple() const
{
    return Flags<Sw>(flags).have(Sw::NO_TRANS);
}


char32_t uc::SwInfo::baseChar(int fill, int rot) const
{
    if (!Flags<Sw>(flags).have(Sw::FINEGRAINED))
        return 0;
    switch (fCp) {
    case 0x1DA5E:
        if (fill == 1 && rot < 5)
            return 0x1D9FF;     // head
        return 0;
    default: return 0;
    }
}


std::u8string_view uc::SwInfo::note() const
{
    if (!Flags<Sw>(flags).have(Sw::FINEGRAINED))
        return {};
    switch (fCp) {
    case 0x1DA5E:
        return loc::get("Prop.Sutton.01");
    default: return {};
    }
}


///// UC misc //////////////////////////////////////////////////////////////////

namespace {
    [[maybe_unused]] std::u8string cache;
    [[maybe_unused]] inline std::string_view esc(std::u8string_view x)
    {
        return str::toSv(escape::cppSv(
                        x, cache, 'n', escape::Spaces::YES, Enquote::NO));
    }
}


void uc::completeData()
{
    // Fill CP → block mapping
    std::fill(std::begin(blocksByCode16), std::end(blocksByCode16), -1);
    for (int iBlock = 0; iBlock < N_BLOCKS; ++iBlock) {
        auto& block = blocks[iBlock];
        auto i16 = block.startingCp >> 4;
        auto end16 = block.endingCp >> 4;
        for (; i16 <= end16; ++i16)
            blocksByCode16[i16] = iBlock;
    }

    // Fill CP info
    std::fill(std::begin(cpsByCode), std::end(cpsByCode), nullptr);
    for (auto& cp : cpInfo) {
        // Bidi class
        ++cp.bidiClass().nChars;
        // Category
        ++cp.category().nChars;
        // Script
        auto& script = cp.script();
        ++script.nChars;
        script.ecVersion = std::min(script.ecVersion, cp.ecVersion);
        if (script.plane == PLANE_UNKNOWN)
            script.plane = cp.plane();
        // Block
        auto block = blockOf(cp.subj);
        if (!block->firstAllocated)
            block->firstAllocated = &cp;
        block->lastAllocated = &cp;
        ++block->nChars;
        block->ecVersion = std::min(block->ecVersion, cp.ecVersion);
        block->ecLastVersion = std::max(block->ecLastVersion, cp.ecVersion);
        // Lookup table
        cpsByCode[cp.subj.val()] = &cp;
    }

    // Check blocks — they should have at least one char
    for (auto& v : blocks) {
        if (!v.firstAllocated || !v.lastAllocated)
            throw std::logic_error("Block w/o chars leaked into data!");

        // Check synthesized icon
        if (v.synthIcon.ecContinent != EcContinent::MISSING
                && (v.synthIcon.subj < v.startingCp || v.synthIcon.subj > v.endingCp))
            throw std::logic_error(
                    "Synthesized icon is not within block " + std::string{v.name});
        if (!cpsByCode[v.synthIcon.subj])
            throw std::logic_error("Synthesized icon points to bad char!");
        if (!v.synthIcon.flags.have(Ifg::CONTINENT_OK)) {
            auto& script = v.script();
            if (script.ecContinent != v.synthIcon.ecContinent) {
                std::cout << "Continent mismatch: " << v.name << std::endl;
            }
        }
    }

    // Save term INI
//    std::ofstream os("term.ini");
//    for (auto& v : terms) {
//        os << "[Term." << v.key << "]\n";
//        os << "Name=" << esc(v.locName) << '\n';
//        os << "Text=" << esc(v.locDesc) << '\n';
//    }
}


struct uc::LoadedFont
{
    QList<QString> families;
    QString familiesComma;
    intptr_t tempId = FONT_NOT_INSTALLED;
    std::unique_ptr<QFont> probe {}, normal {};
    std::unique_ptr<QFontMetrics> probeMetrics;
    std::unique_ptr<QRawFont> rawFont;

    const QString& onlyFamily() const;
    const QFont& get(
            std::unique_ptr<QFont>& font,
            QFont::StyleStrategy strategy,
            Flags<uc::Ffg> flags) const;
};


const QString& uc::LoadedFont::onlyFamily() const
{
    return (families.size() == 1) ? families[0] : uc::Font::qempty;
}


const QFont& uc::LoadedFont::get(
        std::unique_ptr<QFont>& font,
        QFont::StyleStrategy strategy,
        Flags<uc::Ffg> flags) const
{
    if (!font) {
        auto& family1 = onlyFamily();
        font.reset(new QFont(family1, 50, QFont::Normal));
        if (family1.isEmpty())
            font->setFamilies(families);
        // Weight
        if (flags.have(Ffg::BOLD)) {
            font->setBold(true);
        } else if (flags.have(Ffg::SEMIBOLD)) {
            font->setWeight(QFont::DemiBold);
        } else if (flags.have(Ffg::LIGHT)) {
            font->setWeight(QFont::Light);
        }
        font->setStyleStrategy(strategy);
    }
    return *font;
}


namespace {

    QList<QString> toQList(std::string_view s)
    {
        auto strs = str::splitSv(s, ',');
        QList<QString> r;
        r.reserve(strs.size());
        for (auto& v : strs) {
            r.emplaceBack(str::toQ(v));
        }
        return r;
    }

    std::unordered_map<std::string_view, std::shared_ptr<uc::LoadedFont>> loadedFonts;

    bool isFontFname(std::string_view name)
    {
        return name.ends_with(".ttf"sv)
            || name.ends_with(".ttc"sv)
            || name.ends_with(".otf"sv);
    }

    void preloadFont(uc::EcFont font)
    {
        uc::fontInfo[static_cast<int>(font)].load(0);
    }

    bool wantPreloadFonts = true;

    ///
    /// @brief preloadFonts
    ///   Preloads fonts for unglitching
    ///   (we somehow depend on font loading order)
    /// @return [+] preload occured
    ///
    bool preloadFonts()
    {
        bool r = wantPreloadFonts;
        if (r) {
            // Prevent from triggering again…
            wantPreloadFonts = false;
            // …and recursively preload, here are fonts
            preloadFont(uc::EcFont::NOTO_SYMBOL2);
        }
        return r;
    }

}   // anon namespace


void uc::Font::newLoadedStruc() const
{
    auto newLoaded = std::make_shared<LoadedFont>();
    loadedFonts[family.text] = newLoaded;
    q.loaded = newLoaded;
}


void uc::Font::load(char32_t trigger) const
{
onceAgain:
    // Loaded?
    if (q.loaded)
        return;

    // Find in cache
    if (auto it = loadedFonts.find(family.text); it != loadedFonts.end()) {
        q.loaded = it->second;
        return;
    }

    // Create/load it
    if (isFontFname(family.text)) {
        // FILE
        if (preloadFonts())     // File → preload other files
            goto onceAgain;
        newLoadedStruc();

        auto tempFont = installTempFontRel(family.text, trigger);
        q.loaded->tempId = tempFont.id;
        q.loaded->familiesComma = tempFont.families.join(',');
        q.loaded->families = std::move(tempFont.families);

        if (family.flags.have(Fafg::RAW_FONT) && tempFont.mems) {
            q.loaded->rawFont = std::make_unique<QRawFont>(tempFont.mems->qdata(), 50);
        }
    } else {
        // FAMILY
        newLoadedStruc();
        q.loaded->familiesComma = str::toQ(family.text);
        q.loaded->families = toQList(family.text);
    }

    // Make probe font
    q.loaded->get(q.loaded->probe,  fst::TOFU,   flags);
    q.loaded->get(q.loaded->normal, fst::DEFAULT, flags);
        // force EXACT match
    q.loaded->probeMetrics = std::make_unique<QFontMetrics>(*q.loaded->probe);
    doesSupportChar(trigger);
}


QFont uc::Font::get(FontPlace place, int size, bool noAa, char32_t trigger) const
{
    load(trigger);
    QFont font = *q.loaded->normal;
    font.setPointSize(computeSize(place, size));
    if (place == FontPlace::CELL && noAa) {
        font.setStyleStrategy(fst::NO_AA);
    }
    return font;
}


const QString& uc::Font::familiesComma(char32_t trigger) const
{
    load(trigger);
    return q.loaded->familiesComma;
}


bool uc::Font::doesSupportChar(char32_t subj) const
{
    // First load, then check version: if YES, we’ll need this char;
    //                                 if NO, check using probeMetrics
    load(subj);
    if (q.loaded->rawFont) {
        return q.loaded->rawFont->supportsCharacter(subj);
    } else {
        return q.loaded->probeMetrics->inFontUcs4(subj);
    }
}

int uc::Font::computeSize(FontPlace place, int size) const
{
    auto r = sizeAdjust.apply(size);
    switch (place) {
    case FontPlace::CELL:
        if (flags.have(Ffg::CELL_SMALLER))
            r = r * 9 / 10;     // =90%
        if (flags.have(Ffg::CELL_BIGGER))
            r = r * 23 / 20;    // =115%
        break;
    case FontPlace::SAMPLE:
    case FontPlace::PROBE: break;
    }
    return r;
}


QString uc::Cp::viewableName() const
{
    QString r = name.tech();
    if (auto pos = r.indexOf('#'); pos >= 0) {
        char buf[30];
        snprintf(buf, std::size(buf), "%04X", static_cast<unsigned>(subj.uval()));
        r.replace(pos, 1, buf);
    }
    return r;
}



std::u8string_view uc::Cp::abbrev() const
{
    if (!isAbbreviated())
        return {};
    const char8_t* x = name.tech();
    // +1
    std::u8string_view sv = x;
    x += (sv.length());  ++ x;
    return x;
}


uc::SampleProxy uc::Cp::sampleProxy(int dpi) const
{
    switch (drawMethod(dpi)) {
    case DrawMethod::SAMPLE:
        break;  // go through
    case DrawMethod::CUSTOM_AA:
        if (dpi == DPI_ALL_CHARS)
            break;
        [[fallthrough]];
    default:
        return {};
    }

    auto fn = font(uc::MatchLast::NO);
    auto style = fn->styleSheet;
    auto code = subj.ch32();

    if (fn->flags.have(Ffg::STUB_FINEGRAINED)) {
        // Special stubs char-by char, enabled by STUB_FINEGRAINED flag
        switch (code) {
        case 0x1BC9E:   // Duployan shorthand
            return { NBSP + str::toQ(code) + NBSP + NBSP + NBSP + NBSP, style };
        case 0x1CF42:   // Znamenny
        case 0x1CF43:
        case 0x1CF44:
        case 0x1CF45:
        case 0x1CF46:
            return { str::toQ(code), style };
        }
    }

    switch (ecCategory) {
    case EcCategory::MARK_ENCLOSING:
    case EcCategory::MARK_NONSPACING:
        // Stub vice versa?
        if (fn->flags.have(Ffg::STUB_VICEVERSA)) {
            return { ZWSP + str::toQ(code) + STUB_CIRCLE, style };
        }
        [[fallthrough]];
    case EcCategory::MARK_SPACING:
        // Stub off?
        if (fn->flags.have(Ffg::STUB_OFF))
            break;
        if (fn->flags.have(Ffg::STUB_RTL))
            return { str::toQ(STUB_RTL_CIRCLE) + str::toQ(code), style };
        return { STUB_CIRCLE + str::toQ(code), style };
    case EcCategory::SEPARATOR_SPACE:
        if (isTrueSpace()) {
            return { QChar(L'▕') + str::toQ(code) + QChar(L'▏'), style };
        }
        break;
    default: ;
    }
    if (fn->flags.have(Ffg::STUB_INTERCHAR))
        return { str::toQ(code) + str::toQ(code), style };
    return { str::toQ(code), style };
}


bool uc::Cp::isGraphical() const
{
    return static_cast<bool>(category().isGraphical);
}


uc::DrawMethod uc::Cp::drawMethod(int dpi) const
{
    if (flags.haveAny(Cfg::M_ALL)) [[unlikely]] {
        if (flags.have(Cfg::M_CUSTOM_CONTROL))
            return uc::DrawMethod::CUSTOM_CONTROL;
        if (flags.have(Cfg::M_SVG_EMOJI))
            return (block().flags.have(Bfg::NO_EMOJI))
                    ? uc::DrawMethod::SAMPLE
                    : uc::DrawMethod::SVG_EMOJI;
        if (flags.have(Cfg::M_SPACE))
            return uc::DrawMethod::SPACE;
        if (flags.have(Cfg::M_EGYPTIAN_HATCH))
            return uc::DrawMethod::EGYPTIAN_HATCH;
        if (isAbbreviated())
            return uc::DrawMethod::ABBREVIATION;
    }
    if (isTrueSpace())
        return uc::DrawMethod::SPACE;
    if (dpi < 130 && script().font().flags.have(Ffg::CUSTOM_AA))
        return uc::DrawMethod::CUSTOM_AA;
    return uc::DrawMethod::SAMPLE;
}


QString uc::Cp::osProxy() const
{
    if (!isGraphical())
        return {};

    switch (category().upCat) {
    case UpCategory::MARK:
        return STUB_CIRCLE + str::toQ(subj.ch32());
    default: ;
    }
    return str::toQ(subj.ch32());
}


uc::EcScript uc::Cp::ecScriptEx() const
{
    if (ecScript != EcScript::NONE && ecScript != EcScript::Zinh)
        return ecScript;
    return block().ecScript;
}


const uc::Font& uc::Cp::firstFont() const
{
    // Priority: block → script — block’s script
    auto& blk = block();
    // Block
    if (blk.flags.have(Bfg::UNGLITCH_MARKS) && category().upCat == UpCategory::MARK) {
        // All enclosing marks are in Funky now,
        // and the rest are still in Noto
        if (ecCategory == EcCategory::MARK_ENCLOSING)
            return fontInfo[static_cast<int>(EcFont::FUNKY)];
        return fontInfo[static_cast<int>(EcFont::NOTO)];
    }
    auto hfont = blk.ecFont;
    if (hfont != EcFont::NORMAL || blk.flags.have(Bfg::FORCE_FONT)) {
        return fontInfo[static_cast<int>(hfont)];
    }
    // Script
    if (ecScript != EcScript::NONE && ecScript != EcScript::Zinh) {
        auto& si = scriptInfo[static_cast<int>(ecScript)];
        if (si.ecFont != EcFont::NORMAL)
            return si.font();
    }
    // Block’s script
    return blk.script().font();
}


const uc::Font* uc::Cp::font(MatchLast matchLast) const
{
    if (subj.ch32() == 0xFFFFFF) {
        std::cout << "Debug here!" << std::endl;
    }
    auto v = &firstFont();
    bool isBuggy = flags.have(Cfg::RENDER_BUG);
    auto sb = subj.ch32();
    while (v->flags.have(Ffg::FALL_TO_NEXT)) {
        if (isBuggy
                ? !v->flags.have(Ffg::BUG_AVOID)        // BUGGY: avoid flag → bad, it’s for normal only
                : !v->flags.have(Ffg::BUG_FIXUP)) {     // NORMAL: fixup flag → bad, it’s for buggy only
            if (v->doesSupportChar(sb))
                return v;
        }
        ++v;
    }
    if (matchLast != MatchLast::NO) {
        if (!v->doesSupportChar(sb))
            return nullptr;
    }
    return v;
}


uc::TofuInfo uc::Cp::tofuInfo() const
{
    uc::TofuInfo r;
    r.block = &block();
    if (r.block->flags.haveAny(Bfg::COLLAPSIBLE | Bfg::CJK)
            || script().ecContinent == EcContinent::CJK)
        r.place = TofuPlace::CJK;

    static constexpr auto DPI_DUMMY = 96;
    if (drawMethod(DPI_DUMMY) > uc::DrawMethod::LAST_FONT) {
        r.state = TofuState::NO_FONT;
    } else {
        r.state = font(MatchLast::YES) ? TofuState::PRESENT : TofuState::TOFU;
    }
    return r;
}


SafeVector<std::u8string_view> uc::Cp::allRawNames() const
{
    SafeVector<std::u8string_view> r;
    r.reserve(name.alts + 1);
    std::u8string_view it = name.tech();
    r.emplace_back(it);
    for (auto n = name.alts; n != 0; --n) {
        it = std::to_address(it.end()) + 1;
        r.emplace_back(it);
    }
    return r;
}


const uc::Block* uc::blockOf(char32_t subj)
{
    unsigned index = subj >> 4;
    if (index >= std::size(blocksByCode16))
        return nullptr;
    auto q = blocksByCode16[index];
    return (q >= 0) ? &blocks[q] : nullptr;
}


size_t uc::Block::index() const
{
    return this - std::begin(blocks);
}


unsigned uc::Block::nNonChars() const
{
    unsigned r = 0;
    if (flags.have(Bfg::HAS_32_NONCHARS))
        r += 32;
    if ((endingCp & 0xFFFF) == 0xFFFF)
        r += 2;
    return r;
}


void uc::Block::printfLocKey(char* buf, size_t n, const char* suffix) const
{
    snprintf(buf, n, "Block.%04X.%s", (int)startingCp, suffix);
}


void uc::Script::printfLocKey(char* buf, size_t n, const char* suffix) const
{
    snprintf(buf, n, "Script.%s.%s", id.data(), suffix);
}


void uc::Category::printfLocKey(char* buf, size_t n, const char* suffix) const
{
    snprintf(buf, n, "CharCat.%s.%s", id.data(), suffix);
}

void uc::Term::printfLocKey(char* buf, size_t n, const char* suffix) const
{
    snprintf(buf, n, "Term.%s.%s", key.data(), suffix);
}

const uc::Term* uc::findTerm(std::string_view id)
{
    for (auto& v : terms) {
        if (v.key == id)
            return &v;
    }
    return nullptr;
}


QFont uc::funkyFont(FontPlace place, int size, char32_t trigger)
{
    auto& font = fontInfo[static_cast<int>(uc::EcFont::FUNKY)];
    return font.get(place, size, false, trigger);
}


namespace {

    bool isTermLess(const uc::Term* x, const uc::Term* y)
    {
        return std::lexicographical_compare(
                    std::begin(x->loc.sortKey), std::end(x->loc.sortKey),
                    std::begin(y->loc.sortKey), std::end(y->loc.sortKey));
    }

    void sortTerms(const uc::Term** beg, const uc::Term** end)
    {
        if ((*beg)->cat().sort != uc::SortTerms::NO) {
            std::stable_sort(beg, end, isTermLess);
        }
    }

}   // anon namespace


void uc::finishTranslation(const std::unordered_map<char32_t, int>& sortOrder)
{
    char c[40];
    for (auto& blk : blocks) {
        blk.printfLocKey(c, "Name");
        blk.loc.name = loc::get(c);

        if (blk.hasDescription()) {
            blk.printfLocKey(c, "Text");
            blk.loc.description = loc::get(c);
        }
    }

    for (auto& sc : scriptInfo) {
        sc.printfLocKey(c, "Name");
            sc.loc.name = loc::get(c);
        if (!sc.flags.have(Sfg::NO_LANGS)) {
            sc.printfLocKey(c, "Lang");
                sc.loc.langs = loc::get(c);
        }
        if (sc.time.needsCustomNote()) {
            sc.printfLocKey(c, "Note");
                sc.loc.timeComment = loc::get(c);
        }
        if (sc.flags.have(Sfg::DESC_FROM_PREV)) {
            auto p = &sc;
            --p;
            sc.loc.description = p->loc.description;
        } else {
            sc.printfLocKey(c, "Text");
                sc.loc.description = loc::get(c);
        }
    }


    for (auto& bidi : bidiClassInfo) {
        bidi.printfLocKey(c, "Name");
            bidi.loc.name = loc::get(c);
        bidi.printfLocKey(c, "Short");
            bidi.loc.shortName = loc::get(c);
        bidi.printfLocKey(c, "Text");
            bidi.loc.description = loc::get(c);
    }

    for (auto& cat : categoryInfo) {
        cat.printfLocKey(c, "Name");
            cat.loc.name = loc::get(c);
        cat.printfLocKey(c, "Text");
            cat.loc.description = loc::get(c);
    }

    auto pSorted = std::begin(sortedTerms);
    for (auto& term : terms) {
        // Get name
        term.printfLocKey(c, "Name");
        term.loc.name = loc::get(c);

        // Build sort key
        auto name32 = mojibake::toS<std::u32string>(term.loc.name);
        std::fill(std::begin(term.loc.sortKey), std::end(term.loc.sortKey), -1);
        size_t len = 0;
        for (auto v : name32) {
            auto it = sortOrder.find(v);
            if (it != sortOrder.end()) {
                term.loc.sortKey[len] = it->second;
                if ((++len) >= std::size(term.loc.sortKey))
                    break;
            }
        }

        // Get desc
        if (term.borrowedDesc.empty()) {
            term.printfLocKey(c, "Text");
            term.loc.description = loc::get(c);
        } else {
            term.loc.description = loc::get(term.borrowedDesc);
        }

        // Add to sorted
        *(pSorted++) = &term;
    }

    auto pBeg = std::begin(sortedTerms);
    auto oldCat = (*pBeg)->ecCat;
    for (auto& pt : sortedTerms) {
        auto newCat = pt->ecCat;
        if (newCat != oldCat) {
            sortTerms(pBeg, &pt);
            pBeg = &pt;
            oldCat = newCat;
        }
    }
    sortTerms(pBeg, std::end(sortedTerms));
}
