#include "decap.h"

#include "u_Strings.h"  // Used in macros

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
        // Talu
    EX("New Tai Lue vowel sign Vowel shortener")
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
    EX("squared Katakana De")
    EX("squared Katakana Koko")
    EX("squared Katakana Sa")
    EX("kaeriten Re")
    EX("symbol of unification")
    EX("di ren")
    EX("ren di")
    EX("Chinese small simplified Er")
    EX("Chinese small traditional Er")

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
    EX("combining Is below")            // Tricky
    EX("combining Ur above")            // Same
    EX("combining Us above")            // Same
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
    EX("Iris")                          // Not always
    EX("Flora")                         // Not always
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
        // Maps
    EX("Japanese Self-Defence Force site")  // Badly decapped
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
