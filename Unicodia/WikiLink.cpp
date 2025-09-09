// My header
#include "WikiLink.h"

template class std::shared_ptr<const mywiki::Link>;

void mywiki::History::trimTo(size_t wantedSize)
{
    if (size() > wantedSize) {
        links.erase(links.begin(), links.end() - wantedSize);
    }
}


void mywiki::History::push(const PHistoryLink& link)
{
    // No link? (foolproofing)
    if (!link)
        return;
    if (auto q = findIt(*link)) {
        // Found — boost
        PHistoryLink what = **q;
        links.erase(*q);
        links.push_back(std::move(what));
    } else {
        // Not found trim + push
        trim1();
        links.push_back(link);
    }
}


auto mywiki::History::findIt(const HistoryLink& x) noexcept
        -> std::optional<Vec::iterator>
{
    auto it = std::find_if(links.begin(), links.end(),
            [&x](const auto& a) {
                return a->isSame(x);
            });
    if (it == links.end())
        return std::nullopt;
    return it;
}


mywiki::HistoryPlace mywiki::History::back(TinyOpt<HistoryLink> me) const noexcept
{
    auto it = links.end();
    // 1 back
    if (it == links.begin())
        return {};
    --it;
    // Check
    if (!me || (*it)->isDifferent(*me))
        return { .thing = *it, .index = size() - 1 };
    // 2 back
    if (it == links.begin())
        return {};
    --it;
    // Check, me is not null
    if ((*it)->isDifferent(*me))  // should be OK
        return { .thing = *it, .index = size() - 2 };
    return {};  // should not happen
}


mywiki::PHistoryLink mywiki::History::extract(unsigned index)
{
    if (index >= size())
        return nullptr;
    auto r = std::move(links[index]);
    links.resize(index);
    return r;
}
