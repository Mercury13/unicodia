// My header
#include "nonAscii.h"

// STL
#include <unordered_map>
#include <memory>

// Strings
#include "u_Strings.h"
#include "mojibake.h"

const srh::NonAsciiComparator srh::NonAsciiComparator::INST;


namespace {

    using M = std::unordered_map<char32_t, char>;

    /// Somehow reentrant; fighting for object and making 2+ is OK
    /// (though we do not use threads and it won’t happen)
    std::unique_ptr<M> NON_ASCII_MAP;

    constinit std::initializer_list<M::value_type> NON_ASCII_INIT {
        { U'°', '*' },
        // All Latin-1 letters regardless of whether they pre present
        // C0
        { U'À', 'A' },
        { U'Á', 'A' },
        { U'Â', 'A' },
        { U'Ã', 'A' },
        { U'Ä', 'A' },
        { U'Å', 'A' },
        // Æ complex, unused
        { U'Ç', 'C' },
        // C8
        { U'È', 'E' },
        { U'É', 'E' },
        { U'Ê', 'E' },
        { U'Ë', 'E' },
        { U'Ì', 'I' },
        { U'Í', 'I' },
        { U'Î', 'I' },
        { U'Ï', 'I' },
        // D0
        { U'Ð', 'D' },
        { U'Ñ', 'N' },
        { U'Ò', 'O' },
        { U'Ó', 'O' },
        { U'Ô', 'O' },
        { U'Õ', 'O' },
        { U'Ö', 'O' },
        // × unused
        // D8
        { U'Ø', 'O' },
        { U'Ù', 'U' },
        { U'Ú', 'U' },
        { U'Û', 'U' },
        { U'Ü', 'U' },
        { U'Ý', 'Y' },
        // Þ unused
        // ß unused
        // E0
        { U'à', 'a' },
        { U'á', 'a' },
        { U'â', 'a' },
        { U'ã', 'a' },
        { U'ä', 'a' },
        { U'å', 'a' },
        // æ used once, no reason to implement complex logic
        { U'ç', 'c' },
        // E8
        { U'è', 'e' },
        { U'é', 'e' },
        { U'ê', 'e' },
        { U'ë', 'e' },
        { U'ì', 'i' },
        { U'í', 'i' },
        { U'î', 'i' },
        { U'ï', 'i' },
        // F0
        { U'ð', 'd' },  // Let it be, used once and it’s INDEED version of D
        { U'ñ', 'n' },
        { U'ò', 'o' },
        { U'ó', 'o' },
        { U'ô', 'o' },
        { U'õ', 'o' },
        { U'ö', 'o' },
        // ÷ unused
        // F8
        { U'ø', 'o' },
        { U'ù', 'u' },
        { U'ú', 'u' },
        { U'û', 'u' },
        { U'ü', 'u' },
        { U'ý', 'u' },
        // þ unused
        { U'ÿ', 'y' },
        // 110
        { U'Ā', 'A' },
        { U'ā', 'a' },
        { U'Ă', 'A' },
        { U'ă', 'a' },
        { U'Ą', 'A' },
        { U'ą', 'a' },
        { U'Ć', 'C' },
        { U'ć', 'c' },
        // 108
        { U'Ĉ', 'C' },
        { U'ĉ', 'c' },
        { U'Ċ', 'C' },
        { U'ċ', 'c' },
        { U'Č', 'C' },
        { U'č', 'c' },
        { U'Ď', 'D' },
        { U'ď', 'd' },
        // 110
        { U'Đ', 'D' },
        { U'đ', 'd' },
        { U'Ē', 'E' },
        { U'ē', 'e' },
        { U'Ĕ', 'E' },
        { U'ĕ', 'e' },
        { U'Ė', 'E' },
        { U'ė', 'e' },
        // 118
        { U'Ę', 'E' },
        { U'ę', 'e' },
        { U'Ě', 'E' },
        { U'ě', 'e' },
        { U'Ĝ', 'G' },
        { U'ĝ', 'g' },
        { U'Ğ', 'G' },
        { U'ğ', 'g' },
        // 120
        { U'Ġ', 'G' },
        { U'ġ', 'g' },
        { U'Ģ', 'G' },
        { U'ģ', 'g' },
        { U'Ĥ', 'H' },
        { U'ĥ', 'h' },
        { U'Ħ', 'H' },
        { U'ħ', 'h' },
        // 128
        { U'Ĩ', 'I' },
        { U'ĩ', 'i' },
        { U'Ī', 'I' },
        { U'ī', 'i' },
        { U'Ĭ', 'I' },
        { U'ĭ', 'i' },
        { U'Į', 'I' },
        { U'į', 'i' },
        // 15*
        { U'Ő', 'O' },
        { U'ő', 'o' },
        // 16*
        { U'ū', 'u' },
        // 17*
        { U'Ű', 'U' },
        { U'ű', 'u' },
        // 1C*
        { U'ǎ', 'a' },
        // 2B*
        { U'ʼ', '\'' },
        // 627 — Arabic (instead of deprecated)
        // 65F — Arabic (instead of deprecated)
        // F71 — Tibetan (instead of deprecated)
        // F80 — Tibetan (instead of deprecated)
        // FB2 — Tibetan (instead of deprecated)
        // FB3 — Tibetan (instead of deprecated)
        // 17A2 — Khmer (instead of deprecated)
        // 17B6 — Khmer (instead of deprecated)
        // 201*
        { U'’', '\'' },
        { U'“', '"' },
        { U'”', '"' },
        // 2191 = ↑ (abbreviation)
        // 2193 = ↓ (abbreviation)
        // 2194 = ↔ (abbreviation)
        // 2715 = ✕ (abbreviation)
        // 27E8 = ⟨ (instead of deprecated)
        // 27E9 = ⟩ (instead of deprecated)
        // 3008 = 〈 (instead of deprecated)
        // 3009 = 〉 (instead of deprecated)
    };

}   // anon namespace

void srh::NonAsciiComparator::prepareHaystack(
        std::u8string_view haystack, std::u8string& result) const
{
    if (haystack == u8"ten o’clock") {
        std::cout << "Ten" "\n";
    }
    result.clear();
    auto func = [&result](char32_t x) {
        if (x < 128) {
            result.push_back(lat::toUpper(x));
        } else {
            // Lazy big object initialization
            if (!NON_ASCII_MAP) {
                NON_ASCII_MAP = std::make_unique<M>(NON_ASCII_INIT);
            }
            auto res = NON_ASCII_MAP->find(x);
            if (res != NON_ASCII_MAP->end()) {
                result.push_back(lat::toUpper(res->second));
            } else {
                // otherwise do nothing?
            }
        }
    };
    mojibake::copyS(haystack.begin(), haystack.end(),
                    mojibake::Utf32CallIterator(func));
}
