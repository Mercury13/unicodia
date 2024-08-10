#pragma once

// C++
#include <cstdint>

// STL
#include <string_view>

// Libs
#include "u_Array.h"
#include "i_MemStream.h"
#include "u_Vector.h"

// Qt
#include <QIODevice>

namespace mf {

    enum class Type { FONT, BLOCK };

    class Obj { // interface
    public:
        virtual Type type() const noexcept = 0;
        virtual size_t nChildren() const noexcept = 0;
        virtual const Obj& childAt(size_t i) const = 0;
        virtual std::string text() const noexcept = 0;
        // Need virtual dtor
        virtual ~Obj() = default;
    };

    struct Char4
    {
        union {
            char asChars[4] { 0, 0, 0, 0 };
            uint32_t asInt;
        } d;

        constexpr Char4() = default;
        constexpr Char4(const char (&x)[5])
            : d {{ x[0], x[1], x[2], x[3] }} {}
        std::string_view toSv() const { return { d.asChars, 4}; }
    };

    constexpr bool operator == (Char4 x, Char4 y) { return x.d.asInt == y.d.asInt; }
    constexpr bool operator != (Char4 x, Char4 y) { return x.d.asInt != y.d.asInt; }

    class Block : public mf::Obj
    {
    public:
        Char4 name;
        uint32_t posInDir = 0, posInFile = 0, length = 0;

        Buf1d<char> toBuf(Buf1d<char> data);

        Type type() const noexcept override { return Type::BLOCK; }
        size_t nChildren() const noexcept override { return 0; }
        const Obj& childAt(size_t i) const override;
        std::string text() const noexcept override
            { return std::string { name.toSv() }; };
    };

    struct Block2
    {
        const Block* b = nullptr;
        Buf1d<char> d;

        operator bool() const { return b; }
    };

}   // namespace mf


class MemFont : public mf::Obj
{
public:
    bool load(const QString& fname);
    bool load(QIODevice& f);

    // High-level bhv
    mf::Block2 findBlock(mf::Char4 name);
    mf::Block2 rqBlock(mf::Char4 name, uint32_t len = 0);
    /// @param bytes  ASCII only!!
    void mangle(std::string_view bytes);
    /// prefer over nChildren
    size_t nBlocks() const { return blocks.size(); }
    auto qdata() const { return slave.qdata(); }
    Mems& stream() & { return slave; }
    Mems&& stream() && { return std::move(slave); }
    Mems&& giveStream() { return std::move(slave); }

    mf::Type type() const noexcept override { return mf::Type::FONT; }
    size_t nChildren() const noexcept override { return blocks.size(); }
    virtual const mf::Block& childAt(size_t i) const override;
    std::string text() const noexcept override { return "Font"; }
private:
    Mems slave;
    SafeVector<mf::Block> blocks;

    bool readDir();
    mf::Block readBlockEntry();
    void recomputeChecksum(const mf::Block& b);
};
