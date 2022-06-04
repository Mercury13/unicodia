#pragma once

///
///  MOJIBAKE
///    simple C++17 Unicode conversion library
///
///  LICENSE: MIT
///

#include <algorithm>
#include <iterator>
#include <limits>

#if __cplusplus >= 202002L
    #include <bit>
#endif

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

    class Utf8  { public: using Ch = char; };
    class Utf16 { public: using Ch = char16_t; };
    class Utf32 { public: using Ch = char32_t; };

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

    #include "internal/detail.hpp"

    ///
    /// Puts code point to some iterator
    /// @tparam  It   iterator
    /// @tparam  Enc  enc::Utf8, enc::Utf16, etc::Utf32
    /// @param [in,out]  it   iterator
    /// @param [in]      cp   code point, SHOULD BE GOOD
    /// @warning  Bhv on bad cp is implementation-specific
    ///
    template <class It,
              class Enc = typename detail::ItUtfTraits<It>::Enc,
              class = std::void_t<typename std::iterator_traits<It>::value_type>>
    inline void put(It& it, char32_t cp)
        { detail::ItEnc<It, Enc>::put(it, cp); }

    template <class Enc,
              class It,
              class = std::void_t<typename std::iterator_traits<It>::value_type>>
    inline void put(It& it, char32_t cp)
        { detail::ItEnc<It, Enc>::put(it, cp); }


    ///
    /// Copies data to another,
    /// REACTING APPROPRIATELY TO MOJIBAKE
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

    template <class Enc1, class Enc2,  class Mjh, class It1, class It2,
              class = std::void_t<typename std::iterator_traits<It1>::value_type>,
              class = std::void_t<typename std::iterator_traits<It2>::value_type>>
    inline It2 copy(It1 beg, It1 end, It2 dest, const Mjh& onMojibake = Mjh{})
    {
        return copy<It1, It2, Enc1, Enc2, Mjh>(beg, end, dest, onMojibake);
    }

    template <class Enc2, class Mjh, class It1, class It2, class Enc1,
              class = std::void_t<typename std::iterator_traits<It1>::value_type>,
              class = std::void_t<typename std::iterator_traits<It2>::value_type>>
    inline It2 copy(It1 beg, It1 end, It2 dest, const Mjh& onMojibake = Mjh{})
    {
        return copy<It1, It2, Enc1, Enc2, Mjh>(beg, end, dest, onMojibake);
    }

    ///
    /// Copies data to another, SKIPPING MOJIBAKE
    /// So mo mojibake handler
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

    template <class Enc2, class It1, class It2, class Enc1,
              class = std::void_t<typename std::iterator_traits<It1>::value_type>,
              class = std::void_t<typename std::iterator_traits<It2>::value_type>>
    inline It2 copyS(It1 beg, It1 end, It2 dest)
    {
        return copyS<It1, It2, Enc1, Enc2>(beg, end, dest);
    }

    ///
    /// Copies data to another, WRITING MOJIBAKE
    /// So mo mojibake handler
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

    template <class Enc2, class It1, class It2, class Enc1,
              class = std::void_t<typename std::iterator_traits<It1>::value_type>,
              class = std::void_t<typename std::iterator_traits<It2>::value_type>>
    inline It2 copyM(It1 beg, It1 end, It2 dest)
    {
        return copyM<It1, It2, Enc1, Enc2>(beg, end, dest);
    }

    ///
    /// Copies data to another, WRITING MOJIBAKE AND HALTING
    /// So mo mojibake handler
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
        return copyM<It1, It2, Enc1, Enc2>(beg, end, dest);
    }

    template <class Enc2, class It1, class It2, class Enc1,
              class = std::void_t<typename std::iterator_traits<It1>::value_type>,
              class = std::void_t<typename std::iterator_traits<It2>::value_type>>
    inline It2 copyMH(It1 beg, It1 end, It2 dest)
    {
        return copyM<It1, It2, Enc1, Enc2>(beg, end, dest);
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

#define MOJIBAKE_IMPLEMENT_TO_X_1(Name, Char, Encoding1) \
        template <class To,   \
                  class Enc = typename detail::ContUtfTraits<To>::Enc,  \
                  class Enc1 = Encoding1> \
        inline To Name(const Char* from) { \
            std::basic_string_view from1{from};  \
            return Name<To>(from1);  \
        }

#if __cplusplus >= 202002L
    #define MOJIBAKE_IMPLEMENT_TO_X_ALL(Name) \
        MOJIBAKE_IMPLEMENT_TO_X_1(Name, char, Utf8) \
        MOJIBAKE_IMPLEMENT_TO_X_1(Name, char8_t, Utf8) \
        MOJIBAKE_IMPLEMENT_TO_X_1(Name, char16_t, Utf16) \
        MOJIBAKE_IMPLEMENT_TO_X_1(Name, wchar_t, typename detail::UtfTraits<wchar_t>::Enc) \
        MOJIBAKE_IMPLEMENT_TO_X_1(Name, char32_t, Utf32)
#else
    #define MOJIBAKE_IMPLEMENT_TO_X_ALL(Name) \
        MOJIBAKE_IMPLEMENT_TO_X_1(Name, char, Utf8) \
        MOJIBAKE_IMPLEMENT_TO_X_1(Name, char16_t, Utf16) \
        MOJIBAKE_IMPLEMENT_TO_X_1(Name, wchar_t, typename detail::UtfTraits<wchar_t>::Enc) \
        MOJIBAKE_IMPLEMENT_TO_X_1(Name, char32_t, Utf32)
#endif


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

    // Implement toS for char’s
    MOJIBAKE_IMPLEMENT_TO_X_ALL(toS)

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

    // Implement toM for char’s
    MOJIBAKE_IMPLEMENT_TO_X_ALL(toM)

    ///
    /// Check for string validity
    ///
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
        using CallIterator<Utf8, Func>::CallIterator;
    };

    template <class Func>
    Utf16CallIterator(const Func&) -> Utf16CallIterator<Func>;

    template <class Func>
    class Utf32CallIterator : public CallIterator<Utf32, Func>
    {
    public:
        using CallIterator<Utf8, Func>::CallIterator;
    };

    template <class Func>
    Utf32CallIterator(const Func&) -> Utf32CallIterator<Func>;

//    template <class To, class From>
//    To to(const From& from);

}   // namespace mojibake


namespace std {

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

}   // namespace std
