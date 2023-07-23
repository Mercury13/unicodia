#pragma once

#include <string_view>
#include <string>
#include <compare>
#include <span>

#ifdef QT_STRINGS
    #include <QString>
#endif

struct Version
{
    unsigned part1 = 0, part2 = 0, part3 = 0;

    constexpr Version() = default;
    constexpr explicit Version(unsigned p1) : part1(p1) {}
    constexpr Version(unsigned p1, unsigned p2) : part1(p1), part2(p2) {}
    constexpr Version(unsigned p1, unsigned p2, unsigned p3) : part1(p1), part2(p2), part3(p3) {}

    constexpr std::strong_ordering operator <=>(const Version& other) const = default;

    static_assert(sizeof(char) == sizeof(char8_t));

    /// @warning  Works like snprintf
    ///    1. data is always null-terminated.
    ///    2. Returns data size as if there were enough buffer.
    ///    3. Actually need ret+1 bytes to write data completely.
    size_t sprintf(char* data, size_t size) const;
    size_t sprintf(std::span<char> data) const { return sprintf(data.data(), data.size()); }
    size_t sprintf(char8_t* data, size_t size) const
        { return sprintf(reinterpret_cast<char*>(data), size); }
    size_t sprintf(std::span<char8_t> data) const { return sprintf(data.data(), data.size()); }

    std::string_view toSv(std::span<char> data) const;
    std::u8string_view toSv(std::span<char8_t> data) const;

    std::string toS() const;

    [[nodiscard]] bool hasSmth() const
        { return (part1 != 0 || part2 != 0 || part3 != 0); }
    explicit operator bool() const { return hasSmth(); }

    /// Does permissive parsing:
    /// Finds digit, then parses version within string
    ///   v.3.4alpha → (3,4,0)
    static Version parsePermissive(std::string_view text);

#ifdef QT_STRINGS
    QString toQ() const;
    static Version parsePermissive(const QString& text);
#endif

};
