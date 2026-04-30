#include "FontBase.h"

const fb::Info fb::Info::EMPTY {};

namespace {

    struct MyPair {
        std::string_view k;
        fb::Info v;
    };
    inline bool operator < (const MyPair& x, std::string_view y) { return (x.k < y); }

    const MyPair data_S[] {
        {   .k = "Sans Serif Collection",
            .v { .flags = fb::Fg::SMALL_CELL }
        }
    };

    const fb::Info* getLetter1(
            std::string_view x,
            const MyPair* beg, const MyPair* end)
    {
        auto where = std::lower_bound(beg, end, x);
        if (where != end && where->k == x)
            return &where->v;
        return nullptr;
    }

    template <size_t N>
    inline const fb::Info* getLetter(
            std::string_view x,
            const MyPair (&data)[N])
        { return getLetter1(x, data, data + N); }
}

const fb::Info* fb::getOrNull(std::string_view name)
{
    // Empty: no result
    if (name.empty())
        return nullptr;

    // Font names
    switch (name[0]) {
    case 'S':
        return getLetter(name, data_S);
    default: ;
    }
    return nullptr;
}

const fb::Info* fb::getOrEmpty(std::string_view name)
{
    if (auto q = getOrNull(name))
        return q;
    return &Info::EMPTY;
}
