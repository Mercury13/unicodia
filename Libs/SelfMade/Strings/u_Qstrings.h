#pragma once

#include <QString>
#include <QStringView>

#include <string_view>
#include <charconv>

namespace str {

    inline QString toQ(std::string_view x)
        { return QString::fromUtf8(x.data(), x.size()); }

    inline QString toQ(std::wstring_view x)
        { return QString::fromWCharArray(x.data(), x.size()); }

    inline QString toQ(std::u8string_view x)
        { return QString::fromUtf8(x.data(), x.size()); }

    inline QString toQ(char32_t x)
        { return QString::fromUcs4(&x, 1); }

    inline QString toQ(std::u16string_view x)
        { return QString::fromUtf16(x.data(), x.size()); }

    inline QString toQ(std::u32string_view x)
        { return QString::fromUcs4(x.data(), x.size()); }

    std::u8string_view toU8sv(const QString& x, std::string& cache);
    std::u8string toU8(const QString& x);

    void append(QString& x, std::string_view s);
    void append(QString& x, std::u8string_view s);
    void append(QString& x, std::u32string_view s);
    //inline void append(QString& x, const char* s) { append(x, std::string_view(s)); }
    inline void append(QString& x, char c) { x.append(c); }
    void append(QString& x, char32_t c);

    template <size_t N>
    inline void append(QString& x, const char (&s)[N]) {
        if constexpr (N > 1) {
            append(x, std::string_view(s, N - 1));
        }
    }

    template <size_t N>
    inline void append(QString& x, char (&s)[N])
        { append(x, std::string_view(s, strnlen(s, N))); }

    template <class T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
    void append(QString& x, T y)
    {
        char buf[30];
        auto beg = std::begin(buf);
        auto end = std::end(buf);
        auto res = std::to_chars(beg, end, y);
        x.append(QByteArray::fromRawData(beg, res.ptr - beg));
    }

    template <class T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
    void appendHex(QString& x, T y)
    {
        char buf[30];
        auto beg = std::begin(buf);
        auto end = std::end(buf);
        auto res = std::to_chars(beg, end, y, 16);
        x.append(QByteArray::fromRawData(beg, res.ptr - beg));
    }

    class QSep
    {
    public:
        QSep(QString& aS, const QString& aSep) : s(aS), len(aS.length()), fSep(aSep) {}
        QSep(QString& aS, std::u8string_view aSep) : s(aS), len(aS.length()), fSep(str::toQ(aSep)) {}
        QSep(const QSep&) = delete;
        void sep();
        QString& target() const noexcept { return s; }
        void reset() noexcept { len = s.length(); }
    private:
        QString& s;
        int len;
        QString fSep;
    };

}   // namespace str
