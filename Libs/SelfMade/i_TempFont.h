#pragma once

#include <QList>
#include <QString>

constexpr auto FONT_NOT_INSTALLED = -1000;
constexpr auto FONT_BADLY_INSTALLED = -1;
extern std::string tempPrefix;

struct TempFont {
    intptr_t id = FONT_NOT_INSTALLED;
    QList<QString> families;
};

TempFont installTempFontFull(QString fname, char32_t trigger);
QString expandTempFontName(std::string_view fname);
TempFont installTempFontRel(std::string_view fname, char32_t trigger);
