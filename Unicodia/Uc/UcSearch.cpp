// My header
#include "UcSearch.h"

// STL
#include <unordered_set>

// Libs
#include "u_Qstrings.h"

// Unicode
#include "UcData.h"

constinit const uc::SearchLine uc::SearchLine::STUB;

const std::string_view uc::cpTypeKeys[CpType_N] {
    {},     // existing
    "Search.NonChar",
    "Search.Private",
    "Search.Surrogate",
    "Search.Unassigned",
    "Search.Empty",
    "{LibNode}",
};

const std::string_view uc::searchErrorKeys[SearchError_N] {
    {},     // OK
    {},     // no search
    "Search.NotFound",
    "Search.BadCode",
    "Search.BigCode",
};


///// MultiResult //////////////////////////////////////////////////////////////


uc::MultiResult::MultiResult(const SingleResult& x)
    : err(x.err)
{
    if (err == SearchError::OK) {
        auto& bk = v.emplace_back(x);
        bk.prio.high = HIPRIO_HEX;
    }
}

const uc::Cp* uc::MultiResult::one() const
{
    if (err == SearchError::OK && v.size() == 1 && v[0].type == CpType::EXISTING)
        return v[0].cp;
    return nullptr;
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

    /// @todo [future] Can move that trie to compile-time?
    struct TriePath;

    struct TrieNode {
        const uc::LibNode* result = nullptr;
        std::unordered_map<char32_t, TrieNode> children;
    };

    struct TriePath : public std::unordered_map<char32_t, TrieNode> {};

    TrieNode trieRoot;

    SafeVector<std::u8string_view> allSearchableNames(const uc::Cp& cp)
    {
        SafeVector<std::u8string_view> r;
        std::u8string_view it = cp.name.tech();
        r.emplace_back(it);
        cp.name.traverseAllT([&r](uc::TextRole role, std::u8string_view text) {
            switch (role) {
            case uc::TextRole::ALT_NAME:
            case uc::TextRole::HTML:
            case uc::TextRole::ABBREV:
                r.push_back(text);
                break;
            case uc::TextRole::MAIN_NAME:
            case uc::TextRole::DEP_INSTEAD:
            case uc::TextRole::DEP_INSTEAD2:
            case uc::TextRole::CMD_END:
                break;
            }
        });
        return r;
    }

    SafeVector<std::u8string_view> allHtmlNames(const uc::Cp& cp)
    {
        SafeVector<std::u8string_view> r;
        cp.name.traverseAllT([&r](uc::TextRole role, std::u8string_view text) {
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
            std::basic_string_view name { cp.name.tech() };
            return (!has(name, u8"ideograph"));
        } else {
            return false;
        }
    }

    bool isLatUpper(QChar x) { return (x >= 'A' && x <= 'Z'); }

}   // anon namespace


void uc::ensureEmojiSearch()
{
    if (hasEmojiSearch)
        return;

    for (auto& node : allLibNodes()) {
        // Build trie
        if (node.flags.have(uc::Lfg::DECODEABLE)) {
            auto* p = &trieRoot;
            for (auto c : node.value) {
                p = &p->children[c];
            }
            p->result = &node;
        }
    }
    hasEmojiSearch = true;
}


SafeVector<uc::DecodedEmoji> uc::decodeEmoji(std::u32string_view s)
{
    ensureEmojiSearch();

    struct Last {
        const LibNode* result = nullptr;
        size_t iLastPos = 0;
    } lastKnown;

    SafeVector<uc::DecodedEmoji> r;
    const TrieNode* p = &trieRoot;

#define REGISTER_RESULT \
    r.emplace_back( \
            lastKnown.iLastPos + 1 - lastKnown.result->value.length(), \
            lastKnown.result);
    // Why +1? We do not search for single-char emoji, but if…
    //   iLastPos == 0, length == 1 → how to make 0 out of them?

    for (size_t index = 0; index < s.length(); ++index) {
        char32_t c = s[index];
        auto itChild = p->children.find(c);
        if (itChild == p->children.end()) {
            // Dead end. Found smth?
            if (lastKnown.result) {
                REGISTER_RESULT
                // I do not want to make true Aho-Corasick here! Just back down.
                index = lastKnown.iLastPos;
            }
            // Anyway move to root
            p = &trieRoot;
            lastKnown.result = nullptr;
        } else {
            p = &itChild->second;
            if (p->result) {
                lastKnown.result = p->result;
                lastKnown.iLastPos = index;
            }
        }
    }
    if (lastKnown.result) {
        REGISTER_RESULT
    }
    return r;

#undef REGISTER_RESULT
}


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
        if (what.length() == 2) {
            auto upCase = what.toUpper();
            if (upCase.length() == 2    // Should have, for reliability
                    && isLatUpper(upCase[0]) && isLatUpper(upCase[1])) {
                ensureEmojiSearch();
                static constexpr auto OFS_FLAGA = cp::FLAG_A - 'A';
                const auto cp1 = char32_t(upCase[0].unicode()) + OFS_FLAGA;
                auto where1 = trieRoot.children.find(cp1);
                if (where1 != trieRoot.children.end()) {
                    const auto cp2 = char32_t(upCase[1].unicode()) + OFS_FLAGA;
                    auto where2 = where1->second.children.find(cp2);
                    if (where2 != trieRoot.children.end() && where2->second.result) {
                        // At last found
                        auto& bk = r.emplace_back(where2->second.result);
                        bk.prio.high = uc::HIPRIO_FLAG;
                        bk.giveTriggerName(str::toU8(upCase));
                    }
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
                QString q = what;
                q.replace(',', '.');
                bool isOk;
                auto val = q.toDouble(&isOk);
                if (isOk) {
                    // Decimal fraction
                    // Need two digits after point; three and more → stop
                    int denom = 100;
                    auto val1 = val * denom;
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
        for (auto& cp : uc::cpInfo) {
            if (&cp != hex && &cp != dec) {  // Do not check what we found once again
                // Numeric search
                if (numerics.contains(cp.iNumeric)) {
                    auto& bk = r.emplace_back(cp);
                    bk.prio.high = isHiprioNumber(cp)
                            ? HIPRIO_NUMERIC_HI : HIPRIO_NUMERIC;
                } else {
                    // Textual search
                    auto names = allSearchableNames(cp);
                    struct {
                        srh::Prio prio;
                        std::u8string_view name;
                    } best;
                    auto& cat = cp.category();
                    auto block = blockOf(cp.subj);
                    bool isNonScript =
                            (cp.ecCategory != EcCategory::SYMBOL_MODIFIER
                            && cat.upCat != UpCategory::LETTER
                            && cat.upCat != UpCategory::MARK
                            && !block->flags.have(Bfg::SCRIPTLIKE)
                            && cp.script().flags.have(Sfg::NONSCRIPT));
                    for (auto& nm : names) {
                        if (nm.starts_with('&')) {
                            // Search by HTML mnemonic
                            if (nm.size() == sv.size() + 2) {
                                auto mnemo = nm.substr(1, sv.size());
                                if (sv == mnemo) {
                                    auto& bk = r.emplace_back(cp, nm);
                                    bk.prio.high = HIPRIO_MNEMONIC_EXACT;
                                    goto brk;
                                } else if (srh::stringsCiEq(sv, mnemo)) {
                                    auto& bk = r.emplace_back(cp, nm);
                                    bk.prio.high = HIPRIO_MNEMONIC_CASE;
                                    goto brk;
                                }
                            }
                        } if (nm.find('#') == std::u8string_view::npos) {
                            // Search by keyword
                            if (auto pr = srh::findNeedle(nm, needle, isNonScript);
                                    pr > best.prio) {
                                best.prio = pr;
                                best.name = nm;
                            }
                        }
                    }
                    if (best.prio > srh::Prio::EMPTY) {
                        if (best.name == names[0])
                            best.name = std::u8string();
                        r.emplace_back(cp, best.name, best.prio);
                    }
                }
            brk:;
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
                auto& bk = r.emplace_back(pEmoji->node);
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
