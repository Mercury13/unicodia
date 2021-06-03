#pragma once

#include <algorithm>

namespace detail {

    enum class Place { BEFORE = -1, FOUND = 0, AFTER = 1 };

    // For UT’ability
    template <class It, class T, class Compare>
    constexpr Place upperBoundHint(It first, It last, const T& value, Compare cmp, It hint)
    {
        // upper_bound returns the first it whose cmp(value, *it) is true, i.e. *it > value

        if (hint != last && !cmp(value, *hint))
            return Place::AFTER;

        if (hint != first) {
            auto prev = hint;
            --prev;
            return cmp(value, *prev) ? Place::BEFORE : Place::FOUND;
        } else {
            return Place::FOUND;
        }
    }

    template <class It, class T>
    constexpr Place upperBoundHint(It first, It last, const T& value, It hint)
    {
        // upper_bound returns the first it whose op < (value, *it) is true, i.e. *it > value

        if (hint != last && !operator< (value, *hint))
            return Place::AFTER;

        if (hint != first) {
            auto prev = hint;
            --prev;
            return operator< (value, *prev) ? Place::BEFORE : Place::FOUND;
        } else {
            return Place::FOUND;
        }
    }

}   // namespace detail


template <class It, class T, class Compare>
constexpr It hintedUpperBound(It first, It last, const T& value, Compare cmp, It hint)
{
    if (detail::upperBoundHint(first, last, value, cmp, hint))
        return hint;
    return std::upper_bound(first, last, value, cmp);
}

template <class It, class T>
constexpr It hintedUpperBound(It first, It last, const T& value, It hint)
{
    if (detail::upperBoundHint(first, last, value, hint))
        return hint;
    return std::upper_bound(first, last, value);
}
