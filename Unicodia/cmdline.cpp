#include "cmdline.h"

// Uses GetCommandLine → supports Unicode
#include <QCoreApplication>


namespace {

    struct ParseOne {
        QString key, value;
        operator bool() const noexcept {
            return !key.isEmpty();
        }
    };

    ParseOne parseOne(const QString& v)
    {
        if (v.length() < 2 || !v.startsWith('/'))
            return {};
    // NOILINTBEGIN(bugprone-macro-parentheses)
    #define DEF_VAR(name, ch) \
        auto name = v.indexOf(ch);   \
        if (name < 0) {              \
            name = v.length();       \
        } else if (name < 1) {       \
            return {};            \
        }
    // NOLINTEND(bugprone-macro-parentheses)
        DEF_VAR(p, '=')
        DEF_VAR(p2, ':')
        p = std::min(p, p2);
        return {
            .key = v.mid(1, p - 1).trimmed(),
            .value = v.mid(p + 1).trimmed()
        };
    #undef DEF_VAR
    }

    // Auto-conversion from various chars, U16 only for now
    template <unsigned Sz> struct ToSv;

    template <> struct ToSv<sizeof(char16_t)> {
        static inline std::u16string_view run(const QString& x)
        {
            return {
                reinterpret_cast<const char16_t*>(x.data()),
                size_t(x.length()) };
        }
    };

    inline auto toSv(const QString& x) {
        return ToSv<sizeof(QChar)>::run(x);
    }

}   // anon namespace


Cmdline Cmdline::parse()
{
    Cmdline r;
    auto args = QCoreApplication::arguments();
    for (auto i = 1; i < args.size(); ++i) {
        if (auto p = parseOne(args[i])) {
            switch (p.key[0].unicode()) {
            case 'q':
                if (p.key == "qafonts") {
                    r.qa.fonts = toSv(p.value);
                }
            }
        }
    }
    return r;
}
