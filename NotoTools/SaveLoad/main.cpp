#include "pugixml.hpp"
#include "Noto/xml.h"

const char* FNAME = "G_hU_e.fina.glif";

int main()
{
    pugi::xml_document doc;
    doc.load_file(FNAME);

    noto::saveGlif(doc, FNAME);
}
