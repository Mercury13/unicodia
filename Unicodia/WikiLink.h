#pragma once

// Qt
#include <QWidget>
#include <memory>

// Libs
#include "u_Vector.h"
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

        /// @return  Some object associated with a link
        virtual const void* obj() const = 0;

        /// @return  [+] links point to the same object
        ///     May be virtualized (and obj() moved to a separate ObjLink)
        ///     if that obj() is no longer a universal means to distinguish
        ///     links
        bool isSame(const HistoryLink& that) const
            { return (obj() == that.obj()); }

        /// @todo [urgent]  Must be purely virtual
        virtual std::u8string header() const { return u8"[History link]"; }
    };


    class History
    {
        static constexpr unsigned MAX_SIZE = 5;
        using PLink = std::shared_ptr<HistoryLink>;
        using Vec = SafeVector<PLink>;

        auto size() { return links.size(); }

        /// Trims history to MAX_SIZE
        void trimTo(size_t wantedSize);
        void trim() { trimTo(MAX_SIZE); }
        void trim1() { trimTo(MAX_SIZE - 1); }
        void push(const PLink& link);
    private:
        Vec links;
        std::optional<Vec::iterator> findIt(const HistoryLink& x);
    };
}
