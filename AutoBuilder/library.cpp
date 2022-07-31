#include "library.h"

// STL
#include <stack>
#include <fstream>

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
constexpr char32_t VS16 = 0xFE0F;


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
                // Only fully-qualified emoji are added
                auto hexCodes = str::splitSv(spannedCodes, ' ');
                auto nCodes = hexCodes.size();
                for (size_t i = 0; i < nCodes; ++i) {
                    codes[i] = fromHex(hexCodes[i]);
                }
                // #### + VS16 → that code requires VS16
                if (nCodes == 2 && codes[1] == VS16) {
                    r.vs16.insert(codes[0]);
                }
                // Add to tree
                auto [text, emVersion, name] = splitLineSv(comment, ' ', ' ');
                auto& newItem = treePath.top()->children.emplace_back();
                newItem.name = str::toU8sv(name);
                newItem.value.assign(codes.buffer(), nCodes);
            }
        }
    }
    return r;
}


namespace {

    void recurseEnum(const lib::Node& node, int& counter)
    {
        for (auto& v : node.children) {
            v.cache.index = counter++;
        }
        for (auto& v : node.children) {
            recurseEnum(v, counter);
        }
    }

    int enumNodes(const lib::Node& root)
    {
        int counter = 0;
        root.cache.index = counter++;
        recurseEnum(root, counter);
        return counter;
    }

}   // anon namespace


lib::Result lib::write(const Node& root, const char* fname)
{
    lib::Result r;
    r.nNodes = enumNodes(root);

    std::ofstream os(fname);

    os << "// Automatically generated, do not edit!" << '\n';
    os << '\n';
    os << R"(#include "UcAutoDefines.h")" << '\n';
    os << '\n';
    os << R"(using namespace std::string_view_literals;)" << '\n';
    os << '\n';

    return r;
}
