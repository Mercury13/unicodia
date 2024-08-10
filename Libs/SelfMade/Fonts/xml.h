#pragma once

#include "pugixml.hpp"
#include <filesystem>

namespace noto {

    void saveGlif(pugi::xml_document& doc, const std::filesystem::path& fname);

}   // namespace noto
