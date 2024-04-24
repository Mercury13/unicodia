#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace ucd {    

    struct HangulLine {
        char32_t cp;
        std::string name;
    };

    /// Info on Kangxi radicals: index (0 = none) + # of strokes
    struct Kx {
        short radical = 0;
        short plusStrokes = 0;
        static const Kx DFLT;
    };

    struct NumType {
        std::string_view id; ///< identifier for precompiled Unicode data
    };

    struct Numeric {
        const NumType* type = nullptr;
        std::string value;
    };

    struct PointingNumeric {
        const NumType* type = nullptr;  ///< While traversing UCD base, will never be NULL
        std::string_view value;

        auto& operator = (const Numeric& x) {
            type = x.type; value = x.value; return *this;
        }
    };

    struct SupportData {
        unsigned nBlocks = 0;
        std::vector<HangulLine> hangulLines;
        std::unordered_map<char32_t, Numeric> hanNumValues;
        std::unordered_map<char32_t, Kx> hanKangxi;
    };

    struct CpInfo {
        char32_t cp = 0;
        std::string_view name;
        std::string_view generalCat;
        std::string_view bidiCat;
        PointingNumeric numeric;
        char32_t upperCase = 0;     ///< 0 = none
        bool isMirrored = false;
        bool isDeprecated = false;
        Kx kx;
    };

    class BaseSink {    // interface
    public:
        virtual void act(const CpInfo&) const = 0;
        virtual ~BaseSink() = default;
    };

    template <class Body>
    class BaseSinkT final : public BaseSink {
    public:
        const Body& body;

        BaseSinkT(const Body& x) : body(x) {}
        void act(const CpInfo& x) const override { body(x); }
    };

    class DummySink : public BaseSink {
    public:
        virtual void act(const CpInfo&) const override {}
    };

    SupportData loadSupportData();
    void processMainBase(const SupportData& supportData, const BaseSink& sink);

    template <class Body>
    inline void processMainBaseT(const SupportData& supportData, const Body& body)
    {
        BaseSinkT<Body> sink(body);
        processMainBase(supportData, sink);
    }

}   // namespace ucd
