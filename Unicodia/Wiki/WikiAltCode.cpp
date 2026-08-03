#include "WikiAltCode.h"

#include "UcOldInput.h"
#include "u_Cmap.h"

namespace {

    inline myalt::ComboMode toMode(bool x)
        { return static_cast<myalt::ComboMode>(x); }

    enum class AltFilterMode : unsigned char {
        COMMON,     ///< is a common
        EXACT       ///< exactly equals to
    };
    struct AltFilter {
        AltFilterMode mode;
        unsigned value;
        bool operator () (unsigned x) const noexcept;
    };

    bool AltFilter::operator () (unsigned x) const noexcept
    {
        switch (mode) {
        case AltFilterMode::COMMON:
            return (x == 0 || x == value);
        case AltFilterMode::EXACT:
            return (x == value);
        }
        __builtin_unreachable();
    }

    inline AltFilter filCom(unsigned x) noexcept
        { return { .mode = AltFilterMode::COMMON, .value = x}; }

    inline AltFilter filExact(unsigned x) noexcept
        { return { .mode = AltFilterMode::EXACT, .value = x}; }

    class WriteLang {
    public:
        WriteLang(myalt::Printer& aPrn, AltFilter aFilter) noexcept
            : prn(aPrn), filter(aFilter) {}
        void operator () (unsigned char code, const uc::OneByteInfo& page) const;
    private:
        myalt::Printer& prn;
        AltFilter filter;
        mutable bool isFirst = true;
    };

    void WriteLang::operator () (unsigned char code, const uc::OneByteInfo& page) const
    {
        if (filter(code)) {
            prn.continueList(isFirst, page);
            isFirst = false;
        }
    }

    template <class K>
    void writeLangList(
            myalt::Printer& prn, AltFilter filter, const uc::LocBase<K>& codes)
    {
        WriteLang wl(prn, filter);
        codes.run(wl);
        prn.endList();
    }

    template <class Rng, class V>
    bool hasValue(const Rng& rng, V val)
    {
        auto end = std::end(rng);
        return (std::find(std::begin(rng), end, val) != end);
    }

}   // anon namespace


void myalt::print(const uc::AltCode& alt, Printer& prn)
{
    // Do we have Alt codes?
    if (alt == uc::InputMethods::NONE.alt)
        return;

    bool needLocale = !alt.hasLocaleIndependent();
    if (alt.dosCommon) {
        auto hasList = alt.locDos.hasOtherThan(alt.dosCommon);
        prn.startCombo(alt.dosCommon, CODE_DOS, toMode(hasList));
        if (hasList) {
            writeLangList(prn, filCom(alt.dosCommon), alt.locDos);
            // Need locale, but only when Windows' common locale is absent
            if (alt.winCommon == 0)
                needLocale = true;
        }
    }
    if (alt.winCommon) {
        prn.startCombo(alt.winCommon, CODE_WIN, ComboMode::EVERYWHERE);
    }
    if (needLocale) {
        auto runEncoding = [&prn]
            (auto& loc, char prefix, unsigned char commonCode) {
                std::vector<unsigned> alreadyUsed;
                if (commonCode != 0)
                    alreadyUsed.push_back(commonCode);
                for (auto v : loc) {
                    if (v > CMAP_LAST_TECH && !hasValue(alreadyUsed, v)) {
                        prn.startCombo(v, prefix, ComboMode::LIST);
                        writeLangList(prn, filExact(v), loc);
                        alreadyUsed.push_back(v);
                    }
                }
            };
        runEncoding(alt.locDos, CODE_DOS, alt.dosCommon);
        runEncoding(alt.locWin, CODE_WIN, 0);
    }
}
