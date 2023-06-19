#pragma once

// C++
#include <cmath>
#include <charconv>

// STL
#include <string>
#include <vector>

namespace loc {

    enum class Plural { ZERO, ONE, TWO, FEW, MANY, OTHER };
    constexpr unsigned Plural_N = static_cast<unsigned>(Plural::OTHER) + 1;

    class PluralRule {   // interface
    public:
        virtual Plural ofUint(unsigned long long n) const = 0;
        virtual Plural ofInt(long long n) const { return ofUint(std::abs(n)); }
    };

    ///  Default rule for English cardinal plurals
    ///  1 lap (one), 2+ laps (many)
    ///  Dubbed as default rule for basic plural
    ///
    class DefaultQtyRule : public PluralRule
    {
    public:
        Plural ofUint(unsigned long long n) const override;
        static const DefaultQtyRule INST;
    };

    class Locale {  // interface
    public:
        virtual const PluralRule& qtyRule() const = 0;
        virtual char unitSpaceC() const { return ' '; }
        virtual char32_t unitSpaceL() const { return U' '; }
        virtual ~Locale() = default;
    };

    class DefaultLocale : public Locale
    {
    public:
        const PluralRule& qtyRule() const override;
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

    class Zchecker {  // interface
    public:
        virtual Plural check(const PluralRule& plu) const = 0;
        virtual ~Zchecker() = default;
    };

    class ZsgnChecker : public Zchecker {
    public:
        explicit ZsgnChecker(long long x) noexcept : v(x) {}
        Plural check(const PluralRule& plu) const override;
    private:
        long long v;
    };

    class ZunsChecker : public Zchecker {
    public:
        explicit ZunsChecker(unsigned long long x) noexcept : v(x) {}
        Plural check(const PluralRule& plu) const override;
    private:
        unsigned long long v;
    };

    template <class Ch>
    class Fmt
    {
    public:
        using Str = std::basic_string<Ch>;
        using Sv = std::basic_string_view<Ch>;
        Fmt(Str x);
        Fmt(Sv x);
        Fmt(const Ch* x);
        Fmt(const Locale& lc, Str x);
        Fmt(const Locale& lc, Sv x);
        Fmt(const Locale& lc, const Ch* x);

        const Str& str() const& noexcept { return d; }
        Str&& str() && noexcept { return std::move(d); }
        const Ch* c_str() const noexcept { return d.c_str(); }

        /// @warning  Debug only
        /// @return  # of total substitutions
        size_t nAllSubsts() const noexcept { return substs.size(); }
        /// @warning  debug only
        /// @return  i’th substitution
        const Zsubst& allSubsts(size_t i) const { return substs.at(i); }
        /// @warning  debug only
        /// @return  index of 1st working substitution, or Zsubst::NO_LINK
        size_t iFirstSubst() const noexcept { return lnkFirst; }
        size_t nextKey() const noexcept { return fNextKey; }

        Fmt& n(signed char x)        { nn(static_cast<int>(x));          return *this; }
        Fmt& n(unsigned char x)      { nn(static_cast<unsigned int>(x)); return *this; }
        Fmt& n(short x)              { nn(static_cast<int>(x));          return *this; }
        Fmt& n(unsigned short x)     { nn(static_cast<unsigned int>(x)); return *this; }
        Fmt& n(int x)                { nn(x);                            return *this; }
        Fmt& n(unsigned int x)       { nn(x);                            return *this; }
        Fmt& n(long x)               { nn(x);                            return *this; }
        Fmt& n(unsigned long x)      { nn(x);                            return *this; }
        Fmt& n(long long x)          { nn(x);                            return *this; }
        Fmt& n(unsigned long long x) { nn(x);                            return *this; }
    protected:
        void nn(int x);
        void nn(unsigned int x);
        void nn(long x);
        void nn(unsigned long x);
        void nn(long long x);
        void nn(unsigned long long x);
        ///  Why such an architecture?
        ///  There will be three modes:
        ///  • cardinal (5 laps)
        ///  • ordinal (5th lap)
        ///  • basic plural (the laps)
        ///  Currently cardinal only. All modes are stored in substitutions.
        ///  Everything is parsed on substituting, just for simplicity.
        ///  We see ordinal substitution → ask ordinal plural rule
        void nnn(std::string_view x, const Zchecker& chk);
    private:
        const Locale& loc;
        Str d;

        std::vector<Zsubst> substs;
        size_t lnkFirst;
        size_t fNextKey;

        static constexpr size_t NO_LINK = Zsubst::NO_LINK;

        void init();
    };

    extern const loc::Locale* activeLocale;

    template <class Ch>
    class FmtL : public Fmt<Ch>
    {
    private:
        using Super = Fmt<Ch>;
    public:
        using typename Super::Str;
        using typename Super::Sv;
        FmtL(Str x) : Super(*activeLocale, std::move(x)) {}
        FmtL(Sv x) : Super(*activeLocale, x) {}
        FmtL(const Ch* x) : Super(*activeLocale, x) {}
    };

    // Deduction guides
    template <class Ch> Fmt(const loc::Locale&, std::basic_string<Ch>) -> Fmt<Ch>;
    template <class Ch> Fmt(std::basic_string<Ch>) -> Fmt<Ch>;
    template <class Ch> FmtL(std::basic_string<Ch>) -> FmtL<Ch>;

    template <class Ch> Fmt(const loc::Locale&, std::basic_string_view<Ch>) -> Fmt<Ch>;
    template <class Ch> Fmt(std::basic_string_view<Ch>) -> Fmt<Ch>;
    template <class Ch> FmtL(std::basic_string_view<Ch>) -> FmtL<Ch>;

    template <class Ch> Fmt(const loc::Locale&, const Ch*) -> Fmt<Ch>;
    template <class Ch> Fmt(const Ch*) -> Fmt<Ch>;
    template <class Ch> FmtL(const Ch*) -> FmtL<Ch>;

}   // namespace loc

extern template class loc::Fmt<char>;
extern template class loc::Fmt<wchar_t>;
extern template class loc::Fmt<char8_t>;
extern template class loc::FmtL<char>;
extern template class loc::FmtL<wchar_t>;
extern template class loc::FmtL<char8_t>;

template <class Ch>
loc::Fmt<Ch>::Fmt(Str x) : loc(DefaultLocale::INST), d(std::move(x)) { init(); }

template <class Ch>
loc::Fmt<Ch>::Fmt(const Locale& lc, Str x) : loc(lc), d(std::move(x)) { init(); }

template <class Ch>
loc::Fmt<Ch>::Fmt(Sv x) : loc(DefaultLocale::INST), d(x) { init(); }

template <class Ch>
loc::Fmt<Ch>::Fmt(const Locale& lc, Sv x) : loc(lc), d(x) { init(); }

template <class Ch>
loc::Fmt<Ch>::Fmt(const Ch* x) : loc(DefaultLocale::INST), d(x) { init(); }

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
                        static_assert((std::numeric_limits<uint32_t>::max() % 10) != 9,
                                "Strange numeric system: uint32_t's base shouldn't be multiple of 10");
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
        lnkFirst = NO_LINK;
    } else {
        lnkFirst = 0;
        substs.back().lnkNext = NO_LINK;
    }
    fNextKey = 0;
}   // init()


template <class Ch>
void loc::Fmt<Ch>::nn(int x)
{
    char buf[std::numeric_limits<int>::digits10 + 4];
    auto q = std::to_chars(buf, buf + std::size(buf), x);
    std::string_view sv(buf, q.ptr);
    nnn(sv, ZsgnChecker(x));
}


template <class Ch>
void loc::Fmt<Ch>::nn(unsigned int x)
{
    char buf[std::numeric_limits<unsigned>::digits10 + 4];
    auto q = std::to_chars(buf, buf + std::size(buf), x);
    std::string_view sv(buf, q.ptr);
    nnn(sv, ZunsChecker(x));
}


template <class Ch>
void loc::Fmt<Ch>::nn(long x)
{
    char buf[std::numeric_limits<long>::digits10 + 4];
    auto q = std::to_chars(buf, buf + std::size(buf), x);
    std::string_view sv(buf, q.ptr);
    nnn(sv, ZsgnChecker(x));
}


template <class Ch>
void loc::Fmt<Ch>::nn(unsigned long x)
{
    char buf[std::numeric_limits<unsigned long>::digits10 + 4];
    auto q = std::to_chars(buf, buf + std::size(buf), x);
    std::string_view sv(buf, q.ptr);
    nnn(sv, ZunsChecker(x));
}


template <class Ch>
void loc::Fmt<Ch>::nn(long long x)
{
    char buf[std::numeric_limits<long long>::digits10 + 4];
    auto q = std::to_chars(buf, buf + std::size(buf), x);
    std::string_view sv(buf, q.ptr);
    nnn(sv, ZsgnChecker(x));
}


template <class Ch>
void loc::Fmt<Ch>::nn(unsigned long long x)
{
    char buf[std::numeric_limits<unsigned long long>::digits10 + 4];
    auto q = std::to_chars(buf, buf + std::size(buf), x);
    std::string_view sv(buf, q.ptr);
    nnn(sv, ZunsChecker(x));
}


template <class Ch>
void loc::Fmt<Ch>::nnn(std::string_view x, const Zchecker& chk)
{
    size_t lnkPrev = NO_LINK;
    size_t lnkCurr = lnkFirst;
    auto pos = 0;
    while (lnkCurr != NO_LINK) {
        auto& currSub = substs[lnkCurr];
        if (currSub.key == fNextKey) {
            auto currPos = pos + currSub.advance;
            // Do replacement
            /// @todo [urgent] What to do with the key?
            ///
            auto lnkNext = currSub.lnkNext;
            // Fix up prev
            if (lnkPrev == NO_LINK) {
                lnkFirst = lnkNext;
            } else {
                substs[lnkPrev].lnkNext = lnkNext;
            }
            // Fix up next
            if (lnkNext != NO_LINK) {
                auto& nextSub = substs[lnkNext];
                /// @todo [urgent] Fix up advance
            }
        } else {
            pos += currSub.advance;
            pos += currSub.length;
        }
        lnkPrev = lnkCurr;
        lnkCurr = currSub.lnkNext;
    }
    ++fNextKey;
}
