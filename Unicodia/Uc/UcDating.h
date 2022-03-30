#pragma once

#include <string>
#include <vector>

namespace uc {

    enum class CrangeMode {
        ARABIC_SPECIAL,         // English: 12-13th century
        SPECIAL_SPECIAL };      // Russian: XII-XIII век

    struct DatingLoc
    {
        static constexpr int MAX_CENTURY = 20;
        using Cnames = std::u8string_view[MAX_CENTURY + 1];

        // Year
                // 1234 not localized
        std::u8string_view yBc;         ///< 1234 BC
        std::u8string_view yBefore;     ///< before 1234, never BC
                // 1234–1357 not localized
        std::u8string_view yApprox;     ///< ≈1234
        std::u8string_view yApproxBc;   ///< ≈1234 BC

        // Decade
        std::u8string_view decade;      ///< 1230s, never BC

        // Century
        Cnames centuryNames;            ///< 0 does not matter
        std::u8string_view century;     ///< 12th century
        std::u8string_view centuryBc;   ///< 12th century BC
        CrangeMode crangeMode;
                // crange uses century/centuryBc
        //std::u8string_view crangeBcCe;   ///< 2nd century BC - 3rd century CE;  unused
        std::u8string_view capprox;     ///< ≈12th century
        std::u8string_view capproxBc;   ///< ≈12th century BC
                // Before 12th century is Special

        std::u8string_view formatCentury(int x) const;
        std::u8string_view format1stCentury(int x, char* start, char* end) const;
        template <size_t N>
          std::u8string_view format1stCentury(int x, char(&buf)[N]) const
            { return format1stCentury(x, std::begin(buf), std::end(buf)); }
    };

    ///
    ///  Used for i18n: dates are written in code, only notes are localized
    ///
    class Dating
    {
    public:
        enum class Mode {
            UNKNOWN,    ///<  ???
            // Year
            YEAR,       ///<  1234
            YBEFORE,    ///<  before 1234
            YRANGE,     ///<  1234-1357
            YAPPROX,    ///<  ≈1234
            // Decade
            DECADE,     ///<  1230s
            //DEND,       ///<  end of 1230s
            // Century
            CENTURY,    ///<  12th century
            CRANGE,     ///<  12–13th century
            CAPPROX,    ///<  ≈12th century
            // Special
            SPECIAL,    ///<  2nd century (main hypothesis), 7th century (for sure)
        };
        int value1() const { return fValue1; }
        int value2() const { return fValue2; }
        Mode mode() const { return fMode; }
        std::u8string_view note() const { return fNote; }
        std::u8string wikiText(const DatingLoc& loc) const;

        // Year
        static constexpr Dating year(int y, std::u8string_view note = {}) noexcept
                    { return { Mode::YEAR, y, note }; }
        static constexpr Dating ybefore(int y, std::u8string_view note = {}) noexcept
                    { return { Mode::YBEFORE, y, note }; }
        static constexpr Dating yrange(int y1, int y2, std::u8string_view note = {}) noexcept
                    { return { Mode::YRANGE, y1, y2, note }; }
        static constexpr Dating yapprox(int y, std::u8string_view note = {}) noexcept
                    { return { Mode::YAPPROX, y, note }; }

        // Century
        static constexpr Dating century(int c, std::u8string_view note = {}) noexcept
                    { return { Mode::CENTURY, c, note }; }
        static constexpr Dating crange(int c1, int c2, std::u8string_view note = {}) noexcept
                    { return { Mode::CRANGE, c1, c2, note }; }
        static constexpr Dating capprox(int c, std::u8string_view note = {}) noexcept
                    { return { Mode::CAPPROX, c, note }; }

        // Decade
        static constexpr Dating decade(int y, std::u8string_view note = {}) noexcept
                    { return { Mode::DECADE, y, note }; }
        //static constexpr Dating dend(int y, std::u8string_view note = {}) noexcept
        //            { return { Mode::DEND, y, note }; }

        // Other
        static constexpr Dating special(std::u8string_view note) noexcept { return { note }; }
        static constexpr Dating unknown() noexcept { return { Mode::UNKNOWN, 0, {} }; }

        constexpr explicit operator bool() const noexcept { return (fMode != Mode::UNKNOWN); }
    private:
        Mode fMode;
        int fValue1 = 0, fValue2 = 0;
        std::u8string_view fNote;

        constexpr Dating(Mode aMode, int value, std::u8string_view aNote) noexcept
                : fMode(aMode), fValue1(value), fNote(aNote) {}
        constexpr Dating(Mode aMode, int value1, int value2, std::u8string_view aNote) noexcept
                : fMode(aMode), fValue1(value1), fValue2(value2), fNote(aNote) {}
        constexpr Dating(std::u8string_view aNote) noexcept
                : fMode(Mode::SPECIAL), fNote(aNote) {}
    };

}
