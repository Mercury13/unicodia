#include "XDic.h"

// STL
#include <limits>
#include <stdexcept>

static_assert(std::numeric_limits<unsigned char>::max() == 255, "Strange machine");

// Simple unit-test-like
static_assert(xid::ACCUMULATE.index() == 0, "The 1st object");
static_assert(xid::ADDITIVE.index() == 1, "The 2nd object");
static_assert( xid::STOP_COLOR.has(xs::IdBit::STOP));
static_assert(!xid::STOP_COLOR.has(xs::IdBit::FILL));

namespace {

    struct IdInfo {
        std::string_view name;
        xs::DicId fullId;
    };

    #define XACT(id, str, bit) { str, xid::id },
    constexpr IdInfo CE_ALL[] {
        #include "Xid.h"
    };
    static_assert(std::size(CE_ALL) == xs::MAX_INDEX);

    consteval size_t checkAscending(const IdInfo (&x)[xs::MAX_INDEX]) {
        for (size_t i = 1; i < xs::MAX_INDEX; ++i) {
            auto& prev = x[i - 1];
            auto& curr = x[i];
            if (prev.name >= curr.name)
                return i;
        }
        return 0;
    }
    constexpr size_t ASC_TEST = checkAscending(CE_ALL);
    static_assert(ASC_TEST == 0);

}   // anon namespace

std::string_view xs::DicId::key() const noexcept
{
    auto ind = index();
    if (ind > MAX_INDEX)
        return {};
    return CE_ALL[ind].name;
}


xs::DicId xs::DicId::byKey(std::string_view key) noexcept
{
    /// @todo [urgent] Non-optimal version
    for (auto& v : CE_ALL) {
        if (v.name == key)
            return v.fullId;
    }
    return {};
}

xs::Dic::Dic()
{
    std::fill(std::begin(byId), std::end(byId), NO_INDEX);
    entries.resize(N_INITIAL);
    // Attribute
    auto& firstAttr = entries[II_FIRST_ATTR];
    firstAttr.l.prev = II_FIRST_ATTR;
    firstAttr.l.next = II_LAST_ATTR;
    auto& lastAttr = entries[II_LAST_ATTR];
    lastAttr.l = firstAttr.l;
    // Style
    auto& firstStyle = entries[II_FIRST_STYLE];
    firstStyle.l.prev = II_FIRST_STYLE;
    firstStyle.l.next = II_LAST_STYLE;
    auto& lastStyle = entries[II_LAST_STYLE];
    lastStyle.l = firstStyle.l;
}


xs::ValueVar& xs::Dic::putIndexAt(size_t internalIndex, Place place)
{
    auto& entry = entries[internalIndex];
    if (entry.place != place) {
        unlinkEntry(internalIndex);
        linkEntry(internalIndex, place);
    }
    return entry.value;
}


xs::ValueVar& xs::Dic::putAt(DicId id, Place place)
{
    if (place == Place::DELETED)
        throw std::invalid_argument("[putAt] Cannot use Place::DELETED");
    auto index = id.index();
    if (index >= MAX_INDEX)
        throw std::invalid_argument("[putAt] Index too big");
    auto& internalIndex = byId[index];
    if (internalIndex == NO_INDEX) {
        auto& entry = addEntry(id, place);
        return entry.value;
    } else {
        return putIndexAt(internalIndex, place);
    }
}

[[nodiscard]] xs::ValueVar& xs::Dic::putAt(std::string_view key, Place place)
{
    if (place == Place::DELETED)
        throw std::invalid_argument("[putAt] Cannot use Place::DELETED");
    if (auto id = DicId::byKey(key)) {
        return putAt(id, place);
    }
    // Probably OK - put-misses must be rare
    auto [it,_] = byUnknown.try_emplace(std::string{key});
    auto internalIndex = it->second.v;
    if (internalIndex == NO_INDEX) {
        auto& entry = addEntry(*it, place);
        return entry.value;
    } else {
        return putIndexAt(internalIndex, place);
    }
}

xs::Entry& xs::Dic::addEntry(DicId id, Place place)
{
    auto internalIndex = entries.size();
    auto& entry = entries.emplace_back(id);
    byId[id.index()] = internalIndex;
    linkEntry(internalIndex, place);
    return entry;
}

xs::Entry& xs::Dic::addEntry(UM::value_type& um, Place place)
{
    auto internalIndex = entries.size();
    auto& entry = entries.emplace_back(um.first);
    um.second.v = internalIndex;
    linkEntry(internalIndex, place);
    return entry;
}

void xs::Dic::unlinkEntry(size_t index)
{
    auto& entry = entries[index];
    entries[entry.l.prev].l.next = entry.l.next;
    entries[entry.l.next].l.prev = entry.l.prev;
}

void xs::Dic::linkEntry(size_t index, Place place)
{
    auto& entry = entries[index];
    entry.place = place;
    auto lastIndex = (place == Place::ATTR) ? II_LAST_ATTR : II_LAST_STYLE;
    auto& last = entries[lastIndex];
    auto& prev = entries[last.l.prev];
    entry.l.prev = last.l.prev;
    entry.l.next = lastIndex;
    last.l.prev = index;
    prev.l.next = index;
}

size_t xs::Dic::countAttr() const noexcept
{
    size_t r = 0;
    traverseAttr([&r](auto&) { ++r; });
    return r;
}

size_t xs::Dic::countStyle() const noexcept
{
    size_t r = 0;
    traverseStyle([&r](auto&) { ++r; });
    return r;
}

bool xs::Dic::hasAttr() const noexcept
{
    auto& entry = entries[II_FIRST_ATTR];
    return (entry.l.next != II_LAST_ATTR);
}

bool xs::Dic::hasStyle() const noexcept
{
    auto& entry = entries[II_FIRST_STYLE];
    return (entry.l.next != II_LAST_STYLE);
}
