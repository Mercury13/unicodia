#pragma once

#include "tsl/robin_map.h"
#include "u_TypedFlags.h"

#include "excep.h"

enum class CountryFg : unsigned short {
    DUMMY      = 0,
    CHINA      = 1<<0,
    TAIWAN     = 1<<1,
    JAPAN      = 1<<2,
    KOREA      = 1<<3,
    N_KOREA    = 1<<4,
    VIETNAM    = 1<<5,
    MACAO      = 1<<6,
    HONGKONG   = 1<<7,
    KOREA_N    = 1<<8,
    DAIZO      = 1<<9,  ///< Japanese Buddhist texts
    CONSORTIUM = 1<<10, ///< added by Consortium
    WESTERN    = 1<<11, ///< found in Western archives
};

struct UnicodeInfo {
    Flags<CountryFg> countries;
};

using UnicodeList = tsl::robin_map<char32_t, UnicodeInfo>;

[[nodiscard]] UnicodeList readUnicodeList();

DEFINE_EXCEPTION_CLASS(BadUnicode, std::logic_error)
