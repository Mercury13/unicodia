#include "kagelist.h"

#include "u_Strings.h"
#include "u_Vector.h"

class LineTraverser
{
public:
    LineTraverser(std::string_view x, unsigned wantedSize);
    bool get(std::string_view& r) noexcept;
    size_t size() const noexcept { return lines.size(); }
    size_t remainder() const noexcept;
private:
    SafeVector<std::string_view> lines;
    size_t index = 0;
};

LineTraverser::LineTraverser(std::string_view x, unsigned wantedSize)
{
    // true is OK
    lines.reserve(wantedSize);
    str::splitByAnySvTo(x, "\r\n", lines, true);
}

bool LineTraverser::get(std::string_view& r) noexcept
{
    if (index >= lines.size()) {
        r = std::string_view{};
        return false;
    }
    r = lines[index++];
    return true;
}

size_t LineTraverser::remainder() const noexcept
{
    if (index > size())
        return 0;
    return size() - index;
}


size_t readKageList(KageList& r, KageCache& cache, unsigned wantedSize, const char* fname)
{
    if (!cache.data.empty())
        throw std::invalid_argument("You are using full cache, that's error-prone");

    auto oldSize = r.size();
    {
        std::ifstream is(fname, std::ios::binary);
        if (!is.is_open()) {
            char buf[200];
            snprintf(buf, std::size(buf), "Need file '%s', part of GlyphWiki dump", fname);
            throw std::logic_error(buf);
        }

        // Read big piece of data
        is.seekg(0, std::ios::end);
        size_t size = is.tellg();
        is.seekg(0, std::ios::beg);
        cache.data.resize(size);
        is.read(cache.data.data(), size);
        std::cout << "In mem..." << std::flush;
    }

    std::string_view s;
    LineTraverser tr(cache.data, wantedSize);
    // Parse header
    while (tr.get(s)) {
        if (s.starts_with("--")) {
            goto headerEnd;
        }
    }
    throw StrangeDump("Did not find where header ends");
headerEnd:
    r.rehash(r.size() + tr.remainder());
    std::cout << "Going..." << std::flush;
    // Parse normal lines
    while (tr.get(s)) {
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
                    auto sz = r.size() - oldSize;
                    if (value != sz)
                        throw StrangeDump("Number of lines mismatch");
                    goto dumpEnd;
                }
            } [[fallthrough]];
        default:
            throw StrangeDump(str::cat("Not three columns: ", s));
        }
        std::string key { cols[0] };
        str::replace(key, "\\@", "@");
        auto value = cols[2];
        auto [_, wasIns] = r.try_emplace(std::move(key), value);
        if (!wasIns)
            throw StrangeDump("Key repeats: " + key);
    }
dumpEnd:
    auto sz = r.size() - oldSize;
    if (sz == 0)
        throw StrangeDump("No data for some reason");
    return sz;
}
