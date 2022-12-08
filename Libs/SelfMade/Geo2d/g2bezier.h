#pragma once

// STL
#include <optional>

// Geo2d
#include "g2defines.h"

namespace g2bz {

    struct Seg {
        g2::Dpoint a, b;

        double distFrom2(const g2::Dpoint& p);
    };

    struct Quad {
        g2::Dpoint a, m, b;

        /// @return  arm/handle vector pointing from A
        g2::Dvec armA() const noexcept { return m - a; }

        /// @return  arm/handle vector pointing from B
        g2::Dvec armB() const noexcept { return m - b; }

        /// @return  arm/handle vector of equivalent cubic curve pointing from A
        g2::Dvec cubicArmA() const noexcept { return armA() * (2.0/3.0); }

        /// @return  arm/handle vector of equivalent cubic curve pointing from B
        g2::Dvec cubicArmB() const noexcept { return armB() * (2.0/3.0); }

        /// @return fast approximated (Nehab-Hoppe) squared distance from curve to point
        /// @see https://hhoppe.com/ravg.pdf
        /// @see https://www.shadertoy.com/view/MlKcDD
        double fastDistFrom2(const g2::Dpoint& p) const;

        /// @return fast approximated (Nehab-Hoppe) distance from curve to point
        double fastDistFrom(const g2::Dpoint& p) const
            { return std::sqrt(fastDistFrom2(p)); }

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

        /// Makes a quad Bezier curve by two points and two tangents
        /// @return [-] impossible:
        ///         * any of segments is 0
        ///         * any of segments are collinear
        ///         * tangents point to different half-planes (inflection point)
        ///         * tangents point outwards (they intersect when continued backwards)
        static std::optional<Quad> by2tan(
                const g2::Dpoint& a, const g2::Dvec& ah,
                const g2::Dvec& bh, const g2::Dpoint& b);

    };



}   // namespace g2bz
