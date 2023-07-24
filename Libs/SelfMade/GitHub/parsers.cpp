// My header
#include "parsers.h"

// Json
#include "rapidjson/document.h"


github::UpdateReply github::checkForUpdate(std::string_view body, const Version& myVersion)
{
    if (myVersion == VER_BAD_REPLY)
        body = "Something really bad";
    rapidjson::Document doc;

    UpdateReply r;
    doc.Parse(body.data(), body.length());
    if (!doc.IsObject())
        return r;
    auto data = doc.FindMember("name");
    if (!data->value.IsString())
        return r;
    r.versionText = data->value.GetString();
    // Debug
    if (myVersion == VER_BAD_VERSION)
        r.versionText = "alpha<br>bravo";

    r.version = Version::parsePermissive(r.versionText);
    if (!r.version) {
        r.code = UpdateCode::BAD_VERSION;
        return r;
    }
    auto q = (r.version <=> myVersion);
    if (q == std::strong_ordering::less) {
        r.code = UpdateCode::FOUND_EARLIER;
    } else if (q == std::strong_ordering::greater) {
        r.code = UpdateCode::FOUND_LATER;
    } else {
        r.code = UpdateCode::COINCIDE;
    }
    return r;
}
