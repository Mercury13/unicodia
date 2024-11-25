#pragma once

#include <string_view>
#include <string>
#include <map>
#include <unordered_map>
#include <stdexcept>

///
/// @throw  if bad conversion
///
unsigned fromHex(std::string_view x);

int fromDec(std::string_view x);

bool fromHexIf(std::string_view x, unsigned& y);

///
/// Encodes a string to C string format
///
std::string encodeC(std::string_view x);


namespace ucd {

    struct Range {
        char32_t first, last;
        auto operator <=> (const Range& x) const { return first <=> x.first; }
        auto operator <=> (char32_t x) const { return first <=> x; }
        bool contains(char32_t x) const { return first <= x && x <= last; }
        static Range from(std::string_view x);
    };

    struct RangeCmp : public std::less<Range> {
        using is_transparent = void;
        using std::less<Range>::operator();
        bool operator () (const Range& x, char32_t y) const { return (x.first < y); }
        bool operator () (char32_t x, const Range& y) const { return (x < y.first); }
    };

    struct Cmp {
        using is_transparent = void;
        inline bool operator() (std::string_view x, std::string_view y) const noexcept
            { return (x < y); }
    };

    struct Hash : public std::hash<std::string_view> {
        using is_transparent = void;
    };

    class PropBase
    {
    public:
        size_t nScripts() const { return scriptsLongToShort.size(); }
        std::string_view shortenScript(std::string_view text) const;

        void addScript(std::string_view longv, std::string_view shortv);

        using MStoS = std::map<std::string, std::string>;
        MStoS buildShortToLong() const;
    private:
        /// Strange, but need std::equal_to!
        using UStoS = std::unordered_map<std::string, std::string, Hash, std::equal_to<>>;
        /// key = long, value = short
        UStoS scriptsLongToShort;
    };

    PropBase loadPropBase();

    template <class Value>
    class RangeMap
    {
    public:
        template <class... Args>
        void add(const Range& pair, Args&& ... args)
        {
            if constexpr (sizeof...(args) == 0) {
                m.insert_or_assign(pair, Value{} );
            } else {
                m.insert_or_assign( pair, std::forward<Args>(args)...);
            }
        }

        template <class... Args>
        void add(char32_t first, char32_t last, Args&& ... args)
            { add(Range{first, last}, std::forward<Args>(args)...); }

        const Value* find(char32_t key) const;
        bool contains(char32_t key) const { return find(key); }

        /// Finds key, or returns default value
        /// @warning  Leaves reference
        const Value& findDefRef(char32_t key, const Value& def) const;

        const Value& findRq(char32_t key) const;

        size_t size() const { return m.size(); }
    private:
        std::map<Range, Value, RangeCmp> m;
    };

}   // namespace ucd


template <class Value>
const Value* ucd::RangeMap<Value>::find(char32_t key) const
{
    auto it = m.lower_bound(key + 1);
    if (it == m.begin())
        return nullptr;
    --it;
    if (!it->first.contains(key))
        return nullptr;
    return &it->second;
}


template <class Value>
const Value& ucd::RangeMap<Value>::findDefRef(char32_t key, const Value& def) const
{
    auto q = find(key);
    return q ? *q : def;
}


template <class Value>
const Value& ucd::RangeMap<Value>::findRq(char32_t key) const
{
    auto q = find(key);
    if (!q) [[unlikely]] {
        char buf[80];
        snprintf(buf, std::size(buf), "[RangeMap.findRq] Cannot find char %04X", (int)key);
        throw std::logic_error(buf);
    }
    return *q;
}
