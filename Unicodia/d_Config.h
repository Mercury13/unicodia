#pragma once

// C++
#include <filesystem>

// Qt
#include "QRect"


namespace progsets {
    enum class DirMode { INSTALLED, PORTABLE };
    extern DirMode dirMode;
}

namespace fname {
    extern std::filesystem::path config;
    extern std::filesystem::path progsets;
}

namespace path {
    // Some path within app’s filesystem to store bundled data
    extern std::filesystem::path exeBundled;
    // Some path within app’s filesystem that’s somehow accessible to admin
    // In Windows exeAdmined == exeBundled
    extern std::filesystem::path exeAdmined;
    // Some writeable path within system’s/app’s filesystem where configs are placed
    extern std::filesystem::path config;
}

namespace config {
    namespace window {
        extern bool isMaximized;
    }
    namespace lang {
        extern std::string wanted;
        extern int savedStamp;
    }

    void init(QRect& winRect);
    void save(const QRect& winRect, bool isMaximized);
}
