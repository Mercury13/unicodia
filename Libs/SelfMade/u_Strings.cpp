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
