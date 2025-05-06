#include "base.h"


void kage::Base::add(std::string x, std::string y)
{
    // m[s_v] = y  is  C++26
    m[std::move(x)] = std::move(y);
}
