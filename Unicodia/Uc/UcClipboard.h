#pragma once

#include <string_view>

//
//  A simple unit between Qt and Unicode
//

namespace uc {
    class LibNode;

    enum class CopiedChannel { CHAR, SAMPLE };
    void copyCp(char32_t cp, CopiedChannel channel);
    /// @return [+] string not empty
    bool copyString(std::u32string_view sv);
    /// @return [+] node’s value is not empty
    bool copyNode(const uc::LibNode& node);
    /// @return [+] node present, value not empty
    bool copyNode(const uc::LibNode* node);
}
