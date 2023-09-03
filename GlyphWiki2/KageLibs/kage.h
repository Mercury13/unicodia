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

        auto& operator[](unsigned i) { return d[i]; }
        auto& operator[](unsigned i) const { return d[i]; }
    };

    enum class Style {
        SANS, SERIF,
        GOTHIC = SANS, MINCHO = SERIF
    };
    struct GlyphSets {
        Style style = Style::SANS;
        int kMinWidthY = 2;
        int kMinWidthU = 2;
        int kMinWidthT = 6;
        float kWidth = 5;
        float kKakato = 3;
        float kL2RDfatten = 1.1;
        float kMage = 10;
        int kUseCurve = 0;

        int kAdjustKakatoL[5] { 14, 9, 5, 2, 0 }; // for KAKATO adjustment 000,100,200,300,400
        int kAdjustKakatoR[4] { 8, 6, 4, 2 }; // for KAKATO adjustment 000,100,200,300
        int kAdjustKakatoRangeX = 20; // check area width
        int kAdjustKakatoRangeY[10] { 1, 19, 24, 30 }; // 3 steps of checking
        int kAdjustKakatoStep = 3; // number of steps

        int kAdjustUrokoX[4] { 24, 20, 16, 12 }; // for UROKO adjustment 000,100,200,300
        int kAdjustUrokoY[4] { 12, 11, 9, 8 }; // for UROKO adjustment 000,100,200,300
        int kAdjustUrokoLength[10] { 22, 36, 50 }; // length for checking
        int kAdjustUrokoLengthStep = 3; // number of steps
        int kAdjustUrokoLine[10] { 22, 26, 30 }; // check for crossing. corresponds to length

        int kAdjustUroko2Step = 3;
        int kAdjustUroko2Length = 40;

        int kAdjustTateStep = 4;

        int kAdjustMageStep = 5;
    };

    struct Glyph
    {
    public:
        SafeVector<Line> lines;
        void adjustStrokes(const GlyphSets& sets);
    private:
        void adjustHane();
    };

    Glyph toGlyph(
            std::string_view source,
            const SourceEngine& engine,
            const GlyphSets& sets);

}   // namespace kage
