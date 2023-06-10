#pragma once

#include <unordered_map>
#include <filesystem>
#include "u_Strings.h"

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
        template <class T>
        void aaInc(std::u8string& r, char8_t (&subst)[4], const T& x) const;

        static void aa(std::u8string& r, const char8_t* subst, std::u8string_view x);
        static void aa(std::u8string& r, const char8_t* subst, std::string_view x);
        static void aa(std::u8string& r, const char8_t* subst, int x);
        static void aa(std::u8string& r, const char8_t* subst, unsigned x);
        static void aa(std::u8string& r, const char8_t* subst, long long x);

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


template <class T>
void loc::Text::aaInc(std::u8string& r, char8_t (&subst)[4], const T& x) const
{
    aa(r, subst, x);
    ++subst[1];
}

template <class... T>
std::u8string loc::Text::arg(const T&... x) const
{
    static_assert(sizeof...(T) >= 1 && sizeof...(T) <= 9, "Need 1 to 9 args");
    std::u8string r = fSrc;
    if constexpr (sizeof...(T) == 1) {
        aa(r, u8"{1}", x...);
    } else {
        char8_t su[4] = u8"{1}";
        (aaInc(r, su, x), ...);
    }
    return r;
}
