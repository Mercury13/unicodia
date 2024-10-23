#include "optstorage.h"

#include "pugixml.hpp"
#include "hash_sha256.h"

#include <filesystem>


std::string sha256Text(std::string_view data)
{
    hash_sha256 ha;
    ha.sha256_init();
    ha.sha256_update(reinterpret_cast<const std::uint8_t*>(data.data()), data.size());
    auto binaryHash = ha.sha256_final();

    std::string r;
    r.reserve(binaryHash.size() * 2);
    char buf[10];
    for (auto q : binaryHash) {
        snprintf(buf, std::size(buf), "%02x", q);
        r += q;
    }
    return r;
}


void OptStorage::clear()
{
    data.clear();
    isModified = false;
}


void OptStorage::readXml(const char* fname)
{
    clear();
    pugi::xml_document doc;
    if (doc.load_file(fname)) {
        auto hRoot = doc.root().child("files");
        for (auto hFile : hRoot.children("file")) {
            std::string name = hFile.attribute("name").as_string();
            if (!name.empty()) {
                auto& info = data[name];
                info.length = hFile.attribute("size").as_uint();
                info.sha256text = hFile.attribute("sha256").as_string();
            }
        }
    }
}


void OptStorage::removeUntouched()
{
    for (auto it = data.begin(); it != data.end(); ) {
        if (!it->second.isTouched) {
            it = data.erase(it);
            isModified = true;
        } else {
            ++it;
        }
    }
}


void OptStorage::writeXml(const char* fname)
{
    pugi::xml_document doc;
    auto hFiles = doc.append_child("files");
    for (auto& [k, v] : data) {
        auto hFile = hFiles.append_child("file");
        hFile.append_attribute("name") = k.c_str();
        hFile.append_attribute("size") = v.length;
        hFile.append_attribute("sha256") = v.sha256text.c_str();
    }
    doc.save_file(fname);
}


constexpr const char* COMMAND =
        "--coordinates-precision=2 --properties-precision=2 "
        "--transforms-precision=4 --paths-coordinates-precision=2 --multipass";


OptResult OptStorage::checkFile(const char* fname)
{
    if (!std::filesystem::exists(fname)) {
        data.erase(fname);
        return OptResult::NOT_FOUND;
    }
    auto fp = std::fopen(fname, "rb");
    if (!fp) {
        data.erase(fname);
        return OptResult::NOT_FOUND;
    }
    std::fseek(fp, 0u, SEEK_END);
    auto size = std::ftell(fp);
    if (size == 0) {
        data.erase(fname);
        std::fclose(fp);
        return OptResult::NOT_FOUND;
    }
    std::fseek(fp, 0u, SEEK_SET);
    std::string content(size, '\0');
    std::fread(&content[0], 1u, size, fp);
    std::fclose(fp);

    auto computedSha256 = sha256Text(content);
    auto& info = data[fname];
    info.isTouched = true;
    if (info.is(size, computedSha256)) {
        info.content = content;
        return OptResult::ALREADY_OPTIMIZED;
    }

    // Run optimizer

    isModified = true;
    return OptResult::OPTIMIZER_RAN;
}
