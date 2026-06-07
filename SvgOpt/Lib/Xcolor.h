#pragma once

#include <optional>
#include <string>

namespace xs {
    struct Color {
        unsigned char r = 0, g = 0, b = 0;

        bool operator == (const Color&) const noexcept = default;
        void encodeAttr(std::string& dest) const;
        bool isShort() const noexcept;
        static std::optional<Color> parse(std::string_view x);
    };

}