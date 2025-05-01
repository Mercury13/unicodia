// STL
#include <iostream>
#include <unordered_map>
#include <fstream>

// Libs
#include "u_Strings.h"

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
        auto eqr = r.equal_range(key);
        if (eqr.first != eqr.second)
            throw StrangeDump("Key repeats: " + key);
        r.emplace_hint(eqr.first, KageList::value_type{ std::move(key), value });
    }
dumpEnd:
    if (r.empty())
        throw StrangeDump("No data for some reason");
    return r;
}


void writeError(const char* header, const std::exception& e)
{
    std::cout << "\n\n" << header << ": " << e.what() << '\n';
}

int main()
{
    try {
        std::cout << "Reading Kage list..." << std::flush;
        auto kageList = readKageList();
        std::cout << "OK, " << kageList.size() << " entries" "\n";

        std::cout << "Success!" "\n";
    } catch (const BadData& e) {
        writeError("BAD DATA", e);
    } catch (const StrangeDump& e) {
        writeError("STRANGE DUMP", e);
    } catch (const std::exception& e) {
        writeError("ERROR", e);
    }
    return 0;
}
