#include "RememberWindow.h"

// Qt
#include <QMainWindow>
#include <QApplication>
#include <QScreen>

// XML
#include "pugixml.hpp"

QSize config::window::requestedDesktopSize;
QSize config::window::actualDesktopSize;


void config::window::State::takeFrom(QWidget& widget)
{
    rect = widget.normalGeometry();
    isMax = widget.windowState().testFlag(Qt::WindowMaximized);
}


void config::window::load(const pugi::xml_node& root, State& state)
{
    auto oldW = state.rect.width();
    auto oldH = state.rect.height();
    auto hWin = root.child("window");
       state.rect.setLeft(hWin.attribute("x").as_int(state.rect.left()));
       state.rect.setTop(hWin.attribute("y").as_int(state.rect.top()));
       state.rect.setWidth(hWin.attribute("w").as_int(oldW));
       state.rect.setHeight(hWin.attribute("h").as_int(oldH));
       state.isMax = hWin.attribute("max").as_bool();

       auto hDesktop = hWin.child("desktop");
           config::window::requestedDesktopSize.setWidth (hDesktop.attribute("w").as_int());
           config::window::requestedDesktopSize.setHeight(hDesktop.attribute("h").as_int());
}


void config::window::save(pugi::xml_node& root, const State& state)
{
    // Maximized window will remain maximized regardless of desktop size,
    // probably on other monitor
    auto hWin = root.append_child("window");
        hWin.append_attribute("max") = state.isMax;

        if (!window::actualDesktopSize.isEmpty()) {
            hWin.append_attribute("x") = state.rect.left();
            hWin.append_attribute("y") = state.rect.top();
            hWin.append_attribute("w") = state.rect.width();
            hWin.append_attribute("h") = state.rect.height();

            auto hDesk = hWin.append_child("desktop");
                hDesk.append_attribute("w") = window::actualDesktopSize.width();
                hDesk.append_attribute("h") = window::actualDesktopSize.height();
        }
}


namespace {

    template <class T>
    inline T clampMin(const T& val, const T& min, const T& max)
        { return std::max(std::min(val, max), min); }

}


void config::window::setGeometry(QWidget& win, const State& state)
{
    static constexpr int BUFFER_ZONE = 80;  // We should see ? px of window
    static constexpr int HEADER_ZONE = 40;  // We should see ? px of header

    auto screens = QApplication::screens();
    QRect desktopRect = screens[0]->availableVirtualGeometry();
    config::window::actualDesktopSize = desktopRect.size();

    if (config::window::actualDesktopSize == config::window::requestedDesktopSize) {
        // Reduce width-height
        auto w = std::min(state.rect.width(), desktopRect.width());
        auto h = std::min(state.rect.height(), desktopRect.height());

        // Move x-y
        auto x = clampMin(state.rect.left(),
                          desktopRect.left() + BUFFER_ZONE - w,
                          desktopRect.right() - BUFFER_ZONE);
        auto y = clampMin(state.rect.top(),
                          desktopRect.top(),
                          desktopRect.bottom() - HEADER_ZONE);

        win.setGeometry(x, y, w, h);
    }

    auto winState = win.windowState();
    winState.setFlag(Qt::WindowMaximized, state.isMax);
    win.setWindowState(winState);
}
