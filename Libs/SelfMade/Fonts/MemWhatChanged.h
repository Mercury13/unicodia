#pragma once

#include "u_EnumSize.h"

#include <vector>

class MemFont;

namespace mf {

    DEFINE_ENUM_TYPE_IN_NS(mf, ChangeAction, unsigned char,
        ADD, DEL, CHG, BAD);

    struct ChangeLine {
        char32_t cp;
        ChangeAction action;
    };

    std::vector<ChangeLine> whatChanged(MemFont& ol, MemFont& nw);

}   // namespace mf
