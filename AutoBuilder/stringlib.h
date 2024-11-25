#pragma once

///
/// String library that collects strings and writes them
///

// STL
#include <string>
#include <set>
#include <deque>

// Unicode
#include "UcFlags.h"


struct StringData
{
    std::string s;
    char32_t subj;
    int offset;
    uc::TextRole role;
    bool isLast;
};

struct RememberResult
{
    StringData* data;
    bool wasIns;
};

class StringLib
{
public:
    RememberResult remember(
            char32_t subj,
            uc::TextRole role,
            std::string_view s);
    void forceRemember(
            StringData*& r,
            char32_t subj,
            uc::TextRole role,
            std::string_view s);
    void finishCp();
    auto& inOrder() const { return fInOrder; }
    auto& nonAscii() const { return fNonAscii; }
    void write(std::ostream& os);
private:
    std::deque<StringData> fInOrder;
    size_t fLength = 1;     // 0 here
    std::set<char32_t> fNonAscii;
    RememberResult rememberInner(
            char32_t subj,
            uc::TextRole role,
            std::string_view s);
};
