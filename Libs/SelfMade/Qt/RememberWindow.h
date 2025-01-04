#pragma once

// Qt
#include <QRect>

namespace pugi {
    class xml_node;
}

class QWidget;

namespace config {

    namespace window {
        struct State {
            QRect rect;
            bool isMax = false;

            State() = default;
            explicit State(QWidget& widget) { takeFrom(widget); }
            void takeFrom(QWidget& widget);
        };

        extern QSize requestedDesktopSize;
        extern QSize actualDesktopSize;

        void load(const pugi::xml_node& root, State& state);
        void save(pugi::xml_node& root, const State& state);
        void setGeometry(QWidget& win, const State& state);
    }

}   // namespace config
