#pragma once

#include <filesystem>

struct Cmdline {
    struct Qa {
        std::filesystem::path fonts, http;

        bool hasFonts() const noexcept { return !fonts.empty(); }
        bool hasHttp()  const noexcept { return !http .empty(); }

        bool isPresent() const noexcept {
            return hasFonts() || hasHttp();
        }
        operator bool () const noexcept { return isPresent(); }
    } qa;

    static Cmdline parse();
};
