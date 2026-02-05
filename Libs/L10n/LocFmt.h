#pragma once

// C++
#include <cmath>
#include <charconv>
#include <cstdint>

// STL
#include <string>
#include <vector>
#include <tuple>

// Libs
#include "u_Strings.h"

#ifdef QT_STRINGS
    #include <QString>
#endif

namespace loc {

    /// @warning  Locales should NEVER return SPECIAL and BAD
    ///           SPECIAL may be used:
    ///           • in strings (as REST)
    ///           • in decimal lookups (as NO_DECISION)
    enum class Plural : unsigned char {
        ZERO, ONE, TWO, FEW, MANY, OTHER, SPECIAL,
        BAD = 9,
        LAST = SPECIAL,
        REST = SPECIAL,
        NO_DECISION = SPECIAL,
    };
    constexpr unsigned Plural_N = static_cast<unsigned>(Plural::REST) + 1;
    extern const std::string_view pluralNames[Plural_N];
    inline Plural& operator ++ (Plural& x) {
        x = static_cast<Plural>(static_cast<unsigned char>(x) + 1);
        return x;
    }
    inline Plural& operator -- (Plural& x) {
        x = static_cast<Plural>(static_cast<unsigned char>(x) - 1);
        return x;
    }

    namespace key {
        constexpr std::string_view ZERO = "zero";
        constexpr std::string_view ONE = "one";
        constexpr std::string_view TWO = "two";
        constexpr std::string_view FEW = "few";
        constexpr std::string_view MANY = "many";
        constexpr std::string_view OTHER = "other";
        constexpr std::string_view REST = "rest";
        constexpr char ST_ZERO = ZERO[0];
        constexpr char ST_ONE_OTHER = ONE[0];
        constexpr char ST_FEW = FEW[0];
        constexpr char ST_TWO = TWO[0];
        constexpr char ST_MANY = MANY[0];
        constexpr char ST_REST = REST[0];
    }

    class PluralRule {   // interface
    public:
        virtual Plural ofUint(unsigned long long n) const = 0;
        virtual Plural ofInt(long long n) const { return ofUint(std::abs(n)); }
    };

    ///  Default rule for English cardinal plurals
    ///  1 lap (one), 2+ laps (many)
    ///  Dubbed as default rule for basic plural
    ///
    class DefaultQtyRule final : public PluralRule
    {
    public:
        Plural ofUint(unsigned long long n) const override;
        static const DefaultQtyRule INST;
    };

    class Locale {  // interface
    public:
        /// Rule for cardinal numerals
        virtual const PluralRule& cardinalRule() const = 0;
        virtual char unitSpaceC() const { return ' '; }
        virtual char32_t unitSpaceL() const { return U' '; }
        virtual ~Locale() = default;
    };

    class DefaultLocale : public Locale
    {
    public:
        const PluralRule& cardinalRule() const override;
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

    template <class T>
    concept Char = std::integral<T>;

    /// Preformatted integer: both string and integral value
    template <Char Ch, std::integral Int>
    struct PreformN {
        std::basic_string_view<Ch> str;
        Int val;
    };

    template <Char Ch>
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

    #ifdef QT_STRINGS
        QString q() const;
    #endif

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

        Fmt& s(Sv x)                 { ss(x); return *this; }

        Fmt& operator() (short x)              { nn(static_cast<int>(x));           return *this; }
        Fmt& operator() (unsigned short x)     { nn(static_cast<unsigned int>(x));  return *this; }
        Fmt& operator() (int x)                { nn(x);                             return *this; }
        Fmt& operator() (unsigned int x)       { nn(x);                             return *this; }
        Fmt& operator() (long x)               { nn(x);                             return *this; }
        Fmt& operator() (unsigned long x)      { nn(x);                             return *this; }
        Fmt& operator() (long long x)          { nn(x);                             return *this; }
        Fmt& operator() (unsigned long long x) { nn(x);                             return *this; }
        Fmt& operator() (Sv x)                 { ss(x); return *this; }
        Fmt& operator() ()                     { return *this; }

        template <std::integral Int>
        Fmt& operator() (const PreformN<Ch, Int>& x)  { preformN(x.str, x.val); }

        Fmt& emptyStr() { ss(Sv{}); return *this; }

        template <class First, class Second, class... Rest>
        Fmt& operator()(First&& first, Second&& second, Rest&&... rest) {
            eat(std::forward<First>(first));
            eat(std::forward<Second>(second));
            (eat(std::forward<Rest>(rest)), ...);
            return *this;
        }

        /// Use this for variable templates, e.g. op()
        void eat(short x)              { nn(static_cast<int>(x)); }
        void eat(unsigned short x)     { nn(static_cast<unsigned int>(x)); }
        void eat(int x)                { nn(x); }
        void eat(unsigned int x)       { nn(x); }
        void eat(long x)               { nn(x); }
        void eat(unsigned long x)      { nn(x); }
        void eat(long long x)          { nn(x); }
        void eat(unsigned long long x) { nn(x); }
        void eat(Ch x) { ss({&x, 1}); }
        void eat(Sv x) { ss(x); }
        void eat() {}
        template <std::integral Int>
        void eat(const PreformN<Ch, Int>& x)  { preformN(x.str, x.val); }

        /// Can also eat tuples, dismantling them into separate params
        template <class... T>
        void eat(const std::tuple<T...>& x) { eatTup<0>(x); }

        /// Preformatted number: both preformatted string and numeric value
        void preformN(Sv preform, unsigned char x) { prefN(preform, static_cast<unsigned>(x)); }
        void preformN(Sv preform, signed char x) { prefN(preform, static_cast<int>(x)); }
        void preformN(Sv preform, unsigned short x) { prefN(preform, static_cast<unsigned>(x)); }
        void preformN(Sv preform, signed short x) { prefN(preform, static_cast<int>(x)); }
        void preformN(Sv preform, unsigned int x) { prefN(preform, x); }
        void preformN(Sv preform, signed int x) { prefN(preform, x); }
        void preformN(Sv preform, unsigned long x) { prefN(preform, x); }
        void preformN(Sv preform, signed long x) { prefN(preform, x); }
        void preformN(Sv preform, unsigned long long x) { prefN(preform, x); }
        void preformN(Sv preform, signed long long x) { prefN(preform, x); }

        /// 1. Stops substituting.
        /// 2. Returns temporary link to data.
        Str&& giveStr() { lnkFirst = NO_LINK; return std::move(d); }
    protected:
        template <std::integral T>
            void nn(T x);
        ///  Why such an architecture?
        ///  There will be three modes:
        ///  • cardinal (5 laps)
        ///  • ordinal (5th lap)
        ///  • basic plural (the laps)
        ///  Currently cardinal only. All modes are stored in substitutions.
        ///  Everything is parsed on substituting, just for simplicity.
        ///  We see ordinal substitution → ask ordinal plural rule
        template <class Sv1>
            void nnnSv(Sv1 x, const Zchecker& chk);
        template <std::integral T>
            void prefN(Sv sv, T x);
        void ss(Sv x);

        template <size_t Index, class Tup>
        inline void eatTup(const Tup& x) {
            if constexpr (Index < std::tuple_size_v<Tup>) {
                eat(std::get<Index>(x));
                eatTup<Index + 1, Tup>(x);
            }
        }
    private:
        struct Kv {
            Sv key;
            Ch *valStart, *valEnd;
            bool isKey(std::string_view x) const noexcept;
            bool isValEmpty() const { return (valStart == valEnd); }
        };

        const Locale& loc;
        Str d;

        std::vector<Zsubst> substs;
        std::vector<Kv> values;
        std::vector<size_t> smartSpans;
        size_t lnkFirst;
        size_t fNextKey;

        static constexpr size_t NO_LINK = Zsubst::NO_LINK;

        void init();

        /// Parses substitution, using substs as cache
        void parseSubst(const Zsubst& sub, size_t pos);

        /// Dumbly replaces substitution with byWhat
        /// @return addition to “advance” variable
        template <class Ch2>
        size_t dumbReplace(
                const Zsubst& sub,
                size_t pos,
                std::basic_string_view<Ch2> byWhat);

        /// Smartly replaces substitution with byWhat,
        ///   unescaping data and replacing ? with value
        /// @return addition to “advance” variable
        template <class Sv1>
        size_t replaceHash(
                const Zsubst& sub,
                size_t pos,
                const Kv& byWhat,
                Sv1 value);

        const Kv* findVal(std::string_view key) const noexcept;
        const Kv* findExactPluralVal(Plural num) const noexcept;
        static Plural parsePluralKey(const Kv& kv);
        const Kv* findPluralVal(Plural num) const noexcept;
    };

    extern const loc::Locale* activeFmtLocale;

    template <class Ch>
    class FmtL : public Fmt<Ch>
    {
    private:
        using Super = Fmt<Ch>;
    public:
        using typename Super::Str;
        using typename Super::Sv;
        FmtL(Str x) : Super(*activeFmtLocale, std::move(x)) {}
        FmtL(Sv x) : Super(*activeFmtLocale, x) {}
        FmtL(const Ch* x) : Super(*activeFmtLocale, x) {}
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

    template <class Ch> constexpr bool alwaysFalse = false;
}   // namespace loc

extern template class loc::Fmt<char>;
extern template class loc::Fmt<wchar_t>;
extern template class loc::Fmt<char8_t>;
extern template class loc::FmtL<char>;
extern template class loc::FmtL<wchar_t>;
extern template class loc::FmtL<char8_t>;

template <loc::Char Ch>
loc::Fmt<Ch>::Fmt(Str x) : loc(DefaultLocale::INST), d(std::move(x)) { init(); }

template <loc::Char Ch>
loc::Fmt<Ch>::Fmt(const Locale& lc, Str x) : loc(lc), d(std::move(x)) { init(); }

template <loc::Char Ch>
loc::Fmt<Ch>::Fmt(Sv x) : loc(DefaultLocale::INST), d(x) { init(); }

template <loc::Char Ch>
loc::Fmt<Ch>::Fmt(const Locale& lc, Sv x) : loc(lc), d(x) { init(); }

template <loc::Char Ch>
loc::Fmt<Ch>::Fmt(const Ch* x) : loc(DefaultLocale::INST), d(x) { init(); }

template <loc::Char Ch>
loc::Fmt<Ch>::Fmt(const Locale& lc, const Ch* x) : loc(lc), d(x) { init(); }

template <loc::Char Ch>
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


template <loc::Char Ch> template <std::integral T>
void loc::Fmt<Ch>::nn(T x)
{
    if (lnkFirst == NO_LINK)
        return;
    char buf[std::numeric_limits<int>::digits10 + 4];
    auto q = std::to_chars(buf, buf + std::size(buf), x);
    std::string_view sv(buf, q.ptr);
    if constexpr (std::is_signed_v<T>) {
        nnnSv(sv, ZsgnChecker(x));
    } else {
        nnnSv(sv, ZunsChecker(x));
    }
}


template <loc::Char Ch> template <std::integral T>
void loc::Fmt<Ch>::prefN(Sv sv, T x)
{
    if (lnkFirst == NO_LINK)
        return;
    if constexpr (std::is_signed_v<T>) {
        nnnSv(sv, ZsgnChecker(x));
    } else {
        nnnSv(sv, ZunsChecker(x));
    }
}


template <loc::Char Ch> template <class Sv1>
void loc::Fmt<Ch>::nnnSv(Sv1 x, const Zchecker& chk)
{
    size_t lnkPrev = NO_LINK;
    size_t lnkCurr = lnkFirst;
    auto pos = 0;
    while (lnkCurr != NO_LINK) {
        auto& currSub = substs[lnkCurr];
        if (currSub.key == fNextKey) {
            // pos will stay!!
            auto currPos = pos + currSub.advance;
            // Parse substitution
            parseSubst(currSub, currPos);
            // Do replacement
            size_t newAdvance;
            if (values.empty()) {
                // Basic replacement
                newAdvance = dumbReplace(currSub, currPos, x);
            } else {
                /// @todo [future] modes other than cardinal (5 laps completed):
                ///    ordinal (5th lap completed), simple plural (laps completed)
                /// @todo [future] short cardinal: {1|q=lap:s} completed
                auto plural = chk.check(loc.cardinalRule());
                const Kv* tmpl = findPluralVal(plural);
                if (tmpl) {
                    newAdvance = replaceHash(currSub, currPos, *tmpl, x);
                } else {
                    newAdvance = dumbReplace(currSub, currPos, x);
                }
            }
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
                nextSub.advance += newAdvance;
            }
            // leave pos as is, we’ll add newAdvance later
            // leave lnkPrev as is
        } else {    // Just skip
            pos += currSub.advance;
            pos += currSub.length;
            lnkPrev = lnkCurr;
        }
        lnkCurr = currSub.lnkNext;
    }
    ++fNextKey;
}


template <loc::Char Ch>
void loc::Fmt<Ch>::parseSubst(const Zsubst& sub, size_t pos)
{
    values.clear();

    Ch* p = d.data() + pos + 1;
    Ch* end = p + sub.length - 2;
    // We know that all right braces are escaped here
    Ch* pStart = p;
    Ch* pEqual = nullptr;
    bool hasBadChars = false;

    auto processPart = [this, &pStart, &pEqual, &hasBadChars](Ch* p) {
        // Equal sign should exist
        // Bad chars should not exist
        if (!pEqual || hasBadChars)
            return;
        auto key =  str::trimSv(Sv{pStart, pEqual});
        // Key should not be empty
        if (key.empty())
            return;
        values.emplace_back(key, pEqual + 1, p);
    };

    for (; p != end; ++p) {
        uint_fast32_t c = *p;
        switch (c) {
        case '=':   // Key-value delimiter
            if (!pEqual)
                pEqual = p;
            break;
        case '{':   // Escape
            if ((++p) == end)
                goto brk1;
            break;
        case '|':   // Pair delimiter
            processPart(p);
            pStart = p + 1;
            pEqual = nullptr;
            hasBadChars = false;
            break;
        default:
            if (!pEqual) {  // in key outside ASCII → bad chars
                if (c < ' ' || c >= 127)
                    hasBadChars = true;
            }
        }
    }
brk1:
    processPart(end);
}


template <loc::Char Ch>
bool loc::Fmt<Ch>::Kv::isKey(std::string_view x) const noexcept
{
    return (x.length() == key.length()
            && std::equal(x.begin(), x.end(), key.begin()));
}


template <loc::Char Ch> template <class Ch2>
size_t loc::Fmt<Ch>::dumbReplace(
        const Zsubst& sub, size_t pos, std::basic_string_view<Ch2> byWhat)
{
    auto delta = static_cast<ptrdiff_t>(byWhat.length()) - static_cast<ptrdiff_t>(sub.length);
    if (delta < 0) {
        d.erase(pos, -delta);
    } else if (delta > 0) {
        d.insert(pos, delta, 0);
    }
    std::copy(byWhat.begin(), byWhat.end(), d.data() + pos);
    return sub.advance + byWhat.length();
}


template <loc::Char Ch>
void loc::Fmt<Ch>::ss(Sv x)
{
    size_t lnkPrev = NO_LINK;
    size_t lnkCurr = lnkFirst;
    auto pos = 0;
    while (lnkCurr != NO_LINK) {
        auto& currSub = substs[lnkCurr];
        if (currSub.key == fNextKey) {
            // pos will stay!!
            auto currPos = pos + currSub.advance;
            size_t newAdvance = dumbReplace(currSub, currPos, x);
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
                nextSub.advance += newAdvance;
            }
            // leave pos as is, we’ll add newAdvance later
        } else {    // Just skip
            pos += currSub.advance;
            pos += currSub.length;
            lnkPrev = lnkCurr;
        }
        lnkCurr = currSub.lnkNext;
    }
    ++fNextKey;
}


template <loc::Char Ch>
auto loc::Fmt<Ch>::findVal(std::string_view key) const noexcept -> const Kv*
{
    for (const Kv& v : values) {
        if (v.isKey(key)) {
            return &v;
        }
    }
    return nullptr;
}


template <loc::Char Ch>
auto loc::Fmt<Ch>::findExactPluralVal(loc::Plural plural) const noexcept -> const Kv*
{
    return findVal(pluralNames[static_cast<unsigned char>(plural)]);
}


template <loc::Char Ch>
loc::Plural loc::Fmt<Ch>::parsePluralKey(const Kv& kv)
{
    if (kv.key.empty())
        return Plural::BAD;
    switch (kv.key[0]) {
    case key::ST_ZERO:
        if (kv.isKey(key::ZERO))
            return Plural::ZERO;
        break;
    case key::ST_ONE_OTHER:
        if (kv.isKey(key::ONE))
            return Plural::ONE;
        if (kv.isKey(key::OTHER))
            return Plural::OTHER;
        break;
    case key::ST_TWO:
        if (kv.isKey(key::TWO))
            return Plural::TWO;
        break;
    case key::ST_FEW:
        if (kv.isKey(key::FEW))
            return Plural::FEW;
        break;
    case key::ST_MANY:
        if (kv.isKey(key::MANY))
            return Plural::MANY;
        break;
    case key::ST_REST:
        if (kv.isKey(key::REST))
            return Plural::REST;
        break;
    default: ;
    }
    return Plural::BAD;
}


template <loc::Char Ch>
auto loc::Fmt<Ch>::findPluralVal(loc::Plural plural) const noexcept -> const Kv*
{
    auto mainKey = pluralNames[static_cast<unsigned char>(plural)];
    const Kv* fallbacks[Plural_N] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
    bool hasFallback = false;
    for (const Kv& v : values) {
        if (v.isKey(mainKey)) {
            return &v;
        }
        // Otherwise parse
        if (auto newKey = parsePluralKey(v); newKey != Plural::BAD) {
            fallbacks[static_cast<unsigned char>(newKey)] = &v;
            hasFallback = true;
        }
    }
    // Most substitutions have no fallbacks
    if (!hasFallback)
        return nullptr;
    // Check Rest
    if (auto q = fallbacks[static_cast<unsigned char>(Plural::REST)])
        return q;
    // Go forward
    for (loc::Plural p = plural; p < Plural::OTHER;) { ++p;
        if (auto q = fallbacks[static_cast<unsigned char>(p)])
            return q;
    }
    // Go back
    for (loc::Plural p = plural; p > Plural::ZERO;) { --p;
        if (auto q = fallbacks[static_cast<unsigned char>(p)])
            return q;
    }
    return nullptr;
}


template <loc::Char Ch> template <class Sv1>
size_t loc::Fmt<Ch>::replaceHash(
        const Zsubst& sub, size_t pos, const Kv& byWhat, Sv1 value)
{
    // byWhat is empty?
    if (byWhat.isValEmpty()) {
        d.erase(pos, sub.length);
        return sub.advance;
    }
    // Escape string, count smartSpans
    smartSpans.clear();
    size_t pOut = pos;
    for (auto ptrIn = byWhat.valStart; ptrIn != byWhat.valEnd; ++ptrIn) {
        auto c = *ptrIn;
        switch (c) {
        case '{':   // Escape
            if ((++ptrIn) == byWhat.valEnd)
                goto brk;
            d[pOut++] = *ptrIn;
            break;
        case '#':   // Replacement
            smartSpans.push_back(pOut);
            break;
        default:
            d[pOut++] = c;
        }
    }
brk:
    // Now pOutStart..pOus is our template
    auto newLength = (pOut - pos) + value.length() * smartSpans.size();
    auto delta = static_cast<ptrdiff_t>(newLength) - static_cast<ptrdiff_t>(sub.length);
    if (delta < 0) {
        d.erase(pOut, -delta);
    } else if (delta > 0) {
        d.insert(pOut, delta, 0);
    }
    if (!value.empty()) {   // Value is empty → already OK
        // Go back
        auto ptrDest = d.data() + pos + newLength;
        for (auto q = smartSpans.end(); q != smartSpans.begin(); ) { --q;
            // Static
            ptrDest = std::copy_backward(d.data() + *q, d.data() + pOut, ptrDest);
            // Value
            ptrDest = std::copy_backward(value.begin(), value.end(), ptrDest);
            // Reassign
            pOut = *q;
        }
    }
    return sub.advance + newLength;
}

#ifdef QT_STRINGS
template <loc::Char Ch>
QString loc::Fmt<Ch>::q() const
{
    if constexpr (sizeof(Ch) == sizeof(char)) {
        return QString::fromUtf8(reinterpret_cast<const char*>(d.data()), d.length());
    } else if constexpr (sizeof(Ch) == sizeof(char16_t)) {
        return QString::fromUtf16(reinterpret_cast<const char16_t*>(d.data()), d.length());
    } else if constexpr (sizeof(Ch) == sizeof(char32_t)) {
        return QString::fromUcs4(reinterpret_cast<const char32_t*>(d.data()), d.length());
    } else {
        static_assert(alwaysFalse<Ch>, "Unknown conversion method for this type");
    }
}
#endif
