#include "work.h"

#include "pugixml.hpp"
#include "u_Strings.h"

using namespace std::string_view_literals;

const BadFile::Discrep BadFile::Discrep::EMPTY;

namespace {
    /// @return [+] We have all troubles findable in non-specific
    ///             attributes (x.aa && x.bb if several!)
    bool areAttrsComplete(const BadFile::Discrep& x)
    {
        return x.hasRgbaColor;
    }

    /// We use const char* here (want final 0)
    bool hasRgba(const char* x)
    {
        auto p = strstr(x, "rgba");
        if (!p)
            return false;
        // That’s OK, we work with the 1st occurrence
        p += 4; // size of "rgba"
        while (*p == ' ') {
            ++p;
        }
        return (*p == '(');
    }

    void checkForBad(pugi::xml_node node, BadFile::Discrep& r)
        //  xml_node is handle, so OK by value
    {
        // Check attrs
        if (!areAttrsComplete(r)) {
            for (auto& v : node.attributes()) {
                if (hasRgba(v.value())) {
                    r.hasRgbaColor = true;
                }
            }
        }

        // Check children
        for (auto& v : node.children()) {
            if (v.name() == "clipPath"sv) {
                r.hasClipPath = true;
            } else if (v.name() == "linearGradient"sv
                    || v.name() == "radialGradient"sv) {
                // We find GradientXlink by absence of stops
                auto p = v.child("stop");
                if (!p)
                    r.hasGradientXlink = true;
                // Still need to traverse all stops,
                //  as RGBA is often somewhere in stops.
                checkForBad(v, r);
            } else {
                checkForBad(v, r);
            }
        }
    }

}

std::optional<BadFile> processFile(const std::filesystem::path& fname)
{
    pugi::xml_document doc;
    auto res = doc.load_file(fname.c_str());
    BadFile bad;
    if (res.status != pugi::xml_parse_status::status_ok) {
        return bad;
    }
    bad.isValid = true;

    checkForBad(doc.root(), bad.discrep);
    if (!bad.discrep.hasAny())
        return std::nullopt;  // OK
    bad.name = fname.stem();
    return bad;
}

Result processDir(const std::filesystem::path& dir)
{
    Result r;
    for (auto const& v : std::filesystem::directory_iterator{dir}) {
        if (v.exists()
                && v.is_regular_file()
                && lat::areCaseEqual<std::wstring_view>(
                        v.path().extension().c_str(), L".svg"sv)) {
            auto badInfo = processFile(v.path());
            if (badInfo) {
                r.bad.emplace_back(std::move(*badInfo));
            } else {
                r.good.emplace_back(v.path().stem());
            }
        }
    }
    return r;
}
