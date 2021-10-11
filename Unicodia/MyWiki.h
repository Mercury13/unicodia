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

namespace str {
    class QSep;
}

namespace uc {
    class BidiClass;
    class Category;
    class Font;
    class Script;
    class Version;
    class Cp;
    class Block;
    class Term;
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
        virtual void copyTextAbs(
                QWidget* widget, const QRect& absRect, const QString& text) = 0;
        virtual ~Gui() = default;

        // Utils
        void popupAtRel(
                QWidget* widget, const QRect& relRect, const QString& html);
        void popupAtRelMaybe(
                QWidget* widget, TinyOpt<QRect> relRect, const QString& html);
        void popupAtWidget(QWidget* widget, const QString& html);
        void copyTextRel(
                QWidget* widget, TinyOpt<QRect> relRect, const QString& text);
    };

    class Link    // interface
    {
    public:
        virtual void go(QWidget* widget, TinyOpt<QRect> rect, Gui& gui) = 0;
        virtual ~Link() = default;
    };

    std::unique_ptr<Link> parseLink(std::string_view link);
    std::unique_ptr<Link> parseLink(std::string_view link);
    std::unique_ptr<Link> parseLink(std::string_view scheme, std::string_view target);
    std::unique_ptr<Link> parsePopBidiLink(std::string_view target);
    std::unique_ptr<Link> parsePopCatLink(std::string_view target);
    std::unique_ptr<Link> parsePopFontsLink(std::string_view target);
    std::unique_ptr<Link> parsePopScriptLink(std::string_view target);
    std::unique_ptr<Link> parsePopTermLink(std::string_view target);
    std::unique_ptr<Link> parsePopIBlockLink(std::string_view target);
    QString buildHtml(const uc::BidiClass& x);
    QString buildHtml(const uc::Category& x);
    QString buildHtml(const uc::Script& x);
    QString buildHtml(const uc::Term& x);
    QString buildHtml(const uc::Block& x);
    QString buildFontsHtml(const char32_t cp, QFontDatabase::WritingSystem ws, Gui& gui);
    QString buildHtml(const uc::Cp& cp, const uc::Block* hint,
            const std::optional<QFont>& font, QFontDatabase::WritingSystem ws);
    void appendStylesheet(QString& text);
    void appendHtml(QString& text, const uc::Script& x, bool isScript);
    void go(QWidget* widget, TinyOpt<QRect> rect, Gui& gui, std::string_view link);
    void appendNoFont(QString& x, const std::u8string_view wiki);
    void append(QString& x, const std::u8string_view wiki, const uc::Font& font);
    void appendVersionValue(QString& text, const uc::Version& version);
    void appendVersion(QString& text, std::u8string_view prefix, const uc::Version& version);
    void appendUtf(QString& text, str::QSep& sp, char32_t code);
    void appendMissingCharInfo(QString& text, const uc::Block* hint, char32_t code);
    QString buildNonCharHtml(char32_t code, const uc::Block* hint);
    QString buildEmptyCpHtml(char32_t code, const QColor& color, const uc::Block* hint);

}   // namespace mywiki
