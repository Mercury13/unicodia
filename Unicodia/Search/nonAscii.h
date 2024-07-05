#pragma once

///
/// Support of non-ASCII characters in search
///

#include "engine.h"

namespace srh {

    class NonAsciiComparator final : public DefaultComparator
    {
    public:
        void prepareHaystack(
                std::u8string_view haystack, std::u8string& result) const override;
        static const NonAsciiComparator INST;
    };

}
