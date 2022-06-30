#pragma once

// L10n
#include "LocDic.h"
#include "LocManager.h"

class QObject;
class QWidget;
class QTabWidget;

namespace loc {

    void translateBareObj(
            QObject* widget, QObject* mainForm, std::string_view prefix);
    void translateForm(QWidget* form);
    void translateContainerWidget(
            QObject* widget, QObject* mainForm,
            std::string_view prefix, std::string_view actionPrefix);
    void translateWidget(
            QObject* widget, QObject* mainForm,
            std::string_view prefix, std::string_view actionPrefix);
    void translateTabWidget(QTabWidget* widget, std::string_view prefix);
    bool translateProperty(
            QObject* object, std::string_view prefix, std::string_view name,
            const char* property);

    template <class That>   // CRTP → cannot write; waiting for C++23 this as param
    class Form : public Subject
    {
    public:
        void translateMe() override
        {
            if (auto fm = dynamic_cast<That*>(this)) {
                translateForm(fm);
            }
        }

        ~Form()
        {
            loc::man.erase(this);
        }
    };

}   // namespace loc
