#include "Xstyle.h"

#include <charconv>

#include "u_Strings.h"

using namespace std::string_view_literals;


///// IdLink ///////////////////////////////////////////////////////////////////


static_assert('0' == 48, "Non-ASCII machine");
static_assert('A' == 65, "Non-ASCII machine");
static_assert('a' == 97, "Non-ASCII machine");

namespace {

#define TWO_OF(x) (x), (x)
#define FOUR_OF(x) TWO_OF(x), TWO_OF(x)
#define EIGHT_OF(x) FOUR_OF(x), FOUR_OF(x)
#define TEN_OF(x) EIGHT_OF(x), TWO_OF(x)
#define ROW_OF(x) EIGHT_OF(x), EIGHT_OF(x)
#define ALPH_OF(x) ROW_OF(x), TEN_OF(x)  // 16+10
#define ba xs::CharType::BAN
#define ST xs::CharType::START
#define NX xs::CharType::NEXT

    constinit const xs::CharType charTypes[] {
        ROW_OF(ba), ROW_OF(ba), // 32 control
        ba,ba,ba,ba,  ba,ba,ba,ba,  ba,ba,ba,ba, ba,NX,NX,ba,  // only 2D -, 2E .
        TEN_OF(NX),                       NX,ba, ba,ba,ba,ba,  // digs and 3A :
        ba, ALPH_OF(ST),                     ba, ba,ba,ba,ST,  // letters and 5F _
        ba, ALPH_OF(ST),                     ba, ba,ba,ba,ba,  // letters only
        ROW_OF(ST), ROW_OF(ST), ROW_OF(ST), ROW_OF(ST),  // last 128
        ROW_OF(ST), ROW_OF(ST), ROW_OF(ST), ROW_OF(ST),
    };
    static_assert(std::size(charTypes) == 256);

#undef ba
#undef ST
#undef NX
#undef TWO_OF
#undef FOUR_OF
#undef EIGHT_OF
#undef TEN_OF
#undef ROW_OF
#undef ALPH_OF

}   // anon namespace

inline xs::CharType xs::IdLink::charType(unsigned char x) noexcept
{
    return charTypes[x];
}


bool xs::IdLink::isId(std::string_view x) noexcept
{
    if (x.empty())
        return false;
    if (charType(x[0]) != CharType::START)
        return false;
    x = x.substr(1);
    for (auto c : x)
        if (charType(c) == CharType::BAN)
            return false;
    return true;
}


std::optional<xs::IdLink> xs::IdLink::parse(std::string_view x)
{
    if (!x.starts_with("url"sv) || !x.ends_with(')'))
        return std::nullopt;
    x = str::trimSv(x.substr(3, x.length() - 4));
    if (!x.starts_with('('))
        return std::nullopt;
    x = str::trimLeftSv(x.substr(1));
    if (!x.starts_with('#'))
        return std::nullopt;
    x = str::trimLeftSv(x.substr(1));
    if (!isId(x))
        return std::nullopt;
    return xs::IdLink(x);
}


void xs::IdLink::encodeAttr(std::string& dest) const
{
    dest += "url(#"sv;
    xsin::encodeAttr(dest, refId);
    dest += ')';
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
            q->encodeAttr(dest);
        }
        break;
    case I_SPECIAL:
        if (auto* q = std::get_if<Special>(this)) {
            xsin::encodeAttr(dest, q->text);
        }
        break;
    }
}

void xs::Fill::parse(std::string_view x)
{
    x = str::trimSv(x);
    if (x.empty() || lat::areCaseEqual(x, xsin::V_INHERIT)) {
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
    if (auto q = IdLink::parse(x)) {
        *this = std::move(*q);
        return;
    }
    *this = Special(x);
}


///// FillRuleWrap /////////////////////////////////////////////////////////////

constexpr std::string_view V_EVENODD = "evenodd";
constexpr std::string_view V_NONZERO = "nonzero";

void xs::FillRuleWrap::encodeAttr(std::string& dest) const
{
    if (v == FillRule::EVENODD) {
        dest += V_EVENODD;
    } else {
        dest += V_NONZERO;
    }
}

std::optional<xs::FillRuleWrap> xs::FillRuleWrap::parse(std::string_view x) noexcept
{
    if (lat::areCaseEqual(x, V_EVENODD)) {
        return FillRule::EVENODD;
    } else if (lat::areCaseEqual(x, V_NONZERO)) {
        return FillRule::NONZERO;
    }
    return std::nullopt;
}


///// MaybeOpacity /////////////////////////////////////////////////////////////


std::optional<xs::Opacity> xs::Opacity::parse(std::string_view x) noexcept
{
    auto s = x;
    auto unit = OpacityUnit::UNIT;
    if (s.ends_with('%')) {
        unit = OpacityUnit::PERCENT;
        s = str::trimRightSv(s.substr(0, s.length() - 1));
    }
    double val;
    auto r = std::from_chars(x.begin(), x.end(), val);
    if (r.ec == std::errc{}) {
        return Opacity(val, unit);
    }
    return std::nullopt;
}

void xs::Opacity::encodeAttr(std::string& dest) const
{
    if (permille <= 0) {
        dest += '0';
        return;
    }
    if (permille >= OPACITY_UNIT) {
        dest += '1';
        return;
    }
    // Otherwise fraction
    dest += '.';
    auto q = permille;
    // 500 → 50 → 5;   520 → 52
    while (q % 10 == 0)
        q /= 10;
    char buf[10];
    auto convres = std::to_chars(std::begin(buf), std::end(buf), q);
    dest.append(std::begin(buf), convres.ptr);
}

void xs::MaybeTotalOpacity::encodeAttr(std::string& dest) const
{
    std::visit(
        [&dest](const auto& v) {
            v.encodeAttr(dest);
        },
        *this);
}

int xs::MaybeTotalOpacity::parse(std::string_view x)
{
    if (auto q = Opacity::parse(x)) {
        *this = *q;
        return I_OPACITY;
    }
    *this = Special(x);
    return I_SPECIAL;
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

constexpr std::string_view K_CLIP_PATH = "clip-path";
constexpr std::string_view K_FILL = "fill";
constexpr std::string_view K_FILL_OPACITY = "fill-opacity";
constexpr std::string_view K_FILL_RULE = "fill-rule";
constexpr std::string_view K_STOP_COLOR = "stop-color";
constexpr std::string_view K_STOP_OPACITY = "stop-opacity";


void xs::Style::traverse(
        Flags<AllowAttr> allowed, const MultiTypeCallback& x)
{
    bool canFill = allowed.have(AllowAttr::FILL);
    bool canStroke = allowed.have(AllowAttr::STROKE);
    x.onFill(K_FILL, fill, canFill);
    x.onPartOpacity(K_FILL_OPACITY, fillOpacity, canFill);
    x.onFillRule(K_FILL_RULE, fillRule, canFill);

    bool canDraw = canFill | canStroke;
    x.onLink(K_CLIP_PATH, clipPath, canDraw);

    bool isStop = allowed.have(AllowAttr::STOP);
    x.onColor(K_STOP_COLOR, stopColor, isStop);
    x.onPartOpacity(K_STOP_OPACITY, stopOpacity, isStop);
}


bool xs::Style::find(
        std::string_view key, Flags<AllowAttr> allowed,
        const MultiTypeCallback& x)
{
#define CHECK(kkey, method, object, flag) \
    if (key == kkey) {  \
        x.method(kkey, object, allowed.haveAny(flag)); \
        return true; \
    }

    if (key.empty())
        return false;
    switch (key[0]) {
    case 'c':
        CHECK(K_CLIP_PATH, onLink, clipPath, AllowAttr::ANYDRAW)
        return false;
    case 'f':
        CHECK(K_FILL, onFill, fill, AllowAttr::FILL);
        CHECK(K_FILL_OPACITY, onPartOpacity, fillOpacity, AllowAttr::FILL);
        CHECK(K_FILL_RULE, onFillRule, fillRule, AllowAttr::FILL);
        return false;
    case 's':
        CHECK(K_STOP_COLOR, onColor, stopColor, AllowAttr::STOP);
        CHECK(K_STOP_OPACITY, onPartOpacity, stopOpacity, AllowAttr::STOP);
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
