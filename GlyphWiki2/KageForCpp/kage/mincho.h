#pragma once

#include "style.h"
#include "2d.h"

namespace kage {

    struct MinchoSets {
        // Things suitable for size = 1
        Float kMinWidthY = 1.2;
        Float kMinWidthU = 1.2;
        Float kMinWidthT = 3.6;
        Float kWidth = 3;
        Float kKakato = 1.8;
        Float kL2RDfatten = 1.1;
        Float kMage = 6;

        FixedList<unsigned> kAdjustKakatoL = { 8, 5, 3, 1 }; // for KAKATO adjustment 000,100,200,300,400
        FixedList<unsigned> kAdjustKakatoR = { 4, 3, 2, 1 }; // for KAKATO adjustment 000,100,200,300
        unsigned kAdjustKakatoRangeX = 12; // check area width
        FixedList<unsigned> kAdjustKakatoRangeY = { 1, 11, 14, 18 }; // 3 steps of checking
        unsigned kAdjustKakatoStep = 3; // number of steps

        FixedList<unsigned> kAdjustUrokoX = { 14, 12, 9, 7 }; // for UROKO adjustment 000,100,200,300
        FixedList<unsigned> kAdjustUrokoY = { 7, 6, 5, 4 }; // for UROKO adjustment 000,100,200,300
        FixedList<unsigned> kAdjustUrokoLength = { 13, 21, 30 }; // length for checking
        unsigned kAdjustUrokoLengthStep = 3; // number of steps
        FixedList<unsigned> kAdjustUrokoLine = { 13, 15, 18 }; // check for crossing. corresponds to length

        unsigned kAdjustUroko2Step = 3;
        unsigned kAdjustUroko2Length = 25;

        unsigned kAdjustTateStep = 4;
        Float kAdjustMageStep = 5;
        Float kMinWidthT_adjust = 6;

        float kMinWidthYY = 2;

        static const MinchoSets ONE;
    };

    class Mincho : public Style
    {
    public:
        MinchoSets sets;
        Drawing draw(std::span<const Stroke> strokes) const override;
        static Mincho DFLT;
    private:
        /// @warning  You must search and skip stroke in all
        void drawAdjusted(Drawing& r, const Stroke& stroke, std::span<const Stroke>& all) const;
        Float adjustTateParam(const Stroke& stroke, std::span<const Stroke>& all) const;
        Float adjustMageParam(const Stroke& stroke, std::span<const Stroke>& all) const;
        Polygon getStartOfVLine(Point<int> p1, Point<int> p2, int a1,
                                Float kMinWidthT, Drawing& cv) const;
        kage::Polygon getStartOfLine(Point<Float> pt, Dir dir, Float halfWidth) const;
        kage::Polygon getStartOfOffsetLine(
                Point<Float> pt, Dir dir, Float halfWidth, Float off_right1, Float off_left1) const;
    };

}   // namespace kage
