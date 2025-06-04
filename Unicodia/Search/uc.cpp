// My header
#include "uc.h"

// STL
#include <unordered_set>

// Libs
#include "u_Strings.h"

// Unicode
#include "UcCp.h"
#include "UcData.h"

// Emoji painter
#include "CharPaint/emoji.h"

// Search
#include "nonAscii.h"
#include "trie.h"

using namespace std::string_view_literals;

constinit const uc::SearchLine uc::SearchLine::STUB;

constexpr const std::string_view EMPTY_SV {};

constinit const ec::Array<std::string_view, uc::CpType> uc::cpTypeKeys {
    EMPTY_SV,     // existing
    "Search.NonChar",
    "Search.Private",
    "Search.Surrogate",
    "Search.Unassigned",
    "Search.Empty",
    "{LibNode}",
};

constinit const ec::Array<std::string_view, uc::SearchError> uc::searchErrorKeys {
    EMPTY_SV,  // OK
    EMPTY_SV,  // no search
    "Search.NotFound",
    "Search.BadCode",
    "Search.BigCode",
};


///// MultiResult //////////////////////////////////////////////////////////////


uc::MultiResult::MultiResult(const SingleResult& x)
    : err(x.err)
{
    if (err == SearchError::OK) {
        auto& bk = groups.emplace_back().lines.emplace_back(x);
        bk.prio.high = HIPRIO_HEX;
    }
}


uc::MultiResult::MultiResult(SafeVector<SearchLine>&& aV)
    : err(aV.empty() ? SearchError::NOT_FOUND : SearchError::OK)
{
    if (err == SearchError::OK) {
        groups.emplace_back(std::move(aV));
    }
}


const uc::Cp* uc::MultiResult::one() const
{
    if (err == SearchError::OK && style == ReplyStyle::FLAT && groups.size() == 1) {
        auto& lines = groups[0].lines;
        if (lines.size() == 1 && lines[0].type == CpType::EXISTING) {
            return lines[0].cp;
        }
    }
    return nullptr;
}


bool uc::MultiResult::isEmpty() const
{
    switch (groups.size()) {
    case 0 : return true;
    case 1 : return groups[0].isEmpty();
    default : return false;
    }
}


///// Search functions /////////////////////////////////////////////////////////


uc::SingleResult uc::findCode(unsigned long long ull)
{
    // Too big?
    if (ull >= uc::CAPACITY)
        return { SearchError::TOO_BIG };

    char32_t code = ull;

    // Have that character?
    auto pCp = uc::cpsByCode[code];
    if (pCp)
        return { *pCp };

    // If not → find what the heck
    if (isNonChar(code))
        return { code, CpType::NONCHARACTER };
    if ((code >= cp::PRIV_BMP_FIRST && code <= cp::PRIV_BMP_LAST) || code >= cp::PLANE_F_START)
        return { code, CpType::PRIVATE_USE };
    if (code >= 0xD800 && code <= 0xDFFF)
        return { code, CpType::SURROGATE };
    auto blk = uc::blockOf(code);
    if (!blk || code > blk->endingCp)
        return { code, CpType::UNALLOCATED };

    // Find nearest char
    const uc::Cp* nearestCp =
            (code <= *blk->firstAllocated)
                ? blk->firstAllocated
                : (code >= *blk->lastAllocated)
                  ? blk->lastAllocated
                  // If we have { 1, 3 } and finding 2, l_b and u_b will return [3..3)
                  // Thus −1, as we need 1
                  // We cannot find 1− and 3+, these are covered by branches above
                  : std::lower_bound(blk->firstAllocated, blk->lastAllocated, code) - 1;
    return { code, CpType::RESERVED, nearestCp };
}


uc::SingleResult uc::findStrCode(QStringView what, int base, long long& code)
{
    bool isOk = false;
    if (code = what.toLongLong(&isOk, base); isOk) {
        return findCode(code);
    } else {
        code = NO_CODE;
        return { SearchError::CONVERT_ERROR };
    }
}


namespace {

    inline bool isAlnum(char32_t cp)
    {
        return (cp >= 'A' && cp <= 'Z')
            || (cp >= 'a' && cp <= 'z')
            || (cp >= '0' && cp <= '9');
    }

}


bool uc::isNameChar(char32_t cp)
{
    switch (cp) {
    case '/':   // Not really a name char, but need for search by fraction
    case '.':   // Same
    case ',':   // Same
    case '-':
    case '\'':  // Lots of apostrophes in Unicode
    case ' ':
        return true;
    default:
        return isAlnum(cp);
    }
}


bool uc::isNameChar(QStringView x)
{
    // Special for 1-char
    if (x.length() == 1) {
        return (isAlnum(x[0].unicode()));
    }
    for (auto v : x) {
        if (!isNameChar(v.unicode()))
            return false;
    }
    return true;
}


bool uc::isMnemoChar(char32_t cp) { return isAlnum(cp); }


bool uc::isMnemoChar(QStringView x)
{
    for (auto v : x) {
        if (!isMnemoChar(v.unicode()))
            return false;
    }
    return true;
}


namespace {

    inline std::u8string_view toU8(std::string& x)
    {
        return { reinterpret_cast<const char8_t*>(x.data()), x.length() };
    }

}   // anon namespace


std::u8string uc::toMnemo(const QString& x)
{
    // Check for &
    if (!x.startsWith('&'))
        return {};
    // Virtually chop &
    auto sv = QStringView(x).sliced(1);
    // Virtually chop ;
    if (sv.endsWith(';'))
        sv.chop(1);
    // Result is bad/empty?
    if (sv.empty() || !isMnemoChar(sv))
        return {};
    // Build string: & + sv + ;
    std::u8string s;
    s.reserve(sv.length() + 2);
    s += '&';
    for (auto v : sv)
        s += v.unicode();
    s += ';';
    return s;
}


namespace {

    bool hasEmojiSearch = false;

    /// @todo [future] Can move this set to compile-time?
    std::unordered_map<char32_t, const uc::LibNode*> singleChars;

    using MyNode = srh::TrieNode<const uc::LibNode*>;
    using MyRoot = srh::TrieRoot<const uc::LibNode*>;
    MyRoot trieRoot;

    struct SearchableName {
        const std::u8string_view value;
        const srh::Comparator& comparator;
        const uc::TextRole role;
    };

    void allSearchableNamesTo(const uc::Cp& cp, SafeVector<SearchableName>& r)
    {
        r.clear();
        cp.traverseTextsT(uc::AutoName::NO,  // Auto names are NOT searchable
            [&r](uc::TextRole role, std::u8string_view text) {
                switch (role) {
                case uc::TextRole::MAIN_NAME:
                case uc::TextRole::HTML:
                case uc::TextRole::ABBREV:
                    r.emplace_back(text, srh::DefaultComparator::INST, role);
                    break;
                case uc::TextRole::ALT_NAME:
                case uc::TextRole::EMOJI_NAME:
                case uc::TextRole::EGYP_EWP:
                case uc::TextRole::EGYP_UC:
                    r.emplace_back(text, srh::NonAsciiComparator::INST, role);
                    break;
                case uc::TextRole::DEP_INSTEAD:  // They are not searchable
                case uc::TextRole::DEP_INSTEAD2:
                case uc::TextRole::EGYP_EQUIV:
                case uc::TextRole::CMD_END:
                    break;
                }
            });
    }

    SafeVector<std::u8string_view> allHtmlNames(const uc::Cp& cp)
    {
        SafeVector<std::u8string_view> r;
        cp.traverseTextsT(uc::AutoName::NO,  // Main name is not here
            [&r](uc::TextRole role, std::u8string_view text) {
                switch (role) {
                case uc::TextRole::HTML:
                    r.push_back(text);
                    break;
                case uc::TextRole::ALT_NAME:
                case uc::TextRole::ABBREV:
                case uc::TextRole::MAIN_NAME:
                case uc::TextRole::DEP_INSTEAD:
                case uc::TextRole::DEP_INSTEAD2:
                case uc::TextRole::CMD_END:
                case uc::TextRole::EMOJI_NAME:
                case uc::TextRole::EGYP_EWP:
                case uc::TextRole::EGYP_UC:
                case uc::TextRole::EGYP_EQUIV:
                    break;
                }
            });
        return r;
    }

    std::unordered_set<unsigned char> findNumerics(
            long long aNum, long long aDenom)
    {
        std::unordered_set<unsigned char> r;
        long long altInt = std::numeric_limits<long long>::min();   // never occurs
        if (aDenom == 1 && aNum != 0) {
            altInt = aNum;
        }
        for (int i = 0; i < uc::N_NUMERICS; ++i) {
            auto& num = uc::allNumerics[i];
            if ((num.num == aNum && num.denom == aDenom)
                    || (num.altInt == altInt)) {
                r.insert(i);
            }
        }
        return r;
    }

    bool has(std::u8string_view haystack, std::u8string_view needle)
    {
        return (haystack.find(needle) != std::u8string_view::npos);
    }

    bool isHiprioNumber(const uc::Cp& cp)
    {
        if (cp.block().flags.have(uc::Bfg::HIPRIO_NUMBERS)) {
            std::basic_string_view name { cp.techName() };
            return (!has(name, u8"ideograph"));
        } else {
            return false;
        }
    }

    bool isFlagKeywordOf2(std::string_view x)
    {
        return (x == "of"sv);
    }

    bool isFlagKeywordOfOther(std::string_view x)
    {
        return (x == "flag"sv);
    }

    /// Detects country flag in “flag of ua” etc
    std::string detectFlag(const QString& x)
    {
        // Check for bad chars
        // Before space, after 7F, digits
        for (auto c : x) {
            if (c < ' ' || c >= QChar(0x7F) ||
                    (c >= '0' && c <= '9'))
                return {};
        }

        // Get naked string
        std::string naked;
        naked.reserve(x.size());
        for (auto c : x) {
            /// @todo [future] localized names: флаг, прапор, bandera…
            ///    thus case-folded
            auto cc = static_cast<char>(c.toCaseFolded().unicode());
            if (lat::isLower(cc)) {
                naked += cc;
            } else {
                naked += ' ';
            }
        }

        // Split and check
        auto words = str::splitSv(naked, ' ', true);
        if (words.size() == 0 || words.size() > 3)
            return {};

        std::string_view goodWord {};
        enum class State { UNKNOWN, DUBIOUS, KNOWN };
        State state = State::UNKNOWN;
        for (auto word : words) {
            if (word.length() == 2 && lat::isLower(word)) {
                switch (state) {
                case State::UNKNOWN:
                case State::DUBIOUS:
                    goodWord = word;
                    state = isFlagKeywordOf2(word) ? State::DUBIOUS : State::KNOWN;
                    break;
                case State::KNOWN:
                    if (isFlagKeywordOf2(word)) {
                        // do nothing
                    } else {
                        // Cannot use two countries
                        return {};
                    }
                }
            } else {
                if (!isFlagKeywordOfOther(word))
                    return {};
            }
        }

        if (goodWord.empty())
            return {};

        return lat::toUpper(goodWord);
    }

}   // anon namespace


void uc::ensureEmojiSearch()
{
    if (hasEmojiSearch)
        return;

    for (auto& node : allLibNodes()) {
        if (!node.value.empty() && node.flags.have(Lfg::GRAPHIC_EMOJI)) {
            // Build trie
            if (node.flags.have(Lfg::DECODEABLE)) {
                trieRoot.add(node.value, &node);
            }
            if (auto q = EmojiPainter::getCp(node.value)) {
                singleChars[q.cp] = &node;
            }
        }
    }
    hasEmojiSearch = true;
}


SafeVector<uc::DecodedEmoji> uc::decodeEmoji(std::u32string_view s)
{
    ensureEmojiSearch();
    return trieRoot.decode(s);
}

namespace {

    std::optional<double> myToDouble(const QString& what)
    {
        QString q = what;
        q.replace(',', '.');
        bool isOk;
        double r = q.toDouble(&isOk);
        if (!isOk) {
            return std::nullopt;
        }
        int rint = r;
        if (r == rint                       // resulting number is integer
                && q.contains('e', Qt::CaseInsensitive)) {  // exponential way
            // So, we wrote integer in an exponential way
            if (std::abs(r) < 10'000    // less than 10k
                    || QString::number(rint).length() < what.length()) { // not-really-economic way to write
                return std::nullopt;
            }
        }
        return r;
    }

    srh::RoleType roleType(uc::TextRole role)
    {
        switch (role) {
        case uc::TextRole::HTML:        // HTML unused (processed separately), but let it be
        case uc::TextRole::ABBREV:
            return srh::RoleType::LACONIC;
        case uc::TextRole::ALT_NAME:
        case uc::TextRole::EMOJI_NAME:
        case uc::TextRole::MAIN_NAME:
            return srh::RoleType::BRIEF;
        case uc::TextRole::EGYP_EWP:
        case uc::TextRole::EGYP_UC:
            return srh::RoleType::VERBOSE;
        case uc::TextRole::CMD_END:     // These are unused, but let them be
        case uc::TextRole::DEP_INSTEAD:
        case uc::TextRole::DEP_INSTEAD2:
        case uc::TextRole::EGYP_EQUIV:
            return srh::RoleType::UNSEARCHABLE;
        }
        __builtin_unreachable();
    }

}   // anon namespace


uc::MultiResult uc::doSearch(QString what)
{
    if (what.isEmpty())
        return { SearchError::NO_SEARCH };

    // Find a single character, maybe space
    if (what.size() <= 2) {
        auto u32 = what.toStdU32String();
        if (u32.size() == 1) {
            auto code = u32[0];
            if (code == ' ' || !isNameChar(code)) {
                auto codeResult = uc::findCode(code);
                return codeResult;
            }
        }
    }

    what = what.trimmed();

    // Searching for nothing?
    if (what.isEmpty())
        return { SearchError::NO_SEARCH };

    long long code;
    if (what.startsWith("U+", Qt::CaseInsensitive)) {
        // U+:
        auto sHex = QStringView(what).mid(2);
        auto codeResult = uc::findStrCode(sHex, 16, code);
        return codeResult;
    }

    SafeVector<uc::SearchLine> r;

    if (auto mnemo = toMnemo(what); !mnemo.empty()) {
        // SEARCH BY HTML MNEMONIC
        // Search
        for (auto& cp : uc::cpInfo) {
            auto names = allHtmlNames(cp);
            for (auto& nm : names) {
                if (mnemo == nm) {
                    auto& v = r.emplace_back(cp);
                    v.prio.high = uc::HIPRIO_MNEMONIC_EXACT;
                    v.triggerName = nm;
                    break;
                } if (srh::stringsCiEq(mnemo, nm)) {
                    auto& v = r.emplace_back(cp);
                    v.prio.high = uc::HIPRIO_MNEMONIC_CASE;
                    v.triggerName = nm;
                    break;
                }
            }
        }

        // Sort by relevance
        std::stable_sort(r.begin(), r.end());

        return r;
    } else if (isNameChar(what)) {
        const bool isLongEnoughNumber = (what.size() >= 2);

        // Try find hex
        const uc::Cp* hex = nullptr;
        if (isLongEnoughNumber) {
            if (auto q = uc::findStrCode(what, 16, code);
                    q.err == SearchError::OK) {
                auto& bk = r.emplace_back(q);
                bk.prio.high = uc::HIPRIO_HEX;
                hex = q.cp;
            }
        }

        // Try find dec
        const uc::Cp* dec = nullptr;
        if (auto q = uc::findStrCode(what, 10, code);
                q.err == SearchError::OK && q.code >= 10 && isLongEnoughNumber) {       // if you find 08 → do not dupe
            auto& bk = r.emplace_back(q);
            bk.prio.high = uc::HIPRIO_DEC;
            dec = q.cp;
        }

        // Find flag
        if (auto flagName = detectFlag(what); flagName.length() == 2) {
            ensureEmojiSearch();
            static constexpr auto OFS_FLAGA = cp::FLAG_A - 'A';
            const auto cp1 = char32_t(flagName[0]) + OFS_FLAGA;
            if (auto where1 = trieRoot.unsafeFind(cp1)) {
                const auto cp2 = char32_t(flagName[1]) + OFS_FLAGA;
                auto where2 = where1->find(cp2);
                if (where2 && where2->result()) {
                    // At last found
                    auto& bk = r.emplace_back(where2->result());
                    bk.prio.high = uc::HIPRIO_FLAG;
                    bk.giveTriggerName(str::toU8(flagName));
                }
            }
        }

        // Find number
        std::unordered_set<unsigned char> numerics;
        if (code != NO_CODE) {
            // Integer
            numerics = findNumerics(code, 1);
        } else {
            auto things = what.split('/');
            if (things.size() == 2) {
                // Vulgar fraction
                bool isOk1, isOk2;
                auto num   = things[0].trimmed().toInt(&isOk1);
                auto denom = things[1].trimmed().toInt(&isOk2);
                if (isOk1 && isOk2) {
                    if (num == 0 && denom == 3) {
                        // Zero thirds — special case (used in baseball, numeric value is 0)
                        r.emplace_back(*cpsByCode[cp::ZERO_THIRDS]);
                        return r;
                    }
                    if (denom >= 1 && num != 0) {
                        auto absNum = std::abs(num);
                        auto reduceValue = std::gcd(absNum, denom);
                        num /= reduceValue;
                        denom /= reduceValue;
                        numerics = findNumerics(num, denom);
                    }
                }
            } else {
                if (auto val = myToDouble(what)) {
                    // Decimal fraction
                    // Need two digits after point; three and more → stop
                    int denom = 100;
                    auto val1 = (*val) * denom;
                    auto val2 = std::round(val1);
                    if (std::abs(val1 - val2) < 1e-5) {
                        int num = val2;
                        if (num != 0) {
                            auto reduceValue = std::gcd(num, denom);
                            num /= reduceValue;
                            denom /= reduceValue;
                            numerics = findNumerics(num, denom);
                        }
                    }
                }
            }
        }

        // SEARCH BY KEYWORD/mnemonic
        auto u8Name = what.toStdString();
        auto sv = toU8(u8Name);
        srh::Needle needle(sv);
        srh::Cache cache;
        SafeVector<SearchableName> names;
        for (auto& cp : uc::cpInfo) {
            if (&cp != hex && &cp != dec) {  // Do not check what we found once again
                // Numeric search
                if (numerics.contains(cp.iNumeric)) {
                    auto& bk = r.emplace_back(cp);
                    bk.prio.high = isHiprioNumber(cp)
                            ? HIPRIO_NUMERIC_HI : HIPRIO_NUMERIC;
                } else {
                    // Textual search
                    allSearchableNamesTo(cp, names);
                    struct {
                        srh::Prio prio {};
                        std::u8string_view name {};
                        uc::TextRole role = uc::TextRole::CMD_END;
                    } best;
                    auto& cat = cp.category();
                    auto& block = cp.block();
                    auto& script = cp.script();
                    bool isScript =
                            (cp.ecCategory == EcCategory::SYMBOL_MODIFIER   // Some chosen symbol types
                            || cat.upCat == EcUpCategory::LETTER
                            || cat.upCat == EcUpCategory::MARK
                            || block.flags.have(Bfg::SCRIPTLIKE)            // …or char in script-like block
                            || !script.flags.have(Sfg::NONSCRIPT));    // …or char has script (nonscripts are NONE and pseudo-scripts)
                    Flags hclasses = isScript ? srh::HaystackClass::SCRIPT : srh::HaystackClass::NONSCRIPT;
                    if (block.synthIcon.ecContinent == EcContinent::CJK
                            || script.containsIdeograph()) {
                        hclasses |= srh::HaystackClass::IDEOGRAPH;
                    }
                    if (!block.flags.have(Bfg::COOL_WORDS_1))
                        hclasses |= srh::HaystackClass::EXCEPT_COOL_1;
                    if (!(block.flags.have(Bfg::COOL_WORDS_2) || cp.flags.have(Cfg::S_COOL_2)))
                        hclasses |= srh::HaystackClass::EXCEPT_COOL_2;
                    for (auto& nm : names) {
                        switch (nm.role) {
                        case uc::TextRole::HTML:
                            if (nm.value.size() == sv.size() + 2) {
                                auto mnemo = nm.value.substr(1, sv.size());
                                if (sv == mnemo) {
                                    auto& bk = r.emplace_back(cp, nm.value);
                                    bk.prio.high = HIPRIO_MNEMONIC_EXACT;
                                    goto brk;
                                } else if (srh::stringsCiEq(sv, mnemo)) {
                                    auto& bk = r.emplace_back(cp, nm.value);
                                    bk.prio.high = HIPRIO_MNEMONIC_CASE;
                                    goto brk;
                                }
                            }
                            break;
                        default:
                            if (auto pr = srh::findNeedle(
                                        nm.value, needle, hclasses,
                                        roleType(nm.role), cache, nm.comparator);
                                    pr > best.prio) {
                                best.prio = pr;
                                best.name = nm.value;
                                best.role = nm.role;
                            }
                        }
                    }
                    if (best.prio > srh::Prio::EMPTY) {
                        // Main name → do not show it
                        if (best.role == uc::TextRole::MAIN_NAME)
                            best.name = u8""sv;
                        r.emplace_back(cp, best.name, best.prio);
                    }
                }
            brk:;
            }
        }

        // Same search, but emoji
        for (auto& node: uc::allLibNodes()) {
            if (!node.flags.have(uc::Lfg::SEARCHABLE))
                continue;
            auto prio = srh::findNeedle(
                    node.text, needle, srh::HaystackClass::MASK_EMOJI,
                    // All emoji are brief
                    srh::RoleType::BRIEF, cache,
                    srh::NonAsciiComparator::INST);
            if (prio > srh::Prio::EMPTY) {
                r.emplace_back(&node, prio);
            }
        }

        // Sort by relevance
        std::stable_sort(r.begin(), r.end());
    } else {
        // DEBRIEF STRING
        auto u32 = what.toStdU32String();

        auto emojiList = decodeEmoji(u32);
        auto pEmoji = emojiList.begin();
        size_t iLevel1 = 0;

        for (size_t i32 = 0; i32 < u32.length(); ++i32) {
            // Insert emoji
            if (pEmoji != emojiList.end() && pEmoji->index == i32) {
                auto& bk = r.emplace_back(pEmoji->result);
                bk.prio.high = HIPRIO_HEX;
                iLevel1 = i32 + bk.node->value.length();
                ++pEmoji;
            }
            auto c = u32[i32];
            auto find = uc::findCode(c);
            if (find.err == SearchError::OK) {
                auto& bk = r.emplace_back(find);
                bk.prio.high = HIPRIO_HEX;
                bk.nestLevel = static_cast<int>(i32 < iLevel1);
            }
        }
    }

    return r;
}


const uc::LibNode* uc::findEmoji(char32_t x)
{
    ensureEmojiSearch();
    if (auto it = singleChars.find(x); it != singleChars.end())
        return it->second;
    return nullptr;
}
