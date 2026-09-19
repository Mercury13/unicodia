#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <optional>

struct BadFile {
    std::wstring name;
    bool isValid = false;
    struct Discrep {
        bool hasClipPath = false;
        bool hasGradientXlink = false;
        bool hasRgbaColor = false;

        static const Discrep EMPTY;
        bool operator == (const Discrep&) const = default;
        bool hasAny() const { return (*this != EMPTY); }
    } discrep;
};

struct Result {
    std::vector<std::wstring> good;
    std::vector<BadFile> bad;
};

std::optional<BadFile> processFile(const std::filesystem::path& fname);
Result processDir(const std::filesystem::path& dir);
