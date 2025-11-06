// My header
#include "u_EgypIndex.h"


namespace {

    enum class CharType : unsigned char {
        PUNC, LETTER, DIGIT, REST, NN
    };
    constexpr auto CharType_N = static_cast<unsigned>(CharType::NN);
    constexpr auto CharType_Punc = static_cast<unsigned>(CharType::PUNC);

    enum class State : unsigned char {
        READY, LETTER_1, LETTER_2, DIGIT, TAIL, BAD, NN
    };
    constexpr auto State_N = static_cast<unsigned>(State::NN);

    enum class Action : unsigned char {
        WAIT,       ///< do nothing
        START,      ///< start index
        INDEX,      ///< drop good index
        SCRAP = WAIT
    };

    struct Transition {
        State nextState;
        Action action;
    };

    constinit const Transition MACHINE[][CharType_N] {
        { //-- READY
            { .nextState = State::READY,    .action = Action::WAIT  }, // PUNC
            { .nextState = State::LETTER_1, .action = Action::START }, // LETTER
            { .nextState = State::BAD,      .action = Action::WAIT  }, // DIGIT
            { .nextState = State::BAD,      .action = Action::WAIT  }, // REST
        },
        { //-- LETTER_1
            { .nextState = State::READY,    .action = Action::SCRAP }, // PUNC
            { .nextState = State::LETTER_2, .action = Action::WAIT  }, // LETTER
            { .nextState = State::DIGIT,    .action = Action::WAIT  }, // DIGIT
            { .nextState = State::BAD,      .action = Action::SCRAP }, // REST
        },
        { //-- LETTER_2
            { .nextState = State::READY,    .action = Action::SCRAP }, // PUNC
            { .nextState = State::BAD,      .action = Action::SCRAP }, // LETTER
            { .nextState = State::DIGIT,    .action = Action::WAIT  }, // DIGIT
            { .nextState = State::BAD,      .action = Action::SCRAP }, // REST
        },
        { //-- DIGIT
            { .nextState = State::READY,    .action = Action::INDEX }, // PUNC
            { .nextState = State::TAIL,     .action = Action::WAIT  }, // LETTER
            { .nextState = State::DIGIT,    .action = Action::WAIT  }, // DIGIT
            { .nextState = State::BAD,      .action = Action::SCRAP }, // REST
        },
        { //-- TAIL
            { .nextState = State::READY,    .action = Action::INDEX }, // PUNC
            { .nextState = State::BAD,      .action = Action::SCRAP }, // LETTER
            { .nextState = State::BAD,      .action = Action::SCRAP }, // DIGIT
            { .nextState = State::BAD,      .action = Action::SCRAP }, // REST
        },
        { //-- BAD
            { .nextState = State::READY,    .action = Action::WAIT  }, // PUNC
            { .nextState = State::BAD,      .action = Action::WAIT  }, // LETTER
            { .nextState = State::BAD,      .action = Action::WAIT  }, // DIGIT
            { .nextState = State::BAD,      .action = Action::WAIT  }, // REST
        },
    };

    static_assert(std::size(MACHINE) == State_N);

    ///  @return  [+] we have a valid index in this state,
    ///               so-called “final state” in Moore automata
    inline bool isFinal(State state)
    {
        return MACHINE[static_cast<unsigned>(state)][CharType_Punc].action == Action::INDEX;
    }

    constexpr CharType classify(char x)
    {
        if (x >= 'A' && x <= 'Z')
            return CharType::LETTER;
        if (x >= 'a' && x <= 'z')
            return CharType::LETTER;
        if (x >= '0' && x <= '9')
            return CharType::DIGIT;
        if (static_cast<unsigned char>(x) <= 0x7F)
            return CharType::PUNC;
        return CharType::REST;
    }

}   // anon namespace


void egyp::extractIndexes(std::string_view s, Callback cb)
{
    const char* p = std::to_address(s.begin());
    const char* pEnd = std::to_address(s.end());
    const char* pText = p;
    const char* pIndex = p;
    State state = State::READY;
    for (; p != pEnd; ++p) {
        auto ct = classify(*p);
        auto& what = MACHINE[unsigned(state)][unsigned(ct)];
        switch (what.action) {
        case Action::WAIT: break;
        case Action::START: pIndex = p; break;
        case Action::INDEX:
            cb(std::string_view{pText, pIndex}, std::string_view{pIndex, p});
            pText = pIndex = p;
            break;
        }

        state = what.nextState;
    }
    if (isFinal(state)) {
        cb(std::string_view{pText, pIndex}, std::string_view{pIndex, pEnd});
    } else {
        cb(std::string_view{pText, pEnd}, {});
    }
}
