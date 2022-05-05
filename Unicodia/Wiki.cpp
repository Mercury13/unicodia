#include "Wiki.h"

#include <stack>
#include "u_Strings.h"

wiki::Param wiki::skipParam(const char* pos, const char* end, char cEnd)
{
    std::stack<char> stk;
    auto minus1 = end - 1;

    for (; pos != end; ++pos) {
        auto c = *pos;
        switch (c) {
        case '|':
            if (stk.empty())
                return { ParamType::BAR, pos, pos + 1 };
            break;
        case '}':
        case ']':
            if (pos != minus1 && *(pos + 1) == c) {   // ]] or }}
                if (stk.empty()) {
                    // Empty stack → probably OK
                    if (c == cEnd)
                        return { ParamType::THING_END, pos, pos + 2 };
                } else {
                    // Non-empty → pop if equal
                    if (stk.top() == c)
                        stk.pop();
                }
                ++pos;
            } break;
        case '{':
        case '[': {
                if (pos != minus1 && *(pos + 1) == c) {
                    char otherBracket = c + 2;
                    stk.push(otherBracket);
                    ++pos;
                }
            } break;
        }
    }
    return { ParamType::STRING_END, end, end };
}


wiki::Thing wiki::findThing(const char* pos, const char* end)
{
    auto minus1 = end - 1;

    for (; pos != end; ++pos) {
        auto c = *pos;
        switch (c) {
        case '{':
        case '[': { // O(n²) here, but OK, no evil text here
                if (pos != minus1 && *(pos + 1) == c) {
                    char otherBracket = c + 2;
                    SafeVector<std::string_view> params;
                    auto p = pos + 2;   // after double bracket
                    while (true) {
                        auto param = skipParam(p, end, otherBracket);
                        switch (param.type) {                        
                        case ParamType::STRING_END:
                            // Found nothing
                            goto brk;
                        case ParamType::BAR:
                            // 1st param should be trimmed and not empty
                            // otherwise false alarm
                            if (params.empty()) {
                                str::trim(p, param.posEnd);
                                if (p == param.posEnd)
                                    goto brk;
                            }
                            params.emplace_back( p, param.posEnd );
                            p = param.posNext;
                            break;
                        case ParamType::THING_END: {
                                auto start = p;
                                auto end = param.posEnd;
                                if (params.empty()) {
                                    str::trim(start, end);
                                    if (start == end)
                                        goto brk;
                                }
                                params.emplace_back( start, end );
                                auto type = (c == '[') ? Type::LINK : Type::TEMPLATE;
                                // >=2 params in link!
                                if (type == Type::LINK && params.size() == 1) {
                                    std::string_view sMain{ p, param.posEnd };
                                    if (sMain.empty())
                                        sMain = "???";
                                    params.emplace_back(sMain);
                                }
                                return { type, pos, param.posNext, std::move(params) };
                            } break;
                        }
                    }   // loop
                    brk:;
                }   // if ()
            } break;
        case '\'':  // Programmer’s apostrophe
            if (pos != minus1 && *(pos + 1) == '\'') {
                auto q = pos + 2;
                while (q != end && *q == '\'')
                    ++q;
                auto nQuotes = q - pos;
                auto type = (nQuotes % 2 == 0)
                        ? ((nQuotes == 2) ? Type::ITALIC : Type::EMPTY)
                        : ((nQuotes == 5) ? Type::BOLD_ITALIC : Type::BOLD);
                return { type, pos, q, {} };
            }
            break;
        case '\n': {
            // Count those LFs
                auto q = pos + 1;
                while (q != end && *q == '\n') {
                    ++q;
                }
                auto n = q - pos;
                if (n > 1) {
                    return { Type::PARAGRAPH, pos, q, {} };
                }
            } break;
        default: ;
        }   // switch (c)
    }
    return { Type::STRING_END, end, end, {} };
}


[[nodiscard]] wiki::detail::ProbedWeight wiki::detail::probeWeight(
        const char* pos, const char* end)
{
    auto minus1 = end - 1;

    for (; pos != end; ++pos) {
        auto c = *pos;
        switch (c) {
        case '\'':
            if (pos != minus1 && *(pos + 1) == '\'') {
                auto q = pos + 2;
                while (q != end && *q == '\'')
                    ++q;
                auto nQuotes = q - pos;
                auto weight = (nQuotes % 2 == 0)
                        ? ((nQuotes == 2) ? Weight::ITALIC : Flags<Weight>() )
                        : ((nQuotes == 5) ? Weight::BOLD | Weight::ITALIC : Weight::BOLD);
                return { weight, q };
            }
        default: ;
        }   // switch (c)
    }
    return { {}, nullptr };
}


void wiki::run(Engine& engine, const char* start, const char* end)
{
    while (true) {
        auto x = findThing(start, end);
        if (x.posStart != start)
            engine.appendPlain({ start, x.posStart });
        switch (x.type) {
        case Type::EMPTY:
            break;
        case Type::BOLD:
            engine.toggleWeight(Weight::BOLD);
            break;
        case Type::ITALIC:
            engine.toggleWeight(Weight::ITALIC);
            break;
        case Type::BOLD_ITALIC:
            engine.toggleWeight(Weight::BOLD | Weight::ITALIC);
            break;
        case Type::STRING_END:
            goto brk;
        case Type::LINK:
            engine.appendLink(x.params, (x.posNext != end));
            break;
        case Type::TEMPLATE:
            engine.appendTemplate(x.params, (x.posNext != end));
            break;
        case Type::PARAGRAPH:
            engine.appendParagraph();
        }
        start = x.posNext;
    }
brk:
    engine.finish();
}


Flags<wiki::Weight> wiki::probeWeights(const char* start, const char* end)
{
    static constexpr auto ALL_FLAGS = Weight::BOLD | Weight::ITALIC;
    Flags<Weight> r = {};
    while (true) {
        auto x = detail::probeWeight(start, end);
        if (!x.posNext)
            break;
        r |= x.flags;
        if (r == ALL_FLAGS)
            return r;
        start = x.posNext;
    }
    return r;
}


///// HtWeight /////////////////////////////////////////////////////////////////


constinit const wiki::HtWeight::ToggleResult wiki::HtWeight::machine[5][4] {
        // NO TAGS
    {   { {},       State::z },     // ⌀ ^ ⌀ = ⌀
        { "<i>",    State::I },     // ⌀ ^ i = i
        { "<b>",    State::B },     // ⌀ ^ b = b
        { "<b><i>", State::BI } },  // ⌀ ^ bi = bi
        // ITALIC
    {   { {},       State::I },     // i ^ ⌀ = i
        { "</i>",   State::z },     // i ^ i = ⌀
        { "<b>",    State::IB },    // i ^ b = ib
        { "</i><b>", State::B } },  // i ^ bi = b: close i, open b
        // BOLD
    {   { {},       State::B },     // b ^ ⌀ = b
        { "<i>",    State::BI },    // b ^ i = bi
        { "</b>",   State::z },     // b ^ b = ⌀
        { "</b><i>", State::I } },  // b ^ bi = i: close b, open i
        // BOLD-ITALIC
    {   { {},       State::BI},     // bi ^ ⌀ = bi
        { "</i>",   State::B },     // bi ^ i = b; just close i
        { "</i></b><i>", State::I }, // bi ^ b = i; you cannot close b, need 3 tags
        { "</i></b>", State::z } }, // bi ^ bi = ⌀, close in reverse order
        // ITALIC-BOLD
    {   { {},       State::BI},     // ib ^ ⌀ = ib
        { "</b></i><b>", State::B }, // ib ^ i = b; you cannot close i, need 3 tags
        { "</b>",   State::I },     // ib ^ b = i; just close b
        { "</b></i>", State::z } }  // ib ^ bi = ⌀, close in reverse order
};


constinit const std::string_view wiki::HtWeight::finishResult[5] {
    // Bold-italic close as </i></b>, etc
    {}, "</i>", "</b>", "</i></b>", "</b></i>" };

constinit const std::string_view wiki::HtWeight::restartResult[5] {
    {}, "<i>", "<b>", "<b><i>", "<i><b>" };

constinit const Flags<wiki::Weight> wiki::HtWeight::flagsResult[5] {
    {},
    Weight::ITALIC,
    Weight::BOLD,
    Weight::BOLD | Weight::ITALIC,
    Weight::BOLD | Weight::ITALIC,
};


std::string_view wiki::HtWeight::toggle(Flags<Weight> changed)
{
    auto& m = machine[static_cast<int>(fState)][changed.numeric()];
    fState = m.state;
    return m.result;
}


std::string_view wiki::HtWeight::finish() const
{
    return finishResult[static_cast<int>(fState)];
}


std::string_view wiki::HtWeight::restart() const
{
    return restartResult[static_cast<int>(fState)];
}

Flags<wiki::Weight> wiki::HtWeight::flags() const
{
    return flagsResult[static_cast<int>(fState)];
}
