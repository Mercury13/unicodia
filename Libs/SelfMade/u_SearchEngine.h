#pragma once

#include <string>
#include <span>

#include "u_Vector.h"
#include "u_TypedFlags.h"

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

    Place findWord(std::span<HayWord> haystack, const NeedleWord& needle,
                   HaystackClass hclass);
    Prio findNeedle(std::span<HayWord> haystack, const Needle& needle,
                    HaystackClass hclass);
    Prio findNeedle(std::u8string_view haystack, const Needle& needle,
                    HaystackClass hclass);
    bool stringsCiEq(std::u8string_view s1, std::u8string_view s2);

}   // namespace srh
