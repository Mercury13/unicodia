#pragma once

// STL
#include <memory>

// Qt
#include <QString>
#include <QFontDatabase>

// Common libs
#include "u_TinyOpt.h"
#include "FontDef.h"

class QWidget;
class QRect;

namespace uc {
    class BidiClass;
    class Category;
}

namespace mywiki
{
    class Gui    // interface
    {
    public:
        virtual void popupAtAbs(
                QWidget* widget, const QRect& absRect, const QString& html) = 0;
        virtual FontList allSysFonts(
                char32_t cp, QFontDatabase::WritingSystem ws, size_t maxCount) = 0;
        virtual ~Gui() = default;

        // Utils
        void popupAtRel(
                QWidget* widget, const QRect& relRect, const QString& html);
        void popupAtRelMaybe(
                QWidget* widget, TinyOpt<QRect> relRect, const QString& html);
        void popupAtWidget(QWidget* widget, const QString& html);
    };

    class Link    // interface
    {
    public:
        virtual void go(QWidget* widget, TinyOpt<QRect> rect, Gui& gui) = 0;
        virtual ~Link() = default;
    };

    std::unique_ptr<Link> parseLink(std::string_view link);
    std::unique_ptr<Link> parseLink(std::string_view scheme, std::string_view target);
    std::unique_ptr<Link> parsePopBidiLink(std::string_view target);
    std::unique_ptr<Link> parsePopCatLink(std::string_view target);
    std::unique_ptr<Link> parsePopFontsLink(std::string_view target);
    QString buildHtml(const uc::BidiClass& x);
    QString buildHtml(const uc::Category& x);
    QString buildFontsHtml(const char32_t cp, QFontDatabase::WritingSystem ws, Gui& gui);
    void go(QWidget* widget, TinyOpt<QRect> rect, Gui& gui, std::string_view link);
    void appendNoFont(QString& x, const std::u8string_view wiki);

}   // namespace mywiki
