#pragma once

// Maps
#include "tsl/robin_map.h"

// Project-local
#include "excep.h"

class Shash : public std::hash<std::string_view>
{
private:
    using Super = std::hash<std::string_view>;
public:
    using is_transparent = void;
    using Super::operator ();
};

using KageList = tsl::robin_map<std::string, std::string_view, Shash, std::equal_to<>>;

struct KageCache {
    std::string data;
};

DEFINE_EXCEPTION_CLASS(StrangeDump, std::logic_error)

///
/// @brief readKageList
/// @param r              KageList that accommodates both current and old hieros
/// @param cache          cache where s_v’s are stored
/// @param wantedSize     a little bit bigger than actual size in lines
/// @param fname          fname for Kage DB dump
/// @return   # of entries read
///
size_t readKageList(KageList& r, KageCache& cache, unsigned wantedSize, const char* fname);
