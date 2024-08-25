#pragma once

#ifdef QT_CORE_LIB
    #include <QByteArray>
#endif

#include "u_Array.h"

///
/// \brief The Mems class
///   Simple memory stream; need stream-like bhv because of Motorola data
///
class Mems
{
public:
    Mems() noexcept = default;
    /// Ctor; by-val+move idiom
    Mems(Buf1d<char> dd) noexcept { borrowRW(dd); }
    Mems(Buf1d<const char> dd) noexcept { borrowR(dd); }
    /// Move only, do not copy
    Mems(const Mems&) = delete;
    Mems(Mems&&) noexcept = default;
    Mems& operator = (const Mems&) = delete;
    Mems& operator = (Mems&&) noexcept = default;

    void alloc(size_t n) { d.alloc(n); p = d.begin(); canWrite = true; }
    Buf1d<char> data() noexcept { return d; }
    Buf1d<const char> data() const noexcept { return d; }
    /// Borrow readable/writeable
    void borrowRW(Buf1d<char> dd) noexcept { d.borrow(dd); p = dd.begin(); canWrite = true; }
    /// Borrow readable
    void borrowR(Buf1d<const char> dd) noexcept
        { Buf1d<char>d1{ dd.size(), const_cast<char*>(dd.buffer()) };
          d.borrow(d1); p = d1.begin(); canWrite = false; }

#ifdef QT_CORE_LIB
    QByteArray qdata() const { return QByteArray::fromRawData(d.buffer(), d.size()); }
#endif

    /// @return  ptr to beginning
    char* beg() noexcept { return d.begin(); }
    const char* beg() const noexcept { return d.begin(); }

    /// @return  ptr to end
    char* end() { return d.end(); }
    const char* end() const { return d.end(); }

    /// @return  ptr to carriage
    char* ptr() noexcept { return p; }
    const char* ptr() const noexcept { return p; }

    /// @return  data size
    size_t size() const noexcept { return d.size(); }

    /// @return  carriage position
    size_t pos() const noexcept { return p - beg(); }

    /// @return  how many bytes remains
    size_t remainder() const noexcept { return end() - p; }

    /// Seeks carriage to position x
    /// @post  pos() == x
    /// @throw  too big x
    void seek(size_t x);

    /// Reads dsize bytes, native byte order
    /// @throw too big dsize
    void read(char* data, size_t dsize);

    /// Writes dsize bytes, native byte order
    /// @throw too big dsize
    void write(const char* data, size_t dsize);
    void write(std::string_view data) { write(data.data(), data.size()); }

    /// Helper function: reads struc of known length, native byte order
    template <size_t Len, class T>
    void readStruc(T& data)
    {
        static constexpr auto actualLen = sizeof(std::remove_reference_t<T>);
        static_assert(Len == actualLen, "Wrong Len, maybe mistake?");
        read(reinterpret_cast<char*>(&data), Len);
    }

    /// Helper function: writes struc of known length, native byte order
    template <size_t Len, class T>
    void writeStruc(const T& data)
    {
        static constexpr auto actualLen = sizeof(std::remove_reference_t<T>);
        static_assert(Len == actualLen, "Wrong Len, maybe mistake?");
        write(reinterpret_cast<const char*>(&data), Len);
    }

    /// reads one byte
    uint8_t readB() {
        uint8_t r;
        readStruc<1>(r);
        return r;
    }

    /// read Intel word (2 bytes)
    uint16_t readIW();

    /// read Motorola word (2 bytes)
    uint16_t readMW();

    /// read Intel dword (4 bytes)
    uint32_t readID();

    /// read Motorola dword (4 bytes)
    uint32_t readMD();

    /// write byte
    void writeB(uint8_t x) { writeStruc<1>(x); }

    /// write Motorola word (2 bytes)
    void writeMW(uint16_t x);

    /// write word (2 bytes) equal to 0 (Motorola=Intel here)
    void writeW0();

    /// write Motorola dword (4 bytes)
    void writeMD(uint32_t x);

    void skip(size_t dsize);

    /// Skip word (2 bytes)
    inline void skipW() { skip(2); }

    /// Skip dword (4 bytes)
    inline void skipD() { skip(4); }

protected:
    Array1d<char> d;
    char* p = nullptr;
    bool canWrite = false;
};
