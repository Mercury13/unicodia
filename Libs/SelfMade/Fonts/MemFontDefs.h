#pragma once

namespace mf {

    enum class Plat : unsigned short {
        UCODE = 0,
        MAC = 1,
        ISO = 2,
        WIN = 3,
        CUSTOM = 4,
        N = 5,
        REALLY_BAD = 55555,
    };
    constexpr auto PLAT_N  = (unsigned)Plat::N;
    constexpr auto PLAT_N1 = (unsigned)Plat::CUSTOM;

    enum class Enc : unsigned short {
        // Unicode
        UCODE_1_0 = 0,
        UCODE_1_1 = 1,
        UCODE_ISO_10646 = 2,
        UCODE_BMP = 3,
        UCODE_FULL = 4,
        UCODE_VS_14 = 5,
        UCODE_FULL_13 = 6,
        UCODE_N = 7,
        // Mac
        MAC_ROMAN = 0,
        MAC_JAPN = 1,
        MAC_CHINESE_TRAD = 2,
        MAC_KORE = 3,
        MAC_ARAB = 4,
        MAC_HEBR = 5,
        MAC_GREK = 6,
        MAC_RUSSIAN = 7,
        MAC_RSYMBOL = 8,
        MAC_DEVA = 9,
        MAC_GURU = 10,
        MAC_GUJR = 11,
        MAC_ORYA = 12,
        MAC_BENG = 13,
        MAC_TAML = 14,
        MAC_TELU = 15,
        MAC_KNDA = 16,
        MAC_MLYM = 17,
        MAC_SINH = 18,
        MAC_MYMR = 19,
        MAC_KHMR = 20,
        MAC_THAI = 21,
        MAC_LAOO = 22,
        MAC_GEOR = 23,
        MAC_ARMN = 24,
        MAC_CHINESE_SIMP = 25,
        MAC_TIBT = 26,
        MAC_MONG = 27,
        MAC_ETHI = 28,
        MAC_SLAVIC = 29,
        MAC_VIETNAMESE = 30,
        MAC_SINDHI = 31,
        MAC_UNINTERPRETED = 32,
        MAC_N = 33,
        // ISO
        ISO_ASCII = 0,
        ISO_10646 = 1,
        ISO_1 = 2,  ///< Encoding known as ISO-1
        ISO_N = 3,
        // Windows
        WIN_SYMBOL = 0,
        WIN_UNICODE_BMP = 1,
        WIN_SHIFT_JIS = 2,
        WIN_PRC = 3,
        WIN_BIG5 = 4,
        WIN_WANSUNG = 5,
        WIN_JOHAB = 6,
        WIN_RESERVED_7 = 7,
        WIN_RESERVED_8 = 8,
        WIN_RESERVED_9 = 9,
        WIN_UNICODE_FULL = 10,
        WIN_N = 11,
        // Really bad
        REALLY_BAD = 55555,
    };
    constexpr auto ENC_UNICODE_N = (unsigned)Enc::UCODE_N;
    constexpr auto ENC_MAC_N = (unsigned)Enc::MAC_N;
    constexpr auto ENC_ISO_N = (unsigned)Enc::ISO_N;
    constexpr auto ENC_WIN_N = (unsigned)Enc::WIN_N;

    enum class TableFormat : unsigned short {
        SINGLE_BYTE = 0,
        HIGH_BYTE = 2,
        SEGMENT_TO_DELTA = 4,
        TRIMMED_TABLE = 6,
        MIXED = 8,
        TRIMMED_ARRAY = 10,
        SEGMENT_COVERAGE = 12,
        MANY_TO_ONE_RANGE = 13,
        UNICODE_VARIATION = 14,
        N = 15,
        REALLY_BAD = 55555,
    };
    constexpr auto TABLE_FORMAT_N = (unsigned)TableFormat::N;

}   // namespace mf
