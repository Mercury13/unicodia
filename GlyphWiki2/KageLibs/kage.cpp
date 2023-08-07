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


std::string kage::toSvg(std::string_view source, const SourceEngine&)
{
    std::string r;

    static constexpr std::string_view SEPARATORS = "\r" "\n" "$";
    auto lines = str::splitByAnySv(source, SEPARATORS);

    return r;
}
