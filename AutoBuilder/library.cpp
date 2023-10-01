#include "library.h"

// STL
#include <stack>
#include <fstream>

// XML
#include "pugixml.hpp"

// Libs
#include "u_Strings.h"
#include "mojibake.h"

// Project-local
#include "data.h"
#include "utils.h"

// Unicode
#include "UcCp.h"

using namespace std::string_view_literals;


unsigned lib::Node::maxValueLength() const
{
    unsigned r = value.length();
    for (auto& v : children) {
        r = std::max(r, v.maxValueLength());
    }
    return r;
}



namespace {

    /// Library items that do not make tiles
    const std::unordered_set<std::u8string_view> NO_TILE {
        u8"Component",  // Really ugly, scrap for the sake of nicer ones
        u8"backhand index pointing up",     // Scrapped for the sake of “placard with finger”
        u8"backhand index pointing down",   // Same
        u8"index pointing up",              // Same
        u8"animal-amphibian",   // Big library, scrap for the sake of spouting whale
    };

    const std::unordered_set<std::u32string_view> MISRENDERS {
        U"\U0001F1E6\U0001F1EB",    // Afghanistan
        U"\U0001F1F2\U0001F1F6",    // Martinique
        U"\U0001F46A",              // family
        U"\U0001F68F",              // bus stop
        U"\U0001F6DE",              // wheel
        U"\U0001FA85",              // piñata
        U"\U0001FA92",              // razor
        U"\U0001FAA4",              // mouse trap
    };

    constexpr const char32_t UNSEARCHABLE_EMOJI_C[] {
        cp::SKIN1, cp::SKIN2, cp::SKIN3, cp::SKIN4, cp::SKIN5, cp::RIGHT_ARROW, 0 };
    constexpr const std::u32string_view UNSEARCHABLE_EMOJI (UNSEARCHABLE_EMOJI_C);

    enum class SearchLevel {
        HIDDEN, DECODEABLE, SEARCHABLE
    };

    SearchLevel searchLevel(std::u32string_view emoji)
    {
        // 1-character → HIDDEN
        if (emoji.length() <= 1)
            return SearchLevel::HIDDEN;
        // VS16 — decodeable
        if (emoji.length() == 2 && emoji[1] == cp::VS16)
            return SearchLevel::DECODEABLE;
        // The rest are at least decodeable.
        // Unsearchable are: skins, directions
        auto index = emoji.find_first_of(UNSEARCHABLE_EMOJI);
        return (index == std::u32string_view::npos)
                ? SearchLevel::SEARCHABLE : SearchLevel::DECODEABLE;
    }

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
        newItem.flags |= uc::Lfg::TRANSLATE;
        if (NO_TILE.contains(newItem.name))
            newItem.flags |= uc::Lfg::NO_TILE;
        x.push(&newItem);
    }

}   // anon namespace


constexpr std::string_view S_GROUP = "group: ";
constexpr std::string_view S_SUBGROUP = "subgroup: ";


lib::EmojiData lib::loadEmoji(const char* fname)
{
    std::ifstream is(fname);
    if (!is.is_open())
        throw std::logic_error(str::cat("Cannot open ", fname));

    Fix1d<char32_t, 20> codes;

    EmojiData r;
    r.root.name = u8"Emoji";
    r.root.flags |= uc::Lfg::TRANSLATE;

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
            if (qualType == "fully-qualified"sv
                    || qualType == "component"sv) {
                // Only fully-qualified emoji are added
                auto hexCodes = str::splitSv(spannedCodes, ' ');
                auto nCodes = hexCodes.size();
                for (size_t i = 0; i < nCodes; ++i) {
                    codes[i] = fromHex(hexCodes[i]);
                }
                // #### + VS16 → that code requires VS16
                char32_t mainCode = (nCodes == 1) ? codes[0] : 0;
                if (nCodes == 2 && codes[1] == cp::VS16) {
                    r.vs16.insert(codes[0]);
                    mainCode = codes[0];
                }
                // Add to tree
                auto [text, emVersion, name] = splitLineSv(comment, ' ', ' ');
                auto& newItem = treePath.top()->children.emplace_back();
                if (lat::hasUpper(name)) { // has uppercase letter → pre-decapped
                    newItem.name = str::toU8sv(name);
                } else { // otherwise decapitalize
                    newItem.name = str::toU8sv(decapitalizeEmoji(name));
                }
                newItem.emojiVersion = emVersion;
                newItem.flags = uc::Lfg::GRAPHIC_EMOJI;
                if (NO_TILE.contains(newItem.name))
                    newItem.flags |= uc::Lfg::NO_TILE;                
                newItem.value.assign(codes.buffer(), nCodes);
                if (MISRENDERS.contains(newItem.value)) {
                    newItem.flags |= uc::Lfg::MISRENDER;
                    if (mainCode != 0)
                        r.misrenders.insert(mainCode);
                }
                auto level = searchLevel(newItem.value);
                switch (level) {
                case SearchLevel::SEARCHABLE:
                    newItem.flags |= (uc::Lfg::SEARCHABLE | uc::Lfg::DECODEABLE);
                    break;
                case SearchLevel::DECODEABLE:
                    newItem.flags |= uc::Lfg::DECODEABLE;
                    break;
                case SearchLevel::HIDDEN: ;
                }
                ++r.count;
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

    void writeU32sv(std::ostream& os, std::u32string_view s)
    {
        if (s.empty()) {
            os << "{}";
        } else {
            os << "U\"";
            char buf[16];
            for (auto c : s) {
                if (c <= 0xFFFF) {
                    snprintf(buf, std::size(buf), "\\u%04X", static_cast<unsigned>(c));
                } else {
                    snprintf(buf, std::size(buf), "\\U%08X", static_cast<unsigned>(c));
                }
                os << buf;
            }
            os << '"';
        }
    }

    void writeU8sv(std::ostream& os, std::u8string_view s)
    {
        if (s.empty()) {
            os << "{}";
        } else {
            os << "u8\"" << str::toSv(s) << '"';
        }
    }

    constexpr int ROOTS_PARENT = 0;     // index of root’s parent

    void writeNode(std::ostream& os, const lib::Node& node, int iParent)
    {
        // opening brace
        os << "{ ";
        // value
        writeU32sv(os, node.value);
        os << ", ";
        // text
        writeU8sv(os, node.name);
        os << ", ";
        // parent
        os << iParent << ", ";
        // childred.count
        os << node.children.size() << ", ";
        // children.index
        if (node.children.empty()) {
            os << "-1";
        } else {
            os << node.children[0].cache.index;
        }
        os << ", ";
        // flags
        if (node.flags.empty()) {
            os << "{}";
        } else {
            os << "Lfg(" << static_cast<int>(node.flags.numeric()) << ")";
        }
        os << ", "
        // version
              "EcVersion::";
        if (node.emojiVersion.empty()) {
            os << "NONE";
        } else {
            std::string newVer = node.emojiVersion;
            str::replace(newVer, '.', '_');
            os << newVer;
        }
        // closing brace
        os << " },   // " << node.cache.index << "\n";
    }

    void recurseWrite(
            std::ostream& os,
            const lib::Node& node)
    {
        for (auto& v : node.children) {
            writeNode(os, v, node.cache.index);
        }
        for (auto& v : node.children) {
            recurseWrite(os, v);
        }
    }

}   // anon namespace


lib::Result lib::write(const Node& root, const char* fname)
{
    lib::Result r;
    r.nNodes = enumNodes(root);

    std::ofstream os(fname);

    os << "// Automatically generated, do not edit!"  "\n";
    os << '\n';
    os << R"(#include "UcAutoDefines.h")"  "\n";
    os << '\n';

    os << "constinit const uc::LibNode uc::libNodes[" << r.nNodes << "] {"  "\n";
    writeNode(os, root, ROOTS_PARENT);
    recurseWrite(os, root);
    os << "};"  "\n"
          "size_t uc::nLibNodes() { return std::size(libNodes); }" "\n"
          "std::span<const uc::LibNode> uc::allLibNodes() { return libNodes; }" "\n";

    return r;
}


///// StrangeCjk ///////////////////////////////////////////////////////////////


namespace {

    enum class StrangeTarget { NONE, AFTER };


    struct StrangeCatInfo {
        char key;
        StrangeTarget target = StrangeTarget::NONE;
        uc::Lfgs targetFlags {};
    };


    constinit const StrangeCatInfo strangeCats[] = {
        { 'A' },    // Asymmetric
        { 'B', StrangeTarget::AFTER, uc::Lfg::NO_TILE },  // Bopomofo-like
        { 'C' },    // Cursive
        { 'F', StrangeTarget::AFTER, uc::Lfg::CODE_AS_NAME | uc::Lfg::NO_TILE }, // Fully-reflective
        { 'H' },    // Contain Hangul
        { 'I' },    // Look incomplete
        { 'K', StrangeTarget::AFTER, uc::Lfg::NO_TILE },    // Katakana-like
        { 'M', StrangeTarget::AFTER, uc::Lfg::CODE_AS_NAME | uc::Lfg::NO_TILE }, // Mirrored
        { 'O' },    // Odd component
        { 'R', StrangeTarget::AFTER, uc::Lfg::CODE_AS_NAME | uc::Lfg::NO_TILE },    // Rotated
        { 'S' },    // Stroke-heavy
        { 'U' },    // Unusual structure
    };

}   // anon namespace

lib::StrangeCjk::StrangeCjk()
{
    root.name = u8"Strange";
    root.flags |= uc::Lfg::TRANSLATE;
    for (auto& v : strangeCats) {
        auto& cat = root.children.emplace_back();
        cat.name.assign(1, v.key);
        cat.flags |= uc::Lfg::TRANSLATE;
    }
}


void lib::StrangeCjk::processCp(char32_t cp, std::string_view sStrange)
{
    auto q = str::splitSv(sStrange, ' ');
    for (auto& v : q) {
        auto type = v[0];
        auto whatFound = std::lower_bound(
                    std::begin(strangeCats), std::end(strangeCats), type,
                    [](const StrangeCatInfo& x, char y) {
                        return (x.key < y);
                    });
        if (whatFound == std::end(strangeCats) || whatFound->key != type)
            throw std::logic_error("Unknown strange category");
        auto iCat = whatFound - std::begin(strangeCats);
        auto& subcat = root.children[iCat];
        auto& child = subcat.children.emplace_back();
        child.value.assign(1, cp);
        child.flags |= uc::Lfg::CODE_AS_NAME;

        if (whatFound->target == StrangeTarget::AFTER) {
            auto cpNames = str::splitSv(v.substr(1), ':');
            auto& parent = (cpNames.size() > 1)
                    ? child : subcat;
            for (auto cpName : cpNames) {
                if (cpName.starts_with("U+")) {
                    auto sCode = cpName.substr(2);
                    auto code = fromHex(sCode);
                    auto& child2 = parent.children.emplace_back();
                    child2.value.assign(1, code);
                    child2.flags = whatFound->targetFlags;
                }
            }
        }
    }
}


namespace {

    void loadName(lib::Node& result, pugi::xml_node tag)
    {
        if (tag.attribute("hasText").as_bool())
            result.flags |= uc::Lfg::HAS_TEXT;
        if (!tag.attribute("tile").as_bool(true))
            result.flags |= uc::Lfg::NO_TILE;
        result.name = str::toU8(tag.attribute("loc").as_string());
        if (!result.name.empty()) {
            result.flags |= uc::Lfg::TRANSLATE;
            return;
        }
        result.name = str::toU8(tag.attribute("name").as_string());
        if (result.name.empty())
            throw std::logic_error("Need sequence/folder name");
    }

    void conditionalBan(lib::Node& node,
                        std::string_view tilePattern, unsigned& indexInPattern)
    {
        if (!tilePattern.empty()) {
            auto i = indexInPattern % tilePattern.length();
            switch (tilePattern[i]) {
            case '0':
            case '_':
            case '.':
            case '-':
                node.flags |= uc::Lfg::NO_TILE;
                break;
            default:;
            }
        }
        ++indexInPattern;
    }

    void appendDump(lib::Node& result, forget::Map& forgetMap,
                    pugi::xml_node tag, bool isForgetTest,
                    std::string_view tilePattern, unsigned& indexInPattern)
    {
        std::string_view text8 = tag.text().get();
        auto text32 = mojibake::toS<std::u32string>(text8);
        auto sub32 = str::splitSv(text32, ' ');
        result.children.reserve(result.children.size() + sub32.size());
        char32_t cap = '!';     // something odd
        for (auto v : sub32) {
            auto& node = result.children.emplace_back();
            node.value = v;
            conditionalBan(node, tilePattern, indexInPattern);
            if (v.length() == 1) {
                auto cp = v[0];
                if (isForgetTest) {
                    auto& w = forgetMap[cp];
                    if (w.lib.possibleCapital == 0)
                        w.lib.possibleCapital = cap;
                    ++w.lib.count;
                }
                cap = cp;
            } else {
                if (isForgetTest) {
                    throw std::logic_error("String of non-1 length under forget test");
                }
                cap = 0;
            }
        }
    }

    void appendRange(lib::Node& result, forget::Map& forgetMap,
                     pugi::xml_node tag, bool isForgetTest,
                     std::string_view tilePattern, unsigned& indexInPattern)
    {
        auto startCode = fromHex(tag.attribute("start").as_string());
        auto endCode = fromHex(tag.attribute("end").as_string());
        if (startCode >= endCode) {
            throw std::logic_error("Bad char codes");
        }
        auto nCodes = endCode - startCode + 1;
        result.children.reserve(result.children.size() + nCodes);
        for (auto i = startCode; i <= endCode; ++i) {
            char32_t cp = i;
            auto& node = result.children.emplace_back();
            node.value = std::u32string{ cp };
            conditionalBan(node, tilePattern, indexInPattern);
            if (isForgetTest) {
                auto& w = forgetMap[cp];
                ++w.lib.count;
            }
        }
    }

    void appendSequence(lib::Node& result, pugi::xml_node tag,
                        std::string_view tilePattern, unsigned& indexInPattern)
    {
        std::string_view value8 = tag.attribute("v").as_string();
        if (value8.empty())
            return;
        auto value32 = mojibake::toS<std::u32string>(value8);
        if (value32.empty())
            return;
        auto& node = result.children.emplace_back();
        loadName(node, tag);
        node.value = std::move(value32);
        conditionalBan(node, tilePattern, indexInPattern);
    }

    void loadRecurse(lib::Node& result, forget::Map& forgetMap,
                     pugi::xml_node tag, bool isForgetTest,
                     std::string_view tilePattern)
    {
        unsigned indexInPattern = 0;
        isForgetTest = tag.attribute("forgetTest").as_bool(isForgetTest);
        for (auto v : tag.children()) {
            if (v.name() == "f"sv) {
                // Folder
                auto& newFolder = result.children.emplace_back();
                loadName(newFolder, v);
                // As all tile patterns are taken from XML → they are null-term
                std::string_view newTilePattern = v.attribute("tilePattern").as_string(tilePattern.data());
                loadRecurse(newFolder, forgetMap, v, isForgetTest, newTilePattern);
            } else if (v.name() == "d"sv) {
                // Dump
                appendDump(result, forgetMap, v, isForgetTest, tilePattern, indexInPattern);
            } else if (v.name() == "range"sv) {
                appendRange(result, forgetMap, v, isForgetTest, tilePattern, indexInPattern);
            } else if (v.name() == "sq"sv) {
                appendSequence(result, v, tilePattern, indexInPattern);
            }
        }
    }

}   // anon namespace


lib::Manual lib::loadManual(const char* fname)
{
    pugi::xml_document doc;
    if (auto parseRes = doc.load_file(fname); !parseRes)
        throw std::logic_error(str::cat("Cannot open ", fname));

    lib::Manual r;

    auto root = doc.root().child("library");
    if (!root)
        throw std::logic_error("No root");

    // Do not use empty tilePattern, need working data()
    loadRecurse(r.root, r.forgetMap, root, false, ""sv);

    return r;
}
