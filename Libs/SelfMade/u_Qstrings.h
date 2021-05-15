#pragma once

#include <QString>
#include <string_view>

namespace str {

    inline QString toQ(std::string_view x)
        { return QString::fromUtf8(x.data(), x.size()); }

    inline QString toQ(char32_t x)
    {
        uint32_t cp1 = x;
        return QString::fromUcs4(&cp1, 1);
    }

    void append(QString& x, std::string_view s);

    class QSep
    {
    public:
        QSep(QString& aS, const QString& aSep) : s(aS), len(aS.length()), fSep(aSep) {}
        void sep();
    private:
        QString& s;
        int len;
        QString fSep;
    };
}   // namespace str
