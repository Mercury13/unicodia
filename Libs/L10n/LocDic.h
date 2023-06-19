#pragma once

#include <unordered_map>
#include <filesystem>

// Libs
#include "u_Strings.h"

// L10n
#include "LocFmt.h"

#ifdef QT_STRINGS
    #include <QString>
#endif

namespace loc {

    class Text
    {
    public:
        Text() noexcept = default;
        Text(const Text&) = delete;
        Text(Text&&) = default;
        Text& operator = (const Text&) = delete;
        Text& operator = (Text&&) = default;

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
        operator const std::u8string&() const { return fSrc; }
        const char* c_str() const { return str::toC(fSrc); }
        const std::u8string& str() const { return fSrc; }
        bool isFull() const { return fIsFull; }
    #ifdef QT_STRINGS
        QString q() const { return QString::fromUtf8(fSrc.data(), fSrc.size()); }
        operator QString() const { return q(); }
    #endif
        template <class... T>
        std::u8string arg(const T&... x) const;
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
        const Text* getIf(std::string_view id) const;
    private:
        std::unordered_map<std::string, Text> fMap;
    };

    extern Dic dic;

    void loadIni(Dic& d, const std::filesystem::path& path);
    inline const Text& get(std::string_view id) { return dic.get(id); }

}   // namespace loc


template <class... T>
std::u8string loc::Text::arg(const T&... x) const
    { return loc::Fmt(fSrc)(x...).giveStr(); }
