#pragma once

// STL
#include <string_view>

// Misc
#include "../../function_ref.hpp"

namespace egyp {

    using Callback = tl::function_ref<void(std::string_view text, std::string_view index)>;

    void extractIndexes(std::string_view s, Callback cb);

}   // namespace egyp
