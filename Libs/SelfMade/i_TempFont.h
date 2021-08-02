#pragma once

#include <QString>

constexpr auto FONT_NOT_INSTALLED = -1000;

struct TempFont {
    intptr_t id = FONT_NOT_INSTALLED;
    QString namesComma;
};

TempFont installTempFontFull(QString fname);
QString expandTempFontName(std::string_view fname);
TempFont installTempFontRel(std::string_view fname);
