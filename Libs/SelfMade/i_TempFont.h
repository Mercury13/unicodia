#pragma once

#include <QString>

intptr_t installTempFontFull(QString fname);
QString expandTempFontName(std::string_view fname);
intptr_t installTempFontRel(std::string_view fname);
