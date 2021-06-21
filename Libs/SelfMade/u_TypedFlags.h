#ifndef U_TYPEDFLAGS_H
#define U_TYPEDFLAGS_H

#include <stdint.h>
#include <type_traits>

/// @warning
///    C++17 here! Most of common units use C++03, so cannot include
///    С++17 to C++03.

template <class>
struct FlagStorage {
    using Type = uint32_t;
};

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
    inline constexpr Flags() = default;
    explicit inline constexpr Flags(Storage x) : fValue(x) {}
    inline constexpr Flags(En x) : fValue(toStorage(x)) {}

    // helper function for variadic templates
    static inline constexpr Flags valueOf(En x) { return Flags<En>(x); }
    static inline constexpr Flags valueOf(Flags<En> x) { return x; }

    template <class First, class... Args>
    static inline constexpr Flags<En> valueOf(First x, Args... rest)
        { return valueOf(x) | valueOf(rest...); }

    template <En V>
    static inline constexpr Flags<En> valueOf() noexcept
        { return Flags<En>(V); }
    template <En First, En Second, En... Rest>
    static inline constexpr Flags<En> valueOf() noexcept
        { return Flags<En>(First) | valueOf<Second, Rest...>(); }

    void setNumeric(Storage x) noexcept { fValue = x; }

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
    constexpr inline Flags& operator |= (En x) { fValue |= toStorage(x); return *this; }
    constexpr inline Flags& operator |= (Flags<En> x) { fValue |= x.fValue; return *this; }

    // and
    constexpr inline Flags& operator &= (En x) { fValue &= toStorage(x); return *this; }
    constexpr inline Flags& operator &= (Flags<En> x) { fValue &= x.fValue; return *this; }

    // xor
    constexpr inline Flags& operator ^= (En x) { fValue ^= toStorage(x); return *this; }
    constexpr inline Flags& operator ^= (Flags<En> x) { fValue ^= x.fValue; return *this; }

    constexpr inline bool have(En x) const { return fValue & toStorage(x); }
    constexpr inline bool haveAny(Flags<En> x) const { return fValue & x.fValue; }
    constexpr inline bool haveAll(Flags<En> x) const { return (fValue & x.fValue) == x.fValue; }

    template<class... Args>
    constexpr inline bool haveAny(Args... flags) const { return haveAny(valueOf(flags...)); }
    template<class... Args>
    constexpr inline bool haveAll(Args... flags) const { return haveAll(valueOf(flags...)); }

    template<En... Args>
    constexpr inline bool haveAny() const { return haveAny(valueOf<Args...>()); }
    template<En... Args>
    constexpr inline bool haveAll() const { return haveAll(valueOf<Args...>()); }

    [[nodiscard]] constexpr inline Flags transformAny(Flags<En> from, Flags<En> to) const
    {
        if (haveAny(from))
            return minus(from) | to;
        return *this;
    }

    constexpr void remove(En x) { fValue &= ~toStorage(x); }
    constexpr void remove(Flags<En> x) { fValue &= ~x.fValue; }
    [[nodiscard]] constexpr Flags minus(Flags<En> x) const { return Flags(fValue & ~x.fValue); }
    constexpr void clear() { fValue = 0; }

    /// @return  0 or one flag — the smallest of them
    /// @warning If you suspect extraneous flags, use switch - default,
    ///          not switch — case NO_FLAG
    constexpr inline En smallest() const { return static_cast<En>(fValue & (-fValue)); }
    constexpr inline En smallestOf(Flags<En> x) const { return (*this & x).smallest(); }
    constexpr inline Flags& setIf(bool cond, En x) { if (cond) this->operator|=(x); return *this; }
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
