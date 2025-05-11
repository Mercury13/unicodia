#pragma once

#include <filesystem>

struct Cmdline {
    struct Qa {
        std::filesystem::path fonts;

        bool isPresent() const noexcept {
            return !fonts.empty();
        }
        operator bool () const noexcept { return isPresent(); }
    } qa;

    static Cmdline parse();
};
