#include "u_Strings.h"


void str::trim(const char* &start, const char* &end)
{
    while (start != end
           && isBlank(*start))
        ++start;
    if (start == end) return;
    while (isBlank(*(end - 1)))
        --end;
}


void str::trim(const char8_t* &start, const char8_t* &end)
{
    while (start != end
           && isBlank(*start))
        ++start;
    if (start == end) return;
    while (isBlank(*(end - 1)))
        --end;
}


std::string_view str::trimSv(std::string_view s)
{
    const char* start = s.data();
    const char* end = start + s.length();
    trim(start, end);
    return std::string_view(start, end - start);
}

SafeVector<std::string_view> str::splitSv(std::string_view s, char comma, bool skipEmpty)
{
    SafeVector<std::string_view> r;

    const char* start = s.data();
    const char* end = start + s.length();
    str::trim(start, end);
    if (start == end)
        return r;

    const char *sstart = start;
    for (const char *p = start; p != end; ++p)
    {
        if (*p != comma) continue;
        const char *send = p;
        str::trim(sstart, send);
        if (p != sstart || !skipEmpty)
            r.emplace_back(sstart, send-sstart);
        sstart = p + 1;
    }
    str::trim(sstart, end);
    if (sstart != end || !skipEmpty)
        r.emplace_back(sstart, end-sstart);
    return r;
}


SafeVector<std::u8string_view> str::splitSv(std::u8string_view s, char comma, bool skipEmpty)
{
    SafeVector<std::u8string_view> r;

    const char8_t* start = s.data();
    const char8_t* end = start + s.length();
    str::trim(start, end);
    if (start == end)
        return r;

    const char8_t *sstart = start;
    for (const char8_t *p = start; p != end; ++p)
    {
        if (*p != comma) continue;
        const char8_t *send = p;
        str::trim(sstart, send);
        if (p != sstart || !skipEmpty)
            r.emplace_back(sstart, send-sstart);
        sstart = p + 1;
    }
    str::trim(sstart, end);
    if (sstart != end || !skipEmpty)
        r.emplace_back(sstart, end-sstart);
    return r;
}


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
