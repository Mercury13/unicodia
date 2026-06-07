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


bool xs::Style::hasSmth() const noexcept
{
    return fill.hasSmth()
    || stopColor.hasSmth()
        || !attrs.empty();
}


void xs::Style::encodeAttr(std::string& dest) const
{
    auto oldLength = dest.length();
    auto writeSemicolon = [&]() {
        if (dest.length() != oldLength)
            dest += ';';
        oldLength = dest.length();
    };
    // Known, semicolons in between
    if (fill) {
        dest += "fill:";
        fill.encodeAttr(dest);
    }
    if (fillRule) {
        writeSemicolon();
        dest += "fill-rule:";
        fillRule.encodeAttr(dest);
    }
    if (stopColor) {
        writeSemicolon();
        dest += "stop-color:";
        stopColor.encodeAttr(dest);
    }
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


void xs::Style::writeAttrIf(std::string& dest) const
{
    if (!hasSmth())
        return;
    dest += R"( style=")";
    encodeAttr(dest);
    dest += '"';
}


void xs::Style::clear()
{
    fill = Inherit{};
    attrs.clear();
}


void xs::Style::add(std::string_view key, std::string_view value)
{
    if ((key = str::trimSv(key)).empty())
        return;
    if ((value = str::trimSv(value)).empty())
        return;
    switch (key[0]) {
    case 'f':
        if (key == "fill"sv) {
            fill.parse(value);
            return;
        }
        if (key == "fill-rule"sv) {
            fillRule.parse(value);
            return;
        }
        break;
    case 's':
        if (key == "stop-color"sv) {
            stopColor.parse(value);
            return;
        }
        break;
    default:;
    }
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
