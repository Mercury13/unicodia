#include "SkinToneQa.h"

// C++
#include <filesystem>

// STL
#include <string_view>
#include <stdexcept>
#include <array>

// Qt
#include <QPixmap>
#include <QPainter>

// Libs
#include "u_Strings.h"
#include "u_TypedFlags.h"

// Char paint
#include "routines.h"

// Unicode
#include "UcCp.h"

namespace {

    bool containsAny(const std::u32string_view haystack, std::u32string_view needle)
        { return (haystack.find_first_of(needle) != std::u32string_view::npos); }

    struct CountOf {
        unsigned count = 0;
        int lastIndex = -1;
    };

    CountOf countOf(const std::u32string_view haystack, std::u32string_view needle)
    {
        auto index = 0;
        CountOf r;
        while (true) {
            auto index2 = haystack.find_first_of(needle, index);
            if (index2 == std::u32string_view::npos)
                break;
            auto lastIndex = needle.find(haystack[index2]);
            if (lastIndex == std::u32string_view::npos)
                throw std::logic_error("First found, then didn't");
            r.lastIndex = lastIndex;
            index = index2 + 1;
            ++r.count;
        }
        return r;
    }

    enum QaMode {
        NONE = 0,
        WHITE = 1,
        EBONY = 2,      // Flags!!
        ALL = WHITE | EBONY
    };

    struct QaInfo {
        QaMode mode;
        std::u32string cleanName;

        explicit operator bool() const { return (mode != QaMode::NONE); }
    };

    std::u32string doCleanName(std::u32string_view x)
    {
        std::u32string r{x};
        std::erase(r, cp::VS16);
        std::erase(r, cp::SKIN2);
        std::erase(r, cp::SKIN5);
        return r;
    }

    QaInfo qaInfo(std::u32string_view text)
    {
        // Skin 2 (light) has a few nice features related to hair colour
        // Skin 5 (ebony) is nice for checking for forgotten repaintings
        static constexpr const char32_t MY_TONES_DATA[] { cp::SKIN2, cp::SKIN5 };
            // All emoji facing right are auto-generated
            // Skin 2…4 are unused
        static constexpr const char32_t BAD_CHARS_DATA[] { cp::EMOJI_RIGHT_ARROW, cp::SKIN1, cp::SKIN3, cp::SKIN4 };
        static constexpr std::u32string_view MY_TONES { std::begin(MY_TONES_DATA), std::end(MY_TONES_DATA) };
        static constexpr std::u32string_view BAD_CHARS { std::begin(BAD_CHARS_DATA), std::end(BAD_CHARS_DATA) };
        if (text.length() >= 2  // Not just skintone
                && !containsAny(text, BAD_CHARS)) {
            auto cof = countOf(text, MY_TONES);  // Contains EXACTLY ONE tone 1 (white) or 5 (ebony) → the most visible bugs
            if (cof.count == 1) {
                if (cof.lastIndex == 0) {
                    // White: OK
                    return { .mode = QaMode::WHITE, .cleanName = doCleanName(text) };
                } else {
                    // Ebony: replace with white
                    return { .mode = QaMode::EBONY, .cleanName = doCleanName(text) };
                }
            }
        }
        return { .mode = QaMode::NONE, .cleanName {} };
    }

    static constexpr unsigned SIDE = 192;

    struct QaThing {
        QPixmap pix { SIDE * 2, SIDE };
        Flags<QaMode> flags;
        QaThing();
    };

    QaThing::QaThing() {
        pix.fill(Qt::white);
    }

}


SkinToneQa doSkinToneQa()
{
    static constexpr const char* DIR = "SkinToneQa";
    std::filesystem::path paDir = DIR;
    // Try create directory;
    std::error_code ec;
    std::filesystem::create_directories(paDir, ec);
    if (ec != std::error_code{})
        return { .ec = SkinToneError::CANNOT_CREATE_DIR, .nFiles = 0 };

    // Erase BMP files
    std::filesystem::directory_iterator it(paDir);
    for (const auto& p : it) {
        std::filesystem::remove(p, ec);
        if (ec != std::error_code{})
            return { .ec = SkinToneError::CANNOT_REMOVE_FILE, .nFiles = 0 };
    }

    QRect rcWhite { 0,    0,  SIDE, SIDE };
    QRect rcEbony { SIDE, 0,  SIDE, SIDE };

    std::map<std::u32string, QaThing> things;

    for (auto& node : uc::allLibNodes()) {
        if (node.flags.have(uc::Lfg::GRAPHIC_EMOJI)) {
            if (auto qi = qaInfo(node.value)) {
                auto& thing = things[qi.cleanName];
                // Check whether the mode was used
                if (thing.flags.have(qi.mode)) {
                    return { .ec = SkinToneError::REPEATING_EMOJI, .nFiles = 0 };
                }
                thing.flags |= qi.mode;
                const QRect& rcTest = (qi.mode == QaMode::EBONY) ? rcEbony : rcWhite;
                QPainter painter(&thing.pix);
                drawEmojiDirect(&painter, rcTest, node.value, Qt::black);
            }
        }
    }

    // Write files
    char buf[100];  // Should be enough;
    for (auto& [key, val] : things) {
        if (val.flags != QaMode::ALL)
            return { .ec = SkinToneError::INCOMPLETE_EMOJI, .nFiles = 0 };
        uc::sprintPlus(buf, key);
        auto fname = str::cat(buf, ".bmp");
        auto paFile = paDir / fname;
        if (!val.pix.save(QString::fromStdString(paFile.string()))) {
            return { .ec = SkinToneError::CANNOT_SAVE_FILE, .nFiles = 0 };
        }
    }

    return { .ec = SkinToneError::OK, .nFiles = things.size() };
}
