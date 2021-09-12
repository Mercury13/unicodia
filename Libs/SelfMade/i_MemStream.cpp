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

void Mems::writeMW(uint16_t x)
{
    Word1 d;
    d.asWord = x;
    swapMW(d);
    writeStruc<2>(d);
}

void Mems::writeMD(uint32_t x)
{
    Dword1 d;
    d.asDword = x;
    swapMD(d);
    writeStruc<4>(d);
}


namespace {

    void throwSeek(unsigned long long x,
                   unsigned long long size, unsigned long long formerPos)
    {
        char buf[100];
        snprintf(buf, sizeof(buf), "[Mems.seek] Seek past end: %llu/%llu, former pos=%llu",
                 static_cast<unsigned long long>(x), static_cast<unsigned long long>(size),
                 static_cast<unsigned long long>(formerPos));
        throw std::logic_error(buf);
    }

}   // anon namespace

void Mems::seek(size_t x)
{
    if (x > size())
        throwSeek(x, size(), pos());
    x = std::min(x, size());
    p = beg() + x;
}

namespace {

    void throwRead(unsigned long long dsize, unsigned long long pos,
                    unsigned long long size, unsigned long long rem)
    {
        char buf[100];
        snprintf(buf, sizeof(buf), "[Mems.read] Cannot read %llu bytes: pos=%llu/%llu, rem=%llu",
                 static_cast<unsigned long long>(dsize), static_cast<unsigned long long>(pos),
                 static_cast<unsigned long long>(size), static_cast<unsigned long long>(rem));
        throw std::logic_error(buf);
    }

}   // anon namespace


void Mems::read(char* data, size_t dsize)
{
    auto rem = remainder();
    if (dsize > rem)
        throwRead(dsize, pos(), size(), rem);
    auto pNew = p + dsize;
    std::copy(p, pNew, data);
    p = pNew;
}

namespace {

    void throwWrite(unsigned long long dsize, unsigned long long pos,
                    unsigned long long size, unsigned long long rem)
    {
        char buf[100];
        snprintf(buf, sizeof(buf), "[Mems.write] Cannot write %llu bytes: pos=%llu/%llu, rem=%llu",
                 static_cast<unsigned long long>(dsize), static_cast<unsigned long long>(pos),
                 static_cast<unsigned long long>(size), static_cast<unsigned long long>(rem));
        throw std::logic_error(buf);
    }

}   // anon namespace

void Mems::write(const char* data, size_t dsize)
{
    auto rem = remainder();
    if (dsize > rem)
        throwWrite(dsize, pos(), size(), rem);
    auto pNew = p + dsize;
    p = std::copy(data, data + dsize, p);
    p = pNew;
}

namespace {

    void throwSkip(unsigned long long dsize, unsigned long long pos,
                   unsigned long long size, unsigned long long rem)
    {
        char buf[100];
        snprintf(buf, sizeof(buf), "[Mems.write] Cannot skip %llu bytes: pos=%llu/%llu, rem=%llu",
                 static_cast<unsigned long long>(dsize), static_cast<unsigned long long>(pos),
                 static_cast<unsigned long long>(size), static_cast<unsigned long long>(rem));
        throw std::logic_error(buf);
    }

}   // anon namespace


void Mems::skip(size_t dsize)
{
    auto rem = remainder();
    if (dsize > rem)
        throwSkip(dsize, pos(), size(), rem);
    p += dsize;
}
