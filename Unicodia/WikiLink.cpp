// My header
#include "WikiLink.h"


void mywiki::History::trimTo(size_t wantedSize)
{
    if (size() > wantedSize) {
        links.erase(links.begin(), links.end() - wantedSize);
    }
}


void mywiki::History::push(const std::shared_ptr<HistoryLink>& link)
{
    // No link? (foolproofing)
    if (!link)
        return;
    if (auto q = findIt(*link)) {
        // Found — boost
        PLink what = **q;
        links.erase(*q);
        links.push_back(std::move(what));
    } else {
        // Not found trim + push
        trim1();
        links.push_back(link);
    }
}


auto mywiki::History::findIt(const HistoryLink& x) -> std::optional<Vec::iterator>
{
    auto it = std::find_if(links.begin(), links.end(),
            [&x](const auto& a) {
                return a->isSame(x);
            });
    if (it == links.end())
        return std::nullopt;
    return it;
}
