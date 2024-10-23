#include "optstorage.h"

// Libs
#include "pugixml.hpp"
#include "hash_sha256.h"
#include "u_Strings.h"

// C++
#include <cstdlib>
#include <stdexcept>

#ifdef _WIN32
    #include <shlobj.h>
#endif


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
        snprintf(buf, std::size(buf), "%02x", (unsigned)q);
        r += buf;
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


bool OptStorage::findOptimizer()
{
    static constexpr std::string_view MYFNAME = "svgcleaner-cli.exe";
    pathToOptimizer.clear();
    std::filesystem::path newPath = MYFNAME;
    for (int i = 0; ; ++i) {
        if (std::filesystem::exists(newPath)) {
            pathToOptimizer = std::move(newPath);
            return true;
        }
        if (i >= 4)
            break;
        newPath = ".." / newPath;
    }

#ifdef _WIN32
    {
        // Copied from header
        static constinit const GUID PROGRAM_FILES_X86 {
            0x7c5a40ef, 0xa0fb, 0x4bfc, { 0x87, 0x4a, 0xc0, 0xf2, 0xe0, 0xb9, 0xfa, 0x8e } };
        PWSTR folder = nullptr;
        auto q = SHGetKnownFolderPath(
                    PROGRAM_FILES_X86,
                    KF_FLAG_DEFAULT,
                    nullptr,
                    &folder);
        if (q == S_OK) {
            newPath = folder;
            newPath /= "SVG Cleaner";
            newPath /= MYFNAME;
            CoTaskMemFree(folder);
            if (std::filesystem::exists(newPath)) {
                pathToOptimizer = std::move(newPath);
                return true;
            }
        }
    }
#endif

    return false;
}


constexpr const char* COMMAND =
        "--coordinates-precision=2 --properties-precision=2 "
        "--transforms-precision=4 --paths-coordinates-precision=2 --multipass "
        "--copy-on-error";


OptResult OptStorage::checkFile(const char* fname)
{
    if (!std::filesystem::exists(fname)) {
        data.erase(fname);
        return { .status = OptStatus::NOT_FOUND, .info = nullptr };
    }
    auto fp = std::fopen(fname, "rb");
    if (!fp) {
        data.erase(fname);
        return { .status = OptStatus::NOT_FOUND, .info = nullptr };
    }
    std::fseek(fp, 0u, SEEK_END);
    auto size = std::ftell(fp);
    if (size == 0) {
        data.erase(fname);
        std::fclose(fp);
        return { .status = OptStatus::NOT_FOUND, .info = nullptr };
    }
    std::fseek(fp, 0u, SEEK_SET);
    std::string content(size, '\0');
    std::fread(content.data(), 1u, size, fp);
    std::fclose(fp);

    auto computedSha256 = sha256Text(content);

    // Get naked name
    auto nakedName = fname;
    for (auto it = fname; *it != 0; ++it) {
        switch (*it) {
        case '/':
        case '\\':
            nakedName = it + 1;
            break;
        default: ;
        }
    }

    auto& info = data[nakedName];
    info.isTouched = true;
    if (info.is(size, computedSha256)) {
        info.content = content;
        return { .status = OptStatus::ALREADY_OPTIMIZED, .info = &info };
    }

    // Run optimizer
    if (pathToOptimizer.empty()) {
        throw std::logic_error(str::cat(
            "File ", fname, " is not optimized, and optimizer was not found"));
    }
    static constexpr const char* TEMPFNAME = "~temp.svg";
    std::cout << "NOTE: optimizing " << fname << '\n';
    auto command = str::cat('"', str::toSv(pathToOptimizer.u8string()),
                            "\" ", fname, ' ', TEMPFNAME, ' ', COMMAND);
    std::system(command.c_str());
    if (!std::filesystem::exists(TEMPFNAME)) {
        throw std::logic_error(str::cat("Optimizer did not create ", TEMPFNAME));
    }

    // Read file once again
    fp = std::fopen(TEMPFNAME, "rb");
    if (!fp) {
        throw std::logic_error(str::cat("Cannot open <", fname, "> once again"));
    }
    std::fseek(fp, 0u, SEEK_END);
    size = std::ftell(fp);
    if (size == 0) {
        std::fclose(fp);
        throw std::logic_error(str::cat("File <", fname, "> is empty"));
    }
    std::fseek(fp, 0u, SEEK_SET);
    std::string newContent(size, '\0');
    std::fread(newContent.data(), 1u, size, fp);
    std::fclose(fp);

    // Fill new info
    info.content = newContent;
    info.length = size;
    info.sha256text = sha256Text(newContent);

    if (newContent == content) {
        std::cout << "NOTE: " << fname << " was not touched by optimizer" "\n";
        remove(TEMPFNAME);
        return { .status = OptStatus::OPTIMIZER_IN_VAIN, .info = &info };
    }

    remove(fname);
    rename(TEMPFNAME, fname);

    isModified = true;
    return { .status = OptStatus::OPTIMIZER_SUCCESS, .info = &info };
}
