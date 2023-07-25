#include "forget.h"

// STL
#include <unordered_set>
#include <unordered_map>
#include <fstream>

// Libs
#include "u_Strings.h"

namespace {

    enum class ForgetChannel { SCRIPT, LETTER, BAN };

    const std::unordered_map<std::string_view, ForgetChannel> forgetWords {
        // Script channel
        { "LATIN", ForgetChannel::SCRIPT },
        { "MODIFIER", ForgetChannel::SCRIPT },
        { "SUPERSCRIPT", ForgetChannel::SCRIPT },
        { "SUBSCRIPT", ForgetChannel::SCRIPT },

        // Letter channel
        { "A", ForgetChannel::LETTER },
        { "AA", ForgetChannel::LETTER },
        { "AE", ForgetChannel::LETTER },
        { "ALPHA", ForgetChannel::LETTER },
        { "B", ForgetChannel::LETTER },
        { "C", ForgetChannel::LETTER },
        { "D", ForgetChannel::LETTER },
        { "DZ", ForgetChannel::LETTER },
        { "DEZH", ForgetChannel::LETTER },
        { "E", ForgetChannel::LETTER },
        { "SCHWA", ForgetChannel::LETTER },
        { "F", ForgetChannel::LETTER },
        { "FENG", ForgetChannel::LETTER },
        { "G", ForgetChannel::LETTER },
        { "H", ForgetChannel::LETTER },
        { "I", ForgetChannel::LETTER },
        { "IJ", ForgetChannel::LETTER },
        { "IOTA", ForgetChannel::LETTER },
        { "J", ForgetChannel::LETTER },
        { "K", ForgetChannel::LETTER },
        { "L", ForgetChannel::LETTER },
        { "LJ", ForgetChannel::LETTER },
        { "LEZH", ForgetChannel::LETTER },
        { "M", ForgetChannel::LETTER },
        { "N", ForgetChannel::LETTER },
        { "NJ", ForgetChannel::LETTER },
        { "ENG", ForgetChannel::LETTER},
        { "NUM", ForgetChannel::LETTER},
        { "O", ForgetChannel::LETTER },
        { "OE", ForgetChannel::LETTER },
        { "P", ForgetChannel::LETTER },
        { "Q", ForgetChannel::LETTER },
        { "R", ForgetChannel::LETTER },
        { "S", ForgetChannel::LETTER },
        { "ESH", ForgetChannel::LETTER },
        { "T", ForgetChannel::LETTER },
        { "ETH", ForgetChannel::LETTER },
        { "TESH", ForgetChannel::LETTER },
        { "TS", ForgetChannel::LETTER },
        { "TZ", ForgetChannel::LETTER },
        { "U", ForgetChannel::LETTER },
        { "V", ForgetChannel::LETTER },
        { "W", ForgetChannel::LETTER },
        { "X", ForgetChannel::LETTER },
        { "Y", ForgetChannel::LETTER },
        { "Z", ForgetChannel::LETTER },
        { "EZH", ForgetChannel::LETTER},

        // Banned
        { "CYRILLIC", ForgetChannel::BAN },
        { "SAMARITAN", ForgetChannel::BAN },
        { "FULLWIDTH", ForgetChannel::BAN },
        { "COMBINING", ForgetChannel::BAN },
    };

    const std::unordered_set<char32_t> bannedCps {
        0x0149,     // Actually banned from Unicode
    };

}   // anon namespace


bool forget::isIn(char32_t cp, std::string_view name)
{
    // ASCII, plane 2+ → never in
    if (cp <= 127 || cp >= 0x2'0000)
        return false;
    // Banned CP → never in
    if (bannedCps.contains(cp))
        return false;
    // Stock name → never in
    if (name.find('#') != std::string_view::npos)
        return false;
    // Split into words
    // Should contain both script and letter, should not contain ban
    auto upCase = str::toUpper(name);
    auto words = str::splitSv(upCase, ' ');
    bool hasScript = false, hasLetter = false;
    for (auto word : words) {
        auto q = forgetWords.find(word);
        if (q != forgetWords.end()) {
            switch (q->second) {
            case ForgetChannel::SCRIPT:
                hasScript = true; break;
            case ForgetChannel::LETTER:
                hasLetter = true; break;
            case ForgetChannel::BAN:
                return false;
            }
        }
    }
    return hasScript && hasLetter;
}


namespace {

    /// Register CP in forgotten base
    void reg(forget::Cp& q, std::string_view name, char32_t capital, bool isIn)
    {
        q.database.name = name;
        q.database.capital = capital;
        q.computed.isIn = isIn;
    }

}   // anon namespace


void forget::processCp(Map& map, char32_t cp, std::string_view name, char32_t capital)
{
    if (isIn(cp, name)) {
        auto& q = map[cp];
        reg(q, name, capital, true);
    } else {
        auto it = map.find(cp);
        if (it != map.end()) {
            reg(it->second, name, capital, false);
        }
    }
}


namespace {

    /// Section manager: automatically writes header
    class SectionMan
    {
    public:
        explicit SectionMan(std::ostream& aOs) noexcept : os(aOs) {}
        void arm(std::string aHead);
        void trigger();
        void disarm();
        bool hasEverTriggered() const { return fHasEverTriggered; }
    private:
        enum class State { SAFE, ARMED, TRIGGERED };

        std::ostream& os;
        std::string head;
        State state = State::SAFE;
        bool fHasEverTriggered = false;
    };

    void SectionMan::arm(std::string aHead)
    {
        disarm();
        head = std::move(aHead);
        state = State::ARMED;
    }

    void SectionMan::trigger()
    {
        if (state == State::ARMED) {
            os << head << '\n';
            state = State::TRIGGERED;
            fHasEverTriggered = true;
        }
    }

    void SectionMan::disarm()
    {
        if (state == State::TRIGGERED) {
            os << '\n';
        }
        state = State::SAFE;
    }
}


forget::Stats forget::postprocess(const Map& map, const char* fname)
{
    std::ofstream os(fname);
    SectionMan secMan(os);

    auto print = [&os, &secMan](const Map::value_type& val) {
        // Trigger
        secMan.trigger();
        // Print code
        char q[20];
        snprintf(q, std::size(q), "%04X", static_cast<unsigned>(val.first));
        os << q;
        // Print name
        if (!val.second.database.name.empty()) {
            os << ' ' << val.second.database.name << '\n';
        }
    };

    Stats r;

    secMan.arm("REPEATING");
    for (auto& q : map) {
        if (q.second.lib.count > 1) {
            print(q);
            ++r.nRepeat;
        }
    }

    secMan.arm("FORGOTTEN");
    for (auto& q : map) {
        if (q.second.lib.count == 0 && q.second.computed.isIn) {
            print(q);
            ++r.nMissing;
        }
    }

    secMan.arm("EXTRANEOUS");
    for (auto& q : map) {
        if (q.second.lib.count > 0 && !q.second.computed.isIn) {
            print(q);
            ++r.nExtra;
        }
    }

    if (!secMan.hasEverTriggered()) {
        os << "No forgotten codepoints." << '\n';
    }
    return r;
}
