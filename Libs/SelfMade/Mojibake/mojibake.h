#pragma once

///
///  MOJIBAKE
///    simple C++17 Unicode conversion library
///
///  LICENSE: MIT
///

#include <algorithm>
#include <iterator>
#include <type_traits>

#if __cplusplus >= 202002L
    #include <bit>
#endif

#include "internal/auto.hpp"

namespace mojibake {

    constexpr char32_t SURROGATE_MIN = 0xD800;
    constexpr char32_t SURROGATE_MAX = 0xDFFF;
    constexpr char32_t SURROGATE_LO_MIN = SURROGATE_MIN;
    constexpr char32_t SURROGATE_HI_MIN = 0xDC00;
    constexpr char32_t SURROGATE_LO_MAX = SURROGATE_HI_MIN - 1;
    constexpr char32_t SURROGATE_HI_MAX = SURROGATE_MAX;
    constexpr char32_t MOJIBAKE = 0xFFFD;
    constexpr char32_t UNICODE_MAX = 0x10FFFF;
    constexpr char32_t U8_1BYTE_MAX = 0x7F;
    constexpr char32_t U8_2BYTE_MIN = 0x80;
    constexpr char32_t U8_2BYTE_MAX = 0x7FF;
    constexpr char32_t U8_3BYTE_MIN = 0x800;
    constexpr char32_t U8_3BYTE_MAX = 0xFFFF;
    constexpr char32_t U8_4BYTE_MIN = 0x10000;
    constexpr char32_t U8_4BYTE_MAX = UNICODE_MAX;
    constexpr char32_t U16_1WORD_MAX = 0xFFFF;
    constexpr char32_t U16_2WORD_MIN = 0x10000;
    constexpr char32_t U16_2WORD_MAX = UNICODE_MAX;

    struct Utf8  { using Ch = char; };
    struct Utf16 { using Ch = char16_t; };
    struct Utf32 { using Ch = char32_t; };

    /// @warning  Reimplement to true for limited iterators
    template <class Iterator>
    struct IteratorLimit {
        static constexpr bool isLimited = false;
        static constexpr size_t remainder(const Iterator&) { return 10; }
    };

    ///
    /// @return [+] codepoint is good: allocated, unallocated, reserved,
    ///               private-use, non-character
    ///         [-] it is bad: surrogate, too high
    ///
    inline bool isValid(char32_t cp) noexcept
    {
        return (cp < SURROGATE_MIN
                || (cp > SURROGATE_MAX && cp <= UNICODE_MAX));
    }

    enum class Event {
        CODE,       // UTF-8/32, bad character code, or too long sequence. Ptr = START
        BYTE_START, // UTF-8/16, bad starting byte/word. Ptr obvious
        BYTE_NEXT,  // UTF-8/16, bad any byte/word except start. Ptr = BAD
        END,        // UTF-8/16, code sequence abruptly ended. Ptr = START
    };

    namespace handler {

        constexpr char32_t RET_SKIP = 0xFFFFFF;
        constexpr char32_t FG_HALT = 0x80000000;
        constexpr char32_t MASK_CODE = 0xFFFFFF;
        constexpr char32_t RET_HALT = RET_SKIP | FG_HALT;

        ///
        /// Mojibake handler MUST:
        /// • accept two params: place and event
        /// • return code point or RET_SKIP, maybe with FG_HALT.
        ///
        /// Mojibake support varies between serialization types, and…
        /// • [U32] every bad codepoint → CODE
        /// • [U8/16] implementation-dependent, but…
        ///   • [U8] surrogate/high but well-serialized → CODE
        ///   • [U8] too long code sequence → CODE
        ///   • incomplete then good → BYTE_NEXT + normal recovery
        ///       e.g. losur + losur + hisur = mojibake(BYTE_NEXT) + surrogate
        ///   • [U8] if several of abrupt end, bad byte and low/high CP happen,
        ///       decoder reports JUST ONE problem.
        ///         BYTE_NEXT > END, CODE!!!
        ///       Example: F0 8F 30 = …
        ///        • abrupt end (F0 → 4-byte code sequence)
        ///        • too low code (F0 8F → code < 10000, need 3 bytes)
        ///        • bad byte 30 (80…BF allowed here)
        ///       We definitely respond BYTE_NEXT here (it’s > end, code)
        ///
        template <class It>
        class Skip final {
        public:
            inline char32_t operator () (

                    [[maybe_unused]] It place,
                    [[maybe_unused]] Event event) const noexcept { return RET_SKIP; }
        };  // class Skip

        template <class It>
        class Moji final {
        public:
            inline char32_t operator () (
                    [[maybe_unused]] It place,
                    [[maybe_unused]] Event event) const noexcept { return MOJIBAKE; }
        };  // class Skip

        template <class It>
        class MojiHalt final {
        public:
            inline char32_t operator () (
                    [[maybe_unused]] It place,
                    [[maybe_unused]] Event event) const noexcept { return MOJIBAKE | FG_HALT; }
        };  // class Skip

    }   // namespace handler
} // namespace mojibake

#include "internal/detail.hpp"

namespace mojibake {
    ///
    /// Puts code point to some iterator
    /// @tparam  It   iterator
    /// @tparam  Enc  enc::Utf8, enc::Utf16, etc::Utf32
    /// @param [in,out]  it   iterator
    /// @param [in]      cp   code point, SHOULD BE GOOD
    /// @warning  Bhv on bad cp is implementation-specific
    /// @return [+] was put [-] no room
    ///
    template <class It,
              class Enc = typename detail::ItUtfTraits<It>::Enc,
              class = std::void_t<typename std::iterator_traits<It>::value_type>>
    inline bool put(It& it, char32_t cp)
        { return detail::ItEnc<It, Enc>::put(it, cp); }

    template <class Enc,
              class It,
              class = std::void_t<typename std::iterator_traits<It>::value_type>>
    inline bool put(It& it, char32_t cp)
        { return detail::ItEnc<It, Enc>::put(it, cp); }


    ///
    /// Copies data to another,
    /// REACTING APPROPRIATELY TO MOJIBAKE
    ///
    /// @warning Even if encoding is the same, deserializes/checks/serializes
    ///          codepoints.
    ///
    template <class It1, class It2,
              class Enc1 = typename detail::ItUtfTraits<It1>::Enc,
              class Enc2 = typename detail::ItUtfTraits<It2>::Enc,
              class Mjh,
              class = std::void_t<typename std::iterator_traits<It1>::value_type>,
              class = std::void_t<typename std::iterator_traits<It2>::value_type>>
    inline It2 copy(It1 beg, It1 end, It2 dest, const Mjh& onMojibake = Mjh{})
    {
        return detail::ItEnc<It1, Enc1>::template copy<It2, Enc2, Mjh>(beg, end, dest, onMojibake);
    }

    template <class Enc1, class Enc2, class Mjh, class It1, class It2,
              class = std::void_t<typename std::iterator_traits<It1>::value_type>,
              class = std::void_t<typename std::iterator_traits<It2>::value_type>>
    inline It2 copy(It1 beg, It1 end, It2 dest, const Mjh& onMojibake = Mjh{})
    {
        return copy<It1, It2, Enc1, Enc2, Mjh>(beg, end, dest, onMojibake);
    }

    template <class Enc2, class Mjh, class It1, class It2,
              class Enc1 = typename detail::ItUtfTraits<It1>::Enc,
              class = std::void_t<typename std::iterator_traits<It1>::value_type>,
              class = std::void_t<typename std::iterator_traits<It2>::value_type>>
    inline It2 copy(It1 beg, It1 end, It2 dest, const Mjh& onMojibake = Mjh{})
    {
        return copy<It1, It2, Enc1, Enc2, Mjh>(beg, end, dest, onMojibake);
    }

    ///
    /// Copies data to another, SKIPPING MOJIBAKE
    /// So no mojibake handler
    ///
    template <class It1, class It2,
              class Enc1 = typename detail::ItUtfTraits<It1>::Enc,
              class Enc2 = typename detail::ItUtfTraits<It2>::Enc,
              class = std::void_t<typename std::iterator_traits<It1>::value_type>,
              class = std::void_t<typename std::iterator_traits<It2>::value_type>>
    inline It2 copyS(It1 beg, It1 end, It2 dest)
    {
        using Sk = mojibake::handler::Skip<It1>;
        return detail::ItEnc<It1, Enc1>::template copy<It2, Enc2, Sk>(beg, end, dest, Sk{});
    }

    template <class Enc1, class Enc2, class It1, class It2,
              class = std::void_t<typename std::iterator_traits<It1>::value_type>,
              class = std::void_t<typename std::iterator_traits<It2>::value_type>>
    inline It2 copyS(It1 beg, It1 end, It2 dest)
    {
        return copyS<It1, It2, Enc1, Enc2>(beg, end, dest);
    }

    template <class Enc2, class It1, class It2,
              class Enc1 = typename detail::ItUtfTraits<It1>::Enc,
              class = std::void_t<typename std::iterator_traits<It1>::value_type>,
              class = std::void_t<typename std::iterator_traits<It2>::value_type>>
    inline It2 copyS(It1 beg, It1 end, It2 dest)
    {
        return copyS<It1, It2, Enc1, Enc2>(beg, end, dest);
    }

    ///
    /// Copies data to another, WRITING MOJIBAKE
    /// So no mojibake handler
    ///
    template <class It1, class It2,
              class Enc1 = typename detail::ItUtfTraits<It1>::Enc,
              class Enc2 = typename detail::ItUtfTraits<It2>::Enc,
              class = std::void_t<typename std::iterator_traits<It1>::value_type>,
              class = std::void_t<typename std::iterator_traits<It2>::value_type>>
    inline It2 copyM(It1 beg, It1 end, It2 dest)
    {
        using Mo = mojibake::handler::Moji<It1>;
        return detail::ItEnc<It1, Enc1>::template copy<It2, Enc2, Mo>(beg, end, dest, Mo{});
    }

    template <class Enc1, class Enc2, class It1, class It2,
              class = std::void_t<typename std::iterator_traits<It1>::value_type>,
              class = std::void_t<typename std::iterator_traits<It2>::value_type>>
    inline It2 copyM(It1 beg, It1 end, It2 dest)
    {
        return copyM<It1, It2, Enc1, Enc2>(beg, end, dest);
    }

    template <class Enc2, class It1, class It2,
              class Enc1 = typename detail::ItUtfTraits<It1>::Enc,
              class = std::void_t<typename std::iterator_traits<It1>::value_type>,
              class = std::void_t<typename std::iterator_traits<It2>::value_type>>
    inline It2 copyM(It1 beg, It1 end, It2 dest)
    {
        return copyM<It1, It2, Enc1, Enc2>(beg, end, dest);
    }

    ///
    /// Copies data to another, WRITING MOJIBAKE AND HALTING
    /// So no mojibake handler
    ///
    template <class It1, class It2,
              class Enc1 = typename detail::ItUtfTraits<It1>::Enc,
              class Enc2 = typename detail::ItUtfTraits<It2>::Enc,
              class = std::void_t<typename std::iterator_traits<It1>::value_type>,
              class = std::void_t<typename std::iterator_traits<It2>::value_type>>
    inline It2 copyMH(It1 beg, It1 end, It2 dest)
    {
        using Mh = mojibake::handler::MojiHalt<It1>;
        return detail::ItEnc<It1, Enc1>::template copy<It2, Enc2, Mh>(beg, end, dest, Mh{});
    }

    template <class Enc1, class Enc2, class It1, class It2,
              class = std::void_t<typename std::iterator_traits<It1>::value_type>,
              class = std::void_t<typename std::iterator_traits<It2>::value_type>>
    inline It2 copyMH(It1 beg, It1 end, It2 dest)
    {
        return copyMH<It1, It2, Enc1, Enc2>(beg, end, dest);
    }

    template <class Enc2, class It1, class It2,
              class Enc1 = typename detail::ItUtfTraits<It1>::Enc,
              class = std::void_t<typename std::iterator_traits<It1>::value_type>,
              class = std::void_t<typename std::iterator_traits<It2>::value_type>>
    inline It2 copyMH(It1 beg, It1 end, It2 dest)
    {
        return copyMH<It1, It2, Enc1, Enc2>(beg, end, dest);
    }

    ///
    /// Copies data to another, QUICK AND DIRTY
    ///     We don’t have a funk what to do with bad characters
    ///     (actually replaces with mojibake character)
    /// So no mojibake handler
    /// @warning  Used for extreme optimization of cross-platform or highly
    ///             templated code, does not recode when encoding is the same
    /// @warning  Use if data is reliable, or it is of low importance like error messages
    ///
    template <class It1, class It2,
              class Enc1 = typename detail::ItUtfTraits<It1>::Enc,
              class Enc2 = typename detail::ItUtfTraits<It2>::Enc,
              class = std::void_t<typename std::iterator_traits<It1>::value_type>,
              class = std::void_t<typename std::iterator_traits<It2>::value_type>>
    inline It2 copyQ(It1 beg, It1 end, It2 dest)
    {
        if constexpr (std::is_same_v<Enc1, Enc2>) {
            return std::copy(beg, end, dest);
        } else {
            return copyM<It1, It2, Enc1, Enc2>(beg, end, dest);
        }
    }

    template <class Enc1, class Enc2, class It1, class It2,
              class = std::void_t<typename std::iterator_traits<It1>::value_type>,
              class = std::void_t<typename std::iterator_traits<It2>::value_type>>
    inline It2 copyQ(It1 beg, It1 end, It2 dest)
    {
        return copyQ<It1, It2, Enc1, Enc2>(beg, end, dest);
    }

    template <class Enc2, class It1, class It2,
              class Enc1 = typename detail::ItUtfTraits<It1>::Enc,
              class = std::void_t<typename std::iterator_traits<It1>::value_type>,
              class = std::void_t<typename std::iterator_traits<It2>::value_type>>
    inline It2 copyQ(It1 beg, It1 end, It2 dest)
    {
        return copyQ<It1, It2, Enc1, Enc2>(beg, end, dest);
    }

    ///
    /// Generic conversion
    ///
    template <class To, class From, class Mjh,
              class Enc2 = typename detail::ContUtfTraits<To>::Enc,
              class Enc1 = typename detail::ContUtfTraits<From>::Enc>
    To to(const From& from, const Mjh& onMojibake = Mjh{})
    {
        To r;
        std::back_insert_iterator it(r);
        using It1 = decltype(std::begin(from));
        using It2 = decltype(it);
        copy<It1, It2, Enc1, Enc2, Mjh>(std::begin(from), std::end(from), it, onMojibake);
        return r;
    }

    ///
    /// Same, mojibake just skipped
    ///
    template <class To, class From,
              class Enc2 = typename detail::ContUtfTraits<To>::Enc,
              class Enc1 = typename detail::ContUtfTraits<From>::Enc>
    inline To toS(const From& from)
    {
        using It = decltype(std::begin(from));
        using Sk = mojibake::handler::Skip<It>;
        return to<To, From, Sk, Enc2, Enc1>(from);
    }

    /// Implementation for const char*
    template <class To, class From,
              class Enc2 = typename detail::ContUtfTraits<To>::Enc,
              class Enc1 = typename detail::UtfTraits<From>::Enc>   // Also a SFINAE
    inline To toS(const From* from)
    {
        std::basic_string_view from1{from};
        return toS<To, decltype(from1), Enc2, Enc1>(from1);
    }

    ///
    /// Same, mojibake quietly displayed
    ///
    template <class To, class From,
              class Enc2 = typename detail::ContUtfTraits<To>::Enc,
              class Enc1 = typename detail::ContUtfTraits<From>::Enc>
    inline To toM(const From& from)
    {
        using It = decltype(std::begin(from));
        using Mo = mojibake::handler::Moji<It>;
        return to<To, From, Mo, Enc2, Enc1>(from);
    }

    /// Implementation for const char*
    template <class To, class From,
              class Enc2 = typename detail::ContUtfTraits<To>::Enc,
              class Enc1 = typename detail::UtfTraits<From>::Enc>   // Also a SFINAE
    inline To toM(const From* from)
    {
        std::basic_string_view from1{from};
        return toM<To, decltype(from1), Enc2, Enc1>(from1);
    }

    ///
    /// Same, quick and dirty — when we don’t have a funk what to do with bad chars
    /// @warning   Does not recode when encoding is the same
    /// @warning   Use when data is surely good, or for low-importance strings
    ///            like error messages
    ///
    template <class To, class From,
              class Enc2 = typename detail::ContUtfTraits<To>::Enc,
              class Enc1 = typename detail::ContUtfTraits<From>::Enc>
    inline To toQ(const From& from)
    {
        To r;
        std::back_insert_iterator it(r);
        using It1 = decltype(std::begin(from));
        using It2 = decltype(it);
        /// @todo [mojibake,performance] How to check for append (beg, end) function?
        copyQ<It1, It2, Enc1, Enc2>(std::begin(from), std::end(from), it);
        return r;
    }

    /// Implementation for const char*
    template <class To, class From,
              class Enc2 = typename detail::ContUtfTraits<To>::Enc,
              class Enc1 = typename detail::UtfTraits<From>::Enc>   // Also a SFINAE
    inline To toQ(const From* from)
    {
        std::basic_string_view from1{from};
        return toQ<To, decltype(from1), Enc2, Enc1>(from1);
    }

    ///
    /// Check for string validity
    /// @return [+] all CPs are well-encoded, and are
    ///              allocated, unallocated, reserved, private-use, noncharacters
    ///         [-] at least one CP is surrogate, too high or badly-encoded
    ///              (abruptly ends, too long, starts with continuation code unit)
    /// @warning Unallocated will eventually be reserved, then allocated.
    ///           And the function is quick and dirty, and does not have
    ///           a character base.
    /// @warning Private-use are not controlled by Consortium at all
    /// @warning It’s possible to use noncharacters in strings, e.g. as weight
    ///           for sorting, substitution char, error code, special value
    ///           that’s unlikely to appear in the wild etc.
    template <class Cont,
              class Enc = typename detail::ContUtfTraits<Cont>::Enc>
    bool isValid(const Cont& cont)
    {
        using It = decltype(std::begin(cont));
        return detail::ItEnc<It, Enc>::isValid(std::begin(cont), std::end(cont));
    }

    template <class Enc = Utf32>
    inline bool isValid(const char32_t* x)
    {
        std::basic_string_view sv(x);
        return isValid<decltype(sv), Enc>(sv);
    }

    template <class Enc = Utf16>
    inline bool isValid(const char16_t* x)
    {
        std::basic_string_view sv(x);
        return isValid<decltype(sv), Enc>(sv);
    }

    template <class Enc = typename detail::LenTraits<sizeof(wchar_t)>::Enc>
    inline bool isValid(const wchar_t* x)
    {
        std::basic_string_view sv(x);
        return isValid<decltype(sv), Enc>(sv);
    }

    template <class Enc = Utf8>
    inline bool isValid(const char* x)
    {
        std::basic_string_view sv(x);
        return isValid<decltype(sv), Enc>(sv);
    }

#if __cplusplus >= 202002L
    template <class Enc = Utf8>
    inline bool isValid(const char8_t* x)
    {
        std::basic_string_view sv(x);
        return isValid<decltype(sv), Enc>(sv);
    }
#endif

    ///
    /// Simple case fold
    ///
    inline char32_t simpleCaseFoldCp(char32_t x)
    {
        if (x >= detail::CF_MAXCP)
            return x;
        auto index = detail::simpleCfLookup[x >> detail::CF_BLOCKSHIFT];
        if (index < 0)
            return x;
        return detail::simpleCfBlocks[index][x & detail::CF_BLOCKMASK];
    }

    inline char32_t simpleCaseFold(char32_t x) { return simpleCaseFoldCp(x); }

    template <class To, class Func, class Enc>
    class AppendFuncIterator
    {
    public:
        AppendFuncIterator(To& r, const Func& aFunc) : it(r), func(aFunc) {}

        using value_type = char32_t;
        const AppendFuncIterator& operator ++() const noexcept { return *this; }
        const AppendFuncIterator& operator * () const noexcept { return *this; }
        const AppendFuncIterator* operator ->() const noexcept { return this; }
        void operator = (value_type c) const { put<Enc>(it, func(c)); }
    private:
        mutable std::back_insert_iterator<To> it;
        const Func& func;
    };

    template <class To, class From,
              class Enc2 = typename detail::ContUtfTraits<To>::Enc,
              class Enc1 = typename detail::ContUtfTraits<From>::Enc>
    To simpleCaseFold(const From& from)
    {
        To to;
        AppendFuncIterator<To, decltype(simpleCaseFoldCp), Enc2> it(to, simpleCaseFoldCp);
        // As case fold is just for comparison → skip bad!
        using It = decltype(std::begin(from));
        using Sk = mojibake::handler::Skip<It>;
        mojibake::copy<It, decltype(it), Enc1, Utf32, Sk>(
                    std::begin(from), std::end(from), it, Sk());
        return to;
    }

    /// This function exploits a common feature of most STL implementations:
    /// containers do not shrink, just expand
    template <class To, class From,
              class Enc2 = typename detail::ContUtfTraits<To>::Enc,
              class Enc1 = typename detail::ContUtfTraits<From>::Enc>
    To& simpleCaseFold(const From& from, To& to)
    {
        to.clear();
        AppendFuncIterator<To, decltype(simpleCaseFoldCp), Enc2> it(to, simpleCaseFoldCp);
        // As case fold is just for comparison → skip bad!
        using It = decltype(std::begin(from));
        using Sk = mojibake::handler::Skip<It>;
        mojibake::copy<It, decltype(it), Enc1, Utf32, Sk>(
                    std::begin(from), std::end(from), it, Sk());
        return to;
    }

    template <class To, class From,
              class Enc2 = typename detail::ContUtfTraits<To>::Enc,
              class Enc1 = typename detail::UtfTraits<From>::Enc>       // also a SFINAE
    To simpleCaseFold(const From* from)
    {
        std::basic_string_view from1{from};
        return simpleCaseFold<To, decltype(from1), Enc2, Enc1>(from1);
    }

    ///
    /// Pseudo-iterator for mojibake::put that calls some functor instead
    ///
    template <class Enc, class Func>
    class CallIterator
    {
    public:
        using value_type = typename Enc::Ch;
        CallIterator(const Func& aFunc) noexcept : func(aFunc) {}
        const CallIterator& operator ++() const noexcept { return *this; }
        const CallIterator& operator * () const noexcept { return *this; }
        const CallIterator* operator ->() const noexcept { return this; }
        void operator = (value_type c) const { func(c); }
    private:
        const Func& func;
    };

    template <class Func>
    class Utf8CallIterator : public CallIterator<Utf8, Func>
    {
    public:
        using CallIterator<Utf8, Func>::CallIterator;
    };

    template <class Func>
    Utf8CallIterator(const Func&) -> Utf8CallIterator<Func>;

    template <class Func>
    class Utf16CallIterator : public CallIterator<Utf16, Func>
    {
    public:
        using CallIterator<Utf16, Func>::CallIterator;
    };

    template <class Func>
    Utf16CallIterator(const Func&) -> Utf16CallIterator<Func>;

    template <class Func>
    class Utf32CallIterator : public CallIterator<Utf32, Func>
    {
    public:
        using CallIterator<Utf32, Func>::CallIterator;
    };

    template <class Func>
    Utf32CallIterator(const Func&) -> Utf32CallIterator<Func>;

    template <class It>
    struct LimitedIterator {
        It curr;
        const It end;

        constexpr LimitedIterator(It aCurr, It aEnd) : curr(aCurr), end(aEnd) {}
        constexpr LimitedIterator(It aCurr, size_t size) : curr(aCurr), end(aCurr + size) {}
        constexpr LimitedIterator& operator ++() { ++curr; return *this; }
        constexpr auto& operator * () const { return *curr; }
        constexpr auto operator -> () const { return &*curr; }
    };

    template <class UnderIt>
    struct IteratorLimit<LimitedIterator<UnderIt>> {
        using It = LimitedIterator<UnderIt>;
        static constexpr bool isLimited = true;
        static constexpr size_t remainder(const It& x) { return x.end - x.curr; }
    };

    ///
    /// Conversion to limited buffer
    /// DOES NOT put trailing zero
    ///
    template <class Mjh, class From, class ItTo,
              class Enc1 = typename detail::ContUtfTraits<From>::Enc,
              class Enc2 = typename detail::ItUtfTraits<ItTo>::Enc>
    ItTo copyLim(const From& from, ItTo beg, ItTo end, const Mjh& onMojibake = Mjh{})
    {
        LimitedIterator it(beg, end);
        using It1 = decltype(std::begin(from));
        using It2 = decltype(it);
        return copy<Enc1, Enc2, Mjh, It1, It2>(std::begin(from), std::end(from), it, onMojibake).curr;
    }

    ///
    /// @overload   beg/size instead of beg/end
    ///
    template <class Mjh, class From, class ItTo,
              class Enc1 = typename detail::ContUtfTraits<From>::Enc,
              class Enc2 = typename detail::ItUtfTraits<ItTo>::Enc>
    ItTo copyLim(const From& from, ItTo beg, size_t size, const Mjh& onMojibake = Mjh{})
    {
        LimitedIterator it(beg, size);
        using It1 = decltype(std::begin(from));
        using It2 = decltype(it);
        return copy<Enc1, Enc2, Mjh, It1, It2>(std::begin(from), std::end(from), it, onMojibake).curr;
    }

    ///
    /// Conversion to limited buffer; skip bad codepoints
    /// DOES NOT put trailing zero
    /// @warning   No copyLimQ because of its limited usefulness:
    ///            depending on system, we get incomplete codepoint,
    ///            though copyLim’s aim is to get rid of those
    ///
    template <class From, class ItTo,
              class Enc1 = typename detail::ContUtfTraits<From>::Enc,
              class Enc2 = typename detail::ItUtfTraits<ItTo>::Enc>
    inline ItTo copyLimS(const From& from, ItTo beg, ItTo end)
    {
        using It1 = decltype(std::begin(from));
        using Mjh = handler::Skip<It1>;
        return copyLim<Mjh, From, ItTo, Enc1, Enc2>(from, beg, end);
    }

    ///
    /// @overload   beg/size instead of beg/end
    ///
    template <class From, class ItTo,
              class Enc1 = typename detail::ContUtfTraits<From>::Enc,
              class Enc2 = typename detail::ItUtfTraits<ItTo>::Enc>
    inline ItTo copyLimS(const From& from, ItTo beg, size_t size)
    {
        using It1 = decltype(std::begin(from));
        using Mjh = handler::Skip<It1>;
        return copyLim<Mjh, From, ItTo, Enc1, Enc2>(from, beg, size);
    }

    ///
    /// Conversion to limited buffer; write mojibake character
    /// DOES NOT put trailing zero
    ///
    template <class From, class ItTo,
              class Enc1 = typename detail::ContUtfTraits<From>::Enc,
              class Enc2 = typename detail::ItUtfTraits<ItTo>::Enc>
    inline ItTo copyLimM(const From& from, ItTo beg, ItTo end)
    {
        using It1 = decltype(std::begin(from));
        using Mjh = handler::Moji<It1>;
        return copyLim<Mjh, From, ItTo, Enc1, Enc2>(from, beg, end);
    }

    ///
    /// @overload   beg/size instead of beg/end
    ///
    template <class From, class ItTo,
              class Enc1 = typename detail::ContUtfTraits<From>::Enc,
              class Enc2 = typename detail::ItUtfTraits<ItTo>::Enc>
    inline ItTo copyLimM(const From& from, ItTo beg, size_t size)
    {
        using It1 = decltype(std::begin(from));
        using Mjh = handler::Moji<It1>;
        return copyLim<Mjh, From, ItTo, Enc1, Enc2>(from, beg, size);
    }

    template <class T>
    using EquivChar = typename detail::LenTraits<sizeof(T)>::EquivChar;
} // namespace mojibake

#include "internal/detail2.hpp"

namespace mojibake {
    ///
    /// Class that converts constant string to new encoding:
    /// either small string_view, or a full string
    /// @warning  Uses quick-and-dirty conversion method,
    ///           use for reliable or low-importance strings
    ///
    template <typename ToC, typename FromC>
    class ConvString :
            protected detail::ConvStringProto<EquivChar<ToC>, FromC, detail::isAliasable<ToC, FromC>()>
    {
        using Super = detail::ConvStringProto<EquivChar<ToC>, FromC, detail::isAliasable<ToC, FromC>()>;
    public:
        using FromSv = std::basic_string_view<FromC>;
        using ToSv = std::basic_string_view<ToC>;

        /// The only ctor
        ConvString(FromSv x) : Super(x) {}

        /// Cannot build from temporary string
        template <class Trait, class Alloc>
        ConvString(std::basic_string<FromC, Trait, Alloc>&&) = delete;

        /// @return string’s length in characters
        using Super::length;

        /// @return string’s length in characters
        size_t size() const { return Super::length(); }

        /// @return string’s length in bytes
        size_t sizeBytes() const { return Super::length() * sizeof(ToC); }

        const ToC* data() const { return static_cast<const ToC*>(Super::data()); }

        /// Used for some low-level libs like ODBC w/o const correctness
        /// @warning   You are still not allowed to change that data
        ToC* nonConstData() { return static_cast<ToC*>(Super::nonConstData()); }

        /// Conversion to string view
        operator ToSv() const { return { data(), length() }; }
        ToSv sv() const { return { data(), length() }; }

        /// @return [+] is converted [-] is simple string view
        using Super::isConverted;

        /// @return the converse: [+] is simple string view [-] is converted
        static constexpr bool isStringView() { return !isConverted(); }
    };

}   // namespace mojibake


namespace std {

    template <class To, class From, class Enc>
    class iterator_traits<mojibake::AppendFuncIterator<To, From, Enc>>
    {
    public:
        using difference_type = int;
        using AFI = typename mojibake::AppendFuncIterator<To, From, Enc>;
        using value_type = typename AFI::value_type;
        using pointer = AFI*;
        using reference = AFI&;
        using iterator_category = forward_iterator_tag;
    };

    template <class Enc, class Func>
    class iterator_traits<mojibake::CallIterator<Enc, Func>>
    {
    public:
        using difference_type = int;
        using CI = typename mojibake::CallIterator<Enc, Func>;
        using value_type = typename CI::value_type;
        using pointer = CI*;
        using reference = CI&;
        using iterator_category = forward_iterator_tag;
    };

    template <class Func>
    class iterator_traits<mojibake::Utf8CallIterator<Func>>
    {
    public:
        using difference_type = int;
        using CI = typename mojibake::Utf8CallIterator<Func>;
        using value_type = typename CI::value_type;
        using pointer = CI*;
        using reference = CI&;
        using iterator_category = forward_iterator_tag;
    };

    template <class Func>
    class iterator_traits<mojibake::Utf16CallIterator<Func>>
    {
    public:
        using difference_type = int;
        using CI = typename mojibake::Utf16CallIterator<Func>;
        using value_type = typename CI::value_type;
        using pointer = CI*;
        using reference = CI&;
        using iterator_category = forward_iterator_tag;
    };

    template <class Func>
    class iterator_traits<mojibake::Utf32CallIterator<Func>>
    {
    public:
        using difference_type = int;
        using CI = typename mojibake::Utf32CallIterator<Func>;
        using value_type = typename CI::value_type;
        using pointer = CI*;
        using reference = CI&;
        using iterator_category = forward_iterator_tag;
    };

    template <class UnderIt>
    class iterator_traits<mojibake::LimitedIterator<UnderIt>>
    {
    public:
        using difference_type  = typename std::iterator_traits<UnderIt>::difference_type;
        using value_type       = typename std::iterator_traits<UnderIt>::value_type;
        using pointer          = typename std::iterator_traits<UnderIt>::pointer;
        using reference        = typename std::iterator_traits<UnderIt>::reference;
        using iterator_category = forward_iterator_tag;
    };

}   // namespace std
