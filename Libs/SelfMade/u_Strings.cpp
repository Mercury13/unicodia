#include "u_Strings.h"

template void str::trim<char>(const char*&, const char*&);
template void str::trim<wchar_t>(const wchar_t*&, const wchar_t*&);
template void str::trim<char8_t>(const char8_t*&, const char8_t*&);
template void str::trim<char16_t>(const char16_t*&, const char16_t*&);
template void str::trim<char32_t>(const char32_t*&, const char32_t*&);

template std::string_view str::detail::trimSv<std::string_view>(std::string_view);
template std::wstring_view str::detail::trimSv<std::wstring_view>(std::wstring_view);
template std::u8string_view str::detail::trimSv<std::u8string_view>(std::u8string_view);
template std::u16string_view str::detail::trimSv<std::u16string_view>(std::u16string_view);
template std::u32string_view str::detail::trimSv<std::u32string_view>(std::u32string_view);

template SafeVector<std::string_view> str::detail::splitSv<std::string_view>(std::string_view, char, bool);
template SafeVector<std::wstring_view> str::detail::splitSv<std::wstring_view>(std::wstring_view, wchar_t, bool);
template SafeVector<std::u8string_view> str::detail::splitSv<std::u8string_view>(std::u8string_view, char8_t, bool);
template SafeVector<std::u16string_view> str::detail::splitSv<std::u16string_view>(std::u16string_view, char16_t, bool);
template SafeVector<std::u32string_view> str::detail::splitSv<std::u32string_view>(std::u32string_view, char32_t, bool);


void str::toUpperInPlace(std::u8string& x)
{
    for (auto& v : x) {
        v = toupper(v);
    }
}


namespace {

    int CHAR_END = -1;

    inline int getChar(const char* &p, const char* end)
    {
        if (p == end)
            return CHAR_END;
        return *(p++);
    }

}


bool str::isIndex(std::string_view s)
{
    // Length
    if (s.size() < 2)
        return false;
    const char* p = s.data();
    const char* const end = p + s.size();
    // Alpha, then digit
    if (!isalpha(getChar(p, end)) || !isdigit(getChar(p, end)))
        return false;
    int a;
    do {
        a = getChar(p, end);
    } while (isdigit(a));
    // Alpha
    if (isalpha(a))
        a = getChar(p, end);
    // End;
    return (a == CHAR_END);
}
