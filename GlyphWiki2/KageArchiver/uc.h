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
    VIETNAM    = 1<<4,
    MACAO      = 1<<5,
    HONGKONG   = 1<<6,
    KOREA_N    = 1<<7,
    DAIZO      = 1<<8,  ///< Japanese Buddhist texts
    CONSORTIUM = 1<<9,  ///< added by Consortium
    WESTERN    = 1<<10, ///< found in Western archives
    BIG_DUMMY,
    BIGGEST = BIG_DUMMY - 1,
    ALL = (BIGGEST<<1) - 1,
};

struct UnicodeInfo {
    Flags<CountryFg> countries;
};

using UnicodeList = tsl::robin_map<char32_t, UnicodeInfo>;

[[nodiscard]] UnicodeList readUnicodeList();

DEFINE_EXCEPTION_CLASS(BadUnicode, std::logic_error)
