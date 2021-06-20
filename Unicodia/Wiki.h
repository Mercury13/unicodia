#pragma once

// STL
#include <string_view>

// Libs
#include "u_Vector.h"


namespace wiki {

    enum ParamType { BAR, THING_END, STRING_END };
    struct Param {
        ParamType type;
        const char *posEnd, *posNext;
    };

    /// @param [in] cEnd  ALWAYS ']' or '}'
    Param skipParam(const char* pos, const char* end, char cEnd);

    /// @warning DO NOT USE in production code, just for unit-testing
    inline Param skipParam(std::string_view data, char cEnd)
        { return skipParam(data.data(), data.data() + data.length(), cEnd); }

    enum class Type { STRING_END, LINK, TEMPLATE };
    struct Thing {
        Type type;
        const char* posStart;
        const char* posNext;
        SafeVector<std::string_view> params;
    };

    Thing findThing(const char* pos, const char* end);

    /// @warning DO NOT USE in production code, just for unit-testing
    inline Thing findThing(std::string_view x)
        { return findThing(x.data(), x.data() + x.size()); }

    class Engine {  // interface
    public:
        virtual void appendPlain(std::string_view x) = 0;
        virtual void appendLink(const SafeVector<std::string_view> x) = 0;
        virtual void appendTemplate(const SafeVector<std::string_view> x) = 0;
        virtual ~Engine() = default;
    };


    void run(Engine& engine, const char* start, const char* end);
    inline void run(Engine& engine, std::string_view x)
        { run(engine, x.data(), x.data() + x.size()); }
    void run(Engine& engine, std::u8string_view x);

    template <class S>
    void append(S& r, const char* start, const char* end);

    template <>
    inline void append(std::string& r, const char* start, const char* end) {
        r.append(start, end);
    }

    namespace detail {
        template <class S>
        inline void append1(S& r, const char*& pSpan, const char* p, const char* entity) {
            append(r, pSpan, p);
            r.append(entity);
            pSpan = p + 1;
        }
    }

    template <class S>
    void appendHtml(S& r, std::string_view s)
    {
        r.reserve(r.size() + s.size());
        const char* p = s.data();
        const char* pSpan = p;
        const char* end = p + s.size();

        for (; p != end; ++p) {
            auto c = *p;
            switch(c) {
            case '\'':
                detail::append1(r, pSpan, p, "&apos;");
                break;
            case '"':
                detail::append1(r, pSpan, p, "&quot;");
                break;
            case '<':
                detail::append1(r, pSpan, p, "&lt;");
                break;
            case '>':
                detail::append1(r, pSpan, p, "&gt;");
                break;
            case '&':
                detail::append1(r, pSpan, p, "&amp;");
                break;
            }
        }
        append(r, pSpan, end);
    }

}   // namespace wiki
