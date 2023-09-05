// My header
#include "parsers.h"

// STL
#include <span>

// Libs
#include "u_Strings.h"

// Json
#include "rapidjson/document.h"


github::UpdateReply::UpdateReply(std::span<std::string_view> aPlats)
{
    if (!aPlats.empty())
        myPlatform = aPlats[0];
}


namespace {

    template <class Doc>
    void checkJsonVersionForUpdate(
            const Doc& doc,
            const Version& myVersion,
            std::span<std::string_view> equivPlatforms,
            github::UpdateReply& r)
    {
        if (!doc.IsObject())
            return;
        auto data = doc.FindMember("name");
        if (!data->value.IsString())
            return;
        r.versionText = data->value.GetString();

        // Debug
        if (myVersion == github::VER_BAD_VERSION)
            r.versionText = "alpha<br>bravo";

        /// @todo [urgent] Check for platform existence

        auto hAssets = doc.FindMember("assets");
        if (!hAssets->value.IsArray()) {
            r.code = github::UpdateCode::ABANDONED;
            return;
        }
        for (auto it = hAssets->value.Begin(); it != hAssets->value.End(); ++it) {
            if (!it->IsObject())
                continue;
            auto assetName = it->FindMember("name");
            if (!assetName->value.IsString())
                continue;
            std::string_view fileName = data->value.GetString();
            // Bite off extension
            auto pPoint = fileName.find_last_of('.');
            if (pPoint != std::string_view::npos)
                fileName = fileName.substr(0, pPoint);
            // Split
            auto parts = str::splitSv(fileName, '-');
            for (auto p : parts) {
                for (auto q : equivPlatforms) {
                    if (lat::areCaseEqual(p, q))
                        goto found;
                }
            }
        }
        r.code = github::UpdateCode::ABANDONED;
        return;
    found:
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

    inline auto unpackPlatforms(std::string_view equivPlatforms)
    {
        return str::splitSv(equivPlatforms, '|');
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

    auto plats = unpackPlatforms(equivPlatforms);
    UpdateReply r(plats);

    doc.Parse(body.data(), body.length());

    checkJsonVersionForUpdate(doc, myVersion, plats, r);
    return r;
}


github::UpdateReply github::checkPageForUpdate(
        std::string_view body,
        const Version& myVersion,
        std::string_view equivPlatforms)
{
    if (myVersion == VER_BAD_REPLY)
        body = "Something really bad";
    rapidjson::Document doc;

    auto plats = unpackPlatforms(equivPlatforms);
    UpdateReply r(plats);

    doc.Parse(body.data(), body.length());

    checkJsonVersionForUpdate(doc, myVersion, plats, r);
    return r;
}
