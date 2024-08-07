#pragma once

// STL
#include <optional>

// Geo2d
#include "g2defines.h"

namespace g2bz {

    struct Seg {
        g2::Dpoint a, b;

        /// @return  squared distance from line segment to point
        double dist2from(const g2::Dpoint& p) const noexcept;

        /// @return  distance from line segment to point
        double distFrom(const g2::Dpoint& p) const noexcept
            { return std::sqrt(dist2from(p)); }

        g2::Dpoint posOf(double t) const noexcept;

        /// Swaps a ↔ b, making posOf′(t) = posOf(1-t)
        void reverse() noexcept { std::swap(a, b); }

        double len() const noexcept { return a.distFromD(b); }
    };

    struct Quad {
        g2::Dpoint a, m, b;

        /// @return length of quad’s base
        double baseLen() const noexcept { return a.distFromD(b); }

        /// @return  arm/handle vector pointing from A
        g2::Dvec armA() const noexcept { return m - a; }

        /// @return  arm/handle vector pointing from B
        g2::Dvec armB() const noexcept { return m - b; }

        /// @return  arm/handle vector of equivalent cubic curve pointing from A
        g2::Dvec cubicArmA() const noexcept { return armA() * (2.0/3.0); }

        /// @return  arm/handle vector of equivalent cubic curve pointing from B
        g2::Dvec cubicArmB() const noexcept { return armB() * (2.0/3.0); }

        g2::Dpoint posOf(double t) const noexcept;

        /// @return fast approximated (Nehab-Hoppe) squared distance from curve to point
        /// @see https://hhoppe.com/ravg.pdf
        /// @see https://www.shadertoy.com/view/MlKcDD
        double fastDist2from(const g2::Dpoint& p) const;

        /// @return fast approximated (Nehab-Hoppe) distance from curve to point
        double fastDistFrom(const g2::Dpoint& p) const
            { return std::sqrt(fastDist2from(p)); }

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

        /// Swaps a ↔ b, making posOf′(t) = posOf(1-t)
        void reverse() noexcept { std::swap(a, b); }

        std::optional<g2::Dpoint> extremumX() const noexcept;
        std::optional<g2::Dpoint> extremumY() const noexcept;

        /// Makes a.x = m.x
        void straightenAX() noexcept;
        /// Makes a.y = m.y
        void straightenAY() noexcept;
        /// Makes b.x = m.x
        void straightenBX() noexcept;
        /// Makes b.y = m.y
        void straightenBY() noexcept;

        /// Makes |armA| = newL
        void scaleArmAToLength(double newL) noexcept;
        /// Makes |armB| = newL
        void scaleArmBToLength(double newL) noexcept;
    };

}   // namespace g2bz
