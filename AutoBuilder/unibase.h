#pragma once

#include <string_view>

namespace ucd {

    struct CpInfo {
        char32_t cp = 0;
        std::string_view name;
        std::string_view generalCat;
        std::string_view bidiCat;
        std::string_view decimalValue;
        std::string_view digitValue;
        std::string_view numericValue;
        char32_t upperCase = 0;     ///< 0 = none
        bool isMirrored = false;
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

    void processMainBase(const BaseSink& sink);

    template <class Body>
    inline void processMainBaseT(const Body& body)
    {
        BaseSinkT<Body> sink(body);
        processMainBase(sink);
    }

    class DummySink : public BaseSink {
    public:
        virtual void act(const CpInfo&) const override {}
    };

}   // namespace ucd
