#pragma once

#include <QList>
#include <QString>

constexpr auto FONT_NOT_INSTALLED = -1000;

struct TempFont {
    intptr_t id = FONT_NOT_INSTALLED;
    QList<QString> families;
};

TempFont installTempFontFull(QString fname);
QString expandTempFontName(std::string_view fname);
TempFont installTempFontRel(std::string_view fname);
