#pragma once

#include "2dstroke.h"
#include "style.h"

#include <unordered_map>
#include <string>
#include <vector>
#include <memory>

namespace kage {

    using Structure = std::vector<Stroke>;

    struct Hash : public std::hash<std::string_view> {
        using is_transparent = void;
        using std::hash<std::string_view>::operator ();
    };

    class Base
    {
    public:
        void add(std::string x, std::string y);
        std::shared_ptr<const Structure> structureOf(std::string_view x) const;
        Drawing drawingOf(std::string_view x, const Style& style) const;

    private:
        struct Entry {
            std::string text;
            mutable std::shared_ptr<const Structure> cache;

            void operator = (std::string&& y) { text = y; cache.reset(); }
        };
        std::unordered_map<std::string, Entry, Hash, std::equal_to<>> m;
    };

}
