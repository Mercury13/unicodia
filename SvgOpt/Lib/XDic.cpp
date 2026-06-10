#include "XDic.h"

// STL
#include <limits>
#include <stdexcept>

static_assert(std::numeric_limits<unsigned char>::max() == 255, "Strange machine");

// D = 1
static constexpr std::string_view K_DESC = "desc";
// F = 1
static constexpr std::string_view K_FILL = "fill";
// M = 1
static constexpr std::string_view K_METADATA = "metadata";
// S = 1
static constexpr std::string_view K_STYLE = "style";
// T = 1
static constexpr std::string_view K_TITLE = "title";

xs::IdInfo xs::idInfo[] {
    { .name = K_STYLE },
    { .name = K_DESC },
    { .name = K_METADATA },
    { .name = K_TITLE },
    { .name = K_FILL },
    // 5
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
        auto& prevLink = (entry.prevAttr == NO_INDEX)
                ? firstAttr : entries[entry.prevAttr].nextAttr;
        prevLink = entry.nextAttr;
        auto& nextLink = (entry.nextAttr == NO_INDEX)
                ? lastAttr : entries[entry.nextAttr].prevAttr;
        nextLink = entry.nextAttr;
    } else {
        auto& prevLink = (entry.prevStyle == NO_INDEX)
                ? firstStyle : entries[entry.prevStyle].nextStyle;
        prevLink = entry.nextStyle;
        auto& nextLink = (entry.nextStyle == NO_INDEX)
                ? lastStyle : entries[entry.nextStyle].prevStyle;
        nextLink = entry.nextStyle;
    }
}

void xs::Dic::linkEntry(size_t index, Place place)
{
    auto& entry = entries[index];
    entry.place = place;
    if (place == Place::ATTR) {
        if (lastAttr == NO_INDEX) {
            firstAttr = lastAttr = index;
        } else {
            entry.prevAttr = lastAttr;
            lastAttr = index;
        }
    } else {
        if (lastStyle == NO_INDEX) {
            firstStyle = lastStyle = index;
        } else {
            entry.prevStyle = lastStyle;
            lastStyle = index;
        }
    }
}
