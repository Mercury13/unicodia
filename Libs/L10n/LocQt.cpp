#include "LocQt.h"

// Qt
#include <QWidget>
#include <QVariant>
#include <QDialogButtonBox>
#include <QTabWidget>
#include <QMetaProperty>
#include <QAction>

// Libs
#include "u_Strings.h"
#include "u_Qstrings.h"

using namespace std::string_view_literals;


void loc::translateContainerWidget(
        QObject* widget, QObject* mainForm,
        std::string_view prefix, std::string_view actionPrefix)
{
    for (auto& child : widget->children()) {
        translateWidget(child, mainForm, prefix, actionPrefix);
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


void loc::translateTabWidget(QTabWidget* widget, std::string_view prefix)
{
    std::string key;
    for (int i = 0; i < widget->count(); ++i) {
        QWidget* child = widget->widget(i);
        QString name1 = child->objectName();
        if (!name1.isEmpty()) {
            str::catIp(key, prefix, name1.toStdString(), ":at" );
            if (auto res = loc::dic.getIf(key)) {
                widget->setTabText(i, res->q());
            }
        }
    }
}


void loc::translateBareObj(
        QObject* widget, QObject* mainForm, std::string_view prefix)
{
    QString name1 = widget->objectName();
    if (name1.isEmpty())
        return;
    std::string key1 { prefix };
    if (widget != mainForm) {
        key1 += name1.toStdString();
    }

    auto metaObj = widget->metaObject();
    auto nProps = metaObj->propertyCount();
    for (int i = 0; i < nProps; ++i) {
        auto metaProp = metaObj->property(i);
        if (metaProp.typeId() == QMetaType::QString) {
            std::string_view propName = metaProp.name();    // const char*, so data() OK
            std::string* pKey = &key1;
            std::string tempKey;
            if (propName == "text"sv || propName == "title"sv) {
                // do nothing
            } else {
                pKey = &tempKey;
                tempKey = str::cat(key1, ":", propName);
            }
            if (auto res = loc::dic.getIf(*pKey)) {
                widget->setProperty(propName.data(), res->q());
            }
        }
    }
}

void loc::translateWidget(
        QObject* widget, QObject* mainForm,
        std::string_view prefix, std::string_view actionPrefix)
{
    bool isAction = qobject_cast<QAction*>(widget);
    auto prefix1 = isAction ? actionPrefix : prefix;
    translateBareObj(widget, mainForm, prefix1);

    if (auto tabWidget = qobject_cast<QTabWidget*>(widget)) {
        translateTabWidget(tabWidget, prefix);
    }

    // No button boxes right now
//    else if (auto buttonBox = qobject_cast<QDialogButtonBox*>(widget)) {
//        //translateButtonBox(buttonBox);
//    }

    translateContainerWidget(widget, mainForm, prefix, actionPrefix);
}


void loc::translateForm(QWidget* form)
{
    std::string_view objName = form->metaObject()->className();

    static constinit std::string_view standardNames[] { "Fm", "Wi" };

    std::string_view formKey = objName;
    for (auto v : standardNames) {
        if (auto remder = str::remainderSv(objName, v);
                !remder.empty()) {
            formKey = remder;
            break;
        }
    }

    auto prefix = str::cat( formKey, ".Ui." );
    auto actionPrefix = str::cat( prefix, "actions." );
    translateWidget(form, form, prefix, actionPrefix);
}
