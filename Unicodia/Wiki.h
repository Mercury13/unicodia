#pragma once

// STL
#include <string_view>

// Libs
#include "u_Vector.h"
#include "u_TypedFlags.h"


//
//  Bold/italic styles:
//  ' = apostrophe
//  '' = i
//  ''''' = bi
//  The rest odd = b
//  The rest even = do nothing: i-i or b-b. E.g. 8 = i-i i-i
//


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

    enum class Type {
            EMPTY, BOLD, ITALIC, BOLD_ITALIC, STRING_END, LINK, TEMPLATE,
            LINEBREAK, PARAGRAPH };
    enum class Strength { BREAK, PARAGRAPH };
    enum class Feature { NONE, INDENT, BULLET };

    struct Thing {
        Type type;
        Feature feature;
        const char* posStart;
        const char* posNext;
        SafeVector<std::string_view> params;
    };

    Thing findThing(const char* pos, const char* end, Feature currFeature);

    /// @warning DO NOT USE in production code, just for unit-testing
    inline Thing findThing(std::string_view x)
        { return findThing(x.data(), x.data() + x.size(), Feature::NONE); }

    enum class Weight { ITALIC = 1, BOLD = 2 };
    DEFINE_ENUM_OPS(wiki::Weight)

    ///
    /// @warning  Engine class is usually stateful and thus non-reentrant
    ///           (except for recursive parsing of the same text, and,
    ///            of course, 1) the engine initiates recursion itself;
    ///            2) it should be ready for that)
    ///
    class Engine {  // interface
    public:
        /// @param [in] changed   Flags that changed,
        ///                       state ← state ^ changed
        /// @param [in] remainder  Remainder of wiki text, to check
        ///                        a bit forward
        /// @warning  The engine itself keeps track of weights, as it can run
        ///           recursively.
        virtual void toggleWeight(
                    [[maybe_unused]] Flags<Weight> changed) {}

        /// Writes plain text
        virtual void appendPlain(std::string_view x) = 0;
        virtual void appendLink(
                    const SafeVector<std::string_view>& x,
                    bool hasRemainder) = 0;
        virtual void appendTemplate(
                    const SafeVector<std::string_view>& x,
                    bool hasRemainder) = 0;
        virtual void appendBreak(Strength strength, Feature feature) = 0;

        /// Called after wiki parsing.
        /// Closes weight tags that are not open.
        virtual void finish() {}
        virtual ~Engine() = default;
    };


    namespace detail {
        struct ProbedWeight {
            Flags<Weight> flags;
            const char* posNext;
        };
        [[nodiscard]] ProbedWeight probeWeight(const char* pos, const char* end);
    }

    /// Does a simple check which bold/italic flags are inside text
    [[nodiscard]] Flags<Weight> probeWeights(const char* start, const char* end);
    [[nodiscard]] Flags<Weight> inline probeWeights(std::string_view x)
        { return probeWeights(x.data(), x.data() + x.size()); }
    [[nodiscard]] Flags<Weight> inline probeWeights (std::u8string_view x)
    {
        auto data = reinterpret_cast<const char*>(x.data());
        return probeWeights(data, data + x.size() );
    }

    void run(Engine& engine, const char* start, const char* end);
    inline void run(Engine& engine, std::string_view x)
        { run(engine, x.data(), x.data() + x.size()); }
    inline void run(Engine& engine, std::u8string_view x) {
        auto data = reinterpret_cast<const char*>(x.data());
        run(engine, data, data + x.size() );
    }

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

    // A helper class that keeps track of HTML state
    class HtWeight
    {
    public:
        constexpr HtWeight() = default;
        constexpr void clear() { fState = State::z; }
        /// @return HTML tags, just append them to your HTML
        /// Toggle those bits → change internal state ang return tag
        [[nodiscard]] std::string_view toggle(Flags<Weight> changed);
        /// Finish e.g. italic-bold → what’s tag?
        [[nodiscard]] std::string_view finishingTag() const;
        /// Restart e.g. italic-bold → what’s tag?
        [[nodiscard]] std::string_view restartingTag() const;
        /// What flags?  Both bold-italic and italic-bold are <b>
        [[nodiscard]] Flags<Weight> flags() const;
    private:
        enum class State { z, I, B, BI, IB, NN };
        static constexpr unsigned N_STATES = static_cast<unsigned>(State::NN);
        static constexpr unsigned N_FLAGS = 4;   ///< 0 = none, 1=i, 2=b, 3=b+i
        struct ToggleResult { std::string_view result; State state; };

        // Simple Mealy FSM here
        State fState = State::z;
        static const ToggleResult machine[N_STATES][N_FLAGS];
        static const std::string_view finishResult[N_STATES];
        static const std::string_view restartResult[N_STATES];
        static const Flags<Weight> flagsResult[N_STATES];
    };

}   // namespace wiki
