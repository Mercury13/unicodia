#pragma once

#include <string>

#include "u_Array.h"
#include "u_Vector.h"

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

    constexpr std::string_view SEPARATORS = "\r" "\n" "$";
    constexpr int SIZE = 200;

    SafeVector<std::string_view> splitIntoLinesSv(std::string_view source);

    struct Line {
        Fix1d<float, 11> d;
        Line() noexcept
            { std::fill(d.begin(), d.end(), std::numeric_limits<float>::quiet_NaN()); }
    };

    struct Glyph {
        SafeVector<Line> lines;
        std::string toSvg() const;
    };

    Glyph toGlyph(std::string_view source, const SourceEngine& engine);
    std::string toSvg(std::string_view source, const SourceEngine& engine);

}   // namespace kage
