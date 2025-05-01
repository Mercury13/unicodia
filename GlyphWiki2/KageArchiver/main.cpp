// STL
#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <unordered_map>

// Libs
#include "u_Strings.h"

using namespace std::string_view_literals;

class Shash : public std::hash<std::string_view>
{
private:
    using Super = std::hash<std::string_view>;
public:
    using is_transparent = void;
    using Super::operator ();
};

// NOLINTBEGIN(bugprone-macro-parentheses)
#define DEFINE_EXCEPTION_CLASS(CMe, CSuper) \
    class CMe : public CSuper { \
    private:                    \
        using Super = CSuper;   \
    public:                     \
        using Super::Super; };
// NOLINTEND(bugprone-macro-parentheses)

DEFINE_EXCEPTION_CLASS(StrangeDump, std::logic_error)
DEFINE_EXCEPTION_CLASS(BadData, std::logic_error)
DEFINE_EXCEPTION_CLASS(BadTask, std::logic_error)

using KageList = std::unordered_map<std::string, std::string, Shash, std::equal_to<>>;

KageList readKageList()
{
    std::ifstream is("dump_newest_only.txt");
    if (!is.is_open())
        throw std::logic_error("Need dump_newest_only.txt, part of GlyphWiki dump");

    KageList r;
    std::string s;
    // Parse header
    while (std::getline(is, s)) {
        if (s.starts_with("--")) {
            goto headerEnd;
        }
    }
    throw StrangeDump("Dis not find where header ends");
headerEnd:
    // Parse normal lines
    while (std::getline(is, s)) {
        auto cols = str::splitSv(s, '|', false);
        switch (cols.size()) {
        case 3:
            break;
        case 0:
            continue;
        case 1: {
                auto q = cols[0];
                if (q.empty())
                    continue;
                if (q.starts_with('(')) {
                    q = q.substr(1);
                    unsigned value = 0;
                    std::from_chars(std::to_address(q.begin()), std::to_address(q.end()), value);
                    if (value != r.size())
                        throw StrangeDump("Number of lines mismatch");
                    goto dumpEnd;
                }
            } [[fallthrough]];
        default:
            throw StrangeDump("Not three columns: " + s);
        }
        std::string key { cols[0] };
        auto value = cols[2];
        auto [_, wasIns] = r.try_emplace(std::move(key), value);
        if (!wasIns)
            throw StrangeDump("Key repeats: " + key);
    }
dumpEnd:
    if (r.empty())
        throw StrangeDump("No data for some reason");
    return r;
}


struct Task
{
    // No data for now, but next time we’ll have Taiwan
    [[nodiscard]] SafeVector<std::string> candidates(char32_t code) const;
};


constexpr unsigned BUFSZ = 100;
using Buf = char[100];


void printCand(Buf& buf, const char* prefix, unsigned code, const char* suffix)
{
    unsigned sz = snprintf(buf, BUFSZ, "%s%x%s", prefix, unsigned(code), suffix);
    if (sz >= BUFSZ - 2) {
        snprintf(buf, BUFSZ, "Candidate for %X too long", unsigned(code));
        throw BadTask(buf);
    }
}

SafeVector<std::string> Task::candidates(char32_t code) const
{
    char buf[100];
    printCand(buf, "u", code, "");
    SafeVector<std::string> r;
    r.push_back(buf);
    return r;
}


using TaskList = std::map<char32_t, Task>;

void putTask(TaskList& r, char32_t code)
{
    auto [_, was] = r.try_emplace(code);
    if (!was) {
        char buf[40];
        snprintf(buf, std::size(buf), "Code repeats: %X", unsigned(code));
        throw BadTask(buf);
    }
}

[[nodiscard]] char32_t parseHexTask(std::string_view s)
{
    unsigned code = 0;
    s = str::trimSv(s);
    auto res = std::from_chars(
                std::to_address(s.begin()), std::to_address(s.end()), code, 16);
    if (res.ec != std::error_code())
        throw BadTask(str::cat("Cannot parse hex ", s));
    return code;
}

[[nodiscard]] TaskList readTaskList()
{
    std::ifstream is("hani-tofu.txt");
    if (!is.is_open())
        throw std::logic_error("Need hani-tofu.txt, our new task list with commands");

    TaskList r;

    std::string s;
    while (std::getline(is, s)) {
        auto st = str::trimSv(s);
        // Empty line, comment
        if (st.empty() || st.starts_with('#'))
            continue;
        if (st.starts_with('/')) { // Command
            st = st.substr(1);  // erase slash
            auto params = str::splitSv(st, ' ', true);
            if (params.empty())
                throw BadTask("Just slash, no command");
            auto cmd = params[0];
            if (cmd == "range"sv) {
                // COMMAND: /range
                if (params.size() != 3)
                    throw BadTask("/range should have two params");
                auto min = parseHexTask(params[1]);
                auto max = parseHexTask(params[2]);
                if (min > max)
                    throw BadTask("In /range min should be <= max");
                for (char32_t c = min; c <= max; ++c) {
                    putTask(r, c);
                }
            } else {
                throw BadTask(str::cat("Unknown command: ", cmd));
            }
        } else { // Single char
            char32_t code = parseHexTask(st);
            putTask(r, code);
        }
    }

    if (r.empty())
        throw BadTask("No tasks for some reason");
    return r;
}


#define PRF_SV(x) (unsigned((x).length())), ((x).data())


/// KageList value_type
using Klv = const KageList::value_type*;

///
/// Follows aliases until it finds a meaningful thing
/// @return  may return nullptr if not found
///
[[nodiscard]] Klv followKage(const KageList& kageList, std::string_view root)
{
    static constexpr std::string_view ALIAS_LINK_PREFIX = "99:0:0:0:0:200:200:";
    static constexpr auto PREFIX_L = ALIAS_LINK_PREFIX.length();

    std::string_view key = root;
    auto it = kageList.find(key);
    if (it == kageList.end()) {
        return nullptr;
    }
    char buf[100];
    // And now follow hardlinks
    while (true) {
        std::string_view value = it->second;
        // Has >1 lines? Stop!
        auto pFind = value.find('$');
        if (pFind != std::string::npos) {
            return std::to_address(it);
        }
        // Is not a alias-link? Stop!
        if (!value.starts_with(ALIAS_LINK_PREFIX)) {
            return std::to_address(it);
        }
        value = value.substr(PREFIX_L);
        // Ensure that no extra columns
        pFind = value.find(':');
        if (pFind != std::string::npos) {
            snprintf(buf, std::size(buf),
                     "Alias-link in '%*s' has extra columns", PRF_SV(key));
            throw BadData(buf);
        }
        // Find, replace key and it
        it = kageList.find(value);
        if (it == kageList.end()) {
            snprintf(buf, std::size(buf),
                     "Ideo '%*s' hardlinks to missing ideo '%*s'",
                     PRF_SV(key), PRF_SV(value));
            throw BadData(buf);
        }
        key = value;
    }
}


struct KlvCmp {
    bool operator () (Klv x, Klv y) const;
};

bool KlvCmp::operator () (Klv x, Klv y) const
{
    if (x == y)
        return false;
    return (x->first < y->first);
}

// Colour in Deep-first search:
//   White = untouched (tree-link),
//   Grey = entered, back-link
//   Black = entered and left, cross- or forward-link
enum class DfsColor : unsigned char { GRAY, BLACK };

struct KlvInfo {
    DfsColor color = DfsColor::GRAY;
};

using Sklv = std::map<Klv, KlvInfo, KlvCmp>;

void doFollowDeepLinks(
        Sklv& r, const KageList& kageList, Klv start, Klv thing)
{
    char buf[200];
    auto [it, wasIns] = r.try_emplace(thing);
    if (!wasIns) {
        if (it->second.color == DfsColor::GRAY) {
            std::string_view badKey = it->first->first;
            snprintf(buf, std::size(buf),
                     "Cyclic reference: initiator '%*s', good '%*s', bad '%*s'",
                     PRF_SV(start->first), PRF_SV(thing->first), PRF_SV(badKey));
            throw BadData(buf);
        }
        return;
    }
    it->second.color = DfsColor::GRAY;
    std::string_view kageText = thing->second;
    auto lines = str::splitSv(kageText, '$', true);
    for (auto line : lines) {
        if (line.empty())
            continue;
        auto cols = str::splitSv(line, ':', false);
        if (cols.empty())
            continue;
        if (cols[0] == "99") {  // 99 = link
            if (cols.size() < 8) {
                snprintf(buf, std::size(buf),
                         "Strange link in '%*s': need 8+ cols",
                         PRF_SV(thing->first));
                throw BadData(buf);
            }
            auto target = cols[7];
            // Target may be “blah@5”, remove that At
            auto posAt = target.find('@');
            if (posAt != std::string_view::npos) {
                target = target.substr(0, posAt);
            }
            // Find target and recurse
            auto it1 = kageList.find(target);
            if (it1 == kageList.end()) {
                snprintf(buf, std::size(buf),
                         "Ideo '%*s' links to missing ideo '%*s'",
                         PRF_SV(thing->first), PRF_SV(target));
                throw BadData(buf);
            }
            doFollowDeepLinks(r, kageList, start, std::to_address(it1));
        }
    }
    it->second.color = DfsColor::BLACK;
}

[[nodiscard]] Sklv followDeepLinks(
        const KageList& kageList, Klv start)
{
    if (!start)
        throw std::logic_error("Start should not be null");
    Sklv r;
    doFollowDeepLinks(r, kageList, start, start);
    return r;
}


void archiveTasks(const TaskList& taskList, const KageList& kageList)
{
    char buf[100];

    std::ofstream os("hani-tasks.txt");
    os << "# Automatically created with KageArchiver, do not edit!" "\n"
          "# Commands:" "\n"
          "#   =blah:blah  - database entry" "\n"
          "#   G code root - create an ideograph and erase entries" "\n";

    for (const auto& [k, t] : taskList) {
        const auto cands = t.candidates(k);
        if (cands.empty()) {
            char buf[100];
            snprintf(buf, std::size(buf), "Task for %X made no candidates", unsigned(k));
            throw BadTask(buf);
        }
        for (const auto& v : cands) {
            if (auto where = followKage(kageList, v)) {
                auto fullList = followDeepLinks(kageList, where);
                os << '\n';
                for (auto& q : fullList) {
                    auto& [k, v] = *q.first;
                    os << '=' << k << '=' << v << '\n';
                }
                snprintf(buf, std::size(buf),
                         "G %X %*s" "\n",
                         k, PRF_SV(where->first));
                os << buf;
                goto found;
            }
        }
        snprintf(buf, std::size(buf),
                 "All candidates for %X are missing in Kage base",
                 unsigned(k));
        throw BadTask(buf);
    found:;
    }
}


void writeError(const char* header, const std::exception& e)
{
    std::cout << "\n\n" << header << ": " << e.what() << '\n';
}

int main()
{
    try {
        std::cout << "Reading task list..." << std::flush;
        auto taskList = readTaskList();
        std::cout << "OK, " << taskList.size() << " tasks" "\n";

        std::cout << "Reading Kage list..." << std::flush;
        auto kageList = readKageList();
        std::cout << "OK, " << kageList.size() << " entries" "\n";

        std::cout << "Archiving tasks..." << std::flush;
        archiveTasks(taskList, kageList);
        std::cout << "OK" "\n";

        std::cout << "Success!" "\n";
    } catch (const BadTask& e) {
        writeError("BAD TASKS", e);
    } catch (const BadData& e) {
        writeError("BAD DATA", e);
    } catch (const StrangeDump& e) {
        writeError("STRANGE DUMP", e);
    } catch (const std::exception& e) {
        writeError("ERROR", e);
    }
    return 0;
}
