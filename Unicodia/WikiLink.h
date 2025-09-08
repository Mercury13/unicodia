#pragma once

// Qt
#include <QWidget>

// Libs
#include "u_TinyOpt.h"

namespace mywiki {

    // Let it be this way?
    class Gui;

    enum class LinkClass : unsigned char {
        POPUP,      ///< Opens a popup window, green
        COPY,       ///< Copies text, pale
        INTERNAL,   ///< Goes somewhere within program, blue
        INET,       ///< Opens browser/mailer, also blue
        SEARCH = INTERNAL   ///< Opens search; as we use enum for colour only, no difference
    };

    class Link    // interface
    {
    public:
        virtual void go(QWidget* widget, TinyOpt<QRect> rect, Gui& gui) = 0;
        virtual LinkClass clazz() const { return LinkClass::POPUP; }
        virtual ~Link() = default;
    };

    class HistoryLink : public Link
    {
    public:
        LinkClass clazz() const final { return LinkClass::POPUP; }
    };
}
