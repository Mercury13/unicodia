#include "entities.h"

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

#include "u_Array.h"

namespace {

    class AutoFile
    {
    public:
        FILE* fp = nullptr;

        AutoFile(FILE* x) noexcept : fp(x) {}
        ~AutoFile() { close(); }
        void close() noexcept;
        operator FILE* () const { return fp; }
    };

    void AutoFile::close() noexcept
    {
        if (fp) {
            fclose(fp);
            fp = nullptr;
        }
    }

    bool isSmall(std::string_view s) noexcept
    {
        for (auto c : s) {
            if (c >= 'A' && c <= 'Z')
                return false;
        }
        return true;
    }

    bool moveBest(std::vector<std::string>& list, size_t i0)
    {
        // Check size
        if (i0 + 1 >= list.size())
            return false;
        // Find best
        struct Best {
            size_t index = 0;
            bool isSmall = false;
            std::string_view s = "qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq";
        } best;
        for (size_t i = i0; i < list.size(); ++i) {
            auto& s = list[i];
            if (s.length() <= best.s.length()) {
                bool isSm = isSmall(s);
                if ((isSm && !best.isSmall) || (s.length() < best.s.length())) {
                    best.index = i;
                    best.s = s;
                    best.isSmall = isSm;
                }
            }
        }
        // Best is capital + non-0 index — BAD
        if (!best.isSmall && i0 > 0)
            return false;
        // Best to front
        if (best.index > i0) {
            std::string tmp { best.s };
            list.erase(list.begin() + best.index);
            list.insert(list.begin() + i0, tmp);
            return true;
        }
        return false;
    }

}   // anon namespace


Entities readEntities(const char* fname)
{
    Entities r;

    { AutoFile af = fopen(fname, "r");
        char buffer[1024 * 64];
        rapidjson::FileReadStream frs(af, buffer, std::size(buffer));
        rapidjson::Document doc;
        doc.ParseStream(frs);
        for (auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it) {
            if (!it->name.IsString() || !it->value.IsObject())
                continue;
            std::string_view entity = it->name.GetString();
            if (!entity.starts_with('&') || !entity.ends_with(';')) {
                ++r.stats.entities.nWrong;
                continue;
            }
            auto obj = it->value.GetObject();
            auto itCps = obj.FindMember("codepoints");
            if (itCps == obj.MemberEnd() || !itCps->value.IsArray())
                continue;
            auto cpArr = itCps->value.GetArray();
            if (cpArr.Size() != 1) {
                ++r.stats.entities.nTooLong;
                continue;
            }
            auto it0 = cpArr.Begin();
            if (!it0->IsInt())
                continue;
            r.data[it0->GetInt()].emplace_back(entity);
            ++r.stats.entities.nGood;
        }
    }

    // The simplest → to front
    for (auto& v : r.data) {
        auto& list = v.second;
        size_t i0 = 0;
        while (moveBest(list, i0)) {
            ++i0;
        }
    }

    return r;
}
