#pragma once

#include <QString>

void installTempFontFull(QString fname);
QString expandTempFontName(std::string_view fname);
void installTempFontRel(std::string_view fname);
