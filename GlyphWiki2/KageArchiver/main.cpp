// STL
#include <iostream>
#include <fstream>
#include <map>
#include <unordered_map>

// Libs
#include "u_Strings.h"

using namespace std::string_view_literals;

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

using KageList = std::unordered_map<std::string, std::string>;

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
                auto q = str::trimSv(cols[0]);
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
        auto key = str::trim(cols[0]);
        auto value = str::trimSv(cols[2]);
        auto [_, was] = r.try_emplace(std::move(key), value);
        if (!was)
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
    SafeVector<std::string> candidates(char32_t code) const;
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

char32_t parseHexTask(std::string_view s)
{
    unsigned code;
    s = str::trimSv(s);
    auto res = std::from_chars(
                std::to_address(s.begin()), std::to_address(s.end()), code, 16);
    if (res.ec != std::error_code())
        throw BadTask(str::cat("Cannot parse hex ", s));
    return code;
}

TaskList readTaskList()
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


void archiveTasks(const TaskList& taskList, const KageList& kageList)
{
    for (const auto& [k, t] : taskList) {
        auto cands = t.candidates(k);
        if (cands.empty()) {
            char buf[100];
            snprintf(buf, std::size(buf), "Task for %X made no candidates", unsigned(k));
            throw BadTask(buf);
        }
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
