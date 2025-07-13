#include "qa.h"

// C++
#include <fstream>
#include <unordered_map>

// Unicode
#include "UcData.h"

// Lib
#include "u_Strings.h"

qa::TestFonts qa::testFonts(const std::filesystem::path& fname)
{
    if (fname.empty())
        return TestFonts::EMPTY_FNAME;

    std::ofstream os(fname);
    if (!os.is_open()) {
        return TestFonts::CANNOT_CREATE;
    }

    constexpr char32_t BAD_CP = 0xFFFFFF;
    struct CurrInfo {
        char32_t first = BAD_CP, last = BAD_CP;
        std::string_view family {};
        const uc::Block* blk = nullptr;
    } currInfo;
    auto dump = [&os, &currInfo]() {
        char buf[50];
        if (currInfo.first == BAD_CP)
            return;
        // Render range
        int nDigs = (currInfo.first <= 0xFFFF) ? 4 : 5; // that’s enough, 10XXXX aren’t in ucode
        if (currInfo.first >= currInfo.last) {
            snprintf(buf, std::size(buf), "%04X..%.*s: ",
                     unsigned(currInfo.first), nDigs, ".........");
        } else {
            const char* div = (currInfo.first + 1 == currInfo.last)
                       ? ", " : "..";
            snprintf(buf, std::size(buf), "%04X%s%04X: ",
                     unsigned(currInfo.first), div, unsigned(currInfo.last));
        }
        // Render family
        auto fam = currInfo.family;
        if (fam.length() >= 5 && (
                fam.ends_with(".ttf") || fam.ends_with(".otf"))) {
            fam = fam.substr(0, fam.length() - 4);
        }
        os << buf << fam << '\n';
        currInfo.first = BAD_CP;
    };
    for (auto& cp : uc::cpInfo) {
        // Block
        const auto* newBlock = &cp.block();
        // Font
        std::string_view newFamily;
        switch (cp.drawMethod(uc::EmojiDraw::CONSERVATIVE, uc::GlyphStyleSets::EMPTY)) {
        case uc::DrawMethod::VIRTUAL_VIRAMA:
            newFamily = "[Virtual virama]"; break;
        case uc::DrawMethod::SVG_EMOJI:
            newFamily = "[SVG emoji]"; break;
        case uc::DrawMethod::ABBREVIATION:
            newFamily = "[Abbreviation]"; break;
        case uc::DrawMethod::CUSTOM_CONTROL:
            newFamily = "[Custom control]"; break;
        case uc::DrawMethod::SAMPLE:
        case uc::DrawMethod::SPACE:
        case uc::DrawMethod::MARCHEN:
        case uc::DrawMethod::SAMPLED_CONTROL:
        case uc::DrawMethod::VERTICAL_CW:
        case uc::DrawMethod::VERTICAL_CCW:
            if (auto what = cp.font(match::NullForTofu::INST)) {
                newFamily = what->family.text;
            } else {
                newFamily = "[Tofu]";
            }
        }
        if (newBlock != currInfo.blk || newFamily != currInfo.family) {
            dump();
            if (newBlock != currInfo.blk) {
                os << "== " << newBlock->name << '\n';
            }
            currInfo.first = cp.subj;
            currInfo.family = newFamily;
            currInfo.blk = newBlock;
        }
        currInfo.last = cp.subj;
    }
    dump();
    os.close();
    return TestFonts::OK;
}


namespace {

    struct Counter {
        unsigned v = 0;
    };

}   // anon namespace


std::vector<qa::BlockFontLine> qa::blockFontStats(const uc::Block& block)
{
    unsigned nNonFont = 0;
    unsigned nTofu = 0;
    std::unordered_map<std::string_view, Counter> m;
    for (auto c = block.startingCp; c <= block.endingCp; ++c) {
        auto cp = uc::cpsByCode[c];
        if (!cp)
            continue;
        switch (cp->drawMethod(uc::EmojiDraw::CONSERVATIVE, uc::GlyphStyleSets::EMPTY)) {
        case uc::DrawMethod::VIRTUAL_VIRAMA:
        case uc::DrawMethod::SVG_EMOJI:
        case uc::DrawMethod::ABBREVIATION:
        case uc::DrawMethod::CUSTOM_CONTROL:
            ++nNonFont; break;
        case uc::DrawMethod::SAMPLE:
        case uc::DrawMethod::SPACE:
        case uc::DrawMethod::MARCHEN:
        case uc::DrawMethod::SAMPLED_CONTROL:
        case uc::DrawMethod::VERTICAL_CW:
        case uc::DrawMethod::VERTICAL_CCW:
            if (auto what = cp->font(match::NullForTofu::INST)) {
                ++m[what->family.text].v;
            } else {
                ++nTofu;
            }
        }
    }
    std::vector<qa::BlockFontLine> r;
    r.reserve(m.size());
    if (nTofu != 0) {
        r.emplace_back(BlockFontSpecial::TOFU, "[Tofu]", std::string{}, nTofu);
    }
    if (nNonFont != 0) {
        r.emplace_back(BlockFontSpecial::NONFONT, "[Non-font]", std::string{}, nNonFont);
    }
    auto iNonSort = r.size();
    for (auto& v : m) {
        auto& bk = r.emplace_back(BlockFontSpecial::NORMAL, std::string{v.first}, std::string{}, v.second.v);
        if (bk.fname.ends_with(".ttf") || bk.fname.ends_with((".otf"))) {
            bk.fname.resize(bk.fname.length() - 4);
        }
        bk.sortKey = lat::toUpper(bk.fname);
    }
    std::sort(r.begin() + iNonSort, r.end(),
            [](const BlockFontLine& x, const BlockFontLine& y) {
                return x.sortKey < y.sortKey;
            });
    return r;
}
