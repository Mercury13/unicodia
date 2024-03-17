#pragma once

#include <cstddef>

enum class SkinToneError {
    OK, CANNOT_CREATE_DIR, CANNOT_REMOVE_FILE, REPEATING_EMOJI,
    INCOMPLETE_EMOJI, CANNOT_SAVE_FILE };

struct SkinToneQa
{
    SkinToneError ec;
    size_t nFiles;
};

SkinToneQa doSkinToneQa();
