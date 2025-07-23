#include "library.h"

// STL
#include <stack>
#include <fstream>
#include <array>

// XML
#include "pugixml.hpp"

// Libs
#include "u_Strings.h"
#include "mojibake.h"

// Project-local
#include "decap/decap.h"
#include "ucdcom.h"

// Unicode
#include "UcCp.h"

using namespace std::string_view_literals;


lib::Node* lib::Node::findChild(std::u8string_view name)
{
    for (auto& v : children) {
        if (v->name == name) {
            return std::to_address(v);
        }
    }
    return nullptr;
}


lib::Node& lib::Node::rqChild(std::u8string_view name)
{
    auto q = findChild(name);
    if (!q) [[unlikely]]
        throw std::logic_error("Cannot find child");
    return *q;
}


unsigned lib::Node::maxValueLength() const
{
    unsigned r = value.length();
    for (auto& v : children) {
        r = std::max(r, v->maxValueLength());
    }
    return r;
}

lib::Node& lib::Node::newChild()
{
    return *children.emplace_back(new Node);
}


size_t lib::Node::countLeaves() const
{
    size_t r = 0;
    for (auto& ch : children) {
        r += ch->countLeaves();
    }
    if (!value.empty())
        ++r;
    return r;
}


namespace {

    consteval size_t slLen(char32_t) { return 1; }
    // Kiss is genderless → does not require concatenating strings rather than CPs
    //consteval size_t slLen(std::u32string_view x) { return x.length(); }

    template <class Obj>
    concept StringLike = requires(Obj x) {
        { slLen(x) } -> std::integral;
    };

    template <StringLike auto... Cps>
    consteval size_t totalLength() {
        size_t r = 0;
        (..., (r += slLen(Cps)));
        return r;
    }

    template <StringLike auto... Cps>
    consteval size_t zwjRemder() { return sizeof...(Cps) + totalLength<Cps...>(); }

    template <StringLike auto... Cps>
    consteval size_t zwjSize() { return zwjRemder<Cps...>() - 1; }

    template <size_t N, StringLike auto First, StringLike auto... Rest>
    consteval void copyZwj(std::array<char32_t, N + 1>& data) {
        auto i = 0;
        data[i] = First; ++i;
        (..., (data[i] = cp::ZWJ, ++i, data[i] = Rest, ++i));
    }

    template <char32_t... Cps>
    struct ZwjCat {
        static constexpr auto N = zwjSize<Cps...>();
        using Arr = std::array<char32_t, N + 1>;
        static consteval Arr go() {
            Arr res;
            std::fill(res.begin(), res.end(), 0);
            copyZwj<N, Cps...>(res);
            return res;
        }
        static constexpr Arr arr = go();
        static constexpr std::u32string_view sv { arr.data(), arr.size() - 1 };
    };

    #define ZZ(...) ZwjCat<__VA_ARGS__>::sv

    /// Unit-test for that ZwjCat
    constexpr std::u32string_view WOMAN_BOY = ZZ(cp::WOMAN, cp::BOY);
    static_assert(WOMAN_BOY == U"\U0001F469" "\u200D" "\U0001F466"sv);


    /// Library items that do not make tiles
    const std::unordered_set<std::u8string_view> NO_TILE {
        u8"Component",  // Really ugly, scrap for the sake of nicer ones
        u8"backhand index pointing up",     // Scrapped for the sake of “placard with finger”
        u8"backhand index pointing down",   // Same
        u8"index pointing up",              // Same
        u8"animal-amphibian",   // Scrap frog for the sake of spouting whale
        u8"bust in silhouette", // Scrapped for family silhouette
    };

    /// Library items that forcefully make tiles
    const std::unordered_set<std::u32string_view> FORCE_TILE {
        U"\U0001F468" "\u200D" "\U0001F469" "\u200D" "\U0001F466",  // Family: man, woman, boy
    };

    const std::unordered_map<std::u32string_view, uc::Lfgs> MISRENDERS {
        { U"\U0001F1E6\U0001F1EB", uc::MISRENDER_SIMPLE }, // Afghanistan
        { U"\U0001F1E6\U0001F1F6", uc::MISRENDER_SIMPLE }, // Antarctica
        { U"\U0001F1E9\U0001F1EC", uc::MISRENDER_IO     }, // Diego Garcia
        { U"\U0001F1ED\U0001F1F3", uc::MISRENDER_SIMPLE }, // Honduras
        { U"\U0001F1EE\U0001F1F4", uc::MISRENDER_IO     }, // British Indian Ocean Territory
        { U"\U0001F1F2\U0001F1F6", uc::MISRENDER_SIMPLE }, // Martinique
        { U"\U0001F1F8\U0001F1FE", uc::MISRENDER_SIMPLE }, // Syria
        { U"\U0001F46A",           uc::MISRENDER_SIMPLE }, // family
        { U"\U0001F52B",           uc::MISRENDER_SIMPLE }, // pistol
        { U"\U0001F68F",           uc::MISRENDER_SIMPLE }, // bus stop
        { U"\U0001F6DE",           uc::MISRENDER_SIMPLE }, // wheel
        { U"\U0001F9A0",           uc::MISRENDER_SIMPLE }, // microbe
        { U"\U0001FA85",           uc::MISRENDER_SIMPLE }, // piñata
        { U"\U0001FA92",           uc::MISRENDER_SIMPLE }, // razor
        { U"\U0001FAA4",           uc::MISRENDER_SIMPLE }, // mouse trap
        // Genderless families
        { ZZ(cp::ADULT, cp::ADULT, cp::CHILD),            uc::MISRENDER_FAMILY },
        { ZZ(cp::ADULT, cp::ADULT, cp::CHILD, cp::CHILD), uc::MISRENDER_FAMILY },
        { ZZ(cp::ADULT, cp::CHILD),                       uc::MISRENDER_FAMILY },
        { ZZ(cp::ADULT, cp::CHILD, cp::CHILD),            uc::MISRENDER_FAMILY },
        // Gendered families
        { ZZ(cp::MAN,   cp::WOMAN, cp::BOY),              uc::MISRENDER_FAMILY },
        { ZZ(cp::MAN,   cp::WOMAN, cp::GIRL),             uc::MISRENDER_FAMILY },
        { ZZ(cp::MAN,   cp::WOMAN, cp::GIRL, cp::BOY),    uc::MISRENDER_FAMILY },
        { ZZ(cp::MAN,   cp::WOMAN, cp::BOY,  cp::BOY),    uc::MISRENDER_FAMILY },
        { ZZ(cp::MAN,   cp::WOMAN, cp::GIRL, cp::GIRL),   uc::MISRENDER_FAMILY },
        { ZZ(cp::MAN,   cp::MAN,   cp::BOY),              uc::MISRENDER_FAMILY },
        { ZZ(cp::MAN,   cp::MAN,   cp::GIRL),             uc::MISRENDER_FAMILY },
        { ZZ(cp::MAN,   cp::MAN,   cp::GIRL, cp::BOY),    uc::MISRENDER_FAMILY },
        { ZZ(cp::MAN,   cp::MAN,   cp::BOY,  cp::BOY),    uc::MISRENDER_FAMILY },
        { ZZ(cp::MAN,   cp::MAN,   cp::GIRL, cp::GIRL),   uc::MISRENDER_FAMILY },
        { ZZ(cp::WOMAN, cp::WOMAN, cp::BOY),              uc::MISRENDER_FAMILY },
        { ZZ(cp::WOMAN, cp::WOMAN, cp::GIRL),             uc::MISRENDER_FAMILY },
        { ZZ(cp::WOMAN, cp::WOMAN, cp::GIRL, cp::BOY),    uc::MISRENDER_FAMILY },
        { ZZ(cp::WOMAN, cp::WOMAN, cp::BOY,  cp::BOY),    uc::MISRENDER_FAMILY },
        { ZZ(cp::WOMAN, cp::WOMAN, cp::GIRL, cp::GIRL),   uc::MISRENDER_FAMILY },
        { ZZ(cp::MAN,   cp::BOY),                         uc::MISRENDER_FAMILY },
        { ZZ(cp::MAN,   cp::GIRL),                        uc::MISRENDER_FAMILY },
        { ZZ(cp::MAN,   cp::GIRL, cp::BOY),               uc::MISRENDER_FAMILY },
        { ZZ(cp::MAN,   cp::BOY,  cp::BOY),               uc::MISRENDER_FAMILY },
        { ZZ(cp::MAN,   cp::GIRL, cp::GIRL),              uc::MISRENDER_FAMILY },
        { ZZ(cp::WOMAN, cp::BOY),                         uc::MISRENDER_FAMILY },
        { ZZ(cp::WOMAN, cp::GIRL),                        uc::MISRENDER_FAMILY },
        { ZZ(cp::WOMAN, cp::GIRL, cp::BOY),               uc::MISRENDER_FAMILY },
        { ZZ(cp::WOMAN, cp::BOY,  cp::BOY),               uc::MISRENDER_FAMILY },
        { ZZ(cp::WOMAN, cp::GIRL, cp::GIRL),              uc::MISRENDER_FAMILY },
    };

    const std::unordered_map<char32_t, std::u32string_view> NON_STD_EMOJI {
        { cp::MAN_AND_WOMAN, ZZ(cp::MAN, cp::HANDSHAKE, cp::WOMAN) },
    };

    constexpr const char32_t UNSEARCHABLE_EMOJI_C[] {
        cp::SKIN1, cp::SKIN2, cp::SKIN3, cp::SKIN4, cp::SKIN5, cp::EMOJI_RIGHT_ARROW, 0 };
    constexpr const std::u32string_view UNSEARCHABLE_EMOJI (UNSEARCHABLE_EMOJI_C);

    enum class SearchLevel : unsigned char {
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

    void addGroup(std::stack<lib::Node*>& x, size_t size, std::string_view name)
    {
        auto top = crop(x, size);
        auto& newItem = top->newChild();
        newItem.name = str::toU8sv(name);
        newItem.flags |= uc::Lfg::TRANSLATE;
        if (NO_TILE.contains(newItem.name))
            newItem.flags |= uc::Lfg::NO_TILE;
        x.push(&newItem);
    }

    bool hasSkinGender(std::u32string_view x)
    {
        static constinit char32_t ALL_CHARS_AR[] {
            // Allowed when used alone
            cp::SKIN1, cp::SKIN2, cp::SKIN3, cp::SKIN4, cp::SKIN5,   // 5
            cp::MALE, cp::FEMALE, cp::MAN, cp::WOMAN,            // 9
            cp::EMOJI_RIGHT_ARROW,                         // 10
            // The rest
            cp::MAN_AND_WOMAN, cp::TWO_MEN, cp::TWO_WOMEN, cp::SANTA_CLAUS, cp::MRS_CLAUS };
        constexpr size_t OFS_1 = 10;
        static constinit std::u32string_view ALL_CHARS { ALL_CHARS_AR, std::size(ALL_CHARS_AR) };
        static constinit std::u32string_view SPEC_CHARS { ALL_CHARS_AR + OFS_1, std::end(ALL_CHARS_AR) };

        switch (x.size()) {
        case 0:
            return false;
        case 1:
            return (x.find_first_of(SPEC_CHARS) != std::u32string::npos);
        case 2: {
                auto sv = (x[1] == cp::VS16) ? SPEC_CHARS : ALL_CHARS;
                return (x.find_first_of(sv) != std::u32string::npos);
            }
        default:
            return (x.find_first_of(ALL_CHARS) != std::u32string::npos);
        }
    }

    constexpr int NO_POS = -2;
    constexpr int TWIN_POS = -1;
    constexpr int REAL_POS = 0;

    int onlyPos(std::u32string_view x, char32_t c)
    {
        auto p1 = x.find(c);
        if (p1 == std::u32string_view::npos)
            return NO_POS;
        auto p2 = x.find(c, p1 + 1);
        if (p2 != std::u32string_view::npos)
            return TWIN_POS;
        return p1;
    }

    uc::Lfgs checkMenWomen(std::u32string_view x, std::string_view name)
    {
        uc::Lfgs r;
        // No ZWJ → do nothing
        if (x.size() < 3    // min 3: XXX + ZWJ + YYY
                || x.find(cp::ZWJ) == std::u32string_view::npos)
            return r;

        // Men/women
        auto pMan = onlyPos(x, cp::MAN);
        auto pWoman = onlyPos(x, cp::WOMAN);
        bool hasManWoman = (pMan >= REAL_POS && pWoman >= REAL_POS);
        if (hasManWoman) {
            r |= uc::Lfg::SWAP_MAN_WOMAN;
        }

        // Boys/girls
        auto pBoy = onlyPos(x, cp::BOY);
        auto pGirl = onlyPos(x, cp::GIRL);
        if (pBoy >= TWIN_POS || pGirl >= TWIN_POS) {
            // Family emoji: man → woman, girl → boy
            if (pMan < TWIN_POS && pWoman < TWIN_POS) {
                throw std::logic_error(str::cat(
                    "Family emoji <", name, ">: has boy/girl, no men/women"));
            }
            if (pBoy >= REAL_POS && pGirl >= REAL_POS) {
                if (pBoy < pGirl) {
                    throw std::logic_error(str::cat(
                        "Emoji <", name, ">: BOY < GIRL"));
                }
                if (hasManWoman && (pWoman < pMan)) {
                    throw std::logic_error(str::cat(
                        "Family emoji <", name, ">: WOMAN < MAN"));
                }
                r |= uc::Lfg::SWAP_BOY_GIRL;
            }
        } else {
            // Couple emoji: woman → man
            if (hasManWoman && (pMan < pWoman)) {
                throw std::logic_error(str::cat(
                    "Couple emoji <", name, ">: MAN < WOMAN"));
            }
        }
        return r;
    }

    constexpr char32_t NO_CP = 0;
    struct EmojiCpInfo {
        char32_t mainCp;
        char32_t skin;

        constexpr operator bool() const noexcept { return mainCp; }
    };

    EmojiCpInfo emojiCp(std::u32string_view x)
    {
        switch (x.length()) {
        case 1:
            return { .mainCp = x[0], .skin = NO_CP };
        case 2:
            if (x[1] == cp::VS16)
                return { .mainCp = x[0], .skin = NO_CP };
            if (x[1] >= cp::SKIN1 && x[1] <= cp::SKIN5)
                return { .mainCp = x[0], .skin = x[1] };
            [[fallthrough]];
        default:
            return { .mainCp = NO_CP, .skin = NO_CP };
        }
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
                addGroup(treePath, 1, q);
            } if (auto q = str::remainderSv(s, S_SUBGROUP); !q.empty()) {
                // Subgroup
                addGroup(treePath, 2, q);
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
                const std::u32string_view myStr { codes.buffer(), nCodes };

                // Add to tree
                const auto [text, emVersion, name] = splitLineSv(comment, ' ', ' ');
                const auto additionalFlags = checkMenWomen(myStr, name);
                auto& newItem = treePath.top()->newChild();
                if (lat::hasUpper(name)) { // has uppercase letter → pre-decapped
                    newItem.name = str::toU8sv(name);
                } else { // otherwise decapitalize
                    newItem.name = str::toU8sv(decapitalizeEmoji(name));
                }
                newItem.value.assign(codes.buffer(), nCodes);

                newItem.emojiVersion = emVersion;
                newItem.flags = additionalFlags | uc::Lfg::GRAPHIC_EMOJI;
                if (mainCode != 0)
                    r.allSingleChar.emplace(mainCode, &newItem);
                if (NO_TILE.contains(newItem.name))
                    newItem.flags |= uc::Lfg::NO_TILE;
                if (!FORCE_TILE.contains(newItem.value) && hasSkinGender(newItem.value))
                    newItem.flags |= uc::Lfg::NO_TILE;

                // Misrender
                if (auto it = MISRENDERS.find(newItem.value); it != MISRENDERS.end()) {
                    newItem.flags |= it->second;
                    if (it->second == uc::MISRENDER_SIMPLE && mainCode != 0)
                        r.misrenders.insert(mainCode);
                }

                // Non-standard sequence
                if (auto ocp = emojiCp(myStr)) {
                    // For ZWJ sequence non-standard values are computed in Unicodia
                    // and sometimes multiple, need a special branch for
                    // single-chars and VS16
                    auto nsit = NON_STD_EMOJI.find(ocp.mainCp);
                    if (nsit != NON_STD_EMOJI.end()) {
                        newItem.nonStandardValue = nsit->second;
                    }
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

                // Emoji data
                auto [itData, _] = r.all.try_emplace(std::u32string{myStr});
                auto& v = itData->second;
                v.emojiVersion = emVersion;

                ++r.count.withComponents;
            }
        }
    }
    // Flags A…Z are also single-characters
    auto& nodeComponents = r.root.rqChild(u8"Component");
        nodeComponents.flags |= uc::Lfg::NO_COUNTING;
    auto& nodeRegional = nodeComponents.newChild();
        nodeRegional.flags |= uc::Lfg::TRANSLATE;
        nodeRegional.name = u8"Regional";

    for (char32_t i = cp::FLAG_A; i <= cp::FLAG_Z; ++i) {
        r.allSingleChar.emplace(i, nullptr);
        auto& thatNode = nodeRegional.newChild();
        thatNode.value = std::u32string(1, i);
        thatNode.flags |= uc::Lfg::GRAPHIC_EMOJI | uc::Lfg::NO_COUNTING;
    }
    return r;
}


namespace {

    void recurseEnum(const lib::Node& node, int& counter)
    {
        for (auto& v : node.children) {
            v->cache.index = counter++;
        }
        for (auto& v : node.children) {
            recurseEnum(*v, counter);
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
            os << node.children[0]->cache.index;
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
            os << "NOT_EMOJI";
        } else {
            std::string newVer = node.emojiVersion;
            str::replace(newVer, '.', '_');
            os << newVer;
        }
        // non-standard sequence
        if (!node.nonStandardValue.empty()) {
            os << ", ";
            writeU32sv(os, node.nonStandardValue);
        }
        // closing brace
        os << " },   // " << node.cache.index << "\n";
    }

    void recurseWrite(
            std::ostream& os,
            const lib::Node& node)
    {
        for (auto& v : node.children) {
            writeNode(os, *v, node.cache.index);
        }
        for (auto& v : node.children) {
            recurseWrite(os, *v);
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
        { 'H' },    // Contain Hangul
        { 'I' },    // Look incomplete
        { 'K', StrangeTarget::AFTER, uc::Lfg::NO_TILE },    // Katakana-like
        { 'M', StrangeTarget::AFTER, uc::Lfg::CODE_AS_NAME | uc::Lfg::NO_TILE }, // Mirrored
        { 'O' },    // Odd component
        { 'R', StrangeTarget::AFTER, uc::Lfg::CODE_AS_NAME | uc::Lfg::NO_TILE },    // Rotated
        { 'S' },    // Stroke-heavy
        { 'U' },    // Unusual structure
        { 'Y', StrangeTarget::AFTER, uc::Lfg::CODE_AS_NAME | uc::Lfg::NO_TILE }, // Fully-reflective
    };

}   // anon namespace

lib::StrangeCjk::StrangeCjk()
{
    root.name = u8"Strange";
    root.flags |= uc::Lfg::TRANSLATE;
    for (auto& v : strangeCats) {
        auto& cat = root.newChild();
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
        if (whatFound == std::end(strangeCats) || whatFound->key != type) {
            char buf[100];
            snprintf(buf, std::size(buf),
                     "Unknown strange category '%c'", type);
            throw std::logic_error(buf);
        }
        auto iCat = whatFound - std::begin(strangeCats);
        auto& subcat = root.children[iCat];
        auto& child = *subcat->children.emplace_back(new lib::Node);
        child.value.assign(1, cp);
        child.flags |= uc::Lfg::CODE_AS_NAME;

        if (whatFound->target == StrangeTarget::AFTER) {
            auto cpNames = str::splitSv(v.substr(1), ':');
            auto& parent = (cpNames.size() > 1)
                    ? child : *subcat;
            for (auto cpName : cpNames) {
                if (cpName.starts_with("U+")) {
                    auto sCode = cpName.substr(2);
                    auto code = fromHex(sCode);
                    auto& child2 = *parent.children.emplace_back(new lib::Node);
                    child2.value.assign(1, code);
                    child2.flags = whatFound->targetFlags;
                }
            }
        }
    }
}


namespace {

    void retrieveEmojiVersion(lib::Node& node, const lib::MAll& mAll)
    {
        auto it = mAll.find(node.value);
        if (it != mAll.end()) {
            node.emojiVersion = it->second.emojiVersion;
        }
    }

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
                    std::string_view tilePattern, unsigned& indexInPattern,
                    const lib::MAll& mAll)
    {
        std::string_view text8 = tag.text().get();
        auto text32 = mojibake::toS<std::u32string>(text8);
        auto sub32 = str::splitSv(text32, ' ');
        result.children.reserve(result.children.size() + sub32.size());
        char32_t cap = '!';     // something odd
        for (auto v : sub32) {
            auto& node = *result.children.emplace_back(new lib::Node);
            node.value = v;
            retrieveEmojiVersion(node, mAll);
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
                     std::string_view tilePattern, unsigned& indexInPattern,
                     const lib::MAll& mAll)
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
            auto& node = *result.children.emplace_back(new lib::Node);
            node.value = std::u32string{ cp };
            retrieveEmojiVersion(node, mAll);
            conditionalBan(node, tilePattern, indexInPattern);
            if (isForgetTest) {
                auto& w = forgetMap[cp];
                ++w.lib.count;
            }
        }
    }

    void appendSequence(lib::Node& result, pugi::xml_node tag,
                        std::string_view tilePattern, unsigned& indexInPattern,
                        const lib::MAll& mAll)
    {
        std::string_view value8 = tag.attribute("v").as_string();
        if (value8.empty())
            return;
        auto value32 = mojibake::toS<std::u32string>(value8);
        if (value32.empty())
            return;
        auto& node = *result.children.emplace_back(new lib::Node);
        loadName(node, tag);
        node.value = std::move(value32);
        retrieveEmojiVersion(node, mAll);
        conditionalBan(node, tilePattern, indexInPattern);
    }

    void loadRecurse(lib::Node& result, forget::Map& forgetMap,
                     pugi::xml_node tag, bool isForgetTest,
                     std::string_view tilePattern,
                     const lib::MAll& mAll)
    {
        unsigned indexInPattern = 0;
        isForgetTest = tag.attribute("forgetTest").as_bool(isForgetTest);
        for (auto v : tag.children()) {
            if (v.name() == "f"sv) {
                // Folder
                auto& newFolder = result.newChild();
                loadName(newFolder, v);
                // As all tile patterns are taken from XML → they are null-term
                std::string_view newTilePattern = v.attribute("tilePattern").as_string(tilePattern.data());
                loadRecurse(newFolder, forgetMap, v, isForgetTest, newTilePattern, mAll);
            } else if (v.name() == "d"sv) {
                // Dump
                appendDump(result, forgetMap, v, isForgetTest, tilePattern, indexInPattern, mAll);
            } else if (v.name() == "range"sv) {
                appendRange(result, forgetMap, v, isForgetTest, tilePattern, indexInPattern, mAll);
            } else if (v.name() == "sq"sv) {
                appendSequence(result, v, tilePattern, indexInPattern, mAll);
            }
        }
    }

}   // anon namespace


lib::Manual lib::loadManual(const char* fname, const MAll& mAll)
{
    pugi::xml_document doc;
    if (auto parseRes = doc.load_file(fname); !parseRes)
        throw std::logic_error(str::cat("Cannot open ", fname));

    lib::Manual r;

    auto root = doc.root().child("library");
    if (!root)
        throw std::logic_error("No root");

    // Do not use empty tilePattern, need working data()
    loadRecurse(r.root, r.forgetMap, root, false, ""sv, mAll);

    return r;
}
