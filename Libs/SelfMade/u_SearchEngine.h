#pragma once

#include <string>

#include "u_Vector.h"

namespace srh {

    enum class Class { OTHER, LETTER, DIGIT };

    Class classify(char8_t x);

    struct Word {
        std::u8string v;
        Class ccFirst = Class::OTHER, ccLast = Class::OTHER;

        Word() = default;
        Word(std::u8string x)
            : v(std::move(x)), ccFirst(classify(v.front())), ccLast(classify(v.back())) {}

        std::u8string_view sv() const { return v; }
        size_t length() const { return v.length(); }
    };

    enum class Place { NONE, PARTIAL, INITIAL, EXACT };

    struct Prio {
        int high = 0, exact = 0, initial = 0, partial = 0;
        std::partial_ordering operator <=>(const Prio& x) const = default;
        static const Prio EMPTY;
    };

    struct Needle
    {
        SafeVector<srh::Word> words;

        Needle(std::u8string_view x);
    };

    Place findWord(std::u8string_view haystack, const Word& needle);
    Prio findNeedle(std::u8string_view haystack, const Needle& needle);
}
