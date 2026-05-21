#include "i_DarkMode.h"

#include <QString>
#include <QFile>
#include <QApplication>
#include <QPalette>

QString dark::fileName;
std::optional<QPalette> dark::palette;

namespace {
    bool isDarkOn = false;
}

#ifdef _WIN32
// Win32 dark mode
#include <QOperatingSystemVersion>
#include <QSettings>
#include <windows.h>
#include <uxtheme.h>
#include "IatHook.h"

namespace {

    enum class DarkOs : unsigned char {
        UNSUPPORTED,
        //PRE_18362,  Won't support W10 THAT old
        NEW
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

    enum IMMERSIVE_HC_CACHE_MODE
    {
        IHCM_USE_CACHED_VALUE,
        IHCM_REFRESH
    };

    // 1903 18362
    enum PreferredAppMode
    {
        Default,
        AllowDark,
        ForceDark,
        ForceLight,
        Max
    };

    enum WINDOWCOMPOSITIONATTRIB
    {
        WCA_UNDEFINED = 0,
        WCA_NCRENDERING_ENABLED = 1,
        WCA_NCRENDERING_POLICY = 2,
        WCA_TRANSITIONS_FORCEDISABLED = 3,
        WCA_ALLOW_NCPAINT = 4,
        WCA_CAPTION_BUTTON_BOUNDS = 5,
        WCA_NONCLIENT_RTL_LAYOUT = 6,
        WCA_FORCE_ICONIC_REPRESENTATION = 7,
        WCA_EXTENDED_FRAME_BOUNDS = 8,
        WCA_HAS_ICONIC_BITMAP = 9,
        WCA_THEME_ATTRIBUTES = 10,
        WCA_NCRENDERING_EXILED = 11,
        WCA_NCADORNMENTINFO = 12,
        WCA_EXCLUDED_FROM_LIVEPREVIEW = 13,
        WCA_VIDEO_OVERLAY_ACTIVE = 14,
        WCA_FORCE_ACTIVEWINDOW_APPEARANCE = 15,
        WCA_DISALLOW_PEEK = 16,
        WCA_CLOAK = 17,
        WCA_CLOAKED = 18,
        WCA_ACCENT_POLICY = 19,
        WCA_FREEZE_REPRESENTATION = 20,
        WCA_EVER_UNCLOAKED = 21,
        WCA_VISUAL_OWNER = 22,
        WCA_HOLOGRAPHIC = 23,
        WCA_EXCLUDED_FROM_DDA = 24,
        WCA_PASSIVEUPDATEMODE = 25,
        WCA_USEDARKMODECOLORS = 26,
        WCA_LAST = 27
    };

    struct WINDOWCOMPOSITIONATTRIBDATA
    {
        WINDOWCOMPOSITIONATTRIB Attrib;
        PVOID pvData;
        SIZE_T cbData;
    };

    using fnRtlGetNtVersionNumbers = void (WINAPI *)(LPDWORD major, LPDWORD minor, LPDWORD build);
    using fnSetWindowCompositionAttribute = BOOL (WINAPI *)(HWND hWnd, WINDOWCOMPOSITIONATTRIBDATA*);
    // 1809 17763
    using fnShouldAppsUseDarkMode = bool (WINAPI *)(); // ordinal 132
    using fnAllowDarkModeForWindow = bool (WINAPI *)(HWND hWnd, bool allow); // ordinal 133
    using fnAllowDarkModeForApp = bool (WINAPI *)(bool allow); // ordinal 135, in 1809
    using fnFlushMenuThemes = void (WINAPI *)(); // ordinal 136
    using fnRefreshImmersiveColorPolicyState = void (WINAPI *)(); // ordinal 104
    using fnIsDarkModeAllowedForWindow = bool (WINAPI *)(HWND hWnd); // ordinal 137
    using fnGetIsImmersiveColorUsingHighContrast = bool (WINAPI *)(IMMERSIVE_HC_CACHE_MODE mode); // ordinal 106
    using fnOpenNcThemeData = HTHEME(WINAPI *)(HWND hWnd, LPCWSTR pszClassList); // ordinal 49
    // 1903 18362
    using fnShouldSystemUseDarkMode = bool (WINAPI *)(); // ordinal 138
    using fnSetPreferredAppMode = PreferredAppMode (WINAPI *)(PreferredAppMode appMode); // ordinal 135, in 1903
    using fnIsDarkModeAllowedForApp = bool (WINAPI *)(); // ordinal 139

    fnSetWindowCompositionAttribute _SetWindowCompositionAttribute = nullptr;
    fnShouldAppsUseDarkMode _ShouldAppsUseDarkMode = nullptr;
    fnAllowDarkModeForWindow _AllowDarkModeForWindow = nullptr;
    fnAllowDarkModeForApp _AllowDarkModeForApp = nullptr;
    //fnFlushMenuThemes _FlushMenuThemes = nullptr;
    fnRefreshImmersiveColorPolicyState _RefreshImmersiveColorPolicyState = nullptr;
    fnIsDarkModeAllowedForWindow _IsDarkModeAllowedForWindow = nullptr;
    fnGetIsImmersiveColorUsingHighContrast _GetIsImmersiveColorUsingHighContrast = nullptr;
    fnOpenNcThemeData _OpenNcThemeData = nullptr;
    // 1903 18362
    //fnShouldSystemUseDarkMode _ShouldSystemUseDarkMode = nullptr;
    fnSetPreferredAppMode _SetPreferredAppMode = nullptr;

    bool g_darkModeEnabled = false;

    [[maybe_unused]] bool AllowDarkModeForWindow(HWND hWnd, bool allow)
    {
        if (doesWindowsSupportDarkMode())
            return _AllowDarkModeForWindow(hWnd, allow);
        return false;
    }

    bool IsHighContrast()
    {
        HIGHCONTRASTW highContrast { sizeof(highContrast), 0, nullptr };
        if (SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(highContrast), &highContrast, FALSE))
            return highContrast.dwFlags & HCF_HIGHCONTRASTON;
        return false;
    }

    [[maybe_unused]] void RefreshTitleBarThemeColor(HWND hWnd)
    {
        BOOL dark = FALSE;
        if (_IsDarkModeAllowedForWindow(hWnd) &&
            _ShouldAppsUseDarkMode() &&
            !IsHighContrast())
        {
            dark = TRUE;
        }
        //if (darkOs == DarkOs::PRE_18362)
        //    SetPropW(hWnd, L"UseImmersiveDarkModeColors", reinterpret_cast<HANDLE>(static_cast<INT_PTR>(dark)));
        else if (_SetWindowCompositionAttribute)
        {
            WINDOWCOMPOSITIONATTRIBDATA data = { WCA_USEDARKMODECOLORS, &dark, sizeof(dark) };
            _SetWindowCompositionAttribute(hWnd, &data);
        }
    }

    bool IsColorSchemeChangeMessage(LPARAM lParam)
    {
        bool is = false;
        if (lParam && CompareStringOrdinal(reinterpret_cast<LPCWCH>(lParam), -1, L"ImmersiveColorSet", -1, TRUE) == CSTR_EQUAL)
        {
            _RefreshImmersiveColorPolicyState();
            is = true;
        }
        _GetIsImmersiveColorUsingHighContrast(IHCM_REFRESH);
        return is;
    }

    [[maybe_unused]] bool IsColorSchemeChangeMessage(UINT message, LPARAM lParam)
    {
        if (message == WM_SETTINGCHANGE)
            return IsColorSchemeChangeMessage(lParam);
        return false;
    }

    void AllowDarkModeForApp(bool allow)
    {
        if (_AllowDarkModeForApp)
            _AllowDarkModeForApp(allow);
        else if (_SetPreferredAppMode)
            _SetPreferredAppMode(allow ? AllowDark : Default);
    }

    void FixDarkScrollBar()
    {
        HMODULE hComctl = LoadLibraryExW(L"comctl32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (hComctl)
        {
            auto addr = FindDelayLoadThunkInModule(hComctl, "uxtheme.dll", 49); // OpenNcThemeData
            if (addr)
            {
                DWORD oldProtect;
                if (VirtualProtect(addr, sizeof(IMAGE_THUNK_DATA), PAGE_READWRITE, &oldProtect))
                {
                    auto MyOpenThemeData = [](HWND hWnd, LPCWSTR classList) -> HTHEME {
                        if (wcscmp(classList, L"ScrollBar") == 0)
                        {
                            hWnd = nullptr;
                            classList = L"Explorer::ScrollBar";
                        }
                        return _OpenNcThemeData(hWnd, classList);
                    };

                    addr->u1.Function = reinterpret_cast<ULONG_PTR>(static_cast<fnOpenNcThemeData>(MyOpenThemeData));
                    VirtualProtect(addr, sizeof(IMAGE_THUNK_DATA), oldProtect, &oldProtect);
                }
            }
        }
    }

    template <class Func>
    Func funcTo(FARPROC x) { return (Func)(void*)x; }

    [[maybe_unused]] void InitDarkMode()
    {
        discoverWindowsVersion();
        if (darkOs == DarkOs::UNSUPPORTED)
            return;
        HMODULE hUxtheme = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (hUxtheme)
        {
            _OpenNcThemeData = funcTo<fnOpenNcThemeData>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(49)));
            _RefreshImmersiveColorPolicyState = funcTo<fnRefreshImmersiveColorPolicyState>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(104)));
            _GetIsImmersiveColorUsingHighContrast = funcTo<fnGetIsImmersiveColorUsingHighContrast>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(106)));
            _ShouldAppsUseDarkMode = funcTo<fnShouldAppsUseDarkMode>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(132)));
            _AllowDarkModeForWindow = funcTo<fnAllowDarkModeForWindow>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(133)));

            auto ord135 = GetProcAddress(hUxtheme, MAKEINTRESOURCEA(135));
//            if (darkOs == DarkOs::PRE_18362)
//                _AllowDarkModeForApp = funcTo<fnAllowDarkModeForApp>(ord135);
//            else
                _SetPreferredAppMode = funcTo<fnSetPreferredAppMode>(ord135);

            //_FlushMenuThemes = reinterpret_cast<fnFlushMenuThemes>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(136)));
            _IsDarkModeAllowedForWindow = funcTo<fnIsDarkModeAllowedForWindow>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(137)));

            _SetWindowCompositionAttribute = funcTo<fnSetWindowCompositionAttribute>(GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute"));

            if (_OpenNcThemeData &&
                _RefreshImmersiveColorPolicyState &&
                _ShouldAppsUseDarkMode &&
                _AllowDarkModeForWindow &&
                (_AllowDarkModeForApp || _SetPreferredAppMode) &&
                //_FlushMenuThemes &&
                _IsDarkModeAllowedForWindow)
            {
                darkOs = DarkOs::UNSUPPORTED;

                AllowDarkModeForApp(true);
                _RefreshImmersiveColorPolicyState();

                g_darkModeEnabled = _ShouldAppsUseDarkMode() && !IsHighContrast();

                FixDarkScrollBar();
            }
        }
    }

}   // anon namespace

#endif  // _WIN32

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
    if (palette) {
        QApplication::setPalette(*palette);
        isDarkOn = true;
    }
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
    getApp()->setStyleSheet({});
    isDarkOn = false;
}


void dark::init()
{
#ifdef _WIN32
    // Sorry, cannot test
    //InitDarkMode();
#endif
}
