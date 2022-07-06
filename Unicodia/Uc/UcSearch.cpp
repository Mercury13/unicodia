// My header
#include "UcSearch.h"

// STL
#include <unordered_set>

// Libs
#include "u_Strings.h"

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
    if ((code >= 0xE000 && code <= 0xF8FF) || code >= 0xF'0000)
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


bool uc::isNameChar(char32_t cp)
{
    return (cp >= 'A' && cp <= 'Z')
        || (cp >= 'a' && cp <= 'z')
        || (cp >= '0' && cp <= '9')
        || (cp == '/')   // Not really a name char, but need for search by fraction
        || (cp == '-')
        || (cp == ' ');
}


bool uc::isNameChar(QStringView x)
{
    for (auto v : x) {
        if (!isNameChar(v.unicode()))
            return false;
    }
    return true;
}


bool uc::isMnemoChar(char32_t cp)
{
    return (cp >= 'A' && cp <= 'Z')
        || (cp >= 'a' && cp <= 'z')
        || (cp >= '0' && cp <= '9');
}


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

    std::unordered_set<unsigned char> findNumerics(
            long long aNum, long long aDenom)
    {
        std::unordered_set<unsigned char> r;
        for (int i = 0; i < uc::N_NUMERICS; ++i) {
            auto& num = uc::allNumerics[i];
            if (num.num == aNum && num.denom == aDenom) {
                r.insert(i);
            }
        }
        return r;
    }

}   // anon namespace


uc::MultiResult uc::doSearch(QString what)
{
    if (what.isEmpty())
        return { SearchError::NO_SEARCH };

    // Find a single character, maybe space
    if (what.size() <= 2) {
        auto u32 = what.toUcs4();
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
            auto names = cp.allRawNames();
            for (auto& nm : names) {
                if (mnemo == nm) {
                    auto& v = r.emplace_back(cp);
                    v.prio.high = uc::HIPRIO_MNEMONIC_EXACT;
                    break;
                } if (srh::stringsCiEq(mnemo, nm)) {
                    auto& v = r.emplace_back(cp);
                    v.prio.high = uc::HIPRIO_MNEMONIC_CASE;
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

        // Find integer
        std::unordered_set<unsigned char> numerics;
        if (code != NO_CODE) {
            numerics = findNumerics(code, 1);
        } else {
            auto things = what.split('/');
            if (things.size() == 2) {
                bool isOk1, isOk2;
                auto num   = things[0].trimmed().toInt(&isOk1);
                auto denom = things[1].trimmed().toInt(&isOk2);
                if (isOk1 && isOk2) {
                    if (num == 0 && denom == 3) {
                        // Zero thirds — special case (used in baseball, numeric value is 0)
                        r.emplace_back(*cpsByCode[0x2189]);
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
                    bk.prio.high = HIPRIO_NUMERIC;
                } else {
                    // Textual search
                    auto names = cp.allRawNames();
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
        auto u32 = what.toUcs4();
        for (auto v : u32) {
            auto find = uc::findCode(v);
            if (find.err == SearchError::OK) {
                auto& bk = r.emplace_back(find);
                bk.prio.high = HIPRIO_HEX;
            }
        }
    }

    return r;
}
