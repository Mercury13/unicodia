// What we are testing
#include "data.h"

// Google test
#include "gtest/gtest.h"

///
///  Simple test with senseless string
///
TEST (Decap, Simple)
{
    auto r = decapitalize("ALPHA BRAVO CHARLIE");
    EXPECT_EQ ("Alpha bravo charlie", r);
}


///
///  Capital letter A — A is a difficult thing
///
TEST (Decap, A1)
{
    auto r = decapitalize("LATIN CAPITAL LETTER A");
    EXPECT_EQ ("Latin capital letter A", r);
}


///
///  Commercial At is a dictionary
///
TEST (Decap, CommercialAt)
{
    auto r = decapitalize("COMMERCIAL AT");
    EXPECT_EQ ("commercial At", r);
}


///
///  Make somehow capitalize “Fraction One”
///
TEST (Decap, FractionNumeral)
{
    auto r = decapitalize("VULGAR FRACTION ONE QUARTER");
    EXPECT_EQ ("vulgar fraction One quarter", r);
}


///
///  Test some adjectives
///
TEST (Decap, Adjective)
{
    auto r = decapitalize("LATIN SMALL LETTER SHARP S");
    EXPECT_EQ("Latin small letter Sharp S", r);
}


///
///  Some random thing
///
TEST (Decap, Random1)
{
    auto r = decapitalize("LATIN SMALL LETTER N WITH ACUTE");
    EXPECT_EQ("Latin small letter N with acute", r);
}


///
///  Some random thing
///
TEST (Decap, Random2)
{
    auto r = decapitalize("LATIN CAPITAL LETTER D WITH SMALL LETTER Z WITH CARON");
    EXPECT_EQ("Latin capital letter D with small letter Z with caron", r);
}


///
///  Forgot Orkhon adjective
///
TEST (Decap, Orkhon)
{
    auto r = decapitalize("OLD TURKIC LETTER ORKHON OP");
    EXPECT_EQ("Old Turkic letter Orkhon Op", r);
}


///
///  Forgot Yenisei adjective
///
TEST (Decap, Yenisei)
{
    auto r = decapitalize("OLD TURKIC LETTER YENISEI OQ");
    EXPECT_EQ("Old Turkic letter Yenisei Oq", r);
}


///
///  Forgot Hanifi Rohingya two-word script
///
TEST (Decap, Hanifi)
{
    auto r = decapitalize("HANIFI ROHINGYA LETTER SA");
    EXPECT_EQ("Hanifi Rohingya letter Sa", r);
}


///
///  Check for other Hanifi thing
///
TEST (Decap, Hanifi2)
{
    auto r = decapitalize("HANIFI ROHINGYA MARK NA KHONNA");
    EXPECT_EQ("Hanifi Rohingya mark Na khonna", r);
}


///
///  Bug in data itself — wrong Tai key
///
TEST (Decap, TaiViet)
{
    auto r = decapitalize("TAI VIET VOWEL AY");
    EXPECT_EQ("Tai Viet vowel Ay", r);
}


///
///  Special thing in Tai Laing
///
TEST (Decap, TaiLaing)
{
    auto r = decapitalize("MYANMAR LETTER TAI LAING NYA");
    EXPECT_EQ("Myanmar letter Tai Laing Nya", r);
}


///
///  Digit is noun, so it is small
///
TEST (Decap, TaiLaing2)
{
    auto r = decapitalize("MYANMAR TAI LAING DIGIT SEVEN");
    EXPECT_EQ("Myanmar Tai Laing digit Seven", r);
}


///
///  Bug: sign final → sign Final
///
TEST (Decap, SignFinal)
{
    auto r = decapitalize("CHAM CONSONANT SIGN FINAL NG");
    EXPECT_EQ("Cham consonant sign Final Ng", r);
}


///
///  Bug: Khamti is a script name
///
TEST (Decap, Khamti)
{
    auto r = decapitalize("MYANMAR LETTER KHAMTI DDA");
    EXPECT_EQ("Myanmar letter Khamti Dda", r);
}


TEST (Decap, Danda)
{
    auto r = decapitalize("CHAM PUNCTUATION DANDA");
    EXPECT_EQ("Cham punctuation Danda", r);
}


TEST (Decap, KhamtiReduplication)
{
    auto r = decapitalize("MYANMAR MODIFIER LETTER KHAMTI REDUPLICATION");
    EXPECT_EQ("Myanmar modifier letter Khamti reduplication", r);
}


///
///  Forgot adjective “high”
///
TEST (Decap, HighAdjective)
{
    auto r = decapitalize("TAI VIET LETTER HIGH KHHO");
    EXPECT_EQ("Tai Viet letter High Khho", r);
}


///
///  Another synthetic test
///
TEST (Decap, SynthMultiAdj)
{
    auto r = decapitalize("TAI VIET LETTER HIGH THREE-D SYLLABIC KHHO");
    EXPECT_EQ("Tai Viet letter High three-D syllabic Khho", r);
}


///
///  Another synthetic test
///
TEST (Decap, SynthMultiAdj2)
{
    auto r = decapitalize("TAI VIET LETTER HIGH LOW SYLLABIC KHHO");
    EXPECT_EQ("Tai Viet letter High low syllabic Khho", r);
}


///
///  Fraction slash: exception
///
TEST (Decap, FractionSlash)
{
    auto r = decapitalize("FRACTION SLASH");
    EXPECT_EQ("fraction slash", r);
}


///
///  Fraction numerator One: check once again
///
TEST (Decap, FractionNumerator)
{
    auto r = decapitalize("FRACTION NUMERATOR ONE");
    EXPECT_EQ("fraction numerator One", r);
}


///
///  Fraction numerator One: check once again
///
TEST (Decap, NorthIndic)
{
    auto r = decapitalize("NORTH INDIC FRACTION ONE SIXTEENTH");
    EXPECT_EQ("North Indic fraction One sixteenth", r);
}


///
///  Forgot script “Sogdian”
///
TEST (Decap, Sogdian)
{
    auto r = decapitalize("OLD SOGDIAN FRACTION ONE HALF");
    EXPECT_EQ("Old Sogdian fraction One half", r);
}


///
///  Forgot script “Siyaq”
///
TEST (Decap, Siyaq)
{
    auto r = decapitalize("INDIC SIYAQ FRACTION THREE QUARTERS");
    EXPECT_EQ("Indic Siyaq fraction Three quarters", r);
}


///
///  Circled Katakana prefix
///
TEST (Decap, CircledKatakana)
{
    auto r = decapitalize("CIRCLED KATAKANA ALPHA BRAVO ALEF CHARLIE");
    EXPECT_EQ("circled Katakana Alpha bravo Alef charlie", r);
}


///
///  A = maybe letter
///
TEST (Decap, MathA)
{
    auto r = decapitalize("MATHEMATICAL BOLD CAPITAL A");
    EXPECT_EQ("mathematical bold capital A", r);
}


///
///  A = maybe letter
///
TEST (Decap, MathA1)
{
    auto r = decapitalize("MATHEMATICAL BOLD SMALL A");
    EXPECT_EQ("mathematical bold small A", r);
}


///
///  A = maybe letter, Fraktur = script
///
TEST (Decap, Fraktur)
{
    auto r = decapitalize("MATHEMATICAL FRAKTUR CAPITAL A");
    EXPECT_EQ("mathematical Fraktur capital A", r);
}


///
///  A = maybe letter
///  Flattened should not transfer uppercase to open
///
TEST (Decap, OpenA)
{
    auto r = decapitalize("COMBINING LATIN SMALL LETTER FLATTENED OPEN A ABOVE");
    EXPECT_EQ("combining Latin small letter Flattened open A above", r);
}


///
///  Works somehow
///
TEST (Decap, Turned)
{
    auto r = decapitalize("LATIN SMALL LETTER TURNED DELTA");
    EXPECT_EQ("Latin small letter Turned Delta", r);
}


///
///  What happens to inverted?
///
TEST (Decap, Inverted)
{
    auto r = decapitalize("MODIFIER LETTER SMALL CAPITAL INVERTED R");
    EXPECT_EQ("modifier letter Small capital inverted R", r);
}


///
///  Stop is noun
///
TEST (Decap, Stop)
{
    auto r = decapitalize("MODIFIER LETTER REVERSED GLOTTAL STOP");
    EXPECT_EQ("modifier letter Reversed glottal stop", r);
}


///
///  is noun
///
TEST (Decap, Candrabindu)
{
    auto r = decapitalize("DEVANAGARI SIGN INVERTED CANDRABINDU");
    EXPECT_EQ("Devanagari sign Inverted candrabindu", r);
}


///
///  Om is letter
///
TEST (Decap, Om)
{
    auto r = decapitalize("DEVANAGARI OM");
    EXPECT_EQ("Devanagari Om", r);
}


///
///  When NOT all are capitalized?
///
TEST (Decap, NotCap)
{
    auto r = decapitalize("DEVaNAGARI oM");
    EXPECT_EQ("Devanagari Om", r);
}


///
///  Au is letter
///
TEST (Decap, Au)
{
    auto r = decapitalize("BENGALI AU LENGTH MARK");
    EXPECT_EQ("Bengali Au length mark", r);
}


///
///  Special capitalization of custom adjectives
///
TEST (Decap, PhaseF)
{
    auto r = decapitalize("BAMUM LETTER PHASE-F KA");
    EXPECT_EQ("Bamum letter Phase-F Ka", r);
}


///
///  Bug: Tone 1
///  Because “Tone” word is multifunctional, you need to test it thoroughly.
///
TEST (Decap, ToneNkoApostrophe)
{
    auto r = decapitalize("NKO HIGH TONE APOSTROPHE");
    EXPECT_EQ("NKo high tone apostrophe", r);
}


///
///  Bug: Tone 2
///
TEST (Decap, ToneSix)
{
    auto r = decapitalize("LATIN CAPITAL LETTER TONE SIX");
    EXPECT_EQ("Latin capital letter Tone six", r);
}


///
///  Bug: Tone 3
///
TEST (Decap, ToneExtraHigh)
{
    auto r = decapitalize("MODIFIER LETTER EXTRA-HIGH TONE BAR");
    EXPECT_EQ("modifier letter Extra-high tone bar", r);
}


///
///  Bug: Tone 4
///
TEST (Decap, ToneGrave)
{
    auto r = decapitalize("COMBINING GRAVE TONE MARK");
    EXPECT_EQ("combining grave tone mark", r);
}


///
///  Bug: Tone 5
///
TEST (Decap, ToneNkoGrave)
{
    auto r = decapitalize("NKO COMBINING SHORT HIGH TONE");
    EXPECT_EQ("NKo combining short high tone", r);
}


///
///  Bug: Tone 6
///
TEST (Decap, ToneArabic)
{
    auto r = decapitalize("ARABIC TONE TWO DOTS ABOVE");
    EXPECT_EQ("Arabic tone Two dots above", r);
}


///
///  Bug: Tone 6a
///
TEST (Decap, ToneArabicA)
{
    auto r = decapitalize("ARABIC TONE LOOP BELOW");
    EXPECT_EQ("Arabic tone Loop below", r);
}


///
///  Bug: Tone 7
///
TEST (Decap, ToneLao)
{
    auto r = decapitalize("LAO TONE MAI TI");
    EXPECT_EQ("Lao tone Mai ti", r);
}


///
///  Bug: Tone 8
///
TEST (Decap, ToneMyanmar)
{
    auto r = decapitalize("MYANMAR TONE MARK SGAW KAREN HATHI");
    EXPECT_EQ("Myanmar tone mark Sgaw Karen Hathi", r);
}


///
///  Bug: Tone 9
///
TEST (Decap, ToneMyanmar1)
{
    auto r = decapitalize("MYANMAR SIGN SHAN COUNCIL EMPHATIC TONE");
    EXPECT_EQ("Myanmar sign Shan Council emphatic tone", r);
}


///
///  Bug: Tone 10
///
TEST (Decap, ToneMark1)
{
    auto r = decapitalize("NEW TAI LUE TONE MARK-1");
    EXPECT_EQ("New Tai Lue tone mark-1", r);
}


///
///  Bug: Tone 11
///
TEST (Decap, ToneVedic1)
{
    auto r = decapitalize("VEDIC TONE SHARA");
    EXPECT_EQ("Vedic tone Shara", r);
}


///
///  Bug: Tone 12
///
TEST (Decap, ToneVedic2)
{
    auto r = decapitalize("VEDIC TONE CANDRA BELOW");
    EXPECT_EQ("Vedic tone Candra below", r);
}


///
///  Bug: Tone 13
///
TEST (Decap, ToneVedic3)
{
    auto r = decapitalize("VEDIC TONE DOUBLE RING ABOVE");
    EXPECT_EQ("Vedic tone Double ring above", r);
}


///
///  Bug: Tone 14
///
TEST (Decap, ToneIdeo)
{
    auto r = decapitalize("IDEOGRAPHIC LEVEL TONE MARK");
    EXPECT_EQ("ideographic level tone mark", r);
}


///
///  Bug: Tone 15
///
TEST (Decap, ToneHangul)
{
    auto r = decapitalize("HANGUL SINGLE DOT TONE MARK");
    EXPECT_EQ("Hangul single dot tone mark", r);
}


///
///  Bug: Tone 16
///
TEST (Decap, ToneLisyu)
{
    auto r = decapitalize("LISU LETTER TONE MYA TI");
    EXPECT_EQ("Lisu letter Tone Mya ti", r);
}


///
///  Bug: Tone 17
///
TEST (Decap, ToneModChinese)
{
    auto r = decapitalize("MODIFIER LETTER CHINESE TONE YIN PING");
    EXPECT_EQ("modifier letter Chinese tone Yin ping", r);
}


///
///  Bug: Tone 18
///
TEST (Decap, ToneBar)
{
    auto r = decapitalize("MODIFIER LETTER MID DOTTED TONE BAR");
    EXPECT_EQ("modifier letter Mid dotted tone bar", r);
}


///
///  Bug: Tone 19
///
TEST (Decap, ToneStress)
{
    auto r = decapitalize("MODIFIER LETTER STRESS AND LOW TONE");
    EXPECT_EQ("modifier letter Stress and low tone", r);
}


///
///  Bug: Tone 20
///
TEST (Decap, ToneKayah)
{
    auto r = decapitalize("KAYAH LI TONE CALYA PLOPHU");
    EXPECT_EQ("Kayah Li tone Calya plophu", r);
}


///
///  Bug: Tone 21
///
TEST (Decap, ToneMyanmar2)
{
    auto r = decapitalize("MYANMAR SIGN PAO KAREN TONE");
    EXPECT_EQ("Myanmar sign Pao Karen tone", r);
}


///
///  Bug: Tone 22
///
TEST (Decap, ToneMyanmar3)
{
    auto r = decapitalize("MYANMAR SIGN TAI LAING TONE-2");
    EXPECT_EQ("Myanmar sign Tai Laing tone-2", r);
}


///
///  Bug: Tone 23
///
TEST (Decap, ToneMai)
{
    auto r = decapitalize("TAI VIET TONE MAI NUENG");
    EXPECT_EQ("Tai Viet tone Mai nueng", r);
}


///
///  Bug: Tone 26
///
TEST (Decap, ToneBassa)
{
    auto r = decapitalize("BASSA VAH COMBINING LOW-MID TONE");
    EXPECT_EQ("Bassa Vah combining low-mid tone", r);
}


// 27..29 are Miao-related and moved to manual exceptions
// See also test BugMiaoTone


///
///  Bug: Tone 30
///
TEST (Decap, ToneMusic1)
{
    auto r = decapitalize("MUSICAL SYMBOL QUARTER TONE SHARP");
    EXPECT_EQ("musical symbol Quarter tone sharp", r);
}


///
///  Bug: Tone 31
///
TEST (Decap, ToneWancho)
{
    auto r = decapitalize("WANCHO TONE TUPNI");
    EXPECT_EQ("Wancho tone Tupni", r);
}


///
///  Diversion: Tone1..Tone-9 are nouns
///
TEST (Decap, ToneNumeric1)
{
    auto r = decapitalize("MYANMAR SIGN KHAMTI TONE-3");
    EXPECT_EQ("Myanmar sign Khamti tone-3", r);
}


///
///  Found one more numeric tone, should be capital
///
TEST (Decap, ToneNumeric2)
{
    auto r = decapitalize("TAI LE LETTER TONE-5");
    EXPECT_EQ("Tai Le letter Tone-5", r);
}


///
///  One more trigger for numeric tones
///
TEST (Decap, ToneNumeric3)
{
    auto r = decapitalize("TAI THAM SIGN TONE-1");
    EXPECT_EQ("Tai Tham sign Tone-1", r);
}


///
///  Found one more numeric tone
///
TEST (Decap, ToneNumeric4)
{
    auto r = decapitalize("TAI THAM SIGN KHUEN TONE-3");
    EXPECT_EQ("Tai Tham sign Khuen tone-3", r);
}


///
///  Candra: reg. test 1
///
TEST (Decap, Candra1)
{
    auto r = decapitalize("DEVANAGARI LETTER CANDRA E");
    EXPECT_EQ("Devanagari letter Candra E", r);
}


///
///  Candra: reg. test 2
///
TEST (Decap, Candra2)
{
    auto r = decapitalize("DEVANAGARI VOWEL SIGN CANDRA LONG E");
    EXPECT_EQ("Devanagari vowel sign Candra long E", r);
}


///
///  Candra: reg. test 3
///
TEST (Decap, Candra3)
{
    auto r = decapitalize("BALINESE SIGN ULU CANDRA");
    EXPECT_EQ("Balinese sign Ulu candra", r);
}


TEST (Decap, Candra4)
{
    auto r = decapitalize("ZANABAZAR SQUARE SIGN CANDRA WITH ORNAMENT");
    EXPECT_EQ("Zanabazar square sign Candra with ornament", r);
}


TEST (Decap, Candra5)
{
    auto r = decapitalize("MASARAM GONDI SIGN CANDRA");
    EXPECT_EQ("Masaram Gondi sign Candra", r);
}


TEST (Decap, BugCanadian)
{
    auto r = decapitalize("CANADIAN SYLLABICS FINAL SMALL RING");
    EXPECT_EQ("Canadian syllabics Final small ring", r);
}


TEST (Decap, BugZanabazar)
{
    auto r = decapitalize("ZANABAZAR SQUARE MARK DOUBLE SHAD");
    EXPECT_EQ("Zanabazar square mark Double shad", r);
}


TEST (Decap, ArabicAlef)
{
    auto r = decapitalize("ARABIC LIGATURE ALEF MAKSURA WITH SUPERSCRIPT ALEF FINAL FORM");
    EXPECT_EQ("Arabic ligature Alef maksura with superscript Alef final form", r);
}


TEST (Decap, Tibetan1)
{
    auto r = decapitalize("TIBETAN DIGIT HALF ONE");
    EXPECT_EQ("Tibetan digit Half one", r);
}


///
///  Insular is adjective
///
TEST (Decap, TurnedInsular)
{
    auto r = decapitalize("LATIN SMALL LETTER TURNED INSULAR G");
    EXPECT_EQ("Latin small letter Turned insular G", r);
}


///
///  Pada is noun
///
TEST (Decap, Pada)
{
    auto r = decapitalize("JAVANESE PADA TIRTA TUMETES");
    EXPECT_EQ("Javanese pada Tirta tumetes", r);
}


///
///  RestSmall bhv: Cyrillic is script, G is always capital
///
TEST (Decap, RestSmall)
{
    auto r = decapitalize("SIGNWRITING CYRILLIC LETTER G");
    EXPECT_EQ("SignWriting Cyrillic letter G", r);
}


//
//  Word divider
//  Does not work because of strange status of “Word” word,
//   isn’t it better to check manually?
//
//TEST (Decap, WordDivider1)
//{
//    auto r = decapitalize("SIGNWRITING NUMERIC SIGN WORD DIVIDER");
//    EXPECT_EQ("SignWriting numeric sign Word divider", r);
//}


///
///  Word divider
///
TEST (Decap, WordDivider2)
{
    auto r = decapitalize("SIGNWRITING SIGN ASSYRIAN WORD DIVIDER");
    EXPECT_EQ("SignWriting sign Assyrian word divider", r);
}


///
///  Cuneiform prerequisites
///  That’s just how should decapitalize when cunei DOES NOT TRIGGER
///
TEST (Decap, CuneiPrereq)
{
    auto r = decapitalize("SIGNWRITING SIGN ASSYRIAN WORD DIVIDER OVER ASH");
    EXPECT_EQ("SignWriting sign Assyrian word divider over ash", r);
}


///
///  And when triggering cunei…
///
TEST (Decap, Cunei1)
{
    auto r = decapitalize("CUNEIFORM SIGN ASSYRIAN WORD DIVIDER OVER ASH TENU TIMES LAK-123");
    EXPECT_EQ("cuneiform sign Assyrian word divider over ASH Tenu times LAK-123", r);
}


///
///  What happens with Arabic?
///
TEST (Decap, ArabicBug)
{
    auto r = decapitalize("ARABIC LETTER AFRICAN QAF WITH THREE DOTS ABOVE");
    EXPECT_EQ("Arabic letter African Qaf with three dots above", r);
}


///
///  Bug: Cuneiform and Arabic interact strangely
///
TEST (Decap, BugCuneiTimesArabic)
{
    auto r = decapitalize("CUNEIFORM SIGN LAM TIMES KUR PLUS RU");
    EXPECT_EQ("cuneiform sign LAM times KUR plus RU", r);
}


///
///  Bug: Very Heavy Greek cross
///
TEST (Decap, BugGreekCross)
{
    auto r = decapitalize("VERY HEAVY GREEK CROSS");
    EXPECT_EQ("very heavy Greek cross", r);
}


///
///  Bug: Cypro-Minoan sign Cm005b, ad what to do?
///  Cause: custom decapitalization replaced originals, so you needed to write
///      ~~CYPRO-MINOAN~~ → Cypro-Minoan
///  Solution: original + isAsIs → customCap
///    Another clause related to Sutton SignWriting was discovered
///
TEST (Decap, BugCyproMinoan)
{
    auto r = decapitalize("CYPRO-MINOAN SIGN CM005B");
    EXPECT_EQ("Cypro-Minoan sign CM005B", r);
}



///
///  Bug: Miao tone Below
///
TEST (Decap, BugMiaoTone)
{
    auto r = decapitalize("MIAO TONE BELOW");
    EXPECT_EQ("Miao tone below", r);
}


///
///  Bug: Tag Digit Five
///
TEST (Decap, BugTagDigit)
{
    auto r = decapitalize("TAG DIGIT FIVE");
    EXPECT_EQ("tag Digit Five", r);
}


///
///  Bug: MODIFIER LETTER CYRILLIC SMALL BYELORUSSIAN-UKRAINIAN I
///  U15: “SMALL” is capital somehow
///
TEST (Decap, BugByeloUkr)
{
    auto r = decapitalize("MODIFIER LETTER CYRILLIC SMALL BYELORUSSIAN-UKRAINIAN I");
    EXPECT_EQ("modifier letter Cyrillic small Byelorussian-Ukrainian I", r);
}


///
///  Same as prev, “SMALL” used to be capital
///  Actual bug that persisted in U14
///
TEST (Decap, BugTahBelow)
{
    auto r = decapitalize("ARABIC LETTER HAH WITH SMALL ARABIC LETTER TAH BELOW");
    EXPECT_EQ("Arabic letter Hah with small Arabic letter Tah below", r);
}


///
///  Old and South are script parts
///
TEST (Decap, ScriptParts)
{
    auto r = decapitalize("OLD SOUTH ARABIAN LETTER RESH");
    EXPECT_EQ("Old South Arabian letter Resh", r);
}
