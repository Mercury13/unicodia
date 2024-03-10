#pragma once

// C++
#include <filesystem>
#include <unordered_set>
#include <vector>
#include <span>

// Qt
#include "QRect"

// Libs
#include "u_Version.h"
#include "u_EnumSize.h"
#include "u_TinyOpt.h"


DEFINE_ENUM (BlockOrder,
    ALPHA,      ///< Alphabetical
    CONTINENT,  ///< By continent, then alphabetically
    CODE,       ///< By code
    TECH,       ///< By technical name
    DEFAULT = CONTINENT,
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

    class Favs
    {
    public:
        /// @return  [+] was added
        TinySizet add(char32_t code);
        /// @return  [+] was erased
        TinySizet erase(char32_t code);
        size_t size() const { return fCodes.size(); }
        /// @return  All codes, ordered by CP
        std::span<const char32_t> codes() const { return fCodes; }
        void clear();
        bool contains(char32_t code) const { return ndx.contains(code); }
        [[nodiscard]] bool empty() const { return fCodes.empty(); }
        bool isEmpty() const { return fCodes.empty(); }
    private:
        std::vector<char32_t> fCodes;
        std::unordered_set<char32_t> ndx;
    };

    namespace window {
        extern bool isMaximized;
    }
    namespace lang {
        extern std::string wanted;
        extern int savedStamp;
    }

    extern Favs favs;

    /// All params should be initialized!
    void init(QRect& winRect, BlockOrder& blockOrder);

    void save(const QRect& winRect, bool isMaximized,
              BlockOrder blockOrder);
}
