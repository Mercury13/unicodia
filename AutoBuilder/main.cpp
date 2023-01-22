// C++
#include <iostream>
#include <fstream>
#include <charconv>
#include <deque>
#include <unordered_set>

// PugiXML
#include "pugixml.hpp"

// My libs
#include "u_Strings.h"

// Project-local
#include "data.h"
#include "loader.h"
#include "library.h"
#include "egyptian.h"

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


struct StringData
{
    std::string s;
    char32_t subj;
    int offset;
    uc::TextRole role;
    bool isLast;
};

struct RememberResult
{
    StringData* data;
    bool wasIns;    
};

class StringLib
{
public:
    RememberResult remember(
            char32_t subj,
            uc::TextRole role,
            std::string_view s);
    RememberResult forceRemember(
            char32_t subj,
            uc::TextRole role,
            std::string_view s);
    void finishCp();
    auto& inOrder() const { return fInOrder; }
private:
    using M = std::unordered_map<std::string_view, StringData*>;
    M fNdx;
    std::deque<StringData> fInOrder;
    size_t fLength = 0;
};

RememberResult StringLib::forceRemember(
        char32_t subj, uc::TextRole role, std::string_view aS)
{
    auto& v = fInOrder.emplace_back(StringData{
                    .s = std::string { aS },
                    .subj = subj,
                    .offset = static_cast<int>(fLength),
                    .role = role,
                    .isLast = false });
    fLength += (aS.length() + 2);
    return { .data =&v, .wasIns = true };
}

RememberResult StringLib::remember(
        char32_t subj, uc::TextRole role, std::string_view s)
{
    if (role != uc::TextRole::MAIN_NAME)
        return forceRemember(subj, role, s);
    auto it = fNdx.find(s);
    if (it == fNdx.end()) {   // Was inserted
        auto r = forceRemember(subj, role, s);
        fNdx[r.data->s] = r.data;
        return r;
    } else {
        return {
            .data = it->second,
            .wasIns = false
        };
    }
}


void StringLib::finishCp()
{
    if (!fInOrder.empty()) {
        auto& bk = fInOrder.back();
        if (!bk.isLast) {
            bk.isLast = true;
            // +1: command CMD_END
            ++fLength;
        }
    }
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


std::string_view findTag(
        std::string_view haystack,
        std::string_view open, std::string_view close)
{
    auto p = haystack.find(open);
    if (p == std::string_view::npos)
        return {};
    // Skip what we found
    p += open.size();
    if (!close.empty()) {
        // Special close tag
        size_t p2 = haystack.find(close, p);
        if (p2 == std::string_view::npos)
            return {};
        return haystack.substr(p, p2 - p);
    } else {
        // Close same as open; did not find → return entire tail
        size_t p2 = haystack.find(open, p);
        if (p2 == std::string_view::npos)
            return haystack.substr(p);
        return haystack.substr(p, p2 - p);
    }
}


void tagMinus(std::string_view& haystack,
              size_t lenClose,
              std::string_view r)
{
    auto pBeg = std::to_address(r.end());
    auto pEnd = std::to_address(haystack.end());
    if (pBeg == pEnd) {
        haystack = {};
        return;
    }

    pBeg += lenClose;
    size_t newLen = pEnd - pBeg;
    if (newLen >= haystack.length())
        throw std::logic_error("[tagMinus] Result is not within haystack's memory");

    haystack = { pBeg, newLen };
}


/// \brief findTagMinus
///    Finds tag (open..open) or (open..close), sets haystack to
///        (close .. haystack.end)
/// \param haystack   where we find///
/// \param open       opening tag
/// \param close      closing tag (⌀ to find from open to open)
/// \return  IF FINDING OPEN..CLOSE:  [+] [open.end .. close.begin), haystack := [close.end .. )
///          IF FINDING OPEN..OPEN: [+] [open.end .. open2.begin), haystack := [open2.begin .. )
///          [-] not found (in both cases), haystack intact
///
std::string_view findTagMinus(
        std::string_view& haystack,
        std::string_view open, std::string_view close)
{
    auto r = findTag(haystack, open, close);
    if (r.empty())
        return {};

    tagMinus(haystack, close.length(), r);
    return r;
}


std::string_view rqTag(
        std::string_view haystack,
        std::string_view open, std::string_view close)
{
    auto r = findTag(haystack, open, close);
    if (r.empty()) {
        std::string data = "Cannot find tag ";
        data.append(open);
        throw std::logic_error(data);
    }
    return r;
}


///
/// \brief rqTagMinus
/// @see findTagMinus, but it REQUIRES that tag
///
std::string_view rqTagMinus(
        std::string_view& haystack,
        std::string_view open, std::string_view close)
{
    auto r = rqTag(haystack, open, close);
    tagMinus(haystack, close.length(), r);
    return r;
}


struct NotoData {
    std::unordered_set<char32_t> singleChar;
};


//inline std::string_view chompPrefSuffSv(
//        std::string_view x, std::string_view pref, std::string_view suff)
//{
//    auto len2 = pref.length() + suff.length();
//    if (x.length() > len2 && x.starts_with(pref) && x.ends_with(suff)) {
//        return x.substr(pref.length(), x.length() - len2);
//    } else {
//        return {};
//    }
//}


//inline std::string_view chompPref(std::string_view x, std::string_view pref)
//{
//    if (x.starts_with(pref)) {
//        return x.substr(pref.length());
//    } else {
//        return {};
//    }
//}


NotoData loadNotoEmoji()
{
    NotoData r;
    std::ifstream is(SINGLEEMOJI_TXT);
    std::string s;
    while (std::getline(is, s)) {
        // Single-char emoji
        auto code = fromHex(s);
        r.singleChar.insert(code);
    }
    return r;
}


int main()
{
    std::cout << "Have " << dictionary.size() << " words in dictionary, "
              << nExceptions() << " exceptions." << std::endl;

    ///// Loader ///////////////////////////////////////////////////////////////
    ///
    std::cout << "Checking for loader..." << std::flush;
    try {
        if (checkLoader()) {
            std::cout << "OK" << std::endl;
        } else {
            std::cout << "RE-RUN" << std::endl;
            runLoader();
        }
    } catch (const std::exception& e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        return 1;
    }

    ///// HTML entities ////////////////////////////////////////////////////////

    std::cout << "Loading HTML entities..." << std::flush;
    std::ifstream is(ENTITIES_HTML, std::ios::binary);
    if (!is.is_open())
        throw std::logic_error("Cannot open entities.htm");

    is.seekg(0, std::ios::end);
    auto fsize = is.tellg();
    is.seekg(0, std::ios::beg);

    std::string sEntities;
    sEntities.resize(fsize);
    is.read(sEntities.data(), fsize);

    std::unordered_map<char32_t, std::vector<std::string>> htmlEntities;
    auto tagTable = rqTag(sEntities, "<table", "</table");

    size_t nEntities = 0;
    while (true) {
        auto tagTr = findTagMinus(tagTable, "<tr ", {});
        if (tagTr.empty())
            break;
        auto hexCode = rqTagMinus(tagTr, "U+", " ");
        char32_t charCode = fromHex(hexCode);
        auto& second = htmlEntities[charCode];
        if (!second.empty()) {
            throw std::logic_error("Found the same char once again");
        }
        auto spanNames1 = rqTag(tagTr, "class=\"named\"", "</code>");
        auto cleanNames = rqTag(spanNames1, "<code>", "");
        std::string spanNames { cleanNames };
        str::replace(spanNames, "&amp;"sv, "&"sv);
        auto names = str::splitSv(spanNames, ' ', true);
        second.assign(names.begin(), names.end());
        nEntities += second.size();
    }

    std::cout << "OK" << std::endl;
    std::cout << "  File size is " << std::dec << fsize << ", found " << htmlEntities.size() << " chars, "
              << nEntities << " entities." << std::endl;

    ///// Noto emoji ///////////////////////////////////////////////////////////

    std::cout << "Loading Noto emoji list..." << std::flush;
    NotoData noto = loadNotoEmoji();
    std::cout << "OK" << std::endl;
    std::cout << "  Found " << noto.singleChar.size() << " single-char emoji." << std::endl;

    ///// Emoji ////////////////////////////////////////////////////////////////

    std::cout << "Loading Unicode emoji table..." << std::flush;
    auto emoji = lib::loadEmoji(EMOJI_TEST);
    std::cout << "OK" << std::endl;
    std::cout << "  Found " << emoji.vs16.size() << " VS16 emoji." << std::endl;

    lib::StrangeCjk strangeCjk;

    ///// Egyptian /////////////////////////////////////////////////////////////

    std::cout << "Loading Egyptian base..." << std::flush;
    auto egypBase = egyp::loadBase();
    std::cout << "OK" << std::endl;
    std::cout << "  Loaded " << egypBase.size() << " Egyptian hieros." << std::endl;

    ///// Library //////////////////////////////////////////////////////////////

    std::cout << "Loading manual library..." << std::flush;
    auto manualLib = lib::loadManual(LIBRARY_XML);
    std::cout << "OK" << std::endl;

    ///// Open output file /////////////////////////////////////////////////////

    std::ofstream os("UcAuto.cpp");
    os << "// Automatically generated, do not edit!" << '\n';
    os << '\n';
    os << R"(#include "UcAutoDefines.h")" << '\n';
    os << '\n';

    pugi::xml_document doc;

    std::cout << "Loading Unicode base..." << std::flush;
    doc.load_file(UCD_XML);
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
        if (sName.empty()) {
            sName = elChar.attribute("na1").as_string();
            // Remove abbreviation in brackets
            if (auto pos = sName.find(" ("sv); pos != std::string_view::npos) {
                sName = sName.substr(0, pos);
            }
        }

        std::string_view defaultAbbrev {};      // empty
        std::vector<std::string_view> allAbbrevs;
        std::vector<std::string> restAliases;
        std::vector<std::string> aliasAbbrevs;

        AbbrevState abbrevState = AbbrevState::NORMAL;
        if (auto it = abbrevs.find(cp); it != abbrevs.end()) {
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
            std::string_view newName =  elAlias.attribute("alias").as_string();
            if (sType == "alternate"sv) {
                restAliases.emplace_back(newName);
            } else if (sType == "control"sv || sType == "figment"sv) {
                if (sName.empty()) {
                    sName = newName;
                } else {
                    // Find everywhere
                    if (sName != newName
                        && std::find(restAliases.begin(), restAliases.end(), newName) == restAliases.end()) {
                        restAliases.emplace_back(newName);
                    }
                }
            } else if (sType == "correction"sv) {
                // Checked known chars, and corrections ARE BETTER than originals
                sName = newName;
            } else if (sType == "abbreviation") {
                // Abbreviations
                switch (abbrevState) {
                case AbbrevState::DISABLE: break;
                case AbbrevState::NORMAL:
                    if (newName != defaultAbbrev)      // do not dupe defaultAbbrev
                        allAbbrevs.push_back(newName);
                    break;
                case AbbrevState::ALIAS:
                    aliasAbbrevs.emplace_back(newName);
                    break;
                }
            }
        }

        if (allAbbrevs.size() > 1 && defaultAbbrev.empty()) {
            nl.trigger();
            std::cout << "WARNING: char " << std::hex << cp << " has no default abbreviation." << std::endl;
        }
        bool hasAbbrev = !allAbbrevs.empty();
        std::string sLowerName = decapitalize(sName, cp);
        auto [pTech, wasIns] = strings.remember(cp, uc::TextRole::MAIN_NAME, sLowerName);
        if (!allAbbrevs.empty() && !wasIns) {
            nl.trigger();
            std::cout << "WARNING: char " << std::hex << cp << " has an abbreviation and a repeating name." << std::endl;
        }

        for (auto& v : allAbbrevs) {
            strings.forceRemember(cp, uc::TextRole::ABBREV, std::string{v});
        }

        for (auto& v : aliasAbbrevs) {
            strings.forceRemember(cp, uc::TextRole::ALT_NAME, v);
        }

        if (auto v = egypBase.find(cp); v != egypBase.end()) {
            strings.forceRemember(cp, uc::TextRole::ALT_NAME, v->second);
        }

        for (auto& v : restAliases) {
            auto it = std::find(allAbbrevs.begin(), allAbbrevs.end(), v);
            if (it == allAbbrevs.end()) {
                sLowerName = decapitalize(v, cp);
                strings.forceRemember(cp, uc::TextRole::ALT_NAME, sLowerName);
            }
        }

        // HTML
        auto v = htmlEntities.find(cp);
        if (v != htmlEntities.end()) {
            if (!wasIns) {
                nl.trigger();
                std::cout << "WARNING: char " << std::hex << cp << " has HTML and a repeating name." << std::endl;
            }
            for (auto &w : v->second)
                strings.forceRemember(cp, uc::TextRole::HTML, w);
        }

        Flags<uc::Cfg> flags;
        if (hasAbbrev)
            flags |= uc::Cfg::M_ABBREVIATION;
        // Deprecated
        if (elChar.attribute("Dep").as_string()[0] == 'Y') {
            flags |= uc::Cfg::U_DEPRECATED;
            ++nDeprecated;
        }
        // Alternate
        if (isAlternate(cp))
            flags |= uc::Cfg::RENDER_BUG;
        // Virtual virama
        if (customDrawnControlChars.contains(cp)
                || (cp >= 0xE0000 && cp <= 0xE007F))
            flags |= uc::Cfg::M_CUSTOM_CONTROL;
        // No anti-aliasing
        if (isNoAa(cp))
            flags |= uc::Cfg::NO_AA;
        // Default-ignorable
        if (elChar.attribute("DI").as_string()[0] == 'Y') {
            flags |= uc::Cfg::U_DEF_IGNORABLE;
        }
        // VS16
        if (emoji.vs16.contains(cp))
            flags |= uc::Cfg::U_VS16_EMOJI;
        // SVG emoji
        if (noto.singleChar.contains(cp))
            flags |= uc::Cfg::M_SVG_EMOJI;
        // Draw as space
        if (charsDrawnAsSpaces.contains(cp))
            flags |= uc::Cfg::M_SPACE;
        // Draw as Egyptian hatch
        if (charsEgyptianHatch.contains(cp))
            flags |= uc::Cfg::M_EGYPTIAN_HATCH;

        // CJK strange
        if (std::string_view sStrange = elChar.attribute("kStrange").as_string();
                !sStrange.empty()) {
            strangeCjk.processCp(cp, sStrange);
        }

        // OUTPUT
        os << "{ "
           << "0x" << std::hex << cp << ", "    // subj
           << "{ "                              // name
                << std::dec << pTech->offset    // name.tech,
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
            os << "Cfgs{" << flags.numeric() << "}";
        } else {
            os << "{}";
        }

        os << "}," << '\n';
        ++nChars;

        strings.finishCp();
    }

    os << "};" << '\n';

    std::cout << "OK" << std::endl;
    std::cout << "Found " << std::dec << nChars << " chars, "
              << nDeprecated << " deprecated, "
              << nSpecialRanges << " special ranges." << std::endl;

    os << "const char8_t uc::allStrings[] = \n";
    char text[40];
    for (auto& v : strings.inOrder()) {
        snprintf(text, std::size(text), R"(u8"\x%02X\x%02X" ")",
                 static_cast<unsigned>(v.role),
                 static_cast<unsigned>(v.s.length()));
        os << text << v.s << "\"  ";
        if (v.isLast)
            os << R"("\0")";
        os << "  // " << std::hex << static_cast<int>(v.subj) << '\n';
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

    ///// Close main file //////////////////////////////////////////////////////

    os.close();

    ///// Write UcAutoLib.cpp //////////////////////////////////////////////////

    lib::Node root;
    // Node 1: emoji
    root.children.emplace_back(std::move(emoji.root));
    // Node 2+: manual library
    for (auto& v : manualLib.children) {
        root.children.emplace_back(std::move(v));
    }
    // Node N: strange hiero
    root.children.emplace_back(strangeCjk.give());
    // Write!
    auto libInfo = lib::write(root, "UcAutoLib.cpp");

    ///// Write UcAutoCount ////////////////////////////////////////////////////
    os.open("UcAutoCount.h");
    os << "#pragma once\n";
    os << '\n';
    os << "// Automatically generated, do not edit!\n";
    os << '\n';
    os << "namespace uc {\n";
    os << "constexpr int N_CPS = " << std::dec << nChars << ";\n";
    os << "constexpr int N_BLOCKS = " << std::dec << nBlocks << ";\n";
    os << "constexpr int N_NUMERICS = " << std::dec << nums.size() << ";\n";
    os << "constexpr int N_LIBNODES = " << std::dec << libInfo.nNodes << ";\n";
    os << "}\n";

    ///// Done !! //////////////////////////////////////////////////////////////

    std::cout << "Successfully finished!" << std::endl << std::endl;

    return 0;
}
