#include "i_DarkMode.h"

#include <QString>
#include <QFile>
#include <QApplication>

QString dark::fileName = ":/dark.qss";

namespace {
    bool isDarkOn = false;
}

#ifdef _WIN32
#include <QOperatingSystemVersion>
#include <QSettings>

namespace {
    inline bool doesWindowsSupportDarkMode()
    {
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

bool dark::doesSystemWant()
{
#ifdef _WIN32
    return doesWindowsSupportDarkMode() && doesWindowsHaveDarkMode();
#else
    return false;
#endif
}

bool dark::doesUserWant(Mode mode)
{
    switch (mode) {
    case Mode::AUTO:
        return doesSystemWant();
    case Mode::ALWAYS_DARK:
        return true;
    case Mode::ALWAYS_LIGHT:
        return false;
    }
    __builtin_unreachable();
}


bool dark::isActuallyOn() { return isDarkOn; }


void dark::turnOn()
{
    if (!isDarkOn)
        forceOn();
}

void dark::turnOff()
{
    if (isDarkOn)
        forceOff();
}

void dark::set(bool x)
{
    if (isDarkOn == x)
        return;
    if (x) {
        forceOn();
    } else {
        forceOff();
    }
}


void dark::set(Mode mode)
    { set(doesUserWant(mode)); }


namespace {

    QApplication* getApp()
    {
        return qobject_cast<QApplication*>(QApplication::instance());
    }

}


void dark::forceOn()
{
    QFile f(fileName);
    if ( !f.exists() ) {
        forceOff();
        return;
    }
   f.open( QFile::ReadOnly | QFile::Text );
   QTextStream ts( &f );
   getApp()->setStyleSheet( ts.readAll() );
   isDarkOn = true;
}


void dark::forceOff()
{
    getApp()->setStyleSheet({});
    isDarkOn = false;
}
