// My header
#include "UcSearch.h"

// STL
#include <unordered_set>

// Libs
#include "u_Strings.h"

// Unicode
#include "UcData.h"


constinit const uc::SearchLine uc::SearchLine::STUB {
    0, SingleError::ONE, &cpInfo[0], {}, {}
};


const uc::ErrorInfo uc::errorInfo[SingleError_N] {
    { IsCp::YES, {} },     // one
    { IsCp::NO,  {} },     // multiple
    { IsCp::NO,  {} },     // no search
    { IsCp::NO,  u8"Не найдено" },
    { IsCp::NO,  u8"Код символа не распознан" },
    { IsCp::NO,  u8"Код символа слишком велик" },
    { IsCp::YES, u8"Выброшенная позиция" },
    { IsCp::YES, u8"Символ для личного пользования" },
    { IsCp::YES, u8"Код из суррогатной пары" },
    { IsCp::YES, u8"Нераспределённый символ" },
    { IsCp::YES, u8"Свободное место" },
};


uc::SingleSearchResult uc::findCode(char32_t code)
{
    // Too big?
    if (code >= uc::N_CHARS)
        return { code, SingleError::TOO_BIG };

    // Have that character?
    auto pCp = uc::cpsByCode[code];
    if (pCp)
        return { code, SingleError::ONE, pCp };

    // If not → find what the heck
    if (isNonChar(code))
        return { code, SingleError::NONCHARACTER };
    if ((code >= 0xE000 && code <= 0xF8FF) || code >= 0xF'0000)
        return { code, SingleError::PRIVATE_USE };
    if (code >= 0xD800 && code <= 0xDFFF)
        return { code, SingleError::SURROGATE };
    auto v = uc::blockOf(code, 0);
    if (code > v->endingCp)
        return { code, SingleError::UNALLOCATED };

    return { code, SingleError::RESERVED, v->firstAllocated  };
}


uc::SingleSearchResult uc::findStrCode(QStringView what, int base)
{
    uint code = 0;
    bool isOk = false;
    if (code = what.toUInt(&isOk, base); isOk) {
        return findCode(code);
    } else {
        return { 0, SingleError::CONVERT_ERROR };
    }
}


bool uc::isNameChar(char32_t cp)
{
    return (cp >= 'A' && cp <= 'Z')
        || (cp >= 'a' && cp <= 'z')
        || (cp >= '0' && cp <= '9')
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


std::u8string uc::toMnemo(QString x)
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

    uc::SearchResult toMultiple(const uc::SingleSearchResult& x)
    {
        using enum uc::SingleError;
        // Found one, or search has no CP
        if (x.err == ONE || !uc::errorInfo[static_cast<int>(x.err)].isCp())
            return {x};

        // Turn single to multiple
        uc::SearchResult r;
        r.err = MULTIPLE;
        r.multiple.emplace_back(x.singleCode, x.err, x.one);
        return r;
    }

}   // anon namespace


uc::SearchResult uc::doSearch(QString what)
{
    if (what.isEmpty())
        return {{ 0, SingleError::NO_SEARCH }};

    // Find a single character, maybe space
    if (what.size() <= 2) {
        auto u32 = what.toUcs4();
        if (u32.size() == 1) {
            auto code = u32[0];
            if (code == ' ' || !isNameChar(code)) {
                auto codeResult = uc::findCode(code);
                return toMultiple(codeResult);
            }
        }
    }

    what = what.trimmed();

    // Searching for nothing?
    if (what.isEmpty())
        return {{ 0, SingleError::NO_SEARCH }};

    if (what.startsWith("U+", Qt::CaseInsensitive)) {
        // U+:
        auto sHex = QStringView(what).mid(2);
        auto codeResult = uc::findStrCode(sHex, 16);
        return toMultiple(codeResult);
    }

    SafeVector<uc::SearchLine> r;

    if (auto mnemo = toMnemo(what); !mnemo.empty()) {
        // SEARCH BY HTML MNEMONIC
        // Search
        for (auto& cp : uc::cpInfo) {
            auto names = cp.allRawNames();
            for (auto& nm : names) {
                if (srh::stringsCiEq(mnemo, nm)) {
                    r.emplace_back(cp.subj, uc::SingleError::ONE, &cp);
                    break;
                }
            }
        }
        if (r.size() == 1)
            return {{ r[0].cp->subj, SingleError::ONE, r[0].cp }};
    } else if (isNameChar(what)) {
        // Try find hex
        const uc::Cp* hex = nullptr;
        if (what.size() >= 2) {
            if (auto q = uc::findStrCode(what, 16); q.err == SingleError::ONE) {
                hex = q.one;
                auto& bk = r.emplace_back(q.singleCode, uc::SingleError::ONE, q.one);
                bk.prio.high = uc::HIPRIO_HEX;
            }
        }

        // Try find dec
        const uc::Cp* dec = nullptr;
        if (what.size() >= 2) {
            if (auto q = uc::findStrCode(what, 10);
                    q.err == SingleError::ONE
                    && q.one->subj.val() >= 10) {       // if you find 08 → do not dupe
                dec = q.one;
                auto& bk = r.emplace_back(q.singleCode, uc::SingleError::ONE, q.one);
                bk.prio.high = uc::HIPRIO_DEC;
            }
        }

        // SEARCH BY KEYWORD/mnemonic
        const uc::Block* block = &uc::blocks[0];
        auto u8Name = what.toStdString();
        auto sv = toU8(u8Name);
        srh::Needle needle(sv);
        for (auto& cp : uc::cpInfo) {
            if (&cp != hex && &cp != dec) {   // Do not check hex/dec once again
                auto names = cp.allRawNames();
                struct {
                    srh::Prio prio;
                    std::u8string_view name;
                } best;
                auto& cat = cp.category();
                block = blockOf(cp.subj, block);
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
                                auto& bk = r.emplace_back(cp.subj, uc::SingleError::ONE, &cp, nm);
                                bk.prio.high = HIPRIO_MNEMONIC_EXACT;
                                goto brk;
                            } else if (srh::stringsCiEq(sv, mnemo)) {
                                auto& bk = r.emplace_back(cp.subj, uc::SingleError::ONE, &cp, nm);
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
                    r.emplace_back(cp.subj, uc::SingleError::ONE, &cp, best.name, best.prio);
                }
            brk:;
            }
        }

        if (r.size() == 1 && r[0].prio.high >= HIPRIO_FIRST_ONE)
            return {{ r[0].cp->subj, SingleError::ONE, r[0].cp }};

        // Sort by relevance
        std::stable_sort(r.begin(), r.end());
    } else {
        // DEBRIEF STRING
        auto u32 = what.toUcs4();
        for (auto v : u32) {
            auto find = uc::findCode(v);
            if (errorInfo[static_cast<int>(find.err)].isCp()) {
                r.emplace_back(find.singleCode, find.err, find.one);
            }
        }
    }

    if (r.empty()) {
        return {{ 0, SingleError::NOT_FOUND }};
    } else {
        return r;
    }
}
