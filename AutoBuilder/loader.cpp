// My header
#include "loader.h"

// C++
#include <cstdlib>


bool checkLoader()
{
    return false;
}


void runLoader()
{
    system("curl --output ucd.all.flat.zip https://www.unicode.org/Public/14.0.0/ucdxml/ucd.all.flat.zip");
}
