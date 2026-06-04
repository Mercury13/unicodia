#include "i_DarkMode.h"

#include <QString>
#include <QFile>
#include <QApplication>
#include <QPalette>
#include <QWidget>

#include <QStyleFactory>
namespace {
    QString fileName;
    QPalette oldPalette;
    dark::Setting progSetting = dark::Setting::AUTO;
    bool isDarkOn = false;
    std::optional<QPalette> darkPalette;
}

enum class Os {
    UNKNOWN,    // generic OS
    WINDOWS     // Windows: special bhv in forced dark
};

#ifdef _WIN32
constexpr Os CURR_OS = Os::WINDOWS;

// Win32 dark mode
#include <QOperatingSystemVersion>
#include <QSettings>

namespace {

    enum class DarkOs : unsigned char {
        UNSUPPORTED,  ///< controlled by someone else / unsupported completely
        //PRE_18362,  Won't support W10 THAT old
        NEW     ///< controlled by our unit
    };
    DarkOs darkOs = DarkOs::UNSUPPORTED;

    [[maybe_unused]] void discoverWindowsVersion()
    {
        auto os = QOperatingSystemVersion::current();
        // dark mode supported Windows 10 1809 10.0.17763 onward
        // https://stackoverflow.com/questions/53501268/win10-dark-theme-how-to-use-in-winapi
        if ( os.majorVersion() == 10 ) {
            if (os.microVersion() >= 18362) {
                darkOs = DarkOs::NEW;
            //} else if (os.microVersion() >= 17763) {
            //    darkOs = DarkOs::PRE_18362;
            } else {
                darkOs = DarkOs::UNSUPPORTED;
            }
        } else {
            darkOs = DarkOs::NEW;
        }
    }

    inline bool doesWindowsSupportDarkMode()
    {
        return (darkOs > DarkOs::UNSUPPORTED);
    }

    inline bool doesWindowsHaveDarkMode()
    {
        QSettings settings( "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat );
        return settings.value( "AppsUseLightTheme", 1 ).toInt() == 0;
    }

}
#endif  // _WIN32

bool dark::doesSystemWant()
{
#ifdef _WIN32
    return doesWindowsSupportDarkMode() && doesWindowsHaveDarkMode();
#else
    return false;
#endif
}

bool dark::doesSystemSupport()
{
#ifdef _WIN32
    return doesWindowsSupportDarkMode();
#else
    return false;
#endif
}

bool dark::isActuallyOn() noexcept { return isDarkOn; }


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


namespace {

    QApplication* getApp()
    {
        return qobject_cast<QApplication*>(QApplication::instance());
    }

    QPalette getDarkPalette()
    {
        QPalette pal;
        pal.setColor(QPalette::Window, QColor(53, 53, 53));
        pal.setColor(QPalette::WindowText, Qt::white);
        pal.setColor(QPalette::Base, QColor(35, 35, 35));
        pal.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
        pal.setColor(QPalette::ToolTipBase, QColor(25, 25, 25));
        pal.setColor(QPalette::ToolTipText, Qt::white);
        pal.setColor(QPalette::Text, Qt::white);
        pal.setColor(QPalette::Button, QColor(53, 53, 53));
        pal.setColor(QPalette::ButtonText, Qt::white);
        pal.setColor(QPalette::BrightText, Qt::red);
        pal.setColor(QPalette::Link, QColor(42, 130, 218));
        pal.setColor(QPalette::Light, QColor(99, 99, 99));
        pal.setColor(QPalette::Shadow, QColor(99, 99, 99));
        pal.setColor(QPalette::Midlight, QColor(99, 99, 99));
        pal.setColor(QPalette::Highlight, QColor(42, 130, 218));
        pal.setColor(QPalette::HighlightedText, Qt::white);
        pal.setColor(QPalette::Active, QPalette::Button, QColor(53, 53, 53));
        pal.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
        pal.setColor(QPalette::Disabled, QPalette::WindowText, Qt::darkGray);
        pal.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
        pal.setColor(QPalette::Disabled, QPalette::Light, QColor(53, 53, 53));
        return pal;
    }

}   // anon namespace


void dark::forceOn()
{
    if (!darkPalette) {
        darkPalette = getDarkPalette();
    }
    QApplication::setPalette(*darkPalette);
    isDarkOn = true;
    if (!fileName.isEmpty()) {
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
}


void dark::forceOff()
{
    QApplication::setPalette(oldPalette);
    getApp()->setStyleSheet({});
    isDarkOn = false;
}


void dark::processNewPalette()
{
    if (!doesSystemSupport())
        return;
    auto palette = QApplication::palette();
    if (bool isDark = isPaletteDark(palette);
            isDark != isDarkOn)
    {
        if (isDark) {
            forceOn();
        } else {
            forceOff();
        }
    }
}


void dark::init1(Setting setting)
{
    progSetting = setting;
    if (setting == Setting::LIGHT)
        return;     // do nothing if forced light

#ifdef _WIN32
    discoverWindowsVersion();
    if (doesSystemSupport()) {
        oldPalette = QApplication::palette();
        // Auto setting, dark palette → do not switch dark/light
        if (setting == Setting::AUTO && isPaletteDark(oldPalette)) {
            darkOs = DarkOs::UNSUPPORTED;
            return;
        }
        qputenv("QT_QPA_PLATFORM", "windows:darkmode=2");
    }
#endif
}


void dark::init2(const QString& aFname)
{
    fileName = aFname;
    if constexpr (CURR_OS == Os::WINDOWS) {
        // Dark Windows: set legacy, palette-controlled style
        // (Light Windows: totally get rid of dark mode
        //  Dynamic Windows: do it automatically)
        if (progSetting == Setting::DARK) {
            getApp()->setStyle(QStyleFactory::create("Windows"));
        }
    }
    if (doesSystemSupport()) {
        processNewPalette();
    }
}

int dark::lightness(const QColor& color) noexcept
{
    return color.red()   * 2
         + color.green() * 4
         + color.blue();
}

bool dark::isPaletteDark(const QPalette& palette) noexcept
{
    switch (progSetting) {
    case Setting::DARK:
        return true;
    case Setting::LIGHT:
        return false;
    case Setting::AUTO: break;
    }

    auto liWinText = lightness(palette.windowText().color());
    auto liWindow = lightness(palette.window().color());
    auto liGray = lightness(Qt::gray);          // liGray > liDarkGray
    auto liDarkGray = lightness(Qt::darkGray);
    return (liWinText > liWindow) && (liWinText > liDarkGray)
           && (liWindow < liGray);
}
