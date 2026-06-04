#pragma once

#include "u_EnumSize.h"

class QString;
class QPalette;
class QWidget;
struct QColor;

namespace dark {

    DEFINE_ENUM_TYPE_IN_NS(dark, Setting, unsigned char,
            AUTO, LIGHT, DARK,
            DEFAULT = AUTO)

    /// @return [+] whether OS supports dark mode
    bool doesSystemSupport();

    /// @warning Call it BEFORE QApplication
    void init1(dark::Setting setting);
    /// @warning Call it AFTER QAllpication
    void init2(const QString& aFname);

    /// @return [+] whether dark is on according to OS settings
    bool doesSystemWant();

    /// @return [+] dark mode [-] light mode
    bool isActuallyOn() noexcept;

    /// Forcefully sets application to dark mode
    void forceOn();

    /// Forcefully sets application to light mode
    void forceOff();

    /// Sets application to dark/light mode depending on x and whether we already set
    void set(bool x);

    /// @return  relative colour lightness
    int lightness(const QColor& color) noexcept;

    /// @return [+] palette is dark  [-] unknown
    bool isPaletteDark(const QPalette& palette) noexcept;

    void processNewPalette();
}
