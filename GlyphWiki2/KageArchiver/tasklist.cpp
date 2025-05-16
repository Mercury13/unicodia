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
    void parseCountry(std::span<std::string_view> params);
    void parseDefaultCountry(std::span<std::string_view> params);
    void setPrefix(std::string x);
    /// @post  never null
    const std::shared_ptr<TaskSets>& sets();
private:
    TaskSets currSets;
    std::shared_ptr<TaskSets> cachedPtr;
    SafeVector<std::string> defaultCountries {};  // empty initially
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

template <class Dest, class Src>
inline void insertBack(Dest& dest, Src& src)
{
    dest.insert(std::end(dest), std::begin(src), std::end(src));
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
        } else if (v == "UHAN"sv) {
            r.limitToUnihan = true;
        } else if (v == "DEFAULT"sv) {
            insertBack(r.suffixSequence, defaultCountries);
        } else if (v == "ORERASE"sv) {
            r.orErase = true;
        } else {
            char buf[200];
            snprintf(buf, std::size(buf),
                    "Unknown command for /country: '%.*s'", PRF_SV(v));
            throw BadTask(buf);
        }
    }
    if (!isStrict) {
        r.suffixSequence.emplace_back();
    }
    currSets.country = r;
    cachedPtr.reset();
}


void SettingsMan::setPrefix(std::string x)
{
    currSets.prefix = std::move(x);
    cachedPtr.reset();
}


void SettingsMan::parseDefaultCountry(std::span<std::string_view> params)
{
    defaultCountries.clear();
    for (auto v : params) {
        if (v.length() == 1) {
            defaultCountries.emplace_back(str::cat('-', v));
        } else {
            char buf[200];
            snprintf(buf, std::size(buf),
                    "Unknown command for /defaultcountry: '%.*s'", PRF_SV(v));
            throw BadTask(buf);
        }
    }
}


constexpr unsigned BUFSZ = 100;
using Buf = char[100];

void printCand(Buf& buf, std::string_view prefix, unsigned code, std::string_view suffix)
{
    unsigned sz = snprintf(buf, BUFSZ, "%.*s%x%.*s",
                           PRF_SV(prefix), unsigned(code), PRF_SV(suffix));
    if (sz >= BUFSZ - 2) {
        snprintf(buf, BUFSZ, "Candidate for %X too long", unsigned(code));
        throw BadTask(buf);
    }
}

Flags<CountryFg> suffixToCountries(std::string_view suffix)
{
    if (suffix.empty())
        return CountryFg::ALL;
    if (suffix.length() != 2 || suffix[0] != '-')
        return NO_FLAGS;
    switch (suffix[1]) {
    case 'g': return CountryFg::CHINA;
    case 'j': return CountryFg::JAPAN;
    case 'k': return CountryFg::KOREA;
    case 't': return CountryFg::TAIWAN;
    case 'h': return CountryFg::HONGKONG;
    case 'm': return CountryFg::MACAO;
    case 'v': return CountryFg::VIETNAM;
    case 's': return CountryFg::DAIZO;
    default:  return NO_FLAGS;
    }
}

SafeVector<Candidate> Task::candidates(char32_t code, const UnicodeList& list) const
{
    Buf buf;
    SafeVector<Candidate> r;
    if (sets->country.limitToUnihan) {
        for (auto& v : sets->country.suffixSequence) {
            auto it = list.find(code);
            if (it == list.end()) {
                snprintf(buf, std::size(buf),
                         "No info about codepoint %X", unsigned(code));
                throw BadTask(buf);
            }
            auto countries = suffixToCountries(v);
                if (it->second.countries.haveAny(countries)) {
                printCand(buf, sets->prefix, code, v);
                r.push_back(
                    Candidate { .text = buf, .isFallback = v.empty() });
            }
        }
    } else {
        for (auto& v : sets->country.suffixSequence) {
            printCand(buf, sets->prefix, code, v);
            r.push_back(
                Candidate { .text = buf, .isFallback = v.empty() });
        }
    }
    return r;
}

enum class PutMode : unsigned char { NEW, FIXUP };

///
///  @return  pointer to task, never null
///
Task* putTask(TaskList& r, char32_t code, SettingsMan& man, PutMode mode)
{
    auto [it, wasIns] = r.insert_or_assign(
                        code, Task { .sets = man.sets() });
    char buf[100];
    switch (mode) {
    case PutMode::NEW:
        if (!wasIns) {
            snprintf(buf, std::size(buf), "Code repeats: %X", unsigned(code));
            throw BadTask(buf);
        }
        break;
    case PutMode::FIXUP:
        if (wasIns) {
            snprintf(buf, std::size(buf), "Code does not fix up: %X", unsigned(code));
            throw BadTask(buf);
        }
        break;
    }
    return &it->second;
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
    char buf[200];

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
                    putTask(r, c, setMan, PutMode::NEW);
                }
            } else if (cmd == "country"sv) {
                std::span p1 = params;
                p1 = p1.subspan(1);
                setMan.parseCountry(p1);
            } else if (cmd == "fixup"sv) {
                if (params.size() < 2)
                   throw BadTask("/fixup should have one param");
                auto cp = parseHexTask(params[1]);
                auto pTask = putTask(r, cp, setMan, PutMode::FIXUP);
                std::span q = params;
                q = q.subspan(2);
                for (auto v : q) {
                    if (v == "ALERT"sv) {
                        pTask->alertWhenTrueCountry = true;
                    } else {
                        snprintf(buf, std::size(buf),
                                "Unknown /fixup's option '%.*s'", PRF_SV(v));
                        throw BadTask(buf);
                    }
                }
            } else if (cmd == "defaultcountry"sv) {
                std::span p1 = params;
                p1 = p1.subspan(1);
                setMan.parseDefaultCountry(p1);
            } else if (cmd == "prefix"sv) {
                std::string_view prefix = params.safeGetV(1, {});
                setMan.setPrefix(std::string{prefix});
            } else {
                throw BadTask(str::cat("Unknown command: ", cmd));
            }
        } else { // Single char
            char32_t code = parseHexTask(st);
            putTask(r, code, setMan, PutMode::NEW);
        }
    }

    if (r.empty())
        throw BadTask("No tasks for some reason");
    return r;
}
