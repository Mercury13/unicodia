#include "u_SearchEngine.h"

srh::Class srh::classify(char8_t x)
{
    if ((x >= 'A' && x <= 'Z') || (x >= 'a' && (x <= 'z')))
        return Class::LETTER;
    if (x >= '0' && x <= '9')
        return Class::DIGIT;
    return Class::OTHER;
}
