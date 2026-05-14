#pragma once

namespace upd {

#define N_CHECKED_VERSIONS "8"
#define SUBURL_REPO "Mercury13/unicodia/releases"

    constexpr const char* URL_API = "https://api.github.com/repos/" SUBURL_REPO "?per_page=" N_CHECKED_VERSIONS;
    constexpr const char* URL_REPO = "https://github.com/" SUBURL_REPO;

#undef N_CHECKED_VERSIONS
#undef SUBURL_REPO

}
