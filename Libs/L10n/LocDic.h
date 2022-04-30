#pragma once

#include <unordered_map>
#include <filesystem>
#include "u_Strings.h"

namespace loc {

    class Text
    {
    public:
        Text() noexcept = default;

        template <class T>
        Text(T&& x) : fSrc(std::forward<T>(x)), fIsFull(true) {}

        template <class T>
        Text& operator = (T&& x)
        {
            fSrc = std::forward<T>(x);
            fIsFull = true;
            return *this;
        }

        std::u8string_view u8() const { return fSrc; }
        operator std::u8string_view() const { return fSrc; }
        operator std::u8string() const { return fSrc; }
        const char* c_str() const { return str::toC(fSrc); }
        bool isFull() const { return fIsFull; }
    private:
        std::u8string fSrc;
        bool fIsFull = false;
    };

    class Dic
    {
    public:
        void clear() { fMap.clear(); }
        void add(std::string_view id, std::u8string translation);
        void dump(size_t maxSize = 0);
        const Text& get(std::string_view id);
    private:
        std::unordered_map<std::string, Text> fMap;
    };

    extern Dic dic;

    void loadIni(Dic& d, const std::filesystem::path& path);
    inline const Text& get(std::string_view id) { return dic.get(id); }

}   // namespace loc
