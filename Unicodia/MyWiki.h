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

namespace wiki {
    enum Mode : unsigned char;
}

namespace uc {
    struct Numeric;
    struct BidiClass;
    struct Category;
    struct Font;
    struct Script;
    struct Version;
    struct Cp;
    struct Block;
    struct Term;
    struct LibNode;
    struct GlyphStyleChannel;
    class Request;
    enum class EcVersion : unsigned char;
    struct Lang;
    namespace old {
        struct Info;
    }
}

/// Whether we need UTF-32 in current state of wiki
/// (do not need in Blocks, and need in Library)
enum class Want32 : unsigned char { NO, YES };

namespace mywiki {

    /// Interface that’s used to walk through internal links
    /// Every type of link has its own function
    ///
    /// Internal link = significantly changes program’s state,
    ///     or relies on main window’s layout
    /// Examples: search, go to character, point to Favourites…
    /// Popup, copy and internet do not change and are not internal!
    ///
    /// Architectural justification: Gui is somehow detached class
    /// (relies on general features of OS and windowing framework),
    /// and InternalWalker is just a simplified model of main window
    ///
    class InternalLinkWalker    // interface
    {
    public:
        /// Goes to codepoint of Blocks tab
        virtual void gotoCp(QWidget* initiator, char32_t cp) = 0;
        /// Goes to codepoint of Library tab
        /// May fail but we do not check for errors, as we can read
        ///   Unicode data and can ensure that the link will surely work
        virtual void gotoLibCp(QWidget* initiator, char32_t cp) = 0;
        /// Blink “Favs are empty, go here and add”
        ///   (actual link is in lockit, Main.NoFavs)
        virtual void blinkAddCpToFavs() = 0;
        /// Searches for some request
        virtual void searchForRequest(const uc::Request& request) = 0;
        /// Just a simple dtor
        virtual ~InternalLinkWalker() = default;
    };

    ///  User interface, general features
    ///  A bit yo-yo’ish (Gui = some class, Walker = main window),
    ///    but see PopupGui class (special generic GUI for pop-up windows);
    ///    nicer architecture will require more code for nothing
    class Gui    // interface
    {
    public:
        /// @param [in] widget     Widget who initiated copying, NEVER null
        /// @param [in] absRect    Absolute (in desktop coords) rectangle
        ///                         that encloses link
        ///                         Window will shun this rectangle if possible
        /// @param [in] text       QHTML of popup window
        virtual void popupAtAbs(
                QWidget* widget, const QRect& absRect, const QString& html) = 0;

        /// Copies text to clipboard, writes “Copied”
        /// @param [in] widget     Widget who initiated copying, NEVER null
        /// @param [in] absRect    Absolute (in desktop coords) rectangle
        ///                         that encloses link/cursor/widget
        ///                         Window will shun this rectangle if possible
        /// @param [in] text       Text to copy
        virtual void copyTextAbs(
                QWidget* widget, const QRect& absRect, const QString& text) = 0;

        /// Follows standard internet link: HTTP(S), MailTo…
        /// (Wiki itself parses links and decides what to do on click)
        virtual void followUrl(const QString& x) = 0;

        virtual FontSource& fontSource() = 0;
        virtual InternalLinkWalker& linkWalker() = 0;

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

    class DescFont {
    public:
        const uc::Font& v;
        DescFont(const uc::Font& font) noexcept : v(getDescFont(font)) {}
        operator const uc::Font& () noexcept { return v; }
        const uc::Font& operator *  () const noexcept { return v;  }
        const uc::Font* operator -> () const noexcept { return &v; }
    private:
        static const uc::Font& getDescFont(const uc::Font& font);
    };

    struct Context {
        DescFont font;
        const uc::Lang* lang;
        std::string_view locPrefixDot;
    };

    struct AppendWiki {
        bool hasNSpeakers;
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
    std::unique_ptr<Link> parsePopOldCompLink(std::string_view target);
    std::unique_ptr<Link> parsePopGlyphStyleLink(std::string_view target);
    std::unique_ptr<Link> parseGotoCpLink(std::string_view target);
    std::unique_ptr<Link> parseGotoLibCpLink(std::string_view target);
    std::unique_ptr<Link> parseGotoInterfaceLink(std::string_view target);
    std::unique_ptr<Link> parseCharRequestLink(std::string_view target);
    std::unique_ptr<Link> parseEmojiRequestLink(std::string_view target);
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
    QString buildHtml(const uc::old::Info& info);
    QString buildEmptyFavsHtml();
    void appendStylesheet(QString& text, bool hasSignWriting = false);
    void go(QWidget* widget, TinyOpt<QRect> rect, Gui& gui, std::string_view link);
    void appendCopyable(QString& text, const QString& x, std::string_view clazz="copy");
    template <class Str>
    void appendCopyable(Str& text, unsigned x, std::string_view clazz="copy");
    /// @warning  Both toCopy and toView are raw HTML
    void appendCopyableHt(QString& text,
                         std::string_view toCopy,
                         std::u8string_view toView,
                         std::string_view clazz="copy");
    void appendCopyableHt(std::u8string& text,
                         std::string_view toCopy,
                         std::u8string_view toView,
                         std::string_view clazz="copy");
    void appendHtml(QString& text, const uc::Script& x, bool isScript);
    void appendNoFont(QString& x, std::u8string_view wiki, wiki::Mode mode);
    mywiki::AppendWiki append(QString& x, std::u8string_view wiki, const Context& context,
                        wiki::Mode mode);
    void appendVersionValue(QString& text, const uc::Version& version);
    void appendVersionValue(QString& text, uc::EcVersion version);
    void appendEmojiValue(QString& text, const uc::Version& version, const uc::Version& prevVersion);
    void appendVersion(QString& text, std::u8string_view prefix, const uc::Version& version);
    void appendUtf(QString& text, Want32 want32, str::QSep& sp, char32_t code);
    void appendUtf(QString& text, Want32 want32, str::QSep& sp, std::u32string_view value);
    void appendMissingCharInfo(QString& text, char32_t code);
    QString buildNonCharHtml(char32_t code);
    QString buildVacantCpHtml(char32_t code, const QColor& color);
    bool isEngTermShown(const uc::Term& term);

    void hackDocument(QTextDocument* doc);
    enum class NumPlace : unsigned char { RAW, HTML };
    QString toString(const uc::Numeric& numc, NumPlace place);

}   // namespace mywiki
