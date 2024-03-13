#pragma once

// STL
#include <memory>

// Qt
#include <QString>
#include <QFontDatabase>
#include <QTextDocument>

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
    class LibNode;
    class GlyphStyleChannel;
}

enum class Want32 { NO, YES };

namespace mywiki {

    /// Interface that’s used to walk through internal links
    /// Every type of link has its own function
    ///
    /// Architectural justification: Gui is somehow detached class,
    /// and InternalWalker is closely related to main window
    ///
    class InternalWalker    // interface
    {
    public:
        virtual void gotoCp(QWidget* initiator, char32_t cp) = 0;
        virtual void blinkAddCpToFavs() = 0;
        virtual ~InternalWalker() = default;
    };

    class Gui    // interface
    {
    public:
        /// @warning   Widgets are NEVER nullptr
        virtual void popupAtAbs(
                QWidget* widget, const QRect& absRect, const QString& html) = 0;
        virtual void copyTextAbs(
                QWidget* widget, const QRect& absRect, const QString& text) = 0;
        /// Follows standard internet link
        virtual void followUrl(const QString& x) = 0;
        virtual FontSource& fontSource() = 0;
        virtual InternalWalker& internalWalker() = 0;

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

    enum class LinkClass { POPUP, COPY, INTERNAL, INET };

    class Link    // interface
    {
    public:
        virtual void go(QWidget* widget, TinyOpt<QRect> rect, Gui& gui) = 0;
        virtual LinkClass clazz() const { return LinkClass::POPUP; }
        virtual ~Link() = default;
    };

    void translateDatingLoc();

    std::unique_ptr<Link> parseLink(std::string_view link);
    std::unique_ptr<Link> parseLink(std::string_view link);
    std::unique_ptr<Link> parseLink(std::string_view scheme, std::string_view target);
    std::unique_ptr<Link> parsePopBidiLink(std::string_view target);
    std::unique_ptr<Link> parsePopCatLink(std::string_view target);
    std::unique_ptr<Link> parsePopFontsLink(std::string_view target);
    std::unique_ptr<Link> parsePopScriptLink(std::string_view target);
    std::unique_ptr<Link> parsePopTermLink(std::string_view target);
    std::unique_ptr<Link> parsePopIBlockLink(std::string_view target);
    std::unique_ptr<Link> parsePopVersionLink(std::string_view target);
    std::unique_ptr<Link> parsePopGlyphStyleLink(std::string_view target);
    std::unique_ptr<Link> parseGotoCpLink(std::string_view target);
    std::unique_ptr<Link> parseGotoInterfaceLink(std::string_view target);
    QString buildHtml(const uc::BidiClass& x);
    QString buildHtml(const uc::Category& x);
    QString buildHtml(const uc::Script& x);
    QString buildHtml(const uc::Term& x);
    QString buildHtml(const uc::Block& x);
    QString buildFontsHtml(char32_t cp, QFontDatabase::WritingSystem ws, Gui& gui);
    QString buildHtml(const uc::Cp& cp);
    QString buildLibFolderHtml(const uc::LibNode& node, const QColor& color);
    QString buildHtml(const uc::LibNode& node, const uc::LibNode& parent);
    QString buildHtml(const uc::Version& version);
    QString buildHtml(const uc::GlyphStyleChannel& channel);
    QString buildEmptyFavsHtml();
    void appendStylesheet(QString& text, bool hasSignWriting = false);
    void go(QWidget* widget, TinyOpt<QRect> rect, Gui& gui, std::string_view link);
    void appendCopyable(QString& text, const QString& x, std::string_view clazz="copy");
    void appendCopyable(QString& text, unsigned x, std::string_view clazz="copy");
    void appendCopyable2(QString& text,
                         const QString& full,
                         const QString& shrt,
                         std::string_view clazz="copy");
    void appendHtml(QString& text, const uc::Script& x, bool isScript);
    void appendNoFont(QString& x, std::u8string_view wiki);
    void append(QString& x, std::u8string_view wiki, const uc::Font& font);
    void appendVersionValue(QString& text, const uc::Version& version);
    void appendEmojiValue(QString& text, const uc::Version& version, const uc::Version& prevVersion);
    void appendVersion(QString& text, std::u8string_view prefix, const uc::Version& version);
    void appendUtf(QString& text, Want32 want32, str::QSep& sp, char32_t code);
    void appendUtf(QString& text, Want32 want32, str::QSep& sp, std::u32string_view value);
    void appendMissingCharInfo(QString& text, char32_t code);
    QString buildNonCharHtml(char32_t code);
    QString buildVacantCpHtml(char32_t code, const QColor& color);
    bool isEngTermShown(const uc::Term& term);

    void hackDocument(QTextDocument* doc);

}   // namespace mywiki
