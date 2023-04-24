#include "UcData.h"

// Long and glitchy, that’s why I moved out from UcData

constexpr uc::SvgHint operator "" _hx (unsigned long long x)
    { return uc::SvgHint { static_cast<uint8_t>(x), 0 }; }

constexpr uc::SvgHint operator "" _hy (unsigned long long y)
    { return uc::SvgHint { 0, static_cast<uint8_t>(y) }; }

constinit const uc::Block uc::blocks[] {
    // Basic Latin OK
    { 0x0000, 0x007F, { 'L', EcContinent::EUROPE, {}, SvgHint { 5, 15, ImbaX::RIGHT_3 } },
            "Basic Latin",
            { EcScript::Latn, 0 },
            EcScript::Latn, EcFont::NORMAL,
            Bfg::HAS_DESCRIPTION | Bfg::HIPRIO_NUMBERS },
    // Latin-1 OK
    { 0x0080, 0x00FF, { 0xE4, EcContinent::EUROPE },
            "Latin-1 Supplement",
            { EcScript::Latn, 1 },
            EcScript::Latn, EcFont::NORMAL,
            Bfg::NO_EMOJI | Bfg::HAS_DESCRIPTION | Bfg::HIPRIO_NUMBERS },
    // Latin extended A OK
    { 0x0100, 0x017F, { 0x153, EcContinent::EUROPE, {}, 8_hy },
            "Latin Extended-A",
            { EcScript::Latn, 'A' },
            EcScript::Latn, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Latin extended B OK
    { 0x0180, 0x024F, { 0x188, EcContinent::EUROPE },
            "Latin Extended-B",
            { EcScript::Latn, 'B' },
            EcScript::Latn, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // IPA extensions OK
    { 0x0250, 0x02AF, { 0x259, EcContinent::NONE },
            "IPA Extensions", MyName::INST },
    // Spacing letter modifiers OK
    { 0x02B0, 0x02FF, { 0x2D0, EcContinent::NONE },
            "Spacing Modifier Letters",
            MyName::INST, EcScript::NONE, EcFont::NORMAL,
            Bfg::FORCE_FONT | Bfg::SCRIPTLIKE },
    // Basic umlauts OK
    { 0x0300, 0x036F, { 0x301, EcContinent::NONE },
            "Combining Diacritical Marks", { EcScript::ZDIA, 0 },
            EcScript::NONE, EcFont::FUNKY },
    // Greek and Coptic OK
    { 0x0370, 0x03FF, { L'ξ', EcContinent::EUROPE, Ifg::CONTINENT_OK },
        "Greek and Coptic", { EcScript::Grek, 0 } },
    // Cyrillic OK
    { 0x0400, 0x04FF, { 0x42F, EcContinent::EUROPE, {}, 12_hx },
            "Cyrillic",
            { EcScript::Cyrl, 0 },
            EcScript::Cyrl, EcFont::NORMAL, Bfg::UNGLITCH_MARKS },
    // Cyrillic supplement OK
    { 0x0500, 0x052F, { 0x524, EcContinent::EUROPE },
            "Cyrillic Supplement",
            { EcScript::Cyrl, 1 },
            EcScript::Cyrl, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Armenian OK
    { 0x0530, 0x058F, { 0x536, EcContinent::ASIA, {}, 14_hy },
            "Armenian", { EcScript::Armn, 0 }, EcScript::Armn },
    // Hebrew OK, managed to draw yod triangle
    { 0x0590, 0x05FF, { 0x5D0, EcContinent::ASIA },
            "Hebrew", MyName::INST, EcScript::Hebr },
    // Arabic OK
    { 0x0600, 0x06FF, { 0x636, EcContinent::ASIA },
            "Arabic", { EcScript::Arab, 0 }, EcScript::Arab },
    /// @todo [font] #86 Which font to select and what to do with Syriac Malayalam?
    { 0x0700, 0x074F, { 0x71B, EcContinent::ASIA },
            "Syriac", { EcScript::Syrc, 0 }, EcScript::Syrc },
    // Arabic supplement OK
    { 0x0750, 0x077F, { 0x762, EcContinent::ASIA },
            "Arabic Supplement",
            { EcScript::Arab, 1 },
            EcScript::Arab, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Thaana OK
    { 0x0780, 0x07BF, { 0x798, EcContinent::OCEAN },
            "Thaana", { EcScript::Thaa, 0 }, EcScript::Thaa },
    // N’ko OK
    { 0x07C0, 0x07FF, { 0x7D0, EcContinent::AFRICA },
            "NKo", { EcScript::Nkoo, 0 }, EcScript::Nkoo },
    // Samaritan OK
    { 0x0800, 0x083F, { 0x800, EcContinent::ASIA },
            "Samaritan", { EcScript::Samr, 0 }, EcScript::Samr },
    // Mandaic OK
    { 0x0840, 0x085F, { 0x84C, EcContinent::ASIA },
            "Mandaic", { EcScript::Mand, 0 }, EcScript::Mand },
    /// @todo [font] #86 Which font to select and what to do with Syriac Malayalam?
    { 0x0860, 0x086F, { 0x862, EcContinent::ASIA },
            "Syriac Supplement",
            { EcScript::Syrc, 1 },
            EcScript::Syrc, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Arabic ex B OK, Sceherazade updated in Nov 2021
    { 0x0870, 0x089F, { 0x877, EcContinent::ASIA },
            "Arabic Extended-B",
            { EcScript::Arab, 'B' },
            EcScript::Arab, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Arabic ex A OK
    { 0x08A0, 0x08FF, { 0x8A4, EcContinent::ASIA },
            "Arabic Extended-A",
            { EcScript::Arab, 'A' },
            EcScript::Arab, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Devanagari OK
    { 0x0900, 0x097F, { 0x915, EcContinent::ASIA, {}, SvgHint{ 9, 2 } },
            "Devanagari", { EcScript::Deva, 0 }, EcScript::Deva },
    // Bengali OK
    { 0x0980, 0x09FF, { 0x995, EcContinent::ASIA, {}, SvgHint{ 8, 2, ImbaY::BELOW_3 } },
            "Bengali", { EcScript::Beng, 0 }, EcScript::Beng },
    // Gurmukhi OK
    { 0x0A00, 0x0A7F, { 0xA15, EcContinent::ASIA },
            "Gurmukhi", { EcScript::Guru, 0 }, EcScript::Guru },
    // Gujarati OK, Google Noto + FORCE_TOFU show nothing
    { 0x0A80, 0x0AFF, { 0xA9B, EcContinent::ASIA },
            "Gujarati", { EcScript::Gujr, 0 }, EcScript::Gujr },
    // Oriya OK, Noto + everything bad manually fixed
    { 0x0B00, 0x0B7F, { 0xB1C, EcContinent::ASIA },
            "Oriya", { EcScript::Orya, 0 }, EcScript::Orya },
    // Tamil OK, font pair is good
    { 0x0B80, 0x0BFF, { 0xB95, EcContinent::ASIA, {}, SvgHint{ 8, 3, ImbaY::BELOW_4 } },
            "Tamil", { EcScript::Taml, 0 }, EcScript::Taml },
    // Telugu OK
    { 0x0C00, 0x0C7F, { 0xC35, EcContinent::ASIA },
            "Telugu", { EcScript::Telu, 0 }, EcScript::Telu },
    // Kannada OK
    { 0x0C80, 0x0CFF, { 0xCA0, EcContinent::ASIA },
            "Kannada", { EcScript::Knda, 0 }, EcScript::Knda },
    // Malayalam OK
    { 0x0D00, 0x0D7F, { 0xD16, EcContinent::ASIA, {}, SvgHint{ 15, 13 } },
            "Malayalam", { EcScript::Mlym, 0 }, EcScript::Mlym },
    // Sinhala OK
    { 0x0D80, 0x0DFF, { 0xD9A, EcContinent::OCEAN },
            "Sinhala", { EcScript::Sinh, 0 }, EcScript::Sinh },
    // Thai OK
    { 0x0E00, 0x0E7F, { 0xE0A, EcContinent::ASIA },
            "Thai", { EcScript::Thai, 0 }, EcScript::Thai },
    // Lao OK
    { 0x0E80, 0x0EFF, { 0xE9D, EcContinent::ASIA },
            "Lao", { EcScript::Laoo, 0 }, EcScript::Laoo },
    // Tibetan OK
    { 0x0F00, 0x0FFF, { 0xF4F, EcContinent::ASIA },
            "Tibetan", { EcScript::Tibt, 0 }, EcScript::Tibt },
    // Myanmar OK
    { 0x1000, 0x109F, { 0x101E, EcContinent::ASIA },
            "Myanmar", { EcScript::Mymr, 0 }, EcScript::Mymr },
    // Georgian OK
    { 0x10A0, 0x10FF, { 0x10D3, EcContinent::EUROPE },
            "Georgian", { EcScript::Geor, 0 }, EcScript::Geor },
    { 0x1100, 0x11FF, { 0x113F, EcContinent::CJK },
            "Hangul Jamo",
            { EcScript::Hang, 0 },
            EcScript::Hang, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Ethiopic OK
    { 0x1200, 0x137F, { 0x12F1, EcContinent::AFRICA },
            "Ethiopic", { EcScript::Ethi, 0 }, EcScript::Ethi },
    // Ethiopic supplement OK
    { 0x1380, 0x139F, { 0x1382, EcContinent::AFRICA },
            "Ethiopic Supplement",
            { EcScript::Ethi, 1 },
            EcScript::Ethi, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Cherokee OK
    { 0x13A0, 0x13FF, { 0x13B6, EcContinent::AMERICA },
            "Cherokee", { EcScript::Cher, 0 }, EcScript::Cher },
    // Canadian aboriginal OK
    { 0x1400, 0x167F, { 0x140E, EcContinent::AMERICA },
            "Unified Canadian Aboriginal Syllabics", { EcScript::Cans, 0 }, EcScript::Cans },
    // Ogham OK
    { 0x1680, 0x169F, { 0x168E, EcContinent::EUROPE },
            "Ogham", { EcScript::Ogam, 0 }, EcScript::Ogam },
    // Runic OK
    { 0x16A0, 0x16FF, { 0x16A0, EcContinent::EUROPE, {}, 3_hx },
            "Runic", { EcScript::Runr, 0 }, EcScript::Runr },
    // Tagalog OK
    { 0x1700, 0x171F, { 0x1706, EcContinent::OCEAN },
            "Tagalog", { EcScript::Tglg, 0 }, EcScript::Tglg },
    // Hanunoo OK
    { 0x1720, 0x173F, { 0x1726, EcContinent::OCEAN },
            "Hanunoo", { EcScript::Hano, 0 }, EcScript::Hano },
    // Buhid OK
    { 0x1740, 0x175F, { 0x1746, EcContinent::OCEAN },
            "Buhid", { EcScript::Buhd, 0 }, EcScript::Buhd },
    // Tagbanwa OK
    { 0x1760, 0x177F, { 0x1766, EcContinent::OCEAN },
            "Tagbanwa", { EcScript::Tagb, 0 }, EcScript::Tagb },
    // Khmer OK
    { 0x1780, 0x17FF, { 0x178D, EcContinent::ASIA },
            "Khmer", { EcScript::Khmr, 0 }, EcScript::Khmr },
    // Mongol OK, implemented ligature
    { 0x1800, 0x18AF, { 0x183E, EcContinent::ASIA, Ifg::APPROX_ROTATED, 11_hx },
            "Mongolian", { EcScript::Mong, 0 }, EcScript::Mong },
    // Canadian extended OK
    { 0x18B0, 0x18FF, { 0x18E5, EcContinent::AMERICA },
            "Unified Canadian Aboriginal Syllabics Extended",
            { EcScript::Cans, 1 },
            EcScript::Cans, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Limbu OK
    { 0x1900, 0x194F, { 0x1900, EcContinent::ASIA, {}, 15_hx },
            "Limbu", { EcScript::Limb, 0 }, EcScript::Limb },
    // Tai Le OK
    { 0x1950, 0x197F, { 0x195E, EcContinent::ASIA },
            "Tai Le", { EcScript::Tale, 0 }, EcScript::Tale },
    // New Tai Lue OK
    { 0x1980, 0x19DF, { 0x19C5, EcContinent::ASIA },
            "New Tai Lue", { EcScript::Talu, 0 }, EcScript::Talu },
    // Khmer syms OK
    { 0x19E0, 0x19FF, { 0x19EF, EcContinent::ASIA },
            "Khmer Symbols",
            { EcScript::Khmr, 1 },
            EcScript::Khmr, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Buginese OK
    { 0x1A00, 0x1A1F, { 0x1A01, EcContinent::OCEAN },
            "Buginese", { EcScript::Bugi, 0 }, EcScript::Bugi },
    // Lanna OK
    { 0x1A20, 0x1AAF, { 0x1A28, EcContinent::ASIA },
            "Tai Tham", { EcScript::Lana, 0 }, EcScript::Lana },
    // Diacritical ex OK
    { 0x1AB0, 0x1AFF, { 0x1AB2, EcContinent::NONE },
            "Combining Diacritical Marks Extended",
            { EcScript::ZDIA, 2 }, EcScript::NONE, EcFont::FUNKY },
    // Balinese OK
    { 0x1B00, 0x1B7F, { 0x1B11, EcContinent::OCEAN },
            "Balinese", { EcScript::Bali, 0 }, EcScript::Bali },
    // Sundanese OK
    { 0x1B80, 0x1BBF, { 0x1B91, EcContinent::OCEAN },
            "Sundanese", { EcScript::Sund, 0 }, EcScript::Sund },
    // Batak OK
    { 0x1BC0, 0x1BFF, { 0x1BD4, EcContinent::OCEAN },
            "Batak", { EcScript::Batk, 0 }, EcScript::Batk },
    // Lepcha OK
    { 0x1C00, 0x1C4F, { 0x1C1D, EcContinent::ASIA },
            "Lepcha", { EcScript::Lepc, 0 }, EcScript::Lepc },
    // Ol Chiki OK
    { 0x1C50, 0x1C7F, { 0x1C5A, EcContinent::ASIA },
            "Ol Chiki", { EcScript::Olck, 0 }, EcScript::Olck },
    // Cyr C OK
    { 0x1C80, 0x1C8F, { 0x1C88, EcContinent::EUROPE },
            "Cyrillic Extended-C",
            { EcScript::Cyrl, 'C' },
            EcScript::Cyrl, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Geor ex OK
    { 0x1C90, 0x1CBF, { 0x1C93, EcContinent::EUROPE, Ifg::APPROX_OTHER_LINES },
            "Georgian Extended",
            { EcScript::Geor, 2 },
            EcScript::Geor, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Sundanese supplement OK
    { 0x1CC0, 0x1CCF, { 0x1CC6, EcContinent::OCEAN },
            "Sundanese Supplement",
            { EcScript::Sund, 1 },
            EcScript::Sund, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Vedic ex OK
    { 0x1CD0, 0x1CFF, { 0x1CEC, EcContinent::ASIA, Ifg::CONTINENT_OK },
            "Vedic Extensions",
            MyName::INST, EcScript::NONE, EcFont::VEDIC },
    // Phonetic ext OK
    { 0x1D00, 0x1D7F, { 0x1D6F, EcContinent::NONE },
            "Phonetic Extensions",
            MyName::INST, EcScript::NONE, EcFont::NORMAL, Bfg::FORCE_FONT },
    // Phonetic ext supp OK
    { 0x1D80, 0x1DBF, { 0x1D95, EcContinent::NONE, {}, SvgHint{ 12, 6 } },
            "Phonetic Extensions Supplement", MyName::INST },
    // Combining supp OK, fixed manually in Noto
    { 0x1DC0, 0x1DFF, { 0x1DD4, EcContinent::NONE },
            "Combining Diacritical Marks Supplement",
            { EcScript::ZDIA, 1, }, EcScript::NONE, EcFont::NOTO },
    // Lat ex OK
    { 0x1E00, 0x1EFF, { 0x1EEA, EcContinent::EUROPE },
            "Latin Extended Additional",
            { EcScript::Latn, 2 },
            EcScript::Latn, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Greek ex OK, though DejaVu is a big gun
    { 0x1F00, 0x1FFF, { 0x1F6B, EcContinent::EUROPE },
            "Greek Extended",
            { EcScript::Grek, 1 },
            EcScript::Grek, EcFont::DEJAVU, Bfg::HAS_DESCRIPTION },
    // General punct OK, changed 2-dot leader to low one.
    { 0x2000, 0x206F, { L'‰', EcContinent::NONE },
            "General Punctuation",
            MyName::INST, EcScript::NONE, EcFont::PUNCTUATION },
    // Sup/sub OK
    { 0x2070, 0x209F, { 0x2074, EcContinent::NONE, Ifg::APPROX_2_CHARS },
            "Superscripts and Subscripts",
            MyName::INST, EcScript::NONE, EcFont::NORMAL, Bfg::HIPRIO_NUMBERS },
    // Curr symbols OK
    { 0x20A0, 0x20CF, { 0x20AC, EcContinent::NONE },
            "Currency Symbols", MyName::INST },
    // Marks for symbols OK, made optimized versions in Noto
    { 0x20D0, 0x20FF, { 0x20E0, EcContinent::NONE },
            "Combining Diacritical Marks for Symbols",
            { EcScript::ZDIA, 'z' }, EcScript::NONE, EcFont::UMLAUT_SYMBOL },
    // Letterlike OK
    { 0x2100, 0x214F, { 0x211D, EcContinent::NONE },
            "Letterlike Symbols",
            MyName::INST, EcScript::NONE, EcFont::NORMAL, Bfg::SCRIPTLIKE },
    // Number forms OK
    { 0x2150, 0x218F, { L'⅓', EcContinent::NONE },
            "Number Forms",
            MyName::INST, EcScript::NONE, EcFont::NORMAL, Bfg::HIPRIO_NUMBERS },
    // Arrows OK
    { 0x2190, 0x21FF, { L'↑', EcContinent::NONE, {}, SvgHint{ 7, ImbaX::LEFT_3 } },
            "Arrows",
            { EcScript::ZARR, 0 }, EcScript::NONE, EcFont::NORMAL, Bfg::NO_EMOJI },
    // Math op OK
    { 0x2200, 0x22FF, { L'√', EcContinent::NONE },
            "Mathematical Operators", { EcScript::ZMAT, 0 },
            EcScript::NONE, EcFont::MATH },
    // Misc tech OK
    { 0x2300, 0x23FF, { L'⏻', EcContinent::NONE },
            "Miscellaneous Technical",
            MyName::INST, EcScript::NONE, EcFont::TECHNICAL },
    // Control pictures OK
    { 0x2400, 0x243F, { L'␣', EcContinent::NONE },
            "Control Pictures",
            MyName::INST, EcScript::NONE, EcFont::NOTO_SYMBOL2_BIGGER },
    // OCR/MICR OK
    { 0x2440, 0x245F, { L'⑀', EcContinent::NONE },
            "Optical Character Recognition",
            // Fonts coincide
            MyName::INST, EcScript::NONE, EcFont::PHAISTOS_DISC },
    // Enclosed alnum OK, need ordinary Cambria
    { 0x2460, 0x24FF, { L'①', EcContinent::NONE, Ifg::CUSTOM_ENGINE },
            "Enclosed Alphanumerics",
            MyName::INST, EcScript::NONE, EcFont::NORMAL,
            Bfg::SCRIPTLIKE | Bfg::HIPRIO_NUMBERS },
    // Box drawing OK
    { 0x2500, 0x257F, { L'╢', EcContinent::NONE },
            "Box Drawing",
            MyName::INST, EcScript::NONE, EcFont::DEJAVU },
    // Block elements OK
    { 0x2580, 0x259F, { L'░', EcContinent::NONE, Ifg::CUSTOM_ENGINE },
            "Block Elements",
            MyName::INST, EcScript::NONE, EcFont::DEJAVU },
    // Geometric OK
    { 0x25A0, 0x25FF, { L'◆', EcContinent::NONE },
            "Geometric Shapes", MyName::INST },
    // Misc sym OK
    { 0x2600, 0x26FF, { L'☺', EcContinent::NONE },
            "Miscellaneous Symbols", MyName::INST },
    // Dingbats OK, need Cambria here!
    { 0x2700, 0x27BF, { L'❧', EcContinent::NONE },
            "Dingbats",  { EcScript::ZDIN, 0 },
            EcScript::NONE, EcFont::DINGBAT2, Bfg::HIPRIO_NUMBERS },
    // Misc math A OK
    { 0x27C0, 0x27EF, { L'⟈', EcContinent::NONE },
            "Miscellaneous Mathematical Symbols-A",
            { EcScript::ZMAT, 'A' }, EcScript::NONE, EcFont::MATH },
    // Arrows A OK
    { 0x27F0, 0x27FF, { L'⟳', EcContinent::NONE },
            "Supplemental Arrows-A", { EcScript::ZARR, 'A' } },
    // Braille OK
    { 0x2800, 0x28FF, { L'⠝', EcContinent::NONE },
            "Braille Patterns", { EcScript::Brai, 0 }, EcScript::Brai },
    // Arrows B OK
    { 0x2900, 0x297F, { L'⤶', EcContinent::NONE },
            "Supplemental Arrows-B", { EcScript::ZARR, 'B' } },
    // Misc math B OK
    { 0x2980, 0x29FF, { L'⧮', EcContinent::NONE },
            "Miscellaneous Mathematical Symbols-B",
            { EcScript::ZMAT, 'B' }, EcScript::NONE, EcFont::MATH },
    // Supp math ops OK
    { 0x2A00, 0x2AFF, { L'⨔', EcContinent::NONE },
            "Supplemental Mathematical Operators",
            { EcScript::ZMAT, 1 }, EcScript::NONE, EcFont::MATH },
    // Misc syms & arrows OK
    { 0x2B00, 0x2BFF, { L'⮊', EcContinent::NONE },
            "Miscellaneous Symbols and Arrows", MyName::INST,
            EcScript::NONE, EcFont::NORMAL_THEN_DEJAVU },
    // Glagolitic OK
    { 0x2C00, 0x2C5F, { 0x2C19, EcContinent::EUROPE },
            "Glagolitic", { EcScript::Glag, 0 }, EcScript::Glag },
    // Latin C OK
    { 0x2C60, 0x2C7F, { L'Ɱ', EcContinent::EUROPE },
            "Latin Extended-C",
            { EcScript::Latn, 'C' },
            EcScript::Latn, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Coptic OK
    { 0x2C80, 0x2CFF, { L'Ⲁ', EcContinent::AFRICA },
            "Coptic", { EcScript::Copt, 0 }, EcScript::Copt },
    // Georgian supp OK
    { 0x2D00, 0x2D2F, { L'ⴃ', EcContinent::EUROPE },
            "Georgian Supplement",
            { EcScript::Geor, 1 },
            EcScript::Geor, EcFont::GEORGIAN_NUSKHURI, Bfg::HAS_DESCRIPTION },
    // Tifinagh OK
    { 0x2D30, 0x2D7F, { L'ⵣ', EcContinent::AFRICA },
            "Tifinagh", { EcScript::Tfng, 0 }, EcScript::Tfng },
    // Ethiopic ex OK
    { 0x2D80, 0x2DDF, { L'ⶤ', EcContinent::AFRICA },
            "Ethiopic Extended",
            { EcScript::Ethi, 1 },
            EcScript::Ethi, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Cyr ex A OK, fixed those renderings in Noto
    { 0x2DE0, 0x2DFF, { 0x2DF0, EcContinent::EUROPE },
            "Cyrillic Extended-A",
            { EcScript::Cyrl, 'A' },
            EcScript::Cyrl, EcFont::NOTO, Bfg::HAS_DESCRIPTION },
    // Supp punct OK, drew U14 in FunkySample
    { 0x2E00, 0x2E7F, { L'⸘', EcContinent::NONE },
            "Supplemental Punctuation",
            MyName::INST, EcScript::NONE, EcFont::PUNCTUATION },
    // CJK radicals supp OK
    { 0x2E80, 0x2EFF, { L'⺓', EcContinent::CJK },
            "CJK Radicals Supplement",
            MyName::INST, EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Kangxi OK
    { 0x2F00, 0x2FDF, { L'⼊', EcContinent::CJK },
            "Kangxi Radicals",
            MyName::INST, EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Ideographic desc OK, not collapsible: small block
    { 0x2FF0, 0x2FFF, { L'⿺', EcContinent::CJK, Ifg::CONTINENT_OK | Ifg::CUSTOM_ENGINE },
            "Ideographic Description Characters",
            MyName::INST, EcScript::NONE, EcFont::CJK_STRUCTURE, Bfg::CJK },
    // CJK sym/punct OK
    // Cannot hint: if hint line y=3, then y=5 is too bad on 1.25×
    { 0x3000, 0x303F, { L'〠', EcContinent::CJK },
            "CJK Symbols and Punctuation",
            MyName::INST, EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Hiragana OK, installed small version of Noto CJK Korean
    { 0x3040, 0x309F, { L'ぢ', EcContinent::CJK },
            "Hiragana", { EcScript::Hira, 0 }, EcScript::Hira },
    // Katakana OK, not collapsible: syllabary
    { 0x30A0, 0x30FF, { L'ヂ', EcContinent::CJK },
            "Katakana", { EcScript::Kana, 0 }, EcScript::Kana },
    // Bopomofo OK
    { 0x3100, 0x312F, { L'ㄉ', EcContinent::CJK },
            "Bopomofo",
            { EcScript::Bopo, 0 },
            EcScript::Bopo, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // Hangul compat jamo OK
    { 0x3130, 0x318F, { L'ㅭ', EcContinent::CJK },
            "Hangul Compatibility Jamo",
            { EcScript::Hang, 1 },
            EcScript::Hang, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Kanbun OK, not collapsible: small block
    { 0x3190, 0x319F, { L'㆝', EcContinent::CJK, Ifg::CONTINENT_OK },
            "Kanbun",
            MyName::INST, EcScript::NONE, EcFont::CJK_KANBUN, Bfg::CJK },
    // Bopomofo OK, at least in W10
    { 0x31A0, 0x31BF, { L'ㆮ', EcContinent::CJK },
            "Bopomofo Extended",
            { EcScript::Bopo, 1 },
            EcScript::Bopo, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // CJK strokes OK
    { 0x31C0, 0x31EF, { L'㇉', EcContinent::CJK },
            "CJK Strokes",
            MyName::INST, EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Katakana phon OK, not collapsible: small block
    { 0x31F0, 0x31FF, { L'ㇹ', EcContinent::CJK },
            "Katakana Phonetic Extensions",
            { EcScript::Kana, 1 },
            EcScript::Kana, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Enclosed CJK letters and months OK
        { 0x3200, 0x32FF, { L'㋀', EcContinent::CJK, Ifg::CONTINENT_OK },
            "Enclosed CJK Letters and Months",
            MyName::INST, EcScript::NONE, EcFont::CJK_COMPAT,
            Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION | Bfg::HIPRIO_NUMBERS },
    // CJK compatibility OK
    { 0x3300, 0x33FF, { L'㌀', EcContinent::CJK },
            "CJK Compatibility",
            MyName::INST, EcScript::Hani, EcFont::CJK_COMPAT, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // CJK A OK
    { 0x3400, 0x4DBF, { L'㐾', EcContinent::CJK },
            "CJK Unified Ideographs Extension A",
            { EcScript::Hani, 'A' },
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Yijing OK
    { 0x4DC0, 0x4DFF, { L'䷧', EcContinent::CJK, Ifg::CONTINENT_OK | Ifg::CUSTOM_ENGINE },
            "Yijing Hexagram Symbols", MyName::INST },
    // CJK hieroglyphs OK
    { 0x4E00, 0x9FFF, { L'丈', EcContinent::CJK },
            "CJK Unified Ideographs",
            { EcScript::Hani, 0 },
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // Yi OK
    { 0xA000, 0xA48F, { 0xA011, EcContinent::ASIA },
            "Yi Syllables", { EcScript::Yiii, 0 }, EcScript::Yiii },
    // Yi radicals OK
    { 0xA490, 0xA4CF, { 0xA4BA, EcContinent::ASIA },
            "Yi Radicals",
            { EcScript::Yiii, 1 },
            EcScript::Yiii, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Lisu OK
    { 0xA4D0, 0xA4FF, { L'ꓤ', EcContinent::ASIA, {}, 13_hx },
            "Lisu", { EcScript::Lisu, 0 }, EcScript::Lisu },
    // Vai OK
    { 0xA500, 0xA63F, { L'ꔃ', EcContinent::AFRICA },
            "Vai", { EcScript::Vaii, 0 }, EcScript::Vaii },
    // Cyr ex B OK, modified font a bit
    { 0xA640, 0xA69F, { L'Ꙛ', EcContinent::EUROPE },
            "Cyrillic Extended-B",
            { EcScript::Cyrl, 'B' },
            EcScript::Cyrl, EcFont::NORMAL, Bfg::UNGLITCH_MARKS | Bfg::HAS_DESCRIPTION },
    // Bamum OK
    { 0xA6A0, 0xA6FF, { 0xA6AE, EcContinent::AFRICA },
            "Bamum", { EcScript::Bamu, 0 }, EcScript::Bamu },
    // Modifier tone OK
    { 0xA700, 0xA71F, { L'ꜛ', EcContinent::NONE, {}, 7_hx },
            "Modifier Tone Letters", MyName::INST },
    // Latin ex D OK, dereferenced bad chars of Google Noto, implemented U14 in Funky
    { 0xA720, 0xA7FF, { L'Ꝕ', EcContinent::EUROPE },
            "Latin Extended-D",
            { EcScript::Latn, 'D' },
            EcScript::Latn, EcFont::LATIN, Bfg::HAS_DESCRIPTION },
    // Syloti OK
    // Do not hint by X: we don’t want to grow a tree while the forest is dying
    { 0xA800, 0xA82F, { 0xA808, EcContinent::ASIA, {}, 3_hy },
            "Syloti Nagri", { EcScript::Sylo, 0 }, EcScript::Sylo },
    // Common Indic OK
    { 0xA830, 0xA83F, { 0xA835, EcContinent::ASIA },
            "Common Indic Number Forms",
            MyName::INST, EcScript::Deva, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Phags-pa OK
    { 0xA840, 0xA87F,
            { 0xA850, EcContinent::ASIA, Ifg::APPROX_ROTATED, SvgHint{ 15, 3, ImbaY::BELOW_4 } },
            "Phags-pa", { EcScript::Phag, 0 }, EcScript::Phag },
    // Saurashtra OK
    { 0xA880, 0xA8DF, { 0xA8A5, EcContinent::ASIA, {}, 10_hx },
            "Saurashtra", { EcScript::Saur, 0 }, EcScript::Saur },
    // Devanagari ex OK, drew 4 chars
    { 0xA8E0, 0xA8FF, { 0xA8EC, EcContinent::ASIA, {}, SvgHint{ 11, 1 } },
            "Devanagari Extended",
            { EcScript::Deva, 1 }, EcScript::Deva, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Kayah Li OK
    { 0xA900, 0xA92F, { 0xA922, EcContinent::ASIA, Ifg::APPROX_2_CHARS },
            "Kayah Li", { EcScript::Kali, 0 }, EcScript::Kali },
    // Rejang OK
    { 0xA930, 0xA95F, { 0xA93B, EcContinent::OCEAN },
            "Rejang", { EcScript::Rjng, 0 }, EcScript::Rjng },
    // Hangul jamo A OK
    { 0xA960, 0xA97F, { L'ꥵ', EcContinent::CJK },
            "Hangul Jamo Extended-A",
            { EcScript::Hang, 'A' },
            EcScript::Hang, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Javanese OK
    { 0xA980, 0xA9DF, { L'ꦈ', EcContinent::OCEAN },
            "Javanese", { EcScript::Java, 0 }, EcScript::Java },
    // Myanmar ex B OK
    { 0xA9E0, 0xA9FF, { L'ꧬ', EcContinent::ASIA },
            "Myanmar Extended-B",
            { EcScript::Mymr, 'B' },
            EcScript::Mymr, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Cham OK
    { 0xAA00, 0xAA5F, { 0xAA0C, EcContinent::ASIA },
            "Cham", { EcScript::Cham, 0 }, EcScript::Cham },
    // Myanmar ex A OK
    { 0xAA60, 0xAA7F, { L'ꩴ', EcContinent::ASIA, {}, 12_hx },
            "Myanmar Extended-A",
            { EcScript::Mymr, 'A' },
            EcScript::Mymr, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Tai Viet OK
    { 0xAA80, 0xAADF, { 0xAA8F, EcContinent::ASIA },
            "Tai Viet", { EcScript::Tavt, 0 }, EcScript::Tavt },
    // Meetei ex OK
    { 0xAAE0, 0xAAFF, { 0xAAE6, EcContinent::ASIA },
            "Meetei Mayek Extensions",
            { EcScript::Mtei, 1 },
            EcScript::Mtei, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Ethiopic ex A OK
    { 0xAB00, 0xAB2F, { 0xAB2A, EcContinent::AFRICA },
            "Ethiopic Extended-A",
            { EcScript::Ethi, 'A' },
            EcScript::Ethi, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Latin ex E OK, dereferenced bad chars of Google Noto
    { 0xAB30, 0xAB6F, { 0xAB66, EcContinent::EUROPE },
            "Latin Extended-E",
            { EcScript::Latn, 'E' },
            EcScript::Latn, EcFont::LATIN, Bfg::HAS_DESCRIPTION },
    // Cherokee supp OK
    { 0xAB70, 0xABBF, { L'ꮆ', EcContinent::AMERICA },
            "Cherokee Supplement",
            { EcScript::Cher, 1 },
            EcScript::Cher, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Meetei OK
    { 0xABC0, 0xABFF, { 0xABC4, EcContinent::ASIA, {}, 3_hy },
            "Meetei Mayek", { EcScript::Mtei, 0 }, EcScript::Mtei },
    // Hangul syllables OK
    { 0xAC00, 0xD7AF, { 0xAC13, EcContinent::CJK },
            "Hangul Syllables",
            { EcScript::Hang, 'Z' },
            EcScript::Hang, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Hangul Jamo B OK
    { 0xD7B0, 0xD7FF, { L'ힽ', EcContinent::CJK },
            "Hangul Jamo Extended-B",
            { EcScript::Hang, 'B' },
            EcScript::Hang, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // CJK compat hiero OK
    { 0xF900, 0xFAFF, { L'車', EcContinent::CJK },
            "CJK Compatibility Ideographs",
            { EcScript::Hani, 1 },
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Alphabetic presentation forms OK
    { 0xFB00, 0xFB4F, { L'ﬄ', EcContinent::NONE },
            "Alphabetic Presentation Forms", MyName::INST },
    // Arabic presentation forms A OK
    { 0xFB50, 0xFDFF, { 0xFB6D, EcContinent::ASIA },
            "Arabic Presentation Forms-A",
            { EcScript::Arab, 'a' },
            EcScript::Arab, EcFont::ARABIC_NOTO, Bfg::HAS_32_NONCHARS | Bfg::HAS_DESCRIPTION },
    // Variation selectors OK
    { 0xFE00, 0xFE0F,
            { 0xFE00, EcContinent::TECH, Ifg::APPROX_COLLECTIVE | Ifg::CONTINENT_OK | Ifg::FORMAT },
            "Variation Selectors", MyName::INST },
    // Vertical forms OK, not collapsible: small
    { 0xFE10, 0xFE1F, { L'︗', EcContinent::CJK, Ifg::CONTINENT_OK },
            "Vertical Forms",
            MyName::INST, EcScript::NONE, EcFont::CJK_STRUCTURE },
    // Combining half marks OK
    { 0xFE20, 0xFE2F, { 0xFE20, EcContinent::NONE, Ifg::APPROX_2_CHARS },
            "Combining Half Marks",
            MyName::INST, EcScript::NONE, EcFont::NOTO },
    // CJK compat forms OK
    { 0xFE30, 0xFE4F, { 0xFE3D, EcContinent::CJK },
            "CJK Compatibility Forms",
            MyName::INST, EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Small variants OK
    { 0xFE50, 0xFE6F, { L'﹖', EcContinent::CJK, Ifg::CONTINENT_OK },
            "Small Form Variants",
            MyName::INST, EcScript::NONE, EcFont::CJK, Bfg::COLLAPSIBLE },
    // Arabic forms B OK, no combinings
    { 0xFE70, 0xFEFF, { 0xFEA0, EcContinent::ASIA },
            "Arabic Presentation Forms-B",
            { EcScript::Arab, 'b' },
            EcScript::Arab, EcFont::ARABIC_NOTO, Bfg::HAS_DESCRIPTION },
    // Half/full OK
    { 0xFF00, 0xFFEF, { L'５', EcContinent::CJK, Ifg::CONTINENT_OK },
            "Halfwidth and Fullwidth Forms",
            MyName::INST, EcScript::NONE, EcFont::CJK,
            Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION | Bfg::HIPRIO_NUMBERS },
    // Specials OK
    { 0xFFF0, 0xFFFF, { L'�', EcContinent::TECH, Ifg::CONTINENT_OK },
            "Specials", MyName::INST, },
    // Linear B syll OK
    { 0x10000, 0x1007F, { 0x10002, EcContinent::EUROPE, {}, SvgHint { 7, 9 } },
            "Linear B Syllabary", { EcScript::Linb, 0 }, EcScript::Linb },
    // Linear B hier OK
    { 0x10080, 0x100FF, { 0x100E0, EcContinent::EUROPE },
            "Linear B Ideograms", { EcScript::Linb, 1 }, EcScript::Linb },
    // Aegean numbers OK
    { 0x10100, 0x1013F, { 0x1010D, EcContinent::EUROPE, Ifg::CUSTOM_ENGINE },
            "Aegean Numbers",
            { EcScript::Linb, 2 },
            EcScript::Linb, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Greek numbers OK
    { 0x10140, 0x1018F, { 0x10144, EcContinent::EUROPE },
        "Ancient Greek Numbers",
            { EcScript::Grek, 2 },
            EcScript::Grek, EcFont::NOTO_SYMBOL2, Bfg::HAS_DESCRIPTION },
    // Ancient symbols OK
    { 0x10190, 0x101CF, { 0x10192, EcContinent::EUROPE, Ifg::CONTINENT_OK },
            "Ancient Symbols",
            MyName::INST, EcScript::NONE, EcFont::NOTO_SYMBOL2 },
    // Phaistos Disc OK
    { 0x101D0, 0x101FF, { 0x101D1, EcContinent::EUROPE, Ifg::CONTINENT_OK },
        "Phaistos Disc",
            MyName::INST, EcScript::NONE, EcFont::PHAISTOS_DISC },
    // Lycian OK
    { 0x10280, 0x1029F, { 0x10299, EcContinent::ASIA },
            "Lycian", { EcScript::Lyci, 0 }, EcScript::Lyci },
    // Carian OK
    { 0x102A0, 0x102DF, { 0x102C6, EcContinent::ASIA },
            "Carian", { EcScript::Cari, 0 }, EcScript::Cari },
    // Coptic epact OK
    { 0x102E0, 0x102FF, { 0x102F1, EcContinent::AFRICA },
        "Coptic Epact Numbers",
            { EcScript::Copt, 1 },
            EcScript::Copt, EcFont::NOTO_SYMBOL2, Bfg::HAS_DESCRIPTION },
    // Old Italic OK
    { 0x10300, 0x1032F, { 0x10300, EcContinent::EUROPE, {}, 2_hx },
            "Old Italic", { EcScript::Ital, 0 }, EcScript::Ital },
    // Gothic OK
    { 0x10330, 0x1034F, { 0x10330, EcContinent::EUROPE },
            "Gothic", { EcScript::Goth, 0 }, EcScript::Goth },
    // Old Permic OK
    { 0x10350, 0x1037F, { 0x10350, EcContinent::EUROPE, {}, SvgHint{ 3, 1 } },
            "Old Permic", { EcScript::Perm, 0 }, EcScript::Perm },
    // Ugaritic OK
    { 0x10380, 0x1039F, { 0x10380, EcContinent::ASIA },
            "Ugaritic", { EcScript::Ugar, 0 }, EcScript::Ugar },
    // Old Persian OK
    { 0x103A0, 0x103DF, { 0x103A2, EcContinent::ASIA },
            "Old Persian", { EcScript::Xpeo, 0 }, EcScript::Xpeo },
    // Deseret OK
    { 0x10400, 0x1044F, { 0x10414, EcContinent::AMERICA, {}, 14_hx },
            "Deseret", { EcScript::Dsrt, 0 }, EcScript::Dsrt },
    // Shavian OK
    { 0x10450, 0x1047F, { 0x10459, EcContinent::EUROPE },
            "Shavian", { EcScript::Shaw, 0 }, EcScript::Shaw },
    // Osmanya OK
    { 0x10480, 0x104AF, { 0x10499, EcContinent::AFRICA },
            "Osmanya", { EcScript::Osma, 0 }, EcScript::Osma },
    // Osage OK
    { 0x104B0, 0x104FF, { 0x104C8, EcContinent::AMERICA },
            "Osage", { EcScript::Osge, 0 }, EcScript::Osge },
    // Elbasan OK
    { 0x10500, 0x1052F, { 0x10500, EcContinent::EUROPE },
            "Elbasan", { EcScript::Elba, 0 }, EcScript::Elba },
    // Caucasian OK
    { 0x10530, 0x1056F, { 0x10530, EcContinent::EUROPE },
            "Caucasian Albanian", { EcScript::Aghb, 0 }, EcScript::Aghb },
    // Vithkuqi OK
    { 0x10570, 0x105BF, { 0x10570, EcContinent::EUROPE },
            "Vithkuqi", { EcScript::Vith, 0 }, EcScript::Vith },
    // Linear A OK
    { 0x10600, 0x1077F, { 0x1062C, EcContinent::EUROPE },
            "Linear A", { EcScript::Lina, 0 }, EcScript::Lina },
    // Latin ex F OK, drew in FunkySample
    { 0x10780, 0x107BF, { 0x107BA, EcContinent::EUROPE, Ifg::APPROX_SQUARE },
            "Latin Extended-F",
            { EcScript::Latn, 'F' },
            EcScript::Latn, EcFont::FUNKY, Bfg::HAS_DESCRIPTION },
    // Cypriot OK
    { 0x10800, 0x1083F, { 0x1080E, EcContinent::EUROPE },
            "Cypriot Syllabary", { EcScript::Cprt, 0 }, EcScript::Cprt },
    // Imperial Aramaic OK
    { 0x10840, 0x1085F, { 0x10840, EcContinent::ASIA },
            "Imperial Aramaic", { EcScript::Armi, 0 }, EcScript::Armi },
    // Palmyrene OK
    { 0x10860, 0x1087F, { 0x10860, EcContinent::ASIA },
            "Palmyrene", { EcScript::Palm, 0 }, EcScript::Palm },
    // Nabataean OK
    { 0x10880, 0x108AF, { 0x10880, EcContinent::ASIA },
            "Nabataean", { EcScript::Nbat, 0 }, EcScript::Nbat },
    // Hatran OK
    { 0x108E0, 0x108FF, { 0x108E0, EcContinent::ASIA },
            "Hatran", { EcScript::Hatr, 0 }, EcScript::Hatr },
    // Phoenician OK
    { 0x10900, 0x1091F, { 0x10900, EcContinent::ASIA },
            "Phoenician", { EcScript::Phnx, 0 }, EcScript::Phnx },
    // Lydian OK
    { 0x10920, 0x1093F, { 0x10920, EcContinent::ASIA },
            "Lydian", { EcScript::Lydi, 0 }, EcScript::Lydi },
    // Meroitic hiero OK
    { 0x10980, 0x1099F, { 0x10980, EcContinent::AFRICA },
            "Meroitic Hieroglyphs", { EcScript::Mero, 0 }, EcScript::Mero },
    // Meroitic cursive OK
    { 0x109A0, 0x109FF, { 0x109A0, EcContinent::AFRICA },
            "Meroitic Cursive", { EcScript::Mero, 1 }, EcScript::Mero },
    // Kharoshthi OK
    { 0x10A00, 0x10A5F, { 0x10A10, EcContinent::ASIA },
            "Kharoshthi", { EcScript::Khar, 0 }, EcScript::Khar },
    // Old South Arab OK
    { 0x10A60, 0x10A7F, { 0x10A71, EcContinent::ASIA },
            "Old South Arabian", { EcScript::Sarb, 0 }, EcScript::Sarb },
    // Old North Arab OK
    { 0x10A80, 0x10A9F, { 0x10A91, EcContinent::ASIA, {}, 3_hy },
            "Old North Arabian", { EcScript::Narb, 0 }, EcScript::Narb },
    // Manichaean OK
    { 0x10AC0, 0x10AFF, { 0x10AC0, EcContinent::ASIA },
            "Manichaean", { EcScript::Mani, 0 }, EcScript::Mani },
    // Avestan OK
    { 0x10B00, 0x10B3F, { 0x10B00, EcContinent::ASIA },
            "Avestan", { EcScript::Avst, 0 }, EcScript::Avst },
    // Inscr Parthian OK
    { 0x10B40, 0x10B5F, { 0x10B40, EcContinent::ASIA, {}, SvgHint{ 15, 13 } },
            "Inscriptional Parthian", { EcScript::Prti, 0 }, EcScript::Prti },
    // Inscr Pahlavi OK
    { 0x10B60, 0x10B7F, { 0x10B60, EcContinent::ASIA },
            "Inscriptional Pahlavi", { EcScript::Phli, 0 }, EcScript::Phli },
    // Psalter Pahlavi OK
    /// @todo [alphasort] What to do? — At least one block should have Pahlavi in front
    { 0x10B80, 0x10BAF, { 0x10B99, EcContinent::ASIA, Ifg::APPROX_COLOR },
            "Psalter Pahlavi", { EcScript::Phlp, 0 }, EcScript::Phlp },
    // Turkic runes OK
    { 0x10C00, 0x10C4F, { 0x10C14, EcContinent::ASIA },
            "Old Turkic", { EcScript::Orkh, 0 }, EcScript::Orkh },
    // Hung runes OK
    { 0x10C80, 0x10CFF, { 0x10CAF, EcContinent::EUROPE },
            "Old Hungarian", { EcScript::Hung, 0 }, EcScript::Hung },
    // Hanifi OK
    { 0x10D00, 0x10D3F, { 0x10D0D, EcContinent::ASIA, {}, SvgHint{ 4, 15 } },
            "Hanifi Rohingya", { EcScript::Rohg, 0 }, EcScript::Rohg },
    // Rumi OK
    { 0x10E60, 0x10E7F, { 0x10E77, EcContinent::AFRICA, Ifg::CONTINENT_OK },
            "Rumi Numeral Symbols",
            MyName::INST, EcScript::NONE, EcFont::NOTO_SYMBOL2 },
    // Yezidi OK
    { 0x10E80, 0x10EBF, { 0x10E91, EcContinent::ASIA, {}, 11_hy },
            "Yezidi", { EcScript::Yezi, 0 }, EcScript::Yezi },
    // Arabic C OK
    { 0x10EC0, 0x10EFF, { 0x10EFF, EcContinent::ASIA },
            "Arabic Extended-C",
            { EcScript::Arab, 'C' },
            EcScript::Arab, EcFont::ARABIC, Bfg::HAS_DESCRIPTION },
    // Old Sogdian OK
    { 0x10F00, 0x10F2F, { 0x10F00, EcContinent::ASIA, {}, 14_hy },
            "Old Sogdian", { EcScript::Sogd, 0 }, EcScript::Sogo },
    // Sogdian OK
    { 0x10F30, 0x10F6F, { 0x10F30, EcContinent::ASIA },
            "Sogdian", { EcScript::Sogd, 1 }, EcScript::Sogd },
    // Old Uyghur OK
    { 0x10F70, 0x10FAF, { 0x10F70, EcContinent::ASIA },
            "Old Uyghur", { EcScript::Ougr, 0 }, EcScript::Ougr },
    // Chorasmian OK
    { 0x10FB0, 0x10FDF, { 0x10FB0, EcContinent::ASIA, {}, SvgHint{ 15, 11 } },
            "Chorasmian", { EcScript::Chrs, 0 }, EcScript::Chrs },
    // Elymaic OK
    { 0x10FE0, 0x10FFF, { 0x10FE0, EcContinent::ASIA, {}, 13_hy },
            "Elymaic", { EcScript::Elym, 0 }, EcScript::Elym },
    // Brahmi OK
    { 0x11000, 0x1107F, { 0x1101C, EcContinent::ASIA, {}, SvgHint{ 6, 1 } },
            "Brahmi", { EcScript::Brah, 0 }, EcScript::Brah },
    // Kaithi OK
    { 0x11080, 0x110CF, { 0x1108D, EcContinent::ASIA, {}, 7_hx },
            "Kaithi", { EcScript::Kthi, 0 }, EcScript::Kthi },
    // Sora OK
    { 0x110D0, 0x110FF, { 0x110D0, EcContinent::ASIA },
            "Sora Sompeng", { EcScript::Sora, 0 }, EcScript::Sora },
    // Chakma OK
    { 0x11100, 0x1114F, { 0x11110, EcContinent::ASIA },
            "Chakma", { EcScript::Cakm, 0 }, EcScript::Cakm },
    // Mahajani OK
    { 0x11150, 0x1117F, { 0x11158, EcContinent::ASIA },
            "Mahajani", { EcScript::Mahj, 0 }, EcScript::Mahj },
    // Sharada OK
    { 0x11180, 0x111DF, { 0x11191, EcContinent::ASIA, {}, SvgHint{ 7, 2 } },
            "Sharada", { EcScript::Shrd, 0 }, EcScript::Shrd },
    // Sinhala archaic OK
    // Noto Sinhala is light but archaic numbers are bold!
    { 0x111E0, 0x111FF, { 0x111E1, EcContinent::OCEAN },
            "Sinhala Archaic Numbers",
            { EcScript::Sinh, 1 },
            EcScript::Sinh, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Khojki OK
    { 0x11200, 0x1124F, { 0x11208, EcContinent::ASIA },
            "Khojki", { EcScript::Khoj, 0 }, EcScript::Khoj },
    // Multani OK
    { 0x11280, 0x112AF, { 0x11284, EcContinent::ASIA },
            "Multani", { EcScript::Mult, 0 }, EcScript::Mult },
    // Khuda OK
    { 0x112B0, 0x112FF, { 0x112BA, EcContinent::ASIA },
            "Khudawadi", { EcScript::Sind, 0 }, EcScript::Sind },
    // Grantha OK
    { 0x11300, 0x1137F, { 0x11315, EcContinent::ASIA },
            "Grantha", { EcScript::Gran, 0 }, EcScript::Gran },
    // Newa OK
    { 0x11400, 0x1147F, { 0x1140E, EcContinent::ASIA, {}, 2_hy },
            "Newa", { EcScript::Newa, 0 }, EcScript::Newa },
    // Tirhuta OK
    { 0x11480, 0x114DF, { 0x1148F, EcContinent::ASIA, {}, SvgHint{ 8, 3 } },
            "Tirhuta", { EcScript::Tirh, 0 }, EcScript::Tirh },
    // Siddham OK
    { 0x11580, 0x115FF, { 0x1158E, EcContinent::ASIA, {}, 9_hx },
            "Siddham", { EcScript::Sidd, 0 }, EcScript::Sidd },
    // Modi OK
    { 0x11600, 0x1165F, { 0x1160E, EcContinent::ASIA, {}, SvgHint{ 2, ImbaY::BELOW_3 } },
            "Modi", { EcScript::Modi, 0 }, EcScript::Modi },
    // Mongolian supp OK
    { 0x11660, 0x1167F, { 0x11668, EcContinent::ASIA, Ifg::APPROX_ROTATED },
            "Mongolian Supplement",
            { EcScript::Mong, 1 },
            EcScript::Mong, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Takri OK
    { 0x11680, 0x116CF, { 0x1168A, EcContinent::ASIA },
            "Takri", { EcScript::Takr, 0 }, EcScript::Takr },
    // Ahom OK
    { 0x11700, 0x1174F, { 0x11700, EcContinent::ASIA },
            "Ahom", { EcScript::Ahom, 0 }, EcScript::Ahom },
    // Dogra OK
    { 0x11800, 0x1184F, { 0x1180A, EcContinent::ASIA, {}, 1_hy },
            "Dogra", { EcScript::Dogr, 0 }, EcScript::Dogr },
    // Warang OK
    { 0x118A0, 0x118FF, { 0x118FF, EcContinent::ASIA },
            "Warang Citi", { EcScript::Wara, 0 }, EcScript::Wara },
    // Dives Akuru OK
    { 0x11900, 0x1195F, { 0x1190D, EcContinent::OCEAN },
            "Dives Akuru", { EcScript::Diak, 0 }, EcScript::Diak },
    // Nandi OK
    { 0x119A0, 0x119FF, { 0x119B4, EcContinent::ASIA, {}, SvgHint{ 9, 2 } },
            "Nandinagari", { EcScript::Nand, 0 }, EcScript::Nand },
    // Zanabazar square OK
    { 0x11A00, 0x11A4F, { 0x11A0B, EcContinent::ASIA },
            "Zanabazar Square", { EcScript::Zanb, 0 }, EcScript::Zanb },
    // Soyombo OK
    { 0x11A50, 0x11AAF, { 0x11A5C, EcContinent::ASIA, {}, SvgHint{ 13, 1 } },
            "Soyombo", { EcScript::Soyo, 0 }, EcScript::Soyo },
    // Canadian A OK
    { 0x11AB0, 0x11ABF, { 0x11AB0, EcContinent::AMERICA },
            "Unified Canadian Aboriginal Syllabics Extended-A",
            { EcScript::Cans, 'A' },
            EcScript::Cans, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Pau Cin Hau OK
    { 0x11AC0, 0x11AFF, { 0x11AC0, EcContinent::ASIA, {}, 9_hx },
            "Pau Cin Hau", { EcScript::Pauc, '0' }, EcScript::Pauc },
    /// @todo [future] Deva ex A temporarily in Funky
    { 0x11B00, 0x11B5F, { 0x11B07, EcContinent::ASIA },
            "Devanagari Extended-A",
            { EcScript::Deva, 'A' },
            EcScript::Deva, EcFont::FUNKY, Bfg::HAS_DESCRIPTION },
    // Bhaiksuki OK
    { 0x11C00, 0x11C6F, { 0x11C0E, EcContinent::ASIA },
            "Bhaiksuki", { EcScript::Bhks, 0 }, EcScript::Bhks },
    /// @todo [semi-tofu] #85 Marchen too small because of subjoining
    { 0x11C70, 0x11CBF,
            { 0x11C74, EcContinent::ASIA, {}, SvgHint{ 13, 1, ImbaY::ABOVE_1 } },
            "Marchen", { EcScript::Marc, 0 }, EcScript::Marc },
    { 0x11D00, 0x11D5F, { 0x11D0C, EcContinent::ASIA, {}, 7_hy },
            "Masaram Gondi", MyName::INST, EcScript::Gonm },
    { 0x11D60, 0x11DAF, { 0x11D7B, EcContinent::ASIA, {}, SvgHint{ 2, ImbaY::BELOW_4 } },
            "Gunjala Gondi", MyName::INST, EcScript::Gong },
    // Makasar OK
    { 0x11EE0, 0x11EFF, { 0x11EE4, EcContinent::OCEAN },
            "Makasar", { EcScript::Maka, 0 }, EcScript::Maka },
    // Kawi OK
    { 0x11F00, 0x11F5F, { 0x11F12, EcContinent::OCEAN },
            "Kawi", { EcScript::Kawi, 0 }, EcScript::Kawi },
    // Lisu supplement OK, new font engine works
    { 0x11FB0, 0x11FBF, { 0x11FB0, EcContinent::ASIA, {}, SvgHint{ 7, ImbaX::LEFT_2 } },
            "Lisu Supplement",
            { EcScript::Lisu, 1 },
            EcScript::Lisu, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Tamil supp OK
    { 0x11FC0, 0x11FFF, { 0x11FD8, EcContinent::ASIA },
            "Tamil Supplement",
            { EcScript::Taml, 1 },
            EcScript::Taml, EcFont::TAMIL_SUPPLEMENT, Bfg::HAS_DESCRIPTION },
    // Cuneiform OK
    { 0x12000, 0x123FF, { 0x1202D, EcContinent::ASIA },
            "Cuneiform", { EcScript::Xsux, 0 }, EcScript::Xsux },
    // Cuneiform nembers and punct OK
    { 0x12400, 0x1247F, { 0x1240A, EcContinent::ASIA },
            "Cuneiform Numbers and Punctuation",
            { EcScript::Xsux, 1 },
            EcScript::Xsux, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Early cuneiform OK
    { 0x12480, 0x1254F, { 0x12525, EcContinent::ASIA },
            "Early Dynastic Cuneiform", { EcScript::Xsux, 2 }, EcScript::Xsux },
    // Cypro-Minoan OK
    { 0x12F90, 0x12FFF, { 0x12FCC, EcContinent::EUROPE, Ifg::APPROX_HISTORICAL },
            "Cypro-Minoan", { EcScript::Cpmn, 0 }, EcScript::Cpmn },
    // Egyptian hiero OK
    { 0x13000, 0x1342F, { 0x1302F, EcContinent::AFRICA },
            "Egyptian Hieroglyphs", { EcScript::Egyp, 0 }, EcScript::Egyp },
    // Egyptian hiero format OK
    { 0x13430, 0x1345F, { 0x13434, EcContinent::AFRICA, Ifg::FORMAT },
            "Egyptian Hieroglyph Format Controls",
            { EcScript::Egyp, 1 },
            EcScript::Egyp, EcFont::FUNKY, Bfg::HAS_DESCRIPTION },
    // Anatolian hiero OK
    { 0x14400, 0x1467F, { 0x1446E, EcContinent::ASIA },
            "Anatolian Hieroglyphs", { EcScript::Hluw, 0 }, EcScript::Hluw },
    // Bamum supplement OK
    { 0x16800, 0x16A3F, { 0x16812, EcContinent::AFRICA },
            "Bamum Supplement",
            { EcScript::Bamu, 1 },
            EcScript::Bamu, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Mro OK
    { 0x16A40, 0x16A6F, { 0x16A52, EcContinent::ASIA },
            "Mro", { EcScript::Mroo, 0 }, EcScript::Mroo },
    // Tangsa OK, found the author
    { 0x16A70, 0x16ACF, { 0x16A86, EcContinent::ASIA, {}, 2_hx },
            "Tangsa", { EcScript::Tnsa, 0 }, EcScript::Tnsa },
    // Bassa OK
    { 0x16AD0, 0x16AFF, { 0x16AD0, EcContinent::AFRICA },
            "Bassa Vah", { EcScript::Bass, 0 }, EcScript::Bass },
    // Pahawh OK
    { 0x16B00, 0x16B8F, { 0x16B00, EcContinent::ASIA },
            "Pahawh Hmong", { EcScript::Hmng, 0 }, EcScript::Hmng },
    // Medefaidrin OK
    { 0x16E40, 0x16E9F, { 0x16E60, EcContinent::AFRICA, {}, SvgHint{ 3, ImbaY::BELOW_3 } },
            "Medefaidrin", { EcScript::Medf, 0 }, EcScript::Medf },
    // Miao OK
    { 0x16F00, 0x16F9F, { 0x16F03, EcContinent::ASIA, {}, SvgHint{ 12, 15, ImbaX::LEFT_4 } },
            "Miao", { EcScript::Plrd, 0 }, EcScript::Plrd },
    // Ideographic sym/punct OK
    { 0x16FE0, 0x16FFF, { 0x16FF0, EcContinent::CJK },
            "Ideographic Symbols and Punctuation",
            // Hard block, though small, but all chars have scripts.
            // Hani is just over-insurance.
            MyName::INST, EcScript::Hani, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Tangut OK
    { 0x17000, 0x187FF, { 0x17032, EcContinent::CJK, Ifg::APPROX_HISTORICAL },
            "Tangut",
            { EcScript::Tang, 0 },
            EcScript::Tang, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // Tangut components OK
    { 0x18800, 0x18AFF, { 0x18844, EcContinent::CJK, Ifg::APPROX_HISTORICAL },
            "Tangut Components",
            { EcScript::Tang, 1 },
            EcScript::Tang, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Khitan small OK
    { 0x18B00, 0x18CFF, { 0x18B4C, EcContinent::CJK, Ifg::APPROX_HISTORICAL },
            "Khitan Small Script",
            { EcScript::Kits, 0 },
            EcScript::Kits, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // Tangut supplement OK, not collapsible: too small
    { 0x18D00, 0x18D8F, { 0x18D00, EcContinent::CJK, Ifg::APPROX_HISTORICAL },
            "Tangut Supplement",
            { EcScript::Tang, 1 },
            EcScript::Tang, EcFont::NORMAL, Bfg::CJK | Bfg::HAS_DESCRIPTION },
    // Kana ex B OK: Made for myself tofu from GlyphWiki
    { 0x1AFF0, 0x1AFFF, { 0x1AFFB, EcContinent::CJK },
            "Kana Extended-B",
            { EcScript::Kana, 'B' },
            EcScript::Kana, EcFont::NORMAL, Bfg::CJK | Bfg::HAS_DESCRIPTION },
    // Kana supp OK
    { 0x1B000, 0x1B0FF, { 0x1B046, EcContinent::CJK },
            "Kana Supplement",
            { EcScript::Kana, '@' },
            EcScript::Hent, EcFont::KOREAN, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Kana ex A OK
    { 0x1B100, 0x1B12F, { 0x1B10E, EcContinent::CJK },
            "Kana Extended-A",
            { EcScript::Kana, 'A' },
            EcScript::Hent, EcFont::KOREAN, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Small kana ex OK
    { 0x1B130, 0x1B16F, { 0x1B150, EcContinent::CJK },
            "Small Kana Extension",
            { EcScript::Kana, 'Z' },
            EcScript::Kana, EcFont::KOREAN, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Nushu OK
    { 0x1B170, 0x1B2FF, { 0x1B1E0, EcContinent::CJK },
            "Nushu",
            { EcScript::Nshu, 0},
            EcScript::Nshu, EcFont::NORMAL, Bfg::COLLAPSIBLE },
    // Duployan OK
    { 0x1BC00, 0x1BC9F, { 0x1BC22, EcContinent::EUROPE },
            "Duployan", { EcScript::Dupl, 0}, EcScript::Dupl },
    // Shorthand format controls OK
    { 0x1BCA0, 0x1BCAF,
            { 0x1BCA3, EcContinent::EUROPE, Ifg::CONTINENT_OK | Ifg::FORMAT, SvgHint{ 7, ImbaX::LEFT_4 } },
            "Shorthand Format Controls",
            { EcScript::Dupl, 1},
            EcScript::NONE, EcFont::DUPLOYAN, Bfg::SCRIPTLIKE | Bfg::HAS_DESCRIPTION },
    // Znamenny OK, found cool font on Ponomar
    { 0x1CF00, 0x1CFCF, { 0x1CF50, EcContinent::EUROPE, Ifg::CONTINENT_OK },
            "Znamenny Musical Notation",
            MyName::INST, EcScript::NONE, EcFont::ZNAMENNY },
    // Byzantine music seemingly OK
    { 0x1D000, 0x1D0FF, { 0x1D035, EcContinent::EUROPE, Ifg::CONTINENT_OK },
            "Byzantine Musical Symbols",
            MyName::INST, EcScript::NONE, EcFont::MUSIC_BIGGER  },
    // Music OK
    { 0x1D100, 0x1D1FF, { 0x1D161, EcContinent::NONE, {}, 8_hx },
            "Musical Symbols",
            MyName::INST, EcScript::NONE, EcFont::MUSIC },
    // Greek music OK
    { 0x1D200, 0x1D24F, { 0x1D200, EcContinent::EUROPE, {}, 1_hx },
            "Ancient Greek Musical Notation",
            { EcScript::Grek, 3 }, EcScript::Grek, EcFont::MUSIC_NORMAL, Bfg::HAS_DESCRIPTION },
    // Kaktovik numerals OK
    { 0x1D2C0, 0x1D2DF, { 0x1D2CD, EcContinent::AMERICA, Ifg::CONTINENT_OK },
            "Kaktovik Numerals",
            MyName::INST, EcScript::NONE, EcFont::NOTO_SYMBOL2 },
    // Mayan numerals OK
    { 0x1D2E0, 0x1D2FF,
            { 0x1D2ED, EcContinent::AMERICA, Ifg::CONTINENT_OK, SvgHint{ 2, ImbaX::RIGHT_4 } },
            "Mayan Numerals",
            MyName::INST, EcScript::NONE, EcFont::NOTO_SYMBOL2 },
    // Tai Xuan Jing OK
    { 0x1D300, 0x1D35F, { 0x1D329, EcContinent::CJK, Ifg::CONTINENT_OK | Ifg::CUSTOM_ENGINE },
            "Tai Xuan Jing Symbols", MyName::INST },
    // Counting rods OK
    { 0x1D360, 0x1D37F, { 0x1D378, EcContinent::NONE, Ifg::CUSTOM_ENGINE },
            "Counting Rod Numerals",
            MyName::INST, EcScript::NONE, EcFont::NOTO_SYMBOL2_BIGGER },
    // Math alnum OK
    { 0x1D400, 0x1D7FF, { 0x1D4E0, EcContinent::NONE },
            "Mathematical Alphanumeric Symbols",
            { EcScript::ZMAT, 'a' }, EcScript::NONE, EcFont::MATH,
            Bfg::HIPRIO_NUMBERS },
    // Sutton SignWriting OK
    { 0x1D800, 0x1DAAF, { 0x1D8A0, EcContinent::NONE },
            "Sutton SignWriting", { EcScript::Sgnw, 0}, EcScript::Sgnw },
    // Latin ex G OK, drew in FunkySample
    { 0x1DF00, 0x1DFFF, { 0x1DF04, EcContinent::EUROPE, {}, SvgHint{ 7, 15 } },
            "Latin Extended-G",
            { EcScript::Latn, 'G'},
            EcScript::Latn, EcFont::FUNKY, Bfg::HAS_DESCRIPTION },
    // Glagolitic supp OK
    { 0x1E000, 0x1E02F, { 0x1E000, EcContinent::EUROPE },
            "Glagolitic Supplement",
            { EcScript::Glag, 1},
            EcScript::Glag, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Cyr ex D OK, really nice implementation in Funky
    { 0x1E030, 0x1E08F, { 0x1E047, EcContinent::EUROPE, Ifg::APPROX_SQUARE },
            "Cyrillic Extended-D",
            { EcScript::Cyrl, 'D'},
            EcScript::Cyrl, EcFont::FUNKY, Bfg::HAS_DESCRIPTION },
    // NP Hmong OK
    { 0x1E100, 0x1E14F, { 0x1E118, EcContinent::AMERICA, {}, 3_hx },
            "Nyiakeng Puachue Hmong", { EcScript::Hmnp, 0}, EcScript::Hmnp },
    // Toto OK
    { 0x1E290, 0x1E2BF, { 0x1E290, EcContinent::ASIA },
            "Toto", { EcScript::Toto, 0}, EcScript::Toto },
    // Wancho OK
    { 0x1E2C0, 0x1E2FF, { 0x1E2C0, EcContinent::ASIA },
            "Wancho", { EcScript::Wcho, 0}, EcScript::Wcho },
    // Nag OK, done in FunkySample
    { 0x1E4D0, 0x1E4FF, { 0x1E4D7, EcContinent::ASIA },
            "Nag Mundari", { EcScript::Nagm, 0}, EcScript::Nagm },
    // Ethiopic ex B OK, Noto quickly arrived
    { 0x1E7E0, 0x1E7FF, { 0x1E7FB, EcContinent::AFRICA, {}, SvgHint{ 14, ImbaY::ABOVE_4 } },
            "Ethiopic Extended-B",
            { EcScript::Ethi, 'B' },
            EcScript::Ethi, EcFont::NORMAL, Bfg::HAS_DESCRIPTION },
    // Mende Kikakui OK
    { 0x1E800, 0x1E8DF, { 0x1E800, EcContinent::AFRICA },
            "Mende Kikakui", { EcScript::Mend, 0}, EcScript::Mend },
    // Adlam OK
    { 0x1E900, 0x1E95F, { 0x1E902, EcContinent::AFRICA },
            "Adlam", { EcScript::Adlm, 0}, EcScript::Adlm },
    // Indic Siyaq OK
    { 0x1EC70, 0x1ECBF, { 0x1EC77, EcContinent::ASIA, Ifg::CONTINENT_OK },
            "Indic Siyaq Numbers",
            MyName::INST, EcScript::NONE, EcFont::SIYAQ_INDIC },
    // Ottoman Siyaq OK, Noto drew though it’s beta
    { 0x1ED00, 0x1ED4F, { 0x1ED0A, EcContinent::ASIA, Ifg::CONTINENT_OK },
            "Ottoman Siyaq Numbers",
            MyName::INST, EcScript::NONE, EcFont::SIYAQ_OTTOMAN },
    // Arab mathematical OK, currently we use Noto Math
    { 0x1EE00, 0x1EEFF, { 0x1EEA2, EcContinent::ASIA },
            "Arabic Mathematical Alphabetic Symbols",
            { EcScript::Arab, 'z'},
            EcScript::Arab, EcFont::MATH, Bfg::HAS_DESCRIPTION },
    // Mahjong tiles OK
    { 0x1F000, 0x1F02F, { 0x1F022, EcContinent::NONE, Ifg::APPROX_COLOR | Ifg::CUSTOM_ENGINE },
            "Mahjong Tiles",
            // Use emoji, with fallback to Phaistos Disc
            MyName::INST, EcScript::NONE, EcFont::MAHJONG_TILES },
    // Domino tiles OK
    { 0x1F030, 0x1F09F, { 0x1F043, EcContinent::NONE },
            "Domino Tiles",
            // Fonts coincide
            MyName::INST, EcScript::NONE, EcFont::PHAISTOS_DISC },
    // Cards OK
    { 0x1F0A0, 0x1F0FF, { 0x1F0B1, EcContinent::NONE, Ifg::APPROX_COLOR | Ifg::CUSTOM_ENGINE },
            "Playing Cards",
            MyName::INST, EcScript::NONE, EcFont::PHAISTOS_DISC },
    // Enclosed alnum OK, added a few symbols to FunkySample
    { 0x1F100, 0x1F1FF, { 0x1F19B, EcContinent::NONE, Ifg::CUSTOM_ENGINE },
            "Enclosed Alphanumeric Supplement",
            MyName::INST, EcScript::NONE, EcFont::DINGBAT, Bfg::SCRIPTLIKE },
    // Enclosed hiero OK
    { 0x1F200, 0x1F2FF,
            { 0x1F202, EcContinent::CJK, Ifg::CONTINENT_OK | Ifg::APPROX_COLOR | Ifg::CUSTOM_ENGINE },
            "Enclosed Ideographic Supplement",
            MyName::INST, EcScript::NONE, EcFont::DINGBAT },
    // Misc OK
    { 0x1F300, 0x1F5FF, { 0x1F52B, EcContinent::NONE },
            "Miscellaneous Symbols and Pictographs", { EcScript::ZSYM, 0 } },
    // Emoticons OK
    { 0x1F600, 0x1F64F, { 0x1F60D, EcContinent::NONE },
            "Emoticons",
            MyName::INST, EcScript::NONE, EcFont::DINGBAT },
    // Orna dingbats OK
    { 0x1F650, 0x1F67F, { 0x1F656, EcContinent::NONE },
            "Ornamental Dingbats", { EcScript::ZDIN, 1 },
            EcScript::NONE, EcFont::NOTO_SYMBOL2 },
    // Transport/map OK
    { 0x1F680, 0x1F6FF, { 0x1F697, EcContinent::NONE },
            "Transport and Map Symbols",
            MyName::INST, EcScript::NONE, EcFont::DINGBAT },
    // Alchem OK
    { 0x1F700, 0x1F77F, { 0x1F708, EcContinent::NONE },
            "Alchemical Symbols", MyName::INST },
    // Geo shapes ex OK
    { 0x1F780, 0x1F7FF, { 0x1F7B0, EcContinent::NONE },
            "Geometric Shapes Extended",
            MyName::INST, EcScript::NONE, EcFont::DINGBAT },
    // Arrows C OK
    { 0x1F800, 0x1F8FF, { 0x1F87A, EcContinent::NONE },
            "Supplemental Arrows-C", { EcScript::ZARR, 'C' } },
    // Supp sym/picto OK
    { 0x1F900, 0x1F9FF, { 0x1F98A, EcContinent::NONE },
            "Supplemental Symbols and Pictographs",
            { EcScript::ZSYM, 1 }, EcScript::NONE, EcFont::DINGBAT },
    // Chess OK, turned bad pawn
    { 0x1FA00, 0x1FA6F, { 0x1FA10, EcContinent::NONE },
            "Chess Symbols", MyName::INST, },
    // Sym and picto ext A OK
    { 0x1FA70, 0x1FAFF, { 0x1FA82, EcContinent::NONE },
            "Symbols and Pictographs Extended-A",
            { EcScript::ZSYM, 'A' }, EcScript::NONE, EcFont::DINGBAT },
    // Legacy OK
    { 0x1FB00, 0x1FBFF, { 0x1FBB2, EcContinent::NONE, Ifg::APPROX_2_CHARS | Ifg::CUSTOM_ENGINE },
            "Symbols for Legacy Computing",
            MyName::INST, EcScript::NONE, EcFont::NOTO_SYMBOL2_BIGGER,
            Bfg::HIPRIO_NUMBERS },
    // CJK B OK
    { 0x20000, 0x2A6DF, { 0x20024, EcContinent::CJK },
            "CJK Unified Ideographs Extension B",
            { EcScript::Hani, 'B' },
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // CJK C OK
    { 0x2A700, 0x2B73F, { 0x2A704, EcContinent::CJK },
            "CJK Unified Ideographs Extension C",
            { EcScript::Hani, 'C' },
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // CJK D OK
    { 0x2B740, 0x2B81F, { 0x2B742, EcContinent::CJK },
            "CJK Unified Ideographs Extension D",
            { EcScript::Hani, 'D' },
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // CJK E OK
    // Remove 2nd horizontal line from above from 2B283, and you’ll get
    //   common 4E1C “east”. But that’s probably feature, not bug
    { 0x2B820, 0x2CEAF, { 0x2B823, EcContinent::CJK },
            "CJK Unified Ideographs Extension E",
            { EcScript::Hani, 'E' },
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // CJK F OK
    { 0x2CEB0, 0x2EBEF, { 0x2CEB5, EcContinent::CJK },
            "CJK Unified Ideographs Extension F",
            { EcScript::Hani, 'F' },
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // CJK compat OK
    { 0x2F800, 0x2FA1F, { 0x2F81F, EcContinent::CJK },
            "CJK Compatibility Ideographs Supplement",
            { EcScript::Hani, 'X' },
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // CJK G OK, found a HanaMin version supporting CJK G
    { 0x30000, 0x3134F, { 0x3000C, EcContinent::CJK },
            "CJK Unified Ideographs Extension G",
            { EcScript::Hani, 'G' },
            EcScript::Hani, EcFont::NORMAL, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // CJK H OK, UnicodiaHan
    { 0x31350, 0x323AF, { 0x31378, EcContinent::CJK },
            "CJK Unified Ideographs Extension H",
            { EcScript::Hani, 'H' },
            EcScript::Hani, EcFont::CJK_NEWHAN, Bfg::COLLAPSIBLE | Bfg::HAS_DESCRIPTION },
    // Tags OK
    { 0xE0000, 0xE007F,
            { 0xE0054, EcContinent::TECH, Ifg::APPROX_COLLECTIVE | Ifg::CONTINENT_OK | Ifg::FORMAT },
            "Tags",
            MyName::INST, EcScript::NONE, EcFont::NORMAL, Bfg::SCRIPTLIKE },
    // Var sel supp OK
    { 0xE0100, 0xE01EF,
            { 0xE0100, EcContinent::TECH, Ifg::APPROX_COLLECTIVE | Ifg::CONTINENT_OK | Ifg::FORMAT },
            "Variation Selectors Supplement", MyName::INST },
};
static_assert(std::size(uc::blocks) == uc::N_BLOCKS,
              "Sorry, need to set block data manually");
