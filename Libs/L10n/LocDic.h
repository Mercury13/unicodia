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

    enum class TextState : unsigned char {
        NOT_FOUND,      // Just not found
        UNTRANSLATED,   // Found in backup L10n resource
        NORMAL          // Found in L10n resource
    };

    class Text
    {
    public:
        Text() noexcept = default;
        Text(const Text&) = delete;
        Text(Text&&) = default;
        Text& operator = (const Text&) = delete;
        Text& operator = (Text&&) = default;

        template <class T>
        explicit Text(T&& x, TextState state)
            : fSrc(std::forward<T>(x)), fState(state) {}

        template <class T>
        void assign(T&& x, TextState state) {
            fSrc = std::forward<T>(x);
            fState = state;
        }

        std::u8string_view u8() const { return fSrc; }
        operator std::u8string_view() const { return fSrc; }
        operator const std::u8string&() const { return fSrc; }
        const char* c_str() const { return str::toC(fSrc); }
        const std::u8string& str() const { return fSrc; }
        TextState state() const { return fState; }
        template <class... T>
            std::u8string arg(const T&... x) const;

        /// Equiv. to arg, but the number is preformatted
        template <std::integral T>
            std::u8string preformNum(std::string_view preform, T val) const;

    #ifdef QT_STRINGS
        // QString functions
        QString q() const { return QString::fromUtf8(fSrc.data(), fSrc.size()); }
        operator QString() const { return q(); }
        template <class... T>
            QString argQ(const T&... x) const;
    #endif

    private:
        std::u8string fSrc;
        TextState fState = TextState::UNTRANSLATED;
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

    ///  @return  [+] OK [-] troubles
    ///  @warning  Always clears, even if returns false
    bool loadIni(Dic& d, const std::filesystem::path& path);

    inline const Text& get(std::string_view id) { return dic.get(id); }

}   // namespace loc


template <class... T>
std::u8string loc::Text::arg(const T&... x) const
{
    loc::FmtL<char8_t> fmt(fSrc);
    (fmt.eat(x), ...);
    return fmt.giveStr();
}


template <std::integral T>
std::u8string loc::Text::preformNum(std::string_view preform, T val) const
{
    loc::FmtL<char8_t> fmt(fSrc);
    fmt.preformN(str::toU8sv(preform), val);
    return fmt.giveStr();
}


#ifdef QT_STRINGS
    template <class... T>
    QString loc::Text::argQ(const T&... x) const
    {
        loc::FmtL<char8_t> fmt(fSrc);
        (fmt.eat(x), ...);
        return fmt.q();
    }
#endif
