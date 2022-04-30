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
    { FAM_DEFAULT, Ffg::FALL_TO_NEXT },                                         // Normal
      { FNAME_FUNKY, Ffg::FALL_TO_NEXT },                                       // …1
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
      { "NotoSansEgyptianHieroglyphs-Regular.otf", Ffg::CELL_BIGGER },          // …1
    { "NotoSansElbasan-Regular.ttf" },                                          // Elbasan
    { "NotoSansElymaic-Regular.ttf" },                                          // Elymaic
    { "NotoSerifEthiopic-Regular.ttf", Ffg::DESC_BIGGER },                      // Ethiopic
    { "Sylfaen", Ffg::FALL_TO_NEXT },                                           // Georgian Nuskhuri
      { FNAME_DEJAVU, Ffg::FALL_TO_NEXT },                                      // …1 — FALL to Georgian
    { "NotoSerifGeorgian-Regular.ttf" },                                        // Georgian
    { "NotoSansGlagolitic-Regular.ttf" },                                       // Glagolitic
    { "NotoSansGrantha-Regular.ttf", Ffg::CELL_SMALLER,
            "padding-top:10%; padding-bottom:12%;"_sty },                       // Grantha
    { "NotoSerifGujarati-Regular.ttf", 110_pc },                                // Gujarati
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
    { "NotoSansLimbu-Regular.ttf" },                                            // Limbu
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
    { "NotoSansMyanmar-Regular.ttf" },                                          // Myanmar
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
    { "NotoSansOldTurkic-Regular.ttf" },                                        // Orkhon
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
        // Tibetan chars are VERY high → 90%
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
    { u8"живая ''(основным языкам ничего не угрожает, как кириллица)''" },
    { u8"в опасности{{-}}из-за языка ''(как чероки)''" },
    { u8"в опасности{{-}}из-за конкуренции письменностей ''(как ланна)''" },
    { u8"мёртвая ''(скоро потеряет/недавно потеряла носителей, как нюй-шу)''" },
    { u8"историческая ''(давно мертва, как глаголица)''" },
    { u8"религиозная ''(мертва в светском обороте, как коптская)''" },
    { u8"новая ''(появилась в новейшее время, как нко)''" },
    { u8"возрождённая ''(как ахом)''" },
    { u8"экспериментальная <i>(попытка придумать имеющемуся языку новую письменность, как мормонская)</i>" },
    //{ u8"Ошибка" },
};


constinit const uc::WritingDir uc::writingDirInfo[static_cast<int>(EcWritingDir::NN)] {
    { {} },
    { u8"→" },
    { u8"←" },
    { u8"→ <i>(исторически ←↓↓ по столбцам)</i>" },
    { u8"←↓↓ по столбцам, затем →" },
    { u8"→, ↑" },
    { u8"↓↓→ по столбцам" },
    { u8"↓" },
    { u8"писали и ←, и →" },
    { u8"писали ←↓↓ по столбцам и →" },
    { u8"←↓↓ по столбцам" },
    { u8"преимущественно ←" },
    { u8"↓↓→ по столбцам; ←" },
    //{ u8"Ошибка" },
};


constinit const uc::ScriptType uc::scriptTypeInfo[] {
    { {} },
    { u8"неизвестно" },
    { u8"[[pt:alphabet|алфавитная]]{{-}}''как кириллица''" },
    { u8"[[pt:arguable|спорный]]{{-}}''как тана''" },
    { u8"[[pt:abjad|консонантная]]{{-}}''как иврит''" },
    { u8"[[pt:syllabic|слоговая]]{{-}}''как японские катакана/хирагана''" },
    { u8"алфавитно-слоговая{{-}}''как иберский''" },
    { u8"[[pt:abugida|абугида]]{{-}}''как эфиопский''" },
    { u8"[[pt:brahmic|брахмийская абугида]]{{-}}''как деванагари''" },
    { u8"[[pt:hiero|иероглифическая]]{{-}}''как китайский''" },
    { u8"слогоиероглифическая{{-}}''как линейное письмо Б''" },
    { u8"консонантно-иероглифическое{{-}}''как египетские иероглифы''" },
    { u8"[[pt:code|коды и стенография]]{{-}}''как шрифт Брайля''" },
    { u8"[[pt:nontext|нетекстовая]]{{-}}''как ноты''" },
    //{ u8"Ошибка" },
};
static_assert (std::size(uc::scriptTypeInfo) == static_cast<int>(uc::EcScriptType::NN));


const uc::Version uc::versionInfo[] {
    { {},     0    },
    //{ "1.0",  1991 },
    //{ "1.0.1",1992 },
    { "1.1",  1993 },
    { "2.0",  1996 },
    { "2.1",  1998 },
    { "3.0",  1999 },
    { "3.1",  2001 },
    { "3.2",  2002 },
    { "4.0",  2003 },
    { "4.1",  2005 },
    { "5.0",  2006 },
    { "5.1",  2008 },
    { "5.2",  2009 },
    { "6.0",  2010 },
    { "6.1",  2012 },
    { "6.2",  2012 },
    { "6.3",  2013 },
    { "7.0",  2014 },
    { "8.0",  2015 },
    { "9.0",  2016 },
    { "10.0", 2017 },
    { "11.0", 2018 },
    { "12.0", 2019 },
    { "12.1", 2019 },
    { "13.0", 2020 },
    { "14.0", 2021 },
};
static_assert (std::size(uc::versionInfo) == static_cast<int>(uc::EcVersion::NN));


constexpr std::u8string_view CT_NOCHAR =
        u8"Выброшены (''non-characters'') 66 позиций: *FFFE и *FFFF во всех семнадцати плоскостях, "
                "а также FDD0…FDEF. Их никогда не{{_}}будет в Юникоде."
            "<p>Эти коды '''можно''' использовать как коды ошибок, знаки подстановки, «утяжелитель» при сортировке "
                    "или что-то другое, внутреннее для программы, перекодировать в [[pt:utf8|UTF-8]] или [[pt:utf16|UTF-16]]. "
                "Если программа никак не{{_}}использует данный код, она может обращаться с ним как с обычным неизвестным символом."
            "<p>Их '''нельзя''' вводить с клавиатуры, использовать в межпрограммном обмене. "
                "Благодаря тому, что символа FFFE нет в Юникоде, работает [[pt:bom|метка порядка байтов]].";

constinit const std::u8string_view uc::TX_NOCHAR = CT_NOCHAR;

constexpr std::u8string_view CT_CONTROL =
    u8"В первые годы, когда часто требовалось повторить сессию обмена вручную (перфоратором или консолью), "
                "управляющие символы играли роль форматов данных и сетевых протоколов. "
            "Клавиша ''Ctrl'' изначально служила для ввода управляющих символов. "
            "В настоящее время используются немногие из них:"
         "<p>• нулевой символ 00{{-}}конец строки в памяти;<br>"
            "• конец файла 04 (Unix) и 1A (DOS/Windows){{-}}вводятся в консоли как Ctrl+D/Ctrl+Z, чтобы закончить текст;<br>"
            "• звонок 07{{-}}терминал или принтер привлекает внимание оператора. "
                "Сейчас обычно через звуковую плату и без приостановки печати;<br>"
            "• шаг назад (забой) 08{{-}}перемещает каретку на шаг назад;<br>"
            "• табуляция 09{{-}}перемещает к позициям 9, 17, 25, 33… для набора таблиц;<br>"
            "• перевод строки 0A{{-}}конец строки в текстовом файле (Unix), проворот барабана на строку;<br>"
            "• прогон листа 0C{{-}}принтер выбрасывает лист;<br>"
            "• возврат каретки 0D. "
                "Отсюда конец строки в DOS/Windows 0D+0A{{-}}принтер возвращает каретку и проворачивает барабан;<br>"
            "• Escape 1B{{-}}исполнить команду принтера (шрифты, графика) или консоли (перемещение курсора, цвета)."
        "<p>Другие интересные команды:"
         "<p>• вертикальная табуляция 0B{{-}}прокрутить барабан к запрограммированной позиции. "
                "Делает быстрее и тише печать на готовых формулярах;<br>"
            "• вкл/выкл альтернативный регистр 0E/0F{{-}}другой цвет ленты, другой набор символов;<br>"
            "• экранирование управляющего 10{{-}}следующий символ [[pt:graphic|графический]], а не{{_}}управляющий;<br>"
            "• удаление 7F{{-}}исправление ошибки на перфоленте (пробивают все семь дорожек, такой символ не{{_}}подлежит считыванию). "
                "В некоторых терминалах эту же роль играл FF, из-за чего [[ps:Cyrl|русскую]] «я» заменяли заглавной "
                    "на файл-серверах, в играх жанра MUD."
        "<p>Все 65 символов из категории «Управляющие» исторические. "
            "Символы, влияющие на отображение текста, называются [[pt:format|форматирующими]].";

constexpr std::u8string_view CT_FORMAT =
    u8"Невидимый символ, использующийся для управления различными алгоритмами обработки текста:"
        "<p>• Разбивает текст на строки: 00AD{{-}}мягкий перенос."
        "<p>• Задаёт семантику текста: 2062{{-}}невидимый знак умножения в формуле, который можно перенести a×&nbsp;//&nbsp;×b, "
                "в отличие от невидимого вызова функции 2061."
        "<p>• Участвует в [[pt:ligature|лигатурах]] или, наоборот, разрывает их: 200D{{-}}нулевой соединитель."
        "<p>• Задаёт направление письма: 2066{{-}}изолировать участок, идущий слева направо."
        "<p>• Выполняет специфичные для письменности задачи, которые невозможно реализовать через шрифт: "
                "1D173{{-}}начать группу из нескольких нот, соединённых ребром.";

constexpr std::u8string_view CT_SPACE =
    u8"Символ, призванный оставить место. "
            "В хорошо проработанной типографике пустого места столько, сколько нужно: "
                "146%{{-}}мало, 146{{_}}%{{-}}много, {{%|146}}{{-}}самое то. "
            "Потому и пробелы бывают разные. "
            "Некоторые из них неразрывные: не{{_}}расширяются при выключке, не{{_}}переносятся на другую строку."
        "<p>Пробелы обычно никак не{{_}}отображаются и только передвигают курсор. "
            "Но если включено подчёркивание, или если письменность опирается на общую черту ([[ps:Ogam|огамическая]]), "
                "пробелы могут быть видны."
        "<p>В «Юникодии» пробелы обведены точно по габаритам кегельной площадки{{-}}"
            "так что узость обманчива. Часто для отступа хватает тончайшего пробела U+200A."
        "<p>Все пробелы являются [[pt:spacing|протяжёнными]] символами. "
            "Пробелы нулевой ширины, которые не{{_}}оставляют места, отнесены не к пробелам, а к [[pt:format|форматирующим]].";

constexpr std::u8string_view CT_ASCII =
    u8"'''American Standard Code for Information Interchange (ASCII)'''{{-}}разработанная в США в 1963 семибитная кодировка символов. "
            "Ширину в 7 бит выбрали как компромисс для передачи данных по телеграфу, "
                "к тому же ASCII легко обрезать до 6 бит, оставив символы 40…5F и 20…3F. "
            "'''Базовая латиница''' Юникода совпадает с ASCII{{-}}так что английский юникодный текст читается, даже если кодировка сбита."
        "<p>Первые 32 символа, 00…1F, [[pt:control|управляющие]], и сегодня немногие из них действительно используются. "
            "Затем идут 95 печатных символов. "
            "Многие из них с пишущих машинок: например, чтобы пропечатать À, нужно напечатать A, вернуть каретку "
                "и напечатать обратный апостроф. "
            "Последний символ, 7F=DEL, также управляющий, родом с перфолент{{-}}отверстия, пробитые во всех дорожках, "
                "говорят, что символ выбит ошибочно и не{{_}}подлежит считыванию."
        "<p>Существовало множество национальных вариантов ASCII: британцы заменяли решётку # на фунт £, "
                "СССР доллар $ на монету ¤, немцы [\\]{|} на умляуты ÄÖÜäöü, японцы обратную косую черту \\ на иену ¥…"
        "<p>Оставшиеся 128 символов могли заполняться чем угодно. Одна из таких 8-битных кодировок, [[pt:latin1|латиница-1]], "
                "также вошла в Юникод.";

constexpr std::u8string_view CT_LATIN1 =
        // French delegate refused: https://elementy.ru/problems/2689/Yunikod_na_Novyy_god
    u8"Латиница-1{{-}}второе название 8-битной кодировки '''ISO{{_}}8859-1''', появившейся в 1987 году "
                "и расширяющей [[pt:ascii|ASCII]] до европейских языков."
        "<p>Первые 32 символа, 80…9F, [[pt:control|управляющие]] для совместимости с 7-битными кодировками: "
                "почтовые программы иногда теряли верхний бит, и не{{_}}хотелось бы в этих позициях ставить печатные символы. "
            "Сейчас эти символы не{{_}}применяются по назначению и служат [[pt:private|личными]]. "
            "Так, кодировка Windows-1252{{-}}это та же латиница-1, только вместо управляющих символов типографские. "
            "Дошло до того, что в стандартах Всемирной паутины написано: если страница просит кодировку ISO{{_}}8859-1, "
                "отобразить сайт в Windows-1252."
        "<p>Среди букв видны символы × и ÷. "
            "Изначально там должен быть Œœ, но французский делегат настаивал, что хватает OE.";


constinit const uc::Category uc::categoryInfo[static_cast<int>(uc::EcCategory::NN)] {
    { UpCategory::CONTROL,      "Cc",   Graphical::NO,  u8"Управляющий", CT_CONTROL },
    { UpCategory::FORMAT,       "Cf",   Graphical::NO,  u8"Форматирующий", CT_FORMAT },
    // PRIVATE_USE,     -- unused as Unicodia has nothing to tell
    // SURROGATE,       -- unused as Unicodia has nothing to tell
    { UpCategory::LETTER,       "Ll",   Graphical::YES, u8"Буква/строчная",
            u8"Письмо «[[pt:minuscule|минускул]]», появившееся в раннее средневековье для экономии дорогого пергамента, "
                "превратилось в строчные буквы и сделало латиницу очень выразительным алфавитом." },
    { UpCategory::LETTER,       "Lm",   Graphical::YES, u8"Буква/модифицирующая",
            u8"Такие «недобуквы», приделывающие к букве оттенки смысла, чаще всего встречаются в фонетике. "
                u8"Так, модифицирующей буквой является штрих мягкости xʹ, в отличие от похожего математического штриха x′." },
    { UpCategory::LETTER,       "Lo",   Graphical::YES, u8"Буква/другая",
            u8"Символы различных письменностей, где нет деления на заглавные и строчные буквы. "
                "Например, [[ps:Hebr|иврит]], [[ps:Arab|арабская вязь]] и [[ps:Hani|китайские иероглифы]]. Также особые символы вроде "
                    "мужского/женского порядкового знака романских языков (1º{{-}}первый, 1ª{{-}}первая)." },
    { UpCategory::LETTER,       "Lt",   Graphical::YES, u8"Буква/смешанный регистр",
            u8"Символы-диграфы, состоящие из заглавной и строчной букв. "
                    "Так, в хорватской латинице есть диграф ǈ. "
                "Сербский аналог Љ не{{_}}является диграфом и потому смешанного регистра не{{_}}имеет." },
    { UpCategory::LETTER,       "Lu",   Graphical::YES, u8"Буква/заглавная",
            u8"Те письменности, которые приняли маленькие (строчные) буквы, буквы старой формы стали "
                "называть большими, заглавными или прописными." },
    { UpCategory::MARK,         "Mc",   Graphical::YES, u8"Метка/протяжённая",
            u8"Протяжённые (обладающие шириной) комбинирующие метки встречаются в некоторых "
                "языках Юго-Восточной Азии: деванагари, бенгальском, кáннаде, хангыле…" },
    { UpCategory::MARK,         "Me",   Graphical::YES, u8"Метка/охватывающая",
            u8"Охватывающие метки используются в древнерусских буквенных цифрах (А҈). "
                "Также существуют охватывающие квадрат, круг и другие фигуры. "
                "Ни один проверенный типографский движок на Windows 10 20H2 (GDI, Cairo, Skia) не{{_}}поддерживает подобные символы идеально, "
                    "но на хороших шрифтах вроде DejaVu результат очень неплох." },
    { UpCategory::MARK,         "Mn",   Graphical::YES, u8"Метка/непротяжённая",
            u8"Непротяжённые метки (например, знак ударе́ния) устроены как символы нулевой ширины, отсюда название. "
                "В хороших шрифтах дизайнер вручную помещает метки на наиболее распространённые буквы. "
                "Но если подобной комбинации не{{_}}предусмотрели (8́), символы накладываются как попало, и результат обычно плох." },
    { UpCategory::NUMBER,       "Nd",   Graphical::YES, u8"Числовой/десятичный",
            u8"Люди считают десятками, поскольку у них десять пальцев. Слова «цифра» и «палец» во многих языках близки. "
                "Изобретённая в Индии позиционная система счисления используется всем миром, однако "
                    "в языках Ближнего Востока и Юго-Восточной Азии цифры бывают причудливые." },
    { UpCategory::NUMBER,       "Nl",   Graphical::YES, u8"Числовой/буквенный",
            u8"Римские, китайские и другие цифры, основанные на буквах. Древнерусские также основаны на буквах, "
                    "но в Юникоде для этого используются обычные А, Б, Г… с комбинирующими метками." },
    { UpCategory::NUMBER,       "No",   Graphical::YES, u8"Числовой/другой",
            u8"Архаичные системы счисления, [[pt:precomposed|монолитные]] дроби, верхние и нижние индексы, цифры в кругах, "
                    "сокращения для больших чисел и другие цифровые символы." },
    { UpCategory::PUNCTUATION,  "Pc",   Graphical::YES, u8"Знак препинания/соединитель",
            u8"Небольшая группа символов. Наиболее известный из них{{-}}знак подчёркивания." },
    { UpCategory::PUNCTUATION,  "Pd",   Graphical::YES, u8"Знак препинания/черта",
            u8"Дефисы и тире. Минус также является чертой, но отнесён к математическим знакам." },
    { UpCategory::PUNCTUATION,  "Pe",   Graphical::YES, u8"Знак препинания/закрывающая скобка",
            u8"Скобки отнечены к отдельной категории, потому что играют важную роль в двунаправленном алгоритме." },
    { UpCategory::PUNCTUATION,  "Pf",   Graphical::YES, u8"Знак препинания/конечный",
            u8"В основном кавычки." },
    { UpCategory::PUNCTUATION,  "Pi",   Graphical::YES, u8"Знак препинания/начальный",
            u8"В основном кавычки." },
    { UpCategory::PUNCTUATION,  "Po",   Graphical::YES, u8"Знак препинания/другой",
            u8"Точка, запятая, процент, маркер списка и многое что ещё." },
    { UpCategory::PUNCTUATION,  "Ps",   Graphical::YES, u8"Знак препинания/открывающая скобка",
            u8"Скобки отнечены к отдельной категории, потому что играют важную роль в двунаправленном алгоритме." },
    { UpCategory::SYMBOL,       "Sc",   Graphical::YES, u8"Символ/валютный",
            u8"Валютный символ{{-}}важная часть имиджа страны, и даже у монголов есть тугрик <font size='+1'>₮</font>, "
                    "напоминающий могильный крест. "
                "Артемий Лебедев в конце 1990‑х говорил, что рублю не{{_}}нужен особый знак, "
                    "но впоследствии именно его пиар сделал знак <font size='+1'>₽</font> официальным." },
    { UpCategory::SYMBOL,       "Sk",   Graphical::YES, u8"Символ/модифицирующий",
            u8"Символы вроде крышки ^, внешне совпадающие с [[pt:combining|комбинирующими]] метками, но лишённые особых свойств. "
                    "А также некоторые другие символы."
                "<p>Многие из таких символов родом с пишущих машинок: чтобы нарисовать, например, ударение, надо было "
                        "вернуть каретку на шаг назад и выбить ударение в том же [[pt:mono|знакоместе]]. "
                    "Литера рисовалась так, чтобы один символ годился на роль апострофа и ударения.", },
    { UpCategory::SYMBOL,       "Sm",   Graphical::YES, u8"Символ/математический",
            u8"Изначально для математики использовались несколько разных систем, наиболее известные из них{{-}}TᴇX (читается «тех») "
            "и MathType (он же ''Microsoft Equation''). Юникод даёт надежду, что в компьютерной математике появится какая-то стандартизация{{-}}"
            "а также ограниченная возможность писать формулы в системах общего назначения вроде интернет-форумов." },
    { UpCategory::SYMBOL,       "So",   Graphical::YES, u8"Символ/другой",
            u8"Юникод содержит множество разных символов, в том числе коммерческие, технические и [[pt:emoji|эмодзи]]." },
    { UpCategory::SEPARATOR,    "Zl",   Graphical::NO,  u8"Разделитель/строк",
            u8"Единственный символ 2028" },
    { UpCategory::SEPARATOR,    "Zp",   Graphical::NO,  u8"Разделитель/абзацев",
            u8"Единственный символ 2029" },
    { UpCategory::SEPARATOR,    "Zs",   Graphical::YES, u8"Разделитель/пробел", CT_SPACE },
    //{ u8"Ошибка"sv },     //check for equal number
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
        Dating::special(), EcFont::CAUCASIAN_ALBANIAN },
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
    /// @todo [future, link] wait for book Pahlavi
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
        Dating::yapprox(-1550, StdNote::CUSTOM), EcFont::FUNKY },
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
        Dating::crange_ins(-9, -8), EcFont::NORMAL },
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
        EcScriptType::ARGUABLE, EcLangLife::HISTORICAL, EcWritingDir::LTR_COL, EcContinent::ASIA,
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
            "Phonetic Extensions" },
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
    { 0x13430, 0x1343F, { 0x13434, EcContinent::AFRICA },
            "Egyptian Hieroglyph Format Controls",
            EcScript::Egyp, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
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
            EcScript::Kana, EcFont::KOREAN, Bfg::HAS_DESCRIPTION },
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
    // NP Hmong OK
    { 0x1E100, 0x1E14F, { 0x1E118, EcContinent::AMERICA },
            "Nyiakeng Puachue Hmong", EcScript::Hmnp },
    // Toto OK
    { 0x1E290, 0x1E2BF, { 0x1E290, EcContinent::ASIA },
            "Toto", EcScript::Toto },
    // Wancho OK
    { 0x1E2C0, 0x1E2FF, { 0x1E2C0, EcContinent::ASIA },
            "Wancho", EcScript::Wcho },
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
    // Tags OK
    { 0xE0000, 0xE007F, { 0xE0054, EcContinent::NONE, Ifg::APPROX_COLLECTIVE },
            "Tags",
            EcScript::NONE, EcFont::NORMAL, Bfg::SCRIPTLIKE },
    // Var sel supp OK
    { 0xE0100, 0xE01EF, { 0xE0100, EcContinent::NONE, Ifg::APPROX_COLLECTIVE },
            "Variation Selectors Supplement" },
};
static_assert(std::size(uc::blocks) == uc::N_BLOCKS);

constinit const uc::TermCat uc::termCats[] {
    { u8"Кодирование" },
    { u8"Сериализация" },
    { u8"Классификация письменностей" },
    { u8"Классификация символов" },
    { u8"Алгоритмы Юникода" },
    { u8"Общие принципы Юникода" },
    { u8"Стили письма" },
    { u8"Ввод" },
    { u8"Прочие" },
};
static_assert (std::size(uc::termCats) == static_cast<size_t>(uc::EcTermCat::NN));

constinit const uc::Term uc::terms[] {
    /// @todo [windows] draw buttons
    { "winemoji", EcTermCat::INPUT,
      u8"панель эмодзи Windows 10/11", {},
        u8"В Windows 10/11 нажмите '''Win'''+''';''' или '''Win'''+'''.''', чтобы раскрыть панель эмодзи. "
            "Умеет вводить:<br>"
            "• [[pt:emoji|эмодзи]], существующие в стандартном шрифте. С пополнением шрифта пополнялась и панель;<br>"
            "• текстовые смайлики;<br>"
            "• типографские символы;<br>"
            "• латиницу с умляутами;<br>"
            "• валютные символы;<br>"
            "• [[ps:Grek|греческий]];<br>"
            "• стрелки;<br>"
            "• индексы, дроби, математические знаки." },
    { "altcode", EcTermCat::INPUT,
      u8"Alt-коды", u8"Alt codes",
        u8"Появившийся в BIOS компьютера IBM PC метод ввода символов по коду. "
                "В Windows семейства NT есть три типа кодов."
            "<p>'''Коды DOS'''"
            "<p>Удерживая '''Alt''' (левый{{-}}везде, правый{{-}}зависит от раскладки), "
                    "набрать на '''цифровой''' клавиатуре '''десятичный''' код символа от 1 до 255. "
                "Например, Alt…51 даст цифру 3."
            "<p>Кодировка общая для всех пользователей, является какой-то из кодировок DOS (437, 866 и{{_}}т.д.) "
                    "и зависит от выбранной кодировки неюникодных программ. "
                "Коды 1…31 дают печатаемые версии соответствующих символов: 1={{em|☺}}."
            "<p>'''Коды Windows'''"
            "<p>Удерживая '''Alt''', набрать на цифровой клавиатуре '''0''', затем '''десятичный''' код символа от 32 до 255. "
                "Alt…0240 даст букву ð."
            "<p>Кодировка Windows-1252 независимо от настроек Windows. "
                "Код 127 не{{_}}работает. Коды 128…159{{-}}разные типографские символы. "
                "Остальные совпадают с [[pt:latin1|латиницей-1]]."
            "<p>'''Коды Юникода'''"
            "<p>Экспериментальные, работают не{{_}}во всех программах. "
                "Чтобы они работали, надо в раздел реестра<br>"
                        "{{_}}HKEY_CURRENT_USER\\Control Panel\\Input Method<br>"
                    "добавить '''строковый''' параметр EnableHexNumpad=1 и перезагрузить компьютер."
            "<p>Удерживая '''Alt''', набрать '''Циф+''' и '''шестнадцатеричный''' код символа. "
                "Цифры A…F вводятся с основной клавиатуры. "
                "Работает только [[pt:bmp|базовая плоскость]]. "
                "Alt…+262F даст знак «инь и ян» {{em|☯}}." },
    { "appentry", EcTermCat::INPUT,
      u8"методы ввода из разных программ", {},
        u8"'''Libre- и Microsoft Office'''"
            "<p>Наберите шестнадцатеричный код символа (например, 262F) и нажмите '''Alt+X'''. "
                "Код преобразуется в символ{{-}}в данном случае {{em|☯}} U+262F «инь и ян»."
            "<p>'''UnicodeInput'''"
            "<p>Резидентная программа для Windows, решающая недостатки [[pt:altcode|Alt-кодов]]. "
                "Находится в области уведомлений, срабатывает при нажатии '''Alt-Циф+''' "
                    "и вводит любой символ Юникода в любую программу."
            "<p>[[https://www.fileformat.info/tool/unicodeinput/index.htm|Скачать тут]]" },
    { "birman", EcTermCat::INPUT,
      u8"типографская раскладка Ильи Бирмана", {},
        u8"Существует в двух языках: английский (США) и русский, специальные клавиши для них одинаковы. "
                "Позволяет вводить такие символы через AltGr на Windows, Alt на MacOS:"
             "<p>• европейские буквы и знаки ó ò ö ŏ ǒ ő õ ç ß ¡ ¿<br>"
                "• дореволюционная кириллица ѣ і ѵ ѳ<br>"
                "• математика − ± ≠ ≈ ′ ″ × · ∞<br>"
                "• некоторые индексы и дроби ¹²³ ½⅓¼<br>"
                "• типографские знаки препинания «» “” „“ ‹› ‘’ • — –<br>"
                "• валюты € £ ₽ ¢<br>"
                "• стрелки ← → ↓ ↑<br>"
                "• интерфейс MacOS ⌘ ⌥ ⌃ ⇧<br>"
                "• ввод ASCII-символов из русской раскладки [ { $<br>"
                "• единицы измерения ⌀ ° ‰<br>"
                "• знаки интеллектуальной собственности ™ ©"
            "<p>Версия 3.7, последняя на январь 2022, на AltGr+G имеет [[pt:private|личный]] символ F8FF, яблоко Apple."
            "<p>[[https://ilyabirman.ru/typography-layout/|Скачать тут]]" },
    { "hiero", EcTermCat::SCRIPT_CLASS,
      u8"иероглифы", u8"ideographic script",
        u8"Как говорил Артемий Лебедев, пиктограмма «стакан» означает стакан. "
                "Идеограмма означает стакан, стекло, хрупкость, пьянство… "
                "То и другое{{-}}иероглифы. "
                "Так, китайский иероглиф «человек+дерево» означает «отдых»."
            "<p>Запись иероглифами коротких мыслей нас окружает повсюду{{-}}на дорожных знаках, в интерфейсах приборов и программ. "
                "Но в целом такая запись недалеко ушла от пиктограмм. "
                "Запись иероглифами речи{{-}}это два шага вперёд: "
                    "картинка может означать понятие, и упорядочены они по месту в языке, а не{{_}}по отношению друг к другу. "
                "Если «охотник убил оленя копьём», то придумывают знак для убийства, и копьё рисуют последним, а не{{_}}между охотником и оленем."
            "<p>Прочитайте про '''[[ps:Hani|китайские иероглифы]]''', как китайцы обратили недостатки иероглифов в достоинства."
            "<p>Переход от иероглифов к [[pt:syllabic|слоговому письму]] прост: "
                    "достаточно записать незнакомое слово подходящими односложными иероглифами. "
                "Все крупные цивилизации Евразии, кроме китайской, к началу нашей эры бросили иероглифы." },
    { "abjad", EcTermCat::SCRIPT_CLASS,
      u8"консонантная письменность, абджад", u8"consonant script, abjad",
        u8"Письменность, где записываются только согласные звуки. "
                "Гласные либо опускаются полностью, либо записываются опциональными огласовками. "
                "Название «абджад» происходит от самоназвания [[ps:Arab|арабского]] алфавита{{-}}أبجد ''абджадия''."
            "<p>Несмотря на очевидные недостатки, у консонантной письменности есть и преимущества.<br>"
                "• В семитских языках{{-}}[[ps:Phnx|финикийском]], [[ps:Hebr|иврите]], арабском{{-}}корнем является костяк согласных, "
                        "а гласные играют роль суффиксов. "
                    "Так, в арабском ''джихадом'' занимается ''моджахед'' (или, точнее, ''муджахид'').<br>"
                "• Легче позволяет собирать обширные империи{{-}}сглаживаются различия в диалектах.<br>"
                "• Различия сглаживаются не{{_}}только в пространстве, но и во времени{{-}}вековые надписи лучше отвечают современному языку.<br>"
                "• Такое письмо короче, что удобно при плохом качестве «бумаги» и перьев."
            "<p>Во многих абджадах есть буквы ''матрес лекционис (матери чтения)''{{-}}согласные буквы, означающие гласный звук. "
                "Один из вариантов, зафиксированный в иврите{{,-}}''байт'' → ''бет'' (дом), и буква «йуд» переосмыслилась на слова, "
                    "где двойного звука никогда не{{_}}было. "
                "Другой вариант{{-}}по артикуляции, и к финикийской букве «йод», иногда означавшей «и», "
                    "восходит йота из [[ps:Grek|греческого]], первого известного алфавита. "
                "С появлением огласовок ''матрес лекционис'' не{{_}}исчезли, а стали дополнять их." },
    { "syllabic", EcTermCat::SCRIPT_CLASS,
      u8"слоговая азбука", u8"syllabic script",
        u8"Письменность, в которой каждая буква записывает слог из гласной и согласной. "
                "Обычно содержат 80–120 букв. "
                "Разновидностью слоговой письменности является [[pt:abugida|абугида]], где у сходных слогов сходные буквы."
            "<p>Слоговую письменность легко изобрести из [[pt:hiero|иероглифической]]: люди говорят не{{_}}звуками, а слогами, "
                "и достаточно записать незнакомое слово подходящими односложными иероглифами. "
                "Подобный процесс замечен в [[ps:Xsux|клинописи]], [[ps:Egyp|египетских иероглифах]], [[ps:Hira|японском]]."
            "<p>Чтобы записывать стечения и одинокие согласные, есть несколько вариантов (возьмём для примера слово ''стол''):<br>"
                "• Придумать символ для одинокой буквы: с‑то‑л ([[ps:Cans|канадская слоговая]]).<br>"
                "• Сделать такой символ, приделав к слогу умлят ([[pt:virama|вираму]]): сⱥ‑то‑лⱥ "
                    "(многие [[pt:brahmic|брахмийские абугиды]], особенно малоразвитые).<br>"
                "• Некоторые брахмийские абугиды ([[ps:Laoo|лаосский]], [[ps:Tavt|тай-вьет]]) в XX{{_}}веке сделали реформу "
                    "и избавились от вирамы. Основной символ стал обозначать одинокую согласную.<br>"
                "• Запретить стечения и конечные, при заимствовании добавляя гласные: су‑то‑лу (японский). "
                    "''Японский позаимствовал бы су-то-ру, но это вопрос фонетики.''<br>"
                "• В определённых условиях гласная действительно бывает немой: сᵊ‑то‑лᵊ ([[ps:Guru|гурмукхи]], частично японский).<br>"
                "• Придумать [[pt:ligature|лигатуры]]: сᵀо‑л? (развитые брахмийские абугиды вроде [[ps:Deva|деванагари]]) "
                    "или с?‑тоᶺ ([[ps:Limb|лимбу]]).<br>"
                "• Отбросить конечную согласную: с?‑то ([[ps:Linb|микенский]], [[ps:Buhd|бухид]])." },
    { "abugida", EcTermCat::SCRIPT_CLASS,
      u8"абугида", u8"abugida",
        u8"Исследователи не{{_}}сходятся в определении абугиды, и самое широкое из них такое: "
                    "[[pt:syllabic|слоговая]] азбука, где у сходных слогов сходные символы."
            "<p>Абугидами '''точно являются''' [[pt:brahmic|брахмийские письменности]]."
            "<p>Абугидами '''являются у многих исследователей''' [[ps:Ethi|эфиопская]] и [[ps:Cans|канадская слоговая]] "
                    "письменности."
            "<p>Абугидой '''является в отдельных источниках''' [[ps:Thaa|тана]]."
            "<p>Абугидами '''не являются ни в одном источнике''' [[ps:Hang|хангыль]], [[ps:Yiii|и]]."
            "<p>Что интересно, название «абугида» происходит от эфиопского {{sm|አቡጊዳ}} (а-бу-ги-да){{-}}получается, некоторые исследователи "
                    "отказывают в праве называться абугидой той письменности, откуда это понятие пошло!" },
    { "brahmic", EcTermCat::SCRIPT_CLASS,
      u8"брахмийская абугида", u8"Brahmic abugida, Indic script",
        u8"Исследователи расходятся в определении [[pt:abugida|абугиды]]. "
                "Но в одном они единогласны: абугидами является '''группа письменностей, восходящих к [[ps:Brah|брахми]]'''."
            "<p>В семитских языках корнем слова являются согласные, а гласные{{-}}суффиксы и окончания: "
                    "в [[ps:Arab|арабском]] ''джихадом'' занимается ''моджахед'' (или, точнее, ''муджахид''). "
                "Для них естественно было ввести [[pt:abjad|консонантную]] письменность. "
                "Когда [[ps:Armi|арамейское]] письмо пошло на восток и столкнулось с индоевропейскими языками, появилась "
                    "слоговая письменность, которая в том или ином виде распространилась на всю Юго-Восточную Азию, кроме "
                    "[[pt:cjk|Китая, Японии и Кореи]]."
            "<p>Особенности брахмийских абугид:<br>"
                "• согласная буква означает слог с некоей гласной по умолчанию, остальные приделываются огласовками: "
                    "[[ps:Deva|деванагари]] <nobr>ка {{sm|क}} + и {{sm| ि}} = ки {{sm|कि}}</nobr>;<br>"
                "• чтобы получить одинокую согласную, используется знак «[[pt:virama|вирама]]»: "
                    "<nobr>ка {{sm|क}} + {{sm|्}} = к {{sm|क्}}</nobr>;<br>"
                "• ''могут'' быть [[pt:ligature|лигатуры]] из нескольких согласных{{-}}деванагари {{sm|क्ष}} кша. "
                    "Печатаются они, как будто лигатуры нет: ка+вирама+ша;<br>"
                "• прочие диакритические знаки{{-}}анусвара (носовой призвук в гласном), "
                    "чандрабинду (вся гласная произносится через нос), "
                    "нукта (заимствованные звуки; кстати, буква «к» в этом слове заимствованная и пишется с нуктой), "
                    "висарга (звук ''h'' в санскрите);<br>"
                "• написание слева направо, в отличие от семитских языков;<br>"
                "• алфавитный порядок{{-}}фонологический: сначала гласные, потом согласные от глотки «к» до зубов «с», "
                    "и в конце выдох «h»;<br>"
                "• часты собственные [[pt:number|цифры]]{{-}}как реально используемые, так и [[pt:obsolete|устаревшие]]."
            "<p>Брахмийскими абугидами являются большинство письменностей Индии, [[ps:Khmr|кхмерский]], [[ps:Thai|тайский]], "
                    "[[ps:Laoo|лаосский]], [[ps:Lana|ланна]], а также множество мелких исчезающих письменностей региона "
                    "вроде [[ps:Sund|сунданского]]." },
    { "alphabet", EcTermCat::SCRIPT_CLASS,
      u8"алфавитная письменность", u8"alphabet script",
        u8"Линейное письмо, записывающее гласные и согласные звуки. "
                "Первым известным алфавитом считается [[ps:Grek|греческий]]."
            "<p>Нелинейное письмо, в котором гласные и согласные объединяются в слоги со сложным порядком, "
                    "алфавитом считают или не{{_}}все исследователи ([[ps:Thaa|тана]], [[ps:Hang|хангыль]]), "
                    "или вообще никто ([[ps:Laoo|лаосский]])."
            "<p>При всех достоинствах алфавитов у них есть серьёзный недостаток. "
                "Из-за сдвигов в произношении появляется сложная исторически обусловленная орфография, "
                    "которую время от времени нужно реформировать. "
                "Так, [[ps:Cyrl|русский]] ять '''ѣ''' был отдельным звуком, в XVIII{{_}}веке полностью исчезнувшим. "
                "Орфография [[ps:Latn|английского]] и французского во многом историческая." },
    { "arguable", EcTermCat::SCRIPT_CLASS,
      u8"письменность спорного типа", {},
        u8"Для пяти письменностей источники не{{_}}договорились, что они собой представляют."
            "<p>[[ps:Hang|Хангыль]] ''(корейский)''{{-}}[[pt:syllabic|слоговая]] ''(Юникод)'' "
                    "или [[pt:alphabet|алфавитная]] ''(Omniglot, английская Википедия)''."
            "<p>[[ps:Thaa|Тана]] ''(мальдивский)''{{-}}[[pt:abjad|консонантная]] ''(Omniglot)'', "
                    "[[pt:abugida|абугида]] ''(английская Википедия)'' или алфавитная ''(Юникод)''."
            "<p>[[ps:Wara|Варанг-кшити]] и [[ps:Sora|соранг-сомпенг]]{{-}}"
                    "алфавит ''(английская Википедия, Юникод)'' или абугида ''(Omniglot)''. "
                "Обе индийские, отошли от многих черт [[pt:brahmic|брахмийских абугид]], но оставили гласную «а» по умолчанию."
            "<p>[[ps:Phag|Монгольское квадратное]] (письмо Пагба-ламы){{-}}абугида ''(Юникод, Omniglot)'' "
                    "или алфавит ''(английская Википедия)''. "
                "Из признаков абугиды осталась буква по умолчанию и соединение букв в слоги." },
    { "cjk", EcTermCat::SCRIPT_CLASS,
      u8"ККЯ = китайский, корейский, японский", u8"CJK = Chinese, Japanese, Korean",
        u8"Группа письменностей, близких к [[ps:Hani|китайским иероглифам]]. "
                "К ним относятся также слоговые [[ps:Kana|катакана]] и [[ps:Hira|хирагана]], и "
                    "[[ps:Hang|хангыль]] [[pt:arguable|спорного типа]]. "
                "Из исторических{{-}}[[ps:Tang|тангутский]], [[ps:Kits|киданьский]], [[ps:Nshu|нюй-шу]]."
            "<p>Особенности ККЯ:<br>"
                "• размашистое письмо кистью и тушью (за исключением нюй-шу);<br>"
                "• символы моноширинные{{-}}независимо от количества черт, выровнены по невидимой сетке. "
                    "Заимствованные буквы часто полуширинные{{-}}занимают половину клетки;<br>"
                "• письмо изначально справа налево по столбцам; сейчас ''обычно'' слева направо по строкам. "
                    "И поныне японские комиксы ''манга'' читаются справа налево;<br>"
                "• иероглифы несколько отличаются от страны к стране, Юникод оставляет эти тонкости за шрифтами;<br>"
                "• поскольку символы не{{_}}вписываются в 256 [[pt:codepoint|позиций]], существовали специфичные кодировки для иероглифов." },
    { "code", EcTermCat::SCRIPT_CLASS,
      u8"коды и стенография", u8"codes and shorthand",
        u8"Код{{-}}это другая форма записи обычного письменного текста. "
                "Другими словами, мы представляем устную речь в виде букв по правилам правописания, а потом кодируем. "
                "Цель такой записи бывает разная:<br>"
                "• связь и сигнализация (флажный семафор, код Морзе);<br>"
                "• реабилитация инвалидов ([[ps:Brai|шрифт Брайля]]);<br>"
                "• стенография{{-}}письмо, выполняемое специально обученным писцом и успевающее за устной речью "
                    "([[ps:Dupl|система Дюплойе]])."
            "<p>Такие системы часто захватывают много языков, и в разных языках кодовые таблицы разные. "
                "Так, отличается даже британский и американский Брайль." },
    { "nontext", EcTermCat::SCRIPT_CLASS,
      u8"нетекстовая нотация", u8"non-text notation",
        u8"Запись других видов информации, кроме текста: "
                "музыки ([[pk:1D100|ноты]]), чертежей ([[pk:2300|технические символы]]), жестов ([[ps:Sgnw|жестовое письмо]])."
            "<p>Юникод работает с [[pt:plaintext|простым текстом]] и не{{_}}предназначен для кодирования такой информации. "
                "Однако кодирование таких символов сокращает PDF-файлы, "
                    "расширяет возможности систем общего назначения{{-}}форумов, мессенджеров, векторных редакторов." },

    ///// Char classes /////////////////////////////////////////////////////////

    { "acrophonic", EcTermCat::CHAR_CLASS,
      u8"акрофонический символ", u8"acrophonic character",
        u8"Для буквы{{-}}называется словом, начинающимся с этой буквы, как в [[ps::Thai|тайском]], старой [[ps:Cyrl|кириллице]] "
                    "и [[ps:Phnx|финикийском]]."
            "<p>Для цифр и прочего{{-}}символ происходит от первой буквы понятия. "
                "Например, знак '''№''' происходит от лат. ''numero''." },
    { "virama", EcTermCat::CHAR_CLASS,
      u8"вирама", u8"virama",
        u8"Вирама (санскрит «остановка»){{-}}знак в [[pt:brahmic|брахмийских]] абугидах, означающий «стереть гласную по умолчанию». "
                "Например, в [[ps:Deva|деванагари]]: <nobr>ка {{sm|क}} + {{sm|्}} = к {{sm|क्}}</nobr>."
            "<p>Реально в индийских языках символ называется ''халант (хинди «убийца»), халанта, хошонто, "
                "чандраккала ([[ps:Mlym|малаялам]] «лунный»)''."
            "<p>Некоторые языки пользуются вирамой нечасто. Например:<br>"
                "• [[pt:ligature|лигатуры]], означающие стечение согласных{{-}}деванагари {{sm|क्ष}} кша. "
                    "Печатаются и кодируются они, как будто лигатуры нет: ка+вирама+ша. "
                    "Чтобы разорвать лигатуру и вывести две буквы и вираму, вводится ка+вирама+ZWNJ+ша;<br>"
                "• знаки для конечного согласного{{-}}[[ps:Cham|чамский]], [[ps:Limb|лимбу]];<br>"
                "• гласная по умолчанию редуцировалась{{-}}[[ps:Guru|пенджабский]];<br>"
                "• реформа орфографии лишила букву гласной по умолчанию{{-}}[[ps:Laoo|лаосский]];<br>"
                "• вирама может быть единым целым с согласной{{-}}[[ps:Talu|новое письмо лы]];<br>"
                "• не{{_}}способны записывать «лишние» согласные{{-}}[[ps:Bugi|лонтара]]."
            "<p>Большинство вирам{{-}}[[pt:combining|комбинирующие метки]]. "
                "Но встречаются и [[pt:spacing|протяжённые]] ([[ps:Java|яванский]])."
            "<p>В некоторых письменностях ([[ps:Khar|кхароштхи]], [[ps:Mtei|манипури]]) существует «виртуальная вирама» "
                    "для набора лигатур. "
                "Иногда даже ([[ps:Sund|сунданский]], [[ps:Cakm|чакма]]) в дополнение к видимой вираме. "
                "Виртуальная вирама для удобства выглядит как крест, но оставить её видимой в обычном тексте{{-}}ошибка." },
    { "graphic", EcTermCat::CHAR_CLASS,
      u8"графический символ, печатаемый символ", u8"graphic character, printable character",
        u8"Символ, который печатается на экране или бумаге. "
                "Любой, кроме [[pt:control|управляющих]], [[pt:format|форматирующих]], разделителей строк и абзацев{{-}}"
                    "даже [[pt:space|пробелы]] и невидимые [[pt:combining|метки]]."
            "<p>Пробел нулевой ширины считается форматирующим, а не{{_}}пробелом{{-}}а значит, он '''не''' графический."
            "<p>Парадокс: невидимые метки одновременно графические и [[pt:ignorable|игнорируемые]]."
            "<p>Не путать с [[pt:pseudographics|псевдографикой]]." },
    { "deprecated", EcTermCat::CHAR_CLASS,
      u8"запрещённый символ", u8"deprecated character",
        u8"Символ, чьё использование настоятельно не{{_}}рекомендуется."
            "<p>Одна из гарантий [[pt:stability|стабильности]] Юникода: как только символ назначен, он не{{_}}сдвинется и не{{_}}исчезнет. "
                "Если символ оказывается неудачным, его запрещают. "
                "Причины запрета разные."
            "<p>В Консорциуме Юникода сидят люди западной культуры, и они могут лишь ссылаться на исследования специалистов по редким языкам{{-}}"
                    "так случилось для пяти символов [[ps:Arab|арабского]], [[ps:Tibt|тибетского]] и [[ps:Khmr|кхмерского]]."
            "<p>Две технические угловые скобки запретили, чтобы не{{_}}путали с математическими. "
                "Также запрещены языковый [[pk:E0000|тэг]] (внёс больше неразберихи, чем решил проблем), "
                    "шесть символов [[pt:bidi|двунаправленного письма]] (предложен другой механизм) "
                    "и латинская [[pt:ligature|лигатура]] '''ŉ''' "
                        "(в голландском и африкаанс есть много похожих сокращений, никак не{{_}}закодированных, "
                        "проблемы с [[pt:normalization|нормализацией]], "
                        "упразднили тип символов «буква со знаком»){{-}}итого 15 на Юникод{{_}}14.0."
            "<p>Известны несколько случаев возврата символов в обиход.<br>"
                "• Блок тэгов{{-}}использовался для указания языка. "
                    "Вернули для [[pt:emoji|эмодзи]] флагов регионов (например, Шотландии), "
                        "оставив под запретом один символ.<br>"
                "• 0340, 0341 (метки нисходящего/восходящего тона). "
                    "Предлагается замена в виде обычных умляутов à и á. "
                    "С Юникода 5.2 просто не{{_}}рекомендуются.<br>"
                "• 17D3 (кхмерский умляут ''bathamasat''). "
                    "С появлением [[pt:precomposed|монолитных]] символов [[pk:19E0|лунного календаря]] в Юникоде 4.0 запретили. "
                    "С Юникода 5.2 просто не{{_}}рекомендуется{{-}}вероятно, из-за громоздкости календарной лигатуры. "
                    "У автора «Юникодии» были проблемы с поиском шрифта, где лигатуры хорошо видны."
            "<p>Не путать с [[pt:obsolete|устаревшим]]." },
    { "ignorable", EcTermCat::CHAR_CLASS,
      u8"игнорируемый символ", u8"default-ignorable character",
        // For translator: столбики = stacks, шрифтовые механизмы = font features,
        //   стыковаться = tile, разъединитель графем = grapheme joiner, человек = person
        /// @todo [link] links to single char: CGJ, etc.
        u8"Юникод велик и постоянно расширяется: в версии {{version}} символов уже {{nchars}}. "
                "Так что даже обширная коллекция шрифтов не{{_}}поддерживает весь Юникод."
            "<p>Если в шрифтах не{{_}}нашлось глифа, символ обычно отображаются как [[pt:tofu|тофу]]{{-}}белый квадратик. "
                "'''Но есть символы, которые просто пропускаются.'''"
        "<p>Большинство игнорируемых символов являются [[pt:format|форматирующими]], но есть и непротяжённые [[pt:combining|метки]]. "
            "Игнорируемые символы делятся на такие категории:"
         "<p>• Символы, несущие дополнительную семантику{{-}}мягкий перенос 00AD, разъединитель графем 034F, [[pk:E0000|тэги]]…<br>"
            "• Тэги [[pt:bidi|направления]] написания{{-}}например, метки LRM и RLM.<br>"
            "• Невидимые символы, задействованные в необязательных шрифтовых механизмах{{-}}"
                "например, соединитель нулевой длины (ZWJ), селекторы начертания (VS), заполнители [[ps:Hang|корейских]] чамо. "
                "Пилот (человек+ZWJ+самолёт) при отсутствии [[pt:ligature|лигатуры]] отобразится "
                "как человек+самолёт {{em|🧑✈️}}{{-}}и не{{_}}важно, есть ли в шрифте ZWJ.<br>"
            "• Пробел нулевой длины ZWSP; "
                "[[pt:bom|метка порядка байтов]] BOM, также являющаяся пробелом нулевой длины; "
                "разъединитель нулевой длины ZWNJ.<br>"
            "• Символы сложного форматирования{{-}}[[pk:1BCA0|стенографии]], [[pk:1D100|нот]]."
        "<p>'''Не являются''' игнорируемыми:<br>"
            "• Все имеющие графическое отображение, все [[pt:spacing|протяжённые]].<br>"
            "• [[pt:control|Управляющие]], разделители строк.<br>"
            "• Символы, задействованные в шрифтовых механизмах, которые пропускать нельзя{{-}}"
                    "например, виртуальные [[pt:virama|вирамы]]. "
                "Ведь если вирама исчезла, «кта» превращается в «ката».<br>"
            "• Символы сложного форматирования [[ps:Egyp|египетских иероглифов]]." },
    { "combining", EcTermCat::CHAR_CLASS,
      u8"комбинирующая метка", u8"combining mark",
        u8"Несамостоятельный символ, прикрепляемый к основному. Используется для:"
             "<p>• диакритических знаков{{-}}пометок, прикрепляемых к основному символу;<br>"
                "• огласовок в [[pt:abjad|консонантных]] письменностях и [[pt:abugida|абугидах]];<br>"
                "• разных «хаков» через [[pt:ligature|лигатуры]] в шрифтах: например, буква + селектор начертания = буква другой формы."
            "<p>Юникод делит метки на [[pt:spacing|протяжённые]], непротяжённые и охватывающие."
            "<p>В шрифтах комбинирующие метки устанавливаются несколькими cпособами, от самого качественного:<br>"
                "• Лигатура: отдельный [[pt:character|глиф]] для символа с меткой.<br>"
                "• Подмена глифа: встав на глиф А, метка Б приобретает форму В. "
                    "Это используется для сплющенных меток на заглавные буквы, подгонки размера индийских огласовок.<br>"
                "• Якорь: вручную установленная точка привязки к символу.<br>"
                "• И якорь по умолчанию: метка ставится по положительным или отрицательным координатам, чтобы, будучи "
                    "нарисованной относительно курсора, встала на подходящем месте."
            "<p>Юникод не{{_}}позволяет накрыть одной меткой несколько символов, однако позволяет задействовать несколько механизмов шрифтов:<br>"
                "• у некоторых меток половина находится над символом, половина свисает наружу, "
                    "и можно накрыть этой меткой два символа: {{sm|а͡б}};<br>"
                "• первый символ накрываем начальным фрагментом, второй и{{_}}т.д.{{-}}повторяющимся, и последний{{-}}конечным. "
                    "В большинстве шрифтов этот метод реализован плохо." },
    { "ligature", EcTermCat::CHAR_CLASS,
      u8"лигатура", u8"ligature",
        u8"Лигатура{{-}}это несколько символов в одном:"
            "<p>1. В типографике{{-}}символ, составленный из нескольких. Так, амперсанд '''&''' является "
                    "лигатурой латинского ''et{{-}}и''."
            "<p>2. В шрифтовом деле{{-}}[[pt:character|глиф]], означающий последовательность из нескольких [[pt:codepoint|кодовых позиций]]. "
                "Так, в некоторых шрифтах набор «fi» пропечатывает лигатуру '''ﬁ'''. "
                "Много таких лигатур в [[pt:brahmic|брахмийских абугидах]]."
            "<p>Все лигатуры{{-}}[[pt:spacing|протяжённые]] символы."
            "<p>[[pt:casefolding|Свёртка регистра]] ''может'' раскрывать лигатуры." },
    { "precomposed", EcTermCat::CHAR_CLASS,
      u8"монолитный символ", u8"precomposed character",
        u8"Символ и [[pt:combining|диакритическая метка]] в одном. "
                "Например, '''á''' собрано из буквы и метки, а '''á'''{{-}}один монолитный символ."
            "<p>Три основные причины создания монолитных символов{{-}}другая сущность, совместимость и красота."
            "<p>'''Другая сущность:''' согласитесь, Ё{{-}}это не{{_}}Е с умляутом. "
                "А Ю́{{-}}это именно что Ю с ударением, потому такого монолитного символа нет."
            "<p>'''Совместимость:''' если ограниченный типографский движок не{{_}}может накладывать метки, или "
                    "ограниченный клавиатурный драйвер печатает по одному символу, монолитные символы{{-}}всё, что остаётся. "
                "Часто монолитные символы берутся из существующих кодировок."
            "<p>'''Красота:''' часто механизмы наложения меток оставляют желать лучшего, особенно если дизайнер не{{_}}предусмотрел "
                    "на символе А якоря для метки Б. "
                "И монолитные символы могут стать предупреждением дизайнеру, что ''такое бывает''."
            "<p>В более широком контексте монолитными называют любые символы, которые можно нарисовать стилями, "
                "но они закодированы едиными символом{{-}}например, дробь <big>½</big>."
            "<p>Все монолитные символы являются [[pt:spacing|протяжёнными]]. "
                "См. также [[pt:normalization|Нормализация]]." },
    { "space", EcTermCat::CHAR_CLASS,
      u8"пробел", u8"space character", CT_SPACE },
    { "spacing", EcTermCat::CHAR_CLASS,
      u8"протяжённый символ", u8"spacing character",
        u8"Символ, занимающий место в строке."
            "<p>Таковыми являются буквы, [[pt:number|цифры]], знаки препинания, [[pt:space|пробелы]], разные символы "
                    "и протяжённые [[pt:combining|комбинирующие метки]]."
            "<p>Таковыми не{{_}}являются [[pt:control|управляющие]], [[pt:format|форматирующие]] символы и прочие типы меток." },
    { "pseudographics", EcTermCat::CHAR_CLASS,
      u8"псевдографика", u8"pseudographics, semigraphics, box drawing",
        u8"Символы для рисования геометрических фигур{{-}}линий, прямоугольников, рамок{{-}}в текстовом режиме. "
                "Широко применялись для оживления текстовых интерфейсов пользователя."
            "<p>В современных графических ОС бессмысленны: есть настоящая графика, а большинство шрифтов пропорциональные "
                    "и не{{_}}дают состыковать линии. "
                "Но даже в них изредка встречаются текстовые консоли{{-}}а там псевдографика."
            "<p>Все псевдографические символы являются [[pt:spacing|протяжёнными]]." },
    { "control", EcTermCat::CHAR_CLASS,
      u8"управляющий символ", u8"control character", CT_CONTROL },
    { "obsolete", EcTermCat::CHAR_CLASS,
      u8"устаревший символ", u8"obsolete character",
        u8"Символ, который вышел из использования. "
                "Например, [[ps:Cyrl|кириллический]] ять <big>'''ѣ'''</big>, во всех языках превратившийся в другие гласные: "
                    "'''е''' в русском, '''і''' в украинском."
            "<p>В соответствующем контексте его использовать совершенно законно: "
                    "«бѣлый, блѣдный, бѣдный бѣсъ…»"
            "<p>Не путать с [[pt:deprecated|запрещённым символом]]." },
    { "format", EcTermCat::CHAR_CLASS,
      u8"форматирующий символ", u8"format character", CT_FORMAT },
    { "number", EcTermCat::CHAR_CLASS,
      u8"числовой символ", u8"number character",
        u8"Юникод делит числовые символы на три класса:"
         "<p>• Десятичные позиционные цифры{{-}}как привычные [[pt:ascii|ASCII]]-цифры 0…9, так и цифры других письменностей "
                "([[ps:Deva|деванагари]] ०…९).<br>"
            "• Эти же цифры, используемые в особых контекстах{{-}}например, в верхних/нижних индексах.<br>"
            "• Прочие числовые символы: цифры недесятичных, непозиционных и частично позиционных систем счисления "
                "(римские, эгейские, счётные палочки), [[pt:precomposed|монолитные]] числа вроде <big>½</big>…"
            "<p>Символы, чьё основное предназначение нечисловое (например, славянские цифры вроде А̄) Юникод числами не{{_}}считает." },
    { "emoji", EcTermCat::CHAR_CLASS,
      u8"эмодзи", u8"emoji",
        u8"От японского {{sm|絵}} ''э{{-}}картинка'' + {{sm|文字}} ''модзи{{-}}знак, символ."
            "<p>Придуманы мобильным оператором NTT DoCoMo около 1999 для экономии места в SMS. "
                "Западные операторы начали разрабатывать свои наборы с 2004, и в 2010 кодировку DoCoMo внесли в Юникод 6.0. "
                "Многие эмодзи специфичны для Японии: кланяющийся бизнесмен {{em|🙇}}, знак начинающего водителя {{em|🔰}}… "
                "Другие изменили значение за пределами Японии: знак высшего балла {{em|💯}} превратился в «полностью согласен»."
            "<p>Несколько старых пиктограмм, появившихся до 2010, гарантированно становятся эмодзи только с символом "
                "U+FE0F (селектор начертания 16)."
            "<p>Все эмодзи являются [[pt:spacing|протяжёнными]] символами." },

    ///// Principles ///////////////////////////////////////////////////////////

    { "stability", EcTermCat::PRINCIPLES,
      u8"гарантии стабильности", u8"stability guarantees",
        u8"Консорциум Юникода гарантирует, что не{{_}}изменит часть характеристик символов."
            "<p>'''Символ не{{_}}сдвинется и не{{_}}исчезнет''' (2.0+). "
                "Каждый новый Юникод будет надмножеством предыдущего. "
                "Сортировка текста происходит через национальный порядок сортировки."
            "<p>'''Название символа не{{_}}изменится''' (2.0+). "
                "Впрочем, существуют названия-исправления, и именно  '''их''' показывает «Юникодия». "
                "Также не{{_}}изменятся названия-синонимы, в том числе исправления (5.0+)."
            "<p>'''[[pt:normalization|Нормализация]] строки не{{_}}изменится''' (4.1+). "
                "В версии 3.1 были более слабые гарантии, связанные с тем, что в Юникод входило всё больше [[ps:Latn|латиницы]] "
                    "и других частых письменностей, для которых нормализация имеет смысл."
            "<p>'''Свойства символа могут меняться, но не{{_}}настолько, чтобы изменить его сущность''' (1.1+). "
                "Например, заглавная буква никак не{{_}}может стать строчной, а [[pt:noncharacter|выброшенный]] "
                    "не{{_}}вернётся в пользование. "
                "В дальнейших версиях Юникода это правило конкретизировалось, не{{_}}будем уточнять."
            "<p>'''Нормативные и информационные свойства символа из баз Юникода не{{_}}исчезнут''' (5.2+). "
                "Остальные могут исчезать и переименовываться, но при переименовании никогда не{{_}}займут место другого."
            "<p>'''Не изменится [[pt:casefolding|свёртка регистра]]''' (5.0+). "
                "Свёртка регистра служит для локаленезависимого сравнения строк без регистра и не{{_}}обязательно перевод в строчные: "
                    "для [[ps:Cher|чероки]] это перевод в заглавные, так как они появились раньше."
            "<p>'''Если два символа не{{_}}были (или были) в регистровой паре, они там и останутся''' (5.0+). "
                "У строчной буквы может появиться аналогичная заглавная (или наоборот, как для чероки), "
                    "и это единственный способ для буквы войти в регистровую пару." },
    { "dynamic", EcTermCat::PRINCIPLES,
      u8"динамическая компоновка", u8"dynamic composition",
        u8"Такой высокой цели, как [[pt:universality|универсальность]], Юникод добивается с помощью динамической компоновки{{-}}"
                    "изображение собирается на лету из частей. "
                "Иногда для удобства делают и [[pt:precomposed|монолитные символы]], но в целом <nobr>A + ¨ = Ä</nobr>."
            "<p>Чтобы проверять совпадение текстов с учётом разных вариантов кодирования, используется алгоритм "
                "[[pt:normalization|нормализации]]." },
    { "logical", EcTermCat::PRINCIPLES,
      u8"логический порядок", u8"logical order",
        u8"Юникод хранит символы в логическом порядке, который примерно соответствует порядку произношения или набора с клавиатуры. "
                "Этот порядок не{{_}}совпадает с порядком отображения текста на экране, и алгоритмы [[pt:bidi|двунаправленного текста]] "
                    "призваны расставить символы в визуальном порядке."
            "<p>[[pt:combining|Комбинирующие метки]] хранятся после своих символов."
            "<p>Исключение: некоторые [[pt:brahmic|брахмийские]] абугиды ([[ps:Thai|тайский]], [[ps:Laoo|лаосский]]) "
                "традиционно хранят текст в визуальном порядке: некоторые гласные находятся перед согласной, даже если читаются после." },
    { "convertibility", EcTermCat::PRINCIPLES,
      u8"преобразуемость", u8"convertibility",
        u8"Юникод используется для обмена с другими стандартами текста{{-}}национальными, международными, "
                    "специфичными для производителя. "
                "Если варианты одного символа получают разные коды в таких стандартах, они получают разные коды и в Юникоде. "
                "Это позволяет преобразовывать из кодировки в кодировку через Юникод, не{{_}}терять информацию при круговом преобразовании "
                    "(из кодировки в Юникод и обратно)."
            "<p>Иногда, впрочем, одна кодовая позиция другой кодировки требует нескольких символов Юникода, или наоборот." },
    { "plaintext", EcTermCat::PRINCIPLES,
      u8"простой текст", u8"plain text",
        u8"Простой текст{{-}}последовательность кодов символов, в противовес ''форматированному тексту'', который может содержать "
                    "элементы оформления наподобие цветов, шрифтов, гиперссылок. "
                "Если из форматированного текста убрать эти элементы, останется простой."
            "<p>Чтобы превратить простой текст в строку видимого текста, нужен процесс под названием ''рендеринг'', или ''отрисовка''. "
                "Если дать простой текст другой реализации рендерера, текст, скорее всего, будет выглядеть по-другому."
            "<p>В стандарте Юникода есть две чеканных фразы."
            "<p>1. Юникод кодирует простой текст."
            "<p>2. Простой текст должен содержать достаточно информации, чтобы отрисовать его читаемо, и больше ничего." },
    { "semantics", EcTermCat::PRINCIPLES,
      u8"семантика", u8"semantics",
        u8"Поведение символа в первую очередь задаётся не{{_}}названием, а сложными таблицами. "
                "Отрывки этих таблиц приведены и в «Юникодии»: [[pt:number|числовое]] значение, регистр, [[pt:bidi|направление]] письма…" },
    { "character", EcTermCat::PRINCIPLES,
      u8"символы, не глифы", u8"characters, not glyphs",
        u8"Символ{{-}}единица смысла. Глиф{{-}}изображение, содержащееся в шрифте и выводимое на экран/печать."
            "<p>Символы и глифы соотносятся не 1:1. "
                "''Настали́к'', стиль [[ps:Arab|арабского]] текста, требует тысячи глифов в шрифте. "
                "Но в кодировке всего две сотни символов, передающих ''смысл''. "
                "И наоборот, у разных символов, несмотря на [[pt:unification|унификацию]], бывает один глиф." },
    { "universality", EcTermCat::PRINCIPLES,
      u8"универсальность", u8"universality",
        u8"Юникод разработан для людей разных языков и профессий: работающих в бизнесе, образовании, религии и науке, "
                    "для современных и исторических текстов."
            "<p>За пределами Юникода лежат:<br>"
                "• письменности, про которые мало что известно, чтобы надёжно закодировать символы;<br>"
                "• письменности, чьи пользователи не{{_}}пришли к де-факто стандарту;<br>"
                "• нетекстовые (например, пиктографические) письменности." },
    { "unification", EcTermCat::PRINCIPLES,
      u8"унификация", u8"unification",
        u8"Юникод старается не{{_}}дублировать символы. "
                "Знаки препинания закодированы один раз и используются во всех западных языках{{-}}а также в восточных, куда они проникли. "
                "Точно так же одни и те же арабские знаки препинания используются в персидском и урду. "
                "Буква Y в английском («вай»), французском («игрек») и немецком («ипсилон»){{-}}одна и та же кодовая позиция. "
                "Мало того, одни и те же [[ps:Hani|иероглифы]] в китайском и японском могут выглядеть по-разному."
            "<p>Есть, впрочем, несколько важных исключений:<br>"
                "• Совсем разные сущности: буква A из [[ps:Latn|латинского]], [[ps:Grek|греческого]], [[ps:Cyrl|кириллицы]] "
                    "и [[ps:Cher|чероки]]{{-}}это разные буквы.<br>"
                "• Удобство обработки: существует дефис простой и неразрывный. "
                    "В Юникоде аж три буквы Ð с разными строчными.<br>"
                "• Удобство шрифтового дела: в каждом южноазиатском языке виртуальная [[pt:virama|вирама]] "
                    "(спецсимвол для набора стыков согласных) своя, "
                    "чтобы дизайнер прочитал стандарт и посмотрел, что она делает.<br>"
                "• Исторические причины{{-}}для [[pt:convertibility|кругового преобразования]]: "
                    "китайские иероглифы из старых кодировок всегда китайские и не{{_}}имеют японских аналогов." },
    { "efficiency", EcTermCat::PRINCIPLES,
      u8"эффективность", u8"efficiency",
        u8"Юникод старается, чтобы эффективные реализации были осуществимы. "
                "В частности:"
             "<p>• В Юникоде нет переключаемых регистров. Это упрощает доступ и поиск.<br>"
                "• Основные формы [[pt:serialization|сериализации]]{{-}}[[pt:utf8|UTF-8]] и [[pt:utf16|UTF-16]]{{-}}самосинхронизирующиеся. "
                    "Это упрощает доступ и поиск.<br>"
                "• Коды символов{{-}}числа от 0 до 10FFFF₁₆. "
                    "Даже если не{{_}}получается работать прямо в UTF-8/16, таблицы поиска всё равно действуют.<br>"
                "• Обработка [[pt:ascii|ASCII]] возможна в UTF-8 без раскодировки. "
                    "Обработка [[pt:bmp|большинства языков]] возможна в UTF-16 без раскодировки.<br>"
                "• Символы одной письменности сгруппированы рядом настолько, насколько это возможно.<br>"
                "• Знаки препинания общие для всех письменностей.<br>"
                "• Форматирующие символы описаны по максимуму.<br>"
                "• Избегают [[pt:format|форматирующих]] символов, которые меняют внутренее состояние. "
                    "Это не{{_}}всегда возможно, [[pt:bidi|двунаправленное письмо]] в принципе сложное." },

    ///// Encoding /////////////////////////////////////////////////////////////

    { "ascii", EcTermCat::ENCODING, u8"ASCII", {}, CT_ASCII },
    { "bmp", EcTermCat::ENCODING,
      u8"базовая многоязычная плоскость", u8"BMP = Basic Multilingual Plane",
        u8"[[pt:plane|Плоскость]] Юникода с номером 0, содержащая символы 0000…FFFD. "
                "В базовой плоскости закодированы наиболее ходовые символы живых языков, "
                    "что позволяет обрабатывать эти языки прямо в [[pt:utf16|UTF-16]]. "
            "<p>До 1996 Юникод был двухбайтовым. "
                "Но к тому времени несколько прогрессивных языков и библиотек (Java, Windows{{_}}NT) успели перейти на Юникод, "
                    "что и привело к компромиссу в виде базовой плоскости и UTF-16."
            "<p>В Юникоде 14.0 нераспределёнными остались 16 позиций, 2FE0…2FEF."
            "<p>Базовая плоскость содержит [[pt:ascii|ASCII]], [[pt:latin1|латиницу-1]] и 6400 [[pt:private|личных]] символов." },
    { "noncharacter", EcTermCat::ENCODING,
      u8"выброшенная позиция", u8"non-character", CT_NOCHAR },
    { "codepoint", EcTermCat::ENCODING,
      u8"кодовая позиция", u8"code point",
        u8"Кодовая позиция{{-}}число или последовательность чисел, которые представляют некую концепцию "
                    "в кодировке{{-}}символ, управляющую команду, [[pt:combining|комбинирующую метку]]. "
                "Или теоретически могут представлять, но пусты{{-}}временно или [[pt:noncharacter|навечно]]. "
                "В многобайтовых кодировках латинские буквы могут быть кодовыми позициями из одного байта, а иероглифы{{-}}"
                    "кодовыми позициями из нескольких байтов."
            "<p>В Юникоде кодовая позиция записывается числом от 0 до 1114111 (в шестнадцатеричной системе{{-}}до 10FFFF). "
                "Не путать с ''кодовыми единицами''{{-}}байтами или словами в "
                    "[[pt:utf8|UTF-8]]/[[pt:utf16|16]]/[[pt:utf32|32]]."
            "<p>Один символ может записываться в разные кодовые позиции, потому что изображает разные концепции. "
                "Простейший пример{{-}}A [[ps:Latn|латинское]] и [[ps:Cyrl|русское]]. "
                "Иногда это случается по историческим причинам{{-}}микро- {{sm|µ}} и мю {{sm|μ}}."
            "<p>И наоборот, разные символы могут [[pt:unification|записываться]] одной кодовой позицией: "
                "сходные [[ps:Hani|иероглифы]] китайского и японского." },
    { "latin1", EcTermCat::ENCODING, u8"латиница-1", u8"Latin-1", CT_LATIN1 },
    { "plane", EcTermCat::ENCODING,
      u8"плоскость", u8"plane",
        u8"Диапазон символов от *0000 до *FFFD. "
                "Последние две [[pt:codepoint|позиции]] каждой плоскости, *FFFE и *FFFF, [[pt:noncharacter|выброшены]]. "
                "Из-за ограничений [[pt:utf16|UTF-16]] в{{_}}Юникоде семнадцать плоскостей."
             "<p>• Плоскость {{№|0}} называется '''[[pt:bmp|базовой многоязычной]]''' и содержит большинство символов живых языков. "
                    "В Юникоде{{_}}14.0 остаются 16{{_}}нераспределённых позиций.<br>"
                "• Плоскость {{№|1}} называется '''дополнительной многоязычной''' и содержит символы редких и мёртвых языков, а также технические. "
                    "Заполнена менее чем на {{%|50}}.<br>"
                "• Плоскость {{№|2}} называется '''дополнительной иероглифической''' и содержит только [[ps:Hani|китайские иероглифы]]. "
                    "Заполнена на {{%|90}}.<br>"
                "• Плоскость {{№|3}} называется '''третичной иероглифической''', содержит только китайские иероглифы, "
                    "и заполнена на {{%|10}}.<br>"
                "• Плоскости 4…0D в Юникоде{{_}}14.0 не{{_}}задействованы.<br>"
                "• Плоскость 0E называется '''дополнительной плоскостью специального назначения''' и содержит менее 500 специсмволов.<br>"
                "• Плоскости 0F и 10 отведены под [[pt:private|личное пользование]]." },
    { "private", EcTermCat::ENCODING,
      u8"символ для личного пользования", u8"private-use character",
        u8"На собственные нужды всех заинтересованных отведены 6400 символов E000…F8FF из [[pt:bmp|базовой плоскости]], "
                    "а также 131068 символов в двух [[pt:plane|плоскостях]] 0F и 10, кроме *FFFE и *FFFF, "
                    "которые [[pt:noncharacter|выброшены]]."
            "<p>Договорившись, люди могут закодировать в этих диапазонах всё, что хотят, в том числе символы, не{{_}}подлежащие "
                    "внесению в Юникод: товарные знаки, вымышленные алфавиты, редкие символы и письменности, "
                    "тэги оформления, знаки подстановки. "
                "Консорциум Юникода никак не{{_}}отвечает за это. "
                "Приведём лишь четыре распространённых применения личных символов:"
             "<p>• Производители ОС часто кодируют там свои эмблемы. "
                    "При этом конечные пользователи захватывают коды начиная с E000 и вверх, а разработчики ПО{{-}}"
                        "начиная с F8FF и вниз.<br>"
                "• Реестр искусственных письменностей (Under-ConScript Unicode Registry){{-}}более 60 "
                        "искусственных языков, письменностей из кино, литературы и игр. "
                    "Три из них{{-}}[[pk:101D0|Фестский диск]], [[ps:Shaw|алфавит Шоу]], [[ps:Dsrt|мормонский]]{{-}}проникли в Юникод.<br>"
                "• Standard Music Font Layout (SMuFL){{-}}открытый стандарт музыкальных шрифтов для сокращения [[pk:1D100|нотных]] PDF.<br>"
                "• Заявки в Юникод{{-}}блок ещё не{{_}}отдан, а шрифт-реализацию делать уже надо."
            "<p>Неофициально в личное пользование под управляющие и форматирующие символы берут [[pt:control|управляющие]]: "
                    "в большинстве случаев они не{{_}}могут находиться в тексте. "
                "Так, в компьютерной игре ''Doom'' (1993) символы с 1 по 4 служат для задания адресата сообщения." },

    ///// Algorithms ///////////////////////////////////////////////////////////

    { "bidi", EcTermCat::ALGORITHM,
      u8"двунаправленный текст", u8"bidirectional text",
        u8"Текст, содержащий хотя бы один символ с написанием «справа налево». "
                "Алгоритм обработки такого текста сложен, и опишем его в общих чертах."
            "<p>1. Разбить текст на независимые строки."
            "<p>2. Разбить каждую строку на вложенные отрезки: вся строка считается отрезком «слева направо», в неё вкладываются "
                    "отрезки «справа налево», в них отрезки «слева направо», и{{_}}т.д. "
                "В iOS существовала ошибка: из-за глубокой вложенности отрезков телефон перезагружался."
            "<p>3. Вычленить европейские числа, дать им направление слева направо."
            "<p>4. Дать направление остальным нейтральным символам. Отзеркалить, если они идут справа налево и отзеркаливаются."
            "<p>5. Исполнить протоколы более высокого уровня{{-}}например, выяснить направление таблиц."
            "<p>Например: «XX,YY 1,00&lt;2,00 XX» = "
                            "«ثث,حح"
                            " 1,00&lt;2,00 "
                            "ثث»" },
    { "normalization", EcTermCat::ALGORITHM,
      u8"нормализация", u8"normalization",
        u8"Преобразование строк для нежёсткого сравнения. "
                "В Юникоде существуют четыре варианта нормализации:"
            "<p>1. NFD{{-}}каноническая декомпозиция (разборка [[pt:precomposed|монолитных символов]], "
                    "слогов [[ps:Hang|хангыля]] на чамо (буквы), замена некоторых знаков вроде «ом» и «ангстрем», "
                    "приведение [[pt:combining|меток]] в стандартный порядок)."
            "<p>2. NFC{{-}}каноническая композиция (NFD, за ней сборка монолитных символов и слогов хангыля)."
            "<p>3. NFKD{{-}}совместимая декомпозиция (NFD + разборка [[pt:ligature|лигатур]], "
                "замена буквоподобных символов на буквы, индексов на цифры, полуширинных на стандартные "
                "и прочее)."
            "<p>4. NFKC{{-}}совместимая композиция (после NFKD идёт '''каноническая''' сборка символов, как в NFC)."
            "<p>Все четыре нормализации идемпотентны{{-}}NFD(NFD(s))=NFD(s) и{{_}}т.д. "
                "То есть вторая нормализация ничего не{{_}}сделает." },
    { "casefolding", EcTermCat::ALGORITHM,
      u8"свёртка регистра", u8"case folding",
        u8"Преобразование строки, используемое для (в основном) локаленезависимого сравнения строк без регистра. "
                "Бывает двух основных типов и одного дополнительного:<br>"
                "• Простая (без изменения длины): <big>ᾨ → ᾠ</big><br>"
                "• Полная (с раскрытием [[pt:ligature|лигатур]]): <big>ᾨ → ὠ ι</big><br>"
                "• Тюркская (специальные правила для турецкого и азербайджанского): <big>I → ı</big>"
            "<p>Обычно свёртка{{-}}это перевод в нижний регистр. "
                "Но для [[ps:Cher|чероки]] это перевод в верхний: заглавные буквы появились раньше."
            "<p>Свёртка идемпотентна: CF(CF(s))=CF(s), то есть вторая свёртка ничего не{{_}}сделает." },

    ///// Serialization ////////////////////////////////////////////////////////

    { "serialization", EcTermCat::SERIALIZATION,
      u8"сериализация", u8"serialization",
        u8"Метод преобразования данных (например, юникодного текста) в цепочку ''кодовых единиц''{{-}}байтов для [[pt:utf8|UTF-8]], "
                    "слов из двух байтов для [[pt:utf16|UTF-16]], слов из четырёх байтов для [[pt:utf32|UTF-32]], "
                    "символов ASCII для [[pt:utf7|UTF-7]] и [[pt:punycode|Punycode]]."
            "<p>Поскольку диски и каналы связи оперируют байтами, при передаче UTF-16/32 играет роль порядок байтов в слове. "
                "Соответственно появляются форматы UTF-16BE, UTF-32BE (начиная со старших байтов), "
                    "и UTF-16LE, UTF-32LE (начиная с младших)."
            "<p>[[pt:bom|Метка порядка байтов]] позволяет различать между этими форматами." },
    { "punycode", EcTermCat::SERIALIZATION,
      u8"Punycode", {},
        u8"Формат [[pt:serialization|сериализации]] короткой юникодной строки в цепочку печатных ASCII-символов. "
            "<p>Сначала записываются все ASCII-символы, затем после дефиса по сложной процедуре записывается, куда вставить "
                    "остальные символы.<br>"
                    "{{__}}München = Mnchen-3ya"
            "<p>Если строка состоит из одних ASCII-символов, в конце просто добавляется дефис:<br>"
                    "{{__}}test{{_}}={{_}}test-"
            "<p>Если ASCII-символов вообще нет, дефис в начале опускается:<br>"
                    "{{__}}правда{{_}}={{_}}80aafi6cg"
            "<p>Используется только в системе доменных имён Интернета, где экономия байтов и читаемость важнее эффективности. "
                "При этом к международным именам добавляется «xn--»:<br>"
                    "{{__}}münchen.example.com = xn--mnchen-3ya.example.com" },
    { "utf7", EcTermCat::SERIALIZATION,
      u8"UTF-7", {},
        u8"'''Unicode transformation format, 7-bit'''{{-}}"
                    "формат [[pt:serialization|сериализации]] Юникода в цепочку печатных [[pt:ascii|ASCII]]-символов. "
            "<p>52 буквы, 10 цифр и '(),-./:? записываются как есть. "
                "Иногда могут записываться как есть и другие ASCII-символы, кроме ~\\+. "
                "Знак + можно кодировать как +- (плюс-минус). "
                "Остальные символы кодируются в [[pt:utf16|UTF-16BE]], переводятся в Base64 (62='''+''', 63='''/''') "
                    "и забираются в символы +…- (например, <nobr>'''за''' = +BDcEMA-</nobr>)."
            "<p>Если после кода идёт любой символ, кроме символов Base64 и дефиса, заключительный дефис можно опустить. "
                "Если между двумя кодируемыми символами есть одинокий некодируемый, можно закодировать и его, "
                    "чтобы сэкономить байты."
            "<p>Формат UTF-7 не{{_}}обладает хорошими свойствами [[pt:utf8|UTF-8]] и даже не{{_}}однозначен: "
                    "один и тот же текст можно закодировать по-разному."
            "<p>Используется только в электронной почте, как альтернатива более громоздкому ''quoted-printable''. "
                "В протоколе IMAP для имён ящиков используется версия UTF-7, где вместо плюса '''+''' используется амперсанд '''&''', "
                    "63=запятая, заключительный дефис пишется всегда, и из символов ASCII кодируется только &. "
                "Такой UTF-7 уже однозначен, что важно для поиска. "
                "Во Всемирной паутине UTF-7 прямо запрещён: если обмануть браузер, чтобы он принял текст за UTF-7, "
                    "можно сделать XSS-атаку." },
    { "utf8", EcTermCat::SERIALIZATION,
      u8"UTF-8", {},
        u8"'''Unicode transformation format, 8-bit'''{{-}}формат [[pt:serialization|сериализации]] Юникода в цепочку байтов. "
                "Придуман в 1992 в операционной системе Plan{{_}}9 для совместимости с имеющимся интернет-ПО. "
                "Каждая [[pt:codepoint|кодовая позиция]] записывается одной из таких последовательностей:<br>"
                "• 0xxx.xxxx<br>"
                "• 110x.xxxx{{_}}10xx.xxxx<br>"
                "• 1110.xxxx{{_}}10xx.xxxx{{_}}10xx.xxxx<br>"
                "• 1111.0xxx{{_}}10xx.xxxx{{_}}10xx.xxxx{{_}}10xx.xxxx"
            "<p>Свойства UTF-8:<br>"
                "• Теоретически может записывать 2{{_}}млрд кодовых позиций последовательностями до 6 байт. "
                    "С ограничениями, которые накладывает на Юникод [[pt:utf16|UTF-16]], не{{_}}бывает последовательностей длиннее 4.<br>"
                "• Существует нестандартный формат UTF-8, где нулевой символ кодируется как C0{{_}}80, "
                    "отличая его от символа конца строки 00.<br>"
                "• Обратная совместимость: латинский текст неотличим от ASCII-текста. "
                    "Байт 7F [[pt:control|управляющий]], байта FF не{{_}}бывает, что даёт совместимость с терминальными протоколами.<br>"
                "• Самосинхронизирующийся: если добавить любые ошибочные байты, сохранившийся текст останется читаемым. "
                    "Поиск в корректном UTF-8 программой, не{{_}}понимающей UTF-8, также не{{_}}даст ложных срабатываний.<br>"
                "• Надёжное опознание: осмысленный текст в однобайтовой кодировке (если он не{{_}}чистый ASCII) или UTF-16 "
                    "невозможно спутать с UTF-8.<br>"
                "• Монотонный: программа, не{{_}}понимающая UTF-8, отсортирует тексты в порядке кодов."
            "<p>Как формат межпрограммного обмена используется широко, в том числе в Интернете. "
                "[[pt:bom|Метка порядка байтов]] EF{{_}}BB{{_}}BF. "
                "Как формат программного интерфейса используется в Linux, кроссплатформенных библиотеках вроде OpenSSL." },
    { "utf16", EcTermCat::SERIALIZATION,
      u8"UTF-16, суррогатная пара", u8"UTF-16, surrogate pair",
        u8"'''Unicode transformation format, 16-bit'''{{-}}формат [[pt:serialization|сериализации]] Юникода в цепочку двухбайтовых "
                    "машинных слов."
            "<p>Изначально Юникод задумывался как двухбайтовая кодировка, и запись текста двухбайтовыми символами называлась UCS-2. "
                "Прогрессивные языки и библиотеки тех времён, вроде Java и Windows NT, быстро приняли двухбайтовый символ, "
                    "это упрощало обработку текста. "
                "Но вскоре стало понятно, что двух байтов не{{_}}хватит, и в 1996, как компромисс, сделали кодировку с переменным "
                    "числом двухбайтовых слов, которую и назвали UTF-16.<br>"
                "• Символы 0…D7FF и E000…FFFF записываются как есть;<br>"
                "• Символы D800…DFFF запрещены;<br>"
                "• Символы от 10000 записываются '''суррогатной парой''':<br>"
                    "{{__}}1101.10yy.yyyy.yyyy{{_}}1101.11yy.yyyy.yyyy, где y=x−10000₁₆."
            "<p>Свойства UTF-16:<br>"
                "• Накладывает ограничения на ёмкость Юникода{{-}}2<big>²⁰</big>+2<big>¹⁶</big>−2<big>¹¹</big>=1.112.064 символа.<br>"
                "• Обратная совместимость: текст в [[pt:bmp|базовой плоскости]] без пользовательских символов неотличим от UCS-2. "
                    "Если рабочий язык лежит в базовой плоскости, а символы за её пределами не{{_}}обрабатываются (остаются или удаляются), "
                    "двухбайтовый символ вполне пригоден для обработки текста.<br>"
                "• Самосинхронизирующийся: если добавить любые ошибочные '''слова''', сохранившийся текст останется читаемым. "
                    "Поиск в UTF-16 программой, понимающей UCS-2, также не{{_}}даст ложных срабатываний. "
                    "Для байтов и однобайтовых кодировок это неверно;<br>"
                "• Ненадёжное опознание: без [[pt:bom|метки порядка байтов]] формат UTF-16 и порядок байтов определяется "
                    "по косвенным признакам;<br>"
                "• Немонотонный: программа, понимающая UCS-2, отсортирует тексты не{{_}}в порядке кодов."
            "<p>Как формат межпрограммного обмена используется в основном в двоичных файлах родом с Windows и Mac/Motorola "
                    "(например, шрифтах TrueType). "
                "Таким файлам метка порядка байтов не{{_}}нужна; в текстовых файлах она FE{{_}}FF для UTF-16BE и FF{{_}}FE для UTF-16LE. "
                "Как формат программного интерфейса используется в Windows, Java, Qt." },
    { "utf32", EcTermCat::SERIALIZATION,
      u8"UTF-32", {},
        u8"'''Unicode transformation format, 32-bit''', он же '''UCS-4'''{{-}}"
                    "формат [[pt:serialization|сериализации]] Юникода в цепочку четырёхбайтовых машинных слов. "
                    "Коды символов просто записываются один за другим."
            "<p>Свойства UTF-32:<br>"
                "• Кодировка постоянной длины. Поэтому она наиболее удобна для сложной обработки текста.<br>"
                "• Крайне громоздка: если в [[pt:utf8|UTF-8]] и [[pt:utf16|UTF-16]] максимальный размер символа{{-}}4 байта, "
                    "то в UTF-32 все символы 4-байтовые.<br>"
                "• Надёжно опознаётся по одиннадцати нулевым верхним битам."
            "<p>Как формат межпрограммного обмена практически не{{_}}используется из-за громоздкости. "
                "Но если нужно{{-}}[[pt:bom|метка порядка байтов]] 00{{_}}00{{_}}FE{{_}}FF для UTF-32BE "
                    "и FF{{_}}FE{{_}}00{{_}}00 для UTF-32LE. "
                "Как формат программного интерфейса используется в новых версиях Unix-подобных ОС, в том числе MacOS." },
    { "bom", EcTermCat::SERIALIZATION,
      u8"метка порядка байтов", u8"BOM = byte order mark",
        u8"Символ U+FEFF. Его можно (не обязательно) ставить в начале юникодного текстового файла, чтобы указать "
                    "формат [[pt:serialization|сериализации]]. "
                "Закодированной метка порядка байтов будет выглядеть так:<br>"
                "• EF{{_}}BB{{_}}BF{{-}}[[pt:utf8|UTF-8]];<br>"
                "• FE{{_}}FF{{-}}[[pt:utf16|UTF-16BE]] (начиная со старшего байта);<br>"
                "• FF{{_}}FE, дальше что угодно, кроме 00{{_}}00{{-}}UTF-16LE (начиная с младшего байта);<br>"
                "• 00{{_}}00{{_}}FE{{_}}FF{{-}}[[pt:utf32|UTF-32BE]];<br>"
                "• FF{{_}}FE{{_}}00{{_}}00{{-}}UTF-32LE."
            "<p>[[pt:utf7|UTF-7]] и [[pt:punycode|Punycode]] не{{_}}встречаются как формат текстовых файлов. "
                "UTF-32 встречается крайне редко."
            "<p>В двоичных файлах формат сериализации жёстко зафиксирован, и метка порядка байтов не{{_}}нужна." },

    ///// Writing styles ///////////////////////////////////////////////////////

    { "cursive", EcTermCat::WRITING_STYLE,
      u8"курсив, скоропись, беглое письмо", u8"cursive",
        u8"Стиль письма, когда нужно быстро написать что-то, особенно хрупким гусиным пером. "
                "Особенности варьируются от языка к языку, но в целом:<br>"
                "• Простая форма букв.<br>"
                "• Углы, кроме очень острых, скругляются.<br>"
                "• Пишущий старается не{{_}}поднимать перо{{-}}отсюда соединённые буквы, проход по одному штриху туда-сюда, "
                    "петли в выносных элементах.<br>"
                "• Буквы соединяются в [[pt:ligature|лигатуры]]."
            "<p>Не путать со шрифтом «курсив» ''(italic)'', который, впрочем, произошёл от беглого письма."
            "<p>Не путать со стенографией ''(shorthand)'', когда писец успевает за речью, "
                    "принося при этом жертвы: сложное обучение, разлинованная бумага, расшифровка в спокойной обстановке…" },
    { "minuscule", EcTermCat::WRITING_STYLE,
        u8"минýскульное письмо", u8"minuscule writing",
        u8"Стиль письма с низкой строкой и обилием выносных элементов. "
                "Использовался в средние века для экономии пергамента, распространён в [[ps:Latn|латинице]], [[ps:Geor|грузинском]], "
                    "[[ps:Grek|греческом]]. "
                "В латинице он превратился в строчные буквы{{-}}а грузиница является регистровой лишь формально."
            "<p>Если у письменности в принципе большие выносные элементы ([[ps:Lana|ланна]]), минускулом это называть не{{_}}принято." },

    ///// Other ////////////////////////////////////////////////////////////////

    { "abc", EcTermCat::OTHER,
      u8"алфавитный порядок", u8"collation",
        u8"Стандартный порядок для сортировки текстов. "
                "Впервые известен в [[ps:Ugar|угаритском]]."
            "<p>Происхождение порядка:<br>"
                "• [[ps:Latn|Латиница]], [[ps:Grek|греческий]], [[ps:Cyrl|кириллица]], [[ps:Arab|арабский]], [[ps:Geor|грузинский]], "
                    "[[ps:Armn|армянский]], [[ps:Hebr|иврит]], [[ps:Ethi|эфиопский]]{{-}}из [[ps:Phnx|финикийского]]. "
                    "Он, видимо, из более ранних семитских языков;<br>"
                "• [[ps:Runr|руны]] (''футарк''){{-}}неизвестно;<br>"
                "• Все [[pt:brahmic|брахмийские абугиды]], [[pt:cjk|ККЯ]] по произношению{{-}}фонологический, от глотки «к» до зубов «с»;<br>"
                "• ККЯ по [[ps:Hani|написанию]]{{-}}через так называемые ключи."
            "<p>Юникод записывает буквы в порядке, близком к алфавитному, однако никак не{{_}}кодифицирует правила алфавитной сортировки. "
                "Если алфавитный порядок изменится, страна сама меняет правила сортировки, не{{_}}советуясь с Юникодом." },
    { "mono", EcTermCat::OTHER,
      u8"знакоместо", u8"character cell",
        u8"Воображаемая клетка, на которые разбивается моноширинный текст. "
                "<p>Текст на «знакоместа» делится и в [[pt:cjk|ККЯ]], но в первую очередь термин относится "
                    "к пишущим машинкам, текстовым режимам компьютеров, алфавитно-цифровым принтерам." },
    { "inclusion", EcTermCat::OTHER,
      u8"инклюзивность и разнообразие", u8"inclusion and diversity",
        u8"Один из принципов [[pt:emoji|эмодзи]], появившийся, впрочем, не сразу. "
                "Это значит, что эмодзи должны поддерживать расовые, гендерные и прочие меньшинства{{-}}"
                    "насколько это возможно без комбинаторного взрыва. "
                "Так, семьи разного расового состава возможны, но не стандартизированы{{-}}это 4225{{_}}глифов. "
                "''(Вся библиотека Noto Emoji, используемая и в «Юникодии», на начало 2022 содержит 3427 глифов.)''"
            "<p>Изначально люди отображаются с нереалистичным цветом кожи{{-}}обычно жёлтым. "
                "Один из пяти цветов кожи даётся специальным модификатором: {{em|🏃}}+{{em|🏿}} ={{_}}{{em|🏃🏿}}."
            "<p>Большинство эмодзи бесполы; пол при желании добавляется как {{em|🧙}}+ZWJ+{{em|♂}}+VS16."
            "<p>Есть только два чисто женских эмодзи: {{em|🤱}}{{_}}кормление грудью и {{em|🧕}}{{_}}женщина в платке."
            "<p>В Юникоде{{_}}13 бесполыми сделали {{em|👰}}{{_}}невесту, {{em|🤵}}{{_}}человека в смокинге, "
                "{{em|🕴}}{{_}}левитирующего человека в костюме из шрифта ''Webdings'', {{em|👲}}{{_}}человека в китайской шапке, "
                "{{em|🧔}}{{_}}бородача."
            "<p>Люди с невидимой кожей{{-}}{{em|🏂}} сноубордист{{-}}также должны поддерживать модификаторы пола и цвета кожи, "
                    "даже если это никак не изменит глиф."
            "<p>Если эмодзи захватывает несколько человек, механизмы установки пола и цвета кожи задаются отдельно для каждого случая. "
                "Так, в кормлении грудью {{em|🤱}} кожа младенца скрыта, поцелуй {{em|💏}} собирается с нуля как "
                    "{{em|👩}}+ZWJ+{{em|❤}}+VS16+ZWJ+{{em|💋}}+ZWJ+{{em|👨}}."
            "<p>Порядок модификаторов в ZWJ-последовательностях: базовый символ, цвет кожи (без ZWJ), волосы, прочие цвета {{em|⬛}}, пол, "
                    "направление движения {{em|⬅}}+VS16. "
                "(Единственный стандартизованный эмодзи с цветом{{-}}чёрный кот {{em|🐈}}+ZWJ+{{em|⬛}})"
            "<p>Колобки {{em|😂}} не инклюзивны и всегда жёлтые." },
    { "tofu", EcTermCat::OTHER,
      u8"тофу", u8"tofu",
        u8"[[pt:character|Глиф]], отображаемый, если символа нет в шрифте. "
                "Обычно белый прямоугольник, отсюда название в честь японского соевого сыра, нарезаемого кубиками."
            "<p>В некоторых шрифтах бывают креативные тофу: прямоугольник с вопросительным знаком, крестом, "
                    "схематичным обозначением письменности."
            "<p>Часть символов Юникода объявлены [[pt:ignorable|игнорируемыми]]{{-}}не показывающимися как тофу."
            "<p>Комплект шрифтов «Noto», используемый в том числе и в «Юникодии», означает ''no{{_}}tofu{{-}}нет{{_}}тофу''." },
};


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
        return u8"Заливка F2 с поворотами 0…R5 изображена на символе "
               "U+1D9FF «голова»: иначе не{{_}}складывается в лигатуру. "
               "Копироваться будет без него.";
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
        if (v.synthIcon.subj < v.startingCp || v.synthIcon.subj > v.endingCp)
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

    // Save script INI
//    std::ofstream os("script.ini");
//    for (auto& v : scriptInfo) {
//        os << "[Script." << v.id << "]\n";
//        os << "Name=" << esc(v.locName) << '\n';
//        if (!v.flags.have(Sfg::NO_LANGS))
//            os << "Lang=" << esc(v.locLangs) << '\n';
//        if (v.time.needsCustomNote())
//            os << "Note=" << esc(v.locTimeComment) << '\n';
//        os << "Text=" << esc(v.locDescription) << '\n';
//        os << '\n';
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
    if (flags.have(Cfg::CUSTOM_CONTROL))
        return uc::DrawMethod::CUSTOM_CONTROL;
    if (flags.have(Cfg::SVG_EMOJI))
        return (block().flags.have(Bfg::NO_EMOJI))
                ? uc::DrawMethod::SAMPLE
                : uc::DrawMethod::SVG_EMOJI;
    if (isAbbreviated())
        return uc::DrawMethod::ABBREVIATION;
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


void uc::finishTranslation()
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
}
