#pragma once

//
//  Misc. things related to Library (Подборки)
//

// STL
#include <unordered_set>

// Lib
#include "u_Vector.h"

// Unicodia
#include "UcFlags.h"

// Utils
unsigned fromHex(std::string_view x);


namespace lib {

    struct Node {
        std::u8string name;
        std::u32string value;
        SafeVector<Node> children;
        uc::Lfgs flags {};
        std::string emojiVersion;
        mutable struct Cache {
            int index = 0;
        } cache;
    };


    struct EmojiData {
        /// List of single-char emoji that use VS16
        std::unordered_set<char32_t> vs16;
        /// Root of emoji branch
        Node root;
    };


    EmojiData loadEmoji(const char* fname);
    SafeVector<Node> loadManual(const char* fname);

    struct Result {
        int nNodes = 0;
    };

    Result write(const Node& root, const char* fname);

    class StrangeCjk
    {
    public:
        StrangeCjk();
        void processCp(char32_t cp, std::string_view sStrange);
        Node&& give() { return std::move(root); }
    private:
        Node root;
    };

}   // namespace lib
