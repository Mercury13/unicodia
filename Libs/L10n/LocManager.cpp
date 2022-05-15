#include "LocManager.h"


loc::Manager loc::man;


void loc::Manager::translateMe()
{
    for (auto v : children) {
        if (v)
            v->translateMe();
    }
}
