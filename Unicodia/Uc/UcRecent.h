#pragma once

//
//  You need to update this info periodically
//
//  If the situation changes in 2060:
//  1. Change all L10n’s you know, probably get rid of template
//     (no longer need to confirm every year)
//  2. Are any L10n’s left?
//     2.1. No, all → delete here and from MyWiki
//     2.2. Yes, left → set to 2059
//

namespace recent {

    /// Fact to check: all major vendors use UN blue emoji of Antarctica
    /// Piece of data: year
    /// Article: flag of Antarctica (AQ)
    /// L10n key: Lib.Misr.U+1F1E6+1F1F6
    /// Template: {{yantarc}}
    /// Sample reaction: Horns and Hooves switched to the new flag in 2060
    /// Known status: still unanimous
    constexpr unsigned YEAR_ANTARC = 2025;

    /// Fact to check: Afghanistan is ruled by the Taliban,
    ///     and all major vendors still use 2021 flag of Afghanistan
    /// Piece of data: year
    /// Article: flag of Afghanistan (AF)
    /// L10n key: Lib.Misr.U+1F1E6+1F1EB
    /// Template: {{ytalib}}
    /// Sample reaction: see Antarctica
    /// Known status: only Catrinity switched
    constexpr unsigned YEAR_TALIBAN = 2025;

    /// Fact to check: Brodie Lockard is alive
    /// Piece of data: year
    /// Article: mahjong
    /// L10n key: Block.1F000.Text
    /// Template: {{ylockard}}
    /// Sample reaction: “Brodie Lockard (1960-2060)”, born actually 1960
    /// Known status: still alive
    constexpr unsigned YEAR_LOCKARD = 2025;

    /// Fact to check: Unicode has 15 deprecated chars
    /// Piece of data: Unicode version, w/o “alpha” or “beta”
    /// Article: deprecated
    /// L10n key: Term.deprecated.Text
    /// Template: {{vdeprec15}}
    /// Sample reaction: blah-blah, and character X, total {{vdeprec16}}
    /// Known status: still 15 as of U17
    constexpr const char8_t* V_DEPREC_15 = u8"17";

}   // anmespace uc
