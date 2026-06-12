#pragma once

#include <vector>
#include <optional>
#include <string>
#include <variant>
#include <limits>

#include "Xbit.h"
#undef XACT
#include "Xtypes.h"

namespace xsin {

    #define XACT(id, str, bit)  id,
    enum class IdIndex : unsigned char {
        #include "Xid.h"
        NN
    };
    // Simple check
    static_assert(static_cast<int>(IdIndex::NN) > 9);

}   // namespace xsin

namespace xs {

    template <class T>
    concept IsIdBit = std::same_as<std::decay_t<T>, IdBit>;

    class DicId {
    public:
        constexpr DicId() noexcept = default;

        template <IsIdBit... T>
        explicit consteval DicId(unsigned char aIndex, T... bits) noexcept
            : value((... | static_cast<unsigned>(bits)) | aIndex) {}

        constexpr unsigned index() const noexcept { return static_cast<unsigned char>(value); }
        constexpr bool hasSmth() const noexcept { return (value != BAD); }
        constexpr explicit operator bool() const noexcept { return hasSmth(); }
        std::string_view key() const noexcept;
        static DicId byKey(std::string_view key) noexcept;
        constexpr bool has(IdBit x) const noexcept { return (value & static_cast<unsigned>(x)); }
        constexpr bool operator == (const DicId& x) const noexcept = default;
    private:
        static constexpr unsigned BAD = std::numeric_limits<unsigned char>::max();
        unsigned value = BAD;
    };

}   // xs

namespace xid {

    #define XID_OF(id, bit) { static_cast<int>(xsin::IdIndex::id), bit }
    #define XACT(id, str, bit) \
        constexpr xs::DicId id XID_OF(id, bit);
    #include "Xid.h"

}   // namespace xid

namespace xs {

    constexpr auto MAX_INDEX = static_cast<unsigned>(xsin::IdIndex::NN);

    constexpr size_t NO_INDEX = std::numeric_limits<size_t>::max();

    enum class Place { DELETED, ATTR, STYLE };
    struct Kv {
        Kv() = default;
        explicit Kv(const DicId& aId) : id(aId) {}
        const DicId id;
        const std::string storedKey;
        std::string_view key() const
            { return id ? id.key() : storedKey; }
        ValueVar value;
    };
    struct Entry : public Kv {
        Place place = Place::DELETED;
        struct L {
            size_t next = NO_INDEX;  // actually a linked list
            size_t prev = NO_INDEX;
        } l; // linksl
        using Kv::Kv;
    };

    class Dic {
    public:
        Dic();
        [[nodiscard]] ValueVar& putAt(DicId id, Place place);

        template <class Body> void traverseAttr(const Body& body);
        template <class Body> void traverseStyle(const Body& body);
        template <class Body> void traverseAttr(const Body& body) const;
        template <class Body> void traverseStyle(const Body& body) const;

        /// @return [+] # of entries in attributes
        size_t countAttr() const noexcept;
        /// @return [+] # of entries in styles
        size_t countStyle() const noexcept;

        /// @return [+] has at least one entry in attributes
        bool hasAttr() const noexcept;
        /// @return [+] has at least one entry in styles
        bool hasStyle() const noexcept;
    private:
        static constexpr size_t II_FIRST_ATTR = 0;
        static constexpr size_t II_LAST_ATTR = 1;
        static constexpr size_t II_FIRST_STYLE = 2;
        static constexpr size_t II_LAST_STYLE = 3;
        static constexpr size_t N_INITIAL = 4;
        std::vector<Entry> entries;
        size_t byId[MAX_INDEX];
        Entry& addEntry(DicId id, Place place);
        void unlinkEntry(size_t index);
        void linkEntry(size_t index, Place place);
    };

}   // namespace xs

template <class Body>
void xs::Dic::traverseAttr(const Body& body)
{
    auto index = entries[II_FIRST_ATTR].l.next;
    while (index != II_LAST_ATTR) {
        auto& entry = entries[index];
        body(static_cast<Kv&>(entry));
        index = entry.l.next;
    }
}

template <class Body>
void xs::Dic::traverseAttr(const Body& body) const
{
    auto index = entries[II_FIRST_ATTR].l.next;
    while (index != II_LAST_ATTR) {
        auto& entry = entries[index];
        body(static_cast<const Kv&>(entry));
        index = entry.l.next;
    }
}

template <class Body>
void xs::Dic::traverseStyle(const Body& body)
{
    auto index = entries[II_FIRST_STYLE].l.next;
    while (index != II_LAST_STYLE) {
        auto& entry = entries[index];
        body(static_cast<Kv&>(entry));
        index = entry.l.next;
    }
}

template <class Body>
void xs::Dic::traverseStyle(const Body& body) const
{
    auto index = entries[II_FIRST_STYLE].l.next;
    while (index != II_LAST_STYLE) {
        auto& entry = entries[index];
        body(static_cast<const Kv&>(entry));
        index = entry.l.next;
    }
}
