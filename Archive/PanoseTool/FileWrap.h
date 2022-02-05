#ifndef FILEWRAP_H
#define FILEWRAP_H

// C++
#include <cstdint>
#include <type_traits>

// Qt
#include <QFile>


class FileWrap
{
public:
    QIODevice& f;

    FileWrap(QIODevice& aF) : f(aF) {}
    void read(char* data, size_t size);
    void write(const char* data, size_t size);

    /// read structure of known size, native byte order
    /// @tparam  Len # of bytes to read
    template <size_t Len, class T>
    void readStruc(T& data)
    {
        static constexpr auto actualLen = sizeof(std::remove_reference_t<T>);
        static_assert(Len == actualLen, "Wrong Len, maybe mistake?");
        read(reinterpret_cast<char*>(&data), Len);
    }

    /// write structure of known size, native byte order
    /// @tparam  Len # of bytes to write
    template <size_t Len, class T>
    void writeStruc(const T& data)
    {
        static constexpr auto actualLen = sizeof(std::remove_reference_t<T>);
        static_assert(Len == actualLen, "Wrong Len, maybe mistake?");
        write(reinterpret_cast<const char*>(&data), Len);
    }

    /// read byte
    uint8_t readB() {
        uint8_t r;
        readStruc<1>(r);
        return r;
    }

    /// read Intel word
    uint16_t readIW();

    /// read Motorola word
    uint16_t readMW();

    /// read Intel dword
    uint32_t readID();

    /// read Motorola dword
    uint32_t readMD();

    /// read Motorola dword
    void writeMD(uint32_t x);

    /// skip dword
    void skipD() {
        uint32_t noMatter;
        readStruc<4>(noMatter);
    }

    long long size() const { return f.size(); }
    void seek(long long x);
    long long pos() const { return f.pos(); }
};

#endif // FILEWRAP_H
