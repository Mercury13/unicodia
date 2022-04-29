// My header
#include "UcDating.h"

#include "u_Strings.h"


///// DatingLoc ////////////////////////////////////////////////////////////////

constexpr auto BUCK = u8'$';

std::u8string_view uc::DatingLoc::formatCentury(int x) const
{
    x = std::abs(x);
    if (x == 0 || x > MAX_CENTURY)
        return u8"[CENTURY??]";
    return centuryNames[x];
}


std::u8string_view uc::DatingLoc::format1stCentury(int x, char* start, char* end) const
{
    switch (crangeMode) {
    case CrangeMode::SPECIAL_SPECIAL:
        return formatCentury(x);
    case CrangeMode::ARABIC_SPECIAL:
        return str::toCharsU8(start, end, std::abs(x));
    default:
        return u8"[format1stCentury!!! strange mode]";
    }
}


std::u8string uc::DatingLoc::formatCenturyText(int x) const
{
    auto cry = formatCentury(x);
    std::u8string r;
    if (x > 0) {
        r = century;
    } else {
        r = centuryBc;
    }
    str::replace(r, BUCK, cry);
    return r;
}


void uc::DatingLoc::appendCenturyText(std::u8string& r, int x) const
    { r.append(formatCenturyText(x)); }

namespace {
    void appendCustomNote(std::u8string& r, std::u8string_view note)
    {
        r.append(u8" (");
        r.append(note);
        r.append(u8")");
    }
}

void uc::DatingLoc::appendNote(
        std::u8string& r, StdNote sn, std::u8string_view note) const
{
    switch (sn) {
    case StdNote::NONE:
        break;
    case StdNote::CUSTOM:
        appendCustomNote(r, note);
        break;
    case StdNote::FIRST_KNOWN:
        appendCustomNote(r, firstInscription);
        break;
    case StdNote::MODERN_FORM:
        appendCustomNote(r, modernForm);
        break;
    case StdNote::MAYBE_EARLIER:
        appendCustomNote(r, maybeEarlier);
        break;
    }
}


///// Dating ///////////////////////////////////////////////////////////////////


bool uc::Dating::needsCustomNote() const
{
    switch (fMode) {
    case Mode::SPECIAL:     // 2ND CENTURY (MAIN HYPOTHESIS), 7TH CENTURY (FOR SURE)
    case Mode::CRANGE_INS:  // 12th century (SOME COMMENT), 13th century (first known inscription)
        return true;
    case Mode::NONE:        // technical
        return false;
    case Mode::YEAR:
    case Mode::YBEFORE:
    case Mode::YRANGE:
    case Mode::YAPPROX:
    case Mode::DECADE:
    case Mode::CENTURY:
    case Mode::CRANGE:
    case Mode::UNKNOWN:
        return (fStdNote == StdNote::CUSTOM);
    }
    throw std::logic_error("[needsCustomNote] Strange mode");
}


std::u8string uc::Dating::wikiText(const DatingLoc& loc, std::u8string_view customNote) const
{
    char buf[60];
    char buf2[20];
    std::u8string r, t;

    static constexpr std::u8string_view NDASH = u8"–";

    switch (fMode) {
    // SPECIAL
    case Mode::NONE:
        return u8"[NONE!!!!!!]";
    case Mode::SPECIAL:
        return std::u8string { customNote };
    case Mode::UNKNOWN:
        r = loc.unknown;
        break;
    // YEAR
    case Mode::YEAR: {
            auto u8 = str::toCharsU8(buf, std::abs(fValue1));
            if (fValue1 >= 0) {     // Year CE
                r.append(u8);
            } else {        // Year BC
                r = loc.yBc;
                str::replace(r, u8'$', u8);
            }
        } break;
    case Mode::YRANGE: {
            auto y1 = str::toCharsU8(buf,  std::abs(fValue1));
            auto y2 = str::toCharsU8(buf2, std::abs(fValue2));
            if (fValue1 < 0) {
                if (fValue2 < 0) {  // BC to BC
                    t.append(y1);
                    t.append(NDASH);
                    t.append(y2);
                    r = loc.yBc;
                    str::replace(r, BUCK, t);
                } else {    // BC to CE
                    return u8"[Range BC…CE!!!!!!!!!!!!]";
                }
            } else {    // CE to CE
                r.append(y1);
                r.append(NDASH);
                r.append(y2);
            }
        } break;
    case Mode::YAPPROX: {
            auto u8 = str::toCharsU8(buf, std::abs(fValue1));
            if (fValue1 >= 0) {     // ≈1234 CE
                r = loc.yApprox;
            } else {        // ≈1234 BC
                r = loc.yApproxBc;
            }
            str::replace(r, BUCK, u8);
        } break;
    case Mode::YBEFORE: {
            auto u8 = str::toCharsU8(buf, fValue1);     // never BC
            r = loc.yBefore;
            str::replace(r, BUCK, u8);
        } break;
    // DECADE
    case Mode::DECADE: {
            auto u8 = str::toCharsU8(buf, fValue1);     // never BC
            r = loc.decade;
            str::replace(r, BUCK, u8);
        } break;
    // CENTURY
    case Mode::CENTURY:
        r = loc.formatCenturyText(fValue1);
        break;
    case Mode::CRANGE: {
            auto c1 = loc.format1stCentury(fValue1, buf2);
            auto c2 = loc.formatCentury(fValue2);
            t.append(c1);
            t.append(NDASH);
            t.append(c2);
            if (fValue1 < 0) {
                if (fValue2 < 0) {  // BC to BC
                    r = loc.centuryBc;
                } else {    // BC to CE
                    return u8"[Range BC…CE!!!!!!!!!!!!]";
                }
            } else {    // CE to CE
                r = loc.century;
            }
            str::replace(r, BUCK, t);
        } break;
    case Mode::CRANGE_INS:
        r = loc.formatCenturyText(fValue1);
        loc.appendNote(r, fStdNote, customNote);
        r.append(u8", ");
        loc.appendCenturyText(r, fValue2);
        loc.appendNote(r, StdNote::FIRST_KNOWN, {});
        return r;
    }
    loc.appendNote(r, fStdNote, customNote);
    return r;
}
