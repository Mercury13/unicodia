// My header
#include "parsers.h"

// STL
#include <span>

// Libs
#include "u_Strings.h"

// Json
#include "rapidjson/document.h"


namespace {

    void checkJsonVersionForUpdate(
            const rapidjson::Document& doc,
            const Version& myVersion,
            std::span<std::string_view> equivPlatforms,
            github::UpdateReply& r)
    {
        if (!doc.IsObject())
            return;
        auto data = doc.FindMember("name");
        if (!data->value.IsString())
            return;

        // Debug
        if (myVersion == github::VER_BAD_VERSION)
            r.versionText = "alpha<br>bravo";

        r.version = Version::parsePermissive(r.versionText);
        if (!r.version) {
            r.code = github::UpdateCode::BAD_VERSION;
            return;
        }
        auto q = (r.version <=> myVersion);
        if (q == std::strong_ordering::less) {
            r.code = github::UpdateCode::FOUND_EARLIER;
        } else if (q == std::strong_ordering::greater) {
            r.code = github::UpdateCode::FOUND_LATER;
        } else {
            r.code = github::UpdateCode::COINCIDE;
        }
    }

}   // anon namespace


github::UpdateReply github::checkVersionForUpdate(
        std::string_view body,
        const Version& myVersion,
        std::string_view equivPlatforms)
{
    if (myVersion == VER_BAD_REPLY)
        body = "Something really bad";
    rapidjson::Document doc;

    UpdateReply r;
    auto plats = str::splitSv(equivPlatforms, '|');
    r.myPlatform = plats.safeGetV(0, "");

    doc.Parse(body.data(), body.length());

    checkJsonVersionForUpdate(doc, myVersion, plats, r);
    return r;
}
