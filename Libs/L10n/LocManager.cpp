#include "LocManager.h"

template class std::list<loc::Subject*>;
template class std::unordered_map<loc::Subject*, loc::Manager::List::iterator>;

loc::Manager loc::man;


///// Manager //////////////////////////////////////////////////////////////////


void loc::Manager::translateMe()
{
    for (auto v : inOrder) {
        if (v)
            v->translateMe();
    }
    wasEverTranslated = true;
}


void loc::Manager::add(Subject& x)
{
    // Exists?
    if (bySubj.contains(&x))
        return;

    // Add
    auto it = inOrder.insert(inOrder.end(), &x);
    bySubj[&x] = it;

    // Translate
    if (wasEverTranslated)
        x.translateMe();
}


bool loc::Manager::erase(Subject* x)
{
    if (doesErase) {
        if (auto it = bySubj.find(x); it != bySubj.end()) {
            inOrder.erase(it->second);
            bySubj.erase(it);
            return true;
        }
    }
    return false;
}
