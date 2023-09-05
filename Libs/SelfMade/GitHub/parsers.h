#pragma once

// STL
#include <string>

// Libs
#include "u_Version.h"

namespace github {

    constexpr Version VER_BAD_REPLY { 0, 0, 1 };
    constexpr Version VER_BAD_VERSION { 0, 0, 2 };
    constexpr Version VER_BAD_REQUEST { 0, 0, 3 };

    enum class UpdateCode { BAD_DOCUMENT, FOUND_EARLIER,
                            COINCIDE, FOUND_LATER, BAD_VERSION,
                            ABANDONED,
                            PLATFORM_NOT_FOUND = ABANDONED
                          };
    struct UpdateReply {
        UpdateCode code = UpdateCode::BAD_DOCUMENT;
        Version version;
        std::string versionText;
        std::string myPlatform;

        UpdateReply() = default;
        UpdateReply(std::span<std::string_view> aPlats);
    };

    UpdateReply checkVersionForUpdate(
            std::string_view body,
            const Version& myVersion,
            std::string_view equivPlatforms);
    UpdateReply checkPageForUpdate(
            std::string_view body,
            const Version& myVersion,
            std::string_view equivPlatforms);

}   // namespace github
