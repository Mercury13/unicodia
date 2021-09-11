// My header
#include "i_MemStream.h"

// Libs
#include "i_ByteSwap.h"


uint16_t Mems::readIW()
{
    Word1 w;
    readStruc<2>(w);
    swapIW(w);
    return w.asWord;
}

uint16_t Mems::readMW()
{
    Word1 w;
    readStruc<2>(w);
    swapMW(w);
    return w.asWord;
}

uint32_t Mems::readID()
{
    Dword1 d;
    readStruc<4>(d);
    swapID(d);
    return d.asDword;
}

uint32_t Mems::readMD()
{
    Dword1 d;
    readStruc<4>(d);
    swapMD(d);
    return d.asDword;
}

void Mems::writeMD(uint32_t x)
{
    Dword1 d;
    d.asDword = x;
    swapMD(d);
    writeStruc<4>(d);
}

void Mems::seek(size_t x)
{
    if (x > size()) {
        char buf[100];
        snprintf(buf, sizeof(buf), "[Mems.seek] Seek past end: %llu/%llu, former pos=%llu",
                 static_cast<unsigned long long>(x), static_cast<unsigned long long>(size()),
                 static_cast<unsigned long long>(pos()));
        throw std::logic_error(buf);
    }
    x = std::min(x, size());
    p = beg() + x;
}

void Mems::read(char* data, size_t dsize)
{
    auto rem = remainder();
    if (dsize > rem) {
        char buf[100];
        snprintf(buf, sizeof(buf), "[Mems.read] Cannot read %llu bytes: pos=%llu/%llu, rem=%llu",
                 static_cast<unsigned long long>(dsize), static_cast<unsigned long long>(pos()),
                 static_cast<unsigned long long>(size()), static_cast<unsigned long long>(rem));
        throw std::logic_error(buf);
    }
    auto pNew = p + dsize;
    std::copy(p, pNew, data);
    p = pNew;
}

void Mems::write(const char* data, size_t dsize)
{
    auto rem = remainder();
    if (dsize > rem) {
        char buf[100];
        snprintf(buf, sizeof(buf), "[Mems.write] Cannot write %llu bytes: pos=%llu/%llu, rem=%llu",
                 static_cast<unsigned long long>(dsize), static_cast<unsigned long long>(pos()),
                 static_cast<unsigned long long>(size()), static_cast<unsigned long long>(rem));
        throw std::logic_error(buf);
    }
    auto pNew = p + dsize;
    p = std::copy(data, data + dsize, p);
    p = pNew;
}
