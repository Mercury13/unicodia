#include "MemWhatChanged.h"

// STL
#include <map>

// Fonts
#include "MemFont.h"


namespace {

    using MCp = std::map<char32_t, unsigned>;

    class Cb {
    public:
        Cb(MCp& x) noexcept : m(x) {}
        void operator ()(char32_t cp, unsigned glyph) const
            { m[cp] = glyph; }
    private:
        MCp& m;
    };

}   // anon namespace


std::vector<mf::ChangeLine> mf::whatChanged(
        const MemFont& ol, const MemFont& nw)
{
    std::vector<mf::ChangeLine> r;
    MCp oldCps, newCps;

    ol.traverseCps(Cb(oldCps));
    nw.traverseCps(Cb(newCps));

    auto itOld = oldCps.begin(), itNew = newCps.begin();
    while (itOld != oldCps.end() && itNew != newCps.end()) {
        auto oldCp = itOld->first;
        auto newCp = itNew->first;
        if (oldCp == newCp) {
            /// @todo [urgent] track change
            ++itOld; ++itNew;
        } else if (oldCp < newCp) {
            r.emplace_back(oldCp, ChangeAction::DEL);
            ++itOld;
        } else {  // newCp < oldCp
            r.emplace_back(newCp, ChangeAction::ADD);
            ++itNew;
        }
    }
    return r;
}
