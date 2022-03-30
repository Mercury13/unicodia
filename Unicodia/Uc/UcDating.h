#pragma once

#include <string>
#include <vector>

namespace uc {

    enum class CrangeMode {
        ARABIC_SPECIAL,         // English: 12-13th century
        SPECIAL_SPECIAL };      // Russian: XII-XIII век

    enum class StdNote {
        CUSTOM,         ///< Custom (none = CUSTOM + no note)
        FIRST_KNOWN,    ///< (first known inscription)
        MODERN_FORM     ///< (in modern form)
    };

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
        CrangeMode crangeMode;          ///< 12th century (some comment), 13th century (first known inscription)
                // crange uses century/centuryBc
        //std::u8string_view crangeBcCe;   ///< 2nd century BC - 3rd century CE;  unused
                // Before 12th century is Special

        // Other
        std::u8string_view unknown;     ///< unknown;

        // Notes
        std::u8string_view firstInscription;    /// first inscription
        std::u8string_view modernForm;          /// modern form

        std::u8string_view formatCentury(int x) const;
        std::u8string_view format1stCentury(int x, char* start, char* end) const;
        std::u8string formatCenturyText(int x) const;
        void appendCenturyText(std::u8string& r, int x) const;
        template <size_t N>
          std::u8string_view format1stCentury(int x, char(&buf)[N]) const
            { return format1stCentury(x, std::begin(buf), std::end(buf)); }
        void appendNote(std::u8string& r, StdNote sn, std::u8string_view note) const;
    };

    ///
    ///  Used for i18n: dates are written in code, only notes are localized
    ///
    class Dating
    {
    public:
        enum class Mode {
            NONE,       ///<  ???
            // Year
            YEAR,       ///<  1234              Armn, Bopo, Brai …………
            YBEFORE,    ///<  before 1234       Cyrl, Zanb
            YRANGE,     ///<  1234-1357         Bamu, Modi, Samr …
            YAPPROX,    ///<  ≈1234             Avst, Bali, Cpmn …………
            // Decade
            DECADE,     ///<  1230s             Hmnp, Vaii, Talu …
            //DEND,       ///<  end of 1230s
            // Century
            CENTURY,    ///<  12th century      Arab, Bhks, Cprt …………
            CRANGE,     ///<  12–13th century   Deva, Diak, Taml …………
            CRANGE_INS, ///<  12th century (some comment), 13th century (first known inscription)
                        ///<                    Grek
            // Special
            UNKNOWN,    ///<  really unknown    Cakm
            SPECIAL,    ///<  2nd century (main hypothesis), 7th century (for sure)
                        ///<                    Adlm, Aghb, Brah …………
        };
        int value1() const { return fValue1; }
        int value2() const { return fValue2; }
        Mode mode() const { return fMode; }
        StdNote stdNote() const { return fStdNote; }
        std::u8string_view note() const { return fNote; }
        std::u8string wikiText(const DatingLoc& loc) const;

        // Year
        static constexpr Dating year(int y, std::u8string_view note = {}) noexcept
                    { return { Mode::YEAR, y, note }; }
        static constexpr Dating year(int y, StdNote note) noexcept
                    { return { Mode::YEAR, y, note }; }
        static constexpr Dating ybefore(int y, std::u8string_view note = {}) noexcept
                    { return { Mode::YBEFORE, y, note }; }
        static constexpr Dating yrange(int y1, int y2, std::u8string_view note = {}) noexcept
                    { return { Mode::YRANGE, y1, y2, note }; }
        static constexpr Dating yapprox(int y, std::u8string_view note = {}) noexcept
                    { return { Mode::YAPPROX, y, note }; }
        static constexpr Dating yapprox(int y, StdNote note) noexcept
                    { return { Mode::YAPPROX, y, note }; }

        // Century
        static constexpr Dating century(int c, std::u8string_view note = {}) noexcept
                    { return { Mode::CENTURY, c, note }; }
        static constexpr Dating century(int c, StdNote note) noexcept
                    { return { Mode::CENTURY, c, note }; }
        static constexpr Dating crange(int c1, int c2, std::u8string_view note = {}) noexcept
                    { return { Mode::CRANGE, c1, c2, note }; }
                 // Need note here!
        static constexpr Dating crange_ins(int c1, int c2, std::u8string_view note) noexcept
                    { return { Mode::CRANGE_INS, c1, c2, note }; }

        // Decade
        static constexpr Dating decade(int y, std::u8string_view note = {}) noexcept
                    { return { Mode::DECADE, y, note }; }

        // Other
        static constexpr Dating special(std::u8string_view note) noexcept { return { note }; }
        static constexpr Dating unknown() noexcept { return { Mode::UNKNOWN, 0, {} }; }
        static constexpr Dating none() noexcept { return { Mode::NONE, 0, {} }; }

        constexpr explicit operator bool() const noexcept { return (fMode != Mode::NONE); }
    private:
        Mode fMode;
        int fValue1 = 0, fValue2 = 0;
        std::u8string_view fNote;
        StdNote fStdNote = StdNote::CUSTOM;

        constexpr Dating(Mode aMode, int value, std::u8string_view aNote) noexcept
                : fMode(aMode), fValue1(value), fNote(aNote) {}
        constexpr Dating(Mode aMode, int value, StdNote aNote) noexcept
                : fMode(aMode), fValue1(value), fStdNote(aNote) {}
        constexpr Dating(Mode aMode, int value1, int value2, std::u8string_view aNote) noexcept
                : fMode(aMode), fValue1(value1), fValue2(value2), fNote(aNote) {}
        constexpr Dating(std::u8string_view aNote) noexcept
                : fMode(Mode::SPECIAL), fNote(aNote) {}
    };

}
