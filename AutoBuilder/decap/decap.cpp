// My header
#include "decap.h"

// C++
#include <vector>

// Common libs
#include "u_Strings.h"
#include "u_Vector.h"

using namespace std::string_view_literals;

#define EX(x) { TOUPPER_SV(x), decap::Exception{ .r{x}, .flags {} } },
#define EX2(x, fgs) { TOUPPER_SV(x), decap::Exception{ .r{x}, .flags{fgs} } },

const std::unordered_map<std::string_view, decap::Exception> exceptions{
    // EUROPEAN SCRIPTS
        // Cyrl
    EX("Cyrillic capital ligature En Ghe")   // All “ligature” are hand-checked
    EX("Cyrillic small ligature En Ghe")
    EX("Cyrillic capital ligature Te Tse")
    EX("Cyrillic small ligature Te Tse")
    EX("Cyrillic capital ligature A Ie")
    EX("Cyrillic small ligature A Ie")
    EX("Cyrillic \"beautiful Omega\"")  // Letter is BIG
    EX("voiceless l")               // letter is SMALL
    EX("voiceless r")               // letter is SMALL
    EX("palatalized l")             // letter is SMALL
    EX("palatalized n")             // letter is SMALL
        // Old Cyrl
    EX("old Cyrillic yest")         // letter is SMALL
    EX("old Cyrillic i")            // letter is SMALL
        // Dupl
    EX("Duployan affix Attached I hook")            // Same
    EX("Duployan thick letter selector")            // A clear miss of my algo, IDK what to do
    EX("Shorthand format Letter overlap")           // Letter is not keyword
        // Glag
    EX("Glagolitic capital letter Caudate Chrivi")  // Don’t want to encode Chrivi
    EX("Glagolitic small letter Caudate Chrivi")    // Same
        // Grek
    EX("Greek lunate epsilon symbol")   // Epsilon is small here
    EX("Greek reversed lunate epsilon symbol") // Same
    EX("Greek beta symbol")             // Beta is small here
    EX("Greek capital Kai symbol")      // Kai is capital here
    EX("Greek kai symbol")              // Kai is small here
    EX("Greek lunate sigma symbol")     // Sigma is small here
    EX("Greek rho with stroke symbol")  // Rho is small here
    EX("Greek one half sign alternate form")    // sign, alternate form
    EX("Greek non-spacing iota below")  // This letter is SMALL
    EX("iota subscript")                // This letter is SMALL
    EX("gamma function")                // Letter is big, function is small
    EX("Iota adscript")                 // Letter is BIG
    EX2("Lambda", decap::Exf::MIXCASE)
    EX("curled beta")                   // Letter is SMALL
    EX("script theta")                  // Letter is SMALL
    EX("omega pi")                      // Letter is SMALL
    EX("script kappa")                  // Letter is SMALL
    EX("tailed rho")                    // Letter is SMALL
    EX("straight epsilon")              // Letter is SMALL
    EX("reversed straight epsilon")     // Letter is SMALL
    EX("Antisigma")                     // Letter is BIG
    EX("Antisigma periestigmenon")      // Letter is BIG
    EX("Claudian Antisigma")            // Letter is BIG
    EX("Epidaurean acrophonic symbol Three")    // Vertical ellipsis
            // Greek capital reversed lunate Sigma symbol — OK, Sigma is cap
        // Latn
    EX2("I dot", decap::Exf::MIXCASE)
    EX2("I bar", decap::Exf::MIXCASE)
    EX2("O bar", decap::Exf::MIXCASE)
    EX2("Lambda bar", decap::Exf::MIXCASE)
    EX2("barred Lambda", decap::Exf::MIXCASE)
    EX2("Z bar", decap::Exf::MIXCASE)
    EX2("barred Z", decap::Exf::MIXCASE)
    EX("script f")              // This letter is SMALL
    EX("reversed Polish-hook o")    // This letter is SMALL
    EX2("O slash", decap::Exf::MIXCASE)
    EX2("barred L", decap::Exf::MIXCASE)
    EX2("D retroflex hook", decap::Exf::MIXCASE)
    EX2("Epsilon", decap::Exf::MIXCASE)
    EX2("closed reversed Epsilon", decap::Exf::MIXCASE)
    EX2("closed Epsilon", decap::Exf::MIXCASE)
    EX2("reversed Epsilon", decap::Exf::MIXCASE)
        // Runr
    EX("runic letter Dotted-N") // All are really tricky!!
    EX("runic letter Dotted-L")
    EX("runic letter Long-branch-Yr")
    EX("runic letter Short-twig-Yr")
    EX("runic letter Icelandic-Yr")
    EX("runic letter Dotted-P")
    EX("runic letter Open-P")
    EX("runic letter Sigel Long-branch-Sol S")
    EX("runic letter Short-twig-Sol S")
    EX("runic letter Long-branch-Ar Ae")
    EX("runic letter Short-twig-Ar A")
    EX("runic letter Long-branch-Hagall H")
    EX("runic letter Short-twig-Hagall H")
    EX("runic letter Short-twig-Bjarkan B")
    EX("runic letter Long-branch-Oss O")
    EX("runic letter Short-twig-Oss O")
    EX("runic letter Short-twig-Naud N")
    EX("runic letter Long-branch-Madr M")
    EX("runic letter Short-twig-Madr M")
    EX("runic letter Short-twig-Tyr T")
    EX("runic Arlaug symbol")
    EX("runic Tvimadur symbol")  // Same
    EX("runic Belgthor symbol")
        // Byzantine notes
    EX("Byzantine musical symbol Fthora diatoniki Pa")  // Pa is not just note

    // BRAHMIC SCRIPTS
        // Common-Indic
    EX("siddham")                               // siddham sign
    EX("vowel half-U")
        // Beng
    EX("Bengali letter Khanda Ta")  // Sword-like; because of Sikh character it’s tricky
    EX("Bengali Va")
        // Deva
    EX("Devanagari letter Candra A") // Both Candra and A are tricky, better to make an exception
    EX("Vedic tone Svarita")    // In NamesList.txt it’s in Mixed case → need
    EX("Vedic tone Anudatta")   // Same
        // Diak
    EX("Dives Akuru medial Ya")
    EX("Dives Akuru initial Ra")
    EX("Dives Akuru medial Ra")
        // Guru
    EX("Gurmukhi Iri")      // Same as Gurmukhi letter Uu
    EX("Gurmukhi Ura")      // Same
    EX("Gurmukhi Ek Onkar") // Ligature of phrase
        // Kthi
    EX("Kaithi number sign above")  // Number’s mark → number is not keyword
        // Lana
    EX("Tai Tham consonant sign High Ratha or Low Pa")
        // Mymr
    EX("Myanmar sign Pao Karen tone")  // What to do, “tone” is overloaded
        // Sinh
    EX("Sinhala vowel sign Kombuva haa Aela-pilla")     // Haa is also a letter
    EX("Sinhala vowel sign Kombuva haa diga Aela-pilla")
    EX("Sinhala vowel sign Kombuva haa gayanukitta")
        // Tglg
    EX("Zambales Ra")
        // Thai
    EX("Thai character No nu")      // Conflict with Greek Nu
    EX("mai may")                   // IDK what triggers May
        // Tibt
    EX("Tibetan astrological sign -Khyud pa")   // Because of hyphen (=apostrophe)
    EX("reversed Sha")
    EX("reversed subjoined Sha")
    EX("A-chung")
    EX("Ya-btags (Ya ta)")
    EX("Ra-btags (Ra ta)")
        // Zanb
    EX("Zanabazar square final consonant mark") // Some bugs

    // NON-BRAHMIC ASIAN AND OCEAN SCRIPTS
        // Arab
    EX("Arabic small low Waw")          // IDK what to do
    EX("Arabic small high Sad")
    EX("Arabic small high Ain")
    EX("Arabic small high Qaf")
    EX("Arabic small high Noon with kasra")
    EX("Arabic small low Noon with kasra")
    EX("Arabic small high Waw")
    EX("Arabic small high word Ar-rub") // “Word” is a tricky word
    EX("Arabic small high word Ath-thalatha")
    EX("Arabic small high word As-sajda")
    EX("Arabic small high word An-nisf")
    EX("Arabic small high word Sakta")
    EX("Arabic small high word Qif")
    EX("Arabic small high word Waqfa")
    EX("Arabic pound mark above")    // Mark is not special word here. You may call “above” an adverb, but no
    EX("Arabic piastre mark above")
    EX("Arabic letter Lam with small Arabic letter Tah above")  // Some bug
    EX("Arabic letter Teh with small Teh above")    // Tricky
    EX("Arabic letter Reh with small Noon above")   // Same
    EX("Arabic letter Yeh with two dots below and small Noon above")
    EX("Arabic letter Peh with small Meem above")
    EX("Arabic letter Beh with small Meem above")
    EX("Arabic-Indic Per mille sign")               // Tricky
    EX("Arabic-Indic Per ten thousand sign")        // Tricky
    EX("Arabic number mark above")              // Number’s mark
    EX("Arabic ligature Salla used as Koranic stop sign isolated form") // Sign is not a keyword
    EX("Arabic ligature Qala used as Koranic stop sign isolated form")  // Sign is not a keyword
    EX("Arabic letter Hamzah on Ha")        // Shall we make Ha a keyword?
        // Armn
    EX("Armenian small ligature Ech Yiwn")  // All “ligature” are manually checked
    EX("Armenian small ligature Men Now")
    EX("Armenian small ligature Men Ech")
    EX("Armenian small ligature Men Ini")
    EX("Armenian small ligature Vew Now")
    EX("Armenian small ligature Men Xeh")
        // Hebr
    EX("Hebrew Yod triangle")                       // IDK what to do
    EX("Hebrew ligature Yiddish Vav Yod")           // Same
    EX("Hebrew ligature Yiddish double Yod")
    EX("Hebrew ligature Yiddish double Vav")
    EX("Hebrew ligature Yiddish Yod Yod Patah")
    EX("Hebrew ligature Alef Lamed")
        // Mand
    EX("pharyngeal Hu")
    EX("I")
        // Mani
    EX("Manichaean abbreviation mark above")    // Abbreviation’s mark → abbreviation/mark are not keywords
    EX("Manichaean abbreviation mark below")    // The same
        // Mong
    EX("Mongolian Sibe syllable boundary marker")   // Syllable is not keyword
        // Pauc
    EX("Pau Cin Hau rising tone long")          // Tone is not keyword
    EX("Pau Cin Hau rising tone long final")    // Same
    EX("Pau Cin Hau rising tone final")
    EX("Pau Cin Hau sandhi tone long")
    EX("Pau Cin Hau sandhi tone long final")
    EX("Pau Cin Hau sandhi tone final")
    EX("Pau Cin Hau mid-level tone long final")
    EX("Pau Cin Hau mid-level tone final")
    EX("Pau Cin Hau low-falling tone long")
    EX("Pau Cin Hau low-falling tone long final")
    EX("Pau Cin Hau low-falling tone final")
        // Sogd
    EX("Sogdian independent Shin")          // Hebrew Shin / shin dot
        // Syrc
    EX("SAM")             // Syriac abbreviation mark
        // Thaa
    EX("Thaana Abafili")  // I think they should be capitalized
    EX("Thaana Aabaafili")
    EX("Thaana Ibifili")
    EX("Thaana Eebeefili")
    EX("Thaana Ubufili")
    EX("Thaana Ooboofili")
    EX("Thaana Ebefili")
    EX("Thaana Eybeyfili")
    EX("Thaana Obofili")
    EX("Thaana Oaboafili")

    // NEW ASIAN SCRIPTS
        // Hmnp
    EX("Nyiakeng Puachue Hmong circled Ca") // IDK what to do
        // Mong
    EX("Mongolian vowel separator")     // Vowel is not keyword
        // Olck
    EX("Ol Chiki mu ttuddag")           // Conflict with Greek Mu
        // Plrd
    EX("Miao sign Consonant modifier bar")  // Modifier is not keyword here
    EX("Miao tone right")     // Tone is not special here
    EX("Miao tone top right")
    EX("Miao tone above")
    EX("Miao tone below")
        // Hmng
    EX("Pahawh Hmong sign Xyeem rho")   // Conflict with Greek Rho

    // AMERICAN SCRIPTS
    EX("Canadian syllabics hyphen")     // What to do?

    // AFRICAN SCRIPTS
        // Adlm
    EX("Adlam Alif lengthener")     // Alif is a letter
    EX("Adlam vowel lengthener")    // Vowel is not keyword
        // Bamu
    EX("Bamum letter Phase-A Mon nggeuaet")     // Mon is also Myanmar nation
        // Copt
    EX("Coptic combining Ni above") // Ni is widely used in cuneiform, and IDK what to do
    EX("Coptic symbol kai")             // Seemingly small
    EX("Coptic symbol Mi Ro")
    EX("Coptic symbol Pi Ro")
    EX("Coptic symbol Tau Ro")
    EX("Coptic symbol Khi Ro")
    EX("Coptic symbol Shima Sima")
        // Medf
    EX("Medefaidrin exclamation Oh")  // Cannot make a rule for “exclamation”, too hard
        // Tfng
    EX("Adrar Yaj")
    EX("harpoon Yaz")
    EX("Tifinagh letter Berber Academy Yaj")
    EX("Tuareg Yab")
    EX("Tuareg Yaw")            

    // Misc letters
    EX2("A", decap::Exf::MIXCASE)
    EX2("B", decap::Exf::MIXCASE)
    EX("Ai")
    EX("Bb")
    EX("Bha")
    EX("Bs")
    EX2("C", decap::Exf::MIXCASE)
    EX("Ca")
    EX("Cha")
    EX2("Chi", decap::Exf::LEAVE_BY_CONDITION)
    EX("Chha")
    EX("soft Da")
    EX("soft Dda")
    EX("hard Da")
    EX("hard Dda")
    EX2("D", decap::Exf::MIXCASE)
    EX("Dd")
    EX("Dda")
    EX("Ddha")
    EX("Dha")
    { "ĕ", decap::Exception{ "Ĕ", {} } },  // A9BC
    EX("Ei")
    EX2("F", decap::Exf::MIXCASE)
    EX("Fu")
    EX2("G", decap::Exf::MIXCASE)
    EX("Gg")
    EX("Gha")
    EX("Gs")
    EX2("H", decap::Exf::MIXCASE)
    EX("Ha")
    EX("Ii")
    EX2("J", decap::Exf::MIXCASE)
    EX("Ja")
    EX("Jha")
    EX("Ji (not unique)")
    EX("Jj")
    EX("Jnya")
    EX2("K", decap::Exf::MIXCASE)
    EX("Ka")
    EX("Kaam")
    EX("Kai")
    EX("Kaim")
    EX("Kam")
    EX("Kau")
    EX("Kaum")
    EX("Kaw")
    EX("Kee")
    EX("Kem")
    EX("Keem")
    EX("Kha")
    EX("Kia")
    EX("Kim")
    EX("Ki")
    EX("Kog")
    EX("Kom")
    EX("Kua")
    EX("Kuam")
    EX("Kum")
    EX("Kwm")
    EX2("L", decap::Exf::MIXCASE)
    EX("La")
    EX("Lb")
    EX("Lg")
    EX("Lha")
    EX("Lm")
    EX("Ls")
    EX("Lt")
    EX("Lp")
    EX("Lh")
    EX("vocalic L")
    EX("vocalic Ll")
    EX2("M", decap::Exf::MIXCASE)
    EX("Ma")
    EX("subjoined Ma")
    EX2("Maa", decap::Exf::LEAVE_BY_CONDITION)
    EX("logosyllabic Muwa")
    EX2("N", decap::Exf::MIXCASE)
    EX("hard Na")
    EX("Ng")
    EX("Nga")
    EX("Nh")
    EX("Nha")
    EX("Nj")
    EX("Nna")
    EX2("P", decap::Exf::MIXCASE)
    EX("Pha")
    EX2("Pi", decap::Exf::LEAVE_BY_CONDITION)
    EX2("Q", decap::Exf::MIXCASE)
    EX("Qa")
    EX2("R", decap::Exf::MIXCASE)
    EX("vocalic R")
    EX("Ra")
    EX("medial Ra")
    EX("vocalic Rr")
    EX2("S", decap::Exf::MIXCASE)
    EX("Sad")
    EX("Sha")
    EX("Sheen")
    EX("Shi")
    EX("Ssa")
    EX("Ss")
    EX("soft Sha")
    EX2("T", decap::Exf::MIXCASE)
    EX("Ta")
    EX("Tha")
    EX("syllabic Ti")
    EX("Tra")
    EX("Tta")
    EX("Ttha")
    EX("Tsu")
    EX("small Tsu")
    EX("syllabic Tu")
    EX2("V", decap::Exf::MIXCASE)
    EX("Uu")
    EX("Wa")
    EX("subjoined Wa")
    EX("syllabic Wi(ya)")
    EX("Ya")
    EX("medial Ya")
    EX2("Z", decap::Exf::MIXCASE)
    EX("Zha")
    EX("Zu (not unique)")

    // CJK
        // Hani
    EX("CJK radical Small one")     // Small is not adjective here
    EX("CJK radical Small two")     // Same
    EX("CJK radical Second one")    // Second is not adjective, just reliability
    EX("CJK radical Second two")    // Same
    EX("CJK radical Second three")  // Same
    EX("CJK radical C-simplified Long") // Should I add an exception: last word is NOT adjective?
    EX("CJK radical Simplified Half tree trunk") // ???
    EX("CJK radical Long one")      // Long is not adjective here
    EX("CJK radical Long two")      // Same
    EX("CJK radical C-simplified Bird") // Bird is not noun
    EX("Kangxi radical Short thread") // Thread is noun here
    EX("Kangxi radical Dotted Cliff") // Reliability, sic! → see 2F1A
    EX("Kangxi radical Long stride") // Stride is noun here
    EX("Kangxi radical Dotted tent") // Dotted tent is just a shape, it was

        // Hang
    EX("Hangul choseong filler")    // Simpler to add exception
    EX("Hangul jungseong filler")   // Same
        // Decorations etc
    EX("rounded symbol for Fu")
    EX("rounded symbol for Lu")
    EX("rounded symbol for Shou")
    EX("rounded symbol for Xi")
    EX("rounded symbol for Shuangxi")
    EX("rounded symbol for Cai")
    EX("square Hiragana Hoka")  // Better exception?
    EX("squared Katakana Koko")
    EX("squared Katakana Sa")
    EX("kaeriten Re")
    EX("symbol of unification")
    EX("di ren")
    EX("ren di")

    // SPECIALS
        // Misc
    EX("commercial At")                 // At is not a preposition
    EX("tag Commercial At")             // Same
    EX("fullwidth commercial At")       // Same
    EX("small commercial At")           // Same
    EX("symbol for type A electronics") // A is tricky
    EX("combining ligature left half")  // Other meaning of ligature — horz parenthesis
    EX("combining ligature right half") // Same
    EX("combining ligature left half below")
    EX("combining ligature right half below")
    EX("combining Almost equal to below") // Tricky
    EX("modifier breve with inverted breve")    // Modifier
    EX("modifier letter Low circumflex accent") // What to do with circumflex?
    EX("modifier letter Short equals sign")     // What to do with equals?
    EX("direct current symbol form two") // Symbol is not a keyword
    EX("Roman numeral Reversed one hundred") // IDK what happens with numerals
    EX("AC current")                    // Alternating current current :)
    EX("ligature tie below")            // ligature tie = smile
    EX("ligature tie")                  // Same
    EX("symbol of Iran")                // Just glitch
        // Religion
    EX("Adi Shakti")
    EX("West Syriac cross")
    EX("East Syriac cross")
        // Math
    EX("small Element of")              // small sign “Element of”
    EX("small Contains as member")      // Same
    EX("small Element of with vertical bar at end of horizontal stroke")
    EX("small Element of with overbar")
    EX("small Contains with vertical bar at end of horizontal stroke")
    EX("small Contains with overbar")
    EX("Or with dot inside")            // Still same
    EX("And with dot")
    EX("angle with S inside")           // Because of Znamenny, S is tricky
    EX("perpendicular with S")          // Same
    EX("fraction slash")                // Fraction’s slash
    EX("sloping large Or")              // Or is a tricky word
    EX("sloping large And")             // And is a tricky word
    EX("Planck constant over two pi")   // pi is small here
    EX("invisible Times")               // ???
    EX("S in triangle")                 // Because of Znamenny, S is tricky
    EX("radical symbol bottom")         // symbol’s bottom
    EX("XOR")
    EX("NAND")
    EX("NOR")
    EX("Riemann integral")
    EX("Z notation Cartesian product")  // Z???
    EX("EMF (electromotive force)")
    EX("N-ary Dijkstra choice")
    EX("alef symbol")                   // ???
    EX("bet symbol")
    EX("gimel symbol")
    EX("dalet symbol")
    EX("nabla")                         // Other nablas are capital
        // Astronomy
    EX("Earth")                         // Planet, not element
    EX2("Sun", decap::Exf::CPONLY)      // CP: star; Emoji: weather
    EX("Mercury")                       // Planet, not metal
    EX("Cancer")                        // Constellation, not disease
        // Emoji
    EX("Earth globe Europe-Africa")     // Special proper name
    EX("Earth globe Americas")          // Special proper name
    EX("Earth globe Asia-Australia")    // Special proper name
    EX("Milky Way")                     // Special proper name
    EX("Tokyo Tower")                   // Special proper name
    EX("Mount Fuji")                    // Special proper name
    EX("Statue of Liberty")             // Special proper name
    EX("man with gua pi mao")           // Conflict with Greek Pi
    EX("face with No good gesture")     // Complex rules, better manually
    EX("face with OK gesture")          // Complex rules, better manually
    EX("OK hand sign")                  // OK is both a syllable and an emoji
    EX("turned OK hand sign")           // The same
    EX("Call me hand")                  // “Call me” hand
    EX("I love you hand sign")          // same + I
    EX("letter box")                    // Letter is a paper
    EX("ID")                            // Just two letters
        // Formatting
    EX("national digit shapes")   // Digit is not special here
    EX("nominal digit shapes")    // Same
        // Music
    EX("musical symbol With fingernails")       // With is not a preposition
    EX("musical symbol Quarter rest")           // Quarter is a tricky word
    EX("musical symbol Quarter note")           // The same
    EX("musical symbol Left repeat sign")       // Just manually, 3 chars
    EX("musical symbol Right repeat sign")      // The same
    EX("musical symbol Half pedal mark")        // The same
    EX("musical symbol Combining augmentation dot")  // Augmentation is a future landmine
        // Music/Znamenny
    EX("Znamenny neume Dva v chelnu")  // Better not to make exception from V
        // Games
    EX("mahjong tile back")
    // domino tile Horizontal/Vertical back probably OK
    EX("playing card back")

    // B-I-G  S-E-T-S

    // Mathematical alphabets
#define MATH_SYM(x) \
        EX("Greek " x " symbol") \
        EX("mathematical bold " x " symbol") \
        EX("mathematical italic " x " symbol") \
        EX("mathematical bold italic " x " symbol") \
        EX("mathematical sans-serif bold " x " symbol") \
        EX("mathematical sans-serif bold italic " x " symbol")

    MATH_SYM ("epsilon")    // Small epsilon etc here
    MATH_SYM ("theta")
    MATH_SYM ("kappa")
    MATH_SYM ("phi")
    MATH_SYM ("rho")
    MATH_SYM ("pi")
#undef MATH_SYM
    EX("Arabic mathematical Sad")    // Ambiguous
    EX("Arabic mathematical Sheen")  // Ambiguous

    // Legacy
    EX("NPN transistor")
    EX("PNP transistor")
    EX("logic gate Or")
    EX("logic gate And")

    // Hand-checked CJK Latin
    EX("squared HV")                            // Just manually, small block
    EX("squared MV")
    EX("squared SD")
    EX("squared SS")
    EX("squared PPV")
    EX("squared WC")
    EX("square Corporation")                    // IDK what’s happening
    EX("square hPa")                            // A whole cohort of Chinese units
    EX("square IU")
    EX("square da")
    EX("square AU")
    EX("square bar")
    EX("square oV")
    EX("square pc")
    EX("square dm squared")
    EX("square pA amps")
    EX("square nA")
    EX("square mu A")
    EX("square mA")
    EX("square kA")
    EX("square KB")
    EX("square MB")
    EX("square GB")
    EX("square cal")
    EX("square kcal")
    EX("square pF")
    EX("square nF")
    EX("square mu F")
    EX("square mu G")
    EX("square mg")
    EX("square kg")
    EX("square Hz")
    EX("square kHz")
    EX("square MHz")
    EX("square GHz")
    EX("square THz")
    EX("square mu l")
    EX("square ml")
    EX("square dl")
    EX("square kl")
    EX("square fm")
    EX("square nm")
    EX("square mu m")
    EX("square mm")
    EX("square cm")
    EX("square km")
    EX("square mm squared")
    EX("square cm squared")
    EX("square m squared")
    EX("square km squared")
    EX("square mm cubed")
    EX("square m cubed")
    EX("square km cubed")
    EX("square m over s")
    EX("square m over s squared")
    EX("square Pa")
    EX("square kPa")
    EX("square MPa")
    EX("square GPa")
    EX("square rad")
    EX("square rad over s")
    EX("square rad over s squared")
    EX("square ps")
    EX("square ns")
    EX("square mu s")
    EX("square ms")
    EX("square pV")
    EX("square nV")
    EX("square mu V")
    EX("square mV")
    EX("square kV")
    EX("square MV mega")
    EX("square pW")
    EX("square nW")
    EX("square mu W")
    EX("square mW")
    EX("square kW")
    EX("square MW mega")
    EX("square k Ohm")
    EX("square M Ohm")
    EX("square am")
    EX("square Bq")
    EX("square cc")
    EX("square cd")
    EX("square C over kg")
    EX("square Co")
    EX("square dB")
    EX("square Gy")
    EX("square ha")
    EX("square HP")
    EX("square in")
    EX("square KK")
    EX("square KM capital")
    EX("square kt")
    EX("square lm")
    EX("square ln")
    EX("square log")
    EX("square lx")
    EX("square mb small")
    EX("square mil")
    EX("square mol")
    EX("square pH")
    EX("square pm")
    EX("square PPM")
    EX("square PR")
    EX("square sr")
    EX("square Sv")
    EX("square Wb")
    EX("square V over m")   // Volt/metre
    EX("square A over m")   // Ampere/metre
    EX("square gal")
    EX("square Hg")
    EX("square erg")
    EX("square eV")
    EX("square DJ")

        // Enclosed Latin
    EX("circled CC")            // Closed Captions, hand-checked
    EX("circled CD")            // Compact disc? hand-checked
    EX("circled Wz")            // German Warenzeichen
    EX("negative squared IC")   // Hand-checked
    EX("negative squared PA")
    EX("negative squared SA")
    EX("negative squared AB")
    EX("negative squared WC")
    EX("raised MC sign")        // Hand-checked
    EX("raised MD sign")
    EX("raised MR sign")
    EX("squared OK")            // OK is both a syllable and an emoji
    EX("squared SOS")           // The same
    EX("squared CL")            // Squared is a tricky word
    EX("squared Cool")
    EX("squared New")
    EX("squared No good")
    EX("squared Up with exclamation mark")
    EX("squared Three D")
    EX("squared Second screen")
    EX("squared Two K")
    EX("squared Four K")
    EX("squared Eight K")
    EX("squared Five point one")
    EX("squared Seven point one")
    EX("squared Twenty-two point two")
    EX("squared Sixty P")
    EX("squared One hundred twenty P")
    EX("squared HC")
    EX("squared Vs")
    EX("squared New")
    EX("squared NG")
    EX("squared Free")
    EX("squared ID")
    EX("squared Hi-res")
    EX("squared HDR")
    EX("squared SHV")
    EX("squared VOD")
    EX("squared UHD")
    EX("squared Lossless")
    EX("Back with leftwards arrow above")
    EX("End with leftwards arrow above")
    EX("On with exclamation mark with left right arrow above")
    EX("Soon with rightwards arrow above")
    EX("Top with upwards arrow above")
    EX("keycap Ten")
};

/// @warning DO NOT REMOVE sv, w/o does not work and IDK how to ban
const std::multiset<IdiomEntry> idioms {
    // Prefixes
    { { "APL"sv, "FUNCTIONAL"sv, "SYMBOL"sv }, IsPrefix::YES, IdiomAction::REST_CAPSMALL },
    { { "QUADRANT"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },
    { { "BOX"sv, "DRAWINGS"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },
    { { "LARGE"sv, "TYPE"sv, "PIECE"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },
    { { "MAHJONG"sv, "TILE"sv }, IsPrefix::YES, IdiomAction::REST_CAPSMALL },
    { { "DOMINO"sv, "TILE"sv }, IsPrefix::YES, IdiomAction::REST_CAPSMALL },
    { { "PLAYING"sv, "CARD"sv }, IsPrefix::YES, IdiomAction::REST_CAPSMALL },
    { { "TIBETAN"sv, "DIGIT"sv, "HALF"sv }, IsPrefix::YES, IdiomAction::NEXT_SMALL },
    { { "SUPERSCRIPT"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },
    { { "SUBSCRIPT"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },
    { { "INDIC"sv, "SIYAQ"sv, "NUMBER"sv }, IsPrefix::YES, IdiomAction::REST_CAPSMALL },
    { { "MODIFIER"sv, "LETTER"sv, "CHINESE"sv, "TONE"sv, }, IsPrefix::YES, IdiomAction::NEXT_CAP },
    { { "RUNIC"sv, "LETTER"sv, "FRANKS"sv, "CASKET"sv }, IsPrefix::YES, IdiomAction::REST_CAP },      // prevent next!
    { { "RUNIC"sv, "LETTER"sv }, IsPrefix::YES, IdiomAction::REST_CAP },
    { { "CIRCLED"sv, "KATAKANA"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },  // IDK what to do, normal rules fail
    { { "CJK"sv, "STROKE"sv }, IsPrefix::YES, IdiomAction::REST_ALLCAP },
    { { "MIAO"sv, "LETTER"sv, "YI"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP }, // Yi is ambiguous
    { { "SQUARE"sv, "ERA"sv, "NAME"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },
    { { "SQUARE"sv }, 0x3300, 0x3357, IsPrefix::YES, IdiomAction::NEXT_CAP },     // And what to do with those Katakana chars?
    { { "CYPRO-MINOAN"sv, "SIGN"sv }, IsPrefix::YES, IdiomAction::REST_ALLCAP },
    { { "LINEAR"sv, "A"sv, "SIGN"sv }, IsPrefix::YES, IdiomAction::REST_CAP },
    { { "LINEAR"sv, "B"sv, "SYLLABLE"sv }, IsPrefix::YES, IdiomAction::REST_CAP },
    { { "PAHAWH"sv, "HMONG"sv, "CONSONANT"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },   // Consonant is tricky
    { { "SHORTHAND"sv, "FORMAT"sv }, IsPrefix::YES, IdiomAction::REST_CAPSMALL },
    { { "ZNAMENNY"sv, "PRIZNAK"sv, "MODIFIER"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },
    { { "HEBREW"sv, "ACCENT"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },     // Accent is tricky, though seemingly good
    { { "HEBREW"sv, "POINT"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },      // Point is really tricky
    { { "TAG"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },                    // Tags
    { { "CLOCK"sv, "FACE"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },        // clock face XXX
    { { "CIRCLED"sv, "IDEOGRAPH"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP }, // circled ideograph XXX
    { { "PARENTHESIZED"sv, "IDEOGRAPH"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP }, // parenthesized ideograph XXX
    { { "CIRCLED"sv, "HANGUL"sv }, IsPrefix::YES, IdiomAction::REST_CAP },        // circled Hangul Xxx
    { { "PARENTHESIZED"sv, "HANGUL"sv }, IsPrefix::YES, IdiomAction::REST_CAP },  // parenthesized Hangul Xxx
    { { "BALINESE"sv, "MUSICAL"sv, "SYMBOL"sv }, IsPrefix::YES, IdiomAction::REST_CAPSMALL },
    { { "IDEOGRAPHIC"sv, "TALLY"sv, "MARK"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },   // ideographic tally mark One
    { { "TALLY"sv, "MARK"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },
    { { "EMOJI"sv, "COMPONENT"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },
    { { "IDEOGRAPHIC"sv, "DESCRIPTION"sv, "CHARACTER"sv }, IsPrefix::YES, IdiomAction::NEXT_CAP },
    { { "DUPLOYAN"sv, "LETTER"sv }, IsPrefix::YES, IdiomAction::REST_DETECT_LETTERS },
    // Rest idioms
    { { "VOWEL"sv, "LENGTH"sv, "MARK"sv }, IsPrefix::NO, IdiomAction::SECOND_THIRD_SMALL },
    { { "VEDIC"sv, "TONE"sv}, IsPrefix::NO, IdiomAction::NEXT_CAP },
    { { "AI"sv, "LENGTH"sv, "MARK"sv}, IsPrefix::NO, IdiomAction::FIRST_CAP },
    { { "SYLLABLE"sv, "LENGTHENER"sv }, IsPrefix::NO, IdiomAction::SECOND_THIRD_SMALL },
    { { "SYLLABLE"sv, "ITERATION"sv, "MARK"sv}, IsPrefix::NO, IdiomAction::SECOND_THIRD_SMALL },
};


const std::unordered_map<char32_t, std::string_view> abbrevs {
    { 9, "TAB" },       // TAB or HT
    { 10, "LF" },       // NL or LF
    { ' ', {} },        // SPACE has abbrev SP
    { 0x0019, "EOM" },  // EOM is better than EM
    { 0x00A0, "!" },    // NBSP bas abbreviations, turn them to aliases
    { 0x115F, "HC F" }, // Hangul starting filler
    { 0x1160, "HJ F" }, // Hangul medial filler
    { 0x17B4, "KIV AQ" }, // Khmer intependent vowel Aq
    { 0x17B5, "KIV AA" }, // Khmer intependent vowel Aa
    { 0x2028, "LSEP" }, // Line separator, somehow has no abbrev
    { 0x2029, "PSEP" }, // Paragraph separator, same
    { 0x202F, "!" },    // Narrow NBSP has abbrevs, turn them to aliases
    { 0x205F, "!" },    // Medium math space same
    { 0x2061, "f()" },  // Function application
    { 0x2062, "✕" },   // Invisible times
    { 0x2063, "," },    // Invisible separator
    { 0x2064, "+" },    // Invisible plus
    { 0x206A, "ISS" },  // Inhibit symmetric swapping
    { 0x206B, "ASS" },  // Activate symmetric swapping
    { 0x206C, "IAFS" }, // Inhibit Arabic form shaping
    { 0x206D, "AAFS" }, // Activate Arabic form shaping
    { 0x206E, "NADS" }, // National digit shapes
    { 0x206F, "NODS" }, // Nominal digit shapes
    { 0x3164, "HF" },   // Hangul filler
    { 0xFEFF, "BOM" },  // BOM or ZWNBSP
    { 0xFFA0, "HWHF" }, // Halfwidth Hangul filler
    { 0xFFF9, "IA A" }, // Interlinear annotation anchor
    { 0xFFFA, "IA S" }, // Interlinear annotation separator
    { 0xFFFB, "IA T" }, // Interlinear annotation terminator
    { 0xFFFC, "OBJ" },  // Object replacement character
    { 0x1107F, "BNJ" }, // Brahmi number joiner
    { 0x13430, ":" },   // Egyptian hiero vertical joiner
    { 0x13436, "+" },   // Egyptian hiero overlay middle
    { 0x13437, "(" },   // Egyptian hiero begin segment
    { 0x13438, ")" },   // Egyptian hiero end segment
    { 0x13440, "↔" },   // Egyptian hiero mirror
    { 0x16FE4, "KSS F" }, // Khitan small filler
    { 0x1BC9D, "DTLS" }, // Duployan thick letter selector
    { 0x1BCA2, "↓" },   // Shorthand format Down step
    { 0x1BCA3, "↑" },   // Shorthand format Up step
    { 0x1D159, "NULL NOTE HEAD" }, // Music
    { 0x1D173, "BEGIN BEAM" },
    { 0x1D174, "END BEAM" },
    { 0x1D175, "BEGIN TIE" },
    { 0x1D176, "END TIE" },
    { 0x1D177, "BEGIN SLUR" },
    { 0x1D178, "END SLUR" },
    { 0x1D179, "BEGIN PHR." },
    { 0x1D17A, "END PHR." },
    { 0x1DA9B, "SW F2" },   // Sutton SignWriting fill
    { 0x1DA9C, "SW F3" },
    { 0x1DA9D, "SW F4" },
    { 0x1DA9E, "SW F5" },
    { 0x1DA9F, "SW F6" },
    { 0x1DAA1, "SW R2" },   // Sutton SignWriting rotation
    { 0x1DAA2, "SW R3" },
    { 0x1DAA3, "SW R4" },
    { 0x1DAA4, "SW R5" },
    { 0x1DAA5, "SW R6" },
    { 0x1DAA6, "SW R7" },
    { 0x1DAA7, "SW R8" },
    { 0x1DAA8, "SW R9" },
    { 0x1DAA9, "SW R10" },
    { 0x1DAAA, "SW R11" },
    { 0x1DAAB, "SW R12" },
    { 0x1DAAC, "SW R13" },
    { 0x1DAAD, "SW R14" },
    { 0x1DAAE, "SW R15" },
    { 0x1DAAF, "SW R16" },
    // Then Variation Selectors
};

const std::unordered_map<char32_t, uc::Cfgs> drawMethods {
    // Knda
    { 0x0CF1,  uc::m::SAMPLED_CONTROL }, // Knda vedic sign jihva
    { 0x0CF2,  uc::m::SAMPLED_CONTROL }, // Knda vedic sign upadh
    // Mlym
    { 0x0D4E,  uc::m::SAMPLED_CONTROL }, // Mlym dot reph
    // Tibt
    { 0x0F0C,  uc::m::SAMPLED_CONTROL }, // Tibt non-breaking tsheg
    // Mymr
    { 0x1039,  uc::m::VIRTUAL_VIRAMA  }, // Mymr virtual virama
    // Sund
    { 0x1BAB,  uc::m::VIRTUAL_VIRAMA  }, // Sund virtual virama
    // Khmr
    { 0x17D2,  uc::m::VIRTUAL_VIRAMA  }, // Khmr coeng (virtual virama)
    // Lana
    { 0x1A60,  uc::m::VIRTUAL_VIRAMA  }, // Lana sakot (virtual virama)
    // Vedic
    { 0x1CF5,  uc::m::SAMPLED_CONTROL }, // Upadh
    { 0x1CF6,  uc::m::SAMPLED_CONTROL }, // Jihva
    // Brai
    { 0x2800,  uc::m::SPACE           }, // Braille blank
    // CJK
    { 0x303E,  uc::m::CUSTOM_CONTROL  }, // ideographic variation indicator
    // Mtei
    { 0xAAF6,  uc::m::VIRTUAL_VIRAMA  }, // Mtei virtual virama
    // Khar
    { 0x10A3F, uc::m::VIRTUAL_VIRAMA }, // Khar virtual virama (does not fit)
    // Brah
    { 0x11003, uc::m::SAMPLED_CONTROL }, // Brah jihva
    { 0x11004, uc::m::SAMPLED_CONTROL }, // Brah upadh
    // Cakm
    { 0x11133, uc::m::VIRTUAL_VIRAMA  }, // Cakm virama
    // Shrd
    { 0x111C2, uc::m::SAMPLED_CONTROL }, // Shrd jihva
    { 0x111C3, uc::m::SAMPLED_CONTROL }, // Shrd upadh
    // Tutg
    { 0x113D0, uc::m::VIRTUAL_VIRAMA  }, // Tutg conjoiner
    { 0x113D1, uc::m::SAMPLED_CONTROL }, // Tutg conjoiner
    // Diak
    { 0x1193E, uc::m::VIRTUAL_VIRAMA  }, // Diak virama
    { 0x1193F, uc::m::SAMPLED_CONTROL }, // Diak nasal sign
    { 0x11941, uc::m::SAMPLED_CONTROL }, // Diak repha
    // Zanb
    { 0x11A3A, uc::m::SAMPLED_CONTROL | uc::Cfg::M_NO_SHAPING }, // Zanb repha (cluster-initial Ra)
    { 0x11A47, uc::m::VIRTUAL_VIRAMA  }, // Zanb subjoiner (virtual virama)
    // Soyo
    { 0x11A84, uc::m::SAMPLED_CONTROL | uc::Cfg::M_NO_SHAPING }, // Soyo vedic sign jihva
    { 0x11A85, uc::m::SAMPLED_CONTROL | uc::Cfg::M_NO_SHAPING }, // Soyo vedic sign upadh
    { 0x11A86, uc::m::SAMPLED_CONTROL }, // Soyo Ra+
    { 0x11A87, uc::m::SAMPLED_CONTROL }, // Soyo La+
    { 0x11A88, uc::m::SAMPLED_CONTROL }, // Soyo Sha+
    { 0x11A89, uc::m::SAMPLED_CONTROL }, // Soyo Sa+
    { 0x11A99, uc::m::VIRTUAL_VIRAMA  }, // Soyo subjoiner
    // Gonm
    { 0x11D45, uc::m::VIRTUAL_VIRAMA  }, // Gonm virtual virama
    { 0x11D46, uc::m::SAMPLED_CONTROL }, // Gonm repha
    // Gong
    { 0x11D97, uc::m::VIRTUAL_VIRAMA  },  // Gong virtual virama
    // Kawi
    { 0x11F02, uc::m::SAMPLED_CONTROL }, // Kawi repha
    { 0x11F42, uc::m::VIRTUAL_VIRAMA  }, // Kawi subjoiner
    // Egyp
    { 0x13431, uc::m::CUSTOM_CONTROL },  // Egyptian hiero horiz joiner
    { 0x13432, uc::m::CUSTOM_CONTROL },  // Egyptian hiero insert at top start
    { 0x13433, uc::m::CUSTOM_CONTROL },  // Egyptian hiero insert at bottom start
    { 0x13434, uc::m::CUSTOM_CONTROL },  // Egyptian hiero insert at top end
    { 0x13435, uc::m::CUSTOM_CONTROL },  // Egyptian hiero insert at bottom end
    { 0x13439, uc::m::CUSTOM_CONTROL },  // Egyptian hieroglyph Insert at middle
    { 0x1343A, uc::m::CUSTOM_CONTROL },  // Egyptian hieroglyph Insert at top
    { 0x1343B, uc::m::CUSTOM_CONTROL },  // Egyptian hieroglyph Insert at bottom
    { 0x1343C, uc::m::CUSTOM_CONTROL },  // Egyptian hieroglyph Begin enclosure
    { 0x1343D, uc::m::CUSTOM_CONTROL },  // Egyptian hieroglyph End enclosure
    { 0x1343E, uc::m::CUSTOM_CONTROL },  // Egyptian hieroglyph Begin walled enclosure
    { 0x1343F, uc::m::CUSTOM_CONTROL },  // Egyptian hieroglyph End walled enclosure
    { 0x13441, uc::m::SPACE },           // Egyptian hiero Full blank
    { 0x13442, uc::m::SPACE },           // Egyptian hiero Half blank
    // Plrd (Miao)
    { 0x16F8F, uc::m::CUSTOM_CONTROL },  // Miao tone right
    { 0x16F90, uc::m::CUSTOM_CONTROL },  // Miao tone top right
    { 0x16F91, uc::m::CUSTOM_CONTROL },  // Miao tone above
    { 0x16F92, uc::m::CUSTOM_CONTROL },  // Miao tone below
    // Dupl
    { 0x1BCA0, uc::m::CUSTOM_CONTROL },  // shorthand format Letter overlap
    { 0x1BCA1, uc::m::CUSTOM_CONTROL },  // shorthand format Continuing overlap
};

// Just capitalize them!
const std::unordered_set<std::string_view> cuneiformSymbols {
    "BAR", "LAM", "MU", "NU", "PI", "SA"
};

const std::unordered_set<std::string_view> cuneiformSuffixes {
    "TENU", "GUNU", "SHESHIG", "NUTILLU"
};

const std::unordered_set<std::string_view> cuneiformKeywords {
    "CUNEIFORM",
    "SIGN",
    "TIMES",
    "OVER",
    "CROSSING",
    "PLUS",
    "NUMERIC",
    "INVERTED",
    "ROTATED",
    "NINETY",
    "DEGREES",
    "PUNCTUATION",
    "DIAGONAL",
    "QUADCOLON",
    "OPPOSING",
    "SQUARED",
    "VARIANT",
    "ASTERISK",
    "WORD",
    "DIVIDER",
    "THIRD",
    "THIRDS",
    "FOURTH",
    "FOURTHS",
    "FIFTH",
    "FIFTHS",
    "SIXTH",
    "SIXTHS",
};

struct RangeByEnd {
    char32_t a = 0, b = 0;
    constexpr RangeByEnd() = default;
    constexpr RangeByEnd(char32_t aa) : a(aa), b(aa) {}
    constexpr RangeByEnd(char32_t aa, char32_t bb) : a(aa), b(bb) {}
};

constexpr bool operator < (const RangeByEnd& x, const RangeByEnd& y) { return (x.b < y.b); }

///  BUG_AVOID / BUG_FIXUP are used
const std::set<RangeByEnd> alternateRanges {
    { 0x0126 },             // H bar: Strange bar in Cambria
    { 0x1D66, 0x1D6A },     // Clipped below in Cambria
    { 0x1E00, 0x1E01 },     // Clipped below in Cambria
    { 0x1E5C, 0x1E5D },     // Misplaced umlaut in Cambria
    { 0x1E68, 0x1E69 },     // Same
    { 0x02CE, 0x02CF },     // Misrender in Cambria
    { 0x0333 },             // Basic umlauts: misrenders in Cambria
    { 0x2010, 0x203B },     // Several chars from Punctuation…
    { 0x203D, 0x203E },     // …Skip emoji and go on
    { 0x23B4, 0x23B6 },     // Technical — horz brackets, misrender in Cambria
    { 0x23DC, 0x23E0 },     // Same
    { 0x266D, 0x266F },     // Misc — use Funky font on musical symbols
    { 0x2C7C },             // Clipped below in Cambria
    { 0x3021, 0x3029 },     // U16 glyph changes, BabelStone’s here!
                            // Actually 26…29, the rest are for uniformness
    // CJK compatibles: SimSun → BabelStone
    { 0xFE31, 0xFE34 },     // Some dashes
    { 0xFE49, 0xFE4F },     // More dashes
    // BS = BabelStone, SimSun = built-in Windows font, G = mainland China, T = Taiwan?
    // As BS is now primary font, destroying everything
    { 0x34F0  },    // CJK A: BS drew a hypotethical Chinese version
    { 0x2561E },    // CJK B: BS drew a hypotethical Chinese version
    { 0x256A1 },    // CJK B: SimSun-B drew more canonical glyph    

    // Mathematical: Cambria → Funky
    { 0x220F, 0x2211 },     // Big sums and products
    { 0x222B, 0x2233 },     // Integrals bunch 1
    { 0x2320, 0x2321 },     // Multi-storey integrals part 1
    { 0x239B, 0x23AE },     // Multi-storey ()[]{}, integral part 2
    { 0x23B0, 0x23B1 },     // Two-storey {}
    { 0x2A0A, 0x2A1C },     // Integrals bunch 2

    // Mathematical: Cambria → Noto
    { 0x2201 },         // set C
    { 0x2205 },         // empty set
    { 0x22DA, 0x22DB },
    { 0x27EC, 0x27ED },
    { 0x298B, 0x298C },
    { 0x2997, 0x2998 },
    { 0x2A79, 0x2AAD },  // greater-less
};

const std::set<RangeByEnd> noAaRanges {
    { 0x133FA, 0x1340C },   // Egyptian — sticks 1…9
};

constexpr auto TWO_STYLES = uc::Cfg::G_STYLE_0 | uc::Cfg::G_STYLE_1;

const std::map<RangeByEnd, uc::Cfgs> styleRanges {
    // Glagolitic: Bulgaria or Croatia
    { { 0x2C00,  0x2C5F  }, TWO_STYLES },   // Glag main
    { { 0x1E000, 0x1E02F }, uc::Cfg::G_STYLE_0 }, // Glag ex
    // Sogdian etc: horizontal or vertical
    { { 0x1800,  0x180A  }, TWO_STYLES },   // Mong
    { { 0x180B,  0x180F  }, uc::Cfg::G_STYLE_0 }, // A small hole in Mong
    { { 0x1810,  0x18AF  }, TWO_STYLES },   // Mong
    { { 0x11660, 0x1167F }, TWO_STYLES },   // Mong ex
    { { 0xA840,  0xA87F  }, TWO_STYLES },   // Phag
    { { 0x10F30, 0x10F6F }, TWO_STYLES },   // Sogd
    { { 0x10F70, 0x10FAF }, TWO_STYLES },   // Ougr
};

/// One method of homonym disambig: these chars are left as-are
/// while the rest are decapped by dictionary under Exf::LEAVE_BY_CONDITION
const std::unordered_set<char32_t> charsConditionalLeave {
    0x11451,    // Newa digit One = chi
    0x11454,    // Newa digit Four = pi
    0x11fc8,    // Tamil fraction One twentieth = maa
};


const std::unordered_map<char32_t, DepInfo> deprecatedInfo {
    // Latin A
    { 0x0149, { .whatsInstead = u8"\u02BC" "\u006E" } },
    // Arabic
    { 0x0673, { .whatsInstead = u8"\u0627" "\u065F" } },
    // Tibetan
    { 0x0F77, { .whatsInstead = u8"\u0FB2" "\u0F71" "\u0F80" } },
    { 0x0F79, { .whatsInstead = u8"\u0FB3" "\u0F71" "\u0F80" } },
    // Khmer
    { 0x17A3, { .whatsInstead = u8"\u17A2" } },
    { 0x17A4, { .whatsInstead = u8"\u17A2" "\u17B6" } },
    // General punctuation
    { 0x206A, { .whatsInstead = u8"2" } },
    { 0x206B, { .whatsInstead = u8"1" } },
    { 0x206C, { .whatsInstead = u8"1" } },
    { 0x206D, { .whatsInstead = u8"3" } },
    { 0x206E, { .whatsInstead = u8"4" } },
    { 0x206F, { .whatsInstead = u8"1" } },
    // Misc tech
    { 0x2329, { .whatsInstead = u8"\u27E8", .whatsInstead2 = u8"\u3008" } },
    { 0x232A, { .whatsInstead = u8"\u27E9", .whatsInstead2 = u8"\u3009" } },
    // Tags
    { 0xE0001, { .whatsInstead = u8"5" } },
    // -- total 15
};


/// @todo [langs] Stopped at Canadian syllabics
const std::set<std::string_view> langNames {
    "also Cornish",     // корнский (Великобритания)
    "also used in Americanist linguistics",
    "used in transcription of Mandarin Chinese",
    "used by linguists working on Chinese and other Sino-Tibetan languages",
    "used in some Polynesian orthographies for glottal stop",
    "also used for Enets",      // энецкий (Россия)
    "also used for Itelmen"     // ительменский (Россия)
    "also used for Nenets"
    "also used for Ossetian"    // осетинский (Россия)
    "Middle Eastern Arabic for foreign words"
    "North African Arabic for foreign words"
        /// @todo [langs] Headers like “@→→Komi letters”, → = TAB

    "and many other languages",
    "...",
    "not in current use",

    "Abkhasian",
    "Adamawa Fulfulde", // фулá (Камерун)
    "Adighe",           // адыгейский (=западночеркесский)
    "African",
    "African languages",
    "Afrikaans",
    "Algonquin",
    "Algonquian",       ///< =algonquin
    "Altay",
    "Amazigh",          ///< =berber
    "Americanist phonetic usage",   // индейская фонетика
    "Arwi",             // Арви (арабская запись тамильского)
    "Asturian",
    "Athapascan",       // атабаскские (Канада)
    "Azerbaijani",
    "Baluchi",          // белуджский (Пакистан)
    "Bashkir",
    "Basketo",           // баскето (Эфиопия)
    "Berber",
    "Belarusian",       ///< =Byelorussian
    "Bihari languages", // бихарские (Индия)
    "Bosnian",
    "Bulgarian",
    "Burushaski",       // бурушаски (Кашмир)
    "Byelorussian",
    "Carrier",          ///< кэрриер (Канада), better stop?
    "Crimean Tatar",
    "Chechen",
    "Chukchi",
    "Chuvash",
    "Croatian",
    "Czech",
    "Danish",
    "Dargwa",           // даргинский (Дагестан)
    "Dutch",
    "early Jawi",
    "early Persian",
    "East Cree",
    "Esperanto",
    "Estonian",
    "Ewe",          // эве (Того)
    "Faroese",
    "Finnish",
    "Finnish Romany",   // финский кало (цыганский)
    "French",
    "former-Soviet minority language scripts",
    "Galician",
    "German",
    "German dialectology",
    "Germanic",
    "Greenlandic (old orthography)",
    "Gojri",                    // гуджарский (Пакистан)
    "Gothic transliteration",   // готская транслитерация
    "Hausa",
    "Hittite transcription",    // транскрипция хеттского
    "Hungarian",
    "Huron",
    "Icelandic",
    "Igbo",         // игбо (Нигерия)
    "Ingush",
    "Indic transliteration",    // транслитерация санскрита
    "Inuktitut",    ///< better stop?
    "Irish Gaelic (old orthography)",
    "Italian",
    "IPA",
    "IPA and many languages",
    "Iroquoian",
    "Jawi",         // джави (Бруней)
    "Kalami",       // гаври (=калами, Пакистан)
    "Kashmiri",
    "Kazakh",
    "Kharoshthi transliteration",   // транслитерация кхароштхи (Индия, Ср. Азия, −I…I тыс.)
    "Khoisan tradition",    // койсанские (бушменские)
    "Khwarazmian",  // хорезмийский
    "Kikuyu",       // кикуйю (Кения)
    "Kildin Sami",  // кильдинский саамский
    "Kurdish",
    "Kuvi",         // куви (Индия)
    "Labrador Inuttut", // лабрадорский инуитский
    "Lahnda",   // лахнда (западнопенджабские)
    "Lak",      // лакский (Дагестан)
    "Lakota",   // лакота (индейцы США)
        /// @todo [langs] Remove text in parentheses, after:
    "Latin",
    "Latin abbreviation",
    "Latvian",
    "Lithuanian",
    "Lithuanian phonetics and medievalist transcriptions",
    "Livonian",     // ливский (Латвия)
    "Maba",         // маба (Чад)
    "Macedonian",
    "Macedonian and Serbian transliteration",
    "Maghrib Arabic",
    "Madurese",     // мадурский (Индонезия)
    "Malagasy",
    "Malay",        // малайский
    "Maltese",
    "Mari",
    "Marshallese",
    "medieval Hungarian orthography",
    "Mende",        // менде (Сьерра-Леоне)
    "Middle English",   // среднеанглийский
    "Middle High German",
    "Moldavian",
    "Moose Cree",   // мусский кри (Канада)
    "Moroccan Arabic",
    "Norwegian",
    "older orthographies for Abkhasian",
    "Old English",
    "Old Hausa",
    "Old Icelandic",
    "Old Norse",        // древнескандинавский
    "Old Saxon",
    "Old Urdu",
    "Ormuri",           // ормури (Пакистан)
    "Pan-Nigerian alphabet",    // паннигерийский алфавит
    "Pan-Turkic Latin alphabets",   // тюркские
    "Pan-Turkic Latin orthography",
    "Pegon",            // пегон (яванское арабское письмо)
    "Pinyin",                   // пиньинь (транскрипция китайского)
    "Pinyin first tone",
    "Pinyin second tone",
    "Pinyin third tone",
    "Pinyin fourth tone",
        "Mandarin Chinese first tone",
        "Mandarin Chinese second tone",
        "Mandarin Chinese third tone",
        "Mandarin Chinese fourth tone",
        "Mandarin Chinese fifth tone (light or neutral)",
    "phonetics",
    "phonetic use in Sinology",
    "Pashto",       // пуштунский
    "Persian",
    "Philippine languages",
    "Polish",
    "Portuguese",
    "Quranic Arabic",
    "Romanian",
    "Russian",
    "Sami",         // also Saami
    "Saraiki",      // сараики (Пакистан)
    "Sayisi",       // сайиси (Канада)
    "Scots",        // англо-шотландский
    "Semitic transliteration",  // транслитерация семитских языков
    "Serbian",
    "Serbian and Croatian phonetics",
    "Shina",        // шина (Кашмир)
    "Sindhi",       // синдхи (Пакистан)
    "Skolt Sami",   // колтта-саамский (Финляндия)
    "Slovak",
    "Slovenian",
    "Spanish",
    "Swedish",
    "Swedish grave accent",
    "Tajik",
    "Tatar",
    "Torwali",      // торвали (Пакистан)
    "Tunisian Arabic",
    "Turkish",
    "Twi",          // чви (Гана)
    "Uighur",
    "Ukrainian",
    "Uralicist usage",  // уральские языки
    "Urdu",
    "Uzbek",
    "Vietnamese",
    "Walloon",
    "Welsh",
    "West Cree",
    "Yakut",
    "Yoruba",
    "Yupik",        // юпикские (Чукотка)
    "Zhuang (old orthography)", // Чжуанский (старая орфография, Китай)
    "Zulu",
};

namespace {

    enum class WordState { SMALL, TITLE, CUSTOM_TITLE, CUSTOM, ALL_CAP };

    struct Word {
        std::string_view original, customCap;
        bool isAllCap = false;
        bool isCapital = false;
        bool nextNoun = false;
        bool nextAdjective = false;
        bool nextNumeral = false;
        bool isAdjective = false;
        Flags<Dicf> dicFlags;
    };

    constexpr auto TRIG_ANY_SCRIPT = Dicf::TRIG_SCRIPT | Dicf::TRIG_SCRIPT_IF_FIRST
            | Dicf::TRIG_TWO_WORD_SCRIPT | Dicf::TRIG_THREE_WORD_SCRIPT
            | Dicf::TRIG_SCRIPT_ADJECTIVE;

    bool hasDigit(std::string_view x) {
        for (auto v : x) {
            if (std::isdigit(v))
                return true;
        }
        return false;
    }

    void appendLower(std::string& r, std::string_view x) {
        for (auto v : x)
            r += char(std::tolower(v));
    }

    void appendUpper(std::string& r, std::string_view x) {
        for (auto v : x)
            r += char(std::toupper(v));
    }

    void appendTitle(std::string& r, std::string_view x) {
        if (x.empty())
            return;
        r += char(std::toupper(x[0]));
        for (size_t i = 1; i < x.length(); ++i) {
            r += char(std::tolower(x[i]));
        }
    }

    void appendAsIsTitle(std::string& r, std::string_view x) {
        if (x.empty())
            return;
        r += char(std::toupper(x[0]));
        r.append(x.data() + 1, x.length() - 1);
    }

    /// @param [in] wantWordAfter  [+] want at least one word after idiom
    bool doesIdiomFit(const SafeVector<Word>& words, size_t pos,
                    Buf1d<const std::string_view> idiom,
                    bool wantWordAfter)
    {
        size_t wantedSize = idiom.size() + pos + static_cast<int>(wantWordAfter);
        return words.size() >= wantedSize
                && std::equal(idiom.begin(), idiom.end(), words.begin() + pos,
                            [](std::string_view x, const Word& y) {
                                return x == y.original;
                            });
    }

    void doIdiomAction(SafeVector<Word>& words,
                       size_t iStart, size_t iEndPlus,
                       IdiomAction action)
    {
        // Prefix actions remove appropriate dictionary flags,
        // as they are more important than dic. properties
        switch (action) {
        case IdiomAction::REST_SMALL:
            for (size_t i = iEndPlus; i < words.size(); ++i)
                words[i].isCapital = false;
            break;
        case IdiomAction::SECOND_THIRD_SMALL:
            for (size_t i = iStart + 1; i < iEndPlus; ++i) {
                auto& w = words[i];
                w.isCapital = false;
            } break;
        case IdiomAction::REST_CAP:
            for (size_t i = iEndPlus; i < words.size(); ++i) {
                auto& w = words[i];
                w.isCapital = true;
                w.dicFlags.remove(Dicf::CAP_SMALL);
            }
            break;
        case IdiomAction::REST_DETECT_LETTERS:
            for (size_t i = iEndPlus; i < words.size(); ++i) {
                auto& w = words[i];
                if (w.original.length() == 1) {
                    w.isCapital = true;
                }
            }
            break;
        case IdiomAction::REST_ALLCAP:
            for (size_t i = iEndPlus; i < words.size(); ++i) {
                auto& w = words[i];
                w.isAllCap = true;
                w.dicFlags.remove(Dicf::CAP_SMALL);
            }
            break;
        case IdiomAction::REST_CAPSMALL:
            for (size_t i = iEndPlus + 1; i < words.size(); ++i)
                words[i].isCapital = false;
            [[fallthrough]];
        case IdiomAction::NEXT_CAP: {
                auto& wp = words[iEndPlus];
                wp.isCapital = true;
                wp.dicFlags.remove(Dicf::CAP_SMALL);
            } break;
        case IdiomAction::NEXT_SMALL:
            words[iEndPlus].isCapital = false;
            break;
        case IdiomAction::FIRST_CAP:
            words[iStart].isCapital = true;
            break;
        }
    }

    class MultiPrinter
    {
    public:
        void print(const char* x);
        void printIf(bool cond, const char* x);
    private:
        bool wasPrinted = false;
    };

    void MultiPrinter::print(const char* x)
    {
        if (wasPrinted)
            std::cout << " ";
        std::cout << x;
        wasPrinted = true;
    }

    void MultiPrinter::printIf(bool cond, const char* x)
    {
        if (cond)
            print(x);
    }

    void dump(Flags<Dicf> x)
    {
        MultiPrinter mp;
        std::cout << "[";
        mp.printIf(x.have(Dicf::CAP_ALL), "ALL");
        mp.printIf(x.have(Dicf::CAP_SMALL), "small");
        mp.printIf(x.have(Dicf::CAP_TITLE), "Title");
        mp.printIf(x.have(Dicf::IF_SCRIPT), "ifscr");
        mp.printIf(x.have(Dicf::IF_NEXT_NOT_NOUN), "ifnnn");
        mp.printIf(x.have(Dicf::PART_ADJECTIVE), "adj");
        mp.printIf(x.have(Dicf::PART_MAYBE_LETTER), "let?");
        mp.printIf(x.have(Dicf::PART_NOUN), "noun");
        mp.printIf(x.have(Dicf::PART_NUMERAL), "num");
        mp.printIf(x.have(Dicf::PART_MIXED), "mixed");
        mp.printIf(x.have(Dicf::TRIG_CAP_NEXT), "capnext");
        mp.printIf(x.have(Dicf::TRIG_DIGIT_PROPAGATES_RIGHT), "dig->");
        mp.printIf(x.have(Dicf::TRIG_SCRIPT), "Script");
        mp.printIf(x.have(Dicf::TRIG_SCRIPT_IF_FIRST), "Script1");
        mp.printIf(x.have(Dicf::TRIG_THREE_WORD_SCRIPT), "3wScript");
        mp.printIf(x.have(Dicf::TRIG_TWO_WORD_SCRIPT), "2wScript");
        std::cout << "]";
    }

    [[maybe_unused]] void dump(const SafeVector<Word>& words)
    {
        for (auto& v : words) {
            std::cout << v.original << " ";
            dump(v.dicFlags);
            std::cout << '\n';
        }
    }

    constexpr auto STR_IDEOGRAPH = "IDEOGRAPH-"sv;

    bool isHex(std::string_view s) {
        for (char c : s) {
            if (!isxdigit(c))
                return false;
        }
        return true;
    }

    bool isFullDecap(const SafeVector<std::string_view>& words)
    {
        for (auto& word : words) {
            auto itWord = dictionary.find(word);
            if (itWord != dictionary.end()) {
                if (itWord->second.flags.have(Dicf::TRIG_FORCE_FULL_DECAP))
                    return true;
            }
        }
        return false;
    }

}   // anon namespace


size_t nExceptions() { return exceptions.size(); }


std::string decapitalize(
        std::string_view x, char32_t cp,
        Flags<Dcfg> flags, DecapDebug debug)
{
    // Detect exceptions
    std::string upper = str::toUpper(x);
    auto itEx = exceptions.find(upper);
    if (itEx != exceptions.end()) {
        if (cp || !itEx->second.flags.have(decap::Exf::CPONLY) ) {
            // Mixcase flag: in upcase mode leave, in locase mode to lower
            if (itEx->second.flags.have(decap::Exf::MIXCASE) && flags.have(Dcfg::LOCASE)) {
                return str::toLower(itEx->second.r);
            }
            if (itEx->second.flags.have(decap::Exf::LEAVE_BY_CONDITION)
                    && charsConditionalLeave.contains(cp)) {
                return std::string{x};
            }
            return std::string(itEx->second.r);
        }
    }

    // Get words
    auto rawWords = str::splitSv(upper, ' ');
    if (rawWords.empty())
        return {};

    // Check: do we need to trigger normal decapitalization?
    if (flags.have(Dcfg::SHORTCUT)) {
        if (!isFullDecap(rawWords))
            return std::string{x};
    }

    SafeVector<Word> words;
    words.resize(rawWords.size());
    for (size_t iWord = 0; iWord < words.size(); ++iWord) {
        words[iWord].original = rawWords[iWord];
    }

    // Detect scripts, cap, small
    bool hasScript = false;
    bool digitPropagatesRight = false;
    Word* nextWord = nullptr;
    Word* nextNextWord = nullptr;
    std::vector<std::string> tempWords;

    for (auto iWord = words.size(); iWord != 0; ) { --iWord;
        auto& word = words[iWord];

        if (str::isIndex(word.original))
            word.isCapital = true;

        auto itWord = dictionary.find(word.original);
        if (itWord != dictionary.end()) {
            auto& wordInfo = itWord->second;
            word.dicFlags = wordInfo.flags;
            if (word.dicFlags.haveAny(TRIG_ANY_SCRIPT)) {
                if (word.dicFlags.have(Dicf::TRIG_SCRIPT)) {
                    // Script
                    word.isCapital = true;
                    hasScript = true;
                } else if (word.dicFlags.have(Dicf::TRIG_SCRIPT_IF_FIRST)) {
                    // Script only if 1st word
                    if (iWord == 0) {
                        word.isCapital = true;
                        hasScript = true;
                    }
                } else if (word.dicFlags.have(Dicf::TRIG_TWO_WORD_SCRIPT)) {
                    // Two-word script
                    if (nextWord &&
                            (nextWord->original == wordInfo.trigger1
                          || nextWord->original == wordInfo.trigger2
                          || nextWord->original == wordInfo.trigger3
                          || nextWord->original == wordInfo.trigger4
                          || nextWord->original == wordInfo.trigger5)) {
                        word.isCapital = true;
                        nextWord->isCapital = true;
                        hasScript = true;
                    }
                } else if (word.dicFlags.have(Dicf::TRIG_THREE_WORD_SCRIPT)) {
                    // Three-word script
                    if (nextNextWord && nextWord->original == wordInfo.trigger1
                            && nextNextWord->original == wordInfo.trigger2) {
                        word.isCapital = true;
                        nextWord->isCapital = true;
                        nextNextWord->isCapital = true;
                        hasScript = true;
                    }
                }
                // Works together with other flags
                if (word.dicFlags.have(Dicf::TRIG_SCRIPT_ADJECTIVE)) {
                    // Propagate adjective that are script parts
                    // OLD SOUTH Arabian
                    if (nextWord) {
                        // Cascade capitalization: OLD North Arabian
                        if ((nextWord->isCapital && nextWord->dicFlags.have(Dicf::TRIG_SCRIPT_ADJECTIVE))
                                    // First-party capitalization: NORTH Arabian
                                 || (nextWord->dicFlags.have(Dicf::TRIG_SCRIPT))) {
                            word.isCapital = true;
                            hasScript = true;
                        }
                    }
                }
            }
            // Check whether digits propagate right
            if (word.dicFlags.have(Dicf::TRIG_DIGIT_PROPAGATES_RIGHT)) {
                digitPropagatesRight = true;
            }
            // Check for custom capitalization
            if (!wordInfo.customCap.empty()) {
                word.customCap = wordInfo.customCap;
            }
        }

        // Check for ideograph capitalization
        if (word.original.starts_with(STR_IDEOGRAPH)) {
            auto remainder = word.original.substr(STR_IDEOGRAPH.length());
            if (isHex(remainder)) {
                auto& w = tempWords.emplace_back(word.original);
                for (size_t i = 0; i < STR_IDEOGRAPH.length(); ++i) {
                    w[i] = tolower(w[i]);
                }
                word.customCap = w;
            }
        }

        if (nextWord) {
            if (nextWord->dicFlags.have(Dicf::PART_NOUN)) {
                word.nextNoun = true;
            } else if (nextWord->dicFlags.have(Dicf::PART_ADJECTIVE)) {
                word.nextAdjective = true;
            } else if (nextWord->dicFlags.have(Dicf::PART_NUMERAL)) {
                if (nextNextWord && nextNextWord->dicFlags.have(Dicf::PART_NOUN)) {     // tone Two dots → next noun “dots”, not numeral 2
                    //word.nextNoun = true;         // But do not register it, “tone” will also be a noun
                } else {
                    word.nextNumeral = true;
                }
            } else if (nextWord->dicFlags.have(Dicf::PART_MIXED)) {
                if (nextWord->nextNoun || nextWord->nextAdjective) {
                    word.nextAdjective = true;
                } else {
                    word.nextNoun = true;
                }
            }
        }
        nextNextWord = nextWord;  nextWord = &word;
    }

    if (debug != DecapDebug::NO)
        dump(words);

    // Propagate right
    int propagationLevel = 0;
    Word* prevWord = nullptr;
    for (auto& word : words) {
        bool wantPropagate = false;
        if (word.dicFlags.have(Dicf::PART_MIXED)) {
            if (word.nextNoun) {
                // Adjective
                word.isAdjective = true;
            }
        }
        if (propagationLevel != 0) {
            if (!prevWord)
                throw std::logic_error("prevWord should be non-null!");
            if (word.dicFlags.have(Dicf::PART_MIXED)) {
                if (word.isAdjective    // We already defined that it is adjective
                        || (word.nextAdjective && propagationLevel > 1)) {  // High tone thing
                    // Adjective
                    word.isAdjective = true;
                    wantPropagate = true;
                } else {
                    // Noun
                    word.isCapital = true;
                }
            } else if (word.dicFlags.have(Dicf::PART_ADJECTIVE)) {
                wantPropagate = true;
                word.isAdjective = true;
                // Adj + adj = no cap; “First second third Pi”
                if (!prevWord->isAdjective) {
                    word.isCapital = true;
                }
            } else if (word.dicFlags.have(Dicf::PART_NOUN)) {
                // Stop propagating;
                // Adj + noun = no cap; “Noun”, but “First noun”
                if (!prevWord->isAdjective) {
                    word.isCapital = true;
                }
            } else {
                word.isCapital = true;
            }
            if (digitPropagatesRight && hasDigit(word.original)) {
                // Adjectives propagate further
                // Digits too if we set this
                wantPropagate = true;
            }
        }
        bool wantTrigger = true;
        if (word.dicFlags.have(Dicf::IF_SCRIPT) && !hasScript)
            wantTrigger = false;
        if (word.dicFlags.have(Dicf::IF_NEXT_NOT_NOUN) && (word.nextNoun || word.nextNumeral))
            wantTrigger = false;
        if (wantTrigger) {
            if (word.dicFlags.have(Dicf::TRIG_CAP_NEXT) && !word.isAdjective)
                wantPropagate = true;
        }
        if (word.dicFlags.have(Dicf::PART_MAYBE_LETTER)
                && prevWord && prevWord->dicFlags.have(Dicf::PART_ADJECTIVE)) {
            word.isCapital = true;
        }
        prevWord = &word;
        if (wantPropagate) {
            ++propagationLevel;
        } else {
            propagationLevel = 0;
        }
    }

    // Check for idioms
    for (size_t posIdiom = 0; posIdiom < words.size(); ++posIdiom) {
        IdiomEntry idiomKey { words[posIdiom].original, IsPrefix::NO, IdiomAction::NOMATTER };
        auto range = idioms.equal_range(idiomKey);
        for (auto it = range.first; it != range.second; ++it) {
            auto& idiom = *it;
            if ((!idiom.isPrefix() || posIdiom == 0)           // Prefix?
                    && cp >= idiom.lo && cp <= idiom.hi      // Code point OK?
                    && doesIdiomFit(words, posIdiom, idiom.toBuf(), idiom.doesNeedWordAfter())) {
                doIdiomAction(words, posIdiom, posIdiom + idiom.nTriggers, idiom.action);
                break;
            }
        }
    }

    // Find length capitalization
    size_t newLen = words.empty() ? 0 : words.size() - 1;
    for (auto& word : words) {
        newLen += word.original.length();
    }

    // Cuneiform recapitalization
    if (words[0].original == "CUNEIFORM"sv) {
        for (auto& word : words) {
            auto q = dictionary.find(word.original);
            // 1. Symbols — YES
            if (cuneiformSymbols.contains(word.original)) {
                word.isAllCap = true;
            // 2. Script/noun/adjective — NO
            } else if (q != dictionary.end() && q->second.flags.haveAny(TRIG_ANY_SCRIPT
                        | Dicf::PART_ADJECTIVE | Dicf::PART_NOUN | Dicf::PART_NUMERAL)) {
                // do nothing
            // 3. Suffix — initial capital
            } else if (cuneiformSuffixes.contains(word.original)) {
                word.isCapital = true;
            // 4. Keyword — YES
            } else if (!cuneiformKeywords.contains(word.original)) {
                word.isAllCap = true;
            }
        }
    }

    // Compose
    std::string r;
    r.reserve(newLen);
    for (auto& word : words) {
        if (!r.empty())
            r.append(" "sv);

        WordState state = WordState::SMALL;        
        if (word.isAllCap) {
            state = WordState::ALL_CAP;
        } else if (word.dicFlags.have(Dicf::CAP_SMALL)) {
            state = WordState::SMALL;
        } else if (word.dicFlags.have(Dicf::CAP_ALL)) {
            state = WordState::ALL_CAP;
        } else if (word.dicFlags.have(Dicf::CAP_TITLE)) {
            state = WordState::TITLE;
        } else if (!word.customCap.empty()) {
            state = word.isCapital ? WordState::CUSTOM_TITLE : WordState::CUSTOM;
        } else if (word.isCapital) {
            state = WordState::TITLE;
        }

        switch (state) {
        case WordState::CUSTOM:
            r.append(word.customCap);
            break;
        case WordState::CUSTOM_TITLE:
            appendAsIsTitle(r, word.customCap);
            break;
        case WordState::SMALL:
            appendLower(r, word.original);
            break;
        case WordState::TITLE:
            appendTitle(r, word.original);
            break;
        case WordState::ALL_CAP:
            appendUpper(r, word.original);
            break;
        }
    }

    return r;
}


bool isAlternate(char32_t x)
{
    auto it = alternateRanges.lower_bound(RangeByEnd{x,x});
    if (it == alternateRanges.end())
        return false;
    // so x <= it->b
    // upper_bound gives x < it->b
    return (it->a <= x);
}


bool isNoAa(char32_t x)
{
    auto it = noAaRanges.lower_bound(RangeByEnd{x,x});
    if (it == noAaRanges.end())
        return false;
    // so x <= it->b
    // upper_bound gives x < it->b
    return (it->a <= x);
}


std::string decapitalizeEmoji(
        std::string_view x,
        DecapDebug debug)
{
    auto list = str::splitSv(x, ": ", false);
    if (list.size() == 1) {
        return decapitalize(x, 0, debug);
    }
    std::string r;
    for (auto v : list) {
        if (!r.empty())
            r += ": ";
        auto decap = decapitalize(v, 0, debug);
        r += decap;
    }
    return r;
}


std::string decapitalizeByTable(std::string_view x)
{
    std::string upper = str::toUpper(x);
    auto itEx = exceptions.find(upper);
    if (itEx != exceptions.end())
        return std::string(itEx->second.r);
    return std::string{x};
}


Flags<uc::Cfg> styleFlags(char32_t x)
{
    auto it = styleRanges.lower_bound(RangeByEnd{x,x});
    if (it != styleRanges.end()) {
        // so x <= it->b
        // upper_bound gives x < it->b
        if (it->first.a <= x) {
            return it->second;
        }
    }
    return {};
}
