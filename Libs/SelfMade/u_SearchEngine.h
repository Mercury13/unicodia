#pragma once

#include <string>

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

}
