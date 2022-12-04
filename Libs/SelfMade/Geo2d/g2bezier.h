#pragma once

// STL
#include <optional>

// Geo2d
#include "g2defines.h"

namespace g2bz {

    struct Quad {
        g2::Dpoint a, m, b;
        g2::Dvec armA() const { return m - a; }
        g2::Dvec armB() const { return m - b; }
        double distFrom(const g2::Dpoint& p) const;

        /// Makes a quad Bezier curve through three points and time
        /// @param [in] t   so-called “time”, (0..1)
        /// @return  quad Bezier whose a=start, 𝐜(t)=mid, b=end
        static Quad by3time(
                const g2::Dpoint& start, const g2::Dpoint& mid,
                const g2::Dpoint& end, double t);

        /// Makes a quad Bezier curve through three points
        /// @return  quad Bezier whose a=start, 𝐜(?)=mid, b=end
        static Quad by3(
                const g2::Dpoint& start, const g2::Dpoint& mid,
                const g2::Dpoint& end);

        /// Makes a quad Bezier curve through three points
        /// but other policy of making best time — so-called “quadratic” policy
        ///   (longer segment = more straight)
        /// @return  quad Bezier whose a=start, 𝐜(?)=mid, b=end
        static Quad by3q(
                const g2::Dpoint& start, const g2::Dpoint& mid,
                const g2::Dpoint& end);

        static std::optional<Quad> by2tan(
                const g2::Dpoint& a, const g2::Dvec& ah,
                const g2::Dvec& bh, const g2::Dpoint& b);

    };



}   // namespace g2bz
