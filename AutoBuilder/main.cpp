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
#include "mojibake.h"

// Unicode
#include "UcCp.h"

// Project-local
#include "ucdcom.h"
#include "data.h"
#include "egyptian.h"
#include "entities.h"
#include "legacy.h"
#include "library.h"
#include "loader.h"
#include "sutton.h"
#include "textbase.h"
#include "unibase.h"

using namespace std::string_view_literals;

template <class T>
inline auto need(T&& val, const char* errmsg)
{
    if (!val) throw std::logic_error(errmsg);
    return std::forward<T>(val);
}

struct Numeric {
    long long num, denom;
    long long altInt = 0;
    std::string_view type;
    std::string textValue;
    size_t index;
};

long long fromChars(std::string_view x, std::string_view numType)
{
    long long r;
    auto beg = x.data();
    auto end = beg + x.length();
    auto res = std::from_chars(beg, end, r);
    if (res.ec != std::errc() || res.ptr != end) {
        std::string errm = str::cat(
            "[fromChars] Cannot parse number ", x, ", numType = ", numType);
        throw std::invalid_argument(errm);
    }
    return r;
}

Numeric parseNumeric(std::string_view numType, std::string_view x, size_t index)
{
    if (x.empty()) {
        return {
            .num = 0,
            .denom = 0,
                    .altInt = 0,
            .type = numType,
            .textValue{},
            .index = index };
    }

    long long altNum = 0;
    auto pAlt = x.find(' ');
    if (pAlt != std::string_view::npos) {
        auto sAlt = str::trimSv(x.substr(pAlt + 1));
        altNum = fromChars(sAlt, numType);
        x = x.substr(0, pAlt);
    }

    auto pSlash = x.find('/');
    if (pSlash == std::string_view::npos) {
        // Integer
        return {
            .num = fromChars(x, numType),
            .denom = 1,
            .altInt = altNum,
            .type = numType,
            .textValue = std::string{x},
            .index = index };
    } else {
        // Fraction
        auto num = x.substr(0, pSlash);
        auto den = x.substr(pSlash + 1);
        return {
            .num = fromChars(num, numType),
            .denom = fromChars(den, numType),
            .altInt = altNum,
            .type = numType,
            .textValue = std::string{x},
            .index = index };
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
    std::string key = str::cat(numType, x);
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
    auto& nonAscii() const { return fNonAscii; }
private:
    using M = std::unordered_map<std::string_view, StringData*>;
    M fNdx;
    std::deque<StringData> fInOrder;
    size_t fLength = 0;
    std::set<char32_t> fNonAscii;
};

RememberResult StringLib::forceRemember(
        char32_t subj, uc::TextRole role, std::string_view aS)
{
    auto func = [this](char32_t c) {
        if (c >= 128)
            fNonAscii.insert(c);
    };
    mojibake::copyS(aS.begin(), aS.end(),
            mojibake::Utf32CallIterator(func));
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


namespace {
    tx::Cp DUMMY_CP;

    bool containsWordSeq(const std::string_view haystack, const std::string_view needle)
    {
        if (haystack.length() < needle.length())
            return false;
        if (haystack == needle)
            return true;

        std::string h1 = lat::toUpper(haystack);
        std::string n1 = lat::toUpper(needle);
        str::replace(h1, '-', ' ');
        str::replace(n1, '-', ' ');
        if (h1.length() == n1.length())
            return (h1 == n1);

        auto hwords = str::splitSv(h1, ' ');
        auto nwords = str::splitSv(n1, ' ');
        if (hwords.size() <= nwords.size())
            return false;
        auto beg = hwords.begin();
        auto end = hwords.end();
        for (auto& vn : nwords) {
            auto whereFound = std::find(beg, end, vn);
            if (whereFound == end)
                return false;
            beg = whereFound;
            ++beg;
        }
        return true;
    }

}   // anon namespace


std::string encodeC(std::string_view x)
{
    std::string r;
    r.reserve(x.size());
    for (auto c : x) {
        switch (c) {
        case '"':
            r += R"(\")";
            break;
        case '\\':
            r += R"(\\)";
            break;
        default:
            r += c;
            break;
        }
    }
    return r;
}


int main()
{
    std::cout << "Have " << dictionary.size() << " words in dictionary, "
              << nExceptions() << " exceptions." << '\n';

    ///// Loader ///////////////////////////////////////////////////////////////
    ///
    std::cout << "Checking for loader..." << std::flush;
    try {
        checkLoader();
        std::cout << "OK" << '\n';
    } catch (const std::exception& e) {
        std::cout << "ERROR: " << e.what() << '\n';
        return 1;
    }

    ///// HTML entities ////////////////////////////////////////////////////////

    std::cout << "Loading HTML entities..." << std::flush;
    auto htmlEntities = readEntities(ENTITIES_JSON);
    std::cout << "OK, " << htmlEntities.data.size() << " chars, "
                        << htmlEntities.stats.entities.nGood << " good en's, "
                        << htmlEntities.stats.entities.nTooLong << " too long (2+) en's, "
                        << htmlEntities.stats.entities.nWrong << " wrong en's." << std::endl;

    ///// Property list ////////////////////////////////////////////////////////

    std::cout << "Loading Unicode property list..." << std::flush;
    const ucd::PropBase propBase = ucd::loadPropBase();
    std::cout << "OK, " << propBase.nScripts() << " scripts." << '\n';

    ///// Script data //////////////////////////////////////////////////////////

    std::cout << "Loading Unicode script list..." << std::flush;
    const tx::Scripts scripts = tx::loadScripts(propBase);
    std::cout << "OK, " << scripts.size() << " lines." << '\n';

    ///// Age data /////////////////////////////////////////////////////////////

    std::cout << "Loading Unicode ages..." << std::flush;
    const tx::Ages ages = tx::loadAges();
    std::cout << "OK, " << ages.size() << " lines." << '\n';

    ///// Derived props ////////////////////////////////////////////////////////

    std::cout << "Loading Unicode props..." << std::flush;
    const tx::Props props = tx::loadProps();
    std::cout << "OK, " << props.defaultIgnorable.size() << " DI lines, "
                        << props.deprecated.size() << " DEP lines." << '\n';

    ///// Emoji ////////////////////////////////////////////////////////////////

    std::cout << "Loading Unicode emoji table..." << std::flush;
    auto emoji = lib::loadEmoji(EMOJI_TEST);
    std::cout << "OK, " << emoji.count << " emoji, " << emoji.vs16.size() << " are VS16." << '\n';

    lib::StrangeCjk strangeCjk;

    ///// Egyptian /////////////////////////////////////////////////////////////

    std::cout << "Loading Egyptian base..." << std::flush;
    auto egypBase = egyp::loadBase();
    std::cout << "OK, " << egypBase.size() << " hieros." << '\n';

    ///// Library //////////////////////////////////////////////////////////////

    std::cout << "Loading manual library..." << std::flush;
    auto manualLib = lib::loadManual(LIBRARY_XML, emoji.all);
    std::cout << "OK, " << manualLib.forgetMap.size() << " CPs in forget checker." << '\n';

    ///// Text base ////////////////////////////////////////////////////////////

    std::cout << "Loading Unicode text base..." << std::flush;
    auto textBase = tx::loadBase();
    std::cout << "OK, " << textBase.size() << " text objects." << '\n';

    ///// SupportData //////////////////////////////////////////////////////////

    std::cout << "Loading main base's support data..." << std::flush;
    const auto supportData = ucd::loadSupportData();
    std::cout << "OK, " << supportData.nBlocks << " blocks, "
                        << supportData.hangulLines.size() << " Hangul lines, "
                        << supportData.hanKangxi.size() << " Kangxi lines." << '\n';

    ///// Open output file /////////////////////////////////////////////////////

    std::ofstream os("UcAuto.cpp");
    os << "// Automatically generated, do not edit!" << '\n';
    os << '\n';
    os << R"(#include "UcAutoDefines.h")" << '\n';
    os << '\n';

    ///// Main base ////////////////////////////////////////////////////////////

    StringLib strings;
    NumCache nums;
    int nDeprecated = 0, nUpCase = 0, nChars = 0;
    NewLine nl;
    std::cout << "Processing main base..." << std::flush;

    os << '\n';
    os << R"(uc::Cp uc::cpInfo[N_CPS] {)" << '\n';

    ucd::processMainBaseT(supportData, [&](const ucd::CpInfo& cpInfo) {
        auto cp = cpInfo.cp;
        std::string_view sName = cpInfo.name;

        std::string_view defaultAbbrev {};      // empty

        AbbrevState abbrevState = AbbrevState::NORMAL;
        if (auto it = abbrevs.find(cp); it != abbrevs.end()) {
            if (it->second.empty()) {
                abbrevState = AbbrevState::DISABLE;
            } else {
                if (it->second == "!"sv) {
                    abbrevState = AbbrevState::ALIAS;
                } else {
                    defaultAbbrev = it->second;
                }
            }
        }

        // Find other names in text base
        auto itCp = textBase.find(cp);
        tx::Cp* textCp = &DUMMY_CP;
        bool hasTextEntry = false;
        if (itCp != textBase.end()) {
            textCp = &itCp->second;
            hasTextEntry = true;
        }

        // Get corrected name
        std::string sLowerName;
        if (!textCp->correction.empty()) {
            sLowerName = std::move(textCp->correction);
        } else {
            sLowerName = decapitalize(sName, cp);
        }

        if (hasTextEntry) {
            if (sLowerName.empty() && !textCp->controls.empty()) {
                sLowerName = std::move(textCp->controls[0]);
                textCp->controls.erase(textCp->controls.begin());
            }

            if (abbrevState == AbbrevState::DISABLE)
                textCp->abbrs.clear();

            // Name
            textCp->eraseName(sLowerName);

            textCp->eraseName(defaultAbbrev);
            textCp->eraseName(decapitalize(defaultAbbrev, cp));
        }

        bool hasAbbrev = !defaultAbbrev.empty() || !textCp->abbrs.empty();
        auto [pTech, wasIns] = strings.remember(cp, uc::TextRole::MAIN_NAME, sLowerName);
        if (hasAbbrev && !wasIns) {
            nl.trigger();
            std::cout << "WARNING: char " << std::hex << int(cp) << " has an abbreviation and a repeating name." << '\n';
        }

        auto abbrevRole = (abbrevState == AbbrevState::NORMAL)
                          ? uc::TextRole::ABBREV : uc::TextRole::ALT_NAME;
        if (!defaultAbbrev.empty()) {
            strings.forceRemember(cp, abbrevRole, defaultAbbrev);
        }

        for (auto& v : textCp->abbrs) {
            strings.forceRemember(cp, abbrevRole, std::string{v});
        }

        for (auto& v : textCp->controls) {
            if (v == sLowerName)
                continue;
            strings.forceRemember(cp, uc::TextRole::ALT_NAME, v);
        }

        if (auto v = egypBase.find(cp); v != egypBase.end()) {
            strings.forceRemember(cp, uc::TextRole::ALT_NAME, v->second);
        }

        for (auto& v : textCp->names) {
            strings.forceRemember(cp, uc::TextRole::ALT_NAME, v);
        }

        // HTML
        auto v = htmlEntities.data.find(cp);
        if (v != htmlEntities.data.end()) {
            if (!wasIns) {
                nl.trigger();
                std::cout << "WARNING: char " << std::hex << int(cp) << " has HTML and a repeating name." << '\n';
            }
            for (auto &w : v->second)
                strings.forceRemember(cp, uc::TextRole::HTML, w);
        }

        // Deprecated
        bool isDeprecated = props.deprecated.contains(cp);
        if (isDeprecated) {
            auto itDep = deprecatedInfo.find(cp);
            if (itDep != deprecatedInfo.end()) {
                auto& q = itDep->second;
                if (!q.whatsInstead.empty()) {
                    strings.forceRemember(cp, uc::TextRole::DEP_INSTEAD, str::toSv(q.whatsInstead));
                    if (!q.whatsInstead2.empty()) {
                        strings.forceRemember(cp, uc::TextRole::DEP_INSTEAD2, str::toSv(q.whatsInstead2));
                    }
                }
            }
        }

        Flags<uc::Cfg> flags;
        if (hasAbbrev && abbrevState == AbbrevState::NORMAL)
            flags |= uc::m::ABBREVIATION;
        // Deprecated
        if (isDeprecated) {
            flags |= uc::Cfg::U_DEPRECATED;
            ++nDeprecated;
        }
        // Alternate
        if (isAlternate(cp))
            flags |= uc::Cfg::G_RENDER_BUG;
        // Tags
        if (cp >= ::cp::TAG_MIN && cp <= ::cp::TAG_MAX)
                    flags |= uc::m::CUSTOM_CONTROL;
        // No anti-aliasing
        if (isNoAa(cp))
            flags |= uc::m::NO_AA;
        // Default-ignorable
        if (props.defaultIgnorable.contains(cp)) {
            flags |= uc::Cfg::U_DEF_IGNORABLE;
        }
        // VS16
        if (emoji.vs16.contains(cp)) {
            flags |= uc::Cfg::U_VS16_EMOJI;
        }
        // Misrenders
        if (emoji.misrenders.contains(cp))
            flags |= uc::Cfg::G_MISRENDER;
        // SVG emoji
        if (auto q = emoji.allSingleChar.find(cp); q != emoji.allSingleChar.end()) {
            flags |= uc::m::SVG_EMOJI;
            if (auto p = q->second) {    // May have NULL here
                auto name1 = str::toSv(p->name);
                if (!containsWordSeq(sLowerName, name1) && !textCp->contains(name1)) {
                    strings.forceRemember(cp, uc::TextRole::EMOJI_NAME, name1);
                }
            }
        }
        // Special drawing
        if (auto q = drawMethods.find(cp); q != drawMethods.end())
            flags |= q->second;
        flags |= styleFlags(cp);

        // OUTPUT
        os << "{ "
           << "0x" << std::hex << int(cp) << ", "    // subj
           << "{ "                              // name
                << std::dec << pTech->offset    // name.tech,
           << " }, ";                           // /name

        // Char’s type
        os << "EcCategory::" << transform(cpInfo.generalCat, smCharCat) << ", ";

        // Char’s version
        auto& sVersion = ages.findRq(cp);
        os << "EcVersion::V_" << transformVersion(sVersion) << ", ";

        // Char’s bidirectional data
        std::string_view sBidiClass = cpInfo.bidiCat;
            // Check whether have bracket
        /// @todo [urgent] Can check column 9 instead of a separate file?
        if (cpInfo.isMirrored) {
            if (sBidiClass != "ON"sv)
                throw std::logic_error("Got strange bidi class for mirrored char");
            sBidiClass = "MIR"sv;
        }
        os << "EcBidiClass::z_" << sBidiClass << ", ";

        auto sScript = scripts.findDefRef(cp, "Zyyy"sv);
        // Char’s script
            // Hiragana → Hentaigata
        if (sScript == "Hira"sv && sLowerName.starts_with("Hentaigana"))
            sScript = "Hent"sv;
        os << "EcScript::" << sScript << ", ";

        if (cpInfo.upperCase != 0) {
            ++nUpCase;
        }
        forget::processCp(manualLib.forgetMap, cp, sLowerName, sScript, cpInfo.upperCase);

        // Char’s numeric values
        // nt = …
        //    • None — no numeric value
        //    • De — decimal digit
        //    • Di — special digit
        //    • Nu — number
        // nv = Nan / whole number / vulgar fraction
        auto& numPlace = nums.parse(cpInfo.numeric.type->id, cpInfo.numeric.value);
        os << numPlace.index << ", ";

        if (flags) {
            os << "Cfgs{" << flags.numeric() << "}";
        } else {
            os << "{}";
        }

        os << ", {" << cpInfo.kx.radical << "," << cpInfo.kx.plusStrokes << "}";

        os << "}," << '\n';
        ++nChars;

        strings.finishCp();
    });

    os << "};" << '\n';

    std::cout << "OK" << '\n';
    std::cout << "  Found " << std::dec << nChars << " chars, "
              << nUpCase << " to-up-case, "
              << nDeprecated << " deprecated." << '\n';

    os << "const char8_t uc::allStrings[] = \n";
    char text[40];
    for (auto& v : strings.inOrder()) {
        snprintf(text, std::size(text), R"(u8"\x%02X\x%02X" ")",
                 static_cast<unsigned>(v.role),
                 static_cast<unsigned>(v.s.length()));
        os << text << encodeC(v.s) << R"("  )";
        if (v.isLast)
            os << R"("\0")";
        os << "  // " << std::hex << static_cast<int>(v.subj) << '\n';
    }
    os << ";\n";

    ///// Numerics /////////////////////////////////////////////////////////////

    std::cout << "  Stockpiled " << nums.size() << " numerics." << '\n';
    /// @todo [future] We are dangerously close to limit, what to do?
    if (nums.size() >= 255) {
        throw std::logic_error("Too many numerics, invent another way!");
    }

    os << "const uc::Numeric uc::allNumerics[uc::N_NUMERICS] { \n";
    for (const auto& v : nums.ord) {
        std::string_view shownText = v.textValue.empty() ? "NaN"sv : v.textValue;
        os << "{ " << std::dec << v.num << ", " << v.denom << ", " << v.altInt
           << ", EcNumType::" << v.type
           << " },  // " << v.index << " is " << shownText << '\n';
    }
    os << "};\n";

    ///// Close main file //////////////////////////////////////////////////////

    os.close();

    ///// Han dictionary ///////////////////////////////////////////////////////

    std::cout << "Processing Han dictionary..." << std::flush;
    { std::ifstream is(HAN_DIC);
        std::string line;
        while (std::getline(is, line)) {
            auto trimmed = str::trimSv(line);
            if (trimmed.empty() || trimmed.starts_with('#'))
                continue;
            auto vals = str::splitSv(line, '\t');
            if (vals.size() < 3)
                continue;
            auto sCp = vals.at(0);
            if (sCp.starts_with("U+"sv))
                sCp = sCp.substr(2);
            auto cp = fromHex(sCp);
            auto prop = vals.at(1);
            auto value = vals.at(2);
            if (prop == "kStrange") {
                strangeCjk.processCp(cp, value);
            }
        }
    }
    std::cout << "OK" << '\n';

    ///// Write UcAutoLib.cpp //////////////////////////////////////////////////

    std::cout << "Saving library..." << std::flush;
    lib::Node root;
    // Node 1: emoji
    root.children.emplace_back(new lib::Node(std::move(emoji.root)));
    // Node 2+: manual library
    for (auto& v : manualLib.root.children) {
        root.children.emplace_back(std::move(v));
    }
    // Node N: strange hiero
    root.children.emplace_back(new lib::Node(strangeCjk.give()));
    // Write!
    auto libr = lib::write(root, "UcAutoLib.cpp");
    auto longest = root.maxValueLength();
    std::cout << "OK, " << libr.nNodes << " nodes, longest is " << longest << '.' << '\n';

    ///// Old computers ////////////////////////////////////////////////////////

    std::cout << "Processing old computer base..." << std::flush;
    auto oldr = old::process();
    std::cout << "OK, " << oldr.nCps << " CPs, "
                        << oldr.nSpans << " spans." "\n";

    ///// Write UcAutoCount ////////////////////////////////////////////////////

    os.open("UcAutoCount.h");
    os << "#pragma once\n";
    os << '\n';
    os << "// Automatically generated, do not edit!\n";
    os << '\n';
    os << "namespace uc {\n";
    os << "constexpr int N_CPS = " << std::dec << nChars << ";\n";
    os << "constexpr int N_BLOCKS = " << std::dec << supportData.nBlocks << ";\n";
    os << "constexpr int N_NUMERICS = " << std::dec << nums.size() << ";\n";
    os << "constexpr int N_EMOJI = " << std::dec << emoji.count << ";\n";
    os << "constexpr unsigned LONGEST_LIB = " << std::dec << longest << ";  // in codepoints" "\n";
    os << "constexpr unsigned N_OLDCOMP_SPANS = " << std::dec << oldr.nSpans << ";\n";
    os << "}\n";
    os.close();

    ///// Write UcAutoScripts //////////////////////////////////////////////////

    auto sToL = propBase.buildShortToLong();
    sToL.erase("Zyyy");  // Written manually
    sToL.erase("Zzzz");  // Unused
    sToL.erase("Zinh");  // Written manually
    sToL.erase("Hrkt");  // Hiragana-Katakana, actually unused
    sToL.emplace("Hent", "Hentaigana");

    os.open("UcAutoScripts.h");
    os << "// Automatically generated, do not edit!" "\n";
    for (auto& [sh, lo] : sToL) {
        std::string spLo = lo;
        str::replace(spLo, '_', ' ');
        os << sh << ",  // " << spLo << '\n';
    }
    os.close();

    ///// Sutton SignWriting ///////////////////////////////////////////////////

    std::cout << "Processing Sutton base..." << std::flush;
    auto swr = sw::process();
    std::cout << "OK, " << swr.nLines << " lines, first inequal "
              << std::hex << static_cast<uint32_t>(swr.firstInequal) << '\n';

    ///// Forgotten CPs ////////////////////////////////////////////////////////

    static constinit const char* FNAME_FORGET = "forget.log";
    auto forgetStats = forget::postprocess(manualLib.forgetMap, FNAME_FORGET);
    std::cout << "Forgotten: " << std::dec
              << forgetStats.nRepeat << " repeating, "
              << forgetStats.nMissing << " missing, "
              << forgetStats.nExtra << " extra, "
              << forgetStats.nBadCase << " bad case, see " << FNAME_FORGET << "." << '\n';

    ///// Temp files ///////////////////////////////////////////////////////////

    std::cout << "Removing temp files..." << std::flush;
    cleanUpLoader();
    std::cout << "OK" "\n";

    ///// Done !! //////////////////////////////////////////////////////////////

    std::cout << "Successfully finished!" << "\n\n";

    return 0;
}
