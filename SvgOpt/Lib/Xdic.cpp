#include "XDic.h"

// STL
#include <limits>
#include <stdexcept>

static_assert(std::numeric_limits<unsigned char>::max() == 255, "Strange machine");

static_assert(xid::CLIP_PATH.index() == 0, "The 1st object");
static_assert(xid::CLIP_RULE.index() == 1, "The 2nd object");
static_assert( xid::STOP_COLOR.has(xs::IdBit::STOP));
static_assert(!xid::STOP_COLOR.has(xs::IdBit::FILL));

namespace {

    #define XACT(id, str, bit) { str, xid::id },
    constexpr xs::IdInfo CE_ALL[] {
        #include "Xid.h"
    };
    static_assert(std::size(CE_ALL) == xs::MAX_INDEX);

    consteval size_t checkAscending(const xs::IdInfo (&x)[xs::MAX_INDEX]) {
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

}

std::string_view xs::DicId::key() const noexcept
{
    auto ind = index();
    if (ind > MAX_INDEX)
        return {};
    return CE_ALL[ind].name;
}

xs::Dic::Dic()
{
    std::fill(std::begin(byId), std::end(byId), NO_INDEX);
    entries.resize(N_INITIAL);
    auto& first = entries[II_FIRST];
    first.l.at.prev = II_FIRST;
    first.l.at.next = II_LAST;
    first.l.st = first.l.at;
    auto& last = entries[II_LAST];
    last.l = first.l;
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
        auto& entry = entries[internalIndex];
        if (entry.place != place) {
            unlinkEntry(internalIndex);
            linkEntry(internalIndex, place);
        }
        return entry.value;
    }
}

xs::Entry& xs::Dic::addEntry(DicId id, Place place)
{
    auto internalIndex = entries.size();
    auto& entry = entries.emplace_back();
    entry.id = id;
    entry.key = id.key();
    linkEntry(internalIndex, place);
    return entry;
}

void xs::Dic::unlinkEntry(size_t index)
{
    auto& entry = entries[index];
    if (entry.place == Place::ATTR) {
        entries[entry.l.at.prev].l.at.next = entry.l.at.next;
        entries[entry.l.at.next].l.at.prev = entry.l.at.prev;
    } else {
        entries[entry.l.st.prev].l.st.next = entry.l.st.next;
        entries[entry.l.st.next].l.st.prev = entry.l.st.prev;
    }
}

void xs::Dic::linkEntry(size_t index, Place place)
{
    auto& entry = entries[index];
    entry.place = place;
    auto& last = entries[II_LAST];
    if (place == Place::ATTR) {
        entry.l.at.prev = last.l.at.prev;
        last.l.at.prev = index;
    } else {
        entry.l.st.prev = last.l.st.prev;
        last.l.st.prev = index;
    }
}
