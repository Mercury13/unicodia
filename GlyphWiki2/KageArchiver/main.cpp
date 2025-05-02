// STL
#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <unordered_map>

// Libs
#include "u_Strings.h"

// Project-local
#include "kagelist.h"
#include "tasklist.h"


DEFINE_EXCEPTION_CLASS(BadData, std::logic_error)


/// KageList value_type
using Klv = const KageList::value_type*;


std::string_view tryRemovePrefixSv(
        std::string_view currRoot,
        std::string_view currKey,
        std::string_view bigKey)
{
    auto posVersion = bigKey.find('@');
    if (posVersion != std::string_view::npos) {
        char buf[200];
        snprintf(buf, std::size(buf),
                "Aggressive search + specific version, IDK, check manually what to do:"
                " root '%*s', key '%*s', troublesome '%*s'",
                PRF_SV(currRoot), PRF_SV(currKey), PRF_SV(bigKey));
        throw BadData(buf);
    }
    auto posDash = bigKey.rfind('-');
    if (posDash == std::string_view::npos)
        return {};
    auto next = bigKey.substr(posDash + 1);
    if (next.empty())
        return {};
    for (auto c : next) {
        switch (c) {
        case 'g':   // Possible country codes
        case 't':
        case 'j':
        case 'k':
        case 'v': break;
        default : return {};
        }
    }
    return bigKey.substr(0, posDash);
}


constexpr bool hasKagePrefix(std::string_view value, std::string_view prefix)
{
    if (prefix.empty())
        return false;
    return value.starts_with(prefix)
           && ((value.length() <= prefix.length())
                    || (value[prefix.length()] == '-'));
}

// UTs
static_assert(!hasKagePrefix("alpha", ""));
static_assert(!hasKagePrefix("-alpha", ""));
static_assert(!hasKagePrefix("alpha-bravo", "charlie"));
static_assert(!hasKagePrefix("alphab-ravo", "alpha"));
static_assert( hasKagePrefix("alpha-bravo", "alpha"));


///
/// Follows aliases until it finds a meaningful thing
/// @return  may return nullptr if not found
///
[[nodiscard]] Klv followKage(
        const KageList& kageList, std::string_view root,
        const TaskSets::Country& country)
{
    static constexpr std::string_view ALIAS_LINK_PREFIX = "99:0:0:0:0:200:200:";
    static constexpr auto PREFIX_L = ALIAS_LINK_PREFIX.length();

    std::string key { root };
    auto it = kageList.find(key);
    if (it == kageList.end()) {
        return nullptr;
    }
    char buf[100];
    std::string circlePrevention {};  // to prevent going in circles in aggressive mode
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
        if (country.isAggressive && !hasKagePrefix(value, circlePrevention)) {
            auto smallValue = tryRemovePrefixSv(root, key, value);
            if (!smallValue.empty()) {
                for (auto& suffix : country.suffixSequence) {
                    if (suffix.empty())
                        continue;
                    auto newKey = str::cat(smallValue, suffix);
                    it = kageList.find(newKey);
                    if (it == kageList.end())
                        continue;
                    key = newKey;
                    circlePrevention = smallValue;
                    goto cont;
                }
            }
        }
        it = kageList.find(value);
        if (it == kageList.end()) {
            snprintf(buf, std::size(buf),
                     "Ideo '%*s' hardlinks to missing ideo '%*s'",
                     PRF_SV(key), PRF_SV(value));
            throw BadData(buf);
        }
        key = value;
    cont:;
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
    // :) The thing is white, turn to grey
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
          "#   #blahblah   - comment, do nothing" "\n"
          "#   =blah:blah  - database entry" "\n"
          "#   G code root - go; create an ideograph and erase entries" "\n";

    for (const auto& [k, t] : taskList) {
        const auto cands = t.candidates(k);
        if (cands.empty()) {
            char buf[100];
            snprintf(buf, std::size(buf), "Task for %X made no candidates", unsigned(k));
            throw BadTask(buf);
        }
        for (const auto& v : cands) {
            if (auto where = followKage(kageList, v, t.sets->country)) {
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

        KageList kageList;
        std::cout << "Reading small Kage list..." << std::flush;
        KageCache smallCache;
        auto nSmall = readKageList(kageList, smallCache, 2'000'000, "dump_newest_only.txt");
        std::cout << "OK, " << nSmall << " entries" "\n";

        std::cout << "Reading LARGE Kage list..." << std::flush;
        KageCache largeCache;
        auto nLarge = readKageList(kageList, largeCache, 5'500'000, "dump_all_versions.txt");
        std::cout << "OK, " << nLarge << " entries" "\n";

        std::cout << "Archiving tasks..." << std::flush;
        archiveTasks(taskList, kageList);
        std::cout << "OK" "\n";

        std::cout << "Success, wait a bit for data destruction!" "\n";
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
