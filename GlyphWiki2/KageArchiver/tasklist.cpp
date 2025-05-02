#include "tasklist.h"

// STL
#include <span>

// Libs
#include "u_Strings.h"

using namespace std::string_view_literals;


class SettingsMan
{
public:
    SettingsMan();
    ///  @post  never null
    void parseCountry(std::span<std::string_view> params);
    const std::shared_ptr<TaskSets>& sets();
private:
    TaskSets currSets;
    std::shared_ptr<TaskSets> cachedPtr;
};


SettingsMan::SettingsMan()
{
    parseCountry({});
}


const std::shared_ptr<TaskSets>& SettingsMan::sets()
{
    if (!cachedPtr) {
        cachedPtr = std::make_unique<TaskSets>(currSets);
    }
    return cachedPtr;
}


void SettingsMan::parseCountry(std::span<std::string_view> params)
{
    bool isStrict = false;
    TaskSets::Country r {};
    for (auto v : params) {
        if (v.length() == 1) {
            r.suffixSequence.emplace_back(str::cat('-', v));
        } else if (v == "STRICT"sv) {
            isStrict = true;
        } else if (v == "AGGR"sv) {
            r.isAggressive = true;
        }
    }
    if (!isStrict) {
        r.suffixSequence.emplace_back();
    }
    currSets.country = r;
    cachedPtr.reset();
}


constexpr unsigned BUFSZ = 100;
using Buf = char[100];

void printCand(Buf& buf, const char* prefix, unsigned code, std::string_view suffix)
{
    unsigned sz = snprintf(buf, BUFSZ, "%s%x%*s", prefix, unsigned(code), PRF_SV(suffix));
    if (sz >= BUFSZ - 2) {
        snprintf(buf, BUFSZ, "Candidate for %X too long", unsigned(code));
        throw BadTask(buf);
    }
}

SafeVector<std::string> Task::candidates(char32_t code) const
{
    Buf buf;
    SafeVector<std::string> r;
    for (auto& v : sets->country.suffixSequence) {
        printCand(buf, "u", code, v);
        r.push_back(buf);
    }
    return r;
}


void putTask(TaskList& r, char32_t code, SettingsMan& man)
{
    auto [_, was] = r.try_emplace(code, man.sets());
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
    SettingsMan setMan;

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
                    putTask(r, c, setMan);
                }
            } else if (cmd == "country"sv) {
                std::span p1 = params;
                p1 = p1.subspan(1);
                setMan.parseCountry(p1);
            } else {
                throw BadTask(str::cat("Unknown command: ", cmd));
            }
        } else { // Single char
            char32_t code = parseHexTask(st);
            putTask(r, code, setMan);
        }
    }

    if (r.empty())
        throw BadTask("No tasks for some reason");
    return r;
}
