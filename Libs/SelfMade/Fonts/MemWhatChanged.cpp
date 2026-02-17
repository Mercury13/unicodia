#include "MemWhatChanged.h"

std::vector<mf::ChangeLine> mf::whatChanged(
        const MemFont& ol, const MemFont& nw)
{
    std::vector<mf::ChangeLine> r;
    r.emplace_back(1, ChangeAction::ADD);
    r.emplace_back(2, ChangeAction::DEL);
    r.emplace_back(3, ChangeAction::CHG);
    return r;
}
