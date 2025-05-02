#include "tasklist.h"

#include "u_Strings.h"

using namespace std::string_view_literals;


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
