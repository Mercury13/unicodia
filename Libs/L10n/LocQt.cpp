#include "LocQt.h"

// Qt
#include <QWidget>
#include <QVariant>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QDialogButtonBox>
#include <QTabWidget>

// Libs
#include "u_Qstrings.h"


void loc::translateContainerWidget(QObject* widget, std::string_view prefix)
{
    for (auto& child : widget->children()) {
        translateWidget(child, prefix);
    }
}


bool loc::translateProperty(
        QObject* object, std::string_view prefix, std::string_view name,
        const char* property)
{
    std::string key { prefix };
        key += name;
        key += ':';
        key += property;
    auto res = loc::dic.getIf(key);
    if (!res)
        return false;
    object->setProperty(property, res->q());
    return true;
}


void loc::translateWidget(QObject* widget, std::string_view prefix)
{
    QString name1 = widget->objectName();
    if (name1.isEmpty())
        return;
    std::string name2 = name1.toStdString();
    std::string key { prefix };
        key += name2;

    /// @todo [urgent] getS — what it was?
    auto res = loc::dic.getIf(key);
    if (res) {
        QVariant v = widget->property("text");
        if (v.metaType().id() == QMetaType::QString) {
            widget->setProperty("text", res->q());
        } else {
            v = widget->property("title");
            if (v.metaType().id() == QMetaType::QString) {
                widget->setProperty("title", res->q());
            }
        }
    } else {
        if (auto lineEdit = qobject_cast<QLineEdit*>(widget)) {
            translateProperty(lineEdit, prefix, name2, "placeholderText");
        } else if (auto textEdit = qobject_cast<QPlainTextEdit*>(widget)) {
            translateProperty(textEdit, prefix, name2, "placeholderText");
        } else if (auto tabWidget = qobject_cast<QTabWidget*>(widget)) {
            /// @todo [urgent] what to do?
            //translateTabWidget(tabWidget, prefix);
        } else if (auto buttonBox = qobject_cast<QDialogButtonBox*>(widget)) {
            /// @todo [urgent] what to do?
            //translateButtonBox(buttonBox);
        }
    }

    translateContainerWidget(widget, prefix);
}


void loc::translateForm(QWidget* form)
{
    /// @todo [L10n] translate form
}
