#pragma once

#include "g2defines.h"

namespace g2bz {

    /// Makes a quad Bezier curve through three points and time
    /// @param [in] t   so-called “time”, (0..1)
    /// @return  control point of quad Bezier curve
    ///        whose 𝐜(0)=start, 𝐜(t)=mid, 𝐜(1)=end
    g2::Dpoint quadBy3time(
            const g2::Dpoint& start, const g2::Dpoint& mid,
            const g2::Dpoint& end, double t);

    /// Makes a quad Bezier curve through three points
    /// @return  control point of quad Bezier curve
    ///        whose 𝐜(0)=start, 𝐜(?)=mid, 𝐜(1)=end
    g2::Dpoint quadBy3(
            const g2::Dpoint& start, const g2::Dpoint& mid,
            const g2::Dpoint& end);

}   // namespace g2bz
