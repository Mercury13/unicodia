#pragma once

#include <vector>
#include <optional>
#include <string>
#include <variant>
#include <limits>

namespace xs
{
    enum class IdBit : unsigned int {
        FILL = 1 << 8,          ///< Useful where there is fill
        STROKE = 1 << 9,        ///< Useful where there is stroke
        TEXT = 1 << 10,         ///< Useful wher ethere is text
        META = 1 << 11,         ///< Useless, just metadata
        ATTR_ONLY = 1 << 28,    ///< Only attribute, forbidden in style
        SPECIAL = 1 << 29,      ///< Special cases
        FREE_STRING = 1 << 30,  ///< The type is free unparsed string
        COMBO_META = META | ATTR_ONLY | FREE_STRING,
    };

    template <class T>
    concept IsIdBit = std::same_as<std::decay_t<T>, IdBit>;

    class DicId {
    public:
        constexpr DicId() noexcept = default;

        template <IsIdBit... T>
        explicit constexpr DicId(unsigned char aIndex, T... bits) noexcept
            : value((... | static_cast<unsigned>(bits)) | aIndex) {}

        constexpr unsigned index() const { return static_cast<unsigned char>(value); }
        constexpr bool hasSmth() const { return (value != BAD); }
        std::string_view key() const noexcept;
    private:
        static constexpr unsigned BAD = std::numeric_limits<unsigned char>::max();
        unsigned value = BAD;
    };

    struct IdInfo {
        std::string_view name;
    };

    constexpr DicId ID_STYLE { 0, IdBit::SPECIAL };
    constexpr DicId ID_DESC { 1, IdBit::COMBO_META };
    constexpr DicId ID_METADATA { 2, IdBit::COMBO_META };
    constexpr DicId ID_TITLE { 3, IdBit::COMBO_META };
    constexpr DicId ID_FILL { 4, IdBit::FILL };
    constexpr unsigned char MAX_INDEX = 5;
    extern IdInfo idInfo[];

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
