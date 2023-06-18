#pragma once

// C++
#include <cmath>

// STL
#include <string>
#include <vector>

namespace loc {

    enum class Plural { ZERO, ONE, TWO, FEW, MANY, OTHER };
    constexpr unsigned Plural_N = static_cast<unsigned>(Plural::OTHER) + 1;

    class Locale    // interface
    {
    public:
        virtual Plural uintQtyRule(unsigned long long n) const = 0;
        virtual Plural intQtyRule(long long n) const { return uintQtyRule(std::abs(n)); }
        virtual char unitSpaceC() const { return ' '; }
        virtual char32_t unitSpaceL() const { return U' '; }
        virtual ~Locale() = default;
    };

    class DefaultLocale : public Locale
    {
    public:
        Plural uintQtyRule(unsigned long long n) const override;
        static const DefaultLocale INST;
    };

    ///  Substitution, debugging only
    struct Zsubst {
        static constexpr size_t NO_LINK = std::numeric_limits<size_t>::max();
        size_t key;       ///< 0-based: {1} = 0
        size_t advance;   ///< distance from END of prev.substitution:
                          ///< x{1}xxx{2}xx → advance[0]=1, [1]=3
        size_t length;    ///< length of substitution
        size_t lnkNext;   ///< not a pointer → so object is copyable!

        bool isExpired() const noexcept { return (advance == NO_LINK); }
        bool isLast() const noexcept { return (lnkNext == NO_LINK || advance == NO_LINK); }
    };

    template <class Ch>
    class Fmt
    {
    public:
        using Str = std::basic_string<Ch>;
        using Sv = std::basic_string_view<Ch>;
        Fmt(Str&& x);
        Fmt(Sv x);
        Fmt(const Ch* x);
        Fmt(const Locale& lc, Str&& x);
        Fmt(const Locale& lc, Sv x);
        Fmt(const Locale& lc, const Ch* x);

        const Str& str() const& noexcept { return d; }
        Str&& str() && noexcept { return std::move(d); }
        const Ch* c_str() const noexcept { return d.c_str(); }

        /// @return  # of total substitutions
        size_t nAllSubsts() const noexcept { return substs.size(); }
        /// @return  i’th substitution
        const Zsubst& allSubsts(size_t i) const { return substs.at(i); }
        size_t iFirstSubst() const noexcept { return lnkFirst; }
        size_t nextKey() const noexcept { return fNextKey; }
    private:
        const Locale& loc;
        Str d;

        std::vector<Zsubst> substs;
        size_t lnkFirst;
        size_t fNextKey;

        void init();
    };

    template <class Ch>
    class FmtL : public Fmt<Ch>
    {
    public:
    };

    // Deduction guides
    template <class Ch>
    Fmt(const loc::Locale&, std::basic_string_view<Ch>) -> Fmt<Ch>;

    template <class Ch>
    Fmt(std::basic_string_view<Ch>) -> Fmt<Ch>;

    template <class Ch>
    Fmt(const loc::Locale&, const Ch*) -> Fmt<Ch>;

    template <class Ch>
    Fmt(const Ch*) -> Fmt<Ch>;

}   // namespace loc

extern template class loc::Fmt<char>;
extern template class loc::Fmt<wchar_t>;
extern template class loc::Fmt<char8_t>;
extern template class loc::FmtL<char>;
extern template class loc::FmtL<wchar_t>;
extern template class loc::FmtL<char8_t>;

template <class Ch>
loc::Fmt<Ch>::Fmt(Str&& x) : loc(DefaultLocale::INST), d(x) { init(); }

template <class Ch>
loc::Fmt<Ch>::Fmt(Sv x) : loc(DefaultLocale::INST), d(x) { init(); }

template <class Ch>
loc::Fmt<Ch>::Fmt(const Ch* x) : loc(DefaultLocale::INST), d(x) { init(); }

template <class Ch>
loc::Fmt<Ch>::Fmt(const Locale& lc, Str&& x) : loc(lc), d(x) { init(); }

template <class Ch>
loc::Fmt<Ch>::Fmt(const Locale& lc, Sv x) : loc(lc), d(x) { init(); }

template <class Ch>
loc::Fmt<Ch>::Fmt(const Locale& lc, const Ch* x) : loc(lc), d(x) { init(); }

template <class Ch>
void loc::Fmt<Ch>::init()
{
    size_t lastSubstPos = 0;
    size_t outputPos = 0;
    size_t nextKey = 0;

    size_t inputPos = 0;
    auto copyTo = [this, &inputPos, &outputPos](size_t p) {
        size_t nChars = p - inputPos;
        if (inputPos != outputPos)
            std::copy(d.data() + inputPos, d.data() + p, d.data() + outputPos);
        outputPos += nChars;
        inputPos = p;
    };

    while (true) {
        auto p = d.find('{', inputPos);
        if (p == Str::npos)
            break;
        if (p + 1 >= d.length())
            break;
        if (d[p + 1] == '{') {
            // {{ = escaped {
            ++p;
            copyTo(p);
            inputPos = ++p;
        } else {
            // Normal substitution
            copyTo(p);
            size_t p1 = p + 1;
            for (; p1 < d.length(); ++p1) {
                switch (d[p1]) {
                case '{':   // escape
                    ++p1; break;
                case '}':
                    goto wasFound;
                default: ;
                }
                // break; goes here
            }
            break;  // breaks loop — incomplete substitution
        wasFound:
            // broke using goto brk1: found
            // Find key
            size_t p2 = p + 1;
            uint32_t key = 0;
            for (; p2 < p1; ++p2) {
                auto c = d[p2];
                switch (c) {
                case '|':
                    goto brk2;
                default:
                    if (c >= '0' && c <= '9') {
                        unsigned digit = c - '0';
                        key = key * 10u + digit;
                        if (key % 10u != digit) { // overflow: base is never multiple of 10
                            key = 0;
                            goto brk2;
                        }
                    } else {    // Bad character
                        key = 0;
                        goto brk2;
                    }
                }

            }
        brk2:
            if (key == 0) {
                key = nextKey;
            } else {
                --key;
            }
            auto oldOutPos = outputPos;

            nextKey = key + 1;
            auto& nextSub = substs.emplace_back();
            nextSub.key = key;
            nextSub.advance = outputPos - lastSubstPos;
            copyTo(p1 + 1);
            nextSub.length = outputPos - oldOutPos;
            nextSub.lnkNext = substs.size();
            lastSubstPos = outputPos;
        }
    }

    copyTo(d.length());
    d.resize(outputPos);

    if (substs.empty()) {
        lnkFirst = Zsubst::NO_LINK;
    } else {
        lnkFirst = 0;
        substs.back().lnkNext = Zsubst::NO_LINK;
    }
    fNextKey = 0;
}   // init()
