#include "UcData.h"

// Qt
#include <QFontDatabase>

// Libs
#include "i_TempFont.h"
#include "u_Strings.h"
#include "u_Qstrings.h"
#include "u_Iterator.h"
#include "Skin.h"

using namespace std::string_view_literals;
uc::Cp* uc::cps[N_CHARS];
const QString uc::Font::qempty;

constexpr QChar STUB_CIRCLE(0x25CC);
constexpr QChar ZWSP(0x200B);

constinit const uc::Font uc::fontInfo[static_cast<int>(EcFont::NN)] {
    /// @todo [tofu] 10E60, that’s Arabic too
    /// @todo [semi-tofu] Qt’s font matching algorithm is extremely poor!
    ///       If we’ve got circle in Cambria and umlaut in Noto → complete random!
    ///       e.g. 1DE0
    { FAMILY_DEFAULT "," FAMILY_BACKUP ",Segoe UI Emoji,Noto Sans Symbols,"
            "Noto Sans Symbols2,Segoe UI Historic", {} },  // Normal
    { "Noto Serif",                 {} },                                       // Noto
    { "Segoe UI Emoji,Noto Sans Symbols", {}, {}, {}, 120_pc },                 // Noto symbol
    { "Noto Sans Symbols2",         {} },                                       // Noto symbol2
    { "Segoe UI Symbol",            {} },                                       // Symbol
    { "Segoe UI Historic",          {} },                                       // Historic
    { "Noto Sans Adlam",            "NotoSansAdlam-Regular.ttf" },              // Adlam
    { "Noto Serif Ahom",            "NotoSerifAhom-Regular.ttf" },              // Ahom
    /// @todo [font] Arabic has tall math operators ≈1EE50, what to do?
    { "Noto Naskh Arabic",          "NotoNaskhArabic-Regular.ttf" },            // Arabic
    { FAMILY_DEFAULT ",Sylfaen",    {} },                                       // Armenian
    { "Noto Sans Balinese",         "NotoSansBalinese-Regular.ttf", {}, "padding-bottom: 12%;", 90_pc }, // Balinese
    { "Noto Sans Bamum",            "NotoSansBamum-Regular.ttf", Ffg::STUB_VICEVERSA }, // Bamum
    { "Noto Sans Batak",            "NotoSansBatak-Regular.ttf" },              // Batak
    { "Noto Serif Bengali",         "NotoSerifBengali-Regular.ttf", {}, {}, 120_pc }, // Bengali
    { "Noto Sans Buginese",         "NotoSansBuginese-Regular.ttf", Ffg::STUB_ON }, // Buginese
    { "Noto Sans Buhid",            "NotoSansBuhid-Regular.ttf", Ffg::STUB_ON }, // Buhid
    { "Noto Sans Canadian Aboriginal", "NotoSansCanadianAboriginal-Regular.ttf" }, // Canadian aboriginal
    { "Noto Sans Cham",             "NotoSansCham-Regular.ttf" },               // Cham
    { "Noto Sans Cherokee",         "NotoSansCherokee-Regular.ttf" },           // Cherokee
        // CJK chars are square, and there’s always not enough detail → bigger
    { "Noto Serif CJK SC",          "NotoSerifCJK-Regular.ttc", Ffg::STD_IN_SAMPLES, {}, 125_pc }, // CJK
    { "Noto Serif Devanagari",      "NotoSerifDevanagari-Regular.ttf" },        // Devanagari
    { "Noto Serif Ethiopic",        "NotoSerifEthiopic-Regular.ttf" },          // Ethiopic
    { "Noto Serif Georgian",        "NotoSerifGeorgian-Regular.ttf" },          // Georgian
    { "Noto Sans Glagolitic",       "NotoSansGlagolitic-Regular.ttf" },
    //{ "Raavi,Nirmala UI",           {} },                                       // Gurmukhi
    { "Noto Sans Gurmukhi",         "NotoSansGurmukhi-Regular.ttf" },           // Gurmukhi
    { "Noto Sans Hanunoo"sv,        "NotoSansHanunoo-Regular.ttf" },
    { "Noto Serif Hebrew"sv,        "NotoSerifHebrew-Regular.ttf" },
    { "Noto Sans Javanese",         "NotoSansJavanese-Regular.ttf" },           // Javanese
    { "Noto Serif Kannada",         "NotoSerifKannada-Light.ttf", Ffg::LIGHT, {}, 110_pc }, // Kannada
    { "Noto Sans Kayah Li"sv,       "NotoSansKayahLi-Regular.ttf" },            // Kayah Li
    { "Noto Sans Tai Tham"sv,       "NotoSansTaiTham-Regular.ttf", Ffg::STUB_ON },    // Lanna
    { "SengBuhan",                  "sengbuhan.ttf", Ffg::STUB_ON, "padding-top: 12%;" },   // Lao
    { "Noto Sans Lepcha",           "NotoSansLepcha-Regular.ttf" },
    { "Noto Sans Limbu",            "NotoSansLimbu-Regular.ttf" },
    { "Noto Sans Lisu",             "NotoSansLisu-Regular.ttf" },
    { "Noto Sans Malayalam",        "NotoSansMalayalam-Light.ttf", Ffg::LIGHT }, // Malayalam
    { "Noto Sans Mandaic",          "NotoSansMandaic-Regular.ttf" },
    { "Noto Sans MeeteiMayek"sv,    "NotoSansMeeteiMayek-Regular.ttf" },
    { "Noto Sans Mongolian"sv,      "NotoSansMongolian-Regular.ttf" },
    { "Noto Sans Myanmar",          "NotoSansMyanmar-Regular.ttf" },
    { "Noto Sans NKo",              "NotoSansNKo-Regular.ttf" },                // N’Ko
    { "Noto Sans Nushu",            "NotoSansNushu-Regular.ttf" },              // Nushu
    { "Noto Sans Ogham",            "NotoSansOgham-Regular.ttf" },              // Ogham
    { "Noto Sans Ol Chiki",         "NotoSansOlChiki-Regular.ttf" },            // Ol Chiki
    { "Microsoft PhagsPa",          {} },                                       // Phags-Pa
    { "Noto Sans Symbols2",         {}, {}, {}, 150_pc },                       // Phaistos disc
    { "Noto Sans Rejang",           "NotoSansRejang-Regular.ttf" },             // Rejang
    { "Noto Sans Runic",            "NotoSansRunic-Regular.ttf" },              // Runic
    { "Noto Sans Samaritan",        "NotoSansSamaritan-Regular.ttf" },          // Samaritan
    { "Noto Sans Saurashtra",       "NotoSansSaurashtra-Regular.ttf", Ffg::STUB_ON }, // Saurashtra
    { "Noto Sans Sinhala",          "NotoSansSinhala-Light.ttf", Ffg::LIGHT },  // Sinhala
    { "Noto Sans Sundanese",        "NotoSansSundanese-Regular.ttf", Ffg::STUB_ON }, // Sundanese
    { "Noto Sans Syloti Nagri",     "NotoSansSylotiNagri-Regular.ttf", Ffg::STUB_ON }, // Syloti Nagri
    { "Serto Antioch Bible",        "SertoAntochBible_2020_Release.ttf", {}, {}, 120_pc }, // Syriac
    { "Noto Sans Tagalog"sv,        "NotoSansTagalog-Regular.ttf" },
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


constinit const std::u8string_view uc::TX_NOCHAR =
    u8"<p>Зарезервированы как отсутствующие (''non-characters'') 66 позиций: *FFFE и *FFFF во всех семнадцати плоскостях, "
            "а также FDD0…FDEF. Их никогда не будет в Юникоде."
        "<p>Эти коды можно использовать как коды ошибок, знаки подстановки, «утяжелитель» при сортировке "
            "или что-то другое, внутреннее для программы, перекодировать в UTF-8 или UTF-16."
        "<p>Их нельзя вводить с клавиатуры, использовать в межпрограммном обмене. "
            "Благодаря тому, что символа FFFE нет в Юникоде, работает метка порядка байтов. "
            "Если программа никак не использует данный код, она может обращаться с ним как с обычным "
            "неизвестным символом."sv;


constinit const uc::LangLife uc::langLifeInfo[static_cast<int>(EcLangLife::NN)] {
    { {} },
    { u8"живая <i>(основным языкам ничего не угрожает, как кириллица)</i>"sv },
    { u8"в опасности <i>(задавлена соседними языками, как чероки, или социальными потрясениями, как сирийская, "
        "или соседними письменностями, как ланна)</i>"sv },
    { u8"мёртвая <i>(скоро потеряет/недавно потеряла носителей, как нюй-шу)</i>"sv },
    { u8"историческая <i>(давно мертва, как глаголица)</i>"sv },
    { u8"новая <i>(появилась в новейшее время, как нко)</i>"sv },
    { u8"возрождённая <i>(как ахом)</i>"sv },
    //{ u8"Ошибка"sv },
};


constinit const uc::WritingDir uc::writingDirInfo[static_cast<int>(EcWritingDir::NN)] {
    { {} },
    { u8"→"sv },
    { u8"←"sv },
    { u8"→ <i>(исторически ←↓↓ по столбцам)</i>"sv },
    { u8"→, ↑"sv },
    { u8"↓↓→ по столбцам"sv },
    //{ u8"Ошибка"sv },
};


constinit const uc::ScriptType uc::scriptTypeInfo[static_cast<int>(EcScriptType::NN)] {
    { {} },
    { u8"неизвестно"sv },
    { u8"алфавитная <i>(линейное письмо из гласных и согласных, как кириллица)</i>"sv },
    { u8"псевдоалфавитная <i>(гласные и согласные, объединённые в слоги; "
            "название условное, классификация таких алфавитов разнится от исследователя "
            "к исследователю\u00A0— как тана)</i>"sv },
    { u8"консонантная <i>(согласные звуки, как иврит)</i>"sv },
    { u8"слоговая <i>(как японские катакана/хирагана)</i>"sv },
    { u8"алфавитно-слоговая <i>(как иберийский)</i>"sv },
    { u8"абугида <i>(слоговая, у сходных слогов похожие символы)</i>\u00A0— как эфиопский"sv },
    { u8"абугида <i>(слоговая, у сходных слогов похожие символы)</i>, "sv
        "основанная на брахми <i>(согласная буква означает слог с некоей гласной, обычно «а»; "
        "чтобы заменить, добавляется огласовка; чтобы получить простую согласную, "
        "добавляется знак «вирама»\u00A0— «убийца»)</i>"sv },
    { u8"иероглифическая <i>(символ означает понятие, как китайский)</i>"sv },
    { u8"слогоиероглифическая <i>(как линейное письмо Б)</i>"sv },
    { u8"код <i>(записывает информацию в другой форме, как шрифт Брайля)</i>"sv },
    { u8"символы <i>(как ноты)</i>"sv },
    { u8"настольная игра <i>(как игральные карты)</i>"sv },
    { u8"эмодзи <i>(японские SMS-картинки)</i>"sv },
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


constinit const uc::Category uc::categoryInfo[static_cast<int>(uc::EcCategory::NN)] {
    { UpCategory::CONTROL,      "Cc"sv,     u8"Управляющий"sv,
            u8"В первые годы, когда часто требовалось повторить сессию обмена вручную (перфоратором или консолью), "sv
            u8"управляющие символы играли роль форматов данных и сетевых протоколов. "sv
            u8"В настоящее время используются немногие из них:<br>"sv
            u8"• нулевой символ 00;<br>"sv
            u8"• конец файла 04 (Unix) и 1A (DOS/Windows);<br>"sv
            u8"• шаг назад (забой) 08;<br>"sv
            u8"• табуляция 09;<br>"sv
            u8"• перевод строки 0A;<br>"sv
            u8"• прогон листа 0C;<br>"sv
            u8"• возврат каретки 0D;<br>"sv
            u8"• Escape 1B."sv },
    { UpCategory::FORMAT,       "Cf"sv,     u8"Форматирующий"sv,
            u8"Используются в Юникоде для управления различными его алгоритмами, в первую очередь двунаправленным письмом "sv
            u8"(когда соседствуют текст слева направо и справа налево) и разбиением текста на строки."sv },
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
            u8"Например, [[pop_scr:Hebr|иврит]], [[pop_scr:Arab|арабская вязь]] и [[pop_scr:Hani|китайские иероглифы]]. Также особые символы вроде "sv
            u8"мужского/женского порядкового знака романских языков (1º\u00A0— первый, 1ª\u00A0— первая)."sv },
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
            u8"Небольшая группа символов. Наиболее известный из них\u00A0— знак подчёркивания."sv },
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
            u8"Валютный символ\u00A0— важная часть имиджа страны, и даже у монголов есть тугрик ₮, напоминающий могильный крест. "sv
            u8"Артемий Лебедев в конце 1990-х говорил, что рублю не нужен особый знак, "sv
            u8"но впоследствии именно его пиар сделал знак ₽ официальным."sv },
    { UpCategory::SYMBOL,       "Sk"sv,     u8"Символ/модифицирующий"sv,
            u8"Символы вроде крышки ^, внешне совпадающие с комбинирующими метками, но лишённые особых свойств. "
                "А также некоторые другие символы.</p>"
            "<p>Многие из таких символов родом с пишущих машинок: чтобы нарисовать, например, ударение, надо было "
                "вернуть каретку на шаг назад и выбить ударение в том же знакоместе. Литера рисовалась так, чтобы один "
                "символ годился на роль апострофа и ударения.</p>", },
    { UpCategory::SYMBOL,       "Sm"sv,     u8"Символ/математический"sv,
            u8"Изначально для математики использовались несколько разных систем, наиболее известные из них\u00A0— TᴇX (читается «тех») "sv
            u8"и MathType (он же Microsoft Equation). Юникод даёт надежду, что в компьютерной математике появится какая-то стандартизация\u00A0— "sv
            u8"а также ограниченная возможность писать формулы в системах общего назначения вроде интернет-форумов."sv },
    { UpCategory::SYMBOL,       "So"sv,     u8"Символ/другой"sv,
            u8"Юникод содержит множество разных символов, в том числе коммерческие, технические и эмодзи."sv },
    { UpCategory::SEPARATOR,    "Zl"sv,     u8"Разделитель/строк"sv,
            u8"Единственный символ 2028"sv },
    { UpCategory::SEPARATOR,    "Zp"sv,     u8"Разделитель/абзацев"sv,
            u8"Единственный символ 2029"sv },
    { UpCategory::SEPARATOR,    "Zs"sv,     u8"Разделитель/пробел"sv,
            u8"В хорошо проработанной типографике "sv
            u8"пустого места столько, сколько нужно: 146%\u00A0— мало, 146\u00A0%\u00A0— много, "sv
            u8"146<span style='font-size:3pt'>\u00A0</span>%\u00A0— самое то. Потому и пробелы бывают разные. "sv
            u8"Некоторые из них неразрывные: не расширяются при выключке, не переносятся на другую строку."
            "<p>В «Юникодии» пробелы зажаты псевдографическими символами без пустого места в кегельной площадке\u00A0— "
            "так что узость обманчива. Часто для отступа хватает тончайшего пробела U+200A."sv },
    //{ u8"Ошибка"sv },     //check for equal number
};


constinit const uc::Script uc::scriptInfo[static_cast<int>(uc::EcScript::NN)] {
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
                "Иногда использовался волоф{{-}}сильно видоизменённый [[pop_scr:Arab|арабский]] шрифт. "
                "Основная же письменность{{-}}[[pop_scr:Latn|латиница]].</p>"sv,
                EcFont::ADLAM },
    /// @todo [tofu, P1] W7/10 no font at all
    { "Aghb"sv, QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::EUROPE,
        u8"Агванская"sv, u8"II век до н.э."sv,
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
    { "Arab"sv, QFontDatabase::Arabic,
        EcScriptType::CONSONANT, EcLangLife::ALIVE, EcWritingDir::RTL, EcContinent::ASIA_INDIAN,
        u8"Арабская"sv, u8"IV—VI век"sv,
        u8"арабский, персидский, урду, уйгурский, пуштунский…"sv,
        u8"<p>Письменность развилась из [[pop_scr:Syrc|сирийской]]. Арабский язык тесно связан с исламом; на этом языке написан Коран (610–632). "
                "Арабский халифат насаждал как ислам, так и вязь. "
                "Многие исламские народы (турки, казахи, башкиры) пользовались арабицей до начала XX века.</p>"
            "<p>Компьютерная арабица осложняется написанием арабских букв: у каждой есть обособленная, начальная, средняя и конечная форма. "
                "В обычном тексте предпочтительнее «общая» форма буквы, подстраивающаяся под положение в слове. "
                "Но если нужна конечная форма в обособленной букве, в Юникоде есть и «жёсткие» варианты.</p>"sv,
                EcFont::ARABIC },
    /// @todo [tofu, P1] No font at all
    { "Armi"sv, QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::HISTORICAL, EcWritingDir::RTL, EcContinent::ASIA_INDIAN,
        u8"Имперская арамейская"sv, u8"VII в. до н.э."sv,
        u8"имперский арамейский <i>(также канцелярский арамейский\u00A0— язык Персии 500—329 до н.э.)</i>",
        u8"<p>Использовался в Ахеменидской империи как книжный язык. Был кодифицирован настолько, что крайне сложно опознать "
                "время и место конкретного документа. С завоеванием Персии Александром Македонским началась "
                "фрагментация языка и дальнейшее формирование [[pop_scr:Syrc|сирийских]] языков (предков [[pop_scr:Arab|арабского]]) "
                "и иудейских (предков [[pop_scr:Hebr|иврита]]).</p>"sv,
                EcFont::HISTORIC },
    /// @todo [tofu, BMP] Noto is ugly and has tofu 0560 and 0588, W7 has tofu 058D+.
    { "Armn"sv, QFontDatabase::Armenian,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Армянская"sv, u8"405"sv,
        u8"армянский"sv,
        u8"<p>Изобретён учёным и священником Месропом Маштоцем (362–440). Непонятно, брался ли какой-то алфавит за основу "
                u8"(возможно, несохранившиеся древнеармянские буквы). Алфавит тесно связан с распространением христианства в Армении. "
                u8"В XII веке добавились буквы {{sm|Օ}} и {{sm|Ֆ}}.</p>"
            u8"<p>Считается, что армянская литература богаче среднеперсидской (доисламской), потому что армянский алфавит проще "
                u8"[[pop_scr:Syrc|манихейской]] вязи.</p>"sv,
                EcFont::ARMENIAN },
    // Balinese OK, installed Google Noto font
    { "Bali"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::PACIFIC,
        u8"Балийская"sv, u8"≈1000"sv,
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
        u8"Бенгальская"sv, u8"XV век"sv,
        u8"бенгальский, ассамский, санскрит <i>(Бангладеш, Индия)</i>"sv,
        u8"<p>Относится к восточной ветви индийского письма. Гласная по умолчанию{{-}}среднее между «а» и «о», "
                "в Юникоде условно обозначается «а», в других источниках «ɔ» или «ô». "
                "Чтобы заменить гласную, добавляются огласовки: {{sm|ক}}=кô, {{sm|কি}}=ки."
            "<p>Вирама (''бирам'', убрать гласную по умолчанию) используется крайне редко{{-}}{{sm|নাক্ }}= нôк{{-}}"
                "потому что в бенгальском есть множество лигатур: {{sm|ক্ত}}=ктô. "
                "Из-за этого бенгальский считается одной из красивейших письменностей мира. "
                "Набор лигатур стандартный: первая буква, вирама, вторая буква."
            "<p>Бангладеш{{-}}одно из двух исламских государств с собственной письменностью "
                "(не [[pop_scr:Arab|арабской]], не [[pop_scr:Latn|латиницей]], не [[pop_scr:Cyrl|кириллицей]]). "
                "Второе{{-}}[[pop_scr:Thaa|Мальдивские острова]]."sv,
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
        u8"Бугийская (лонтара)"sv, u8"XVII век"sv,
        u8"бугийский и другие языки о. Сулавеси"sv,
        u8"<p>Бугийская письменность, или лонтара\u00A0— потомок брахми и работает по тому же принципу. Гласная по умолчанию «а». "
            "Со времён голландской колонизации заменяется латиницей.</p>"sv,
                EcFont::BUGINESE },
    // Buhid OK, installed Google Noto font
    { "Buhd"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::PACIFIC,
        u8"Бухид"sv, u8"≈1300"sv,
        u8"бухидский <i>(Филиппины)</i>"sv,
        u8"<p>Используется мангиан{{-}}небольшой филиппинской народностью (8000 на 1991\u00A0год). "
                "Восходит к брахми и родственный с тагальским.</p>"
            "<p>В языке всего 15 согласных и три гласных: «а» по умолчанию, «и» и «у» задаются огласовками. Начальные гласные\u00A0— "
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
        u8"Чамская"sv, u8"IV век"sv,
        u8"чамский <i>(Вьетнам, Камбоджа)</i>"sv,
        u8"<p>Чамская письменность произошла из брахми, и слоги по умолчанию заканчиваются гласной «а». Чтобы добавить другую гласную, "
                "дописывается огласовка. Но у чамов исчез знак «вирама» (убрать гласную), вместо этого к слогу добавляется знак "
                "конечной согласной: {{sm|ꨆ}}=ка, {{sm|ꨆꨯ}}=ко, {{sm|ꨆꨯꩃ}}=конг.</p>"
            "<p>Большинство чамов исповедуют ислам и пишут [[pop_scr:Arab|арабицей]], лишь некоторые чамы Вьетнама "
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
    /// @todo [tofu] 2C80 block is not covered at all under W7
    { "Copt"sv, QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::LTR, EcContinent::AFRICA,
        u8"Коптская"sv, u8"II век"sv,
        u8"коптский"sv,
        u8"<p>Происходит из [[pop_scr:Grek|греческого]]. Используется коптами (египетскими христианами) как богослужебный. "
                "Общаются копты по-арабски, как разговорный коптский умер от XVII до XIX{{_}}века."
            "<p>С Юникода 4.1 (2005) коптский отделён от греческого."sv },
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
    /// @todo [tofu, BMP] Another PitA in W7, A8FC+
    { "Deva"sv, QFontDatabase::Devanagari,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Деванагари"sv, u8"I—VII век",
        u8"хинди, санскрит и другие языки Индии"sv,
        u8"<p>Деванагари (буквально «язык божественного города») развился из письма брахми и стал алфавитом для многих языков Индии. "
                "Особенность деванагари{{-}}все буквы свисают с горизонтальной черты.</p>"
        "<p>Каждый символ означает слог с гласной «а». Чтобы отобразить другие слоги, надо добавить огласовку: "
                "<nobr>ка {{sm|क}} + и {{sm| ि}} = ки {{sm|कि}}</nobr>. "
                "Чтобы получить простую согласную, надо добавить знак «вирама» («убийца»): к {{sm|क्}}.</p>"sv,
                EcFont::DEVANAGARI },
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
        u8"Грузинская"sv, u8"≈V век"sv,
        u8"грузинский, мегрельский и другие картвельские"sv,
        u8"<p>Достоверно неизвестно, когда создана грузиница и что было прототипом. "
                "Распространённая версия, что грузинский алфавит создан создателем [[pop_scr:Armn|армянского]] Месропом Маштоцем, "
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
        u8"Греческая"sv, u8"IX век до н.э."sv,
        u8"греческий"sv,
        u8"<p>Греческий сделан из финикийского без оглядки на раннегреческие системы{{-}}линейное письмо Б и кипрское. "
                "Финикийский алфавит был консонантным (задаёт согласные звуки), и семитским языкам это подходит, ведь корень слова{{-}}"
                "костяк согласных, а гласные играют роль окончаний: так, в [[pop_scr:Arab|арабском]] (тоже семитский) ''джихадом'' занимается <i>моджахед</i> "
                "(или, точнее, ''муджахид'').</p>"
            "<p>Греки добавили гласные звуки, и греческий{{-}}первый известный истинный алфавит, кодирующий как согласные, так и гласные. "
                "Названия букв изменились мало, но перестали что-то значить: ''алеф=бык''\u00A0→ ''альфа''.</p>"
            u8"<p>Из греческого письма пошли [[pop_scr:Latn|латиница]], [[pop_scr:Cyrl|кириллица]], "
                "[[pop_scr:Copt|коптский]] и, возможно, [[pop_scr:Armn|армянский]] и [[pop_scr:Runr|руны]].</p>"sv },
    /// @todo [tofu] Sorry have tofu in W7, install a font
    { "Gujr"sv, QFontDatabase::Gujarati,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Гуджарати"sv, u8"X–XVI век"sv,
        u8"гуджаратский <i>(запад Индии)</i>"sv,
        u8"<p>Появился из [[pop_scr:Deva|деванагари]] и очень похож на него, но уже старый гуджарати потерял характерную черту сверху. "
                "Гласная по умолчанию «ə».</p>"sv },
    // Gurmukhi OK, installed Google Noto because of W7 troubles
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
        EcScriptType::PSEUDOALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Хангыль"sv, u8"1443"sv,
        u8"корейский"sv,
        u8"<p>Хангыль (мужской род!){{-}}корейское алфавитно-слоговое письмо. В нём 51 чамо (буква) объединяется в группы приблизительно по слогам.</p>"
        u8"<p>Придуман группой учёных под руководством царя Седжона Великого, считавшего, что [[pop_scr:Hani|китайские иероглифы]] "
                "плохо передают корейский язык и сложны для народа. Образованная публика смотрела на хангыль надменно, "
                "считая женским, детским или народным письмом. Последующие цари даже запрещали хангыль. Возрождение началось в XX{{_}}веке, "
                "официальным стал в 1945. Единственная письменность Северной Кореи и основная{{-}}в Южной."sv,
                EcFont::CJK },
    /// @todo [tofu, BMP] Current Noto font is limited with 2017
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
        u8"<p>Иврит развился из [[pop_scr:Armi|арамейской]] письменности, и ко II{{_}}в. до{{_}}н.э. приобрёл почти современный вид.</p>"
            "<p>Записывает только согласные буквы, но четыре буквы {{sm|אהוי}} могут означать гласные. "
                "С той же целью иногда используют огласовки{{-}}точки над буквами."
            "<p>К X{{_}}веку иврит уходит из разговорной практики, оставшись только книжным языком, однако еврейскими буквами "
                "писали и на разговорных еврейских языках: идише, ладино и других. С появлением государства Израиль "
                "иврит снова сделали языком повседневного общения."sv,
                EcFont::HEBREW },
    { "Hira"sv, QFontDatabase::Japanese,
        EcScriptType::SYLLABLE, EcLangLife::ALIVE, EcWritingDir::LTR_CJK, EcContinent::ASIA_INDIAN,
        u8"Хирагана"sv, u8"VIII—IX век"sv,
        u8"японский"sv,
        u8"<p>Около III{{_}}века японцы стали писать [[pop_scr:Hani|китайскими иероглифами]]. "
                "Из них произошла вспомогательная азбука манъёгана{{-}}роль играло не изображение на иероглифе, а прочтение. "
                "Хирáгана{{-}}не что иное, как очень небрежно записанная манъёгана."
            "<p>Женщины, которым не было доступно образование, писали слоговой азбукой, и предпочитали не рубленую "
                "[[pop_scr:Kana|катáкану]], а плавную хирáгану. Сейчас хираганой пишут слова, у которых иероглифа нет или неизвестен пишущему/читающему (кроме "
                "заимствованных, для них катакана), окончания слов, учат детей, подписывают прочтение иероглифов."sv,
                EcFont::CJK },
    // Javanese OK, W10 has “Javanese Text”, W7 does not, installed Google Noto font
    { "Java"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::PACIFIC,
        u8"Яванская"sv, u8"XV век"sv,
        u8"яванский, мелкие языки Явы"sv,
        u8"<p>Происходит из брахми. Гласная по умолчанию «а». Знак «вирама» (стереть гласную) называется «пангкон» "
                "и представляет собой не косую чёрточку, а длинный крюк. Яванская письменность близка к балийской. "
                "В настоящее время вытесняется латиницей.</p>"sv,
                EcFont::JAVANESE },
    // Kayah Li OK, W10 none, installed Google Noto font
    { "Kali"sv, QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Кая-ли"sv, u8"1962"sv,
        u8"краснокаренский <i>(Мьянма)</i>"sv,
        u8"<p>Кая-ли происходит из брахми, создана Хтаэ Бу Пхаэ в 1962 в ответ на распространение [[pop_scr:Latn|латиницы]]{{-}}"
                "но буквы не имеют гласной по умолчанию, и потому это не абугида (слоговая азбука, где у сходных слогов сходное написание). "
                "Четыре гласных пишутся отдельными буквами, остальные диакритикой над {{sm|ꤢ}}. "
                "Ещё несколько гласных из заимствованных слов пишутся диакритикой над {{sm|ꤣ}}.</p>"
            "<p>Кая-ли{{-}}во многом алфавит беженцев: {{%|90}} живущих в тайских лагерях беженцев{{-}}карены, сбежавшие от зверств бирманских "
                "военных, и в околотюремных условиях важно всё, что сплачивает. "
                "Карены{{-}}разнородная группа народов, среди них есть красные, чёрные, белые и другие, по цвету национального костюма. "
                "Помимо кая-ли, пишут [[pop_scr:Thai|тайским]], латиницей и [[pop_scr:Mymr|бирманским]].</p>"sv,
                EcFont::KAYAH_LI },
    /// @todo [img] what images to choose for Katakana/Hiragana?
    { "Kana"sv, QFontDatabase::Japanese,
        EcScriptType::SYLLABLE, EcLangLife::ALIVE, EcWritingDir::LTR_CJK, EcContinent::PACIFIC,
        u8"Катакана"sv, u8"VIII—IX век"sv,
        u8"японский"sv,
        u8"<p>Около III{{_}}века японцы стали писать [[pop_scr:Hani|китайскими иероглифами]]. "
                "Из них произошла вспомогательная азбука манъёгана{{-}}"
                "роль играло не изображение на иероглифе, а его прочтение. Манъёгана упростилась до катáканы.</p>"
            "<p>Катáкану используют для записи заимствованных слов и начального обучения иностранцев. "
                "До 1946 использовали для записи окончаний слов (сейчас это делают [[pop_scr:Hira|хирáганой]]).</p>"sv,
                EcFont::CJK },
    { "Khmr"sv, QFontDatabase::Khmer,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Кхмерская"sv, u8"VI век"sv,
        u8"кхмерский <i>(Камбоджа)</i>"sv,
        u8"<p>Как и другие языки Юго-Восточной Азии, произошёл из брахми. Гласная по умолчанию для одних согласных «а», "
                "для других «о». Самый старый источник датируется 611. "
                "Стал основой для тайского и лаосского.</p>"sv },
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
        u8"Лаосская"sv, u8"≈1350"sv,
        u8"лаосский"sv,
        u8"<p>Лаосский алфавит стандартизирован в XIV веке. Как и тайский, происходит из [[pop_scr:Khmr|кхмерского]]. "
                "После реформы, сделанной коммунистами в 1960-е, исчезли гласная по умолчанию и знак вирамы, и у лаосского остался последний "
                "признак абугиды{{-}}сложные правила записи гласных: {{sm|ເກົ້າ}} ''kao'' «девять». Лаосский язык тоновый, и в этом слове "
                "мы видим над согласной {{sm|ກ}} как огласовку из трёх (!) символов , так и тон.</p>"
            "<p>Между словами не ставятся пробелы. Как и в [[pop_scr:Thai|тайском]], буквы называются подходящими словами: "
                "{{sm|ບ}} называется «бо-бэ» или просто «бэ» (козёл). Буква {{sm|ຣ}} (r) стала немой или преобразовалась в «л», и потому "
                "исчезла из орфографии, оставшись только для записи иностранных имён.</p>"
            "<p>В буддизме используется язык пали как священный, для записи этого языка используются несколько дополнительных букв "
                "и та самая вирама.</p>"
            "<p>В Юникоде для пустого места над меткой принят пунктирный кружок; лаосцы традиционно используют косой крест, "
                "в Юникод не попавший. Иногда для этого используют [[pop_scr:Latn|латинскую]] «x».</p>"sv,
                EcFont::LAO },
    /// @todo [tofu] Latin is very extensive, check it thoroughly
    { "Latn"sv, QFontDatabase::Latin,
        EcScriptType::ALPHABET, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::EUROPE,
        u8"Латиница"sv, u8"I тысячелетие до н.э."sv,
        u8"большинство языков западного и тюркского мира (включая Восточную Европу, Прибалтику, Молдавию), Чёрной Африки, Океании; "sv
            u8"вьетнамский, карельский, курдский, эсперанто"sv,
        u8"<p>Латиница\u00A0— древнегреческое письмо, адаптированное около VII\u00A0в. до нашей эры "sv
                u8"для записи этрусских говоров, впоследствии ставших латинском языком.</p>"sv
            u8"<p>В средние века к латинице добавились буквы J, V и W. Минускульное письмо, изобретённое для экономии дорогого пергамента, "sv
                u8"превратилось в строчные буквы.</p>"sv
            u8"<p>Латинский язык давно мёртв, но широкая территория Римской империи, миссионерская деятельность Папского престола "sv
                u8"и Великие географические открытия, совершённые западным миром, сделали латиницу основным алфавитом мира. "sv
                u8"Латиница используется в математике, медицине, фонетике, программировании.</p>"sv
            u8"<p>С развалом СССР на латиницу перешли Азербайджан, Молдавия, Туркмения, Узбекистан.</p>" },
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
                "и письменность на [[pop_scr:Thai|тайской]] основе.</p>"sv,
                EcFont::LISU },
    // Mandaic OK, W10 has not, installed Google Noto
    { "Mand"sv, QFontDatabase::Any,
        EcScriptType::CONSONANT, EcLangLife::ENDANGERED, EcWritingDir::RTL, EcContinent::ASIA_INDIAN,
        u8"Мандейская"sv, u8"II—VII век"sv,
        u8"мандейский <i>(Иран и Ирак)</i>"sv,
        u8"<p>Произошла из арамейского или парфянского письма. Используется так называемыми «болотными арабами», живущими у слияния рек "sv
                u8"Тигр и Евфрат и исповедующими мандеизм, необычную гностическую религию. Их количество в Ираке быстро уменьшается "sv
                u8"с 45&nbsp;000 (1996) до 5000 (2007), около 60&nbsp;тыс. разбросаны по миру, и, вероятно, на правах беженцев они быстро исчезнут.</p>"sv,
                EcFont::MANDAIC },
    // Malayalam OK, W7 does not have 2014/17 extensions → installed Google Noto
    { "Mlym"sv, QFontDatabase::Malayalam,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Малаялам"sv, u8"830"sv,
        u8"малая́лам <i>(Южная Индия)</i>"sv,
        u8"<p>Как и большинство других письменностей Юго-Восточной Азии, произошёл из брахми. "
                "Гласная по умолчанию «а». Отличается тем, что в начале слова используются полные огласовки, "
                "в середине и в конце\u00A0— сокращённые.</p>"sv,
                EcFont::MALAYALAM },
    // Mongol OK, dislike standard font of W10 → installed Google Noto
    { "Mong"sv, QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::REVIVED, EcWritingDir::LTR_COL, EcContinent::ASIA_INDIAN,
        u8"Монгольская (старая)"sv, u8"X век"sv,
        u8"монгольский"sv,
        u8"<p>Развилась из староуйгурского. Единственная действующая система с записью слева направо по столбцам, из-за чего компьютеры "
                "очень неохотно её поддерживают. Первый известный документ датируется 1204. "
                "После войны монголы разработали новую письменность на кириллице, но с развалом СССР есть планы "
                "расширить использование старой письменности.</p>"sv,
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
                "Чтобы поставить другую гласную, используются огласовки, чтобы убрать\u00A0— знак «вирама», которая по-бирмански называется "
                "«а-сат».</p>"
            "<p>Раннебирманское письмо было квадратным; закруглилось оно около XVII века, когда распространилось письмо на пальмовых листьях: "
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
    // NKo ok, W10 “Ebrima”, W7 lacks a few chars → installed Google Noto
    { "Nshu"sv, QFontDatabase::Any,
        EcScriptType::SYLLABLE, EcLangLife::DEAD, EcWritingDir::LTR_CJK, EcContinent::ASIA_INDIAN,
        u8"Нюй-шу"sv, u8"900 (предположительно) … 1300 (точно)"sv,
        u8"один из диалектов сянского <i>(Хунань, Китай)</i>"sv,
        u8"<p>Нюй-шу существовало веками и использовалось как женское письмо: [[pop_scr:Hani|грамоте]] учили мужчин, а женщины "
                "использовали более простое слоговое письмо. Многие из памятников нюй-шу{{-}}''послания третьего дня'': "
                "девушка уходила замуж в другую деревню, а подруги и сёстры вышивали ей пожелания, часто в стихах."
            "<p>Несмотря на распространённое заблуждение, нюй-шу не было тайнописью; на него смотрели как на «низкое» письмо."
            "<p>В отличие от японской [[pop_scr:Hira|хираганы]] сходного назначения, нюй-шу так и осталось неофициальным письмом. "
                "К закату нюй-шу привели ликбез и распространение швейных машин в XX{{_}}веке. "
                "К тому же японские оккупанты запрещали нюй-шу, видя в нём шифровку, "
                "а хунвэйбины в Культурную революцию уничтожали памятники старины, в том числе вышивки."
            "<p>Повторно открыли нюй-шу в 1980-е. Последняя носительница нюй-шу, Ян Хуаньи, умерла в 2004 в возрасте 98{{_}}лет{{-}}"
                "после неё письменностью владели только исследователи. "
                "Вышивки с нюй-шу стало популярным туристским сувениром; опасаются, что туризм исказит древнее письмо."
            "<p>Известно около 1800 разных символов, но многие означали один и тот же слог."sv,
                EcFont::NUSHU },
    // Ogham OK, Windows font is differently named in W7 and W10 → installed Google Noto
    { "Ogam"sv, QFontDatabase::Ogham,
        EcScriptType::ALPHABET, EcLangLife::HISTORICAL, EcWritingDir::LTR_BU, EcContinent::EUROPE,
        u8"Огамическая"sv, u8"IV век"sv,
        u8"древнеирландский, пиктский"sv,
        u8"<p>Огамической письменностью пользовались древние кельты и пикты, жившие в Ирландии и Британии, с IV до X века. "
                "Заодно в ходу была [[pop_scr:Latn|латиница]], потому есть мнение, что это тайнопись. "
                "Письменность приспособлена под зарубки на палках и досках. "
                "Надпись начинается с {{sm|᚛}} и заканчивается {{sm|᚜}}.</p>"sv,
                EcFont::OGHAM },
    // Ol Chiki OK, W10 “Nirmala UI”, W7 none → installed Google Noto
    { "Olck"sv, QFontDatabase::Any,
        EcScriptType::ALPHABET, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Ол-чики"sv, u8"1925"sv,
        u8"сантальский <i>(Индия)</i>"sv,
        u8"<p>Алфавит придумал в 1925 году Рагхунатх Мурму. До этого пользовались [[pop_scr:Beng|бенгальской]] письменностью, "
                "[[pop_scr:Orya|орией]] или [[pop_scr:Latn|латиницей]], хотя большинство населения было неграмотным. "
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
        u8"<p>Ори́я (традиционно считается однокоренным с ''арийцами'') развилась из нагари (ранней формы [[pop_scr:Deva|деванагари]] "
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
                "Письмо постепенно заменяется латиницей.</p>"
            "<p>Перед нами обычная брахмийская абугида с гласной по умолчанию «а» и вирамой: "
                "{{sm|ꤰ}}=ка, {{sm|ꤰꥉ}}=ke, {{sm|ꤰ꥓}}=k. "
                "Письменность деградировала по той же причине, что и [[pop_scr:Runr|руны]]: проще выцарапывать на дереве.</p>"sv,
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
        u8"Самаритянская"sv, u8"около 600—200 до н.э.",
        u8"иврит, самаритянский арамейский"sv,
        u8"<p>Происходит из палеоеврейского письма. По Библии, самаритяне пришли в Палестину из Двуречья и приняли еврейскую "
                "религию и культуру. На 2021 существует не более 700 самаритян.</p>"sv,
                EcFont::SAMARITAN },
    // Saurashtra OK, W10 none, installed Google Noto
    { "Saur"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ENDANGERED, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Саураштра"sv, u8"XIX век (возможно, раньше)",
        u8"саураштра <i>(Индия, письменность используется только в штате Тамилнад)</i>"sv,
        u8"<p>Происхождение неясно, все [[pop_scr:Taml|тамильские]] саураштра двуязычны, а в остальных штатах пишут на "
                "[[pop_scr:Deva|деванагари]], [[pop_scr:Gujr|гуджарати]], [[pop_scr:Telu|телугу]]. "
                "По переписи 2007, в Индии 247{{_}}тыс. саураштра."
            "<p>Перед нами обычная брахмийская абугида с гласной по умолчанию «а» и вирамой: "
                "{{sm|ꢒ}}=ка, {{sm|ꢒꢶ}}=ки, {{sm|ꢒ꣄}}=к. Вирама употребляется часто. "
                "Начальная гласная{{-}}отдельный символ: {{sm|ꢄ}}=и."sv,
                EcFont::SAURASHTRA },
    // Sinhala OK, W10 obviously has no 2020 extension → installed Google Noto
    /// @todo [desc, BMP] interesting script, describe
    { "Sinh"sv, QFontDatabase::Sinhala,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Сингальская"sv, u8"300",
        u8"сингальский <i>(Шри-Ланка)</i>",
        u8"<p>Как и большинство других письменностей Юго-Восточной Азии, произошла из брахми. Буквы имеют витиеватую форму, "
                "потому что в древности часто писали на пальмовых листьях, а прямые линии разрезали бы лист по жилкам.</p>"sv,
                EcFont::SINHALA },
    // Sundanese OK, even W10 off → installed Google Noto
    { "Sund"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::REVIVED, EcWritingDir::LTR, EcContinent::PACIFIC,
        u8"Сунданская"sv, u8"XIV век",
        u8"сунданский <i>(Ява)</i>"sv,
        u8"<p>Язык сундов (38{{_}}млн на 2007), распространённый на западе Явы. Обычная брахмийская абугида с гласной по умолчанию «а». "
                "Для других гласных используются огласовки, для голой согласной{{-}}знак «вирама» ''(pamaaeh)'': "
                "{{sm|ᮞ}}=са, {{sm|ᮞᮥ}}=су, {{sm|ᮞ᮪}}=с. Начальные гласные{{-}}отдельные буквы.</p>"
            "<p>Письменность использовалась до XVIII{{_}}века, после этого стали использовать [[pop_scr:Java|яванскую]] и "
                "пегон (местный вариант [[pop_scr:Arab|арабского]]); с XX века{{-}}[[pop_scr:Latn|латиницу]]. "
                "В 1997 году власти Индонезии решили дать сундам собственную письменность, сделав её из старосунданского.</p>"sv,
                EcFont::SUNDANESE },
    // Syloti Nagri OK, W10 none → installed Google Noto
    { "Sylo"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Силхети-нагари"sv, u8"XV век",
        u8"силхетский <i>(Бангладеш, Индия)</i>"sv,
        u8"<p>По одной из теорий, письменность произошла из кайтхи и изобретена мусульманами. За простоту письменность была любима поэтами "
                "XIX{{_}}века, но к середине XX{{_}}века уступила [[pop_scr:Beng|бенгальской]]. В 2000-е годы в Бангладеше "
                "начали снова изучать силхети. На 2007 было 11{{_}}млн силхетов."
            "<p>Обычная брахмийская абугида с гласной по умолчанию «о» и вирамой ''(хасанта)'': {{sm|ꠇ}}=ко, {{sm|ꠇꠣ}}=ка, {{sm|ꠇ꠆}}=к. "
                "Существует и другая запись вирамы, {{sm|ꠇ꠬}}=к, на манер других брахмийских письменностей. "
                "Начальные гласные пишутся отдельными буквами. "
                "Текст свисает с общей черты{{-}}но, в отличие от [[pop_scr:Deva|деванагари]], крепко за неё «держится»."
            "<p>Вирама в силхети нечаста по двум причинам. Во-первых, двойные согласные и некоторые стыки образуют лигатуры: "
                "{{sm|ꠇ\uA806ꠇ}}=кко, {{sm|ꠇ\uA806ꠔ}}=кто. Набор таких лигатур стандартный: первая буква, вирама, вторая буква. "
                "Кроме того, иногда конечная «о» становится немой."
            "<p>Запись слева направо, но силхетские книги, как дань исламу, сшиты справа налево."sv,
                EcFont::SYLOTI_NAGRI },
    // Syriac OK, W10 has no Malayalam extensions → Beth Mardutho made a nice font.
    { "Syrc"sv, QFontDatabase::Syriac,
        EcScriptType::CONSONANT, EcLangLife::ENDANGERED, EcWritingDir::RTL, EcContinent::ASIA_INDIAN,
        u8"Сирийская"sv, u8"I век",
        u8"сирийский <i>(исп. как литургический)</i>, новоарамейские, малаялам, согдийский"sv,
        u8"<p>Потомок [[pop_scr:Armi|арамейского]] алфавита, впоследствии развившийся в [[pop_scr:Arab|арабицу]]. "
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
                "создать [[pop_scr:Talu|новое письмо лы]], письменность почти что превратилась в алфавитную, и абугидой её "
                "оставляют «пустая» согласная {{sm|ᥟ}} в начале слова, если оно начинается с гласной, и тоновые буквы "
                "после конечной согласной слога (если таковая есть).</p>"
            "<p>Пробелы пишутся между предложениями, но не между словами. </p>"
            "<p>В лы третьей реформы (1963—1988) для записи тонов использовались обычные европейские умляуты: "
                "<big>à, á, ȧ, ä, ǎ</big>.</p>"
            "<p>Дайцев в Китае ≈500 тыс. (2001), но язык быстро заменяется китайским и потому отнесён к уязвимым. "
                "Ещё не более 500 тыс. разбросано по другим странам, они пользуются в основном [[pop_scr:Lana|ланной]].</p>"sv,
                EcFont::TAILE },
    // New Tai Lue OK, M$ has a good font
    { "Talu"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::NEW, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Новое письмо лы"sv, u8"1950-е"sv,
        u8"дайские (=лы; <i>Китай; общины лы в других странах пользуются старым письмом</i>)"sv,
        u8"<p>Созданный в 1950-е годы китайскими властями алфавит встретил сопротивление народа, и Китай пошёл по пути "
                "постепенного упрощения [[pop_scr:Tale|старого письма лы]].</p>"
            "<p>Гласная по умолчанию «а». Вирама имеет вид крючка с петлей и в Юникоде записывается как единое целое с согласной: {{sm|ᧇ}}. "
                "Необычно устроена запись тонов: согласные бывают в двух регистрах, ещё два знака {{sm|ᧈᧉ}} могут "
                "приписываться к слогу в конце, итого 2·3=6 тонов.</p>",
                EcFont::TAILUE },
    // Tamil BMP OK, different fonts in W10 and W7
    /// @todo [tofu, P1] Aux. Tamil characters are still tofu
    { "Taml"sv, QFontDatabase::Tamil,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Тамильская"sv, u8"VI—IX век"sv,
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
        u8"<p>Происходит из [[pop_scr:Thai|старотайского]] (царство Сукхотаи). "
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
                "Письмо было консонантное, писали чаще всего снизу вверх или справа налево."
            "<p>В XX веке из староливийского создали новую письменность, с написанием слева направо. Исламские государства "
                "(в частности, Ливия Каддафи и Марокко 80-х) запрещали письменность, но в 2003 король Марокко разрешил тифинаг. "
                "Переходный совет Ливии также разрешил берберские языки и тифинаг."sv,
                EcFont::TIFINAGH },
    // Tagalog OK, W10 off → installed Google Noto
    { "Tglg"sv, QFontDatabase::Any,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::DEAD, EcWritingDir::LTR, EcContinent::PACIFIC,
        u8"Тагальская (байбайин)"sv, u8"1300 (спорно) — 1500 (точно)"sv,
        u8"старотагальский и другие языки Явы и Филиппин"sv,
        u8"<p>Гласная по умолчанию «а».</p>"
            "<p>Испанские миссионеры (XVI\u00A0век) увидели, что островитяне поголовно грамотны, и "
                "сами стали учить их на байбайине, но в XVIII\u00A0веке обнаружили, что аборигены переняли латиницу. "
                "К XIX\u00A0веку байбайин был мёртв, и оставалось только упорядочить латинский алфавит. "
                "Есть современные попытки возродить байбайин.</p>"sv,
                EcFont::TAGALOG },
    // Thaana OK, Monotype/M$ “MV Boli” font
    { "Thaa"sv, QFontDatabase::Thaana,
        EcScriptType::PSEUDOALPHABET, EcLangLife::ALIVE, EcWritingDir::RTL, EcContinent::ASIA_INDIAN,
        u8"Тана"sv, u8"XVIII век"sv,
        u8"дивехи <i>(мальдивский)</i>"sv,
        u8"<p>Письменная культура пришла на Мальдивские острова вместе с буддизмом, появилось письмо ''эвела акуру'' на основе "
                "[[pop_scr:Sinh|сингальского]]. Самая старая надпись датируется VII—VIII веком. "
                "В 1153 году некто Мухаммад ул-Абдалла провозгласил себя султаном и обратил мальдивцев в ислам; "
                "к XIV веку письмо видоизменилось в ''дивес акуру'' и сменило направление на обратное. "
                "В 1703 году султан Ибрахим{{_}}III заменил старую графику новой{{-}}''габули таной'' (стандартным письмом).</p>"
            "<p>Происхождение таны уникально: согласные произошли из арабских и доисламских цифр, гласные{{-}}"
                "из [[pop_scr:Arab|арабских]] огласовок. "
                "Со временем линии сгладились, стали писать с изящным 45-градусным наклоном. Путешественник Тур Хейердал сравнил тану "
                "с нарезанной лапшой.</p>"
            "<p>В 1970-е годы тане грозило исчезновение: при президенте Ибрагиме Насире появилась телексная связь, и "
                u8"правительство перешло на довольно некачественную латинскую транслитерацию. Новый президент Момун Абдул Гайюм восстановил тану, "
                u8"и обе письменности используются параллельно.</p>"sv,
                EcFont::THAANA },
    // Thai OK, W7 and 10 fonts are different, but managed to
    { "Thai"sv, QFontDatabase::Thai,
        EcScriptType::ABUGIDA_BRAHMI, EcLangLife::ALIVE, EcWritingDir::LTR, EcContinent::ASIA_INDIAN,
        u8"Тайская"sv, u8"1283"sv,
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
                "[[pop_scr:Cher|письма чероки]] (вместе с неграми в Либерию эмигрировали и индейцы, "
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
               u8"Управляющие, соединители и тэги, не имеющие своего написания.",
               false },
    { "CS"sv,  EcBidiStrength::WEAK,        u8"Цифровой разделитель"sv,
               u8"Цифровой разделитель"sv, u8"CS",
               u8"Оказавшись в арабском или европейском числе, эти символы становятся его частью.",
               true },

    { "EN"sv,  EcBidiStrength::WEAK,        u8"Европейская цифра"sv,
               u8"Европейская цифра"sv, u8"EN",
               u8"Пишутся слева направо. Есть тонкие правила, связанные с арабскими буквами."sv,
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
                    "Поскольку подавляющая доля Юникода{{-}}[[pop_scr:Hani|китайские иероглифы]], это самый многочисленный класс."sv,
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
               u8"Нейтральны. Есть тонкие правила, касающиеся форматирования пробелами.",
               true },
    //{ u8"Ошибка"sv },  check for equal number
};


constinit const uc::Block uc::blocks[302] {
    { 0x0000, 0x007F,
            "Basic Latin",
            u8"Базовая латиница"sv,
            u8"<p>128 символов базовой латиницы совпадают с ASCII{{-}}американской семибитной кодировкой, ставшей основой для большинства "
                    "однобайтовых кодировок. ASCII содержит важнейшие символы английского языка{{-}}так что английский юникодный "
                    "текст читается, даже если кодировка сбита.</p>"
                "<p>Первые 32 символа, 00…1F, управляющие, и сегодня немногие из них действительно используются. Затем идут 95 печатных символов. "
                    "Последний, 7F=DEL, также управляющий, родом с перфолент{{-}}отверстия, пробитые во всех дорожках, "
                    "говорят, что символ выбит ошибочно и не подлежит считыванию.</p>"
                "<p>Управляющие символы, если те не задействованы в программе, часто служат пользовательскими: "
                    "разного рода символы подстановки, пустое место в базе данных, отличающееся от пустой строки, и т.д.</p>"sv },
    { 0x0080, 0x00FF,
            "Latin-1 Supplement", u8"Латиница-1"sv,
            u8"<p>Латиница-1 совпадает с однобайтовой кодировкой ISO 8859-1, появившейся в 1987 году и расширяющей ASCII до европейских языков.</p>"
                "<p>Первые 32 символа, 80…9F, управляющие для совместимости с 7-битными кодировками: почтовые и другие программы иногда "
                    "теряли верхний бит, и не хотелось бы в этих позициях ставить печатные символы. Сейчас эти символы не применяются "
                    "по назначению и служат пользовательскими. Так, кодировка Windows-1252{{-}}это та же латиница-1, только вместо "
                    "управляющих символов типографские.</p>"
                "<p>Среди букв видны символы × и ÷, изначально там должен быть Œœ.</p>"sv },
    { 0x0100, 0x017F,
            "Latin Extended-A", u8"Латиница расширенная A"sv,
            u8"Содержит символы языков Центральной и Восточной Европы, Прибалтики, кельтских, а также саамского, мальтийского, "
                "турецкого, эсперанто и некоторых других."sv },
    { 0x0180, 0x024F,
            "Latin Extended-B", u8"Латиница расширенная B"sv,
            u8"Содержит символы словенского, хорватского, румынского, ливского, чжуанского, пиньиня (латинизации китайского), африканских, "
                    "индейских языков, а также старой (до 1930) латиницы языков бывшего СССР."sv },
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
    /// @todo [desc] Cannot use {sm} here, what to do?
    { 0x02B0, 0x02FF,
            "Spacing Modifier Letters", u8"Протяжённые модификаторы букв"sv,
            u8"<p>Модификаторы букв используются в фонетике и языкознании, чтобы передать тоны, длину, мягкость и условные звуки: "
                    "так, английское ''car'' (легковой автомобиль) записывается как {{sm|[kɑːʳ]}},"
                    "и последние два символа{{-}}модификаторы, обозначающие длину и условное «r».</p>"
                "<p>Также в этом блоке есть протяжённые копии диакритических меток.</p>"sv },
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
    { 0x0370, 0x03FF,
            "Greek and Coptic", u8"Греческий и коптский",
            u8"<p>[[pop_scr:Grek|Греческий]]{{-}}первый настоящий алфавит, предок всех европейских алфавитов.</p>"
                "<p>[[pop_scr:Copt|Коптский]]{{-}}язык египетских христиан. Его алфавит основан на греческом.</p>"sv },
    { 0x0400, 0x04FF,
            "Cyrillic", u8"Кириллица", {}, EcScript::Cyrl },
    { 0x0500, 0x052F,
            "Cyrillic Supplement", u8"Кириллица дополнительная",
            u8"<p>Буквы нескольких мелких кириллических языков: абзахского, курдского ''(как нацменьшинства СССР)'', коми, "
                    "мордовского, алеутского, азербайджанского ''(ныне на [[pop_scr:Latn|латинице]])'' "
                    "и чувашского в алфавите Яковлева ''(использовался 1873–1938)''."
                "<p>Содержит смешную букву {{sm|Ԥ}}. Её «изобрёл» писатель Леонид Каганов{{-}}как оказалось, "
                    "не{{_}}первым: ещё за три года до его заметки (2008) майки с этой буквой продавались в России. "
                    "И независимо от них в абхазском вместо крюка {{sm|Ҧ}} (читается «пх») стали использовать лапку, "
                    "что и дало букве {{sm|Ԥ}} место в Юникоде."sv,
            EcScript::Cyrl },
    { 0x0530, 0x058F,
            "Armenian", u8"Армянский", {}, EcScript::Armn },
    { 0x0590, 0x05FF,
            "Hebrew", u8"Иврит", {}, EcScript::Hebr },
    { 0x0600, 0x06FF,
            "Arabic", u8"Арабский", {}, EcScript::Arab },
    { 0x0700, 0x074F,
            "Syriac", u8"Сирийский", {}, EcScript::Syrc },
    { 0x0750, 0x077F,
            "Arabic Supplement", u8"Арабский дополнительный"sv,
            u8"<p>Буквы для языков Африки, Пакистана и раннего персидского."sv,
            EcScript::Arab },
    { 0x0780, 0x07BF,
            "Thaana", u8"Тана", {}, EcScript::Thaa },
    { 0x07C0, 0x07FF,
            "NKo", u8"Нко", {}, EcScript::Nkoo },
    { 0x0800, 0x083F,
            "Samaritan", u8"Самаритянский", {}, EcScript::Samr },
    { 0x0840, 0x085F,
            "Mandaic", u8"Мандейский", {}, EcScript::Mand },
    { 0x0860, 0x086F,
            "Syriac Supplement", u8"Сирийский дополнительный"sv,
            u8"<p>Необычная запись языка [[pop_scr:Mlym|мамая́лам]] сирийскими буквами, именуемая '''суриани''' или '''каршони'''. "
                    "Использовалась индийскими христианами до XIX{{_}}века."sv,
            EcScript::Syrc },
    { 0x08A0, 0x08FF,
            "Arabic Extended-A", u8"Арабский расширенный A"sv,
            u8"<p>Дополнительные арабские буквы для рохинджа ''(Мьянма)'', белорусского, татарского, башкирского, "
                    "арви ''(Индия, Шри-Ланка''), шахмукхи ''([[pop_scr:Guru|пенджабского]])'', хиндко ''(Пакистан)'' "
                    "и африканских языков (в частности, берберского). "
                    "Также знаки комментариев к Корану.",
            EcScript::Arab },
    { 0x0900, 0x097F,
            "Devanagari", u8"Деванагари", {}, EcScript::Deva },
    { 0x0980, 0x09FF,
            "Bengali", u8"Бенгальский", {}, EcScript::Beng },
    { 0x0A00, 0x0A7F,
            "Gurmukhi", u8"Гурмукхи", {}, EcScript::Guru },
    { 0x0A80, 0x0AFF,
            "Gujarati", u8"Гуджарати", {}, EcScript::Gujr },
    { 0x0B00, 0x0B7F,
            "Oriya", u8"Ория", {}, EcScript::Orya },
    { 0x0B80, 0x0BFF,
            "Tamil", u8"Тамильский", {}, EcScript::Taml },
    { 0x0C00, 0x0C7F,
            "Telugu", u8"Телугу", {}, EcScript::Telu },
    { 0x0C80, 0x0CFF,
            "Kannada", u8"Каннада", {}, EcScript::Knda },
    { 0x0D00, 0x0D7F,
            "Malayalam", u8"Малаялам", {}, EcScript::Mlym },
    { 0x0D80, 0x0DFF,
            "Sinhala", u8"Сингальский", {}, EcScript::Sinh },
    { 0x0E00, 0x0E7F,
            "Thai", u8"Тайский", {}, EcScript::Thai },
    { 0x0E80, 0x0EFF,
            "Lao", u8"Лаосский", {}, EcScript::Laoo },
    { 0x0F00, 0x0FFF,
            "Tibetan", u8"Тибетский", {}, EcScript::Tibt },
    { 0x1000, 0x109F,
            "Myanmar", u8"Бирманский", {}, EcScript::Mymr },
    { 0x10A0, 0x10FF,
            "Georgian", u8"Грузинский", {}, EcScript::Geor },
    { 0x1100, 0x11FF,
            "Hangul Jamo", u8"Хангыль\u00A0— чамо"sv,
            u8"<p>В хангыле (корейском алфавите) всего 51 буква ''(чамо)''. Блок намного больше: одна и та же буква "
                    "в начале и конце слога кодируется разными символами, к тому же в блоке много устаревших чамо."
                "<p>Чхосон{{-}}начальные буквы, чунсон{{-}}средние, чонсон{{-}}конечные."sv,
            EcScript::Hang },
    { 0x1200, 0x137F,
            "Ethiopic", u8"Эфиопский", {}, EcScript::Ethi },
    { 0x1380, 0x139F,
            "Ethiopic Supplement", u8"Эфиопский дополнительный"sv,
            u8"<p>Буквы языка себат-бет гураге (Эфиопия, 1,5{{_}}млн, ''Ethnologue'' статус 5: развивающийся), "
                    "а также метки тонов."sv, EcScript::Ethi },
    { 0x13A0, 0x13FF,
            "Cherokee", u8"Чероки", {}, EcScript::Cher },
    { 0x1400, 0x167F,
            "Unified Canadian Aboriginal Syllabics", u8"Канадская слоговая",
            {}, EcScript::Cans },
    { 0x1680, 0x169F,
            "Ogham", u8"Огамическая", {}, EcScript::Ogam },
    { 0x16A0, 0x16FF,
            "Runic", u8"Руны", {}, EcScript::Runr },
    { 0x1700, 0x171F,
            "Tagalog", u8"Тагальский", {}, EcScript::Tglg },
    { 0x1720, 0x173F,
            "Hanunoo", u8"Хануноо", {}, EcScript::Hano },
    { 0x1740, 0x175F,
            "Buhid", u8"Бухид", {}, EcScript::Buhd },
    { 0x1760, 0x177F,
            "Tagbanwa", u8"Тагбанва", {}, EcScript::Tang },
    { 0x1780, 0x17FF,
            "Khmer", u8"Кхмерский", {}, EcScript::Khmr },
    { 0x1800, 0x18AF,
            "Mongolian", u8"Монгольский", {}, EcScript::Mong },
    { 0x18B0, 0x18FF,
            "Unified Canadian Aboriginal Syllabics Extended",
            u8"Канадская слоговая расширенная"sv,
            u8"<p>Буквы для кри, оджибве ''(алгонкинского)'', кэрриер, дене ''(атабаскских)''.", EcScript::Cans },
    { 0x1900, 0x194F,
            "Limbu", u8"Лимбу", {}, EcScript::Limb },
    { 0x1950, 0x197F,
            "Tai Le", u8"Лы (тайлэ)", {}, EcScript::Tale },
    { 0x1980, 0x19DF,
            "New Tai Lue", u8"Новое письмо лы", {}, EcScript::Talu },
    { 0x19E0, 0x19FF,
            "Khmer Symbols", u8"Кхмерские символы", {}, EcScript::Khmr },
    { 0x1A00, 0x1A1F,
            "Buginese", u8"Лонтара (бугийский)", {}, EcScript::Bugi },
    { 0x1A20, 0x1AAF,
            "Tai Tham", u8"Ланна (тай-тхам)", {}, EcScript::Lana },
    { 0x1AB0, 0x1AFF,
            "Combining Diacritical Marks Extended",
            u8"Диакритические метки расширенные"sv,
            u8"<p>Используются в немецкой и шотландской диалектологии."sv,
            EcScript::NONE, EcFont::NOTO },
    { 0x1B00, 0x1B7F,
            "Balinese", u8"Балийский", {}, EcScript::Bali },
    { 0x1B80, 0x1BBF,
            "Sundanese", u8"Сунданский", {}, EcScript::Sund },
    { 0x1BC0, 0x1BFF,
            "Batak", u8"Батакская", {}, EcScript::Batk },
    { 0x1C00, 0x1C4F,
            "Lepcha", u8"Лепча", {}, EcScript::Lepc },
    { 0x1C50, 0x1C7F,
            "Ol Chiki", u8"Ол-чики", {}, EcScript::Olck },
    { 0x1C80, 0x1C8F,
            "Cyrillic Extended-C", u8"Кириллица расширенная C",
            {}, EcScript::Cyrl },
    { 0x1C90, 0x1CBF,
            "Georgian Extended", u8"Грузинский расширенный",
            {}, EcScript::Geor },
    { 0x1CC0, 0x1CCF,
            "Sundanese Supplement", u8"Сунданский дополнительный",
            {}, EcScript::Sund },
    { 0x1CD0, 0x1CFF,
            "Vedic Extensions", u8"Ведические символы", {}, EcScript::Deva },
    { 0x1D00, 0x1D7F,
            "Phonetic Extensions", u8"Фонетические расширения"sv },
    { 0x1D80, 0x1DBF,
            "Phonetic Extensions Supplement",
            u8"Фонетические расширения дополнительные"sv },
    { 0x1DC0, 0x1DFF,
            "Combining Diacritical Marks Supplement",
            u8"Диакритические метки дополнительные"sv,
            {}, EcScript::NONE, EcFont::NOTO },
    { 0x1E00, 0x1EFF,
            "Latin Extended Additional",
            u8"Латиница расширенная дополнительная" },
    { 0x1F00, 0x1FFF,
            "Greek Extended", u8"Греческий расширенный", {},
            EcScript::Grek, EcFont::NOTO },
    { 0x2000, 0x206F,
            "General Punctuation", u8"Знаки препинания" },
    { 0x2070, 0x209F,
            "Superscripts and Subscripts", u8"Верхние и нижние индексы" },
    { 0x20A0, 0x20CF,
            "Currency Symbols", u8"Символы валют" },
    { 0x20D0, 0x20FF,
            "Combining Diacritical Marks for Symbols",
            u8"Диакритические метки для символов" },
    { 0x2100, 0x214F,
            "Letterlike Symbols", u8"Буквоподобные символы" },
    { 0x2150, 0x218F,
            "Number Forms", u8"Числовые формы" },
    { 0x2190, 0x21FF,
            "Arrows", u8"Стрелки" },
    { 0x2200, 0x22FF,
            "Mathematical Operators", u8"Математические знаки" },
    { 0x2300, 0x23FF,
            "Miscellaneous Technical", u8"Разные технические"sv,
            {}, EcScript::NONE, EcFont::NOTO_SYMBOL },
    { 0x2400, 0x243F,
            "Control Pictures", u8"Изображения управляющих" },
    { 0x2440, 0x245F,
            "Optical Character Recognition",
            u8"Оптическое распознавание символов" },
    { 0x2460, 0x24FF,
            "Enclosed Alphanumerics", u8"Обрамлённые буквы и цифры" },
    { 0x2500, 0x257F,
            "Box Drawing", u8"Рисование рамок" },
    { 0x2580, 0x259F,
            "Block Elements", u8"Блочные элементы" },
    { 0x25A0, 0x25FF,
            "Geometric Shapes", u8"Геометрические фигуры" },
    { 0x2600, 0x26FF,
            "Miscellaneous Symbols", u8"Разные символы" },
    { 0x2700, 0x27BF,
            "Dingbats", u8"Украшения" },
    { 0x27C0, 0x27EF,
            "Miscellaneous Mathematical Symbols-A",
            u8"Разные математические символы A" },
    { 0x27F0, 0x27FF,
            "Supplemental Arrows-A", u8"Дополнительные стрелки A" },
    { 0x2800, 0x28FF,
            "Braille Patterns", u8"Шрифт Брайля", {}, EcScript::Brai },
    { 0x2900, 0x297F,
            "Supplemental Arrows-B", u8"Дополнительные стрелки B" },
    { 0x2980, 0x29FF,
            "Miscellaneous Mathematical Symbols-B",
            u8"Разные математические символы B" },
    { 0x2A00, 0x2AFF,
            "Supplemental Mathematical Operators",
            u8"Дополнительные математические знаки" },
    { 0x2B00, 0x2BFF,
            "Miscellaneous Symbols and Arrows",
            u8"Разные символы и стрелки" },
    { 0x2C00, 0x2C5F,
            "Glagolitic", u8"Глаголица", {}, EcScript::Glag },
    { 0x2C60, 0x2C7F,
            "Latin Extended-C", u8"Латиница расширенная C", {}, EcScript::Latn },
    { 0x2C80, 0x2CFF,
            "Coptic", u8"Коптский", {}, EcScript::Copt },
    { 0x2D00, 0x2D2F,
            "Georgian Supplement", u8"Грузинский дополнительный", {}, EcScript::Geor },
    { 0x2D30, 0x2D7F,
            "Tifinagh", u8"Тифинаг (берберский)", {}, EcScript::Tfng },
    { 0x2D80, 0x2DDF,
            "Ethiopic Extended", u8"Эфиопский расширенный", {}, EcScript::Ethi },
    { 0x2DE0, 0x2DFF,
            "Cyrillic Extended-A", u8"Кириллица расширенная A", {}, EcScript::Cyrl },
    { 0x2E00, 0x2E7F,
            "Supplemental Punctuation", u8"Дополнительные знаки препинания"sv },
    { 0x2E80, 0x2EFF,
            "CJK Radicals Supplement", u8"ККЯ дополнительные ключи", {}, EcScript::Hani },
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
    { 0x3400, 0x4DBF,
            "CJK Unified Ideographs Extension A",
            u8"ККЯ иероглифы расширение A"sv, {}, EcScript::Hani },
    { 0x4DC0, 0x4DFF,
            "Yijing Hexagram Symbols", u8"Гексаграммы И цзин (Книги перемен)" },
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
            "Modifier Tone Letters", u8"Символы изменения тона" },
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
    { 0xAAE0, 0xAAFF,
            "Meetei Mayek Extensions", u8"Манипури\u00A0— расширения" },
    { 0xAB00, 0xAB2F,
            "Ethiopic Extended-A", u8"Эфиопский расширенный А", {}, EcScript::Ethi },
    { 0xAB30, 0xAB6F,
            "Latin Extended-E", u8"Латиница расширенная E", {}, EcScript::Latn },
    { 0xAB70, 0xABBF,
            "Cherokee Supplement", u8"Чероки дополнительный", {}, EcScript::Cher },
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
    { 0xFB00, 0xFB4F,
            "Alphabetic Presentation Forms", u8"Алфавитные формы начертания"sv },
    { 0xFB50, 0xFDFF,
            "Arabic Presentation Forms-A", u8"Арабские формы начертания А",
            {}, EcScript::Arab },
    { 0xFE00, 0xFE0F,
            "Variation Selectors", u8"Селекторы начертания"sv },
    { 0xFE10, 0xFE1F,
            "Vertical Forms", u8"Вертикальные формы"sv,
            u8"<p>[[pop_scr:Hani|Китайский язык]] изначально писался по столбцам справа налево, и сейчас так пишут "
                    "в художественных целях: стихи, открытки, вывески."
                "<p>Блок кодирует символы из китайской кодировки GB{{_}}18030 (2005), предназначенные для вертикального письма."sv,
            EcScript::Hani },
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
    { 0xFFF0, 0xFFFF,
            "Specials", u8"Спецсимволы" },
    { 0x10000, 0x1007F,
            "Linear B Syllabary", u8"Линейное письмо Б\u00A0— слоги", {}, EcScript::Linb },
    { 0x10080, 0x100FF,
            "Linear B Ideograms", u8"Линейное письмо Б\u00A0— иероглифы", {}, EcScript::Linb },
    { 0x10100, 0x1013F,
            "Aegean Numbers", u8"Эгейские цифры" },
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
            "<p>Впоследствии аттическая система уступила место [[pop_scr:Grek|ионийской]] "
                "(Α=1, Β=2, …, Ι=10, Κ=20, …, Ρ=100, Σ=200…), "
                "но взаимообмен культур повлиял на этрусков, что и привело к появлению римских цифр."
            "<p>Также в этом блоке находятся символы древнегреческих единиц измерения."sv  },
    { 0x10190, 0x101CF,
            "Ancient Symbols", u8"Древние символы" },
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
    { 0x10280, 0x1029F,
            "Lycian", u8"Ликийский", {}, EcScript::Lyci },
    { 0x102A0, 0x102DF,
            "Carian", u8"Карийский", {}, EcScript::Cari },
    { 0x102E0, 0x102FF,
        "Coptic Epact Numbers", u8"Коптские курсивные цифры"sv,
        u8"<p>Здесь слово ''epact'' (греч. ἐπακτός) означает «заимствованные», а не «епакта» (формулы для перевода из солнечного "
                "календаря в лунный) или «пасхалия» (формулы для расчёта пасхи). "
                "И действительно, эти цифры, будучи потомками обычной [[pop_scr:Copt|коптской]] записи "
                "(которая, в свою очередь, позаимствована из [[pop_scr:Grek|греческого]]), широко использовались в копто-арабском мире "
                "с X по XIX век; в частности, в астрономических таблицах ≈1800. Запись слева направо."
            "<p>Из-за такого необычного использования эти цифры закодированы отдельным блоком, а не как вариант коптского."
            "<p>В изображениях символов вы увидите серьёзное расхождение с таблицами Юникода: специалист по древним "
                "письменностям ''Anshuman Pandey'' обвёл рисунки пером с растекающимися чернилами, а проект "
                "''Google Noto'' попытался повторить росчерки постоянной шириной."sv,
            EcScript::NONE, EcFont::NOTO_SYMBOL2 },
    { 0x10300, 0x1032F,
            "Old Italic", u8"Этрусский", {}, EcScript::Ital },
    { 0x10330, 0x1034F,
            "Gothic", u8"Готский", {}, EcScript::Goth },
    { 0x10350, 0x1037F,
            "Old Permic", u8"Древнепермский (анбур)", {}, EcScript::Perm },
    { 0x10380, 0x1039F,
            "Ugaritic", u8"Угаритский", {}, EcScript::Ugar },
    { 0x103A0, 0x103DF,
            "Old Persian", u8"Древнеперсидский", {}, EcScript::Xpeo },
    { 0x10400, 0x1044F,
            "Deseret", u8"Дезеретский (мормонский)", {}, EcScript::Dsrt },
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
    { 0x11FB0, 0x11FBF,
            "Lisu Supplement", u8"Лису дополнительный", {}, EcScript::Lisu },
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
    { 0x13000, 0x1342F,
            "Egyptian Hieroglyphs", u8"Египетские иероглифы", {}, EcScript::Egyp },
    { 0x13430, 0x1343F,
            "Egyptian Hieroglyph Format Controls",
            u8"Форматирующие символы для египетских иероглифов", {}, EcScript::Egyp },
    { 0x14400, 0x1467F,
            "Anatolian Hieroglyphs", u8"Анатолийские иероглифы", {}, EcScript::Hluw },
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
            "Kana Supplement", u8"Кана дополнительная" },   /// @todo [script] what are these three?
    { 0x1B100, 0x1B12F,
            "Kana Extended-A", u8"Кана расширенная A" },
    { 0x1B130, 0x1B16F,
            "Small Kana Extension", u8"Маленькие символы каны" },
    { 0x1B170, 0x1B2FF,
            "Nushu", u8"Нюй-шу", {}, EcScript::Nshu },
    { 0x1BC00, 0x1BC9F,
            "Duployan", u8"Стенография Дюплойе", {}, EcScript::Dupl },
    { 0x1BCA0, 0x1BCAF,
            "Shorthand Format Controls",
            u8"Форматирующие символы стенографии" },
    { 0x1D000, 0x1D0FF,
            "Byzantine Musical Symbols",
            u8"Византийские музыкальные символы" },
    { 0x1D100, 0x1D1FF,
            "Musical Symbols", u8"Музыкальные символы" },
    { 0x1D200, 0x1D24F,
            "Ancient Greek Musical Notation",
            u8"Древнегреческая музыкальная нотация" },
    { 0x1D2E0, 0x1D2FF,
            "Mayan Numerals", u8"Цифры майя" },
    { 0x1D300, 0x1D35F,
            "Tai Xuan Jing Symbols",
            u8"Символы Тайсюаньцзин (Канона великой тайны)" },
    { 0x1D360, 0x1D37F,
            "Counting Rod Numerals", u8"Счётные палочки" },
    { 0x1D400, 0x1D7FF,
            "Mathematical Alphanumeric Symbols",
            u8"Математические буквы и цифры" },
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
            u8"Обрамлённые иероглифы дополнительные" },
    { 0x1F300, 0x1F5FF,
            "Miscellaneous Symbols and Pictographs",
            u8"Разные символы и пиктограммы" },
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
            u8"Символы и пиктограммы дополнительные" },
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
                "<p>Также в этом блоке есть фишки китайской игры сянци."
        ""},
    { 0x1FA70, 0x1FAFF,
            "Symbols and Pictographs Extended-A",
            u8"Символы и пиктограммы расширенные А" },
    { 0x1FB00, 0x1FBFF,
            "Symbols for Legacy Computing",
            u8"Символы со старых компьютеров" },
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
    { 0xE0100, 0xE01EF,
            "Variation Selectors Supplement", u8"Селекторы начертания дополнительные" },
    //{ 0xE0100, 0xE01EF, "Check for error"sv },
};


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
