#pragma once

// L10n
#include "LocDic.h"
#include "LocManager.h"

class QObject;
class QWidget;

namespace loc {

    void translateForm(QWidget* form);
    void translateContainerWidget(QObject* widget, std::string_view prefix);
    void translateWidget(QObject* widget, std::string_view prefix);
    bool translateProperty(
            QObject* object, std::string_view prefix, std::string_view name,
            const char* property);

    template <class That> requires std::is_base_of_v<QWidget, That>
    class Form : public Subject
    {
    public:
        void translateMe() override
        {
            if (auto fm = dynamic_cast<That*>(this)) {
                translateForm(fm);
            }
        }
    };

}   // namespace loc
