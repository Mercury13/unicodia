#pragma once

#include <vector>
#include <string>
#include <unordered_map>

namespace cl {

    struct EmojiEntry {
        std::vector<std::string> names;

        EmojiEntry() = default;
        EmojiEntry(const EmojiEntry&) = delete;
        EmojiEntry& operator = (const EmojiEntry&) = delete;
        void eraseName(std::string_view text);
    };

    struct Base {
        std::unordered_map<char32_t, EmojiEntry> emoji;

        EmojiEntry* findEmoji(char32_t c);
        EmojiEntry& findEmojiOr(char32_t c, EmojiEntry& fallback);
    };

    Base load();

}