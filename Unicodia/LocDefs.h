#pragma once

namespace loc {

    ///
    /// What to do with English terms
    ///
    enum class EngTerms : unsigned char {
        OFF = 0,        ///< Mainly English
        NORMAL = 1,     ///< Most other languages
        SORT_KEY = 2,   ///< Special mode when do not want to sort by Kana
        MIN = OFF,
        MAX = SORT_KEY
    };

}   // namespace loc
