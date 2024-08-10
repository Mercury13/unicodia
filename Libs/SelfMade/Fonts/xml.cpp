// My header
#include "xml.h"

// STL
#include <sstream>
#include <fstream>

// My libs
#include "u_Strings.h"

void noto::saveGlif(pugi::xml_document& doc, const std::filesystem::path& fname)
{
    pugi::xml_node decl = doc.prepend_child(pugi::node_declaration);
    decl.append_attribute("version") = "1.0";
    decl.append_attribute("encoding") = "UTF-8";

    std::ostringstream os;
    doc.save(os, "  ",
             pugi::format_default | pugi::format_write_bom,
             pugi::encoding_utf8);

    auto s = os.str();
    str::replace(s, " />", "/>");

    if (s.ends_with("\r\n"))
        s.resize(s.length() - 2);
    if (s.ends_with("\n"))
        s.resize(s.length() - 1);

    std::ofstream os1(fname);
    os1 << s << std::endl;
}
