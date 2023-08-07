#pragma once

#include <string>

namespace kage {

    class SourceEngine
    {
    public:
        virtual std::string getKage(std::string_view name) const = 0;
        virtual ~SourceEngine() = default;
    };

    class NoSource : public SourceEngine
    {
    public:
        std::string getKage(std::string_view name) const override;
        static const NoSource INST;
    };

    class BadSource : public SourceEngine
    {
    public:
        std::string getKage(std::string_view name) const override;
        static const BadSource INST;
    };

    std::string toSvg(std::string_view source, const SourceEngine& engine);

}   // namespace kage
