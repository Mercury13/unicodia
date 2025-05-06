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

    inline int roundDiv(int x, int y)
    {
        return std::lround(static_cast<kage::Float>(x) / y);
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
        /// @todo [urgent] maybe use fixed or float?
        p = roundDiv((p - p1) * (p4 - p3), (p2 - p1)) + p3;
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
        auto scalePair = [x1, y1, dx = x2 - x1, dy = y2 - y1](int& x, int& y) {
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
        Structure r;
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
                  // 99:0:0:0:0:200:200:new_id
                  // 0  1 2 3 4 5   6   7        8 9 10
                if (v1 == 0 && v2 == 0 && v3 == 0 && v4 == 0
                        && v5 == 200 && v6 == 200 && v9 == 0 && v10 == 0) {
                    // Simple addition
                    if (lines.size() == 1) {
                        // Dumb link detected (A is same as B),
                        // they are 50% of GlyphWiki
                        entry.cache = data;
                        return entry.cache;
                    }
                    r.insert(r.end(), data->begin(), data->end());
                } else {
                    // Scaled
                    appendScaled(r, *data, v3, v4, v5, v6, v1, v2, v9, v10);
                }
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
                r.emplace_back(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10);
            }
        }
        // Moving to dynamic memory (simple operation)
        entry.cache = std::make_shared<Structure>(std::move(r));
    }
    return entry.cache;
}


kage::Drawing kage::Base::drawingOf(std::string_view x, const Style& style) const
{
    auto struc = structureOf(x);
    return style.draw(*struc);
}
