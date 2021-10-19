// My header
#include "UcSearch.h"

// STL
#include <unordered_set>

// Unicode
#include "UcData.h"


std::u8string_view uc::errorStrings[uc::SingleError_N] {
    {},     // one
    {},     // multiple
    {},     // no search
    u8"Не найдено",
    u8"Код символа не распознан",
    u8"Код символа слишком велик",
    u8"Код зарезервирован как отсутствующий",
    u8"Символ для личного пользования",
    u8"Код из суррогатной пары",
    u8"Нераспределённый символ",
    u8"Свободное место",
};


uc::SingleSearchResult uc::findCode(char32_t code)
{
    // Too big?
    if (code >= uc::N_CHARS)
        return { SingleError::TOO_BIG };

    // Have that character?
    auto pCp = uc::cps[code];
    if (pCp)
        return { SingleError::ONE, pCp };

    // If not → find what the heck
    if (isNonChar(code))
        return { SingleError::NONCHARACTER };
    if ((code >= 0xE000 && code <= 0xF8FF) || code >= 0xF'0000)
        return { SingleError::PRIVATE_USE };
    if (code >= 0xD800 && code <= 0xDFFF)
        return { SingleError::SURROGATE };
    auto v = uc::blockOf(code, 0);
    if (code > v->endingCp)
        return { SingleError::UNALLOCATED };

    return { SingleError::RESERVED, v->firstAllocated  };
}


uc::SingleSearchResult uc::findStrCode(QStringView what, int base)
{
    uint code = 0;
    bool isOk = false;
    if (code = what.toUInt(&isOk, base); isOk) {
        return findCode(code);
    } else {
        return { SingleError::CONVERT_ERROR };
    }
}


bool uc::isNameChar(char32_t cp)
{
    return (cp >= 'A' && cp <= 'Z')
        || (cp >= 'a' && cp <= 'z')
        || (cp >= '0' && cp <= '9')
        || (cp == '-');
}


bool uc::isNameChar(QStringView x)
{
    for (auto v : x) {
        if (!isNameChar(v.unicode()))
            return false;
    }
    return true;
}


namespace {

    //constexpr uint8_t TOTAL_FRACTION = 200;

    struct ResultPrio {
        uint8_t exactFraction = 0,
                initialFraction = 0,
                partialFraction = 0;
        std::strong_ordering operator <=>(const ResultPrio& x) const = default;
    };

    struct ResultEx {
        const uc::Cp* subj = nullptr;
        ResultPrio prio;
    };

}


uc::SearchResult uc::doSearch(QString what)
{
    if (what.isEmpty())
        return {{ SingleError::NO_SEARCH }};

    // Find a single character, maybe space
    if (what.size() <= 2) {
        auto u32 = what.toUcs4();
        if (u32.size() == 1) {
            auto code = u32[0];
            if (!isNameChar(code)) {
                return uc::findCode(code);
            }
        }
    }

    what = what.trimmed();

    // Searching for nothing?
    if (what.isEmpty())
        return {{ SingleError::NO_SEARCH }};

    if (what.startsWith("U+", Qt::CaseInsensitive)) {
        // U+:
        auto sHex = QStringView(what).mid(2);
        return uc::findStrCode(sHex, 16);
    }

    SafeVector<const uc::Cp*> r;

    if (isNameChar(what)) {
        //std::unordered_set<const uc::Cp*> ndx;
        auto u8Name = what.toStdString();
        std::u8string_view sv(reinterpret_cast<const char8_t*>(u8Name.data()), u8Name.length());
        for (auto& cp : uc::cpInfo) {
            auto names = cp.allRawNames();
            for (auto& nm : names) {
                if (nm.find('#') == std::u8string_view::npos) {
                    if (nm.find(sv) != std::u8string_view::npos) {
                        r.emplace_back(&cp);
                        goto found;
                    }
                }
            }
            found:;
        }
    } else {
        auto u32 = what.toUcs4();
        for (auto v : u32) {
            auto find = uc::findCode(v);
            if (find.one)
                r.emplace_back(find.one);
        }
    }

    if (r.empty()) {
        return {{ SingleError::NOT_FOUND }};
    } else {
        return r;
    }
}
