#include "Xstyle.h"

#include "u_Strings.h"

using namespace std::string_view_literals;

///// MaybeColor ///////////////////////////////////////////////////////////////

void xs::MaybeColor::encodeAttr(std::string& dest) const
{
    switch (index()) {
    case I_INHERIT:
    default:
        break;
    case I_COLOR:
        if (auto* q = std::get_if<Color>(this))
            q->encodeAttr(dest);
        break;
    case I_SPECIAL:
        if (auto* q = std::get_if<Special>(this)) {
            xsin::encodeAttr(dest, q->text);
        }
        break;
    }
}

void xs::MaybeColor::writeAttrIf(std::string& dest, std::string_view key) const
{
    if (!hasSmth())
        return;
    xsin::startAttr(dest, key);
    encodeAttr(dest);
    dest += '"';
}


void xs::MaybeColor::parse(std::string_view x)
{
    x = str::trimSv(x);
    if (x.empty()) {
        *this = Inherit{};
        return;
    }
    if (auto q = Color::parse(x)) {
        *this = *q;
        return;
    }
    *this = Special{ .text = std::string{x} };
}


///// Fill /////////////////////////////////////////////////////////////////////

void xs::Fill::encodeAttr(std::string& dest) const
{
    switch (index()) {
    case I_INHERIT:
    default:
        break;
    case I_COLOR:
        if (auto* q = std::get_if<Color>(this))
            q->encodeAttr(dest);
        break;
    case I_NONE:
        dest += "none"sv;
        break;
    case I_IDLINK:
        if (auto* q = std::get_if<IdLink>(this)) {
            dest += "url(#";
            dest += q->wantedId;
            dest += ')';
        }
        break;
    case I_SPECIAL:
        if (auto* q = std::get_if<Special>(this)) {
            xsin::encodeAttr(dest, q->text);
        }
        break;
    }
}

void xs::Fill::writeAttrIf(std::string& dest, std::string_view key) const
{
    if (!hasSmth())
        return;
    xsin::startAttr(dest, key);
    encodeAttr(dest);
    dest += '"';
}

void xs::Fill::parse(std::string_view x)
{
    x = str::trimSv(x);
    if (x.empty()) {
        *this = Inherit{};
        return;
    }
    if (lat::areCaseEqual(x, "none"sv)) {
        *this = None{};
        return;
    }
    if (auto q = Color::parse(x)) {
        *this = *q;
        return;
    }
    /// @todo [urgent] URL is still Special now
    *this = Special{ .text = std::string{x} };
}


///// MaybeFillRule ////////////////////////////////////////////////////////////


void xs::MaybeFillRule::encodeAttr(std::string& dest) const
{
    switch (index()) {
    case I_INHERIT:
    default:
        break;
    case I_FILLRULE:
        if (auto* q = std::get_if<FillRule>(this)) {
            if (*q == FillRule::EVENODD) {
                dest += "evenodd";
            } else {
                dest += "nonzero";
            }
        }
        break;
    case I_SPECIAL:
        if (auto* q = std::get_if<Special>(this)) {
            xsin::encodeAttr(dest, q->text);
        }
        break;
    }
}

void xs::MaybeFillRule::writeAttrIf(std::string& dest, std::string_view key) const
{
    if (!hasSmth())
        return;
    xsin::startAttr(dest, key);
    encodeAttr(dest);
    dest += '"';
}

void xs::MaybeFillRule::parse(std::string_view x)
{
    x = str::trimSv(x);
    if (x.empty()) {
        *this = Inherit{};
    } else if (lat::areCaseEqual(x, "evenodd"sv)) {
        *this = FillRule::EVENODD;
    } else if (lat::areCaseEqual(x, "nonzero"sv)) {
        *this = FillRule::NONZERO;
    }
}


///// Style ////////////////////////////////////////////////////////////////////


void xs::Style::writeAttrIf(std::string& dest)
{
    if (!hasStyle())
        return;
    dest += R"( style=")";
    encodeAttr(dest);
    dest += '"';
}

constexpr std::string_view K_FILL = "fill";
constexpr std::string_view K_FILL_RULE = "fill-rule";
constexpr std::string_view K_STOP_COLOR = "stop-color";


void xs::Style::traverse(
        Flags<AllowAttr> allowed, const MultiTypeCallback& x)
{
    bool canDraw = allowed.have(AllowAttr::DRAW);
    x.onFill(K_FILL, fill, canDraw);
    x.onFillRule(K_FILL_RULE, fillRule, canDraw);

    bool isStop = allowed.have(AllowAttr::STOP);
    x.onColor(K_STOP_COLOR, stopColor, isStop);
}


bool xs::Style::find(
        std::string_view key, Flags<AllowAttr> allowed,
        const MultiTypeCallback& x)
{
#define CHECK(kkey, method, object, flag) \
    if (key == kkey) {  \
        x.method(kkey, object, allowed.have(flag)); \
        return true; \
    }

    if (key.empty())
        return false;
    switch (key[0]) {
    case 'f':
        CHECK(K_FILL, onFill, fill, AllowAttr::DRAW);
        CHECK(K_FILL_RULE, onFillRule, fillRule, AllowAttr::DRAW);
        return false;
    case 's':
        CHECK(K_STOP_COLOR, onColor, stopColor, AllowAttr::STOP);
        return false;
    default:
        return false;
    }
#undef CHECK
}

bool xs::Style::trySpecificAttr(std::string_view key, std::string_view value)
{
    return findT(key, NO_FLAGS,
        [value](std::string_view, auto obj, bool) {
            obj.attr.parse(value);
        });
}

void xs::Style::clear()
{
    traverseT(NO_FLAGS,
        [](std::string_view, auto& x, bool) {
            x.clear();
        });
    attrs.clear();
}


void xs::Style::add(std::string_view key, std::string_view value)
{
    if ((key = str::trimSv(key)).empty())
        return;
    if ((value = str::trimSv(value)).empty())
        return;
    auto wasFound = findT(key, NO_FLAGS,
        [value](std::string_view, auto& obj, bool) {
            obj.style.parse(value);
        });
    if (!wasFound)
        attrs.emplace_back(std::string(key), std::string(value));
}


void xs::Style::parse(std::string_view x)
{
    std::vector<char> openBrackets;
    char quote = 0;
    using It = decltype(x.begin());
    It itStart = x.begin();
    std::optional<It> itColon = std::nullopt;

    auto process = [&](It objEnd) {
        if (!itColon)
            return;
        std::string_view key {itStart, *itColon};
        std::string_view value {*itColon + 1, objEnd};
        add(key, value);
        itColon.reset();
        itStart = objEnd + 1;
    };
    for (auto it = x.begin(); it != x.end(); ++it) {
        auto c = *it;
        switch (c) {
        case ';':
            if (!quote && openBrackets.empty())
                process(it);
            break;
        case ':':
            if (!itColon && !quote && openBrackets.empty())
                itColon = it;
            break;
        case '(':
            if (!quote)
                openBrackets.push_back(')');
            break;
        case '[':
            if (!quote)
                openBrackets.push_back(']');
            break;
        case '{':
            if (!quote)
                openBrackets.push_back('}');
            break;
        case ')':
        case ']':
        case '}':
            if (!quote && !openBrackets.empty() && openBrackets.back() == c)
                openBrackets.pop_back();
            break;
        case '\'':
        case '"':
            if (quote) {
                if (quote == c)
                    quote = 0;
            } else {
                quote = c;
            }
            break;
        default: ;
        }
    }
    process(x.end());
}

bool xs::Style::hasSmth() noexcept
{
    if (!attrs.empty())
        return true;
    bool r = false;
    traverseT(NO_FLAGS,
        [&r](std::string_view, auto& obj, bool) {
            r |= obj.hasSmth();
        });
    return r;
}


bool xs::Style::hasStyle() noexcept
{
    if (!attrs.empty())
        return true;
    bool r = false;
    traverseT(NO_FLAGS,
        [&r](std::string_view, auto& obj, bool) {
            r |= obj.style.hasSmth();
        });
    return r;
}


void xs::Style::encodeAttr(std::string& dest)
{
    auto oldLength = dest.length();
    auto writeSemicolon = [&]() {
        if (dest.length() != oldLength)
            dest += ';';
        oldLength = dest.length();
    };
    traverseT(NO_FLAGS,
        [&](std::string_view key, auto& obj, bool) {
            if (obj.style.hasSmth()) {
                writeSemicolon();
                dest += key;
                dest += ':';
                obj.style.encodeAttr(dest);
            }
        });
    // Misc
    for (auto& q : attrs) {
        if (!q.key.empty()) {
            writeSemicolon();
            xsin::encodeAttr(dest, q.key);
            dest += ':';
            xsin::encodeAttr(dest, q.value);
        }
    }
}

///// xsin /////////////////////////////////////////////////////////////////////

void xsin::encodeAttr(std::string& dest, std::string_view x)
{
    dest.reserve(dest.length() + x.length());
    for (auto c : x) {
        switch (c) {
        case '"':
            dest += "&quot;";
            break;
        default:
            dest += c;
            break;
        }
    }
}

void xsin::writeAttrIf(std::string& dest, std::string_view key, std::string_view value)
{
    if (!value.empty())
        writeAttr(dest, key, value);
}

void xsin::startAttr(std::string& dest, std::string_view key)
{
    dest += ' ';
    dest += key;
    dest += R"(=")";
}

void xsin::writeAttr(std::string& dest, std::string_view key, std::string_view value)
{
    xsin::startAttr(dest, key);
    xsin::encodeAttr(dest, value);
    dest += '"';
}
