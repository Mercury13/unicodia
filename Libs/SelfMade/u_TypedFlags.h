#ifndef U_TYPEDFLAGS_H
#define U_TYPEDFLAGS_H

#include <stdint.h>
#include <type_traits>

/// @warning
///    C++17 here! Most of common units use C++03, so cannot include
///    С++17 to C++03.

template <class T>
struct FlagStorage {
    using Type = typename std::make_unsigned<
                typename std::underlying_type<T>::type
                                            >::type;
};

enum class NoFlags { INST };
constexpr NoFlags NO_FLAGS = NoFlags::INST;

template <class En>
class Flags
{
public:
    using Storage = typename FlagStorage<En>::Type;
    using SignedStorage = typename std::make_signed<Storage>::type;
    static inline constexpr Storage toStorage(En x) { return static_cast<Storage>(x); }
    static inline constexpr Storage toSignedStorage(En x) { return static_cast<SignedStorage>(x); }
    static inline constexpr Storage toSignedStorage(bool x) { return static_cast<SignedStorage>(x); }

    // ctor
    constexpr Flags() = default;
    constexpr Flags(NoFlags) {}
    explicit constexpr Flags(Storage x) : fValue(x) {}
    constexpr Flags(En x) : fValue(toStorage(x)) {}

    // helper function for variadic templates
    static constexpr Flags valueOf(En x) { return Flags<En>(x); }
    static constexpr Flags valueOf(Flags<En> x) { return x; }

    template <class First, class... Args>
    static constexpr Flags<En> valueOf(First x, Args... rest)
        { return valueOf(x) | valueOf(rest...); }

    template <En V>
    static constexpr Flags<En> valueOf() noexcept
        { return Flags<En>(V); }
    template <En First, En Second, En... Rest>
    static constexpr Flags<En> valueOf() noexcept
        { return Flags<En>(First) | valueOf<Second, Rest...>(); }

    static constexpr int shiftOf(En x)
    {
        auto t = static_cast<Storage>(x);
        if (t == 0)
            return 0;
        int r = -1;
        while (t != 0) {
            ++r;
            t >>= 1;
        }
        return r;
    }

    void setNumeric(Storage x) noexcept { fValue = x; }

    ///  Converts flag Flag to custom enum-class boolean (0/1)
    template <class En2, En Flag>
    constexpr En2 toCustomBool() const
    {
        static_assert (static_cast<Storage>(Flag) != 0);
        constexpr auto SHIFT = shiftOf(Flag);
        if constexpr (SHIFT == 0) {
            return static_cast<En2>(fValue & static_cast<Storage>(Flag));
        } else {
            return static_cast<En2>((fValue & static_cast<Storage>(Flag)) >> SHIFT);
        }
    }

    // bool
    constexpr operator bool() const { return fValue; }
    operator char() const = delete;
    operator unsigned char() const = delete;
    operator short() const = delete;
    operator unsigned short() const = delete;
    operator int() const = delete;
    operator unsigned int() const = delete;
    operator long() const = delete;
    operator unsigned long() const = delete;
    operator long long() const = delete;
    operator unsigned long long() const = delete;
    operator float() const = delete;
    operator double() const = delete;
    [[deprecated("Use numeric()")]] constexpr Storage value() const { return fValue; }
    constexpr Storage numeric() const { return fValue; }

    // or
    constexpr Flags& operator |= (En x) { fValue |= toStorage(x); return *this; }
    constexpr Flags& operator |= (Flags<En> x) { fValue |= x.fValue; return *this; }

    // and
    constexpr Flags& operator &= (En x) { fValue &= toStorage(x); return *this; }
    constexpr Flags& operator &= (Flags<En> x) { fValue &= x.fValue; return *this; }

    // xor
    constexpr Flags& operator ^= (En x) { fValue ^= toStorage(x); return *this; }
    constexpr Flags& operator ^= (Flags<En> x) { fValue ^= x.fValue; return *this; }

    constexpr bool have(En x) const { return fValue & toStorage(x); }
    constexpr bool haveAny(Flags<En> x) const { return fValue & x.fValue; }
    constexpr bool haveAll(Flags<En> x) const { return (fValue & x.fValue) == x.fValue; }

    template<class... Args>
    constexpr bool haveAny(Args... flags) const { return haveAny(valueOf(flags...)); }
    template<class... Args>
    constexpr bool haveAll(Args... flags) const { return haveAll(valueOf(flags...)); }

    template<En... Args>
    constexpr bool haveAny() const { return haveAny(valueOf<Args...>()); }
    template<En... Args>
    constexpr bool haveAll() const { return haveAll(valueOf<Args...>()); }

    /// @return  if we have any of FROM flags → remove them all, add TO flags
    ///          otherwise no change
    [[nodiscard]] constexpr inline Flags transformAny(Flags<En> from, Flags<En> to) const
    {
        if (haveAny(from))
            return minus(from) | to;
        return *this;
    }

    constexpr void remove(En x) { fValue &= ~toStorage(x); }
    constexpr void remove(Flags<En> x) { fValue &= ~x.fValue; }

    /// @return  value w/o x
    [[nodiscard]] constexpr Flags minus(Flags<En> x) const { return Flags(fValue & ~x.fValue); }

    constexpr void clear() { fValue = 0; }

    /// @return  0 or one flag — the smallest of them
    /// @warning If you suspect extraneous flags, use switch - default,
    ///          not switch — case NO_FLAG
    constexpr En smallest() const { return static_cast<En>(fValue & (-fValue)); }

    /// @return  0 or one flag — the smallest of X that’s set in this
    constexpr En smallestOf(Flags<En> x) const { return (*this & x).smallest(); }

    constexpr Flags& setIf(bool cond, En x) { if (cond) this->operator|=(x); return *this; }
    constexpr bool holdsValue() const noexcept { return operator bool(); }
    constexpr bool empty() const noexcept { return !holdsValue(); }

private:
    Storage fValue = 0;
};


template <class En>
constexpr inline Flags<En> operator ~ (Flags<En> x) { return Flags<En>(~x.numeric()); }

template <class En>
constexpr inline Flags<En> operator | (Flags<En> x, Flags<En> y) { return Flags<En>(x.numeric() | y.numeric()); }

template <class En>
constexpr inline Flags<En> operator | (Flags<En> x, En y) { return Flags<En>(x.numeric() | Flags<En>::toStorage(y)); }

template <class En>
constexpr inline Flags<En> operator | (En x, Flags<En> y) { return Flags<En>::toStorage(x) | Flags<En>(y.numeric()); }

template <class En>
constexpr inline Flags<En> operator & (Flags<En> x, Flags<En> y) { return Flags<En>(x.numeric() & y.numeric()); }

template <class En>
constexpr inline Flags<En> operator & (Flags<En> x, En y) { return Flags<En>(x.numeric() & Flags<En>::toStorage(y)); }

template <class En>
constexpr inline Flags<En> operator & (En x, Flags<En> y) { return Flags<En>::toStorage(x) & Flags<En>(y.numeric()); }

template <class En>
constexpr inline Flags<En> operator ^ (Flags<En> x, Flags<En> y) { return Flags<En>(x.numeric() ^ y.numeric()); }

template <class En>
constexpr inline Flags<En> operator ^ (Flags<En> x, En y) { return Flags<En>(x.numeric() ^ Flags<En>::toStorage(y)); }

template <class En>
constexpr inline Flags<En> operator ^ (En x, Flags<En> y) { return Flags<En>::toStorage(x) ^ Flags<En>(y.numeric()); }

template <class En>
constexpr inline bool operator == (Flags<En> x, En y) { return (x.numeric() == Flags<En>::toStorage(y)); }

template <class En>
constexpr inline bool operator == (En x, Flags<En> y) { return (Flags<En>::toStorage(x) == y.numeric()); }

template <class En>
constexpr inline bool operator == (Flags<En> x, Flags<En> y) { return (x.numeric() == y.numeric()); }

template <class En>
constexpr inline bool operator != (Flags<En> x, En y) { return (x.numeric() != Flags<En>::toStorage(y)); }

template <class En>
constexpr inline bool operator != (En x, Flags<En> y) { return (Flags<En>::toStorage(x) != y.numeric()); }

template <class En>
constexpr inline bool operator != (Flags<En> x, Flags<En> y) { return (x.numeric() != y.numeric()); }

template <class En>
constexpr inline Flags<En> flagIf(bool x, En y)
    { return Flags<En>( Flags<En>::toStorage(y)
                & static_cast<typename Flags<En>::Storage>(
                    -Flags<En>::toSignedStorage(x))
                // A small hack: 0/1 → 0/FFFF
                      ); }

// AND and XOR are unneded for obvious reasons
#define DEFINE_ENUM_OPS(En)  \
    [[maybe_unused]] constexpr inline Flags<En> operator | (En x, En y) { return Flags<En>(x) | y; }  \
    [[maybe_unused]] constexpr inline Flags<En> operator ~ (En x) { return Flags<En>(~Flags<En>::toStorage(x)); }

#endif // U_TYPEDFLAGS_H
