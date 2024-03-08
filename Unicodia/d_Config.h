#pragma once

// C++
#include <filesystem>

// Qt
#include "QRect"

// Libs
#include "u_Version.h"
#include "u_EnumSize.h"


DEFINE_ENUM (BlockOrder,
    ALPHA,      ///< Alphabetical
    CONTINENT,  ///< By continent, then alphabetically
    DEFAULT = CONTINENT,
    CODE,       ///< By code
    TECH        ///< By technical name
)


namespace progsets {
    enum class DirMode { INSTALLED, PORTABLE };
    extern DirMode dirMode;
}

namespace updatever {
    extern Version version;
    extern std::string equivPlatforms;
    extern bool isDebuggingVersion;
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

    /// All params should be initialized!
    void init(QRect& winRect, BlockOrder& blockOrder);

    void save(const QRect& winRect, bool isMaximized,
              BlockOrder blockOrder);
}
