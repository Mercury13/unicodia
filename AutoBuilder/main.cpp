// C++
#include <iostream>
#include <fstream>
#include <charconv>
#include <deque>

// PugiXML
#include "pugixml.hpp"

// Project-local
#include "data.h"

using namespace std::string_view_literals;

template <class T>
inline auto need(T&& val, const char* errmsg)
{
    if (!val) throw std::logic_error(errmsg);
    return std::forward<T>(val);
}

struct Numeric {
    long long num, denom;
    std::string_view type, textValue;
    size_t index;
};

long long fromChars(std::string_view x, std::string_view numType)
{
    long long r;
    auto beg = x.data();
    auto end = beg + x.length();
    auto res = std::from_chars(beg, end, r);
    if (res.ec != std::errc() || res.ptr != end) {
        std::string errm = "[fromChars] Cannot parse number ";
        errm.append(x);
        errm.append(", numType = ");
        errm.append(numType);
        throw std::invalid_argument(errm);
    }
    return r;
}

unsigned fromHex(std::string_view x)
{
    long long r;
    auto beg = x.data();
    auto end = beg + x.length();
    auto res = std::from_chars(beg, end, r, 16);
    if (res.ec != std::errc() || res.ptr != end) {
        std::string errm = "[fromChars] Cannot parse hex ";
        errm.append(x);
        throw std::invalid_argument(errm);
    }
    return r;
}

Numeric parseNumeric(std::string_view numType, std::string_view x, size_t index)
{
    if (x == "NaN"sv) {
        return { 0, 0, numType, x, index };
    }
    auto pSlash = x.find('/');
    if (pSlash == std::string_view::npos) {
        // Integer
        return { fromChars(x, numType), 1, numType, x, index };
    } else {
        // Fraction
        auto num = x.substr(0, pSlash);
        auto den = x.substr(pSlash + 1);
        return { fromChars(num, numType), fromChars(den, numType), numType, x, index };
    }
}

struct NumCache
{
    std::deque<Numeric> ord;
    std::map<std::string, const Numeric*> ndx;

    const Numeric& parse(std::string_view numType, std::string_view x);
    size_t size() const { return ord.size(); }
};

const Numeric& NumCache::parse(std::string_view numType, std::string_view x)
{
    // Get key
    std::string key;
    key.reserve(numType.length() + x.length());
    key.append(numType);
    key.append(x);
    // Check index
    auto it = ndx.find(key);
    if (it != ndx.end())
        return *it->second;
    // Insert
    Numeric newNum = parseNumeric(numType, x, ord.size());
    auto& newPlace = ord.emplace_back(newNum);
    ndx[key] = &newPlace;
    return newPlace;
}

    std::string transformVersion(std::string s)
{
    for (auto& c : s) {
        if (c == '.')
            c = '_';
    }
    return s;
}

inline std::string transformVersion(std::string_view s) { return transformVersion(std::string{s}); }

struct StrMap {
    std::string_view source, image;
};

template <size_t N>
std::string_view transform(std::string_view x, StrMap (&map)[N])
{
    for (auto& v : map) {
        if (v.source == x) {
            return v.image;
        }
    }
    std::string errm = "[transform] Bad string ";
    errm.append(x);
    throw std::invalid_argument(std::move(errm));
}

StrMap smNumType[] {
    { "De"sv, "DIGIT" },
    { "Di"sv, "SPECIAL_DIGIT" },
    { "None"sv, "NONE" },
    { "Nu"sv, "NUMBER" },
};
StrMap smCharCat[] {
    { "Cc"sv, "CONTROL"sv },
    { "Cf"sv, "FORMAT"sv },
    { "Ll"sv, "LETTER_LOWERCASE"sv },
    { "Lm"sv, "LETTER_MODIFIER"sv },
    { "Lo"sv, "LETTER_OTHER"sv },
    { "Lt"sv, "LETTER_TITLECASE"sv },
    { "Lu"sv, "LETTER_UPPERCASE"sv },
    { "Mc"sv, "MARK_SPACING"sv },
    { "Me"sv, "MARK_ENCLOSING"sv },
    { "Mn"sv, "MARK_NONSPACING"sv },
    { "Nd"sv, "NUMBER_DECIMAL"sv },
    { "Nl"sv, "NUMBER_LETTER"sv },
    { "No"sv, "NUMBER_OTHER"sv },
    { "Pc"sv, "PUNCTUATION_CONNECTOR"sv },
    { "Pd"sv, "PUNCTUATION_DASH"sv },
    { "Pe"sv, "PUNCTUATION_CLOSE"sv },
    { "Pf"sv, "PUNCTUATION_FINAL"sv },
    { "Pi"sv, "PUNCTUATION_INITIAL"sv },
    { "Po"sv, "PUNCTUATION_OTHER"sv },
    { "Ps"sv, "PUNCTUATION_OPEN"sv },
    { "Sc"sv, "SYMBOL_CURRENCY"sv },
    { "Sk"sv, "SYMBOL_MODIFIER"sv },
    { "Sm"sv, "SYMBOL_MATH"sv },
    { "So"sv, "SYMBOL_OTHER"sv },
    { "Zl"sv, "SEPARATOR_LINE"sv },
    { "Zp"sv, "SEPARATOR_PARAGRAPH"sv },
    { "Zs"sv, "SEPARATOR_SPACE"sv },
};

enum class AbbrevState { NORMAL, ALIAS, DISABLE };


struct StringPayload
{
    char32_t subj = 0;
    int offset = -1;
};

struct RememberResult
{
    int offset;
    bool wasIns;
};

class StringLib
{
public:
    /// @return offset
    RememberResult remember(const std::string& s, char32_t subj);
    RememberResult forceRemember(const std::string& s, char32_t subj);
    auto& inOrder() const { return fInOrder; }
private:
    using M = std::unordered_map<std::string, StringPayload>;
    M fData;
    std::deque<M::value_type> fRepeats;     // need ptrs → deque
    std::vector<const M::value_type*> fInOrder;
    size_t fLength = 0;
};

RememberResult StringLib::forceRemember(const std::string& s, char32_t subj)
{
    auto res = remember(s, subj);
    if (!res.wasIns) {
        auto& v = fRepeats.emplace_back(s, StringPayload{subj, static_cast<int>(fLength)});
        res.offset = fLength;
        fLength += (s.length() + 1);
        fInOrder.push_back(&v);
    }
    return res;
}

RememberResult StringLib::remember(const std::string& s, char32_t subj)
{
    auto [it, wasIns] = fData.try_emplace(s, StringPayload());
    if (wasIns) {   // Was inserted
        it->second.subj = subj;
        it->second.offset = fLength;
        fLength += (s.length() + 1);
        fInOrder.push_back(&*it);
    } else {}    // was found — do nothing
    return { it->second.offset, wasIns };
}


bool hasSubstr(std::string_view haystack, std::string_view needle)
{
    auto pos = haystack.find(needle);
    return (pos != std::string_view::npos);
}


class NewLine
{
public:
    void trigger();
private:
    bool isCocked = true;
};


void NewLine::trigger()
{
    if (isCocked) {
        std::cout << std::endl;
        isCocked = false;
    }
}


int main()
{
    std::ofstream os("UcAuto.cpp");
    os << "// Automatically generated, do not edit!" << '\n';
    os << '\n';
    os << R"(#include "UcAutoDefines.h")" << '\n';
    os << '\n';
    os << R"(using namespace std::string_view_literals;)" << '\n';
    os << '\n';

    pugi::xml_document doc;

    std::cout << "Loading Unicode base..." << std::flush;
    doc.load_file("ucd.all.flat.xml");
    std::cout << "OK" << std::endl;

    ///// CpInfo ///////////////////////////////////////////////////////////////

    unsigned nChars = 0, nSpecialRanges = 0;

    auto elRoot = need(doc.root().child("ucd"), "Need <ucd>");
    auto elRepertoire = need(elRoot.child("repertoire"), "Need <repertoire>");
    std::cout << "Found repertoire, generating character info..." << std::flush;
    NewLine nl;
    os << '\n';
    os << R"(uc::Cp uc::cpInfo[N_CPS] {)" << '\n';

    StringLib strings;
    NumCache nums;
    int nDeprecated = 0;
    for (pugi::xml_node elChar : elRepertoire.children("char")) {
        std::string_view sCp = elChar.attribute("cp").as_string();
        if (sCp.empty()) {
            ++nSpecialRanges;
            continue;
        }
        // Get CP
        auto cp = fromHex(sCp);

        // Aliases:
        // • Abbreviation: Implement later
        // • Alternate: Prefer na1
        // • Control: Prefer na1
        // • Figment: Implement

        /// @todo [future] Sometimes we have fixups, what to take?
        /// @todo [future] Sometimes we have abbreviations, take them
        std::string_view sName = elChar.attribute("na").as_string();
        if (sName.empty())
            sName = elChar.attribute("na1").as_string();

        std::string_view defaultAbbrev {};      // empty
        std::vector<std::string_view> allAbbrevs;
        std::vector<std::string> restAliases;

        AbbrevState abbrevState = AbbrevState::NORMAL;
        if (cp >= 0xE0000 && cp <= 0xE007F) {
            allAbbrevs.push_back("`");
        } else if (auto it = abbrevs.find(cp); it != abbrevs.end()) {
            if (it->second.empty()) {
                abbrevState = AbbrevState::DISABLE;
            } else {
                if (it->second == "!"sv) {
                    abbrevState = AbbrevState::ALIAS;
                } else {
                    defaultAbbrev = it->second;
                    allAbbrevs.push_back(defaultAbbrev);
                }
            }
        }

        // Aliases?
        for (auto elAlias : elChar.children("name-alias")) {
            std::string_view sType = elAlias.attribute("type").as_string();
            if (sType == "alternate"sv || sType == "control"sv || sType == "figment") {
                if (sName.empty())
                    sName = elAlias.attribute("alias").as_string();
            } else if (sType == "correction") {
                // Checked known chars, and corrections ARE BETTER than originals
                sName = elAlias.attribute("alias").as_string();
            } else if (sType == "abbreviation") {
                // Abbreviations
                switch (abbrevState) {
                case AbbrevState::DISABLE: break;
                case AbbrevState::NORMAL: {
                        auto abbr = elAlias.attribute("alias").as_string();
                        if (abbr != defaultAbbrev)      // do not dupe defaultAbbrev
                            allAbbrevs.push_back(abbr);
                    }
                    break;
                case AbbrevState::ALIAS:
                    restAliases.emplace_back(elAlias.attribute("alias").as_string());
                    break;
                }
            }
        }

        if (allAbbrevs.size() > 1 && defaultAbbrev.empty()) {
            nl.trigger();
            std::cout << "WARNING: char " << std::hex << cp << " has no default abbreviation." << std::endl;
        }
        std::string sLowerName = decapitalize(sName);
        auto [iTech, wasIns] = strings.remember(sLowerName, cp);
        if (!allAbbrevs.empty() && !wasIns) {
            nl.trigger();
            std::cout << "WARNING: char " << std::hex << cp << " has an abbreviation and a repeating name." << std::endl;
        }

        for (auto& v : allAbbrevs) {
            strings.forceRemember(std::string{v}, cp);
        }

        auto nAliases = allAbbrevs.size();
        int flags = 0;
        if (!allAbbrevs.empty())
            flags |= 1;
        // Deprecated
        if (elChar.attribute("Dep").as_string()[0] == 'Y') {
            flags |= 2;
            ++nDeprecated;
        }

        os << "{ "
           << "0x" << std::hex << cp << ", "    // subj
           << "{ "                              // name
                << std::dec << iTech << ", "      // name.tech,
                << nAliases << " "                // name.alt
           << " }, ";                           // /name

        // Char’s type
        std::string_view sCharCat = elChar.attribute("gc").as_string();
        os << "EcCategory::" << transform(sCharCat, smCharCat) << ", ";

        // Char’s version
        std::string_view sVersion = elChar.attribute("age").as_string();
        os << "EcVersion::V_" << transformVersion(sVersion) << ", ";

        // Char’s bidirectional data
        std::string_view sBidiClass = elChar.attribute("bc").as_string();
        bool isMirrored = elChar.attribute("Bidi_M").as_bool();
        if (isMirrored) {
            if (sBidiClass != "ON"sv)
                throw std::logic_error("Got strange bidi class for mirrored char");
            sBidiClass = "MIR";
        }
        os << "EcBidiClass::z_" << sBidiClass << ", ";

        // Char’s script
        std::string_view sScript = elChar.attribute("sc").as_string();
            // Hiragana → Hentaigata
        if (sScript == "Hira"sv && sName.starts_with("HENTAIGANA"))
            sScript = "Hent"sv;
        //std::string_view sScriptX = elChar.attribute("scx").as_string();
        //if (sScript != sScriptX) {
        //    std::cout << sCp << " has sc=" << sScript << ", scx=" << sScriptX << std::endl;
        //}
        os << "EcScript::" << sScript << ", ";

        // Char’s numeric values
        // nt = …
        //    • None — no numeric value
        //    • De — decimal digit
        //    • Di — special digit
        //    • Nu — number
        // nv = Nan / whole number / vulgar fraction
        std::string_view sNumType = elChar.attribute("nt").as_string();
        std::string_view sNumValue = elChar.attribute("nv").as_string();
        auto& numPlace = nums.parse(sNumType, sNumValue);
        os << numPlace.index << ", ";

        if (flags) {
            os << "Cfgs{" << flags << "}";
        } else {
            os << "{}";
        }

        os << "}," << '\n';
        ++nChars;
    }

    os << "};" << '\n';

    std::cout << "OK" << std::endl;
    std::cout << "Found " << std::dec << nChars << " chars, "
              << nDeprecated << " deprecated, "
              << nSpecialRanges << " special ranges." << std::endl;

    os << "const char8_t uc::allStrings[] = \n";
    for (auto& v : strings.inOrder()) {
        os << R"(u8")" << v->first << R"("   "\0"   // )" << std::hex << static_cast<int>(v->second.subj) << '\n';
    }
    os << ";\n";

    ///// Blocks ///////////////////////////////////////////////////////////////

    size_t nBlocks = 0;
    auto elBlocks = need(elRoot.child("blocks"), "Need <blocks>");

    for (pugi::xml_node elBlock : elBlocks.children("block")) {
        //auto sFirst = elBlock.attribute("first-cp").as_string();
        //auto sLast = elBlock.attribute("last-cp").as_string();
        std::string_view name = elBlock.attribute("name").as_string();

        if (hasSubstr(name, "Private Use") || hasSubstr(name, "Surrogate"))
            continue;

        ++nBlocks;
    }
    std::cout << "Found " << std::dec << nBlocks << " blocks." << std::endl;

    ///// Numerics /////////////////////////////////////////////////////////////

    std::cout << "Stockpiled " << nums.size() << " numerics." << std::endl;
    os << "const uc::Numeric uc::allNumerics[uc::N_NUMERICS] { \n";
    for (const auto& v : nums.ord) {
        os << "{ " << std::dec << v.num << ", " << v.denom
           << ", EcNumType::" << transform(v.type, smNumType)
           << " },  // " << v.index << " is " << v.textValue << '\n';
    }
    os << "};\n";

    os.close();

    os.open("UcAutoCount.h");
    os << "#pragma once\n";
    os << '\n';
    os << "// Automatically generated, do not edit!\n";
    os << '\n';
    os << "namespace uc {\n";
    os << "constexpr int N_CPS = " << std::dec << nChars << ";\n";
    os << "constexpr int N_BLOCKS = " << std::dec << nBlocks << ";\n";
    os << "constexpr int N_NUMERICS = " << std::dec << nums.size() << ";\n";
    os << "}\n";

    std::cout << "Successfully finished!" << std::endl << std::endl;

    return 0;
}
