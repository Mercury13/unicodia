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

    enum class SmallRes { BAD_JSON, GO_NEXT, STOP_EXACT, STOP_OVERSHOOT };

    /// Checks whether documeht has one of equivPlatforms
    /// @param [in] goodCode    what to return if GOOD (result always STOP_EXACT)
    /// @param [in] badResult   what to return if BAD (code always ABANDONED)
    SmallRes checkJsonPlatforms(
            const rapidjson::Value& doc,
            std::span<std::string_view> equivPlatforms,
            github::UpdateReply& r,
            github::UpdateCode goodCode,
            SmallRes badResult)
    {
        auto hAssets = doc.FindMember("assets");
        if (!hAssets->value.IsArray()) {
            r.code = github::UpdateCode::ABANDONED;
            return SmallRes::GO_NEXT;
        }
        for (auto it = hAssets->value.Begin(); it != hAssets->value.End(); ++it) {
            if (!it->IsObject())
                continue;
            auto assetName = it->FindMember("name");
            if (!assetName->value.IsString())
                continue;
            std::string_view fileName = assetName->value.GetString();
            // Bite off extension
            auto pPoint = fileName.find_last_of('.');
            if (pPoint != std::string_view::npos)
                fileName = fileName.substr(0, pPoint);
            // Split
            auto parts = str::splitSv(fileName, '-');
            for (auto p : parts) {
                for (auto q : equivPlatforms) {
                    if (lat::areCaseEqual(p, q)) {
                        r.code = goodCode;
                        return SmallRes::STOP_EXACT;
                    }
                }
            }
        }
        r.code = github::UpdateCode::ABANDONED;
        return badResult;
    }

    SmallRes checkJsonVersionForUpdate(
            const rapidjson::Value& doc,
            const Version& myVersion,
            std::span<std::string_view> equivPlatforms,
            github::UpdateReply& r)
    {
        if (!doc.IsObject())
            return SmallRes::BAD_JSON;
        auto data = doc.FindMember("name");
        if (!data->value.IsString())
            return SmallRes::BAD_JSON;
        r.versionText = data->value.GetString();

        // Debug
        if (myVersion == github::VER_BAD_VERSION)
            r.versionText = "alpha<br>bravo";


        r.version = Version::parsePermissive(r.versionText);
        if (!r.version) {
            r.code = github::UpdateCode::BAD_VERSION;
            return SmallRes::BAD_JSON;
        }        
        auto q = (r.version <=> myVersion);
        if (q == std::strong_ordering::less) {
            r.code = github::UpdateCode::FOUND_EARLIER;
            return SmallRes::STOP_OVERSHOOT;
        } else if (q == std::strong_ordering::greater) {
            return checkJsonPlatforms(
                        doc, equivPlatforms, r,
                        github::UpdateCode::FOUND_LATER, SmallRes::GO_NEXT);
        } else {
            return checkJsonPlatforms(
                        doc, equivPlatforms, r,
                        github::UpdateCode::COINCIDE, SmallRes::STOP_EXACT);
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
    std::optional<UpdateReply> oldR;

    doc.Parse(body.data(), body.length());
    if (!doc.IsArray())
        return r;

    for (auto it = doc.Begin(); it != doc.End(); ++it) {
        auto q = checkJsonVersionForUpdate(*it, myVersion, plats, r);
        switch (q) {
        case SmallRes::STOP_OVERSHOOT:
            // Stop means we found this/earlier version
            if (oldR)
                r = std::move(*oldR);
            [[fallthrough]];
        case SmallRes::STOP_EXACT:
            if (r.code == UpdateCode::ABANDONED)
                r.code = UpdateCode::COINCIDE;
            return r;
        case SmallRes::BAD_JSON:
            break;
        case SmallRes::GO_NEXT:
            oldR = r;
            break;
        }
    }
    return r;
}
