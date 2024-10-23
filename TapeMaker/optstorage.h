#pragma once

#include <string>
#include <map>

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

enum class OptResult : unsigned char {
    NOT_FOUND,              ///< No file, zero in length, cannot open
    ALREADY_OPTIMIZED,
    OPTIMIZER_RAN
};

struct OptStorage
{
    std::map<std::string, OptInfo> data;
    bool isModified = false;

    void clear();
    bool isEmpty() const noexcept { return data.empty(); }
    void readXml(const char* fname);
    void removeUntouched();
    void writeXml(const char* fname);
    OptResult checkFile(const char* fname);
};
