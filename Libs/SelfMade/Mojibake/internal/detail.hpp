#pragma once

#if __cplusplus >= 202002L
    #define CPP20_LIKELY [[likely]]
    #define CPP20_UNLIKELY [[unlikely]]
#else
    #define CPP20_LIKELY
    #define CPP20_UNLIKELY
#endif

#include <iterator> // won’t be included actually
#include <limits>   // won’t be included actually

namespace detail {

    template <class It>
    using ChType = typename std::iterator_traits<It>::value_type;

    template <class Ch>
    class UtfTraits;

    template <size_t Len>
    class LenTraits;

    template <class It>
    using ItUtfTraits = UtfTraits<ChType<It>>;

    template <class Cont>
    using ContUtfTraits = UtfTraits<typename Cont::value_type>;

    ///// LenTraits ////////////////////////////////////////////////////////////

    template<>
    class LenTraits<1> {
    public:
        using Enc = Utf8;
    };

    template<>
    class LenTraits<2> {
    public:
        using Enc = Utf16;
    };

    template<>
    class LenTraits<4> {
    public:
        using Enc = Utf32;
    };

    template<>
    class LenTraits<8> {
    public:
        using Enc = Utf32;
    };

    ///// UtfTraits ////////////////////////////////////////////////////////////

    template<>
    class UtfTraits<char> {
    public:
        using Enc = Utf8;
    };

    template<>
    class UtfTraits<signed char> {
    public:
        using Enc = Utf8;
    };

    template<>
    class UtfTraits<unsigned char> {
    public:
        using Enc = Utf8;
    };

    #if __cplusplus >= 202002L
        template<>
        class UtfTraits<char8_t> {
        public:
            using Enc = Utf8;
        };
    #endif

    template<>
    class UtfTraits<char16_t> {
    public:
        using Enc = Utf16;
    };

    template<>
    class UtfTraits<signed short> {
    public:
        using Enc = Utf16;
    };

    template<>
    class UtfTraits<unsigned short> {
    public:
        using Enc = Utf16;
    };

    template<>
    class UtfTraits<signed int> {
    public:
        using Enc = typename LenTraits<sizeof(int)>::Enc;
    };

    template<>
    class UtfTraits<unsigned int> {
    public:
        using Enc = typename LenTraits<sizeof(unsigned int)>::Enc;
    };

    template<>
    class UtfTraits<char32_t> {
    public:
        using Enc = Utf32;
    };

    template<>
    class UtfTraits<long> {
    public:
        using Enc = Utf32;
    };

    template<>
    class UtfTraits<unsigned long> {
    public:
        using Enc = Utf32;
    };

    template<>
    class UtfTraits<long long> {
    public:
        using Enc = Utf32;
    };

    template<>
    class UtfTraits<unsigned long long> {
    public:
        using Enc = Utf32;
    };

    template<>
    class UtfTraits<wchar_t>
    {
    public:
        using Enc = typename LenTraits<sizeof(wchar_t)>::Enc;
    };

    ///// ItEnc ////////////////////////////////////////////////////////////////

    ///
    ///  Iterator + encoding
    ///
    template <class It, class Enc>
    class ItEnc;

    template <class It>
    class ItEnc<It, Utf32>
    {
    public:
        static inline void put(It& it, char32_t cp)
            noexcept (noexcept(*it = cp) && noexcept (++it))
        {
            *it = cp;
            ++it;
        }

        template <class It2, class Enc2, class Mjh>
        static inline It2 copy(It p, It end, It2 dest, const Mjh& onMojibake);

        static bool isValid(It p, It end);
    };

    /// @return [+] halt
    template <class Enc2, class It1, class It2, class Mjh>
    bool handleMojibake(It1 ptr, Event event, It2& dest, const Mjh& onMojibake)
    {
        auto result = onMojibake(ptr, event);
        bool halt = result & handler::FG_HALT;
        result &= handler::MASK_CODE;
        if (result != handler::RET_SKIP)
            ItEnc<It2, Enc2>::put(dest, result);
        return halt;
    }

    template <class Enc2, class It1, class It2>
    inline bool handleMojibake(It1, Event, It2&,
            const typename handler::Skip<It1>&) noexcept
        { return false; }

    template <class Enc2, class It1, class It2>
    inline bool handleMojibake(
            It1 ptr, Event event, It2& dest,
            const typename handler::Moji<It1>& onMojibake) noexcept
    {
        auto result = onMojibake(ptr, event);
        ItEnc<It2, Enc2>::put(dest, result);
        return false;
    }

    template <class It>
    bool ItEnc<It, Utf32>::isValid(It p, It end)
    {
        for (; p != end; ++p) {
            char32_t c = *p;
            if (!mojibake::isValid(c))
                return false;
        }
        return true;
    }

    template <class It> template <class It2, class Enc2, class Mjh>
    inline It2 ItEnc<It, Utf32>::copy(It p, It end, It2 dest, const Mjh& onMojibake)
    {
        for (; p != end; ++p) {
            char32_t c = *p;
            if (mojibake::isValid(c)) CPP20_LIKELY {
                ItEnc<It2, Enc2>::put(dest, c);
            } else CPP20_UNLIKELY {
                if (handleMojibake<Enc2>(p, Event::CODE, dest, onMojibake))
                    break;
            }
        }
        return dest;
    }

    template <class It>
    class ItEnc<It, Utf16>
    {
    public:
        static void put(It& it, char32_t cp)
                noexcept (noexcept(*it = cp) && noexcept (++it));

        template <class It2, class Enc2, class Mjh>
        static It2 copy(It p, It end, It2 dest, const Mjh& onMojibake);

        static bool isValid(It p, It end);
    };

    template <class It>
    void ItEnc<It, Utf16>::put(It& it, char32_t cp)
            noexcept (noexcept(*it = cp) && noexcept (++it))
    {
        if (cp < U16_2WORD_MIN) CPP20_LIKELY {   // 1 word
            *(it) = static_cast<wchar_t>(cp);
            ++it;
        } else if (cp <= U16_2WORD_MAX) CPP20_UNLIKELY { // 2 words
            cp -= U16_2WORD_MIN;
            const wchar_t lo10 = cp & 0x3FF;
            const wchar_t hi10 = cp >> 10;
            *it = static_cast<wchar_t>(0xD800 | hi10);
            ++it;
            *it = static_cast<wchar_t>(0xDC00 | lo10);
            ++it;
        }
    }

    template <class It> template <class It2, class Enc2, class Mjh>
    It2 ItEnc<It, Utf16>::copy(It p, It end, It2 dest, const Mjh& onMojibake)
    {
        for (; p != end;) {
            auto cpStart = p++;
            char16_t word1 = *cpStart;
            if (word1 < SURROGATE_HI_MIN) CPP20_LIKELY {
                if (word1 < SURROGATE_MIN) { // Low BMP char => OK
                    ItEnc<It2, Enc2>::put(dest, word1);
                } else {  // Leading surrogate
                    if (p == end) CPP20_UNLIKELY {
                        // Abrupt end — break always
                        handleMojibake<Enc2>(cpStart, Event::END, dest, onMojibake);
                        break;
                    } else {
                        char16_t word2 = *p;
                        if (word2 < SURROGATE_HI_MIN || word2 > SURROGATE_HI_MAX)
                        CPP20_UNLIKELY {
                            if (handleMojibake<Enc2>(p, Event::BYTE_NEXT, dest, onMojibake))
                                break;
                            // DO NOT increment if BYTE_NEXT, the byte may come in handy
                        } else CPP20_LIKELY {
                            ++p;
                            char32_t cp = (((word1 & 0x3FF) << 10) | (word2 & 0x3FF)) + 0x10000;
                            ItEnc<It2, Enc2>::put(dest, cp);
                        }
                    }
                }
            } else {
                if (word1 <= SURROGATE_MAX) CPP20_UNLIKELY { // Trailing surrogate
                    if (handleMojibake<Enc2>(cpStart, Event::BYTE_START, dest, onMojibake))
                        break;
                } else { // High BMP char => OK
                    ItEnc<It2, Enc2>::put(dest, word1);
                }
            }   // big if
        }   // for
        return dest;
    }

    template <class It>
    bool ItEnc<It, Utf16>::isValid(It p, It end)
    {
        for (; p != end;) {
            auto cpStart = p++;
            char16_t word1 = *cpStart;
            if (word1 < SURROGATE_HI_MIN) CPP20_LIKELY {
                if (word1 < SURROGATE_MIN) { // Low BMP char => OK
                    // OK, do nothing
                } else {  // Leading surrogate
                    if (p == end) CPP20_UNLIKELY {
                        return false;
                    } else {
                        char16_t word2 = *p;
                        if (word2 < SURROGATE_HI_MIN || word2 > SURROGATE_HI_MAX)
                        CPP20_UNLIKELY {
                            return false;
                        } else CPP20_LIKELY {
                            ++p;
                        }
                    }
                }
            } else {
                if (word1 <= SURROGATE_MAX) CPP20_UNLIKELY { // Trailing surrogate
                    return false;
                } else { // High BMP char => OK
                    // do nothing
                }
            }   // big if
        }   // for
        return true;
    }

    template <class It>
    class ItEnc<It, Utf8>
    {
    public:
        static void put(It& it, char32_t cp)
                noexcept (noexcept(*it = cp) && noexcept (++it));

        template <class It2, class Enc2, class Mjh>
        static inline It2 copy(It p, It end, It2 dest, const Mjh& onMojibake);

        static bool isValid(It p, It end);
    };

    template <class It>
    void ItEnc<It, Utf8>::put(It& it, char32_t cp)
            noexcept (noexcept(*it = cp) && noexcept (++it))
    {
        if (cp <= U8_2BYTE_MAX) CPP20_LIKELY {  // 1 or 2 bytes, the most frequent case
            if (cp <= U8_1BYTE_MAX) {  // 1 byte
                *it = cp;  ++it;
            } else { // 2 bytes
                *it     = (cp >> 6)   | 0xC0;
                *(++it) = (cp & 0x3F) | 0x80;  ++it;
            }
        } else {  // 3 or 4 bytes
            if (cp <= U8_3BYTE_MAX) {  // 3 bytes
                *it     =  (cp >> 12)        | 0xE0;
                *(++it) = ((cp >> 6) & 0x3F) | 0x80;
                *(++it) =  (cp       & 0x3F) | 0x80;  ++it;
            } else {    // 4 bytes
                *it     = ((cp >> 18) & 0x07) | 0xF0;
                *(++it) = ((cp >> 12) & 0x3F) | 0x80;
                *(++it) = ((cp >> 6)  & 0x3F) | 0x80;
                *(++it) =  (cp        & 0x3F) | 0x80; ++it;
            }
        }
    }

    constexpr int fallbackCount1(unsigned char x)
    {
        static_assert(std::numeric_limits<unsigned char>::max() == 255);
#define ROW(x) x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x
        constexpr char table[256] = {
            ROW(0), ROW(0), ROW(0), ROW(0),
            ROW(0), ROW(0), ROW(0), ROW(0),
            ROW(1), ROW(1), ROW(1), ROW(1),
            ROW(2), ROW(2), ROW(3),
            4,4,4,4, 4,4,4,4, 5,5,5,5, 6,6,7,8
        };
#undef ROW
        return table[x];
    }

#if __cplusplus >= 202002L
    // C++20: use standard bit library
    constexpr int count1(unsigned char x)
        { return std::countl_one(x); }
#elif defined(__GNUC__) || defined (__clang__)
    // G++/clang: re-implement using __builtin_clz; exists long before C++17
    constexpr int count1(unsigned char x)
    {
        constexpr auto ndInt  = std::numeric_limits<unsigned int> ::digits;
        constexpr auto ndChar = std::numeric_limits<unsigned char>::digits;
        constexpr auto diff = ndInt - ndChar;
        return __builtin_clz(x ^ 0xFF) - diff;
    }
#else
    // Fallback
    constexpr int count1(unsigned char x)
        { return fallbackCount1(x); }
#endif

    constexpr bool isCont(unsigned char b) { return (b & 0xC0) == 0x80; }

    // We need this for current UTF-8 reading
    static_assert(std::numeric_limits<unsigned char>::radix == 2);
    static_assert(std::numeric_limits<unsigned char>::digits == 8);

    template <class It> template <class It2, class Enc2, class Mjh>
    inline It2 ItEnc<It, Utf8>::copy(It p, It end, It2 dest, const Mjh& onMojibake)
    {
    #define MJ_READCP \
                if (p == end) goto abruptEnd; \
                byte2 = *p;  \
                if (!isCont(byte2)) goto badNext; \
                ++p;

        for (; p != end;) {
            auto cpStart = p++;
            unsigned char byte1 = *cpStart;
            unsigned char byte2;
            char32_t cp;
            switch (count1(byte1)) {
            case 0:  // 0###.#### = 1 byte
                ItEnc<It2, Enc2>::put(dest, byte1);
                break;
            // 1 is default!!
            case 2:  // 110#.#### = 2 bytes
                // Cannot check byte1 here → first read all bytes and ensure
                // that it looks like two-byte code seq, then check
                cp = (byte1 & 0x1F) << 6;
                MJ_READCP;  cp |= (byte2 & 0x3F);
                if (cp < U8_2BYTE_MIN)
                    goto badCode;
                ItEnc<It2, Enc2>::put(dest, cp);
                break;
            case 3: // 1110.#### = 3 bytes
                cp = (byte1 & 0x0F) << 12;
                MJ_READCP;  cp |= ((byte2 & 0x3F) << 6);
                MJ_READCP;  cp |=  (byte2 & 0x3F);
                if ((cp >= SURROGATE_MIN && cp <= SURROGATE_MAX) || cp < U8_3BYTE_MIN)
                    goto badCode;
                ItEnc<It2, Enc2>::put(dest, cp);
                break;
            case 4: // 1111.0### = 4 bytes
                cp = (byte1 & 0x07) << 18;
                MJ_READCP;  cp |= ((byte2 & 0x3F) << 12);
                MJ_READCP;  cp |= ((byte2 & 0x3F) << 6);
                MJ_READCP;  cp |=  (byte2 & 0x3F);
                if (cp < U8_4BYTE_MIN || cp > UNICODE_MAX)
                    goto badCode;
                ItEnc<It2, Enc2>::put(dest, cp);
                break;
            default: CPP20_UNLIKELY {
                    if (handleMojibake<Enc2>(cpStart, Event::BYTE_START, dest, onMojibake))
                        goto brk;
                    // Skip all continuation bytes
                    while (p != end && isCont(*p))
                        ++p;
                } break;
            abruptEnd: CPP20_UNLIKELY {
                    if (handleMojibake<Enc2>(cpStart, Event::END, dest, onMojibake))
                        goto brk;
                } break;
            badNext: CPP20_UNLIKELY {
                    if (handleMojibake<Enc2>(p, Event::BYTE_NEXT, dest, onMojibake))
                        goto brk;
                } break;
            badCode: CPP20_UNLIKELY {
                    if (handleMojibake<Enc2>(cpStart, Event::CODE, dest, onMojibake))
                        goto brk;
                } break;
            }   // switch
        }
    brk:
        return dest;

    #undef MJ_READCP
    }

    template <class It>
    bool ItEnc<It, Utf8>::isValid(It p, It end)
    {
#define MJ_READCP \
            if (p == end) return false; \
            byte1 = *p;  \
            if (!isCont(byte1)) return false; \
            ++p;

        for (; p != end;) {
            auto cpStart = p++;
            unsigned char byte1 = *cpStart;
            unsigned cp;
            switch (count1(byte1)) {
            case 0:  // 0###.#### = 1 byte
                break;
            // 1 is default!!
            case 2:  // 110#.#### = 2 bytes
                // 110#.####  10##.####,  put 80 = 1000.0000 →
                // ___0.0010  __00.0000
                // byte1 < 1100.0010 = C2 → BAD
                if (byte1 < 0xC2)
                    return false;
                MJ_READCP;
                break;
            case 3: // 1110.#### = 3 bytes
                // E0 A0 80 = 800  (1st 3-byte)
                // ED A0 80 = D800 (1st surrogate)
                // ED BF BF = DFFF (last surrogate)
                cp = byte1 << 8;
                MJ_READCP;  cp |= byte1;
                if ((cp >= 0xEDA0 && cp <= 0xEDBF) || cp < 0xE0A0)
                    return false;
                MJ_READCP;
                break;
            case 4: // 1111.0### = 4 bytes
                // F0 90 80 80 = 1'0000  (1st 4-byte)
                // F4 8F BF BF = 10'FFFF (last Unicode)
                cp = byte1 << 8;
                MJ_READCP;  cp |= byte1;
                if (cp < 0xF090 || cp > 0xF48F)
                    return false;
                MJ_READCP;
                MJ_READCP;
                break;
            default:
                return false;
            }   // switch
        }
        return true;

#undef MJ_READCP
    }

}   // namespace detail
