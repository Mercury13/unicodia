#include "i_DarkMode.h"

#ifdef _WIN32
#include <QOperatingSystemVersion>
#include <QSettings>

namespace {
    inline bool doesWindowsSupportDarkMode()
    {
        QSettings settings( "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat );
        return settings.value( "AppsUseLightTheme", 1 ).toInt() == 0;

        auto os = QOperatingSystemVersion::current();
        // dark mode supported Windows 10 1809 10.0.17763 onward
        // https://stackoverflow.com/questions/53501268/win10-dark-theme-how-to-use-in-winapi
        if ( os.majorVersion() == 10 ) {
            return os.microVersion() >= 17763;
        }
        else return (os.majorVersion() > 10);
    }

    inline bool doesWindowsHaveDarkMode()
    {
        QSettings settings( "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat );
        return settings.value( "AppsUseLightTheme", 1 ).toInt() == 0;
    }
}
#endif

bool dark::isOn()
{
#ifdef _WIN32
    return doesWindowsSupportDarkMode() && doesWindowsHaveDarkMode();
#else
    return false;
#endif
}
