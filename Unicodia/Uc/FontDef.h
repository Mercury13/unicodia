#pragma once

#include <QString>
#include <QFontDatabase>
#include "u_Vector.h"

enum class FontPrio : unsigned char { BAD, NORMAL, GOOD, MAIN };

struct FontLine {
    QString name;
    FontPrio priority = FontPrio::NORMAL;
};

struct FontList {
    SafeVector<FontLine> lines;
    bool hasMore = false;
};

class FontSource    // interface
{
public:
    virtual FontList allSysFonts(
            char32_t cp, QFontDatabase::WritingSystem ws, size_t maxCount) = 0;
    virtual ~FontSource() = default;
};

class EgypChecker   // interface
{
public:
    virtual bool hasEgypI() const = 0;
    virtual ~EgypChecker() = default;
};
