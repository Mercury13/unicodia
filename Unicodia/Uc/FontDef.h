#pragma once

#include <QString>
#include "u_Vector.h"

enum class FontPrio { BAD, NORMAL, GOOD, MAIN };

struct FontLine {
    QString name;
    FontPrio priority = FontPrio::NORMAL;
};

struct FontList {
    SafeVector<FontLine> lines;
    bool hasMore = false;
};

