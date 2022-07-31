#pragma once

//
//  Misc. things related to Library (Подборки)
//

// STL
#include <unordered_set>

// Lib
#include "u_Vector.h"

// Utils
unsigned fromHex(std::string_view x);


namespace lib {

    struct Node {
        std::u8string name;
        std::u32string value;
        SafeVector<Node> children;
        struct Cache {
            size_t index = 0;
        } cache;
    };


    struct EmojiData {
        /// List of single-char emoji that use VS16
        std::unordered_set<char32_t> vs16;
        /// Root of emoji branch
        Node root;
    };


    EmojiData loadEmoji(const char* fname);

}   // namespace lib
