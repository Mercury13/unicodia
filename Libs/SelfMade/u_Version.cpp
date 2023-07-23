#include "u_Version.h"

#include <algorithm>
#include <charconv>

namespace {

    bool isDigit(char c) { return (c >= '0' && c <= '9'); }

}   // anon namespace

Version Version::parsePermissive(std::string_view text)
{
    auto p = std::to_address(text.begin());
    const auto end = std::to_address(text.end());

    // Skip non-numeric
    Version r;
    p = std::find_if(p, end, isDigit);
    if (p == end)
        return r;

    // Read 1st data
    auto q = std::from_chars(p, end, r.part1);
    if ((q.ptr == nullptr || q.ptr == end) || *q.ptr != '.')
        return r;

    // Read 2nd data
    p = q.ptr + 1;
    if (p == end || !isDigit(*p))
        return r;
    q = std::from_chars(p, end, r.part2);
    if ((q.ptr == nullptr || q.ptr == end) || *q.ptr != '.')
        return r;

    // Read 3rd data
    p = q.ptr + 1;
    if (p == end || !isDigit(*p))
        return r;
    std::from_chars(p, end, r.part3);
    return r;
}


size_t Version::sprintf(char* data, size_t size) const
{
    if (part3 != 0) {
        return snprintf(data, size, "%u.%u.%u", part1, part2, part3);
    } else {
        return snprintf(data, size, "%u.%u", part1, part2);
    }
}


std::string_view Version::toSv(std::span<char> data) const
{
    auto n = sprintf(data);
    return { data.data(), n };
}


std::u8string_view Version::toSv(std::span<char8_t> data) const
{
    auto n = sprintf(data);
    return { data.data(), n };
}


std::string Version::toS() const
{
    char q[50];
    auto n = sprintf(q, std::size(q));
    return { q, n };
}


#ifdef QT_STRINGS
    QString Version::toQ() const
    {
        char q[50];
        auto n = sprintf(q, std::size(q));
        return QString::fromUtf8(q, n);
    }


    Version Version::parsePermissive(const QString& text)
    {
        return parsePermissive(text.toStdString());
    }
#endif
