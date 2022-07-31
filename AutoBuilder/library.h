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
        size_t index = 0;
        SafeVector<Node> children;
    };


    struct EmojiData {
        std::unordered_set<char32_t> vs16;
        Node root;
    };


    EmojiData loadEmoji(const char* fname);

}   // namespace lib
