#pragma once

namespace upd {

#define N_CHECKED_VERSIONS "8"
#define REPO "Mercury13/unicodia"

    constexpr const char* URL_API = "https://api.github.com/repos/" REPO "/releases?per_page=" N_CHECKED_VERSIONS;
    constexpr const char* URL_REPO = "https://github.com/" REPO "/releases";

#undef N_CHECKED_VERSIONS
#undef REPO

}
