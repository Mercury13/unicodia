#include "base.h"

#include "defs.h"
#include "util.h"

#include "u_Strings.h"


void kage::Base::add(std::string x, std::string y)
{
    if (x.empty()) {
        throw std::logic_error("[Base.add] Empty key is forbidden");
    }
    if (str::isBlank(x.front()) || str::isBlank(x.back())) {
        throw std::logic_error("Untrimmed key is forbidden");
    }
    // m[s_v] = y  is  C++26
    m[std::move(x)] = std::move(y);
}

namespace {

    int parseInt(std::string_view key, std::span<const std::string_view> cols, unsigned index)
    {
        if (index >= cols.size())
            return 0;
        auto text = cols[index];
        if (text.empty())
            return 0;
        int q;
        auto res = std::from_chars(std::to_address(text.begin()), std::to_address(text.end()), q);
        if (res.ec != std::errc()) {
            char buf[200];
            snprintf(buf, std::size(buf),
                     "Bad Kage data in '%.*s': '%.*s is not integer",
                     int(key.length()), key.data(),
                     int(text.length()), text.data());
            throw std::logic_error(buf);
        }
        return q;
    }

    void doStretch(int dp, int sp, int& p, int min, int max)
    {
        int p1, p2, p3, p4;
        if (p < sp + 100) {
            p1 = min;
            p3 = min;
            p2 = sp + 100;
            p4 = dp + 100;
        } else {
            p1 = sp + 100;
            p3 = dp + 100;
            p2 = max;
            p4 = max;
        }
        p = ((p - p1) * (p4 - p3) / (p2 - p1)) + p3;
    }

    void appendScaled(kage::Structure& dest,
                      std::span<const kage::Stroke> src,
                      int x1, int y1, int x2, int y2, int sx, int sy, int sx2, int sy2)
    {
        auto box = getBoundingBox(src);
        if (sx != 0 || sy != 0) {
            if (sx > 100) {
                sx -= 200;
            } else {
                sx2 = 0;
                sy2 = 0;
            }
        }
        auto scalePair = [x1, y1, dx = x2 - x1, dy = y2 = y1](int& x, int& y) {
            x = x1 + x * dx / 200;
            y = y1 + y * dy / 200;
        };
        dest.reserve(dest.size() + src.size());
        for (kage::Stroke v : src) {  // by value!
            if (sx != 0 || sy != 0) {
                doStretch(sx, sx2, v.v3, box.minX, box.maxX);
                doStretch(sy, sy2, v.v4, box.minY, box.maxY);
                doStretch(sx, sx2, v.v5, box.minX, box.maxX);
                doStretch(sy, sy2, v.v6, box.minY, box.maxY);
                if (v.type != kage::stroke::REFERENCE) {
                    doStretch(sx, sx2, v.v7, box.minX, box.maxX);
                    doStretch(sy, sy2, v.v8, box.minY, box.maxY);
                    doStretch(sx, sx2, v.v9, box.minX, box.maxX);
                    doStretch(sy, sy2, v.v10, box.minY, box.maxY);
                }
            }
            scalePair(v.v3, v.v4);
            scalePair(v.v5, v.v6);
            scalePair(v.v7, v.v8);
            scalePair(v.v9, v.v10);
            dest.push_back(v);
        }
    }

}   // anon namespace


std::shared_ptr<const kage::Structure> kage::Base::detectDumbLink(
        std::string_view key,
        std::span<const std::string_view> x) const
{
    static constexpr std::string_view PREFIX = "99:0:0:0:0:200:200:";
                                            //  0  1 2 3 4 5   6
                                            // 7=0, 8=1, 9=2, 10=3
    static constexpr auto PREFIX_L = PREFIX.length();
    if (x.size() != 1)
        return {};
    auto line = x[0];
    if (!line.starts_with(PREFIX))
        return {};
    auto remder = line.substr(PREFIX_L);
    auto things = str::splitSv(remder, ':');
    // Should be key2 (7)
    auto key2 = things.safeGetV(0, {});
    if (key2.empty()) {
        char buf[200];
        snprintf(buf, std::size(buf),
                 "Kage character '%.*s' has empty reference",
                 int(key.length()), key.data());
        throw std::logic_error(buf);
    }
    // 9 and 10 (2 and 3), if present, should be 0’s
    if (parseInt(key, things, 2) != 0 || parseInt(key, things, 3) != 0)
        return {};
    return structureOf(key2);
}

auto kage::Base::structureOf(std::string_view x) const -> std::shared_ptr<const Structure>
{
    auto q = m.find(x);
    if (q == m.end()) {
        char buf[200];
        snprintf(buf, std::size(buf),
                 "Kage character '%.*s' missing",
                 int(x.length()), x.data());
        throw std::logic_error(buf);
    }
    char buf[200];
    auto& entry = q->second;
    if (!entry.cache) {
        auto lines = str::splitSv(entry.text, '$');
        if (auto link = detectDumbLink(x, lines)) {
            // Dumb links are
            entry.cache = link;
        } else {
            auto r = std::make_shared<Structure>();
            for (auto& v : lines) {
                auto cols = str::splitSv(v, ':', false);
                auto v0 = parseInt(x, cols, 0);
                if (v0 == stroke::REFERENCE) {
                    auto dest = cols.safeGetV(7, {});
                    if (dest.empty()) {
                        snprintf(buf, std::size(buf),
                                 "Kage character '%.*s' has empty reference",
                                 int(x.length()), x.data());
                        throw std::logic_error(buf);
                    }
                    auto data = structureOf(dest);
                    auto v1  = parseInt(x, cols, 1);
                    auto v2  = parseInt(x, cols, 2);
                    auto v3  = parseInt(x, cols, 3);
                    auto v4  = parseInt(x, cols, 4);
                    auto v5  = parseInt(x, cols, 5);
                    auto v6  = parseInt(x, cols, 6);
                    // 7, 8 unused
                    auto v9  = parseInt(x, cols, 9);
                    auto v10 = parseInt(x, cols, 10);
                    appendScaled(*r, *data, v3, v4, v5, v6, v1, v2, v9, v10);
                } else {
                    auto v1  = parseInt(x, cols, 1);
                    auto v2  = parseInt(x, cols, 2);
                    auto v3  = parseInt(x, cols, 3);
                    auto v4  = parseInt(x, cols, 4);
                    auto v5  = parseInt(x, cols, 5);
                    auto v6  = parseInt(x, cols, 6);
                    auto v7  = parseInt(x, cols, 7);
                    auto v8  = parseInt(x, cols, 8);
                    auto v9  = parseInt(x, cols, 9);
                    auto v10 = parseInt(x, cols, 10);
                    r->emplace_back(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10);
                }
            }
            entry.cache = r;
        }
    }
    return entry.cache;
}


kage::Drawing kage::Base::drawingOf(std::string_view x, const Style& style) const
{
    auto struc = structureOf(x);
    return style.draw(*struc);
}
