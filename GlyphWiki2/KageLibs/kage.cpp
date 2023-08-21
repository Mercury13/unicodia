#include "kage.h"

// STL
#include <stdexcept>
#include <span>

// Libs
#include "u_Strings.h"

const kage::NoSource kage::NoSource::INST;
const kage::BadSource kage::BadSource::INST;


///// SourceEngine’s ///////////////////////////////////////////////////////////


std::string kage::NoSource::getKage(std::string_view) const
    { return {}; }


std::string kage::BadSource::getKage(std::string_view) const
    { throw std::logic_error("[BadSource.getKage] Links to other glyphs are disallowed!"); }


///// toSvg ////////////////////////////////////////////////////////////////////

namespace {

    enum {
        I_THING = 0,
        I_LINK_TARGET = 7,
        I_LINK_SX = 1,
        I_LINK_SY = 2,
        I_LINK_X1 = 3,
        I_LINK_Y1 = 4,
        I_LINK_X2 = 5,
        I_LINK_Y2 = 6,
        I_LINK_SX2 = 9,
        I_LINK_SY2 = 10,
    };

    enum {
        STROKE_SEPARATOR = 0,
        STROKE_STRAIGHT = 1,
        STROKE_CURVED = 2,
        STROKE_BENT = 3,
        STROKE_OTSU = 4,
        STROKE_BEZIER = 6,
        STROKE_SWEPT = 7,
        STROKE_CHILD_SPACE = 9,
        STROKE_HOOK = 13,
        STROKE_LINK = 99,
    };

    enum class CapType {
        OPEN = 0,
        CONNECT_HORIZONTAL = 2,
        HANE_LEFT = 4,
        HANE_RIGHT = 5,
        THIN = 7,
        STOP = 8,
        // CAP_? = 9
        // The following five might be removed in favour of automatically
        //   determining the start/end for CAP_CONNECT
        CORNER_TOP_LEFT = 12,
        CORNER_BOTTOM_LEFT = 13,
        CORNER_TOP_RIGHT = 22,
        CORNER_BOTTOM_RIGHT = 23,
        CONNECT_VERTICAL = 32,
    };

}   // anon namespace


std::string kage::Glyph::toSvg() const
{
    std::string r;

    /// @todo [urgent] to Svg

    return r;
}


///// Misc /////////////////////////////////////////////////////////////////////


SafeVector<std::string_view> kage::splitIntoLinesSv(std::string_view source)
{
    return str::splitByAnySv(source, SEPARATORS);
}

namespace {

    int parseInt(std::string_view x, const char* errmsg)
    {
        int r;
        auto q = std::from_chars(
                     std::to_address(x.begin()),
                     std::to_address(x.end()),
                     r);
        if (q.ec != std::errc{})
            throw std::logic_error(errmsg);
        return r;
    }

    int parseInt(std::span<const std::string_view> data, unsigned index, const char* errmsg)
    {
        if (index > data.size()) {
            char buf[60];
            snprintf(buf, std::size(buf), "[parseInt] Array index out of bounds %u/%u",
                                index, unsigned(data.size()));
            throw std::logic_error(buf);
        }
        return parseInt(data[index], errmsg);
    }

    auto appendStrokesOfBuhin(
                kage::Glyph& r,
                std::string_view source,
                int x1, int y1, int x2, int y2,
                int sx, int sy, int sx2, int sy2)
    {
        throw std::logic_error("[appendStrokesOfBuhin] Not implemented");
    }

}   // anon namespace


kage::Glyph kage::toGlyph(std::string_view source, const SourceEngine& engine)
{
    Glyph r;
    auto lines = splitIntoLinesSv(source);
    for (auto line : lines) {
        auto columns = str::splitSv(line, ':', false);
        auto col0 = parseInt(columns, I_THING, "[toGlyph] Cannot parse column 0");
        if (col0 == STROKE_LINK) {
            auto target = columns.safeGetV(I_LINK_TARGET, "");
            if (target.empty())
                throw std::logic_error("[toGlyph] No link found");
            auto source = engine.getKage(target);
            auto x1  = parseInt(columns, I_LINK_X1,  "[toGlyph] Cannot parse link x1 (=column 3)");
            auto y1  = parseInt(columns, I_LINK_Y1,  "[toGlyph] Cannot parse link y1 (=column 4)");
            auto x2  = parseInt(columns, I_LINK_X2,  "[toGlyph] Cannot parse link x2 (=column 5)");
            auto y2  = parseInt(columns, I_LINK_Y2,  "[toGlyph] Cannot parse link y2 (=column 6)");
            auto sx  = parseInt(columns, I_LINK_SX,  "[toGlyph] Cannot parse link sx (=column 1)");
            auto sy  = parseInt(columns, I_LINK_SY,  "[toGlyph] Cannot parse link sy (=column 2)");
            auto sx2 = parseInt(columns, I_LINK_SX2, "[toGlyph] Cannot parse link sx2 (=column 9)");
            auto sy2 = parseInt(columns, I_LINK_SY2, "[toGlyph] Cannot parse link sy2 (=column 10)");
            appendStrokesOfBuhin(r, source, x1, y1, x2, y2, sx, sy, sx2, sy2);
        } else {
            auto& newLine = r.lines.emplace_back();
            auto sz = std::min<size_t>(columns.size(), LINE_SIZE);
            for (size_t j = 0; j < sz; ++j)
                newLine.d[j] = MaybeInt<int>::fromStr(columns[j]);
        }
    }
    return r;
}

std::string kage::toSvg(std::string_view source, const SourceEngine& engine)
{
    return toGlyph(source, engine).toSvg();
}
