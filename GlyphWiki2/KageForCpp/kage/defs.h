#pragma once

namespace kage {

    namespace dim {
        constexpr unsigned MIN = 0;
        constexpr unsigned MAX = 200;
    }

    namespace stroke {
        constexpr unsigned STRAIGHT = 1;
        constexpr unsigned CURVE = 2;           ///< quadratic Bezier curve
        constexpr unsigned BENDING = 3;         ///< used in "札" "己"
        constexpr unsigned BENDING_ROUND = 4;   ///< used for "乙"
        constexpr unsigned BEZIER = 6;          ///< cubic Bezier curve
        constexpr unsigned VCURVE = 7;          ///< vertical line and curve. used in the leftmost stroke of "月".
                            ///< although the angle of line can be chosen arbitrarily, only vertical line is expected.
        constexpr unsigned REFERENCE = 99;
    }

    namespace start {
        // INTERNAL_FOR_BENDING : 6,
        // INTERNAL_PLANE : 1,
        constexpr unsigned OPEN =  0;  ///< simple lines like "三" or "川" (two strokes on the right side)
                            ///< also used in the left stroke of "人"
        constexpr unsigned CONNECTING_H = 2; ///< horizontal strokes connecting to other strokes.  used in the center strokes of "日".
        constexpr unsigned UPPER_LEFT_CORNER = 12; ///< the starting point is at the upper left corner.  usually used for vertical lines, like the leftmost stroke of "日".
        constexpr unsigned UPPER_RIGHT_CORNER = 22; ///< the starting point is at the upper right corner.  usually used for vertical lines, like the rightmost stroke of "日".
        constexpr unsigned CONNECTING_V = 32; ///< vertical strokes connecting to other strokes.  used in the center strokes of "工".
        constexpr unsigned THIN = 7; ///< used in the right stroke of "人"
        constexpr unsigned ROOFED_THIN = 27; ///< used in the right stroke of "入"
        //////custom
        constexpr unsigned CONNECT_THIN = 37;
        constexpr unsigned CONNECTING_MANUAL = 39; ///<the edge is cut in a certain direction; the degree is determined by hidden parameter. This shape is for straight line, but should be encoded as CURVE data
                                                ///< 40-60 is used for curve strokes
    }

    namespace end {
        // INTERNAL_LAST_FILLED_CIRCLE : 1,
        // INTERNAL_TURN_LEFT : 14,//used in original KAGE-engine implementation
        // INTERNAL_TURN_UPWARDS : 15,//used in original KAGE-engine implementation

        constexpr unsigned OPEN = 0; ///< simple lines like "三" or "川" (two strokes on the right side)
                                     ///< also used in the right stroke of "人"(L2R sweep)
        constexpr unsigned CONNECTING_H  = 2;
        constexpr unsigned CONNECTING_V  = 32; ///vertical strokes connecting to other strokes.  used in the center strokes of "工".
        constexpr unsigned LOWER_LEFT_CORNER = 13;
        constexpr unsigned LOWER_RIGHT_CORNER = 23;
        constexpr unsigned LOWER_LEFT_ZH_OLD = 313; ///< for characters used in China.
        constexpr unsigned LOWER_LEFT_ZH_NEW = 413; ///< for characters used in China.

        constexpr unsigned TURN_LEFT = 4; ///< adds a short line to the left.  used in the rightmost stroke of "月".
        constexpr unsigned LOWER_RIGHT_HT = 24; ///< for characters used in China.
        constexpr unsigned TURN_UPWARDS = 5; ///< adds a short upward line.  used in the rightmost stroke of "札" or "風".
        constexpr unsigned LEFT_SWEEP = 7;  ///< thin end; used in the left stroke of "人".
        constexpr unsigned STOP = 8; ///< used in the rightmost stroke of "小" or lower four dots of "魚".
    }

}   // namespace kage
