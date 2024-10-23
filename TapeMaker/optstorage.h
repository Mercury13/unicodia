#pragma once

#include <string>
#include <map>
#include <filesystem>

struct OptInfo
{
    unsigned length = 0;
    std::string sha256text;
    std::string content;
    bool isTouched = false;

    bool is(unsigned aLength, std::string_view aSha256) const noexcept {
        return (length == aLength && sha256text == aSha256);
    }
};

enum class OptStatus : unsigned char {
    NOT_FOUND,              ///< No file, zero in length, cannot open
    ALREADY_OPTIMIZED,
    OPTIMIZER_IN_VAIN,
    OPTIMIZER_SUCCESS
};

struct OptResult {
    OptStatus status;
    const OptInfo* info;
};

struct OptStorage
{
    /// Need some order
    std::map<std::string, OptInfo> data;
    std::filesystem::path pathToOptimizer;
    bool isModified = false;

    /// @return [+] optimizer found
    bool findOptimizer();

    void clear();
    bool isEmpty() const noexcept { return data.empty(); }
    bool hasOptimizer() const noexcept { return !pathToOptimizer.empty(); }
    void readXml(const char* fname);
    void removeUntouched();
    void writeXml(const char* fname);
    OptResult checkFile(const char* fname);
};
