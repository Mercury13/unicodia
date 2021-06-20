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
        }
    }
    return { Type::STRING_END, end, end, {} };
}


void wiki::run(Engine& engine, const char* start, const char* end)
{
    while (true) {
        auto x = findThing(start, end);
        if (x.posStart != start)
            engine.appendPlain({ start, x.posStart });
        switch (x.type) {
        case Type::STRING_END:
            goto brk;
        case Type::LINK:
            engine.appendLink(x.params);
            break;
        case Type::TEMPLATE:
            engine.appendTemplate(x.params);
            break;
        }
        start = x.posNext;
    }
    brk: ;
}


void wiki::run(Engine& engine, std::u8string_view x)
{
    auto data = reinterpret_cast<const char*>(x.data());
    run(engine, data, data + x.size() );
}
