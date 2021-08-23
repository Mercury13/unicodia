#include "UcData.h"

// Qt
#include <QFontDatabase>

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

constexpr QChar STUB_CIRCLE(0x25CC);
constexpr QChar ZWSP(0x200B);

constinit const uc::Font uc::fontInfo[] {
    /// @todo [tofu] 10E60, that’s Arabic too
    /// @todo [semi-tofu] Qt’s font matching algorithm is extremely poor!
    ///       If we’ve got circle in Cambria and umlaut in Noto → complete random!
    ///       e.g. 1DE0
    { FAM_DEFAULT "," FAM_BACKUP ",Segoe UI Emoji,Noto Sans Math,Noto Sans Symbols,"
            "Noto Sans Symbols2,Segoe UI Historic", {} },                       // Normal
    { "Noto Serif",                 {} },                                       // Noto
    { "Segoe UI Emoji,Noto Sans Symbols,Noto Sans Symbols2", {}, {}, {}, 120_pc },  // Noto symbol
    { "Noto Sans Symbols2",         {} },                                       // Noto symbol2
    { "Noto Sans Symbols2",         {}, Ffg::DESC_BIGGER },                     // Noto symbol2 bigger
    { "Segoe UI Symbol",            {} },                                       // Symbol
    { "Segoe UI Historic",          {} },                                       // Historic
    { "Lucida Sans Unicode",        {} },                                       // Block
    { FAM_EMOJI "," FAM_DEFAULT ",Arial," FAM_BACKUP, {} },                     // Punctuation
    { "Noto Sans Math",             {} },                                       // Math
    { "Noto Music",                 "NotoMusic-Regular.ttf", {}, {}, 110_pc },  // Music
        //-----
    { "Noto Sans Adlam",            "NotoSansAdlam-Regular.ttf" },              // Adlam
    { "Noto Serif Ahom",            "NotoSerifAhom-Regular.ttf" },              // Ahom
    /// @todo [font] Arabic has tall math operators ≈1EE50, what to do?
    { "Noto Naskh Arabic",          "NotoNaskhArabic-Regular.ttf" },            // Arabic
    { FAM_DEFAULT ",Sylfaen",    {} },                                       // Armenian
    { "Noto Sans Balinese",         "NotoSansBalinese-Regular.ttf", {}, "padding-bottom: 12%;", 90_pc }, // Balinese
    /// @todo [future] Stub is vice-versa because of missing circle
    { "Noto Sans Bamum",            "NotoSansBamum-Regular.ttf", Ffg::STUB_VICEVERSA, {}, 110_pc }, // Bamum
    { "Noto Sans Batak",            "NotoSansBatak-Regular.ttf" },              // Batak
    { "Noto Serif Bengali",         "NotoSerifBengali-Regular.ttf", {}, {}, 120_pc }, // Bengali
    { "Noto Sans Buginese",         "NotoSansBuginese-Regular.ttf", Ffg::STUB_ON }, // Buginese
    { "Noto Sans Buhid",            "NotoSansBuhid-Regular.ttf", Ffg::STUB_ON }, // Buhid
    { "Noto Sans Canadian Aboriginal", "NotoSansCanadianAboriginal-Regular.ttf" }, // Canadian aboriginal
    { "Noto Sans Cham",             "NotoSansCham-Regular.ttf" },               // Cham
    { "Noto Sans Cherokee",         "NotoSansCherokee-Regular.ttf" },           // Cherokee
        // CJK chars are square, and there’s always not enough detail → bigger
    { "Noto Serif CJK SC",          "NotoSerifCJK-Regular.ttc",                 // CJK
                Ffg::DESC_STD | Ffg::STUB_OFF, {}, 120_pc },
    { "Noto Sans Coptic",           "NotoSansCoptic-Regular.ttf" },             // Coptic
    { "Noto Serif Devanagari",      "NotoSerifDevanagari-Regular.ttf", {}, {}, 110_pc }, // Devanagari
    { "Noto Serif Dogra",           "NotoSerifDogra-Regular.ttf",               // Dogra
                Ffg::STUB_ON | Ffg::DESC_BIGGER },
    { "Noto Sans EgyptHiero",       "NotoSansEgyptianHieroglyphs-Regular.ttf"}, // Egyptian
    { "Noto Serif Ethiopic",        "NotoSerifEthiopic-Regular.ttf" },          // Ethiopic
    { "Noto Serif Georgian",        "NotoSerifGeorgian-Regular.ttf" },          // Georgian
    { "Noto Sans Glagolitic",       "NotoSansGlagolitic-Regular.ttf" },         // Glagolitic
    { "Noto Sans Gurmukhi UI",      "NotoSansGurmukhiUI-Regular.ttf", {}, {}, 110_pc },  // Gurmukhi
    { "Noto Sans Hanunoo",          "NotoSansHanunoo-Regular.ttf" },            // Hanunoo
    { "Noto Serif Hebrew",          "NotoSerifHebrew-Regular.ttf" },            // Hebrew
    /// @todo [semi-tofu] Sort out hiragana and hentaigana!
    { "HanaMinA",                   "HanaMinA.ttf", Ffg::DESC_STD },            // Hentaigana
    { "Noto Sans Javanese",         "NotoSansJavanese-Regular.ttf" },           // Javanese
    { "Noto Serif Kannada",         "NotoSerifKannada-Light.ttf", Ffg::LIGHT, {}, 110_pc }, // Kannada
    { "Noto Sans Kayah Li"sv,       "NotoSansKayahLi-Regular.ttf" },            // Kayah Li
        // Initial Noto Sans Khmer loses umlauts
    { "Noto Sans Khmer UI"sv,       "NotoSansKhmerUI-Regular.ttf", {}, {}, 110_pc }, // Khmer
    { "Noto Sans Tai Tham"sv,       "NotoSansTaiTham-Regular.ttf", Ffg::STUB_ON },    // Lanna
    { "SengBuhan",                  "sengbuhan.ttf", Ffg::STUB_ON, "padding-top: 12%;" },   // Lao
    { "Noto Sans Lepcha",           "NotoSansLepcha-Regular.ttf" },             // Lepcha
    { "Noto Sans Limbu",            "NotoSansLimbu-Regular.ttf" },              // Limbu
    { "Noto Sans Linear B",         "NotoSansLinearB-Regular.ttf" },            // Linear B
    { "Noto Sans Lisu",             "NotoSansLisu-Regular.ttf" },               // Lisu
    { "Noto Sans Malayalam",        "NotoSansMalayalam-Light.ttf", Ffg::LIGHT }, // Malayalam
    { "Noto Sans Mandaic",          "NotoSansMandaic-Regular.ttf" },            // Mandaic
    { "Noto Sans MeeteiMayek",      "NotoSansMeeteiMayek-Regular.ttf" },        // Meetei Mayek
    { "Noto Sans Mongolian",        "NotoSansMongolian-Regular.ttf" },          // Mongol
    { "Noto Sans Myanmar",          "NotoSansMyanmar-Regular.ttf" },            // Myanmar
    { "Noto Sans NKo",              "NotoSansNKo-Regular.ttf" },                // N’Ko
    { "Noto Sans Nushu",            "NotoSansNushu-Regular.ttf" },              // Nushu
    { "Noto Sans Ogham",            "NotoSansOgham-Regular.ttf", {}, {}, 110_pc }, // Ogham
    { "Noto Sans Ol Chiki",         "NotoSansOlChiki-Regular.ttf" },            // Ol Chiki
    { "Noto Sans Old Italic",       "NotoSansOldItalic-Regular.ttf" },          // Old Italic
    { "Microsoft PhagsPa",          {} },                                       // Phags-Pa
    { "Noto Sans Symbols2",         {}, {}, {}, 150_pc },                       // Phaistos disc
    { "Noto Sans Rejang",           "NotoSansRejang-Regular.ttf", Ffg::DESC_BIGGER }, // Rejang
    { "Noto Sans Runic",            "NotoSansRunic-Regular.ttf" },              // Runic
    { "Noto Sans Samaritan",        "NotoSansSamaritan-Regular.ttf" },          // Samaritan
    { "Noto Sans Saurashtra",       "NotoSansSaurashtra-Regular.ttf", Ffg::STUB_ON }, // Saurashtra
    { "Noto Sans Sinhala",          "NotoSansSinhala-Light.ttf", Ffg::LIGHT },  // Sinhala
    { "Noto Sans Sundanese",        "NotoSansSundanese-Regular.ttf", Ffg::STUB_ON }, // Sundanese
    { "Noto Sans Syloti Nagri",     "NotoSansSylotiNagri-Regular.ttf", Ffg::STUB_ON }, // Syloti Nagri
    { "Serto Antioch Bible",        "SertoAntochBible_2020_Release.ttf", {}, {}, 120_pc }, // Syriac
    { "Noto Sans Tagalog",          "NotoSansTagalog-Regular.ttf", Ffg::STUB_ON | Ffg::DESC_BIGGER }, // Tagalog
    { "Noto Sans Tagbanwa"sv,       "NotoSansTagbanwa-Regular.ttf" },
    { "Microsoft Tai Le",           {} },                                       // Tai Le
    { "Microsoft New Tai Lue",      {} },                                       // Tai Lue (new)
    { "Noto Sans Tai Viet",         "NotoSansTaiViet-Regular.ttf" },            // Tai Viet
    { "Nirmala UI,Latha",           {} },                                       // Tamil
    { "Noto Sans Telugu",           "NotoSansTelugu-Light.ttf", Ffg::LIGHT },   // Telugu
    { "MV Boli",                    {}, {}, {}, 110_pc },                       // Thaana
    { "Leelawadee,Leelawadee UI",   {}, Ffg::STUB_ON, "padding-bottom:10%;", 110_pc },  // Thai
        // Tibetan chars are VERY high → 90%
    { "Noto Serif Tibetan",         "NotoSerifTibetan-Regular.ttf", {}, {}, 90_pc }, // Tibetan
    { "Noto Sans Tifinagh",         "NotoSansTifinagh-Regular.ttf" },           // Tifinagh
    { "Ebrima",                     {} },                                       // Vai
    { "Microsoft Yi Baiti",         {}, {}, {}, 120_pc },                       // Yi
};

static_assert (std::size(uc::fontInfo) == static_cast<size_t>(uc::EcFont::NN));

constinit const uc::LangLife uc::langLifeInfo[static_cast<int>(EcLangLife::NN)] {
    { {} },
    { u8"живая <i>(основным языкам ничего не угрожает, как кириллица)</i>"sv },
    { u8"в опасности <i>(задавлена соседними языками, как чероки, или социальными потрясениями, как сирийская, "
        "или соседними письменностями, как ланна)</i>"sv },
    { u8"мёртвая <i>(скоро потеряет/недавно потеряла носителей, как нюй-шу)</i>"sv },
    { u8"историческая <i>(давно мертва, как глаголица)</i>"sv },
    { u8"новая <i>(появилась в новейшее время, как нко)</i>"sv },
    { u8"возрождённая <i>(как ахом)</i>"sv },
    { u8"экспериментальная <i>(попытка придумать имеющемуся языку новую письменность, как мормонская)</i>"sv },
    //{ u8"Ошибка"sv },
};


constinit const uc::WritingDir uc::writingDirInfo[static_cast<int>(EcWritingDir::NN)] {
    { {} },
    { u8"→"sv },
    { u8"←"sv },
    { u8"→ <i>(исторически ←↓↓ по столбцам)</i>"sv },
    { u8"→, ↑"sv },
    { u8"↓↓→ по столбцам"sv },
    { u8"писали и ←, и →"sv },
    //{ u8"Ошибка"sv },
};


constinit const uc::ScriptType uc::scriptTypeInfo[static_cast<int>(EcScriptType::NN)] {
    { {} },
    { u8"неизвестно"sv },
    { u8"[[pt:alphabet|алфавитная]]{{-}}''как кириллица''"sv },
    { u8"[[pt:arguable|спорный]]{{-}}''как тана''"sv },
    { u8"[[pt:abjad|консонантная]]{{-}}''как иврит''"sv },
    { u8"[[pt:syllabic|слоговая]]{{-}}''как японские катакана/хирагана''"sv },
    { u8"алфавитно-слоговая <i>(как иберийский)</i>"sv },
    { u8"[[pt:abugida|абугида]]{{-}}''как эфиопский''"sv },
    { u8"[[pt:brahmic|брахмийская абугида]]{{-}}''как деванагари''"sv },
    { u8"иероглифическая <i>(символ означает понятие, как китайский)</i>"sv },
    { u8"слогоиероглифическая <i>(как линейное письмо Б)</i>"sv },
    { u8"консонантно-иероглифическое <i>(как египетские иероглифы)</i>"sv },
    { u8"код <i>(записывает информацию в другой форме, как шрифт Брайля)</i>"sv },
    { u8"символы <i>(как ноты)</i>"sv },
    //{ u8"настольная игра <i>(как игральные карты)</i>"sv },
    //{ u8"эмодзи <i>(японские SMS-картинки)</i>"sv },
    //{ u8"Ошибка"sv },
};


const uc::Version uc::versionInfo[static_cast<int>(uc::EcVersion::NN)] {
    //{ "1.0"sv,  1991 },
    //{ "1.0.1"sv,1992 },
    { "1.1"sv,  1993 },
    { "2.0"sv,  1996 },
    { "2.1"sv,  1998 },
    { "3.0"sv,  1999 },
    { "3.1"sv,  2001 },
    { "3.2"sv,  2002 },
    { "4.0"sv,  2003 },
    { "4.1"sv,  2005 },
    { "5.0"sv,  2006 },
    { "5.1"sv,  2008 },
    { "5.2"sv,  2009 },
    { "6.0"sv,  2010 },
    { "6.1"sv,  2012 },
    { "6.2"sv,  2012 },
    { "6.3"sv,  2013 },
    { "7.0"sv,  2014 },
    { "8.0"sv,  2015 },
    { "9.0"sv,  2016 },
    { "10.0"sv, 2017 },
    { "11.0"sv, 2018 },
    { "12.0"sv, 2019 },
    { "12.1"sv, 2019 },
    { "13.0"sv, 2020 },
    //{ "14.0"sv, 2021 },       //check for equal number
};


constexpr auto CT_NOCHAR =
        u8"<p>Зарезервированы как отсутствующие (''non-characters'') 66 позиций: *FFFE и *FFFF во всех семнадцати плоскостях, "
                "а также FDD0…FDEF. Их никогда не будет в Юникоде."
            "<p>Эти коды '''можно''' использовать как коды ошибок, знаки подстановки, «утяжелитель» при сортировке "
                    "или что-то другое, внутреннее для программы, перекодировать в [[pt:utf8|UTF-8]] или [[pt:utf16|UTF-16]]. "
                "Если программа никак не использует данный код, она может обращаться с ним как с обычным неизвестным символом."
            "<p>Их '''нельзя''' вводить с клавиатуры, использовать в межпрограммном обмене. "
                "Благодаря тому, что символа FFFE нет в Юникоде, работает [[pt:bom|метка порядка байтов]]."sv;

constinit const std::u8string_view uc::TX_NOCHAR = CT_NOCHAR;

constexpr auto CT_CONTROL =
    u8"<p>В первые годы, когда часто требовалось повторить сессию обмена вручную (перфоратором или консолью), "
                "управляющие символы играли роль форматов данных и сетевых протоколов. "
            "Клавиша ''Ctrl'' изначально служила для ввода управляющих символов. "sv
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
         "<p>• вкл/выкл альтернативный регистр 0E/0F{{-}}включает другой цвет ленты, другой набор символов принтера;<br>"
            "• экранирование управляющего 10{{-}}говорит, что следующий символ [[pt:graphic|графический]], а не управляющий;<br>"
            "• удаление 7F{{-}}исправление ошибки на перфоленте (пробивают все семь дорожек, такой символ не подлежит считыванию). "
                "В некоторых терминалах эту же роль играл FF, из-за чего [[ps:Cyrl|русскую]] «я» заменяли заглавной "
                    "на файл-серверах, в играх жанра MUD."
        "<p>Все 65 символов из категории «Управляющие» исторические. "
            "Символы, влияющие на отображение текста, называются [[pt:format|форматирующими]]."sv;

constexpr auto CT_FORMAT =
    u8"<p>Невидимый символ, использующийся для управления различными алгоритмами обработки текста:<br>"
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
        "<p>В «Юникодии» пробелы зажаты псевдографическими символами без пустого места в кегельной площадке{{-}}"
            "так что узость обманчива. Часто для отступа хватает тончайшего пробела U+200A."
        "<p>Все пробелы являются [[pt:spacing|протяжёнными]] символами. "
            "Пробелы нулевой ширины, которые не оставляют места, отнесены не к пробелам, а к [[pt:format|форматирующим]]."sv;

constexpr auto CT_ASCII =
    u8"<p>'''American Standard Code for Information Interchange (ASCII)'''{{-}}разработанная в США в 1963 семибитная кодировка символов. "
            "Ширину в 7 бит выбрали как компромисс для передачи данных по телеграфу, "
                "к тому же ASCII легко обрезать до 6 бит, оставив символы 40…5F и 20…3F. "
            "Базовая латиница Юникода совпадает с ASCII{{-}}так что английский юникодный текст читается, даже если кодировка сбита."
        "<p>Первые 32 символа, 00…1F, [[pt:control|управляющие]], и сегодня немногие из них действительно используются. "
            "Затем идут 95 печатных символов. "
            "Многие из них с пишущих машинок: например, чтобы пропечатать À, нужно напечатать A, вернуть каретку "
                "и напечатать обратный апостроф. "
            "Последний символ, 7F=DEL, также управляющий, родом с перфолент{{-}}отверстия, пробитые во всех дорожках, "
                "говорят, что символ выбит ошибочно и не подлежит считыванию."
        "<p>Существовало множество национальных вариантов ASCII: британцы заменяли решётку # на фунт £, "
                "русские доллар $ на монету ¤, немцы [\\]{|} на умляуты ÄÖÜäöü, японцы обратную косую черту \\ на иену ¥…"
        "<p>Оставшиеся 128 символов могли заполняться чем угодно. Одна из таких 8-битных кодировок, [[pt:latin1|латиница-1]], "
                "также вошла в Юникод."sv;

constexpr auto CT_LATIN1 =
    u8"<p>Латиница-1{{-}}второе название 8-битной кодировки '''ISO{{_}}8859-1''', появившейся в 1987 году "
                "и расширяющей [[pt:ascii|ASCII]] до европейских языков."
        "<p>Первые 32 символа, 80…9F, [[pt:control|управляющие]] для совместимости с 7-битными кодировками: "
                "почтовые программы иногда теряли верхний бит, и не хотелось бы в этих позициях ставить печатные символы. "
            "Сейчас эти символы не применяются по назначению и служат [[pt:private|личными]]. "
            "Так, кодировка Windows-1252{{-}}это та же латиница-1, только вместо управляющих символов типографские. "
            "Дошло до того, что в стандартах Всемирной паутины написано: если страница просит кодировку ISO{{_}}8859-1, "
                "отобразить сайт в Windows-1252."
        "<p>Среди букв видны символы × и ÷, изначально там должен быть Œœ."sv;


constinit const uc::Category uc::categoryInfo[static_cast<int>(uc::EcCategory::NN)] {
    { UpCategory::CONTROL,      "Cc"sv,     u8"Управляющий"sv, CT_CONTROL },
    { UpCategory::FORMAT,       "Cf"sv,     u8"Форматирующий"sv, CT_FORMAT },
    // PRIVATE_USE,     -- unused as Unicodia has nothing to tell
    // SURROGATE,       -- unused as Unicodia has nothing to tell
    { UpCategory::LETTER,       "Ll"sv,     u8"Буква/строчная"sv,
            u8"Письмо «минускул», появившееся в раннее средневековье для экономии дорогого пергамента, "sv
            u8"превратилось в строчные буквы и сделало латиницу очень выразительным алфавитом." },
    { UpCategory::LETTER,       "Lm"sv,     u8"Буква/модифицирующая"sv,
            u8"Такие «недобуквы», приделывающие к букве оттенки смысла, чаще всего встречаются в фонетике. "sv
            u8"Так, модифицирующей буквой является штрих мягкости xʹ, в отличие от похожего математического штриха x′."sv },
    { UpCategory::LETTER,       "Lo"sv,     u8"Буква/другая"sv,
            u8"Символы различных письменностей, где нет деления на заглавные и строчные буквы. "sv
            u8"Например, [[ps:Hebr|иврит]], [[ps:Arab|арабская вязь]] и [[ps:Hani|китайские иероглифы]]. Также особые символы вроде "
              "мужского/женского порядкового знака романских языков (1º\{{-}}первый, 1ª{{-}}первая)."sv },
    { UpCategory::LETTER,       "Lt"sv,     u8"Буква/смешанный регистр"sv,
            u8"Символы-диграфы, состоящие из заглавной и строчной букв. "sv
            u8"Так, в хорватской латинице есть диграф ǈ. "sv
            u8"Сербский аналог Љ не является диграфом и потому смешанного регистра не имеет."sv },
    { UpCategory::LETTER,       "Lu"sv,     u8"Буква/заглавная"sv,
            u8"Те письменности, которые приняли маленькие (строчные) буквы, буквы старой формы стали "sv
            u8"называть большими, заглавными или прописными."sv },
    { UpCategory::MARK,         "Mc"sv,     u8"Метка/протяжённая"sv,
            u8"Протяжённые (обладающие шириной) комбинирующие метки встречаются в некоторых "sv
            u8"языках Юго-Восточной Азии: деванагари, бенгальском, каннаде, хангыле…"sv },
    { UpCategory::MARK,         "Me"sv,     u8"Метка/охватывающая"sv,
            u8"Охватывающие метки используются в древнерусских буквенных цифрах (А҈). "sv
            u8"Также существуют охватывающие квадрат, круг и другие фигуры. "sv
            u8"Ни один проверенный типографский движок на Windows 10 20H2 (GDI, Cairo, Skia) не поддерживает подобные символы идеально, "sv
            u8"но на хороших шрифтах вроде DejaVu результат очень неплох." },
    { UpCategory::MARK,         "Mn"sv,     u8"Метка/непротяжённая"sv,
            u8"Непротяжённые метки (например, знак ударе́ния) устроены как символы нулевой ширины, отсюда название. "sv
            u8"В хороших шрифтах дизайнер вручную помещает метки на наиболее распространённые буквы. "sv
            u8"Но если подобной комбинации не предусмотрели (8́), символы накладываются как попало, и результат обычно плох."sv },
    { UpCategory::NUMBER,       "Nd"sv,     u8"Числовой/десятичный"sv,
            u8"Люди считают десятками, поскольку у них десять пальцев. Слова «цифра» и «палец» во многих языках близки. "sv
            u8"Изобретённая в Индии позиционная система счисления используется всем миром, однако "sv
            u8"в языках Ближнего Востока и Юго-Восточной Азии цифры бывают причудливые."sv },
    { UpCategory::NUMBER,       "Nl"sv,     u8"Числовой/буквенный"sv,
            u8"Римские, китайские и другие цифры, основанные на буквах. Древнерусские также основаны на буквах, "sv
            u8"но в Юникоде для этого используются обычные А, Б, Г… с комбинирующими метками."sv },
    { UpCategory::NUMBER,       "No"sv,     u8"Числовой/другой"sv,
            u8"Архаичные системы счисления, монолитные дроби, верхние и нижние индексы, цифры в кругах, "sv
            u8"сокращения для больших чисел и другие цифровые символы."sv },
    { UpCategory::PUNCTUATION,  "Pc"sv, u8"Знак препинания/соединитель"sv,
            u8"Небольшая группа символов. Наиболее известный из них{{-}}знак подчёркивания."sv },
    { UpCategory::PUNCTUATION,  "Pd"sv,     u8"Знак препинания/черта"sv,
            u8"Дефисы и тире. Минус также является чертой, но отнесён к математическим знакам."sv },
    { UpCategory::PUNCTUATION,  "Pe"sv,     u8"Знак препинания/закрывающая скобка"sv,
            u8"Скобки отнечены к отдельной категории, потому что играют важную роль в двунаправленном алгоритме."sv },
    { UpCategory::PUNCTUATION,  "Pf"sv,     u8"Знак препинания/конечный"sv,
            u8"В основном кавычки."sv },
    { UpCategory::PUNCTUATION,  "Pi"sv,     u8"Знак препинания/начальный"sv,
            u8"В основном кавычки."sv },
    { UpCategory::PUNCTUATION,  "Po"sv,     u8"Знак препинания/другой"sv,
            u8"Точка, запятая, процент, маркер списка и многое что ещё."sv },
    { UpCategory::PUNCTUATION,  "Ps"sv,     u8"Знак препинания/открывающая скобка"sv,
            u8"Скобки отнечены к отдельной категории, потому что играют важную роль в двунаправленном алгоритме."sv },
    { UpCategory::SYMBOL,       "Sc"sv,     u8"Символ/валютный"sv,
            u8"Валютный символ{{-}}важная часть имиджа страны, и даже у монголов есть тугрик <font size='+1'>₮</font>, напоминающий могильный крест. "sv
            u8"Артемий Лебедев в конце 1990-х говорил, что рублю не нужен особый знак, "sv
            u8"но впоследствии именно его пиар сделал знак <font size='+1'>₽</font> официальным."sv },
    { UpCategory::SYMBOL,       "Sk"sv,     u8"Символ/модифицирующий"sv,
            u8"Символы вроде крышки ^, внешне совпадающие с комбинирующими метками, но лишённые особых свойств. "
                "А также некоторые другие символы.</p>"
            "<p>Многие из таких символов родом с пишущих машинок: чтобы нарисовать, например, ударение, надо было "
                "вернуть каретку на шаг назад и выбить ударение в том же знакоместе. Литера рисовалась так, чтобы один "
                "символ годился на роль апострофа и ударения.</p>", },
    { UpCategory::SYMBOL,       "Sm"sv,     u8"Символ/математический"sv,
            u8"Изначально для математики использовались несколько разных систем, наиболее известные из них{{-}}TᴇX (читается «тех») "
            "и MathType (он же ''Microsoft Equation''). Юникод даёт надежду, что в компьютерной математике появится какая-то стандартизация{{-}}"
            "а также ограниченная возможность писать формулы в системах общего назначения вроде интернет-форумов."sv },
    { UpCategory::SYMBOL,       "So"sv,     u8"Символ/другой"sv,
            u8"Юникод содержит множество разных символов, в том числе коммерческие, технические и [[pt:emoji|эмодзи]]."sv },
    { UpCategory::SEPARATOR,    "Zl"sv,     u8"Разделитель/строк"sv,
            u8"Единственный символ 2028"sv },
    { UpCategory::SEPARATOR,    "Zp"sv,     u8"Разделитель/абзацев"sv,
            u8"Единственный символ 2029"sv },
    { UpCategory::SEPARATOR,    "Zs"sv,     u8"Разделитель/пробел"sv, CT_SPACE },
    //{ u8"Ошибка"sv },     //check for equal number
};


constinit const uc::Script uc::scriptInfo[] {
    { "Zyyy"sv, QFontDatabase::Any,
        EcScriptType::NONE, EcLangLife::NOMATTER, EcWritingDir::NOMATTER, EcContinent::NONE,
            u8"Нет"sv, {}, {}, u8"Символы вне письменности."sv },
    // Adlam OK, beyond BMP, we could work somehow with umlauts, but font is REALLY small.
    { "Adlm"sv, QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::RTL, EcContinent::AFRICA,
        u8"Адлам"sv, u8"конец 1980-х"sv,
        u8"фулá <i>(семейство языков Западной Африки)</i>"sv,
        u8"<p>Фулá{{-}}кочевники, антропологически средние между европейцами и неграми, и самые ярые проповедники ислама "
                "в западной Африке.</p>"
            "<p>Алфавит придуман братьями Ибрагима и Абдулайе Барри, чтобы лучше передавать языки фулá, чем латиница или арабский. "
                "Новая письменность прижилась, и её учат в школах Гвинеи, Либерии и других стран, локализован Android.</p>"
            "<p>Алфавит назван по первым четырём буквам: A, D, L, M. "
                "Эти буквы означают «Alkule Dandayɗe Leñol Mulugol»{{-}}«алфавит, защищающий народы от исчезновения».</p>"
            "<p>До этого были ещё две неудачных попытки придумать письменность фулá, не описанных в Юникоде: "
                "фула Дита (1936, вдохновлена доарабской культурой) и фула Ба (1963). "
                "Иногда использовался волоф{{-}}сильно видоизменённый [[ps:Arab|арабский]] шрифт. "
                "Основная же письменность{{-}}[[ps:Latn|латиница]].</p>"sv,
                EcFont::ADLAM },
    /// @todo [tofu, P1] W7/10 no font at all
    { "Aghb"sv, QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::EUROPE,
        u8"Агванский"sv, u8"II век до н.э."sv,
        u8"агванский <i>(язык Кавказской Албании)</i>"sv,
        u8"<p>Считается, что создана Месропом Маштоцем, создателем армянского алфавита\u00A0— впрочем, это не так: "
                "Маштоц жил 362–440, и непонятно, какова была его роль. "
                "Упоминалась с конца XIX века. Окончательно открыта в 1937 году советскими арменоведами. "
                "Первая расшифровка вышла в 2009 году.</p>"sv },
    // Ahom OK, installed Google Noto font
    { "Ahom"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::REVIVED, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Ахом"sv, u8"XIII век"sv,
        u8"тайско-ахомский"sv,
        u8"<p>Тайцы, переселившиеся в долину реки Брахмапутра, создали письменность на основе тогдашней индийской абугиды. "
                "К XIX веку язык окончательно заместился ассамским с бенгальским письмом. Возрождается с 1954 года, "
                "в письменность добавлены десятичные цифры, не имеющие исторического обоснования.</p>"sv,
                EcFont::AHOM },
    /// @todo [tofu, BMP] Real troubles, even W10 + Noto cannot display those chars
    ///    08BE+ (2020),
    { "Arab"sv, QFontDatabase::Arabic,
        EcScriptType::CONSONANT, EcLangLife::ALIVE, EcWritingDir::RTL, EcContinent::ASIA_INDIAN,
        u8"Арабский"sv, u8"IV—VI век"sv,
        u8"арабский, персидский, урду, уйгурский, пуштунский…"sv,
        u8"<p>Письменность развилась из [[ps:Syrc|сирийской]]. "
                "Арабский язык тесно связан с исламом; на этом языке написан Коран (610–632). "
                "Арабский халифат насаждал как ислам, так и вязь. "
                "Многие исламские народы (турки, казахи, башкиры) пользовались арабицей до начала XX{{_}}века.</p>"
            "<p>Компьютерная арабица осложняется написанием арабских букв: у каждой есть обособленная, начальная, средняя и конечная форма. "
                "В обычном тексте предпочтительнее «общая» форма буквы, подстраивающаяся под положение в слове. "
                "Но если нужна конечная форма в обособленной букве, в Юникоде есть и «жёсткие» варианты.</p>"sv,
                EcFont::ARABIC },
    /// @todo [tofu, P1] No font at all
    { "Armi"sv, QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::ASIA_INDIAN,
        u8"Имперский арамейский"sv, u8"VII в. до н.э."sv,
        u8"имперский арамейский <i>(также канцелярский арамейский\u00A0— язык Персии 500—329 до н.э.)</i>",
        u8"<p>Использовался в Ахеменидской империи как книжный язык. Был кодифицирован настолько, что крайне сложно опознать "
                "время и место конкретного документа. С завоеванием Персии Александром Македонским началась "
                "фрагментация языка и дальнейшее формирование [[ps:Syrc|сирийских]] языков (предков [[ps:Arab|арабского]]) "
                "и иудейских (предков [[ps:Hebr|иврита]]).</p>"sv,
                EcFont::HISTORIC },
    /// @todo [tofu, BMP] Noto is ugly and has tofu 0560 and 0588, W7 has tofu 058D+.
    { "Armn"sv, QFontDatabase::Armenian,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Армянский"sv, u8"405"sv,
        u8"армянский"sv,
        u8"<p>Изобретён учёным и священником Месропом Маштоцем (362–440). Непонятно, брался ли какой-то алфавит за основу "
                u8"(возможно, несохранившиеся древнеармянские буквы). Алфавит тесно связан с распространением христианства в Армении. "
                u8"В XII веке добавились буквы {{sm|Օ}} и {{sm|Ֆ}}.</p>"
            u8"<p>Считается, что армянская литература богаче среднеперсидской (доисламской), потому что армянский алфавит проще "
                u8"[[ps:Syrc|манихейской]] вязи.</p>"sv,
                EcFont::ARMENIAN },
    // Balinese OK, installed Google Noto font
    { "Bali"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::PACIFIC,
        u8"Балийский"sv, u8"≈1000"sv,
        u8"балийский, сасакский"sv,
        u8"<p>Гласная по умолчанию «а». Балийская письменность не используется в общении (вместо неё латиница), "
                "но важна в индуизме.</p>"sv,
                EcFont::BALINESE },
    // Bamum OK, none even in W10, installed Google Noto + STUB_VICEVERSA.
    { "Bamu"sv, QFontDatabase::Any,
        EcScriptType::SYLLABLE, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::AFRICA,
        u8"Бамум"sv, u8"1895—1910"sv,
        u8"бамум <i>(Камерун)</i>"sv,
        u8"<p>Письменность придумал Ибрагим Нджойя, султан Бамума (запад Камеруна). Было выпущено семь версий, начиная от иероглифов "
                "и заканчивая слоговой. Правда, Нджойя не учёл, что бамум{{-}}тональный язык, потому возникло много омографов. "
                "Пришедшие в 1918 французы выгнали Нджойю и перевели язык на латиницу. В 2007 начат проект по возрождению, "
                "прогресс незначителен. Более ранние художества Нджойи можно увидеть в дополнительных плоскостях Юникода.</p>"sv,
                EcFont::BAMUM },
    // Batak OK, installed Google Noto font
    { "Batk"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::PACIFIC,
        u8"Батакская"sv, u8"≈1300"sv,
        u8"батакские <i>(Суматра)</i>"sv,
        u8"<p>Происходит от брахми и мало чем отличается от других абугид Юго-Восточной Азии. Гласная по умолчанию «а». "
                "Практически не используется, заменена латиницей.</p>"sv,
                EcFont::BATAK },
    // Bengali OK, W7 has tofu of 2009+ → installed Google Noto
    { "Beng"sv, QFontDatabase::Bengali,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Бенгальский"sv, u8"XV век"sv,
        u8"бенгальский, ассамский, санскрит <i>(Бангладеш, Индия)</i>"sv,
        u8"<p>Относится к восточной ветви индийского письма. Гласная по умолчанию{{-}}среднее между «а» и «о», "
                "в Юникоде условно обозначается «а», в других источниках «ɔ» или «ô». "
                "Чтобы заменить гласную, добавляются огласовки: {{sm|ক}}=кô, {{sm|কি}}=ки."
            "<p>[[pt:virama|Вирама]] (''хосонто'', убрать гласную по умолчанию) используется крайне редко{{-}}{{sm|নাক্ }}= нôк{{-}}"
                "потому что в бенгальском есть множество лигатур: {{sm|ক্ত}}=ктô. "
                "Из-за этого бенгальский считается одной из красивейших письменностей мира. "
                "Набор лигатур стандартный: первая буква, вирама, вторая буква."
            "<p>Бангладеш{{-}}одно из двух исламских государств с собственной письменностью "
                "(не [[ps:Arab|арабской]], не [[ps:Latn|латиницей]], не [[ps:Cyrl|кириллицей]]). "
                "Второе{{-}}[[ps:Thaa|Мальдивские острова]]."sv,
                EcFont::BENGALI },
    /// @todo [tofu, BMP] Managed to dispose of tofu in main block, but extended is here, 31BB+, 2020!
    { "Bopo"sv, QFontDatabase::Any,
        EcScriptType::ALPHASYLLABLE, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Бопомофо (чжуинь)"sv, u8"1913"sv,
        u8"китайский"sv,
        u8"<p>Бопомофо (по первым четырём буквам), или чжуинь фухао{{-}}китайская фонетическая система, придуманная У Цзиньханом в 1912–13 "
                "и принятая в 1918. После прихода к власти Мао Цзэдуна (1949) используется только на Тайване, но и там потихоньку уступает "
                "привычному пиньиню (латинской записи китайских слогов) и применяется в первую очередь для начального обучения детей, "
                "ввода в мобильный телефон, поиска иероглифов в словарях по произношению.</p>"
            "<p>В 2007 на Тайване поступила в продажу модель смартфона BlackBerry, поддерживавшая ввод только через пиньинь, "
                "она оказалась маловостребованной. В последующих версиях пообещали и чжуинь.</p>"sv,
                EcFont::CJK },
    // Braille OK, “Segoe UI Symbol”
    { "Brai"sv, QFontDatabase::Any,
        EcScriptType::CODE, EcLangLife::NOMATTER, EcWritingDir::NOMATTER, EcContinent::TECHNICAL,
        u8"Шрифт Брайля"sv, u8"1824"sv, {},
        u8"<p>Шрифт Брайля{{-}}точечный шрифт для слепых. В нём используются группы 2×3 точки с двоичным кодированием; иногда добавляется "
                "и четвёртая строка. Кодирование различается даже для британского и американского Брайля.</p>"
            "<p>Луи Брайль, сын сапожника, в возрасте 3 лет поранился ножом и потерял зрение. Брайль сумел попасть в школу для слепых "
                "Валентина Гаюи́ и пользовался там рельефно-линейным шрифтом. Другой прообраз шрифта Брайля{{-}}ночная азбука "
                "Шарля Барбье. В этой азбуке матрица имела размер 6×6 ячеек, что подходит для зрячего военного, изредка читающего "
                "ощупью, и совершенно непригодно для слепого, читающего много. Брайль сократил матрицу, воспользовавшись двоичным кодом.</p>"
            "<p>Шрифт Брайля поставил реабилитацию слепых на промышленные рельсы: легко сделать устройство для тиснения Брайлем, "
                "он не бросается в глаза, будучи выдавлен вместе с обычным текстом для зрячих или даже поверх него.</p>"sv,
                EcFont::SYMBOL },
    // Buginese OK, W10 only → installed Google Noto
    { "Bugi"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::PACIFIC,
        u8"Бугийский (лонтара)"sv, u8"XVII век"sv,
        u8"бугийский и другие языки о. Сулавеси"sv,
        u8"<p>Бугийская письменность, или лонтара{{-}}потомок брахми и работает по тому же принципу. "
                "Со времён голландской колонизации заменяется [[ps:Latn|латиницей]]. "
                "Название «лонтара» означает местную пальму, на чьих листьях и писали бугийцы."
            "<p>Гласная по умолчанию «а»: {{sm|ᨀ}}=ка, {{sm|ᨀᨚ}}=ко. "
                "Одинокие гласные устанавливаются на букву {{sm|ᨕ}}: {{sm|ᨕᨚ}}=о. "
                "[[pt:virama|Вирамы]] нет: основные стечения согласных{{-}}отдельные буквы, остальные просто не записываются. "
                "Этот дефект лонтары даже используют как загадку: правильно прочитай каламбур и найди скрытый смысл. "
                "Некоторые специалисты по старому письму пользуются вирамой."sv,
                EcFont::BUGINESE },
    // Buhid OK, installed Google Noto font
    { "Buhd"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::PACIFIC,
        u8"Бухид"sv, u8"≈1300"sv,
        u8"бухидский <i>(Филиппины)</i>"sv,
        u8"<p>Используется мангиан{{-}}небольшой филиппинской народностью (8000 на 1991\u00A0год). "
                "Восходит к брахми и родственный с тагальским.</p>"
            "<p>В языке всего 15 согласных и три гласных: «а» по умолчанию, «и» и «у» задаются огласовками. Начальные гласные{{-}}"
                "отдельные буквы. На стыках слогов могут появляться дополнительные согласные, они не записываются: "
                "{{sm|ᝐᝒᝑᝋᝓ}} означает ''sihamu'', но читается ''singhanmu''</p>",
                EcFont::BUHID },
    // Canadian syllabics OK, W10 “Gadugi”, W7 lacks 2009 extensions, installed Google Noto font
    { "Cans"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::AMERICA,
        u8"Канадская слоговая"sv, u8"1833"sv,
        u8"языки канадских аборигенов: кри, оджибве <i>(алгонкинский)</i>, наскапи, инуктитут <i>(эскимосский)</i>…"sv,
        u8"<p>Изобретена миссионером Джеймсом Эвансом в 1833 году. Огласовки заключаются в повороте буквы: "
                "ке={{sm|ᑫ}}, ки={{sm|ᑭ}}, ко={{sm|ᑯ}}, ка={{sm|ᑲ}}. "
                "Более сложные слоги (например, долгие гласные) задаются диакритикой: кии={{sm|ᑮ}}. "
                "Простая согласная{{-}}уменьшенная буква: к={{sm|ᒃ}}.</p>"sv,
                EcFont::CANADIAN_ABORIGINAL },
    { "Cham"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Чамский"sv, u8"IV век"sv,
        u8"чамский <i>(Вьетнам, Камбоджа)</i>"sv,
        u8"<p>Чамская письменность произошла из брахми, и слоги по умолчанию заканчиваются гласной «а». Чтобы добавить другую гласную, "
                "дописывается огласовка. Но у чамов исчез знак «вирама» (убрать гласную), вместо этого к слогу добавляется знак "
                "конечной согласной: {{sm|ꨆ}}=ка, {{sm|ꨆꨯ}}=ко, {{sm|ꨆꨯꩃ}}=конг.</p>"
            "<p>Большинство чамов исповедуют ислам и пишут [[ps:Arab|арабицей]], лишь некоторые чамы Вьетнама "
                "используют чамское письмо.</p>"sv,
                EcFont::CHAM },
    // Cherokee OK, installed Google Noto font. Need it, W7 has no 2014 extensions.
    { "Cher"sv, QFontDatabase::Any,
        EcScriptType::SYLLABLE, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::AMERICA,
        u8"Чероки"sv, u8"1821"sv,
        u8"чероки"sv,
        u8"<p>Будущий вождь Секвойя, изобретатель азбуки чероки, был неграмотен, но брал символы из имевшихся книг{{-}}потому они значат "sv
                u8"совсем не то, что в языке-прообразе. Так, гласная «i» записывается буквой {{sm|Ꭲ}}.</p>"sv
            u8"<p>Первой ученицей была шестилетняя дочь Аёка. Изобретение азбуки приняли с недоверием (приняли за одержимого), "sv
                u8"но Секвойя и Аёка с блеском прошли проверку, прочитывая записки друг друга. Письмо быстро распространилось "sv
                u8"среди индейцев, к 1830 грамотны были {{%|90}} чероки.</p>"sv
            u8"<p>С 1828 года Секвойя участвовал в переговорах с США за землю чероки.</p>",
            EcFont::CHEROKEE },
    // Coptic OK, W7 problems → installed Google Noto font
    { "Copt"sv, QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::AFRICA,
        u8"Коптский"sv, u8"II век"sv,
        u8"коптский"sv,
        u8"<p>Происходит из [[ps:Grek|греческого]]. Используется коптами (египетскими христианами) как богослужебный. "
                "Общаются копты по-арабски, как разговорный коптский умер от XVII до XIX{{_}}века."
            "<p>С Юникода 4.1 (2005) коптский отделён от греческого."sv,
                EcFont::COPTIC },
    /// @todo [tofu] Troubles with some rare letters, what to do?
    { "Cyrl"sv, QFontDatabase::Cyrillic,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::EUROPE,
        u8"Кириллица"sv, u8"конец IX века"sv,
        u8"русский, украинский, белорусский, русинский, болгарский, македонский, сербохорватский <i>(Сербия)</i>, "sv
            u8"казахский, киргизский, таджикский, языки народов России"sv,
        u8"<p>До 900 года византийские монахи Кирилл и Мефодий придумали славянскую письменность; неизвестно, была ли это кириллица. "
                u8"Прообразом кириллицы стало византийское письмо унциал для греческого языка, с добавлением звуков, отсутствующих в греческом. "
                u8"Старшинство кириллицы и глаголицы (появившейся тогда же) спорно, большинство учёных считают, что "
                u8"глаголицу создал Кирилл, а кириллицу{{-}}его ученик Климент Охридский, работавший в Болгарии.</p>"
            u8"<p>Кириллица распространилась среди южных славян и проникла на Русь с крещением. "
                u8"В странах, попавших под влияние Рима, кириллицу прямо запрещали.</p>"
            u8"<p>Современная кириллица восходит к гражданскому шрифту, введённому Петром{{_}}I. Шрифт стал компромиссом между сторонниками "
                u8"традиций и западниками.</p>"
            u8"<p>СССР сделал кириллическую письменность для многих языков союзных республик. С развалом СССР на латиницу перешли "
                u8"Азербайджан, Молдавия, Туркмения, Узбекистан.</p>"sv },
    /// @todo [tofu, BMP] Main block OK, but Vedic is a read PitA.
    { "Deva"sv, QFontDatabase::Devanagari,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Деванагари"sv, u8"I—VII век",
        u8"хинди, санскрит и другие языки Индии"sv,
        u8"<p>Деванагари (буквально «язык божественного города») развился из письма брахми и стал алфавитом для многих языков Индии. "
                "Особенность деванагари{{-}}все буквы свисают с горизонтальной черты.</p>"
            "<p>Согласные буквы означают слог с гласной «а». Чтобы отобразить другие слоги, надо добавить огласовку: "
                "<nobr>ка {{sm|क}} + и {{sm| ि}} = ки {{sm|कि}}</nobr>. "
                "Чтобы получить простую согласную, надо добавить знак «[[pt:virama|вирама]]» («остановка»): к{{_}}{{sm|क्}}.</p>"
            "<p>Вирамы в деванагари редки и обычно встречаются в конце слов из-за [[pt:ligature|лигатур]]: "
                "<nobr>ка {{sm|क}} + вирама {{sm|◌्}} + сса {{sm|स}} = ксса {{sm|क्ष}}</nobr>."sv,
                EcFont::DEVANAGARI },
    // Dogri OK, W10 off → installed Google Noto
    { "Dogr"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::REVIVED, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Догра"sv, u8"XIX век",
        u8"догри <i>(Кашмир)</i>"sv,
        u8"<p>Язык догров, индийской народности. Письменность позаимствована из [[ps:Takr|такри]], стандартизирована "
                "в середине XIX{{_}}века при магарадже Ранбире Сингхе. "
                "После смерти Ранбира в 1885 под давлением британцев официальным языком выбрали [[ps:Arab|урду]]. "
                "В XX{{_}}веке стали писать на [[ps:Deva|деванагари]], с XXI{{_}}века появился интерес и к своей "
                "письменности."
            "<p>Перед нами брахмийская абугида с гласной по умолчанию «ə» (в Юникоде условно названа «а»), "
                "которая в безударных слогах сильно редуцировалась, как в [[ps:Guru|пенджабском]]. "
                "К тому же в догри встречаются [[ps:ligature|лигатуры]], не закодированные в данном шрифте. "
                "Потому вирама в догри используется редко, что видно в самоназвании: {{sm|𑠖𑠵𑠌𑠤𑠬}} «до-гᵊ-ра:»."
            "<p>Цифры берутся из такри."sv,
                EcFont::DOGRA },
    // Deseret OK, W10 Segoe UI Symbol, plane 1
    { "Dsrt"sv, QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::EXPERIMANTAL, EcWritingDir::LTR, EcContinent::AMERICA,
        u8"Дезеретская (мормонская)"sv, u8"1854",
        u8"английский, отдельные языки индейцев"sv,
        u8"<p>В XIX{{_}}веке было множество попыток придумать фонетический алфавит для английского, "
                "и Бригам Янг, второй руководитель мормонской церкви, был не первый и не последний. "
                "Считалось, что это позволит иммигрантам лучше ознакомиться с английским. "
                "Основную работу выполнили Парли Пратт и Джордж Уотт."
            "<p>Алфавит намеренно не содержит выносных элементов{{-}}это должно снизить износ шрифта. "
                "В современных шрифтах выносные элементы иногда добавляют."
            "<p>В новом алфавите вышло четыре книги. Когда Янг осознал, сколько денег потребуется, чтобы издать "
                "библиотеку, эксперименты прекратились."
            "<p>В 1997 алфавит внесён в Реестр искусственных письменностей. "
                "Исключён в 2001, когда мормонский появился в настоящем Юникоде."sv },
    // Egyptian hiero OK
    { "Egyp"sv, QFontDatabase::Any,
        EcScriptType::CONSONANTHIEROGLYPH, EcLangLife::HISTORICAL, EcWritingDir::BOTH, EcContinent::AFRICA,
        u8"Египетские иероглифы"sv, u8"≈3200 до н.э.",
        u8"древнеегипетский"sv,
        u8"<p>Египетская письменность прошла обычный путь от пиктографического письма (слова изображаются наглядными рисунками) "
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
            "<p>В Юникоде{{_}}14.0 обещают серьёзную перерисовку иероглифов, имеющийся в «Юникодии» шрифт соответствует "
                "Юникоду{{_}}13.0."sv,
                EcFont::EGYPTIAN },
    // Ethiopic OK, lots of tofu, espec. in W7 → installed Google Noto
    { "Ethi"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::AFRICA,
        u8"Эфиопская"sv, u8"I—VII век",
        u8"амхарский, тигринья и другие эфиосемитские"sv,
        u8"<p>Эфиопская письменность использовалась для записи языка геэз, вышедшего из употребления в XIII{{_}}веке. "
                "Геэз всё ещё используется эфиопскими христианами как литургический{{-}}"
                "а письменность распространилась на другие языки Эфиопии.</p>"sv,
                EcFont::ETHIOPIC },
    // Georgian OK, installed Google Noto font
    { "Geor"sv, QFontDatabase::Georgian,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::EUROPE,
        u8"Грузинский"sv, u8"≈V век"sv,
        u8"грузинский, мегрельский и другие картвельские"sv,
        u8"<p>Достоверно неизвестно, когда создана грузиница и что было прототипом. "
                "Распространённая версия, что грузинский алфавит создан создателем [[ps:Armn|армянского]] Месропом Маштоцем, "
                "отметается учёными.</p>"sv,
                EcFont::GEORGIAN },
    // Glagolitic OK, installed Google Noto font
    { "Glag"sv, QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::EUROPE,
        u8"Глаголица"sv, u8"IX век"sv,
        u8"древнеславянский"sv,
        u8"<p>Глаголица, по наиболее распространённому мнению, была составлена около 863 Кириллом и Мефодием. Считается, что глаголица "
                "старее кириллицы: дорогой пергамент старались повторно использовать, и сохранились палимпсесты (пергаменты, где стёрли "
                "одно и написали другое), где глаголицу заменили кириллицей.</p>"
            "<p>На Руси глаголица встречалась редко, больше применялась в Болгарии и Хорватии.</p>"sv,
            EcFont::GLAGOLITIC },
    // Greek OK, W7 Cambria is horrible → installed Google Noto and checked umlauts
    { "Grek"sv, QFontDatabase::Greek,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::EUROPE,
        u8"Греческий"sv, u8"IX век до н.э."sv,
        u8"греческий"sv,
        u8"<p>Греческий сделан из [[ps:Phnx|финикийского]] без оглядки на раннегреческие системы{{-}}"
                    "[[ps:Linb|линейное письмо Б]] и [[pt:Cypr|кипрское]]. "
                "Финикийский алфавит был [[pt:abjad|консонантным]] (задаёт согласные звуки), и семитским языкам это подходит, ведь корень слова{{-}}"
                "костяк согласных, а гласные играют роль окончаний: так, в [[ps:Arab|арабском]] (тоже семитский) ''джихадом'' занимается <i>моджахед</i> "
                "(или, точнее, ''муджахид'').</p>"
            "<p>Греки добавили гласные звуки, и греческий{{-}}первый известный истинный алфавит, кодирующий как согласные, так и гласные. "
                "Названия букв изменились мало, но перестали что-то значить: ''алеф=бык''{{_}}→ ''альфа''.</p>"
            u8"<p>Из греческого письма пошли [[ps:Latn|латиница]], [[ps:Cyrl|кириллица]], "
                "[[ps:Copt|коптский]] и, возможно, [[ps:Armn|армянский]] и [[ps:Runr|руны]].</p>"sv },
    /// @todo [tofu] Sorry have tofu in W7, install a font
    { "Gujr"sv, QFontDatabase::Gujarati,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Гуджарати"sv, u8"X–XVI век"sv,
        u8"гуджаратский <i>(запад Индии)</i>"sv,
        u8"<p>Появился из [[ps:Deva|деванагари]] и очень похож на него, но уже старый гуджарати потерял характерную черту сверху. "
                "Гласная по умолчанию «ə».</p>"sv },
    // Gurmukhi OK, installed Google Noto *UI* because of W7 troubles; UI better handles umlauts
    { "Guru"sv, QFontDatabase::Gurmukhi,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Гурмукхи"sv, u8"XVI век"sv,
        u8"пенджабский"sv,
        u8"<p>С X века в Пенджабе использовалась письменность шарада, плохо отвечавшая местному языку. Новую письменность придумал "
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
                "шахмукхи («из уст шаха»), разновидность арабского алфавита."sv,
                EcFont::GURMUKHI },
    // Hangul OK, installed Noto CJK font
    { "Hang"sv, QFontDatabase::Korean,
        EcScriptType::ARGUABLE, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Хангыль"sv, u8"1443"sv,
        u8"корейский"sv,
        u8"<p>Хангыль (мужской род!){{-}}корейское алфавитно-слоговое письмо. В нём 51 чамо (буква) объединяется в группы приблизительно по слогам.</p>"
        u8"<p>Придуман группой учёных под руководством царя Седжона Великого, считавшего, что [[ps:Hani|китайские иероглифы]] "
                "плохо передают корейский язык и сложны для народа. Образованная публика смотрела на хангыль надменно, "
                "считая женским, детским или народным письмом. Последующие цари даже запрещали хангыль. Возрождение началось в XX{{_}}веке, "
                "официальным стал в 1945. Единственная письменность Северной Кореи и основная{{-}}в Южной."sv,
                EcFont::CJK },
    // Hani will never be OK, what a pity…
    { "Hani"sv, WS_HANI,       // Special rules for hieroglyphs, SimChi triggers them
        EcScriptType::HIEROGLYPH, EcLangLife::ALIVE, EcWritingDir::LTR_CJK, EcContinent::ASIA_INDIAN,
        u8"Китайские иероглифы"sv, u8"около 2000 до н.э."sv,
        u8"китайский, японский, ранее вьетнамский и корейский"sv,
        u8"<p>Первые пиктограммы относятся к VI{{_}}тысячелетию до н.э., впоследствии они видоизменились под письмо кистью и тушью. "
                "Уже в I{{_}}тысячелетии до н.э. люди стали отходить от иероглифов, но Китай законсервировал архаичную письменность, "
                "этому есть несколько причин:<br>"
            "• многонациональный народ{{-}}помимо государствообразующего народа хань, было ещё около десятка, и они понимали "
                "друг друга хоть на письме;<br>"
            "• культ традиции{{-}}даже сейчас китайцы довольно консервативны;<br>"
            "• неплохо действовавшая меритократия{{-}}тысячи знаков и умение их красиво писать выявляли обучаемых людей.<p>"
            "<p>В III{{_}}веке иероглифы проникли в Японию. У многих иероглифов есть как китайское, так и японское прочтение: "
                "например, {{sm|月}} (луна) имеет китайские прочтения «гэцу» и «гацу», и японское «цуки». "
                "Как читать\u00A0— определяется контекстом: {{sm|月曜日}}=гэцуо:би (понедельник), {{sm|月見}}=цукими (любование луной).</p>"
            "<p>В Китае, Тайване, Японии и Корее иероглифы отличаются, Юникод эти различия не кодирует: тонкости написания передаются шрифтами, "
                "и даже если шрифт неверный, понятно, о чём речь.</p>"
            "<p>Вьетнам отказался от иероглифов в 1945. Северная Корея не использует иероглифы, Южная использует крайне редко.</p>"sv,
                EcFont::CJK },
    // Hanunoo OK, installed Google Noto font
    { "Hano"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::PACIFIC,
        u8"Хануноо"sv, u8"≈1300"sv,
        u8"филиппинские горные народы"sv,
        u8"<p>Хануноо восходит к брахми и похож на тагальский.</p>"sv,
            EcFont::HANUNOO },
    /// @todo [tofu, BMP] 05EF in W7, Yod triangle (rare historical char)
    { "Hebr"sv, QFontDatabase::Hebrew,
        EcScriptType::CONSONANT, EcLangLife::ALIVE, EcWritingDir::RTL, EcContinent::ASIA_INDIAN,
        u8"Иврит"sv, u8"VI—II в. до н.э."sv,
        u8"иврит, ладино, идиш, караимский, крымчакский"sv,
        u8"<p>Иврит развился из [[ps:Armi|арамейской]] письменности, и ко II{{_}}в. до{{_}}н.э. приобрёл почти современный вид.</p>"
            "<p>Записывает только согласные буквы, но четыре буквы {{sm|אהוי}} могут означать гласные. "
                "С той же целью иногда используют огласовки{{-}}точки над буквами."
            "<p>К X{{_}}веку иврит уходит из разговорной практики, оставшись только книжным языком, однако еврейскими буквами "
                "писали и на разговорных еврейских языках: идише, ладино и других. С появлением государства Израиль "
                "иврит снова сделали языком повседневного общения."sv,
                EcFont::HEBREW },
    { "Hent"sv, QFontDatabase::Japanese,
        EcScriptType::SYLLABLE, EcLangLife::HISTORICAL, EcWritingDir::LTR_CJK, EcContinent::PACIFIC,
        u8"Хэнтайгана"sv, u8"≈X век"sv,
        u8"японский"sv,
        u8"<p>Хэнтáйгана{{-}}общее название устаревших знаков [[ps:Hira|хирáганы]], записанных крайне небрежно."
            "<p>Из [[ps:Hani|иероглифов]] произошла вспомогательная азбука манъёгана{{-}}роль играло не изображение на иероглифе, "
                "а прочтение. К X{{_}}веку манъёгана уступила хирагане и [[ps:Kana|катакане]]. "
                "Какой-то системы не было, потому существовали разные знаки для одного слога. "
            "<p>В 1900{{_}}хирагану стандартизировали, за каждым слогом оставили один символ. А хэнтайгана стала чем-то вроде "
                "западного готического шрифта или кириллического устава{{-}}применяется в открытках, религиозных документах, "
                "дипломах школ боевых искусств и… на вывесках ресторанов."
            "<p>«Китайские порномультики» хентай{{-}}омофон: они происходят от {{sm|変態}} «хэнтай»{{-}}извращённый, "
                "а хэнтайгана от {{sm|変体}} «хэнтай»{{-}}необычный."sv,
                EcFont::HENTAIGANA },
    { "Hira"sv, QFontDatabase::Japanese,
        EcScriptType::SYLLABLE, EcLangLife::ALIVE, EcWritingDir::LTR_CJK, EcContinent::ASIA_INDIAN,
        u8"Хирагана"sv, u8"VIII—IX век"sv,
        u8"японский"sv,
        u8"<p>Около III{{_}}века японцы стали писать [[ps:Hani|китайскими иероглифами]]. "
                "Из них произошла вспомогательная азбука манъёгана{{-}}роль играло не изображение на иероглифе, а прочтение. "
                "Хирáгана{{-}}не что иное, как очень небрежно записанная манъёгана."
            "<p>Женщины, которым не было доступно образование, писали слоговой азбукой, и предпочитали не рубленую "
                "[[ps:Kana|катáкану]], а плавную хирáгану. Сейчас хираганой пишут слова, у которых иероглифа нет или неизвестен пишущему/читающему (кроме "
                "заимствованных, для них катакана), окончания слов, учат детей, подписывают прочтение иероглифов."sv,
                EcFont::CJK },
    // Old Italic OK, installed Google Noto (Segoe has no newer chars od 2014+)
    { "Ital", QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::BOTH, EcContinent::EUROPE,
        u8"Этрусский"sv, u8"700–100 до н.э.",
        u8"этрусский и другие языки Италии"sv,
        u8"<p>Алфавит развился под влиянием [[ps:Pnhx|финикийского]] и [[ps:Grek|греческого]]. "
                "В дальнейшем развился в [[ps:Latn|латинский]]."
            "<p>Юникод не различает языков Италии, объединяя все в один блок{{-}}этрусский."sv,
                EcFont::OLD_ITALIC },
    // Javanese OK, W10 has “Javanese Text”, W7 does not, installed Google Noto font
    { "Java"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::PACIFIC,
        u8"Яванский"sv, u8"XV век"sv,
        u8"яванский, мелкие языки Явы"sv,
        u8"<p>Гласная по умолчанию «а». Знак «вирама» (стереть гласную) называется «пангкон» "
                "и представляет собой не косую чёрточку, а длинный крюк. Яванская письменность близка к балийской. "
                "В настоящее время вытесняется латиницей.</p>"sv,
                EcFont::JAVANESE },
    // Kayah Li OK, W10 none, installed Google Noto font
    { "Kali"sv, QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Кая-ли"sv, u8"1962"sv,
        u8"краснокаренский <i>(Мьянма)</i>"sv,
        u8"<p>Кая-ли происходит из брахми, создана Хтаэ Бу Пхаэ в 1962 в ответ на распространение [[ps:Latn|латиницы]]{{-}}"
                "но буквы не имеют гласной по умолчанию, и потому это не абугида (слоговая азбука, где у сходных слогов сходное написание). "
                "Четыре гласных пишутся отдельными буквами, остальные диакритикой над {{sm|ꤢ}}. "
                "Ещё несколько гласных из заимствованных слов пишутся диакритикой над {{sm|ꤣ}}.</p>"
            "<p>Кая-ли{{-}}во многом алфавит беженцев: {{%|90}} живущих в тайских лагерях беженцев{{-}}карены, сбежавшие от зверств бирманских "
                "военных, и в околотюремных условиях важно всё, что сплачивает. "
                "Карены{{-}}разнородная группа народов, среди них есть красные, чёрные, белые и другие, по цвету национального костюма. "
                "Помимо кая-ли, пишут [[ps:Thai|тайским]], латиницей и [[ps:Mymr|бирманским]].</p>"sv,
                EcFont::KAYAH_LI },
    /// @todo [img] what images to choose for Katakana/Hiragana?
    { "Kana"sv, QFontDatabase::Japanese,
        EcScriptType::SYLLABLE, EcLangLife::ALIVE, EcWritingDir::LTR_CJK, EcContinent::PACIFIC,
        u8"Катакана"sv, u8"VIII—IX век"sv,
        u8"японский"sv,
        u8"<p>Около III{{_}}века японцы стали писать [[ps:Hani|китайскими иероглифами]]. "
                "Из них произошла вспомогательная азбука манъёгана{{-}}"
                "роль играло не изображение на иероглифе, а его прочтение. Манъёгана упростилась до катáканы.</p>"
            "<p>Катáкану используют для записи заимствованных слов и начального обучения иностранцев. "
                "До 1946 использовали для записи окончаний слов (сейчас это делают [[ps:Hira|хирáганой]]).</p>"sv,
                EcFont::CJK },
    // Khmer OK, fancy and inconsistent in W7/10, installed Google Noto *UI* because of umlauts
    /// @todo [desc] Describe Khmer, interesting script
    { "Khmr"sv, QFontDatabase::Khmer,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Кхмерский"sv, u8"VI век"sv,
        u8"кхмерский <i>(Камбоджа)</i>"sv,
        u8"<p>Как и другие языки Юго-Восточной Азии, произошёл из брахми. Гласная по умолчанию для одних согласных «а», "
                "для других «о». Самый старый источник датируется 611. "
                "Стал основой для [[ps:Thai|тайского]] и [[ps:Laoo|лаосского]]."sv,
                EcFont::KHMER },
    // Kannada OK, W7 has no recent extensions → installed Google Noto
    /// @todo [desc, BMP] Interesting script, describe
    { "Knda"sv, QFontDatabase::Kannada,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Каннада"sv, u8"XIV век"sv,
        u8"кáннада <i>(Юго-Западная Индия)</i>"sv,
        u8"<p>Как и большинство других письменностей Юго-Восточной Азии, произошла из брахми. "
                "Гласная по умолчанию «а». "
                "Часто применяется в смайликах: {{sm|ಠ␣ಠ}}.</p>"sv,
                EcFont::KANNADA },
    // Lanna OK, W10 none, installed Google Noto font
    { "Lana"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Ланна (тай-тхам)"sv, u8"≈1300"sv,
        u8"юан (=северотайский, =ланна), лы (=тай-лы), кхынский; все\u00A0— Таиланд"sv,
        u8"<p>Происходит из северотайского государства Ланна или Ланнатай (1296—1558).</p>"
            "<p>Как и в других подобных письменностях, круглые очертания букв произошли из-за письма на пальмовых листьях "
                "(прямая черта прорежет лист по жилкам). Гласная по умолчанию «а». "
                "Между словами не ставятся пробелы. Гласные буквы бывают двух видов: независимые (в начале слова) и огласовки. "
                "Вирама называется «ra haam» и используется нечасто.</p>"
            "<p>Имеет два набора цифр: хора (повседневные) и тхам (священные).</p>"
            "<p>Говорящих на языке юан несколько миллионов, но с XX века не многие грамотны в ланне, чаще пишут тайским письмом.</p>"sv,
                EcFont::LANNA },
    // Lao OK, W10 partial, installed SengBuhan font
    { "Laoo"sv, QFontDatabase::Lao,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Лаосский"sv, u8"≈1350"sv,
        u8"лаосский"sv,
        u8"<p>Лаосский алфавит стандартизирован в XIV веке. Как и тайский, происходит из [[ps:Khmr|кхмерского]]. "
                "После реформы, сделанной коммунистами в 1960-е, исчезли гласная по умолчанию и знак вирамы, и у лаосского остался последний "
                "признак абугиды{{-}}сложные правила записи гласных: {{sm|ເກົ້າ}} ''kao'' «девять». Лаосский язык тоновый, и в этом слове "
                "мы видим над согласной {{sm|ກ}} как огласовку из трёх (!) символов , так и тон.</p>"
            "<p>Между словами не ставятся пробелы. Как и в [[ps:Thai|тайском]], буквы называются подходящими словами: "
                "{{sm|ບ}} называется «бо-бэ» или просто «бэ» (козёл). Буква {{sm|ຣ}} (r) стала немой или преобразовалась в «л», и потому "
                "исчезла из орфографии, оставшись только для записи иностранных имён.</p>"
            "<p>В буддизме используется язык пали как священный, для записи этого языка используются несколько дополнительных букв "
                "и та самая вирама.</p>"
            "<p>В Юникоде для пустого места над меткой принят пунктирный кружок; лаосцы традиционно используют косой крест, "
                "в Юникод не попавший. Иногда для этого используют [[ps:Latn|латинскую]] «x».</p>"sv,
                EcFont::LAO },
    /// @todo [semi-tofu] Latin is not OK, see ranges
    { "Latn"sv, QFontDatabase::Latin,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::EUROPE,
        u8"Латиница"sv, u8"I тысячелетие до н.э."sv,
        u8"большинство языков западного и тюркского мира (включая Восточную Европу, Прибалтику, Молдавию), Чёрной Африки, Океании; "sv
            u8"вьетнамский, карельский, курдский, эсперанто"sv,
        u8"<p>Латиница{{-}}[[ps:Grek|древнегреческое]] письмо, адаптированное около VII{{_}}в. до{{_}}н.э. "
                "для записи [[ps:Ital|этрусских]] говоров, впоследствии ставших латинском языком.</p>"
            "<p>В средние века к латинице добавились буквы J, V и W. Минускульное письмо, изобретённое для экономии дорогого пергамента, "
                "превратилось в строчные буквы.</p>"
            "<p>Латинский язык давно мёртв, но широкая территория Римской империи, миссионерская деятельность Папского престола "
                "и Великие географические открытия, совершённые западным миром, сделали латиницу основным алфавитом мира. "
                "Латиница используется в математике, медицине, фонетике, программировании.</p>"
            "<p>С развалом СССР на латиницу перешли Азербайджан, Молдавия, Туркмения, Узбекистан.</p>"sv },
    // Lepcha OK, W10 none, installed Google Noto font
    { "Lepc"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Лепча (ронг)"sv, u8"≈1700"sv,
        u8"лепча <i>(Индия, Бутан, Непал)</i>"sv,
        u8"<p>Гласная по умолчанию «а». Произошла из тибетского, изначально писалась сверху вниз, но потом стали писать горизонтально, "
                "потому буквы напоминают тибетские, повёрнутые на 90°.</p>"
            "<p>Начальные гласные не имеют особую форму, а пишутся с «нулевой» согласной буквой, похожей на амперсанд. "
                "Вирамы нет. Вместо этого, чтобы получить слог из трёх звуков, добавляют сначала огласовку, а затем конечный согласный. "
                "Между словами нет пробелов.</p>"sv,
                EcFont::LEPCHA },
    // Limbu OK, W10 none, installed Google Noto font
    { "Limb"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Лимбу"sv, u8"≈1700"sv,
        u8"лимбу <i>(Непал)</i>"sv,
        u8"<p>Гласная по умолчанию «о».</p>"
            "<p>Начальные гласные не имеют особую форму, а пишутся с «нулевой» согласной буквой {{sm|ᤀ}}, похожей на непальский флаг. "
                "Вирамы нет. Вместо этого, чтобы получить слог из трёх звуков, добавляют сначала огласовку, а затем конечный согласный.</p>"sv,
                EcFont::LIMBU },
    // Linear B OK, W10 none, installed Google Noto font
    { "Linb"sv, QFontDatabase::Any,
        EcScriptType::SYLLABOHIEROGLYPH, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::EUROPE,
        u8"Линейное письмо Б"sv, u8"≈1450 до н.э."sv,
        u8"микенский <i>(протогреческий)</i>"sv,
        u8"<p>Достаточно точно известен момент утверждения на Крите греческой власти{{-}}1450{{_}}до{{_}}н.э. "
                "[[ps:Lina|Линейное письмо А]] начало выходить из использования."
            "<p>Витиеватая форма символов позволяет предположить, что писали на коже или папирусе, тем более линейным письмом А "
                    "действительно писали чернилами. "
                "Но все сохранившиеся памятники выполнены на глиняных табличках, которые обожглись в пожарах и сохранились тысячелетиями. "
                "Письмо использовалось узкой кастой писцов, ориентировавшихся на грамматику минойского языка, а не греческого. "
                "Все сохранившиеся памятники{{-}}канцелярские книги: договоры аренды, списки ремесленников, описи имущества…"
            "<p>Открыто в начале XX{{_}}века Артуром Эвансом. Расшифровано в 1950-е годы."sv,
                EcFont::LINEAR_B },
    /// @todo [tofu, P1] 11FB0 (a single Lisu letter), what to do?
    // W10 has, W7 none (though lots of software install) → installed Google Noto
    { "Lisu"sv, QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Лису (алфавит Фрейзера)"sv, u8"1915"sv,
        u8"лису <i>(Китай)</i>"sv,
        u8"<p>Придуман британским миссионером Джеймсом Фрейзером. Состоит из латинских букв, как обычных, так и повёрнутых. "
                "Строчного регистра нет. Благодаря усилиями миссионеров {{%|40}} лису{{-}}протестанты. "
                "Китайцы признали письменность в 1992.</p>"
            "<p>До этого лису писали слоговым письмом, напоминающим иероглифы. Также миссионеры использовали алфавит Полларда (мяо) "
                "и письменность на [[ps:Thai|тайской]] основе.</p>"sv,
                EcFont::LISU },
    // Mandaic OK, W10 has not, installed Google Noto
    { "Mand"sv, QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::ENDANGERED, EcWritingDir::RTL, EcContinent::ASIA_INDIAN,
        u8"Мандейский"sv, u8"II—VII век"sv,
        u8"мандейский <i>(Иран и Ирак)</i>"sv,
        u8"<p>Произошла из арамейского или парфянского письма. Используется так называемыми «болотными арабами», живущими у слияния рек "sv
                u8"Тигр и Евфрат и исповедующими мандеизм, необычную гностическую религию. Их количество в Ираке быстро уменьшается "sv
                u8"с 45&nbsp;000 (1996) до 5000 (2007), около 60&nbsp;тыс. разбросаны по миру, и, вероятно, на правах беженцев они быстро исчезнут.</p>"sv,
                EcFont::MANDAIC },
    /// @todo [desc] Malayalam
    // Malayalam OK, W7 does not have 2014/17 extensions → installed Google Noto
    { "Mlym"sv, QFontDatabase::Malayalam,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Малаялам"sv, u8"830"sv,
        u8"малая́лам <i>(Южная Индия)</i>"sv,
        u8"<p>Как и большинство других письменностей Юго-Восточной Азии, произошёл из брахми. "
                "Гласная по умолчанию «а». Отличается тем, что в начале слова используются полные огласовки, "
                "в середине и в конце{{-}}сокращённые.</p>"sv,
                EcFont::MALAYALAM },
    // Mongol OK, dislike standard font of W10 → installed Google Noto
    /// @todo [semi-tofu] 18A9 bad rendering
    { "Mong"sv, QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::REVIVED, EcWritingDir::LTR_COL, EcContinent::ASIA_INDIAN,
        u8"Монгольская (старая)"sv, u8"1204"sv,
        u8"монгольский"sv,
        u8"<p>В 1204 монголы завоевали уйгуров и позаимствовали алфавит, восходящий к [[ps:Syrc|сирийскому]]. "
                "Единственная действующая система с записью слева направо по столбцам, из-за чего компьютеры "
                "очень неохотно её поддерживают, заваливая столбец на 90°."
            "<p>В 1587 учёный Аюши-гуши сделал реформу письменности, а также добавил ряд символов для облегчения транскрипции "
                "санскрита, тибетского и китайского, важных в буддизме. Новые символы получили имя али-гали."
            "<p>В России письменность использовалась калмыками и бурятами. "
                "После войны монголы разработали новую письменность на [[ps:Cyrl|кириллице]], но с развалом СССР есть планы "
                "расширить использование старой письменности."
            "<p>Содержит омографы (например, {{sm|ᠣ}}=o/u), однако закон гармонии гласных{{-}}в слове могут быть или одни твёрдые гласные, "
                "или одни мягкие{{-}}часто позволяют раскрыть эти омографы."sv,
                EcFont::MONGOLIAN },
    // Meetei Mayek OK, W10 does not have extensions → installed Google Noto
    { "Mtei"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::REVIVED, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Манипури (мейтей-майек)"sv, u8"XI век; по другим данным, 1930 и всё, что раньше,— подделки"sv,
        u8"манипури <i>(Индия)</i>"sv,
        u8"<p>Какая бы версия о происхождении манипури ни была верной, с XVIII века пользовались бенгальским. Письменность возродили в 1930-х, "
                "в 1976 подогнали под современную фонетику.</p>"
            "<p>Буквы называются по частям тела: скажем, первая\u00A0— «kok» (голова). "
                "Гласная по умолчанию «а». Вирама есть, но применяется крайне редко: "
                "одинокий звук обозначается обрезанными версиями согласных.</p>"sv,
                EcFont::MEETEI },
    // Myanmar OK, W10 “Myanmar Text”, W7 none → installed Google Noto font
    { "Mymr"sv, QFontDatabase::Myanmar,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Бирманская"sv, u8"XI век"sv,
        u8"бирманский <i>(Мьянма)</i>, пали <i>(мёртвый, культовый в буддизме)</i>"sv,
        u8"<p>Как и большинство других письменностей Юго-Восточной Азии, произошёл из брахми. Согласная буква означает слог с «ə». "
                "Чтобы поставить другую гласную, используются огласовки, чтобы убрать{{-}}знак «вирама», которая по-бирмански называется "
                "«а-сат».</p>"
            "<p>Раннебирманское письмо было квадратным; закруглилось оно около XVII{{_}}века, когда распространилось письмо на пальмовых листьях: "
                "прямая линия может разрезать лист по жилкам.</p>"sv,
                EcFont::MYANMAR },
    // NKo ok, W10 “Ebrima”, W7 lacks a few chars → installed Google Noto
    { "Nkoo"sv, QFontDatabase::Nko,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::RTL, EcContinent::AFRICA,
        u8"Нко"sv, u8"1949"sv,
        u8"языки манден <i>(Западная Африка)</i>"sv,
        u8"<p>Алфавит создал гвинейский писатель Соломана Канте в 1949 году. Применяется в основном в Гвинее, Кот-д’Ивуаре и Мали. "
            "«N’ko» означает «я говорю».</p>"sv,
                EcFont::NKO },
    // Nushu OK, W10 off, installed Google Noto
    { "Nshu"sv, QFontDatabase::Any,
        EcScriptType::SYLLABLE, EcLangLife::DEAD, EcWritingDir::LTR_CJK, EcContinent::ASIA_INDIAN,
        u8"Нюй-шу"sv, u8"900 (предположительно) … 1300 (точно)"sv,
        u8"один из диалектов сянского <i>(Хунань, Китай)</i>"sv,
        u8"<p>Нюй-шу существовало веками и использовалось как женское письмо: [[ps:Hani|грамоте]] учили мужчин, а женщины "
                "использовали более простое слоговое письмо. "
                "В отличие от размашистых иероглифов, нюй-шу писали тонкой щепкой. "
                "Многие из памятников нюй-шу{{-}}''послания третьего дня'': "
                "девушка уходила замуж в другую деревню, а подруги и сёстры вышивали ей пожелания, часто в стихах. "
            "<p>Несмотря на распространённое заблуждение, нюй-шу не было тайнописью; на него смотрели как на «низкое» письмо."
            "<p>В отличие от японской [[ps:Hira|хираганы]] сходного назначения, нюй-шу так и осталось неофициальным письмом. "
                "К закату нюй-шу привели ликбез и распространение швейных машин в XX{{_}}веке. "
                "К тому же японские оккупанты запрещали нюй-шу, видя в нём шифровку, "
                "а хунвэйбины в Культурную революцию уничтожали памятники старины, в том числе вышивки."
            "<p>Повторно открыли нюй-шу в 1980-е. Последняя носительница нюй-шу, Ян Хуаньи, умерла в 2004 в возрасте 98{{_}}лет{{-}}"
                "после неё письменностью владели только исследователи. "
                "Вышивки с нюй-шу стало популярным туристским сувениром; опасаются, что туризм исказит древнее письмо."
            "<p>Известно около 1800 разных символов, но многие означали один и тот же слог."
            "<p>«Юникодия» использует шрифт Noto Sans Nushu. Главный типограф{{-}}женщина, француженка китайского происхождения Лиза Хуан."sv,
                EcFont::NUSHU },
    // Ogham OK, Windows font is differently named in W7 and W10 → installed Google Noto
    { "Ogam"sv, QFontDatabase::Ogham,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::LTR_BU, EcContinent::EUROPE,
        u8"Огамическая"sv, u8"IV век"sv,
        u8"древнеирландский, пиктский"sv,
        u8"<p>Огамической письменностью пользовались древние кельты и пикты, жившие в Ирландии и Британии, с IV до X века. "
                "Заодно в ходу была [[ps:Latn|латиница]], потому есть мнение, что это тайнопись. "
                "Письменность приспособлена под зарубки на палках и досках. "
                "Надпись начинается с {{sm|᚛}} и заканчивается {{sm|᚜}}.</p>"sv,
                EcFont::OGHAM },
    // Ol Chiki OK, W10 “Nirmala UI”, W7 none → installed Google Noto
    { "Olck"sv, QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Ол-чики"sv, u8"1925"sv,
        u8"сантальский <i>(Индия)</i>"sv,
        u8"<p>Алфавит придумал в 1925 году Рагхунатх Мурму. До этого пользовались [[ps:Beng|бенгальской]] письменностью, "
                "[[ps:Orya|орией]] или [[ps:Latn|латиницей]], хотя большинство населения было неграмотным. "
                "Сантальский язык австроазиатский, а не индоевропейский, и алфавиты, основанные на брахми, подходили плохо. "
                "Так, у санталов звонкие и глухие чередуются при словоизменении, потому различать их бессмысленно."
            "<p>Ол-чики состоит из 30 букв, названия соответствуют внешнему виду. Первые три: {{sm|ᱚ}}{{_}}огонь, {{sm|ᱛ}}{{_}}земля, "
                "{{sm|ᱜ}}{{_}}отрыжка.",
                EcFont::OLCHIKI },
    /// @todo [tofu] Even Noto has no U+0B55 (2020)
    /// @todo [tofu] Noto has badly placed circle, W7 lacks 2010 fractions
    /// @todo [font] Build a genuine Oriya pair, W7’s Oriya font is Kalinga
    { "Orya"sv, QFontDatabase::Oriya,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Ория"sv, u8"XIV век (в современном виде)"sv,
        u8"ория <i>(Восточная Индия)</i>"sv,
        u8"<p>Ори́я (традиционно считается однокоренным с ''арийцами'') развилась из нагари (ранней формы [[ps:Deva|деванагари]] "
                "без сплошной черты).</p>"
            "<p>Есть мнение, что округлая форма букв сложилась, потому что писали на пальмовых листьях{{-}}чтобы не разрезать лист "
                "по жилкам, верхняя линия стала дугой. Самый ранний известный лист датируется XV веком. "
                "Храмовая надпись XIII века всё ещё обладала разорванной линией, как в нагари.</p>"
            "<p>Гласная по умолчанию «а», убирается знаком вирамы: {{sm|କ}}=ка, {{sm|କି}}=ки, {{sm|କ୍}}=к. "
                "Но вирама в ории употребляется нечасто из-за большого количества лигатур: {{sm|ମ+ଭ=ମ୍ଭ}}{{_}}мбха. "
                "В одной редакции 1-й статьи Декларации прав человека (стандартного текста для показа чужого языка) "
                "нет вирам вообще, в другой{{-}}одна вирама. "
                "Набор таких лигатур стандартный: первая буква, вирама, вторая буква.</p>"sv,
                EcFont::GURMUKHI },
    // Phags-Pa OK, have “Microsoft PhagsPa” font
    { "Phag"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::HISTORICAL, EcWritingDir::LTR_COL, EcContinent::ASIA_INDIAN,
        u8"Монгольское квадратное (Пагба-ламы)", u8"1269",
        u8"китайский, монгольский, тибетский, санскрит, уйгурский",
        u8"<p>Монгольское письмо использовало уйгурский алфавит с минимальными изменениями. "
                "С захватом монголами Китая у императора Хубилая возникли проблемы с коммуникацией между двором и многонациональным народом, "
                "и эту задачу выполнил Пагба-лама, наставник Хубилая.</p>"
            "<p>В 1368 году китайцы выбили монголов, письмо быстро забыли.</p>"
            "<p>Если отбросить необычное направление письма, письмо Пагба-ламы{{-}}обычная брахмийская абугида с гласной "
                "по умолчанию «а». В шрифтах письмо обычно рисуется повёрнутым на 90° влево, чтобы писать по строкам.</p>"sv,
                EcFont::PHAGS_PA },
    // Rejang OK, installed Google Noto font
    { "Rjng"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::PACIFIC,
        u8"Реджанг", u8"≈1750 (первый известный документ)",
        u8"реджанг <i>(Суматра)</i>",
        u8"<p>На языке реджанг говорят 200{{_}}тыс. человек (2000), большинство из них неграмотно. "
                "Письмо постепенно заменяется [[ps:Latn|латиницей]]."
            "<p>Перед нами обычная брахмийская абугида с гласной по умолчанию «а» и вирамой: "
                "{{sm|ꤰ}}=ка, {{sm|ꤰꥉ}}=ke, {{sm|ꤰ꥓}}=k. "
                "Письменность деградировала по той же причине, что и [[ps:Runr|руны]]: проще выцарапывать на дереве."sv,
                EcFont::REJANG },
    // Runes OK, installed Google Noto font
    { "Runr"sv, QFontDatabase::Runic,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::EUROPE,
        u8"Руны"sv, u8"I век"sv,
        u8"древние германские и скандинавские языки"sv,
        u8"<p>Руны имеют специфическую угловатую форму, приспособленную для вырезания: вдоль и поперёк волокон дерево сопротивляется по-разному, "
                "потому сложно делать кривые линии. Позднейшие варианты рун видоизменились под скоропись пером. "
                "С появлением христианства руны были вытеснены латиницей.</p>"
            "<p>Найдено около 5000 памятников рунического письма, самые старые относятся к I{{_}}веку. Скандинавский рунический алфавит"
                "называют «футарк», по первым шести буквам.</p>"
            "<p>В массовой культуре рунами называют письмена произвольной формы, в том числе непригодные ни для вырезания, "
                "ни для письма кистью или пером. Алфавиты древних тюрков, булгар, венгров сходной угловатой формы также "
                "называют рунами.</p>"
            "<p>Фашистская Германия любила эзотерику, а вместе с ней и руны{{-}}потому символы многих фашистских организаций "
                "(тогдашних и современных) представляют собой руны.</p>"
            "<p>Блок кодирует алфавиты скандинавских и англосаксонских рун, частично пересекающиеся. "
                "В 2014 добавили три руны для записи современного английского, приписываемые Дж.{{_}}Толкину, "
                "и пять рун с ларца Фрэнкса, реликвии VIII{{_}}века.</p>"sv,
                EcFont::RUNIC },
    // Samaritan OK, installed Google Noto font, added dotted circle
    { "Samr"sv, QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::ENDANGERED, EcWritingDir::RTL, EcContinent::ASIA_INDIAN,
        u8"Самаритянский"sv, u8"около 600—200 до н.э.",
        u8"иврит, самаритянский арамейский"sv,
        u8"<p>Происходит из палеоеврейского письма. По Библии, самаритяне пришли в Палестину из Двуречья и приняли еврейскую "
                "религию и культуру. На 2021 существует не более 700 самаритян.</p>"sv,
                EcFont::SAMARITAN },
    // Saurashtra OK, W10 none, installed Google Noto
    { "Saur"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Саураштра"sv, u8"XIX век (возможно, раньше)",
        u8"саураштра <i>(Индия, письменность используется только в штате Тамилнад)</i>"sv,
        u8"<p>Происхождение неясно, все [[ps:Taml|тамильские]] саураштра двуязычны, а в остальных штатах пишут на "
                "[[ps:Deva|деванагари]], [[ps:Gujr|гуджарати]], [[ps:Telu|телугу]]. "
                "По переписи 2007, в Индии 247{{_}}тыс. саураштра."
            "<p>Перед нами обычная брахмийская абугида с гласной по умолчанию «а» и вирамой: "
                "{{sm|ꢒ}}=ка, {{sm|ꢒꢶ}}=ки, {{sm|ꢒ꣄}}=к. Вирама употребляется часто. "
                "Начальная гласная{{-}}отдельный символ: {{sm|ꢄ}}=и."sv,
                EcFont::SAURASHTRA },
    // Sinhala OK, W10 obviously has no 2020 extension → installed Google Noto
    /// @todo [desc, BMP] interesting script, describe
    { "Sinh"sv, QFontDatabase::Sinhala,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Сингальский"sv, u8"300",
        u8"сингальский <i>(Шри-Ланка)</i>",
        u8"<p>Как и большинство других письменностей Юго-Восточной Азии, произошла из брахми. Буквы имеют витиеватую форму, "
                "потому что в древности часто писали на пальмовых листьях, а прямые линии разрезали бы лист по жилкам.</p>"sv,
                EcFont::SINHALA },
    // Sundanese OK, even W10 off → installed Google Noto
    { "Sund"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::REVIVED, EcWritingDir::LTR, EcContinent::PACIFIC,
        u8"Сунданский"sv, u8"XIV век",
        u8"сунданский <i>(Ява)</i>"sv,
        u8"<p>Язык сундов (38{{_}}млн на 2007), распространённый на западе Явы. Обычная брахмийская абугида с гласной по умолчанию «а». "
                "Для других гласных используются огласовки, для голой согласной{{-}}знак «вирама» ''(pamaaeh)'': "
                "{{sm|ᮞ}}=са, {{sm|ᮞᮥ}}=су, {{sm|ᮞ᮪}}=с. Начальные гласные{{-}}отдельные буквы.</p>"
            "<p>Письменность использовалась до XVIII{{_}}века, после этого стали использовать [[ps:Java|яванскую]] и "
                "пегон (местный вариант [[ps:Arab|арабского]]); с XX века{{-}}[[ps:Latn|латиницу]]. "
                "В 1997 году власти Индонезии решили дать сундам собственную письменность, сделав её из старосунданского.</p>"sv,
                EcFont::SUNDANESE },
    // Syloti Nagri OK, W10 none → installed Google Noto
    { "Sylo"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Силхети-нагари"sv, u8"XV век",
        u8"силхетский <i>(Бангладеш, Индия)</i>"sv,
        u8"<p>По одной из теорий, письменность произошла из кайтхи и изобретена мусульманами. За простоту письменность была любима поэтами "
                "XIX{{_}}века, но к середине XX{{_}}века уступила [[ps:Beng|бенгальской]]. В 2000-е годы в Бангладеше "
                "начали снова изучать силхети. На 2007 было 11{{_}}млн силхетов."
            "<p>Обычная брахмийская абугида с гласной по умолчанию «о» и вирамой ''(хасанта)'': {{sm|ꠇ}}=ко, {{sm|ꠇꠣ}}=ка, {{sm|ꠇ꠆}}=к. "
                "Существует и другая запись вирамы, {{sm|ꠇ꠬}}=к, на манер других брахмийских письменностей. "
                "Начальные гласные пишутся отдельными буквами. "
                "Текст свисает с общей черты{{-}}но, в отличие от [[ps:Deva|деванагари]], крепко за неё «держится»."
            "<p>Вирама в силхети нечаста по двум причинам. Во-первых, двойные согласные и некоторые стыки образуют лигатуры: "
                "{{sm|ꠇ\uA806ꠇ}}=кко, {{sm|ꠇ\uA806ꠔ}}=кто. Набор таких лигатур стандартный: первая буква, вирама, вторая буква. "
                "Кроме того, иногда конечная «о» становится немой."
            "<p>Запись слева направо, но силхетские книги, как дань исламу, сшиты справа налево."sv,
                EcFont::SYLOTI_NAGRI },
    // Syriac OK, W10 has no Malayalam extensions → Beth Mardutho made a nice font.
    { "Syrc"sv, QFontDatabase::Syriac,
        EcScriptType::CONSONANT, EcLangLife::ENDANGERED, EcWritingDir::RTL, EcContinent::ASIA_INDIAN,
        u8"Сирийский"sv, u8"I век",
        u8"сирийский <i>(исп. как литургический)</i>, новоарамейские, малаялам, согдийский"sv,
        u8"<p>Потомок [[ps:Armi|арамейского]] алфавита, впоследствии развившийся в [[ps:Arab|арабицу]]. "
                "Используется малыми семитскими народами. В сирийском письме уже видны некоторые особенности арабицы: "
                "сплошная вязь, выписываемая (кроме огласовок) одним росчерком, разные формы изолированной, начальной, внутренней "
                "и конечной буквы.</p>"
            "<p>Тот вид письма, которым написаны раннехристианские рукописи (50–400), известен под названием эстрангело (греч. στρονγύλη{{-}}круглое). "
                "Западная Сирия (Сирия, Палестина) принадлежала грекам, а восточная (Персия, Двуречье){{-}}персам. Это разделило церковь, "
                "язык и письменность на две формы: восточносирийскую (несторианскую, халдейскую) и  западносирийскую (яковитскую, маронитскую). "
                "В них используются разные почерки и огласовки.</p>"
            "<p>В системе, скорее всего, найдётся эстрангело{{-}}он принят основным в западной историографии. "
                "А вот с «Юникодией» для разнообразия поставляется западный вариант{{-}}его отлил дипломат и печатник Савари́ де Брев "
                "(1560–1627), а довёл до компьютерного шрифта институт ''Beth Mardutho'' для издания Антиохийской Библии.</p>"sv,
                EcFont::SYRIAC },
    // Tagbanwa OK, W10 off → installed Google Noto
            /// @todo [describe] Another interesting script            
    { "Tagb"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::PACIFIC,
        u8"Тагбанва"sv, u8"≈1300"sv,
        u8"языки о. Палаван <i>(Филиппины)</i>"sv,
        u8"<p>Восходит к брахми и родственная с тагальской. Осталось не более 25000 людей, говорящих на этих языках.</p>"sv,
                EcFont::TAGBANWA },
    // Old Tai Le OK, M$ has a good font
    { "Tale"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Лы"sv, u8"≈1200"sv,
        u8"дайские (=лы; <i>Китай, Вьетнам, Лаос, Мьянма, Таиланд</i>)"sv,
        u8"<p>Как и большинство других письменностей Юго-Восточной Азии, произошла из брахми. Тона изначально не записывались. "
                "После нескольких реформ XX\u00A0века (последняя\u00A0— 1988), в том числе неудачной попытки китайских властей "
                "создать [[ps:Talu|новое письмо лы]], письменность почти что превратилась в алфавитную, и абугидой её "
                "оставляют «пустая» согласная {{sm|ᥟ}} в начале слова, если оно начинается с гласной, и тоновые буквы "
                "после конечной согласной слога (если таковая есть).</p>"
            "<p>Пробелы пишутся между предложениями, но не между словами. </p>"
            "<p>В лы третьей реформы (1963—1988) для записи тонов использовались обычные европейские умляуты: "
                "<big>à, á, ȧ, ä, ǎ</big>.</p>"
            "<p>Дайцев в Китае ≈500 тыс. (2001), но язык быстро заменяется китайским и потому отнесён к уязвимым. "
                "Ещё не более 500 тыс. разбросано по другим странам, они пользуются в основном [[ps:Lana|ланной]].</p>"sv,
                EcFont::TAILE },
    // New Tai Lue OK, M$ has a good font
    { "Talu"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Новое письмо лы"sv, u8"1950-е"sv,
        u8"дайские (=лы; <i>Китай; общины лы в других странах пользуются старым письмом</i>)"sv,
        u8"<p>Созданный в 1950-е годы китайскими властями алфавит встретил сопротивление народа, и Китай пошёл по пути "
                "постепенного упрощения [[ps:Tale|старого письма лы]].</p>"
            "<p>Гласная по умолчанию «а». Вирама имеет вид крючка с петлей и в Юникоде записывается как единое целое с согласной: {{sm|ᧇ}}. "
                "Необычно устроена запись тонов: согласные бывают в двух регистрах, ещё два знака {{sm|ᧈᧉ}} могут "
                "приписываться к слогу в конце, итого 2·3=6 тонов.</p>",
                EcFont::TAILUE },
    // Tamil BMP OK, different fonts in W10 and W7
    /// @todo [tofu, P1] Aux. Tamil characters are still tofu
    { "Taml"sv, QFontDatabase::Tamil,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Тамильский"sv, u8"VI—IX век"sv,
        u8"тамильский <i>(Индия, Шри-Ланка, Сингапур)</i>"sv,
        u8"<p>Как и большинство других письменностей Юго-Восточной Азии, произошла из брахми. "
                "Гласная по умолчанию «а». Вирама употребляется очень часто и превратилась в точку. "
                "Методы прикрепления огласовок к согласным довольно причудливы: ка={{sm|க}}, ки={{sm|கி}}, тта={{sm|ட}}, тти={{sm|டி}}. "
                "Начальная гласная записывается отдельным знаком: и={{sm|இ}}."
            "<p>В отличие от других индийских систем счёта, есть специальные символы для 10, 100 и 1000."
            "<p>Дравидийский Юг никогда не покорялся мусульманам и даже создал шиваизм, особо ортодоксальную ветвь индуизма."
            "<p>В русском заимствований из тамильского мало, наиболее известное\u00A0— <i>катамаран</i>. "
                "В английском также <i>cash</i> (касса, наличные), <i>ginger</i> (имбирь), <i>coolie</i> (чернорабочий) и другие."
            "<p>До того, как правительство Индии утвердило знак рупии ₹, у тамилов он существовал: {{sm|௹}}."sv,
                EcFont::TAMIL },
    // Tai Viet OK, installed Google Noto font
    { "Tavt"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Тай-вьет"sv, u8"XVI век"sv,
        u8"языки чёрных тай <i>(Вьетнам)</i>"sv,
        u8"<p>Происходит из [[ps:Thai|старотайского]] (царство Сукхотаи). "
                "Впервые стандартизирована в 1961, позднее под эгидой ЮНЕСКО в 2006."
            "<p>Гласной по умолчанию нет, однако письменность остаётся абугидой из-за причудливых огласовок слева, сверху и справа:"
                "{{sm|ꪶꪀ}}=ко, {{sm|ꪀꪱ}}=ка, {{sm|ꪀꪲ}}=ки. "
                "Традиционно читатель должен был угадывать тон, уже в первой стандартизации добавили тоновые знаки."sv,
                EcFont::TAI_VIET },
    // Telugu OK, W7 lacks recent extensions → installed Google Noto
    /// @todo [desc, BMP] Telugu
    { "Telu"sv, QFontDatabase::Telugu,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Телугу"sv, u8"XIV век"sv,
        u8"телугу <i>(Юго-Восточная Индия)</i>"sv,
        u8"<p>Как и большинство других письменностей Юго-Восточной Азии, произошла из брахми.</p>"sv,
                EcFont::TELUGU },
    // Tifinagh OK, W7 “Ebrima” of course has no 2012 chars → installed Google Noto
    { "Tfng"sv, QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::AFRICA,
        u8"Тифинаг (берберский)", u8"не позднее III в. (древняя консонантная), XX в. (современная)",
        u8"туарегские и другие берберские <i>(Марокко)</i>",
        u8"<p>Происхождение староливийского письма неизвестно. Самый старый памятник{{-}}погребение царицы Тин-Хинан (IV{{_}}в.). "
                "Письмо было [[pt:abjad|консонантное]], писали чаще всего снизу вверх или справа налево."
            "<p>В XX веке из староливийского создали новую письменность, с написанием слева направо. Исламские государства "
                "(в частности, Ливия Каддафи и Марокко 80-х) запрещали письменность, но в 2003 король Марокко разрешил тифинаг. "
                "Переходный совет Ливии также разрешил берберские языки и тифинаг."sv,
                EcFont::TIFINAGH },
    // Tagalog OK, W10 off → installed Google Noto
    { "Tglg"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::PACIFIC,
        u8"Тагальский (байбайин)"sv, u8"1300 (спорно) — 1500 (точно)"sv,
        u8"старотагальский и другие языки Явы и Филиппин"sv,
        u8"<p>Обычная брахмийская абугида с гласной по умолчанию «а» и вирамой. Обойдусь самоназванием письменности: "
            "{{sm|ᜊᜌ᜔ᜊᜌᜒᜈ᜔}} «ба-й-ба-йи-н»."
            "<p>Испанские миссионеры (XVI{{_}}век) увидели, что островитяне поголовно грамотны, и сами стали учить их на байбайине. "
                "При этом не сразу поняли, в каком направлении пишут: текст вырезали на бамбуковых дощечках, направляя нож от себя, "
                "потому могли писать в одном направлении и читать в другом."
            "<p>Причин упадка байбайина много: испанцы завезли бумагу, перо и печать, под их влиянием аборигены стали различать звуки "
                "r/d, u/o, i/e. А главное: [[ps:Latn|латиница]] помогает в жизни, позволяя получить престижную конторскую работу. "
                "Так что в XVIII{{_}}веке обнаружили, что аборигены переняли латиницу. "
                "К XIX{{_}}веку байбайин был мёртв, и оставалось только упорядочить латинский алфавит. "
                "Есть современные попытки возродить байбайин.</p>"sv,
                EcFont::TAGALOG },
    // Thaana OK, Monotype/M$ “MV Boli” font
    { "Thaa"sv, QFontDatabase::Thaana,
        EcScriptType::ARGUABLE, EcLangLife::ALIVE, EcWritingDir::RTL, EcContinent::ASIA_INDIAN,
        u8"Тана"sv, u8"XVIII век"sv,
        u8"дивехи <i>(мальдивский)</i>"sv,
        u8"<p>Письменная культура пришла на Мальдивские острова вместе с буддизмом, появилось письмо ''эвела акуру'' на основе "
                "[[ps:Sinh|сингальского]]. Самая старая надпись датируется VII—VIII веком. "
                "В 1153 году некто Мухаммад ул-Абдалла провозгласил себя султаном и обратил мальдивцев в ислам; "
                "к XIV веку письмо видоизменилось в ''дивес акуру'' и сменило направление на обратное. "
                "В 1703 году султан Ибрахим{{_}}III заменил старую графику новой{{-}}''габули таной'' (стандартным письмом).</p>"
            "<p>Происхождение таны уникально: согласные произошли из арабских и доисламских цифр, гласные{{-}}"
                "из [[ps:Arab|арабских]] огласовок. "
                "Со временем линии сгладились, стали писать с изящным 45-градусным наклоном. Путешественник Тур Хейердал сравнил тану "
                "с нарезанной лапшой.</p>"
            "<p>В 1970-е годы тане грозило исчезновение: при президенте Ибрагиме Насире появилась телексная связь, и "
                u8"правительство перешло на довольно некачественную латинскую транслитерацию. Новый президент Момун Абдул Гайюм восстановил тану, "
                u8"и обе письменности используются параллельно.</p>"sv,
                EcFont::THAANA },
    // Thai OK, W7 and 10 fonts are different, but managed to
    { "Thai"sv, QFontDatabase::Thai,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Тайский"sv, u8"1283"sv,
        u8"тайский"sv,
        u8"<p>Произошла из старокхмерского, который, в свою очередь, произошёл из брахми. "
                "Алфавит упорядочил и утвердил король Рамакхамхаенг Великий в 1283 году.</p>"
            "<p>Гласная по умолчанию «а» в закрытых слогах и «о» в открытых: {{sm|ช}}=чо. Но буквы называются по словам: "
                "так, эта буква называется «чо-чан» (слон). Между словами не ставятся пробелы.</p>"sv,
                EcFont::THAI },
    // Tibetan OK, W10 did not have swastika, installed Google Noto
    { "Tibt"sv, QFontDatabase::Tibetan,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Тибетская"sv, u8"≈650"sv,
        u8"тибетские, дзонг-кэ <i>(Бутан)</i>, шерпский…"sv,
        u8"<p>Как и большинство других письменностей Юго-Восточной Азии, произошла из брахми, гласная по умолчанию «а».</p>"
            "<p>Считается, что письменность изобрёл Тхонми Самбхота, съездивший в Индию и заложивший основы тибетского буддизма. "
                "За это время орфография мало менялась, а язык изменился, потому произношение сильно расходится с написанием.</p>"sv,
                EcFont::TIBETAN },
    // Vai OK, have good M$ font Ebrima
    { "Vaii"sv, QFontDatabase::Any,
        EcScriptType::SYLLABLE, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::AFRICA,
        u8"Ваи"sv, u8"≈1832"sv,
        u8"ваи <i>(Либерия)</i>"sv,
        u8"<p>Письмо появилось около 1832. Разработал его Момолу Дувалу Букеле, полагают, что под влиянием "
                "[[ps:Cher|письма чероки]] (вместе с неграми в Либерию эмигрировали и индейцы, "
                "один стал даже вождём ваи).</p>"
            "<p>Одна из причин выживания ваи{{-}}удачные знаки, применявшиеся и до этого в негритянской культуре. "
                "Другая{{-}}он использовался для торговли с голландцами и португальцами. "
                "В 1962 Университет Либерии добавил знаки для недостающих слогов.</p>"sv,
                EcFont::VAI },
    // Yi OK, have M$ font. Though quite thin, Noto is better, I managed to counteract by increasing font.
    { "Yiii"sv, QFontDatabase::Any,
        EcScriptType::SYLLABLE, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::AFRICA,
        u8"И (носу)"sv, u8"1974 (слоговая)"sv,
        u8"и <i>(Китай)</i>"sv,
        u8"<p>Первый известный экземпляр старого (иероглифического) письма '''и''' датируется ≈1500, но письмо наверняка намного старше. "
                "Предания говорят, что оно существовало ещё в династию Тан (618—907). Письмо состоит из 8000…10000 иероглифов.</p>"
            "<p>Современный слоговой '''и''' придуман правительством Китая в 1974. В языке 43 согласных, 8 гласных и 4 тона, "
                "но не все комбинации возможны, и письмом '''и''' можно записать 1165 слогов.</p>",
                EcFont::YI },
    { "Zinh"sv, QFontDatabase::Any,
        EcScriptType::NONE, EcLangLife::NOMATTER, EcWritingDir::NOMATTER, EcContinent::NONE,
        u8"Разные"sv, {}, {},
        u8"<p>Комбинирующая метка используется в нескольких разных письменностях.</p>"sv },
};


static_assert(std::size(uc::scriptInfo) == static_cast<size_t>(uc::EcScript::NN));


const uc::Script* uc::findScript(std::string_view x) { return findInArray(x, scriptInfo); }


constinit const uc::NumType uc::numTypeInfo[static_cast<int>(uc::EcNumType::NN)] {
    { {} },
    { u8"Цифра"sv },
    { u8"Особая цифра"sv },
    { u8"Число"sv },
};


constinit const uc::BidiClass uc::bidiClassInfo[static_cast<int>(EcBidiClass::NN)] {
    { "DIR"sv, EcBidiStrength::EXPLICIT,    u8"Тэг направления"sv,
               u8"Тэг направления"sv, u8"FSI, LRE и др.",
               u8"Форматирующие символы, корректирующие работу двунаправленного алгоритма Юникода.</p>"
                    "<p>Строго говоря, каждый из этих символов{{-}}это отдельный класс в двунаправленном алгоритме, "
                    "но если вы не программист, вам достаточно знать: это тэги направления."sv,
               false },
    { "AL"sv,  EcBidiStrength::STRONG,      u8"Арабская буква"sv,
               u8"Арабская буква"sv, u8"AL",
               u8"Пишутся справа налево. Есть тонкие правила, связанные с европейскими цифрами."sv,
               true },
    { "AN"sv,  EcBidiStrength::WEAK,        u8"Арабская цифра"sv,
               u8"Арабская цифра"sv, u8"AN",
               u8"Пишутся справа налево. Есть тонкие правила, связанные с уровнями вложенности.",
               true },

    { "B"sv,   EcBidiStrength::NEUTRAL,     u8"Разделитель абзацев"sv,
               u8"Разделитель абзацев"sv, u8"B",
               u8"Каждый абзац обрабатывается двунаправленным алгоритмом по отдельности."sv,
               false },
    { "BN"sv,  EcBidiStrength::NEUTRAL,     u8"Неграфический"sv,
               u8"Неграфический"sv, u8"BN",
               u8"Управляющие, соединители и тэги, не имеющие своего написания."
                    "<p>См. [[pt:graphic|графический символ]].",
               false },
    { "CS"sv,  EcBidiStrength::WEAK,        u8"Цифровой разделитель"sv,
               u8"Цифровой разделитель"sv, u8"CS",
               u8"Оказавшись в арабском или европейском числе, эти символы становятся его частью.",
               true },

    { "EN"sv,  EcBidiStrength::WEAK,        u8"Европейская цифра"sv,
               u8"Европейская цифра"sv, u8"EN",
               u8"Пишутся слева направо. Есть тонкие правила, связанные с арабским текстом."sv,
               true },
    { "ES"sv,  EcBidiStrength::WEAK,        u8"Европейский ±"sv,
               u8"Европейский ±"sv, u8"ES",
               u8"Символы + и − внутри европейского числа становятся его частью. Символ ± к этому классу не относится."sv,
               true },
    { "ET"sv,  EcBidiStrength::WEAK,        u8"Европейская единица измерения"sv,
               u8"Европейская единица измерения"sv, u8"ET",
               u8"Валютные и прочие символы, встречающиеся рядом с числами. Оказавшись рядом с европейским числом, они становятся частью числа."sv,
               true },

    { "L"sv,   EcBidiStrength::STRONG,      u8"Слева направо"sv, u8"→", u8"L",
               u8"Латиница, кириллица и прочие алфавиты с написанием слева направо. "
                    "Поскольку подавляющая доля Юникода{{-}}[[ps:Hani|китайские иероглифы]], это самый многочисленный класс."sv,
               true },
    { "NSM"sv, EcBidiStrength::WEAK,        u8"Непротяжённая метка"sv,
               u8"Непротяжённая метка"sv, u8"NSM",
               u8"Непротяжённые и охватывающие комбинирующие метки. Направление написания\u00A0— как у основного символа."sv,
               true },
    { "ON"sv,  EcBidiStrength::NEUTRAL,     u8"Прочий нейтральный"sv,
               u8"Прочий нейтральный"sv, u8"ON\u00A0+\u00A0¬\u00A0Bidi_M",
               u8"Приспосабливается к направлению окружающего текста."sv,
               true },

    { "NM"sv,  EcBidiStrength::NEUTRAL,     u8"Нейтральный отзеркаливающийся"sv,
               u8"Отзеркаливающийся"sv, u8"ON\u00A0+\u00A0Bidi_M",
               u8"Скобки и похожие символы при написании справа налево отзеркаливаются. По понятным причинам все такие символы "
                    "являются нейтральными: приспосабливаются к направлению окружающего текста."sv,
               true },
    { "R"sv,   EcBidiStrength::STRONG,      u8"Справа налево"sv, u8"←", u8"R",
               u8"Иврит, а также исторические письменности Ближнего Востока."sv,
               true },
    { "S"sv,   EcBidiStrength::NEUTRAL,     u8"Разделитель участков"sv,
               u8"Разделитель участков"sv, u8"S",
               u8"Tab и некоторые другие символы. Посреди текстового абзаца встречаются редко, но обычно каждый участок обрабатывается двунаправленным алгоритмом по отдельности. Направление табуляции внутри абзаца постоянное."sv,
               false },
    { "WS"sv,  EcBidiStrength::NEUTRAL,     u8"Пробел"sv, u8"Пробел"sv, u8"WS",
               u8"Нейтральны. Есть тонкие правила, касающиеся форматирования пробелами."
                "<p>См. термин [[pt:space|пробел]].",
               true },
    //{ u8"Ошибка"sv },  check for equal number
};


constinit const uc::Block uc::blocks[302] {
    // Basic Latin OK
    { 0x0000, 0x007F,
            "Basic Latin", u8"Базовая латиница"sv, CT_ASCII, EcScript::Latn },
    // Latin-1 OK
    { 0x0080, 0x00FF,
            "Latin-1 Supplement", u8"Латиница-1"sv, CT_LATIN1, EcScript::Latn },
    // Latin extended A OK
    { 0x0100, 0x017F,
            "Latin Extended-A", u8"Латиница расширенная A"sv,
            u8"Содержит символы языков Центральной и Восточной Европы, Прибалтики, кельтских, а также саамского, мальтийского, "
                "турецкого, эсперанто и некоторых других."sv },
    // Latin extended B OK
    { 0x0180, 0x024F,
            "Latin Extended-B", u8"Латиница расширенная B"sv,
            u8"Содержит символы словенского, хорватского, румынского, ливского, чжуанского, пиньиня (латинизации китайского), африканских, "
                    "индейских языков, а также старой (до 1930) латиницы языков бывшего СССР."sv },
    // IPA extensions OK
    { 0x0250, 0x02AF,
            "IPA Extensions", u8"Расширения МФА"sv,
            u8"<p>Международный фонетический алфавит на основе латинского используется языковедами, логопедами, певцами и актёрами, "
                    "чтобы передавать произношение языков. Для передачи дополнительных особенностей речи{{-}}шепелявости, скрипа зубов "
                    "и прочего{{-}}используются расширения МФА.</p>"
                "<p>На самом деле большая часть символов этого блока{{-}}не расширения, а обычный МФА: так, {{sm|ɦ}} "
                    "есть в русском «ага», украинском «гривня» и чешском «hrad».</p>"
                "<p>Оперные певцы часто поют на чужом языке, и их способность читать знаки транскрипции часто используется составителями "
                    "звуковых словарей. В частности, знаменитый звук «Янни или Лорел» изначально начитал певец Джей Обри Джонс{{-}}"
                    "низкой мужской голос, пройдя через некачественный динамик и столь же некачественный микрофон, стал восприниматься "
                    "как «Янни».</p>"
                "<p>А ещё символы МФА позволяют перевернуть текст шутки ради.</p>"sv },
    // Spacing letter modifiers OK
    { 0x02B0, 0x02FF,
            "Spacing Modifier Letters", u8"Протяжённые модификаторы букв"sv,
            u8"<p>Модификаторы букв используются в фонетике и языкознании, чтобы передать тоны, длину, мягкость и условные звуки: "
                    "так, английское ''car'' (легковой автомобиль) записывается как {{sm|[kɑːʳ]}},"
                    "и последние два символа{{-}}модификаторы, обозначающие длину и условное «r».</p>"
                "<p>Также в этом блоке есть протяжённые копии диакритических меток.</p>"sv },
    /// @todo [semi-tofu] Diacritical marks work somehow, though circle from 6 circles is too rough
    { 0x0300, 0x036F,
            "Combining Diacritical Marks", u8"Диакритические метки"sv,
            u8"<p>Диакритические знаки{{-}}надстрочные, подстрочные или внутристрочные знаки, прикрепляемые к букве и изменяющие "
                    "или уточняющие её значение. Например, точки над Ё или ударéние.</p>"
                "<p>Комбинирующий символ, или метка{{-}}несамостоятельный символ в Юникоде, прикрепляемый к другому символу. "
                    "Комбинирующий символ всегда идёт после основного. Определения настолько близки, что очевидно: комбинирующие символы "
                    "придуманы в первую очередь для диакритических знаков.</p>"
                "<p>В большинстве ОС механизм прикрепления продуман плохо, и если дизайнер предусмотрел верное положение "
                    "метки, получается красиво (о́). А если нет{{-}}обычно плохо (8́).</p>"
                "<p>Иногда диакритические знаки настолько срастаются с буквой, что получается, по сути, другая буква. "
                    "Таковы русские Ё и Й. Считается, что все такие «другие буквы» крупных живых языков представлены в Юникоде "
                    "монолитными символами. Ударение другой буквой не считается, к тому же сразу в нескольких шрифтах Windows "
                    "его неудачно нарисовали{{-}}потому нередко ударные русские буквы имитируют похожими монолитными латинскими.</p>"sv },
    // Greek and Coptic OK
    { 0x0370, 0x03FF,
            "Greek and Coptic", u8"Греческий и коптский",
            u8"<p>[[ps:Grek|Греческий]]{{-}}первый настоящий алфавит, предок всех европейских алфавитов.</p>"
                "<p>[[ps:Copt|Коптский]]{{-}}язык египетских христиан. Его алфавит основан на греческом.</p>"sv },
    /// @todo [semi-tofu] 0488, 0489 renders too badly for a sample
    { 0x0400, 0x04FF,
            "Cyrillic", u8"Кириллица", {}, EcScript::Cyrl },
    // Cyrillic supplement OK
    { 0x0500, 0x052F,
            "Cyrillic Supplement", u8"Кириллица дополнительная",
            u8"<p>Буквы нескольких мелких кириллических языков: абзахского, курдского ''(как нацменьшинства СССР)'', коми, "
                    "мордовского, алеутского, азербайджанского ''(ныне на [[ps:Latn|латинице]])'' "
                    "и чувашского в алфавите Яковлева ''(использовался 1873–1938)''."
                "<p>Содержит смешную букву {{sm|Ԥ}}. Её «изобрёл» писатель Леонид Каганов{{-}}как оказалось, "
                    "не{{_}}первым: ещё за три года до его заметки (2008) майки с этой буквой продавались в России. "
                    "И независимо от них в абхазском вместо крюка {{sm|Ҧ}} (читается «пх») стали использовать лапку, "
                    "что и дало букве {{sm|Ԥ}} место в Юникоде."sv,
            EcScript::Cyrl },
    /// @todo [tofu] Armenian 0560, 0588 (both are 2018)
    { 0x0530, 0x058F,
            "Armenian", u8"Армянский", {}, EcScript::Armn },
    /// @todo [tofu] Hebrew 05EF of 2018, yod triangle
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
            "Arabic Supplement", u8"Арабский дополнительный"sv,
            u8"<p>Буквы для языков Африки, Пакистана и раннего персидского."sv,
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
            "Syriac Supplement", u8"Сирийский дополнительный"sv,
            u8"<p>Необычная запись языка [[ps:Mlym|малая́лам]] сирийскими буквами, именуемая '''суриани''' или '''каршони'''. "
                    "Использовалась индийскими христианами до XIX{{_}}века."sv,
            EcScript::Syrc },
    /// @todo [tofu] 08BE+, all are 2020, 11 chars
    { 0x08A0, 0x08FF,
            "Arabic Extended-A", u8"Арабский расширенный A"sv,
            u8"<p>Дополнительные арабские буквы для рохинджа ''(Мьянма)'', белорусского, татарского, башкирского, "
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
    /// @todo [tofu?] Check for Gujarati
    { 0x0A80, 0x0AFF,
            "Gujarati", u8"Гуджарати", {}, EcScript::Gujr },
    /// @todo [tofu] Check for Oriya
    { 0x0B00, 0x0B7F,
            "Oriya", u8"Ория", {}, EcScript::Orya },
    /// @todo [tofu?] Check for Tamil
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
    /// @todo [tofu] Need special support of custom fonts (someone may install obsolete Noto)
    { 0x10A0, 0x10FF,
            "Georgian", u8"Грузинский", {}, EcScript::Geor },
    /// @todo [semi-tofu] How to show filler 115F, 1160?
    { 0x1100, 0x11FF,
            "Hangul Jamo", u8"Хангыль\u00A0— чамо"sv,
            u8"<p>В хангыле (корейском алфавите) всего 51 буква ''(чамо)''. Блок намного больше: одна и та же буква "
                    "в начале и конце слога кодируется разными символами, к тому же в блоке много устаревших чамо."
                "<p>Чхосон{{-}}начальные буквы, чунсон{{-}}средние, чонсон{{-}}конечные."sv,
            EcScript::Hang },
    // Ethiopic OK
    { 0x1200, 0x137F,
            "Ethiopic", u8"Эфиопский", {}, EcScript::Ethi },
    // Ethiopic supplement OK
    { 0x1380, 0x139F,
            "Ethiopic Supplement", u8"Эфиопский дополнительный"sv,
            u8"<p>Буквы языка себат-бет гураге (Эфиопия, 1,5{{_}}млн, ''Ethnologue'' статус 5: развивающийся), "
                    "а также метки тонов."sv, EcScript::Ethi },
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
            "Tagalog", u8"Тагальский", {}, EcScript::Tglg },
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
    /// @todo [wrong glyph] 18A9
    { 0x1800, 0x18AF,
            "Mongolian", u8"Монгольский", {}, EcScript::Mong },
    // Canadian extended OK
    { 0x18B0, 0x18FF,
            "Unified Canadian Aboriginal Syllabics Extended",
            u8"Канадская слоговая расширенная"sv,
            u8"<p>Буквы для кри, оджибве ''(алгонкинского)'', кэрриер, дене ''(атабаскских)''."sv,
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
            "Khmer Symbols", u8"Кхмерские символы"sv,
            u8"<p>Первый символ блока {{sm|᧠}}{{-}}название восьмого месяца в буддийском солнечно-лунном календаре "
                        "(по-кхмерски ''asat'' или ''băthámsath'', санскрит ''ашадха'', приходится на июнь-июль). "
                    "Чтобы не{{_}}отставать от смены времён года, этот месяц повторяется дважды каждые два-три года. "
                    "Умляут 17D3 {{sm|◌៓}} некоторое время был [[pt:deprecated|запрещён]] в пользу данного символа, "
                        "да и в Юникоде{{_}}13.0 всё еще не{{_}}рекомендуется."
                "<p>Остальные{{-}}названия лунных дней от новолуния до полнолуния и обратно."sv,
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
            u8"Диакритические метки расширенные"sv,
            u8"<p>Используются в немецкой и шотландской диалектологии."sv,
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
    /// @todo [desc] Cyr C
    { 0x1C80, 0x1C8F,
            "Cyrillic Extended-C", u8"Кириллица расширенная C",
            {}, EcScript::Cyrl },
    /// @todo [semi-tofu] What to do with those Mtavruli?
    /// @todo [desc] Georgian extended
    { 0x1C90, 0x1CBF,
            "Georgian Extended", u8"Грузинский расширенный",
            {}, EcScript::Geor },
    // Sundanese suplement OK
    /// @todo [desc] Sundanese supplement
    { 0x1CC0, 0x1CCF,
            "Sundanese Supplement", u8"Сунданский дополнительный",
            {}, EcScript::Sund },
    /// @todo [tofu] 1CF7+, 4 chars
    /// @todo [desc] Vedic extensions
    { 0x1CD0, 0x1CFF,
            "Vedic Extensions", u8"Ведические символы", {}, EcScript::Deva },
    // Phonetic ext OK
    /// @todo [desc] Phonetic ext
    { 0x1D00, 0x1D7F,
            "Phonetic Extensions", u8"Фонетические расширения"sv },
    // Phonetic ext supp OK
    /// @todo [desc] Phonetic ext supp
    { 0x1D80, 0x1DBF,
            "Phonetic Extensions Supplement",
            u8"Фонетические расширения дополнительные"sv },
    /// @todo [semi-tofu] Win/Qt cannot render 1DD4..1DF4, 33 chars
    /// @todo [desc] Diacrit supp
    { 0x1DC0, 0x1DFF,
            "Combining Diacritical Marks Supplement",
            u8"Диакритические метки дополнительные"sv,
            {}, EcScript::NONE, EcFont::NOTO },
    // Lat ex OK
    /// @todo [desc] Lat ex
    { 0x1E00, 0x1EFF,
            "Latin Extended Additional",
            u8"Латиница расширенная дополнительная"sv },
    /// @todo [semi-tofu] Bad chars, but OK
    /// @todo [desc] Greek ex
    { 0x1F00, 0x1FFF,
            "Greek Extended", u8"Греческий расширенный"sv, {},
            EcScript::Grek, EcFont::NOTO },
    /// @todo [semi-tofu] What is two-point leader and what font to use?
    /// @todo [block] Link to ASCII
    { 0x2000, 0x206F,
            "General Punctuation", u8"Знаки препинания"sv,
            u8"<p>Пунктуация (лат. ''punctum'' «точка»){{-}}система знаков, подчёркивающих синтаксис и интонацию речи, "
                    "а также правила постановки их в тексте."
                "<p>Очевидно, основные знаки препинания (точка, запятая, двоеточие…) закодированы в ASCII и латинице-1. "
                    "А в блоке знаков препинания находятся так называемые ''типографские знаки'', "
                    "чтобы красиво оформить текст."sv,
            EcScript::NONE, EcFont::PUNCTUATION },
    // Sup/sub OK
    /// @todo [semi-tofu] Some are absent in W7, and Noto does fun
    /// @todo [block] Links to other blocks when they are ready
    { 0x2070, 0x209F,
            "Superscripts and Subscripts",
            u8"Верхние и нижние индексы"sv,
            u8"<p>Блок содержит надстрочные и подстрочные буквы, знаки и цифры, применяемые в математике и фонетике. "
                "<p>Ещё до распространения интернета выяснилось: бездумное копирование текста может привести "
                        "к тому, что число Авогадро «равняется» 6,632·1023. "
                    "Система (текстовый файл, интернет-форум) может вообще не поддерживать индексов, "
                        "и не стоит принуждать людей переходить на более сложные системы ради такой мелочи, "
                        "или имитировать вроде «6,022e23» или «6,022·10^23». "
                "<p>К тому же нарисованные типографом индексы обычно качественнее, чем синтезированные редактором документов."
                "<p>Многие из важных индексов есть в других местах: например, транспонирование матрицы Aᵀ{{-}}"
                        "в блоке фонетических расширений."sv },
    // Curr symbols OK
    { 0x20A0, 0x20CF,
            "Currency Symbols", u8"Символы валют"sv,
            u8"Символы валют делятся на две чёткие категории. Одна группа возникла в XVII–XIX{{_}}веке с развитием "
                    "делового языка: доллар {{sm|$}}, фунт {{sm|£}}, [[ps:Taml|тамильский]] знак рупии {{sm|௹}}. "
                    "Другая{{-}}принята государством с конца XX{{_}}века: евро {{sm|€}}, гривна {{sm|₴}}, "
                    "рубль {{sm|₽}}, рупия {{sm|₹}}."sv },
    /// @todo [semi-tofu] Need SVG images, existing are BAD!
    { 0x20D0, 0x20FF,
            "Combining Diacritical Marks for Symbols",
            u8"Диакритические метки для символов"sv,
            u8"Практически все из этих символов нормально работают только если в шрифте есть подходящая [[pt:ligature|лигатура]]{{-}}"
                "например, {{sm| 1⃣}}. Образцы всех символов, кроме двух, пришлось рисовать в SVG."sv,
            EcScript::NONE, EcFont::NOTO_SYMBOL2 },
    // Letterlike OK
    { 0x2100, 0x214F,
            "Letterlike Symbols", u8"Буквоподобные символы"sv,
            u8"<p>Математические, технические и деловые символы буквенного происхождения."
                "<p>Ажурный шрифт {{sm|ℕℤℚℝℂ}} происходит из жирного, записанного мелом на доске. "
                    "Распространился он со знаменитым учебником по комплексному анализу 1965{{_}}года Ганнинга и Росси."
                "<p>Символ {{sm|ℵ}} для мощности множеств предложен Г.{{_}}Кантором."sv },
    // Number forms OK
    /// @todo [block] Latin-1
    { 0x2150, 0x218F,
            "Number Forms", u8"Числовые формы"sv,
            u8"<p>Монолитные обыкновенные дроби и римские цифры. "
                    "Дробь {{sm|↉}} используется в бейсболе, "
                        "{{sm|↊}} и {{sm|↋}}{{-}}в двенадцатеричной системе счисления. "
                "<p>Ещё три дроби закодированы в латинице-1."sv },
    // Arrows OK
    { 0x2190, 0x21FF,
            "Arrows", u8"Стрелки"sv,
            u8"Стрелки{{-}}символы, напоминающие стрелу и указывающие направление. "
                    "Вот немногие примеры использования стрелок:<br>"
                    "• для маркировки клавиш;<br>"
                    "• на дорожных знаках, транспортных схемах;<br>"
                    "• в математике{{-}}отображение, следствие, предел, вектор, монотонность;<br>"
                    "• в химии{{-}}реакция, идущая в одну или обе стороны, выделяется газ, выпадает осадок."sv },
    // Math op OK
    { 0x2200, 0x22FF,
            "Mathematical Operators", u8"Математические знаки"sv,
            u8"<p>Простейшие математические знаки: математические операции, сравнение, интегралы, теория множеств. "
                    "Система математических обозначений складывалась веками и отличается для разных стран, "
                        "областей математики и даже математических школ. "
                    "Например, плюс в круге {{sm|⊕}} может означать исключающее ИЛИ в формальной логике "
                        "и машинное сложение в численных методах. "
                    "Для равенства по определению используют {{sm|≔}}, {{sm|≝}}, {{sm|≡}}." },
    // Misc tech OK
    { 0x2300, 0x23FF,
            "Miscellaneous Technical", u8"Разные технические"sv,
            u8"<p>Символы, используемые в черчении, электронике, программировании, стоматологии, поэзии, химии, оптике, "
                        "пользовательских интерфейсах."
                "<p>Также символы, используемые в системах компьютерной математики вроде MathType (он же ''Microsoft Equation'') "
                        "и TᴇX (читается «тех») для создания многоэтажных скобок."sv,
            EcScript::NONE, EcFont::NOTO_SYMBOL },
    // Control pictures OK
    { 0x2400, 0x243F,
            "Control Pictures", u8"Изображения управляющих"sv,
            u8"<p>Сами по себе управляющие символы не имеют никакого графического представления. "
                    "В этом блоке собраны условные знаки, которыми изображают управляющие символы. "
                    "В первую очередь нам важен символ {{sm|␣}}, изображающий [[pt:space|пробел]]."
                "<p>Несколько изображений управляющих символов есть в и других блоках: {{sm|↵←⌫}}."sv,
            EcScript::NONE, EcFont::NOTO_SYMBOL2_BIGGER },
    // OCR/MICR OK
    { 0x2440, 0x245F,
            "Optical Character Recognition",
            u8"Оптическое распознавание символов"sv,
            u8"<p>Оптическое и магнитное распознавание символов служит для автоматического ввода документов, "
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
                    "«Электронные жучки» и других."sv,
            EcScript::NONE, EcFont::PHAISTOS_DISC },
    // Enclosed alnum OK
    { 0x2460, 0x24FF,
            "Enclosed Alphanumerics", u8"Обрамлённые буквы и цифры"sv,
            u8"По данным английской Википедии, эти символы в основном предназначены для списков{{-}}"
                        "«наполовину маркированных, наполовину нумерованных». "
                    "{{sm|Ⓜ}} используется как [[pt:emoji|эмодзи]] для метро."sv },
    // Box drawing OK
    { 0x2500, 0x257F,
            "Box Drawing", u8"Рисование рамок"sv,
            u8"<p>Элементы [[pt:pseudographics|псевдографики]], используемые для рисования рамок. "
                    "Как и любая псевдографика, чаще всего используются в моноширинных шрифтах."
                "<p>В IBM-совместимых ПК были две разновидности линий{{-}}одиночная и двойная. "
                    "Юникод добавляет к ним тонкую, пунктирную и тонкую пунктирную."
                "<p>В компьютере BBC{{_}}Master есть закруглённые уголки."sv },
    // Block elements OK
    { 0x2580, 0x259F,
            "Block Elements", u8"Блочные элементы"sv,
            u8"<p>Элементы [[pt:pseudographics|псевдографики]], используемые для сплошной заливки. "
                    "Как и любая псевдографика, чаще всего используются в моноширинных шрифтах. "
                    "В пропорциональных шрифтах символы родом с IBM{{-}}половинная заливка {{sm|▀}}, "
                        "растровая заливка {{sm|░}}{{-}}обычно вытянуты по вертикали, а четвертная заливка с "
                        "ZX{{_}}Spectrum {{sm|▚}}{{-}}квадратная."
                "<p>У псевдографики, как правило, отсутствует межсимвольный интервал, "
                    "потому «Юникодия» использует псевдографические символы для показа ширины [[pt:space|пробелов]]."
                "<p>Часть псевдографических символов со старых компьютеров{{-}}например, 2×3 блока{{-}}"
                    "здесь отсутствуют и добавлены только в 2020."sv,
            EcScript::NONE, EcFont::BLOCK },
    // Geometric OK
    { 0x25A0, 0x25FF,
            "Geometric Shapes", u8"Геометрические фигуры"sv,
            u8"<p>Геометрические фигуры используются для маркировки кнопок, грубого рисования пользовательских интерфейсов. "
                    "Вот, например, четыре кнопки ''Sony Playstation'' {{sm|🞩○△□}}{{-}}три из них взяты из этого блока. "
                    "Символ {{sm|◌}} используется в Юникоде для маркировки [[pt:combining|комбинирующих]] знаков."sv },
    // Misc sym OK
    { 0x2600, 0x26FF,
            "Miscellaneous Symbols", u8"Разные символы"sv,
            u8"<p>В этом разделе есть [[pt:emoji|эмодзи]], знаки из астрономии, астрологии, настольных игр, восточной эзотерики, "
                        "религии, политики и просто для украшения печатного текста. "
                    "Многие символы взяты из японской кодировки ARIB STD B24, используемой для передачи новостей, погоды, "
                        "информации о дорожных заторах через цифровое телевидение."sv },
    // Dingbats OK
    { 0x2700, 0x27BF,
            "Dingbats", u8"Украшения"sv,
            u8"<p>Символы, чаще всего используемые для украшения печатного текста. "
                    "Блок содержат несколько [[pt:emoji|эмодзи]], что вполне оправдано: в интернете это будут цветные картинки, "
                    "а печатник оформит публикацию чёрно-белым шрифтом."sv },
    // Misc math A OK
    { 0x27C0, 0x27EF,
            "Miscellaneous Mathematical Symbols-A",
            u8"Разные математические символы A"sv,
            u8"<p>Несколько символов, используемых в геометрии, формальной логике, верификации программ, теории баз данных "
                    "и других разделах математики. Также западный символ деления столбиком."sv,
            EcScript::NONE, EcFont::MATH },
    // Arrows A OK
    /// @todo [desc] Arrows A
    { 0x27F0, 0x27FF,
            "Supplemental Arrows-A", u8"Дополнительные стрелки A"sv },
    // Braille OK
    { 0x2800, 0x28FF,
            "Braille Patterns", u8"Шрифт Брайля", {}, EcScript::Brai },
    // Arrows B OK
    /// @todo [desc] Arrows B
    { 0x2900, 0x297F,
            "Supplemental Arrows-B", u8"Дополнительные стрелки B"sv },
    // Misc math B OK
    { 0x2980, 0x29FF,
            "Miscellaneous Mathematical Symbols-B",
            u8"Разные математические символы B"sv,
            u8"<p>Различные математические символы: скобки, углы, пустые множества, «ящики с усами»…"sv },
    // Supp math ops OK
    { 0x2A00, 0x2AFF,
            "Supplemental Mathematical Operators",
            u8"Дополнительные математические знаки"sv,
            u8"<p>Символы, используемые в математическом анализе, функциях комплексного переменного, Z-нотации, "
                    "теории баз данных, теории управления, формальной логике и других разделах математики."sv,
            EcScript::NONE, EcFont::MATH },
    // Misc syms & arrows OK
    /// @todo [desc] Misc syms & arrows
    { 0x2B00, 0x2BFF,
            "Miscellaneous Symbols and Arrows",
            u8"Разные символы и стрелки"sv },
    // Glagolitic OK
    { 0x2C00, 0x2C5F,
            "Glagolitic", u8"Глаголица"sv, {}, EcScript::Glag },
    // Latin C OK
    /// @todo [desc] Latin C
    { 0x2C60, 0x2C7F,
            "Latin Extended-C", u8"Латиница расширенная C", {}, EcScript::Latn },
    // Coptic OK
    { 0x2C80, 0x2CFF,
            "Coptic", u8"Коптский", {}, EcScript::Copt },
    // Georgian supp OK
    /// @todo [desc] Georgian supp
    { 0x2D00, 0x2D2F,
            "Georgian Supplement", u8"Грузинский дополнительный", {}, EcScript::Geor },
    // Tifinagh OK
    { 0x2D30, 0x2D7F,
            "Tifinagh", u8"Тифинаг (берберский)", {}, EcScript::Tfng },
    // Ethiopic ex OK
    /// @todo [desc] Ethiopic ex
    { 0x2D80, 0x2DDF,
            "Ethiopic Extended", u8"Эфиопский расширенный"sv, {}, EcScript::Ethi },
    /// @todo [semi-tofu] Cyr ex A renders badly
    /// @todo [desc] Cyr ex A
    { 0x2DE0, 0x2DFF,
            "Cyrillic Extended-A", u8"Кириллица расширенная A"sv, {},
            EcScript::Cyrl, EcFont::NOTO },
    // Supp punct OK
    /// @todo [desc] Supp punct
    { 0x2E00, 0x2E7F,
            "Supplemental Punctuation", u8"Дополнительные знаки препинания"sv },
    { 0x2E80, 0x2EFF,
            "CJK Radicals Supplement", u8"ККЯ дополнительные ключи"sv, {}, EcScript::Hani },
    { 0x2F00, 0x2FDF,
            "Kangxi Radicals", u8"Ключи канси", {}, EcScript::Hani },
    { 0x2FF0, 0x2FFF,
            "Ideographic Description Characters",
            u8"Символы структуры иероглифов"sv },
    /// @todo [tofu] 302E/F, these are Hangul
    { 0x3000, 0x303F,
            "CJK Symbols and Punctuation",
            u8"ККЯ символы и знаки препинания"sv, {}, EcScript::Hani },
    { 0x3040, 0x309F,
            "Hiragana", u8"Хирагана", {}, EcScript::Hira },
    { 0x30A0, 0x30FF,
            "Katakana", u8"Катакана", {}, EcScript::Kana },
    { 0x3100, 0x312F,
            "Bopomofo", u8"Бопомофо (чжуинь)", {}, EcScript::Bopo },
    { 0x3130, 0x318F,
            "Hangul Compatibility Jamo", u8"Хангыль\u00A0— совместимые чамо",
            {}, EcScript::Hang },
    { 0x3190, 0x319F,
            "Kanbun", u8"Камбун (подсказки к древнеяпонским иероглифам)" },
    { 0x31A0, 0x31BF,
            "Bopomofo Extended", u8"Бопомофо (чжуинь) расширенный" },
    { 0x31C0, 0x31EF,
        "CJK Strokes", u8"Черты ККЯ"sv, {}, EcScript::Hani },
    { 0x31F0, 0x31FF,
            "Katakana Phonetic Extensions", u8"Катакана\u00a0— фонетические расширения" },
    { 0x3200, 0x32FF,
            "Enclosed CJK Letters and Months", u8"Обрамлённые буквы и месяцы ККЯ",
            {}, EcScript::Hani },
    { 0x3300, 0x33FF,
            "CJK Compatibility", u8"ККЯ\u00A0— символы совместимости"sv,
            {}, EcScript::Hani },
    /// @todo [tofu] 10 hieroglyphs from 2018 and 2020, 4BD6+
    { 0x3400, 0x4DBF,
            "CJK Unified Ideographs Extension A",
            u8"ККЯ иероглифы расширение A"sv, {}, EcScript::Hani },
    { 0x4DC0, 0x4DFF,
            "Yijing Hexagram Symbols", u8"Гексаграммы И цзин (Книги перемен)"sv },
    /// @todo [tofu] 18 hieroglyphs from 2018 and 2020, 9FEB+
    { 0x4E00, 0x9FFF,
            "CJK Unified Ideographs", u8"ККЯ иероглифы", {}, EcScript::Hani },
    { 0xA000, 0xA48F,
            "Yi Syllables", u8"И\u00A0— слоги", {}, EcScript::Yiii },
    { 0xA490, 0xA4CF,
            "Yi Radicals", u8"И\u00A0— компоненты", {}, EcScript::Yiii },
    { 0xA4D0, 0xA4FF,
            "Lisu", u8"Лису (алфавит Фрейзера)", {}, EcScript::Lisu },
    { 0xA500, 0xA63F,
            "Vai", u8"Ваи", {}, EcScript::Vaii },
    { 0xA640, 0xA69F,
            "Cyrillic Extended-B", u8"Кириллица расширенная B", {}, EcScript::Cyrl },
    { 0xA6A0, 0xA6FF,
            "Bamum", u8"Бамум", {}, EcScript::Bamu },
    { 0xA700, 0xA71F,
            "Modifier Tone Letters", u8"Символы изменения тона"sv },
    /// @todo [semi-tofu] A7F8, A7F9 render badly
    /// @todo [desc] Latin ex D
    { 0xA720, 0xA7FF,
            "Latin Extended-D", u8"Латиница расширенная D", {}, EcScript::Latn },
    { 0xA800, 0xA82F,
            "Syloti Nagri", u8"Силхети-нагари", {}, EcScript::Sylo },
    { 0xA830, 0xA83F,
            "Common Indic Number Forms", u8"Общеиндийские числовые символы",
            u8"Такие знаки присутствуют в источниках XVI{{_}}века, и применяются по сей день "
                "в Северной Индии, Пакистане и Непале для записи дробей: например, "
                "размера, веса или цены.",
            EcScript::Deva },
    { 0xA840, 0xA87F,
            "Phags-pa", u8"Монгольское квадратное (Пагба-ламы)", {}, EcScript::Phag },
    { 0xA880, 0xA8DF,
            "Saurashtra", u8"Саураштра", {}, EcScript::Saur },
    /// @todo [tofu] Another PitA in W7, A8FC+
    { 0xA8E0, 0xA8FF,
            "Devanagari Extended", u8"Деванагари расширенный", {}, EcScript::Deva },
    { 0xA900, 0xA92F,
            "Kayah Li", u8"Кая-ли", {}, EcScript::Kali },
    { 0xA930, 0xA95F,
            "Rejang", u8"Реджанг", {}, EcScript::Rjng },
    { 0xA960, 0xA97F,
            "Hangul Jamo Extended-A", u8"Хангыль\u00A0— расширенные чамо А",
            {}, EcScript::Hang },
    { 0xA980, 0xA9DF,
            "Javanese", u8"Яванский", {}, EcScript::Java },
    { 0xA9E0, 0xA9FF,
            "Myanmar Extended-B", u8"Бирманский расширенный B", {}, EcScript::Mymr },
    { 0xAA00, 0xAA5F,
            "Cham", u8"Чамский", {}, EcScript::Cham },
    { 0xAA60, 0xAA7F,
            "Myanmar Extended-A", u8"Бирманский расширенный А", {}, EcScript::Mymr },
    { 0xAA80, 0xAADF,
            "Tai Viet", u8"Тай-вьет", {}, EcScript::Tavt },
    // Meetei ex OK
    /// @todo [desc] Meetei ex
    { 0xAAE0, 0xAAFF,
            "Meetei Mayek Extensions", u8"Манипури\u00A0— расширения" },
    { 0xAB00, 0xAB2F,
            "Ethiopic Extended-A", u8"Эфиопский расширенный А", {}, EcScript::Ethi },
    // Latin ex E OK
    /// @todo [desc] Latin ex E
    { 0xAB30, 0xAB6F,
            "Latin Extended-E", u8"Латиница расширенная E", {}, EcScript::Latn },
    // Cherokee supp OK
    { 0xAB70, 0xABBF,
            "Cherokee Supplement", u8"Чероки дополнительный",
            u8"<p>В этом блоке находятся строчные буквы письменности чероки, кроме шести, закодированных в основном блоке.",
            EcScript::Cher },
    // Meetei OK
    { 0xABC0, 0xABFF,
            "Meetei Mayek", u8"Манипури (мейтей-майек)", {}, EcScript::Mtei },
    { 0xAC00, 0xD7AF,
            "Hangul Syllables", u8"Хангыль\u00A0— слоги", {}, EcScript::Hang },
    { 0xD7B0, 0xD7FF,
            "Hangul Jamo Extended-B", u8"Хангыль\u00A0— расширенные чамо B",
            {}, EcScript::Hang },
    { 0xF900, 0xFAFF,
            "CJK Compatibility Ideographs", u8"ККЯ совместимые иероглифы",
            {}, EcScript::Hani },
    // Alphabetic presentation forms OK
    { 0xFB00, 0xFB4F,
            "Alphabetic Presentation Forms", u8"Алфавитные формы начертания"sv,
            u8"<p>Стандартные [[pt:ligature|лигатуры]] для [[ps:Latn|латиницы]], [[ps:Armn|армянского]] и [[ps:Hebr|иврита]]."sv },
    { 0xFB50, 0xFDFF,
            "Arabic Presentation Forms-A", u8"Арабские формы начертания А",
            {}, EcScript::Arab },
    // Variation selectors OK
    { 0xFE00, 0xFE0F,
            "Variation Selectors", u8"Селекторы начертания"sv,
            u8"<p>Чаще всего (например, в браузерах) тот или иной вариант написания символа включают, выбирая другой шрифт "
                    "или включая функции OpenType. Юникод позволяет это ещё одним методом: созданием лигатур из символа и селектора."
                "<p>[[ps:Latn|Латиница]], [[ps:Mymr|бирманский и ещё несколько письменностей используют VS1. "
                    "[[ps:Hani|Китайские иероглифы]] используют VS1…VS3. [[pt:emoji|Эмодзи]] можно сделать цветными (VS16) "
                    "и текстовыми (VS15)."sv },
    // Vertical forms OK
    { 0xFE10, 0xFE1F,
            "Vertical Forms", u8"Вертикальные формы"sv,
            u8"<p>[[ps:Hani|Китайский язык]] изначально писался по столбцам справа налево, и сейчас так пишут "
                    "в художественных целях: стихи, открытки, вывески."
                "<p>Блок кодирует символы из китайской кодировки GB{{_}}18030 (2005), предназначенные для вертикального письма."sv,
            EcScript::Hani },
    // Combining half marks OK
    { 0xFE20, 0xFE2F,
            "Combining Half Marks", u8"Комбинирующие полузнаки"sv,
            u8"<p>Части диакритических знаков, охватывающих несколько символов. "
                    "Например, можно накрыть три буквы {{sm|a︤c︦e︥}} одной чертой. "
                    "В большинстве шрифтов реализованы плохо, и автор «Юникодии» специально подобрал "
                    "три удачных символа. А вот церковнославянское сокращение {{sm|Х︮с︯}} выглядит плохо."sv,
            EcScript::NONE, EcFont::NOTO },
    { 0xFE30, 0xFE4F,
            "CJK Compatibility Forms", u8"ККЯ совместимые формы", {}, EcScript::Hani },
    { 0xFE50, 0xFE6F,
            "Small Form Variants", u8"Малые формы знаков препинания" },
    { 0xFE70, 0xFEFF,
            "Arabic Presentation Forms-B", u8"Арабские формы начертания B", {}, EcScript::Arab },
    /// @todo [font, BMP] CJK probably has no such chars, find smth siutable for W7 and 10
    { 0xFF00, 0xFFEF,
            "Halfwidth and Fullwidth Forms",
            u8"Полуширинные и полноширинные формы",
            u8"<p>В ККЯ иероглиф традиционно занимает одну клетку сетки, а европейская буква{{-}}"
                    "половину клетки. Отсюда необычное начертание текста на китайских товарах "
                    "с плохо подобранными шрифтами: так, в интернете нашумели стельки «Атлетизм». "
                "<p>В этом блоке собраны «неправильные» символы, занимающие "
                    "половину клетки вместо целой и наоборот.",
            EcScript::NONE, EcFont::CJK },
    // Specials OK
    { 0xFFF0, 0xFFFF,
            "Specials", u8"Спецсимволы" },
    // Linear B syll OK
    { 0x10000, 0x1007F,
            "Linear B Syllabary", u8"Линейное письмо Б\u00A0— слоги", {}, EcScript::Linb },
    // Linear B hier OK
    { 0x10080, 0x100FF,
            "Linear B Ideograms", u8"Линейное письмо Б\u00A0— иероглифы", {}, EcScript::Linb },
    // Aegean numbers OK
    { 0x10100, 0x1013F,
            "Aegean Numbers", u8"Эгейские цифры"sv,
            u8"<p>Очень простая десятичная система счисления, использовавшаяся минойской и крито-микенской цивилизациями."
                    "Замечена в линейном письме [[ps:Lina|А]] и [[ps:Linb|Б]]. "
                    "В [[ps:Cypr|кипрском]] письме была обнаружена единственная табличка с числом «100»."sv,
            EcScript::Linb },
    // Greek numbers OK
    { 0x10140, 0x1018F,
        "Ancient Greek Numbers", u8"Древнегреческие цифры"sv,
        u8"<p>Аттическая система счисления была акрофонической, то есть цифрами служили первые буквы слов:<br>"
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
                "Во Франции ''литрóн''{{-}}мера объёма сыпучих веществ, отсюда ''литр''."sv  },
    // Ancient symbols OK
    /// @todo [desc] Ancient symbols
    { 0x10190, 0x101CF,
            "Ancient Symbols", u8"Древние символы"sv },
    // Phaistos Disc OK
    { 0x101D0, 0x101FF,
        "Phaistos Disc", u8"Фестский диск"sv,
        u8"<p>Фестский диск{{-}}артефакт крито-минойской (прото-греческой) культуры. Диск, сделанный из глины "
                "без гончарного круга, найден в 1908 на Крите, имеет диаметр 160{{_}}мм и толщину 20{{_}}мм. "
                "Датируется 2100–1100{{_}}до н.э., и содержит две надписи, сделанных по спирали "
                "неизвестной письменностью с помощью штампов. "
                "Выставлен в Археологическом музее Ираклиона (Крит)."
            "<p>Общепринята гипотеза, что текст читается от края к центру (справа налево). "
                "Для написания слева направо многие из символов отражены: так, в оригинале ПЕШЕХОД "
                "идёт направо. "
                "Никто не знает, критская ли это письменность и к какому типу относится. "
                "В числе необычных гипотез{{-}}список владений критского царя и игра-ходилка."
            "<p>В 2006 Фестский диск предложен в Реестр искусственных письменностей на место "
                "E6D0–E6FF; порядок символов тот же (по Артуру Эвансу, 1905, по убыванию частоты). "
                "С появлением Фестского диска в настоящем Юникоде диапазон расформирован. "
                "Два символа из Реестра в Юникод не вошли и берутся из латиницы: | и ¦. "
                "Названия символов{{-}}по Луи Годáру (1995)."sv,
            EcScript::NONE, EcFont::PHAISTOS_DISC },
    // Lycian OK
    { 0x10280, 0x1029F,
            "Lycian", u8"Ликийский"sv, {}, EcScript::Lyci },
    // Carian OK
    { 0x102A0, 0x102DF,
            "Carian", u8"Карийский"sv, {}, EcScript::Cari },
    // Coptic epact OK
    { 0x102E0, 0x102FF,
        "Coptic Epact Numbers", u8"Коптские курсивные цифры"sv,
        u8"<p>Здесь слово ''epact'' (греч. ἐπακτός) означает «заимствованные», а не «епакта» (формулы для перевода из солнечного "
                "календаря в лунный) или «пасхалия» (формулы для расчёта пасхи). "
                "И действительно, эти цифры, будучи потомками обычной [[ps:Copt|коптской]] записи "
                "(которая, в свою очередь, позаимствована из [[ps:Grek|греческого]]), широко использовались в копто-арабском мире "
                "с X по XIX век; в частности, в астрономических таблицах ≈1800. Запись слева направо."
            "<p>Из-за такого необычного использования эти цифры закодированы отдельным блоком, а не как вариант коптского."
            "<p>В изображениях символов вы увидите серьёзное расхождение с таблицами Юникода: специалист по древним "
                "письменностям ''Anshuman Pandey'' обвёл старые чернильные символы, а проект "
                "''Google Noto'' попытался повторить росчерки постоянной шириной."sv,
            EcScript::NONE, EcFont::NOTO_SYMBOL2 },
    // Old Italic OK
    { 0x10300, 0x1032F,
            "Old Italic", u8"Этрусский", {}, EcScript::Ital },
    // Gothic OK
    { 0x10330, 0x1034F,
            "Gothic", u8"Готский", {}, EcScript::Goth },
    { 0x10350, 0x1037F,
            "Old Permic", u8"Древнепермский (анбур)", {}, EcScript::Perm },
    { 0x10380, 0x1039F,
            "Ugaritic", u8"Угаритский", {}, EcScript::Ugar },
    { 0x103A0, 0x103DF,
            "Old Persian", u8"Древнеперсидский", {}, EcScript::Xpeo },
    // Deseret OK
    { 0x10400, 0x1044F,
            "Deseret", u8"Дезеретский (мормонский)", {}, EcScript::Dsrt },
    // Shavian OK
    /// @todo [script] Shavian
    { 0x10450, 0x1047F,
            "Shavian", u8"Алфавит Б.Шоу", {}, EcScript::Shaw },
    { 0x10480, 0x104AF,
            "Osmanya", u8"Исмания (сомалийский)", {}, EcScript::Osma },
    { 0x104B0, 0x104FF,
            "Osage", u8"Осейдж", {}, EcScript::Osge },
    { 0x10500, 0x1052F,
            "Elbasan", u8"Эльбасанский албанский", {}, EcScript::Elba },
    { 0x10530, 0x1056F,
            "Caucasian Albanian", u8"Агванский (Кавказская Албания)", {}, EcScript::Aghb },
    { 0x10600, 0x1077F,
            "Linear A", u8"Линейное письмо А", {}, EcScript::Lina },
    { 0x10800, 0x1083F,
            "Cypriot Syllabary", u8"Кипрский", {}, EcScript::Cprt },
    { 0x10840, 0x1085F,
            "Imperial Aramaic", u8"Имперский арамейский", {}, EcScript::Armi },
    { 0x10860, 0x1087F,
            "Palmyrene", u8"Пальмирский", {}, EcScript::Palm },
    { 0x10880, 0x108AF,
            "Nabataean", u8"Набатейский", {}, EcScript::Nbat },
    { 0x108E0, 0x108FF,
            "Hatran", u8"Хатранский", {}, EcScript::Hatr },
    { 0x10900, 0x1091F,
            "Phoenician", u8"Финикийский", {}, EcScript::Phnx },
    { 0x10920, 0x1093F,
            "Lydian", u8"Лидийский", {}, EcScript::Lydi },
    { 0x10980, 0x1099F,
            "Meroitic Hieroglyphs", u8"Мероитские иероглифы", {}, EcScript::Mero },
    { 0x109A0, 0x109FF,
            "Meroitic Cursive", u8"Мероитский курсив", {}, EcScript::Mero },
    { 0x10A00, 0x10A5F,
            "Kharoshthi", u8"Кхароштхи", {}, EcScript::Khar },
    { 0x10A60, 0x10A7F,
            "Old South Arabian", u8"Древний южноаравийский", {}, EcScript::Sarb },
    { 0x10A80, 0x10A9F,
            "Old North Arabian", u8"Древний североаравийский", {}, EcScript::Narb },
    { 0x10AC0, 0x10AFF,
            "Manichaean", u8"Манихейский", {}, EcScript::Mani },
    { 0x10B00, 0x10B3F,
            "Avestan", u8"Авестийский", {}, EcScript::Avst },
    { 0x10B40, 0x10B5F,
            "Inscriptional Parthian", u8"Парфянские надписи", {}, EcScript::Prti },
    { 0x10B60, 0x10B7F,
            "Inscriptional Pahlavi", u8"Пехлевийские надписи", {}, EcScript::Phli },
    { 0x10B80, 0x10BAF,
            "Psalter Pahlavi", u8"Пехлевийский псалтырь", {}, EcScript::Phlp },
    { 0x10C00, 0x10C4F,
            "Old Turkic", u8"Тюркские руны", {}, EcScript::Orkh },
    { 0x10C80, 0x10CFF,
            "Old Hungarian", u8"Венгерские руны", {}, EcScript::Hung },
    { 0x10D00, 0x10D3F,
            "Hanifi Rohingya", u8"Ханифи (рохинджа)", {}, EcScript::Rohg },
    { 0x10E60, 0x10E7F,
            "Rumi Numeral Symbols", u8"Цифры руми" },
    { 0x10E80, 0x10EBF,
            "Yezidi", u8"Езидский", {}, EcScript::Yezi },
    { 0x10F00, 0x10F2F,
            "Old Sogdian", u8"Древнесогдийский", {}, EcScript::Sogo },
    { 0x10F30, 0x10F6F,
            "Sogdian", u8"Согдийский", {}, EcScript::Sogd },
    { 0x10FB0, 0x10FDF,
            "Chorasmian", u8"Хорезмийский", {}, EcScript::Chrs },
    { 0x10FE0, 0x10FFF,
            "Elymaic", u8"Элимайский", {}, EcScript::Elym },
    { 0x11000, 0x1107F,
            "Brahmi", u8"Брахми", {}, EcScript::Brah },
    { 0x11080, 0x110CF,
            "Kaithi", u8"Кайтхи", {}, EcScript::Kthi },
    { 0x110D0, 0x110FF,
            "Sora Sompeng", u8"Соранг-сомпенг", {}, EcScript::Sora },
    { 0x11100, 0x1114F,
            "Chakma", u8"Чакма", {}, EcScript::Cakm },
    { 0x11150, 0x1117F,
            "Mahajani", u8"Махаджани", {}, EcScript::Mahj },
    { 0x11180, 0x111DF,
            "Sharada", u8"Шарада", {}, EcScript::Shrd },
    { 0x111E0, 0x111FF,
            "Sinhala Archaic Numbers", u8"Старые сингальские цифры" },
    { 0x11200, 0x1124F,
            "Khojki", u8"Ходжки", {}, EcScript::Khoj },
    { 0x11280, 0x112AF,
            "Multani", u8"Мултани", {}, EcScript::Mult },
    { 0x112B0, 0x112FF,
            "Khudawadi", u8"Кхудабади", {}, EcScript::Sind },
    { 0x11300, 0x1137F,
            "Grantha", u8"Грантха", {}, EcScript::Gran },
    { 0x11400, 0x1147F,
            "Newa"sv, u8"Неварский", {}, EcScript::Newa },
    { 0x11480, 0x114DF,
            "Tirhuta", u8"Тирхута (митхилакшар)", {}, EcScript::Tirh },
    { 0x11580, 0x115FF,
            "Siddham", u8"Сиддхаматрика", {}, EcScript::Sidd },
    { 0x11600, 0x1165F,
            "Modi", u8"Моди", {}, EcScript::Modi },
    { 0x11660, 0x1167F,
            "Mongolian Supplement", u8"Монгольский дополнительный", {}, EcScript::Mong },
    { 0x11680, 0x116CF,
            "Takri", u8"Такри", {}, EcScript::Takr },
    { 0x11700, 0x1173F,
            "Ahom", u8"Ахом", {}, EcScript::Ahom },
    { 0x11800, 0x1184F,
            "Dogra", u8"Догра", {}, EcScript::Dogr },
    { 0x118A0, 0x118FF,
            "Warang Citi", u8"Варанг-кшити", {}, EcScript::Wara },
    { 0x11900, 0x1195F,
            "Dives Akuru", u8"Дивес-акуру (древнемальдивский)", {}, EcScript::Diak },
    { 0x119A0, 0x119FF,
            "Nandinagari", u8"Нандинагари", {}, EcScript::Nand },
    { 0x11A00, 0x11A4F,
            "Zanabazar Square", u8"Монгольское квадратное (Дзанабадзара)", {}, EcScript::Zanb },
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
            "Makasar", u8"Старый макасарский", {}, EcScript::Maka },
    /// @tofo [tofu] Only one char, and it’s tofu
    { 0x11FB0, 0x11FBF,
            "Lisu Supplement", u8"Лису дополнительный", {}, EcScript::Lisu },
    /// @todo [tofu] Tamil supp
    { 0x11FC0, 0x11FFF,
            "Tamil Supplement", u8"Тамильский дополнительный", {}, EcScript::Taml },
    { 0x12000, 0x123FF,
            "Cuneiform", u8"Клинопись", {}, EcScript::Xsux },
    { 0x12400, 0x1247F,
            "Cuneiform Numbers and Punctuation",
            u8"Клинописные цифры и знаки препинания", {}, EcScript::Xsux },
    { 0x12480, 0x1254F,
            "Early Dynastic Cuneiform",
            u8"Раннединастическая клинопись", {}, EcScript::Xsux },
    // Egyptian hiero OK
    { 0x13000, 0x1342F,
            "Egyptian Hieroglyphs", u8"Египетские иероглифы", {}, EcScript::Egyp },
    // Egyptian hiero format OK
    { 0x13430, 0x1343F,
            "Egyptian Hieroglyph Format Controls",
            u8"Форматирующие символы для египетских иероглифов", {}, EcScript::Egyp },
    /// @todo [tofu] Anatolyan hiero
    { 0x14400, 0x1467F,
            "Anatolian Hieroglyphs", u8"Анатолийские иероглифы", {}, EcScript::Hluw },
    // Bamum supplement OK
    { 0x16800, 0x16A3F,
            "Bamum Supplement", u8"Бамум дополнительный", {}, EcScript::Bamu },
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
    { 0x16FE0, 0x16FFF,
            "Ideographic Symbols and Punctuation",
            u8"Символы и знаки препинания для иероглифов", {}, EcScript::Hani },
    { 0x17000, 0x187FF,
            "Tangut", u8"Тангутский", {}, EcScript::Tang },
    { 0x18800, 0x18AFF,
            "Tangut Components", u8"Элементы тангутского письма", {}, EcScript::Tang },
    { 0x18B00, 0x18CFF,
            "Khitan Small Script", u8"Малое киданьское письмо", {}, EcScript::Kits },
    { 0x18D00, 0x18D8F,
            "Tangut Supplement", u8"Тангутский дополнительный", {}, EcScript::Tang },
    { 0x1B000, 0x1B0FF,
            "Kana Supplement", u8"Кана дополнительная"sv },
    { 0x1B100, 0x1B12F,
            "Kana Extended-A", u8"Кана расширенная A"sv },
    { 0x1B130, 0x1B16F,
            "Small Kana Extension", u8"Маленькие символы каны", {},
            EcScript::NONE, EcFont::HENTAIGANA },
    { 0x1B170, 0x1B2FF,
            "Nushu", u8"Нюй-шу", {}, EcScript::Nshu },
    { 0x1BC00, 0x1BC9F,
            "Duployan", u8"Стенография Дюплойе", {}, EcScript::Dupl },
    { 0x1BCA0, 0x1BCAF,
            "Shorthand Format Controls",
            u8"Форматирующие символы стенографии"sv },
    /// @todo [semi-tofu] Increase size very much!!
    /// @todo [desc] Byzantine music
    { 0x1D000, 0x1D0FF,
            "Byzantine Musical Symbols",
            u8"Византийские музыкальные символы"sv,
            {},
            EcScript::NONE, EcFont::MUSIC },
    /// @todo [tofu] How to show chars like NULL NOTE HEAD?
    { 0x1D100, 0x1D1FF,
            "Musical Symbols", u8"Музыкальные символы"sv,
            u8"<p>С X{{_}}века появилась нотная запись, похожая на современную. "
                    "Ноты были квадратными, потому что рисовались жирным пером. "
                    "Для певцов делали 4 линии на строку, для инструментов больше. "
                    "Абсолютной привязки к частотам не было. "
                    "Круглые ноты появились в попытке бегло нарисовать ноту пером средней толщины, тем более такая нота "
                        "может передавать какую-то информацию вроде длительности. "
                    "И поныне производят такие перья для музыкантов."
                "<p>До современного вида ноты дошли в XVII{{_}}веке с появлением равнотемперированного строя, "
                    "позволившего переносить музыку на другие инструменты, играть оркестрами."
                "<p>Юникод как таковой не умеет писать музыку. "
                    "Однако заманчиво использовать имеющиеся типографские движки для хранения нот и их частей в шрифтах, "
                        "чтобы упрощать нотное ПО, сокращать нотные PDF. "
                    "В 2013 вышел открытый стандарт ''Standard Music Font Layout'' (SMuFL), расширяющий блок "
                        "до более чем 2600 символов с использованием позиций для [[pt:private|личного пользования]]."
                "<p>Также в этом блоке есть несколько символов из устаревших музыкальных нотаций."sv,
            EcScript::NONE, EcFont::MUSIC },
    /// @todo [desc] Greek music
    { 0x1D200, 0x1D24F,
            "Ancient Greek Musical Notation",
            u8"Древнегреческая музыкальная нотация"sv,
            {},
            EcScript::NONE, EcFont::MUSIC },
    { 0x1D2E0, 0x1D2FF,
            "Mayan Numerals", u8"Цифры майя" },
    { 0x1D300, 0x1D35F,
            "Tai Xuan Jing Symbols",
            u8"Символы Тайсюаньцзин (Канона великой тайны)" },
    { 0x1D360, 0x1D37F,
            "Counting Rod Numerals", u8"Счётные палочки"sv,
            u8"<p>Палочками учат считать детей. "
                    "Древние китайцы, начиная с Воюющих царств (V{{_}}век) до XVI{{_}}века пользовались настоящей позиционной "
                    "системой счисления из палочек, раскладываемых на счётной доске. "
                    "Была запись для дробей и даже отрицательных чисел (палочками другого цвета)."
                "<p>Вертикальные палочки означали единицы, сотни, десятки тысяч…, горизонтальные{{-}}десятки, тысячи, сотни тысяч… "
                    "Иногда наоборот{{-}}именно в таком виде палочки вошли в Юникод. "
                    "Символы счётных палочек часто писали и на бумаге."
                "<p>Если нужно что-то сосчитать и запомнить количество, используют счётные символы. "
                    "Европейские пятёрки {{sm|𝍸}} чаще всего ассоциируются с тюрьмой. "
                    "Китайцы пишут иероглиф <font size='+1'>正</font>. Лесники считают десятками, этот метод описан у Я.И.{{_}}Перельмана, "
                    "но отсутствует в Юникоде."sv,
            EcScript::NONE, EcFont::NOTO_SYMBOL2_BIGGER },
    /// @todo [block] Letterlike
    { 0x1D400, 0x1D7FF,
            "Mathematical Alphanumeric Symbols",
            u8"Математические буквы и цифры"sv,
            u8"<p>Математики обозначают различными шрифтами объекты, с которыми имеют дело:<br>"
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
                    "С расширением Юникода за базовую плоскость добавили полный латинский алфавит и арабские цифры разными шрифтами."sv },
    { 0x1D800, 0x1DAAF,
            "Sutton SignWriting", u8"Жестовое письмо Саттон", {}, EcScript::Sgnw },
    { 0x1E000, 0x1E02F,
            "Glagolitic Supplement", u8"Глаголица дополнительная", {}, EcScript::Glag },
    { 0x1E100, 0x1E14F,
            "Nyiakeng Puachue Hmong", u8"Ньякэ пуацы хмонг", {}, EcScript::Hmnp },
    { 0x1E2C0, 0x1E2FF,
            "Wancho", u8"Ванчо", {}, EcScript::Wcho },
    { 0x1E800, 0x1E8DF,
            "Mende Kikakui", u8"Кикакуи (менде)", {}, EcScript::Mend },
    { 0x1E900, 0x1E95F,
            "Adlam", u8"Адлам", {}, EcScript::Adlm },
    { 0x1EC70, 0x1ECBF,
            "Indic Siyaq Numbers", u8"Индийские цифры сийяк" },
    { 0x1ED00, 0x1ED4F,
            "Ottoman Siyaq Numbers", u8"Османские цифры сийяк" },
    { 0x1EE00, 0x1EEFF,
            "Arabic Mathematical Alphabetic Symbols",
            u8"Арабские математические алфавитные символы" },
    { 0x1F000, 0x1F02F,
            "Mahjong Tiles", u8"Фишки маджонга" },
    { 0x1F030, 0x1F09F,
            "Domino Tiles", u8"Кости домино" },
    { 0x1F0A0, 0x1F0FF,
            "Playing Cards", u8"Игральные карты" },
    { 0x1F100, 0x1F1FF,
            "Enclosed Alphanumeric Supplement",
            u8"Обрамлённые буквы и цифры дополнительные" },
    { 0x1F200, 0x1F2FF,
            "Enclosed Ideographic Supplement",
            u8"Обрамлённые иероглифы дополнительные"sv },
    { 0x1F300, 0x1F5FF,
            "Miscellaneous Symbols and Pictographs",
            u8"Разные символы и пиктограммы"sv,
            u8"<p>В блоке содержатся [[pt:emoji|эмодзи]] (как японские, так и международные), "
                    "а также типографские узоры и символы из пользовательских интерфейсов."
                "<p>Примечательна судьба символа «пистолет» {{em|🔫}}{{-}}с подачи ''Apple'' большинство производителей "
                    "делают его игрушечным для снижения сетевых угроз." },
    { 0x1F600, 0x1F64F,
            "Emoticons", u8"Смайлики" },
    { 0x1F650, 0x1F67F,
            "Ornamental Dingbats", u8"Печатные узоры" },
    { 0x1F680, 0x1F6FF,
            "Transport and Map Symbols",
            u8"Транспортные и картографические символы" },
    { 0x1F700, 0x1F77F,
            "Alchemical Symbols", u8"Алхимические символы" },
    { 0x1F780, 0x1F7FF,
            "Geometric Shapes Extended",
            u8"Геометрические фигуры расширенные" },
    { 0x1F800, 0x1F8FF,
            "Supplemental Arrows-C", u8"Стрелки дополнительные C" },
    { 0x1F900, 0x1F9FF,
            "Supplemental Symbols and Pictographs",
            u8"Символы и пиктограммы дополнительные"sv },
    /// @todo [wrong glyph] 1FA38, 1FA3E — replace with SVG
    { 0x1FA00, 0x1FA6F,
            "Chess Symbols", u8"Шахматные символы",
            u8"<p>В сказочной композиции в эпоху наборного шрифта за неимением другого использовали обычные повёрнутые фигуры. "
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
                "<p>Также в этом блоке есть фишки китайской игры сянци."sv },
    { 0x1FA70, 0x1FAFF,
            "Symbols and Pictographs Extended-A",
            u8"Символы и пиктограммы расширенные А"sv },
    // Legacy OK
    { 0x1FB00, 0x1FBFF,
            "Symbols for Legacy Computing",
            u8"Символы со старых компьютеров"sv,
            u8"<p>Символы с разных компьютеров и систем 1970-х и 80-х:<br>"
                    "• Amstrad CPC{{-}}успешный британский ПК;<br>"
                    "• Mattel Aquarius{{-}}гонконго-американский ПК, устаревший ещё при появлении;<br />"
                    "• Atari 400, 800 и ST{{-}}серия успешных американских ПК;<br />"
                    "• Tandy/RadioShack TRS-80 (Mark 1 и Color){{-}}успешные американские ПК;<br>"
                    "• Oric{{-}}британский ПК, получивший определённую популярность во Франции и известный по болгарскому "
                        "клону «Правец-8D»;<br>"
                    "• Apple IIc и более поздние (кодировка ''MouseText''){{-}}сверхуспешные американские ПК;<br>"
                    "• Commodore PET и 64{{-}}сверхуспешные американские ПК;<br>"
                    "• Sinclair ZX80 и ZX81{{-}}успешные британские ПК;<br>"
                    "• MSX, он же КУВТ Ямаха{{-}}японский открытый стандарт ПК;<br>"
                    "• Acorn Archimedes ''(RISC OS)''{{-}}прогрессивный британский ПК, сметённый IBM-совместимыми;<br>"
                    "• телетекст{{-}}система передачи текста в полях гашения телевизионного сигнала;<br>"
                    "• Minitel{{-}}французская терминальная служба;<br>"
                    "• и, по случайному совпадению, «Корвет»{{-}}советский технологический/учебный ПК."
                "<p>Из-за политики Консорциума не включать товарные знаки в блоке отсутствуют два яблока Apple. "
                    "Часть знаков с этих компьютеров уже есть в других блоках Юникода{{-}}в некоторых шрифтах оказывается, "
                    "что символы псевдографики не стыкуются с символами из данного блока: {{sm|█🬗}}."sv,
            EcScript::NONE, EcFont::NOTO_SYMBOL2_BIGGER },
    { 0x20000, 0x2A6DF,
            "CJK Unified Ideographs Extension B",
            u8"ККЯ иероглифы расширение B", {}, EcScript::Hani },
    { 0x2A700, 0x2B73F,
            "CJK Unified Ideographs Extension C",
            u8"ККЯ иероглифы расширение C", {}, EcScript::Hani },
    { 0x2B740, 0x2B81F,
            "CJK Unified Ideographs Extension D",
            u8"ККЯ иероглифы расширение D", {}, EcScript::Hani },
    { 0x2B820, 0x2CEAF,
            "CJK Unified Ideographs Extension E",
            u8"ККЯ иероглифы расширение E", {}, EcScript::Hani },
    { 0x2CEB0, 0x2EBEF,
            "CJK Unified Ideographs Extension F",
            u8"ККЯ иероглифы расширение F", {}, EcScript::Hani  },
    { 0x2F800, 0x2FA1F,
            "CJK Compatibility Ideographs Supplement",
            u8"ККЯ совместимые иероглифы дополнение", {}, EcScript::Hani },
    { 0x30000, 0x3134F,
            "CJK Unified Ideographs Extension G",
            u8"ККЯ иероглифы расширение G", {}, EcScript::Hani },
    { 0xE0000, 0xE007F,
            "Tags", u8"Тэги" },
    // Var sel supp OK
    { 0xE0100, 0xE01EF,
            "Variation Selectors Supplement", u8"Селекторы начертания дополнительные",
            u8"Дополнительные селекторы начертания используются только с [[ps:Hani|китайскими иероглифами]], чтобы дать тот или иной "
                "вариант начертания."sv },
    //{ 0xE0100, 0xE01EF, "Check for error"sv },
};

constinit const uc::TermCat uc::termCats[] {
    { u8"Кодирование"sv },
    { u8"Сериализация"sv },
    { u8"Классификация письменностей"sv },
    { u8"Классификация символов"sv },
    { u8"Алгоритмы Юникода"sv },
};
static_assert (std::size(uc::termCats) == static_cast<size_t>(uc::EcTermCat::NN));

constinit const uc::Term uc::terms[] {
    { "abjad", EcTermCat::SCRIPT_CLASS,
      u8"консонантная письменность, абджад"sv, u8"consonant script, abjad"sv,
        u8"<p>Письменность, где записываются только согласные звуки. "
                "Гласные либо опускаются полностью, либо записываются опциональными огласовками. "
                "Название «абджад» происходит от самоназвания [[ps:Arab|арабского]] алфавита{{-}}أبجد ''абджадия''."
            "<p>Несмотря на очевидные недостатки, у консонантной письменности есть и преимущества.<br>"
            "• В семитских языках{{-}}[[ps:Phnx|финикийском]], [[ps:Hebr|иврите]], арабском{{-}}корнем является костяк согласных, "
                    "а гласные играют роль суффиксов. "
                "Так, в арабском ''джихадом'' занимается ''моджахед'' (или, точнее, ''муджахид'').<br>"
            "• Легче позволяет собирать обширные империи{{-}}сглаживаются различия в диалектах.<br>"
            "• Реже нужно проводить реформы орфографии."sv },
    { "syllabic", EcTermCat::SCRIPT_CLASS,
      u8"слоговая азбука"sv, u8"syllabic script"sv,
        u8"<p>Письменность, в которой каждая буква записывает слог из гласной и согласной. "
                "Обычно содержат 80–120 букв. "
                "Разновидностью слоговой письменности является [[pt:abugida|абугида]], где у сходных слогов сходные буквы."
            "<p>Слоговую письменность легко изобрести из иероглифической: люди говорят не звуками, а слогами, "
                "и достаточно записать незнакомое слово подходящими односложными иероглифами. "
                "Подобный процесс замечен в [[ps:Xsux|клинописи]], [[ps:Egyp|египетских иероглифах]], [[ps:Hira|японском]]."
            "<p>Чтобы записывать стечения и одинокие согласные, есть несколько вариантов (возьмём для примера слово ''стол''):<br>"
                "• Придумать символ для одинокой буквы: с‑то‑л ([[ps:Cans|канадская слоговая]]).<br>"
                "• Сделать такой символ, приделав к слогу умлят ([[ps:virama|вираму]]): сⱥ‑то‑лⱥ "
                    "(многие [[pt:brahmic|брахмийские абугиды]], особенно малоразвитые).<br>"
                "• Некоторые брахмийские абугиды ([[ps:Laoo|лаосский]], [[ps:Tavt|тай-вьет]]) в XX{{_}}веке сделали реформу "
                    "и избавились от вирамы. Основной символ стал обозначать одинокую согласную.<br>"
                "• Запретить стечения и конечные, при заимствовании добавляя гласные: су‑то‑лу (японский). "
                    "''Японский позаимствовал бы су-то-ру, но это вопрос фонетики.''<br>"
                "• В определённых условиях гласная действительно бывает немой: сᵊ‑то‑лᵊ ([[ps:Guru|гурмукхи]], частично японский).<br>"
                "• Придумать [[pt:ligature|лигатуры]]: сᵀо‑л? (развитые брахмийские абугиды вроде [[ps:Deva|деванагари]]) "
                    "или с?‑тоᶺ ([[ps:Limb|лимбу]]).<br>"
                "• Отбросить конечную согласную: с?‑то ([[ps:Linb|микенский]], [[ps:Buhd|бухид]])."sv },
    { "abugida", EcTermCat::SCRIPT_CLASS,
      u8"абугида"sv, u8"abugida"sv,
        u8"<p>Исследователи не сходятся в определении абугиды, и самое широкое из них такое: "
                    "[[pt:syllabic|слоговая]] азбука, где у сходных слогов сходные символы."
            "<p>Абугидами '''точно являются''' [[pt:brahmic|брахмийские письменности]]."
            "<p>Абугидами '''являются у многих исследователей''' [[ps:Ethi|эфиопская]] и [[ps:Cans|канадская слоговая]] "
                    "письменности."
            "<p>Абугидой '''является в отдельных источниках''' [[ps:Thaa|тана]]."
            "<p>Абугидами '''не являются ни в одном источнике''' [[ps:Hang|хангыль]], [[ps:Yiii|и]]."
            "<p>Что интересно, название «абугида» происходит от эфиопского {{sm|አቡጊዳ}} (а-бу-ги-да){{-}}получается, некоторые исследователи "
                    "отказывают в праве называться абугидой той письменности, откуда это понятие пошло!"sv },
    { "brahmic", EcTermCat::SCRIPT_CLASS,
      u8"брахмийская абугида"sv, u8"Brahmic abugida"sv,
        u8"<p>Исследователи расходятся в определении [[pt:abugida|абугиды]]. "
                "Но в одном они единогласны: абугидами является '''группа письменностей, восходящих к [[ps:Brah|брахми]]'''."
            "<p>В семитских языках корнем слова являются согласные, а гласные{{-}}суффиксы и окончания: "
                    "в [[ps:Arab|арабском]] ''джихадом'' занимается ''моджахед'' (или, точнее, ''муджахид''). "
                "Для них естественно было ввести [[pt:abjad|консонантную]] письменность. "
                "Когда [[ps:Phnx|финикийское]] письмо пошло на восток и столкнулось с индоевропейскими языками, появилась "
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
                    "вроде [[ps:Sund|сунданского]]."sv },
    { "alphabet", EcTermCat::SCRIPT_CLASS,
      u8"алфавитная письменность"sv, u8"alphabet script"sv,
        u8"<p>Линейное письмо, записывающее гласные и согласные звуки. "
                "Первым известным алфавитом считается [[ps:Grek|греческий]]."
            "<p>Нелинейное письмо, в котором гласные и согласные объединяются в слоги со сложным порядком, "
                    "алфавитом считают или не все исследователи ([[ps:Thaa|тана]], [[ps:Hang|хангыль]]), "
                    "или вообще никто ([[ps:Laoo|лаосский]])."
            "<p>При всех достоинствах алфавитов у них есть серьёзный недостаток. "
                "Из-за сдвигов в произношении появляется сложная исторически обусловленная орфография, "
                    "которую время от времени нужно реформировать. "
                "Так, [[ps:Cyrl|русский]] ять '''ѣ''' был отдельным звуком, в XVIII{{_}}веке полностью исчезнувшим. "
                "Орфография [[ps:Latn|английского]] и французского во многом историческая."sv },
    { "arguable", EcTermCat::SCRIPT_CLASS,
      u8"письменность спорного типа"sv, {},
        u8"<p>Для двух письменностей источники не договорились, что они собой представляют."
            "<p>[[ps:Hang|Хангыль]] ''(корейский)''{{-}}[[pt:syllabic|слоговая]] ''(Юникод)'' "
                    "или [[pt:alphabet|алфавитная]] ''(Omniglot, английская Википедия)''."
            "<p>[[ps:Thaa|Тана]] ''(мальдивский)''{{-}}[[pt:abjad|консонантная]] ''(Omniglot)'', "
                    "[[pt:abugida|абугида]] ''(английская Википедия)'' или [[pt:alphabet|алфавитная]] ''(Юникод)''."sv },
    { "cjk", EcTermCat::SCRIPT_CLASS,
      u8"ККЯ = китайский, корейский, японский"sv, u8"CJK = Chinese, Japanese, Korean"sv,
        u8"<p>Группа письменностей, восходящих к [[ps:Hani|китайским иероглифам]]. "
                "К ним относятся также слоговые [[ps:Kana|катакана]] и [[ps:Hira|хирагана]], и "
                    "[[ps:Hang|хангыль]] [[pt:arguable|спорного типа]]."
            "<p>Особенности ККЯ:<br>"
                "• размашистое письмо кистью и тушью;<br>"
                "• символы моноширинные{{-}}независимо от количества черт, выровнены по невидимой сетке. "
                    "Заимствованные буквы часто полуширинные{{-}}занимают половину клетки;<br>"
                "• письмо изначально справа налево по столбцам; сейчас ''обычно'' слева направо по строкам. "
                    "И поныне манга (японские комиксы) читается справа налево;<br>"
                "• иероглифы несколько отличаются от страны к стране, Юникод оставляет эти тонкости за шрифтами;<br>"
                "• поскольку символы не вписываются в 256 [[pt:codepoint|позиций]], существовали специфичные кодировки для иероглифов."sv },

    ///// Char classes /////////////////////////////////////////////////////////

    { "acrophonic", EcTermCat::CHAR_CLASS,
      u8"акрофонический символ"sv, u8"acrophonic character"sv,
        u8"<p>Для буквы{{-}}называется словом, начинающимся с этой буквы, как в [[ps::Thai|тайском]], старой [[ps:Cyrl|кириллице]] "
                    "и [[ps:Phnx|финикийском]]."
            "<p>Для цифр и прочего{{-}}символ происходит от первой буквы понятия. "
                "Например, знак '''№''' происходит от лат. ''numero''." },
    { "virama", EcTermCat::CHAR_CLASS,
      u8"вирама"sv, u8"virama"sv,
        u8"<p>Вирама (санскрит «остановка»){{-}}знак в [[pt:brahmic|брахмийских]] абугидах, означающий «стереть гласную по умолчанию». "
                "Например, в [[ps:Deva|деванагари]]: <nobr>ка {{sm|क}} + {{sm|्}} = к {{sm|क्}}</nobr>."
            "<p>Реально в индийских языках символ называется ''халант'', ''халанта'', ''хосонто'', ''чандраккала''."
            "<p>Некоторые языки пользуются вирамой нечасто. Например:<br>"
                "• используют [[pt:ligature|лигатуры]]{{-}}деванагари {{sm|क्ष}} ксса. "
                    "Печатаются и кодируются они, как будто лигатуры нет: ка+вирама+сса;<br>"
                "• гласная по умолчанию редуцировалась{{-}}[[ps:Guru|пенджабский]];<br>"
                "• реформа орфографии лишила букву гласной по умолчанию{{-}}[[ps:Laoo|лаосский]];<br>"
                "• вирама может быть единым целым с согласной{{-}}[[ps:Talu|новое письмо лы]];<br>"
                "• не способны записывать «лишние» согласные{{-}}[[ps:Bugi|лонтара]]."
        u8"<p>Большинство вирам{{-}}[[pt:combining|комбинирующие метки]]. "
            "Но встречаются и [[pt:spacing|протяжённые]] ([[ps:Java|яванский]])."sv },
    { "graphic", EcTermCat::CHAR_CLASS,
      u8"графический символ, печатаемый символ"sv, u8"graphic character, printable character"sv,
        u8"<p>Символ, который печатается на экране или бумаге. "
                "Любой, кроме [[pt:control|управляющих]], [[pt:format|форматирующих]], разделителей строк и абзацев{{-}}"
                    "даже [[pt:space|пробел]]."
            "<p>Пробел нулевой ширины считается форматирующим, а не пробелом{{-}}а значит, он '''не''' графический."sv },
    { "deprecated", EcTermCat::CHAR_CLASS,
      u8"запрещённый символ"sv, u8"deprecated character"sv,
        u8"<p>Символ, чьё использование настоятельно не рекомендуется."
            "<p>Одна из гарантий [[pt:stability|стабильности]] Юникода: как только символ назначен, он не{{_}}сдвинется и не{{_}}исчезнет. "
                "Если символ оказывается неудачным, его запрещают. "
                "Причины запрета разные."
            "<p>В Консорциуме Юникода сидят люди западной культуры, и они могут лишь ссылаться на исследования специалистов по редким языкам{{-}}"
                    "так случилось для пяти символов [[ps:Arab|арабского]], [[ps:Tibt|тибетского]] и [[ps:Khmr|кхмерского]]."
            "<p>Две технические угловые скобки запретили, чтобы не путали с математическими. "
                "Также запрещены языковый тэг (внёс больше неразберихи, чем решил проблем), "
                    "шесть символов двунаправленного письма (предложен другой механизм) "
                    "и латинская [[pt:ligature|лигатура]] '''ŉ''' "
                        "(в голландском и африкаанс есть много похожих сокращений, никак не закодированных, "
                        "проблемы с [[pt:normalization|нормализацией]], "
                        "упразднили тип символов «буква со знаком»){{-}}итого 15 на Юникод{{_}}13.0."
            "<p>Известны несколько случаев возврата символов в обиход.<br>"
                "• Блок тэгов{{-}}использовался для указания языка. "
                    "Вернули для [[pt:emoji|эмодзи]] флагов государственных субъектов (например, Шотландии), "
                        "оставив под запретом один символ.<br>"
                "• 0340, 0341 (метки нисходящего/восходящего тона). "
                    "Предлагается замена в виде обычных умляутов à и á. "
                    "С Юникода 5.2 просто не{{_}}рекомендуются.<br>"
                "• 17D3 (кхмерский умляут ''bathamasat''). "
                    "С появлением [[pt:precomposed|монолитных]] символов лунного календаря в Юникоде 4.0 запретили. "
                    "С Юникода 5.2 просто не{{_}}рекомендуется{{-}}вероятно, из-за громоздкости календарной [[pt:ligature|лигатуры]]; "
                        "у автора «Юникодии» были проблемы с поиском шрифта, где лигатуры хорошо видны."
            "<p>Не путать с [[pt:obsolete|устаревшим]]."sv },
    { "combining", EcTermCat::CHAR_CLASS,
      u8"комбинирующая метка"sv, u8"combining mark"sv,
        u8"<p>Несамостоятельный символ, прикрепляемый к основному. Используется для:"
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
                    "В большинстве шрифтов этот метод реализован плохо."sv },
    { "ligature", EcTermCat::CHAR_CLASS,
      u8"лигатура"sv, u8"ligature"sv,
        u8"<p>Лигатура{{-}}это несколько символов в одном:"
            "<p>1. В типографике{{-}}символ, составленный из нескольких. Так, амперсанд '''&''' является "
                    "лигатурой латинского ''et{{-}}и''."
            "<p>2. В шрифтовом деле{{-}}символ, означающий последовательность из нескольких [[pt:codepoint|кодовых позиций]]. "
                "Так, в некоторых шрифтах набор «fi» пропечатывает лигатуру '''ﬁ'''. "
                "Много таких лигатур в [[pt:brahmic|брахмийских абугидах]]."
            "<p>Все лигатуры{{-}}[[pt:spacing|протяжённые]] символы."
            "<p>[[pt:casefolding|Свёртка регистра]] ''может'' раскрывать лигатуры."sv },
    { "precomposed", EcTermCat::CHAR_CLASS,
      u8"монолитный символ"sv, u8"precomposed character"sv,
        u8"<p>Символ и [[pt:combining|диакритическая метка]] в одном. "
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
                "См. также [[pt:normalization|Нормализация]]."sv },
    { "space", EcTermCat::CHAR_CLASS,
      u8"пробел"sv, u8"space character"sv, CT_SPACE },
    { "spacing", EcTermCat::CHAR_CLASS,
      u8"протяжённый символ"sv, u8"spacing character"sv,
        u8"<p>Символ, занимающий место в строке."
            "<p>Таковыми являются буквы, знаки препинания, [[pt:space|пробелы]], разные символы "
                    "и протяжённые [[pt:combining|комбинирующие метки]]."
            "<p>Таковыми не являются [[pt:control|управляющие]], [[pt:format|форматирующие]] символы и прочие типы меток."sv },
    { "control", EcTermCat::CHAR_CLASS,
      u8"управляющий символ"sv, u8"control character"sv, CT_CONTROL },
    { "obsolete", EcTermCat::CHAR_CLASS,
      u8"устаревший символ"sv, u8"obsolete character"sv,
        u8"<p>Символ, который вышел из использования. "
                "Например, [[ps:Cyrl|кириллический]] ять <big>'''ѣ'''</big>, превратившийся в Е."
            "<p>Не путать с [[pt:deprecated|запрещённым символом]]."sv },
    { "format", EcTermCat::CHAR_CLASS,
      u8"форматирующий символ"sv, u8"format character"sv, CT_FORMAT },
    { "number", EcTermCat::CHAR_CLASS,
      u8"числовой символ"sv, u8"number character"sv,
        u8"<p>Юникод делит числовые символы на три класса:"
         "<p>• Десятичные позиционные цифры{{-}}как привычные ASCII-цифры 0…9, так и цифры других письменностей "
                "([[ps:Deva|деванагари]] ०…९).<br>"
            "• Эти же цифры, используемые в особых контекстах{{-}}например, в верхних/нижних индексах.<br>"
            "• Прочие числовые символы: цифры недесятичных, непозиционных и частично позиционных систем счисления "
                "(римские, эгейские, счётные палочки), [[pt:precomposed|монолитные]] числа…"
            "<p>Символы, чьё основное предназначение нечисловое (например, славянские цифры вроде А̄) Юникод числами не считает." },
    { "emoji", EcTermCat::CHAR_CLASS,
      u8"эмодзи"sv, u8"emoji"sv,
        u8"<p>От японского {{sm|絵}} ''э{{-}}картинка'' + {{sm|文字}} ''модзи{{-}}знак, символ."
            "<p>Придуманы мобильным оператором NTT DoCoMo около 1999 для экономии места в SMS. "
                "Западные операторы начали разрабатывать свои наборы с 2004, и в 2010 кодировку DoCoMo внесли в Юникод 6.0. "
                "Многие эмодзи специфичны для Японии: кланяющийся бизнесмен {{em|🙇}}, знак начинающего водителя {{em|🔰}}… "
                "Другие изменили значение за пределами Японии: знак высшего балла {{em|💯}} превратился в «полностью согласен»."
            "<p>Все эмодзи являются [[pt:spacing|протяжёнными]] символами."sv },

    ///// Encoding /////////////////////////////////////////////////////////////

    { "ascii", EcTermCat::ENCODING, u8"ASCII"sv, {}, CT_ASCII },
    { "bmp", EcTermCat::ENCODING,
      u8"базовая многоязычная плоскость"sv, u8"BMP = Basic Multilingual Plane"sv,
        u8"<p>[[pt:plane|Плоскость]] Юникода с номером 0, содержащая символы 0000…FFFD. "
                "В базовой плоскости закодированы наиболее ходовые символы живых языков, "
                    "что позволяет обрабатывать эти языки прямо в [[pt:utf16|UTF-16]]. "
            "<p>До 1996 Юникод был двухбайтовым. "
                "Но к тому времени несколько прогрессивных языков и библиотек (Java, Windows{{_}}NT) успели перейти на Юникод, "
                    "что и привело к компромиссу в виде базовой плоскости и UTF-16."
            "<p>В Юникоде 14.0 нераспределёнными остались 16 позиций, 2FE0…2FEF."
            "<p>Базовая плоскость содержит [[pt:ascii|ASCII]], [[pt:latin1|латиницу-1]] и 6400 [[pt:private|личных]] символов."sv },
    { "stability", EcTermCat::ENCODING,
      u8"гарантии стабильности"sv, u8"stability guarantees"sv,
        u8"<p>Консорциум Юникода гарантирует, что не{{_}}изменит часть характеристик символов."
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
                    "и это единственный способ для буквы войти в регистровую пару."sv },
    { "noncharacter", EcTermCat::ENCODING,
      u8"зарезервирован как отсутствующий"sv, u8"non-character"sv, CT_NOCHAR },
    { "codepoint", EcTermCat::ENCODING,
      u8"кодовая позиция"sv, u8"code point"sv,
        u8"<p>Кодовая позиция{{-}}число или последовательность чисел, которые представляют некую концепцию "
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
            "<p>И наоборот: разные символы могут записываться одной кодовой позицией: сходные [[ps:Hani|иероглифы]] китайского и японского."sv },
    { "latin1", EcTermCat::ENCODING, u8"латиница-1"sv, u8"Latin-1"sv, CT_LATIN1 },
    { "plane", EcTermCat::ENCODING,
      u8"плоскость"sv, u8"plane"sv,
        u8"<p>Диапазон символов от *0000 до *FFFD. "
                "Последние две [[pt:codepoint|позиции]] каждой плоскости, *FFFE и *FFFF, "
                    "[[pt:noncharacter|зарезервированы как отсутствующие]]. "
                "Из-за ограничений [[pt:utf16|UTF-16]] в{{_}}Юникоде семнадцать плоскостей."
             "<p>• Плоскость {{№|0}} называется '''[[pt:bmp|базовой многоязычной]]''' и содержит большинство символов живых языков. "
                    "В Юникоде{{_}}14 остаются 16{{_}}нераспределённых позиций.<br>"
                "• Плоскость {{№|1}} называется '''дополнительной многоязычной''' и содержит символы редких и мёртвых языков, а также технические. "
                    "Заполнена менее чем на {{%|50}}.<br>"
                "• Плоскость {{№|2}} называется '''дополнительной иероглифической''' и содержит только [[ps:Hani|китайские иероглифы]]. "
                    "Заполнена на {{%|90}}.<br>"
                "• Плоскость {{№|3}} называется '''третичной иероглифической''', содержит только китайские иероглифы, "
                    "и заполнена на {{%|10}}.<br>"
                "• Плоскости 4…0D в Юникоде{{_}}14 не задействованы.<br>"
                "• Плоскость 0E называется '''дополнительной плоскостью специального назначения''' и содержит менее 500 специсмволов.<br>"
                "• Плоскости 0F и 10 отведены под [[pt:private|личное пользование]]."sv },
    { "private", EcTermCat::ENCODING,
      u8"символ для личного пользования"sv, u8"private-use character"sv,
        u8"<p>На собственные нужды всех заинтересованных отведены 6400 символов E000…F8FF из [[pt:bmp|базовой плоскости]], "
                    "а также 131068 символов в двух [[pt:plane|плоскостях]] 0F и 10, кроме *FFFE и *FFFF, "
                    "[[pt:noncharacter|зарезервированных как отсутствующие]]."
            "<p>Договорившись, люди могут закодировать в этих диапазонах всё, что хотят, в том числе символы, не подлежащие "
                    "внесению в Юникод: товарные знаки, вымышленные алфавиты, редкие символы и письменности, "
                    "тэги оформления, знаки подстановки. "
                "Консорциум Юникода никак не отвечает за это. "
                "Приведём лишь три распространённых применения личных символов:"
             "<p>• Производители ОС часто кодируют там свои эмблемы.<br>"
                "• Реестр искусственных письменностей (Under-ConScript Unicode Registry){{-}}более 60 "
                        "искусственных языков, письменностей из кино, литературы и игр. "
                    "Три из них{{-}}Фестский диск, [[ps:Shaw|алфавит Шоу]], [[ps:Dsrt|мормонский]]{{-}}проникли в Юникод.<br>"
                "• Standard Music Font Layout (SMuFL){{-}}открытый стандарт музыкальных шрифтов для сокращения нотных PDF."
            "<p>Неофициально в личное пользование под управляющие и форматирующие символы берут [[pt:control|управляющие]]: "
                    "в большинстве случаев они не могут находиться в тексте. "
                "Так, в компьютерной игре ''Doom'' (1993) символы с 1 по 4 служат для задания адресата сообщения."sv },

    ///// Algorithms ///////////////////////////////////////////////////////////

    { "bidi", EcTermCat::ALGORITHM,
      u8"двунаправленный текст"sv, u8"bidirectional text"sv,
        u8"<p>Текст, содержащий хотя бы один символ с написанием «справа налево». "
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
                            "ثث»"sv },
    { "normalization", EcTermCat::ALGORITHM,
      u8"нормализация"sv, u8"normalization"sv,
        u8"<p>Преобразование строк для нежёсткого сравнения. "
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
                "То есть вторая нормализация ничего не сделает."sv },
    { "casefolding", EcTermCat::ALGORITHM,
      u8"свёртка регистра"sv, u8"case folding"sv,
        u8"<p>Преобразование строки, используемое для (в основном) локаленезависимого сравнения строк без регистра. "
                "Бывает двух основных типов и одного дополнительного:<br>"
                "• Простая (без изменения длины): <big>ᾨ → ᾠ</big><br>"
                "• Полная (с раскрытием [[pt:ligature|лигатур]]): <big>ᾨ → ὠ ι</big><br>"
                "• Тюркская (специальные правила для турецкого и азербайджанского): <big>I → ı</big>"
            "<p>Обычно свёртка{{-}}это перевод в нижний регистр. "
                "Но для [[ps:Cher|чероки]] это перевод в верхний: заглавные буквы появились раньше."
            "<p>Свёртка идемпотентна: CF(CF(s))=CF(s), то есть вторая свёртка ничего не сделает."sv },

    ///// Serialization ////////////////////////////////////////////////////////

    { "serialization", EcTermCat::SERIALIZATION,
      u8"сериализация"sv, u8"serialization"sv,
        u8"<p>Метод преобразования данных (например, юникодного текста) в цепочку ''кодовых единиц''{{-}}байтов для [[pt:utf8|UTF-8]], "
                    "слов из двух байтов для [[pt:utf16|UTF-16]], слов из четырёх байтов для [[pt:utf32|UTF-32]], "
                    "символов ASCII для [[pt:utf7|UTF-7]] и [[pt:punycode|Punycode]]."
            "<p>Поскольку диски и каналы связи оперируют байтами, при передаче UTF-16/32 играет роль порядок байтов в слове. "
                "Соответственно появляются форматы UTF-16BE, UTF-32BE (начиная со старших байтов), "
                    "и UTF-16LE, UTF-32LE (начиная с младших)."
            "<p>[[pt:bom|Метка порядка байтов]] позволяет различать между этими форматами."sv },
    { "punycode", EcTermCat::SERIALIZATION,
      u8"Punycode"sv, {},
        u8"<p>Формат [[pt:serialization|сериализации]] короткой юникодной строки в цепочку печатных ASCII-символов. "
            "<p>Сначала записываются все ASCII-символы, затем после дефиса по сложной процедуре записывается, куда вставить "
                    "остальные символы.<br>"
                    "{{_}}{{_}}München = Mnchen-3ya"
            "<p>Если строка состоит из одних ASCII-символов, в конце просто добавляется дефис:<br>"
                    "{{_}}{{_}}test{{_}}={{_}}test-"
            "<p>Если ASCII-символов вообще нет, дефис в начале опускается:<br>"
                    "{{_}}{{_}}правда{{_}}={{_}}80aafi6cg"
            "<p>Используется только в системе доменных имён Интернета, при этом к международным именам добавляется «xn--»:<br>"
                    "{{_}}{{_}}münchen.example.com = xn--mnchen-3ya.example.com"sv },
    { "utf7", EcTermCat::SERIALIZATION,
      u8"UTF-7"sv, {},
        u8"<p>'''Unicode transformation format, 7-bit'''{{-}}"
                    "формат [[pt:serialization|сериализации]] Юникода в цепочку печатных ASCII-символов. "
            "<p>52 буквы, 10 цифр и '(),-./:? записываются как есть. "
                "Иногда могут записываться как есть и другие ASCII-символы, кроме ~\\+. "
                "Знак + можно кодировать как +- (плюс-минус). "
                "Остальные символы кодируются в [[pt:utf16|UTF-16BE]], переводятся в Base64 (62='''+''', 63='''/''') "
                    "и забираются в символы +…- (например, <nobr>'''за''' = +BDcEMA-</nobr>)."
            "<p>Если после кода идёт любой символ, кроме символов Base64 и дефиса, заключительный дефис тожно опустить. "
                "Если между двумя кодируемыми символами есть одинокий некодируемый, можно закодировать и его, "
                    "чтобы сэкономить байты."
            "<p>Формат UTF-7 не обладает хорошими свойствами [[pt:utf8|UTF-8]] и даже не одозначен: "
                    "один и тот же текст можно закодировать по-разному."
            "<p>Используется только в электронной почте, как альтернатива более громоздкому ''quoted-printable''. "
                "В протоколе IMAP для имён ящиков используется версия UTF-7, где вместо плюса '''+''' используется амперсанд '''&''', "
                    "63=запятая, и запрещается кодировать все символы ASCII, кроме амперсанда. "
                "Во Всемирной паутине UTF-7 прямо запрещён: если обмануть браузер, чтобы он принял текст за UTF-7, "
                    "можно сделать XSS-атаку."sv },
    { "utf8", EcTermCat::SERIALIZATION,
      u8"UTF-8"sv, {},
        u8"<p>'''Unicode transformation format, 8-bit'''{{-}}формат [[pt:serialization|сериализации]] Юникода в цепочку байтов. "
                "Придуман в 1992 в операционной системе Plan{{_}}9 для совместимости с имеющимся интернет-ПО. "
                "Каждая [[pt:codepoint|кодовая позиция]] записывается одной из таких последовательностей:<br>"
                "• 0xxx.xxxx<br>"
                "• 110x.xxxx{{_}}10xx.xxxx<br>"
                "• 1110.xxxx{{_}}10xx.xxxx{{_}}10xx.xxxx<br>"
                "• 1111.0xxx{{_}}10xx.xxxx{{_}}10xx.xxxx{{_}}10xx.xxxx"
            "<p>Свойства UTF-8:<br>"
                "• Теоретически может записывать 2{{_}}млн кодовых позиций последовательностями до 6 байт. "
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
                "Как формат программного интерфейса используется в Linux, старых кроссплатформенных библиотеках вроде OpenSSL."sv },
    { "utf16", EcTermCat::SERIALIZATION,
      u8"UTF-16"sv, {},
        u8"<p>'''Unicode transformation format, 16-bit'''{{-}}формат [[pt:serialization|сериализации]] Юникода в цепочку двухбайтовых "
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
            "<p>Как формат межпрограммного обмена используется в основном в двоичных файлах родом с Windows (например, шрифтах TrueType). "
                "Таким файлам метка порядка байтов не нужна; в текстовых файлах она FE{{_}}FF для UTF-16BE и FF{{_}}FE для UTF-16LE. "
                "Как формат программного интерфейса используется в Windows, Java, Qt."sv },
    { "utf32", EcTermCat::SERIALIZATION,
      u8"UTF-32"sv, {},
        u8"<p>'''Unicode transformation format, 32-bit''', он же '''UCS-4'''{{-}}"
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
                "Как формат программного интерфейса используется в новых версиях Unix-подобных ОС, в том числе MacOS."sv },
    { "bom", EcTermCat::SERIALIZATION,
      u8"метка порядка байтов"sv, u8"BOM = byte order mark"sv,
        u8"<p>Символ U+FEFF. Его можно (не обязательно) ставить в начале юникодного текстового файла, чтобы указать "
                    "формат [[pt:serialization|сериализации]]. "
                "Закодированной метка порядка байтов будет выглядеть так:<br>"
                "• EF{{_}}BB{{_}}BF{{-}}[[pt:utf8|UTF-8]];<br>"
                "• FE{{_}}FF{{-}}[[pt:utf16|UTF-16BE]] (начиная со старшего байта);<br>"
                "• FF{{_}}FE, дальше что угодно, кроме 00{{_}}00{{-}}UTF-16LE (начиная с младшего байта);<br>"
                "• 00{{_}}00{{_}}FE{{_}}FF{{-}}[[pt:utf32|UTF-32BE]];<br>"
                "• FF{{_}}FE{{_}}00{{_}}00{{-}}UTF-32LE."
            "<p>[[pt:utf7|UTF-7]] и [[pt:punycode|Punycode]] не встречаются как формат текстовых файлов. "
                "UTF-32 встречается крайне редко."
            "<p>В двоичных файлах формат сериализации жёстко зафиксирован, и метка порядка байтов не нужна."sv },
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
        // Lookup table
        cps[cp.subj.val()] = &cp;
    }

    // Check blocks — they should have at least one char
    for (auto& v : blocks) {
        if (!v.firstAllocated)
            throw std::logic_error("Block w/o chars leaked into data!");
    }
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

}   // anon namespace


void uc::Font::load() const
{
    if (q.tempFont.id != FONT_NOT_INSTALLED)
        return;
    if (!fileName.empty()) {
        q.tempFont = installTempFontRel(fileName);
        //q.raw = std::make_unique<QRawFont>(expandTempFontName(fileName), 10);
    } else {
        q.tempFont.id = FONT_CHECKED;
    }
    q.families = toQList(family);
}


bool uc::Font::doesSupportChar(char32_t) const
{
    /// @todo [urgent] supportsChar
    load();
//    if (!q.raw)
//        return true;
//    return q.raw->supportsCharacter(static_cast<uint>(x));
    return false;
}


const QFont& uc::Font::get(std::unique_ptr<QFont>& font, int size) const
{
    if (!font) {
        load();
        auto& family1 = onlyFamily();
        font.reset(new QFont(family1, sizeAdjust.apply(size), QFont::Normal));
        if (family1.isEmpty())
            font->setFamilies(q.families);
        // Weight
        if (flags.have(Ffg::BOLD)) {
            font->setBold(true);
        } else if (flags.have(Ffg::SEMIBOLD)) {
            font->setWeight(QFont::DemiBold);
        } else if (flags.have(Ffg::LIGHT)) {
            font->setWeight(QFont::Light);
        }
        int strategy = QFont::PreferAntialias | QFont::PreferMatch | QFont::PreferFullHinting;
        font->setStyleStrategy(
                    static_cast<QFont::StyleStrategy>(strategy));
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
    if (isAbbreviated())
        return {};

    auto& fn = font(hint);
    auto style = fn.styleSheet;
    switch (ecCategory) {
    case EcCategory::MARK_ENCLOSING:
        //return { STUB_CIRCLE + QString(" ") + str::toQ(subj.ch32()), style };
    case EcCategory::MARK_NONSPACING:
        if (fn.flags.have(Ffg::STUB_VICEVERSA)) {
            return { ZWSP + str::toQ(subj.ch32()) + STUB_CIRCLE, style };
        }
        [[fallthrough]];
    case EcCategory::MARK_SPACING:
        // Brahmi scripts probably do fine
        if (script().ecType == EcScriptType::ABUGIDA_BRAHMI) {
            if (!fn.flags.have(Ffg::STUB_ON))
                break;
        } else {
            if (fn.flags.have(Ffg::STUB_OFF))
                break;
        }
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


QString uc::Cp::osProxy() const
{
    if (!isGraphical())
        return {};

    switch (category().upCat) {
    case UpCategory::MARK:
        // Brahmi scripts probably do fine
        if (script().ecType == EcScriptType::ABUGIDA_BRAHMI
                && !script().font().flags.have(Ffg::STUB_ON))
            break;
        return QChar(STUB_CIRCLE) + str::toQ(subj.ch32());
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


const uc::Font& uc::Cp::font(const Block*& hint) const
{
    // Priority: Script unless font is normal → block → block’s script
    if (ecScript != EcScript::NONE && ecScript != EcScript::Zinh) {
        auto& si = scriptInfo[static_cast<int>(ecScript)];
        if (si.ecFont != EcFont::NORMAL)
            return si.font();
    }
    hint = blockOf(subj, hint);
    return hint->font();
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


const uc::Term* uc::findTerm(std::string_view id)
{
    for (auto& v : terms) {
        if (v.key == id)
            return &v;
    }
    return nullptr;
}
