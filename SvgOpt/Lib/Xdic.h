#pragma once

#include <vector>
#include <optional>
#include <string>
#include <variant>
#include <limits>

#include "Xbit.h"
#undef XACT

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

        constexpr unsigned index() const { return static_cast<unsigned char>(value); }
        constexpr bool hasSmth() const { return (value != BAD); }
        std::string_view key() const noexcept;
        static DicId byKey(std::string_view key) noexcept;
        constexpr bool has(IdBit x) const noexcept { return (value & static_cast<unsigned>(x)); }
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

    struct IdInfo {
        std::string_view name;
        DicId fullId;
    };

#define DEFINE_DEFAULT_5(T) \
    T() = default;   \
        T(const T&) = default;  \
        T(T&&) = default;  \
        T& operator = (const T&) = default;  \
        T& operator = (T&&) = default;

    enum class CharType : unsigned char { BAN, NEXT, START };
    struct IdLink {
        std::string refId;

        bool operator == (const IdLink&) const noexcept = default;
        DEFINE_DEFAULT_5(IdLink)
        template <class T> explicit IdLink(T&& x)
            : refId(std::forward<T>(x)) {}
        static CharType charType(unsigned char x) noexcept;
        static CharType charType(char x) noexcept { return charType(static_cast<unsigned char>(x)); }
        static bool isId(std::string_view x) noexcept;
        static std::optional<IdLink> parse(std::string_view x);
        void encodeAttr(std::string& dest) const;
    };
    struct Inherit {
        bool operator == (const Inherit&) const noexcept { return true; }
    };

    using ValueVar = std::variant<Inherit, IdLink>;

    constexpr auto MAX_INDEX = static_cast<unsigned>(xsin::IdIndex::NN);

    constexpr size_t NO_INDEX = std::numeric_limits<size_t>::max();

    enum class Place { DELETED, ATTR, STYLE };
    struct Entry {
        DicId id;
        std::string key;
        Place place = Place::DELETED;
        ValueVar value;
        struct L {
            struct Pair {
                size_t next = NO_INDEX;  // actually a linked list
                size_t prev = NO_INDEX;
            } at, st;
        } l; // links
    };

    class Dic {
    public:
        Dic();
        [[nodiscard]] ValueVar& putAt(DicId id, Place place);
    private:
        static constexpr size_t II_FIRST = 0;
        static constexpr size_t II_LAST = 1;
        static constexpr size_t N_INITIAL = 2;
        std::vector<Entry> entries;
        size_t byId[MAX_INDEX];
        Entry& addEntry(DicId id, Place place);
        void unlinkEntry(size_t index);
        void linkEntry(size_t index, Place place);
    };
}
