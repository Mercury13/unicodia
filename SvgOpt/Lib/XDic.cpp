#include "XDic.h"

// STL
#include <limits>
#include <stdexcept>

static_assert(std::numeric_limits<unsigned char>::max() == 255, "Strange machine");

// C = 1
constexpr std::string_view K_CLIP_PATH = "clip-path";
// D = 1
constexpr std::string_view K_DESC = "desc";
// F = 3
constexpr std::string_view K_FILL = "fill";
constexpr std::string_view K_FILL_OPACITY = "fill-opacity";
constexpr std::string_view K_FILL_RULE = "fill-rule";
// M = 1
constexpr std::string_view K_METADATA = "metadata";
// S = 1
constexpr std::string_view K_STOP_COLOR = "stop-color";
constexpr std::string_view K_STOP_OPACITY = "stop-opacity";
constexpr std::string_view K_STYLE = "style";
// T = 1
static constexpr std::string_view K_TITLE = "title";

constinit xs::IdInfo xs::idInfo[] {
    { .name = K_STYLE },
    { .name = K_DESC },
    { .name = K_METADATA },
    { .name = K_TITLE },
    { .name = K_FILL },
    // 5
    { .name = K_CLIP_PATH },
    { .name = K_FILL_OPACITY },
    { .name = K_FILL_RULE },
    { .name = K_STOP_COLOR },
    { .name = K_STOP_OPACITY },
    // 10
};

static_assert(std::size(xs::idInfo) == xs::MAX_INDEX);

std::string_view xs::DicId::key() const noexcept
{
    auto ind = index();
    if (ind > MAX_INDEX)
        return {};
    return idInfo[ind].name;
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
