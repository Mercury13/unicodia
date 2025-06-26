#pragma once

//
//  You need to update this info periodically
//
//  If the situation changes in 2060:
//  1. Change all L10n’s you know.
//  2. Are any L10n’s left?
//     2.1. No, all → delete here and from MyWiki
//     2.2. Yes, left → set to 2059
//

namespace uc {

    /// Fact to check: All major vendors use UN blue emoji of Antarctica
    /// Article: flag of Antarctica (AQ)
    /// L10n key: Lib.Misr.U+1F1E6+1F1F6
    /// Template: {{yantarc}}
    /// Reaction to partial change: think for yourself, e.g.
    ///    “Horns and Hoves switched to the new flag in 2060”,
    ///    without template (no longer need to confirm every year)
    /// Known status: still true
    constexpr unsigned YEAR_ANTARC = 2025;

    /// Fact to check: All major vendors use 2021 flag of Afghanistan
    /// Article: flag of Afghanistan (AF)
    /// L10n key: Lib.Misr.U+1F1E6+1F1EB
    /// Template: {{ytalib}}
    /// Reaction to partial change: same as with Antarctica
    /// Known status: still true
    constexpr unsigned YEAR_TALIBAN = 2025;

    /// Fact to check: Brodie Lockard is alive
    /// Article: mahjong
    /// L10n key: Block.1F000.Text
    /// Template: {{ylockard}}
    /// Reaction to partial change: N/A, he’s either alive or dead,
    ///    and I wrote nothing about his current life (environmental activist)
    /// Known status: still true
    constexpr unsigned YEAR_LOCKARD = 2025;

}   // anmespace uc
