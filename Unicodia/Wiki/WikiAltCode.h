#pragma once

///
/// My wiki — generic Alt code support
/// Specific printers are in MyWiki
///

namespace uc {
    struct AltCode;
    struct OneByteInfo;
}

namespace myalt {

    constexpr char CODE_DOS = 0;
    constexpr char CODE_WIN = '0';

    enum class ComboMode : unsigned char {
        EVERYWHERE, ///< A single key combination works on every observed machine
        LIST        ///< There are special conditions, listed in continueList
    };

    class Printer {
    public:
        ///  Two modes:
        ///  1. startCombo(EVERYWHERE)
        ///  2. startCombo(LIST), continueList..., endList
        virtual void startCombo(
                int value, char prefix, ComboMode mode) = 0;
        virtual void continueList(
                bool isFirst, const uc::OneByteInfo& info) = 0;
        virtual void endList() = 0;
        virtual ~Printer() = default;
    };

    void print(const uc::AltCode& alt, Printer& prn);

};