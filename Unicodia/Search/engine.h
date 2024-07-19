#pragma once

#include <string>
#include <span>

#include "u_Vector.h"
#include "u_TypedFlags.h"

#include "defs.h"

namespace srh {

    enum class Class { OTHER, LETTER, DIGIT };

    Class classify(char8_t x);

    enum class HaystackClass : unsigned char {
        NOWHERE = 0,        ///< Technical value
        SCRIPT = 1,         ///< Codepoint in script
        NONSCRIPT = 2,      ///< Codepoint outside script
        EMOJI = 4,          ///< Emoji
    };

    DEFINE_ENUM_OPS(HaystackClass)

    namespace hc {
        constexpr auto ALL_CPS = HaystackClass::NONSCRIPT | HaystackClass::SCRIPT;  ///< Codepoints regardless of script
        constexpr auto EVERYWHERE = HaystackClass::NONSCRIPT | HaystackClass::SCRIPT | HaystackClass::EMOJI;
    }

    enum class FindStatus : unsigned char {
        NONE,           ///< not found
        SUBSTR,         ///< found as substring
        INITIAL,        ///< found as prefix
        COMPLETE };     ///< found as complete match

    class Comparator
    {
    public:
        virtual void prepareHaystack(
                std::u8string_view haystack, std::u8string& result) const = 0;
        virtual srh::FindStatus find(
                std::u8string_view haystack, std::u8string_view needle) const = 0;
        virtual ~Comparator() = default;
    };

    struct NeedleWord {
        std::u8string v;
        Class ccFirst = Class::OTHER, ccLast = Class::OTHER;
        std::u8string_view dicWord;
        bool isDicWord = false;
        /// Interesting thing here: searching for “le” → avoid “letter”
        Flags<HaystackClass> lowPrioClass = HaystackClass::NOWHERE;

        NeedleWord() = default;
        NeedleWord(std::u8string x);

        std::u8string_view sv() const { return v; }
        size_t length() const { return v.length(); }
    };

    struct HayWord {
        std::u8string_view v;
        Flags<HaystackClass> lowPrioClass = HaystackClass::NOWHERE;

        HayWord() = default;
        HayWord(std::u8string_view x);

        std::u8string_view sv() const { return v; }
        size_t length() const { return v.length(); }
    };

    enum class Place { NONE, PARTIAL, INITIAL_SRIPT, INITIAL, EXACT_SCRIPT, EXACT };

    /// @brief
    ///   Just a normal T, but compares in reverse order
    /// @todo [future] Is Reverse object needed?
    template <class T>
    struct Reverse {
    private:
        using This = Reverse<T>;
    public:
        T value = 0;
        // Ctor
        constexpr Reverse() noexcept = default;
        constexpr Reverse(const This&) = default;
        constexpr Reverse(This&&) = default;
        template <class U>
                requires(!std::is_same_v<This, std::remove_cv<U>>)
            constexpr Reverse(U&& data)
                noexcept(std::is_nothrow_constructible_v<T, U>)
                    : value(std::forward<U>(data)) {}
        // Op=
        constexpr Reverse& operator = (const This& x) = default;
        constexpr Reverse& operator = (This&& x) = default;
        template <class U>
            constexpr Reverse& operator = (U&& data)
                noexcept(std::is_nothrow_assignable_v<T, U>)
                { value = std::forward<U>(data); return *this; }
        constexpr operator T& () noexcept { return value; }
        constexpr operator T&& () && noexcept { return value; }
        operator const T& () const noexcept { return value; }
        constexpr bool operator == (const This& x) const = default;
        constexpr auto operator <=> (const This& x) const
            noexcept(noexcept(x.value <=> this->value))
                { return (x.value <=> this->value); }
    };

    struct Prio {
        short high = 0;
        unsigned short exact = 0, exactScript = 0,
                       initial = 0, initialScript = 0,
                       partial = 0;
        std::partial_ordering operator <=>(const Prio& x) const = default;
        static const Prio EMPTY;
    };

    ///
    /// \brief The Needle struct
    ///    Compiled “needle” for searching
    ///    Avoid relying on any structs inside
    ///
    struct Needle
    {
        SafeVector<srh::NeedleWord> words;

        Needle(std::u8string_view x);
    };

    struct Cache {
        std::u8string haystack;
        SafeVector<std::u8string_view> words1;
        SafeVector<HayWord> words2;
    };

    Place findWord(std::span<HayWord> haystack, const NeedleWord& needle,
                   HaystackClass hclass, const Comparator& comparator);
    Prio findNeedle(std::span<HayWord> haystack, const Needle& needle,
                    HaystackClass hclass, const Comparator& comparator);
    Prio findNeedle(std::u8string_view haystack, const Needle& needle,
                    HaystackClass hclass, Cache& cache, const Comparator& comparator);
    bool stringsCiEq(std::u8string_view s1, std::u8string_view s2);

    class DefaultComparator : public Comparator
    {
    public:
        void prepareHaystack(
                std::u8string_view haystack, std::u8string& result) const override;
        /// final just for optimization, w/o logical reason
        srh::FindStatus find(
                std::u8string_view haystack, std::u8string_view needle) const final;
        static const DefaultComparator INST;
    };

}   // namespace srh
