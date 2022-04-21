// My header
#include "u_Qstrings.h"

void str::append(QString& x, std::string_view s)
{
    x.append(QByteArray::fromRawData(s.data(), s.size()));
}


void str::append(QString& x, char32_t c)
{
    if (c < 0x1'0000) {
        x.append(QChar{c});
    } else {
        x.append(str::toQ(c));
    }
}


void str::append(QString& x, std::u8string_view s)
{
    x.append(QByteArray::fromRawData(
                 reinterpret_cast<const char*>(s.data()), s.size()));
}


void str::QSep::sep()
{
    if (s.length() != len)
        s.append(fSep);
}


std::u8string_view str::toU8sv(const QString& x, std::string& cache)
{
    cache = x.toStdString();
    return { reinterpret_cast<const char8_t*>(cache.data()), cache.length() };
}


std::u8string str::toU8(const QString& x)
{
    std::string cache;
    return std::u8string { toU8sv(x, cache) };
}
