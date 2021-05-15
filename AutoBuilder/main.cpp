// C++
#include <iostream>
#include <fstream>

// PugiXML
#include "pugixml.hpp"

// Project-local
#include "data.h"

template <class T>
inline auto need(T&& val, const char* errmsg)
{
    if (!val) throw std::logic_error(errmsg);
    return std::forward<T>(val);
}

int main()
{
    std::ofstream os("UcAuto.cpp");
    os << "// Automatically generated, do not edit!" << '\n';
    os << '\n';
    os << R"(#include "UcData.h")" << '\n';
    os << '\n';
    os << R"(using namespace std::string_view_literals;)" << '\n';
    os << '\n';

    pugi::xml_document doc;

    std::cout << "Loading Unicode base..." << std::flush;
    doc.load_file("ucd.all.flat.xml");
    std::cout << "OK" << std::endl;

    ///// CpInfo ///////////////////////////////////////////////////////////////

    unsigned nChars = 0, nSpecialRanges = 0;

    auto elRoot = need(doc.root().child("ucd"), "Need <ucd>");
    auto elRepertoire = need(elRoot.child("repertoire"), "Need <repertoire>");
    std::cout << "Found repertoire, generating character info..." << std::flush;
    os << '\n';
    os << R"(uc::Cp uc::cpInfo[] {)" << '\n';

    for (pugi::xml_node elChar : elRepertoire.children("char")) {
        std::string_view sCp = elChar.attribute("cp").as_string();
        if (sCp.empty()) {
            ++nSpecialRanges;
            continue;
        }
        /// @todo [urgent] Some characters have descriptions in other places
        /// @todo [future] Sometimes we have fixups, what to take?
        /// @todo [future] Sometimes we have abbreviations, take them
        std::string_view sName = elChar.attribute("na").as_string();
        std::string sLowerName = decapitalize(sName);
        os << "{ "
           << "0x" << sCp << ", "       // subj
           << "0x" << sCp << ", "       // proxy
           << "{ "                      // name
                << '\"' << sLowerName << R"("sv, )"  // name.tech,
                << R"(""sv)"                    // name.loc
           << " }"
           << " }," << '\n';
        ++nChars;
    }

    os << "};" << '\n';

    std::cout << "OK" << std::endl;
    std::cout << "Found " << nChars << " chars, " << nSpecialRanges << " special ranges " << std::endl;

    os << "unsigned uc::nCps() { return " << nChars << "; }" << '\n';

    std::cout << "Successfully finished!" << std::endl << std::endl;

    return 0;
}
