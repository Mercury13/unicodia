#include "UcData.h"

// Qt
#include <QFontDatabase>
#include <QFontMetrics>

// Libs
#include "i_TempFont.h"
#include "u_Strings.h"
#include "u_Qstrings.h"
#include "u_Iterator.h"
#include "Skin.h"

#define NBSP "\u00A0"

using namespace std::string_view_literals;
uc::Cp* uc::cps[N_CHARS];
const QString uc::Font::qempty;

constexpr QChar ZWSP(0x200B);

// [+] any missing char is tofu (BUGGY)  [-] try smth from system
constexpr bool FORCE_TOFU = false;

constexpr auto STRATEGY_TOFU = static_cast<QFont::StyleStrategy>(
            QFont::PreferAntialias | QFont::PreferOutline | QFont::NoFontMerging
            | QFont::PreferQuality | QFont::PreferFullHinting);
constexpr auto STRATEGY_COMPAT = static_cast<QFont::StyleStrategy>(
            QFont::PreferAntialias | QFont::PreferOutline | QFont::PreferMatch
            | QFont::PreferFullHinting);
constexpr auto STRATEGY_DEFAULT = FORCE_TOFU ? STRATEGY_TOFU : STRATEGY_COMPAT;

constexpr auto STYLE_DEVA = "padding-top:10%"_sty;

constinit const uc::Font uc::fontInfo[] = {
    { FAM_DEFAULT, Ffg::FALL_TO_NEXT },                                         // Normal
      { FNAME_NOTO, Ffg::FALL_TO_NEXT },                                        // …1
      { "Segoe UI Emoji", Ffg::FALL_TO_NEXT },                                  // …2
      { FNAME_NOTOMATH, Ffg::FALL_TO_NEXT },                                    // …3
      { FNAME_NOTOSYM1, Ffg::FALL_TO_NEXT },                                    // …4
      { FNAME_NOTOSYM2, Ffg::FALL_TO_NEXT },                                    // …5
      { "Segoe UI Symbol", Ffg::FALL_TO_NEXT },                                 // …6  Let it be, system font
      { "Segoe UI Historic" },                                                  // …7  Let it be, system font
    { FNAME_NOTO },                                                             // Noto
    { FNAME_NOTOSYM2 },                                                         // Noto symbol2
    { FNAME_NOTOSYM2, Ffg::DESC_BIGGER },                                       // Noto symbol2 bigger
    { "Segoe UI Symbol" },                                                      // Symbol
    { "Segoe UI Historic" },                                                    // Historic
    { FAM_EMOJI "," FAM_DEFAULT ",Arial", Ffg::FALL_TO_NEXT  },                 // Punctuation — both are built-in
      { FNAME_NOTO },                                                           // …1
    { FNAME_NOTOMATH },                                                         // Math
    { FNAME_NOTOMUSIC, 110_pc },                                                // Music
    { FNAME_NOTOMUSIC, 150_pc },                                                // Music bigger
    { FNAME_NOTOMUSIC },                                                        // Music normal
    { "Gadugi" },                                                               // Gadugi
    { FNAME_DEJAVU },                                                           // DejaVu
    { FNAME_FUNKY, Ffg::DESC_BIGGER },                                          // Funky
    { FNAME_FUNKY, Ffg::DESC_BIGGER, 115_pc },                                  // Funky bigger
        //-----
    { "NotoSansAdlam-Regular.ttf" },                                            // Adlam
    { "NotoSerifAhom-Regular.ttf" },                                            // Ahom
    { "ScheherazadeNew-Regular.ttf" },                                          // Arabic
    { "ScheherazadeNew-Regular.ttf", Ffg::FALL_TO_NEXT },                       // Arabic Noto — Scheh has a few nice chars!
      { "NotoNaskhArabic-Regular.ttf" },                                        // …1, main font
    { "NotoSansImperialAramaic-Regular.ttf" },                                  // Aramaic
        // Two fonts OK, as they both are built-in
    { FAM_DEFAULT, Ffg::FALL_TO_NEXT },                                         // Armenian
      { "NotoSerifArmenian-Regular.ttf" },                                      // …1
    { "NotoSansAvestan-Regular.ttf" },                                          // Avestan
    { "NotoSansBalinese-Regular.ttf", Ffg::STUB_OFF, "padding-bottom: 12%;"_sty, 90_pc }, // Balinese
    { "NotoSansBamum-Regular.ttf", 110_pc },                                    // Bamum
    { "NotoSansBatak-Regular.ttf" },                                            // Batak
    { "NotoSerifDevanagari.ttf", Ffg::FALL_TO_NEXT, STYLE_DEVA, 110_pc },       // Vedic = Devanagari → Bengali
    { "NotoSerifBengali-Regular.ttf", 120_pc },                                 // Bengali
    { "NotoSansBhaiksuki-Regular.ttf", Ffg::DESC_BIGGER, 130_pc },              // Bhaiksuki
    { "NotoSansBrahmi-Regular.ttf" },                                           // Brahmi
    { "NotoSansBuginese-Regular.ttf" },                                         // Buginese
    { "NotoSansBuhid-Regular.ttf" },                                            // Buhid
    { "NotoSansCanadianAboriginal-Regular.ttf" },                               // Canadian aboriginal
    { "NotoSansCaucasianAlbanian-Regular.ttf" },                                // Caucasian Albanian
    { "NotoSansChakma-Regular.ttf", Ffg::DESC_BIGGER, "padding-bottom:12%;"_sty }, // Chakma
    { "NotoSansCham-Regular.ttf" },                                             // Cham
    { "NotoSansCherokee-Regular.ttf" },                                         // Cherokee
        // CJK chars are square, and there’s always not enough detail → bigger
    { "SimSun", Ffg::FALL_TO_NEXT, 120_pc },                                    // CJK
      { "SimSun-ExtB", Ffg::FALL_TO_NEXT, 120_pc },                             // …1
      { "BabelStoneHan.ttf", Ffg::FALL_TO_NEXT, 120_pc },                       // …2
      { "Microsoft YaHei", Ffg::FALL_TO_NEXT, 120_pc },                         // …3
      { "MS Gothic", Ffg::FALL_TO_NEXT, 120_pc },                               // …4 Japanese
      { "Malgun Gothic", Ffg::FALL_TO_NEXT, 120_pc },                           // …5 Korean
      { "Yu Gothic", Ffg::FALL_TO_NEXT, 120_pc },                               // …6 Japanese
      { FNAME_HANA_A, Ffg::FALL_TO_NEXT, 120_pc },                              // …7
      { FNAME_HANA_B, 120_pc },                                                 // …8
    { "Yu Gothic", Ffg::FALL_TO_NEXT, 120_pc },                                 // CJK compat
      { "MS Gothic", Ffg::FALL_TO_NEXT, 120_pc },                               // …1
      { FNAME_HANA_A, 120_pc },                                                 // …2
    { "Microsoft YaHei" },                                                      // CJK kanbun
    { "SimSun,Microsoft YaHei" },                                               // CJK structure
    { "NotoSansCoptic-Regular.ttf" },                                           // Coptic
    { "NotoSansCuneiform-Regular.ttf" },                                        // Cuneiform
    { "NotoSerifDevanagari.ttf", STYLE_DEVA, 110_pc },                          // Devanagari
    { "Segoe UI Emoji", Ffg::FALL_TO_NEXT },                                    // Dingbat
      { FNAME_NOTOSYM1, Ffg::FALL_TO_NEXT },                                    // …1
      { FNAME_NOTOSYM2, Ffg::FALL_TO_NEXT },                                    // …2
      { "Segoe UI Symbol", Ffg::FALL_TO_NEXT },                                 // …3
      { FNAME_FUNKY, Ffg::FALL_TO_NEXT },                                       // …4
      { "BabelStoneHan.ttf", 130_pc },                                          // …5
    { "NotoSerifDogra-Regular.ttf", Ffg::DESC_BIGGER },                         // Dogra
    { "NotoSansEgyptianHieroglyphs-Regular.ttf"},                               // Egyptian
    { "NotoSansElbasan-Regular.ttf"},                                           // Elbasan
    { "NotoSansElymaic-Regular.ttf"},                                           // Elymaic
    { "NotoSerifEthiopic-Regular.ttf" },                                        // Ethiopic
    { "Sylfaen", Ffg::FALL_TO_NEXT },                                           // Georgian Nuskhuri
      { FNAME_DEJAVU, Ffg::FALL_TO_NEXT },                                      // …1 — FALL to Georgian
    { "NotoSerifGeorgian-Regular.ttf" },                                        // Georgian
    { "NotoSansGlagolitic-Regular.ttf" },                                       // Glagolitic
    { "Segoe UI Historic", Ffg::FALL_TO_NEXT },                                 // Gothic
      { FNAME_HANA_A },                                                         // …1
    { "NotoSansGrantha-Regular.ttf", Ffg::DESC_BIGGER | Ffg::CELL_SMALLER,
            "padding-top:10%; padding-bottom:12%;"_sty },                       // Grantha
    { "NotoSerifGujarati-Regular.ttf", 110_pc },                                // Gujarati
    { "NotoSansGurmukhiUI-Regular.ttf", 110_pc },                               // Gurmukhi
    { "NotoSansHanunoo-Regular.ttf" },                                          // Hanunoo
    { "NotoSansHatran-Regular.ttf" },                                           // Hatran
    { "NotoSerifHebrew-Regular.ttf" },                                          // Hebrew
    { "HanaMinA.ttf", Ffg::DESC_STD, 110_pc },                                  // Hentaigana
    { "NotoSansJavanese-Regular.ttf" },                                         // Javanese
    { "NotoSansKaithi-Regular.ttf" },                                           // Kaithi
    { "NotoSerifKannada-Regular.ttf", Ffg::LIGHT | Ffg::DESC_BIGGER, 110_pc },  // Kannada
    { "NotoSansKayahLi-Regular.ttf" },                                          // Kayah Li
    { "NotoSansKharoshthi-Regular.ttf", Ffg::DESC_BIGGER },                     // Kharoshthi
    { "BabelStoneKhitanSmallLinear.ttf", 115_pc },                              // Khitan small
    { "NotoSansKhmerUI-Regular.ttf", 110_pc },                                  // Khmer — initial Noto loses umlauts
    { "NotoSansKhojki-Regular.ttf", Ffg::DESC_BIGGER },                         // Khojki
    { "NotoSansKhudawadi-Regular.ttf" },                                        // Khudawadi
    { "NotoSansKR-Regular.otf", Ffg::FALL_TO_NEXT | Ffg::STUB_OFF, 110_pc },    // Korean:  we use it for well-known chars, no need to rename
      { "Yu Gothic", Ffg::FALL_TO_NEXT, },                                      // …1 — for P1 Katakana
      { FNAME_HANA_A, Ffg::FALL_TO_NEXT },                                      // …2 HanaMin
      { "BabelStoneHan.ttf" },                                                  // …2 U14
    { "NotoSansTaiTham-Regular.ttf" },                                          // Lanna
    { "sengbuhan.ttf", "padding-top: 12%;"_sty },                               // Lao
    { "NotoSansLepcha-Regular.ttf" },                                           // Lepcha
    { "NotoSansLimbu-Regular.ttf" },                                            // Limbu
    { "NotoSansLinearA-Regular.ttf" },                                          // Linear A
    { "NotoSansLinearB-Regular.ttf" },                                          // Linear B
    { "NotoSansLisu-Regular.ttf" },                                             // Lisu
    { "NotoSansMahajani-Regular.ttf" },                                         // Mahajani
    { "MakasarGraphite.ttf", Ffg::STUB_OFF },                                   // Makasar
    { "NotoSansMalayalamUI-Light.ttf", Ffg::LIGHT, 110_pc },                    // Malayalam
    { "NotoSansMandaic-Regular.ttf" },                                          // Mandaic
    { "NotoSansManichaean-Regular.ttf" },                                       // Manichaean
    { "NotoSansMeeteiMayek-Regular.ttf", Ffg::DESC_BIGGER },                    // Meetei Mayek
    { "NotoSansMeroitic-Regular.ttf" },                                         // Meroitic
    { "NotoSansModi-Regular.ttf", Ffg::DESC_BIGGER },                           // Modi
    { "NotoSansMongolian-Regular.ttf" },                                        // Mongol
    { "NotoSansMultani-Regular.ttf" },                                          // Multani
    { "NotoSansMyanmar-Regular.ttf" },                                          // Myanmar
    { "NotoSansNabataean-Regular.ttf" },                                        // Nabataean
    { "NotoSansNewa-Regular.ttf", Ffg::DESC_BIGGER },                           // Newa
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
    { "NotoSansOriyaUI-Regular.ttf", Ffg::DESC_BIGGER, 120_pc },                // Oriya
    { "NotoSansPalmyrene-Regular.ttf" },                                        // Palmyrene
    { "Microsoft PhagsPa" },                                                    // Phags-Pa
    { "NotoSansPhoenician-Regular.ttf" },                                       // Phoenician
    { "Segoe UI Emoji", Ffg::FALL_TO_NEXT },                                    // Playing cards
      { FNAME_NOTOSYM2, 150_pc },                                               // …1 = Phaistos disc
    { "NotoSansPsalterPahlavi-Regular.ttf" },                                   // Psalter Pahlavi
    { "NotoSansRejang-Regular.ttf", Ffg::DESC_BIGGER },                         // Rejang
    { "NotoSansHanifiRohingya-Regular.ttf", Ffg::DESC_BIGGER },                 // Rohingya (Hanifi)
    { "NotoSansRunic-Regular.ttf" },                                            // Runic
    { "NotoSansSamaritan-Regular.ttf" },                                        // Samaritan
    { "NotoSansSaurashtra-Regular.ttf" },                                       // Saurashtra
    { "NotoSansSharada-Regular.ttf" },                                          // Sharada
    { "NotoSansSiddham-Regular.ttf", Ffg::DESC_BIGGER },                        // Siddham
    { "NotoSansSignWriting-Regular.ttf", Ffg::STUB_OFF | Ffg::DESC_BIGGER, 145_pc }, // SignWriting
    { "NotoSansSinhala-Light.ttf", Ffg::LIGHT },                                // Sinhala
    { "NotoSansIndicSiyaqNumbers-Regular.ttf" },                                // Siyaq Indic
    { "NotoSansSogdian-Regular.ttf" },                                          // Sogdian
    { "NotoSansSundanese-Regular.ttf" },                                        // Sundanese
    { "NotoSansSylotiNagri-Regular.ttf" },                                      // Syloti Nagri
    { "SertoAntochBible_2020_Release.ttf", 120_pc },                            // Syriac
    { "NotoSansTagalog-Regular.ttf", Ffg::DESC_BIGGER },                        // Tagalog
    { "NotoSansTagbanwa-Regular.ttf" },                                         // Tagbanwa
    { "NotoSansTakri-Regular.ttf", Ffg::DESC_BIGGER },                          // Takri
    { "Microsoft Tai Le" },                                                     // Tai Le
    { "Microsoft New Tai Lue" },                                                // Tai Lue (new)
    { "NotoSansTaiViet-Regular.ttf" },                                          // Tai Viet
    { "Nirmala UI,Latha" },                                                     // Tamil
    { "NotoSansTamilSupplement-Regular.ttf" },                                  // Tamil supplement
    { "NotoSerifTangut-Regular.ttf", 125_pc },                                  // Tangut
    { FAM_DEFAULT, Ffg::FALL_TO_NEXT | Ffg::ALTERNATE },                        // Technical
      { "Segoe UI Emoji", Ffg::FALL_TO_NEXT },                                  // …1
      { FNAME_NOTOSYM1, Ffg::FALL_TO_NEXT },                                    // …2
      { FNAME_NOTOMATH, Ffg::FALL_TO_NEXT },                                    // …3
      { FNAME_NOTOSYM2, Ffg::FALL_TO_NEXT, },                                   // …4
      { FNAME_FUNKY, Ffg::FALL_TO_NEXT },                                       // …5
      { FNAME_DEJAVU },                                                         // …6
    { "NotoSansTeluguUI-Light.ttf", Ffg::LIGHT, 110_pc },                       // Telugu
    { "MV Boli", 110_pc },                                                      // Thaana
    { "Leelawadee,Leelawadee UI", "padding-bottom:10%;"_sty, 110_pc },          // Thai
        // Tibetan chars are VERY high → 90%
    { "NotoSerifTibetan-Regular.ttf", 90_pc },                                  // Tibetan
    { "NotoSansTifinagh-Regular.ttf" },                                         // Tifinagh
    { "NotoSansTirhuta-Regular.ttf", Ffg::DESC_BIGGER },                        // Tirhuta
    { FNAME_NOTOSYM2, Ffg::FALL_TO_NEXT },                                      // Umlaut symbol
      { FNAME_NOTO },                                                           // …1
    { "Ebrima" },                                                               // Vai
    { "NotoSansWarangCiti-Regular.ttf", Ffg::DESC_BIGGER },                     // Warang Citi
    { "NotoSerifYezidi-Regular.ttf", 110_pc },                                  // Yezidi
    { "Microsoft Yi Baiti", 120_pc },                                           // Yi
    { "NotoSansZanabazarSquare-Regular.ttf" },                                  // Zanabazar square
};

static_assert (std::size(uc::fontInfo) == static_cast<size_t>(uc::EcFont::NN));

constinit const uc::LangLife uc::langLifeInfo[static_cast<int>(EcLangLife::NN)] {
    { {} },
    { u8"живая ''(основным языкам ничего не угрожает, как кириллица)''" },
    { u8"в опасности{{-}}из-за языка ''(как чероки)''" },
    { u8"в опасности{{-}}из-за конкуренции письменностей ''(как ланна)''" },
    { u8"мёртвая ''(скоро потеряет/недавно потеряла носителей, как нюй-шу)''" },
    { u8"историческая ''(давно мертва, как глаголица)''" },
    { u8"религиозная ''(мертва в светском обороте, как балийская)''" },
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


const uc::Version uc::versionInfo[static_cast<int>(uc::EcVersion::NN)] {
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
    //{ "14.0", 2021 },       //check for equal number
};


constexpr auto CT_NOCHAR =
        u8"Зарезервированы как отсутствующие (''non-characters'') 66 позиций: *FFFE и *FFFF во всех семнадцати плоскостях, "
                "а также FDD0…FDEF. Их никогда не будет в Юникоде."
            "<p>Эти коды '''можно''' использовать как коды ошибок, знаки подстановки, «утяжелитель» при сортировке "
                    "или что-то другое, внутреннее для программы, перекодировать в [[pt:utf8|UTF-8]] или [[pt:utf16|UTF-16]]. "
                "Если программа никак не использует данный код, она может обращаться с ним как с обычным неизвестным символом."
            "<p>Их '''нельзя''' вводить с клавиатуры, использовать в межпрограммном обмене. "
                "Благодаря тому, что символа FFFE нет в Юникоде, работает [[pt:bom|метка порядка байтов]]."sv;

constinit const std::u8string_view uc::TX_NOCHAR = CT_NOCHAR;

constexpr auto CT_CONTROL =
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
            "• экранирование управляющего 10{{-}}следующий символ [[pt:graphic|графический]], а не управляющий;<br>"
            "• удаление 7F{{-}}исправление ошибки на перфоленте (пробивают все семь дорожек, такой символ не подлежит считыванию). "
                "В некоторых терминалах эту же роль играл FF, из-за чего [[ps:Cyrl|русскую]] «я» заменяли заглавной "
                    "на файл-серверах, в играх жанра MUD."
        "<p>Все 65 символов из категории «Управляющие» исторические. "
            "Символы, влияющие на отображение текста, называются [[pt:format|форматирующими]]."sv;

constexpr auto CT_FORMAT =
    u8"Невидимый символ, использующийся для управления различными алгоритмами обработки текста:<br>"
            "• разбивает текст на строки: 00AD{{-}}мягкий перенос;<br>"
            "• участвует в [[pt:ligature|лигатурах]] или, наоборот, разрывает их: 200D{{-}}нулевой соединитель;<br>"
            "• задаёт направление письма: 2066{{-}}изолировать участок, идущий слева направо."sv;

constexpr auto CT_SPACE =
    u8"Символ, призванный оставить место. "
            "В хорошо проработанной типографике пустого места столько, сколько нужно: "
                "146%{{-}}мало, 146{{_}}%{{-}}много, {{%|146}}{{-}}самое то. "
            "Потому и пробелы бывают разные. "
            "Некоторые из них неразрывные: не{{_}}расширяются при выключке, не{{_}}переносятся на другую строку."
        "<p>Пробелы обычно никак не отображаются и только передвигают курсор. "
            "Но если включено подчёркивание, или если письменность опирается на общую черту ([[ps:Ogam|огамическая]]), "
                "пробелы могут быть видны."
        "<p>В «Юникодии» пробелы обведены точно по габаритам кегельной площадки{{-}}"
            "так что узость обманчива. Часто для отступа хватает тончайшего пробела U+200A."
        "<p>Все пробелы являются [[pt:spacing|протяжёнными]] символами. "
            "Пробелы нулевой ширины, которые не оставляют места, отнесены не к пробелам, а к [[pt:format|форматирующим]]."sv;

constexpr auto CT_ASCII =
    u8"'''American Standard Code for Information Interchange (ASCII)'''{{-}}разработанная в США в 1963 семибитная кодировка символов. "
            "Ширину в 7 бит выбрали как компромисс для передачи данных по телеграфу, "
                "к тому же ASCII легко обрезать до 6 бит, оставив символы 40…5F и 20…3F. "
            "'''Базовая латиница''' Юникода совпадает с ASCII{{-}}так что английский юникодный текст читается, даже если кодировка сбита."
        "<p>Первые 32 символа, 00…1F, [[pt:control|управляющие]], и сегодня немногие из них действительно используются. "
            "Затем идут 95 печатных символов. "
            "Многие из них с пишущих машинок: например, чтобы пропечатать À, нужно напечатать A, вернуть каретку "
                "и напечатать обратный апостроф. "
            "Последний символ, 7F=DEL, также управляющий, родом с перфолент{{-}}отверстия, пробитые во всех дорожках, "
                "говорят, что символ выбит ошибочно и не подлежит считыванию."
        "<p>Существовало множество национальных вариантов ASCII: британцы заменяли решётку # на фунт £, "
                "СССР доллар $ на монету ¤, немцы [\\]{|} на умляуты ÄÖÜäöü, японцы обратную косую черту \\ на иену ¥…"
        "<p>Оставшиеся 128 символов могли заполняться чем угодно. Одна из таких 8-битных кодировок, [[pt:latin1|латиница-1]], "
                "также вошла в Юникод."sv;

constexpr auto CT_LATIN1 =
    u8"Латиница-1{{-}}второе название 8-битной кодировки '''ISO{{_}}8859-1''', появившейся в 1987 году "
                "и расширяющей [[pt:ascii|ASCII]] до европейских языков."
        "<p>Первые 32 символа, 80…9F, [[pt:control|управляющие]] для совместимости с 7-битными кодировками: "
                "почтовые программы иногда теряли верхний бит, и не хотелось бы в этих позициях ставить печатные символы. "
            "Сейчас эти символы не применяются по назначению и служат [[pt:private|личными]]. "
            "Так, кодировка Windows-1252{{-}}это та же латиница-1, только вместо управляющих символов типографские. "
            "Дошло до того, что в стандартах Всемирной паутины написано: если страница просит кодировку ISO{{_}}8859-1, "
                "отобразить сайт в Windows-1252."
        "<p>Среди букв видны символы × и ÷, изначально там должен быть Œœ."sv;


constexpr auto CT_MEROITIC =
    u8"Мероитская письменность применялась в Нубии и Судане для записи местных языков до VII{{_}}века. "
            "Имеет две формы: иероглифическую и курсивную. "
            "Иероглифическая форма происходит от [[ps:Egyp|египетских иероглифов]] и писалась справа налево по столбцам; "
                "известны только надписи, высеченные на монументах. "
            "Курсивная{{-}}от египетского демотического письма и писалась справа налево, {{%|90}} всех надписей."
        "<p>Существует более 2000 надписей. "
            "Алфавит расшифрован в начале XX{{_}}века, но язык во многом непонятен."sv;

constexpr auto CT_SOGDIAN =
    u8"Согдийская письменность происходит из [[ps:Syrc|сирийской]], появилась ≈100. "
            "Предок староуйгурской, а из неё пошла [[ps:Mong|старомонгольская]]. "
            "Употреблялась до 1200."
        "<p>В Юникоде закодированы два варианта согдийского: старый (по образцу ≈300) и новый "
            "(по образцу буддийских рукописей с VI{{_}}века)."
        "<p>Видно, как монгольское письмо стало вертикальным: согдийцы иногда заваливали текст на 90° влево, "
                "и именно такой вариант позаимствовали уйгуры."sv;


constinit const uc::Category uc::categoryInfo[static_cast<int>(uc::EcCategory::NN)] {
    { UpCategory::CONTROL,      "Cc",     u8"Управляющий", CT_CONTROL },
    { UpCategory::FORMAT,       "Cf",     u8"Форматирующий", CT_FORMAT },
    // PRIVATE_USE,     -- unused as Unicodia has nothing to tell
    // SURROGATE,       -- unused as Unicodia has nothing to tell
    { UpCategory::LETTER,       "Ll",     u8"Буква/строчная",
            u8"Письмо «[[pt:minuscule|минускул]]», появившееся в раннее средневековье для экономии дорогого пергамента, "
                "превратилось в строчные буквы и сделало латиницу очень выразительным алфавитом." },
    { UpCategory::LETTER,       "Lm",     u8"Буква/модифицирующая",
            u8"Такие «недобуквы», приделывающие к букве оттенки смысла, чаще всего встречаются в фонетике. "
                u8"Так, модифицирующей буквой является штрих мягкости xʹ, в отличие от похожего математического штриха x′." },
    { UpCategory::LETTER,       "Lo",     u8"Буква/другая",
            u8"Символы различных письменностей, где нет деления на заглавные и строчные буквы. "
                "Например, [[ps:Hebr|иврит]], [[ps:Arab|арабская вязь]] и [[ps:Hani|китайские иероглифы]]. Также особые символы вроде "
                    "мужского/женского порядкового знака романских языков (1º\{{-}}первый, 1ª{{-}}первая)." },
    { UpCategory::LETTER,       "Lt",     u8"Буква/смешанный регистр",
            u8"Символы-диграфы, состоящие из заглавной и строчной букв. "
                    "Так, в хорватской латинице есть диграф ǈ. "
                "Сербский аналог Љ не является диграфом и потому смешанного регистра не имеет." },
    { UpCategory::LETTER,       "Lu",     u8"Буква/заглавная",
            u8"Те письменности, которые приняли маленькие (строчные) буквы, буквы старой формы стали "
                "называть большими, заглавными или прописными." },
    { UpCategory::MARK,         "Mc",     u8"Метка/протяжённая",
            u8"Протяжённые (обладающие шириной) комбинирующие метки встречаются в некоторых "sv
                "языках Юго-Восточной Азии: деванагари, бенгальском, каннаде, хангыле…" },
    { UpCategory::MARK,         "Me",     u8"Метка/охватывающая",
            u8"Охватывающие метки используются в древнерусских буквенных цифрах (А҈). "
                "Также существуют охватывающие квадрат, круг и другие фигуры. "
                "Ни один проверенный типографский движок на Windows 10 20H2 (GDI, Cairo, Skia) не поддерживает подобные символы идеально, "
                    "но на хороших шрифтах вроде DejaVu результат очень неплох." },
    { UpCategory::MARK,         "Mn",     u8"Метка/непротяжённая",
            u8"Непротяжённые метки (например, знак ударе́ния) устроены как символы нулевой ширины, отсюда название. "
                "В хороших шрифтах дизайнер вручную помещает метки на наиболее распространённые буквы. "
                "Но если подобной комбинации не предусмотрели (8́), символы накладываются как попало, и результат обычно плох." },
    { UpCategory::NUMBER,       "Nd",     u8"Числовой/десятичный",
            u8"Люди считают десятками, поскольку у них десять пальцев. Слова «цифра» и «палец» во многих языках близки. "
                "Изобретённая в Индии позиционная система счисления используется всем миром, однако "
                    "в языках Ближнего Востока и Юго-Восточной Азии цифры бывают причудливые." },
    { UpCategory::NUMBER,       "Nl",     u8"Числовой/буквенный",
            u8"Римские, китайские и другие цифры, основанные на буквах. Древнерусские также основаны на буквах, "
                    "но в Юникоде для этого используются обычные А, Б, Г… с комбинирующими метками." },
    { UpCategory::NUMBER,       "No",     u8"Числовой/другой",
            u8"Архаичные системы счисления, [[pt:precomposed|монолитные]] дроби, верхние и нижние индексы, цифры в кругах, "
                    "сокращения для больших чисел и другие цифровые символы." },
    { UpCategory::PUNCTUATION,  "Pc", u8"Знак препинания/соединитель",
            u8"Небольшая группа символов. Наиболее известный из них{{-}}знак подчёркивания." },
    { UpCategory::PUNCTUATION,  "Pd",     u8"Знак препинания/черта",
            u8"Дефисы и тире. Минус также является чертой, но отнесён к математическим знакам." },
    { UpCategory::PUNCTUATION,  "Pe",     u8"Знак препинания/закрывающая скобка",
            u8"Скобки отнечены к отдельной категории, потому что играют важную роль в двунаправленном алгоритме." },
    { UpCategory::PUNCTUATION,  "Pf",     u8"Знак препинания/конечный",
            u8"В основном кавычки." },
    { UpCategory::PUNCTUATION,  "Pi",     u8"Знак препинания/начальный",
            u8"В основном кавычки." },
    { UpCategory::PUNCTUATION,  "Po",     u8"Знак препинания/другой",
            u8"Точка, запятая, процент, маркер списка и многое что ещё." },
    { UpCategory::PUNCTUATION,  "Ps",     u8"Знак препинания/открывающая скобка",
            u8"Скобки отнечены к отдельной категории, потому что играют важную роль в двунаправленном алгоритме." },
    { UpCategory::SYMBOL,       "Sc",     u8"Символ/валютный",
            u8"Валютный символ{{-}}важная часть имиджа страны, и даже у монголов есть тугрик <font size='+1'>₮</font>, напоминающий могильный крест. "
                "Артемий Лебедев в конце 1990‑х говорил, что рублю не нужен особый знак, "
                    "но впоследствии именно его пиар сделал знак <font size='+1'>₽</font> официальным." },
    { UpCategory::SYMBOL,       "Sk",     u8"Символ/модифицирующий",
            u8"Символы вроде крышки ^, внешне совпадающие с [[pt:combining|комбинирующими]] метками, но лишённые особых свойств. "
                    "А также некоторые другие символы."
                "<p>Многие из таких символов родом с пишущих машинок: чтобы нарисовать, например, ударение, надо было "
                        "вернуть каретку на шаг назад и выбить ударение в том же [[pt:mono|знакоместе]]. "
                    "Литера рисовалась так, чтобы один символ годился на роль апострофа и ударения.", },
    { UpCategory::SYMBOL,       "Sm",     u8"Символ/математический",
            u8"Изначально для математики использовались несколько разных систем, наиболее известные из них{{-}}TᴇX (читается «тех») "
            "и MathType (он же ''Microsoft Equation''). Юникод даёт надежду, что в компьютерной математике появится какая-то стандартизация{{-}}"
            "а также ограниченная возможность писать формулы в системах общего назначения вроде интернет-форумов." },
    { UpCategory::SYMBOL,       "So",     u8"Символ/другой",
            u8"Юникод содержит множество разных символов, в том числе коммерческие, технические и [[pt:emoji|эмодзи]]." },
    { UpCategory::SEPARATOR,    "Zl",     u8"Разделитель/строк",
            u8"Единственный символ 2028" },
    { UpCategory::SEPARATOR,    "Zp",     u8"Разделитель/абзацев",
            u8"Единственный символ 2029" },
    { UpCategory::SEPARATOR,    "Zs",     u8"Разделитель/пробел", CT_SPACE },
    //{ u8"Ошибка"sv },     //check for equal number
};


constinit const uc::Script uc::scriptInfo[] {
    { "Zyyy", QFontDatabase::Any,
        EcScriptType::NONE, EcLangLife::NOMATTER, EcWritingDir::NOMATTER, EcContinent::NONE,
            u8"Нет", {}, {}, u8"Символы вне письменности." },
    // Adlam OK, W10 has, but placement of umlauts + RTL = ??? → better Noto
    { "Adlm", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::RTL, EcContinent::AFRICA,
        u8"Адлам", u8"конец 1980-х",
        u8"фулá ''(семейство языков Западной Африки)''"sv,
        u8"Фулá{{-}}кочевники, антропологически средние между европейцами и неграми, и самые ярые проповедники ислама "
                "в западной Африке."
            "<p>Алфавит придуман братьями Ибрагима и Абдулайе Барри, чтобы лучше передавать языки фулá, чем латиница или арабский. "
                "Новая письменность прижилась, и её учат в школах Гвинеи, Либерии и других стран, локализован Android."
            "<p>Алфавит назван по первым четырём буквам: A, D, L, M. "
                "Эти буквы означают «Alkule Dandayɗe Leñol Mulugol»{{-}}«алфавит, защищающий народы от исчезновения»."
            "<p>До этого были ещё две неудачных попытки придумать письменность фулá, не описанных в Юникоде: "
                "фула Дита (1936, вдохновлена доарабской культурой) и фула Ба (1963). "
                "Иногда использовался волоф{{-}}сильно видоизменённый [[ps:Arab|арабский]] шрифт. "
                "Основная же письменность{{-}}[[ps:Latn|латиница]].",
                EcFont::ADLAM },
    // Caucasian Albanian OK, W10 none, installed Google Noto
    { "Aghb", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::EUROPE,
        u8"Агванский (Кавказская Албания)", u8"II век до н.э.",
        u8"агванский ''(язык Кавказской Албании)''",
        u8"Никакого отношения к балканской Албании страна не имеет, и вообще самоназвание неизвестно: "
                "''Албания''{{-}}название греческое, ''Агванк''{{-}}современное армянское. "
                "Заселена людьми, говорившими на лезгинских языках. "
                "С I{{_}}века{{bc}} имела сильное царство. "
                "В 461 завоёвана персами, в VII{{_}}веке снова получила свободу. "
                "С X{{_}}века часть Албании тюркизировалась и преобразовалась в Ширван (современный Азербайджан) и Карабах. "
                "Другие части участвовали в этногенезе армян, грузин Кахетии, дагестанцев. "
                "Поскольку страна захватывает современную Россию и Грузию, условно отнесена к Европе."
            "<p>Считается, что письменность создал Месроп Маштоц, создатель [[ps:Armn|армянского]] алфавита{{-}}впрочем, это не так: "
                    "Маштоц жил 362–440, и непонятно, какова была его роль. "
                "Упоминалась с конца XIX века. Окончательно открыта в 1937 советскими арменоведами. "
                "Первая расшифровка вышла в 2009.",
                EcFont::CAUCASIAN_ALBANIAN },
    // Ahom OK, installed Google Noto font
    { "Ahom", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::REVIVED, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Ахом", u8"XIII век",
        u8"тайско-ахомский",
        u8"Тайцы, переселившиеся в долину реки Брахмапутра, создали письменность на основе тогдашней индийской абугиды. "
                "К XIX веку язык окончательно заместился ассамским с бенгальским письмом. Возрождается с 1954 года, "
                "в письменность добавлены десятичные цифры, не имеющие исторического обоснования.",
                EcFont::AHOM },
    // Arabic OK, three fonts, incl. Windows Amiri, Google Noto and SIL Sheherazade
    { "Arab", QFontDatabase::Arabic,
        EcScriptType::CONSONANT, EcLangLife::ALIVE, EcWritingDir::RTL, EcContinent::ASIA,
        u8"Арабский", u8"IV—VI век",
        u8"арабский, персидский, урду, уйгурский, пуштунский…",
        u8"Письменность развилась из [[ps:Armi|арамейской]] через [[ps:Syrc|сирийскую]] и/или [[ps:Nbat|набатейскую]]. "
                "Арабский язык тесно связан с исламом; на этом языке написан Коран (610–632). "
                "Арабский халифат насаждал как ислам, так и вязь. "
                "Многие исламские народы (турки, казахи, башкиры) пользовались арабицей до начала XX{{_}}века."
            "<p>Компьютерная арабица осложняется написанием арабских букв: у каждой есть обособленная, начальная, средняя и конечная форма. "
                "В обычном тексте предпочтительнее «общая» форма буквы, подстраивающаяся под положение в слове. "
                "Но если нужна конечная форма в обособленной букве, в Юникоде есть и «жёсткие» варианты.",
                EcFont::ARABIC },
    // Imperial Aramaic OK, because of sheer importance install Google Noto
    { "Armi", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::ASIA,
        u8"Имперский арамейский", u8"IX–VII в. до н.э.",
        u8"имперский арамейский ''(также канцелярский арамейский{{-}}язык Персии 500—329{{bc}})''",
        u8"На основе арамейского в Ахеменидской империи создали книжный язык, "
                    " кодифицированный настолько, что крайне сложно опознать время и место конкретного документа. "
                "С завоеванием Персии Александром Македонским началась фрагментация языка и дальнейшее формирование "
                    "[[ps:Syrc|сирийских]] языков (предков [[ps:Arab|арабского]]) "
                    "и иудейских (предков [[ps:Hebr|иврита]]).",
                EcFont::ARAMAIC },
    // Armenian OK, Cambria+Noto is more than nice!
    { "Armn", QFontDatabase::Armenian,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Армянский", u8"405",
        u8"армянский",
        u8"Изобретён учёным и священником Месропом Маштоцем (362–440). Непонятно, брался ли какой-то алфавит за основу "
                "(возможно, несохранившиеся древнеармянские буквы). Алфавит тесно связан с распространением христианства в Армении. "
                "В XII веке добавились буквы {{sm|Օ}} и {{sm|Ֆ}}."
            "<p>Считается, что армянская литература богаче среднеперсидской (доисламской), потому что армянский алфавит проще "
                "[[ps:Mani|манихейской]] вязи.",
                EcFont::ARMENIAN },
    // Avestan OK, installed Google Noto
    /// @todo [link, block] Phaistos disc
    /// @todo [future, link] wait for book Pahlavi
    { "Avst", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::SACRED, EcWritingDir::RTL, EcContinent::ASIA,
        u8"Авестийский", u8"≈400",
        u8"авестийский, среднеперсидский",
        u8"Алфавит связан с зороастризмом и его священным текстом{{-}}Авестой. "
                "Около 400 в сасанидской Персии для кодификации Авесты создали специальный алфавит "
                    "на основе книжного пехлеви (письма среднеперсидских рукописей). "
                "К тому времени зороастризму было много веков и язык был малопонятен, а для действенности молитв важно точное повторение{{-}}"
                    "так что алфавит снабдили настоящими гласными, позаимствованными из других письменностей."
            "<p>При арабах зороастрийцы стали переселяться в Индию, "
                    "а с XII{{_}}века писали на авестийском местным письмом, традиционно [[ps:Gujr|гуджарати]]. "
                "Самый известный из таких зороастрийцев{{-}}Фредди Меркьюри (правда, его родителей судьба занесла аж на Занзибар)."
            "<p>В 2000 древние письменности разделили на пять категорий: "
                    "A (используемые{{-}}[[ps:Copt|коптский]], [[ps:Runr|руны]]), "
                    "B1 (изученные [[pt:character|кодируемые]]{{-}}[[ps:Xpeo|персидская клинопись]]), "
                    "B2 (изученные некодируемые{{-}}[[ps:Egyp|египетские иероглифы]]), "
                    "C1 (несистематизируемые, запросы от любителей{{-}}[[ps:Xsux|шумерская клинопись]]), "
                    "и C2 (малоизученные, запросы от любителей{{-}}Фестский диск). "
                "Авестийский попал в самую важную категорию A, что дало ему шанс на место в [[pt:bmp|базовой плоскости]]. "
                "Но победило другое мнение{{-}}закодировать авестийский и [[ps:Phlp|пехлеви]] рядом, в дополнительной. "
                "Для незакодированного книжного пехлеви в Юникоде оставлена дыра.",
                EcFont::AVESTAN },
    // Balinese OK, installed Google Noto font
    { "Bali", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::SACRED, EcWritingDir::LTR, EcContinent::OCEAN,
        u8"Балийский", u8"≈1000",
        u8"балийский, сасакский",
        u8"Гласная по умолчанию «а». Балийская письменность не используется в общении (вместо неё латиница), "
                "но важна в индуизме.",
                EcFont::BALINESE },
    // Bamum OK, none even in W10, installed Google Noto and fixed stub
    { "Bamu", QFontDatabase::Any,
        EcScriptType::SYLLABLE, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::AFRICA,
        u8"Бамум", u8"1895—1910",
        u8"бамум ''(Камерун)''",
        u8"Письменность придумал Ибрагим Нджойя, султан Бамума (запад Камеруна). Было выпущено семь версий (A…G), начиная от иероглифов "
                "и заканчивая слоговой. Правда, Нджойя не учёл, что бамум{{-}}тональный язык, потому возникло много омографов. "
                "Пришедшие в 1918 французы выгнали Нджойю и перевели язык на [[ps:Latn|латиницу]]. В 2007 начат проект по возрождению, "
                "прогресс незначителен. Более ранние художества Нджойи можно увидеть в дополнительных плоскостях Юникода.",
                EcFont::BAMUM },
    // Batak OK, installed Google Noto font
    { "Batk", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::OCEAN,
        u8"Батакская", u8"≈1300",
        u8"батакские ''(Суматра)''",
        u8"Происходит от брахми и мало чем отличается от других абугид Юго-Восточной Азии. Гласная по умолчанию «а». "
                "Практически не используется, заменена латиницей.",
                EcFont::BATAK },
    // Bengali OK, W7 has tofu of 2009+ → installed Google Noto
    { "Beng", QFontDatabase::Bengali,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Бенгальский", u8"XV век",
        u8"бенгальский, ассамский, санскрит ''(Бангладеш, Индия)''",
        u8"Относится к восточной ветви индийского письма. Гласная по умолчанию{{-}}среднее между «а» и «о», "
                "в Юникоде условно обозначается «а», в других источниках «ɔ» или «ô». "
                "Чтобы заменить гласную, добавляются огласовки: {{sm|ক}}=кô, {{sm|কি}}=ки."
            "<p>[[pt:virama|Вирама]] (''хосонто'', убрать гласную по умолчанию) используется крайне редко{{-}}{{sm|নাক্ }}= нôк{{-}}"
                "потому что в бенгальском есть множество лигатур: {{sm|ক্ত}}=ктô. "
                "Из-за этого бенгальский считается одной из красивейших письменностей мира. "
                "Ввод лигатур стандартный: первая буква, вирама, вторая буква."
            "<p>Бангладеш{{-}}одно из двух исламских государств с собственной письменностью "
                "(не [[ps:Arab|арабской]], не [[ps:Latn|латиницей]], не [[ps:Cyrl|кириллицей]]). "
                "Второе{{-}}[[ps:Thaa|Мальдивские острова]].",
                EcFont::BENGALI },
    // Bhaiksuki OK, installed Google Noto
    { "Bhks", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Бхаикшуки (стрелоглавое письмо)", u8"XI век",
        u8"санскрит",
        u8"Было в ходу в Восточной Индии в XI–XII{{_}}веках, его упоминал хорезмский (современные Туркмения/Узбекистан) учёный Бируни́. "
                "Первый источник был открыт в Катманду в 1884, предположили, что это и есть «бхаикшуки» Бируни. "
                "Существует 4 рукописи и 11 надписей; все{{-}}буддийские тексты. "
                "Цифры 0 и 3 неизвестны и взяты из похожих письменностей."
            "<p>Перед нами обычная брахмийская абугида: гласная по умолчанию «а», другие приделываются огласовками, "
                    "в простую согласную превращается [[pt:virama|вирамой]]. "
                "Из-за развитой системы [[pt:ligature|лигатур]] вирама в явном виде встречается только в конце слов.<br>"
                    "{{_}}{{_}}{{_}}{{_}}ка {{sm|𑰎}} + и {{sm|◌𑰰}} = ки {{sm|𑰎𑰰}}<br>"
                    "{{_}}{{_}}{{_}}{{_}}ка {{sm|𑰎}} + вирама {{sm|◌𑰿}} = к {{sm|𑰎𑰿}}<br>"
                    "{{_}}{{_}}{{_}}{{_}}ка {{sm|𑰎}} + вирама {{sm|◌𑰿}} + та {{sm|𑰝}} = кта {{sm|𑰎𑰿𑰝}}",
                EcFont::BHAIKSUKI },
    // Bopomofo OK
    { "Bopo", QFontDatabase::Any,
        EcScriptType::ALPHASYLLABLE, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Бопомофо (чжуинь)", u8"1913",
        u8"китайский",
        u8"Бопомофо (по первым четырём буквам), или чжуинь фухао{{-}}китайская фонетическая система, придуманная У Цзиньханом в 1912–13 "
                "и принятая в 1918. После прихода к власти Мао Цзэдуна (1949) используется только на Тайване, но и там потихоньку уступает "
                "привычному пиньиню (латинской записи китайских слогов) и применяется в первую очередь для начального обучения детей, "
                "ввода в мобильный телефон, поиска иероглифов в словарях по произношению."
            "<p>В 2007 на Тайване поступила в продажу модель смартфона BlackBerry, поддерживавшая ввод только через пиньинь, "
                "она оказалась маловостребованной. В последующих версиях пообещали и чжуинь.",
                EcFont::CJK },
    // Brahmi OK, because of sheer importance installed Google Noto
    { "Brah", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Брахми", u8"VIII–VII век до н.э. (основная гипотеза)",
        u8"санскрит, пали, праиндийские языки",
        u8"Неизвестно, когда и откуда брахми произошёл{{-}}обычно считают, что из [[ps:Armi|арамейского]] "
                    "в VIII–VII{{_}}веке{{bc}}. "
                "Самый древний из надёжных источников{{-}}надписи царя Ашоки (III{{_}}век{{bc}}). "
                "Более ранние источники датированы плохо."
            "<p>Брахми обладает такими характерными признаками: "
                    "письмо слева направо, согласная буква означает слог со звуком «а», остальные гласные приделываются огласовками, "
                    "убирается буква «а» знаком «[[pt:virama|вирама]]». "
                "Письменности брахмийского происхождения сильно меняют графику символов и звук по умолчанию, но сохраняют все "
                    "эти особенности и называются «брахмийские абугиды»."
            "<p>Юникод не различает брахми по времени и месту; большинство шрифтов близки к геометричным надписям Ашоки. "
                "К V{{_}}веку брахми трансформировалось в две крупных ветви:<br>"
                    "• северная (=гупта, позднее брахми, северное брахми) → [[ps:Shrd|шарада]], [[ps:Sidd|сиддхаматрика]], "
                        "[[ps:Tibt|тибетский]], [[ps:Bhks|бхайкшуки]];<br>"
                    "• южная → [[ps:Taml|тамильский]], [[ps:Gran|грантха]], [[ps:Khmr|кхмерский]]…",
                EcFont::BRAHMI },
    // Braille OK, “Segoe UI Symbol”
    { "Brai", QFontDatabase::Any,
        EcScriptType::CODE, EcLangLife::NOMATTER, EcWritingDir::NOMATTER, EcContinent::NONE,
        u8"Шрифт Брайля", u8"1824",
        u8"крупные языки мира, включая латинские, кириллические, брахмийские, арабские, китайские иероглифы",
        u8"Шрифт Брайля{{-}}точечный шрифт для слепых. "
                "В нём используются группы 2×3 точки с двоичным кодированием; иногда добавляется и четвёртая строка. "
                "Кодирование различается даже для британского и американского Брайля. "
                "Имеет несколько «регистров» (буквы, цифры, выделение), «переключающихся» специальными символами."
            "<p>Луи Брайль, сын сапожника, в возрасте 3 лет поранился ножом и потерял зрение. Брайль сумел попасть в школу для слепых "
                "Валентина Гаюи́ и пользовался там рельефно-линейным шрифтом. Другой прообраз шрифта Брайля{{-}}ночная азбука "
                "Шарля Барбье. В этой азбуке матрица имела размер 6×6 ячеек, что подходит для зрячего военного, изредка читающего "
                "ощупью, и совершенно непригодно для слепого, читающего много. Брайль сократил матрицу, воспользовавшись двоичным кодом."
            "<p>Шрифт Брайля поставил реабилитацию слепых на промышленные рельсы: легко сделать устройство для тиснения Брайлем, "
                "он не бросается в глаза, будучи выдавлен вместе с обычным текстом для зрячих или даже поверх него.",
                EcFont::SYMBOL },
    // Buginese OK, W10 only → installed Google Noto
    { "Bugi", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::OCEAN,
        u8"Бугийский (лонтара)"sv, u8"XVII век"sv,
        u8"бугийский и другие языки о. Сулавеси"sv,
        u8"Бугийская письменность, или лонтара{{-}}потомок брахми и работает по тому же принципу. "
                "Со времён голландской колонизации заменяется [[ps:Latn|латиницей]]. "
                "Название «лонтара» означает местную пальму, на чьих листьях и писали бугийцы."
            "<p>Гласная по умолчанию «а»: {{sm|ᨀ}}=ка, {{sm|ᨀᨚ}}=ко. "
                "Одинокие гласные устанавливаются на букву {{sm|ᨕ}}: {{sm|ᨕᨚ}}=о. "
                "[[pt:virama|Вирамы]] нет: основные стечения согласных{{-}}отдельные буквы, остальные просто не записываются. "
                "Этот дефект лонтары даже используют как загадку: правильно прочитай каламбур и найди скрытый смысл. "
                "Некоторые специалисты по старому письму пользуются вирамой.",
                EcFont::BUGINESE },
    // Buhid OK, installed Google Noto font
    { "Buhd", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::OCEAN,
        u8"Бухид", u8"≈1300",
        u8"бухидский ''(Филиппины)''",
        u8"Используется мангиан{{-}}небольшой филиппинской народностью (8000 на 1991{{_}}год). "
                "Восходит к [[ps:Brah|брахми]] и родственный с [[ps:Tglg|тагальским]]."
            "<p>В языке всего 15 согласных и три гласных: «а» по умолчанию, «и» и «у» задаются огласовками. Начальные гласные{{-}}"
                "отдельные буквы. На стыках слогов могут появляться дополнительные согласные, они не записываются: "
                "{{sm|ᝐᝒᝑᝋᝓ}} означает ''sihamu'', но читается ''singhanmu''.",
                EcFont::BUHID },
    // Chakma OK, added Noto font, mostly because of missing glyphs
    { "Cakm", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Чакма", u8"после XI века",
        u8"чакма ''(Индия, Бангладеш, Мьянма)''",
        u8"На языке чакма говорят 500{{_}}тыс. человек и он вне угрозы, но письменность вытесняется [[ps:Beng|бенгальской]]. "
                "Точную датировку автору «Юникодии» найти не удалось, но если это потомок [[ps:Mymr|бирманского]]{{-}}то "
                    "точно после XI{{_}}века."
            "<p>Обычная брахмийская абугида с гласной по умолчанию «а:» (длинное «а») "
                    "и часто употребляемой [[pt:virama|вирамой]] ''(maayaa)''. "
                    "Вторая вирама служит для сборки [[pt:ligature|лигатур]], многие из них "
                        "упразднены реформой 2001.<br>"
                    "{{_}}{{_}}ка: {{sm|𑄇}} + и {{sm|◌𑄨}} = ки {{sm|𑄇𑄨}}<br>"
                    "{{_}}{{_}}ка: {{sm|𑄇}} + maayaa {{sm|◌𑄴}} = к {{sm|𑄇𑄴}}<br>"
                    "{{_}}{{_}}ка: {{sm|𑄇}} + virama {{sm|◌𑄳}} + та: {{sm|𑄖}} = кта: {{sm|𑄇𑄳𑄖}}<br>",
                EcFont::CHAKMA },
    // Carian OK, W10 P1
    { "Cari", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::BOTH, EcContinent::ASIA,
        u8"Карийский", u8"VII век до н.э.",
        u8"карийский ''(Малая Азия)''",
        u8"Использовался с VII до I веков{{bc}} "
                "Существовало множество вариантов алфавита. "
                "В Малой Азии писали слева направо, наёмники в Египте{{-}}справа налево."
            "<p>Советский (тогда ещё) учёный Виталий Шеворошкин в 1960 показал, что письменность не слоговая, а алфавитная. "
                "Ключевая книга по расшифровке (Игнасий-Хавьер Адьего) вышла в 1993. "
                "Некоторые символы всё ещё не известны.",
                EcFont::GOTHIC },
    // Canadian syllabics OK, W10 “Gadugi”, W7 lacks 2009 extensions, installed Google Noto font
    { "Cans", QFontDatabase::Any,
        EcScriptType::ABUGIDA, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::AMERICA,
        u8"Канадская слоговая", u8"1833",
        u8"языки канадских аборигенов: кри, оджибве ''(алгонкинский)'', наскапи, инуктитут ''(эскимосский)''…",
        u8"Изобретена миссионером Джеймсом Эвансом в 1833 году. Огласовки заключаются в повороте буквы: "
                "ке={{sm|ᑫ}}, ки={{sm|ᑭ}}, ко={{sm|ᑯ}}, ка={{sm|ᑲ}}. "
                "Более сложные слоги (например, долгие гласные) задаются диакритикой: кии={{sm|ᑮ}}. "
                "Простая согласная{{-}}уменьшенная буква: к={{sm|ᒃ}}.",
                EcFont::CANADIAN_ABORIGINAL },
    { "Cham", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Чамский", u8"IV век",
        u8"чамский ''(Вьетнам, Камбоджа)''",
        u8"Чамская письменность произошла из [[ps:Brah|брахми]], и слоги по умолчанию заканчиваются гласной «а»."
                "Чтобы добавить другую гласную, дописывается огласовка. "
                "Но у чамов исчез знак «[[pt:virama|вирама]]» (убрать гласную), вместо этого к слогу добавляется знак "
                    "конечной согласной: {{sm|ꨆ}}=ка, {{sm|ꨆꨯ}}=ко, {{sm|ꨆꨯꩃ}}=конг."
            "<p>Большинство чамов исповедуют ислам и пишут [[ps:Arab|арабицей]], лишь некоторые чамы Вьетнама "
                "используют чамское письмо.",
                EcFont::CHAM },
    // Cherokee OK, installed Google Noto font. Need it, W7 has no 2014 extensions.
    { "Cher", QFontDatabase::Any,
        EcScriptType::SYLLABLE, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::AMERICA,
        u8"Чероки", u8"1821",
        u8"чероки",
        u8"Будущий вождь Секвойя, изобретатель азбуки чероки, был неграмотен, но брал символы из имевшихся книг{{-}}потому они значат "
                u8"совсем не то, что в языке-прообразе. Так, гласная «i» записывается буквой {{sm|Ꭲ}}."
            "<p>Первой ученицей была шестилетняя дочь Аёка. Изобретение азбуки приняли с недоверием (приняли за одержимого), "
                "но Секвойя и Аёка с блеском прошли проверку, прочитывая записки друг друга. Письмо быстро распространилось "
                "среди индейцев, к 1830 грамотны были {{%|90}} чероки."
            "<p>С 1828 года Секвойя участвовал в переговорах с США за землю чероки.",
            EcFont::CHEROKEE },
    // Chorasmian OK, built my own font
    { "Chrs", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::SOGDIAN, EcContinent::ASIA,
        u8"Хорезмийский", u8"550 до н.э.",
        u8"хорезмийский",
        u8"Язык использовался в Хорезме в районе Амударьи до XIII{{_}}века, постепенно заменяясь персидским. "
                "На этом языке говорили учёный Бируни и проповедник Замахшари{{-}}"
                    "но письменность [[ps:Armi|арамейского]] происхождения ещё в VIII{{_}}веке заменили [[ps:Arab|арабицей]]."
            "<p>Шрифт сделан автором Юникодии и откровенно плохого качества. "
                "Найдёте лучше{{-}}сообщите. "
                "Английская Википедия для объяснения языка пользуется арамейским алфавитом.",
            EcFont::FUNKY },
    // Coptic OK, W7 problems → installed Google Noto font
    { "Copt", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::SACRED, EcWritingDir::LTR, EcContinent::AFRICA,
        u8"Коптский", u8"II век",
        u8"коптский",
        u8"Происходит из [[ps:Grek|греческого]]. Используется коптами (египетскими христианами) как богослужебный. "
                "Общаются копты по-арабски, как разговорный коптский умер от XVII до XIX{{_}}века."
            "<p>С Юникода 4.1 (2005) коптский отделён от греческого.",
                EcFont::COPTIC },
    // Cypriot OK, W10 Segoe Historic
    { "Cprt", QFontDatabase::Any,
        EcScriptType::SYLLABLE, EcLangLife::HISTORICAL, EcWritingDir::RTL_MOSTLY, EcContinent::EUROPE,
        u8"Кипрская", u8"XI век до н.э.",
        u8"древнегреческий, этеокипрский, финикийский ''(одна кототкая надпись)''",
        u8"Происходит из [[ps:Lina|линейного письма А]] через кипро-минойский (не путать!). "
                "Встречалось с XI до III{{_}}века{{bc}} "
                "Надписи IX–VIII{{_}}веков не обнаружены, в дальнейшем письмо разделилось на два варианта: "
                    "пафосский с сохранением направления →, и общекипрский, перевернувшийся в ←. "
                "Частично расшифровано уже в конце XIX{{_}}века, полностью в 1961.",
                EcFont::HISTORIC },
    // Cyr OK except enclosing; managed to modify Noto
    { "Cyrl", QFontDatabase::Cyrillic,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::EUROPE,
        u8"Кириллица", u8"конец IX века",
        u8"русский, украинский, белорусский, русинский, болгарский, македонский, сербохорватский ''(Сербия)'', "
                    "казахский, киргизский, таджикский, языки народов России",
        u8"До 900 года византийские монахи Кирилл и Мефодий придумали славянскую письменность; неизвестно, была ли это кириллица. "
                "Прообразом кириллицы стало византийское письмо унциал для греческого языка, с добавлением звуков, отсутствующих в греческом. "
                "Старшинство кириллицы и глаголицы (появившейся тогда же) спорно, большинство учёных считают, что "
                    "глаголицу создал Кирилл, а кириллицу{{-}}его ученик Климент Охридский, работавший в Болгарии."
            "<p>Кириллица распространилась среди южных славян и проникла на Русь с крещением. "
                "В странах, попавших под влияние Рима, кириллицу прямо запрещали."
            "<p>Современная кириллица восходит к гражданскому шрифту, введённому Петром{{_}}I. "
                "Шрифт стал компромиссом между сторонниками традиций и западниками."
            "<p>СССР сделал кириллическую письменность для многих языков союзных республик. "
                "С развалом СССР на латиницу перешли Азербайджан, Молдавия, Туркмения, Узбекистан." },
    // Devanagari OK, added 8 characters to Noto Serif
    { "Deva", QFontDatabase::Devanagari,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Деванагари", u8"I—VII век",
        u8"хинди, санскрит и другие языки Индии",
        u8"Деванагари (буквально «язык божественного города») развился из письма брахми и стал алфавитом для многих языков Индии. "
                "Особенность деванагари{{-}}все буквы свисают с горизонтальной черты."
            "<p>Согласные буквы означают слог с гласной «а». Чтобы отобразить другие слоги, надо добавить огласовку: "
                "<nobr>ка {{sm|क}} + и {{sm| ि}} = ки {{sm|कि}}</nobr>. "
                "Чтобы получить простую согласную, надо добавить знак «[[pt:virama|вирама]]» («остановка»): к{{_}}{{sm|क्}}."
            "<p>Вирамы в деванагари редки и обычно встречаются в конце слов из-за [[pt:ligature|лигатур]]:<br>"
                "{{_}}{{_}}{{_}}{{_}}ка {{sm|क}} + вирама {{sm|◌्}} + сса {{sm|स}} = ксса {{sm|क्ष}}",
                EcFont::DEVANAGARI },
    /// @todo [urgent] Diak: working
    { "Diak", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::OCEAN,
        u8"Дивес-акуру (древнемальдивский)", u8"VIII–XII век",
        u8"дивехи ''(мальдивский)''",
        u8"Развился на Мальдивских островах из [[ps:Gran|грантхи]] и существовал до XVIII{{_}}века, а на некоторых островах и до XX. "
                    "Раннюю форму (до XII{{_}}века) исследователи называют ''эвела-акуру''. "
                    "С переходом в ислам (1153) появился собственно ''дивес-акуру''{{-}}что интересно, остался слева направо. "
                "В начале XVIII{{_}}века по указу султана был заменён [[ps:Thaa|таной]]{{-}}уже справа налево."
            "<p>Шрифт разработан автором «Юникодии», и определённо некрасив и неполнофункционален. "
                "Из-за нефункциональности не могу объяснить работу письменности{{-}}но перед нами обычная брахмийская абугида. "
                "Гласная по умолчанию «а», заменяется на другую огласовками, убирается знаком «[[pt:virama|вирама]]» ''(халанта)''. "
                "Вторая вирама служит для сборки [[pt:ligature|лигатур]]. "
                "Начальные гласные{{-}}отдельные буквы.",
                EcFont::FUNKY_BIGGER },
    // Dogri OK, W10 off → installed Google Noto
    { "Dogr", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::REVIVED, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Догра", u8"XIX век",
        u8"догри ''(Кашмир)''",
        u8"Язык догров, индийской народности. Письменность позаимствована из [[ps:Takr|такри]], стандартизирована "
                "в середине XIX{{_}}века при магарадже Ранбире Сингхе. "
                "После смерти Ранбира в 1885 под давлением британцев официальным языком выбрали [[ps:Arab|урду]]. "
                "В XX{{_}}веке стали писать на [[ps:Deva|деванагари]], с XXI{{_}}века появился интерес и к своей "
                "письменности."
            "<p>Перед нами брахмийская абугида с гласной по умолчанию «ə» (в Юникоде условно названа «а»), "
                "которая в безударных слогах сильно редуцировалась, как в [[ps:Guru|пенджабском]]. "
                "К тому же в догри встречаются [[pt:ligature|лигатуры]], не закодированные в данном шрифте. "
                "Потому вирама в догри используется редко, что видно в самоназвании: {{sm|𑠖𑠵𑠌𑠤𑠬}} «до-гᵊ-ра:»."
            "<p>Цифры берутся из такри.",
                EcFont::DOGRA },
    // Deseret OK, W10 Segoe UI Symbol, plane 1
    { "Dsrt", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::EXPERIMENTAL, EcWritingDir::LTR, EcContinent::AMERICA,
        u8"Дезеретская (мормонская)", u8"1854",
        u8"английский, отдельные языки индейцев",
        u8"В XIX{{_}}веке было множество попыток придумать фонетический алфавит для английского, "
                "и Бригам Янг, второй руководитель мормонской церкви, был не первый и не последний. "
                "Считалось, что это позволит иммигрантам лучше ознакомиться с английским. "
                "Основную работу выполнили Парли Пратт и Джордж Уотт."
            "<p>Алфавит намеренно не содержит выносных элементов{{-}}это должно снизить износ шрифта. "
                "В современных шрифтах выносные элементы иногда добавляют."
            "<p>В новом алфавите вышло четыре книги. Когда Янг осознал, сколько денег потребуется, чтобы издать "
                "библиотеку, эксперименты прекратились."
            "<p>В 1997 алфавит внесён в Реестр искусственных письменностей. "
                "Исключён в 2001, когда мормонский появился в настоящем Юникоде.",
                EcFont::SYMBOL },
    // Egyptian hiero OK
    { "Egyp", QFontDatabase::Any,
        EcScriptType::CONSONANTHIEROGLYPH, EcLangLife::HISTORICAL, EcWritingDir::BOTH, EcContinent::AFRICA,
        u8"Египетские иероглифы", u8"≈3200 до н.э.",
        u8"древнеегипетский",
        u8"Египетская письменность прошла обычный путь от пиктографического письма (слова изображаются наглядными рисунками) "
                "через идеографическое (появляются абстрактные понятия{{-}}знаком «солнце» обозначают день) "
                "к [[pt:abjad|консонантному]] (начали появляться звуковые знаки). "
                "Подобное мы видели и в других развитых цивилизациях{{-}}например, [[ps:Xsux|шумерской]]."
            "<p>Писали чаще всего справа налево, иногда слева направо или сверху вниз."
            "<p>Помимо иероглифов, были не две не внесённые в Юникод письменности:<br>"
                "• иератическое (''священное'') письмо{{-}}сильно упрощённые иероглифы, книги и хозяйственные документы;<br>"
                "• демотическое (''народное'') письмо{{-}}появилось в поздний период, больше алфавитных знаков, "
                    "сначала письма и хозяйственные документы, потом и книги.<br>"
                "Но лучше всех мы знаем именно иероглифы, потому что на камне они лучше всех сохранились."
            "<p>В шрифтах Windows отсутствуют три иероглифа полового члена."
            "<p>В Юникоде{{_}}14.0 серьёзно перерисовали иероглифы, имеющийся в «Юникодии» шрифт соответствует "
                "Юникоду{{_}}13.0.",
                EcFont::EGYPTIAN },
    // Elbasan OK, none in W10 → installed Google Noto
    /// @todo [link,U14] Vithkuqi
    { "Elba", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::EUROPE,
        u8"Эльбасанский албанский", u8"XVIII век",
        u8"албанский",
        u8"Придумана в середине XVIII{{_}}века и использовалась в окрестностях города Эльбасан (Албания). "
                "Создание приписывается учителю Теодору Хаджи Филиппу (1730–1806), которого убили, когда он перевозил шрифт. "
                "Применялась до XIX{{_}}века. "
            "<p>Полагают, что создание местных письменностей в Албании (эльбасанской, виткутьской, Весо-Бея, Веллары) "
                    "служило для конспирации от турок. "
                "По другим данным, албанские просвещённые умы смотрели на славян, греков и турок с собственным алфавитом. "
                "Известны рукописное Эльбасанское Евангелие размером всего 7×10{{_}}см, 59{{_}}страниц и несколько записок.",
                EcFont::ELBASAN },
    // Elymaic OK, W10 none → installed Google Noto
    { "Elym", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::ASIA,
        u8"Элимайский (эламский)", u8"II век до н.э.",
        u8"элимайский ''(=эламский, Персидский залив)''",
        u8"После военных поражений от Александра Македонского Персия прекратила своё существование. "
                "Последователи Александра не смогли удержать огромное многонациональное государство, и в результате распада "
                    "появился в том числе и Элам."
            "<p>Государство существовало с 147{{bc}} до 224, часто попадало под власть парфян "
                    "и имело письменность [[ps:Armi|арамейского]] происхождения с несоединёнными буквами. "
                "Никаких знаков препинания, кроме пробела, не известно. "
                "Собственные названия букв неизвестны и взяты из арамейского.",
                EcFont::ELYMAIC },
    // Ethiopic OK, lots of tofu, espec. in W7 → installed Google Noto
    { "Ethi", QFontDatabase::Any,
        EcScriptType::ABUGIDA, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::AFRICA,
        u8"Эфиопская", u8"I—VII век",
        u8"амхарский, тигринья и другие эфиосемитские",
        u8"Эфиопская письменность использовалась для записи языка геэз, вышедшего из употребления в XIII{{_}}веке. "
                "Геэз всё ещё используется эфиопскими христианами как литургический{{-}}"
                "а письменность распространилась на другие языки Эфиопии."
            "<p>С конца XX{{_}}века теряет влияние и заменяется [[ps:Latn|латиницей]].",
                EcFont::ETHIOPIC },
    // Georgian OK, installed Google Noto font
    { "Geor", QFontDatabase::Georgian,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::EUROPE,
        u8"Грузинский", u8"≈V век",
        u8"грузинский, мегрельский и другие картвельские",
        u8"Достоверно неизвестно, когда создана грузиница и что было прототипом. "
                "Распространённая версия, что грузинский алфавит создан создателем [[ps:Armn|армянского]] Месропом Маштоцем, "
                "отметается учёными."
            "<p>Грузинский язык безрегистровый, однако в Юникоде исторически существуют четыре стиля письма, по порядку кодов:<br>"
                "• асомтаврули{{-}}устаревшие заглавные буквы. "
                    "Из-за неудачных образцовых глифов этот диапазон закрепился именно за ними;<br />"
                "• мхедрули{{-}}обычный грузинский с развитыми над- и подстрочными элементами;<br />"
                "• мтаврули{{-}}«заглавная» форма обычного грузинского без выносных элементов;<br />"
                "• нусхури{{-}}курсивный церковный [[pt:minuscule|минускул]].",
                EcFont::GEORGIAN },
    // Glagolitic OK, installed Google Noto font
    { "Glag", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::EUROPE,
        u8"Глаголица", u8"IX век",
        u8"древнеславянский",
        u8"Глаголица, по наиболее распространённому мнению, была составлена около 863 Кириллом и Мефодием. Считается, что глаголица "
                "старее кириллицы: дорогой пергамент старались повторно использовать, и сохранились палимпсесты (пергаменты, где стёрли "
                "одно и написали другое), где глаголицу заменили кириллицей."
            "<p>На Руси глаголица встречалась редко, больше применялась в Болгарии и Хорватии.",
                EcFont::GLAGOLITIC },
    // Gothic OK, the best is Segoe UI Historic
    { "Goth", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::EUROPE,
        u8"Готский", u8"IV век",
        u8"готский",
        u8"Считается, что алфавит создал епископ Вульфила, который перевёл Библию на готский и не хотел, чтобы священный текст "
                    "писался [[ps:Runr|рунами]]. "
                "Этим алфавитом написаны памятники готского языка IV–VI{{_}}веков. "
            "<p>Алфавит взят из [[ps:Grek|греческого]], названия букв те же, что и у рун. "
                "Две буквы имеют только числовое значение: {{sm|𐍁}}=90 и {{sm|𐍊}}=900. "
                "Благодаря такому происхождению письменности «канонический» вид готских букв{{-}}написанные жирным пером, "
                    "а не острым предметом.",
                EcFont::GOTHIC },
    // Grantha OK, W10 none → installed Google Noto
    { "Gran", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Грантха", u8"VI век",
        u8"санскрит, тамильский",
        u8"Грантха{{-}}промежуточная ступень развития южной ветви [[ps:Brah|брахми]]. "
                "Древний вариант грантхи называется ''паллава'' и использовался в государстве Паллавов (275–897). "
                "Для тамильского языка грантха использовалась до XVI{{_}}века, для санскрита до XIX. "
                "Повлияла на [[ps:Taml|тамильскую]], [[ps:Mlym|малая́лам]], [[ps:Sinh|сингальскую]]. "
                "В библиотеках Индии хранится около 200{{_}}тысяч источников, как написанных на пальмовых листьях, так и печатных."
            "<p>Около 2010 не удалась попытка ввести грантху в Юникод в [[pt:unification|унификации]] с тамильской. "
                "Из-за поднявшегося шума (у тамилов силён национализм) решили разунифицировать письменности, "
                    "оставив только цифры, которые в поздней грантхе совпадают с тамильскими."
            "<p>Обычная брахмийская абугида. "
                "[[pt:virama|Вирама]] крайне редка из-за тяги санскрита к открытым слогам "
                    "и большого количества [[pt:ligature|лигатур]].<br>"
                "{{_}}{{_}}ка {{sm|𑌕}} + и {{sm|◌𑌿}} {{_}} = ки {{sm|𑌕𑌿}}<br>"
                "{{_}}{{_}}ка {{sm|𑌕}} + вирама {{sm|◌𑍍}} = к {{sm|𑌕𑍍}}<br>"
                "{{_}}{{_}}ка {{sm|𑌕}} + вирама {{sm|◌𑍍}} + та {{sm|𑌤}} = кта {{sm|𑌕𑍍𑌤}}",
                EcFont::GRANTHA },
    // Greek OK, W7 Cambria + Noto do the job
    { "Grek", QFontDatabase::Greek,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::EUROPE,
        u8"Греческий", u8"IX век до н.э.",
        u8"греческий",
        u8"Греческий сделан из [[ps:Phnx|финикийского]] без оглядки на раннегреческие системы{{-}}"
                    "[[ps:Linb|линейное письмо Б]] и [[pt:Cprt|кипрское]]. "
                "Финикийский алфавит был [[pt:abjad|консонантным]] (задаёт согласные звуки), и семитским языкам это подходит, ведь корень слова{{-}}"
                "костяк согласных, а гласные играют роль окончаний: так, в [[ps:Arab|арабском]] (тоже семитский) ''джихадом'' занимается <i>моджахед</i> "
                "(или, точнее, ''муджахид'')."
            "<p>Греки добавили гласные звуки, и греческий{{-}}первый известный истинный алфавит, кодирующий как согласные, так и гласные. "
                "Названия букв изменились мало, но перестали что-то значить: ''алеф=бык''{{_}}→ ''альфа''."
            "<p>Из греческого письма пошли [[ps:Latn|латиница]], [[ps:Cyrl|кириллица]], "
                "[[ps:Copt|коптский]] и, возможно, [[ps:Armn|армянский]] и [[ps:Runr|руны]]." },
    // Gujarati OK, installed Google Noto: cannot find a good pair for W7/10, and Noto is REALLY nice.
    { "Gujr", QFontDatabase::Gujarati,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Гуджарати", u8"X–XVI век",
        u8"гуджаратский ''(запад Индии)''",
        u8"Появился из [[ps:Deva|деванагари]] и очень похож на него, но уже старый гуджарати потерял характерную черту сверху. "
                "Гласная по умолчанию «ə».",
            EcFont::GUJARATI },
    // Gurmukhi OK, installed Google Noto *UI* because of W7 troubles; UI better handles umlauts
    { "Guru", QFontDatabase::Gurmukhi,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Гурмукхи", u8"XVI век",
        u8"пенджабский",
        u8"С X века в Пенджабе использовалась письменность [[ps:Shrd|шарада]], плохо отвечавшая местному языку. Новую письменность придумал "
                "сикхский гуру Ангад, один из основателей религии. Он основывал школы, где учились родному пенджабскому, "
                "а не классическому санскриту, и общественные столовые, где все сидели вместе, независимо от индуистской касты."
            "<p>Гласная по умолчанию «ə» (в Юникоде условно названа «а»). "
                "Остальные гласные добавляются огласовками: {{sm|ਕ}}=кə, {{sm|ਕਿ}}=ки. "
                "В безударных слогах «ə» сильно редуцировалась, а в конце слов не читается вообще, "
                "это хорошо видно в самоназвании{{-}}{{sm|ਗੁਰਮੁਖੀ}} «гу-рᵊ-му-кхии». "
                "Кроме того, есть подстрочные буквы{{-}}{{sm|ਕ੍ਰ}} «крə», печатаются они стандартно: "
                "основная буква, символ вирамы, подстрочная. "
                "Потому вирама пенджабскому нужна на клавиатуре, но не в печати, однако встречается "
                "в санскритских текстах. "
                "Чтобы начать слово с гласной, огласовки приделывают к нулевой согласной (для разных гласных разной)."
            "<p>Гурмукхи сильно связан с сикхизмом, название означает «из уст гуру». Пенджабские мусульмане используют "
                "шахмукхи («из уст шаха»), разновидность арабского алфавита.",
                EcFont::GURMUKHI },
    // Hangul OK, installed Noto CJK font
    { "Hang", QFontDatabase::Korean,
        EcScriptType::ARGUABLE, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Хангыль", u8"1443",
        u8"корейский",
        u8"Хангыль (мужской род!){{-}}корейское алфавитно-слоговое письмо. В нём 51 чамо (буква) объединяется в группы приблизительно по слогам."
            "<p>Придуман группой учёных под руководством царя Седжона Великого, считавшего, что [[ps:Hani|китайские иероглифы]] "
                "плохо передают корейский язык и сложны для народа. Образованная публика смотрела на хангыль надменно, "
                "считая женским, детским или народным письмом. Последующие цари даже запрещали хангыль. Возрождение началось в XX{{_}}веке, "
                "официальным стал в 1945. Единственная письменность Северной Кореи и основная{{-}}в Южной.",
                EcFont::KOREAN },
    // Hani will never be OK, what a pity…
    { "Hani", WS_HANI,       // Special rules for hieroglyphs, SimChi triggers them
        EcScriptType::HIEROGLYPH, EcLangLife::ALIVE, EcWritingDir::LTR_CJK, EcContinent::ASIA,
        u8"Китайские иероглифы", u8"около 2000 до н.э.",
        u8"китайский, японский, ранее вьетнамский и корейский",
        u8"Первые пиктограммы относятся к VI{{_}}тысячелетию до н.э., их выцарапывали или рисовали краской, "
                    "впоследствии они видоизменились под письмо кистью и тушью. "
                "Уже в I{{_}}тысячелетии до н.э. люди стали отходить от иероглифов, но Китай законсервировал архаичную письменность, "
                "этому есть несколько причин:<br>"
            "• многонациональный народ{{-}}помимо государствообразующего народа хань, было ещё около десятка, и они понимали "
                "друг друга хоть на письме;<br>"
            "• культ традиции{{-}}даже сейчас китайцы довольно консервативны;<br>"
            "• неплохо действовавшая меритократия{{-}}тысячи знаков и умение их красиво писать выявляли обучаемых людей.<p>"
            "<p>В III{{_}}веке иероглифы проникли в Японию. У многих иероглифов есть как китайское, так и японское прочтение: "
                "например, {{sm|月}} (луна) имеет китайские прочтения «гэцу» и «гацу», и японское «цуки». "
                "Как читать{{-}}определяется контекстом: {{sm|月曜日}}=гэцуо:би (понедельник), {{sm|月見}}=цукими (любование луной)."
            "<p>В Китае, Тайване, Японии и Корее иероглифы отличаются, Юникод эти различия не кодирует: тонкости написания передаются шрифтами, "
                "и даже если шрифт неверный, понятно, о чём речь."
            "<p>Вьетнам отказался от иероглифов в 1945. Северная Корея не использует иероглифы, Южная использует крайне редко.",
                EcFont::CJK },
    // Hanunoo OK, installed Google Noto font
    { "Hano", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::OCEAN,
        u8"Хануноо", u8"≈1300",
        u8"филиппинские горные народы",
        u8"Хануноо восходит к [[ps:Brah|брахми]] и похож на [[ps:Tglg|тагальский]].",
                EcFont::HANUNOO },
    // Hatran OK, W10 none, installed Google Noto
    { "Hatr", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::ASIA,
        u8"Хатранский (ашшурский)", u8"≈100 до н.э.",
        u8"хатранский (ашшурский) арамейский",
        u8"Потомок [[ps:Armi|арамейского]]. "
                "Использовался в Хатре (современный Ирак) до III{{_}}века, когда Хатру разрушил Шапур{{_}}I Сасанид. "
                "Все известные надписи очень короткие, самая длинная{{-}}13 строк.",
                EcFont::HATRAN },
    // Hebrew OK, installed very nice Noto, implemented yod triangle
    { "Hebr", QFontDatabase::Hebrew,
        EcScriptType::CONSONANT, EcLangLife::ALIVE, EcWritingDir::RTL, EcContinent::ASIA,
        u8"Иврит", u8"VI—II в. до н.э.",
        u8"иврит, ладино, идиш, караимский, крымчакский",
        u8"Иврит развился из [[ps:Armi|арамейской]] письменности, и ко II{{_}}в.{{bc}} приобрёл почти современный вид."
            "<p>Записывает только согласные буквы, но четыре буквы {{sm|אהוי}} могут означать гласные. "
                "С той же целью иногда используют огласовки{{-}}точки над буквами."
            "<p>К X{{_}}веку иврит уходит из разговорной практики, оставшись только книжным языком, однако еврейскими буквами "
                "писали и на разговорных еврейских языках: идише, ладино и других. С появлением государства Израиль "
                "иврит снова сделали языком повседневного общения.",
                EcFont::HEBREW },
    { "Hent", QFontDatabase::Japanese,
        EcScriptType::SYLLABLE, EcLangLife::HISTORICAL, EcWritingDir::LTR_CJK, EcContinent::OCEAN,
        u8"Хэнтайгана", u8"≈X век",
        u8"японский",
        u8"Хэнтáйгана{{-}}общее название устаревших знаков [[ps:Hira|хирáганы]], записанных крайне небрежно."
            "<p>Из [[ps:Hani|иероглифов]] произошла вспомогательная азбука манъёгана{{-}}роль играло не изображение на иероглифе, "
                "а прочтение. К X{{_}}веку манъёгана уступила хирагане и [[ps:Kana|катакане]]. "
                "Какой-то системы не было, потому существовали разные знаки для одного слога. "
            "<p>В 1900{{_}}хирагану стандартизировали, за каждым слогом оставили один символ. А хэнтайгана стала чем-то вроде "
                "западного готического шрифта или кириллического устава{{-}}применяется в открытках, религиозных документах, "
                "дипломах школ боевых искусств и… на вывесках ресторанов."
            "<p>«Китайские порномультики» хентай{{-}}омофон: они происходят от {{sm|変態}} «хэнтай»{{-}}извращённый, "
                "а хэнтайгана от {{sm|変体}} «хэнтай»{{-}}необычный.",
                EcFont::HENTAIGANA },
    // Hiragana OK, installed small version of Noto CJK Korean
    { "Hira", QFontDatabase::Japanese,
        EcScriptType::SYLLABLE, EcLangLife::ALIVE, EcWritingDir::LTR_CJK, EcContinent::ASIA,
        u8"Хирагана", u8"VIII—IX век",
        u8"японский",
        u8"Около III{{_}}века японцы стали писать [[ps:Hani|китайскими иероглифами]]. "
                "Из них произошла вспомогательная азбука манъёгана{{-}}роль играло не изображение на иероглифе, а прочтение. "
                "Хирáгана{{-}}не что иное, как очень небрежно записанная манъёгана."
            "<p>Женщины, которым не было доступно образование, писали слоговой азбукой, и предпочитали не рубленую "
                "[[ps:Kana|катáкану]], а плавную хирáгану. Сейчас хираганой пишут слова, у которых иероглифа нет или неизвестен пишущему/читающему (кроме "
                "заимствованных, для них катакана), окончания слов, учат детей, подписывают прочтение иероглифов.",
                EcFont::KOREAN },
    // NP Hmong OK, W10 none → installed Google Noto
    { "Hmnp", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::AMERICA,
        u8"Ньякэ пуацы хмонг (алфавит Черванга)", u8"1980-е",
        u8"языки белых и зелёных хмонгов",
        u8"В 1970‑е вьетнамские и лаосские хмонги разбежались по странам, в том числе осели в США. "
                "О судьбе американского хмонга есть великолепный фильм «Гран-Торино» (2008)."
            "<p>Письменность придумал в 1980‑е Черванг Конг, священник одной из протестантских церквей Америки. "
                "Церковь активно проповедовала, и письменность распространилась по США. "
                "Ограниченно используется в Азии и Австралии. "
                "Для собственных имён вместо заглавных букв используются специальные знаки."
            "<p>Название нечитаемое, письменность новая, и автору «Юникодии» пришлось расшифровать название "
                    "на стандартной хмонгской [[ps:Latn|латинице]]{{-}}''Nyiajkeeb Puajtxwm Hmoob''. "
                "Если он ошибся{{-}}поправьте.",
                EcFont::NP_HMONG },
    // Hungarian runes OK, W10 none → installed some open-source font
    { "Hung", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::EUROPE,
        u8"Венгерские (секейские) руны", u8"X век (возможно, раньше)",
        u8"венгерский",
        u8"Неизвестно, как венгерские руны связаны с [[ps:Orkh|тюркскими]]: "
                "то ли произошли от них, то ли сестринские письменности."
            "<p>Иштван{{_}}I, первый король Венгрии, объединил венгерские племена, обратил в христианство "
                    "и около 1000 перешёл на [[ps:Latn|латиницу]], но руны продолжают использоваться, постепенно угасая. "
                "Последние надписи датируются ≈1800."
            "<p>С XVII{{_}}века началось изучение, около 1900 проведена основная работа "
                    "и даже придумали руны для недостающих звуков венгерского. "
                "Венгры гордятся рунами и иногда дублируют ими надписи. "
                "Но мало кто умеет читать руны: "
                    "в 2017 шутники заменили знак города Эрд на «Szia» (''привет''), и заметили это через месяц.",
                EcFont::OLD_HUNGARIAN },
    // Old Italic OK, installed Google Noto (Segoe has no newer chars od 2014+)
    { "Ital", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::BOTH, EcContinent::EUROPE,
        u8"Этрусский", u8"700–100 до н.э.",
        u8"этрусский и другие языки Италии",
        u8"Алфавит развился под влиянием [[ps:Phnx|финикийского]] и [[ps:Grek|греческого]]. "
                "В дальнейшем развился в [[ps:Latn|латинский]]."
            "<p>Юникод не различает языков Италии, объединяя все в один блок{{-}}этрусский.",
                EcFont::OLD_ITALIC },
    // Javanese OK, W10 has “Javanese Text”, W7 does not, installed Google Noto font
    { "Java", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::OCEAN,
        u8"Яванский", u8"XV век",
        u8"яванский, мелкие языки Явы",
        u8"Гласная по умолчанию «а». Знак «вирама» (стереть гласную) называется «пангкон» "
                "и представляет собой не косую чёрточку, а длинный крюк. Яванская письменность близка к балийской. "
                "В настоящее время вытесняется латиницей.",
                EcFont::JAVANESE },
    // Kayah Li OK, W10 none, installed Google Noto font
    { "Kali", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Кая-ли", u8"1962",
        u8"краснокаренский ''(Мьянма)''",
        u8"Кая-ли происходит из брахми, создана Хтаэ Бу Пхаэ в 1962 в ответ на распространение [[ps:Latn|латиницы]]{{-}}"
                "но буквы не имеют гласной по умолчанию, и потому это не абугида (слоговая азбука, где у сходных слогов сходное написание). "
                "Четыре гласных пишутся отдельными буквами, остальные диакритикой над {{sm|ꤢ}}. "
                "Ещё несколько гласных из заимствованных слов пишутся диакритикой над {{sm|ꤣ}}."
            "<p>Кая-ли{{-}}во многом алфавит беженцев: {{%|90}} живущих в тайских лагерях беженцев{{-}}карены, сбежавшие от зверств бирманских "
                "военных, и в околотюремных условиях важно всё, что сплачивает. "
                "Карены{{-}}разнородная группа народов, среди них есть красные, чёрные, белые и другие, по цвету национального костюма. "
                "Помимо кая-ли, пишут [[ps:Thai|тайским]], латиницей и [[ps:Mymr|бирманским]].",
                EcFont::KAYAH_LI },
    // Katakana OK, installed small version of Noto CJK Korean
    { "Kana", QFontDatabase::Japanese,
        EcScriptType::SYLLABLE, EcLangLife::ALIVE, EcWritingDir::LTR_CJK, EcContinent::CJK,
        u8"Катакана", u8"VIII—IX век",
        u8"японский",
        u8"Около III{{_}}века японцы стали писать [[ps:Hani|китайскими иероглифами]]. "
                "Из них произошла вспомогательная азбука манъёгана{{-}}"
                "роль играло не изображение на иероглифе, а его прочтение. Манъёгана упростилась до катáканы."
            "<p>Катáкану используют для записи заимствованных слов и начального обучения иностранцев. "
                "До 1946 использовали для записи окончаний слов (сейчас это делают [[ps:Hira|хирáганой]]).",
                EcFont::KOREAN },
    // Kharoshthi OK, W10 tofu → installed Noto
        /// @todo [portability] Ligature is created from a wrong sequence here
    { "Khar", QFontDatabase::Any,
        EcScriptType::ABUGIDA, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::ASIA,
        u8"Кхароштхи", u8"IV век до н.э.",
        u8"гандхари, пали, сакские, санскрит",
        u8"Кхароштхи (индобактрийское, кабульское письмо) применялось в Средней Азии и Афганистане до III{{_}}века. "
            "Происходит из [[ps:Armi|арамейского]] и считается сестринским с [[ps:Brah|брахми]], потомков нет. "
                    "Обладает одним из свойств [[pt:brahmic|брахмийской абугиды]]{{-}}гласная по умолчанию меняется огласовками. "
                    "[[pt:virama|Вирама]] чисто виртуальная, конечная согласная записывается нижним индексом. "
                    "Также известно около 30 [[pt:ligature|лигатур]] из сдвоенных согласных:<br>"
                    "{{_}}{{_}}{{_}}ка {{sm|𐨐}} + и {{sm|◌𐨁}} = ки {{sm|𐨐𐨁}}<br>"
                    "{{_}}{{_}}{{_}}дха {{sm|𐨢}} + и {{sm|◌𐨁}} + ка {{sm|𐨐}} + вирама {{sm|◌𐨿}} = дхик {{sm|𐨐𐨿𐨢𐨁}}<br>"
                    "{{_}}{{_}}{{_}}ка {{sm|𐨐}} + вирама {{sm|◌𐨿}} + та {{sm|𐨟}} = кта {{sm|&#68127;&#68112;&#68159;}}"
            "<p>''(В «Юникодии» почему-то нарисовало правильный знак через ка+вираму+дха+и{{_}}/ та+ка+вираму, "
                    "и копирование через буфер даст неправильную последовательность, "
                    "но во всех браузерах, даже в древнем Internet Explorer, всё работает верно.)''"
            "<p>Кхароштхи открыто в XIX{{_}}веке и довольно быстро расшифровано.",
                EcFont::KHAROSHTHI },
    // Khmer OK, fancy and inconsistent in W7/10, installed Google Noto *UI* because of umlauts
    /// @todo [desc] Describe Khmer, interesting script
    { "Khmr", QFontDatabase::Khmer,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Кхмерский", u8"VI век",
        u8"кхмерский ''(Камбоджа)''",
        u8"Как и другие языки Юго-Восточной Азии, произошёл из брахми. "
                "Гласная по умолчанию для одних согласных «а», для других «о». "
                "Самый старый источник датируется 611. "
                "Стал основой для [[ps:Thai|тайского]] и [[ps:Laoo|лаосского]].",
                EcFont::KHMER },
    { "Khoj", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::SACRED, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Ходжки", u8"XIV–XVI век",
        u8"качи, синдхи ''(Индия)''",
        u8"Традиционно считается, что ходжки придумал Пир Садардин (XIV{{_}}век), посланный в Индию проповедовать ислам. "
                "По более приземлённой версии, это выделившийся в XVI{{_}}веке вариант ланды{{-}}"
                    "очень беглой [[ps:Shrd|шарады]]. "
                "Используется и изучается поныне индийскими шиитами."
            "<p>Ходжки{{-}}обычная брахмийская абугида с гласной по умолчанию «а», [[pt:virama|вирамой]] "
                    "и четырьмя [[pt:ligature|лигатурами]] на стыках согласных, "
                    "одна из которых за неимением согласной «сса» пишется через знак «нукта»:<br>"
                "{{_}}{{_}}ка {{sm|𑈈}} + и {{sm|◌𑈭}} = ки {{sm|𑈈𑈭}}<br>"
                "{{_}}{{_}}ка {{sm|𑈈}} + вирама {{sm|◌𑈵}} = к {{sm|𑈈𑈵}}<br>"
                "{{_}}{{_}}та {{sm|𑈙}} + вирама {{sm|◌𑈵}} + ра {{sm|𑈦}} = тра {{sm|𑈙𑈵𑈦}}<br>"
                "(также «джнья» и «дра»)<br>"
                "{{_}}{{_}}ка {{sm|𑈈}} + вирама {{sm|◌𑈵}} + са {{sm|𑈩}} + нукта {{sm|◌𑈶}} = ксса {{sm|𑈈𑈵𑈩𑈶}}"
            "<p>Цифры очень похожи на [[ps:Gujr|гуджарати]] и [[pt:unification|унифицированы]] с ним.",
                EcFont::KHOJKI },
    // Khitan small OK, installed a font
    { "Kits", QFontDatabase::Any,
        EcScriptType::SYLLABOHIEROGLYPH, EcLangLife::HISTORICAL, EcWritingDir::RTL_COL, EcContinent::ASIA,
        u8"Малое киданьское письмо", u8"X век",
        u8"киданьский ''(северо-восточный Китай)''",
        u8"Использовалось в государстве Ляо (северо-восточный Китай). "
                "Изобретено около 925 учёным по имени Елюй Дела на основе уйгурского. "
                "Слова записываются блоками до семи иероглифов. "
                "Из 378 известных на 1987{{_}}год символов 125{{-}}иероглифы, 115{{-}}фонетические, остальные не расшифрованы."
            "<p>Большое и малое письмо использовались параллельно, малое проще (большое{{-}}чисто иероглифическое). "
                "Киданьских книг не сохранилось, только предметы и надписи на захоронениях знатных граждан, последняя{{-}}около 1200. "
                "В 1216 киданей окончательно разбили монголы и корейцы."
            "<p>Многие шрифты кладут киданьский текст на 90°. "
                "Данный шрифт показывает иероглифы прямо{{-}}как в книге ''Daniel Kane. The Kitan Language and Script''. "
                "Из-за вопросов с авторскими правами иконка показывает один из символов с медного киданьского зеркала "
                    "(прорисовал проф. Цзин Юнши), а шрифт{{-}}прямой современный, перерисованный Эндрю Уэстом.",
                EcFont::KHITAN_SMALL },
    // Kannada OK, W7 has no recent extensions → installed Google Noto
    { "Knda", QFontDatabase::Kannada,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Кáннада", u8"XIV век",
        u8"кáннада ''(Юго-Западная Индия)''",
        u8"Как и большинство других письменностей Юго-Восточной Азии, произошла из [[ps:Brah|брахми]]. "
                "Гласная по умолчанию «а», заменяется огласовками:<br>"
                "{{_}}{{_}}ка {{sm|ಕ}} + и {{sm|◌ಿ}} = ки {{sm|ಕಿ}}<br>"
                "[[pt:virama|Вирама]] (знак «убрать гласную») появляется на письме только для передачи санскрита. "
                "На компьютерах вирама служит для набора [[pt:ligature|лигатур]] «согласная+согласная»:<br>"
                "{{_}}{{_}}ка {{sm|ಕ}} + вирама {{sm|◌್}} + та {{sm|ತ}} = кта {{sm|ಕ್ತ}}<br>"
                "Часто применяется в смайликах: {{sm|ಠ␣ಠ}}.",
                EcFont::KANNADA },
    // Kaithi OK, W10 none → installed Google Noto
    { "Kthi", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Кайтхи", u8"XVI век",
        u8"хинди, мелкие языки Индии",
        u8"Название письменности происходит от касты «каястха» (конторщики). "
                "Является очень [[pt:cursive|беглым]] вариантом [[ps:Deva|деванагари]]. "
                "Использовалось с Империи великих моголов в северо-восточной Индии, разрешалось в судах, "
                    "а с 1950 до 1954 было основной судебной письменностью. "
                "Рассчитывали расширить пользование кайтхи, но каста брахманов воспротивилась этому, и современные индийцы "
                    "едва читают кайтхи."
            "<p>Цифры похожи на [[ps:Deva|деванагари]], кроме 5 и 9, и [[pt:unification|унифицированы]] с ним.",
                EcFont::KAITHI},
    // Lanna OK, W10 none, installed Google Noto font
    { "Lana", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Ланна (тай-тхам)", u8"≈1300",
        u8"юан (=северотайский, =ланна), лы (=тай-лы), кхынский; все{{-}}Таиланд",
        u8"Происходит из северотайского государства Ланна или Ланнатай (1296—1558)."
            "<p>Как и в других подобных письменностях, круглые очертания букв произошли из-за письма на пальмовых листьях "
                "(прямая черта прорежет лист по жилкам). Гласная по умолчанию «а». "
                "Между словами не ставятся пробелы. Гласные буквы бывают двух видов: независимые (в начале слова) и огласовки. "
                "Вирама называется «ra haam» и используется нечасто."
            "<p>Имеет два набора цифр: хора (повседневные) и тхам (священные)."
            "<p>Говорящих на языке юан несколько миллионов, но с XX века не многие грамотны в ланне, чаще пишут [[ps:Thai|тайским]] письмом.",
                EcFont::LANNA },
    // Lao OK, W10 partial, installed SengBuhan font
    { "Laoo", QFontDatabase::Lao,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Лаосский", u8"≈1350",
        u8"лаосский",
        u8"Лаосский алфавит стандартизирован в XIV веке. Как и тайский, происходит из [[ps:Khmr|кхмерского]]. "
                "После реформы, сделанной коммунистами в 1960‑е, исчезли гласная по умолчанию и знак вирамы, и у лаосского остался последний "
                "признак абугиды{{-}}сложные правила записи гласных: {{sm|ເກົ້າ}} ''kao'' «девять». Лаосский язык тоновый, и в этом слове "
                "мы видим над согласной {{sm|ກ}} как огласовку из трёх (!) символов , так и тон."
            "<p>Между словами не ставятся пробелы. Как и в [[ps:Thai|тайском]], буквы называются подходящими словами: "
                "{{sm|ບ}} называется «бо-бэ» или просто «бэ» (козёл). Буква {{sm|ຣ}} (r) стала немой или преобразовалась в «л», и потому "
                "исчезла из орфографии, оставшись только для записи иностранных имён."
            "<p>В буддизме используется язык пали как священный, для записи этого языка используются несколько дополнительных букв "
                "и та самая вирама."
            "<p>В Юникоде для пустого места над меткой принят пунктирный кружок; лаосцы традиционно используют косой крест, "
                "в Юникод не попавший. Иногда для этого используют [[ps:Latn|латинскую]] «x».",
                EcFont::LAO },
    // Latin OK, managed to modify
    { "Latn", QFontDatabase::Latin,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::EUROPE,
        u8"Латиница", u8"I тысячелетие до н.э.",
        u8"большинство языков западного и тюркского мира (включая Восточную Европу, Прибалтику, Молдавию), Чёрной Африки, Океании; "
                "вьетнамский, карельский, курдский, эсперанто",
        u8"Латиница{{-}}[[ps:Grek|древнегреческое]] письмо, адаптированное около VII{{_}}в.{{bc}} "
                "для записи [[ps:Ital|этрусских]] говоров, впоследствии ставших латинском языком."
            "<p>В средние века к латинице добавились буквы J, V и W. [[pt:minuscule|Минускульное]] письмо, изобретённое для экономии "
                "дорогого пергамента, превратилось в строчные буквы."
            "<p>Латинский язык давно мёртв, но широкая территория Римской империи, миссионерская деятельность Папского престола "
                "и Великие географические открытия, совершённые западным миром, сделали латиницу основным алфавитом мира. "
                "Латиница используется в математике, медицине, фонетике, программировании."
            "<p>С развалом СССР на латиницу перешли Азербайджан, Молдавия, Туркмения, Узбекистан." },
    // Lepcha OK, W10 none, installed Google Noto font
    { "Lepc", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Лепча (ронг)", u8"≈1700",
        u8"лепча ''(Индия, Бутан, Непал)''",
        u8"Гласная по умолчанию «а». Произошла из [[ps:Tibt|тибетского]], изначально писалась сверху вниз, но потом стали писать горизонтально, "
                "потому буквы напоминают тибетские, повёрнутые на 90°."
            "<p>Начальные гласные не имеют особую форму, а пишутся с «нулевой» согласной буквой, похожей на амперсанд. "
                "[[pt:virama|Вирамы]] нет. Вместо этого, чтобы получить слог из трёх звуков, добавляют сначала огласовку, а затем конечный согласный. "
                "Между словами нет пробелов.",
                EcFont::LEPCHA },
    // Limbu OK, W10 none, installed Google Noto font
    { "Limb", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Лимбу", u8"≈1700",
        u8"лимбу ''(Непал)''",
        u8"Гласная по умолчанию «о»."
            "<p>Начальные гласные не имеют особую форму, а пишутся с «нулевой» согласной буквой {{sm|ᤀ}}, похожей на непальский флаг. "
                "Вирамы нет. Вместо этого, чтобы получить слог из трёх звуков, добавляют сначала огласовку, а затем конечный согласный.",
                EcFont::LIMBU },
    // Linear A OK, W10 none, installed Google Noto font
    { "Lina", QFontDatabase::Any,
        EcScriptType::SYLLABOHIEROGLYPH, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::EUROPE,
        u8"Линейное письмо А", u8"≈1800 до н.э.",
        u8"предположительно минойский",
        u8"Использовалось на островах Крит и Киклады. "
                "Большая часть надписей канцелярские и выполнены на глине, встречаются нарисованные чернилами на сосудах и других предметах. "
                "Но форма знаков подразумевает, что писали чернилами на коже или другом мягком материале. "
                "Около 1450, с вторжением греков, заменяется [[ps:Linb|линейным письмом Б]]."
            "<p>Артур Эванс обнаружил письмо в конце XIX{{_}}века, причём часть артефактов погибла{{-}}"
                    "необожжённая глина, залитая дождём, размякла. "
                "Он затягивал публикацию, надеясь расшифровать самостоятельно, вплоть до смерти в 1941. "
                "Исчерпывающей расшифровки всё ещё нет{{-}}в отличие от более позднего линейного Б.",
                EcFont::LINEAR_A },
    // Linear B OK, W10 none, installed Google Noto font
    { "Linb", QFontDatabase::Any,
        EcScriptType::SYLLABOHIEROGLYPH, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::EUROPE,
        u8"Линейное письмо Б", u8"≈1450 до н.э.",
        u8"микенский ''(протогреческий)''",
        u8"Достаточно точно известен момент утверждения на Крите греческой власти{{-}}1450{{bc}} "
                "[[ps:Lina|Линейное письмо А]] начало выходить из использования."
            "<p>Витиеватая форма символов позволяет предположить, что писали на коже или папирусе, тем более линейным письмом А "
                    "действительно писали чернилами. "
                "Но все сохранившиеся памятники выполнены на глиняных табличках, которые обожглись в пожарах и сохранились тысячелетиями. "
                "Письмо использовалось узкой кастой писцов, ориентировавшихся на грамматику минойского языка, а не греческого. "
                "Все сохранившиеся памятники{{-}}канцелярские книги: договоры аренды, списки ремесленников, описи имущества…"
            "<p>Открыто в начале XX{{_}}века Артуром Эвансом. Расшифровано в 1950‑е годы.",
                EcFont::LINEAR_B },
    // W10 has, W7 none (though lots of software install) → installed Google Noto
    { "Lisu", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Лису (алфавит Фрейзера)", u8"1915",
        u8"лису <i>(Китай)</i>",
        u8"Придуман британским миссионером Джеймсом Фрейзером. Состоит из латинских букв, как обычных, так и повёрнутых. "
                "Строчного регистра нет. Благодаря усилиями миссионеров {{%|40}} лису{{-}}протестанты. "
                "Китайцы признали письменность в 1992."
            "<p>До этого лису писали слоговым письмом, напоминающим иероглифы. Также миссионеры использовали алфавит Полларда (мяо) "
                "и письменность на [[ps:Thai|тайской]] основе.",
                EcFont::LISU },
    // Lycian OK, W10 P1 Segoe Historic
    { "Lyci", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Ликийский", u8"VI век до н.э.",
        u8"ликийский ''(Малая Азия)''",
        u8"Придуман под влиянием [[ps:Phnx|финикийского]] и [[ps:Grek|греческого]]. "
                "Так что сходство с греческим алфавитом обманчиво: часть букв финикийские. "
                "Вытеснен греческим с IV до I{{_}}века{{bc}}, когда Ликию завоевал Александр Македонский."
            "<p>Ликия{{-}}не самоназвание, так называли область греки. "
                "Самоназвание языка{{-}}''trm̃mili''."
            "<p>Расшифрован в начале XX{{_}}века.",
                EcFont::GOTHIC },
    // Lydian OK, W10 P1 Segoe Historic
    { "Lydi", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::RTL_MOSTLY, EcContinent::ASIA,
        u8"Лидийский (сардийский)", u8"VII век до н.э.",
        u8"лидийский ''(=сардийский, Малая Азия)''",
        u8"Близок к [[ps:Lyci|ликийскому]], произошёл из [[ps:Phnx|финикийского]]. "
                "Вытеснен греческим около III{{_}}века{{bc}}, когда Лидию завоевал Александр Македонский."
            "<p>Ранние надписи были в обоих направлениях, поздний исключительно справа налево. "
                "Уникальная черта{{-}}треугольная кавычка."
            "<p>Лидия{{-}}не самоназвание, сами лидийцы называли область по столице{{-}}Сарды."
            "<p>Расшифрован в 1916 и изучен хорошо, несмотря на небольшое количество (на 1986{{-}}109{{_}}шт.) надписей.",
                EcFont::GOTHIC },
    // Mahajani OK, W10 none, installed Google Noto
    { "Mahj", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Махаджани", u8"неизвестно (порядка XVII века)",
        u8"хинди, пенджабский, марвари ''(небольшой язык Индии)''",
        u8"Ещё один, как и [[ps:Kthi|кайтхи]], отголосок индийской кастовой системы{{-}}эта письменность использовалась банкирами "
                "и бухгалтерами вплоть до XX{{_}}века, и поныне в крайне малом пользовании."
            "<p>Письменность деградировала почти до [[pt:abjad|консонантной]]: согласная буква означает и «ка», и «ки», и «к». "
                "Если всё-таки гласная важна, есть специальные знаки. "
                "[[pt:virama|Вирам]] и [[pt:ligature|лигатур]] нет. "
                "Зато есть куча сокращений и дробей, важных для бухгалтерских книг. "
                "Многие из них плохо исследованы и потому всё ещё не закодированы."
            "<p>Линия сверху встречается только в заголовках. "
                "Цифры замечены похожие на [[ps:Deva|деванагари]] и [[ps:Gujr|гуджарати]]; "
                    "в Юникоде [[pt:unification|унифицированы]] с первым.",
                EcFont::MAHAJANI },
    // Makasar OK, installed strange font by Anshuman Pandey
    { "Maka", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::OCEAN,
        u8"Макасарский (птичье письмо)", u8"XVII век (возможно, XVI)",
        u8"макасарский ''(южный Сулавеси)''",
        u8"За форму букв письмо получило прозвище «птичье». "
                "Первый известный источник{{-}}подпись на договоре с Голландской Ост-Индской компанией (1667)."
            "<p>Брахмийская абугида из 18 букв без [[pt:virama|вирамы]]. "
                "Начальная гласная{{-}}буква {{sm|𑻱}} (возможно, с огласовками). "
                "Конечные согласные{{-}}''м'', ''нн'' и гортанная смычка{{-}}опускаются. "
                "Между словами не ставятся пробелы. "
                "С такой функциональностью письменность будет неполной{{-}}потому к XIX{{_}}веку была полностью заменена "
                    "[[ps:Bugi|лонтарой]], которая считается сестринской."
            "<p>Цифры встречались как европейские, так и арабо-индийские. "
                "В одной из рукописей встречался знак препинания в виде пальмы, "
                    "[[pt:unification|унифицированный]] с [[pt:emoji|эмодзи]] {{sm|🌴}}.",
                EcFont::MAKASAR },
    // Mandaic OK, W10 has not, installed Google Noto
    { "Mand", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::ENDANGERED, EcWritingDir::RTL, EcContinent::ASIA,
        u8"Мандейский", u8"II—VII век",
        u8"мандейский ''(Иран и Ирак)''",
        u8"Произошла из [[ps:Armi|арамейского]] или [[ps:Prti|парфянского]] письма. "
                "Используется так называемыми «болотными арабами», живущими у слияния рек Тигр и Евфрат "
                    "и исповедующими мандеизм, необычную гностическую религию. "
                "Их количество в Ираке быстро уменьшается с 45&nbsp;000 (1996) до 5000 (2007), "
                    "около 60&nbsp;тыс. разбросаны по миру, и, вероятно, на правах беженцев они быстро исчезнут.",
                EcFont::MANDAIC },
    // Mandaic OK, W10 has not, installed Google Noto
    { "Mani", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::ASIA,
        u8"Манихейская", u8"III век",
        u8"согдийский, парфянский, среднеперсидский",
        u8"Происходит из [[ps:Syrc|сирийского]] и связано с распространением религии манихейства. "
                "Когда сасанидская Персия около 300 изгнала манихеев, источники стали встречаться севернее, в Средней Азии. "
                "Последние источники датируются X{{_}}веком и найдены в современном Синцзян-Уйгурском округе."
            "<p>Термин придумал немец Фридрих Мюллер, он же в 1903 прочитал письмо.",
                EcFont::MANICHAEAN },
    // Meroitic cursive OK, W10 has small part
    { "Merc", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::AFRICA,
        u8"Мероитский курсив", u8"III в. до н.э.",
        u8"мероитский; возможно, древненубийский",
        CT_MEROITIC, EcFont::MEROITIC },
    // Meroitic OK, W10 none
    { "Mero", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::RTL_COL, EcContinent::AFRICA,
        u8"Мероитские иероглифы", u8"III в. до н.э.",
        u8"мероитский; возможно, древненубийский",
        CT_MEROITIC, EcFont::MEROITIC },
    /// @todo [desc] Malayalam
    // Malayalam OK, W7 does not have 2014/17 extensions → installed Google Noto
    { "Mlym", QFontDatabase::Malayalam,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Малаялам", u8"830 (первый источник)",
        u8"малая́лам ''(Южная Индия)''",
        u8"Как и большинство других письменностей Юго-Восточной Азии, произошёл из [[ps:Brah|брахми]]. "
                "Гласная по умолчанию «а». Отличается тем, что в начале слова используются полные огласовки, "
                "в середине и в конце{{-}}сокращённые.",
                EcFont::MALAYALAM },
    { "Modi", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Моди", u8"1200–1600",
        u8"маратхи ''(Западная Индия)''",
        u8"Письмо родственное с [[ps:Deva|деванагари]], но значительно более [[pt:cursive|беглое]]. "
                "Есть две стандартных теории появления: Хемадри Пандит, дипломат и министр (около 1260), "
                    "и Кхандерао Баллал, дипломат (около 1700). "
                "Использовалось вместе с деванагари до 1917, после чего стандартным письмом для маратхи объявили "
                    "''балбодх'', беглый стиль деванагари, существовавший минимум с 1800."
            "<p>Верхнюю черту обычно разлиновывают до того, как начать писать, из-за чего пробелы едва видны. "
                "[[pt:ligature|Лигатур]] на стыках согласных нет, однако методы прикрепления огласовок довольно причудливы "
                    "и сами образуют лигатуры:<br>"
                "{{_}}{{_}}гᵊ {{sm|𑘐}} + у: {{sm|◌𑘴}} = гу: {{sm|𑘐𑘴}}<br>"
                // Because of ligh line, I do not want a paragraph by means of empty line here.
            "Гласная по умолчанию «ə» в маратхи почти стёрлась, и [[pt:virama|вирама]] используется только для других языков: "
                    "хинди, санскрита.",
                EcFont::MODI },
    // Mongol OK, dislike standard font of W10 → installed Google Noto
    { "Mong", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::REVIVED, EcWritingDir::LTR_COL, EcContinent::ASIA,
        u8"Монгольская (старая)", u8"1204",
        u8"монгольский",
        u8"В 1204 монголы завоевали уйгуров и позаимствовали алфавит, восходящий к [[ps:Syrc|сирийскому]]. "
                "Единственная действующая система с записью слева направо по столбцам, из-за чего компьютеры "
                "очень неохотно её поддерживают, заваливая столбец на 90°."
            "<p>В 1587 учёный Аюши-гуши сделал реформу письменности, а также добавил ряд символов для облегчения транскрипции "
                "санскрита, тибетского и китайского, важных в буддизме. Новые символы получили имя ''али-гали'' или ''галик''."
            "<p>В России письменность использовалась калмыками и бурятами. "
                "После войны монголы разработали новую письменность на [[ps:Cyrl|кириллице]], но с развалом СССР есть планы "
                "расширить использование старой письменности."
            "<p>Содержит омографы (например, {{sm|ᠣ}}=o/u), однако закон гармонии гласных{{-}}в слове могут быть или одни твёрдые гласные, "
                "или одни мягкие{{-}}часто позволяют раскрыть эти омографы.",
                EcFont::MONGOLIAN },
    // Meetei Mayek OK, W10 does not have extensions → installed Google Noto
    { "Mtei", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::REVIVED, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Манипури (мейтей-майек)", u8"XI век; по другим данным, 1930 и всё, что раньше,— подделки",
        u8"манипури <i>(Индия)</i>",
        u8"Какая бы версия о происхождении манипури ни была верной, с XVIII века пользовались [[ps:Beng|бенгальским]]. "
                "Письменность возродили в 1930‑х, в 1976 подогнали под современную фонетику."
            "<p>Буквы называются по частям тела: скажем, первая{{-}}«kok» (голова). "
                "Гласная по умолчанию «а». "
                "[[pt:virama|Вирама]] виртуальная и служит для сборки старинных [[pt:ligature|лигатур]]: "
                    "одинокий звук обозначается обрезанными версиями согласных.<br>"
                "{{_}}{{_}}ка {{sm|ꯀ}} + и {{sm|◌ꯤ}} = ки {{sm|ꯀꯤ}}<br>"
                "{{_}}{{_}}к {{sm|ꯛ}}",
                EcFont::MEETEI },
    // Multani OK, W10 none → installed Google Noto
    { "Mult", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Мултани", u8"XVIII век",
        u8"сирайки, синдхи, пенджабский ''(Пакистан, Индия)''",
        u8"Сирайки{{-}}почти поголовно мусульмане. "
                "На 2010‑е их 14…19{{_}}млн по разным источникам только в Пакистане. "
            "<p>Письменность сформировалась в XVIII{{_}}веке. "
                "Первый печатный источник{{-}}Новый завет 1819{{_}}года. "
                "В конце XIX{{_}}века британская администрация придумала [[ps:Arab|арабскую]] письменность, "
                    "и к XX{{_}}веку сирайки приняли её."
            "<p>Все источники называют мултанскую письменность брахмийской абугидой, "
                    "но по факту она деградировала до [[pt:abjad|консонантной]]: "
                    "огласовок нет, гласные пишутся крайне выборочно. "
                "Цифры [[pt:unification|унифицированы]] с [[ps:Guru|гурмукхи]]: есть незначительная разница в цифрах 6 и 7.",
                EcFont::MULTANI },
    // Myanmar OK, W10 “Myanmar Text”, W7 none → installed Google Noto font
    { "Mymr", QFontDatabase::Myanmar,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Бирманская", u8"XI век",
        u8"бирманский ''(Мьянма)'', пали ''(мёртвый, культовый в буддизме)''",
        u8"Как и большинство других письменностей Юго-Восточной Азии, произошёл из брахми. Согласная буква означает слог с «ə». "
                "Чтобы поставить другую гласную, используются огласовки, чтобы убрать{{-}}знак «[[pt:virama|вирама]]», "
                    "которая по-бирмански называется «а-сат»."
            "<p>Раннебирманское письмо было квадратным; "
                "закруглилось оно около XVII{{_}}века, когда распространилось письмо на пальмовых листьях: "
                "прямая линия может разрезать лист по жилкам.",
                EcFont::MYANMAR },
    // Nandinagari OK, though font non-functional
    { "Nand", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Нандинагари", u8"VIII век",
        u8"санскрит, кáннада ''(Южная Индия)''",
        u8"Происходит из нагари, промежуточной формы индийского письма. "
                "Сестра [[ps:Deva|деванагари]], «нандинагари» также означает «письмо священного города». "
                "Если бы не различия{{-}}разорванная черта, другие [[pt:ligature|лигатуры]] и цифры{{-}}"
                    "нанди- можно было бы считать разновидностью деванагари. "
                "Письмо использовалось до XIX{{_}}века."
            "<p>Гласная по умолчанию «а», заменяется на другую [[pt:combining|метками]]-огласовками. "
                "Все метки, включая [[pt:virama|вираму]], не отрываются от букв. "
                "Вирама служит только для передачи санскрита: "
                    "каннада тяготеет к открытым слогам, а редкие стыки согласных передаются лигатурами.<br>"
                "{{_}}{{_}}га {{sm|𑦰}} + у {{sm|◌𑧔}} = гу {{sm|𑦰𑧔}}<br>"
                "Цифры похожи на [[ps:Knda|каннаду]] (есть разница в 2 и 6) и [[pt:unification|унифицированы]] с ней.",
                EcFont::FUNKY },
    // North Arabian OK, W10 none → installed Google Noto
    { "Narb", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::ASIA,
        u8"Древняя североаравийская", u8"III век до н.э.",
        u8"североаравийский оазисов, сафаитский, хисмайский и другие",
        u8"Понятие относится ко всем южносемитским письменностям, за исключением [[ps:Sarb|южноаравийской]]. "
                "Происходят из протосинайской письменности. "
                "Известны по кратким надписям III{{_}}века{{bc}}— IV{{_}}века.",
                EcFont::NORTH_ARABIAN },
    // Nabataean OK, W10 none → installed Google Noto
    { "Nbat", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::ASIA,
        u8"Набатейский", u8"II век до н.э.",
        u8"набатейский",
        u8"Происходит из [[ps:Armi|арамейского]]. "
                "Язык применялся в Набатее (столица Пéтра на территории современной Иордании) со II века{{bc}} "
                "В 106 Траян присоединил Набатею к римской империи, а письменность просуществовала до IV{{_}}века "
                    "и, возможно, стала предком [[ps:Arab|арабского]] через плохо изученное новосинайское письмо. "
                "Между словами нет пробелов. "
                "Числа писались справа налево непозиционной системой счисления, похожей на римские цифры.",
                EcFont::NABATAEAN },
    // Newa OK, W10 none → installed Google Noto
    { "Newa", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Неварский (прахалит)", u8"1654 (первое свидетельство)",
        u8"бхаса ''(=неварский, Непал)'', санскрит, пали",
        u8"Истоки неизвестны{{-}}разные стили письма существовали с X{{_}}века. "
                "Первое свидетельство{{-}}непальский царь Пратап Малла написал в 1654 «письмо невара». "
                "Реально сохранившиеся источники{{-}}XVIII{{_}}век. "
                "В 1905 непальские диктаторы династии Рана запретили язык и письмо. "
                "В 1951 Индия вмешалась, прогнала узурпаторов, посадила старую династию Шах и разрешила снова. "
                "На 2010‑е на бхаса говорят 800{{_}}тыс. человек."
            "<p>Классическая брахмийская абугида. "
                "Несмотря на множество [[pt:ligature|лигатур]] из уменьшенных согласных, [[pt:virama|вирама]] всё-таки встречается.<br>"
                "{{_}}{{_}}ка {{sm|𑐎}} + и {{sm|◌𑐶}} = ки {{sm|𑐎𑐶}}<br>"
                "{{_}}{{_}}ка {{sm|𑐎}} + вирама {{sm|◌𑑂}} = к {{sm|𑐎𑑂}}<br>"
                "{{_}}{{_}}ка {{sm|𑐎}} + вирама {{sm|◌𑑂}} + та {{sm|𑐟}} = кта {{sm|𑐎𑑂𑐟}}",
                EcFont::NEWA },
    // NKo ok, W10 “Ebrima”, W7 lacks a few chars → installed Google Noto
    { "Nkoo", QFontDatabase::Nko,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::RTL, EcContinent::AFRICA,
        u8"Нко", u8"1949",
        u8"языки манден ''(Западная Африка)''",
        u8"Алфавит создал гвинейский писатель Соломана Канте в 1949 году. "
                "Применяется в основном в Гвинее, Кот-д’Ивуаре и Мали. "
                "«N’ko» означает «я говорю». "
                "Буквы соединены.",
                EcFont::NKO },
    // Nushu OK, W10 off, installed Google Noto
    { "Nshu", QFontDatabase::Any,
        EcScriptType::SYLLABLE, EcLangLife::DEAD, EcWritingDir::LTR_CJK, EcContinent::ASIA,
        u8"Нюй-шу", u8"900 (предположительно) … 1300 (точно)",
        u8"один из диалектов сянского ''(Хунань, Китай)''",
        u8"Нюй-шу существовало веками и использовалось как женское письмо: [[ps:Hani|грамоте]] учили мужчин, а женщины "
                "использовали более простое слоговое письмо. "
                "В отличие от размашистых иероглифов, нюй-шу писали тонкой щепкой. "
                "Многие из памятников нюй-шу{{-}}''послания третьего дня'': "
                "девушка уходила замуж в другую деревню, а подруги и сёстры вышивали ей пожелания, часто в стихах. "
            "<p>Несмотря на распространённое заблуждение, нюй-шу не было тайнописью; на него смотрели как на «низкое» письмо."
            "<p>В отличие от японской [[ps:Hira|хираганы]] сходного назначения, нюй-шу так и осталось неофициальным письмом. "
                "К закату нюй-шу привели ликбез и распространение швейных машин в XX{{_}}веке. "
                "К тому же японские оккупанты запрещали нюй-шу, видя в нём шифровку, "
                "а хунвэйбины в Культурную революцию уничтожали памятники старины, в том числе вышивки."
            "<p>Повторно открыли нюй-шу в 1980‑е. Последняя носительница нюй-шу, Ян Хуаньи, умерла в 2004 в возрасте 98{{_}}лет{{-}}"
                "после неё письменностью владели только исследователи. "
                "Вышивки с нюй-шу стали популярным туристским сувениром; опасаются, что туризм исказит древнее письмо."
            "<p>Известно около 1800 разных символов, но многие означали один и тот же слог."
            "<p>«Юникодия» использует шрифт Noto Sans Nushu. Главный типограф{{-}}женщина, француженка китайского происхождения Лиза Хуан.",
                EcFont::NUSHU },
    // Ogham OK, Windows font is differently named in W7 and W10 → installed Google Noto
    { "Ogam", QFontDatabase::Ogham,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::LTR_BU, EcContinent::EUROPE,
        u8"Огамическая", u8"IV век",
        u8"древнеирландский, пиктский",
        u8"Огамической письменностью пользовались древние кельты и пикты, жившие в Ирландии и Британии, с IV до X{{_}}века. "
                "Заодно в ходу была [[ps:Latn|латиница]], потому есть мнение, что это тайнопись. "
                "Письменность приспособлена под зарубки на палках и досках. "
                "Надпись начинается с {{sm|᚛}} и заканчивается {{sm|᚜}}.",
                EcFont::OGHAM },
    // Ol Chiki OK, W10 “Nirmala UI”, W7 none → installed Google Noto
    { "Olck", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Ол-чики", u8"1925",
        u8"сантальский ''(Индия)''",
        u8"Алфавит придумал в 1925 Рагхунатх Мурму. До этого пользовались [[ps:Beng|бенгальской]] письменностью, "
                "[[ps:Orya|орией]] или [[ps:Latn|латиницей]], хотя большинство населения было неграмотным. "
                "Сантальский язык австроазиатский, а не индоевропейский, и алфавиты, основанные на брахми, подходили плохо. "
                "Так, у санталов звонкие и глухие чередуются при словоизменении, потому различать их бессмысленно."
            "<p>Ол-чики состоит из 30 букв, названия соответствуют внешнему виду. Первые три: {{sm|ᱚ}}{{_}}огонь, {{sm|ᱛ}}{{_}}земля, "
                "{{sm|ᱜ}}{{_}}отрыжка.",
                EcFont::OLCHIKI },
    { "Orkh", QFontDatabase::Any,
        EcScriptType::ALPHASYLLABLE, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::ASIA,
        u8"Тюркские (орхоно-енисейские) руны", u8"VI век",
        u8"орхоно-тюркский, енисейско-кыргызский и др. ''(современная Монголия, Прибайкалье, Синьцзян-Уйгурский округ)''",
        u8"Русские исследователи начали собирать информацию ещё с Петра{{_}}I, письмена сразу же сравнили с [[ps:Runr|рунами]], "
                "тем более Даниэлю Мессершмидту действительно помогали пленённые под Полтавой шведы. "
                "Финны поспешили приписать руны своим предкам."
            "<p>Датский учёный Вильгельм Томсен в 1893 расшифровал письмо и посрамил финнов: это тюркский текст."
            "<p>Известно около 200 надписей VII–X{{_}}веков и несколько рукописей IX–X{{_}}веков. "
                "Второй тюркский каганат был 682–744, Уйгурский каганат 744–840. "
                "После поражения от енисейских кыргызов уйгуры ушли на юг и в 1030 были завоёваны [[ps:Tang|тангутами]]. "
                "История кыргызов не вполне понятна; известно только, что в XIII{{_}}веке они стали вассалами Чингисхана.",
                EcFont::ORKHON },
    // Oriya OK, took Noto + everything bad manually fixed
    { "Orya", QFontDatabase::Oriya,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Ория", u8"XIV век (в современном виде)",
        u8"ория ''(Восточная Индия)''",
        u8"Ори́я (традиционно считается однокоренным с ''арийцами'') развилась из нагари (ранней формы [[ps:Deva|деванагари]] "
                "без сплошной черты)."
            "<p>Есть мнение, что округлая форма букв сложилась, потому что писали на пальмовых листьях{{-}}чтобы не разрезать лист "
                "по жилкам, верхняя линия стала дугой. Самый ранний известный лист датируется XV{{_}}веком. "
                "Храмовая надпись XIII{{_}}века всё ещё обладала разорванной линией, как в нагари."
            "<p>Гласная по умолчанию «а», убирается знаком вирамы: {{sm|କ}}=ка, {{sm|କି}}=ки, {{sm|କ୍}}=к. "
                "Но вирама в ории употребляется нечасто из-за большого количества лигатур: {{sm|ମ+ଭ=ମ୍ଭ}}{{_}}мбха. "
                "В одной редакции 1-й статьи Декларации прав человека (стандартного текста для показа чужого языка) "
                "нет вирам вообще, в другой{{-}}одна вирама. "
                "Набор таких лигатур стандартный: первая буква, вирама, вторая буква.",
                EcFont::ORIYA },
    // Osage OK, W10 Gadugi
    { "Osge", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::AMERICA,
        u8"Осейдж", u8"2006",
        u8"осейдж ''(индейский язык группы сиу)''",
        u8"Письменность придумал в 2006 Герман Монгрейн Лукаут для возрождаемого языка осейдж (последняя носительница умерла в 2005). "
                "Он выяснил, что ученики переносили английскую орфографию на индейский. "
                "За основу взята сильно изменённая [[ps:Latn|латиница]]. "
                "Используются европейские цифры и знаки препинания."
            "<p>С 2012, когда началась работа по внесению языка в Юникод, совершена реформа письма.",
                EcFont::GADUGI },
    // Osmanya OK, W10 Ebrima
    { "Osma", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::AFRICA,
        u8"Османья (сомалийский)", u8"1920",
        u8"сомалийский",
        u8"Придуман ≈1920 Османом Юсуфом Кенадидом, братом султана Али Юсуфа Кенадида. "
                "Письменность получила определённое распространение в Сомали, но конкуренция с новоприбывшей [[ps:Latn|латиницей]] "
                    "и традиционным [[ps:Arab|арабским]] была серьёзной."
            "<p>В 1961 османья становится официальным письмом, наряду с латиницей. "
                "В 1972 диктатор Мохаммед Барре запретил османью, в первую очередь из-за распространившихся пишущих машинок."
            "<p>Из-за нестабильной обстановки в Сомали нет современных данных.",
                EcFont::VAI },
    // Plamyrene OK, W10 none, installed Google Noto
    { "Palm", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::ASIA,
        u8"Пальмирский", u8"100 до н.э.",
        u8"пальмирский арамейский",
        u8"Письменность применялась с 100{{bc}} до 300 в районе Пальмиры (современная Сирия). "
                "Происходит из [[ps:Armi|арамейского]]. "
                "В алфавите 22 согласных буквы, одна в двух вариантах. "
                "Пробелов и знаков препинания нет. "
                "Числа записывались непозиционной системой счисления, напоминающей римские цифры, справа налево. "
                "Расшифрована в 1754.",
                EcFont::PALMYRENE },
    // Old Permic OK, W10 none, installed Google Noto
    { "Perm", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::EUROPE,
        u8"Древнепермский (коми-зырянский, анбур)", u8"1372",
        u8"коми-зырянский, коми-пермяцкий",
        u8"Письменность предложена миссионером Стефаном Пермским в 1372 и представляет собой вариацию на тему [[ps:Cyrl|кириллицы]] "
                    "с вкраплением [[ps:Grek|греческого]] и местных рун. "
                "Названа по первым двум буквам, «ан» и «бур». "
                "Иногда использовалась как тайнопись для тогдашнего русского. "
                "В XVII{{_}}веке окончательно вытеснена кириллицей."
            "<p>Анбур{{-}}важный источник по финно-угорским языкам. "
                "Древнее только [[ps:Hung|венгерские руны]].",
                EcFont::OLD_PERMIC },
    // Phags-Pa OK, have “Microsoft PhagsPa” font
    { "Phag", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::HISTORICAL, EcWritingDir::LTR_COL, EcContinent::ASIA,
        u8"Монгольское квадратное (Пагба-ламы)", u8"1269",
        u8"китайский, монгольский, тибетский, санскрит, уйгурский",
        u8"Монгольское письмо использовало уйгурский алфавит с минимальными изменениями. "
                "С захватом монголами Китая у императора Хубилая возникли проблемы с коммуникацией между двором и многонациональным народом, "
                "и эту задачу выполнил Пагба-лама, наставник Хубилая."
            "<p>В 1368 году китайцы выбили монголов, письмо быстро забыли."
            "<p>Если отбросить необычное направление письма, письмо Пагба-ламы{{-}}обычная брахмийская абугида с гласной "
                "по умолчанию «а». В шрифтах письмо обычно рисуется повёрнутым на 90° влево, чтобы писать по строкам.",
                EcFont::PHAGS_PA },
    // Inscriptional Pahlavi OK, W10 Historic
    { "Phli", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::ASIA,
        u8"Пехлевийские надписи", u8"≈150 до н.э.",
        u8"иранские языки",
        u8"Язык иранских надписей на глине, самый ранний вариант письма пехлеви. "
                "Использовался даже в начале Сасанидской империи, до V{{_}}века.",
                EcFont::HISTORIC },
    // Psalter Pahlavi OK, W10 none → installed Google Noto
    { "Phlp", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::ASIA,
        u8"Псалтырское пехлеви", u8"до IV в., точнее неизвестно",
        u8"среднеперсидский",
        u8"Очень беглое письмо, опознанное как промежуточная фаза письма пехлеви. "
                "Есть лишь два источника: 12-страничный фрагмент Псалтыря VI в., найденный в Синцзян-Уйгурском округе, и крест из Афганистана. "
                "Знаки параграфов писались наполовину красным, наполовину чёрным."
            "<p>''Книжное пехлеви'', последняя форма письма, так и не вошло в Юникод, для него оставлена дыра.",
                EcFont::PSALTER_PAHLAVI },
    // Phoenician OK, because of importance installed Google Noto
    { "Phnx", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::ASIA,
        u8"Финикийский", u8"XI век до н.э.",
        u8"финикийский и другие семитские (моавитский, филистимский…), пунический",
        u8"Названия букв [[pt:acrophonic|акрофонические]]: алеф=бык, бет=дом, гимел=верблюд…"
            "<p>Хоть не финикийский, а [[ps:Ugar|угаритский]] алфавит считается первым расшифрованным консонантным "
                    "и первым известным алфавитным порядком, важность финикийского письма трудно переоценить: "
                    "финикийцы были хорошими мореплавателями и разнесли азбуку по древнему миру."
            "<p>Из финикийской письменности развились:<br>"
                "• малоазийские алфавиты (например, [[ps:Lyci|ликийский]]{{-}}исчезли);<br>"
                "• древнеливийская (предок [[ps:Tfng|тифинага]]);<br>"
                "• палеоиспанская, или иберская (исчезла);<br>"
                "• [[ps:Armi|арамейская]] (предок [[ps:Syrc|сирийской]], [[ps:Arab|арабской]] и, "
                    "по общепринятой гипотезе, [[ps:Brah|брахми]]);<br>"
                "• [[ps:Grek|греческая]] (предок [[ps:Latn|латиницы]] и [[ps:Cyrl|кириллицы]])."
            "<p>Таким образом, большая часть грамотного мира обязана финикийцам."
            "<p>Исчезла около 150{{bc}} с разрушением Карфагена; в самой Финикии уже давно ходили письменности-потомки.",
                EcFont::PHOENICIAN },
    // Inscriptional Parthian OK, W10 Historic
    { "Prti", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::ASIA,
        u8"Парфянские надписи", u8"≈250 до н.э.",
        u8"парфянский",
        u8"Использовался в надписях и монетах вплоть до начала Сасанидской империи (III{{_}}век). "
                "Повлиял на [[ps:Armn|армянский]], но впоследствии уступил персидскому. "
                "Содержит семь [[pt:ligature|лигатур]], которые не удалось извлечь из имеющегося шрифта.",
                EcFont::HISTORIC },
    // Rejang OK, installed Google Noto font
    { "Rjng", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::OCEAN,
        u8"Реджанг", u8"≈1750 (первый известный документ)",
        u8"реджанг ''(Суматра)''",
        u8"На языке реджанг говорят 200{{_}}тыс. человек (2000), большинство из них неграмотно. "
                "Полностью заменена [[ps:Latn|латиницей]]."
            "<p>Перед нами обычная брахмийская абугида с гласной по умолчанию «а» и вирамой: "
                "{{sm|ꤰ}}=ка, {{sm|ꤰꥉ}}=ke, {{sm|ꤰ꥓}}=k. "
                "Письменность деградировала по той же причине, что и [[ps:Runr|руны]]: проще выцарапывать на дереве.",
                EcFont::REJANG },
    // Rohingya, installed Google Noto font
    { "Rohg", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::RTL, EcContinent::ASIA,
        u8"Ханифи (рохинджа)", u8"1980-е",
        u8"рохинджа ''(Бангладеш, Мьянма)''",
        u8"Рохинджа или руэйнджа{{-}}небольшой преимущественно исламский народ (2{{_}}млн), "
                    "раскиданный по Бангладеш, Мьянме и сопредельным странам. "
                "С XIX{{_}}века известно письмо [[ps:Arab|арабицей]]."
            "<p>С 1980 проповедник Мохаммад Ханиф придумал более подходящую письменность для рохинджа. "
                "В письме используются собственные десятичные цифры. "
                "Буквы соединены, как в арабице, попробуйте расшифровать самоназвание:"
            "<p>{{sm|𐴌𐴗𐴥𐴝𐴙𐴚𐴒𐴙𐴝}}",
                EcFont::ROHINGYA },
    // Runes OK, several tofu → installed Google Noto font
    { "Runr", QFontDatabase::Runic,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::EUROPE,
        u8"Руны", u8"I век",
        u8"древние германские и скандинавские языки",
        u8"Руны имеют специфическую угловатую форму, приспособленную для вырезания: вдоль и поперёк волокон дерево сопротивляется по-разному, "
                    "потому сложно делать кривые линии. "
                "Позднейшие варианты рун видоизменились под скоропись пером. "
                "Найдено около 5000 памятников рунического письма, самые старые относятся к I{{_}}веку. "
                    "Скандинавский рунический алфавит называют «футарк», по первым шести буквам."
            "<p>С появлением христианства руны были вытеснены [[ps:Latn|латиницей]]: "
                    "в Западной Европе к XI{{_}}веку, в Скандинавии до XV, "
                    "а в отдельных деревнях{{-}}и до XX, постепенно заменяясь латиницей."
            "<p>Фашистская Германия любила эзотерику, а вместе с ней и руны. "
                "Популярны они в нью-эйдже и неоязычестве."
            "<p>В массовой культуре рунами называют письмена произвольной формы, в том числе непригодные для вырезания. "
                "Алфавиты древних [[ps:Orkh|тюрков]], булгар, [[ps:Hung|венгров]] сходной угловатой формы также называют рунами."
            "<p>Блок кодирует алфавиты скандинавских и англосаксонских рун, частично пересекающиеся. "
                "В 2014 добавили три руны для записи современного английского, приписываемые Дж.{{_}}Толкину, "
                "и пять рун с ларца Фрэнкса, реликвии VIII{{_}}века.",
                EcFont::RUNIC },
    // Samaritan OK, installed Google Noto font, added dotted circle
    { "Samr", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::ENDANGERED, EcWritingDir::RTL, EcContinent::ASIA,
        u8"Самаритянский", u8"около 600—200 до н.э.",
        u8"иврит, самаритянский арамейский",
        u8"Происходит из палеоеврейского письма. По Библии, самаритяне пришли в Палестину из Двуречья и приняли еврейскую "
                "религию и культуру. На 2021 существует не более 700 самаритян.",
                EcFont::SAMARITAN },
    // South Arabian OK, W10 Segoe UI Historic
    { "Sarb", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::RTL_MOSTLY, EcContinent::ASIA,
        u8"Древняя южноаравийская", u8"IX век д.э.",
        u8"южноаравийские ''(сабейский, катабанский и прочие)'', геэз",
        u8"Предок [[ps:Ethi|эфиопского]] письма, происходит из протосинайской письменности. "
                "В VII{{_}}веке с экспансией арабов вытеснено [[ps:Arab|арабицей]].",
                EcFont::GOTHIC },
    // Saurashtra OK, W10 none, installed Google Noto
    { "Saur", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Саураштра", u8"XIX век (возможно, раньше)",
        u8"саураштра ''(Индия, письменность используется только в штате Тамилнад)''"sv,
        u8"Происхождение неясно, все [[ps:Taml|тамильские]] саураштра двуязычны, а в остальных штатах пишут на "
                "[[ps:Deva|деванагари]], [[ps:Gujr|гуджарати]], [[ps:Telu|телугу]]. "
                "По переписи 2007, в Индии 247{{_}}тыс. саураштра."
            "<p>Перед нами обычная брахмийская абугида с гласной по умолчанию «а» и вирамой: "
                "{{sm|ꢒ}}=ка, {{sm|ꢒꢶ}}=ки, {{sm|ꢒ꣄}}=к. Вирама употребляется часто. "
                "Начальная гласная{{-}}отдельный символ: {{sm|ꢄ}}=и.",
                EcFont::SAURASHTRA },
    // Sutton SignWriting OK, installed Google Noto font
    /// @todo [tool] Make a SignWriting tool!
    { "Sgnw", QFontDatabase::Any,
        EcScriptType::NONTEXT, EcLangLife::NEW, EcWritingDir::TD, EcContinent::NONE,
        u8"Жестовое письмо Саттон", u8"1974",
        u8"жестовые языки, в частности амслен <i>(язык американских глухих)</i>",
        u8"Систему придумала в 1974 танцовщица Валери Саттон на основе созданной двумя годами ранее системы записи танцев. "
                "Впоследствии она же придумала системы для записи пантомимы, других видов спорта (фигурного катания, каратэ…), "
                    "физических упражнений, языка тела людей и животных, которые, впрочем, не были столь успешны."
            "<p>Не указано, с чьей точки зрения пишутся жесты{{-}}исполнителя или зрителя. "
                "Но большинство публикаций ориентируются на исполнителя. "
                "Символов очень много и передают движения пальцев, рук, плеч, взгляда, а также прикосновения и выражения лица. "
                "Для ладони есть кулак (квадрат), несжатый кулак (круг), открытая ладонь (пятиугольник), ребро (прямоугольник) и другое. "
                "Один и тот же жест можно передать несколькими способами."
            "<p>Хотя система не является строго [[pt:universality|текстовой]]{{-}}положение жестов на странице ''играет'' роль{{-}}"
                    "её в 2015 добавили в Юникод. "
                "Добавили только символы, без какого-либо форматирования. "
                "Но существует много программ, фиксирующих координаты X/Y на листе{{-}}например, векторные редакторы. "
            "<p>Жесты другой рукой, в другую сторону, с поворотом задаются комбинирующими метками{{-}}"
                    "любая из Fill{{_}}2…6, и одновременно любая из Rotation{{_}}2…16. "
                "Так, чтобы передать «моргалы выколю» {{sm|𝠎𝪟𝦆}}, надо написать 1D80E+Fill6+1D986.",
                EcFont::SIGNWRITING },
    // Shavian OK, W10 Segoe UI Historic
    { "Shaw", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::EXPERIMENTAL, EcWritingDir::LTR, EcContinent::EUROPE,
        u8"Алфавит Бернарда Шоу", u8"1960",
        u8"английский, эсперанто",
        u8"Ирландский писатель Джордж Бернард Шоу работал в комитете Би-Би-Си по произношению и увлекался стенографией. "
                "Так что он завещал сделать фонетический алфавит для английского, "
                    "причём далёкий от [[ps:Latn|латиницы]], чтобы не{{_}}казаться «олбанским». "
                "Конкурс выиграл Рональд Кингсли Рид."
            "<p>Алфавит состоит из трёх видов букв: низких, высоких и глубоких. "
                "Низкие буквы{{-}}гласные, плавные (r, l) и носовые согласные, высокие{{-}}глухие согласные. "
                "Перевёрнутая на 180° высокая буква становится глубокой и означает соответствующую звонкую (кроме пары ng/h)."
            "<p>Первым произведением, вышедшим в новом алфавите, стала пьеса Шоу «Андрокл и лев»{{-}}в билингве с английским (1962). "
                "В 2013 вышла первая не-билингва{{-}}«Алиса в стране чудес». "
                "В любом случае публикации в новом алфавите единичны.",
                EcFont::HISTORIC },
    // Sharada OK, W10 none → installed Google Noto
    { "Shrd", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::SACRED, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Шарада", u8"700",
        u8"санскрит, кашмирский",
        u8"Шарада происходит из гупты (северной ветви [[ps:Brah|брахми]]), была распространена в VIII–XII{{_}}веке "
                    "в северо-западной Индии и Афганистане. "
                "После этого шарада становилась всё менее пригодной для записи кашмирского и вышла из употребления. "
                "Предок письменности [[ps:Guru|гурмукхи]]."
            "<p>В настоящее время употребляется кашмирскими пандитами (индуистским меньшинством среди мусульман-кашмирцев) "
                "и пишущими на санскрите индийскими буддистами."
            "<p>Перед нами обычная брахмийская абугида с гласной по умолчанию «а». "
                "[[pt:virama|Вирама]] используется только для передачи санскрита: "
                    "в середине слов не нужна из-за особенностей кашмирского, а в конце опускается.<br>"
                    "{{_}}{{_}}ка {{sm|𑆑}} + и {{sm|◌𑆴}} = ки {{sm|𑆑𑆴}}<br>"
                    "{{_}}{{_}}ка {{sm|𑆑}} + вирама {{sm|◌𑇀}} = к {{sm|𑆑𑇀}}",
                EcFont::SHARADA },
    // Siddham OK, W10 none, installed Google Noto font
    { "Sidd", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::SACRED, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Сиддхаматрика (сиддхам, бондзи)", u8"VI век",
        u8"санскрит",
        u8"Средневековое североиндийское письмо, появившееся из [[ps:Brah|брахми]] и связанное с буддизмом. "
                "Часта запись кистью на китайский манер. "
                "Между словами не ставятся пробелы. "
            "<p>Распространилось по всему буддийскому Востоку, включая Японию, но в IX{{_}}веке Китай устроил несколько "
                    "чисток «иностранных религий», отрезав Японию от материковых источников. "
                "К 1200 сиддхам заменился другими шрифтами и в настоящее время сохранился только в японском буддизме "
                    "под названием ''бондзи''."
            "<p>В японском бондзи нет цифр, а под староиндийские формы цифр оставили место. "
                "Закодировано много свойственных религиозным текстам знаков препинания и украшений."
            "<p>Обычная брахмийская абугида. "
                "[[pt:virama|Вирама]] крайне редка и встречается только на конце слов "
                    "из-за большого количества [[pt:ligature|лигатур]]{{-}}букв, записанных сверху вниз.<br>"
                "{{_}}{{_}}ка {{sm|𑖎}} + и {{sm|◌𑖰}} = ки {{sm|𑖎𑖰}}<br>"
                "{{_}}{{_}}ка {{sm|𑖎}} + вирама {{sm|◌𑖿}} = к {{sm|𑖎𑖿}}<br>"
                "{{_}}{{_}}ка {{sm|𑖎}} + вирама {{sm|◌𑖿}} + та {{sm|𑖝}} = кта {{sm|𑖎𑖿𑖝}}",
                EcFont::SIDDHAM },
    // Sindhi (Khudawadi) OK, W10 none, installed Google Noto font
    { "Sind", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Кхудабади (синдхи)", u8"XVI век",
        u8"синдхи ''(Пакистан, Индия)''",
        u8"Письмо названо в честь пакистанского города Кхудабад. "
                "В 1868 году объявлено британской администрацией основным письмом синдхов. "
                "К XX{{_}}веку синдхи, преимущественно мусульмане, перебрались на [[ps:Arab|арабский]], "
                    "но письмо живо и поныне. "
                "Также пользуются [[ps:Deva|деванагари]] и [[ps:Guru|гурмукхи]], исторически использовали [[ps:Khoj|ходжки]]. "
                "На 2010‑е язык синдхи является родным для 32{{_}}млн человек. "
            "<p>Обычная брахмийская абугида с гласной по умолчанию «а» и часто применяемой [[pt:virama|вирамой]]:<br>"
                "{{_}}{{_}}ка {{sm|𑊺}} + и {{sm|◌𑋡}} = ки {{sm|𑊺𑋡}}<br>"
                "{{_}}{{_}}ка {{sm|𑊺}} + вирама {{sm|◌𑋪}} = к {{sm|𑊺𑋪}}",
                EcFont::KHUDAWADI },
    // Sinhala OK, W10 obviously has no 2020 extension → installed Google Noto
    /// @todo [desc, BMP] interesting script, describe
    { "Sinh", QFontDatabase::Sinhala,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::OCEAN,
        u8"Сингальский", u8"300",
        u8"сингальский ''(Шри-Ланка)''",
        u8"Как и большинство других письменностей Юго-Восточной Азии, произошла из брахми. Буквы имеют витиеватую форму, "
                "потому что в древности часто писали на пальмовых листьях, а прямые линии разрезали бы лист по жилкам.",
                EcFont::SINHALA },
    // Sogdian OK, W10 None → installed Google Noto
    { "Sogd", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::SOGDIAN, EcContinent::ASIA,
        u8"Согдийский", u8"≈500",
        u8"согдийский ''(Самарканд)''", CT_SOGDIAN, EcFont::SOGDIAN },
    // Old Sogdian OK, W10 None → installed Google Noto
    { "Sogo", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::ASIA,
        u8"Старосогдийский", u8"≈100",
        u8"согдийский ''(Самарканд)''", CT_SOGDIAN, EcFont::OLD_SOGDIAN },
    // Sora Sompeng OK, W10 Nirmala UI
    { "Sora", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Соранг-сомпенг", u8"1936",
        u8"сора ''(Индия)''",
        u8"Сора или савара{{-}}небольшой (400{{_}}тыс. на 2011) австроазиатский (не индоевропейский!) язык Восточной Индии. "
                "Письменность придумал в 1936 Мангей Гоманго, до этого писали [[ps:Latn|латиницей]]. "
                "Позднее для сора придумали чисто алфавитную (без огласовок и [[pt:virama|вирам]]) письменность "
                    "на основе [[ps:Orya|ории]] и [[ps:Telu|телугу]].",
                EcFont::TAMIL },
    // Sundanese OK, even W10 off → installed Google Noto
    { "Sund", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::REVIVED, EcWritingDir::LTR, EcContinent::OCEAN,
        u8"Сунданский", u8"XIV век",
        u8"сунданский ''(Ява)''",
        u8"Язык сундов (38{{_}}млн на 2007), распространённый на западе Явы. Обычная брахмийская абугида с гласной по умолчанию «а». "
                "Для других гласных используются огласовки, для голой согласной{{-}}знак «[[pt:virama|вирама]]» ''(pamaaeh)'': "
                    "{{sm|ᮞ}}=са, {{sm|ᮞᮥ}}=су, {{sm|ᮞ᮪}}=с. "
                "Начальные гласные{{-}}отдельные буквы. "
            "<p>Письменность использовалась до XVIII{{_}}века, после этого стали использовать [[ps:Java|яванскую]] и "
                "пегон (местный вариант [[ps:Arab|арабского]]); с XX века{{-}}[[ps:Latn|латиницу]]. "
                "В 1997 году власти Индонезии решили дать сундам собственную письменность, сделав её из старосунданского."
            "<p>Вторая крестообразная вирама служит исключительно для набора исторических [[pt:ligature|лигатур]]. "
                "В имеющемся шрифте лигатур нет, но автор «Юникодии» нарисовал одну в [[pt:private|личной]] позиции:<br>"
                    "{{_}}{{_}}{{_}}{{_}}ка {{sm|ᮊ}} + вирама {{sm|◌᮫}}{{_}} + та {{sm|ᮒ}} = кта {{sm|\uE000}}",
                EcFont::SUNDANESE },
    // Syloti Nagri OK, W10 none → installed Google Noto
    { "Sylo", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Силхети-нагари", u8"XV век",
        u8"силхетский ''(Бангладеш, Индия)''",
        u8"По одной из теорий, письменность произошла из кайтхи и изобретена мусульманами. За простоту письменность была любима поэтами "
                "XIX{{_}}века, но к середине XX{{_}}века уступила [[ps:Beng|бенгальской]]. "
                "В 2000‑е годы в Бангладеш начали снова изучать силхети. "
                "На 2007 было 11{{_}}млн силхетов."
            "<p>Обычная брахмийская абугида с гласной по умолчанию «о» и вирамой ''(хасанта)'': {{sm|ꠇ}}=ко, {{sm|ꠇꠣ}}=ка, {{sm|ꠇ꠆}}=к. "
                "Существует и другая запись вирамы, {{sm|ꠇ꠬}}=к, на манер других брахмийских письменностей. "
                "Начальные гласные пишутся отдельными буквами. "
                "Текст свисает с общей черты{{-}}но, в отличие от [[ps:Deva|деванагари]], крепко за неё «держится»."
            "<p>Вирама в силхети нечаста по двум причинам. Во-первых, двойные согласные и некоторые стыки образуют лигатуры: "
                "{{sm|ꠇ\uA806ꠇ}}=кко, {{sm|ꠇ\uA806ꠔ}}=кто. Набор таких лигатур стандартный: первая буква, вирама, вторая буква. "
                "Кроме того, иногда конечная «о» становится немой."
            "<p>Запись слева направо, но силхетские книги, как дань исламу, сшиты справа налево.",
                EcFont::SYLOTI_NAGRI },
    // Syriac OK, W10 has no Malayalam extensions → Beth Mardutho made a nice font.
    { "Syrc", QFontDatabase::Syriac,
        EcScriptType::CONSONANT, EcLangLife::ENDANGERED, EcWritingDir::RTL, EcContinent::ASIA,
        u8"Сирийский"sv, u8"I век",
        u8"сирийский <i>(исп. как литургический)</i>, новоарамейские, малаялам, согдийский"sv,
        u8"Потомок [[ps:Armi|арамейского]] алфавита, впоследствии развившийся в [[ps:Arab|арабицу]]. "
                "Используется малыми семитскими народами. В сирийском письме уже видны некоторые особенности арабицы: "
                "сплошная вязь, выписываемая (кроме огласовок) одним росчерком, разные формы изолированной, начальной, внутренней "
                "и конечной буквы."
            "<p>Тот вид письма, которым написаны раннехристианские рукописи (50–400), "
                    "известен под названием эстрангело (греч. στρονγύλη{{-}}круглое). "
                "Западная Сирия (Сирия, Палестина) принадлежала грекам, а восточная (Персия, Двуречье){{-}}персам. Это разделило церковь, "
                "язык и письменность на две формы: восточносирийскую (несторианскую, халдейскую) и  западносирийскую (яковитскую, маронитскую). "
                "В них используются разные почерки и огласовки."
            "<p>В системе, скорее всего, найдётся эстрангело{{-}}он принят основным в западной историографии. "
                "А вот с «Юникодией» для разнообразия поставляется западный вариант{{-}}его отлил дипломат и печатник Савари́ де Брев "
                "(1560–1627), а довёл до компьютерного шрифта институт ''Beth Mardutho'' для издания Антиохийской Библии.",
                EcFont::SYRIAC },
    // Tagbanwa OK, W10 off → installed Google Noto
    { "Tagb", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::OCEAN,
        u8"Тагбанва", u8"≈1300",
        u8"языки о. Палаван ''(Филиппины)''",
        u8"Восходит к [[ps:Brah|брахми]] и родственная с [[ps:Tglg|тагальской]]. "
                "Осталось не более 25000 людей, говорящих на этих языках. "
                "Новые поколения предпочитают говорить по-тагальски."
            "<p>[[pt:virama|Вирамы]] и каких-то знаков для заключительных букв нет, так что эти буквы угадываются из контекста, "
                    "как в [[ps:Buhd|бухиде]].",
                EcFont::TAGBANWA },
    // Takri OK, W10 none → installed Google Noto
    { "Takr", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::OCEAN,
        u8"Такри", u8"≈XVI век",
        u8"малые языки Северной Индии и Пакистана",
        u8"Происходит из [[ps:Shrd|шарады]]. "
                "Ранее такри широко применялся в делопроизводстве, "
                    "но с послевоенным освобождением Индии стал быстро заменяться [[ps:Deva|деванагари]]. "
                "Есть попытки возродить письменность."
            "<p>В каждом штате свой стиль такри. "
                "Иногда [[ps:Dogr|догра]] считается одним из таких стилей, но [[pt:unification|разунифицирована]] с такри "
                    "как по политическим, так и по техническим причинам. "
                "[[pt:virama|Вирамы]] нет ни в одном из местных языков, это видно по самоназванию {{sm|𑚔𑚭𑚊𑚤𑚯}} «тта:‑кᵊ‑ри:». "
                    "Но она употребляется для древних языков{{-}}пали и санскрита. "
                "Только три буквы могут входить в [[pt:ligature|лигатуры]] вторым знаком, «йа», «ра» и «ха»: {{sm|𑚞𑚶𑚤}} прᵊ.",
                EcFont::TAKRI },
    // Old Tai Le OK, M$ has a good font
    { "Tale", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Лы", u8"≈1200",
        u8"дайские (=лы; ''Китай, Вьетнам, Лаос, Мьянма, Таиланд'')",
        u8">Как и большинство других письменностей Юго-Восточной Азии, произошла из брахми. Тона изначально не записывались. "
                "После нескольких реформ XX{{_}}века (последняя{{-}}1988), в том числе неудачной попытки китайских властей "
                "создать [[ps:Talu|новое письмо лы]], письменность почти что превратилась в алфавитную, и абугидой её "
                "оставляют «пустая» согласная {{sm|ᥟ}} в начале слова, если оно начинается с гласной, и тоновые буквы "
                "после конечной согласной слога (если таковая есть)."
            "<p>Пробелы пишутся между предложениями, но не между словами. "
            "<p>В лы третьей реформы (1963—1988) для записи тонов использовались обычные европейские умляуты: "
                "<big>à, á, ȧ, ä, ǎ</big>."
            "<p>Дайцев в Китае ≈500 тыс. (2001), но язык быстро заменяется китайским и потому отнесён к уязвимым. "
                "Ещё не более 500 тыс. разбросано по другим странам, они пользуются в основном [[ps:Lana|ланной]].",
                EcFont::TAILE },
    // New Tai Lue OK, M$ has a good font
    { "Talu", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::EXPERIMENTAL, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Новое письмо лы", u8"1950-е",
        u8"дайские (=лы; ''Китай; общины лы в других странах пользуются старым письмом'')",
        u8"Созданный в 1950‑е годы китайскими властями алфавит встретил сопротивление народа, и Китай пошёл по пути "
                "постепенного упрощения [[ps:Tale|старого письма лы]]."
            "<p>Гласная по умолчанию «а». Вирама имеет вид крючка с петлей и в Юникоде записывается как единое целое с согласной: {{sm|ᧇ}}. "
                "Необычно устроена запись тонов: согласные бывают в двух регистрах, ещё два знака {{sm|ᧈᧉ}} могут "
                "приписываться к слогу в конце, итого 2·3=6 тонов.",
                EcFont::TAILUE },
    // Tamil BMP OK, different fonts in W10 and W7
    { "Taml", QFontDatabase::Tamil,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Тамильский", u8"VI—IX век",
        u8"тамильский ''(Индия, Шри-Ланка, Сингапур)''",
        u8"Письменность произошла из [[ps:Brah|брахми]] через паллаву (старую [[ps:Gran|грантху]]). "
                "Гласная по умолчанию «а». "
                "[[pt:virama|Вирама]] употребляется очень часто и превратилась в точку. "
                "Методы прикрепления огласовок к согласным довольно причудливы: ка={{sm|க}}, ки={{sm|கி}}, тта={{sm|ட}}, тти={{sm|டி}}. "
                "Начальная гласная записывается отдельным знаком: и={{sm|இ}}."
            "<p>Местные цифры чаще встречаются в текстах на грантхе{{-}}а тамилы предпочитают обычные 0…9. "
                "В отличие от других индийских систем счёта, есть специальные символы для 10, 100 и 1000. "
                "До того, как правительство Индии утвердило знак рупии ₹, у тамилов он существовал: {{sm|௹}}."
            "<p>Дравидийский Юг никогда не покорялся мусульманам и даже создал шиваизм, особо ортодоксальную ветвь индуизма."
            "<p>В русском заимствований из тамильского мало, наиболее известное{{-}}''катамаран''. "
                "В английском также ''cash'' (касса, наличные), ''ginger'' (имбирь), ''coolie'' (чернорабочий) и другие.",
                EcFont::TAMIL },
    // Tangut OK, installed Google Noto
    { "Tang", QFontDatabase::Any,
        EcScriptType::HIEROGLYPH, EcLangLife::HISTORICAL, EcWritingDir::TANGUT, EcContinent::ASIA,
        u8"Тангутский", u8"1036",
        u8"тангутский ''(северо-западный Китай)''",
        u8"Очень сложная иероглифическая система, существовавшая в тангутском царстве Си Ся (северо-западный Китай). "
                "Самые простые иероглифы состоят из 4{{_}}черт, в иероглифе «один» их 6. "
                "Наиболее распространённая версия{{-}}письмо разработали около 1036 император Ли Юань‑хао и учёный Ири Жэнь‑юн."
            "<p>Как и у [[ps:Hani|китайцев]], у тангутов была развита каллиграфия, у одного иероглифа было до 4 форм. "
                "В Юникоде они закодированы одним символом, если не встречаются в одном источнике. "
                "Знаки, имеющие одинаковую форму, но разные названия, также объединены."
            "<p>Тангутское государство уничтожено Чингисханом в 1227, но письмо продолжало использоваться ещё несколько веков: "
                "самый поздний памятник{{-}}1502, также существует рукописная помета позднее 1680."
            "<p>Изучать мёртвое письмо начали европейцы с 1870, ключевую роль сыграл советский учёный Николай Невский."
            "<p>Из-за вопросов с авторскими правами иконки оригинальные (прорисовал проф. Цзин Юнши), "
                    "а шрифт прямой современный, перерисованный компанией Google.",
                EcFont::TANGUT },
    // Tai Viet OK, installed Google Noto font
    { "Tavt", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Тай-вьет", u8"XVI век",
        u8"языки чёрных тай ''(Вьетнам)''",
        u8"Происходит из [[ps:Thai|старотайского]] (царство Сукхотаи). "
                "Впервые стандартизирована в 1961, позднее под эгидой ЮНЕСКО в 2006."
            "<p>Гласной по умолчанию нет, однако письменность остаётся абугидой из-за причудливых огласовок слева, сверху и справа:"
                "{{sm|ꪶꪀ}}=ко, {{sm|ꪀꪱ}}=ка, {{sm|ꪀꪲ}}=ки. "
                "Традиционно читатель должен был угадывать тон, уже в первой стандартизации добавили тоновые знаки.",
                EcFont::TAI_VIET },
    // Telugu OK, W7 lacks recent extensions → installed Google Noto
    /// @todo [desc, BMP] Telugu
    { "Telu", QFontDatabase::Telugu,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Телугу", u8"XIV век",
        u8"телугу ''(Юго-Восточная Индия)''",
        u8"Как и большинство других письменностей Юго-Восточной Азии, произошла из брахми.",
                EcFont::TELUGU },
    // Tifinagh OK, W7 “Ebrima” of course has no 2012 chars → installed Google Noto
    { "Tfng", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::AFRICA,
        u8"Тифинаг (берберский)", u8"не позднее III в. (древняя консонантная), XX в. (современная)",
        u8"туарегские и другие берберские ''(Марокко)''",
        u8"Происхождение староливийского письма неизвестно. Самый старый памятник{{-}}погребение царицы Тин-Хинан (IV{{_}}в.). "
                "Письмо было [[pt:abjad|консонантное]], писали чаще всего снизу вверх или справа налево."
            "<p>В XX веке из староливийского создали новую письменность, с написанием слева направо. Исламские государства "
                "(в частности, Ливия Каддафи и Марокко 80‑х) запрещали письменность, но в 2003 король Марокко разрешил тифинаг. "
                "Переходный совет Ливии также разрешил берберские языки и тифинаг.",
                EcFont::TIFINAGH },
    // Tagalog OK, W10 off → installed Google Noto
    { "Tglg", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::OCEAN,
        u8"Тагальский (байбайин)", u8"1300 (спорно) — 1500 (точно)",
        u8"старотагальский и другие языки Явы и Филиппин",
        u8"Обычная брахмийская абугида с гласной по умолчанию «а» и [[pt:virama|вирамой]]. "
                "Обойдусь самоназванием письменности: {{sm|ᜊᜌ᜔ᜊᜌᜒᜈ᜔}} «ба-й-ба-йи-н»."
            "<p>Испанские миссионеры (XVI{{_}}век) увидели, что островитяне поголовно грамотны, и сами стали учить их на байбайине. "
                "При этом не сразу поняли, в каком направлении пишут: текст вырезали на бамбуковых дощечках, направляя нож от себя, "
                    "потому могли писать в одном направлении и читать в другом."
            "<p>Причин упадка байбайина много: испанцы завезли бумагу, перо и печать, под их влиянием аборигены стали различать звуки "
                    "r/d, u/o, i/e. "
                "А главное: [[ps:Latn|латиница]] помогает в жизни, позволяя получить престижную конторскую работу. "
                "Так что в XVIII{{_}}веке обнаружили, что аборигены переняли латиницу. "
                "К XIX{{_}}веку байбайин был мёртв, и оставалось только упорядочить латинский алфавит. "
                "Есть современные попытки возродить байбайин.",
                EcFont::TAGALOG },
    // Thaana OK, Monotype/M$ “MV Boli” font
    { "Thaa", QFontDatabase::Thaana,
        EcScriptType::ARGUABLE, EcLangLife::ALIVE, EcWritingDir::RTL, EcContinent::OCEAN,
        u8"Тана", u8"XVIII век",
        u8"дивехи ''(мальдивский)''",
        u8"Письменная культура пришла на Мальдивские острова вместе с буддизмом, появилось письмо ''эвела акуру'' на основе "
                    "[[ps:Sinh|сингальского]]. "
                "Самая старая надпись датируется VII—VIII веком. "
                "В 1153 году некто Мухаммад ул-Абдалла провозгласил себя султаном и обратил мальдивцев в ислам; "
                    "к XIV веку письмо видоизменилось в ''[[ps:Diak|дивес акуру]]'', но так и осталось слева направо. "
                "В 1703 году султан Ибрахим{{_}}III заменил старую графику новой{{-}}''габули таной'' (стандартным письмом)."
            "<p>Происхождение таны уникально: согласные произошли из арабских и доисламских цифр, гласные{{-}}"
                    "из [[ps:Arab|арабских]] огласовок. "
                "Со временем линии сгладились, стали писать с изящным 45-градусным наклоном. "
                "Путешественник Тур Хейердал сравнил тану с нарезанной лапшой."
            "<p>В 1970‑е годы тане грозило исчезновение: при президенте Ибрагиме Насире появилась телексная связь, "
                    "и правительство перешло на довольно некачественную латинскую транслитерацию. "
                "Новый президент Момун Абдул Гайюм восстановил тану, и обе письменности используются параллельно.",
                EcFont::THAANA },
    // Thai OK, W7 and 10 fonts are different, but managed to
    { "Thai", QFontDatabase::Thai,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Тайский", u8"1283",
        u8"тайский",
        u8"Произошла из [[ps:Khmr|старокхмерского]], который, в свою очередь, произошёл из [[ps:Brah|брахми]]. "
                "Алфавит упорядочил и утвердил король Рамакхамхаенг Великий в 1283 году."
            "<p>Гласная по умолчанию «а» в открытых слогах и «о» в закрытых: {{sm|ถนน}}=тханон (дорога). "
                "Но буквы называются по словам: так, буква {{sm|ช}} называется «чо-чан» (слон). "
                "Между словами не ставятся пробелы.",
                EcFont::THAI },
    // Tibetan OK, W10 did not have swastika, installed Google Noto
    { "Tibt", QFontDatabase::Tibetan,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Тибетская", u8"≈650",
        u8"тибетские, дзонг-кэ ''(Бутан)'', шерпский…",
        u8"Как и большинство других письменностей Юго-Восточной Азии, произошла из брахми, гласная по умолчанию «а»."
            "<p>Считается, что письменность изобрёл Тхонми Самбхота, съездивший в Индию и заложивший основы тибетского буддизма. "
                "За это время орфография мало менялась, а язык изменился, потому произношение сильно расходится с написанием.",
                EcFont::TIBETAN },
    // Tirhuta OK, W10 none, installed Google Noto
    { "Tirh", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::COMPETING, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Тирхута (митхилакшар)", u8"XIII век",
        u8"майтхили ''(Индия)'', санскрит",
        u8"Письмо близко к [[ps:Beng|бенгальскому]], многие символы похожи, "
                    "но [[pt:unification|унификация]] невозможна из-за других [[pt:ligature|лигатур]]. "
                "Первый металлический шрифт сделан в 1920‑е годы, первый цифровой в 1990‑е. "
                "Большинство говорящих, коих 34 млн на 2000, в XX{{_}}веке перешли на [[ps:Deva|деванагари]], "
                    "и письмо употребляется в основном в индуизме. "
                "Долго майтхили считался диалектом хинди или бенгальского, но в 2003 его объявили отдельным языком, "
                    "основной письменностью оставили деванагари."
            "<p>Обычная брахмийская абугида с гласной по умолчанию «а». "
                "[[pt:virama|Вирама]] употребляется только для санскрита: "
                    "стыки согласных записываются лигатурами, а в конце слова гласная немеет.<br>"
                "{{_}}{{_}}ка {{sm|𑒏}} + и {{sm|◌𑒱}} = ки {{sm|𑒏𑒱}}<br>"
                "{{_}}{{_}}ка {{sm|𑒏}} + вирама {{sm|◌𑓂}} + та {{sm|𑒞}} = кта {{sm|𑒏𑓂𑒞}}",
                EcFont::TIRHUTA },
    // Tibetan OK, W10 did not have swastika, installed Google Noto
    { "Ugar", QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Угаритский", u8"≈1400 до н.э.",
        u8"угаритский, хурритский <i>(только в районе Угарита)</i>",
        u8"Жившие в Передней Азии (современная Сирия) семиты не хвастались экономической, культурной или военной мощью, "
                    "зато были на пересечении торговых путей. "
                "Так что они переняли принцип [[ps:Xsux|клинописи]] под свои нужды. "
                "Получился алфавит из 30 согласных, писавшийся слева направо; откуда он взялся и что было до него{{-}}неизвестно. "
                "Названия многих букв{{-}}реконструкции и достоверно неизвестны. "
                "Единственный знак препинания{{-}}разделитель слов."
            "<p>Сохранилось много литературы и хозяйственных документов, дававших представление о разных сторонах угаритской жизни. "
                "Есть подозрение, что алфавит авторский, поскольку экономный{{-}}чем чаще буква, тем меньше в ней клиньев."
            "<p>Считается, что потомков у угаритского не было, хотя порядок букв близок к [[ps:Phnx|финикийскому]], "
                    "и существуют два необычных варианта: сокращённый алфавит из 21 буквы с записью справа налево, "
                    "и табличка с [[ps:Sarb|южноаравийским]] порядкой букв."
            "<p>Город Угарит брошен около 1200{{bc}} с катастрофой бронзового века{{-}}общим упадком спорной природы, "
                    "царившим в древнем мире того времени.",
                EcFont::HISTORIC },
    // Vai OK, have good M$ font Ebrima
    { "Vaii", QFontDatabase::Any,
        EcScriptType::SYLLABLE, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::AFRICA,
        u8"Ваи", u8"≈1832",
        u8"ваи ''(Либерия)''",
        u8"Письмо появилось около 1832. Разработал его Момолу Дувалу Букеле, полагают, что под влиянием "
                "[[ps:Cher|письма чероки]] (вместе с неграми в Либерию эмигрировали и индейцы, "
                "один стал даже вождём ваи)."
            "<p>Одна из причин выживания ваи{{-}}удачные знаки, применявшиеся и до этого в негритянской культуре. "
                "Другая{{-}}он использовался для торговли с голландцами и португальцами. "
                "В 1962 Университет Либерии добавил знаки для недостающих слогов.",
                EcFont::VAI },
    // Warang Citi OK, W10 none → installed Google Noto
    { "Wara", QFontDatabase::Any,
        EcScriptType::ARGUABLE, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Варанг-кшити", u8"≈1950",
        u8"хо ''(Восточная Индия)''",
        u8"Письменность придумал с нуля лидер коммуны Лако Бодра, заявив, что она пришла ему в божественном откровении от некоего "
                    "Дхавана Тури из XIII{{_}}века. "
                "Алфавит начинается с символа «ом»{{-}}первого звука создания Вселенной. "
                "Хо{{-}}бесписьменный язык (≈1{{_}}млн), около {{%|2}} пишет варанг-кшити и столько же [[ps:Orya|орией]]."
            "<p>Тип спорный: у каждой буквы есть гласная по умолчанию «а» (реже «о» или «э»), "
                    "как в [[pt:brahmic|брахмийских абугидах]], но есть и присущие алфавиту черты: "
                    "два регистра, все гласные буквы полные, не огласовки. "
                "Из‑за этого возможны разночтения: непонятно, немеет ли гласная по умолчанию "
                    "(в конечной позиции немеет всегда). "
                "Прочитайте самоназвание {{sm|𑢹𑣗𑣁𑣜𑣊 𑣏𑣂𑣕𑣂}}, и заодно поймёте, где ставить ударение. "
                "Буквы читаются по ''последнему'' звуку названия, кроме {{sm|𑢹}}=х и {{sm|𑣗}}=б, этот диграф читается «в».",
                EcFont::WARANG_CITI },
    // Old Persian OK. Small font, let’s install!!
    { "Xpeo", QFontDatabase::Any,
        EcScriptType::ALPHASYLLABLE, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Дневнеперсидская клинопись", u8"≈520 до н.э.",
        u8"древнеперсидский",
        u8"Из [[ps:Xsux|шумеро-аккадской клинописи]] взят только принцип письма, символы почти полностью другие. "
                "Наиболее распространённая версия{{-}}письменность создана искусственно для фиксации достижений царей Ахемендиской империи. "
                "Все известные надписи относятся к двум векам 520–330{{bc}}"
            "<p>К 1800 расшифровали слово «царь». "
                "В 1802 Георг Гротефенд прочитал имена царей{{-}}он не имел билингвы, только знал их греческое прочтение вроде "
                        "«Дарий» или «Ксеркс»{{-}}в оригинале «Дараявауш» и «Хашаярша». "
                "В 1823 Франсуа Шампольон, найдя билингву [[ps:Egyp|египетский]]-клинопись, подтвердил, что Гротефенд прав. "
                "В 1836 Эжéн Бюрнýф, догадавшись, что одна из надписей{{-}}список сатрапий Дария, расшифровал почти весь алфавит. "
                "Расшифровка персидской клинописи стала ключом к шумерской.",
                EcFont::OLD_PERSIAN },
    /// @todo [semi-tofu, P1] Sumerian cuneiform shows badly, Noto = M$ (but Noto is larger!)
    { "Xsux", QFontDatabase::Any,
        EcScriptType::SYLLABOHIEROGLYPH, EcLangLife::HISTORICAL, EcWritingDir::LTR_CUNEIFORM, EcContinent::ASIA,
        u8"Клинопись"sv, u8"≈3200 до н.э."sv,
        u8"шумерский, аккадский, хеттский, эламский, хурритский, урартский, древнеперсидский"sv,
        u8"Изначально (≈3500{{bc}}) на глине писали хозяйственные документы (10 коров), но к 3200 появились иероглифы, "
                    "а к 3000{{bc}} иероглифы стали использовать как слоги. "
                "Параллельно менялся и стиль: символы повернулись на 90° против часовой стрелки, "
                    "стали выдавливаться палочкой в виде призмы{{-}}отсюда элементы письма, острые клинышки."
            "<p>В 1 тысячелетии{{bc}} клинопись стала вытесняться более прогрессивным письмом. "
                "Так, персидская клинопись (1{{_}}тыс.{{bc}}) была уже алфавитно-слоговой. "
                "Последний известный памятник{{-}}75 уже нашей эры."
            "<p>Многотысячелетняя клинопись хорошо сохранилась в пустынях Персии и Ирака, и в первой половине XIX{{_}}века "
                    "расшифровали [[ps:Xpeo|древнеперсидскую]] клинопись. "
                "По шумеро-аккадской клинописи основную работу сделали Эдуард Хинкс, Генри Роулинсон и Юлиус Опперт. "
                "К 1900 расшифровали все известные языки."
            "<p>Диапазон «Клинопись» примерно соответствует клинописи третьей династии Ура (≈2100{{bc}}), "
                    "а более поздние формы (старо- и новоассирийская) [[pt:unification|унифицируются]] как варианты. "
                "Диапазон «Раннединастическая клинопись» описывает более старые символы ≈2900.",
                EcFont::CUNEIFORM },
    // Yezidi OK, W10 none → installed Google Noto
    { "Yezi", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::SACRED, EcWritingDir::RTL, EcContinent::ASIA,
        u8"Езидская", u8"XIII–XVII век"sv,
        u8"курдские"sv,
        u8"Езиды{{-}}часть курдского народа, исповедуют религию езидизм, вышедшую из [[ps:Avst|зороастризма]]. "
                "Священные езидские книги, ''Книга откровения'' и ''Чёрная книга'', написаны особым письмом "
                    "(известны и более ранние версии на [[ps:Arab|арабице]]). "
                "Консенсуса насчёт датировки этих книг нет.",
                EcFont::YEZIDI },
    // Yi OK, have M$ font. Though quite thin, Noto is better, I managed to counteract by increasing font.
    { "Yiii", QFontDatabase::Any,
        EcScriptType::SYLLABLE, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::AFRICA,
        u8"И (носу)", u8"1974 (слоговая)",
        u8"и ''(Китай)''",
        u8"Первый известный экземпляр старого (иероглифического) письма '''и''' датируется ≈1500, но письмо наверняка намного старше. "
                "Предания говорят, что оно существовало ещё в династию Тан (618—907). Письмо состоит из 8000…10000 иероглифов."
            "<p>Современный слоговой '''и''' придуман правительством Китая в 1974. В языке 43 согласных, 8 гласных и 4 тона, "
                "но не все комбинации возможны, и письмом '''и''' можно записать 1165 слогов.",
                EcFont::YI },
    // Zanabazar square OK, installed Google Noto
    { "Zanb", QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::ASIA,
        u8"Монгольское горизонтально-квадратное (Дзанабадзара)", u8"до 1700",
        u8"монгольский, лхасский тибетский, санскрит",
        u8"Буддийский монах Дзанабадзар (1635–1723), языковед и художник, известен разработкой письма [[ps:Soyo|соёмбо]]. "
                "Менее известно, что он придумал монгольское горизонтально-квадратное письмо, также на основе [[ps:Tibt|тибетского]]{{-}}"
                    "неизвестно, раньше или позже соёмбо (основная гипотеза{{-}}раньше), зачем и насколько широко использовалось. "
                "Заново открыто в 1801, сохранилось лишь немного источников."
            "<p>Брахмийская абугида с гласной по умолчанию «а» и [[pt:virama|вирамой]]. "
                "Точнее, физических вирам целых две{{-}}обе встречаются в источниках и не{{_}}имеют управляющего значения, "
                    "одна для последней буквы в слове, другая для стыков согласных. "
                "Есть ещё два способа стыковать согласные{{-}}через третью виртуальную вираму "
                    "(буквы ставятся друг на друга, часто уменьшаясь по высоте), "
                    "и через уменьшенные знаки дополнительных согласных. "
                "Эти методы причудливо переплетаются: например, одна согласная с вирамой, и ещё три{{-}}друг на друге. "
                "Начальные гласные образуются прикреплением огласовок к {{sm|𑨀}}. "
                "Цифр нет ни{{_}}в{{_}}одном источнике. "
                "Вертикальная запись причудлива и за пределами Юникода: так, изначально находившийся над второй буквой знак "
                    "может оказаться над всей надписью.",
                EcFont::ZANABAZAR },
    { "Zinh", QFontDatabase::Any,
        EcScriptType::NONE, EcLangLife::NOMATTER, EcWritingDir::NOMATTER, EcContinent::NONE,
        u8"Разные", {}, {},
        u8"Комбинирующая метка используется в нескольких разных письменностях." },
};


static_assert(std::size(uc::scriptInfo) == static_cast<size_t>(uc::EcScript::NN));


const uc::Script* uc::findScript(std::string_view x) { return findInArray(x, scriptInfo); }


constinit const uc::NumType uc::numTypeInfo[static_cast<int>(uc::EcNumType::NN)] {
    { {} },
    { u8"Цифра" },
    { u8"Особая цифра" },
    { u8"Число" },
};


constinit const uc::BidiClass uc::bidiClassInfo[static_cast<int>(EcBidiClass::NN)] {
    { "DIR", EcBidiStrength::EXPLICIT,    u8"Тэг направления",
               u8"Тэг направления", u8"FSI, LRE и др.",
               u8"Форматирующие символы, корректирующие работу двунаправленного алгоритма Юникода."
                    "<p>Строго говоря, каждый из этих символов{{-}}это отдельный класс в двунаправленном алгоритме, "
                    "но если вы не программист, вам достаточно знать: это тэги направления.",
               false },
    { "AL",  EcBidiStrength::STRONG,      u8"Арабская буква",
               u8"Арабская буква", u8"AL",
               u8"Пишутся справа налево. Есть тонкие правила, связанные с европейскими цифрами.",
               true },
    { "AN",  EcBidiStrength::WEAK,        u8"Арабская цифра",
               u8"Арабская цифра", u8"AN",
               u8"Пишутся справа налево. Есть тонкие правила, связанные с уровнями вложенности.",
               true },

    { "B",   EcBidiStrength::NEUTRAL,     u8"Разделитель абзацев",
               u8"Разделитель абзацев", u8"B",
               u8"Каждый абзац обрабатывается двунаправленным алгоритмом по отдельности.",
               false },
    { "BN",  EcBidiStrength::NEUTRAL,     u8"Неграфический",
               u8"Неграфический", u8"BN",
               u8"Управляющие, соединители и тэги, не имеющие своего написания."
                    "<p>См. [[pt:graphic|графический символ]].",
               false },
    { "CS",  EcBidiStrength::WEAK,        u8"Цифровой разделитель",
               u8"Цифровой разделитель", u8"CS",
               u8"Оказавшись в арабском или европейском числе, эти символы становятся его частью.",
               true },

    { "EN",  EcBidiStrength::WEAK,        u8"Европейская цифра",
               u8"Европейская цифра", u8"EN",
               u8"Пишутся слева направо. Есть тонкие правила, связанные с арабским текстом.",
               true },
    { "ES",  EcBidiStrength::WEAK,        u8"Европейский ±",
               u8"Европейский ±", u8"ES",
               u8"Символы + и − внутри европейского числа становятся его частью. Символ ± к этому классу не относится.",
               true },
    { "ET",  EcBidiStrength::WEAK,        u8"Европейская единица измерения",
               u8"Европейская единица измерения", u8"ET",
               u8"Валютные и прочие символы, встречающиеся рядом с числами. Оказавшись рядом с европейским числом, они становятся частью числа.",
               true },

    { "L",   EcBidiStrength::STRONG,      u8"Слева направо", u8"→", u8"L",
               u8"Латиница, кириллица и прочие алфавиты с написанием слева направо. "
                    "Поскольку подавляющая доля Юникода{{-}}[[ps:Hani|китайские иероглифы]], это самый многочисленный класс.",
               true },
    { "NSM", EcBidiStrength::WEAK,        u8"Непротяжённая метка",
               u8"Непротяжённая метка", u8"NSM",
               u8"Непротяжённые и охватывающие комбинирующие метки. Направление написания{{-}}как у основного символа.",
               true },
    { "ON",  EcBidiStrength::NEUTRAL,     u8"Прочий нейтральный",
               u8"Прочий нейтральный", u8"ON\u00A0+\u00A0¬\u00A0Bidi_M",
               u8"Приспосабливается к направлению окружающего текста.",
               true },

    { "NM",  EcBidiStrength::NEUTRAL,     u8"Нейтральный отзеркаливающийся",
               u8"Отзеркаливающийся", u8"ON\u00A0+\u00A0Bidi_M",
               u8"Скобки и похожие символы при написании справа налево отзеркаливаются. По понятным причинам все такие символы "
                    "являются нейтральными: приспосабливаются к направлению окружающего текста.",
               true },
    { "R",   EcBidiStrength::STRONG,      u8"Справа налево", u8"←", u8"R",
               u8"Иврит, а также исторические письменности Ближнего Востока.",
               true },
    { "S",   EcBidiStrength::NEUTRAL,     u8"Разделитель участков",
               u8"Разделитель участков", u8"S",
               u8"Tab и некоторые другие символы. "
                        "Посреди текстового абзаца встречаются редко, но обычно каждый участок обрабатывается "
                        "двунаправленным алгоритмом по отдельности. "
                    "Направление табуляции внутри абзаца постоянное.",
               false },
    { "WS",  EcBidiStrength::NEUTRAL,     u8"Пробел", u8"Пробел", u8"WS",
               u8"Нейтральны. Есть тонкие правила, касающиеся форматирования пробелами."
                "<p>См. термин [[pt:space|пробел]].",
               true },
    //{ u8"Ошибка" },  check for equal number
};


constinit const uc::Block uc::blocks[] {
    // Basic Latin OK
    { 0x0000, 0x007F,
            "Basic Latin", u8"Латиница базовая", CT_ASCII, EcScript::Latn },
    // Latin-1 OK
    { 0x0080, 0x00FF,
            "Latin-1 Supplement", u8"Латиница-1", CT_LATIN1, EcScript::Latn },
    // Latin extended A OK
    { 0x0100, 0x017F,
            "Latin Extended-A", u8"Латиница расширенная A",
            u8"Содержит символы языков Центральной и Восточной Европы, Прибалтики, кельтских, а также саамского, мальтийского, "
                "турецкого, эсперанто и некоторых других." },
    // Latin extended B OK
    { 0x0180, 0x024F,
            "Latin Extended-B", u8"Латиница расширенная B",
            u8"Содержит символы словенского, хорватского, румынского, ливского, чжуанского, пиньиня (латинизации китайского), африканских, "
                    "индейских языков, а также старой (до 1930) латиницы языков бывшего СССР." },
    // IPA extensions OK
    { 0x0250, 0x02AF,
            "IPA Extensions", u8"Расширения МФА",
            u8"Международный фонетический алфавит на основе латинского используется языковедами, логопедами, певцами и актёрами, "
                    "чтобы передавать произношение языков. Для передачи дополнительных особенностей речи{{-}}шепелявости, скрипа зубов "
                    "и прочего{{-}}используются расширения МФА."
                "<p>На самом деле большая часть символов этого блока{{-}}не расширения, а обычный МФА: так, {{sm|ɦ}} "
                    "есть в русском «ага», украинском «гривня» и чешском «hrad»."
                "<p>Оперные певцы часто поют на чужом языке, и их способность читать знаки транскрипции часто используется составителями "
                    "звуковых словарей. В частности, знаменитый звук «Янни или Лорел» изначально начитал певец Джей Обри Джонс{{-}}"
                    "низкой мужской голос, пройдя через некачественный динамик и столь же некачественный микрофон, стал восприниматься "
                    "как «Янни»."
                "<p>А ещё символы МФА позволяют перевернуть текст шутки ради." },
    // Spacing letter modifiers OK
    { 0x02B0, 0x02FF,
            "Spacing Modifier Letters", u8"Протяжённые модификаторы букв",
            u8"Модификаторы букв используются в фонетике и языкознании, чтобы передать тоны, длину, мягкость и условные звуки: "
                    "так, английское ''car'' (легковой автомобиль) записывается как {{sm|[kɑːʳ]}},"
                    "и последние два символа{{-}}модификаторы, обозначающие длину и условное «r»."
                "<p>Также в этом блоке есть протяжённые копии диакритических меток.",
            EcScript::NONE, EcFont::NORMAL, Bfg::FORCE_FONT },
    /// @todo [semi-tofu, BMP] Diacritical marks work somehow, though circle from 6 circles is too rough
    { 0x0300, 0x036F,
            "Combining Diacritical Marks", u8"Диакритические метки",
            u8"Диакритические знаки{{-}}надстрочные, подстрочные или внутристрочные знаки, прикрепляемые к букве и изменяющие "
                    "или уточняющие её значение. Например, точки над Ё или ударéние."
                "<p>Комбинирующий символ, или метка{{-}}несамостоятельный символ в Юникоде, прикрепляемый к другому символу. "
                    "Комбинирующий символ всегда идёт после основного. Определения настолько близки, что очевидно: комбинирующие символы "
                    "придуманы в первую очередь для диакритических знаков."
                "<p>В большинстве ОС механизм прикрепления продуман плохо, и если дизайнер предусмотрел верное положение "
                    "метки, получается красиво (о́). А если нет{{-}}обычно плохо (8́)."
                "<p>Иногда диакритические знаки настолько срастаются с буквой, что получается, по сути, другая буква. "
                    "Таковы русские Ё и Й. Считается, что все такие «другие буквы» крупных живых языков представлены в Юникоде "
                    "монолитными символами. Ударение другой буквой не{{_}}считается, к тому же сразу в нескольких шрифтах Windows "
                    "его неудачно нарисовали{{-}}потому нередко ударные русские буквы имитируют похожими монолитными латинскими." },
    // Greek and Coptic OK
    { 0x0370, 0x03FF,
            "Greek and Coptic", u8"Греческий и коптский",
            u8"[[ps:Grek|Греческий]]{{-}}первый настоящий алфавит, предок всех европейских алфавитов."
                "<p>[[ps:Copt|Коптский]]{{-}}язык египетских христиан. Его алфавит основан на греческом." },
    // Cyrillic OK
    { 0x0400, 0x04FF,
            "Cyrillic", u8"Кириллица", {},
            EcScript::Cyrl, EcFont::NORMAL, Bfg::UNGLITCH_MARKS },
    // Cyrillic supplement OK
    { 0x0500, 0x052F,
            "Cyrillic Supplement", u8"Кириллица дополнительная",
            u8"Буквы нескольких мелких кириллических языков: абзахского, курдского ''(как нацменьшинства СССР)'', коми, "
                    "мордовского, алеутского, азербайджанского ''(ныне на [[ps:Latn|латинице]])'' "
                    "и чувашского в алфавите Яковлева ''(использовался 1873–1938)''."
                "<p>Содержит смешную букву {{sm|Ԥ}}. Её «изобрёл» писатель Леонид Каганов{{-}}как оказалось, "
                    "не{{_}}первым: ещё за три года до его заметки (2008) майки с этой буквой продавались в России. "
                    "И независимо от них в абхазском вместо крюка {{sm|Ҧ}} (читается «пх») стали использовать лапку, "
                    "что и дало букве {{sm|Ԥ}} место в Юникоде.",
            EcScript::Cyrl },
    // Armenian OK
    { 0x0530, 0x058F,
            "Armenian", u8"Армянский", {}, EcScript::Armn },
    // Hebrew OK, managed to draw yod triangle
    { 0x0590, 0x05FF,
            "Hebrew", u8"Иврит", {}, EcScript::Hebr },
    // Arabic OK
    { 0x0600, 0x06FF,
            "Arabic", u8"Арабский", {}, EcScript::Arab },
    /// @todo [font] Which font to select and what to do with Syriac Malayalam?
    { 0x0700, 0x074F,
            "Syriac", u8"Сирийский", {}, EcScript::Syrc },
    // Arabic supplement OK
    { 0x0750, 0x077F,
            "Arabic Supplement", u8"Арабский дополнительный",
            u8"Буквы для языков Африки, Пакистана и раннего персидского.",
            EcScript::Arab },
    // Thaana OK
    { 0x0780, 0x07BF,
            "Thaana", u8"Тана", {}, EcScript::Thaa },
    // N’ko OK
    { 0x07C0, 0x07FF,
            "NKo", u8"Нко", {}, EcScript::Nkoo },
    // Samaritan OK
    { 0x0800, 0x083F,
            "Samaritan", u8"Самаритянский", {}, EcScript::Samr },
    // Mandaic OK
    { 0x0840, 0x085F,
            "Mandaic", u8"Мандейский", {}, EcScript::Mand },
    /// @todo [font] Which font to select and what to do with Syriac Malayalam?
    { 0x0860, 0x086F,
            "Syriac Supplement", u8"Сирийский дополнительный",
            u8"Необычная запись языка [[ps:Mlym|малая́лам]] сирийскими буквами, именуемая '''суриани''' или '''каршони'''. "
                    "Использовалась индийскими христианами до XIX{{_}}века.",
            EcScript::Syrc },
    // Arabic ex A OK
    { 0x08A0, 0x08FF,
            "Arabic Extended-A", u8"Арабский расширенный A",
            u8"Дополнительные арабские буквы для рохинджа ''(Мьянма)'', белорусского, татарского, башкирского, "
                    "арви ''(Индия, Шри-Ланка''), шахмукхи ''([[ps:Guru|пенджабского]])'', хиндко ''(Пакистан)'' "
                    "и африканских языков (в частности, берберского). "
                    "Также знаки комментариев к Корану.",
            EcScript::Arab },
    // Devanagari OK
    { 0x0900, 0x097F,
            "Devanagari", u8"Деванагари", {}, EcScript::Deva },
    // Bengali OK
    { 0x0980, 0x09FF,
            "Bengali", u8"Бенгальский", {}, EcScript::Beng },
    // Gurmukhi OK
    { 0x0A00, 0x0A7F,
            "Gurmukhi", u8"Гурмукхи", {}, EcScript::Guru },
    // Gujarati OK, Google Noto + FORCE_TOFU show nothing
    { 0x0A80, 0x0AFF,
            "Gujarati", u8"Гуджарати", {}, EcScript::Gujr },
    // Oriya OK, Noto + everything bad manually fixed
    { 0x0B00, 0x0B7F,
            "Oriya", u8"Ория", {}, EcScript::Orya },
    // Tamil OK, font pair is good
    { 0x0B80, 0x0BFF,
            "Tamil", u8"Тамильский", {}, EcScript::Taml },
    // Telugu OK
    { 0x0C00, 0x0C7F,
            "Telugu", u8"Телугу", {}, EcScript::Telu },
    // Kannada OK
    { 0x0C80, 0x0CFF,
            "Kannada", u8"Каннада", {}, EcScript::Knda },
    // Malayalam OK
    { 0x0D00, 0x0D7F,
            "Malayalam", u8"Малаялам", {}, EcScript::Mlym },
    // Sinhala OK
    { 0x0D80, 0x0DFF,
            "Sinhala", u8"Сингальский", {}, EcScript::Sinh },
    // Thai OK
    { 0x0E00, 0x0E7F,
            "Thai", u8"Тайский", {}, EcScript::Thai },
    // Lao OK
    { 0x0E80, 0x0EFF,
            "Lao", u8"Лаосский", {}, EcScript::Laoo },
    // Tibetan OK
    { 0x0F00, 0x0FFF,
            "Tibetan", u8"Тибетский", {}, EcScript::Tibt },
    // Myanmar OK
    { 0x1000, 0x109F,
            "Myanmar", u8"Бирманский", {}, EcScript::Mymr },
    // Georgian OK
    { 0x10A0, 0x10FF,
            "Georgian", u8"Грузинский", {}, EcScript::Geor },
    { 0x1100, 0x11FF,
            "Hangul Jamo", u8"Хангыль\u00A0— чамо",
            u8"В хангыле (корейском алфавите) всего 51 буква ''(чамо)''. Блок намного больше: одна и та же буква "
                    "в начале и конце слога кодируется разными символами, к тому же в блоке много устаревших чамо."
                "<p>Чхосон{{-}}начальные буквы, чунсон{{-}}средние, чонсон{{-}}конечные.",
            EcScript::Hang, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // Ethiopic OK
    { 0x1200, 0x137F,
            "Ethiopic", u8"Эфиопский", {}, EcScript::Ethi },
    // Ethiopic supplement OK
    { 0x1380, 0x139F,
            "Ethiopic Supplement", u8"Эфиопский дополнительный",
            u8"Буквы языка себат-бет (Эфиопия, 1,5{{_}}млн, ''Ethnologue'' статус 5: развивающийся), "
                    "а также метки тонов.",
            EcScript::Ethi },
    // Cherokee OK
    { 0x13A0, 0x13FF,
            "Cherokee", u8"Чероки", {}, EcScript::Cher },
    // Canadian aboriginal OK
    { 0x1400, 0x167F,
            "Unified Canadian Aboriginal Syllabics", u8"Канадская слоговая",
            {}, EcScript::Cans },
    // Ogham OK
    { 0x1680, 0x169F,
            "Ogham", u8"Огамическая", {}, EcScript::Ogam },
    // Runic OK
    { 0x16A0, 0x16FF,
            "Runic", u8"Руны", {}, EcScript::Runr },
    // Tagalog OK
    { 0x1700, 0x171F,
            "Tagalog", u8"Тагальский (байбайин)", {}, EcScript::Tglg },
    // Hanunoo OK
    { 0x1720, 0x173F,
            "Hanunoo", u8"Хануноо", {}, EcScript::Hano },
    // Buhid OK
    { 0x1740, 0x175F,
            "Buhid", u8"Бухид", {}, EcScript::Buhd },
    // Tagbanwa OK
    { 0x1760, 0x177F,
            "Tagbanwa", u8"Тагбанва", {}, EcScript::Tang },
    // Khmer OK
    { 0x1780, 0x17FF,
            "Khmer", u8"Кхмерский", {}, EcScript::Khmr },
    // Mongol OK, implemented ligature
    { 0x1800, 0x18AF,
            "Mongolian", u8"Монгольский", {}, EcScript::Mong },
    // Canadian extended OK
    { 0x18B0, 0x18FF,
            "Unified Canadian Aboriginal Syllabics Extended",
            u8"Канадская слоговая расширенная",
            u8"Буквы для кри, оджибве ''(алгонкинского)'', кэрриер, дене ''(атабаскских)''.",
            EcScript::Cans },
    // Limbu OK
    { 0x1900, 0x194F,
            "Limbu", u8"Лимбу", {}, EcScript::Limb },
    // Tai Le OK
    { 0x1950, 0x197F,
            "Tai Le", u8"Лы (тайлэ)", {}, EcScript::Tale },
    // New Tai Lue OK
    { 0x1980, 0x19DF,
            "New Tai Lue", u8"Новое письмо лы", {}, EcScript::Talu },
    // Khmer syms OK
    { 0x19E0, 0x19FF,
            "Khmer Symbols", u8"Кхмерские символы",
            u8"Первый символ блока {{sm|᧠}}{{-}}название восьмого месяца в буддийском солнечно-лунном календаре "
                        "(по-кхмерски ''asat'' или ''băthámsath'', санскрит ''ашадха'', приходится на июнь-июль). "
                    "Чтобы не{{_}}отставать от смены времён года, этот месяц повторяется дважды каждые два-три года. "
                    "Умляут 17D3 {{sm|◌៓}} некоторое время был [[pt:deprecated|запрещён]] в пользу данного символа, "
                        "да и в Юникоде{{_}}13.0 всё еще не{{_}}рекомендуется."
                "<p>Остальные{{-}}названия лунных дней от новолуния до полнолуния и обратно.",
            EcScript::Khmr },
    // Buginese OK
    { 0x1A00, 0x1A1F,
            "Buginese", u8"Лонтара (бугийский)", {}, EcScript::Bugi },
    // Lanna OK
    { 0x1A20, 0x1AAF,
            "Tai Tham", u8"Ланна (тай-тхам)", {}, EcScript::Lana },
    // Diacritical ex OK
    { 0x1AB0, 0x1AFF,
            "Combining Diacritical Marks Extended",
            u8"Диакритические метки расширенные",
            u8"Используются в немецкой и шотландской диалектологии.",
            EcScript::NONE, EcFont::NOTO },
    // Balinese OK
    { 0x1B00, 0x1B7F,
            "Balinese", u8"Балийский", {}, EcScript::Bali },
    // Sundanese OK
    { 0x1B80, 0x1BBF,
            "Sundanese", u8"Сунданский", {}, EcScript::Sund },
    // Batak OK
    { 0x1BC0, 0x1BFF,
            "Batak", u8"Батакская", {}, EcScript::Batk },
    // Lepcha OK
    { 0x1C00, 0x1C4F,
            "Lepcha", u8"Лепча", {}, EcScript::Lepc },
    // Ol Chiki OK
    { 0x1C50, 0x1C7F,
            "Ol Chiki", u8"Ол-чики", {}, EcScript::Olck },
    // Cyr C OK
    { 0x1C80, 0x1C8F,
            "Cyrillic Extended-C", u8"Кириллица расширенная C",
            u8"Варианты кириллических букв из печатных церковных книг: "
                        "от Библии Франциска Скорины (Прага, 1519, ещё до Фёдорова) до Киево-Печерского Типикона (1893).",
            EcScript::Cyrl },
    // Geor ex OK
    { 0x1C90, 0x1CBF,
            "Georgian Extended", u8"Грузинский расширенный",
            u8"''Мтаврули''{{-}}заглавная форма стандартного грузинского письма ''мхедрули''. "
                    "Заглавные буквы из основного блока называются ''асомтаврули'' и считаются устаревшими."
                "<p>Мтаврули используется в выделении и заголовках, и долгое время считался вариантом шрифта. "
                    "Однако исследователи нашли документы XIX и начала XX{{_}}века, где мхедрули и мтаврули смешивались. "
                    "К тому же грузинские шрифты ходят парами{{-}}мхедрули и мтаврули, что усложняет повседневную работу "
                        "и снижает выразительность грузинского текста, если смена шрифтов недоступна. "
                    "Майклу Эверсону, типографу из Консорциума, прислали перевод «Алисы в стране чудес» "
                        "в 8-битной кодировке, где «ВЫПЕЙ МЕНЯ» было набрано мтаврули."
                "<p>Теперь Юникод смотрит на мхедрули и мтаврули как на строчные и заглавные, "
                        "только с другими правилами употребления{{-}}выделяется всё слово. "
                    "Двойная черта снизу, едва заметно выведенная на иконке{{,-}}редакторская помета, означающая заглавную букву.",
            EcScript::Geor },
    // Sundanese suplement OK
    { 0x1CC0, 0x1CCF,
            "Sundanese Supplement", u8"Сунданский дополнительный",
            u8"Исторические знаки препинания для сунданского.",
            EcScript::Sund },
    // Vedic ex OK
    { 0x1CD0, 0x1CFF,
            "Vedic Extensions", u8"Ведические символы",
            u8"Символы из Вед, применяемые в [[ps:Deva|деванагари]] и других индийских письменностях. "
                    "Произношение тех времён сильно отличалось от современного, и в основном это фонетические знаки."
                "<p>Часть символов отличаются по форме от письменности к письменности. "
                    "1CF5 в деванагари похож на песочные часы, в бенгальском на крест. "
                    "1CF6 в деванагари похож на кратку от Й, в бенгальском{{-}}на прямую или перевёрнутую малую омегу ω. "
                    "Символ в виде прямой и перевёрнутой омеги, некоторое время существовавший в Юникоде, "
                            "чаще встречается у исследователей, чем в первичных источниках.",
            EcScript::NONE, EcFont::VEDIC },
    // Phonetic ext OK
    { 0x1D00, 0x1D7F,
            "Phonetic Extensions", u8"Фонетические расширения",
            u8"Символы Уральского фонетического алфавита, фонетики древнеирландских языков, Оксфордского словаря английского языка, "
                        "фонетике индейских и русского." },
    // Phonetic ext supp OK
    { 0x1D80, 0x1DBF,
            "Phonetic Extensions Supplement", u8"Фонетические расширения дополнительные",
            u8"Специализированные и устаревшие формы Международного фонетического алфавита." },
    // Combining supp OK, fixed manually in Noto
    { 0x1DC0, 0x1DFF,
            "Combining Diacritical Marks Supplement",
            u8"Диакритические метки дополнительные",
            u8"Символы Уральского фонетического алфавита, средневековых сокращений "
                    "и Teuthonista (системы записи немецких диалектов).",
        EcScript::NONE, EcFont::NOTO },
    // Lat ex OK
    { 0x1E00, 0x1EFF,
            "Latin Extended Additional", u8"Латиница расширенная дополнительная",
            u8"[[pt:precomposed|Монолитные]] символы с различными метками. "
                    "Девяносто из этих символов вьетнамские, около десятка{{-}}средневековые, "
                    "в том числе заглавный эсцет '''ẞ'''." },
    // Greek ex OK, though DejaVu is a big gun
    { 0x1F00, 0x1FFF,
            "Greek Extended", u8"Греческий расширенный",
            u8"Политоническая фонетика, существовавшая в некоторых диалектах древнегреческого:<br>"
                    "• даси́я{{-}}густое придыхание (добавление ''h'');<br>"
                    "• псили{{-}}тонкое придыхание (отсутствие этого ''h'');<br>"
                    "• подстрочная йота{{-}}дифтонг с i;<br>"
                    "• окси́я{{-}}высокий/восходящий тон;<br>"
                    "• вари́я{{-}}низкий/нисходящий тон;<br>"
                    "• тильда{{-}}смена тона в одном слоге;<br>"
                    "• кратка{{-}}сокращённый звук, если таковой не передаётся алфавитом, в отличие от ε/η и ο/ω);<br>"
                    "• черта{{-}}длинный звук;<br>"
                    "• диерезис{{-}}раздельное прочтение звуков."
                "<p>Добавлены в начале нашей эры, убраны только в 1982, когда столь сложной фонетики много веков не было.",
            EcScript::Grek, EcFont::DEJAVU },
    // General punct OK, changed 2-dot leader to low one.
    { 0x2000, 0x206F,
            "General Punctuation", u8"Знаки препинания",
            u8"Пунктуация (лат. ''punctum'' «точка»){{-}}система знаков, подчёркивающих синтаксис и интонацию речи, "
                    "а также правила постановки их в тексте."
                "<p>Очевидно, основные знаки препинания (точка, запятая, двоеточие…) закодированы в [[pt:ascii|ASCII]] и латинице-1. "
                    "А в блоке знаков препинания находятся так называемые ''типографские знаки'', "
                    "чтобы красиво оформить текст.",
            EcScript::NONE, EcFont::PUNCTUATION },
    // Sup/sub OK
    /// @todo [link,block] Links to other blocks when they are ready
    { 0x2070, 0x209F,
            "Superscripts and Subscripts",
            u8"Верхние и нижние индексы",
            u8"Блок содержит надстрочные и подстрочные буквы, знаки и цифры, применяемые в математике и фонетике. "
                "<p>Ещё до распространения интернета выяснилось: бездумное копирование текста может привести "
                        "к тому, что число Авогадро «равняется» 6,632·1023. "
                    "Система (текстовый файл, интернет-форум) может вообще не поддерживать индексов, "
                        "и не стоит принуждать людей переходить на более сложные системы ради такой мелочи, "
                        "или имитировать вроде «6,022e23» или «6,022·10^23». "
                "<p>К тому же нарисованные типографом индексы обычно качественнее, чем синтезированные редактором документов."
                "<p>Многие из важных индексов есть в других местах: например, транспонирование матрицы Aᵀ{{-}}"
                        "в блоке фонетических расширений." },
    // Curr symbols OK
    { 0x20A0, 0x20CF,
            "Currency Symbols", u8"Символы валют",
            u8"Символы валют делятся на две чёткие категории. Одна группа возникла в XVII–XIX{{_}}веке с развитием "
                    "делового языка: доллар {{sm|$}}, фунт {{sm|£}}, [[ps:Taml|тамильский]] знак рупии {{sm|௹}}. "
                    "Другая{{-}}принята государством с конца XX{{_}}века: евро {{sm|€}}, гривна {{sm|₴}}, "
                    "рубль {{sm|₽}}, рупия {{sm|₹}}." },
    // Marks for symbols OK, made optimized versions in Noto
    { 0x20D0, 0x20FF,
            "Combining Diacritical Marks for Symbols",
            u8"Диакритические метки для символов",
            u8"Практически все из этих символов нормально работают, если в шрифте есть подходящая [[pt:ligature|лигатура]]{{-}}"
                        "например, {{sm| 1⃣}}. "
                    "Образцы всех символов, кроме трёх, пришлось собирать в редакторе шрифтов.",
            EcScript::NONE, EcFont::UMLAUT_SYMBOL },
    // Letterlike OK
    { 0x2100, 0x214F,
            "Letterlike Symbols", u8"Буквоподобные символы",
            u8"Математические, технические и деловые символы буквенного происхождения."
                "<p>Ажурный шрифт {{sm|ℕℤℚℝℂ}} происходит из жирного, записанного мелом на доске. "
                    "Распространился он со знаменитым учебником по комплексному анализу 1965{{_}}года Ганнинга и Росси."
                "<p>Символ {{sm|ℵ}} для мощности множеств предложен Г.{{_}}Кантором." },
    // Number forms OK
    /// @todo [link,block] Latin-1
    { 0x2150, 0x218F,
            "Number Forms", u8"Числовые формы",
            u8"Монолитные обыкновенные дроби и римские цифры. "
                    "Дробь {{sm|↉}} используется в бейсболе, "
                        "{{sm|↊}} и {{sm|↋}}{{-}}в двенадцатеричной системе счисления. "
                "<p>Ещё три дроби закодированы в латинице-1." },
    // Arrows OK
    { 0x2190, 0x21FF,
            "Arrows", u8"Стрелки",
            u8"Стрелки{{-}}символы, напоминающие стрелу и указывающие направление. "
                    "Вот немногие примеры использования стрелок:<br>"
                    "• для маркировки клавиш;<br>"
                    "• на дорожных знаках, транспортных схемах;<br>"
                    "• в математике{{-}}отображение, следствие, предел, вектор, монотонность;<br>"
                    "• в химии{{-}}реакция, идущая в одну или обе стороны, выделяется газ, выпадает осадок." },
    // Math op OK
    { 0x2200, 0x22FF,
            "Mathematical Operators", u8"Математические знаки",
            u8"Простейшие математические знаки: математические операции, сравнение, интегралы, теория множеств. "
                    "Система математических обозначений складывалась веками и отличается для разных стран, "
                        "областей математики и даже математических школ. "
                    "Например, плюс в круге {{sm|⊕}} может означать исключающее ИЛИ в формальной логике "
                        "и машинное сложение в численных методах. "
                    "Для равенства по определению используют {{sm|≔}}, {{sm|≝}}, {{sm|≡}}." },
    // Misc tech OK
    { 0x2300, 0x23FF,
            "Miscellaneous Technical", u8"Разные технические",
            u8"Символы, используемые в черчении, электронике, программировании, стоматологии, поэзии, химии, оптике, "
                        "пользовательских интерфейсах."
                "<p>Также символы, используемые в системах компьютерной математики вроде MathType (он же ''Microsoft Equation'') "
                        "и TᴇX (читается «тех») для создания многоэтажных скобок.",
            EcScript::NONE, EcFont::TECHNICAL },
    // Control pictures OK
    { 0x2400, 0x243F,
            "Control Pictures", u8"Изображения управляющих",
            u8"Сами по себе управляющие символы не имеют никакого графического представления. "
                    "В этом блоке собраны условные знаки, которыми изображают управляющие символы. "
                    "В первую очередь нам важен символ {{sm|␣}}, изображающий [[pt:space|пробел]]."
                "<p>Несколько изображений управляющих символов есть в и других блоках: {{sm|↵←⌫}}.",
            EcScript::NONE, EcFont::NOTO_SYMBOL2_BIGGER },
    // OCR/MICR OK
    { 0x2440, 0x245F,
            "Optical Character Recognition", u8"Оптическое распознавание символов",
            u8"Оптическое и магнитное распознавание символов служит для автоматического ввода документов, "
                    "в первую очередь паспортов, банковских чеков, транспортных билетов. "
                    "В отличие от штрих-кодов, одни и те же символы распознаются людьми и машинами, что исключает подлог. "
                    "Обе технологии используют специальные моноширинные шрифты. "
                    "Эти шрифты (иногда в несколько искажённом виде) широко встречаются в фильмах и играх жанра «киберпанк»."
                "<p>В 1956 США продемонстрировали распознавание символов на магнитных чернилах (MICR). "
                    "Система понимает десять цифр и четыре специальных символа, задающих смысл поля. "
                    "Магнитное распознавание работает значительно надёжнее оптического. "
                    "Если видите квадратный шрифт, в котором часть вертикальных штрихов толстая "
                        "(например, на коробке приставки ''Magnavox Odyssey''){{-}}это стилизация MICR."
                "<p>В 1968 в США сделали первую действующую технологию оптического распознавания{{-}}OCR-A. "
                    "Понимает весь [[ps:Latn|латинский]] алфавит с малыми буквами. "
                    "Для разделения полей используются специальные символы: крючок, стул, вилка, пряжка и бабочка. "
                    "Шрифт встречается в фильмах «13 часов: Тайные солдаты Бенгази», «Чёрный список», «Притворщик», "
                    "«Электронные жучки» и других."
                "<p>Шрифтонезависимое оптическое распознавание появилось лишь в 1990‑е. "
                    "Оно используется для оцифровки книг, обработки анкет и значительно менее надёжное. "
                    "Так что загранпаспорт с машиночитаемой страницей, написанной шрифтом OCR-B{{,-}}не{{_}}анахронизм.",
            // Fonts coincide
            EcScript::NONE, EcFont::PHAISTOS_DISC },
    // Enclosed alnum OK, need ordinary Cambria
    { 0x2460, 0x24FF,
            "Enclosed Alphanumerics", u8"Обрамлённые буквы и цифры",
            u8"По данным английской Википедии, эти символы в основном предназначены для списков{{-}}"
                        "«наполовину маркированных, наполовину нумерованных». "
                    "{{em|Ⓜ}} используется как [[pt:emoji|эмодзи]] для метро." },
    // Box drawing OK
    { 0x2500, 0x257F,
            "Box Drawing", u8"Рисование рамок",
            u8"Элементы [[pt:pseudographics|псевдографики]], используемые для рисования рамок. "
                    "Как и любая псевдографика, чаще всего используются в моноширинных шрифтах."
                "<p>В IBM-совместимых ПК были две разновидности линий{{-}}одиночная и двойная. "
                    "Юникод добавляет к ним тонкую, пунктирную и тонкую пунктирную."
                "<p>В компьютере BBC{{_}}Master есть закруглённые уголки.",
            EcScript::NONE, EcFont::DEJAVU },
    // Block elements OK
    { 0x2580, 0x259F,
            "Block Elements", u8"Блочные элементы",
            u8"Элементы [[pt:pseudographics|псевдографики]], используемые для сплошной заливки. "
                    "Как и любая псевдографика, чаще всего используются в моноширинных шрифтах. "
                    "В пропорциональных шрифтах половинная заливка {{sm|▀}}, растровая заливка {{sm|░}} "
                        "и другие символы с IBM обычно вытянуты по вертикали, а четвертная заливка с "
                        "ZX{{_}}Spectrum {{sm|▚}}{{-}}квадратная. "
                    "В данном{{-}}это открытый шрифт DejaVu, брошенный в 2016{{-}}всё правильно."
                "<p>У псевдографики, как правило, отсутствует межсимвольный интервал."
                "<p>Часть псевдографических символов со старых компьютеров{{-}}например, 2×3 блока{{-}}"
                    "здесь отсутствуют и добавлены только в 2020.",
            EcScript::NONE, EcFont::DEJAVU },
    // Geometric OK
    { 0x25A0, 0x25FF,
            "Geometric Shapes", u8"Геометрические фигуры",
            u8"Геометрические фигуры используются для маркировки кнопок, грубого рисования пользовательских интерфейсов. "
                    "Вот, например, четыре кнопки ''Sony Playstation'' {{sm|🞩○△□}}{{-}}три из них взяты из этого блока. "
                    "Символ {{sm|◌}} используется в Юникоде для маркировки [[pt:combining|комбинирующих]] знаков." },
    // Misc sym OK
    { 0x2600, 0x26FF,
            "Miscellaneous Symbols", u8"Разные символы",
            u8"В этом разделе есть [[pt:emoji|эмодзи]], знаки из астрономии, астрологии, настольных игр, восточной эзотерики, "
                        "религии, политики и просто для украшения печатного текста. "
                    "Многие символы взяты из японской кодировки ARIB STD B24, используемой для передачи новостей, погоды, "
                        "информации о дорожных заторах через цифровое телевидение." },
    // Dingbats OK, need Cambria here!
    { 0x2700, 0x27BF,
            "Dingbats", u8"Украшения",
            u8"Символы, чаще всего используемые для украшения печатного текста. "
                    "Блок содержат несколько [[pt:emoji|эмодзи]], что вполне оправдано: в интернете это будут цветные картинки, "
                    "а печатник оформит публикацию чёрно-белым шрифтом." },
    // Misc math A OK
    { 0x27C0, 0x27EF,
            "Miscellaneous Mathematical Symbols-A", u8"Разные математические символы A",
            u8"Несколько символов, используемых в геометрии, формальной логике, верификации программ, теории баз данных "
                    "и других разделах математики. Также западный символ деления столбиком.",
            EcScript::NONE, EcFont::MATH },
    // Arrows A OK
    /// @todo [desc] Arrows A
    { 0x27F0, 0x27FF,
            "Supplemental Arrows-A", u8"Дополнительные стрелки A" },
    // Braille OK
    { 0x2800, 0x28FF,
            "Braille Patterns", u8"Шрифт Брайля", {}, EcScript::Brai },
    // Arrows B OK
    /// @todo [desc] Arrows B
    { 0x2900, 0x297F,
            "Supplemental Arrows-B", u8"Дополнительные стрелки B" },
    // Misc math B OK
    { 0x2980, 0x29FF,
            "Miscellaneous Mathematical Symbols-B",
            u8"Разные математические символы B",
            u8"Различные математические символы: скобки, углы, пустые множества, «ящики с усами»…" },
    // Supp math ops OK
    { 0x2A00, 0x2AFF,
            "Supplemental Mathematical Operators",
            u8"Дополнительные математические знаки",
            u8"Символы, используемые в математическом анализе, функциях комплексного переменного, Z-нотации, "
                    "теории баз данных, теории управления, формальной логике и других разделах математики.",
            EcScript::NONE, EcFont::MATH },
    // Misc syms & arrows OK
    /// @todo [desc] Misc syms & arrows
    { 0x2B00, 0x2BFF,
            "Miscellaneous Symbols and Arrows", u8"Разные символы и стрелки",
            {} },
    // Glagolitic OK
    { 0x2C00, 0x2C5F,
            "Glagolitic", u8"Глаголица", {}, EcScript::Glag },
    // Latin C OK
    /// @todo [desc] Latin C
    { 0x2C60, 0x2C7F,
            "Latin Extended-C", u8"Латиница расширенная C", {}, EcScript::Latn },
    // Coptic OK
    { 0x2C80, 0x2CFF,
            "Coptic", u8"Коптский", {}, EcScript::Copt },
    // Georgian supp OK
    { 0x2D00, 0x2D2F,
            "Georgian Supplement", u8"Грузинский дополнительный",
            u8"Устаревший [[pt:minuscule|минускульный]] грузинский шрифт ''нусхури''. "
                    "Буквы наклонены вправо и очень угловатые. "
                    "Буквицы при этом оформляли шрифтом ''асомтаврули'' из основного блока."
                "<p>Ещё в 2000 департамент информатизации Грузии и католикóс (глава грузинской церкви) "
                    "просили закодировать нусхури, и Консорциум откликнулся.",
            EcScript::Geor, EcFont::GEORGIAN_NUSKHURI },
    // Tifinagh OK
    { 0x2D30, 0x2D7F,
            "Tifinagh", u8"Тифинаг (берберский)", {}, EcScript::Tfng },
    // Ethiopic ex OK
    { 0x2D80, 0x2DDF,
            "Ethiopic Extended", u8"Эфиопский расширенный",
            u8"Эфиопское письмо для языков мекан (с 2009 перешёл на [[ps:Latn|латиницу]], "
                    "билин (с 1990‑х полностью на латинице), бенч (с 2008 на латинице), себат-бет.",
            EcScript::Ethi },
    // Cyr ex A OK, fixed those renderings in Noto
    { 0x2DE0, 0x2DFF,
            "Cyrillic Extended-A", u8"Кириллица расширенная A",
            u8"Комбинирующие кириллические буквы из церковнославянского.",
            EcScript::Cyrl, EcFont::NOTO },
    // Supp punct OK
    { 0x2E00, 0x2E7F,
            "Supplemental Punctuation", u8"Дополнительные знаки препинания",
            u8"Специализированные знаки препинания: текстология Нового Завета, символы с древнегреческих папирусов, "
                    "старинная пунктуация, немецкие словарные пометки, символы из Типикона (русского монашеского устава XV{{_}}века) "
                    "и многое другое." },
    // CJK radicals supp OK
    { 0x2E80, 0x2EFF,
            "CJK Radicals Supplement", u8"ККЯ ключи дополнительные",
            u8"Альтернативные формы ключей Канси, применяющиеся в заголовках словарей.",
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // Kangxi OK
    { 0x2F00, 0x2FDF,
            "Kangxi Radicals", u8"Ключи Канси",
            u8"Вы спросите: а как найти нужный иероглиф в словаре, если нет произношения? "
                    "Для этого находят в иероглифе ''ключ''{{-}}черту или их комбинацию."
                "<p>Словарь китайского языка Канси составлен 1710–1716 по приказу императора Канси и содержит 49030 позиций. "
                    "Считался стандартным вплоть до конца XIX{{_}}века. "
                    "В 1831 исправили 2500 ошибок, вызванных спешкой первоначальных составителей."
                "<p>Система «алфавитной» расстановки иероглифов из 214 ключей придумана ещё в 1615, но именно словарь Канси "
                        "сделал её стандартом.",
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // Ideographic desc OK, not collapsible: small block
    { 0x2FF0, 0x2FFF,
            "Ideographic Description Characters",
            u8"Символы структуры иероглифов",
            u8"В Юникоде есть механизм описания незакодированного [[ps:Hani|иероглифа ККЯ]]. "
                    "А для этого важно указать, на какие части он разбит и что в каждой. "
                    "Для первого и предназначен данный блок.",
            EcScript::NONE, EcFont::CJK_STRUCTURE },
    /// @todo [desc] CJK sym/punct
    // CJK sym/punct OK
    { 0x3000, 0x303F,
            "CJK Symbols and Punctuation",
            u8"ККЯ символы и знаки препинания", {},
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // Hiragana OK, installed small version of Noto CJK Korean
    { 0x3040, 0x309F,
            "Hiragana", u8"Хирагана", {}, EcScript::Hira },
    // Katakana OK, not collapsible: syllabary
    { 0x30A0, 0x30FF,
            "Katakana", u8"Катакана", {}, EcScript::Kana },
    // Bopomofo OK
    { 0x3100, 0x312F,
            "Bopomofo", u8"Бопомофо (чжуинь)", {},
            EcScript::Bopo, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // Hangul compat jamo OK
    /// @todo [desc] Hangul compat jamo
    { 0x3130, 0x318F,
            "Hangul Compatibility Jamo", u8"Хангыль\u00A0— совместимые чамо",
            {},
            EcScript::Hang, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // Kanbun OK, not collapsible: small block
    { 0x3190, 0x319F,
            "Kanbun", u8"Камбун (подсказки к древнеяпонским иероглифам)",
            u8"Словом «камбун» (''китайское письмо'') называли письменный язык средневековой Японии, основанный на [[ps:Hani|китайском]]. "
                    "Китайцы говорят «подлежащее-сказуемое-дополнение», а японцы{{-}}«подлежащее-дополнение-сказуемое», "
                        "так что текст читался не по порядку, и приходилось писать значки ''каэритэн''. "
                    "Эти значки обычно меньше иероглифов."
                "<p>Помимо каэритэна, в камбуне были и другие значки: склонение слов, знаки препинания, японское прочтение иероглифа…"
                "<p>В модернизацию Мэйдзи (конец XIX{{_}}века) камбун отменили. "
                    "Камбун учат в современных японских школах, но новых текстов на нём не пишут."
                "<p>В данном блоке закодирован один значок, использовавшийся для слитного прочтения и собственных имён (вертикальная черта) "
                        "и 15 значков порядка.",
            EcScript::NONE, EcFont::CJK_KANBUN },
    // Bopomofo OK, at least in W10
    { 0x31A0, 0x31BF,
            "Bopomofo Extended", u8"Бопомофо (чжуинь) расширенный",
            u8"Фонетические символы для языков: южноминьского (миньнань), хакка, хмонгских и кантонского.",
            EcScript::Bopo, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // CJK strokes OK
    { 0x31C0, 0x31EF,
            "CJK Strokes", u8"Черты ККЯ",
            u8"Иероглифы [[pt:cjk|ККЯ]] состоят из основных восьми элементов:"
                 "<p>• точки D;<br>"
                    "• крючка G;<br>"
                    "• горизонтальной H;<br>"
                    "• откидной черты вправо N;<br>"
                    "• откидной черты влево P;<br>"
                    "• вертикальной S;<br>"
                    "• восходящей T;<br>"
                    "• изогнутой откидной W."
                "<p>Из дополнительных элементов{{-}}прямой участок B, кружок Q, наклон X, излом Z. "
                    "Для каждой черты кодифицировано, в каком направлении её писать."
                "<p>Все названия китайские: например, B от ''biǎn{{-}}плоский''.",
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // Katakana phon OK, not collapsible: small block
    /// @todo [desc] Katakana phon extensions
    { 0x31F0, 0x31FF,
            "Katakana Phonetic Extensions", u8"Катакана\u00a0— фонетические расширения",
            {} },
    // Enclosed CJK letters and months OK
    /// @todo [desc] Enclosed CJK letters and months
    { 0x3200, 0x32FF,
            "Enclosed CJK Letters and Months", u8"Обрамлённые буквы и месяцы ККЯ",
            {},
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // CJK compatibility OK
    { 0x3300, 0x33FF,
            "CJK Compatibility", u8"ККЯ символы совместимости",
            u8"Символы для обеспечения совместимости с кодировками [[pt:cjk|ККЯ]]. "
                    "Различные японские сокращения (квартира, альфа, ампер, ар…), телеграфные символы, "
                        "эры правления японских императоров, вписывающиеся в квадраты ККЯ единицы измерения.",
            EcScript::Hani, EcFont::CJK_COMPAT, Bfg::COLLAPSIBLE },
    /// @todo [desc] CJK A
    { 0x3400, 0x4DBF,
            "CJK Unified Ideographs Extension A",
            u8"ККЯ иероглифы расширение A", {},
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // Yijing OK
    { 0x4DC0, 0x4DFF,
            "Yijing Hexagram Symbols", u8"Гексаграммы И цзин (Книги перемен)",
            u8"Книга перемен (И цзин){{-}}китайский эзотерический трактат, написанный разными поколениями людей. "
                    "Наиболее старая его часть написана около 700{{bc}} и описывает знаменитую методику "
                        "клеромантии (гадания на случайных числах). "
                    "Нужно разыграть гексаграмму и прочитать её значение."
                "<p>Известно, что для гадания применялся пучок из 50 палочек, традиционно стеблей тысячелистника, но как именно{{-}}неизвестно. "
                    "Методы с бросанием монет появились позднее, а современное гадание на палочках{{-}}реконструкция."
                "<p>В Китае также был философ И Цзин, никак с книгой не{{_}}связанный." },
    // CJK hieroglyphs OK
    { 0x4E00, 0x9FFF,
            "CJK Unified Ideographs", u8"ККЯ иероглифы",
            {},
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // Yi OK
    { 0xA000, 0xA48F,
            "Yi Syllables", u8"И\u00A0— слоги", {}, EcScript::Yiii },
    // Yi radicals OK
    { 0xA490, 0xA4CF,
            "Yi Radicals", u8"И\u00A0— ключи",
            u8"Ключи, используемые для «алфавитной сортировки» словарей языка '''[[ps:Yiii|и]]'''.",
            EcScript::Yiii },
    // Lisu OK
    { 0xA4D0, 0xA4FF,
            "Lisu", u8"Лису (алфавит Фрейзера)", {}, EcScript::Lisu },
    // Vai OK
    { 0xA500, 0xA63F,
            "Vai", u8"Ваи", {}, EcScript::Vaii },
    // Cyr ex B OK, modified font a bit
    { 0xA640, 0xA69F,
            "Cyrillic Extended-B", u8"Кириллица расширенная B",
            u8"Старо- и церковнославянский, дореволюционный абхазский (кириллица Услара, 1862), "
                        "знаки интонации для литовской диалектологии.",
            EcScript::Cyrl, EcFont::NORMAL, Bfg::UNGLITCH_MARKS },
    // Bamum OK
    { 0xA6A0, 0xA6FF,
            "Bamum", u8"Бамум", {}, EcScript::Bamu },
    // Modifier tone OK
    { 0xA700, 0xA71F,
            "Modifier Tone Letters", u8"Модифицирующие символы тона",
            u8"Символы Международного фонетического алфавита, используемые в китайском, чинантекском ''(индейцы Мексики)'' "
                        "и африканских языках для указания тона{{-}}изменения высоты звука в пределах одной фонемы."
                "<p>Символы {{sm|꜀꜁}} и прочие используются в этимологическом анализе китайских тонов."
                "<p>{{sm|꜈꜉}} и прочие{{-}}для прямого задания высоты звука и ''тонового сандхи''. "
                        "(''Сандхи''{{-}}санскритское понятие, означающее изменение звука в зависимости от соседних звуков. "
                        "Например, русское ''обой'''щик'''/пулемёт'''чик'''''{{-}}правда, это сандхи не тоновое, в русском тонов нет.)"
                "<p>{{sm|ꜗꜘꜙꜚ}} применяются в чинантекском."
                "<p>Стрелки используются для ''тоновых шагов'' (в следующем слоге тон выше или ниже, чем в предыдущем). "
                    "Восклицательные знаки{{-}}частый суррогат, использовавшийся до широкой поддержки МФА в шрифтах." },
    // Latin ex D OK, dereferenced bad chars of Google Noto
    /// @todo [desc] Latin ex D
    { 0xA720, 0xA7FF,
            "Latin Extended-D", u8"Латиница расширенная D", {}, EcScript::Latn },
    // Syloti OK
    { 0xA800, 0xA82F,
            "Syloti Nagri", u8"Силхети-нагари", {}, EcScript::Sylo },
    // Common Indic OK
    { 0xA830, 0xA83F,
            "Common Indic Number Forms", u8"Общеиндийские числовые символы",
            u8"Такие знаки присутствуют в источниках XVI{{_}}века, и применяются по сей день "
                "в Северной Индии, Пакистане и Непале для записи дробей: например, "
                "размера, веса или цены.",
            EcScript::Deva },
    // Phang-pa OK
    { 0xA840, 0xA87F,
            "Phags-pa", u8"Монгольское квадратное (Пагба-ламы)", {}, EcScript::Phag },
    // Saurashtra OK
    { 0xA880, 0xA8DF,
            "Saurashtra", u8"Саураштра", {}, EcScript::Saur },
    // Devanagari ex OK, drew 4 chars
    { 0xA8E0, 0xA8FF,
            "Devanagari Extended", u8"Деванагари расширенный",
            u8"Распевы из Самаведы (книги релизиозных песен), варианты знака чандрабинду (носовая гласная), "
                    "редакторские пометы, религиозные символы, "
                    "гласная «ай» для мелких неписьменных языков Индии, придуманная европейцами.",
            EcScript::Deva },
    // Kayah Li OK
    { 0xA900, 0xA92F,
            "Kayah Li", u8"Кая-ли", {}, EcScript::Kali },
    // Rejang OK
    { 0xA930, 0xA95F,
            "Rejang", u8"Реджанг", {}, EcScript::Rjng },
    // Hangul jamo A OK
    /// @todo [desc] Hangul jamo A
    { 0xA960, 0xA97F,
            "Hangul Jamo Extended-A", u8"Хангыль\u00A0— расширенные чамо А",
            {},
            EcScript::Hang, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // Javanese OK
    { 0xA980, 0xA9DF,
            "Javanese", u8"Яванский", {}, EcScript::Java },
    // Myanmar ex B OK
    { 0xA9E0, 0xA9FF,
            "Myanmar Extended-B", u8"Бирманский расширенный B", {}, EcScript::Mymr },
    // Cham OK
    { 0xAA00, 0xAA5F,
            "Cham", u8"Чамский", {}, EcScript::Cham },
    // Myanmar ex A OK
    { 0xAA60, 0xAA7F,
            "Myanmar Extended-A", u8"Бирманский расширенный А", {}, EcScript::Mymr },
    // Tai viet OK
    { 0xAA80, 0xAADF,
            "Tai Viet", u8"Тай-вьет", {}, EcScript::Tavt },
    // Meetei ex OK
    { 0xAAE0, 0xAAFF,
            "Meetei Mayek Extensions", u8"Манипури\u00A0— расширения",
            u8"Устаревшие символы из языка манипури, в том числе [[pt:virama|вирама]] для сборки старых [[pt:ligature|лигатур]]." },
    // Ethiopic ex A OK
    { 0xAB00, 0xAB2F,
            "Ethiopic Extended-A", u8"Эфиопский расширенный А",
            u8"Буквы для языков гамо-гофа-дауро (три народа, один язык), баскето и гумуз (с 2007 на [[ps:Latn|латинице]]).",
            EcScript::Ethi },
    // Latin ex E OK, dereferenced bad chars of Google Noto
    { 0xAB30, 0xAB6F,
            "Latin Extended-E", u8"Латиница расширенная E",
            u8"Буквы, используемые в германской и шотландской диалектологии, транскрипции индейских и сино-тибетских языков, "
                    "а также в латинской записи якутского (алфавит Новгородова, 1920–1929).",
            EcScript::Latn },
    // Cherokee supp OK
    { 0xAB70, 0xABBF,
            "Cherokee Supplement", u8"Чероки дополнительный",
            u8"В этом блоке находятся строчные буквы письменности чероки, кроме шести, закодированных в основном блоке.",
            EcScript::Cher },
    // Meetei OK
    { 0xABC0, 0xABFF,
            "Meetei Mayek", u8"Манипури (мейтей-майек)", {}, EcScript::Mtei },
    // Hangul syllables OK
    { 0xAC00, 0xD7AF,
            "Hangul Syllables", u8"Хангыль\u00A0— слоги",
            u8"[[pt:precomposed|Монолитные]] слоги хангыля, большей частью для совместимости со старыми кодировками. "
                "<p>Добавление корейского в Юникод сопровождалось большими трудностями. "
                    "В Юникоде 1.0 блок был на месте 3400…3D2D (сейчас там [[ps:Hani|китайские иероглифы]]). "
                    "В Юникоде 2.0 (1996) блок поставили на окончательное место, полагая, что корейского юникодного текста ещё мало. "
                    "Этот случай даже обозвали «корейский бардак». "
                    "В ответ Юникод выпустил главную гарантию [[pt:stability|стабильности]]: "
                        "больше ни один символ не сдвинется и не исчезнет.",
            EcScript::Hang, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // Hangul Jamo B OK
    /// @todo [desc] Hangul jamo B
    { 0xD7B0, 0xD7FF,
            "Hangul Jamo Extended-B", u8"Хангыль\u00A0— расширенные чамо B",
            {},
            EcScript::Hang, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // CJK compat hiero OK
    { 0xF900, 0xFAFF,
            "CJK Compatibility Ideographs", u8"ККЯ совместимые иероглифы",
            u8"Блок содержит [[ps:Hani|китайские иероглифы]], которые закодированы в других кодировках несколько раз, "
                        "чтобы не терять информацию при круговом [[pt:convertibility|преобразовании]]. "
                    "У них жёстко задано начертание. "
                    "Впрочем, встречаются и обычные [[pt:unification|унифицированные]]{{-}}например, FA0E и 0F, 11, 13, 14…",
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // Alphabetic presentation forms OK
    { 0xFB00, 0xFB4F,
            "Alphabetic Presentation Forms", u8"Алфавитные формы начертания",
            u8"Стандартные [[pt:ligature|лигатуры]] для [[ps:Latn|латиницы]], [[ps:Armn|армянского]] и [[ps:Hebr|иврита]]." },
    // Arabic presentation forms A OK
    { 0xFB50, 0xFDFF,
            "Arabic Presentation Forms-A", u8"Арабские формы начертания А",
            u8"Блок создан для [[pt:convertibility|совместимости]] со старыми кодировками вроде CP864 (DOS), "
                        "где из-за [[pt:dynamic|ограниченности]] типографских движков приходилось использовать разные коды "
                        "для обособленных, начальных, средних и конечных букв."
                "<p>32 символа FDDx и FDEx [[pt:noncharacter|зарезервированы как отсутствующие]]."
                "<p>Также содержит [[pt:spacing|протяжённые]] огласовки."
                "<p>В обычном письме эти символы лучше не использовать. "
                    "Единственное законное применение{{-}}учебные материалы, и потому символам дан класс «арабская буква» "
                        "(во многих тогдашних кодировках текст записывался слева направо).",
            EcScript::Arab, EcFont::ARABIC_NOTO, Bfg::HAS_32_NONCHARS },
    // Variation selectors OK
    { 0xFE00, 0xFE0F,
            "Variation Selectors", u8"Селекторы начертания",
            u8"Чаще всего (например, в браузерах) тот или иной вариант написания символа включают, выбирая другой шрифт "
                    "или включая функции OpenType. Юникод позволяет это ещё одним методом: созданием лигатур из символа и селектора."
                "<p>[[ps:Latn|Латиница]], [[ps:Mymr|бирманский и ещё несколько письменностей используют VS1. "
                    "[[ps:Hani|Китайские иероглифы]] используют VS1…VS3. [[pt:emoji|Эмодзи]] можно сделать цветными (VS16) "
                    "и текстовыми (VS15)." },
    // Vertical forms OK, not collapsible: small
    { 0xFE10, 0xFE1F,
            "Vertical Forms", u8"Вертикальные формы",
            u8"[[ps:Hani|Китайский язык]] изначально писался по столбцам справа налево, и сейчас так пишут "
                    "в художественных целях: стихи, открытки, вывески."
                "<p>Блок кодирует символы из китайской кодировки GB{{_}}18030 (2005), предназначенные для вертикального письма.",
            EcScript::NONE, EcFont::CJK_STRUCTURE },
    // Combining half marks OK
    { 0xFE20, 0xFE2F,
            "Combining Half Marks", u8"Комбинирующие полузнаки",
            u8"Части диакритических знаков, охватывающих несколько символов. "
                    "Например, можно накрыть три буквы {{sm|a︤c︦e︥}} одной чертой. "
                    "В большинстве шрифтов реализованы плохо, и автор «Юникодии» специально подобрал "
                    "три удачных символа. А вот церковнославянское сокращение {{sm|Х︮с︯}} выглядит плохо.",
            EcScript::NONE, EcFont::NOTO },
    // CJK compat forms OK
    /// @todo [desc] CJK compat forms
    { 0xFE30, 0xFE4F,
            "CJK Compatibility Forms", u8"ККЯ совместимые формы",
            {},
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // Small variants OK
    { 0xFE50, 0xFE6F,
            "Small Form Variants", u8"Малые формы знаков препинания",
            u8"Уменьшенные знаки препинания для совместимости с китайской кодировкой CNS 11643.",
            EcScript::NONE, EcFont::CJK, Bfg::COLLAPSIBLE },
    // Arabic forms B OK, no combinings
    { 0xFE70, 0xFEFF,
            "Arabic Presentation Forms-B", u8"Арабские формы начертания B",
            u8"Блок предназначен для [[pt:convertibility|совместимости]] со старыми кодировками, "
                    "где из-за [[pt:dynamic|ограниченности]] типографских движков приходилось использовать разные коды "
                    "для обособленных, начальных, средних и конечных букв."
            "<p>Содержит символ FEFF «[[pt:bom|метка порядка байтов]]»."
            "<p>В обычном письме эти символы лучше не использовать. "
                "Единственное законное применение{{-}}учебные материалы, и потому символам дан класс «арабская буква» "
                    "(во многих тогдашних кодировках текст записывался слева направо).",
            EcScript::Arab, EcFont::ARABIC_NOTO },
    // Half/full OK
    { 0xFF00, 0xFFEF,
            "Halfwidth and Fullwidth Forms",
            u8"Полуширинные и полноширинные формы",
            u8"В ККЯ иероглиф традиционно занимает одну клетку сетки, а европейская буква{{-}}половину клетки. "
                    "Отсюда необычное начертание текста на китайских товарах с плохо подобранными шрифтами: "
                        "так, в интернете нашумели стельки «Атлетизм»."
                "<p>В этом блоке собраны «неправильные» символы, занимающие половину клетки вместо целой и наоборот.",
            EcScript::NONE, EcFont::CJK, Bfg::COLLAPSIBLE },
    // Specials OK
    { 0xFFF0, 0xFFFF,
            "Specials", u8"Спецсимволы",
            u8"Короткий блок в конце [[pt:bmp|базовой плоскости]]. Из 14 символов "
                    "(два [[pt:noncharacter|отсутствующие]]) назначены только пять: "
                        "три символа межстрочных помет, символ подстановки в составном документе "
                        "(«здесь должна быть картинка») и знак неудачного считывания."
                "<p>С последним, {{sm|�}}, мы чаще всего и сталкиваемся{{-}}он означает, что «крокозябра» появилась "
                        "не из-за отсутствия символа в шрифте, а из-за неудачной раскодировки. "
                        "Обычно{{-}}текст в однобайтовой кодировке попыталось прочитать как [[pt:utf8|UTF-8]]." },
    // Linear B syll OK
    { 0x10000, 0x1007F,
            "Linear B Syllabary", u8"Линейное письмо Б\u00A0— слоги", {}, EcScript::Linb },
    // Linear B hier OK
    { 0x10080, 0x100FF,
            "Linear B Ideograms", u8"Линейное письмо Б\u00A0— иероглифы", {}, EcScript::Linb },
    // Aegean numbers OK
    { 0x10100, 0x1013F,
            "Aegean Numbers", u8"Эгейские цифры",
            u8"Очень простая десятичная система счисления, использовавшаяся минойской и крито-микенской цивилизациями."
                    "Замечена в линейном письме [[ps:Lina|А]] и [[ps:Linb|Б]]. "
                    "В [[ps:Cprt|кипрском]] письме была обнаружена единственная табличка с числом «100».",
            EcScript::Linb },
    // Greek numbers OK
    { 0x10140, 0x1018F,
        "Ancient Greek Numbers", u8"Древнегреческие цифры",
        u8"Аттическая система счисления была [[pt:acrophonic|акрофонической]], то есть цифрами служили первые буквы слов:<br>"
                "• Ι = 1, ἴος<br>"
                "• обрезанное Π = 5, πέντε<br>"
                "• Δ = 10, δέκα<br>"
                "• Η = 100, ηἑκατόν<br/>"
                "• Χ = 1000, χίλιοι<br/>"
                "• Μ = 10{{_}}000, μύριοι"
            "<p>Для цифр 50, 500 и{{_}}т.д. существовали символы, состоящие из Π и 10/100/1000…"
            "<p>Впоследствии аттическая система уступила место [[ps:Grek|ионийской]] "
                "(Α=1, Β=2, …, Ι=10, Κ=20, …, Ρ=100, Σ=200…), "
                "но взаимообмен культур повлиял на этрусков, что и привело к появлению римских цифр."
            "<p>Также в этом блоке находятся символы древнегреческих единиц измерения."
            "<p>Слово ''литра'', так греющее душу русским алкоголикам, действительно родственное ''литру''. "
                "В древней Греции ''литра''{{-}}мера массы и монета. "
                "Во Франции ''литрóн''{{-}}мера объёма сыпучих веществ, отсюда ''литр''.",
            EcScript::NONE, EcFont::NOTO_SYMBOL2 },
    // Ancient symbols OK
    { 0x10190, 0x101CF,
            "Ancient Symbols", u8"Древние символы",
            u8"Римские символы для денег, мер и весов. "
                    "Также греческое библейское сокращение «крест» (реже «Христос»).",
            EcScript::NONE, EcFont::NOTO_SYMBOL2 },
    // Phaistos Disc OK
    { 0x101D0, 0x101FF,
        "Phaistos Disc", u8"Фестский диск",
        u8"Фестский диск{{-}}артефакт крито-минойской (прото-греческой) культуры. Диск, сделанный из глины "
                "без гончарного круга, найден в 1908 на Крите, имеет диаметр 160{{_}}мм и толщину 20{{_}}мм. "
                "Датируется 2100–1100{{bc}}, и содержит две надписи, сделанных по спирали "
                "неизвестной письменностью с помощью штампов. "
                "Выставлен в Археологическом музее Ираклиона (Крит)."
            "<p>Общепринята гипотеза, что текст читается от края к центру (справа налево). "
                "Для написания слева направо многие из символов отражены: так, в оригинале ПЕШЕХОД "
                    "идёт направо. "
                "Никто не знает, критская ли это письменность и к какому типу относится. "
                "В числе необычных гипотез{{-}}подделка работавшего на раскопках художника, "
                    "список владений критского царя и игра-ходилка."
            "<p>В 2006 Фестский диск предложен в Реестр искусственных письменностей на место "
                "E6D0–E6FF; порядок символов тот же (по Артуру Эвансу, 1905, по убыванию частоты). "
                "С появлением Фестского диска в настоящем Юникоде диапазон расформирован. "
                "Два символа из Реестра в Юникод не вошли и берутся из латиницы: | и ¦. "
                "Названия символов{{-}}по Луи Годáру (1995).",
            EcScript::NONE, EcFont::PHAISTOS_DISC },
    // Lycian OK
    { 0x10280, 0x1029F,
            "Lycian", u8"Ликийский", {}, EcScript::Lyci },
    // Carian OK
    { 0x102A0, 0x102DF,
            "Carian", u8"Карийский", {}, EcScript::Cari },
    // Coptic epact OK
    { 0x102E0, 0x102FF,
        "Coptic Epact Numbers", u8"Коптские курсивные цифры",
        u8"Здесь слово ''epact'' (греч. ἐπακτός) означает «заимствованные», а не «епакта» (формулы для перевода из солнечного "
                "календаря в лунный) или «пасхалия» (формулы для расчёта пасхи). "
                "И действительно, эти цифры, будучи потомками обычной [[ps:Copt|коптской]] записи "
                "(которая, в свою очередь, позаимствована из [[ps:Grek|греческого]]), широко использовались в копто-арабском мире "
                "с X по XIX век; в частности, в астрономических таблицах ≈1800. Запись слева направо."
            "<p>Из-за такого необычного использования эти цифры закодированы отдельным блоком, а не как вариант коптского."
            "<p>В изображениях символов вы увидите серьёзное расхождение с таблицами Юникода: специалист по древним "
                "письменностям ''Anshuman Pandey'' обвёл старые чернильные символы, а проект "
                "''Google Noto'' попытался повторить росчерки постоянной шириной.",
            EcScript::NONE, EcFont::NOTO_SYMBOL2 },
    // Old Italic OK
    { 0x10300, 0x1032F,
            "Old Italic", u8"Этрусский", {}, EcScript::Ital },
    // Gothic OK
    { 0x10330, 0x1034F,
            "Gothic", u8"Готский", {}, EcScript::Goth },
    // Old Permic OK
    { 0x10350, 0x1037F,
            "Old Permic", u8"Древнепермский (анбур)", {}, EcScript::Perm },
    // Ugaritic OK
    { 0x10380, 0x1039F,
            "Ugaritic", u8"Угаритский", {}, EcScript::Ugar },
    // Old Persian OK
    { 0x103A0, 0x103DF,
            "Old Persian", u8"Древнеперсидский", {}, EcScript::Xpeo },
    // Deseret OK
    { 0x10400, 0x1044F,
            "Deseret", u8"Дезеретский (мормонский)", {}, EcScript::Dsrt },
    // Shavian OK
    { 0x10450, 0x1047F,
            "Shavian", u8"Алфавит Бернарда Шоу", {}, EcScript::Shaw },
    // Osmanya OK
    { 0x10480, 0x104AF,
            "Osmanya", u8"Османья (сомалийский)", {}, EcScript::Osma },
    // Osage OK
    { 0x104B0, 0x104FF,
            "Osage", u8"Осейдж", {}, EcScript::Osge },
    // Elbasan OK
    { 0x10500, 0x1052F,
            "Elbasan", u8"Эльбасанский албанский", {}, EcScript::Elba },
    // Caucasian OK
    { 0x10530, 0x1056F,
            "Caucasian Albanian", u8"Агванский (Кавказская Албания)", {}, EcScript::Aghb },
    // Linear A OK
    { 0x10600, 0x1077F,
            "Linear A", u8"Линейное письмо А", {}, EcScript::Lina },
    // Cypriot OK
    { 0x10800, 0x1083F,
            "Cypriot Syllabary", u8"Кипрская", {}, EcScript::Cprt },
    // Imperial Aramaic OK
    { 0x10840, 0x1085F,
            "Imperial Aramaic", u8"Имперский арамейский", {}, EcScript::Armi },
    // Palmyrene OK
    { 0x10860, 0x1087F,
            "Palmyrene", u8"Пальмирский", {}, EcScript::Palm },
    // Nabataean OK
    { 0x10880, 0x108AF,
            "Nabataean", u8"Набатейский", {}, EcScript::Nbat },
    // Hatran OK
    { 0x108E0, 0x108FF,
            "Hatran", u8"Хатранский (ашшурский)", {}, EcScript::Hatr },
    // Phoenician OK
    { 0x10900, 0x1091F,
            "Phoenician", u8"Финикийский", {}, EcScript::Phnx },
    // Lydian OK
    { 0x10920, 0x1093F,
            "Lydian", u8"Лидийский (сардийский)", {}, EcScript::Lydi },
    // Meroitic hiero OK
    { 0x10980, 0x1099F,
            "Meroitic Hieroglyphs", u8"Мероитские иероглифы", {}, EcScript::Mero },
    // Meroitic cursive OK
    { 0x109A0, 0x109FF,
            "Meroitic Cursive", u8"Мероитский курсив", {}, EcScript::Mero },
    // Kharoshthi OK
    { 0x10A00, 0x10A5F,
            "Kharoshthi", u8"Кхароштхи", {}, EcScript::Khar },
    // Old South Arab OK
    { 0x10A60, 0x10A7F,
            "Old South Arabian", u8"Древняя южноаравийская", {}, EcScript::Sarb },
    // Old North Arab OK
    { 0x10A80, 0x10A9F,
            "Old North Arabian", u8"Древняя североаравийская", {}, EcScript::Narb },
    // Manichaean OK
    { 0x10AC0, 0x10AFF,
            "Manichaean", u8"Манихейская", {}, EcScript::Mani },
    // Avestan OK
    { 0x10B00, 0x10B3F,
            "Avestan", u8"Авестийский", {}, EcScript::Avst },
    // Inscr Parthian OK
    { 0x10B40, 0x10B5F,
            "Inscriptional Parthian", u8"Парфянские надписи", {}, EcScript::Prti },
    // Inscr Pahlavi OK
    { 0x10B60, 0x10B7F,
            "Inscriptional Pahlavi", u8"Пехлевийские надписи", {}, EcScript::Phli },
    // Psalter Pahlavi OK
    { 0x10B80, 0x10BAF,
            "Psalter Pahlavi", u8"Псалтырское пехлеви", {}, EcScript::Phlp },
    // Turkic runes OK
    { 0x10C00, 0x10C4F,
            "Old Turkic", u8"Тюркские (орхоно-енисейские) руны", {}, EcScript::Orkh },
    // Hung runes OK
    { 0x10C80, 0x10CFF,
            "Old Hungarian", u8"Венгерские (секейские) руны", {}, EcScript::Hung },
    // Hanifi OK
    { 0x10D00, 0x10D3F,
            "Hanifi Rohingya", u8"Ханифи (рохинджа)", {}, EcScript::Rohg },
    // Rumi OK
    { 0x10E60, 0x10E7F,
            "Rumi Numeral Symbols", u8"Цифры руми",
            u8"Цифры, использовавшиеся в арабской Африке с X по XVII{{_}}век, особенно в Фесе ''(Марокко)''.",
            EcScript::NONE, EcFont::NOTO_SYMBOL2 },
    // Yezidi OK
    { 0x10E80, 0x10EBF,
            "Yezidi", u8"Езидская", {}, EcScript::Yezi },
    // Old Sogdian OK
    { 0x10F00, 0x10F2F,
            "Old Sogdian", u8"Старосогдийский", {}, EcScript::Sogo },
    // Sogdian OK
    { 0x10F30, 0x10F6F,
            "Sogdian", u8"Согдийский", {}, EcScript::Sogd },
    // Chorasmian OK, built own font
    { 0x10FB0, 0x10FDF,
            "Chorasmian", u8"Хорезмийский", {}, EcScript::Chrs },
    // Elymaic OK
    { 0x10FE0, 0x10FFF,
            "Elymaic", u8"Элимайский (эламский)", {}, EcScript::Elym },
    // Brahmi OK
    { 0x11000, 0x1107F,
            "Brahmi", u8"Брахми", {}, EcScript::Brah },
    // Kaithi OK
    { 0x11080, 0x110CF,
            "Kaithi", u8"Кайтхи", {}, EcScript::Kthi },
    // Sora OK
    { 0x110D0, 0x110FF,
            "Sora Sompeng", u8"Соранг-сомпенг", {}, EcScript::Sora },
    // Chakma OK
    { 0x11100, 0x1114F,
            "Chakma", u8"Чакма", {}, EcScript::Cakm },
    // Mahajani OK
    { 0x11150, 0x1117F,
            "Mahajani", u8"Махаджани", {}, EcScript::Mahj },
    // Sharada OK
    { 0x11180, 0x111DF,
            "Sharada", u8"Шарада", {}, EcScript::Shrd },
    // Sinhala archaic OK
    { 0x111E0, 0x111FF,
            "Sinhala Archaic Numbers", u8"Старые сингальские цифры",
            u8"Старинная непозиционная система счисления, использовавшаяся в Шри-Ланке до начала XIX{{_}}века." },
    // Khojki OK
    { 0x11200, 0x1124F,
            "Khojki", u8"Ходжки", {}, EcScript::Khoj },
    // Multani OK
    { 0x11280, 0x112AF,
            "Multani", u8"Мултани", {}, EcScript::Mult },
    // Khuda OK
    { 0x112B0, 0x112FF,
            "Khudawadi", u8"Кхудабади (синдхи)", {}, EcScript::Sind },
    // Grantha OK
    { 0x11300, 0x1137F,
            "Grantha", u8"Грантха", {}, EcScript::Gran },
    // Newa OK
    { 0x11400, 0x1147F,
            "Newa", u8"Неварский (прахалит)", {}, EcScript::Newa },
    // Tirhuta OK
    { 0x11480, 0x114DF,
            "Tirhuta", u8"Тирхута (митхилакшар)", {}, EcScript::Tirh },
    // Siddham OK
    { 0x11580, 0x115FF,
            "Siddham", u8"Сиддхаматрика (сиддхам, бондзи)", {}, EcScript::Sidd },
    // Modi OK
    { 0x11600, 0x1165F,
            "Modi", u8"Моди", {}, EcScript::Modi },
    // Mongolian supp OK
    { 0x11660, 0x1167F,
            "Mongolian Supplement", u8"Монгольский дополнительный",
            u8"Символ «бирга», проникший в монгольский из [[ps:Tibt|тибетского]] и играющий роль буквицы. "
                        "Поскольку монгольский пишется сверху вниз, а тибетский{{-}}слева направо, тибетские ''yig-mgo'' использовать нельзя."
                "<p>Юникод расщедрился не на варианты написания, а на настоящие кодовые позиции{{-}}"
                        "правда, не в [[pt:bmp|базовой плоскости]], а в дополнительной."
                "<p>Иконка соответствует написанию сверху вниз{{-}}"
                        "а глифы в шрифтах повёрнуты на 90° против часовой стрелки.",
            EcScript::Mong },
    // Takri OK
    { 0x11680, 0x116CF,
            "Takri", u8"Такри", {}, EcScript::Takr },
    // Ahom OK
    { 0x11700, 0x1173F,
            "Ahom", u8"Ахом", {}, EcScript::Ahom },
    // Dogra OK
    { 0x11800, 0x1184F,
            "Dogra", u8"Догра", {}, EcScript::Dogr },
    // Warang OK
    { 0x118A0, 0x118FF,
            "Warang Citi", u8"Варанг-кшити", {}, EcScript::Wara },
    { 0x11900, 0x1195F,
            "Dives Akuru", u8"Дивес-акуру (древнемальдивский)", {}, EcScript::Diak },
    { 0x119A0, 0x119FF,
            "Nandinagari", u8"Нандинагари", {}, EcScript::Nand },
    { 0x11A00, 0x11A4F,
            "Zanabazar Square", u8"Монгольское горизонтально-квадратное (Дзанабадзара)", {}, EcScript::Zanb },
    { 0x11A50, 0x11AAF,
            "Soyombo", u8"Соёмбо", {}, EcScript::Soyo },
    { 0x11AC0, 0x11AFF,
            "Pau Cin Hau", u8"Письмо По Чин Хо", {}, EcScript::Pauc },
    { 0x11C00, 0x11C6F,
            "Bhaiksuki", u8"Бхаикшуки (стрелоглавое письмо)", {}, EcScript::Bhks },
    { 0x11C70, 0x11CBF,
            "Marchen", u8"Марчхен", {}, EcScript::Marc },
    { 0x11D00, 0x11D5F,
            "Masaram Gondi", u8"Письмо Масарама (гонди)", {}, EcScript::Gonm },
    { 0x11D60, 0x11DAF,
            "Gunjala Gondi", u8"Гунджала (гонди)", {}, EcScript::Gong },
    { 0x11EE0, 0x11EFF,
            "Makasar", u8"Макасарский (птичье письмо)", {}, EcScript::Maka },
    // Lisu supplement OK, new font engine works
    { 0x11FB0, 0x11FBF,
            "Lisu Supplement", u8"Лису дополнительный",
            u8"Один символ из языка наси (юго-западный Китай, 300{{_}}тыс., в безопасности).",
            EcScript::Lisu },
    // Tamil supp OK
    { 0x11FC0, 0x11FFF,
            "Tamil Supplement", u8"Тамильский дополнительный",
            u8"Старинные тамильские дроби и другие символы: для валют, сельского хозяйства, бухгалтерии.",
            EcScript::Taml, EcFont::TAMIL_SUPPLEMENT },
    // Cuneiform OK
    { 0x12000, 0x123FF,
            "Cuneiform", u8"Клинопись", {}, EcScript::Xsux },
    // Cuneiform nembers and punct OK
    { 0x12400, 0x1247F,
            "Cuneiform Numbers and Punctuation",
            u8"Клинописные цифры и знаки препинания",
            u8"Вавилонянам около 2000{{bc}} приписывается первая позиционная система счисления, основание у неё было 60. "
                    "{{sm|𒐕}}{{-}}единица от 0 до 9 штук, {{sm|𒌋}}{{-}}десятка от 0 до 5. "
                    "Ноль {{sm|𒑊}} писали только между разрядами, но не{{_}}на конце числа, порядок можно определить "
                        "только из контекста."
                "<p>До этого использовалась непозиционная система, когда разные предметы и разные разряды "
                    "записывались разными знаками.",
            EcScript::Xsux },
    // Early cuneiform OK
    { 0x12480, 0x1254F,
            "Early Dynastic Cuneiform",
            u8"Раннединастическая клинопись", {}, EcScript::Xsux },
    // Egyptian hiero OK
    { 0x13000, 0x1342F,
            "Egyptian Hieroglyphs", u8"Египетские иероглифы", {}, EcScript::Egyp },
    // Egyptian hiero format OK
    { 0x13430, 0x1343F,
            "Egyptian Hieroglyph Format Controls",
            u8"Форматирующие символы для египетских иероглифов",
            u8"Семь необязательных символов, обеспечивающих полное форматирование египетских иероглифов.",
            EcScript::Egyp },
    /// @todo [tofu, P1] Anatolyan hiero
    { 0x14400, 0x1467F,
            "Anatolian Hieroglyphs", u8"Анатолийские иероглифы", {}, EcScript::Hluw },
    // Bamum supplement OK
    { 0x16800, 0x16A3F,
            "Bamum Supplement", u8"Бамум дополнительный",
            u8"Ранние варианты [[ps:Bamu|бамума]] A…F, как иероглифические, так и слоговые. "
                "Окончательная слоговая версия G{{-}}в [[pt:bmp|базовой плоскости]].",
            EcScript::Bamu },
    { 0x16A40, 0x16A6F,
            "Mro", u8"Мру", {}, EcScript::Mroo },
    { 0x16AD0, 0x16AFF,
            "Bassa Vah", u8"Басса", {}, EcScript::Bass },
    { 0x16B00, 0x16B8F,
            "Pahawh Hmong", u8"Пахау хмонг", {}, EcScript::Hmng },
    { 0x16E40, 0x16E9F,
            "Medefaidrin", u8"Обэри-окаимэ (медефайдрин)", {}, EcScript::Medf },
    { 0x16F00, 0x16F9F,
            "Miao", u8"Мяо (письмо Полларда)", {}, EcScript::Plrd },
    // Ideographic sym/punct OK
    { 0x16FE0, 0x16FFF,
            "Ideographic Symbols and Punctuation",
            u8"Символы и знаки препинания для иероглифов",
            u8"Различные символы, используемые в старых [[pt:cjk|ККЯ]]-письменностях: "
                        "[[ps:Nshu|нюй-шу]], [[ps:Tang|тангутском]], [[ps:Kits|малом киданьском]], "
                        "старых [[ps:Hani|китайских иероглифах]]."
                "<p>Одинаковые иероглифы не пишут рядом, вместо этого используют знак повтора. "
                    "В современном печатном китайском уже повторяют, китайцы неформально и японцы используют {{sm|々}} U+3005."
                "<p>Крючок в старых рукописях означает конец предложения или главы. "
                "<p>Если блок малого киданьского начинается с одного иероглифа, а не двух, на вакантное место ставят заполнитель. "
                "<p>Пока Вьетнам писал иероглифами, два символа служили для передачи местного произношения. "
                    "У обоих есть два варианта, никогда не встречавшиеся в одном тексте и потому [[pt:unification|унифицированные]]. "
                    "Вторая форма напоминает соответственно стрелку <big>↑</big> и молнию <big>ϟ</big>.",
            // Hard block, though small, but all chars have scripts.
            // Hani is just over-insurance.
            EcScript::Hani },
    // Tangut OK
    { 0x17000, 0x187FF,
            "Tangut", u8"Тангутский", {},
            EcScript::Tang, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // Tangut components OK
    { 0x18800, 0x18AFF,
            "Tangut Components", u8"Элементы тангутского письма",
            u8"Компоненты и радикалы, используемые в современном изучении тангутского письма.",
            EcScript::Tang, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // Khitan small OK
    { 0x18B00, 0x18CFF,
            "Khitan Small Script", u8"Малое киданьское письмо",
            {}, EcScript::Kits, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // Tangut supplement OK, not collapsible: too small
    { 0x18D00, 0x18D8F,
            "Tangut Supplement", u8"Тангутский дополнительный",
            u8"Блок содержит девять [[pt:unification|деунификаций]] тангутского языка. "
                "Другими словами: все девять иероглифов{{-}}чьи-то омографы. "
                "Например: «пара» осталась на старом месте 17134, а омограф «глупый» получил новый номер 18D00.",
            EcScript::Tang },
    // Kana supp OK
    { 0x1B000, 0x1B0FF,
            "Kana Supplement", u8"Кана дополнительная",
            u8"Один устаревший символ [[ps:Kana|катаканы]] и 255 символов [[ps:Hent|хэнтайганы]] (старой [[ps:Hira|хираганы]]). "
                    "Хэнтайгана продолжается и в следующем блоке.",
            EcScript::NONE, EcFont::KOREAN, Bfg::COLLAPSIBLE },
    { 0x1B100, 0x1B12F,
            "Kana Extended-A", u8"Кана расширенная A",
            u8"35 (на Юникод{{_}}14) символов [[ps:Hent|хэнтайганы]]{{-}}старой [[ps:Hira|хираганы]].",
            EcScript::NONE, EcFont::KOREAN, Bfg::COLLAPSIBLE },
    // Small kana ex OK
    { 0x1B130, 0x1B16F,
            "Small Kana Extension", u8"Маленькие символы каны",
            u8"Маленькие символы катаканы используются для передачи звуков, отсутствующих в японском. "
                    "Так, маленькое {{sm|ケヰ}} передаёт огублённое «кʷи». "
                    "Также добавлены соответствующие символы хираганы{{-}}правда, к моменту принятия заявка похудела: "
                        "отпали «ко» обеих азбук и «н» хираганы.",
            EcScript::NONE, EcFont::KOREAN },
    // Nushu OK
    { 0x1B170, 0x1B2FF,
            "Nushu", u8"Нюй-шу",
            {}, EcScript::Nshu, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    { 0x1BC00, 0x1BC9F,
            "Duployan", u8"Стенография Дюплойе", {}, EcScript::Dupl },
    { 0x1BCA0, 0x1BCAF,
            "Shorthand Format Controls",
            u8"Форматирующие символы стенографии" },
    // Byzantine music seemingly OK
    { 0x1D000, 0x1D0FF,
            "Byzantine Musical Symbols",
            u8"Византийские музыкальные символы",
            u8"О византийской церковной музыке известно мало, а о светской{{-}}вообще ничего. "
                    "Тем не менее, символы византийского происхождения использовались православными как минимум до XIX{{_}}века, "
                        "даже под властью турок, и были реформированы около 1800.",
            EcScript::NONE, EcFont::MUSIC_BIGGER  },
    /// @todo [tofu, P1] How to show chars like NULL-NOTE-HEAD?
    /// @todo [semi-tofu, P1] Check and take smth from Segoe UI Symbol
    { 0x1D100, 0x1D1FF,
            "Musical Symbols", u8"Музыкальные символы",
            u8"С X{{_}}века появилась нотная запись, похожая на современную. "
                    "Ноты были квадратными, потому что рисовались жирным пером. "
                    "Для певцов делали 4 линии на строку, для инструментов больше. "
                    "Абсолютной привязки к частотам не{{_}}было. "
                    "Круглые ноты появились в попытке бегло нарисовать ноту пером средней толщины, тем более такая нота "
                        "может передавать какую-то информацию вроде длительности. "
                    "И поныне производят такие перья для музыкантов."
                "<p>До современного вида ноты дошли в XVII{{_}}веке с появлением равнотемперированного строя, "
                    "позволившего переносить музыку на другие инструменты, играть оркестрами."
                "<p>Музыка{{-}}[[pt:universality|нетекстовая]] информация и потому вне внимания Юникода. "
                    "Однако заманчиво использовать имеющиеся типографские движки для хранения нот и их частей в шрифтах, "
                        "чтобы упрощать нотное ПО, сокращать нотные PDF. "
                    "В этом свете непонятно назначение восьми [[pt:format|форматирующих]] символов (ребро, связка, лига, фраза). "
                    "В 2013 вышел открытый стандарт ''Standard Music Font Layout'' (SMuFL), расширяющий блок "
                        "до более чем 2600 символов с использованием позиций для [[pt:private|личного пользования]]."
                "<p>Также в этом блоке есть несколько символов из устаревших музыкальных нотаций.",
            EcScript::NONE, EcFont::MUSIC },
    // Greek music OK
    { 0x1D200, 0x1D24F,
            "Ancient Greek Musical Notation",
            u8"Древнегреческая музыкальная нотация",
            u8"Про древних греков известно мало. "
                    "Изначально они имели пять нот на октаву, компенсируя такую нехватку разными методами настройки инструментов, "
                        "но впоследствии добрались до полутонов, изредка используя четверть-тоны. "
                    "Основными инструментами были авлос (две дудки, на которых играют одновременно) и лира."
                "<p>Для музыкантов и певцов были разные нотации, это позволяло писать одновременно слова, музыку и подсказки для певцов. "
                    "Ноты записывались старинными буквами, и те из них, что не{{_}}совпадают с греческими, приведены в этом блоке "
                        "без всякой системы.",
            EcScript::NONE, EcFont::MUSIC_NORMAL },
    // Mayan numerals OK
    { 0x1D2E0, 0x1D2FF,
            "Mayan Numerals", u8"Цифры майя",
            u8"Майя пользовались двадцатеричной позиционной системой счисления. "
                    "Разряды записывались сверху вниз. "
                    "Иногда вместо цифры рисовали лицо соответствующего бога. "
                    "Считают, что позиционный счёт и ноль были изобретены до майя (возможно, ольмеками) до нашей эры. "
                    "Ракушки, палочки и точки говорят, что такая система счисления родом со счётных досок."
                "<p>Некоторые источники говорят, что в быту майя пользовались более простой непозиционной системой счисления.",
            EcScript::NONE, EcFont::NOTO_SYMBOL2 },
    { 0x1D300, 0x1D35F,
            "Tai Xuan Jing Symbols", u8"Символы Тайсюаньцзин (Канона великой тайны)",
            u8"Тайсюаньцзин (Канон великой тайны){{-}}китайский гадательный трактат 2{{_}}года нашей эры, вдохновлённый Книгой перемен. "
                    "В трактате описана 81 тетраграмма. "
                    "Каждая линия бывает в трёх вариантах: сплошная (небеса), с одним разрывом (земля), и с двумя разрывами (человек). "
                    "Надлежало четыре раза выкинуть случайное число, получить тетраграмму и прочитать её расшифровку в книге."
                "<p>В Консорциуме Юникода по ошибке спутали землю и человека. "
                    "С гарантиями [[pt:stability|стабильности]] оставалось только объяснить эту досадную ошибку в комментариях."
                "<p>Часть монограмм и диграмм уже есть в базовой плоскости." },
    { 0x1D360, 0x1D37F,
            "Counting Rod Numerals", u8"Счётные палочки",
            u8"Палочками учат считать детей. "
                    "Древние китайцы, начиная с Воюющих царств (V{{_}}век) до XVI{{_}}века пользовались настоящей позиционной "
                    "системой счисления из палочек, раскладываемых на счётной доске. "
                    "Была запись для дробей и даже отрицательных чисел (палочками другого цвета)."
                "<p>Вертикальные палочки означали единицы, сотни, десятки тысяч…, горизонтальные{{-}}десятки, тысячи, сотни тысяч… "
                    "Иногда наоборот{{-}}именно в таком виде палочки вошли в Юникод. "
                    "Символы счётных палочек часто писали и на бумаге."
                "<p>Если нужно что-то сосчитать и запомнить количество, используют счётные символы. "
                    "Европейские пятёрки {{sm|𝍸}} чаще всего ассоциируются с тюрьмой. "
                    "Китайцы пишут иероглиф <font size='+1'>正</font>. Лесники считают десятками, этот метод описан у Я.И.{{_}}Перельмана, "
                    "но отсутствует в Юникоде.",
            EcScript::NONE, EcFont::NOTO_SYMBOL2_BIGGER },
    /// @todo [link,block] Letterlike
    // Math alnum OK
    { 0x1D400, 0x1D7FF,
            "Mathematical Alphanumeric Symbols",
            u8"Математические буквы и цифры",
            u8"Математики обозначают различными шрифтами объекты, с которыми имеют дело:<br>"
                    "• векторы{{-}}жирным, скаляры{{-}}курсивом: {{sm|𝐅=𝑚𝐚}};<br>"
                    "• особые множества{{-}}жирным или ажурным: {{sm|ℕ}}{{-}}натуральные числа 1, 2, 3…;<br>"
                    "• пространства{{-}}рукописным: {{sm|𝒰}}{{-}}универсальное множество;<br>"
                    "• мощности множеств{{-}}готическим: {{sm|𝔠}}{{-}}континуум;<br>"
                    "• различные определения одного и того же{{-}}рукописным или готическим: предел {{sm|ℋlim}} по Гейне "
                        "и {{sm|𝒞lim}} по Коши;<br>"
                    "• функции{{-}}самыми разными шрифтами: математическое ожидание {{sm|𝖬ξ}} часто без засечек;<br>"
                    "• константы{{-}}жирным или ажурным: границы решётки {{sm|𝟘}} и {{sm|𝟙}}, мнимые единицы {{sm|𝕚𝕛𝕜}}…"
                "<p>Ажурный шрифт {{sm|ℕℤℚℝℂ}} происходит из жирного, записанного мелом на доске. "
                    "Распространился он со знаменитым учебником по комплексному анализу 1965{{_}}года Ганнинга и Росси."
                "<p>Наиболее распространённые из таких обозначений находятся в базовой плоскости в разделе буквоподобных символов. "
                    "С расширением Юникода за базовую плоскость добавили полный латинский алфавит и арабские цифры разными шрифтами." },
    // Sutton SignWriting OK
    { 0x1D800, 0x1DAAF,
            "Sutton SignWriting", u8"Жестовое письмо Саттон", {}, EcScript::Sgnw },
    // Glagolitic supp OK
    { 0x1E000, 0x1E02F,
            "Glagolitic Supplement", u8"Глаголица дополнительная",
            u8"Надстрочные буквы глаголицы, встречающиеся в различных источниках.",
            EcScript::Glag },
    // NP Hmong OK
    { 0x1E100, 0x1E14F,
            "Nyiakeng Puachue Hmong", u8"Ньякэ пуацы хмонг (алфавит Черванга)", {}, EcScript::Hmnp },
    { 0x1E2C0, 0x1E2FF,
            "Wancho", u8"Ванчо", {}, EcScript::Wcho },
    { 0x1E800, 0x1E8DF,
            "Mende Kikakui", u8"Кикакуи (менде)", {}, EcScript::Mend },
    // Adlam OK
    { 0x1E900, 0x1E95F,
            "Adlam", u8"Адлам", {}, EcScript::Adlm },
    { 0x1EC70, 0x1ECBF,
            "Indic Siyaq Numbers", u8"Индийские цифры сийяк",
            u8"Арабские непозиционные цифры, применявшиеся в Индии с XVII{{_}}века "
                        "(ещё под империей Великих Моголов) до середины XX{{_}}века. "
                    "Арабское «сийяк» означает «порядок». "
                    "Также цифры известны под названием «ракм», арабское «счёт»."
                "<p>Цифры до 90{{_}}тысяч [[pt:acrophonic|акрофонические]], из арабского, "
                        "со 100{{_}}тысяч{{-}}индийские слова. "
                    "Написание справа налево, порядок сложный: 515 записывается как 500, 5 (в варианте ''prefixed''), 10.",
            EcScript::NONE, EcFont::SIYAQ_INDIC },
    { 0x1ED00, 0x1ED4F,
            "Ottoman Siyaq Numbers", u8"Османские цифры сийяк" },
    // Arab mathematical OK, W10 Amiri
    { 0x1EE00, 0x1EEFF,
            "Arabic Mathematical Alphabetic Symbols",
            u8"Арабские математические алфавитные символы",
            u8"У арабов своя математическая нотация. "
                    "Они пользуются [[ps:Arab|собственными]] буквами с написанием справа налево, "
                        "и как на Западе есть прямой, жирный, курсив и другие, "
                        "у арабов есть буквы изолированные, начальные, с хвостом, с оттяжкой, с петлей и ажурные. "
                    "Знаки общеевропейские отзеркаленные, но встречаются и собственные{{-}}например, сумма. "
                        "Озеркаливают даже знаки «существует» или «частный дифференциал»."
                "<p>Первыми такой набор реализовали разработчики системы математического текста TᴇX (читается «тех»), и в 2012, "
                    "после семи лет утрясок, эти наработки перенесли в Юникод.",
            EcScript::NONE, EcFont::MATH },
    // Mahjong tiles OK
    { 0x1F000, 0x1F02F,
            "Mahjong Tiles", u8"Фишки маджонга",
            u8"Азартная игра мацзян (часто через романизацию именуемая маджонг) изобретена в конце XIX{{_}}века, "
                        "первый сохранившийся набор{{-}}1870. "
                    "Сама игра западному миру малоизвестна, однако распространён пасьянс: с боков фигуры снимать одинаковые фишки. "
                "<p>Пасьянс изобрёл студент Броди Локард в 1981{{-}}он упал с батута, был полностью парализован и во время реабилитации "
                        "уговорил компанию CDC поставить терминал, с которого и программировал, держа в зубах палку. "
                    "Пасьянс был помесью двух китайских игр{{-}}маджонга и «охоты на черепаху», игравшейся костями домино. "
                    "Терминал системы PLATO был монохромным с разрешением 512×512{{-}}этого хватало для столь сложной игры."
                "<p>Масти маджонга происходят от одной монеты (кóльца), связки из 100 монет (бамбуки) и 10{{_}}тысяч монет "
                        "(иероглиф внизу означает 10000). "
                    "Козыри{{-}}четыре ветра, три дракона, четыре времени года и четыре цветка. "
                    "В некоторых вариантах маджонга встречаются и другие козыри{{-}}профессии (рыбак, дровосек, крестьянин, учёный) "
                        "или виды искусства (музыка, го, каллиграфия, рисование)." },
    // Domino tiles OK
    { 0x1F030, 0x1F09F,
            "Domino Tiles", u8"Кости домино",
            u8"Домино изобрели китайцы около XIII{{_}}века. "
                    "В XVIII{{_}}веке игра попала в Италию."
                "<p>Есть три версии происхождения названия. "
                    "По первой, выигравший становился ''domino'', то есть господином. "
                    "По второй, игру разрешали в монастырях, а там любое дело начинали с молитвы Господу (итал. ''domino''). "
                    "По третьей{{-}}в честь чёрно-белого карнавального костюма ''domino'', разыгрывающего монаха."
                "<p>В результате переосмысления слова «домино» появились ''тримино'', ''тетрамино'' и{{_}}т.д.{{-}}"
                        "фигуры из 3, 4 и{{_}}т.д. квадратов.",
            // Fonts coincide
            EcScript::NONE, EcFont::PHAISTOS_DISC },
    // Cards OK
    { 0x1F0A0, 0x1F0FF,
            "Playing Cards", u8"Игральные карты",
            u8"Блок объединяет стандартные карты и колоду таро."
                "<p>Игру в карты придумали около XII{{_}}века в Восточной Азии. "
                    "Первые упоминания карт в Европе относят к XIV{{_}}веку. "
                    "Хотя мусульмане отрицают, что игра в карты пришла через них, "
                        "оказалось, что мамлюки даже сделали свою колоду, напоминающую таро."
                "<p>Джокер никакого отношения к козырям таро не имеет, его добавили в XIX{{_}}веке."
                "<p>Юникодные названия козырей соответствуют французской колоде таро (гадают обычно итальянской колодой). "
                    "А именно: 0){{_}}шут; 1){{_}}человек; 2){{_}}детство; 3){{_}}юность; 4){{_}}зрелость; 5){{_}}старость; 6){{_}}утро; "
                        "7){{_}}день; 8){{_}}вечер; 9){{_}}ночь; 10){{_}}земля и воздух; 11){{_}}вода и огонь; 12){{_}}танец; 13){{_}}покупки; "
                        "14){{_}}забавы на воздухе; 15){{_}}искусство; 16){{_}}весна; 17){{_}}лето; 18){{_}}осень; 19){{_}}зима; "
                        "20){{_}}игра; 21){{_}}коллектив."
                "<p>В блоке один [[pt:emoji|эмодзи]], чёрный джокер.",
            // Font coincides with Phaistos Disc, with one difference:
            // One emoji here → colour emoji font
            EcScript::NONE, EcFont::PLAYING_CARDS },
    { 0x1F100, 0x1F1FF,
            "Enclosed Alphanumeric Supplement",
            u8"Обрамлённые буквы и цифры дополнительные",
            u8"Японские телевизионные символы, символы из шрифта Webdings, символы интеллектуальной собственности…"
                "<p>Группы крови, знак стоянки и некоторые другие символы являются [[pt:emoji|эмодзи]]."
                "<p>Из региональных индикаторов собирают эмодзи флагов: "
                        "если написать эмодзи «GB», получится флаг Великобритании.",
            EcScript::NONE, EcFont::DINGBAT },
    // Enclosed hiero OK
    { 0x1F200, 0x1F2FF,
            "Enclosed Ideographic Supplement",
            u8"Обрамлённые иероглифы дополнительные",
            u8"Иероглифы в скобках, квадратах, кругах. "
                    "Используются в Японии в плакатах, телевидении."
                "<p>1F227…1F231, 1F240…1F248{{-}}знаки из японских бейсбольных сводок: подающий, ловец, игрок на 1-й базе… "
                    "Этот спорт в Японии популярен, на Олимпиаде 2021 в Токио у них золото по бейсболу и софтболу! "
                    "У 1F214 двойное назначение: двуязычная трансляция и игрок на 2-й базе."
                "<p>1F260…1F265{{-}}шесть символов из китайских народных верований: "
                        "удача, процветание, долголетие, счастье, любовь, изобилие.",
            EcScript::NONE, EcFont::DINGBAT },
    // Misc OK
    { 0x1F300, 0x1F5FF,
            "Miscellaneous Symbols and Pictographs",
            u8"Разные символы и пиктограммы",
            u8"В блоке содержатся [[pt:emoji|эмодзи]] (как японские, так и международные), "
                    "а также типографские узоры и символы из пользовательских интерфейсов."
                "<p>Примечательна судьба символа «пистолет» {{em|🔫}}{{-}}с подачи ''Apple'' большинство производителей "
                    "делают его игрушечным для снижения сетевых угроз." },
    // Emoticons OK
    { 0x1F600, 0x1F64F,
            "Emoticons", u8"Смайлики",
            u8"Эмоциконы или смайлики{{-}}символы и пиктограммы, изображающие эмоции. "
                    "К моменту предложения (2008) смайлики ходили в японских мобильных сетях около 9{{_}}лет. "
                    "По исследованию Консорциума 2019, из десяти самых используемых [[pt:emoji|эмодзи]] шесть{{-}}смайлики.",
            EcScript::NONE, EcFont::DINGBAT },
    // Orna dingbats OK
    { 0x1F650, 0x1F67F,
            "Ornamental Dingbats", u8"Печатные узоры",
            u8"Часть пиктографических шрифтов ''Wingdings'' и ''Webdings''.",
            EcScript::NONE, EcFont::NOTO_SYMBOL2 },
    // Transport/map OK
    { 0x1F680, 0x1F6FF,
            "Transport and Map Symbols",
            u8"Транспортные и картографические символы",
            u8"Иконки и [[pt:emoji|эмодзи]] транспортных средств, картографических символов и знаков навигации, "
                        "во многом для совместимости с кодировками японских телефонных операторов.",
            EcScript::NONE, EcFont::DINGBAT },
    // Alchem OK
    { 0x1F700, 0x1F77F,
            "Alchemical Symbols", u8"Алхимические символы",
            u8"Основоположником химии считается Антуан Лавуазье (конец XVIII{{_}}века), закрывший, например, понятие «флогистон». "
                    "А до этого были очень разные теории, почему и как вещества взаимодействуют, подчас дикие. "
                    "Отдельные алхимики даже предполагали, что наука сможет сделать золото или получить искусственную жизнь. "
                    "У них были бессистемные, но понятные любому знакомому с латынью и древними учениями знаки "
                        "для разных веществ и методов обработки." },
    // Geo shapes ex OK
    { 0x1F780, 0x1F7FF,
            "Geometric Shapes Extended",
            u8"Геометрические фигуры расширенные",
            u8"Блок появился в попытке перенести в Юникод пиктографические шрифты ''Wingdings'' и ''Webdings''. "
                    "К тому же он позволил поставить стрелку или крест именно той толщины, что надо. "
                "<p>Позднее добавились символы для игры го, знаки цветов{{-}}даже если нет [[pt:ligature|лигатуры]] "
                    "жёлтого бантика, пара {{em|🟨🎀}} будет понятной." },
    // Arrows C OK
    { 0x1F800, 0x1F8FF,
            "Supplemental Arrows-C", u8"Стрелки дополнительные C",
            u8"По состоянию на Юникод{{_}}13.0{{-}}148 стрелок из шрифтов ''Wingdings'' и ''Webdings'', и две стрелки со старых компьютеров. "
                    "Юникод{{_}}14 не принёс новых стрелок." },
    // Supp sym/picto OK
    { 0x1F900, 0x1F9FF,
            "Supplemental Symbols and Pictographs",
            u8"Символы и пиктограммы дополнительные",
            u8"Появившийся в 2015 блок символов разного назначения, в основном [[pt:emoji|эмодзи]]. "
                    "Также несколько символов из Типикона{{-}}типового устава православного богослужения, "
                        "кодифицированного около 1100, последние крупные правки 1695.",
            EcScript::NONE, EcFont::DINGBAT },
    // Chess OK, turned bad pawn
    { 0x1FA00, 0x1FA6F,
            "Chess Symbols", u8"Шахматные символы",
            u8"В сказочной композиции в эпоху наборного шрифта за неимением другого использовали обычные повёрнутые фигуры. "
                    "Из нестандартных фигур устоялись:<br>"
                    "• чёрно-белая фигура, которой может ходить любая из сторон;<br>"
                    "• конеферзь{{-}}также суперферзь, ферзь всяческая, магараджа;<br>"
                    "• конеслон{{-}}также кардинал;<br>"
                    "• конеладья{{-}}также маршал, канцлер;<br>"
                    "• эквихоппер{{-}}выбирает любую фигуру (свою или чужую), прыгает через неё и приземляется на таком же расстоянии "
                        "с другой стороны. "
                        "Если он прыгает в восьми основных направлениях, все поля от старта до финиша (кроме опорной фигуры) "
                        "должны быть пусты. "
                        "Очень манёвренный, но ограничен четвертью клеток."
                "<p>Также в этом блоке есть фишки китайской игры сянци." },
    // Sym and picto ext A OK
    { 0x1FA70, 0x1FAFF,
            "Symbols and Pictographs Extended-A",
            u8"Символы и пиктограммы расширенные А",
            u8"Блок [[pt:emoji|эмодзи]], появившийся в 2019. "
                "Одежда, медицина, развлечения, разные предметы, еда…",
            EcScript::NONE, EcFont::DINGBAT },
    // Legacy OK
    { 0x1FB00, 0x1FBFF,
            "Symbols for Legacy Computing",
            u8"Символы со старых компьютеров",
            u8"Символы с разных компьютеров и систем 1970‑х и 80‑х:<br>"
                    "• Amstrad CPC{{-}}успешный британский ПК;<br>"
                    "• Mattel Aquarius{{-}}гонконго-американский ПК, устаревший ещё при появлении;<br />"
                    "• Atari 400, 800 и ST{{-}}серия успешных американских ПК;<br />"
                    "• Tandy/RadioShack TRS-80 (Mark 1 и Color){{-}}успешные американские ПК;<br>"
                    "• Oric{{-}}британский ПК, получивший определённую популярность во Франции и известный по болгарскому "
                        "клону «Правец-8D»;<br>"
                    "• Apple IIc и более поздние (кодировка ''MouseText''){{-}}сверхуспешные американские ПК;<br>"
                    "• Commodore PET и 64{{-}}сверхуспешные американские ПК;<br>"
                    "• Sinclair ZX80 и ZX81{{-}}успешные британские ПК;<br>"
                    "• MSX, он же [[pt:kuvt|КУВТ]] Ямаха{{-}}японский открытый стандарт ПК;<br>"
                    "• Acorn Archimedes ''(RISC OS)''{{-}}прогрессивный британский ПК, сметённый IBM-совместимыми;<br>"
                    "• телетекст{{-}}система передачи текста в полях гашения телевизионного сигнала;<br>"
                    "• Minitel{{-}}французская терминальная служба;<br>"
                    "• и, по случайному совпадению, «Корвет»{{-}}советский технологический/учебный ПК."
                "<p>В стандарт '''не вошли''':<br>"
                    "• два яблока Apple{{-}}из-за политики Консорциума не включать товарные знаки;<br>"
                    "• инверсная псевдографика: например, у Apple [[pt:mono|знакоместо]] 7×8, и чтобы заливка не была клетчатой, "
                        "прямой символ заливки чередуют с инверсным;<br>"
                    "• знаки, [[pt:unification|уже имеющиеся]] в разделе [[pt:pseudographics|псевдографики]]. "
                        "В правильном шрифте старая псевдографика должна полностью стыковаться с новыми символами{{-}}"
                            "правда, невозможно показать: нет шрифта, поддерживающего и старую, и новую псевдографику.",
            EcScript::NONE, EcFont::NOTO_SYMBOL2_BIGGER },
    // CJK B OK
    { 0x20000, 0x2A6DF,
            "CJK Unified Ideographs Extension B",
            u8"ККЯ иероглифы расширение B",
            u8"Редкие и исторические [[ps:Hani|китайские иероглифы]]. "
                "Эти иероглифы упаковали в стандартный шрифт «SimSunExtB» для Windows. "
                "Впоследствии в блоке нашли много ошибок, и даже восемь ошибочно продублированных иероглифов.",
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // CJK C OK
    { 0x2A700, 0x2B73F,
            "CJK Unified Ideographs Extension C",
            u8"ККЯ иероглифы расширение C",
            u8"4149 редких и старых иероглифов, добавленных в 2009. "
                    "В 2021 добавили ещё 4 штуки.",
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // CJK D OK
    { 0x2B740, 0x2B81F,
            "CJK Unified Ideographs Extension D",
            u8"ККЯ иероглифы расширение D",
            u8"222 редких и старых иероглифа, добавленных в 2010.",
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // CJK E OK
    { 0x2B820, 0x2CEAF,
            "CJK Unified Ideographs Extension E",
            u8"ККЯ иероглифы расширение E",
            u8"5762 редких и старых иероглифа, добавленных в 2015.",
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // CJK F OK
    { 0x2CEB0, 0x2EBEF,
            "CJK Unified Ideographs Extension F",
            u8"ККЯ иероглифы расширение F",
            u8"7473 иероглифа, добавленных в 2017. "
                    "Включают редкие, старые, а также больше тысячи чжуанских (один из языков Китая).",
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // CJK compat OK
    { 0x2F800, 0x2FA1F,
            "CJK Compatibility Ideographs Supplement",
            u8"ККЯ совместимые иероглифы дополнение",
            u8"Появившаяся в 2001 [[pt:unification|разунификация]] 542 иероглифов "
                        "для [[pt:convertibility|круговой совместимости]] с тайваньской кодировкой CNS{{_}}11643-1992. "
                    "Разумеется, все иероглифы этого блока традиционные.",
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    /// @todo [tofu, P3] CJK G
    { 0x30000, 0x3134F,
            "CJK Unified Ideographs Extension G",
            u8"ККЯ иероглифы расширение G",
            u8"В 2020 Юникод перешагнул в [[pt:plane|плоскость]] номер 3, выделив там 4939 редких и старых иероглифов. "
                    "Именно здесь находятся два знаменитых рекордных иероглифа:<br>"
                    "• ''бян''{{-}}китайский сорт лапши: U+30EDD упрощённый 42 черты, U+30EDE традиционный 58 черт;<br>"
                    "• ''дайто'' или ''отодо''{{-}}иероглиф из 84 черт японского изобретения ''«вид дракона в полёте»'', U+3106C."
                "<p>Хотя Эндрю Уэст китаевед, он не мог не добавить в шрифт рекордный японский знак.",
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // Tags OK
    { 0xE0000, 0xE007F,
            "Tags", u8"Тэги",
            u8"Блок тэгов повторяет [[pt:ascii|ASCII]]. "
                    "Изначально (Unicode 3.1, 2001) тэги применялись для задания языка "
                        "по договорённости между отправителем и получателем. "
                    "Это применение полностью [[pt:deprecated|запретили]], но в Юникоде 9.0 (2017) тэги вернули "
                        "как модификаторы [[pt:emoji|эмодзи]]. "
                    "Используются только для флагов регионов, определены три региона: Англия, Уэльс, Шотландия."},
    // Var sel supp OK
    { 0xE0100, 0xE01EF,
            "Variation Selectors Supplement", u8"Селекторы начертания дополнительные",
            u8"Дополнительные селекторы начертания используются только с [[ps:Hani|китайскими иероглифами]], чтобы дать тот или иной "
                    "вариант начертания. "
                "Хотя ничего не мешает использовать их с другими символами по желанию дизайнера шрифта." },
};
static_assert(std::size(uc::blocks) == uc::N_BLOCKS);

constinit const uc::TermCat uc::termCats[] {
    { u8"Кодирование" },
    { u8"Сериализация" },
    { u8"Классификация письменностей" },
    { u8"Классификация символов" },
    { u8"Алгоритмы Юникода" },
    { u8"Общие принципы" },
    { u8"Стили письма" },
    { u8"Прочие" },
};
static_assert (std::size(uc::termCats) == static_cast<size_t>(uc::EcTermCat::NN));

constinit const uc::Term uc::terms[] {
    { "hiero", EcTermCat::SCRIPT_CLASS,
      u8"иероглифы", u8"ideographic script",
        u8"Как говорил Артемий Лебедев, пиктограмма «стакан» означает стакан. "
                "Идеограмма означает стакан, стекло, хрупкость, пьянство… "
                "То и другое{{-}}иероглифы. "
                "Так, китайский иероглиф «человек+дерево» означает «отдых»."
            "<p>Запись иероглифами коротких мыслей нас окружает повсюду{{-}}на дорожных знаках, в интерфейсах приборов и программ. "
                "Но в целом такая запись недалеко ушла от пиктограмм. "
                "Запись иероглифами речи{{-}}это два шага вперёд: "
                    "картинка может означать понятие, и упорядочены они по месту в языке, а не по отношению друг к другу. "
                "Если «охотник убил оленя копьём», то придумывают знак для убийства, и копьё рисуют последним, а не между охотником и оленем."
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
            "• Различия сглаживаются не только в пространстве, но и во времени{{-}}вековые надписи лучше отвечают современному языку.<br>"
            "• Такое письмо короче, что удобно при плохом качестве «бумаги» и перьев." },
    { "syllabic", EcTermCat::SCRIPT_CLASS,
      u8"слоговая азбука", u8"syllabic script",
        u8"Письменность, в которой каждая буква записывает слог из гласной и согласной. "
                "Обычно содержат 80–120 букв. "
                "Разновидностью слоговой письменности является [[pt:abugida|абугида]], где у сходных слогов сходные буквы."
            "<p>Слоговую письменность легко изобрести из [[pt:hiero|иероглифической]]: люди говорят не звуками, а слогами, "
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
        u8"Исследователи не сходятся в определении абугиды, и самое широкое из них такое: "
                    "[[pt:syllabic|слоговая]] азбука, где у сходных слогов сходные символы."
            "<p>Абугидами '''точно являются''' [[pt:brahmic|брахмийские письменности]]."
            "<p>Абугидами '''являются у многих исследователей''' [[ps:Ethi|эфиопская]] и [[ps:Cans|канадская слоговая]] "
                    "письменности."
            "<p>Абугидой '''является в отдельных источниках''' [[ps:Thaa|тана]]."
            "<p>Абугидами '''не являются ни в одном источнике''' [[ps:Hang|хангыль]], [[ps:Yiii|и]]."
            "<p>Что интересно, название «абугида» происходит от эфиопского {{sm|አቡጊዳ}} (а-бу-ги-да){{-}}получается, некоторые исследователи "
                    "отказывают в праве называться абугидой той письменности, откуда это понятие пошло!" },
    { "brahmic", EcTermCat::SCRIPT_CLASS,
      u8"брахмийская абугида", u8"Brahmic abugida",
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
                "• ''могут'' быть [[pt:ligature|лигатуры]] из нескольких согласных{{-}}деванагари {{sm|क्ष}} ксса. "
                    "Печатаются они, как будто лигатуры нет: ка+вирама+сса;<br>"
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
                    "алфавитом считают или не все исследователи ([[ps:Thaa|тана]], [[ps:Hang|хангыль]]), "
                    "или вообще никто ([[ps:Laoo|лаосский]])."
            "<p>При всех достоинствах алфавитов у них есть серьёзный недостаток. "
                "Из-за сдвигов в произношении появляется сложная исторически обусловленная орфография, "
                    "которую время от времени нужно реформировать. "
                "Так, [[ps:Cyrl|русский]] ять '''ѣ''' был отдельным звуком, в XVIII{{_}}веке полностью исчезнувшим. "
                "Орфография [[ps:Latn|английского]] и французского во многом историческая." },
    { "arguable", EcTermCat::SCRIPT_CLASS,
      u8"письменность спорного типа", {},
        u8"Для трёх письменностей источники не договорились, что они собой представляют."
            "<p>[[ps:Hang|Хангыль]] ''(корейский)''{{-}}[[pt:syllabic|слоговая]] ''(Юникод)'' "
                    "или [[pt:alphabet|алфавитная]] ''(Omniglot, английская Википедия)''."
            "<p>[[ps:Thaa|Тана]] ''(мальдивский)''{{-}}[[pt:abjad|консонантная]] ''(Omniglot)'', "
                    "[[pt:abugida|абугида]] ''(английская Википедия)'' или алфавитная ''(Юникод)''."
            "<p>[[ps:Wara|Варанг-кшити]]{{-}}алфавит ''(английская Википедия, Юникод)'' или абугида ''(Omniglot)''." },
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
                "• поскольку символы не вписываются в 256 [[pt:codepoint|позиций]], существовали специфичные кодировки для иероглифов." },
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
      u8"нетекстовая письменность", u8"non-text script",
        u8"Запись других видов информации, кроме текста: "
                "музыки (ноты), чертежей (технические символы), жестов ([[ps:Sgnw|жестовое письмо]])."
            "<p>Юникод работает с [[pt:plaintext|простым текстом]] и не предназначен для кодирования такой информации. "
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
                "• [[pt:ligature|лигатуры]], означающие стечение согласных{{-}}деванагари {{sm|क्ष}} ксса. "
                    "Печатаются и кодируются они, как будто лигатуры нет: ка+вирама+сса. "
                    "Чтобы разорвать лигатуру и вывести две буквы и вираму, вводится ка+вирама+ZWNJ+сса;<br>"
                "• знаки для конечного согласного{{-}}[[ps:Cham|чамский]], [[ps:Limb|лимбу]];<br>"
                "• гласная по умолчанию редуцировалась{{-}}[[ps:Guru|пенджабский]];<br>"
                "• реформа орфографии лишила букву гласной по умолчанию{{-}}[[ps:Laoo|лаосский]];<br>"
                "• вирама может быть единым целым с согласной{{-}}[[ps:Talu|новое письмо лы]];<br>"
                "• не способны записывать «лишние» согласные{{-}}[[ps:Bugi|лонтара]]."
            "<p>Большинство вирам{{-}}[[pt:combining|комбинирующие метки]]. "
                "Но встречаются и [[pt:spacing|протяжённые]] ([[ps:Java|яванский]])."
            "<p>В некоторых письменностях ([[ps:Khar|кхароштхи]], [[ps:Mtei|манипури]]) существует «виртуальная вирама» "
                    "для набора лигатур. "
                "Иногда даже ([[ps:Sund|сунданский]], [[ps:Cakm|чакма]]) в дополнение к современной вираме, убирающей гласную." },
    { "graphic", EcTermCat::CHAR_CLASS,
      u8"графический символ, печатаемый символ", u8"graphic character, printable character",
        u8"Символ, который печатается на экране или бумаге. "
                "Любой, кроме [[pt:control|управляющих]], [[pt:format|форматирующих]], разделителей строк и абзацев{{-}}"
                    "даже [[pt:space|пробел]]."
            "<p>Пробел нулевой ширины считается форматирующим, а не пробелом{{-}}а значит, он '''не''' графический." },
    { "deprecated", EcTermCat::CHAR_CLASS,
      u8"запрещённый символ", u8"deprecated character",
        u8"Символ, чьё использование настоятельно не рекомендуется."
            "<p>Одна из гарантий [[pt:stability|стабильности]] Юникода: как только символ назначен, он не{{_}}сдвинется и не{{_}}исчезнет. "
                "Если символ оказывается неудачным, его запрещают. "
                "Причины запрета разные."
            "<p>В Консорциуме Юникода сидят люди западной культуры, и они могут лишь ссылаться на исследования специалистов по редким языкам{{-}}"
                    "так случилось для пяти символов [[ps:Arab|арабского]], [[ps:Tibt|тибетского]] и [[ps:Khmr|кхмерского]]."
            "<p>Две технические угловые скобки запретили, чтобы не путали с математическими. "
                "Также запрещены языковый тэг (внёс больше неразберихи, чем решил проблем), "
                    "шесть символов [[pt:bidi|двунаправленного письма]] (предложен другой механизм) "
                    "и латинская [[pt:ligature|лигатура]] '''ŉ''' "
                        "(в голландском и африкаанс есть много похожих сокращений, никак не закодированных, "
                        "проблемы с [[pt:normalization|нормализацией]], "
                        "упразднили тип символов «буква со знаком»){{-}}итого 15 на Юникод{{_}}13.0."
            "<p>Известны несколько случаев возврата символов в обиход.<br>"
                "• Блок тэгов{{-}}использовался для указания языка. "
                    "Вернули для [[pt:emoji|эмодзи]] флагов регионов (например, Шотландии), "
                        "оставив под запретом один символ.<br>"
                "• 0340, 0341 (метки нисходящего/восходящего тона). "
                    "Предлагается замена в виде обычных умляутов à и á. "
                    "С Юникода 5.2 просто не{{_}}рекомендуются.<br>"
                "• 17D3 (кхмерский умляут ''bathamasat''). "
                    "С появлением [[pt:precomposed|монолитных]] символов лунного календаря в Юникоде 4.0 запретили. "
                    "С Юникода 5.2 просто не{{_}}рекомендуется{{-}}вероятно, из-за громоздкости календарной лигатуры. "
                    "У автора «Юникодии» были проблемы с поиском шрифта, где лигатуры хорошо видны."
            "<p>Не путать с [[pt:obsolete|устаревшим]]." },
    { "combining", EcTermCat::CHAR_CLASS,
      u8"комбинирующая метка", u8"combining mark",
        u8"Несамостоятельный символ, прикрепляемый к основному. Используется для:"
             "<p>• диакритических знаков{{-}}пометок, прикрепляемых к основному символу;<br>"
                "• огласовок в [[pt:abjad|консонантных]] письменностях и [[pt:abugida|абугидах]];<br>"
                "• разных «хаков» через [[pt:ligature|лигатуры]] в шрифтах: например, буква + селектор начертания = буква другой формы."
            "<p>Юникод делит метки на [[pt:spacing|протяжённые]], непротяжённые и охватывающие."
            "<p>В шрифтах комбинирующие метки устанавливаются одним из трёх способов, от самого качественного: "
                    "лигатура (отдельный глиф для символа с меткой), якорь (вручную установленная точка привязки к символу) "
                    "и якорь по умолчанию (метка ставится по положительным или отрицательным координатам, чтобы, будучи "
                    "нарисованной относительно курсора, встала на подходящем месте)."
            "<p>Юникод не позволяет накрыть одной меткой несколько символов, однако позволяет задействовать несколько механизмов шрифтов:<br>"
                "• у некоторых меток половина находится над символом, половина свисает наружу, "
                    "и можно накрыть этой меткой два символа: {{sm|а͡б}};<br>"
                "• первый символ накрываем начальным фрагментом, второй и{{_}}т.д.{{-}}повторяющимся, и последний{{-}}конечным. "
                    "В большинстве шрифтов этот метод реализован плохо." },
    { "ligature", EcTermCat::CHAR_CLASS,
      u8"лигатура", u8"ligature",
        u8"Лигатура{{-}}это несколько символов в одном:"
            "<p>1. В типографике{{-}}символ, составленный из нескольких. Так, амперсанд '''&''' является "
                    "лигатурой латинского ''et{{-}}и''."
            "<p>2. В шрифтовом деле{{-}}символ, означающий последовательность из нескольких [[pt:codepoint|кодовых позиций]]. "
                "Так, в некоторых шрифтах набор «fi» пропечатывает лигатуру '''ﬁ'''. "
                "Много таких лигатур в [[pt:brahmic|брахмийских абугидах]]."
            "<p>Все лигатуры{{-}}[[pt:spacing|протяжённые]] символы."
            "<p>[[pt:casefolding|Свёртка регистра]] ''может'' раскрывать лигатуры." },
    { "precomposed", EcTermCat::CHAR_CLASS,
      u8"монолитный символ", u8"precomposed character",
        u8"Символ и [[pt:combining|диакритическая метка]] в одном. "
                "Например, '''á''' собрано из буквы и метки, а '''á'''{{-}}один монолитный символ."
            "<p>Три основные причины создания монолитных символов{{-}}другая сущность, совместимость и красота."
            "<p>'''Другая сущность:''' согласитесь, Ё{{-}}это не Е с умляутом. "
                "А Ю́{{-}}это именно что Ю с ударением, потому такого монолитного символа нет."
            "<p>'''Совместимость:''' если ограниченный типографский движок не может накладывать метки, или "
                    "ограниченный клавиатурный драйвер печатает по одному символу, монолитные символы{{-}}всё, что остаётся. "
                "Часто монолитные символы берутся из существующих кодировок."
            "<p>'''Красота:''' часто механизмы наложения меток оставляют желать лучшего, особенно если дизайнер не предусмотрел "
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
            "<p>Таковыми не являются [[pt:control|управляющие]], [[pt:format|форматирующие]] символы и прочие типы меток." },
    { "pseudographics", EcTermCat::CHAR_CLASS,
      u8"псевдографика", u8"pseudographics, semigraphics, box drawing",
        u8"Символы для рисования геометрических фигур{{-}}линий, прямоугольников, рамок{{-}}в текстовом режиме. "
                "Широко применялись для оживления текстовых интерфейсов пользователя."
            "<p>В современных графических ОС бессмысленны: есть настоящая графика, а большинство шрифтов пропорциональные "
                    "и не дают состыковать линии. "
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
            "<p>Символы, чьё основное предназначение нечисловое (например, славянские цифры вроде А̄) Юникод числами не считает." },
    { "emoji", EcTermCat::CHAR_CLASS,
      u8"эмодзи", u8"emoji",
        u8"От японского {{sm|絵}} ''э{{-}}картинка'' + {{sm|文字}} ''модзи{{-}}знак, символ."
            "<p>Придуманы мобильным оператором NTT DoCoMo около 1999 для экономии места в SMS. "
                "Западные операторы начали разрабатывать свои наборы с 2004, и в 2010 кодировку DoCoMo внесли в Юникод 6.0. "
                "Многие эмодзи специфичны для Японии: кланяющийся бизнесмен {{em|🙇}}, знак начинающего водителя {{em|🔰}}… "
                "Другие изменили значение за пределами Японии: знак высшего балла {{em|💯}} превратился в «полностью согласен»."
            "<p>Все эмодзи являются [[pt:spacing|протяжёнными]] символами." },

    ///// Principles ///////////////////////////////////////////////////////////

    { "stability", EcTermCat::PRINCIPLES,
      u8"гарантии стабильности", u8"stability guarantees",
        u8"Консорциум Юникода гарантирует, что не{{_}}изменит часть характеристик символов."
            "<p>'''Символ не сдвинется и не исчезнет''' (2.0+). "
                "Каждый новый Юникод будет надмножеством предыдущего. "
                "Сортировка текста происходит через национальный порядок сортировки."
            "<p>'''Название символа не изменится''' (2.0+). "
                "Впрочем, существуют названия-исправления, и именно  '''их''' показывает «Юникодия». "
                "Также не{{_}}изменятся названия-синонимы, в том числе исправления (5.0+)."
            "<p>'''[[pt:normalization|Нормализация]] строки не изменится''' (4.1+). "
                "В версии 3.1 были более слабые гарантии, связанные с тем, что в Юникод входило всё больше [[ps:Latn|латиницы]] "
                    "и других частых письменностей, для которых нормализация имеет смысл."
            "<p>'''Свойства символа могут меняться, но не настолько, чтобы изменить его сущность''' (1.1+). "
                "Например, заглавная буква никак не{{_}}может стать строчной, а [[pt:noncharacter|зарезервированный как отсутствующий]] "
                    "не{{_}}вернётся в пользование. "
                "В дальнейших версиях Юникода это правило конкретизировалось, не{{_}}будем уточнять."
            "<p>'''Нормативные и информационные свойства символа из баз Юникода не исчезнут''' (5.2+). "
                "Остальные могут исчезать и переименовываться, но при переименовании никогда не{{_}}займут место другого."
            "<p>'''Не изменится [[pt:casefolding|свёртка регистра]]''' (5.0+). "
                "Свёртка регистра служит для локаленезависимого сравнения строк без регистра и не обязательно перевод в строчные: "
                    "для [[ps:Cher|чероки]] это перевод в заглавные, так как они появились раньше."
            "<p>'''Если два символа не были (или были) в регистровой паре, они там и останутся''' (5.0+). "
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
                "Этот порядок не совпадает с порядком отображения текста на экране, и алгоритмы [[pt:bidi|двунаправленного текста]] "
                    "призваны расставить символы в визуальном порядке."
            "<p>[[pt:combining|Комбинирующие метки]] хранятся после своих символов."
            "<p>Исключение: некоторые [[pt:brahmic|брахмийские]] абугиды ([[ps:Thai|тайский]], [[ps:Laoo|лаосский]]) "
                "традиционно хранят текст в визуальном порядке: некоторые гласные находятся перед согласной, даже если читаются после." },
    { "convertibility", EcTermCat::PRINCIPLES,
      u8"преобразуемость", u8"convertibility",
        u8"Юникод используется для обмена с другими стандартами текста{{-}}национальными, международными, "
                    "специфичными для производителя. "
                "Если варианты одного символа получают разные коды в таких стандартах, они получают разные коды и в Юникоде. "
                "Это позволяет преобразовывать из кодировки в кодировку через Юникод, не терять информацию при круговом преобразовании "
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
        u8"Поведение символа в первую очередь задаётся не названием, а сложными таблицами. "
                "Отрывки этих таблиц приведены и в «Юникодии»: [[pt:number|числовое]] значение, регистр, [[pt:bidi|направление]] письма…" },
    { "character", EcTermCat::PRINCIPLES,
      u8"символы, не глифы", u8"characters, not glyphs",
        u8"Символ{{-}}единица смысла. Глиф{{-}}изображение, содержащееся в шрифте и выводимое на экран/печать."
            "<p>Символы и глифы соотносятся не 1:1. "
                "''Настали́к'', стиль [[ps:Arab|арабского]] текста, требует тысячи глифов в шрифте. "
                "Но в кодировке всего две сотни символов, передающих ''смысл''. "
                "И наоборот, у разных символов бывает один глиф{{-}}несмотря на [[pt:unification|унификацию]], такое бывает." },
    { "universality", EcTermCat::PRINCIPLES,
      u8"универсальность", u8"universality",
        u8"Юникод разработан для людей разных языков и профессий: работающих в бизнесе, образовании, религии и науке, "
                    "для современных и исторических текстов."
            "<p>За пределами Юникода лежат:<br>"
                "• письменности, про которые мало что известно, чтобы надёжно закодировать символы;<br>"
                "• письменности, чьи пользователи не пришли к де-факто стандарту;<br>"
                "• нетекстовые (например, пиктографические) письменности." },
    { "unification", EcTermCat::PRINCIPLES,
      u8"унификация", u8"unification",
        u8"Юникод старается не дублировать символы. "
                "Знаки препинания закодированы один раз и используются во всех западных языках{{-}}а также в восточных, куда они проникли. "
                "Точно так же одни и те же арабские знаки препинания используются в персидском и урду. "
                "Буква Y в английском («вай»), французском («игрек») и немецком («ипсилон»){{-}}одна и та же кодовая позиция. "
                "Мало того, одни и те же [[ps:Hani|иероглифы]] в китайском и японском могут выглядеть по-разному."
            "<p>Есть, впрочем, несколько важных исключений. Буква A из латинского, греческого, кириллицы и чероки{{-}}это разные буквы. "
                "В Юникоде аж три буквы Ð{{-}}у всех у них разные строчные." },
    { "efficiency", EcTermCat::PRINCIPLES,
      u8"эффективность", u8"efficiency",
        u8"Юникод старается, чтобы эффективные реализации были осуществимы. "
                "В частности:"
             "<p>• В Юникоде нет переключаемых регистров. Это упрощает доступ и поиск.<br>"
                "• Основные формы [[pt:serialization|сериализации]]{{-}}[[pt:utf8|UTF-8]] и [[pt:utf16|UTF-16]]{{-}}самосинхронизирующиеся. "
                    "Это упрощает доступ и поиск.<br>"
                "• Коды символов{{-}}числа от 0 до 10FFFF₁₆. "
                    "Даже если не получается работать прямо в UTF-8/16, таблицы поиска всё равно действуют.<br>"
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
      u8"зарезервирован как отсутствующий", u8"non-character", CT_NOCHAR },
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
                "Последние две [[pt:codepoint|позиции]] каждой плоскости, *FFFE и *FFFF, "
                    "[[pt:noncharacter|зарезервированы как отсутствующие]]. "
                "Из-за ограничений [[pt:utf16|UTF-16]] в{{_}}Юникоде семнадцать плоскостей."
             "<p>• Плоскость {{№|0}} называется '''[[pt:bmp|базовой многоязычной]]''' и содержит большинство символов живых языков. "
                    "В Юникоде{{_}}14.0 остаются 16{{_}}нераспределённых позиций.<br>"
                "• Плоскость {{№|1}} называется '''дополнительной многоязычной''' и содержит символы редких и мёртвых языков, а также технические. "
                    "Заполнена менее чем на {{%|50}}.<br>"
                "• Плоскость {{№|2}} называется '''дополнительной иероглифической''' и содержит только [[ps:Hani|китайские иероглифы]]. "
                    "Заполнена на {{%|90}}.<br>"
                "• Плоскость {{№|3}} называется '''третичной иероглифической''', содержит только китайские иероглифы, "
                    "и заполнена на {{%|10}}.<br>"
                "• Плоскости 4…0D в Юникоде{{_}}14.0 не задействованы.<br>"
                "• Плоскость 0E называется '''дополнительной плоскостью специального назначения''' и содержит менее 500 специсмволов.<br>"
                "• Плоскости 0F и 10 отведены под [[pt:private|личное пользование]]." },
    { "private", EcTermCat::ENCODING,
      u8"символ для личного пользования", u8"private-use character",
        u8"На собственные нужды всех заинтересованных отведены 6400 символов E000…F8FF из [[pt:bmp|базовой плоскости]], "
                    "а также 131068 символов в двух [[pt:plane|плоскостях]] 0F и 10, кроме *FFFE и *FFFF, "
                    "[[pt:noncharacter|зарезервированных как отсутствующие]]."
            "<p>Договорившись, люди могут закодировать в этих диапазонах всё, что хотят, в том числе символы, не подлежащие "
                    "внесению в Юникод: товарные знаки, вымышленные алфавиты, редкие символы и письменности, "
                    "тэги оформления, знаки подстановки. "
                "Консорциум Юникода никак не отвечает за это. "
                "Приведём лишь четыре распространённых применения личных символов:"
             "<p>• Производители ОС часто кодируют там свои эмблемы.<br>"
                "• Реестр искусственных письменностей (Under-ConScript Unicode Registry){{-}}более 60 "
                        "искусственных языков, письменностей из кино, литературы и игр. "
                    "Три из них{{-}}Фестский диск, [[ps:Shaw|алфавит Шоу]], [[ps:Dsrt|мормонский]]{{-}}проникли в Юникод.<br>"
                "• Standard Music Font Layout (SMuFL){{-}}открытый стандарт музыкальных шрифтов для сокращения нотных PDF.<br>"
                "• Заявки в Юникод{{-}}блок ещё не отдан, а шрифт-реализацию делать уже надо."
            "<p>Неофициально в личное пользование под управляющие и форматирующие символы берут [[pt:control|управляющие]]: "
                    "в большинстве случаев они не могут находиться в тексте. "
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
                "То есть вторая нормализация ничего не сделает." },
    { "casefolding", EcTermCat::ALGORITHM,
      u8"свёртка регистра", u8"case folding",
        u8"Преобразование строки, используемое для (в основном) локаленезависимого сравнения строк без регистра. "
                "Бывает двух основных типов и одного дополнительного:<br>"
                "• Простая (без изменения длины): <big>ᾨ → ᾠ</big><br>"
                "• Полная (с раскрытием [[pt:ligature|лигатур]]): <big>ᾨ → ὠ ι</big><br>"
                "• Тюркская (специальные правила для турецкого и азербайджанского): <big>I → ı</big>"
            "<p>Обычно свёртка{{-}}это перевод в нижний регистр. "
                "Но для [[ps:Cher|чероки]] это перевод в верхний: заглавные буквы появились раньше."
            "<p>Свёртка идемпотентна: CF(CF(s))=CF(s), то есть вторая свёртка ничего не сделает." },

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
                    "{{_}}{{_}}München = Mnchen-3ya"
            "<p>Если строка состоит из одних ASCII-символов, в конце просто добавляется дефис:<br>"
                    "{{_}}{{_}}test{{_}}={{_}}test-"
            "<p>Если ASCII-символов вообще нет, дефис в начале опускается:<br>"
                    "{{_}}{{_}}правда{{_}}={{_}}80aafi6cg"
            "<p>Используется только в системе доменных имён Интернета, при этом к международным именам добавляется «xn--»:<br>"
                    "{{_}}{{_}}münchen.example.com = xn--mnchen-3ya.example.com" },
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
            "<p>Формат UTF-7 не обладает хорошими свойствами [[pt:utf8|UTF-8]] и даже не одозначен: "
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
                    "С ограничениями, которые накладывает на Юникод [[pt:utf16|UTF-16]], не бывает последовательностей длиннее 4.<br>"
                "• Существует нестандартный формат UTF-8, где нулевой символ кодируется как C0{{_}}80, "
                    "отличая его от символа конца строки 00.<br>"
                "• Обратная совместимость: латинский текст неотличим от ASCII-текста. "
                    "Байт 7F [[pt:control|управляющий]], байта FF не бывает, что даёт совместимость с терминальными протоколами.<br>"
                "• Самосинхронизирующийся: если добавить любые ошибочные байты, сохранившийся текст останется читаемым. "
                    "Поиск в корректном UTF-8 программой, не понимающей UTF-8, также не даст ложных срабатываний.<br>"
                "• Надёжное опознание: осмысленный текст в однобайтовой кодировке (если он не чистый ASCII) или UTF-16 "
                    "невозможно спутать с UTF-8.<br>"
                "• Монотонный: программа, не понимающая UTF-8, отсортирует тексты в порядке кодов."
            "<p>Как формат межпрограммного обмена используется широко, в том числе в Интернете. "
                "[[pt:bom|Метка порядка байтов]] EF{{_}}BB{{_}}BF. "
                "Как формат программного интерфейса используется в Linux, кроссплатформенных библиотеках вроде OpenSSL." },
    { "utf16", EcTermCat::SERIALIZATION,
      u8"UTF-16", {},
        u8"'''Unicode transformation format, 16-bit'''{{-}}формат [[pt:serialization|сериализации]] Юникода в цепочку двухбайтовых "
                    "машинных слов."
            "<p>Изначально Юникод задумывался как двухбайтовая кодировка, и запись текста двухбайтовыми символами называлась UCS-2. "
                "Прогрессивные языки и библиотеки тех времён, вроде Java и Windows NT, быстро приняли двухбайтовый символ, "
                    "это упрощало обработку текста. "
                "Но вскоре стало понятно, что двух байтов не хватит, и в 1996, как компромисс, сделали кодировку с переменным "
                    "числом двухбайтовых слов, которую и назвали UTF-16.<br>"
                "• Символы 0…D7FF и E000…FFFF записываются как есть;<br>"
                "• Символы D800…DFFF запрещены;<br>"
                "• Символы от 10000 записываются ''суррогатной парой'':<br>"
                "{{_}}{{_}}{{_}}1101.10yy.yyyy.yyyy{{_}}1101.11yy.yyyy.yyyy, где y=x−10000₁₆."
            "<p>Свойства UTF-16:<br>"
                "• Накладывает ограничения на ёмкость Юникода{{-}}2<big>²⁰</big>+2<big>¹⁶</big>−2<big>¹¹</big>=1.112.064 символа.<br>"
                "• Обратная совместимость: текст в [[pt:bmp|базовой плоскости]] без пользовательских символов неотличим от UCS-2. "
                    "Если рабочий язык лежит в базовой плоскости, а символы за её пределами не обрабатываются (остаются или удаляются), "
                    "двухбайтовый символ вполне пригоден для обработки текста.<br>"
                "• Самосинхронизирующийся: если добавить любые ошибочные '''слова''', сохранившийся текст останется читаемым. "
                    "Поиск в UTF-16 программой, понимающей UCS-2, также не даст ложных срабатываний. "
                    "Для байтов и однобайтовых кодировок это неверно;<br>"
                "• Ненадёжное опознание: без [[pt:bom|метки порядка байтов]] формат UTF-16 и порядок байтов определяется "
                    "по косвенным признакам;<br>"
                "• Немонотонный: программа, понимающая UCS-2, отсортирует тексты не в порядке кодов."
            "<p>Как формат межпрограммного обмена используется в основном в двоичных файлах родом с Windows и Mac/Motorola "
                    "(например, шрифтах TrueType). "
                "Таким файлам метка порядка байтов не нужна; в текстовых файлах она FE{{_}}FF для UTF-16BE и FF{{_}}FE для UTF-16LE. "
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
            "<p>Как формат межпрограммного обмена практически не используется из-за громоздкости. "
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
            "<p>[[pt:utf7|UTF-7]] и [[pt:punycode|Punycode]] не встречаются как формат текстовых файлов. "
                "UTF-32 встречается крайне редко."
            "<p>В двоичных файлах формат сериализации жёстко зафиксирован, и метка порядка байтов не нужна." },

    ///// Writing styles ///////////////////////////////////////////////////////

    { "vyaz", EcTermCat::WRITING_STYLE,
      u8"вязь", {},
        u8"Нелинейный стиль письма, используемый для красоты и экономии материала: "
                    "текст превращается в узор, покрывающий большую площадь и читающийся по ломаной линии. "
                "Распространён в [[ps:Cyrl|кириллице]] (надписи на иконах), [[ps:Arab|арабице]] "
                    "(узоры-арабески, террористические флаги)."
            "<p>Вязь{{-}}не [[pt:plaintext|простой текст]] и потому не покрывается Юникодом."
            "<p>Также не совсем корректно вязью называют арабское или [[ps:Syrc|сирийское]] письмо одним росчерком." },
    { "cursive", EcTermCat::WRITING_STYLE,
      u8"курсив, скоропись, беглое письмо", u8"cursive",
        u8"Стиль письма, когда нужно быстро написать что-то, особенно хрупким гусиным пером. "
                "Особенности варьируются от языка к языку, но в целом:<br>"
                "• Простая форма букв.<br>"
                "• Углы, кроме очень острых, скругляются.<br>"
                "• Пишущий старается не поднимать перо{{-}}отсюда соединённые буквы, проход по одному штриху туда-сюда, "
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
            "<p>Если у письменности в принципе большие выносные элементы ([[ps:Lana|ланна]]), минускулом это называть не принято." },


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
            "<p>Юникод записывает буквы в порядке, близком к алфавитному, однако никак не кодифицирует правила алфавитной сортировки. "
                "Если алфавитный порядок изменится, страна сама меняет правила сортировки, не советуясь с Юникодом." },
    { "mono", EcTermCat::OTHER,
      u8"знакоместо", u8"character cell",
        u8"Воображаемая клетка, на которые разбивается моноширинный текст. "
                "<p>Текст на «знакоместа» делится и в [[pt:cjk|ККЯ]], но в первую очередь термин относится "
                    "к пишущим машинкам, текстовым режимам компьютеров, алфавитно-цифровым принтерам." },
    { "kuvt", EcTermCat::OTHER,
      u8"КУВТ = комплекс учебной вычислительной техники", {},
        u8"Советское название для стандартной аппаратуры компьютерного класса."
            "<p>КУВТ состоял из централизованной аппаратуры питания, учительской машины с дисководом и принтером, "
                    "до 20 ученических машин и компьютерной сети, соединяющей всё это. "
                "Иногда встречался телевизор, дублировавший учительский монитор."
            "<p>Основные КУВТы{{-}}ДВК, Агат (клон Apple II), Ямаха (русифицированные японские машины стандарта MSX), "
                    "КУВТ-86/87 (БК-0010Ш), Корвет, УКНЦ, Поиск (IBM-совместимый). "
                "Наиболее распространённый{{-}}УКНЦ (22{{_}}тыс. комплектов)."
            "<p>КУВТ умел передавать по сети на ученические машины программы и игры. "
                "Но сохранить или распечатать работу ученика на учительской машине ''обычно'' не{{_}}мог; "
                    "учителя сажали во внеурочное время самых способных за основной компьютер. "
                "Сетевой игры ни на одной из этих платформ не{{_}}существовало, зато многие игры позволяли играть двоим, "
                    "вместе или по одной жизни." },
};


size_t uc::nTerms() { return std::size(terms); }


std::u8string_view uc::oldCompNames[] {
    u8"Amstrad" NBSP "CPC",
    u8"Apple" NBSP "IIc+",
    u8"Mattel" NBSP "Aquaruis",
    u8"Atari" NBSP "ST",
    u8"Atari" NBSP "400/800",
    u8"Minitel",
    u8"MSX",
    u8"Oric",
    u8"Commodore" NBSP "PET/64",
    u8"RISC OS",
    u8"Sinclair" NBSP "ZX80/81",
    u8"телетекст",
    u8"Tandy" NBSP "TRS80" NBSP "Mark" NBSP "1",
    u8"Tandy" NBSP "TRS80" NBSP "Color",
    u8"Корвет",
};

#define REP_2(x) x,x
#define REP_3(x) x,x,x
#define REP_4(x) x,x,x,x
#define REP_5(x) x,x,x,x,x
#define REP_6(x) x,x,x,x,x,x
#define REP_8(x) REP_4(x),REP_4(x)
#define REP_10(x) REP_4(x),REP_4(x),x,x
#define REP_11(x) REP_4(x),REP_4(x),REP_3(x)
#define REP_12(x) REP_4(x),REP_4(x),REP_4(x)
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

}   // anon namespace


Flags<uc::OldComp> uc::cpOldComps(char32_t cp)
{
    cp -= 0x1FB00;
    if (cp >= std::size(oldCompData))
        return {};
    return oldCompData[cp];
}

void uc::completeData()
{
    const Block* hint = std::begin(blocks);
    std::fill(std::begin(cps), std::end(cps), nullptr);
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
        auto block = blockOf(cp.subj.ch32(), hint);
        if (!block->firstAllocated)
            block->firstAllocated = &cp;
        ++block->nChars;
        hint = block;
        block->ecVersion = std::min(block->ecVersion, cp.ecVersion);
        block->ecLastVersion = std::max(block->ecLastVersion, cp.ecVersion);
        // Lookup table
        cps[cp.subj.val()] = &cp;
    }

    // Check blocks — they should have at least one char
    for (auto& v : blocks) {
        if (!v.firstAllocated)
            throw std::logic_error("Block w/o chars leaked into data!");
    }
}


struct uc::LoadedFont
{
    QList<QString> families;
    QString familiesComma;
    intptr_t tempId = FONT_NOT_INSTALLED;
    std::unique_ptr<QFont> probe {};
    std::unique_ptr<QFontMetrics> probeMetrics;
};


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

}   // anon namespace


void uc::Font::load(char32_t trigger) const
{
    // Loaded?
    if (q.loaded)
        return;

    // Find in cache
    if (auto it = loadedFonts.find(family); it != loadedFonts.end()) {
        q.loaded = it->second;
        return;
    }

    // Create empty font
    auto newLoaded = std::make_shared<LoadedFont>();
    loadedFonts[family] = newLoaded;
    q.loaded = newLoaded;

    // Create/load it
    if (isFontFname(family)) {
        // FILE
        auto tempFont = installTempFontRel(family, trigger);
        newLoaded->tempId = tempFont.id;
        newLoaded->familiesComma = tempFont.families.join(',');
        newLoaded->families = std::move(tempFont.families);
    } else {
        // FAMILY
        newLoaded->familiesComma = str::toQ(family);
        newLoaded->families = toQList(family);
    }

    // Make probe font
    get(newLoaded->probe, FontPlace::PROBE, 50, trigger);
        // force EXACT match
    newLoaded->probe->setStyleStrategy(STRATEGY_TOFU);
    newLoaded->probeMetrics = std::make_unique<QFontMetrics>(*newLoaded->probe);
}


const QString& uc::Font::onlyFamily(char32_t trigger) const
{
    load(trigger);
    return (q.loaded->families.size() == 1) ? q.loaded->families[0] : qempty;
}


const QString& uc::Font::familiesComma(char32_t trigger) const
{
    load(trigger);
    return q.loaded->familiesComma;
}


bool uc::Font::doesSupportChar(char32_t subj) const
{
    load(subj);
    return q.loaded->probeMetrics->inFontUcs4(subj);
}

int uc::Font::computeSize(FontPlace place, int size) const
{
    auto r = sizeAdjust.apply(size);
    switch (place) {
    case FontPlace::CELL:
        if (flags.have(Ffg::CELL_SMALLER))
            r = r * 9 / 10;
        break;
    case FontPlace::SAMPLE:
    case FontPlace::PROBE: break;
    }
    return r;
}

const QFont& uc::Font::get(
        std::unique_ptr<QFont>& font,
        FontPlace place,
        int size, char32_t trigger) const
{
    if (!font) {
        //load();  onlyFamily WILL load
        auto& family1 = onlyFamily(trigger);
        font.reset(new QFont(family1, computeSize(place, size), QFont::Normal));
        if (family1.isEmpty())
            font->setFamilies(q.loaded->families);
        // Weight
        if (flags.have(Ffg::BOLD)) {
            font->setBold(true);
        } else if (flags.have(Ffg::SEMIBOLD)) {
            font->setWeight(QFont::DemiBold);
        } else if (flags.have(Ffg::LIGHT)) {
            font->setWeight(QFont::Light);
        }
        font->setStyleStrategy(STRATEGY_DEFAULT);
    }
    return *font;
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


uc::SampleProxy uc::Cp::sampleProxy(const Block*& hint) const
{
    if (drawMethod() != DrawMethod::SAMPLE)
        return {};

    auto& fn = font(hint);
    auto style = fn.styleSheet;
    switch (ecCategory) {
    case EcCategory::MARK_ENCLOSING:
    case EcCategory::MARK_NONSPACING:
        if (fn.flags.have(Ffg::STUB_VICEVERSA)) {
            return { ZWSP + str::toQ(subj.ch32()) + STUB_CIRCLE, style };
        }
        [[fallthrough]];
    case EcCategory::MARK_SPACING:
        // Stub off?
        if (fn.flags.have(Ffg::STUB_OFF))
            break;
        return { STUB_CIRCLE + str::toQ(subj.ch32()), style };
    case EcCategory::SEPARATOR_SPACE:
        if (isTrueSpace()) {
            return { QChar(L'▕') + str::toQ(subj.ch32()) + QChar(L'▏'), style };
        }
        break;
    default: ;
    }
    return { str::toQ(subj.ch32()), style };
}


bool uc::Cp::isGraphical() const
{
    // These tags are also non-graphical
    switch (subj.ch32()) {
    case 0x200E:    // LTR mark
    case 0x200F:    // RTL mark
        return true;
    default:;
    }
    return bidiClass().isGraphical;
}


uc::DrawMethod uc::Cp::drawMethod() const
{
    if (isAbbreviated())
        return uc::DrawMethod::ABBREVIATION;
    if (isTrueSpace())
        return uc::DrawMethod::SPACE;
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


uc::EcScript uc::Cp::ecScriptEx(const Block*& hint) const
{
    if (ecScript != EcScript::NONE && ecScript != EcScript::Zinh)
        return ecScript;
    hint = blockOf(subj, hint);
    return hint->ecScript;
}


const uc::Font& uc::Cp::firstFont(const Block*& hint) const
{
    // Priority: block → script — block’s script
    hint = blockOf(subj, hint);
    // Block
    if (hint->flags.have(Bfg::UNGLITCH_MARKS) && category().upCat == UpCategory::MARK) {
        return fontInfo[static_cast<int>(EcFont::NOTO)];
    }
    auto hfont = hint->ecFont;
    if (hfont != EcFont::NORMAL || hint->flags.have(Bfg::FORCE_FONT)) {
        return fontInfo[static_cast<int>(hfont)];
    }
    // Script
    if (ecScript != EcScript::NONE && ecScript != EcScript::Zinh) {
        auto& si = scriptInfo[static_cast<int>(ecScript)];
        if (si.ecFont != EcFont::NORMAL)
            return si.font();
    }
    // Block’s script
    return hint->script().font();
}


const uc::Font& uc::Cp::font(const Block*& hint) const
{
    auto v = &firstFont(hint);
    bool isAlternate = flags.have(Cfg::ALT_FONT);
    auto sb = subj.uval();
    while (v->flags.have(Ffg::FALL_TO_NEXT)) {
        if (isAlternate || !v->flags.have(Ffg::ALTERNATE)) {
            if (v->doesSupportChar(sb))
                break;
        }
        ++v;
    }
    return *v;
}


uc::TofuState uc::Cp::tofuState(const Block*& hint) const
{
    if (drawMethod() > uc::DrawMethod::LAST_FONT)
        return TofuState::NO_FONT;

    auto v = &firstFont(hint);
    bool isAlternate = flags.have(Cfg::ALT_FONT);
    auto sb = subj.uval();
    while (v->flags.have(Ffg::FALL_TO_NEXT)) {
        if (isAlternate || !v->flags.have(Ffg::ALTERNATE)) {
            if (v->doesSupportChar(sb))
                return TofuState::PRESENT;
        }
        ++v;
    }
    return v->doesSupportChar(subj)
            ? TofuState::PRESENT : TofuState::TOFU;
}

namespace {

    bool isLessCB(char32_t x, const uc::Block& y)
        { return (x < y.startingCp); }

}   // anon namespace


const uc::Block* uc::blockOf(char32_t subj, const Block* hint)
{
    auto it = hintedUpperBound(
                std::begin(blocks), std::end(blocks), subj, isLessCB,
                hint + 1);
    if (it != std::begin(blocks))
        --it;
    return it;
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


const uc::Term* uc::findTerm(std::string_view id)
{
    for (auto& v : terms) {
        if (v.key == id)
            return &v;
    }
    return nullptr;
}
