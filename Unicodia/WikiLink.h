#pragma once

// Qt
#include <memory>
#include <optional>

// Libs
#include "u_Vector.h"
#include "u_TinyOpt.h"

class QWidget;
class QRect;

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

    class Link;
    using PLink = std::shared_ptr<const Link>;

    class Link  // interface
    {
    public:
        virtual void go(
                QWidget* widget, TinyOpt<QRect> rect,
                const PLink& that, Gui& gui) const = 0;
        virtual LinkClass clazz() const noexcept { return LinkClass::POPUP; }
        virtual ~Link() = default;
    };

    /// Universal ID of object, same objects — same ID,
    /// different objects — different IDs.
    /// You may extend it to some std::variant
    using HistoryObj = const void*;

    class HistoryLink : public Link
    {
    public:
        LinkClass clazz() const noexcept final { return LinkClass::POPUP; }

        /// @return  Some object associated with a link
        virtual HistoryObj obj() const noexcept = 0;

        /// @return  [+] links point to the same object
        ///     May be virtualized (and obj() moved to a separate ObjLink)
        ///     if that obj() is no longer a universal means to distinguish
        ///     links
        bool isSame(const HistoryLink& that) const noexcept
            { return (obj() == that.obj()); }

        bool isDifferent(const HistoryLink& that) const noexcept
            { return !isSame(that); }

        /// @return  Localized header of linked object
        /// @warning  As it creates a string, is NOT noexcept
        virtual std::u8string header() const = 0;
    };
    using PHistoryLink = std::shared_ptr<const HistoryLink>;

    struct HistoryPlace {
        PHistoryLink thing;
        unsigned index = std::numeric_limits<unsigned>::max();

        explicit operator bool() const noexcept { return static_cast<bool>(thing); }
    };

    class History
    {
    public:
        static constexpr unsigned MAX_SIZE = 5;
        using Vec = SafeVector<PHistoryLink>;

        unsigned size() const noexcept { return links.size(); }

        /// Trims history to MAX_SIZE
        void trimTo(size_t wantedSize);
        void trim() { trimTo(MAX_SIZE); }
        void trim1() { trimTo(MAX_SIZE - 1); }
        void push(const PHistoryLink& link);
        HistoryPlace back(TinyOpt<HistoryLink> me) const noexcept;
        PHistoryLink extract(unsigned index);
    private:
        Vec links;
        std::optional<Vec::iterator> findIt(const HistoryLink& x) noexcept;
    };
}

extern template class std::shared_ptr<const mywiki::Link>;
