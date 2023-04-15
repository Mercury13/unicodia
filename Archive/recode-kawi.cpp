///// Kawi /////////////////////////////////////////////////////////////////////


namespace {

    enum class RecodeIdentity { INST };
    constexpr auto IDENTITY = RecodeIdentity::INST;

    struct RecodeSpan {
        const char32_t upper;
        const int delta;

        consteval RecodeSpan(char32_t aUpper, RecodeIdentity)
            : upper(aUpper), delta(0) {}
        consteval RecodeSpan(
                char32_t aSample, char32_t aUpper,
                char32_t aSampleTo, char32_t aUpperTo)
            : upper(aUpper), delta(int(aSampleTo) - int(aSample))
        {
            assert(aUpper >= aSample);
            assert(int(aUpperTo) - int(aUpper) == delta);
        }
        consteval RecodeSpan(
                char32_t aUpper, char32_t aUpperTo)
            : upper(aUpper), delta(int(aUpperTo) - int(aUpper)) {}
    };

    struct CmpBySpan {
        bool operator () (const RecodeSpan& x, char32_t y) { return (x.upper < y); }
        bool operator () (char32_t x, const RecodeSpan& y) { return (x < y.upper); }
    };

    constinit RecodeSpan kawiSpans[] {
        { 0x11EFF, IDENTITY },                  // Before
        { 0x11F00, 0x11F32, 0x1B01, 0x1B33 },   // Candrabindu … Ha → Bali
        { 0x11F33, 0xA998 },                    // Jnya → Java
        { 0x11F34, 0x1B35 },                    // Small Aa → Bali
        { 0x11F35, 0xA9B5 },                    // Big Aa → Java
        { 0x11F36, 0x11F3F, 0x1B36, 0x1B3F },   // I … Ai → Java
        { 0x11F40, 0x1B42 },                    // Eu = schwa → Java pepet
        { 0x11F41, 0x1B44 },                    // Virama
        { 0x11F42, IDENTITY },                  // Conjoiner
        { 0x11F43, 0xA9CA },                    // Danda
        { 0x11F44, 0x1B5A },                    // Double danda
        { 0x11F45, 0xA9C3 },                    // Section marker
        { 0x11F46, 0x1B5B },                    // Alt. section marker
        { 0x11F47, 0xA9C1 },                    // Flower
        { 0x11F48, 0xA9CF },                    // Space filler
        { 0x11F49, 0x1B5E },                    // Dot
        { 0x11F4A, 0x1B5D },                    // Double dot
        { 0x11F4E, IDENTITY },                  // Three circles and spiral
        { 0x11F4F, 0xA9C2 },                    // Closing spiral
        { 0x11F50, 0x11F59, 0x1B50, 0x1B59 },   // Digits 0…9
        { 0xFFFFFFFF, IDENTITY }                // After
    };

}   // anon namespace


char32_t recodeKawi(char32_t unicode)
{
    auto pSpan = std::lower_bound(std::begin(kawiSpans), std::end(kawiSpans), unicode, CmpBySpan{});
    return unicode + pSpan->delta;
}
