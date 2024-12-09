#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <span>

#include "u_Strings.h"

enum class RMode { NONE, REPH, OTHER };

// Tulu consonant
struct Cons {
    char32_t code;
    std::string_view ascName;
    std::u8string_view niceName;
    RMode rMode = RMode::NONE;
    /// KK, KKh, KN, KNn etc are always written.
    /// If you need to write e.g. KKhV and KTV:
    ///   "KhV,TV"
    std::string_view attestedBigTails {};
    /// Same for issues: if KV, KVUu and KVvocR us bad, write "V,VUu,VZz"
    struct Issues {
        std::string_view techVisible {};    ///< technical issues visible → really bad
        std::string_view bad {};            ///< bad rendering, better fix
        std::string_view neglect {};        ///< bad rendering, just neglect
        std::string_view nice {};           ///< other but nice rendering
    } issueTails {};
};

struct Vowel {
    char32_t code;
    /// Vocalic names: Z, Zz
    /// Looped virama name: Q
    std::string_view ascName;
    std::u8string_view niceName;
    bool isEverywhere;
};


constinit const Cons allCons[] {
    { .code = 0x11392, .ascName = "K",   .niceName = u8"K", },
    { .code = 0x11393, .ascName = "Kh",  .niceName = u8"Kʰ" },
    { .code = 0x11394, .ascName = "G",   .niceName = u8"G"  },
    { .code = 0x11395, .ascName = "Gh",  .niceName = u8"Gʰ" },
    { .code = 0x11396, .ascName = "Ng",  .niceName = u8"Ŋ"  },
    { .code = 0x11397, .ascName = "C",   .niceName = u8"C"  },
    { .code = 0x11398, .ascName = "Ch",  .niceName = u8"Cʰ" },
    { .code = 0x11399, .ascName = "J",   .niceName = u8"J"  },
    { .code = 0x1139A, .ascName = "Jh",  .niceName = u8"Jʰ" },
    { .code = 0x1139B, .ascName = "Ny",  .niceName = u8"Ñ"  },
    { .code = 0x1139C, .ascName = "Tt",  .niceName = u8"Ṯ"  },
    { .code = 0x1139D, .ascName = "Tth", .niceName = u8"Ṯʰ" },
    { .code = 0x1139E, .ascName = "Dd",  .niceName = u8"Ḏ"  },
    { .code = 0x1139F, .ascName = "Ddh", .niceName = u8"Ḏʰ" },
    { .code = 0x113A0, .ascName = "Nn",  .niceName = u8"Ṉ"  },
    { .code = 0x113A1, .ascName = "T",   .niceName = u8"T"  },
    { .code = 0x113A2, .ascName = "Th",  .niceName = u8"Tʰ" },
    { .code = 0x113A3, .ascName = "D",   .niceName = u8"D"  },
    { .code = 0x113A4, .ascName = "Dh",  .niceName = u8"Dʰ" },
    { .code = 0x113A5, .ascName = "N",   .niceName = u8"N"  },
    { .code = 0x113A6, .ascName = "P",   .niceName = u8"P"  },
    { .code = 0x113A7, .ascName = "Ph",  .niceName = u8"Pʰ" },
    { .code = 0x113A8, .ascName = "B",   .niceName = u8"B"  },
    { .code = 0x113A9, .ascName = "Bh",  .niceName = u8"Bʰ" },
    { .code = 0x113AA, .ascName = "M",   .niceName = u8"M"  },
    { .code = 0x113AB, .ascName = "Y",   .niceName = u8"Y"  },
    { .code = 0x113AC, .ascName = "R",   .niceName = u8"R", .rMode = RMode::REPH },
    { .code = 0x113AD, .ascName = "L",   .niceName = u8"L"  },
    { .code = 0x113AE, .ascName = "V",   .niceName = u8"V"  },
    { .code = 0x113AF, .ascName = "Sh",  .niceName = u8"Sʰ" },
    { .code = 0x113B0, .ascName = "Ss",  .niceName = u8"Š"  },
    { .code = 0x113B1, .ascName = "S",   .niceName = u8"S"  },
    { .code = 0x113B2, .ascName = "H",   .niceName = u8"H"  },
    { .code = 0x113B3, .ascName = "Ll",  .niceName = u8"Ḻ"  },
    { .code = 0x113B4, .ascName = "Rr",  .niceName = u8"Ṟ", .rMode = RMode::OTHER },
    { .code = 0x113B5, .ascName = "Lll", .niceName = u8"Ł"  },
};

constinit const Vowel allVowels[] {
    { .code = 0,       .ascName {},     .niceName {},       .isEverywhere = true  },
    { .code = 0x113B8, .ascName = "Aa", .niceName = u8"Ā",  .isEverywhere = true  },
    { .code = 0x113B9, .ascName = "I",  .niceName = u8"I",  .isEverywhere = true  },
    { .code = 0x113BA, .ascName = "Ii", .niceName = u8"Ii", .isEverywhere = true  },
    { .code = 0x113BB, .ascName = "U",  .niceName = u8"U",  .isEverywhere = true  },
    { .code = 0x113BC, .ascName = "Uu", .niceName = u8"Ū",  .isEverywhere = true  },
    { .code = 0x113BD, .ascName = "Z",  .niceName = u8"Ŕ",  .isEverywhere = true  },
    { .code = 0x113BE, .ascName = "Zz", .niceName = u8"Ŕ:", .isEverywhere = false },
    { .code = 0x113BF, .ascName = "W",  .niceName = u8"Ĺ",  .isEverywhere = false },
    { .code = 0x113C0, .ascName = "Ww", .niceName = u8"Ĺ:", .isEverywhere = false },
    { .code = 0x113C2, .ascName = "Ee", .niceName = u8"Ē",  .isEverywhere = true  },
    { .code = 0x113C5, .ascName = "Ai", .niceName = u8"Ai", .isEverywhere = true  },
    { .code = 0x113C7, .ascName = "Au", .niceName = u8"Au", .isEverywhere = true  },
    { .code = 0x113C7, .ascName = "Q",  .niceName = u8"Q",  .isEverywhere = false },
};


constexpr char32_t CONJOINER = 0x113D0;
constexpr char32_t REPH = 0x113D1;


void writeCode(std::ostream& os, int x)
{
    char buf[20];
    snprintf(buf, std::size(buf), "&#%d;", x);
    os << buf;
}


void writeSyllable(std::ostream& os,
                   std::span<const Cons*> consonants,
                   const Vowel& vowel)
{
    unsigned index = 0;
    for (auto& c : consonants) {
        if (index == 0 && c->rMode == RMode::REPH) {
            writeCode(os, REPH);
        } else {
            writeCode(os, c->code);
            if (index + 1 < consonants.size())
                writeCode(os, CONJOINER);
        }
        ++index;
    }
    if (vowel.code != 0) {
        writeCode(os, vowel.code);
    }
}


void writeSpaces(std::ostream& os, unsigned x)
{
    for (unsigned i = 0; i < x; ++i)
        os.write(" ", 1);
}


void writeSyllableName(std::ostream& os,
                       std::span<const Cons*> consonants)
{
    for (auto c : consonants) {
        os << str::toSv(c->niceName);
    }
}


enum class ExtendedSet : unsigned char { NO, YES };
inline bool is(ExtendedSet x) { return static_cast<bool>(x); }


void writeRow(std::ostream& os,
              unsigned nSpaces,
              std::span<const Cons*> consonants,
              ExtendedSet extendedSet)
{
    writeSpaces(os, nSpaces);
    os << "<tr>";

    writeSpaces(os, nSpaces + 1);
    os << "<td>";
    writeSyllableName(os, consonants);
    os << '\n';

    writeSpaces(os, nSpaces + 1);
    os << "<td class=\"daa\">";
    for (auto& v : allVowels) {
        if (v.isEverywhere || is(extendedSet)) {
            if (v.code != 0)
                os << ' ';
            writeSyllable(os, consonants, v);
        }
    }
    os << '\n';
}


#define FNAME_IN  "TuluTemplate.html"
#define FNAME_OUT "TuluTest2.html"

std::string readAll(std::istream& is)
{
    std::stringstream ss;
    ss << is.rdbuf();
    return std::move(ss).str();
}

struct FindInd {
    size_t pos = std::string_view::npos;
    unsigned nSpaces = 0;
};

FindInd findIndented(std::string_view s, std::string_view text)
{
    FindInd r;
    r.pos = s.find(text);
    if (r.pos != std::string_view::npos) {
        while (r.pos > 0 && s[r.pos] == ' ') {
            --r.pos;
            ++r.nSpaces;
        }
    }
    return r;
}

int main()
{
    try {
        // Read file’s content
        std::ifstream is(FNAME_IN);
        if (!is.is_open())
            throw std::logic_error("File " FNAME_IN " not open");
        auto sContent = readAll(is);
        std::string_view content = sContent;

        // Find position of </table>
        auto [pos, nSpaces] = findIndented(content, "</table>");
        if (pos == std::string_view::npos)
            throw std::logic_error("Cannot find </table> in " FNAME_IN);

        is.close();

        // Start
        std::ofstream os(FNAME_OUT);
        os << content.substr(0, pos);

        for (auto& c1 : allCons) {
            for (auto& c2 : allCons) {
                // Pair impossible
                if (c1.rMode == RMode::REPH && c2.rMode == RMode::OTHER)
                    continue;
                const Cons* simpleSyl[] = { &c1, &c2 };
                writeRow(os, nSpaces, simpleSyl, ExtendedSet::YES);
            }
        }

        os << content.substr(pos);

        std::cout << "OK" "\n";

    } catch (const std::exception& e) {
        std::cout << "ERR: " << e.what() << '\n';
    }
}
