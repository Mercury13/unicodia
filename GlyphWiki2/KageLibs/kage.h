#pragma once

#include <string>

// Libs
#include "u_Array.h"
#include "u_Vector.h"
#include "u_MaybeInt.h"

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
    constexpr int GLYPH_SIZE = 200;

    SafeVector<std::string_view> splitIntoLinesSv(std::string_view source);

    constexpr unsigned LINE_SIZE = 11;
    struct Line {
        using T = MaybeInt<int>;
        Fix1d<T, LINE_SIZE> d;
    };

    struct Glyph {
        SafeVector<Line> lines;
        std::string toSvg() const;
    };

    Glyph toGlyph(std::string_view source, const SourceEngine& engine);
    std::string toSvg(std::string_view source, const SourceEngine& engine);

}   // namespace kage
