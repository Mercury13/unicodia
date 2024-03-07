#pragma once

//
//  A simple unit between Qt and Unicode
//

namespace uc {

    enum class CopiedChannel { CHAR, SAMPLE };
    void copyCp(char32_t cp, CopiedChannel channel);

}
