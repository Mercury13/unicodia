#include "FileWrap.h"

// C++
#include <algorithm>
#include <bit>


namespace {

    static_assert (sizeof(uint8_t) == 1, "Strange machine, uint8_t != 1");

    union Word1 {
        uint16_t asWord;
        uint8_t asBytes[2];
        char asChars[2];
    };
    static_assert (sizeof(Word1) == 2, "Strange machine: Word1 != 2");

    union Dword1 {
        uint32_t asDword;
        float asFloat;
        uint8_t asBytes[4];
        char asChars[4];
    };
    static_assert (sizeof(Dword1) == 4, "Strange machine: Dword1 != 4");

    static_assert(std::endian::native == std::endian::big
                || std::endian::native == std::endian::little,
                    "Strange machine: unknown endianness");

    [[maybe_unused]] inline void doSwapW(Word1& x) {
        std::swap(x.asBytes[0], x.asBytes[1]);
    }

    [[maybe_unused]] inline void swapMW(Word1& x) {
        if constexpr(std::endian::native == std::endian::little) {
            doSwapW(x);
        }
    }

    [[maybe_unused]] inline void swapIW(Word1& x) {
        if constexpr(std::endian::native == std::endian::big) {
            doSwapW(x);
        }
    }

    [[maybe_unused]] inline void doSwapD(Dword1& x) {
        std::swap(x.asBytes[0], x.asBytes[3]);
        std::swap(x.asBytes[1], x.asBytes[2]);
    }

    [[maybe_unused]] inline void swapMD(Dword1& x) {
        if constexpr(std::endian::native == std::endian::little) {
            doSwapD(x);
        }
    }

    [[maybe_unused]] inline void swapID(Dword1& x) {
        if constexpr(std::endian::native == std::endian::big) {
            doSwapD(x);
        }
    }

}   // anon namespace


void FileWrap::read(char* data, size_t size)
{
    long long nRead = f.read(data, size);
    if (static_cast<unsigned long long>(nRead) != size) {
        char buf[100];
        snprintf(buf, std::size(buf),
                "Cannot read %llu bytes, got only %lld",
                 static_cast<unsigned long long>(size),
                 nRead);
        throw std::logic_error(buf);
    }
}


void FileWrap::write(const char* data, size_t size)
{
    long long nWritten = f.write(data, size);
    if (static_cast<unsigned long long>(nWritten) != size) {
        char buf[100];
        snprintf(buf, std::size(buf),
                "Cannot write %llu bytes, did only %lld",
                 static_cast<unsigned long long>(size),
                 nWritten);
        throw std::logic_error(buf);
    }
}


uint16_t FileWrap::readIW()
{
    Word1 w;
    readStruc<2>(w);
    swapIW(w);
    return w.asWord;
}

uint16_t FileWrap::readMW()
{
    Word1 w;
    readStruc<2>(w);
    swapMW(w);
    return w.asWord;
}

uint32_t FileWrap::readID()
{
    Dword1 d;
    readStruc<4>(d);
    swapID(d);
    return d.asDword;
}

uint32_t FileWrap::readMD()
{
    Dword1 d;
    readStruc<4>(d);
    swapMD(d);
    return d.asDword;
}


void FileWrap::seek(long long x)
{
    if (!f.seek(x))
        throw std::logic_error("Cannot seek");
}


void FileWrap::writeMD(uint32_t x)
{
    Dword1 d;
    d.asDword = x;
    swapMD(d);
    writeStruc<4>(d);
}
