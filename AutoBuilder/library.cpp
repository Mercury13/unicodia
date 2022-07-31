#include "library.h"

// STL
#include <stack>

// Libs
#include "u_Strings.h"

using namespace std::string_view_literals;


unsigned fromHex(std::string_view x)
{
    long long r;
    auto beg = x.data();
    auto end = beg + x.length();
    auto res = std::from_chars(beg, end, r, 16);
    if (res.ec != std::errc() || res.ptr != end) {
        std::string errm = "[fromChars] Cannot parse hex ";
        errm.append(x);
        throw std::invalid_argument(errm);
    }
    return r;
}


namespace {

    template<typename T, unsigned N, typename... REST>
    struct TupleN
    {
        using Type = typename TupleN<T, N-1, T, REST...>::Type;
    };

    template<typename T, typename... REST>
    struct TupleN<T, 0, REST...>
    {
        using Type = std::tuple<REST...>;
    };

    inline std::tuple<std::string_view> splitLineSv(std::string_view s)
        { return {s}; }

    template <class... Args>
    typename TupleN<std::string_view, sizeof...(Args) + 2>::Type splitLineSv(
            std::string_view s, char sep1, Args... restSeps)
    {
        s = str::trimLeftSv(s);
        auto p = s.find(sep1);
        if (p == std::string_view::npos) {
            return std::tuple_cat(
                std::make_tuple(s),
                typename TupleN<std::string_view, sizeof...(Args) + 1>::Type{});
        }

        auto rest = s.substr(p + 1);
        return std::tuple_cat(
            std::make_tuple(str::trimRightSv(s.substr(0, p))),
            splitLineSv(rest, restSeps...));
    }

    lib::Node* crop(std::stack<lib::Node*>& x, size_t size)
    {
        if (x.size() < size)
            throw std::logic_error("Bad stack size");
        while(x.size() > size)
            x.pop();
        return x.top();
    }

    void addNew(std::stack<lib::Node*>& x, size_t size, std::string_view name)
    {
        auto top = crop(x, size);
        auto& newItem = top->children.emplace_back();
        newItem.name = str::toU8sv(name);
        x.push(&newItem);
    }

}   // anon namespace


constexpr std::string_view S_GROUP = "group: ";
constexpr std::string_view S_SUBGROUP = "subgroup: ";



lib::EmojiData lib::loadEmoji(const char* fname)
{
    std::ifstream is(fname);
    if (!is.is_open())
        throw std::logic_error(std::string("Cannot open ") + fname);

    Fix1d<char32_t, 20> codes;

    EmojiData r;
    r.root.name = u8"Emoji";

    std::stack<Node*> treePath;
    treePath.push(&r.root);

    std::string s;
    while (std::getline(is, s)) {
        std::string_view mainLine{str::trimSv(s) };
        if (mainLine.empty())
            continue;

        if (mainLine.starts_with('#')) {
            // Comment
            s = str::trimSv(s.substr(1));
            if (auto q = str::remainderSv(s, S_GROUP); !q.empty()) {
                // Group
                addNew(treePath, 1, q);
            } if (auto q = str::remainderSv(s, S_SUBGROUP); !q.empty()) {
                // Subgroup
                addNew(treePath, 2, q);
            }
        } else {
            // Normal line
            auto [spannedCodes, qualType, comment] = splitLineSv(mainLine, ';', '#');
            if (qualType == "fully-qualified"sv) {
                auto hexCodes = str::splitSv(spannedCodes, ' ');
                auto nCodes = hexCodes.size();
                for (size_t i = 0; i < nCodes; ++i) {
                    codes[i] = fromHex(hexCodes[i]);
                }
                if (nCodes == 2 && codes[1] == 0xFE0F) {
                    r.vs16.insert(codes[0]);
                }
            }
        }
    }
    return r;
}
