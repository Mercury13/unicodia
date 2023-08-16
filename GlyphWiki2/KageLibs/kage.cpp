#include "kage.h"

// C++
#include <stdexcept>

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

    enum class StrokeType {
        SEPARATOR = 0,
        STRAIGHT = 1,
        CURVED = 2,
        BENT = 3,
        OTSU = 4,
        BEZIER = 6,
        SWEPT = 7,
        CHILD_SPACE = 9,
        HOOK = 13,
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


}


SafeVector<std::string_view> kage::splitIntoLinesSv(std::string_view source)
{
    return str::splitByAnySv(source, SEPARATORS);
}


kage::Glyph kage::toGlyph(std::string_view source, const SourceEngine& engine)
{
    Glyph r;
    auto lines = splitIntoLinesSv(source);
    for (auto line : lines) {
        /// @todo [urgent] what to do with lines?
    }
    return r;
}

std::string kage::toSvg(std::string_view source, const SourceEngine& engine)
{
    auto glyph = toGlyph(source, engine);
    std::string r;

    return r;
}
