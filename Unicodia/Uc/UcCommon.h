#pragma once

// Unicode
#include "UcAutoDefines.h"

// Libs
#include "u_TypedFlags.h"
#include "u_Array.h"

// STL
#include <string_view>

namespace uc {

    struct CoarseDate {
        short year;
        Month month;
        explicit operator bool() const { return (year != 0); }
    };

    enum class Vfg : unsigned char {
        TEXT = 1,
    };
    DEFINE_ENUM_OPS(Vfg)

    struct EmojiCounter
    {
        unsigned nNormal = 0, nSkintone = 0;

        EmojiCounter() = default;
        EmojiCounter(unsigned x) : nNormal(x) {}

        constexpr unsigned nTotal() const { return nNormal + nSkintone; }
        void add(bool isSkintone)
        {
            if (isSkintone) {
                ++nSkintone;
            } else {
                ++nNormal;
            }
        }
    };

    struct Version
    {
        std::u8string_view unicodeName;
        CoarseDate date;
        Flags<Vfg> flags;
        std::u8string_view emojiName {};
        CoarseDate otherEmojiDate { 0, Month::NUL };

        mutable struct Stats {
            struct Chars {
                unsigned nTotal = 0;
                unsigned nTransient = 0;
                struct Nw {
                    unsigned nHani = 0;
                    unsigned nNewScripts = 0;
                    unsigned nExistingScripts = 0;
                    unsigned nFormat = 0;  ///< Format w/o script
                    unsigned nSymbols = 0;
                    unsigned nTotal() const noexcept
                        { return nHani + nNewScripts + nExistingScripts + nFormat + nSymbols; }
                } nw;
            } chars;
            struct Emoji {
                struct Total {
                    unsigned n = 0;
                    unsigned nColor = 0;
                } total;
                /// New emoji
                struct Nw {
                    struct SingleChar {
                        unsigned nThisUnicode = 0;
                        unsigned nOldUnicode = 0;
                        bool areVs16Present = false;
                        unsigned nTotal() const noexcept { return nThisUnicode + nOldUnicode; }
                    } singleChar;
                    struct Zwj {
                        unsigned nMultiracial = 0, nFamilies = 0;
                        EmojiCounter right, color, gender, activity,
                                appearance, other;
                        unsigned nTotal() const noexcept
                            { return nMultiracial + nFamilies + right.nTotal() + color.nTotal()
                                    + gender.nTotal() + activity.nTotal() + other.nTotal(); }
                    } zwj;
                    struct Other {
                        unsigned nSingleSkintone = 0;
                        unsigned nNationalFlags = 0;
                        unsigned nSubdivisionFlags = 0;
                        unsigned nKeycaps = 0;
                        unsigned nTotal() const noexcept
                            { return nSingleSkintone + nNationalFlags + nSubdivisionFlags
                                   + nKeycaps; }
                    } other;
                    unsigned nTotal() const noexcept { return singleChar.nTotal() + zwj.nTotal() + other.nTotal(); }
                } nw;
            } emoji;
            struct Blocks {
                unsigned nNew = 0;
                bool wereExtended = false;  ///< [+] at least 1 block was extended
                bool wereShrunk = false;    ///< [+] at least 1 block was shrunk
            } blocks;
            struct Scripts {
                unsigned nNew = 0;
                unsigned nTotal = 0;
            } scripts;

            EcVersion thisEcVersion = EcVersion::NO_VALUE;
        } stats {};

        std::u8string locName() const;
        std::u8string locLongName() const;
        std::u8string techName() const;
        /// @return  name in Terms
        std::u8string termName() const;
        std::u8string link(std::u8string_view prefix) const;
        std::u8string link() const;
        bool isFirst() const noexcept { return (stats.chars.nw.nTotal() + stats.chars.nTransient == stats.chars.nTotal); }
        const CoarseDate& emojiDate() const noexcept
            { return otherEmojiDate ? otherEmojiDate : date; }
    };
    extern const Version versionInfo[];
    Buf1d<const Version> allVersions();
    const Version* findVersion(std::string_view id);

}   // namespace uc
