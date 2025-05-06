#pragma once

#include <unordered_map>
#include <string>

namespace kage {

    struct Hash : public std::hash<std::string_view> {
        using is_transparent = void;
        using std::hash<std::string_view>::operator ();
    };

    class Base
    {
    public:
        void add(std::string x, std::string y);

    private:
        struct Entry {
            std::string text;
            void operator = (std::string&& y) { text = y; }
        };
        std::unordered_map<std::string, Entry, Hash, std::equal_to<>> m;
    };

}
