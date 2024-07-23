#pragma once

//
//  Misc. things related to Library (Подборки)
//

// STL
#include <unordered_set>
#include <unordered_map>
#include <memory>

// Lib
#include "u_Vector.h"

// Unicodia
#include "UcFlags.h"

// Project-local
#include "forget.h"

namespace lib {

    struct Node {
        std::u8string name;
        std::u32string value;
        SafeVector<std::unique_ptr<Node>> children {};
        uc::Lfgs flags {};
        std::string emojiVersion;
        mutable struct Cache {
            int index = 0;
        } cache;
        unsigned maxValueLength() const;
        [[nodiscard]] Node& newChild();
        Node* findChild(std::u8string_view name);
        Node& rqChild(std::u8string_view name);
    };

    struct Hash32 : public std::hash<std::u32string_view> {
        using is_transparent = void;
    };

    struct EmojiInfo {
        std::string emojiVersion;
    };

    using MAll = std::unordered_map<std::u32string, EmojiInfo, Hash32>;

    struct EmojiData {
        /// Total # of emoji
        size_t count = 0;
        std::unordered_map<char32_t, const Node*> allSingleChar;
        /// List of single-char emoji that use VS16
        std::unordered_set<char32_t> vs16;
        /// List of single-char emoji that are
        std::unordered_set<char32_t> misrenders;
        /// All emoji together with Unicode versions
        MAll all;
        /// Root of emoji branch
        Node root;
    };

    EmojiData loadEmoji(const char* fname);

    struct Manual {
        Node root;
        forget::Map forgetMap;
    };

    Manual loadManual(const char* fname, const MAll& mAll);

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
